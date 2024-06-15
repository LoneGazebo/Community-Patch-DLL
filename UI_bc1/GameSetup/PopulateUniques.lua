-------------------------------------------------
-- created by bc1 / compatible with 1.0.3.144 code
-------------------------------------------------
if not EUI then
	include( "EUI_utilities" )
end
local pairs = pairs
local pcall = pcall
local string_format = string.format
local math_max = math.max
local math_ceil = math.ceil

local IconLookup = EUI.IconLookup
local IconHookup = EUI.IconHookup
include( "EUI_tooltips" )
local GetHelpTextForUnit = EUI.GetHelpTextForUnit
local GetHelpTextForBuilding = EUI.GetHelpTextForBuilding
local GetHelpTextForImprovement = EUI.GetHelpTextForImprovement
--[[
Assumptions:
CivilopediaControl is set before include statement with string of popup control
instance IconInstance, parent to Portrait image
Icons stack = where each IconInstance goes
- UU/UB/UI are determined from civ.Type
- civ icon Portrait:
	hooked to civ.Portrait & civ.IndexIconAtlas
	tooltip is civ.Description
	pedia callback to civ.Pedia or civ.ShortDescription
Button = parent to Icons stack, set to same height
	pedia callback to civ.LeaderPedia or civ.LeaderDescription
Portrait = leader image specified in civ.LeaderPortraitIndex, civ.LeaderIconAtlas
Description = trait.Description
Title = civ.LeaderDescription, civ.ShortDescription, trait.ShortDescription
(trait is determined from civ.LeaderType)
Anim = height set to Button height + 2
--]]

local bnw_mode = ContentManager.IsActive("6DA07636-4123-4018-B643-6575B4EC336B", ContentType.GAMEPLAY);
local g_traitsQuery, g_uniqueUnitsQuery, g_uniqueBuildingsQuery, g_uniqueImprovementsQuery;
local g_questionMark =	{ PortraitIndex = 23, IconAtlas = "CIV_COLOR_ATLAS" };
local g_questionMarkTip = Locale.ConvertTextKey( "TXT_KEY_MISC_UNKNOWN" );
local g_randomTrait =	{ Description = "TXT_KEY_RANDOM_LEADER_HELP", ShortDescription = "TXT_KEY_MISC_RANDOMIZE" };
local g_randomCiv =	{	ID=-1,
				LeaderPortraitIndex = 22,
				LeaderIconAtlas = "LEADER_ATLAS",
				LeaderDescription = "TXT_KEY_RANDOM_LEADER",
				ShortDescription = "TXT_KEY_RANDOM_CIV",
			};--g_randomCiv

local function initializePopulateCivilizationUniques()

	g_traitsQuery = DB.CreateQuery([[SELECT Description, ShortDescription FROM Traits inner join
					Leader_Traits ON Traits.Type = Leader_Traits.TraitType
					WHERE Leader_Traits.LeaderType = ? LIMIT 1]]);

	g_uniqueUnitsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Units INNER JOIN
					Civilization_UnitClassOverrides ON Units.Type = Civilization_UnitClassOverrides.UnitType
					WHERE Civilization_UnitClassOverrides.CivilizationType = ? AND
					Units.ShowInPedia IS NOT 0 AND
					Civilization_UnitClassOverrides.UnitType IS NOT NULL]]);

	g_uniqueBuildingsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Buildings INNER JOIN
					Civilization_BuildingClassOverrides ON Buildings.Type = Civilization_BuildingClassOverrides.BuildingType
					WHERE Civilization_BuildingClassOverrides.CivilizationType = ? AND
					Civilization_BuildingClassOverrides.BuildingType IS NOT NULL]]
					.. ( bnw_mode and " AND Buildings.GreatWorkCount IS NOT -1" or "" ) );

	g_uniqueImprovementsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Improvements
					WHERE CivilizationType = ?]]);
end

-----------------
-- Pedia Callback
-----------------
local getPedia;
local function getPediaB( ... )
	Events.SearchForPediaEntry( ... )
end
local function getPediaA( ... )
	UIManager:QueuePopup( LookUpControl( CivilopediaControl ), PopupPriority.eUtmost );
	getPedia = getPediaB;
	getPedia( ... )
end
getPedia = CivilopediaControl and getPediaA;

----------------
-- Icon Creation
----------------
local function newBlankIcon( parentControl )
	local iconControls = {};
	ContextPtr:BuildInstanceForControl( "IconInstance", iconControls, parentControl );
	return iconControls;
end

local function populateIcon( controls, item, isTip, tip, pedia )
	local thisText = controls.Text;
	local thisIcon = controls.Portrait;
	local description = item.Description;
	-- set the item picture
	local textureOffset, textureAtlas = IconLookup( item.PortraitIndex, thisIcon:GetSizeX(), item.IconAtlas );
	if textureAtlas then
		thisIcon:SetTexture( textureAtlas );
		thisIcon:SetTextureOffset( textureOffset );
	end
	-- set the item tooltip
	if isTip then
		thisIcon:SetToolTipString( tip )
	elseif description then
		thisIcon:LocalizeAndSetToolTip( description );
	end
	-- set the item description
	if thisText and description then
		thisText:LocalizeAndSetText( description );
	end
	-- set the pedia callback
	pedia = CivilopediaControl and ( pedia or description );
	if pedia then
		thisIcon:RegisterCallback( Mouse.eRClick, function() getPedia( pedia ) end );
	end
end

local function newItemIcon( parentControl, ... )
	populateIcon( newBlankIcon( parentControl ), ... )
end

local function populateUniques( parentControl, civType )
	-- UU icons
	for unit in g_uniqueUnitsQuery( civType ) do
		newItemIcon( parentControl, unit, pcall( GetHelpTextForUnit, unit.ID, true ) );
	end
	-- UB icons
	for building in g_uniqueBuildingsQuery( civType ) do
		newItemIcon( parentControl, building, pcall( GetHelpTextForBuilding, building.ID ) );
	end
	-- UI icons
	for improvement in g_uniqueImprovementsQuery( civType ) do
		newItemIcon( parentControl, improvement, pcall( GetHelpTextForImprovement, improvement.ID ) );
	end
end

-----------------
-- Populate Entry
-----------------
local function populateCivilizationUniquesInitialized( civControls, civKeys )

	local trait;
	local civ = {};
	local iconParentControl = civControls.Icons;
	if civKeys then
		for k, v in pairs(civKeys) do
			civ[k] = v;
		end
		-- Civ icon
		newItemIcon( iconParentControl, civ, nil, nil, civ.Pedia or civ.ShortDescription );
		-- UU, UB, UI icons
		populateUniques( iconParentControl, civ.Type )
		-- Leader
		local pedia = CivilopediaControl and ( civ.LeaderPedia or civ.LeaderDescription );
		if pedia then
			civControls.Button:RegisterCallback( Mouse.eRClick, function() getPedia( pedia ) end );
		end
		trait = g_traitsQuery( civ.LeaderType )();
--		trait = GameInfo.Traits[ GameInfo.Leader_Traits{ LeaderType = civ.LeaderType )().TraitType ];
	else
		-- Question mark icons
		for i = 1, 3 do
			newItemIcon( iconParentControl, g_questionMark, true, g_questionMarkTip );
		end
		trait = g_randomTrait;
		civ = g_randomCiv;
	end
	local n = iconParentControl:GetNumChildren()
	iconParentControl:SetWrapWidth( math_ceil( n / math_ceil( n / 4 ) ) * 56 )
	iconParentControl:CalculateSize();
	iconParentControl:ReprocessAnchoring();
	civControls.Button:SetVoid1( civ.ID );
	civControls.Description:LocalizeAndSetText( trait.Description );
	civControls.Title:SetText( string_format("%s (%s)",
			Locale.ConvertTextKey( "TXT_KEY_RANDOM_LEADER_CIV", civ.LeaderDescription, civ.ShortDescription ),
			Locale.ConvertTextKey( trait.ShortDescription ) ) );
	IconHookup( civ.LeaderPortraitIndex, civControls.Portrait:GetSizeX(), civ.LeaderIconAtlas, civControls.Portrait );
	local height = math_max( 100, iconParentControl:GetSizeY() + 8 );
	civControls.Button:SetSizeY( height );
	civControls.Anim:SetSizeY( height + 4 );
	civControls.Button:ReprocessAnchoring();
end

-------------------
-- Export Functions
-------------------
InitializePopulateUniques = initializePopulateCivilizationUniques;
PopulateUniquesForSelectCivilization = populateCivilizationUniquesInitialized;
PopulateUniquesForGameSetup = populateCivilizationUniquesInitialized;
PopulateUniquesForGameLoad = populateUniques;

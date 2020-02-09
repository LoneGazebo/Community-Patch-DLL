-------------------------------------------------
-- Include file that has handy stuff for the tech tree and other screens that need to show a tech button
-------------------------------------------------
include( "FLuaVector" ) --compatibility with Firaxis scripts
include( "EUI_utilities" )
IconLookup = EUI.IconLookup
IconHookup = EUI.IconHookup
CivIconHookup = EUI.CivIconHookup
SimpleCivIconHookup = EUI.SimpleCivIconHookup
local IconHookup = EUI.IconHookup
local table = EUI.table
local YieldIcons = EUI.YieldIcons
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query

include( "EUI_tooltips" )
local GetHelpTextForUnit = EUI.GetHelpTextForUnit
local GetHelpTextForBuilding = EUI.GetHelpTextForBuilding
local GetHelpTextForImprovement = EUI.GetHelpTextForImprovement
local GetHelpTextForProject = EUI.GetHelpTextForProject
local GetHelpTextForPlayerPerk = EUI.GetHelpTextForPlayerPerk -- BE only
--CBP
local GetHelpTextForCorp = EUI.GetHelpTextForCorp
--END

local civ5_mode = type( MouseOverStrategicViewResource ) == "function"
local civBE_mode = type( Game.GetAvailableBeliefs ) == "function"
local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil
local civ5_bnw_mode = bnw_mode and civ5_mode

-------------------------------
-- minor lua optimizations
-------------------------------

local pairs = pairs
local ipairs = ipairs
local tonumber = tonumber
local tostring = tostring
local unpack = unpack
local table_insert = table.insert

local UI = UI
--local UIManager = UIManager
--local Controls = Controls
--local ContextPtr = ContextPtr
--local Players = Players
--local Teams = Teams
--local GameInfo = GameInfo
--local GameInfoActions = GameInfoActions
local GameInfoTypes = GameInfoTypes
--local GameDefines = GameDefines
--local InterfaceDirtyBits = InterfaceDirtyBits
--local CityUpdateTypes = CityUpdateTypes
--local ButtonPopupTypes = ButtonPopupTypes
--local YieldTypes = YieldTypes
--local GameOptionTypes = GameOptionTypes
local DomainTypes = DomainTypes
--local FeatureTypes = FeatureTypes
--local FogOfWarModeTypes = FogOfWarModeTypes
--local OrderTypes = OrderTypes
--local PlotTypes = PlotTypes
--local TerrainTypes = TerrainTypes
--local InterfaceModeTypes = InterfaceModeTypes
--local NotificationTypes = NotificationTypes
--local ActivityTypes = ActivityTypes
--local MissionTypes = MissionTypes
--local ActionSubTypes = ActionSubTypes
--local GameMessageTypes = GameMessageTypes
--local TaskTypes = TaskTypes
--local CommandTypes = CommandTypes
--local DirectionTypes = DirectionTypes
--local DiploUIStateTypes = DiploUIStateTypes
--local FlowDirectionTypes = FlowDirectionTypes
--local PolicyBranchTypes = PolicyBranchTypes
--local FromUIDiploEventTypes = FromUIDiploEventTypes
--local CoopWarStates = CoopWarStates
--local ThreatTypes = ThreatTypes
--local DisputeLevelTypes = DisputeLevelTypes
--local LeaderheadAnimationTypes = LeaderheadAnimationTypes
--local TradeableItems = TradeableItems
--local EndTurnBlockingTypes = EndTurnBlockingTypes
--local ResourceUsageTypes = ResourceUsageTypes
--local MajorCivApproachTypes = MajorCivApproachTypes
--local MinorCivTraitTypes = MinorCivTraitTypes
--local MinorCivPersonalityTypes = MinorCivPersonalityTypes
--local MinorCivQuestTypes = MinorCivQuestTypes
--local CityAIFocusTypes = CityAIFocusTypes
--local AdvisorTypes = AdvisorTypes
--local GenericWorldAnchorTypes = GenericWorldAnchorTypes
--local GameStates = GameStates
--local GameplayGameStateTypes = GameplayGameStateTypes
--local CombatPredictionTypes = CombatPredictionTypes
--local ChatTargetTypes = ChatTargetTypes
--local ReligionTypes = ReligionTypes
--local BeliefTypes = BeliefTypes
--local FaithPurchaseTypes = FaithPurchaseTypes
--local ResolutionDecisionTypes = ResolutionDecisionTypes
--local InfluenceLevelTypes = InfluenceLevelTypes
--local InfluenceLevelTrend = InfluenceLevelTrend
--local PublicOpinionTypes = PublicOpinionTypes
--local ControlTypes = ControlTypes

--local PreGame = PreGame
--local Game = Game
--local Map = Map
--local OptionsManager = OptionsManager
local Events = Events
local Mouse = Mouse
--local MouseEvents = MouseEvents
--local MouseOverStrategicViewResource = MouseOverStrategicViewResource
local Locale = Locale
local L = Locale.ConvertTextKey
--getmetatable("").__index.L = L

techPediaSearchStrings = {}	-- needs to be GLOBAL
g_searchTable = {}		-- GLOBAL Holds mapping of searchable words to techs for Civ BE (unused in this script)
g_recentlyAddedUnlocks = {}	-- needs to be GLOBAL

turnsString = L("TXT_KEY_TURNS")
freeString = L("TXT_KEY_FREE")
lockedString = "[ICON_LOCKED]" --L("TXT_KEY_LOCKED")

local g_AffinityInfo = civBE_mode and {
	AFFINITY_TYPE_PURITY	= { 0, "AFFINITY_ATLAS_TECHWEB", "TXT_KEY_TECHWEB_AFFINITY_ADDS_PURITY" },
	AFFINITY_TYPE_SUPREMACY	= { 1, "AFFINITY_ATLAS_TECHWEB", "TXT_KEY_TECHWEB_AFFINITY_ADDS_SUPREMACY" },
	AFFINITY_TYPE_HARMONY	= { 2, "AFFINITY_ATLAS_TECHWEB", "TXT_KEY_TECHWEB_AFFINITY_ADDS_HARMONY" },
}

local g_buttonItemInfo = {}
local validUnitBuilds = {}
local validBuildingBuilds = {}
local validImprovementBuilds = {}
-- C4DF
local g_bResearchAgreements = Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS");
local g_bNoTechTrading = Game.IsOption("GAMEOPTION_NO_TECH_TRADING");
local g_bNoVassalage = Game.IsOption("GAMEOPTION_NO_VASSALAGE");
-- END

function GetTechPedia( techID, void2, button )
	local row = g_buttonItemInfo[tostring(button)]
	local rowType = row and row.Type or ""
	if rowType:sub(1,6) == "BUILD_" then
		if row.RouteType then
			row = GameInfo.Routes[ row.RouteType or -1 ]
		elseif row.ImprovementType then
			row = GameInfo.Improvements[ row.ImprovementType or -1 ]
		else -- we are a choppy thing
			row = GameInfo.Concepts.CONCEPT_WORKERS_CLEARINGLAND
		end
	elseif rowType:sub(1,7) =="MISSION_" then
-- todo		row = GameInfo.Technologies[techID]

	elseif civBE_mode and rowType:sub(1,10) == "TERRAFORM_" then
		if rowType == "TERRAFORM_ADD_MIASMA" then
			row = GameInfo.Concepts.CONCEPT_WORKERS_PLACE
		elseif rowType == "TERRAFORM_CLEAR_MIASMA" then
			row = GameInfo.Concepts.CONCEPT_WORKERS_REMOVE
		elseif row.FeatureTypeChange then
			row = GameInfo.Features[ row.FeatureTypeChange or -1 ]
		elseif row.TerrainTypeChange then
			row = GameInfo.Terrains[ row.TerrainTypeChange or -1 ]
		end
	end
	Events.SearchForPediaEntry( row and row.Description or techPediaSearchStrings[tostring(button)] )
end
local GetTechPedia = GetTechPedia

function GatherInfoAboutUniqueStuff( civType )

	validUnitBuilds = {}
	validBuildingBuilds = {}
	validImprovementBuilds = {}

	-- put in the default units for any civ
	for thisUnitClass in GameInfo.UnitClasses() do
		validUnitBuilds[thisUnitClass.Type] = thisUnitClass.DefaultUnit
	end

	-- put in my overrides
	for thisOverride in GameInfo.Civilization_UnitClassOverrides() do
		if thisOverride.CivilizationType == civType then
			validUnitBuilds[thisOverride.UnitClassType] = thisOverride.UnitType
		end
	end

	-- put in the default buildings for any civ
	for thisBuildingClass in GameInfo.BuildingClasses() do
		validBuildingBuilds[thisBuildingClass.Type] = thisBuildingClass.DefaultBuilding
	end

	-- put in my overrides
	for thisOverride in GameInfo.Civilization_BuildingClassOverrides() do
		if thisOverride.CivilizationType == civType then
			validBuildingBuilds[thisOverride.BuildingClassType] = thisOverride.BuildingType
		end
	end

	-- add in support for unique improvements
	for thisImprovement in GameInfo.Improvements() do
		if not thisImprovement.CivilizationType or thisImprovement.CivilizationType == civType then
			validImprovementBuilds[thisImprovement.Type] = thisImprovement.Type
		end
	end

	-- kludge to prevent TECH PANEL opening upon ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH
	if OnOpenInfoCorner and OnPopup then
		Events.SerialEventGameMessagePopup.Remove( OnPopup )
		OnPopup = nil
	end
end

local function adjustArtOnButton( toolTipCallBack, button, row, textureSize, portraitOffset, portraitAtlas )
	if button then
		g_buttonItemInfo[tostring(button)] = row
		button:RegisterCallback( Mouse.eRClick, GetTechPedia )
		button:RegisterCallback( Mouse.eMouseEnter, toolTipCallBack )
		if (Game.AnyoneHasWonder(row.ID) and GameInfo.Buildings[row.Type] and GameInfo.BuildingClasses[row.BuildingClass].MaxGlobalInstances == 1) then
			button:SetAlpha(0.33);
		else
			button:SetAlpha(1);
		end
		return IconHookup( portraitOffset or 0, textureSize, portraitAtlas or "GENERIC_FUNC_ATLAS", button )
	end
end

local function unitToolTip( techID, void2, button )

	return button:SetToolTipString( GetHelpTextForUnit( g_buttonItemInfo[tostring(button)].ID, true ) )
end

function AdjustArtOnGrantedUnitButton( button, row, textureSize )

	return adjustArtOnButton( unitToolTip, button, row, textureSize, UI.GetUnitPortraitIcon(row.ID) )
end
local AdjustArtOnGrantedUnitButton = AdjustArtOnGrantedUnitButton

local function buildingToolTip( techID, void2, button )

	return button:SetToolTipString( GetHelpTextForBuilding( g_buttonItemInfo[tostring(button)].ID ) )
end

function AdjustArtOnGrantedBuildingButton( button, row, textureSize )

	return adjustArtOnButton( buildingToolTip, button, row, textureSize, row.PortraitIndex, row.IconAtlas )
end
local AdjustArtOnGrantedBuildingButton = AdjustArtOnGrantedBuildingButton

local function projectToolTip( techID, void2, button )

	return button:SetToolTipString( GetHelpTextForProject( g_buttonItemInfo[tostring(button)].ID, nil, true ) )
end

function AdjustArtOnGrantedProjectButton( button, row, textureSize )

	return adjustArtOnButton( projectToolTip, button, row, textureSize, row.PortraitIndex, row.IconAtlas )
end
local AdjustArtOnGrantedProjectButton = AdjustArtOnGrantedProjectButton

-- CORP
local function corpToolTip( techID, void2, button )

	return button:SetToolTipString( GetHelpTextForCorp( g_buttonItemInfo[tostring(button)].ID, true ) )
end

function AdjustArtOnGrantedCorpButton( button, row, textureSize )

	return adjustArtOnButton( corpToolTip, button, row, textureSize, row.PortraitIndex, row.IconAtlas )
end
local AdjustArtOnGrantedCorpButton = AdjustArtOnGrantedCorpButton
--END

local function resourceToolTip( techID, void2, button )

	return button:LocalizeAndSetToolTip( "TXT_KEY_REVEALS_RESOURCE_ON_MAP", g_buttonItemInfo[tostring(button)].Description )
--todo: provide more info
end

function AdjustArtOnGrantedResourceButton( button, row, textureSize )

	return adjustArtOnButton( resourceToolTip, button, row, textureSize, row.PortraitIndex, row.IconAtlas )
end
local AdjustArtOnGrantedResourceButton = AdjustArtOnGrantedResourceButton

local function actionToolTip( techID, void2, button )
	local row = g_buttonItemInfo[tostring(button)]
	local improvementID = row and GameInfoTypes[ row.ImprovementType or -1 ]
	if improvementID then
		button:SetToolTipString( GetHelpTextForImprovement( improvementID ) )
	else
		button:LocalizeAndSetToolTip( row.Description )
	end
end

function AdjustArtOnGrantedActionButton( button, row, textureSize )

	return adjustArtOnButton( actionToolTip, button, row, textureSize, row.IconIndex, row.IconAtlas )
end
local AdjustArtOnGrantedActionButton = AdjustArtOnGrantedActionButton
AdjustArtOnGrantedImprovementButton = AdjustArtOnGrantedActionButton

-- civBE_mode only
local function playerPerkToolTip( techID, void2, button )

	return button:SetToolTipString( GetHelpTextForPlayerPerk( g_buttonItemInfo[tostring(button)].ID, true ) )
end

function AdjustArtOnGrantedPlayerPerkButton( button, row, textureSize )

	return adjustArtOnButton( playerPerkToolTip, button, row, textureSize )
end
local AdjustArtOnGrantedPlayerPerkButton = AdjustArtOnGrantedPlayerPerkButton

local function AdjustArtOnGrantedItem( button, row, textureSize, toolTip, pedia )
	if button then
		button:SetToolTipString( tostring(toolTip) )
		button:ClearCallback( Mouse.eMouseEnter )
		g_buttonItemInfo[tostring(button)] = pedia or row
		button:RegisterCallback( Mouse.eRClick, GetTechPedia )
		return IconHookup( row.IconIndex or 0, textureSize, row.IconAtlas or "GENERIC_FUNC_ATLAS", button )
	end
end

-- ===========================================================================
--	Has a few assumptions:
--		1.) the small buttons are named "B1", "B2", "B3"
--		2.) GatherInfoAboutUniqueStuff() has been called before this
--
--	ARGS:
--	thisTechButtonInstance,	UI element
--	tech,					data structure with technology info
--	maxSmallButtonSize		no more than this many buttons will be populated
--	textureSize
--	startingButtoNum,		(optional) 1, but will use this instead if set
--
--	RETURNS: the # of small buttons added
-- ===========================================================================

function AddSmallButtonsToTechButton( thisTechButtonInstance, tech, maxSmallButtons, textureSize, startingButtonNum )

	local techType = tech and tech.Type

	if not techType then
		return 0
	end

	-- temporary used (e.g., search populating)
	g_recentlyAddedUnlocks = {}
	local buttonNum = startingButtonNum or 1
	local thisPrereqTech = { PrereqTech = techType }
	local thisTechPrereq = { TechPrereq = techType }
	local thisTechType = { TechType = techType }

	local function addSmallButtonAndPedia( pedia, index, atlas, ... )
		local button = thisTechButtonInstance["B"..buttonNum]
		if button then
			IconHookup( index or 0, textureSize, atlas or "GENERIC_FUNC_ATLAS", button )
			if ... then
				button:LocalizeAndSetToolTip( ... )
			else
				button:SetToolTipString( "<game or mod database error>" )
			end
			button:SetText("")
			g_buttonItemInfo[tostring(button)] = pedia
			button:RegisterCallback( Mouse.eRClick, GetTechPedia )
			button:ClearCallback( Mouse.eMouseEnter )
			buttonNum = buttonNum + 1
			return button
		end
	end
	local function addSmallButton( ... )
		return addSmallButtonAndPedia( tech, ... )
	end

	local function addSmallGenericButton( ... )
		return addSmallButtonAndPedia( tech, 0, "GENERIC_FUNC_ATLAS", ... )
	end
	local function addSmallGenericButtonIf( ... )
		if ... then
			return addSmallButtonAndPedia( tech, 0, "GENERIC_FUNC_ATLAS", ... )
		end
	end
	local function addSmallArtButton( adjustArtFunc, row, ... ) -- ... is used only for AdjustArtOnGrantedItem
		if row then
			local button = thisTechButtonInstance["B"..buttonNum]
			if button then
				table_insert( g_recentlyAddedUnlocks, row.Description )
				button:SetText("")
				adjustArtFunc( button, row, textureSize, ... )
				buttonNum = buttonNum + 1
				return button
			end
		else
			return addSmallGenericButton( ... )
		end
	end

	-- CBP
	local function addSmallArtYieldButton(row, icons, toolTip)
		if row then
			local button = thisTechButtonInstance["B"..buttonNum]
			if button then
				table.insert( g_recentlyAddedUnlocks, row.Description )
				button:SetToolTipString( toolTip )
				button:ClearCallback( Mouse.eMouseEnter )
				g_buttonItemInfo[tostring(button)] = row
				button:RegisterCallback( Mouse.eRClick, GetTechPedia )
				IconHookup( row.PortraitIndex, textureSize, row.IconAtlas or "GENERIC_FUNC_ATLAS", button )
				buttonNum = buttonNum + 1
				button:GetTextControl():SetOffsetY( textureSize/3 )
				button:GetTextControl():SetAlpha( 0.8 )
				button:SetText( icons or "" )
				return button
			end
		end
	end
	-- END

	local function addSmallActionButton( build, icons, ... )
		local button
		if build then
			button = addSmallArtButton( AdjustArtOnGrantedItem, build, L(...) )
		else
			button = addSmallGenericButton( ... )
		end
		if button then
			button:GetTextControl():SetOffsetY( textureSize/3 )
			button:GetTextControl():SetAlpha( 0.8 )
			button:SetText( icons or "" )
			return button
		end
	end

	if civBE_mode then
		-- hide icon underlays
		for i = 1, maxSmallButtons do
			local iconUnderlay = thisTechButtonInstance["IconUnderlay"..i]
			if iconUnderlay then
				iconUnderlay:SetHide(true)
			else
				break
			end
		end
		-- If an affinity exists, wire it up as the first small button
		local textureSizeSave = textureSize
		textureSize = 64
		for tech in GameInfo.Technology_Affinities( thisTechType ) do
			local affinityInfo = g_AffinityInfo[ tech.AffinityType ]
			if affinityInfo then
				local button = addSmallButtonAndPedia( GameInfo.Concepts[ (GameInfo.Affinity_Types[ tech.AffinityType or -1 ] or {}).CivilopediaConcept or -1 ], unpack( affinityInfo ) )
				if button then
					if textureSizeSave == 45 then
						button:SetTextureOffsetVal( affinityInfo[1]*64+6,12 )
					end
				else
					break
				end
			end
		end
		textureSize = textureSizeSave
	end

	-- units unlocked by this tech
	for row in GameInfo.Units( thisPrereqTech ) do
		local unitType = row.Type
		if(unitType) then
			local iMinorCivGift = row.MinorCivGift
			if (iMinorCivGift ~= 1) and (validUnitBuilds[row.Class] == unitType ) then
				addSmallArtButton( AdjustArtOnGrantedUnitButton, row )
			end
		end
	end

	-- buildings and wonders unlocked by this tech
	for row in GameInfo.Buildings( thisPrereqTech ) do
		if validBuildingBuilds[row.BuildingClass] == row.Type and not addSmallArtButton( AdjustArtOnGrantedBuildingButton, row ) then
			break
		end
	end

	-- resources revealed by this tech
	for row in GameInfo.Resources{ TechReveal = techType } do
		if not addSmallArtButton( AdjustArtOnGrantedResourceButton, row ) then
			break
		end
	end

	-- projects unlocked by this tech
	for row in GameInfo.Projects( thisTechPrereq ) do
		if not addSmallArtButton( AdjustArtOnGrantedProjectButton, row ) then
			break
		end
	end

	-- actions enabled by this tech (usually only workers can do these)
	for row in GameInfo.Builds( thisPrereqTech ) do
		local improvementType = row.ImprovementType
		if ( not civ5_bnw_mode or row.ShowInTechTree )
			and (not improvementType or validImprovementBuilds[improvementType] == improvementType )
			and not addSmallArtButton( AdjustArtOnGrantedActionButton, row, actionToolTip )
		then
			break
		end
	end

	-- processes unlocked by this tech
	for row in GameInfo.Processes( thisTechPrereq ) do
		if not addSmallButton( row.PortraitIndex, row.IconAtlas, "TXT_KEY_ENABLE_PRODUCITON_CONVERSION", row.Description ) then
			break
		end
	end

	-- todo: need to add abilities, etc.
	-- Player Perk unlocks
	if civBE_mode then
		for row in GameInfo.Technology_FreePlayerPerks( thisTechType ) do
			if not addSmallArtButton( AdjustArtOnGrantedPlayerPerkButton, GameInfo.PlayerPerks[row.PlayerPerkType or -1] ) then
				break
			end
		end
	end

	for row in GameInfo.Route_TechMovementChanges( thisTechType ) do
		if not addSmallGenericButton( "TXT_KEY_FASTER_MOVEMENT", (GameInfo.Routes[row.RouteType] or {}).Description or "???" ) then
			break
		end
	end

	for row in GameInfo.Build_TechTimeChanges(thisTechType) do
		if not addSmallGenericButton( "TXT_KEY_BUILD_COST_REDUCTION", GameInfo.Builds[row.BuildType].Description, row.TimeChange/100 ) then
			break
		end
	end


	local playerID = Game.GetActivePlayer();	
	local player = Players[playerID];
	local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;

	if (tonumber(tech.EmbarkedMoveChange) or 0) > 0 then
		addSmallActionButton( GameInfo.Missions.MISSION_EMBARK, "[ICON_MOVES]", "TXT_KEY_FASTER_EMBARKED_MOVEMENT" )
	end

	if tech.AllowsEmbarking then
		addSmallActionButton( GameInfo.Missions.MISSION_EMBARK, "", "TXT_KEY_ALLOWS_EMBARKING" )
	end

	if civ5_mode then
		if tech.AllowsDefensiveEmbarking then
			addSmallActionButton( GameInfo.Missions.MISSION_EMBARK, "[ICON_STRENGTH]", "TXT_KEY_ABLTY_DEFENSIVE_EMBARK_STRING" )
		end

		addSmallGenericButtonIf( tech.EmbarkedAllWaterPassage and "TXT_KEY_ALLOWS_CROSSING_OCEANS" )
	end
	if gk_mode then
		addSmallGenericButtonIf( (tonumber(tech.UnitFortificationModifier) or 0) > 0 and "TXT_KEY_UNIT_FORTIFICATION_MOD", tech.UnitFortificationModifier )

		addSmallGenericButtonIf( (tonumber(tech.UnitBaseHealModifier) or 0) > 0 and "TXT_KEY_UNIT_BASE_HEAL_MOD", tech.UnitBaseHealModifier )

		addSmallGenericButtonIf( civ5_mode and tech.AllowEmbassyTradingAllowed and "TXT_KEY_ALLOWS_EMBASSY" )
	end
	if civ5_mode then
		addSmallGenericButtonIf( tech.OpenBordersTradingAllowed and "TXT_KEY_ALLOWS_OPEN_BORDERS" )

		addSmallGenericButtonIf( tech.DefensivePactTradingAllowed and "TXT_KEY_ALLOWS_DEFENSIVE_PACTS" )

		-- C4DF: Tech grants map trading
		addSmallGenericButtonIf( tech.MapTrading and "TXT_KEY_ALLOWS_MAP_TRADING" )
		
		-- Tech grants tech trades and tech trades aren't disabled
		addSmallGenericButtonIf( (tech.TechTrading and not g_bNoTechTrading) and "TXT_KEY_ALLOWS_TECH_TRADING" )
		
		-- Tech grants research agreements and research agreements aren't disabled
		addSmallGenericButtonIf( (tech.ResearchAgreementTradingAllowed and g_bResearchAgreements) and "TXT_KEY_ALLOWS_RESEARCH_AGREEMENTS" )
		
		-- Tech grants vassalage and vassalage isn't disabled
		addSmallGenericButtonIf( (tech.VassalageTradingAllowed and not g_bNoVassalage) and "TXT_KEY_ALLOWS_VASSALAGE" )
		-- END

		addSmallGenericButtonIf( tech.TradeAgreementTradingAllowed and "TXT_KEY_ALLOWS_TRADE_AGREEMENTS" )

		addSmallGenericButtonIf( tech.UnlocksEspionageAdvancedActions and "TXT_KEY_ALLOWS_ADVANCED_ACTIONS" )

		if tech.BridgeBuilding then
			addSmallActionButton( GameInfo.Missions.MISSION_ROUTE_TO, "", "TXT_KEY_ALLOWS_BRIDGES" )
		end

--CBP
		if tech.FeatureProductionModifier > 0 then
			addSmallActionButton( GameInfo.Missions.MISSION_GOLDEN_AGE, "", "TXT_KEY_ABLTY_TECH_BOOST_CHOP", tech.FeatureProductionModifier )
		end

		if tech.Happiness > 0 then
			addSmallActionButton( GameInfo.Missions.MISSION_GOLDEN_AGE, "", "TXT_KEY_ABLTY_HAPPINESS_BUMP", tech.Happiness )
		end
		if tech.BombardRange > 0 then
			addSmallActionButton( GameInfo.Missions.MISSION_RANGE_ATTACK, "", "TXT_KEY_ABLTY_CITY_RANGE_INCREASE" )
		end
		if tech.BombardIndirect > 0 then
			addSmallActionButton( GameInfo.Missions.MISSION_RANGE_ATTACK, "", "TXT_KEY_ABLTY_CITY_INDIRECT_INCREASE" )
		end
		if tech.CityLessEmbarkCost then
			addSmallActionButton( GameInfo.Missions.MISSION_EMBARK, "", "TXT_KEY_ABLTY_CITY_LESS_EMBARK_COST_STRING" )
		end
		if tech.CityNoEmbarkCost then
			addSmallActionButton( GameInfo.Missions.MISSION_EMBARK, "", "TXT_KEY_ABLTY_CITY_NO_EMBARK_COST_STRING" )
		end

		if tech.CorporationsEnabled then
			addSmallActionButton( GameInfo.Missions.MISSION_BUY_CITY_STATE, "", "TXT_KEY_ABLTY_ENABLES_CORPORATIONS" )			
			for row in GameInfo.Corporations() do
				if not addSmallArtButton( AdjustArtOnGrantedCorpButton, row ) then
					break
				end
			end
		end

		for row in GameInfo.Tech_SpecialistYieldChanges( thisTechType ) do
			local specialist = GameInfo.Specialists[row.SpecialistType]
			if specialist then
				local icons = ""
				local toolTip = ""
				for row2 in GameInfo.Tech_SpecialistYieldChanges( thisTechType ) do
					if(row2.SpecialistType == row.SpecialistType) then
						local yield = GameInfo.Yields[row2.YieldType]
						if yield and (row2.Yield > 0) then
							local icon = YieldIcons[row2.YieldType] or "???"
							icons = icons .. icon
							toolTip = toolTip .. L("TXT_KEY_SPECIALIST_YIELD_CHANGE", specialist.Description, yield.Description, row2.Yield, Locale.ConvertTextKey(yield.IconString))
						end
					end
				end
				addSmallArtYieldButton(specialist, icons, toolTip)
			end
		end
-- END	

	elseif civBE_mode then
		addSmallGenericButtonIf( tech.AllianceTradingAllowed and "TXT_KEY_ALLOWS_ALLIANCES" )
		addSmallGenericButtonIf( (tonumber(tech.UnitBaseMiasmaHeal) or 0) > 0 and "TXT_KEY_BASE_MIASMA_HEAL", tech.UnitBaseMiasmaHeal )
	end

	addSmallGenericButtonIf( tech.MapVisible and "TXT_KEY_REVEALS_ENTIRE_MAP" )

	if bnw_mode then
		local internationalTradeRoutesChange = tech.InternationalTradeRoutesChange
		if (internationalTradeRoutesChange or 0) > 0 then
			addSmallActionButton( GameInfo.Missions.MISSION_ESTABLISH_TRADE_ROUTE, "+"..internationalTradeRoutesChange.."   ", "TXT_KEY_ADDITIONAL_INTERNATIONAL_TRADE_ROUTE" )
		end

		for row in GameInfo.Technology_TradeRouteDomainExtraRange( thisTechType ) do
			if row.TechType == techType and (row.Range or 0) > 0 then
				local toolTip = ""
				local domainID = (GameInfo.Domains[ row.DomainType or -1 ] or {}).ID
				if domainID == DomainTypes.DOMAIN_LAND then
					toolTip = "TXT_KEY_EXTENDS_LAND_TRADE_ROUTE_RANGE"
				elseif domainID == DomainTypes.DOMAIN_SEA then
					toolTip = "TXT_KEY_EXTENDS_SEA_TRADE_ROUTE_RANGE"
				end
				if not addSmallGenericButton( toolTip ) then
					break
				end
			end
		end

		if civ5_mode then
			addSmallGenericButtonIf( (tonumber(tech.InfluenceSpreadModifier) or 0) > 0 and "TXT_KEY_DOUBLE_TOURISM" )

			addSmallGenericButtonIf( tech.AllowsWorldCongress and "TXT_KEY_ALLOWS_WORLD_CONGRESS" )

			addSmallGenericButtonIf( (tonumber(tech.ExtraVotesPerDiplomat) or 0) > 0 and "TXT_KEY_EXTRA_VOTES_FROM_DIPLOMATS", tech.ExtraVotesPerDiplomat )

			addSmallGenericButtonIf( tech.ScenarioTechButton == 1 and "TXT_KEY_SCENARIO_TECH_BUTTON_1" )
			addSmallGenericButtonIf( tech.ScenarioTechButton == 2 and "TXT_KEY_SCENARIO_TECH_BUTTON_2" )
			addSmallGenericButtonIf( tech.ScenarioTechButton == 3 and "TXT_KEY_SCENARIO_TECH_BUTTON_3" )
			addSmallGenericButtonIf( tech.ScenarioTechButton == 4 and "TXT_KEY_SCENARIO_TECH_BUTTON_4" )
		end
	end

	if gk_mode then
		for row in GameInfo.Technology_FreePromotions( thisTechType ) do
			local promotion = GameInfo.UnitPromotions[ row.PromotionType or -1 ]
			if promotion and not addSmallButton( promotion.PortraitIndex, promotion.IconAtlas, "TXT_KEY_FREE_PROMOTION_FROM_TECH", promotion.Description, promotion.Help ) then
				break
			end
		end
	end

	-- Some improvements can have multiple yield changes, group them and THEN add buttons.
	local improvementTypesChanged = {}
	for row in GameInfo.Improvement_TechYieldChanges( thisTechType ) do
		local thisImprovement = GameInfo.Improvements[ row.ImprovementType ]
		if thisImprovement and (not thisImprovement.CivilizationType or thisImprovement.CivilizationType == civType) then
			improvementTypesChanged[ row.ImprovementType or -1 ] = true
		end
	end
	-- Let's sort the yield change buttons!
	local sortedImprovements = table()
	for improvementType in pairs( improvementTypesChanged ) do
		sortedImprovements:insertIf( GameInfo.Improvements[ improvementType ] )
	end
	sortedImprovements:sort( function(a,b) return Locale.Compare(a.Description or "", b.Description or "") == -1 end )

	local thisTechAndImprovementTypes = { TechType = techType }
	for i,improvement in ipairs( sortedImprovements ) do
		local toolTip = L( improvement.Description or "???" )
		local icons = ""
		thisTechAndImprovementTypes.ImprovementType = improvement.Type
		for row in GameInfo.Improvement_TechYieldChanges( thisTechAndImprovementTypes ) do
			if (row.Yield or 0)~=0 then
				local icon = YieldIcons[row.YieldType] or "???"
				icons = icons .. icon
				toolTip = toolTip .. (" +%g"):format(row.Yield) .. icon
			end
		end
		if not addSmallActionButton( GameInfo.Builds{ ImprovementType = improvement.Type }() or "???", icons, toolTip ) then
			break
		end
	end

	for row in GameInfo.Improvement_TechNoFreshWaterYieldChanges( thisTechType ) do
		local yield = GameInfo.Yields[row.YieldType]
		if yield and not addSmallActionButton( GameInfo.Builds{ ImprovementType = row.ImprovementType }(), yield.IconString,
				"TXT_KEY_NO_FRESH_WATER", (GameInfo.Improvements[row.ImprovementType] or {}).Description or "???" , yield.Description, row.Yield )
		then
			break
		end
	end

	for row in GameInfo.Improvement_TechFreshWaterYieldChanges( thisTechType ) do
		local yield = GameInfo.Yields[row.YieldType]
		if yield and not addSmallActionButton( GameInfo.Builds{ ImprovementType = row.ImprovementType }(), yield.IconString,
				"TXT_KEY_FRESH_WATER", (GameInfo.Improvements[row.ImprovementType] or {}).Description or "???", yield.Description, row.Yield )
		then
			break
		end
	end

	-- show buttons we are using and hide the rest
	for i = 1, maxSmallButtons do
		local button = thisTechButtonInstance["B"..i]
		if button then
			button:SetHide(i>=buttonNum)
		else
			break
		end
	end
	if civ5_mode then
		return buttonNum
	else
		return buttonNum - 1  -- another Firaxis cleverness
	end
end
local AddSmallButtonsToTechButton = AddSmallButtonsToTechButton

-- ===========================================================================
--	Obtain small buttons for a tech and lay them out radially centered
--	from the bottom.
-- ===========================================================================
function AddSmallButtonsToTechButtonRadial( thisTechButtonInstance, tech, maxSmallButtons, textureSize )
	local buttonNum = AddSmallButtonsToTechButton( thisTechButtonInstance, tech, maxSmallButtons, textureSize )

	-- Push the start back based on # of icons
	local phiDegrees = 90 - ((buttonNum-1) * 24 ) -- 90° is facing down (0° is far right), +values are clockwise, 24° is 1/2 angle per icon
	for i = 1, buttonNum do
		thisTechButtonInstance["B"..i]:SetOffsetVal( PolarToCartesian( 46, 24 * i + phiDegrees ) ) -- 46 is radius
	end

	return buttonNum;
end

function AddCallbackToSmallButtons( thisTechButtonInstance, maxSmallButtons, void1, void2, thisEvent, thisCallback )
	for i = 1, maxSmallButtons do
		local button = thisTechButtonInstance["B"..i]
		if button then
			button:SetVoids(void1, void2)
			button:RegisterCallback(thisEvent, thisCallback)
		else
			break
		end
	end
end

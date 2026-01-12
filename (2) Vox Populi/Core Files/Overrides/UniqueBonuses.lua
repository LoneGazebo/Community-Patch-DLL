-------------------------------------------------
-- Unique Bonuses

-- This is a file intended to be shared
-------------------------------------------------
include( "IconSupport" );
include( "InfoTooltipInclude" );

-- List the textures that we will need here
local defaultErrorTextureSheet = "IconFrame64.dds";

nullOffset = Vector2(0,0);
questionOffset, questionTextureSheet = IconLookup( 23, 64, "CIV_COLOR_ATLAS" );
unknownString = Locale.ConvertTextKey( "TXT_KEY_MISC_UNKNOWN" );

-------------------------------------------------
-- Pedia Callback Setup
-------------------------------------------------
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

------------------------------------------------------------------
------------------------------------------------------------------
function AdjustArtOnUniqueUnitButton( thisButton, thisFrame, thisUnitInfo, textureSize, extendedTooltip, noTooltip)
	if thisButton then
		if (noTooltip ~= true) then
			if (extendedTooltip) then
				thisButton:SetToolTipString( Locale.ConvertTextKey(GetHelpTextForUnit(thisUnitInfo.ID, true, nil, false, not Game)));
			else
				thisButton:SetToolTipString( Locale.ConvertTextKey( thisUnitInfo.Description ) );
			end
		else
			thisButton:SetToolTipString("");
		end

		-- if we have one, update the unit picture
		local textureOffset, textureSheet = IconLookup( thisUnitInfo.PortraitIndex, textureSize, thisUnitInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		thisFrame:SetHide( false );
	end
end

------------------------------------------------------------------
------------------------------------------------------------------
function AdjustArtOnUniqueBuildingButton( thisButton, thisFrame, thisBuildingInfo, textureSize, extendedTooltip, noTooltip)
	if thisButton then
		if(noTooltip ~= true) then
			if (extendedTooltip) then
				thisButton:SetToolTipString( Locale.ConvertTextKey(GetHelpTextForBuilding(thisBuildingInfo.ID, false, nil, false, nil, not Game)));
			else
				thisButton:SetToolTipString( Locale.ConvertTextKey( thisBuildingInfo.Description ) );
			end
		else
			thisButton:SetToolTipString("");
		end

		-- if we have one, update the building (or wonder) picture
		local textureOffset, textureSheet = IconLookup( thisBuildingInfo.PortraitIndex, textureSize, thisBuildingInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end

		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		thisFrame:SetHide( false );
	end
end

------------------------------------------------------------------
------------------------------------------------------------------
function AdjustArtOnUniqueImprovementButton( thisButton, thisFrame, thisImprovmentInfo, textureSize, extendedTooltip, noTooltip)
	if thisButton then
		if(noTooltip ~= true) then
			if (extendedTooltip) then
				thisButton:SetToolTipString( Locale.ConvertTextKey(GetHelpTextForImprovement(thisImprovmentInfo.ID, false, false, false)));
			else
				thisButton:SetToolTipString( Locale.ConvertTextKey( thisImprovmentInfo.Description ) );
			end
		else
			thisButton:SetToolTipString("");
		end

		-- if we have one, update the building (or wonder) picture
		local textureOffset, textureSheet = IconLookup( thisImprovmentInfo.PortraitIndex, textureSize, thisImprovmentInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end

		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		thisFrame:SetHide( false );
	end
end
------------------------------------------------------------------
------------------------------------------------------------------
function AdjustArtOnUniqueProjectButton( thisButton, thisFrame, thisProjectInfo, textureSize, extendedTooltip, noTooltip)
	if thisButton then
		if(noTooltip ~= true) then
			if (extendedTooltip) then
				thisButton:SetToolTipString( Locale.ConvertTextKey(GetHelpTextForProject(thisProjectInfo.ID)));
			else
				thisButton:SetToolTipString( Locale.ConvertTextKey( thisProjectInfo.Description ) );
			end
		else
			thisButton:SetToolTipString("");
		end

		-- if we have one, update the building (or wonder) picture
		local textureOffset, textureSheet = IconLookup( thisProjectInfo.PortraitIndex, textureSize, thisProjectInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end

		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		thisFrame:SetHide( false );
	end
end

------------------------------------------------------------------
-- Developer Note:
-- The argument noTooltip is an additional argument to specify no tooltip should be used.
-- This is an additional argument instead of modifying extendedTooltip so that pre-existing calls
-- will behave the same way.
------------------------------------------------------------------
function PopulateUniqueBonuses( controlTable, civ, _, extendedTooltip, noTooltip)

	local maxSmallButtons = GameDefines.NUM_UNIQUE_COMPONENTS;
	local BonusText = {};

	function PopulateButton(button, buttonFrame)
		local textureSize = 64;

		for row in DB.Query([[SELECT ID, Description, PortraitIndex, IconAtlas from Units INNER JOIN 
							Civilization_UnitClassOverrides ON Units.Type = Civilization_UnitClassOverrides.UnitType 
							WHERE Units.ShowInPedia = 1 AND Civilization_UnitClassOverrides.CivilizationType = ? AND
							Civilization_UnitClassOverrides.UnitType IS NOT NULL]], civ.Type) do
			AdjustArtOnUniqueUnitButton(button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
 			button, buttonFrame = coroutine.yield(row.Description);	
		end
		
		for row in DB.Query([[SELECT ID, Description, PortraitIndex, IconAtlas from Buildings INNER JOIN 
							Civilization_BuildingClassOverrides ON Buildings.Type = Civilization_BuildingClassOverrides.BuildingType 
							WHERE Buildings.ShowInPedia = 1 AND Civilization_BuildingClassOverrides.CivilizationType = ? AND
							Civilization_BuildingClassOverrides.BuildingType IS NOT NULL]], civ.Type) do
			AdjustArtOnUniqueBuildingButton(button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
 			button, buttonFrame = coroutine.yield(row.Description);	
		end
			
		for row in DB.Query([[SELECT ID, Description, PortraitIndex, IconAtlas from Improvements where CivilizationType = ?]], civ.Type) do
			AdjustArtOnUniqueImprovementButton( button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
 			button, buttonFrame = coroutine.yield(row.Description);
		end

		-- 5/6 UC improvements using Trait_BuildsUnitClasses and OrderPriority
		for row in DB.Query([[SELECT DISTINCT i.ID, i.Description, i.PortraitIndex, i.IconAtlas from Improvements i, Civilization_Leaders cl, Leader_Traits lt, Trait_BuildsUnitClasses tbuc, Builds b 
			where cl.CivilizationType = ? and lt.LeaderType = cl.LeaderheadType and lt.TraitType = tbuc.TraitType and tbuc.BuildType = b.Type and i.Type = b.ImprovementType and b.OrderPriority=90]], civ.Type) do
			AdjustArtOnUniqueImprovementButton( button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
 			button, buttonFrame = coroutine.yield(row.Description);
		end

		for row in DB.Query([[SELECT ID, Description, PortraitIndex, IconAtlas from Projects where CivilizationType = ?]], civ.Type) do
			AdjustArtOnUniqueProjectButton( button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
 			button, buttonFrame = coroutine.yield(row.Description);
		end
	end
	

	local co = coroutine.create(PopulateButton);
	for buttonNum = 1, maxSmallButtons, 1 do
		local buttonName = "B"..tostring(buttonNum);
		local buttonFrameName = "BF"..tostring(buttonNum);

		local button = controlTable[buttonName];
		local buttonFrame = controlTable[buttonFrameName];

		if(button and buttonFrame) then
			button:SetHide(true);
			buttonFrame:SetHide(true);
			local _, text = coroutine.resume(co, button, buttonFrame);
			table.insert(BonusText, text);

			-- Register pedia callback after button is populated
			if CivilopediaControl and text and button then
				button:RegisterCallback( Mouse.eRClick, function() getPedia( text ) end );
			end
		end
	end
    
    return BonusText;
end


------------------------------------------------------------------
-- This method generates a method optimized for reuse for 
-- populating unique bonus icons.
------------------------------------------------------------------
function PopulateUniqueBonuses_CreateCached()
	
	local uniqueUnitsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Units INNER JOIN 
								Civilization_UnitClassOverrides ON Units.Type = Civilization_UnitClassOverrides.UnitType 
								WHERE Units.ShowInPedia = 1 AND Civilization_UnitClassOverrides.CivilizationType = ? AND
								Civilization_UnitClassOverrides.UnitType IS NOT NULL]]);
								
	local uniqueBuildingsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Buildings INNER JOIN 
								Civilization_BuildingClassOverrides ON Buildings.Type = Civilization_BuildingClassOverrides.BuildingType 
								WHERE Buildings.ShowInPedia = 1 AND Civilization_BuildingClassOverrides.CivilizationType = ? AND
								Civilization_BuildingClassOverrides.BuildingType IS NOT NULL]]);
								
	local uniqueImprovementsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Improvements where CivilizationType = ?]]);

	-- 5/6 UC
	local culturalImprovementsQuery = DB.CreateQuery([[SELECT DISTINCT i.ID, i.Description, i.PortraitIndex, i.IconAtlas from Improvements i, Civilization_Leaders cl, Leader_Traits lt, Trait_BuildsUnitClasses tbuc, Builds b 
		where cl.CivilizationType = ? and lt.LeaderType = cl.LeaderheadType and lt.TraitType = tbuc.TraitType and tbuc.BuildType = b.Type and i.Type = b.ImprovementType and b.OrderPriority=90]]);
	
	local uniqueProjectsQuery = DB.CreateQuery([[SELECT ID, Description, PortraitIndex, IconAtlas from Projects where CivilizationType = ?]]);
	

	return function(controlTable, civType, extendedTooltip, noTooltip)
		local maxSmallButtons = GameDefines.NUM_UNIQUE_COMPONENTS;
		local BonusText = {};

		local textureSize = 64;

		local buttonFuncs = {};
		
		for row in uniqueUnitsQuery(civType) do
			table.insert(buttonFuncs, function(button, buttonFrame)
				AdjustArtOnUniqueUnitButton(button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
				return row.Description;
			end);
		end
		
		if(#buttonFuncs < maxSmallButtons) then
			for row in uniqueBuildingsQuery(civType) do
				table.insert(buttonFuncs, function(button, buttonFrame)
					AdjustArtOnUniqueBuildingButton(button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
					return row.Description;
				end);	
			end
		
			if(#buttonFuncs < maxSmallButtons) then	
				for row in uniqueImprovementsQuery(civType) do
					table.insert(buttonFuncs, function(button, buttonFrame)
						AdjustArtOnUniqueImprovementButton( button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
						return row.Description;
					end);
				end
				
				if(#buttonFuncs < maxSmallButtons) then	
					for row in uniqueProjectsQuery(civType) do
						table.insert(buttonFuncs, function(button, buttonFrame)
							AdjustArtOnUniqueProjectButton( button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
							return row.Description;
						end);
					end
		
					if(#buttonFuncs < maxSmallButtons) then	
						for row in culturalImprovementsQuery(civType) do
							table.insert(buttonFuncs, function(button, buttonFrame)
								AdjustArtOnUniqueImprovementButton( button, buttonFrame, row, textureSize, extendedTooltip, noTooltip);
								return row.Description;
							end);
						end
					end

				end
			end
		end

		-- Hide all buttons first
		for buttonNum = 1, 6, 1 do
			local buttonName = "B"..tostring(buttonNum);
			local buttonFrameName = "BF"..tostring(buttonNum);
			local button = controlTable[buttonName];
			local buttonFrame = controlTable[buttonFrameName];

			if(button and buttonFrame) then
				button:SetHide(true);
				buttonFrame:SetHide(true);
			end
		end

		-- Show only the buttons with content
		for buttonNum = 1, maxSmallButtons, 1 do
			local buttonName = "B"..tostring(buttonNum);
			local buttonFrameName = "BF"..tostring(buttonNum);

			local button = controlTable[buttonName];
			local buttonFrame = controlTable[buttonFrameName];

			if(button and buttonFrame and #buttonFuncs >= buttonNum) then
				local text = buttonFuncs[buttonNum](button, buttonFrame);
				table.insert(BonusText, text);

				-- Register pedia callback after button is populated
				if CivilopediaControl and text and button then
					button:RegisterCallback( Mouse.eRClick, function() getPedia( text ) end );
				end
			end
		end
	    
		return BonusText;
	end;
end

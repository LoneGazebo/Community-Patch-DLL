-------------------------------------------------
-- Include file that has handy stuff for the tech tree and other screens that need to show a tech button
-------------------------------------------------
include( "IconSupport" );
include( "InfoTooltipInclude" );

-- List the textures that we will need here
local defaultErrorTextureSheet;

techPediaSearchStrings = {};

if g_UseSmallIcons then
	defaultErrorTextureSheet = "UnitActions360.dds";
else
	defaultErrorTextureSheet = "UnitActions.dds";
end

local validUnitBuilds = nil;
local validBuildingBuilds = nil;
local validImprovementBuilds = nil;

local g_bResearchAgreementTrading = Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS");
local g_bNoTechTrading = Game.IsOption("GAMEOPTION_NO_TECH_TRADING");
local g_bNoVassalage = Game.IsOption("GAMEOPTION_NO_VASSALAGE");

turnsString = Locale.ConvertTextKey("TXT_KEY_TURNS");
freeString = Locale.ConvertTextKey("TXT_KEY_FREE");
lockedString = "[ICON_LOCKED]"; --Locale.ConvertTextKey("TXT_KEY_LOCKED");

function GetTechPedia( void1, void2, button )
	local searchString = techPediaSearchStrings[tostring(button)];
	Events.SearchForPediaEntry( searchString );		
end

function GatherInfoAboutUniqueStuff( civType )

	validUnitBuilds = {};
	validBuildingBuilds = {};
	validImprovementBuilds = {};

	-- put in the default units for any civ
	for thisUnitClass in GameInfo.UnitClasses() do
		validUnitBuilds[thisUnitClass.Type]	= thisUnitClass.DefaultUnit;	
	end

	-- put in my overrides
	for thisOverride in GameInfo.Civilization_UnitClassOverrides() do
 		if thisOverride.CivilizationType == civType then
			validUnitBuilds[thisOverride.UnitClassType]	= thisOverride.UnitType;
 		end
	end

	-- put in the default buildings for any civ
	for thisBuildingClass in GameInfo.BuildingClasses() do
		validBuildingBuilds[thisBuildingClass.Type]	= thisBuildingClass.DefaultBuilding;	
	end

	-- put in my overrides
	for thisOverride in GameInfo.Civilization_BuildingClassOverrides() do
 		if thisOverride.CivilizationType == civType then
			validBuildingBuilds[thisOverride.BuildingClassType]	= thisOverride.BuildingType;	
 		end
	end
	
	-- add in support for unique improvements
	for thisImprovement in GameInfo.Improvements() do
		if thisImprovement.CivilizationType == civType or thisImprovement.CivilizationType == nil then
			validImprovementBuilds[thisImprovement.Type] = thisImprovement.Type;	
		else
			validImprovementBuilds[thisImprovement.Type] = nil;	
		end
	end
	
end


function AddSmallButtonsToTechButton( thisTechButtonInstance, tech, maxSmallButtons, textureSize )
	-- This has a few assumptions, the main one being that the small buttons are named "B1", "B2", "B3"... and that GatherInfoAboutUniqueStuff() has been called before this

	-- first, hide the ones we aren't using
	for buttonNum = 1, maxSmallButtons, 1 do
		local buttonName = "B"..tostring(buttonNum);
		thisTechButtonInstance[buttonName]:SetHide(true);
		thisTechButtonInstance[buttonName]:SetAlpha(1);
	end
	
	if tech == nil then
		return;
	end

	local buttonNum = 1;

	local techType = tech.Type;

	-- add the stuff granted by this tech here
  	for thisUnitInfo in GameInfo.Units(string.format("PreReqTech = '%s'", techType)) do
 		-- if this tech grants this player the ability to make this unit
		if validUnitBuilds[thisUnitInfo.Class] == thisUnitInfo.Type then
			local iMinorCivGift = thisUnitInfo.MinorCivGift
			if (iMinorCivGift ~= 1) then
				local buttonName = "B"..tostring(buttonNum);
				local thisButton = thisTechButtonInstance[buttonName];
				if thisButton then
					AdjustArtOnGrantedUnitButton( thisButton, thisUnitInfo, textureSize );
					buttonNum = buttonNum + 1;
				end
			end
		end
 	end
 	
 	for thisBuildingInfo in GameInfo.Buildings(string.format("PreReqTech = '%s'", techType)) do
 		-- if this tech grants this player the ability to construct this building
		if validBuildingBuilds[thisBuildingInfo.BuildingClass] == thisBuildingInfo.Type then
			local buttonName = "B"..tostring(buttonNum);
			local thisButton = thisTechButtonInstance[buttonName];
			if thisButton then
				AdjustArtOnGrantedBuildingButton( thisButton, thisBuildingInfo, textureSize );
				buttonNum = buttonNum + 1;
			end
		end
 	end

 	for thisResourceInfo in GameInfo.Resources(string.format("TechReveal = '%s'", techType)) do
 		-- if this tech grants this player the ability to reveal this resource
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			AdjustArtOnGrantedResourceButton( thisButton, thisResourceInfo, textureSize );
			buttonNum = buttonNum + 1;
		end
 	end
 
 	for thisProjectInfo in GameInfo.Projects(string.format("TechPrereq = '%s'", techType)) do
 		-- if this tech grants this player the ability to build this project
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
 		if thisButton then
			AdjustArtOnGrantedProjectButton( thisButton, thisProjectInfo, textureSize );
 			buttonNum = buttonNum + 1;
 		end
	end

	-- if this tech grants this player the ability to perform this action (usually only workers can do these)
	for thisBuildInfo in GameInfo.Builds{PrereqTech = techType, ShowInTechTree  = 1} do
		if thisBuildInfo.ImprovementType then
			if validImprovementBuilds[thisBuildInfo.ImprovementType] == thisBuildInfo.ImprovementType then
				local buttonName = "B"..tostring(buttonNum);
				local thisButton = thisTechButtonInstance[buttonName];
				if thisButton then
					AdjustArtOnGrantedActionButton( thisButton, thisBuildInfo, textureSize );
 					buttonNum = buttonNum + 1;
 				end
 			end
		else
			local buttonName = "B"..tostring(buttonNum);
			local thisButton = thisTechButtonInstance[buttonName];
			if thisButton then
				AdjustArtOnGrantedActionButton( thisButton, thisBuildInfo, textureSize );
 				buttonNum = buttonNum + 1;
 			end
		end
	end
	
	-- show processes
	local processCondition = "TechPrereq = '" .. techType .. "'";
	for row in GameInfo.Processes(processCondition) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( row.PortraitIndex, textureSize, row.IconAtlas, thisButton );
			thisButton:SetHide( false );
			local strPText = Locale.ConvertTextKey( row.Description );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ENABLE_PRODUCITON_CONVERSION", strPText) );
		end
		buttonNum = buttonNum + 1;
	end	
		
 	-- todo: need to add abilities, etc.
	local condition = "TechType = '" .. techType .. "'";
		
	for row in GameInfo.Route_TechMovementChanges(condition) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_FASTER_MOVEMENT", GameInfo.Routes[row.RouteType].Description ) );
			buttonNum = buttonNum + 1;
		else
			break
		end
	end	

	for row in GameInfo.Build_TechTimeChanges(condition) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_BUILD_COST_REDUCTION", GameInfo.Builds[row.BuildType].Description, row.TimeChange/100) );
			buttonNum = buttonNum + 1;
		else
			break
		end
	end	

	local playerID = Game.GetActivePlayer();	
	local player = Players[playerID];
	local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;

	if tech.FeatureProductionModifier > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_TECH_BOOST_CHOP", tech.FeatureProductionModifier ) );
			buttonNum = buttonNum + 1;
		end
	end

	if tech.EmbarkedMoveChange > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_FASTER_EMBARKED_MOVEMENT" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.AllowsEmbarking then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_EMBARKING" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.AllowsDefensiveEmbarking then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_DEFENSIVE_EMBARK_STRING" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.EmbarkedAllWaterPassage then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_CROSSING_OCEANS" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.UnitFortificationModifier > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_UNIT_FORTIFICATION_MOD", tech.UnitFortificationModifier ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.UnitBaseHealModifier > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_UNIT_BASE_HEAL_MOD", tech.UnitBaseHealModifier ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.AllowEmbassyTradingAllowed then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_EMBASSY" ) );
			buttonNum = buttonNum + 1;
		end	
	end
	
	if tech.OpenBordersTradingAllowed then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_OPEN_BORDERS" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.DefensivePactTradingAllowed then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_DEFENSIVE_PACTS" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	-- Putmalk: Tech grants map trading
	if tech.MapTrading then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_MAP_TRADING" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	-- Putmalk: Tech grants tech trades and tech trades aren't disabled
	if (tech.TechTrading and not g_bNoTechTrading)then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_TECH_TRADING" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	-- Putmalk: Tech grants research agreements and research agreements aren't disabled
	if (tech.ResearchAgreementTradingAllowed and g_bResearchAgreements) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_RESEARCH_AGREEMENTS" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	-- Putmalk: Tech grants vassalage and vassalage isn't disabled
	if (tech.VassalageTradingAllowed and not g_bNoVassalage) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_VASSALAGE" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.TradeAgreementTradingAllowed then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_TRADE_AGREEMENTS" ) );
			buttonNum = buttonNum + 1;
		end
	end

	if tech.UnlocksEspionageAdvancedActions then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_ADVANCED_ACTIONS" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.BridgeBuilding then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_BRIDGES" ) );
			buttonNum = buttonNum + 1;
		end
	end

--CBP

	if tech.Happiness > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_HAPPINESS_BUMP", tech.Happiness ) );
			buttonNum = buttonNum + 1;
		end
	end
	if tech.BombardRange > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_RANGE_INCREASE" ) );
			buttonNum = buttonNum + 1;
		end
	end
	if tech.BombardIndirect > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_INDIRECT_INCREASE" ) );
			buttonNum = buttonNum + 1;
		end
	end
	if tech.CityLessEmbarkCost then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_LESS_EMBARK_COST_STRING" ) );
			buttonNum = buttonNum + 1;
		end
	end
	if tech.CityNoEmbarkCost then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_CITY_NO_EMBARK_COST_STRING" ) );
			buttonNum = buttonNum + 1;
		end
	end
	if tech.CorporationsEnabled then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ABLTY_ENABLES_CORPORATIONS" ) );
			buttonNum = buttonNum + 1;
		end
		for thisCorpInfo in GameInfo.Corporations() do
 			-- if this tech grants this player the ability to build this corp
			local buttonName = "B"..tostring(buttonNum);
			local thisButton = thisTechButtonInstance[buttonName];
 			if thisButton then
				AdjustArtOnGrantedCorpButton( thisButton, thisCorpInfo, textureSize );
 				buttonNum = buttonNum + 1;
 			end
		end
	end

	for row in GameInfo.Tech_SpecialistYieldChanges(condition) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_SPECIALIST_YIELD_CHANGE", GameInfo.Specialists[row.SpecialistType].Description , GameInfo.Yields[row.YieldType].Description, row.Yield, Locale.ConvertTextKey(GameInfo.Yields[row.YieldType].IconString)));
			buttonNum = buttonNum + 1;
		else
			break;
		end
	end
-- END	

	if tech.MapVisible then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_REVEALS_ENTIRE_MAP" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.InternationalTradeRoutesChange > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ADDITIONAL_INTERNATIONAL_TRADE_ROUTE" ) );
			buttonNum = buttonNum + 1;
		end	
	end
	
	
	if (tech.ScenarioTechButton == 1) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SCENARIO_TECH_BUTTON_1" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if (tech.ScenarioTechButton == 2) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SCENARIO_TECH_BUTTON_2" ) );
			buttonNum = buttonNum + 1;
		end
	end


	if (tech.ScenarioTechButton == 3) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SCENARIO_TECH_BUTTON_3" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if (tech.ScenarioTechButton == 4) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SCENARIO_TECH_BUTTON_4" ) );
			buttonNum = buttonNum + 1;
		end
	end

	if (tech.ScenarioTechButton == 3) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "Allows infantry to pillage" ) );
			buttonNum = buttonNum + 1;
		end
	end

	if (tech.ScenarioTechButton == 4) then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "Allows infantry to pillage" ) );
			buttonNum = buttonNum + 1;
		end
	end

	for row in GameInfo.Technology_TradeRouteDomainExtraRange(condition) do
		if (row.TechType == techType and row.Range > 0) then
			local buttonName = "B"..tostring(buttonNum);
			local thisButton = thisTechButtonInstance[buttonName];
			if thisButton then
				IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
				thisButton:SetHide( false );
				if (GameInfo.Domains[row.DomainType].ID == DomainTypes.DOMAIN_LAND) then
					thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_EXTENDS_LAND_TRADE_ROUTE_RANGE" ) );
				elseif (GameInfo.Domains[row.DomainType].ID == DomainTypes.DOMAIN_SEA) then
					thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_EXTENDS_SEA_TRADE_ROUTE_RANGE" ) );
				end
				buttonNum = buttonNum + 1;
			end	
		end
	end
	
	if tech.InfluenceSpreadModifier > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DOUBLE_TOURISM" ) );
			buttonNum = buttonNum + 1;
		end	
	end
	
	if tech.AllowsWorldCongress then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_ALLOWS_WORLD_CONGRESS" ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	if tech.ExtraVotesPerDiplomat > 0 then
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_EXTRA_VOTES_FROM_DIPLOMATS", tech.ExtraVotesPerDiplomat ) );
			buttonNum = buttonNum + 1;
		end
	end
	
	for row in GameInfo.Technology_FreePromotions(condition) do
		local promotion = GameInfo.UnitPromotions[row.PromotionType];
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton and promotion ~= nil then
			IconHookup( promotion.PortraitIndex, textureSize, promotion.IconAtlas, thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_FREE_PROMOTION_FROM_TECH", promotion.Description, promotion.Help) );
			buttonNum = buttonNum + 1;
		else
			break;
		end
	end

	-- Some improvements can have multiple yield changes, group them and THEN add buttons.
	local yieldChanges = {};
	for row in GameInfo.Improvement_TechYieldChanges(condition) do
		local improvementType = row.ImprovementType;

		local improvement = GameInfo.Improvements[row.ImprovementType];
		if improvement and (not improvement.CivilizationType or improvement.CivilizationType == civType) then

			if(yieldChanges[improvementType] == nil) then
				yieldChanges[improvementType] = {};
			end

			local yield = GameInfo.Yields[row.YieldType];
		
			table.insert(yieldChanges[improvementType], Locale.Lookup( "TXT_KEY_YIELD_IMPROVED", improvement.Description , yield.Description, row.Yield));
		end
	end
	
	-- Let's sort the yield change butons!
	local sortedYieldChanges = {};
	for k,v in pairs(yieldChanges) do
	
		
		table.insert(sortedYieldChanges, {k,v});
	end
	table.sort(sortedYieldChanges, function(a,b) return Locale.Compare(a[1], b[1]) == -1 end); 
	
	for i,v in pairs(sortedYieldChanges) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if(thisButton ~= nil) then
			table.sort(v[2], function(a,b) return Locale.Compare(a,b) == -1 end);
		
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString(table.concat(v[2], "[NEWLINE]"));
			buttonNum = buttonNum + 1;
		else
			break;
		end
	end	
	
	for row in GameInfo.Improvement_TechNoFreshWaterYieldChanges(condition) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_NO_FRESH_WATER", GameInfo.Improvements[row.ImprovementType].Description , GameInfo.Yields[row.YieldType].Description, row.Yield));
			buttonNum = buttonNum + 1;
		else
			break;
		end
	end	

	for row in GameInfo.Improvement_TechFreshWaterYieldChanges(condition) do
		local buttonName = "B"..tostring(buttonNum);
		local thisButton = thisTechButtonInstance[buttonName];
		if thisButton then
			IconHookup( 0, textureSize, "GENERIC_FUNC_ATLAS", thisButton );
			thisButton:SetHide( false );
			thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_FRESH_WATER", GameInfo.Improvements[row.ImprovementType].Description , GameInfo.Yields[row.YieldType].Description, row.Yield));
			buttonNum = buttonNum + 1;
		else
			break;
		end
	end	
	
	return buttonNum;
	
end


function AddCallbackToSmallButtons( thisTechButtonInstance, maxSmallButtons, void1, void2, thisEvent, thisCallback )
	for buttonNum = 1, maxSmallButtons, 1 do
		local buttonName = "B"..tostring(buttonNum);
		thisTechButtonInstance[buttonName]:SetVoids(void1, void2);
		thisTechButtonInstance[buttonName]:RegisterCallback(thisEvent, thisCallback);
	end
end


function AdjustArtOnGrantedUnitButton( thisButton, thisUnitInfo, textureSize )
	-- if we have one, update the unit picture
	if thisButton then
		
		-- Tooltip
		local bIncludeRequirementsInfo = true;
		thisButton:SetToolTipString( GetHelpTextForUnit(thisUnitInfo.ID, bIncludeRequirementsInfo) );
		local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(thisUnitInfo.ID);
		local textureOffset, textureSheet = IconLookup( portraitOffset, textureSize, portraitAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey(thisUnitInfo.Description);
		thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
	end
end
--Corps
function AdjustArtOnGrantedCorpButton( thisButton, thisCorpInfo, textureSize )
	-- if we have one, update the building (or wonder) picture
	if thisButton then
		
		-- Tooltip
		local bExcludeName = false;
		local bExcludeHeader = false;

		thisButton:SetToolTipString( "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey(thisCorpInfo.Description ) .. "[ENDCOLOR]" .. "[NEWLINE]----------------[NEWLINE]" .. Locale.ConvertTextKey(thisCorpInfo.Help) );
		
		local textureOffset, textureSheet = IconLookup( thisCorpInfo.PortraitIndex, textureSize, thisCorpInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );

		techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey(thisCorpInfo.Description);
		thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
	end
end
-- END


function AdjustArtOnGrantedBuildingButton( thisButton, thisBuildingInfo, textureSize )
	-- if we have one, update the building (or wonder) picture
	if thisButton then
		
		-- Tooltip
		local bExcludeName = false;
		local bExcludeHeader = false;
		thisButton:SetToolTipString( GetHelpTextForBuilding(thisBuildingInfo.ID, bExcludeName, bExcludeHeader, false, nil) );
		
		local textureOffset, textureSheet = IconLookup( thisBuildingInfo.PortraitIndex, textureSize, thisBuildingInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		--Hide Wonders already taken
		if (Game.AnyoneHasWonder(thisBuildingInfo.ID) and GameInfo.BuildingClasses[thisBuildingInfo.BuildingClass].MaxGlobalInstances == 1) then
			thisButton:SetAlpha(0.33);
		else
			thisButton:SetAlpha(1);
		end
		techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey(thisBuildingInfo.Description);
		thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
	end
end


function AdjustArtOnGrantedProjectButton( thisButton, thisProjectInfo, textureSize )
	-- if we have one, update the project picture
	if thisButton then
		
		-- Tooltip
		local bIncludeRequirementsInfo = true;
		thisButton:SetToolTipString( GetHelpTextForProject(thisProjectInfo.ID, nil, bIncludeRequirementsInfo) );

		local textureOffset, textureSheet = IconLookup( thisProjectInfo.PortraitIndex, textureSize, thisProjectInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey(thisProjectInfo.Description);
		thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
	end
end


function AdjustArtOnGrantedResourceButton( thisButton, thisResourceInfo, textureSize )
	if thisButton then
		thisButton:SetToolTipString( Locale.ConvertTextKey("TXT_KEY_REVEALS_RESOURCE_ON_MAP", thisResourceInfo.Description)); 

		local textureOffset, textureSheet = IconLookup( thisResourceInfo.PortraitIndex, textureSize, thisResourceInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide( false );
		techPediaSearchStrings[tostring(thisButton)] =  Locale.Lookup(thisResourceInfo.Description);
		thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
	end
end

function AdjustArtOnGrantedActionButton( thisButton, thisBuildInfo, textureSize )
	if thisButton then
		thisButton:SetToolTipString( Locale.ConvertTextKey( thisBuildInfo.Description ) );
		local textureOffset, textureSheet = IconLookup( thisBuildInfo.IconIndex, textureSize, thisBuildInfo.IconAtlas );				
		if textureOffset == nil then
			textureSheet = defaultErrorTextureSheet;
			textureOffset = nullOffset;
		end				
		thisButton:SetTexture( textureSheet );
		thisButton:SetTextureOffset( textureOffset );
		thisButton:SetHide(false);

		if thisBuildInfo.RouteType then
			techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey( GameInfo.Routes[thisBuildInfo.RouteType].Description );
		elseif thisBuildInfo.ImprovementType then
			techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey( GameInfo.Improvements[thisBuildInfo.ImprovementType].Description );
		else -- we are a choppy thing
			techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey( GameInfo.Concepts["CONCEPT_WORKERS_CLEARINGLAND"].Description );
		end
		thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
		--techPediaSearchStrings[tostring(thisButton)] = Locale.ConvertTextKey( thisBuildInfo.Description );
		--thisButton:RegisterCallback( Mouse.eRClick, GetTechPedia );
	end
end

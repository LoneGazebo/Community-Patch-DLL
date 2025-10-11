-- modified by bc1 from 1.0.3.144 brave new world code
-- extend full unit & building info pregame
-- extend AI mood info
-- code is common using gk_mode and bnw_mode switches
-- compatible with Communitas breaking yield types
-- TODO: lots !
--print( "Loading EUI tooltips for "..tostring(ContextPtr:GetID()).." context" )

local civ5_mode = InStrategicView ~= nil
local civBE_mode = not civ5_mode
local bnw_mode = civBE_mode or ContentManager.IsActive("6DA07636-4123-4018-B643-6575B4EC336B", ContentType.GAMEPLAY)
local gk_mode = bnw_mode or ContentManager.IsActive("0E3751A1-F840-4e1b-9706-519BF484E59D", ContentType.GAMEPLAY)
local civ5gk_mode = civ5_mode and gk_mode
local civ5bnw_mode = civ5_mode and bnw_mode
local g_currencyIcon = civ5_mode and "[ICON_GOLD]" or "[ICON_ENERGY]"
local g_maintenanceCurrency = civ5_mode and "GoldMaintenance" or "EnergyMaintenance"
local EUI = EUI
local TT = {} EUI.InfoTooltipInclude = TT
local table = EUI.table
local YieldIcons = EUI.YieldIcons
local YieldNames = EUI.YieldNames
local GreatPeopleIcon = EUI.GreatPeopleIcon
local PushScratchDeal = EUI.PushScratchDeal
local PopScratchDeal = EUI.PopScratchDeal
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query

--print( "Root contexts:", LookUpControl( "/FrontEnd" ) or "nil", LookUpControl( "/InGame" ) or "nil", LookUpControl( "/LeaderHeadRoot" ) or "nil")
-------------------------------
-- minor lua optimizations
-------------------------------
local ipairs = ipairs
local math_ceil = math.ceil
local math_floor = math.floor
local math_max = math.max
local pairs = pairs
local pcall = pcall
local select = select
local table_concat = table.concat
local table_insert = table.insert
local tonumber = tonumber
local tostring = tostring
local S = string.format

local DisputeLevelTypes = DisputeLevelTypes
local Game = Game
local GameDefines = GameDefines
local GameInfoTypes = GameInfoTypes
local GameOptionTypes = GameOptionTypes
local Locale_ToLower = Locale.ToLower
local Locale_ToUpper = Locale.ToUpper
local MajorCivApproachTypes = MajorCivApproachTypes
local OptionsManager = OptionsManager
local Players = Players
local PreGame = PreGame
local Teams = Teams
local ThreatTypes = ThreatTypes
local TradeableItems = TradeableItems
local UI_GetHeadSelectedCity = UI.GetHeadSelectedCity
local UI_GetNumCurrentDeals = UI.GetNumCurrentDeals
local UI_LoadCurrentDeal = UI.LoadCurrentDeal
local YieldTypes = YieldTypes
local L
do
	local _L = Locale.ConvertTextKey
	function L( text, ...)
		return _L( tostring(text), ... )
	end
end

local S = string.format

local g_deal = UI.GetScratchDeal()
local g_dealDuration = Game and Game.GetDealDuration()

local g_isScienceEnabled = not Game or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)
local g_isPoliciesEnabled = not Game or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)
--local g_isHappinessEnabled = not Game or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HAPPINESS)
local g_isReligionEnabled = civ5gk_mode and (not Game or not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION))
-------------------------------------------------------------
-- See if the mod for permanent pantheon bonus is activated
-------------------------------------------------------------
local g_isPermanentPantheonMod = Game and Game.IsCustomModOption("BALANCE_PERMANENT_PANTHEONS")

-- Vox Populi granted by building cache data
local g_grantedByBuilding = {}
-- cache buildings that grant others
for building in GameInfo.Buildings() do
	local function InsertIntoGrantedByBuilding(buildingType, freeBuildingClass)
		if g_grantedByBuilding[freeBuildingClass] == nil then g_grantedByBuilding[freeBuildingClass] = {} end
		table.insert(g_grantedByBuilding[freeBuildingClass], buildingType)
	end
	if building.FreeBuilding         then InsertIntoGrantedByBuilding(building.Type, building.FreeBuilding) end
	if building.FreeBuildingThisCity then InsertIntoGrantedByBuilding(building.Type, building.FreeBuildingThisCity) end
end
-- connect Civ uniques
for row in GameInfo.Civilization_BuildingClassOverrides() do
	if row.BuildingType and g_grantedByBuilding[row.BuildingClassType] then
		g_grantedByBuilding[ GameInfo.Buildings[row.BuildingType].BuildingClass ] = g_grantedByBuilding[row.BuildingClassType]
	end
end
-- Vox Populi end

local function GetCivUnit( civilizationType, unitClassType )
	if unitClassType then
		if civilizationType then
			local unit = GameInfo.Civilization_UnitClassOverrides{ CivilizationType = civilizationType, UnitClassType = unitClassType }()
			unit = unit and GameInfo.Units[ unit.UnitType ]
			if unit then
				return unit
			end
		end
		local unitClass = GameInfo.UnitClasses[ unitClassType ]
		return unitClass and GameInfo.Units[ unitClass.DefaultUnit ]
	end
end

local function GetCivBuilding( civilizationType, buildingClassType )
	if buildingClassType then
		if civilizationType then
			local building = GameInfo.Civilization_BuildingClassOverrides{ CivilizationType = civilizationType, BuildingClassType = buildingClassType }()
			building = building and GameInfo.Buildings[ building.BuildingType ]
			if building then
				return building
			end
		end
		local buildingClass = GameInfo.BuildingClasses[ buildingClassType ]
		return buildingClass and GameInfo.Buildings[ buildingClass.DefaultBuilding ]
	end
end

local function GetYieldStringSpecial( tag, format, ... )
	local tip = ""
	for row in ... do
		if (row[tag] or 0) ~=0 then
			tip = S( format, tip, row[tag], tostring(YieldIcons[ row.YieldType ]) )
		end
	end
	return tip
end
local function GetYieldString( ... )
	return GetYieldStringSpecial( "Yield", "%s %+i%s", ... )
end

local function GetCivName( player )
	-- Met
	if Teams[Game.GetActiveTeam()]:IsHasMet(player:GetTeam()) then
		return player:GetCivilizationShortDescription()
	-- Not met
	else
		return L"TXT_KEY_UNMET_PLAYER"
	end
end

local function GetLeaderName( player )
	-- You
	if player:GetID() == Game.GetActivePlayer() then
		return L"TXT_KEY_YOU"
	-- Not met
	elseif not Teams[Game.GetActiveTeam()]:IsHasMet(player:GetTeam()) then
		return L"TXT_KEY_UNMET_PLAYER"
	-- Human
	elseif player:IsHuman() then -- Game.IsGameMultiPlayer()
		local n = player:GetNickName()
		if n and n ~= "" then
			return player:GetNickName()
		end
	end
	local n = PreGame.GetLeaderName(player:GetID())
	if n and n ~= "" then
		return L( n )
	else
		return L( (GameInfo.Leaders[ player:GetLeaderType() ] or {}).Description )
	end
end


local negativeOrPositiveTextColor = { [true] = "[COLOR_POSITIVE_TEXT]", [false] = "[COLOR_WARNING_TEXT]" }

local function TextColor( c, s )
	return c..s.."[ENDCOLOR]"
end

local function UnitColor( s )
	return TextColor("[COLOR_UNIT_TEXT]", s)
end

local function BuildingColor( s )
	return TextColor("[COLOR_YIELD_FOOD]", s)
end

local function PolicyColor( s )
	return TextColor("[COLOR_MAGENTA]", s)
end

local function TechColor( s )
	return TextColor("[COLOR_CYAN]", s)
end

local function BeliefColor( s )
	return TextColor("[COLOR_WHITE]", s)
end

local function SetKey( t, key, value )
	if key then
		t[key] = value or true
	end
end

local function AddPreWrittenHelpTextAndConcat( tips, row ) -- assumes tips is custom EUI table object
	local tip = row and row.Help and L( row.Help ) or ""
	if tip ~= "" then
		tips:insertIf( #tips > 2 and "----------------" )
		tips:insert( tip )
	end
	return tips:concat( "[NEWLINE]" )
end

-- ===========================================================================
-- Help text for Corp
-- ===========================================================================
local function GetHelpTextForCorp( corpID )
	local corp = GameInfo.Corporations[ corpID ]

	-- Name
	local tips = table( Locale_ToUpper( corp.Description ), "----------------" )

	-- Pre-written Help text
	return AddPreWrittenHelpTextAndConcat( tips, corp )
end


-------------------------------------------------
-- Help text for Buildings
-------------------------------------------------

local g_gameAvailableBeliefs = Game and { Game.GetAvailablePantheonBeliefs, Game.GetAvailableFounderBeliefs, Game.GetAvailableFollowerBeliefs, Game.GetAvailableFollowerBeliefs, Game.GetAvailableEnhancerBeliefs, Game.GetAvailableBonusBeliefs }

-------------------------------------------------
-- Helper function to build religion tooltip string
-------------------------------------------------
local function GetSpecialistSlotsTooltip( specialistType, numSlots )
	local tip = ""
	for row in GameInfo.SpecialistYields{ SpecialistType = specialistType } do
		tip = S( "%s %+i%s", tip, row.Yield, tostring(YieldIcons[ row.YieldType ]) )
	end
	return S( "%i %s%s", numSlots, L( (GameInfo.Specialists[ specialistType ] or {}).Description ), tip )
end

local function GetSpecialistYields( city, specialist )
	local specialistID = specialist.ID
	local tip = ""
	if city then
		-- Culture
		local cultureFromSpecialist = city:GetCultureFromSpecialist( specialistID )
		cultureFromSpecialist = (cultureFromSpecialist + city:GetSpecialistYield( specialistID, YieldTypes.YIELD_CULTURE ) + city:GetSpecialistYieldChange( specialistID, YieldTypes.YIELD_CULTURE))

		-- Yield
		for yieldID = 0, YieldTypes.NUM_YIELD_TYPES-1 do
			-- CBP
			if(yieldID ~= YieldTypes.YIELD_CULTURE)then		
				local specialistYield = city:GetSpecialistYield( specialistID, yieldID )
				specialistYield = (specialistYield + city:GetSpecialistYieldChange( specialistID, yieldID))
				if specialistYield ~= 0 then
					tip = S( "%s %+i%s", tip, specialistYield, YieldIcons[ yieldID ] or "???" )
				end
			end
			--END
		end
		if cultureFromSpecialist > 0 then
			tip = S( "%s %+i[ICON_CULTURE]", tip, cultureFromSpecialist )
		end
	else
		for row in GameInfo.SpecialistYields{ SpecialistType = specialist.Type or -1 } do
			tip = S( "%s %+i%s", tip, row.Yield, tostring(YieldIcons[ row.YieldType ]) )
		end
	end
	if civ5_mode and (specialist.GreatPeopleRateChange or 0) > 0 then
		tip = S( "%s %+i%s", tip, specialist.GreatPeopleRateChange, GreatPeopleIcon( specialist.Type ) )
	end
	return tip
end

-- ===========================================================================
-- PLAYER PERK (civ BE only)
-- ===========================================================================

function TT.GetHelpTextForPlayerPerk( perkID )

	local perkInfo = GameInfo.PlayerPerks[ perkID ]

	-- Name
	local tips = table( Locale_ToUpper( perkInfo.Description ), "----------------" )

	-- Yield from Buildings
	for playerPerkBuildingYieldEffect in GameInfo.PlayerPerks_BuildingYieldEffects{ PlayerPerkType = perkInfo.Type } do
		local building = GameInfo.BuildingClasses[ playerPerkBuildingYieldEffect.BuildingClassType ]
		local yield = building and GameInfo.Yields[ playerPerkBuildingYieldEffect.YieldType ]
		if yield then
			tips:insertLocalized( "TXT_KEY_PLAYERPERK_ALL_BUILDING_YIELD_EFFECT", playerPerkBuildingYieldEffect.FlatYield, tostring(yield.IconString), tostring(yield.Description), building.Description )
		end
	end

	-- Pre-written Help text
	return AddPreWrittenHelpTextAndConcat( tips, perkInfo )
end

-- ===========================================================================
-- Tooltips for Yield & Similar (e.g. Culture)
-- ===========================================================================

-- ENERGY
local function GetEnergyTooltip( city )

	return L"TXT_KEY_ENERGY_HELP_INFO" .. "[NEWLINE][NEWLINE]" .. city:GetYieldRateTooltip(YieldTypes.YIELD_ENERGY)
end



-- HEALTH
function TT.GetHealthTooltip(city)

	local tips = table( L"TXT_KEY_HEALTH_HELP_INFO", "", L"TXT_KEY_HEALTH_HELP_INFO_POP_CAP_WARNING", "" )
	
	-- base Health
	local localHealth = city:GetLocalHealth()
	local healthFromTerrain = city:GetHealthFromTerrain()
	local healthFromBuildings = city:GetHealthFromBuildings()	
	local baseLocalHealth = healthFromTerrain + healthFromBuildings
		
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_BUILDINGS", healthFromBuildings, "[ICON_HEALTH_1]" )
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_TERRAIN", healthFromTerrain, "[ICON_HEALTH_1]" )
	tips:insertLocalized( "TXT_KEY_YIELD_BASE", baseLocalHealth, "[ICON_HEALTH_1]" )
	tips:insert( "----------------" )

	-- health Modifier
	local healthModifier = city:GetTotalHealthModifier() - 100
	if healthModifier ~= 0 then
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_HEALTH_BUILDING_MOD_INFO", healthModifier )
		tips:insert( "----------------" )
	end

	-- total Health
	tips:insertLocalized( "TXT_KEY_YIELD_TOTAL", localHealth, localHealth < 0 and "[ICON_HEALTH_3]" or "[ICON_HEALTH_1]" )

	-- if local health is positive and our output is higher than the city's population, include the cap reminder
	if baseLocalHealth > localHealth then		
		tips:append( L("TXT_KEY_HEALTH_POP_CAPPED", city:GetPopulation() ) )
	end

	return tips:concat( "[NEWLINE]" )
end

local g_relationshipDuration = gk_mode and GameInfo.GameSpeeds[ PreGame.GetGameSpeed() ].RelationshipDuration or 50

-------------------------------------------------
-- Expose functions
-------------------------------------------------

if Game and Game.GetReligionName == nil then
	if not GetCityStateStatusToolTip then
		include( "CityStateStatusHelper" )
	end
	local GetCityStateStatusToolTip = GetCityStateStatusToolTip
	function TT.GetCityStateStatus( minorPlayer, majorPlayerID )
		return GetCityStateStatusToolTip( majorPlayerID, minorPlayer:GetID(), true )
	end
end
function TT.GetEnergyTooltip( ... ) return select(2, pcall( GetEnergyTooltip, ... ) ) end
--CBP
function TT.GetHelpTextForCorp( ... ) return select(2, pcall( GetHelpTextForCorp, ... ) ) end
--END

if civBE_mode then
	local GetHelpTextForUnitAttributes, GetHelpTextForUnitPlayerPerkBuffs, GetHelpTextForUnitInherentPromotions, GetHelpTextForUnitPromotions, GetHelpTextForUnitPerks, GetHelpTextForAffinityLevel
	----------------------------------------------------------------
	----------------------------------------------------------------
	-- UNIT COMBO THINGS
	----------------------------------------------------------------
	----------------------------------------------------------------
	function TT.GetHelpTextForSpecificUnit(unit)
		local s = "";

		-- Attributes
		s = s .. GetHelpTextForUnitAttributes(unit:GetUnitType(), "[ICON_BULLET]");

		-- Player Perks
		local temp = GetHelpTextForUnitPlayerPerkBuffs(unit:GetUnitType(), unit:GetOwner(), "[ICON_BULLET]");
		if temp ~= "" then
			if s ~= "" then
				s = s .. "[NEWLINE]";
			end
			s = s .. temp;
		end

		-- Promotions
		temp = GetHelpTextForUnitPromotions(unit, "[ICON_BULLET]");
		if temp ~= "" then
			if s ~= "" then
				s = s .. "[NEWLINE]";
			end
			s = s .. temp;
		end

		-- Upgrades and Perks
		local player = Players[unit:GetOwner()];
		if player then
			local allPerks = player:GetPerksForUnit(unit:GetUnitType());
			local tempPerks = player:GetFreePerksForUnit(unit:GetUnitType());
			for _, perk in ipairs(tempPerks) do
				table_insert(allPerks, perk);
			end
			local ignoreCoreStats = true;
			temp = GetHelpTextForUnitPerks(allPerks, ignoreCoreStats, "[ICON_BULLET]");
			if temp ~= "" then
				if s ~= "" then
					s = s .. "[NEWLINE]";
				end
				s = s .. temp;
			end
		end

		return s;
	end

	function TT.GetHelpTextForUnitType(unitType, playerID, includeFreePromotions)
		local s = "";

		-- Attributes
		s = s .. GetHelpTextForUnitAttributes(unitType, nil);

		-- Player Perks
		if includeFreePromotions and includeFreePromotions == true then
			local temp = GetHelpTextForUnitPlayerPerkBuffs(unitType, playerID, nil);
			if temp ~= "" then
				if s ~= "" then
					s = s .. "[NEWLINE]";
				end
				s = s .. temp;
			end
		end

		-- Promotions
		if includeFreePromotions and includeFreePromotions == true then
			local temp = GetHelpTextForUnitInherentPromotions(unitType, nil);
			if temp ~= "" then
				if s ~= "" then
					s = s .. "[NEWLINE]";
				end
				s = s .. temp;
			end
		end

		-- Upgrades and Perks
		local player = Players[playerID];
		if player then
			local allPerks = player:GetPerksForUnit(unitType);
			local tempPerks = player:GetFreePerksForUnit(unitType);
			for _, perk in ipairs(tempPerks) do
				table_insert(allPerks, perk);
			end
			local ignoreCoreStats = true;
			local temp = GetHelpTextForUnitPerks(allPerks, ignoreCoreStats, nil);
			if temp ~= "" then
				if s ~= "" then
					s = s .. "[NEWLINE]";
				end
				s = s .. temp;
			end
		end

		return s;
	end

	----------------------------------------------------------------
	----------------------------------------------------------------
	-- UNIT MISCELLANY
	-- Stuff not covered by promotions or perks
	----------------------------------------------------------------
	----------------------------------------------------------------
	function TT.GetUpgradedUnitDescriptionKey(player, unitType)
		local descriptionKey = "";
		local unitInfo = GameInfo.Units[unitType];
		if unitInfo then
			descriptionKey = unitInfo.Description;
			if player then
				local bestUpgrade = player:GetBestUnitUpgrade(unitType);
				if bestUpgrade ~= -1 then
					local bestUpgradeInfo = GameInfo.UnitUpgrades[bestUpgrade];
					if bestUpgradeInfo then
						descriptionKey = bestUpgradeInfo.Description;
					end
				end
			end
		end
		return descriptionKey;
	end

	--TODO: antonjs: Once we have a text budget and refactor time,
	--roll these miscellaneous things (player perks, attributes in
	--the base unit XML) in with existing unit buff systems
	--instead of being special case like this.
	function TT.GetHelpTextForUnitAttributes(unitType, prefix)
		local s = "";
		local unitInfo = GameInfo.Units[unitType];
		if unitInfo then
			if unitInfo.OrbitalAttackRange >= 0 then
				if s ~= "" then
					s = s .. "[NEWLINE]";
				end
				if prefix then
					s = s .. prefix;
				end
				s = s .. L("TXT_KEY_INTERFACEMODE_ORBITAL_ATTACK");
			end
		end
		return s;
	end
	GetHelpTextForUnitAttributes = TT.GetHelpTextForUnitAttributes

	function TT.GetHelpTextForUnitPlayerPerkBuffs(unitType, playerID, prefix)
		local s = "";
		local player = Players[playerID];
		local unitInfo = GameInfo.Units[ unitType ]
		if player and unitInfo then
			for info in GameInfo.PlayerPerks() do
				if player:HasPerk(info.ID) then
					if info.MiasmaBaseHeal > 0 or info.UnitPercentHealChange > 0 then
						if s ~= "" then
							s = s .. "[NEWLINE]";
						end
						if prefix then
							s = s .. prefix;
						end
						s = s .. L(GameInfo.PlayerPerks[info.ID].Help);
					end

					-- Help text for this player perk is inaccurate. Commencing hax0rs.
					if info.UnitFlatVisibilityChange > 0 then
						if s ~= "" then
							s = s .. "[NEWLINE]";
						end
						if prefix then
							s = s .. prefix;
						end
						s = s .. L("TXT_KEY_UNITPERK_VISIBILITY_CHANGE", info.UnitFlatVisibilityChange);
					end
				end
			end
		end
		return s;
	end
	GetHelpTextForUnitPlayerPerkBuffs = TT.GetHelpTextForUnitPlayerPerkBuffs

	----------------------------------------------------------------
	----------------------------------------------------------------
	-- UNIT PROMOTIONS
	----------------------------------------------------------------
	----------------------------------------------------------------
	function TT.GetHelpTextForUnitInherentPromotions(unitType, prefix)
		local s = "";
		local unitInfo = GameInfo.Units[unitType];
		if unitInfo then
			for pairInfo in GameInfo.Unit_FreePromotions{ UnitType = unitInfo.Type } do
				local promotionInfo = GameInfo.UnitPromotions[pairInfo.PromotionType];
				if promotionInfo and promotionInfo.Help then
					if s ~= "" then
						s = s .. "[NEWLINE]";
					end
					if prefix then
						s = s .. prefix;
					end
					s = s .. L(promotionInfo.Help);
				end
			end
		end
		return s;
	end
	GetHelpTextForUnitInherentPromotions = TT.GetHelpTextForUnitInherentPromotions

	function TT.GetHelpTextForUnitPromotions(unit, prefix)
		local s = "";
		for promotionInfo in GameInfo.UnitPromotions() do
			if unit:IsHasPromotion(promotionInfo.ID) then
				if s ~= "" then
					s = s .. "[NEWLINE]";
				end
				if prefix then
					s = s .. prefix;
				end
				s = s .. L(promotionInfo.Help);
			end
		end
		return s;
	end

	----------------------------------------------------------------
	----------------------------------------------------------------
	-- UNIT PERKS
	----------------------------------------------------------------
	----------------------------------------------------------------

	local function ComposeUnitPerkNumberHelpText(perkIDTable, textKey, numberKey, firstEntry, prefix)
		local s = "";
		local number = 0;
		for _, perkID in ipairs(perkIDTable) do
			local perkInfo = GameInfo.UnitPerks[perkID];
			if perkInfo and perkInfo[numberKey] and perkInfo[numberKey] ~= 0 then
				number = number + perkInfo[numberKey];
			end
		end

		if number ~= 0 then
			if not firstEntry then
				s = s .. "[NEWLINE]";
			end
			if prefix then
				s = s .. prefix;
			end
			s = s .. L(textKey, number);
		end

		return s;
	end

	local function ComposeUnitPerkFlagHelpText(perkIDTable, textKey, flagKey, firstEntry, prefix)
		local s = "";
		local flag = false;
		for _, perkID in ipairs(perkIDTable) do
			local perkInfo = GameInfo.UnitPerks[perkID];
			if perkInfo and perkInfo[flagKey] and perkInfo[flagKey] then
				flag = true;
				break;
			end
		end

		if flag then
			if not firstEntry then
				s = s .. "[NEWLINE]";
			end
			if prefix then
				s = s .. prefix;
			end
			s = s .. L(textKey);
		end

		return s;
	end


	local function ComposeUnitPerkDomainCombatModHelpText(perkIDTable, textKey, domainKey, firstEntry, prefix)
		local s = "";
		local number = 0;
		for _, perkID in ipairs(perkIDTable) do
			local perkInfo = GameInfo.UnitPerks[perkID];
			if perkInfo then
				for domainCombatInfo in GameInfo.UnitPerks_DomainCombatMods("UnitPerkType = \"" .. perkInfo.Type .. "\" AND DomainType = \"" .. domainKey .. "\"") do
					if domainCombatInfo.CombatMod ~= 0 then
						number = number + domainCombatInfo.CombatMod;
					end
				end
			end
		end

		if number ~= 0 then
			if not firstEntry then
				s = s .. "[NEWLINE]";
			end
			if prefix then
				s = s .. prefix;
			end
			s = s .. L(textKey, number);
		end

		return s;
	end

	function GetHelpTextForUnitPerks(perkIDTable, ignoreCoreStats, prefix)
		local s = "";

		-- Text key overrides
		local filteredPerkIDTable = {};
		for _, perkID in ipairs(perkIDTable) do
			local perkInfo = GameInfo.UnitPerks[perkID];
			if perkInfo then
				if perkInfo.Help then
					if s ~= "" then
						s = s .. "[NEWLINE]";
					end
					if prefix then
						s = s .. prefix;
					end
					s = s .. L(perkInfo.Help);
				else
					table_insert(filteredPerkIDTable, perkID);
				end
			end
		end

		-- Basic Attributes
		if not ignoreCoreStats then
			s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_EXTRA_COMBAT_STRENGTH", "ExtraCombatStrength", s == "", prefix);
			s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_EXTRA_RANGED_COMBAT_STRENGTH", "ExtraRangedCombatStrength", s == "", prefix);
		end
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGE_CHANGE", "RangeChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGE_AT_FULL_HEALTH_CHANGE", "RangeAtFullHealthChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGE_AT_FULL_MOVES_CHANGE", "RangeAtFullMovesChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGE_FOR_ONBOARD_CHANGE", "RangeForOnboardChange", s == "", prefix);
		if not ignoreCoreStats then
			s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_MOVES_CHANGE", "MovesChange", s == "", prefix);
		end
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_VISIBILITY_CHANGE", "VisibilityChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_CARGO_CHANGE", "CargoChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGE_AGAINST_ORBITAL_CHANGE", "RangeAgainstOrbitalChange", s == "", prefix);
		-- General combat
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_MOD", "AttackMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_FORTIFIED_MOD", "AttackFortifiedMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_WOUNDED_MOD", "AttackWoundedMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_WHILE_IN_MIASMA_MOD", "AttackWhileInMiasmaMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_CITY_MOD", "AttackCityMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_FOR_ONBOARD_MOD", "AttackForOnboardMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DEFEND_MOD", "DefendMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DEFEND_RANGED_MOD", "DefendRangedMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DEFEND_WHILE_IN_MIASMA_MOD", "DefendWhileInMiasmaMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DEFEND_FOR_ONBOARD_MOD", "DefendForOnboardMod", s == "", prefix);
		-- Air combat
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_WITH_AIR_SWEEP_MOD", "AttackWithAirSweepMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ATTACK_WITH_INTERCEPTION_MOD", "AttackWithInterceptionMod", s == "", prefix);
		-- Territory
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_FRIENDLY_LANDS_MOD", "FriendlyLandsMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_OUTSIDE_FRIENDLY_LANDS_MOD", "OutsideFriendlyLandsMod", s == "", prefix);
		-- Battlefield position
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ADJACENT_FRIENDLY_MOD", "AdjacentFriendlyMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_PER_ADJACENT_FRIENDLY_MOD", "PerAdjacentFriendlyMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_NO_ADJACENT_FRIENDLY_MOD", "NoAdjacentFriendlyMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_FLANKING_MOD", "FlankingMod", s == "", prefix);
		-- Other conditional bonuses
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ALIEN_COMBAT_MOD", "AlienCombatMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_FORTIFIED_MOD", "FortifiedMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_CITY_MOD", "CityMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_PER_UNUSED_MOVE_MOD", "PerUnusedMoveMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DAMAGE_TO_ADJACENT_UNITS_ON_DEATH", "DamageToAdjacentUnitsOnDeath", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DAMAGE_TO_ADJACENT_UNITS_ON_ATTACK", "DamageToAdjacentUnitsOnAttack", s == "", prefix);
		-- Attack logistics
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_IGNORE_RANGED_ATTACK_LINE_OF_SIGHT", "IgnoreRangedAttackLineOfSight", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_MELEE_ATTACK_HEAVY_CHARGE", "MeleeAttackHeavyCharge", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_EXTRA_ATTACKS", "ExtraAttacks", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_EXTRA_INTERCEPTIONS", "ExtraInterceptions", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGED_ATTACK_SETUPS_NEEDED_MOD", "RangedAttackSetupsNeededMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_RANGED_ATTACK_SCATTER_CHANCE_MOD", "RangedAttackScatterChanceMod", s == "", prefix);
		-- Movement logistics
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_MOVE_AFTER_ATTACKING", "MoveAfterAttacking", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_IGNORE_TERRAIN_COST", "IgnoreTerrainCost", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_IGNORE_PILLAGE_COST", "IgnorePillageCost", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_IGNORE_ZONE_OF_CONTROL", "IgnoreZoneOfControl", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_FLAT_MOVEMENT_COST", "FlatMovementCost", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_MOVE_ANYWHERE", "MoveAnywhere", s == "", prefix);
		-- Don't show "Hover", since it is redundant with the descriptions for "FlatMovementCost" and "MoveAnywhere"
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_WITHDRAW_FROM_MELEE_CHANCE_MOD", "WithdrawFromMeleeChanceMod", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_FREE_REBASES", "FreeRebases", s == "", prefix);
		-- Healing
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ALWAYS_HEAL", "AlwaysHeal", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_HEAL_OUTSIDE_FRIENDLY_TERRITORY", "HealOutsideFriendlyTerritory", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ENEMY_HEAL_CHANGE", "EnemyHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_NEUTRAL_HEAL_CHANGE", "NeutralHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_FRIENDLY_HEAL_CHANGE", "FriendlyHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_MIASMA_HEAL_CHANGE", "MiasmaHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ADJACENT_UNIT_HEAL_CHANGE", "AdjacentUnitHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_SAME_TILE_HEAL_CHANGE", "SameTileHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_KILL_UNIT_HEAL_CHANGE", "KillUnitHealChange", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_PILLAGE_HEAL_CHANGE", "PillageHealChange", s == "", prefix);
		-- Orbital layer
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_GENERATE_MIASMA_IN_ORBIT", "GenerateMiasmaInOrbit", s == "", prefix);
		s = s .. ComposeUnitPerkFlagHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ALLOW_MANUAL_DEORBIT", "AllowManualDeorbit", s == "", prefix);
		s = s .. ComposeUnitPerkNumberHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_ORBITAL_COVERAGE_RADIUS_CHANGE", "OrbitalCoverageRadiusChange", s == "", prefix);
		-- Attrition
		-- Actions
		-- Domain combat mods
		s = s .. ComposeUnitPerkDomainCombatModHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DOMAIN_COMBAT_MOD_LAND", "DOMAIN_LAND", s == "", prefix);
		s = s .. ComposeUnitPerkDomainCombatModHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DOMAIN_COMBAT_MOD_SEA", "DOMAIN_SEA", s == "", prefix);
		s = s .. ComposeUnitPerkDomainCombatModHelpText(filteredPerkIDTable, "TXT_KEY_UNITPERK_DOMAIN_COMBAT_MOD_AIR", "DOMAIN_AIR", s == "", prefix);

		return s;
	end
	TT.GetHelpTextForUnitPerks = GetHelpTextForUnitPerks

	function TT.GetHelpTextForUnitPerk( perkID )
		return GetHelpTextForUnitPerks( {perkID}, false )
	end

	----------------------------------------------------------------
	----------------------------------------------------------------
	-- VIRTUES
	----------------------------------------------------------------
	----------------------------------------------------------------

	local function ComposeVirtueNumberHelpText(virtueIDTable, textKey, numberKey, firstEntry, postProcessFunction)
		local s = "";
		local number = 0;
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo and virtueInfo[numberKey] and virtueInfo[numberKey] ~= 0 then
				number = number + virtueInfo[numberKey];
			end
		end

		if number ~= 0 then
			if postProcessFunction then
				number = postProcessFunction(number);
			end
			if not firstEntry then
				s = s .. "[NEWLINE]";
			else
				firstEntry = false;
			end
			s = s .. "[ICON_BULLET]";
			s = s .. L(textKey, number);
		end

		return s;
	end

	local function ComposeVirtueFlagHelpText(virtueIDTable, textKey, flagKey, firstEntry)
		local s = "";
		local flag = false;
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo and virtueInfo[flagKey] and virtueInfo[flagKey] then
				flag = true;
				break;
			end
		end

		if flag then
			if not firstEntry then
				s = s .. "[NEWLINE]";
			else
				firstEntry = false;
			end
			s = s .. "[ICON_BULLET]";
			s = s .. L(textKey);
		end

		return s;
	end

	local function ComposeVirtueInterestHelpText(virtueIDTable, textKey, numberKey, firstEntry)
		local s = "";
		local interestPercent = 0;
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo and virtueInfo[numberKey] and virtueInfo[numberKey] ~= 0 then
				interestPercent = interestPercent + virtueInfo[numberKey];
			end
		end

		if interestPercent ~= 0 then
			local maximum = (interestPercent * GameDefines["ENERGY_INTEREST_PRINCIPAL_MAXIMUM"]) / 100;
			if not firstEntry then
				s = s .. "[NEWLINE]";
			else
				firstEntry = false;
			end
			s = s .. "[ICON_BULLET]";
			s = s .. L(textKey, interestPercent, maximum);
		end

		return s;
	end

	local function ComposeVirtueYieldHelpText(virtueIDTable, textKey, tableKey, firstEntry, postProcessFunction)
		local s = "";
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo and GameInfo[tableKey] then
				for tableInfo in GameInfo[tableKey]("PolicyType = \"" .. virtueInfo.Type .. "\"") do
					if tableInfo.YieldType and tableInfo.Yield then
						local yieldInfo = GameInfo.Yields[tableInfo.YieldType];
						local yieldNumber = tableInfo.Yield;
						if yieldNumber ~= 0 then
							if postProcessFunction then
								yieldNumber = postProcessFunction(yieldNumber);
							end
							if not firstEntry then
								s = s .. "[NEWLINE]";
							else
								firstEntry = false;
							end
							s = s .. "[ICON_BULLET]";
							s = s .. L( textKey, yieldNumber, tostring(yieldInfo.IconString), tostring(yieldInfo.Description) );
						end
					end
				end
			end
		end
		return s;
	end

	local function ComposeVirtueResourceClassYieldHelpText(virtueIDTable, textKey, firstEntry)
		local s = "";
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo then
				for tableInfo in GameInfo.Policy_ResourceClassYieldChanges("PolicyType = \"" .. virtueInfo.Type .. "\"") do
					local resourceClassInfo = GameInfo.ResourceClasses[tableInfo.ResourceClassType];
					local yieldInfo = GameInfo.Yields[tableInfo.YieldType];
					local yieldNumber = tableInfo.YieldChange;
					if yieldNumber ~= 0 then
						if not firstEntry then
							s = s .. "[NEWLINE]";
						else
							firstEntry = false;
						end
						s = s .. "[ICON_BULLET]";
						s = s .. L( textKey, yieldNumber, tostring(yieldInfo.IconString), tostring(yieldInfo.Description), tostring(resourceClassInfo.Description) );
					end
				end
			end
		end
		return s;
	end

	local function ComposeVirtueImprovementYieldHelpText(virtueIDTable, textKey, firstEntry)
		local s = "";
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo then
				for tableInfo in GameInfo.Policy_ImprovementYieldChanges("PolicyType = \"" .. virtueInfo.Type .. "\"") do
					local improvementInfo = GameInfo.Improvements[tableInfo.ImprovementType];
					local yieldInfo = GameInfo.Yields[tableInfo.YieldType];
					local yieldNumber = tableInfo.Yield;
					if yieldNumber ~= 0 then
						if not firstEntry then
							s = s .. "[NEWLINE]";
						else
							firstEntry = false;
						end
						s = s .. "[ICON_BULLET]";
						s = s .. L(textKey, yieldNumber, tostring(yieldInfo.IconString), tostring(yieldInfo.Description), tostring(improvementInfo.Description) );
					end
				end
			end
		end
		return s;
	end

	local function ComposeVirtueFreeUnitHelpText(virtueIDTable, textKey, firstEntry)
		local s = "";
		for _, virtueID in ipairs(virtueIDTable) do
			local virtueInfo = GameInfo.Policies[virtueID];
			if virtueInfo then
				for tableInfo in GameInfo.Policy_FreeUnitClasses("PolicyType = \"" .. virtueInfo.Type .. "\"") do
					local unitClassInfo = GameInfo.UnitClasses[tableInfo.UnitClassType];
					local unitInfo = GameInfo.Units[unitClassInfo.DefaultUnit];
					if unitInfo then
						if not firstEntry then
							s = s .. "[NEWLINE]";
						else
							firstEntry = false;
						end
						s = s .. "[ICON_BULLET]";
						s = s .. L(textKey, unitInfo.Description);
					end
				end
			end
		end
		return s;
	end

	local function GetHelpTextForVirtues(virtueIDTable)
		local s = "";

		-- Post-processing functions to display values more clearly to player
		local divByHundred = function(number)
			-- Some database values are in multiplied by 100 to match game core usage
			number = number * 0.01;
			return number;
		end;
		local flipSign = function(number)
			number = number * -1;
			return number;
		end;
		local modByGameResearchSpeed = function(number)
			local gameSpeedResearchMod = 1;
			if Game then
				gameSpeedResearchMod = Game.GetResearchPercent() / 100;
			end
			number = number * gameSpeedResearchMod;
			number = math_floor(number); -- for display, truncate trailing decimals
			return number;
		end;

		s = s .. ComposeVirtueFlagHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CAPTURE_OUTPOSTS_FOR_SELF", "CaptureOutpostsForSelf", s == "");

		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_BARBARIAN_COMBAT_BONUS", "BarbarianCombatBonus", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_RESEARCH_FROM_BARBARIAN_KILLS", "ResearchFromBarbarianKills", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_RESEARCH_FROM_BARBARIAN_CAMPS", "ResearchFromBarbarianCamps", s == "", modByGameResearchSpeed); --value modified by game speed
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_EXP_MODIFIER", "ExpModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_MILITARY_PRODUCTION_MODIFIER", "MilitaryProductionModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_HEALTH_PER_MILITARY_UNIT_TIMES_100", "HealthPerMilitaryUnitTimes100", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_TECH_AFFINITY_XP_MODIFIER", "TechAffinityXPModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_COVERT_OPS_INTRIGUE_MODIFIER", "CovertOpsIntrigueModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NUM_FREE_AFFINITY_LEVELS", "NumFreeAffinityLevels", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_UNIT_PRODUCTION_MODIFIER_PER_UPGRADE", "UnitProductionModifierPerUpgrade", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_STRATEGIC_RESOURCE_MOD", "StrategicResourceMod", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_ORBITAL_COVERAGE_RADIUS_FROM_STATION_TRADE", "OrbitalCoverageRadiusFromStationTrade", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_UNIT_GOLD_MAINTENANCE_MOD", "UnitGoldMaintenanceMod", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_COMBAT_MODIFIER", "CombatModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_OUTPOST_GROWTH_MODIFIER", "OutpostGrowthModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_FOOD_KEPT_AFTER_GROWTH_PERCENT", "FoodKeptAfterGrowthPercent", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_WORKER_SPEED_MODIFIER", "WorkerSpeedModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_PLOT_CULTURE_COST_MODIFIER", "PlotCultureCostModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_EXPLORER_EXPEDITION_CHARGES", "ExplorerExpeditionCharges", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_LAND_TRADE_ROUTE_GOLD_CHANGE", "LandTradeRouteGoldChange", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_SEA_TRADE_ROUTE_GOLD_CHANGE", "SeaTradeRouteGoldChange", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NEW_CITY_EXTRA_POPULATION", "NewCityExtraPopulation", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_EXTRA_HEALTH", "ExtraHealth", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_EXTRA_HEALTH_PER_LUXURY", "HealthPerBasicResourceTypeTimes100", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_UNHEALTH_MOD", "UnhealthMod", s == "", flipSign); --less confusing to show as a positive number in text
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_RESEARCH_MOD_PER_EXTRA_CONNECTED_TECH", "ResearchModPerExtraConnectedTech", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_HEALTH_TO_SCIENCE", "HealthToScience", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_HEALTH_TO_CULTURE", "HealthToCulture", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_POLICY_COST_MODIFIER", "PolicyCostModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_PERCENT_CULTURE_RATE_TO_ENERGY", "PercentCultureRateToEnergy", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_HEALTH_PER_X_POPULATION", "HealthPerXPopulation", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NUM_CITIES_RESEARCH_COST_DISCOUNT", "NumCitiesResearchCostDiscount", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NUM_CITIES_POLICY_COST_DISCOUNT", "NumCitiesPolicyCostDiscount", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_LEAF_TECH_RESEARCH_MODIFIER", "LeafTechResearchModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_PERCENT_CULTURE_RATE_TO_RESEARCH", "PercentCultureRateToResearch", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CULTURE_PER_WONDER", "CulturePerWonder", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_BUILDING_PRODUCTION_MODIFIER", "BuildingProductionModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_WONDER_PRODUCTION_MODIFIER", "WonderProductionModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_BUILDING_ALREADY_IN_CAPITAL_MODIFIER", "BuildingAlreadyInCapitalModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_INTERNAL_TRADE_ROUTE_YIELD_MODIFIER", "InternalTradeRouteYieldModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_HEALTH_PER_TRADE_ROUTE_TIMES_100", "HealthPerTradeRouteTimes100", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_ORBITAL_PRODUCTION_MODIFIER", "OrbitalProductionModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_ORBITAL_DURATION_MODIFIER", "OrbitalDurationModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_UNIT_PURCHASE_COST_MODIFIER", "UnitPurchaseCostModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_HEALTH_PER_BUILDING_TIMES_100", "HealthPerBuildingTimes100", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_EXTRA_HEALTH_PER_CITY", "ExtraHealthPerCity", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NUM_FREE_TECHS", "NumFreeTechs", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NUM_FREE_POLICIES", "NumFreePolicies", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_NUM_FREE_COVERT_AGENTS", "NumFreeCovertAgents", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_ORBITAL_COVERAGE_MODIFIER", "OrbitalCoverageModifier", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_RESEARCH_FROM_EXPEDITIONS", "ResearchFromExpeditions", s == "", modByGameResearchSpeed); --value modified by game speed
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CITY_GROWTH_MOD", "CityGrowthMod", s == "");
		s = s .. ComposeVirtueNumberHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CAPITAL_GROWTH_MOD", "CapitalGrowthMod", s == "");

		s = s .. ComposeVirtueInterestHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_ENERGY_INTEREST_PERCENT_PER_TURN", "EnergyInterestPercentPerTurn", s == "");

		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_YIELD_MODIFIER", "Policy_YieldModifiers", s == "");
		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CAPITAL_YIELD_MODIFIER", "Policy_CapitalYieldModifiers", s == "");
		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CITY_YIELD_CHANGE", "Policy_CityYieldChanges", s == "");
		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CITY_YIELD_PER_POP_CHANGE", "Policy_CityYieldPerPopChanges", s == "", divByHundred); --value in hundredths
		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_CAPITAL_YIELD_CHANGE", "Policy_CapitalYieldChanges", s == "");
		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_SPECIALIST_EXTRA_YIELD", "Policy_SpecialistExtraYields", s == "");
		s = s .. ComposeVirtueYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_TRADE_ROUTE_WITH_STATION_PER_TIER_YIELD_CHANGE", "Policy_TradeRouteWithStationPerTierYieldChanges", s == "");

		s = s .. ComposeVirtueResourceClassYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_RESOURCE_CLASS_YIELD_CHANGE", s == "");

		s = s .. ComposeVirtueImprovementYieldHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_IMPROVEMENT_YIELD_CHANGE", s == "");

		s = s .. ComposeVirtueFreeUnitHelpText(virtueIDTable, "TXT_KEY_POLICY_EFFECT_FREE_UNIT_CLASS", s == "");

		return s;
	end
	TT.GetHelpTextForVirtues = GetHelpTextForVirtues

	function TT.GetHelpTextForVirtue(virtueID)
		return GetHelpTextForVirtues{virtueID}
	end

	----------------------------------------------------------------
	----------------------------------------------------------------
	-- AFFINITIES
	----------------------------------------------------------------
	----------------------------------------------------------------
	function TT.GetHelpTextForAffinity(affinityID, player)
		local s = "";
		local affinityInfo = GameInfo.Affinity_Types[affinityID]
		if affinityInfo then

			local currentLevel = -1;
			if player then
				-- Current level
				s = s .. L("TXT_KEY_AFFINITY_STATUS_DETAIL", tostring(affinityInfo.IconString), affinityInfo.ColorType, affinityInfo.Description, player:GetAffinityLevel(affinityInfo.ID));
				s = s .. "[NEWLINE][NEWLINE]";
				currentLevel = player:GetAffinityLevel(affinityID);
			end

			-- Player perks we can earn
			local firstEntry = true;
			for levelInfo in GameInfo.Affinity_Levels() do
				local levelText = GetHelpTextForAffinityLevel(affinityID, levelInfo.ID);
				if levelText ~= "" then
					levelText = tostring(affinityInfo.IconString) .. "[" .. affinityInfo.ColorType .. "]" .. levelInfo.ID .. "[ENDCOLOR] : " .. levelText;

					if levelInfo.ID <= currentLevel then
						levelText = "[" .. affinityInfo.ColorType .. "]" .. levelText .. "[ENDCOLOR]";
					end

					if firstEntry then
						firstEntry = false;
					else
						s = s .. "[NEWLINE]";
					end

					s = s .. levelText;
				end
			end

			if player then
				local nextLevel = player:GetAffinityLevel(affinityID) + 1;
				local nextLevelInfo = GameInfo.Affinity_Levels[nextLevel];

				-- Progress towards next level
				if nextLevelInfo then
					s = s .. "[NEWLINE][NEWLINE]";
					s = s .. L("TXT_KEY_AFFINITY_STATUS_PROGRESS", player:GetAffinityScoreTowardsNextLevel(affinityInfo.ID), player:CalculateAffinityScoreNeededForNextLevel(affinityInfo.ID));
				else
					s = s .. "[NEWLINE][NEWLINE]";
					s = s .. L("TXT_KEY_AFFINITY_STATUS_MAX_LEVEL");
				end

				-- Dominance
				local isDominant = affinityInfo.ID == player:GetDominantAffinityType();
				if nextLevelInfo then
					local penalty = nextLevelInfo.AffinityValueNeededAsNonDominant - nextLevelInfo.AffinityValueNeededAsDominant;
					-- Only show dominance once we are at the point where the level curve diverges
					if penalty > 0 then
						if isDominant then
							s = s .. "[NEWLINE][NEWLINE]";
							s = s .. L("TXT_KEY_AFFINITY_STATUS_DOMINANT");
						else
							s = s .. "[NEWLINE][NEWLINE]";
							s = s .. L("TXT_KEY_AFFINITY_STATUS_NON_DOMINANT_PENALTY", penalty);
						end
					end
				elseif isDominant then
					-- Or once we have reached max level
					s = s .. "[NEWLINE][NEWLINE]";
					s = s .. L("TXT_KEY_AFFINITY_STATUS_DOMINANT");
				end
			end
		end
		return s;
	end

	local g_playerPerkKey = { [GameInfoTypes.AFFINITY_TYPE_HARMONY] = "HarmonyPlayerPerk", [GameInfoTypes.AFFINITY_TYPE_PURITY] = "PurityPlayerPerk", [GameInfoTypes.AFFINITY_TYPE_SUPREMACY] = "SupremacyPlayerPerk" }
	-- Does not include unit upgrade unlocks
	function GetHelpTextForAffinityLevel( affinityID, affinityLevel )
		local tips = table()
		local affinityInfo = GameInfo.Affinity_Types[ affinityID ]
		local affinityLevelInfo = GameInfo.Affinity_Levels[ affinityLevel ]
		if affinityInfo and affinityLevelInfo then

			-- Gained a Player Perk?
			local perkInfo = GameInfo.PlayerPerks[ affinityLevelInfo[ g_playerPerkKey[affinityID] ] ]
			tips:insertLocalizedIf( perkInfo and perkInfo.Help )

			-- Unlocked Covert Ops?
			for row in GameInfo.CovertOperation_AffinityPrereqs{ AffinityType = affinityInfo.Type, Level = affinityLevel } do
				local covertOpInfo = GameInfo.CovertOperations[ row.CovertOperationType ] or {}
				tips:insertLocalized( "TXT_KEY_AFFINITY_LEVEL_UP_DETAILS_COVERT_OP_UNLOCKED", covertOpInfo.Description )
			end

			-- Unlocked Projects (for Victory)?
			for row in GameInfo.Project_AffinityPrereqs{ AffinityType = affinityInfo.Type, Level = affinityLevel } do
				local projectInfo = GameInfo.Projects[ row.ProjectType ] or {}
				local victoryInfo = GameInfo.Victories[ projectInfo.VictoryPrereq ] or {}
				tips:insertLocalized( "TXT_KEY_AFFINITY_LEVEL_UP_DETAILS_PROJECT_UNLOCKED", projectInfo.Description, victoryInfo.Description )
			end

			-- Unlocked Units
			for affinity in GameInfo.Unit_AffinityPrereqs{ AffinityType = affinityInfo.Type, Level = affinityLevel } do
				local unit = GameInfo.Units[affinity.UnitType]
				if unit then
					local tip = UnitColor( L(unit.Description) )
					if (unit.RangedCombat or 0) > 0 then
						tip = S("%s %i[ICON_RANGE_STRENGTH]", tip, unit.RangedCombat )
					elseif (unit.Combat or 0) > 0 then
						tip = S("%s %i[ICON_STRENGTH]", tip, unit.Combat )
					end
					tips:insert( tip )
				end
			end

			-- Unlocked Buildings
			for affinity in GameInfo.Building_AffinityPrereqs{ AffinityType = affinityInfo.Type, Level = affinityLevel } do
				local building = GameInfo.Buildings[ affinity.BuildingType ]
				tips:insertIf( building and BuildingColor( L(building.Description) ) )
			end
		end
		return tips:concat( ", " )
	end
	TT. GetHelpTextForAffinityLevel = GetHelpTextForAffinityLevel
	function TT.CacheDatabaseQueries() end
end
for k, v in pairs(TT) do
	EUI[k] = v
end

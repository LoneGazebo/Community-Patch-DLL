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

-------------------------------------------------
-- Help text for Units
-------------------------------------------------

local function GetHelpTextForUnit( unitID ) -- isIncludeRequirementsInfo )
	local unit = GameInfo.Units[ unitID ]
	if not unit then
		return "<Unit undefined in game database>"
	end

	-- Unit XML stats
	local productionCost = unit.Cost
	local rangedStrength = unit.RangedCombat
	local unitRange = unit.Range
	local combatStrength = unit.Combat
	local unitMoves = unit.Moves
	local airstrike = unit.BaseLandAirDefense 

	local unitDomainType = unit.Domain

	local thisUnitType = { UnitType = unit.Type }
	local thisUnitClass =  { UnitClassType = unit.Class }

	local freePromotions = table()
	for row in GameInfo.Unit_FreePromotions( thisUnitType ) do
		local promotion = GameInfo.UnitPromotions[ row.PromotionType ]
		if promotion then
			freePromotions:insertLocalized( promotion.Description )
			unitRange = unitRange + (promotion.RangeChange or 0)
			unitMoves = unitMoves + (promotion.MovesChange or 0)
		end
	end

	-- Player data
	local activePlayerID = Game and Game.GetActivePlayer()
	local activePlayer = activePlayerID and Players[ activePlayerID ]
	local city, activeCivilizationType

	-- BE orbital units
	local orbitalInfo = civBE_mode and unit.Orbital and GameInfo.OrbitalUnits[ unit.Orbital ]

	-- BE unit upgrades
	local unitName = unit.Description
	if activePlayer and civBE_mode then
		local bestUpgradeInfo = GameInfo.UnitUpgrades[ activePlayer:GetBestUnitUpgrade(unit.ID) ]
		unitName = bestUpgradeInfo and bestUpgradeInfo.Description or unitName
	end

	if activePlayer then
		productionCost = activePlayer:GetUnitProductionNeeded( unitID )
		activeCivilizationType = (GameInfo.Civilizations[ activePlayer:GetCivilizationType() ] or {}).Type
		city = UI_GetHeadSelectedCity()
		if city and city:GetOwner() ~= activePlayerID then
			city = nil
		end
		city = city or activePlayer:GetCapitalCity() or activePlayer:Cities()(activePlayer)
	end

	-- Name
	local combatClass = unit.CombatClass and GameInfo.UnitCombatInfos[ unit.CombatClass ]
	local tips = table( UnitColor( Locale_ToUpper( unitName ) ) .. (combatClass and " ("..L(combatClass.Description)..")" or "") )
	local item

	if orbitalInfo then
		tips:append( " ("..L"TXT_KEY_ORBITAL_UNITS"..")" )
		-- Orbital Duration
		tips:insertIf( activePlayer and L("TXT_KEY_PRODUCTION_ORBITAL_DURATION", activePlayer:GetTurnsUnitAllowedInOrbit(unit.ID, true) ) ) --todo xml
		-- Orbital Effect Range
		tips:insertLocalized( "TXT_KEY_PRODUCTION_ORBITAL_EFFECT_RANGE", orbitalInfo.EffectRange or 0 )
	else
		-- Movement:
		tips:insertIf( unitDomainType ~= "DOMAIN_AIR" and L"TXT_KEY_PEDIA_MOVEMENT_LABEL" .. " " .. unitMoves .. "[ICON_MOVES]" )
	end

	-- Ranged Combat:
	tips:insertIf( rangedStrength > 0 and L"TXT_KEY_PEDIA_RANGEDCOMBAT_LABEL" .. " " .. rangedStrength .. "[ICON_RANGE_STRENGTH]" .. unitRange )

	-- Combat:
	tips:insertIf( combatStrength > 0 and S( "%s %g[ICON_STRENGTH]", L"TXT_KEY_PEDIA_COMBAT_LABEL", combatStrength ) )

	-- Air Combat:
	tips:insertIf( airstrike > 0 and S( "%s %g[ICON_AIRSTRIKE_DEFENSE]", L"TXT_KEY_AIR_STRENGTH_DEFENSE", airstrike ) )

	-- Abilities:	--TXT_KEY_PEDIA_FREEPROMOTIONS_LABEL
	tips:insertIf( #freePromotions > 0 and "[ICON_BULLET]" .. freePromotions:concat( "[NEWLINE][ICON_BULLET]" ) )

	-- Ability to create building in city (e.g. vanilla great general)
	for row in GameInfo.Unit_Buildings( thisUnitType ) do
		local building = GameInfo.Buildings[ row.BuildingType ]
		tips:insertIf( building and "[ICON_BULLET]"..L"TXT_KEY_MISSION_CONSTRUCT".." " .. BuildingColor( L(building.Description) ) )
	end

	-- Actions	--TXT_KEY_PEDIA_WORKER_ACTION_LABEL
	for row in GameInfo.Unit_Builds( thisUnitType ) do
		local build = GameInfo.Builds[ row.BuildType ]
		if build then
			local requiredCivilizationType = build.ImprovementType and (GameInfo.Improvements[ build.ImprovementType ] or {}).CivilizationType
			if not requiredCivilizationType or not activePlayer or GameInfoTypes[ requiredCivilizationType ] == activePlayer:GetCivilizationType() then -- GameInfoTypes not available pregame: works because activePlayer is also nil
				item = build.PrereqTech and GameInfo.Technologies[ build.PrereqTech ]
				tips:insert( "[ICON_BULLET]" .. (item and TechColor( L(item.Description) ) .. " " or "") .. L(build.Description) )
			end
		end
	end
	-- Great Engineer
	tips:insertIf( (unit.BaseHurry or 0) > 0 and S( "[ICON_BULLET]%s %i[ICON_PRODUCTION]%+i[ICON_PRODUCTION]/[ICON_CITIZEN]", L"TXT_KEY_MISSION_HURRY_PRODUCTION", unit.BaseHurry, unit.HurryMultiplier or 0 ) )

	-- Great Merchant
	tips:insertIf( (unit.BaseGold or 0) > 0 and S( "[ICON_BULLET]%s %i%s%+i[ICON_INFLUENCE]", L"TXT_KEY_MISSION_CONDUCT_TRADE_MISSION", unit.BaseGold + ( unit.NumGoldPerEra or 0 ) * ( Game and Teams[Game.GetActiveTeam()]:GetCurrentEra() or PreGame.GetEra() ), g_currencyIcon, GameDefines.MINOR_FRIENDSHIP_FROM_TRADE_MISSION or 0 ) )
	local tipKey, tip
	-- Other tags
	for k,v in pairs(unit) do
		if v and v ~=0 and v~=-1 then
			tipKey = "TXT_KEY_EUI_UNIT_" .. k:upper()
			tip = L( tipKey, v )
			if tip ~= tipKey then
				tips:insert( "[ICON_BULLET]" .. tip )
			end
		end
	end
	-- Technology_DomainExtraMoves
	for row in GameInfo.Technology_DomainExtraMoves{ DomainType = unitDomainType } do
		item = GameInfo.Technologies[ row.TechType ]
		tips:insertIf( item and (row.Moves or 0)~=0 and S( "[ICON_BULLET]%s %+i[ICON_MOVES]", TechColor( L(item.Description) ), row.Moves ) )
	end
--TODO Technology_TradeRouteDomainExtraRange

	-- Ability to generate tourism upon spawn
	if bnw_mode then
		for row in GameInfo.Policy_TourismOnUnitCreation( thisUnitClass ) do
			item = GameInfo.Policies[ row.PolicyType ]
			tips:insertIf( item and (row.Tourism or 0)~=0 and S( "[ICON_BULLET]%s %+i[ICON_TOURISM]", PolicyColor( L(item.Description) ), row.Tourism ) )
		end
	end

	-- Resources required:
	if Game then
		for resource in GameInfo.Resources() do
			local numResource = Game.GetNumResourceRequiredForUnit( unitID, resource.ID )
			tips:insertIf( resource and numResource ~= 0 and S( "%s: %+i%s", L(resource.Description), -numResource, tostring(resource.IconString) ) )
		end
	else
		for row in GameInfo.Unit_ResourceQuantityRequirements( thisUnitType ) do
			local resource = GameInfo.Resources[ row.ResourceType ]
			tips:insertIf( resource and (row.Cost or 0)~=0 and S( "%s: %+i%s", L(resource.Description), -row.Cost, tostring(resource.IconString) ) )
		end
	end

	-- Resources total required:
	if Game and Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") then
		local isFirstLine = true
		for resource in GameInfo.Resources() do
			local numResource = Game.GetNumResourceTotalRequiredForUnit( unitID, resource.ID )
			if isFirstLine and resource and numResource ~= 0 then
				tips:insertIf( resource and numResource ~= 0 and S( "%s: [NEWLINE][ICON_BULLET] %s: %i%s", L"TXT_KEY_EUI_RESOURCES_QUANTITY_TOTAL_REQUIRED", L(resource.Description), numResource, tostring(resource.IconString) ) )
				isFirstLine = false
			else
				tips:insertIf( resource and numResource ~= 0 and S( "[NEWLINE][ICON_BULLET] %s: %i%s", L(resource.Description), numResource, tostring(resource.IconString) ) )
			end
		end
	else
		local isFirstLine = true
		for row in GameInfo.Unit_ResourceQuantityTotals( thisUnitType ) do
			local resource = GameInfo.Resources[ row.ResourceType ]
			if isFirstLine and resource and (row.Cost or 0) ~= 0 then
				tips:insertIf( resource and (row.Cost or 0)~=0 and S( "%s [NEWLINE][ICON_BULLET] %s: %i%s", L"TXT_KEY_EUI_RESOURCES_QUANTITY_TOTAL_REQUIRED", L(resource.Description), row.Cost, tostring(resource.IconString) ) )
				isFirstLine = false
			else
				tips:insertIf( resource and numResource ~= 0 and S( "[NEWLINE][ICON_BULLET] %s: %i%s", L(resource.Description), numResource, tostring(resource.IconString) ) )
			end
		end
	end

	tips:insert( "----------------" )

	-- Cost:
	local costTip
	if productionCost > 1 then -- Production cost
		if not unit.PurchaseOnly then
			costTip = productionCost .. "[ICON_PRODUCTION]"
		end
		local goldCost = 0
		if city then
			goldCost = city:GetUnitPurchaseCost( unitID )
		elseif (unit.HurryCostModifier or 0) > 0 then
			goldCost = (productionCost * GameDefines.GOLD_PURCHASE_GOLD_PER_PRODUCTION ) ^ GameDefines.HURRY_GOLD_PRODUCTION_EXPONENT
			goldCost = (unit.HurryCostModifier + 100) * goldCost / 100
			goldCost = goldCost - ( goldCost % GameDefines.GOLD_PURCHASE_VISIBLE_DIVISOR )
		end
		if goldCost > 0 then
			if costTip then
				costTip = costTip .. ("(%i%%)"):format(productionCost*100/goldCost)
				if civ5gk_mode then
					costTip = L("TXT_KEY_PEDIA_A_OR_B", costTip, goldCost .. g_currencyIcon )
				else
					costTip = costTip .. " / " .. goldCost .. g_currencyIcon
				end
			else
				costTip = goldCost .. g_currencyIcon
			end
		end
	end -- production cost
	if g_isReligionEnabled then -- Faith cost
		local faithCost = 0
		if city then
			faithCost = city:GetUnitFaithPurchaseCost( unitID, true )
		elseif Game then
			faithCost = Game.GetFaithCost( unitID )
		elseif unit.RequiresFaithPurchaseEnabled and unit.FaithCost then
			faithCost = unit.FaithCost
		end
		if ( faithCost or 0 ) > 0 then
			if costTip then
				costTip = L("TXT_KEY_PEDIA_A_OR_B", costTip, faithCost .. "[ICON_PEACE]" )
			else
				costTip = faithCost .. "[ICON_PEACE]"
			end
		end
	end --faith cost
	tips:insertIf( costTip and L"TXT_KEY_PEDIA_COST_LABEL" .. " " .. ( costTip or L"TXT_KEY_FREE" ) )

	-- Settler Specifics
	tips:insertLocalizedIf( unit.Food and "TXT_KEY_CITYVIEW_STAGNATION_TEXT" ) -- build using food / stop city growth
	if unit.Found or unit.FoundAbroad then
		tips:append( L("TXT_KEY_NO_ACTION_SETTLER_SIZE_LIMIT", GameDefines.CITY_MIN_SIZE_FOR_SETTLERS) )
	end

	-- Civilization:
	local civs = table()
	for requiredCivilizationType in GameInfo.Civilization_UnitClassOverrides( thisUnitType ) do
		item = GameInfo.Civilizations[ requiredCivilizationType.CivilizationType ]
		civs:insertIf( item and L( item.ShortDescription ) )
	end
	tips:insertIf( #civs > 0 and L"TXT_KEY_PEDIA_CIVILIZATIONS_LABEL".." "..civs:concat(", ") )

	-- Replaces:
	item = GameInfo.Units[ (GameInfo.UnitClasses[ unit.Class ] or {}).DefaultUnit ]
	tips:insertIf( item and item ~= unit and L"TXT_KEY_PEDIA_REPLACES_LABEL".." "..UnitColor( L(item.Description) ) ) --!!! row

	if civBE_mode then
		-- Affinity Level Requirements
		for affinityPrereq in GameInfo.Unit_AffinityPrereqs( thisUnitType ) do
			local affinityInfo = (tonumber( affinityPrereq.Level) or 0 ) > 0 and GameInfo.Affinity_Types[ affinityPrereq.AffinityType ]
			tips:insertIf( affinityInfo and L( "TXT_KEY_AFFINITY_LEVEL_REQUIRED", affinityInfo.ColorType, affinityPrereq.Level, tostring(affinityInfo.IconString), tostring(affinityInfo.Description) ) )
		end
	end

	-- Required Policies:
	item = unit.PolicyType and GameInfo.Policies[ unit.PolicyType ]
	tips:insertIf( item and L"TXT_KEY_PEDIA_PREREQ_POLICY_LABEL" .. " " .. PolicyColor( L(item.Description) ) )

	-- Required Buildings:
	local buildings = table()
	for row in GameInfo.Unit_BuildingClassRequireds( thisUnitType ) do
		item = GetCivBuilding( activeCivilizationType, row.BuildingClassType )
		buildings:insertIf( item and BuildingColor( L(item.Description) ) )
	end
	item = unit.ProjectPrereq and GameInfo.Projects[ unit.ProjectPrereq ]
	buildings:insertIf( item and BuildingColor( L(item.Description) ) )
	tips:insertIf( #buildings > 0 and L"TXT_KEY_PEDIA_REQ_BLDG_LABEL" .. " " .. buildings:concat(", ") ) -- TXT_KEY_NO_ACTION_UNIT_REQUIRES_BUILDING

	-- Prerequisite Techs:
	item = unit.PrereqTech and GameInfo.Technologies[ unit.PrereqTech ]
	tips:insertIf( item and item.ID > 0 and L"TXT_KEY_PEDIA_PREREQ_TECH_LABEL" .. " " .. TechColor( L(item.Description) ) )

	-- Becomes Obsolete with:
	local obsoleteTech = unit.ObsoleteTech and GameInfo.Technologies[ unit.ObsoleteTech ]
	tips:insertIf( obsoleteTech and L"TXT_KEY_PEDIA_OBSOLETE_TECH_LABEL" .. " " .. TechColor( L(obsoleteTech.Description) ) )

	-- Pre-written Help text
	return AddPreWrittenHelpTextAndConcat( tips, unit )
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

local function GetHelpTextForBuilding( buildingID, bExcludeName, bExcludeHeader, bNoMaintenance, city )
	local building = GameInfo.Buildings[ buildingID ]
	if not building then
		return "<Building undefined in game database>"
	end
	local buildingType = building.Type or -1
	local buildingClassType = building.BuildingClass
	local buildingClass = GameInfo.BuildingClasses[ buildingClassType ]
	local buildingClassID = buildingClass and buildingClass.ID
	local activePlayerID = Game and Game.GetActivePlayer()
	local activePlayer = activePlayerID and Players[ activePlayerID ]
	local maxGlobalInstances = buildingClass and tonumber(buildingClass.MaxGlobalInstances) or 0
	local thisBuildingType = { BuildingType = buildingType }
	local thisBuildingAndResourceTypes =  { BuildingType = buildingType }
	local thisBuildingClassType = { BuildingClassType = buildingClassType }
	local activeTeamID = Game and Game.GetActiveTeam()
	local activeTeam = activeTeamID and Teams[ activeTeamID ]
	local activeTeamTechs = activeTeam and activeTeam:GetTeamTechs()
	local activePlayerIdeologyID = bnw_mode and activePlayer and activePlayer:GetLateGamePolicyTree()
	local activePlayerIdeology = activePlayerIdeologyID and GameInfo.PolicyBranchTypes[ activePlayerIdeologyID ]
	local activePlayerIdeologyType = activePlayerIdeology and activePlayerIdeology.Type
	local activePlayerBeliefs = {}
	local availableBeliefs = {}
	local activePerkTypes = {}
	local activeCivilizationType

	if g_isReligionEnabled and activePlayer then
		local religionID = activePlayer:GetReligionCreatedByPlayer()
		if religionID > 0 then
			activePlayerBeliefs = Game.GetBeliefsInReligion( religionID )
		elseif activePlayer:HasCreatedPantheon() then
			activePlayerBeliefs = { activePlayer:GetBeliefInPantheon() }
		end

		for i = 1, activePlayer:IsTraitBonusReligiousBelief() and 6 or 5 do
			if (activePlayerBeliefs[i] or -1) < 0 then -- active player does not already have a belief in "i" belief class
				for _,beliefID in pairs( g_gameAvailableBeliefs[i]() ) do
					availableBeliefs[beliefID] = true -- because available to active player in "i" belief class
				end
			end
		end
	end
	------------------
	-- Tech Filter
	local function techFilter( row )
		return row and g_isScienceEnabled and ( not activeTeamTechs or not activeTeamTechs:HasTech( row.ID ) )
	end

	------------------
	-- Policy Filter
	local function policyFilter( row )
		return row and g_isPoliciesEnabled
			and not( activePlayer and activePlayer:HasPolicy( row.ID ) and not activePlayer:IsPolicyBlocked( row.ID ) )
			and not( activePlayerIdeologyType and activePlayerIdeologyType ~= row.PolicyBranchType )
	end

	------------------
	-- Belief Filter
	local function beliefFilter( row )
		return row and g_isReligionEnabled and ( not activePlayer or availableBeliefs[ row.ID ] )
	end


	local productionCost = tonumber(building.Cost) or 0
	local maintenanceCost = tonumber(building[g_maintenanceCurrency]) or 0
	local happinessChange = (tonumber(building.Happiness) or 0) + (tonumber(building.UnmoddedHappiness) or 0)
	local defenseChange = tonumber(building.Defense) or 0
	local hitPointChange = tonumber(building.ExtraCityHitPoints) or 0
	local cultureChange = not gk_mode and tonumber(building.Culture) or 0
	local cultureModifier = tonumber(building.CultureRateModifier) or 0

-- Vox Populi Supply Cap
	local citySupplyModifier = tonumber(building.CitySupplyModifier) or 0
	local citySupplyModifierGlobal = tonumber(building.CitySupplyModifierGlobal) or 0
	local citySupplyFlat = tonumber(building.CitySupplyFlat) or 0
	local citySupplyFlatGlobal = tonumber(building.CitySupplyFlatGlobal) or 0
-- Vox Populi END
	
	local enhancedYieldTech = building.EnhancedYieldTech and GameInfo.Technologies[ building.EnhancedYieldTech ]
	local enhancedYieldTechName = enhancedYieldTech and TechColor( L(enhancedYieldTech.Description) ) or ""

	if activePlayer then
		activeCivilizationType = (GameInfo.Civilizations[ activePlayer:GetCivilizationType() ] or {}).Type
		city = city or UI_GetHeadSelectedCity()
		city = (city and city:GetOwner() == activePlayerID and city) or activePlayer:GetCapitalCity() or activePlayer:Cities()(activePlayer)

		-- player production cost
		productionCost = activePlayer:GetBuildingProductionNeeded( buildingID )
-- CBP
		if (city ~= nil) then
			if(city:GetBuildingInvestment(buildingID) > 0) then
				productionCost = city:GetBuildingInvestment(buildingID);
			end
		end
-- END
		if civ5_mode then
			-- player extra happiness
			happinessChange = happinessChange + activePlayer:GetExtraBuildingHappinessFromPolicies( buildingID )
			if gk_mode then
				happinessChange = happinessChange + activePlayer:GetPlayerBuildingClassHappiness( buildingClassID )
			end
		else
			-- get the active perk types
			activePerkTypes = activePlayer:GetAllActivePlayerPerkTypes()
		end
	end

	if city and civ5gk_mode and buildingClassID then
		happinessChange = happinessChange + city:GetReligionBuildingClassHappiness(buildingClassID)
	end

	local tip, tipKey, items, item, tipAlt
	-- Name
	local tips = table( BuildingColor( Locale_ToUpper( building.Description ) ) )
-- CBP
	if (city ~= nil) then
		if(city:GetBuildingInvestment(buildingID) > 0) then
			if tips then
				tips:insert( L"TXT_KEY_INVESTED")
			end
		end
	end
-- END

	-- Other tags
	for k,v in pairs(building) do
		if v and v ~=0 and v~=-1 then
			tipKey = "TXT_KEY_EUI_BUILDING_" .. k:upper()
			tip = L( tipKey, v )
			if tip ~= tipKey then
				tips:insert( tip )
			end
		end
	end

--local function GetBuildingYields( buildingID, buildingType, buildingClassID, activePlayer )
	-- Yields
	local thisBuildingAndYieldTypes = { BuildingType = buildingType }
	tip = ""
	for yield in GameInfo.Yields() do
		local yieldID = yield.ID
		local yieldChange = 0
		local yieldModifier = 0
		thisBuildingAndYieldTypes.YieldType = yield.Type

		if Game and buildingClassID and yieldID < YieldTypes.NUM_YIELD_TYPES then -- weed out strange Communitas yields
			yieldChange = Game.GetBuildingYieldChange( buildingID, yieldID )
			yieldModifier = Game.GetBuildingYieldModifier( buildingID, yieldID )
			if activePlayer then
				if gk_mode then
					yieldChange = yieldChange + activePlayer:GetPlayerBuildingClassYieldChange( buildingClassID, yieldID )
								+ activePlayer:GetPolicyBuildingClassYieldChange( buildingClassID, yieldID )
-- Start Vox Populi
					-- Player yield bonuses to World Wonders
					if Game.IsWorldWonderClass(buildingClassID) then
						yieldChange = yieldChange + activePlayer:GetExtraYieldWorldWonder(buildingID, yieldID)
					end
-- End Vox Populi
				end
				yieldModifier = yieldModifier + activePlayer:GetPolicyBuildingClassYieldModifier( buildingClassID, yieldID )
				for i=1, #activePerkTypes do
					yieldChange = yieldChange + Game.GetPlayerPerkBuildingClassFlatYieldChange( activePerkTypes[i], buildingClassID, yieldID )
					yieldModifier = yieldModifier + Game.GetPlayerPerkBuildingClassPercentYieldChange( activePerkTypes[i], buildingClassID, yieldID )
				end
			end
			if city and gk_mode then
				yieldChange = yieldChange + city:GetReligionBuildingClassYieldChange( buildingClassID, yieldID )
				if bnw_mode then
					yieldChange = yieldChange + city:GetLeagueBuildingClassYieldChange( buildingClassID, yieldID )
				end
-- CBP
				if yield.Type == "YIELD_CULTURE" then
					yieldChange = yieldChange + city:GetBuildingClassCultureChange(buildingClassID )
				end
				
				yieldChange = yieldChange + city:GetLocalBuildingClassYield(buildingClassID, yieldID)
					
				yieldChange = yieldChange + city:GetReligionBuildingYieldRateModifier(buildingClassID, yieldID)
				
				if (city) then
					local corporatechange = city:GetBuildingYieldChangeFromCorporationFranchises(buildingClassID, yieldID)
					if(corporatechange > 0) then
						yieldChange = yieldChange + corporatechange
					end
				end
				-- Events
				yieldChange = yieldChange + city:GetEventBuildingClassYield(buildingClassID, yieldID);
				-- End 
-- END CBP
			end
		else -- not Game
			for row in GameInfo.Building_YieldChanges( thisBuildingAndYieldTypes ) do
				yieldChange = yieldChange + (row.Yield or 0)
			end
			for row in GameInfo.Building_YieldModifiers( thisBuildingAndYieldTypes ) do
				yieldModifier = yieldModifier + (row.Yield or 0)
			end
		end
		if yield.Type == "YIELD_CULTURE" then -- works pregame, when GameInfoTypes is not available
			yieldChange = yieldChange + cultureChange
			yieldModifier = yieldModifier + cultureModifier
			cultureChange = 0
			cultureModifier = 0
		end
		local yieldPerPop = 0
		for row in GameInfo.Building_YieldChangesPerPop( thisBuildingAndYieldTypes ) do
			yieldPerPop = yieldPerPop + (row.Yield or 0)
		end
		local yieldPerPopInEmpire = 0
		for row in GameInfo.Building_YieldChangesPerPopInEmpire( thisBuildingAndYieldTypes ) do
			yieldPerPopInEmpire = yieldPerPopInEmpire + (row.Yield or 0)
		end
		if yieldChange ~=0 then
			tip = S("%s %+i%s", tip, yieldChange, tostring(yield.IconString) )
		end
		if yieldModifier ~=0 then
			tip = S("%s %+i%%%s", tip, yieldModifier, tostring(yield.IconString) )
		end
		if yieldPerPop ~= 0 then
			tip = S("%s %+i%%[ICON_CITIZEN]%s", tip, yieldPerPop, tostring(yield.IconString) )
		end
		if yieldPerPopInEmpire ~= 0 then
			tip = S("%s %+i%%[ICON_CITY_STATE][ICON_CITIZEN]%s", tip, yieldPerPopInEmpire, tostring(yield.IconString) )-- .. " " .. L("TXT_KEY_EUI_EMPIRE") .. " " .. "[ICON_CITIZEN]" .. tostring(yield.IconString)
		end
		if tips and tip~="" then
			tips:insert( tostring(YieldNames[ yieldID ]) .. ":" .. tip )
			tip = ""
		end
	end
	-- Culture leftovers
	if cultureChange ~=0 then
		tip = S("%s %+i[ICON_CULTURE]", tip, cultureChange )
	end
	if cultureModifier ~=0 then
		tip = S("%s %+i%%[ICON_CULTURE]", tip, cultureModifier )
	end
	if tips and tip~="" then
		tips:insert( L"TXT_KEY_PEDIA_CULTURE_LABEL" .. tip )
		tip = ""
	end
	if civ5_mode then
		-- Happiness:
		if happinessChange ~=0 then
			tip = S( "%s %+i[ICON_HAPPINESS_1]", tip, happinessChange )
		end
		if bnw_mode and (building.HappinessPerXPolicies or 0)~=0 then
			tip = S( "%s +1[ICON_HAPPINESS_1]/%i %s", tip, building.HappinessPerXPolicies, PolicyColor( L"TXT_KEY_VP_POLICIES" ) )
		end
		if tips and tip~="" then
			tips:insert( L"TXT_KEY_PEDIA_HAPPINESS_LABEL" .. tip )
			tip = ""
		end
	else
		-- Health
		local healthChange = (tonumber(building.Health) or 0) + (tonumber(building.UnmoddedHealth) or 0)
		local healthModifier = tonumber(building.HealthModifier) or 0
		if activePlayer then
			-- Health from Virtues
			healthChange = healthChange + activePlayer:GetExtraBuildingHealthFromPolicies( buildingID )
			-- Player Perks
			for i=1, #activePerkTypes do
				healthChange = healthChange + Game.GetPlayerPerkBuildingClassPercentHealthChange( activePerkTypes[i], buildingClassID )
				healthModifier = healthModifier + Game.GetPlayerPerkBuildingClassPercentHealthChange( activePerkTypes[i], buildingClassID )
				defenseChange = defenseChange + Game.GetPlayerPerkBuildingClassCityStrengthChange( activePerkTypes[i], buildingClassID )
				hitPointChange = hitPointChange + Game.GetPlayerPerkBuildingClassCityHPChange( activePerkTypes[i], buildingClassID )
				maintenanceCost = maintenanceCost + Game.GetPlayerPerkBuildingClassEnergyMaintenanceChange( activePerkTypes[i], buildingClassID )
			end
		end
		if healthChange ~=0 then
			tip = S("%s %+i[ICON_HEALTH_1]", tip, healthChange )
		end
		if healthModifier ~=0 then
			tip = S("%s %+i%%[ICON_HEALTH_1]", tip, healthModifier )
		end
		if tips and tip~="" then
			tips:insert( L"TXT_KEY_HEALTH" .. ":" .. tip )
			tip = ""
		end
	end
	--CP EVENTS
	if city then
		for pYieldInfo in GameInfo.Yields() do
			local iYieldID = pYieldInfo.ID;
			local iYieldAmount = city:GetEventBuildingClassModifier(buildingClassID, iYieldID);
							
			if (iYieldAmount > 0) then
				tips:insert( L("TXT_KEY_BUILDING_EVENT_MODIFIER", iYieldAmount, pYieldInfo.IconString, pYieldInfo.Description))
			end
		end
	end
	--END
	-- Defense:
	if defenseChange ~=0 then
		tip = S("%s %+g[ICON_STRENGTH]", tip, defenseChange / 100 )
	end
	if hitPointChange ~=0 then
		tip = tip .. " " .. L( "TXT_KEY_PEDIA_DEFENSE_HITPOINTS", hitPointChange )
	end
	if tips and tip~="" then
		tips:insert( L"TXT_KEY_PEDIA_DEFENSE_LABEL" .. tip )
		tip = ""
	end

	-- Vox Populi Supply Cap
	if citySupplyFlat ~=0 then
		tip = S("%s %+d[ICON_SILVER_FIST]", tip, citySupplyFlat )
	end
	if citySupplyModifier ~=0 then
		tip = S("%s %+d%%[ICON_SILVER_FIST]", tip, citySupplyModifier )
	end
	if tips and tip~="" then
		tips:insert( L"TXT_KEY_EUI_BUILDING_SUPPLY_CAP_LOCAL" .. tip )
		tip = ""
	end
	if citySupplyFlatGlobal ~=0 then
		tip = S("%s %+d[ICON_SILVER_FIST][ICON_SILVER_FIST]", tip, citySupplyFlatGlobal )
	end
	if citySupplyModifierGlobal ~=0 then
		tip = S("%s %+d%%[ICON_SILVER_FIST][ICON_SILVER_FIST]", tip, citySupplyModifierGlobal )
	end
	if tips and tip~="" then
		tips:insert( L"TXT_KEY_EUI_BUILDING_SUPPLY_CAP_GLOBAL" .. tip )
		tip = ""
	end
	-- Vox Populi Supply Cap
	-- Maintenance:
	tips:insertIf( maintenanceCost ~= 0 and	S( "%s %+i%s", L"TXT_KEY_PEDIA_MAINT_LABEL", -maintenanceCost, g_currencyIcon) )

	-- Resources required:
	if Game then
		for resource in GameInfo.Resources() do
			local numResource = Game.GetNumResourceRequiredForBuilding( buildingID, resource.ID )
			tips:insertIf( numResource ~= 0 and S( "%s: %+i%s", L(resource.Description), -numResource, tostring(resource.IconString) ) )
		end
	else
		for row in GameInfo.Building_ResourceQuantityRequirements( thisBuildingType ) do
			local resource = GameInfo.Resources[ row.ResourceType ]
			tips:insertIf( resource and (row.Cost or 0)~=0 and S( "%s: %+i%s", L(resource.Description), -row.Cost, tostring(resource.IconString) ) )
		end
	end

-- CBP -- Global Average Modifiers
	if(tips and Game and buildingID) then
		if(city and building.GPRateModifierPerXFranchises ~= 0) then
			local iCorpGPChange = city:GetGPRateModifierPerXFranchises();
			if iCorpGPChange ~=0 then
				tips:insert( L( "TXT_KEY_PEDIA_CORP_GP_CHANGE", iCorpGPChange))
			end
		end
		
		local iGetPovertyHappinessChangeBuilding = Game.GetPovertyHappinessChangeBuilding( buildingID)
		if (iGetPovertyHappinessChangeBuilding ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_POVERTY_AVERAGE_MODIFIER", iGetPovertyHappinessChangeBuilding))
		end
		local iGetDefenseHappinessChangeBuilding = Game.GetDefenseHappinessChangeBuilding( buildingID)
		if (iGetDefenseHappinessChangeBuilding ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_DEFENSE_AVERAGE_MODIFIER", iGetDefenseHappinessChangeBuilding))
		end
		local iGetUnculturedHappinessChangeBuilding = Game.GetUnculturedHappinessChangeBuilding( buildingID) 
		if (iGetUnculturedHappinessChangeBuilding ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_CULTURE_AVERAGE_MODIFIER", iGetUnculturedHappinessChangeBuilding))
		end
		local iGetIlliteracyHappinessChangeBuilding = Game.GetIlliteracyHappinessChangeBuilding( buildingID)
		if (iGetIlliteracyHappinessChangeBuilding ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_SCIENCE_AVERAGE_MODIFIER", iGetIlliteracyHappinessChangeBuilding))
		end
		local iGetMinorityHappinessChangeBuilding = Game.GetMinorityHappinessChangeBuilding( buildingID)
		if (iGetMinorityHappinessChangeBuilding ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_MINORITY_AVERAGE_MODIFIER", iGetMinorityHappinessChangeBuilding))
		end
		local iGetPovertyHappinessChangeBuildingGlobal = Game.GetPovertyHappinessChangeBuildingGlobal( buildingID)
		if (iGetPovertyHappinessChangeBuildingGlobal ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_POVERTY_GLOBAL_AVERAGE_MODIFIER", iGetPovertyHappinessChangeBuildingGlobal))
		end
		local iGetDefenseHappinessChangeBuildingGlobal = Game.GetDefenseHappinessChangeBuildingGlobal( buildingID)
		if (iGetDefenseHappinessChangeBuildingGlobal ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_DEFENSE_GLOBAL_AVERAGE_MODIFIER", iGetDefenseHappinessChangeBuildingGlobal))
		end
		local iGetUnculturedHappinessChangeBuildingGlobal = Game.GetUnculturedHappinessChangeBuildingGlobal( buildingID)
		if (iGetUnculturedHappinessChangeBuildingGlobal ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_CULTURE_GLOBAL_AVERAGE_MODIFIER", iGetUnculturedHappinessChangeBuildingGlobal))
		end
		local iGetIlliteracyHappinessChangeBuildingGlobal = Game.GetIlliteracyHappinessChangeBuildingGlobal( buildingID)
		if (iGetIlliteracyHappinessChangeBuildingGlobal ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_SCIENCE_GLOBAL_AVERAGE_MODIFIER", iGetIlliteracyHappinessChangeBuildingGlobal))
		end
		local iGetMinorityHappinessChangeBuildingGlobal = Game.GetMinorityHappinessChangeBuildingGlobal( buildingID)
		if (iGetMinorityHappinessChangeBuildingGlobal ~= 0) then
			tips:insert( L( "TXT_KEY_BUILDING_MINORITY_GLOBAL_AVERAGE_MODIFIER", iGetMinorityHappinessChangeBuildingGlobal))
		end
		if (not OptionsManager.IsNoBasicHelp()) then	
			if(city) then
				local iPovertyTotal = iGetPovertyHappinessChangeBuilding + iGetPovertyHappinessChangeBuildingGlobal;
				if(iPovertyTotal ~= 0) then
					iNewThreshold = city:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
					local iOldThreshold = city:GetUnhappinessFromGoldNeeded() / 100;
					if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
						tips:insert( L( "TXT_KEY_BUILDING_POVERTY_NEW_THRESHOLD", iNewThreshold, iOldThreshold))
					end
				end		
				local iDefenseTotal = iGetDefenseHappinessChangeBuilding + iGetDefenseHappinessChangeBuildingGlobal;
				if(iDefenseTotal ~= 0) then
					iNewThreshold = city:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
					local iOldThreshold = city:GetUnhappinessFromDefenseNeeded() / 100;
					if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
						tips:insert( L( "TXT_KEY_BUILDING_DEFENSE_NEW_THRESHOLD", iNewThreshold, iOldThreshold))
					end
				end
				local iIlliteracyTotal = iGetIlliteracyHappinessChangeBuilding + iGetIlliteracyHappinessChangeBuildingGlobal;
				if(iIlliteracyTotal ~= 0) then
					iNewThreshold = city:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
					local iOldThreshold = city:GetUnhappinessFromScienceNeeded() / 100;
					if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
						tips:insert( L( "TXT_KEY_BUILDING_ILLITERACY_NEW_THRESHOLD", iNewThreshold, iOldThreshold))
					end
				end
				local iCultureTotal = iGetUnculturedHappinessChangeBuilding + iGetUnculturedHappinessChangeBuildingGlobal;
				if(iCultureTotal ~= 0) then
					iNewThreshold = city:GetTheoreticalUnhappinessDecrease(buildingID) / 100;
					local iOldThreshold = city:GetUnhappinessFromCultureNeeded() / 100;
					if(iNewThreshold ~= 0 and iOldThreshold ~= 0)then
						tips:insert( L( "TXT_KEY_BUILDING_CULTURE_NEW_THRESHOLD", iNewThreshold, iOldThreshold))
					end
				end		
			end
		end
	end
-- END

	-- Specialists
	local specialistType = building.SpecialistType
	local specialist = specialistType and GameInfo.Specialists[ specialistType ]
	if specialist and city then
		tips:insertIf( ( building.GreatPeopleRateChange or 0 ) ~= 0 and S("%s %+i%s", L( specialist.GreatPeopleTitle ), building.GreatPeopleRateChange, GreatPeopleIcon( specialistType ) ) )
		if (building.SpecialistCount or 0) ~= 0 then
			if civ5_mode then
				local numSpecialistsInBuilding = UI_GetHeadSelectedCity() and city:GetNumSpecialistsInBuilding( buildingID ) or building.SpecialistCount
				if numSpecialistsInBuilding ~= building.SpecialistCount then
					numSpecialistsInBuilding = numSpecialistsInBuilding.."/"..building.SpecialistCount
				end
				tips:insert( L( "TXT_KEY_CITYVIEW_BUILDING_SPECIALIST_YIELD", numSpecialistsInBuilding, tostring(specialist.Description), GetSpecialistYields( city, specialist ) ) )
			else
				tips:insert( S( "%i[ICON_CITIZEN]%s /%s", building.SpecialistCount, L( specialist.Description ), GetSpecialistYields( city, specialist ) ) )
			end
		end
	end

	if civ5bnw_mode then
		-- Great Work Slots
		local greatWorkType = (building.GreatWorkCount or 0) > 0 and building.GreatWorkSlotType and GameInfo.GreatWorkSlots[building.GreatWorkSlotType]
		tips:insertIf( greatWorkType and L( greatWorkType.SlotsToolTipText, building.GreatWorkCount ) )
		for row in GameInfo.Building_DomainFreeExperiencePerGreatWork( thisBuildingType ) do
			item = GameInfo.Domains[ row.DomainType ]
			tips:insertIf( item and (row.Experience or 0)>0 and L(item.Description).." "..L( "TXT_KEY_EXPERIENCE_POPUP", row.Experience ).."/ "..L("TXT_KEY_VP_GREAT_WORKS") )
		end
		-- Theming Bonus
-- TODO Building_ThemingBonuses
--		tips:insertLocalizedIf( building.ThemingBonusHelp )
		-- Free Great Work
		local freeGreatWork = building.FreeGreatWork and GameInfo.GreatWorks[ building.FreeGreatWork ]
-- TODO type rather than blurb
		tips:insertIf( freeGreatWork and L"TXT_KEY_FREE" .." "..L( freeGreatWork.Description ) )

-- TODO sacred sites properly
-- TODO Building_YieldChangesPerReligion
		tip = ""
		if (building.FaithCost or 0) > 0 and building.UnlockedByBelief and building.Cost == -1 then
			local tourism = city and city:GetFaithBuildingTourism() or 0
			if tourism ~= 0 then
				tip = S(" %+i[ICON_TOURISM]", tourism )
			end
		end
-- CBP FIX
		if (building.FaithCost or 0) > 0 and building.PolicyType ~= nil and building.Cost == -1 then
			if city then
				 tourism = city:GetFaithBuildingTourism()
			else
			end
			if tourism ~= 0 then
				tip = S(" %+i[ICON_TOURISM]", city:GetFaithBuildingTourism() )
			end
		end
		if city then
			local faithtourism = city:GetBuildingClassTourism(buildingClassID)
			if faithtourism ~= 0 then
				tip = S(" %+i[ICON_TOURISM]", faithtourism )
			end
		end
-- END
		if enhancedYieldTechName and (building.TechEnhancedTourism or 0) ~= 0 then
			tip = S("%s %s %+i[ICON_TOURISM]", tip, enhancedYieldTechName, building.TechEnhancedTourism )
		end
	-- CBP
		if city then
			local iTourism = activePlayer:GetExtraYieldWorldWonder(buildingID, YieldTypes.YIELD_TOURISM);
			if(iTourism ~= 0) then
				tip = S("%s %+i[ICON_TOURISM]", tip, iTourism)
			end
		end
	-- END
		tips:insertIf( #tip > 0 and L"TXT_KEY_CITYVIEW_TOURISM_TEXT" .. ":" .. tip )
	end
-- TODO GetInternationalTradeRouteYourBuildingBonus
	if gk_mode then
		-- Resources
		for row in GameInfo.Building_ResourceQuantity( thisBuildingType ) do
			local resource = GameInfo.Resources[ row.ResourceType ]
			tips:insertIf( resource and (row.Quantity or 0)~=0 and S("%s: %+i%s", L(resource.Description), row.Quantity, tostring(resource.IconString) ) )
		end
-- CBP
		for row in GameInfo.Building_ResourceQuantityPerXFranchises( thisBuildingType ) do
			local resource = GameInfo.Resources[ row.ResourceType ]
			local amount = city:GetResourceQuantityPerXFranchises(resource)
			tips:insertIf( resource and (amount)~=0 and S("%s: %+i%s", L(resource.Description), tonumber(amount), tostring(resource.IconString) ) )
		end
-- END
	end

	-- Resource Yields enhanced by Building
	for resource in GameInfo.Resources() do
		thisBuildingAndResourceTypes.ResourceType = resource.Type or -1
		tip = GetYieldString( GameInfo.Building_ResourceYieldChanges( thisBuildingAndResourceTypes ) )
		tipAlt = GetYieldString( GameInfo.Building_ResourceYieldChangesGlobal( thisBuildingAndResourceTypes ) )
		for row in GameInfo.Building_ResourceCultureChanges( thisBuildingAndResourceTypes ) do
			if (row.CultureChange or 0)~= 0 then
				tip = tip .. S(" %+i[ICON_CULTURE]", row.CultureChange )
			end
		end
		if g_isReligionEnabled then
			for row in GameInfo.Building_ResourceFaithChanges( thisBuildingAndResourceTypes ) do
				if (row.FaithChange or 0)~= 0 then
					tip = S("%s %+i[ICON_FAITH]", tip, row.FaithChange )
				end
			end
		end
-- TODO GameInfo.Building_ResourceYieldModifiers( thisBuildingType ), ResourceType, YieldType, Yield
		tips:insertIf( #tip > 0 and tostring(resource.IconString) .. " " .. L(resource.Description) .. ":" .. tip )
		tips:insertIf( #tipAlt > 0 and L("TXT_KEY_EUI_GLOBAL") .. " " .. tostring(resource.IconString) .. " " .. L(resource.Description) .. ":" .. tipAlt )
	end

	-- Feature Yields enhanced by Building
	for feature in GameInfo.Features() do
		tip = GetYieldString( GameInfo.Building_FeatureYieldChanges{ BuildingType = buildingType, FeatureType = feature.Type } )
		tips:insertIf( #tip > 0 and L(feature.Description) .. ":" .. tip )
	end
	if gk_mode then
		-- Terrain Yields enhanced by Building
		for terrain in GameInfo.Terrains() do
			tip = GetYieldString( GameInfo.Building_TerrainYieldChanges{ BuildingType = buildingType, TerrainType = terrain.Type } )
			if civBE_mode then
				local health = 0
				for row in GameInfo.Building_TerrainHealthChange{ BuildingType = buildingType, TerrainType = terrain.Type } do
					health = health + ( tonumber( row.Quantity ) or 0 )
				end
				if health ~= 0 then
					tip = S( "%s %+i[ICON_HEALTH_1]", tip, health )
				end
			end
			tips:insertIf( #tip > 0 and L(terrain.Description) .. ":" .. tip )
		end
	end
	-- Specialist Yields enhanced by Building
	for specialist in GameInfo.Specialists() do
		tip = GetYieldString( GameInfo.Building_SpecialistYieldChanges{ BuildingType = buildingType, SpecialistType = specialist.Type } )
		tips:insertIf( #tip > 0 and L(specialist.Description) .. ":" .. tip )
	end

	-- River Yields enhanced by Building
	tip = GetYieldString( GameInfo.Building_RiverPlotYieldChanges( thisBuildingType ) )
	tips:insertIf( #tip > 0 and L"TXT_KEY_PLOTROLL_RIVER" .. ":" .. tip )

	-- Lake Yields enhanced by Building
	tip = GetYieldString( GameInfo.Building_LakePlotYieldChanges( thisBuildingType ) )
	tips:insertIf( #tip > 0 and L"TXT_KEY_PLOTROLL_LAKE" .. ":" .. tip )

	-- Ocean Yields enhanced by Building
	tip = GetYieldString( GameInfo.Building_SeaPlotYieldChanges( thisBuildingType ) )
	tips:insertIf( #tip > 0 and L"TXT_KEY_PLOTROLL_OCEAN" .. ":" .. tip )

	-- Ocean Resource Yields enhanced by Building
	tip = GetYieldString( GameInfo.Building_SeaResourceYieldChanges( thisBuildingType ) )
-- todo determine sea resources
	tips:insertIf( #tip > 0 and L"TXT_KEY_PLOTROLL_OCEAN" .." "..L"TXT_KEY_TOPIC_RESOURCES".. ":" .. tip )

	-- Area Yields enhanced by Building
	tip = GetYieldStringSpecial( "Yield", "%s %+i%%%s", GameInfo.Building_AreaYieldModifiers( thisBuildingType ) )
	tips:insertIf( #tip > 0 and L"TXT_KEY_PGSCREEN_CONTINENTS" .. ":" .. tip )
	-- Map Yields enhanced by Building
	tip = GetYieldStringSpecial( "Yield", "%s %+i%%%s", GameInfo.Building_GlobalYieldModifiers( thisBuildingType ) )
	tips:insertIf( #tip > 0 and L"TXT_KEY_TOPIC_CITIES" .. ":" .. tip )

	-- victory requisite
	local victory = building.VictoryPrereq and GameInfo.Victories[ building.VictoryPrereq ]
	tips:insertIf( victory and TextColor( "[COLOR_GREEN]", L( victory.Description ) ) )

	-- free building in this city
	local freeBuilding = GetCivBuilding( activeCivilizationType, building.FreeBuildingThisCity )
	tips:insertIf( freeBuilding and L"TXT_KEY_FREE".." "..BuildingColor( L( freeBuilding.Description ) ) )

	-- free building in all cities
	freeBuilding = GetCivBuilding( activeCivilizationType, building.FreeBuilding )
	tips:insertIf( freeBuilding and L"TXT_KEY_FREE".." "..BuildingColor( L( freeBuilding.Description ) ) )-- todo xml

	-- Vox Populi granted by building
	if g_grantedByBuilding[buildingClassType] then
		for _,grantedBy in ipairs(g_grantedByBuilding[buildingClassType]) do
			tips:insertIf( grantedBy and L"TXT_KEY_EUI_BUILDING_GRANTED_BY".." "..BuildingColor( L( GameInfo.Buildings[grantedBy].Description ) ) )
		end
	end
	-- Vox Populi end
	
	-- free units
	for row in GameInfo.Building_FreeUnits( thisBuildingType ) do
		local freeUnit = GameInfo.Units[ row.UnitType ]
		freeUnit = freeUnit and GetCivUnit( activeCivilizationType, freeUnit.Class )
		tips:insertIf( freeUnit and (row.NumUnits or 0)>0 and ( row.NumUnits > 1 and row.NumUnits.." " or "" ) ..L"TXT_KEY_FREE".." "..UnitColor( L( freeUnit.Description ) ) )
	end
--Building_FreeSpecialistCounts unused ?
	-- free promotion to units trained in this city
	local freePromotion = building.TrainedFreePromotion and GameInfo.UnitPromotions[ building.TrainedFreePromotion ]
	tips:insertIf( freePromotion and L"TXT_KEY_PEDIA_FREEPROMOTIONS_LABEL".." "..L( freePromotion.Help ) )

	-- free promotion for all units
	freePromotion = building.FreePromotion and GameInfo.UnitPromotions[ building.FreePromotion ]
	tips:insertIf( freePromotion and L"TXT_KEY_PEDIA_FREEPROMOTIONS_LABEL".." "..L( freePromotion.Help ) )

	-- hurry modifiers
	for row in GameInfo.Building_HurryModifiers( thisBuildingType ) do
		item = GameInfo.HurryInfos[ row.HurryType ]
		tips:insertIf( item and (row.HurryCostModifier or 0)~=0 and S( "%s %+i%%", L(item.Description), row.HurryCostModifier ) )
	end

	-- Unit production modifier
	for row in GameInfo.Building_UnitCombatProductionModifiers( thisBuildingType ) do
		item = GameInfo.UnitCombatInfos[ row.UnitCombatType ]
		tips:insertIf( item and (row.Modifier or 0)~=0 and S( "%s %+i%%[ICON_PRODUCTION]", L(item.Description), row.Modifier ) )
	end
	for row in GameInfo.Building_DomainProductionModifiers( thisBuildingType ) do
		item = GameInfo.Domains[ row.DomainType ]
		tips:insertIf( item and (row.Modifier or 0)~=0 and S( "%s %+i%%[ICON_PRODUCTION]", L(item.Description), row.Modifier ) )
	end

	-- free experience
	for row in GameInfo.Building_DomainFreeExperiences( thisBuildingType ) do
		item = GameInfo.Domains[ row.DomainType ]
		tips:insertIf( item and (row.Experience or 0)~=0 and L(item.Description).." "..L( "TXT_KEY_EXPERIENCE_POPUP", row.Experience ) )
	end
	for row in GameInfo.Building_DomainFreeExperiencesGlobal( thisBuildingType ) do
		item = GameInfo.Domains[ row.DomainType ]
		tips:insertIf( item and (row.Experience or 0)~=0 and L( "TXT_KEY_EUI_GLOBAL" ).." "..L(item.Description).." "..L( "TXT_KEY_EXPERIENCE_POPUP", row.Experience ))
	end
	for row in GameInfo.Building_UnitCombatFreeExperiences( thisBuildingType ) do
		item = GameInfo.UnitCombatInfos[ row.UnitCombatType ]
		tips:insertIf( item and (row.Experience or 0)>0 and L(item.Description).." "..L( "TXT_KEY_EXPERIENCE_POPUP", row.Experience ) )
	end

	-- Yields enhanced by Technology
	if techFilter( enhancedYieldTech ) then
		tip = GetYieldString( GameInfo.Building_TechEnhancedYieldChanges( thisBuildingType ) )
		tips:insertIf( #tip > 0 and "[ICON_BULLET]" .. enhancedYieldTechName .. tip )
	end

	items = {}
	-- Yields enhanced by Policy
	for row in GameInfo.Policy_BuildingClassYieldChanges( thisBuildingClassType ) do
		if row.PolicyType and (row.YieldChange or 0)~=0 then
			items[row.PolicyType] = S( "%s %+i%s", items[row.PolicyType] or "", row.YieldChange, tostring(YieldIcons[row.YieldType]) )
		end
	end
	for row in GameInfo.Policy_BuildingClassCultureChanges( thisBuildingClassType ) do
		if row.PolicyType and (row.CultureChange or 0)~=0 then
			items[row.PolicyType] = S( "%s %+i[ICON_CULTURE]", items[row.PolicyType] or "", row.CultureChange )
		end
	end
	-- Yield modifiers enhanced by Policy
	for row in GameInfo.Policy_BuildingClassYieldModifiers( thisBuildingClassType ) do
		if row.PolicyType and (row.YieldMod or 0)~=0 then
			items[row.PolicyType] = S( "%s %+i%%%s", items[row.PolicyType] or "", row.YieldMod, tostring(YieldIcons[row.YieldType]) )
		end
	end
	if civ5_mode then
		if bnw_mode then
			for row in GameInfo.Policy_BuildingClassTourismModifiers( thisBuildingClassType ) do
				if row.PolicyType and (row.TourismModifier or 0)~=0 then
					items[row.PolicyType] = S( "%s %+i%%[ICON_TOURISM]", items[row.PolicyType] or "", row.TourismModifier )
				end
			end
		end
		for row in GameInfo.Policy_BuildingClassHappiness( thisBuildingClassType ) do
			if row.PolicyType and (row.Happiness or 0)~=0 then
				items[row.PolicyType] = S( "%s %+i[ICON_HAPPINESS_1]", items[row.PolicyType] or "", row.Happiness )
			end
		end
		local lastTip -- universal healthcare kludge
		for policyType, tip in pairs(items) do
			local policy = GameInfo.Policies[ policyType ]
			if policyFilter( policy ) and #tip > 0 then
				tip = "[ICON_BULLET]" .. PolicyColor( L(policy.Description) ) .. tip
				tips:insertIf( tip~=lastTip and tip )
				lastTip = tip
			end
		end

		if gk_mode then
			-- Yields enhanced by Beliefs
			items = {}
			for row in GameInfo.Belief_BuildingClassYieldChanges( thisBuildingClassType ) do
				if row.BeliefType and (row.YieldChange or 0)~=0 then
					items[row.BeliefType] = S( "%s %+i%s", items[row.BeliefType] or "", row.YieldChange, tostring(YieldIcons[row.YieldType]) )
				end
			end
			if maxGlobalInstances > 0 then -- world wonder
				for row in GameInfo.Belief_YieldChangeWorldWonder() do
					if row.BeliefType and (row.Yield or 0)~=0 then
						items[row.BeliefType] = S( "%s %+i%s", items[row.BeliefType] or "", row.Yield, tostring(YieldIcons[row.YieldType]) )
					end
				end
			end
			if bnw_mode then
				for row in GameInfo.Belief_BuildingClassTourism( thisBuildingClassType ) do
					if row.BeliefType and (row.Tourism or 0)~=0 then
						items[row.BeliefType] = S( "%s %+i[ICON_TOURISM]", items[row.BeliefType] or "", row.Tourism )
					end
				end
			end
			for row in GameInfo.Belief_BuildingClassHappiness( thisBuildingClassType ) do
				if row.BeliefType and (row.Happiness or 0)~=0 then
					items[row.BeliefType] = S( "%s %+i[ICON_HAPPINESS_1]", items[row.BeliefType] or "", row.Happiness )
				end
			end
			for beliefType, tip in pairs(items) do
				local belief = GameInfo.Beliefs[beliefType]
				tips:insertIf( beliefFilter( belief ) and #tip > 0 and "[ICON_BULLET]" .. BeliefColor( L(belief.ShortDescription) ) .. tip )
			end

			-- Other Building Yields enhanced by this Building
			local buildingClassTypes = {}
			for row in GameInfo.Building_BuildingClassYieldChanges( thisBuildingType ) do
				SetKey( buildingClassTypes, row.BuildingClassType )
			end
			if civ5_mode then
				for row in GameInfo.Building_BuildingClassHappiness( thisBuildingType ) do
					SetKey( buildingClassTypes, row.BuildingClassType )
				end
			end
			local condition = { BuildingType = buildingType }
			for buildingClassType in pairs( buildingClassTypes ) do
				condition.BuildingClassType = buildingClassType
				tip = GetYieldStringSpecial( "YieldChange", "%s %+i%s", GameInfo.Building_BuildingClassYieldChanges( condition ) )
				if civ5_mode then
					local happinessChange = 0
					for row in GameInfo.Building_BuildingClassHappiness( condition ) do
						happinessChange = happinessChange + (row.Happiness or 0)
					end
					if happinessChange ~= 0 then
						tip = S( "%s %+i[ICON_HAPPINESS_1]", tip, happinessChange )
					end
				end
				local enhancedBuilding = GetCivBuilding( activeCivilizationType, buildingClassType )
				tips:insertIf( enhancedBuilding and #tip > 0 and "[ICON_BULLET]" ..L"TXT_KEY_SV_ICONS_ALL".." ".. BuildingColor( L( enhancedBuilding.Description ) ) .. tip )
			end
		end
	else
		-- Orbital Production
		tips:insertLocalizedIfNonZero( "TXT_KEY_DOMAIN_PRODUCTION_MOD", building.OrbitalProductionModifier or 0, "TXT_KEY_ORBITAL_UNITS" )

		-- Orbital Coverage
		tips:insertLocalizedIfNonZero( "TXT_KEY_BUILDING_ORBITAL_COVERAGE", building.OrbitalCoverageChange or 0 )

		-- Anti-Orbital Strike
		tips:insertLocalizedIfNonZero( "TXT_KEY_UNITPERK_RANGE_AGAINST_ORBITAL_CHANGE", building.OrbitalStrikeRangeChange or 0 )

		-- Covert Ops Intrigue Cap
		tips:insertLocalizedIfNonZero( "TXT_KEY_BUILDING_CITY_INTRIGUE_CAP", -(building.IntrigueCapChange or 0) * (GameDefines.MAX_CITY_INTRIGUE_LEVELS or 0) / 100 )
	end

	tips:insert( "----------------" )

	-- Cost:
	local costTip
	-- League project
	if bnw_mode and building.UnlockedByLeague then
		if Game
			and Game.GetNumActiveLeagues() > 0
			and Game.GetActiveLeague()
		then
			costTip = L( "TXT_KEY_LEAGUE_PROJECT_COST_PER_PLAYER",
				Game.GetActiveLeague():GetProjectBuildingCostPerPlayer( buildingID ) )
		else
			local leagueProjectReward = GameInfo.LeagueProjectRewards{ Building = buildingType }()
			local leagueProject = leagueProjectReward and GameInfo.LeagueProjects{ RewardTier3 = leagueProjectReward.Type }()
			local costPerPlayer = leagueProject and leagueProject.CostPerPlayer	-- * GameInfo.GameSpeeds[ PreGame.GetGameSpeed() ].ConstructPercent * GameInfo.Eras[ PreGame.GetEra() ].ConstructPercent / 100000	-- GameInfo.Eras[ player:GetCurrentEra() ]
			if costPerPlayer then
				costTip = L( "TXT_KEY_LEAGUE_PROJECT_COST_PER_PLAYER", costPerPlayer )
			end
		end
	-- Production
	else
		if productionCost > 1 then
			costTip = productionCost .. "[ICON_PRODUCTION]"
			local goldCost = 0
			if city then
				goldCost = city:GetBuildingPurchaseCost( buildingID )

			elseif building.HurryCostModifier and building.HurryCostModifier >= 0 then
				goldCost = (productionCost * GameDefines.GOLD_PURCHASE_GOLD_PER_PRODUCTION ) ^ GameDefines.HURRY_GOLD_PRODUCTION_EXPONENT
				goldCost = (building.HurryCostModifier + 100) * goldCost / 100
				goldCost = goldCost - ( goldCost % GameDefines.GOLD_PURCHASE_VISIBLE_DIVISOR )
			end
			if goldCost > 0 then
				if costTip then
					costTip = costTip .. ("(%i%%)"):format(productionCost*100/goldCost)
					if civ5gk_mode then
						costTip = L( "TXT_KEY_PEDIA_A_OR_B", costTip, goldCost .. g_currencyIcon )
					else
						costTip = costTip .. " / " .. goldCost .. g_currencyIcon
					end
				else
					costTip = goldCost .. g_currencyIcon
				end
			end
		end
		-- Faith
		if g_isReligionEnabled then
			local faithCost = 0
			if city then
				faithCost = city:GetBuildingFaithPurchaseCost( buildingID, true )
			else
				faithCost = building.FaithCost or 0
			end
			if faithCost > 0 then
				if costTip then
					costTip = L( "TXT_KEY_PEDIA_A_OR_B", costTip, faithCost .. "[ICON_PEACE]" )
				else
					costTip = faithCost .. "[ICON_PEACE]"
				end
			end
		end
	end
	tips:insert( L"TXT_KEY_PEDIA_COST_LABEL" .. " " .. ( costTip or L"TXT_KEY_FREE" ) )

	-- Production Modifiers
	for row in GameInfo.Policy_BuildingClassProductionModifiers( thisBuildingClassType ) do
		local policy = GameInfo.Policies[ row.PolicyType ]
		tips:insertIf( policyFilter( policy ) and (row.ProductionModifier or 0)~=0 and S( "[ICON_BULLET]%s +%i%%[ICON_PRODUCTION]", PolicyColor( L(policy.Description) ), row.ProductionModifier ) )
	end


	if civBE_mode then
		-- Affinity Level Requirement
		for affinityPrereq in GameInfo.Building_AffinityPrereqs( thisBuildingType ) do
			local affinityInfo = (tonumber( affinityPrereq.Level) or 0 ) > 0 and GameInfo.Affinity_Types[ affinityPrereq.AffinityType ]
			tips:insertIf( affinityInfo and L( "TXT_KEY_AFFINITY_LEVEL_REQUIRED", affinityInfo.ColorType, affinityPrereq.Level, tostring(affinityInfo.IconString), tostring(affinityInfo.Description) ) )
		end
	end

	-- Civilization:
	local civs = table()
	for row in GameInfo.Civilization_BuildingClassOverrides( thisBuildingType ) do
		civs:insertLocalizedIf( ( GameInfo.Civilizations[ row.CivilizationType ] or{} ).ShortDescription )
	end
	tips:insertIf( #civs > 0 and L"TXT_KEY_PEDIA_CIVILIZATIONS_LABEL" .. " " .. civs:concat(", ") )

	-- Replaces:
	local buildingReplaced = buildingClass and GameInfo.Buildings[ buildingClass.DefaultBuilding ]
	tips:insertIf( buildingReplaced and buildingReplaced ~= building and L"TXT_KEY_PEDIA_REPLACES_LABEL".." "..BuildingColor( L(buildingReplaced.Description) ) ) --!!! row

	-- Required Social Policy:
	item = building.PolicyBranchType and GameInfo.PolicyBranchTypes[ building.PolicyBranchType ]
	tips:insertIf( item and L"TXT_KEY_PEDIA_PREREQ_POLICY_LABEL" .. " " .. PolicyColor( L(item.Description) ) )

	-- Prerequisite Techs:
	local techs = table()
	item = building.PrereqTech and GameInfo.Technologies[ building.PrereqTech ]
	techs:insertIf( item and item.ID > 0 and TechColor( L(item.Description) ) )
	for row in GameInfo.Building_TechAndPrereqs( thisBuildingType ) do
		item = GameInfo.Technologies[ row.TechType ]
		techs:insertIf( item and item.ID > 0 and TechColor( L(item.Description) ) )
	end
	tips:insertIf( #techs > 0 and L"TXT_KEY_PEDIA_PREREQ_TECH_LABEL" .. " " .. techs:concat( ", " ) )

	-- CBP Num Social Policies
	if(activePlayer)then
		local iNumPolicies = building.NumPoliciesNeeded;
		if(city) then
			iNumPolicies = city:GetNumPoliciesNeeded(buildingID)
		end
		if(iNumPolicies > 0) then
			local iNumHave = activePlayer:GetNumPolicies(true);
			tips:insert(L("TXT_KEY_PEDIA_NUM_POLICY_NEEDED_LABEL", iNumPolicies, iNumHave))
		end
	end

	--- National/Local Population
	if(activePlayer) then
		local iNumNationalPop = activePlayer:GetScalingNationalPopulationRequrired(buildingID);
		if(iNumNationalPop > 0) then
			local iNumHave = activePlayer:GetTotalPopulation();
			tips:insert(L("TXT_KEY_PEDIA_NUM_POPULATION_NATIONAL_NEEDED_LABEL", iNumNationalPop, iNumHave))
		end
	end
	local iNumLocalPop = building.LocalPopRequired;
	if(iNumLocalPop > 0) then
		if (city) then
			local iNumHave = city:GetPopulation();
			tips:insert(L("TXT_KEY_PEDIA_NUM_POPULATION_LOCAL_NEEDED_LABEL", iNumLocalPop, iNumHave))
		end
	end

	-- Required Buildings:
	local buildings = table()
	for row in GameInfo.Building_PrereqBuildingClasses( thisBuildingType ) do
		local item = GetCivBuilding( activeCivilizationType, row.BuildingClassType )
		if item then
			tips:insert( L"TXT_KEY_PEDIA_REQ_BLDG_LABEL".." ".. BuildingColor( L(item.Description) ).." ("..((row.NumBuildingNeeded or -1)==-1 and L"TXT_KEY_SV_ICONS_ALL" or row.NumBuildingNeeded)..")" )
			buildings[ item ] = true
		end
	end
	for row in GameInfo.Building_ClassesNeededInCity( thisBuildingType ) do
		local item = GetCivBuilding( activeCivilizationType, row.BuildingClassType )
		buildings:insertIf( item and not buildings[ item ] and BuildingColor( L(item.Description) ) )
	end
	items = {}
	if (building.MutuallyExclusiveGroup or -1) >= 0 then
		for row in GameInfo.Buildings{ MutuallyExclusiveGroup = building.MutuallyExclusiveGroup } do
			SetKey( items, row.BuildingClass ~= buildingClassType and row.BuildingClass )
		end
	end
	for buildingClassType in pairs( items ) do
		local item = GetCivBuilding( activeCivilizationType, buildingClassType )
		buildings:insertIf( item and TextColor( "[COLOR_RED]", L(item.Description) ) )
	end
	tips:insertIf( #buildings > 0 and L"TXT_KEY_PEDIA_REQ_BLDG_LABEL" .. " " .. buildings:concat(", ") )

	-- Local Resources Required:
	local resources = table()
	for row in GameInfo.Building_LocalResourceAnds( thisBuildingType ) do
		local resource = GameInfo.Resources[ row.ResourceType ]
		resources:insertIf( resource and tostring(resource.IconString) ) -- .. L(resource.Description) )
	end
	if #resources > 0 then
		resources = table( resources:concat(", ") )
	end
	for row in GameInfo.Building_LocalResourceOrs( thisBuildingType ) do
		local resource = GameInfo.Resources[ row.ResourceType ]
		resources:insertIf( resource and tostring(resource.IconString) ) -- .. L(resource.Description) )
	end
	if civ5gk_mode then
		local txt = resources[1] or ""
		for i = 2, #resources do
			txt = L( "TXT_KEY_PEDIA_A_OR_B", txt, resources[i] )
		end
		resources = txt
	else
		resources = resources:concat(" / ")
	end
	tips:insertIf( #resources > 0 and L"TXT_KEY_PEDIA_LOCAL_RESRC_LABEL" .. " " .. resources )

	local terrains = table()
	terrains:insertLocalizedIf( building.Water and "TXT_KEY_TERRAIN_COAST" )
	terrains:insertLocalizedIf( building.River and "TXT_KEY_PLOTROLL_RIVER" )
	terrains:insertLocalizedIf( building.FreshWater and "TXT_KEY_ABLTY_FRESH_WATER_STRING" )
	terrains:insertIf( building.Mountain and L"TXT_KEY_TERRAIN_MOUNTAIN" .. "[ICON_RANGE_STRENGTH]1" )
	terrains:insertIf( building.NearbyMountainRequired and L"TXT_KEY_TERRAIN_MOUNTAIN" .. "[ICON_RANGE_STRENGTH]2" )
	terrains:insertLocalizedIf( building.Hill and "TXT_KEY_TERRAIN_HILL" )
	terrains:insertLocalizedIf( building.Flat and "TXT_KEY_MAP_OPTION_FLAT" )
	-- mandatory terrain
	local terrain = building.NearbyTerrainRequired and GameInfo.Terrains[ building.NearbyTerrainRequired ]
	terrains:insertIf( terrain and L(terrain.Description) .. "[ICON_RANGE_STRENGTH]1" )
	-- prohibited terrain
	terrain = building.ProhibitedCityTerrain and GameInfo.Terrains[ building.ProhibitedCityTerrain ]
	terrains:insertIf( terrain and TextColor( "[COLOR_RED]", L( terrain.Description ) ) )

	tips:insertIf( #terrains > 0 and L"TXT_KEY_PEDIA_TERRAIN_LABEL"..": "..terrains:concat(", ") )

	-- Becomes Obsolete with:
	local obsoleteTech = building.ObsoleteTech and GameInfo.Technologies[ building.ObsoleteTech ]
	tips:insertIf( obsoleteTech and L"TXT_KEY_PEDIA_OBSOLETE_TECH_LABEL" .. " " .. TechColor( L(obsoleteTech.Description) ) )

	-- Buildings Unlocked:
	local buildingsUnlocked = {}
	for row in GameInfo.Building_ClassesNeededInCity( thisBuildingClassType ) do
		local buildingUnlocked = GameInfo.Buildings[ row.BuildingType ]
		SetKey( buildingsUnlocked, buildingUnlocked and buildingUnlocked.BuildingClass )
	end
	local buildings = table()
	for buildingUnlocked in pairs(buildingsUnlocked) do
		buildingUnlocked = GetCivBuilding( activeCivilizationType, buildingUnlocked )
		buildings:insertIf( buildingUnlocked and BuildingColor( L(buildingUnlocked.Description) ) )
	end
	tips:insertIf( #buildings > 0 and L"TXT_KEY_PEDIA_BLDG_UNLOCK_LABEL" .. " " .. buildings:concat(", ") )

	-- Limited number can be built
	local function InsertBuiltLimit( count, text, id, activeTeamID )
		if (count or 0) > 0 then
			if activePlayer then
				for playerID = id or 0, id or (GameDefines.MAX_MAJOR_CIVS - 1) do
					local player = Players[playerID]
					if player and player:IsAlive() and (not activeTeamID or player:GetTeam() == activeTeamID) then
						for city in player:Cities() do
							local n = city:GetNumBuilding( buildingID )
							if n > 0 then
								count = count - n
								local builderID = city:GetBuildingOriginalOwner( buildingID )
								local builder = Players[ builderID ] or player
								tips:insert( (playerID == activePlayerID and "[COLOR_POSITIVE_TEXT]" or "[COLOR_WARNING_TEXT]")..L( "TXT_KEY_EUI_WONDER_BUILT_BY", builderID == activePlayerID and "TXT_KEY_YOU" or (activeTeam:IsHasMet(builder:GetTeam()) and builder:GetName()) or "TXT_KEY_UNMET_PLAYER", city:Plot():IsRevealed( activeTeamID ) and city:GetName() or "TXT_KEY_EUI_UNKNOWN_CITY" ).."[ENDCOLOR]" )
							end
							if count < 1 then return true end
						end
					end
				end
			end
			if id==false and Game and Game.GetBuildingClassCreatedCount( buildingClassID ) >= maxGlobalInstances then
				tips:insert( "[COLOR_WARNING_TEXT]"..L"TXT_KEY_RAZED_CITY".."[ENDCOLOR]" )
			else
				tips:append( L( text, count ) )
			end
			return true
		end
	end
	if not ( InsertBuiltLimit( maxGlobalInstances, "TXT_KEY_NO_ACTION_GAME_COUNT_MAX", false )
		or InsertBuiltLimit( buildingClass.MaxTeamInstances, "TXT_KEY_NO_ACTION_TEAM_COUNT_MAX", nil, activeTeamID )
		or InsertBuiltLimit( buildingClass.MaxPlayerInstances, "TXT_KEY_NO_ACTION_PLAYER_COUNT_MAX", activePlayerID ) )
		and activePlayer
	then
		tips:insertLocalized( "TXT_KEY_EUI_BUILT_IN_X_CITIES", activePlayer:GetBuildingClassCount( buildingClassID ), activePlayer:GetBuildingClassMaking( buildingClassID ) )    --CountNumBuildings( buildingID ) )
	end
	-- Pre-written Help text
-- CBP
	if (city ~= nil) then	
		if(city:GetNumRealBuilding(buildingID) <= 0) then
			if(city:GetNumFreeBuilding(buildingID) <= 0) then
				if(city:GetBuildingInvestment(buildingID) <= 0) then
					local iAmount = GameDefines.BALANCE_BUILDING_INVESTMENT_BASELINE;
					iAmount = (iAmount * -1);
					local iWonderAmount = (iAmount / 2);
					if(iAmount > 0) then
						tips:append( ( "[NEWLINE]" .. L("TXT_KEY_PRODUCTION_INVESTMENT_BUILDING", iAmount, iWonderAmount) ) )
					end
				end
				if(city:IsWorldWonder(buildingID)) then
					local iCost = city:GetWorldWonderCost(buildingID);
					if(iCost > 0) then
						-- Separator
						tips:append( ( "[NEWLINE]" .. L("TXT_KEY_WONDER_COST_INCREASE_METRIC", iCost)) )
					end
				end
			end
		end
	end
-- END
	return AddPreWrittenHelpTextAndConcat( tips, building )
end

-------------------------------------------------
-- Help text for Improvements
-------------------------------------------------
local function GetHelpTextForImprovement( improvementID )
	local improvement = improvementID and GameInfo.Improvements[ improvementID ]

	local thisImprovementType = { ImprovementType = improvement.Type or -1 }
	local ItemBuild = GameInfo.Builds( thisImprovementType )()
	local tip, items, item, condition

	-- Name
	local tips = table( TextColor( "[COLOR_GREEN]", Locale_ToUpper( improvement.Description ) ) )

	-- Maintenance:
	tips:insertIf( (improvement[g_maintenanceCurrency] or 0) ~= 0 and S( "%s %+i%s", L"TXT_KEY_PEDIA_MAINT_LABEL", -improvement[g_maintenanceCurrency], g_currencyIcon) )

	-- Improved Resources
	items = table()
	for row in GameInfo.Improvement_ResourceTypes( thisImprovementType ) do
		item = GameInfo.Resources[ row.ResourceType ]
		items:insertIf( item and tostring(item.IconString) )
	end
	tips:insertIf( #items > 0  and L"TXT_KEY_PEDIA_IMPROVES_RESRC_LABEL" .. " " .. items:concat( ", " ) )

	-- Yields
	items = table()
	for row in GameInfo.Improvement_Yields( thisImprovementType ) do
		SetKey( items, (row.Yield or 0)~=0 and row.YieldType, S("%s %+i%s", items[ row.YieldType ] or "", row.Yield, tostring(YieldIcons[ row.YieldType ]) ) )
	end
	for row in GameInfo.Improvement_FreshWaterYields( thisImprovementType ) do
		SetKey( items, (row.Yield or 0)~=0 and row.YieldType, S( "%s %s %+i%s", items[ row.YieldType ] or "", L"TXT_KEY_ABLTY_FRESH_WATER_STRING", row.Yield, tostring(YieldIcons[ row.YieldType ]) ) )
	end
	if bnw_mode then
		for row in GameInfo.Improvement_YieldPerEra( thisImprovementType ) do
			SetKey( items, (row.Yield or 0)~=0 and row.YieldType, S( "%s %+i%s/%s", items[ row.YieldType ] or "", row.Yield, tostring(YieldIcons[ row.YieldType ]), L"TXT_KEY_AD_SETUP_GAME_ERA" ) )
		end
	end
	for yieldType, tip in pairs( items ) do
		tips:insert( tostring(YieldNames[ yieldType ]) .. ":" ..  tip )
	end

	-- Culture
	tips:insertIf( (improvement.Culture or 0)~=0 and S( "%s: %+i[ICON_CULTURE]", L"TXT_KEY_CITYVIEW_CULTURE_TEXT", improvement.Culture ) )

	-- Mountain Bonus
	tip = GetYieldString( GameInfo.Improvement_AdjacentMountainYieldChanges( thisImprovementType ) )
	tips:insertIf( #tip>0 and L"TXT_KEY_PEDIA_MOUNTAINADJYIELD_LABEL" .. tip )
	-- Defense Modifier
	tips:insertIf( (improvement.DefenseModifier or 0)~=0 and S( "%s %+i%%[ICON_STRENGTH]", L"TXT_KEY_PEDIA_DEFENSE_LABEL", improvement.DefenseModifier ) )

	-- Tech yield changes
	items = {}
	condition = { ImprovementType = improvement.Type }
	for row in GameInfo.Improvement_TechYieldChanges( thisImprovementType ) do
		SetKey( items, row.TechType )
	end
	for techType in pairs( items ) do
		item = GameInfo.Technologies[ techType ]
		if item then
			condition.TechType = techType
			tip = GetYieldString( GameInfo.Improvement_TechYieldChanges( condition ) )
			tips:insertIf( tip~="" and "[ICON_BULLET]" .. TechColor( L(item.Description) ) .. tip )
		end
	end
	items = {}
	for row in GameInfo.Improvement_TechFreshWaterYieldChanges( thisImprovementType ) do
		SetKey( items, row.TechType )
	end
	for techType in pairs(items) do
		item = GameInfo.Technologies[ techType ]
		if item then
			condition.TechType = techType
			tip = GetYieldString( GameInfo.Improvement_TechFreshWaterYieldChanges( condition ) )
			tips:insertIf( tip~="" and "[ICON_BULLET]" .. TechColor( L(item.Description) ) .. " (" .. L"TXT_KEY_ABLTY_FRESH_WATER_STRING" .. ")" .. tip )
		end
	end
	items = {}
	for row in GameInfo.Improvement_TechNoFreshWaterYieldChanges( thisImprovementType ) do
		SetKey( items, row.TechType )
	end
	for techType in pairs(items) do
		item = GameInfo.Technologies[ techType ]
		if item then
			condition.TechType = techType
			tip = GetYieldString( GameInfo.Improvement_TechNoFreshWaterYieldChanges( condition ) )
			tips:insertIf( tip~="" and "[ICON_BULLET]" .. TechColor( L(item.Description) ) .. " (" .. L("TXT_KEY_ABLTY_NO_FRESH_WATER_STRING") .. ")" .. tip )
		end
	end

	-- Policy yield changes
	items = {}
	condition = { ImprovementType = improvement.Type }
	for row in GameInfo.Policy_ImprovementYieldChanges( thisImprovementType ) do
		SetKey( items, row.PolicyType )
	end
	for row in GameInfo.Policy_ImprovementCultureChanges( thisImprovementType ) do
		SetKey( items, row.PolicyType )
	end
	for policyType in pairs(items) do
		item = GameInfo.Policies[ policyType ]
		if item then
			tip = ""
			condition.PolicyType = policyType
			tip = GetYieldString( GameInfo.Policy_ImprovementYieldChanges( condition ) )
			for row in GameInfo.Policy_ImprovementCultureChanges( condition ) do
				if (row.CultureChange or 0)~=0 then
					tip = S( "%s %+i[ICON_CULTURE]", tip, row.CultureChange )
				end
			end
			tips:insertIf( tip~="" and "[ICON_BULLET]" .. PolicyColor( L(item.Description) ) .. tip )
		end
	end

	-- Belief yield changes
	if g_isReligionEnabled then
		items = {}
		condition = { ImprovementType = improvement.Type }
		for row in GameInfo.Belief_ImprovementYieldChanges( thisImprovementType ) do
			SetKey( items, row.BeliefType )
		end
		for beliefType in pairs(items) do
			item = GameInfo.Beliefs[ beliefType ]
			if item then
				condition.BeliefType = beliefType
				tip = GetYieldString( GameInfo.Belief_ImprovementYieldChanges( condition ) )
				tips:insertIf( tip~="" and "[ICON_BULLET]" .. BeliefColor( L(item.ShortDescription) ) .. tip )
			end
		end

	end

	-- Resource Yields
	local thisImprovementAndResourceTypes = { ImprovementType = improvement.Type or -1 }
	for resource in GameInfo.Resources() do
		thisImprovementAndResourceTypes.ResourceType = resource.Type or -1
		tip = GetYieldString( GameInfo.Improvement_ResourceType_Yields( thisImprovementAndResourceTypes ) )
		tips:insertIf( #tip > 0 and tostring(resource.IconString) .. " " .. L(resource.Description) .. ":" .. tip )
	end

	tips:insert( "----------------" )

	-- Civ Requirement
	item = improvement.SpecificCivRequired and improvement.CivilizationType
	item = item and GameInfo.Civilizations[ item ]
	tips:insertIf( item and L"TXT_KEY_PEDIA_CIVILIZATIONS_LABEL".." "..L( item.ShortDescription ) )

	-- Tech Requirements
	item = ItemBuild and ItemBuild.PrereqTech and GameInfo.Technologies[ItemBuild.PrereqTech]
	tips:insertIf( item and item.ID > 0  and L"TXT_KEY_PEDIA_PREREQ_TECH_LABEL" .. " " .. TechColor( L(item.Description) ) )

	-- Terrain
	items = table()
	for row in GameInfo.Improvement_ValidTerrains( thisImprovementType ) do
		item = GameInfo.Terrains[ row.TerrainType ]
		items:insertIf( item and L( item.Description ) )
	end
	for row in GameInfo.Improvement_ValidFeatures( thisImprovementType ) do
		item = GameInfo.Features[ row.FeatureType ]
		items:insertIf( item and L( item.Description ) )
	end
	if bnw_mode then
		for row in GameInfo.Improvement_ValidImprovements( thisImprovementType ) do
			item = GameInfo.Improvements[ row.PrereqImprovement ]
			items:insertIf( item and L(item.Description ) )
		end
	end
	for row in GameInfo.Improvement_ResourceTypes( thisImprovementType ) do
		item = GameInfo.Resources[ row.ResourceType ]
		items:insertIf( item and row.ResourceMakesValid and item.IconString )
	end
--	items:insertIf( improvement.HillsMakesValid and L( (GameInfo.Terrains.TERRAIN_HILL or {}).Description ) )-- hackery for hills
	tips:insertIf( #items > 0  and L"TXT_KEY_PEDIA_FOUNDON_LABEL" .. " " .. items:concat( ", " ) )

--[[
	-- Upgrade
	item = improvement.ImprovementUpgrade and GameInfo.Improvements[ improvement.ImprovementUpgrade ]
-- TODO xml text
	tips:insertIf( item and L"" .. " " .. L(item.Description) )
--]]
	-- Pre-written Help text
	return AddPreWrittenHelpTextAndConcat( tips, improvement )
end

-- ===========================================================================
-- Help text for Projects
-- ===========================================================================
local function GetHelpTextForProject( projectID, city )
	local project = GameInfo.Projects[ projectID ]

	local productionCost = 0;
	-- Name & Cost
	if(city ~= nil) then
		productionCost = city:GetProjectProductionNeeded(projectID)
	else
		productionCost = (Game and Players[Game.GetActivePlayer()]:GetProjectProductionNeeded(projectID)) or project.Cost
	end
	local tips = table( Locale_ToUpper( project.Description ), "----------------", L"TXT_KEY_PEDIA_COST_LABEL" .. " " .. productionCost .. "[ICON_PRODUCTION]" )

	if civBE_mode then
		-- Affinity Level Requirement
		for affinityPrereq in GameInfo.Project_AffinityPrereqs{ ProjectType = project.Type } do
			local affinityInfo = (tonumber( affinityPrereq.Level) or 0 ) > 0 and GameInfo.Affinity_Types[ affinityPrereq.AffinityType ]
			tips:insertIf( affinityInfo and L( "TXT_KEY_AFFINITY_LEVEL_REQUIRED", affinityInfo.ColorType, affinityPrereq.Level, tostring(affinityInfo.IconString), tostring(affinityInfo.Description) ) )
		end
	end

	-- Requirements?
	tips:insertLocalizedIf( project.Requirements )

	-- Pre-written Help text
	return AddPreWrittenHelpTextAndConcat( tips, project )
end

-- ===========================================================================
-- Help text for Processes
-- ===========================================================================
local function GetHelpTextForProcess( processID )
	local process = GameInfo.Processes[ processID ]

	-- Name
	local tips = table( Locale_ToUpper( process.Description ), "----------------" )
	for row in GameInfo.Process_ProductionYields{ ProcessType = process.Type } do
		local yield = GameInfo.Yields[ row.YieldType ]
		local percent = yield and tonumber( row.Yield ) or 0
		if percent == 0 then
		elseif civBE_mode then
			tips:insertLocalized( "TXT_KEY_PROCESS_GENERIC_HELP", percent, tostring(yield.IconString), tostring(yield.Description) )
		else
			tips:insert( S( "%s = %i%%([ICON_PRODUCTION])", tostring(yield.IconString), percent ) )
		end
	end
	-- League Project text
	local activeLeague = civ5bnw_mode and Game and not Game.IsOption("GAMEOPTION_NO_LEAGUES") and Game.GetActiveLeague()
	if activeLeague then
		for row in GameInfo.LeagueProjects{ Process = process.Type } do
			tips:insert( activeLeague:GetProjectDetails( GameInfoTypes[row.Type], Game.GetActivePlayer() ) )
		end
	end

	-- Pre-written Help text
	return AddPreWrittenHelpTextAndConcat( tips, process )
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

-- Helper function to build yield tooltip string
local function GetYieldTooltip( city, yieldID, baseYield, totalYield, yieldIconString, strModifiersString )

	yieldIconString = tostring(yieldIconString or YieldIcons[yieldID])
	local tips = table()

	-- Base Yield from Terrain
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_TERRAIN", city:GetBaseYieldRateFromTerrain( yieldID ), yieldIconString )

	-- Base Yield from Buildings
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_BUILDINGS", city:GetBaseYieldRateFromBuildings( yieldID ), yieldIconString )

	-- Base Yield from Specialists
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_SPECIALISTS", city:GetBaseYieldRateFromSpecialists( yieldID ), yieldIconString )

	-- Base Yield from Misc-
	tips:insertLocalizedBulletIfNonZero( yieldID == YieldTypes.YIELD_SCIENCE and "TXT_KEY_YIELD_FROM_POP" or "TXT_KEY_YIELD_FROM_MISC", city:GetBaseYieldRateFromMisc( yieldID ), yieldIconString )

	-- Base Yield from Population
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_POP_EXTRA", city:GetYieldPerPopTimes100( yieldID ) * city:GetPopulation() / 100, yieldIconString )

	-- Base Yield from Religion
	if gk_mode then
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_RELIGION", city:GetBaseYieldRateFromReligion( yieldID ), yieldIconString )
	end

-- CBP
	-- Yield Increase from City Yields
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_CITY_YIELDS", city:GetYieldFromCityYield( yieldID ), yieldIconString)

	-- Yield Increase from CS Alliance (Germany)
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_CS_ALLIANCE", city:GetBaseYieldRateFromCSAlliance( yieldID ), yieldIconString)

	-- Yield Increase from Corporations
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_CORPORATIONS", city:GetYieldChangeFromCorporationFranchises( yieldID ), yieldIconString)

	-- Yield Increase from Piety
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_PIETY", city:GetReligionYieldRateModifier( yieldID ), yieldIconString)

	-- Base Yield From City Connections
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_CONNECTION", city:GetYieldChangeTradeRoute( yieldID ), yieldIconString)

	-- Yield from Great Works
	if(yieldID ~= YieldTypes.YIELD_CULTURE) then
		tips:insertLocalizedBulletIfNonZero("TXT_KEY_YIELD_FROM_ART_CBP", city:GetBaseYieldRateFromGreatWorks( yieldID ), yieldIconString)
	end
	
	if(yieldID ~= YieldTypes.YIELD_CULTURE) then
		tips:insertLocalizedBulletIfNonZero("TXT_KEY_YIELD_FROM_TRAIT_BONUS", city:GetYieldPerTurnFromTraits( yieldID ), yieldIconString)
	end

-- CBP
	-- WLTKD MOD
	--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_WLTKD", city:GetModFromWLTKD(yieldID), yieldIconString)

	-- Golden Age MOD
	--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_GOLDEN_AGE", city:GetModFromGoldenAge(yieldID), yieldIconString)

	-- CP EVENTS
	-- Base Yield from Events
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_EVENTS", city:GetEventCityYield(yieldID), yieldIconString)

-- END CBP

	-- Vox Populi Base Yield from Process
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_PROCESS", city:GetBaseYieldRateFromProcess( yieldID ), yieldIconString)

-- Base Yield from League Art (CSD)
	if(yieldID == YieldTypes.YIELD_SCIENCE) then
		local iYieldFromLeague = city:GetBaseYieldRateFromLeague(yieldID);
		if (iYieldFromLeague ~= 0) then
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_SCIENCE_YIELD_FROM_LEAGUE_ART", iYieldFromLeague, yieldIconString)
		end
	end
-- END

	if civBE_mode then
		-- Yield from Production Processes
		local yieldFromProcesses = city:GetYieldRateFromProductionProcesses( yieldID )
		if yieldFromProcesses ~= 0 then
			local processInfo = GameInfo.Processes[ city:GetProductionProcess() ]
			strModifiersString = strModifiersString .. "[NEWLINE][ICON_BULLET]" .. L( "TXT_KEY_SHORT_YIELD_FROM_SPECIFIC_OBJECT", yieldFromProcesses, yieldIconString, tostring(processInfo and processInfo.Description) )
		end
		-- Base Yield from Loadout (colonists)
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_LOADOUT", city:GetYieldPerTurnFromLoadout( yieldID ), yieldIconString )
	end

	-- Food eaten by pop
	if yieldID == YieldTypes.YIELD_FOOD then
		-- Vox Populi food from TRs is listed with modifiers
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FOOD_FROM_TRADE_ROUTES", city:GetYieldRate( yieldID, false ) - city:GetYieldRate( yieldID, true ) )
		--strModifiersString = "[NEWLINE][ICON_BULLET]" .. L( "TXT_KEY_YIELD_EATEN_BY_POP", city:FoodConsumption( true, 0 ), yieldIconString ) .. strModifiersString
	elseif civBE_mode then
		local yieldFromTrade = city:GetYieldPerTurnFromTrade( yieldID )
		if yieldFromTrade ~= 0 then
			strModifiersString = strModifiersString .. "[NEWLINE][ICON_BULLET]" .. L( "TXT_KEY_YIELD_FROM_TRADE", yieldFromTrade, yieldIconString )
		end
	end

	-- Modifiers
	if strModifiersString ~= "" then
		tips:insert( "----------------" )
		tips:insertLocalized( "TXT_KEY_YIELD_BASE", baseYield, yieldIconString )
		tips:insert( strModifiersString )
	end

	-- Vox Populi Base Yield from Trade Routes
	if yieldID ~= YieldTypes.YIELD_FOOD then
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_TRADE_ROUTES", city:GetBaseYieldRateFromTradeRoutes( yieldID )/100.0, yieldIconString)
	end
	
	-- Total
	tips:insert( "----------------" )
	tips:insertLocalized( totalYield >= 0 and "TXT_KEY_YIELD_TOTAL" or "TXT_KEY_YIELD_TOTAL_NEGATIVE", totalYield, yieldIconString )

	return tips:concat( "[NEWLINE]" )

end

-- Yield Tooltip Helper
local function GetYieldTooltipHelper( city, yieldID, yieldIconString )

	return GetYieldTooltip( city, yieldID, city:GetBaseYieldRate( yieldID ) + city:GetYieldPerPopTimes100( yieldID ) * city:GetPopulation() / 100, yieldID == YieldTypes.YIELD_FOOD and city:FoodDifferenceTimes100()/100 or city:GetYieldRateTimes100( yieldID )/100, yieldIconString, city:GetYieldModifierTooltip( yieldID ) )
end

-- FOOD
local function GetFoodTooltip( city )

	local tipText
	local isNoob = civBE_mode or not OptionsManager.IsNoBasicHelp()
	local cityPopulation = city:GetPopulation()

	local foodStoredTimes100 = city:GetFoodTimes100()
	local foodPerTurnTimes100 = city:FoodDifferenceTimes100( true )	-- true means size 1 city cannot starve
	local foodThreshold = city:GrowthThreshold()
	local turnsToCityGrowth = city:GetFoodTurnsLeft()

	if foodPerTurnTimes100 < 0 then
		foodThreshold = 0
		turnsToCityGrowth = math_floor( foodStoredTimes100 / -foodPerTurnTimes100 ) + 1
		tipText = "[COLOR_WARNING_TEXT]" .. cityPopulation - 1 .. "[ENDCOLOR][ICON_CITIZEN]"
		if isNoob then
			tipText = L( "TXT_KEY_CITYVIEW_STARVATION_TEXT" ) .. "[NEWLINE]"
			.. L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
		end
	elseif city:IsForcedAvoidGrowth() then
		tipText = "[ICON_LOCKED]".. cityPopulation .."[ICON_CITIZEN]"
		if isNoob then
			tipText = L"TXT_KEY_CITYVIEW_FOCUS_AVOID_GROWTH_TEXT" .. " " .. tipText
		end
		foodPerTurnTimes100 = 0
	elseif foodPerTurnTimes100 == 0 then
		tipText = "[COLOR_YELLOW]" .. cityPopulation .. "[ENDCOLOR][ICON_CITIZEN]"
		if isNoob then
			tipText = L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
		end
	else
		tipText = "[COLOR_POSITIVE_TEXT]" .. cityPopulation +1 .. "[ENDCOLOR][ICON_CITIZEN]"
		if isNoob then
			tipText = L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
		end
	end

	tipText = GetYieldTooltipHelper( city, YieldTypes.YIELD_FOOD ) .. "[NEWLINE][NEWLINE]" .. tipText .. "  " .. foodStoredTimes100 / 100 .. "[ICON_FOOD]/ " .. foodThreshold .. "[ICON_FOOD][NEWLINE]"

	if foodPerTurnTimes100 == 0 then

		tipText = tipText .. L"TXT_KEY_CITYVIEW_STAGNATION_TEXT"
	else
		local foodOverflowTimes100 = foodPerTurnTimes100 * turnsToCityGrowth + foodStoredTimes100 - foodThreshold * 100

		if turnsToCityGrowth > 1 then
			tipText = S( "%s%s %+g[ICON_FOOD]  ", tipText, L( "TXT_KEY_STR_TURNS", turnsToCityGrowth -1 ), ( foodOverflowTimes100 - foodPerTurnTimes100 ) / 100 )
		end
		tipText =  S( "%s%s%s[ENDCOLOR] %+g[ICON_FOOD]", tipText, foodPerTurnTimes100 < 0 and "[COLOR_WARNING_TEXT]" or "[COLOR_POSITIVE_TEXT]", Locale_ToUpper( L( "TXT_KEY_STR_TURNS", turnsToCityGrowth ) ), foodOverflowTimes100 / 100 )
	end

    tipText = tipText .. city:getPotentialUnhappinessWithGrowth(); 

	if isNoob then
		return L"TXT_KEY_FOOD_HELP_INFO" .. "[NEWLINE][NEWLINE]" .. tipText
	else
		return tipText
	end
end

-- GOLD
local function GetGoldTooltip( city )

	if civ5_mode and OptionsManager.IsNoBasicHelp() then
		return GetYieldTooltipHelper( city, YieldTypes.YIELD_GOLD )
	else
		return L"TXT_KEY_GOLD_HELP_INFO" .. "[NEWLINE][NEWLINE]" .. GetYieldTooltipHelper( city, YieldTypes.YIELD_GOLD )
	end
end

-- ENERGY
local function GetEnergyTooltip( city )

	return L"TXT_KEY_ENERGY_HELP_INFO" .. "[NEWLINE][NEWLINE]" .. GetYieldTooltipHelper( city, YieldTypes.YIELD_ENERGY )
end

-- SCIENCE
local function GetScienceTooltip( city )

	if g_isScienceEnabled then
		if civ5_mode and OptionsManager.IsNoBasicHelp() then
			return GetYieldTooltipHelper( city, YieldTypes.YIELD_SCIENCE )
		else
			return L"TXT_KEY_SCIENCE_HELP_INFO" .. "[NEWLINE][NEWLINE]" .. GetYieldTooltipHelper( city, YieldTypes.YIELD_SCIENCE )
		end
	else
		return L"TXT_KEY_TOP_PANEL_SCIENCE_OFF_TOOLTIP"
	end
end

-- PRODUCTION
local function GetProductionTooltip( city )

	local isNoob = civBE_mode or not OptionsManager.IsNoBasicHelp()
	local cityProductionName = city:GetProductionNameKey()
	local tipText = ""
	local productionColor

	local productionPerTurn100 = city:GetCurrentProductionDifferenceTimes100( false, false )	-- food = false, overflow = false
	local productionStored100 = city:GetProductionTimes100() + city:GetCurrentProductionDifferenceTimes100(false, true) - productionPerTurn100
	local productionNeeded = 0
	local productionTurnsLeft = 1

	local unitProductionID = city:GetProductionUnit()
	local buildingProductionID = city:GetProductionBuilding()
	local projectProductionID = city:GetProductionProject()
	local processProductionID = city:GetProductionProcess()

	if unitProductionID ~= -1 then
--		tipText = GetHelpTextForUnit( unitProductionID, false )
		productionColor = UnitColor

	elseif buildingProductionID ~= -1 then
--		tipText = GetHelpTextForBuilding( buildingProductionID, true, true, true, city )
		productionColor = BuildingColor

	elseif projectProductionID ~= -1 then
--		tipText = GetHelpTextForProject( projectProductionID, false )
		productionColor = BuildingColor

	elseif processProductionID ~= -1 then
		tipText = GetHelpTextForProcess( processProductionID, false )
	else
		if isNoob then
			tipText = L( "TXT_KEY_CITY_NOT_PRODUCING", city:GetName() )
		else
			tipText = L"TXT_KEY_PRODUCTION_NO_PRODUCTION"
		end
	end

	if productionColor then
		productionNeeded = city:GetProductionNeeded()
		productionTurnsLeft = city:GetProductionTurnsLeft()
		tipText = productionColor( Locale_ToUpper( cityProductionName ) )
		if isNoob then
			tipText = L( "TXT_KEY_PROGRESS_TOWARDS", tipText )
		end
		tipText = tipText .. "  " .. productionStored100 / 100 .. "[ICON_PRODUCTION]/ " .. productionNeeded .. "[ICON_PRODUCTION]"
		if productionPerTurn100 > 0 then

			tipText = tipText .. "[NEWLINE]"

			local productionOverflow100 = productionPerTurn100 * productionTurnsLeft + productionStored100 - productionNeeded * 100
			if productionTurnsLeft > 1 then
				tipText =  S( "%s%s %+g[ICON_PRODUCTION]  ", tipText, L( "TXT_KEY_STR_TURNS", productionTurnsLeft -1 ), ( productionOverflow100 - productionPerTurn100 ) / 100 )
			end
			tipText = S( "%s%s %+g[ICON_PRODUCTION]", tipText, productionColor( Locale_ToUpper( L( "TXT_KEY_STR_TURNS", productionTurnsLeft ) ) ), productionOverflow100 / 100 )
		end
	end
	local strModifiersString = city:GetYieldModifierTooltip( YieldTypes.YIELD_PRODUCTION )
	-- Extra Production from Food (ie. producing Colonists)
	if city:IsFoodProduction() then
		local productionFromFood = city:GetYieldRate( YieldTypes.YIELD_FOOD, false ) - city:FoodConsumption( true, 0 )
		if productionFromFood <= 0 then
			productionFromFood = 0
		elseif productionFromFood <= 2 then
			productionFromFood = productionFromFood * 100
		elseif productionFromFood <= 4 then
			productionFromFood = 200 + (productionFromFood - 2) * 50
		else
			productionFromFood = 300 + (productionFromFood - 4) * 25
		end
		if productionFromFood > 0 then
			strModifiersString = strModifiersString .. L( "TXT_KEY_PRODMOD_FOOD_CONVERSION", productionFromFood / 100 )
		end
	end
	tipText = GetYieldTooltip( city, YieldTypes.YIELD_PRODUCTION, city:GetBaseYieldRate( YieldTypes.YIELD_PRODUCTION ) + city:GetYieldPerPopTimes100( YieldTypes.YIELD_PRODUCTION ) * city:GetPopulation() / 100, productionPerTurn100 / 100, "[ICON_PRODUCTION]", strModifiersString ) .. "[NEWLINE][NEWLINE]" .. tipText

	-- Basic explanation of production
	if isNoob then
		return L"TXT_KEY_PRODUCTION_HELP_INFO" .. "[NEWLINE][NEWLINE]" .. tipText
	else
		return tipText
	end
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

-- CULTURE
local function GetCultureTooltip( city )

	local tips = table()
	local cityOwner = Players[city:GetOwner()]
	local culturePerTurn, cultureStored, cultureNeeded, cultureFromBuildings, cultureFromPolicies, cultureFromSpecialists, cultureFromTraits, baseCulturePerTurn
	-- Thanks fo Firaxis Cleverness...
	if civ5_mode then
		culturePerTurn = city:GetJONSCulturePerTurn()
		cultureStored = city:GetJONSCultureStored()
		cultureNeeded = city:GetJONSCultureThreshold()
		cultureFromBuildings = city:GetJONSCulturePerTurnFromBuildings()
		cultureFromPolicies = city:GetJONSCulturePerTurnFromPolicies()
		cultureFromSpecialists = (city:GetJONSCulturePerTurnFromSpecialists() + city:GetBaseYieldRateFromSpecialists(YieldTypes.YIELD_CULTURE))
		cultureFromTraits = city:GetJONSCulturePerTurnFromTraits()
		baseCulturePerTurn = city:GetBaseJONSCulturePerTurn()
		cultureFromPop = city:GetYieldPerPopTimes100(YieldTypes.YIELD_CULTURE) -- CBP
		if (cultureFromPop ~= 0) then
			cultureFromPop = cultureFromPop * city:GetPopulation();
			cultureFromPop = cultureFromPop / 100;
		end
		cultureFromPiety = city:GetReligionYieldRateModifier(YieldTypes.YIELD_CULTURE) -- CBP
		cultureFromCSAlliance = city:GetBaseYieldRateFromCSAlliance(YieldTypes.YIELD_CULTURE) -- CBP
		cultureFromTraits = (cultureFromTraits + city:GetYieldPerTurnFromTraits(YieldTypes.YIELD_CULTURE)) -- CBP
	else
		culturePerTurn = city:GetCulturePerTurn()
		cultureStored = city:GetCultureStored()
		cultureNeeded = city:GetCultureThreshold()
		cultureFromBuildings = city:GetCulturePerTurnFromBuildings()
		cultureFromPolicies = city:GetCulturePerTurnFromPolicies()
		cultureFromSpecialists = city:GetCulturePerTurnFromSpecialists()
		cultureFromTraits = city:GetCulturePerTurnFromTraits()
		baseCulturePerTurn = city:GetBaseCulturePerTurn()
	end

	if civBE_mode or not OptionsManager.IsNoBasicHelp() then
		tips:insertLocalized( "TXT_KEY_CULTURE_HELP_INFO" )
		tips:insert( "" )
	end

	-- Culture from Buildings
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_BUILDINGS", cultureFromBuildings )

	-- Culture from Policies
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_POLICIES", cultureFromPolicies )

	-- Culture from Specialists
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_SPECIALISTS", cultureFromSpecialists )
	
-- CBP -- Culture from Pop, Piety, and CS Alliance:
	-- Culture from Pop
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_POPULATION", cultureFromPop )
	
	-- Culture from Piety
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_PIETY", cultureFromPiety )
	
	-- Culture from Alliances
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_CS_ALLIANCE", cultureFromCSAlliance )
-- END CBP
	
	-- Culture from Religion
	if civ5gk_mode then
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_RELIGION", city:GetJONSCulturePerTurnFromReligion() )
	end

	if civ5bnw_mode then
		-- Culture from Great Works
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_GREAT_WORKS", city:GetJONSCulturePerTurnFromGreatWorks() )
		-- Culture from Leagues
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_LEAGUES", city:GetJONSCulturePerTurnFromLeagues() )
	end

	-- Culture from Terrain
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_TERRAIN", gk_mode and city:GetBaseYieldRateFromTerrain( YieldTypes.YIELD_CULTURE ) or city:GetJONSCulturePerTurnFromTerrain() )

	-- Culture from Traits
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_TRAITS", cultureFromTraits )

	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_CITY_YIELDS", city:GetYieldFromCityYield( YieldTypes.YIELD_CULTURE ) )

	-- CP EVENT
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_EVENTS", city:GetEventCityYield(YieldTypes.YIELD_CULTURE) )

	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_CORPORATIONS", city:GetYieldChangeFromCorporationFranchises(YieldTypes.YIELD_CULTURE))

	-- Base Yield from Misc
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_MISC", city:GetBaseYieldRateFromMisc(YieldTypes.YIELD_CULTURE), GameInfo.Yields[YieldTypes.YIELD_CULTURE].IconString)
	-- END
	
	-- Vox Populi Base Yield from Trade Routes
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_TRADE_ROUTES", city:GetBaseYieldRateFromTradeRoutes(YieldTypes.YIELD_CULTURE)/100.0, "[ICON_CULTURE]")
	
	-- Vox Populi Base Yield from Process
	tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_PROCESS", city:GetBaseYieldRateFromProcess(YieldTypes.YIELD_CULTURE), "[ICON_CULTURE]")
	
	-- Base Total
	if baseCulturePerTurn ~= culturePerTurn then
		tips:insert( "----------------" )
		tips:insertLocalized( "TXT_KEY_YIELD_BASE", baseCulturePerTurn, "[ICON_CULTURE]" )
		tips:insert( "" )
	end

	-- Empire Culture modifier
	-- tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_PLAYER_MOD", cityOwner and cityOwner:GetCultureCityModifier() or 0 ) -- Vox Populi

	local trculture = city:GetYieldModifierTooltip(YieldTypes.YIELD_CULTURE)
	if(trculture ~= "") then
		tips:append( L(trculture))
	end

	if civ5_mode then
		-- City Culture modifier
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_CITY_MOD", city:GetCultureRateModifier()) -- Vox Populi

		-- Culture Wonders modifier
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_WONDER_BONUS", city:GetNumWorldWonders() > 0 and cityOwner and cityOwner:GetCultureWonderMultiplier() or 0 ) -- Vox Populi
		
-- CBP
		-- WLTKD MOD
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_WLTKD", city:GetModFromWLTKD(YieldTypes.YIELD_CULTURE))

		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_WLTKD_TRAIT", city:GetCultureModFromCarnaval()) -- Vox Populi already handled by modifier tooltip

		-- Golden Age MOD
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_GOLDEN_AGE", city:GetModFromGoldenAge(YieldTypes.YIELD_CULTURE))
		
-- END

-- CBP -- Resource Monopoly
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_RESOURCE_MONOPOLY", city:GetCityYieldModFromMonopoly(YieldTypes.YIELD_CULTURE)) -- Vox Populi already handled by modifier tooltip 
		
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_CORPORATION", city:GetTradeRouteCityMod(YieldTypes.YIELD_CULTURE)) -- Vox Populi already handled by modifier tooltip

		
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_GWS", city:GetGreatWorkYieldMod(YieldTypes.YIELD_CULTURE)) -- Vox Populi

		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_CULTURE_FROM_SPIES", city:GetActiveSpyYieldMod(YieldTypes.YIELD_CULTURE)) -- Vox Populi
-- END
	end

	-- Puppet modifier
	if (city:IsPuppet() and culturePerTurn > 0) then
		local puppetMod = cityOwner:GetPuppetYieldPenalty(YieldTypes.YIELD_CULTURE)		
		if (puppetMod ~= 0) then
			tips:append( L( "TXT_KEY_PRODMOD_PUPPET", puppetMod ) )
		end
	end

	-- Puppet modifier
	-- Vox Populi
	--local puppetMod = city:IsPuppet() and GameDefines.PUPPET_CULTURE_MODIFIER or 0
	--if (puppetMod ~= 0 and not Players[city:GetOwner()]:IsIgnorePuppetPenalties()) then
	--	tips:append( L( "TXT_KEY_PRODMOD_PUPPET", puppetMod ) )
	--end
	-- Vox Populi end

	-- Total
	tips:insert( "----------------" )
	tips:insertLocalized( "TXT_KEY_YIELD_TOTAL", culturePerTurn, "[ICON_CULTURE]" )

	-- Tile growth
	tips:insert( "" )
	tips:insertLocalized( "TXT_KEY_CULTURE_INFO", "[COLOR_MAGENTA]" .. cultureStored .. "[ICON_CULTURE][ENDCOLOR]", "[COLOR_MAGENTA]" .. cultureNeeded .. "[ICON_CULTURE][ENDCOLOR]" )
	if culturePerTurn > 0 then
		local tipText = ""
		local turnsRemaining =  math_max(math_ceil((cultureNeeded - cultureStored ) / culturePerTurn), 1)
		local overflow = culturePerTurn * turnsRemaining + cultureStored - cultureNeeded
		if turnsRemaining > 1 then
			tipText = S( "%s %+g[ICON_CULTURE]  ", L( "TXT_KEY_STR_TURNS", turnsRemaining -1 ), overflow - culturePerTurn )
		end
		tips:insert( S( "%s[COLOR_MAGENTA]%s[ENDCOLOR] %+g[ICON_CULTURE]", tipText, Locale_ToUpper( L( "TXT_KEY_STR_TURNS", turnsRemaining ) ), overflow ) )
	end
	return tips:concat( "[NEWLINE]" )
end

-- CBP
local function GetCityHappinessTooltip(city)
	
	local strHappinessBreakdown = city:GetCityHappinessBreakdown();
	
	return strHappinessBreakdown;
end
local function GetCityUnhappinessTooltip(city)
	
	local strHappinessBreakdown = city:GetCityUnhappinessBreakdown(true);
	
	return strHappinessBreakdown;
end
-- END

-------------------------------------------------
-- Helper function to build religion tooltip string
-------------------------------------------------
local function GetReligionTooltip(city)

	if g_isReligionEnabled then

		local tips = table()
		local majorityReligionID = city:GetReligiousMajority()
		local pressureMultiplier = GameDefines.RELIGION_MISSIONARY_PRESSURE_MULTIPLIER or 1

		for religion in GameInfo.Religions() do
			local religionID = religion.ID

			if religionID >= 0 then
				local pressureLevel, numTradeRoutesAddingPressure = city:GetPressurePerTurn(religionID)
				local numFollowers = city:GetNumFollowers(religionID)
				local religion = GameInfo.Religions[religionID]
				local religionName = L( Game.GetReligionName(religionID) )
				local religionIcon = tostring(religion.IconString)

				if pressureLevel > 0 or numFollowers > 0 then

					local religionTip = ""
					if pressureLevel > 0 then
						religionTip = L( "TXT_KEY_RELIGIOUS_PRESSURE_STRING", math_floor(pressureLevel/pressureMultiplier))
					end

					if numTradeRoutesAddingPressure and numTradeRoutesAddingPressure > 0 then
						religionTip = L( "TXT_KEY_RELIGION_TOOLTIP_LINE_WITH_TRADE", religionIcon, numFollowers, religionTip, numTradeRoutesAddingPressure)
					else
						religionTip = L( "TXT_KEY_RELIGION_TOOLTIP_LINE", religionIcon, numFollowers, religionTip)
					end

					if religionID == majorityReligionID then
						local beliefs
						if religionID > 0 then
							beliefs = Game.GetBeliefsInReligion( religionID )
						else
							beliefs = {Players[city:GetOwner()]:GetBeliefInPantheon()}
						end
						for _,beliefID in pairs( beliefs or {} ) do
							if(GameInfo.Beliefs[beliefID].Pantheon and Game.IsBeliefValid(religionID, beliefID, city, false)) then
								religionTip = religionTip .. "[NEWLINE][ICON_BULLET] ".. L("TXT_KEY_RO_BELIEF_TYPE_PANTHEON") .. ": " .. L(GameInfo.Beliefs[ beliefID ].Description)
							elseif(GameInfo.Beliefs[beliefID].Founder and Game.IsBeliefValid(religionID, beliefID, city, true)) then
								religionTip = religionTip .. "[NEWLINE][ICON_BULLET] ".. L("TXT_KEY_RO_BELIEF_TYPE_FOUNDER") .. ": " .. L(GameInfo.Beliefs[ beliefID ].Description)
							elseif(GameInfo.Beliefs[beliefID].Follower and Game.IsBeliefValid(religionID, beliefID, city, false)) then
								religionTip = religionTip .. "[NEWLINE][ICON_BULLET] ".. L("TXT_KEY_RO_BELIEF_TYPE_FOLLOWER") .. ": " .. L(GameInfo.Beliefs[ beliefID ].Description)
							elseif(GameInfo.Beliefs[beliefID].Enhancer and Game.IsBeliefValid(religionID, beliefID, city, true)) then
								religionTip = religionTip .. "[NEWLINE][ICON_BULLET] ".. L("TXT_KEY_RO_BELIEF_TYPE_ENHANCER") .. ": " .. L(GameInfo.Beliefs[ beliefID ].Description)
							elseif(GameInfo.Beliefs[beliefID].Reformation and Game.IsBeliefValid(religionID, beliefID, city, true)) then
								religionTip = religionTip .. "[NEWLINE][ICON_BULLET] ".. L("TXT_KEY_RO_BELIEF_TYPE_REFORMATION") .. ": " .. L(GameInfo.Beliefs[ beliefID ].Description)
							end
						end
						tips:insert( 1, religionTip )
					else
						tips:insert( religionTip )
					end
				end

				if city:IsHolyCityForReligion( religionID ) then
					tips:insert( 1, L( "TXT_KEY_HOLY_CITY_TOOLTIP_LINE", religionIcon, religionName) )
				end
			end
		end
		return tips:concat( "[NEWLINE]" )
	else
		return L"TXT_KEY_TOP_PANEL_RELIGION_OFF_TOOLTIP"
	end
end

--------
-- FAITH
--------

local function GetFaithTooltip( city )

	if g_isReligionEnabled then
		local tips = table()

		if civBE_mode or not OptionsManager.IsNoBasicHelp() then
			tips:insertLocalized( "TXT_KEY_FAITH_HELP_INFO" )
			tips:insert( "" )
		end

		-- Faith from Buildings
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_BUILDINGS", city:GetFaithPerTurnFromBuildings() )

-- CBP
		-- Faith from Pop
		local iYieldPerPop = city:GetYieldPerPopTimes100(YieldTypes.YIELD_FAITH);
		if (iYieldPerPop ~= 0) then
			iYieldPerPop = iYieldPerPop * city:GetPopulation();
			iYieldPerPop = iYieldPerPop / 100;
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_POP", iYieldPerPop)
		end

		-- Faith from Specialists
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_YIELD_FROM_SPECIALISTS_FAITH", city:GetBaseYieldRateFromSpecialists(YieldTypes.YIELD_FAITH))
		
		-- Faith from Traits (EDITED BY CBP)
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_TRAITS", (city:GetFaithPerTurnFromTraits() + city:GetYieldPerTurnFromTraits( YieldTypes.YIELD_FAITH )))

		-- Yield Increase from Piety
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_PIETY", city:GetReligionYieldRateModifier(YieldTypes.YIELD_FAITH))
	
		-- Yield Increase from CS Alliance
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_CS_ALLIANCE", city:GetBaseYieldRateFromCSAlliance(YieldTypes.YIELD_FAITH))
	
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_WLTKD", city:GetModFromWLTKD(YieldTypes.YIELD_FAITH))
				
		--tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_GOLDEN_AGE", city:GetModFromGoldenAge(YieldTypes.YIELD_FAITH))
	
		-- Yield from Great Works
		tips:insertLocalizedBulletIfNonZero("TXT_KEY_YIELD_FROM_ART_CBP_FAITH", city:GetBaseYieldRateFromGreatWorks( YieldTypes.YIELD_FAITH ))

		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_RESOURCE_MONOPOLY", city:GetCityYieldModFromMonopoly(YieldTypes.YIELD_FAITH))
-- END
		-- Faith from Terrain
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_TERRAIN", city:GetBaseYieldRateFromTerrain( YieldTypes.YIELD_FAITH ) )

		-- Faith from Policies
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_POLICIES", city:GetFaithPerTurnFromPolicies() )

		-- Faith from Religion
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_RELIGION", city:GetFaithPerTurnFromReligion() )

		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_CITY_YIELDS", city:GetYieldFromCityYield( YieldTypes.YIELD_FAITH ) )

		--CP EVENTS
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_EVENTS", city:GetEventCityYield(YieldTypes.YIELD_FAITH) )

		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_CORPORATIONS", city:GetYieldChangeFromCorporationFranchises(YieldTypes.YIELD_FAITH))

		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_GWS", city:GetGreatWorkYieldMod(YieldTypes.YIELD_FAITH))
		tips:insertLocalizedBulletIfNonZero( "TXT_KEY_FAITH_FROM_GWS", city:GetActiveSpyYieldMod(YieldTypes.YIELD_FAITH))

		--END

		-- Puppet modifier
		if (city:IsPuppet()) then
			local puppetMod = Players[city:GetOwner()]:GetPuppetYieldPenalty(YieldTypes.YIELD_FAITH);
			tips:insertLocalizedBulletIfNonZero( "TXT_KEY_PRODMOD_PUPPET", puppetMod)
		end
		
		local trfaith = city:GetYieldModifierTooltip(YieldTypes.YIELD_FAITH)
		if(trfaith ~= "") then
			tips:append( L(trfaith))
		end

		-- Citizens breakdown
		tips:insert( "----------------")
		tips:insertLocalized( "TXT_KEY_YIELD_TOTAL", city:GetFaithPerTurn(), "[ICON_PEACE]" )
		tips:insert( "" )
		tips:insert( GetReligionTooltip( city ) )

		return tips:concat( "[NEWLINE]" )
	else
		return L"TXT_KEY_TOP_PANEL_RELIGION_OFF_TOOLTIP"
	end
end

-- TOURISM
local function GetTourismTooltip(city)
	return city:GetTourismTooltip()
end

----------------------------------------------------------------
-- MAJOR PLAYER MOOD INFO
----------------------------------------------------------------
local function addIfNz( v, icon )
	if v and v > 0 then
		return "+"..(v/100)..icon
	else
		return ""
	end
end
local function routeBonus( route, d )
	return addIfNz( route[d.."GPT"], g_currencyIcon )
		.. addIfNz( route[d.."Food"], "[ICON_FOOD]" )
		.. addIfNz( route[d.."Production"], "[ICON_PRODUCTION]" )
		.. addIfNz( route[d.."Science"], "[ICON_RESEARCH]" )
end
local function inParentheses( duration )
	if duration and duration >= 0 then
		return " (".. duration ..")"
	else
		return ""
	end
end

local g_relationshipDuration = gk_mode and GameInfo.GameSpeeds[ PreGame.GetGameSpeed() ].RelationshipDuration or 50

local function GetMoodInfo( playerID )

	local activePlayerID = Game.GetActivePlayer()
	local activePlayer = Players[activePlayerID]
	local activeTeamID = activePlayer:GetTeam()
	local activeTeam = Teams[activeTeamID]

	local player = Players[playerID]
	local teamID = player:GetTeam()
	local team = Teams[teamID]

	-- Player & civ names
	local strInfo = GetCivName(player) .. " (" .. GetLeaderName(player) .. ") "

	-- Always war ?
	if (playerID ~= activePlayerID) and (
		( activeTeam:IsAtWar( teamID )
		and Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE ) )
		or Game.IsOption(GameOptionTypes.GAMEOPTION_ALWAYS_WAR) )
	then
		return strInfo .. L"TXT_KEY_ALWAYS_WAR_TT"
	end

	-- Era
	strInfo = strInfo .. L( ( GameInfo.Eras[ player:GetCurrentEra() ] or {} ).Description )

	-- Tech in Progress
	if teamID == activeTeamID then
		local currentTechID = player:GetCurrentResearch()
		local currentTech = GameInfo.Technologies[currentTechID]
		if currentTech and g_isScienceEnabled then
			strInfo = strInfo .. " [ICON_RESEARCH] " .. L(currentTech.Description)
		end
	else
	-- Mood
		local visibleApproachID = activePlayer:GetApproachTowardsUsGuess(playerID)
		strInfo = strInfo .. " " .. (
		-- At war
		((team:IsAtWar( activeTeamID ) or visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR) and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR")
		-- Denouncing
		or (player:IsDenouncingPlayer( activePlayerID ) and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_DENOUNCING")
		-- Resurrected
		or (gk_mode and player:WasResurrectedThisTurnBy( activePlayerID ) and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_LIBERATED")
		-- Appears Hostile
		or (visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_HOSTILE")
		-- Appears Guarded
		or (visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_GUARDED")
		-- Appears Afraid
		or (visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_AFRAID")
		-- Appears Friendly
		or (visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY and L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_FRIENDLY")
		-- Neutral - default string
		or L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_NEUTRAL" ) .. "[/COLOR]"
	end

	-- Techs Known
	local tips = table( team:GetTeamTechs():GetNumTechsKnown() .. " " .. TechColor( Locale_ToLower("TXT_KEY_VP_TECH") ) )
	-- Policies
	for policyBranch in GameInfo.PolicyBranchTypes() do
		local policyCount = 0

		for policy in GameInfo.Policies() do
			if policy.PolicyBranchType == policyBranch.Type and player:HasPolicy(policy.ID) then
				policyCount = policyCount + 1
			end
		end
		-- Add here for opener
		if(policyCount > 0) then
			policyCount = policyCount + 1
		end

		tips:insertIf( policyCount > 0 and policyCount .. " " .. PolicyColor( Locale_ToLower(policyBranch.Description) ) )
	end
	-- Religion Founded
	if g_isReligionEnabled then
		local religionID = player:GetReligionCreatedByPlayer()
		tips:insertIf( religionID > 0 and tostring( (GameInfo.Religions[ religionID ] or {}).IconString )..BeliefColor( L("TXT_KEY_RO_STATUS_FOUNDER", Game.GetReligionName( religionID ) ) ) )
	end
	tips = table( strInfo, tips:concat(", ") )

	-- Wonders
	local wonders = table()
	for building in GameInfo.Buildings() do
		if building.BuildingClass
			and ( ( GameInfo.BuildingClasses[building.BuildingClass] or {} ).MaxGlobalInstances or 0 ) > 0
			and player:CountNumBuildings(building.ID) > 0
		then
			wonders:insert( BuildingColor( L(building.Description) ) )
		end
	end
	-- Population
	tips:insert( player:GetTotalPopulation() .. "[ICON_CITIZEN]"
			.. ", "
			.. player:GetNumCities() .. " " .. Locale_ToLower("TXT_KEY_VP_CITIES")
			.. ", "
			.. player:GetNumWorldWonders() .. " " .. Locale_ToLower("TXT_KEY_VP_WONDERS")
			.. (#wonders>0 and ": " .. wonders:concat( ", " ) or "") )
--[[ too much info
	local cities = table()
	for city in player:Cities() do
		-- Name & population
		local cityTxt = S("%i[ICON_CITIZEN] %s", city:GetPopulation(), city:GetName())
		if city:IsCapital() then
			cityTxt = "[ICON_CAPITAL]" .. cityTxt
		end
		-- Wonders
		local wonders = table()
		for building in GameInfo.Buildings() do
			if building.BuildingClass
				and GameInfo.BuildingClasses[building.BuildingClass].MaxGlobalInstances > 0
				and city:IsHasBuilding(building.ID)
			then
				wonders:insert( L(building.Description) )
			end
		end
		if #wonders > 0 then
			cityTxt = cityTxt .. " (" .. wonders:concat(", ") .. ")"
		end
		cities:insert( cityTxt )
	end
	if #cities > 1 then
		tips:insert( #cities .. " " .. L("TXT_KEY_DIPLO_CITIES"):lower() .. " : " .. cities:concat(", ") )
	elseif #cities ==1 then
		tips:insert( cities[1] )
	end
--]]

	-- Gold (can be seen in diplo relation ship)
	tips:insert( S( "%i%s(%+i)", player:GetGold(), g_currencyIcon, player:CalculateGoldRate() ) )


	--------------------------------------------------------------------
	-- Loop through the active player's current deals
	--------------------------------------------------------------------

	local isTradeable, isActiveDeal
	local dealsFinalTurn = {}
	local deals = table()
	local tradeRoutes = table()
	local opinions = table()
	local treaties = table()
	local currentTurn = Game.GetGameTurn() -1
	local isUs = playerID == activePlayerID

	local function GetDealTurnsRemaining( itemID )
		local turnsRemaining
		if bnw_mode and itemID == TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP then -- DoF special kinky case
			turnsRemaining = g_relationshipDuration - activePlayer:GetDoFCounter( playerID )
		elseif itemID then
			local finalTurn = dealsFinalTurn[ itemID ]
			if finalTurn then
				turnsRemaining = finalTurn - currentTurn
			end
		end
		return inParentheses( isActiveDeal and turnsRemaining )
	end

	local dealItems = {}
	local finalTurns = table()
	PushScratchDeal()
	for i = 0, UI_GetNumCurrentDeals( activePlayerID ) - 1 do
		UI_LoadCurrentDeal( activePlayerID, i )
		local toPlayerID = g_deal:GetOtherPlayer( activePlayerID )
		g_deal:ResetIterator()
		repeat
			local itemID, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID = g_deal:GetNextItem()
			if not bnw_mode then
				fromPlayerID = data3
			end
			if itemID then
				if isUs or toPlayerID == playerID or fromPlayerID == playerID then
--					finalTurn = finalTurn - currentTurn
					local isFromUs = fromPlayerID == activePlayerID
					local dealItem = dealItems[ finalTurn ]
					if not dealItem then
						dealItem = {}
						dealItems[ finalTurn ] = dealItem
						finalTurns:insert( finalTurn )
					end
					if itemID == TradeableItems.TRADE_ITEM_GOLD_PER_TURN then
						dealItem.GPT = (dealItem.GPT or 0) + (isFromUs and -data1 or data1)
					elseif itemID == TradeableItems.TRADE_ITEM_RESOURCES then
						dealItem[data1] = (dealItem[data1] or 0) + (isFromUs and -data2 or data2)
					else
						dealsFinalTurn[ itemID + (isFromUs and 65536 or 0) ] = finalTurn
					end
				end
			else
				break
			end
		until false
	end
	PopScratchDeal()
	finalTurns:sort()
	for i = 1, #finalTurns do
		local finalTurn = finalTurns[i]
		local dealItem = dealItems[ finalTurn ] or {}
		local deal = table()
		local quantity = dealItem.GPT
		deal:insertIf( quantity and quantity ~= 0 and S( "%+g%s", quantity, g_currencyIcon ) )
		for resource in GameInfo.Resources() do
			local quantity = dealItem[ resource.ID ]
			deal:insertIf( (quantity or 0) ~= 0 and S( "%+g%s", quantity, tostring(resource.IconString) ) )
		end
		deals:insertIf( #deal > 0 and deal:concat() .. "("..( finalTurn - currentTurn )..")" )
	end

	
	--if bnw_mode then
	--	for _, route in ipairs( activePlayer:GetTradeRoutes() ) do
	--		if route.ToID == playerID then
		--		local tip = "   [ICON_INTERNATIONAL_TRADE]" .. route.FromCityName .. " "
			--			.. routeBonus( route, "From" )
				--		.. "[ICON_MOVES]" .. route.ToCityName
			--	if route.ToID == activePlayerID then
			--		tip = tip .. " ".. routeBonus( route, "To" )
			--	end
			--	tradeRoutes:insert( tip .. " ("..(route.TurnsLeft-1)..")" )
			--end
		--end
		--for _, route in ipairs( activePlayer:GetTradeRoutesToYou() ) do
			--if isUs or route.FromID == playerID then
				--tradeRoutes:insert( "   [ICON_INTERNATIONAL_TRADE]" .. route.FromCityName .. "[ICON_MOVES]" .. route.ToCityName .. " "
					--	.. routeBonus( route, "To" )
					--	)
			--end
		--end
	--end

	if isUs then

		-- Resources available for trade
		local luxuries = table()
		local strategic = table()
		for resource in GameInfo.Resources() do
			local i = activePlayer:GetNumResourceAvailable( resource.ID, false )
			if i > 0 then
				if resource.ResourceClassType == "RESOURCECLASS_LUXURY" then
					luxuries:insert( " " .. activePlayer:GetNumResourceAvailable( resource.ID, true ) .. tostring(resource.IconString) )
				elseif resource.ResourceClassType ~= "RESOURCECLASS_BONUS" then
					strategic:insert( " " .. i .. tostring(resource.IconString) )
				end
			end
		end
		tips:append( luxuries:concat() .. strategic:concat() )

	else  --if teamID ~= activeTeamID then

		local visibleApproachID = activePlayer:GetApproachTowardsUsGuess(playerID)

		if activeTeam:IsAtWar( teamID ) then	-- At war right now

			opinions:insert( L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" )

		else	-- Not at war right now

			-- Resources available from them
			local luxuries = table()
			local strategic = table()
			for resource in GameInfo.Resources() do
				if g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_RESOURCES, resource.ID, 1 ) then	-- 1 here is 1 quantity of the Resource, which is the minimum possible
					local resource = "  " .. player:GetNumResourceAvailable( resource.ID, false ) .. tostring(resource.IconString)
					if resource.ResourceClassType == "RESOURCECLASS_LUXURY" then
						luxuries:insert( resource )
					else
						strategic:insert( resource )
					end
				end
			end
			tips:append( luxuries:concat() .. strategic:concat() )

			-- Resources they would like from us
			luxuries = table()
			strategic = table()
			for resource in GameInfo.Resources() do
				if g_deal:IsPossibleToTradeItem( activePlayerID, playerID, TradeableItems.TRADE_ITEM_RESOURCES, resource.ID, 1 ) then	-- 1 here is 1 quantity of the Resource, which is the minimum possible
					if resource.ResourceClassType == "RESOURCECLASS_LUXURY" then
						luxuries:insert( " " .. activePlayer:GetNumResourceAvailable( resource.ID, true ) .. tostring(resource.IconString) )
					else
						strategic:insert( " " .. activePlayer:GetNumResourceAvailable( resource.ID, false ) .. tostring(resource.IconString) )
					end
				end
			end
			if #luxuries > 0 or #strategic > 0 then
				tips:insert( L"TXT_KEY_DIPLO_YOUR_ITEMS_LABEL" .. ": " .. luxuries:concat() .. strategic:concat() )
			end

			-- Treaties
			local peaceTurnExpire = dealsFinalTurn[ TradeableItems.TRADE_ITEM_PEACE_TREATY ]
			if peaceTurnExpire and peaceTurnExpire > currentTurn then
				treaties:insert( "[ICON_PEACE]" .. L( "TXT_KEY_DIPLO_PEACE_TREATY", peaceTurnExpire - currentTurn ) )
			end
			if gk_mode then

				-- Embassy to them
				isTradeable = g_deal:IsPossibleToTradeItem( activePlayerID, playerID, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_dealDuration )
				isActiveDeal = team:HasEmbassyAtTeam( activeTeamID )

				if isTradeable or isActiveDeal then
					treaties:insert( negativeOrPositiveTextColor[isActiveDeal] .. "[ICON_CAPITAL]"
							.. L("TXT_KEY_DIPLO_ALLOW_EMBASSY"):lower()
							.. "[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY + 65536 ) -- 65536 means from us
					)
				end

				-- Embassy from them
				isTradeable = g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_dealDuration )
				isActiveDeal = activeTeam:HasEmbassyAtTeam( teamID )

				if isTradeable or isActiveDeal then
					treaties:insert( negativeOrPositiveTextColor[isActiveDeal]
							.. L("TXT_KEY_DIPLO_ALLOW_EMBASSY"):lower()
							.. "[ICON_CAPITAL][ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY )
					)
				end
			end

			-- Open Borders to them
			isTradeable = g_deal:IsPossibleToTradeItem( activePlayerID, playerID, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_dealDuration )
			isActiveDeal = activeTeam:IsAllowsOpenBordersToTeam(teamID)

			if isTradeable or isActiveDeal then
				treaties:insert( negativeOrPositiveTextColor[isActiveDeal] .. "<"
						.. L"TXT_KEY_DO_OPEN_BORDERS"
						.. "[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_OPEN_BORDERS + 65536 ) -- 65536 means from us
				)
			end

			-- Open Borders from them
			isTradeable = g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_dealDuration )
			isActiveDeal = team:IsAllowsOpenBordersToTeam( activeTeamID )

			if isTradeable or isActiveDeal then
				treaties:insert( negativeOrPositiveTextColor[isActiveDeal]
						.. L"TXT_KEY_DO_OPEN_BORDERS"
						.. ">[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_OPEN_BORDERS )
				)
			end

			-- Declaration of Friendship
			isTradeable = not gk_mode or g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_dealDuration )
			isActiveDeal = activePlayer:IsDoF(playerID)
			if isTradeable or isActiveDeal then
				treaties:insert( negativeOrPositiveTextColor[isActiveDeal] .. "[ICON_FLOWER]"
						.. L"TXT_KEY_DIPLOMACY_FRIENDSHIP_ADV_QUEST"
						.. "[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP )
				)
			end

			-- Research Agreement
--			isTradeable = (activeTeam:IsResearchAgreementTradingAllowed() or team:IsResearchAgreementTradingAllowed())
--				and not activeTeam:GetTeamTechs():HasResearchedAllTechs() and not team:GetTeamTechs():HasResearchedAllTechs()
--				and not g_isScienceEnabled
			isTradeable = g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_dealDuration )
			isActiveDeal = activeTeam:IsHasResearchAgreement(teamID)
			if isTradeable or isActiveDeal then
				treaties:insert( negativeOrPositiveTextColor[isActiveDeal] .. "[ICON_RESEARCH]"
						.. L"TXT_KEY_DO_RESEARCH_AGREEMENT"
						.. "[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT )
				)
			end

			-- Trade Agreement
			isTradeable = g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_dealDuration )
			isActiveDeal = activeTeam:IsHasTradeAgreement(teamID)
			if isTradeable or isActiveDeal then
				treaties:insert( negativeOrPositiveTextColor[isActiveDeal] .. "[ICON_RESEARCH]"
						.. L("TXT_KEY_DIPLO_TRADE_AGREEMENT"):lower()
						.. "[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT )
				)
			end

			-- Defensive Pact
			isTradeable = g_deal:IsPossibleToTradeItem( playerID, activePlayerID, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_dealDuration )
			isActiveDeal = activeTeam:IsDefensivePact(teamID)
			if isTradeable or isActiveDeal then
				treaties:insert( negativeOrPositiveTextColor[isActiveDeal] .. "[ICON_STRENGTH]"
						.. L"TXT_KEY_DO_PACT"
						.. "[ENDCOLOR]" .. GetDealTurnsRemaining( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT )
				)
			end

			-- We've fought before
			if not gk_mode and activePlayer:GetNumWarsFought(playerID) > 0 then
				-- They don't appear to be mad
				if visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY or
					visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL then
					opinions:insert( L"TXT_KEY_DIPLO_PAST_WAR_NEUTRAL" )
				-- They aren't happy with us
				else
					opinions:insert( L"TXT_KEY_DIPLO_PAST_WAR_BAD" )
				end
			end
		end

		if player.GetOpinionTable then
			opinions = player:GetOpinionTable( activePlayerID )
		else

			-- Good things
			opinions:insertLocalizedIf( activePlayer:IsDoF(playerID) and "TXT_KEY_DIPLO_DOF" )
			opinions:insertLocalizedIf( activePlayer:IsPlayerDoFWithAnyFriend(playerID) and "TXT_KEY_DIPLO_MUTUAL_DOF" ) -- Human has a mutual friend with the AI
			opinions:insertLocalizedIf( activePlayer:IsPlayerDenouncedEnemy(playerID) and "TXT_KEY_DIPLO_MUTUAL_ENEMY" ) -- Human has denounced an enemy of the AI
			opinions:insertLocalizedIf( player:GetNumCiviliansReturnedToMe(activePlayerID) > 0 and "TXT_KEY_DIPLO_CIVILIANS_RETURNED" )

			-- Neutral things
			opinions:insertLocalizedIf( visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID and "TXT_KEY_DIPLO_AFRAID" )

			-- Bad things
			opinions:insertLocalizedIf( player:IsFriendDeclaredWarOnUs(activePlayerID) and "TXT_KEY_DIPLO_HUMAN_FRIEND_DECLARED_WAR" ) -- Human was a friend and declared war on us
			opinions:insertLocalizedIf( player:IsFriendDenouncedUs(activePlayerID) and "TXT_KEY_DIPLO_HUMAN_FRIEND_DENOUNCED" ) -- Human was a friend and denounced us
			opinions:insertLocalizedIf( activePlayer:GetWeDeclaredWarOnFriendCount() > 0 and "TXT_KEY_DIPLO_HUMAN_DECLARED_WAR_ON_FRIENDS" ) -- Human declared war on friends
			opinions:insertLocalizedIf( activePlayer:GetWeDenouncedFriendCount() > 0 and "TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIENDS" ) -- Human has denounced his friends
			opinions:insertLocalizedIf( activePlayer:GetNumFriendsDenouncedBy() > 0 and "TXT_KEY_DIPLO_HUMAN_DENOUNCED_BY_FRIENDS" ) -- Human has been denounced by friends
			opinions:insertLocalizedIf( activePlayer:IsDenouncedPlayer(playerID) and "TXT_KEY_DIPLO_DENOUNCED_BY_US" )
			opinions:insertLocalizedIf( player:IsDenouncedPlayer(activePlayerID) and "TXT_KEY_DIPLO_DENOUNCED_BY_THEM" )
			opinions:insertLocalizedIf( player:IsPlayerDoFWithAnyEnemy(activePlayerID) and "TXT_KEY_DIPLO_HUMAN_DOF_WITH_ENEMY" )
			opinions:insertLocalizedIf( player:IsPlayerDenouncedFriend(activePlayerID) and "TXT_KEY_DIPLO_HUMAN_DENOUNCED_FRIEND" )
			opinions:insertLocalizedIf( player:IsPlayerNoSettleRequestEverAsked(activePlayerID) and "TXT_KEY_DIPLO_NO_SETTLE_ASKED" )
			opinions:insertLocalizedIf( player:IsDemandEverMade(activePlayerID) and "TXT_KEY_DIPLO_TRADE_DEMAND" )
			opinions:insertLocalizedIf( player:GetNumTimesCultureBombed(activePlayerID) > 0 and "TXT_KEY_DIPLO_CULTURE_BOMB" )
			opinions:insertLocalizedIf( player:IsPlayerBrokenMilitaryPromise(activePlayerID) and "TXT_KEY_DIPLO_MILITARY_PROMISE" )
			opinions:insertLocalizedIf( player:IsPlayerIgnoredMilitaryPromise(activePlayerID) and "TXT_KEY_DIPLO_MILITARY_PROMISE_IGNORED" )
			opinions:insertLocalizedIf( player:IsPlayerBrokenExpansionPromise(activePlayerID) and "TXT_KEY_DIPLO_EXPANSION_PROMISE" )
			opinions:insertLocalizedIf( player:IsPlayerIgnoredExpansionPromise(activePlayerID) and "TXT_KEY_DIPLO_EXPANSION_PROMISE_IGNORED" )
			opinions:insertLocalizedIf( player:IsPlayerBrokenBorderPromise(activePlayerID) and "TXT_KEY_DIPLO_BORDER_PROMISE" )
			opinions:insertLocalizedIf( player:IsPlayerIgnoredBorderPromise(activePlayerID) and "TXT_KEY_DIPLO_BORDER_PROMISE_IGNORED" )
			opinions:insertLocalizedIf( player:IsPlayerBrokenCityStatePromise(activePlayerID) and "TXT_KEY_DIPLO_CITY_STATE_PROMISE" )
			opinions:insertLocalizedIf( player:IsPlayerIgnoredCityStatePromise(activePlayerID) and "TXT_KEY_DIPLO_CITY_STATE_PROMISE_IGNORED" )
			opinions:insertLocalizedIf( player:IsPlayerBrokenCoopWarPromise(activePlayerID) and "TXT_KEY_DIPLO_COOP_WAR_PROMISE" )
			opinions:insertLocalizedIf( player:IsPlayerRecklessExpander(activePlayerID) and "TXT_KEY_DIPLO_RECKLESS_EXPANDER" )
			opinions:insertLocalizedIf( player:GetNumRequestsRefused(activePlayerID) > 0 and "TXT_KEY_DIPLO_REFUSED_REQUESTS" )
			opinions:insertLocalizedIf( player:GetRecentTradeValue(activePlayerID) > 0 and "TXT_KEY_DIPLO_TRADE_PARTNER" )
			opinions:insertLocalizedIf( player:GetCommonFoeValue(activePlayerID) > 0 and "TXT_KEY_DIPLO_COMMON_FOE" )
			opinions:insertLocalizedIf( player:GetRecentAssistValue(activePlayerID) > 0 and "TXT_KEY_DIPLO_ASSISTANCE_TO_THEM" )
			opinions:insertLocalizedIf( player:IsLiberatedCapital(activePlayerID) and "TXT_KEY_DIPLO_LIBERATED_CAPITAL" )
			opinions:insertLocalizedIf( player:IsLiberatedCity(activePlayerID) and "TXT_KEY_DIPLO_LIBERATED_CITY" )
			opinions:insertLocalizedIf( player:IsGaveAssistanceTo(activePlayerID) and "TXT_KEY_DIPLO_ASSISTANCE_FROM_THEM" )
			opinions:insertLocalizedIf( player:IsHasPaidTributeTo(activePlayerID) and "TXT_KEY_DIPLO_PAID_TRIBUTE" )
			opinions:insertLocalizedIf( player:IsNukedBy(activePlayerID) and "TXT_KEY_DIPLO_NUKED" )
			opinions:insertLocalizedIf( player:IsCapitalCapturedBy(activePlayerID) and "TXT_KEY_DIPLO_CAPTURED_CAPITAL" )
			-- Protected Minors
			if player:GetOtherPlayerNumProtectedMinorsKilled(activePlayerID) > 0 then
				opinions:insert( L"TXT_KEY_DIPLO_PROTECTED_MINORS_KILLED" )
			-- Only worry about protected minors ATTACKED if they haven't KILLED any
			elseif player:GetOtherPlayerNumProtectedMinorsAttacked(activePlayerID) > 0 then
				opinions:insert( L"TXT_KEY_DIPLO_PROTECTED_MINORS_ATTACKED" )
			end

			--local actualApproachID = player:GetMajorCivApproach(activePlayerID)

			-- Bad things we don't want visible if someone is friendly (acting or truthfully)
			if visibleApproachID ~= MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY then
				-- and actualApproachID ~= MajorCivApproachTypes.MAJOR_CIV_APPROACH_DECEPTIVE then
				opinions:insertLocalizedIf( player:GetLandDisputeLevel(activePlayerID) > DisputeLevelTypes.DISPUTE_LEVEL_NONE and "TXT_KEY_DIPLO_LAND_DISPUTE" )
				--opinions:insertLocalizedIf( player:GetVictoryDisputeLevel(activePlayerID) > DisputeLevelTypes.DISPUTE_LEVEL_NONE and "TXT_KEY_DIPLO_VICTORY_DISPUTE" )
				opinions:insertLocalizedIf( player:GetWonderDisputeLevel(activePlayerID) > DisputeLevelTypes.DISPUTE_LEVEL_NONE and "TXT_KEY_DIPLO_WONDER_DISPUTE" )
				opinions:insertLocalizedIf( player:GetMinorCivDisputeLevel(activePlayerID) > DisputeLevelTypes.DISPUTE_LEVEL_NONE and "TXT_KEY_DIPLO_MINOR_CIV_DISPUTE" )
				opinions:insertLocalizedIf( player:GetWarmongerThreat(activePlayerID) > ThreatTypes.THREAT_NONE and "TXT_KEY_DIPLO_WARMONGER_THREAT" )
			end
		end

		--  No specific events - let's see what string we should use
		if #opinions == 0 then
			-- At war
			if visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR then
				opinions = { L"TXT_KEY_DIPLO_AT_WAR" }
			-- Appears Friendly
			elseif visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY then
				opinions = { L"TXT_KEY_DIPLO_FRIENDLY" }
			-- Appears Guarded
			elseif visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED then
				opinions = { L"TXT_KEY_DIPLO_GUARDED" }
			-- Appears Hostile
			elseif visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE then
				opinions = { L"TXT_KEY_DIPLO_HOSTILE" }
			-- Appears Afraid
			elseif visibleApproachID == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID  then
				opinions = { L"TXT_KEY_DIPLO_AFRAID" }
			-- Neutral - default string
			else
				opinions = { L"TXT_KEY_DIPLO_DEFAULT_STATUS" }
			end
		end

	end -- playerID vs activePlayerID

--TODO "TXT_KEY_DO_WE_PROVIDE" & "TXT_KEY_DO_THEY_PROVIDE"
	tips:insertIf( #deals > 0 and L"TXT_KEY_DO_CURRENT_DEALS" .. "[NEWLINE]" .. deals:concat( ", " ) )
	tips:insertIf( #tradeRoutes > 0 and tradeRoutes:concat( "[NEWLINE]" ) )
	tips:insertIf( #treaties > 0 and treaties:concat( ", " ) .. "[ENDCOLOR]" )
	tips:insertIf( #opinions > 0 and "[ICON_BULLET]" .. table_concat( opinions, "[NEWLINE][ICON_BULLET]" ) .. "[ENDCOLOR]" )

	local allied = table()
	local friends = table()
	local protected = table()
	local pacts = table()
	--CBP
	local marriage = table()
	--END
	--C4df
	local master = table()
	local vassal = table()
	--END
	local denouncements = table()
	local backstabs = table()
	local denouncedBy = table()
	local wars = table()

	-- Relationships with others
	for otherPlayerID = 0, GameDefines.MAX_CIV_PLAYERS - 1 do
		local otherPlayer = Players[otherPlayerID]
		if otherPlayer
			and otherPlayerID ~= playerID
			and otherPlayer:IsAlive()
			and activeTeam:IsHasMet(otherPlayer:GetTeam())
		then
			local otherPlayerName =  GetCivName(otherPlayer)
			-- Wars
			if team:IsAtWar(otherPlayer:GetTeam()) then			
				if not otherPlayer:IsMinorCiv() then
					local iWarScore = player:GetWarScore(otherPlayerID);
					wars:insert(otherPlayerName .. " (" .. L"TXT_KEY_DIPLO_WARSCORE_CP" .. " " .. iWarScore .. ")")
				else
					wars:insert( otherPlayerName)

				end
			end
			if otherPlayer:IsMinorCiv() then
				
				if otherPlayer:IsMarried(playerID) then
					marriage:insert( otherPlayerName )
				end
				-- Alliances
				if otherPlayer:IsAllies(playerID) then
					allied:insert( otherPlayerName )
				-- Friendships
				elseif otherPlayer:IsFriends(playerID) then
					friends:insert( otherPlayerName )
				end
				-- Protections
				if player:IsProtectingMinor(otherPlayerID) then
-- CBP
					protected:insert( otherPlayerName .. inParentheses( gk_mode and isUs and ( otherPlayer:GetTurnLastPledgedProtectionByMajor(playerID) - Game.GetGameTurn() + GameDefines.BALANCE_MINOR_PROTECTION_MINIMUM_DURATION ) ) )  -- todo check scaling % game speed
-- END
					--protected:insert( otherPlayerName .. inParentheses( gk_mode and isUs and ( otherPlayer:GetTurnLastPledgedProtectionByMajor(playerID) - Game.GetGameTurn() + 10 ) ) )  -- todo check scaling % game speed
				end
			else
				-- Friendships
				if player:IsDoF(otherPlayerID) then
					friends:insert( otherPlayerName .. inParentheses( bnw_mode and ( g_relationshipDuration - player:GetDoFCounter( otherPlayerID ) ) ) )
				end
				-- Backstab
				if otherPlayer:IsFriendDenouncedUs(playerID) or otherPlayer:IsFriendDeclaredWarOnUs(playerID) then
					backstabs:insert( otherPlayerName )
				-- Denouncement
				elseif player:IsDenouncedPlayer(otherPlayerID) then
					denouncements:insert( otherPlayerName .. inParentheses( bnw_mode and ( g_relationshipDuration - player:GetDenouncedPlayerCounter( otherPlayerID ) ) ) )
				end
				-- Denounced by 3rd party
				if otherPlayer:IsDenouncedPlayer(playerID) then
					denouncedBy:insert( otherPlayerName .. inParentheses( bnw_mode and ( g_relationshipDuration - otherPlayer:GetDenouncedPlayerCounter( playerID ) ) ) )
				end

				if player:IsHasDefensivePactWithPlayer(otherPlayerID) then
					pacts:insert( otherPlayerName )
				end

				--C4df
				if team:IsVassal(otherPlayer:GetTeam()) then
					vassal:insert( otherPlayerName )
				end
				if Teams[otherPlayer:GetTeam()]:IsVassal(player:GetTeam()) then
					master:insert( otherPlayerName )
				end
				--END
			end
		end
	end

	tips:insertIf( #allied > 0 and "[ICON_CITY_STATE]" .. L( "TXT_KEY_ALLIED_WITH", allied:concat(", ") ) )
	tips:insertIf( #friends > 0 and "[ICON_FLOWER]" .. L( "TXT_KEY_DIPLO_FRIENDS_WITH", friends:concat(", ") ) )
	tips:insertIf( #protected > 0 and "[ICON_STRENGTH]" .. L"TXT_KEY_POP_CSTATE_PLEDGE_TO_PROTECT" .. ": " .. protected:concat(", ") )
	tips:insertIf( #pacts > 0 and "[ICON_VICTORY_DOMINATION]" .. L"TXT_KEY_CP_DIPLO_DP" .. ": " .. pacts:concat(", ") )
	tips:insertIf( #marriage > 0 and "[ICON_CITY_STATE]" .. L"TXT_KEY_CP_DIPLO_MARRIAGE" .. ": " .. marriage:concat(", ") )
	--C4DF
	tips:insertIf( #master > 0 and "[ICON_PUPPET]" .. L"TXT_KEY_CP_DIPLO_MASTER_OF" .. ": " .. master:concat(", ") )
	tips:insertIf( #vassal > 0 and "[ICON_OCCUPIED]" .. L"TXT_KEY_CP_DIPLO_VASSAL_OF" .. ": " .. vassal:concat(", ") )
	--END
	tips:insertIf( #backstabs > 0 and "[ICON_PIRATE]" .. L( "TXT_KEY_DIPLO_BACKSTABBED", backstabs:concat(", ") ) )
	tips:insertIf( #denouncements > 0 and "[ICON_DENOUNCE]" .. L( "TXT_KEY_DIPLO_DENOUNCED", denouncements:concat(", ") ) )
	tips:insertIf( #denouncedBy > 0 and "[ICON_DENOUNCE]" .. TextColor( negativeOrPositiveTextColor[false], L( "TXT_KEY_NTFN_DENOUNCED_US_S", denouncedBy:concat(", ") ) ) )
	tips:insertIf( #wars > 0 and "[ICON_WAR]" .. L( "TXT_KEY_AT_WAR_WITH_CP", wars:concat(", ") ) )

	return tips:concat( "[NEWLINE]" )
end

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
function TT.GetHelpTextForUnit( ... ) return select(2, pcall( GetHelpTextForUnit, ... ) ) end
function TT.GetHelpTextForBuilding( ... ) return select(2, pcall( GetHelpTextForBuilding, ... ) ) end
function TT.GetHelpTextForImprovement( ... ) return select(2, pcall( GetHelpTextForImprovement, ... ) ) end
function TT.GetHelpTextForProject( ... ) return select(2, pcall( GetHelpTextForProject, ... ) ) end
function TT.GetHelpTextForProcess( ... ) return select(2, pcall( GetHelpTextForProcess, ... ) ) end
function TT.GetFoodTooltip( ... ) return select(2, pcall( GetFoodTooltip, ... ) ) end
function TT.GetGoldTooltip( ... ) return select(2, pcall( GetGoldTooltip, ... ) ) end
function TT.GetEnergyTooltip( ... ) return select(2, pcall( GetEnergyTooltip, ... ) ) end
function TT.GetScienceTooltip( ... ) return select(2, pcall( GetScienceTooltip, ... ) ) end
function TT.GetProductionTooltip( ... ) return select(2, pcall( GetProductionTooltip, ... ) ) end
function TT.GetCultureTooltip( ... ) return select(2, pcall( GetCultureTooltip, ... ) ) end
function TT.GetReligionTooltip( ... ) return select(2, pcall( GetReligionTooltip, ... ) ) end
function TT.GetFaithTooltip( ... ) return select(2, pcall( GetFaithTooltip, ... ) ) end
function TT.GetTourismTooltip( ... ) return select(2, pcall( GetTourismTooltip, ... ) ) end
function TT.GetYieldTooltipHelper( ... ) return select(2, pcall( GetYieldTooltipHelper, ... ) ) end
function TT.GetYieldTooltip( ... ) return select(2, pcall( GetYieldTooltip, ... ) ) end
function TT.GetMoodInfo( ... ) return select(2, pcall( GetMoodInfo, ... ) ) end
--CBP
function TT.GetHelpTextForCorp( ... ) return select(2, pcall( GetHelpTextForCorp, ... ) ) end
function TT.GetCityHappinessTooltip( ... ) return select(2, pcall( GetCityHappinessTooltip, ... ) ) end
function TT.GetCityUnhappinessTooltip( ... ) return select(2, pcall( GetCityUnhappinessTooltip, ... ) ) end
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
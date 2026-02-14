print("This is the modded InfoTooltipInclude from Community Patch");

include("VPUI_core");
include("CPK.lua");

-- GameDefines is not available in PreGame, so this will have to do
local CITY_RESOURCE_WLTKD_TURNS = GameInfo.Defines{Name = "CITY_RESOURCE_WLTKD_TURNS"}().Value;
local GOLDEN_AGE_LENGTH = GameInfo.Defines{Name = "GOLDEN_AGE_LENGTH"}().Value;
local PILLAGE_HEAL_AMOUNT = GameInfo.Defines{Name = "PILLAGE_HEAL_AMOUNT"}().Value;
local ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER = GameInfo.Defines{Name = "ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER"}().Value;
local ESPIONAGE_SPY_POINT_UNIT = GameInfo.Defines{Name = "ESPIONAGE_SPY_POINT_UNIT"}().Value;
local MIN_WATER_SIZE_FOR_OCEAN = GameInfo.Defines{Name = "MIN_WATER_SIZE_FOR_OCEAN"}().Value;
local BALANCE_BUILDING_INVESTMENT_BASELINE = GameInfo.Defines{Name = "BALANCE_BUILDING_INVESTMENT_BASELINE"}().Value;
local INQUISITION_EFFECTIVENESS = GameInfo.Defines{Name = "INQUISITION_EFFECTIVENESS"}().Value;

-- Cache these values
local eMerchantOfVeniceUnit;
local iTrainPercent = Game and GameInfo.GameSpeeds[Game.GetGameSpeedType()].TrainPercent or 100;
local iNumEras = #GameInfo.Eras;

local L = Locale.Lookup;
local VP = MapModData and MapModData.VP or VP;
local GameInfoTypes = VP.GameInfoTypes;
local GameInfoCache = VP.GameInfoCache;
local GetCivsFromTrait = VP.GetCivsFromTrait;
local GetGreatPersonInfoFromSpecialist = VP.GetGreatPersonInfoFromSpecialist;
local CustomModOptionEnabled = CPK.Misc.CustomModOptionEnabled;

-- Mod options
local MOD_BALANCE_VP = CustomModOptionEnabled("BALANCE_VP");
local MOD_BALANCE_CORE_JFD = CustomModOptionEnabled("BALANCE_CORE_JFD");
local MOD_BALANCE_BUILDING_INVESTMENTS = CustomModOptionEnabled("BALANCE_BUILDING_INVESTMENTS");
local MOD_BALANCE_UNIT_INVESTMENTS = CustomModOptionEnabled("BALANCE_UNIT_INVESTMENTS");
local MOD_UNITS_RESOURCE_QUANTITY_TOTALS = CustomModOptionEnabled("UNITS_RESOURCE_QUANTITY_TOTALS");
local MOD_BALANCE_NEW_GREAT_PERSON_ATTRIBUTES = CustomModOptionEnabled("BALANCE_NEW_GREAT_PERSON_ATTRIBUTES");
local MOD_BALANCE_INQUISITOR_NERF = CustomModOptionEnabled("BALANCE_INQUISITOR_NERF");
local MOD_BALANCE_RETROACTIVE_PROMOTIONS = CustomModOptionEnabled("BALANCE_RETROACTIVE_PROMOTIONS");
local MOD_BALANCE_BOMBARD_RANGE_BUILDINGS = CustomModOptionEnabled("BALANCE_BOMBARD_RANGE_BUILDINGS");
local MOD_BALANCE_WORLD_WONDER_COST_INCREASE = CustomModOptionEnabled("BALANCE_WORLD_WONDER_COST_INCREASE");
local MOD_BALANCE_SPY_POINTS = CustomModOptionEnabled("BALANCE_SPY_POINTS");
local MOD_BALANCE_SETTLERS_CONSUME_POPULATION = CustomModOptionEnabled("BALANCE_SETTLERS_CONSUME_POPULATION");
local MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV = CustomModOptionEnabled("BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV");

local SEPARATOR_STRING = "----------------";
local EMPTY_SLOT_STRING = L("TXT_KEY_CITYVIEW_EMPTY_SLOT");

-------------------------------------------------
-- Help text for game components (Units, Buildings, etc.)
-------------------------------------------------

-- Helper functions

--- @param strColor string The open tag of the color, e.g. `[COLOR_POSITIVE_TEXT]`
--- @param strText string The text to be colored
--- @return string
local function TextColor(strColor, strText)
	return strColor .. strText .."[ENDCOLOR]";
end

--- Surround a unit name with `[COLOR_YELLOW]`
--- @param strText string
--- @return string
local function UnitColor(strText)
	return TextColor("[COLOR_YELLOW]", strText);
end

--- Surround a building name with `[COLOR_YIELD_FOOD]`
--- @param strText string
--- @return string
local function BuildingColor(strText)
	return TextColor("[COLOR_YIELD_FOOD]", strText);
end

--- Surround a project name with `[COLOR_CITY_BLUE]`
--- @param strText string
--- @return string
local function ProjectColor(strText)
	return TextColor("[COLOR_CITY_BLUE]", strText);
end

--- Surround a policy name with `[COLOR_MAGENTA]`
--- @param strText string
--- @return string
local function PolicyColor(strText)
	return TextColor("[COLOR_MAGENTA]", strText);
end

--- Surround a tech name with `[COLOR_CYAN]`
--- @param strText string
--- @return string
local function TechColor(strText)
	return TextColor("[COLOR_CYAN]", strText);
end

--- Surround a belief name with `[COLOR_WHITE]`
--- @param strText string
--- @return string
local function BeliefColor(strText)
	return TextColor("[COLOR_WHITE]", strText);
end

--- Return something like ` (<Tech name>)`, or empty string if `kTechInfo` is `nil`
--- @param kTechInfo table|nil The row in Technologies table
--- @return string
local function AppendTech(kTechInfo)
	return kTechInfo and TechColor(string.format(" (%s)", L(kTechInfo.Description))) or "";
end

--- Append `in all Cities` (or localized version) to the input string
--- @param strTooltip string
--- @return string
local function AppendGlobal(strTooltip)
	return string.format("%s %s", strTooltip, L("TXT_KEY_PRODUCTION_GLOBAL_SUFFIX"));
end

--- Append `, scaling with Era` (or localized version) to the input string
--- @param strTooltip string
--- @return string
local function AppendEraScaling(strTooltip)
	return string.format("%s, %s", strTooltip, L("TXT_KEY_PRODUCTION_BUILDING_ERA_SCALING_SUFFIX"));
end

--- Concatenate a list of strings with localized ", " and " and " before the last item
--- e.g. {"a"} -> "a", {"a", "b"} -> "a and b", {"a", "b", "c"} -> "a, b and c"
--- @param tItems string[]
--- @return string
local strListSeparator = L("TXT_KEY_LIST_SEPARATOR") .. " ";
local strListAnd = " " .. L("TXT_KEY_LIST_AND") .. " ";
local function ConcatWithCommaAnd(tItems)
	local n = #tItems;
	if n == 1 then
		return tItems[1];
	else
		local strTemp = ""
		strTemp = L("TXT_KEY_LIST_COMBINE_FINAL", tItems[n-1], tItems[n]);
		if n > 2 then
			for j = n-2, 1, -1 do
				strTemp = L("TXT_KEY_LIST_COMBINE_NONFINAL", tItems[j], strTemp);
			end
		end
		return strTemp
	end
end

--- Can the given unit gain the given promotion?
--- @param kUnitInfo Info
--- @param strPromotionType string
--- @return boolean
local function CanHavePromotion(kUnitInfo, strPromotionType)
	for _ in GameInfo.UnitPromotions_UnitCombats{PromotionType = strPromotionType, UnitCombatType = kUnitInfo.CombatClass} do
		return true;
	end
	for _ in GameInfo.UnitPromotions_CivilianUnitType{PromotionType = strPromotionType, UnitType = kUnitInfo.Type} do
		return true;
	end
	return false;
end

--- Get the corresponding unique building of the given player/city from the given building class.<br>
--- May return -1.
--- @param eBuildingClass BuildingClassType
--- @param pPlayer Player?
--- @param pCity City?
--- @return integer
local function GetUniqueBuildingFromBuildingClass(eBuildingClass, pPlayer, pCity)
	local eBuilding = pCity and pCity:GetBuildingTypeFromClass(eBuildingClass, true) or (pPlayer and pPlayer:GetCivilizationBuilding(eBuildingClass) or -1);
	-- Use default building if active civ doesn't have access to this building class
	if eBuilding == -1 then
		local strDefaultBuildingType = GameInfo.BuildingClasses[eBuildingClass].DefaultBuilding;
		return GameInfoTypes[strDefaultBuildingType] or -1;
	end
	return eBuilding;
end

--- Get the corresponding unique unit of the given player/city from the given unit class.<br>
--- May return -1.
--- @param strUnitClassType string
--- @param pPlayer Player?
--- @param pCity City?
--- @return integer
local function GetUniqueUnitFromUnitClass(strUnitClassType, pPlayer, pCity)
	local eUnit = pPlayer and pPlayer:GetSpecificUnitType(strUnitClassType, not pCity) or -1;
	-- Use default unit if active civ doesn't have access to this unit class
	if eUnit == -1 then
		local strDefaultUnitType = GameInfo.UnitClasses[strUnitClassType].DefaultUnit;
		return GameInfoTypes[strDefaultUnitType] or -1;
	end
	return eUnit;
end

--- Short hand for building a yield boost string. Instant yield string is also in the same format.
--- @param kYieldInfo Info
--- @param nYield number
--- @return string
local function GetYieldBoostString(kYieldInfo, nYield)
	return L("TXT_KEY_PRODUCTION_YIELD_BOOST", nYield, kYieldInfo.IconString);
end

--- Short hand for building a yield modifier string
--- @param kYieldInfo Info
--- @param nYield number
--- @return string
local function GetYieldModifierString(kYieldInfo, nYield)
	return L("TXT_KEY_PRODUCTION_YIELD_MODIFIER", nYield, kYieldInfo.IconString);
end

--- Short hand for building a fractional yield boost string
--- @param kYieldInfo Info
--- @param iNumerator integer
--- @param iDenominator integer
--- @return string
local function GetFractionYieldBoostString(kYieldInfo, iNumerator, iDenominator)
	if iDenominator == 1 then
		return GetYieldBoostString(kYieldInfo, iNumerator);
	end
	return L("TXT_KEY_PRODUCTION_YIELD_BOOST_FRACTION", iNumerator, iDenominator, kYieldInfo.IconString);
end

--- Add a localized text to the given table if the given conditions are met
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param bGlobal boolean Append `in all Cities` to the added string if true
--- @param bEraScaling boolean Append `, scaling with Era` to the added string if true
--- @param bNotZero boolean Add the text only if the first parameter is not zero (error if first parameter is string)
--- @param bPositive boolean Add the text only if the first parameter is positive (error if first parameter is string)
--- @param ... number|string The parameters for the text key
local function AddTooltipGeneric(tTooltipList, strTextKey, bGlobal, bEraScaling, bNotZero, bPositive, ...)
	if bNotZero and select(1, ...) == 0 then
		return;
	end

	if bPositive and select(1, ...) <= 0 then
		return;
	end

	-- print(strTextKey, ...);
	local strTooltip = L(strTextKey, ...);
	if bGlobal then
		strTooltip = AppendGlobal(strTooltip);
	end
	if bEraScaling then
		strTooltip = AppendEraScaling(strTooltip);
	end

	table.insert(tTooltipList, strTooltip);
end

--- Add a localized text to the given table
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param ... number|string The parameters for the text key
local function AddTooltip(tTooltipList, strTextKey, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, false, false, false, false, ...);
end

--- Add a localized text to the given table, appended with ", scaling with Era"
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param ... number|string The parameters for the text key
local function AddTooltipEraScaling(tTooltipList, strTextKey, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, false, true, false, false, ...);
end

--- Add a localized text to the given table only if the first parameter is not zero
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param nFirstParameter number First parameter for the text key
--- @param ... number|string The rest of the parameters for the text key
local function AddTooltipNonZero(tTooltipList, strTextKey, nFirstParameter, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, false, false, true, false, nFirstParameter, ...);
end

--- Add a localized text to the given table only if the first parameter is not zero<br>
--- The absolute value of the first parameter is passed into the localization function, and its sign ("+" or "-") is added as the second parameter
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param nFirstParameter number First parameter for the text key
--- @param ... number|string The rest of the parameters for the text key
local function AddTooltipNonZeroSigned(tTooltipList, strTextKey, nFirstParameter, ...)
	local strSign = nFirstParameter > 0 and "+" or "-";
	if not strSign then
		print(strTextKey, nFirstParameter);
	end
	nFirstParameter = math.abs(nFirstParameter);
	AddTooltipGeneric(tTooltipList, strTextKey, false, false, true, false, nFirstParameter, strSign, ...);
end

--- Add a localized text to the given table only if the first parameter is positive
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param nFirstParameter number First parameter for the text key
--- @param ... number|string The parameters for the text key
local function AddTooltipPositive(tTooltipList, strTextKey, nFirstParameter, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, false, false, false, true, nFirstParameter, ...);
end

--- Add a localized text to the given table, appended with `in all Cities`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param ... number|string The parameters for the text key
local function AddTooltipGlobal(tTooltipList, strTextKey, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, true, false, false, false, ...);
end

--- Add a localized text to the given table, appended with `in all Cities, scaling with Era`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param ... number|string The parameters for the text key
local function AddTooltipGlobalEraScaling(tTooltipList, strTextKey, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, true, true, false, false, ...);
end

--- Add a localized text to the given table, appended with `in all Cities`, only if the first parameter is not zero
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param nFirstParameter number First parameter for the text key
--- @param ... number|string The rest of the parameters for the text key
local function AddTooltipGlobalNonZero(tTooltipList, strTextKey, nFirstParameter, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, true, false, true, false, nFirstParameter, ...);
end

--- Add a localized text to the given table, appended with `in all Cities`, only if the first parameter is not zero<br>
--- The absolute value of the first parameter is passed into the localization function, and its sign ("+" or "-") is added as the second parameter
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param nFirstParameter number First parameter for the text key
--- @param ... number|string The rest of the parameters for the text key
local function AddTooltipGlobalNonZeroSigned(tTooltipList, strTextKey, nFirstParameter, ...)
	local strSign = nFirstParameter > 0 and "+" or "-";
	if not strSign then
		print(strTextKey, nFirstParameter);
	end
	nFirstParameter = math.abs(nFirstParameter);
	AddTooltipGeneric(tTooltipList, strTextKey, true, false, true, false, nFirstParameter, strSign, ...);
end

--- Add a localized text to the given table, appended with `in all Cities`, only if the first parameter is positive
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param nFirstParameter number First parameter for the text key
--- @param ... number|string The parameters for the text key
local function AddTooltipGlobalPositive(tTooltipList, strTextKey, nFirstParameter, ...)
	AddTooltipGeneric(tTooltipList, strTextKey, true, false, false, true, nFirstParameter, ...);
end

--- Add a localized text to the given table if the given condition is true
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added
--- @param bCondition boolean The condition that must be satified
--- @param ... number|string The parameters for the text key
local function AddTooltipIfTrue(tTooltipList, strTextKey, bCondition, ...)
	if bCondition then
		AddTooltipGeneric(tTooltipList, strTextKey, false, false, false, false, ...);
	end
end

--- Generic version of the AddTooltipsYieldTable functions, not supposed to be called directly.
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ or 2+ parameters (refer to the specific functions)
--- @param tYieldTable table<integer, table<integer, number?>?> The yield/modifier table to be converted into tooltip. If strOtherTypeTable is nil, the table is tYieldTable[1] and other entries are ignored.
--- @param strOtherTypeTable string? The name of the database table used as the key of the yield/modifier table, e.g. `"Specialists"`
--- @param bModifier boolean tYieldTable consists of modifiers (%) if true, flat yields otherwise
--- @param bGlobal boolean Whether "in all Cities" should be appended to tooltips
--- @param bEraScaling boolean Whether ", scaling with Era" should be appended to tooltips
--- @param ... number|string The rest of the parameters of strTextKey
local tGroupableTypeColors = {
	Resources = "",
	Features = "[COLOR_PLAYER_DARK_GREEN_TEXT]",
	Terrains = "[COLOR_PLAYER_DARK_GREEN_TEXT]",
	Plots = "[COLOR_PLAYER_DARK_GREEN_TEXT]",
	Specialists = "[COLOR_POSITIVE_TEXT]",
	BuildingClasses = "[COLOR_YIELD_FOOD]",
	Buildings = "[COLOR_YIELD_FOOD]",
	Improvements = "[COLOR_POSITIVE_TEXT]",
};

local function AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, tYieldTable, strOtherTypeTable, bModifier, bGlobal, bEraScaling, ...)
	if not next(tYieldTable) then
		return;
	end
	local GetStringFunc = bModifier and GetYieldModifierString or GetYieldBoostString;
	local AddTooltipFunc = bGlobal and (bEraScaling and AddTooltipGlobalEraScaling or AddTooltipGlobal) or (bEraScaling and AddTooltipEraScaling or AddTooltip);
	if strOtherTypeTable then
		local strColor = tGroupableTypeColors[strOtherTypeTable];
		if strColor then
			-- Group items by their yield boost string for merging
			local tItemsByYields = {};
			local tYieldsOrder = {};
			for eOtherType, kOtherTypeInfo in GameInfoCache(strOtherTypeTable) do
				if tYieldTable[eOtherType] then
					local tBoostStrings = {};
					for eYield, kYieldInfo in GameInfoCache("Yields") do
						local nYield = tYieldTable[eOtherType][eYield];
						if nYield and nYield ~= 0 then
							table.insert(tBoostStrings, GetStringFunc(kYieldInfo, nYield));
						end
					end
					if next(tBoostStrings) then
						local strYields = table.concat(tBoostStrings, " ");
						if not tItemsByYields[strYields] then
							tItemsByYields[strYields] = {};
							table.insert(tYieldsOrder, strYields);
						end
						local strName;
						if strOtherTypeTable == "Resources" then
							strName = kOtherTypeInfo.IconString .. " " .. L(kOtherTypeInfo.Description);
						else
							strName = strColor .. L(kOtherTypeInfo.Description) .. "[ENDCOLOR]";
						end
						table.insert(tItemsByYields[strYields], strName);
					end
				end
			end
			for _, strYields in ipairs(tYieldsOrder) do
				AddTooltipFunc(tTooltipList, strTextKey, ConcatWithCommaAnd(tItemsByYields[strYields]), strYields, ...);
			end
		else
			-- Non-groupable types (Beliefs, Technologies, Policies, etc.)
			for eOtherType, kOtherTypeInfo in GameInfoCache(strOtherTypeTable) do
				if tYieldTable[eOtherType] then
					local tBoostStrings = {};
					for eYield, kYieldInfo in GameInfoCache("Yields") do
						local nYield = tYieldTable[eOtherType][eYield];
						if nYield and nYield ~= 0 then
							table.insert(tBoostStrings, GetStringFunc(kYieldInfo, nYield));
						end
					end
					if next(tBoostStrings) then
						if strOtherTypeTable == "Beliefs" then
							AddTooltipFunc(tTooltipList, strTextKey, kOtherTypeInfo.ShortDescription, table.concat(tBoostStrings, " "), ...);
						else
							AddTooltipFunc(tTooltipList, strTextKey, kOtherTypeInfo.Description, table.concat(tBoostStrings, " "), ...);
						end
					end
				end
			end
		end
	else
		local tBoostStrings = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			local nYield = tYieldTable[1][eYield];
			if nYield and nYield ~= 0 then
				table.insert(tBoostStrings, GetStringFunc(kYieldInfo, nYield));
			end
		end
		if next(tBoostStrings) then
			AddTooltipFunc(tTooltipList, strTextKey, table.concat(tBoostStrings, " "), ...);
		end
	end
end

--- Add a localized yield boost text to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `Factory: +2 [ICON_PRODUCTION] +2 [ICON_GOLD]`<br>
--- `Astronomy: +3 [ICON_RESEARCH]`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 2+ parameters - first is the name of "other type", second is the yield boost
--- @param tYieldTable table<integer, table<integer, number?>?> The yield table to be converted into tooltip
--- @param strOtherTypeTable string The name of the database table used as the key of the yield table, e.g. `"Specialists"`
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipsYieldBoostTable(tTooltipList, strTextKey, tYieldTable, strOtherTypeTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, tYieldTable, strOtherTypeTable, false, false, false, ...);
end

--- Add a localized yield boost text, appended with "in all Cities", to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `Factory: +2 [ICON_PRODUCTION] +2 [ICON_GOLD] in all Cities`<br>
--- `Astronomy: +3 [ICON_RESEARCH] in all Cities`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 2 parameters - first is the name of "other type", second is the yield boost
--- @param tYieldTable table<integer, table<integer, number?>?> The yield table to be converted into tooltip
--- @param strOtherTypeTable string The name of the database table used as the key of the yield table, e.g. `"Specialists"`
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipsYieldBoostTableGlobal(tTooltipList, strTextKey, tYieldTable, strOtherTypeTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, tYieldTable, strOtherTypeTable, false, true, false, ...);
end

--- Add a localized yield boost text to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `+2 [ICON_PRODUCTION] +2 [ICON_GOLD] during [ICON_GOLDEN_AGE] Golden Age`<br>
--- `+1 [ICON_CULTURE] to Lake Tiles`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ parameter - the yield string
--- @param tYieldTable table<integer, number?> The yield table to be converted into tooltip
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipSimpleYieldBoostTable(tTooltipList, strTextKey, tYieldTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, {tYieldTable}, nil, false, false, false, ...);
end

--- Add a localized yield boost text, appended with "in all Cities", to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `+2 [ICON_PRODUCTION] +2 [ICON_GOLD] during [ICON_GOLDEN_AGE] Golden Age in all Cities`<br>
--- `+1 [ICON_RESEARCH] to Luxury Resources in all Cities`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ parameter - the yield string
--- @param tYieldTable table<integer, number?> The yield table to be converted into tooltip
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipSimpleYieldBoostTableGlobal(tTooltipList, strTextKey, tYieldTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, {tYieldTable}, nil, false, true, false, ...);
end

--- Add a localized yield boost text, appended with ", scaling with Era", to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `When entering a Golden Age: +10 [ICON_PRODUCTION] +10 [ICON_GOLD], scaling with Era`<br>
--- `On completion: +150 [ICON_CULTURE], scaling with Era`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ parameter - the yield string
--- @param tYieldTable table<integer, number?> The yield table to be converted into tooltip
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipSimpleYieldBoostTableEraScaling(tTooltipList, strTextKey, tYieldTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, {tYieldTable}, nil, false, false, true, ...);
end

--- Add a localized yield boost text, appended with "in all Cities, scaling with Era", to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `When pillaging an Improvement: +5 [ICON_GOLD] in all Cities, scaling with Era`<br>
--- `When completing a Building: +3 [ICON_CULTURE] in all Cities, scaling with Era`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ parameter - the yield string
--- @param tYieldTable table<integer, number?> The yield table to be converted into tooltip
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipSimpleYieldBoostTableGlobalEraScaling(tTooltipList, strTextKey, tYieldTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, {tYieldTable}, nil, false, true, true, ...);
end

--- Add a localized yield modifier text to the given tooltip table based on the given yield modifier table<br>
--- Example lines:<br>
--- `Windmill: +5% [ICON_PRODUCTION] +5% [ICON_GOLD]`<br>
--- `Division of Labor: +3% [ICON_RESEARCH]`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 2 parameters - first is the name of "other type", second is the yield modifier
--- @param tYieldModTable table<integer, table<integer, number?>?> The yield modifier table to be converted into tooltip
--- @param strOtherTypeTable string The name of the database table used as the key of the yield modifier table, e.g. `"Technologies"`
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipsYieldModifierTable(tTooltipList, strTextKey, tYieldModTable, strOtherTypeTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, tYieldModTable, strOtherTypeTable, true, false, false, ...);
end

--- Add a localized yield modifier text, appended with "in all Cities", to the given tooltip table based on the given yield modifier table<br>
--- Example lines:<br>
--- `+5% [ICON_PRODUCTION] +5% [ICON_GOLD] to Windmill in all Cities`<br>
--- `Electricity: +3% [ICON_RESEARCH] in all Cities`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 2 parameters - first is the name of "other type", second is the yield modifier
--- @param tYieldModTable table<integer, table<integer, number?>?> The yield modifier table to be converted into tooltip
--- @param strOtherTypeTable string The name of the database table used as the key of the yield modifier table, e.g. `"Technologies"`
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipsYieldModifierTableGlobal(tTooltipList, strTextKey, tYieldModTable, strOtherTypeTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, tYieldModTable, strOtherTypeTable, true, true, false, ...);
end

--- Add a localized yield modifier text to the given tooltip table based on the given yield modifier table<br>
--- Example lines:<br>
--- `+15% [ICON_PRODUCTION] +15% [ICON_GOLD] during [ICON_HAPPINESS_1] "We Love the King Day"`<br>
--- `+20% [ICON_RESEARCH] during [ICON_GOLDEN_AGE] Golden Age`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1 parameter - the yield modifier string
--- @param tYieldModTable table<integer, number?> The yield modifier table to be converted into tooltip
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipSimpleYieldModifierTable(tTooltipList, strTextKey, tYieldModTable, ...)
	AddTooltipsYieldTableGeneric(tTooltipList, strTextKey, {tYieldModTable}, nil, true, false, false, ...);
end

--- Generic version of the AddTooltipsYieldFractionTable functions, not supposed to be called directly.
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ or 2+ parameters (refer to the specific functions)
--- @param tYieldFractionTable table<integer, table<integer, table<string, integer>?>?> The fractional yield table to be converted into tooltip. If strOtherTypeTable is nil, the table is tYieldFractionTable[1] and other entries are ignored.
--- @param strOtherTypeTable string|nil The name of the database table used as the key of the yield table, e.g. `"Specialists"`
--- @param bGlobal boolean Whether "in all Cities" should be appended to tooltips
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipsYieldFractionTableGeneric(tTooltipList, strTextKey, tYieldFractionTable, strOtherTypeTable, bGlobal, ...)
	if not next(tYieldFractionTable) then
		return;
	end
	local AddTooltipFunc = bGlobal and AddTooltipGlobal or AddTooltip;
	if strOtherTypeTable then
		local strColor = tGroupableTypeColors[strOtherTypeTable];
		if strColor then
			-- Group items by their yield boost string for merging
			local tItemsByYields = {};
			local tYieldsOrder = {};
			for eOtherType, kOtherTypeInfo in GameInfoCache(strOtherTypeTable) do
				if tYieldFractionTable[eOtherType] then
					local tBoostStrings = {};
					for eYield, kYieldInfo in GameInfoCache("Yields") do
						local fYield = tYieldFractionTable[eOtherType][eYield];
						if fYield and fYield.Numerator ~= 0 then
							table.insert(tBoostStrings, GetFractionYieldBoostString(kYieldInfo, fYield.Numerator, fYield.Denominator));
						end
					end
					if next(tBoostStrings) then
						local strYields = table.concat(tBoostStrings, " ");
						if not tItemsByYields[strYields] then
							tItemsByYields[strYields] = {};
							table.insert(tYieldsOrder, strYields);
						end
						local strName;
						if strOtherTypeTable == "Resources" then
							strName = kOtherTypeInfo.IconString .. " " .. L(kOtherTypeInfo.Description);
						else
							strName = strColor .. L(kOtherTypeInfo.Description) .. "[ENDCOLOR]";
						end
						table.insert(tItemsByYields[strYields], strName);
					end
				end
			end
			for _, strYields in ipairs(tYieldsOrder) do
				AddTooltipFunc(tTooltipList, strTextKey, ConcatWithCommaAnd(tItemsByYields[strYields]), strYields, ...);
			end
		else
			-- Non-groupable types (Beliefs, etc.)
			for eOtherType, kOtherTypeInfo in GameInfoCache(strOtherTypeTable) do
				if tYieldFractionTable[eOtherType] then
					local tBoostStrings = {};
					for eYield, kYieldInfo in GameInfoCache("Yields") do
						local fYield = tYieldFractionTable[eOtherType][eYield];
						if fYield and fYield.Numerator ~= 0 then
							table.insert(tBoostStrings, GetFractionYieldBoostString(kYieldInfo, fYield.Numerator, fYield.Denominator));
						end
					end
					if next(tBoostStrings) then
						if strOtherTypeTable == "Beliefs" then
							AddTooltipFunc(tTooltipList, strTextKey, kOtherTypeInfo.ShortDescription, table.concat(tBoostStrings, " "), ...);
						else
							AddTooltipFunc(tTooltipList, strTextKey, kOtherTypeInfo.Description, table.concat(tBoostStrings, " "), ...);
						end
					end
				end
			end
		end
	else
		local tBoostStrings = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			local fYield = tYieldFractionTable[1][eYield];
			if fYield and fYield.Numerator ~= 0 then
				table.insert(tBoostStrings, GetFractionYieldBoostString(kYieldInfo, fYield.Numerator, fYield.Denominator));
			end
		end
		if next(tBoostStrings) then
			AddTooltipFunc(tTooltipList, strTextKey, table.concat(tBoostStrings, " "), ...);
		end
	end
end

--- Add a localized fractional yield boost text to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `Each Plantation in this City: +1/2 [ICON_PRODUCTION] +1/2 [ICON_GOLD]`<br>
--- `Each Forest in this City: +2/3 [ICON_RESEARCH]`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 2+ parameters - first is the name of "other type", second is the yield boost
--- @param tYieldFractionTable table<integer, table<integer, table<string, integer>?>?> The yield table to be converted into tooltip
--- @param strOtherTypeTable string The name of the database table used as the key of the yield table, e.g. `"Improvements"`
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipsYieldFractionTable(tTooltipList, strTextKey, tYieldFractionTable, strOtherTypeTable, ...)
	AddTooltipsYieldFractionTableGeneric(tTooltipList, strTextKey, tYieldFractionTable, strOtherTypeTable, false, ...);
end

--- Add a localized fractional yield boost text to the given tooltip table based on the given yield table<br>
--- Example lines:<br>
--- `Each luxury resource connected to this City: +1/2 [ICON_CULTURE] +1/2 [ICON_PEACE]`<br>
--- `Each building in this City: +1/3 [ICON_GOLD]`
--- @param tTooltipList string[] List of tooltip strings/lines to be added to
--- @param strTextKey string Text key to be added, must have 1+ parameter - the yield string
--- @param tYieldFractionTable table<integer, table<string, integer>>?> The yield table to be converted into tooltip
--- @param ... number|string The rest of the parameters of strTextKey
local function AddTooltipSimpleYieldFractionTable(tTooltipList, strTextKey, tYieldFractionTable, ...)
	AddTooltipsYieldFractionTableGeneric(tTooltipList, strTextKey, {tYieldFractionTable}, nil, false, ...);
end

--- Generate the tooltip for a unit. Can be called from tech tree, city view, etc.<br>
--- The city view version includes most buffs the unit has received from techs, policies, buildings, etc., to closely reflect what could be produced.
--- @param eUnit UnitType
--- @param bIncludeRequirementsInfo boolean
--- @param pCity City?
--- @param bExcludeName boolean? If true, omit the unit name in header
--- @param bGeneralInfo boolean? If true, don't compute any info that's player/city-specific. The `pCity` parameter is ignored. Defaults to true if the `Game` object doesn't exist.
--- @return string
function GetHelpTextForUnit(eUnit, bIncludeRequirementsInfo, pCity, bExcludeName, bGeneralInfo)
	local kUnitInfo = GameInfo.Units[eUnit];

	--- @type Player?
	local pActivePlayer = Game and Players[Game.GetActivePlayer()];

	-- Only general info if Game object doesn't exist (e.g. in pregame)
	if not Game then
		bGeneralInfo = true;
	end

	-- This will save a search later! Assume there's only one unit that can buy city states (or it'll likely break the game anyway)
	if not eMerchantOfVeniceUnit and kUnitInfo.CanBuyCityState then
		eMerchantOfVeniceUnit = eUnit;
	end

	-- When viewing a (foreign) city, always show tooltips as they are for the city owner
	if pCity then
		pActivePlayer = Players[pCity:GetOwner()];
	end

	-- Sometimes a city is needed in tooltips not in city view; in that case use the capital city
	local pActiveCity = pCity or pActivePlayer and pActivePlayer:GetCapitalCity();

	local kUnitClassInfo = GameInfo.UnitClasses[kUnitInfo.Class];
	local eActiveCiv = pActivePlayer and pActivePlayer:GetCivilizationType() or -1;
	local iInvestedCost = pCity and pCity:GetUnitInvestment(eUnit) or 0;

	-- Invalidate pCity, pActivePlayer, pActiveCity if we only want general info (then we don't have to additionally check for bGeneralInfo on top of nil checks)
	if bGeneralInfo then
		pCity = nil;
		pActivePlayer = nil;
		pActiveCity = nil;
	end

	local function CanPlayerEverBuildImprovement(strImprovementType)
		if not strImprovementType or not pActivePlayer then
			return false;
		end

		local kImprovementInfo = GameInfo.Improvements[strImprovementType];
		if kImprovementInfo.GraphicalOnly then
			return false;
		end

		if kImprovementInfo.CivilizationType and GameInfoTypes[kImprovementInfo.CivilizationType] ~= eActiveCiv then
			return false;
		end

		return true;
	end

	local tCanEverBuildImprovementCache = {};
	local function CanPlayerEverBuildImprovementCached(strImprovementType)
		if tCanEverBuildImprovementCache[strImprovementType] == nil then
			tCanEverBuildImprovementCache[strImprovementType] = CanPlayerEverBuildImprovement(strImprovementType);
		end
		return tCanEverBuildImprovementCache[strImprovementType];
	end

	-- Era-based changes (done early since they'll be accessed in different areas)
	local tEraStrengthLines = {};
	local tEraCombatLines = {};
	local tEraPromotionLines = {};
	local iCurrentEraStrength;
	local strCurrentEraCombatType;
	local tAttainedEraPromotions = {};
	if kUnitInfo.UnitEraUpgrade then
		local eCurrentEra = pActivePlayer and pActivePlayer:GetCurrentEra() or -1;
		for eEra, kEraInfo in GameInfoCache("Eras") do
			local bAttained = pCity and eCurrentEra >= eEra;
			local iEraStrength = 0;
			for row in GameInfo.Unit_EraCombatStrength{UnitType = kUnitInfo.Type, EraType = kEraInfo.Type} do
				iEraStrength = row.CombatStrength;
				-- Not breaking here; taking the last row value like in DLL if duplicated
			end

			local strEraCombatType;
			-- Assume there's only one entry; otherwise it'll be more complicated than the promotions below
			for row in GameInfo.Unit_EraCombatType{UnitType = kUnitInfo.Type, EraType = kEraInfo.Type} do
				if row.Value > 0 then
					strEraCombatType = row.UnitCombatType;
				end
			end

			local tEraPromotions = {};
			for row in GameInfo.Unit_EraUnitPromotions{UnitType = kUnitInfo.Type, EraType = kEraInfo.Type} do
				tEraPromotions[row.PromotionType] = row.Value;
				if bAttained then
					tAttainedEraPromotions[GameInfoTypes[row.PromotionType]] = row.Value;
				end
			end
			local tEraPromotionArray = {};
			for strEraPromotionType, iValue in pairs(tEraPromotions) do
				if iValue > 0 then
					table.insert(tEraPromotionArray, L(GameInfo.UnitPromotions[strEraPromotionType].Description));
				end
			end

			if bAttained then
				iCurrentEraStrength = iEraStrength > 0 and iEraStrength or iCurrentEraStrength;
				strCurrentEraCombatType = strEraCombatType or strCurrentEraCombatType;
			else
				AddTooltipPositive(tEraStrengthLines, "TXT_KEY_PRODUCTION_UNIT_STRENGTH_FROM_ERA", iEraStrength, kEraInfo.Description);
				if strEraCombatType then
					AddTooltip(tEraCombatLines, "TXT_KEY_PRODUCTION_UNIT_COMBAT_FROM_ERA",
						GameInfo.UnitCombatInfos[strEraCombatType].Description, kEraInfo.Description);
				end
				if next(tEraPromotionArray) then
					table.insert(tEraPromotionLines, L("TXT_KEY_PRODUCTION_UNIT_PROMOTION_FROM_ERA", table.concat(tEraPromotionArray, ", ")));
				end
			end
		end
	end

	local tLines = {};

	----------------------
	-- Header section
	----------------------
	local tHeaderLines = {};

	-- Name
	if not bExcludeName then
		local strName = UnitColor(Locale.ToUpper(L(kUnitInfo.Description)));
		local strUnitCombatType = strCurrentEraCombatType or kUnitInfo.CombatClass;
		if strUnitCombatType then
			if strUnitCombatType == "UNITCOMBAT_ARCHER" and kUnitInfo.IsMounted then
				strName = strName .. " " .. L("TXT_KEY_PRODUCTION_UNIT_COMBAT_CLASS_SKIRMISHER");
			else
				strName = strName .. " " .. L("TXT_KEY_PRODUCTION_UNIT_COMBAT_CLASS", GameInfo.UnitCombatInfos[strUnitCombatType].Description);
			end
		end
		if kUnitInfo.SpaceshipProject then
			strName = strName .. " " .. L("TXT_KEY_PRODUCTION_UNIT_SPACESHIP_PART");
		end
		if iInvestedCost > 0 then
			strName = strName .. L("TXT_KEY_INVESTED");
		end
		table.insert(tHeaderLines, strName);
	end

	-- Unique Unit? Usually unique to one civ, but it's possible that multiple civs have access to the same unit
	if kUnitInfo.MinorCivGift then
		AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_UNIQUE_UNIT_CITY_STATE");
	else
		local tCivAdjectives = {};
		for row in GameInfo.Civilization_UnitClassOverrides{UnitType = kUnitInfo.Type} do
			table.insert(tCivAdjectives, L(GameInfo.Civilizations[row.CivilizationType].Adjective));
		end
		if next(tCivAdjectives) then
			-- Get the unit it is replacing
			local kDefaultUnitInfo = GameInfo.Units[kUnitClassInfo.DefaultUnit];
			local strCivAdj = table.concat(tCivAdjectives, "/");
			if kDefaultUnitInfo then
				AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_UNIQUE_UNIT", strCivAdj, kDefaultUnitInfo.Description);
			else
				-- This unit isn't replacing anything
				AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_UNIQUE_UNIT_NO_DEFAULT", strCivAdj);
			end
		end
	end

	if next(tHeaderLines) then
		table.insert(tLines, table.concat(tHeaderLines, "[NEWLINE]"));
	end

	----------------------
	-- Stats section
	----------------------
	local tStatLines = {};

	-- Costs
	if kUnitInfo.Cost > 0 or kUnitInfo.FaithCost > 0 then
		local tCosts = {};
		local iProductionCost, iGoldCost, iFaithCost = 0, 0, 0;
		if kUnitInfo.Cost > 0 then
			iProductionCost = (iInvestedCost > 0) and iInvestedCost or
				(pActiveCity and pActiveCity:GetUnitProductionNeeded(eUnit) or
				(pActivePlayer and pActivePlayer:GetUnitProductionNeeded(eUnit) or kUnitInfo.Cost));
			iGoldCost = pActiveCity and pActiveCity:GetUnitPurchaseCost(eUnit) or 0;
		end

		if kUnitInfo.FaithCost > 0 then
			iFaithCost = pActiveCity and pActiveCity:GetUnitFaithPurchaseCost(eUnit, true) or kUnitInfo.FaithCost;
		end

		if not kUnitInfo.PurchaseOnly then
			AddTooltipPositive(tCosts, "TXT_KEY_PRODUCTION_COST_PRODUCTION", iProductionCost);
		end
		AddTooltipPositive(tCosts, "TXT_KEY_PRODUCTION_COST_GOLD", iGoldCost);
		AddTooltipPositive(tCosts, "TXT_KEY_PRODUCTION_COST_FAITH", iFaithCost);

		if next(tCosts) then
			table.insert(tStatLines, L("TXT_KEY_PRODUCTION_COST_PREFIX", table.concat(tCosts, " / ")));
		end
	end

	-- Maintenance (base amount too dynamic to be shown)
	-- These columns can stack, even if they don't make sense together
	if kUnitInfo.ExtraMaintenanceCost > 0 then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_UNIT_EXTRA_MAINTENANCE", kUnitInfo.ExtraMaintenanceCost);
	else
		AddTooltipNonZero(tStatLines, "TXT_KEY_PRODUCTION_UNIT_REDUCED_MAINTENANCE", kUnitInfo.ExtraMaintenanceCost * -1);
	end
	AddTooltipIfTrue(tStatLines, "TXT_KEY_PRODUCTION_UNIT_NO_MAINTENANCE", kUnitInfo.NoMaintenance);

	-- Supply cost
	AddTooltipIfTrue(tStatLines, "TXT_KEY_PRODUCTION_UNIT_NO_SUPPLY", kUnitInfo.NoSupply);

	-- Stagnation
	AddTooltipIfTrue(tStatLines, "TXT_KEY_PRODUCTION_UNIT_STAGNATION", kUnitInfo.Food);
	AddTooltipIfTrue(tStatLines, "TXT_KEY_PRODUCTION_UNIT_REDUCE_POPULATION", MOD_BALANCE_SETTLERS_CONSUME_POPULATION and kUnitInfo.Food and (kUnitInfo.Found or kUnitInfo.FoundMid or kUnitInfo.FoundLate or kUnitInfo.FoundAbroad));

	-- Max HP (show when non-standard only)
	local iMaxHP = kUnitInfo.MaxHitPoints;
	if iMaxHP ~= 100 then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_UNIT_MAX_HP", iMaxHP);
	end

	-- Moves
	if not kUnitInfo.Immobile then
		AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_MOVEMENT", kUnitInfo.Moves);
	end

	-- Sight (show when non-standard only)
	local iSight = kUnitInfo.BaseSightRange;
	if iSight > 0 and iSight ~= 2 then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_UNIT_SIGHT", iSight);
	end

	-- Range
	AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_RANGE", kUnitInfo.Range);

	-- Ranged Combat Strength
	AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_RANGED_STRENGTH", kUnitInfo.RangedCombat);

	-- Combat Strength
	AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_STRENGTH", iCurrentEraStrength or kUnitInfo.Combat);

	-- Air Defense
	AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_UNIT_AIR_DEFENSE", kUnitInfo.BaseLandAirDefense);

	-- Intercept Range
	AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_UNIT_INTERCEPT_RANGE", kUnitInfo.AirInterceptRange);

	-- Air Slots (show when non-standard only)
	if kUnitInfo.Domain == GameInfoTypes.DOMAIN_AIR then
		local iAirSlots = kUnitInfo.AirUnitCap;
		if iAirSlots > 1 then
			AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_UNIT_AIR_SLOTS", iAirSlots);
		elseif iAirSlots == 0 then
			AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_UNIT_NO_AIR_SLOTS");
		end
	end

	-- Cargo
	if kUnitInfo.SpecialCargo then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_UNIT_CAN_CARRY", GameInfo.SpecialUnits[kUnitInfo.SpecialCargo].Description);
	end

	-- Unhappiness (can be negative)
	if not MOD_BALANCE_VP then
		if kUnitInfo.Unhappiness > 0 then
			AddTooltipNonZero(tStatLines, "TXT_KEY_PRODUCTION_UNIT_UNHAPPINESS", kUnitInfo.Unhappiness);
		else
			AddTooltipNonZero(tStatLines, "TXT_KEY_PRODUCTION_UNIT_UNHAPPINESS_REDUCTION", kUnitInfo.Unhappiness * -1);
		end
	end

	if next(tStatLines) then
		table.insert(tLines, table.concat(tStatLines, "[NEWLINE]"));
	end

	----------------------
	-- Ability section
	----------------------
	local tAbilityLines = {};

	-- Simple (boolean) abilities
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_MOVE_AFTER_PURCHASE", kUnitInfo.MoveAfterPurchase);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_IGNORE_BUILDING_DEFENSE", kUnitInfo.IgnoreBuildingDefense);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_FREE_UPGRADE", kUnitInfo.FreeUpgrade);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_RIVAL_TERRITORY", kUnitInfo.RivalTerritory);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_MISSION_BUY_CITY_STATE", kUnitInfo.CanBuyCityState);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_MISSION_SPREAD_RELIGION", kUnitInfo.SpreadReligion);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_MISSION_REMOVE_HERESY", kUnitInfo.RemoveHeresy);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_MISSION_FOUND_RELIGION", kUnitInfo.FoundReligion);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_CULTURE_ON_DISBAND_UPGRADE", kUnitInfo.CulExpOnDisbandUpgrade);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_EXTRA_PLUNDER_GOLD", kUnitInfo.HighSeaRaider);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_EXPEND_COPY_TILE_YIELD", kUnitInfo.CopyYieldsFromExpendTile);
	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_MOVE_AFTER_UPGRADE", kUnitInfo.MoveAfterUpgrade);

	-- Block/weaken active spread
	if kUnitInfo.ProhibitsSpread then
		if MOD_BALANCE_INQUISITOR_NERF then
			AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_PROHIBIT_SPREAD_PARTIAL", INQUISITION_EFFECTIVENESS);
		else
			AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_PROHIBIT_SPREAD");
		end
	end

	-- Found city
	if kUnitInfo.Found or kUnitInfo.FoundAbroad or kUnitInfo.FoundColony > 0 then
		local strFound;

		-- Technically, if FoundColony is a small number and Found is false, there is only a limited number of colonies that can be founded per game (see old Venice)
		-- But I don't see that feature being used anymore, so let's just assume colonies can always be founded
		if kUnitInfo.FoundColony > 0 then
			strFound = L("TXT_KEY_PRODUCTION_UNIT_FOUND_CITY_PUPPET");
		else
			if kUnitInfo.FoundAbroad then
				strFound = L("TXT_KEY_PRODUCTION_UNIT_FOUND_CITY_ABROAD");
			else
				strFound = L("TXT_KEY_PRODUCTION_UNIT_FOUND_CITY");
			end
			if kUnitInfo.FoundMid or kUnitInfo.FoundLate then
				strFound = string.format("%s %s", strFound, L("TXT_KEY_PRODUCTION_UNIT_FOUND_CITY_ADVANCED"));
			end
		end

		-- Founding restriction
		strFound = string.format("%s[NEWLINE]%s", strFound, L("TXT_KEY_PRODUCTION_UNIT_FOUND_CITY_RESTRICTION"));
		table.insert(tAbilityLines, strFound);

		-- Free buildings on found
		local tFoundBuildings = {};
		for row in GameInfo.Unit_BuildOnFound{UnitType = kUnitInfo.Type} do
			local eBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer);
			if eBuilding ~= -1 then
				AddTooltip(tFoundBuildings, GameInfo.Buildings[eBuilding].Description);
			else
				AddTooltip(tFoundBuildings, GameInfo.BuildingClasses[row.BuildingClassType].Description);
			end
		end
		if next(tFoundBuildings) then
			AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_FOUND_CITY_FREE_BUILDINGS", table.concat(tFoundBuildings, ", "));
		end
	end

	-- Culture bomb
	local iRadius = kUnitInfo.CultureBombRadius;
	local iCultureBomb = kUnitInfo.NumberOfCultureBombs;
	if iRadius > 0 and iCultureBomb > 0 then
		if pCity then
			iRadius = iRadius + (pActivePlayer and pActivePlayer:GetCultureBombBoost() or 0);
		end
		-- It is possible to have a negative radius boost, and a culture bomb radius of 0 is possible (claiming the tile the unit is on)
		if iRadius >= 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_CULTURE_BOMB", iRadius, iCultureBomb);
		end
	end

	-- Sell exotic goods
	AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_SELL_EXOTIC_GOODS", kUnitInfo.NumExoticGoods);

	-- Ancient ruin reward modifier
	AddTooltipNonZeroSigned(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_GOODY_MODIFIER", kUnitInfo.GoodyModifier);

	-- Free promotions (ensure no duplicates)
	local tPromotionKeys = {};
	local tPromotionLines = {};
	for row in GameInfo.Unit_FreePromotions{UnitType = kUnitInfo.Type} do
		tPromotionKeys[GameInfoTypes[row.PromotionType]] = true;
		AddTooltip(tPromotionLines, GameInfo.UnitPromotions[row.PromotionType].Description);
	end
	-- Show these only in city view
	if pActivePlayer and pCity then
		for ePromotion, kPromotionInfo in GameInfoCache("UnitPromotions") do
			if not tPromotionKeys[ePromotion]
			and (pActivePlayer:IsFreePromotion(ePromotion) or pCity:IsFreePromotion(ePromotion))
			and CanHavePromotion(kUnitInfo, kPromotionInfo.Type) then
				tPromotionKeys[ePromotion] = true;
				AddTooltip(tPromotionLines, kPromotionInfo.Description);
			end
		end
		for ePromotion, iValue in pairs(tAttainedEraPromotions) do
			if iValue > 0 and not tPromotionKeys[ePromotion] then
				tPromotionKeys[ePromotion] = true;
				AddTooltip(tPromotionLines, GameInfo.UnitPromotions[ePromotion].Description);
			end
		end
	end
	if next(tPromotionLines) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_FREE_PROMOTIONS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tPromotionLines, "[NEWLINE][ICON_BULLET]"));
	end

	-- Free promotion in friendly lands
	if kUnitInfo.FriendlyLandsPromotion then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_FREE_PROMOTIONS_FRIENDLY", GameInfo.UnitPromotions[kUnitInfo.FriendlyLandsPromotion].Description);
	end

	-- Builds (e.g. chop trees and build roads) and improvements
	local tBuildLines = {};
	local tImprovementLines = {};
	for row in GameInfo.Unit_Builds{UnitType = kUnitInfo.Type} do
		local kBuildInfo = GameInfo.Builds[row.BuildType];
		if kBuildInfo.ShowInPedia then
			local kPrereqTechInfo = kBuildInfo.PrereqTech and GameInfo.Technologies[kBuildInfo.PrereqTech];
			if kBuildInfo.ImprovementType then
				-- Only show improvements that the player can build
				if not pActivePlayer or CanPlayerEverBuildImprovementCached(kBuildInfo.ImprovementType) then
					table.insert(tImprovementLines, L(GameInfo.Improvements[kBuildInfo.ImprovementType].Description) .. AppendTech(kPrereqTechInfo));
				end
			else
				table.insert(tBuildLines, L(kBuildInfo.Description) .. AppendTech(kPrereqTechInfo));
			end
		end
	end
	if next(tBuildLines) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_BUILD_NON_IMPROVEMENTS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tBuildLines, "[NEWLINE][ICON_BULLET]"));
	end
	if next(tImprovementLines) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_BUILD_IMPROVEMENTS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tImprovementLines, "[NEWLINE][ICON_BULLET]"));
	end

	-- Instant yield when created
	local tInstantYields = {};
	for row in GameInfo.Unit_YieldOnCompletion{UnitType = kUnitInfo.Type} do
		table.insert(tInstantYields, GetYieldBoostString(GameInfo.Yields[row.YieldType], row.Yield));
	end
	if next(tInstantYields) then
		table.insert(tAbilityLines, L("TXT_KEY_PRODUCTION_UNIT_YIELD_ON_COMPLETION", table.concat(tInstantYields, ", ")));
	end

	-- These work on GP only
	if kUnitInfo.Special == "SPECIALUNIT_PEOPLE" then
		-- Global WLTKD on birth
		if kUnitInfo.WLTKDFromBirth then
			local iWLTKDTurn = math.floor(math.floor(CITY_RESOURCE_WLTKD_TURNS / 3) * iTrainPercent / 100);
			AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_BIRTH_WLTKD", iWLTKDTurn);
		end

		-- Golden age on birth
		if kUnitInfo.GoldenAgeFromBirth then
			AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_BIRTH_WLTKD", pActivePlayer and pActivePlayer:GetGoldenAgeLength() or GOLDEN_AGE_LENGTH);
		end

		-- Culture on birth to capital
		if kUnitInfo.CultureBoost then
			if pActivePlayer then
				AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_BIRTH_CULTURE", pActivePlayer:GetTotalJONSCulturePerTurnTimes100() * 4 / 100);
			else
				AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_BIRTH_CULTURE_GENERIC");
			end
		end

		-- Free supply when expended
		AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_EXPEND_SUPPLY", kUnitInfo.SupplyCapBoost);
	end

	-- Extra attack/move and heal on kill
	if kUnitInfo.ExtraAttackHealthOnKill then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_EXTRA_ATTACK_HEAL_ON_KILL", PILLAGE_HEAL_AMOUNT);
	end

	-- Instant yields on kill
	tInstantYields = {};
	for row in GameInfo.Unit_YieldFromKills{UnitType = kUnitInfo.Type} do
		table.insert(tInstantYields, GetYieldBoostString(GameInfo.Yields[row.YieldType], row.Yield / 100));
	end
	if next(tInstantYields) then
		table.insert(tAbilityLines, L("TXT_KEY_PRODUCTION_UNIT_INSTANT_YIELD_ON_KILL", table.concat(tInstantYields, ", ")));
	end
	tInstantYields = {};
	for row in GameInfo.Unit_YieldFromBarbarianKills{UnitType = kUnitInfo.Type} do
		table.insert(tInstantYields, GetYieldBoostString(GameInfo.Yields[row.YieldType], row.Yield / 100));
	end
	if next(tInstantYields) then
		table.insert(tAbilityLines, L("TXT_KEY_PRODUCTION_UNIT_INSTANT_YIELD_ON_KILL_BARBARIAN", table.concat(tInstantYields, ", ")));
	end

	-- Free resource when expended
	for row in GameInfo.Unit_ResourceQuantityExpended{UnitType = kUnitInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_FREE_RESOURCE", row.Amount, kResourceInfo.IconString, kResourceInfo.Description);
	end

	-- Free XP to units when expended
	AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_EXPEND_XP", kUnitInfo.TileXPOnExpend);

	-- Yields/bonuses when expended
	if pActivePlayer then
		-- Avoid calling DLL where possible - table lookups are way faster
		local iGold = (kUnitInfo.BaseGold > 0 or kUnitInfo.BaseGoldTurnsToCount > 0 or kUnitInfo.NumGoldPerEra ~= 0) and pActivePlayer:GetTradeGold(eUnit) or 0;
		local strGold = (iGold > 0) and L("TXT_KEY_PRODUCTION_UNIT_TRADE_MISSION_GOLD", iGold);
		local iWLTKDTurn = MOD_BALANCE_NEW_GREAT_PERSON_ATTRIBUTES and (kUnitInfo.BaseWLTKDTurns > 0) and pActivePlayer:GetTradeWLTKDTurns(eUnit) or 0;
		local strWLTKD = (iWLTKDTurn > 0) and L("TXT_KEY_PRODUCTION_UNIT_TRADE_MISSION_WLTKD", iWLTKDTurn);
		if strGold and strWLTKD then
			table.insert(tAbilityLines, string.format("%s: %s, %s", L("TXT_KEY_MISSION_CONDUCT_TRADE_MISSION"), strGold, strWLTKD));
		elseif strGold or strWLTKD then
			table.insert(tAbilityLines, string.format("%s: %s", L("TXT_KEY_MISSION_CONDUCT_TRADE_MISSION"), strGold or strWLTKD));
		end

		local iScience = (kUnitInfo.BaseBeakersTurnsToCount > 0) and pActivePlayer:GetDiscoverScience(eUnit) or 0;
		local strScience = (iScience > 0) and L("TXT_KEY_PRODUCTION_UNIT_DISCOVER_TECH_SCIENCE", iScience);
		local iTechs = kUnitInfo.NumFreeTechs;
		local strTechs = (iTechs > 0) and L("TXT_KEY_PRODUCTION_UNIT_DISCOVER_TECH", iTechs);
		if strScience and strTechs then
			table.insert(tAbilityLines, string.format("%s: %s, %s", L("TXT_KEY_MISSION_DISCOVER_TECH"), strTechs, strScience));
		elseif strScience or strTechs then
			table.insert(tAbilityLines, string.format("%s: %s", L("TXT_KEY_MISSION_DISCOVER_TECH"), strTechs or strScience));
		end

		local iCulture = (kUnitInfo.BaseCultureTurnsToCount > 0) and pActivePlayer:GetTreatiseCulture(eUnit) or 0;
		local strCulture = (iCulture > 0) and L("TXT_KEY_PRODUCTION_UNIT_GIVE_POLICIES_CULTURE", iCulture);
		local iPolicies = kUnitInfo.FreePolicies;
		local strPolicies = (iPolicies > 0) and L("TXT_KEY_PRODUCTION_UNIT_GIVE_POLICIES", iPolicies);
		if strCulture and strPolicies then
			table.insert(tAbilityLines, string.format("%s: %s, %s", L("TXT_KEY_MISSION_GIVE_POLICIES"), strPolicies, strCulture));
		elseif strCulture or strPolicies then
			table.insert(tAbilityLines, string.format("%s: %s", L("TXT_KEY_MISSION_GIVE_POLICIES"), strPolicies or strCulture));
		end

		local iGAP = (kUnitInfo.BaseTurnsForGAPToCount > 0) and pActivePlayer:GetBlastGAP(eUnit) or 0;
		local iGATurns = kUnitInfo.GoldenAgeTurns;
		if iGAP > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_START_GOLDENAGE", iGAP);
		elseif iGATurns > 0 then
			-- Number of turns is dynamic so we won't know at this point
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_START_GOLDENAGE");
		end

		local iTourism = (kUnitInfo.OneShotTourism > 0) and pActivePlayer:GetBlastTourism(eUnit) or 0;
		if iTourism > 0 then
			local iOthersPercent = kUnitInfo.OneShotTourismPercentOthers;
			if iOthersPercent > 0 then
				AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_ONE_SHOT_TOURISM_OTHER_PLAYERS", iTourism, iOthersPercent);
			else
				AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_ONE_SHOT_TOURISM", iTourism);
			end
		end

		local iTourismTurn = (kUnitInfo.TourismBonusTurns > 0) and pActivePlayer:GetBlastTourismTurns(eUnit) or 0;
		AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_CONCERT_TOUR", iTourismTurn);

		-- For hurry production, we only know the exact number if there's a city specified, or if it doesn't depend on population
		if pCity or kUnitInfo.HurryMultiplier == 0 then
			if pActiveCity then
				local iProduction = (kUnitInfo.BaseHurry > 0 or kUnitInfo.BaseProductionTurnsToCount > 0) and pActiveCity:GetHurryProduction(eUnit) or 0;
				AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_HURRY_PRODUCTION", iProduction);
			end
		elseif kUnitInfo.BaseHurry > 0 or kUnitInfo.BaseProductionTurnsToCount > 0 or kUnitInfo.HurryMultiplier > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_HURRY_PRODUCTION");
		end
	else
		if kUnitInfo.BaseGold > 0 or kUnitInfo.BaseGoldTurnsToCount > 0 or kUnitInfo.NumGoldPerEra > 0 or kUnitInfo.BaseWLTKDTurns > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_CONDUCT_TRADE_MISSION");
		end
		if kUnitInfo.BaseCultureTurnsToCount > 0 or kUnitInfo.NumFreeTechs > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_DISCOVER_TECH");
		end
		if kUnitInfo.BaseCultureTurnsToCount > 0 or kUnitInfo.FreePolicies > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_GIVE_POLICIES");
		end
		if kUnitInfo.BaseTurnsForGAPToCount > 0 or kUnitInfo.GoldenAgeTurns > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_START_GOLDENAGE");
		end
		if kUnitInfo.OneShotTourism > 0 or kUnitInfo.TourismBonusTurns > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_ONE_SHOT_TOURISM");
		end
		if kUnitInfo.BaseHurry > 0 or kUnitInfo.BaseProductionTurnsToCount > 0 or kUnitInfo.HurryMultiplier > 0 then
			AddTooltip(tAbilityLines, "TXT_KEY_MISSION_HURRY_PRODUCTION");
		end
	end

	AddTooltipNonZeroSigned(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_TRADE_MISSION_RESTING_INFLUENCE", kUnitInfo.RestingPointChange);

	AddTooltipPositive(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_FREE_LUXURY", kUnitInfo.NumFreeLux);

	-- Bad abilities (placed at the end)
	local tNegativeAbilities = {
		Suicide = "TXT_KEY_PRODUCTION_UNIT_SUICIDE",
		CityAttackOnly = "TXT_KEY_PRODUCTION_UNIT_CITY_ATTACK_ONLY",
	};

	for k, v in pairs(tNegativeAbilities) do
		AddTooltipIfTrue(tAbilityLines, v, kUnitInfo[k]);
	end

	AddTooltipIfTrue(tAbilityLines, "TXT_KEY_PRODUCTION_UNIT_NO_EMBARK", kUnitInfo.CannotEmbark);

	-- Bounty on this unit
	tInstantYields = {};
	for row in GameInfo.Unit_Bounties{UnitType = kUnitInfo.Type} do
		table.insert(tInstantYields, GetYieldBoostString(GameInfo.Yields[row.YieldType], row.Yield));
	end
	if next(tInstantYields) then
		table.insert(tAbilityLines, L("TXT_KEY_PRODUCTION_UNIT_BOUNTY", table.concat(tInstantYields, ", ")));
	end

	if next(tAbilityLines) then
		table.insert(tLines, table.concat(tAbilityLines, "[NEWLINE]"));
	end

	----------------------
	-- Requirement section
	-- Most are skipped in city view
	----------------------
	local tReqLines = {};

	-- Building requirement
	if not pCity then
		local tBuildings = {};
		for row in GameInfo.Unit_BuildingClassRequireds{UnitType = kUnitInfo.Type} do
			local eBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer);
			if eBuilding ~= -1 then
				AddTooltip(tBuildings, GameInfo.Buildings[eBuilding].Description);
			else
				AddTooltip(tBuildings, GameInfo.BuildingClasses[row.BuildingClassType].Description);
			end
		end
		if next(tBuildings) then
			table.insert(tReqLines, L("TXT_KEY_PRODUCTION_REQUIRED_BUILDINGS", table.concat(tBuildings, ", ")));
		end
	end

	-- Buildings required for purchase/investment
	local tBuildings = {};
	for row in GameInfo.Unit_BuildingClassPurchaseRequireds{UnitType = kUnitInfo.Type} do
		local eBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer, pCity);
		if eBuilding ~= -1 then
			AddTooltip(tBuildings, GameInfo.Buildings[eBuilding].Description);
		else
			AddTooltip(tBuildings, GameInfo.BuildingClasses[row.BuildingClassType].Description);
		end
	end
	if next(tBuildings) then
		local strTextKey = MOD_BALANCE_BUILDING_INVESTMENTS and "TXT_KEY_PRODUCTION_REQUIRED_BUILDINGS_INVEST" or "TXT_KEY_PRODUCTION_REQUIRED_BUILDINGS_PURCHASE";
		table.insert(tReqLines, L(strTextKey, table.concat(tBuildings, ", ")));
	end

	-- May be purchased/invested in puppets
	local strTextKey = MOD_BALANCE_UNIT_INVESTMENTS and "TXT_KEY_PRODUCTION_PUPPET_INVESTABLE" or "TXT_KEY_PRODUCTION_PUPPET_PURCHASABLE";
	AddTooltipIfTrue(tReqLines, strTextKey, kUnitInfo.PuppetPurchaseOverride);

	if not pCity then
		-- Project requirement
		if kUnitInfo.ProjectPrereq then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_PROJECT", GameInfo.Projects[kUnitInfo.ProjectPrereq].Description);
		end

		-- Policy requirement
		if kUnitInfo.PolicyType then
			-- Is this an opener or finisher? Assume openers and finishers are distinct across policy branches
			local bOpenerOrFinisher = false;
			for _, kPolicyBranchInfo in GameInfoCache("PolicyBranchTypes") do
				if kPolicyBranchInfo.FreePolicy == kUnitInfo.PolicyType then
					bOpenerOrFinisher = true;
					AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY_BRANCH_OPENER", kPolicyBranchInfo.Description);
					break;
				end
				if kPolicyBranchInfo.FreeFinishingPolicy == kUnitInfo.PolicyType then
					bOpenerOrFinisher = true;
					AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY_BRANCH_COMPLETION", kPolicyBranchInfo.Description);
					break;
				end
			end
			if not bOpenerOrFinisher then
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY", GameInfo.Policies[kUnitInfo.PolicyType].Description);
			end
		end

		-- Belief requirement
		if kUnitInfo.BeliefRequired then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_BELIEF", GameInfo.Beliefs[kUnitInfo.BeliefRequired].ShortDescription);
		end

		-- Prereq techs
		local tTechs = {};
		if kUnitInfo.PrereqTech then
			AddTooltip(tTechs, GameInfo.Technologies[kUnitInfo.PrereqTech].Description);
		end
		for row in GameInfo.Unit_TechTypes{UnitType = kUnitInfo.Type} do
			AddTooltip(tTechs, GameInfo.Technologies[row.TechType].Description);
		end
		if next(tTechs) then
			table.insert(tReqLines, L("TXT_KEY_PRODUCTION_PREREQ_TECH", table.concat(tTechs, ", ")));
		end
	end

	-- Obsolete tech
	if kUnitInfo.ObsoleteTech then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_OBSOLETE_TECH", GameInfo.Technologies[kUnitInfo.ObsoleteTech].Description);
	end

	-- Resource requirements
	for row in GameInfo.Unit_ResourceQuantityRequirements{UnitType = kUnitInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		AddTooltipPositive(tReqLines, "TXT_KEY_PRODUCTION_RESOURCES_REQUIRED", row.Cost, kResourceInfo.IconString, kResourceInfo.Description);
	end

	if kUnitInfo.ResourceType then
		local kResourceInfo = GameInfo.Resources[kUnitInfo.ResourceType];
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_TOTAL_RESOURCES_REQUIRED", 1, kResourceInfo.IconString, kResourceInfo.Description);
	end

	if MOD_UNITS_RESOURCE_QUANTITY_TOTALS then
		for row in GameInfo.Unit_ResourceQuantityTotals{UnitType = kUnitInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			AddTooltipPositive(tReqLines, "TXT_KEY_PRODUCTION_TOTAL_RESOURCES_REQUIRED", row.Amount, kResourceInfo.IconString, kResourceInfo.Description);
		end
	end

	-- Enhanced religion
	AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_UNIT_REQUIRE_ENHANCED_RELIGION", not pCity and kUnitInfo.RequiresEnhancedReligion);

	-- Unit class limits
	local iMaxGlobalInstance = kUnitClassInfo.MaxGlobalInstances;
	if iMaxGlobalInstance ~= -1 then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_MAX_GLOBAL_INSTANCE", iMaxGlobalInstance);
	end
	local iMaxTeamInstance = kUnitClassInfo.MaxTeamInstances;
	if iMaxTeamInstance ~= -1 then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_MAX_TEAM_INSTANCE", iMaxTeamInstance);
	end
	local iMaxPlayerInstance = kUnitClassInfo.MaxPlayerInstances;
	local iMaxCityInstance = kUnitClassInfo.UnitInstancePerCity;
	if iMaxCityInstance ~= -1 then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_UNIT_MAX_CITY_INSTANCE", iMaxCityInstance);
	elseif iMaxPlayerInstance ~= -1 then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_MAX_PLAYER_INSTANCE", iMaxPlayerInstance);
	end

	if next(tReqLines) then
		table.insert(tLines, table.concat(tReqLines, "[NEWLINE]"));
	end

	----------------------
	-- Potential boosts
	-- In city view, only show if the boost has not been obtained
	----------------------
	local tBoostLines = {};

	if next(tEraStrengthLines) then
		AddTooltip(tBoostLines, "TXT_KEY_PRODUCTION_UNIT_STRENGTH_FROM_ERA_SUMMARY");
		table.insert(tBoostLines, "[ICON_BULLET]" .. table.concat(tEraStrengthLines, "[NEWLINE][ICON_BULLET]"));
	end

	if next(tEraCombatLines) then
		AddTooltip(tBoostLines, "TXT_KEY_PRODUCTION_UNIT_COMBAT_FROM_ERA_SUMMARY");
		table.insert(tBoostLines, "[ICON_BULLET]" .. table.concat(tEraCombatLines, "[NEWLINE][ICON_BULLET]"));
	end

	if next(tEraPromotionLines) then
		AddTooltip(tBoostLines, "TXT_KEY_PRODUCTION_UNIT_PROMOTION_FROM_ERA_SUMMARY");
		table.insert(tBoostLines, "[ICON_BULLET]" .. table.concat(tEraPromotionLines, "[NEWLINE][ICON_BULLET]"));
	end

	if next(tBoostLines) then
		table.insert(tLines, table.concat(tBoostLines, "[NEWLINE]"));
	end

	----------------------
	-- Pre-written section
	----------------------
	local tPreWrittenLines = {};

	-- Pre-written Help text
	if kUnitInfo.Help then
		local strWrittenHelp = L(kUnitInfo.Help);
		if strWrittenHelp ~= "" then
			table.insert(tLines, strWrittenHelp);
		end
	end

	-- Requirements text
	if bIncludeRequirementsInfo and kUnitInfo.Requirements then
		AddTooltip(tPreWrittenLines, kUnitInfo.Requirements);
	end

	if next(tPreWrittenLines) then
		table.insert(tLines, table.concat(tPreWrittenLines, "[NEWLINE][NEWLINE]"));
	end

	return table.concat(tLines, "[NEWLINE]" .. SEPARATOR_STRING .. "[NEWLINE]");
end

--- Generate the tooltip for a building. Can be called from tech tree, city view, etc.<br>
--- The city view version includes most buffs the unit has received from techs, policies, buildings, etc., to closely reflect what could be built.
--- @param eBuilding integer
--- @param bExcludeName boolean
--- @param bNoMaintenance boolean
--- @param pCity City?
--- @param bGeneralInfo boolean? If true, don't compute any info that's player/city-specific. The `pCity` parameter is ignored. Defaults to true if the `Game` object doesn't exist.
--- @param bShowProjectedYields boolean? If true, show projected yields when this building is built in `pCity`.
--- @return string
function GetHelpTextForBuilding(eBuilding, bExcludeName, _, bNoMaintenance, pCity, bGeneralInfo, bShowProjectedYields)
	local kBuildingInfo = GameInfo.Buildings[eBuilding];

	--- @type Player?
	local pActivePlayer = Game and Players[Game.GetActivePlayer()];

	-- Only general info if Game object doesn't exist (e.g. in pregame)
	if not Game then
		bGeneralInfo = true;
	end

	-- When viewing a (foreign) city, always show tooltips as they are for the city owner
	if pCity then
		pActivePlayer = Players[pCity:GetOwner()];
	end

	local eActiveCiv = pActivePlayer and pActivePlayer:GetCivilizationType() or -1;

	-- Sometimes a city is needed in tooltips not in city view; in that case use the capital city
	local pActiveCity = pCity or pActivePlayer and pActivePlayer:GetCapitalCity();

	local kBuildingClassInfo = GameInfo.BuildingClasses[kBuildingInfo.BuildingClass];
	local eBuildingClass = kBuildingClassInfo.ID;

	local bIsWorldWonder = Game and Game.IsWorldWonderClass(eBuildingClass);
	local fTradeMultiplier = 1 + (pActivePlayer and pActivePlayer:GetTradeBuildingModifier() / 100 or 0);

	-- Invalidate pCity, pActivePlayer, pActiveCity if we only want general info (then we don't have to additionally check for bGeneralInfo on top of nil checks)
	if bGeneralInfo then
		pCity = nil;
		pActivePlayer = nil;
		pActiveCity = nil;
	end

	local tHasBuildingCache = {};
	local function CityHasBuilding(eCheckBuilding)
		if tHasBuildingCache[eCheckBuilding] == nil then
			tHasBuildingCache[eCheckBuilding] = pCity and pCity:HasBuilding(eCheckBuilding) or false;
		end
		return tHasBuildingCache[eCheckBuilding];
	end

	local function CanPlayerEverBuild(eCheckBuilding)
		if not pActivePlayer then
			return false;
		end

		local kCheckBuildingInfo = GameInfo.Buildings[eCheckBuilding];
		if kCheckBuildingInfo.IsDummy then
			return false;
		end

		if kCheckBuildingInfo.CivilizationRequired and GameInfoTypes[kCheckBuildingInfo.CivilizationRequired] ~= eActiveCiv then
			return false;
		end

		local eCheckBuildingClass = GameInfoTypes[kBuildingInfo.BuildingClass];
		if pActivePlayer:GetCivilizationBuilding(eCheckBuildingClass) ~= kBuildingInfo.ID then
			return false;
		end

		return true;
	end

	local tCanEverBuildCache = {};
	local function CanPlayerEverBuildCached(eCheckBuilding)
		if tCanEverBuildCache[eCheckBuilding] == nil then
			tCanEverBuildCache[eCheckBuilding] = CanPlayerEverBuild(eCheckBuilding);
		end
		return tCanEverBuildCache[eCheckBuilding];
	end

	--- Given a database table with BuildingType, <X>Type, YieldType and Yield, extract all entries of the given building/yield pair.
	--- Duplicate rows are overwritten.
	--- @param tYieldTable table<integer, table<integer, integer?>?> The yield table, could be empty or partially filled
	--- @param strDatabaseTable string The name of the database table to be extracted
	--- @param kYieldInfo table The yield entry
	--- @param strOtherColumn string The name of the other column (in the form of <X>Type)
	--- @param strYieldColumnName string|nil The name of the yield column (default "Yield" if nil)
	local function ExtractYieldTable(tYieldTable, strDatabaseTable, kYieldInfo, strOtherColumn, strYieldColumnName)
		local eYield = kYieldInfo.ID;
		strYieldColumnName = strYieldColumnName or "Yield";
		for row in GameInfo[strDatabaseTable]{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
			local eOtherType = GameInfoTypes[row[strOtherColumn]];
			tYieldTable[eOtherType] = tYieldTable[eOtherType] or {};
			tYieldTable[eOtherType][eYield] = row[strYieldColumnName];
		end
	end

	--- Given a database table with BuildingType, YieldType and Yield, extract all entries of the given building/yield pair.
	--- Duplicate rows are overwritten.
	--- @param tYieldTable table<integer, integer> The yield table, could be empty or partially filled
	--- @param strDatabaseTable string The name of the database table to be extracted
	--- @param kYieldInfo table The yield entry
	--- @param strYieldColumnName string|nil The name of the yield column (default "Yield" if nil)
	local function ExtractSimpleYieldTable(tYieldTable, strDatabaseTable, kYieldInfo, strYieldColumnName)
		local eYield = kYieldInfo.ID;
		strYieldColumnName = strYieldColumnName or "Yield";
		for row in GameInfo[strDatabaseTable]{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
			tYieldTable[eYield] = row[strYieldColumnName];
		end
	end

	--- Given a database table with BuildingType, <X>Type, YieldType, Yield and NumRequired, extract all entries of the given building/yield pair.
	--- Duplicate rows are overwritten.
	--- @param tYieldFractionTable table<integer, table<integer, table<string, integer>?>?> The yield table, could be empty or partially filled. Each entry is a table with Numerator and Denominator keys.
	--- @param strDatabaseTable string The name of the database table to be extracted
	--- @param kYieldInfo table The yield entry
	--- @param strOtherColumn string The name of the other column (in the form of <X>Type)
	--- @param strYieldColumnName string|nil The name of the yield column (default "Yield" if nil)
	--- @param strDenominatorColumnName string|nil The name of the denominator column (default "NumRequired" if nil)
	local function ExtractYieldFractionTable(tYieldFractionTable, strDatabaseTable, kYieldInfo, strOtherColumn, strYieldColumnName, strDenominatorColumnName)
		local eYield = kYieldInfo.ID;
		strYieldColumnName = strYieldColumnName or "Yield";
		strDenominatorColumnName = strDenominatorColumnName or "NumRequired";
		for row in GameInfo[strDatabaseTable]{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
			local eOtherType = GameInfoTypes[row[strOtherColumn]];
			tYieldFractionTable[eOtherType] = tYieldFractionTable[eOtherType] or {};
			tYieldFractionTable[eOtherType][eYield] = {Numerator = row[strYieldColumnName], Denominator = row[strDenominatorColumnName]};
		end
	end

	--- Given a database table with BuildingType, <X>Type, YieldType, Yield and NumRequired, extract all entries of the given building/yield pair.
	--- Duplicate rows are overwritten.
	--- @param tYieldFractionTable table<integer, table<string, integer>?> The yield table, could be empty or partially filled. Each entry is a table with Numerator and Denominator keys.
	--- @param strDatabaseTable string The name of the database table to be extracted
	--- @param kYieldInfo table The yield entry
	--- @param strYieldColumnName string|nil The name of the yield column (default "Yield" if nil)
	--- @param strDenominatorColumnName string|nil The name of the denominator column (default "NumRequired" if nil)
	local function ExtractSimpleYieldFractionTable(tYieldFractionTable, strDatabaseTable, kYieldInfo, strYieldColumnName, strDenominatorColumnName)
		local eYield = kYieldInfo.ID;
		strYieldColumnName = strYieldColumnName or "Yield";
		strDenominatorColumnName = strDenominatorColumnName or "NumRequired";
		for row in GameInfo[strDatabaseTable]{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
			tYieldFractionTable[eYield] = {Numerator = row[strYieldColumnName], Denominator = row[strDenominatorColumnName]};
		end
	end

	-- Extra player instances (done early since it'll be accessed in different areas)
	local tExtraInstanceLines = {};
	local iCurrentExtraInstances = 0;
	for eAccomplishment, kAccomplishmentInfo in GameInfoCache("Accomplishments") do
		local iCompleted = pActivePlayer and pActivePlayer:GetNumTimesAccomplishmentCompleted(eAccomplishment) or 0;
		local iNumInstance = 0;
		for row in GameInfo.Building_ExtraPlayerInstancesFromAccomplishments{BuildingType = kBuildingInfo.Type, AccomplishmentType = kAccomplishmentInfo.Type} do
			iNumInstance = row.ExtraInstances;
		end
		iCurrentExtraInstances = iCurrentExtraInstances + iNumInstance * iCompleted;
		if not (pCity and kAccomplishmentInfo.MaxPossibleCompletions and iCompleted >= kAccomplishmentInfo.MaxPossibleCompletions) then
			AddTooltipNonZero(tExtraInstanceLines, "TXT_KEY_PRODUCTION_BUILDING_PLAYER_INSTANCE_FROM_ACCOMPLISHMENT", iNumInstance, kAccomplishmentInfo.Description);
		end
	end

	local tLines = {};
	local tHeaderLines = {};
	local tStatLines = {};
	local tReqLines = {};
	local tAbilityLines = {};
	local tYieldLines = {};
	local tLocalAbilityLines = {};
	local tGlobalAbilityLines = {};
	local tTeamAbilityLines = {};
	local tNewMedianLines = {};
	local tCorporationAbilities = {};

	----------------------
	-- Header section
	----------------------
	local iInvestedCost = pCity and pCity:GetBuildingInvestment(eBuilding) or 0;

	-- Name
	if not bExcludeName then
		local strName = BuildingColor(Locale.ToUpper(L(kBuildingInfo.Description)));
		if pCity and MOD_BALANCE_BUILDING_INVESTMENTS then
			if iInvestedCost > 0 then
				strName = strName .. L("TXT_KEY_INVESTED");
			end
		end
		table.insert(tHeaderLines, strName);
	end

	-- Unique Building? Usually unique to one civ, but it's possible that multiple civs have access to the same building
	local tCivAdjectives = {};
	local bExclusive = false; -- If only one civ can build it but it is not an override of any building class, it is an exclusive building
	if kBuildingInfo.CivilizationRequired then
		bExclusive = true;
		tCivAdjectives = {L(GameInfo.Civilizations[kBuildingInfo.CivilizationRequired].Adjective)};
	end

	for row in GameInfo.Civilization_BuildingClassOverrides{BuildingType = kBuildingInfo.Type} do
		if row.CivilizationType == kBuildingInfo.CivilizationRequired then
			-- Only this civ is allowed to build this building, regardless of overrides
			tCivAdjectives = {L(GameInfo.Civilizations[row.CivilizationType].Adjective)};
			bExclusive = false;
			break;
		end
		table.insert(tCivAdjectives, L(GameInfo.Civilizations[row.CivilizationType].Adjective));
	end

	if next(tCivAdjectives) then
		if bExclusive then
			AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_EXCLUSIVE_BUILDING", tCivAdjectives[1]);
		else
			-- Get the building it is replacing
			local kDefaultBuildingInfo = GameInfo.Buildings[kBuildingClassInfo.DefaultBuilding];
			local strCivAdj = table.concat(tCivAdjectives, "/");
			if kDefaultBuildingInfo then
				AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_UNIQUE_BUILDING", strCivAdj, kDefaultBuildingInfo.Description);
			else
				-- This building isn't replacing anything
				AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_UNIQUE_BUILDING_NO_DEFAULT", strCivAdj);
			end
		end
	end

	-- Corporation building?
	local eCorporationFreeBuildingClass = -1;
	for kCorporationInfo in GameInfo.Corporations{HeadquartersBuildingClass = kBuildingClassInfo.Type} do
		AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_CORPORATION_HQ", kCorporationInfo.Description);
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_CORPORATION_BASE_FRANCHISE", kCorporationInfo.BaseFranchises);
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_CORPORATION_RANDOM_FRANCHISE_CHANCE", kCorporationInfo.RandomSpreadChance);

		eCorporationFreeBuildingClass = GameInfoTypes[kCorporationInfo.OfficeBuildingClass];

		AddTooltipNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_TRADE_ROUTES", kCorporationInfo.NumFreeTradeRoutes);

		local tBuildingClassBoosts = {};
		local tSpecialistBoosts = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			for row in GameInfo.Corporation_BuildingClassYieldChanges{CorporationType = kCorporationInfo.Type, YieldType = kYieldInfo.Type} do
				local eBoostedBuildingClass = GameInfoTypes[row.BuildingClassType];
				tBuildingClassBoosts[eBoostedBuildingClass] = tBuildingClassBoosts[eBoostedBuildingClass] or {};
				tBuildingClassBoosts[eBoostedBuildingClass][eYield] = row.YieldChange;
			end

			for row in GameInfo.Corporation_SpecialistYieldChanges{CorporationType = kCorporationInfo.Type, YieldType = kYieldInfo.Type} do
				local eSpecialist = GameInfoTypes[row.SpecialistType];
				tSpecialistBoosts[eSpecialist] = tSpecialistBoosts[eSpecialist] or {};
				tSpecialistBoosts[eSpecialist][eYield] = row.Yield;
			end
		end

		AddTooltipsYieldBoostTableGlobal(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_BOOST", tBuildingClassBoosts, "BuildingClasses");
		AddTooltipsYieldBoostTableGlobal(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_SPECIALIST_BOOST", tSpecialistBoosts, "Specialists");

		for row in GameInfo.Corporation_UnitResourceProductionModifier{CorporationType = kCorporationInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			AddTooltipNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_CORPORATION_SPECIFIC_STRATEGIC_UNIT_PRODUCTION_MODIFIER",
				row.Modifier, kResourceInfo.IconString, kResourceInfo.Description);
		end

		for row in GameInfo.Corporation_NumFreeResource{CorporationType = kCorporationInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			-- It's positive only but we want to reuse the text key
			if row.NumResource > 0 then
				AddTooltipNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_FREE_RESOURCE",
					row.NumResource, kResourceInfo.IconString, kResourceInfo.Description);
			end
		end

		AddTooltipGlobalNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_LAND_TRADE_ROUTE_RANGE_MODIFIER",
			kCorporationInfo.TradeRouteLandDistanceModifier);
		AddTooltipGlobalNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_SEA_TRADE_ROUTE_RANGE_MODIFIER",
			kCorporationInfo.TradeRouteSeaDistanceModifier);
		AddTooltipGlobalNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_ETR_DESTINATION_OUR_GOLD",
			kCorporationInfo.TradeRouteRecipientBonus * fTradeMultiplier);
		AddTooltipGlobalNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_ETR_DESTINATION_THEIR_GOLD",
			kCorporationInfo.TradeRouteTargetBonus * fTradeMultiplier);
		AddTooltipGlobalNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_TRADE_UNIT_EXTRA_SIGHT", kCorporationInfo.TradeRouteVisionBoost);
		AddTooltipGlobalNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_CORPORATION_FRANCHISE_TOURISM_MODIFIER",
			kCorporationInfo.TourismModNumFranchises);

		if kCorporationInfo.TradeRouteSpeedModifier > 0 and kCorporationInfo.TradeRouteSpeedModifier ~= 100 then
			AddTooltip(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_TRADE_UNIT_SPEED_MULTIPLIER", kCorporationInfo.TradeRouteSpeedModifier / 100);
		end

		AddTooltipIfTrue(tCorporationAbilities, "TXT_KEY_PRODUCTION_CORPORATION_TRADE_ROUTE_IMMUNITY", kCorporationInfo.TradeRoutesInvulnerable);

		-- Requirements
		local tReqMonopolies = {};
		for row in GameInfo.Corporation_ResourceMonopolyOrs{CorporationType = kCorporationInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			table.insert(tReqMonopolies, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
		end
		if next(tReqMonopolies) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_GLOBAL_MONOPOLY_OR", table.concat(tReqMonopolies, ", "));
		end
		tReqMonopolies = {};
		for row in GameInfo.Corporation_ResourceMonopolyAnds{CorporationType = kCorporationInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			table.insert(tReqMonopolies, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
		end
		if next(tReqMonopolies) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_GLOBAL_MONOPOLY_AND", table.concat(tReqMonopolies, ", "));
		end
	end

	for kCorporationInfo in GameInfo.Corporations{OfficeBuildingClass = kBuildingClassInfo.Type} do
		AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_CORPORATION_OFFICE", kCorporationInfo.Description);

		AddTooltipNonZeroSigned(tCorporationAbilities, "TXT_KEY_PRODUCTION_BUILDING_GPP_MODIFIER_FROM_CORPORATION", kBuildingInfo.GPRateModifierPerXFranchises);

		local tTradeRouteCityModifiers = {};
		local tTradeRouteYieldModifiers = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			for row in GameInfo.Corporation_TradeRouteCityYield{CorporationType = kCorporationInfo.Type, YieldType = kYieldInfo.Type} do
				tTradeRouteCityModifiers[eYield] = row.Yield;
			end

			for row in GameInfo.Corporation_TradeRouteMod{CorporationType = kCorporationInfo.Type, YieldType = kYieldInfo.Type} do
				tTradeRouteYieldModifiers[eYield] = row.Yield;
			end
		end

		AddTooltipSimpleYieldModifierTable(tCorporationAbilities,
			"TXT_KEY_PRODUCTION_CORPORATION_YIELD_MODIFIER_TR_WITH_FRANCHISE_CITY", tTradeRouteCityModifiers);
		AddTooltipSimpleYieldModifierTable(tCorporationAbilities,
			"TXT_KEY_PRODUCTION_CORPORATION_YIELD_MODIFIER_TR_WITH_FRANCHISE", tTradeRouteYieldModifiers);
	end

	for kCorporationInfo in GameInfo.Corporations{FranchiseBuildingClass = kBuildingClassInfo.Type} do
		AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_CORPORATION_FRANCHISE", kCorporationInfo.Description);

		local tResourceBoosts = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			for row in GameInfo.Corporation_ResourceYieldChanges{CorporationType = kCorporationInfo.Type, YieldType = kYieldInfo.Type} do
				local eResource = GameInfoTypes[row.ResourceType];
				tResourceBoosts[eResource] = tResourceBoosts[eResource] or {};
				tResourceBoosts[eResource][eYield] = row.Yield;
			end
		end

		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RESOURCE_BOOST", tResourceBoosts, "Resources");
	end

	-- World Congress project reward?
	for kRewardInfo in GameInfo.LeagueProjectRewards{Building = kBuildingInfo.Type} do
		for kProjectInfo in GameInfo.LeagueProjects{RewardTier1 = kRewardInfo.Type} do
			AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_WORLD_CONGRESS_PROJECT_REWARD_1", kProjectInfo.Description);
		end
		for kProjectInfo in GameInfo.LeagueProjects{RewardTier2 = kRewardInfo.Type} do
			AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_WORLD_CONGRESS_PROJECT_REWARD_2", kProjectInfo.Description);
		end
		for kProjectInfo in GameInfo.LeagueProjects{RewardTier3 = kRewardInfo.Type} do
			AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_WORLD_CONGRESS_PROJECT_REWARD_3", kProjectInfo.Description);
		end
	end

	if next(tHeaderLines) then
		table.insert(tLines, table.concat(tHeaderLines, "[NEWLINE]"));
	end

	----------------------
	-- Stats section
	----------------------

	-- Costs
	if Game and not bGeneralInfo and kBuildingInfo.FreeStartEra and Game.GetStartEra() >= GameInfoTypes[kBuildingInfo.FreeStartEra] then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_ON_FOUND");
	elseif kBuildingInfo.Cost > 0 or kBuildingInfo.FaithCost > 0 then
		local tCosts = {};
		local iProductionCost, iGoldCost, iFaithCost = 0, 0, 0;
		if kBuildingInfo.Cost > 0 then
			iProductionCost = (iInvestedCost > 0) and iInvestedCost or
				(pActiveCity and pActiveCity:GetBuildingProductionNeeded(eBuilding) or
				(pActivePlayer and pActivePlayer:GetBuildingProductionNeeded(eBuilding) or kBuildingInfo.Cost));
			iGoldCost = pActiveCity and pActiveCity:GetBuildingPurchaseCost(eBuilding) or 0;
		end

		if kBuildingInfo.FaithCost > 0 then
			iFaithCost = pActiveCity and pActiveCity:GetBuildingFaithPurchaseCost(eBuilding) or kBuildingInfo.FaithCost;
		end

		if not kBuildingInfo.PurchaseOnly then
			AddTooltipPositive(tCosts, "TXT_KEY_PRODUCTION_COST_PRODUCTION", iProductionCost);
		end
		AddTooltipPositive(tCosts, "TXT_KEY_PRODUCTION_COST_GOLD", iGoldCost);
		AddTooltipPositive(tCosts, "TXT_KEY_PRODUCTION_COST_FAITH", iFaithCost);

		if next(tCosts) then
			table.insert(tStatLines, L("TXT_KEY_PRODUCTION_COST_PREFIX", table.concat(tCosts, " / ")));
		end
	end

	if (not Game or bGeneralInfo) and kBuildingInfo.FreeStartEra then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_ON_FOUND_GENERIC", GameInfo.Eras[kBuildingInfo.FreeStartEra].Description);
	end

	if kBuildingInfo.UnlockedByLeague and not (Game and Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
		local pLeague = Game and not bGeneralInfo and Game.GetActiveLeague();
		if pLeague then
			local iCostPerPlayer = pLeague:GetProjectBuildingCostPerPlayer(eBuilding);
			AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_WORLD_CONGRESS_PROJECT_COST_PER_PLAYER", iCostPerPlayer);
		else
			local kLeagueRewardInfo = GameInfo.LeagueProjectRewards{Building = kBuildingInfo.Type}(); -- assume only one
			local kLeagueProjectInfo = kLeagueRewardInfo and GameInfo.LeagueProjects{RewardTier3 = kLeagueRewardInfo.Type}(); -- assume tier 3
			local iCostPerPlayer = kLeagueProjectInfo and kLeagueProjectInfo.CostPerPlayer or 0;
			AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_WORLD_CONGRESS_PROJECT_COST_PER_PLAYER", iCostPerPlayer);
		end
	end

	-- Maintenance
	if not bNoMaintenance then
		AddTooltipNonZero(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_MAINTENANCE", kBuildingInfo.GoldMaintenance);
	end

	-- Retained on conquest? Don't show if it's your city
	if not (pCity and Game.GetActivePlayer() == pCity:GetOwner()) then
		if kBuildingInfo.NeverCapture then
			AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_NEVER_CAPTURE");
		elseif kBuildingInfo.ConquestProb <= 0 then
			AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_CONQEUST_PROB_0");
		elseif kBuildingInfo.ConquestProb >= 100 then
			AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_CONQEUST_PROB_100");
		end
	end

	-- Current building count
	if pActivePlayer then
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_COUNT", pActivePlayer:GetBuildingClassCount(kBuildingClassInfo.ID),
			pActivePlayer:GetBuildingClassMaking(kBuildingClassInfo.ID));
	end

	-- Victory requirement
	for row in GameInfo.BuildingClass_VictoryThresholds{BuildingClassType = kBuildingClassInfo.Type} do
		AddTooltipPositive(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_VICTORY_REQUIREMENT", row.Threshold, GameInfo.Victories[row.VictoryType].Description);
	end

	-- Special session requirement
	for kSpecialSessionInfo in GameInfo.LeagueSpecialSessions{BuildingTrigger = kBuildingInfo.Type} do
		AddTooltip(tStatLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIAL_SESSION_REQUIREMENT",
			GameInfo.Resolutions[kSpecialSessionInfo.TriggerResolution].Description);
	end

	if next(tStatLines) then
		table.insert(tLines, table.concat(tStatLines, "[NEWLINE]"));
	end

	----------------------
	-- Yields and abilities section
	----------------------

	-- Yield change and yield modifier tooltips
	local tProjectedYields = {};
	local tProjectedModifiers = {};
	local bBoostedYields = false;
	local bBoostedModifiers = false;
	local bBoostedHappiness = false;
	local bIsCultureBuilding = false;
	for eYield, kYieldInfo in GameInfoCache("Yields") do
		-- Only show modified numbers in city view
		local iYield = Game and Game.GetBuildingYieldChange(eBuilding, eYield) or 0;
		if not Game then
			for row in GameInfo.Building_YieldChanges{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iYield = row.Yield;
			end
		end
		tProjectedYields[eYield] = pCity and pCity:GetBuildingYieldRateTimes100(eBuilding, eYield) / 100 or 0;
		AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_CHANGE", iYield, kYieldInfo.IconString, kYieldInfo.Description);
		if pCity and bShowProjectedYields and tProjectedYields[eYield] ~= iYield then
			bBoostedYields = true;
		end
		if eYield == GameInfoTypes.YIELD_CULTURE and iYield ~= 0 then
			bIsCultureBuilding = true;
		end

		local iModifier = Game and Game.GetBuildingYieldModifier(eBuilding, eYield) or 0;
		if not Game then
			for row in GameInfo.Building_YieldModifiers{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iModifier = row.Yield;
			end
		end
		tProjectedModifiers[eYield] = pCity and pCity:GetBuildingYieldModifier(eBuilding, eYield) or 0;
		if eYield == GameInfoTypes.YIELD_CULTURE_LOCAL then
			iModifier = iModifier + kBuildingInfo.BorderGrowthRateIncrease;
			tProjectedModifiers[eYield] = tProjectedModifiers[eYield] + kBuildingInfo.BorderGrowthRateIncrease;
		end
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_MODIFIER", iModifier, kYieldInfo.IconString, kYieldInfo.Description);
		if pCity and bShowProjectedYields and tProjectedModifiers[eYield] ~= iModifier then
			bBoostedModifiers = true;
		end

		local iGlobalMod = 0;
		for row in GameInfo.Building_GlobalYieldModifiers{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
			iGlobalMod = row.Yield;
			-- Not breaking here; taking the last row value like in DLL if duplicated
		end
		if eYield == GameInfoTypes.YIELD_CULTURE then
			iGlobalMod = iGlobalMod + kBuildingInfo.GlobalCultureRateModifier;
		elseif eYield == GameInfoTypes.YIELD_CULTURE_LOCAL then
			iGlobalMod = iGlobalMod + kBuildingInfo.BorderGrowthRateIncreaseGlobal;
		end
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_MODIFIER", iGlobalMod, kYieldInfo.IconString, kYieldInfo.Description);
	end

	-- Yield from % of other yield
	for row in GameInfo.Building_YieldFromYieldPercent{BuildingType = kBuildingInfo.Type} do
		local kYieldInInfo = GameInfo.Yields[row.YieldIn];
		local kYieldOutInfo = GameInfo.Yields[row.YieldOut];
		AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_FROM_OTHER_YIELD",
		row.Value, kYieldInInfo.IconString, kYieldInInfo.Description, kYieldOutInfo.IconString, kYieldOutInfo.Description);
	end
	for row in GameInfo.Building_YieldFromYieldPercentGlobal{BuildingType = kBuildingInfo.Type} do
		local kYieldInInfo = GameInfo.Yields[row.YieldIn];
		local kYieldOutInfo = GameInfo.Yields[row.YieldOut];
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_FROM_OTHER_YIELD_GLOBAL",
		row.Value, kYieldInInfo.IconString, kYieldInInfo.Description, kYieldOutInfo.IconString, kYieldOutInfo.Description);
	end

	-- Happiness (from all sources)
	local iHappinessTotal = kBuildingInfo.Happiness + kBuildingInfo.UnmoddedHappiness;
	if MOD_BALANCE_VP then
		AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS", iHappinessTotal);
		AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_UNHAPPINESS", kBuildingInfo.Unhappiness);
	else
		iHappinessTotal = iHappinessTotal + kBuildingInfo.Unhappiness;
		if iHappinessTotal > 0 then
			AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS", iHappinessTotal);
		else
			AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_UNHAPPINESS", iHappinessTotal * -1);
		end
	end

	-- Only show modified number in city view
	if pCity and pActivePlayer then
		local iBoostedHappiness = pCity:GetReligionBuildingClassHappiness(eBuildingClass)
			+ pActivePlayer:GetExtraBuildingHappinessFromPolicies(eBuilding)
			+ pActivePlayer:GetPlayerBuildingClassHappiness(eBuildingClass);
		if (iBoostedHappiness > 0) then
			bBoostedHappiness = true
			iHappinessTotal = iHappinessTotal + iBoostedHappiness
		end
	end

	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_GLOBAL", kBuildingInfo.HappinessPerCity);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_POLICY_CAPITAL", kBuildingInfo.HappinessPerXPolicies);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_WAR", kBuildingInfo.GlobalHappinessPerMajorWar);

	-- Defense
	AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_DEFENSE", kBuildingInfo.Defense / 100);
	AddTooltipPositive(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_DEFENSE_FROM_WONDERS", kBuildingInfo.DefensePerXWonder);

	-- Hit points
	AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_HITPOINTS", kBuildingInfo.ExtraCityHitPoints);

	-- Damage reduction
	AddTooltipNonZeroSigned(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_DAMAGE_REDUCTION", kBuildingInfo.DamageReductionFlat);

	-- Air defense
	AddTooltipPositive(tYieldLines, "TXT_KEY_PRODUCTION_BUILDING_AIR_DEFENSE", kBuildingInfo.CityAirStrikeDefense);

	-- Great People Points and specialist slots
	if kBuildingInfo.SpecialistType then
		local kSpecialistInfo = GameInfo.Specialists[kBuildingInfo.SpecialistType];
		local iNumPoints = kBuildingInfo.GreatPeopleRateChange;
		if iNumPoints > 0 then
			table.insert(tYieldLines, string.format("[ICON_BULLET][ICON_GREAT_PEOPLE] %s %d", L(kSpecialistInfo.GreatPeopleTitle), iNumPoints));
		end

		local iNumSlots = kBuildingInfo.SpecialistCount;
		if iNumSlots > 0 then
			-- Append a key such as TXT_KEY_SPECIALIST_ARTIST_SLOTS
			local strSpecialistSlotsKey = kSpecialistInfo.Description .. "_SLOTS";
			table.insert(tYieldLines, string.format("[ICON_BULLET][ICON_GREAT_PEOPLE] %s %d", L(strSpecialistSlotsKey), iNumSlots));
		end
	end

	-- Great Work slots
	if kBuildingInfo.GreatWorkSlotType then
		AddTooltipPositive(tYieldLines, GameInfo.GreatWorkSlots[kBuildingInfo.GreatWorkSlotType].SlotsToolTipText, kBuildingInfo.GreatWorkCount);
		if kBuildingInfo.GreatWorkYieldType and kBuildingInfo.GreatWorkYieldType ~= "YIELD_CULTURE" then
			local kYieldInfo = GameInfo.Yields[kBuildingInfo.GreatWorkYieldType];
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GREAT_WORK_YIELD_TYPE", kYieldInfo.IconString, kYieldInfo.Description);
		end
	end

	local iMaxThemingBonus = 0;
	for row in GameInfo.Building_ThemingBonuses{BuildingType = kBuildingInfo.Type} do
		iMaxThemingBonus = math.max(iMaxThemingBonus, row.Bonus);
	end
	-- Theming bonus (not yield bonus) is affected by modifiers
	if pCity and iMaxThemingBonus ~= 0 then
		iMaxThemingBonus = math.floor(iMaxThemingBonus * pCity:GetThemingBonusMultiplier());
	end

	-- Consolidate theming bonus and theming yield bonus
	local tThemingYields = {};
	for eYield, kYieldInfo in GameInfoCache("Yields") do
		ExtractSimpleYieldTable(tThemingYields, "Building_ThemingYieldBonus", kYieldInfo);
		if kYieldInfo.Type == kBuildingInfo.GreatWorkYieldType or kYieldInfo.Type == "YIELD_TOURISM" then
			tThemingYields[eYield] = tThemingYields[eYield] or 0;
			tThemingYields[eYield] = tThemingYields[eYield] + iMaxThemingBonus;
		end
	end
	AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_THEMING_BONUS", tThemingYields);

	-- Defense modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_DEFENSE_MODIFIER", kBuildingInfo.BuildingDefenseModifier);
	AddTooltipNonZeroSigned(tTeamAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_DEFENSE_MODIFIER_TEAM", kBuildingInfo.GlobalDefenseMod);

	-- Food modifier per follower
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FOOD_MODIFIER_PER_FOLLOWER", kBuildingInfo.FoodBonusPerCityMajorityFollower);

	-- Great Person rate
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GPP_MODIFIER", kBuildingInfo.GreatPeopleRateModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GPP_MODIFIER", kBuildingInfo.GlobalGreatPeopleRateModifier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GPP_MODIFIER_FROM_MARRIAGE", kBuildingInfo.GPRateModifierPerMarriage);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GPP_MODIFIER_FROM_LOCAL_THEME", kBuildingInfo.GPRateModifierPerLocalTheme);

	for row in GameInfo.Building_SpecificGreatPersonRateModifier{BuildingType = kBuildingInfo.Type} do
		local kGreatPersonInfo = GetGreatPersonInfoFromSpecialist(row.SpecialistType);
		AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIFIC_GPP_MODIFIER", row.Modifier, kGreatPersonInfo.IconString, kGreatPersonInfo.Description);
	end

	-- Great General rate
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GGP_MODIFIER", kBuildingInfo.GreatGeneralRateModifier);

	-- Golden Age length modifier
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GODLEN_AGE_LENGTH_MODIFIER", kBuildingInfo.GoldenAgeModifier);

	-- Unit upgrade cost modifier
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_UPGRADE_COST_MODIFIER", kBuildingInfo.UnitUpgradeCostMod);

	-- Trained unit experience
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_ALL", kBuildingInfo.Experience);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_ALL", kBuildingInfo.GlobalExperience);
	for row in GameInfo.Building_UnitCombatFreeExperiences{BuildingType = kBuildingInfo.Type} do
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_COMBAT", row.Experience, GameInfo.UnitCombatInfos[row.UnitCombatType].Description);
	end
	do
		local tXPDomains = {};
		for row in GameInfo.Building_DomainFreeExperiences{BuildingType = kBuildingInfo.Type} do
			if row.Experience ~= 0 then
				if not tXPDomains[row.Experience] then tXPDomains[row.Experience] = {} end
				table.insert(tXPDomains[row.Experience], "[COLOR_YELLOW]" .. L(GameInfo.Domains[row.DomainType].Description) .. "[ENDCOLOR]");
			end
		end
		for iXP, tDomains in pairs(tXPDomains) do
			local strSign = iXP > 0 and "+" or "-";
			table.insert(tLocalAbilityLines, L("TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_DOMAINS", math.abs(iXP), strSign, ConcatWithCommaAnd(tDomains)));
		end
	end
	do
		local tXPDomains = {};
		for row in GameInfo.Building_DomainFreeExperiencesGlobal{BuildingType = kBuildingInfo.Type} do
			if row.Experience ~= 0 then
				if not tXPDomains[row.Experience] then tXPDomains[row.Experience] = {} end
				table.insert(tXPDomains[row.Experience], "[COLOR_YELLOW]" .. L(GameInfo.Domains[row.DomainType].Description) .. "[ENDCOLOR]");
			end
		end
		for iXP, tDomains in pairs(tXPDomains) do
			local strSign = iXP > 0 and "+" or "-";
			table.insert(tGlobalAbilityLines, AppendGlobal(L("TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_DOMAINS", math.abs(iXP), strSign, ConcatWithCommaAnd(tDomains))));
		end
	end
	do
		local tXPDomains = {};
		for row in GameInfo.Building_DomainFreeExperiencePerGreatWork{BuildingType = kBuildingInfo.Type} do
			if row.Experience ~= 0 then
				if not tXPDomains[row.Experience] then tXPDomains[row.Experience] = {} end
				table.insert(tXPDomains[row.Experience], "[COLOR_YELLOW]" .. L(GameInfo.Domains[row.DomainType].Description) .. "[ENDCOLOR]");
			end
		end
		for iXP, tDomains in pairs(tXPDomains) do
			local strSign = iXP > 0 and "+" or "-";
			table.insert(tLocalAbilityLines, L("TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_DOMAINS_FROM_GREAT_WORK", math.abs(iXP), strSign, ConcatWithCommaAnd(tDomains)));
		end
	end
	do
		local tXPDomains = {};
		for row in GameInfo.Building_DomainFreeExperiencePerGreatWorkGlobal{BuildingType = kBuildingInfo.Type} do
			if row.Experience ~= 0 then
				if not tXPDomains[row.Experience] then tXPDomains[row.Experience] = {} end
				table.insert(tXPDomains[row.Experience], "[COLOR_YELLOW]" .. L(GameInfo.Domains[row.DomainType].Description) .. "[ENDCOLOR]");
			end
		end
		for iXP, tDomains in pairs(tXPDomains) do
			local strSign = iXP > 0 and "+" or "-";
			table.insert(tGlobalAbilityLines, AppendGlobal(L("TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_DOMAINS_FROM_GREAT_WRITING_GLOBAL", math.abs(iXP), strSign, ConcatWithCommaAnd(tDomains))));
		end
	end
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_XP_ON_GOLDEN_AGE_START", kBuildingInfo.ExperiencePerGoldenAge, kBuildingInfo.ExperiencePerGoldenAgeCap);

	-- Food carried over on growth
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FOOD_KEPT", kBuildingInfo.FoodKept);

	-- Supply
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SUPPLY_FLAT", kBuildingInfo.CitySupplyFlat);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SUPPLY_MODIFIER", kBuildingInfo.CitySupplyModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SUPPLY_FLAT", kBuildingInfo.CitySupplyFlatGlobal);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SUPPLY_MODIFIER", kBuildingInfo.CitySupplyModifierGlobal);

	-- City security
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_SECURITY", kBuildingInfo.SpySecurityModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_SECURITY", kBuildingInfo.GlobalSpySecurityModifier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_SECURITY_PER_X_POPULATION", kBuildingInfo.SpySecurityModifierPerXPop / ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER);

	-- City strike strength
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_STRIKE_STRENGTH_MODIFIER", kBuildingInfo.RangedStrikeModifier);

	-- City strike range
	if MOD_BALANCE_BOMBARD_RANGE_BUILDINGS then
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_STRIKE_RANGE", kBuildingInfo.CityRangedStrikeRange);
		AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INDIRECT_FIRE", kBuildingInfo.CityIndirectFire);
	end

	-- City working range
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WORKING_RANGE", kBuildingInfo.CityWorkingChange);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WORKING_RANGE", kBuildingInfo.GlobalCityWorkingChange);

	-- Air slots
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_AIR_SLOTS", kBuildingInfo.AirModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_AIR_SLOTS", kBuildingInfo.AirModifierGlobal);

	-- Cooldown reduction for unit purchase/investment
	if MOD_BALANCE_UNIT_INVESTMENTS then
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INVEST_MILITARY_COOLDOWN_MODIFIER", kBuildingInfo.PurchaseCooldownReduction * -1);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INVEST_CIVILIAN_COOLDOWN_MODIFIER", kBuildingInfo.PurchaseCooldownReductionCivilian * -1);
	else
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PURCHASE_MILITARY_COOLDOWN_MODIFIER", kBuildingInfo.PurchaseCooldownReduction * -1);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PURCHASE_CIVILIAN_COOLDOWN_MODIFIER", kBuildingInfo.PurchaseCooldownReductionCivilian * -1);
	end

	-- Missionary spread actions
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPREAD_ACTION", kBuildingInfo.ExtraMissionarySpreads);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPREAD_ACTION_GLOBAL", kBuildingInfo.ExtraMissionarySpreadsGlobal);

	-- Religious strength modifier
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPREAD_STRENGTH_MODIFIER_GLOBAL", kBuildingInfo.ExtraMissionaryStrengthGlobal);

	-- Pressure modifier
	local strPressureModKey = kBuildingInfo.UnlockedByBelief and "TXT_KEY_PRODUCTION_BUILDING_PRESSURE_MODIFIER_IF_MAJORITY" or "TXT_KEY_PRODUCTION_BUILDING_PRESSURE_MODIFIER";
	AddTooltipNonZeroSigned(tLocalAbilityLines, strPressureModKey, kBuildingInfo.ReligiousPressureModifier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PRESSURE_MODIFIER_IF_TRADE_ROUTE", kBuildingInfo.TradeReligionModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BASE_PRESSURE_MODIFIER", kBuildingInfo.BasePressureModifierGlobal);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PRESSURE_RECEIVED_MODIFIER", kBuildingInfo.ConversionModifier);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PRESSURE_RECEIVED_MODIFIER_GLOBAL", kBuildingInfo.GlobalConversionModifier);

	-- Defensive espionage modifier
	if not MOD_BALANCE_VP then
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ESPIONAGE_MODIFIER", kBuildingInfo.EspionageModifier);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ESPIONAGE_MODIFIER", kBuildingInfo.GlobalEspionageModifier);
	end

	-- Diplomatic mission influence change
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INFLUENCE_CHANGE", kBuildingInfo.DiplomatInfluenceBoost);

	-- Garrison ranged attack modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GARRISON_RANGED_ATTACK_MODIFIER", kBuildingInfo.GarrisonRangedAttackModifier);

	-- Garrison extra heal
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GARRISON_HEAL_RATE", kBuildingInfo.HealRateChange);

	-- Garrison always heal
	AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GARRISON_HEAL_RATE_ALWAYS", kBuildingInfo.AlwaysHeal);

	-- Nuke resistance
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_NUKE_RESISTANCE", kBuildingInfo.NukeModifier);

	-- Nuke interception chance
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_NUKE_INTERCEPTION", kBuildingInfo.NukeInterceptionChance);

	-- Work rate modifier
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WORK_RATE_MODIFIER", kBuildingInfo.WorkerSpeedModifier);

	-- Building production modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_PRODUCTION_MODIFIER", kBuildingInfo.BuildingProductionModifier);

	-- Wonder production modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WONDER_PRODUCTION_MODIFIER", kBuildingInfo.WonderProductionModifier);

	-- Military unit production modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_MILITARY_PRODUCTION_MODIFIER", kBuildingInfo.MilitaryProductionModifier);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_MILITARY_PRODUCTION_MODIFIER_FROM_WAR", kBuildingInfo.GlobalMilitaryProductionModPerMajorWar);

	-- Spaceship part production modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPACESHIP_PRODUCTION_MODIFIER", kBuildingInfo.SpaceProductionModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPACESHIP_PRODUCTION_MODIFIER", kBuildingInfo.GlobalSpaceProductionModifier);

	-- Specific unit production modifier
	for row in GameInfo.Unit_ProductionModifierBuildings{BuildingType = kBuildingInfo.Type} do
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIFIC_UNIT_PRODUCTION_MODIFIER", row.ProductionModifier,
			GameInfo.Units[row.UnitType].Description);
	end

	-- Specific unit combat production modifier
	for row in GameInfo.Building_UnitCombatProductionModifiers{BuildingType = kBuildingInfo.Type} do
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIFIC_UNIT_COMBAT_PRODUCTION_MODIFIER", row.Modifier,
			GameInfo.UnitCombatInfos[row.UnitCombatType].Description);
	end
	for row in GameInfo.Building_UnitCombatProductionModifiersGlobal{BuildingType = kBuildingInfo.Type} do
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIFIC_UNIT_COMBAT_PRODUCTION_MODIFIER", row.Modifier,
			GameInfo.UnitCombatInfos[row.UnitCombatType].Description);
	end

	-- Specific domain production modifier
	for row in GameInfo.Building_DomainProductionModifiers{BuildingType = kBuildingInfo.Type} do
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIFIC_DOMAIN_PRODUCTION_MODIFIER", row.Modifier, 			GameInfo.Domains[row.DomainType].Description);
	end

	-- City connection modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_CONNECTION_MODIFIER", kBuildingInfo.CityConnectionGoldModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_CONNECTION_MODIFIER", kBuildingInfo.CityConnectionTradeRouteModifier);

	-- Policy cost modifier
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_POLICY_COST_MODIFIER", kBuildingInfo.PolicyCostModifier);

	-- Building maintenance modifier
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_MAINTENANCE_MODIFIER", kBuildingInfo.GlobalBuildingGoldMaintenanceMod);

	-- Border growth cost modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BORDER_GROWTH_COST_MODIFIER", kBuildingInfo.PlotCultureCostModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BORDER_GROWTH_COST_MODIFIER", kBuildingInfo.GlobalPlotCultureCostModifier);

	-- Purchase/invest cost modifier
	for row in GameInfo.Building_HurryModifiersLocal{BuildingType = kBuildingInfo.Type, HurryType = "HURRY_GOLD"} do
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PURCHASE_COST_MODIFIER", row.HurryCostModifier);
	end
	for row in GameInfo.Building_HurryModifiers{BuildingType = kBuildingInfo.Type, HurryType = "HURRY_GOLD"} do
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PURCHASE_COST_MODIFIER", row.HurryCostModifier);
	end

	-- Plot purchase cost modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PLOT_PURCHASE_COST_MODIFIER", kBuildingInfo.PlotBuyCostModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PLOT_PURCHASE_COST_MODIFIER", kBuildingInfo.GlobalPlotBuyCostModifier);

	-- Tile/World Wonder culture to tourism
	AddTooltipNonZero(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CULTURE_TO_TOURISM_TERRAIN_WONDER", kBuildingInfo.LandmarksTourismPercent);
	AddTooltipGlobalNonZero(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CULTURE_TO_TOURISM_TERRAIN_WONDER", kBuildingInfo.GlobalLandmarksTourismPercent);

	-- Tourism from Great Works modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GREAT_WORK_TOURISM_MODIFIER", kBuildingInfo.GreatWorksTourismModifier);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GREAT_WORK_TOURISM_MODIFIER", kBuildingInfo.GlobalGreatWorksTourismModifier);

	-- Production modifier from trade routes with city states
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PRODUCTION_MODIFIER_FROM_MINOR_TRADE_ROUTES", kBuildingInfo.CityStateTradeRouteProductionModifier);

	-- Discover tech science modifier
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_DISCOVER_TECH_SCIENCE_MODIFIER", kBuildingInfo.GreatScientistBeakerModifier);

	-- "Nullify" tech tourism modifier from other players
	if kBuildingInfo.NullifyInfluenceModifier then
		for kTechInfo in GameInfo.Technologies("InfluenceSpreadModifier > 0") do
			AddTooltip(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_NULLIFY_TECH_TOURISM_MODIFIER", kTechInfo.InfluenceSpreadModifier, kTechInfo.Description);
		end
	end

	-- Need modifiers
	if MOD_BALANCE_VP then
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BASIC_NEEDS_MEDIAN_MODIFIER", kBuildingInfo.BasicNeedsMedianModifier);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GOLD_MEDIAN_MODIFIER", kBuildingInfo.GoldMedianModifier);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SCIENCE_MEDIAN_MODIFIER", kBuildingInfo.ScienceMedianModifier);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CULTURE_MEDIAN_MODIFIER", kBuildingInfo.CultureMedianModifier);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RELIGIOUS_UNREST_MODIFIER", kBuildingInfo.ReligiousUnrestModifier);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EMPIRE_SIZE_MODIFIER", kBuildingInfo.EmpireSizeModifierReduction);

		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BASIC_NEEDS_MEDIAN_MODIFIER", kBuildingInfo.BasicNeedsMedianModifierGlobal);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GOLD_MEDIAN_MODIFIER", kBuildingInfo.GoldMedianModifierGlobal);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SCIENCE_MEDIAN_MODIFIER", kBuildingInfo.ScienceMedianModifierGlobal);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_CULTURE_MEDIAN_MODIFIER", kBuildingInfo.CultureMedianModifierGlobal);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RELIGIOUS_UNREST_MODIFIER", kBuildingInfo.ReligiousUnrestModifierGlobal);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EMPIRE_SIZE_MODIFIER", kBuildingInfo.EmpireSizeModifierReductionGlobal);

		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_DISTRESS_REDUCTION", kBuildingInfo.DistressFlatReduction * -1);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_POVERTY_REDUCTION", kBuildingInfo.PovertyFlatReduction * -1);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ILLITERACY_REDUCTION", kBuildingInfo.IlliteracyFlatReduction * -1);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BOREDOM_REDUCTION", kBuildingInfo.BoredomFlatReduction * -1);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RELIGIOUS_UNREST_REDUCTION", kBuildingInfo.ReligiousUnrestFlatReduction * -1);

		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_DISTRESS_REDUCTION", kBuildingInfo.DistressFlatReductionGlobal * -1);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_POVERTY_REDUCTION", kBuildingInfo.PovertyFlatReductionGlobal * -1);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ILLITERACY_REDUCTION", kBuildingInfo.IlliteracyFlatReductionGlobal * -1);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BOREDOM_REDUCTION", kBuildingInfo.BoredomFlatReductionGlobal * -1);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RELIGIOUS_UNREST_REDUCTION", kBuildingInfo.ReligiousUnrestFlatReductionGlobal * -1);

		if not OptionsManager.IsNoBasicHelp() then
			if pCity then
				local iBasicModifierTotal = kBuildingInfo.BasicNeedsMedianModifier + kBuildingInfo.BasicNeedsMedianModifierGlobal;
				if iBasicModifierTotal ~= 0 then
					local iNewMedian = pCity:GetTheoreticalNewBasicNeedsMedian(eBuilding) / 100;
					local iOldMedian = pCity:GetBasicNeedsMedian() / 100;
					if iNewMedian ~= 0 or iOldMedian ~= 0 then
						AddTooltip(tNewMedianLines, "TXT_KEY_PRODUCTION_BUILDING_BASIC_NEEDS_NEW_MEDIAN", iNewMedian, iOldMedian);
					end
				end
				local iGoldModifierTotal = kBuildingInfo.GoldMedianModifier + kBuildingInfo.GoldMedianModifierGlobal;
				if iGoldModifierTotal ~= 0 then
					local iNewMedian = pCity:GetTheoreticalNewGoldMedian(eBuilding) / 100;
					local iOldMedian = pCity:GetGoldMedian() / 100;
					if iNewMedian ~= 0 or iOldMedian ~= 0 then
						AddTooltip(tNewMedianLines, "TXT_KEY_PRODUCTION_BUILDING_GOLD_NEW_MEDIAN", iNewMedian, iOldMedian);
					end
				end
				local iScienceModifierTotal = kBuildingInfo.ScienceMedianModifier + kBuildingInfo.ScienceMedianModifierGlobal;
				if iScienceModifierTotal ~= 0 then
					local iNewMedian = pCity:GetTheoreticalNewScienceMedian(eBuilding) / 100;
					local iOldMedian = pCity:GetScienceMedian() / 100;
					if iNewMedian ~= 0 or iOldMedian ~= 0 then
						AddTooltip(tNewMedianLines, "TXT_KEY_PRODUCTION_BUILDING_SCIENCE_NEW_MEDIAN", iNewMedian, iOldMedian);
					end
				end
				local iCultureModifierTotal = kBuildingInfo.CultureMedianModifier + kBuildingInfo.CultureMedianModifierGlobal;
				if iCultureModifierTotal ~= 0 then
					local iNewMedian = pCity:GetTheoreticalNewCultureMedian(eBuilding) / 100;
					local iOldMedian = pCity:GetCultureMedian() / 100;
					if iNewMedian ~= 0 or iOldMedian ~= 0 then
						AddTooltip(tNewMedianLines, "TXT_KEY_PRODUCTION_BUILDING_CULTURE_NEW_MEDIAN", iNewMedian, iOldMedian);
					end
				end
				local iReligionModifierTotal = kBuildingInfo.ReligiousUnrestModifier + kBuildingInfo.ReligiousUnrestModifierGlobal;
				if iReligionModifierTotal ~= 0 then
					local iNewUnhappyPerPop = pCity:GetTheoreticalNewReligiousUnrestPerMinorityFollower(eBuilding) / 100;
					local iOldUnhappyPerPop = pCity:GetReligiousUnrestPerMinorityFollower(eBuilding) / 100;
					if iNewUnhappyPerPop ~= 0 or iOldUnhappyPerPop ~= 0 then
						AddTooltip(tNewMedianLines, "TXT_KEY_PRODUCTION_BUILDING_RELIGIOUS_UNREST_NEW_THRESHOLD", iNewUnhappyPerPop, iOldUnhappyPerPop);
					end
				end
			end
		end
	else
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNHAPPINESS_MODIFIER_LOCAL", kBuildingInfo.LocalUnhappinessModifier);
		AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNHAPPINESS_MODIFIER", kBuildingInfo.UnhappinessModifier);
		AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNHAPPINESS_MODIFIER_CITY_COUNT", kBuildingInfo.CityCountUnhappinessMod);
	end

	-- Yield boosts on other components + yield modifiers
	local tResourceModifiers = {};
	local tAreaBoosts = {};
	do
		local tSpecialistBoosts = {};
		local tSpecialistBoostsGlobal = {};
		local tBuildingClassBoosts = {};
		local tBuildingClassBoostsGlobal = {};
		local tBuildingModifiers = {};
		local tWorldWonderBoosts = {};
		local tWorldWonderBoostsGlobal = {};
		local tPlotBoosts = {};
		local tTerrainBoosts = {};
		local tFeatureBoosts = {};
		local tResourceBoosts = {};
		local tResourceBoostsGlobal = {};
		local tImprovementBoosts = {};
		local tImprovementBoostsGlobal = {};
		local tLuxuryBoosts = {};
		local tRiverBoosts = {};
		local tLakeBoosts = {};
		local tLakeBoostsGlobal = {};
		local tSeaBoosts = {};
		local tWaterResourceBoosts = {};
		local tGreatWorkBoosts = {};
		local tGreatWorkBoostsGlobal = {};
		local tGoldenAgeModifiers = {};
		local tWLTKDModifiers = {};
		local tInternalTRBoosts = {};
		for _, kYieldInfo in GameInfoCache("Yields") do
			ExtractYieldTable(tSpecialistBoosts, "Building_SpecialistYieldChangesLocal", kYieldInfo, "SpecialistType");
			ExtractYieldTable(tSpecialistBoostsGlobal, "Building_SpecialistYieldChanges", kYieldInfo, "SpecialistType");
			ExtractYieldTable(tBuildingClassBoosts, "Building_BuildingClassLocalYieldChanges", kYieldInfo, "BuildingClassType", "YieldChange");
			ExtractYieldTable(tBuildingClassBoostsGlobal, "Building_BuildingClassYieldChanges", kYieldInfo, "BuildingClassType", "YieldChange");
			ExtractYieldTable(tBuildingModifiers, "Building_BuildingClassYieldModifiers", kYieldInfo, "BuildingClassType", "Modifier");
			ExtractSimpleYieldTable(tWorldWonderBoosts, "Building_YieldChangeWorldWonder", kYieldInfo);
			ExtractSimpleYieldTable(tWorldWonderBoostsGlobal, "Building_YieldChangeWorldWonderGlobal", kYieldInfo);
			ExtractSimpleYieldTable(tAreaBoosts, "Building_AreaYieldModifiers", kYieldInfo);
			ExtractYieldTable(tPlotBoosts, "Building_PlotYieldChanges", kYieldInfo, "PlotType");
			ExtractYieldTable(tTerrainBoosts, "Building_TerrainYieldChanges", kYieldInfo, "TerrainType");
			ExtractYieldTable(tFeatureBoosts, "Building_FeatureYieldChanges", kYieldInfo, "FeatureType");
			ExtractYieldTable(tResourceBoosts, "Building_ResourceYieldChanges", kYieldInfo, "ResourceType");
			ExtractYieldTable(tResourceBoostsGlobal, "Building_ResourceYieldChangesGlobal", kYieldInfo, "ResourceType");
			ExtractYieldTable(tImprovementBoosts, "Building_ImprovementYieldChanges", kYieldInfo, "ImprovementType");
			ExtractYieldTable(tImprovementBoostsGlobal, "Building_ImprovementYieldChangesGlobal", kYieldInfo, "ImprovementType");
			ExtractSimpleYieldTable(tLuxuryBoosts, "Building_LuxuryYieldChanges", kYieldInfo);
			ExtractSimpleYieldTable(tRiverBoosts, "Building_RiverPlotYieldChanges", kYieldInfo);
			ExtractSimpleYieldTable(tLakeBoosts, "Building_LakePlotYieldChanges", kYieldInfo);
			ExtractSimpleYieldTable(tLakeBoostsGlobal, "Building_LakePlotYieldChangesGlobal", kYieldInfo);
			ExtractSimpleYieldTable(tSeaBoosts, "Building_SeaPlotYieldChanges", kYieldInfo);
			ExtractSimpleYieldTable(tWaterResourceBoosts, "Building_SeaResourceYieldChanges", kYieldInfo);
			ExtractSimpleYieldTable(tGreatWorkBoosts, "Building_GreatWorkYieldChangesLocal", kYieldInfo);
			ExtractSimpleYieldTable(tGreatWorkBoostsGlobal, "Building_GreatWorkYieldChanges", kYieldInfo);
			ExtractSimpleYieldTable(tGoldenAgeModifiers, "Building_GoldenAgeYieldMod", kYieldInfo);
			ExtractSimpleYieldTable(tWLTKDModifiers, "Building_WLTKDYieldMod", kYieldInfo);
			ExtractYieldTable(tResourceModifiers, "Building_ResourceYieldModifiers", kYieldInfo, "ResourceType");
		end

		for row in GameInfo.Building_YieldFromInternalTR{BuildingType = kBuildingInfo.Type} do
			if row.YieldType == "YIELD_FOOD" or row.YieldType == "YIELD_PRODUCTION" then
				tInternalTRBoosts[GameInfoTypes[row.YieldType]] = row.Yield;
			end
		end

		if kBuildingInfo.SpecialistExtraCulture ~= 0 then
			for eSpecialist in GameInfoCache("Specialists") do
				local eYield = GameInfoTypes.YIELD_CULTURE;
				tSpecialistBoostsGlobal[eSpecialist] = tSpecialistBoostsGlobal[eSpecialist] or {};
				tSpecialistBoostsGlobal[eSpecialist][eYield] = tSpecialistBoostsGlobal[eSpecialist][eYield] or 0;
				tSpecialistBoostsGlobal[eSpecialist][eYield] = tSpecialistBoostsGlobal[eSpecialist][eYield] + kBuildingInfo.SpecialistExtraCulture;
			end
		end

		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIALIST_BOOST", tSpecialistBoosts, "Specialists");
		AddTooltipsYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPECIALIST_BOOST", tSpecialistBoostsGlobal, "Specialists");
		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_BOOST", tBuildingClassBoosts, "BuildingClasses");
		AddTooltipsYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_BOOST", tBuildingClassBoostsGlobal, "BuildingClasses");
		AddTooltipsYieldModifierTableGlobal(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_BOOST", tBuildingModifiers, "BuildingClasses");
		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PLOT_BOOST", tPlotBoosts, "Plots");
		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TERRAIN_BOOST", tTerrainBoosts, "Terrains");
		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FEATURE_BOOST", tFeatureBoosts, "Features");
		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RESOURCE_BOOST", tResourceBoosts, "Resources");
		AddTooltipsYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RESOURCE_BOOST", tResourceBoostsGlobal, "Resources");
		AddTooltipsYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_IMPROVEMENT_BOOST", tImprovementBoosts, "Improvements");
		AddTooltipsYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_IMPROVEMENT_BOOST", tImprovementBoostsGlobal, "Improvements");
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WORLD_WONDER_BOOST", tWorldWonderBoosts);
		AddTooltipSimpleYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WORLD_WONDER_BOOST", tWorldWonderBoostsGlobal);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_LUXURY_BOOST", tLuxuryBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RIVER_BOOST", tRiverBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_LAKE_BOOST", tLakeBoosts);
		AddTooltipSimpleYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_LAKE_BOOST", tLakeBoostsGlobal);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SEA_BOOST", tSeaBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WATER_RESOURCE_BOOST", tWaterResourceBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GREAT_WORK_BOOST", tGreatWorkBoosts);
		AddTooltipSimpleYieldBoostTableGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GREAT_WORK_BOOST", tGreatWorkBoostsGlobal);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ITR_BOOST", tInternalTRBoosts);
		AddTooltipSimpleYieldModifierTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_MODIFIER_GOLDEN_AGE", tGoldenAgeModifiers);
		AddTooltipSimpleYieldModifierTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_MODIFIER_WLTKD", tWLTKDModifiers);

		-- Process boosts (both conversion rate and need modifier)
		for row in GameInfo.Building_YieldFromProcessModifier{BuildingType = kBuildingInfo.Type} do
			for row2 in GameInfo.Process_ProductionYields{YieldType = row.YieldType} do
				local kProcessInfo = GameInfo.Processes[row2.ProcessType];
				local kYieldInfo = GameInfo.Yields[row.YieldType];
				local strYields;
				local strTextKey = "TXT_KEY_PRODUCTION_BUILDING_PROCESS_EFFICIENCY_MODIFIER";
				if MOD_BALANCE_VP then
					if row.YieldType == "YIELD_FOOD" then
						strTextKey = "TXT_KEY_PRODUCTION_BUILDING_PROCESS_EFFICIENCY_MODIFIER_DISTRESS";
					elseif row.YieldType == "YIELD_GOLD" or row.YieldType == "YIELD_SCIENCE" or row.YieldType == "YIELD_CULTURE" then
						strYields = string.format("%s %s", kYieldInfo.IconString, L(kYieldInfo.Description));
					elseif row.YieldType == "YIELD_FAITH" then
						strTextKey = "TXT_KEY_PRODUCTION_BUILDING_PROCESS_EFFICIENCY_MODIFIER_RELIGIOUS_UNREST";
					end
				end
				if strYields then
					AddTooltipNonZero(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PROCESS_EFFICIENCY_MODIFIER_NEED", row.Yield, strYields, kProcessInfo.Description);
				else
					AddTooltipNonZero(tLocalAbilityLines, strTextKey, row.Yield, kProcessInfo.Description);
				end
			end
		end

		-- Happiness to building class
		for row in GameInfo.Building_BuildingClassLocalHappiness{BuildingType = kBuildingInfo.Type} do
			AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_HAPPINESS", row.Happiness,
				GameInfo.BuildingClasses[row.BuildingClassType].Description);
		end

		for row in GameInfo.Building_BuildingClassHappiness{BuildingType = kBuildingInfo.Type} do
			AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BUILDING_HAPPINESS", row.Happiness,
				GameInfo.BuildingClasses[row.BuildingClassType].Description);
		end
	end

	-- Instant yield on completion
	do
		local tCompleteYields = {};
		local tCompleteYieldsFromPopulation = {};
		for _, kYieldInfo in GameInfoCache("Yields") do
			ExtractSimpleYieldTable(tCompleteYields, "Building_InstantYield", kYieldInfo);
			ExtractSimpleYieldTable(tCompleteYieldsFromPopulation, "Building_YieldFromBirthRetroactive", kYieldInfo);
		end
		-- Add vanilla Gold column
		tCompleteYields[GameInfoTypes.YIELD_GOLD] = tCompleteYields[GameInfoTypes.YIELD_GOLD] or 0;
		tCompleteYields[GameInfoTypes.YIELD_GOLD] = tCompleteYields[GameInfoTypes.YIELD_GOLD] + kBuildingInfo.Gold;
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_ON_COMPLETION", tCompleteYields);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_ON_COMPLETION_FROM_POPULATION", tCompleteYieldsFromPopulation);
	end

	-- WLTKD on completion
	if kBuildingInfo.WLTKDTurns > 0 then
		local iWLTKDTurn = math.floor(kBuildingInfo.WLTKDTurns * iTrainPercent / 100);
		AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_START_WLTKD", iWLTKDTurn);
	end

	-- WLTKD on project completion
	for row in GameInfo.Building_WLTKDFromProject{BuildingType = kBuildingInfo.Type} do
		local iWLTKDTurn = math.floor(row.Turns * iTrainPercent / 100);
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WLTKD_FROM_PROJECT", iWLTKDTurn, GameInfo.Projects[row.ProjectType].Description);
	end

	-- Instant population
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_POPULATION", kBuildingInfo.PopulationChange);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_POPULATION", kBuildingInfo.GlobalPopulationChange);

	-- Automaton workers
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_AUTOMATON", kBuildingInfo.CityAutomatonWorkersChange);
	AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_AUTOMATON", kBuildingInfo.GlobalCityAutomatonWorkersChange);

	-- Victory points on completion
	AddTooltipNonZeroSigned(tTeamAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_VICTORY_POINTS_ON_COMPLETION", kBuildingInfo.VictoryPoints);

	-- Plunder modifier
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_PLUNDER_MODIFIER", kBuildingInfo.CapturePlunderModifier);

	-- Tech share
	AddTooltipPositive(tTeamAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TECH_SHARE", kBuildingInfo.TechShare);

	-- Free resources
	for row in GameInfo.Building_ResourceQuantity{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_RESOURCE", row.Quantity, kResourceInfo.IconString, kResourceInfo.Description);
	end

	for row in GameInfo.Building_ResourceQuantityFromPOP{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_RESOURCE_FROM_POPULATION",
			row.Modifier / 100, kResourceInfo.IconString, kResourceInfo.Description);
	end

	for row in GameInfo.Building_ResourceQuantityPerXFranchises{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_RESOURCE_FROM_FRANCHISE",
			row.NumFranchises, kResourceInfo.IconString, kResourceInfo.Description);
	end

	-- Free techs
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_TECHS", kBuildingInfo.FreeTechs);

	-- Free policies
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_POLICIES", kBuildingInfo.FreePolicies);

	-- Free great people
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_GP", kBuildingInfo.FreeGreatPeople);

	-- Free great work
	if kBuildingInfo.FreeGreatWork then
		AddTooltip(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_GREAT_WORK", GameInfo.GreatWorks[kBuildingInfo.FreeGreatWork].Description);
	end

	-- Free artifacts
	AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_ARTIFACT", kBuildingInfo.FreeArtifacts);

	-- Free spies
	if MOD_BALANCE_SPY_POINTS then
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_SPY_POINTS", kBuildingInfo.ExtraSpies * ESPIONAGE_SPY_POINT_UNIT);
	else
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_SPIES", kBuildingInfo.ExtraSpies);
	end

	-- Free units
	for row in GameInfo.Building_FreeSpecUnits{BuildingType = kBuildingInfo.Type} do
		if row.NumUnits > 0 then
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_UNIT", row.NumUnits, GameInfo.Units[row.UnitType].Description);
		end
	end

	for row in GameInfo.Building_FreeUnits{BuildingType = kBuildingInfo.Type} do
		if row.NumUnits > 0 then
			local eUnit = -1;
			local kUnitInfo = GameInfo.Units[row.UnitType];

			if pActivePlayer then
				-- Venice gets all settler units converted into Merchant of Venice
				if kUnitInfo.Found and pActivePlayer:MayNotAnnex() then
					if not eMerchantOfVeniceUnit then
						-- Only get the first like in DLL. We already assumed there's only one unit.
						eMerchantOfVeniceUnit = GameInfo.Units{CanBuyCityState = 1}().ID;
					end
					eUnit = eMerchantOfVeniceUnit;
				else
					-- This includes policy overrides, but it's player-wide so we'll just allow it
					eUnit = pActivePlayer:GetSpecificUnitType(kUnitInfo.Class);
				end
			else
				local kUnitClassInfo = GameInfo.UnitClasses[kUnitInfo.Class];
				eUnit = GameInfoTypes[kUnitClassInfo.DefaultUnit];
			end

			if eUnit ~= -1 then
				AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_UNIT", row.NumUnits, GameInfo.Units[eUnit].Description);
			end
		end
	end

	-- Free building
	if eCorporationFreeBuildingClass ~= -1 then
		local eFreeBuilding = GetUniqueBuildingFromBuildingClass(eCorporationFreeBuildingClass, pActivePlayer, pCity);
		if eFreeBuilding ~= -1 then
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_BUILDING", GameInfo.Buildings[eFreeBuilding].Description);
		end
	end
	if kBuildingInfo.FreeBuildingThisCity then
		local eFreeBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[kBuildingInfo.FreeBuildingThisCity], pActivePlayer, pCity);
		if eFreeBuilding ~= -1 then
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_BUILDING", GameInfo.Buildings[eFreeBuilding].Description);
		end
	end
	if kBuildingInfo.FreeBuilding then
		local eFreeBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[kBuildingInfo.FreeBuilding], pActivePlayer);
		if eFreeBuilding ~= -1 then
			AddTooltipGlobal(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_BUILDING", GameInfo.Buildings[eFreeBuilding].Description);
		end
	end

	-- Free promotion
	if kBuildingInfo.TrainedFreePromotion then
		local kFreePromotionInfo = GameInfo.UnitPromotions[kBuildingInfo.TrainedFreePromotion];
		if MOD_BALANCE_RETROACTIVE_PROMOTIONS then
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_PROMOTION_RETROACTIVE", kFreePromotionInfo.Description);
		else
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_PROMOTION", kFreePromotionInfo.Description);
		end
	end
	if kBuildingInfo.FreePromotion then
		AddTooltip(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_FREE_PROMOTION_GLOBAL",
			GameInfo.UnitPromotions[kBuildingInfo.FreePromotion].Description);
	end

	-- Free votes
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES", kBuildingInfo.SingleLeagueVotes);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_SCALING", kBuildingInfo.ExtraLeagueVotes);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_FROM_GPT", kBuildingInfo.VotesPerGPT);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_FROM_DP", kBuildingInfo.DPToVotes);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_FROM_CAPITALS", kBuildingInfo.CapitalsToVotes);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_FROM_DOF", kBuildingInfo.DoFToVotes);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_FROM_RESEARCH_AGREEMENTS", kBuildingInfo.RAToVotes);
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_VOTES_FROM_RELIGION", kBuildingInfo.FaithToVotes);

	-- Starting spy rank
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPY_RANK", kBuildingInfo.SpyRankChange);

	-- Instant spy rank
	AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_SPY_RANK", kBuildingInfo.InstantSpyRankChange);

	-- Instant pressure
	AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_PRESSURE", kBuildingInfo.InstantReligiousPressure / 10);

	-- Nearby unique luxury
	-- This assumes Traits.UniqueLuxuryCities is set to a large value if non-zero, so that it always spawns a resource
	if kBuildingInfo.GrantsRandomResourceTerritory > 0 then
		if eActiveCiv ~= -1 then
			-- Get the list of unique luxuries for the active civ
			local tResources = {};
			for kResourceInfo in GameInfo.Resources{CivilizationType = GameInfo.Civilizations[eActiveCiv].Type} do
				table.insert(tResources, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
			end
			if next(tResources) then
				AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPAWN_NEARBY_LUXURY", kBuildingInfo.GrantsRandomResourceTerritory, table.concat(tResources, ", "));
			end
		else
			AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SPAWN_NEARBY_LUXURY_GENERIC", kBuildingInfo.GrantsRandomResourceTerritory);
		end
	end

	-- Nearby resource
	local tNearbyResources = {};
	for row in GameInfo.Building_ResourcePlotsToPlace{BuildingType = kBuildingInfo.Type} do
		tNearbyResources[row.ResourceType] = tNearbyResources[row.ResourceType] or {};
		tNearbyResources[row.ResourceType][row.ResourceQuantityToPlace] = tNearbyResources[row.ResourceType][row.ResourceQuantityToPlace] or 0;
		tNearbyResources[row.ResourceType][row.ResourceQuantityToPlace] = tNearbyResources[row.ResourceType][row.ResourceQuantityToPlace] + row.NumPlots;
	end
	for strResourceType, tQuantityPlots in pairs(tNearbyResources) do
		local kResourceInfo = GameInfo.Resources[strResourceType];
		for iQuantity, iNumPlots in pairs(tQuantityPlots) do
			if iNumPlots > 0 then
				local strTooltip = L("TXT_KEY_PRODUCTION_BUILDING_SPAWN_NEARBY_RESOURCE", iNumPlots, kResourceInfo.IconString, kResourceInfo.Description);
				if iQuantity > 1 then
					table.insert(tLocalAbilityLines, string.format("%s %s", strTooltip, L("TXT_KEY_PRODUCTION_BUILDING_SPAWN_NEARBY_RESOURCE_SUFFIX", iQuantity)));
				else
					table.insert(tLocalAbilityLines, strTooltip);
				end
			end
		end
	end

	-- Trade route bonuses
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TRADE_ROUTES", kBuildingInfo.NumTradeRouteBonus);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_LAND_TRADE_ROUTE_RANGE_MODIFIER", kBuildingInfo.TradeRouteLandDistanceModifier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SEA_TRADE_ROUTE_RANGE_MODIFIER", kBuildingInfo.TradeRouteSeaDistanceModifier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_LAND_ETR_ORIGIN_GOLD", kBuildingInfo.TradeRouteLandGoldBonus * fTradeMultiplier / 100);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SEA_ETR_ORIGIN_GOLD", kBuildingInfo.TradeRouteSeaGoldBonus * fTradeMultiplier / 100);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ETR_DESTINATION_OUR_GOLD", kBuildingInfo.TradeRouteRecipientBonus * fTradeMultiplier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ETR_DESTINATION_THEIR_GOLD", kBuildingInfo.TradeRouteTargetBonus * fTradeMultiplier);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RESOURCE_DIVERSITY_MODIFIER", kBuildingInfo.ResourceDiversityModifier);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TRADE_ROUTE_DURATION_MODIFIER", kBuildingInfo.TRTurnModGlobal);
	AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TRADE_ROUTE_DURATION_MODIFIER_LOCAL", kBuildingInfo.TRTurnModLocal);
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TRADE_UNIT_EXTRA_SIGHT", kBuildingInfo.TRVisionBoost);

	if kBuildingInfo.TRSpeedBoost > 1 then
		AddTooltip(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TRADE_UNIT_SPEED_MULTIPLIER", kBuildingInfo.TRSpeedBoost);
	end

	-- Unit copy
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_COPY", kBuildingInfo.InstantMilitaryIncrease > 0);

	-- Allow production of units
	local tAllowedUnits = {};
	for row in GameInfo.Building_UnitClassTrainingAllowed{BuildingType = kBuildingInfo.Type} do
		local eUnit = GetUniqueUnitFromUnitClass(row.UnitClassType, pActivePlayer, pCity);
		if eUnit ~= -1 then
			AddTooltip(tAllowedUnits, GameInfo.Units[eUnit].Description);
		else
			AddTooltip(tAllowedUnits, GameInfo.UnitClasses[row.UnitClassType].Description);
		end
	end
	if next(tAllowedUnits) then
		AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ALLOW_UNITS", table.concat(tAllowedUnits, ", "));
	end

	-- Resource tile claim
	local tResourceClaim = {};
	local tResourceSteal = {};
	for row in GameInfo.Building_ResourceClaim{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		table.insert(row.IncludeOwnedByOtherPlayer and tResourceSteal or tResourceClaim, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
	end
	if next(tResourceClaim) then
		AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_NEARBY_RESOURCE_CLAIM", table.concat(tResourceClaim, ", "));
	end
	if next(tResourceSteal) then
		AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_NEARBY_RESOURCE_STEAL", table.concat(tResourceSteal, ", "));
	end

	-- Research agreement science modifier
	local bResearchAgreement = Game and Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS");
	local iRAMod = kBuildingInfo.MedianTechPercentChange * 100 / 50; -- 50 is base RA multiplier hardcoded in DLL
	if bResearchAgreement and not bGeneralInfo then
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RESEARCH_AGREEMENT_SCIENCE_MODIFIER", iRAMod);
	elseif bResearchAgreement == nil or bGeneralInfo then
		AddTooltipPositive(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_RESEARCH_AGREEMENT_SCIENCE_MODIFIER_GENERIC", iRAMod);
	end

	-- Damage for enemies stopping on ocean
	AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_OCEAN_DAMAGE", kBuildingInfo.DeepWaterTileDamage);

	-- Global follower% requirement reduction
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_REDUCED_GLOBAL_FOLLOWER_REQUIREMENT", kBuildingInfo.ReformationFollowerReduction * -1);

	-- Urbanization
	if MOD_BALANCE_VP or MOD_BALANCE_CORE_JFD then
		AddTooltipNonZeroSigned(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_URBANIZATION_REDUCTION", kBuildingInfo.NoUnhappfromXSpecialists);
		AddTooltipGlobalNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_URBANIZATION_REDUCTION", kBuildingInfo.NoUnhappfromXSpecialistsGlobal);
	end

	-- Free units from vassals
	AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_VASSAL_UNIT_PER_ERA", kBuildingInfo.VassalLevyEra);

	-- Instant influence when expending great people
	AddTooltipNonZeroSigned(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_INFLUENCE_ON_GP_EXPEND", kBuildingInfo.GPExpendInfluence);

	-- Instant great person points on growth
	AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GPP_ON_GROWTH", kBuildingInfo.GPPOnCitizenBirth);

	-- Simple (boolean) abilities
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_START_GOLDEN_AGE", kBuildingInfo.GoldenAge);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WATER_CONNECTION", kBuildingInfo.AllowsWaterRoutes);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_WATER_INDUSTRIAL_CONNECTION", kBuildingInfo.AllowsIndustrialWaterRoutes);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_AIR_CONNECTION", kBuildingInfo.AllowsAirRoutes);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ALLOW_PUPPET_PURCHASE", kBuildingInfo.AllowsPuppetPurchase);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ITR_FOOD", kBuildingInfo.AllowsFoodTradeRoutes);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ITR_PRODUCTION", kBuildingInfo.AllowsProductionTradeRoutes);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_EXTRA_LUXURIES", kBuildingInfo.ExtraLuxuries);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_AIRLIFT", kBuildingInfo.Airlift);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_REMOVE_OCCUPIED_UNHAPPINESS", kBuildingInfo.NoOccupiedUnhappiness);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ADD_FRESH_WATER", kBuildingInfo.AddsFreshWater);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GAINLESS_PILLAGE", kBuildingInfo.CityGainlessPillage);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BORDER_OBSTACLE_LAND", kBuildingInfo.BorderObstacleCity);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BORDER_OBSTACLE_WATER", kBuildingInfo.BorderObstacleWater);
	AddTooltipIfTrue(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_NO_STARVATION_FROM_NON_SPECIALISTS", kBuildingInfo.NoStarvationNonSpecialist);

	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GAINLESS_PILLAGE_GLOBAL", kBuildingInfo.CityGainlessPillage);
	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BORDER_OBSTACLE_LAND_PLAYER", kBuildingInfo.PlayerBorderObstacle);
	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ITR_FOOD_GLOBAL", kBuildingInfo.AllowsFoodTradeRoutesGlobal);
	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_ITR_PRODUCTION_GLOBAL", kBuildingInfo.AllowsProductionTradeRoutesGlobal);
	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_SECONDARY_PANTHEON", kBuildingInfo.SecondaryPantheon);
	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_REFORMATION", kBuildingInfo.IsReformation);
	AddTooltipIfTrue(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_REMOVE_DP_LIMIT", kBuildingInfo.IgnoreDefensivePactLimit);

	AddTooltipIfTrue(tTeamAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TEAM_SHARE", kBuildingInfo.TeamShare);
	AddTooltipIfTrue(tTeamAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_BORDER_OBSTACLE_LAND_TEAM", kBuildingInfo.BorderObstacle);

	-- Instant yield/GPP triggers
	do
		local tGPExpendScalingYields = {};
		local tWLTKDYields = {};
		local tGrowthYields = {};
		local tGrowthYieldsEraScaling = {};
		local tBorderGrowthYields = {};
		local tCombatExperienceYields = {};
		local tConstructionYields = {};
		local tDeathYields = {};
		local tGoldenAgeYields = {};
		local tITREndYields = {};
		local tETREndYields = {};
		local tLongCountYields = {};
		local tPillageYields = {};
		local tPillageYieldsGlobal = {};
		local tPillageYieldsGlobalPlayer = {};
		local tTechYields = {};
		local tPolicyYields = {};
		local tUnitGiftYields = {};
		local tUnitPromoteYields = {};
		local tUnitPromoteYieldsGlobal = {};
		local tSpyMissionYields = {};
		local tSpyKillYields = {};
		local tSpyIdentifyYields = {};
		local tSpyKillOrIdentifyYields = {};
		local tRigElectionYields = {};
		local tUnitKillYields = {};
		local tUnitKillYieldsGlobal = {};
		local tUnitKillYieldsGlobalEraScaling = {};
		local tUnitKillYieldsGlobalGoldenAge = {};
		local tUnitKillYieldsGlobalGoldenAgeEraScaling = {};
		local tUnitKillYieldsGlobalPlayer = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			ExtractSimpleYieldTable(tGPExpendScalingYields, "Building_YieldFromGPExpend", kYieldInfo);
			ExtractSimpleYieldTable(tWLTKDYields, "Building_InstantYieldFromWLTKDStart", kYieldInfo);
			ExtractSimpleYieldTable(tBorderGrowthYields, "Building_YieldFromBorderGrowth", kYieldInfo);
			ExtractSimpleYieldTable(tConstructionYields, "Building_YieldFromConstruction", kYieldInfo);
			ExtractSimpleYieldTable(tDeathYields, "Building_YieldFromDeath", kYieldInfo);
			ExtractSimpleYieldTable(tGoldenAgeYields, "Building_YieldFromGoldenAgeStart", kYieldInfo);
			ExtractSimpleYieldTable(tITREndYields, "Building_YieldFromInternalTREnd", kYieldInfo);
			ExtractSimpleYieldTable(tETREndYields, "Building_YieldFromInternationalTREnd", kYieldInfo);
			ExtractSimpleYieldTable(tLongCountYields, "Building_YieldFromLongCount", kYieldInfo);
			ExtractSimpleYieldTable(tPillageYields, "Building_YieldFromPillage", kYieldInfo);
			ExtractSimpleYieldTable(tPillageYieldsGlobal, "Building_YieldFromPillageGlobal", kYieldInfo);
			ExtractSimpleYieldTable(tPillageYieldsGlobalPlayer, "Building_YieldFromPillageGlobalPlayer", kYieldInfo);
			ExtractSimpleYieldTable(tTechYields, "Building_YieldFromTech", kYieldInfo);
			ExtractSimpleYieldTable(tPolicyYields, "Building_YieldFromPolicyUnlock", kYieldInfo);
			ExtractSimpleYieldTable(tUnitGiftYields, "Building_YieldFromUnitGiftGlobal", kYieldInfo);
			ExtractSimpleYieldTable(tUnitPromoteYields, "Building_YieldFromUnitLevelUp", kYieldInfo);
			ExtractSimpleYieldTable(tUnitPromoteYieldsGlobal, "Building_YieldFromUnitLevelUpGlobal", kYieldInfo);
			ExtractSimpleYieldTable(tUnitKillYields, "Building_YieldFromVictory", kYieldInfo);
			ExtractSimpleYieldTable(tUnitKillYieldsGlobalPlayer, "Building_YieldFromVictoryGlobalPlayer", kYieldInfo);
			ExtractSimpleYieldTable(tSpyKillYields, "Building_YieldFromSpyDefense", kYieldInfo);
			if MOD_BALANCE_VP then
				ExtractSimpleYieldTable(tSpyMissionYields, "Building_YieldFromSpyAttack", kYieldInfo);
				ExtractSimpleYieldTable(tSpyIdentifyYields, "Building_YieldFromSpyIdentify", kYieldInfo);
				ExtractSimpleYieldTable(tSpyKillOrIdentifyYields, "Building_YieldFromSpyDefenseOrID", kYieldInfo);
				ExtractSimpleYieldTable(tRigElectionYields, "Building_YieldFromSpyRigElection", kYieldInfo);
			end

			for row in GameInfo.Building_YieldFromBirth{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type, IsEraScaling = 0} do
				tGrowthYields[eYield] = row.Yield;
			end
			for row in GameInfo.Building_YieldFromBirth{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type, IsEraScaling = 1} do
				tGrowthYieldsEraScaling[eYield] = row.Yield;
			end

			for row in GameInfo.Building_YieldFromVictoryGlobal{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type, IsEraScaling = 0, GoldenAgeOnly = 0} do
				tUnitKillYieldsGlobal[eYield] = row.Yield;
			end
			for row in GameInfo.Building_YieldFromVictoryGlobal{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type, IsEraScaling = 1, GoldenAgeOnly = 0} do
				tUnitKillYieldsGlobalEraScaling[eYield] = row.Yield;
			end
			for row in GameInfo.Building_YieldFromVictoryGlobal{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type, IsEraScaling = 0, GoldenAgeOnly = 1} do
				tUnitKillYieldsGlobalGoldenAge[eYield] = row.Yield;
			end
			for row in GameInfo.Building_YieldFromVictoryGlobal{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type, IsEraScaling = 1, GoldenAgeOnly = 1} do
				tUnitKillYieldsGlobalGoldenAgeEraScaling[eYield] = row.Yield;
			end

			for row in GameInfo.Building_YieldFromCombatExperienceTimes100{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				tCombatExperienceYields[eYield] = row.Yield / 100;
			end

			local iGrowthYield = 0;
			for row in GameInfo.Building_GrowthExtraYield{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iGrowthYield = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GROWTH_SCALING",
				iGrowthYield, kYieldInfo.IconString, kYieldInfo.Description);

			local iUnitTrainingYield = 0;
			for row in GameInfo.Building_YieldFromUnitProduction{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iUnitTrainingYield = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_COMPLETION",
				iUnitTrainingYield, kYieldInfo.IconString, kYieldInfo.Description);

			local iGoldPurchaseYield = 0;
			for row in GameInfo.Building_YieldFromPurchase{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iGoldPurchaseYield = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GOLD_PURCHASE",
				iGoldPurchaseYield, kYieldInfo.IconString, kYieldInfo.Description);

			local iGoldPurchaseYieldGlobal = 0;
			for row in GameInfo.Building_YieldFromPurchaseGlobal{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iGoldPurchaseYieldGlobal = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GOLD_PURCHASE_GLOBAL",
				iGoldPurchaseYieldGlobal, kYieldInfo.IconString, kYieldInfo.Description);

			local iFaithPurchaseYield = 0;
			for row in GameInfo.Building_YieldFromFaithPurchase{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iFaithPurchaseYield = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_FAITH_PURCHASE",
				iFaithPurchaseYield, kYieldInfo.IconString, kYieldInfo.Description);

			local iWriterSpawnYield = 0;
			for row in GameInfo.Building_YieldFromGPBirthScaledWithWriterBulb{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iWriterSpawnYield = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GP_BIRTH_WRITER",
				iWriterSpawnYield, kYieldInfo.IconString, kYieldInfo.Description);

			local iArtistSpawnYield = 0;
			for row in GameInfo.Building_YieldFromGPBirthScaledWithArtistBulb{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iArtistSpawnYield = row.Yield;
			end
			AddTooltipPositive(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GP_BIRTH_ARTIST",
				iArtistSpawnYield, kYieldInfo.IconString, kYieldInfo.Description);
		end

		if MOD_BALANCE_VP then
			tGPExpendScalingYields[GameInfoTypes.YIELD_GOLD] = tGPExpendScalingYields[GameInfoTypes.YIELD_GOLD] or 0;
			tGPExpendScalingYields[GameInfoTypes.YIELD_GOLD] = tGPExpendScalingYields[GameInfoTypes.YIELD_GOLD] + kBuildingInfo.GreatPersonExpendGold;
		elseif kBuildingInfo.GreatPersonExpendGold > 0 then
			AddTooltip(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GP_EXPEND",
				GetYieldBoostString(GameInfo.Yields.YIELD_GOLD, kBuildingInfo.GreatPersonExpendGold));
		end

		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GROWTH", tGrowthYields);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GOLDEN_AGE_START", tGoldenAgeYields);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_FROM_COMBAT_EXPERIENCE", tCombatExperienceYields);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_PROMOTION", tUnitPromoteYields);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_PROMOTION_GLOBAL", tUnitPromoteYieldsGlobal);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_KILL_GLOBAL", tUnitKillYieldsGlobal);
		AddTooltipSimpleYieldBoostTable(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_KILL_GLOBAL_GOLDEN_AGE", tUnitKillYieldsGlobalGoldenAge);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_WLTKD", tWLTKDYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GROWTH", tGrowthYieldsEraScaling);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_BORDER_GROWTH", tBorderGrowthYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_CONSTRUCTION", tConstructionYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_ITR_COMPLETION", tITREndYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_ETR_COMPLETION", tETREndYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_LONG_COUNT", tLongCountYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_TECH", tTechYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_POLICY", tPolicyYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_KILL", tUnitKillYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_KILL_GLOBAL", tUnitKillYieldsGlobalEraScaling);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_KILL_GLOBAL_GOLDEN_AGE", tUnitKillYieldsGlobalGoldenAgeEraScaling);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_PILLAGE", tPillageYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_PILLAGE_GLOBAL", tPillageYieldsGlobal);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_SPY_MISSION", tSpyMissionYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_SPY_KILL", tSpyKillYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_SPY_IDENTIFICATION", tSpyIdentifyYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_SPY_KILL_OR_IDENTIFICATION", tSpyKillOrIdentifyYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_RIG_ELECTION", tRigElectionYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_GIFT", tUnitGiftYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GP_EXPEND", tGPExpendScalingYields);
		AddTooltipSimpleYieldBoostTableEraScaling(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_DEATH", tDeathYields);
		AddTooltipSimpleYieldBoostTableGlobalEraScaling(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_UNIT_KILL_GLOBAL", tUnitKillYieldsGlobalPlayer);
		AddTooltipSimpleYieldBoostTableGlobalEraScaling(tGlobalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_PILLAGE_GLOBAL", tPillageYieldsGlobalPlayer);

		local tGPPOnConstruction = {};
		for row in GameInfo.Building_GreatPersonPointFromConstruction{BuildingType = kBuildingInfo.Type} do
			tGPPOnConstruction[row.EraType] = tGPPOnConstruction[row.EraType] or {};
			tGPPOnConstruction[row.EraType][row.GreatPersonType] = tGPPOnConstruction[row.EraType][row.GreatPersonType] or 0;
			tGPPOnConstruction[row.EraType][row.GreatPersonType] = tGPPOnConstruction[row.EraType][row.GreatPersonType] + row.Value;
		end
		for strEraType, tGPPValues in pairs(tGPPOnConstruction) do
			local kEraInfo = GameInfo.Eras[strEraType];
			for strGreatPersonType, iValue in pairs(tGPPValues) do
				if iValue > 0 then
					local kGreatPersonInfo = GameInfo.GreatPersons[strGreatPersonType];
					AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GPP_ON_CONSTRUCTION",
						iValue, kEraInfo.Description, kGreatPersonInfo.IconString, kGreatPersonInfo.Description);
				end
			end
		end

		tGPPOnConstruction = {};
		for row in GameInfo.Building_GreatPersonProgressFromConstruction{BuildingType = kBuildingInfo.Type} do
			tGPPOnConstruction[row.EraType] = tGPPOnConstruction[row.EraType] or {};
			tGPPOnConstruction[row.EraType][row.GreatPersonType] = tGPPOnConstruction[row.EraType][row.GreatPersonType] or 0;
			tGPPOnConstruction[row.EraType][row.GreatPersonType] = tGPPOnConstruction[row.EraType][row.GreatPersonType] + row.Value;
		end
		for strEraType, tGPPValues in pairs(tGPPOnConstruction) do
			local kEraInfo = GameInfo.Eras[strEraType];
			for strGreatPersonType, iValue in pairs(tGPPValues) do
				if iValue > 0 then
					local kGreatPersonInfo = GameInfo.GreatPersons[strGreatPersonType];
					AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GP_PROGRESS_ON_CONSTRUCTION",
						iValue, kEraInfo.Description, kGreatPersonInfo.IconString, kGreatPersonInfo.Description);
				end
			end
		end

		local tGPBirthYields = {};
		for row in GameInfo.Building_YieldFromGPBirthScaledWithPerTurnYield{BuildingType = kBuildingInfo.Type} do
			local eYieldIn = GameInfoTypes[row.YieldIn];
			local eYieldOut = GameInfoTypes[row.YieldOut];
			tGPBirthYields[row.GreatPersonType] = tGPBirthYields[row.GreatPersonType] or {};
			tGPBirthYields[row.GreatPersonType][eYieldIn] = tGPBirthYields[row.GreatPersonType][eYieldIn] or {};
			tGPBirthYields[row.GreatPersonType][eYieldIn][eYieldOut] = tGPBirthYields[row.GreatPersonType][eYieldIn][eYieldOut] or 0;
			tGPBirthYields[row.GreatPersonType][eYieldIn][eYieldOut] = tGPBirthYields[row.GreatPersonType][eYieldIn][eYieldOut] + row.Value;
		end
		for strGreatPersonType, tYieldConversions in pairs(tGPBirthYields) do
			local kGreatPersonInfo = GameInfo.GreatPersons[strGreatPersonType];
			for eYieldIn, tYieldOuts in pairs(tYieldConversions) do
				for eYieldOut, iValue in pairs(tYieldOuts) do
					if iValue > 0 then
						local kYieldInInfo = GameInfo.Yields[eYieldIn];
						local kYieldOutInfo = GameInfo.Yields[eYieldOut];
						AddTooltip(tLocalAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INSTANT_YIELD_ON_GP_BIRTH_SCALING", iValue,
							kGreatPersonInfo.IconString, kGreatPersonInfo.Description, kYieldInInfo.IconString, kYieldInInfo.Description,
							kYieldOutInfo.IconString, kYieldOutInfo.Description);
					end
				end
			end
		end
	end

	if next(tYieldLines) then
		table.insert(tAbilityLines, table.concat(tYieldLines, "[NEWLINE]"));
	end

	if next(tCorporationAbilities) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_CORPORATION_EFFECTS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tCorporationAbilities, "[NEWLINE][ICON_BULLET]"));
	end
	local iLocalAbilityPos = #tAbilityLines + 1;

	if next(tGlobalAbilityLines) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_GLOBAL_EFFECTS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tGlobalAbilityLines, "[NEWLINE][ICON_BULLET]"));
	end

	if next(tTeamAbilityLines) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_TEAM_EFFECTS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tTeamAbilityLines, "[NEWLINE][ICON_BULLET]"));
	end

	-- Historic event triggers
	-- The game allows negative values for some, but I don't see an easy way to make the UI cater to that, so it'll still use the positive wordings
	local tHistoricEvents = {};
	AddTooltipNonZero(tHistoricEvents, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENT_ERA_CHANGE", kBuildingInfo.EventTourism * 3);
	AddTooltipNonZero(tHistoricEvents, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENT_WORLD_WONDER", kBuildingInfo.EventTourism * 6);
	AddTooltipNonZero(tHistoricEvents, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENT_GREAT_PERSON", kBuildingInfo.EventTourism * 2);
	AddTooltipNonZero(tHistoricEvents, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENT_WON_WAR", kBuildingInfo.EventTourism * 6);
	AddTooltipPositive(tHistoricEvents, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENT_TRADE_LAND", kBuildingInfo.FinishLandTRTourism * 2);
	AddTooltipPositive(tHistoricEvents, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENT_TRADE_SEA", kBuildingInfo.FinishSeaTRTourism * 2);
	if next(tHistoricEvents) then
		AddTooltip(tAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_HISTORIC_EVENTS");
		table.insert(tAbilityLines, "[ICON_BULLET]" .. table.concat(tHistoricEvents, "[NEWLINE][ICON_BULLET]"));
	end

	-- Influence with all city states on completion
	AddTooltipNonZeroSigned(tAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_INFLUENCE_ON_COMPLETION", kBuildingInfo.MinorFriendshipChange);

	-- Yield boosts on cities in the same area
	AddTooltipSimpleYieldModifierTable(tAbilityLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_MODIFIER_AREA", tAreaBoosts);

	local iAbilityInsertPos = #tLines + 1;

	----------------------
	-- New medians (VP only)
	----------------------
	if next(tNewMedianLines) then
		table.insert(tNewMedianLines, 1, L("TXT_KEY_PRODUCTION_BUILDING_MEDIAN_CHANGES"));
		table.insert(tLines, table.concat(tNewMedianLines, "[NEWLINE]"));
	end

	----------------------
	-- Requirement section
	-- Most are skipped in city view
	----------------------

	-- May be purchased/invested in puppets
	local strInvestKey = MOD_BALANCE_BUILDING_INVESTMENTS and "TXT_KEY_PRODUCTION_PUPPET_INVESTABLE" or "TXT_KEY_PRODUCTION_PUPPET_PURCHASABLE";
	AddTooltipIfTrue(tReqLines, strInvestKey, kBuildingInfo.PuppetPurchaseOverride);

	if not pCity then
		-- Simple (boolean) requirements
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_CAPITAL", kBuildingInfo.CapitalOnly);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_HOLY_CITY", kBuildingInfo.HolyCity);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_RIVER", kBuildingInfo.River);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_FRESH_WATER", kBuildingInfo.FreshWater);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_COASTAL_OR_FRESH_WATER", kBuildingInfo.AnyWater);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_MOUNTAIN_ADJACENT", kBuildingInfo.Mountain);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_MOUNTAIN_NEARBY", kBuildingInfo.NearbyMountainRequired);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_HILL", kBuildingInfo.Hill);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_FLAT", kBuildingInfo.Flat);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NO_FRESH_WATER", kBuildingInfo.IsNoWater);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NO_RIVER", kBuildingInfo.IsNoRiver);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NO_COASTAL", kBuildingInfo.IsNoCoast);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_INDUSTRIAL_CONNECTION", kBuildingInfo.RequiresIndustrialCityConnection);
		AddTooltipIfTrue(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_OCCUPIED_ONLY", kBuildingInfo.NoOccupiedUnhappiness and not kBuildingInfo.BuildAnywhere);

		-- Coastal or whatever custom water size
		if kBuildingInfo.Water then
			local iMinWaterSize = kBuildingInfo.MinAreaSize;
			if iMinWaterSize == MIN_WATER_SIZE_FOR_OCEAN then
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_COASTAL");
			elseif iMinWaterSize > 0 then
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_COASTAL_CUSTOM_SIZE", iMinWaterSize);
			else
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_COASTAL_INCLUDING_LAKE");
			end
		end

		-- Nearby terrain
		if kBuildingInfo.NearbyTerrainRequired then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NEARBY_TERRAIN", GameInfo.Terrains[kBuildingInfo.NearbyTerrainRequired].Description);
		end

		-- Not on terrain
		if kBuildingInfo.ProhibitedCityTerrain then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NO_TERRAIN", GameInfo.Terrains[kBuildingInfo.ProhibitedCityTerrain].Description);
		end

		-- Local features
		local tReqFeatures = {};
		for row in GameInfo.Building_LocalFeatureOrs{BuildingType = kBuildingInfo.Type} do
			table.insert(tReqFeatures, L(GameInfo.Features[row.FeatureType].Description));
		end
		if next(tReqFeatures) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_LOCAL_FEATURE_OR", table.concat(tReqFeatures, ", "));
		end
		tReqFeatures = {};
		for row in GameInfo.Building_LocalFeatureAnds{BuildingType = kBuildingInfo.Type} do
			table.insert(tReqFeatures, L(GameInfo.Features[row.FeatureType].Description));
		end
		if next(tReqFeatures) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_LOCAL_FEATURE_AND", table.concat(tReqFeatures, ", "));
		end

		-- Global monopolies
		local tReqMonopolies = {};
		for row in GameInfo.Building_ResourceMonopolyOrs{BuildingType = kBuildingInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			table.insert(tReqMonopolies, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
		end
		if next(tReqMonopolies) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_GLOBAL_MONOPOLY_OR", table.concat(tReqMonopolies, ", "));
		end
		tReqMonopolies = {};
		for row in GameInfo.Building_ResourceMonopolyAnds{BuildingType = kBuildingInfo.Type} do
			local kResourceInfo = GameInfo.Resources[row.ResourceType];
			table.insert(tReqMonopolies, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
		end
		if next(tReqMonopolies) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_GLOBAL_MONOPOLY_AND", table.concat(tReqMonopolies, ", "));
		end

		-- Prereq buildings
		local tBuildings = {};
		if kBuildingInfo.NeedBuildingThisCity then
			AddTooltip(tBuildings, GameInfo.Buildings[kBuildingInfo.NeedBuildingThisCity].Description);
		end
		for row in GameInfo.Building_ClassesNeededInCity{BuildingType = kBuildingInfo.Type} do
			local ePrereqBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer);
			if ePrereqBuilding ~= -1 then
				AddTooltip(tBuildings, GameInfo.Buildings[ePrereqBuilding].Description);
			else
				AddTooltip(tBuildings, GameInfo.BuildingClasses[row.BuildingClassType].Description);
			end
		end
		if next(tBuildings) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_BUILDINGS", table.concat(tBuildings, ", "));
		end

		tBuildings = {};
		for row in GameInfo.Building_ClassNeededAnywhere{BuildingType = kBuildingInfo.Type} do
			local ePrereqBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer);
			if ePrereqBuilding ~= -1 then
				AddTooltip(tBuildings, GameInfo.Buildings[ePrereqBuilding].Description);
			else
				AddTooltip(tBuildings, GameInfo.BuildingClasses[row.BuildingClassType].Description);
			end
		end
		if next(tBuildings) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_REQUIRED_BUILDINGS_GLOBAL", table.concat(tBuildings, ", "));
		end

		-- Blocking buildings
		tBuildings = {};
		local iExclusiveGroup = kBuildingInfo.MutuallyExclusiveGroup;
		if iExclusiveGroup ~= -1 then
			for kExclusiveBuildingInfo in GameInfo.Buildings{MutuallyExclusiveGroup = iExclusiveGroup} do
				AddTooltipIfTrue(tBuildings, kExclusiveBuildingInfo.Description, kExclusiveBuildingInfo.ID ~= eBuilding);
			end
		end
		for row in GameInfo.Building_LockedBuildingClasses{BuildingType = kBuildingInfo.Type} do
			local eExclusiveBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer);
			if eExclusiveBuilding ~= -1 then
				AddTooltip(tBuildings, GameInfo.Buildings[eExclusiveBuilding].Description);
			else
				AddTooltip(tBuildings, GameInfo.BuildingClasses[row.BuildingClassType].Description);
			end
		end
		if next(tBuildings) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_EXCLUSIVE_LOCAL", table.concat(tBuildings, ", "));
		end

		tBuildings = {};
		for row in GameInfo.Building_ClassNeededNowhere{BuildingType = kBuildingInfo.Type} do
			local eExclusiveBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[row.BuildingClassType], pActivePlayer);
			if eExclusiveBuilding ~= -1 then
				AddTooltip(tBuildings, GameInfo.Buildings[eExclusiveBuilding].Description);
			end
		end
		if next(tBuildings) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_EXCLUSIVE_GLOBAL", table.concat(tBuildings, ", "));
		end

		-- Prereq techs
		local tTechs = {};
		if kBuildingInfo.PrereqTech then
			AddTooltip(tTechs, GameInfo.Technologies[kBuildingInfo.PrereqTech].Description);
		end
		for row in GameInfo.Building_TechAndPrereqs{BuildingType = kBuildingInfo.Type} do
			AddTooltip(tTechs, GameInfo.Technologies[row.TechType].Description);
		end
		if next(tTechs) then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_PREREQ_TECH", table.concat(tTechs, ", "));
		end

		-- Policy requirement
		if kBuildingInfo.PolicyType then
			-- Is this an opener or finisher? Assume openers and finishers are distinct across policy branches
			local bOpenerOrFinisher = false;
			for _, kPolicyBranchInfo in GameInfoCache("PolicyBranchTypes") do
				if kPolicyBranchInfo.FreePolicy == kBuildingInfo.PolicyType then
					bOpenerOrFinisher = true;
					AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY_BRANCH_OPENER", kPolicyBranchInfo.Description);
					break;
				end
				if kPolicyBranchInfo.FreeFinishingPolicy == kBuildingInfo.PolicyType then
					bOpenerOrFinisher = true;
					AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY_BRANCH_COMPLETION", kPolicyBranchInfo.Description);
					break;
				end
			end
			if not bOpenerOrFinisher then
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY", GameInfo.Policies[kBuildingInfo.PolicyType].Description);
			end
		end

		-- Policy branch requirement
		if kBuildingInfo.PolicyBranchType then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_REQUIRED_POLICY_BRANCH", GameInfo.PolicyBranchTypes[kBuildingInfo.PolicyBranchType].Description);
		end

		-- Total resource requirements
		if kBuildingInfo.ResourceType then
			local kResourceInfo = GameInfo.Resources[kBuildingInfo.ResourceType];
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_TOTAL_RESOURCES_REQUIRED", 1, kResourceInfo.IconString, kResourceInfo.Description);
		end

		-- Unlocked by belief? Different cases for reformation building and faith purchase
		if kBuildingInfo.UnlockedByBelief then
			-- Due to complexity, assume the unlocking beliefs are all of the same type (pantheon/founder/follower/enhancer/reformation)
			local bFounder = false;
			local bFollower = false;
			local bEnhancer = false;
			local bReformation = false;
			local tBeliefs = {};
			for row in GameInfo.Belief_BuildingClassFaithPurchase{BuildingClassType = kBuildingClassInfo.Type} do
				local kBeliefInfo = GameInfo.Beliefs[row.BeliefType];
				bFounder = kBeliefInfo.Founder;
				bFollower = kBeliefInfo.Follower;
				bEnhancer = kBeliefInfo.Enhancer;
				bReformation = kBeliefInfo.Reformation;
				AddTooltip(tBeliefs, kBeliefInfo.ShortDescription);
			end

			if kBuildingInfo.IsReformation then
				-- Followers require the city's majority religion to have one of the beliefs
				if bFollower then
					AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_MAJORITY_RELIGION_BELIEF_NEEDED", table.concat(tBeliefs, ", "));
				-- Founders and enhancers require it to be built in the holy city of the player's primary religion
				-- Assume these are never pantheons or reformations because that would make no sense
				else
					AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_HOLY_CITY_BELIEF_NEEDED", table.concat(tBeliefs, ", "));
				end
			end

			if kBuildingInfo.FaithCost > 0 then
				-- One of the beliefs needs to be part of the city's majority religion
				AddTooltip(tReqLines, "TXT_KEY_PURCHASE_BUILDING_MAJORITY_RELIGION_BELIEF_NEEDED", table.concat(tBeliefs, ", "));

				-- Founders and enhancers only work for the holy city owner
				AddTooltipIfTrue(tReqLines, "TXT_KEY_PURCHASE_BUILDING_OWN_HOLY_CITY", bFounder or bEnhancer);

				-- Reformations work for holy city owner or adopter
				AddTooltipIfTrue(tReqLines, "TXT_KEY_PURCHASE_BUILDING_OWN_HOLY_CITY_OR_STATE_RELIGION", bReformation);
			end
		end
	end

	-- Local resources
	local tReqResources = {};
	for row in GameInfo.Building_LocalResourceOrs{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		table.insert(tReqResources, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
	end
	if next(tReqResources) then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_LOCAL_RESOURCE_OR", table.concat(tReqResources, ", "));
	end
	tReqResources = {};
	for row in GameInfo.Building_LocalResourceAnds{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		table.insert(tReqResources, string.format("%s %s", kResourceInfo.IconString, L(kResourceInfo.Description)));
	end
	if next(tReqResources) then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_LOCAL_RESOURCE_AND", table.concat(tReqResources, ", "));
	end

	-- Obsolete tech
	if kBuildingInfo.ObsoleteTech then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_OBSOLETE_TECH", GameInfo.Technologies[kBuildingInfo.ObsoleteTech].Description);
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_OBSOLETE_TECH_REMARK");
	end

	-- Required global buildings
	local tPrereqBuildings = {};
	for row in GameInfo.Building_PrereqBuildingClasses{BuildingType = kBuildingInfo.Type} do
		tPrereqBuildings[row.BuildingClassType] = row.NumBuildingNeeded;
	end
	for strBuildingClassType, iNumNeeded in pairs(tPrereqBuildings) do
		if iNumNeeded > 0 then
			local ePrereqBuilding = GetUniqueBuildingFromBuildingClass(GameInfoTypes[strBuildingClassType], pActivePlayer, pCity);
			if ePrereqBuilding ~= -1 then
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_REQUIRED_BUILDING_COUNT_GLOBAL", iNumNeeded, GameInfo.Buildings[ePrereqBuilding].Description);
			else
				AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_REQUIRED_BUILDING_COUNT_GLOBAL", iNumNeeded, GameInfo.BuildingClasses[strBuildingClassType].Description);
			end
		end
	end

	-- Required number of policies
	local iNumRequired = kBuildingInfo.NumPoliciesNeeded;
	if pCity then
		iNumRequired = pCity:GetNumPoliciesNeeded(eBuilding);
	end
	if iNumRequired > 0 then
		if pActivePlayer then
			local iNumHave = pActivePlayer:GetNumPolicies(true);
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_POLICY_NEEDED", iNumRequired, iNumHave);
		else
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_POLICY_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Required number of tier 3 tenets
	AddTooltipPositive(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_T3_TENET_NEEDED", kBuildingInfo.NumRequiredTier3Tenets);

	-- Required unit level
	iNumRequired = kBuildingInfo.LevelPrereq;
	if iNumRequired > 0 then
		if pActivePlayer then
			local iNumHave = pActivePlayer:GetHighestUnitLevel();
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_UNIT_LEVEL_NEEDED", iNumRequired, iNumHave);
		else
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_UNIT_LEVEL_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Required number of cities
	iNumRequired = kBuildingInfo.CitiesPrereq;
	if iNumRequired > 0 then
		if pActivePlayer then
			local iNumHave = pActivePlayer:GetNumCities();
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_CITY_NEEDED", iNumRequired, iNumHave);
		else
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_CITY_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Required national population
	if pActivePlayer then
		iNumRequired = pActivePlayer:GetScalingNationalPopulationRequired(eBuilding);
		if iNumRequired > 0 then
			local iNumHave = pActivePlayer:GetTotalPopulation();
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_POPULATION_NATIONAL_NEEDED", iNumRequired, iNumHave);
		end
	else
		iNumRequired = kBuildingInfo.NationalPopRequired;
		if iNumRequired > 0 then
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_POPULATION_NATIONAL_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Required local population
	iNumRequired = kBuildingInfo.LocalPopRequired;
	if iNumRequired > 0 then
		if pCity then
			local iNumHave = pCity:GetPopulation();
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_POPULATION_LOCAL_NEEDED", iNumRequired, iNumHave);
		else
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_POPULATION_LOCAL_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Required national followers
	iNumRequired = kBuildingInfo.NationalFollowerPopRequired;
	if iNumRequired > 0 then
		if pActivePlayer then
			local iNumHave = pActivePlayer:GetNumFollowerPrimaryReligion();
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_NATIONAL_FOLLOWER_NEEDED", iNumRequired, iNumHave);
		else
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_NATIONAL_FOLLOWER_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Required resources
	for row in GameInfo.Building_ResourceQuantityRequirements{BuildingType = kBuildingInfo.Type} do
		local kResourceInfo = GameInfo.Resources[row.ResourceType];
		AddTooltipPositive(tReqLines, "TXT_KEY_PRODUCTION_RESOURCES_REQUIRED", row.Cost, kResourceInfo.IconString, kResourceInfo.Description);
	end

	-- Required global follower% over population
	iNumRequired = kBuildingInfo.GlobalFollowerPopRequired;
	if iNumRequired > 0 then
		if pActivePlayer then
			iNumRequired = iNumRequired - pActivePlayer:GetReformationFollowerReduction();
			iNumRequired = math.floor(iNumRequired * GameInfo.Worlds[Map.GetWorldSize()].ReformationPercentRequired / 100);
			local iNumHave = pActivePlayer:GetNumGlobalFollowerPrimaryReligion();
			local iNumRequiredReal = math.ceil(Game.GetTotalPopulation() * iNumRequired / 100);
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_GLOBAL_FOLLOWER_NEEDED", iNumRequired, iNumRequiredReal, iNumHave);
		else
			AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_BUILDING_NUM_GLOBAL_FOLLOWER_NEEDED_GENERIC", iNumRequired);
		end
	end

	-- Building class limits
	local iMaxGlobalInstance = kBuildingClassInfo.MaxGlobalInstances;
	if iMaxGlobalInstance ~= -1 then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_MAX_GLOBAL_INSTANCE", iMaxGlobalInstance);
	end
	local iMaxTeamInstance = kBuildingClassInfo.MaxTeamInstances;
	if iMaxTeamInstance ~= -1 then
		AddTooltip(tReqLines, "TXT_KEY_PRODUCTION_MAX_TEAM_INSTANCE", iMaxTeamInstance);
	end
	local iMaxPlayerInstance = kBuildingClassInfo.MaxPlayerInstances;
	if iMaxPlayerInstance ~= -1 then
		local strExtraInstance = pCity and iCurrentExtraInstances > 0 and L("TXT_KEY_PRODUCTION_BUILDING_EXTRA_PLAYER_INSTANCE_SUFFIX", iCurrentExtraInstances) or "";
		table.insert(tReqLines, string.format("%s %s", L("TXT_KEY_PRODUCTION_MAX_PLAYER_INSTANCE", iMaxPlayerInstance), strExtraInstance));
	end

	if next(tReqLines) then
		table.insert(tLines, table.concat(tReqLines, "[NEWLINE]"));
	end

	----------------------
	-- Potential boosts
	-- In city view, only show if the boost has not been obtained
	----------------------
	local tBoostLines = {};
	local tLocalBoostLines = {};

	do
		local tTechBoosts = {};
		local tPolicyBoosts = {};
		local tPolicyBoostsStateReligion = {};
		local tPolicyModifierBoosts = {};
		local tEraBoosts = {};
		local tEraModifierBoosts = {};
		local tAccomplishmentBoosts = {};
		local tCityStrengthBoosts = {};
		local tCSStrategicBoosts = {};
		local tPassingTRBoosts = {};
		local tGABoosts = {};
		local tGABoostCaps = {};
		local tThemeBoosts = {};
		local tMonopolyBoosts = {};
		local tPopulationBoosts = {};
		local tPopulationBoostsGlobal = {};
		local tReligionBoosts = {};
		local tBuildingCountBoosts = {};
		local tPlotCountBoosts = {};
		local tBuildingBoosts = {};
		local tBuildingBoostsGlobal = {};
		local tBuildingModifierBoosts = {};
		local tFranchiseBoosts = {};
		local tCSFriendBoosts = {};
		local tCSAllyBoosts = {};
		local tTerrainBoosts = {};
		local tFeatureBoosts = {};
		local tMountainBoosts = {};
		local tSnowBoosts = {};
		local tImprovementBoosts = {};
		local tImprovementBoostsGlobal = {};
		local tBeliefBoosts = {};
		local tCorporationBoosts = {};
		local tTraitBoosts = {};
		for eYield, kYieldInfo in GameInfoCache("Yields") do
			for row in GameInfo.Building_TechEnhancedYieldChanges{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				local strTechType = row.TechType or kBuildingInfo.EnhancedYieldTech;
				if not strTechType then
					break;
				end

				local eTech = GameInfoTypes[strTechType];

				-- Don't show if the player already owns the tech in city view (only call DLL once per tech)
				if tTechBoosts[eTech] or not (pCity and pActivePlayer and pActivePlayer:HasTech(eTech)) then
					tTechBoosts[eTech] = tTechBoosts[eTech] or {};
					tTechBoosts[eTech][eYield] = row.Yield;
				end
			end

			for row in GameInfo.Policy_BuildingClassYieldChanges{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local ePolicy = GameInfoTypes[row.PolicyType];

				-- Don't show if the player already owns the policy in city view (only call DLL once per policy)
				if tPolicyBoosts[ePolicy] or tPolicyModifierBoosts[ePolicy] or tPolicyBoostsStateReligion[ePolicy] or
				not (pCity and pActivePlayer and pActivePlayer:HasPolicy(ePolicy)) then
					tPolicyBoosts[ePolicy] = tPolicyBoosts[ePolicy] or {};
					tPolicyBoosts[ePolicy][eYield] = row.YieldChange;
				end
			end

			for row in GameInfo.Policy_BuildingClassYieldModifiers{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local ePolicy = GameInfoTypes[row.PolicyType];

				-- Don't show if the player already owns the policy in city view (only call DLL once per policy)
				if tPolicyBoosts[ePolicy] or tPolicyModifierBoosts[ePolicy] or tPolicyBoostsStateReligion[ePolicy] or
				not (pCity and pActivePlayer and pActivePlayer:HasPolicy(ePolicy)) then
					tPolicyModifierBoosts[ePolicy] = tPolicyModifierBoosts[ePolicy] or {};
					tPolicyModifierBoosts[ePolicy][eYield] = row.YieldMod;
				end
			end

			for row in GameInfo.Policy_ReligionBuildingYieldMod{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local ePolicy = GameInfoTypes[row.PolicyType];

				-- Don't show if the player already owns the policy in city view (only call DLL once per policy)
				if tPolicyBoosts[ePolicy] or tPolicyModifierBoosts[ePolicy] or tPolicyBoostsStateReligion[ePolicy] or
				not (pCity and pActivePlayer and pActivePlayer:HasPolicy(ePolicy)) then
					tPolicyBoostsStateReligion[ePolicy] = tPolicyBoostsStateReligion[ePolicy] or {};
					tPolicyBoostsStateReligion[ePolicy][eYield] = row.YieldMod;
				end
			end

			if not (pActivePlayer and pActivePlayer:HasReachedEra(iNumEras - 1)) then
				ExtractSimpleYieldTable(tEraBoosts, "Building_YieldChangesEraScalingTimes100", kYieldInfo);
				ExtractSimpleYieldTable(tEraModifierBoosts, "Building_YieldModifiersEraScaling", kYieldInfo);
			end

			for row in GameInfo.Belief_BuildingClassYieldChanges{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local kBeliefInfo = GameInfo.Beliefs[row.BeliefType];
				local eBelief = kBeliefInfo.ID;

				-- Don't show if the city cannot benefit from this belief (unique belief)
				if not (MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV and pActivePlayer and kBeliefInfo.CivilizationType and GameInfoTypes[kBeliefInfo.CivilizationType] ~= eActiveCiv) then
					-- Don't show if the city already benefits from the belief in city view (only call DLL once per belief)
					if tBeliefBoosts[eBelief] or not (pCity and Game.IsBeliefValid(pCity:GetReligiousMajority(), eBelief, pCity, false)) then
						tBeliefBoosts[eBelief] = tBeliefBoosts[eBelief] or {};
						tBeliefBoosts[eBelief][eYield] = row.YieldChange;
					end
				end
			end

			for row in GameInfo.Corporation_BuildingClassYieldChanges{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local eCorporation = GameInfoTypes[row.CorporationType];

				-- Don't show if the player already has a corporation
				if tCorporationBoosts[eCorporation] or not (pCity and pActivePlayer and pActivePlayer:GetCorporation() ~= -1) then
					tCorporationBoosts[eCorporation] = tCorporationBoosts[eCorporation] or {};
					tCorporationBoosts[eCorporation][eYield] = row.YieldChange;
				end
			end

			for row in GameInfo.Building_YieldChangesFromAccomplishments{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				local kAccomplishmentInfo = GameInfo.Accomplishments[row.AccomplishmentType];
				local eAccomplishment = kAccomplishmentInfo.ID;
				local iMaxAccomplishments = kAccomplishmentInfo.MaxPossibleCompletions;

				-- Don't show if the player already has this accomplishment maxed out
				if tAccomplishmentBoosts[eAccomplishment] or
				not (pCity and iMaxAccomplishments and pActivePlayer and pActivePlayer:GetNumTimesAccomplishmentCompleted(eAccomplishment) >= iMaxAccomplishments) then
					tAccomplishmentBoosts[eAccomplishment] = tAccomplishmentBoosts[eAccomplishment] or {};
					tAccomplishmentBoosts[eAccomplishment][eYield] = tAccomplishmentBoosts[eAccomplishment][eYield] or 0;
					tAccomplishmentBoosts[eAccomplishment][eYield] = tAccomplishmentBoosts[eAccomplishment][eYield] + row.Yield;
				end
			end

			for row in GameInfo.Building_BuildingClassLocalYieldChanges{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local eBoostingBuilding = GameInfoTypes[row.BuildingType];
				if CanPlayerEverBuildCached(eBoostingBuilding) and not CityHasBuilding(eBoostingBuilding) then
					tBuildingBoosts[eBoostingBuilding] = tBuildingBoosts[eBoostingBuilding] or {};
					tBuildingBoosts[eBoostingBuilding][eYield] = row.YieldChange;
				end
			end

			for row in GameInfo.Building_BuildingClassYieldChanges{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local eBoostingBuilding = GameInfoTypes[row.BuildingType];
				if CanPlayerEverBuildCached(eBoostingBuilding) then
					tBuildingBoostsGlobal[eBoostingBuilding] = tBuildingBoostsGlobal[eBoostingBuilding] or {};
					tBuildingBoostsGlobal[eBoostingBuilding][eYield] = row.YieldChange;
				end
			end

			for row in GameInfo.Building_BuildingClassYieldModifiers{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
				local eBoostingBuilding = GameInfoTypes[row.BuildingType];
				if CanPlayerEverBuildCached(eBoostingBuilding) then
					tBuildingModifierBoosts[eBoostingBuilding] = tBuildingModifierBoosts[eBoostingBuilding] or {};
					tBuildingModifierBoosts[eBoostingBuilding][eYield] = row.Modifier;
				end
			end

			for row in GameInfo.Building_YieldPerXTerrainTimes100{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				local eTerrain = GameInfoTypes[row.TerrainType];
				if eTerrain == GameInfoTypes.TERRAIN_MOUNTAIN then
					tMountainBoosts[eYield] = row.Yield / 100;
				elseif eTerrain == GameInfoTypes.TERRAIN_SNOW then
					tSnowBoosts[eYield] = row.Yield / 100;
				else
					tTerrainBoosts[eTerrain] = tTerrainBoosts[eTerrain] or {};
					tTerrainBoosts[eTerrain][eYield] = row.Yield / 100;
				end
			end

			for row in GameInfo.Building_YieldPerXFeatureTimes100{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				local eFeature = GameInfoTypes[row.FeatureType];
				tFeatureBoosts[eFeature] = tFeatureBoosts[eFeature] or {};
				tFeatureBoosts[eFeature][eYield] = row.Yield / 100;
			end

			if bIsWorldWonder then
				for row in GameInfo.Building_YieldChangeWorldWonder{YieldType = kYieldInfo.Type} do
					local eBoostingBuilding = GameInfoTypes[row.BuildingType];
					if CanPlayerEverBuildCached(eBoostingBuilding) and not CityHasBuilding(eBoostingBuilding) then
						tBuildingBoosts[eBoostingBuilding] = tBuildingBoosts[eBoostingBuilding] or {};
						tBuildingBoosts[eBoostingBuilding][eYield] = tBuildingBoosts[eBoostingBuilding][eYield] or 0;
						tBuildingBoosts[eBoostingBuilding][eYield] = tBuildingBoosts[eBoostingBuilding][eYield] + row.Yield;
					end
				end

				for row in GameInfo.Building_YieldChangeWorldWonderGlobal{YieldType = kYieldInfo.Type} do
					local eBoostingBuilding = GameInfoTypes[row.BuildingType];
					if CanPlayerEverBuildCached(eBoostingBuilding) then
						tBuildingBoostsGlobal[eBoostingBuilding] = tBuildingBoostsGlobal[eBoostingBuilding] or {};
						tBuildingBoostsGlobal[eBoostingBuilding][eYield] = tBuildingBoostsGlobal[eBoostingBuilding][eYield] or 0;
						tBuildingBoostsGlobal[eBoostingBuilding][eYield] = tBuildingBoostsGlobal[eBoostingBuilding][eYield] + row.Yield;
					end
				end
			end

			for row in GameInfo.Building_YieldChangesPerPop{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				tPopulationBoosts[eYield] = row.Yield / 100;
			end

			for row in GameInfo.Building_YieldChangesPerPopInEmpire{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				tPopulationBoostsGlobal[eYield] = row.Yield / 100;
			end

			for row in GameInfo.Building_YieldChangesPerCityStrengthTimes100{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				tCityStrengthBoosts[eYield] = row.Yield / 100;
			end

			for row in GameInfo.Building_YieldChangesPerReligion{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				tReligionBoosts[eYield] = row.Yield / 100;
			end

			local iGAYield = 0;
			local iGAYieldCap = 0;
			for row in GameInfo.Building_YieldChangesPerGoldenAge{BuildingType = kBuildingInfo.Type, YieldType = kYieldInfo.Type} do
				iGAYield = row.Yield;
				iGAYieldCap = row.YieldCap;
			end
			if iGAYield > 0 then
				table.insert(tGABoosts, GetYieldBoostString(kYieldInfo, iGAYield));
			end
			if iGAYieldCap > 0 then
				table.insert(tGABoostCaps, GetYieldBoostString(kYieldInfo, iGAYieldCap));
			end

			-- Don't show in city view
			if not pCity then
				-- This stacks when a civ has two traits boosting the same building, but not when there are two rows of the same trait, building, and yield types
				-- Too complex - we'll just assume it always stacks and let the modder's mistakes show
				for row in GameInfo.Trait_BuildingClassYieldChanges{BuildingClassType = kBuildingClassInfo.Type, YieldType = kYieldInfo.Type} do
					for _, eCiv in ipairs(GetCivsFromTrait(row.TraitType)) do
						tTraitBoosts[eCiv] = tTraitBoosts[eCiv] or {};
						tTraitBoosts[eCiv][eYield] = tTraitBoosts[eCiv][eYield] or 0;
						tTraitBoosts[eCiv][eYield] = tTraitBoosts[eCiv][eYield] + row.YieldChange;
					end
				end
			end

			ExtractYieldFractionTable(tImprovementBoosts, "Building_YieldPerXImprovementLocal", kYieldInfo, "ImprovementType");
			ExtractYieldFractionTable(tImprovementBoostsGlobal, "Building_YieldPerXImprovementGlobal", kYieldInfo, "ImprovementType");
			ExtractSimpleYieldFractionTable(tBuildingCountBoosts, "Building_YieldChangesPerXBuilding", kYieldInfo);
			ExtractSimpleYieldFractionTable(tCSStrategicBoosts, "Building_YieldChangesFromXCityStateStrategicResource", kYieldInfo);
			ExtractSimpleYieldFractionTable(tPlotCountBoosts, "Building_YieldChangesPerXTiles", kYieldInfo);
			ExtractSimpleYieldTable(tPassingTRBoosts, "Building_YieldChangesFromPassingTR", kYieldInfo);
			ExtractSimpleYieldTable(tThemeBoosts, "Building_YieldChangesPerLocalTheme", kYieldInfo);
			ExtractSimpleYieldTable(tMonopolyBoosts, "Building_YieldChangesPerMonopoly", kYieldInfo);
			ExtractSimpleYieldTable(tFranchiseBoosts, "Building_YieldPerFranchise", kYieldInfo);
			ExtractSimpleYieldTable(tCSFriendBoosts, "Building_YieldPerFriend", kYieldInfo);
			ExtractSimpleYieldTable(tCSAllyBoosts, "Building_YieldPerAlly", kYieldInfo);
		end

		-- Special case for the TechEnhancedTourism column
		if kBuildingInfo.EnhancedYieldTech and kBuildingInfo.TechEnhancedTourism > 0 then
			local eTech = GameInfoTypes[kBuildingInfo.EnhancedYieldTech];
			if tTechBoosts[eTech] or not (pCity and pActivePlayer and pActivePlayer:HasTech(eTech)) then
				local eYield = GameInfoTypes.YIELD_TOURISM;
				tTechBoosts[eTech] = tTechBoosts[eTech] or {};
				tTechBoosts[eTech][eYield] = tTechBoosts[eTech][eYield] or 0;
				tTechBoosts[eTech][eYield] = tTechBoosts[eTech][eYield] + kBuildingInfo.TechEnhancedTourism;
			end
		end

		for row in GameInfo.Policy_BuildingClassCultureChanges{BuildingClassType = kBuildingClassInfo.Type} do
			local ePolicy = GameInfoTypes[row.PolicyType];
			if tPolicyBoosts[ePolicy] or tPolicyModifierBoosts[ePolicy] or tPolicyBoostsStateReligion[ePolicy] or
			not (pCity and pActivePlayer and pActivePlayer:HasPolicy(ePolicy)) then
				local eYield = GameInfoTypes.YIELD_CULTURE;
				tPolicyBoosts[ePolicy] = tPolicyBoosts[ePolicy] or {};
				tPolicyBoosts[ePolicy][eYield] = tPolicyBoosts[ePolicy][eYield] or 0;
				tPolicyBoosts[ePolicy][eYield] = tPolicyBoosts[ePolicy][eYield] + row.CultureChange;
			end
		end

		for row in GameInfo.Policy_BuildingClassTourismModifiers{BuildingClassType = kBuildingClassInfo.Type} do
			local ePolicy = GameInfoTypes[row.PolicyType];
			if tPolicyBoosts[ePolicy] or tPolicyModifierBoosts[ePolicy] or tPolicyBoostsStateReligion[ePolicy] or
			not (pCity and pActivePlayer and pActivePlayer:HasPolicy(ePolicy)) then
				local eYield = GameInfoTypes.YIELD_TOURISM;
				tPolicyModifierBoosts[ePolicy] = tPolicyModifierBoosts[ePolicy] or {};
				tPolicyModifierBoosts[ePolicy][eYield] = tPolicyModifierBoosts[ePolicy][eYield] or 0;
				tPolicyModifierBoosts[ePolicy][eYield] = tPolicyModifierBoosts[ePolicy][eYield] + row.TourismModifier;
			end
		end

		for row in GameInfo.Belief_BuildingClassTourism{BuildingClassType = kBuildingClassInfo.Type} do
			local kBeliefInfo = GameInfo.Beliefs[row.BeliefType];
			local eBelief = kBeliefInfo.ID;
			if not (MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV and pActivePlayer and kBeliefInfo.CivilizationType and GameInfoTypes[kBeliefInfo.CivilizationType] ~= eActiveCiv) then
				if tBeliefBoosts[eBelief] or not (pCity and Game.IsBeliefValid(pCity:GetReligiousMajority(), eBelief, pCity, false)) then
					local eYield = GameInfoTypes.YIELD_TOURISM;
					tBeliefBoosts[eBelief] = tBeliefBoosts[eBelief] or {};
					tBeliefBoosts[eBelief][eYield] = tBeliefBoosts[eBelief][eYield] or 0;
					tBeliefBoosts[eBelief][eYield] = tBeliefBoosts[eBelief][eYield] + row.Tourism;
				end
			end
		end

		if not pCity and bIsCultureBuilding then
			for kTraitInfo in GameInfo.Traits("CultureBuildingYieldChange <> 0") do
				for _, eCiv in ipairs(GetCivsFromTrait(kTraitInfo.Type)) do
					local eYield = GameInfoTypes.YIELD_CULTURE;
					tTraitBoosts[eCiv] = tTraitBoosts[eCiv] or {};
					tTraitBoosts[eCiv][eYield] = tTraitBoosts[eCiv][eYield] or 0;
					tTraitBoosts[eCiv][eYield] = tTraitBoosts[eCiv][eYield] + kTraitInfo.CultureBuildingYieldChange;
				end
			end
		end

		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_POPULATION", tPopulationBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_POPULATION_GLOBAL", tPopulationBoostsGlobal);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_CITY_STRENGTH", tCityStrengthBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_PASSING_TR", tPassingTRBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_THEMES", tThemeBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_MONOPOLIES", tMonopolyBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_FRANCHISES", tFranchiseBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_RELIGION_COUNT", tReligionBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_CITY_STATE_FRIENDS", tCSFriendBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_CITY_STATE_ALLIES", tCSAllyBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_ERA", tEraBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_MOUNTAINS", tMountainBoosts);
		AddTooltipSimpleYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_SNOW", tSnowBoosts);
		AddTooltipSimpleYieldFractionTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_BUILDING_COUNT", tBuildingCountBoosts);
		AddTooltipSimpleYieldFractionTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_PLOT_COUNT", tPlotCountBoosts);
		AddTooltipSimpleYieldFractionTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_CITY_STATE_STRATEGIC_RESOURCES", tCSStrategicBoosts);
		AddTooltipSimpleYieldModifierTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_ERA", tEraModifierBoosts);
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_BUILDING", tBuildingBoosts, "Buildings");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_BUILDING_GLOBAL", tBuildingBoostsGlobal, "Buildings");
		AddTooltipsYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_WORKED_TERRAIN", tTerrainBoosts, "Terrains");
		AddTooltipsYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_WORKED_FEATURE", tFeatureBoosts, "Features");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_TECH", tTechBoosts, "Technologies");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_POLICY", tPolicyBoosts, "Policies");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_POLICY_STATE_RELIGION", tPolicyBoostsStateReligion, "Policies");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_BELIEF", tBeliefBoosts, "Beliefs");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_CORPORATION", tCorporationBoosts, "Corporations");
		AddTooltipsYieldBoostTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_TRAIT", tTraitBoosts, "Civilizations");
		AddTooltipsYieldBoostTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_ACCOMPLISHMENT", tAccomplishmentBoosts, "Accomplishments");
		AddTooltipsYieldFractionTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_IMPROVEMENT", tImprovementBoosts, "Improvements");
		AddTooltipsYieldFractionTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_IMPROVEMENT_GLOBAL", tImprovementBoostsGlobal, "Improvements");
		AddTooltipsYieldModifierTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_BUILDING_GLOBAL", tBuildingModifierBoosts, "Buildings");
		AddTooltipsYieldModifierTable(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_POLICY", tPolicyModifierBoosts, "Policies");

		if next(tGABoosts) then
			AddTooltip(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_GOLDEN_AGE_START", table.concat(tGABoosts, " "), table.concat(tGABoostCaps, " "));
		end

		local tBoostsFromResource = {};
		for row in GameInfo.Building_ResourceCultureChanges{BuildingType = kBuildingInfo.Type} do
			tBoostsFromResource[row.ResourceType] = tBoostsFromResource[row.ResourceType] or {};
			tBoostsFromResource[row.ResourceType].YIELD_CULTURE = row.CultureChange;
		end
		for row in GameInfo.Building_ResourceFaithChanges{BuildingType = kBuildingInfo.Type} do
			tBoostsFromResource[row.ResourceType] = tBoostsFromResource[row.ResourceType] or {};
			tBoostsFromResource[row.ResourceType].YIELD_FAITH = row.FaithChange;
		end
		for row in GameInfo.Building_ResourceHappinessChange{BuildingType = kBuildingInfo.Type} do
			tBoostsFromResource[row.ResourceType] = tBoostsFromResource[row.ResourceType] or {};
			tBoostsFromResource[row.ResourceType].HAPPINESS = row.HappinessChange;
		end
		for strResourceType, tResourceYields in pairs(tBoostsFromResource) do
			local kResourceInfo = GameInfo.Resources[strResourceType];
			local tBoostStrings = {};
			local iYield = tResourceYields.YIELD_CULTURE or 0;
			if iYield ~= 0 then
				table.insert(tBoostStrings, GetYieldBoostString(GameInfo.Yields.YIELD_CULTURE, iYield));
			end
			iYield = tResourceYields.YIELD_FAITH or 0;
			if iYield ~= 0 then
				table.insert(tBoostStrings, GetYieldBoostString(GameInfo.Yields.YIELD_FAITH, iYield));
			end
			if next(tBoostStrings) then
				AddTooltip(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_LOCAL_RESOURCE",
				kResourceInfo.IconString .. " " .. L(kResourceInfo.Description), table.concat(tBoostStrings, " "));
			end

			local iHappiness = tResourceYields.HAPPINESS or 0;
			AddTooltipNonZeroSigned(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_LOCAL_RESOURCE",
			iHappiness, kResourceInfo.IconString, kResourceInfo.Description);
		end

		AddTooltipsYieldModifierTable(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_YIELD_BOOST_FROM_LOCAL_RESOURCE", tResourceModifiers, "Resources");

		for row in GameInfo.Building_BonusFromAccomplishments{BuildingType = kBuildingInfo.Type} do
			local kAccomplishmentInfo = GameInfo.Accomplishments[row.AccomplishmentType];
			AddTooltipNonZeroSigned(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_ACCOMPLISHMENT", row.Happiness, kAccomplishmentInfo.Description);
			if row.DomainType and row.DomainXP ~= 0 then
				AddTooltipNonZeroSigned(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_XP_FOR_DOMAIN_FROM_ACCOMPLISHMENT", row.DomainXP,
					kAccomplishmentInfo.Description, GameInfo.Domains[row.DomainType].Description);
			end
			if row.UnitCombatType and row.UnitProductionModifier ~= 0 then
				AddTooltipNonZeroSigned(tLocalBoostLines, "TXT_KEY_PRODUCTION_BUILDING_UNIT_COMBAT_PRODUCTION_MODIFIER_FROM_ACCOMPLISHMENT", row.UnitProductionModifier,
				kAccomplishmentInfo.Description, GameInfo.UnitCombatInfos[row.UnitCombatType].Description);
			end
		end

		for row in GameInfo.Building_BuildingClassLocalHappiness{BuildingClassType = kBuildingClassInfo.Type} do
			AddTooltipNonZeroSigned(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_BUILDING",
			row.Happiness, GameInfo.Buildings[row.BuildingType].Description);
		end

		for row in GameInfo.Building_BuildingClassHappiness{BuildingClassType = kBuildingClassInfo.Type} do
			AddTooltipNonZeroSigned(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_BUILDING_GLOBAL",
			row.Happiness, GameInfo.Buildings[row.BuildingType].Description);
		end

		for row in GameInfo.Policy_BuildingClassHappiness{BuildingClassType = kBuildingClassInfo.Type} do
			local kPolicyInfo = GameInfo.Policies[row.PolicyType];
			if tPolicyBoosts[kPolicyInfo.ID] or tPolicyModifierBoosts[kPolicyInfo.ID] or tPolicyBoostsStateReligion[kPolicyInfo.ID] or
			not (pCity and pActivePlayer and pActivePlayer:HasPolicy(kPolicyInfo.ID)) then
				AddTooltipNonZeroSigned(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_POLICY", row.Happiness, kPolicyInfo.Description);
			end
		end

		for row in GameInfo.Policy_BuildingClassSecurityChanges{BuildingClassType = kBuildingClassInfo.Type} do
			local kPolicyInfo = GameInfo.Policies[row.PolicyType];
			if tPolicyBoosts[kPolicyInfo.ID] or tPolicyModifierBoosts[kPolicyInfo.ID] or tPolicyBoostsStateReligion[kPolicyInfo.ID] or
			not (pCity and pActivePlayer and pActivePlayer:HasPolicy(kPolicyInfo.ID)) then
				AddTooltipNonZeroSigned(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_CITY_SECURITY_FROM_POLICY", row.SecurityChange, kPolicyInfo.Description);
			end
		end

		for row in GameInfo.Belief_BuildingClassHappiness{BuildingClassType = kBuildingClassInfo.Type} do
			local kBeliefInfo = GameInfo.Beliefs[row.BeliefType];
			if not (MOD_BALANCE_UNIQUE_BELIEFS_ONLY_FOR_CIV and pActivePlayer and kBeliefInfo.CivilizationType and GameInfoTypes[kBeliefInfo.CivilizationType] ~= eActiveCiv) then
				if tBeliefBoosts[kBeliefInfo.ID] or not (pCity and Game.IsBeliefValid(pCity:GetReligiousMajority(), kBeliefInfo.ID, pCity, false)) then
					AddTooltipNonZeroSigned(tBoostLines, "TXT_KEY_PRODUCTION_BUILDING_HAPPINESS_FROM_BELIEF", row.Happiness, kBeliefInfo.ShortDescription);
				end
			end
		end
	end

	if next(tExtraInstanceLines) then
		table.insert(tBoostLines, table.concat(tExtraInstanceLines, "[NEWLINE]"));
	end

	do
		local tLocalEffectStr = {};
		if next(tLocalAbilityLines) then
			table.insert(tLocalEffectStr, "[ICON_BULLET]" .. table.concat(tLocalAbilityLines, "[NEWLINE][ICON_BULLET]"));
		end
		if next(tLocalBoostLines) then
			table.insert(tLocalEffectStr, table.concat(tLocalBoostLines, "[NEWLINE]"));
		end
		if next(tLocalEffectStr) then
			table.insert(tAbilityLines, iLocalAbilityPos, table.concat(tLocalEffectStr, "[NEWLINE]"));
		end
	end

	if next(tAbilityLines) then
		table.insert(tLines, iAbilityInsertPos, table.concat(tAbilityLines, "[NEWLINE]"));
	end

	if bBoostedYields or bBoostedModifiers or bBoostedHappiness then
		local tProjectedLines = {}
		if bBoostedYields then
			AddTooltipSimpleYieldBoostTable(tProjectedLines, "TXT_KEY_PRODUCTION_BUILDING_PROJECTED_YIELD_CHANGE", tProjectedYields);
		end
		if bBoostedModifiers then
			AddTooltipSimpleYieldBoostTable(tProjectedLines, "TXT_KEY_PRODUCTION_BUILDING_PROJECTED_YIELD_MODIFIER", tProjectedModifiers);
		end
		if bBoostedHappiness then
			AddTooltipNonZeroSigned(tProjectedLines, "TXT_KEY_PRODUCTION_BUILDING_PROJECTED_HAPPINESS", iHappinessTotal);
		end
		table.insert(tLines, table.concat(tProjectedLines, "[NEWLINE]"));
	end

	if next(tBoostLines) then
		local strHeadlineKey = pCity and "TXT_KEY_PRODUCTION_BUILDING_POTENTIAL_BOOSTS" or "TXT_KEY_PRODUCTION_BUILDING_ALL_BOOSTS";
		table.insert(tBoostLines, 1, L(strHeadlineKey));
		table.insert(tLines, table.concat(tBoostLines, "[NEWLINE]"));
	end

	----------------------
	-- Footer section(s)
	----------------------

	-- Pre-written Help text
	if kBuildingInfo.Help then
		local strWrittenHelp = L(kBuildingInfo.Help);
		if strWrittenHelp ~= "" then
			table.insert(tLines, strWrittenHelp);
		end
	end

	if pCity then
		-- Investment rules (for unbuilt buildings only)
		if MOD_BALANCE_BUILDING_INVESTMENTS and pCity:GetNumRealBuilding(eBuilding) <= 0 and pCity:GetNumFreeBuilding(eBuilding) <= 0 then
			local iAmount = BALANCE_BUILDING_INVESTMENT_BASELINE * -1;
			local iWonderAmount = iAmount / 2;
			AddTooltip(tLines, "TXT_KEY_PRODUCTION_BUILDING_INVESTMENT", iAmount, iWonderAmount);
		end

		-- Wonder cost increase
		if MOD_BALANCE_WORLD_WONDER_COST_INCREASE then
			AddTooltipPositive(tLines, "TXT_KEY_PRODUCTION_BUILDING_WONDER_COST_INCREASE_METRIC", pCity:GetWorldWonderCost(eBuilding));
		end
	end

	return table.concat(tLines, "[NEWLINE]" .. SEPARATOR_STRING .. "[NEWLINE]");
end

function GetHelpTextForImprovement(eImprovement, bExcludeName, bExcludeHeader)
	local kImprovementInfo = GameInfo.Improvements[eImprovement];
	local tLines = {};

	-- Header
	local bHaveHeader = false;
	if not bExcludeHeader then
		-- Name
		if not bExcludeName then
			table.insert(tLines, Locale.ToUpper(L(kImprovementInfo.Description)));
			table.insert(tLines, SEPARATOR_STRING);
		end
	end

	if bHaveHeader then
		table.insert(tLines, SEPARATOR_STRING);
	end

	-- Pre-written Help text
	if not kImprovementInfo.Help then
		-- print("Improvement help is NULL:", L(kImprovementInfo.Description));
	else
		local strWrittenHelp = L(kImprovementInfo.Help);
		-- Will include separator if there is extra info
		-- table.insert(tLines, SEPARATOR_STRING);
		table.insert(tLines, strWrittenHelp);
	end

	return table.concat(tLines, "[NEWLINE]");
end

function GetHelpTextForProject(eProject, pCity, bGeneralInfo)
	local kProjectInfo = GameInfo.Projects[eProject];

	--- @type Player?
	local pActivePlayer = Game and Players[Game.GetActivePlayer()];

	-- Only general info if Game object doesn't exist (e.g. in pregame)
	if not Game then
		bGeneralInfo = true;
	end

	-- When viewing a (foreign) city, always show tooltips as they are for the city owner
	if pCity then
		pActivePlayer = Players[pCity:GetOwner()];
	end

	-- Sometimes a city is needed in tooltips not in city view; in that case use the capital city
	local pActiveCity = pCity or pActivePlayer and pActivePlayer:GetCapitalCity();

	-- Invalidate pCity, pActivePlayer, pActiveCity if we only want general info (then we don't have to additionally check for bGeneralInfo on top of nil checks)
	if bGeneralInfo then
		pCity = nil;
		pActivePlayer = nil;
		pActiveCity = nil;
	end

	local tLines = {};
	local tHeaderLines = {};

	-- Name
	table.insert(tHeaderLines, Locale.ToUpper(L(kProjectInfo.Description)));

	-- Unique Project?
	if kProjectInfo.CivilizationType then
		AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_EXCLUSIVE_PROJECT", GameInfo.Civilizations[kProjectInfo.CivilizationType].Adjective);
	end

	-- Cost
	local iCost;
	if pActiveCity then
		iCost = pActiveCity:GetProjectProductionNeeded(eProject);
	elseif pActivePlayer then
		iCost = pActivePlayer:GetProjectProductionNeeded(eProject);
	else
		iCost = kProjectInfo.Cost;
	end
	AddTooltip(tHeaderLines, "TXT_KEY_PRODUCTION_COST", iCost);

	table.insert(tLines, table.concat(tHeaderLines, "[NEWLINE]"));

	-- Pre-written Help text
	if not kProjectInfo.Help then
		print("Project help is NULL:", L(kProjectInfo.Description));
	else
		local strWrittenHelp = L(kProjectInfo.Help);
		table.insert(tLines, strWrittenHelp);
	end

	return table.concat(tLines, "[NEWLINE]" .. SEPARATOR_STRING .. "[NEWLINE]");
end

function GetHelpTextForProcess(eProcess)
	local kProcessInfo = GameInfo.Processes[eProcess];
	local tLines = {};

	-- Name
	table.insert(tLines, Locale.ToUpper(L(kProcessInfo.Description)));

	-- This process builds a World Congress project?
	local kLeagueProjectInfo;
	local pLeague = Game.GetActiveLeague();
	if pLeague and not Game.IsOption("GAMEOPTION_NO_LEAGUES") then
		kLeagueProjectInfo = GameInfo.LeagueProjects{Process = kProcessInfo.Type}(); -- there should only be one
	end

	local strWrittenHelp;
	if kProcessInfo.Help then
		-- Pre-written Help text
		strWrittenHelp = L(kProcessInfo.Help);
	elseif kLeagueProjectInfo then
		-- Auto-generated Help text
		strWrittenHelp = L("TXT_KEY_TT_PROCESS_LEAGUE_PROJECT_HELP", kLeagueProjectInfo.Description);
	end

	if strWrittenHelp and strWrittenHelp ~= "" then
		table.insert(tLines, SEPARATOR_STRING);
		table.insert(tLines, strWrittenHelp);
	end

	-- Project details
	if kLeagueProjectInfo then
		table.insert(tLines, "");
		table.insert(tLines, pLeague:GetProjectDetails(kLeagueProjectInfo.ID, Game.GetActivePlayer()));
	end

	return table.concat(tLines, "[NEWLINE]");
end

--- Generate the tooltips for a type of specialist slot, both when filled and empty<br>
--- TODO: Also account for GPP modifiers
--- @param eSpecialist SpecialistType
--- @param pCity City
--- @return string strTooltip # Tooltip of the specialist slot when it is filled
--- @return string strEmptyTooltip # Tooltip of the specialist slot when it is empty
function GetHelpTextForSpecialist(eSpecialist, pCity)
	local kSpecialistInfo = GameInfo.Specialists[eSpecialist];
	local kGreatPersonInfo = GetGreatPersonInfoFromSpecialist(kSpecialistInfo.Type);
	local strTooltip = L(kSpecialistInfo.Description);

	local iCultureFromSpecialist = pCity:GetCultureFromSpecialist(eSpecialist);
	for eYield, kYieldInfo in GameInfoCache("Yields") do
		local iYield = pCity:GetSpecialistYield(eSpecialist, eYield) + pCity:GetSpecialistYieldChange(eSpecialist, eYield);
		if eYield == YieldTypes.YIELD_CULTURE then
			iYield = iYield + iCultureFromSpecialist;
		end
		if iYield > 0 then
			strTooltip = string.format("%s +%d%s", strTooltip, iYield, kYieldInfo.IconString);
		end
	end
	local iGPP = kSpecialistInfo.GreatPeopleRateChange + pCity:GetEventGPPFromSpecialists();
	if iGPP > 0 then
		strTooltip = string.format("%s +%d%s", strTooltip, iGPP, kGreatPersonInfo.IconString);
	end
	local strEmptyTooltip = string.format("%s[NEWLINE](%s)", EMPTY_SLOT_STRING, strTooltip);
	return strTooltip, strEmptyTooltip;
end

-------------------------------------------------
-- Yield tooltips
-------------------------------------------------
function GetFoodTooltip(pCity)
	local tLines = {};

	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_FOOD_HELP_INFO"));
		table.insert(tLines, "");
	end

	local fFoodProgress = pCity:GetFoodTimes100() / 100;
	local iFoodNeeded = pCity:GrowthThreshold();

	table.insert(tLines, L("TXT_KEY_FOOD_PROGRESS", fFoodProgress, iFoodNeeded));
	table.insert(tLines, "");
	table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_FOOD));

	if MOD_BALANCE_VP then
		table.insert(tLines, "");
		table.insert(tLines, pCity:getPotentialUnhappinessWithGrowth());
	end

	return table.concat(tLines, "[NEWLINE]");
end

function GetProductionTooltip(pCity)
	local tLines = {};

	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_PRODUCTION_HELP_INFO"));
		table.insert(tLines, "");
	end

	table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_PRODUCTION));

	return table.concat(tLines, "[NEWLINE]");
end

function GetGoldTooltip(pCity)
	local tLines = {};

	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_GOLD_HELP_INFO"));
		table.insert(tLines, "");
	end

	table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_GOLD));

	return table.concat(tLines, "[NEWLINE]");
end

function GetScienceTooltip(pCity)
	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
		return L("TXT_KEY_TOP_PANEL_SCIENCE_OFF_TOOLTIP");
	end

	local tLines = {};

	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_SCIENCE_HELP_INFO"));
		table.insert(tLines, "");
	end

	table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_SCIENCE));

	return table.concat(tLines, "[NEWLINE]");
end

function GetCultureTooltip(pCity)
	local tLines = {};
	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES) then
		table.insert(tLines, L("TXT_KEY_TOP_PANEL_POLICIES_OFF_TOOLTIP"));
	else
		if not OptionsManager.IsNoBasicHelp() then
			table.insert(tLines, L("TXT_KEY_CULTURE_HELP_INFO"));
			table.insert(tLines, "");
		end
		table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_CULTURE));
	end

	return table.concat(tLines, "[NEWLINE]");
end

function GetBorderGrowthTooltip(pCity)
	local tLines = {};

	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_BORDER_GROWTH_HELP_INFO"));
		table.insert(tLines, "");
	end
	local eYield = YieldTypes.YIELD_CULTURE_LOCAL;
	table.insert(tLines, pCity:GetYieldRateTooltip(eYield));
	local fCultureStored = pCity:GetJONSCultureStoredTimes100() / 100;
	local iCultureNeeded = pCity:GetJONSCultureThreshold();
	local iBorderGrowth = pCity:GetYieldRateTimes100(eYield) / 100;

	local strBorderGrowthProgress = L("TXT_KEY_CULTURE_INFO", fCultureStored, iCultureNeeded);
	if iBorderGrowth > 0 then
		local iCultureTurns = math.ceil((iCultureNeeded - fCultureStored) / iBorderGrowth);
		strBorderGrowthProgress = strBorderGrowthProgress .. " " .. L("TXT_KEY_CULTURE_TURNS", iCultureTurns);
	end
	table.insert(tLines, strBorderGrowthProgress);

	return table.concat(tLines, "[NEWLINE]");
end

function GetFaithTooltip(pCity)
	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION) then
		return L("TXT_KEY_TOP_PANEL_RELIGION_OFF_TOOLTIP");
	end

	local tLines = {};
	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_FAITH_HELP_INFO"));
		table.insert(tLines, "");
	end

	table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_FAITH));
	table.insert(tLines, SEPARATOR_STRING);

	-- Religion info
	table.insert(tLines, GetReligionTooltip(pCity));

	return table.concat(tLines, "[NEWLINE]");
end

function GetTourismTooltip(pCity)
	local tLines = {};
	if not OptionsManager.IsNoBasicHelp() then
		table.insert(tLines, L("TXT_KEY_TOURISM_HELP_INFO"));
	end

	table.insert(tLines, pCity:GetYieldRateTooltip(YieldTypes.YIELD_TOURISM));

	return table.concat(tLines, "[NEWLINE]");
end

function GetCityHappinessTooltip(pCity)
	return pCity:GetCityHappinessBreakdown();
end

function GetCityUnhappinessTooltip(pCity)
	return pCity:GetCityUnhappinessBreakdown(true);
end

----------------------------------------------------------------
-- Diplomacy overview / player icon tooltip
----------------------------------------------------------------
function GetMoodInfo(eOtherPlayer)
	local eActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[eActivePlayer];
	local eActiveTeam = Game.GetActiveTeam();
	local pActiveTeam = Teams[eActiveTeam];
	local pOtherPlayer = Players[eOtherPlayer];
	local eOtherTeam = pOtherPlayer:GetTeam();
	local pOtherTeam = Teams[eOtherTeam];
	local iVisibleApproach = pActivePlayer:GetApproachTowardsUsGuess(eOtherPlayer);

	-- Always war!
	if pActiveTeam:IsAtWar(eOtherTeam) then
		if Game.IsOption(GameOptionTypes.GAMEOPTION_ALWAYS_WAR) or Game.IsOption(GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE) then
			return "[ICON_BULLET]" .. L("TXT_KEY_ALWAYS_WAR_TT");
		end
	end

	-- Get the opinion modifier table from the DLL and convert it into bullet points
	local tOpinion = pOtherPlayer:GetOpinionTable(eActivePlayer);
	if next(tOpinion) then
		return "[ICON_BULLET]" .. table.concat(tOpinion, "[NEWLINE][ICON_BULLET]");
	end

	-- No specific modifiers are visible, so let's see what string we should use (based on visible approach towards us)
	-- Eliminated
	if not pOtherPlayer:IsAlive() then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_ELIMINATED_INDICATOR");
	end

	-- Teammates
	if eActiveTeam == eOtherTeam then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_HUMAN_TEAMMATE");
	end

	-- At war with us
	if pActiveTeam:IsAtWar(eOtherTeam) then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_AT_WAR");
	end

	-- Appears Friendly
	if iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_FRIENDLY");
	end

	-- Appears Afraid
	if iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_AFRAID");
	end

	-- Appears Guarded
	if iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_GUARDED");
	end

	-- Appears Hostile
	if iVisibleApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE then
		return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_HOSTILE");
	end

	-- Appears Neutral, opinions deliberately hidden
	if Game.IsHideOpinionTable() then
		if pOtherPlayer:IsActHostileTowardsHuman(eActivePlayer) then
			return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_NEUTRAL_HOSTILE");
		end
		if pOtherTeam:GetTurnsSinceMeetingTeam(eActiveTeam) ~= 0 then
			return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_NEUTRAL_FRIENDLY");
		end
	end

	-- Appears Neutral, no opinions
	return "[ICON_BULLET]" .. L("TXT_KEY_DIPLO_DEFAULT_STATUS");
end

----------------------------------------------------------------
-- Religion tooltip
----------------------------------------------------------------
function GetReligionTooltip(pCity)
	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION) then
		return "";
	end

	local tReligions = {};
	local eMajorityReligion = pCity:GetReligiousMajority();
	local tLines = {};
	local tReligionFollowers = {};
	local iPressureMultiplier = GameDefines.RELIGION_MISSIONARY_PRESSURE_MULTIPLIER;

	-- First, determine the list of religions in this city and the follower count of each
	-- Religions with no followers are not shown even if there is accumulated pressure
	-- Also add the Holy City line at the top
	for eReligion, kReligionInfo in GameInfoCache("Religions") do
		local strReligionName = L(Game.GetReligionName(eReligion));
		local strIconString = kReligionInfo.IconString;
		tReligionFollowers[eReligion] = pCity:GetNumFollowers(eReligion);

		if pCity:IsHolyCityForReligion(eReligion) then
			table.insert(tLines, L("TXT_KEY_HOLY_CITY_TOOLTIP_LINE", strIconString, strReligionName));
		end

		if eReligion == eMajorityReligion then
			table.insert(tReligions, 1, kReligionInfo);
		elseif tReligionFollowers[eReligion] > 0 then
			table.insert(tReligions, kReligionInfo);
		end
	end

	-- Sort the religion list by majority and follower count, greater number first
	local function CompareReligion(kReligionInfo1, kReligionInfo2)
		local eReligion1 = kReligionInfo1.ID;
		local eReligion2 = kReligionInfo2.ID;
		if eMajorityReligion == eReligion1 then
			return true;
		end
		if eMajorityReligion == eReligion2 then
			return false;
		end
		return tReligionFollowers[eReligion1] > tReligionFollowers[eReligion2];
	end

	table.sort(tReligions, CompareReligion);

	-- Now generate the tooltips
	for _, kReligionInfo in ipairs(tReligions) do
		local eReligion = kReligionInfo.ID;
		local strIconString = kReligionInfo.IconString;
		local iPressure, _, iExistingPressure = pCity:GetPressurePerTurn(eReligion);
		local iFollowers = pCity:GetNumFollowers(eReligion);
		local iDisplayPressure = math.floor(iPressure / iPressureMultiplier);
		local iDisplayExistingPressure = math.floor(iExistingPressure / iPressureMultiplier);

		local strPressure = "";
		if iDisplayPressure > 0 or iDisplayExistingPressure > 0 then
			strPressure = L("TXT_KEY_RELIGIOUS_PRESSURE_STRING_EXTENDED", iDisplayExistingPressure, iDisplayPressure);
		end

		table.insert(tLines, L("TXT_KEY_RELIGION_TOOLTIP_LINE", strIconString, iFollowers, strPressure));
	end

	if #tReligions == 0 then
		return L("TXT_KEY_RELIGION_NO_FOLLOWERS");
	end

	return table.concat(tLines, "[NEWLINE]");
end

(function()
	if VP then
		return
	end

	local global = MapModData or _G

	if global and global.VP then
		VP = global.VP
		return
	end

	print("VPUI - Populating VP core library");
	local MOD_BALANCE_CORE_YIELDS = GameInfo.CustomModOptions { Name = "BALANCE_CORE_YIELDS" } ().Value == 1;
	local MOD_BALANCE_CORE_JFD = GameInfo.CustomModOptions { Name = "BALANCE_CORE_JFD" } ().Value == 1;

	local iNumYields = Game and Game.GetNumYieldTypes and Game.GetNumYieldTypes();
	if not iNumYields then
		local iNumJFDYields = GameInfo.Yields { Type = "YIELD_JFD_SOVEREIGNTY" } ().ID + 1;
		local iNumCoreYields = GameInfo.Yields { Type = "YIELD_CULTURE_LOCAL" } ().ID + 1;
		local iNumBaseYields = GameInfo.Yields { Type = "YIELD_GOLDEN_AGE_POINTS" } ().ID + 1;
		iNumYields = MOD_BALANCE_CORE_JFD and iNumJFDYields or (MOD_BALANCE_CORE_YIELDS and iNumCoreYields or iNumBaseYields);
	end

	-- Yields are so frequently accessed it is worth to cache the ID, Type, IconString, and Description of each
	-- The ID + 1 should always match the index of this table due to the remapper in DLL
	local tYieldInfos = {};
	for eYield = 0, iNumYields - 1 do
		local kYieldInfo = GameInfo.Yields[eYield];
		table.insert(tYieldInfos, {
			ID = eYield,
			Type = kYieldInfo.Type,
			IconString = kYieldInfo.IconString,
			Description = kYieldInfo.Description,
		});
	end

	-- Cache frequently used columns of primary tables, as looping through the full tables unfiltered is very slow
	-- We do it once here and never have to do it again in the same game session
	-- Don't save too much data or we run into memory problem
	-- This is kept mostly private to prevent alteration
	local tTemplates = {
		Eras = {},
		Technologies = {},
		Policies = {},
		Beliefs = { "ShortDescription" },
		Religions = { "IconString" },
		Resolutions = {},
		Victories = {},
		UnitPromotions = { "PortraitIndex", "IconAtlas", "Help" },
		UnitCombatInfos = {},
		Units = { "Class" },
		UnitClasses = { "DefaultUnit" },
		Buildings = { "BuildingClass", "PortraitIndex", "IconAtlas", "CivilizationRequired" },
		BuildingClasses = { "MaxGlobalInstances", "DefaultBuilding" },
		Projects = {},
		Processes = {},
		Specialists = { "PortraitIndex", "IconAtlas", "GreatPeopleTitle" },
		Accomplishments = { "MaxPossibleCompletions" },
		Plots = {},
		Terrains = { "PortraitIndex", "IconAtlas" },
		Features = { "PortraitIndex", "IconAtlas" },
		Resources = { "PortraitIndex", "IconAtlas", "IconString", "CivilizationType" },
		Improvements = { "PortraitIndex", "IconAtlas" },
		Corporations = { "PortraitIndex", "IconAtlas", "Help" },
		Civilizations = { "Adjective" },
		Missions = { "IconIndex", "IconAtlas" },
	};
	local tGameInfoCache = {};
	for strTableName in pairs(tTemplates) do
		tGameInfoCache[strTableName] = {};
	end
	for strTableName, tGameInfos in pairs(tGameInfoCache) do
		for kInfo in GameInfo[strTableName]() do
			local kInfoCopy = {
				ID = kInfo.ID,
				Type = kInfo.Type,
				Description = kInfo.Description,
			};
			for _, strColumn in pairs(tTemplates[strTableName]) do
				kInfoCopy[strColumn] = kInfo[strColumn];
			end
			table.insert(tGameInfos, kInfoCopy);
		end
	end

	tGameInfoCache.Yields = tYieldInfos;

	local tTraitCivsCache = {};

	--- Get the list of civilization IDs that have a given trait
	--- @param strTraitType string
	--- @return integer[]
	local function GetCivsFromTrait(strTraitType)
		if not tTraitCivsCache[strTraitType] then
			for row in GameInfo.Leader_Traits { TraitType = strTraitType } do
				for row2 in GameInfo.Civilization_Leaders { LeaderheadType = row.LeaderType } do
					tTraitCivsCache[strTraitType] = tTraitCivsCache[strTraitType] or {};
					table.insert(tTraitCivsCache[strTraitType], GameInfoTypes[row2.CivilizationType]);
				end
			end
		end
		return tTraitCivsCache[strTraitType];
	end

	-- Index the GreatPersonType/SpecialistType relationship
	local tSpecialistGreatPersons = {};
	for kGreatPersonInfo in GameInfo.GreatPersons() do
		if kGreatPersonInfo.Specialist then
			tSpecialistGreatPersons[kGreatPersonInfo.Specialist] = kGreatPersonInfo;
		end
	end

	--- Get the great person database entry corresponding to the specified specialist type
	--- @param strSpecialistType string
	--- @return table
	local function GetGreatPersonInfoFromSpecialist(strSpecialistType)
		return tSpecialistGreatPersons[strSpecialistType];
	end

	--- Get the cached partial database entry of the specified table and ID.<br>
	--- Will raise an error if `strTable` doesn't exist in cache.
	--- Warning: do NOT alter any values inside the returned table!
	--- @param strTable string
	--- @param iId integer
	--- @return Info
	local function GetInfoFromId(strTable, iId)
		if not tGameInfoCache[strTable] then
			error("The specified table is not cached");
		end
		return tGameInfoCache[strTable][iId + 1];
	end

	--- Get the cached partial database entry of the specified table and Type.<br>
	--- Will raise an error if `strTable` doesn't exist in cache.
	--- Warning: do NOT alter any values inside the returned table!
	--- @param strTable string
	--- @param strType string
	--- @return Info
	local function GetInfoFromType(strTable, strType)
		return GameInfoTypes[strType] and GetInfoFromId(strTable, GameInfoTypes[strType]);
	end

	--- Get an iterator of the specified partial database table, starting from ID **0**.<br>
	--- Will raise an error if `strTable` doesn't exist in cache.
	--- @param strTable string
	--- @return fun(): integer, Info
	--- @return Info[]
	--- @return integer
	local function GameInfoCache(strTable)
		if not tGameInfoCache[strTable] then
			error("The specified table " .. strTable .. " is not cached");
		end

		local function iter(t, i)
			i = i + 1;
			local v = t[i + 1];
			if v then
				return i, v;
			end
		end

		return iter, tGameInfoCache[strTable], -1;
	end

	--- Cache the ID, Type, and Description columns of `strTable`.<br>
	--- Does nothing if `strTable` is already cached.<br>
	--- Will raise an error if `strTable` doesn't exist or the table doesn't have all of the above columns.
	--- @param strTable string
	local function PopulateGameInfoCache(strTable)
		if tGameInfoCache[strTable] then
			return;
		end

		tGameInfoCache[strTable] = {};
		for kInfo in GameInfo[strTable]() do
			table.insert(tGameInfoCache[strTable], {
				ID = kInfo.ID,
				Type = kInfo.Type,
				Description = kInfo.Description,
			});
		end
	end

	--- Get the number of rows in the `strTable` table (number of yields depends on mod options instead).<br>
	--- Will raise an error if `strTable` doesn't exist in cache.
	--- @param strTable string
	--- @return integer
	local function GetNumInfos(strTable)
		if not tGameInfoCache[strTable] then
			error("The specified table " .. strTable .. " is not cached");
		end
		return #tGameInfoCache[strTable];
	end

	-- Add the local functions to VP object
	local _VP = {};
	_VP.GetNumInfos = GetNumInfos;
	_VP.GetInfoFromId = GetInfoFromId;
	_VP.GetInfoFromType = GetInfoFromType;
	_VP.GameInfoCache = GameInfoCache;
	_VP.PopulateGameInfoCache = PopulateGameInfoCache;
	_VP.GetCivsFromTrait = GetCivsFromTrait;
	_VP.GetGreatPersonInfoFromSpecialist = GetGreatPersonInfoFromSpecialist;

	-- MapModData may not always exist. While it doesn't, we have no choice but to save the VP object under the context global.
	if global then
		global.VP = _VP
	end

	VP = _VP
end)()

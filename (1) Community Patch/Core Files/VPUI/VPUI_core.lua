-- Check both context global and MapModData for VP object
if not VP and not (MapModData and MapModData.VP) then
	-- Need to localize all globals to circumvent their disappearance when entering leader screen
	local GameInfo = GameInfo;
	local table = table;

	local print = print;
	local error = error;
	local table_insert = table.insert;
	local math_floor = math.floor;

	local MOD_BALANCE_CORE_JFD = GameInfo.CustomModOptions("Name = 'BALANCE_CORE_JFD'")().Value == 1;

	print("VPUI - Populating VP core library");

	local iNumYields = Game and Game.GetNumYieldTypes();
	if not iNumYields then
		local iNumJFDYields = GameInfo.Yields.YIELD_JFD_SOVEREIGNTY.ID + 1;
		local iNumCoreYields = GameInfo.Yields.YIELD_CULTURE_LOCAL.ID + 1;
		iNumYields = MOD_BALANCE_CORE_JFD and iNumJFDYields or iNumCoreYields;
	end

	--- Get the number of yield types in this game
	--- @return integer
	local function GetNumYields()
		return iNumYields;
	end

	local tTraitCivsCache = {};

	--- Get the list of civilization IDs that have a given trait
	--- @param strTraitType string
	--- @return integer[]
	local function GetCivsFromTrait(strTraitType)
		if not tTraitCivsCache[strTraitType] then
			for row in GameInfo.Leader_Traits{TraitType = strTraitType} do
				for row2 in GameInfo.Civilization_Leaders{LeaderheadType = row.LeaderType} do
					tTraitCivsCache[strTraitType] = tTraitCivsCache[strTraitType] or {};
					table_insert(tTraitCivsCache[strTraitType], GameInfo.Civilizations[row2.CivilizationType].ID);
				end
			end
		end
		return tTraitCivsCache[strTraitType];
	end

	--- Get an iterator of the specified partial database table, starting from ID **0**.<br>
	--- For the Yields table, this skips the yields blocked by mod options.<br>
	--- This is a lot faster than using the `GameInfo` iterator.
	---
	--- This assumes that the table is a primary table, starts with ID 0, and doesn't contain ID gaps.<br>
	--- Will raise an error if `strTable` doesn't exist.
	--- @param strTable string
	--- @return fun(): integer, Info
	--- @return Info[]
	--- @return integer
	local function GameInfoCache(strTable)
		if not GameInfo[strTable] then
			error("The specified table " .. strTable .. " doesn't exist");
		end

		local function iter(t, i)
			i = i + 1;
			if strTable == "Yields" and i >= iNumYields then
				return;
			end
			local v = t[i];
			if v then
				return i, v;
			end
		end

		return iter, GameInfo[strTable], -1;
	end

	-- Index the GreatPersonType/SpecialistType relationship
	local tSpecialistGreatPersons = {};
	for _, kGreatPersonInfo in GameInfoCache("GreatPersons") do
		if kGreatPersonInfo.Specialist then
			tSpecialistGreatPersons[kGreatPersonInfo.Specialist] = kGreatPersonInfo;
		end
	end

	--- Get the great person database entry corresponding to the specified specialist type
	--- @param strSpecialistType string
	--- @return Info
	local function GetGreatPersonInfoFromSpecialist(strSpecialistType)
		return tSpecialistGreatPersons[strSpecialistType];
	end

	local tIconAtlasCache = {};

	--- IconHookup, but set the control texture to the random civ icon if it fails (instead of retaining the previous texture)
	--- @param iPortraitIndex integer
	--- @param iIconSize integer
	--- @param strAtlas string
	--- @param imageControl Control
	local function IconHookupOrDefault(iPortraitIndex, iIconSize, strAtlas, imageControl)
		if iPortraitIndex < 0 then
			iPortraitIndex = 23;
			strAtlas = "CIV_COLOR_ATLAS";
		end

		tIconAtlasCache[strAtlas] = tIconAtlasCache[strAtlas] or {};
		if not tIconAtlasCache[strAtlas][iIconSize] then
			for row in GameInfo.IconTextureAtlases{Atlas = strAtlas, IconSize = iIconSize} do
				tIconAtlasCache[strAtlas][iIconSize] = {
					FileName = row.Filename,
					Columns = row.IconsPerRow,
					Rows = row.IconsPerColumn,
				};
			end
		end

		local tAtlas = tIconAtlasCache[strAtlas][iIconSize];
		if not tAtlas or iPortraitIndex >= tAtlas.Columns * tAtlas.Rows then
			if strAtlas == "CIV_COLOR_ATLAS" and iPortraitIndex == 23 then
				-- Even the default icon is missing! We give up
				error("Default icon is missing!");
			end
			print("The specified icon doesn't exist; using default icon instead.");
			IconHookupOrDefault(23, iIconSize, "CIV_COLOR_ATLAS", imageControl);
		end

		local iX = iPortraitIndex % tAtlas.Columns;
		local iY = math_floor(iPortraitIndex / tAtlas.Columns);
		imageControl:SetTexture(tAtlas.FileName);
		imageControl:SetTextureOffsetVal(iX * iIconSize, iY * iIconSize);
	end

	--- @class VPUI
	--- @field GameInfoTypes table<string, integer>
	local _VP = setmetatable({}, {
		__index = function (t, key)
			-- Back up for GameInfoTypes if it doesn't exist. Should be run only once.
			if key == "GameInfoTypes" then
				if GameInfoTypes then
					t[key] = GameInfoTypes;
					return GameInfoTypes;
				end

				print("GameInfoTypes doesn't exist in this context. Creating the table...");

				-- This basically replicates what CvDllDatabaseUtility::CacheGameDatabaseData() does
				local GameInfoTypes = {};

				for row in DB.Query("SELECT tbl_name FROM sqlite_master WHERE type = 'table'") do
					local strTable = row.tbl_name;
					local bHasID = false;
					local bHasType = false;
					for row2 in DB.Query("PRAGMA table_info(" .. strTable .. ")") do
						if row2.name == "ID" then
							bHasID = true;
						elseif row2.name == "Type" then
							bHasType = true;
						end
					end
					if bHasID and bHasType then
						for eIndex, kInfo in GameInfoCache(strTable) do
							GameInfoTypes[kInfo.Type] = eIndex;
						end
					end
				end

				t[key] = GameInfoTypes;
				return GameInfoTypes;
			end
		end,
	});

	-- Add the local functions to VP object
	_VP.GetNumYields = GetNumYields;
	_VP.GameInfoCache = GameInfoCache;
	_VP.GetCivsFromTrait = GetCivsFromTrait;
	_VP.GetGreatPersonInfoFromSpecialist = GetGreatPersonInfoFromSpecialist;
	_VP.IconHookupOrDefault = IconHookupOrDefault;

	-- MapModData may not always exist...
	if MapModData then
		MapModData.VP = _VP;
	end

	VP = _VP; -- There is really no good reason to not save VP to the global env of the calling context in any case...

end

-- Include the custom tooltips here
if MapModData and not MapModData.CommonContext and ContextPtr then
	MapModData.CommonContext = ContextPtr:LoadNewContext("VPUI_common");
end

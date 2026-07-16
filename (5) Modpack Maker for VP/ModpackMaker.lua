-- Modpack Maker for VP
-- Adopted from: Multiplayer Mods Workaround by cicero225, which was adopted from MultiPlayer ModsPack Maker by Gedemon
-- Author: azum4roll

--[[
Allow the creation of a VP_MODPACK folder in assets/DLC to copy (and format) activated mods

Usage:
	- Activate all desired mods, launch a new game from the mods menu, and from "ModpackMaker" context in Firetuner call CreateMP()
	- Check ModpackMaker.log for errors after creation
	- Close the game (exit to Windows) at any time (the game should still function normally until then)
	- Launch the game again, start a new game from the main menu, the modpack should be activated
	- Check database.log if the game crashes before the main menu, or if there are errors in the Lua.log

Limitations:
	- Cannot be used along with other VP-dependent modmods, as they require the VP mod to be activated; you must make a new modpack for this
	- Load order is not followed for non-database files, instead they are loaded in alphabetical folder order
	- You must manually delete the VP_MODPACK folder in Steam\steamapps\common\Sid Meier's Civilization V\Assets\DLC if you want to play the vanilla game again
	- A savegame doesn't know if a modpack is used; the game will simply crash if an incompatible one is loaded
	- You must guarantee everyone's using the same modpack in multiplayer, or the game is going to be extremely unstable
--]]

-- Excluded from modpack
local MODPACK_MAKER_ID = "eb8f6ed3-109d-4f2f-a81d-516c8d2f91c1";

local DATABASE_FILE = "CIV5Units.xml";
local TEXT_FILE = "CIV5Units_Mongol.xml";

local HEADER_COMMENT = "This modpack is only compatible with the Community Patch DLL";

local ADDIN_FILES = {
	{
		File = "InGame.lua",
		AddinType = "InGameUIAddin",
	},
	{
		File = "CityView.lua",
		AddinType = "CityViewUIAddin",
	},
	{
		File = "LeaderHeadRoot.lua",
		AddinType = "DiplomacyUIAddin",
	},
	{
		File = "MiniMapPanel.lua",
		AddinType = "MiniMapOverlayAddin",
	},
	{
		File = "MapGenerator.lua",
		AddinType = "PreMapGenScript",
	},
};

local LANGUAGE_LIST = {
	"en_US",
	"DE_DE",
	"ES_ES",
	"FR_FR",
	"IT_IT",
	"JA_JP",
	"KO_KR",
	"PL_PL",
	"RU_RU",
	"ZH_HANT_HK",
};

-- These tables already exist in game before XML files, so we shouldn't create them again
local SKIPPED_SCHEMAS = {
	ArtDefine_LandmarkTypes = true,
	ArtDefine_Landmarks = true,
	ArtDefine_StrategicView = true,
	ArtDefine_UnitInfoMemberInfos = true,
	ArtDefine_UnitInfos = true,
	ArtDefine_UnitMemberCombatWeapons = true,
	ArtDefine_UnitMemberCombats = true,
	ArtDefine_UnitMemberInfos = true,
	Audio_2DSounds = true,
	Audio_3DSounds = true,
	Audio_ScriptTypes = true,
	Audio_SoundLoadTypes = true,
	Audio_SoundScapeElementScripts = true,
	Audio_SoundScapeElements = true,
	Audio_SoundScapes = true,
	Audio_SoundTypes = true,
	Audio_Sounds = true,
	Audio_SpeakerChannels = true,
	Map_Folders = true,
	Map_Sizes = true,
	Maps = true,
};

-- These seem to be inserted by the game engine, so we don't do it ourselves
local SKIPPED_TABLES = {
	ApplicationInfo = true,
	DownloadableContent = true,
	MapScriptOptionPossibleValues = true,
	MapScriptOptions = true,
	MapScriptRequiredDLC = true,
	MapScripts = true,
	ScannedFiles = true,
};

--- Escape a given string to be safely used in XML tags
--- @param str string
--- @return string
local function EscapeXmlTags(str)
	local entities = {
		["&"] = "&amp;",
		["<"] = "&lt;",
		[">"] = "&gt;",
		['"'] = "&quot;",
		["'"] = "&apos;",
	};

	return (str:gsub("[<&>'\"]", entities));
end

--- Escape a given string to be safely used in XML outside of tags
--- @param str string
--- @return string
local function EscapeXml(str)
	local entities = {
		["&"] = "&amp;",
		["<"] = "&lt;",
		[">"] = "&gt;",
	};

	return (str:gsub("[<&>]", entities));
end

--- Copy all activated mods into the modpack Mods folder, and addin entry files into the UI folder.
---
--- Also hardcode lines into the respective entry points to load the contexts/include the addin files,
--- as Modding.GetActivatedModEntryPoints() no longer returns anything in an "unmodded" game.
local function CopyActivatedMods()
	local tActivatedMods = Modding.GetActivatedMods();
	for _, mod in ipairs(tActivatedMods) do
		if mod.ID ~= MODPACK_MAKER_ID then
			local strName = Modding.GetModProperty(mod.ID, mod.Version, "Name");
			print("Copying " .. strName);
			if not Game.CopyModDataToMPMP(strName, mod.ID, tostring(mod.Version)) then
				print("WARNING: Failed to copy mod data for: " .. strName);
				print("Please check ModpackMaker.log for more details.");
			end
		end
	end

	for _, addinInfo in ipairs(ADDIN_FILES) do
		for addin in Modding.GetActivatedModEntryPoints(addinInfo.AddinType) do
			if addin.ModID ~= MODPACK_MAKER_ID then
				local addinFile = Modding.GetEvaluatedFilePath(addin.ModID, addin.Version, addin.File);
				local strFilePath = addinFile.EvaluatedPath;
				local strFileName = Path.GetFileNameWithoutExtension(strFilePath);
				print(string.format("Adding %s to %s...", strFileName, addinInfo.File));
				Game.AddUIAddinToMPMP(addinInfo.File, strFileName);
			end
		end
	end
end

--- Get the structure of a table as an XML string
--- @param strTableName string
--- @return string
local function GetTableStructure(strTableName)
	-- Don't bother escaping the table name, as it needs to be a tag later and will fail anyway if containing invalid characters
	local strTableStructure = string.format([[
	<Table name="%s">
]], strTableName);

	for row in DB.Query(string.format("PRAGMA table_info(%s)", strTableName)) do
		-- Same with column names
		local strColumn = string.format('\t\t<Column name="%s" type="%s"', row.name, EscapeXmlTags(row.type));

		if row.pk > 0 then
			strColumn = strColumn .. ' primarykey="true"';
		end

		if row.name == "ID" and row.pk > 0 then
			strColumn = strColumn .. ' autoincrement="true"';
		end

		if row.name == "Type" and row.pk > 0 then
			strColumn = strColumn .. ' unique="true"';
		end

		if row.notnull > 0 then
			strColumn = strColumn .. ' notnull="true"';
		end

		local strDefaultValue = string.gsub(row.dflt_value and tostring(row.dflt_value) or "", "'", "");
		if row.type == "boolean" then
			if strDefaultValue == "0" then
				strDefaultValue = "false";
			elseif strDefaultValue == "1" then
				strDefaultValue = "true";
			end
		end
		if strDefaultValue ~= "" and strDefaultValue ~= "NULL" then
			strColumn = strColumn .. string.format(' default="%s"', EscapeXmlTags(strDefaultValue));
		end

		strTableStructure = strTableStructure .. strColumn .. "/>\n";
	end

	return strTableStructure .. "\t</Table>\n";
end

--- Write the whole debug database into DATABASE_FILE
local function CopyFullDatabase()
	-- Open file
	Game.WriteMPMP(DATABASE_FILE, string.format([[
<?xml version="1.0" encoding="utf-8"?>
<!-- %s -->
<GameData>]], HEADER_COMMENT), true);

	for row in DB.Query("SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT LIKE 'sqlite_%' ORDER BY name") do
		local strTableName = row.name;

		if SKIPPED_TABLES[strTableName] then
			print("Skipping: " .. strTableName);
		else
			print("Copying: " .. strTableName);

			if not SKIPPED_SCHEMAS[strTableName] then
				Game.WriteMPMP(DATABASE_FILE, GetTableStructure(strTableName), false);
			end

			Game.WriteMPMP(DATABASE_FILE, string.format("\t<%s>\n\t\t<Delete/>\n", strTableName), false);

			local tColumns = {};
			for row2 in DB.Query(string.format("PRAGMA table_info(%s)", strTableName)) do
				table.insert(tColumns, row2.name);
			end

			for row2 in DB.Query(string.format("SELECT * FROM %s", strTableName)) do
				local strRow = "\t\t<Row>\n";
				for _, strColumn in ipairs(tColumns) do
					local strValue = tostring(row2[strColumn]);
					if strValue ~= "" and strValue ~= "nil" then
						strRow = strRow .. string.format("\t\t\t<%s>%s</%s>\n", strColumn, EscapeXml(strValue), strColumn);
					end
				end
				strRow = strRow .. "\t\t</Row>";
				Game.WriteMPMP(DATABASE_FILE, strRow, false);
			end

			local strTemp = string.format("\t</%s>\n", strTableName);
			Game.WriteMPMP(DATABASE_FILE, strTemp, false);
		end
	end

	Game.WriteMPMP(DATABASE_FILE, "</GameData>\n", false);
end

local function CopyTextDatabase()
	-- Open file
	Game.WriteMPMP(TEXT_FILE, string.format([[
<?xml version="1.0" encoding="utf-8"?>
<!-- %s -->
<GameData>]], HEADER_COMMENT), true);

	for _, strLang in ipairs(LANGUAGE_LIST) do
		print("Copying: Language_" .. strLang);
		Game.WriteMPMP(TEXT_FILE, "\t<Language_" .. strLang .. ">", false);

		local tLines = {};
		local bHasData = false;
		for row in DB.Query("SELECT * FROM Language_" .. strLang) do
			bHasData = true;
			table.insert(tLines, string.format('\t\t<Replace Tag="%s">', row.Tag));
			if row.Text then
				table.insert(tLines, '\t\t\t<Text>');
				table.insert(tLines, '\t\t\t\t' .. EscapeXml(row.Text));
				table.insert(tLines, '\t\t\t</Text>');
			end
			if row.Gender then
				table.insert(tLines, '\t\t\t<Gender>');
				table.insert(tLines, '\t\t\t\t' .. EscapeXml(row.Gender));
				table.insert(tLines, '\t\t\t</Gender>');
			end
			if row.Plurality then
				table.insert(tLines, '\t\t\t<Plurality>');
				table.insert(tLines, '\t\t\t\t' .. EscapeXml(row.Plurality));
				table.insert(tLines, '\t\t\t</Plurality>');
			end
			table.insert(tLines, '\t\t</Replace>');

			-- Write to file every 1000 lines to avoid memory issues
			if #tLines >= 1000 then
				Game.WriteMPMP(TEXT_FILE, table.concat(tLines, "\n"), false);
				tLines = {};
			end
		end

		if next(tLines) then
			Game.WriteMPMP(TEXT_FILE, table.concat(tLines, "\n"), false);
		end

		if not bHasData then
			print("WARNING: " .. strLang .. " is not a loaded language database!");
		end

		Game.WriteMPMP(TEXT_FILE, "\t</Language_" .. strLang .. ">", false);
	end

	Game.WriteMPMP(TEXT_FILE, "</GameData>", false);
end

--- Entry point
function CreateMP()
	print("Deleting previous modpack if exists...");
	Game.DeleteMPMP();

	print("Creating new modpack folder...");
	Game.CreateMPMP();

	print("Copying activated mods...");
	CopyActivatedMods();

	print("Copying the database...");
	CopyFullDatabase();

	print("Getting texts...");
	CopyTextDatabase();

	print("Done!");
end

-- Hotkey: Ctrl + Shift + M
ContextPtr:SetInputHandler(function (uiMsg, wParam)
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.M and UI.CtrlKeyDown() and UI.ShiftKeyDown() then
			CreateMP();
			local iActivePlayer = Game.GetActivePlayer();
			local pPlayer = Players[iActivePlayer];
			pPlayer:AddMessage("Modpack generation complete. Verify there are no error in Lua.log.");
			return true;
		end
	end
	return false;
end);

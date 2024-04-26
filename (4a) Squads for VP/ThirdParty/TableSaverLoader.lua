-- TableSaverLoader
-- TableSave() saves a table with arbitrary table nesting structure to SavedGameDB
-- TableLoad() rebuilds the table from SavedGameDB
-- Author: Pazyryk
------------------------------------------------------------------------------------------------------
-- Version 0.10, 10/6/11 initial beta release
------------------------------------------------------------------------------------------------------
-- Some easy rules:
--  1. All keys must be either integers or strings.
--  2. All values must be of type "number", "string", "boolean", or "table" (NOT "function", "userdata" or "thread").
--  3. Remember that table variables really contain pointers to actual tables. The functions here will create new
--     tables as needed and fill them with contents saved from your old tables (at any table nesting depth). Be careful
--     not to point at something you don't want copied (for example, GameInfo).
------------------------------------------------------------------------------------------------------
-- Set up a master save table ("gT" in this example) in your game init:

-- gValues = {}					-- These are all globals of arbitrary complexity. Values and nested tables within these
-- gPlots = {}					-- globals can be created and destroyed as needed during game session.
-- etc...		
								
-- gT = { gValues,			
--		  gPlots  }					


-- Then call TableSave(gT, "MyMod") before game save and at each active player turn end (for autosaves).

-- Then call TableLoad(gT, "MyMod") after game load to rebuild gT exactly as before.

-- Notes:
-- 1. In the above example, 2 DB tables would be created, "MyMod_Data" and "MyMod_Info", which hold the
--    global table data and save/load info (including checksum), respectively.
-- 2. The global gT must exist (i.e., it's a table rather than nil) before calling TableSave or TableLoad.
-- 3. You can "pre-load" gT as I have done above. TableLoad will use nested tables that already exist.
--    Alternatively, you could run TableLoad first, creating all nested tables in gT, and then assign:
--    gValues = gT[2]; gPlots = gT[1]; etc...
-- 4. You can include values as well as tables in gT. For example, if you assign gT.bBoolean = true
--    during game session, then you can recover this value by b = gT.bBoolean after game load.

-- Suggestions for optimization:
-- 1. TableSave will accommodate any table nesting structure. However, if you have an index that
--    will be created and destroyed often (say iUnit) it is optimal to put that index last. In other words,
--    gUnits.Support[iPlayer][iUnit] is better than gUnits[iUnit].Support[iPlayer] when iUnit values will be
--    created and destroyed frequently. The reason is that the former involves creation/deletion of single
--    non-table items, while the latter involves creation/deletion of whole tables.

------------------------------------------------------------------------------------------------------
-- USER SETTINGS
------------------------------------------------------------------------------------------------------

local bChecksumLastByte = true		-- Do checksum of last byte of each table element on save/load. Print
									-- error if different on load.
local bChecksumEveryByte = false	-- As above but every byte.
local bKeepDBChangeInfo = true		-- If true, MyMod_Info will keep a running record of all saves/loads.
local bVerboseDB = true			-- If true, print all statements that change the DB.

------------------------------------------------------------------------------------------------------
-- FUNCTIONS
------------------------------------------------------------------------------------------------------
print ("loaded VarSaverLoader.lua")
local bInited, bCreateTables, bInitDBVars = false, false, false
local dbParent = {}				-- these 4 keep exact replica of DB during game session (id matches DB rows)
local dbKey = {}			
local dbType = {}
local dbText = {}
local dbFoundDontDelete = {}	--id matches DB rows (so may have gaps)
local insertBuffer = {parent={}; key={}; varType={}; varText={}}	--buffers
local updateTextIDBuffer = {}
local updateTypeTextIDBuffer = {}
local deleteIDBuffer = {}
local varBuffer = {top=0; bottom=0; var={}; varText={}}
local builtTables = {name={}; pointer={}}		-- used and recycled by TableSave to prevent redundant table filling for "loopy" nesting (i.e., multiple paths to the same table)
local checksum = (bChecksumLastByte or bChecksumEveryByte) and 0			--checksum (false if not used)
local inserts, deletes, updates, unchangeds = 0, 0, 0, 0				--counters
local precedingSaveTime = bKeepDBChangeInfo and 0						--timers
local loadTime = bKeepDBChangeInfo and 0
local maxInsertsPerTransaction = 300			-- observed query error somewhere between 500 and 600
local maxUpdatesPerTransaction = 600			-- successful up to 1600 (but these tests on 3 col table)
local DBTableData = ""		--just a holder so these don't need to be passed around between functions
local DBTableInfo = ""
local idStrTable = {}	-- these 3 for string construction (reused/overwritten without cleanup)
local typeStrTable = {}
local textStrTable = {}

--call TableSave() before game save and TableLoad() after game load
function TableSave(globalTable, DBTablePrefix)
	local timer = bKeepDBChangeInfo and os.clock()
	local TableBuilder = TableBuilder
	local DBChange = DBChange
	local DBQuery = Modding.OpenSaveData().Query
	DBTableData = DBTablePrefix.."_Data"	--terminal error of no prefix supplied (good)
	DBTableInfo = DBTablePrefix.."_Info"
	if not bInited then			-- this is the first VarSave after game start or game load
		bCreateTables = true
		for row in DBQuery("SELECT name FROM sqlite_master WHERE name='"..DBTableData.."'") do
			if row.name == DBTableData then bCreateTables = false end
		end
		if bCreateTables then		--create table and table_SaveLoadInfo
			print("Creating SavedGameDB tables for new game: "..DBTableData.." and "..DBTableInfo)
			DBChange("CREATE TABLE ["..DBTableData.."] ('ID' INTEGER PRIMARY KEY, 'parent' TEXT NOT NULL, 'key' TEXT NOT NULL, 'varType' INTEGER NOT NULL, 'varText' TEXT NOT NULL)")
			DBChange("CREATE TABLE ["..DBTableInfo.."] ('ID' INTEGER PRIMARY KEY, 'turn' TEXT, 'bCreateTables' TEXT, 'bInitDBVars' TEXT, 'items' TEXT, 'inserts' TEXT, 'deletes' TEXT, 'updates' TEXT, 'unchangeds' TEXT, 'checksum' TEXT, 'precedingSaveTime' TEXT, 'loadTurn' TEXT, 'loadTime' TEXT, 'loadErrors' TEXT)")
		else	--load up the dbVars
			print ("Getting DB info for TableSave (this is the first save after a game load)...")
			bInitDBVars = true
			for row in DBQuery("SELECT * FROM ["..DBTableData.."]") do
				local id = row.ID
				dbParent[id] = row.parent
				dbKey[id] = row.key
				dbType[id] = row.varType
				dbText[id] = row.varText
				dbFoundDontDelete[id] = false
			end
		end
	end
	TableBuilder(globalTable, "_")
	-- use varBuffer to build tables in an orderly fasion from parent to child (not necessary, but runs faster on load)
	while varBuffer.bottom < varBuffer.top do
		varBuffer.bottom = varBuffer.bottom + 1
		TableBuilder(varBuffer.var[varBuffer.bottom], varBuffer.varText[varBuffer.bottom])	--varText holds the name of the child table
		varBuffer.var[varBuffer.bottom], varBuffer.varText[varBuffer.bottom] = nil, nil
	end			
	varBuffer.top, varBuffer.bottom = 0, 0	--ready for re-use (varBuffer empty now)
	--fill deleteIDBuffer with IDs not seen in this save
	for id, bKeep in pairs(dbFoundDontDelete) do
		if bKeep then
			dbFoundDontDelete[id] = false	--reset for next cycle
		else
			deleteIDBuffer[#deleteIDBuffer + 1] = id
			deletes = deletes + 1
		end
	end
	--do deletes/updates/inserts
	if #deleteIDBuffer > 0 then DoDBDeletes() end
	if #updateTypeTextIDBuffer > 0 then DoDBTypeTextUpdates() end
	if #updateTextIDBuffer > 0 then DoDBTextUpdates() end
	if #insertBuffer.parent > 0 then DoDBInserts() end
	if bKeepDBChangeInfo then
		local items = inserts + updates + unchangeds
		DBChange("INSERT INTO ["..DBTableInfo.."] (turn, bCreateTables, bInitDBVars, items, inserts, deletes, updates, unchangeds, checksum, precedingSaveTime) VALUES ('"..Game.GetGameTurn().."','"..tostring(bCreateTables).."','"..tostring(bInitDBVars).."','"..items.."','"..inserts.."','"..deletes.."','"..updates.."','"..unchangeds.."','"..(checksum or "na").."','"..precedingSaveTime.."')")
	elseif bChecksumLastByte or bChecksumEveryByte then
		DBChange("REPLACE INTO ["..DBTableInfo.."] (ID, checksum) VALUES (1,"..checksum..")")
	end
	-- housekeeping to prepare for next call
	bInited, bCreateTables, bInitDBVars = true, false, false
	for i= #builtTables.name, 1, -1 do
		builtTables.name[i] = nil
		builtTables.pointer[i] = nil
	end
	precedingSaveTime = precedingSaveTime and os.clock() - timer		--saved over until next function call
	print("TableSave time: "..precedingSaveTime..", checksum: "..checksum..", inserts: "..inserts..", updates: "..updates..", unchanged: "..unchangeds..", deletes: "..deletes)
	inserts, deletes, updates, unchangeds  = 0, 0, 0, 0, 0
	checksum = checksum and 0
end

--don't call this directly
function TableBuilder(parentTable, parentName)
	--print("called TableBuilder("..tostring(parentTable)..", "..parentName..")")
	local byte = string.byte
	for originalKey, var in pairs(parentTable) do
		local key = ""
		if type(originalKey) == "number" and math.floor(originalKey) == originalKey then
			key = ","..tostring(originalKey)
		elseif type(originalKey) == "string" then
			key = "."..originalKey
		else
			print ("!!!! TableSaverLoader can only process keys that are integers or strings; bad key:"..originalKey)
		end 
		local varTypeStr = type(var)
		local varType = 0
		if varTypeStr == "number" then
			varText = tostring(var)
		elseif varTypeStr == "boolean" then
			varType = 1
			varText = (var and "1") or "0"
		elseif varTypeStr == "string" then
			varType = 2
			varText = var
		elseif varTypeStr == "table" then
			varType = 3
			--is var an already existing table? If so, we should use that name rather than generate a new name.
			local bExists = false
			for i=1, #builtTables.name do
				if var == builtTables.pointer[i] then
					bExists = true
					varText = builtTables.name[i]
					break
				end
			end
			if not bExists then
				varText = parentName .. key
				local i = #builtTables.name + 1
				builtTables.name[i] = varText		-- saved here so that subsequent references to this table will use same name (regardles of how nested)
				builtTables.pointer[i] = var
				varBuffer.top = varBuffer.top + 1	-- hold var and varText in buffer for subsequent calls to TableBuilder (1st in 1st out so parents are before children)
				varBuffer.var[varBuffer.top] = var				
				varBuffer.varText[varBuffer.top] = varText
			end
		else
			print ("!!!! TableSaverLoader doesn't know type "..varTypeStr)
		end
		--checksum bytes as written
		if bChecksumEveryByte then
			for i = 1, #varText do
				checksum = checksum + byte(varText, i)
			end
		elseif bChecksumLastByte then
			checksum = checksum + byte(varText, -1)
		end
		--have all values for this line; now decide what to do
		local bFound = false
		for id, parent in pairs(dbParent) do
			if parent == parentName and dbKey[id] == key then
				bFound = true
				dbFoundDontDelete[id] = true
				if dbType[id] == varType then
					if dbText[id] == varText then
						unchangeds = unchangeds + 1
					else
						local i = #updateTextIDBuffer + 1
						updateTextIDBuffer[i] = id
						dbText[id] = varText
						if i > maxUpdatesPerTransaction then DoDBTextUpdates() end
						updates = updates + 1
					end
				else
					local i = #updateTypeTextIDBuffer + 1
					updateTypeTextIDBuffer[i] = id
					dbType[id] = varType
					dbText[id] = varText
					if i > maxUpdatesPerTransaction then DoDBTypeTextUpdates() end
					updates = updates + 1
				end
				break
			end
		end
		if not bFound then
			--insertBuffer = {ID={}; parent={}; key={}; varType={}; varText={}}
			local i = #insertBuffer.parent + 1
			insertBuffer.parent[i] = parentName
			insertBuffer.key[i] = key
			insertBuffer.varType[i] = varType
			insertBuffer.varText[i] = varText
			if i > maxInsertsPerTransaction then DoDBInserts() end
			inserts = inserts + 1
		end
	end
end

-- call at game init after game load
-- each call will overwrite keyed gT values (except for nested tables, which will be created anew only if they don't already exist)
function TableLoad(globalTable, DBTablePrefix)
	local timer = bKeepDBChangeInfo and os.clock()
	local DBChange = DBChange
	local DBQuery = Modding.OpenSaveData().Query
	DBTableData = DBTablePrefix.."_Data"	--terminal error of no prefix supplied (good)
	DBTableInfo = DBTablePrefix.."_Info"
	local loadBuffer = {parent={}; key={}; varType={}; varText={}}	-- no gain in recycling/reuse because function normally called once per game session
	local parentPointer = {}			
	parentPointer._ = globalTable	-- the highest level parent (must already exist)
	local function AddTableValues(parent, key, varType, varText)
		local byte = string.byte
		if varType == 0 then		--number
			parentPointer[parent][key] = tonumber(varText)
		elseif varType == 1 then	--boolean
			parentPointer[parent][key] = (varText == "1" and true) or false
		elseif varType == 2 then	--string
			parentPointer[parent][key] = varText
		elseif varType == 3 then	--table
			if parentPointer[varText] then		-- table already exists because another element had this same table
				parentPointer[parent][key] = parentPointer[varText]
			elseif parentPointer[parent][key] then -- this table may exist because it was "preloaded" in gT by the mod before TableLoad()
				parentPointer[varText] = parentPointer[parent][key]
			else	-- table does not exist, make it
				parentPointer[parent][key] =  {}	--assign a new table to this element
				parentPointer[varText] = parentPointer[parent][key]	--the magic step that makes the element (child) a new parent
			end						
		end
		--checksum bytes as read
		if bChecksumEveryByte then
			for i = 1, #varText do
				checksum = checksum + byte(varText, i)
			end
		elseif bChecksumLastByte then
			checksum = checksum + byte(varText, -1)
		end
	end
	for row in DBQuery("SELECT * FROM ["..DBTableData.."]") do
		--build the key
		local key = ""
		if row.key:sub(1,1) == "," then
			key = tonumber(row.key:sub(2))
		else
			key = row.key:sub(2)
		end
		if parentPointer[row.parent] then	-- we have a table pointer! Add values
			AddTableValues(row.parent, key, row.varType, row.varText)
		else		-- order must be bollixed with child before parent; hold DB row values and try again later
			local i = #loadBuffer.parent + 1
			loadBuffer.parent[i] = row.parent
			loadBuffer.key[i] = key
			loadBuffer.varType[i] = row.varType
			loadBuffer.varText[i] = row.varText
		end
	end
	-- do unprocessed until none left (or throw error if row can never be linked to parent)
	local endlessLoopCounter = 0
	local i = next(loadBuffer.parent)
	while i do
		if parentPointer[loadBuffer.parent[i]] then
			AddTableValues(loadBuffer.parent[i], loadBuffer.key[i], loadBuffer.varType[i], loadBuffer.varText[i])
			loadBuffer.parent[i], loadBuffer.key[i], loadBuffer.varType[i], loadBuffer.varText[i] = nil, nil, nil, nil
		end
		local i = next(loadBuffer.parent, i) or next(loadBuffer.parent)
		endlessLoopCounter = endlessLoopCounter + 1
		if endlessLoopCounter > 100000 then
			print (DBTableData.." is corrupt: could not find parents for "..#loadBuffer.parent.." rows (e.g., "..(loadBuffer.parent[i] or "nil")..")")
			break
		end
	end
	-- test checksum and write load info
	local lastInfoRow = 1
	if bKeepDBChangeInfo then
		for row in DBQuery("SELECT MAX(ID) FROM ["..DBTableInfo.."]") do
			lastInfoRow = row["MAX(ID)"]		--get current Info row ID
		end
	end
	local error = "none"
	if bChecksumLastByte or bChecksumEveryByte then
		local checksumOld = 0
		for row in DBQuery("SELECT * FROM ["..DBTableInfo.."] WHERE ID="..lastInfoRow) do
			checksumOld = tonumber(row.checksum)
		end
		if checksum == checksumOld then
			print ("TableLoad ran without error; checksum: "..checksum)
		else
			error = "!!!! Error loading tables: checksumOld="..checksumOld..", checksumNew="..checksum.." !!!!"
			print (error)
		end
	end
	if bKeepDBChangeInfo then		-- write load info
		loadTime = os.clock() - timer
		DBChange("UPDATE ["..DBTableInfo.."] SET loadTurn='"..Game.GetGameTurn().."', loadTime='"..loadTime.."', loadErrors='"..error.."' WHERE ID="..lastInfoRow)
	end
	checksum = checksum and 0
end

function DoDBDeletes()
	local DBChange = DBChange
	DBChange("delete from ["..DBTableData.."] where ID in ("..table.concat(deleteIDBuffer,",")..")")
	-- make dbVars look like DB and clear out deleteIDBuffer for re-use
	for i = 1, #deleteIDBuffer do
		local id = deleteIDBuffer[i]
		dbParent[id] = nil
		dbKey[id] = nil	
		dbType[id] = nil
		dbText[id] = nil
		dbFoundDontDelete[id] = nil
		deleteIDBuffer[i] = nil
	end
end
	
function DoDBInserts()
	local DBChange = DBChange
	local items = #insertBuffer.parent
	for i = 1, items do
		local parent = insertBuffer.parent[i]
		local key = insertBuffer.key[i]
		local varType = insertBuffer.varType[i]
		local varText = insertBuffer.varText[i]
		local id = #dbParent + 1	-- may insert into a gap if there is one
		textStrTable[i] = id..",'"..parent.."','"..key.."',"..varType..",'"..varText.."'"	-- no '' for id or varType (integers)
		dbParent[id] = parent
		dbKey[id] = key
		dbType[id] = varType
		dbText[id] = varText
		insertBuffer.parent[i] = nil
		insertBuffer.key[i] = nil
		insertBuffer.varType[i] = nil
		insertBuffer.varText[i] = nil
	end
	DBChange("insert into ["..DBTableData.."] (ID,parent,key,varType,varText) select "..table.concat(textStrTable, " union all select ", 1, items))
end


function DoDBTextUpdates()
	local DBChange = DBChange
	local items = #updateTextIDBuffer
	for i = 1, items do
		local id = updateTextIDBuffer[i]
		local idStr = tostring(id)
		idStrTable[i] = idStr
		textStrTable[i] = "when ID="..idStr.." then '"..dbText[id].."'"
		updateTextIDBuffer[i] = nil
	end
	DBChange("update ["..DBTableData.."] set varText=case "..table.concat(textStrTable, " ",1,items).." end where ID in ("..table.concat(idStrTable, ",",1,items)..")")
end

function DoDBTypeTextUpdates()
	local DBChange = DBChange
	local items = #updateTypeTextIDBuffer
	for i = 1, items do
		local id = updateTypeTextIDBuffer[i]
		local idStr = tostring(id)
		idStrTable[i] = idStr
		typeStrTable[i] = "when ID="..idStr.." then "..dbType[id]
		textStrTable[i] = "when ID="..idStr.." then '"..dbText[id].."'"
		updateTypeTextIDBuffer[i] = nil
	end
	DBChange("update ["..DBTableData.."] set varType=case "..table.concat(typeStrTable, " ",1,items).." end, varText=case "..table.concat(textStrTable, " ",1,items).." end where ID in ("..table.concat(idStrTable, ",",1,items)..")")
end

function DBChange(str)
	--for single DB commands when you don't want a row iteration value
	local DBQuery = Modding.OpenSaveData().Query
	local sub = string.sub
	if bVerboseDB then
		local i = 1
		while i < #str do
			print(sub(str,i,i+2999))
			i = i + 3000
		end
	end
	for _ in DBQuery(str) do end
end


------------------------------------------------------------------------------------------------------------------
-- Testing / debugging
------------------------------------------------------------------------------------------------------------------

function DebugTime(seconds)
	--test whether civ5 crashes if hijacked by Lua
	local startTime = os.clock()
	while os.clock() < startTime + seconds do end
	print("I'm back..")
end

local debugRows = 0
function DebugDBInsert(lines, bAppend)
	local DBChange = DBChange
	local DBQuery = Modding.OpenSaveData().Query
	if bAppend then
		DBChange("CREATE TABLE IF NOT EXISTS DebugTestDB ('ID' INTEGER PRIMARY KEY, 'int' INTEGER, 'col' TEXT)")
	else
		DBChange("DROP TABLE IF EXISTS DebugTestDB")
		DBChange("CREATE TABLE DebugTestDB ('ID' INTEGER PRIMARY KEY, 'int' INTEGER, 'col' TEXT)")
		debugRows = 0
	end
	local startRow = debugRows + 1
	local stopRow = debugRows + lines
	local timer = os.clock()
	for i = startRow, stopRow do
		DBChange("INSERT INTO DebugTestDB (ID, int, col) VALUES ("..i..","..i..",'teststring')")
		debugRows = debugRows + 1
	end
	DBTime = os.clock() - timer
	local errors = 0
	for i = startRow, stopRow do
		for row in DBQuery("SELECT * FROM DebugTestDB WHERE ID="..i) do
			if row.int ~= i or row.col ~= "teststring" then
				errors = errors + 1
			end
		end
	end
	print ("Lines: "..lines.."; Errors: "..errors.."; DBTime: "..DBTime)
end

function DebugDBBigInsert(lines, bAppend)
	local DBChange = DBChange
	local DBQuery = Modding.OpenSaveData().Query
	if bAppend then
		DBChange("CREATE TABLE IF NOT EXISTS DebugTestDB ('ID' INTEGER PRIMARY KEY, 'int' INTEGER, 'col' TEXT)")
	else
		DBChange("DROP TABLE IF EXISTS DebugTestDB")
		DBChange("CREATE TABLE DebugTestDB ('ID' INTEGER PRIMARY KEY, 'int' INTEGER, 'col' TEXT)")
		debugRows = 0
	end
	local startRow = debugRows + 1
	local stopRow = debugRows + lines
	local strTable = {}
	local timer = os.clock()
	for i = startRow, stopRow do
		local iStr = tostring(i)
		strTable[#strTable + 1] = iStr..","..iStr..",'teststring'"
		debugRows = debugRows + 1
	end
	local changeStr = "insert into DebugTestDB (ID,int,col) select "..table.concat(strTable, " union all select ")
	DBChange(changeStr)
	DBTime = os.clock() - timer
	local errors = 0
	for i = startRow, stopRow do
		for row in DBQuery("SELECT * FROM DebugTestDB WHERE ID="..i) do
			if row.int ~= i or row.col ~= "teststring" then
				errors = errors + 1
			end
		end
	end
	print ("Lines: "..lines.."; Errors: "..errors.."; DBTime: "..DBTime)
end

--Structrue for big updates
--[[
update DebugTestDB set
int=case when ID=1 then 10 when ID=2 then 20 end,
col=case when ID=1 then 'teststr1' when ID=2 then 'teststr2' end
where ID in (1,2)
]]

function DebugDBBigUpdate(startRow, stopRow)
	local timer = os.clock()
	local DBChange = DBChange
	local DBQuery = Modding.OpenSaveData().Query
	local startRow = startRow or 1
	local stopRow = stopRow or 100
	local intStrTable = {}
	local colStrTable = {}
	local idStrTable = {}
	for i = startRow, stopRow do
		local iStr = tostring(i)
		intStrTable[#intStrTable + 1] = "when ID="..iStr.." then "..iStr
		colStrTable[#colStrTable + 1] = "when ID="..iStr.." then 'teststr2'"
		idStrTable[#idStrTable + 1] = iStr
	end
	local changeStr = "update DebugTestDB set int=case "..table.concat(intStrTable, " ").." end, col=case "..table.concat(colStrTable, " ").." end where ID in ("..table.concat(idStrTable, ",")..")"
	DBChange(changeStr)
	DBTime = os.clock() - timer
	local errors = 0
	for i = startRow, stopRow do
		for row in DBQuery("SELECT * FROM DebugTestDB WHERE ID="..i) do
			if row.int ~= i or row.col ~= "teststr2" then
				errors = errors + 1
			end
		end
	end
	print ("Lines: "..(stopRow-startRow+1).."; Errors: "..errors.."; DBTime: "..DBTime)
end

--This function will fill gTiles from the running game
--gPlots[x][y][sub] = value		sub= iPlot, TileType, etc. 
function DebugFillgPlots()
	for iPlot = 0, Map.GetNumPlots() - 1 do
		local plot = Map.GetPlotByIndex(iPlot)
		local x = plot:GetX()
		local y = plot:GetY()
		local plotType = plot:GetPlotType()
		local featureType = plot:GetFeatureType()
		local improvementType = plot:GetImprovementType()
		gPlots[x] = gPlots[x] or {}
		gPlots[x][y] = gPlots[x][y] or {}
		gPlots[x][y].plotType = plotType
		gPlots[x][y].featureType = featureType
		gPlots[x][y].improvementType = improvementType
	end
	print("found "..Map.GetNumPlots().." plots")
end

--test gPlots for consistancy after TableLoad()
function DebugTestgPlots()
	local tests, errors = 0, 0
	for x, gPlotX in pairs(gPlots) do
		for y, gPlotXY in pairs(gPlotX) do
			local plot = Map.GetPlot(x, y)
			--?? how to get iPlot ??
			if gPlotXY.plotType ~= plot:GetPlotType() then errors = errors + 1 end
			if gPlotXY.featureType ~= plot:GetFeatureType() then errors = errors + 1 end
			if gPlotXY.improvementType ~= plot:GetImprovementType() then errors = errors + 1 end
			tests = tests + 3
		end
	end
	print("tests: "..tests.."; errors: "..errors)
end

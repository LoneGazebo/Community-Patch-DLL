--[[

	This is a fully compatible replacement for the atrocious GameInfo object provided by fireaxis' civ5 game engine...
	Created for CP/VP but works with stock Civ5 as well...
	
	For the 5.03 alpha version of VP with EUI replacing the stock GameInfo object with this, reduces lua's memory by about 25Mb from 50Mb total after loading a safegame (LuaJIT)
	Needs about 1.7mB of memory to store everythink requested from the databse when loading said safegame...
	
	Main design concepts: 
		* Use arrays to store data of columns that are filled with mostly non defaults (PRIO_NAMES), because they are more memory efficient and faster
		* For other columns only store for each row the non default values
		* 'Rows' given by any functions here don't hold data, they just act as a kind of pointer to it, and retrieve it when indexed

	Common names for variables in this code:
	
	idx - index of a row in the storage system setup by this code
	prop/property - name of column
	root - a lua table object that holds all data of a single database table
	row - table that acts as a pointer to the data of a row
	filter - an array holding all inidices(idx) for rows that statisfy an iteration conditional
--]]


local CONST_NIL = {NIL=true} ;

--local DB_PRESET_PRIO_NAMES = { "ID" , "Type", "Description", "PortraitIndex", "IconAtlas", "Civilopedia", "Help", "PediaType", "PediaEntry", "FlagPromoOrder", "Name", "Value" }

local DB_PRESET_PRIO_NAMES = { "rowid"; "Adjective"; "Advisor"; "AdvisorQuestion"; "AlphaIconAtlas"; "ArtDefineTag"; "ArtStylePrefix"; "ArtStyleSuffix"; "ArtStyleType"; 
	"Asset"; "Atlas"; "AudioIntro"; "AudioIntroHeader"; "BeliefType"; "Blue"; "Boldness"; "BuildingClass"; "BuildingClassType"; "BuildType"; 
	"Civilopedia"; "CivilopediaHeaderType"; "CivilopediaTag"; "Class"; "CombatClass"; "CursorType"; "DawnOfManAudio"; "DawnOfManImage"; "DawnOfManQuote"; 
	"DefaultBuilding"; "DefaultPlayerColor"; "DefaultUnit"; "DefaultUnitAI"; "DenounceWillingness"; "Description"; "DiploBalance"; "DoFWillingness"; "Domain"; 
	"Era"; "FeatureType"; "Filename"; "FileName"; "FlagPromoOrder"; "Forgiveness"; "Green"; "Help"; "IconAtlas"; "IconAtlasAchieved"; "IconIndex"; "IconSize"; 
	"IconsPerColumn"; "IconsPerRow"; "IconString"; "ID"; "ImprovementType"; "InfluenceCost"; "LeaderheadType"; "LeaderType"; "Loyalty"; "MapImage"; "Meanness"; 
	"MinorCivCompetitiveness"; "MinorCivTrait"; "Movement"; "MoveRate"; "Moves"; "Name"; "Neediness"; "OrderPriority"; "PediaEntry"; "PediaType"; "PillageGold"; 
	"PortraitIndex"; "PrereqTech"; "PrimaryColor"; "PrimaryVictoryPursuit"; "ProposerDecision"; "ResearchPercent"; "ResourceClassType"; "SecondaryColor"; 
	"ShortDescription"; "Sound"; "StrategicViewType"; "Summary"; "TechImproveable"; "TechType"; "TerrainType"; "TextColor"; "TileType"; "Tooltip"; "Topic"; 
	"TraitType"; "Type"; "UnitArtInfo"; "UnitFlagAtlas"; "UnitFlagIconOffset"; "Value"; "VictoryCompetitiveness"; "WarmongerHate"; "WonderCompetitiveness"; 
	"WorkAgainstWillingness"; "WorkWithWillingness"; "Yield"; "YieldType" }

local assert = assert ;
local collectgarbage = collectgarbage ;
local type = type ;
local setmetatable = setmetatable ;
local tonumber = tonumber ;
local tostring = tostring ;
local ipairs = ipairs ;
local pairs = pairs ;
local error = error ;
local gprint = print ;
local print = function(...) return gprint("[GAMEINFO]",...) end
local origDB = DB ;

local function Q( what )
	local ret = {} ;
	for row in origDB.Query(what) do
		ret[#ret+1] = row
	end
	return ret ;
end

local function Q2( what, colname )
	local ret = {} ;
	for row in origDB.Query(what) do
		ret[#ret+1] = row[colname]
	end
	return ret ;
end

local function findIndex( what, where, offset )
	return ( (not offset) and findIndex(what, where, 1 ) ) or ( (where[offset] == what) and offset ) or ( ( offset >= (#where) ) and -1  ) or findIndex( what, where, offset+1 ) ;
end

-- Supposed to transform the information returned by PRAGMA about a default value and its type to the correct lua value...
-- what and into are never parsed by the DB so are always of type string (or nil???) 
-- This function is a mess. But so is the data returned by PRAGMA, which in turn can only return what's in the DB, and the default values there lack any standardization...
local function valuetransform( what, into ) 
	--print("VT",what,type(what),into,type(into))
	--if not what then return CONST_NIL end
	--if not into then return what end
	
	if what == nil then return CONST_NIL end;
	
	if what == "NULL" then return CONST_NIL end
	if what == "'-1'" then return -1 end
	
	local ret ;
	if into == "boolean" then
		ret = ( what == "true" or what == "1" ) ;
	else
		ret = ( tonumber(what) or what ) ;
	end
	return ( ( ret == nil ) and CONST_NIL ) or ret ;
end

-- Main lookup function!
-- root is the table, idx the index in our array/indivdata table and prop the name of the column
local function getProperty( root, idx, prop )
	local prionames = root.__PRIONAMES ;
	local prioidx = findIndex(prop,prionames) ;
	if prioidx > 0 then
		return root.__ARRAYDATA[ prioidx + root.__PRIOCOUNT * (idx-1) ] ;
	else
		local indivd  = root.__INDIVDATA[idx]  ;
		local val = nil ;
		
		if indivd then
			val = indivd[prop] ;
		end
		
		if val == nil then
			val = root.__DEFVALUES[prop] ;
		end
		
		if val == CONST_NIL then
			return nil
		elseif val ~= nil then
			return val ;
		else
			if root.__NOTACOLWARN == false then
				print( "'" .. tostring(prop) .. "' is not a column in table '" .. root.__TABLENAME .. "'! Returning 'nil' to caller!" ) ;
				root.__NOTACOLWARN = tostring(pro) ;
			end
			return nil ;
		end
	end
end

-- The metatable for a row returned by <ROOTTABLE>[key] or iterators <ROOTTABLE>()
local MT_ROW_INSTANCE = {
	__index = function( rowinstance, prop ) return getProperty(rowinstance.__ROOT, rowinstance.__IDX, prop ) end ;
}

local function getRowFromIdxOrNil( root, idx )
	if idx and idx > 0 and idx <= root.__COUNT then 
		return setmetatable({
			__IDX = idx ;
			__ROOT = root ;},MT_ROW_INSTANCE); 
	end 
end

-- Some mods do 'for row in Game and GameInfo.Buildings() or DB.Query("SELECT * FROM Buildings") do {BLOCK} end' which is not exactly how lua's 'for' iterators are supposed to work. This means we need a closure...
-- See: https://lua.org/manual/5.1/manual.html#2.4.5 
-- TBF: Actually, when it comes to speed, a closure is propably the fastest way to implement iteration...
-- in theory a static instance for the two different closures, as well as for the row would be crazy efficient, but would also require additional code to not break nested iterators, so not worth it... 
local function createIteratorClosure( root, filter )
	local iteratoridx = 0 ;
	return (filter and (function()
		iteratoridx = iteratoridx + 1 ;
		return getRowFromIdxOrNil( root, filter[iteratoridx] or -1 ) ;
	end)) or (function() iteratoridx = iteratoridx + 1 ; return getRowFromIdxOrNil( root, iteratoridx ) end) ;
end

local function createFilterFromTable( root, filterarg )
	local ret = {}
	for idx = 1 , root.__COUNT do
		local pass = true  ;
		for k,v in pairs(filterarg) do
			local prop = getProperty(root, idx, k) ;
			if  prop ~= v and not( prop==true and v==1 ) then -- {PrereqTech = techType, ShowInTechTree = 1} , used in stock civ5. Unhappy :/
				pass = false ;
				break ;
			end
		end
		if pass then ret[#ret+1] = idx end
	end
	return ret ;
end

local function adjustDatabaseFilter( root, filter )
	local reverseTable = root.__DBIDTOIDX ;
	if reverseTable then
		for pos,gameid in ipairs(filter) do
			filter[pos] = reverseTable[gameid]
		end
	end
	return filter ;
end

-- EG: GI.Buildings(); GI.Buildings("Cost>500"); GI.Buildings({Cost = 200})
-- Returns an iterator, that returns a row each time it's called
local function callRootTable( root, filterarg )
	if filterarg == nil then
		return createIteratorClosure( root )
	end
	
	if type(filterarg) == "table" then
		return createIteratorClosure( root, createFilterFromTable( root, filterarg ) );
	end
	
	assert( type(filterarg) == "string" , "Filter has to be a value of type table or string!" );
	
	if root.__IDCOL > 0 then -- Not going to implement a parser for generic SQL string conditions, but if the table has an ID column we can request just those from the DB
		local querystring = "SELECT ID+1 AS luaid FROM " .. root.__TABLENAME .. " WHERE " .. filterarg; --offset by +1 because the data for row with ID=0 sits at position 1 in our array
		return createIteratorClosure( root, adjustDatabaseFilter(root, Q2(querystring,"luaid")) ) ;
	end
	
	-- Vaderkos <3 suggests rowids, but this case is so rare in CP/VP/EUI that I will stick with this not solution, as storing rowids would take quite alot of space:
	-- even when only done for tables that have no id column
	-- Eg. Defines has 2982 entries and two columns in VP. Lua allocates memory for arrays in power of twos so for 5964 entries that still becomas an array of length 8192 (without rowid)
	-- with rowid there is a third field, however 3*2982 = 8946 is now greater than 8192 so now we have an array of length 16384, double that of before
	-- In LuaJIT each allocated field of an array takes 8 bytes, in stock Lua it's double that
	-- That computes to 65kb (LuaJIT) or 131kb (stock) of extra memory consumption just from this one table (which might be the worst offender in that regard, but still)
	-- given the ~1700kb this code needs to store almost the entire DB, and again how rare these requests are anyway, fetching/storing rowids is not worth it (IMHO)...
	local querystring = "SELECT * FROM " .. root.__TABLENAME .. " WHERE " .. filterarg;
	if root.__FILTERARGWARNING == false then
		print( "Can't handle filterarg for table:", root.__TABLENAME ,"Arg:", filterarg , "Redirecting to original database with query:", querystring ) ;
		print( "This warning is only shown once per table!" ) ;
		root.__FILTERARGWARNING = filterarg ;
	end
	return origDB.Query( querystring ) ;

end

-- EG: GI.Buildings[0] or GI.Buildings.BUILDING_COURTHOUSE
local function indexRootTable( root, token )
	if type(token) == "number" then return getRowFromIdxOrNil(root, (root.__IDCOL == -1 and token) or ( (not root.__DBIDTOIDX) and token+1) or root.__DBIDTOIDX[token+1] ) 
	elseif type(token) == "string" then 
		local typecol = root.__TYPECOL ;
		if typecol > 0 then
			local array, step = root.__ARRAYDATA, root.__PRIOCOUNT ;
			for idx = 0 , root.__COUNT - 1 do
				if array[ typecol + idx * step ] == token then
					return getRowFromIdxOrNil( root, idx+1 )
				end
			end
			return nil
		else
			print("Indexing by type not supported for this table!" , root.__TABLENAME, token );
			return nil ;
		end
	else
		error("Can only index by number(ID column if available, otherwise (undefined) cache ordering) or string(if the table has a type column)");
	end
end

local MT_ROOT_INSTANCE = {
	__call = callRootTable ;
	__index = indexRootTable ;
}

local function setProtodata( root, tablename )


	local db_proto = Q( "PRAGMA table_info(" .. tablename .. ")" ) ;
	if #db_proto < 1 then error("No such table! '" .. tostring(tablename) .. "'" ) end;
	
	local db_count = Q( "SELECT COUNT() AS C FROM " .. tablename )[1]["C"] ;
	
	local prionames = {} ;
	local priocount = 0 ;
	local defvalues = {} ;
	
	local idcol = -1 ;
	local typecol = -1 ;
	
	for idx,row in ipairs(db_proto) do
		if findIndex( row.name, DB_PRESET_PRIO_NAMES ) > 0 then
			priocount = priocount + 1 ;
			prionames[priocount] = row.name ;
		else
			defvalues[row.name] = valuetransform(row.dflt_value, row.type) ;
		end
		if row.name == "ID" then
			idcol = idx ;
		elseif row.name == "Type" then
			typecol = idx ;
		end
	end
	root.__PRIONAMES = prionames ;
	root.__PRIOCOUNT = priocount ;
	root.__DEFVALUES = defvalues ;
	root.__COUNT = db_count ;
	root.__IDCOL = idcol ;
	root.__TYPECOL = typecol ;
	
	root.__FILTERARGWARNING = false ;
	root.__NOTACOLWARN = false ;
	root.__DBIDTOIDX = false ;
	
end


local function nullStack( count, ... )
	if count > 1024 then -- Don't explode the stack too much
		return nullStack(1024, ...)
	elseif count > 16 then
		return nullStack( count-16,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  ... ) ;
	elseif count > 1 then
		return nullStack( count-1 , 0, ... ) ;
	else
		return 0, ... ;
	end
end

local function copyDataFromDB( root, tablename )
	
	
	local prionames = root.__PRIONAMES ;
	local priocount = root.__PRIOCOUNT ;
	local defvalues = root.__DEFVALUES ;
	
	local db_data ;
	if( root.__IDCOL > 0 ) then
		db_data = Q( "SELECT * FROM " .. tablename .. " ORDER BY ID" ) ;
	else
		db_data = Q( "SELECT * FROM " .. tablename ) ;
	end
	
	
	local entries = 0 ;
	local arraysize = priocount * root.__COUNT ;
	
	-- This will prevent occasional nil entries from flipping the array into a key-value table...
	local array = {nullStack(arraysize)}
	
	
	if arraysize > 4096 then 
		print( "Got big table:", tablename, arraysize ) ;
	end
	
	local nondefs = {} ;

	for dataidx,datarow in ipairs(db_data) do
		local val ;
		for prioidx, name in ipairs(prionames) do
			array[ prioidx + (dataidx-1) * priocount ] = datarow[name] ;
		end
		for name, defval in pairs(defvalues) do
			val = datarow[name] ;
			if val == nil then val = CONST_NIL end;
			if val ~= defval then
				nondefs[dataidx] = nondefs[dataidx] or {} ;
				nondefs[dataidx][name] = val ;
			end
		end
		entries = entries + 1 ;
	end
	
	root.__ARRAYDATA = array ;
	root.__INDIVDATA = nondefs ;
	
	if entries ~= root.__COUNT then
		print("entries ~= expected count",entries,root.__COUNT);
	end 
	
	if root.__IDCOL ~= -1 then
		local idcol = root.__IDCOL ;
		local controlvar = false -- array[ idcol ] ;
		for idx = 1, entries do
			if array[ idcol + (idx-1) * priocount ] ~= (idx-1) --[[ + controlvar --]] then
				print("ID column found, but they are not 0,1,2 .... n-2, n-1!", tablename);
				controlvar = true ;
				break ;
			end
		end
		if (controlvar == true) --[[ or (controlvar ~= 0) --]] then
			local reverseTable = {nullStack(entries)} ;
			for idx = 1, entries do
				reverseTable[ array[idcol + (idx-1) * priocount] + 1 ] = idx ;   --Offset by +1 too keep it an array in the LuaEngine
			end
			root.__DBIDTOIDX = reverseTable ;
		end
	end
	
end

local function indexGameInfo( GI, tablename )
	if type(tablename) ~= "string" then error("Invalid tablename!") end
	
	local root = { __TABLENAME = tablename } ;
	
	setProtodata(root, tablename);
	copyDataFromDB(root, tablename );
	
	GI[tablename] = root ;
	return setmetatable(root, MT_ROOT_INSTANCE);
end


local MT_GAMEINFO_OBJECT = {
	__call = function(GI,ops)
		if ops == "reset" then
			collectgarbage("collect") ;
			local m = collectgarbage("count") * 1024;
			local tot = 0 ;
			print()
			print("Releasing tables: bytes freed / DB table name" )
			for k in pairs(GI) do
				GI[k] = nil ;
				collectgarbage("collect") ;
				local m2 = collectgarbage("count") * 1024;
				print( m - m2, k );
				tot = tot + m-m2 ;
				m = m2 ;
			end
			print( tot, "Total") ;
			print();
		end
		return GI ;
	end;
	__index = indexGameInfo ;
}

function createGameInfoObject( GI )
	return setmetatable(GI or {}, MT_GAMEINFO_OBJECT ) ;
end

newGameInfo = createGameInfoObject( newGameInfo )
if not oldGameInfo then
	oldGameInfo = GameInfo ;
end
GameInfo = newGameInfo ;


--[[

-- DEBUG STUFF
--


-- All tables that got cached during a game load...

dbgTablenames = { "Specialists"; "Accomplishments"; "MapScripts"; "Routes"; "Features"; "Corporations"; "Missions"; "Builds"; "Technology_TradeRouteDomainExtraRange"; "Civilizations";
	 "Technology_FreePromotions"; "LeagueProjects"; "GreatPersons"; "Tech_SpecialistYieldChanges"; "Terrain_TechYieldChanges"; "Feature_TechYieldChanges"; "PlayerColors";
	 "Concepts"; "Colors"; "HistoricRankings"; "Leaders"; "Civilization_Leaders"; "CustomModOptions"; "Improvements"; "MinorCivTraits_Status"; "Building_TerrainYieldChanges";
	 "Belief_ResourceYieldChanges"; "Eras"; "Buildings"; "GameSpeeds"; "Building_RiverPlotYieldChanges"; "Units"; "BuildFeatures"; "Defines"; "Building_ResourceCultureChanges";
	 "Traits"; "Leader_Traits"; "Improvement_AccomplishmentYieldChanges"; "Build_TechTimeChanges"; "Policy_ImprovementCultureChanges"; "FakeFeatures"; "Commands";
	 "Civilization_BuildingClassOverrides"; "Belief_TerrainYieldChanges"; "Building_ResourceYieldChangesGlobal"; "Building_ResourceYieldChanges"; "InterfaceModes";
	 "Worlds"; "Technology_ORPrereqTechs"; "Technology_PrereqTechs"; "Automates"; "Processes"; "Resources"; "IconTextureAtlases"; "GameOptions"; "Resource_YieldChanges";
	 "Yields"; "Terrains"; "Policy_PrereqPolicies"; "Trait_AlternateResourceTechs"; "PolicyBranchTypes"; "Technologies"; "BuildingClasses"; "Policies"; "Route_TechMovementChanges";
	 "Religions"; "Resolutions"; "Victories"; "UnitPromotions"; "ArtDefine_StrategicView"; "Improvement_TechYieldChanges"; "Improvement_TechFreshWaterYieldChanges";
	 "Improvement_TechNoFreshWaterYieldChanges"; "Policy_ImprovementYieldChanges"; "UnitClasses"; "Belief_ImprovementYieldChanges"; "HandicapInfos"; "Beliefs"; "Controls";
	 "MinorCivTraits"; "Building_TechEnhancedYieldChanges"; "Projects"; "MinorCivilizations"; "GreatWorkSlots" };

local mem = function() return collectgarbage("count") * 1024 end

local gc = function() collectgarbage("collect") end

function loadAllTables( tab )
	--tab = createGameInfoObject( tab ) ;
	local tot1,tot2,tot3 = 0 ,0,0;
	gc();gc();gc();
	local mem_start = mem()
	gprint("T","C","P","Table", "Memory at start:", mem_start )
	for k,v in ipairs( dbgTablenames ) do
		local m1 = mem(gc()) ;
		for row in tab[v]() do
			tot1 = tot1 ;
		end
		local m2 = mem()
		local m3 = mem(gc()) ;
		tot1,tot2,tot3 = tot1 + m2 - m3 , tot2 + m2 - m1 , tot3 + m3 - m1 ;
		gprint( m2-m1, m2-m3, m3-m1, v ) 
	end
	gprint( tot1, tot2, tot3 , "(totals)", "Memory at end:", mem() , "Memory Delta", mem() - mem_start  ) ;
end

--[[

function createGameInfoAndLoadAllTablesNoPresets( )
	local tab = {} ;
	local backup = DB_PRESET_PRIO_NAMES;
	DB_PRESET_PRIO_NAMES = {} ;
	loadAllTables( tab ) ;
	DB_PRESET_PRIO_NAMES = backup ;
	return tab ;
end


function countUniqueColumnNamesAndNonDefaultEntries(tab)
	local COLUMNS = {} 
	
	for tablename,root in pairs( tab ) do
		for col, defval in pairs(root.__DEFVALUES) do
			COLUMNS[col] = COLUMNS[col] or {0,0,{}} ;
			for idx = 1, root.__COUNT do
				local val = root.__INDIVDATA[idx] and root.__INDIVDATA[idx][col]
				COLUMNS[col][3][ ((val == nil) and CONST_NIL) or val] = true ;
				if val == nil then
					COLUMNS[col][1] = COLUMNS[col][1] + 1 ;
				else
					COLUMNS[col][2] = COLUMNS[col][2] + 1 ;
				end
			end
			COLUMNS[col][tablename] = true ;
		end
	end
	print("D","S","Column")
	for col, stats in pairs(COLUMNS) do
		print(stats[1], stats[2], table.count(stats[3]), col)
	end
	return COLUMNS ;
end















--]]
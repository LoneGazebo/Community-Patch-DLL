FLAG_STATIC_INCLUDE_ENV = true ;

local gprint = print ;
local print = function( ... ) return gprint( "[VPL]", ... ) end

print("FILE LOADED");


local orig_include = include ;
local CodeBuddy = CodeBuddy ;
local EMPTY_TABLE = {} ;

local loadstring = loadstring ;
local pairs = pairs ;
local type = type ;
local ipairs = ipairs ;
local tostring = tostring ;
local error = error ;
local os_clock = os.clock ;
local coroutine_wrap = coroutine.wrap ;
local coroutine_yield = coroutine.yield ;

local __vpRegisterContext ;

local function psh( tab, val ) tab[#tab+1]=val end
local function rawgetfromenv( key ) local val = loadstring("return " .. key )(); if val == nil then print("Global is nil", key) end; return val; end
local function checkandwarn( tab, key ) if type(key) ~= "string" then print("Non string key!" , key ) elseif tab[key] ~= nil then print( "Key already exists!" , key ) end end

local init_unsafe = function(op)

	
	
	gprint( "################### VPLOADER:INIT@" .. tostring(StateName) .. "@" .. tostring(ContextPtr) .. "########################")
	local time = os_clock() ;




	staticIncludes = { -- This is an array, so that load order is properly defined by using ipairs later
		{
			file = "newGameInfo" ;
			objects = { "GameInfo" } ;
		};
		{ 
			file = "CPK.lua" ;
			file_aliases = { "CPK" } ;
			objects = { "CPK" } ;
		};
		{
			file = "VPUI_core" ;
			objects = { "VP" } ;
		};
		{ 
			file = "InstanceManager" ;
			objects = { "InstanceManager", "GenerationalInstanceManager" } ;
		};
		{
			file = "InfoTooltipInclude" ;
			packed_objects = { "EXPORTS_ITTI" } ;
		};
		
	}local staticIncludes = staticIncludes ;


	importTable = {
		-- serves as lookup for includes
	} local importTable = importTable ;
	
	generatedObjects = {
		-- Will hold key-object pairs to be imported
	}local generatedObjects = generatedObjects ;
	CodeBuddy.vpStaticObjects = generatedObjects ;
	

	do -- process the files declard in staticIncludes
	
	
		local tmp_already_included = {}
		
		local include_if_not_already = function( ... )
			if not tmp_already_included[(...)] then
				tmp_already_included[(...)] = orig_include( ... )
			end
			return tmp_already_included[(...)]
		end
		
		local wrapped_include = function( file )
			include = include_if_not_already ;
			local ret = include(file) ;
			include = orig_include ;
			return ret ;
		end
		
		for idx, entry in ipairs(staticIncludes) do
			
			-- Step 1: Include the file. Duplicate includes semi silently fail when include is wrapped. The core flag is mainly (not needed?) compat for CPK so that is can bind the original include instead...
			local tmp ;
			
			if entry.core then
				tmp = orig_include(entry.file) ;
			else
				tmp = wrapped_include(entry.file) ;
			end
			
			if tmp == nil or #tmp == 0 then
				print("Failed to include file!",entry.file);
				entry.importstr = false ;
			else
			
				-- Step 2:
				-- *A: Build a list of and fetch objects that this file has supposedly generated
				-- *B: Build a string to fetch them from the storage location we set up later
				
				tmp = "" ;
				
				-- objects field:
				for _, name in ipairs(entry.objects or EMPTY_TABLE) do
				
					checkandwarn(generatedObjects,name);
					generatedObjects[name] = rawgetfromenv(name) ;
					
					tmp = tmp .. ( name .. "=CodeBuddy.vpStaticObjects." .. name .. ";") ; 
				end
				
				-- packed_objects field:
				for _, packname in ipairs(entry.packed_objects or EMPTY_TABLE) do
					local pack = rawgetfromenv(packname) ;
					for name, object in pairs(pack or EMPTY_TABLE) do
					
						checkandwarn(generatedObjects,name);
						generatedObjects[name] = object ;
						
						tmp = tmp .. ( name .. "=CodeBuddy.vpStaticObjects." .. name .. ";" ) ; 
					end
				end
				
				if #tmp > 0 then
					entry.importstr = tmp ;
				else
					entry.importstr = false ;
				end
				
				-- Step 3: populate reverse lookup table
				importTable[entry.file] = entry ;
				for _,key in ipairs( entry.file_aliases or EMPTY_TABLE ) do
					importTable[key] = entry ;
				end
			end
		end
	end


	local function vpinclude( arg1, arg2 )
		if type(arg1) == "table" then --bulk include support
			for _, filename in ipairs(arg1) do vpinclude(filename) end
			
		elseif type(arg1) == "string" then
			if arg2 == nil and (importTable[arg1] or EMPTY_TABLE).importstr then -- arg2 == nil -> no regular exp and that file is a known static import
				loadstring( importTable[arg1].importstr )() ;
			else --if arg2 is provided or this isn't a known file, just do regular include
				return orig_include(arg1);
			end
			
		else -- arg1 not a string!?!?!?!?
			return error("Argument to vpinclude must be a string or a table!", arg1) ;
			
		end
	end
	
	
	
	generatedObjects.vpinclude = vpinclude ;
	generatedObjects.include = vpinclude ;
	generatedObjects.c5include = orig_include ;
	
	
	importTable.vpinclude = { importstr = [[
		vpinclude = CodeBuddy.vpStaticObjects.vpinclude ;
		c5include = CodeBuddy.vpStaticObjects.c5include ;
		
		include = CodeBuddy.vpStaticObjects.vpinclude ;
		]];
	}


	local CORE_LIBS = { "vpinclude" , "CPK.lua" , "newGameInfo" }
	local currentstate = function() return loadstring("return StateName;")() end
	
	__vpRegisterContext = function (...)
		if CodeBuddy.vpDebugHook and (not CodeBuddy.vpDebugHook(...)) then
			return false
		end
		
		--print( "@registerContext():",currentstate() )
		vpinclude( CORE_LIBS );
		
		return true ;
	end

	
	print( "INIT DONE! Time(ms):" , (os_clock() - time)*1000 )
end


local REINIT_REQ = "full" ;
local __init = coroutine_wrap(function(op)
	while true do
		init_unsafe(op)
		coroutine_yield()
	end
end)

local function __checkReinit()
	if REINIT_REQ then
		print("REINIT INITIATED!")
		__init(REINIT_REQ);
		REINIT_REQ = false ;
	end
end


local function __requestReinit()
	print("REINIT REQUESTED")
	if newGameInfo then
		newGameInfo("reset") ;
	end
	REINIT_REQ = "full" ;
end

CodeBuddy.vpRequestReinit = __requestReinit ;

function CodeBuddy.vpRegisterContext( ... )
	__checkReinit() ;
	return __vpRegisterContext(...) ;
end

function CodeBuddy.vpSetGameCore( GC )
	Player = GC.Player;
	NotificationTypes = GC.NotificationTypes;
	Team = GC.Team;
	CommandTypes = GC.CommandTypes;
	OrderTypes = GC.OrderTypes;
	ChatTargetTypes = GC.ChatTargetTypes;
	ActivityTypes = GC.ActivityTypes;
	FaithPurchaseTypes = GC.FaithPurchaseTypes;
	ResolutionDecisionTypes = GC.ResolutionDecisionTypes;
	EndTurnBlockingTypes = GC.EndTurnBlockingTypes;
	ReligionTypes = GC.ReligionTypes;
	Game = GC.Game;
	CoopWarStates = GC.CoopWarStates;
	GameOptionTypes = GC.GameOptionTypes;
	Map = GC.Map;
	MissionTypes = GC.MissionTypes;
	GameInfoTypes = GC.GameInfoTypes;
	ControlTypes = GC.ControlTypes;
	ThreatTypes = GC.ThreatTypes;
	Fractal = GC.Fractal;
	InfluenceLevelTypes = GC.InfluenceLevelTypes;
	FlowDirectionTypes = GC.FlowDirectionTypes;
	GameMessageTypes = GC.GameMessageTypes;
	GameInfoActions = GC.GameInfoActions;
	DiploUIStateTypes = GC.DiploUIStateTypes;
	TerrainTypes = GC.TerrainTypes;
	DisputeLevelTypes = GC.DisputeLevelTypes;
	FogOfWarModeTypes = GC.FogOfWarModeTypes;
	InfluenceLevelTrend = GC.InfluenceLevelTrend;
	Players = GC.Players;
	Teams = GC.Teams;
	BeliefTypes = GC.BeliefTypes;
	PublicOpinionTypes = GC.PublicOpinionTypes;
	LeaderheadAnimationTypes = GC.LeaderheadAnimationTypes;
	GameplayGameStateTypes = GC.GameplayGameStateTypes;
	MapModData = GC.MapModData;
	ResourceUsageTypes = GC.ResourceUsageTypes;
	InterfaceModeTypes = GC.InterfaceModeTypes;
	FromUIDiploEventTypes = GC.FromUIDiploEventTypes;
	GameStates = GC.GameStates;
	GenericWorldAnchorTypes = GC.GenericWorldAnchorTypes;
	AdvisorTypes = GC.AdvisorTypes;
	YieldTypes = GC.YieldTypes;
	PlotTypes = GC.PlotTypes;
	ButtonPopupTypes = GC.ButtonPopupTypes;
	CityUpdateTypes = GC.CityUpdateTypes;
	MinorCivPersonalityTypes = GC.MinorCivPersonalityTypes;
	DomainTypes = GC.DomainTypes;
	MinorCivTraitTypes = GC.MinorCivTraitTypes;
	MajorCivApproachTypes = GC.MajorCivApproachTypes;
	TradeableItems = GC.TradeableItems;
	MinorCivQuestTypes = GC.MinorCivQuestTypes;
	CityAIFocusTypes = GC.CityAIFocusTypes;
	TaskTypes = GC.TaskTypes;
	FeatureTypes = GC.FeatureTypes;
	DirectionTypes = GC.DirectionTypes;
	ActionSubTypes = GC.ActionSubTypes;
	InterfaceDirtyBits = GC.InterfaceDirtyBits;
	CombatPredictionTypes = GC.CombatPredictionTypes;
	PolicyBranchTypes = GC.PolicyBranchTypes;
end


--[[ These fire later
Events.AfterModsDeactivate.Add(function()
	print("VPLOADER TEST AFTER MODS DEACTIVATE")
end);

Events.AfterModsActivate.Add(function()
	print("VPLOADER TEST AFTER MODS ACTIVATE")
end);
--]]


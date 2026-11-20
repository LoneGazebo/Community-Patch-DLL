print("GAMECOREINIT.LUA!")

local REGISTRY = debug.getregistry() ;

local _G = REGISTRY._LOADED._G ;
local THIS = coroutine.running() ;
local _ENV = _G.getfenv(0) ;
debug.dllcontext = _ENV ;

local pexec = function(func,...) local C,M = pcall(func,...) if not C then print(M) end end ;
local wrap = function(func) return (function(...) pexec(func,...) end) end ;
local resetGamecore = function() _G.GameCore = nil ; REGISTRY.vpdllinternals = nil end ;

_ENV._eventfunc = wrap(function( msg )
	print("Received event:", msg) ;
	if msg == "gc_reset" then
		resetGamecore();
	elseif msg == "gc_post" then
		postinit(_G.GameCore)
	elseif msg == "shutdown" then
		resetGamecore();
		setmetatable(_G.Threads,nil) ;
		debug.dllcontext = nil ;
		_G.Threads[THIS] = nil ;
	end
end);

function postinit( core )
	local vpreg = REGISTRY.vpdllinternals ;
	core.LuaTypes = {} ;
	for tag,tab in pairs(vpreg) do
		if tab.__hint then
			vpreg[tab.__hint] = tab ;
			core[tab.__hint] = tab.__meta.__index ;
			core.LuaTypes[tab.__hint] = tab.__meta.__index ;
		end
	end
	
	include("GamecorePostProcess.lua");
	do_gamecore_post(core,vpreg);
end

function pushUtils(tab)
	tab._ENV = tab ;
	tab.vputils = vputils ;
end
function spawnDllContext(tab) 
	--tab.ContextPtr:LoadNewContext("Main~VpDLL");
end

pexec(function()
	local listener = { __newindex = function(tab,key,val) 
		rawset(tab,key,val);
		if key == "include" and not tab.StateName then
			--print("DISATTACH FROM UNNAMED CONTEXT... GAMECORE/MAPCHECKER?");
			setmetatable(tab,nil);
		elseif key == "include" and tab.StateName == "LoadScreen" then
			print("Resetting lua gamecore api before LoadScreen context!");
			resetGamecore();
		elseif key == "Controls" then
			setmetatable(tab,nil);
			pexec(pushUtils,tab);
			if tab.StateName == "FrontEnd" or tab.StateName == "LoadScreen" then
				pexec(spawnDllContext,tab);
			end
		end
	end;};
	setmetatable(_G.Threads,{__newindex = function(tab,thread,env) 
		rawset(tab,thread,env);
		setmetatable(env,listener);
	end;});
end);

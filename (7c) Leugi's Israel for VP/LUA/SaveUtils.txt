-- vymdt.08.2011.02.13.0000
-- Created by: Ryan F. Mercer -Open source
--===========================================================================
-- SaveUtils.lua
--===========================================================================
--[[ Special Thanks: killmeplease, Thalassicus, Afforess.

*Nothing in this file should be changed.

Allows concurrent mods to share the ScriptData save slot on all Player, Plot,
and Unit objects, makes it possible to save complex tables or any data type
except function, userdata, and thread, and provides unlimited save slots per
target object.  Includes cache functionality for improved performance and
shares cache and cache-state as super-globals accessible to all lua states
(requires ShareData.lua).

To use this file, place the following line of code in your modded file's
global scope and set your mod's unique name.

  include( "SaveUtils" ); MY_MOD_NAME = "MyMod";

Automatically shares cache and cache-state upon Events.LoadScreenClose().
Call share_SaveUtils() explicitly to share cache and cache-state immediately.
Explicit call necessary when shared data operations performed from global
scope or any point prior to Events.LoadScreenClose().  Avoid explicit call
whenever possible for greater interoperability of concurrent mods.

-----------------------------------------------------------------------------
Basic Functionality
-----------------------------------------------------------------------------
  
  save( pPlayer, key1, "this" ); --ONLY string and integer keys recommended.
  save( pPlayer, key2, "200" );  --loads as string.
  save( pPlayer, key3, 300 );    --loads as number.
  save( pPlayer, key4, true );   --loads as boolean.
  save( pPlayer, nil,  "that" ); --automatic integer index.
  save( pPlayer, key1, table, "otherMod" ); --optional fourth parameter.
  save( pPlot,   ...,  ...,   ... ); --same as above.
  save( pUnit,   ...,  ...,   ... ); --same as above.

  delete( pPlayer, key1 );             --deletes key and value from table.
  delete( pPlayer, key1, "otherMod" ); --optional third parameter.

  data = load( pPlayer, key1 );
  data = load( pPlayer, key1, "otherMod" ); --optional third parameter.
  data = load( pPlot,   ...,  ... ); --same as above.
  data = load( pUnit,   ...,  ... ); --same as above.

  data = load( pPlayer );                  --entire mod table for pPlayer.
  data = load( pPlayer, nil, "otherMod" ); --entire mod table for pPlayer.

  data = load( pPlayer, nil, false ); --entire save table for pPlayer.
  data = load( pPlot,   nil, false ); --entire save table for pPlot.
  data = load( pUnit,   nil, false ); --entire save table for pUnit.

Invalid data types: function, userdata, thread.  Also be aware that while it
is technically possible to use any other data type except nil for a key,
using a table for reference, ie: load(pPlayer,table), will produce
undesirable results.  It will appear to work normally while still in cache,
only to then fail across saved games.  This is not a limitation of SaveUtils,
but rather is an inherent limitation of all serialized data.  That said, the
following will always work as expected.

  save(pPlayer,{"table key"},"value");
  for k,v in pairs(load(pPlayer)) do

-----------------------------------------------------------------------------
Cache Functionality
-----------------------------------------------------------------------------

Data caching can be set to one of three states per mod:
0 = no cache, 1 = serialize on save(), 2 = serialize on sync().  Default: 1.

  setCacheState( 0 ); --no cache.
  setCacheState( 0, "otherMod" ); --optional second parameter.

  clear();          --error.
  clear( true );    --clears entire cache.
  clear( pPlayer ); --clears pPlayer cache.

  sync();           --error.
  sync( true );     --serializes entire cache.
  sync( pPlayer );  --serializes pPlayer cache.

When using serialize on sync(), it is the modders responsibility to call
global function sync() to serialize cache data, otherwise game data will not
persist across saved games.  Also be aware that multiple lua states may read
from or write to the serialized data at any time.  Lua states that act on the
serialized data should first inspect the cache to avoid desynchronization
(requires ShareData.lua).

It is also possible to override the cache-state on any particular call to
save() and load() by giving the desired cache-state as an optional last
argument.  This can be particularly useful when retrieving the entire save
table, ie: load(pPlayer, nil, false, 0), because logically mod false can not
have a cache-state and thus otherwise uses the cache-state for MY_MOD_NAME.

-----------------------------------------------------------------------------
Share Functionality
-----------------------------------------------------------------------------

Requires ShareData.lua

Shared data can be of any type, including an object or function reference.
Data shared as super-global requires key be unique for all lua states. Data
shared as context-global only requires key be unique for that context.  Share
data as super-global when you want all lua states to act on that same data,
and share data as context-global when you want to make it accessible to any
lua state that specifies your context (mostly for read only).  The context
should be the name of the LuaContext entry or UIAddin file name without file
extension.

It is recommended that data be shared upon Events.LoadScreenClose() to ensure
synchronized order of operations across concurrent mods.  For example,
SaveUtils automatically shares cache and cache-state in the following manner.
--------
g_cacheState, g_pCache = {}, Cache:new();

function share_SaveUtils()
  g_cacheState  = share( "SaveUtils.g_cacheState",  g_cacheState  );
  g_pCache      = share( "SaveUtils.g_pCache",      g_pCache      );
end
Events.LoadScreenClose.Add( share_SaveUtils );
--------

Share References:

  data = share( key, data or default );           --super-global.
  data = share( key, data or default, context );  --context-global.

Once a reference is shared, any actions on that reference by any lua state
take immediate affect for all lua states, as expected for a reference.  Thus
only share() is needed upon instatiation and perhaps rmvShared() on clean-up.

Share Primitives:
  
  --super-global.
  old  = setShared( key, data );
  data = getShared( key );
  case = hasShared( key );

  --context-global.
  old  = setShared( key, data, context );
  data = getShared( key, context );
  case = hasShared( key, context );
  
  --share with automatic integer index.
  setShared( nil, data, context ); --ONLY context-globals.

Primitives (number, string, boolean) are copies not references, thus while it
is possible to share a primitive upon Events.LoadScreenClose, getShared() and
setShared() must still be called later to keep copies syncronized.  For this
reason it may be preferable to assign primitives to a shared table for
reference with key.

Unshare:

  --remove primitive or reference.
  data = rmvShared( key );           --super-global.
  data = rmvShared( key, context );  --context-global.

Be aware that removing a reference from shared data can not eliminate any
additional references created while data was shared.

Other:

  data = getShared();                --entire table of super-globals.
  data = getShared( nil, context );  --entire table of context-globals.
  data = getShared( nil, true );     --entire table of context tables.

  --supress 'not shared' warnings.
  WARN_NOT_SHARED = false; include( "SaveUtils" ); MY_MOD_NAME = "MyMod";
]]
--===========================================================================
-- Save Class
--===========================================================================
--[[
Allows SetScriptData() and GetScriptData() to behave as a hash-table of
itemized data, while maintaining separation of individual mod data.
]]
Save = {};
function Save:new( o )
  o = o or {};
  if type( o ) == "string" and o == "" then o = {}; end
  if type( o ) ~= "table" then o = {o}; end
  setmetatable( o, self );
  self.__index = self;
  return o;
end
--===========================================================================
--[[
Sets the given value to the given key for the given mod.
]]
function Save:set( mod, key, value )
  self[mod] = self[mod] or {};
  if key ~= nil then self[mod][key] = value;
  else table.insert( self[mod], value );
  end
  local i=0; for k,v in pairs( self[mod] ) do i=1; break; end
  if i == 0 then self[mod] = nil; end
end
--===========================================================================
--[[
Returns value for given mod and key. Returns copy of self when mod not given.
Returns mod table when key not given.
]]
function Save:get( mod, key )
  
  local function copy( object, meta ) --deep copy.
    local copied = {};
    local function sub( object )
      if type( object ) ~= "table" then return object; end
      if copied[object] then return copied[object]; end
      local new_table = {};
      copied[object] = new_table;
      for k,v in pairs( object ) do new_table[sub( k )] = sub( v ); end
      local new_meta = getmetatable( object );
      if meta == true then new_meta = sub( new_meta ); end
      return setmetatable( new_table, new_meta );
    end
    return sub( object );
  end

  local r = nil;
  if mod ~= nil then self[mod] = self[mod] or {}; r = self[mod];
    if key ~= nil then r = self[mod][key]; end
  else r = copy( self );
  end
  return r;
end
--===========================================================================
--END Save Class
--===========================================================================
--===========================================================================
-- Cache Class
--===========================================================================
--[[
Manages object keys to unserialized object data.
]]
Cache = {};
function Cache:new( o )
  o = o or {};
  setmetatable( o, self );
  self.__index = self;
  return o;
end
--===========================================================================
--[[
Returns boolean state for given target.
]]
function Cache:has( target )
  r = false;
  if self[target] then r = true; end
  return r;
end
--===========================================================================
--[[
Sets given Save for given target.
]]
function Cache:add( target, save )
  if target ~= nil and save ~= nil then self[target] = save; end
end
--===========================================================================
--[[
Returns Save for given target.  Returns copy of self when target not given.
]]
function Cache:get( target )

  local function copy( object, meta ) --deep copy.
    local copied = {};
    local function sub( object )
      if type( object ) ~= "table" then return object; end
      if copied[object] then return copied[object]; end
      local new_table = {};
      copied[object] = new_table;
      for k,v in pairs( object ) do new_table[sub( k )] = sub( v ); end
      local new_meta = getmetatable( object );
      if meta == true then new_meta = sub( new_meta ); end
      return setmetatable( new_table, new_meta );
    end
    return sub( object );
  end

  local r = self[target];
  if r == nil then r = copy( self ); end
  return r;
end
--===========================================================================
--[[
Removes and returns Save for given target.
]]
function Cache:rmv( target )
  return table.remove( self, target );
end
--===========================================================================
--END Cache Class
--===========================================================================
--===========================================================================
--[[
Sets cache-state for given mod to given state. Uses global MY_MOD_NAME If mod
not given.
]]
function setCacheState( state, mod )
  local t = type( state );
  if not t == "number" then
    print( "setCacheState(): Invalid first argument of type "..t
        ..", expected number." );
    return false; --error.
  end
  if mod == nil then mod = MY_MOD_NAME; end
  t = type( mod );
  if not (t == "number" or t == "string") then
    print( "setCacheState(): Invalid second argument of type "..t
        ..", expected number, or string." );
    return false; --error.
  end
  g_cacheState[mod] = state;
end
--===========================================================================
--[[
Returns cache-state for given mod.  Uses global MY_MOD_NAME If mod not given.
]]
function getCacheState( mod )
  if mod == nil then mod = MY_MOD_NAME; end
  local t = type( mod );
  if not (t == "number" or t == "string") then
    print( "getCacheState(): Invalid first argument of type "..t
        ..", expected number, or string." );
    return false; --error.
  end
  return g_cacheState[mod];
end
--===========================================================================
--[[
Removes cache data for given target, or all data when target is boolean true.
]]
function clear( target )
  local t = type( target );
  if not (t == "table" or target == true) then
    print( "clear(): Invalid first argument of type "..t
        ..", expected table, or boolean true." );
    return false; --error.
  end
  if WARN_NOT_SHARED ~= false and
      not hasShared( "SaveUtils.g_pCache" ) then
    print( "Warning: cache not shared." ); --warning.
  end
  if target == true then
    for k,pSave in pairs( g_pCache ) do g_pCache:rmv( k ); end
  else g_pCache:rmv( target );
  end
end
--===========================================================================
--[[
Serializes cache data for given target, or all cache data when target is
boolean true.
]]
function sync( target )
  local t = type( target );
  if not (t == "table" or target == true) then
    print( "sync(): Invalid first argument of type "..t
        ..", expected table, or boolean true." );
    return false; --error.
  end
  if WARN_NOT_SHARED ~= false and
      not hasShared( "SaveUtils.g_pCache" ) then
    print( "Warning: cache not shared." ); --warning.
  end
  if target == true then
    for k,pSave in pairs( g_pCache ) do
      k:SetScriptData(serialize( pSave ));
    end
  else
    local pSave = g_pCache:get( target );
    if pSave == nil then
      print( "sync(): Target not found." ); return false; --error.
    else target:SetScriptData(serialize( pSave ));
    end
  end
end
--===========================================================================
--[[
Saves the given value to the given key for the given target of the given mod.
Invalid data types: function, userdata, thread.  Uses global MY_MOD_NAME If
mod not given.  Can optionally override the cache-state for any particular
call.  This can be particularly useful when mod is boolean false.
]]
function save( target, key, value, mod, cacheState )
  local t = type( target );
  if not t == "table" then
    print( "save(): Invalid first argument of type "..t
        ..", expected table." );
    return false; --error.
  end  
  if mod == nil then mod = MY_MOD_NAME; end
  t = type( mod );
  if not (t == "number" or t == "string") then
    print( "save(): Invalid fourth argument of type "..t
        ..", expected number, or string." );
    return false; --error.
  end
  t = type( cacheState );
  if not (cacheState == nil or t == "number") then
    print( "save(): Invalid fifth argument of type "..t
        ..", expected nil, or number." );
    return false; --error.
  end
  local pSave = nil;
  local iCache = DEFAULT_CACHE_STATE;
  if cacheState ~= nil then iCache = cacheState;
  else
    if getCacheState( mod ) == nil then setCacheState( iCache, mod );
    else iCache = getCacheState( mod );
    end
  end
  pSave = load( target, nil, false, iCache );
  pSave:set( mod, key, value );
  if iCache <= 1 then target:SetScriptData(serialize( pSave )); end
  if iCache >= 1 then g_pCache:add( target, pSave ); end
end
--===========================================================================
--[[
Loads the value for the given key for the given target of the given mod.
Returns entire mod table for the given target when key is nil.  Returns
entire save table for the given target when mod is boolean false.  Uses
global MY_MOD_NAME If mod not given.  Can optionally override the cache-state
for any particular call.  This can be particularly useful when mod is boolean
false, because logically mod false can not have a cache-state and thus
otherwise uses default cache-state or cache-state for MY_MOD_NAME when set.
]]
function load( target, key, mod, cacheState )
  local t = type( target );
  if not t == "table" then
    print( "load(): Invalid first argument of type "..t
        ..", expected table." );
    return nil; --error.
  end    
  if mod == nil then mod = MY_MOD_NAME; end
  t = type( mod );
  if not (t == "number" or t == "string" or mod == false) then
    print( "load(): Invalid third argument of type "..t
        ..", expected number, string, or boolean false." );
    return nil; --error.
  end
  if mod == false then mod = nil; end
  t = type( cacheState );
  if not (cacheState == nil or t == "number") then
    print( "load(): Invalid fourth argument of type "..t
        ..", expected nil, or number." );
    return nil; --error.
  end
  local pSave = nil;
  local iCache = DEFAULT_CACHE_STATE;
  if cacheState ~= nil then iCache = cacheState;
  else
    if mod == nil then
      if getCacheState( MY_MOD_NAME ) ~= nil then
        iCache = getCacheState( MY_MOD_NAME );
      end
    else
      if getCacheState( mod ) == nil then setCacheState( iCache, mod );
      else iCache = getCacheState( mod );
      end
    end
  end
  if iCache < 1 then pSave = Save:new(deserialize( target:GetScriptData() ));
  elseif iCache > 0 then
    if WARN_NOT_SHARED ~= false then
      local warned = false;
      if not hasShared( "SaveUtils.g_pCache" ) then
        print( "Warning: cache not shared." ); --warning.
        warned = true;
      end
      if not warned and not hasShared( "SaveUtils.g_cacheState" ) then
        print( "Warning: cache-state not shared." ); --warning.
      end
    end
    if g_pCache:has( target ) then pSave = g_pCache:get( target );
    else
      pSave = Save:new(deserialize( target:GetScriptData() ));
      g_pCache:add( target, pSave );
    end
  end
  return pSave:get( mod, key );
end
--===========================================================================
--[[
Removes both the key and value for the given target of the given mod
with the given key. Alias for function save( target, key, nil, mod ).
]]
function delete( target, key, mod )
  save( target, key, nil, mod );
end
--===========================================================================
--[[
Serializes given data and returns result string.  Invalid data types:
function, userdata, thread.
]]
function serialize( p )
  
  local r = ""; local t = type( p );
  if t == "function" or t == "userdata" or t == "thread" then
    print( "serialize(): Invalid type: "..t ); --error.
  elseif p ~= nil then
    if t ~= "table" then
      if p == nil or p == true or p == false
        or t == "number" then r = tostring( p );
      elseif t == "string" then
        if p:lower() == "true" or p:lower() == "false"
            or tonumber( p ) ~= nil then r = '"'..p..'"';
        else r = p;
        end
      end
      r = r:gsub( "{", "\[LCB\]" );
      r = r:gsub( "}", "\[RCB\]" );
      r = r:gsub( "=", "\[EQL\]" );
      r = r:gsub( ",", "\[COM\]" );
    else
      r = "{"; local b = false;
      for k,v in pairs( p ) do
        if b then r = r..","; end
        r = r..serialize( k ).."="..serialize( v );
        b = true;
      end
      r = r.."}"
    end
  end
  return r;
end
--===========================================================================
--[[
Deserializes given string and returns result data.
]]
function deserialize( str )

  local findToken = function( str, int )
    if int == nil then int = 1; end
    local s, e, c = str:find( "({)" ,int);
    if s == int then --table.
      local len = str:len();
      local i = 1; --open brace.
      while i > 0 and s ~= nil and e <= len do --find close.
        s, e, c = str:find( "([{}])" ,e+1);
        if     c == "{" then i = i+1;
        elseif c == "}" then i = i-1;
        end
      end
      if i == 0 then c = str:sub(int,e);
      else print( "deserialize(): Malformed table." ); --error.
      end
    else s, e, c = str:find( "([^=,]*)" ,int); --primitive.
    end
    return s, e, c, str:sub( e+1, e+1 );
  end

  local r = nil; local s, c, d;
  if str ~= nil then
    local sT, eT, cT = str:find( "{(.*)}" );
    if sT == 1 then
      r = {}; local len = cT:len(); local e = 1;
      if cT ~= "" then
        repeat
          local t1, t2; local more = false;
          s, e, c, d = findToken( cT, e );
          if s ~= nil then t1 = deserialize( c ); end
          if d == "=" then --key.
            s, e, c, d = findToken( cT, e+2 );
            if s ~= nil then t2 = deserialize( c ); end
          end
          if d == "," then e = e+2; more = true; end --one more.
          if t2 ~= nil then r[t1] = t2;
          else table.insert( r, t1 );
          end
        until e >= len and not more;
      end
    elseif tonumber(str) ~= nil then r = tonumber(str);
    elseif str == "true"  then r = true;
    elseif str == "false" then r = false;
    else
      s, e, c = str:find( '"(.*)"' );
      if s == 1 and e == str:len() then
        if c == "true" or c == "false" or tonumber( c ) ~= nil then
          str = c;
        end
      end
      r = str;
      r = r:gsub( "%[LCB%]", "{" );
      r = r:gsub( "%[RCB%]", "}" );
      r = r:gsub( "%[EQL%]", "=" );
      r = r:gsub( "%[COM%]", "," );
    end
  end
  return r;
end
--===========================================================================
--[[
Makes given data accessible to all lua states.  Intended for use upon data
instatiation, ie: local data = share( key, data or default, context );
Does not "set" shared data unless not already set, otherwise just returns
already set data.  Requires ShareData.lua.
]]
function share( key, data, context )
  local t = type( key );
  if not (t == "number" or (t == "string" and key ~= "")) then
    print( "share(): Invalid first argument of type "..t
        ..", expected number, or unempty-string." );
    return data; --error.
  end
  local t = type( context );
  if not (t == "nil" or (t == "string" and context ~= "")) then
    print( "share(): Invalid third argument of type "..t
        ..", expected nil, or unempty-string." );
    return data; --error.
  end
  local r, t = data, {}; LuaEvents.HasShared( key, t, context );
  if not t[1] then LuaEvents.SetShared( key, data, context );
  else t = {}; LuaEvents.GetShared( key, t, context ); r = t[key];
  end return r;
end
--===========================================================================
--[[
Returns boolean case of shared context-global for key or boolean case of
shared super-global for key when context not given.  Requires ShareData.lua.
]]
function hasShared( key, context )
  local r = {}; LuaEvents.HasShared( key, r, context ); return r[1] or false;
end
--===========================================================================
--[[
Sets given data to given key as shared context-global or shared super-global
when context not given.  Sets given data to automatic integer-key when key
not given.  Must give key when context not given.  Returns previous data for
given key.  Requires ShareData.lua.
]]
function setShared( key, data, context )
  if key == nil and context == nil then
    print( "setShared(): Invalid first and third argument combination of "
        .."types "..type( key ).." and "..type( context )
        ..", expected any type except nil for key when context nil." );
    return nil; --error.
  end
  local r = {}; LuaEvents.SetShared( key, data, context, r );
  if key ~= nil then r = r[key]; else r = nil; end return r;
end
--===========================================================================
--[[
Returns shared context-global for key or shared super-global for key when
context not given.  Returns entire table of context-globals for given context
when key not given.  Returns entire table of super-globals when both key and
context not given.  Returns entire table of context tables when key not given
and given context boolean true.  Requires ShareData.lua.
]]
function getShared( key, context )
  local r = {}; LuaEvents.GetShared( key, r, context );
  if key ~= nil then r = r[key]; end return r;
end
--===========================================================================
--[[
Removes and returns shared context-global for key or shared super-global for
key when context not given.  Alias for function setShared(key, nil, context).
Requires ShareData.lua.
]]
function rmvShared( key, context )
  return setShared( key, nil, context );
end
--===========================================================================
--[[
Global Variables.

Cache and cache-state automatically shared as super-globals upon
Events.LoadScreenClose().
]]
MY_MOD_NAME, DEFAULT_CACHE_STATE = nil, 1;
g_cacheState, g_pCache = {}, Cache:new();
--===========================================================================
--[[
Shares cache and cache-state as super-globals.
]]
function share_SaveUtils()
  g_cacheState  = share( "SaveUtils.g_cacheState",  g_cacheState  );
  g_pCache      = share( "SaveUtils.g_pCache",      g_pCache      );
end
Events.LoadScreenClose.Add( share_SaveUtils );
--===========================================================================
--END SaveUtils.lua
--===========================================================================
-- Created by: Ryan F. Mercer -Open source
EUI = {} local EUI = EUI
local civ5_mode = InStrategicView ~= nil
local civBE_mode = not civ5_mode
local bnw_mode, gk_mode

-------------------------------------------------
-- Minor lua optimizations
-------------------------------------------------
local math_ceil = math.ceil
local math_floor = math.floor
local math_max = math.max
local math_min = math.min
local math_sqrt = math.sqrt
local pairs = pairs
local print = print
local setmetatable = setmetatable
local table_insert = table.insert
local table_remove = table.remove
local tonumber = tonumber
local tostring = tostring
local unpack = unpack or table.unpack -- depends on Lua version

local ContentManager_IsActive = ContentManager.IsActive
local ContentType_GAMEPLAY = ContentType.GAMEPLAY
local DB_Query = DB.Query
local Game = Game
local GameDefines = GameDefines
local GameInfo = GameInfo
local L = Locale.ConvertTextKey
local Locale = Locale
local PreGame = PreGame
local TradeableItems = TradeableItems

if Game and PreGame then

	local Map_GetPlot = Map.GetPlot
	local Map_GetPlotXY = Map.GetPlotXY
	local g_savedDealStack = {}
	local g_deal = UI.GetScratchDeal()

	function EUI.PushScratchDeal()
--print("PushScratchDeal")
		-- save curent deal
		local item = {}
		local deal = {}
		item.SetFromPlayer = g_deal:GetFromPlayer()
		item.SetToPlayer = g_deal:GetToPlayer()
		item.SetSurrenderingPlayer = g_deal:GetSurrenderingPlayer()
		item.SetDemandingPlayer = g_deal:GetDemandingPlayer()
		item.SetRequestingPlayer = g_deal:GetRequestingPlayer()

		g_deal:ResetIterator()
		repeat
--print( unpack(item) )
			table_insert( deal, item )
			item = { g_deal:GetNextItem() }
		until #item < 1
		table_insert( g_savedDealStack, deal )
		g_deal:ClearItems()
	end

	local g_deal_functions = {
			[ TradeableItems.TRADE_ITEM_MAPS or false ] = function( from )
				return g_deal:AddMapTrade( from )
			end,
			[ TradeableItems.TRADE_ITEM_RESOURCES or false ] = function( from, item )
				return g_deal:AddResourceTrade( from, item[4], item[5], item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_CITIES or false ] = function( from, item )
				local plot = Map_GetPlot( item[4], item[5] )
				local city = plot and plot:GetPlotCity()
				if city and city:GetOwner() == from then
					return g_deal:AddCityTrade( from, city:GetID() )
				else
					print( "Cannot add city trade", city and city:GetName(), unpack(item) )
				end
			end,
			[ TradeableItems.TRADE_ITEM_UNITS or false ] = function( from, item )
				return g_deal:AddUnitTrade( from, item[4] )
			end,
			[ TradeableItems.TRADE_ITEM_OPEN_BORDERS or false ] = function( from, item )
				return g_deal:AddOpenBorders( from, item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_TRADE_AGREEMENT or false ] = function( from, item )
				return g_deal:AddTradeAgreement( from, item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_PERMANENT_ALLIANCE or false ] = function()
				print( "Error - alliance not supported by game DLL")--g_deal:AddPermamentAlliance()
			end,
			[ TradeableItems.TRADE_ITEM_SURRENDER or false ] = function( from )
				return g_deal:AddSurrender( from )
			end,
			[ TradeableItems.TRADE_ITEM_TRUCE or false ] = function()
				print( "Error - truce not supported by game DLL")--g_deal:AddTruce()
			end,
			[ TradeableItems.TRADE_ITEM_PEACE_TREATY or false ] = function( from, item )
				return g_deal:AddPeaceTreaty( from, item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE or false ] = function( from, item )
				return g_deal:AddThirdPartyPeace( from, item[4], item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR or false ] = function( from, item )
				return g_deal:AddThirdPartyWar( from, item[4] )
			end,
			[ TradeableItems.TRADE_ITEM_THIRD_PARTY_EMBARGO or false ] = function( from, item )
				return g_deal:AddThirdPartyEmbargo( from, item[4], item[2] )
			end,
			-- civ5
			[ TradeableItems.TRADE_ITEM_GOLD or false ] = function( from, item )
				return g_deal:AddGoldTrade( from, item[4] )
			end,
			[ TradeableItems.TRADE_ITEM_GOLD_PER_TURN or false ] = function( from, item )
				return g_deal:AddGoldPerTurnTrade( from, item[4], item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_DEFENSIVE_PACT or false ] = function( from, item )
				return g_deal:AddDefensivePact( from, item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT or false ] = function( from, item )
				return g_deal:AddResearchAgreement( from, item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_ALLOW_EMBASSY or false ] = function( from )
				return g_deal:AddAllowEmbassy( from )
			end,
			[ TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP or false ] = function( from )
				return g_deal:AddDeclarationOfFriendship( from )
			end,
			[ TradeableItems.TRADE_ITEM_VOTE_COMMITMENT or false ] = function( from, item )
				return g_deal:AddVoteCommitment( from, item[4], item[5], item[6], item[7] )
			end,
			-- civ be
			[ TradeableItems.TRADE_ITEM_ENERGY or false ] = function( from, item )
				return g_deal:AddGoldTrade( from, item[4] )
			end,
			[ TradeableItems.TRADE_ITEM_ENERGY_PER_TURN or false ] = function( from, item )
				return g_deal:AddGoldPerTurnTrade( from, item[4], item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_ALLIANCE or false ] = function( from, item )
				return g_deal:AddAlliance( from, item[2] )
			end,
			[ TradeableItems.TRADE_ITEM_COOPERATION_AGREEMENT or false ] = function( from )
				return g_deal:AddCooperationAgreement( from )
			end,
			[ TradeableItems.TRADE_ITEM_FAVOR or false ] = function( from, item )
				return g_deal:AddFavorTrade( from, item[4] )
			end,
			[ TradeableItems.TRADE_ITEM_RESEARCH_PER_TURN or false ] = function( from, item )
				return g_deal:AddResearchPerTurnTrade( from, item[4], item[2] )
			end,
			-- cdf / cp / cbp
			[ TradeableItems.TRADE_ITEM_VASSALAGE or false ] = function( from )
				return g_deal:AddVassalageTrade( from )
			end,
			[ TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE or false ] = function( from )
				return g_deal:AddRevokeVassalageTrade( from )
			end,
			[ TradeableItems.TRADE_ITEM_TECHS or false ] = function( from, item )
				return g_deal:AddTechTrade( from, item[4] )
			end,
	} g_deal_functions[false] = nil

	function EUI.PopScratchDeal()
--print("PopScratchDeal")
		-- restore saved deal
		g_deal:ClearItems()
		local deal = table_remove( g_savedDealStack )
		if deal then
			for k,v in pairs( deal[1] ) do
				g_deal[ k ]( g_deal, v )
			end

			for i = 2, #deal do
				local item = deal[ i ]
				local from = item[#item]
				local tradeType = item[1]
				local f = g_deal_functions[ tradeType ]
				if f and g_deal:IsPossibleToTradeItem( from, g_deal:GetOtherPlayer(from), tradeType, item[4], item[5], item[6], item[7] ) then
					f( from, item )
				else
					print( "Cannot restore deal trade", unpack(item) )
				end
			end
--print( "Restored deal#", #g_savedDealStack ) g_deal:ResetIterator() repeat local item = { g_deal:GetNextItem() } print( unpack(item) ) until #item < 1
		else
			print( "Cannot pop scratch deal" )
		end
	end

	local g_maximumAcquirePlotArea = (GameDefines.MAXIMUM_ACQUIRE_PLOT_DISTANCE+1) * GameDefines.MAXIMUM_ACQUIRE_PLOT_DISTANCE * 3
	local g_maximumAcquirePlotPerimeter = GameDefines.MAXIMUM_ACQUIRE_PLOT_DISTANCE * 6

	if Map_GetPlot(0,0).GetCityPurchaseID then
		EUI.CityPlots = function( city )
			local cityID = city:GetID()
			local cityOwnerID = city:GetOwner()
			local m = 0
			local n = g_maximumAcquirePlotArea
			local p = g_maximumAcquirePlotPerimeter
			return function()
				repeat
					for i = m, n do
						local plot = city:GetCityIndexPlot( i )
						if plot	and plot:GetOwner() == cityOwnerID
							and plot:GetCityPurchaseID() == cityID
						then
							m = i+1
							return plot
						end
					end
					-- if no owned plots were found in previous ring then we're done
					if m <= n-p+1 then
						return
					end
					-- plots found, search next ring
					m = n + 1	--first plot of next ring
					p = p + 6	--perimeter of next ring
					n = n + p	--last plot of next ring
				until false
			end
		end
	else
		EUI.CityPlots = function( city )
			local m = 0
			return function()
				for i = m, g_maximumAcquirePlotArea do
					local plot = city:GetCityIndexPlot( i )
					if plot	and plot:GetWorkingCity() == city
					then
						m = i+1
						return plot
					end
				end
			end
		end
	end

	--usage1: IndexPlot( plot, hexAreaPlotIndex )
	--OR usage2: IndexPlot( plot, ringPlotIndex, ringDistanceFromPlot )
	function EUI.IndexPlot( plot, i, r )
		-- determine if input parameters are valid - you can delete this part for a tiny performance boost
		if not plot or not i or i<0 or (r and (r<0 or i>6*r)) then
			return print("IndexPlot error - invalid parameters")
		end
		-- area plot index mode ?
		if not r then
			-- area plot index 0 is a special case
			if i == 0 then
				return plot
			else
				-- which ring are we on ?

				r = math_ceil( ( math_sqrt( 12*i + 9 ) - 3 ) / 6 )

				-- determine ring plot index (substract inside area)
				i = i - ( 3 * (r-1) * r ) - 1
			end
		end

		-- determine coordinate offsets corresponding to ring index
		local dx, dy
		if i <= 2*r then
			dx = math_min( i, r )
		elseif i<= 4*r then
			dx = 3*r-i
		else
			dx = math_max( i-6*r, -r )
		end
		if i <= 3*r then
			dy = math_max( r-i, -r )
		else
			dy = math_min( i-4*r, r )
		end

		-- return plot offset from initial plot
		return Map_GetPlotXY( plot:GetX(), plot:GetY(), dx, dy )
	end
	--usage: returns number of plots in hexagonal area of specified radius, minus center plot
	function EUI.CountHexPlots( r )
		return (r+1) * r * 3
	end

	function EUI.RadiusHexArea( a )
		return ( math_sqrt( 1 + 4*a/3 ) - 1 ) / 2
	end

end

local table = {
	insert = table.insert,
	concat = table.concat,
	remove = table.remove,
	sort = table.sort,
	maxn = table.maxn,
	unpack = unpack,
	count = table.count,--Firaxis specific
	fill = table.fill,--Firaxis specific
}
function table:append( text )
	self[#self] = self[#self] .. text
end
function table:insertLocalized( ... )
	return table_insert( self, L( ... ) )
end
function table:insertIf( s )
	if s then
		return table_insert( self, s )
	end
end
function table:insertLocalizedIf( ... )
	if ... then
		return table_insert( self, L( ... ) )
	end
end
function table:insertLocalizedIfNonZero( textKey, ... )
	if ... ~= 0 then
		return table_insert( self, L( textKey, ... ) )
	end
end
function table:insertLocalizedBulletIfNonZero( a, b, ... )
	if tonumber( b ) then
		if b ~= 0 then
			return table_insert( self, "[ICON_BULLET]" .. L( a, b, ... ) )
		end
	elseif ... ~= 0 then
		return table_insert( self, a .. L( b, ... ) )
	end
end

table.__index = table
setmetatable( table, {__call = function(self, ...) return setmetatable( {...}, table ) end } )
EUI.table = table

local Color
if civ5_mode then
	Color = function( red, green, blue, alpha )
		return {x = red or 0, y = green or 0, z = blue or 0, w = alpha or 1}
	end
else
	local function byte(n)
		return (n>=1 and 255) or (n<=0 and 0) or math_floor(n * 255)
	end
	Color = function( red, green, blue, alpha )
		return byte(red or 0) + byte(green or 0)*0x100 + byte(blue or 0)*0x10000 + byte(alpha or 1)*0x1000000
	end
end
EUI.Color = Color

function EUI.Capitalize( s )
	return  Locale.ToUpper( s:sub(1,1) ) .. Locale.ToLower( s:sub(2) )
end

local nilFunction = function() end
local nilInfoCache = setmetatable( {}, { __call = function() return nilFunction end } )

local GameInfoIndex = function( t, tableName )
	local thisGameInfoTable = GameInfo[ tableName ]
--[[
pre-game:
__call = function() DB_Query("SELECT * from "..tableName)
__index = function( t, key ): if tonumber(key) DB_Query("SELECT * from "..tableName.." where ID = ?", key) ID = key or Type = key...
--]]
	local cache, cacheMT
	if thisGameInfoTable then
		local keys = {}
--print( "Caching GameInfo table", tableName )
		for row in DB_Query( "PRAGMA table_info("..tableName..")" ) do
			-- assumes row.name is always a string
			keys[ row.name ] = true
		end
--for k in pairs( keys ) do print( k ) end
		local setMT
		setMT = { __index = function( set, key )
			if keys[ key ] then -- verify key is actually valid (and must be a string)
--print("Creating subset for key", key )
				local index = {}
				set[ key ] = index
				for i = 1, #set do
					local row = set[i]
					local v = row[ key ]
					if v then
						local subset = index[ v ]
						if not subset then
							subset = setmetatable( {}, setMT )
							index[ v ] = subset
						end
						table_insert( subset, row )
					end
				end
				return index
			end
		end }
		local set = setmetatable( {}, setMT )
		local function iterator( t, condition )
			local subset = set
			if condition then
			-- Warning: EUI's GameInfoCache iterator only supports table conditions
				for key, value in pairs( condition ) do
					subset = (subset[ key ] or {})[ value ]
					if not subset then
						return nilFunction
					end
				end
			end
			local k = 0
			local l = #subset
			return function()
				if k < l then
					k = k+1
					return subset[ k ]
				end
			end
		end
		cacheMT = { __index = function( t, key )
--print("caching", tableName, t, key)
				if key then
					local row = thisGameInfoTable[ key ]
					if row then
						local cache = {}
						for k, v in pairs( row ) do
							cache[k] = v
						end
						t[ cache.ID or key ] = cache
						t[ cache.Type or key ] = cache
						return cache
					else
						t[ key ] = false
						print( "GameInfo."..tableName.."["..tostring(key).."] is undefined" )
					end
				end
			end, __call = function( t, condition )
--print("calling", tableName, t, condition )
				if keys.ID then
					for row in thisGameInfoTable() do
						table_insert( set, t[row.ID] )
					end
				else
					for row in thisGameInfoTable() do
						local cache = {}
						for k, v in pairs( row ) do
							cache[ k ] = v
						end
						table_insert( set, cache )
					end
				end
				cacheMT.__call = iterator
				cacheMT.__index = nil
				return iterator( t, condition )
			end
		}
		cache = setmetatable( {}, cacheMT )
	else
		print( tableName, "GameInfo table is undefined for this game version and/or mods" )
		cache = nilInfoCache
	end
	t[ tableName ] = cache
	return cache
end

local GameInfoCache = setmetatable( {}, { __index = GameInfoIndex } )
EUI.GameInfoCache = GameInfoCache
local YieldIcons = {}
EUI.YieldIcons = YieldIcons
local YieldNames = {}
EUI.YieldNames = YieldNames
local IconTextureAtlases = {}
local PrimaryColors = {}
local BackgroundColors = {}
EUI.PrimaryColors = PrimaryColors
EUI.BackgroundColors = BackgroundColors
local PlayerCivs = {}
local IsCustomColor = {}

local AtlasMT = function( t, atlasName )
	setmetatable( t, nil ) -- remove metatable to prevent recursion
--print( "caching atlasses")
	for row in GameInfo.IconTextureAtlases() do
		local atlas = t[ row.Atlas ]
		if not atlas then
			atlas = {}
			t[ row.Atlas ] = atlas
		end
		atlas[ row.IconSize ] = { row.Filename, row.IconsPerRow, row.IconsPerColumn }
	end
	return t[ atlasName ]
end

local function cleartable( t )
	for k in pairs(t) do
		t[k] = nil
	end
end

local GreatPeopleIcons = {
	SPECIALIST_CITIZEN = "[ICON_CITIZEN]",
	SPECIALIST_WRITER = "[ICON_GREAT_WRITER]",
	SPECIALIST_ARTIST = "[ICON_GREAT_ARTIST]",
	SPECIALIST_MUSICIAN = "[ICON_GREAT_MUSICIAN]",
	SPECIALIST_SCIENTIST = "[ICON_GREAT_SCIENTIST]",
	SPECIALIST_MERCHANT = "[ICON_GREAT_MERCHANT]",
	SPECIALIST_ENGINEER = "[ICON_GREAT_ENGINEER]",
	SPECIALIST_GREAT_GENERAL = "[ICON_GREAT_GENERAL]",
	SPECIALIST_GREAT_ADMIRAL = "[ICON_GREAT_ADMIRAL]",
	SPECIALIST_PROPHET = "[ICON_PROPHET]",
	UNITCLASS_WRITER = "[ICON_GREAT_WRITER]",
	UNITCLASS_ARTIST = "[ICON_GREAT_ARTIST]",
	UNITCLASS_MUSICIAN = "[ICON_GREAT_MUSICIAN]",
	UNITCLASS_SCIENTIST = "[ICON_GREAT_SCIENTIST]",
	UNITCLASS_MERCHANT = "[ICON_GREAT_MERCHANT]",
	UNITCLASS_ENGINEER = "[ICON_GREAT_ENGINEER]",
	UNITCLASS_GREAT_GENERAL = "[ICON_GREAT_GENERAL]",
	UNITCLASS_GREAT_ADMIRAL = "[ICON_GREAT_ADMIRAL]",
	UNITCLASS_PROPHET = "[ICON_PROPHET]",
}

function EUI.GreatPeopleIcon(k)
	return bnw_mode and GreatPeopleIcons[k] or "[ICON_GREAT_PEOPLE]"
end
EUI.GreatPeopleIcons = GreatPeopleIcons

function EUI.ResetCache()
	bnw_mode = civBE_mode or ContentManager_IsActive("6DA07636-4123-4018-B643-6575B4EC336B", ContentType_GAMEPLAY)
	gk_mode = bnw_mode or ContentManager_IsActive("0E3751A1-F840-4e1b-9706-519BF484E59D", ContentType_GAMEPLAY)
	setmetatable( IconTextureAtlases, { __index = AtlasMT } )
	cleartable( IconTextureAtlases )
	cleartable( GameInfoCache )
	cleartable( YieldIcons )
	cleartable( YieldNames )
	for row in Game and GameInfo.Yields() or DB_Query("SELECT * from Yields") do
		YieldIcons[row.ID or false] = row.IconString
		YieldNames[row.ID or false] = L(row.Description)
		YieldIcons[row.Type or false] = row.IconString
		YieldNames[row.Type or false] = L(row.Description)
	end
	YieldIcons.YIELD_CULTURE = YieldIcons.YIELD_CULTURE or "[ICON_CULTURE]"
	YieldNames.YIELD_CULTURE = YieldNames.YIELD_CULTURE or L"TXT_KEY_TRADE_CULTURE"
	for unit in Game and GameInfo.Units() or DB_Query("SELECT * from Units") do
		GreatPeopleIcons[unit.ID or false] = GreatPeopleIcons[unit.Class]
		GreatPeopleIcons[unit.Type or false] = GreatPeopleIcons[unit.Class]
	end

	if PreGame then
		for playerID = 0, GameDefines.MAX_CIV_PLAYERS do -- including barbs/aliens !

			local thisCivType = PreGame.GetCivilization( playerID )
			local thisCiv = GameInfo.Civilizations[ thisCivType ] or {}
			local defaultColorSet = GameInfo.PlayerColors[thisCiv.DefaultPlayerColor or -1]
			local colorSet = GameInfo.PlayerColors[ PreGame.GetCivilizationColor( playerID ) ] or defaultColorSet
			local primaryColor = colorSet and GameInfo.Colors[ colorSet.PrimaryColor or -1 ]
			local secondaryColor = colorSet and GameInfo.Colors[ colorSet.SecondaryColor or -1 ]
			if thisCivType == GameInfo.Civilizations.CIVILIZATION_MINOR.ID then
				primaryColor, secondaryColor = secondaryColor, primaryColor
				defaultColorSet = false
			end
			PlayerCivs[ playerID ] = thisCiv
			IsCustomColor[ playerID ] = colorSet ~= defaultColorSet
			PrimaryColors[ playerID ] = primaryColor and Color( primaryColor.Red, primaryColor.Green, primaryColor.Blue, primaryColor.Alpha ) or Color( 1, 1, 1, 1 )
			BackgroundColors[ playerID ] = secondaryColor and Color( secondaryColor.Red, secondaryColor.Green, secondaryColor.Blue, secondaryColor.Alpha ) or Color( 0, 0, 0, 1 )
		end
--[[
print("ResetCache")
local function zunpack( t )
	local tt = {}
	for k, v in pairs( t or {} ) do table_insert( tt, tostring(k)..":="..tostring(v) ) end
	table.sort( tt )
	return table.concat( tt, "	" )
end
for k, v in pairs( PlayerCivs ) do print( k, IsCustomColor[ k ], zunpack(v), zunpack(PrimaryColors[ k ]), zunpack(BackgroundColors[ k ]) ) end
--]]
	end
end
EUI.ResetCache()
Events.AfterModsDeactivate.Add( EUI.ResetCache )
Events.AfterModsActivate.Add( EUI.ResetCache )

---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
local function IconLookup( offset, iconSize, atlas )

	local entry = (offset or -1) >= 0 and (IconTextureAtlases[ atlas ] or {})[ iconSize ]
	if entry then
		local filename = entry[1]
		local numRows = entry[3]
		local numCols = entry[2]
		if filename and offset < numRows * numCols then
			return { x=(offset % numCols) * iconSize, y = math_floor(offset / numCols) * iconSize }, filename
		end
		print( "IconLookup error - filename:", filename, "numRows:", numRows, "numCols:", numCols )
	end
	print( "IconLookup error - icon index:", offset, "icon size:", iconSize, "atlas:", atlas )
end
EUI.IconLookup = IconLookup

---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
local function IconHookup( offset, iconSize, atlas, imageControl )

	local entry = imageControl and (offset or -1) >= 0 and (IconTextureAtlases[ atlas ] or {})[ iconSize ]
	if entry then
		local filename = entry[1]
		local numRows = entry[3]
		local numCols = entry[2]
		if filename and offset < numRows * numCols then
			imageControl:SetTextureOffsetVal( (offset % numCols) * iconSize, math_floor(offset / numCols) * iconSize )
			imageControl:SetTexture( filename )
			return true
		end
		print( "IconHookup error - filename:", filename, "numRows:", numRows, "numCols:", numCols )
	end
	print( "IconHookup error - icon index:", offset, "icon size:", iconSize, "atlas:", atlas, "image control:", imageControl and imageControl:GetID() )
end
EUI.IconHookup = IconHookup

---------------------------------------------------------------------------------------------------------
-- Art Team Color Variables
---------------------------------------------------------------------------------------------------------
local white = Color( 1, 1, 1, 1 )
local downSizes = { [80] = 64, [64] = 48, [57] = 45, [45] = 32, [32] = 24 }
local textureOffsets = civ5_mode and { [64] = 141, [48] = 77, [32] = 32, [24] = 0 } or { [64] = 200, [48] = 137, [45] = 80, [32] = 32, [24] = 0 }
local unmetCiv = { PortraitIndex = civ5_mode and 23 or 24, IconAtlas = "CIV_COLOR_ATLAS", AlphaIconAtlas = civ5_mode and "CIV_COLOR_ATLAS" or "CIV_ALPHA_ATLAS" }
---------------------------------------------------------------------------------------------------------
-- This is a special case hookup for civilization icons that will take into account
-- the fact that player colors are dynamically handed out
---------------------------------------------------------------------------------------------------------
function EUI.CivIconHookup( playerID, iconSize, iconControl, backgroundControl, shadowIconControl, alwaysUseComposite, shadowed, highlightControl )
	local thisCiv = PlayerCivs[ playerID ]
	if thisCiv then
		if alwaysUseComposite or IsCustomColor[ playerID ] or not thisCiv.IconAtlas then

			if backgroundControl then
				iconSize = downSizes[ iconSize ] or iconSize
				backgroundControl:SetTexture( "CivIconBGSizes.dds" )
				backgroundControl:SetTextureOffsetVal( textureOffsets[ iconSize ] or 0, 0 )
				backgroundControl:SetColor( BackgroundColors[ playerID ] )
				backgroundControl:SetHide( false )
			end

			if highlightControl then
				highlightControl:SetTexture( "CivIconBGSizes_Highlight.dds" )
				highlightControl:SetTextureOffsetVal( textureOffsets[ iconSize ] or 0, 0 )
				highlightControl:SetColor( PrimaryColors[ playerID ] )
				highlightControl:SetHide( false )
			end

			local textureOffset, textureAtlas = IconLookup( thisCiv.PortraitIndex, iconSize, thisCiv.AlphaIconAtlas )

			if iconControl then
				if textureAtlas then
					iconControl:SetTexture( textureAtlas )
					iconControl:SetTextureOffset( textureOffset )
					iconControl:SetColor( PrimaryColors[ playerID ] )
					iconControl:SetHide( false )
				else
					iconControl:SetHide( true )
				end
			end

			if shadowIconControl then
				if shadowed and textureAtlas then
					shadowIconControl:SetTexture( textureAtlas )
					shadowIconControl:SetTextureOffset( textureOffset )
					return shadowIconControl:SetHide( false )
				else
					return shadowIconControl:SetHide( true )
				end
			end
			return
		end
	else
		thisCiv = unmetCiv
	end
	if backgroundControl then
		if iconControl then
			iconControl:SetHide( true )
		end
		IconHookup( thisCiv.PortraitIndex, iconSize, thisCiv.IconAtlas, backgroundControl )
		backgroundControl:SetColor( white )
		backgroundControl:SetHide( false )
	elseif iconControl then
		IconHookup( thisCiv.PortraitIndex, iconSize, thisCiv.IconAtlas, iconControl )
		iconControl:SetColor( white )
		iconControl:SetHide( false )
	end
	if shadowIconControl then
		shadowIconControl:SetHide( true )
	end
	if highlightControl then
		highlightControl:SetHide( true )
	end
end

---------------------------------------------------------------------------------------------------------
-- This is a special case hookup for civilization icons that always uses the one-piece version
---------------------------------------------------------------------------------------------------------
function EUI.SimpleCivIconHookup( playerID, iconSize, iconControl )
	local thisCiv = PlayerCivs[ playerID ] or unmetCiv
	return IconHookup( thisCiv.PortraitIndex, iconSize, thisCiv.IconAtlas, iconControl )
end

-- TODO: player colors
if not EUI then
EUI = {}
local civ5_mode = InStrategicView ~= nil

local math = math
--local os = os
local next = next
local pairs = pairs
--local ipairs = ipairs
--local pcall = pcall
--local print = print
--local select = select
--local string = string
local table = table
local tonumber = tonumber
--local tostring = tostring
local type = type
--local unpack = unpack or table.unpack

local UI = UI
--local UIManager = UIManager
--local Controls = Controls
--local ContextPtr = ContextPtr
--local Players = Players
--local Teams = Teams
local GameInfo = GameInfo
--local GameInfoActions = GameInfoActions
--local GameInfoTypes = GameInfoTypes
local GameDefines = GameDefines
--local InterfaceDirtyBits = InterfaceDirtyBits
--local CityUpdateTypes = CityUpdateTypes
--local ButtonPopupTypes = ButtonPopupTypes
--local YieldTypes = YieldTypes
--local GameOptionTypes = GameOptionTypes
--local DomainTypes = DomainTypes
--local FeatureTypes = FeatureTypes
--local FogOfWarModeTypes = FogOfWarModeTypes
--local OrderTypes = OrderTypes
--local PlotTypes = PlotTypes
--local TerrainTypes = TerrainTypes
--local InterfaceModeTypes = InterfaceModeTypes
--local NotificationTypes = NotificationTypes
--local ActivityTypes = ActivityTypes
--local MissionTypes = MissionTypes
--local ActionSubTypes = ActionSubTypes
--local GameMessageTypes = GameMessageTypes
--local TaskTypes = TaskTypes
--local CommandTypes = CommandTypes
--local DirectionTypes = DirectionTypes
--local DiploUIStateTypes = DiploUIStateTypes
--local FlowDirectionTypes = FlowDirectionTypes
--local PolicyBranchTypes = PolicyBranchTypes
--local FromUIDiploEventTypes = FromUIDiploEventTypes
--local CoopWarStates = CoopWarStates
--local ThreatTypes = ThreatTypes
--local DisputeLevelTypes = DisputeLevelTypes
--local LeaderheadAnimationTypes = LeaderheadAnimationTypes
local TradeableItems = TradeableItems
--local EndTurnBlockingTypes = EndTurnBlockingTypes
--local ResourceUsageTypes = ResourceUsageTypes
--local MajorCivApproachTypes = MajorCivApproachTypes
--local MinorCivTraitTypes = MinorCivTraitTypes
--local MinorCivPersonalityTypes = MinorCivPersonalityTypes
--local MinorCivQuestTypes = MinorCivQuestTypes
--local CityAIFocusTypes = CityAIFocusTypes
--local AdvisorTypes = AdvisorTypes
--local GenericWorldAnchorTypes = GenericWorldAnchorTypes
--local GameStates = GameStates
--local GameplayGameStateTypes = GameplayGameStateTypes
--local CombatPredictionTypes = CombatPredictionTypes
--local ChatTargetTypes = ChatTargetTypes
--local ReligionTypes = ReligionTypes
--local BeliefTypes = BeliefTypes
--local FaithPurchaseTypes = FaithPurchaseTypes
--local ResolutionDecisionTypes = ResolutionDecisionTypes
--local InfluenceLevelTypes = InfluenceLevelTypes
--local InfluenceLevelTrend = InfluenceLevelTrend
--local PublicOpinionTypes = PublicOpinionTypes
--local ControlTypes = ControlTypes

local PreGame = PreGame
local Game = Game
local Map = Map
--local OptionsManager = OptionsManager
--local Events = Events
--local Mouse = Mouse
--local MouseEvents = MouseEvents
--local MouseOverStrategicViewResource = MouseOverStrategicViewResource
local Locale = Locale
local L = Locale.ConvertTextKey
local table_insert = table.insert
--getmetatable("").__index.L = L

if Game and PreGame then
	local gk_mode = Game.GetReligionName ~= nil
	local bnw_mode = Game.GetActiveLeague ~= nil

	local g_savedDealStack = {}
	local g_deal = UI.GetScratchDeal()

	function EUI.PushScratchDeal()
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

	function EUI.PopScratchDeal()
		-- restore saved deal
		g_deal:ClearItems()
		local deal = table.remove( g_savedDealStack )
		for k,v in pairs( deal[1] or {} ) do
			local f = g_deal[ k ]
			if f then
				f( g_deal, v )
			end
		end

		for i = 2, #deal do
			local item = deal[ i ]
			local id = item[1]
			local fromPlayerID = item[#item]
			if id == TradeableItems.TRADE_ITEM_MAPS then
				g_deal:AddMapTrade( fromPlayerID )
			elseif id == TradeableItems.TRADE_ITEM_RESOURCES then
				g_deal:AddResourceTrade( fromPlayerID, item[4], item[5], item[2] )
			elseif id == TradeableItems.TRADE_ITEM_CITIES then
				local plot = Map.GetPlot( item[4], item[5] )
				local city = plot and plot:GetPlotCity()
				if city and city:GetOwner() == fromPlayerID then
					g_deal:AddCityTrade( fromPlayerID, city:GetID() )
				else
					print( "Error restore city deal", city and city:GetName(), unpack(item) )
				end
			elseif id == TradeableItems.TRADE_ITEM_UNITS then
				g_deal:AddUnitTrade( fromPlayerID, item[4] )
			elseif id == TradeableItems.TRADE_ITEM_OPEN_BORDERS then
				g_deal:AddOpenBorders( fromPlayerID, item[2] )
			elseif id == TradeableItems.TRADE_ITEM_TRADE_AGREEMENT then
				g_deal:AddTradeAgreement( fromPlayerID, item[2] )
			elseif id == TradeableItems.TRADE_ITEM_PERMANENT_ALLIANCE then
				print( "Error - alliance not supported by game DLL")--g_deal:AddPermamentAlliance()
			elseif id == TradeableItems.TRADE_ITEM_SURRENDER then
				g_deal:AddSurrender( fromPlayerID )
			elseif id == TradeableItems.TRADE_ITEM_TRUCE then
				print( "Error - truce not supported by game DLL")--g_deal:AddTruce()
			elseif id == TradeableItems.TRADE_ITEM_PEACE_TREATY then
				g_deal:AddPeaceTreaty( fromPlayerID, item[2] )
			elseif id == TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE then
				g_deal:AddThirdPartyPeace( fromPlayerID, item[4], item[2] )
			elseif id == TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR then
				g_deal:AddThirdPartyWar( fromPlayerID, item[4] )
			elseif id == TradeableItems.TRADE_ITEM_THIRD_PARTY_EMBARGO then
				g_deal:AddThirdPartyEmbargo( fromPlayerID, item[4], item[2] )
			elseif civ5_mode then
				if id == TradeableItems.TRADE_ITEM_GOLD then
					g_deal:AddGoldTrade( fromPlayerID, item[4] )
				elseif id == TradeableItems.TRADE_ITEM_GOLD_PER_TURN then
					g_deal:AddGoldPerTurnTrade( fromPlayerID, item[4], item[2] )
				elseif id == TradeableItems.TRADE_ITEM_DEFENSIVE_PACT then
					g_deal:AddDefensivePact( fromPlayerID, item[2] )
				elseif id == TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT then
					g_deal:AddResearchAgreement( fromPlayerID, item[2] )
				elseif gk_mode and id == TradeableItems.TRADE_ITEM_ALLOW_EMBASSY then
					g_deal:AddAllowEmbassy( fromPlayerID )
				elseif gk_mode and id == TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP then
					g_deal:AddDeclarationOfFriendship( fromPlayerID )
				elseif bnw_mode and id == TradeableItems.TRADE_ITEM_VOTE_COMMITMENT then
					g_deal:AddVoteCommitment( fromPlayerID, item[4], item[5], item[6], item[7] )
				end
			else -- civ be
				if id == TradeableItems.TRADE_ITEM_ENERGY then
					g_deal:AddGoldTrade( fromPlayerID, item[4] )
				elseif id == TradeableItems.TRADE_ITEM_ENERGY_PER_TURN then
					g_deal:AddGoldPerTurnTrade( fromPlayerID, item[4], item[2] )
				elseif id == TradeableItems.TRADE_ITEM_ALLIANCE then
					g_deal:AddAlliance( fromPlayerID, item[2] )
				elseif id == TradeableItems.TRADE_ITEM_COOPERATION_AGREEMENT then
					g_deal:AddCooperationAgreement( fromPlayerID )
				elseif id == TradeableItems.TRADE_ITEM_FAVOR then
					g_deal:AddFavorTrade( fromPlayerID, item[4] )
				elseif id == TradeableItems.TRADE_ITEM_RESEARCH_PER_TURN then
					g_deal:AddResearchPerTurnTrade( fromPlayerID, item[4], item[2] )
				end
			end
		end
--print( "Restored deal#", #g_savedDealStack ) g_deal:ResetIterator() repeat local item = { g_deal:GetNextItem() } print( unpack(item) ) until #item < 1
	end

	local g_maximumAcquirePlotArea = (GameDefines.MAXIMUM_ACQUIRE_PLOT_DISTANCE+1) * GameDefines.MAXIMUM_ACQUIRE_PLOT_DISTANCE * 3
	local g_maximumAcquirePlotPerimeter = GameDefines.MAXIMUM_ACQUIRE_PLOT_DISTANCE * 6

	if Map.GetPlotByIndex(0).GetCityPurchaseID then
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
end

local table = {
	insert = table_insert,
	concat = table.concat,
	remove = table.remove,
	sort = table.sort,
	maxn = table.maxn,
	unpack = unpack or table.unpack,-- depends on Lua version
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
	local floor = math.floor
	local function byte(n)
		return (n>=1 and 255) or (n<=0 and 0) or floor(n * 255)
	end
	Color = function( red, green, blue, alpha )
		return byte(red or 0) + byte(green or 0)*0x100 + byte(blue or 0)*0x10000 + byte(alpha or 1)*0x1000000
	end
end
EUI.Color = Color

function EUI.InstanceStackManager( instanceName, rootControlName, parentControl, collapseControl, collapseFunction, collapseControlAlwaysShown, isCollapsed )
	local n = 0
	local m = 0
	local self
	local function Collapse( value )
		if value ~= nil then
			isCollapsed = value
			parentControl:SetHide( value )
			collapseControl:SetText( value and "[ICON_PLUS]" or "[ICON_MINUS]" )
			if collapseFunction then
				collapseFunction( self, isCollapsed )
			end
		end
		return isCollapsed
	end
	self = {
		Count = function()
			return n
		end,
		ResetInstances = function()
			n = 0
		end,
		GetInstance = function()
			n = n + 1
			if n > #self then
				local instance = {}
				ContextPtr:BuildInstanceForControl( instanceName, instance, parentControl )
				--else ContextPtr:BuildInstance( instanceName, instance )
				self[n] = instance
				return instance, true
			else
				return self[n]
			end
		end,
		Commit = function()
			for i = m + 1, n do
				self[i][rootControlName]:SetHide(false)
			end
			for i = n + 1, m do
				self[i][rootControlName]:SetHide(true)
			end
			m = n
			if collapseControl then
				parentControl:CalculateSize()
				if not collapseControlAlwaysShown then
					collapseControl:SetHide( n <= 0 )
				end
			end
		end,
	}
	if collapseControl then
		self.Collapse = Collapse
		collapseControl:GetTextControl():SetAnchor( "L,C" )
		collapseControl:GetTextControl():SetOffsetVal( 12, -2 )
		collapseControl:RegisterCallback( Mouse.eLClick,
		function()
			Collapse( not isCollapsed )
		end)
		Collapse( isCollapsed or false )
	end
	return self
end

function EUI.Capitalize( s )
	return  Locale.ToUpper( s:sub(1,1) ) .. Locale.ToLower( s:sub(2) )
end

--usage1: IndexPlot( plot, hexAreaPlotIndex )
--OR usage2: IndexPlot( plot, ringPlotIndex, ringDistanceFromPlot )
function EUI.IndexPlot( plot, i, r )
	-- determine if input parameters are valid - you can delete this part for a tiny performance boost
	if not plot or not i or i<0 or (r and (r<0 or i>6*r)) then
		print("IndexPlot error - invalid parameters")
		return nil
	end
	-- area plot index mode ?
	if not r then
		-- area plot index 0 is a special case
		if i == 0 then
			return plot
		else
			-- which ring are we on ?

			r = math.ceil( ( math.sqrt( 12*i + 9 ) - 3 ) / 6 )

			-- determine ring plot index (substract inside area)
			i = i - ( 3 * (r-1) * r ) - 1
		end
	end

	-- determine coordinate offsets corresponding to ring index
	local dx, dy
	if i <= 2*r then
		dx = math.min( i, r )
	elseif i<= 4*r then
		dx = 3*r-i
	else
		dx = math.max( i-6*r, -r )
	end
	if i <= 3*r then
		dy = math.max( r-i, -r )
	else
		dy = math.min( i-4*r, r )
	end

	-- return plot offset from initial plot
	return Map.GetPlotXY( plot:GetX(), plot:GetY(), dx, dy )
end
--usage: returns number of plots in hexagonal area of specified radius, minus center plot
function EUI.CountHexPlots( r )
	return (r+1) * r * 3
end

function EUI.RadiusHexArea( a )
	return ( math.sqrt( 1 + 4*a/3 ) - 1 ) / 2
end

local nilFunction = function() end

local GameInfoIndex = function( t, tableName )
	local thisGameInfoTable = GameInfo[ tableName ]
	if thisGameInfoTable then
		local keys = {}
--print( "Caching GameInfo table", tableName )
		for row in DB.Query( "PRAGMA table_info("..tableName..")" ) do
			keys[ row.name ] = true
		end
--for k in pairs( keys ) do print( k ) end
		local setMT
		setMT = { __index = function( set, key )
			if keys[key] then -- verify key is actually valid
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
		local cacheMT
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
		local cache = setmetatable( {}, cacheMT )
		t[ tableName ] = cache
		return cache
	end
end

local GameInfoCache = setmetatable( {}, { __index = GameInfoIndex } )
EUI.GameInfoCache = GameInfoCache
EUI.YieldIcons = {}
EUI.YieldNames = {}
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
			t[row.Atlas] = atlas
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

function EUI.ResetCache()
	setmetatable( IconTextureAtlases, { __index = AtlasMT } )
	cleartable( IconTextureAtlases )
	cleartable( GameInfoCache )
	cleartable( EUI.YieldIcons )
	cleartable( EUI.YieldNames )
	for row in GameInfo.Yields() do
		EUI.YieldIcons[row.ID or -1] = row.IconString
		EUI.YieldNames[row.ID or -1] = L(row.Description)
		EUI.YieldIcons[row.Type or -1] = row.IconString
		EUI.YieldNames[row.Type or -1] = L(row.Description)
	end
	EUI.YieldIcons.YIELD_CULTURE = EUI.YieldIcons.YIELD_CULTURE or "[ICON_CULTURE]"
	EUI.YieldNames.YIELD_CULTURE = EUI.YieldNames.YIELD_CULTURE or L"TXT_KEY_TRADE_CULTURE"

	if PreGame then
		for playerID = 0, GameDefines.MAX_CIV_PLAYERS do -- including barbs/aliens !

			local thisCivType = PreGame.GetCivilization( playerID )
			local thisCiv = GameInfoCache.Civilizations[ thisCivType ] or {}
			local defaultColorSet = GameInfo.PlayerColors[thisCiv.DefaultPlayerColor or -1]
			local colorSet = GameInfo.PlayerColors[ PreGame.GetCivilizationColor( playerID ) ] or defaultColorSet
			local primaryColor = colorSet and GameInfo.Colors[ colorSet.PrimaryColor or -1 ]
			local secondaryColor = colorSet and GameInfo.Colors[ colorSet.SecondaryColor or -1 ]
			if thisCivType == GameInfo.Civilizations.CIVILIZATION_MINOR.ID then
				primaryColor, secondaryColor = secondaryColor, primaryColor
			end
			PlayerCivs[ playerID ] = thisCiv
			IsCustomColor[ playerID ] = colorSet ~= defaultColorSet
			PrimaryColors[ playerID ] = primaryColor and Color( primaryColor.Red, primaryColor.Green, primaryColor.Blue, primaryColor.Alpha ) or Color( 1, 1, 1, 1 )
			BackgroundColors[ playerID ] = secondaryColor and Color( secondaryColor.Red, secondaryColor.Green, secondaryColor.Blue, secondaryColor.Alpha ) or Color( 0, 0, 0, 1 )
		end
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
			return { x=(offset % numCols) * iconSize, y = math.floor(offset / numCols) * iconSize }, filename
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
			imageControl:SetTextureOffsetVal( (offset % numCols) * iconSize, math.floor(offset / numCols) * iconSize )
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
end

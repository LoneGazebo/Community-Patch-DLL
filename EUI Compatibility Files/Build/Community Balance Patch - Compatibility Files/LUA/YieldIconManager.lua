-------------------------------------------------
-- Yield Icon Manager
-- re-written by bc1, compatible with all Civ V and BE versions
-- fix quantity erratic display & make code much more efficient
-- add compatibility with "DLL - Various Mod Components" v63
-------------------------------------------------

-- Minor optimizations
local MapGetPlot = Map.GetPlot
local TableInsert = table.insert
local TableRemove = table.remove
local ControlsYieldStore = Controls.YieldStore
local ControlsScrap = Controls.Scrap
local ContextPtr = ContextPtr

-- Globals
local g_AnchorCache = {}
local g_AvailableAnchors = {}
local g_AvailableImageInstances = {}
local g_maxYieldID = ((YieldTypes or {}).NUM_YIELD_TYPES or 5) -1
local g_activeTeamID = Game.GetActiveTeam()
local g_isVanillaDLL = MapGetPlot(0,0).GetCulture
local g_defaultTexture = "YieldAtlas.dds"
local g_Textures = { [0]=g_defaultTexture, g_defaultTexture, g_defaultTexture, g_defaultTexture, "YieldAtlas_128_Culture.dds", "YieldAtlas_128_Faith.dds", "YieldAtlas_128_Tourism.dds", "YieldAtlas_128_GoldenAge.dds" }
local g_OffsetXs = { [0]=0, 128 , 256, 384, 0, 0, 0, 0, 0 }
-- compatibility with "DLL - Various Mod Components" extra yields
for row in GameInfo.Yields() do
	for k,v in pairs(row) do
		if k == "ImageTexture" then
			g_Textures[row.ID] = v
		elseif k == "ImageOffset" then
			g_OffsetXs[row.ID] = v
		end
	end
end

------------------------------------------------------------------
-- Update the controls to reflect the current known yield
------------------------------------------------------------------
local function BuildAnchorYields( plot, anchor )

	-- calculate the yields
	local yieldInfo = {}
	for yieldID = 0, g_maxYieldID do
		local amount = plot:CalculateYield( yieldID, true ) --* 0 + math.random( 0, 3 ) * math.random( 0, 4 )
		if amount > 0 then
			TableInsert( yieldInfo, { yieldID, amount } )
		end
	end
	if g_isVanillaDLL then
		amount = plot:GetCulture()
		if amount > 0 then
			TableInsert( yieldInfo, { 4, amount } )
		end
	end
	local n = #yieldInfo
	if n > 0 then
		local m = n<5 and 4 or (n+1)/2
		for i = 1, n do
			local yieldID = yieldInfo[i][1]
			local amount = yieldInfo[i][2]

			local iconInstance = TableRemove( g_AvailableImageInstances )
			if iconInstance then
				iconInstance.Image:ChangeParent( i <= m and anchor.Stack1 or anchor.Stack2 )
			else
				iconInstance = {}
				ContextPtr:BuildInstanceForControl( "ImageInstance", iconInstance, i <= m and anchor.Stack1 or anchor.Stack2 )
			end
			TableInsert( anchor, iconInstance )

			iconInstance.Image:SetTexture( g_Textures[ yieldID ] or g_defaultTexture )
			iconInstance.Image:SetTextureOffsetVal( g_OffsetXs[ yieldID ] or 0, amount>4 and 512 or 128 * (amount - 1) )

			if amount > 5 then
				local textInstance = TableRemove( g_AvailableImageInstances )
				if textInstance then
					textInstance.Image:ChangeParent( iconInstance.Image )
				else
					textInstance = {}
					ContextPtr:BuildInstanceForControl( "ImageInstance", textInstance, iconInstance.Image )
				end
				TableInsert( anchor, textInstance )
				textInstance.Image:SetTexture( g_defaultTexture )
				textInstance.Image:SetTextureOffsetVal( amount > 12 and 256 or 128 * ((amount - 6) % 4), amount > 9  and 768 or 640 )
			end
		end

		anchor.Stack1:CalculateSize()
		anchor.Stack2:CalculateSize()
		anchor.Stack:ReprocessAnchoring()
	end
end

------------------------------------------------------------------
-- Update the controls to remove yield display
------------------------------------------------------------------
local function RecycleAnchorYields( anchor )

	for i = 1, #anchor do
		local instance = TableRemove( anchor )
		instance.Image:ChangeParent( ControlsScrap )
		TableInsert( g_AvailableImageInstances, instance )
	end
end

------------------------------------------------------------------
-- Plot Yield Show / Hide
------------------------------------------------------------------
Events.ShowHexYield.Add(
function( x, y, isShown )

	local plot = MapGetPlot( x, y )
	if plot and plot:IsRevealed( g_activeTeamID, false ) then

		local index = plot:GetPlotIndex()
		local anchor = g_AnchorCache[ index ]

		if isShown then
			if anchor then
				-- just show it
				anchor.Anchor:ChangeParent( ControlsYieldStore )
			else
				-- create new anchor
				anchor = TableRemove( g_AvailableAnchors )
				if anchor then
					anchor.Anchor:ChangeParent( ControlsYieldStore )
				else
					anchor = {}
					ContextPtr:BuildInstanceForControl( "AnchorInstance", anchor, ControlsYieldStore )
				end
				g_AnchorCache[ index ] = anchor
				local a,b,c= GridToWorld( x, y )
				anchor.Anchor:SetWorldPositionVal( a,b,c ) -- compatibility with BE: SetHexPosition deprecated
				-- add yields
				BuildAnchorYields( plot, anchor )
			end
			anchor[ 0 ] = true -- visible

		elseif anchor then
			-- just hide it
			anchor.Anchor:ChangeParent( ControlsScrap )
			anchor[ 0 ] = false -- not visible
		end
	end

end)

------------------------------------------------------------------
-- Plot Yield Update
------------------------------------------------------------------
Events.HexYieldMightHaveChanged.Add(
function( x, y )

	local plot = MapGetPlot( x, y )
	if plot and plot:IsRevealed( g_activeTeamID, false ) then
		local index = plot:GetPlotIndex()
		local anchor = g_AnchorCache[ index ]
		if anchor then
			RecycleAnchorYields( anchor )
			-- visible ?
			if anchor[ 0 ] then
				BuildAnchorYields( plot, anchor )
			else
				-- recycle anchor
				TableInsert( g_AvailableAnchors, anchor )
				g_AnchorCache[ index ] = nil
			end
		end
	end
end)

------------------------------------------------------------------
-- 'Active' (local human) player has changed
------------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(
function( activePlayerID, prevActivePlayerID )

	g_activeTeamID = Game.GetActiveTeam()

	for index, anchor in pairs( g_AnchorCache ) do
		RecycleAnchorYields( anchor )
		-- recycle anchor
		anchor.Anchor:ChangeParent( ControlsScrap )
		TableInsert( g_AvailableAnchors, anchor )
   	end

	-- zap anchor cache
	g_AnchorCache = {}

	UI.RefreshYieldVisibleMode()
end)

-- Bit of a hack here, we want to ensure that the yield icons are properly refreshed
-- when starting a new game.
UI.RefreshYieldVisibleMode()
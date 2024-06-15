--==========================================================
-- UnitFlagManager
-- Known bugs:
-- destroy: check fix for need to update plot & cargo & airbase
-- subs: check visibility fix
-- flag offsets vs UI precedence
--==========================================================

local math_max = math.max
local math_min = math.min
local math_ceil = math.ceil
local math_pi = math.pi
local math_cos = math.cos
local math_sin = math.sin
local pairs = pairs
local string_format = string.format
local table_insert = table.insert
local table_remove = table.remove
local type = type

include( "EUI_utilities" )
local IconLookup = EUI.IconLookup
local IconHookup = EUI.IconHookup
local CivIconHookup = EUI.CivIconHookup
local Color = EUI.Color
local GameInfo = EUI.GameInfoCache
local PrimaryColors = EUI.PrimaryColors
local BackgroundColors = EUI.BackgroundColors
include( "EUI_unit_include" )
local ShortUnitTip = EUI.ShortUnitTip

local ContextPtr = ContextPtr
local DomainTypes_DOMAIN_AIR = DomainTypes.DOMAIN_AIR
local Events = Events
local Game = Game
local GridToWorld = GridToWorld
local InStrategicView = InStrategicView
local Locale = Locale
local Map_GetPlot = Map.GetPlot
local Map_GetPlotByIndex = Map.GetPlotByIndex
local Mouse = Mouse
local Players = Players
local PreGame = PreGame
local ReligionTypes = ReligionTypes
local ToGridFromHex = ToGridFromHex
local UI_GetUnitFlagIcon = UI.GetUnitFlagIcon
local UI_GetUnitPortraitIcon = UI.GetUnitPortraitIcon
local UnitMoving = UnitMoving

local GetPlotNumUnits = Map_GetPlot(0,0).GetNumLayerUnits or Map_GetPlot(0,0).GetNumUnits
local GetPlotUnit = Map_GetPlot(0,0).GetLayerUnit or Map_GetPlot(0,0).GetUnit

local g_ScrapControls = Controls.AirCraftFlags --!!! TODO
local g_AirCraftFlags = Controls.AirCraftFlags
local g_CivilianFlags = Controls.CivilianFlags
local g_MilitaryFlags = Controls.MilitaryFlags
local g_GarrisonFlags = Controls.GarrisonFlags
local g_AirbaseControls = Controls.AirbaseFlags
local g_SelectedFlags = Controls.SelectedFlags
g_SelectedFlags:ChangeParent( ContextPtr:LookUpControl( "../SelectedUnitContainer" ) or ContextPtr )

local g_toolTipControls = {}
TTManager:GetTypeControlTable( "EUI_UnitTooltip", g_toolTipControls )

local g_SelectedFlag
local g_SelectedCargo = {}
local g_AirbaseFlags = {}
local g_UnitFlags = {}
for i = -1, #Players do
	g_UnitFlags[i]={}
end
local g_spareNewUnitFlags = {}
local g_spareAirbaseFlags = {}
local g_usedPromotions = {}
local g_sparePromotions = {}

local g_activePlayerID = Game.GetActivePlayer()
local g_activeTeamID = Game.GetActiveTeam()
local g_activePlayer = Players[ g_activePlayerID ]
local g_activeTeam = Teams[ g_activeTeamID ]

local g_colorGreen = Color( 0, 1, 0, 1 )
local g_colorYellow = Color( 1, 1, 0, 1 )
local g_colorRed = Color( 1, 0, 0, 1 )
local g_colorWhite = Color( 1, 1, 1, 1 )

local DebugPrint = print

local function DebugUnit( playerID, unitID, ... )
	local player = Players[ playerID ]
	local unit = player and player:GetUnitByID( unitID )
	DebugPrint( "Player#", playerID, "unit#", unitID, unit and Locale.Lookup(unit:GetNameKey()), "unitXY=", unit and unit:GetX(), unit and unit:GetY(), ... )
end
local function DebugFlag( flag, ... )
	if type(flag)=="table" then
		DebugUnit( flag.m_PlayerID, flag.m_UnitID, "flagXY=", flag.m_Plot and flag.m_Plot:GetX(), flag.m_Plot and flag.m_Plot:GetY(), ... )
	else
		DebugPrint( "flag=", flag, ... )
	end
end

--==========================================================
-- Manage flag interractions with user
--==========================================================
local function UnitFlagClicked( playerID, unitID )
	Events.SerialEventUnitFlagSelected( playerID, unitID )
end

local function CargoFlagClicked( playerID, unitID )
	local player = Players[ playerID ]
	local unit = player and player:GetUnitByID( unitID )
	if unit then
		local plot = unit:GetPlot()
		if plot then
			for i = 0, GetPlotNumUnits( plot ) - 1 do
				local cargoUnit = GetPlotUnit( plot, i )
				if cargoUnit:GetTransportUnit() == unit then
					playerID, unitID = cargoUnit:GetOwner(), cargoUnit:GetID()
					if cargoUnit:CanMove() then break end
				end
			end
		end
	end
	Events.SerialEventUnitFlagSelected( playerID, unitID )
end

local function AirbaseFlagClicked( plotIndex )
	local plot = Map_GetPlotByIndex( plotIndex )
	if plot then
		local playerID, unitID
		for i = 0, GetPlotNumUnits( plot ) - 1 do
			local unit = GetPlotUnit( plot, i )
			if not unit:IsCargo() and unit:GetDomainType() == DomainTypes_DOMAIN_AIR then
				playerID, unitID = unit:GetOwner(), unit:GetID()
				if unit:CanMove() then break end
			end
		end
		if playerID and unitID then
			Events.SerialEventUnitFlagSelected( playerID, unitID )
		end
	end
end

local function UnitFlagToolTip( button )
	local playerID, unitID = button:GetVoid1(), button:GetVoid2()
	local player = Players[ playerID ]
	local unit = player and player:GetUnitByID( unitID )
	if unit then
		local toolTipString = ShortUnitTip( unit )
		if playerID == g_activePlayerID then
			toolTipString = toolTipString .. Locale.ConvertTextKey( "TXT_KEY_UPANEL_CLICK_TO_SELECT" )
		end
		g_toolTipControls.Text:SetText( toolTipString )

		local iconIndex, iconAtlas = UI_GetUnitPortraitIcon( unit )
		IconHookup( iconIndex, 128, iconAtlas, g_toolTipControls.UnitPortrait )
 		CivIconHookup( playerID, 64, g_toolTipControls.CivIcon, g_toolTipControls.CivIconBG, g_toolTipControls.CivIconShadow, false, true )
		local i = 0
		if not( unit.IsTrade and unit:IsTrade() ) then
			for unitPromotion in GameInfo.UnitPromotions() do
				if unit:IsHasPromotion(unitPromotion.ID) and unitPromotion.ShowInUnitPanel ~= false then
					i = i + 1
					local instance = g_usedPromotions[i]
					if not instance then
						instance = table_remove( g_sparePromotions )
						if instance then
							instance.Promotion:ChangeParent( g_toolTipControls.Stack )
						else
							instance = {}
							ContextPtr:BuildInstanceForControl( "Promotion", instance, g_toolTipControls.Stack )
						end
						g_usedPromotions[i] = instance
					end
					IconHookup( unitPromotion.PortraitIndex, 32, unitPromotion.IconAtlas, instance.Promotion )
				end
			end
		end
		for j = i+1, #g_usedPromotions do
			table_insert( g_sparePromotions, g_usedPromotions[j] )
			g_usedPromotions[j].Promotion:ChangeParent( g_ScrapControls )
			g_usedPromotions[j] = nil
		end
		g_toolTipControls.Stack:SetWrapWidth( math_ceil( i / math_ceil( i / 10 ) ) * 26 )
		g_toolTipControls.Box:DoAutoSize()
		g_toolTipControls.Box:DoAutoSize()
	end
end

--==========================================================
local function SetFlagParent( flag )
	-- DebugFlag( flag, "SetFlagParent" ) end
	if flag.m_IsSelected then
		flag.Anchor:ChangeParent( g_SelectedFlags )
	elseif flag.m_TransportUnit or flag.m_IsAirCraft then
		flag.Anchor:ChangeParent( g_AirCraftFlags )
	elseif flag.m_IsCivilian then
		flag.Anchor:ChangeParent( g_CivilianFlags )
	elseif flag.m_IsGarrisoned then
		flag.Anchor:ChangeParent( g_GarrisonFlags )
	else
		flag.Anchor:ChangeParent( g_MilitaryFlags )
	end
end

--==========================================================
local function UpdatePlotFlags( plot )
	-- DebugPrint( "UpdatePlotFlags at plot XY=", plot:GetX(), plot:GetY() ) end
	local flags = {}
	local aflags = {}
	local unit, flag, n
	local city = plot:GetPlotCity()
	if city then
		local l, r, y = -43, 43, Game.GetActiveTeam() == city:GetTeam() and -39 or -36
		local gflags = {}
		for i = 0, GetPlotNumUnits( plot ) - 1 do
			unit = GetPlotUnit( plot, i )
			flag = g_UnitFlags[ unit:GetOwner() ][ unit:GetID() ]
			if flag and flag.m_Plot then
				if unit:IsCargo() then
				elseif flag.m_IsAirCraft then
					table_insert( aflags, unit )
				elseif unit:IsGarrisoned() then
					table_insert( gflags, flag )
				else
					table_insert( flags, flag )
				end
			end
		end
		n = #flags
		-- DebugPrint( n,"flags found in city") end
		if n == 1 then
			flags[1].Container:SetOffsetVal( r, y )
		elseif n > 1 then
			local a = -math_min(35/(n-1),20)
			local b = y-a
			for i=n, 1, -1 do
				-- DebugFlag( flags[i],"update offset at position", i ) end
				flags[i].Container:SetOffsetVal( r, a*i+b )
				SetFlagParent( flags[i] )
			end
		end
		n = #gflags
		-- DebugPrint( n,"gflags found in city") end
		if n == 1 then
			gflags[1].Container:SetOffsetVal( l, y )
		elseif n > 1 then
			local a = -math_min(35/(n-1),20)
			local b = y-a
			for i=n, 1, -1 do
				-- DebugFlag( flags[i],"update offset at position", i ) end
				gflags[i].Container:SetOffsetVal( l, a*i+b )
				SetFlagParent( gflags[i] )
			end
		end
	else
		for i = 0, GetPlotNumUnits( plot ) - 1 do
			unit = GetPlotUnit( plot, i )
			flag = g_UnitFlags[ unit:GetOwner() ][ unit:GetID() ]
			-- DebugFlag( flag,"candidate is aircraft?", flag and flag.m_IsAirCraft ) end
			if flag and flag.m_Plot then
				if unit:IsCargo() then
				elseif flag.m_IsAirCraft then
					table_insert( aflags, unit )
				else
					table_insert( flags, flag )
				end
			end
		end
		n = #flags
		-- DebugPrint( n,"flags found outside city") end
		if n == 1 then
			flags[1].Container:SetOffsetVal( 0, 0 )
		elseif n > 1 then
			local a = -math_min(35/(n-1),20)
			for i=n, 1, -1 do
				-- DebugFlag( flags[i],"update offset at position", i ) end
				flags[i].Container:SetOffsetVal( 0, (i-1)*a )
				SetFlagParent( flags[i] )
			end
		end
	end
	n = #aflags
	-- DebugPrint( n,"airbase flags found") end
	local plotIndex = plot:GetPlotIndex()
	flag = g_AirbaseFlags[ plotIndex ]
	if n > 0 then
		if not flag then
			flag = table_remove( g_spareAirbaseFlags )
			if flag then
				flag.Anchor:ChangeParent( g_AirbaseControls )
			else
				flag = {}
				ContextPtr:BuildInstanceForControl( "AirbaseFlag", flag, g_AirbaseControls )
				flag.Button:RegisterCallback( Mouse.eLClick, AirbaseFlagClicked )
			end
			g_AirbaseFlags[ plotIndex ] = flag
			local x, y, z = GridToWorld( plot:GetX(), plot:GetY() )
			flag.Anchor:SetWorldPositionVal( x, y, z + 35 ) -- World Position Offset
			flag.Button:SetVoid1( plotIndex )
		end
		flag.Anchor:SetHide( not plot:IsVisible( g_activeTeamID, true ) )
		flag.Button:SetText( n )
		flag.Button:LocalizeAndSetToolTip( "TXT_KEY_STATIONED_AIRCRAFT", n )
	elseif flag then
		g_AirbaseFlags[ plotIndex ] = nil
		flag.Anchor:ChangeParent( g_ScrapControls )
		table_insert( g_spareAirbaseFlags, flag )
	end
end--UpdatePlotFlags

--==========================================================
local function SetFlagSelected( flag, isSelected )
	-- DebugFlag( flag, "SetFlagSelected, isSelected=", isSelected ) end
	flag.m_IsSelected = isSelected
	flag.FlagHighlight:SetHide( not isSelected )
	-- RestoreCargoFlagParents
	for i = 1, #g_SelectedCargo do
		SetFlagParent( g_SelectedCargo[i] )
		g_SelectedCargo[i] = nil
	end
	if isSelected then
		local selectedUnit = flag.m_Player:GetUnitByID( flag.m_UnitID )
		local plot = selectedUnit and selectedUnit:GetPlot()
		if plot then
			local transportUnit = selectedUnit:GetTransportUnit()
			local cargoUnit, cargoFlag
			if transportUnit then
				-- DebugFlag( flag, "selected unit is carge" ) end
				local x, y
				local transportFlag = g_UnitFlags[ transportUnit:GetOwner() ][ transportUnit:GetID() ]
				if transportFlag then
					x, y = transportFlag.Container:GetOffsetVal()
				else
					x, y = 0, 0
				end
				local cargo = transportUnit:GetCargo()
				local a = math_min( math_pi / 3, 5.7 / cargo )
				local a0 = -a*(cargo-1)/2 - math_pi
				local j = 0
				-- DebugFlag( transportFlag, "transport identified, cargo#=", cargo ) end
				for i = 0, GetPlotNumUnits( plot ) - 1 do
					cargoUnit = GetPlotUnit( plot, i )
					if cargoUnit:GetTransportUnit() == transportUnit then
						cargoFlag = g_UnitFlags[ cargoUnit:GetOwner() ][ cargoUnit:GetID() ]
						if cargoFlag then
							-- DebugFlag( cargoFlag, "added to cargo at position", j ) end
							table_insert( g_SelectedCargo, cargoFlag )
							cargoFlag.Container:SetOffsetVal( math_cos( a*j + a0 )*38 + x, math_sin( a*j + a0 )*38 + y )
							cargoFlag.Anchor:ChangeParent( g_SelectedFlags )
						end
						j = j + 1
					end
				end
			elseif flag.m_IsAirCraft then
				local airbase = {}
				for i = 0, GetPlotNumUnits( plot ) - 1 do
					cargoUnit = GetPlotUnit( plot, i )
					cargoFlag = g_UnitFlags[ cargoUnit:GetOwner() ][ cargoUnit:GetID() ]
					if cargoFlag and cargoFlag.m_IsAirCraft and not cargoUnit:GetTransportUnit() then
						table_insert( airbase, cargoFlag )
					end
				end
				local n=#airbase
				-- DebugPrint( n, "aircraft found") end
				if n > 0 then
					local a = math_min( 0.5, 2.7 / n )
					local a0 = -a*(n+1)/2 - math_pi/2
					for i = 1, n do
						cargoFlag = airbase[i]
						-- DebugFlag( cargoFlag, "adding aircraft to airbase at position", i ) end
						table_insert( g_SelectedCargo, cargoFlag )
						cargoFlag.Anchor:ChangeParent( g_SelectedFlags )
						cargoFlag.Container:SetOffsetVal( math_cos( a*i + a0 )*80, math_sin( a*i + a0 )*80 )
					end
				end
			end
		end
		g_SelectedFlag = flag
	elseif g_SelectedFlag == flag then
		g_SelectedFlag = nil
	end
	SetFlagParent( flag )
end--SetFlagSelected

--==========================================================
local function FinishMove( flag )
	-- DebugFlag( flag, "FinishMove" ) end
	-- Have we changed carrier ?
	local unit = flag.m_Unit
	local transportUnit = unit:GetTransportUnit()
	if flag.m_TransportUnit ~= transportUnit then
		if flag.m_TransportUnit then
			local oldCarrier = g_UnitFlags[ flag.m_TransportUnit:GetOwner() ][ flag.m_TransportUnit:GetID() ]
			if oldCarrier then
				local cargo = oldCarrier.m_Unit:GetCargo()
				oldCarrier.CargoBG:SetHide( cargo < 1 )
				oldCarrier.Cargo:SetText( cargo )
			end
		end
		flag.m_TransportUnit = transportUnit
		if transportUnit then
			local newCarrier = g_UnitFlags[ transportUnit:GetOwner() ][ transportUnit:GetID() ]
			if newCarrier then
				local cargo = transportUnit:GetCargo()
				newCarrier.CargoBG:SetHide( cargo < 1 )
				newCarrier.Cargo:SetText( cargo )
			end
		end
	end
	-- Have we changed location ?
	local oldPlot = flag.m_Plot
	local newPlot = unit:GetPlot()
	if oldPlot ~= newPlot then
		flag.m_Plot = newPlot
		if oldPlot then
			UpdatePlotFlags( oldPlot )
		end
		if newPlot then
			local x, y, z = GridToWorld( newPlot:GetX(), newPlot:GetY() )
			flag.Anchor:SetWorldPositionVal( x, y, z + 35 ) -- World Position Offset
			UpdatePlotFlags( newPlot )
		end
	end
end

--==========================================================
local function UpdateFlagType( flag )
	-- DebugFlag( flag, "UpdateFlagType" ) end

	local unit = flag.m_Unit
	local textureName, maskName

	if unit:IsEmbarked() then
		textureName = "UnitFlagEmbark.dds"
		maskName = "UnitFlagEmbarkMask.dds"
		flag.UnitIconShadow:SetOffsetVal( -1, -2 )

	elseif unit:IsGarrisoned() then
		textureName = "UnitFlagBase.dds"
		maskName = "UnitFlagMask.dds"
		flag.UnitIconShadow:SetOffsetVal( -1, 1 )

	elseif unit:GetFortifyTurns() > 0 then
		textureName = "UnitFlagFortify.dds"
		maskName = "UnitFlagFortifyMask.dds"
		flag.UnitIconShadow:SetOffsetVal( -1, 0 )

	elseif flag.m_IsTrade then
		textureName = "UnitFlagTrade.dds"
		maskName = "UnitFlagTradeMask.dds"
		flag.UnitIconShadow:SetOffsetVal( -1, 0 )

	elseif not flag.m_IsCivilian then
		textureName = "UnitFlagBase.dds"
		maskName = "UnitFlagMask.dds"
		flag.UnitIconShadow:SetOffsetVal( -1, 0 )
	else
		textureName = "UnitFlagCiv.dds"
		maskName = "UnitFlagCivMask.dds"
		flag.UnitIconShadow:SetOffsetVal( -1, -3 )
	end

	flag.UnitIconShadow:ReprocessAnchoring()

	flag.FlagShadow:SetTexture( textureName )
	flag.FlagBase:SetTexture( textureName )
	flag.FlagBaseOutline:SetTexture( textureName )
	flag.LightEffect:SetTexture( textureName )
	flag.HealthBarBG:SetTexture( textureName )
	flag.AlphaAnim:SetTexture( textureName )
	flag.FlagHighlight:SetTexture( textureName )
	flag.ScrollAnim:SetMask( maskName )
end--UpdateFlagType

--==========================================================
local function UpdateFlagHealth( flag, damage )
	-- DebugFlag( flag, "UpdateFlagHealth, damage=", damage ) end
	if damage > 0 then
		local healthPercent = 1 - damage / flag.m_MaxHitPoints
		if healthPercent > 0.66 then
			flag.HealthBar:SetFGColor( g_colorGreen )
		elseif healthPercent > 0.33 then
			flag.HealthBar:SetFGColor( g_colorYellow )
		elseif healthPercent > 0 then
			flag.HealthBar:SetFGColor( g_colorRed )
		else --unit is dead...
			healthPercent = 0
		end
		-- show the health bar
		flag.HealthBar:SetPercent( healthPercent )
		flag.HealthBarBG:SetHide( false )
		flag.HealthBar:SetHide( false )
		flag.AlphaAnim:SetTextureOffsetVal( 64, 64 )
		flag.FlagHighlight:SetTextureOffsetVal( 64, 128 )
		flag.CargoBG:SetOffsetX( 35 )
	else --full health
		-- hide the health bar
		flag.HealthBarBG:SetHide( true )
		flag.HealthBar:SetHide( true )
		flag.AlphaAnim:SetTextureOffsetVal( 0, 64 )
		flag.FlagHighlight:SetTextureOffsetVal( 0, 128 )
		flag.CargoBG:SetOffsetX( 35 )
	end
end--UpdateFlagHealth

--==========================================================
-- constructor
--==========================================================
local function CreateNewFlag( playerID, unitID, isSelected, isHiddenByFog, isInvisibleToActiveTeam )

	-- DebugUnit( playerID, unitID, "CreateNewFlag, isSelected=", isSelected, "isHiddenByFog", isHiddenByFog, "isInvisibleToActiveTeam", isInvisibleToActiveTeam ) end
	local player = Players[ playerID ]
	local unit = player and player:GetUnitByID( unitID )
	if unit and not unit:IsDead() then

		local teamID = player:GetTeam()
		local isAircraft = false
		local isCivilian = false
		local parentContainer
		if unit:IsCombatUnit() and not unit:IsEmbarked() then
			parentContainer = g_MilitaryFlags
		elseif unit:GetDomainType() == DomainTypes_DOMAIN_AIR then
			parentContainer = g_AirCraftFlags
			isAircraft = true
		else
			parentContainer = g_CivilianFlags
			isCivilian = true
		end

		local flag = table_remove( g_spareNewUnitFlags )
		if flag then
			flag.Anchor:ChangeParent( parentContainer )
			flag.FlagHighlight:SetColor( g_colorWhite )
		else
			flag = {}
			ContextPtr:BuildInstanceForControl( "UnitFlag", flag, parentContainer )
			flag.Button:RegisterCallback( Mouse.eLClick, UnitFlagClicked )
			flag.Cargo:RegisterCallback( Mouse.eLClick, CargoFlagClicked )
			flag.Button:SetToolTipCallback( UnitFlagToolTip )
		end
		---------------------------------------------------------
		-- Hook up the button
		flag.Cargo:SetVoids( playerID, unitID )
		flag.Button:SetVoids( playerID, unitID )

		---------------------------------------------------------
		-- store the flag and build the table for this player
		g_UnitFlags[ playerID ][ unitID ] = flag
		flag.m_TransportUnit = nil
		flag.m_IsAirCraft = isAircraft
		flag.m_IsCivilian = isCivilian
		flag.m_IsGarrisoned = unit:IsGarrisoned()
		flag.m_IsHiddenByFog = isHiddenByFog
		flag.m_IsInvisibleToActiveTeam = isInvisibleToActiveTeam
		flag.m_Plot = nil
		flag.m_IsSelected = isSelected
		flag.m_IsTrade = unit.IsTrade and unit:IsTrade()
		flag.m_MaxHitPoints = unit:GetMaxHitPoints()
		flag.m_Player = player
		flag.m_PlayerID = playerID
		flag.m_Unit = unit
		flag.m_UnitID = unitID

		---------------------------------------------------------
		-- Set Colors
		flag.FlagBase:SetColor( BackgroundColors[ playerID ] )
		local color = PrimaryColors[  playerID ]
		flag.UnitIcon:SetColor( color )
		flag.FlagBaseOutline:SetColor( color )

		---------------------------------------------------------
		-- Set Textures
		local flagOffset, flagAtlas = UI_GetUnitFlagIcon( unit )
		local textureOffset, textureSheet = IconLookup( flagOffset, 32, flagAtlas )
		flag.UnitIcon:SetTexture( textureSheet )
		flag.UnitIconShadow:SetTexture( textureSheet )
		flag.UnitIcon:SetTextureOffset( textureOffset )
		flag.UnitIconShadow:SetTextureOffset( textureOffset )

		---------------------------------------------------------
		-- Can carry units
		local cargo = unit:GetCargo()
		flag.CargoBG:SetHide( cargo < 1 )
		flag.Cargo:SetText( cargo )

		---------------------------------------------------------
		-- update all other info
		flag.Anchor:SetHide( isHiddenByFog or isInvisibleToActiveTeam )
		flag.FlagShadow:SetAlpha( unit:CanMove() and 1 or 0.5 )
		flag.Button:SetDisabled( g_activeTeamID ~= teamID )
		flag.Button:SetConsumeMouseOver( g_activeTeamID == teamID )
		UpdateFlagType( flag )
		UpdateFlagHealth( flag, unit:GetDamage() )
		SetFlagSelected( flag, isSelected )
		if not isSelected and g_activeTeam:IsAtWar( teamID ) then
			flag.FlagHighlight:SetHide( false )
			flag.FlagHighlight:SetColor( g_colorRed )
		end
		FinishMove( flag )

		return flag
	else
		-- DebugUnit( playerID, unitID, "is nil or dead" ) end
	end

end--CreateNewFlag

--==========================================================
local function DestroyFlag( flag )
	-- DebugFlag( flag, "DestroyFlag" ) end
	flag.Anchor:ChangeParent( g_ScrapControls )
	table_insert( g_spareNewUnitFlags, flag )
	g_UnitFlags[ flag.m_PlayerID ][ flag.m_UnitID ] = nil
	if flag.m_Plot then
		UpdatePlotFlags( flag.m_Plot )
	end
end--DestroyFlag

--==========================================================
local function ForceHide( playerID, unitID, isForceHide )
	-- DebugUnit( playerID, unitID, "ForceHide, isForceHide=", isForceHide ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		flag.Anchor:SetHide( isForceHide or flag.m_IsHiddenByFog or flag.m_IsInvisibleToActiveTeam )
	end
end--ForceHide

--==========================================================
-- On Unit Created
--==========================================================
Events.SerialEventUnitCreated.Add(
function( playerID, unitID, hexPos, unitType, cultureType, civID, primaryColor, secondaryColor, unitFlagIndex, fogState, isSelected, isMilitary, isVisible )
	-- DebugUnit( playerID, unitID, "SerialEventUnitCreated, fogState=", fogState, "isSelected=", isSelected, "isVisible=", isVisible, "XY=", ToGridFromHex( hexPos.x, hexPos.y ) ) end
	CreateNewFlag( playerID, unitID, isSelected, fogState ~= 2, not isVisible )
end)

--==========================================================
-- On Unit Position Changed
-- sent by the engine while it walks a unit around
--==========================================================
Events.LocalMachineUnitPositionChanged.Add(
function( playerID, unitID, unitPosition )
	-- DebugUnit( playerID, unitID, "LocalMachineUnitPositionChanged" ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		-- DebugFlag( flag, "Setting flag position while moving" ) end
		flag.Anchor:SetWorldPositionVal( unitPosition.x, unitPosition.y, unitPosition.z + 35 ) -- World Position Offset
		local plot = flag.m_Plot
		if plot then
			if UnitMoving( playerID, unitID ) then
				flag.m_Plot = nil
			end
			local targetPlot = flag.m_Unit:GetPlot()
			if targetPlot ~= plot then
				UpdatePlotFlags( plot )
				-- DebugFlag( flag, "starting to move: setting flag offset to", 0, 0 ) end
				return flag.Container:SetOffsetVal( 0, 0 )
			end
		end
	else
		-- DebugUnit( playerID, unitID, "not found for LocalMachineUnitPositionChanged" ) end
	end
end)

--==========================================================
-- On Flag Type Change
--==========================================================
local function OnFlagTypeChange( playerID, unitID )
	-- DebugUnit( playerID, unitID, "OnFlagTypeChange" ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		UpdateFlagType( flag )
	end
end
Events.UnitActionChanged.Add( OnFlagTypeChange )
Events.UnitEmbark.Add( OnFlagTypeChange ) -- GameplayUnitEmbark

--==========================================================
-- On Unit Move Queue Changed
--==========================================================
local function OnUnitMoveQueueChanged( playerID, unitID, hasRemainingMoves )
	-- DebugUnit( playerID, unitID, "OnUnitMoveQueueChanged, hasRemainingMoves=", hasRemainingMoves ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		return FinishMove( flag )
	end
end--OnUnitMoveQueueChanged
Events.UnitMoveQueueChanged.Add( OnUnitMoveQueueChanged )

Events.SerialEventUnitTeleportedToHex.Add(
function( hexX, hexY, playerID, unitID )
	-- DebugUnit( playerID, unitID, "SerialEventUnitTeleportedToHex, XY=", ToGridFromHex( hexX, hexY ) ) end
	-- nukes teleport instead of moving
	-- spoof out the move queue changed logic.
	OnUnitMoveQueueChanged( playerID, unitID )
end)

--==========================================================
-- On Unit Visibility Changed (GameplayUnitVisibility)
--==========================================================
Events.UnitVisibilityChanged.Add(
function( playerID, unitID, isVisible, checkFlag )--, blendTime )
	-- DebugUnit( playerID, unitID, "UnitVisibilityChanged, isVisible=", isVisible, "checkFlag=", checkFlag ) end
	if checkFlag then
		local flag = g_UnitFlags[ playerID ][ unitID ]
		if flag then
			flag.m_IsInvisibleToActiveTeam = not isVisible
			flag.Anchor:SetHide( not isVisible or flag.m_IsInvisibleToActiveTeamm_IsHiddenByFog )
		end
	end
end)

--==========================================================
-- On Unit Destroyed (GameplayUnitDestroyed)
--==========================================================
Events.SerialEventUnitDestroyed.Add(
function( playerID, unitID )
	-- DebugUnit( playerID, unitID, "SerialEventUnitDestroyed" ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		DestroyFlag( flag )
	else
		-- DebugUnit( playerID, unitID, "flag not found for SerialEventUnitDestroyed" ) end
	end
end)

--==========================================================
-- On Unit Selection Change
--==========================================================
Events.UnitSelectionChanged.Add(
function( playerID, unitID, _, _, _, isSelected )
	-- DebugUnit( playerID, unitID, "UnitSelectionChanged, isSelected=", isSelected ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		SetFlagSelected( flag, isSelected )
	else
		-- DebugUnit( playerID, unitID, "flag not found for UnitSelectionChanged", isSelected ) end
	end
end)

--==========================================================
-- GameplayUnitSetDamage is called only if
-- the amount of damage actually changed
--==========================================================
Events.SerialEventUnitSetDamage.Add(
function( playerID, unitID, damage )--, previousDamage )
	-- !!! can be called for dead unit !!!
	-- DebugUnit( playerID, unitID, "SerialEventUnitSetDamage, damage=", damage ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		UpdateFlagHealth( flag, damage )
	else
		-- DebugUnit( playerID, unitID, "flag not found for SerialEventUnitSetDamage" ) end
	end
end)

--==========================================================
-- this goes off when a hex is seen or unseen
--==========================================================
Events.HexFOWStateChanged.Add(
function( hexPos, fogState, isWholeMap )
	-- DebugPrint( "HexFOWStateChanged, fogState=", fogState, "isWholeMap=", isWholeMap, "XY=", ToGridFromHex( hexPos.x, hexPos.y ) ) end
	local isInvisible = fogState ~= 2 -- eyes on
	if isWholeMap then
		-- unit flags
		for playerID = 0, #Players do
			for unitID, flag in pairs( g_UnitFlags[ playerID ] ) do
				flag.m_IsHiddenByFog = isInvisible
				flag.Anchor:SetHide( isInvisible or flag.m_IsInvisibleToActiveTeam )
			end
		end
		-- city flags
		for plotIndex, flag in pairs( g_AirbaseFlags ) do
			flag.Anchor:SetHide( isInvisible )
		end
	else
		local plot = Map_GetPlot( ToGridFromHex( hexPos.x, hexPos.y ) )
		if plot then
			-- unit flags
			for i = 0, GetPlotNumUnits( plot ) - 1 do
				local unit = GetPlotUnit( plot, i )
				local flag = unit and g_UnitFlags[ unit:GetOwner() ][ unit:GetID() ]
				if flag then
					flag.m_IsHiddenByFog = isInvisible
					flag.Anchor:SetHide( isInvisible or flag.m_IsInvisibleToActiveTeam )
				end
			end
			-- city flag
			local flag = g_AirbaseFlags[ plot:GetPlotIndex() ]
			if flag then
				flag.Anchor:SetHide( isInvisible )
			end
		end
	end
end)

--==========================================================
-- this goes off when a unit moves into or out of the fog
--==========================================================
Events.UnitStateChangeDetected.Add(
function( playerID, unitID, fogState )
	-- DebugUnit( playerID, unitID, "UnitStateChangeDetected, fogState=", fogState ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		flag.m_IsHiddenByFog = fogState ~=2
		flag.Anchor:SetHide( fogState ~=2 or flag.m_IsInvisibleToActiveTeam )
	else
		-- DebugUnit( playerID, unitID, "flag not found for UnitStateChangeDetected", fogState ) end
	end
end)

--==========================================================
-- this goes off when GameplayUnitShouldDimFlag
-- decides a unit's flag dimming should change
--==========================================================
Events.UnitShouldDimFlag.Add(
function( playerID, unitID, isDimmed )
	-- DebugUnit( playerID, unitID, "UnitShouldDimFlag, isDimmed=", isDimmed ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		flag.FlagShadow:SetAlpha( isDimmed and 0.5 or 1.0 )
	else
		-- DebugUnit( playerID, unitID, "flag not found for UnitShouldDimFlag" ) end
	end
end)

--==========================================================
-- On Unit Garrison
--==========================================================
Events.UnitGarrison.Add(
function( playerID, unitID, isGarrisoned )
	-- DebugUnit( playerID, unitID, "UnitGarrison, isGarrisoned=", isGarrisoned ) end
	local flag = g_UnitFlags[ playerID ][ unitID ]
	if flag then
		flag.m_IsGarrisoned = isGarrisoned
		SetFlagParent( flag )
		UpdateFlagType( flag )
	end
end)

--==========================================================
-- On City Created / Destroyed / Captured
--==========================================================
local function OnCity( hexPos )
	-- DebugPrint( "SerialEventCityDestroyed or SerialEventCityCaptured, XY=", ToGridFromHex( hexPos.x, hexPos.y ) ) end
	local plot = Map_GetPlot( ToGridFromHex( hexPos.x, hexPos.y ) )
	if plot then
		UpdatePlotFlags( plot )
	end
end
Events.SerialEventCityDestroyed.Add( OnCity )
Events.SerialEventCityCaptured.Add( OnCity )
Events.SerialEventCityCreated.Add( OnCity )

Events.SerialEventEnterCityScreen.Add(
function()
	g_SelectedFlags:SetHide( true )
end)
Events.SerialEventExitCityScreen.Add(
function()
	g_SelectedFlags:SetHide( InStrategicView() )
end)

--==========================================================
Events.StrategicViewStateChanged.Add( function( isStrategicView, isCityBanners )
	-- DebugPrint( "StrategicViewStateChanged, isStrategicView=", isStrategicView, "isCityBanners=", isCityBanners ) end
	g_CivilianFlags:SetHide( isStrategicView )
	g_MilitaryFlags:SetHide( isStrategicView )
	g_GarrisonFlags:SetHide( isStrategicView and not isCityBanners )
	g_AirbaseControls:SetHide( isStrategicView )
	g_SelectedFlags:SetHide( isStrategicView )
end)

--==========================================================
-- Hide flag during combat
--==========================================================
Events.RunCombatSim.Add(
function( m_AttackerPlayerID,
		m_AttackerUnitID,
		m_AttackerUnitDamage,
		m_AttackerFinalUnitDamage,
		m_AttackerMaxHitPoints,
		m_DefenderPlayerID,
		m_DefenderUnitID,
		m_DefenderUnitDamage,
		m_DefenderFinalUnitDamage,
		m_DefenderMaxHitPoints,
		m_bContinuation )
	-- DebugUnit( m_AttackerPlayerID, m_AttackerUnitID, "RunCombatSim", m_AttackerUnitDamage, m_AttackerFinalUnitDamage, m_AttackerMaxHitPoints, m_DefenderPlayerID, m_DefenderUnitID, m_DefenderUnitDamage, m_DefenderFinalUnitDamage, m_DefenderMaxHitPoints, m_bContinuation ) end
	ForceHide( m_AttackerPlayerID, m_AttackerUnitID, true )
	ForceHide( m_DefenderPlayerID, m_DefenderUnitID, true )
end)

--==========================================================
-- Show flag after combat
--==========================================================
Events.EndCombatSim.Add(
function( m_AttackerPlayerID,
		m_AttackerUnitID,
		m_AttackerUnitDamage,
		m_AttackerFinalUnitDamage,
		m_AttackerMaxHitPoints,
		m_DefenderPlayerID,
		m_DefenderUnitID,
		m_DefenderUnitDamage,
		m_DefenderFinalUnitDamage,
		m_DefenderMaxHitPoints )
	-- DebugUnit( m_AttackerPlayerID, m_AttackerUnitID, "EndCombatSim", m_AttackerUnitDamage, m_AttackerFinalUnitDamage, m_AttackerMaxHitPoints, m_DefenderPlayerID, m_DefenderUnitID, m_DefenderUnitDamage, m_DefenderFinalUnitDamage, m_DefenderMaxHitPoints ) end
	ForceHide( m_AttackerPlayerID, m_AttackerUnitID, false )
	ForceHide( m_DefenderPlayerID, m_DefenderUnitID, false )
end)

--==========================================================
-- War Declared
--==========================================================
Events.WarStateChanged.Add(
function( teamID1, teamID2, isAtWar )
	if teamID1 == g_activeTeamID then
		teamID1 = teamID2
	elseif teamID2 ~= g_activeTeamID then
		return
	end
	local isAtPeace = not isAtWar
	for playerID = 0, #Players do
		local player = Players[playerID]
		if player and player:IsAlive() and player:GetTeam() == teamID1 then
			for unitID, flag in pairs( g_UnitFlags[playerID] ) do
				flag.FlagHighlight:SetHide( isAtPeace )
				flag.FlagHighlight:SetColor( isAtPeace and g_colorWhite or g_colorRed )
			end
		end
	end
end)

--==========================================================
-- 'Active' (local human) player has changed TODO
--==========================================================
Events.GameplaySetActivePlayer.Add( function( activePlayerID )--, iPrevActivePlayerID )
	-- DebugPrint( "GameplaySetActivePlayer, activePlayerID=", activePlayerID ) end
	g_activePlayerID = activePlayerID
	g_activeTeamID = Game.GetActiveTeam()
	g_activePlayer = Players[ activePlayerID ]
	g_activeTeam = Teams[ g_activeTeamID ]
	if g_SelectedFlag then
		SetFlagSelected( g_SelectedFlag, false )
	end
	for playerID = 0, #Players do
		local player = Players[ playerID ]
		if player and player:IsAlive() then
			local teamID = player:GetTeam()
			local isActiveTeam = teamID == g_activeTeamID
			local isAtPeace = not g_activeTeam:IsAtWar( teamID )
			for unitID, flag in pairs( g_UnitFlags[playerID] ) do
				flag.Button:SetDisabled( not isActiveTeam )
				flag.Button:SetConsumeMouseOver( isActiveTeam )
				flag.FlagHighlight:SetHide( isAtPeace )
				flag.FlagHighlight:SetColor( isAtPeace and g_colorWhite or g_colorRed )
			end
		end
	end
end)

--==========================================================
-- on shutdown, we need to get our children back,
-- or they will get duplicted on future hotload
-- and we'll assert clearing the registered button handler
--==========================================================
ContextPtr:SetShutdown( function()
	-- DebugPrint( "SetShutdown" ) end
	g_SelectedFlags:ChangeParent( ContextPtr )
end)

--==========================================================
-- scan for all units when we are loaded
-- this keeps the flags from disappearing on hotload
--==========================================================
if ContextPtr:IsHotLoad() then
	-- DebugPrint( "IsHotLoad" ) end
	-- DestroyFlag any existing unit flags
	for playerID = 0, #Players do
		-- DebugPrint( playerID, "flag count:", #g_UnitFlags )end
		for unitID, flag in pairs( g_UnitFlags[playerID] ) do
			DestroyFlag( flag )
		end
	end
	for playerID = 0, #Players do
		local player = Players[playerID]
		if player and player:IsAlive() then
			for unit in player:Units() do
				local plot = unit:GetPlot()
				CreateNewFlag( playerID, unit:GetID(), unit:IsSelected(), plot and not plot:IsVisible( g_activeTeamID, true ), unit:IsInvisible( g_activeTeamID, true ) )
			end
		end
	end
end

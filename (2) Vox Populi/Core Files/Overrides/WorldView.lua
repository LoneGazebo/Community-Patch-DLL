-- modified by bc1 from 1.0.3.144 brave new world code
-- partial fix for sub visibility
-------------------------------------------------
-- World View
-------------------------------------------------
include( "FLuaVector" );
local Vector2 = Vector2
local Vector4 = Vector4

local print = print
local type = type

local ButtonPopupTypes = ButtonPopupTypes
local CityUpdateTypes = CityUpdateTypes
local CommandTypes = CommandTypes
local ContextPtr = ContextPtr
local DomainTypes = DomainTypes
local Events = Events
local Game = Game
local GameInfo = GameInfo
local GameInfoTypes = GameInfoTypes
local GameMessageTypes = GameMessageTypes
local InStrategicView = InStrategicView
local InterfaceModeTypes = InterfaceModeTypes
local KeyEvents = KeyEvents
local Keys = Keys
local Map = Map
local MissionTypes = MissionTypes
local MouseEvents = MouseEvents
local Players = Players
local TaskTypes = TaskTypes
local ToGridFromHex = ToGridFromHex
local ToggleStrategicView = ToggleStrategicView
local UI = UI
local UIManager = UIManager

local gk_mode = Game.GetReligionName ~= nil;--bc1
local g_isSquadsModEnabled = Game.IsCustomModOption("SQUADS");
local g_isRoutePlannerModEnabled = Game.IsCustomModOption("ROUTE_PLANNER");


local turn1Color = Vector4( 0, 1, 0, 0.25 );
local rButtonDown = false;
local bEatNextUp = false;

local pathBorderStyle = "MovementRangeBorder";
local attackPathBorderStyle = "AMRBorder"; -- attack move

-- VP/bal : for ctrl + click action
local WaypointPathColor = Vector4( 1, 1, 0, 1 ); -- Yellow
local AlternativeMoveColor = Vector4( 0, 1, 0, 1 ); -- Green

local buildAcademy = GameInfoTypes["BUILD_ACADEMY"];
local buildArchaeology = GameInfoTypes["BUILD_ARCHAEOLOGY_DIG"];
local buildCitadel = GameInfoTypes["BUILD_CITADEL"];
local buildOrdo = GameInfoTypes["BUILD_MONGOLIA_ORDO"];
local buildCustomsHouse = GameInfoTypes["BUILD_CUSTOMS_HOUSE"];
local buildEmbassy = GameInfoTypes["BUILD_EMBASSY"];
local buildHolySite = GameInfoTypes["BUILD_HOLY_SITE"];
local buildManufactory = GameInfoTypes["BUILD_MANUFACTORY"];
local buildRepair = GameInfoTypes["BUILD_REPAIR"];

function UpdatePathFromSelectedUnitToMouse()
	local interfaceMode = UI.GetInterfaceMode();
	--Events.ClearHexHighlightStyle(pathBorderStyle);
	--if interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION and rButtonDown) or (interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO then
		--for i, pathNode in ipairs(g_pathFromSelectedUnitToMouse) do
			--local hexID = ToHexFromGrid( Vector2( pathNode.x, pathNode.y) );
			--if pathNode.turn == 1 then
				--Events.SerialEventHexHighlight( hexID, true, turn1Color, pathBorderStyle );
			--elseif pathNode.turn == 2 then
				--Events.SerialEventHexHighlight( hexID, true, turn2Color, pathBorderStyle );
			--else
				--Events.SerialEventHexHighlight( hexID, true, turn3PlusColor, pathBorderStyle );
			--end
		--end

		-- this doesnt work because the size of the array is irrelevant
		--if #g_pathFromSelectedUnitToMouse > 0 then
			--Events.DisplayMovementIndicator( true );
		--else
			--Events.DisplayMovementIndicator( false );
		--end
	--end

	if rButtonDown and interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION or interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO then
		Events.DisplayMovementIndicator( true );
	else
		Events.DisplayMovementIndicator( false );
	end

end

function UpdatePathFromWaypointToMouse()
    Events.ClearHexHighlights()
	local pUnit = UI.GetHeadSelectedUnit()
	local pPlot = Map.GetPlot( UI.GetMouseOverHex() )

	if (pUnit:GetX() ~= pPlot:GetX() or pUnit:GetY() ~= pPlot:GetY()) and rButtonDown then
		local path = pUnit:GeneratePathToNextWaypoint(pPlot)
		for i = 1, #path, 1 do
			local node = path[i]
--			print("i :"..tostring(i)..", X: "..tostring(node.X)..", Y: "..tostring(node.Y))
			Events.SerialEventHexHighlight(ToHexFromGrid({x = node.X, y = node.Y}), true, WaypointPathColor)
		end
--		Events.GameplayFX(hex.x, hex.y, -1);
--		Events.DisplayMovementIndicator( false );
	end
end

function ShowMovementRangeIndicator()
	local unit = UI.GetHeadSelectedUnit();
	if not unit then
		return;
	end

	local iPlayerID = Game.GetActivePlayer();

	Events.ShowMovementRange( iPlayerID, unit:GetID() );
end

-- Add any interface modes that need special processing to this table
-- (look at InGame.lua for a bigger example)
local InterfaceModeMessageHandler =
{
	[InterfaceModeTypes.INTERFACEMODE_DEBUG] = {},
	[InterfaceModeTypes.INTERFACEMODE_SELECTION] = {},
	[InterfaceModeTypes.INTERFACEMODE_PING] = {},
	[InterfaceModeTypes.INTERFACEMODE_MOVE_TO] = {},
	[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_TYPE] = {},
	[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_ALL] = {},
	[InterfaceModeTypes.INTERFACEMODE_ROUTE_TO] = {},
	[InterfaceModeTypes.INTERFACEMODE_AIRLIFT] = {},
	[InterfaceModeTypes.INTERFACEMODE_NUKE] = {},
	[InterfaceModeTypes.INTERFACEMODE_PARADROP] = {},
	[InterfaceModeTypes.INTERFACEMODE_ATTACK] = {},
	[InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK] = {},
	[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK] = {},
	[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE] = {},
	[InterfaceModeTypes.INTERFACEMODE_AIR_SWEEP] = {},
	[InterfaceModeTypes.INTERFACEMODE_REBASE] = {},
	[InterfaceModeTypes.INTERFACEMODE_EMBARK] = {},
	[InterfaceModeTypes.INTERFACEMODE_DISEMBARK] = {},
	[InterfaceModeTypes.INTERFACEMODE_PLACE_UNIT] = {},
	[InterfaceModeTypes.INTERFACEMODE_GIFT_UNIT] = {},
}

if g_isSquadsModEnabled then
  InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_UNIT_MANAGEMENT.ID] = {};
  InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_BASE_MANAGEMENT.ID] = {};
  InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_MOVEMENT.ID] = {};
end

local DefaultMessageHandler = {};


DefaultMessageHandler[KeyEvents.KeyDown] =
function( wParam )
	if wParam == Keys.VK_LEFT then
		Events.SerialEventCameraStopMovingRight();
		Events.SerialEventCameraStartMovingLeft();
		return true;
	elseif wParam == Keys.VK_RIGHT then
		Events.SerialEventCameraStopMovingLeft();
		Events.SerialEventCameraStartMovingRight();
		return true;
	elseif wParam == Keys.VK_UP then
		Events.SerialEventCameraStopMovingBack();
		Events.SerialEventCameraStartMovingForward();
		return true;
	elseif wParam == Keys.VK_DOWN then
		Events.SerialEventCameraStopMovingForward();
		Events.SerialEventCameraStartMovingBack();
		return true;
	elseif wParam == Keys.VK_NEXT or wParam == Keys.VK_OEM_MINUS then
		Events.SerialEventCameraOut( Vector2(0,0) );
		return true;
	elseif wParam == Keys.VK_PRIOR or wParam == Keys.VK_OEM_PLUS then
		Events.SerialEventCameraIn( Vector2(0,0) );
		return true;
	elseif wParam == Keys.VK_ESCAPE and InStrategicView() then
		ToggleStrategicView();
		return true;
	end

	if g_isSquadsModEnabled then
		-- Forward numeric presses to squads handler
		if wParam >= Keys["0"] and wParam <= Keys["9"] then
			LuaEvents.SQUADS_NUMERIC_HOTKEY(
				wParam - Keys["0"], UIManager:GetControl());
		end
	end
end


DefaultMessageHandler[KeyEvents.KeyUp] =
function( wParam )
	if wParam == Keys.VK_LEFT then
		Events.SerialEventCameraStopMovingLeft();
		return true;
	elseif wParam == Keys.VK_RIGHT then
		Events.SerialEventCameraStopMovingRight();
		return true;
	elseif wParam == Keys.VK_UP then
		Events.SerialEventCameraStopMovingForward();
		return true;
	elseif wParam == Keys.VK_DOWN then
		Events.SerialEventCameraStopMovingBack();
		return true;
	end
	return false;
end


-- Emergency key up handler
function KeyUpHandler( wParam )
	if wParam == Keys.VK_LEFT then
		Events.SerialEventCameraStopMovingLeft();
	elseif wParam == Keys.VK_RIGHT then
		Events.SerialEventCameraStopMovingRight();
	elseif wParam == Keys.VK_UP then
		Events.SerialEventCameraStopMovingForward();
	elseif wParam == Keys.VK_DOWN then
		Events.SerialEventCameraStopMovingBack();
	end
end
Events.KeyUpEvent.Add( KeyUpHandler );

if g_isRoutePlannerModEnabled then
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_ROUTE_PLANNER_DRAW.ID] = {}
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_ROUTE_PLANNER_DRAW.ID][MouseEvents.RButtonUp] = LuaEvents.ROUTE_PLANNER_RButtonUp
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_ROUTE_PLANNER_DRAW.ID][MouseEvents.RButtonDown] = LuaEvents.ROUTE_PLANNER_RButtonDown
end

if g_isSquadsModEnabled then
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_UNIT_MANAGEMENT.ID][MouseEvents.RButtonDown] = LuaEvents.SQUADS_MANAGE_UNITS_MODE_RIGHT_CLICK_DOWN;
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_UNIT_MANAGEMENT.ID][MouseEvents.LButtonUp] = LuaEvents.SQUADS_MANAGE_UNITS_MODE_LEFT_CLICK_UP;
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_UNIT_MANAGEMENT.ID][MouseEvents.RButtonUp] = LuaEvents.SQUADS_MANAGE_UNITS_MODE_RIGHT_CLICK_UP;

	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_BASE_MANAGEMENT.ID][MouseEvents.LButtonUp] = LuaEvents.SQUADS_MANAGE_BASE_MODE_LEFT_CLICK;
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_BASE_MANAGEMENT.ID][MouseEvents.RButtonUp] = LuaEvents.SQUADS_MANAGE_BASE_MODE_RIGHT_CLICK_UP;

	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_MOVEMENT.ID][MouseEvents.RButtonUp] = LuaEvents.SQUADS_MOVE_MODE_RIGHT_CLICK;
	InterfaceModeMessageHandler[GameInfo.InterfaceModes.INTERFACEMODE_SQUAD_MOVEMENT.ID][MouseEvents.LButtonUp] = LuaEvents.SQUADS_MOVE_MODE_LEFT_CLICK;

end
-- INTERFACEMODE_DEBUG
--local g_PlopperSettings, g_UnitPlopper, g_ResourcePlopper, g_ImprovementPlopper, g_CityPlopper

g_UnitPlopper =
{
	UnitType = -1,
	Embarked = false,

	Plop =
	function(plot)
		if g_PlopperSettings.Player ~= -1 and g_UnitPlopper.UnitType ~= -1 then
			local player = Players[g_PlopperSettings.Player];
			if player then
				local unit;

				print(g_UnitPlopper.UnitNameOffset);
				print(player.InitUnitWithNameOffset);
				if g_UnitPlopper.UnitNameOffset and player.InitUnitWithNameOffset then
					unit = player:InitUnitWithNameOffset(g_UnitPlopper.UnitType, g_UnitPlopper.UnitNameOffset, plot:GetX(), plot:GetY());
				else
					unit = player:InitUnit(g_UnitPlopper.UnitType, plot:GetX(), plot:GetY());
				end

				if g_UnitPlopper.Embarked then
					unit:Embark();
				end
			end
		end
	end,

	Deplop =
	function(plot)
		local unitCount = plot:GetNumUnits();
		for i = 0, unitCount - 1 do
			local unit = plot:GetUnit(i);
			if unit then
				unit:Kill(true, -1);
			end
		end
	end
}

g_ResourcePlopper =
{
	ResourceType = -1,
	ResourceAmount = 1,

	Plop =
	function(plot)
		if g_ResourcePlopper.ResourceType ~= -1 then
			plot:SetResourceType(g_ResourcePlopper.ResourceType, g_ResourcePlopper.ResourceAmount);
		end
	end,

	Deplop =
	function(plot)
		plot:SetResourceType(-1);
	end
}

g_ImprovementPlopper =
{
	ImprovementType = -1,
	Pillaged = false,
	HalfBuilt = false,

	Plop =
	function(plot)
		if g_ImprovementPlopper.ImprovementType ~= -1 then
			plot:SetImprovementType(g_ImprovementPlopper.ImprovementType);
			if g_ImprovementPlopper.Pillaged then
				plot:SetImprovementPillaged(true);
			end
		end
	end,

	Deplop =
	function(plot)
		plot:SetImprovementType(-1);
	end
}

g_CityPlopper =
{
	Plop =
	function(plot)
		if g_PlopperSettings.Player ~= -1 then
			local player = Players[g_PlopperSettings.Player];
			if player then
				player:InitCity(plot:GetX(), plot:GetY());
			end
		end
	end,

	Deplop =
	function(plot)
		local city = plot:GetPlotCity();
		if city then
			city:Kill();
		end
	end
}

g_PlopperSettings =
{
	Player = -1,
	Plopper = g_UnitPlopper,
	EnabledWhenInTab = false
}

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DEBUG][MouseEvents.LButtonUp] =
function()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local debugItem1 = UI.GetInterfaceModeDebugItemID1();

	-- City
	if UI.debugItem1 == 0 then
		pActivePlayer:InitCity(plotX, plotY);
	-- Unit
	elseif debugItem1 == 1 then
		local iUnitID = UI.GetInterfaceModeDebugItemID2();
		pActivePlayer:InitUnit(iUnitID, plotX, plotY);
	-- Improvement
	elseif debugItem1 == 2 then
		local iImprovementID = UI.GetInterfaceModeDebugItemID2();
		plot:SetImprovementType(iImprovementID);
	-- Route
	elseif debugItem1 == 3 then
		local iRouteID = UI.GetInterfaceModeDebugItemID2();
		plot:SetRouteType(iRouteID);
	-- Feature
	elseif debugItem1 == 4 then
		local iFeatureID = UI.GetInterfaceModeDebugItemID2();
		plot:SetFeatureType(iFeatureID);
	-- Resource
	elseif debugItem1 == 5 then
		local iResourceID = UI.GetInterfaceModeDebugItemID2();
		plot:SetResourceType(iResourceID, 5);
	-- Plopper
	elseif (debugItem1 == 6 and
			type(g_PlopperSettings) == "table" and
			type(g_PlopperSettings.Plopper) == "table" and
			type(g_PlopperSettings.Plopper.Plop) == "function") then

		g_PlopperSettings.Plopper.Plop(plot);
		return true; -- Do not allow the interface mode to be set back to INTERFACEMODE_SELECTION
	end

	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	return true;
end

----------------------------------
-- RIGHT MOUSE BUTTON
----------------------------------

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DEBUG][MouseEvents.RButtonDown] =
function()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local debugItem1 = UI.GetInterfaceModeDebugItemID1();

	-- Plopper
	if (debugItem1 == 6 and
			type(g_PlopperSettings) == "table" and
			type(g_PlopperSettings.Plopper) == "table" and
			type(g_PlopperSettings.Plopper.Deplop) == "function") then

		g_PlopperSettings.Plopper.Deplop(plot);
	end

	return true;
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DEBUG][MouseEvents.RButtonUp] =
function()
	-- Trap RButtonUp
	return true;
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK][KeyEvents.KeyDown] =
function( wParam, lParam )
	if wParam == Keys.VK_ESCAPE then
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	elseif wParam == Keys.VK_NUMPAD1 or wParam == Keys.VK_NUMPAD3 or wParam == Keys.VK_NUMPAD4 or wParam == Keys.VK_NUMPAD6 or wParam == Keys.VK_NUMPAD7 or wParam == Keys.VK_NUMPAD8 then
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	else
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	end
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][KeyEvents.KeyDown] =
function( wParam, lParam )
	if wParam == Keys.VK_ESCAPE then
		UI.ClearSelectedCities();
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	elseif wParam == Keys.VK_NUMPAD1 or wParam == Keys.VK_NUMPAD3 or wParam == Keys.VK_NUMPAD4 or wParam == Keys.VK_NUMPAD6 or wParam == Keys.VK_NUMPAD7 or wParam == Keys.VK_NUMPAD8 then
		UI.ClearSelectedCities();
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	else
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	end
end

-- this is a default handler for all Interface Modes that correspond to a mission
function missionTypeLButtonUpHandler()
	local plot = Map.GetPlot( UI.GetMouseOverHex() )
	if plot then
		local interfaceMode = UI.GetInterfaceMode()
		if UI.CanDoInterfaceMode(interfaceMode) then
			local eInterfaceModeMission = GameInfoTypes[(GameInfo.InterfaceModes[interfaceMode] or {}).Mission]
			if eInterfaceModeMission and eInterfaceModeMission ~= MissionTypes.NO_MISSION then
				Game.SelectionListGameNetMessage( GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, eInterfaceModeMission, plot:GetX(), plot:GetY(), 0, false, UIManager:GetShift() )
			end
		end
	end
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION)
	return true
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_TYPE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_ALL][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_ROUTE_TO][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRLIFT][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_NUKE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PARADROP][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIR_SWEEP][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_REBASE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;

if UI.IsTouchScreenEnabled() then
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_TYPE][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_ALL][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_ROUTE_TO][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRLIFT][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_NUKE][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PARADROP][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIR_SWEEP][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_REBASE][MouseEvents.PointerUp] = missionTypeLButtonUpHandler;
end


function AirStrike()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();

	-- Don't let the user do a ranged attack on a plot that contains some fighting.
	if plot:IsFighting() then
		return true;
	end

	-- should handle the case of units bombarding tiles when they are already at war
	if pHeadSelectedUnit and pHeadSelectedUnit:CanRangeStrikeAt(plotX, plotY, true, true) then
		local interfaceMode = UI.GetInterfaceMode();
		local eInterfaceModeMission = GameInfoTypes[GameInfo.InterfaceModes[interfaceMode].Mission];
		Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, eInterfaceModeMission, plotX, plotY, 0, false, bShift);
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	-- Unit Range Strike - special case for declaring war with range strike
	elseif pHeadSelectedUnit and pHeadSelectedUnit:CanRangeStrikeAt(plotX, plotY, false, true) then
		-- Is there someone here that we COULD bombard, perhaps?
		local eRivalTeam = pHeadSelectedUnit:GetDeclareWarRangeStrike(plot);
		if eRivalTeam ~= -1 then
			UIManager:SetUICursor(0);

			local popupInfo =
			{
				Type  = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARRANGESTRIKE,
				Data1 = eRivalTeam,
				Data2 = plotX,
				Data3 = plotY
			};
			Events.SerialEventGameMessagePopup(popupInfo);
			UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
			return true;
		end
	end

	return true;
end
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE][MouseEvents.LButtonUp] = AirStrike;
if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE][MouseEvents.PointerUp] = AirStrike;
end


function RangeAttack()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();

	-- Don't let the user do a ranged attack on a plot that contains some fighting.
	if plot:IsFighting() then
		return true;
	end

	-- should handle the case of units bombarding tiles when they are already at war
	if pHeadSelectedUnit and pHeadSelectedUnit:CanRangeStrikeAt(plotX, plotY, true, true) then
		Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_RANGE_ATTACK, plotX, plotY, 0, false, bShift);
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	-- Unit Range Strike - special case for declaring war with range strike
	elseif pHeadSelectedUnit and pHeadSelectedUnit:CanRangeStrikeAt(plotX, plotY, false, true) then
		-- Is there someone here that we COULD bombard, perhaps?
		local eRivalTeam = pHeadSelectedUnit:GetDeclareWarRangeStrike(plot);
		if eRivalTeam ~= -1 then
			UIManager:SetUICursor(0);

			local popupInfo =
			{
				Type  = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARRANGESTRIKE,
				Data1 = eRivalTeam,
				Data2 = plotX,
				Data3 = plotY
			};
			Events.SerialEventGameMessagePopup(popupInfo);
			UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
			return true;
		end
	end

	return true;
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK][MouseEvents.LButtonUp] = RangeAttack;
if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK][MouseEvents.PointerUp] = RangeAttack;
end

function CityBombard()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedCity = UI.GetHeadSelectedCity();

	-- Don't let the user do a ranged attack on a plot that contains some fighting.
	if plot:IsFighting() then
		UI.ClearSelectedCities();
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	end

	if pHeadSelectedCity and pHeadSelectedCity:CanRangeStrike() then
		if pHeadSelectedCity:CanRangeStrikeAt(plotX,plotY, true, true) then
			Game.SelectedCitiesGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_TASK, TaskTypes.TASK_RANGED_ATTACK, plotX, plotY);
			local activePlayerID = Game.GetActivePlayer();
			Events.SpecificCityInfoDirty( activePlayerID, pHeadSelectedCity:GetID(), CityUpdateTypes.CITY_UPDATE_TYPE_BANNER);
		end
		UI.ClearSelectedCities();
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	end

	return true;
end
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][MouseEvents.LButtonUp] = CityBombard;
if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][MouseEvents.PointerUp] = CityBombard;
end

-- If the user presses the right mouse button when in city range attack mode, make sure and clear the
-- mode and also clear the selection.
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][MouseEvents.RButtonUp] =
function ()
	UI.ClearSelectedCities();
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	return true;
end

function EmbarkInputHandler()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();

	if pHeadSelectedUnit then
		if pHeadSelectedUnit:CanEmbarkOnto(pHeadSelectedUnit:GetPlot(), plot) then
			Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_EMBARK, plotX, plotY, 0, false, bShift);
		end
	end

	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end
-- Have either right or left click trigger this
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_EMBARK][MouseEvents.LButtonUp] = EmbarkInputHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_EMBARK][MouseEvents.RButtonUp] = EmbarkInputHandler;

if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_EMBARK][MouseEvents.PointerUp] = EmbarkInputHandler;
end

function DisembarkInputHandler()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();

	if pHeadSelectedUnit then
		if pHeadSelectedUnit:CanDisembarkOnto(plot) then
			Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_DISEMBARK, plotX, plotY, 0, false, bShift);
		end
	end

	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end
-- Have either right or left click trigger this
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DISEMBARK][MouseEvents.LButtonUp] = DisembarkInputHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DISEMBARK][MouseEvents.RButtonUp] = DisembarkInputHandler;

if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DISEMBARK][MouseEvents.PointerUp] = DisembarkInputHandler;
end



-- this is a default handler for all Interface Modes that correspond to a mission
--function missionTypeLButtonUpHandler( wParam, lParam )
	--UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	--return true;
--end
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_TYPE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO_ALL][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_ROUTE_TO][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRLIFT][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_NUKE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PARADROP][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
----InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIR_SWEEP][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_REBASE][MouseEvents.LButtonUp] = missionTypeLButtonUpHandler;
--

function canUnitMoveToPlotSameTurn(unit, targetPlot)
	local path = unit:GeneratePath(targetPlot, 1)
	for i = 1, #path, 1 do
		return true;
	end
	return false;
end

function canChangeTradeHomeCity(unit, targetPlot)
	return (unit:GetUnitClassType() == GameInfoTypes["UNITCLASS_CARAVAN"]
	      or (unit:GetUnitClassType() == GameInfoTypes["UNITCLASS_CARGO_SHIP"]) and targetPlot:IsCoastalLand())
		and targetPlot:IsCity() and targetPlot:IsFriendlyCity(unit)
end

function canChangeHomePort(unit, targetPlot)
	return unit:GetUnitClassType() == GameInfoTypes["UNITCLASS_GREAT_ADMIRAL"]                     -- must be great admiral
		and unit:GetPlot():IsCity()                                                                -- must be in a city
		and targetPlot:IsCity() and targetPlot:IsFriendlyCity(unit) and targetPlot:IsCoastalLand() -- must be targeting a friendly coastal city
		and (not canUnitMoveToPlotSameTurn(unit, targetPlot));                                     -- sailing is faster, don't use up whole turn to get there
end

function isAirliftFastestTravelToPlot(unit, targetPlot)
	return unit:CanAirliftAt(targetPlot, unit:GetPlot():GetX(), unit:GetPlot():GetY())
		and (not canUnitMoveToPlotSameTurn(unit, targetPlot))
		and unit:CanMoveOrAttackInto(targetPlot)
end

function highlightAlternativeMove(targetPlot)
	Events.DisplayMovementIndicator(false);
	Events.ShowMovementRange();
	OnClearUnitMoveHexRange();

	Events.SerialEventHexHighlight(ToHexFromGrid({x = targetPlot:GetX(), y = targetPlot:GetY()}), true, AlternativeMoveColor, "GroupBorder");
end


-- Returns true if unit can move to targetPlot without pathfinder
function showAlternativeMoveHighlights(unit, targetPlot)
	Events.ClearHexHighlightStyle("GroupBorder");
	if canChangeHomePort(unit, targetPlot)
		or canChangeTradeHomeCity(unit, targetPlot)
		or isAirliftFastestTravelToPlot(unit, targetPlot) then

		highlightAlternativeMove(targetPlot)
		return true;
	end

	return false;
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.RButtonDown] =
function()
	local bShift = UIManager:GetShift();
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	if not plot then
		return true;
	end

	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();

	if pHeadSelectedUnit ~= nil and showAlternativeMoveHighlights(pHeadSelectedUnit, plot) then
		return;
	end

	ShowMovementRangeIndicator();
	UI.SendPathfinderUpdate();
	if bShift then
		UpdatePathFromWaypointToMouse();
	else
		UpdatePathFromSelectedUnitToMouse();
	end
end

if UI.IsTouchScreenEnabled() then
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.LButtonDoubleClick] =
function()
	if UI.GetHeadSelectedUnit() then
		UI.SetInterfaceMode( InterfaceModeTypes.INTERFACEMODE_MOVE_TO );
		bEatNextUp = true;
	end
end
end

function ClearAllHighlights()
	--Events.ClearHexHighlights(); other systems might be using these!
	Events.ClearHexHighlightStyle("");
	Events.ClearHexHighlightStyle(pathBorderStyle);
	Events.ClearHexHighlightStyle(attackPathBorderStyle);
--Events.ClearHexHighlightStyle(genericUnitHexBorder);
	Events.ClearHexHighlightStyle("FireRangeBorder");
	Events.ClearHexHighlightStyle("GroupBorder");
	Events.ClearHexHighlightStyle("ValidFireTargetBorder");
end

function MovementRButtonUp()
	if bEatNextUp == true then
		bEatNextUp = false;
		return;
	end
	if UI.IsTouchScreenEnabled() then
		UI.SetInterfaceMode( InterfaceModeTypes.INTERFACEMODE_SELECTION );
	end
	local bShift = UIManager:GetShift();
	local bAlt = UIManager:GetAlt();
	local bCtrl = UIManager:GetControl();
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	if not plot then
		return true;
	end

	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	UpdatePathFromSelectedUnitToMouse();
--	if bShift then
--		UpdatePathFromWaypointToMouse();
--	else
--		UpdatePathFromSelectedUnitToMouse();
--	end

	if pHeadSelectedUnit then
		local pUnitClass = pHeadSelectedUnit:GetUnitClassType()
		local pUnitID = pHeadSelectedUnit:GetID()
		if UI.IsCameraMoving() and not Game.GetAllowRClickMovementWhileScrolling() then
			print("Blocked by moving camera");
			--Events.ClearHexHighlights();
			ClearAllHighlights();
			return;
		end

		Game.SetEverRightClickMoved(true);

		local bBombardEnemy = false;

		-- Is there someone here that we COULD bombard perhaps?
		local eRivalTeam = pHeadSelectedUnit:GetDeclareWarRangeStrike(plot);
		-- Don't bombard if we can move peacefully to the plot
		if eRivalTeam ~= -1 and not pHeadSelectedUnit:CanMoveOrAttackInto(plot, 1, 1) then
			UIManager:SetUICursor(0);

			local popupInfo =
			{
				Type  = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARRANGESTRIKE,
				Data1 = eRivalTeam,
				Data2 = plotX,
				Data3 = plotY
			};
			Events.SerialEventGameMessagePopup(popupInfo);
			return true;
		end


		if bShift and pHeadSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_AIR then

			-- Try to Mass Forward Rebase
			for aircraft in getOrderedAircraft(pHeadSelectedUnit:GetPlot(), pUnitClass, highHpThenHighLevelFirst) do
				if aircraft:CanRebaseAt(0, plotX, plotY) then
					UI.SelectUnit(aircraft);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_REBASE, plotX, plotY);
				end
			end

			-- Visible enemy... bombardment?
			local bIsTargetEnemyCity = plot:IsEnemyCity(pHeadSelectedUnit);
			if plot:IsVisibleEnemyUnit(pHeadSelectedUnit:GetOwner()) or bIsTargetEnemyCity then

				local bNoncombatAllowed = false;

				if plot:IsFighting() then
					return true;	-- Already some combat going on in there, just exit
				end

				-- Massed Strike Mode
				for aircraft in getOrderedAircraft(pHeadSelectedUnit:GetPlot(), pUnitClass, highHpThenHighLevelFirst) do
					UI.SelectUnit(aircraft);

					while aircraft:CanRangeStrikeAt(plotX, plotY, true, bNoncombatAllowed) do
						-- If not doing any more damage to a city don't let the rest of the planes on this base keep hitting it
						if bIsTargetEnemyCity then
							local pCity = plot:GetPlotCity();
							if 1 + pCity:GetDamage() >= pCity:GetMaxHitPoints() then
								break;
							end
						end

						aircraft:PushMission(MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, 0, 0);
					end

					UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
					ClearAllHighlights();
				end
			end

		-- Fallback Rebase
		elseif bAlt and pHeadSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_AIR then

			for aircraft in getOrderedAircraft(pHeadSelectedUnit:GetPlot(), pUnitClass, lowHpFirst) do
				if aircraft:CanRebaseAt(0, plotX, plotY) and aircraft:GetCurrHitPoints() < aircraft:GetMaxHitPoints() then
					UI.SelectUnit(aircraft);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_REBASE, plotX, plotY);
				end
			end
		else
			-- rebase with RCLICK
			if pHeadSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_AIR and pHeadSelectedUnit:CanRebaseAt(0, plotX, plotY) then
				Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_REBASE, plotX, plotY);
			end

			-- Visible enemy... bombardment?
			if plot:IsVisibleEnemyUnit(pHeadSelectedUnit:GetOwner()) or plot:IsEnemyCity(pHeadSelectedUnit) then

				local bNoncombatAllowed = false;

				if plot:IsFighting() then
					return true;		-- Already some combat going on in there, just exit
				end

				if pHeadSelectedUnit:CanRangeStrikeAt(plotX, plotY, true, bNoncombatAllowed) then

					local iMission;
					if pHeadSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_AIR then
						iMission = MissionTypes.MISSION_MOVE_TO;		-- Air strikes are moves... yep
					else
						iMission = MissionTypes.MISSION_RANGE_ATTACK;
					end

					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, iMission, plotX, plotY, 0, false, bShift);
					UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
					--Events.ClearHexHighlights();
					ClearAllHighlights();
					return true;
				end
			end
		end

		if not gk_mode then
			-- Garrison in a city
			local city = plot:GetPlotCity();
			if city and city:GetOwner() == pHeadSelectedUnit:GetOwner() and pHeadSelectedUnit:IsCanAttack() then
				local cityOwner = Players[city:GetOwner()];
				if not cityOwner:IsMinorCiv() and city:GetGarrisonedUnit() == nil and pHeadSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_LAND then
					--print("Garrison attempt");
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_GARRISON, plotX, plotY, 0, false, bShift);
					UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
					ClearAllHighlights();
					return true;
				end
			end
		end

		-- No visible enemy to bombard, just move
		--print("bBombardEnemy check");
		if bBombardEnemy == false then
			if bShift == false and pHeadSelectedUnit:AtPlot(plot) then
				--print("Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_COMMAND, CommandTypes.COMMAND_CANCEL_ALL);");
				Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_COMMAND, CommandTypes.COMMAND_CANCEL_ALL);

			-- VP: QoL key+click combinations
			elseif bShift and not bCtrl then -- VP/bal: holding down shift queues move orders
				Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, true);

			elseif bCtrl then --VP/bal: ctrl+click gives move&activate orders (e.g. move&alert, move&improve resource). ctrl+shift+click queues
				if pHeadSelectedUnit:IsCanAttack() then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_ALERT, plotX, plotY, 0, false, true);

				elseif pUnitClass == GameInfoTypes["UNITCLASS_ARCHAEOLOGIST"] and pHeadSelectedUnit:CanBuild(plot, buildArchaeology) then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_BUILD, buildArchaeology, pUnitID, 0, false, true);

				elseif pUnitClass == GameInfoTypes["UNITCLASS_WORKER"] or pUnitClass == GameInfoTypes["UNITCLASS_WORKBOAT"] then
					local build = nil
					if plot:IsImprovementPillaged() or plot:IsRoutePillaged() then
						build = buildRepair
					elseif plot:GetResourceType() ~= NO_RESOURCE then
						build = plot:GetBuildTypeNeededToImproveResource()
					end
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_BUILD, build, pUnitID, 0, false, true);

				elseif pUnitClass == GameInfoTypes["UNITCLASS_MISSIONARY"] then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_SPREAD_RELIGION, plotX, plotY, 0, false, true);

				elseif pUnitClass == GameInfoTypes["UNITCLASS_INQUISITOR"] then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_REMOVE_HERESY, plotX, plotY, 0, false, true);

				elseif pHeadSelectedUnit:GetUnitCombatType() == GameInfoTypes["UNITCOMBAT_DIPLOMACY"] then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_TRADE, plotX, plotY, 0, false, true);

				elseif pHeadSelectedUnit:IsGreatPerson() then
					local build = nil
					if pUnitClass == GameInfoTypes["UNITCLASS_ENGINEER"] then
						build = buildManufactory
					elseif pUnitClass == GameInfoTypes["UNITCLASS_SCIENTIST"] then
						build = buildAcademy
					elseif pUnitClass == GameInfoTypes["UNITCLASS_PROPHET"] then
						build = buildHolySite
					elseif pUnitClass == GameInfoTypes["UNITCLASS_GREAT_GENERAL"] then
						if pHeadSelectedUnit:GetUnitType() ==  GameInfoTypes["UNIT_MONGOLIAN_KHAN"] then
							build = buildOrdo
						else
							build = buildCitadel
						end
					elseif pUnitClass == GameInfoTypes["UNITCLASS_MERCHANT"] then
						build = buildCustomsHouse
					elseif pUnitClass == GameInfoTypes["UNITCLASS_GREAT_DIPLOMAT"] then
						build = buildEmbassy
					else
						Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					end
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_MOVE_TO, plotX, plotY, 0, false, bShift);
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_BUILD, build, pUnitID, 0, false, true);
				end -- VP end

			else--if plot == UI.GetGotoPlot() then
				if canChangeHomePort(pHeadSelectedUnit, plot) then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_CHANGE_ADMIRAL_PORT, plotX, plotY, 0, false, bShift);
				elseif canChangeTradeHomeCity(pHeadSelectedUnit, plot) then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_CHANGE_TRADE_UNIT_HOME_CITY, plotX, plotY, 0, false, bShift);
					
					-- Close trade popups if they are open when this trade unit moved via this shortcut
					local chooseTradeRoute = ContextPtr:LookUpControl( "/InGame/ChooseInternationalTradeRoute" );
					if( chooseTradeRoute ~= nil ) then
						UIManager:DequeuePopup(chooseTradeRoute);
					end

					local chooseTradeUnitHome = ContextPtr:LookUpControl( "/InGame/ChooseTradeRouteNewHome" );
					if( chooseTradeUnitHome ~= nil ) then
						UIManager:DequeuePopup(chooseTradeUnitHome);
					end

				elseif isAirliftFastestTravelToPlot(pHeadSelectedUnit, plot) then
					Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_AIRLIFT, plotX, plotY, 0, false, bShift);
				else
					Game.SelectionListMove(plot,  bAlt, bShift, bCtrl);
        end


				-- Game.SelectionListMove(plot,  bAlt, bShift, bCtrl);
				--UI.SetGotoPlot(nil);
			end
			--Events.ClearHexHighlights();
			ClearAllHighlights();
			return true;
		end
	end
end

function getOrderedAircraft(airbasePlot, unitClass, comparator)
	local aircraft = {}
	local j = 0;

	local unitCount = airbasePlot:GetNumUnits();
	for i = 0, unitCount - 1 do
		local unit = airbasePlot:GetUnit(i);
		if unit:GetUnitClassType() == unitClass then
			print(string.format("Inserting unit %s of type %s", unit:GetName(), unit:GetUnitClassType()));
			j = j + 1;
			aircraft[j] = unit;
		end
	end

	table.sort(aircraft, comparator)

	local i = 0;
	return function()
		i = i + 1;
		if aircraft[i] then
			return aircraft[i];
		end
	end
end

function lowHpFirst(unitA, unitB)
	return unitA:GetCurrHitPoints() < unitB:GetCurrHitPoints();
end

function highHpThenHighLevelFirst(unitA, unitB)
	if unitA:GetCurrHitPoints() ~= unitB:GetCurrHitPoints() then
		return unitA:GetCurrHitPoints() > unitB:GetCurrHitPoints()
	end

	return unitA:GetLevel() > unitB:GetLevel()
end


InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.RButtonUp] = MovementRButtonUp;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.RButtonUp] = MovementRButtonUp;

if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.PointerUp] = MovementRButtonUp;
end


function EjectHandler()
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	--print("INTERFACEMODE_PLACE_UNIT");

	local unit = UI.GetPlaceUnit();
	UI.ClearPlaceUnit();
	local returnValue = false;

	if unit then
		local city = unit:GetPlot():GetPlotCity();
		if city then
			if UI.CanPlaceUnitAt(unit, plot) then
				--Network.SendCityEjectGarrisonChoice(city:GetID(), plotX, plotY);
				returnValue =  true;
			end
		end
	end

	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);

	return returnValue;
end
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PLACE_UNIT][MouseEvents.LButtonUp] = EjectHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PLACE_UNIT][MouseEvents.RButtonUp] = EjectHandler;

if UI.IsTouchScreenEnabled() then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PLACE_UNIT][MouseEvents.PointerUp] = EjectHandler;
end

function SelectHandler()
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end

DefaultMessageHandler[MouseEvents.RButtonUp] = SelectHandler;
if UI.IsTouchScreenEnabled() then
	DefaultMessageHandler[MouseEvents.PointerUp] = SelectHandler;
end


----------------------------------------------------------------
-- Input handling
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == MouseEvents.RButtonDown then
		rButtonDown = true;
	elseif uiMsg == MouseEvents.RButtonUp then
		rButtonDown = false;
	end
	if UI.IsTouchScreenEnabled() and uiMsg == MouseEvents.PointerDown then
		if UIManager:GetNumPointers() > 1 then
			UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	   end
	end

	local interfaceMode = UI.GetInterfaceMode();
	local currentInterfaceModeHandler = InterfaceModeMessageHandler[interfaceMode];
	if currentInterfaceModeHandler and currentInterfaceModeHandler[uiMsg] then
		return currentInterfaceModeHandler[uiMsg]( wParam, lParam );
	elseif DefaultMessageHandler[uiMsg] then
		return DefaultMessageHandler[uiMsg]( wParam, lParam );
	end
	return false;
end
ContextPtr:SetInputHandler( InputHandler );

----------------------------------------------------------------
-- Deal with a new path from the path finder
-- this is a place holder implementation to give an example of how to handle it
----------------------------------------------------------------
function OnUIPathFinderUpdate()
--	UpdatePathFromSelectedUnitToMouse();
	local bShift = UIManager:GetShift();
	if bShift then
		UpdatePathFromWaypointToMouse();
	else
		UpdatePathFromSelectedUnitToMouse();
	end
end
Events.UIPathFinderUpdate.Add( OnUIPathFinderUpdate );

function OnMouseMoveHex()
	local interfaceMode = UI.GetInterfaceMode();
	local bShift = UIManager:GetShift();
	if not bShift and rButtonDown and interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION or interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO then
		local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
		local plot = Map.GetPlot( UI.GetMouseOverHex() );

		if pHeadSelectedUnit ~= nil and plot ~= nil and showAlternativeMoveHighlights(pHeadSelectedUnit, plot) then
			return;
		else
			UI.SendPathfinderUpdate();
		end

	elseif bShift and rButtonDown and interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION or interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO then
		UpdatePathFromWaypointToMouse()
	end
end
Events.SerialEventMouseOverHex.Add( OnMouseMoveHex );

function OnClearUnitMoveHexRange()
	Events.ClearHexHighlightStyle(pathBorderStyle);
	Events.ClearHexHighlightStyle(attackPathBorderStyle);
end
Events.ClearUnitMoveHexRange.Add( OnClearUnitMoveHexRange );

function OnStartUnitMoveHexRange()
	Events.ClearHexHighlightStyle(pathBorderStyle);
	Events.ClearHexHighlightStyle(attackPathBorderStyle);
end
Events.StartUnitMoveHexRange.Add( OnStartUnitMoveHexRange );

function OnAddUnitMoveHexRangeHex(i, j, k, attackMove)
	if attackMove and Map.GetPlot(ToGridFromHex(i, j)):IsVisibleEnemyUnit(Game.GetActivePlayer()) then
		Events.SerialEventHexHighlight( Vector2( i, j ), true, turn1Color, attackPathBorderStyle );
	else
		Events.SerialEventHexHighlight( Vector2( i, j ), true, turn1Color, pathBorderStyle );
	end
end
Events.AddUnitMoveHexRangeHex.Add( OnAddUnitMoveHexRangeHex );

--Events.EndUnitMoveHexRange.Add( OnEndUnitMoveHexRange );

local g_PerPlayerStrategicViewSettings = {}
----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged(iActivePlayer, iPrevActivePlayer)

	if iPrevActivePlayer ~= -1 then
		g_PerPlayerStrategicViewSettings[ iPrevActivePlayer ] = InStrategicView();
	end

	if iActivePlayer ~= -1 and not g_PerPlayerStrategicViewSettings[ iActivePlayer ] == InStrategicView() then
		ToggleStrategicView();
	end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

-------------------------------------------------
if gk_mode then
	function OnMultiplayerGameAbandoned(eReason)
		local popupInfo =
		{
			Type  = ButtonPopupTypes.BUTTONPOPUP_KICKED,
			Data1 = eReason
		};

		Events.SerialEventGameMessagePopup(popupInfo);
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	end
	Events.MultiplayerGameAbandoned.Add( OnMultiplayerGameAbandoned );
end

-------------------------------------------------
function OnMultiplayerGameLastPlayer()
	UI.AddPopup( { Type = ButtonPopupTypes.BUTTONPOPUP_TEXT,
	Data1 = 800,
	Option1 = true,
	Text = "TXT_KEY_MP_LAST_PLAYER" } );
end
Events.MultiplayerGameLastPlayer.Add( OnMultiplayerGameLastPlayer );

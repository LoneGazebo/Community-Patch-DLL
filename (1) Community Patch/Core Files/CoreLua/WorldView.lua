-------------------------------------------------
-- World View 
-------------------------------------------------
include( "FLuaVector" );

local g_pathFromSelectedUnitToMouse = nil;
local turn1Color = Vector4( 0, 1, 0, 0.25 );
local turn2Color = Vector4( 1, 1, 0, 0.25 );
local turn3PlusColor = Vector4( 0.25, 0, 1, 0.25 );
local maxRangeColor = Vector4( 1, 1, 1, 0.25 );
local rButtonDown = false;
local bEatNextUp = false;

local pathBorderStyle = "MovementRangeBorder";
local attackPathBorderStyle = "AMRBorder"; -- attack move
         
function UpdatePathFromSelectedUnitToMouse()
	local interfaceMode = UI.GetInterfaceMode();
	--Events.ClearHexHighlightStyle(pathBorderStyle);
	--if (interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION and rButtonDown) or (interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO) then
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
		--if ( #g_pathFromSelectedUnitToMouse > 0 ) then
			--Events.DisplayMovementIndicator( true );
		--else
			--Events.DisplayMovementIndicator( false );
		--end
	--end
	
	if (interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION and rButtonDown) or (interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO) then
		Events.DisplayMovementIndicator( true );
	else
		Events.DisplayMovementIndicator( false );
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
	[InterfaceModeTypes.INTERFACEMODE_GIFT_UNIT] = {}
}

local DefaultMessageHandler = {};


DefaultMessageHandler[KeyEvents.KeyDown] =
function( wParam, lParam )
	if ( wParam == Keys.VK_LEFT ) then
		Events.SerialEventCameraStopMovingRight();
		Events.SerialEventCameraStartMovingLeft();
		return true;
	elseif ( wParam == Keys.VK_RIGHT ) then
		Events.SerialEventCameraStopMovingLeft();
		Events.SerialEventCameraStartMovingRight();
		return true;
	elseif ( wParam == Keys.VK_UP ) then
		Events.SerialEventCameraStopMovingBack();
		Events.SerialEventCameraStartMovingForward();
		return true;
	elseif ( wParam == Keys.VK_DOWN ) then
		Events.SerialEventCameraStopMovingForward();
		Events.SerialEventCameraStartMovingBack();
		return true;
	elseif ( wParam == Keys.VK_NEXT or wParam == Keys.VK_OEM_MINUS ) then
		Events.SerialEventCameraOut( Vector2(0,0) );
		return true;
	elseif ( wParam == Keys.VK_PRIOR or wParam == Keys.VK_OEM_PLUS ) then
		Events.SerialEventCameraIn( Vector2(0,0) );
		return true;
	elseif ( wParam == Keys.VK_ESCAPE and InStrategicView() ) then
		ToggleStrategicView();
        return true;
	end
end


DefaultMessageHandler[KeyEvents.KeyUp] =
function( wParam, lParam )
	if ( wParam == Keys.VK_LEFT ) then
		Events.SerialEventCameraStopMovingLeft();
        return true;
	elseif ( wParam == Keys.VK_RIGHT ) then
		Events.SerialEventCameraStopMovingRight();
        return true;
	elseif ( wParam == Keys.VK_UP ) then
		Events.SerialEventCameraStopMovingForward();
        return true;
	elseif ( wParam == Keys.VK_DOWN ) then
		Events.SerialEventCameraStopMovingBack();
        return true;
	end
	return false;
end


-- Emergency key up handler
function KeyUpHandler( wParam )
	if ( wParam == Keys.VK_LEFT ) then
		Events.SerialEventCameraStopMovingLeft();
	elseif ( wParam == Keys.VK_RIGHT ) then
		Events.SerialEventCameraStopMovingRight();
	elseif ( wParam == Keys.VK_UP ) then
		Events.SerialEventCameraStopMovingForward();
	elseif ( wParam == Keys.VK_DOWN ) then
		Events.SerialEventCameraStopMovingBack();
	end
end
Events.KeyUpEvent.Add( KeyUpHandler );

-- INTERFACEMODE_DEBUG

g_UnitPlopper =
{
	UnitType = -1,
	Embarked = false,
	
	Plop =
	function(plot)
		if (g_PlopperSettings.Player ~= -1 and g_UnitPlopper.UnitType ~= -1) then
			local player = Players[g_PlopperSettings.Player];
			if (player ~= nil) then
				local unit;
				
				print(g_UnitPlopper.UnitNameOffset);
				print(player.InitUnitWithNameOffset);
				if(g_UnitPlopper.UnitNameOffset ~= nil and player.InitUnitWithNameOffset ~= nil) then
					unit = player:InitUnitWithNameOffset(g_UnitPlopper.UnitType, g_UnitPlopper.UnitNameOffset, plot:GetX(), plot:GetY());
				else
					unit = player:InitUnit(g_UnitPlopper.UnitType, plot:GetX(), plot:GetY());
				end 
						
				if (g_UnitPlopper.Embarked) then
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
			if unit ~= nil then
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
		if (g_ResourcePlopper.ResourceType ~= -1) then
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
		if (g_ImprovementPlopper.ImprovementType ~= -1) then
			plot:SetImprovementType(g_ImprovementPlopper.ImprovementType);
			if (g_ImprovementPlopper.Pillaged) then
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
		if (g_PlopperSettings.Player ~= -1) then
			local player = Players[g_PlopperSettings.Player];
			if (player ~= nil) then
				player:InitCity(plot:GetX(), plot:GetY());
			end
		end
	end,
	
	Deplop =
	function(plot)
		local city = plot:GetPlotCity();
		if (city ~= nil) then
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
function( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local debugItem1 = UI.GetInterfaceModeDebugItemID1();
	
	-- City
	if (UI.debugItem1 == 0) then
		pActivePlayer:InitCity(plotX, plotY);
	-- Unit
	elseif (debugItem1 == 1) then
		local iUnitID = UI.GetInterfaceModeDebugItemID2();
		pActivePlayer:InitUnit(iUnitID, plotX, plotY);
	-- Improvement
	elseif (debugItem1 == 2) then
		local iImprovementID = UI.GetInterfaceModeDebugItemID2();
		plot:SetImprovementType(iImprovementID);
	-- Route
	elseif (debugItem1 == 3) then
		local iRouteID = UI.GetInterfaceModeDebugItemID2();
		plot:SetRouteType(iRouteID);
	-- Feature
	elseif (debugItem1 == 4) then
		local iFeatureID = UI.GetInterfaceModeDebugItemID2();
		plot:SetFeatureType(iFeatureID);
	-- Resource
	elseif (debugItem1 == 5) then
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
function( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
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
function( wParam, lParam )
	-- Trap RButtonUp
	return true;
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK][KeyEvents.KeyDown] = 
function( wParam, lParam )
	if ( wParam == Keys.VK_ESCAPE ) then
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	elseif (wParam == Keys.VK_NUMPAD1 or wParam == Keys.VK_NUMPAD3 or wParam == Keys.VK_NUMPAD4 or wParam == Keys.VK_NUMPAD6 or wParam == Keys.VK_NUMPAD7 or wParam == Keys.VK_NUMPAD8 ) then
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	else
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	end
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][KeyEvents.KeyDown] = 
function( wParam, lParam )
	if ( wParam == Keys.VK_ESCAPE ) then
		UI.ClearSelectedCities();
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return true;
	elseif (wParam == Keys.VK_NUMPAD1 or wParam == Keys.VK_NUMPAD3 or wParam == Keys.VK_NUMPAD4 or wParam == Keys.VK_NUMPAD6 or wParam == Keys.VK_NUMPAD7 or wParam == Keys.VK_NUMPAD8 ) then
		UI.ClearSelectedCities();
		UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	else
		return DefaultMessageHandler[KeyEvents.KeyDown]( wParam, lParam );
	end
end

-- this is a default handler for all Interface Modes that correspond to a mission
function missionTypeLButtonUpHandler( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	if plot then
		local plotX = plot:GetX();
		local plotY = plot:GetY();
		local bShift = UIManager:GetShift();
		local interfaceMode = UI.GetInterfaceMode();
		local eInterfaceModeMission = GameInfoTypes[GameInfo.InterfaceModes[interfaceMode].Mission];
		if eInterfaceModeMission ~= MissionTypes.NO_MISSION then
			if UI.CanDoInterfaceMode(interfaceMode) then
				Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, eInterfaceModeMission, plotX, plotY, 0, false, bShift);
				UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
				return true;
			end
		end
	end
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	return true;
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

if (UI.IsTouchScreenEnabled()) then
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


function AirStrike( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();
	local interfaceMode = InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK;
	
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
		if (eRivalTeam ~= -1) then							
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
if (UI.IsTouchScreenEnabled()) then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_AIRSTRIKE][MouseEvents.PointerUp] = AirStrike;
end


function RangeAttack( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();
	local interfaceMode = InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK;
	
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
		if (eRivalTeam ~= -1) then		
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
if (UI.IsTouchScreenEnabled()) then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_RANGE_ATTACK][MouseEvents.PointerUp] = RangeAttack;
end
function CityBombard( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedCity = UI.GetHeadSelectedCity();
	local bShift = UIManager:GetShift();
	local interfaceMode = InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK;
	
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
if (UI.IsTouchScreenEnabled()) then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][MouseEvents.PointerUp] = CityBombard;
end

-- If the user presses the right mouse button when in city range attack mode, make sure and clear the
-- mode and also clear the selection.
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_CITY_RANGE_ATTACK][MouseEvents.RButtonUp] = 
function ( wParam, lParam )
	UI.ClearSelectedCities();
   	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
	return true;
end

function EmbarkInputHandler( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();

	if pHeadSelectedUnit then
		if (pHeadSelectedUnit:CanEmbarkOnto(pHeadSelectedUnit:GetPlot(), plot)) then
			Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_EMBARK, plotX, plotY, 0, false, bShift);
		end
	end
	
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end
-- Have either right or left click trigger this
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_EMBARK][MouseEvents.LButtonUp] = EmbarkInputHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_EMBARK][MouseEvents.RButtonUp] = EmbarkInputHandler;

if (UI.IsTouchScreenEnabled()) then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_EMBARK][MouseEvents.PointerUp] = EmbarkInputHandler;
end

function DisembarkInputHandler( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	local pHeadSelectedUnit = UI.GetHeadSelectedUnit();
	local bShift = UIManager:GetShift();

	if pHeadSelectedUnit then
		if (pHeadSelectedUnit:CanDisembarkOnto(plot)) then
			Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_DISEMBARK, plotX, plotY, 0, false, bShift);
		end
	end
	
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end
-- Have either right or left click trigger this
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DISEMBARK][MouseEvents.LButtonUp] = DisembarkInputHandler;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_DISEMBARK][MouseEvents.RButtonUp] = DisembarkInputHandler;

if (UI.IsTouchScreenEnabled()) then
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



InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.RButtonDown] = 
function( wParam, lParam )
	ShowMovementRangeIndicator();
	UI.SendPathfinderUpdate();
	UpdatePathFromSelectedUnitToMouse();
end

if (UI.IsTouchScreenEnabled()) then
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.LButtonDoubleClick] = 
function( wParam, lParam )
    if( UI.GetHeadSelectedUnit() ~= NULL ) then
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
	Events.ClearHexHighlightStyle(genericUnitHexBorder);  
	Events.ClearHexHighlightStyle("FireRangeBorder");
	Events.ClearHexHighlightStyle("GroupBorder");
	Events.ClearHexHighlightStyle("ValidFireTargetBorder");
end


function MovementRButtonUp( wParam, lParam )
    if( bEatNextUp == true ) then
        bEatNextUp = false;
        return;
    end
    if (UI.IsTouchScreenEnabled()) then
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

	if pHeadSelectedUnit then
		if (UI.IsCameraMoving() and not Game.GetAllowRClickMovementWhileScrolling()) then
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

		-- Visible enemy... bombardment?
		if (plot:IsVisibleEnemyUnit(pHeadSelectedUnit:GetOwner()) or plot:IsEnemyCity(pHeadSelectedUnit)) then		
			
			local bNoncombatAllowed = false;
			
			if plot:IsFighting() then
				return true;		-- Already some combat going on in there, just exit
			end
			
			if pHeadSelectedUnit:CanRangeStrikeAt(plotX, plotY, true, bNoncombatAllowed) then
				
				local iMission;
				if (pHeadSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_AIR) then
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

		-- No visible enemy to bombard, just move
		--print("bBombardEnemy check");
		if bBombardEnemy == false then
			if bShift == false and pHeadSelectedUnit:AtPlot(plot) then
				--print("Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_COMMAND, CommandTypes.COMMAND_CANCEL_ALL);");
				Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_DO_COMMAND, CommandTypes.COMMAND_CANCEL_ALL);
			else--if plot == UI.GetGotoPlot() then
				--print("Game.SelectionListMove(plot,  bAlt, bShift, bCtrl);");
				Game.SelectionListMove(plot,  bAlt, bShift, bCtrl);
				--UI.SetGotoPlot(nil);
			end
			--Events.ClearHexHighlights();
			ClearAllHighlights();
			return true;
		end
	end
end

InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_SELECTION][MouseEvents.RButtonUp] = MovementRButtonUp;
InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.RButtonUp] = MovementRButtonUp;

if (UI.IsTouchScreenEnabled()) then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_MOVE_TO][MouseEvents.PointerUp] = MovementRButtonUp;
end


function EjectHandler( wParam, lParam )
	local plot = Map.GetPlot( UI.GetMouseOverHex() );
	local plotX = plot:GetX();
	local plotY = plot:GetY();
	--print("INTERFACEMODE_PLACE_UNIT");

	local unit = UI.GetPlaceUnit();
	UI.ClearPlaceUnit();	
	local returnValue = false;
		
	if (unit ~= nil) then
		local city = unitPlot:GetPlotCity();
		if (city ~= nil) then
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

if (UI.IsTouchScreenEnabled()) then
	InterfaceModeMessageHandler[InterfaceModeTypes.INTERFACEMODE_PLACE_UNIT][MouseEvents.PointerUp] = EjectHandler;
end

function SelectHandler( wParam, lParam )
	UI.SetInterfaceMode(InterfaceModeTypes.INTERFACEMODE_SELECTION);
end

DefaultMessageHandler[MouseEvents.RButtonUp] = SelectHandler;
if (UI.IsTouchScreenEnabled()) then
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
	if( UI.IsTouchScreenEnabled() and uiMsg == MouseEvents.PointerDown ) then
	    if( UIManager:GetNumPointers() > 1 ) then
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
function OnUIPathFinderUpdate( thePath )
	--g_pathFromSelectedUnitToMouse = thePath;
	UpdatePathFromSelectedUnitToMouse();
end
Events.UIPathFinderUpdate.Add( OnUIPathFinderUpdate );

function OnMouseMoveHex( hexX, hexY )
	--local pPlot = Map.GetPlot( hexX, hexY);
	local interfaceMode = UI.GetInterfaceMode();	
	if (interfaceMode == InterfaceModeTypes.INTERFACEMODE_SELECTION and rButtonDown) or (interfaceMode == InterfaceModeTypes.INTERFACEMODE_MOVE_TO) then
		UI.SendPathfinderUpdate();
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

function OnAddUnitMoveHexRangeHex(i, j, k, attackMove, unitID)
	if attackMove then
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
	
	if (iPrevActivePlayer ~= -1) then
		g_PerPlayerStrategicViewSettings[ iPrevActivePlayer + 1 ] = InStrategicView();
	end
	
	if (iActivePlayer ~= -1 ) then
		if (g_PerPlayerStrategicViewSettings[ iActivePlayer + 1] and not InStrategicView()) then
			ToggleStrategicView();
		else
			if (not g_PerPlayerStrategicViewSettings[ iActivePlayer + 1] and InStrategicView()) then
				ToggleStrategicView();
			end
		end
	end
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

-------------------------------------------------
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

-------------------------------------------------
function OnMultiplayerGameLastPlayer()
	UI.AddPopup( { Type = ButtonPopupTypes.BUTTONPOPUP_TEXT, 
	Data1 = 800, 
	Option1 = true, 
	Text = "TXT_KEY_MP_LAST_PLAYER" } );
end
Events.MultiplayerGameLastPlayer.Add( OnMultiplayerGameLastPlayer );

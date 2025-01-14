-------------------------------------------------
-------------------------------------------------
include( "InstanceManager" );
local m_InstanceManager  = InstanceManager:new( "TurnIndicator",  "Anchor", Controls.Container );

local MOD_UI_DISPLAY_PRECISE_MOVEMENT_POINTS = GameInfo.CustomModOptions("Name = 'UI_DISPLAY_PRECISE_MOVEMENT_POINTS'")().Value == 1

-------------------------------------------------
-------------------------------------------------
function OnPath( data )
    m_InstanceManager:ResetInstances();

    local lastNode = { turn=1 };
    for _, node in ipairs( data ) do
    
        if( node.turn ~= lastNode.turn ) then
            BuildNode( lastNode, false );
        end
        
        lastNode = node;
    end
    
    BuildNode( lastNode, true );
    
end
Events.UIPathFinderUpdate.Add( OnPath );


-------------------------------------------------
-------------------------------------------------
function BuildNode( data, bShowRemainingMoves )
    local instance = m_InstanceManager:GetInstance();

    -- see CvDllGameContext::TEMPCalculatePathFinderUpdates
    if (not MOD_UI_DISPLAY_PRECISE_MOVEMENT_POINTS) then
		local wholeturns = math.floor(data.turn/100);
		local tmp = data.turn - wholeturns*100;
		local totalmovementpoints = math.floor(tmp/10);
		local remainder = tmp - totalmovementpoints*10;

		if (remainder == 0) then
			instance.TurnLabel:SetText(wholeturns);
			instance.RemainingMoves:SetText("");
		else
			instance.TurnLabel:SetText("<" .. (wholeturns+1));
			instance.RemainingMoves:SetText("[ICON_MOVES]" .. remainder .. "/" .. totalmovementpoints);
		end
	else
		local denominator = 60;
		local scaler = denominator*10;
		local wholeturns = math.floor(data.turn/scaler);
		local remaining = data.turn - wholeturns*scaler;

		if (remaining==0) then
			-- full moves (turn start) or no movement left (turn finished)
			-- value from pathfinder is as expected
			instance.TurnLabel:SetText(wholeturns);
		else
			-- movement points left for the turn
			-- by convention pathfinder returns completed turns, so we convert
			instance.TurnLabel:SetText("<" .. (wholeturns+1));
		end

		if (bShowRemainingMoves) then
			-- for the plot the mouse is on we show extra info
			instance.RemainingMoves:SetText("[ICON_MOVES]" .. remaining .. "/" .. denominator);
		else
			instance.RemainingMoves:SetText("");
		end
	end
    
    local plot = Map.GetPlot( data.x, data.y );
    if( plot ~= nil ) then

        local worldPosX, worldPosY, worldPosZ = GridToWorld( data.x, data.y );

        if( plot:IsRevealed( Game.GetActiveTeam(), false ) ) then
            worldPosZ = worldPosZ + 3;
        else
            worldPosZ = 15;            
        end
        
        instance.Anchor:SetWorldPositionVal( worldPosX, worldPosY, worldPosZ );
    end
end


-------------------------------------------------
-------------------------------------------------
function OnDisplay( bShow )
    Controls.Container:SetHide( not bShow );
end
Events.DisplayMovementIndicator.Add( OnDisplay );


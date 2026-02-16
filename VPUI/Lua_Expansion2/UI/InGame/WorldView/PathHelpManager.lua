if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-------------------------------------------------
include( "InstanceManager" );
local m_InstanceManager  = InstanceManager:new( "TurnIndicator",  "Anchor", Controls.Container );


-------------------------------------------------
-------------------------------------------------
function OnPath( data )
    m_InstanceManager:ResetInstances();

    local lastNode = { turn=1 };
    for _, node in ipairs( data ) do
    
        if( node.turn ~= lastNode.turn ) then
            BuildNode( lastNode );
        end
        
        lastNode = node;
    end
    
    BuildNode( lastNode );
    
end
Events.UIPathFinderUpdate.Add( OnPath );


-------------------------------------------------
-------------------------------------------------
function BuildNode( data )
    local instance = m_InstanceManager:GetInstance();
    instance.TurnLabel:SetText( data.turn );
    
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


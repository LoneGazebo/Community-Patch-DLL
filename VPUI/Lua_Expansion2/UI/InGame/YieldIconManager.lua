if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "FLuaVector" );
include( "InstanceManager" );


-------------------------------------------------
-- Yield Icon Manager
-------------------------------------------------
local g_AnchorIM = InstanceManager:new( "AnchorInstance", "Anchor", Controls.Scrap );
local g_ImageIM  = InstanceManager:new( "ImageInstance",  "Image",  Controls.Scrap );

local cultureTexture = "YieldAtlas_128_Culture.dds"
local defaultTexture = "YieldAtlas.dds"
local faithTexture = "YieldAtlas_128_Faith.dds"

local g_VisibleSet = {};
local g_YieldInfoCache = {};

--[[
local g_Shown = 0;
local g_Hidden = 0;
function Update( fDTime )
    if( g_Shown > 0 or g_Hidden > 0 ) then
        print( "-------------------------------------------------" );
        if( g_Shown > 0 ) then
            print( "Shown hexes: " .. g_Shown );
            g_Shown = 0;
        end
        
        if( g_Hidden > 0 ) then
            print( "Hidden hexes: " .. g_Hidden );
            g_Hidden = 0;
        end
       
        local tableCount = 0; 
        for n, v in pairs( g_ActiveYieldInstances ) do
            tableCount = tableCount + 1;
        end
        print( tableCount .. "-------------------------------------------------" );
        print( g_InstanceManager.m_iAllocatedInstances .. "-------------------------------------------------" );
    end
end
ContextPtr:SetUpdate( Update );
--]]


-----------------------------------------------------------------------
-----------------------------------------------------------------------
function ShowHexYield( x, y, bShow )

    local plot = Map.GetPlot( x, y );
    if( plot ~= nil ) then
		local index = plot:GetPlotIndex();
		if( bShow ) then
			DestroyYield( index );
			BuildYield( x, y, index );
		else
			DestroyYield( index );
		end
	end
 
end
Events.ShowHexYield.Add( ShowHexYield );


------------------------------------------------------------------
-- update the controls to reflect the current known yield 
------------------------------------------------------------------
function BuildYield( x, y, index )

    local plot = Map.GetPlot( x, y );
    if( plot == nil ) then
        print( "Missing Plot information [" .. x .. " " .. y .. "]" );
        return;
    end

    if( not plot:IsRevealed( Game.GetActiveTeam(), false ) ) then
        return;
    end

    --print( "Showing hex( " .. tostring( x ) .. " " .. tostring( y ) .. " ) index: " .. index );


    local yieldInfo = g_YieldInfoCache[ index ];
    if( yieldInfo == nil ) then    
    
        -- calculate the yields
        yieldInfo = RecordYieldInfo( x, y, index, plot );

        -- early out of allocation if there is no yield for this tile
        if( yieldInfo == nil ) then
            return;
        end
    end


    record = {};
    record.AnchorInstance = g_AnchorIM:GetInstance();
    record.AnchorInstance.Anchor:ChangeParent( Controls.YieldStore );
    g_VisibleSet[ index ] = record;
    local anchor = record.AnchorInstance;

    SetYieldIcon( 0, yieldInfo.m_Food,       anchor.Stack, record );
    SetYieldIcon( 1, yieldInfo.m_Production, anchor.Stack, record );
    SetYieldIcon( 2, yieldInfo.m_Gold,       anchor.Stack, record );
    SetYieldIcon( 3, yieldInfo.m_Science,    anchor.Stack, record );
    SetYieldIcon( 4, yieldInfo.m_Culture,    anchor.CultureContainer, record );
    SetYieldIcon( 5, yieldInfo.m_Faith,      anchor.Stack, record );

    anchor.Stack:CalculateSize();
    anchor.Stack:ReprocessAnchoring();
    anchor.Anchor:SetHexPosition( x, y );
end


------------------------------------------------------------------
------------------------------------------------------------------
function SetYieldIcon( yieldType, amount, parent, record )

    if amount > 0 then

        local imageInstance = g_ImageIM:GetInstance();
        
        if( yieldType < 4 ) then
            imageInstance.Image:SetTexture(defaultTexture);
            if( amount >= 6 ) then
                imageInstance.Image:SetTextureOffsetVal( yieldType * 128, 512 );
            else
                imageInstance.Image:SetTextureOffsetVal( yieldType * 128, 128 * ( amount - 1 ) );
            end

			imageInstance.Image:ChangeParent( parent );

			if( record.ImageInstances == nil ) then
				record.ImageInstances = {};
			end
			table.insert( record.ImageInstances, imageInstance );

			 if( amount > 5 ) then
				local textImageInstance = g_ImageIM:GetInstance();

				textImageInstance.Image:SetTextureOffsetVal( GetNumberOffset( amount ) );
	            
				textImageInstance.Image:ChangeParent( imageInstance.Image );
				table.insert( record.ImageInstances, textImageInstance );
			end
       elseif (yieldType == 4) then
            imageInstance.Image:SetTexture(cultureTexture);
            if( amount >= 5 ) then
                imageInstance.Image:SetTextureOffsetVal( 0 * 128, 512 );
            else
                imageInstance.Image:SetTextureOffsetVal( 0 * 128, 128 * ( amount - 1 ) );
            end
            
			imageInstance.Image:ChangeParent( parent );

			if( record.ImageInstances == nil ) then
				record.ImageInstances = {};
			end
			table.insert( record.ImageInstances, imageInstance );

       elseif (yieldType == 5) then
            imageInstance.Image:SetTexture(faithTexture);
            if( amount >= 5 ) then
                imageInstance.Image:SetTextureOffsetVal( 0 * 128, 512 );
            else
                imageInstance.Image:SetTextureOffsetVal( 0 * 128, 128 * ( amount - 1 ) );
            end
            
			imageInstance.Image:ChangeParent( parent );

			if( record.ImageInstances == nil ) then
				record.ImageInstances = {};
			end
			table.insert( record.ImageInstances, imageInstance );

        end
    end
end


------------------------------------------------------------------
------------------------------------------------------------------
function GetNumberOffset( number )
    if( number > 12 ) then
        number = 12;
    end
    
    local x = 128 * ((number - 6) % 4);
    
    local y;
    if( number > 9 ) then
        y = 768
    else
        y = 640;
    end
    
    return x, y;
end


------------------------------------------------------------------
------------------------------------------------------------------
function DestroyYield( index )
    --print( "Hiding hex( " .. tostring( x ) .. " " .. tostring( y ) .. " )  index: " .. index );
    --g_Hidden = g_Hidden + 1;
    
    -- we can get double hide requests when an icon scrolls off the corner of the view
    local instanceSet = g_VisibleSet[ index ];
    if( instanceSet ) then
        local instance = instanceSet.AnchorInstance;
        if( instance ) then
            instance.Anchor:ChangeParent( Controls.Scrap );
            g_AnchorIM:ReleaseInstance( instance );
        end
        
        for i, instance in ipairs( instanceSet.ImageInstances ) do
            instance.Image:ChangeParent( Controls.Scrap );
            g_ImageIM:ReleaseInstance( instance );
        end
        
        g_VisibleSet[ index ] = nil;
    end
    
end
   

------------------------------------------------------------------
------------------------------------------------------------------
function RecordYieldInfo( x, y, index, plot )
	
    local iFood       = plot:CalculateYield( 0, true );
    local iProduction = plot:CalculateYield( 1, true );
    local iGold       = plot:CalculateYield( 2, true );
    local iScience    = plot:CalculateYield( 3, true );
    local iCulture    = plot:CalculateYield( 4, true );
    local iFaith      = plot:CalculateYield( 5, true );
	    
    -- early out of allocations if there is no yield for this tile
    if( iFood == 0 and iProduction == 0 and iGold == 0 and iScience == 0 and iCulture == 0 and iFaith == 0) then
        return;
    end
    
    if( g_YieldInfoCache[ index ] == nil ) then
        g_YieldInfoCache[ index ] = {};
    end
    
    local info = g_YieldInfoCache[ index ];
    info.m_Food       = iFood;
    info.m_Production = iProduction;
    info.m_Gold       = iGold;
    info.m_Science    = iScience;
    info.m_Culture    = iCulture;
    info.m_Faith      = iFaith;
    
    return info;
end


-------------------------------------------------
-------------------------------------------------
function OnYieldChangeEvent( gridX, gridY )

    local plot = Map.GetPlot( gridX, gridY );
    if( plot == nil )
    then
        print( "Missing Plot information [" .. gridX .. " " .. gridY .. "]" );
        return;
    end
    
    if( not plot:IsRevealed( Game.GetActiveTeam(), false ) ) then
        return;
    end

    local index = plot:GetPlotIndex();
    
    RecordYieldInfo( gridX, gridY, index, plot );
    
    if( g_VisibleSet[ index ] ~= nil ) then
        DestroyYield( index );
        BuildYield( gridX, gridY, index );
    end
    
end
Events.HexYieldMightHaveChanged.Add( OnYieldChangeEvent );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged(iActivePlayer, iPrevActivePlayer)
	
	-- Reset the yield data.
	for index, pYield in pairs( g_VisibleSet ) do
        DestroyYield( index );
   	end

	for index, pInfo in pairs( g_YieldInfoCache ) do
        g_YieldInfoCache[index] = nil;
   	end
	UI.RefreshYieldVisibleMode();
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

-- Bit of a hack here, we want to ensure that the yield icons are properly refreshed
-- when starting a new game.
UI.RefreshYieldVisibleMode();
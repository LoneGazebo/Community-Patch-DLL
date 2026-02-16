if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "IconSupport" );

-------------------------------------------------
-------------------------------------------------
local m_NaturalWonderWorldAnchorStore = {};
local m_SettlerWorldAnchorStore = {};
local m_WorkerWorldAnchorStore = {};
local m_gridWidth, _ = Map.GetGridSize();


-------------------------------------------------
-------------------------------------------------
function IndexFromGrid( x, y )
    return x + (y * m_gridWidth);
end


-------------------------------------------------
-------------------------------------------------
function OnGenericWorldAnchor( type, bShow, iPlotX, iPlotY, iData1 )

	--print("World anchor event called");
	--print("Data1 is... " .. tostring(iData1));

    if( type == GenericWorldAnchorTypes.WORLD_ANCHOR_NATURAL_WONDER ) then
        HandleNaturalWonder( bShow, iPlotX, iPlotY, iData1 );
    elseif( type == GenericWorldAnchorTypes.WORLD_ANCHOR_SETTLER ) then
        HandleSettlerRecommendation( bShow, iPlotX, iPlotY, iData1 );
    elseif( type == GenericWorldAnchorTypes.WORLD_ANCHOR_WORKER ) then
        HandleWorkerRecommendation( bShow, iPlotX, iPlotY, iData1 );
    end

end
Events.GenericWorldAnchor.Add( OnGenericWorldAnchor );

-------------------------------------------------
-------------------------------------------------
function DestroyNaturalWonderAnchor( hexIndex )
    local instance = m_NaturalWonderWorldAnchorStore[ hexIndex ];
    if( instance ~= nil ) then
        Controls.NaturalWonderStore:ReleaseChild( instance.Anchor );
        m_NaturalWonderWorldAnchorStore[ hexIndex ] = nil;
    end
end
-------------------------------------------------
-------------------------------------------------
function HandleNaturalWonder( bShow, iPlotX, iPlotY, iData1 )

    local hexIndex = IndexFromGrid( iPlotX, iPlotY );

    if( bShow ) then
		--print("Showing Natural Wonder at " .. iPlotX .. ", " .. iPlotY);
		
        local instance = m_NaturalWonderWorldAnchorStore[ hexIndex ];
        
        -- Don't make another copy of something that exists here already
        if( instance == nil ) then
			instance = {};
			ContextPtr:BuildInstanceForControl( "NaturalWonder", instance, Controls.NaturalWonderStore );
			m_NaturalWonderWorldAnchorStore[ hexIndex ] = instance;
			instance.Anchor:SetHexPosition( iPlotX, iPlotY );
	        
	        local strToolTip = "";
	        
			if iData1 ~= -1 then
				local thisFeatureInfo = GameInfo.Features[iData1];
				local portraitIndex = thisFeatureInfo.PortraitIndex;
				if portraitIndex ~= -1 then
					IconHookup( portraitIndex, 64, thisFeatureInfo.IconAtlas, instance.Icon );				
				end
				
				strToolTip = Locale.ConvertTextKey("TXT_KEY_FOUND_NATURAL_WONDER", thisFeatureInfo.Description);
				
			end
			
			instance.Icon:SetToolTipString(strToolTip);
			
		end
    else
		DestroyNaturalWonderAnchor( hexIndex );
    end
end

-------------------------------------------------
-------------------------------------------------
function DestroySettlerAnchor( hexIndex )
	local instance = m_SettlerWorldAnchorStore[ hexIndex ];
	if( instance ~= nil ) then
		Controls.SettlerReccomendationStore:ReleaseChild( instance.Anchor );
		m_SettlerWorldAnchorStore[ hexIndex ] = nil;
	end
end

-------------------------------------------------
-------------------------------------------------
function HandleSettlerRecommendation( bShow, iPlotX, iPlotY, iData1 )
	
	local hexIndex = IndexFromGrid( iPlotX, iPlotY );
	
	local pPlot = Map.GetPlot(iPlotX, iPlotY);
	
	-- If we can't actually found here, don't show the recommendation!
	if (bShow and not Players[Game.GetActivePlayer()]:CanFound(iPlotX, iPlotY)) then
		print("Can't found at: " .. iPlotX .. ", " .. iPlotY);
		return;
	end

	if( bShow ) then
		--print("Showing Settler Recommendation at " .. iPlotX .. ", " .. iPlotY);
		
		local instance = m_SettlerWorldAnchorStore[ hexIndex ];
        
		-- Don't make another copy of something that exists here already
		if( instance == nil ) then
			instance = {};
			ContextPtr:BuildInstanceForControl( "SettlerReccomendation", instance, Controls.SettlerReccomendationStore );
			m_SettlerWorldAnchorStore[ hexIndex ] = instance;
			instance.Anchor:SetHexPosition( iPlotX, iPlotY );
			
			local iActiveTeam = Game.GetActiveTeam();
			local pPlayer = Players[Game.GetActivePlayer()];
	        
	        local strToolTip = "";
	        
	        strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_SETTLER_BASE");
	        
	        local iTotalFood = 0;
	        local iTotalProduction = 0;
	        local iTotalGold = 0;
	        local iLuxuryResources = 0;
	        local iStrategicResources = 0;
	        
	        local iNumPlots = 0;
	        
	        -- Loop around this plot to see what's nearby
			local iRange = 2;
			for iDX = -iRange, iRange, 1 do
				for iDY = -iRange, iRange, 1 do
					local pTargetPlot = Map.GetPlotXY(iPlotX, iPlotY, iDX, iDY);
					if pTargetPlot ~= nil then
						local plotX = pTargetPlot:GetX();
						local plotY = pTargetPlot:GetY();
						local plotDistance = Map.PlotDistance(iPlotX, iPlotY, plotX, plotY);
						if (plotDistance <= iRange) then
							
							iNumPlots = iNumPlots + 1;
							
							-- Sum up what we find on this plot
							iTotalFood = iTotalFood + pTargetPlot:GetYield(YieldTypes.YIELD_FOOD);
							iTotalProduction = iTotalProduction + pTargetPlot:GetYield(YieldTypes.YIELD_PRODUCTION);
							iTotalGold = iTotalGold + pTargetPlot:GetYield(YieldTypes.YIELD_GOLD);
							
							-- Resource
							if (not pTargetPlot:IsOwned()) then
								local iResource = pTargetPlot:GetResourceType(iActiveTeam);
								if (iResource ~= -1) then
									local iUsage = Game.GetResourceUsageType(iResource);
									
									if (iUsage == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
										if (pPlayer:GetNumResourceAvailable(iResource) == 0) then
											iLuxuryResources = iLuxuryResources + 1;
										end
									elseif (iUsage == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC) then
										iStrategicResources = iStrategicResources + 1;
									end
								end
							end
							
						end
					end
				end
			end
	        
	        -- So, what's near this spot?
	        
	        -- Luxuries
	        if (iLuxuryResources > 0) then
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
				strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_SETTLER_LUXURIES");
			-- Strategic Resources
	        elseif (iStrategicResources > 0) then
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
				strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_SETTLER_STRATEGIC");
			-- Gold
	        elseif (iTotalGold / iNumPlots > 0.75) then
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
				strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_SETTLER_GOLD");
			-- Production
	        elseif (iTotalProduction / iNumPlots > 0.75) then
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
				strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_SETTLER_PRODUCTION");
			-- Food
			elseif (iTotalFood / iNumPlots > 1.2) then
				strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
				strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_SETTLER_FOOD");
	        end
	        
			instance.Icon:SetToolTipString(strToolTip);
		end
	else
		DestroySettlerAnchor( hexIndex );
	end
	
end

-------------------------------------------------
-------------------------------------------------
function DestroyWorkerAnchor( hexIndex )
    local instance = m_WorkerWorldAnchorStore[ hexIndex ];
    if( instance ~= nil ) then
        Controls.WorkerReccomendationStore:ReleaseChild( instance.Anchor );
        m_WorkerWorldAnchorStore[ hexIndex ] = nil;
    end
end
-------------------------------------------------
-------------------------------------------------
function HandleWorkerRecommendation( bShow, iPlotX, iPlotY, iData1 )

    local hexIndex = IndexFromGrid( iPlotX, iPlotY );

    if( bShow ) then
		--print("Showing Worker Recommendation at " .. iPlotX .. ", " .. iPlotY);
		
        local instance = m_WorkerWorldAnchorStore[ hexIndex ];
        
        -- Don't make another copy of something that exists here already
        if( instance == nil ) then
			instance = {};
			ContextPtr:BuildInstanceForControl( "WorkerReccomendation", instance, Controls.WorkerReccomendationStore );
			m_WorkerWorldAnchorStore[ hexIndex ] = instance;
			instance.Anchor:SetHexPosition( iPlotX, iPlotY );
	        
	        local iBuild = iData1;
	        local pBuildInfo = GameInfo.Builds[iBuild];
	        local iImprovement = pBuildInfo.ImprovementType;
	        local iRoute = pBuildInfo.RouteType;
	        local thisInfo;
	        
	        -- Tooltip
	        local strToolTip = Locale.ConvertTextKey("TXT_KEY_RECOMMEND_WORKER", pBuildInfo.Description);
	        
			local pPlot = Map.GetPlot(iPlotX, iPlotY);
	        local iResource = pPlot:GetResourceType(Game.GetActiveTeam());
	        
	        -- Resource here to be hooked up?
	        if (iResource ~= -1 and GameInfo.Resources[iResource].ResourceUsage ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
				
				-- Luxury
				if (GameInfo.Resources[iResource].ResourceUsage == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
					strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
					strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_WORKER_LUXURY");
				-- Strategic
				else
					strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
					strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_RECOMMEND_WORKER_STRATEGIC");
				end
				
	        -- Custom help?
			else
				local strCustomHelp = pBuildInfo.Recommendation;
		        
				if (strCustomHelp ~= nil) then
					strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
					strToolTip = strToolTip .. Locale.ConvertTextKey(strCustomHelp);
				else
					local iYieldChange;
					for iYield = 0, YieldTypes.NUM_YIELD_TYPES-1, 1 do
						iYieldChange = pPlot:GetYieldWithBuild(iBuild, iYield, false, Game.GetActivePlayer());
						iYieldChange = iYieldChange - pPlot:CalculateYield(iYield);
			
						if (iYieldChange > 0) then
							strToolTip = strToolTip .. "[NEWLINE][NEWLINE]";
							if (iYield == YieldTypes.YIELD_FOOD) then
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_BUILD_FOOD_REC");
							elseif (iYield == YieldTypes.YIELD_PRODUCTION) then
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_BUILD_PROD_REC");
							elseif (iYield == YieldTypes.YIELD_GOLD) then
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_BUILD_GOLD_REC");
							elseif (iYield == YieldTypes.YIELD_CULTURE) then
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_BUILD_CULTURE_REC");
							end
						end
					end
				end
	        end
	        
			instance.Icon:SetToolTipString(strToolTip);
	    	IconHookup( pBuildInfo.IconIndex, 45, pBuildInfo.IconAtlas, instance.Icon );		
		end
    else
		DestroyWorkerAnchor( hexIndex );
    end
end

-------------------------------------------------
-------------------------------------------------
function OnStrategicViewStateChanged( bStrategicView )
    Controls.NaturalWonderStore:SetHide( bStrategicView );
end
Events.StrategicViewStateChanged.Add(OnStrategicViewStateChanged);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged(iActivePlayer, iPrevActivePlayer)
	
	-- Currently, all Generic World Anchors only exist for one turn, so just 
	-- remove all of the old player's anchors
	for index, pResource in pairs( m_NaturalWonderWorldAnchorStore ) do
        DestroyNaturalWonderAnchor( index );
   	end
	for index, pResource in pairs( m_SettlerWorldAnchorStore ) do
        DestroySettlerAnchor( index );
   	end
	for index, pResource in pairs( m_WorkerWorldAnchorStore ) do
        DestroyWorkerAnchor( index );
   	end
		
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

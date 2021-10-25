print("This is the modded YieldIconManager from 'DLL - Various Mod Components'")

-------------------------------------------------
-- Yield Icon Manager
-------------------------------------------------
include("FLuaVector");
include("InstanceManager");

local g_AnchorIM = InstanceManager:new("AnchorInstance", "Anchor", Controls.Scrap);
local g_ImageIM  = InstanceManager:new("ImageInstance",  "Image",  Controls.Scrap);

local g_VisibleSet = {};
local g_YieldInfoCache = {};

-- This MUST have (at least) YieldTypes.NUM_YIELD_TYPES entries!  (There are currently a few spare)
local g_StackPlacement = {1, 1, 1, 2, 2, 2, 1, 2, 1, 2}

------------------------------------------------------------------
-- update the controls to reflect the current known yield 
------------------------------------------------------------------
function BuildYield(gridX, gridY, index, pPlot)
    if (not pPlot:IsRevealed(Game.GetActiveTeam(), false)) then
        return;
    end

    local yieldInfo = g_YieldInfoCache[index];
    if (yieldInfo == nil) then    
        yieldInfo = RecordYieldInfo(index, pPlot);

        if (yieldInfo == nil) then
            return;
        end
    end

    record = {};
    record.AnchorInstance = g_AnchorIM:GetInstance();
    record.AnchorInstance.Anchor:ChangeParent(Controls.YieldStore);
    g_VisibleSet[index] = record;
    local anchor = record.AnchorInstance;

	local iPlace = 1
	for iYield = 0, YieldTypes.NUM_YIELD_TYPES-1, 1 do
		if (yieldInfo[iYield] > 0) then
			SetYieldIcon(iYield, yieldInfo[iYield], anchor[string.format("Stack%i", (g_StackPlacement[iPlace] or 1))], record);
			iPlace = iPlace + 1;
		end
	end

    anchor.Stack1:CalculateSize();
    anchor.Stack1:ReprocessAnchoring();
    anchor.Stack2:CalculateSize();
    anchor.Stack2:ReprocessAnchoring();
    anchor.Anchor:SetHexPosition(gridX, gridY);
end

------------------------------------------------------------------
function SetYieldIcon(yieldType, amount, parent, record)
    local imageInstance = g_ImageIM:GetInstance();
	imageInstance.Image:SetTexture(GameInfo.Yields[yieldType].ImageTexture);
			
	local imageOffset = GameInfo.Yields[yieldType].ImageOffset
    if (amount > 5) then
        imageInstance.Image:SetTextureOffsetVal(imageOffset, 512);
    else
        imageInstance.Image:SetTextureOffsetVal(imageOffset, 128 * (amount - 1));
    end

	imageInstance.Image:ChangeParent(parent);

	if (record.ImageInstances == nil) then
		record.ImageInstances = {};
	end
	table.insert(record.ImageInstances, imageInstance);

	if (amount > 5) then
		local textImageInstance = g_ImageIM:GetInstance();
		textImageInstance.Image:SetTextureOffsetVal(GetNumberOffset(amount));
		textImageInstance.Image:ChangeParent(imageInstance.Image);
		table.insert(record.ImageInstances, textImageInstance);
    end
end

------------------------------------------------------------------
function GetNumberOffset(number)
    if (number > 12) then
        number = 12;
    end
    
    local x = 128 * ((number - 6) % 4);
    
    local y;
    if (number > 9) then
        y = 768
    else
        y = 640;
    end
    
    return x, y;
end

------------------------------------------------------------------
function DestroyYield(index)
    local instanceSet = g_VisibleSet[index];
	
    if (instanceSet) then
        local instance = instanceSet.AnchorInstance;
		
        if (instance) then
            instance.Anchor:ChangeParent(Controls.Scrap);
            g_AnchorIM:ReleaseInstance(instance);
        end
        
        for _, instance in ipairs(instanceSet.ImageInstances) do
            instance.Image:ChangeParent(Controls.Scrap);
            g_ImageIM:ReleaseInstance(instance);
        end
        
        g_VisibleSet[index] = nil;
    end
end
   
------------------------------------------------------------------
function RecordYieldInfo(index, pPlot)
    local info = {};
    local iTotal = 0
	
	for iYield = 0, YieldTypes.NUM_YIELD_TYPES-1, 1 do
        info[iYield] = pPlot:CalculateYield(iYield, true);
		iTotal = iTotal + info[iYield]
	end
	
    if (iTotal == 0) then
        info = nil;
    end
    
    g_YieldInfoCache[index] = info;
    
    return info;
end

-----------------------------------------------------------------------
function ShowHexYield(gridX, gridY, bShow)
    local pPlot = Map.GetPlot(gridX, gridY);

    if (pPlot) then
		local index = pPlot:GetPlotIndex();
		
		if (bShow) then
			DestroyYield(index);
			BuildYield(gridX, gridY, index, pPlot);
		else
			DestroyYield(index);
		end
	end
 
end
Events.ShowHexYield.Add(ShowHexYield);

-------------------------------------------------
function OnYieldChangeEvent(gridX, gridY)
    local pPlot = Map.GetPlot(gridX, gridY);

    if (pPlot == nil) then
        print("Missing Plot information [" .. gridX .. " " .. gridY .. "]");
        return;
    end
    
    if (not pPlot:IsRevealed(Game.GetActiveTeam(), false)) then
        return;
    end

    local index = pPlot:GetPlotIndex();
    
    RecordYieldInfo(index, pPlot);
    
    if (g_VisibleSet[index]) then
        DestroyYield(index);
        BuildYield(gridX, gridY, index, pPlot);
    end
    
end
Events.HexYieldMightHaveChanged.Add(OnYieldChangeEvent);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged(iActivePlayer, iPrevActivePlayer)
	for index, _ in pairs(g_VisibleSet) do
        DestroyYield(index);
   	end

	for index, _ in pairs(g_YieldInfoCache) do
        g_YieldInfoCache[index] = nil;
   	end
	
	UI.RefreshYieldVisibleMode();
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

----------------------------------------------------------------
-- Bit of a hack here, we want to ensure that the yield icons
-- are properly refreshed when starting a new game.
----------------------------------------------------------------
UI.RefreshYieldVisibleMode();

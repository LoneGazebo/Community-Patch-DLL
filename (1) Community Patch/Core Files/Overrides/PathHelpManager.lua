print("This is the modded PathHelpManager from Community Patch");

include("InstanceManager");
include("CPK.lua");

local instanceManager = InstanceManager:new("TurnIndicator", "Anchor", Controls.Container);

local MOD_UI_DISPLAY_PRECISE_MOVEMENT_POINTS = CPK.Misc.CustomModOptionEnabled("UI_DISPLAY_PRECISE_MOVEMENT_POINTS");

local MOVE_DENOMINATOR = GameDefines.MOVE_DENOMINATOR;

-------------------------------------------------
--- @param node PathNode
--- @param bShowRemainingMoves boolean
local function BuildNode(node, bShowRemainingMoves, bAirlift, bChangePort)
	local instance = instanceManager:GetInstance();

	local bIsStopNode = false
	-- See CvDllGameContext::TEMPCalculatePathFinderUpdates
	if MOD_UI_DISPLAY_PRECISE_MOVEMENT_POINTS then
		local iScale = MOVE_DENOMINATOR * 10;
		local iTurns = math.floor(node.turn / iScale);
		local iMoves = node.turn - iTurns * iScale;

		if iMoves == 0 then
			-- Full moves (turn start) or no movement left (turn finished)
			-- Value from path finder is as expected
			instance.TurnLabel:SetText(iTurns);
		else
			-- Movement points left for the turn
			-- By convention path finder returns completed turns, so we convert
			instance.TurnLabel:SetText("<" .. (iTurns + 1));
		end

		if bShowRemainingMoves then
			instance.RemainingMoves:SetText("[ICON_MOVES]" .. iMoves .. "/" .. MOVE_DENOMINATOR);
		else
			instance.RemainingMoves:SetText("");
		end
	else
		bIsStopNode = node.turn < 0;
		local iTurns = math.floor(math.abs(node.turn) / 100);
		local iRemainder = math.abs(node.turn) - iTurns * 100;
		local iMaxMoves = math.floor(iRemainder / 10);
		local iMoves = iRemainder - iMaxMoves * 10;

		if bAirlift then
			instance.RemainingMoves:LocalizeAndSetText("TXT_KEY_PATHFINDING_AIRLIFT");
		elseif bChangePort then
			instance.RemainingMoves:LocalizeAndSetText("TXT_KEY_PATHFINDING_CHANGE_PORT");
		elseif iMoves == 0 then
			if bIsStopNode then
				instance.RemainingMoves:LocalizeAndSetText("TXT_KEY_PATHFINDING_ENDING_MOVE_EARLY");
			else
				instance.RemainingMoves:SetText("");
			end
		else
			instance.RemainingMoves:SetText("[ICON_MOVES]" .. iMoves .. "/" .. iMaxMoves);
		end

		if iMoves == 0 then
			instance.TurnLabel:SetText(iTurns);
		else
			instance.TurnLabel:SetText("<" .. (iTurns + 1));
		end
	end

	local pPlot = Map.GetPlot(node.x, node.y);
	if pPlot then
		local iWorldX, iWorldY, iWorldZ = GridToWorld(node.x, node.y);

		if pPlot:IsRevealed(Game.GetActiveTeam(), false) then
			iWorldZ = iWorldZ + 3;
		else
			iWorldZ = 15;
		end

		instance.Anchor:SetWorldPositionVal(iWorldX, iWorldY, iWorldZ);
	end
end

-------------------------------------------------
--- @param tPath PathNode[]
Events.UIPathFinderUpdate.Add(function (tPath)
	instanceManager:ResetInstances();

	local pSelectedUnit = UI.GetHeadSelectedUnit()

	local tAirlift = {};
	local tChangePort = {};
	if pSelectedUnit then
		local bIsLand = pSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_LAND;
		local bIsSea  = pSelectedUnit:GetDomainType() == DomainTypes.DOMAIN_SEA;
		for i = 1, #tPath - 1 do
			local nodeA = tPath[i];
			local nodeB = tPath[i + 1];
			if Map.PlotDistance(nodeA.x, nodeA.y, nodeB.x, nodeB.y) > 1 then
				if bIsLand then
					tAirlift[i]     = true;
					tAirlift[i + 1] = true;
				elseif bIsSea then
					tChangePort[i]     = true;
					tChangePort[i + 1] = true;
				end
			end
		end
	end

	for i = #tPath, 1, -1 do
		local node = tPath[i]
		BuildNode(node, i == #tPath, tAirlift[i] or false, tChangePort[i] or false);
	end

end);

-------------------------------------------------
--- @param bShow boolean
Events.DisplayMovementIndicator.Add(function (bShow)
	Controls.Container:SetHide(not bShow);
end);

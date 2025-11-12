print("This is the modded PathHelpManager from Community Patch");

include("InstanceManager");
include("CPK.lua");

local instanceManager = InstanceManager:new("TurnIndicator", "Anchor", Controls.Container);

local MOD_UI_DISPLAY_PRECISE_MOVEMENT_POINTS = CPK.Misc.CustomModOptionEnabled("UI_DISPLAY_PRECISE_MOVEMENT_POINTS");

local MOVE_DENOMINATOR = GameDefines.MOVE_DENOMINATOR;

-------------------------------------------------
--- @param node PathNode
--- @param bShowRemainingMoves boolean
local function BuildNode(node, bShowRemainingMoves)
	local instance = instanceManager:GetInstance();

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
		local iTurns = math.floor(node.turn / 100);
		local iRemainder = node.turn - iTurns * 100;
		local iMaxMoves = math.floor(iRemainder / 10);
		local iMoves = iRemainder - iMaxMoves * 10;

		if iMoves == 0 then
			instance.TurnLabel:SetText(iTurns);
			instance.RemainingMoves:SetText("");
		else
			instance.TurnLabel:SetText("<" .. (iTurns + 1));
			instance.RemainingMoves:SetText("[ICON_MOVES]" .. iMoves .. "/" .. iMaxMoves);
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

	local lastNode = {turn = 1};
	for _, node in ipairs(tPath) do
		if node.turn ~= lastNode.turn then
			BuildNode(lastNode, false);
		end

		lastNode = node;
	end

	BuildNode(lastNode, true);
end);

-------------------------------------------------
--- @param bShow boolean
Events.DisplayMovementIndicator.Add(function (bShow)
	Controls.Container:SetHide(not bShow);
end);

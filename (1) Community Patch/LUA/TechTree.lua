-------------------------------------------------
-- Tech Tree Popup
-------------------------------------------------
include("InstanceManager");
include("TechButtonInclude");
include("TechHelpInclude");
include("VPUI_core");
include("CPK.lua");

local L = Locale.Lookup;
local VP = VP or MapModData.VP;
local GameInfoCache = VP.GameInfoCache;
local IconHookupOrDefault = VP.IconHookupOrDefault;
local Hide = CPK.UI.Control.Hide;
local Show = CPK.UI.Control.Show;

local stealingTechTargetPlayerID = -1;

local g_PipeManager = InstanceManager:new("TechPipeInstance", "TechPipeIcon", Controls.TechTreeScrollPanel);
local g_EraManager = InstanceManager:new("EraBlockInstance", "EraBlock", Controls.EraStack);
local g_TechInstanceManager = InstanceManager:new("TechButtonInstance", "TechButton", Controls.TechTreeScrollPanel);

local g_NeedsFullRefreshOnOpen = false;
local g_NeedsFullRefresh = false;

local maxSmallButtons = 5;

-- I'll need these before I'm done
local bEspionageViewMode = false;
local playerID = Game.GetActivePlayer();
local player = Players[playerID];
local activeTeamID = Game.GetActiveTeam();
local activeTeam = Teams[activeTeamID];

-- textures I'll be using
local right1Texture = "TechBranchH.dds";

local left1Texture = "TechBranchH.dds";

local topRightTexture = "TechBranches.dds";
local topLeftTexture = "TechBranches.dds";
local bottomRightTexture = "TechBranches.dds";
local bottomLeftTexture = "TechBranches.dds";

local hTexture = "TechBranchH.dds";
local vTexture = "TechBranchV.dds";

local connectorSize = {x = 32, y = 42};

local blockSpacingX = 270 + 96;
local blockSizeX = 270;
local blockSpacingY = 68;
local extraYOffset = 32;

local FREE_STRING = L("TXT_KEY_FREE");

local maxTechNameLength = 22 - Locale.Length(L("TXT_KEY_TURNS"));

local GAMEOPTION_NO_SCIENCE = Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE);

local tooltipInstance = {};
TTManager:GetTypeControlTable("TechTreeTooltip", tooltipInstance);

--- Shorthand for setting a custom tooltip and adjusting its size
--- @param tooltip TooltipInstance
--- @param strTooltip string
local function SetTooltip(tooltip, strTooltip)
	tooltip.TechTreeTooltipText:SetText(strTooltip);
	tooltip.TechTreeTooltipGrid:DoAutoSize();
end

-------------------------------------------------
-- Do initial setup stuff here
-------------------------------------------------

local techButtons = {};
local eraBlocks = {};
local eraColumns = {};

function InitialSetup()
	-- make the scroll bar the correct size for the display size
	Controls.TechTreeScrollBar:SetSizeX(Controls.TechTreeScrollPanel:GetSizeX() - 150);

	-- add the Era panels to the background
	AddEraPanels();

	-- add the pipes
	local techPipes = {};
	for _, kTechInfo in GameInfoCache("Technologies") do
		techPipes[kTechInfo.Type] = {
			leftConnectionUp = false,
			leftConnectionDown = false,
			leftConnectionCenter = false,
			leftConnectionType = 0,
			rightConnectionUp = false,
			rightConnectionDown = false,
			rightConnectionCenter = false,
			rightConnectionType = 0,
			xOffset = 0,
			techType = kTechInfo.Type,
		};
	end

	local cnxCenter = 1;
	local cnxUp = 2;
	local cnxDown = 4;

	-- Figure out which left and right adapters we need
	for row in GameInfo.Technology_PrereqTechs() do
		local prereq = GameInfo.Technologies[row.PrereqTech];
		local tech = GameInfo.Technologies[row.TechType];
		if tech.GridY < prereq.GridY then
			techPipes[tech.Type].leftConnectionDown = true;
			techPipes[prereq.Type].rightConnectionUp = true;
		elseif tech.GridY > prereq.GridY then
			techPipes[tech.Type].leftConnectionUp = true;
			techPipes[prereq.Type].rightConnectionDown = true;
		else
			techPipes[tech.Type].leftConnectionCenter = true;
			techPipes[prereq.Type].rightConnectionCenter = true;
		end

		local xOffset = (tech.GridX - prereq.GridX) - 1;
		if xOffset > techPipes[prereq.Type].xOffset then
			techPipes[prereq.Type].xOffset = xOffset;
		end
	end

	for _, thisPipe in pairs(techPipes) do
		if thisPipe.leftConnectionDown then
			thisPipe.leftConnectionType = thisPipe.leftConnectionType + cnxDown;
		end
		if thisPipe.leftConnectionUp then
			thisPipe.leftConnectionType = thisPipe.leftConnectionType + cnxUp;
		end
		if thisPipe.leftConnectionCenter then
			thisPipe.leftConnectionType = thisPipe.leftConnectionType + cnxCenter;
		end
		if thisPipe.rightConnectionDown then
			thisPipe.rightConnectionType = thisPipe.rightConnectionType + cnxDown;
		end
		if thisPipe.rightConnectionUp then
			thisPipe.rightConnectionType = thisPipe.rightConnectionType + cnxUp;
		end
		if thisPipe.rightConnectionCenter then
			thisPipe.rightConnectionType = thisPipe.rightConnectionType + cnxCenter;
		end
	end

	for row in GameInfo.Technology_PrereqTechs() do
		local prereq = GameInfo.Technologies[row.PrereqTech];
		local tech = GameInfo.Technologies[row.TechType];
		if tech.GridX - prereq.GridX > 1 then
			local hConnection = g_PipeManager:GetInstance();
			hConnection.TechPipeIcon:SetOffsetVal(prereq.GridX * blockSpacingX + blockSizeX + 96, (prereq.GridY - 5) * blockSpacingY + 12 + extraYOffset);
			hConnection.TechPipeIcon:SetTexture(hTexture);
			local size = {x = (tech.GridX - prereq.GridX - 1) * blockSpacingX + 4, y = 42};
			hConnection.TechPipeIcon:SetSize(size);
		end

		if tech.GridY - prereq.GridY == 1 or tech.GridY - prereq.GridY == -1 then
			local vConnection = g_PipeManager:GetInstance();
			vConnection.TechPipeIcon:SetOffsetVal((tech.GridX - 1) * blockSpacingX + blockSizeX + 96,
				((tech.GridY - 5) * blockSpacingY) - (((tech.GridY - prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
			vConnection.TechPipeIcon:SetTexture(vTexture);
			local size = {x = 32, y = (blockSpacingY / 2) + 8};
			vConnection.TechPipeIcon:SetSize(size);
		end

		if tech.GridY - prereq.GridY == 2 or tech.GridY - prereq.GridY == -2 then
			local vConnection = g_PipeManager:GetInstance();
			vConnection.TechPipeIcon:SetOffsetVal((tech.GridX - 1) * blockSpacingX + blockSizeX + 96,
				(tech.GridY - 5) * blockSpacingY - (((tech.GridY - prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
			vConnection.TechPipeIcon:SetTexture(vTexture);
			local size = {x = 32, y = (blockSpacingY * 3 / 2) + 8};
			vConnection.TechPipeIcon:SetSize(size);
		end

		if tech.GridY - prereq.GridY == 3 or tech.GridY - prereq.GridY == -3 then
			local vConnection = g_PipeManager:GetInstance();
			vConnection.TechPipeIcon:SetOffsetVal((tech.GridX - 1) * blockSpacingX + blockSizeX + 96,
				((tech.GridY - 5) * blockSpacingY) - (((tech.GridY - prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
			vConnection.TechPipeIcon:SetTexture(vTexture);
			local size = {x = 32, y = blockSpacingY * 2 + 20};
			vConnection.TechPipeIcon:SetSize(size);
		end

		if tech.GridY - prereq.GridY == 4 or tech.GridY - prereq.GridY == -4 then
			local vConnection = g_PipeManager:GetInstance();
			vConnection.TechPipeIcon:SetOffsetVal((tech.GridX - 1) * blockSpacingX + blockSizeX + 96,
				(tech.GridY - 5) * blockSpacingY - (tech.GridY - prereq.GridY) * blockSpacingY / 2 + extraYOffset);
			vConnection.TechPipeIcon:SetTexture(vTexture);
			local size = {x = 32, y = blockSpacingY * 3 + 8};
			vConnection.TechPipeIcon:SetSize(size);
		end
	end

	for _, thisPipe in pairs(techPipes) do
		local tech = GameInfo.Technologies[thisPipe.techType];

		local yOffset = (tech.GridY - 5) * blockSpacingY + 12 + extraYOffset;

		if thisPipe.rightConnectionType >= 1 then
			local startPipe = g_PipeManager:GetInstance();
			startPipe.TechPipeIcon:SetOffsetVal(tech.GridX * blockSpacingX + blockSizeX + 64, yOffset);
			startPipe.TechPipeIcon:SetTexture(right1Texture);
			startPipe.TechPipeIcon:SetSize(connectorSize);

			local pipe = g_PipeManager:GetInstance();
			if thisPipe.rightConnectionType == 1 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96, yOffset);
				pipe.TechPipeIcon:SetTexture(right1Texture);
			elseif thisPipe.rightConnectionType == 2 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX + thisPipe.xOffset) * blockSpacingX + blockSizeX + 96 - 12, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 72));
			elseif thisPipe.rightConnectionType == 3 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96, yOffset);
				pipe.TechPipeIcon:SetTexture(right1Texture);
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX + thisPipe.xOffset) * blockSpacingX + blockSizeX + 96 - 12, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 72));
			elseif thisPipe.rightConnectionType == 4 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX + thisPipe.xOffset) * blockSpacingX + blockSizeX + 96 - 12, yOffset);
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 0));
			elseif thisPipe.rightConnectionType == 5 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96, yOffset);
				pipe.TechPipeIcon:SetTexture(right1Texture);
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96 - 12, yOffset);
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 0));
			elseif thisPipe.rightConnectionType == 6 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96 - 12, yOffset);
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 0));
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX + thisPipe.xOffset) * blockSpacingX + blockSizeX + 96 - 12, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 72));
			else
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96, yOffset);
				pipe.TechPipeIcon:SetTexture(right1Texture);
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX + blockSizeX + 96 - 12, yOffset);
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 0));
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX + thisPipe.xOffset) * blockSpacingX + blockSizeX + 96 - 12, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72, 72));
			end
		end

		if thisPipe.leftConnectionType >= 1 then
			local startPipe = g_PipeManager:GetInstance();
			startPipe.TechPipeIcon:SetOffsetVal(tech.GridX * blockSpacingX + 26, yOffset);
			startPipe.TechPipeIcon:SetTexture(left1Texture);
			startPipe.TechPipeIcon:SetSize(Vector2(40, 42));

			local pipe = g_PipeManager:GetInstance();
			if thisPipe.leftConnectionType == 1 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(left1Texture);
			elseif thisPipe.leftConnectionType == 2 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 72));
			elseif thisPipe.leftConnectionType == 3 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(left1Texture);
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 72));
			elseif thisPipe.leftConnectionType == 4 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 0));
			elseif thisPipe.leftConnectionType == 5 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(left1Texture);
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 0));
			elseif thisPipe.leftConnectionType == 6 then
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 0));
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 72));
			else
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(left1Texture);
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset);
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 0));
				pipe = g_PipeManager:GetInstance();
				pipe.TechPipeIcon:SetOffsetVal((tech.GridX) * blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0, 72));
			end
		end
	end

	-- add the instances of the tech panels
	for _, tech in GameInfoCache("Technologies") do
		AddTechButton(tech);
	end

	-- resize the panel to fit the contents
	Controls.EraStack:CalculateSize();
	Controls.EraStack:ReprocessAnchoring();
	Controls.TechTreeScrollPanel:CalculateInternalSize();
end

function AddEraPanels()
	-- find the range of columns that each era takes
	for _, tech in GameInfoCache("Technologies") do
		local eraID = GameInfoTypes[tech.Era];
		if not eraColumns[eraID] then
			eraColumns[eraID] = {minGridX = tech.GridX, maxGridX = tech.GridX, researched = false};
		else
			if tech.GridX < eraColumns[eraID].minGridX then
				eraColumns[eraID].minGridX = tech.GridX;
			end
			if tech.GridX > eraColumns[eraID].maxGridX then
				eraColumns[eraID].maxGridX = tech.GridX;
			end
		end
	end

	-- add the era panels
	for _, era in GameInfoCache("Eras") do
		local thisEraBlockInstance = g_EraManager:GetInstance();
		-- store this panel off for later
		eraBlocks[era.ID] = thisEraBlockInstance;

		-- add the correct text for this era panel
		local localizedLabel = L(era.Description);
		thisEraBlockInstance.OldLabel:SetText(localizedLabel);
		thisEraBlockInstance.CurrentLabel:SetText(localizedLabel);
		thisEraBlockInstance.FutureLabel:SetText(localizedLabel);

		-- adjust the sizes of the era panels
		local blockWidth;
		if eraColumns[era.ID] then
			blockWidth = eraColumns[era.ID].maxGridX - eraColumns[era.ID].minGridX + 1;
		else
			blockWidth = 1;
		end

		blockWidth = blockWidth * blockSpacingX;
		if era.ID == 0 then
			blockWidth = blockWidth + 32;
		end
		thisEraBlockInstance.EraBlock:SetSizeX(blockWidth);
		thisEraBlockInstance.OldBar:SetSizeX(blockWidth);
		thisEraBlockInstance.OldBlock:SetSizeX(blockWidth);
		thisEraBlockInstance.CurrentBlock:SetSizeX(blockWidth);
		thisEraBlockInstance.CurrentBlock2:SetSizeX(blockWidth);
		thisEraBlockInstance.CurrentTop:SetSizeX(blockWidth);
		thisEraBlockInstance.CurrentTop1:SetSizeX(blockWidth);
		thisEraBlockInstance.CurrentTop2:SetSizeX(blockWidth);
		thisEraBlockInstance.FutureBlock:SetSizeX(blockWidth);
	end
end

function TechSelected(eTech)
	if bEspionageViewMode then
		return;
	end

	print("stealingTechTargetPlayerID: " .. stealingTechTargetPlayerID);
	print("player:GetNumFreeTechs(): " .. player:GetNumFreeTechs());
	if eTech > -1 then
		if stealingTechTargetPlayerID ~= -1 then
			Network.SendResearch(eTech, 0, stealingTechTargetPlayerID, UIManager:GetShift());
		else
			Network.SendResearch(eTech, player:GetNumFreeTechs(), -1, UIManager:GetShift());
		end
	end
end

function AddTechButton(tech)
	local thisTechButtonInstance = g_TechInstanceManager:GetInstance();

	-- store this instance off for later
	techButtons[tech.ID] = thisTechButtonInstance;

	-- add the input handler to this button
	thisTechButtonInstance.TechButton:SetVoid1(tech.ID); -- indicates tech to add to queue
	thisTechButtonInstance.TechButton:SetVoid2(0); -- how many free techs

	local strTechName = L(tech.Description);
	thisTechButtonInstance.TechButton:RegisterCallback(Mouse.eRClick, function()
		Events.SearchForPediaEntry(strTechName);
	end);

	if not GAMEOPTION_NO_SCIENCE then
		thisTechButtonInstance.TechButton:RegisterCallback(Mouse.eLClick, TechSelected);
	end

	-- position this instance
	thisTechButtonInstance.TechButton:SetOffsetVal(tech.GridX * blockSpacingX + 64, (tech.GridY - 5) * blockSpacingY + extraYOffset);

	-- update the name of this instance
	strTechName = Locale.TruncateString(strTechName, maxTechNameLength, true);
	thisTechButtonInstance.AlreadyResearchedTechName:SetText(strTechName);
	thisTechButtonInstance.CurrentlyResearchingTechName:SetText(strTechName);
	thisTechButtonInstance.AvailableTechName:SetText(strTechName);
	thisTechButtonInstance.UnavailableTechName:SetText(strTechName);
	thisTechButtonInstance.LockedTechName:SetText(strTechName);
	thisTechButtonInstance.FreeTechName:SetText(strTechName);

	-- Use a closure to track if we're already setting the tooltip
	local bSettingTooltip = false;
	thisTechButtonInstance.TechButton:SetToolTipCallback(function ()
		if bSettingTooltip then
			return;
		end
		bSettingTooltip = true;
		SetTooltip(tooltipInstance, GetHelpTextForTech(tech.ID, false, playerID));
		bSettingTooltip = false;
	end);

	-- update the picture
	IconHookupOrDefault(tech.PortraitIndex, 64, tech.IconAtlas, thisTechButtonInstance.TechPortrait);

	-- add the small pictures and their tooltips
	AddSmallButtonsToTechButton(thisTechButtonInstance, tech, maxSmallButtons, 45, playerID);
end

-------------------------------------------------
-- On Display
-------------------------------------------------
local g_isOpen = false;

function OnDisplay(popupInfo)
	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_TECH_TREE then
		return;
	end

	print("popupInfo.Data1: " .. popupInfo.Data1);
	print("popupInfo.Data2: " .. popupInfo.Data2);
	print("popupInfo.Data3: " .. popupInfo.Data3);

	g_isOpen = true;
	if not g_NeedsFullRefresh then
		g_NeedsFullRefresh = g_NeedsFullRefreshOnOpen;
	end
	g_NeedsFullRefreshOnOpen = false;

	if popupInfo.Data1 == 1 then
		if not ContextPtr:IsHidden() then
			Close();
			return;
		else
			UIManager:QueuePopup(ContextPtr, PopupPriority.eUtmost);
		end
	else
		UIManager:QueuePopup(ContextPtr, PopupPriority.TechTree);
	end

	stealingTechTargetPlayerID = popupInfo.Data2;

	if popupInfo.Data4 > 0 then
		-- Espionage view mode
		bEspionageViewMode = true;
		playerID = popupInfo.Data5;
		g_NeedsFullRefresh = true;
		print("Espionage View Mode, showing Tech Tree for Player " .. playerID);
	else
		bEspionageViewMode = false;
		playerID = Game.GetActivePlayer();
	end
	player = Players[playerID];
	activeTeamID = player:GetTeam();
	activeTeam = Teams[activeTeamID];

	Events.SerialEventGameMessagePopupShown(popupInfo);

	RefreshDisplay();
end

Events.SerialEventGameMessagePopup.Add(OnDisplay);

function RefreshDisplay()
	-- reset the era panels
	for _, column in pairs(eraColumns) do
		column.researched = false;
	end

	for _, tech in GameInfoCache("Technologies") do
		RefreshDisplayOfSpecificTech(tech);
	end

	-- update the era panels
	local eHighestEra = 0;
	for eEra, column in pairs(eraColumns) do
		if column.researched and eEra > eHighestEra then
			eHighestEra = eEra;
		end
	end
	for eEra, column in pairs(eraColumns) do
		Hide(column.OldBar, column.CurrentBlock, column.CurrentTop, column.FutureBlock);
		if eEra < eHighestEra then
			Show(column.OldBar);
		elseif eEra == eHighestEra then
			Show(column.CurrentBlock, column.CurrentTop);
		else
			Show(column.FutureBlock);
		end
	end

	g_NeedsFullRefresh = false;
end

function RefreshDisplayOfSpecificTech(tech)
	local techID = tech.ID;
	local thisTechButton = techButtons[techID];
	local numFreeTechs = player:GetNumFreeTechs();
	local researchTurnsLeft = player:GetResearchTurnsLeft(techID, true);
	local turnText = L("TXT_KEY_BUILD_NUM_TURNS", researchTurnsLeft);
	local isAllowedToStealTech = false;
	local isAllowedToGetTechFree = false;

	if bEspionageViewMode then
		numFreeTechs = 0;
		thisTechButton.TechButton:SetDisabled(true);
	else
		thisTechButton.TechButton:SetDisabled(false);
	end

	-- Espionage - stealing a tech!
	if stealingTechTargetPlayerID ~= -1 then
		if player:CanResearch(techID) then
			local opponentPlayer = Players[stealingTechTargetPlayerID];
			local opponentTeam = Teams[opponentPlayer:GetTeam()];
			if opponentTeam:IsHasTech(techID) then
				isAllowedToStealTech = true;
			end
		end
	end

	-- Choosing a free tech - extra conditions may apply
	if numFreeTechs > 0 then
		if player:CanResearchForFree(techID) then
			isAllowedToGetTechFree = true;
		end
	end

	local potentiallyBlockedFromStealing = stealingTechTargetPlayerID ~= -1 and (not isAllowedToStealTech or player:GetNumTechsToSteal(stealingTechTargetPlayerID) <= 0);

	-- Rebuild the small buttons if needed
	if g_NeedsFullRefresh then
		AddSmallButtonsToTechButton(thisTechButton, tech, maxSmallButtons, 45, playerID);
	end

	if GAMEOPTION_NO_SCIENCE then
		turnText = "";
	else
		thisTechButton.TechButton:SetVoid1(techID); -- indicates tech to add to queue
		thisTechButton.TechButton:SetVoid2(numFreeTechs); -- how many free techs
		AddCallbackToSmallButtons(thisTechButton, maxSmallButtons, techID, numFreeTechs, Mouse.eLClick, TechSelected);
	end

	if activeTeam:GetTeamTechs():HasTech(techID) then -- the player (or more accurately his team) has already researched this one
		Show(thisTechButton.AlreadyResearched);
		Hide(thisTechButton.FreeTech);
		Hide(thisTechButton.CurrentlyResearching);
		Hide(thisTechButton.Available);
		Hide(thisTechButton.Unavailable);
		Hide(thisTechButton.Locked);

		-- update the era marker for this tech
		local eraID = GameInfoTypes[tech.Era];
		if eraColumns[eraID] then
			eraColumns[eraID].researched = true;
		end

		if not GAMEOPTION_NO_SCIENCE then
			Hide(thisTechButton.TechQueue);
			thisTechButton.TechButton:SetVoid2(0); -- num free techs
			thisTechButton.TechButton:SetVoid1(-1); -- indicates tech is invalid
			AddCallbackToSmallButtons(thisTechButton, maxSmallButtons, -1, 0, Mouse.eLClick, TechSelected);
		end
	elseif player:GetCurrentResearch() == techID and (not potentiallyBlockedFromStealing) then -- the player is currently researching this one
		Hide(thisTechButton.AlreadyResearched);
		Hide(thisTechButton.Available);
		Hide(thisTechButton.Unavailable);
		Hide(thisTechButton.Locked);
		-- deal with free tech
		if isAllowedToGetTechFree or (stealingTechTargetPlayerID ~= -1 and isAllowedToStealTech) then
			Show(thisTechButton.FreeTech);
			Hide(thisTechButton.CurrentlyResearching);
			-- update number of turns to research
			if player:GetScienceTimes100() > 0 and stealingTechTargetPlayerID == -1 then
				thisTechButton.FreeTurns:SetText(turnText);
				Show(thisTechButton.FreeTurns);
			else
				Hide(thisTechButton.FreeTurns);
			end
			thisTechButton.TechQueueLabel:SetText(FREE_STRING);
			Show(thisTechButton.TechQueue);
		else
			Hide(thisTechButton.FreeTech);
			Show(thisTechButton.CurrentlyResearching);
			-- update number of turns to research
			if player:GetScienceTimes100() > 0 then
				thisTechButton.CurrentlyResearchingTurns:SetText(turnText);
				Show(thisTechButton.CurrentlyResearchingTurns);
			else
				Hide(thisTechButton.CurrentlyResearchingTurns);
			end
			Hide(thisTechButton.TechQueue);
		end
		-- turn on the meter
		local researchProgressPlusThisTurnPercent = 0;
		local currentResearchProgress = player:GetResearchProgressTimes100(techID) / 100;
		local researchNeeded = player:GetResearchCost(techID);
		local researchPerTurn = player:GetScienceTimes100() / 100;
		local currentResearchPlusThisTurn = currentResearchProgress + researchPerTurn;
		researchProgressPlusThisTurnPercent = currentResearchPlusThisTurn / researchNeeded;
		if researchProgressPlusThisTurnPercent > 1 then
			researchProgressPlusThisTurnPercent = 1;
		end
	elseif player:CanResearch(techID) and not GAMEOPTION_NO_SCIENCE and not potentiallyBlockedFromStealing then -- the player research this one right now if he wants
		Hide(thisTechButton.AlreadyResearched);
		Hide(thisTechButton.CurrentlyResearching);
		Hide(thisTechButton.Unavailable);
		Hide(thisTechButton.Locked);
		-- deal with free 		
		if isAllowedToGetTechFree or (stealingTechTargetPlayerID ~= -1 and isAllowedToStealTech) then
			Show(thisTechButton.FreeTech);
			Hide(thisTechButton.Available);
			-- update number of turns to research
			if player:GetScienceTimes100() > 0 and stealingTechTargetPlayerID == -1 then
				thisTechButton.FreeTurns:SetText(turnText);
				Show(thisTechButton.FreeTurns);
			else
				Hide(thisTechButton.FreeTurns);
			end
			-- update queue number to say "FREE"
			thisTechButton.TechQueueLabel:SetText(FREE_STRING);
			Show(thisTechButton.TechQueue);
		else
			Hide(thisTechButton.FreeTech);
			Show(thisTechButton.Available);
			-- update number of turns to research
			if player:GetScienceTimes100() > 0 then
				thisTechButton.AvailableTurns:SetText(turnText);
				Show(thisTechButton.AvailableTurns);
			else
				Hide(thisTechButton.AvailableTurns);
			end
			-- update queue number if needed
			local queuePosition = player:GetQueuePosition(techID);
			if queuePosition == -1 then
				Hide(thisTechButton.TechQueue);
			else
				thisTechButton.TechQueueLabel:SetText(tostring(queuePosition - 1));
				Show(thisTechButton.TechQueue);
			end
		end
	elseif not player:CanEverResearch(techID) or isAllowedToGetTechFree or stealingTechTargetPlayerID ~= -1 then
		Hide(thisTechButton.AlreadyResearched);
		Hide(thisTechButton.CurrentlyResearching);
		Hide(thisTechButton.Available);
		Hide(thisTechButton.Unavailable);
		Show(thisTechButton.Locked);
		Hide(thisTechButton.FreeTech);
		-- have queue number say "LOCKED"
		thisTechButton.TechQueueLabel:SetText("[ICON_LOCKED]");
		Show(thisTechButton.TechQueue);
		if not GAMEOPTION_NO_SCIENCE then
			thisTechButton.TechButton:SetVoid1(-1);
			thisTechButton.TechButton:SetVoid2(0); -- num free techs
			AddCallbackToSmallButtons(thisTechButton, maxSmallButtons, -1, 0, Mouse.eLClick, TechSelected);
		end
	else -- currently unavailable
		Hide(thisTechButton.AlreadyResearched);
		Hide(thisTechButton.CurrentlyResearching);
		Hide(thisTechButton.Available);
		Show(thisTechButton.Unavailable);
		Hide(thisTechButton.Locked);
		Hide(thisTechButton.FreeTech);
		-- update number of turns to research
		if player:GetScienceTimes100() > 0 then
			thisTechButton.UnavailableTurns:SetText(turnText);
			Show(thisTechButton.UnavailableTurns);
		else
			Hide(thisTechButton.UnavailableTurns);
		end

		-- update queue number if needed
		local queuePosition = player:GetQueuePosition(techID);
		if queuePosition == -1 then
			Hide(thisTechButton.TechQueue);
		else
			thisTechButton.TechQueueLabel:SetText(tostring(queuePosition - 1));
			Show(thisTechButton.TechQueue);
		end

		if not GAMEOPTION_NO_SCIENCE then
			thisTechButton.TechButton:SetVoid1(tech.ID);
			AddCallbackToSmallButtons(thisTechButton, maxSmallButtons, techID, numFreeTechs, Mouse.eLClick, TechSelected);
		end
	end
end

----------------------------------------------------------------
-- Input processing
----------------------------------------------------------------
function Close()
	UIManager:DequeuePopup(ContextPtr);
	Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_TECH_TREE, 0);
	g_isOpen = false;
	if bEspionageViewMode then
		g_NeedsFullRefreshOnOpen = true;
	end
end

Controls.CloseButton:RegisterCallback(Mouse.eLClick, Close);

-------------------------------------------------------------------------------
function InputHandler(uiMsg, wParam)
	if g_isOpen and uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			Close();
			return true;
		end
	end
end

ContextPtr:SetInputHandler(InputHandler);


function ShowHideHandler(bIsHide, bIsInit)
	if not bIsInit then
		if not bIsHide then
			UI.incTurnTimerSemaphore();
		else
			UI.decTurnTimerSemaphore();
		end
	end
end

ContextPtr:SetShowHideHandler(ShowHideHandler);

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnTechTreeActivePlayerChanged()
	playerID = Game.GetActivePlayer();
	player = Players[playerID];
	activeTeamID = Game.GetActiveTeam();
	activeTeam = Teams[activeTeamID];

	-- So some extra stuff gets re-built on the refresh call
	if not g_isOpen then
		g_NeedsFullRefreshOnOpen = true;
	else
		g_NeedsFullRefresh = true;
	end

	-- Close it, so the next player does not have to.
	Close();
end

Events.GameplaySetActivePlayer.Add(OnTechTreeActivePlayerChanged);

-------------------------------------------------------------------------------
function OnEventResearchDirty()
	if g_isOpen then
		RefreshDisplay();
	end
end

Events.SerialEventResearchDirty.Add(OnEventResearchDirty);

-------------------------------------------------------------------------------
-- One time initialization
InitialSetup();

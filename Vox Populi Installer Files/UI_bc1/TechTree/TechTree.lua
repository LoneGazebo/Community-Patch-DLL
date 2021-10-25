-------------------------------------------------
-- Tech Tree Popup
-- coded by bc1 from Civ V 1.0.3.276 code
-- capture choose tech & steal tech popup events
-- fix escape/enter popup exit code
-- support Technology_ORPrereqTechs in addition to Technology_PrereqTechs
-- code is common except for vanila gk_mode needs to be set false
-- decreased code size by over 1/3 and still do more :lol:
-------------------------------------------------
Events.SequenceGameInitComplete.Add(function()
print("Loading EUI tech tree",ContextPtr,os.clock(),[[ 
 _____         _   _____              
|_   _|__  ___| |_|_   _| __ ___  ___ 
  | |/ _ \/ __| '_ \| || '__/ _ \/ _ \
  | |  __/ (__| | | | || | |  __/  __/
  |_|\___|\___|_| |_|_||_|  \___|\___|
]])

g_UseSmallIcons = true

include( "TechButtonInclude" )
local IconHookup = IconHookup
local GatherInfoAboutUniqueStuff = GatherInfoAboutUniqueStuff
local AddSmallButtonsToTechButton = AddSmallButtonsToTechButton
local freeString = freeString
local lockedString = lockedString

include( "TechHelpInclude" )
local GetHelpTextForTech = GetHelpTextForTech

-------------------------------
-- minor lua optimizations
-------------------------------
local math_abs = math.abs
local math_max = math.max
local math_min = math.min

local ButtonPopupTypes = ButtonPopupTypes
local ContextPtr = ContextPtr
local Controls = Controls
local Events = Events
local Game = Game
local GameInfo = EUI.GameInfoCache or GameInfo -- warning! use iterator ONLY with table field conditions, NOT string SQL query
local GameInfoTypes = GameInfoTypes
local KeyEvents_KeyDown = KeyEvents.KeyDown
local Keys = Keys
local L = Locale.ConvertTextKey
local Locale = Locale
local Mouse = Mouse
local Network_SendResearch = Network.SendResearch
local Players = Players
local PopupPriority_InGameUtmost = PopupPriority.InGameUtmost
local Teams = Teams
local UI = UI
local UIManager = UIManager

local gk_mode = Players[0].GetNumTechsToSteal ~= nil

local g_scienceEnabled = not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)

local g_options = Modding.OpenUserData( "Enhanced User Interface Options", 1)
local g_popupInfoType, g_isAutoClose
local g_stealingTechTargetPlayerID, g_stealingTechTargetPlayer = -1

local g_NeedsFullRefresh = true
local g_RefreshRequested = false

local g_techButtons = {}
local g_eraBlocks = {}
local g_eraColumns = {}

local g_maxSmallButtons = 5

local g_activePlayerID, g_activePlayer, g_activeCivType, g_activeTeamID, g_activeTeam, g_activeTeamTechs

local g_connectorSizeX = 32	-- connector texture width
local g_connectorSizeY = 42	-- connector texture height
local g_connectorXshift = 12	-- horizontal connector texture shift
local g_connectorYshift = 15	-- vertical connector texture shift
local g_blockSizeX = 270	-- tech block width
local g_blockOffsetX = 64
local g_blockSpacingY = 68	-- tech block vertical spacing
local g_blockOffsetY = 32 - 5*g_blockSpacingY
local g_blockSpacingX = g_blockSizeX + 96
local g_branchOffsetY1 = g_blockOffsetY + 12
local g_branchOffsetY2 = g_branchOffsetY1 - g_connectorYshift
local g_deltaHeight = g_connectorSizeY - g_connectorYshift
local g_pipeStartOffsetX = g_blockOffsetX + g_blockSizeX
local g_pipeEndOffsetX = g_blockOffsetX
local g_branchOffsetX1 = 0
local g_branchOffsetX2 = g_branchOffsetX1 - g_connectorXshift
local g_branchDeltaX = g_pipeEndOffsetX - g_branchOffsetX1
local g_coloredPipe = { x=1.0, y=1.0, z=0.0, w=0.5 }

local g_maxTechNameLength = 22 - Locale.Length(L"TXT_KEY_TURNS")

local CloseTechTree

-------------------------------------------------
-- Tech Pipe Management
-------------------------------------------------

local function AddNewPipeIconWithoutColor( x, y )
	local newPipeInstance = {}
	ContextPtr:BuildInstanceForControl( "TechPipeInstance", newPipeInstance, Controls.TechTreeScrollPanel )
	local pipeIcon = newPipeInstance.TechPipeIcon
	pipeIcon:SetOffsetVal( x, y )
	return pipeIcon
end

local function AddNewPipeIconColored( x, y )
	local pipeIcon = AddNewPipeIconWithoutColor( x, y )
	pipeIcon:SetColor( g_coloredPipe )
	return pipeIcon
end

local function AddTechPipes( AddNewPipeIcon, techPairs )
	local function AddElbowPipe( x, y, tox, toy )
		local pipeIcon = AddNewPipeIcon( x, y )
		pipeIcon:SetTexture( "TechBranches.dds" )
		pipeIcon:SetTextureOffsetVal( tox, toy )
	end

	local function AddHorizontalPipe( x, y, length )
		local pipeIcon = AddNewPipeIcon( x, y )
		pipeIcon:SetSizeVal( length, g_connectorSizeY )
		pipeIcon:SetTexture( "TechBranchH.dds" )
	end

	-- add straight connectors first
	for row in techPairs() do
		local prereq = GameInfo.Technologies[row.PrereqTech]
		local tech = GameInfo.Technologies[row.TechType]
		if tech and prereq then
			local x1 = prereq.GridX*g_blockSpacingX + g_pipeStartOffsetX
			local x2 = tech.GridX*g_blockSpacingX + g_pipeEndOffsetX

			local height = math_abs( tech.GridY - prereq.GridY ) * g_blockSpacingY - g_deltaHeight

			-- vertical connector
			if height > 0 then
				local x = x2 - g_branchDeltaX
				local pipeIcon = AddNewPipeIcon( x, ( (tech.GridY + prereq.GridY) * g_blockSpacingY - g_connectorYshift ) / 2 + g_branchOffsetY1 )
				pipeIcon:SetTexture( "TechBranchV.dds" )
				pipeIcon:SetSizeVal( g_connectorSizeX, height )
				AddHorizontalPipe( x1, prereq.GridY*g_blockSpacingY + g_branchOffsetY1, x - x1 - g_connectorXshift )
				x = x + g_connectorSizeX
				AddHorizontalPipe( x, tech.GridY*g_blockSpacingY + g_branchOffsetY1, x2 - x )
			else
				AddHorizontalPipe( x1, prereq.GridY*g_blockSpacingY + g_branchOffsetY1, x2 - x1 )
			end
		end
	end

	-- add elbow connectors on top
	for row in techPairs() do
		local prereq = GameInfo.Technologies[row.PrereqTech]
		local tech = GameInfo.Technologies[row.TechType]
		if tech and prereq then
			local x = tech.GridX*g_blockSpacingX
			local y1 = prereq.GridY*g_blockSpacingY
			local y2 = tech.GridY*g_blockSpacingY

			if y1 < y2 then -- elbow case ¯¯¯¯¯¯|__

				AddElbowPipe( x + g_branchOffsetX2, y1 + g_branchOffsetY1, 72, 0 )	--topRightTexture  ¯|
				AddElbowPipe( x + g_branchOffsetX1, y2 + g_branchOffsetY2 , 0, 72 )	--bottomLeftTexture |_

			elseif y1 > y2 then -- elbow case ______|¯¯

				AddElbowPipe( x + g_branchOffsetX2, y1 + g_branchOffsetY2, 72, 72 )	--bottomRightTexture _|
				AddElbowPipe( x + g_branchOffsetX1, y2 + g_branchOffsetY1, 0, 0 )	--topLeftTexture |¯
			end
		end
	end
end

-------------------------------------------------
-- Mouse Click Management
-------------------------------------------------

local function TechSelected( techID )
	if not g_RefreshRequested and GameInfo.Technologies[ techID ] then
		local shift = UIManager:GetShift()
		if g_stealingTechTargetPlayer then
			if g_activePlayer:CanResearch( techID )
				and g_activePlayer:GetNumTechsToSteal( g_stealingTechTargetPlayerID ) > 0
				and Teams[ g_stealingTechTargetPlayer:GetTeam() ]:IsHasTech( techID )
			then
				Network_SendResearch( techID, 0, g_stealingTechTargetPlayerID, shift )
				CloseTechTree()
			end
		else
			Network_SendResearch( techID, g_activePlayer:GetNumFreeTechs(), -1, shift )
			if g_isAutoClose and not shift and g_popupInfoType == ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH then
				CloseTechTree()
			end
		end
	end
end

local function TechPedia( techID )
	local tech = GameInfo.Technologies[ techID ] or {}
	Events.SearchForPediaEntry( tech.Description )
end

local function setTechToolTip( techID, _, button )
	button:SetToolTipString( GetHelpTextForTech( techID, g_activePlayer:CanResearch( techID ) ) )
end

-------------------------------------------------
-- Display Refresh Management
-------------------------------------------------

local function RefreshDisplayOfSpecificTech( tech )
	local techID = tech.ID
	local thisTechButton = g_techButtons[ techID ]
	local canResearchThisTech = g_activePlayer:CanResearch( techID )
	local turnText = L( "TXT_KEY_STR_TURNS", g_activePlayer:GetResearchTurnsLeft( techID, true ) )
	local researchPerTurn = g_activePlayer:GetScience()

	-- Rebuild the small buttons if needed
	if g_NeedsFullRefresh then
		AddSmallButtonsToTechButton( thisTechButton, tech, g_maxSmallButtons, 45 )
	end
	thisTechButton.TechButton:RegisterCallback( Mouse.eMouseEnter, setTechToolTip )

	local showAlreadyResearched, showFreeTech, showCurrentlyResearching, showAvailable, showUnavailable, showLocked, queueUpdate, queueText, turnLabel, isClickable

	if g_activeTeamTechs:HasTech( techID ) then
	-- the active player already has this tech
		showAlreadyResearched = true
		-- update the era marker for this tech
		(g_eraColumns[GameInfoTypes[tech.Era]] or {}).researched = true

	elseif g_stealingTechTargetPlayer or g_activePlayer:GetNumFreeTechs() > 0 then
	-- Stealing a tech or Choosing a free tech
		if canResearchThisTech and ( 
			( g_stealingTechTargetPlayer and Teams[ g_stealingTechTargetPlayer:GetTeam() ]:IsHasTech( techID ) )
			or (not g_stealingTechTargetPlayer and (not gk_mode or g_activePlayer:CanResearchForFree( techID ))) )
		then
			showFreeTech = true
			turnLabel = thisTechButton.FreeTurns
			queueText = freeString	-- update queue number to say "FREE"
			isClickable = true
		else
			showLocked = true
		end

	-- the active player is currently researching this one
	elseif g_activePlayer:GetCurrentResearch() == techID then
		showCurrentlyResearching = true
		turnLabel = thisTechButton.CurrentlyResearchingTurns
		queueUpdate = g_activePlayer:GetLengthResearchQueue() > 1	-- update queue number if needed
		isClickable = true -- to clear research queue
--[[
		-- turn on the meter
		local currentResearchProgress = g_activePlayer:GetResearchProgress( techID )
		local researchNeeded = g_activePlayer:GetResearchCost( techID )
		local currentResearchPlusThisTurn = currentResearchProgress + researchPerTurn
		local researchProgressPercent = currentResearchProgress / researchNeeded
		local researchProgressPlusThisTurnPercent = math_min( 1, currentResearchPlusThisTurn / researchNeeded )
--]]
	-- the active player can research this one right now if he wants
	elseif canResearchThisTech and g_scienceEnabled then
		showAvailable = true
		turnLabel = thisTechButton.AvailableTurns
		queueUpdate = true	-- update queue number if needed
		isClickable = true

	elseif g_activePlayer:CanEverResearch( techID ) and g_scienceEnabled then
	-- currently unavailable
		showUnavailable = true
		queueUpdate = true	-- update queue number if needed
		turnLabel = thisTechButton.UnavailableTurns
		isClickable = true  -- shift clickable
	else
		showLocked = true
	end
	thisTechButton.AlreadyResearched:SetHide( not showAlreadyResearched )
	thisTechButton.FreeTech:SetHide( not showFreeTech )
	thisTechButton.CurrentlyResearching:SetHide( not showCurrentlyResearching )
	thisTechButton.Available:SetHide( not showAvailable )
	thisTechButton.Unavailable:SetHide( not showUnavailable )
	thisTechButton.Locked:SetHide( not showLocked )
	if showLocked then
		queueText = lockedString	-- have queue number say "LOCKED"
	elseif queueUpdate then
		local queuePosition = g_activePlayer:GetQueuePosition( techID )
		if queuePosition ~= -1 then
			queueText = queuePosition
		end
	end
	if queueText then
		thisTechButton.TechQueueLabel:SetText( queueText )
	end
	thisTechButton.TechQueue:SetHide( not queueText )
	if isClickable then
		thisTechButton.TechButton:RegisterCallback( Mouse.eLClick, TechSelected )
		for buttonNum = 1, g_maxSmallButtons do
			thisTechButton["B"..buttonNum]:RegisterCallback( Mouse.eLClick, TechSelected )
		end
	else
		thisTechButton.TechButton:ClearCallback( Mouse.eLClick )
		for buttonNum = 1, g_maxSmallButtons do
			thisTechButton["B"..buttonNum]:ClearCallback( Mouse.eLClick )
		end
	end
	if turnLabel then
		if researchPerTurn > 0 and g_scienceEnabled then
			turnLabel:SetText( turnText )
			turnLabel:SetHide( false )
		else
			turnLabel:SetHide( true )
		end
	end
end

local function RefreshDisplayNow()
	g_RefreshRequested = false
	ContextPtr:ClearUpdate()

	for tech in GameInfo.Technologies() do
		RefreshDisplayOfSpecificTech( tech )
	end

	-- update the era panels
	local highestEra = 0
	for thisEra = 0, #g_eraBlocks do
		if g_eraColumns[thisEra] and g_eraColumns[thisEra].researched then
			highestEra = thisEra
		end
	end
	for thisEra = 0, #g_eraBlocks do
		local thisEraBlockInstance = g_eraBlocks[thisEra]
		thisEraBlockInstance.OldBar:SetHide( thisEra >= highestEra )
		thisEraBlockInstance.CurrentBlock:SetHide( thisEra ~= highestEra )
		thisEraBlockInstance.CurrentTop:SetHide( thisEra ~= highestEra )
		thisEraBlockInstance.FutureBlock:SetHide( thisEra <= highestEra )
	end

	g_NeedsFullRefresh = false
end

local function RefreshDisplay()
	if not g_RefreshRequested then
		g_RefreshRequested = true
		ContextPtr:SetUpdate( RefreshDisplayNow )
	end
end


-------------------------------------------------
-- find the range of columns that each era takes
-------------------------------------------------
for tech in GameInfo.Technologies() do
	local eraID = GameInfoTypes[tech.Era]
	local eraColumn = g_eraColumns[eraID]
	if not eraColumn then
		g_eraColumns[eraID] = { minGridX = tech.GridX, maxGridX = tech.GridX, researched = false }
	else
		if tech.GridX < eraColumn.minGridX then
			eraColumn.minGridX = tech.GridX
		end
		if tech.GridX > eraColumn.maxGridX then
			eraColumn.maxGridX = tech.GridX
		end
	end
end

-------------------------------------------------
-- add the era panels to the background
-------------------------------------------------
local eraBlockX = -g_blockSpacingX-32
for era in GameInfo.Eras() do

	local thisEraBlockInstance = {}
	ContextPtr:BuildInstanceForControl( "EraBlockInstance", thisEraBlockInstance, Controls.EraStack )
	-- store this panel off for later
	g_eraBlocks[era.ID] = thisEraBlockInstance

	-- add the correct text for this era panel
	local localizedLabel = L( era.Description )
	thisEraBlockInstance.OldLabel:SetText( localizedLabel )
	thisEraBlockInstance.CurrentLabel:SetText( localizedLabel )
	thisEraBlockInstance.FutureLabel:SetText( localizedLabel )

	-- adjust the sizes of the era panels
	local eraBlockWidth = 0
	if g_eraColumns[era.ID] then
		eraBlockWidth = g_eraColumns[era.ID].maxGridX * g_blockSpacingX
		eraBlockWidth, eraBlockX = eraBlockWidth - eraBlockX, eraBlockWidth
	end

	thisEraBlockInstance.EraBlock:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.FrameBottom:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.OldBar:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.OldBlock:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.CurrentBlock:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.CurrentBlock1:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.CurrentBlock2:SetSizeX( eraBlockWidth )

	thisEraBlockInstance.CurrentTop:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.CurrentTop1:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.CurrentTop2:SetSizeX( eraBlockWidth )
	thisEraBlockInstance.FutureBlock:SetSizeX( eraBlockWidth )
end

-------------------------------------------------
-- add the tech button pipes
-------------------------------------------------
AddTechPipes( AddNewPipeIconWithoutColor, GameInfo.Technology_PrereqTechs )
AddTechPipes( AddNewPipeIconColored, GameInfo.Technology_ORPrereqTechs )

-------------------------------------------------
-- add the tech buttons
-------------------------------------------------
for tech in GameInfo.Technologies() do
	local thisTechButton = {}
	local techID = tech.ID
	ContextPtr:BuildInstanceForControl( "TechButtonInstance", thisTechButton, Controls.TechTreeScrollPanel )

	-- store this instance off for later
	g_techButtons[techID] = thisTechButton

	-- add the input handler to this button
	thisTechButton.TechButton:SetVoids( techID, -1 )
	thisTechButton.TechButton:RegisterCallback( Mouse.eRClick, TechPedia )
	thisTechButton.TechButton:RegisterCallback( Mouse.eMouseEnter, setTechToolTip )

	if g_scienceEnabled then
		thisTechButton.TechButton:RegisterCallback( Mouse.eLClick, TechSelected )
	end

	-- position
	thisTechButton.TechButton:SetOffsetVal( tech.GridX*g_blockSpacingX + g_blockOffsetX, tech.GridY*g_blockSpacingY + g_blockOffsetY )

	-- name
	local techName = Locale.TruncateString( L(tech.Description), g_maxTechNameLength, true )
	thisTechButton.AlreadyResearchedTechName:SetText( techName )
	thisTechButton.CurrentlyResearchingTechName:SetText( techName )
	thisTechButton.AvailableTechName:SetText( techName )
	thisTechButton.UnavailableTechName:SetText( techName )
	thisTechButton.LockedTechName:SetText( techName )
	thisTechButton.FreeTechName:SetText( techName )

	-- picture
	thisTechButton.TechPortrait:SetHide( not IconHookup( tech.PortraitIndex, 64, tech.IconAtlas, thisTechButton.TechPortrait ) )

	for buttonNum = 1, g_maxSmallButtons do
		thisTechButton["B"..buttonNum]:SetVoid1( techID )
	end
end

-------------------------------------------------
-- Resize the panel to fit the contents, and the scroll bar for the display size
-------------------------------------------------
Controls.TechTreeScrollBar:SetSizeX( Controls.TechTreeScrollPanel:GetSizeX() - 150 )
Controls.EraStack:CalculateSize()
Controls.EraStack:ReprocessAnchoring()
Controls.TechTreeScrollPanel:CalculateInternalSize()

-------------------------------------------------
-- Close Tech Tree
-------------------------------------------------
CloseTechTree = function()
	if g_popupInfoType then
		Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_popupInfoType, 0)
		g_popupInfoType = false
		UI.decTurnTimerSemaphore()
	end
	g_stealingTechTargetPlayerID, g_stealingTechTargetPlayer = -1
	UIManager:DequeuePopup( ContextPtr )
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, CloseTechTree )

-------------------------------------------------
-- Open Tech Tree
-------------------------------------------------
Events.SerialEventGameMessagePopup.Add(
function( popupInfo )
	if popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH then
		g_stealingTechTargetPlayerID = -1
	elseif popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_TECH_TREE or popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_CHOOSE_TECH_TO_STEAL then
		g_stealingTechTargetPlayerID = gk_mode and popupInfo.Data2 or -1
	else
		return
	end
	g_stealingTechTargetPlayer = Players[ g_stealingTechTargetPlayerID ] -- g_activePlayer:GetNumTechsToSteal( g_stealingTechTargetPlayerID ) > 0

	Events.SerialEventGameMessagePopupShown(popupInfo)

	if g_popupInfoType then
		return CloseTechTree()
	else
		g_popupInfoType = popupInfo.Type
		UIManager:QueuePopup( ContextPtr, PopupPriority_InGameUtmost )
		UI.incTurnTimerSemaphore()
		-- initialize scrollbar position
		if Controls.TechTreeScrollPanel:GetScrollValue() == 0 then
			local pPlayer = Players[Game.GetActivePlayer()]
			local techID = pPlayer:GetCurrentResearch()
			local dx = 0
			if techID < 0 then
				techID = Teams[pPlayer:GetTeam()]:GetTeamTechs():GetLastTechAcquired()
				dx = 1
			end
			local tech = GameInfo.Technologies[techID]
			local x = tech and tech.GridX
			if x then
				Controls.TechTreeScrollPanel:SetScrollValue( math_min(1,math_max(0,( (x + dx)*g_blockSpacingX/Controls.TechTreeScrollPanel:GetSizeX() - 0.5) / math_max(1,1/Controls.TechTreeScrollPanel:GetRatio() - 1) ) ) )
			end
		end
		return RefreshDisplay()
	end
end)

-------------------------------------------------
-- Input processing
-------------------------------------------------

ContextPtr:SetInputHandler(
function ( uiMsg, wParam )--, lParam )
	if uiMsg == KeyEvents_KeyDown
		and (wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN)
	then
		CloseTechTree()
		return true
	end
end)

Events.SerialEventResearchDirty.Add( RefreshDisplay )

-------------------------------------------------
-- Initialize active player data
-------------------------------------------------
local function InitActivePlayerData()
	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[g_activePlayerID]
	g_activeCivType = GameInfo.Civilizations[g_activePlayer:GetCivilizationType()].Type
	g_activeTeamID = Game.GetActiveTeam()
	g_activeTeam = Teams[g_activeTeamID]
	g_activeTeamTechs = g_activeTeam:GetTeamTechs()

	-- gather info about this active player's unique units and buldings
	GatherInfoAboutUniqueStuff( g_activeCivType )

	g_NeedsFullRefresh = true
	return RefreshDisplay()
end
InitActivePlayerData()

local function UpdateOptions()
--	g_isAdvisor = not ( g_options and g_options.GetValue and g_options.GetValue( "CityAdvisor" ) == 0 )
	g_isAutoClose = not ( g_options and g_options.GetValue and g_options.GetValue( "AutoClose" ) == 0 )
	return RefreshDisplay()
end
UpdateOptions()
Events.GameOptionsChanged.Add( UpdateOptions )

Events.GameplaySetActivePlayer.Add( function()
	-- So some extra stuff gets re-built on the refresh call
	if g_popupInfoType then
		CloseTechTree() -- so the next active player does not have to
	end
	InitActivePlayerData()
end)

print("Finished loading EUI tech tree",os.clock())
end)

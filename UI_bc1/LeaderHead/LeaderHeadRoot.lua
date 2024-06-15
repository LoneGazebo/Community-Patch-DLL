-- ===========================================================================
-- LeaderHeadRoot.lua
-- written by bc1 for civ5 brave new world and civBE
-- original code was such a mess just rewrote everything
-- ===========================================================================
local print = print
local tostring = tostring
local type = type

local ButtonPopupTypes = ButtonPopupTypes
local ContextPtr = ContextPtr
local Controls = Controls
local CoopWarStates = CoopWarStates
local Events = Events
local FromUIDiploEventTypes = FromUIDiploEventTypes
local GameDefines = GameDefines
local GameInfo = GameInfo
local KeyEvents = KeyEvents
local Keys = Keys
local LuaEvents = LuaEvents
local MajorCivApproachTypes = MajorCivApproachTypes
local Modding = Modding
local Mouse = Mouse
local Path = Path
local Players = Players
local PopupPriority = PopupPriority
local Teams = Teams
local UI = UI
local UIManager = UIManager

local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil
local civ5_mode = InStrategicView ~= nil
local civBE_mode = not civ5_mode

include( "InstanceManager" )
include( "GameplayUtilities" )
local GameplayUtilities = GameplayUtilities

include( "EUI_utilities" )
local IconHookup = EUI.IconHookup
local CivIconHookup = EUI.CivIconHookup
include( "EUI_tooltips" )
local GetMoodInfo = EUI.GetMoodInfo

local DiploUIStateTypes = DiploUIStateTypes
local L = Locale.ConvertTextKey

local g_InstanceManager = InstanceManager:new( "Leader Button", "Button", Controls.LeaderStack )

local g_canGoBack = true

local g_isHumanInitiatedTrade = false

local g_UICursor = 0
local g_diploUIstateID, g_diploProcess, g_diploProcesses, g_diploMessage, g_diploData
local g_activePlayerID, g_activePlayer, g_activeTeamID, g_activeTeam, g_isAtWar
local g_diploPlayerID, g_diploPlayer, g_diploPlayerName, g_diploTeamID, g_diploTeam

--local g_strLeaveScreenText = L("TXT_KEY_DIPLOMACY_ANYTHING_ELSE")
--local g_strDefaultDiscussionText = L("TXT_KEY_DIPLOMACY_DISCUSS_WHAT")

local g_isAnimateOutComplete = false
local g_isAnimatingIn = false
local g_bRootWasShownThisEvent = false

local g_tipControls = {}
TTManager:GetTypeControlTable( "EUI_CivilizationTooltip", g_tipControls )
Controls.MoodText:SetToolTipType( "EUI_CivilizationTooltip" )
Controls.MoodText:SetToolTipCallback( function()
		g_tipControls.Text:SetText( GetMoodInfo( g_diploPlayerID, true ) )
		local leader = GameInfo.Leaders[g_diploPlayer:GetLeaderType()]
		g_tipControls.PortraitFrame:SetHide( not IconHookup( leader.PortraitIndex, g_tipControls.Portrait:GetSizeY(), leader.IconAtlas, g_tipControls.Portrait ) )
		CivIconHookup( g_diploPlayerID, g_tipControls.CivIconBG:GetSizeY(), g_tipControls.CivIcon, g_tipControls.CivIconBG, g_tipControls.CivIconShadow, false, true )
		g_tipControls.Box:SetHide( false )
		g_tipControls.Box:DoAutoSize()
end)

-- ===========================================================================
-- Coop War, Denounce, War & Peace
-- ===========================================================================
local function IsThirdPartyPlayerValid( thirdPartyPlayerID )

	local thirdPartyPlayer = Players[ thirdPartyPlayerID ]
	local thirdPartyTeamID = thirdPartyPlayer:GetTeam()

	-- Can't be someone on our team
	return g_activeTeamID ~= thirdPartyTeamID
	-- Has to be alive
		and thirdPartyPlayer:IsAlive()
	-- Can't be the guy we're already talking to
		and g_diploTeamID ~= thirdPartyTeamID
	-- Active player met them?
		and g_activeTeam:IsHasMet( thirdPartyTeamID )
	-- Other player met them?
		and g_diploTeam:IsHasMet( thirdPartyTeamID )
end

local function IsWorkingAgainstThirdPartyPlayerValid( thirdPartyPlayerID )
	-- Have we already agreed?
	return IsThirdPartyPlayerValid( thirdPartyPlayerID )
		and not g_diploPlayer:IsWorkingAgainstPlayerAccepted( g_activePlayerID, thirdPartyPlayerID )
end

local function IsWarAgainstThirdPartyPlayerValid( thirdPartyPlayerID )

	if IsThirdPartyPlayerValid( thirdPartyPlayerID ) then
		-- Have we already agreed and is a DOW valid against this target?
		local coopWarAcceptedState = g_diploPlayer:GetCoopWarAcceptedState( g_activePlayerID, thirdPartyPlayerID )
		return coopWarAcceptedState ~= CoopWarStates.COOP_WAR_STATE_ACCEPTED
			and coopWarAcceptedState ~= CoopWarStates.COOP_WAR_STATE_SOON
			and g_diploTeam:CanDeclareWar( Players[thirdPartyPlayerID]:GetTeam() )
	end
end

local function OnLeaderSelect( thirdPartyPlayerID )
	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_OFFER, g_diploPlayerID, thirdPartyPlayerID, -1 )
end

-- ===========================================================================
--	Request from use to close; either a button or the keyboard.
-- ===========================================================================

local function ClosePanels()
	Controls.BackButton:SetHide( true )
	Controls.LeaderStack:SetHide( true )
	Controls.DenounceConfirm:SetHide( true )
	Controls.WarConfirm:SetHide( true )
end

local function DoLeaveLeader()
	ClosePanels()
	g_isHumanInitiatedTrade = false
	g_diploUIstateID = false --DiploUIStateTypes.NO_DIPLO_UI_STATE
	g_diploProcess = false
	g_bRootWasShownThisEvent = false
	if civ5_mode then
		UIManager:DequeuePopup( ContextPtr )
	else
		ContextPtr:SetHide( true )
	end
end

local function RequestLeaveLeader()
	DoLeaveLeader()
	UI.RequestLeaveLeader()
end

local function ForceLeaveLeader()
	UI.SetLeaderHeadRootUp( false )
	RequestLeaveLeader()
end
Events.LeavingLeaderViewMode.Add( DoLeaveLeader )

local function ButtonClick( buttonID, _, control )
	do print( "clicked discussion dialogue button#", buttonID, control:GetText(), g_diploProcess ) end
	if g_diploProcess and g_diploPlayer and g_diploPlayer:IsAlive() then
		local diploEvent = g_diploProcess[2]
		local diploEventType = type( diploEvent )
		do print( "click action", diploEvent ) end
		if diploEventType == "number" then
			Game.DoFromUIDiploEvent( diploEvent, g_diploPlayerID, buttonID, g_diploData )
		elseif diploEventType == "function" then
			diploEvent( buttonID )
		end
	end
end
for i = 1, 8 do
	Controls["Button"..i]:RegisterCallback( Mouse.eLClick, ButtonClick )
end

local function SetLeaderSpeech( diploMessage )
	Controls.LeaderSpeech:SetText( diploMessage )
	-- Resize the height of the box to fit the text
	local contentSize = Controls.LeaderSpeech:GetSize().y + 48 --offsetOfString + bonusPadding
	Controls.LeaderSpeechBorderFrame:SetSizeY( contentSize )
	Controls.LeaderSpeechFrame:SetSizeY( contentSize - 4 ) --offsetsBetweenFrames
end

local g_moodText = {
	[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR ] = "TXT_KEY_WAR_CAPS",
	[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE ] = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_HOSTILE",
	[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED ] = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_GUARDED",
	[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID ] = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_AFRAID",
	[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY ] = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_FRIENDLY",
	[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL ] = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_NEUTRAL",
}

-- ===========================================================================
--	LEADER MESSAGE PROCESSOR
-- ===========================================================================
local function SetupDisplay( diploPlayerID, diploUIstateID, diploMessage, diploData, isDiscussMode )

--local dps, pdps;for k,v in pairs(DiploUIStateTypes) do if v==diploUIstateID then dps=k end if v==g_diploUIstateID then pdps=k end end; diploMessage = "Previous: "..tostring(g_diploUIstateID)..tostring(pdps).."[NEWLINE]Current: "..tostring(diploUIstateID)..tostring(dps)..tostring(diploData).."[NEWLINE]"..diploMessage

	Controls.DiscussButton:SetHide( isDiscussMode )
	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[ g_activePlayerID ]
	g_activeTeamID = Game.GetActiveTeam()
	g_activeTeam = Teams[ g_activeTeamID ]

	g_diploPlayerID = diploPlayerID
	g_diploUIstateID = diploUIstateID
	g_diploMessage = diploMessage
	g_diploData = diploData
	g_diploPlayer = Players[diploPlayerID]
	g_diploTeamID = g_diploPlayer:GetTeam()
	g_diploTeam = Teams[g_diploTeamID]
	g_diploPlayerName = g_diploPlayer:GetNickName()
	if g_diploPlayerName == "" or not Game:IsNetworkMultiPlayer() then
		g_diploPlayerName = g_diploPlayer:GetName()
	end
	g_isAtWar = g_activeTeam:IsAtWar( g_diploTeamID )
	local isAlive = g_diploPlayer and g_diploPlayer:IsAlive()

--	local diploCivInfo = GameInfo.Civilizations[ g_diploPlayer:GetCivilizationType() ]
	if civ5_mode then
		CivIconHookup( diploPlayerID, 64, Controls.ThemSymbolShadow, Controls.CivIconBG, Controls.CivIconShadow, false, true )
	else
		CivIconHookup( diploPlayerID, 45, Controls.CivIcon, Controls.CivIconBG, nil, false, false, Controls.CivIconHighlight )
	end

	Controls.TitleText:SetText( GameplayUtilities.GetLocalizedLeaderTitle( g_diploPlayer ) )

	-- Mood
	local moodText
	if g_isAtWar then
		moodText = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR"
	elseif g_diploPlayer:IsDenouncingPlayer(g_activePlayerID) then
		moodText = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_DENOUNCING"
	elseif gk_mode and g_diploPlayer:WasResurrectedThisTurnBy(g_activePlayerID) then
		moodText = "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_LIBERATED"
	else
		moodText = g_moodText[ g_activePlayer:GetApproachTowardsUsGuess(diploPlayerID) ] or "TXT_KEY_EMOTIONLESS"
	end
	local diploLeader = GameInfo.Leaders[ g_diploPlayer:GetLeaderType() ]
	Controls.MoodText:LocalizeAndSetText( moodText, diploLeader.Description )
	-- /Mood

	SetLeaderSpeech( diploMessage )

	--Initialize buttons
	for i = 1, 8 do
		local button = Controls["Button"..i]
		local label
		label = g_diploProcess[ i + 2 ]
		button:SetText( label )
		button:SetDisabled( false )
		button:SetToolTipString()
		button:SetHide( not label )
	end

	--Show screen
	g_canGoBack = not g_diploProcess[3] -- !!! kinda hackish !!!
	if g_canGoBack then
		Controls.WarButton:SetHide( not g_activeTeam:CanChangeWarPeace(g_diploTeamID) )
		Controls.DemandButton:SetHide( g_activeTeamID == g_diploTeamID )
		Controls.TradeButton:SetDisabled( not isAlive or g_isAtWar )
		Controls.DemandButton:SetDisabled( not isAlive or g_isAtWar )
		Controls.DiscussButton:SetDisabled( not isAlive or g_isAtWar )
		local isDisabled = true
		if g_isAtWar then
			Controls.WarButton:LocalizeAndSetText( "TXT_KEY_DIPLO_NEGOTIATE_PEACE" )
			local numTurnsLockedIntoWar = g_activeTeam:GetNumTurnsLockedIntoWar( g_diploTeamID )
			-- Not locked into war
			if isAlive and numTurnsLockedIntoWar == 0 then
				isDisabled = false
				Controls.WarButton:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_TT" )
			-- Locked into war
			else
				Controls.WarButton:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_BLOCKED_TT", numTurnsLockedIntoWar )
			end
		else
			Controls.WarButton:LocalizeAndSetText( "TXT_KEY_DIPLO_DECLARE_WAR" )
			if not isAlive then
				Controls.WarButton:SetToolTipString()
			elseif g_activeTeam:IsForcePeace(g_diploTeamID) then
				Controls.WarButton:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_MAY_NOT_ATTACK" )
			elseif not g_activeTeam:CanDeclareWar(g_diploTeamID) then
				Controls.WarButton:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_MAY_NOT_ATTACK_MOD" )
			else
				isDisabled = false
				Controls.WarButton:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_DECLARES_WAR_TT" )
			end
		end
		Controls.WarButton:SetDisabled( isDisabled )
		if civBE_mode then
			Controls.TalkOptionStack:CalculateSize()
			Controls.TalkOptionStack:ReprocessAnchoring()
		end
	end
	Controls.ButtonStack:SetHide( not g_canGoBack )
	ClosePanels()
	Controls.PrimaryStack:CalculateSize()
	Controls.PrimaryStack:ReprocessAnchoring()

	if ContextPtr:IsHidden() then
		if civ5_mode then
			UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderDiscuss )
		else
			-- Whether it's handled here or in a leaderhead sub-screen, the root context should be visible if a
			-- leaderhead message is coming through...
			-- If animations need to kick off (coming from InGame) let that be signaled by the transition manager.
			ContextPtr:SetHide( false )
		end
	end
end

local function SetupRootMode( ... )
	UI.SetLeaderHeadRootUp( true )
	SetupDisplay( ... )
end

local function SetupDisplayWithMinorTooltips(...)
	SetupDisplay(...)
	local minorPlayer = gk_mode and Players[ g_diploData ]
	if minorPlayer and g_diploPlayer then
		local minorName = minorPlayer:GetCivilizationShortDescriptionKey()
		local diploName = g_diploPlayer:GetCivilizationShortDescriptionKey()
		Controls.Button1:LocalizeAndSetToolTip("TXT_KEY_DIPLO_DISCUSS_PROTECTED_MINOR_FORGIVE_TT", diploName, minorName, -(GameDefines.MINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT / 100) ) -- XML is times 100 for fidelity, and negative
		Controls.Button2:LocalizeAndSetToolTip("TXT_KEY_DIPLO_DISCUSS_PROTECTED_MINOR_DISPUTE_TT", diploName, minorName )
	end
end

-- ===========================================================================
--	LEADER MESSAGE HANDLER
-- ===========================================================================
local function AILeaderMessage( diploPlayerID, diploUIstateID, diploMessage, animationActionID, diploData )
	do print( "Handling LeaderMessage: ", diploPlayerID, diploUIstateID, diploMessage, diploData ) end
	local diploProcess = g_diploProcesses[ diploUIstateID ]
	if diploProcess then
		g_diploProcess = diploProcess
		return diploProcess[1]( diploPlayerID, diploUIstateID, diploMessage, diploData )
	end
end
Events.AILeaderMessage.Add( AILeaderMessage )

local isAILeaderMessageNotRestarted = true
local function AILeaderMessageRestartChangeMessage( message )
	do print( "AILeaderMessageRestart", isAILeaderMessageNotRestarted, g_diploPlayerID, g_diploUIstateID, message, g_diploData ) end
	if g_diploProcess and isAILeaderMessageNotRestarted then
		isAILeaderMessageNotRestarted = false
		g_diploProcess[1]( g_diploPlayerID, g_diploUIstateID, message, g_diploData )
	end
	isAILeaderMessageNotRestarted = true
end
local function AILeaderMessageRestart()
	return AILeaderMessageRestartChangeMessage( L"TXT_KEY_DIPLOMACY_DISCUSS_WHAT" )
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, AILeaderMessageRestart )
Events.WarStateChanged.Add(
function( iTeam1, iTeam2, isAtWar )
	-- Active player changed war state with this AI
	if iTeam1 == g_activeTeamID and iTeam2 == g_diploTeamID then
		AILeaderMessageRestart()
	end
end)
local function WarConfirm( isExit )
	if bnw_mode and not isExit then
		UI.AddPopup{ Type = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARMOVE, Data1 = g_diploTeamID, Option1 = true }
	else
		Controls.WarConfirmYes:SetVoids( isExit and 1 or 0, g_diploPlayerID )
		Controls.WarConfirm:SetHide( false )
	end
end
Controls.WarConfirmNo:RegisterCallback( Mouse.eLClick, AILeaderMessageRestart )
Controls.WarConfirmYes:RegisterCallback( Mouse.eLClick,
function( arg )
	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR, g_diploPlayerID, arg, 0) -- 0 leader root, 1 discuss
end)
Controls.DenounceConfirmNo:RegisterCallback( Mouse.eLClick, AILeaderMessageRestart )
Controls.DenounceConfirmYes:RegisterCallback( Mouse.eLClick,
function()
	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DENOUNCE, g_diploPlayerID, 0, 0 )
end)

Controls.GoodByeButton:RegisterCallback( Mouse.eLClick, ForceLeaveLeader )

Controls.TradeButton:RegisterCallback( Mouse.eLClick,
function()
--	Controls.LeaderSpeech:LocalizeAndSetText( "TXT_KEY_DIPLOMACY_ANYTHING_ELSE" )		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
	-- This calls into CvDealAI and sets up the initial state of the UI
	g_diploPlayer:DoTradeScreenOpened()
	UI.OnHumanOpenedTradeScreen( g_diploPlayerID )
end)

Controls.DemandButton:RegisterCallback( Mouse.eLClick,
function()
--	Controls.LeaderSpeech:LocalizeAndSetText( "TXT_KEY_DIPLOMACY_ANYTHING_ELSE" )		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
	UI.OnHumanDemand( g_diploPlayerID )
end)

Controls.DiscussButton:RegisterCallback( Mouse.eLClick,
function()
	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_WANTS_DISCUSSION, g_diploPlayerID, 0, 0 )
end)

Controls.WarButton:RegisterCallback( Mouse.eLClick,
function()
	if g_activeTeam:IsAtWar( g_diploTeamID ) then
	-- Asking for Peace (currently at war) - bring up the trade screen
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_NEGOTIATE_PEACE, g_diploPlayerID, 0, 0 )
	-- Declaring War (currently at peace)
	else
		WarConfirm()
	end
end)

-- ===========================================================================
--	SHOW/HIDE
-- ===========================================================================
ContextPtr:SetShowHideHandler(
function( isHide, isInit )
	if isInit then
		if civBE_mode then
			-- set blackbar based on %
			local screenWidth, screenHeight = UIManager:GetScreenSizeVal()
			local blackBarTopSize			= (screenHeight * .20) / 2		-- slightly less, male model's head is cropped otherwise in min-spec
			local blackBarBottomSize		= (screenHeight * .26) / 2
			Controls.BlackBarTop:SetSizeY( blackBarTopSize )
			Controls.BlackBarBottom:SetSizeY( blackBarBottomSize )
			Controls.AnimBarTop:SetBeginVal(0, -blackBarTopSize)
			Controls.AnimBarBottom:SetBeginVal(0, blackBarBottomSize)
		end
	elseif isHide then
		UIManager:SetUICursor(g_UICursor) -- hiding screen: make sure we retrun the cursor to the previous state
		-- Do not attempt to set default leader text here; it's possible for this same screen to be queued.
	else
		g_UICursor = UIManager:SetUICursor(0) -- showing screen: make sure we start with the default cursor
		LuaEvents.EUILeaderHeadRoot()
--		if civ5_mode then
--			Controls.RootOptions:SetHide( not UI.GetLeaderHeadRootUp() )
--		end
--		AILeaderMessageRestart()
	end
end)

-- ===========================================================================
-- Key Down Processing
-- ===========================================================================
ContextPtr:SetInputHandler(
function( uiMsg, wParam )--, lParam )
	if uiMsg == KeyEvents.KeyDown and ( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
		if Controls.DenounceConfirm:IsHidden() and Controls.WarConfirm:IsHidden() and Controls.LeaderStack:IsHidden() then
			if g_canGoBack then
				ForceLeaveLeader()
			end
		else
			AILeaderMessageRestart()
--			Controls.WarConfirm:SetHide(true)
		end
	end
	return true
end)

-- ===========================================================================

if civBE_mode then
	-- ===========================================================================
	--	Animate all controls for when the screen first comes up
	--	Kicked off by the game engine
	-- ===========================================================================
	function OnAnimateIn()

		if g_isAnimatingIn then
			return
		end

		-- Reset
		g_isAnimatingIn = true
		Controls.AnimBarTop:RegisterAnimCallback( function() end )
		Controls.AnimBarBottom:RegisterAnimCallback( function() end )
		Controls.GamestateTransitionAnimOut:SetToBeginning()

		Controls.AnimBarTop:SetToBeginning()
		Controls.AnimBarBottom:SetToBeginning()
		Controls.AnimAlphaTop:SetToBeginning()
		Controls.AnimAlphaBottom:SetToBeginning()

		Controls.AnimBarTop:Play()
		Controls.AnimBarBottom:Play()
		Controls.AnimAlphaTop:Play()
		Controls.AnimAlphaBottom:Play()
		Controls.BackButton:SetDisabled( false )
	end
	Controls.GamestateTransitionAnimIn:RegisterAnimCallback( OnAnimateIn )

	-- ===========================================================================
	--	Animate all controls for when the screen is being dismissed
	-- ===========================================================================
	function OnAnimateOut()

		-- Reset variables for next animation in.
		g_isAnimatingIn			= false
		g_isAnimateOutComplete	= false
		Controls.GamestateTransitionAnimIn:SetToBeginning()

		-- Play controls backwards

		Controls.AnimBarTop:Reverse()
		Controls.AnimBarTop:Play()
		Controls.AnimBarTop:RegisterAnimCallback( OnUpdateAnimate )

		Controls.AnimBarBottom:Reverse()
		Controls.AnimBarBottom:Play()
		Controls.AnimBarBottom:RegisterAnimCallback( OnUpdateAnimate )

		Controls.AnimAlphaTop:Reverse()
		Controls.AnimAlphaTop:Play()

		Controls.AnimAlphaBottom:Reverse()
		Controls.AnimAlphaBottom:Play()

		Controls.BackButton:SetDisabled( true )
	end
	Controls.GamestateTransitionAnimOut:RegisterAnimCallback( OnAnimateOut )

	-- ===========================================================================
	--	Callback, per-frame, for animation
	-- ===========================================================================
	function OnUpdateAnimate()
		if Controls.AnimBarTop:IsStopped() and Controls.AnimBarBottom:IsStopped() then
			OnAnimateOutComplete()
		end
	end

	-- ===========================================================================
	--	Called once when completed animating out
	-- ===========================================================================
	function OnAnimateOutComplete()
		if g_isAnimateOutComplete then
			return
		end

		g_isAnimateOutComplete	= true
		ContextPtr:SetHide( true )
		UIManager:DequeuePopup( ContextPtr )
	end


	-- ===========================================================================
	--	Raised from sub-screens when they are closed
	-- ===========================================================================
	function OnLeaderheadPopupClosed()

		-- If the popup screen was raised through the menu, just bring the menu (root) options back
		-- otherwise dismiss the whole leaderhead system because the pop-up was directly invoked.
		if g_bRootWasShownThisEvent then
			Controls.RootOptions:SetHide( false )
		else
			UI.SetLeaderHeadRootUp( false )
			UI.RequestLeaveLeader()
		end
	end
	LuaEvents.LeaderheadPopupClosed.Add( OnLeaderheadPopupClosed )


	-- ===========================================================================
	--	Raised from sub-screens as they exit (e.g., Trade logic)
	-- ===========================================================================
	function OnLeaderheadShow()
		-- If this wasn't shown, ignore the event as it may be raised by shared
		-- subscreend (e.g., tradelogic in multiplayer.)
		if g_bRootWasShownThisEvent then
			Controls.RootOptions:SetHide( false )
			ContextPtr:SetHide( false )
		else
			LuaEvents.LeaderheadPopupClosed()
		end
	end
	LuaEvents.LeaderheadRootShow.Add( OnLeaderheadShow )
end

-- ===========================================================================
--	DIPLO PROCESSES TABLE
--	1. screen setup routine when DiploUIStateTypes is triggered
--	2. action when button 1..8 is left clicked
--	3-11. button 1..8 static legend !!! hackish: set 3 nil to allow back !!!
-- ===========================================================================

g_diploProcesses = {
[DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT or false] = { SetupRootMode },
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED or false] =
{
	function( diploPlayerID, diploUIstateID, diploMessage, diploData )

		SetupDisplay( diploPlayerID, diploUIstateID, diploMessage, diploData, true )

		-- Share intrigue
		if gk_mode and g_activePlayer:HasRecentIntrigueAbout( g_diploPlayerID ) and not g_diploPlayer:IsHuman() then
			Controls.Button1:LocalizeAndSetText( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_SHARE_INTRIGUE", g_diploPlayerName )
			Controls.Button1:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_SHARE_INTRIGUE_TT", g_diploPlayerName )
			Controls.Button1:SetHide(false)
		end

		-- Ask the AI player to stop spying
		Controls.Button2:SetHide( not gk_mode or g_diploPlayer:IsStopSpyingMessageTooSoon(g_activePlayerID) or #(g_diploPlayer:GetEspionageSpies()) == 0 or g_diploTeamID == Game.GetActiveTeam() )

		-- Ask the AI player not to send missionaries or prophets
		Controls.Button3:SetHide( not gk_mode or g_activePlayer:GetNegativeReligiousConversionPoints(g_diploPlayerID) <= 0 or g_diploPlayer:IsAskedToStopConverting(g_activePlayerID) )

		-- Ask the AI player to not settle nearby
		Controls.Button4:SetHide( g_diploPlayer:IsDontSettleMessageTooSoon(g_activePlayerID) )

		-- Ask the AI player not to dig up my artifacts
		Controls.Button5:SetHide( not bnw_mode or g_activePlayer:GetNegativeArchaeologyPoints(g_diploPlayerID) <= 0 or g_diploPlayer:IsAskedToStopDigging(g_activePlayerID) )

		-- If we're teammates, there's no need to work together or against anyone
		if g_diploTeamID == g_activeTeamID then
			Controls.Button6:SetHide( true )
		else
			-- Ask the AI player to work together
			if g_diploPlayer:IsDoF(g_activePlayerID) or g_diploPlayer:IsDoFMessageTooSoon(g_activePlayerID) then
				Controls.Button6:SetHide( true )
			else
				Controls.Button6:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP_TT" )
			end

			if not g_activePlayer:IsDenouncedPlayer(g_diploPlayerID) then
				Controls.Button7:LocalizeAndSetText( "TXT_KEY_DIPLO_DISCUSS_DENOUNCE", g_diploPlayerName )
				Controls.Button7:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_DISCUSS_DENOUNCE_TT" )
				Controls.Button7:SetHide( false )
			end
		end

		local disableButton8 = true
		-- Discussion buttons valid?
		for thirdPartyPlayerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do

			-- War button
			if IsWarAgainstThirdPartyPlayerValid( thirdPartyPlayerID ) then
				disableButton8 = false
				break
			end
		end
		Controls.Button8:SetDisabled( disableButton8 )
		Controls.PrimaryStack:CalculateSize()
		Controls.PrimaryStack:ReprocessAnchoring()
	end,
	function( buttonID )
		if buttonID == 1 then
			if gk_mode and g_activePlayer:HasRecentIntrigueAbout(g_diploPlayerID) and not g_diploPlayer:IsHuman() then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_SHARE_INTRIGUE, g_diploPlayerID, g_activePlayer:GetRecentIntrigueInfo(g_diploPlayerID) )
			end
		elseif buttonID == 2 then
			if gk_mode and not g_diploPlayer:IsStopSpyingMessageTooSoon(g_activePlayerID) and #(g_diploPlayer:GetEspionageSpies()) ~= 0 and g_diploTeamID ~= Game.GetActiveTeam() then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING, g_diploPlayerID, 0, 0 )
			end
		elseif buttonID == 3 then
			if gk_mode and g_activePlayer:GetNegativeReligiousConversionPoints(g_diploPlayerID) > 0 and not g_diploPlayer:IsAskedToStopConverting(g_activePlayerID) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPREADING_RELIGION, g_diploPlayerID, 0, 0 )
			end
		elseif buttonID == 4 then
			if not g_diploPlayer:IsDontSettleMessageTooSoon(g_activePlayerID) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_DONT_SETTLE, g_diploPlayerID, 0, 0 )
			end
		elseif buttonID == 5 then
			if bnw_mode and g_activePlayer:GetNegativeArchaeologyPoints(g_diploPlayerID) > 0 and not g_diploPlayer:IsAskedToStopDigging(g_activePlayerID) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_DIGGING, g_diploPlayerID, 0, 0 )
			end
		elseif buttonID == 6 then
			if not g_diploPlayer:IsDoF(g_activePlayerID) and not g_diploPlayer:IsDoFMessageTooSoon(g_activePlayerID) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_WORK_WITH_US, g_diploPlayerID, 0, 0 )
			end
		elseif buttonID == 7 then
			if not g_activePlayer:IsDenouncedPlayer(g_diploPlayerID) then
				Controls.DenounceLabel:LocalizeAndSetText("TXT_KEY_CONFIRM_DENOUNCE", g_diploPlayerName )
				Controls.DenounceConfirm:SetHide(false)
			end
		elseif buttonID == 8 then
			g_InstanceManager:ResetInstances()
			local numPlayers = 0

			-- Loop through all the Majors the active player knows
			for thirdPartyPlayerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do
				local thirdPartyPlayer = Players[ thirdPartyPlayerID ]

				if thirdPartyPlayer and IsWarAgainstThirdPartyPlayerValid(thirdPartyPlayerID) then
					numPlayers = numPlayers + 1
					local instance = g_InstanceManager:GetInstance()
					instance.Button:SetText( thirdPartyPlayer:GetName() )
					instance.Button:SetVoid1( thirdPartyPlayerID ) -- leader ID
					instance.Button:RegisterCallback( Mouse.eLClick, OnLeaderSelect )
				end
			end

			-- If there's no one to talk about then don't do anything
			if numPlayers > 0 then
				-- Time to show the leaders!
				Controls.LeaderStack:CalculateSize()
				Controls.LeaderStack:ReprocessAnchoring()
				Controls.LeaderStack:SetHide( false )
				Controls.BackButton:SetHide( false )
				for i = 1, 8 do
					Controls["Button"..i]:SetHide( true )
				end
				Controls.ButtonStack:SetHide( true )
				Controls.PrimaryStack:ReprocessAnchoring()
				SetLeaderSpeech( L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_DECLARE_WAR" )
			end
		end
		Controls.PrimaryStack:CalculateSize()
		Controls.PrimaryStack:ReprocessAnchoring()
	end,
	nil,--1 !!! hackish - must stay nil for can go back !!!
	gk_mode and L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_STOP_SPYING",--2
	gk_mode and L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_STOP_SPREADING_RELIGION",--3
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_DONT_SETTLE",--4
	bnw_mode and L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_STOP_DIGGING",--5
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP",--6
	nil,--7
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_DECLARE_WAR",--8
},
[DiploUIStateTypes.DIPLO_UI_STATE_PEACE_MADE_BY_HUMAN or false] = { SetupRootMode },
[DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN or false] = { SetupRootMode },
--[[
{
	function( diploPlayerID, diploUIstateID, diploMessage, diploData )
		do print("DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN") end
		if diploData == 1 then
			do print("DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN activated in DiscussionDialog") end
			return SetupDisplay( diploPlayerID, diploUIstateID, diploMessage, diploData )
		else
			do print("DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN NOT activated in DiscussionDialog") end
--			g_diploUIstateID = DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN
		end
	end,
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS or false] = {
},
[DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND or false] = {
},
--]]
[DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT or false] = { SetupDisplay },
[DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION or false] = {
	function( diploPlayerID, diploUIstateID, diploMessage, diploData )
		local diploProcess = g_diploProcesses[ g_diploUIstateID ]
		do print( "blank discussion", diploProcess and diploUIstateID ~= g_diploUIstateID and g_canGoBack and diploPlayerID == g_diploPlayerID, diploProcess, diploUIstateID, g_diploUIstateID, g_canGoBack, diploPlayerID, g_diploPlayerID ) end
-- todo - there is some bug with this
		if false and UI.GetLeaderHeadRootUp() and diploProcess and diploUIstateID ~= g_diploUIstateID and g_canGoBack and diploPlayerID == g_diploPlayerID then
			g_diploProcess = diploProcess
			AILeaderMessageRestartChangeMessage( diploMessage )
		else
			return SetupDisplay( diploPlayerID, diploUIstateID, diploMessage, diploData )
		end
	end,
},
[DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN or false] = {
	SetupDisplay,
	RequestLeaveLeader, -- fluff discussion mode
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_SORRY",
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEAL",
},
[DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI or false] = {
	SetupDisplay,
	RequestLeaveLeader, -- fluff discussion mode
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_OKAY",
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_YOULL_PAY",
},
[DiploUIStateTypes.DIPLO_UI_STATE_AI_DECLARED_WAR or false] = {
	function( diploPlayerID, diploUIstateID, diploMessage, diploData )
		return SetupDisplay( diploPlayerID, diploUIstateID, tostring(diploMessage).."[NEWLINE][NEWLINE](" .. L"TXT_KEY_WAR_DECLARED" .. ")", diploData )
	end,
	RequestLeaveLeader, -- fluff discussion mode
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_OKAY",
	L"TXT_KEY_DIPLO_DISCUSS_MESSAGE_YOULL_PAY",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_AGGRESSIVE_MILITARY_WARNING_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_WE_MEAN_NO_HARM",
	L"TXT_KEY_DIPLO_DISCUSS_TIME_TO_DIE",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV or false] = {
	SetupDisplayWithMinorTooltips,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_I_ATTACKED_YOUR_MINOR_CIV_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NO_DIVIDE",
	L"TXT_KEY_DIPLO_DISCUSS_YOU_WILL_PAY",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV or false] = {
	SetupDisplayWithMinorTooltips,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_I_BULLIED_YOUR_MINOR_CIV_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NO_DIVIDE",
	L"TXT_KEY_DIPLO_DISCUSS_YOU_WILL_PAY",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_ATTACKED_MINOR_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NOT_YOUR_BUSINESS",
	L"TXT_KEY_DIPLO_DISCUSS_WILL_WITHDRAW",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_DONT_CARE",
	L"TXT_KEY_DIPLO_DISCUSS_OFFER_TO_APPEASE",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_BULLIED_MINOR_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NOT_YOUR_BUSINESS",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_BULLYING",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PROTECT_MINOR_CIV or false] = { SetupDisplayWithMinorTooltips },
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_MAKE_US",
	L"TXT_KEY_DIPLO_DISCUSS_OFFER_TO_APPEASE",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_WARNING_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_SETTLE_WHAT_WE_PLEASE",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_SETTLING",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_CLAIM_WHAT_WE_WANT",
	L"TXT_KEY_DIPLO_DISCUSS_OFFER_TO_APPEASE",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_WARNING_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NOT_YOUR_BUSINESS",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_CLAIMING",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_WITH_US or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_WITH_US_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_YES_WORK_TOGETHER",
	L"TXT_KEY_DIPLO_DISCUSS_NO_GO_IT_ALONE",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_NO_INTEREST",
	L"TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU",
	L"TXT_KEY_DIPLO_DISCUSS_DO_WHAT_WE_CAN",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_NO_INTEREST",
	L"TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU",
	L"TXT_KEY_DIPLO_DISCUSS_COOP_WAR_SOON",
	L"TXT_KEY_DIPLO_DISCUSS_COOP_WAR_YES",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_NOW_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_COOP_WAR_NOW",
	L"TXT_KEY_DIPLO_DISCUSS_CHANGED_MIND",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLAN_RA_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_YES_LET_US_PREPARE",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_OTHER_PLANS",
},
[DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_AI_REQUEST_DENOUNCE_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_DO_WHAT_WE_CAN",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_NO_INTEREST",
},
[DiploUIStateTypes.DIPLO_UI_STATE_CAUGHT_YOUR_SPY or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NOT_SORRY_FOR_SPY_CAUGHT",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_SPY_CAUGHT",
},
[DiploUIStateTypes.DIPLO_UI_STATE_KILLED_YOUR_SPY or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_NOT_SORRY_FOR_SPY_KILLED",
	L"TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_SPY_KILLED",
},
[DiploUIStateTypes.DIPLO_UI_STATE_KILLED_MY_SPY or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE,
	L"TXT_KEY_DIPLO_DISCUSS_DONT_FORGIVE_SPYING",
	L"TXT_KEY_DIPLO_DISCUSS_FORGIVE_SPYING",
},
[DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY or false] = {
	function(...)
		SetupDisplay(...)
		Controls.Button2:SetHide( g_diploPlayer:IsStopSpyingMessageTooSoon( g_activePlayerID ) )
		Controls.Button3:SetHide( g_activePlayer:IsDenouncedPlayer( g_diploPlayerID ) )
		Controls.Button3:LocalizeAndSetText( "TXT_KEY_DIPLO_DISCUSS_CAUGHT_DENOUNCE_FOR_SPYING", g_diploPlayerName )
		Controls.Button4:SetHide( g_isAtWar )
		Controls.Button4:LocalizeAndSetText( "TXT_KEY_DIPLO_DISCUSS_CAUGHT_DECLARE_WAR", g_diploPlayerName )
		if g_activeTeam:IsForcePeace( g_diploTeamID ) then
			Controls.Button4:SetDisabled( true )
			Controls.Button4:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_MAY_NOT_ATTACK" )
		end
	end,
	function( buttonID )
		if buttonID == 1 then
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE, g_diploPlayerID, 2, g_diploData )
		elseif buttonID == 2 then
			-- Ask the AI player not to spy any more
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING, g_diploPlayerID, 1, 0 )
		elseif buttonID == 3 then
			if g_diploPlayer then
				Controls.DenounceLabel:LocalizeAndSetText("TXT_KEY_CONFIRM_DENOUNCE", g_diploPlayerName)
				Controls.DenounceConfirm:SetHide(false)
			end
		elseif buttonID == 4 then
			WarConfirm( 1 )
		end
	end,
	L"TXT_KEY_DIPLO_DISCUSS_CAUGHT_FORGIVE_SPYING",--1
	L"TXT_KEY_DIPLO_DISCUSS_CAUGHT_DEMAND_STOP_SPYING",--2
},
[DiploUIStateTypes.DIPLO_UI_STATE_STOP_CONVERSIONS or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_STOP_CONVERSIONS,
	L"TXT_KEY_DIPLO_DISCUSS_DONT_STOP_CONVERSIONS",
	L"TXT_KEY_DIPLO_DISCUSS_STOP_CONVERSIONS",
},
[DiploUIStateTypes.DIPLO_UI_STATE_STOP_DIGGING or false] = {
	SetupDisplay,
	FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_STOP_DIGGING,
	L"TXT_KEY_DIPLO_DISCUSS_DONT_STOP_DIGGING",
	L"TXT_KEY_DIPLO_DISCUSS_STOP_DIGGING",
},
}
g_diploProcesses[ false ] = nil
-- ===========================================================================
-- Support for Modded Add-in UI's
-- ===========================================================================
local g_uiAddins = {}
for addin in Modding.GetActivatedModEntryPoints("DiplomacyUIAddin") do
	local addinFile = Modding.GetEvaluatedFilePath( addin.ModID, addin.Version, addin.File )
	local addinPath = addinFile.EvaluatedPath

	-- Get the absolute path and filename without extension.
	local extension = Path.GetExtension( addinPath )
	local path = addinPath:sub( 1, #addinPath - #extension )

	local ok, result = pcall( ContextPtr.LoadNewContext, ContextPtr, path )
	if ok then
		table.insert( g_uiAddins, result )
	else
		print( addinPath, result )
	end
end

-- modified by bc1 from 1.0.3.276 brave new world code
-- code is common using gk_mode and bnw_mode switches
-- show missing cash for trade agreements
local civ5_mode = InStrategicView ~= nil
local bnw_mode = not civ5_mode or Game.GetActiveLeague ~= nil;
local gk_mode = bnw_mode or Game.GetReligionName ~= nil;
if not civ5_mode then --no glass panel in civBE
	Controls.UsGlass = Controls.UsPanel
	Controls.ThemGlass = Controls.ThemPanel
end
----------------------------------------------------------------
----------------------------------------------------------------
include( "IconSupport" ); local IconHookup = IconHookup; local CivIconHookup = CivIconHookup
include( "InstanceManager" ); local GenerationalInstanceManager = GenerationalInstanceManager
include( "SupportFunctions" ); local TruncateString = TruncateString

local g_UsTableCitiesIM		= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.UsTableCitiesStack );
local g_UsPocketCitiesIM	= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.UsPocketCitiesStack );
local g_ThemTableCitiesIM	= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.ThemTableCitiesStack );
local g_ThemPocketCitiesIM	= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.ThemPocketCitiesStack );
local g_UsTableVoteIM, g_UsPocketVoteIM, g_ThemTableVoteIM, g_ThemPocketVoteIM;
if bnw_mode then
	g_UsTableVoteIM		= GenerationalInstanceManager:new( "TableVote", "Container", Controls.UsTableVoteStack );
	g_UsPocketVoteIM	= GenerationalInstanceManager:new( "PocketVote", "Button", Controls.UsPocketVoteStack );
	g_ThemTableVoteIM	= GenerationalInstanceManager:new( "TableVote", "Container", Controls.ThemTableVoteStack );
	g_ThemPocketVoteIM	= GenerationalInstanceManager:new( "PocketVote", "Button", Controls.ThemPocketVoteStack );
end
local g_bAlwaysWar		= Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR );
local g_bAlwaysPeace		= Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE );
local g_bNoChangeWar		= Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE );

----------------------------------------------------------------
-- local storage
----------------------------------------------------------------
local g_Deal = UI.GetScratchDeal();
local g_diploUIStateID;
local g_bPVPTrade;
local g_bTradeReview = false;
local g_iNumOthers;
local g_OfferIsFixed = false;	-- Is the human allowed to change the offer?  Set to true when the AI is demanding or making a request
local g_bEnableThirdParty = true;

local g_iConcessionsPreviousDiploUIState = -1;
local g_bHumanOfferingConcessions = false;

local g_iDealDuration = Game.GetDealDuration();
local g_iPeaceDuration = gk_mode and Game.GetPeaceDuration() or GameDefines.PEACE_TREATY_LENGTH or 10;

local g_iUs = -1; --Game.GetActivePlayer();
local g_iThem = -1;
local g_pUs;
local g_pThem;
local g_iUsTeam = -1;
local g_iThemTeam = -1;
local g_pUsTeam;
local g_pThemTeam;
local g_UsPocketResources = {};
local g_ThemPocketResources = {};
local g_UsTableResources = {};
local g_ThemTableResources = {};
local g_TableLeaderButtons = {}
local g_PocketLeaderButtons = {}
local g_LeagueVoteList = {};

local g_bMessageFromDiploAI = false;
local g_bAIMakingOffer = false;

local g_OtherPlayersButtons = {};

local g_bNewDeal = true;

local PROPOSE_TYPE	= 1;  -- player is making a proposal
local WITHDRAW_TYPE	= 2;  -- player is withdrawing their offer
local ACCEPT_TYPE	= 3;  -- player is accepting someone else's proposal

local CANCEL_TYPE	= 0;  -- player is canceling their draft
local REFUSE_TYPE	= 1;  -- player is refusing someone else's deal

local offsetOfString = 32;
local bonusPadding = 16
local innerFrameWidth = 524;
local outerFrameWidth = 520;
local offsetsBetweenFrames = 4;

local oldCursor = 0;
local DiploUIStateTypes = DiploUIStateTypes

local UsPocketPeaceButtons, UsPocketWarButtons, ThemPocketPeaceButtons, ThemPocketWarButtons, UsTablePeaceButtons, UsTableWarButtons, ThemTablePeaceButtons, ThemTableWarButtons

---------------------------------------------------------
-- LEADER MESSAGE HANDLER
---------------------------------------------------------
function LeaderMessageHandler( diploPlayerID, diploUIstateID, diploMessage, animationActionID, diploData )
--local dps, pdps;for k,v in pairs(DiploUIStateTypes) do if v==diploUIstateID then dps=k end if v==g_diploUIStateID then pdps=k end end; diploMessage = "Previous: "..tostring(pdps).."[NEWLINE]Current: "..tostring(dps)..tostring(diploData).."[NEWLINE]"..diploMessage
	g_bPVPTrade = false
	g_bTradeReview = false

	-- skip trade deal discussion blurb
	if UI.GetLeaderHeadRootUp() and diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION and
			( g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER
				or g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE ) then
--		g_Deal:ClearItems()
--		diploMessage = diploMessage .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLOMACY_ANYTHING_ELSE" )
		diploUIstateID = DiploUIStateTypes.DIPLO_UI_STATE_TRADE
	-- If we were just in discussion mode and the human offered to make concessions, make a note of that
	elseif (diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS) then
		if (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV) then
			--print("Human offers concessions for killing Minor Civ");
			g_iConcessionsPreviousDiploUIState = g_diploUIStateID;
		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING) then
			--print("Human offers concessions for expansion");
			g_iConcessionsPreviousDiploUIState = g_diploUIStateID;
		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING) then
			--print("Human offers concessions for plot buying");
			g_iConcessionsPreviousDiploUIState = g_diploUIStateID;
		end
	end

	g_diploUIStateID = diploUIstateID;

	g_iUs = Game.GetActivePlayer();
	g_pUs = Players[ g_iUs ];

	g_iUsTeam = g_pUs:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];

	g_iThem = diploPlayerID;
	g_pThem = Players[ g_iThem ];
	g_iThemTeam = g_pThem:GetTeam();
	g_pThemTeam = Teams[ g_iThemTeam ];

	-- Are we in Trade Mode?
	if diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND
	then

		--print("TradeScreen: It's MY mode!");

		if ContextPtr:IsHidden() then
			UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderTrade )
		end

print("Handling LeaderMessage: " .. diploUIstateID .. ", ".. diploMessage)

		g_Deal:SetFromPlayer(g_iUs);
		g_Deal:SetToPlayer(g_iThem);

		-- Unhide our pocket, in case the last thing we were doing in this screen was a human demand
		Controls.UsPanel:SetHide(false);
		Controls.UsGlass:SetHide(false);

		local bClearTableAndDisplayDeal = false;

		-- Is this a UI State where we should be displaying a deal?
		if (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE) then
			--print("DiploUIState: Default Trade");
			bClearTableAndDisplayDeal = true;
		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND) then
			--print("DiploUIState: AI making demand");
			bClearTableAndDisplayDeal = true;

			DoDemandState(true);

		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
			--print("DiploUIState: AI making Request");
			bClearTableAndDisplayDeal = true;

			DoDemandState(true);

		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
			--print("DiploUIState: Human Demand");
			bClearTableAndDisplayDeal = true;
			-- If we're demanding something, there's no need to show OUR items
			Controls.UsPanel:SetHide(true);
			Controls.UsGlass:SetHide(true);

		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS) then
			--print("DiploUIState: Human offers concessions");
			bClearTableAndDisplayDeal = true;
		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
			--print("DiploUIState: AI making offer");
			bClearTableAndDisplayDeal = true;
			g_bAIMakingOffer = true;
		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER) then
			--print("DiploUIState: AI accepted offer");
			g_iConcessionsPreviousDiploUIState = -1;		-- Clear out the fact that we were offering concessions if the AI has agreed to a deal
			bClearTableAndDisplayDeal = true;

		-- If the AI rejects a deal, don't clear the table: keep the items where they are in case the human wants to change things
		elseif (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER) then
			--print("DiploUIState: AI rejects offer");
			bClearTableAndDisplayDeal = false;
		else
			--print("DiploUIState: ?????");
		end

		-- Clear table and display the deal currently stored in InterfaceBuddy
		if (bClearTableAndDisplayDeal) then
			g_bMessageFromDiploAI = true;
			DoClearTable();
			DisplayDeal();

		-- Don't clear the table, leave things as they are
		else

			--print("NOT clearing table");

			g_bMessageFromDiploAI = true;

		end

		-- Resize the height of the box to fit the text
		Controls.DiscussionText:SetText( diploMessage );
		local contentSize = Controls.DiscussionText:GetSize().y + offsetOfString + bonusPadding;
		local frameSize = {};
		frameSize.x = innerFrameWidth;
		frameSize.y = contentSize;
		Controls.LeaderSpeechBorderFrame:SetSize( frameSize );
		frameSize.x = outerFrameWidth;
		frameSize.y = contentSize - offsetsBetweenFrames;
		Controls.LeaderSpeechFrame:SetSize( frameSize );

		DoUpdateButtons();

	-- Not in trade mode
	else

		--print("TradeScreen: NOT my mode! Hiding!");
		--print("diploUIstateID: " .. diploUIstateID);

		g_Deal:ClearItems();

		if (not ContextPtr:IsHidden()) then
			ContextPtr:SetHide( true );
		end

	end

end
--Events.AILeaderMessage.Add( LeaderMessageHandler );
--Events.LeavingLeaderViewMode.Add(
--function()
--	g_isHumanInitiatedTrade = false
--	g_iConcessionsPreviousDiploUIState = -1
--	g_diploUIStateID = -1
--end)

----------------------------------------------------------------
-- Used by SimpleDiploTrade to display pvp deals
----------------------------------------------------------------
function OnOpenPlayerDealScreen( playerID )

	--print( "OpenPlayerDealScreen: " .. playerID );

	-- open a new deal with iThem
	g_iUs = Game.GetActivePlayer();
	g_pUs = Players[ g_iUs ];
	g_iUsTeam = g_pUs:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];

	g_iThem = playerID;
	g_pThem = Players[ g_iThem ];
	g_iThemTeam = g_pThem:GetTeam();
	g_pThemTeam = Teams[ g_iThemTeam ];

	-- if someone is trying to open the deal screen when they are not allowed to negotiate peace
	-- or they already have an outgoing deal, stop them here rather than at all the places we send this event

	local iProposalTo = UI.HasMadeProposal( g_iUs );
	--print( "proposal: " .. iProposalTo );
	--print( "war: " .. tostring( g_pUsTeam:IsAtWar( g_iThemTeam ) ) );
	--print( "war: " .. tostring( g_bAlwaysWar or g_bNoChangeWar ) );


	-- this logic should match OnOpenPlayerDealScreen in TradeLogic.lua, DiploCorner.lua, and DiploList.lua
	if( (g_pUsTeam:IsAtWar( g_iThemTeam ) and (g_bAlwaysWar or g_bNoChangeWar) ) or																			-- Always at War
		(iProposalTo ~= -1 and iProposalTo ~= playerID and not UI.ProposedDealExists(playerID, g_iUs)) ) then -- Only allow one proposal from us at a time.
		-- do nothing
		return;
	end

	--print( "where?" );
	if( g_iUs == g_iThem ) then
		print( "ERROR: OpenPlayerDealScreen called with local player" );
	end

	if( UI.ProposedDealExists( g_iUs, g_iThem ) ) then -- this is our proposal
		g_bNewDeal = false;
		UI.LoadProposedDeal( g_iUs, g_iThem );

	elseif( UI.ProposedDealExists( g_iThem, g_iUs ) ) then -- this is their proposal
		g_bNewDeal = false;
		UI.LoadProposedDeal( g_iThem, g_iUs );

	else
		g_bNewDeal = true;
		g_Deal:ClearItems();
		g_Deal:SetFromPlayer( g_iUs );
		g_Deal:SetToPlayer( g_iThem );

		if( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
			g_Deal:AddPeaceTreaty( g_iUs, g_iPeaceDuration )
			g_Deal:AddPeaceTreaty( g_iThem, g_iPeaceDuration )
		end

	end

	g_bPVPTrade = true;
	DoUpdateButtons();

	ContextPtr:SetHide( false );

	DoClearTable();
	DisplayDeal();
end


----------------------------------------------------------------
-- used by DiploOverview to display active deals
----------------------------------------------------------------
function OpenDealReview()

	--print( "OpenDealReview" );

	g_iUs = Game:GetActivePlayer();
	g_pUs = Players[ g_iUs ];
	g_iUsTeam = g_pUs:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];

	g_iThem = g_Deal:GetOtherPlayer( g_iUs );
	g_pThem = Players[ g_iThem ];
	g_iThemTeam = g_pThem:GetTeam();
	g_pThemTeam = Teams[ g_iThemTeam ];

	if( g_iUs == g_iThem ) then
		print( "ERROR: OpenDealReview called with local player" );
	end

	g_bPVPTrade = false;
	g_bTradeReview = true;

	DoClearTable();
	DisplayDeal();
end


----------------------------------------------------------------
-- BACK
----------------------------------------------------------------
function OnBack( iType )

	if g_bPVPTrade or g_bTradeReview then

		if( iType == CANCEL_TYPE ) then
			g_Deal:ClearItems();
		elseif( iType == REFUSE_TYPE ) then
			UI.DoFinalizePlayerDeal( g_iThem, g_iUs, false );
		end

		ContextPtr:SetHide( true );
		ContextPtr:CallParentShowHideHandler( false );
	else
		g_Deal:ClearItems();
		UIManager:DequeuePopup( ContextPtr )

		-- Human refused to give into an AI demand - this button doubles as the Demand "Refuse" option
		if g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND then
			DoDemandState(false)
			DoClearTable()
			ResetDisplay()
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL, g_iThem, 0, 0 )
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST then
			DoDemandState(false)
			DoClearTable()
			ResetDisplay()

			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL, g_iThem, gk_mode and g_Deal and g_pUs:GetDealMyValue(g_Deal) or 0, 0 )

		-- Exit screen normally
		else

			if (g_iThem ~= -1) then
				Players[g_iThem]:DoTradeScreenClosed(g_bAIMakingOffer);
			end

			-- Don't assume we have a message from the AI leader any more
			g_bMessageFromDiploAI = false;

			-- If the AI opened the screen to make an offer, clear that status
			g_bAIMakingOffer = false;

			-- Reset value that keeps track of how many times we offered a bad deal (for dialogue purposes)
			UI.SetOfferTradeRepeatCount(0);

			-- If the human was offering concessions, he backed out
			if (g_iConcessionsPreviousDiploUIState ~= -1) then
				--print("Human backing out of Concessions Offer. Reseting DiploUIState to trade.");

				iButtonID = 1;		-- This corresponds with the buttons in DiscussionDialog.lua

				-- AI scolded us for killing a Minor - we tell him to go away
				if (g_iConcessionsPreviousDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV) then
					Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE, g_iThem, iButtonID, 0 );
				-- AI seriously warning us about expansion - we tell him to go away
				elseif (g_iConcessionsPreviousDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING) then
					Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE, g_iThem, iButtonID, 0 );
				-- AI seriously warning us about plot buying - we tell him to go away
				elseif (g_iConcessionsPreviousDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING) then
					Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE, g_iThem, iButtonID, 0 );
				end

				g_iConcessionsPreviousDiploUIState = -1;
			end

			-- Exiting human demand mode, unhide his items
			if (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
				Controls.UsPanel:SetHide(false);
				Controls.UsGlass:SetHide(false);
			end
			UI.RequestLeaveLeader();
		end
	end
	g_diploUIStateID = false
end
Controls.CancelButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------
-- SHOW/HIDE
----------------------------------------------------------------
function OnShowHide( isHide, bIsInit )

	-- WARNING: Don't put anything important in here related to gameplay. This function may be called when you don't expect it

	if( not bIsInit ) then
		-- Showing screen
		if( not isHide ) then
			oldCursor = UIManager:SetUICursor(0); -- make sure we start with the default cursor

			if( g_iUs ~= -1 ) then g_Deal:SetFromPlayer( g_iUs ); end
			if( g_iThem ~= -1 ) then g_Deal:SetToPlayer( g_iThem ); end
--[[
			-- reset all the controls on show
			Controls.UsPocketCitiesStack:SetHide( true );
			Controls.ThemPocketCitiesStack:SetHide( true );
			Controls.UsPocketOtherPlayerStack:SetHide( true );
			Controls.ThemPocketOtherPlayerStack:SetHide( true );
			Controls.UsPocketStrategicStack:SetHide( true );
			Controls.ThemPocketStrategicStack:SetHide( true );
			Controls.UsPocketLuxuryStack:SetHide( true );
			Controls.ThemPocketLuxuryStack:SetHide( true );
			if bnw_mode then
				Controls.UsPocketVoteStack:SetHide( true );
				Controls.ThemPocketVoteStack:SetHide( true );
			end
			Controls.UsPocketPanel:SetScrollValue( 0 );
			Controls.UsTablePanel:SetScrollValue( 0 );
			Controls.ThemPocketPanel:SetScrollValue( 0 );
			Controls.ThemTablePanel:SetScrollValue( 0 );


--]]
			ResetDisplay()

			-- Deal can already have items in it if, say, we're at war. In this case every time we open the trade screen there's already Peace Treaty on both sides of the table
			if g_Deal:GetNumItems() > 0 then
				DisplayDeal()
			end

			LuaEvents.TryQueueTutorial("DIPLO_TRADE_SCREEN", true);

		-- Hiding screen
		else
			UIManager:SetUICursor(oldCursor); -- make sure we retrun the cursor to the previous state
			LuaEvents.TryDismissTutorial("DIPLO_TRADE_SCREEN");

		end
	end

end
ContextPtr:SetShowHideHandler( OnShowHide );


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if( uiMsg == KeyEvents.KeyDown ) then
		if( wParam == Keys.VK_ESCAPE ) then

			-- Don't allow any keys to exit screen when AI is asking for something - want to make sure the human has to click something
			if (g_diploUIStateID ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND and
				g_diploUIStateID ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then

				OnBack();
			end

			return true;
		-- Consume enter here otherwise it may be
		-- interpretted as dismissing the dialog.
		elseif ( wParam == Keys.VK_RETURN) then
			return true;
		end
	end
	return false;
end

---------------------------------------------------------
-- UI Deal was modified somehow
---------------------------------------------------------
local function DoUIDealChangedByHuman( isClearTable )

	-- Set state to the default so that it doesn't cause any funny behavior later
	if g_diploUIStateID ~= DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
		g_diploUIStateID = DiploUIStateTypes.DIPLO_UI_STATE_TRADE
	end

	if isClearTable then
		DoClearTable()
	end
	DisplayDeal()
	DoUpdateButtons()

end


---------------------------------------------------------
-- Update buttons at the bottom
---------------------------------------------------------
function DoUpdateButtons()

	if g_bTradeReview then
	elseif g_bPVPTrade then -- Dealing with a human in a MP game
		--print( "PVP Updating ProposeButton" );

		if g_bNewDeal then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_PROPOSE" )
			Controls.ProposeButton:SetVoid1( PROPOSE_TYPE );
			Controls.CancelButton:SetHide( true );

			Controls.ModifyButton:SetHide( true );
			Controls.Pockets:SetHide( false );
			Controls.ModificationBlock:SetHide( true );

		elseif UI.HasMadeProposal( g_iUs ) == g_iThem then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_WITHDRAW" )
			Controls.ProposeButton:SetVoid1( WITHDRAW_TYPE );
			Controls.CancelButton:SetHide( true );

			Controls.ModifyButton:SetHide( true );
			Controls.Pockets:SetHide( true );
			Controls.ModificationBlock:SetHide( false );

		else
			Controls.ProposeButton:SetVoid1( ACCEPT_TYPE );
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_ACCEPT" )
			Controls.CancelButton:SetVoid1( REFUSE_TYPE );
			Controls.CancelButton:SetHide( false );

			Controls.ModifyButton:SetHide( false );
			Controls.Pockets:SetHide( true );
			Controls.ModificationBlock:SetHide( false );
		end

		Controls.MainStack:CalculateSize();
		Controls.MainGrid:DoAutoSize();

	else -- Dealing with an AI

		local numItemsFromUs = 0
		local numItemsFromThem = 0
		local isAtWar = g_pUsTeam and g_pThemTeam and g_pUsTeam:IsAtWar( g_iThemTeam )
		local isAIRequestingConcessions = not isAtWar and UI.IsAIRequestingConcessions()

		-- Table is empty
		if g_Deal:GetNumItems() == 0 then
			Controls.ProposeButton:LocalizeAndSetText( "{TXT_KEY_GOODBYE_BUTTON:upper}" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_CANCEL" )

		-- Human is making a demand
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_DEMAND_BUTTON" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_CANCEL" )

		-- If the AI made an offer change what buttons are visible for the human to respond with
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_ACCEPT" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_REFUSE" )

		-- AI is making a demand or Request
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND or
					g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_ACCEPT" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_REFUSE" )

		else
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_PROPOSE" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_CANCEL" )
			if not isAtWar and not isAIRequestingConcessions then

				-- Loop through deal items to see what the situation is
				g_Deal:ResetIterator()
				repeat
					local deal = { g_Deal:GetNextItem() }
					local n = #deal
					if n < 1 then
						break
					elseif deal[n] == g_iUs then
						numItemsFromUs = numItemsFromUs + 1
					else
						numItemsFromThem = numItemsFromThem + 1
					end
				until false
			end
		end


		-- Depending on what's on the table we can ask the other player about what they think of the deal
		Controls.WhatDoYouWantButton:SetHide( numItemsFromUs > 0 or numItemsFromThem == 0 )
		Controls.WhatWillYouGiveMeButton:SetHide( numItemsFromUs == 0 or numItemsFromThem > 0 )
		Controls.WhatWillMakeThisWorkButton:SetHide( numItemsFromUs == 0 or numItemsFromThem == 0 )
		Controls.WhatWillEndThisWarButton:SetHide( not isAtWar )
		Controls.WhatConcessionsButton:SetHide( not isAIRequestingConcessions )
		Controls.CancelButton:SetHide( false )
		Controls.ProposeButton:SetHide( false )
	end

end


---------------------------------------------------------
-- AI is making a demand of the human
---------------------------------------------------------
function DoDemandState(bDemandOn)

	-- AI is demanding something, hide options that are not useful
	Controls.UsPanel:SetHide( bDemandOn )
	Controls.UsGlass:SetHide( bDemandOn )
	Controls.ThemPanel:SetHide( bDemandOn )
	Controls.ThemGlass:SetHide( bDemandOn )
	Controls.UsTableCover:SetHide( not bDemandOn )
	Controls.ThemTableCover:SetHide( not bDemandOn )

end


---------------------------------------------------------
-- Clear all items off the table (both UI and CvDeal)
---------------------------------------------------------
function DoClearDeal()

	--print("Clearing Table");

	g_Deal:ClearItems();
	DoClearTable();
end
Events.ClearDiplomacyTradeTable.Add(DoClearDeal);



---------------------------------------------------------
-- stacks management
---------------------------------------------------------
-- hack existing XML
Controls.UsPocketCitiesStack:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketCitiesStack:SetOffsetVal( 0, 0 )
Controls.UsPocketCitiesClose:SetHide( true )
Controls.UsPocketOtherPlayer:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketOtherPlayerStack:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketLeaderClose:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketLeaderStack:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketLeaderStack:SetOffsetVal( 0, 0 )
Controls.UsPocketOtherPlayerPeace:SetHide( true ) -- UsPocketOtherPlayer
Controls.UsPocketOtherPlayerWar:SetHide( true ) -- UsPocketLeaderClose

Controls.ThemPocketCitiesStack:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketCitiesStack:SetOffsetVal( 0, 0 )
Controls.ThemPocketCitiesClose:SetHide( true )
Controls.ThemPocketOtherPlayer:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketOtherPlayerStack:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketLeaderClose:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketLeaderStack:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketLeaderStack:SetOffsetVal( 0, 0 )
Controls.ThemPocketOtherPlayerPeace:SetHide( true ) -- ThemPocketOtherPlayer
Controls.ThemPocketOtherPlayerWar:SetHide( true ) -- ThemPocketLeaderClose

local g_SubStacks = {
	UsPocketOtherPlayer = Controls.UsPocketOtherPlayerStack,
	ThemPocketOtherPlayer = Controls.ThemPocketOtherPlayerStack,
	UsPocketLeaderClose = Controls.UsPocketLeaderStack,
	ThemPocketLeaderClose = Controls.ThemPocketLeaderStack,

	UsPocketStrategic = Controls.UsPocketStrategicStack,
	ThemPocketStrategic = Controls.ThemPocketStrategicStack,
	UsPocketLuxury = Controls.UsPocketLuxuryStack,
	ThemPocketLuxury = Controls.ThemPocketLuxuryStack,
	UsPocketCities = Controls.UsPocketCitiesStack,
	ThemPocketCities = Controls.ThemPocketCitiesStack,

	UsPocketVote = Controls.UsPocketVoteStack,
	ThemPocketVote = Controls.ThemPocketVoteStack,
}

local g_SubLabels = {
	UsPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
	ThemPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
	UsPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",
	ThemPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",

	UsPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_STRATEGIC_RESOURCES",
	ThemPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_STRATEGIC_RESOURCES",
	UsPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES",
	ThemPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES",
	UsPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_CITIES",
	ThemPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_CITIES",

	UsPocketVote = Locale.ConvertTextKey"TXT_KEY_TRADE_ITEM_VOTES",
	ThemPocketVote = Locale.ConvertTextKey"TXT_KEY_TRADE_ITEM_VOTES",
}

local g_SubTooltipsYes = {
--	UsPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
--	ThemPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
--	UsPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",
--	ThemPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",

	UsPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES",
	ThemPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES_THEM",
	UsPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES",
	ThemPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES_THEM",
	UsPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_TT",
	ThemPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_TT",

	UsPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_YES",
	ThemPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_YES_THEM",
}

local g_SubTooltipsNo = {
--	UsPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS",
--	ThemPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM",
--	UsPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS",
--	ThemPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM",

	UsPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO",
	ThemPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO_THEM",
	UsPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO",
	ThemPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO_THEM",
	UsPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT",
	ThemPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT",

	UsPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_NO",
	ThemPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_NO_THEM",
}

Controls.UsPocketOtherPlayer:SetVoid1( 1 )
Controls.UsPocketLeaderClose:SetVoid1( 1 )
Controls.UsPocketStrategic:SetVoid1( 1 )
Controls.UsPocketLuxury:SetVoid1( 1 )

Controls.UsPocketCities:SetVoid1( 1 );

Controls.UsPocketOtherPlayerStack:SetHide( true )
Controls.ThemPocketOtherPlayerStack:SetHide( true )
Controls.UsPocketLeaderStack:SetHide( true )
Controls.ThemPocketLeaderStack:SetHide( true )
Controls.UsPocketCitiesStack:SetHide( true )
Controls.ThemPocketCitiesStack:SetHide( true )

local function SubStackToggle( button, isToggle )
	local buttonID = button:GetID()
	local stack = g_SubStacks[ buttonID ]
	stack:CalculateSize()
	if stack:GetSizeY() == 0 then
		button:SetDisabled( true )
		button:GetTextControl():SetColorByName("Gray_Black")
		button:SetToolTipString( g_SubTooltipsNo[ buttonID ] )
		button:SetText( g_SubLabels[ buttonID ] )
	else
		button:SetDisabled( false )
		if isToggle == stack:IsHidden() then
			stack:SetHide( false );
			button:SetText( "[ICON_MINUS]" .. g_SubLabels[ buttonID ] )
		else
			stack:SetHide( true );
			button:SetText( "[ICON_PLUS]" .. g_SubLabels[ buttonID ] )
		end
		button:GetTextControl():SetColorByName("Beige_Black")
		button:SetToolTipString( g_SubTooltipsYes[ buttonID ] )
	end
end

local function SubStackHandler( bIsUs, none, control )
	SubStackToggle( control, true )

	if bIsUs == 1 then
		Controls.UsPocketStack:CalculateSize()
		Controls.UsPocketPanel:CalculateInternalSize()
		Controls.UsPocketPanel:ReprocessAnchoring()
	else
		Controls.ThemPocketStack:CalculateSize()
		Controls.ThemPocketPanel:CalculateInternalSize()
		Controls.ThemPocketPanel:ReprocessAnchoring()
	end
end
for k,v in pairs( g_SubStacks ) do
	Controls[ k ]:RegisterCallback( Mouse.eLClick, SubStackHandler );
end

if bnw_mode then
	Controls.UsPocketVote:SetVoid1( 1 )
	Controls.UsPocketVoteStack:SetHide( true )
	Controls.ThemPocketVoteStack:SetHide( true )

	if not Game.IsOption("GAMEOPTION_NO_LEAGUES") then
		local function VoteStackHandler( bIsUs, ... )
			RefreshPocketVotes( bIsUs )
			SubStackHandler( bIsUs, ... )
		end
		Controls.ThemPocketVote:RegisterCallback( Mouse.eLClick, VoteStackHandler )
		Controls.UsPocketVote:RegisterCallback( Mouse.eLClick, VoteStackHandler )
	end
end


----------------------------------------------------------------
-- Proposal and Negotiate functions
----------------------------------------------------------------

function OnModify()
	g_bNewDeal = true;
	DoUpdateButtons();
end


----------------------------------------------------------------
----------------------------------------------------------------
function OnPropose( iType )

	if g_Deal:GetNumItems() == 0 then
		if not g_bPVPTrade then
			UIManager:DequeuePopup( ContextPtr )
			UI.SetLeaderHeadRootUp( false )
			UI.RequestLeaveLeader()
		end
		return;
	end

	--print( "OnPropose: " .. tostring( g_bPVPTrade ) .. " " .. iType );

	-- Trade between humans
	if g_bPVPTrade then

		if( iType == PROPOSE_TYPE ) then
			-- Set the from/to players in the deal.  We do this because we might have not done this already.
			g_Deal:SetFromPlayer(g_iUs);
			g_Deal:SetToPlayer(g_iThem);
			UI.DoProposeDeal();

		elseif( iType == WITHDRAW_TYPE ) then
			UI.DoFinalizePlayerDeal( g_iUs, g_iThem, false );

		elseif( iType == ACCEPT_TYPE ) then
			UI.DoFinalizePlayerDeal( g_iThem, g_iUs, true );

		else
			print( "invalid ProposalType" );
		end

		ContextPtr:SetHide( true );
		ContextPtr:CallParentShowHideHandler( false );

	-- Trade between a human & an AI
	else

		-- Human Demanding something
		if (g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
			UI.DoDemand();

		-- Trade offer
		else
    		-- Set the from/to players in the deal.  We do this because we might have not done this already.
			g_Deal:SetFromPlayer(g_iUs);
			g_Deal:SetToPlayer(g_iThem);
			UI.DoProposeDeal();
		end

		-- Don't assume we have a message from the AI leader any more
		g_bMessageFromDiploAI = false;

		-- If the AI was demanding something, we're not in that state any more
		DoDemandState(false)
		UIManager:DequeuePopup( ContextPtr )
	end
--	g_diploUIStateID = false
end
Controls.ProposeButton:RegisterCallback( Mouse.eLClick, OnPropose );


----------------------------------------------------------------
----------------------------------------------------------------
function OnEqualizeDeal()

	UI.DoEqualizeDealWithHuman();

	-- Don't assume we have a message from the AI leader any more
	g_bMessageFromDiploAI = false;

end


----------------------------------------------------------------
----------------------------------------------------------------
function OnWhatDoesAIWant()

	UI.DoWhatDoesAIWant();

	-- Don't assume we have a message from the AI leader any more
	g_bMessageFromDiploAI = false;

end


----------------------------------------------------------------
----------------------------------------------------------------
function OnWhatWillAIGive()

	UI.DoWhatWillAIGive();

	-- Don't assume we have a message from the AI leader any more
	g_bMessageFromDiploAI = false;

end


----------------------------------------------------------------------------------
-- Pocket Cities
----------------------------------------------------------------------------------
local function AddCityTrade( playerID, cityID )
	local player = Players[ playerID ]
	if player and player:GetCityByID( cityID ) then
		g_Deal:AddCityTrade( playerID, cityID )
		DoUIDealChangedByHuman()
	else
		print( "Error - city not found", playerID, cityID )
	end
end

local function RemoveCityTrade( playerID, cityID )
	local player = Players[ playerID ]
	if player and player:GetCityByID( cityID ) then
		g_Deal:RemoveCityTrade( playerID, cityID )
		DoUIDealChangedByHuman( true )
	else
		print( "Error - city not found", playerID, cityID )
	end
end

local function SetPocketCities( InstanceManager, fromPlayer, fromPlayerID, toPlayerID )
	InstanceManager:ResetInstances()
	if fromPlayer then
		for city in fromPlayer:Cities() do
			if g_Deal:IsPossibleToTradeItem( fromPlayerID, toPlayerID, TradeableItems.TRADE_ITEM_CITIES, city:GetX(), city:GetY() ) then
				local instance = InstanceManager:GetInstance()
				local cityID = city:GetID()
				instance.CityName:SetText( city:GetName() )
				instance.CityPop:SetText( city:GetPopulation() )
				local button = instance.Button
				button:SetVoids( fromPlayerID, cityID )
				button:RegisterCallback( Mouse.eLClick, AddCityTrade )
			end
		end
	end
end

local function SetPocketCivs( pocketButtonList, fromPlayerID, toPlayerID, tradeType )
	for playerID, button in pairs( pocketButtonList ) do
		local player = Players[ playerID ]
		local isOK
		if player and player:IsAlive() then
			local teamID = player:GetTeam()
			local team = Teams[ teamID ]
			isOK = g_iUsTeam ~= teamID
				and g_iThemTeam ~= teamID
				and g_pUsTeam:IsHasMet( teamID )
				and g_pThemTeam:IsHasMet( teamID )
				and not( player:IsMinorCiv() and Players[ player:GetAlly() ] )
				and g_Deal:IsPossibleToTradeItem( fromPlayerID, toPlayerID, tradeType, teamID )
--[[
				if g_Deal:IsPossibleToTradeItem( fromPlayerID, toPlayerID, tradeType, teamID ) then

					button:SetDisabled( false )
					button:SetAlpha( 1 )
				else

					button:SetDisabled( true )
					button:SetAlpha( 0.5 )
					local fromPlayer = Players[fromPlayerID]
					local fromTeamID = fromPlayer:GetTeam()

					-- Why won't they make peace?
					if tradeType == TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE then

						-- Not at war
						if not team:IsAtWar( fromTeamID ) then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_AT_WAR")

						-- Minor that won't make peace
						elseif player:IsMinorCiv() then

							local ally = Players[ player:GetAlly() ]

							-- Minor in permanent war with this guy
							if player:IsMinorPermanentWar( fromPlayerID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_PERMANENT_WAR")
							-- Minor allied to a player
							elseif ally and Teams[ally:GetTeam()]:IsAtWar( fromTeamID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_ALLY_AT_WAR")
							end

						-- Major that won't make peace
						else
							-- AI don't want no peace!
							if not fromPlayer:IsWillAcceptPeaceWithPlayer( playerID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_THIS_GUY_WANTS_WAR")
							-- Other AI don't want no peace!
							elseif not player:IsWillAcceptPeaceWithPlayer( fromPlayerID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_OTHER_GUY_WANTS_WAR")
							end
						end

					-- Why won't they make war?
					elseif tradeType == TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR then
						-- Already at war
						if team:IsAtWar( fromTeamID ) then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_ALREADY_AT_WAR")

						-- Locked in to peace
						elseif Teams[fromTeamID]:IsForcePeace(teamID) then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_FORCE_PEACE")

						-- City-State ally
						elseif player:IsMinorCiv() and player:GetAlly() == fromPlayerID then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_WAR_ALLIES")
						end
					end
				end
--]]
		end
		button:SetHide( not isOK )
	end
end

local function ShowTableCiv( tableButtonList, pocketButtonList, teamID )
	for playerID, button in pairs( tableButtonList ) do
		local player = Players[ playerID ]
		if player and player:IsAlive() and player:GetTeam() == teamID then
			button:SetHide( false )
			tableButtonList.Parent:SetHide( false )
			button = pocketButtonList[ playerID ]
			if button then
				button:SetHide( true )
			end
		end
	end
end

----------------------------------------------------------------
----------------------------------------------------------------

function ResetDisplay()

	--print("ResetDisplay");

	if g_iUs == -1 or g_iThem == -1 then
		return;
	end

	Controls.UsPocketStack:SetHide( false );
	Controls.ThemPocketStack:SetHide( false );

	Controls.UsPocketGold:SetHide( false );
	Controls.ThemPocketGold:SetHide( false );
	Controls.UsPocketGoldPerTurn:SetHide( false );
	Controls.ThemPocketGoldPerTurn:SetHide( false );
	Controls.UsPocketOpenBorders:SetHide( false );
	Controls.ThemPocketOpenBorders:SetHide( false );
	Controls.UsPocketDefensivePact:SetHide( false );
	Controls.ThemPocketDefensivePact:SetHide( false );
	Controls.UsPocketResearchAgreement:SetHide( false );
	Controls.ThemPocketResearchAgreement:SetHide( false );
	--Controls.UsPocketTradeAgreement:SetHide( false );		Trade agreement disabled for now
	--Controls.ThemPocketTradeAgreement:SetHide( false );		Trade agreement disabled for now
	if gk_mode then
		Controls.UsPocketAllowEmbassy:SetHide( false );
		Controls.ThemPocketAllowEmbassy:SetHide( false );
		if (Controls.UsPocketDoF ~= nil) then
			Controls.UsPocketDoF:SetHide( false );
		end
		if (Controls.ThemPocketDoF ~= nil) then
			Controls.ThemPocketDoF:SetHide( false );
		end
	end -- gk_mode
	-- Propose button could have had its text changed to "ACCEPT" if the AI made an offer to the human
	DoUpdateButtons();

	if( not g_bMessageFromDiploAI ) then
		DoClearTable();
	end

	-- Research Agreement cost
	local iRACost = Game.GetResearchAgreementCost(g_iUs, g_iThem);
	local strRAString = Locale.ConvertTextKey("TXT_KEY_DIPLO_RESCH_AGREEMENT_US", iRACost);
	Controls.UsTableResearchAgreement:SetText(strRAString);
	Controls.ThemTableResearchAgreement:SetText(strRAString);


	if( g_bTradeReview ) then
		-- review mode

		Controls.TradeDetails:SetHide( false );

		CivIconHookup( g_iUs, 64, Controls.UsSymbolShadow, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true );
		CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true );

		Controls.UsText:LocalizeAndSetText( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pUs:GetNameKey() )

		if( m_bIsMulitplayer and pOtherPlayer:IsHuman() ) then
			Controls.ThemText:LocalizeAndSetText( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetNickName() )
		else
			Controls.ThemText:LocalizeAndSetText( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetName() )
		end

	elseif( g_bPVPTrade == false ) then
		-- ai mode
		Controls.WhatDoYouWantButton:SetHide(true);
		Controls.WhatWillYouGiveMeButton:SetHide(true);
		Controls.WhatWillMakeThisWorkButton:SetHide(true);


		Controls.NameText:LocalizeAndSetText( "TXT_KEY_DIPLO_LEADER_SAYS", g_pThem:GetName() )

		if( not g_bMessageFromDiploAI ) then
			Controls.DiscussionText:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_HERE_OFFER"));
		end

		-- Set Civ Icon
		CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true );

		Controls.LeaderNameItems:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_LABEL",Locale.ConvertTextKey(g_pThem:GetNameKey())));

		-- set up their portrait
		local themCivType = g_pThem:GetCivilizationType();
		local themCivInfo = GameInfo.Civilizations[themCivType];

		local themLeaderType = g_pThem:GetLeaderType();
		local themLeaderInfo = GameInfo.Leaders[themLeaderType];

		IconHookup( themLeaderInfo.PortraitIndex, 128, themLeaderInfo.IconAtlas, Controls.ThemPortrait );

		-- Is the AI is making demand?
		if (UI.IsAIRequestingConcessions()) then
			Controls.WhatConcessionsButton:SetHide(false);
		else
			Controls.WhatConcessionsButton:SetHide(true);
		end

		-- If we're at war with the other guy then show the "what will end this war" button
		if (g_iUsTeam >= 0 and g_iThemTeam >= 0 and Teams[g_iUsTeam]:IsAtWar(g_iThemTeam)) then
			Controls.WhatWillEndThisWarButton:SetHide(false);
		else
			Controls.WhatWillEndThisWarButton:SetHide(true);
		end

	elseif( g_bTradeReview == false ) then
		-- PvP mode

		CivIconHookup( g_iUs, 64, Controls.UsSymbolShadow, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true );
		CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true );

		TruncateString(Controls.ThemName, Controls.ThemTablePanel:GetSizeX() - Controls.ThemTablePanel:GetOffsetX(),
							Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetNickName() ));
		Controls.ThemCiv:SetText( "(" .. Locale.ConvertTextKey( GameInfo.Civilizations[ g_pThem:GetCivilizationType() ].ShortDescription ) .. ")" );

	end


	local strTooltip;
	----------------------------------------------------------------------------------
	-- pocket Gold
	----------------------------------------------------------------------------------

	local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now

	-- Us
	local iGold = g_Deal:GetGoldAvailable(g_iUs, iItemToBeChanged);
	local strGoldString = iGold .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD");
	Controls.UsPocketGold:SetText( strGoldString );

	local bGoldTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD, 1);	-- 1 here is 1 Gold, which is the minimum possible
	local sUsPocketGoldTT = nil;
	if (not bGoldTradeAllowed) then
		Controls.UsPocketGold:SetDisabled(true);
		Controls.UsPocketGold:GetTextControl():SetColorByName("Gray_Black");
		if bnw_mode then
			sUsPocketGoldTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_DOF_TT_ONE_LINE");
		end
	else
		Controls.UsPocketGold:SetDisabled(false);
		Controls.UsPocketGold:GetTextControl():SetColorByName("Beige_Black");
	end
	Controls.UsPocketGold:SetToolTipString(sUsPocketGoldTT);

	-- Them
	iGold = g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged);
	strGoldString = iGold .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD");
	Controls.ThemPocketGold:SetText( strGoldString );

	bGoldTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD, 1);	-- 1 here is 1 Gold, which is the minimum possible

	local sThemPocketGoldTT = nil;
	if (not bGoldTradeAllowed) then
		Controls.ThemPocketGold:SetDisabled(true);
		Controls.ThemPocketGold:GetTextControl():SetColorByName("Gray_Black");
		if bnw_mode then
			sThemPocketGoldTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_DOF_TT_ONE_LINE");
		end
	else
		Controls.ThemPocketGold:SetDisabled(false);
		Controls.ThemPocketGold:GetTextControl():SetColorByName("Beige_Black");
	end
	Controls.ThemPocketGold:SetToolTipString(sThemPocketGoldTT);

	----------------------------------------------------------------------------------
	-- pocket Gold Per Turn
	----------------------------------------------------------------------------------

	-- Us
	local goldPerTurn = g_pUs:CalculateGoldRate();
	strGoldString = goldPerTurn .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN");
	Controls.UsPocketGoldPerTurn:SetText( strGoldString );

	local bGPTAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible

	if (not bGPTAllowed) then
		Controls.UsPocketGoldPerTurn:SetDisabled(true);
		Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.UsPocketGoldPerTurn:SetDisabled(false);
		Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Beige_Black");
	end

	-- Them
	goldPerTurn = g_pThem:CalculateGoldRate();
	strGoldString = goldPerTurn .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN");
	Controls.ThemPocketGoldPerTurn:SetText( strGoldString );

	bGPTAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible

	if (not bGPTAllowed) then
		Controls.ThemPocketGoldPerTurn:SetDisabled(true);
		Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.ThemPocketGoldPerTurn:SetDisabled(false);
		Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Beige_Black");
	end
	if gk_mode then
		----------------------------------------------------------------------------------
		-- pocket Allow Embassy
		----------------------------------------------------------------------------------

		local bAllowEmbassyAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration);

		-- Are we not allowed to give Allow Embassy? (already providing it to them?)
		strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_TT", g_iDealDuration);

		local strOurTooltip = strTooltip;
		local strTheirTooltip = strTooltip;

		if (not bAllowEmbassyAllowed) then
			if (not g_pThemTeam:IsAllowEmbassyTradingAllowed()) then
				strOurTooltip = strOurTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_OTHER_PLAYER" ) .. "[ENDCOLOR]";
			end
		end

		-- Are we not allowed to give Allow Embassy? (already providing it to them?)
		if (not bAllowEmbassyAllowed) then
			Controls.UsPocketAllowEmbassy:SetDisabled(true);
			Controls.UsPocketAllowEmbassy:GetTextControl():SetColorByName("Gray_Black");

			if (g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
				strOurTooltip = strOurTooltip .. " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_ALLOW_EMBASSY_THEY_HAVE" );
			end
		else
			Controls.UsPocketAllowEmbassy:SetDisabled(false);
			Controls.UsPocketAllowEmbassy:GetTextControl():SetColorByName("Beige_Black");
		end

		Controls.UsPocketAllowEmbassy:SetToolTipString(strOurTooltip);

		bEmbassyAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration);

		if (not bAllowEmbassyAllowed) then
			if (not g_pUsTeam:IsAllowEmbassyTradingAllowed()) then
				strTheirTooltip = strTheirTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_PLAYER" ) .. "[ENDCOLOR]";
			end
		end

		-- Are they not allowed to have an embassy? (perhaps they already have one established)
		if (not bEmbassyAllowed) then
			Controls.ThemPocketAllowEmbassy:SetDisabled(true);
			Controls.ThemPocketAllowEmbassy:GetTextControl(): SetColorByName("Gray_Black");

			if (g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) then
				strTheirTooltip = strTheirTooltip .. " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_ALLOW_EMBASSY_HAVE" );
			end
		else
			Controls.ThemPocketAllowEmbassy:SetDisabled(false);
			Controls.ThemPocketAllowEmbassy:GetTextControl():SetColorByName("Beige_Black");
		end

		Controls.ThemPocketAllowEmbassy:SetToolTipString(strTheirTooltip);
	end -- gk_mode
	----------------------------------------------------------------------------------
	-- pocket Open Borders
	----------------------------------------------------------------------------------

	local bOpenBordersAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration);

	-- Are we not allowed to give Open Borders? (don't have tech)
	strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_OPEN_BORDERS_TT", g_iDealDuration );
	if (not bOpenBordersAllowed) then
		if (not g_pUsTeam:IsOpenBordersTradingAllowed() and not g_pThemTeam:IsOpenBordersTradingAllowed()) then
			if gk_mode then
				strTooltip = strTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_OPEN_BORDERS_NO_TECH" ) .. "[ENDCOLOR]";
			else
				strTooltip = strTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_WRITING_TT" ) .. "[ENDCOLOR]";
			end
		end
	end

	local strOurTooltip = strTooltip;
	local strTheirTooltip = strTooltip;

	-- Are we not allowed to give OB? (don't have tech, or are already providing it to them)
	if (not bOpenBordersAllowed) then
		Controls.UsPocketOpenBorders:SetDisabled(true);
		Controls.UsPocketOpenBorders:GetTextControl():SetColorByName("Gray_Black");

		if (g_pUsTeam:IsAllowsOpenBordersToTeam(g_iThemTeam)) then
			strOurTooltip = strOurTooltip .. " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_OPEN_BORDERS_HAVE" );
		elseif gk_mode and (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) then
			strOurTooltip = strOurTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_YOU_NEED_EMBASSY_TT" ) .. "[ENDCOLOR]";
		end

	else
		Controls.UsPocketOpenBorders:SetDisabled(false);
		Controls.UsPocketOpenBorders:GetTextControl():SetColorByName("Beige_Black");
	end

	Controls.UsPocketOpenBorders:SetToolTipString(strOurTooltip);

	bOpenBordersAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration);

	-- Are they not allowed to give OB? (don't have tech, or are already providing it to us)
	if (not bOpenBordersAllowed) then
		Controls.ThemPocketOpenBorders:SetDisabled(true);
		Controls.ThemPocketOpenBorders:GetTextControl():SetColorByName("Gray_Black");

		if (g_pUsTeam:IsAllowsOpenBordersToTeam(g_iThemTeam)) then
			strTheirTooltip = strTheirTooltip .. " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_OPEN_BORDERS_THEY_HAVE" );
		elseif gk_mode and (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
			strTheirTooltip = strTheirTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_THEY_NEED_EMBASSY_TT" ) .. "[ENDCOLOR]";
		end
	else
		Controls.ThemPocketOpenBorders:SetDisabled(false);
		Controls.ThemPocketOpenBorders:GetTextControl():SetColorByName("Beige_Black");
	end

	Controls.ThemPocketOpenBorders:SetToolTipString(strTheirTooltip);

	----------------------------------------------------------------------------------
	-- pocket Defensive Pact
	----------------------------------------------------------------------------------

	local bDefensivePactAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration);

	-- Are we not allowed to give DP? (don't have tech, or are already providing it to them)
	strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_TT", g_iDealDuration );
	if (not bDefensivePactAllowed) then

		local strDisabledTT = "";

		if (not g_pUsTeam:IsDefensivePactTradingAllowed() and not g_pThemTeam:IsDefensivePactTradingAllowed()) then
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_NO_TECH" );
		elseif gk_mode and (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) or not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT" );
		elseif (g_pUsTeam:IsDefensivePact(g_iThemTeam)) then
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_EXISTS" );
		else
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_NO_AGREEMENT" );
		end

		strDisabledTT = "[COLOR_WARNING_TEXT]" .. strDisabledTT .. "[ENDCOLOR]";
		strTooltip = strTooltip .. strDisabledTT;
	end

	Controls.UsPocketDefensivePact:SetToolTipString(strTooltip);
	Controls.ThemPocketDefensivePact:SetToolTipString(strTooltip);

	-- Are we not allowed to give DP? (don't have tech, or are already providing it to them)
	if (not bDefensivePactAllowed) then
		Controls.UsPocketDefensivePact:SetDisabled(true);
		Controls.UsPocketDefensivePact:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.UsPocketDefensivePact:SetDisabled(false);
		Controls.UsPocketDefensivePact:GetTextControl():SetColorByName("Beige_Black");
	end

	bDefensivePactAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration);

	-- Are they not allowed to give DP? (don't have tech, or are already providing it to us)
	if (not bDefensivePactAllowed) then
		Controls.ThemPocketDefensivePact:SetDisabled(true);
		Controls.ThemPocketDefensivePact:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.ThemPocketDefensivePact:SetDisabled(false);
		Controls.ThemPocketDefensivePact:GetTextControl():SetColorByName("Beige_Black");
	end

	----------------------------------------------------------------------------------
	-- pocket Research Agreement
	----------------------------------------------------------------------------------

	local bResearchAgreementAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration);

	local iCost = Game.GetResearchAgreementCost(g_iThem, g_iUs);

	-- Are we not allowed to give RA? (don't have tech, or are already providing it to them)
	strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_TT", iCost, g_iDealDuration );
	if (not bResearchAgreementAllowed) then
		local strDisabledTT = "";

		local pUsTeamTechs = g_pUsTeam:GetTeamTechs();
		local pTheirTeam = Teams[g_iThem];
		local pTheirTeamTechs = pTheirTeam:GetTeamTechs();

		if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
			strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_SCIENCE_OFF" );
		elseif (pUsTeamTechs:HasResearchedAllTechs() or pTheirTeamTechs:HasResearchedAllTechs()) then
			strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_ALL_TECHS_RESEARCHED" ) ;
		elseif (g_pUsTeam:IsHasResearchAgreement(g_iThemTeam)) then
			strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_EXISTS" ) ;
		else
			local bEmptyTT = true;
			if (not g_pUsTeam:IsResearchAgreementTradingAllowed() and not g_pThemTeam:IsResearchAgreementTradingAllowed()) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_TECH" ) ;
				bEmptyTT = false;
			end
			if gk_mode and (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) or not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT" ) ;
				bEmptyTT = false;
			end
			if gk_mode and (not g_pUs:IsDoF(g_iThem) or not g_pThem:IsDoF(g_iUs)) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEED_DOF_TT" ) ;
				bEmptyTT = false;
			end
			if (Players[g_iUs]:GetGold() < iCost or Players[g_iThem]:GetGold() < iCost or bEmptyTT) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_AGREEMENT" ) ;
-- bc1 add missing cash info
				local missingCash = iCost - g_Deal:GetGoldAvailable(g_iThem, TradeableItems.TRADE_ITEM_GOLD);
				local spacer = " ";
				if missingCash > 0 then
					strDisabledTT = strDisabledTT .. spacer .. g_pThem:GetName() ..": " .. missingCash;
					spacer = ", ";
				end
				local missingCash = iCost - g_Deal:GetGoldAvailable(g_iUs, TradeableItems.TRADE_ITEM_GOLD);
				if missingCash > 0 then
					strDisabledTT = strDisabledTT .. spacer .. Locale.ConvertTextKey("TXT_KEY_YOU") ..": " .. missingCash
				end
			end
		end

		strTooltip = strTooltip .. "[COLOR_WARNING_TEXT]" .. strDisabledTT .. "[ENDCOLOR]";
-- /bc1 add missing cash info

	end

	Controls.UsPocketResearchAgreement:SetToolTipString(strTooltip);
	Controls.ThemPocketResearchAgreement:SetToolTipString(strTooltip);

	if (not bResearchAgreementAllowed) then
		Controls.UsPocketResearchAgreement:SetDisabled(true);
		Controls.UsPocketResearchAgreement:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.UsPocketResearchAgreement:SetDisabled(false);
		Controls.UsPocketResearchAgreement:GetTextControl():SetColorByName("Beige_Black");
	end

	bResearchAgreementAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration);

	-- Are they not allowed to give RA? (don't have tech, or are already providing it to us)
	if (not bResearchAgreementAllowed) then
		Controls.ThemPocketResearchAgreement:SetDisabled(true);
		Controls.ThemPocketResearchAgreement:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.ThemPocketResearchAgreement:SetDisabled(false);
		Controls.ThemPocketResearchAgreement:GetTextControl():SetColorByName("Beige_Black");
	end

	----------------------------------------------------------------------------------
	-- pocket Trade Agreement
	----------------------------------------------------------------------------------

	local bTradeAgreementAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iDealDuration);

	-- Are we not allowed to give TA? (don't have tech, or are already providing it to them)
	strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_TT" );
	if (not bTradeAgreementAllowed) then

		local strDisabledTT = "";

		if (not g_pUsTeam:IsTradeAgreementTradingAllowed() and not g_pThemTeam:IsTradeAgreementTradingAllowed()) then
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_NO_TECH" );
		elseif (g_pUsTeam:IsHasTradeAgreement(g_iThemTeam)) then
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_EXISTS" );
		else
			strDisabledTT = " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_NO_AGREEMENT" );
		end

		strDisabledTT = "[COLOR_WARNING_TEXT]" .. strDisabledTT .. "[ENDCOLOR]";
		strTooltip = strTooltip .. strDisabledTT;
	end

	Controls.UsPocketTradeAgreement:SetToolTipString(strTooltip);
	Controls.ThemPocketTradeAgreement:SetToolTipString(strTooltip);

	-- Are we not allowed to give RA? (don't have tech, or are already providing it to them)
	if (not bTradeAgreementAllowed) then
		Controls.UsPocketTradeAgreement:SetDisabled(true);
		Controls.UsPocketTradeAgreement:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.UsPocketTradeAgreement:SetDisabled(false);
		Controls.UsPocketTradeAgreement:GetTextControl():SetColorByName("Beige_Black");
	end

	bTradeAgreementAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iDealDuration);

	-- Are they not allowed to give RA? (don't have tech, or are already providing it to us)
	if (not bTradeAgreementAllowed) then
		Controls.ThemPocketTradeAgreement:SetDisabled(true);
		Controls.ThemPocketTradeAgreement:GetTextControl():SetColorByName("Gray_Black");
	else
		Controls.ThemPocketTradeAgreement:SetDisabled(false);
		Controls.ThemPocketTradeAgreement:GetTextControl():SetColorByName("Beige_Black");
	end

	----------------------------------------------------------------------------------
	-- Pocket Declaration of Friendship
	----------------------------------------------------------------------------------

	if gk_mode and Controls.UsPocketDoF ~= nil and Controls.ThemPocketDoF ~= nil then
		if (g_bPVPTrade) then	-- Only PvP trade, with the AI there is a dedicated interface for this trade.

			strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP_TT");

			local bDoFAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iDealDuration);

			Controls.UsPocketDoF:SetHide(false);
			Controls.ThemPocketDoF:SetHide(false);

			if (not bDoFAllowed) then
				Controls.UsPocketDoF:SetDisabled(true);
				Controls.UsPocketDoF:GetTextControl():SetColorByName("Gray_Black");
				Controls.ThemPocketDoF:SetDisabled(true);
				Controls.ThemPocketDoF:GetTextControl():SetColorByName("Gray_Black");

				if ( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE] [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DECLARATION_OF_FRIENDSHIP_AT_WAR" ) .. "[ENDCOLOR]";
				else
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DECLARATION_OF_FRIENDSHIP_ALREADY_EXISTS" );
				end
			else
				Controls.UsPocketDoF:SetDisabled(false);
				Controls.UsPocketDoF:GetTextControl():SetColorByName("Beige_Black");
				Controls.ThemPocketDoF:SetDisabled(false);
				Controls.ThemPocketDoF:GetTextControl():SetColorByName("Beige_Black");
			end

			Controls.UsPocketDoF:SetToolTipString(strTooltip);
			Controls.ThemPocketDoF:SetToolTipString(strTooltip);

		else
			Controls.UsPocketDoF:SetHide(true);
			Controls.ThemPocketDoF:SetHide(true);
		end
	end

	-- Pocket Cities

	SetPocketCities( g_UsPocketCitiesIM, g_pUs, g_iUs, g_iThem )
	SetPocketCities( g_ThemPocketCitiesIM, g_pThem, g_iThem, g_iUs )

	-- pocket other civs
	SetPocketCivs( UsPocketPeaceButtons, g_iUs, g_iThem, TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE )
	SetPocketCivs( ThemPocketPeaceButtons, g_iThem, g_iUs, TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE )
	SetPocketCivs( UsPocketWarButtons, g_iUs, g_iThem, TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR )
	SetPocketCivs( ThemPocketWarButtons, g_iThem, g_iUs, TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR )

	----------------------------------------------------------------------------------
	-- pocket resources for us
	----------------------------------------------------------------------------------
	local bFoundLux = false;
	local bFoundStrat = false;
	local count;

	local iResourceCount;
	--local iOurResourceCount;
	--local iTheirResourceCount;

	-- loop over resources
	if( g_iUs == -1 ) then
		for resType, instance in pairs( g_UsPocketResources ) do
			instance.Button:SetHide( false );
		end
		bFoundLux = true;
		bFoundStrat = true;
	else

		local bCanTradeResource;

		for resourceID, instance in pairs( g_UsPocketResources ) do

			bCanTradeResource = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1);	-- 1 here is 1 quanity of the Resource, which is the minimum possible

			if (bCanTradeResource) then
				if Game.GetResourceUsageType(resourceID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
					bFoundLux = true
				else
					bFoundStrat = true
				end
				instance.Button:SetHide( false );

				local resource = GameInfo.Resources[resourceID];

				if gk_mode then
					iResourceCount = g_Deal:GetNumResource(g_iUs, resourceID);
				else
					iResourceCount = g_pUs:GetNumResourceAvailable( resourceID, false );
				end
				instance.Button:SetText( resource.IconString .. Locale.ConvertTextKey(resource.Description) .. " (" .. iResourceCount .. ")" )
			else
				instance.Button:SetHide( true );
			end
		end
	end

	----------------------------------------------------------------------------------
	-- pocket resources for them
	----------------------------------------------------------------------------------
	bFoundLux = false;
	bFoundStrat = false;
	if( g_iThem == -1 ) then
		for resourceID, instance in pairs( g_ThemPocketResources ) do
			instance.Button:SetHide( false );
		end
		bFoundLux = true;
		bFoundStrat = true;
	else

		local bCanTradeResource;

		for resourceID, instance in pairs( g_ThemPocketResources ) do

			bCanTradeResource = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1);	-- 1 here is 1 quanity of the Resource, which is the minimum possible

			if (bCanTradeResource) then
				if Game.GetResourceUsageType(resourceID) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY then
					bFoundLux = true;
				else
					bFoundStrat = true;
				end
				instance.Button:SetHide( false );

				local resource = GameInfo.Resources[resourceID];

				if gk_mode then
					iResourceCount = g_Deal:GetNumResource(g_iThem, resourceID);
				else
					iResourceCount = g_pThem:GetNumResourceAvailable( resourceID, false );
				end
				instance.Button:SetText( resource.IconString .. " " .. Locale.ConvertTextKey(resource.Description) .. " (" .. iResourceCount .. ")" )
			else
				instance.Button:SetHide( true );
			end
		end
	end
	Controls.ThemPocketStrategic:SetDisabled( not bFoundStrat );
	if (bFoundStrat) then
		Controls.ThemPocketStrategic:GetTextControl():SetColorByName("Beige_Black");
		Controls.ThemPocketStrategic:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES_THEM" ));
	else
		Controls.ThemPocketStrategic:GetTextControl():SetColorByName("Gray_Black");
		Controls.ThemPocketStrategic:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO_THEM" ));
	end


	----------------------------------------------------------------------------------
	-- Votes
	----------------------------------------------------------------------------------
	if bnw_mode then
		if not Game.IsOption("GAMEOPTION_NO_LEAGUES") then
			Controls.UsPocketVote:SetHide(false);
			Controls.ThemPocketVote:SetHide(false);

			RefreshPocketVotes(1);
			local bCanSellVote = g_pUs:CanCommitVote(g_pThem:GetID());
			local sTooltipDetails = g_pUs:GetCommitVoteDetails(g_pThem:GetID());
			Controls.UsPocketVote:SetDisabled( not bCanSellVote );
			if (bCanSellVote) then
				Controls.UsPocketVote:GetTextControl():SetColorByName("Beige_Black");
				Controls.UsPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_YES" ) .. sTooltipDetails);
			else
				Controls.UsPocketVote:GetTextControl():SetColorByName("Gray_Black");
				Controls.UsPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_NO" ) .. sTooltipDetails);
			end

			RefreshPocketVotes(0);
			local bCanBuyVote = g_pThem:CanCommitVote(g_pUs:GetID());
			local sTooltipDetails = g_pThem:GetCommitVoteDetails(g_pUs:GetID());
			Controls.ThemPocketVote:SetDisabled( not bCanBuyVote );
			if (bCanBuyVote) then
				Controls.ThemPocketVote:GetTextControl():SetColorByName("Beige_Black");
				Controls.ThemPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_YES_THEM" ) .. sTooltipDetails);
			else
				Controls.ThemPocketVote:GetTextControl():SetColorByName("Gray_Black");
				Controls.ThemPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_NO_THEM" ) .. sTooltipDetails);
			end

		else
			Controls.UsPocketVote:SetHide(true);
			Controls.ThemPocketVote:SetHide(true);
		end
	end --bnw_mode


	-- Hide things inappropriate for teammates
	if (g_iUsTeam == g_iThemTeam) then
		--print("Teams match!");
		if gk_mode then
			Controls.UsPocketAllowEmbassy:SetHide(true);
			Controls.ThemPocketAllowEmbassy:SetHide(true);
		end
		Controls.UsPocketOpenBorders:SetHide(true);
		Controls.UsPocketDefensivePact:SetHide(true);
		Controls.UsPocketResearchAgreement:SetHide(true);
		Controls.ThemPocketOpenBorders:SetHide(true);
		Controls.ThemPocketDefensivePact:SetHide(true);
		Controls.ThemPocketResearchAgreement:SetHide(true);
	else
		--print("Teams DO NOT match!");
		if gk_mode then
			Controls.UsPocketAllowEmbassy:SetHide(false);
			Controls.ThemPocketAllowEmbassy:SetHide(false);
		end
		Controls.UsPocketOpenBorders:SetHide(false);
		Controls.UsPocketDefensivePact:SetHide(false);
		Controls.UsPocketResearchAgreement:SetHide(false);
		Controls.ThemPocketOpenBorders:SetHide(false);
		Controls.ThemPocketDefensivePact:SetHide(false);
		Controls.ThemPocketResearchAgreement:SetHide(false);
	end

	for k in pairs( g_SubStacks ) do
		SubStackToggle( Controls[k], false ) -- false = not state change
	end
	ResizeStacks();
end


----------------------------------------------------------------
----------------------------------------------------------------
function DoClearTable()
	g_UsTableCitiesIM:ResetInstances();
	g_ThemTableCitiesIM:ResetInstances();
	if bnw_mode then
		g_UsTableVoteIM:ResetInstances();
		g_ThemTableVoteIM:ResetInstances();
		Controls.UsTableVoteStack:SetHide( true );
		Controls.ThemTableVoteStack:SetHide( true );
	end
	Controls.UsTablePeaceTreaty:SetHide( true );
	Controls.ThemTablePeaceTreaty:SetHide( true );
	Controls.UsTableGold:SetHide( true );
	Controls.ThemTableGold:SetHide( true );
	Controls.UsTableGoldPerTurn:SetHide( true );
	Controls.ThemTableGoldPerTurn:SetHide( true );
	if gk_mode then
		Controls.UsTableAllowEmbassy:SetHide( true );
		Controls.ThemTableAllowEmbassy:SetHide( true );
	end
	Controls.UsTableOpenBorders:SetHide( true );
	Controls.ThemTableOpenBorders:SetHide( true );
	Controls.UsTableDefensivePact:SetHide( true );
	Controls.ThemTableDefensivePact:SetHide( true );
	Controls.UsTableResearchAgreement:SetHide( true );
	Controls.ThemTableResearchAgreement:SetHide( true );
	Controls.UsTableTradeAgreement:SetHide( true );
	Controls.ThemTableTradeAgreement:SetHide( true );
--	Controls.UsTableCitiesStack:SetHide( true );
--	Controls.ThemTableCitiesStack:SetHide( true );
	Controls.UsTableStrategicStack:SetHide( true );
	Controls.ThemTableStrategicStack:SetHide( true );
	Controls.UsTableLuxuryStack:SetHide( true );
	Controls.ThemTableLuxuryStack:SetHide( true );

	Controls.UsTableMakePeaceStack:SetHide( true );
	Controls.ThemTableMakePeaceStack:SetHide( true );
	Controls.UsTableDeclareWarStack:SetHide( true );
	Controls.ThemTableDeclareWarStack:SetHide( true );
	for i, button in pairs( g_TableLeaderButtons ) do
		button:SetHide( true )
	end
	-- loop over resources
	for n, instance in pairs( g_UsTableResources ) do
		instance.Container:SetHide( true );
	end
	for n, instance in pairs( g_ThemTableResources ) do
		instance.Container:SetHide( true );
	end

	if Controls.UsTableDoF then
		Controls.UsTableDoF:SetHide(true);
	end
	if Controls.ThemTableDoF then
		Controls.ThemTableDoF:SetHide(true);
	end

	ResizeStacks();

end


----------------------------------------------------------------
----------------------------------------------------------------
function ResizeStacks()
	Controls.UsPocketStrategicStack:CalculateSize();
	Controls.ThemPocketStrategicStack:CalculateSize();
	Controls.UsTableStrategicStack:CalculateSize();
	Controls.ThemTableStrategicStack:CalculateSize();

	Controls.UsPocketLuxuryStack:CalculateSize();
	Controls.ThemPocketLuxuryStack:CalculateSize();
	Controls.UsTableLuxuryStack:CalculateSize();
	Controls.ThemTableLuxuryStack:CalculateSize();

	if bnw_mode then
		Controls.UsPocketVoteStack:CalculateSize();
		Controls.ThemPocketVoteStack:CalculateSize();
		Controls.UsTableVoteStack:CalculateSize();
		Controls.ThemTableVoteStack:CalculateSize();
	end
	Controls.UsTableCitiesStack:CalculateSize();
	Controls.ThemTableCitiesStack:CalculateSize();
	Controls.UsPocketLeaderStack:CalculateSize();
	Controls.ThemPocketLeaderStack:CalculateSize();

	Controls.UsTableMakePeaceStack:CalculateSize();
	Controls.UsTableDeclareWarStack:CalculateSize();
	Controls.ThemTableMakePeaceStack:CalculateSize();
	Controls.ThemTableDeclareWarStack:CalculateSize();


	Controls.UsPocketStack:CalculateSize();
	Controls.ThemPocketStack:CalculateSize();
	Controls.UsTableStack:CalculateSize();
	Controls.ThemTableStack:CalculateSize();

	Controls.UsPocketPanel:CalculateInternalSize();
	Controls.UsPocketPanel:ReprocessAnchoring();
	Controls.ThemPocketPanel:CalculateInternalSize();
	Controls.ThemPocketPanel:ReprocessAnchoring();
	Controls.UsTablePanel:CalculateInternalSize();
	Controls.UsTablePanel:ReprocessAnchoring();
	Controls.ThemTablePanel:CalculateInternalSize();
	Controls.ThemTablePanel:ReprocessAnchoring();

end


----------------------------------------------------------------
----------------------------------------------------------------
function DisplayDeal()
	g_UsTableCitiesIM:ResetInstances();
	g_ThemTableCitiesIM:ResetInstances();
	if bnw_mode then
		g_UsTableVoteIM:ResetInstances();
		g_ThemTableVoteIM:ResetInstances();
	end
	if g_iUs == -1 or g_iThem == -1 then
		return;
	end

	--print("Displaying Deal");

	local itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID, toPlayerID

	local strTooltip;

	ResetDisplay();

	local numItemsFromUs = 0;
	local numItemsFromThem = 0;

	g_Deal:ResetIterator();
	local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now

	--print("Going through check");
	repeat
		if bnw_mode then
			itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID = g_Deal:GetNextItem()
		else
			itemType, duration, finalTurn, data1, data2, fromPlayerID = g_Deal:GetNextItem()
		end
		if not itemType then break end
		local bFromUs = fromPlayerID == g_iUs

		--print("Adding trade item to active deal: " .. itemType);

		if bFromUs then
			toPlayerID = g_iThem
			numItemsFromUs = numItemsFromUs + 1;
		else
			toPlayerID = g_iUs
			numItemsFromThem = numItemsFromThem + 1;
		end

		if TradeableItems.TRADE_ITEM_PEACE_TREATY == itemType then
			local control = bFromUs and Controls.UsTablePeaceTreaty or Controls.ThemTablePeaceTreaty
			control:LocalizeAndSetText( "TXT_KEY_DIPLO_PEACE_TREATY", g_iPeaceDuration )
			control:SetHide( false )

		elseif TradeableItems.TRADE_ITEM_GOLD == itemType then

			Controls.UsPocketGold:SetHide( true );
			Controls.ThemPocketGold:SetHide( true );

			Controls.UsTableGold:SetHide( not bFromUs );
			Controls.ThemTableGold:SetHide( bFromUs );

			-- update quantity
			(bFromUs and Controls.UsGoldAmount or Controls.ThemGoldAmount):SetText( data1 );
			(bFromUs and Controls.UsGoldAmount or Controls.ThemTableGold):LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged) );

		elseif TradeableItems.TRADE_ITEM_GOLD_PER_TURN == itemType then

			Controls.UsPocketGoldPerTurn:SetHide( true );
			Controls.ThemPocketGoldPerTurn:SetHide( true );

			if bFromUs then
				Controls.UsTableGoldPerTurn:SetHide( false );
				Controls.UsGoldPerTurnTurns:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration );
				Controls.UsGoldPerTurnAmount:SetText( data1 );
				Controls.UsTableGoldPerTurn:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GPT", g_pUs:CalculateGoldRate() - data1 );
			else
				Controls.ThemTableGoldPerTurn:SetHide( false );
				Controls.ThemGoldPerTurnTurns:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration );
				Controls.ThemGoldPerTurnAmount:SetText( data1 );
				Controls.ThemTableGoldPerTurn:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GPT", g_pThem:CalculateGoldRate() - data1 );
			end

		elseif TradeableItems.TRADE_ITEM_CITIES == itemType then

			local plot = Map.GetPlot( data1, data2 )
			local city = plot and plot:GetPlotCity()
			local instance = (bFromUs and g_UsTableCitiesIM or g_ThemTableCitiesIM):GetInstance()
			if city then
				instance.Button:SetVoids( fromPlayerID, city:GetID() )
				instance.Button:RegisterCallback( Mouse.eLClick, RemoveCityTrade )
				instance.CityName:SetText( city:GetName() )
				instance.CityPop:SetText( city:GetPopulation() )
			else
				instance.Button:SetVoids( -1, -1 )
				instance.Button:ClearCallback( Mouse.eLClick )
				instance.CityName:LocalizeAndSetText( "TXT_KEY_RAZED_CITY" )
				instance.CityPop:SetText()
			end

		elseif TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE == itemType then
			if bFromUs then
				ShowTableCiv( UsTablePeaceButtons, UsPocketPeaceButtons, data1 )
			else
				ShowTableCiv( ThemTablePeaceButtons, ThemPocketPeaceButtons, data1 )
			end
		elseif TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR == itemType then
			if bFromUs then
				ShowTableCiv( UsTableWarButtons, UsPocketWarButtons, data1 )
			else
				ShowTableCiv( ThemTableWarButtons, ThemPocketWarButtons, data1 )
			end

		elseif gk_mode and TradeableItems.TRADE_ITEM_ALLOW_EMBASSY == itemType then
			if bFromUs then
				Controls.UsPocketAllowEmbassy:SetHide( true );
				Controls.UsTableAllowEmbassy:SetHide( false );
			else
				Controls.ThemPocketAllowEmbassy:SetHide( true );
				Controls.ThemTableAllowEmbassy:SetHide( false );
			end

		elseif TradeableItems.TRADE_ITEM_OPEN_BORDERS == itemType then

			if( bFromUs ) then
				Controls.UsPocketOpenBorders:SetHide( true );
				Controls.UsTableOpenBorders:SetHide( false );
			else
				Controls.ThemPocketOpenBorders:SetHide( true );
				Controls.ThemTableOpenBorders:SetHide( false );
			end

		elseif( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT == itemType ) then

			if( bFromUs ) then
				Controls.UsPocketDefensivePact:SetHide( true );
				Controls.UsTableDefensivePact:SetHide( false );
			else
				Controls.ThemPocketDefensivePact:SetHide( true );
				Controls.ThemTableDefensivePact:SetHide( false );
			end

		elseif( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT == itemType ) then

			if( bFromUs ) then
				Controls.UsPocketResearchAgreement:SetHide( true );
				Controls.UsTableResearchAgreement:SetHide( false );
			else
				Controls.ThemPocketResearchAgreement:SetHide( true );
				Controls.ThemTableResearchAgreement:SetHide( false );
			end

		elseif( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT == itemType ) then

			if( bFromUs ) then
				Controls.UsPocketTradeAgreement:SetHide( true );
				Controls.UsTableTradeAgreement:SetHide( false );
			else
				Controls.ThemPocketTradeAgreement:SetHide( true );
				Controls.ThemTableTradeAgreement:SetHide( false );
			end

		elseif( TradeableItems.TRADE_ITEM_RESOURCES == itemType ) then

			if( bFromUs ) then

				g_UsTableResources[ data1 ].Container:SetHide( false );
				g_UsTableResources[ data1 ].DurationEdit:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration );

				if( GameInfo.Resources[ data1 ].ResourceUsage == 1 ) then -- is strategic
					Controls.UsTableStrategicStack:SetHide( false );
					g_UsTableResources[ data1 ].AmountEdit:SetText( data2 );
				else
					Controls.UsTableLuxuryStack:SetHide( false );
				end
			else

				g_ThemTableResources[ data1 ].Container:SetHide( false );
				g_ThemTableResources[ data1 ].DurationEdit:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration );

				if( GameInfo.Resources[ data1 ].ResourceUsage == 1 ) then -- is strategic
					Controls.ThemTableStrategicStack:SetHide( false );
					g_ThemTableResources[ data1 ].AmountEdit:SetText( data2 );
				else
					Controls.ThemTableLuxuryStack:SetHide( false );
				end
			end

			g_UsPocketResources[ data1 ].Button:SetHide( true );
			g_ThemPocketResources[ data1 ].Button:SetHide( true );
		elseif gk_mode and ( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP == itemType ) then
			-- Declaration of Friendship must be mutual
			if (Controls.UsPocketDoF ~= nil) then
				Controls.UsPocketDoF:SetHide( true );
			end
			if (Controls.UsTableDoF ~= nil) then
				Controls.UsTableDoF:SetHide( false );
			end
			if (Controls.ThemPocketDoF ~= nil) then
				Controls.ThemPocketDoF:SetHide( true );
			end
			if (Controls.ThemTableDoF ~= nil) then
				Controls.ThemTableDoF:SetHide( false );
			end
		elseif bnw_mode and ( TradeableItems.TRADE_ITEM_VOTE_COMMITMENT == itemType ) then
			--print("==debug== VOTE_COMMITMENT found in DisplayDeal");
			local iVoteIndex = GetLeagueVoteIndexFromData(data1, data2, flag1);

			if (Game.GetNumActiveLeagues() > 0) then
				local pLeague = Game.GetActiveLeague();
				if (pLeague ~= nil) then
					if (iVoteIndex ~= nil and g_LeagueVoteList[iVoteIndex] ~= nil) then
						local tVote = g_LeagueVoteList[iVoteIndex];
						local sProposalText = GetVoteText(pLeague, iVoteIndex, flag1, data3);
						local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
						local sTooltip = GetVoteTooltip(pLeague, iVoteIndex, flag1, data3);

						if ( bFromUs ) then
							local cInstance = g_UsTableVoteIM:GetInstance();
							cInstance.ProposalLabel:SetText(sProposalText);
							cInstance.VoteLabel:SetText(sChoiceText);
							cInstance.Button:SetToolTipString(sTooltip);
							cInstance.Button:SetVoids( g_iUs, iVoteIndex );
							cInstance.Button:RegisterCallback( Mouse.eLClick, OnChooseTableVote );
							Controls.UsTableVoteStack:SetHide( false );
						else
							local cInstance = g_ThemTableVoteIM:GetInstance();
							cInstance.ProposalLabel:SetText(sProposalText);
							cInstance.VoteLabel:SetText(sChoiceText);
							cInstance.Button:SetToolTipString(sTooltip);
							cInstance.Button:SetVoids( g_iThem, iVoteIndex );
							cInstance.Button:RegisterCallback( Mouse.eLClick, OnChooseTableVote );
							Controls.ThemTableVoteStack:SetHide( false );
						end
					end
				else
					print("SCRIPTING ERROR: Could not find League when displaying a trade deal with a Vote Commitment");
				end
			end
		end
	until false

	DoUpdateButtons();

	ResizeStacks();
end





-----------------------------------------------------------------------------------------------------------------------
-- Gold Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function PocketGoldHandler( isUs )

		local iAmount = 30;

		local iAmountAvailable;

		local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now

		if( isUs == 1 ) then

			iAmountAvailable = g_Deal:GetGoldAvailable(g_iUs, iItemToBeChanged);
			if (iAmount > iAmountAvailable) then
				iAmount = iAmountAvailable;
			end
			g_Deal:AddGoldTrade( g_iUs, iAmount );

		else
			iAmountAvailable = g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged);
			if (iAmount > iAmountAvailable) then
				iAmount = iAmountAvailable;
			end
			g_Deal:AddGoldTrade( g_iThem, iAmount );
		end
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketGold:RegisterCallback( Mouse.eLClick, PocketGoldHandler );
	Controls.ThemPocketGold:RegisterCallback( Mouse.eLClick, PocketGoldHandler );
	Controls.UsPocketGold:SetVoid1( 1 );
	Controls.ThemPocketGold:SetVoid1( 0 );

	function TableGoldHandler( isUs )
		if( isUs == 1 ) then
			g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD, g_iUs );
		else
			g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD, g_iThem );
		end
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableGold:RegisterCallback( Mouse.eLClick, TableGoldHandler );
	Controls.ThemTableGold:RegisterCallback( Mouse.eLClick, TableGoldHandler );
	Controls.UsTableGold:SetVoid1( 1 );
	Controls.ThemTableGold:SetVoid1( 0 );

	function ChangeGoldAmount( string, control )

		local iGold = 0;
		if( string ~= nil and string ~= "" ) then
			iGold = tonumber(string);
		else
			control:SetText( 0 );
		end

		local iAmountAvailable;

		-- Gold from us
		if( control:GetVoid1() == 1 ) then

			iAmountAvailable = g_Deal:GetGoldAvailable(g_iUs, TradeableItems.TRADE_ITEM_GOLD);
			if (iGold > iAmountAvailable) then
				iGold = iAmountAvailable;
				Controls.UsGoldAmount:SetText(iGold);
			end

			g_Deal:ChangeGoldTrade( g_iUs, iGold );

			local iItemToBeChanged = -1;
			local strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iUs, iItemToBeChanged) );
			Controls.UsTableGold:SetToolTipString( strTooltip );

		-- Gold from them
		else

			iAmountAvailable = g_Deal:GetGoldAvailable(g_iThem, TradeableItems.TRADE_ITEM_GOLD);
			if (iGold > iAmountAvailable) then
				iGold = iAmountAvailable;
				Controls.ThemGoldAmount:SetText(iGold);
			end

			g_Deal:ChangeGoldTrade( g_iThem, iGold );

			local iItemToBeChanged = -1;
			local strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged) );
			Controls.ThemTableGold:SetToolTipString( strTooltip );
		end
	end
	Controls.UsGoldAmount:RegisterCallback( ChangeGoldAmount );
	Controls.UsGoldAmount:SetVoid1( 1 );
	Controls.ThemGoldAmount:RegisterCallback( ChangeGoldAmount );
	Controls.ThemGoldAmount:SetVoid1( 0 );
end

-----------------------------------------------------------------------------------------------------------------------
-- Gold Per Turn Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddGoldPerTurnTrade( playerID, player, goldPerTurn )
		g_Deal:AddGoldPerTurnTrade( playerID, math.min( goldPerTurn, player:CalculateGoldRate() ), g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketGoldPerTurn:RegisterCallback( Mouse.eLClick, function() AddGoldPerTurnTrade( g_iUs, g_pUs, 1 ) end )
	Controls.ThemPocketGoldPerTurn:RegisterCallback( Mouse.eLClick, function() AddGoldPerTurnTrade( g_iThem, g_pThem, 2 ) end )

	function RemoveGoldPerTurnTrade( playerID )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD_PER_TURN, playerID )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableGoldPerTurnButton:RegisterCallback( Mouse.eLClick, function() RemoveGoldPerTurnTrade( g_iUs ) end )
	Controls.ThemTableGoldPerTurnButton:RegisterCallback( Mouse.eLClick, function() RemoveGoldPerTurnTrade( g_iThem ) end )

	local function ChangeGoldPerTurnTrade( string, control, playerID, player, parentControl )
		local goldRate = player:CalculateGoldRate()
		local goldPerTurn = math.min( tonumber( string ) or 0, goldRate )
		control:SetText( goldPerTurn )

		g_Deal:ChangeGoldPerTurnTrade( playerID, goldPerTurn, g_iDealDuration )
		parentControl:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GPT", goldRate - goldPerTurn )
	end
	Controls.UsGoldPerTurnAmount:RegisterCallback( function( string, control ) ChangeGoldPerTurnTrade( string, control, g_iUs, g_pUs, Controls.UsTableGoldPerTurn ) end )
	Controls.ThemGoldPerTurnAmount:RegisterCallback( function( string, control ) ChangeGoldPerTurnTrade( string, control, g_iThem, g_pThem, Controls.ThemTableGoldPerTurn ) end )
end
-----------------------------------------------------------------------------------------------------------------------
-- Allow Embassy Handlers
-----------------------------------------------------------------------------------------------------------------------
if gk_mode then
	local function AddAllowEmbassy( playerID )
		g_Deal:AddAllowEmbassy( playerID )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketAllowEmbassy:RegisterCallback( Mouse.eLClick, function() AddAllowEmbassy( g_iUs ) end )
	Controls.ThemPocketAllowEmbassy:RegisterCallback( Mouse.eLClick, function() AddAllowEmbassy( g_iThem ) end )

	local function RemoveAllowEmbassy( playerID )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, playerID )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableAllowEmbassy:RegisterCallback( Mouse.eLClick, function() RemoveAllowEmbassy( g_iUs ) end );
	Controls.ThemTableAllowEmbassy:RegisterCallback( Mouse.eLClick, function() RemoveAllowEmbassy( g_iThem ) end );

end

-----------------------------------------------------------------------------------------------------------------------
-- Declaration of Friendship Handlers
-----------------------------------------------------------------------------------------------------------------------
if Controls.UsTableDoF and Controls.ThemTableDoF and Controls.UsPocketDoF and Controls.ThemPocketDoF then
	local function AddDeclarationOfFriendship()
		-- The Declaration of Friendship must be on both sides of the deal
		g_Deal:AddDeclarationOfFriendship( g_iUs )
		g_Deal:AddDeclarationOfFriendship( g_iThem )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketDoF:RegisterCallback( Mouse.eLClick, AddDeclarationOfFriendship )
	Controls.ThemPocketDoF:RegisterCallback( Mouse.eLClick, AddDeclarationOfFriendship )

	local function RemoveDeclarationOfFriendship()
		-- If removing the Declaration of Friendship, it must be removed from both sides
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableDoF:RegisterCallback( Mouse.eLClick, RemoveDeclarationOfFriendship )
	Controls.ThemTableDoF:RegisterCallback( Mouse.eLClick, RemoveDeclarationOfFriendship )
end

-----------------------------------------------------------------------------------------------------------------------
-- Open Borders Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddOpenBorders( playerID )

		g_Deal:AddOpenBorders( playerID, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketOpenBorders:RegisterCallback( Mouse.eLClick, function() AddOpenBorders( g_iUs ) end )
	Controls.ThemPocketOpenBorders:RegisterCallback( Mouse.eLClick, function() AddOpenBorders( g_iThem ) end )

	local function RemoveOpenBorders( playerID )

		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_OPEN_BORDERS, playerID )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableOpenBorders:RegisterCallback( Mouse.eLClick, function() RemoveOpenBorders( g_iUs ) end )
	Controls.ThemTableOpenBorders:RegisterCallback( Mouse.eLClick, function() RemoveOpenBorders( g_iThem ) end )
end

-----------------------------------------------------------------------------------------------------------------------
-- Defensive Pact Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddDefensivePact()

		-- Defensive Pact is required on both sides

		g_Deal:AddDefensivePact( g_iUs, g_iDealDuration )
		g_Deal:AddDefensivePact( g_iThem, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketDefensivePact:RegisterCallback( Mouse.eLClick, AddDefensivePact )
	Controls.ThemPocketDefensivePact:RegisterCallback( Mouse.eLClick, AddDefensivePact )

	local function RemoveDefensivePact()
		-- Remove from BOTH sides of the table
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableDefensivePact:RegisterCallback( Mouse.eLClick, RemoveDefensivePact )
	Controls.ThemTableDefensivePact:RegisterCallback( Mouse.eLClick, RemoveDefensivePact )
end

-----------------------------------------------------------------------------------------------------------------------
-- Research Agreement Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddResearchAgreement()

		-- Research Agreement is required on both sides
		g_Deal:AddResearchAgreement( g_iUs, g_iDealDuration )
		g_Deal:AddResearchAgreement( g_iThem, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketResearchAgreement:RegisterCallback( Mouse.eLClick, AddResearchAgreement )
	Controls.ThemPocketResearchAgreement:RegisterCallback( Mouse.eLClick, AddResearchAgreement )

	local function RemoveResearchAgreement()
		-- Remove from BOTH sides of the table
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableResearchAgreement:RegisterCallback( Mouse.eLClick, RemoveResearchAgreement )
	Controls.ThemTableResearchAgreement:RegisterCallback( Mouse.eLClick, RemoveResearchAgreement )
end

-----------------------------------------------------------------------------------------------------------------------
-- Trade Agreement Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddTradeAgreement()

		-- Trade Agreement is required on both sides

		g_Deal:AddTradeAgreement( g_iUs, g_iDealDuration )
		g_Deal:AddTradeAgreement( g_iThem, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketTradeAgreement:RegisterCallback( Mouse.eLClick, AddTradeAgreement )
	Controls.ThemPocketTradeAgreement:RegisterCallback( Mouse.eLClick, AddTradeAgreement )

	local function RemoveTradeAgreement()
		-- Remove from BOTH sides of the table
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableTradeAgreement:RegisterCallback( Mouse.eLClick, RemoveTradeAgreement )
	Controls.ThemTableTradeAgreement:RegisterCallback( Mouse.eLClick, RemoveTradeAgreement )
end

-----------------------------------------------------------------------------------------------------------------------
-- Handle the strategic and luxury resources
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddResourceTrade( playerID, resourceId )

		local resourceQuantity = 1

		if gk_mode then
			resourceQuantity = math.min( g_Deal:GetNumResource( playerID, resourceId ), resourceQuantity )
		else
			resourceQuantity = math.min( Players[playerID]:GetNumResourceAvailable( resourceId, false ), resourceQuantity )
		end
		g_Deal:AddResourceTrade( playerID, resourceId, resourceQuantity, g_iDealDuration )
		DoUIDealChangedByHuman()
	end

	local function AddResourceTradeUs( resourceId )
		AddResourceTrade( g_iUs, resourceId )
	end

	local function AddResourceTradeThem( resourceId )
		AddResourceTrade( g_iThem, resourceId )
	end

	local function RemoveResourceTrade( resourceId )
		g_Deal:RemoveResourceTrade( resourceId )
		DoUIDealChangedByHuman( true )
	end

	local function AddResourceInstance( resource, instanceName, stack, list, clickAction, changeAction )
		local instance = {}
		ContextPtr:BuildInstanceForControl( instanceName, instance, stack )
		instance.Button:SetText( resource.IconString .. Locale.ConvertTextKey( resource.Description ) )
		instance.Button:SetVoid1( resource.ID )
		instance.Button:RegisterCallback( Mouse.eLClick, clickAction )
		list[ resource.ID ] = instance
		if changeAction then
			instance.AmountEdit:SetVoid1( resource.ID )
			instance.AmountEdit:RegisterCallback( changeAction )
		end
	end

	local function ChangeResourceAmount( string, control, playerID )

		local resourceID = control:GetVoid1()
		local resourceQuantity = tonumber(string) or 0

		-- Can't offer more than someone has
		if gk_mode then
			resourceQuantity = math.min( g_Deal:GetNumResource( playerID, resourceID ), resourceQuantity )
		else
			resourceQuantity = math.min( Players[ playerID ]:GetNumResourceAvailable( resourceID, false ), resourceQuantity )
		end

		control:SetText( resourceQuantity )

		g_Deal:ChangeResourceTrade( playerID, resourceID, resourceQuantity, g_iDealDuration )
	end

	local function ChangeResourceAmountUs( string, control )
		ChangeResourceAmount( string, control, g_iUs )
	end

	local function ChangeResourceAmountThem( string, control )
		ChangeResourceAmount( string, control, g_iThem )
	end

	--print( "adding strategic resources" )
	for row in GameInfo.Resources( "ResourceUsage = 1" ) do
		AddResourceInstance( row, "PocketResource", Controls.UsPocketStrategicStack, g_UsPocketResources, AddResourceTradeUs )
		AddResourceInstance( row, "PocketResource", Controls.ThemPocketStrategicStack, g_ThemPocketResources, AddResourceTradeThem )
		AddResourceInstance( row, "TableStrategic", Controls.UsTableStrategicStack, g_UsTableResources, RemoveResourceTrade, ChangeResourceAmountUs )
		AddResourceInstance( row, "TableStrategic", Controls.ThemTableStrategicStack, g_ThemTableResources, RemoveResourceTrade, ChangeResourceAmountThem )
	end
	--print( "adding luxuries" )
	for row in GameInfo.Resources( "ResourceUsage = 2" ) do
		AddResourceInstance( row, "PocketResource", Controls.UsPocketLuxuryStack, g_UsPocketResources, AddResourceTradeUs )
		AddResourceInstance( row, "PocketResource", Controls.ThemPocketLuxuryStack, g_ThemPocketResources, AddResourceTradeThem )
		AddResourceInstance( row, "TableLuxury", Controls.UsTableLuxuryStack, g_UsTableResources, RemoveResourceTrade )
		AddResourceInstance( row, "TableLuxury", Controls.ThemTableLuxuryStack, g_ThemTableResources, RemoveResourceTrade )
	end
end

if bnw_mode then
	-----------------------------------------------------------------------------------------------------------------------
	-- Handle votes
	-----------------------------------------------------------------------------------------------------------------------

	-- Update a global table, refer to proposal and vote pair as a single integer index because of SetVoids limit
	local function UpdateLeagueVotes()
		g_LeagueVoteList = {};

		local pLeague = nil;
		if (Game.GetNumActiveLeagues() > 0) then
			pLeague = Game.GetActiveLeague();
		end

		if (pLeague ~= nil) then
			local tEnactProposals = pLeague:GetEnactProposals();
			for i,t in ipairs(tEnactProposals) do
				local iVoterDecision = GameInfo.ResolutionDecisions[GameInfo.Resolutions[t.Type].VoterDecision].ID;
				local tVoterChoices = pLeague:GetChoicesForDecision(iVoterDecision);
				for i,iChoice in ipairs(tVoterChoices) do
					local tLeagueVote = { Type = t.Type, VoteDecision = iVoterDecision, ID = t.ID, ProposerChoice = t.ProposerDecision, VoteChoice = iChoice, Repeal = false };
					table.insert(g_LeagueVoteList, tLeagueVote);
				end
			end

			local tRepealProposals = pLeague:GetRepealProposals();
			for i,t in ipairs(tRepealProposals) do
				local iVoterDecision = GameInfo.ResolutionDecisions["RESOLUTION_DECISION_REPEAL"].ID; --antonjs: temp
				local tVoterChoices = pLeague:GetChoicesForDecision(iVoterDecision);
				for i,iChoice in ipairs(tVoterChoices) do
					local tLeagueVote = { Type = t.Type, VoteDecision = iVoterDecision, ID = t.ID, ProposerChoice = t.ProposerDecision, VoteChoice = iChoice, Repeal = true };
					table.insert(g_LeagueVoteList, tLeagueVote);
				end
			end
		end
	end

	function GetLeagueVoteIndexFromData(iID, iVoteChoice, bRepeal)
		local iFound = nil;
		for iIndex,tVote in ipairs(g_LeagueVoteList) do
			if (tVote.ID == iID and tVote.VoteChoice == iVoteChoice and tVote.Repeal == bRepeal) then
				iFound = iIndex;
				break;
			end
		end
		return iFound;
	end

	function OnChoosePocketVote( fromPlayerID, iVoteIndex )
		local pLeague = nil;
		if (Game.GetNumActiveLeagues() > 0) then
			pLeague = Game.GetActiveLeague();
		end

		if (pLeague ~= nil and g_LeagueVoteList[iVoteIndex] ~= nil) then
			local iResolutionID = g_LeagueVoteList[iVoteIndex].ID;
			local iVoteChoice = g_LeagueVoteList[iVoteIndex].VoteChoice;
			local iNumVotes = pLeague:GetCoreVotesForMember(fromPlayerID);
			local bRepeal = g_LeagueVoteList[iVoteIndex].Repeal;
			--print("==debug== Vote added to deal, ID=" .. iResolutionID .. ", VoteChoice=" .. iVoteChoice .. ", NumVotes=" .. iNumVotes);
			g_Deal:AddVoteCommitment(fromPlayerID, iResolutionID, iVoteChoice, iNumVotes, bRepeal);

			DoUIDealChangedByHuman()
		else
			print("SCRIPTING ERROR: Could not find valid vote when pocket vote was selected");
		end
	end

	function RefreshPocketVotes(iIsUs)
		UpdateLeagueVotes();
		if (iIsUs == 1) then
			g_UsPocketVoteIM:ResetInstances();
		else
			g_ThemPocketVoteIM:ResetInstances();
		end

		local pLeague = nil;
		if (Game.GetNumActiveLeagues() > 0) then
			pLeague = Game.GetActiveLeague();
		end

		if (pLeague ~= nil) then
			for i,tVote in ipairs(g_LeagueVoteList) do
				-- Us to them?
				if (iIsUs == 1) then
					local iNumVotes = pLeague:GetCoreVotesForMember(g_iUs);
					local sProposalText = GetVoteText(pLeague, i, tVote.Repeal, iNumVotes);
					local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
					local sTooltip = GetVoteTooltip(pLeague, i, tVote.Repeal, iNumVotes);
					if (g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, tVote.ID, tVote.VoteChoice, iNumVotes, tVote.Repeal)) then
						local cInstance = g_UsPocketVoteIM:GetInstance();
						cInstance.ProposalLabel:SetText(sProposalText);
						cInstance.VoteLabel:SetText(sChoiceText);
						cInstance.Button:SetToolTipString(sTooltip);
						cInstance.Button:SetVoids(g_iUs, i);
						cInstance.Button:RegisterCallback( Mouse.eLClick, OnChoosePocketVote );
					end
				-- Them to us?
				else
					local iNumVotes = pLeague:GetCoreVotesForMember(g_iThem);
					local sProposalText = GetVoteText(pLeague, i, tVote.Repeal, iNumVotes);
					local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
					local sTooltip = GetVoteTooltip(pLeague, i, tVote.Repeal, iNumVotes);
					if (g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, tVote.ID, tVote.VoteChoice, iNumVotes, tVote.Repeal)) then
						local cInstance = g_ThemPocketVoteIM:GetInstance();
						cInstance.ProposalLabel:SetText(sProposalText);
						cInstance.VoteLabel:SetText(sChoiceText);
						cInstance.Button:SetToolTipString(sTooltip);
						cInstance.Button:SetVoids(g_iThem, i);
						cInstance.Button:RegisterCallback( Mouse.eLClick, OnChoosePocketVote );
					end
				end
			end
		end
	end

	function OnChooseTableVote(fromPlayerID, iVoteIndex)
		local pLeague = nil;
		if (Game.GetNumActiveLeagues() > 0) then
			pLeague = Game.GetActiveLeague();
		end

		if (pLeague ~= nil and g_LeagueVoteList[iVoteIndex] ~= nil) then
			local iResolutionID = g_LeagueVoteList[iVoteIndex].ID;
			local iVoteChoice = g_LeagueVoteList[iVoteIndex].VoteChoice;
			local iNumVotes = pLeague:GetCoreVotesForMember(fromPlayerID);
			local bRepeal = g_LeagueVoteList[iVoteIndex].Repeal;
			--print("==debug== Vote removed from deal, ID=" .. iResolutionID .. ", VoteChoice=" .. iVoteChoice .. ", NumVotes=" .. iNumVotes);
			g_Deal:RemoveVoteCommitment(fromPlayerID, iResolutionID, iVoteChoice, iNumVotes, bRepeal);

			DoUIDealChangedByHuman( true )
		else
			print("SCRIPTING ERROR: Could not find valid vote when pocket vote was selected");
		end
	end

	function GetVoteText(pLeague, iVoteIndex, bRepeal, iNumVotes)
		local s = "";
		if (pLeague ~= nil and iVoteIndex ~= nil) then
			if (g_LeagueVoteList[iVoteIndex] ~= nil) then
				local tVote = g_LeagueVoteList[iVoteIndex];
				s = pLeague:GetResolutionName(tVote.Type, tVote.ID, tVote.ProposerChoice, false);
				if (tVote.Repeal) then
					s = "[COLOR_WARNING_TEXT]" .. s .. "[ENDCOLOR]";
				else
					s = "[COLOR_POSITIVE_TEXT]" .. s .. "[ENDCOLOR]";
				end
			end
		end

		return s;
	end

	function GetVoteTooltip(pLeague, iVoteIndex, bRepeal, iNumVotes)
		local s = "";
		if (pLeague ~= nil and iVoteIndex ~= nil) then
			if (g_LeagueVoteList[iVoteIndex] ~= nil) then
				local tVote = g_LeagueVoteList[iVoteIndex];
				local sVoteChoice = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
				local sProposalInfo = "";
				if (GameInfo.Resolutions[tVote.Type] ~= nil and GameInfo.Resolutions[tVote.Type].Help ~= nil) then
					sProposalInfo = sProposalInfo .. Locale.Lookup(GameInfo.Resolutions[tVote.Type].Help);
				end

				if (bRepeal) then
					s = Locale.Lookup("TXT_KEY_DIPLO_VOTE_TRADE_REPEAL_TT", iNumVotes, sVoteChoice, sProposalInfo);
				else
					s = Locale.Lookup("TXT_KEY_DIPLO_VOTE_TRADE_ENACT_TT", iNumVotes, sVoteChoice, sProposalInfo);
				end
			end
		end

		return s;
	end
end --bnw_mode

-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------


----------------------------------------------------
-- handlers for when a leader is clicked
----------------------------------------------------
local function AddThirdPartyPeace( firstPartyID, playerID )
	g_Deal:AddThirdPartyPeace( firstPartyID, Players[ playerID ]:GetTeam(), g_iPeaceDuration )
	DoUIDealChangedByHuman()
end

local function AddThirdPartyWar( firstPartyID, playerID )
	g_Deal:AddThirdPartyWar( firstPartyID, Players[ playerID ]:GetTeam() )
	DoUIDealChangedByHuman()
end

local function RemoveThirdPartyPeace( firstPartyID, playerID )

	local player = Players[ playerID ]
	-- if this is a peace negotiation
	local allyID = g_pUsTeam:IsAtWar( g_iThemTeam ) and player:IsMinorCiv() and player:GetAlly()
	if not allyID or ( allyID ~= g_iUs and allyID ~= g_iThem ) then
		--print("Removing peace deal")
		g_Deal:RemoveThirdPartyPeace( firstPartyID, player:GetTeam() )
		DoUIDealChangedByHuman( true )
	end
end

local function RemoveThirdPartyWar( firstPartyID, playerID )
	g_Deal:RemoveThirdPartyWar( firstPartyID, Players[ playerID ]:GetTeam() )
	DoUIDealChangedByHuman( true )
end


-----------------------------------------------------------------------------------------------------------------------
-- Build the THIRD_PARTY lists. These are also know as thirdparty if you're doing a text search
-- the only thing we trade in third party mode is war/peace, so make sure those aren't disabled
-----------------------------------------------------------------------------------------------------------------------

if( Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR ) or
	Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE ) or
	Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE ) ) then

	g_bEnableThirdParty = false;

else
	g_bEnableThirdParty = true;

	local function InitLeaderStack( stack, onButtonClick, masterButtonList )
		local buttonList = {Parent = stack}
--print("init leader stack", stack:GetID(), onButtonClick )
		for playerID = 0, GameDefines.MAX_CIV_PLAYERS-1 do

			local player = Players[ playerID ]

			if player and player:IsEverAlive() then

				local leaderName;
				if player:IsHuman() then
					leaderName = player:GetNickName();
				else
					leaderName = player:GetName();
				end
				leaderName = leaderName .. " (" .. Locale.ConvertTextKey(GameInfo.Civilizations[player:GetCivilizationType()].ShortDescription) .. ")";

				local instance = {}
				ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", instance, stack )
				CivIconHookup( playerID, 32, instance.CivSymbol, instance.CivIconBG, instance.CivIconShadow, false, true )
				TruncateString( instance.Name, instance.ButtonSize:GetSizeX() - instance.Name:GetOffsetX(), leaderName )
				local button = instance.Button
				button:SetVoid1( playerID )
				button:RegisterCallback( Mouse.eLClick, onButtonClick )
				buttonList[ playerID ] = button
				table.insert( masterButtonList, button )
			end
		end
		return buttonList
	end
	UsPocketPeaceButtons = InitLeaderStack( Controls.UsPocketOtherPlayerStack, function( playerID ) AddThirdPartyPeace( g_iUs, playerID, UsPocketPeaceButtons ) end, g_PocketLeaderButtons )
	UsPocketWarButtons = InitLeaderStack( Controls.UsPocketLeaderStack, function( playerID ) AddThirdPartyWar( g_iUs, playerID, UsPocketWarButtons ) end, g_PocketLeaderButtons )
	ThemPocketPeaceButtons = InitLeaderStack( Controls.ThemPocketOtherPlayerStack, function( playerID ) AddThirdPartyPeace( g_iThem, playerID, ThemPocketPeaceButtons ) end, g_PocketLeaderButtons )
	ThemPocketWarButtons = InitLeaderStack( Controls.ThemPocketLeaderStack, function( playerID ) AddThirdPartyWar( g_iThem, playerID, ThemPocketWarButtons ) end, g_PocketLeaderButtons )
	UsTablePeaceButtons = InitLeaderStack( Controls.UsTableMakePeaceStack, function( playerID ) RemoveThirdPartyPeace( g_iUs, playerID, UsPocketPeaceButtons ) end, g_TableLeaderButtons )
	UsTableWarButtons = InitLeaderStack( Controls.UsTableDeclareWarStack, function( playerID ) RemoveThirdPartyWar( g_iUs, playerID, UsPocketWarButtons ) end, g_TableLeaderButtons )
	ThemTablePeaceButtons = InitLeaderStack( Controls.ThemTableMakePeaceStack, function( playerID ) RemoveThirdPartyPeace( g_iThem, playerID, ThemPocketPeaceButtons ) end, g_TableLeaderButtons )
	ThemTableWarButtons = InitLeaderStack( Controls.ThemTableDeclareWarStack, function( playerID ) RemoveThirdPartyWar( g_iThem, playerID, ThemPocketWarButtons ) end, g_TableLeaderButtons )
end
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------

Controls.UsMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.UsDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.ThemMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.ThemDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )

ResetDisplay();
DisplayDeal();

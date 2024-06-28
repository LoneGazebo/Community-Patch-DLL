print("This is the modded TradeLogic from Vox Populi")
----------------------------------------------------------------        
----------------------------------------------------------------     
-- modified by bc1 from 1.0.3.276 brave new world code
-- code is common using gk_mode and bnw_mode switches
-- show missing cash for trade agreements

local civ5_mode = InStrategicView ~= nil
local bnw_mode = not civ5_mode or Game.GetActiveLeague ~= nil
local gk_mode = bnw_mode or Game.GetReligionName ~= nil
if not civ5_mode then --no glass panel in civBE
	Controls.UsGlass = Controls.UsPanel
	Controls.ThemGlass = Controls.ThemPanel
end
----------------------------------------------------------------
----------------------------------------------------------------
include( "IconSupport" ) local IconHookup = IconHookup local CivIconHookup = CivIconHookup
include( "InstanceManager" ) local GenerationalInstanceManager = GenerationalInstanceManager
include( "SupportFunctions" ) local TruncateString = TruncateString
include( "TechHelpInclude" );

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

local g_PeaceDealDuration = GameInfo.GameSpeeds[PreGame.GetGameSpeed()].PeaceDealDuration;
local g_bAllowResearchAgreements = Game.IsOption("GAMEOPTION_RESEARCH_AGREEMENTS");
local g_bDisableScience = Game.IsOption("GAMEOPTION_NO_SCIENCE");
local g_bDisableTechTrading = Game.IsOption("GAMEOPTION_NO_TECH_TRADING");
local g_bDisableVassalage = Game.IsOption("GAMEOPTION_NO_VASSALAGE");
local g_bDisableLeague = Game.IsOption("GAMEOPTION_NO_LEAGUES");

----------------------------------------------------------------
-- local storage
----------------------------------------------------------------
local g_Deal = UI.GetScratchDeal();
local g_iDiploUIState;
local g_bPVPTrade;
local g_bTradeReview = false;
local g_iNumOthers = 0;
local g_iNumOtherNonVassals = 0;
local g_bEnableThirdParty = true;

local g_iConcessionsPreviousDiploUIState = -1;
local g_bHumanOfferingConcessions = false;

local g_iDealDuration = Game.GetDealDuration();
local g_iPeaceDuration = gk_mode and Game.GetPeaceDuration() or 10;

local g_iUs = -1; --Game.GetActivePlayer();
local g_iThem = -1;
local g_pUs = -1;
local g_pThem = -1;
local g_iUsTeam = -1;
local g_iThemTeam = -1;
local g_pUsTeam = -1;
local g_pThemTeam = -1;
local g_UsPocketResources = {};
local g_ThemPocketResources = {};
local g_UsTableResources = {};
local g_ThemTableResources = {};
local g_LuxuryList = {};
local g_LeagueVoteList = {};
-- Putmalk: Init our arrays
local g_UsPocketTechs 		= {};
local g_ThemPocketTechs 	= {};
local g_UsTableTechs 		= {};
local g_ThemTableTechs 		= {};

local g_bMessageFromDiploAI = false;
local g_bAIMakingOffer = false;

local g_bAnyLuxuryUs = false;
local g_bAnyStrategicUs = false;
local g_bAnyTechUs = false;
local g_bAnyVoteUs = false;
local g_bAnyLuxuryThem = false;
local g_bAnyStrategicThem = false;
local g_bAnyTechThem = false;
local g_bAnyVoteThem = false;

local g_UsOtherPlayerMode = -1;
local g_ThemOtherPlayerMode = -1;

local g_OtherPlayersButtons = {};

local g_bNewDeal = true;

local WAR = 0;
local PEACE = 1;

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


---------------------------------------------------------
-- LEADER MESSAGE HANDLER
---------------------------------------------------------
function LeaderMessageHandler( iPlayer, iDiploUIState, szLeaderMessage, iAnimationAction, iData1 )
	g_bPVPTrade = false;

	-- If we were just in discussion mode and the human offered to make concessions, make a note of that
	if (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS) then
		if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV) then
			--print("Human offers concessions for killing Minor Civ");
			g_iConcessionsPreviousDiploUIState = g_iDiploUIState;
		elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING) then
			--print("Human offers concessions for expansion");
			g_iConcessionsPreviousDiploUIState = g_iDiploUIState;
		elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING) then
			--print("Human offers concessions for plot buying");
			g_iConcessionsPreviousDiploUIState = g_iDiploUIState;
		end
	end

	g_iDiploUIState = iDiploUIState;

	g_iUs = Game.GetActivePlayer();
	g_pUs = Players[ g_iUs ];

	g_iUsTeam = g_pUs:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];

	g_iThem = iPlayer;
	g_pThem = Players[ g_iThem ];
	g_iThemTeam = g_pThem:GetTeam();
	g_pThemTeam = Teams[ g_iThemTeam ];


	-- if the AI offers a deal, its valuation might have changed during the AI's turn. Reevaluate the deal and change deal items if necessary
	if(g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
		if(g_Deal:IsCheckedForRenewal()) then
			-- modify leader message if necessary
			szLeaderMessage = g_Deal:GetRenewDealMessage(g_iThem, g_iUs);
		end
	end
	
	if(g_iDiploUIState == DiploUIStateTypes.NO_DIPLO_UI_STATE) then
		OnBack();
	else
		
		-- Leader head fix for more than 22 civs DLL...	
		local playerLeaderInfo = GameInfo.Leaders[Players[iPlayer]:GetLeaderType()];
		local leaderTextures = {
			["LEADER_ALEXANDER"] = "alexander.dds",
			["LEADER_ASKIA"] = "askia.dds",
			["LEADER_AUGUSTUS"] = "augustus.dds",
			["LEADER_BISMARCK"] = "bismark.dds",
			["LEADER_CATHERINE"] = "catherine.dds",
			["LEADER_DARIUS"] = "darius.dds",
			["LEADER_ELIZABETH"] = "elizabeth.dds",
			["LEADER_GANDHI"] = "ghandi.dds",
			["LEADER_HARUN_AL_RASHID"] = "alrashid.dds",
			["LEADER_HIAWATHA"] = "hiawatha.dds",
			["LEADER_MONTEZUMA"] = "montezuma.dds",
			["LEADER_NAPOLEON"] = "napoleon.dds",
			["LEADER_ODA_NOBUNAGA"] = "oda.dds",
			["LEADER_RAMESSES"] = "ramesses.dds",
			["LEADER_RAMKHAMHAENG"] = "ramkhamaeng.dds",
			["LEADER_SULEIMAN"] = "sulieman.dds",
			["LEADER_WASHINGTON"] = "washington.dds",
			["LEADER_WU_ZETIAN"] = "wu.dds",
			["LEADER_GENGHIS_KHAN"] = "genghis.dds",
			["LEADER_ISABELLA"] = "isabella.dds",
			["LEADER_PACHACUTI"] = "pachacuti.dds",
			["LEADER_KAMEHAMEHA"] = "kamehameha.dds",
			["LEADER_HARALD"] = "harald.dds",
			["LEADER_SEJONG"] = "sejong.dds",
			["LEADER_NEBUCHADNEZZAR"] = "nebuchadnezzar.dds",
			["LEADER_ATTILA"] = "attila.dds",
			["LEADER_BOUDICCA"] = "boudicca.dds",
			["LEADER_DIDO"] = "dido.dds",
			["LEADER_GUSTAVUS_ADOLPHUS"] = "gustavus adolphus.dds",
			["LEADER_MARIA"] = "mariatheresa.dds",
			["LEADER_PACAL"] = "pacal_the_great.dds",
			["LEADER_THEODORA"] = "theodora.dds",
			["LEADER_SELASSIE"] = "haile_selassie.dds",
			["LEADER_WILLIAM"] = "william_of_orange.dds",		
			["LEADER_SHAKA"] = "Shaka.dds",
			["LEADER_POCATELLO"] = "Pocatello.dds",
			["LEADER_PEDRO"] = "Pedro.dds",
			["LEADER_MARIA_I"] = "Maria_I.dds",
			["LEADER_GAJAH_MADA"] = "Gajah.dds",
			["LEADER_ENRICO_DANDOLO"] = "Dandolo.dds",
			["LEADER_CASIMIR"] = "Casimir.dds",
			["LEADER_ASHURBANIPAL"] = "Ashurbanipal.dds",
			["LEADER_AHMAD_ALMANSUR"] = "Almansur.dds",
		}
		
		if iPlayer > 21 then
			print ("LeaderMessageHandler: Player ID > 21")
			local backupTexture = "loadingbasegame_9.dds"
			if leaderTextures[playerLeaderInfo.Type] then
				backupTexture = leaderTextures[playerLeaderInfo.Type]
			end

			-- get screen and texture size to set the texture on full screen

			Controls.BackupTexture:SetTexture( backupTexture )
			local screenW, screenH = Controls.BackupTexture:GetSizeVal() -- works, but maybe there is a direct way to get screen size ?
				
			Controls.BackupTexture:SetTextureAndResize( backupTexture )
			local textureW, textureH = Controls.BackupTexture:GetSizeVal()	
			
			print ("Screen Width = " .. tostring(screenW) .. ", Screen Height = " .. tostring(screenH) .. ", Texture Width = " .. tostring(textureW) .. ", Texture Height = " .. tostring(textureH))

			local ratioW = screenW / textureW
			local ratioH = screenH / textureH
			
			print ("Width ratio = " .. tostring(ratioW) .. ", Height ratio = " .. tostring(ratioH))

			local ratio = ratioW
			if ratioH > ratioW then
				ratio = ratioH
			end
			Controls.BackupTexture:SetSizeVal( math.floor(textureW * ratio), math.floor(textureH * ratio) )

			Controls.BackupTexture:SetHide( false )
		else
			
			Controls.BackupTexture:UnloadTexture()
			Controls.BackupTexture:SetHide( true )

		end
		-- End of leader head fix
		
		
		-- Are we in Trade Mode?
		if iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND
			or iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER
		then

			--print("TradeScreen: It's MY mode!");

			if (ContextPtr:IsHidden()) then
				UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderTrade );
			end

			--print("Handling LeaderMessage: " .. iDiploUIState .. ", ".. szLeaderMessage);

			g_Deal:SetFromPlayer(g_iUs);
			g_Deal:SetToPlayer(g_iThem);

			-- Unhide our pocket, in case the last thing we were doing in this screen was a human demand
			Controls.UsPanel:SetHide(false);
			Controls.UsGlass:SetHide(false);

			local bClearTableAndDisplayDeal = false;

			-- Is this a UI State where we should be displaying a deal?
			if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE) then
				--print("DiploUIState: Default Trade");
				bClearTableAndDisplayDeal = true;
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND) then
				--print("DiploUIState: AI making demand");
				bClearTableAndDisplayDeal = true;

				DoDemandState(true);

			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
				--print("DiploUIState: AI making Request");
				bClearTableAndDisplayDeal = true;

				DoDemandState(true);
			-- Putmalk: Open Trade window when AI is giving a gift and set it to the demand state
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER) then
				--print("DiploUIState: AI making Offer");
				bClearTableAndDisplayDeal = true;
				
				DoDemandState(true);
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
				bClearTableAndDisplayDeal = true;
				-- If we're demanding something, there's no need to show OUR items
				Controls.UsPanel:SetHide(true);
				Controls.UsGlass:SetHide(true);

			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS) then
				--print("DiploUIState: Human offers concessions");
				bClearTableAndDisplayDeal = true;
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
				--print("DiploUIState: AI making offer");
				bClearTableAndDisplayDeal = true;
				g_bAIMakingOffer = true;
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER) then
				--print("DiploUIState: AI accepted offer");
				g_iConcessionsPreviousDiploUIState = -1;		-- Clear out the fact that we were offering concessions if the AI has agreed to a deal
				bClearTableAndDisplayDeal = true;

			-- If the AI rejects a deal, don't clear the table: keep the items where they are in case the human wants to change things
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER) then
				--print("DiploUIState: AI rejects offer");
				bClearTableAndDisplayDeal = false;
			else
				--print("DiploUIState: ?????");
			end

			-- Clear table and display the deal currently stored in InterfaceBuddy
			if (bClearTableAndDisplayDeal) then
				g_bMessageFromDiploAI = true;

				Controls.DiscussionText:SetText( szLeaderMessage );

				DoClearTable();
				DisplayDeal();

				if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
					-- Hide unavailable items on their side
					Controls.ThemPocketGold:SetHide(not g_pUs:IsDoF(g_iThem));
					Controls.ThemPocketTradeMap:SetHide(not g_pUs:IsDoF(g_iThem));
					Controls.ThemPocketTechnology:SetHide(not g_pUs:IsDoF(g_iThem));
					Controls.ThemPocketRevokeVassalage:SetHide(true);
					Controls.ThemPocketDefensivePact:SetHide(true);
					Controls.ThemPocketResearchAgreement:SetHide(true);
					Controls.ThemPocketCities:SetHide(true);
					Controls.ThemPocketOtherPlayerWar:SetHide(true);
				end

			-- Don't clear the table, leave things as they are
			else

				--print("NOT clearing table");

				g_bMessageFromDiploAI = true;

				Controls.DiscussionText:SetText( szLeaderMessage );
			end

			-- Resize the height of the box to fit the text
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
			--print("iDiploUIState: " .. iDiploUIState);

			g_Deal:ClearItems();

			if (not ContextPtr:IsHidden()) then
				ContextPtr:SetHide( true );
			end

		end
	end
end
--Events.AILeaderMessage.Add( LeaderMessageHandler );


----------------------------------------------------------------
-- Used by SimpleDiploTrade to display pvp deals
----------------------------------------------------------------
function OnOpenPlayerDealScreen( iOtherPlayer )

	--print( "OpenPlayerDealScreen: " .. iOtherPlayer );

	-- open a new deal with iThem
	g_iUs = Game.GetActivePlayer();
	g_pUs = Players[ g_iUs ];
	g_iUsTeam = g_pUs:GetTeam();
	g_pUsTeam = Teams[ g_iUsTeam ];

	g_iThem = iOtherPlayer;
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
		(iProposalTo ~= -1 and iProposalTo ~= iOtherPlayer and not UI.ProposedDealExists(iOtherPlayer, g_iUs)) ) then -- Only allow one proposal from us at a time.
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
		DoUpdateButtons();

	elseif( UI.ProposedDealExists( g_iThem, g_iUs ) ) then -- this is their proposal
		g_bNewDeal = false;
		UI.LoadProposedDeal( g_iThem, g_iUs );
		DoUpdateButtons();

	else
		g_bNewDeal = true;
		g_Deal:ClearItems();
		g_Deal:SetFromPlayer( g_iUs );
		g_Deal:SetToPlayer( g_iThem );

		if( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
			g_Deal:AddPeaceTreaty( g_iUs, g_PeaceDealDuration );
			g_Deal:AddPeaceTreaty( g_iThem, g_PeaceDealDuration );
		end

		DoUpdateButtons();
	end

	g_bPVPTrade = true;

	ContextPtr:SetHide( false );

	DoClearTable();
	DisplayDeal();
end


----------------------------------------------------------------
-- used by DiploOverview to display active deals
----------------------------------------------------------------
function OpenDealReview(OverridePlayer)

	--print( "OpenDealReview" );

	g_iUs = OverridePlayer or Game:GetActivePlayer();
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
	DisplayDeal(OverridePlayer);
end


----------------------------------------------------------------
-- BACK
----------------------------------------------------------------
function OnBack( iType )

	if( g_bPVPTrade or g_bTradeReview ) then

		if( iType == CANCEL_TYPE ) then
			g_Deal:ClearItems();
		elseif( iType == REFUSE_TYPE ) then
			UI.DoFinalizePlayerDeal( g_iThem, g_iUs, false );
		end

		ContextPtr:SetHide( true );
		ContextPtr:CallParentShowHideHandler( false );
	else
		g_Deal:ClearItems();

		-- Human refused to give into an AI demand - this button doubles as the Demand "Refuse" option
		if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND or
			g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST or
			g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER) then

			--print("Human refused demand!");

			DoDemandState(false);

			DoClearTable();
			ResetDisplay();

			if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL, g_iThem, 0, 0 );
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
				local iDealValue = 0;
				if gk_mode and g_Deal then
					iDealValue = g_pUs:GetDealMyValue(g_Deal);
				end
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL, g_iThem, iDealValue, 0 );
				-- Putmalk: What happened when human refused the request?
			elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER) then
				print("refused: generous offer");
				local iDealValue = 0;
				if (g_Deal) then
					iDealValue = g_pThem:GetDealMyValue(g_Deal);
				end
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL, g_iThem, iDealValue, 0 );
			end

			g_iDiploUIState = DiploUIStateTypes.DIPLO_UI_STATE_TRADE;

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
			if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
				Controls.UsPanel:SetHide(false);
				Controls.UsGlass:SetHide(false);
			end


			UIManager:DequeuePopup( ContextPtr );
			UI.RequestLeaveLeader();
		end
	end


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
    	    
    		if( g_iUs   ~= -1 ) then g_Deal:SetFromPlayer( g_iUs   ); end
    		if( g_iThem ~= -1 ) then g_Deal:SetToPlayer( g_iThem ); end
    		
            -- reset all the controls on show
			Controls.UsPocketCitiesStack:SetHide(true);
			Controls.ThemPocketCitiesStack:SetHide(true);
			Controls.UsPocketOtherPlayerStack:SetHide(true);
			Controls.ThemPocketOtherPlayerStack:SetHide(true);
			Controls.UsPocketStrategicStack:SetHide(true);
			Controls.ThemPocketStrategicStack:SetHide(true);
			Controls.UsPocketLuxuryStack:SetHide(true);
			Controls.ThemPocketLuxuryStack:SetHide(true);
			Controls.UsPocketTechnologyStack:SetHide(true);
			Controls.ThemPocketTechnologyStack:SetHide(true);
			Controls.UsPocketVoteStack:SetHide(true);
			Controls.ThemPocketVoteStack:SetHide(true);
            
			Controls.UsPocketPanel:SetScrollValue(0);
			Controls.UsTablePanel:SetScrollValue(0);
			Controls.ThemPocketPanel:SetScrollValue(0);
			Controls.ThemTablePanel:SetScrollValue(0);

            -- hide unmet players
            if( g_bEnableThirdParty ) then
                g_iNumOthers = 0;
				g_iNumOtherNonVassals = 0;
            	for iLoopPlayer = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
            		pLoopPlayer = Players[iLoopPlayer];
            		iLoopTeam = pLoopPlayer:GetTeam();
					pLoopTeam = Teams[iLoopTeam];
					
					if (iLoopTeam ~= g_iUsTeam and pLoopPlayer:IsAlive() and pLoopPlayer:IsMajorCiv() and (not pLoopTeam:IsVassalOfSomeone())) then
						g_iNumOtherNonVassals = g_iNumOtherNonVassals + 1;
					end
					
					if (pLoopPlayer:IsEverAlive() and g_OtherPlayersButtons[iLoopPlayer] ~= nil) then --This is a bandaid fix TODO: actually find the issue but oh well
        				if (g_iUs ~= iLoopPlayer and g_iThem ~= iLoopPlayer and
                			g_pUsTeam:IsHasMet(iLoopTeam) and g_pThemTeam:IsHasMet(iLoopTeam) and
                			pLoopPlayer:IsAlive()) then

            				g_OtherPlayersButtons[ iLoopPlayer ].UsPocket.Button:SetHide( false );
            				g_OtherPlayersButtons[ iLoopPlayer ].ThemPocket.Button:SetHide( false );
            				g_iNumOthers = g_iNumOthers + 1;
        				else
            				g_OtherPlayersButtons[ iLoopPlayer ].UsPocket.Button:SetHide( true );
            				g_OtherPlayersButtons[ iLoopPlayer ].ThemPocket.Button:SetHide( true );
            			end
            		end
            	end
        	end
            
            ResetDisplay();

			-- Unhide some of the stacks by default if anything was found
			if g_bAnyLuxuryUs then
				Controls.UsPocketLuxuryStack:SetHide(false);
				Controls.UsPocketLuxury:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));
			end
			if g_bAnyStrategicUs then
				Controls.UsPocketStrategicStack:SetHide(false);
				Controls.UsPocketStrategic:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));
			end
			if g_bAnyTechUs then
				Controls.UsPocketTechnologyStack:SetHide(false);
				Controls.UsPocketTechnology:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));
			end
			if g_bAnyVoteUs then
				Controls.UsPocketVoteStack:SetHide(false);
				Controls.UsPocketVote:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
			end
			if g_bAnyLuxuryThem then
				Controls.ThemPocketLuxuryStack:SetHide(false);
				Controls.ThemPocketLuxury:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));
			end
			if g_bAnyStrategicThem then
				Controls.ThemPocketStrategicStack:SetHide(false);
				Controls.ThemPocketStrategic:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));
			end
			if g_bAnyTechThem then
				Controls.ThemPocketTechnologyStack:SetHide(false);
				Controls.ThemPocketTechnology:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));
			end
			if g_bAnyVoteThem then
				Controls.ThemPocketVoteStack:SetHide(false);
				Controls.ThemPocketVote:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
			end

            -- Deal can already have items in it if, say, we're at war.  In this case every time we open the trade screen there's already Peace Treaty on both sides of the table
            if (g_Deal:GetNumItems() > 0) then
    			DisplayDeal();
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
	if( uiMsg == KeyEvents.KeyDown )
	then
		if( wParam == Keys.VK_ESCAPE ) then

			-- Don't allow any keys to exit screen when AI is asking for something - want to make sure the human has to click something
			if (g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND and
				g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST and
				g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER) then		-- Putmalk

				OnBack();
			end

			return true;
		end
	end
	return false;
end

---------------------------------------------------------
-- UI Deal was modified somehow
---------------------------------------------------------
function DoUIDealChangedByHuman()

	--print("UI Deal Changed");

	-- Set state to the default so that it doesn't cause any funny behavior later
	if (g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
		g_iDiploUIState = DiploUIStateTypes.DIPLO_UI_STATE_TRADE;
	end

	DoUpdateButtons();
	
end


---------------------------------------------------------
-- Update buttons at the bottom
---------------------------------------------------------
function DoUpdateButtons()

	-- Dealing with a human in a MP game
	if (g_bPVPTrade) then

		--print( "PVP Updating ProposeButton" );

		if( g_bNewDeal ) then
			Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_PROPOSE" ));
			Controls.ProposeButton:SetVoid1( PROPOSE_TYPE );
			Controls.CancelButton:SetHide( true );

			Controls.ModifyButton:SetHide( true );
			Controls.Pockets:SetHide( false );
			Controls.ModificationBlock:SetHide( true );

		elseif( UI.HasMadeProposal( g_iUs ) == g_iThem ) then
			Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_WITHDRAW" ));
			Controls.ProposeButton:SetVoid1( WITHDRAW_TYPE );
			Controls.CancelButton:SetHide( true );

			Controls.ModifyButton:SetHide( true );
			Controls.Pockets:SetHide( true );
			Controls.ModificationBlock:SetHide( false );

		else
			Controls.ProposeButton:SetVoid1( ACCEPT_TYPE );
			Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ACCEPT" ));
			Controls.CancelButton:SetVoid1( REFUSE_TYPE );
			Controls.CancelButton:SetHide( false );

			Controls.ModifyButton:SetHide( false );
			Controls.Pockets:SetHide( true );
			Controls.ModificationBlock:SetHide( false );
		end

		Controls.DenounceButton:SetHide(true);
		if (not g_pUs:IsDenouncedPlayer(g_iThem)) then
			Controls.DenounceButton:SetHide(false);
			Controls.DenounceButton:SetText(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN"));
			Controls.DenounceButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN_TT"));
		end

        Controls.MainStack:CalculateSize();
        Controls.MainGrid:DoAutoSize();

	-- Dealing with an AI
	elseif (g_bPVPTrade == false and g_bTradeReview == false) then

		-- Human is making a demand
		if( g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND ) then
			if (g_pUs:IsDoF(g_iThem)) then -- Request for help (friends)
				Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_REQUEST_HELP_DEAL_BUTTON" ));
			else
				Controls.ProposeButton:SetText( Locale.ToUpper(Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEMAND_BUTTON" )));
			end
            Controls.CancelButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_CANCEL" ));

		-- If the AI made an offer change what buttons are visible for the human to respond with
		elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
			Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ACCEPT" ));
			Controls.CancelButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_REFUSE" ));

		-- AI is making a demand or Request
		elseif (	g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND or
					g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST or
					g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER) then		-- Putmalk

			Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ACCEPT" ));
			Controls.CancelButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_REFUSE" ));

		else
			Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_PROPOSE" ));
			Controls.CancelButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_CANCEL" ));
		end

		Controls.WhatDoYouWantButton:SetHide(true);
		Controls.WhatWillYouGiveMeButton:SetHide(true);
		Controls.WhatWillMakeThisWorkButton:SetHide(true);
		Controls.WhatWillEndThisWarButton:SetHide(true);
 		Controls.WhatConcessionsButton:SetHide(true);
		Controls.DenounceButton:SetHide(true);

		-- Deal Value To Them
		Controls.PeaceValue:SetHide(false);
		Controls.PeaceDeal:SetHide(false);
		Controls.PeaceDealBorderFrame:SetHide(false);

		if (g_pUsTeam:IsAtWar(g_iThemTeam)) then
			if (g_Deal:GetSurrenderingPlayer() == g_iThem) then
				local iMaxValue = g_pThem:GetCachedValueOfPeaceWithHuman();
				local iCurrentValue = g_pThem:GetTotalValueToMe(g_Deal);
				local Valuestr;

				if (iCurrentValue == -999999) then
					Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_DEAL_VALUE_STR_IMPOSSIBLE");
				else
					Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_DEAL_VALUE_STR", iCurrentValue);
				end

				Controls.PeaceValue:SetText(Valuestr);
				Controls.PeaceValue:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_DEAL_VALUE_STR_TT"));
				Controls.PeaceMax:SetHide(false);
				Controls.PeaceMax:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_MAX_STR", iMaxValue));
				Controls.PeaceMax:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_MAX_STR_TT"));
			else
				local iMinValue = g_pThem:GetCachedValueOfPeaceWithHuman();
				local iCurrentValue = g_pThem:GetTotalValueToMe(g_Deal);
				local Valuestr;

				if (iCurrentValue == -999999) then
					Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_DEAL_VALUE_STR_IMPOSSIBLE");
				else
					Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_DEAL_VALUE_STR", iCurrentValue);
				end

				Controls.PeaceValue:SetText(Valuestr);
				Controls.PeaceValue:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_DEAL_VALUE_STR_TT"));
				Controls.PeaceMax:SetHide(false);
				Controls.PeaceMax:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_MIN_STR", iMinValue));
				Controls.PeaceMax:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_MIN_STR_TT"));
			end
		else
			local iCurrentValue = g_pThem:GetTotalValueToMeNormal(g_Deal);
			local Valuestr;
			local ValuestrTT;

			if (g_pThem:IsTradeSanctioned(g_iUs)) then
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO");
				ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT");
			elseif (iCurrentValue == -999999) then
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_IMPOSSIBLE");
				ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_TT");
			elseif (iCurrentValue == 0) then
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_ACCEPTABLE");
				ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_TT");
			else
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR", iCurrentValue);
				ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_TT");
			end

			Controls.PeaceValue:SetText(Valuestr);
			Controls.PeaceValue:SetToolTipString(ValuestrTT);
			Controls.PeaceMax:SetHide(true);
		end
		--END

		-- At War: show the "what will end this war button
		if (g_pUsTeam:IsAtWar(g_iThemTeam)) then
			Controls.WhatWillEndThisWarButton:SetHide(false);
		-- Not at war
		else
			-- AI asking player for concessions
			if (UI.IsAIRequestingConcessions()) then
				Controls.WhatConcessionsButton:SetHide(false);

			-- Is the someone making a demand?
			elseif (	g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND and
						g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST and
						g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND and
						g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER and
						g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_GENEROUS_OFFER) then		-- Putmalk

				-- Loop through deal items to see what the situation is
				local iNumItemsFromUs = 0;
				local iNumItemsFromThem = 0;

				g_Deal:ResetIterator();
				repeat
					local deal = { g_Deal:GetNextItem() }
					local n = #deal
					if n < 1 then
						break
					elseif deal[n] == g_iUs then
						iNumItemsFromUs = iNumItemsFromUs + 1;
					else
						iNumItemsFromThem = iNumItemsFromThem + 1;
					end
				until false

				-- Depending on what's on the table we can ask the other player about what they think of the deal
				if (iNumItemsFromUs > 0 and iNumItemsFromThem == 0) then
					Controls.WhatWillYouGiveMeButton:SetHide(false);
				elseif (iNumItemsFromUs == 0 and iNumItemsFromThem > 0) then
					Controls.WhatDoYouWantButton:SetHide(false);
				elseif (iNumItemsFromUs > 0 and iNumItemsFromThem > 0) then
					Controls.WhatWillMakeThisWorkButton:SetHide(false);
				end
			end
		end

		-- If they're making a demand and there's nothing on our side of the table then we can't propose anything
		if (UI.IsAIRequestingConcessions() and iNumItemsFromUs == 0) then
	 		Controls.ProposeButton:SetHide(true);
		else
 			Controls.ProposeButton:SetHide(false);
		end
	end
end


---------------------------------------------------------
-- AI is making a demand of the human
---------------------------------------------------------
function DoDemandState(bDemandOn)

	-- AI is demanding something, hide options that are not useful
	if (bDemandOn) then
		Controls.UsPanel:SetHide(true);
		Controls.UsGlass:SetHide(true);
		Controls.ThemPanel:SetHide(true);
		Controls.ThemGlass:SetHide(true);
		Controls.UsTableCover:SetHide(false);
		Controls.ThemTableCover:SetHide(false);

	-- Exiting demand mode, unhide stuff
	else
		Controls.UsPanel:SetHide(false);
		Controls.UsGlass:SetHide(false);
		Controls.ThemPanel:SetHide(false);
		Controls.ThemGlass:SetHide(false);
		Controls.UsTableCover:SetHide(true);
		Controls.ThemTableCover:SetHide(true);

		UIManager:DequeuePopup( ContextPtr );

	end

end


---------------------------------------------------------
-- Clear all items off the table (both UI and CvDeal)
---------------------------------------------------------
function DoClearDeal()
    g_Deal:ClearItems();
    DoClearTable();
end
Events.ClearDiplomacyTradeTable.Add(DoClearDeal);



---------------------------------------------------------
-- stack open/close handlers
---------------------------------------------------------
local g_SubStacks = {};
g_SubStacks[ tostring( Controls.UsPocketOtherPlayer       ) ] = Controls.UsPocketOtherPlayerStack;
g_SubStacks[ tostring( Controls.ThemPocketOtherPlayer     ) ] = Controls.ThemPocketOtherPlayerStack;
g_SubStacks[ tostring( Controls.UsPocketStrategic   ) ] = Controls.UsPocketStrategicStack;
g_SubStacks[ tostring( Controls.ThemPocketStrategic ) ] = Controls.ThemPocketStrategicStack;
g_SubStacks[ tostring( Controls.UsPocketLuxury      ) ] = Controls.UsPocketLuxuryStack;
g_SubStacks[ tostring( Controls.ThemPocketLuxury    ) ] = Controls.ThemPocketLuxuryStack;

local g_SubLabels = {};
g_SubLabels[ tostring( Controls.UsPocketOtherPlayer       ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_OTHER_PLAYERS" );
g_SubLabels[ tostring( Controls.ThemPocketOtherPlayer     ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_OTHER_PLAYERS" );
g_SubLabels[ tostring( Controls.UsPocketStrategic   ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_STRATEGIC_RESOURCES" );
g_SubLabels[ tostring( Controls.ThemPocketStrategic ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_STRATEGIC_RESOURCES" );
g_SubLabels[ tostring( Controls.UsPocketLuxury      ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES" );
g_SubLabels[ tostring( Controls.ThemPocketLuxury    ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES" );

Controls.UsPocketOtherPlayer:SetVoid1( 1 );
Controls.UsPocketStrategic:SetVoid1( 1 );
Controls.UsPocketLuxury:SetVoid1( 1 );

if (not g_bDisableLeague) then
	g_SubStacks[ tostring( Controls.UsPocketVote		) ] = Controls.UsPocketVoteStack;
	g_SubStacks[ tostring( Controls.ThemPocketVote		) ] = Controls.ThemPocketVoteStack;
	g_SubLabels[ tostring( Controls.UsPocketVote		) ] = Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" );
	g_SubLabels[ tostring( Controls.ThemPocketVote		) ] = Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" );

	Controls.UsPocketVote:SetVoid1( 1 );
end
-------------------------------------------------------
-- Putmalk
if ((not g_bDisableTechTrading) and (not g_bDisableScience)) then
	g_SubStacks[ tostring( Controls.UsPocketTechnology      ) ] = Controls.UsPocketTechnologyStack;
	g_SubStacks[ tostring( Controls.ThemPocketTechnology    ) ] = Controls.ThemPocketTechnologyStack;

	g_SubLabels[ tostring( Controls.UsPocketTechnology      ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES" );
	g_SubLabels[ tostring( Controls.ThemPocketTechnology    ) ] = Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES" );

	Controls.UsPocketTechnology:SetVoid1( 1 );
end
-------------------------------------------------------
function SubStackHandler( bIsUs, none, control )
	local stack = g_SubStacks[ tostring( control ) ];
	local label = g_SubLabels[ tostring( control ) ];

	if( stack:IsHidden() ) then
		stack:SetHide( false );
		control:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( label ) );
	else
		stack:SetHide( true );
		control:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( label ) );
	end

	if( bIsUs == 1 )
	then
		Controls.UsPocketStack:CalculateSize();
		Controls.UsPocketPanel:CalculateInternalSize();
		Controls.UsPocketPanel:ReprocessAnchoring();
	else
		Controls.ThemPocketStack:CalculateSize();
		Controls.ThemPocketPanel:CalculateInternalSize();
		Controls.ThemPocketPanel:ReprocessAnchoring();
	end
end
Controls.UsPocketOtherPlayer:RegisterCallback( Mouse.eLClick, SubStackHandler );
Controls.ThemPocketOtherPlayer:RegisterCallback( Mouse.eLClick, SubStackHandler );
Controls.UsPocketStrategic:RegisterCallback( Mouse.eLClick, SubStackHandler );
Controls.ThemPocketStrategic:RegisterCallback( Mouse.eLClick, SubStackHandler );
Controls.UsPocketLuxury:RegisterCallback( Mouse.eLClick, SubStackHandler );
Controls.ThemPocketLuxury:RegisterCallback( Mouse.eLClick, SubStackHandler );
---[[ Putmalk
if	((not g_bDisableTechTrading) and (not g_bDisableScience)) then
	Controls.UsPocketTechnology:RegisterCallback( Mouse.eLClick, SubStackHandler );
	Controls.ThemPocketTechnology:RegisterCallback( Mouse.eLClick, SubStackHandler );
end
--]]

if (not g_bDisableLeague) then
	function VoteStackHandler( bIsUs, none, control )
		RefreshPocketVotes(bIsUs);
		SubStackHandler( bIsUs, none, control );
	end
	Controls.ThemPocketVote:RegisterCallback( Mouse.eLClick, VoteStackHandler );
	Controls.UsPocketVote:RegisterCallback( Mouse.eLClick, VoteStackHandler );
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

	if (g_Deal:GetNumItems() == 0) then
		return;
	end

	--print( "OnPropose: " .. tostring( g_bPVPTrade ) .. " " .. iType );

	-- Trade between humans
	if( g_bPVPTrade ) then

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
		if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
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
		DoDemandState(false);
	end
end
Controls.ProposeButton:RegisterCallback( Mouse.eLClick, OnPropose );
-- CBP
function OnDenounceButton( )
	if( g_bPVPTrade ) then
		if (g_iUs ~= -1) then
			Players[g_iUs]:DoTradeScreenClosed(false);
		end
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DENOUNCE, g_iThem, 0, 0 );				
	end
end
Controls.DenounceButton:RegisterCallback( Mouse.eLClick, OnDenounceButton );
--END


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


----------------------------------------------------------------
----------------------------------------------------------------
function ResetDisplay()
	if g_iUs == -1 or g_iThem == -1 then
		return;
	end

	if (g_bEnableThirdParty) then
		g_iNumOthers = 0;
		g_iNumOtherNonVassals = 0;
		for iLoopPlayer = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
			pLoopPlayer = Players[iLoopPlayer];
			iLoopTeam = pLoopPlayer:GetTeam();
			pLoopTeam = Teams[iLoopTeam];
			
			if (iLoopTeam ~= g_iUsTeam and pLoopPlayer:IsAlive() and pLoopPlayer:IsMajorCiv() and (not pLoopTeam:IsVassalOfSomeone())) then
				g_iNumOtherNonVassals = g_iNumOtherNonVassals + 1;
			end

			if (g_iUs ~= iLoopPlayer and g_iThem ~= iLoopPlayer and
				g_pUsTeam:IsHasMet(iLoopTeam) and g_pThemTeam:IsHasMet(iLoopTeam) and
				pLoopPlayer:IsAlive()) then

				g_iNumOthers = g_iNumOthers + 1;
			end
		end
	end

	g_bAnyLuxuryUs = false;
	g_bAnyStrategicUs = false;
	g_bAnyTechUs = false;
	g_bAnyLuxuryThem = false;
	g_bAnyStrategicThem = false;
	g_bAnyTechThem = false;

	local bTeammates = g_iUsTeam == g_iThemTeam;
	local bShowEmbassy = not bTeammates;
	local bShowOpenBorders = not bTeammates;
	local bShowDefensivePact = (not bTeammates) and (not g_bAlwaysPeace) and (not g_bNoChangeWar);
	local bShowResearchAgreement = g_bAllowResearchAgreements and (not bTeammates) and (not g_bDisableScience);
	local bShowVotes = not g_bDisableLeague;
	local bShowVassalage = (not bTeammates) and (not g_bDisableVassalage);
	local bShowLiberation = (not bTeammates) and (not g_bDisableVassalage);
	local bShowMap = not bTeammates;
	local bShowTechs = (not g_bDisableTechTrading) and (not g_bDisableScience) and (not bTeammates);
	local bShowOtherPlayers = (not bTeammates) and (not g_bAlwaysPeace) and (not g_bNoChangeWar);

	Controls.UsPocketStack:SetHide(false);
	Controls.ThemPocketStack:SetHide(false);
	Controls.UsPocketLeaderStack:SetHide(true);
	Controls.ThemPocketLeaderStack:SetHide(true);
	Controls.UsPocketCitiesStack:SetHide(true);
	Controls.ThemPocketCitiesStack:SetHide(true);

	Controls.UsPocketGold:SetHide(false);
	Controls.UsPocketGoldPerTurn:SetHide(false);
	Controls.UsPocketAllowEmbassy:SetHide(not bShowEmbassy);
	Controls.UsPocketVote:SetHide(not bShowVotes);
	Controls.UsPocketVassalage:SetHide(not bShowVassalage);
	Controls.UsPocketRevokeVassalage:SetHide(not bShowLiberation);
	Controls.UsPocketTradeMap:SetHide(not bShowMap);
	Controls.UsPocketOpenBorders:SetHide(not bShowOpenBorders);
	Controls.UsPocketDefensivePact:SetHide(not bShowDefensivePact);
	Controls.UsPocketResearchAgreement:SetHide(not bShowResearchAgreement);
	Controls.UsPocketTechnology:SetHide(not bShowTechs);
	Controls.UsPocketOtherPlayer:SetHide(not bShowOtherPlayers);

	Controls.ThemPocketGold:SetHide(false);
	Controls.ThemPocketGoldPerTurn:SetHide(false);
	Controls.ThemPocketAllowEmbassy:SetHide(not bShowEmbassy);
	Controls.ThemPocketVote:SetHide(not bShowVotes);
	Controls.ThemPocketVassalage:SetHide(not bShowVassalage);
	Controls.ThemPocketRevokeVassalage:SetHide(not bShowLiberation);
	Controls.ThemPocketTradeMap:SetHide(not bShowMap);
	Controls.ThemPocketOpenBorders:SetHide(not bShowOpenBorders);
	Controls.ThemPocketDefensivePact:SetHide(not bShowDefensivePact);
	Controls.ThemPocketResearchAgreement:SetHide(not bShowResearchAgreement);
	Controls.ThemPocketCities:SetHide(false);
	Controls.ThemPocketOtherPlayerWar:SetHide(false);
	Controls.ThemPocketTechnology:SetHide(not bShowTechs);
	Controls.ThemPocketOtherPlayer:SetHide(not bShowOtherPlayers);

    if (Controls.UsPocketDoF ~= nil) then
		Controls.UsPocketDoF:SetHide(false);
		Controls.UsPocketDoF:RegisterCallback(Mouse.eLClick, PocketDoFHandler);
		Controls.UsPocketDoF:SetVoid1(1);
	end
    if (Controls.ThemPocketDoF ~= nil) then
		Controls.ThemPocketDoF:SetHide(false);
		Controls.ThemPocketDoF:RegisterCallback(Mouse.eLClick, PocketDoFHandler);
		Controls.ThemPocketDoF:SetVoid1(0);
	end

	if bShowOtherPlayers and g_iNumOthers > 0 then
		Controls.UsPocketOtherPlayerStack:SetHide(false);
		Controls.ThemPocketOtherPlayerStack:SetHide(false);
	else
		Controls.UsPocketOtherPlayerStack:SetHide(true);
		Controls.ThemPocketOtherPlayerStack:SetHide(true);
	end

	-- Propose button could have had its text changed to "ACCEPT" if the AI made an offer to the human 
	DoUpdateButtons();

	if (not g_bMessageFromDiploAI) then
		DoClearTable();
	end

	-- Research Agreement cost
	local iRACost = Game.GetResearchAgreementCost(g_iUs, g_iThem);
	local strRAString = Locale.ConvertTextKey("TXT_KEY_DIPLO_RESCH_AGREEMENT_US", iRACost);
	Controls.UsTableResearchAgreement:SetText(strRAString);
	Controls.ThemTableResearchAgreement:SetText(strRAString);

    if (g_bTradeReview) then
        -- review mode
    	Controls.TradeDetails:SetHide( false );
        
    	CivIconHookup( g_iUs, 64, Controls.UsSymbolShadow, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true );
    	CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true );

    	Controls.UsText:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", Locale.ConvertTextKey( g_pUs:GetNameKey() ) ) );

        if (g_pThem:IsHuman()) then
        	Controls.ThemText:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", Locale.ConvertTextKey( g_pThem:GetNickName() ) ) );
        else
        	Controls.ThemText:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", Locale.ConvertTextKey( g_pThem:GetName() ) ) );
    	end
    
    elseif (not g_bPVPTrade) then
	    -- ai mode
    	Controls.WhatDoYouWantButton:SetHide(true);
    	Controls.WhatWillYouGiveMeButton:SetHide(true);
    	Controls.WhatWillMakeThisWorkButton:SetHide(true);

    	local strString = Locale.ConvertTextKey("TXT_KEY_DIPLO_LEADER_SAYS", g_pThem:GetName());
    	Controls.NameText:SetText(strString);
	
    	if (not g_bMessageFromDiploAI) then
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
    	if (Teams[g_iUsTeam]:IsAtWar(g_iThemTeam)) then
    		Controls.WhatWillEndThisWarButton:SetHide(false);
    	else
    		Controls.WhatWillEndThisWarButton:SetHide(true);
    	end
	
	elseif (not g_bTradeReview) then
	    -- PvP mode
    	CivIconHookup( g_iUs, 64, Controls.UsSymbolShadow, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true );
    	CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true );

		TruncateString(Controls.ThemName, Controls.ThemTablePanel:GetSizeX() - Controls.ThemTablePanel:GetOffsetX(), Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetNickName() ));
        Controls.ThemCiv:SetText( "(" .. Locale.ConvertTextKey( GameInfo.Civilizations[ g_pThem:GetCivilizationType() ].ShortDescription ) .. ")" );

		Controls.DenounceButton:SetHide(true);
		if (not g_pUs:IsDenouncedPlayer(g_iThem)) then
			Controls.DenounceButton:SetHide(false);
			Controls.DenounceButton:SetText(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN"));
			Controls.DenounceButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN_TT"));
		end
    end

	----------------------------------------------------------------------------------

    local strTooltip;
	local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now
	local bAtWar = g_pUsTeam:IsAtWar(g_iThemTeam);
	local bWeAreSurrendering = bAtWar and g_Deal:GetSurrenderingPlayer() ~= g_iThem and (not g_bPVPTrade);
	local bTheyAreSurrendering = bAtWar and g_Deal:GetSurrenderingPlayer() == g_iThem and (not g_bPVPTrade);
	local bSanctioned = g_pUs:IsTradeSanctioned(g_iThem) and (not bAtWar);

    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_GOLD
    ----------------------------------------------------------------------------------

	-- Our Side
    local iGold = g_Deal:GetGoldAvailable(g_iUs, iItemToBeChanged);
    Controls.UsPocketGold:SetText( iGold .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD") );

    local bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD, 1);	-- 1 here is 1 Gold, which is the minimum possible

    if (not bTradeAllowed) then
	    Controls.UsPocketGold:SetDisabled(true);
	    Controls.UsPocketGold:GetTextControl():SetColorByName("Gray_Black");

		if (bTheyAreSurrendering) then
			Controls.UsPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.UsPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		else
			strTooltip = g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD, 1);
			if (strTooltip ~= "") then
				Controls.UsPocketGold:SetToolTipString(strTooltip);
			else
				Controls.UsPocketGold:SetToolTipString(nil);
			end
		end
	else
		Controls.UsPocketGold:SetDisabled(false);
	    Controls.UsPocketGold:GetTextControl():SetColorByName("Beige_Black");
	    Controls.UsPocketGold:SetToolTipString(nil);
    end

	----------------------------------------------------------------------------------

	-- Their Side
    iGold = g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged);
    Controls.ThemPocketGold:SetText( iGold .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD") );
    
    bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD, 1);	-- 1 here is 1 Gold, which is the minimum possible
    
    if (not bTradeAllowed) then
	    Controls.ThemPocketGold:SetDisabled(true);
	    Controls.ThemPocketGold:GetTextControl():SetColorByName("Gray_Black");

		if (bWeAreSurrendering) then
			Controls.ThemPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.ThemPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		else
			strTooltip = g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD, 1);
			if (strTooltip ~= "") then
				Controls.ThemPocketGold:SetToolTipString(strTooltip);
			else
				Controls.ThemPocketGold:SetToolTipString(nil);
			end
		end
	else
	    Controls.ThemPocketGold:SetDisabled(false);
	    Controls.ThemPocketGold:GetTextControl():SetColorByName("Beige_Black");
	    Controls.ThemPocketGold:SetToolTipString(nil);	    
    end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_GOLD_PER_TURN
    ----------------------------------------------------------------------------------

	-- Our Side
	local iGoldPerTurn = g_pUs:CalculateGoldRate();
    Controls.UsPocketGoldPerTurn:SetText( iGoldPerTurn .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN") );

    bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible

    if (not bTradeAllowed) then
	    Controls.UsPocketGoldPerTurn:SetDisabled(true);
	    Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Gray_Black");

		if (bTheyAreSurrendering) then
			Controls.UsPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.UsPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		else
			strTooltip = g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);
			if (strTooltip ~= "") then
				Controls.UsPocketGoldPerTurn:SetToolTipString(strTooltip);
			else
				Controls.UsPocketGoldPerTurn:SetToolTipString(nil);
			end
		end
	else
	    Controls.UsPocketGoldPerTurn:SetDisabled(false);
		if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_GOLD_PER_TURN, g_iUs, false, g_iDealDuration, 1)) then
			Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Red_Black");
		else
			Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Beige_Black");
		end
		Controls.UsPocketGoldPerTurn:SetToolTipString(nil);
    end

	----------------------------------------------------------------------------------

    -- Their Side
	iGoldPerTurn = g_pThem:CalculateGoldRate();
    Controls.ThemPocketGoldPerTurn:SetText( iGoldPerTurn .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN") );

    bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible

    if (not bTradeAllowed) then
	    Controls.ThemPocketGoldPerTurn:SetDisabled(true);
	    Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Gray_Black");

		if (bWeAreSurrendering) then
			Controls.ThemPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.ThemPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		else
			strTooltip = g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);
			if (strTooltip ~= "") then
				Controls.ThemPocketGoldPerTurn:SetToolTipString(strTooltip);
			else
				Controls.ThemPocketGoldPerTurn:SetToolTipString(nil);
			end
		end
	else
	    Controls.ThemPocketGoldPerTurn:SetDisabled(false);
	    if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_GOLD_PER_TURN, g_iUs, true, g_iDealDuration, 1)) then
			Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Red_Black");
		else
			Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Beige_Black");
		end
		Controls.ThemPocketGoldPerTurn:SetToolTipString(nil);
    end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_ALLOW_EMBASSY
    ----------------------------------------------------------------------------------

	if bShowEmbassy then
		strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_TT");

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketAllowEmbassy:SetDisabled(true);
			Controls.UsPocketAllowEmbassy:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketAllowEmbassy:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketAllowEmbassy:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketAllowEmbassy:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration));
			end
		else
			Controls.UsPocketAllowEmbassy:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketAllowEmbassy:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketAllowEmbassy:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketAllowEmbassy:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketAllowEmbassy:SetDisabled(true);
			Controls.ThemPocketAllowEmbassy:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketAllowEmbassy:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketAllowEmbassy:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketAllowEmbassy:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration));
			end
		else
			Controls.ThemPocketAllowEmbassy:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketAllowEmbassy:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketAllowEmbassy:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketAllowEmbassy:SetToolTipString(strTooltip);
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_OPEN_BORDERS
    ----------------------------------------------------------------------------------

	if bShowOpenBorders then
		strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_OPEN_BORDERS_TT", g_iDealDuration);

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketOpenBorders:SetDisabled(true);
			Controls.UsPocketOpenBorders:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketOpenBorders:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketOpenBorders:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketOpenBorders:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration));
			end
		else
			Controls.UsPocketOpenBorders:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketOpenBorders:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketOpenBorders:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketOpenBorders:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketOpenBorders:SetDisabled(true);
			Controls.ThemPocketOpenBorders:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketOpenBorders:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketOpenBorders:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketOpenBorders:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration));
			end
		else
			Controls.ThemPocketOpenBorders:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketOpenBorders:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketOpenBorders:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketOpenBorders:SetToolTipString(strTooltip);
		end
	end

 
    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_DEFENSIVE_PACT
    ----------------------------------------------------------------------------------

	if bShowDefensivePact then
		local iLimit = g_pUs:CalculateDefensivePactLimit();

		if iLimit >= g_iNumOtherNonVassals then
			strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEF_PACT_NO_LIMIT_TT", g_iDealDuration);
		else
			strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEF_PACT_TT", g_iDealDuration, iLimit);
		end

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketDefensivePact:SetDisabled(true);
			Controls.UsPocketDefensivePact:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketDefensivePact:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketDefensivePact:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketDefensivePact:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration));
			end
		else
			Controls.UsPocketDefensivePact:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketDefensivePact:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketDefensivePact:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketDefensivePact:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketDefensivePact:SetDisabled(true);
			Controls.ThemPocketDefensivePact:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketDefensivePact:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketDefensivePact:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketDefensivePact:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration));
			end
		else
			Controls.ThemPocketDefensivePact:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketDefensivePact:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketDefensivePact:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketDefensivePact:SetToolTipString(strTooltip);
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_RESEARCH_AGREEMENT
    ----------------------------------------------------------------------------------

	if bShowResearchAgreement then
		strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_RESCH_AGREEMENT_TT", iRACost, g_iDealDuration);

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketResearchAgreement:SetDisabled(true);
			Controls.UsPocketResearchAgreement:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketResearchAgreement:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketResearchAgreement:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketResearchAgreement:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration));
			end
		else
			Controls.UsPocketResearchAgreement:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketResearchAgreement:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketResearchAgreement:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketResearchAgreement:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketResearchAgreement:SetDisabled(true);
			Controls.ThemPocketResearchAgreement:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketResearchAgreement:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketResearchAgreement:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketResearchAgreement:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration));
			end
		else
			Controls.ThemPocketResearchAgreement:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketResearchAgreement:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketResearchAgreement:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketResearchAgreement:SetToolTipString(strTooltip);
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_DECLARATION_OF_FRIENDSHIP (Human to Human only)
    ----------------------------------------------------------------------------------

	if (Controls.UsPocketDoF ~= nil and Controls.ThemPocketDoF ~= nil) then
		if (g_bPVPTrade) then	-- Only PvP trade, with the AI there is a dedicated interface for this trade.
		
			strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP_TT");
			bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iDealDuration);

			if (not bTradeAllowed) then
				Controls.UsPocketDoF:SetDisabled(true);
				Controls.UsPocketDoF:GetTextControl():SetColorByName("Gray_Black");
				Controls.ThemPocketDoF:SetDisabled(true);
				Controls.ThemPocketDoF:GetTextControl():SetColorByName("Gray_Black");

				if (g_pUsTeam:IsAtWar(g_iThemTeam)) then
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DECLARATION_OF_FRIENDSHIP_AT_WAR") .. "[ENDCOLOR]";
				else
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DECLARATION_OF_FRIENDSHIP_ALREADY_EXISTS") .. "[ENDCOLOR]";
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


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_CITIES
    ----------------------------------------------------------------------------------

	local bFound = false;
	for pCity in g_pUs:Cities() do
		if (g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_CITIES, pCity:GetX(), pCity:GetY())) then
			bFound = true;
			break;
		end
	end

    if (bFound) then
		Controls.UsPocketCities:SetDisabled(false);
		Controls.UsPocketCities:GetTextControl():SetColorByName("Beige_Black");
		Controls.UsPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_CITY_TT"));
    else
		Controls.UsPocketCities:SetDisabled(true);
		Controls.UsPocketCities:GetTextControl():SetColorByName("Gray_Black");

		if (bTheyAreSurrendering) then
			Controls.UsPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.UsPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_CITIES, g_iUs, g_iThem)) then
			Controls.UsPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
		else
			Controls.UsPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT"));
		end
    end

	bFound = false;
	for pCity in g_pThem:Cities() do
		if (g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_CITIES, pCity:GetX(), pCity:GetY())) then
			bFound = true;
			break;
		end
	end

    if (bFound) then
		Controls.ThemPocketCities:SetDisabled(false);
		Controls.ThemPocketCities:GetTextControl():SetColorByName("Beige_Black");
		Controls.ThemPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_CITY_TT"));
    else
		Controls.ThemPocketCities:SetDisabled(true);
		Controls.ThemPocketCities:GetTextControl():SetColorByName("Gray_Black");

		if (bWeAreSurrendering) then
			Controls.ThemPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.ThemPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_CITIES, g_iThem, g_iUs)) then
			Controls.UsPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
		else
			Controls.ThemPocketCities:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM"));
		end
    end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_THIRD_PARTY_WAR, TRADE_ITEM_THIRD_PARTY_PEACE
    ----------------------------------------------------------------------------------

	if bShowOtherPlayers then
		if (g_iNumOthers == 0) then
			Controls.UsPocketOtherPlayer:SetDisabled(true);
			Controls.UsPocketOtherPlayer:GetTextControl():SetColorByName("Gray_Black");
			Controls.UsPocketOtherPlayer:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_OTHER_PLAYERS"));

			if (bTheyAreSurrendering) then
				Controls.UsPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS"));
			end

			Controls.ThemPocketOtherPlayer:SetDisabled(true);
			Controls.ThemPocketOtherPlayer:GetTextControl():SetColorByName("Gray_Black");
			Controls.ThemPocketOtherPlayer:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_OTHER_PLAYERS"));

			if (bWeAreSurrendering) then
				Controls.ThemPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM"));
			end
		else
			Controls.UsPocketOtherPlayer:SetDisabled(false);
			Controls.UsPocketOtherPlayer:GetTextControl():SetColorByName("Beige_Black");
			Controls.UsPocketOtherPlayer:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_OTHER_PLAYERS"));
			Controls.UsPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_OTHER_PLAYERS_OPEN"));
			
			Controls.ThemPocketOtherPlayer:SetDisabled(false);
			Controls.ThemPocketOtherPlayer:GetTextControl():SetColorByName("Beige_Black");
			Controls.ThemPocketOtherPlayer:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_OTHER_PLAYERS"));
			Controls.ThemPocketOtherPlayer:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_OTHER_PLAYERS_OPEN"));
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_RESOURCES
    ----------------------------------------------------------------------------------

    local bFoundLux = false;
    local bFoundStrat = false;

    local iResourceCount;
    local pResource;
    local strString;

    -- Our Side
	-- Loop over all resources
	for resType, instance in pairs(g_UsPocketResources) do
		
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESOURCES, resType, 1);	-- 1 here is 1 quantity of the Resource, which is the minimum possible
		
		if (bTradeAllowed) then
			if (g_LuxuryList[resType] == true) then
				bFoundLux = true;
			else
				bFoundStrat = true;
			end

			instance.Button:SetHide(false);

			pResource = GameInfo.Resources[resType];
			iResourceCount = g_pUs:GetNumResourceAvailable(resType, false);
			strString = pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description) .. " (" .. iResourceCount .. ")";
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_RESOURCES, g_iUs, false, g_iDealDuration, resType, 1)) then
				strString = "[COLOR_NEGATIVE_TEXT]" .. strString .."[ENDCOLOR]";
			end
			-- VP Dutch UA. This use hardcoded stuff ok dont rework the UA or ur stinky add a method or something
			if (g_pUs:GetCivilizationType() == GameInfoTypes.CIVILIZATION_NETHERLANDS and (g_pUs:GetResourceExport(resType) > 0 or g_pUs:GetResourceImport(resType) > 0) and pResource.ResourceUsage == 2) then
				strString = "[ICON_CHECKBOX] " .. strString;
			end
			instance.Button:SetText(strString);
		else
			instance.Button:SetHide(true);
		end
	end

    if (bFoundLux) then
		g_bAnyLuxuryUs = true;
		Controls.UsPocketLuxury:SetDisabled(false);
		Controls.UsPocketLuxury:GetTextControl():SetColorByName("Beige_Black");
		Controls.UsPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES"));
		if (Controls.UsPocketLuxuryStack:IsHidden()) then
    		Controls.UsPocketLuxury:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));
		else
    		Controls.UsPocketLuxury:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));
		end
	else
		Controls.UsPocketLuxury:SetDisabled(true);
		Controls.UsPocketLuxury:GetTextControl():SetColorByName("Gray_Black");
		Controls.UsPocketLuxury:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));

		if (bTheyAreSurrendering) then
			Controls.UsPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.UsPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_RESOURCES, g_iUs, g_iThem)) then
			Controls.UsPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
		else
			Controls.UsPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO"));
		end
    end

    if (bFoundStrat) then
		g_bAnyStrategicUs = true;
		Controls.UsPocketStrategic:SetDisabled(false);
		Controls.UsPocketStrategic:GetTextControl():SetColorByName("Beige_Black");
		Controls.UsPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES"));
		if (Controls.UsPocketStrategicStack:IsHidden()) then
    		Controls.UsPocketStrategic:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));
		else
    		Controls.UsPocketStrategic:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));
		end
	else
		Controls.UsPocketStrategic:SetDisabled(true);
		Controls.UsPocketStrategic:GetTextControl():SetColorByName("Gray_Black");
		Controls.UsPocketStrategic:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));

		if (bTheyAreSurrendering) then
			Controls.UsPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.UsPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_RESOURCES, g_iUs, g_iThem)) then
			Controls.UsPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
		else
			Controls.UsPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO"));
		end
    end

    bFoundLux = false;
    bFoundStrat = false;

	-- Their Side
	-- Loop over all resources
	for resType, instance in pairs(g_ThemPocketResources) do

		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESOURCES, resType, 1);	-- 1 here is 1 quantity of the Resource, which is the minimum possible
		
		if (bTradeAllowed) then
			if (g_LuxuryList[resType] == true) then
				bFoundLux = true;
			else
				bFoundStrat = true;
			end
			instance.Button:SetHide(false);
			
			pResource = GameInfo.Resources[resType];
			
			iResourceCount = g_pThem:GetNumResourceAvailable(resType, false);
			strString = pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description) .. " (" .. iResourceCount .. ")";
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_RESOURCES, g_iUs, true, g_iDealDuration, resType, 1)) then
				strString = "[COLOR_NEGATIVE_TEXT]" .. strString .."[ENDCOLOR]";
			end
			-- VP Dutch UA / City-State stuff. Checks if you already own one.
			if (g_pUs:IsShowImports() and (g_pUs:GetResourceExport(resType) > 0 or g_pUs:GetResourceImport(resType) > 0) and pResource.ResourceUsage == 2) then
				strString = "[ICON_CHECKBOX] " .. strString;
			end
			
			-- Add a happy face if WLTKD procs something here I guess
			local WLTKDTT = g_pUs:GetWLTKDResourceTT(resType);
			if (WLTKDTT ~= "") then
				strString = strString .. " [ICON_HAPPINESS_1]";
				instance.Button:SetToolTipString(WLTKDTT);
			end
			--Move it down here it'll be fun
			instance.Button:SetText(strString);
		else
			instance.Button:SetHide(true);
		end
	end

    if (bFoundLux) then
		g_bAnyLuxuryThem = true;
		Controls.ThemPocketLuxury:SetDisabled(false);
		Controls.ThemPocketLuxury:GetTextControl():SetColorByName("Beige_Black");
		Controls.ThemPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES_THEM"));
		if (Controls.ThemPocketLuxuryStack:IsHidden()) then
    		Controls.ThemPocketLuxury:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));
		else
    		Controls.ThemPocketLuxury:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));
		end
	else
		Controls.ThemPocketLuxury:SetDisabled(true);
		Controls.ThemPocketLuxury:GetTextControl():SetColorByName("Gray_Black");
		Controls.ThemPocketLuxury:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES"));

		if (bWeAreSurrendering) then
			Controls.ThemPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.ThemPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_RESOURCES, g_iThem, g_iUs)) then
			Controls.ThemPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
		else
			Controls.ThemPocketLuxury:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO_THEM"));
		end
    end

    if (bFoundStrat) then
		g_bAnyStrategicThem = true;
		Controls.ThemPocketStrategic:SetDisabled(false);
		Controls.ThemPocketStrategic:GetTextControl():SetColorByName("Beige_Black");
		Controls.ThemPocketStrategic:SetToolTipString(Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES_THEM"));
		if (Controls.ThemPocketStrategicStack:IsHidden()) then
    		Controls.ThemPocketStrategic:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));
		else
    		Controls.ThemPocketStrategic:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));
		end
	else
		Controls.ThemPocketStrategic:SetDisabled(true);
		Controls.ThemPocketStrategic:GetTextControl():SetColorByName("Gray_Black");
		Controls.ThemPocketStrategic:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES"));

		if (bWeAreSurrendering) then
			Controls.ThemPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif (bSanctioned) then
			Controls.ThemPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_RESOURCES, g_iThem, g_iUs)) then
			Controls.ThemPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
		else
			Controls.ThemPocketStrategic:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO_THEM"));
		end
    end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_VOTE_COMMITMENT
    ----------------------------------------------------------------------------------

    if bShowVotes then
		-- Our Side
		RefreshPocketVotes(1);

		if (bTheyAreSurrendering) then
			Controls.UsPocketVote:SetDisabled(true);
			Controls.UsPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.UsPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			Controls.UsPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
		elseif (bSanctioned) then
			Controls.UsPocketVote:SetDisabled(true);
			Controls.UsPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.UsPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			Controls.UsPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, g_iUs, g_iThem)) then
			Controls.UsPocketVote:SetDisabled(true);
			Controls.UsPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.UsPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
			Controls.UsPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
		else
			bTradeAllowed = g_pUs:CanCommitVote(g_pThem:GetID());
			local sTooltipDetails = g_pUs:GetCommitVoteDetails(g_pThem:GetID());
			if (bTradeAllowed) then
				Controls.UsPocketVote:SetDisabled(false);
				Controls.UsPocketVote:GetTextControl():SetColorByName("Beige_Black");
				Controls.UsPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_VOTE_TRADE_YES") .. sTooltipDetails);
				if (Controls.UsPocketVoteStack:IsHidden()) then
					Controls.UsPocketVote:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
				else
					Controls.UsPocketVote:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
				end
			else
				Controls.UsPocketVote:SetDisabled(true);
				Controls.UsPocketVote:GetTextControl():SetColorByName("Gray_Black");
				Controls.UsPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_VOTE_TRADE_NO") .. sTooltipDetails);
				Controls.UsPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
			end
		end

		-- Their Side
		RefreshPocketVotes(0);

		if (bWeAreSurrendering) then
			Controls.ThemPocketVote:SetDisabled(true);
			Controls.ThemPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.ThemPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			Controls.ThemPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
		elseif (bSanctioned) then
			Controls.ThemPocketVote:SetDisabled(true);
			Controls.ThemPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.ThemPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			Controls.ThemPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
		elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, g_iThem, g_iUs)) then
			Controls.ThemPocketVote:SetDisabled(true);
			Controls.ThemPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.ThemPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
			Controls.ThemPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
		else
			bTradeAllowed = g_pThem:CanCommitVote(g_pUs:GetID());
			local sTooltipDetails = g_pThem:GetCommitVoteDetails(g_pUs:GetID());
			if (bTradeAllowed) then
				Controls.ThemPocketVote:SetDisabled(false);
				Controls.ThemPocketVote:GetTextControl():SetColorByName("Beige_Black");
				Controls.ThemPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_VOTE_TRADE_YES_THEM") .. sTooltipDetails);
				if (Controls.ThemPocketVoteStack:IsHidden()) then
					Controls.ThemPocketVote:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
				else
					Controls.ThemPocketVote:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
				end
			else
				Controls.ThemPocketVote:SetDisabled(true);
				Controls.ThemPocketVote:GetTextControl():SetColorByName("Gray_Black");
				Controls.ThemPocketVote:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_VOTE_TRADE_NO_THEM") .. sTooltipDetails);
				Controls.ThemPocketVote:SetText(Locale.ConvertTextKey("TXT_KEY_TRADE_ITEM_VOTES"));
			end
		end
    end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_MAPS
    ----------------------------------------------------------------------------------

	if bShowMap then
		strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_MAP_TT", g_iDealDuration);

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_MAPS, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketTradeMap:SetDisabled(true);
			Controls.UsPocketTradeMap:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketTradeMap:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketTradeMap:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketTradeMap:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_MAPS, g_iDealDuration));
			end
		else
			Controls.UsPocketTradeMap:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_MAPS, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketTradeMap:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketTradeMap:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketTradeMap:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_MAPS, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketTradeMap:SetDisabled(true);
			Controls.ThemPocketTradeMap:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketTradeMap:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketTradeMap:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketTradeMap:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_MAPS, g_iDealDuration));
			end
		else
			Controls.ThemPocketTradeMap:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_MAPS, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketTradeMap:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketTradeMap:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketTradeMap:SetToolTipString(strTooltip);
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_VASSALAGE
    ----------------------------------------------------------------------------------

	if bShowVassalage then
		if bAtWar then
			Controls.UsPocketVassalage:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE"));
			Controls.ThemPocketVassalage:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE"));
			strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE_TT", Game.GetMinimumVassalTurns());
		else
			Controls.UsPocketVassalage:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE_PEACE"));
			Controls.ThemPocketVassalage:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE_PEACE"));
			strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE_PEACE_TT", Game.GetMinimumVoluntaryVassalTurns());
		end

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VASSALAGE, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketVassalage:SetDisabled(true);
			Controls.UsPocketVassalage:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketVassalage:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VASSALAGE, g_iDealDuration));
			end
		else
			Controls.UsPocketVassalage:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_VASSALAGE, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketVassalage:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketVassalage:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketVassalage:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VASSALAGE, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketVassalage:SetDisabled(true);
			Controls.ThemPocketVassalage:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketVassalage:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VASSALAGE, g_iDealDuration));
			end
		else
			Controls.ThemPocketVassalage:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_VASSALAGE, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketVassalage:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketVassalage:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketVassalage:SetToolTipString(strTooltip);
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_VASSALAGE_REVOKE
    ----------------------------------------------------------------------------------

	if bShowLiberation then
		strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_VASSALAGE_REVOKE_TT");

		-- Our Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.UsPocketRevokeVassalage:SetDisabled(true);
			Controls.UsPocketRevokeVassalage:GetTextControl():SetColorByName("Gray_Black");

			if (bTheyAreSurrendering) then
				Controls.UsPocketRevokeVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketRevokeVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.UsPocketRevokeVassalage:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iDealDuration));
			end
		else
			Controls.UsPocketRevokeVassalage:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iUs, false, g_iDealDuration)) then
				Controls.UsPocketRevokeVassalage:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.UsPocketRevokeVassalage:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.UsPocketRevokeVassalage:SetToolTipString(strTooltip);
		end

		----------------------------------------------------------------------------------

		-- Their Side
		bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iDealDuration);

		if (not bTradeAllowed) then
			Controls.ThemPocketRevokeVassalage:SetDisabled(true);
			Controls.ThemPocketRevokeVassalage:GetTextControl():SetColorByName("Gray_Black");

			if (bWeAreSurrendering) then
				Controls.ThemPocketRevokeVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketRevokeVassalage:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			else
				Controls.ThemPocketRevokeVassalage:SetToolTipString(strTooltip .. g_Deal:GetReasonsItemUntradeable(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iDealDuration));
			end
		else
			Controls.ThemPocketRevokeVassalage:SetDisabled(false);
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iUs, true, g_iDealDuration)) then
				Controls.ThemPocketRevokeVassalage:GetTextControl():SetColorByName("Red_Black");
			else
				Controls.ThemPocketRevokeVassalage:GetTextControl():SetColorByName("Beige_Black");
			end
			Controls.ThemPocketRevokeVassalage:SetToolTipString(strTooltip);
		end
	end


    ----------------------------------------------------------------------------------
    -- TRADE_ITEM_TECHS
    ----------------------------------------------------------------------------------

	if bShowTechs then
		local bFoundTech = false;
		local pTech;
		
		-- Our Side
		-- Loop through all technologies
		for techType, instance in pairs(g_UsPocketTechs) do
			bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_TECHS, techType);

			if (bTradeAllowed) then
				bFoundTech = true;
				instance.Button:SetHide(false);
				pTech = GameInfo.Technologies[techType];
				strString =  Locale.ConvertTextKey(pTech.Description);
				if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_TECHS, g_iUs, false, g_iDealDuration, techType)) then
					strString = "[COLOR_NEGATIVE_TEXT]" .. strString .."[ENDCOLOR]";
				end
				instance.Button:SetText(strString);
				instance.Button:SetToolTipString(GetShortHelpTextForTech(pTech.ID, false));
			else
				instance.Button:SetHide(true);
			end
		end
		
		-- Found an eligible tech? Cool!
		if bFoundTech then
			g_bAnyTechUs = true;
			Controls.UsPocketTechnology:SetDisabled(false);
			Controls.UsPocketTechnology:GetTextControl():SetColorByName("Beige_Black");
			Controls.UsPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_TECHNOLOGIES_TRADE_YES"));
			if (Controls.UsPocketTechnologyStack:IsHidden()) then
				Controls.UsPocketTechnology:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));
			else
				Controls.UsPocketTechnology:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));
			end
		-- Can't find an eligible tech? Find out why and relay info to player
		else
			Controls.UsPocketTechnology:SetDisabled(true);
			Controls.UsPocketTechnology:GetTextControl():SetColorByName("Gray_Black");
			Controls.UsPocketTechnology:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));

			if (bTheyAreSurrendering) then
				Controls.UsPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.UsPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_TECHS, g_iUs, g_iThem)) then
				Controls.UsPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
			else
				strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_TECHNOLOGIES_TRADE_NO");

				-- No tech?
				if (not g_pUsTeam:IsTechTrading()) then
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_TECHNOLOGIES_NO_TECH_PLAYER") .. "[ENDCOLOR]";
				end
			
				-- No embassy?
				if ((not bAtWar) and ((not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) or (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)))) then
					if ((not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) and (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam))) then
						strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT") .. "[ENDCOLOR]";
					elseif (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) then
						strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_YOU_NEED_EMBASSY_TT") .. "[ENDCOLOR]";
					elseif (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
						strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_THEY_NEED_EMBASSY_TT") .. "[ENDCOLOR]";
					end
				end

				Controls.UsPocketTechnology:SetToolTipString(strTooltip);
			end
		end

		bFoundTech = false;

		-- Their Side
		-- Loop through all technologies
		for techType, instance in pairs(g_ThemPocketTechs) do
			bTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_TECHS, techType);

			if (bTradeAllowed) then
				bFoundTech = true;
				instance.Button:SetHide(false);
				pTech = GameInfo.Technologies[techType];
				strString =  Locale.ConvertTextKey(pTech.Description);
				if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_TECHS, g_iUs, true, g_iDealDuration, techType)) then
					strString = "[COLOR_NEGATIVE_TEXT]" .. strString .."[ENDCOLOR]";
				end
				instance.Button:SetText(strString);
				instance.Button:SetToolTipString(GetShortHelpTextForTech(pTech.ID, false));
			else
				instance.Button:SetHide(true);
			end
		end

		if (bFoundTech) then
			g_bAnyTechThem = true;
			Controls.ThemPocketTechnology:SetDisabled(false);
			Controls.ThemPocketTechnology:GetTextControl():SetColorByName("Beige_Black");
			Controls.ThemPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_TECHNOLOGIES_TRADE_YES_THEM"));
			if (Controls.ThemPocketTechnologyStack:IsHidden()) then
				Controls.ThemPocketTechnology:SetText("[ICON_PLUS]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));
			else
				Controls.ThemPocketTechnology:SetText("[ICON_MINUS]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));
			end
		else
			Controls.ThemPocketTechnology:SetDisabled(true);
			Controls.ThemPocketTechnology:GetTextControl():SetColorByName("Gray_Black");
			Controls.ThemPocketTechnology:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_TECHNOLOGIES"));

			if (bWeAreSurrendering) then
				Controls.ThemPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
			elseif (bSanctioned) then
				Controls.ThemPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT"));
			elseif (g_Deal:BlockTemporaryForPermanentTrade(TradeableItems.TRADE_ITEM_TECHS, g_iThem, g_iUs)) then
				Controls.ThemPocketTechnology:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT"));
			else
				strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_TO_TRADE_TECHNOLOGIES_TRADE_NO_THEM");

				if (g_pThem:IsAITeammateOfHuman()) then
					strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_AI_TEAMMATE_TRADE_TT") .. "[ENDCOLOR]";
				else
					-- No tech?
					if (not g_pThemTeam:IsTechTrading()) then
						strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_TECHNOLOGIES_NO_TECH_OTHER_PLAYER") .. "[ENDCOLOR]";
					end
					
					if ((not bAtWar) and ((not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) or (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)))) then
						if ((not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) and (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam))) then
							strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT") .. "[ENDCOLOR]";
						elseif (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
							strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_THEY_NEED_EMBASSY_TT") .. "[ENDCOLOR]";
						elseif (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) then
							strTooltip = strTooltip .. "[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_YOU_NEED_EMBASSY_TT") .. "[ENDCOLOR]";
						end
					end
				end

				Controls.ThemPocketTechnology:SetToolTipString(strTooltip);
			end
		end
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
	Controls.UsTableCitiesStack:SetHide( true );
	Controls.ThemTableCitiesStack:SetHide( true );
	Controls.UsTableStrategicStack:SetHide( true );
	Controls.ThemTableStrategicStack:SetHide( true );
	Controls.UsTableLuxuryStack:SetHide( true );
	Controls.ThemTableLuxuryStack:SetHide( true );
	Controls.UsTableVoteStack:SetHide( true );
	Controls.ThemTableVoteStack:SetHide( true );	
	Controls.UsTableMakePeaceStack:SetHide( true );
	Controls.ThemTableMakePeaceStack:SetHide( true );
	Controls.UsTableDeclareWarStack:SetHide( true );
	Controls.ThemTableDeclareWarStack:SetHide( true );
	---[[ Putmalk
	Controls.UsTableTradeMap:SetHide( true );
	Controls.ThemTableTradeMap:SetHide( true );
	Controls.UsTableRevokeVassalage:SetHide( true );
	Controls.ThemTableRevokeVassalage:SetHide( true );
	Controls.UsTableVassalage:SetHide( true );
	Controls.ThemTableVassalage:SetHide( true );
	Controls.UsTableTechnologyStack:SetHide( true );
	Controls.ThemTableTechnologyStack:SetHide( true );
	--]]
	for n, table in pairs( g_OtherPlayersButtons ) do
		table.UsTableWar.Button:SetHide( true );
		table.UsTablePeace.Button:SetHide( true );
		table.ThemTableWar.Button:SetHide( true );
		table.ThemTablePeace.Button:SetHide( true );
	end

	-- loop over resources
	for n, instance in pairs( g_UsTableResources ) do
		instance.Container:SetHide( true );
	end
	for n, instance in pairs( g_ThemTableResources ) do
		instance.Container:SetHide( true );
	end
	---[[ Putmalk
	for n, instance in pairs( g_UsTableTechs ) do
		instance.Container:SetHide( true );
	end
	for n, instance in pairs( g_ThemTableTechs ) do
		instance.Container:SetHide( true );
	end
	--]]
	if (Controls.UsTableDoF ~= nil) then
		Controls.UsTableDoF:SetHide(true);
		Controls.UsTableDoF:RegisterCallback( Mouse.eLClick, TableDoFHandler );
		Controls.UsTableDoF:SetVoid1( 1 );
	end
	if (Controls.ThemTableDoF ~= nil) then
		Controls.ThemTableDoF:SetHide(true);
		Controls.ThemTableDoF:RegisterCallback( Mouse.eLClick, TableDoFHandler );
		Controls.ThemTableDoF:SetVoid1( 0 );
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
	-- Putmalk
	Controls.UsPocketTechnologyStack:CalculateSize();
    Controls.ThemPocketTechnologyStack:CalculateSize();
	Controls.UsTableTechnologyStack:CalculateSize();
    Controls.ThemTableTechnologyStack:CalculateSize();
	
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
function DisplayDeal(OverridePlayer)
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

	local itemType;
	local duration;
	local finalTurn;
	local data1;
	local data2;
	local data3;
	local flag1;
	local fromPlayer;

	local strString;
	local strTooltip;

	ResetDisplay();

	local iNumItemsFromUs = 0;
	local iNumItemsFromThem = 0;

	g_Deal:ResetIterator();
	if bnw_mode then
		itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = g_Deal:GetNextItem();
	else
		itemType, duration, finalTurn, data1, data2, fromPlayer = g_Deal:GetNextItem();
	end

	local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now

	--print("Going through check");
	if( itemType ~= nil ) then
	repeat
		local bFromUs = false;

		--print("Adding trade item to active deal: " .. itemType);

		if( fromPlayer == (OverridePlayer or Game.GetActivePlayer()) ) then
			bFromUs = true;
			iNumItemsFromUs = iNumItemsFromUs + 1;
		else
			iNumItemsFromThem = iNumItemsFromThem + 1;
		end

		if( TradeableItems.TRADE_ITEM_PEACE_TREATY == itemType ) then
			local str = Locale.ConvertTextKey("TXT_KEY_DIPLO_PEACE_TREATY", g_iPeaceDuration);
			if( bFromUs ) then
				Controls.UsTablePeaceTreaty:SetText(str);
				Controls.UsTablePeaceTreaty:SetHide( false );
			else
				Controls.ThemTablePeaceTreaty:SetText(str);
				Controls.ThemTablePeaceTreaty:SetHide( false );
			end

		elseif( TradeableItems.TRADE_ITEM_GOLD == itemType ) then

			Controls.UsPocketGold:SetHide( true );
			Controls.ThemPocketGold:SetHide( true );

			Controls.UsTableGold:SetHide( not bFromUs );
			Controls.ThemTableGold:SetHide( bFromUs );

			-- update quantity
			if( bFromUs ) then
				Controls.UsGoldAmount:SetText( data1 );

				strString = Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD");
				Controls.UsTableGold:SetText( strString );
				strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iUs, iItemToBeChanged) );
				Controls.UsTableGold:SetToolTipString( strTooltip );
			else
				Controls.ThemGoldAmount:SetText( data1 );

				strString = Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD");
				Controls.ThemTableGold:SetText( strString );
				strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged) );
				Controls.ThemTableGold:SetToolTipString( strTooltip );
			end

		elseif( TradeableItems.TRADE_ITEM_GOLD_PER_TURN == itemType ) then

			Controls.UsPocketGoldPerTurn:SetHide( true );
			Controls.ThemPocketGoldPerTurn:SetHide( true );

			if( bFromUs ) then
				Controls.UsTableGoldPerTurn:SetHide( false );
				Controls.UsGoldPerTurnTurns:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration );
				Controls.UsGoldPerTurnAmount:SetText( data1 );

				strString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_GOLD_PER_TURN" );
				Controls.UsTableGoldPerTurnButton:SetText( strString );
				strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GPT", g_pUs:CalculateGoldRate() - data1 );
				Controls.UsTableGoldPerTurn:SetToolTipString( strTooltip );
			else
				Controls.ThemTableGoldPerTurn:SetHide( false );
				Controls.ThemGoldPerTurnTurns:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration );
				Controls.ThemGoldPerTurnAmount:SetText( data1 );

				strString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_GOLD_PER_TURN" );
				Controls.ThemTableGoldPerTurnButton:SetText( strString );
				strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GPT", g_pThem:CalculateGoldRate() - data1 );
				Controls.ThemTableGoldPerTurn:SetToolTipString( strTooltip );
			end

		elseif( TradeableItems.TRADE_ITEM_CITIES == itemType ) then

			local pCity = nil;
			local instance = nil;
			local pPlot = Map.GetPlot( data1, data2 );
			if( pPlot ~= nil ) then
				pCity = pPlot:GetPlotCity();
			end

			if( pCity ~= nil ) then

				if( bFromUs ) then

					instance = g_UsTableCitiesIM:GetInstance();
					instance.Button:SetVoids( g_iUs, pCity:GetID() );
					instance.Button:RegisterCallback( Mouse.eLClick, TableCityHandler );
					Controls.UsTableCitiesStack:SetHide( false );
				else
					instance = g_ThemTableCitiesIM:GetInstance();
					instance.Button:SetVoids( g_iThem, pCity:GetID() );
					instance.Button:RegisterCallback( Mouse.eLClick, TableCityHandler );
					Controls.ThemTableCitiesStack:SetHide( false );
				end

				instance.CityName:SetText( pCity:GetName() );
				instance.CityPop:SetText( pCity:GetPopulation() );
			else
				if( bFromUs ) then
					instance = g_UsTableCitiesIM:GetInstance();
					Controls.UsTableCitiesStack:SetHide( false );
				else
					instance = g_ThemTableCitiesIM:GetInstance();
					Controls.ThemTableCitiesStack:SetHide( false );
				end
				instance.CityName:LocalizeAndSetText( "TXT_KEY_RAZED_CITY" );
				instance.CityPop:SetText( "" );
			end

		elseif( TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE == itemType ) then
			DisplayOtherPlayerItem( bFromUs, itemType, duration, data1 );

		elseif( TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR == itemType ) then
			DisplayOtherPlayerItem( bFromUs, itemType, duration, data1 );

		elseif gk_mode and ( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY == itemType ) then
			if( bFromUs ) then
				Controls.UsPocketAllowEmbassy:SetHide( true );
				Controls.UsTableAllowEmbassy:SetHide( false );
			else
				Controls.ThemPocketAllowEmbassy:SetHide( true );
				Controls.ThemTableAllowEmbassy:SetHide( false );
			end

		elseif( TradeableItems.TRADE_ITEM_OPEN_BORDERS == itemType ) then

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
		-- Putmalk: Display the items on the table?
		elseif( TradeableItems.TRADE_ITEM_MAPS == itemType ) then
			
			if( bFromUs ) then
				Controls.UsPocketTradeMap:SetHide( true );
				Controls.UsTableTradeMap:SetHide( false );
			else
				Controls.ThemPocketTradeMap:SetHide( true );
				Controls.ThemTableTradeMap:SetHide( false );
			end
		elseif( TradeableItems.TRADE_ITEM_VASSALAGE == itemType ) then
		    Controls.UsPocketVassalage:SetHide( true );
            Controls.ThemPocketVassalage:SetHide( true );
			print("Found Vassalage");
			if( bFromUs ) then
			 	Controls.UsTableVassalage:SetHide( false );
			else
				Controls.ThemTableVassalage:SetHide( false );
			end
		elseif( TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE == itemType ) then
			Controls.UsPocketRevokeVassalage:SetHide( true );
			Controls.ThemPocketRevokeVassalage:SetHide( true );
			print("Found Revoke Vassalage");
			if( bFromUs ) then
			 	Controls.UsTableRevokeVassalage:SetHide( false );
			else
				Controls.ThemTableRevokeVassalage:SetHide( false );
			end
		elseif( TradeableItems.TRADE_ITEM_TECHS == itemType ) then
			if ( bFromUs ) then
				g_UsTableTechs[ data1 ].Container:SetHide( false );
				Controls.UsTableTechnologyStack:SetHide( false );
			else
				g_ThemTableTechs[ data1 ].Container:SetHide( false );
				Controls.ThemTableTechnologyStack:SetHide( false );
			end
			
			g_UsPocketTechs[ data1 ].Button:SetHide( true );
			g_ThemPocketTechs[ data1 ].Button:SetHide( true );
		end
		if bnw_mode then
			itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = g_Deal:GetNextItem();
		else
			itemType, duration, finalTurn, data1, data2, fromPlayer = g_Deal:GetNextItem();
		end
	until( itemType == nil )
	end

	DoUpdateButtons();

	ResizeStacks();
end


-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
function DisplayOtherPlayerItem( bFromUs, itemType, duration, iOtherTeam )

	--print("iOtherTeam: " .. iOtherTeam);

	local iOtherPlayer = -1;
	for i = 0, GameDefines.MAX_CIV_PLAYERS - 1  do
		local player = Players[i];
		local iTeam = player:GetTeam();
		--print("iTeam: " .. iTeam);
		if (player:GetTeam() == iOtherTeam and player:IsAlive()) then
			--print("iOtherPlayer: " .. i);
			iOtherPlayer = i;
			break;
		end
	end

	--print("iOtherPlayer: " .. iOtherPlayer);
	if(iOtherPlayer ~= -1) then
		if( TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE == itemType ) then
			--print( "Displaying Peace" );
			local otherPlayerButton = g_OtherPlayersButtons[iOtherPlayer];
			if(otherPlayerButton ~= nil) then
				if( bFromUs ) then
					--print( "    from us" );
					g_OtherPlayersButtons[ iOtherPlayer ].UsTablePeace.Button:SetHide( false );
					Controls.UsTableMakePeaceStack:SetHide( false );
					Controls.UsTableMakePeaceStack:CalculateSize();
					Controls.UsTableMakePeaceStack:ReprocessAnchoring();
				else
					--print( "    from them" );
					g_OtherPlayersButtons[ iOtherPlayer ].ThemTablePeace.Button:SetHide( false );
					Controls.ThemTableMakePeaceStack:SetHide( false );
					Controls.ThemTableMakePeaceStack:CalculateSize();
					Controls.ThemTableMakePeaceStack:ReprocessAnchoring();
				end
			end

		elseif( TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR == itemType ) then
			--print( "Displaying War" );
			local otherPlayerButton = g_OtherPlayersButtons[iOtherPlayer];
			if(otherPlayerButton ~= nil) then
				if( bFromUs ) then
					--print( "    from us" );
					g_OtherPlayersButtons[ iOtherPlayer ].UsTableWar.Button:SetHide( false );
					Controls.UsTableDeclareWarStack:SetHide( false );
					Controls.UsTableDeclareWarStack:CalculateSize();
					Controls.UsTableDeclareWarStack:ReprocessAnchoring();
				else
					--print( "    from them" );
					g_OtherPlayersButtons[ iOtherPlayer ].ThemTableWar.Button:SetHide( false );
					Controls.ThemTableDeclareWarStack:SetHide( false );
					Controls.ThemTableDeclareWarStack:CalculateSize();
					Controls.ThemTableDeclareWarStack:ReprocessAnchoring();
				end
			end
		end
	end
end



-----------------------------------------------------------------------------------------------------------------------
-- Gold Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketGoldHandler( isUs )

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
	DisplayDeal();
	DoUIDealChangedByHuman();
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
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
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
		if (iGold == 0) then
			iGold = 1;
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
		if (iGold == 0) then
			iGold = 1;
			Controls.ThemGoldAmount:SetText(iGold);
		end

		g_Deal:ChangeGoldTrade( g_iThem, iGold );

		local iItemToBeChanged = -1;
		local strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged) );
		Controls.ThemTableGold:SetToolTipString( strTooltip );
	end

	--CBP
	DoUIDealChangedByHuman();
	--END
end
Controls.UsGoldAmount:RegisterCallback( ChangeGoldAmount );
Controls.UsGoldAmount:SetVoid1( 1 );
Controls.ThemGoldAmount:RegisterCallback( ChangeGoldAmount );
Controls.ThemGoldAmount:SetVoid1( 0 );

-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------





-----------------------------------------------------------------------------------------------------------------------
-- Gold Per Turn Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketGoldPerTurnHandler( isUs )
	--print("PocketGoldPerTurnHandler")

	local iGoldPerTurn = 2;

	if( isUs == 1 ) then

		if (iGoldPerTurn > g_pUs:CalculateGoldRate()) then
			iGoldPerTurn = g_pUs:CalculateGoldRate();
		end

		g_Deal:AddGoldPerTurnTrade( g_iUs, iGoldPerTurn, g_iDealDuration );
	else

		if (iGoldPerTurn > g_pThem:CalculateGoldRate()) then
			iGoldPerTurn = g_pThem:CalculateGoldRate();
		end

		g_Deal:AddGoldPerTurnTrade( g_iThem, iGoldPerTurn, g_iDealDuration );
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketGoldPerTurn:RegisterCallback( Mouse.eLClick, PocketGoldPerTurnHandler );
Controls.ThemPocketGoldPerTurn:RegisterCallback( Mouse.eLClick, PocketGoldPerTurnHandler );
Controls.UsPocketGoldPerTurn:SetVoid1( 1 );
Controls.ThemPocketGoldPerTurn:SetVoid1( 0 );

function TableGoldPerTurnHandler( isUs )
	--print("TableGoldPerTurnHandler")
	if( isUs == 1 ) then
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD_PER_TURN, g_iUs );
	else
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD_PER_TURN, g_iThem );
	end
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableGoldPerTurnButton:RegisterCallback( Mouse.eLClick, TableGoldPerTurnHandler );
Controls.ThemTableGoldPerTurnButton:RegisterCallback( Mouse.eLClick, TableGoldPerTurnHandler );
Controls.UsTableGoldPerTurnButton:SetVoid1( 1 );
Controls.ThemTableGoldPerTurnButton:SetVoid1( 0 );

-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
function ChangeGoldPerTurnAmount( string, control )

	--print("ChangeGoldPerTurnAmount")

	local g_pUs = Players[ g_iUs ];
	local g_pThem = Players[ g_iThem ];

	local iGoldPerTurn = 0;
	if( string ~= nil and string ~= "" ) then
		iGoldPerTurn = tonumber(string);
	else
		control:SetText( 0 );
	end

	-- GPT from us
	if( control:GetVoid1() == 1 ) then

		if (iGoldPerTurn > g_pUs:CalculateGoldRate()) then
			iGoldPerTurn = g_pUs:CalculateGoldRate();
			Controls.UsGoldPerTurnAmount:SetText(iGoldPerTurn);
		end
		if (iGoldPerTurn == 0) then
			iGoldPerTurn = 1;
			Controls.UsGoldPerTurnAmount:SetText(iGoldPerTurn);
		end

		g_Deal:ChangeGoldPerTurnTrade( g_iUs, iGoldPerTurn, g_iDealDuration );

		local strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GPT", g_pUs:CalculateGoldRate() - iGoldPerTurn );
		Controls.UsTableGoldPerTurn:SetToolTipString( strTooltip );

	-- GPT from them
	else

		if (iGoldPerTurn > g_pThem:CalculateGoldRate()) then
			iGoldPerTurn = g_pThem:CalculateGoldRate();
			Controls.ThemGoldPerTurnAmount:SetText(iGoldPerTurn);
		end
		if (iGoldPerTurn == 0) then
			iGoldPerTurn = 1;
			Controls.ThemGoldPerTurnAmount:SetText(iGoldPerTurn);
		end

		g_Deal:ChangeGoldPerTurnTrade( g_iThem, iGoldPerTurn, g_iDealDuration );

		local strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GPT", g_pThem:CalculateGoldRate() - iGoldPerTurn );
		Controls.ThemTableGoldPerTurn:SetToolTipString( strTooltip );

	end

	--CBP
	DoUIDealChangedByHuman();
	--END
end
Controls.UsGoldPerTurnAmount:RegisterCallback( ChangeGoldPerTurnAmount );
Controls.UsGoldPerTurnAmount:SetVoid1( 1 );
Controls.ThemGoldPerTurnAmount:RegisterCallback( ChangeGoldPerTurnAmount );
Controls.ThemGoldPerTurnAmount:SetVoid1( 0 );

if gk_mode then
	-----------------------------------------------------------------------------------------------------------------------
	-- Allow Embassy Handlers
	-----------------------------------------------------------------------------------------------------------------------
	function PocketAllowEmbassyHandler( isUs )
		--print("PocketAllowEmbassyHandler");
		if( isUs == 1 ) then
			g_Deal:AddAllowEmbassy(g_iUs);
		else
			g_Deal:AddAllowEmbassy(g_iThem);
		end
		DisplayDeal();
		DoUIDealChangedByHuman();
	end
	Controls.UsPocketAllowEmbassy:RegisterCallback( Mouse.eLClick, PocketAllowEmbassyHandler );
	Controls.ThemPocketAllowEmbassy:RegisterCallback( Mouse.eLClick, PocketAllowEmbassyHandler );
	Controls.UsPocketAllowEmbassy:SetVoid1( 1 );
	Controls.ThemPocketAllowEmbassy:SetVoid1( 0 );

	function TableAllowEmbassyHandler( isUs )
		--print("TableAllowEmbassyHandler");
		if( isUs == 1 ) then
			--print( "remove: us " .. g_iUs );
			g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iUs );
		else
			--print( "remove: them " .. g_iThem );
			g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iThem );
		end

		DoClearTable();
		DisplayDeal();
		DoUIDealChangedByHuman();
	end
	Controls.UsTableAllowEmbassy:RegisterCallback( Mouse.eLClick, TableAllowEmbassyHandler );
	Controls.ThemTableAllowEmbassy:RegisterCallback( Mouse.eLClick, TableAllowEmbassyHandler );
	Controls.UsTableAllowEmbassy:SetVoid1( 1 );
	Controls.ThemTableAllowEmbassy:SetVoid1( 0 );

end -- gk_mode

-----------------------------------------------------------------------------------------------------------------------
-- Declaration of Friendship Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketDoFHandler( isUs )
	--print("PocketDoFHandler");
	-- The Declaration of Friendship must be on both sides of the deal
	g_Deal:AddDeclarationOfFriendship(g_iUs);
	g_Deal:AddDeclarationOfFriendship(g_iThem);

	DisplayDeal();
	DoUIDealChangedByHuman();
end

----------------------------------------------------
function TableDoFHandler( isUs )
	--print("TableDoFHandler");
	-- If removing the Declaration of Friendship, it must be removed from both sides
	g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iUs );
	g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iThem );

	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end

-----------------------------------------------------------------------------------------------------------------------
-- Open Borders Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketOpenBordersHandler( isUs )

	if( isUs == 1 ) then
		g_Deal:AddOpenBorders( g_iUs, g_iDealDuration );
	else
		g_Deal:AddOpenBorders( g_iThem, g_iDealDuration );
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketOpenBorders:RegisterCallback( Mouse.eLClick, PocketOpenBordersHandler );
Controls.ThemPocketOpenBorders:RegisterCallback( Mouse.eLClick, PocketOpenBordersHandler );
Controls.UsPocketOpenBorders:SetVoid1( 1 );
Controls.ThemPocketOpenBorders:SetVoid1( 0 );

function TableOpenBordersHandler( isUs )

	if( isUs == 1 ) then
		--print( "remove: us " .. g_iUs );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iUs );
	else
		--print( "remove: them " .. g_iThem );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iThem );
	end


	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableOpenBorders:RegisterCallback( Mouse.eLClick, TableOpenBordersHandler );
Controls.ThemTableOpenBorders:RegisterCallback( Mouse.eLClick, TableOpenBordersHandler );
Controls.UsTableOpenBorders:SetVoid1( 1 );
Controls.ThemTableOpenBorders:SetVoid1( 0 );




-----------------------------------------------------------------------------------------------------------------------
-- Defensive Pact Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketDefensivePactHandler( isUs )

	-- Note that currently Defensive Pact is required on both sides

	if( isUs == 1 ) then
		g_Deal:AddDefensivePact( g_iUs, g_iDealDuration );
		g_Deal:AddDefensivePact( g_iThem, g_iDealDuration );
	else
		g_Deal:AddDefensivePact( g_iUs, g_iDealDuration );
		g_Deal:AddDefensivePact( g_iThem, g_iDealDuration );
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketDefensivePact:RegisterCallback( Mouse.eLClick, PocketDefensivePactHandler );
Controls.ThemPocketDefensivePact:RegisterCallback( Mouse.eLClick, PocketDefensivePactHandler );
Controls.UsPocketDefensivePact:SetVoid1( 1 );
Controls.ThemPocketDefensivePact:SetVoid1( 0 );

function TableDefensivePactHandler()
	-- Remove from BOTH sides of the table
	g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iUs );
	g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iThem );
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableDefensivePact:RegisterCallback( Mouse.eLClick, TableDefensivePactHandler );
Controls.ThemTableDefensivePact:RegisterCallback( Mouse.eLClick, TableDefensivePactHandler );




-----------------------------------------------------------------------------------------------------------------------
-- Research Agreement Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketResearchAgreementHandler( isUs )

	-- Note that currently Research Agreement is required on both sides

	if( isUs == 1 ) then
		g_Deal:AddResearchAgreement( g_iUs, g_iDealDuration );
		g_Deal:AddResearchAgreement( g_iThem, g_iDealDuration );
	else
		g_Deal:AddResearchAgreement( g_iUs, g_iDealDuration );
		g_Deal:AddResearchAgreement( g_iThem, g_iDealDuration );
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketResearchAgreement:RegisterCallback( Mouse.eLClick, PocketResearchAgreementHandler );
Controls.ThemPocketResearchAgreement:RegisterCallback( Mouse.eLClick, PocketResearchAgreementHandler );
Controls.UsPocketResearchAgreement:SetVoid1( 1 );
Controls.ThemPocketResearchAgreement:SetVoid1( 0 );

function TableResearchAgreementHandler()
	-- Remove from BOTH sides of the table
	g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iUs );
	g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iThem );
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableResearchAgreement:RegisterCallback( Mouse.eLClick, TableResearchAgreementHandler );
Controls.ThemTableResearchAgreement:RegisterCallback( Mouse.eLClick, TableResearchAgreementHandler );



-----------------------------------------------------------------------------------------------------------------------
-- Handle the strategic and luxury resources
-----------------------------------------------------------------------------------------------------------------------
function PocketResourceHandler( isUs, resourceId )

	local iAmount = 1;

--	if ( GameInfo.Resources[ resourceId ].ResourceUsage == 2 ) then -- is a luxury resource
--		iAmount = 1;
--	end

	if( isUs == 1 ) then
		iAmount = math.min(g_pUs:GetNumResourceAvailable(resourceId, false),iAmount);
		g_Deal:AddResourceTrade( g_iUs, resourceId, iAmount, g_iDealDuration );
	else
		iAmount = math.min(g_pThem:GetNumResourceAvailable(resourceId, false),iAmount);
		g_Deal:AddResourceTrade( g_iThem, resourceId, iAmount, g_iDealDuration );
	end

	DisplayDeal();
	DoUIDealChangedByHuman();
end


function TableResourceHandler( isUs, resourceId )
	g_Deal:RemoveResourceTrade( resourceId );
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end


function AddPocketResource( row, isUs, stack )
	local controlTable = {};
	ContextPtr:BuildInstanceForControl( "PocketResource", controlTable, stack );

	controlTable.Button:SetText( "" );		-- Text and quantity will be set for the specific player when the UI comes up
	controlTable.Button:SetVoids( isUs, row.ID );
	controlTable.Button:RegisterCallback( Mouse.eLClick, PocketResourceHandler );

	if( isUs == 1 ) then
		g_UsPocketResources[ row.ID ] = controlTable;
	else
		g_ThemPocketResources[ row.ID ] = controlTable;
	end
end


function AddTableStrategic( row, isUs, stack )
	local controlTable = {};
	ContextPtr:BuildInstanceForControl( "TableStrategic", controlTable, stack );

	local strString = row.IconString .. " " .. Locale.ConvertTextKey(row.Description);
	controlTable.Button:SetText( strString );
	controlTable.Button:SetVoids( isUs, row.ID );
	controlTable.Button:RegisterCallback( Mouse.eLClick, TableResourceHandler );
	controlTable.AmountEdit:RegisterCallback( ChangeResourceAmount );

	if( isUs == 1 ) then
		controlTable.AmountEdit:SetVoid1( 1 );
		g_UsTableResources[ row.ID ] = controlTable;
	else
		controlTable.AmountEdit:SetVoid1( 0 );
		g_ThemTableResources[ row.ID ] = controlTable;
	end

	controlTable.AmountEdit:SetVoid2( row.ID );

	g_LuxuryList[ row.ID ] = false;
end


function AddTableLuxury( row, isUs, stack )
	local controlTable = {};
	ContextPtr:BuildInstanceForControl( "TableLuxury", controlTable, stack );

	local strString = row.IconString .. " " .. Locale.ConvertTextKey(row.Description);
	controlTable.Button:SetText( strString );
	controlTable.Button:SetVoids( isUs, row.ID );
	controlTable.Button:RegisterCallback( Mouse.eLClick, TableResourceHandler );

	if( isUs == 1 ) then
		g_UsTableResources[ row.ID ] = controlTable;
	else
		g_ThemTableResources[ row.ID ] = controlTable;
	end

	g_LuxuryList[ row.ID ] = true;
end


-------------------------------------------------------------------
-------------------------------------------------------------------
function ChangeResourceAmount( string, control )

	local bIsUs = control:GetVoid1() == 1;
	local iResourceID = control:GetVoid2();

	local pPlayer;
	local iPlayer;
	if (bIsUs) then
		iPlayer = g_iUs;
	else
		iPlayer = g_iThem;
	end
	pPlayer = Players[iPlayer];

	local iNumResource = 0;
	if( string ~= nil and string ~= "" ) then
		iNumResource = tonumber(string);
--	else
--		control:SetText( 0 );
	end

	-- Can't offer more than someone has
	iNumResource = math.min(pPlayer:GetNumResourceAvailable(iResourceID, false),iNumResource);
	iNumResource = math.max(iNumResource, 1);

	control:SetText(iNumResource);

	if ( bIsUs ) then
		g_Deal:ChangeResourceTrade( g_iUs, iResourceID, iNumResource, g_iDealDuration );
	else
		g_Deal:ChangeResourceTrade( g_iThem, iResourceID, iNumResource, g_iDealDuration );
	end

	--CBP
	DoUIDealChangedByHuman();
	--END
end

-----------------------------------------------------------------------------------------------------------------------
-- Map Trading Handlers						-- Putmalk
-----------------------------------------------------------------------------------------------------------------------
function PocketTradeMapHandler( isUs )
	print("PocketTradeMapHandler");
	if (isUs == 1 ) then
		g_Deal:AddMapTrade(g_iUs);
	else
		g_Deal:AddMapTrade(g_iThem);
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketTradeMap:RegisterCallback( Mouse.eLClick, PocketTradeMapHandler );
Controls.UsPocketTradeMap:SetVoid1( 1 );
Controls.ThemPocketTradeMap:RegisterCallback( Mouse.eLClick, PocketTradeMapHandler );
Controls.ThemPocketTradeMap:SetVoid1( 0 );

function TableMapHandler( isUs )
	print("TableTradeMapHandler");
	if( isUs == 1 ) then
		print( "remove: us " .. g_iUs );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_MAPS, g_iUs );
	else
		print( "remove: them " .. g_iThem );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_MAPS, g_iThem );
	end
	
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableTradeMap:RegisterCallback( Mouse.eLClick, TableMapHandler );
Controls.UsTableTradeMap:SetVoid1( 1 );
Controls.ThemTableTradeMap:RegisterCallback( Mouse.eLClick, TableMapHandler );
Controls.ThemTableTradeMap:SetVoid1( 0 );
-----------------------------------------------------------------------------------------------------------------------
-- Vassalage Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketVassalageHandler( isUs )
	print("PocketVassalageHandler");
	if(isUs == 1) then
		g_Deal:AddVassalageTrade(g_iUs);
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE, g_iThem );
		--print("Calling PocketVassalageHandler with isUs = 1. WHY YOU DO THIS GAME.");
	else
		g_Deal:AddVassalageTrade(g_iThem);
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE, g_iUs );
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketVassalage:RegisterCallback( Mouse.eLClick, PocketVassalageHandler );
Controls.UsPocketVassalage:SetVoid1( 1 );
Controls.ThemPocketVassalage:RegisterCallback( Mouse.eLClick, PocketVassalageHandler );
Controls.ThemPocketVassalage:SetVoid1( 0 );

function TableVassalageHandler( isUs )
	print("TableVassalageHandler");
	if(isUs == 1) then
		print("remove: us " .. g_iUs );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE, g_iUs );
	else
		print("remove: them " .. g_iThem );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE, g_iThem );
	end
	
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableVassalage:RegisterCallback( Mouse.eLClick, TableVassalageHandler );
Controls.UsTableVassalage:SetVoid1( 1 );
Controls.ThemTableVassalage:RegisterCallback( Mouse.eLClick, TableVassalageHandler );
Controls.ThemTableVassalage:SetVoid1( 0 );

-----------------------------------------------------------------------------------------------------------------------
-- Revoke Vassalage Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketRevokeVassalageHandler( isUs )
	print("PocketRevokeVassalageHandler");
	if(isUs == 1) then
		g_Deal:AddRevokeVassalageTrade(g_iUs);
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iThem );
		--print("Calling PocketVassalageHandler with isUs = 1. WHY YOU DO THIS GAME.");
	else
		g_Deal:AddRevokeVassalageTrade(g_iThem);
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iUs );
	end
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsPocketRevokeVassalage:RegisterCallback( Mouse.eLClick, PocketRevokeVassalageHandler );
Controls.UsPocketRevokeVassalage:SetVoid1( 1 );
Controls.ThemPocketRevokeVassalage:RegisterCallback( Mouse.eLClick, PocketRevokeVassalageHandler );
Controls.ThemPocketRevokeVassalage:SetVoid1( 0 );

function TableRevokeVassalageHandler( isUs )
	print("TableRevokeVassalageHandler");
	if(isUs == 1) then
		print("remove: us " .. g_iUs );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iUs );
	else
		print("remove: them " .. g_iThem );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_VASSALAGE_REVOKE, g_iThem );
	end
	
	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end
Controls.UsTableRevokeVassalage:RegisterCallback( Mouse.eLClick, TableRevokeVassalageHandler );
Controls.UsTableRevokeVassalage:SetVoid1( 1 );
Controls.ThemTableRevokeVassalage:RegisterCallback( Mouse.eLClick, TableRevokeVassalageHandler );
Controls.ThemTableRevokeVassalage:SetVoid1( 0 );

-----------------------------------------------------------------------------------------------------------------------
-- Handle the techs
-----------------------------------------------------------------------------------------------------------------------
function PocketTechHandler( isUs, techId )
	if( isUs == 1 ) then
		g_Deal:AddTechTrade( g_iUs, techId );
	else
		g_Deal:AddTechTrade( g_iThem, techId );
	end
	
	DisplayDeal();
	DoUIDealChangedByHuman();
end

function TableTechHandler( isUs, techId )
	g_Deal:RemoveTechTrade(techId);
    DoClearTable();
    DisplayDeal();
    DoUIDealChangedByHuman();
end

function AddPocketTech( row, isUs, stack )
    local controlTable = {};
    ContextPtr:BuildInstanceForControl( "PocketTechnology", controlTable, stack );
       
	controlTable.Button:SetText( " " );
	controlTable.Button:SetVoids( isUs, row.ID );
    controlTable.Button:RegisterCallback( Mouse.eLClick, PocketTechHandler );
    
    if( isUs == 1 ) then
        g_UsPocketTechs[ row.ID ] = controlTable;
    else
        g_ThemPocketTechs[ row.ID ] = controlTable;
    end
end

 
function AddTableTech( row, isUs, stack )
    local controlTable = {};
    ContextPtr:BuildInstanceForControl( "TableTechnology", controlTable, stack );
    
    local strString = Locale.ConvertTextKey(row.Description);
    controlTable.Button:SetText( strString );
    controlTable.Button:SetVoids( isUs, row.ID );
    controlTable.Button:RegisterCallback( Mouse.eLClick, TableTechHandler );
    
    if( isUs == 1 ) then
        g_UsTableTechs[ row.ID ] = controlTable;
    else
        g_ThemTableTechs[ row.ID ] = controlTable;
    end
end
-------------------------------------------------
-- Help text for techs
-------------------------------------------------
-- Putmalk: We need custom tooltip for techs, because when we hover over our techs, we don't know their progress
-- to-do: know their progress if we have a spy in one of their cities???

-------------------------------------------------------------------
-- Populate the lists
-------------------------------------------------------------------
for row in GameInfo.Resources( "ResourceUsage = 1" )
do
	--print( "adding strat: " .. row.Type );
	AddPocketResource( row, 1, Controls.UsPocketStrategicStack   );
	AddPocketResource( row, 0, Controls.ThemPocketStrategicStack );
	AddTableStrategic( row, 1, Controls.UsTableStrategicStack    );
	AddTableStrategic( row, 0, Controls.ThemTableStrategicStack  );
end

for row in GameInfo.Resources( "ResourceUsage = 2" )
do
	--print( "adding lux: " .. row.Type );
	AddPocketResource( row, 1, Controls.UsPocketLuxuryStack   );
	AddPocketResource( row, 0, Controls.ThemPocketLuxuryStack );
	AddTableLuxury(    row, 1, Controls.UsTableLuxuryStack    );
	AddTableLuxury(    row, 0, Controls.ThemTableLuxuryStack  );
end
-- Populate the technology list
-- Putmalk
if( not g_bDisableTechTrading) then
	for row in GameInfo.Technologies( )
	do
		--print( "adding tech: " .. row.Type );
		AddPocketTech( 	row, 1, Controls.UsPocketTechnologyStack );
		AddPocketTech( 	row, 0, Controls.ThemPocketTechnologyStack );
		AddTableTech(	row, 1, Controls.UsTableTechnologyStack );
		AddTableTech( 	row, 0, Controls.ThemTableTechnologyStack );
	end
end
if bnw_mode then
	-----------------------------------------------------------------------------------------------------------------------
	-- Handle votes
	-----------------------------------------------------------------------------------------------------------------------

	-- Update a global table, refer to proposal and vote pair as a single integer index because of SetVoids limit
	function UpdateLeagueVotes()
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

	function OnChoosePocketVote(iFromPlayer, iVoteIndex)
		local pLeague = nil;
		if (Game.GetNumActiveLeagues() > 0) then
			pLeague = Game.GetActiveLeague();
		end

		if (pLeague ~= nil and g_LeagueVoteList[iVoteIndex] ~= nil) then
			local iResolutionID = g_LeagueVoteList[iVoteIndex].ID;
			local iVoteChoice = g_LeagueVoteList[iVoteIndex].VoteChoice;
			-- CBP EDIT
			local iNumChooseVotes = 0;
			if(iFromPlayer == g_iUs) then
				iNumChooseVotes = pLeague:GetPotentialVotesForMember(g_iThem, g_iUs);
			else
				iNumChooseVotes = pLeague:GetPotentialVotesForMember(g_iUs, g_iThem);
			end
			-- END	
			--local iNumVotes = pLeague:GetCoreVotesForMember(iFromPlayer);
			local bRepeal = g_LeagueVoteList[iVoteIndex].Repeal;
			--print("==debug== Vote added to deal, ID=" .. iResolutionID .. ", VoteChoice=" .. iVoteChoice .. ", NumVotes=" .. iNumVotes);
			g_Deal:AddVoteCommitment(iFromPlayer, iResolutionID, iVoteChoice, iNumChooseVotes, bRepeal);

			DisplayDeal();
			DoUIDealChangedByHuman();
		else
			print("SCRIPTING ERROR: Could not find valid vote when pocket vote was selected");
		end
	end

function RefreshPocketVotes(iIsUs)
	UpdateLeagueVotes();
	if (iIsUs == 1) then
		g_UsPocketVoteIM:ResetInstances();
		g_bAnyVoteUs = false;
	else
		g_ThemPocketVoteIM:ResetInstances();
		g_bAnyVoteThem = false;
	end

	local pLeague = nil;
	if (Game.GetNumActiveLeagues() > 0) then
		pLeague = Game.GetActiveLeague();
	end
	
	if (pLeague ~= nil) then
		for i,tVote in ipairs(g_LeagueVoteList) do
			-- Us to them?
			if (iIsUs == 1) then
				local iNumUsVotes = pLeague:GetPotentialVotesForMember(g_iThem, g_iUs);	
				local sProposalText = GetVoteText(pLeague, i, tVote.Repeal, iNumUsVotes);
				local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
				local sTooltip = GetVoteTooltip(pLeague, i, tVote.Repeal, iNumUsVotes);
				if (g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, tVote.ID, tVote.VoteChoice, iNumUsVotes, tVote.Repeal)) then
					g_bAnyVoteUs = true;
					local cInstance = g_UsPocketVoteIM:GetInstance();
					cInstance.ProposalLabel:SetText(sProposalText);
					if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, g_iUs, false, g_iDealDuration, tVote.ID, tVote.VoteChoice, iNumUsVotes, tVote.Repeal)) then
						sChoiceText = "[COLOR_NEGATIVE_TEXT]" .. sChoiceText .. "[ENDCOLOR]";
					end
					cInstance.VoteLabel:SetText(sChoiceText);
					cInstance.Button:SetToolTipString(sTooltip);
					cInstance.Button:SetVoids(g_iUs, i);
					cInstance.Button:RegisterCallback(Mouse.eLClick, OnChoosePocketVote);
				end
			-- Them to us?
			else
				local iNumThemVotes = pLeague:GetPotentialVotesForMember(g_iUs, g_iThem);
				local sProposalText = GetVoteText(pLeague, i, tVote.Repeal, iNumThemVotes);
				local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
				local sTooltip = GetVoteTooltip(pLeague, i, tVote.Repeal, iNumThemVotes);
				if (g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, tVote.ID, tVote.VoteChoice, iNumThemVotes, tVote.Repeal)) then
					g_bAnyVoteThem = true;
					local cInstance = g_ThemPocketVoteIM:GetInstance();
					cInstance.ProposalLabel:SetText(sProposalText);
					if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, g_iUs, true, g_iDealDuration, tVote.ID, tVote.VoteChoice, iNumUsVotes, tVote.Repeal)) then
						sChoiceText = "[COLOR_NEGATIVE_TEXT]" .. sChoiceText .. "[ENDCOLOR]";
					end
					cInstance.VoteLabel:SetText(sChoiceText);
					cInstance.Button:SetToolTipString(sTooltip);
					cInstance.Button:SetVoids(g_iThem, i);
					cInstance.Button:RegisterCallback(Mouse.eLClick, OnChoosePocketVote);
				end
			end
		end
	end
end

	function OnChooseTableVote(iFromPlayer, iVoteIndex)
		local pLeague = nil;
		if (Game.GetNumActiveLeagues() > 0) then
			pLeague = Game.GetActiveLeague();
		end

		if (pLeague ~= nil and g_LeagueVoteList[iVoteIndex] ~= nil) then
			local iResolutionID = g_LeagueVoteList[iVoteIndex].ID;
			local iVoteChoice = g_LeagueVoteList[iVoteIndex].VoteChoice;
-- CBP EDIT
			local iNumTableVotes = 0;
			if(iFromPlayer == g_iUs) then
				iNumTableVotes = pLeague:GetPotentialVotesForMember(g_iThem, g_iUs);
			else
				iNumTableVotes = pLeague:GetPotentialVotesForMember(g_iUs, g_iThem);
			end
-- END		
			local bRepeal = g_LeagueVoteList[iVoteIndex].Repeal;
			--print("==debug== Vote removed from deal, ID=" .. iResolutionID .. ", VoteChoice=" .. iVoteChoice .. ", NumVotes=" .. iNumVotes);
			g_Deal:RemoveVoteCommitment(iFromPlayer, iResolutionID, iVoteChoice, iNumTableVotes, bRepeal);

			DoClearTable();
			DisplayDeal();
			DoUIDealChangedByHuman();
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
					--CBP
					sProposalInfo = sProposalInfo .. GetVoteText(pLeague, iVoteIndex, bRepeal, iNumVotes);
					sProposalInfo = sProposalInfo .. "[NEWLINE][NEWLINE]" .. Locale.Lookup(GameInfo.Resolutions[tVote.Type].Help);
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


-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
function OnChooseCity( playerID, cityID )
	g_Deal:AddCityTrade( playerID, cityID );

	if( playerID == g_iUs ) then
		CityClose( 0, 1 );
	else
		CityClose( 0, 0 );
	end

	DisplayDeal();
	DoUIDealChangedByHuman();
end

----------------------------------------------------
----------------------------------------------------
function CityClose( _, isUs )

	if( isUs == 1 ) then
		Controls.UsPocketCitiesStack:SetHide( true );
		Controls.UsPocketCitiesStack:CalculateSize();

		Controls.UsPocketStack:SetHide( false );
		Controls.UsPocketStack:CalculateSize();

		Controls.UsPocketPanel:CalculateInternalSize();
		Controls.UsPocketPanel:SetScrollValue( 0 );
	else
		Controls.ThemPocketCitiesStack:SetHide( true );
		Controls.ThemPocketCitiesStack:CalculateSize();

		Controls.ThemPocketStack:SetHide( false );
		Controls.ThemPocketStack:CalculateSize();

		Controls.ThemPocketPanel:CalculateInternalSize();
		Controls.ThemPocketPanel:SetScrollValue( 0 );
	end

end
Controls.UsPocketCitiesClose:RegisterCallback( Mouse.eLClick, CityClose );
Controls.UsPocketCitiesClose:SetVoid2( 1 );
Controls.ThemPocketCitiesClose:RegisterCallback( Mouse.eLClick, CityClose );
Controls.ThemPocketCitiesClose:SetVoid2( 0 );


-------------------------------------------------------------
function TableCityHandler( playerID, cityID )
	g_Deal:RemoveCityTrade( playerID, cityID );

	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end

-------------------------------------------------------------
function ShowCityChooser( isUs )

	--print( "ShowCityChooser" );
	local m_pTo;
	local m_pFrom;
	local m_iTo;
	local m_iFrom;
	local m_pIM;
	local m_pocketStack;
	local m_citiesStack;
	local m_panel;

	if( isUs == 1 ) then
		m_pocketStack = Controls.UsPocketStack;
		m_citiesStack = Controls.UsPocketCitiesStack;
		m_panel       = Controls.UsPocketPanel;
		m_pIM   = g_UsPocketCitiesIM;
		m_iTo   = g_iThem;
		m_iFrom = g_iUs;
		m_pTo   = g_pThem;
		m_pFrom = g_pUs;
	else
		m_pocketStack = Controls.ThemPocketStack;
		m_citiesStack = Controls.ThemPocketCitiesStack;
		m_panel       = Controls.ThemPocketPanel;
		m_pIM   = g_ThemPocketCitiesIM;
		m_iTo   = g_iUs;
		m_iFrom = g_iThem;
		m_pTo   = g_pUs;
		m_pFrom = g_pThem;
	end

	m_pIM:ResetInstances();
	m_pocketStack:SetHide( true );
	m_citiesStack:SetHide( false );

	local instance;
	local bFound = false;
	for pCity in m_pFrom:Cities() do

		local iCityID = pCity:GetID();

		if ( g_Deal:IsPossibleToTradeItem( m_iFrom, m_iTo, TradeableItems.TRADE_ITEM_CITIES, pCity:GetX(), pCity:GetY() ) ) then
			local instance = m_pIM:GetInstance();
			if(g_pThem:IsTradeItemValuedImpossible(TradeableItems.TRADE_ITEM_CITIES, g_iUs, isUs==0
			, g_iDealDuration, pCity:GetX(), pCity:GetY())) then
				instance.CityName:SetText("[COLOR_NEGATIVE_TEXT]" ..  pCity:GetName()  .."[ENDCOLOR]")
				instance.CityPop:SetText("[COLOR_NEGATIVE_TEXT]" ..  pCity:GetPopulation()  .."[ENDCOLOR]");
			else
				instance.CityName:SetText( pCity:GetName() );
				instance.CityPop:SetText( pCity:GetPopulation() );
			end
			instance.Button:SetVoids( m_iFrom, iCityID );
			instance.Button:RegisterCallback( Mouse.eLClick, OnChooseCity );

			bFound = true;
		end
	end

	if( not bFound ) then
		m_pocketStack:SetHide( false );
		m_citiesStack:SetHide( true );

		if( isUs == 1 ) then
			Controls.UsPocketCities:SetDisabled( false );
			Controls.UsPocketCities:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT" );
			Controls.UsPocketCities:GetTextControl():SetColorByName("Gray_Black");
		else
			Controls.ThemPocketCities:SetDisabled( true );
			Controls.ThemPocketCities:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM" );
			Controls.ThemPocketCities:GetTextControl():SetColorByName("Gray_Black");
		end
	end

	m_citiesStack:CalculateSize();
	m_pocketStack:CalculateSize();
	m_panel:CalculateInternalSize();
	m_panel:SetScrollValue( 0 );
end

Controls.UsPocketCities:SetVoid1( 1 );
Controls.UsPocketCities:RegisterCallback( Mouse.eLClick, ShowCityChooser );

Controls.ThemPocketCities:SetVoid1( 0 );
Controls.ThemPocketCities:RegisterCallback( Mouse.eLClick, ShowCityChooser );
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------------------------------------
-- OtherPlayer leader lists
-----------------------------------------------------------------------------------------------------------------------
function ShowOtherPlayerChooser(isUs, type)
	local SubTableName;
	local iFromPlayer;
	local tradeType;
	local iToPlayer;

	if (type == WAR) then
		tradeType = TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR;
	elseif (type == PEACE) then
		tradeType = TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE;
	end

	local pocketStack;
	local leaderStack;
	local panel;

	-- disable invalid players
	if (isUs == 1) then
		g_UsOtherPlayerMode   = type;
		pocketStack = Controls.UsPocketStack;
		leaderStack = Controls.UsPocketLeaderStack;
		panel = Controls.UsPocketPanel;
		SubTableName = "UsPocket";
		iFromPlayer = g_iUs;
		iToPlayer = g_iThem;
	else
		g_ThemOtherPlayerMode = type;
		pocketStack = Controls.ThemPocketStack;
		leaderStack = Controls.ThemPocketLeaderStack;
		panel = Controls.ThemPocketPanel;
		SubTableName = "ThemPocket";
		iFromPlayer = g_iThem;
		iToPlayer = g_iUs;
	end

	local bWeAreSurrendering = type == WAR and g_pUsTeam:IsAtWar(g_iThemTeam) and g_Deal:GetSurrenderingPlayer() ~= g_iThem and iFromPlayer == g_iThem and (not g_bPVPTrade);
	local bTheyAreSurrendering = type == WAR and g_pUsTeam:IsAtWar(g_iThemTeam) and g_Deal:GetSurrenderingPlayer() == g_iThem and iFromPlayer == g_iUs and (not g_bPVPTrade);
	local bSanctioned = g_pUs:IsTradeSanctioned(g_iThem) and (not g_pUsTeam:IsAtWar(g_iThemTeam));

	for iLoopPlayer = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
		pLoopPlayer = Players[iLoopPlayer];
		iLoopTeam = pLoopPlayer:GetTeam();
		local otherPlayerButton = g_OtherPlayersButtons[iLoopPlayer];

		if (otherPlayerButton ~= nil) then
			local otherPlayerButtonSubTableNameButton = otherPlayerButton[SubTableName].Button;
			otherPlayerButtonSubTableNameButton:SetHide(true);

			if (g_iUsTeam ~= iLoopTeam and g_iThemTeam ~= iLoopTeam and g_pUsTeam:IsHasMet(iLoopTeam) and g_pThemTeam:IsHasMet(iLoopTeam)) then
				otherPlayerButtonSubTableNameButton:SetHide(false);
				local strTooltip = "";
				
				local szName;
				if( pLoopPlayer:IsHuman() ) then
					szName = pLoopPlayer:GetNickName();
				else
					szName = pLoopPlayer:GetName();
				end
				szName = szName .. " (" .. Locale.ConvertTextKey(GameInfo.Civilizations[pLoopPlayer:GetCivilizationType()].ShortDescription) .. ")";
					
				if (g_Deal:IsPossibleToTradeItem(iFromPlayer, iToPlayer, tradeType, iLoopTeam)) then
					otherPlayerButtonSubTableNameButton:SetDisabled(false);
					if(g_pThem:IsTradeItemValuedImpossible(tradeType, g_iUs, isUs==0, g_iDealDuration, iLoopTeam)) then
						szName = "[COLOR_NEGATIVE_TEXT]" .. szName .. "[ENDCOLOR]";
					end
					otherPlayerButtonSubTableNameButton:SetAlpha(1);
				else
					otherPlayerButtonSubTableNameButton:SetDisabled(true);
					otherPlayerButtonSubTableNameButton:SetAlpha(0.5);

					if (bWeAreSurrendering or bTheyAreSurrendering) then
						strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT");
					elseif (bSanctioned) then
						strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DEAL_VALUE_STR_EMBARGO_TT");
					else
						strTooltip = g_Deal:GetReasonsItemUntradeable(iFromPlayer, iToPlayer, tradeType, iLoopTeam);
					end
				end

				TruncateString(otherPlayerButton[SubTableName].Name, otherPlayerButton[SubTableName].ButtonSize:GetSizeX() - otherPlayerButton[SubTableName].Name:GetOffsetX(), szName);
				otherPlayerButtonSubTableNameButton:SetToolTipString(strTooltip);
			end
		end
	end

	pocketStack:SetHide(true);
	pocketStack:CalculateSize();
	leaderStack:SetHide(false);
	leaderStack:CalculateSize();

	panel:CalculateInternalSize();
	panel:SetScrollValue(0);
end

Controls.UsPocketOtherPlayerWar:SetVoids( 1, WAR );
Controls.UsPocketOtherPlayerWar:RegisterCallback( Mouse.eLClick, ShowOtherPlayerChooser );

Controls.ThemPocketOtherPlayerWar:SetVoids( 0, WAR );
Controls.ThemPocketOtherPlayerWar:RegisterCallback( Mouse.eLClick, ShowOtherPlayerChooser );

Controls.UsPocketOtherPlayerPeace:SetVoids( 1, PEACE );
Controls.UsPocketOtherPlayerPeace:RegisterCallback( Mouse.eLClick, ShowOtherPlayerChooser );

Controls.ThemPocketOtherPlayerPeace:SetVoids( 0, PEACE );
Controls.ThemPocketOtherPlayerPeace:RegisterCallback( Mouse.eLClick, ShowOtherPlayerChooser );


----------------------------------------------------
----------------------------------------------------
function LeaderClose( _, isUs )
	if( isUs == 1 ) then
		Controls.UsPocketLeaderStack:SetHide( true );
		Controls.UsPocketLeaderStack:CalculateSize();

		Controls.UsPocketStack:SetHide( false );
		Controls.UsPocketStack:CalculateSize();

		Controls.UsPocketPanel:CalculateInternalSize();
		Controls.UsPocketPanel:SetScrollValue( 0 );
	else
		Controls.ThemPocketLeaderStack:SetHide( true );
		Controls.ThemPocketLeaderStack:CalculateSize();

		Controls.ThemPocketStack:SetHide( false );
		Controls.ThemPocketStack:CalculateSize();

		Controls.ThemPocketPanel:CalculateInternalSize();
		Controls.ThemPocketPanel:SetScrollValue( 0 );
	end
end
Controls.UsPocketLeaderClose:RegisterCallback( Mouse.eLClick, LeaderClose );
Controls.UsPocketLeaderClose:SetVoid2( 1 );
Controls.ThemPocketLeaderClose:RegisterCallback( Mouse.eLClick, LeaderClose );
Controls.ThemPocketLeaderClose:SetVoid2( 0 );


----------------------------------------------------
-- handler for when the leader is actually clicked
----------------------------------------------------
function LeaderSelected( iOtherPlayer, isUs )

	local iWho;
	local mode;

	LeaderClose( 0, isUs );
	if( isUs == 1 ) then
		iWho = g_iUs;
		mode = g_UsOtherPlayerMode;
	else
		iWho = g_iThem;
		mode = g_ThemOtherPlayerMode;
	end

	local pOtherPlayer = Players[ iOtherPlayer ];
	local iOtherTeam = pOtherPlayer:GetTeam();

	if( mode == WAR ) then
		g_Deal:AddThirdPartyWar( iWho, iOtherTeam );
	else
		g_Deal:AddThirdPartyPeace( iWho, iOtherTeam, g_PeaceDealDuration );
	end

	DisplayDeal();
	DoUIDealChangedByHuman();
end


-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
function OnOtherPlayerTablePeace( iOtherPlayer, isUs )
	local firstParty;
	if (isUs == 1) then
		firstParty = g_iUs;
	else
		firstParty = g_iThem;
	end

	local bRemoveDeal = true;
	local iOtherTeam = Players[iOtherPlayer]:GetTeam();

	--print("iOtherPlayer: " .. iOtherPlayer);

	-- if this is a peace negotiation
	if (g_pUsTeam:IsAtWar(g_iThemTeam )) then
		--print("Peace negotiation");
		-- if the player is a minor
		if (Players[iOtherPlayer]:IsMinorCiv()) then
			--print("Other civ is a minor civ");
			local iAlly = Players[iOtherPlayer]:GetAlly();
			--print("iAlly: " .. iAlly);
			--print("g_iUs: " .. g_iUs);
			--print("g_iThem: " .. g_iThem);
			-- if the minor is allied with either us or them
			if (iAlly == g_iUs or iAlly == g_iThem) then
				-- don't allow the third party peace deal to be removed
				--print("Don't allow removal because they are a third-party ally");
				bRemoveDeal = false;
			end
		end
	end

	if (bRemoveDeal) then
		--print("Removing deal");
		g_Deal:RemoveThirdPartyPeace( firstParty, iOtherTeam );

		DoClearTable();
		DisplayDeal();
		DoUIDealChangedByHuman();
	end
end


-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
function OnOtherPlayerTableWar( iOtherPlayer, isUs )
	local firstParty;
	if( isUs == 1 ) then
		firstParty = g_iUs;
	else
		firstParty = g_iThem;
	end

	local pOtherPlayer = Players[ iOtherPlayer ];
	local iOtherTeam = pOtherPlayer:GetTeam();
	g_Deal:RemoveThirdPartyWar( firstParty, iOtherTeam );

	DoClearTable();
	DisplayDeal();
	DoUIDealChangedByHuman();
end


-----------------------------------------------------------------------------------------------------------------------
-- Build the THIRD_PARTY lists. These are also know as thirdparty if you're doing a text search
-- the only thing we trade in third party mode is war/peace, so make sure those aren't disabled
-----------------------------------------------------------------------------------------------------------------------
if( Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR ) or
	Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE ) or
	Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE ) ) then

	Controls.UsPocketOtherPlayer:SetHide( true );
	Controls.UsPocketOtherPlayerStack:SetHide( true );
	Controls.ThemPocketOtherPlayer:SetHide( true );
	Controls.ThemPocketOtherPlayerStack:SetHide( true );

	g_bEnableThirdParty = false;

else
	g_bEnableThirdParty = true;
	for iLoopPlayer = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do

		local pLoopPlayer = Players[ iLoopPlayer ];
		local iLoopTeam = pLoopPlayer:GetTeam();

		if( pLoopPlayer:IsAlive() ) then

			local newButtonsTable = {};
			g_OtherPlayersButtons[ iLoopPlayer ] = newButtonsTable;

			local szName;
			if( pLoopPlayer:IsHuman() ) then
				szName = pLoopPlayer:GetNickName();
			else
				szName = pLoopPlayer:GetName();
			end
			szName = szName .. " (" .. Locale.ConvertTextKey(GameInfo.Civilizations[Players[iLoopPlayer]:GetCivilizationType()].ShortDescription) .. ")";

			-- Us Pocket
			local controlTable = {};
			ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", controlTable, Controls.UsPocketLeaderStack );
			CivIconHookup( iLoopPlayer, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
			TruncateString(controlTable.Name, controlTable.ButtonSize:GetSizeX() - controlTable.Name:GetOffsetX(), szName);
			controlTable.Button:SetVoids( iLoopPlayer, 1 );
			controlTable.Button:RegisterCallback( Mouse.eLClick, LeaderSelected );
			newButtonsTable.UsPocket = controlTable;


			-- Them Pocket
			controlTable = {};
			ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", controlTable, Controls.ThemPocketLeaderStack );
			CivIconHookup( iLoopPlayer, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
			TruncateString(controlTable.Name, controlTable.ButtonSize:GetSizeX() - controlTable.Name:GetOffsetX(), szName);
			controlTable.Button:SetVoids( iLoopPlayer, 0 );
			controlTable.Button:RegisterCallback( Mouse.eLClick, LeaderSelected );
			newButtonsTable.ThemPocket = controlTable;


			-- Us Table Peace
			controlTable = {};
			ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", controlTable, Controls.UsTableMakePeaceStack );
			CivIconHookup( iLoopPlayer, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
			TruncateString(controlTable.Name, controlTable.ButtonSize:GetSizeX() - controlTable.Name:GetOffsetX(), szName);
			controlTable.Button:SetVoids( iLoopPlayer, 1 );
			controlTable.Button:RegisterCallback( Mouse.eLClick, OnOtherPlayerTablePeace );
			newButtonsTable.UsTablePeace = controlTable;


			-- Us Table War
			controlTable = {};
			ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", controlTable, Controls.UsTableDeclareWarStack );
			CivIconHookup( iLoopPlayer, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
			TruncateString(controlTable.Name, controlTable.ButtonSize:GetSizeX() - controlTable.Name:GetOffsetX(), szName);
			controlTable.Button:SetVoids( iLoopPlayer, 1 );
			controlTable.Button:RegisterCallback( Mouse.eLClick, OnOtherPlayerTableWar );
			newButtonsTable.UsTableWar = controlTable;


			-- Them Table Peace
			controlTable = {};
			ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", controlTable, Controls.ThemTableMakePeaceStack );
			CivIconHookup( iLoopPlayer, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
			TruncateString(controlTable.Name, controlTable.ButtonSize:GetSizeX() - controlTable.Name:GetOffsetX(), szName);
			controlTable.Button:SetVoids( iLoopPlayer, 0 );
			controlTable.Button:RegisterCallback( Mouse.eLClick, OnOtherPlayerTablePeace );
			newButtonsTable.ThemTablePeace = controlTable;


			-- Them Table War
			controlTable = {};
			ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", controlTable, Controls.ThemTableDeclareWarStack );
			CivIconHookup( iLoopPlayer, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
			TruncateString(controlTable.Name, controlTable.ButtonSize:GetSizeX() - controlTable.Name:GetOffsetX(), szName);
			controlTable.Button:SetVoids( iLoopPlayer, 0 );
			controlTable.Button:RegisterCallback( Mouse.eLClick, OnOtherPlayerTableWar );
			newButtonsTable.ThemTableWar = controlTable;

		end
	end
end
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------

Controls.UsMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_PeaceDealDuration);
Controls.UsDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_PeaceDealDuration);
Controls.ThemMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_PeaceDealDuration);
Controls.ThemDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_PeaceDealDuration);

ResetDisplay();
DisplayDeal();

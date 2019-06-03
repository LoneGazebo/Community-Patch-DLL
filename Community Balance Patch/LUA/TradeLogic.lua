print("This is the modded TradeLogic from CBP")
----------------------------------------------------------------        
----------------------------------------------------------------        
include( "IconSupport" );
include( "InstanceManager" );
include( "SupportFunctions"  );

local g_UsTableCitiesIM     = InstanceManager:new( "CityInstance", "Button", Controls.UsTableCitiesStack );
local g_UsPocketCitiesIM    = InstanceManager:new( "CityInstance", "Button", Controls.UsPocketCitiesStack );
local g_ThemTableCitiesIM   = InstanceManager:new( "CityInstance", "Button", Controls.ThemTableCitiesStack );
local g_ThemPocketCitiesIM  = InstanceManager:new( "CityInstance", "Button", Controls.ThemPocketCitiesStack );
local g_UsTableVoteIM		= InstanceManager:new( "TableVote", "Container", Controls.UsTableVoteStack );
local g_UsPocketVoteIM		= InstanceManager:new( "PocketVote", "Button", Controls.UsPocketVoteStack );
local g_ThemTableVoteIM		= InstanceManager:new( "TableVote", "Container", Controls.ThemTableVoteStack );
local g_ThemPocketVoteIM	= InstanceManager:new( "PocketVote", "Button", Controls.ThemPocketVoteStack );

local g_bAlwaysWar = Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR );
local g_bAlwaysPeace = Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE );
local g_bNoChangeWar = Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE );

----------------------------------------------------------------        
-- local storage
----------------------------------------------------------------        
local g_Deal = UI.GetScratchDeal(); 
local g_iDiploUIState;
local g_bPVPTrade;
local g_bTradeReview = false;
local g_iNumOthers;
local g_bEnableThirdParty = true;

local g_iConcessionsPreviousDiploUIState = -1;
local g_bHumanOfferingConcessions = false;

local g_iDealDuration = Game.GetDealDuration();
local g_iPeaceDuration = Game.GetPeaceDuration();

local g_iUs = -1; --Game.GetActivePlayer();
local g_iThem = -1;
local g_pUs = -1;
local g_pThem = -1;
local g_iUsTeam = -1;
local g_iThemTeam = -1;
local g_pUsTeam = -1;
local g_pThemTeam = -1;
local g_UsPocketResources   = {};
local g_ThemPocketResources = {};
local g_UsTableResources    = {};
local g_ThemTableResources  = {};
local g_LuxuryList          = {};
local g_LeagueVoteList		= {};

local g_bMessageFromDiploAI = false;
local g_bAIMakingOffer = false;

local g_UsOtherPlayerMode   = -1;
local g_ThemOtherPlayerMode = -1;

local g_OtherPlayersButtons = {};

local g_bNewDeal = true;

local WAR     = 0;
local PEACE   = 1;

local PROPOSE_TYPE = 1;  -- player is making a proposal
local WITHDRAW_TYPE = 2;  -- player is withdrawing their offer
local ACCEPT_TYPE   = 3;  -- player is accepting someone else's proposal

local CANCEL_TYPE   = 0;  -- player is canceling their draft
local REFUSE_TYPE   = 1;  -- player is refusing someone else's deal

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
	
	local bMyMode = false;
	
	-- Are we in Trade Mode?
	if (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
		bMyMode = true;
	end
	
	if (bMyMode) then
		
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
			
		elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND) then
			--print("DiploUIState: Human Demand");
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
            g_Deal:AddPeaceTreaty( g_iUs, GameDefines.PEACE_TREATY_LENGTH );
            g_Deal:AddPeaceTreaty( g_iThem, GameDefines.PEACE_TREATY_LENGTH );
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
    		g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
    		
    		--print("Human refused demand!");
    		
    		DoDemandState(false);
    		
    		DoClearTable();
    		ResetDisplay();
    		
    		if (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND) then
    			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL, g_iThem, 0, 0 );
    		elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
    			local iDealValue = 0;
				if (g_Deal) then
					iDealValue = g_pUs:GetDealMyValue(g_Deal);
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
    			--print("Human backing out of Concessions Offer.  Reseting DiploUIState to trade.");
    			
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
            Controls.UsPocketCitiesStack:SetHide( true );
            Controls.ThemPocketCitiesStack:SetHide( true );
            Controls.UsPocketOtherPlayerStack:SetHide( true );
            Controls.ThemPocketOtherPlayerStack:SetHide( true );
            Controls.UsPocketStrategicStack:SetHide( true );
            Controls.ThemPocketStrategicStack:SetHide( true );
            Controls.UsPocketLuxuryStack:SetHide( true );
            Controls.ThemPocketLuxuryStack:SetHide( true );
            Controls.UsPocketVoteStack:SetHide( true );
            Controls.ThemPocketVoteStack:SetHide( true );
            
            Controls.UsPocketPanel:SetScrollValue( 0 );
            Controls.UsTablePanel:SetScrollValue( 0 );
            Controls.ThemPocketPanel:SetScrollValue( 0 );
            Controls.ThemTablePanel:SetScrollValue( 0 );
		            
            -- hide unmet players
            if( g_bEnableThirdParty ) then
                g_iNumOthers = 0;
            	for iLoopPlayer = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
            		pLoopPlayer = Players[ iLoopPlayer ];
            		iLoopTeam = pLoopPlayer:GetTeam();
					
					--print("iLoopPlayer: " .. iLoopPlayer);
					
					if (pLoopPlayer:IsEverAlive()) then
						
        				if( g_iUs ~= iLoopPlayer and g_iThem ~= iLoopPlayer and
                			g_pUsTeam:IsHasMet( iLoopTeam ) and g_pThemTeam:IsHasMet( iLoopTeam ) and
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
    			g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
    				    		
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
--CBP
		Controls.DenounceButton:SetHide(true);
		if(not g_pUs:IsDenouncedPlayer(g_iThem) and not g_pUsTeam:IsAtWar( g_iThemTeam )) then
			Controls.DenounceButton:SetHide(false);
			Controls.DenounceButton:SetText(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN"));
			Controls.DenounceButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN_TT"));
		end
--END        
        Controls.MainStack:CalculateSize();
        Controls.MainGrid:DoAutoSize();

	-- Dealing with an AI
	elseif (g_bPVPTrade == false and g_bTradeReview == false) then
		
		-- Human is making a demand
        if( g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND ) then
            Controls.ProposeButton:SetText( Locale.ToUpper(Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEMAND_BUTTON" )));
            Controls.CancelButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_CANCEL" ));
            
    	-- If the AI made an offer change what buttons are visible for the human to respond with
    	elseif (g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
    		Controls.ProposeButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ACCEPT" ));
    		Controls.CancelButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_REFUSE" ));
    		
    	-- AI is making a demand or Request
    	elseif (	g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND or 
    				g_iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then
    				
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
--CBP
		Controls.DenounceButton:SetHide(true);
--END
		--CBP
		Controls.PeaceValue:SetHide(true);
		Controls.PeaceMax:SetHide(true);
		Controls.PeaceDeal:SetHide(true);
		Controls.PeaceDealBorderFrame:SetHide(true);
		if(Teams[g_iUsTeam]:IsAtWar(g_iThemTeam)) then
			if(g_Deal:GetSurrenderingPlayer() == g_iThem) then
				Controls.PeaceValue:SetHide(false);
				Controls.PeaceMax:SetHide(false);
				Controls.PeaceDeal:SetHide(false);
				Controls.PeaceDealBorderFrame:SetHide(false);
				local iMax = g_pThem:GetCachedValueOfPeaceWithHuman();
				local iCurrent = g_pThem:GetTotalValueToMe(g_Deal);
				local Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR", iCurrent);
				if(iCurrent == -1) then
					Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_IMPOSSIBLE");
				elseif(iCurrent == 0) then
					Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_ACCEPTABLE");
				end
				local Maxstr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_MAX_STR", iMax);
				if(iMax == -1) then
					Maxstr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_IMPOSSIBLE");
				end
				local ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_TT");
				local MaxstrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_MAX_STR_TT");
				Controls.PeaceValue:SetText(Valuestr);
				Controls.PeaceMax:SetText(Maxstr);
				Controls.PeaceValue:SetToolTipString(ValuestrTT);
				Controls.PeaceMax:SetToolTipString(MaxstrTT);
			end
		else
			Controls.PeaceValue:SetHide(false);
			Controls.PeaceDeal:SetHide(false);
			Controls.PeaceDealBorderFrame:SetHide(false);
			local iCurrent = g_pThem:GetTotalValueToMeNormal(g_Deal);
			local Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_PEACE", iCurrent);
			if(iCurrent == -99999) then
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_PEACE_EMBARGO");
			elseif(iCurrent == -1) then
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_PEACE_IMPOSSIBLE");
			elseif(iCurrent == 0) then
				Valuestr = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_PEACE_ACCEPTABLE");
			end
			local ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_PEACE_TT");
			if(iCurrent == -99999) then
				ValuestrTT = Locale.ConvertTextKey("TXT_KEY_DIPLO_TRADE_VALUE_STR_PEACE_EMBARGO_TT");
			end
			Controls.PeaceValue:SetText(Valuestr);
			Controls.PeaceValue:SetToolTipString(ValuestrTT);
		end
		--END

		-- At War: show the "what will end this war button
		if (g_iUsTeam >= 0 and g_iThemTeam >= 0 and Teams[g_iUsTeam]:IsAtWar(g_iThemTeam)) then
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
						g_iDiploUIState ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER) then
				
				-- Loop through deal items to see what the situation is
				local iNumItemsFromUs = 0;
				local iNumItemsFromThem = 0;
				
				local itemType;
				local duration;
				local finalTurn;
				local data1;
				local data2;
				local data3;
				local flag1;
				local fromPlayer;
			    
				g_Deal:ResetIterator();
				itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = g_Deal:GetNextItem();
			    
				if( itemType ~= nil ) then
				repeat
					local bFromUs = false;
			        
					if( fromPlayer == Game.GetActivePlayer() ) then
						bFromUs = true;
						iNumItemsFromUs = iNumItemsFromUs + 1;
					else
						iNumItemsFromThem = iNumItemsFromThem + 1;
					end
					
			        itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = g_Deal:GetNextItem();
			        
				until( itemType == nil )
				end
				
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
--CBP
			Controls.DenounceButton:SetHide(true);
--END
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
	
	--print("Clearing Table");
	
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

if (not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
	g_SubStacks[ tostring( Controls.UsPocketVote		) ] = Controls.UsPocketVoteStack;
	g_SubStacks[ tostring( Controls.ThemPocketVote		) ] = Controls.ThemPocketVoteStack;
	g_SubLabels[ tostring( Controls.UsPocketVote		) ] = Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" );
	g_SubLabels[ tostring( Controls.ThemPocketVote		) ] = Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" );

	Controls.UsPocketVote:SetVoid1( 1 );
end

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

if (not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
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
	
	--print("ResetDisplay");
	
	if g_iUs == -1 or g_iThem == -1 then
		return;
	end

	Controls.UsPocketStack:SetHide( false );
	Controls.ThemPocketStack:SetHide( false );
	Controls.UsPocketLeaderStack:SetHide( true );
	Controls.ThemPocketLeaderStack:SetHide( true );
	Controls.UsPocketCitiesStack:SetHide( true );
	Controls.ThemPocketCitiesStack:SetHide( true );
	
    Controls.UsPocketGold:SetHide( false );
    Controls.ThemPocketGold:SetHide( false );
    Controls.UsPocketGoldPerTurn:SetHide( false );
    Controls.ThemPocketGoldPerTurn:SetHide( false );
    Controls.UsPocketAllowEmbassy:SetHide( false );
    Controls.ThemPocketAllowEmbassy:SetHide( false );
    Controls.UsPocketOpenBorders:SetHide( false );
    Controls.ThemPocketOpenBorders:SetHide( false );
    Controls.UsPocketDefensivePact:SetHide( false );
    Controls.ThemPocketDefensivePact:SetHide( false );
    Controls.UsPocketResearchAgreement:SetHide( false );
    Controls.ThemPocketResearchAgreement:SetHide( false );
    --Controls.UsPocketTradeAgreement:SetHide( false );		Trade agreement disabled for now
    --Controls.ThemPocketTradeAgreement:SetHide( false );		Trade agreement disabled for now
    if (Controls.UsPocketDoF ~= nil) then
		Controls.UsPocketDoF:SetHide( false );
		Controls.UsPocketDoF:RegisterCallback( Mouse.eLClick, PocketDoFHandler );
		Controls.UsPocketDoF:SetVoid1( 1 );		
	end
    if (Controls.ThemPocketDoF ~= nil) then
		Controls.ThemPocketDoF:SetHide( false );
		Controls.ThemPocketDoF:RegisterCallback( Mouse.eLClick, PocketDoFHandler );
		Controls.ThemPocketDoF:SetVoid1( 0 );
	end

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

    	Controls.UsText:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", Locale.ConvertTextKey( g_pUs:GetNameKey() ) ) );

        if( m_bIsMulitplayer and pOtherPlayer:IsHuman() ) then
        	Controls.ThemText:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", Locale.ConvertTextKey( g_pThem:GetNickName() ) ) );
        else
        	Controls.ThemText:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", Locale.ConvertTextKey( g_pThem:GetName() ) ) );
    	end
    
    elseif( g_bPVPTrade == false ) then
	    -- ai mode
    	Controls.WhatDoYouWantButton:SetHide(true);
    	Controls.WhatWillYouGiveMeButton:SetHide(true);
    	Controls.WhatWillMakeThisWorkButton:SetHide(true);

			
    	local strString = Locale.ConvertTextKey("TXT_KEY_DIPLO_LEADER_SAYS", g_pThem:GetName());
    	Controls.NameText:SetText(strString);
	
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
--CBP
		Controls.DenounceButton:SetHide(true);
		if(not g_pUs:IsDenouncedPlayer(g_iThem) and not g_pUsTeam:IsAtWar( g_iThemTeam )) then
			Controls.DenounceButton:SetHide(false);
			Controls.DenounceButton:SetText(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN"));
			Controls.DenounceButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_CBP_DENOUNCE_HUMAN_TT"));
		end
--END    
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
    
    if (not bGoldTradeAllowed) then
	    Controls.UsPocketGold:SetDisabled(true);
	    Controls.UsPocketGold:GetTextControl():SetColorByName("Gray_Black");
--CBP
		if ( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
			Controls.UsPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif not (g_pUs:IsDoF(g_iThem)) then
			Controls.UsPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_DOF_TT_ONE_LINE"));
		else
			Controls.UsPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT_ONE_LINE"));
		end
-- END
	else
	    Controls.UsPocketGold:SetDisabled(false);
	    Controls.UsPocketGold:GetTextControl():SetColorByName("Beige_Black");
	    Controls.UsPocketGold:SetToolTipString(nil);	    
    end

	-- Them
    iGold = g_Deal:GetGoldAvailable(g_iThem, iItemToBeChanged);
    strGoldString = iGold .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD");
    Controls.ThemPocketGold:SetText( strGoldString );
    
    bGoldTradeAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD, 1);	-- 1 here is 1 Gold, which is the minimum possible
    
    if (not bGoldTradeAllowed) then
	    Controls.ThemPocketGold:SetDisabled(true);
	    Controls.ThemPocketGold:GetTextControl():SetColorByName("Gray_Black");
--CBP
		if ( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
			Controls.ThemPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		elseif not (g_pUs:IsDoF(g_iThem)) then
			Controls.ThemPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_DOF_TT_ONE_LINE"));
		else
			Controls.ThemPocketGold:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT_ONE_LINE"));
		end
-- END
	else
	    Controls.ThemPocketGold:SetDisabled(false);
	    Controls.ThemPocketGold:GetTextControl():SetColorByName("Beige_Black");
	    Controls.ThemPocketGold:SetToolTipString(nil);	    
    end
    
    ---------------------------------------------------------------------------------- 
    -- pocket Gold Per Turn  
    ---------------------------------------------------------------------------------- 

	-- Us
	local iGoldPerTurn = g_pUs:CalculateGoldRate();
    strGoldString = iGoldPerTurn .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN");
    Controls.UsPocketGoldPerTurn:SetText( strGoldString );

    local bGPTAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible
    
    if (not bGPTAllowed) then
	    Controls.UsPocketGoldPerTurn:SetDisabled(true);
	    Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Gray_Black");
--CBP
		if ( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
			Controls.UsPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		else
			Controls.UsPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT_ONE_LINE"));
		end
--END
	else
	    Controls.UsPocketGoldPerTurn:SetDisabled(false);
	    Controls.UsPocketGoldPerTurn:GetTextControl():SetColorByName("Beige_Black");
    end
    
    -- Them
	iGoldPerTurn = g_pThem:CalculateGoldRate();
    strGoldString = iGoldPerTurn .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN");
    Controls.ThemPocketGoldPerTurn:SetText( strGoldString );
    
    bGPTAllowed = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration);	-- 1 here is 1 GPT, which is the minimum possible
    
    if (not bGPTAllowed) then
	    Controls.ThemPocketGoldPerTurn:SetDisabled(true);
	    Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Gray_Black");
--CBP
		if ( g_pUsTeam:IsAtWar( g_iThemTeam ) ) then
			Controls.ThemPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_CLEAR_WINNER_TT"));
		else
			Controls.ThemPocketGoldPerTurn:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_BANK_TT_ONE_LINE"));
		end
--END
	else
	    Controls.ThemPocketGoldPerTurn:SetDisabled(false);
	    Controls.ThemPocketGoldPerTurn:GetTextControl():SetColorByName("Beige_Black");
    end

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
    
    ---------------------------------------------------------------------------------- 
    -- pocket Open Borders
    ---------------------------------------------------------------------------------- 
    
    local bOpenBordersAllowed = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration);
    
    -- Are we not allowed to give Open Borders? (don't have tech)
    strTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_OPEN_BORDERS_TT", g_iDealDuration );
    if (not bOpenBordersAllowed) then
		if (not g_pUsTeam:IsOpenBordersTradingAllowed() and not g_pThemTeam:IsOpenBordersTradingAllowed()) then
			strTooltip = strTooltip .. " [COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_OPEN_BORDERS_NO_TECH" ) .. "[ENDCOLOR]";
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
		elseif (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam)) then
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

		if (g_pThemTeam:IsAllowsOpenBordersToTeam(g_iUsTeam)) then
			strTheirTooltip = strTheirTooltip .. " " .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_OPEN_BORDERS_THEY_HAVE" );
		elseif (not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
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
		elseif (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) or not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
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
			if (not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) or not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam)) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT" ) ;
				bEmptyTT = false;
			end
			if (not g_pUs:IsDoF(g_iThem) or not g_pThem:IsDoF(g_iUs)) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEED_DOF_TT" ) ;
				bEmptyTT = false;
			end
			if (Players[g_iUs]:GetGold() < iCost or Players[g_iThem]:GetGold() < iCost or bEmptyTT) then
				strDisabledTT = strDisabledTT .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_AGREEMENT" ) ;
			end
		end
		
		strDisabledTT = "[COLOR_WARNING_TEXT]" .. strDisabledTT .. "[ENDCOLOR]";
		strTooltip = strTooltip .. strDisabledTT;
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

	if ( Controls.UsPocketDoF ~= nil and Controls.ThemPocketDoF ~= nil) then
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
	
    ---------------------------------------------------------------------------------- 
    -- Pocket Cities
    ---------------------------------------------------------------------------------- 
    local bFound = false;
    for pCity in g_pUs:Cities() do
        if( g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_CITIES, pCity:GetX(), pCity:GetY() ) ) then
            bFound = true;
            break;
		end	
    end
    if( bFound ) then
        Controls.UsPocketCities:SetDisabled( false );
        Controls.UsPocketCities:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_TO_TRADE_CITY_TT" ));
		Controls.UsPocketCities:GetTextControl():SetColorByName("Beige_Black");
    else
        Controls.UsPocketCities:SetDisabled( true );
        Controls.UsPocketCities:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT" ));
		Controls.UsPocketCities:GetTextControl():SetColorByName("Gray_Black");
    end
    
    
    bFound = false;
    for pCity in g_pThem:Cities() do
        if( g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_CITIES, pCity:GetX(), pCity:GetY() ) ) then
            bFound = true;
            break;
        end
    end
    if( bFound ) then
        Controls.ThemPocketCities:SetDisabled( false );
        Controls.ThemPocketCities:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_TO_TRADE_CITY_TT" ));
		Controls.ThemPocketCities:GetTextControl():SetColorByName("Beige_Black");
    else
        Controls.ThemPocketCities:SetDisabled( true );
        Controls.ThemPocketCities:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM" ));
		Controls.ThemPocketCities:GetTextControl():SetColorByName("Gray_Black");
    end
      

    ---------------------------------------------------------------------------------- 
    -- enable/disable pocket other civs
    ---------------------------------------------------------------------------------- 
    if( g_iNumOthers == 0 ) then
    
		Controls.UsPocketOtherPlayer:SetDisabled( true );
		Controls.UsPocketOtherPlayer:GetTextControl():SetColorByName( "Gray_Black" );
		Controls.UsPocketOtherPlayer:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS") );
		Controls.UsPocketOtherPlayer:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_OTHER_PLAYERS" ) );
		
		Controls.ThemPocketOtherPlayer:SetDisabled( true );
		Controls.ThemPocketOtherPlayer:GetTextControl():SetColorByName( "Gray_Black" );
		Controls.ThemPocketOtherPlayer:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM" ));
		Controls.ThemPocketOtherPlayer:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_OTHER_PLAYERS" ) );
		
	else
		Controls.UsPocketOtherPlayer:SetDisabled( false );
		Controls.UsPocketOtherPlayer:GetTextControl():SetColorByName( "Beige_Black" );
		Controls.UsPocketOtherPlayer:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_OTHER_PLAYERS_OPEN" ) );
		Controls.UsPocketOtherPlayer:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_OTHER_PLAYERS" ) );
		
		Controls.ThemPocketOtherPlayer:SetDisabled( false );
		Controls.ThemPocketOtherPlayer:GetTextControl():SetColorByName( "Beige_Black" );
		Controls.ThemPocketOtherPlayer:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_OTHER_PLAYERS_OPEN" ));
		Controls.ThemPocketOtherPlayer:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_OTHER_PLAYERS" ) );
    end
    	    
    
		
    ---------------------------------------------------------------------------------- 
    -- pocket resources for us
    ---------------------------------------------------------------------------------- 
    local bFoundLux = false;
    local bFoundStrat = false;
    local count;
    
    local iResourceCount;
    --local iOurResourceCount;
    --local iTheirResourceCount;
    local pResource;
    local strString;
    
    -- loop over resources
    if( g_iUs == -1 ) then
        for resType, instance in pairs( g_UsPocketResources ) do
            instance.Button:SetHide( false );
        end
        bFoundLux = true;
        bFoundStrat = true;
    else
		
		local bCanTradeResource;
		
        for resType, instance in pairs( g_UsPocketResources ) do
			
			bCanTradeResource = g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESOURCES, resType, 1);	-- 1 here is 1 quanity of the Resource, which is the minimum possible
			
            if (bCanTradeResource) then
                if( g_LuxuryList[ resType ] == true ) then
                    bFoundLux = true;
                else
                    bFoundStrat = true;
                end
                instance.Button:SetHide( false );
                
                pResource = GameInfo.Resources[resType];
				iResourceCount = g_Deal:GetNumResource(g_iUs, resType);
			    strString = pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description) .. " (" .. iResourceCount .. ")";
                instance.Button:SetText( strString );
            else
                instance.Button:SetHide( true );
            end
        end
    end
    Controls.UsPocketLuxury:SetDisabled( not bFoundLux );
    if (bFoundLux) then
		Controls.UsPocketLuxury:GetTextControl():SetColorByName("Beige_Black");
		Controls.UsPocketLuxury:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES" ));
		if( Controls.UsPocketLuxuryStack:IsHidden() ) then
    		Controls.UsPocketLuxury:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES" ) );
		else
    		Controls.UsPocketLuxury:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES" ) );
		end
	else
		Controls.UsPocketLuxury:GetTextControl():SetColorByName("Gray_Black");
		Controls.UsPocketLuxury:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO" ));
		Controls.UsPocketLuxury:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES" ) );
    end
    
    Controls.UsPocketStrategic:SetDisabled( not bFoundStrat );
    if (bFoundStrat) then
		Controls.UsPocketStrategic:GetTextControl():SetColorByName("Beige_Black");
		Controls.UsPocketStrategic:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES") );
		if( Controls.UsPocketStrategicStack:IsHidden() ) then
    		Controls.UsPocketStrategic:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES" ) );
		else
    		Controls.UsPocketStrategic:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES" ) );
		end
	else
		Controls.UsPocketStrategic:GetTextControl():SetColorByName("Gray_Black");
		Controls.UsPocketStrategic:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO") );
		Controls.UsPocketStrategic:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES" ) );
    end
   
   
    ---------------------------------------------------------------------------------- 
    -- pocket resources for them
    ---------------------------------------------------------------------------------- 
    bFoundLux = false;
    bFoundStrat = false;
    if( g_iThem == -1 ) then
        for resType, instance in pairs( g_ThemPocketResources ) do
            instance.Button:SetHide( false );
        end
        bFoundLux = true;
        bFoundStrat = true;
    else
		
		local bCanTradeResource;
		
        for resType, instance in pairs( g_ThemPocketResources ) do
			
			bCanTradeResource = g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESOURCES, resType, 1);	-- 1 here is 1 quanity of the Resource, which is the minimum possible
			
            if (bCanTradeResource) then
                if( g_LuxuryList[ resType ] == true ) then
                    bFoundLux = true;
                else
                    bFoundStrat = true;
                end
                instance.Button:SetHide( false );
                
                pResource = GameInfo.Resources[resType];
                iResourceCount = g_Deal:GetNumResource(g_iThem, resType);
			    strString = pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description) .. " (" .. iResourceCount .. ")";
                instance.Button:SetText( strString );
            else
                instance.Button:SetHide( true );
            end
        end
    end
    Controls.ThemPocketLuxury:SetDisabled( not bFoundLux );
    if (bFoundLux) then
		Controls.ThemPocketLuxury:GetTextControl():SetColorByName("Beige_Black");
		Controls.ThemPocketLuxury:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES_THEM" ));
		if( Controls.ThemPocketLuxuryStack:IsHidden() ) then
    		Controls.ThemPocketLuxury:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES" ) );
		else
    		Controls.ThemPocketLuxury:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES" ) );
		end
	else
		Controls.ThemPocketLuxury:GetTextControl():SetColorByName("Gray_Black");
		Controls.ThemPocketLuxury:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO_THEM" ));
		Controls.ThemPocketLuxury:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_LUXURY_RESOURCES" ) );
    end
    
    Controls.ThemPocketStrategic:SetDisabled( not bFoundStrat );
    if (bFoundStrat) then
		Controls.ThemPocketStrategic:GetTextControl():SetColorByName("Beige_Black");
		Controls.ThemPocketStrategic:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES_THEM" ));
		if( Controls.ThemPocketStrategicStack:IsHidden() ) then
    		Controls.ThemPocketStrategic:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES" ) );
		else
    		Controls.ThemPocketStrategic:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES" ) );
		end
	else
		Controls.ThemPocketStrategic:GetTextControl():SetColorByName("Gray_Black");
		Controls.ThemPocketStrategic:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO_THEM" ));
		Controls.ThemPocketStrategic:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_STRATEGIC_RESOURCES" ) );
    end
   
   
    ---------------------------------------------------------------------------------- 
    -- Votes
    ----------------------------------------------------------------------------------
    if (not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
		Controls.UsPocketVote:SetHide(false);
		Controls.ThemPocketVote:SetHide(false);
		
		RefreshPocketVotes(1);
		local bCanSellVote = g_pUs:CanCommitVote(g_pThem:GetID());
		local sTooltipDetails = g_pUs:GetCommitVoteDetails(g_pThem:GetID());
		Controls.UsPocketVote:SetDisabled( not bCanSellVote );
		if (bCanSellVote) then
			Controls.UsPocketVote:GetTextControl():SetColorByName("Beige_Black");
			Controls.UsPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_YES" ) .. sTooltipDetails);
			if ( Controls.UsPocketVoteStack:IsHidden() ) then
				Controls.UsPocketVote:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" ));
			else
				Controls.UsPocketVote:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" ));
			end
		else
			Controls.UsPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.UsPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_NO" ) .. sTooltipDetails);
			Controls.UsPocketVote:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" ));
		end
		
		RefreshPocketVotes(0);
		local bCanBuyVote = g_pThem:CanCommitVote(g_pUs:GetID());
		local sTooltipDetails = g_pThem:GetCommitVoteDetails(g_pUs:GetID());
		Controls.ThemPocketVote:SetDisabled( not bCanBuyVote );
		if (bCanBuyVote) then
			Controls.ThemPocketVote:GetTextControl():SetColorByName("Beige_Black");
			Controls.ThemPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_YES_THEM" ) .. sTooltipDetails);
			if ( Controls.ThemPocketVoteStack:IsHidden() ) then
				Controls.ThemPocketVote:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" ));
			else
				Controls.ThemPocketVote:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" ));
			end
		else
			Controls.ThemPocketVote:GetTextControl():SetColorByName("Gray_Black");
			Controls.ThemPocketVote:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_VOTE_TRADE_NO_THEM" ) .. sTooltipDetails);
			Controls.ThemPocketVote:SetText( Locale.ConvertTextKey( "TXT_KEY_TRADE_ITEM_VOTES" ));
		end
   
    else
		Controls.UsPocketVote:SetHide(true);
		Controls.ThemPocketVote:SetHide(true);
    end 
	
   
	-- Hide things inappropriate for teammates
	if (g_iUsTeam == g_iThemTeam) then
		--print("Teams match!");
		Controls.UsPocketAllowEmbassy:SetHide(true);
		Controls.UsPocketOpenBorders:SetHide(true);
		Controls.UsPocketDefensivePact:SetHide(true);
		Controls.UsPocketResearchAgreement:SetHide(true);
		Controls.UsPocketOtherPlayer:SetHide(true);
		Controls.ThemPocketAllowEmbassy:SetHide(true);
		Controls.ThemPocketOpenBorders:SetHide(true);
		Controls.ThemPocketDefensivePact:SetHide(true);
		Controls.ThemPocketResearchAgreement:SetHide(true);
		Controls.ThemPocketOtherPlayer:SetHide(true);
	else
		--print("Teams DO NOT match!");
		Controls.UsPocketAllowEmbassy:SetHide(false);
		Controls.UsPocketOpenBorders:SetHide(false);
		Controls.UsPocketDefensivePact:SetHide(false);
		Controls.UsPocketResearchAgreement:SetHide(false);
		Controls.UsPocketOtherPlayer:SetHide(false);
		Controls.ThemPocketAllowEmbassy:SetHide(false);
		Controls.ThemPocketOpenBorders:SetHide(false);
		Controls.ThemPocketDefensivePact:SetHide(false);
		Controls.ThemPocketResearchAgreement:SetHide(false);
		Controls.ThemPocketOtherPlayer:SetHide(false);
	end

    ResizeStacks();
end


----------------------------------------------------------------        
----------------------------------------------------------------        
function DoClearTable()
    g_UsTableCitiesIM:ResetInstances();
    g_ThemTableCitiesIM:ResetInstances();
    g_UsTableVoteIM:ResetInstances();
    g_ThemTableVoteIM:ResetInstances();
    
	Controls.UsTablePeaceTreaty:SetHide( true );
	Controls.ThemTablePeaceTreaty:SetHide( true );
	Controls.UsTableGold:SetHide( true );
	Controls.ThemTableGold:SetHide( true );
	Controls.UsTableGoldPerTurn:SetHide( true );
	Controls.ThemTableGoldPerTurn:SetHide( true );
	Controls.UsTableAllowEmbassy:SetHide( true );
	Controls.ThemTableAllowEmbassy:SetHide( true );
	Controls.UsTableOpenBorders:SetHide( true );
	Controls.ThemTableOpenBorders:SetHide( true );
	Controls.UsTableDefensivePact:SetHide( true );
	Controls.ThemTableDefensivePact:SetHide( true );
	Controls.UsTableResearchAgreement:SetHide( true );
	Controls.ThemTableResearchAgreement:SetHide( true );
	Controls.UsTableTradeAgreement:SetHide( true );
	Controls.ThemTableTradeAgreement:SetHide( true );
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

    Controls.UsPocketLuxuryStack:CalculateSize();
    Controls.ThemPocketLuxuryStack:CalculateSize();
    Controls.UsTableLuxuryStack:CalculateSize();
    Controls.ThemTableLuxuryStack:CalculateSize();
    
    Controls.UsPocketVoteStack:CalculateSize();
    Controls.ThemPocketVoteStack:CalculateSize();
    Controls.UsTableVoteStack:CalculateSize();
    Controls.ThemTableVoteStack:CalculateSize();
    
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
    g_UsTableVoteIM:ResetInstances();
    g_ThemTableVoteIM:ResetInstances();
	
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
    itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = g_Deal:GetNextItem();
    
    local iItemToBeChanged = -1;	-- This is -1 because we're not changing anything right now
   
	--print("Going through check"); 
    if( itemType ~= nil ) then
    repeat
        local bFromUs = false;
        
		--print("Adding trade item to active deal: " .. itemType);
        
        if( fromPlayer == Game.GetActivePlayer() ) then
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
        
        elseif( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY == itemType ) then
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
        elseif( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP == itemType ) then
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
        elseif( TradeableItems.TRADE_ITEM_VOTE_COMMITMENT == itemType ) then
			print("==debug== VOTE_COMMITMENT found in DisplayDeal");
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
        itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = g_Deal:GetNextItem();
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
	for i = 0, GameDefines.MAX_CIV_PLAYERS - 1 do
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

	local iGoldPerTurn = 5;
	
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

        g_Deal:ChangeGoldPerTurnTrade( g_iUs, iGoldPerTurn, g_iDealDuration );
		
		local strTooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GPT", g_pUs:CalculateGoldRate() - iGoldPerTurn );
		Controls.UsTableGoldPerTurn:SetToolTipString( strTooltip );
		
    -- GPT from them
    else
		
		if (iGoldPerTurn > g_pThem:CalculateGoldRate()) then
			iGoldPerTurn = g_pThem:CalculateGoldRate();
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

-----------------------------------------------------------------------------------------------------------------------
-- Allow Embassy Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketAllowEmbassyHandler( isUs )
	print("PocketAllowEmbassyHandler");
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
	print("TableAllowEmbassyHandler");
	if( isUs == 1 ) then
		print( "remove: us " .. g_iUs );
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iUs );
	else
		print( "remove: them " .. g_iThem );
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

-----------------------------------------------------------------------------------------------------------------------
-- Declaration of Friendship Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketDoFHandler( isUs )
	print("PocketDoFHandler");
	-- The Declaration of Friendship must be on both sides of the deal
	g_Deal:AddDeclarationOfFriendship(g_iUs);
	g_Deal:AddDeclarationOfFriendship(g_iThem);
	
	DisplayDeal();
	DoUIDealChangedByHuman();
end

----------------------------------------------------
function TableDoFHandler( isUs )
	print("TableDoFHandler");
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
        print( "remove: us " .. g_iUs );
        g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iUs );
    else
        print( "remove: them " .. g_iThem );
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
-- Trade Agreement Handlers
-----------------------------------------------------------------------------------------------------------------------
function PocketTradeAgreementHandler( isUs )

	-- Note that currently Trade Agreement is required on both sides
	
    if( isUs == 1 ) then
        g_Deal:AddTradeAgreement( g_iUs, g_iDealDuration );
        g_Deal:AddTradeAgreement( g_iThem, g_iDealDuration );
    else
        g_Deal:AddTradeAgreement( g_iUs, g_iDealDuration );
        g_Deal:AddTradeAgreement( g_iThem, g_iDealDuration );
    end
    DisplayDeal();
    DoUIDealChangedByHuman();
end
Controls.UsPocketTradeAgreement:RegisterCallback( Mouse.eLClick, PocketTradeAgreementHandler );
Controls.ThemPocketTradeAgreement:RegisterCallback( Mouse.eLClick, PocketTradeAgreementHandler );
Controls.UsPocketTradeAgreement:SetVoid1( 1 );
Controls.ThemPocketTradeAgreement:SetVoid1( 0 );

function TableTradeAgreementHandler()
	-- Remove from BOTH sides of the table
    g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iUs );
    g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iThem );
    DoClearTable();
    DisplayDeal();
    DoUIDealChangedByHuman();
end
Controls.UsTableTradeAgreement:RegisterCallback( Mouse.eLClick, TableTradeAgreementHandler );
Controls.ThemTableTradeAgreement:RegisterCallback( Mouse.eLClick, TableTradeAgreementHandler );





-----------------------------------------------------------------------------------------------------------------------
-- Handle the strategic and luxury resources
-----------------------------------------------------------------------------------------------------------------------
function PocketResourceHandler( isUs, resourceId )
	
	local iAmount = 3;

	if ( GameInfo.Resources[ resourceId ].ResourceUsage == 2 ) then -- is a luxury resource
		iAmount = 1;
	end
	
    if( isUs == 1 ) then
		if (iAmount > g_Deal:GetNumResource(g_iUs, resourceId)) then
			iAmount = g_Deal:GetNumResource(g_iUs, resourceId);
		end
        g_Deal:AddResourceTrade( g_iUs, resourceId, iAmount, g_iDealDuration );
    else
		if (iAmount > g_Deal:GetNumResource(g_iThem, resourceId)) then
			iAmount = g_Deal:GetNumResource(g_iThem, resourceId);
		end
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
    else
        control:SetText( 0 );
    end
    
    -- Can't offer more than someone has
    if (iNumResource > g_Deal:GetNumResource(iPlayer, iResourceID)) then
		iNumResource = g_Deal:GetNumResource(iPlayer, iResourceID);
		control:SetText(iNumResource);
	end
    
    if ( bIsUs ) then
        g_Deal:ChangeResourceTrade( g_iUs, iResourceID, iNumResource, g_iDealDuration );
    else
        g_Deal:ChangeResourceTrade( g_iThem, iResourceID, iNumResource, g_iDealDuration );
    end
end


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
		print("==debug== Vote added to deal, ID=" .. iResolutionID .. ", VoteChoice=" .. iVoteChoice .. ", NumVotes=" .. iNumChooseVotes);
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
-- CBP EDITS BELOW
				local iNumUsVotes = pLeague:GetPotentialVotesForMember(g_iThem, g_iUs);
-- END		
				local sProposalText = GetVoteText(pLeague, i, tVote.Repeal, iNumUsVotes);
				local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
				local sTooltip = GetVoteTooltip(pLeague, i, tVote.Repeal, iNumUsVotes);
				if (g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, tVote.ID, tVote.VoteChoice, iNumUsVotes, tVote.Repeal)) then
					local cInstance = g_UsPocketVoteIM:GetInstance();
					cInstance.ProposalLabel:SetText(sProposalText);
					cInstance.VoteLabel:SetText(sChoiceText);
					cInstance.Button:SetToolTipString(sTooltip);
					cInstance.Button:SetVoids(g_iUs, i);
					cInstance.Button:RegisterCallback( Mouse.eLClick, OnChoosePocketVote );
				end
			-- Them to us?
			else
-- CBP EDIT
				local iNumThemVotes = pLeague:GetPotentialVotesForMember(g_iUs, g_iThem);
-- END	
				local sProposalText = GetVoteText(pLeague, i, tVote.Repeal, iNumThemVotes);
				local sChoiceText = pLeague:GetTextForChoice(tVote.VoteDecision, tVote.VoteChoice);
				local sTooltip = GetVoteTooltip(pLeague, i, tVote.Repeal, iNumThemVotes);
				if (g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, tVote.ID, tVote.VoteChoice, iNumThemVotes, tVote.Repeal)) then
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
		print("==debug== Vote removed from deal, ID=" .. iResolutionID .. ", VoteChoice=" .. iVoteChoice .. ", NumVotes=" .. iNumTableVotes);
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
            
            instance.CityName:SetText( pCity:GetName() );
            instance.CityPop:SetText( pCity:GetPopulation() );
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
function ShowOtherPlayerChooser( isUs, type )

	--print( "ShowOtherPlayerChooser" );
	local SubTableName;
	local iFromPlayer;
	local tradeType;
	local iToPlayer;

	if( type == WAR ) then
		tradeType = TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR;
	elseif( type == PEACE ) then
		tradeType = TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE;
	end

	local pocketStack;
	local leaderStack;
	local panel;

	-- disable invalid players
	if( isUs == 1 ) then
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

	for iLoopPlayer = 0, GameDefines.MAX_CIV_PLAYERS-1, 1 do
		pLoopPlayer = Players[ iLoopPlayer ];
		iLoopTeam = pLoopPlayer:GetTeam();

		local otherPlayerButton = g_OtherPlayersButtons[iLoopPlayer];
		
	    -- they're alive, not us, not them, we both know, and we can trade....
		if(otherPlayerButton ~= nil) then

			local otherPlayerButtonSubTableNameButton = otherPlayerButton[SubTableName].Button;
			
			local strToolTip = "";
			local iFromTeam = Players[iFromPlayer]:GetTeam();
			local iToTeam = Players[iToPlayer]:GetTeam();
			-- NO Met
			if (not Teams[iFromTeam]:IsHasMet(iLoopTeam) or not Teams[iToTeam]:IsHasMet(iLoopTeam)) then
				otherPlayerButtonSubTableNameButton:SetHide(true);
			end
			if(iFromTeam == iLoopTeam or iToTeam == iLoopTeam)then
				otherPlayerButtonSubTableNameButton:SetHide(true);
			end

    		if( g_iUsTeam ~= iLoopTeam and g_iThemTeam ~= iLoopTeam and
                g_Deal:IsPossibleToTradeItem( iFromPlayer, iToPlayer, tradeType, iLoopTeam ) ) then
                
    		    otherPlayerButtonSubTableNameButton:SetDisabled( false );
    		    otherPlayerButtonSubTableNameButton:SetAlpha( 1 );
            else                     
    		    otherPlayerButtonSubTableNameButton:SetDisabled( true );
    		    otherPlayerButtonSubTableNameButton:SetAlpha( 0.5 );
    		    
    			-- Why won't they make peace?
    			if (type == PEACE) then

					if (not Teams[iToTeam]:HasEmbassyAtTeam(iFromTeam)) then
						if(strToolTip ~= "") then
							strToolTip = strToolTip .. "[NEWLINE]";
						end
						strToolTip = strToolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEED_EMBASSY" );
					end
					if (not pLoopPlayer:IsMinorCiv()) then
						-- CBP: No peace with non allied if not at war
						if (g_pUsTeam:IsAtWar(g_iThemTeam)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
							strToolTip = strToolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_WAR_NO_PEACE_THIRD_PARTY_TT" );
						end
						-- Not at war
						if (not Teams[iLoopTeam]:IsAtWar(iFromTeam)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
							strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_AT_WAR");
    					else
							-- Can't force third party peace with a loser. Has to be a sizeable difference
							local iFromWarScore = Players[iFromPlayer]:GetWarScore(iLoopPlayer);

							if(iFromWarScore < 75) then
								if(strToolTip ~= "") then
									strToolTip = strToolTip .. "[NEWLINE]";
								end
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_WAR_SCORE", iFromWarScore);
							end
						end

						if (not Teams[iLoopTeam]:CanChangeWarPeace(iFromTeam)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
							strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_CANNOT_PEACE");
    					end
					-- Minor that won't make peace    				
					else	
    					local pMinorTeam = Teams[iLoopTeam];
    					local iAlly = pLoopPlayer:GetAlly();
    					
    					-- Minor in permanent war with this guy
    					if (pLoopPlayer:IsMinorPermanentWar(iFromPlayer)) then
 							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
    						strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_PERMANENT_WAR");
						end
			    		-- Minor allied to a player
	    				if (pMinorTeam:IsAtWar(iFromTeam) and iAlly ~= -1 and Teams[Players[iAlly]:GetTeam()]:IsAtWar(iFromTeam)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
			    			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_ALLY_AT_WAR");
    					end
					end
    			-- Why won't they make war?
    			else
					-- CBP: No peace with non allied if not at war
					if (not g_pUsTeam:IsAtWar(g_iThemTeam)) then
						-- Already at war
						if (Teams[iLoopTeam]:IsAtWar(iFromTeam)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
							strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ALREADY_AT_WAR");
						end
						-- NO WAR
						if (not Teams[iFromTeam]:CanDeclareWar(iLoopTeam, iFromPlayer)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end							
							strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_WAR_BLOCKED");
						end
						-- CBP: Need Embassy:
						if (not Teams[iToTeam]:HasEmbassyAtTeam(iFromTeam)) then
							if(strToolTip ~= "") then
								strToolTip = strToolTip .. "[NEWLINE]";
							end
							strToolTip = strToolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEED_EMBASSY" ) ;
						end		
						-- City-State ally
						if (pLoopPlayer:IsMinorCiv()) then
							if (pLoopPlayer:GetAlly() == iToPlayer or pLoopPlayer:GetAlly() == iFromPlayer) then
								if(strToolTip ~= "") then
									strToolTip = strToolTip .. "[NEWLINE]";
								end
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_WAR_ALLIES");
							end
						else
							-- DOF
							if (pLoopPlayer:IsDoF(iFromPlayer) or pLoopPlayer:IsDoF(iToPlayer)) then
								if(strToolTip ~= "") then
									strToolTip = strToolTip .. "[NEWLINE]";
								end
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_WAR_DOF");
							end
							-- DP
							if (Teams[iLoopTeam]:IsDefensivePact(iFromTeam) or Teams[iLoopTeam]:IsDefensivePact(iToTeam)) then
								if(strToolTip ~= "") then
									strToolTip = strToolTip .. "[NEWLINE]";
								end
								strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_WAR_DP");
							end
						end
					end
  				
    			end
			end
				
			-- Tooltip
			otherPlayerButtonSubTableNameButton:SetToolTipString(strToolTip);
		end
	end

	pocketStack:SetHide( true );
	pocketStack:CalculateSize();
	leaderStack:SetHide( false );
	leaderStack:CalculateSize();

	panel:CalculateInternalSize();
	panel:SetScrollValue( 0 );
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
    	g_Deal:AddThirdPartyPeace( iWho, iOtherTeam, GameDefines.PEACE_TREATY_LENGTH );
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

Controls.UsMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", 15 );
Controls.UsDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", 15 );
Controls.ThemMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", 15 );
Controls.ThemDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", 15 );

ResetDisplay();
DisplayDeal();

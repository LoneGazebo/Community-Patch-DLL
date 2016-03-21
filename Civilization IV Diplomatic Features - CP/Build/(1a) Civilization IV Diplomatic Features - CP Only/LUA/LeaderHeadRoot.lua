----------------------------------------------------------------        
----------------------------------------------------------------        

include( "IconSupport" );
include( "GameplayUtilities" );
include( "InfoTooltipInclude" );

local g_iAIPlayer = -1;
local g_iAITeam = -1;

local g_DiploUIState = -1;

local g_iRootMode = 0;
local g_iTradeMode = 1;
local g_iDiscussionMode = 2;

local g_strLeaveScreenText = Locale.ConvertTextKey("TXT_KEY_DIPLOMACY_ANYTHING_ELSE");

local offsetOfString = 32;
local bonusPadding = 16
local innerFrameWidth = 654;
local outerFrameWidth = 650;
local offsetsBetweenFrames = 4;

----------------------------------------------------------------        
-- LEADER MESSAGE HANDLER
----------------------------------------------------------------        
function LeaderMessageHandler( iPlayer, iDiploUIState, szLeaderMessage, iAnimationAction, iData1 )
	
	g_DiploUIState = iDiploUIState;
	
	g_iAIPlayer = iPlayer;
	g_iAITeam = Players[g_iAIPlayer]:GetTeam();
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[pActivePlayer:GetTeam()];
	
	CivIconHookup( iPlayer, 64, Controls.ThemSymbolShadow, Controls.CivIconBG, Controls.CivIconShadow, false, true );
		
	-- Update title even if we're not in this mode, as we could exit to it somehow
	local player = Players[iPlayer];
	local strTitleText = GameplayUtilities.GetLocalizedLeaderTitle(player);
	Controls.TitleText:SetText(strTitleText);
	
	playerLeaderInfo = GameInfo.Leaders[player:GetLeaderType()];
	
	-- Mood
	local iApproach = pActivePlayer:GetApproachTowardsUsGuess(g_iAIPlayer);
	local strMoodText = Locale.ConvertTextKey("TXT_KEY_EMOTIONLESS");
	
	if (pActiveTeam:IsAtWar(g_iAITeam)) then
		strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" );
	elseif (Players[g_iAIPlayer]:IsDenouncingPlayer(Game.GetActivePlayer())) then
		strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_DENOUNCING" );	
	elseif (Players[g_iAIPlayer]:WasResurrectedThisTurnBy(iActivePlayer)) then
		strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_LIBERATED" );		
	else
		if( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_WAR_CAPS" );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_HOSTILE", playerLeaderInfo.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_GUARDED", playerLeaderInfo.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_AFRAID", playerLeaderInfo.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_FRIENDLY", playerLeaderInfo.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_NEUTRAL", playerLeaderInfo.Description );
		end
	end
	
	Controls.MoodText:SetText(strMoodText);
	
	local strMoodInfo = GetMoodInfo(g_iAIPlayer);
	Controls.MoodText:SetToolTipString(strMoodInfo);
	
	local bMyMode = false;
	
	-- See if we're in this screen
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_PEACE_MADE_BY_HUMAN) then
		bMyMode = true;
	end
	
	print("Handling LeaderMessage: " .. iDiploUIState .. ", ".. szLeaderMessage);
	
	-- Are we in this screen's mode?
	if (bMyMode) then					
		UI.SetLeaderHeadRootUp( true );
		Controls.LeaderSpeech:SetText( szLeaderMessage );		
		-- Resize the height of the box to fit the text
		local contentSize = Controls.LeaderSpeech:GetSize().y + offsetOfString + bonusPadding;
		Controls.LeaderSpeechBorderFrame:SetSizeY( contentSize );
		Controls.LeaderSpeechFrame:SetSizeY( contentSize - offsetsBetweenFrames );
		
	else
		Controls.LeaderSpeech:SetText( g_strLeaveScreenText );		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
		
	end
	
	UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderHead );
    
end
Events.AILeaderMessage.Add( LeaderMessageHandler );


----------------------------------------------------------------        
-- BACK
----------------------------------------------------------------        
function OnReturn()
	--UI.SetNextGameState( GameStates.MainGameView, g_iAIPlayer );
    UIManager:DequeuePopup( ContextPtr );
	UI.SetLeaderHeadRootUp( false );
	UI.RequestLeaveLeader();
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnReturn );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnLeavingLeader()
    -- we shouldn't be able to leave without this already being set to false, 
    -- but just in case...
	UI.SetLeaderHeadRootUp( false );
		
    UIManager:DequeuePopup( ContextPtr );
end
Events.LeavingLeaderViewMode.Add( OnLeavingLeader );


local oldCursor = 0;

----------------------------------------------------------------        
-- SHOW/HIDE
----------------------------------------------------------------        
function OnShowHide( bHide )
	
	-- Showing Screen
	if (not bHide) then
	
		Controls.RootOptions:SetHide( not UI.GetLeaderHeadRootUp() );
		
		local pActiveTeam = Teams[Game.GetActiveTeam()];
		-- Putmalk
		local pActivePlayer = Teams[Game.GetActivePlayer()];
		local pDiscussingPlayer = Players[g_iAIPlayer];
		
		-- Hide or show war/peace button
		if (not pActiveTeam:CanChangeWarPeace(g_iAITeam)) then
			Controls.WarButton:SetHide(true);
		else
			Controls.WarButton:SetHide(false);
		end
		
		-- Hide or show the demand button
		if (Game.GetActiveTeam() == g_iAITeam) then
			Controls.DemandButton:SetHide(true);
		else
			Controls.DemandButton:SetHide(false);
		end

		-- Start Putmalk
		-- What text to display on "Demand Button"?
		local strString = "";
		
		if(pDiscussingPlayer ~= nil) then
			local bFriends = pDiscussingPlayer:IsDoF(pActivePlayer);
			if(bFriends) then
				strString = "TXT_KEY_DIPLO_REQUEST_HELP_BUTTON";
			else
				strString = "TXT_KEY_DIPLO_DEMAND_BUTTON";
			end
		else
			print("Could not find discussing player!");
		end

		if(strString == nil) then
			print("strString == nil!");
			str = "TXT_KEY_DIPLO_REQUEST_HELP_BUTTON";
		end

		Controls.DemandButton:SetText( Locale.ConvertTextKey( strString ));
		-- End Putmalk

		oldCursor = UIManager:SetUICursor(0); -- make sure we start with the default cursor
		
	    if (g_iAITeam ~= -1) then
			local bAtWar = pActiveTeam:IsAtWar(g_iAITeam);
			
			if (bAtWar) then
				Controls.WarButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE" ));
				Controls.TradeButton:SetDisabled(true);
				Controls.DemandButton:SetDisabled(true);
				Controls.DiscussButton:SetDisabled(true);
				
				local iLockedWarTurns = pActiveTeam:GetNumTurnsLockedIntoWar(g_iAITeam);
				
				-- Putmalk: redid logic to accomodate vassals
				-- Locked into war by vassals
				if (pActiveTeam:IsVassalLockedIntoWar(g_iAITeam)) then
					Controls.WarButton:SetDisabled(false);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_VASSAL_BLOCKED_TT" ));
				-- Locked into war
				elseif (iLockedWarTurns ~= 0) then
					Controls.WarButton:SetDisabled(true);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_BLOCKED_TT", iLockedWarTurns ));
				-- Not locked into war
				else
					Controls.WarButton:SetDisabled(false);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_TT" ));
				end
			else
				Controls.WarButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_DECLARE_WAR" ));
				Controls.TradeButton:SetDisabled(false);
				Controls.DemandButton:SetDisabled(false);
				Controls.DiscussButton:SetDisabled(false);
				
				if (pActiveTeam:IsForcePeace(g_iAITeam)) then
					Controls.WarButton:SetDisabled(true);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAY_NOT_ATTACK" ));
				elseif (not pActiveTeam:CanDeclareWar(g_iAITeam)) then
					Controls.WarButton:SetDisabled(true);

					-- change tooltip to accomodate vassals
					local iMaster = Teams[g_iAITeam]:GetMaster();
					if (iMaster ~= -1) then
						local pMaster = Teams[iMaster];
						Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAY_NOT_ATTACK_VASSAL", pMaster:GetName() ));
					else
						Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAY_NOT_ATTACK_MOD" ));
					end
				else
					Controls.WarButton:SetDisabled(false);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_DECLARES_WAR_TT" ));
				end
				
			end
		end
		
	-- Hiding Screen
	else
		UIManager:SetUICursor(oldCursor); -- make sure we retrun the cursor to the previous state
		--Controls.LeaderSpeech:SetText( g_strLeaveScreenText );		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
	
	end
end
ContextPtr:SetShowHideHandler( OnShowHide );


----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
			OnReturn();
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );



----------------------------------------------------------------        
----------------------------------------------------------------        
function OnDiscuss()
	Controls.LeaderSpeech:SetText( g_strLeaveScreenText );		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
		
	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_WANTS_DISCUSSION, g_iAIPlayer, 0, 0 );
end
Controls.DiscussButton:RegisterCallback( Mouse.eLClick, OnDiscuss );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnTrade()
	-- This calls into CvDealAI and sets up the initial state of the UI
	Players[g_iAIPlayer]:DoTradeScreenOpened();
	
	Controls.LeaderSpeech:SetText( g_strLeaveScreenText );		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
	
    UI.OnHumanOpenedTradeScreen(g_iAIPlayer);
end
Controls.TradeButton:RegisterCallback( Mouse.eLClick, OnTrade );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnDemand()
	
	Controls.LeaderSpeech:SetText( g_strLeaveScreenText );		-- Seed the text box with something reasonable so that we don't get leftovers from somewhere else
	
    UI.OnHumanDemand(g_iAIPlayer);
end
Controls.DemandButton:RegisterCallback( Mouse.eLClick, OnDemand );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnWarOrPeace()
	
    local bAtWar = Teams[Game.GetActiveTeam()]:IsAtWar(g_iAITeam);
    
	-- Asking for Peace (currently at war) - bring up the trade screen
    if (bAtWar) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_NEGOTIATE_PEACE, g_iAIPlayer, 0, 0 );
		
    -- Declaring War (currently at peace)
	else
		UI.AddPopup{ Type = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARMOVE, Data1 = g_iAITeam, Option1 = true};
    end
    
end
Controls.WarButton:RegisterCallback( Mouse.eLClick, OnWarOrPeace );


----------------------------------------------------------------        
----------------------------------------------------------------        
function WarStateChangedHandler( iTeam1, iTeam2, bWar )
	
	-- Active player changed war state with this AI
	if (iTeam1 == Game.GetActiveTeam() and iTeam2 == g_iAITeam) then
		
		if (bWar) then
			Controls.WarButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE" ));
			Controls.TradeButton:SetDisabled(true);
			Controls.DemandButton:SetDisabled(true);
			Controls.DiscussButton:SetDisabled(true);
		else
			Controls.WarButton:SetText(Locale.ConvertTextKey( "TXT_KEY_DIPLO_DECLARE_WAR" ));
			Controls.TradeButton:SetDisabled(false);
			Controls.DemandButton:SetDisabled(false);
			Controls.DiscussButton:SetDisabled(false);
		end
		
	end
	
end
Events.WarStateChanged.Add( WarStateChangedHandler );

---------------------------------------------------------------------------------------
-- Support for Modded Add-in UI's
---------------------------------------------------------------------------------------
g_uiAddins = {};
for addin in Modding.GetActivatedModEntryPoints("DiplomacyUIAddin") do
	local addinFile = Modding.GetEvaluatedFilePath(addin.ModID, addin.Version, addin.File);
	local addinPath = addinFile.EvaluatedPath;
	
	-- Get the absolute path and filename without extension.
	local extension = Path.GetExtension(addinPath);
	local path = string.sub(addinPath, 1, #addinPath - #extension);
	
	table.insert(g_uiAddins, ContextPtr:LoadNewContext(path));
end

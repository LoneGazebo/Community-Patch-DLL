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
	
	-- CBP
	-- Warscore
	if (pActiveTeam:IsAtWar(g_iAITeam)) then
		Controls.WarScore:SetHide(false);
		local iWarScore = pActivePlayer:GetWarScore(g_iAIPlayer);
		local strWarScore = Locale.ConvertTextKey("TXT_KEY_WAR_SCORE", iWarScore);

		Controls.WarScore:SetText(strWarScore);
	
		local strWarInfo = Locale.ConvertTextKey("TXT_KEY_WAR_SCORE_EXPLANATION");

		if(Players[g_iAIPlayer]:IsWillingToMakePeaceWithHuman(Game.GetActivePlayer())) then
			local iPeaceValue = Players[g_iAIPlayer]:GetTreatyWillingToOffer(Game.GetActivePlayer());
			if(iPeaceValue >  PeaceTreatyTypes.PEACE_TREATY_WHITE_PEACE) then
				if( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_ARMISTICE ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_ARMISTICE" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_SETTLEMENT ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_SETTLEMENT" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_BACKDOWN ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_BACKDOWN" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_SUBMISSION ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_SUBMISSION" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_SURRENDER ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_SURRENDER" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_CESSION ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_CESSION" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_CAPITULATION ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_CAPITULATION" );
				elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_UNCONDITIONAL_SURRENDER ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_UNCONDITIONAL_SURRENDER" );
				end
			end
		else
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_NO_PEACE_OFFER" );
		end

		local iStrengthAverage = pActivePlayer:GetPlayerMilitaryStrengthComparedToUs(g_iAIPlayer);
		if( iStrengthAverage == StrengthTypes.STRENGTH_PATHETIC ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_PATHETIC" );
		elseif( iStrengthAverage == StrengthTypes.STRENGTH_WEAK ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_WEAK" );
		elseif( iStrengthAverage == StrengthTypes.STRENGTH_POOR ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_POOR" );
		elseif( iStrengthAverage == StrengthTypes.STRENGTH_AVERAGE ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_AVERAGE" );
		elseif( iStrengthAverage == StrengthTypes.STRENGTH_STRONG ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_STRONG" );
		elseif( iStrengthAverage == StrengthTypes.STRENGTH_POWERFUL ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_POWERFUL" );
		elseif( iStrengthAverage == StrengthTypes.STRENGTH_IMMENSE ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_IMMENSE" );
		end

		local iEconomicAverage = pActivePlayer:GetPlayerEconomicStrengthComparedToUs(g_iAIPlayer);
		if( iEconomicAverage == StrengthTypes.STRENGTH_PATHETIC ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_PATHETIC" );
		elseif( iEconomicAverage == StrengthTypes.STRENGTH_WEAK ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_WEAK" );
		elseif( iEconomicAverage == StrengthTypes.STRENGTH_POOR ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_POOR" );
		elseif( iEconomicAverage == StrengthTypes.STRENGTH_AVERAGE ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_AVERAGE" );
		elseif( iEconomicAverage == StrengthTypes.STRENGTH_STRONG ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_STRONG" );
		elseif( iEconomicAverage == StrengthTypes.STRENGTH_POWERFUL ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_POWERFUL" );
		elseif( iEconomicAverage == StrengthTypes.STRENGTH_IMMENSE ) then
			strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_IMMENSE" );
		end

		local iUsWarDamage = Players[g_iAIPlayer]:GetWarDamageLevel(Game.GetActivePlayer());
		local iThemWarDamage = pActivePlayer:GetWarDamageLevel(g_iAIPlayer);
		if(iUsWarDamage > WarDamageLevelTypes.WAR_DAMAGE_LEVEL_NONE)then
			if( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MINOR ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_MINOR" );
			elseif( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MAJOR ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_MAJOR" );
			elseif( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_SERIOUS ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_SERIOUS" );
			elseif( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_CRIPPLED ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_CRIPPLED" );
			end
		elseif(iThemWarDamage > WarDamageLevelTypes.WAR_DAMAGE_LEVEL_NONE)then
			if( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MINOR ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_MINOR" );
			elseif( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MAJOR ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_MAJOR" );
			elseif( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_SERIOUS ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_SERIOUS" );
			elseif( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_CRIPPLED ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_CRIPPLED" );
			end
		end
		Controls.WarScore:SetToolTipString(strWarInfo);
	else
		Controls.WarScore:SetHide(true);
	end
	-- END
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
		
		oldCursor = UIManager:SetUICursor(0); -- make sure we start with the default cursor
		
	    if (g_iAITeam ~= -1) then
			local bAtWar = pActiveTeam:IsAtWar(g_iAITeam);
			
			if (bAtWar) then
				Controls.WarButton:SetText( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE" ));
				Controls.TradeButton:SetDisabled(true);
				Controls.DemandButton:SetDisabled(true);
				Controls.DiscussButton:SetDisabled(true);
				
				local iLockedWarTurns = pActiveTeam:GetNumTurnsLockedIntoWar(g_iAITeam);
				
				-- Not locked into war
				if (iLockedWarTurns == 0) then
					Controls.WarButton:SetDisabled(false);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_TT" ));
				-- Locked into war
				else
					Controls.WarButton:SetDisabled(true);
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_BLOCKED_TT", iLockedWarTurns ));
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
					Controls.WarButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAY_NOT_ATTACK_MOD" ));
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

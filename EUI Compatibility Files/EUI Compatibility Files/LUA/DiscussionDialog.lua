print("This is the modded DiscussionDialog.lua from CBP - C4DF")
----------------------------------------------------------------
----------------------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "GameplayUtilities" );
include( "InfoTooltipInclude" );

local g_InstanceManager = InstanceManager:new( "Leader Button", "Button", Controls.LeaderStack );
local g_iAIPlayer = -1;
local g_iAITeam = -1;
local g_CurrentTopic = -1;

local g_bCanGoBack = true;

local g_DiploUIState = -1;
local g_iDiploData = -1;

-- Discussion modes
local g_iModeDiscussionRoot = 0;
--local g_iModeDiscussionWorkAgainst = 1;
local g_iModeDiscussionWar = 1;
local g_iInvokedDiscussionMode = g_iModeDiscussionRoot;

local g_strDefaultDiscussionText = Locale.ConvertTextKey("TXT_KEY_DIPLOMACY_DISCUSS_WHAT");

local offsetOfString = 32;
local bonusPadding = 16
local innerFrameWidth = 554;
local outerFrameWidth = 550;
local offsetsBetweenFrames = 4;

local oldCursor = 0;

----------------------------------------------------------------        
-- LEADER MESSAGE HANDLER
----------------------------------------------------------------        
function LeaderMessageHandler( iPlayer, iDiploUIState, szLeaderMessage, iAnimationAction, iData1 )
    
    g_DiploUIState = iDiploUIState;
    g_iDiploData = iData1;
    
	g_iAIPlayer = iPlayer;
	g_iAITeam = Players[g_iAIPlayer]:GetTeam();
	
	local pAIPlayer = Players[g_iAIPlayer];
	local iActivePlayer = Game.GetActivePlayer();
	
	local pActiveTeam = Teams[Game.GetActiveTeam()];
	local pAITeam = Teams[g_iAITeam];
	
	local activePlayer = Players[iActivePlayer];
	
	local themCivType = pAIPlayer:GetCivilizationType();
	local themCivInfo = GameInfo.Civilizations[themCivType];
	CivIconHookup( g_iAIPlayer, 64, Controls.ThemSymbolShadow, Controls.CivIconBG, Controls.CivIconShadow, false, true );
    
    local player = Players[g_iAIPlayer];
	local strTitleText = GameplayUtilities.GetLocalizedLeaderTitle(player);
	Controls.TitleText:SetText(strTitleText);
	
	local otherLeader = GameInfo.Leaders[player:GetLeaderType()];
	
	-- Mood
	local iApproach = Players[iActivePlayer]:GetApproachTowardsUsGuess(g_iAIPlayer);
	local strMoodText = Locale.ConvertTextKey("TXT_KEY_EMOTIONLESS");
	
	if (pActiveTeam:IsAtWar(g_iAITeam)) then
		strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" );
	elseif (Players[g_iAIPlayer]:IsDenouncingPlayer(iActivePlayer)) then
		strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_DENOUNCING" );	
	elseif (Players[g_iAIPlayer]:WasResurrectedThisTurnBy(iActivePlayer)) then
		strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_LIBERATED" );
	else
		if( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_WAR_CAPS" );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_HOSTILE", otherLeader.Description );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_GUARDED", otherLeader.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_AFRAID", otherLeader.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_FRIENDLY", otherLeader.Description  );
		elseif( iApproach == MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL ) then
			strMoodText = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_NEUTRAL", otherLeader.Description  );
		end
	end
	
	Controls.MoodText:SetText(strMoodText);
	
	local strMoodInfo = GetMoodInfo(g_iAIPlayer);
	Controls.MoodText:SetToolTipString(strMoodInfo);

	local bMyMode = false;
	
	local strExtra = "";
	
	-- Are we in Discussion Mode?
	if (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_AI_DECLARED_WAR) then
		-- Add text to this string
		strExtra = "[NEWLINE][NEWLINE](" .. Locale.ConvertTextKey("TXT_KEY_WAR_DECLARED") .. ")";
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PROTECT_MINOR_CIV) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_WITH_US) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CAUGHT_YOUR_SPY) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_YOUR_SPY) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_MY_SPY) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_CONVERSIONS) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_DIGGING) then
		bMyMode = true;
	-- Putmalk
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REVOKE_VASSALAGE) then
		bMyMode = true;
	elseif (iDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN) then
		print("DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN");
		if (iData1 == 1) then
			bMyMode = true;
			print("DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN activated in DiscussionDialog");
		else
			print("DiploUIStateTypes.DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN NOT activated in DiscussionDialog");
		end
	end
    
    if (bMyMode) then
    
        if( ContextPtr:IsHidden() ) then
    	    UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderDiscuss );
	    end
		
		print("Handling LeaderMessage: " .. iDiploUIState .. ", ".. szLeaderMessage .. strExtra);
		
		Controls.LeaderSpeech:SetText( szLeaderMessage .. strExtra );
		
		-- Resize the height of the box to fit the text
		local contentSize = Controls.LeaderSpeech:GetSize().y + offsetOfString + bonusPadding;
		Controls.LeaderSpeechBorderFrame:SetSizeY( contentSize );
		Controls.LeaderSpeechFrame:SetSizeY( contentSize - offsetsBetweenFrames );
		
		local strButton1Text = -1;
		local strButton2Text = -1;
		local strButton3Text = -1;
		local strButton4Text = -1;
		local strButton5Text = -1;
		local strButton6Text = -1;
		local strButton7Text = -1;
		local strButton8Text = -1;
		local strButton9Text = -1;
		local strButton10Text = -1;
		local strButton11Text = -1;
		
		local strButton1Tooltip = "";
		local strButton2Tooltip = "";
		local strButton3Tooltip = "";
		local strButton4Tooltip = "";
		local strButton5Tooltip = "";
		local strButton6Tooltip = "";
		local strButton7Tooltip = "";
		local strButton8Tooltip = "";
		local strButton9Tooltip = "";
		local strButton10Tooltip = "";
		local strButton11Tooltip = "";
		
		-- Make sure none of the buttons start disabled
 		Controls.Button1:SetDisabled(false);
 		Controls.Button2:SetDisabled(false);
 		Controls.Button3:SetDisabled(false);
 		Controls.Button4:SetDisabled(false);
		Controls.Button5:SetDisabled(false);
		Controls.Button6:SetDisabled(false);
		Controls.Button7:SetDisabled(false);
		Controls.Button8:SetDisabled(false);
		Controls.Button9:SetDisabled(false);
		Controls.Button10:SetDisabled(false);
		Controls.Button11:SetDisabled(false);
		
		local bHideBackButton = false;
	    
		-- Human invoked discussion
		if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
			
			-- Discussion Root Mode
			if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
				local strLeaderName;
				if(pAIPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
					strLeaderName = pAIPlayer:GetNickName();
				else
					strLeaderName = pAIPlayer:GetName();
				end

				--------------------
				-- SHARE INTRIGUE --
				--------------------

				if (activePlayer:HasRecentIntrigueAbout(iPlayer) and not Players[iPlayer]:IsHuman()) then
					local strLeaderName;
					if(pAIPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
						strLeaderName = pAIPlayer:GetNickName();
					else
						strLeaderName = pAIPlayer:GetName()
					end
								
					strButton2Text = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_SHARE_INTRIGUE", strLeaderName);
					strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_SHARE_INTRIGUE_TT", strLeaderName);
				end
				
				--------------------
				--   STOP SPYING  --
				--------------------

				local agents = pAIPlayer:GetEspionageSpies();
				local bHasAgents = #agents ~= 0;

				if (not pAIPlayer:IsStopSpyingMessageTooSoon(iActivePlayer) and bHasAgents and (g_iAITeam ~= Game.GetActiveTeam())) then
					strButton3Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_STOP_SPYING" );
				end
				
				---------------------
				--  STOP RELIGION  --
				---------------------

				-- Ask the AI player not to send missionaries or prophets
				if (activePlayer:GetNegativeReligiousConversionPoints(g_iAIPlayer) > 0 or activePlayer:HasCreatedReligion()) then
					if (not pAIPlayer:IsAskedToStopConverting(iActivePlayer)) then
						strButton4Text = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_STOP_SPREADING_RELIGION");
					end
				end
					
				---------------------
				--   DON'T SETTLE  --
				---------------------

				-- Ask the AI player to not settle nearby
				if (not pAIPlayer:IsDontSettleMessageTooSoon(iActivePlayer)) then
					strButton5Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DONT_SETTLE" );
				end
				
				-----------------------
				--	STOP ARCHAEOLOGY --
				-----------------------
				
				-- Ask the AI player not to dig up my artifacts
				if (activePlayer:GetNegativeArchaeologyPoints(g_iAIPlayer) > 0 and not pAIPlayer:IsAskedToStopDigging(iActivePlayer)) then
					strButton6Text = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_STOP_DIGGING");
				end
				
				-----------------------
				--  DENOUNCE AND DOF --
				-----------------------
				-- If we're teammates, there's no need to work together or against anyone
				if (pAIPlayer:GetTeam() ~= Players[iActivePlayer]:GetTeam()) then
					-- Ask the AI player to work together
					if (not pAIPlayer:IsDoF(iActivePlayer)) then
						strButton7Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP" );
						strButton7Tooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP_TT" );
-- CBP
					-- Tell the player we're done working with him
					elseif (pAIPlayer:IsDoF(iActivePlayer)) then
						strButton7Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_END_WORK_WITH_US" );
						strButton7Tooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_END_DEC_FRIENDSHIP_TT" );
					end
-- END	
					
					local strLeaderName;
					if(pAIPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
						strLeaderName = pAIPlayer:GetNickName();
					else
						strLeaderName = pAIPlayer:GetName();
					end
					
					if(not activePlayer:IsDenouncedPlayer(iPlayer)) then
						strButton8Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DENOUNCE", strLeaderName);
						strButton8Tooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DENOUNCE_TT");
					end
				end
		 		
				-----------------
				-- MOVE TROOPS --
				-----------------
				-- Hostile!
				print("Hello");
				print(tostring(activePlayer:GetMilitaryAggressivePosture(g_iAIPlayer)));
				print(AggressivePostureTypes.AGGRESSIVE_POSTURE_MEDIUM);
				
				if(not pAIPlayer:MoveRequestTooSoon(iActivePlayer)) then
					if(activePlayer:GetMilitaryAggressivePosture(g_iAIPlayer) >= AggressivePostureTypes.AGGRESSIVE_POSTURE_MEDIUM) then
						if(pAIPlayer:GetMajorCivApproach(iActivePlayer) == MAJOR_CIV_APPROACH_HOSTILE) then
							strButton9Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_MOVE_TROOPS_HOSTILE" );
						else
							strButton9Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_MOVE_TROOPS" );
						end
						
						strButton9Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_MOVE_TROOPS_TT", strLeaderName);
					end
				end
				
				--------------------------------
				--  DECLARE WAR (THIRD PARTY) --
				--------------------------------
				strButton10Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DECLARE_WAR" );
-- CBP
				strButton10Tooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DECLARE_WAR_TT" );
-- END
				Controls.Button10:SetDisabled(true);

				--------------------
				--  SHARE OPINION --
				--------------------
				strButton11Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_SHARE_OPINION" );
				Controls.Button11:SetDisabled(true);
				
				-- Discussion buttons valid?
				for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
					
					-- Working Against button
					--if (IsWorkingAgainstThirdPartyPlayerValid(iPlayerLoop)) then
						--Controls.Button3:SetDisabled(false);
					--end
					
					-- War button: Button 10
					if (IsWarAgainstThirdPartyPlayerValid(iPlayerLoop)) then
						Controls.Button10:SetDisabled(false);
					end

					-- Share Opinion button: Button 11
					if (IsShareOpinionOnThirdPartyPlayerValid(iPlayerLoop)) then
						Controls.Button11:SetDisabled(false);
					end
				end
			end
			
		-- Human did something mean, AI responded, and human responds in turn with fluff
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_SORRY" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEAL" );
-- CBP
			strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_YOULL_PAY_TT");
-- END
			bHideBackButton = true;
			
		-- AI did something mean, and human responds in turn with fluff
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_OKAY" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_YOULL_PAY" );
-- CBP
			strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_YOULL_PAY_TT");
-- END
			bHideBackButton = true;
			
		-- AI declared war on us!
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_AI_DECLARED_WAR) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_OKAY" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MESSAGE_YOULL_PAY" );
			
			bHideBackButton = true;
			
		-- Human has Units on our borders
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_WE_MEAN_NO_HARM" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_TIME_TO_DIE" );
			bHideBackButton = true;
		-- AI attacked a Minor the Human is friends with
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NO_DIVIDE" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_YOU_WILL_PAY" );
			local pMinor = Players[iData1];
			if (pMinor ~= nil) then
				local iInfLoss = (GameDefines["MINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT"] / 100) * -1; -- XML is times 100 for fidelity, and negative
				strButton1Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_PROTECTED_MINOR_FORGIVE_TT", pAIPlayer:GetCivilizationShortDescriptionKey(), pMinor:GetCivilizationShortDescriptionKey(), iInfLoss);
				strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_PROTECTED_MINOR_DISPUTE_TT", pAIPlayer:GetCivilizationShortDescriptionKey(), pMinor:GetCivilizationShortDescriptionKey());
			end
			bHideBackButton = true;
		-- AI bullied a Minor the Human has a PtP with
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NO_DIVIDE" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_YOU_WILL_PAY" );
			local pMinor = Players[iData1];
			if (pMinor ~= nil) then
				local iInfLoss = (GameDefines["MINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT"] / 100) * -1; -- XML is times 100 for fidelity, and negative
				strButton1Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_PROTECTED_MINOR_FORGIVE_TT", pAIPlayer:GetCivilizationShortDescriptionKey(), pMinor:GetCivilizationShortDescriptionKey(), iInfLoss);
				strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_PROTECTED_MINOR_DISPUTE_TT", pAIPlayer:GetCivilizationShortDescriptionKey(), pMinor:GetCivilizationShortDescriptionKey());
			end
			bHideBackButton = true;
		-- Human attacked a Protected Minor
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NOT_YOUR_BUSINESS" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_WILL_WITHDRAW" );
			bHideBackButton = true;
		-- Human killed a Protected Minor
		--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV) then
			--strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DONT_CARE" );
			--strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_OFFER_TO_APPEASE" );
			--bHideBackButton = true;
		-- Human bullied a Protected Minor
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NOT_YOUR_BUSINESS" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_BULLYING" );
			bHideBackButton = true;
		---- AI seriously warning human about his expansion
		--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING) then
			--strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_MAKE_US" );
			--strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_OFFER_TO_APPEASE" );
			--bHideBackButton = true;
		-- AI warning human about his expansion
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SETTLE_WHAT_WE_PLEASE") ;
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_SETTLING") ;
-- CBP
			local iTimeOutTurns = Game:GetPromiseDuration();
			strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DURATION_PROMISE", iTimeOutTurns);
-- END
			bHideBackButton = true;
		-- AI seriously warning human about his plot buying
		--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING) then
			--strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_CLAIM_WHAT_WE_WANT" );
			--strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_OFFER_TO_APPEASE" );
			--bHideBackButton = true;
		-- AI warning human about his plot buying
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NOT_YOUR_BUSINESS" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_CLAIMING" );
-- CBP
			local iTimeOutTurns = Game:GetPromiseDuration();
			strButton2Tooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DURATION_PROMISE", iTimeOutTurns);
-- END
			bHideBackButton = true;
		-- AI asking player to work with him
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_WITH_US) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_YES_WORK_TOGETHER" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NO_GO_IT_ALONE" );
			bHideBackButton = true;
		-- AI asking player to work against someone
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_NO_INTEREST" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU" );
			strButton3Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DO_WHAT_WE_CAN" );
			bHideBackButton = true;
		-- AI asking player to declare war against someone
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_NO_INTEREST" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU" );
			strButton3Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_COOP_WAR_SOON" );
			strButton4Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_COOP_WAR_YES" );
-- CBP
			local pMajor = Players[iData1];
			if (pMajor ~= nil) then
				strButton2Tooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU_TT", pAIPlayer:GetCivilizationShortDescriptionKey(), pMajor:GetCivilizationShortDescriptionKey());
			end
-- END
			bHideBackButton = true;
		-- AI shows up saying it's time to declare war against someone
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_COOP_WAR_NOW" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_CHANGED_MIND" );
			bHideBackButton = true;
		-- AI asking player to make RA in the future - NOT CURRENTLY IN USE
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_YES_LET_US_PREPARE" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_OTHER_PLANS" );
			bHideBackButton = true;
		-- AI asking player to denounce someone
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DO_WHAT_WE_CAN" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_NO_INTEREST" );
			bHideBackButton = true;
		-- AI caught one of player's spies
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CAUGHT_YOUR_SPY) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NOT_SORRY_FOR_SPY_CAUGHT" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_SPY_CAUGHT" );
			bHideBackButton = true;
		-- AI killed one of player's spies
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_YOUR_SPY) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_NOT_SORRY_FOR_SPY_KILLED" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_SORRY_FOR_SPY_KILLED" );
			bHideBackButton = true;
		-- Player killed one of AI's spies
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_MY_SPY) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DONT_FORGIVE_SPYING" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_FORGIVE_SPYING" );
			bHideBackButton = true;
			
		-- Player caught one of the AI's spies
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY) then
			local strLeaderName;
			if(pAIPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
				strLeaderName = pAIPlayer:GetNickName();
			else
				strLeaderName = pAIPlayer:GetName();
			end

			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_CAUGHT_FORGIVE_SPYING" );
			
			if (not pAIPlayer:IsStopSpyingMessageTooSoon(iActivePlayer)) then
				strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_CAUGHT_DEMAND_STOP_SPYING" );
			end
			
			if(not activePlayer:IsDenouncedPlayer(iPlayer)) then
				strButton3Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_CAUGHT_DENOUNCE_FOR_SPYING", strLeaderName );
 			end
 			
			strButton4Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_CAUGHT_DECLARE_WAR", strLeaderName );
			
			if (pActiveTeam:IsForcePeace(g_iAITeam)) then
				Controls.Button4:SetDisabled(true);
				strButton4Tooltip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAY_NOT_ATTACK" );
			end
			
			bHideBackButton = true;
			
		-- Player converted cities in another empire
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_CONVERSIONS) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DONT_STOP_CONVERSIONS" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_STOP_CONVERSIONS" );
			bHideBackButton = true;
	    
		-- Player dug for artifacts in another empire
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_DIGGING) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DONT_STOP_DIGGING" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_STOP_DIGGING" );
			bHideBackButton = true;
		
		-- Putmalk
		-- AI wants to revoke Vassalage
		elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REVOKE_VASSALAGE) then
			strButton1Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_DONT_ALLOW_VASSAL_INDEPENDENCE" );
			strButton2Text = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DISCUSS_ALLOW_VASSAL_INDEPENDENCE" );
			bHideBackButton = true;
		end
	    
		-- Buttons: change text or hide
		if (strButton1Text == -1) then
	 		Controls.Button1:SetHide(true);
		else
	 		Controls.Button1Label:SetText(strButton1Text);		
	 		Controls.Button1:SetHide(false);
	 		Controls.Button1:SetToolTipString(strButton1Tooltip);
		end
	    
		if (strButton2Text == -1) then
	 		Controls.Button2:SetHide(true);
		else
	 		Controls.Button2Label:SetText(strButton2Text);		
	 		Controls.Button2:SetHide(false);
	 		Controls.Button2:SetToolTipString(strButton2Tooltip);
		end

		if (strButton3Text == -1) then
	 		Controls.Button3:SetHide(true);
		else
	 		Controls.Button3Label:SetText(strButton3Text);		
	 		Controls.Button3:SetHide(false);
	 		Controls.Button3:SetToolTipString(strButton3Tooltip);
		end

		if (strButton4Text == -1) then
	 		Controls.Button4:SetHide(true);
		else
	 		Controls.Button4Label:SetText(strButton4Text);		
	 		Controls.Button4:SetHide(false);
	 		Controls.Button4:SetToolTipString(strButton4Tooltip);
		end

		if (strButton5Text == -1) then
			Controls.Button5:SetHide(true);
		else
			Controls.Button5Label:SetText(strButton5Text);
			Controls.Button5:SetHide(false);
			Controls.Button5:SetToolTipString(strButton5Tooltip);
		end	

		if (strButton6Text == -1) then
			Controls.Button6:SetHide(true);
		else
			Controls.Button6Label:SetText(strButton6Text);
			Controls.Button6:SetHide(false);
			Controls.Button6:SetToolTipString(strButton6Tooltip);
		end

		if (strButton7Text == -1) then
			Controls.Button7:SetHide(true);
		else
			Controls.Button7Label:SetText(strButton7Text);
			Controls.Button7:SetHide(false);
			Controls.Button7:SetToolTipString(strButton7Tooltip);
		end

		if (strButton8Text == -1) then
			Controls.Button8:SetHide(true);
		else
			Controls.Button8Label:SetText(strButton8Text);
			Controls.Button8:SetHide(false);
			Controls.Button8:SetToolTipString(strButton8Tooltip);
		end
		
		if (strButton9Text == -1) then
			Controls.Button9:SetHide(true);
		else
			Controls.Button9Label:SetText(strButton9Text);
			Controls.Button9:SetHide(false);
			Controls.Button9:SetToolTipString(strButton9Tooltip);
		end
		
		if (strButton10Text == -1) then
			Controls.Button10:SetHide(true);
		else
			Controls.Button10Label:SetText(strButton10Text);
			Controls.Button10:SetHide(false);
			Controls.Button10:SetToolTipString(strButton10Tooltip);
		end
		
		if (strButton11Text == -1) then
			Controls.Button11:SetHide(true);
		else
			Controls.Button11Label:SetText(strButton11Text);
			Controls.Button11:SetHide(false);
			Controls.Button11:SetToolTipString(strButton11Tooltip);
		end
		
		-- Some situations we force the human to answer - he can't back out
		if (bHideBackButton) then
	 		Controls.BackButton:SetHide(true);
	 		g_bCanGoBack = false;
		else
	 		Controls.BackButton:SetHide(false);
	 		g_bCanGoBack = true;
		end
		
	end
    
end
Events.AILeaderMessage.Add( LeaderMessageHandler );


----------------------------------------------------------------
-- BACK
----------------------------------------------------------------
function OnBack(bForce)
    
    if (g_bCanGoBack or bForce) then
		
	    UIManager:DequeuePopup( ContextPtr );
		Controls.LeaderPanel:SetHide( true );
	    
    	UI.RequestLeaveLeader();
	end

end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------        
-- SHOW/HIDE
----------------------------------------------------------------        
local bEverShown = false;
function OnShowHide( bHide, bIsInit )
	
	if( not bIsInit ) then
    	-- Showing Screen
    	if (not bHide) then
    		oldCursor = UIManager:SetUICursor(0); -- make sure we start with the default cursor
    		
    		bEverShown = true;
    	    
    		local pPlayer = Players[Game.GetActivePlayer()];
    		local pTeam = Teams[pPlayer:GetTeam()];
    		
    		local pThirdPartyPlayer;
    		local iThirdPartyTeam;
    		
    		--Controls.LeaderSpeech:SetText(g_strDefaultDiscussionText);		-- This will be overwritten immediately by a Leader Message
     		--Controls.Button1:SetHide(true);
     		--Controls.Button2:SetHide(true);
     		--Controls.Button3:SetHide(true);
     		--Controls.Button4:SetHide(true);
    		
    		--------------------------------------------------------------------------
    		-- THE CODE AFTER THIS LINE SHOULD BE REWRITTEN - IT IS OBSOLETE
    		--------------------------------------------------------------------------
    		
    		local iNumPlayers = 0;
    		
    		-- Loop through all the Majors the active player knows
    		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
    			
    			pThirdPartyPlayer = Players[iPlayerLoop];
    			iThirdPartyTeam = pThirdPartyPlayer:GetTeam();
    			
    			-- Valid player? - Can't be us and has to be alive
    			if (pPlayer:GetTeam() ~= iThirdPartyTeam and pThirdPartyPlayer:IsAlive()) then
    				
    				-- Can't be the guy we're already talking to
    --				if (g_iAITeam ~= iThirdPartyTeam) then
    					
    					-- Met this player?
    					if (pTeam:IsHasMet(iThirdPartyTeam)) then
    						iNumPlayers = iNumPlayers + 1;
    					end
    --				end
    			end
    		end
    		
    	-- Hiding Screen
    	else
    		UIManager:SetUICursor(oldCursor); -- make sure we retrun the cursor to the previous state
    		
    		g_bCanGoBack	= true;		
    		g_DiploUIState = -1;
    		g_iDiploData = -1;
    		
    		-- Reset to Default mode
    		g_iInvokedDiscussionMode = g_iModeDiscussionRoot;
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
        if( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
			
			if(Controls.DenounceConfirm:IsHidden() and Controls.WarConfirm:IsHidden()) then
				-- Talk-about-a-leader mode
				if (g_iInvokedDiscussionMode ~= g_iModeDiscussionRoot) then
					OnCloseLeaderPanelButton();
					
				-- In base discussion mode, so exit
				else
					OnBack();
				end
			elseif (not Controls.DenounceConfirm:IsHidden()) then
				OnDenounceConfirmNo();
			elseif (not Controls.WarConfirm:IsHidden()) then
				OnWarConfirmNo();
			end
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------
-- BUTTON 1
----------------------------------------------------------------
function OnButton1()
    g_InstanceManager:ResetInstances();
    	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local iButtonID = 1;	-- This format is also used in DiploTrade.lua in the OnBack() function.  If functionality here changes it should be updated there as well.
        
    -- Fluff discussion mode
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN) then
		OnBack(true);
    -- Fluff discussion mode 2
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI) then
		OnBack(true);
    -- AI declared war on us
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_AI_DECLARED_WAR) then
		OnBack(true);
    
    -- AI seriously warning us about expansion - we tell him we mean no harm
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_AGGRESSIVE_MILITARY_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI told us he attacked a Minor we're friends with - we forgive him
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV) then
		local iMinor = g_iDiploData;
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_I_ATTACKED_YOUR_MINOR_CIV_RESPONSE, g_iAIPlayer, iButtonID, iMinor );

    -- AI told us he bullied a Minor we have a PtP with - we forgive him
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV) then
		local iMinor = g_iDiploData;
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_I_BULLIED_YOUR_MINOR_CIV_RESPONSE, g_iAIPlayer, iButtonID, iMinor );

    -- AI warned us about attacking a Minor - we tell him to go away
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_ATTACKED_MINOR_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI scolded us for killing a Minor - we tell him to go away
	--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE, g_iAIPlayer, iButtonID, 0 );

    -- AI warned us about bullying a Minor - we tell him to go away
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_BULLIED_MINOR_RESPONSE, g_iAIPlayer, iButtonID, 0 );

    -- AI seriously warning us about expansion - we tell him to go away
	--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI warning us about expansion - we tell him to go away
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI seriously warning us about plot buying - we tell him to go away
	--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI warning us about plot buying - we tell him to go away
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI asking to work together - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_WITH_US) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_WITH_US_RESPONSE, g_iAIPlayer, iButtonID, 0 );
 
    -- AI asking to work against someone - we tell him sorry
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
        
    -- AI asking to declare war against someone - we say no
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
	    
    -- AI asking to declare war against someone NOW - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_NOW_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
    
    -- AI asking to make RA in the future - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLAN_RA_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI asking to denounce another player - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_AI_REQUEST_DENOUNCE_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
		
	-- AI asking to stop spying - we ignore
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CAUGHT_YOUR_SPY) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer);

	-- AI asking to stop spying after death - we disagree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_YOUR_SPY) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer);
	
	-- AI asking to forgive for spying - we disagree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_MY_SPY) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer);
	
	-- forgive the AI and mark the dialog as done
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY) then
		-- Faking the button type to be forgiven
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE, g_iAIPlayer, 2, iAgainstPlayer);
		
    -- AI asking you to stop religious conversions	
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_CONVERSIONS) then
	   Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_STOP_CONVERSIONS, g_iAIPlayer, iButtonID, iAgainstPlayer);
	   
    -- AI asking you to stop digging
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_DIGGING) then
	   Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_STOP_DIGGING, g_iAIPlayer, iButtonID, iAgainstPlayer);
	  
	-- AI asking you to revoke their vassalage - We tell him to die
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REVOKE_VASSALAGE) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_ENDS_VASSALAGE, g_iAIPlayer, iButtonID, 0 );
	
    -- Default mode - TBR
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		
		local pThirdPartyPlayer;
		local iThirdPartyTeam;
		
		local iNumPlayers = 0;
		
		-- Loop through all the Majors the active player knows
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			
			pThirdPartyPlayer = Players[iPlayerLoop];
			iThirdPartyTeam = pThirdPartyPlayer:GetTeam();
			
			-- Valid player? - Can't be us and has to be alive
			if (pPlayer:GetTeam() ~= iThirdPartyTeam and pThirdPartyPlayer:IsAlive()) then
				
				-- Can't be the guy we're already talking to
				if (g_iAITeam ~= iThirdPartyTeam) then
					
					-- Met this player?
					if (pTeam:IsHasMet(iThirdPartyTeam)) then
						iNumPlayers = iNumPlayers + 1;
						AddLeaderButton( iPlayerLoop, pThirdPartyPlayer:GetName() );
					end
				end
			end
		end

		-- If there's no one to talk about then don't do anything
		if (iNumPlayers > 0) then
			Controls.LeaderPanel:SetHide( false );
		end
	end
	
end
Controls.Button1:RegisterCallback( Mouse.eLClick, OnButton1 );


----------------------------------------------------------------
-- BUTTON 2
----------------------------------------------------------------
function OnButton2()
    g_InstanceManager:ResetInstances();
    	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local pAIPlayer = Players[g_iAIPlayer];
	
	local iActivePlayer = Game.GetActivePlayer();
	
	local iButtonID = 2;	-- This format is also used in DiploTrade.lua in the OnBack() function.  If functionality here changes it should be updated there as well.
        
    -- Human-invoked discussion
    if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			-- Share intrigue with AI player
			local iIntriguePlotter;
			local iIntrigueType;
			iIntriguePlotter, iIntrigueType = pPlayer:GetRecentIntrigueInfo(g_iAIPlayer);			
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_SHARE_INTRIGUE, g_iAIPlayer, iIntriguePlotter, iIntrigueType);
		end
        
    -- Fluff discussion mode
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN) then
-- CBP
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_MEAN_RESPONSE, g_iAIPlayer, iButtonID, 0 );
-- END
    -- Fluff discussion mode 2
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI) then
-- CBP
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_MEAN_RESPONSE, g_iAIPlayer, iButtonID, 0 );
-- END
    -- AI declared war on us!
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_AI_DECLARED_WAR) then
-- CBP
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_MEAN_RESPONSE, g_iAIPlayer, iButtonID, 0 );
-- END
        
    -- AI is telling us he sees a military buildup - we tell him to die
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_AGGRESSIVE_MILITARY_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );

    -- AI told us he attacked a Minor we're friends with - we're going to get revenge for this
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV) then
		local iMinor = g_iDiploData;
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_I_ATTACKED_YOUR_MINOR_CIV_RESPONSE, g_iAIPlayer, iButtonID, iMinor );

    -- AI told us he bullied a Minor we have a PtP with - we're going to get revenge for this
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV) then
		local iMinor = g_iDiploData;
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_I_BULLIED_YOUR_MINOR_CIV_RESPONSE, g_iAIPlayer, iButtonID, iMinor );
    
    -- AI warned us about attacking a Minor - we tell him we'll be nice from now on
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_ATTACKED_MINOR_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI scolded us for killing a Minor - we offer to give him something to keep the peace
	--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE, g_iAIPlayer, iButtonID, 0 );

    -- AI warned us about bullying a Minor - we tell him we'll be nice from now on
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_BULLIED_MINOR_RESPONSE, g_iAIPlayer, iButtonID, 0 );

    -- AI seriously warning us about expansion - we offer to give him something to keep the peace
	--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI warning us about expansion - we apologize
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI seriously warning us about plot buying - we offer to give him something to keep the peace
	--elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI warning us about plot buying - we apologize
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_WARNING_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI asking to work with us - we tell him sorry
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_WITH_US) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_WITH_US_RESPONSE, g_iAIPlayer, iButtonID, 0 );
		
    -- AI asking to work against someone - we're offended
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
        
    -- AI asking to declare war against someone - we're offended
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
    
    -- AI asking to declare war against someone NOW - we tell him no
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_NOW_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
    
    -- AI asking to make RA in the future - we tell him sorry
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLAN_RA_RESPONSE, g_iAIPlayer, iButtonID, 0 );
    
    -- AI asking to denounce another player - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_AI_REQUEST_DENOUNCE_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
		
	-- AI asking to stop spying - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CAUGHT_YOUR_SPY) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer);
		
	-- AI asking to stop spying after death - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_YOUR_SPY) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer);

	-- AI asking to forgive for spying - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_KILLED_MY_SPY) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer);

	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY) then
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			-- Ask the AI player not to spy any more
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING, g_iAIPlayer, 1, 0 );
		end
				
    -- AI asking you to stop religious conversions	
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_CONVERSIONS) then
	   Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_STOP_CONVERSIONS, g_iAIPlayer, iButtonID, iAgainstPlayer);

    -- AI asking you to stop aggressive archaeology	
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_STOP_DIGGING) then
	   Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_STOP_DIGGING, g_iAIPlayer, iButtonID, iAgainstPlayer);

	-- AI asking you to revoke their vassalage - We tell him we'll do it
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_AI_REVOKE_VASSALAGE) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_ENDS_VASSALAGE, g_iAIPlayer, iButtonID, 0 );

    -- Default mode
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		
	end
	
end
Controls.Button2:RegisterCallback( Mouse.eLClick, OnButton2 );


----------------------------------------------------------------
-- BUTTON 3
----------------------------------------------------------------
function OnButton3()
    g_InstanceManager:ResetInstances();
    
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local pAIPlayer = Players[g_iAIPlayer];
	
	local iButtonID = 3;	-- This format is also used in DiploTrade.lua in the OnBack() function.  If functionality here changes it should be updated there as well.
	
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			-- Ask the AI player not to spy any more
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING, g_iAIPlayer, 0, 0 );
		end
		
    -- AI asking to work against someone - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
    
    -- AI asking to declare war against someone - we tell him yes, but not yet
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
	    
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			local name;
			if(pAIPlayer:GetNickName() ~= "" and pAIPlayer:IsHuman()) then
				name = pAIPlayer:GetNickName();
			else
				name = pAIPlayer:GetName();		    
			end
			
			Controls.DenounceLabel:LocalizeAndSetText("TXT_KEY_CONFIRM_DENOUNCE", name);
			
			Controls.DenounceConfirm:SetHide(false);
		end

    -- Default mode
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		
	end
	
end
Controls.Button3:RegisterCallback( Mouse.eLClick, OnButton3 );


----------------------------------------------------------------
-- BUTTON 4
----------------------------------------------------------------
function OnButton4()
    g_InstanceManager:ResetInstances();
    	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pAIPlayer = Players[g_iAIPlayer];
	
	local iButtonID = 4;	-- This format is also used in DiploTrade.lua in the OnBack() function.  If functionality here changes it should be updated there as well.
	
	-- Discussion mode brought up by the human
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			-- Ask the AI player not to spy any more
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPREADING_RELIGION, g_iAIPlayer, 0, 0 );
		end
        
    -- AI asking to declare war against someone - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
	
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			Controls.WarConfirm:SetHide(false);
		end
	
    -- Default mode
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		
	end
	
end
Controls.Button4:RegisterCallback( Mouse.eLClick, OnButton4 );

----------------------------------------------------------------
-- BUTTON 5
----------------------------------------------------------------
function OnButton5()
    g_InstanceManager:ResetInstances();
    	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pAIPlayer = Players[g_iAIPlayer];
	
	local iButtonID = 5;	-- This format is also used in DiploTrade.lua in the OnBack() function.  If functionality here changes it should be updated there as well.
	
	-- Discussion mode brought up by the human
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			-- Ask the AI player not to settle nearby
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_DONT_SETTLE, g_iAIPlayer, 0, 0 );
			-- Tell the player we're done working with him
			--elseif (pAIPlayer:IsDoF(iActivePlayer)) then
				--Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_END_WORK_WITH_US, g_iAIPlayer, 0, 0 );
		end
        
    -- AI asking to work against someone - we agree
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_WORK_AGAINST_SOMEONE) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
    
    -- AI asking to declare war against someone - we tell him yes, but not yet
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_COOP_WAR) then
		local iAgainstPlayer = g_iDiploData;	-- This should be set when receiving the leader message
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE, g_iAIPlayer, iButtonID, iAgainstPlayer );
	    
    -- Default mode
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		
	end
	
end
Controls.Button5:RegisterCallback( Mouse.eLClick, OnButton5 );

----------------------------------------------------------------
-- BUTTON 6
----------------------------------------------------------------
function OnButton6()
    g_InstanceManager:ResetInstances();
    	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pAIPlayer = Players[g_iAIPlayer];

	local iButtonID = 6;

	-- Discussion mode brought up by the human
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_DIGGING, g_iAIPlayer, 0, 0 );
			end
		end
    -- Default mode
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
	
	end	
end
Controls.Button6:RegisterCallback( Mouse.eLClick, OnButton6 );

----------------------------------------------------------------
-- BUTTON 7
----------------------------------------------------------------
function OnButton7()
	g_InstanceManager:ResetInstances();

	local pPlayer = Players[Game.GetActivePlayer()];
	local pAIPlayer = Players[g_iAIPlayer];
	local pTeam = Teams[pPlayer:GetTeam()];
-- CBP
	local iActivePlayer = Game.GetActivePlayer();
-- END
	
	local iButtonID = 7;
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			-- Ask the AI player to work together
			if (not pAIPlayer:IsDoF(iActivePlayer)) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_WORK_WITH_US, g_iAIPlayer, 0, 0 );
-- CBP
			-- Asking AI to end work together
			elseif(pAIPlayer:IsDoF(iActivePlayer)) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_END_WORK_WITH_US_RESPONSE, g_iAIPlayer, 0, 0 );
-- END
			end
			--if (not pAIPlayer:IsWorkingAgainstPlayerMessageTooSoon(iActivePlayer, 0)) then
				--Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_WORK_WITH_US, g_iAIPlayer, 0, 0 );
			--end
			--g_iInvokedDiscussionMode = g_iModeDiscussionWorkAgainst;
			--OpenLeadersPanel();
		end
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
	end
end
Controls.Button7:RegisterCallback( Mouse.eLClick, OnButton7 );

----------------------------------------------------------------
-- BUTTON 8
----------------------------------------------------------------
function OnButton8()
	g_InstanceManager:ResetInstances();

	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local iButtonID = 8;
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			local pAIPlayer = Players[g_iAIPlayer];
			local name;
			if(pAIPlayer:GetNickName() ~= "" and pAIPlayer:IsHuman()) then
				name = pAIPlayer:GetNickName();
			else
				name = pAIPlayer:GetName();		    
			end
			
			Controls.DenounceLabel:LocalizeAndSetText("TXT_KEY_CONFIRM_DENOUNCE", name);
			
			Controls.DenounceConfirm:SetHide(false);
		end
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
	end
end
Controls.Button8:RegisterCallback( Mouse.eLClick, OnButton8 );

----------------------------------------------------------------
-- BUTTON 9
----------------------------------------------------------------
function OnButton9()
	g_InstanceManager:ResetInstances();

	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local iButtonID = 9;
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_MOVE_TROOPS_RESPONSE, g_iAIPlayer, 0, 0 );
		end
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
	end
end
Controls.Button9:RegisterCallback( Mouse.eLClick, OnButton9);
----------------------------------------------------------------
-- BUTTON 10
----------------------------------------------------------------
function OnButton10()
	g_InstanceManager:ResetInstances();

	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local iButtonID = 10;
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			g_iInvokedDiscussionMode = g_iModeDiscussionWar;
			--print("g_iInvokedDiscussionMode = g_iModeDiscussionWar");
			OpenLeadersPanel();
		end
	elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
	end
end
Controls.Button10:RegisterCallback( Mouse.eLClick, OnButton10);
----------------------------------------------------------------
-- BUTTON 11
----------------------------------------------------------------
function OnButton11()
	g_InstanceManager:ResetInstances();
    	
	local pPlayer = Players[Game.GetActivePlayer()];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pAIPlayer = Players[g_iAIPlayer];
	local pAITeam = Teams[pAIPlayer:GetTeam()];

	local iButtonID = 11;

	-- Discussion mode brought up by the human
	if (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED) then
		-- Discussion Root Mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionRoot) then
			g_iInvokedDiscussionMode = g_iModeDiscussionShareOpinion;
			--print("g_iInvokedDiscussionMode = g_iModeDiscussionShareOpinion");
			OpenLeadersPanel();
		end
    -- Default mode
    elseif (g_DiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DEFAULT_ROOT) then
		
	end	 
end
Controls.Button11:RegisterCallback( Mouse.eLClick, OnButton11 );
----------------------------------------------------------------
-- Time to show the leaders!
----------------------------------------------------------------
function OpenLeadersPanel()
	
	local iNumPlayers = 0;
	
	-- Loop through all the Majors the active player knows
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		
		-- Working Against mode
		--if (g_iInvokedDiscussionMode == g_iModeDiscussionWorkAgainst and IsWorkingAgainstThirdPartyPlayerValid(iPlayerLoop)) then
			--iNumPlayers = iNumPlayers + 1;
			--AddLeaderButton( iPlayerLoop, Players[iPlayerLoop]:GetName() );
		--end
		
		-- War mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionWar and IsWarAgainstThirdPartyPlayerValid(iPlayerLoop)) then
			iNumPlayers = iNumPlayers + 1;
			AddLeaderButton( iPlayerLoop, Players[iPlayerLoop]:GetName() );
		end
		
		-- Share Opinion mode
		if (g_iInvokedDiscussionMode == g_iModeDiscussionShareOpinion and IsShareOpinionOnThirdPartyPlayerValid(iPlayerLoop)) then
			iNumPlayers = iNumPlayers + 1;
			AddLeaderButton( iPlayerLoop, Players[iPlayerLoop]:GetName() );
		end
	end

	-- If there's no one to talk about then don't do anything
	if (iNumPlayers > 0) then
		Controls.LeaderStack:CalculateSize();
		Controls.LeaderStack:ReprocessAnchoring();
		Controls.LeaderScrollPanel:CalculateInternalSize();
		
		Controls.LeaderPanel:SetHide( false );

		-- Disable normal buttons
 		Controls.Button1:SetDisabled(true);
 		Controls.Button2:SetDisabled(true);
 		Controls.Button3:SetDisabled(true);
 		Controls.Button4:SetDisabled(true);
 		Controls.Button5:SetDisabled(true);
 		Controls.Button6:SetDisabled(true);
 		Controls.Button7:SetDisabled(true);
 		Controls.Button8:SetDisabled(true);
 		Controls.Button9:SetDisabled(true);
 		Controls.Button10:SetDisabled(true);
 		Controls.Button11:SetDisabled(true);
	end
end

----------------------------------------------------------------
----------------------------------------------------------------
function IsThirdPartyPlayerValid(iThirdPartyPlayer)

	local pActivePlayer = Players[Game.GetActivePlayer()];
	local pActiveTeam = Teams[pActivePlayer:GetTeam()];
	
	local pThirdPartyPlayer = Players[iThirdPartyPlayer];
	local iThirdPartyTeam = pThirdPartyPlayer:GetTeam();
	
	-- Can't be someone on our team
	if (pActivePlayer:GetTeam() == iThirdPartyTeam) then
		return false;
	end
	
	-- Has to be alive
	if (not pThirdPartyPlayer:IsAlive()) then
		return false;
	end
	
	-- Can't be the guy we're already talking to
	if (g_iAITeam == iThirdPartyTeam) then
		return false;
	end
	
	-- Active player met them?
	if (not pActiveTeam:IsHasMet(iThirdPartyTeam)) then
		return false;
	end
	
	-- Other player met them?
	if (not Teams[g_iAITeam]:IsHasMet(iThirdPartyTeam)) then
		return false;
	end
	
	return true;
	
end

----------------------------------------------------------------
----------------------------------------------------------------
--function IsWorkingAgainstThirdPartyPlayerValid(iThirdPartyPlayer)
	--
	--if (not IsThirdPartyPlayerValid(iThirdPartyPlayer)) then
		--return false;
	--end
	--
	--local pActivePlayer = Players[Game.GetActivePlayer()];
	--
    ---- Have we already agreed?
	--if (Players[g_iAIPlayer]:IsWorkingAgainstPlayerAccepted(pActivePlayer:GetID(), iThirdPartyPlayer)) then
		--return false;
	--end
	--
	--return true;
	--
--end

----------------------------------------------------------------
----------------------------------------------------------------
function IsWarAgainstThirdPartyPlayerValid(iThirdPartyPlayer)
	
	if (not IsThirdPartyPlayerValid(iThirdPartyPlayer)) then
		return false;
	end
	
	local pActivePlayer = Players[Game.GetActivePlayer()];
	
    -- Have we already agreed?
    local iCoopState = Players[g_iAIPlayer]:GetCoopWarAcceptedState(pActivePlayer:GetID(), iThirdPartyPlayer);
	if (iCoopState == CoopWarStates.COOP_WAR_STATE_ACCEPTED or iCoopState == CoopWarStates.COOP_WAR_STATE_SOON) then
		return false;
	end
	
	-- Is a DOW valid against this target?
	local pAIPlayerTeam = Teams[Players[g_iAIPlayer]:GetTeam()];
	local iThirdPartyTeamID = Players[iThirdPartyPlayer]:GetTeam();
	if (not pAIPlayerTeam:CanDeclareWar(iThirdPartyTeamID)) then
		return false;
	end
	
	return true;
	
end

----------------------------------------------------------------
----------------------------------------------------------------
function IsShareOpinionOnThirdPartyPlayerValid(iThirdPartyPlayer)
	-- Have we met them?
	if (not IsThirdPartyPlayerValid(iThirdPartyPlayer)) then
		return false;
	end

	return true;
end

----------------------------------------------------------------
----------------------------------------------------------------
function AddLeaderButton( iLeaderId, sLeaderName )
    instanceControls = g_InstanceManager:GetInstance();
    instanceControls.Button:SetVoid1( iLeaderId ); -- leader ID
    instanceControls.LeaderButtonLabel:SetText( sLeaderName );
    instanceControls.Button:RegisterCallback( Mouse.eLClick, OnLeaderSelect );
end

----------------------------------------------------------------
----------------------------------------------------------------
function OnCloseLeaderPanelButton()
	g_iInvokedDiscussionMode = g_iModeDiscussionRoot;
	
	-- Re-enable normal buttons
	Controls.Button1:SetDisabled(false);
	Controls.Button2:SetDisabled(false);
	Controls.Button3:SetDisabled(false);
	Controls.Button4:SetDisabled(false);
	Controls.Button5:SetDisabled(false);
	Controls.Button6:SetDisabled(false);
	Controls.Button7:SetDisabled(false);
	Controls.Button8:SetDisabled(false);
	Controls.Button9:SetDisabled(false);
	Controls.Button10:SetDisabled(false);
	Controls.Button11:SetDisabled(false);
	
    Controls.LeaderPanel:SetHide( true );
end
Controls.CloseLeaderPanelButton:RegisterCallback( Mouse.eLClick, OnCloseLeaderPanelButton );

----------------------------------------------------------------
----------------------------------------------------------------
function OnLeaderSelect( iLeaderId )
    g_CurrentTopic = iLeaderId;
    
    Controls.LeaderPanel:SetHide( true );
    
    -- Working Against discussion mode
	--if (g_iInvokedDiscussionMode == g_iModeDiscussionWorkAgainst) then
	    --Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DENOUNCE, g_iAIPlayer, iLeaderId, -1 );
	--end
    
    -- War discussion mode
	if (g_iInvokedDiscussionMode == g_iModeDiscussionWar) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_COOP_WAR_OFFER, g_iAIPlayer, iLeaderId, -1 );
	end

	-- Sharing opinion mode
	if (g_iInvokedDiscussionMode == g_iModeDiscussionShareOpinion) then
	    Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_SHARE_OPINION, g_iAIPlayer, iLeaderId, -1 );
	end
	
end


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnLeavingLeader()
    UIManager:DequeuePopup( ContextPtr );
end
Events.LeavingLeaderViewMode.Add( OnLeavingLeader );

function OnDenonceConfirmYes( )
	Controls.DenounceConfirm:SetHide(true);

	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DENOUNCE, g_iAIPlayer, 0, 0 );				
end
Controls.DenounceConfirmYes:RegisterCallback( Mouse.eLClick, OnDenonceConfirmYes );

function OnDenounceConfirmNo( )
	Controls.DenounceConfirm:SetHide(true);
end
Controls.DenounceConfirmNo:RegisterCallback( Mouse.eLClick, OnDenounceConfirmNo );

function OnWarConfirmYes( )
	Controls.WarConfirm:SetHide(true);
	
	Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR, g_iAIPlayer, 1, 0);
end
Controls.WarConfirmYes:RegisterCallback( Mouse.eLClick, OnWarConfirmYes );

function OnWarConfirmNo( )
	Controls.WarConfirm:SetHide(true);
end
Controls.WarConfirmNo:RegisterCallback( Mouse.eLClick, OnWarConfirmNo );

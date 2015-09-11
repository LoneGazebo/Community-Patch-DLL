-------------------------------------------------
-- City List
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions"  );
include( "InstanceManager" );
include( "InfoTooltipInclude" );
include( "CityStateStatusHelper" );

local m_PlayerTable = Matchmaking.GetPlayerList();
local m_PlayerNames = {};
for i = 1, #m_PlayerTable do
    m_PlayerNames[ m_PlayerTable[i].playerID ] = m_PlayerTable[i].playerName;
end

local g_LeaderButtonIM = InstanceManager:new( "LeaderButtonInstance", "LeaderButton", Controls.MajorStack );
local g_MinorCivButtonIM = InstanceManager:new( "CityStateInstance", "MinorButton", Controls.MinorStack );

local g_iPlayer = Game.GetActivePlayer();
local g_pPlayer = Players[ g_iPlayer ];
local g_iTeam = g_pPlayer:GetTeam();
local g_pTeam = Teams[ g_iTeam ];
local g_WarTarget;

local g_bAlwaysWar = Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR );
local g_bAlwaysPeace = Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE );
local g_bNoChangeWar = Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE );

local DiploRequestIncoming = Locale.ConvertTextKey( "TXT_KEY_DIPLO_REQUEST_INCOMING" );
local DiploRequestOutgoing = Locale.ConvertTextKey( "TXT_KEY_DIPLO_REQUEST_OUTGOING" );

local AlwaysWarStr = Locale.ConvertTextKey( "TXT_KEY_ALWAYS_WAR_TT" );
local DiploRequestInProgressStr = Locale.ConvertTextKey( "TXT_KEY_DIPLO_REQUEST_IN_PROGRESS" );


-----------------------------------------------------------------
-- Adjust for resolution
-----------------------------------------------------------------
local TOP_COMPENSATION = Controls.OuterGrid:GetOffsetY();
local PANEL_OFFSET = Controls.ScrollPanel:GetOffsetY() + 48;
local BOTTOM_COMPENSATION = 226;
local _, screenY = UIManager:GetScreenSizeVal();
local MAX_SIZE = screenY - (TOP_COMPENSATION + BOTTOM_COMPENSATION);

Controls.OuterGrid:SetSizeY( MAX_SIZE );
Controls.ScrollPanel:SetSizeY( MAX_SIZE - PANEL_OFFSET );

Controls.ScrollPanel:CalculateInternalSize();
Controls.OuterGrid:ReprocessAnchoring();


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
	
		if wParam == Keys.VK_ESCAPE then
    	    if( Controls.WarConfirm:IsHidden() == false ) then
	            OnNo();
	        else
    			OnClose();
    	    end
			return true;
		end
    end
end
ContextPtr:SetInputHandler( InputHandler );


----------------------------------------------------------------
----------------------------------------------------------------
function OnYes()
	Network.SendChangeWar( g_WarTarget, true);
    Controls.WarConfirm:SetHide( true );
end
Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );


----------------------------------------------------------------
----------------------------------------------------------------
function OnNo()
    Controls.WarConfirm:SetHide( true );
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );



----------------------------------------------------------------
----------------------------------------------------------------
function OnPopup( data )
	if( data.Type ~= ButtonPopupTypes.BUTTONPOPUP_DIPLOMACY ) then
	    return;
	end
	
	ContextPtr:SetHide( false );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

    

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
        UpdateDisplay();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-------------------------------------------------
function OnClose( )
    ContextPtr:SetHide( true );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


-------------------------------------------------
-------------------------------------------------
function OnDiploOverview()
	Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_DIPLOMATIC_OVERVIEW } );
end
Controls.DiplomaticOverviewButton:RegisterCallback( Mouse.eLClick, OnDiploOverview );


-------------------------------------------------
-------------------------------------------------
function OnLeagueOverview()
	Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_LEAGUE_OVERVIEW } );
end
Controls.LeagueOverviewButton:RegisterCallback( Mouse.eLClick, OnLeagueOverview );


-------------------------------------------------
-- On Minor Selected
-------------------------------------------------
function MinorSelected ()
	Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_MINOR_CIVS_LIST, } );
end


-------------------------------------------------
-- On Leader Selected
-------------------------------------------------
function LeaderSelected( ePlayer )

	if not Players[Game.GetActivePlayer()]:IsTurnActive() or Game.IsProcessingMessages() then
		return;
	end

    if( Players[ePlayer]:IsHuman() ) then
        Events.OpenPlayerDealScreenEvent( ePlayer );
    else
        
        UI.SetRepeatActionPlayer(ePlayer);
        UI.ChangeStartDiploRepeatCount(1);
    	Players[ePlayer]:DoBeginDiploWithHuman();    

	end
end


-------------------------------------------------
-- On Open Player Deal Screen
-------------------------------------------------
function OnOpenPlayerDealScreen( iOtherPlayer )

    -- any time we're legitimately opening the pvp deal screen, make sure we hide the diplolist.
    local iOtherTeam = Players[iOtherPlayer]:GetTeam();
    local iProposalTo = UI.HasMadeProposal( g_iPlayer );
   
    -- this logic should match OnOpenPlayerDealScreen in TradeLogic.lua, DiploCorner.lua, and DiploList.lua
    if( (g_pTeam:IsAtWar( iOtherTeam ) and (g_bAlwaysWar or g_bNoChangeWar) ) or	-- Always at War
	    (iProposalTo ~= -1 and iProposalTo ~= iOtherPlayer and not UI.ProposedDealExists(iOtherPlayer, g_iPlayer)) ) then -- Only allow one proposal from us at a time.
	    -- do nothing
	    return;
    else
        OnClose();
    end
end
Events.OpenPlayerDealScreenEvent.Add( OnOpenPlayerDealScreen );

-------------------------------------------------
-- On War Button Clicked
-------------------------------------------------
function OnWarButton( ePlayer )
	if (g_pTeam:CanDeclareWar(Players[ ePlayer ]:GetTeam())) then
		g_WarTarget = Players[ ePlayer ]:GetTeam();
		Controls.WarConfirm:SetHide( false );
	end	
end


-------------------------------------------------
-- Update the list of other civs
-------------------------------------------------
function UpdateDisplay()
	
	if (ContextPtr:IsHidden()) then
		return;
	end
	
	local bScenario = PreGame.GetLoadWBScenario();

	-- Clear buttons
	g_LeaderButtonIM:ResetInstances();
	
	-- Your Score Info
	local strMyScore = g_pPlayer:GetScore();
	Controls.MyScore:SetText(strMyScore);
	
	local myScoreEntries = {};
	table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_CITIES", g_pPlayer:GetScoreFromCities()));
	table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_POPULATION", g_pPlayer:GetScoreFromPopulation()));
	table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_LAND", g_pPlayer:GetScoreFromLand()));
	table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_WONDERS", g_pPlayer:GetScoreFromWonders()));
	if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
		table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_TECH", g_pPlayer:GetScoreFromTechs()));
		table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_FUTURE_TECH", g_pPlayer:GetScoreFromFutureTech()));
	end
	if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)) then
		table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_POLICIES", g_pPlayer:GetScoreFromPolicies()));
	end
	table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_GREAT_WORKS", g_pPlayer:GetScoreFromGreatWorks()));
	if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
		table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_RELIGION", g_pPlayer:GetScoreFromReligion()));
	end
	if (bScenario) then
		if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO1")) then
			table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO1", g_pPlayer:GetScoreFromScenario1()));
		end
		if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO2")) then
			table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO2", g_pPlayer:GetScoreFromScenario2()));
		end
		if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO3")) then
			table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO3", g_pPlayer:GetScoreFromScenario3()));
		end
		if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO4")) then
			table.insert(myScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO4", g_pPlayer:GetScoreFromScenario4()));
		end
	end
	
	local strMyScoreTooltip = table.concat(myScoreEntries, "[NEWLINE]");
	Controls.MyScore:SetToolTipString(strMyScoreTooltip);

	local myCivType = g_pPlayer:GetCivilizationType();
	local myCivInfo = GameInfo.Civilizations[myCivType];
	
	local myLeaderType = g_pPlayer:GetLeaderType();
	local myLeaderInfo = GameInfo.Leaders[myLeaderType];
	
	CivIconHookup( g_iPlayer, 32, Controls.MyCivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );

	local leaderDescription = myLeaderInfo.Description;
	local textBoxSize = Controls.NameBox:GetSizeX() - Controls.LeaderName:GetOffsetX();

	if(g_pPlayer:GetNickName() ~= "" and Game.IsGameMultiPlayer()) then
		TruncateString(Controls.LeaderName, textBoxSize, g_pPlayer:GetNickName()); 
	elseif(PreGame.GetLeaderName(g_iPlayer) ~= "") then
		TruncateString(Controls.LeaderName, textBoxSize, Locale.ConvertTextKey( PreGame.GetLeaderName( g_iPlayer ) ), "  (" .. Locale.ConvertTextKey( "TXT_KEY_YOU" ) .. ")");
	else
		TruncateString(Controls.LeaderName, textBoxSize, Locale.ConvertTextKey( leaderDescription ), "  (" .. Locale.ConvertTextKey( "TXT_KEY_YOU" ) .. ")");
	end
	
	if( g_pTeam:GetNumMembers() > 1 ) then
	    Controls.TeamID:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_DEFAULT_TEAM_NAME", g_pTeam:GetID() + 1 );
    else
	    Controls.TeamID:SetText( "" );
	end
	
	local textBoxSize = Controls.NameBox:GetSizeX() - Controls.CivName:GetOffsetX() - 120;
	if(PreGame.GetCivilizationShortDescription(g_iPlayer) ~= "") then
		TruncateString(Controls.CivName, textBoxSize, Locale.ConvertTextKey(PreGame.GetCivilizationShortDescription(g_iPlayer)));
	else
		TruncateString(Controls.CivName, textBoxSize, Locale.ConvertTextKey(myCivInfo.ShortDescription));
	end
	
	
	IconHookup( myLeaderInfo.PortraitIndex, 64, myLeaderInfo.IconAtlas, Controls.LeaderIcon );
	
	local iMajorMetCount = 0;
    local iProposalTo = UI.HasMadeProposal( g_iPlayer );
    --------------------------------------------------------------------
	-- Loop through all the Majors the active player knows
    --------------------------------------------------------------------
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		
		local pOtherPlayer = Players[iPlayerLoop];
		local iOtherTeam = pOtherPlayer:GetTeam();
		local pOtherTeam = Teams[ iOtherTeam ];
		
		-- Valid player? - Can't be us, and has to be alive
		if (iPlayerLoop ~= g_iPlayer and pOtherPlayer:IsAlive()) then

			-- Met this player?
			if (g_pTeam:IsHasMet(iOtherTeam)) then
			    iMajorMetCount = iMajorMetCount + 1;
				local controlTable = g_LeaderButtonIM:GetInstance();

				local primaryColor, secondaryColor = pOtherPlayer:GetPlayerColors();
				local textColor = {x = primaryColor.x, y = primaryColor.y, z = primaryColor.z, w = 1};
				local textBoxSize = controlTable.NameBox:GetSizeX() - controlTable.LeaderName:GetOffsetX();

				if(pOtherPlayer:GetNickName() ~= "" and Game.IsGameMultiPlayer() and pOtherPlayer:IsHuman()) then
					TruncateString(controlTable.LeaderName, textBoxSize, pOtherPlayer:GetNickName()); 
				else
					controlTable.LeaderName:SetText( pOtherPlayer:GetName() );
				end
				
				local civType = pOtherPlayer:GetCivilizationType();
				local civInfo = GameInfo.Civilizations[civType];
				local strCiv = Locale.ConvertTextKey(civInfo.ShortDescription);
				
				local otherLeaderType = pOtherPlayer:GetLeaderType();
				local otherLeaderInfo = GameInfo.Leaders[otherLeaderType];
				
				local leaderButtonTooltip = nil;

				controlTable.CivName:SetText(strCiv);
				CivIconHookup( iPlayerLoop, 32, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
				
				IconHookup( otherLeaderInfo.PortraitIndex, 64, otherLeaderInfo.IconAtlas, controlTable.LeaderPortrait );			
				
				-- team indicator
            	if( pOtherTeam:GetNumMembers() > 1 ) then
            	    controlTable.TeamID:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_DEFAULT_TEAM_NAME", pOtherTeam:GetID() + 1 );
            	    controlTable.TeamID:SetHide( false );
                else
            	    controlTable.TeamID:SetHide( true );
            	end

                -- Status
        	    local statusString;
				controlTable.DiploState:SetHide( false );
            	if( iOtherTeam == g_iTeam ) then
            	    -- team mate
            		local currentTech = pOtherPlayer:GetCurrentResearch();
                    if( currentTech ~= -1 and 
                        GameInfo.Technologies[currentTech] ~= nil and
                        not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
                	    statusString = "[ICON_RESEARCH] " .. Locale.ConvertTextKey( GameInfo.Technologies[currentTech].Description );
                    end
                    
            	else
    				if( g_pTeam:IsAtWar( iOtherTeam ) ) then
    					if (g_bAlwaysWar) then
    						leaderButtonTooltip = AlwaysWarStr;
    					end
        				statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" );
					elseif (pOtherPlayer:IsDenouncingPlayer(g_iPlayer)) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_DENOUNCING" );							
					elseif (pOtherPlayer:WasResurrectedThisTurnBy(g_iPlayer)) then
						statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_LIBERATED" );
    				elseif (pOtherPlayer:IsHuman() or pOtherTeam:IsHuman()) then
						controlTable.DiploState:SetToolTipString(" ");
    				else
    					local eApproachGuess = g_pPlayer:GetApproachTowardsUsGuess( iPlayerLoop );
    					
						if( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR ) then
							statusString = Locale.ConvertTextKey( "TXT_KEY_WAR_CAPS" );
						elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE ) then
							statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_HOSTILE", otherLeaderInfo.Description );
						elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED ) then
							statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_GUARDED", otherLeaderInfo.Description );
						elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID ) then
							statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_AFRAID", otherLeaderInfo.Description);
						elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY ) then
							statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_FRIENDLY", otherLeaderInfo.Description );
						elseif( eApproachGuess == MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL ) then
							statusString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_NEUTRAL", otherLeaderInfo.Description );
						end
					end
					
					local strMoodInfo = GetMoodInfo(iPlayerLoop);
					controlTable.DiploState:SetToolTipString(strMoodInfo);
				end
				
				if( statusString ~= nil ) then
    				controlTable.DiploState:SetHide( false );
            		TruncateString( controlTable.DiploState, controlTable.StatusBox:GetSizeX(), statusString );
        		else
    				controlTable.DiploState:SetHide( true );
				end
				
				-- Update diplomatic request button.
				local localPlayer = Game.GetActivePlayer();
				if(UI.ProposedDealExists(localPlayer, iPlayerLoop)) then
					-- We proposed something to them.
					controlTable.DiploWaiting:SetHide(false);
					controlTable.DiploWaiting:SetAlpha( 0.5 );
					controlTable.DiploWaiting:SetToolTipString( DiploRequestOutgoing );
				elseif(UI.ProposedDealExists(iPlayerLoop, localPlayer)) then
				  --They proposed something to us.
				  controlTable.DiploWaiting:SetHide(false);
				  controlTable.DiploWaiting:SetAlpha( 1.0 );
				  controlTable.DiploWaiting:SetToolTipString( DiploRequestIncoming );
				else
				  -- No deals in progress.
				  controlTable.DiploWaiting:SetHide(true);
				end

				controlTable.Score:SetText( pOtherPlayer:GetScore() );
				
				
				local otherScoreEntries = {};
				table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_CITIES", pOtherPlayer:GetScoreFromCities()));
				table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_POPULATION", pOtherPlayer:GetScoreFromPopulation()));
				table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_LAND", pOtherPlayer:GetScoreFromLand()));
				table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_WONDERS", pOtherPlayer:GetScoreFromWonders()));
				if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
					table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_TECH", pOtherPlayer:GetScoreFromTechs()));
					table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_FUTURE_TECH", pOtherPlayer:GetScoreFromFutureTech()));
				end
				if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES)) then
					table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_POLICIES", pOtherPlayer:GetScoreFromPolicies()));
				end
				table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_GREAT_WORKS", pOtherPlayer:GetScoreFromGreatWorks()));
				if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION)) then
					table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_RELIGION", pOtherPlayer:GetScoreFromReligion()));
				end
				if (bScenario == true) then
					if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO1")) then
						table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO1", pOtherPlayer:GetScoreFromScenario1()));
					end
					
					if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO2")) then
						table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO2", pOtherPlayer:GetScoreFromScenario2()));
					end
					
					if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO3")) then
						table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO3", pOtherPlayer:GetScoreFromScenario3()));
					end
					
					if(Locale.HasTextKey("TXT_KEY_DIPLO_MY_SCORE_SCENARIO4")) then
						table.insert(otherScoreEntries, Locale.Lookup("TXT_KEY_DIPLO_MY_SCORE_SCENARIO4", pOtherPlayer:GetScoreFromScenario4()));
					end
				end
				
				local strOtherScoreTooltip = table.concat(otherScoreEntries, "[NEWLINE]");
				controlTable.Score:SetToolTipString(strOtherScoreTooltip);

				controlTable.LeaderButton:SetVoid1( iPlayerLoop ); -- indicates type
				controlTable.LeaderButton:RegisterCallback( Mouse.eLClick, LeaderSelected );


                if( pOtherPlayer:IsHuman() ) then
                    -- don't open trade if we're at war and war status cannot be changed
                    if( not( g_pTeam:IsAtWar( pOtherPlayer:GetTeam() ) and (g_bAlwaysWar or g_bNoChangeWar) ) ) then                    
                        controlTable.LeaderButton:SetDisabled( true );
                    else
                        controlTable.LeaderButton:SetDisabled( false );
                    end
                    
    				-- Show or hide war button if it's a human
    				if( not g_bAlwaysWar and not g_bAlwaysPeace and not g_bNoChangeWar and
    				    not g_pTeam:IsAtWar( pOtherPlayer:GetTeam()) and g_pTeam:CanDeclareWar(pOtherPlayer:GetTeam()) and
    				    g_iTeam ~= iOtherTeam ) then
    					controlTable.WarButton:SetHide(false);
    					
        				controlTable.WarButton:SetVoid1( iPlayerLoop ); -- indicates type
        				controlTable.WarButton:RegisterCallback( Mouse.eLClick, OnWarButton );
    				else
    					controlTable.WarButton:SetHide(true);
    				end
    			else
    				controlTable.WarButton:SetHide(true);    			
				end
				
				controlTable.StatusStack:CalculateSize();
				controlTable.StatusStack:ReprocessAnchoring();


				-----------------------------------------------------------------------------
			    -- disable the button if this is a human, and we have a pending deal, and
			    -- the deal is not with this player
				-----------------------------------------------------------------------------
				local bCanOpenDeal = true;
				
				if(iProposalTo ~= -1 and not UI.ProposedDealExists(localPlayer, iPlayerLoop) and not UI.ProposedDealExists(iPlayerLoop, localPlayer)) then
				    bCanOpenDeal = false;
				end
				controlTable.LeaderButton:SetDisabled( not bCanOpenDeal );
				if(not bCanOpenDeal) then
					leaderButtonTooltip = DiploRequestInProgressStr;
				end

				-- Set diplo list tooltip.
				controlTable.LeaderButton:SetToolTipString(leaderButtonTooltip);
			end
		end
	end

	if( iMajorMetCount > 0 ) then
		Controls.MajorButton:SetHide( false );
	else
		Controls.MajorButton:SetHide( true );
	end
	
	InitMinorCivList();
	
	Controls.MinorStack:CalculateSize();
	Controls.MajorStack:CalculateSize();
	
	RecalcPanelSize();
end
Events.SerialEventScoreDirty.Add( UpdateDisplay );
Events.SerialEventCityInfoDirty.Add(UpdateDisplay);
Events.SerialEventGameDataDirty.Add(UpdateDisplay);
Events.MultiplayerGamePlayerDisconnected.Add(UpdateDisplay);
   
-------------------------------------------------
-- Look for the CityStates we've met	
-------------------------------------------------
function InitMinorCivList()
	-- Clear buttons
	g_MinorCivButtonIM:ResetInstances();
	
    -------------------------------------------------
    -- Look for the CityStates we've met	
    -------------------------------------------------
	local iMinorMetCount = 0;
	
	for iPlayerLoop = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS-1, 1 do
		
		pOtherPlayer = Players[iPlayerLoop];
		iOtherTeam = pOtherPlayer:GetTeam();
		
		if( pOtherPlayer:IsMinorCiv() and 
		    g_iTeam ~= iOtherTeam     and 
		    pOtherPlayer:IsAlive()    and
			g_pTeam:IsHasMet( iOtherTeam ) ) then

			-- Update colors
			local _, primaryColor = pOtherPlayer:GetPlayerColors();
			local color = Vector4(primaryColor.x, primaryColor.y, primaryColor.z, 1);
			
			
			iMinorMetCount = iMinorMetCount + 1;
			local controlTable = g_MinorCivButtonIM:GetInstance();
			
			local minorCivType = pOtherPlayer:GetMinorCivType();
			local civInfo = GameInfo.MinorCivilizations[minorCivType];
			
			controlTable.MinorName:SetText( Locale.ConvertTextKey( civInfo.Description ) );
			controlTable.MinorName:SetColor( color, 0 );

			local strDiploState = "";
			if (g_pTeam:IsAtWar(iOtherTeam)) then
			    if (g_bAlwaysWar) then
    				controlTable.MinorButton:SetToolTipString(Locale.ConvertTextKey("TXT_KEY_ALWAYS_WAR_TT"));
    			end
			
				strDiploState = Locale.ConvertTextKey( "TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" )
			end
			controlTable.StatusText:SetText( strDiploState);
			
        	local iTrait = pOtherPlayer:GetMinorCivTrait();
        	if( iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_CULTURED ) then
        		controlTable.MinorType:LocalizeAndSetText( "TXT_KEY_CITY_STATE_CULTURED_ADJECTIVE" );
        	elseif( iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC ) then
        		controlTable.MinorType:LocalizeAndSetText( "TXT_KEY_CITY_STATE_MILITARISTIC_ADJECTIVE" );
        	elseif( iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_MARITIME ) then
        		controlTable.MinorType:LocalizeAndSetText( "TXT_KEY_CITY_STATE_MARITIME_ADJECTIVE" );
        	elseif(iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_MERCANTILE) then
        		controlTable.MinorType:LocalizeAndSetText( "TXT_KEY_CITY_STATE_MERCANTILE_ADJECTIVE" );
        	elseif(iTrait == MinorCivTraitTypes.MINOR_CIV_TRAIT_RELIGIOUS) then
        		controlTable.MinorType:LocalizeAndSetText( "TXT_KEY_CITY_STATE_RELIGIOUS_ADJECTIVE" );
        	end

			civType = pOtherPlayer:GetCivilizationType();
			civInfo = GameInfo.Civilizations[civType];

			IconHookup( civInfo.PortraitIndex, 32, civInfo.AlphaIconAtlas, controlTable.LeaderPortrait );
			controlTable.LeaderPortrait:SetColor( color );
					
			controlTable.MinorButton:SetVoid1( iPlayerLoop );
			controlTable.MinorButton:SetVoid2(  pOtherPlayer:GetCapitalCity() );
			controlTable.MinorButton:RegisterCallback( Mouse.eLClick, MinorSelected );
			controlTable.QuestIcon:SetVoid1( iPlayerLoop );
			controlTable.QuestIcon:RegisterCallback( Mouse.eLClick, OnQuestIconClicked );
			
			local bWar = Teams[Game.GetActiveTeam()]:IsAtWar(pOtherPlayer:GetTeam());
			
			local sMinorCivType = pOtherPlayer:GetMinorCivType();
			local strStatusTT = GetCityStateStatusToolTip(g_iPlayer, iPlayerLoop, true);
			local trait = GameInfo.MinorCivilizations[sMinorCivType].MinorCivTrait;
			UpdateCityStateStatusUI(g_iPlayer, iPlayerLoop, controlTable.PositiveStatusMeter, controlTable.NegativeStatusMeter, controlTable.StatusMeterMarker, controlTable.StatusIconBG);
			controlTable.StatusIcon:SetTexture(GameInfo.MinorCivTraits[trait].TraitIcon);
			controlTable.StatusIcon:SetColor( {x=1, y=1, z=1, w=1 } );
			if (GetCityStateStatusType(g_iPlayer, iPlayerLoop) == "MINOR_FRIENDSHIP_STATUS_NEUTRAL" and color) then
				controlTable.StatusIcon:SetColor(color);
			end
			controlTable.StatusIconBG:SetToolTipString(strStatusTT);
			
			-- Ally Status
			local iAlly = pOtherPlayer:GetAlly();
			local bHideIcon = true;
			if (iAlly ~= nil and iAlly ~= -1) then
				if (iAlly == g_iPlayer or Teams[Players[iAlly]:GetTeam()]:IsHasMet(g_iTeam)) then
					bHideIcon = false;
					CivIconHookup(iAlly, 32, controlTable.AllyIcon, controlTable.AllyIconBG, controlTable.AllyIconShadow, false, true);
				else
					bHideIcon = false;
					CivIconHookup(-1, 32, controlTable.AllyIcon, controlTable.AllyIconBG, controlTable.AllyIconShadow, false, true);
				end
			end
			local strAllyTT = GetAllyToolTip(g_iPlayer, iPlayerLoop);
			controlTable.AllyIcon:SetToolTipString(strAllyTT);
			controlTable.AllyIconBG:SetToolTipString(strAllyTT);
			controlTable.AllyIconShadow:SetToolTipString(strAllyTT);
			controlTable.AllyIconContainer:SetHide(bHideIcon);
			
			
			------------------------------------------------------
    		-- Quests
    	
    		-- Hide the quest icon if there are no quests OR the City State is at war with you.
    	    if( ( pOtherPlayer:GetMinorCivNumDisplayedQuestsForPlayer(g_iPlayer) == 0 and 
    				not pOtherPlayer:IsThreateningBarbariansEventActiveForPlayer(g_iPlayer) and
    				not pOtherPlayer:IsProxyWarActiveForMajor(g_iPlayer))
    			or g_pTeam:IsAtWar(iOtherTeam)) then
    			controlTable.QuestIcon:SetHide( true );
    	    else
    			local sIconText = GetActiveQuestText(g_iPlayer, iPlayerLoop);
    			local sToolTipText = GetActiveQuestToolTip(g_iPlayer, iPlayerLoop);
    			
    			controlTable.QuestIcon:SetHide( false );
    			controlTable.QuestIcon:SetText(sIconText);
    			controlTable.QuestIcon:SetToolTipString(sToolTipText);
    		end
		end
	end		

	if( iMinorMetCount > 0 ) then
		Controls.MinorButton:SetHide( false );
	else
		Controls.MinorButton:SetHide( true );
	end
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function MinorSelected( PlayerID )
	Events.SerialEventGameMessagePopup( { Type = ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_DIPLO, Data1 = PlayerID; } );
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnQuestIconClicked( PlayerID )
	local pMinor = Players[PlayerID];
	if (pMinor) then
		if (pMinor:IsMinorCivDisplayedQuestForPlayer(g_iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP)) then
			local iQuestData1 = pMinor:GetQuestData1(g_iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP);
			local iQuestData2 = pMinor:GetQuestData2(g_iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP);
			local pPlot = Map.GetPlot(iQuestData1, iQuestData2);
			if (pPlot) then
				UI.LookAt(pPlot, 0);
				local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));
				Events.GameplayFX(hex.x, hex.y, -1);
			end
		end
		if (pMinor:IsMinorCivDisplayedQuestForPlayer(g_iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY)) then
			local iQuestData1 = pMinor:GetQuestData1(g_iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY);
			local iQuestData2 = pMinor:GetQuestData2(g_iPlayer, MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY);
			local pPlot = Map.GetPlot(iQuestData1, iQuestData2);
			if (pPlot) then
				UI.LookAt(pPlot, 0);
				local hex = ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY()));
				Events.GameplayFX(hex.x, hex.y, -1);
			end
		end
	end
end

-----------------------------------------------------------------
-----------------------------------------------------------------
function RecalcPanelSize()
	Controls.OuterStack:CalculateSize();
	local size = math.min( MAX_SIZE, Controls.OuterStack:GetSizeY() + 290 );
    Controls.OuterGrid:SetSizeY( size );
    Controls.ScrollPanel:SetSizeY( size - PANEL_OFFSET );
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end


-----------------------------------------------------------------
-----------------------------------------------------------------
function OnMajorsButton()
    if( Controls.MajorStack:IsHidden() ) then
        Controls.MajorButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "{TXT_KEY_CIVILIZATION_SECTION_1:upper}" ) );
        Controls.MajorStack:SetHide( false );
    else
        Controls.MajorButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "{TXT_KEY_CIVILIZATION_SECTION_1:upper}" ) );
        Controls.MajorStack:SetHide( true );
    end
    
    RecalcPanelSize();
end
Controls.MajorButton:RegisterCallback( Mouse.eLClick, OnMajorsButton );


-----------------------------------------------------------------
-----------------------------------------------------------------
function OnMinorsButton()
    if( Controls.MinorStack:IsHidden() ) then
        Controls.MinorButton:SetText( "[ICON_MINUS]" .. Locale.ConvertTextKey( "{TXT_KEY_PEDIA_CATEGORY_11_LABEL:upper}" ) );
        Controls.MinorStack:SetHide( false );
    else
        Controls.MinorButton:SetText( "[ICON_PLUS]" .. Locale.ConvertTextKey( "{TXT_KEY_PEDIA_CATEGORY_11_LABEL:upper}" ) );
        Controls.MinorStack:SetHide( true );
    end
    
    RecalcPanelSize();
end
Controls.MinorButton:RegisterCallback( Mouse.eLClick, OnMinorsButton );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnDiploListActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	g_iPlayer = Game.GetActivePlayer();
	g_pPlayer = Players[ g_iPlayer ];
	g_iTeam = g_pPlayer:GetTeam();
	g_pTeam = Teams[ g_iTeam ];
end
Events.GameplaySetActivePlayer.Add(OnDiploListActivePlayerChanged);

OnMajorsButton();
OnMinorsButton();


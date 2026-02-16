if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "IconSupport" );
include( "InstanceManager" );
include( "SupportFunctions"  );

local g_InstanceManager = InstanceManager:new( "DiploVoteButton", "Button", Controls.ButtonStack );
local m_PopupInfo = nil;

-------------------------------------------------
-- On Diplo Vote Chosen
-------------------------------------------------
function DoDiploVoteChosen( iVotePlayer )
    Network.SendDiploVote(iVotePlayer);
    
    OnClose();
    --ContextPtr:SetHide( true );
end

-------------------------------------------------
-- Popup construction
-------------------------------------------------
function OnPopupMessage(popupInfo)
	
	local type = popupInfo.Type;
	if(type ~= ButtonPopupTypes.BUTTONPOPUP_DIPLO_VOTE ) then
		return;
	end

	m_PopupInfo = popupInfo;

    g_InstanceManager:ResetInstances();
    
    local pPlayer = Players[Game.GetActivePlayer()];
    local pTeam = Teams[pPlayer:GetTeam()];
	
	local pOtherPlayer;
	local iPlayerLoop;
	
	
	local bIsTeamGame = false;
	for iTeamLoop = 0, GameDefines.MAX_CIV_TEAMS - 1, 1 do
		local pOtherTeam = Teams[iTeamLoop];
		if(pOtherTeam:IsEverAlive() and pOtherTeam:GetNumMembers() > 1) then
			bIsTeamGame = true;
			break;
		end
	end
	
	
	for iTeamLoop = 0, GameDefines.MAX_CIV_TEAMS-1, 1 do
		
		pOtherTeam = Teams[iTeamLoop];
		if(pOtherTeam ~= nil) then
			iPlayerLoop = pOtherTeam:GetLeaderID();
			pOtherPlayer = Players[iPlayerLoop];

			-- Valid player? - Has to be alive
			if (pOtherPlayer ~= nil and pOtherPlayer:IsAlive()) then
				-- Met this player?
				if (pTeam:IsHasMet(iTeamLoop)) then
					-- Not minor
					if (not pOtherPlayer:IsMinorCiv()) then
						-- Not ourselves
						if (pOtherTeam:GetID() ~= pTeam:GetID()) then
						
							local controlTable = g_InstanceManager:GetInstance();
							CivIconHookup( iPlayerLoop, 45, controlTable.CivIcon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true );
				
							local leader = GameInfo.Leaders[pOtherPlayer:GetLeaderType()];
							local leaderDescription = leader.Description;
							local textBoxSize = controlTable.Button:GetSizeX() - controlTable.ButtonText:GetOffsetX();

							if(pOtherPlayer:GetNickName() ~= "" and Game:IsNetworkMultiPlayer()) then
								TruncateString(controlTable.ButtonText, textBoxSize, pOtherPlayer:GetNickName());
							
							elseif(pOtherTeam:GetID() == pTeam:GetID())then 
							
								if(not bIsTeamGame) then
									if(PreGame.GetLeaderName(Game.GetActivePlayer()) ~= "") then
										TruncateString(controlTable.ButtonText, textBoxSize, Locale.ConvertTextKey( PreGame.GetLeaderName( Game.GetActivePlayer() ) ), "  (" .. Locale.ConvertTextKey( "TXT_KEY_YOU" ) .. ")");
									else
										TruncateString(controlTable.ButtonText, textBoxSize, Locale.ConvertTextKey( leaderDescription ), "  (" .. Locale.ConvertTextKey( "TXT_KEY_YOU" ) .. ")");
									end
								else
									TruncateString(controlTable.ButtonText, textBoxSize, Locale.ConvertTextKey("TXT_KEY_POP_UN_TEAM_LABEL", pOtherTeam:GetID() + 1) , " (" .. Locale.ConvertTextKey("TXT_KEY_POP_VOTE_RESULTS_YOUR_TEAM") .. ")");
								end
								
							else
								
								if(not bIsTeamGame) then
									TruncateString(controlTable.ButtonText, textBoxSize, Locale.ConvertTextKey( leaderDescription ));
								else
									TruncateString(controlTable.ButtonText, textBoxSize, Locale.ConvertTextKey("TXT_KEY_POP_UN_TEAM_LABEL", pOtherTeam:GetID() + 1));
								end
							end
							
							controlTable.Button:SetVoid1( iPlayerLoop );
							controlTable.Button:RegisterCallback( Mouse.eLClick, DoDiploVoteChosen);
						end
					end
				end
			end
		end
	end
	
    Controls.ButtonStack:CalculateSize();
    Controls.ButtonStack:ReprocessAnchoring();
    Controls.ScrollPanel:CalculateInternalSize();
    
	UIManager:QueuePopup( ContextPtr, PopupPriority.DiploVote );
end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose ()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_DIPLO_VOTE, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);

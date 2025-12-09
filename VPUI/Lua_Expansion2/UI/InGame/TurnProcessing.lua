if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Turn Processing Popup
-------------------------------------------------
include( "IconSupport" );
include( "SupportFunctions" );

local ms_IsShowingMinorCiv = false;
------------------------------------------------------------
------------------------------------------------------------
function GetPlayer (iPlayerID)
	if (iPlayerID < 0) then
		return nil;
	end

	if (Players[iPlayerID]:IsHuman()) then
		return nil;
	end;

	return Players[iPlayerID];
end

-------------------------------------------------
-- OnAITurnStart
-------------------------------------------------
function OnAITurnStart(iPlayerID, szTag)
	
	if(PreGame.IsMultiplayerGame()) then
		-- Turn Queue UI (see ActionInfoPanel.lua) replaces the turn processing UI in multiplayer.  
		return;
	end

	if( ContextPtr:IsHidden() ) then
		ContextPtr:SetHide( false );
		Controls.Anim:SetHide( true );
		ms_IsShowingMinorCiv = false;
	end
	
	local player = GetPlayer(iPlayerID);
	if (player == nil) then
		return;	
	end

	if (not player:IsTurnActive()) then
		return;
	end

	-- Determine if the local player has met this player, else we will just a generic processing message
	
	local pLocalTeam = Teams[ Players[ Game.GetActivePlayer() ]:GetTeam() ];
    local bMet = pLocalTeam:IsHasMet( player:GetTeam() );

	local bIsMinorCiv = player:IsMinorCiv();
	
	if (bIsMinorCiv and ms_IsShowingMinorCiv) then
		-- If we are already showing the Minor Civ processing, just exit.  We don't show them individually because they are usually quick to process
		return;
	end

	local civDescription;
	local bIsBarbarian = player:IsBarbarian();
	if (bIsBarbarian and Game.IsOption(GameOptionTypes.GAMEOPTION_NO_BARBARIANS)) then
		-- Even if there are no barbarians, we will get this call, just skip out if they are turned off
		return;
	end
	-- If we have met them, or it is a minor civ or it is the barbarians, show it.
	if (bMet or bIsMinorCiv or bIsBarbarian) then
		-- Set Civ Icon
        Controls.CivIconBG:SetHide( false );
        Controls.CivIconShadow:SetHide( false );
		CivIconHookup(iPlayerID, 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true); 
			
		if (player:IsMinorCiv()) then
			civDescription = Locale.ConvertTextKey( "TXT_KEY_PROCESSING_MINOR_CIVS" );
			ms_IsShowingMinorCiv = true;
		else
			local civType = player:GetCivilizationType();
			local civInfo = GameInfo.Civilizations[civType];
			local strCiv = Locale.ConvertTextKey(civInfo.ShortDescription);
			ms_IsShowingMinorCiv = false;
			if(strCiv and #strCiv > 0) then
				civDescription = Locale.ConvertTextKey(strCiv);
			else	
				civDescription = Locale.ConvertTextKey( "TXT_KEY_MULTIPLAYER_DEFAULT_PLAYER_NAME", iPlayerID + 1 );
			end
				
			civDescription = Locale.ConvertTextKey( "TXT_KEY_PROCESSING_TURN_FOR", civDescription );
		end
	else
		civDescription = Locale.ConvertTextKey( "TXT_KEY_PROCESSING_TURN_FOR_UNMET_PLAYER", iPlayerID + 1 );
		Controls.CivIcon:SetTexture("CivSymbolsColor512.dds");
        Controls.CivIcon:SetTextureOffsetVal( 448 + 7, 128 + 7 );
        Controls.CivIcon:SetColor( Vector4( 1.0, 1.0, 1.0, 1.0 ) );
        Controls.CivIcon:SetHide( false );
        Controls.CivIconBG:SetHide( true );
        Controls.CivIconShadow:SetHide( true );
        ms_IsShowingMinorCiv = false;
	end		
	if (Controls.Anim:IsHidden()) then
		Controls.Anim:SetHide( false );
		Controls.Anim:BranchResetAnimation();
	end
	Controls.TurnProcessingTitle:SetText(civDescription);

end
Events.AIProcessingStartedForPlayer.Add( OnAITurnStart );
-------------------------------------------------------------------------
-- OnPlayerTurnStart
-- Human player's turn, hide the UI
-------------------------------------------------------------------------
function OnPlayerTurnStart()
	if (not ContextPtr:IsHidden()) then
		Controls.Anim:Reverse();
		Controls.Anim:Play();
	end
end
Events.ActivePlayerTurnStart.Add( OnPlayerTurnStart );
Events.RemotePlayerTurnStart.Add( OnPlayerTurnStart );

-------------------------------------------------------------------------
-- Callback while the alpha animation is playing.
-- It will also be called once, when the animation stops.
function OnAlphaAnim()
	if (Controls.Anim:IsStopped() and Controls.Anim:GetAlpha() == 0.0) then
		Controls.Anim:SetHide( true );
		ContextPtr:SetHide( true );
		print("Hiding TurnProcessing");
	end
end
Controls.Anim:RegisterAnimCallback( OnAlphaAnim );
-------------------------------------------------
-- Technology Award Popup
-------------------------------------------------
include( "InstanceManager" );
include( "TechButtonInclude" );
include( "TechHelpInclude" );

local maxSmallButtons = 5;

-- list any localized strings that we are going to need a bunch

-- I'll need these before I'm done
-- KWG: Not all these are used.  Are they referenced in the includes?
local playerID = Game.GetActivePlayer();	
local player = Players[playerID];
local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
local activeTeamID = Game.GetActiveTeam();
local activeTeam = Teams[activeTeamID];

local storedPopupInfo = {};

GatherInfoAboutUniqueStuff( civType );

local m_PopupInfo = nil;

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnPopup( popupInfo )

	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_TECH_AWARD then
		return;
	end
	
	if popupInfo.Data3 == -1 then
		return;
	end

	m_PopupInfo = popupInfo;

	local pTechInfo = GameInfo.Technologies[popupInfo.Data3];
	if pTechInfo == nil then
		return;
	end

	storedPopupInfo = popupInfo;

  	RefreshDisplay( pTechInfo );
  	
	UIManager:QueuePopup( ContextPtr, PopupPriority.TechAward );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function RefreshDisplay( pTechInfo )
	if pTechInfo then
	
		local playerID = Game.GetActivePlayer();	
		local player = Players[playerID];
		
		-- Tech Name
		Controls.TechName:SetText( Locale.ConvertTextKey( pTechInfo.Description ) );
		
		-- Tech Quote
		Controls.TechQuote:SetText( Locale.ConvertTextKey( pTechInfo.Quote ) );
		
		-- if we have one, update the tech picture
		if IconHookup( pTechInfo.PortraitIndex, 128, pTechInfo.IconAtlas, Controls.TechIcon ) then
			Controls.TechIcon:SetHide( false );
		else
			Controls.TechIcon:SetHide( true );
		end
		
		-- Update the little icons
		techPediaSearchStrings = {};
		AddSmallButtonsToTechButton( Controls, pTechInfo, maxSmallButtons, 64 );
		Controls.BonusStack:CalculateSize();
		Controls.BonusStack:ReprocessAnchoring();
		-- if the code ever supports first place awards add a handler here
		
		-- Tech Info
		Controls.TechHelp:SetText( GetShortHelpTextForTech(pTechInfo.ID, false) );
		
		Controls.OuterGrid:DoAutoSize();
	end	
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnContinueButtonClicked ()
	--Events.SerialEventGameMessagePopup( 
	--{
		--Type = ButtonPopupTypes.BUTTONPOPUP_CHOOSETECH; 
		--Data1 = storedPopupInfo.Data1; 
		--Data2 = storedPopupInfo.Data2; 
		--Data3 = storedPopupInfo.Data3; 
	--} );
	
	OnClose();
end
Controls.ContinueButton:RegisterCallback( Mouse.eLClick, OnContinueButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_TECH_AWARD, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnTechAwardActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	playerID = Game.GetActivePlayer();	
	player = Players[playerID];
	civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
	activeTeamID = Game.GetActiveTeam();
	activeTeam = Teams[activeTeamID];
	
	GatherInfoAboutUniqueStuff( civType );
	
	-- Make sure its closed so the next player doesn't see the previous players popup.	
	OnClose();
end

Events.GameplaySetActivePlayer.Add(OnTechAwardActivePlayerChanged);
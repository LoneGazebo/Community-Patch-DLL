include( "IconSupport" );
include( "UniqueBonuses" );
include( "ContentSwitch" ); -- Includes tips for Vox Populi

local iCivID = -1;
local g_bLoadComplete;

function ShowHide( isHide, isInit )
	if ( not isInit ) then
		if ( isHide == true ) then
			UIManager:SetUICursor( 0 );
			Controls.Image:UnloadTexture();
			--print("Texture is unloaded");
			if (iCivID ~= -1) then
				Events.SerialEventDawnOfManHide(iCivID);
			end
		else
			OnInitScreen();
			UIManager:SetUICursor( 1 );
			if (iCivID ~= -1) then
				Events.SerialEventDawnOfManShow(iCivID);
			end
		end
	end

	if(not isHide) then
		UI.SetDontShowPopups(true);
	end
end
ContextPtr:SetShowHideHandler( ShowHide );

Controls.ProgressBar:SetPercent( 1 );

function OnInitScreen()

	g_bLoadComplete = false;

    Controls.AlphaAnim:SetToBeginning();
    Controls.SlideAnim:SetToBeginning();
	Controls.ActivateButton:SetHide(true);

	local civIndex = PreGame.GetCivilization( Game:GetActivePlayer() );

    local civ = GameInfo.Civilizations[civIndex];

    if(civ == nil) then
		PreGame.SetCivilization(0, -1);
	end

    if ( not PreGame.IsMultiplayerGame() ) then
		-- Force some settings off when loading a HotSeat game.
        PreGame.SetGameOption("GAMEOPTION_DYNAMIC_TURNS", false);
        PreGame.SetGameOption("GAMEOPTION_SIMULTANEOUS_TURNS", false);
        PreGame.SetGameOption("GAMEOPTION_PITBOSS", false);
    end

    -- Sets up Selected Civ Slot
    if( civ ~= nil ) then

        -- Use the Civilization_Leaders table to cross reference from this civ to the Leaders table
        local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType];
        local leaderDescription = leader.Description;

		-- Set Leader & Civ Text
		Controls.Civilization:LocalizeAndSetText( civ.Description );
		Controls.Leader:LocalizeAndSetText( leaderDescription );

        -- Set Civ Leader Icon
		IconHookup( leader.PortraitIndex, 128, leader.IconAtlas, Controls.Portrait );

		-- Set Civ Icon
		SimpleCivIconHookup( Game.GetActivePlayer(), 80, Controls.IconShadow );

		-- Sets Trait bonus Text
        local leaderTrait = GameInfo.Leader_Traits("LeaderType ='" .. leader.Type .. "'")();
        local trait = leaderTrait.TraitType;
        Controls.BonusTitle:SetText( Locale.ConvertTextKey( GameInfo.Traits[trait].ShortDescription ));
        Controls.BonusDescription:SetText( Locale.ConvertTextKey( GameInfo.Traits[trait].Description ));
		Controls.Title:SetText(VPTipTitle()) -- Vox Populi tip title
		Controls.Tips:SetText(VPTipText()) -- Vox Populi tip content

         -- Sets Bonus Icons
        local bonusText = PopulateUniqueBonuses( Controls, civ, leader, false, true);

        Controls.BonusUnit:LocalizeAndSetText( bonusText[1] or "" );
        Controls.BonusBuilding:LocalizeAndSetText( bonusText[2] or "" );

        -- Sets Dawn of Man Quote
        Controls.Quote:LocalizeAndSetText(civ.DawnOfManQuote or "");

        -- Sets Dawn of Man Image
        Controls.Image:SetTexture(civ.DawnOfManImage);
		iCivID = civ.ID;
        --print("iCivID: " .. iCivID);
	end


end

function OnActivateButtonClicked ()
	--print("Activate button clicked!");
	Events.LoadScreenClose();
	if (not PreGame.IsMultiplayerGame() and not PreGame.IsHotSeatGame()) then
		Game.SetPausePlayer(-1);
	end

	UI.SetDontShowPopups(false);

	--UI.SetNextGameState( GameStates.MainGameView, g_iAIPlayer );
end
Controls.ActivateButton:RegisterCallback( Mouse.eLClick, OnActivateButtonClicked );


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN ) then
			if (g_bLoadComplete) then
				OnActivateButtonClicked();
			end
        end
    end
    return true;
end
ContextPtr:SetInputHandler( InputHandler );

function HideBackgrounds ()
	Controls.AlphaAnim:Play();
	Controls.SlideAnim:Play();
end

function OnSequenceGameInitComplete ()

	g_bLoadComplete = true;

	if (PreGame.IsMultiplayerGame() or PreGame.IsHotSeatGame()) then
		OnActivateButtonClicked();
	else
		Game.SetPausePlayer(Game.GetActivePlayer());
		local strGameButtonName;
		if (not UI:IsLoadedGame()) then
			strGameButtonName = Locale.ConvertTextKey("TXT_KEY_BEGIN_GAME_BUTTON");
		else
			strGameButtonName = Locale.ConvertTextKey("TXT_KEY_BEGIN_GAME_BUTTON_CONTINUE");
		end

		Controls.ActivateButtonText:SetText(strGameButtonName);
		Controls.ActivateButton:SetHide(false);
		HideBackgrounds();
        UIManager:SetUICursor( 0 );

        -- Update Icons to now have tooltips.
        local civIndex = PreGame.GetCivilization( Game:GetActivePlayer() );
        local civ = GameInfo.Civilizations[civIndex];

		-- Sets up Selected Civ Slot
		if( civ ~= nil ) then

			-- Use the Civilization_Leaders table to cross reference from this civ to the Leaders table
			local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType];

			 -- Sets Bonus Icons
			local bonusText = PopulateUniqueBonuses( Controls, civ, leader, true, false);
		end
	end
end

Events.SequenceGameInitComplete.Add( OnSequenceGameInitComplete );
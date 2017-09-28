-------------------------------------------------
-- Vassalage
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "TechHelpInclude" );

local m_PopupInfo = nil;

print("This is the VassalageOverview.lua for C4DF - CP");

local g_VassalButtonIM = InstanceManager:new( "VassalButtonInstance", "VassalButton", Controls.VassalStack );

local g_iPlayer = Game.GetActivePlayer();
local g_pPlayer = Players[ g_iPlayer ];
local g_iTeam = g_pPlayer:GetTeam();
local g_pTeam = Teams[ g_iTeam ];

local g_IdeologyBackgrounds = {
	[0] = "loading_20.dds",
	POLICY_BRANCH_AUTOCRACY = "loading_13.dds",
	POLICY_BRANCH_FREEDOM = "loadingbasegame_19.dds",
	POLICY_BRANCH_ORDER = "loadingbasegame_7.dds",
}

local g_SelectedVassal = nil;
local g_SelectedMaster = nil;

-----------------------------------------------------------------
-- Add Vassal Overview to Dropdown (if enabled)
-----------------------------------------------------------------
LuaEvents.AdditionalInformationDropdownGatherEntries.Add( function(entries)
	if(not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_VASSALAGE)) then
		table.insert(entries, {
			text = Locale.Lookup("TXT_KEY_VO"),
			call = function()
				Events.SerialEventGameMessagePopup {
					Type = ButtonPopupTypes.BUTTONPOPUP_MODDER_11,
				};
			end,
		});
		
	end
end);

function ResetVassalDescription()
	Controls.VassalDescBox:SetHide(true);
	Controls.MasterDescBox:SetHide(true);
end

function Update()
	-- Update active player
	g_iPlayer = Game.GetActivePlayer();
	g_pPlayer = Players[ g_iPlayer ];
	g_iTeam = g_pPlayer:GetTeam();
	g_pTeam = Teams[ g_iTeam ];

	function TestVassalageStarted()
		return (g_pTeam:GetCurrentEra() >= Game.GetVassalageEnabledEra()) or g_pTeam:IsVassalOfSomeone();
	end

	local bVassalStarted = TestVassalageStarted();

	Controls.LabelNoVassals:SetHide( g_pTeam:GetNumVassals() ~= 0 or g_pTeam:IsVassalOfSomeone() );
	Controls.LabelVassalNotStartedYet:SetHide(bVassalStarted);
	Controls.OverviewPanel:SetHide(not bVassalStarted);
	Controls.NumVassals:SetHide(not bVassalStarted);
	Controls.NumVassalsWorld:SetHide(not bVassalStarted);

	SetBackground();

	if(bVassalStarted) then
		local iTotalVassals = 0;

		-- figure out how many vassals in the world
		for iTeamLoop = 0, GameDefines.MAX_CIV_TEAMS - 1 do
			local pTeam = Teams[iTeamLoop];
			if pTeam:GetMaster() ~= -1 then
				iTotalVassals = iTotalVassals + 1;
			end
		end

		Controls.NumVassals:LocalizeAndSetText( "TXT_KEY_VO_NUM_OUR_VASSALS", g_pTeam:GetNumVassals() );
		Controls.NumVassalsWorld:LocalizeAndSetText( "TXT_KEY_VO_NUM_WORLD_VASSALS", iTotalVassals );

		-- Show Master Information
		if( g_pTeam:IsVassalOfSomeone() ) then
			UpdateMasterList();
			g_SelectedMaster = nil;
		-- Show Vassal Information
		else
			UpdateVassalList();
			g_SelectedVassal = nil;
		end
	end
end

function SetBackground()
	-- background based on ideology
	local iIdeology = g_pPlayer:GetLateGamePolicyTree();
	local strBackgroundImage;
	if(iIdeology ~= -1) then
		local ideology = GameInfo.PolicyBranchTypes[iIdeology];
		strBackgroundImage = g_IdeologyBackgrounds[ideology.Type];
	else
		strBackgroundImage = g_IdeologyBackgrounds[0];
	end
	Controls.VassalBackgroundImage:SetTextureAndResize(strBackgroundImage);
	-- Can't find a method to fit texture to size, so this a hack way of doing it
	Controls.VassalBackgroundImage:SetSizeX(926);
	Controls.VassalBackgroundImage:SetSizeY(500);
end

-- Update list on the side if we have masters
function UpdateMasterList()
	if (ContextPtr:IsHidden()) then
		return;
	end
	
	if (IsGameCoreBusy()) then
		return;
	end

	-- Update label
	Controls.LabelVassalList:SetText( Locale.ConvertTextKey( "TXT_KEY_VO_YOUR_MASTERS" ) );
	Controls.LabelVassalDetails:SetText( Locale.ConvertTextKey( "TXT_KEY_VO_MASTER_DETAILS" ) );

	-- Clear buttons
	g_VassalButtonIM:ResetInstances();

	-- Add masters to stack
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
	
		local pOtherPlayer = Players[iPlayerLoop];
		if pOtherPlayer:IsEverAlive() then
			local iOtherTeam = pOtherPlayer:GetTeam();
			local pOtherTeam = Teams[ iOtherTeam ];

			if (iPlayerLoop ~= g_iPlayer and pOtherPlayer:IsAlive()) then
				-- Are we their vassal?
				if (g_pTeam:IsVassal(iOtherTeam)) then
					local controlTable = g_VassalButtonIM:GetInstance();

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

					controlTable.CivName:SetText(strCiv);
					IconHookup( otherLeaderInfo.PortraitIndex, 64, otherLeaderInfo.IconAtlas, controlTable.LeaderPortrait );

					-- team indicator
            		if( pOtherTeam:GetNumMembers() > 1 ) then
            			controlTable.TeamID:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_DEFAULT_TEAM_NAME", pOtherTeam:GetID() + 1 );
            			controlTable.TeamID:SetHide( false );
					else
            			controlTable.TeamID:SetHide( true );
            		end

					controlTable.VassalButton:SetVoid1( iPlayerLoop ); -- indicates type
					controlTable.VassalButton:RegisterCallback( Mouse.eLClick, MasterSelected );
				end
			end
		end
	end
	Controls.VassalStack:CalculateSize();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end

-- Update list on the side if we have vassals
function UpdateVassalList()
	if (ContextPtr:IsHidden()) then
		return;
	end
	
	if (IsGameCoreBusy()) then
		return;
	end

	-- Update label
	Controls.LabelVassalList:SetText( Locale.ConvertTextKey( "TXT_KEY_VO_YOUR_VASSALS" ) );
	Controls.LabelVassalDetails:SetText( Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_DETAILS" ) );

	-- Clear buttons
	g_VassalButtonIM:ResetInstances();

	-- Add vassals to stack
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
	
		local pOtherPlayer = Players[iPlayerLoop];
		if pOtherPlayer:IsEverAlive() then
			local iOtherTeam = pOtherPlayer:GetTeam();
			local pOtherTeam = Teams[ iOtherTeam ];

			if (iPlayerLoop ~= g_iPlayer and pOtherPlayer:IsAlive()) then
				-- Our vassal?
				if (pOtherTeam:IsVassal(g_iTeam)) then
					local controlTable = g_VassalButtonIM:GetInstance();

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

					controlTable.CivName:SetText(strCiv);
					IconHookup( otherLeaderInfo.PortraitIndex, 64, otherLeaderInfo.IconAtlas, controlTable.LeaderPortrait );

					-- team indicator
            		if( pOtherTeam:GetNumMembers() > 1 ) then
            			controlTable.TeamID:LocalizeAndSetText( "TXT_KEY_MULTIPLAYER_DEFAULT_TEAM_NAME", pOtherTeam:GetID() + 1 );
            			controlTable.TeamID:SetHide( false );
					else
            			controlTable.TeamID:SetHide( true );
            		end

					controlTable.VassalButton:SetVoid1( iPlayerLoop ); -- indicates type
					controlTable.VassalButton:RegisterCallback( Mouse.eLClick, VassalSelected );
				end
			end
		end
	end
	Controls.VassalStack:CalculateSize();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end

-------------------------------------------------
-- On Leader Selected
-------------------------------------------------
function MasterSelected( ePlayer )
	if not Players[Game.GetActivePlayer()]:IsTurnActive() or Game.IsProcessingMessages() then
		return;
	end

	if( Controls.MasterDescBox:IsHidden() ) then
		Controls.MasterDescBox:SetHide(false);
	end

	local pMasterPlayer = Players[ePlayer];
	local iMasterTeam = pMasterPlayer:GetTeam();
	local pMasterTeam = Teams[iMasterTeam];

	g_SelectedMaster = iMasterTeam;
	
	local civType = pMasterPlayer:GetCivilizationType();
	local civInfo = GameInfo.Civilizations[civType];
	local strCiv = Locale.ConvertTextKey(civInfo.ShortDescription);
	
	if(pMasterPlayer:GetNickName() ~= "" and Game.IsGameMultiPlayer() and pMasterPlayer:IsHuman()) then
		TruncateString(Controls.MasterInfoLeaderName, textBoxSize, pMasterPlayer:GetNickName()); 
	else
		Controls.MasterInfoLeaderName:SetText( pMasterPlayer:GetName() );
	end
	
	Controls.MasterInfoCivName:SetText( strCiv );

	-- Leader Portrait
	local pLeader = GameInfo.Leaders[pMasterPlayer:GetLeaderType()];
	IconHookup( pLeader.PortraitIndex, 128, pLeader.IconAtlas, Controls.MasterIcon );
	CivIconHookup( ePlayer, 45, Controls.MasterSubIcon, Controls.MasterSubIconBG, Controls.MasterSubIconShadow, true, true, Controls.MasterSubIconHighlight );

	Controls.MasterTurnsYouAreVassal:LocalizeAndSetText( "TXT_KEY_VO_TURNS", g_pTeam:GetNumTurnsIsVassal(), Game.GetGameTurn() - g_pTeam:GetNumTurnsIsVassal() );
	
	local strIndependencePossible = "";
	if( g_pTeam:CanEndVassal(iMasterTeam) ) then
		strIndependencePossible = "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_YES") .. "[ENDCOLOR]";
	else 
		strIndependencePossible = "[COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_NO") .. "[ENDCOLOR]";
	end
	
	-- Build Independence tooltip
	Controls.MasterDeclareIndependence:LocalizeAndSetText( "TXT_KEY_VO_INDEPENDENCE_POSSIBLE_MASTER", strIndependencePossible );
	Controls.MasterDeclareIndependence:LocalizeAndSetToolTip( g_pPlayer:GetVassalIndependenceTooltipAsVassal(ePlayer) );

	-- Request Independence button
	local bCanRequestIndependence = g_pTeam:CanEndVassal( iMasterTeam );

	local strTooltip = "";
	strTooltip = strTooltip .. Locale.ConvertTextKey( "TXT_KEY_VO_REQUEST_INDEPENDENCE_TT", pMasterTeam:GetName() );
	-- Can't request? Set up tooltip
	if( not bCanRequestIndependence ) then
		local iMinimumTurns = 0;
		if ( g_pTeam:IsVoluntaryVassal(iVassalTeam) ) then
			iMinimumTurns = Game.GetMinimumVoluntaryVassalTurns();
		else 
			iMinimumTurns = Game.GetMinimumVassalTurns();
		end

		local iNumTurnsIsVassal = g_pTeam:GetNumTurnsIsVassal( iVassalTeam );

		if(iNumTurnsIsVassal < iMinimumTurns) then
			strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_REQUEST_INDEPENDENCE_TOO_SOON", iMinimumTurns, iMinimumTurns - iNumTurnsIsVassal);
		else
			strTooltip = strTooltip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_REQUEST_INDEPENDENCE_CONDITION_NOT_MET" );
		end
	end

	Controls.RequestIndependence:SetDisabled( not bCanRequestIndependence );
	Controls.RequestIndependence:SetToolTipString( strTooltip );
	Controls.RequestIndependence:SetVoid1( pMasterTeam:GetLeaderID() );
	Controls.RequestIndependence:RegisterCallback( Mouse.eLClick, OnRequestIndependenceClicked );

	UpdateMasterStatistics( ePlayer );
	UpdateMasterTaxes( ePlayer );

	local strType, strTypeTooltip;
	if( g_pTeam:IsVoluntaryVassal( iMasterTeam ) ) then
		strType = Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_TYPE_VOLUNTARY" );
		strTypeTooltip = Locale.ConvertTextKey( "TXT_KEY_VO_MASTER_TYPE_VOLUNTARY_TT" );
	else
		strType = Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_TYPE_CAPITULATED" );
		strTypeTooltip = Locale.ConvertTextKey( "TXT_KEY_VO_MASTER_TYPE_CAPITULATED_TT" );
	end

	Controls.MasterVassalType:SetText(strType);
	Controls.MasterVassalType:SetToolTipString(strTypeTooltip);
end

function VassalSelected( ePlayer )

	if not Players[Game.GetActivePlayer()]:IsTurnActive() then
		return;
	end

	if( Controls.VassalDescBox:IsHidden() ) then
		Controls.VassalDescBox:SetHide(false);
	end

	local pVassalPlayer = Players[ePlayer];
	local iVassalTeam = pVassalPlayer:GetTeam();
	local pVassalTeam = Teams[iVassalTeam];

	g_SelectedVassal = iVassalTeam;
	
	local civType = pVassalPlayer:GetCivilizationType();
	local civInfo = GameInfo.Civilizations[civType];
	local strCiv = Locale.ConvertTextKey(civInfo.ShortDescription);
	
	if(pVassalPlayer:GetNickName() ~= "" and Game.IsGameMultiPlayer() and pVassalPlayer:IsHuman()) then
		TruncateString(Controls.VassalInfoLeaderName, textBoxSize, pVassalPlayer:GetNickName()); 
	else
		Controls.VassalInfoLeaderName:SetText( pVassalPlayer:GetName() );
	end

	Controls.VassalInfoCivName:SetText( strCiv );
	Controls.VassalInfoStarted:LocalizeAndSetText("TXT_KEY_VO_TURNS", pVassalTeam:GetNumTurnsIsVassal(), Game.GetGameTurn() - pVassalTeam:GetNumTurnsIsVassal() );

	-- Leader Portrait
	local pLeader = GameInfo.Leaders[pVassalPlayer:GetLeaderType()];
	IconHookup( pLeader.PortraitIndex, 128, pLeader.IconAtlas, Controls.VassalIcon );
	CivIconHookup( ePlayer, 45, Controls.VassalSubIcon, Controls.VassalSubIconBG, Controls.VassalSubIconShadow, true, true, Controls.VassalSubIconHighlight );

	-- Calculate percentage of current cities vs. what we had when vassalage started

	local iCityPercent = 0;
	local iPopPercent = 0;
	
	if( pVassalTeam:GetNumCitiesWhenVassalMade() ~= 0 ) then
		iCityPercent = math.floor(pVassalTeam:GetNumCities() * 100 / pVassalTeam:GetNumCitiesWhenVassalMade());
	else
		if( pVassalTeam:GetNumCities() == 0) then
			iCityPercent = 0;
		else
			iCityPercent = 100;
		end
	end
	if( pVassalTeam:GetTotalPopulationWhenVassalMade() ~= 0 ) then
		iPopPercent = math.floor(pVassalTeam:GetTotalPopulation() * 100 / pVassalTeam:GetTotalPopulationWhenVassalMade());
	else
		if( pVassalTeam:GetTotalPopulation() == 0) then
			iPopPercent = 0;
		else
			iPopPercent = 100;
		end
	end

	local iMasterCityPercent = 0;
	local iMasterPopPercent = 0;

	if( g_pTeam:GetNumCities() ~= 0) then
		iMasterCityPercent = math.floor(pVassalTeam:GetNumCities() * 100 / g_pTeam:GetNumCities());
	end
	if( g_pTeam:GetTotalPopulation() ~= 0) then
		iMasterPopPercent = math.floor(pVassalTeam:GetTotalPopulation() * 100 / g_pTeam:GetTotalPopulation());
	end

	local iMinimumTurns = 0;
	if( pVassalTeam:IsVoluntaryVassal(g_iTeam) ) then
		iMinimumTurns = Game.GetMinimumVoluntaryVassalTurns();
	else
		iMinimumTurns = Game.GetMinimumVassalTurns();
	end

	-- Build Independence tooltip
	local strIndependenceTooltip = g_pPlayer:GetVassalIndependenceTooltipAsMaster(ePlayer);

	local strIndependencePossible = "";
	if( pVassalTeam:CanEndVassal(g_iPlayer) ) then
		strIndependencePossible = "[COLOR_NEGATIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_YES") .. "[ENDCOLOR]";
	else 
		strIndependencePossible = "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_NO") .. "[ENDCOLOR]";
	end


	Controls.VassalInfoIndependence:LocalizeAndSetText( strIndependencePossible );
	Controls.VassalInfoIndependence:SetToolTipString(strIndependenceTooltip);

	Controls.VassalPercentMasterCities:LocalizeAndSetText( iMasterCityPercent .. "%" );
	Controls.VassalPercentMasterCities:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_CITY_PERCENT_TT", pVassalTeam:GetNumCities(), g_pTeam:GetNumCities(), pVassalPlayer:GetNumCities() );
	Controls.VassalPercentMasterPop:LocalizeAndSetText( iMasterPopPercent .. "%" );
	Controls.VassalPercentMasterPop:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_POP_PERCENT_TT", pVassalTeam:GetTotalPopulation(), g_pTeam:GetTotalPopulation(), pVassalPlayer:GetTotalPopulation() );

	Controls.VassalInfoCityPercent:LocalizeAndSetText( iCityPercent  .. "%");
	Controls.VassalInfoCityPercent:LocalizeAndSetToolTip("TXT_KEY_VO_CITY_PERCENT_TT", pVassalTeam:GetNumCities(), pVassalTeam:GetNumCitiesWhenVassalMade(), pVassalPlayer:GetNumCities());
	Controls.VassalInfoPopPercent:LocalizeAndSetText( iPopPercent  .. "%");
	Controls.VassalInfoPopPercent:LocalizeAndSetToolTip("TXT_KEY_VO_POP_PERCENT_TT", pVassalTeam:GetTotalPopulation(), pVassalTeam:GetTotalPopulationWhenVassalMade(), pVassalPlayer:GetTotalPopulation());

	local strType, strTypeTooltip;
	if( pVassalTeam:IsVoluntaryVassal( g_iTeam ) ) then
		strType = Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_TYPE_VOLUNTARY" );
		strTypeTooltip = Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_TYPE_VOLUNTARY_TT" );
	else
		strType = Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_TYPE_CAPITULATED" );
		strTypeTooltip = Locale.ConvertTextKey( "TXT_KEY_VO_VASSAL_TYPE_CAPITULATED_TT" );
	end

	Controls.VassalInfoType:LocalizeAndSetText(strType);
	Controls.VassalInfoType:LocalizeAndSetToolTip(strTypeTooltip);

	Controls.TaxSlider:SetValue( TaxValueToPercent( g_pTeam:GetVassalTax( ePlayer ) ) );
	
	local iNumTurnsForTax = Game.GetMinimumVassalTaxTurns();
	local iNumTurnsSinceSet = g_pTeam:GetNumTurnsSinceVassalTaxSet( ePlayer );
	local iNumTurnsLeft = iNumTurnsForTax - iNumTurnsSinceSet;

	if( not g_pTeam:CanSetVassalTax( ePlayer ) ) then
		Controls.TaxSlider:SetDisabled( true );
		Controls.TaxSliderValueToolTip:LocalizeAndSetToolTip( "TXT_KEY_VO_TAX_TOO_SOON", iNumTurnsForTax, iNumTurnsLeft );
	else
		Controls.TaxSlider:SetDisabled( false );
		Controls.TaxSliderValueToolTip:SetToolTipString( "" );
	end
	
	local iTurnTaxesSet = Game.GetGameTurn() - iNumTurnsSinceSet;
	local iTurnTaxesAvailable = Game.GetGameTurn() + iNumTurnsLeft;
	local availableStr = "";
	local taxesSetTurnStr = "";
	if ( iNumTurnsLeft <= 0 or iNumTurnsSinceSet == -1 ) then
		availableStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_CHANGE_READY");
	else
		availableStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_TURN_AVAILABLE", iTurnTaxesAvailable);
	end

	if ( g_pTeam:GetNumTurnsSinceVassalTaxSet( ePlayer ) == -1 ) then	
		taxesSetTurnStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_TURN_NOT_SET_EVER");
	else
		taxesSetTurnStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_TURN_SET", iTurnTaxesSet);
	end

	Controls.TaxesCurrentGPT:LocalizeAndSetText("TXT_KEY_VO_TAX_CONTRIBUTION", g_pPlayer:GetVassalTaxContribution(ePlayer));
	Controls.TaxesCurrentGPT:LocalizeAndSetToolTip("TXT_KEY_VO_TAX_GPT_RECEIVED_TT", g_pPlayer:GetVassalTaxContribution(ePlayer), g_pTeam:GetVassalTax( ePlayer), pVassalPlayer:CalculateGrossGold());

	Controls.TaxesTurnSet:SetText(taxesSetTurnStr);
	Controls.TaxesAvailableTurn:SetText(availableStr);
	Controls.TaxSliderValue:LocalizeAndSetText("TXT_KEY_VO_TAX_RATE", g_pTeam:GetVassalTax( ePlayer ) );

	DoVassalStatistics( ePlayer );
	UpdateVassalTreatment( ePlayer );

	Controls.ApplyChanges:SetDisabled( true );
	Controls.ApplyChanges:SetVoid1( ePlayer );
	Controls.ApplyChanges:RegisterCallback( Mouse.eLClick, OnApplyChangesClicked );
	Controls.ApplyChanges:LocalizeAndSetToolTip( "TXT_KEY_APPLY_CHANGES_TT" );

	-- Liberate Vassal button
	local tooltipStr = Locale.ConvertTextKey("TXT_KEY_VO_LIBERATE_VASSAL_TT", pVassalTeam:GetName());
	if( not g_pTeam:CanLiberateVassal( iVassalTeam ) ) then
		Controls.LiberateCiv:SetDisabled( true );
		tooltipStr = tooltipStr .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_VO_LIBERATE_VASSAL_TOO_SOON", Game.GetMinimumVassalLiberateTurns(), Game.GetMinimumVassalLiberateTurns() - pVassalTeam:GetNumTurnsIsVassal( g_iTeam ));
	else
		Controls.LiberateCiv:SetDisabled( false );
	end

	Controls.LiberateCiv:SetToolTipString( tooltipStr );
	Controls.LiberateCiv:SetVoid1( iVassalTeam );
	Controls.LiberateCiv:RegisterCallback( Mouse.eLClick, OnLiberateCivClicked );

	Controls.StatsScrollPanel:CalculateInternalSize();
	Controls.StatsScrollPanel:ReprocessAnchoring();

	Controls.ManagementScrollPanel:CalculateInternalSize();
	Controls.ManagementScrollPanel:ReprocessAnchoring();
end

-------------------------------------------------
-- Liberate Civ button clicked
-------------------------------------------------
function OnLiberateCivClicked( iVassalTeam )
	print( "Clicked on Liberate Civ: " .. iVassalTeam );

	Controls.ConfirmLabel:LocalizeAndSetText( "TXT_KEY_VO_CONFIRM_LIBERATE", Teams[ iVassalTeam ]:GetName() );
	Controls.Confirm:SetHide( false );
	Controls.BGBlock:SetHide( true );
	
	Controls.Yes:SetVoid1( 0 );
	Controls.Yes:SetVoid2( iVassalTeam );
	Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );
end

function OnApplyChangesClicked( iVassalPlayer )
	local iTaxValue = PercentToTaxValue( Controls.TaxSlider:GetValue() );

	Controls.ConfirmLabel:LocalizeAndSetText( "TXT_KEY_VO_CONFIRM_TAX_CHANGE", iTaxValue, Teams[ Players[iVassalPlayer]:GetTeam() ]:GetName(), Game.GetMinimumVassalTaxTurns() );
	Controls.Confirm:SetHide( false );
	Controls.BGBlock:SetHide( true );
	
	Controls.Yes:SetVoid1( 1 );
	Controls.Yes:SetVoid2( iVassalPlayer );
	Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );
end

function OnRequestIndependenceClicked( iVassalPlayer )
	Controls.ConfirmLabel:LocalizeAndSetText( "TXT_KEY_VO_CONFIRM_REQUEST_INDEPENDENCE", Teams[ Players[iVassalPlayer]:GetTeam() ]:GetName() );
	Controls.Confirm:SetHide( false );
	Controls.BGBlock:SetHide( true );
	
	Controls.Yes:SetVoid1( 2 );
	Controls.Yes:SetVoid2( iVassalPlayer );
	Controls.Yes:RegisterCallback( Mouse.eLClick, OnYes );
end

function UpdateVassalTreatment( ePlayer )
	local pPlayer = Players[ ePlayer ];

	local vassalTreatmentLookup = {
		[0] = "TXT_KEY_VO_TREATMENT_CONTENT",
		[1] = "TXT_KEY_VO_TREATMENT_DISAGREE",
		[2] = "TXT_KEY_VO_TREATMENT_MISTREATED",
		[3] = "TXT_KEY_VO_TREATMENT_UNHAPPY",
		[4] = "TXT_KEY_VO_TREATMENT_ENSLAVED",
	};

	-- Vassal Treatment
	-- probably better to use an array here, but let's be safe :)
    		
	Controls.VassalTreatment:LocalizeAndSetText( "TXT_KEY_VO_TREATMENT", vassalTreatmentLookup[pPlayer:GetVassalTreatmentLevel( g_iPlayer )] );
	Controls.VassalTreatment:SetToolTipString( pPlayer:GetVassalTreatmentToolTip( g_iPlayer ) );
end

function DoVassalStatistics( ePlayer )
	local pPlayer = Players[ ePlayer ];
	
	-- Population
	Controls.VassalStatsPopulation:SetText( pPlayer:GetTotalPopulation() );
	local iHappiness = pPlayer:GetExcessHappiness();
	local szHappinessLabel = "";
	if(iHappiness < 0) then
		szHappinessLabel = "[ICON_HAPPINESS_3] " .. Locale.ConvertTextKey("TXT_KEY_VO_HAPPINESS_STAT");
	else
		szHappinessLabel = "[ICON_HAPPINESS_1] " .. Locale.ConvertTextKey("TXT_KEY_VO_HAPPINESS_STAT");
	end
	Controls.VassalStatsHappyLabel:SetText( szHappinessLabel );
	Controls.VassalStatsHappy:SetText( iHappiness );
	
	-- Economic
	Controls.VassalStatsGross:SetText( pPlayer:CalculateGrossGold() );
	Controls.VassalStatsGold:SetText( pPlayer:CalculateGoldRate() );
	Controls.VassalStatsTradeRoutes:LocalizeAndSetText( "TXT_KEY_VO_TRADE_ROUTES_LABEL", pPlayer:GetNumInternationalTradeRoutesUsed(), pPlayer:GetNumInternationalTradeRoutesAvailable() );

	-- Culture
	local ideologyStr = Locale.ConvertTextKey("TXT_KEY_VO_NO_IDEOLOGY");
	local eIdeology = g_pPlayer:GetLateGamePolicyTree();
	if( eIdeology ~= -1 ) then
		local pIdeology = GameInfo.PolicyBranchTypes[ eIdeology ];
		if( pIdeology ) then
			if( eIdeology == POLICY_BRANCH_ORDER ) then
				ideologyStr = "[ICON_IDEOLOGY_ORDER] ";
			elseif ( eIdeology == POLICY_BRANCH_FREEDOM ) then
				ideologyStr = "[ICON_IDEOLOGY_FREEDOM] ";
			else
				ideologyStr = "[ICON_IDEOLOGY_AUTOCRACY] ";
			end
			ideologyStr = Locale.ConvertTextKey( "TXT_KEY_VO_IDEOLOGY", pIdeology.Description );
		end
	end
	Controls.VassalStatsIdeology:LocalizeAndSetText( "TXT_KEY_VO_IDEOLOGY_STAT", ideologyStr );
	Controls.VassalStatsCulture:SetText( pPlayer:GetTotalJONSCulturePerTurn() );
	Controls.VassalStatsTourism:SetText( pPlayer:GetTourism());
	
	-- Religion
	local eMajorityReligion = pPlayer:GetMajorityReligion();
	local szMajorityReligion = Locale.ConvertTextKey( "TXT_KEY_VO_NO_RELIGION" );
	if(eMajorityReligion ~= -1) then
		local pReligionInfo = GameInfo.Religions[eMajorityReligion];
		szMajorityReligion = Locale.ConvertTextKey( "TXT_KEY_VO_MAJORITY_RELIGION", pReligionInfo.Description );
	end
	Controls.VassalStatsReligion:SetText( szMajorityReligion );
	Controls.VassalStatsFaith:LocalizeAndSetText( pPlayer:GetTotalFaithPerTurn() );

	-- Science
	Controls.VassalStatsScience:SetText( pPlayer:GetScience() );
	DoVassalTechUpdate( ePlayer );
end

-------------------------------------------------
-- Vassal Tech Button
-------------------------------------------------
function DoVassalTechUpdate( iVassalPlayer )
	local pPlayer = Players[ iVassalPlayer ];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pTeamTechs = pTeam:GetTeamTechs();
	
	eCurrentTech = pPlayer:GetCurrentResearch();
	eRecentTech = pTeamTechs:GetLastTechAcquired();
	
	local fResearchProgressPercent = 0;
	local fResearchProgressPlusThisTurnPercent = 0;
	
	local researchStatus = "";
	local szText = "";
	
	-- Are we researching something right now?
	if (eCurrentTech ~= -1) then
		
		local iResearchTurnsLeft = pPlayer:GetResearchTurnsLeft(eCurrentTech, true);
		local iCurrentResearchProgress = pPlayer:GetResearchProgress(eCurrentTech);
		local iResearchNeeded = pPlayer:GetResearchCost(eCurrentTech);
		local iResearchPerTurn = pPlayer:GetScience();
		local iCurrentResearchPlusThisTurn = iCurrentResearchProgress + iResearchPerTurn;
		
		fResearchProgressPercent = iCurrentResearchProgress / iResearchNeeded;
		fResearchProgressPlusThisTurnPercent = iCurrentResearchPlusThisTurn / iResearchNeeded;
		
		if (fResearchProgressPlusThisTurnPercent > 1) then
			fResearchProgressPlusThisTurnPercent = 1
		end
				
		local pTechInfo = GameInfo.Technologies[eCurrentTech];
		if pTechInfo then
			szText = Locale.ConvertTextKey( pTechInfo.Description );
			if iResearchPerTurn > 0 then
				szText = szText .. " (" .. tostring(iResearchTurnsLeft) .. ")";
			end		
			local customHelpString = GetHelpTextForTech(eCurrentTech);
			Controls.TechBox:SetToolTipString( customHelpString );
			Controls.TechLabel:SetText( szText );

			-- if we have one, update the tech picture
			if IconHookup( pTechInfo.PortraitIndex, 64, pTechInfo.IconAtlas, Controls.TechIcon ) then
				Controls.TechIcon:SetHide( false );
			else
				Controls.TechIcon:SetHide( true );
			end			
		end
		--Controls.ResearchLabel:SetText( "" );-- Locale.ConvertTextKey( "TXT_KEY_RESEARCH_ONGOING" ));
		--Controls.TechText:SetHide( false );
		--Controls.FinishedTechText:SetHide( true );
	elseif (eRecentTech ~= -1) then -- maybe we just finished something			
		local pTechInfo = GameInfo.Technologies[eRecentTech];
		if pTechInfo then
			szText = Locale.ConvertTextKey( pTechInfo.Description );
			Controls.TechLabel:SetText( szText );

			-- if we have one, update the tech picture
			if IconHookup( pTechInfo.PortraitIndex, 64, pTechInfo.IconAtlas, Controls.TechIcon ) then
				Controls.TechIcon:SetHide( false );
			else
				Controls.TechIcon:SetHide( true );
			end			
		end
		researchStatus = Locale.ConvertTextKey("TXT_KEY_RESEARCH_FINISHED");
		Controls.TechBox:SetToolTipString( helpTT );
	else
		researchStatus = Locale.ConvertTextKey("TXT_KEY_NOTIFICATION_SUMMARY_NEW_RESEARCH");
		Controls.TechBox:SetToolTipString( helpTT );
	end
	
	-- Research Meter
  	local research = pPlayer:GetCurrentResearch();
  	if( research ~= -1 ) then
    	Controls.ResearchMeter:SetPercents( fResearchProgressPercent, fResearchProgressPlusThisTurnPercent );
	else
    	Controls.ResearchMeter:SetPercents( 1, 0 );
	end
end

-------------------------------------------------
-- Tax Slider
-------------------------------------------------
function TaxSliderUpdate( fValue )

	local iTaxValue = PercentToTaxValue( fValue );
	Controls.TaxSliderValue:SetText( Locale.ConvertTextKey("TXT_KEY_VO_TAX_RATE", iTaxValue) );

	UpdateApplyChanges();
end
Controls.TaxSlider:RegisterSliderCallback( TaxSliderUpdate );

function UpdateApplyChanges()
	Controls.ApplyChanges:SetDisabled( PercentToTaxValue( Controls.TaxSlider:GetValue() ) == g_pTeam:GetVassalTax(g_SelectedVassal) );
end

-- convert percentage to tax tvalue
function PercentToTaxValue( fPercent )
	local iMin = Game.GetMinimumVassalTax();
	local iMax = Game.GetMaximumVassalTax();

	-- desired number of steps we want the meter to grow
	local iStepIncrease = 5;
	local iStepsNeeded = (iMax / iStepIncrease);
	
	fPercent = math.floor( fPercent * iStepsNeeded ) * (1 / iStepsNeeded);
	return fPercent * (iMax - iMin);
end

-- convert Tax Value to percentage from 0.0 to 1.0
function TaxValueToPercent( iTaxValue )
	local iMin = Game.GetMinimumVassalTax();
	local iMax = Game.GetMaximumVassalTax();
	return math.min(math.max((iTaxValue) / (iMax - iMin), 0), 1);
end

-- Statistics for human vassal
function UpdateMasterStatistics( ePlayer )
--	-- Us icon hookup
--	CivIconHookup(Game.GetActivePlayer(), 32, Controls.UsCivIcon, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true, Controls.UsCivIconHighlight);
--	CivIconHookup(Game.GetActivePlayer(), 32, Controls.TeamUsCivIcon, Controls.TeamUsCivIconBG, Controls.TeamUsCivIconShadow, false, true, Controls.TeamUsCivIconHighlight);
--
--	-- Master icon hookup
--	CivIconHookup(ePlayer, 32, Controls.MasterCivIcon, Controls.MasterCivIconBG, Controls.MasterCivIconShadow, false, true, Controls.MasterCivIconHighlight);
--	CivIconHookup(ePlayer, 32, Controls.TeamMasterCivIcon, Controls.TeamMasterCivIconBG, Controls.TeamMasterCivIconShadow, false, true, Controls.TeamMasterCivIconHighlight);

	local pMasterPlayer = Players[ePlayer];
	local iMasterTeam = pMasterPlayer:GetTeam();
	local pMasterTeam = Teams[iMasterTeam];

	-- Population
	local iYourPopulation = g_pPlayer:GetTotalPopulation();
	local iYourPopulationTeam = g_pTeam:GetTotalPopulation();
	local iTheirPopulation = pMasterPlayer:GetTotalPopulation();
	local iTheirPopulationTeam = pMasterTeam:GetTotalPopulation();
	
	Controls.MasterStatsYourPopulation:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY", iYourPopulation, iYourPopulationTeam );
	Controls.MasterStatsYourPopulation:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_YOUR_POP_TT" );
	Controls.MasterStatsTheirPopulation:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY", iTheirPopulation, iTheirPopulationTeam );
	Controls.MasterStatsTheirPopulation:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_THEIR_POP_TT" );
	
	local iPopPercent = 0;
	if( iTheirPopulation ~= 0 ) then
		iPopPercent = math.floor(iYourPopulation * 100 / iTheirPopulation);
	end

	local iPopTeamPercent = 0;
	if( iTheirPopulationTeam ~= 0 ) then
		iPopTeamPercent = math.floor(iYourPopulationTeam * 100 / iTheirPopulationTeam);
	end
	Controls.MasterStatsPopulationPercent:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY_PERCENT_PLAYER", iPopPercent, iPopTeamPercent );
	Controls.MasterStatsPopulationPercent:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_VIEW_POP_PERCENT_TT", pMasterPlayer:GetName() );

	local iYourPopWhenVassalStarted = g_pTeam:GetTotalPopulationWhenVassalMade();
	local iPopFromStartPercent = 0;
	if( iYourPopWhenVassalStarted ~= 0 ) then
		iPopFromStartPercent = math.floor( iYourPopulationTeam * 100 / iYourPopWhenVassalStarted );
	end
	Controls.MasterStatsPopStartedPercent:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY_PERCENT", iPopFromStartPercent);
	Controls.MasterStatsPopStartedPercent:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_POP_STARTED_PERCENT_TT", pMasterTeam:GetName() );

	-- Cities
	local iYourCities = g_pPlayer:GetNumCities();
	local iYourCitiesTeam = g_pTeam:GetNumCities();
	local iTheirCities = pMasterPlayer:GetNumCities();
	local iTheirCitiesTeam = pMasterTeam:GetNumCities();
	
	Controls.MasterStatsYourCities:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY", iYourCities, iYourCitiesTeam  );
	Controls.MasterStatsYourCities:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_YOUR_CITIES_TT"  );
	Controls.MasterStatsTheirCities:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY", iTheirCities, iTheirCitiesTeam );
	Controls.MasterStatsTheirCities:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_THEIR_CITIES_TT" );
	
	local iCityPercent = 0;
	if( iTheirCitiesTeam ~= 0 ) then
		iCityPercent = math.floor(iYourCitiesTeam * 100 / iTheirCitiesTeam);
	end

	local iCityTeamPercent = 0;
	if( iTheirCitiesTeam ~= 0 ) then
		iCityTeamPercent = math.floor(iYourCitiesTeam * 100 / iTheirCitiesTeam);
	end
	Controls.MasterStatsCitiesPercent:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY_PERCENT_PLAYER", iCityPercent, iCityTeamPercent );
	Controls.MasterStatsCitiesPercent:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_CITIES_PERCENT_TT", pMasterPlayer:GetName() );
	
	local iYourCitiesWhenVassalStarted = g_pTeam:GetNumCitiesWhenVassalMade();
	local iCityFromStartPercent = 0;
	if( iYourCitiesWhenVassalStarted ~= 0 ) then
		iCityFromStartPercent = math.floor( iYourCitiesTeam * 100 / iYourCitiesWhenVassalStarted );
	end
	Controls.MasterStatsCitiesStartedPercent:LocalizeAndSetText( "TXT_KEY_VO_MASTER_VALUE_DISPLAY_PERCENT",  iCityFromStartPercent);
	Controls.MasterStatsCitiesStartedPercent:LocalizeAndSetToolTip( "TXT_KEY_VO_MASTER_CITIES_STARTED_PERCENT_TT", pMasterTeam:GetName());

end

-- Update our master's taxes on us
function UpdateMasterTaxes( ePlayer )
	local pMasterPlayer = Players[ePlayer];
	local iMasterTeam = pMasterPlayer:GetTeam();
	local pMasterTeam = Teams[iMasterTeam];

	local iNumTurnsForTax = Game.GetMinimumVassalTaxTurns();
	local iNumTurnsSinceSet = pMasterTeam:GetNumTurnsSinceVassalTaxSet( g_iPlayer );
	local iNumTurnsLeft = iNumTurnsForTax - iNumTurnsSinceSet;
	
	local iTurnTaxesSet = Game.GetGameTurn() - iNumTurnsSinceSet;
	local iTurnTaxesAvailable = Game.GetGameTurn() + iNumTurnsLeft;
	local availableStr = "";
	local taxesSetTurnStr = "";
	if ( iNumTurnsLeft <= 0 or iNumTurnsSinceSet == -1 ) then
		availableStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_CHANGE_READY_MASTER");
	else
		availableStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_TURN_AVAILABLE", iTurnTaxesAvailable);
	end
	
	if ( iNumTurnsSinceSet == -1 ) then	
		taxesSetTurnStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_TURN_NOT_SET_EVER_MASTER");
	else
		taxesSetTurnStr = Locale.ConvertTextKey("TXT_KEY_VO_TAX_TURN_SET", iTurnTaxesSet);
	end

	local iTaxRate = pMasterTeam:GetVassalTax( g_iPlayer );
	
	Controls.LabelMasterTaxRate:LocalizeAndSetText("TXT_KEY_VO_TAX_RATE", iTaxRate );
	Controls.LabelMasterTaxRate:LocalizeAndSetToolTip("TXT_KEY_VO_TAX_RATE_MASTER_TT", iTaxRate, pMasterTeam:GetName() );
	
	Controls.MasterTaxSlider:SetDisabled(false);
	Controls.MasterTaxSlider:SetValue( TaxValueToPercent( pMasterTeam:GetVassalTax( g_iPlayer ) ) );
	Controls.MasterTaxSlider:SetDisabled(true);
	
	Controls.MasterTaxesTurnSet:LocalizeAndSetText( taxesSetTurnStr );
	Controls.MasterAvailableTurn:LocalizeAndSetText( availableStr );
	Controls.MasterTaxesCurrentGPT:LocalizeAndSetText( "TXT_KEY_VO_TAX_CONTRIBUTION_MASTER", g_pPlayer:GetExpensePerTurnFromVassalTaxes());
	Controls.MasterTaxesCurrentGPT:LocalizeAndSetToolTip( "TXT_KEY_VO_TAX_GPT_TAKEN_TT", g_pPlayer:GetExpensePerTurnFromVassalTaxes(), iTaxRate, g_pPlayer:CalculateGrossGold() );
end

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )
	if( popupInfo.Type == ButtonPopupTypes.BUTTONPOPUP_MODDER_11 ) then
    	m_PopupInfo = popupInfo;
    	
		if(not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_VASSALAGE)) then

			if( m_PopupInfo.Data1 == 1 ) then
        		if( ContextPtr:IsHidden() == false ) then
        			OnClose();
				else
            		UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
        		end
    		else
        		UIManager:QueuePopup( ContextPtr, PopupPriority.LoadGameScreen );
    		end

		end
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose);

function OnYes( type, iValue )
	Controls.Confirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
	
	-- Liberation
	if( type == 0 ) then
		g_pTeam:DoLiberateVassal( iValue );
		OnClose();
	-- Apply Tax Change
	elseif ( type == 1 ) then
		local iTaxRate = PercentToTaxValue( Controls.TaxSlider:GetValue() );
		g_pTeam:DoApplyVassalTax( iValue, iTaxRate );
		VassalSelected( iValue );
		UpdateApplyChanges();
	-- Request Independence
	elseif ( type == 2 ) then
		Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_ENDS_VASSALAGE, iValue, 2, 0 );
		OnClose();
	end
end

function OnNo( )
	Controls.Confirm:SetHide(true);
	Controls.BGBlock:SetHide(false);
end
Controls.No:RegisterCallback( Mouse.eLClick, OnNo );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			if(Controls.Confirm:IsHidden()) then
				OnClose();
			else
				Controls.Confirm:SetHide(true);
				Controls.BGBlock:SetHide(false);
			end
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	-- Set player icon at top of screen
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.Icon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
   
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
			Update();
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_MODDER_11, 0);
			ResetVassalDescription();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-- Build vassal benefit text
local strBenefits = "";
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_OPEN_BORDERS" );
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_YIELD_PERCENTAGE" );
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_RELIGION_MOD" );
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_TOURISM_MOD" );
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_DIPLOMAT" );
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_IDEOLOGY" );
strBenefits = strBenefits .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_VO_BENEFITS_LEVY" );
Controls.VassalBenefits:SetText(strBenefits);

-- Just in case :)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();

ContextPtr:SetHide(true);
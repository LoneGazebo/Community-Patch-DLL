-------------------------------------------------
-- Who's Winning Popup
-------------------------------------------------

include( "InstanceManager" );
include( "IconSupport" );

local g_PlayerListInstanceManager = InstanceManager:new( "PlayerEntryInstance", "PlayerEntryBox", Controls.PlayerListStack );
local m_PopupInfo = nil;

local g_iListMode = -1;
local g_iLastListMode = -1;

local g_ListMode_Food = 0;
local g_ListMode_Production = 1;
local g_ListMode_Gold = 2;
local g_ListMode_Science = 3;
local g_ListMode_Culture = 4;
local g_ListMode_Happiness = 5;
local g_ListMode_Wonders = 6;
local g_ListMode_Power = 7;
local g_ListMode_Cultural_Influence = 8;
local g_ListMode_Tourist_Cities = 9;

local g_tListModeText = {};
g_tListModeText[g_ListMode_Food] = "TXT_KEY_PROGRESS_SCREEN_FOOD";
g_tListModeText[g_ListMode_Production] = "TXT_KEY_PROGRESS_SCREEN_PRODUCTION";
g_tListModeText[g_ListMode_Gold] = "TXT_KEY_PROGRESS_SCREEN_GOLD";
g_tListModeText[g_ListMode_Science] = "TXT_KEY_PROGRESS_SCREEN_SCIENCE";
g_tListModeText[g_ListMode_Culture] = "TXT_KEY_PROGRESS_SCREEN_CULTURE";
g_tListModeText[g_ListMode_Happiness] = "TXT_KEY_PROGRESS_SCREEN_HAPPINESS";
g_tListModeText[g_ListMode_Wonders] = "TXT_KEY_PROGRESS_SCREEN_WONDERS";
g_tListModeText[g_ListMode_Power] = "TXT_KEY_PROGRESS_SCREEN_POWER";
g_tListModeText[g_ListMode_Cultural_Influence] = "TXT_KEY_PROGRESS_SCREEN_CULTURAL_INFLUENCE";
g_tListModeText[g_ListMode_Tourist_Cities] = "TXT_KEY_PROGRESS_SCREEN_CITY_TOURISM";

local g_tListOfDudes = {
									"TXT_KEY_DUDE_1",
									"TXT_KEY_DUDE_2",
									"TXT_KEY_DUDE_3",
									"TXT_KEY_DUDE_4",
									"TXT_KEY_DUDE_5",
									"TXT_KEY_DUDE_6",
									"TXT_KEY_DUDE_7",
									"TXT_KEY_DUDE_8",
									"TXT_KEY_DUDE_9",
									"TXT_KEY_DUDE_10",
									"TXT_KEY_DUDE_11",
									"TXT_KEY_DUDE_12",
									"TXT_KEY_DUDE_13",
									"TXT_KEY_DUDE_14",
									"TXT_KEY_DUDE_15",
									"TXT_KEY_DUDE_16",
									"TXT_KEY_DUDE_17",
									"TXT_KEY_DUDE_18",
									"TXT_KEY_DUDE_19",
									"TXT_KEY_DUDE_20",
									"TXT_KEY_DUDE_21",
									"TXT_KEY_DUDE_22",
									"TXT_KEY_DUDE_23",
									"TXT_KEY_DUDE_24",
									"TXT_KEY_DUDE_25",
								};


-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_WHOS_WINNING ) then
		return;
	end
	
	m_PopupInfo = popupInfo;
	g_PlayerListInstanceManager:ResetInstances();

    local iPlayer = Game.GetActivePlayer();
	local pPlayer = Players[iPlayer];
	local pTeam = Teams[pPlayer:GetTeam()];
	
	local pOtherPlayer;
	local iOtherTeam;
	
	local tPlayers = {};
	local tCities = {};
	
	-- Pick a random dude's name
	
	print("Number of dudes to roll from: " .. #g_tListOfDudes);
	
	local iRand = Game.Rand(#g_tListOfDudes, "Rolling to determine Progress Screen dude.");
	iRand = iRand + 1;	-- Screw you, Lua
	local strDudesName = g_tListOfDudes[iRand];
	
	print("iRand: " .. iRand);
	print("strDudesName: " .. strDudesName);
	
	local strPresents = Locale.ConvertTextKey("TXT_KEY_PROGRESS_SCREEN_TITLE", strDudesName);
	Controls.PresentsLabel:SetText(strPresents);
	
	-- Figure out which list we'll be displaying - don't show the same mode we showed last time
	g_iLastListMode = g_iListMode;
	iRand = g_iLastListMode;
	
	local bInvalid = false;
	
	-- Keep rolling until we get a new mode
	while iRand == g_iLastListMode or bInvalid do
		bInvalid = false;
		
		iRand = Game.Rand(#g_tListModeText+1, "Rolling to determine Progress Screen list type.");
		
		-- Some modes aren't valid (e.g. wonders before anyone's built any)
		if (not IsListModeAllowed(iRand)) then
			bInvalid = true;
		end
		
	end
	
	g_iListMode = iRand;
	
	local strListName = Locale.ConvertTextKey(g_tListModeText[g_iListMode]);
	Controls.ListNameLabel:SetText(strListName);
	
	-- Tooltip
	local strToolTip = g_tListModeText[g_iListMode] .. "_TT";	-- Sorta hardcoded, but convenient
	Controls.PresentsLabel:SetToolTipString(Locale.ConvertTextKey(strToolTip));
	Controls.ListNameLabel:SetToolTipString(Locale.ConvertTextKey(strToolTip));

	-- Special case for list of cities	
	if (g_iListMode == g_ListMode_Tourist_Cities) then
	
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pOtherPlayer = Players[iPlayerLoop];
			
			-- Player has to be alive to be in the list
			if (pOtherPlayer:IsAlive()) then
			
				for cityIndex = 0, pOtherPlayer:GetNumCities() - 1, 1 do
    				local pCity = pOtherPlayer:GetCityByID(cityIndex);
    				if (pCity ~= nil) then
						pCity:RefreshTourism();
						if(pCity:GetBaseTourism() > 0) then
							table.insert(tCities, {iPlayerLoop, cityIndex, pCity:GetBaseTourism()});
						end
					end
				end
			end
		end
		
		-- Sort our list of cities
		local f = function(a, b)
			return a[3] > b[3];
		end
		
		table.sort(tCities, f);
		
		local iCount = 0;
		for i, v in ipairs(tCities) do
			if (iCount < 10 and v[3] > 0) then
				AddCityEntry(v[1], v[2], v[3], i);		-- PlayerID, City ID, Score, Rank
				iCount = iCount + 1;
			end
		end
		
	-- Normal case (list of players)
	else
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			
			pOtherPlayer = Players[iPlayerLoop];
			iOtherTeam = pOtherPlayer:GetTeam();
			
			-- Player has to be alive to be in the list
			if (pOtherPlayer:IsAlive()) then
				table.insert(tPlayers, {iPlayerLoop, GetScoreFromMode(iPlayerLoop, g_iListMode)});
			end
		end
		
		-- Sort our list of players
		local f = function(a, b)
			return a[2] > b[2];
		end
		
		table.sort(tPlayers, f);
		
		-- Display sorted list
		for i, v in ipairs(tPlayers) do
			AddPlayerEntry(v[1], v[2], i);		-- PlayerID, Score, Rank
		end
	end
	
	Controls.PlayerListStack:CalculateSize();
	Controls.PlayerListStack:ReprocessAnchoring();
	Controls.PlayerListScrollPanel:CalculateInternalSize();

	UIManager:QueuePopup( ContextPtr, PopupPriority.WhosWinning );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function IsListModeAllowed(iMode)
    
    -- Culture Mode restrictions
    if (iMode == g_ListMode_Culture) then
		
		local pLoopPlayer;
		
		-- Does everyone (alive) have at least 2 Policies?
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pLoopPlayer = Players[iPlayerLoop];
			if (pLoopPlayer:IsAlive()) then
				if (pLoopPlayer:GetNumPolicies() < 2) then
					print("Progress Screen: Can't display Culture Mode because at least 1 player has fewer than 2 Policies!");
					return false;
				end
			end
		end
		
		-- Anyone in the Industrial Era?
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pLoopPlayer = Players[iPlayerLoop];
			if (pLoopPlayer:IsAlive()) then
				if (pLoopPlayer:GetCurrentEra() >= GameInfo.Eras["ERA_INDUSTRIAL"].ID) then
					print("Progress Screen: Can't display Culture Mode because we've reached the Industrial Era!");
					return false;
				end
			end
		end

    -- Cultural Influence restrictions
    elseif (iMode == g_ListMode_Cultural_Influence) then
    
		local bUse = false;
		
		-- Does anyone have influence over another civ?
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pLoopPlayer = Players[iPlayerLoop];
			if (pLoopPlayer:IsAlive()) then
				if (pLoopPlayer:GetNumCivsInfluentialOn() > 0) then
					bUse = true;			
				end
			end
		end
		if (not bUse) then
			print("Progress Screen: Can't display Cultural Influence because no player has influence over another!");
			return false;
		end			
		
    -- Tourism restrictions
    elseif (iMode == g_ListMode_Tourist_Cities) then
		
		local bNoTourism = true;
		
		-- Need someone generating tourism
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pLoopPlayer = Players[iPlayerLoop];
			if (pLoopPlayer:IsAlive()) then
				if (pLoopPlayer:GetTourism() > 0) then
					bNoTourism = false;
				end
			end
		end
		
		if (bNoTourism) then
			print("Progress Screen: Can't display Tourism because no players are generating any!");
			return false;
		else
			print("Progress Screen: Found someone generating tourism!");	
		end
		
    -- Food Mode restrictions
    elseif (iMode == g_ListMode_Food) then
		
		-- Anyone in the Industrial Era?
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pLoopPlayer = Players[iPlayerLoop];
			if (pLoopPlayer:IsAlive()) then
				if (pLoopPlayer:GetCurrentEra() >= GameInfo.Eras["ERA_INDUSTRIAL"].ID) then
					print("Progress Screen: Can't display Food Mode because we've reached the Industrial Era!");
					return false;
				end
			end
		end
			
    -- Production Mode restrictions
    elseif (iMode == g_ListMode_Production) then
		
		-- Anyone in the Industrial Era?
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
			pLoopPlayer = Players[iPlayerLoop];
			if (pLoopPlayer:IsAlive()) then
				if (pLoopPlayer:GetCurrentEra() >= GameInfo.Eras["ERA_INDUSTRIAL"].ID) then
					print("Progress Screen: Can't display Production Mode because we've reached the Industrial Era!");
					return false;
				end
			end
		end

    -- Wonder Mode restrictions
    elseif (iMode == g_ListMode_Wonders) then
		if (Game.GetNumWorldWonders() < 2) then
			print("Progress Screen: Can't display wonders because there are less than 2 in the world!");
			return false;
		end
    
    -- Happiness restrictions
    elseif (iMode == g_ListMode_Happiness) then
		if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_HAPPINESS)) then
			print("Progress Screen: Can't display Happiness because the system is off!");
			return false;
		end
		
	-- Science restrictions
	elseif (iMode == g_ListMode_Science) then
		if (Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE)) then
			print("Progress Screen: Can't display Science because the system is off!");
			return false;		
		end
    end
		 
    return true;
    
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function GetScoreFromMode(iPlayer, iMode)
    
    local pPlayer = Players[iPlayer];
    
        -- Food Mode
	if (iMode == g_ListMode_Food) then
		local iTotalFoodSurplus = 0;
		local iTotalCities = 0;
		
		for pLoopCity in pPlayer:Cities() do
			iTotalFoodSurplus = iTotalFoodSurplus + pLoopCity:FoodDifference();
			iTotalCities = iTotalCities + 1;
		end
		
		if iTotalCities == 0 then
			iTotalCities = 1;
		end
		
		return iTotalFoodSurplus / iTotalCities;
		
	-- Production Mode
	elseif (iMode == g_ListMode_Production) then
		local iTotalProduction = 0;
		local iTotalCities = 0;
		
		local bIgnoreFood = true;
		local bOverflow = false;
		
		for pLoopCity in pPlayer:Cities() do
			iTotalProduction = iTotalProduction + pLoopCity:GetCurrentProductionDifference(bIgnoreFood, bOverflow);
			iTotalCities = iTotalCities + 1;
		end
		
		if iTotalCities == 0 then
			iTotalCities = 1;
		end
		
		return iTotalProduction / iTotalCities;
		
	-- Gold Mode
	elseif (iMode == g_ListMode_Gold) then
		return pPlayer:GetGold();
		
	-- Science Mode
	elseif (iMode == g_ListMode_Science) then
		local pTeam = Teams[pPlayer:GetTeam()];
		return pTeam:GetTeamTechs():GetNumTechsKnown();
		
	-- Culture Mode
	elseif (iMode == g_ListMode_Culture) then
		return pPlayer:GetNumPolicies();
		
	-- Happiness Mode
	elseif (iMode == g_ListMode_Happiness) then
		return pPlayer:GetExcessHappiness();
		
	-- Wonders Mode
	elseif (iMode == g_ListMode_Wonders) then
		return pPlayer:GetNumWorldWonders();
		
	-- Power Mode
	elseif (iMode == g_ListMode_Power) then
		return pPlayer:GetMilitaryMight();
		
	-- Cultural Influence
	elseif (iMode == g_ListMode_Cultural_Influence) then
	    return pPlayer:GetNumCivsInfluentialOn();
	    
    end
    
    return 0;
    
end


-------------------------------------------------
-- AddPlayerEntry
-------------------------------------------------
function AddPlayerEntry (iPlayerID, iScore, iRank)

	local pPlayer = Players[iPlayerID];
	local iTeam = pPlayer:GetTeam();
	local pTeam = Teams[iTeam];

	local controlTable = g_PlayerListInstanceManager:GetInstance();

    -- Use the Civilization_Leaders table to cross reference from this civ to the Leaders table
    local leader = GameInfo.Leaders[pPlayer:GetLeaderType()];
    local leaderDescription = leader.Description;
        
	local strName = Locale.ConvertTextKey("TXT_KEY_PROGRESS_SCREEN_NUMBERING", iRank);
	
	-- Active player
	if (pPlayer:GetID() == Game.GetActivePlayer()) then
		strName = strName .. " " .. Locale.ConvertTextKey( "TXT_KEY_POP_VOTE_RESULTS_YOU" );
		CivIconHookup( iPlayerID, 32, controlTable.Icon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true);  
		IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
	-- Haven't yet met this player
	elseif (not pTeam:IsHasMet(Game.GetActiveTeam())) then
		strName = strName .. " " .. Locale.ConvertTextKey( "TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER" );
		CivIconHookup( -1, 32, controlTable.Icon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true);
        IconHookup( 22, 64, "LEADER_ATLAS", controlTable.Portrait );
	-- Met players
	else
		strName = strName .. " " .. Locale.ConvertTextKey(pPlayer:GetNameKey());
		CivIconHookup( iPlayerID, 32, controlTable.Icon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true);  
		IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
	end    
	
	controlTable.PlayerNameText:SetText(strName);
	
	-- Formats the # and reduces it to X.XX
	local strPoints = Locale.ConvertTextKey("TXT_KEY_FORMAT_NUMBER", iScore);
	controlTable.ScoreText:SetText(strPoints);
	
	-- Tooltip
	local strToolTip = g_tListModeText[g_iListMode] .. "_TT";	-- Sorta hardcoded, but convenient
	controlTable.PlayerEntryBox:SetToolTipString(Locale.ConvertTextKey(strToolTip));
	
end

-------------------------------------------------
-- AddCityEntry
-------------------------------------------------
function AddCityEntry (iPlayerID, iCityID, iScore, iRank)

	local pPlayer = Players[iPlayerID];
	local iTeam = pPlayer:GetTeam();
	local pTeam = Teams[iTeam];
	local pCity = pPlayer:GetCityByID(iCityID);

	local controlTable = g_PlayerListInstanceManager:GetInstance();

    -- Use the Civilization_Leaders table to cross reference from this civ to the Leaders table
    local leader = GameInfo.Leaders[pPlayer:GetLeaderType()];
    local leaderDescription = leader.Description;
        
	local strName = Locale.ConvertTextKey("TXT_KEY_PROGRESS_SCREEN_NUMBERING", iRank);
	
	-- Haven't yet met this player
	if (not pTeam:IsHasMet(Game.GetActiveTeam())) then
		strName = strName .. " " .. Locale.ConvertTextKey( "TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER" );
		CivIconHookup( -1, 32, controlTable.Icon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true);
        IconHookup( 22, 64, "LEADER_ATLAS", controlTable.Portrait );
	-- Met players
	else
		strName = strName .. " " .. Locale.ConvertTextKey(pCity:GetNameKey());
		CivIconHookup( iPlayerID, 32, controlTable.Icon, controlTable.CivIconBG, controlTable.CivIconShadow, false, true);  
		IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.Portrait );
	end    
	
	controlTable.PlayerNameText:SetText(strName);
	
	-- Formats the # and reduces it to X.XX
	local strPoints = Locale.ConvertTextKey("TXT_KEY_FORMAT_NUMBER", iScore);
	controlTable.ScoreText:SetText(strPoints);
	
	-- Tooltip
	local strToolTip = g_tListModeText[g_iListMode] .. "_TT";	-- Sorta hardcoded, but convenient
	controlTable.PlayerEntryBox:SetToolTipString(Locale.ConvertTextKey(strToolTip));
	
end

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
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_WHOS_WINNING, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose);

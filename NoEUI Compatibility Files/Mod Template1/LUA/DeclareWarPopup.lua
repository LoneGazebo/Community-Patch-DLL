print("This is the modded DeclareWarPopup from CBP - C4DF")
-------------------------------------------------
-- Declare War Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );

local g_AlliedCityStatesInstanceManager = GenerationalInstanceManager:new( "CityStateInstance", "Base", Controls.AlliedCityStatesStack);
-- CBP
local g_DefensePactsInstanceManager = GenerationalInstanceManager:new( "DPInstance", "Base", Controls.DefensePactStack);
-- END
local g_UnderProtectionOfInstanceManager = GenerationalInstanceManager:new( "UnderProtectionCivInstance", "Base", Controls.UnderProtectionOfStack);
local g_ActiveTradesInstanceManager = GenerationalInstanceManager:new("TradeRouteInstance", "Base", Controls.TradeRoutesStack);
-- Putmalk
local g_MastersInstanceManager = GenerationalInstanceManager:new( "MasterInstance", "Base", Controls.MastersStack);
local g_VassalsInstanceManager = GenerationalInstanceManager:new( "VassalInstance", "Base", Controls.VassalsStack);

PopupLayouts = {};
PopupInputHandlers = {};
g_PopupInfo = nil;		-- The current popupinfo.

-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

------------------------------------------------
-- Misc Utility Methods
------------------------------------------------
-- Hides popup window.
function HideWindow()
    UIManager:DequeuePopup( ContextPtr );
    ClearButtons();
end

-- Sets popup text.
function SetPopupText(popupText)
	Controls.PopupText:SetText(popupText);
end

-- Remove all buttons from popup.
function ClearButtons()
	local i = 1;
	repeat
		local button = Controls["Button"..i];
		if button then
			button:SetHide(true);
		end
		i = i + 1;
	until button == nil;
end

-- Add a button to popup.
-- NOTE: Current Limitation is 4 buttons
function AddButton(buttonText, buttonClickFunc, strToolTip, bPreventClose)
	local i = 1;
	repeat
		local button = Controls["Button"..i];
		if button and button:IsHidden() then
			local buttonLabel = Controls["Button"..i.."Text"];
			buttonLabel:SetText( buttonText );
			
			button:SetToolTipString(strToolTip);

			--By default, button clicks will hide the popup window after
			--executing the click function
			local clickHandler = function()
				if buttonClickFunc ~= nil then
					buttonClickFunc();
				end
				
				HideWindow();
			end
			local clickHandlerPreventClose = function()
				if buttonClickFunc ~= nil then
					buttonClickFunc();
				end
			end
			
			-- This is only used in one case, when viewing a captured city (PuppetCityPopup)
			if (bPreventClose) then
				button:RegisterCallback(Mouse.eLClick, clickHandlerPreventClose);
			else
				button:RegisterCallback(Mouse.eLClick, clickHandler);
			end
			
			button:SetHide(false);
			
			return;
		end
		i = i + 1;
	until button == nil;
end

-------------------------------------------------
-- On Display
-------------------------------------------------
function OnDisplay(popupInfo)
	
	g_PopupInfo = popupInfo;
	-- Initialize popups
	local initialize = PopupLayouts[popupInfo.Type];
	if initialize then
		ClearButtons();
		if(initialize(popupInfo)) then
			if(popupInfo.Option1) then
				UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderHeadPopup);
			else
				UIManager:QueuePopup( ContextPtr, PopupPriority.GenericPopup );		
			end
		end
		
	end
end
Events.SerialEventGameMessagePopup.Add( OnDisplay );


-------------------------------------------------
-- Keyboard Handler
-------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )

	----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    if(uiMsg == KeyEvents.KeyDown and wParam == Keys.VK_ESCAPE) then
        HideWindow();
	    return true;
    end
end
ContextPtr:SetInputHandler( InputHandler );


function GatherData(RivalId, Text)
	local data = {};
	
	data.Text = Text;
	
	data.LeaderId = Game.GetActivePlayer();

	local activePlayer = Players[data.LeaderId];	
	local leader = GameInfo.Leaders[activePlayer:GetLeaderType()];
	
	data.LeaderIcon = {
		PortraitIndex = leader.PortraitIndex,
		IconAtlas = leader.IconAtlas
	};
	
	data.RivalId = RivalId;
	local rivalPlayer = Players[data.RivalId];
	
	local rivalCiv = GameInfo.Civilizations[rivalPlayer:GetCivilizationType()];
	if(rivalCiv.Type == "CIVILIZATION_MINOR") then
		data.RivalIsMinor = true;	
	else
		data.RivalIsMinor = false;
		local rivalLeader = GameInfo.Leaders[rivalPlayer:GetLeaderType()];
	
		data.RivalIcon = {
			PortraitIndex = rivalLeader.PortraitIndex,
			IconAtlas = rivalLeader.IconAtlas
		};
	end
	
	if (not data.RivalIsMinor) then
		data.HasDoF = activePlayer:IsDoF(RivalId);
		if(activePlayer.GetDoFCounter ~= nil and GameDefines.DOF_EXPIRATION_TIME ~= nil) then
			data.DoFTurnsLeft = GameDefines.DOF_EXPIRATION_TIME - activePlayer:GetDoFCounter(RivalId);
		end
			
		data.WeDenouncedThem = activePlayer:IsDenouncedPlayer(RivalId);
		if(activePlayer.GetDenouncedPlayerCounter and GameDefines.DENUNCIATION_EXPIRATION_TIME ~= nil) then
			data.WeDenouncedThemTurnsLeft = GameDefines.DENUNCIATION_EXPIRATION_TIME - activePlayer:GetDenouncedPlayerCounter(RivalId);
		end
		
		data.TheyDenouncedUs = rivalPlayer:IsDenouncedPlayer(data.LeaderId);
		if(rivalPlayer.GetDenouncedPlayerCounter and GameDefines.DENUNCIATION_EXPIRATION_TIME ~= nil) then
			data.TheyDenouncedUsTurnsLeft = GameDefines.DENUNCIATION_EXPIRATION_TIME - rivalPlayer:GetDenouncedPlayerCounter(data.LeaderId);
		end
		--CBP
		data.TheyHaveDefensivePactWithPlayer = rivalPlayer:IsHasDefensivePactWithPlayer(data.LeaderId);
		--END
	end
	
	-- Rival Civ + his Allies
	local rivalIDs = {
		[RivalId] = true,
	};
	
	data.AlliedCityStates = {};
	
	for iPlayerLoop = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS - 1, 1 do	
		local cityState = Players[iPlayerLoop];
		if(cityState and cityState:GetAlly() == RivalId) then
			rivalIDs[iPlayerLoop] = true;
			local playerColor = GameInfo.PlayerColors[cityState:GetPlayerColor()];
			local primaryColor = GameInfo.Colors[playerColor.PrimaryColor];
			local secondaryColor = GameInfo.Colors[playerColor.SecondaryColor];
			table.insert(data.AlliedCityStates, {
				PlayerID = iPlayerLoop,
				Name = cityState:GetCivilizationDescriptionKey(),
				PrimaryColor = primaryColor,
				SecondaryColor = secondaryColor,
			});
		end
	end
	
-- CBP
	-- Rival Civ + his DP
	data.DefensePacts = {};
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS - 1, 1 do
		local player = Players[iPlayerLoop];
		if(player and player ~= activePlayer and player:IsHasDefensivePactWithPlayer(RivalId) and player:IsAlive()) then
			print("Found DP!");
			table.insert(data.DefensePacts, {
				PlayerID = iPlayerLoop,
				Name = player:GetCivilizationShortDescriptionKey(),
			});
		end
	end
-- END

	data.UnderProtectionOf = {};
	if(data.RivalIsMinor) then
		for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS - 1, 1 do	
			local player = Players[iPlayerLoop];
			if(player and player:IsAlive() and player:IsProtectingMinor(RivalId)) then
				table.insert(data.UnderProtectionOf, {
					PlayerID = iPlayerLoop,
					Name = player:GetCivilizationShortDescriptionKey(),
				});
			end
		end	
	end
	
	-- Putmalk
	local rivalTeam = Teams[rivalPlayer:GetTeam()];
	
	data.Masters = {};
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS - 1, 1 do
		local master = Players[iPlayerLoop];
		local masterTeam = Teams[ master:GetTeam() ];
		if(master and rivalTeam:GetMaster() == master:GetTeam() and master:IsAlive()) then
			print("Found Master!");
			table.insert(data.Masters, {
				PlayerID = iPlayerLoop,
				Name = master:GetCivilizationShortDescriptionKey(),
			});
		end
	end

	data.Vassals = {};
	for iPlayerLoop = 0, GameDefines.MAX_MAJOR_CIVS - 1, 1 do
		local vassal = Players[iPlayerLoop];
		local vassalTeam = Teams[ vassal:GetTeam() ];
		if(vassal and vassalTeam:GetMaster() == rivalPlayer:GetTeam() and vassal:IsAlive()) then
			print("Found Vassal!");
			table.insert(data.Vassals, {
				PlayerID = iPlayerLoop,
				Name = vassal:GetCivilizationShortDescriptionKey(),
			});
		end
	end
	
	local dealsFromThem = {};
	local dealsFromUs = {};
	
	local ourResources = {};
	local theirResources = {};
	
	
	local iNumCurrentDeals = UI.GetNumCurrentDeals( data.LeaderId );    
    if( iNumCurrentDeals > 0 ) then
        for i = 0, iNumCurrentDeals - 1 do
			UI.LoadCurrentDeal( data.LeaderId, i );
			local deal = UI.GetScratchDeal();
			if(deal:GetOtherPlayer(data.LeaderId) == RivalId) then
				print("We have an active deal!");
				
				
				deal:ResetIterator();
				local itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = deal:GetNextItem();
    
				while(itemType ~= nil) do
					local bFromUs = (fromPlayer == data.LeaderId);
			        			        
					if( TradeableItems.TRADE_ITEM_GOLD_PER_TURN == itemType ) then
						if( bFromUs ) then
							local gpt = data1;
							local str = tostring(gpt) .. " " .. Locale.Lookup("TXT_KEY_DIPLO_GOLD_PER_TURN");
							table.insert(dealsFromUs, str);
						else
							local gpt = data1;
							local str = tostring(gpt) .. " " .. Locale.Lookup("TXT_KEY_DIPLO_GOLD_PER_TURN");
							table.insert(dealsFromThem, str);
						end
					elseif( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT == itemType ) then
			    		local iRACost = Game.GetResearchAgreementCost(g_iUs, g_iThem);
						local strRAString = Locale.ConvertTextKey("TXT_KEY_DIPLO_RESCH_AGREEMENT_US", iRACost);
						if( bFromUs ) then
							table.insert(dealsFromUs, strRAString);
						else
							table.insert(dealsFromThem, strRAString);
						end
			        			        
					elseif( TradeableItems.TRADE_ITEM_RESOURCES == itemType ) then
			        	local resource = GameInfo.Resources[data1];
						
						local str = Locale.Lookup("TXT_KEY_ACTIVE_DEAL_RESOURCE", data2, resource.IconString, resource.Description);
						if( bFromUs ) then
							table.insert(ourResources, str);
						else
							table.insert(theirResources, str);
						end
					elseif( TradeableItems.TRADE_ITEM_VOTE_COMMITMENT == itemType ) then
						if (Game.GetNumActiveLeagues() > 0) then
							local pLeague = Game.GetActiveLeague();
							if (pLeague ~= nil) then			
								
								local proposal;	
								if(flag1) then
									local repealProposals = pLeague:GetRepealProposals();
									for i,t in ipairs(repealProposals) do
										if(t.ID == data1) then
											proposal = t;
										end
									end
								else
									local enactProposals = pLeague:GetEnactProposals();
									for i,t in ipairs(enactProposals) do
										if(t.ID == data1) then
											proposal = t;
										end
									end
								end					
								
								if(proposal ~= nil) then
									
									local str = pLeague:GetResolutionName(proposal.Type, data1, proposal.ProposerDecision, true);
							
									local iVoterDecision = GameInfo.ResolutionDecisions[GameInfo.Resolutions[proposal.Type].VoterDecision].ID;
			
									local decision = pLeague:GetTextForChoice(iVoterDecision, data2);
									
									str = str .. " [ICON_ARROW_RIGHT] " .. decision;
									
									if(bFromUs) then
										table.insert(dealsFromUs, str);
									else
										table.insert(dealsFromThem, str);
									end
								
								end
								

							else
								print("SCRIPTING ERROR: Could not find League when displaying a trade deal with a Vote Commitment");
							end
						end
					end
					
					itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayer = deal:GetNextItem();
				end
			end
			
			if(deal ~= nil) then
				deal:ClearItems();
			end
        end
    end
    
    if(#ourResources > 0) then
		table.insert(dealsFromUs, table.concat(ourResources, ", "));
    end
    
    if(#theirResources > 0) then
		table.insert(dealsFromThem, table.concat(theirResources, ", "));
    end
	
	data.DealsFromUs = table.concat(dealsFromUs, "[NEWLINE]");
	data.DealsFromThem = table.concat(dealsFromThem, "[NEWLINE]");	
	
	local tradeRoutes = {};
	local myTrades = activePlayer:GetTradeRoutes();
	local theirTrades = activePlayer:GetTradeRoutesToYou();

	local bonusTips = {};
	
	local tips = {
		{
			Field = "GPT",
			Description = "TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_GOLD",
		},
		{
			Field = "Food",
			Description = "TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_FOOD",
		},
		{
			Field = "Production",
			Description = "TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_PRODUCTION",
		},
		{
			Field = "Science",
			Description = "TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_SCIENCE",
		},
	}
	
	function GetBonusTips(tradeRoute, bonuses, arrowIcon, direction)
		for i,v in ipairs(tips) do
			local value = tradeRoute[direction .. v.Field];
			if(value and value > 0) then
				table.insert(bonuses, arrowIcon .. " " .. Locale.Lookup(v.Description, value/100));
			end
		end
		
		-- special case religious pressure
		local pressure = tradeRoute[direction .. "Pressure"];
		if(pressure > 0) then
			local religionId = tradeRoute[direction .. "Religion"];
			local religion = GameInfo.Religions[religionId];
			table.insert(bonuses, arrowIcon .. " " .. Locale.Lookup("TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_RELIGIOUS_PRESSURE", religion.IconString, pressure));
		end
	end

	for i,v in ipairs(myTrades) do
		if(rivalIDs[v.ToID] == true) then
			v.TargetPlayerId = v.ToID;
			
			local bonuses = {};
			GetBonusTips(v, bonuses, "[ICON_ARROW_LEFT]", "From");
			GetBonusTips(v, bonuses, "[ICON_ARROW_RIGHT]", "To"); 
			
			v.Bonuses = table.concat(bonuses, "[NEWLINE]");
			
			table.insert(tradeRoutes, v);
		end
	end	
	
	for i,v in ipairs(theirTrades) do
		if(rivalIDs[v.FromID] == true) then
			v.TargetPlayerId = v.FromID;
			
			local bonuses = {};
			GetBonusTips(v, bonuses, "[ICON_ARROW_LEFT]", "To");
			GetBonusTips(v, bonuses, "[ICON_ARROW_RIGHT]", "From"); 
			
			v.Bonuses = table.concat(bonuses, "[NEWLINE]");
			
			table.insert(tradeRoutes, v);
		end
	end
	
	data.TradeRoutes = tradeRoutes;

	return data;
end

function View(data)
	Controls.PopupText:SetText(data.Text);
		
	IconHookup(data.LeaderIcon.PortraitIndex, 128, data.LeaderIcon.IconAtlas, Controls.LeaderIcon);           
	CivIconHookup(data.LeaderId, 45, Controls.LeaderSubIcon, Controls.LeaderSubIconBG, Controls.LeaderSubIconShadow, true, true, Controls.LeaderSubIconHighlight);
		
	Controls.DiplomacyNone:SetHide(false);
	
	function SizeParentToChildContent(parent, child, minWidth, minHeight, heightPadding)
		local cWidth, cHeight = child:GetSizeVal();
		parent:SetSizeVal(math.max(cWidth, minWidth), math.max(cHeight + heightPadding, minHeight));
	end
	
	if(data.HasDoF) then      
        local dofText = Locale.Lookup("TXT_KEY_DIPLO_DOF");
        if(data.DoFTurnsLeft ~= nil) then
			dofText = dofText .. " (" .. Locale.Lookup("TXT_KEY_DECLARE_WAR_DEALS_TURNS_LEFT", data.DoFTurnsLeft) .. ")";
        end
              
		Controls.DiplomacyNone:SetHide(true);
        Controls.DoFLabel:SetText(dofText);
        SizeParentToChildContent(Controls.HasDoF, Controls.DoFLabel, 478, 28, 20);
        Controls.HasDoF:SetHide(false);
	else
        Controls.HasDoF:SetHide(true);
	end
	
	if(data.WeDenouncedThem) then	
		local denouncedText = Locale.Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_US");
		if(data.WeDenouncedThemTurnsLeft ~= nil) then
			denouncedText = denouncedText .. " (" .. Locale.Lookup("TXT_KEY_DECLARE_WAR_DEALS_TURNS_LEFT", data.WeDenouncedThemTurnsLeft) .. ")";
        end
        
        Controls.DiplomacyNone:SetHide(true);
        Controls.WeDenouncedThemLabel:SetText(denouncedText);
        SizeParentToChildContent(Controls.WeDenouncedThem, Controls.WeDenouncedThemLabel, 478, 28, 20);
        Controls.WeDenouncedThem:SetHide(false);
	else
		Controls.WeDenouncedThem:SetHide(true);
	end
	
	if(data.TheyDenouncedUs) then
		local denouncedText = Locale.Lookup("TXT_KEY_DIPLO_DENOUNCED_BY_THEM");
		if(data.TheyDenouncedUsTurnsLeft ~= nil) then
			denouncedText = denouncedText .. " (" .. Locale.Lookup("TXT_KEY_DECLARE_WAR_DEALS_TURNS_LEFT", data.TheyDenouncedUsTurnsLeft) .. ")";
        end
        
        Controls.DiplomacyNone:SetHide(true);
        Controls.TheyDenouncedUsLabel:SetText(denouncedText);
        SizeParentToChildContent(Controls.TheyDenouncedUs, Controls.TheyDenouncedUsLabel, 478, 28, 20);
        Controls.TheyDenouncedUs:SetHide(false);
        
	else
		Controls.TheyDenouncedUs:SetHide(true);
	end

-- CBP
	if(data.TheyHaveDefensivePactWithPlayer) then
		local DPTextUs = Locale.Lookup("TXT_KEY_DIPLO_DEFENSE_PACT_WITH_US");
        Controls.DiplomacyNone:SetHide(true);
        Controls.TheyDPUsLabel:SetText(DPTextUs);
        SizeParentToChildContent(Controls.TheyHaveDefensivePactWithPlayer, Controls.TheyDPUsLabel, 478, 28, 20);
        Controls.TheyHaveDefensivePactWithPlayer:SetHide(false);   
	else
		Controls.TheyHaveDefensivePactWithPlayer:SetHide(true);
	end
-- END
		
	if(data.RivalIsMinor) then
	
	    CivIconHookup(data.RivalId, 128, Controls.RivalIcon, Controls.RivalIconBG, Controls.RivalIconShadow, true, true);
		
		Controls.RivalIconBG:SetHide(true);
		Controls.RivalIconShadow:SetHide(false);
		Controls.RivalSubIconFrame:SetHide(true);	
		
		Controls.AlliedCityStatesHeader:SetHide(true);
		Controls.AlliedCityStatesStack:SetHide(true);

-- CBP
		Controls.DefensePactHeader:SetHide(true);
		Controls.DefensePactStack:SetHide(true);
-- END
		
		Controls.ActiveDealsHeader:SetHide(true);
        Controls.ActiveDealsStack:SetHide(true);
		
		Controls.UnderProtectionOfHeader:SetHide(false);
		Controls.UnderProtectionOfStack:SetHide(false);
		
		-- Start Putmalk
		Controls.MastersHeader:SetHide(true);
		Controls.MastersStack:SetHide(true);

		Controls.VassalsHeader:SetHide(true);
		Controls.VassalsStack:SetHide(true);
	else
	    
	    local white = Vector4(1.0, 1.0, 1.0, 1.0);
	    Controls.RivalIcon:SetColor(white);
	    IconHookup(data.RivalIcon.PortraitIndex, 128, data.RivalIcon.IconAtlas, Controls.RivalIcon);           
		CivIconHookup(data.RivalId, 45, Controls.RivalSubIcon, Controls.RivalSubIconBG, Controls.RivalSubIconShadow, true, true, Controls.RivalSubIconHighlight);
		
		Controls.RivalIconBG:SetHide(true);
		Controls.RivalIconShadow:SetHide(true);
		Controls.RivalSubIconFrame:SetHide(false);
		
		Controls.UnderProtectionOfHeader:SetHide(true);
		Controls.UnderProtectionOfStack:SetHide(true);
		
		Controls.ActiveDealsHeader:SetHide(false);
        Controls.ActiveDealsStack:SetHide(false);
		
		Controls.AlliedCityStatesHeader:SetHide(false);
		Controls.AlliedCityStatesStack:SetHide(false);
		
		-- Putmalk
		Controls.MastersHeader:SetHide(false);
		Controls.MastersStack:SetHide(false);

		Controls.VassalsHeader:SetHide(false);
		Controls.VassalsStack:SetHide(false);
-- CBP
		Controls.DefensePactHeader:SetHide(false);
		Controls.DefensePactStack:SetHide(false);
-- END
	end
	
	g_AlliedCityStatesInstanceManager:ResetInstances();
	local alliedCityStates = data.AlliedCityStates or {};
	Controls.AlliedCityStatesNone:SetHide(#alliedCityStates > 0);
	
	for i,v in ipairs(alliedCityStates) do
	
		local instance = g_AlliedCityStatesInstanceManager:GetInstance();	
		CivIconHookup(v.PlayerID, 45, instance.Icon, instance.IconBG, instance.IconShadow, true, true, instance.IconHighlight);
		
		instance.Label:SetColor({x = v.SecondaryColor.Red, y = v.SecondaryColor.Green, z = v.SecondaryColor.Blue, w = 1.0}, 0);
		
		local name = Locale.Lookup(v.Name);
		instance.Label:SetText(name);
		instance.IconFrame:SetToolTipString(name);
	end	
-- CBP
	g_DefensePactsInstanceManager:ResetInstances();
	local defensePacts = data.DefensePacts or {};
	Controls.DefensePactNone:SetHide(#defensePacts > 0);
	
	for i,v in ipairs(defensePacts) do
	
		local instance = g_DefensePactsInstanceManager:GetInstance();	
		CivIconHookup(v.PlayerID, 45, instance.Icon, instance.IconBG, instance.IconShadow, true, true, instance.IconHighlight);
		
		local name = Locale.Lookup(v.Name);
		instance.Label:SetText(name);
		instance.IconFrame:SetToolTipString(name);
	end	
-- END
	g_UnderProtectionOfInstanceManager:ResetInstances();
	local underProtectionOf = data.UnderProtectionOf or {};
	Controls.UnderProtectionOfNone:SetHide(#underProtectionOf > 0);
	
	for i,v in ipairs(underProtectionOf) do
	
		local instance = g_UnderProtectionOfInstanceManager:GetInstance();	
		CivIconHookup(v.PlayerID, 45, instance.Icon, instance.IconBG, instance.IconShadow, true, true, instance.IconHighlight);
		
		local name = Locale.Lookup(v.Name);
		instance.Label:SetText(name);
		instance.IconFrame:SetToolTipString(name);
	end	
	
	-- Start Putmalk
	g_MastersInstanceManager:ResetInstances();
	local masters = data.Masters or {};
	Controls.MastersNone:SetHide(#masters > 0);
	
	for i,v in ipairs(masters) do
	
		local instance = g_MastersInstanceManager:GetInstance();	
		CivIconHookup(v.PlayerID, 45, instance.Icon, instance.IconBG, instance.IconShadow, true, true, instance.IconHighlight);
		
		local name = Locale.Lookup(v.Name);
		instance.Label:SetText(name);
		instance.IconFrame:SetToolTipString(name);
	end	

	g_VassalsInstanceManager:ResetInstances();
	local vassals = data.Vassals or {};
	Controls.VassalsNone:SetHide(#vassals > 0);
	
	for i,v in ipairs(vassals) do
	
		local instance = g_VassalsInstanceManager:GetInstance();	
		CivIconHookup(v.PlayerID, 45, instance.Icon, instance.IconBG, instance.IconShadow, true, true, instance.IconHighlight);
		
		local name = Locale.Lookup(v.Name);
		instance.Label:SetText(name);
		instance.IconFrame:SetToolTipString(name);
	end	
	-- End Putmalk
     
	local hasDeals = false;
	local hasDealsFromThem = false;
	local hasDealsFromUs = false;
	if(data.DealsFromThem and #data.DealsFromThem > 0) then
		Controls.DealsFromThemHeader:SetHide(false);
		Controls.DealsFromThem:SetHide(false);
		Controls.DealsFromThem:SetText(data.DealsFromThem);
		hasDeals = true;
		hasDealsFromThem = true;
	else
		Controls.DealsFromThemHeader:SetHide(true);
		Controls.DealsFromThem:SetHide(true);
	end
	
	if(data.DealsFromUs and #data.DealsFromUs > 0) then
		Controls.DealsFromYouHeader:SetHide(false);
		Controls.DealsFromYou:SetHide(false);
		Controls.DealsFromYou:SetText(data.DealsFromUs);
		hasDeals = true;
		hasDealsFromUs = true;
	else
		Controls.DealsFromYouHeader:SetHide(true);
		Controls.DealsFromYou:SetHide(true);
	end
	
	Controls.ActiveDealsNone:SetHide(hasDeals);
	Controls.DealsSeparator:SetHide(hasDealsFromThem == false or hasDealsFromUs == false);
			
	g_ActiveTradesInstanceManager:ResetInstances();
	local tradeRoutes = data.TradeRoutes or {};
	Controls.TradeRoutesNone:SetHide(#tradeRoutes > 0);
	for _, tradeRoute in ipairs(tradeRoutes) do
		
		local itemInstance = g_ActiveTradesInstanceManager:GetInstance();
		
		itemInstance.CityName:SetText(tradeRoute.FromCityName .. " [ICON_ARROW_RIGHT] " .. tradeRoute.ToCityName);		
		itemInstance.Bonuses:SetText(tradeRoute.Bonuses);
	
		CivIconHookup(tradeRoute.TargetPlayerId, 64, itemInstance.CivIcon, itemInstance.CivIconBG, itemInstance.CivIconShadow, true, true );
		itemInstance.Base:SetToolTipString(tradeRoute.ToolTip);
	
		local buttonWidth, buttonHeight = itemInstance.Base:GetSizeVal();
		local descWidth, descHeight = itemInstance.Bonuses:GetSizeVal();		
		local newHeight = math.max(80, descHeight + 40);	
		
		itemInstance.Base:SetSizeVal(buttonWidth, newHeight);
	end
		
	Controls.DiplomacyStack:ReprocessAnchoring();
	Controls.DiplomacyStack:CalculateSize();	
	Controls.AlliedCityStatesStack:ReprocessAnchoring();
	Controls.AlliedCityStatesStack:CalculateSize();
-- CBP
	Controls.DefensePactStack:ReprocessAnchoring();
	Controls.DefensePactStack:CalculateSize();
-- END
	Controls.UnderProtectionOfStack:ReprocessAnchoring();
	Controls.UnderProtectionOfStack:CalculateSize();
	Controls.ActiveDealsStack:ReprocessAnchoring();
	Controls.ActiveDealsStack:CalculateSize();
	Controls.TradeRoutesStack:ReprocessAnchoring();
	Controls.TradeRoutesStack:CalculateSize();
	---[[ Putmalk
	Controls.MastersStack:ReprocessAnchoring();
	Controls.MastersStack:CalculateSize();
	Controls.VassalsStack:ReprocessAnchoring();
	Controls.VassalsStack:CalculateSize();
	--]]
	
	Controls.DeclareWarDetailsStack:ReprocessAnchoring();
	Controls.DeclareWarDetailsStack:CalculateSize();
	
	Controls.DeclareWarDetailsScrollPanel:CalculateInternalSize();
	
	Controls.ButtonStack:ReprocessAnchoring();
	Controls.ButtonStack:CalculateSize();
	
	
	local frameWidth, frameHeight = Controls.ButtonStackFrame:GetSizeVal();
	local stackWidth, stackHeight = Controls.ButtonStack:GetSizeVal();
	
	Controls.ButtonStackFrame:SetSizeVal(frameWidth, stackHeight + 160);
	
	Controls.ButtonStackFrame:ReprocessAnchoring();
end

-- DECLARE WAR MOVE POPUP
-- This popup occurs when a team unit moves onto rival territory
-- or attacks an rival unit
PopupLayouts[ButtonPopupTypes.BUTTONPOPUP_DECLAREWAR_PLUNDER_TRADE_ROUTE] = function(popupInfo)
	local eRivalTeam = popupInfo.Data1;
	local eOtherTeam = popupInfo.Data2;
	local popupText;
	
	-- If there's no rival team, let the other popup handle the warning.
	if(eRivalTeam == nil or eRivalTeam == -1) then
		return false;
	end		
	
	-- slewis - removed this because we're no longer upsetting others when trade routes are plundered.
	--if (eOtherTeam ~= -1) then
	--	popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR_PLUS_PILLAGE_UPSET", Teams[eRivalTeam]:GetNameKey(), Teams[eOtherTeam]:GetNameKey());
	--else
		popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR", Teams[eRivalTeam]:GetNameKey());
	--end
	
	local data = GatherData(Teams[eRivalTeam]:GetLeaderID(), popupText);
	View(data);
	
	-- Initialize 'yes' button.
	local OnYesClicked = function()
		-- Send War netmessage.
		Network.SendChangeWar(eRivalTeam, true);	
		
		-- Diplomatic response from AI
		if (not Teams[eRivalTeam]:IsMinorCiv() and not Teams[eRivalTeam]:IsHuman()) then
			if (not Game.IsNetworkMultiPlayer()) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR, Teams[eRivalTeam]:GetLeaderID(), 0, 0 );
			end
		end
		
		if (eOtherTeam ~= -1) then
			Network.SendIgnoreWarning(eOtherTeam);
		end
		
		Game.SelectionListGameNetMessage(GameMessageTypes.GAMEMESSAGE_PUSH_MISSION, MissionTypes.MISSION_PLUNDER_TRADE_ROUTE, -1, -1, 0, false, false);
	end
	
	local buttonText = Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_YES");
	AddButton(buttonText, OnYesClicked);
	
	-- Initialize 'no' button.
	local buttonText = Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_NO");
	AddButton(buttonText, nil);	
	
	return true;
end

-- DECLARE WAR MOVE POPUP
-- This popup occurs when a team unit moves onto rival territory
-- or attacks an rival unit
PopupLayouts[ButtonPopupTypes.BUTTONPOPUP_DECLAREWARMOVE] = function(popupInfo)
	local eRivalTeam = popupInfo.Data1;
	local iX = popupInfo.Data2;
	local iY = popupInfo.Data3;
	
	local popupText;
	local plot;
	if(popupInfo.Option1 ~= true and iX ~= nil and iY ~= nil and iX > -1 and iY > -1) then
		plot = Map.GetPlot(iX, iY);
	end
	-- Declaring war by entering player's lands
	if (plot and plot:GetTeam() == eRivalTeam and (not Teams[eRivalTeam]:IsMinorCiv() or plot:IsCity())) then
		local owner = Players[plot:GetOwner()];
		
		-- If not a Minor and allowed to make open borders, ask about that
		if (not owner:IsMinorCiv() and Teams[Game.GetActiveTeam()]:IsOpenBordersTradingAllowedWithTeam(plot:GetTeam())) then
			popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR", Teams[eRivalTeam]:GetNameKey());
			popupText = popupText .. "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_POPUP_ENTER_WITH_OPEN_BORDERS");
			
		-- Normal DoW message
		else
			if (owner:IsMinorCiv()) then
				popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_ENTER_CITY_STATE_WAR", Teams[eRivalTeam]:GetNameKey());			
			else
				popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_ENTER_LANDS_WAR", Teams[eRivalTeam]:GetNameKey());			
			end
		end
		
	-- Declaring war by attacking Unit
	else
	    popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR", Teams[eRivalTeam]:GetNameKey());
	end
	
	local data = GatherData(Teams[eRivalTeam]:GetLeaderID(), popupText);
	View(data);
	
	-- Initialize 'yes' button.
	local OnYesClicked = function()
		-- Send War netmessage.
		Network.SendChangeWar(eRivalTeam, true);
		
		-- Diplomatic response from AI
		if (not Teams[eRivalTeam]:IsMinorCiv() and not Teams[eRivalTeam]:IsHuman()) then
			if (not Game.IsNetworkMultiPlayer() or plot == nil) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR, Teams[eRivalTeam]:GetLeaderID(), 0, 0 );
			end
		end
		
		-- Tell unit to move to position.
		if(plot ~= nil) then
			Game.SelectionListMove(plot, false, false, false);
		end
	end
	
	local buttonText = Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_YES");
	AddButton(buttonText, OnYesClicked);
	
	-- Initialize 'no' button.
	local buttonText = Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_NO");
	AddButton(buttonText, nil);
	
	return true;
end

-- DECLARE WAR RANGE STRIKE POPUP
-- This popup occurs when a team unit attempts a range strike
-- attack on a rival unit or city.
PopupLayouts[ButtonPopupTypes.BUTTONPOPUP_DECLAREWARRANGESTRIKE] = function(popupInfo)
	local eRivalTeam	= popupInfo.Data1;
	local iX			= popupInfo.Data2;
	local iY			= popupInfo.Data3;

	local rivalTeam = Teams[eRivalTeam];
	popupText = Locale.ConvertTextKey("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR", rivalTeam:GetName());
		
	local data = GatherData(Teams[eRivalTeam]:GetLeaderID(), popupText);
	View(data);
	
	-- Initialize 'yes' button.
	local OnYesClicked = function()
		-- Send War netmessage.
		Network.SendChangeWar(eRivalTeam, true);

		-- Diplomatic response from AI
		if (not rivalTeam:IsMinorCiv() and not rivalTeam:IsHuman()) then
			if (not Game.IsNetworkMultiPlayer()) then
				Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR, rivalTeam:GetLeaderID(), 0, 0 );
			end
		end
		
		-- Attack!
		local messagePushMission = GameMessageTypes.GAMEMESSAGE_PUSH_MISSION
		local missionRangeAttack = MissionTypes.MISSION_RANGE_ATTACK
		Game.SelectionListGameNetMessage(messagePushMission, missionRangeAttack, iX, iY);
		
		local interfaceModeSelection = InterfaceModeTypes.INTERFACEMODE_SELECTION
		UI.SetInterfaceMode(interfaceModeSelection);
		
	end
	
	local buttonText = Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_YES");
	AddButton(buttonText, OnYesClicked);
		
	-- Initialize 'no' button.
	local buttonText = Locale.ConvertTextKey("TXT_KEY_DECLARE_WAR_NO");
	AddButton(buttonText, nil);
	
	return true;
end


local oldCursor; -- The previous cursor being used.
function ShowHideHandler( bIsHide, bInitState )
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore(); 
        	
        	oldCursor = UIManager:SetUICursor(0); -- make sure we start with the default cursor
        	
        	if(g_PopupInfo ~= nil) then        	
        		Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        		Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
        	end
        else
			UIManager:SetUICursor(oldCursor);
			if(g_PopupInfo ~= nil) then
		--		Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
			end
            UI.decTurnTimerSemaphore();
            
            
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------
-- Collapse/Expand Behaviors --
-------------------------------
function OnCollapseExpand()
	Controls.DiplomacyStack:CalculateSize();
	Controls.DiplomacyStack:ReprocessAnchoring();
	Controls.UnderProtectionOfStack:CalculateSize();
	Controls.UnderProtectionOfStack:ReprocessAnchoring();
	Controls.AlliedCityStatesStack:CalculateSize();
	Controls.AlliedCityStatesStack:ReprocessAnchoring();
-- CBP
	Controls.DefensePactStack:CalculateSize();
	Controls.DefensePactStack:ReprocessAnchoring();
--END
	Controls.ActiveDealsStack:CalculateSize();
	Controls.ActiveDealsStack:ReprocessAnchoring();
	Controls.TradeRoutesStack:CalculateSize();
	Controls.TradeRoutesStack:ReprocessAnchoring();
	Controls.DeclareWarDetailsStack:CalculateSize();
	Controls.DeclareWarDetailsStack:ReprocessAnchoring();
	Controls.DeclareWarDetailsScrollPanel:CalculateInternalSize();
end


local diplomacyText = Locale.Lookup("TXT_KEY_DIPLO_DIPLOMACY_BUTTON");
RegisterCollapseBehavior{	
	Header = Controls.DiplomacyHeader, 
	HeaderLabel = Controls.DiplomacyHeader, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. diplomacyText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. diplomacyText,
	Panel = Controls.DiplomacyStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

local underProtectionOfText = Locale.Lookup("TXT_KEY_DECLARE_WAR_UNDER_PROTECTION_OF");
RegisterCollapseBehavior{	
	Header = Controls.UnderProtectionOfHeader, 
	HeaderLabel = Controls.UnderProtectionOfHeader, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. underProtectionOfText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. underProtectionOfText,
	Panel = Controls.UnderProtectionOfStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

-- Putmalk
local mastersText = Locale.Lookup("TXT_KEY_DECLARE_WAR_MASTERS");
RegisterCollapseBehavior{	
	Header = Controls.MastersHeader, 
	HeaderLabel = Controls.MastersHeader, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. mastersText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. mastersText,
	Panel = Controls.MastersStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

local vassalsText = Locale.Lookup("TXT_KEY_DECLARE_WAR_VASSALS");
RegisterCollapseBehavior{	
	Header = Controls.VassalsHeader, 
	HeaderLabel = Controls.VassalsHeader, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. vassalsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. vassalsText,
	Panel = Controls.VassalsStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

local alliedCityStatesText = Locale.Lookup("TXT_KEY_DECLARE_WAR_ALLIED_CITYSTATES");
RegisterCollapseBehavior{	
	Header = Controls.AlliedCityStatesHeader, 
	HeaderLabel = Controls.AlliedCityStatesHeader, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. alliedCityStatesText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. alliedCityStatesText,
	Panel = Controls.AlliedCityStatesStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};
--CBP
local dptext = Locale.Lookup("TXT_KEY_DECLARE_WAR_DPS_CP");
RegisterCollapseBehavior{	
	Header = Controls.DefensePactHeader, 
	HeaderLabel = Controls.DefensePactHeader, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. dptext,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. dptext,
	Panel = Controls.DefensePactStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};
--END			
local activeDealsText = Locale.Lookup("TXT_KEY_DECLARE_WAR_ACTIVE_DEALS");
RegisterCollapseBehavior{
	Header = Controls.ActiveDealsHeader,
	HeaderLabel = Controls.ActiveDealsHeader,
	HeaderExpandedLabel = "[ICON_MINUS] " .. activeDealsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. activeDealsText,
	Panel = Controls.ActiveDealsStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

local tradeRoutesText = Locale.Lookup("TXT_KEY_DECLARE_WAR_TRADE_ROUTES");
RegisterCollapseBehavior{
	Header = Controls.TradeRoutesHeader,
	HeaderLabel = Controls.TradeRoutesHeader,
	HeaderExpandedLabel = "[ICON_MINUS] " .. tradeRoutesText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. tradeRoutesText,
	Panel = Controls.TradeRoutesStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};


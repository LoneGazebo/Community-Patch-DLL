-------------------------------------------------
-- Diplomatic
-------------------------------------------------
include("IconSupport");

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler(bIsHide, bInitState)
	if (not bIsHide) then
		InitMajorCivList();
	end
end
ContextPtr:SetShowHideHandler(ShowHideHandler);


-------------------------------------------------
-------------------------------------------------
function InitMajorCivList()

	local g_iUs = Game.GetActivePlayer();
	local g_pUs = Players[ g_iUs ];
	local g_iUsTeam = g_pUs:GetTeam();
	local g_pUsTeam = Teams[ g_iUsTeam ];
	
	-- Clear buttons
	Controls.ItemStack:DestroyAllChildren();
	
	local count = 0;
	--------------------------------------------------------------------
	-- Loop through all the Majors the active player knows
	--------------------------------------------------------------------
	for iOtherPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
		
		local pOtherPlayer = Players[ iOtherPlayer ];
		local iOtherTeam = pOtherPlayer:GetTeam();
		local pOtherTeam = Teams[ iOtherTeam ];
		
		-- Valid player? - Can't be us, and has to be alive
		if (iOtherPlayer ~= g_iUs and pOtherPlayer:IsAlive()) then

			-- Met this player?
			if (g_pUsTeam:IsHasMet(iOtherTeam)) then
			
				count = count + 1;
				
				local controlTable = {};
				ContextPtr:BuildInstanceForControl("MajorInstance", controlTable, Controls.ItemStack);
				
				local primaryColor, secondaryColor = pOtherPlayer:GetPlayerColors();
				local civType = pOtherPlayer:GetCivilizationType();
				local civInfo = GameInfo.Civilizations[ civType ];
				
				local otherLeaderType = pOtherPlayer:GetLeaderType();
				local otherLeaderInfo = GameInfo.Leaders[otherLeaderType];
		
				IconHookup(otherLeaderInfo.PortraitIndex, 128, otherLeaderInfo.IconAtlas, controlTable.LeaderPortrait);
				CivIconHookup(iOtherPlayer, 64, controlTable.CivSymbol, controlTable.CivIconBG, controlTable.CivIconShadow, false, true);

				-- Player name
				local otherName;
				if (pOtherPlayer:IsHuman()) then
					otherName = pOtherPlayer:GetNickName();
				else
					otherName = pOtherPlayer:GetName();
				end
				controlTable.LeaderName:SetText(otherName);
				
				-- Civ name
				controlTable.CivName:LocalizeAndSetText(civInfo.ShortDescription);
				
				-- Civ Era
				local iEra = pOtherPlayer:GetCurrentEra();
				controlTable.EraLabel:SetText(Locale.ConvertTextKey(GameInfo.Eras[iEra].Description));
				
				-- What happens if we click this?
				controlTable.Button:SetVoid1(iOtherPlayer);
				controlTable.Button:RegisterCallback(Mouse.eLClick, LeaderSelected);
				
				-- Policies
				for pPolicyBranch in GameInfo.PolicyBranchTypes() do
					local iPolicyBranch = pPolicyBranch.ID;
					
					local iCount = 0;
					
					for pPolicy in GameInfo.Policies() do
						local iPolicy = pPolicy.ID;
						
						if (pPolicy.PolicyBranchType == pPolicyBranch.Type) then
							if (pOtherPlayer:HasPolicy(iPolicy)) then
								iCount = iCount + 1;
							end
						end
					end
					
				 	if (iCount > 0) then
						local textControls = {};
						ContextPtr:BuildInstanceForControl("LTextEntry", textControls, controlTable.PoliciesStack);
						local strText = Locale.ConvertTextKey(pPolicyBranch.Description) .. ": " .. iCount;
						textControls.Text:SetText(strText);
					end
				end
				
				-- Wonders
				for pBuilding in GameInfo.Buildings() do
					local iBuilding = pBuilding.ID;
					
					local pBuildingClass = GameInfo.BuildingClasses[pBuilding.BuildingClass];
					if (pBuildingClass.MaxGlobalInstances > 0) then
						if (pOtherPlayer:CountNumBuildings(iBuilding) > 0) then
							local textControls = {};
							ContextPtr:BuildInstanceForControl("TextEntryShort", textControls, controlTable.WondersStack);
							textControls.Text:LocalizeAndSetText(pBuilding.Description);
						end
					end
				end
								
				local bHasEntry = false;
				
				-- War
				if (g_pUsTeam:IsAtWar(iOtherTeam)) then
					controlTable.WarLabel:SetHide(false);
					controlTable.WarLabel:SetText("[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("{TXT_KEY_DO_AT_WAR:upper}") .. "[ENDCOLOR]");
					bHasEntry = true;
				else
					controlTable.WarLabel:SetHide(true);
				end
				
				-- We Denounced Them
				if (Players[g_iUs]:IsDenouncedPlayer(iOtherPlayer)) then
					controlTable.DenounceLabel:SetHide(false);
					
					-- Backstab?
					if (pOtherPlayer:IsFriendDenouncedUs(g_iUs) or pOtherPlayer:IsFriendDeclaredWarOnUs(g_iUs)) then
						controlTable.DenounceLabel:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_YOU_HAVE_BACKSTABBED"));
					-- Normal denouncement
					else
						local text = Locale.Lookup("TXT_KEY_DIPLO_YOU_HAVE_DENOUNCED")
						if(Players[g_iUs].GetDenouncedPlayerCounter ~= nil) then
							local turnsLeft = GameDefines.DENUNCIATION_EXPIRATION_TIME - Players[g_iUs]:GetDenouncedPlayerCounter(iOtherPlayer);
							text = text .. " (" .. Locale.Lookup("TXT_KEY_DECLARE_WAR_DEALS_TURNS_LEFT", turnsLeft) .. ")";
						end
					
						controlTable.DenounceLabel:SetText(text);
					end
					bHasEntry = true;
				else
					controlTable.DenounceLabel:SetHide(true);
				end
				
				-- We're his vassal
				if (g_pUsTeam:IsVassal(iOtherTeam)) then
					controlTable.VassalLabel:SetHide(false);
					controlTable.VassalLabel:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_YOU_ARE_VASSAL"));
					bHasEntry = true;
				else
					controlTable.VassalLabel:SetHide(true);
				end
				
				-- The following goes into the diplo stack on the right
				
				-- War with 3rd party
				for iThirdPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];
						
						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam  = pThirdPlayer:GetTeam();
							
							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- War (NOT us - handled above)
								if (pOtherTeam:IsAtWar(iThirdTeam) and iThirdPlayer ~= g_iUs) then
									bHasEntry = true;
									
									-- Human
									if (pThirdPlayer:IsHuman()) then
										thirdName = pThirdPlayer:GetNickName();
									-- AI
									else
										thirdName = pThirdPlayer:GetCivilizationShortDescription();
									end
									
									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									local iWarScore = pOtherPlayer:GetWarScore(iThirdPlayer);
									textControls.Text:LocalizeAndSetText("TXT_KEY_AT_WAR_WITH", thirdName, iWarScore);
								end
							end
						end
					end
				end
				
				-- Vassals
				for iThirdPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];

						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam = pThirdPlayer:GetTeam();

							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- Is a Vassal
								if (pOtherTeam:IsVassal(iThirdTeam)) then
									bHasEntry = true;

									-- Us
									if (iThirdPlayer == g_iUs) then
										thirdName = "TXT_KEY_YOU";
									-- Human
									elseif (pThirdPlayer:IsHuman()) then
										thirdName = pThirdPlayer:GetNickName();
									-- AI
									else
										thirdName = pThirdPlayer:GetCivilizationShortDescription();
									end

									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									textControls.Text:LocalizeAndSetText("TXT_KEY_DIPLO_VASSAL_OF", thirdName);
								end
							end
						end
					end
				end
				
				-- DoFs
				for iThirdPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];
						
						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam = pThirdPlayer:GetTeam();
							
							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- Has a DoF
								if (pOtherPlayer:IsDoF(iThirdPlayer)) then
									bHasEntry = true;
									
									-- Us
									if (iThirdPlayer == g_iUs) then
										thirdName = "TXT_KEY_YOU";
									-- Human
									elseif (pThirdPlayer:IsHuman()) then
										thirdName = pThirdPlayer:GetNickName();
									-- AI
									else
										thirdName = pThirdPlayer:GetCivilizationShortDescription();
									end
									
									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									
									local text = Locale.Lookup("TXT_KEY_DIPLO_FRIENDS_WITH", thirdName);
									if(pOtherPlayer.GetDoFCounter ~= nil) then
										local turnsLeft = GameDefines.DOF_EXPIRATION_TIME - pOtherPlayer:GetDoFCounter(iThirdPlayer);
										text = text .. " (" .. Locale.Lookup("TXT_KEY_DECLARE_WAR_DEALS_TURNS_LEFT", turnsLeft) .. ")";
        							end
									textControls.Text:SetText(text);
								end
							end
						end
					end
				end

				-- DPS (CBP)
				for iThirdPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];
						
						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam = pThirdPlayer:GetTeam();
							
							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- Has a DP
								if (pOtherPlayer:IsHasDefensivePactWithPlayer(iThirdPlayer)) then
									bHasEntry = true;
									
									-- Us
									if (iThirdPlayer == g_iUs) then
										thirdName = "TXT_KEY_YOU";
									-- Human
									elseif (pThirdPlayer:IsHuman()) then
										thirdName = pThirdPlayer:GetNickName();
									-- AI
									else
										thirdName = pThirdPlayer:GetCivilizationShortDescription();
									end
									
									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									
									--local text = Locale.Lookup("TXT_KEY_DIPLO_DP_WITH_CBP", thirdName);
									textControls.Text:LocalizeAndSetText("TXT_KEY_DIPLO_DP_WITH_CBP", thirdName);
								end
							end
						end
					end
				end
				-- END
				-- Marriages (CBP)
				for iThirdPlayer = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];
						
						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam  = pThirdPlayer:GetTeam();
							
							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- Allied with Minor
								if (pThirdPlayer:IsMarried(iOtherPlayer)) then
									bHasEntry = true;
									
									thirdName = pThirdPlayer:GetCivilizationShortDescription();
									
									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									textControls.Text:LocalizeAndSetText("TXT_KEY_MARRIED_TO", thirdName);
								end
							end
						end
					end
				end
				-- END
				-- Denouncements
				for iThirdPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];
						
						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam  = pThirdPlayer:GetTeam();
							
							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- Denounced
								if (pOtherPlayer:IsDenouncedPlayer(iThirdPlayer) or pThirdPlayer:IsFriendDeclaredWarOnUs(iOtherPlayer)) then
									bHasEntry = true;
									
									-- Us
									if (iThirdPlayer == g_iUs) then
										thirdName = "TXT_KEY_YOU";
									-- Human
									elseif (pThirdPlayer:IsHuman()) then
										thirdName = pThirdPlayer:GetNickName();
									-- AI
									else
										thirdName = pThirdPlayer:GetCivilizationShortDescription();
									end
									
									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									
									-- Backstab?
									if (pThirdPlayer:IsFriendDenouncedUs(iOtherPlayer) or pThirdPlayer:IsFriendDeclaredWarOnUs(iOtherPlayer)) then
										textControls.Text:LocalizeAndSetText("TXT_KEY_DIPLO_BACKSTABBED", thirdName);
									-- Normal denouncement
									else
										local text = Locale.Lookup("TXT_KEY_DIPLO_DENOUNCED", thirdName);
										if(pOtherPlayer.GetDenouncedPlayerCounter ~= nil) then
											local turnsLeft = GameDefines.DENUNCIATION_EXPIRATION_TIME - pOtherPlayer:GetDenouncedPlayerCounter(iThirdPlayer);
											text = text .. " (" .. Locale.Lookup("TXT_KEY_DECLARE_WAR_DEALS_TURNS_LEFT", turnsLeft) .. ")";
        								end
										textControls.Text:SetText(text);
										
									end
								end
							end
						end
					end
				end
				
				-- Allied to Minors
				for iThirdPlayer = GameDefines.MAX_MAJOR_CIVS, GameDefines.MAX_CIV_PLAYERS - 1 do
					if (iThirdPlayer ~= iOtherPlayer) then
						local pThirdPlayer = Players[iThirdPlayer];
						
						if (pThirdPlayer ~= nil and pThirdPlayer:IsAlive()) then
							local iThirdTeam  = pThirdPlayer:GetTeam();
							
							if (g_pUsTeam:IsHasMet(iThirdTeam) or iThirdPlayer == g_iUs) then
								
								-- Allied with Minor
								if (pThirdPlayer:IsAllies(iOtherPlayer)) then
									bHasEntry = true;
									
									thirdName = pThirdPlayer:GetCivilizationShortDescription();
									
									local textControls = {};
									ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
									textControls.Text:LocalizeAndSetText("TXT_KEY_ALLIED_WITH", thirdName);
								end
							end
						end
					end
				end
				
				-- Promises (Vox Populi)
				local function ShowPromiseTurns(iNumTurns, sDiploText)
					if iNumTurns <= 0 then return end -- do not display "0 turns"
					local textControls = {};
					ContextPtr:BuildInstanceForControl("TextEntryLong", textControls, controlTable.PactStack);
					textControls.Text:LocalizeAndSetText(sDiploText, iNumTurns);
				end
				ShowPromiseTurns(pOtherPlayer:GetNumTurnsMilitaryPromise(g_iUs),  "TXT_KEY_DIPLO_MILITARY_PROMISE_TURNS");
				ShowPromiseTurns(pOtherPlayer:GetNumTurnsExpansionPromise(g_iUs), "TXT_KEY_DIPLO_EXPANSION_PROMISE_TURNS");
				ShowPromiseTurns(pOtherPlayer:GetNumTurnsBorderPromise(g_iUs),    "TXT_KEY_DIPLO_BORDER_PROMISE_TURNS");
				-- Promises END
				
				--controlTable.NothingLabel:SetHide(bHasEntry);
				controlTable.PactStack:CalculateSize();
				controlTable.PoliciesStack:CalculateSize();
				controlTable.WondersStack:CalculateSize();
				controlTable.WarStack:CalculateSize();
				controlTable.MinorStack:CalculateSize();
				controlTable.OuterStack:CalculateSize();
				
				print("controlTable.OuterStack:GetSizeY()" .. controlTable.OuterStack:GetSizeY());
				print("controlTable.PoliciesStack:GetSizeY()" .. controlTable.PoliciesStack:GetSizeY());
				print("controlTable.WondersStack:GetSizeY()" .. controlTable.WondersStack:GetSizeY());
				
				local sizeY = math.max(controlTable.OuterStack:GetSizeY(), controlTable.PoliciesStack:GetSizeY());
				local sizeY = math.max(sizeY, controlTable.WondersStack:GetSizeY());
				if (sizeY < 120) then 
					sizeY = 120;
				end
				
				local newSizeY = sizeY + controlTable.OuterStack:GetOffsetY()
				
				controlTable.Button:SetSizeY(newSizeY);
				controlTable.ButtonAnim:SetSizeY(newSizeY);
				controlTable.ButtonAnimGrid:SetSizeY(newSizeY+5);
				controlTable.OuterStack:ReprocessAnchoring();
				
				controlTable.PlayerInfoStack:CalculateSize();
				controlTable.PlayerInfoStack:ReprocessAnchoring();
			end
		end
	end

	if (count > 0) then
		Controls.NoMajorCivs:SetHide(true);
	else
		Controls.NoMajorCivs:SetHide(false);
	end
	
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.MajorCivScrollPanel:CalculateInternalSize();
end



-------------------------------------------------
-- On Leader Selected
-------------------------------------------------
function LeaderSelected(ePlayer)

	if (Players[ ePlayer ]:IsHuman()) then
		Events.OpenPlayerDealScreenEvent(ePlayer);
	else
		
		UI.SetRepeatActionPlayer(ePlayer);
		UI.ChangeStartDiploRepeatCount(1);
		Players[ ePlayer ]:DoBeginDiploWithHuman();	

	end
end

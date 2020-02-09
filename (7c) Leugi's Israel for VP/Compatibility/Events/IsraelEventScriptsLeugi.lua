-- Leugi Israel Events
-- Author: Leugi
--=======================================================================================================================
print("Leugi's Israel Events: loaded")
--=======================================================================================================================
-- Civ Specific Events
--=======================================================================================================================
--  Bathsheba
iProphetID = GameInfo.Units.UNIT_PROPHET.ID
BribeCostNathan = 500
MinFaithNathan = 100
NathanUnhappinessCost = 4
--------------------------------------------------------------------------------------------------------------------------
local Event_Leugi_Israel_Bathseba = {}
	Event_Leugi_Israel_Bathseba.Name = "TXT_KEY_EVENT_ISRAEL_BATHSEBA"
	Event_Leugi_Israel_Bathseba.Desc = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_DESC"
	Event_Leugi_Israel_Bathseba.Weight = 20
	Event_Leugi_Israel_Bathseba.CanFunc = (
		function(pPlayer)
			-- Check if the event fired before			
			if load(pPlayer, "Event_Leugi_Israel_Bathseba") == true then return false end
			--
			-- Check the Civ
			if pPlayer:GetCivilizationType() ~= GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"] then return false end
			--
			-- Check minimum faith
			local iMinFaith = math.ceil((MinFaithNathan) * iMod)
			if (pPlayer:GetFaith() < iMinFaith) then return false end
			--
			-- Fire the Event! (if chances are met)
			return true
		end
		)
	Event_Leugi_Israel_Bathseba.Outcomes = {}
	--=========================================================
	-- Outcome 1
	--=========================================================
	Event_Leugi_Israel_Bathseba.Outcomes[1] = {}
	Event_Leugi_Israel_Bathseba.Outcomes[1].Name = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_1"
	Event_Leugi_Israel_Bathseba.Outcomes[1].Desc = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_1"
	Event_Leugi_Israel_Bathseba.Outcomes[1].Weight = 10
	Event_Leugi_Israel_Bathseba.Outcomes[1].CanFunc = (
		function(pPlayer)			
			-- Create Description			
			Event_Leugi_Israel_Bathseba.Outcomes[1].Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_1")
			return true
		end
		)
	Event_Leugi_Israel_Bathseba.Outcomes[1].DoFunc = (
		function(pPlayer) 
			-- Remove all Faith
			pPlayer:SetFaith(0);
			-- Remove Golden Age
			pPlayer:SetGoldenAgeProgressMeter(0)
			local GoldenAgeTurns = pPlayer:GetGoldenAgeTurns()
			pPlayer:ChangeGoldenAgeTurns(-GoldenAgeTurns)
			-- Grant the Great Prophet
			InitUnitFromCity(pPlayer:GetCapitalCity(), GameInfoTypes.UNIT_PROPHET, 1)
			-- Save the event
			save(pPlayer, "Event_Leugi_Israel_Bathseba", true)
			-- Send the notification
			JFD_SendNotification(pPlayer:GetID(), "NOTIFICATION_GENERIC", Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_1_NOTIFICATION"), Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA"))
			Event_Leugi_Israel_Bathseba.Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_DESC")
			for _, nPlayer in pairs(Players) do
				if (nPlayer:IsAlive()) then
					local pteamID = pPlayer:GetTeam();
					local pTeam = Teams[pteamID];
					local notifyteamID = nPlayer:GetTeam();
					local nTeam = Teams[notifyteamID];
					if (nTeam:IsHasMet(pTeam)) then
						if nPlayer:IsHuman() then
							Events.GameplayAlertMessage(Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OTHERS_ALERT"))
						end
					end
				end
			end
		end)
	--=========================================================
	-- Outcome 2
	--=========================================================
	Event_Leugi_Israel_Bathseba.Outcomes[2] = {}
	Event_Leugi_Israel_Bathseba.Outcomes[2].Name = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_2"
	Event_Leugi_Israel_Bathseba.Outcomes[2].Desc = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_2"
	Event_Leugi_Israel_Bathseba.Outcomes[2].Weight = 0
	Event_Leugi_Israel_Bathseba.Outcomes[2].CanFunc = (
		function(pPlayer)			
			-- Create Description			
			Event_Leugi_Israel_Bathseba.Outcomes[2].Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_2", NathanUnhappinessCost)
			return true
		end
		)
	Event_Leugi_Israel_Bathseba.Outcomes[2].DoFunc = (
		function(pPlayer) 
			-- Generate Unhappiness
			iActualUnhappiness = NathanUnhappinessCost * 100
			pPlayer:ChangeUnhappinessFromUnits(iActualUnhappiness)
			-- Save the event
			save(pPlayer, "Event_Leugi_Israel_Bathseba", true)
			-- Send the notification
			JFD_SendNotification(pPlayer:GetID(), "NOTIFICATION_GENERIC", Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_2_NOTIFICATION"), Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA"))
			Event_Leugi_Israel_Bathseba.Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_DESC")
			for _, nPlayer in pairs(Players) do
				if (nPlayer:IsAlive()) then
					local pteamID = pPlayer:GetTeam();
					local pTeam = Teams[pteamID];
					local notifyteamID = nPlayer:GetTeam();
					local nTeam = Teams[notifyteamID];
					if (nTeam:IsHasMet(pTeam)) then
						if nPlayer:IsHuman() then
							Events.GameplayAlertMessage(Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OTHERS_ALERT"))
						end
					end
				end
			end
		end)
		
	--=========================================================
	-- Outcome 3
	--=========================================================
	Event_Leugi_Israel_Bathseba.Outcomes[3] = {}
	Event_Leugi_Israel_Bathseba.Outcomes[3].Name = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_3"
	Event_Leugi_Israel_Bathseba.Outcomes[3].Desc = "TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_3"
	Event_Leugi_Israel_Bathseba.Outcomes[3].Weight = 5
	Event_Leugi_Israel_Bathseba.Outcomes[3].CanFunc = (
		function(pPlayer)	
			-- Get Gold Cost
			local iGoldCost = math.ceil((BribeCostNathan) * iMod)		
			-- Create Description			
			Event_Leugi_Israel_Bathseba.Outcomes[3].Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_3", iGoldCost)
			if pPlayer:GetGold() < iGoldCost then return false end
			return true
		end
		)
	Event_Leugi_Israel_Bathseba.Outcomes[3].DoFunc = (
		function(pPlayer) 
			-- Get Gold Cost
			local iGoldCost = math.ceil((BribeCostNathan) * iMod)	
			pPlayer:ChangeGold(-iGoldCost);
			-- Roll the dice!
			rNum = (Game.Rand(3, "Randomness!"))
			if rNum == 0 then
				-- Grant the Great Prophet
				InitUnitFromCity(pPlayer:GetCapitalCity(), GameInfoTypes.UNIT_PROPHET, 1)
			else
				-- Remove all Faith
				pPlayer:SetFaith(0);
			end
			-- Save the event
			save(pPlayer, "Event_Leugi_Israel_Bathseba", true)
			-- Send the notification
			JFD_SendNotification(pPlayer:GetID(), "NOTIFICATION_GENERIC", Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OUTCOME_RESULT_3_NOTIFICATION"), Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA"))
			Event_Leugi_Israel_Bathseba.Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_DESC")
			for _, nPlayer in pairs(Players) do
				if (nPlayer:IsAlive()) then
					local pteamID = pPlayer:GetTeam();
					local pTeam = Teams[pteamID];
					local notifyteamID = nPlayer:GetTeam();
					local nTeam = Teams[notifyteamID];
					if (nTeam:IsHasMet(pTeam)) then
						if nPlayer:IsHuman() then
							Events.GameplayAlertMessage(Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_BATHSEBA_OTHERS_ALERT"))
						end
					end
				end
			end
		end)

	

Events_AddCivilisationSpecific(GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"], "Event_Leugi_Israel_Bathseba", Event_Leugi_Israel_Bathseba)


--=======================================================================================================================
--=======================================================================================================================
--  Battle of Jericho
iProphetID = GameInfo.Units.UNIT_PROPHET.ID
iSiegePromo = GameInfoTypes.PROMOTION_SAPPER;
--------------------------------------------------------------------------------------------------------------------------
local Event_Leugi_Israel_Jericho = {}
	Event_Leugi_Israel_Jericho.Name = "TXT_KEY_EVENT_ISRAEL_JERICHO"
	Event_Leugi_Israel_Jericho.Desc = "TXT_KEY_EVENT_ISRAEL_JERICHO_DESC"
	Event_Leugi_Israel_Jericho.Weight = 20
	Event_Leugi_Israel_Jericho.CanFunc = (
		function(pPlayer)
			-- Check if the event fired before			
			if load(pPlayer, "Event_Leugi_Israel_Jericho") == true then return false end
			--
			-- Check the Civ
			if pPlayer:GetCivilizationType() ~= GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"] then return false end
			--
			-- Check Era
			if (pPlayer:GetCurrentEra() > GameInfo.Eras["ERA_MEDIEVAL"].ID) then return false end
			--
			--Check War
			local pTeam = pPlayer:GetTeam();
			local isWar = 0;
			for ioPlayer = 0, GameDefines.MAX_MAJOR_CIVS-1 do
				local oPlayer = Players[ioPlayer];
				if oPlayer ~= pPlayer then
					if (oPlayer:IsAlive()) then
						local oTeam = oPlayer:GetTeam();
						if Teams[pTeam]:IsAtWar(oTeam) then
							isWar = isWar + 1;
							break
						end
					end
				end
			end
			if isWar <= 0 then return false end
			--
			-- Check number of Great Prophets
			nProphets = 0
			for pUnit in pPlayer:Units() do
				if (pUnit:GetUnitType() == iProphetID) then
					nProphets = nProphets + 1
				end
			end
			if nProphets == 0 then return false end
			--
			-- Fire the Event! (if chances are met)
			Event_Leugi_Israel_Jericho.Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_JERICHO_DESC")
			return true
		end
		)
	Event_Leugi_Israel_Jericho.Outcomes = {}
	--=========================================================
	-- Outcome 1
	--=========================================================
	Event_Leugi_Israel_Jericho.Outcomes[1] = {}
	Event_Leugi_Israel_Jericho.Outcomes[1].Name = "TXT_KEY_EVENT_ISRAEL_JERICHO_OUTCOME_1"
	Event_Leugi_Israel_Jericho.Outcomes[1].Desc = "TXT_KEY_EVENT_ISRAEL_JERICHO_OUTCOME_RESULT_1"
	Event_Leugi_Israel_Jericho.Outcomes[1].Weight = 10
	Event_Leugi_Israel_Jericho.Outcomes[1].CanFunc = (
		function(pPlayer)			
			-- Create Description			
			Event_Leugi_Israel_Jericho.Outcomes[1].Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_JERICHO_OUTCOME_RESULT_1")
			return true
		end
		)
	Event_Leugi_Israel_Jericho.Outcomes[1].DoFunc = (
		function(pPlayer) 
			-- Grant the promotion
			for pUnit in pPlayer:Units() do
				if (pUnit:GetUnitType() == iProphetID) then
					pUnit:SetHasPromotion(iSiegePromo,true);
				end
			end
			-- Save the event
			save(pPlayer, "Event_Leugi_Israel_Jericho", true)
			-- Send the notification
			JFD_SendNotification(pPlayer:GetID(), "NOTIFICATION_GENERIC", Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_JERICHO_OUTCOME_RESULT_1_NOTIFICATION"), Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_JERICHO"))
			for _, nPlayer in pairs(Players) do
				if (nPlayer:IsAlive()) then
					local pteamID = pPlayer:GetTeam();
					local pTeam = Teams[pteamID];
					local notifyteamID = nPlayer:GetTeam();
					local nTeam = Teams[notifyteamID];
					if (nTeam:IsHasMet(pTeam)) then
						if nPlayer:IsHuman() then
							Events.GameplayAlertMessage(Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_JERICHO_OTHERS_ALERT"))
						end
					end
				end
			end
		end)

Events_AddCivilisationSpecific(GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"], "Event_Leugi_Israel_Jericho", Event_Leugi_Israel_Jericho)


--=======================================================================================================================
--=======================================================================================================================
--  Heir Problems
iHeirMagistrates = 4
iHeirFaith = 500
--------------------------------------------------------------------------------------------------------------------------
local Event_Leugi_Israel_Heir = {}
	Event_Leugi_Israel_Heir.Name = "TXT_KEY_EVENT_ISRAEL_HEIR"
	Event_Leugi_Israel_Heir.Desc = "TXT_KEY_EVENT_ISRAEL_HEIR_DESC"
	Event_Leugi_Israel_Heir.Weight = 15
	Event_Leugi_Israel_Heir.CanFunc = (
		function(pPlayer)
			-- Check if the event fired before			
			if load(pPlayer, "Event_Leugi_Israel_Heir") == true then return false end
			--
			-- Check the Civ
			if pPlayer:GetCivilizationType() ~= GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"] then return false end
			--
			-- Check Era
			if (pPlayer:GetCurrentEra() > GameInfo.Eras["ERA_CLASSICAL"].ID) then return false end
			--
			-- Check Number of Cities
			nCities = 0
			for pCity in pPlayer:Cities() do
				nCities = nCities + 1
			end
			if nCities == 0 then return false end
			--
			-- Fire the Event! (if chances are met)
			return true
		end
		)
	Event_Leugi_Israel_Heir.Outcomes = {}
	--=========================================================
	-- Outcome 1
	--=========================================================
	Event_Leugi_Israel_Heir.Outcomes[1] = {}
	Event_Leugi_Israel_Heir.Outcomes[1].Name = "TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_1"
	Event_Leugi_Israel_Heir.Outcomes[1].Desc = "TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_RESULT_1"
	Event_Leugi_Israel_Heir.Outcomes[1].Weight = 1
	Event_Leugi_Israel_Heir.Outcomes[1].CanFunc = (
		function(pPlayer)			
			-- Create Description			
			Event_Leugi_Israel_Heir.Outcomes[1].Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_RESULT_1", iHeirMagistrates)
			return true
		end
		)
	Event_Leugi_Israel_Heir.Outcomes[1].DoFunc = (
		function(pPlayer) 
			-- Grant Magistrates
			pPlayer:ChangeNumResourceTotal(iMagistrate, iHeirMagistrates)
			-- Save the event
			save(pPlayer, "Event_Leugi_Israel_Heir", true)
			-- Send the notification
			JFD_SendNotification(pPlayer:GetID(), "NOTIFICATION_GENERIC", Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_RESULT_1_NOTIFICATION"), Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR"))
			Event_Leugi_Israel_Heir.Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_DESC")
			for _, nPlayer in pairs(Players) do
				if (nPlayer:IsAlive()) then
					local pteamID = pPlayer:GetTeam();
					local pTeam = Teams[pteamID];
					local notifyteamID = nPlayer:GetTeam();
					local nTeam = Teams[notifyteamID];
					if (nTeam:IsHasMet(pTeam)) then
						if nPlayer:IsHuman() then
							Events.GameplayAlertMessage(Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_OTHERS_ALERT"))
						end
					end
				end
			end
		end)
	--=========================================================
	-- Outcome 2
	--=========================================================
	Event_Leugi_Israel_Heir.Outcomes[2] = {}
	Event_Leugi_Israel_Heir.Outcomes[2].Name = "TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_2"
	Event_Leugi_Israel_Heir.Outcomes[2].Desc = "TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_RESULT_2"
	Event_Leugi_Israel_Heir.Outcomes[2].Weight = 10
	Event_Leugi_Israel_Heir.Outcomes[2].CanFunc = (
		function(pPlayer)			
			-- Create Description			
			local iFaith = math.ceil((iHeirFaith) * iMod)
			nCapital = "No City"
			CityName = "Your second city"
			for pCity in pPlayer:Cities() do
				if pCity:IsCapital() then
					nCapital = Locale.ConvertTextKey(pCity:GetName());
				else
					TurnFounded = pCity:GetGameTurnFounded()
					if (OldTurnFounded == nil) or (TurnFounded < OldTurnFounded) then
						OldTurnFounded = TurnFounded
						OldestCity = pCity
					end
				end
			end
			if OldestCity ~= nil then
				CityName = Locale.ConvertTextKey(OldestCity:GetName());
			end 
			Event_Leugi_Israel_Heir.Outcomes[2].Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_RESULT_2", CityName, iFaith)
			return true
		end
		)
	Event_Leugi_Israel_Heir.Outcomes[2].DoFunc = (
		function(pPlayer) 
			-- Start the rebellion!
			for pCity in pPlayer:Cities() do
				if pCity:IsCapital() then
					nCapital = Locale.ConvertTextKey(pCity:GetName());
				else
					TurnFounded = pCity:GetGameTurnFounded()
					if (OldTurnFounded == nil) or (TurnFounded < OldTurnFounded) then
						OldTurnFounded = TurnFounded
						OldestCity = pCity
					end
				end
			end
			OldestCity:ChangeDamage(70)
			Players[63]:AcquireCity(OldestCity, true, false)
			-- Grant Goodies
			pPlayer:ChangeGoldenAgeTurns(30)
			local iFaith = math.ceil((iHeirFaith) * iMod)
			pPlayer:ChangeFaith(iFaith);
			-- Save the event
			save(pPlayer, "Event_Leugi_Israel_Heir", true)
			-- Send the notification
			JFD_SendNotification(pPlayer:GetID(), "NOTIFICATION_GENERIC", Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_OUTCOME_RESULT_2_NOTIFICATION"), Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR"))
			Event_Leugi_Israel_Heir.Desc = Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_DESC")
			for _, nPlayer in pairs(Players) do
				if (nPlayer:IsAlive()) then
					local pteamID = pPlayer:GetTeam();
					local pTeam = Teams[pteamID];
					local notifyteamID = nPlayer:GetTeam();
					local nTeam = Teams[notifyteamID];
					if (nTeam:IsHasMet(pTeam)) then
						if nPlayer:IsHuman() then
							Events.GameplayAlertMessage(Locale.ConvertTextKey("TXT_KEY_EVENT_ISRAEL_HEIR_OTHERS_ALERT"))
						end
					end
				end
			end
		end)

Events_AddCivilisationSpecific(GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"], "Event_Leugi_Israel_Heir", Event_Leugi_Israel_Heir)


--=======================================================================================================================
--=======================================================================================================================


print("Leugi's Israel Decisions")

--Psalms
--------------------------------------
-- Psalms Vars
iPsalmTechPrereq = GameInfoTypes.TECH_WRITING;
iPsalmCultureMaxBaseCost = 300
iPsalmCultureReducedCost = 2
iPsalmCultureCostperEra = 120
iPsalmCultureReducedEraCost = 1
iPsalmFaithPerCitizen = 5
---------------------------------------

local Decisions_Israel_Psalms = {}
	Decisions_Israel_Psalms.Name = "TXT_KEY_DECISIONS_LEUGI_ISRAEL_PSALMS"
	Decisions_Israel_Psalms.Desc = "TXT_KEY_DECISIONS_LEUGI_ISRAEL_PSALMS_DESC"
	HookDecisionCivilizationIcon(Decisions_Israel_Psalms, "CIVILIZATION_LEUGI_ISRAEL")
	Decisions_Israel_Psalms.Weight = nil
	Decisions_Israel_Psalms.CanFunc = (
		function(pPlayer)	
			-- Check Civ
			if (pPlayer:GetCivilizationType() ~= GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"]) then return false, false end
			-- Check if the player has enacted in this era
			local iEra = load(pPlayer, "Decisions_Israel_Psalms")
			local iCurrentEra = pPlayer:GetCurrentEra()
			if iEra ~= nil then
				if iEra < iCurrentEra then
					save(pPlayer, "Decisions_Israel_Psalms", nil)
				else
					if pPlayer:IsHuman() then	
						Decisions_Israel_Psalms.Desc = Locale.ConvertTextKey("TXT_KEY_DECISIONS_LEUGI_ISRAEL_PSALMS_ENACTED_DESC")
					end
					return false, false, true
				end
			end
			PsalmsEnactable = true
			-- Religious Unity
			---- Get Religion
			iUnity = 0
			local iReligion = pPlayer:GetReligionCreatedByPlayer();
			if not (pPlayer:HasCreatedReligion()) then
				PsalmsEnactable = false
			else
				iUnity = GetReligiousUnity(pPlayer, iReligion)
				iUnity = math.floor(iUnity * 100)

				-- Majority Religion
				nFollowers = 0
				for pCity in pPlayer:Cities() do
					local CityFollowers = pCity:GetNumFollowers(iReligion)
					nFollowers = nFollowers + CityFollowers
				end
				local totalpop = pPlayer:GetTotalPopulation()
				local FollowDiff = totalpop - nFollowers
				if FollowDiff > (nFollowers / 2) then
					PsalmsEnactable = false
				end
			end
			
			-- Requirements
			---- Tech Prereq
			local pTeam = pPlayer:GetTeam();
			if not (Teams[pTeam]:IsHasTech(iPsalmTechPrereq)) then PsalmsEnactable = false end
			-- Costs
			---- Culture
			------ Create the cost
			local iExtraEraCost = iCurrentEra * iPsalmCultureCostperEra
			local iExtraEraUnityCost = iCurrentEra * iPsalmCultureReducedEraCost * iUnity
			local iEraCost = iExtraEraCost - iExtraEraUnityCost

			local iUnityCost = iPsalmCultureReducedCost * iUnity
			local baseCost = iPsalmCultureMaxBaseCost - iUnityCost

			local iCost =math.ceil((baseCost + iEraCost) * iMod)

			if (pPlayer:GetJONSCulture() < iCost) then PsalmsEnactable = false end
			---- Received Faith
			local iFaithBoost =math.ceil((iPsalmFaithPerCitizen) * iMod)
			---- Get num followers
			nFollowers = 0
			for pCity in pPlayer:Cities() do
				local Followers = pCity:GetNumFollowers(iReligion)
				nFollowers = nFollowers + Followers
			end
			local totalFaitboost = nFollowers * iFaithBoost
			-- Build the Description
			Decisions_Israel_Psalms.Desc = Locale.ConvertTextKey("TXT_KEY_DECISIONS_LEUGI_ISRAEL_PSALMS_DESC", iCost, iFaithBoost, iUnity, totalFaitboost)
			if PsalmsEnactable == false then
				return true, false 
			end
			-- Allow the decision!
			return true, true
		end
	)
	
	Decisions_Israel_Psalms.DoFunc = (
	function(pPlayer)
		-- Get Religious Unity Again
		local iReligion = pPlayer:GetReligionCreatedByPlayer();
		iUnity = GetReligiousUnity(pPlayer, iReligion)
		iUnity = math.floor(iUnity * 100)
		-- Pay the price
		local iCurrentEra = pPlayer:GetCurrentEra()
		---- Culture
		local iExtraEraCost = iCurrentEra * iPsalmCultureCostperEra
			local iExtraEraUnityCost = iCurrentEra * iPsalmCultureReducedEraCost * iUnity
			local iEraCost = iExtraEraCost - iExtraEraUnityCost

			local iUnityCost = iPsalmCultureReducedCost * iUnity
			local baseCost = iPsalmCultureMaxBaseCost - iUnityCost

			local iCost =math.ceil((baseCost + iEraCost) * iMod)
			pPlayer:ChangeJONSCulture(-iCost)
		-- Rewards
		---- Faith
		local iFaithBoost =math.ceil((iPsalmFaithPerCitizen) * iMod)
		---- Get num followers
		nFollowers = 0
		for pCity in pPlayer:Cities() do
			local Followers = pCity:GetNumFollowers(iReligion)
			nFollowers = nFollowers + Followers
		end
		local totalFaithboost = nFollowers * iFaithBoost
		pPlayer:ChangeFaith(totalFaithboost);
		-- Save the Decision
		local iCurrentEra = pPlayer:GetCurrentEra()
		save(pPlayer, "Decisions_Israel_Psalms", iCurrentEra)
	end
	)

Decisions_AddCivilisationSpecific(GameInfoTypes["CIVILIZATION_LEUGI_ISRAEL"], "Decisions_Israel_Psalms", Decisions_Israel_Psalms)

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
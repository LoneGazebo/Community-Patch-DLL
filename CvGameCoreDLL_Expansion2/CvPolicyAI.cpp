/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvPolicyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvInfosSerializationHelper.h"
#if defined(MOD_BALANCE_CORE)
#include "CvEconomicAI.h"
#endif

// Include this after all other headers.
#include "LintFree.h"

/// Constructor
CvPolicyAI::CvPolicyAI(CvPlayerPolicies* currentPolicies):
	m_pCurrentPolicies(currentPolicies)
{
}

/// Destructor
CvPolicyAI::~CvPolicyAI(void)
{
}

/// Clear out AI local variables
void CvPolicyAI::Reset()
{
	m_PolicyAIWeights.clear();
	m_iPolicyWeightPropagationLevels = GC.getPOLICY_WEIGHT_PROPAGATION_LEVELS();
	m_iPolicyWeightPercentDropNewBranch = GC.getPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH();

	CvAssertMsg(m_pCurrentPolicies != NULL, "Policy AI init failure: player policy data is NULL");
	if(m_pCurrentPolicies != NULL)
	{
		CvPolicyXMLEntries* pPolicyEntries = m_pCurrentPolicies->GetPolicies();
		CvAssertMsg(pPolicyEntries != NULL, "Policy AI init failure: no policy data");
		if(pPolicyEntries != NULL)
		{
			// Loop through reading each one and add an entry with 0 weight to our vector
			const int nPolicyEntries = pPolicyEntries->GetNumPolicies();
			for(int i = 0; i < nPolicyEntries; i++)
			{
				m_PolicyAIWeights.push_back(i, 0);
			}
		}
	}
}

/// Serialization read
void CvPolicyAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int iWeight;

	CvAssertMsg(m_pCurrentPolicies->GetPolicies() != NULL, "Policy AI serialization failure: no policy data");
	CvAssertMsg(m_pCurrentPolicies->GetPolicies()->GetNumPolicies() > 0, "Policy AI serialization failure: number of policies not greater than 0");

	// Reset vector
	m_PolicyAIWeights.clear();

	uint uiPolicyArraySize = m_pCurrentPolicies->GetPolicies()->GetNumPolicies();
	// Must set to the final size because we might not be adding in sequentially
	m_PolicyAIWeights.resize(uiPolicyArraySize);
	// Clear the contents in case we are loading a smaller set
	for(uint uiIndex = 0; uiIndex < uiPolicyArraySize; ++uiIndex)
		m_PolicyAIWeights.SetWeight(uiIndex, 0);

	uint uiPolicyCount;
	kStream >> uiPolicyCount;

	for(uint uiIndex = 0; uiIndex < uiPolicyCount; ++uiIndex)
	{
		PolicyTypes ePolicy = (PolicyTypes)CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> iWeight;
		if(ePolicy != NO_POLICY && (uint)ePolicy < uiPolicyArraySize)
			m_PolicyAIWeights.SetWeight((uint)ePolicy, iWeight);
	}
}

/// Serialization write
void CvPolicyAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	CvAssertMsg(m_pCurrentPolicies->GetPolicies() != NULL, "Policy AI serialization failure: no policy data");
	CvAssertMsg(m_pCurrentPolicies->GetPolicies()->GetNumPolicies() > 0, "Policy AI serialization failure: number of policies not greater than 0");

	// Loop through writing each entry
	uint uiPolicyCount = m_pCurrentPolicies->GetPolicies()->GetNumPolicies();
	kStream << uiPolicyCount;

	for(int i = 0; i < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); i++)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, static_cast<const PolicyTypes>(i));
		kStream << m_PolicyAIWeights.GetWeight(i);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvPolicyAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent)
{
	if (iWeight==0)
		return;

	int iPolicy;
	CvPolicyEntry* entry;
	int* paiTempWeights;

	CvPolicyXMLEntries* pkPolicyEntries = m_pCurrentPolicies->GetPolicies();
	// Create a temporary array of weights
	paiTempWeights = (int*)_alloca(sizeof(int*) * pkPolicyEntries->GetNumPolicies());

	// Loop through all our policies
	for(iPolicy = 0; iPolicy < pkPolicyEntries->GetNumPolicies(); iPolicy++)
	{
		entry = pkPolicyEntries->GetPolicyEntry(iPolicy);

		// Set its weight by looking at policy's weight for this flavor and using iWeight multiplier passed in
		if(entry)
			paiTempWeights[iPolicy] = entry->GetFlavorValue(eFlavor) * iWeight;
		else
			paiTempWeights[iPolicy] = 0;
	}

	// Propagate these values left in the tree so prereqs get bought
	if(iPropagationPercent > 0)
	{
		WeightPrereqs(paiTempWeights, iPropagationPercent);
	}

	// Add these weights over previous ones
	for(iPolicy = 0; iPolicy < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicy++)
	{
		m_PolicyAIWeights.IncreaseWeight(iPolicy, paiTempWeights[iPolicy]);
	}
}

/// Choose a player's next policy purchase (could be opening a branch)
int CvPolicyAI::ChooseNextPolicy(CvPlayer* pPlayer)
{
	RandomNumberDelegate fcn;
	fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	int iRtnValue = (int)NO_POLICY;
	int iPolicyLoop;
	vector<int> aLevel3Tenets;

	bool bMustChooseTenet = (pPlayer->GetNumFreeTenets() > 0);

	// Create a new vector holding only policies we can currently adopt
	m_AdoptablePolicies.clear();

	// Loop through adding the adoptable policies
	for(iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		if(m_pCurrentPolicies->CanAdoptPolicy((PolicyTypes) iPolicyLoop) && (!bMustChooseTenet || m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetLevel() > 0))
		{
			int iWeight = 0;

			iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
#if defined(MOD_BALANCE_CORE)
			//Grand Strategy Considerations - if valid, it doubles our initial weighting.
			// == Grand Strategy ==
			AIGrandStrategyTypes eGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
			bool bSeekingDiploVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");
			bool bSeekingConquestVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
			bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
			bool bSeekingScienceVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");
			CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo((PolicyTypes) iPolicyLoop);
			if(pkPolicyInfo)
			{
				for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
				{
					FlavorTypes eFlavor = (FlavorTypes)iFlavor;
					if(eFlavor == NO_FLAVOR)
						continue;

					if(pkPolicyInfo->GetFlavorValue(eFlavor) > 0)
					{
						if(GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_DIPLOMACY")
						{
							if(bSeekingDiploVictory)
							{
								iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
							}
							else if(bSeekingConquestVictory)
							{
								iWeight -= m_PolicyAIWeights.GetWeight(iPolicyLoop);
							}
							else
							{
								for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
								{
									PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;
									if(eMinor == NO_PLAYER)
										continue;

									// Loop through all minors - if we're itching to conquer, bail out on diplo policies.
									if(GET_PLAYER(eMinor).isMinorCiv() && GET_PLAYER(eMinor).isAlive())
									{
										if(pPlayer->GetDiplomacyAI()->GetMinorCivApproach(eMinor) >= MINOR_CIV_APPROACH_CONQUEST)
										{
											iWeight -= m_PolicyAIWeights.GetWeight(iPolicyLoop);
											break;
										}
									}
								}
							}
						}
						if(bSeekingConquestVictory && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_OFFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MILITARY_TRAINING"))
						{
							iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
						}
						if(bSeekingCultureVictory && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_CULTURE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_WONDER"))
						{
							iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
						}
						if(bSeekingScienceVictory && (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SCIENCE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SPACESHIP"))
						{
							iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
						}
					}
				}
			}
			//If this is an ideology policy, let's snap those up.
			if(m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetLevel() > 0)
			{
				iWeight *= (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetLevel() + 1);
			}
			if(!pPlayer->GetCorporations()->HasFoundedCorporation())
			{
				//Corporate-specific policies should only be taken if you have a corporation.
				if(m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->IsCorporationOfficesAsFranchises() ||m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->IsCorporationRandomForeignFranchise() || m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->IsCorporationFreeFranchiseAbovePopular())
				{
					iWeight = 0;
				}
			}
			//Older branches should be slowly phased out.
			PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes)m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetPolicyBranchType();
			if(ePolicyBranch != NO_POLICY_BRANCH_TYPE)
			{
				CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
				if(pkPolicyBranchInfo)
				{
					//If we're already in this branch, let's get a bonus based on how many we have in it (this will push the AI to finish branches quickly.
					if(m_pCurrentPolicies->GetNumPoliciesOwnedInBranch(ePolicyBranch) > 0 || m_pCurrentPolicies->IsPolicyBranchUnlocked(ePolicyBranch))
					{
						iWeight *= (m_pCurrentPolicies->GetNumPoliciesOwnedInBranch(ePolicyBranch) + 1);
					}
					else
					{
						int iPolicyEra = pkPolicyBranchInfo->GetEraPrereq();
						int iPlayerEra = pPlayer->GetCurrentEra();
						if(iPolicyEra < iPlayerEra)
						{
							iWeight /= max(1, (iPlayerEra - iPolicyEra));
						}
					}
				}
				if(ePolicyBranch == (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_PIETY", true))
				{
					EconomicAIStrategyTypes eStrategyBuildingReligion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_DEVELOPING_RELIGION", true);
					bool bBuildingReligion = false;
					if (eStrategyBuildingReligion != NO_ECONOMICAISTRATEGY)
					{
						bBuildingReligion = pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyBuildingReligion);
					}
					if(!bBuildingReligion)
					{
						iWeight = 0;
					}
					else
					{
						iWeight *= 5;
					}
					if(GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0 && pPlayer->GetReligions()->GetReligionCreatedByPlayer() <= RELIGION_PANTHEON)
					{
						iWeight = 0;
					}
				}
				if(ePolicyBranch == (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_PATRONAGE", true))
				{
					if(GC.getGame().GetNumMinorCivsAlive() <= 0)
					{
						iWeight = 0;
					}
					else if(pPlayer->GetDiplomacyAI()->GetNumMinorCivApproach(MINOR_CIV_APPROACH_FRIENDLY) <= 0)
					{
						iWeight = 0;
					}
				}
			}
			
#endif

			// Does this policy finish a branch for us?
			if(m_pCurrentPolicies->WillFinishBranchIfAdopted((PolicyTypes) iPolicyLoop))
			{
				int iPolicyBranch = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetPolicyBranchType();
				if(iPolicyBranch != NO_POLICY_BRANCH_TYPE)
				{
					int iFinisherPolicy = m_pCurrentPolicies->GetPolicies()->GetPolicyBranchEntry(iPolicyBranch)->GetFreeFinishingPolicy();
					if(iFinisherPolicy != NO_POLICY)
					{
						iWeight += m_PolicyAIWeights.GetWeight(iFinisherPolicy);
					}
				}
			}
			m_AdoptablePolicies.push_back(iPolicyLoop + GC.getNumPolicyBranchInfos(), iWeight);

			if (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetLevel() == 3)
			{
				aLevel3Tenets.push_back(iPolicyLoop);
			}
		}
	}

	// Did we already start a branch in the set that is mutually exclusive?
	bool bStartedAMutuallyExclusiveBranch = false;
	for(int iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
	{
		const PolicyBranchTypes ePolicyBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
		if(pkPolicyBranchInfo)
		{
			if(pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
			{
				if(pkPolicyBranchInfo->IsMutuallyExclusive())
				{
					bStartedAMutuallyExclusiveBranch = true;
				}
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
						//Leftover from Vanilla victory
#else
	AIGrandStrategyTypes eCultureGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	AIGrandStrategyTypes eCurrentGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
#endif
	// Loop though the branches adding each as another possibility
	if (!bMustChooseTenet)
	{
		for(int iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
		{
			const PolicyBranchTypes ePolicyBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
			CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
			if(pkPolicyBranchInfo)
			{
				if(bStartedAMutuallyExclusiveBranch && pkPolicyBranchInfo->IsMutuallyExclusive())
				{
					continue;
				}

#if defined(MOD_BALANCE_CORE)
				bool bNeedToFinish = false;
				for(int iBranchLoop2 = 0; iBranchLoop2 < GC.getNumPolicyBranchInfos(); iBranchLoop2++)
				{
					const PolicyBranchTypes ePolicyBranch2 = static_cast<PolicyBranchTypes>(iBranchLoop2);
					CvPolicyBranchEntry* pkPolicyBranchInfo2 = GC.getPolicyBranchInfo(ePolicyBranch2);
					//Do we already have a different policy branch unlocked?
					if(pkPolicyBranchInfo2 && ePolicyBranch2 != ePolicyBranch && pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch2))
					{
						//Have we not finished it yet? If so, let's not open a new one.
						if(!pPlayer->GetPlayerPolicies()->HasPolicy((PolicyTypes)pkPolicyBranchInfo2->GetFreeFinishingPolicy()))
						{
							bNeedToFinish = true;
							break;
						}
					}
				}
				if(bNeedToFinish)
				{
					continue;
				}
#endif
				if(pPlayer->GetPlayerPolicies()->CanUnlockPolicyBranch(ePolicyBranch) && !pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
				{
					int iBranchWeight = 0;

					// Does this branch actually help us, based on game options?
					if(IsBranchEffectiveInGame(ePolicyBranch))
					{
						iBranchWeight += WeighBranch(ePolicyBranch);

						iBranchWeight *= (100 - m_iPolicyWeightPercentDropNewBranch);
						iBranchWeight /= 100;
#if defined(MOD_BALANCE_CORE)
						//Leftover from Vanilla victory
#else
						if(eCurrentGrandStrategy == eCultureGrandStrategy)
						{
							iBranchWeight /= 3;
						}
#endif
					}

					m_AdoptablePolicies.push_back(iBranchLoop, iBranchWeight);
				}
			}
		}
	}

	m_AdoptablePolicies.SortItems();
	LogPossiblePolicies();
#if defined(MOD_BALANCE_CORE)
#else
	// If there were any Level 3 tenets found, consider going for the one that matches our victory strategy
	if (aLevel3Tenets.size() > 0)
	{
		vector<int>::const_iterator it;
		for (it = aLevel3Tenets.begin(); it != aLevel3Tenets.end(); it++)
		{
			CvPolicyEntry *pEntry;
			pEntry = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(*it);
			if (pEntry)
			{
				AIGrandStrategyTypes eGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();
				if (eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}
				else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
				{
					if (pEntry->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE")) > 0)
					{
						LogPolicyChoice((PolicyTypes)*it);
						return (*it) + GC.getNumPolicyBranchInfos();
					}
				}

			}
		}
	}
#endif
	CvAssertMsg(m_AdoptablePolicies.GetTotalWeight() >= 0, "Total weights of considered policies should not be negative! Please send Anton your save file and version.");

	// If total weight is above 0, choose one above a threshold
	if(m_AdoptablePolicies.GetTotalWeight() > 0)
	{
		int iNumChoices = GC.getGame().getHandicapInfo().GetPolicyNumOptions();
		iRtnValue = m_AdoptablePolicies.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing policy from Top Choices");
	}
	// Total weight may be 0 if the only branches and policies left are ones that are ineffective in our game, but we gotta pick something
	else if(m_AdoptablePolicies.GetTotalWeight() == 0 && m_AdoptablePolicies.size() > 0)
	{
		iRtnValue = m_AdoptablePolicies.ChooseAtRandom(&fcn, "Choosing policy at random (no good choices)");
	}

	// Log our choice
	if(iRtnValue != (int)NO_POLICY)
	{
		if(iRtnValue >= GC.getNumPolicyBranchInfos())
		{
			LogPolicyChoice((PolicyTypes)(iRtnValue - GC.getNumPolicyBranchInfos()));
		}
		else
		{
			LogBranchChoice((PolicyBranchTypes)iRtnValue);
		}
	}

	return iRtnValue;
}

void CvPolicyAI::DoChooseIdeology(CvPlayer *pPlayer)
{
	int iFreedomPriority = 0;
	int iAutocracyPriority = 0;
	int iOrderPriority = 0;
	int iFreedomMultiplier = 1;
	int iAutocracyMultiplier = 1;
	int iOrderMultiplier = 1;
	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return;
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES)
	{
		if(GET_TEAM(pPlayer->getTeam()).IsVassalOfSomeone())
		{
			TeamTypes eMasterTeam = GET_TEAM(pPlayer->getTeam()).GetMaster();
			if(eMasterTeam != NO_TEAM)
			{
				// Loop through all players to see if they're on our team
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eMaster = (PlayerTypes) iPlayerLoop;

					// Assumes one player per team for master
					if(GET_PLAYER(eMaster).getTeam() == GET_TEAM(eMasterTeam).GetID())
					{
						if(GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree() != NO_POLICY_BRANCH_TYPE)
						{
							pPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree(), true, false);
							LogBranchChoice(GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree());
							return;
						}
					}
				}
			}
		}
	}
#endif

	// First consideration is our victory type
	int iConquestPriority = max(0, pPlayer->GetGrandStrategyAI()->GetConquestPriority());
	int iDiploPriority = max(0, pPlayer->GetGrandStrategyAI()->GetUnitedNationsPriority());
	int iTechPriority = max(0, pPlayer->GetGrandStrategyAI()->GetSpaceshipPriority());
	int iCulturePriority = max(0, pPlayer->GetGrandStrategyAI()->GetCulturePriority());
	
#if defined(MOD_EVENTS_IDEOLOGIES)
	if (MOD_EVENTS_IDEOLOGIES) {
		CvPlayerPolicies* pPolicies = pPlayer->GetPlayerPolicies();

		// Just jump on the band-wagon and hard code for three ideologies!!!
		if (!pPolicies->CanAdoptIdeology(eFreedomBranch)) {
			iFreedomMultiplier = 0;
		}
		if (!pPolicies->CanAdoptIdeology(eAutocracyBranch)) {
			iAutocracyMultiplier = 0;
		}
		if (!pPolicies->CanAdoptIdeology(eOrderBranch)) {
			iOrderMultiplier = 0;
		}
	}
#endif

#if defined(MOD_EVENTS_IDEOLOGIES)
	if (iFreedomMultiplier != 0 && iAutocracyMultiplier != 0 && iOrderMultiplier != 0) {
#endif
		// Rule out one ideology if we are clearly (at least 25% more priority) going for the victory this ideology doesn't support
		int iClearPrefPercent = GC.getIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF();
		if (iConquestPriority > (iDiploPriority   * (100 + iClearPrefPercent) / 100) &&
			iConquestPriority > (iTechPriority    * (100 + iClearPrefPercent) / 100) &&
			iConquestPriority > (iCulturePriority * (100 + iClearPrefPercent) / 100))
		{
			iFreedomMultiplier = 0;
		}
		else if (iDiploPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
				 iDiploPriority > (iTechPriority     * (100 + iClearPrefPercent) / 100) &&
				 iDiploPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
		{
			iOrderMultiplier = 0;
		}
		else if (iTechPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
				 iTechPriority > (iDiploPriority    * (100 + iClearPrefPercent) / 100) &&
				 iTechPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
		{
			iAutocracyMultiplier = 0;
		}
#if defined(MOD_EVENTS_IDEOLOGIES)
	}
#endif

	int iFreedomTotal = iDiploPriority + iTechPriority + iCulturePriority;
	int iAutocracyTotal = iDiploPriority + iConquestPriority + iCulturePriority;
	int iOrderTotal = iTechPriority + iConquestPriority + iCulturePriority;
	int iGrandTotal = iFreedomTotal + iAutocracyTotal + iOrderTotal;

	if (iGrandTotal > 0)
	{
		int iPriorityToDivide = GC.getIDEOLOGY_SCORE_GRAND_STRATS();
		iFreedomPriority = (iFreedomTotal * iPriorityToDivide) / iGrandTotal;
		iAutocracyPriority = (iAutocracyTotal * iPriorityToDivide) / iGrandTotal;
		iOrderPriority = (iOrderTotal * iPriorityToDivide) / iGrandTotal;
	}

	CvString stage = "After Grand Strategies";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Next look at free policies we can get
	iFreedomPriority += PolicyHelpers::GetNumFreePolicies(eFreedomBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();
	iAutocracyPriority += PolicyHelpers::GetNumFreePolicies(eAutocracyBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();
	iOrderPriority += PolicyHelpers::GetNumFreePolicies(eOrderBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();

	stage = "After Free Policies";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Finally see what our friends (and enemies) have already chosen
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eLoopPlayer);
			PolicyBranchTypes eOtherPlayerIdeology;
			eOtherPlayerIdeology = kOtherPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

			switch(pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ true))
			{
			case MAJOR_CIV_APPROACH_HOSTILE:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
					iOrderPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
					iOrderPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
					iFreedomPriority += GC.getIDEOLOGY_SCORE_HOSTILE();
				}
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_GUARDED();
					iOrderPriority += GC.getIDEOLOGY_SCORE_GUARDED();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_GUARDED();
					iOrderPriority += GC.getIDEOLOGY_SCORE_GUARDED();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_GUARDED();
					iFreedomPriority += GC.getIDEOLOGY_SCORE_GUARDED();
				}
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_AFRAID();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_AFRAID();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += GC.getIDEOLOGY_SCORE_AFRAID();
				}
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += GC.getIDEOLOGY_SCORE_FRIENDLY();
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_FRIENDLY();
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += GC.getIDEOLOGY_SCORE_FRIENDLY();
				}
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				// No changes
				break;
			}
		}
	}

	stage = "After Relations";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Look at Happiness impacts
	int iHappinessModifier = GC.getIDEOLOGY_SCORE_HAPPINESS();

	// -- Happiness we could add through tenets
	int iHappinessDelta;
#if defined(MOD_BALANCE_CORE)
	//This was a dumb reason to weigh an ideology branch.
#else
	int iHappinessPoliciesInBranch;
	iHappinessDelta = GetBranchBuildingHappiness(pPlayer, eFreedomBranch);
	iHappinessPoliciesInBranch = GetNumHappinessPolicies(pPlayer, eFreedomBranch);
	if (iHappinessPoliciesInBranch > 0)
	{
		iFreedomPriority += iHappinessDelta * iHappinessModifier / iHappinessPoliciesInBranch;		
	}
	iHappinessDelta = GetBranchBuildingHappiness(pPlayer, eAutocracyBranch);
	iHappinessPoliciesInBranch = GetNumHappinessPolicies(pPlayer, eAutocracyBranch);
	if (iHappinessPoliciesInBranch > 0)
	{
		iAutocracyPriority += iHappinessDelta * iHappinessModifier / iHappinessPoliciesInBranch;		
	}
	iHappinessDelta = GetBranchBuildingHappiness(pPlayer, eOrderBranch);
	iHappinessPoliciesInBranch = GetNumHappinessPolicies(pPlayer, eOrderBranch);
	if (iHappinessPoliciesInBranch > 0)
	{
		iOrderPriority += iHappinessDelta * iHappinessModifier / iHappinessPoliciesInBranch;		
	}

	stage = "After Tenet Happiness Boosts";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);
#endif
#if defined(MOD_BALANCE_CORE)
	bool bFirstIdeology = true;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eLoopPlayer);
			PolicyBranchTypes eOtherPlayerIdeology;
			eOtherPlayerIdeology = kOtherPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
			if(eOtherPlayerIdeology != NO_POLICY_BRANCH_TYPE)
			{
				bFirstIdeology = false;
				break;
			}
		}
	}
	if(!bFirstIdeology)
	{
#endif
	// -- Happiness we'd lose through Public Opinion
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eFreedomBranch));
	iFreedomPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eAutocracyBranch));
	iAutocracyPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eOrderBranch));
	iOrderPriority += iHappinessDelta * iHappinessModifier;

	stage = "After Public Opinion Happiness";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);
#if defined(MOD_BALANCE_CORE)
	}
#endif
#if defined(MOD_BALANCE_CORE)
	// bigger random add-on
	iFreedomPriority += GC.getGame().getJonRandNum(75, "Freedom random priority bump");
	iAutocracyPriority += GC.getGame().getJonRandNum(75, "Autocracy random priority bump");
	iOrderPriority += GC.getGame().getJonRandNum(75, "Order random priority bump");
#else
	// Small random add-on
	iFreedomPriority += GC.getGame().getJonRandNum(10, "Freedom random priority bump");
	iAutocracyPriority += GC.getGame().getJonRandNum(10, "Autocracy random priority bump");
	iOrderPriority += GC.getGame().getJonRandNum(10, "Order random priority bump");
#endif
	stage = "After Random (1 to 10)";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Rule out any branches that are totally out of consideration
	iFreedomPriority = iFreedomPriority * iFreedomMultiplier;
	iAutocracyPriority = iAutocracyPriority * iAutocracyMultiplier;
	iOrderPriority = iOrderPriority * iOrderMultiplier;

	stage = "Final (after Clear Victory Preference)";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Pick the ideology
	PolicyBranchTypes eChosenBranch;
	if (iFreedomPriority >= iAutocracyPriority && iFreedomPriority >= iOrderPriority)
	{
		eChosenBranch = eFreedomBranch;
	}
	else if (iAutocracyPriority >= iFreedomPriority && iAutocracyPriority >= iOrderPriority)
	{
		eChosenBranch = eAutocracyBranch;
	}
	else
	{
		eChosenBranch = eOrderBranch;
	}
	pPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(eChosenBranch, true, false);
	LogBranchChoice(eChosenBranch);
#if defined(MOD_BALANCE_CORE)
	int iPolicyGEorGM = pPlayer->GetPlayerTraits()->GetPolicyGEorGM();
	if(iPolicyGEorGM > 0)
	{
		CvCity* pLoopCity;
		int iLoop;
		int iValue = iPolicyGEorGM * (pPlayer->GetCurrentEra() + 1);
		SpecialistTypes eBestSpecialist = NO_SPECIALIST;
		int iRandom = GC.getGame().getJonRandNum(100, "Random GE or GM value");
		if(iRandom <= 33)
		{
			eBestSpecialist = (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_ENGINEER");
		}
		else if(iRandom > 34 && iRandom <= 66)
		{
			eBestSpecialist = (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_SCIENTIST");
		}
		else if(iRandom > 66)
		{
			eBestSpecialist = (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_MERCHANT");			
		}
		if(eBestSpecialist != NULL)
		{
			for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_ENGINEER"))
				{
					pLoopCity->changeProduction(iValue);
				}
				else if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_MERCHANT"))
				{
					pPlayer->GetTreasury()->ChangeGold(iValue);
				}
				else if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_SCIENTIST"))
				{
					TechTypes eCurrentTech = pPlayer->GetPlayerTechs()->GetCurrentResearch();
					if(eCurrentTech == NO_TECH)
					{
						pPlayer->changeOverflowResearch(iValue);
						if(pPlayer->getOverflowResearch() <= 0)
						{
							pPlayer->setOverflowResearch(0);
						}
					}
					else
					{
						GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iValue, pPlayer->GetID());
						if(GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->GetResearchProgress(eCurrentTech) <= 0)
						{
							GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->SetResearchProgress(eCurrentTech, 0, pPlayer->GetID());
						}
					}
				}
				CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eBestSpecialist);
				if(pkSpecialistInfo)
				{
					int iGPThreshold = pLoopCity->GetCityCitizens()->GetSpecialistUpgradeThreshold((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass());
					iGPThreshold *= 100;
					//Get % of threshold for test.
					iGPThreshold *= iPolicyGEorGM;
					iGPThreshold /= 100;
				
					pLoopCity->GetCityCitizens()->ChangeSpecialistGreatPersonProgressTimes100(eBestSpecialist, iGPThreshold);
					if(pPlayer->GetID() == GC.getGame().getActivePlayer())
					{
						iGPThreshold /= 100;
						char text[256] = {0};
						float fDelay = 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GREAT_PEOPLE]", iGPThreshold);
						DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text, fDelay);
						CvNotifications* pNotification = pPlayer->GetNotifications();
						if(pNotification)
						{
							CvString strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS", iGPThreshold);
							CvString strSummary;
							// Class specific specialist message
							if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
							{
								strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS_MERCHANT", iGPThreshold);
							}
							else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
							{
								strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS_ENGINEER", iGPThreshold);
							}
							else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
							{
								strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS_SCIENTIST", iGPThreshold);
							}
							strSummary = GetLocalizedText("TXT_KEY_POLICY_ADOPT_SUMMARY_GP_BONUS");
							pNotification->Add(NOTIFICATION_GENERIC, strMessage, strSummary, -1, -1, -1);
						}
					}
				}
			}
		}
	}
#endif
#if defined(MOD_BUGFIX_MISSING_POLICY_EVENTS)
	if (MOD_BUGFIX_MISSING_POLICY_EVENTS)
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(pPlayer->GetID());
			args->Push(eChosenBranch);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "PlayerAdoptPolicyBranch", args.get(), bResult);
		}
	}
#endif
}

/// Should the AI look at switching ideology branches?
void CvPolicyAI::DoConsiderIdeologySwitch(CvPlayer* pPlayer)
{
	// Gather basic Ideology info
	int iCurrentHappiness = pPlayer->GetExcessHappiness();
	int iPublicOpinionUnhappiness = pPlayer->GetCulture()->GetPublicOpinionUnhappiness();
	PolicyBranchTypes ePreferredIdeology = pPlayer->GetCulture()->GetPublicOpinionPreferredIdeology();
	PolicyBranchTypes eCurrentIdeology = pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
#if !defined(NO_ACHIEVEMENTS)
	PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES)
	{
		if(GET_TEAM(pPlayer->getTeam()).IsVassalOfSomeone() && pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree() != NO_POLICY_BRANCH_TYPE)
		{
			TeamTypes eMasterTeam = GET_TEAM(pPlayer->getTeam()).GetMaster();
			if(eMasterTeam != NO_TEAM)
			{
				// Loop through all players to see if they're on our team
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eMaster = (PlayerTypes) iPlayerLoop;

					// Assumes one player per team for master
					if(GET_PLAYER(eMaster).getTeam() == GET_TEAM(eMasterTeam).GetID())
					{
						if(GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree() != NO_POLICY_BRANCH_TYPE && GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree() != pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree())
						{
							// Cleared all obstacles -- REVOLUTION!
							pPlayer->SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
							pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree());	
							Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
							Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
							pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS(), -1);
							return;
						}
					}
				}
			}
		}
	}
#endif	
	// Possible enough that we need to look at this in detail?
#if defined(MOD_BALANCE_CORE)
	if (iCurrentHappiness <= GC.getSUPER_UNHAPPY_THRESHOLD() && iPublicOpinionUnhappiness >= (-1 * GC.getSUPER_UNHAPPY_THRESHOLD()))
	{
		//Sanity check - would a change to this branch simply make us unhappy in another way? If so, don't do it.
		if(ePreferredIdeology != NO_POLICY_BRANCH_TYPE)
		{
			int iUnhappiness = pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(ePreferredIdeology);
			if(iUnhappiness >= iPublicOpinionUnhappiness)
			{
				return;
			}
		}
		//Final sanity check - are we flip-flopping?
		if(GC.getGame().getGameTurn() - pPlayer->GetCulture()->GetTurnIdeologySwitch() <= 30)
		{
			return;
		}

		// Cleared all obstacles -- REVOLUTION!
		pPlayer->SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
		pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);	
		Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
		Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
		pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS(), -1);
	}
#endif
#if defined(MOD_BALANCE_CORE)
	else if (iCurrentHappiness <= GC.getVERY_UNHAPPY_THRESHOLD() && iPublicOpinionUnhappiness >= (-1 * GC.getVERY_UNHAPPY_THRESHOLD()))
#else
	if (iCurrentHappiness <= GC.getSUPER_UNHAPPY_THRESHOLD() && iPublicOpinionUnhappiness >= 10)
#endif
	{
		// How much Happiness could we gain from a switch?
#if !defined(MOD_BALANCE_CORE)
		int iHappinessCurrentIdeology = GetBranchBuildingHappiness(pPlayer, eCurrentIdeology);
		int iHappinessPreferredIdeology = GetBranchBuildingHappiness(pPlayer, ePreferredIdeology);
#endif
		// Does the switch fight against our clearly preferred victory path?
		bool bDontSwitchFreedom = false;
		bool bDontSwitchOrder = false;
		bool bDontSwitchAutocracy = false;
		int iConquestPriority = pPlayer->GetGrandStrategyAI()->GetConquestPriority();
		int iDiploPriority = pPlayer->GetGrandStrategyAI()->GetUnitedNationsPriority();
		int iTechPriority = pPlayer->GetGrandStrategyAI()->GetSpaceshipPriority();
		int iCulturePriority = pPlayer->GetGrandStrategyAI()->GetCulturePriority();
		int iClearPrefPercent = GC.getIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF();
		if (iConquestPriority > (iDiploPriority   * (100 + iClearPrefPercent) / 100) &&
			iConquestPriority > (iTechPriority    * (100 + iClearPrefPercent) / 100) &&
			iConquestPriority > (iCulturePriority * (100 + iClearPrefPercent) / 100))
		{
			bDontSwitchFreedom = true;
		}
		else if (iDiploPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
			iDiploPriority > (iTechPriority     * (100 + iClearPrefPercent) / 100) &&
			iDiploPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
		{
			bDontSwitchOrder = true;
		}
		else if (iTechPriority > (iConquestPriority * (100 + iClearPrefPercent) / 100) &&
			iTechPriority > (iDiploPriority    * (100 + iClearPrefPercent) / 100) &&
			iTechPriority > (iCulturePriority  * (100 + iClearPrefPercent) / 100))
		{
			bDontSwitchAutocracy = true;
		}
#if defined(MOD_BALANCE_CORE)
		//Sanity check - would a change to this branch simply make us unhappy in another way? If so, don't do it.
		if(ePreferredIdeology != NO_POLICY_BRANCH_TYPE)
		{
			int iUnhappiness = pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(ePreferredIdeology);
			if(iUnhappiness >= iPublicOpinionUnhappiness)
			{
				return;
			}
			if((iCurrentHappiness + iUnhappiness) <= GC.getVERY_UNHAPPY_THRESHOLD())
			{
				return;
			}
			// Finally see what our friends (and enemies) have already chosen
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if (eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
				{
					CvPlayer &kOtherPlayer = GET_PLAYER(eLoopPlayer);
					PolicyBranchTypes eOtherPlayerIdeology;
					eOtherPlayerIdeology = kOtherPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

					switch(pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ true))
					{
						case MAJOR_CIV_APPROACH_HOSTILE:
							if (eOtherPlayerIdeology == ePreferredIdeology)
							{
								return;
							}
							break;
						case MAJOR_CIV_APPROACH_WAR:
							if (eOtherPlayerIdeology == ePreferredIdeology)
							{
								return;
							}
							break;
					}
				}
			}
#endif
#if defined(MOD_BALANCE_CORE)
#else
		int iTotalHappinessImprovement = iPublicOpinionUnhappiness + iHappinessPreferredIdeology - iHappinessCurrentIdeology;
		if (iTotalHappinessImprovement >= 10)
		{
#endif
			if (bDontSwitchFreedom && ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM())
			{
				return;
			}
			if (bDontSwitchAutocracy && ePreferredIdeology == GC.getPOLICY_BRANCH_AUTOCRACY())
			{
				return;
			}
			if (bDontSwitchOrder && ePreferredIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				return;
			}
#if defined(MOD_BALANCE_CORE)
			//Final sanity check - are we flip-flopping?
			if(GC.getGame().getGameTurn() - pPlayer->GetCulture()->GetTurnIdeologySwitch() <= 30)
			{
				return;
			}
#endif

			// Cleared all obstacles -- REVOLUTION!
			pPlayer->SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
			pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);	
#if defined(MOD_BALANCE_CORE)
			Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
			Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
			pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS(), -1);
			if(eCurrentIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
#endif
#if !defined(NO_ACHIEVEMENTS)
			if (ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM() && eCurrentIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				if (GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
				}
			}
#endif
#if defined(MOD_BALANCE_CORE)
			}
#endif
		}
	}
}

/// What's the total Happiness benefit we could get from all policies/tenets in the branch based on our current buildings?
int CvPolicyAI::GetBranchBuildingHappiness(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{
	// Policy Building Mods
	int iSpecialPolicyBuildingHappiness = 0;
	int iBuildingClassLoop;
	BuildingClassTypes eBuildingClass;
	for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
	{
		PolicyTypes ePolicy = (PolicyTypes)iPolicyLoop;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if(pkPolicyInfo)
		{
			if (pkPolicyInfo->GetPolicyBranchType() == eBranch)
			{
				for(iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if (!pkBuildingClassInfo)
					{
						continue;
					}

					if (pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass) != 0)
					{
						BuildingTypes eBuilding = (BuildingTypes)pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
						if (eBuilding != NO_BUILDING)
						{
							CvCity *pCity;
							int iLoop;
							for (pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
							{
								if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
								{
									iSpecialPolicyBuildingHappiness += pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass);
								}
							}
						}
					}
				}
			}
		}
	}
	return iSpecialPolicyBuildingHappiness;
}

/// How many policies in this branch help happiness?
int CvPolicyAI::GetNumHappinessPolicies(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{
	int iRtnValue = 0;
	int iBuildingClassLoop;
	BuildingClassTypes eBuildingClass;
	for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
	{
		PolicyTypes ePolicy = (PolicyTypes)iPolicyLoop;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if(pkPolicyInfo)
		{
			if (pkPolicyInfo->GetPolicyBranchType() == eBranch)
			{
				for(iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if (!pkBuildingClassInfo)
					{
						continue;
					}

					BuildingTypes eBuilding = (BuildingTypes)pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
					if (eBuilding != NO_BUILDING)
					{
						// Don't count a building that can only be built in conquered cities
						CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
						if (!pkEntry || pkEntry->IsNoOccupiedUnhappiness())
						{
							continue;
						}

						if (pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass) != 0)
						{
							iRtnValue++;
							break;
						}
					}
				}
			}
		}
	}
	return iRtnValue;
}

//=====================================
// PRIVATE METHODS
//=====================================
/// Add weights to policies that are prereqs for the ones already weighted in this strategy
void CvPolicyAI::WeightPrereqs(int* paiTempWeights, int iPropagationPercent)
{
	int iPolicyLoop;

	// Loop through policies looking for ones that are just getting some new weight
	for(iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		// If found one, call our recursive routine to weight everything to the left in the tree
		if(paiTempWeights[iPolicyLoop] > 0)
		{
			PropagateWeights(iPolicyLoop, paiTempWeights[iPolicyLoop], iPropagationPercent, 0);
		}
	}
}

/// Recursive routine to weight all prerequisite policies
void CvPolicyAI::PropagateWeights(int iPolicy, int iWeight, int iPropagationPercent, int iPropagationLevel)
{
	if(iPropagationLevel < m_iPolicyWeightPropagationLevels)
	{
		int iPropagatedWeight = iWeight * iPropagationPercent / 100;

		// Loop through all prerequisites
		for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
		{
			// Did we find a prereq?
			int iPrereq = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicy)->GetPrereqAndPolicies(iI);
			if(iPrereq != NO_POLICY)
			{
				// Apply reduced weight here.  Note that we apply these to the master weight array, not
				// the temporary one.  The temporary one is just used to hold the newly weighted policies
				// (from which this weight propagation must originate).
				m_PolicyAIWeights.IncreaseWeight(iPrereq, iPropagatedWeight);

				// Recurse to its prereqs (assuming we have any weight left)
				if(iPropagatedWeight > 0)
				{
					PropagateWeights(iPrereq, iPropagatedWeight, iPropagationPercent, iPropagationLevel++);
				}
			}
			else
			{
				break;
			}
		}
	}
}

/// Priority for opening up this branch
int CvPolicyAI::WeighBranch(PolicyBranchTypes eBranch)
{
	int iWeight = 0;

	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranch);
	if(pkPolicyBranchInfo)
	{
		for(int iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
		{
			const PolicyTypes ePolicyLoop = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(ePolicyLoop);
			if(pkLoopPolicyInfo)
			{
				// Policy we don't have?
				if(!m_pCurrentPolicies->HasPolicy(ePolicyLoop))
				{
					// From this branch we are considering opening?
					if(pkLoopPolicyInfo->GetPolicyBranchType() == eBranch)
					{
						// With no prereqs?
						if(pkLoopPolicyInfo->GetPrereqAndPolicies(0) == NO_POLICY)
						{
							iWeight += m_PolicyAIWeights.GetWeight(iPolicyLoop);
						}
					}
				}
			}
		}

		// Add weight of free policy from branch
		iWeight += m_PolicyAIWeights.GetWeight(pkPolicyBranchInfo->GetFreePolicy());
	}

	return iWeight;
}

/// Based on game options (religion off, science off, etc.), would this branch do us any good?
bool CvPolicyAI::IsBranchEffectiveInGame(PolicyBranchTypes eBranch)
{
	CvPolicyBranchEntry* pBranchInfo = GC.getPolicyBranchInfo(eBranch);
	CvAssertMsg(pBranchInfo, "Branch info not found! Please send Anton your save file and version.");
	if (!pBranchInfo) return false;
	
	if (pBranchInfo->IsDelayWhenNoReligion())
		if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
			return false;

	if (pBranchInfo->IsDelayWhenNoCulture())
		if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
			return false;

	if (pBranchInfo->IsDelayWhenNoScience())
		if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

	if (pBranchInfo->IsDelayWhenNoCityStates())
		if (GC.getGame().GetNumMinorCivsEver() <= 0)
			return false;

	return true;
}

/// Log all possible policy choices
void CvPolicyAI::LogPossiblePolicies()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		int iNumBranches = GC.getNumPolicyBranchInfos();

		// Dump out the weight of each possible policy
		for(int iI = 0; iI < m_AdoptablePolicies.size(); iI++)
		{
			int iWeight = m_AdoptablePolicies.GetWeight(iI);

			if(m_AdoptablePolicies.GetElement(iI) < iNumBranches)
			{
				strTemp.Format("Branch %d, %d", m_AdoptablePolicies.GetElement(iI), iWeight);
			}
			else
			{

				PolicyTypes ePolicy = (PolicyTypes)(m_AdoptablePolicies.GetElement(iI) - iNumBranches);
				CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
				const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetType() : "Unknown";
				strTemp.Format("%s, %d", szPolicyType, iWeight);
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log chosen policy
void CvPolicyAI::LogPolicyChoice(PolicyTypes ePolicy)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
		const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetType() : "Unknown";
		strTemp.Format("CHOSEN, %s", szPolicyType);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Log chosen policy
void CvPolicyAI::LogBranchChoice(PolicyBranchTypes eBranch)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		strTemp.Format("CHOSEN, Branch %d", eBranch);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Logging function to write out info on Ideology choices
void CvPolicyAI::LogIdeologyChoice(CvString &decisionState, int iWeightFreedom, int iWeightAutocracy, int iWeightOrder)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;

		// Find the name of this civ
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		strTemp.Format("%s, Freedom: %d, Order: %d, Autocracy: %d", decisionState.c_str(), iWeightFreedom, iWeightOrder, iWeightAutocracy);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvPolicyAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PolicyAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PolicyAILog.csv";
	}

	return strLogName;
}
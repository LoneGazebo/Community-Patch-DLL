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

	AIGrandStrategyTypes eCultureGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	AIGrandStrategyTypes eCurrentGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();

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

				if(pPlayer->GetPlayerPolicies()->CanUnlockPolicyBranch(ePolicyBranch) && !pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
				{
					int iBranchWeight = 0;

					// Does this branch actually help us, based on game options?
					if(IsBranchEffectiveInGame(ePolicyBranch))
					{
						iBranchWeight += WeighBranch(ePolicyBranch);

						iBranchWeight *= (100 - m_iPolicyWeightPercentDropNewBranch);
						iBranchWeight /= 100;
						if(eCurrentGrandStrategy == eCultureGrandStrategy)
						{
							iBranchWeight /= 3;
						}
					}

					m_AdoptablePolicies.push_back(iBranchLoop, iBranchWeight);
				}
			}
		}
	}

	m_AdoptablePolicies.SortItems();
	LogPossiblePolicies();

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

	// First consideration is our victory type
	int iConquestPriority = max(0, pPlayer->GetGrandStrategyAI()->GetConquestPriority());
	int iDiploPriority = max(0, pPlayer->GetGrandStrategyAI()->GetUnitedNationsPriority());
	int iTechPriority = max(0, pPlayer->GetGrandStrategyAI()->GetSpaceshipPriority());
	int iCulturePriority = max(0, pPlayer->GetGrandStrategyAI()->GetCulturePriority());

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
	iOrderPriority += PolicyHelpers::GetNumFreePolicies(eOrderBranch) * GC.getIDEOLOGY_SCORE_PER_FREE_TENET();;

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
					iAutocracyPriority += GC.getIDEOLOGY_SCORE_HOSTILE();;
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

	// -- Happiness we'd lose through Public Opinion
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eFreedomBranch));
	iFreedomPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eAutocracyBranch));
	iAutocracyPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 100 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eOrderBranch));
	iOrderPriority += iHappinessDelta * iHappinessModifier;

	stage = "After Public Opinion Happiness";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Small random add-on
	iFreedomPriority += GC.getGame().getJonRandNum(10, "Freedom random priority bump");
	iAutocracyPriority += GC.getGame().getJonRandNum(10, "Autocracy random priority bump");
	iOrderPriority += GC.getGame().getJonRandNum(10, "Order random priority bump");

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
}

/// Should the AI look at switching ideology branches?
void CvPolicyAI::DoConsiderIdeologySwitch(CvPlayer* pPlayer)
{
	// Gather basic Ideology info
	int iCurrentHappiness = pPlayer->GetExcessHappiness();
	int iPublicOpinionUnhappiness = pPlayer->GetCulture()->GetPublicOpinionUnhappiness();
	PolicyBranchTypes ePreferredIdeology = pPlayer->GetCulture()->GetPublicOpinionPreferredIdeology();
	PolicyBranchTypes eCurrentIdeology = pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
	
	// Possible enough that we need to look at this in detail?
	if (iCurrentHappiness <= GC.getSUPER_UNHAPPY_THRESHOLD() && iPublicOpinionUnhappiness >= 10)
	{
		// How much Happiness could we gain from a switch?
		int iHappinessCurrentIdeology = GetBranchBuildingHappiness(pPlayer, eCurrentIdeology);
		int iHappinessPreferredIdeology = GetBranchBuildingHappiness(pPlayer, ePreferredIdeology);

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

		int iTotalHappinessImprovement = iPublicOpinionUnhappiness + iHappinessPreferredIdeology - iHappinessCurrentIdeology;
		if (iTotalHappinessImprovement >= 10)
		{
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

			// Cleared all obstacles -- REVOLUTION!
			pPlayer->SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
			pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);	

			if (ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM() && eCurrentIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				if (GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
				}
			}
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
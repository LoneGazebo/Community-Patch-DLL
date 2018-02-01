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
#include "CvTypes.h"
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
	if (pPlayer->isMinorCiv())
		return 0;

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
			int iWeight = WeighPolicy(pPlayer, (PolicyTypes)iPolicyLoop);

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
#if defined(MOD_BALANCE_CORE)
	bool bNeedToFinish = false;
	for (int iBranchLoop2 = 0; iBranchLoop2 < GC.getNumPolicyBranchInfos(); iBranchLoop2++)
	{
		const PolicyBranchTypes ePolicyBranch2 = static_cast<PolicyBranchTypes>(iBranchLoop2);
		CvPolicyBranchEntry* pkPolicyBranchInfo2 = GC.getPolicyBranchInfo(ePolicyBranch2);
		//Do we already have a different policy branch unlocked?
		if (pkPolicyBranchInfo2 && pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch2))
		{
			//Have we not finished it yet? If so, let's not open a new one.
			if (!pPlayer->GetPlayerPolicies()->HasPolicy((PolicyTypes)pkPolicyBranchInfo2->GetFreeFinishingPolicy()))
			{
				bNeedToFinish = true;
				break;
			}
		}
	}
#endif
	// Loop though the branches adding each as another possibility
	if (!bMustChooseTenet && !bNeedToFinish)
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
						iBranchWeight += WeighBranch(pPlayer, ePolicyBranch);

						iBranchWeight *= (100 - m_iPolicyWeightPercentDropNewBranch);
						iBranchWeight /= 100;
					}

					//Deemphasize older branches
					if (pkPolicyBranchInfo->GetEraPrereq() <= pPlayer->GetCurrentEra())
					{
						int iDivisor = pPlayer->GetCurrentEra() - max(0, pkPolicyBranchInfo->GetEraPrereq());
						iBranchWeight /= max(1, iDivisor);
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
	else if (m_AdoptablePolicies.size() > 0)
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
	if (MOD_DIPLOMACY_CIV4_FEATURES)
	{
		if (GET_TEAM(pPlayer->getTeam()).IsVassalOfSomeone())
		{
			TeamTypes eMasterTeam = GET_TEAM(pPlayer->getTeam()).GetMaster();
			if (eMasterTeam != NO_TEAM)
			{
				// Loop through all players to see if they're on our team
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eMaster = (PlayerTypes)iPlayerLoop;

					// Assumes one player per team for master
					if (GET_PLAYER(eMaster).getTeam() == GET_TEAM(eMasterTeam).GetID())
					{
						if (GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree() != NO_POLICY_BRANCH_TYPE)
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
	// == Grand Strategy ==
	int iDiploInterest = 0;
	int iConquestInterest = 0;
	int iScienceInterest = 0;
	int iCultureInterest = 0;

	//Grand Strategy Considerations - if valid, it doubles our initial weighting.
	// == Grand Strategy ==
	int iGrandStrategiesLoop;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy;
	CvString strGrandStrategyName;

	// Loop through all GrandStrategies and get priority. Since these are usually 100+, we will divide by 10 later
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GC.GetGameAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes)iGrandStrategiesLoop;
		pGrandStrategy = GC.GetGameAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
		strGrandStrategyName = (CvString)pGrandStrategy->GetType();

		if (strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
		{
			iConquestInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
		{
			iCultureInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
		{
			iDiploInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
		{
			iScienceInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
	}

	// First consideration is our victory type
	int iConquestPriority = iConquestInterest;
	int iDiploPriority = iDiploInterest;
	int iTechPriority = iScienceInterest;
	int iCulturePriority = iCultureInterest;

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
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eLoopPlayer);
			PolicyBranchTypes eOtherPlayerIdeology;
			eOtherPlayerIdeology = kOtherPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

			if (pPlayer->GetDiplomacyAI()->GetBiggestCompetitor() == eLoopPlayer)
			{
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
			}
			else if (pPlayer->GetDiplomacyAI()->GetMostValuableDefensivePact(false) == eLoopPlayer || pPlayer->GetDiplomacyAI()->GetMostValuableDoF(false) == eLoopPlayer)
			{
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
			}

			switch (pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ true))
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
	// Loop through adding the adoptable policies
	for (int iPolicyBranchLoop = 0; iPolicyBranchLoop < GC.getNumPolicyBranchInfos(); iPolicyBranchLoop++)
	{
		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo((PolicyBranchTypes)iPolicyBranchLoop);
		if (pkPolicyBranchInfo && pkPolicyBranchInfo->IsPurchaseByLevel())
		{
			int iWeight = WeighBranch(pPlayer, (PolicyBranchTypes)iPolicyBranchLoop) / 25;
			if ((PolicyBranchTypes)iPolicyBranchLoop == eFreedomBranch)
				iFreedomPriority += iWeight;
			else if ((PolicyBranchTypes)iPolicyBranchLoop == eAutocracyBranch)
				iAutocracyPriority += iWeight;
			else if ((PolicyBranchTypes)iPolicyBranchLoop == eOrderBranch)
				iOrderPriority += iWeight;
		}
	}
	stage = "After Tenet Weights and Flavors";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);
#endif
	//This was a dumb reason to weigh an ideology branch.

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
	iHappinessDelta = max (0, 250 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eFreedomBranch));
	iFreedomPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 250 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eAutocracyBranch));
	iAutocracyPriority += iHappinessDelta * iHappinessModifier;
	iHappinessDelta = max (0, 250 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eOrderBranch));
	iOrderPriority += iHappinessDelta * iHappinessModifier;

	stage = "After Public Opinion Happiness";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);
#if defined(MOD_BALANCE_CORE)
	}
#endif

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
	CvPlayerTraits* pPlayerTraits = pPlayer->GetPlayerTraits();

	int iPolicyGEorGM = pPlayerTraits->GetPolicyGEorGM();
	if(iPolicyGEorGM > 0)
	{
		CvCity* pLoopCity;
		CvCity* pCapital = pPlayer->getCapitalCity(); // JJ: Define capital
		int iLoop;
			int iEra = pPlayer->GetCurrentEra(); // JJ: Changed era scaling to match rest of VP
			if(iEra < 1)
			{
				iEra = 1;
			}
		int iValue = iPolicyGEorGM * iEra; // JJ: Changed formula
		iValue *= GC.getGame().getGameSpeedInfo().getTrainPercent(); // JJ: Game speed mod (note that TrainPercent is a percentage value, will need to divide by 100)
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
			CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eBestSpecialist);
			if(pkSpecialistInfo)
			{
				int iGPThreshold = pCapital->GetCityCitizens()->GetSpecialistUpgradeThreshold((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass());
				iGPThreshold *= 100;
				//Get % of threshold for test.
				iGPThreshold *= iPolicyGEorGM;
				iGPThreshold /= 100;
				int iGPThresholdString = iGPThreshold / 100;
				
				for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
				{
					if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_ENGINEER"))
					{
						pLoopCity->changeProduction((iValue * 2) / 100); // JJ: Production yield is 2x of science. Dividing by 100 here to minimise rounding error.
					}
					else if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_MERCHANT"))
					{
						pPlayer->GetTreasury()->ChangeGold((iValue * 4) / 100); // JJ: Gold yield is 4x of science, 2x of production. Dividing by 100 here to minimise rounding error.
					}
					else if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_SCIENTIST"))
					{
						TechTypes eCurrentTech = pPlayer->GetPlayerTechs()->GetCurrentResearch();
						if(eCurrentTech == NO_TECH)
						{
							pPlayer->changeOverflowResearch(iValue / 100); // JJ: Dividing by 100 here to minimise rounding error.
							if(pPlayer->getOverflowResearch() <= 0)
							{
								pPlayer->setOverflowResearch(0);
							}
						}
						else
						{
							GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, (iValue / 100), pPlayer->GetID()); // JJ: Dividing by 100 here to minimise rounding error.
							if(GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->GetResearchProgress(eCurrentTech) <= 0)
							{
								GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->SetResearchProgress(eCurrentTech, 0, pPlayer->GetID());
							}
						}
					}
				//CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eBestSpecialist);
				// JJ: Moved outside of for loop
					//int iGPThreshold = pLoopCity->GetCityCitizens()->GetSpecialistUpgradeThreshold((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass());
					//iGPThreshold *= 100;
					////Get % of threshold for test.
					//iGPThreshold *= iPolicyGEorGM;
					//iGPThreshold /= 100;
				
					pLoopCity->GetCityCitizens()->ChangeSpecialistGreatPersonProgressTimes100(eBestSpecialist, iGPThreshold, true);
					if(pPlayer->GetID() == GC.getGame().getActivePlayer())
					{
						if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_ENGINEER"))
						{
							char text[256] = {0};
							float fDelay = 0.5f;
							sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GREAT_ENGINEER]", iGPThresholdString);
							DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text, fDelay);
							char text2[256] = {0};
							sprintf_s(text2, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PRODUCTION]", (iValue*2/100));
							DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text2, fDelay+fDelay);
						}
						else if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_MERCHANT"))
						{
							char text[256] = {0};
							float fDelay = 0.5f;
							sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GREAT_MERCHANT]", iGPThresholdString);
							DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text, fDelay);
								char text2[256] = {0};
							sprintf_s(text2, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GOLD]", (iValue*4/100));
							DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text2, fDelay+fDelay);
						}
						else if(eBestSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_SCIENTIST"))
						{
							char text[256] = {0};
							float fDelay = 0.5f;
							sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GREAT_SCIENTIST]", iGPThresholdString);
							DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text, fDelay);
							char text2[256] = {0};
							sprintf_s(text2, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_RESEARCH]", (iValue/100));
							DLLUI->AddPopupText(pLoopCity->getX(),pLoopCity->getY(), text2, fDelay+fDelay);
						}
					}
				} //end of for loop
				if(pPlayer->GetID() == GC.getGame().getActivePlayer()) // JJ: Moved notification outside of for loop as it was flooding the screen
				{
					CvNotifications* pNotification = pPlayer->GetNotifications();
					if(pNotification)
					{
						CvString strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS", iGPThresholdString);
						CvString strSummary;
						// Class specific specialist message
						if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
						{
							strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS_MERCHANT", iGPThresholdString, (iValue*4/100));
						}
						else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
						{
							strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS_ENGINEER", iGPThresholdString, (iValue*2/100));
						}
						else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
						{
							strMessage = GetLocalizedText("TXT_KEY_POLICY_ADOPT_GP_BONUS_SCIENTIST", iGPThresholdString, (iValue/100));
						}
						strSummary = GetLocalizedText("TXT_KEY_POLICY_ADOPT_SUMMARY_GP_BONUS");
						pNotification->Add(NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER, strMessage, strSummary, -1, -1, -1);
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

#if !defined(NO_ACHIEVEMENTS)
			if (ePreferredIdeology == GC.getPOLICY_BRANCH_FREEDOM() && eCurrentIdeology == GC.getPOLICY_BRANCH_ORDER())
			{
				PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
				if (eMostPressure != NO_PLAYER && GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
				}
			}
#endif

			// Cleared all obstacles -- REVOLUTION!
			pPlayer->SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
			pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);	
#if defined(MOD_BALANCE_CORE)
			Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
			Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
			pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS(), -1);
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

Firaxis::Array< int, NUM_YIELD_TYPES > CvPolicyAI::WeightPolicyAttributes(CvPlayer* pPlayer, PolicyTypes ePolicy)
{
	Firaxis::Array< int, NUM_YIELD_TYPES > yield;
	for (unsigned int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		yield[i] = 0;
	}

	if (yield.size() <= 0)
		return yield;

	if (pPlayer->getCapitalCity() == NULL)
		return yield;

	CvPlayerTraits* pPlayerTraits = pPlayer->GetPlayerTraits();

	CvPolicyEntry* PolicyInfo = GC.getPolicyInfo(ePolicy);

	//Useful cache info

	int iNumCities = pPlayer->getNumCities();

	//Special rule for venice - focus on OCC!
	if (pPlayer->GetPlayerTraits()->IsNoAnnexing())
	{
		iNumCities = 1;
	}

	int iPopulation = pPlayer->getAveragePopulation();
	int iNumWonders = pPlayer->GetNumWonders();
	

	if (PolicyInfo->GetPolicyCostModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPolicyCostModifier() * -4 * iNumCities;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPolicyCostModifier() * -2 * iNumCities;
		}
	}
	if (PolicyInfo->GetCulturePerCity() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerCity() * 5 * iNumCities;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerCity() * 2 * iNumCities;
		}
	}
	if (PolicyInfo->GetCulturePerWonder() != 0)
	{
		yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerWonder() * pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() + iNumWonders;
	}
	if (PolicyInfo->GetCultureWonderMultiplier() != 0)
	{
		yield[YIELD_CULTURE] += PolicyInfo->GetCultureWonderMultiplier() * pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() + iNumWonders;
	}
	if (PolicyInfo->GetCulturePerTechResearched() != 0)
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerTechResearched() * 10;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerTechResearched() * 5;
		}
	}
	if (PolicyInfo->GetCultureImprovementChange() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureImprovementChange() * 4;
		}
		else
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureImprovementChange() * 2;
	}
	if (PolicyInfo->GetCultureFromKills() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureFromKills() * 2;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureFromKills();
		}
	}
	if (PolicyInfo->GetCultureFromBarbarianKills() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureFromBarbarianKills() * 2;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureFromBarbarianKills();
		}
	}

	if (PolicyInfo->GetGoldFromKills() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetGoldFromKills() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetGoldFromKills();
		}
	}
	if (PolicyInfo->GetEmbarkedExtraMoves() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetEmbarkedExtraMoves() * 150;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetEmbarkedExtraMoves() * 50;
		}
	}
	if (PolicyInfo->GetAttackBonusTurns() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetAttackBonusTurns() * PolicyInfo->GetAttackBonusTurns();
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetAttackBonusTurns() * (PolicyInfo->GetAttackBonusTurns() / 2);
		}
	}
	if (PolicyInfo->GetGoldenAgeTurns() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTurns() * pPlayer->GetNumGoldenAges() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTurns() * pPlayer->GetNumGoldenAges() * 2;
		}
	}
	if (PolicyInfo->GetGoldenAgeMeterMod() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeMeterMod() * 2 * pPlayer->GetNumGoldenAges();
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeMeterMod() * pPlayer->GetNumGoldenAges();
		}
	}
	if (PolicyInfo->GetGoldenAgeDurationMod() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeDurationMod() * 4 * pPlayer->GetNumGoldenAges();
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeDurationMod() * 2 * pPlayer->GetNumGoldenAges();
		}
	}

	if (PolicyInfo->GetNumFreeTechs() != 0)
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetNumFreeTechs() * 250;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetNumFreeTechs() * 125;
		}
	}
	if (PolicyInfo->GetNumFreePolicies() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumFreePolicies() * 250;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumFreePolicies() * 125;
		}
	}
	if (PolicyInfo->GetNumFreeGreatPeople() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumFreeGreatPeople() * 300;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumFreeGreatPeople() * 150;
		}
	}
	if (PolicyInfo->GetStrategicResourceMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetStrategicResourceMod() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetStrategicResourceMod() * 2;
		}
	}
	if (PolicyInfo->GetWonderProductionModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetWonderProductionModifier() * (4 + iNumWonders);
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetWonderProductionModifier() * (2 + iNumWonders);
		}
	}
	if (PolicyInfo->GetBuildingProductionModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetBuildingProductionModifier() * 5;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetBuildingProductionModifier() * 2;
		}
	}
	if (PolicyInfo->GetGreatPeopleRateModifier() != 0)
	{
		if (pPlayerTraits->IsTourism() || pPlayerTraits->IsSmaller())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetGreatPeopleRateModifier() * 5;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetGreatPeopleRateModifier();
		}
	}
	if (PolicyInfo->GetGreatGeneralRateModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGreatGeneralRateModifier() * 3 + pPlayer->getGreatGeneralsCreated(true);
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGreatGeneralRateModifier() * 1 + pPlayer->getGreatGeneralsCreated(true);
		}
	}
	if (PolicyInfo->GetGreatAdmiralRateModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGreatAdmiralRateModifier() * 3 + pPlayer->getGreatAdmiralsCreated(true);
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGreatAdmiralRateModifier() * 1 + pPlayer->getGreatAdmiralsCreated(true);
		}
	}
	if (PolicyInfo->GetGreatWriterRateModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatWriterRateModifier() * 3 + pPlayer->getGreatWritersCreated(true);
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatWriterRateModifier() * 1 + pPlayer->getGreatWritersCreated(true);
		}
	}
	if (PolicyInfo->GetGreatArtistRateModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatArtistRateModifier() * 3 + pPlayer->getGreatArtistsCreated(true);
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatArtistRateModifier() * 1 + pPlayer->getGreatArtistsCreated(true);
		}
	}
	if (PolicyInfo->GetGreatMusicianRateModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatMusicianRateModifier() * 3 + pPlayer->getGreatMusiciansCreated(true);
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatMusicianRateModifier() * 1 + pPlayer->getGreatMusiciansCreated(true);
		}
	}
	if (PolicyInfo->GetGreatMerchantRateModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetGreatMerchantRateModifier() * 3 + pPlayer->getGreatMerchantsCreated(true);
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetGreatMerchantRateModifier() * 1 + pPlayer->getGreatMerchantsCreated(true);
		}
	}
	if (PolicyInfo->GetGreatScientistRateModifier() != 0)
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetGreatScientistRateModifier() * 3 + pPlayer->getGreatScientistsCreated(true);
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetGreatScientistRateModifier() * 1 + pPlayer->getGreatScientistsCreated(true);
		}
	}
	if (PolicyInfo->GetGreatDiplomatRateModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetGreatDiplomatRateModifier() * 3 + pPlayer->getGreatDiplomatsCreated(true);
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetGreatDiplomatRateModifier() * 1 + pPlayer->getGreatDiplomatsCreated(true);
		}
	}
	if (PolicyInfo->GetDomesticGreatGeneralRateModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetDomesticGreatGeneralRateModifier() * 2 + pPlayer->getGreatGeneralsCreated(true);
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetDomesticGreatGeneralRateModifier() * 1 + pPlayer->getGreatGeneralsCreated(true);
		}
	}
	if (PolicyInfo->GetExtraHappiness() != 0)
	{
		yield[YIELD_FOOD] += PolicyInfo->GetExtraHappiness() * 50;
	}
	if (PolicyInfo->GetExtraHappinessPerCity() != 0)
	{
		yield[YIELD_FOOD] += PolicyInfo->GetExtraHappinessPerCity() * 25;
	}
	if (PolicyInfo->GetExtraNaturalWonderHappiness() != 0)
	{
		yield[YIELD_FOOD] += PolicyInfo->GetExtraNaturalWonderHappiness() * 25;
	}
	if (PolicyInfo->GetUnhappinessMod() != 0)
	{
		yield[YIELD_FOOD] += PolicyInfo->GetUnhappinessMod() * 10;
	}
	if (PolicyInfo->GetCityCountUnhappinessMod() != 0)
	{
		yield[YIELD_FOOD] += PolicyInfo->GetCityCountUnhappinessMod() * 10;
	}
	if (PolicyInfo->GetOccupiedPopulationUnhappinessMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetOccupiedPopulationUnhappinessMod() * 5;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetOccupiedPopulationUnhappinessMod() * 2;
		}
	}
	if (PolicyInfo->GetCapitalUnhappinessMod() != 0)
	{
		yield[YIELD_FOOD] += PolicyInfo->GetCapitalUnhappinessMod() * 5;
	}
	if (PolicyInfo->GetFreeExperience() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetFreeExperience() * 15;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetFreeExperience() * 2;
		}
	}
	if (PolicyInfo->GetExpModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExpModifier() * 15;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExpModifier() * 2;
		}
	}
	if (PolicyInfo->GetWorkerSpeedModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetWorkerSpeedModifier() * 3;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetWorkerSpeedModifier();
		}
	}
	if (PolicyInfo->GetBaseFreeUnits() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBaseFreeUnits() * 10;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBaseFreeUnits() * 5;
		}
	}
	if (PolicyInfo->GetBaseFreeMilitaryUnits() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBaseFreeMilitaryUnits() * 10;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBaseFreeMilitaryUnits() * 2;
		}
	}
	if (PolicyInfo->GetFreeUnitsPopulationPercent() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetFreeUnitsPopulationPercent() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetFreeUnitsPopulationPercent() * 2;
		}
	}
	if (PolicyInfo->GetFreeMilitaryUnitsPopulationPercent() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetFreeMilitaryUnitsPopulationPercent() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetFreeMilitaryUnitsPopulationPercent() * 2;
		}
	}
	if (PolicyInfo->GetHappinessPerGarrisonedUnit() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerGarrisonedUnit() * 10 * iNumCities;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerGarrisonedUnit() * 5 * iNumCities;
		}
	}
	if (PolicyInfo->GetCulturePerGarrisonedUnit() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerGarrisonedUnit() * 10 * iNumCities;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetCulturePerGarrisonedUnit() * 5 * iNumCities;
		}
	}
	if (PolicyInfo->GetHappinessPerTradeRoute() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerTradeRoute() * 10 * iNumCities;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerTradeRoute() * 5 * iNumCities;
		}
	}
	if (PolicyInfo->GetHappinessPerXPopulation() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += (iPopulation * 200) / PolicyInfo->GetHappinessPerXPopulation();
		}
		else
		{
			yield[YIELD_FOOD] += (iPopulation * 100) / PolicyInfo->GetHappinessPerXPopulation();
		}
	}
	if (PolicyInfo->GetHappinessPerXPopulationGlobal() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += (iPopulation * 200) / PolicyInfo->GetHappinessPerXPopulationGlobal();
		}
		else
		{
			yield[YIELD_FOOD] += (iPopulation * 100) / PolicyInfo->GetHappinessPerXPopulationGlobal();
		}
	}
	if (PolicyInfo->IsCorporationOfficesAsFranchises())
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_GOLD] += 30 * iNumCities;
		}
		else
		{
			yield[YIELD_GOLD] += 15 * iNumCities;
		}
	}
	if (PolicyInfo->IsCorporationFreeFranchiseAbovePopular())
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += 500;
		}
		else
		{
			yield[YIELD_TOURISM] += 250;
		}
	}
	if (PolicyInfo->IsCorporationRandomForeignFranchise())
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_GOLD] += 500;
		}
		else
		{
			yield[YIELD_GOLD] += 200;
		}
	}
	if (PolicyInfo->GetAdditionalNumFranchisesMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetAdditionalNumFranchisesMod() * 10;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetAdditionalNumFranchisesMod() * 5;
		}
	}
	if (PolicyInfo->IsUpgradeCSTerritory())
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 150;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 50;
		}
	}

	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	if (PolicyInfo->GetArchaeologicalDigTourism() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetArchaeologicalDigTourism() * 5 * pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetArchaeologicalDigTourism() * pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		}
	}
	if (PolicyInfo->GetGoldenAgeTourism() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTourism() * 25 * pPlayer->GetNumGoldenAges();
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTourism() * 5 * pPlayer->GetNumGoldenAges();
		}
	}
	if (PolicyInfo->GetExtraCultureandScienceTradeRoutes() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 50;
			yield[YIELD_CULTURE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 50;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 25;
			yield[YIELD_CULTURE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 25;
		}
	}
	if (PolicyInfo->GetTradeRouteLandDistanceModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetTradeRouteLandDistanceModifier() * 10;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetTradeRouteLandDistanceModifier() * 2;
		}
	}
	if (PolicyInfo->GetTradeRouteSeaDistanceModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetTradeRouteSeaDistanceModifier() * 10;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetTradeRouteSeaDistanceModifier() * 2;
		}
	}
	if (PolicyInfo->GetEspionageModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetEspionageModifier() * -3;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetEspionageModifier() * -2;
		}
	}
	if (PolicyInfo->GetXCSAlliesLowersPolicyNeedWonders() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetXCSAlliesLowersPolicyNeedWonders() * 25;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetXCSAlliesLowersPolicyNeedWonders() * 5;
		}
	}
	if (PolicyInfo->GetHappinessPerXPolicies() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerXPolicies() * 20;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerXPolicies() * 5;
		}
	}
	if (PolicyInfo->GetHappinessPerXGreatWorks() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerXGreatWorks() * 20;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerXGreatWorks() * 5;
		}
	}
	if (PolicyInfo->GetExtraHappinessPerLuxury() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetExtraHappinessPerLuxury() * 10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetExtraHappinessPerLuxury() * 2;
		}
	}
	if (PolicyInfo->GetUnhappinessFromUnitsMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetUnhappinessFromUnitsMod() * 10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetUnhappinessFromUnitsMod();
		}
	}
	if (PolicyInfo->GetPlotGoldCostMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetPlotGoldCostMod() * -5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetPlotGoldCostMod() * -2;
		}
	}
	if (PolicyInfo->GetPlotCultureCostModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPlotCultureCostModifier() * -3;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPlotCultureCostModifier() * -2;
		}
	}
	if (PolicyInfo->GetPlotCultureExponentModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPlotCultureExponentModifier() * -3;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPlotCultureExponentModifier() * -2;
		}
	}
	if (PolicyInfo->GetNumCitiesPolicyCostDiscount() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumCitiesPolicyCostDiscount() * 25;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumCitiesPolicyCostDiscount() * 5;
		}
	}
	if (PolicyInfo->GetGarrisonedCityRangeStrikeModifier() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGarrisonedCityRangeStrikeModifier();
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGarrisonedCityRangeStrikeModifier() / 2;
		}
	}
	if (PolicyInfo->GetUnitPurchaseCostModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitPurchaseCostModifier() * 4;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitPurchaseCostModifier() * 2;
		}
	}
	if (PolicyInfo->GetBuildingPurchaseCostModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetBuildingPurchaseCostModifier() * -5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetBuildingPurchaseCostModifier() * -2;
		}
	}
	if (PolicyInfo->GetCityConnectionTradeRouteGoldModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetCityConnectionTradeRouteGoldModifier() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetCityConnectionTradeRouteGoldModifier();
		}
	}
	if (PolicyInfo->GetTradeMissionGoldModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetTradeMissionGoldModifier() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetTradeMissionGoldModifier();
		}
	}
	if (PolicyInfo->GetFaithCostModifier() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += PolicyInfo->GetFaithCostModifier() * -3;
		}
		else
		{
			yield[YIELD_FAITH] += PolicyInfo->GetFaithCostModifier() * -1;
		}
	}
	if (PolicyInfo->GetStealTechSlowerModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetStealTechSlowerModifier() / 2;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetStealTechSlowerModifier() / 4;
		}
	}
	if (PolicyInfo->GetStealTechFasterModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetStealTechFasterModifier() / 2;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetStealTechFasterModifier() / 4;
		}
	}
	if (PolicyInfo->GetCatchSpiesModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetCatchSpiesModifier() * 2;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetCatchSpiesModifier() / 2;
		}
	}
	if (PolicyInfo->GetCityStrengthMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCityStrengthMod();
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCityStrengthMod() / 2;
		}
	}
	if (PolicyInfo->GetCityGrowthMod() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCityGrowthMod() * 15 * iNumCities;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCityGrowthMod() * 5 * iNumCities;
		}
	}
	if (PolicyInfo->GetCapitalGrowthMod() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCapitalGrowthMod() * 20;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCapitalGrowthMod() * 5;
		}
	}
	if (PolicyInfo->GetSettlerProductionModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetSettlerProductionModifier();
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetSettlerProductionModifier() / 2;
		}
	}
	if (PolicyInfo->GetCapitalSettlerProductionModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetCapitalSettlerProductionModifier();
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetCapitalSettlerProductionModifier() / 2;
		}
	}
	if (PolicyInfo->GetNewCityExtraPopulation() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNewCityExtraPopulation() * 50;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNewCityExtraPopulation() * 25;
		}
	}
	if (PolicyInfo->GetUnitGoldMaintenanceMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitGoldMaintenanceMod() * -15;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitGoldMaintenanceMod() * -5;
		}
	}
	if (PolicyInfo->GetUnitSupplyMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitSupplyMod() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitSupplyMod();
		}
	}
	if (PolicyInfo->GetExpModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExpModifier() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExpModifier();
		}
	}
	if (PolicyInfo->GetExpInBorderModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExpInBorderModifier() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExpInBorderModifier();
		}
	}
	if (PolicyInfo->GetMinorQuestFriendshipMod() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorQuestFriendshipMod() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorQuestFriendshipMod();
		}
	}
	if (PolicyInfo->GetMinorGoldFriendshipMod() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorGoldFriendshipMod() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorGoldFriendshipMod();
		}
	}
	if (PolicyInfo->GetMinorFriendshipMinimum() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorFriendshipMinimum() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorFriendshipMinimum();
		}
	}
	if (PolicyInfo->GetMinorFriendshipDecayMod() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorFriendshipDecayMod() * -2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorFriendshipDecayMod() * -1;
		}
	}
	if (PolicyInfo->GetCityStateUnitFrequencyModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCityStateUnitFrequencyModifier() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCityStateUnitFrequencyModifier();
		}
	}
	if (PolicyInfo->GetCommonFoeTourismModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetCommonFoeTourismModifier() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetCommonFoeTourismModifier();
		}
	}
	if (PolicyInfo->GetLessHappyTourismModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetLessHappyTourismModifier() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetLessHappyTourismModifier();
		}
	}
	if (PolicyInfo->GetSharedIdeologyTourismModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetSharedIdeologyTourismModifier() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetSharedIdeologyTourismModifier();
		}
	}
	if (PolicyInfo->GetLandTradeRouteGoldChange() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += (max(1, pPlayer->GetTrade()->GetNumberOfTradeRoutes()) * PolicyInfo->GetLandTradeRouteGoldChange()) / 50;
		}
		else
		{
			yield[YIELD_GOLD] += (max(1, pPlayer->GetTrade()->GetNumberOfTradeRoutes()) * PolicyInfo->GetLandTradeRouteGoldChange()) / 100;
		}
	}
	if (PolicyInfo->GetSeaTradeRouteGoldChange() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += (max(1, pPlayer->GetTrade()->GetNumberOfTradeRoutes()) * PolicyInfo->GetSeaTradeRouteGoldChange()) / 50;
		}
		else
		{
			yield[YIELD_GOLD] += (max(1, pPlayer->GetTrade()->GetNumberOfTradeRoutes()) * PolicyInfo->GetSeaTradeRouteGoldChange()) / 100;
		}
	}
	if (PolicyInfo->GetSharedIdeologyTradeGoldChange() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetSharedIdeologyTradeGoldChange() * 25;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetSharedIdeologyTradeGoldChange() * 5;
		}
	}
	if (PolicyInfo->GetRiggingElectionModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetRiggingElectionModifier() * 5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetRiggingElectionModifier();
		}
	}
	if (PolicyInfo->GetMilitaryUnitGiftExtraInfluence() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMilitaryUnitGiftExtraInfluence() * 5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMilitaryUnitGiftExtraInfluence();
		}
	}
	if (PolicyInfo->GetProtectedMinorPerTurnInfluence() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetProtectedMinorPerTurnInfluence() * 5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetProtectedMinorPerTurnInfluence();
		}
	}
	if (PolicyInfo->GetAfraidMinorPerTurnInfluence() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetAfraidMinorPerTurnInfluence() * 5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetAfraidMinorPerTurnInfluence();
		}
	}
	if (PolicyInfo->GetMinorBullyScoreModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorBullyScoreModifier() * 5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMinorBullyScoreModifier() / 2;
		}
	}
	if (PolicyInfo->GetThemingBonusMultiplier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetThemingBonusMultiplier() * (pPlayer->GetCulture()->GetTotalThemingBonuses() / 2);
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetThemingBonusMultiplier() * (pPlayer->GetCulture()->GetTotalThemingBonuses() / 5);
		}
	}
	if (PolicyInfo->GetInternalTradeRouteYieldModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetInternalTradeRouteYieldModifier() / 5;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetInternalTradeRouteYieldModifier() / 10;
		}
	}
	if (PolicyInfo->GetPositiveWarScoreTourismMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetPositiveWarScoreTourismMod() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetPositiveWarScoreTourismMod() / 2;
		}
	}
	if (PolicyInfo->GetInternalTradeRouteYieldModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetInternalTradeRouteYieldModifierCapital() * 2;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetInternalTradeRouteYieldModifierCapital() / 2;
		}
	}
	if (PolicyInfo->GetTradeRouteYieldModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetTradeRouteYieldModifierCapital() * 2;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetTradeRouteYieldModifierCapital() / 2;
		}
	}
	if (PolicyInfo->GetNewCityFreeBuilding() != NO_BUILDINGCLASS)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(PolicyInfo->GetNewCityFreeBuilding());
		if (pkBuildingClassInfo)
		{
			const BuildingTypes eBuilding = ((BuildingTypes)(pPlayer->getCivilizationInfo().getCivilizationBuildings(PolicyInfo->GetNewCityFreeBuilding())));
			if (NO_BUILDING != eBuilding)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (pkBuildingInfo)
				{
					int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 50, 10, 10, 10, false, true, true);
					if (iValue > 0)
					{
						iValue /= 2;

						if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
							iValue /= 2;

						iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

						yield[YIELD_PRODUCTION] += max(0, iValue);
					}
				}
			}
		}
	}
	if (PolicyInfo->IsNoCSDecayAtWar())
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 250;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 50;
		}
	}
	if (PolicyInfo->GetBullyGlobalCSReduction())
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 250;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 50;
		}
	}
	if (PolicyInfo->CanBullyFriendlyCS())
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 500;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 50;
		}
	}
	if (PolicyInfo->IsVassalsNoRebel() && GET_TEAM(pPlayer->getTeam()).GetNumVassals() > 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 100 * GET_TEAM(pPlayer->getTeam()).GetNumVassals();
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 25 * GET_TEAM(pPlayer->getTeam()).GetNumVassals();
		}
	}
	if (PolicyInfo->GetVassalCSBonusModifier() && GET_TEAM(pPlayer->getTeam()).GetNumVassals() > 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += 50;
		}
		else
		{
			yield[YIELD_GOLD] += 10;
		}
	}
	if (PolicyInfo->GetVassalCSBonusModifier() && GET_TEAM(pPlayer->getTeam()).GetNumVassals() > 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += 25;
		}
		else
		{
			yield[YIELD_GOLD] += 10;
		}
	}
	if (PolicyInfo->GetSharedReligionTourismModifier() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetSharedReligionTourismModifier() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetSharedReligionTourismModifier();
		}
	}
	if (PolicyInfo->GetTradeRouteTourismModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetTradeRouteTourismModifier() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetTradeRouteTourismModifier();
		}
	}
	if (PolicyInfo->GetOpenBordersTourismModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetOpenBordersTourismModifier() * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetOpenBordersTourismModifier();
		}
	}
	if (PolicyInfo->IsMinorGreatPeopleAllies())
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_TOURISM] += 250;
		}
		else
		{
			yield[YIELD_TOURISM] += 50;
		}
	}
	if (PolicyInfo->IsMinorScienceAllies())
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_SCIENCE] += 250;
		}
		else
		{
			yield[YIELD_SCIENCE] += 50;
		}
	}
	if (PolicyInfo->IsMinorResourceBonus())
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_FOOD] += 250;
		}
		else
		{
			yield[YIELD_FOOD] += 50;
		}
	}
	if (PolicyInfo->GetHappinessToCulture() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetHappinessToCulture() * 2;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetHappinessToCulture();
		}
	}
	if (PolicyInfo->GetHappinessToScience() != 0)
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetHappinessToScience() * 3;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetHappinessToScience();
		}
	}
	if (PolicyInfo->GetNumCitiesFreeCultureBuilding() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_CULTURE] += 50;
		}
		else
		{
			yield[YIELD_SCIENCE] += 25;
		}
	}
	if (PolicyInfo->GetNumCitiesFreeFoodBuilding() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += 50;
		}
		else
		{
			yield[YIELD_FOOD] += 25;
		}
	}
	if (PolicyInfo->IsHalfSpecialistUnhappiness() != 0)
	{
		if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += 250;
		}
		else
		{
			yield[YIELD_FOOD] += 50;
		}
	}
	if (PolicyInfo->IsHalfSpecialistFood() != 0)
	{
		if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += 250;
		}
		else
		{
			yield[YIELD_FOOD] += 50;
		}
	}
	if (PolicyInfo->IsHalfSpecialistFoodCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += 250;
		}
		else
		{
			yield[YIELD_FOOD] += 50;
		}
	}

	if (PolicyInfo->GetDefenseBoost() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 20 * iNumCities;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 5 * iNumCities;
		}
	}

	if (PolicyInfo->GetStealGWSlowerModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetStealGWSlowerModifier() * 2;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetStealGWSlowerModifier();
		}
	}
	if (PolicyInfo->GetStealGWFasterModifier() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetStealGWFasterModifier() * 2;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetStealGWFasterModifier();
		}
	}
	if (PolicyInfo->GetEventTourism() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetEventTourism() * 50;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetEventTourism() * 25;
		}
	}
	if (PolicyInfo->GetEventTourismCS() != 0)
	{
		if (pPlayerTraits->IsDiplomat() || pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetEventTourismCS() * 50;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetEventTourismCS() * 25;
		}
	}
	if (PolicyInfo->GetMonopolyModFlat() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMonopolyModFlat() * 30;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMonopolyModFlat() * 15;
		}
	}
	if (PolicyInfo->GetMonopolyModPercent() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMonopolyModPercent() * 2;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMonopolyModPercent();
		}
	}
	if (PolicyInfo->GetCityStateCombatModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCityStateCombatModifier() * 2;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCityStateCombatModifier() / 2;
		}
	}
	if (PolicyInfo->GetGreatEngineerRateModifier() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetGreatEngineerRateModifier() * 2 + pPlayer->getGreatEngineersCreated(true);
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetGreatEngineerRateModifier() * 1 + pPlayer->getGreatEngineersCreated(true);
		}
	}
	if (PolicyInfo->GetUnitUpgradeCostMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitUpgradeCostMod() * -5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitUpgradeCostMod() * -1;
		}
	}
	if (PolicyInfo->GetBarbarianCombatBonus() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBarbarianCombatBonus() * 3;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBarbarianCombatBonus();
		}
	}
	if (PolicyInfo->IsAlwaysSeeBarbCamps())
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 50;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 5;
		}
	}
	if (PolicyInfo->IsRevealAllCapitals())
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += 100;
		}
		else
		{
			yield[YIELD_GOLD] += 25;
		}
	}
	if (PolicyInfo->IsGarrisonFreeMaintenance())
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += 10 * iNumCities;
		}
		else
		{
			yield[YIELD_GOLD] += 5 * iNumCities;
		}
	}
	if (PolicyInfo->IsSecondReligionPantheon())
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += 100;
		}
		else
		{
			yield[YIELD_FAITH] += 50;
		}
	}
	if (PolicyInfo->IsAddReformationBelief())
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += 1000;
		}
		else
		{
			yield[YIELD_FAITH] += 50;
		}
	}
	if (PolicyInfo->IsEnablesSSPartHurry())
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += 1000;
		}
		else
		{
			yield[YIELD_SCIENCE] += 100;
		}
	}
	if (PolicyInfo->IsEnablesSSPartPurchase())
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += 1000;
		}
		else
		{
			yield[YIELD_SCIENCE] += 100;
		}
	}
	if (PolicyInfo->IsAbleToAnnexCityStates())
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += 500;
		}
		else
		{
			yield[YIELD_GOLD] += 50;
		}
	}
	if (PolicyInfo->HasFaithPurchaseUnitClasses())
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_GOLD] += 150;
		}
		else
		{
			yield[YIELD_GOLD] += 50;
		}
	}
	if (PolicyInfo->GetNoUnhappinessExpansion() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNoUnhappinessExpansion() * 10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNoUnhappinessExpansion() * 5;
		}
	}
	if (PolicyInfo->GetHappinessPerActiveTradeRoute() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerActiveTradeRoute() * 25;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerActiveTradeRoute() * 10;
		}
	}
	if (PolicyInfo->GetFreeBuildingOnConquest() != NO_BUILDING)
	{
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(PolicyInfo->GetFreeBuildingOnConquest());
		if (pkBuildingInfo)
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType());
			if (pkBuildingClassInfo)
			{
				int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(PolicyInfo->GetFreeBuildingOnConquest(), 50, 10, 10, 10, false, true, true);
				if (iValue > 0)
				{
					if (pPlayerTraits->IsWarmonger())
						iValue /= 2;
					else
						iValue /= 4;
						
					iValue -= pPlayer->getNumBuildings(PolicyInfo->GetFreeBuildingOnConquest()) * 10;

					yield[YIELD_PRODUCTION] += max(0, iValue);
				}
			}
		}
	}
	if (PolicyInfo->GetPovertyHappinessChangePolicy() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->getUnhappinessFromCityGold() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPovertyHappinessChangePolicy() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPovertyHappinessChangePolicy() * -2;
		}
	}
	if (PolicyInfo->GetDefenseHappinessChangePolicy() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->getUnhappinessFromCityDefense() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetDefenseHappinessChangePolicy() * -5;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetDefenseHappinessChangePolicy() * -2;
		}
	}
	if (PolicyInfo->GetUnculturedHappinessChangePolicy() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->getUnhappinessFromCityCulture() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetUnculturedHappinessChangePolicy() * -5;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetUnculturedHappinessChangePolicy() * -2;
		}
	}
	if (PolicyInfo->GetIlliteracyHappinessChangePolicy() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->getUnhappinessFromCityScience() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetIlliteracyHappinessChangePolicy() * -5;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetIlliteracyHappinessChangePolicy() * -2;
		}
	}
	if (PolicyInfo->GetMinorityHappinessChangePolicy() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->getUnhappinessFromCityMinority() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetMinorityHappinessChangePolicy() * -5;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetMinorityHappinessChangePolicy() * -2;
		}
	}
	if (PolicyInfo->GetPovertyHappinessChangePolicyCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPovertyHappinessChangePolicyCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPovertyHappinessChangePolicyCapital() * -2;
		}
	}
	if (PolicyInfo->GetDefenseHappinessChangePolicyCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetDefenseHappinessChangePolicyCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetDefenseHappinessChangePolicyCapital() * -2;
		}
	}
	if (PolicyInfo->GetUnculturedHappinessChangePolicyCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetUnculturedHappinessChangePolicyCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetUnculturedHappinessChangePolicyCapital() * -2;
		}
	}
	if (PolicyInfo->GetIlliteracyHappinessChangePolicyCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetIlliteracyHappinessChangePolicyCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetIlliteracyHappinessChangePolicyCapital() * -2;
		}
	}
	if (PolicyInfo->GetMinorityHappinessChangePolicyCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetMinorityHappinessChangePolicyCapital() *  -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetMinorityHappinessChangePolicyCapital() * -2;
		}
	}
	if (PolicyInfo->GetPuppetUnhappinessMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPuppetUnhappinessMod() * 3;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPuppetUnhappinessMod() * 2;
		}
	}
	if (PolicyInfo->GetNoUnhappfromXSpecialists() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNoUnhappfromXSpecialists() * 15;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNoUnhappfromXSpecialists() * 10;
		}
	}
	if (PolicyInfo->GetHappfromXSpecialists() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappfromXSpecialists() * 10 * max(1, (iPopulation / 5));
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappfromXSpecialists() * 2 * max(1, (iPopulation / 10));
		}
	}
	if (PolicyInfo->GetNoUnhappfromXSpecialistsCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNoUnhappfromXSpecialistsCapital() * 50;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetNoUnhappfromXSpecialistsCapital() * 25;
		}
	}

	if (PolicyInfo->GetSpecialistFoodChange() != 0)
	{
		if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism() || pPlayerTraits->IsNerd())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetSpecialistFoodChange() * -10 * max(1, (iPopulation / 5));
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetSpecialistFoodChange() * -4 * max(1, (iPopulation / 10));
		}
	}

	if (PolicyInfo->GetWarWearinessModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetWarWearinessModifier() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetWarWearinessModifier() * 2;
		}
	}
	if (PolicyInfo->GetWarScoreModifier() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetWarScoreModifier() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetWarScoreModifier() * 2;
		}
	}
	if (PolicyInfo->GetGreatGeneralExtraBonus() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGreatGeneralExtraBonus() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGreatGeneralExtraBonus();
		}
	}
	if (PolicyInfo->GetGarrisonsOccupiedUnhapppinessMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGarrisonsOccupiedUnhapppinessMod() * 10;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGarrisonsOccupiedUnhapppinessMod() * 2;
		}
	}
	if (PolicyInfo->GetTradeReligionModifier() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += PolicyInfo->GetTradeReligionModifier() * 3;
		}
		else
		{
			yield[YIELD_FAITH] += PolicyInfo->GetTradeReligionModifier();
		}
	}
	if (PolicyInfo->GetBestRangedUnitSpawnSettle() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBestRangedUnitSpawnSettle() * 10;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetBestRangedUnitSpawnSettle() * 5;
		}
	}
	if (PolicyInfo->GetFreePopulation() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetFreePopulation() * 50;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetFreePopulation() * 25;
		}
	}
	if (PolicyInfo->GetExtraMoves() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExtraMoves() * 100;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExtraMoves() * 25;
		}
	}
	if (PolicyInfo->GetMaxCorps() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMaxCorps() * 25;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMaxCorps() * 10;
		}
	}
	if (PolicyInfo->GetRazingSpeedBonus() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetRazingSpeedBonus() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetRazingSpeedBonus();
		}
	}
	if (PolicyInfo->GetExtraSupplyPerPopulation() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExtraSupplyPerPopulation() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExtraSupplyPerPopulation();
		}
	}
	if (PolicyInfo->GetInvestmentModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetInvestmentModifier() * -3;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetInvestmentModifier() * -2;
		}
	}
	if (PolicyInfo->GetIncreasedQuestInfluence() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetIncreasedQuestInfluence() * 3;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetIncreasedQuestInfluence();
		}
	}
	if (PolicyInfo->GetGreatScientistBeakerModifier() != 0)
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetGreatScientistBeakerModifier() * 4;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetGreatScientistBeakerModifier();
		}
	}
	if (PolicyInfo->GetGreatEngineerHurryModifier() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetGreatEngineerHurryModifier() * 4;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetGreatEngineerHurryModifier();
		}
	}
	if (PolicyInfo->GetTechCostXCitiesMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetTechCostXCitiesMod() * -25;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetTechCostXCitiesMod() * -10;
		}
	}

	if (PolicyInfo->GetTourismCostXCitiesMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetTourismCostXCitiesMod() * 10;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetTourismCostXCitiesMod() * 2;
		}
	}
	if (PolicyInfo->GetCitadelBoost() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCitadelBoost() * 250;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCitadelBoost() * 50;
		}
	}
	if (PolicyInfo->GetPuppetProdMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetPuppetProdMod() * 5;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetPuppetProdMod();
		}
	}
	if (PolicyInfo->GetOccupiedProdMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetOccupiedProdMod() * 5;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetOccupiedProdMod();
		}
	}
	if (PolicyInfo->GetInternalTradeGold() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetInternalTradeGold() * 10;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetInternalTradeGold();
		}
	}
	if (PolicyInfo->GetFreeWCVotes() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetFreeWCVotes() * 250;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetFreeWCVotes() * 50;
		}
	}
	if (PolicyInfo->GetInfluenceGPExpend() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetInfluenceGPExpend() * 75;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetInfluenceGPExpend() * 25;
		}
	}
	if (PolicyInfo->GetFreeTradeRoute() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetFreeTradeRoute() * 250;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetFreeTradeRoute() * 50;
		}
	}
	if (PolicyInfo->GetFreeSpy() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetFreeSpy() * 250;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetFreeSpy() * 50;
		}
	}
	if (PolicyInfo->GetReligionDistance() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += PolicyInfo->GetReligionDistance() * 10;
		}
		else
		{
			yield[YIELD_FAITH] += PolicyInfo->GetReligionDistance() * 5;
		}
	}
	if (PolicyInfo->GetPressureMod() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += PolicyInfo->GetPressureMod() * 3;
		}
		else
		{
			yield[YIELD_FAITH] += PolicyInfo->GetPressureMod();
		}
	}
	if (PolicyInfo->GetMissionInfluenceModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMissionInfluenceModifier() * 25;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMissionInfluenceModifier() * 10;
		}
	}

	if (PolicyInfo->IsNoPartisans())
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 150;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += 50;
		}
	}
	if (PolicyInfo->GetNoUnhappyIsolation())
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += 50;
		}
		else
		{
			yield[YIELD_FOOD] += 20;
		}
	}
	if (PolicyInfo->GetDoubleBorderGA())
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += 100;
		}
		else
		{
			yield[YIELD_CULTURE] += 50;
		}
	}
	if (PolicyInfo->IsCSResourcesForMonopolies())
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += 150;
		}
		else
		{
			yield[YIELD_GOLD] += 50;
		}
	}

	UnitCombatTypes eUnitCombat;
	for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		eUnitCombat = (UnitCombatTypes)iI;
		if (PolicyInfo->GetUnitCombatProductionModifiers(eUnitCombat) != 0)
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitCombatProductionModifiers(eUnitCombat) * 4;
			}
			else
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitCombatProductionModifiers(eUnitCombat);
			}
		}
		if (PolicyInfo->GetUnitCombatFreeExperiences(eUnitCombat)  != 0)
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitCombatFreeExperiences(eUnitCombat) * 10;
			}
			else
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitCombatFreeExperiences(eUnitCombat) * 2;
			}
		}
	}

	BuildingClassTypes eBuildingClass;
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		eBuildingClass = (BuildingClassTypes)iI;

		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if (pkBuildingClassInfo)
		{
			const BuildingTypes eBuilding = ((BuildingTypes)(pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));
			if (NO_BUILDING != eBuilding)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (pkBuildingInfo && pkBuildingInfo->GetPolicyType() == ePolicy)
				{
					int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 50, 10, 10, 10, false, true, true);
					if (iValue > 0)
					{
						iValue /= 2;

						if (pkBuildingInfo->GetFaithCost() != 0)
						{
							if (pPlayerTraits->IsReligious())
								iValue *= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							yield[YIELD_FAITH] += max(0, iValue);
						}
						else
						{
							if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
								iValue /= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							yield[YIELD_PRODUCTION] += max(0, iValue);
						}
					}
				}
			}
		}
		if (PolicyInfo->GetBuildingClassCultureChange(eBuildingClass) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[YIELD_CULTURE] += 2 * PolicyInfo->GetBuildingClassCultureChange(eBuildingClass) * iNumCities;
			}
			else
			{
				yield[YIELD_CULTURE] += PolicyInfo->GetBuildingClassCultureChange(eBuildingClass) * iNumCities;
			}
		}
		if (PolicyInfo->GetBuildingClassHappiness(eBuildingClass))
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[YIELD_FOOD] += 3 * PolicyInfo->GetBuildingClassHappiness(eBuildingClass) * iNumCities;
			}
			else
			{
				yield[YIELD_FOOD] += PolicyInfo->GetBuildingClassHappiness(eBuildingClass) * iNumCities;
			}
		}
		if (PolicyInfo->GetBuildingClassProductionModifier(eBuildingClass))
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[YIELD_PRODUCTION] += (PolicyInfo->GetBuildingClassProductionModifier(eBuildingClass) * iNumCities) / 15;
			}
			else
			{
				yield[YIELD_PRODUCTION] += (PolicyInfo->GetBuildingClassProductionModifier(eBuildingClass) * iNumCities) / 25;
			}
		}
		if (PolicyInfo->GetBuildingClassTourismModifier(eBuildingClass))
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[YIELD_TOURISM] += 4 * PolicyInfo->GetBuildingClassTourismModifier(eBuildingClass) * iNumCities;
			}
			else
			{
				yield[YIELD_TOURISM] += 2 * PolicyInfo->GetBuildingClassTourismModifier(eBuildingClass) * iNumCities;
			}
		}
		if (PolicyInfo->GetFreeChosenBuilding(eBuildingClass) != 0) 
		{
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if (pkBuildingClassInfo)
			{
				const BuildingTypes eBuilding = ((BuildingTypes)(pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));
				if (NO_BUILDING != eBuilding)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
					if (pkBuildingInfo)
					{
						int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 50, 10, 10, 10, false, true, true);
						if (iValue > 0)
						{
							iValue /= 2;

							if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
								iValue /= 2;
							else if (pkBuildingClassInfo->getMaxGlobalInstances() == 1)
								iValue /= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							yield[YIELD_PRODUCTION] += max(0, iValue);
						}
					}
				}
			}
		}
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;

			if (PolicyInfo->GetBuildingClassYieldModifiers(eBuildingClass, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += (PolicyInfo->GetBuildingClassYieldModifiers(eBuildingClass, eYield) * max(1, iNumCities / 2));
				}
				else
				{
					yield[eYield] += (PolicyInfo->GetBuildingClassYieldModifiers(eBuildingClass, eYield) * max(1, iNumCities/3));
				}
			}
			if (PolicyInfo->GetBuildingClassYieldChanges(eBuildingClass, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetBuildingClassYieldChanges(eBuildingClass, eYield) * 3 * iNumCities;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetBuildingClassYieldChanges(eBuildingClass, eYield) * iNumCities;
				}
			}
			if (PolicyInfo->GetReligionBuildingYieldMod(eBuildingClass, eYield) != 0)
			{
				if (pPlayerTraits->IsReligious())
				{
					yield[eYield] += PolicyInfo->GetReligionBuildingYieldMod(eBuildingClass, eYield) * 3 * iNumCities;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetReligionBuildingYieldMod(eBuildingClass, eYield) * iNumCities;
				}
			}
		}
	}

	UnitClassTypes eUnitClass;
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eUnitClass = (UnitClassTypes)iI;
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if (!pkUnitClassInfo)
			continue;
		
		const UnitTypes eUnit = (UnitTypes)pPlayer->getCivilizationInfo().getCivilizationUnits(eUnitClass);
		CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
		if (!pUnitEntry)
			continue;
		bool bCombat = pUnitEntry->GetCombat() > 0 || pUnitEntry->GetRangedCombat() > 0;
		if (PolicyInfo->GetUnitClassProductionModifiers(eUnitClass) != 0)
		{
			if (bCombat ? pPlayerTraits->IsWarmonger() : pPlayerTraits->IsExpansionist())
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitClassProductionModifiers(eUnitClass) * 2;
			}
			else
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetUnitClassProductionModifiers(eUnitClass);
			}
		}
	
		if (PolicyInfo->GetNumFreeUnitsByClass(eUnitClass) != 0)
		{
			if (bCombat ? pPlayerTraits->IsWarmonger() : pPlayerTraits->IsExpansionist())
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetNumFreeUnitsByClass(eUnitClass) * 15;
			}
			else
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetNumFreeUnitsByClass(eUnitClass) * 5;
			}
		}
		if (PolicyInfo->GetTourismByUnitClassCreated(eUnitClass) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[YIELD_TOURISM] += PolicyInfo->GetTourismByUnitClassCreated(eUnitClass) * 5;
			}
			else
			{
				yield[YIELD_TOURISM] += PolicyInfo->GetTourismByUnitClassCreated(eUnitClass);
			}
		}

		if (pPlayer->getCapitalCity() != NULL)
		{
			if (PolicyInfo->IsFaithPurchaseUnitClass(eUnitClass, GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA)) != 0)
			{
				CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
				if (pkUnitClassInfo)
				{
					const UnitTypes eUnit = (UnitTypes)pPlayer->getCivilizationInfo().getCivilizationUnits(eUnitClass);
					CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
					if (pUnitEntry)
					{
						int Value = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, NULL, 20, 10, 10, 10, true, true);
						if (pPlayerTraits->IsReligious())
						{
							Value *= 2;
						}
						yield[YIELD_FAITH] += Value;
					}
				}
			}
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
			if (pkUnitClassInfo)
			{
				const UnitTypes eUnit = (UnitTypes)pPlayer->getCivilizationInfo().getCivilizationUnits(eUnitClass);
				CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
				if (pUnitEntry && pUnitEntry->GetPolicyType() == ePolicy)
				{
					int Value = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, NULL, 30, 10, 10, 10, true, true);
					if (pPlayerTraits->IsWarmonger())
					{
						Value *= 2;
					}
					if (pUnitEntry->GetDomainType() == DOMAIN_LAND || pUnitEntry->GetDomainType() == DOMAIN_AIR)
						yield[YIELD_GREAT_GENERAL_POINTS] += Value;
					else
						yield[YIELD_GREAT_ADMIRAL_POINTS] += Value;
				}
			}
		}
	}

	ImprovementTypes eImprovement;
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		eImprovement = (ImprovementTypes)iI;

		int NumImprovements = pPlayer->CountAllImprovement(eImprovement);

		if (NumImprovements <= 0)
			continue;

		if (PolicyInfo->GetImprovementCultureChanges(eImprovement) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[YIELD_CULTURE] += PolicyInfo->GetImprovementCultureChanges(eImprovement) * NumImprovements;
			}
			else
			{
				yield[YIELD_CULTURE] += PolicyInfo->GetImprovementCultureChanges(eImprovement) * NumImprovements/2;
			}
		}

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;

			if (PolicyInfo->GetImprovementYieldChanges(eImprovement, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetImprovementYieldChanges(eImprovement, eYield) * NumImprovements * 5;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetImprovementYieldChanges(eImprovement, eYield) * NumImprovements * 2;
				}
			}
		}
	}

	HurryTypes eHurry;
	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		eHurry = (HurryTypes)iI;
		if (PolicyInfo->GetHurryModifier(eHurry) != 0)
		{
			if (pPlayerTraits->IsDiplomat())
			{
				yield[YIELD_GOLD] += PolicyInfo->GetHurryModifier(eHurry) * -2;
			}
			else
			{
				yield[YIELD_GOLD] += PolicyInfo->GetHurryModifier(eHurry) * -1;
			}
		}
	}

	ResourceTypes eResource;
	for (int iI = 0; iI < GC.getNumResourceInfos(); iI++)
	{
		eResource = (ResourceTypes)iI;
		if (PolicyInfo->GetResourceFromCSAlly(eResource) != 0)
		{
			if (pPlayerTraits->IsDiplomat())
			{
				yield[YIELD_GOLD] += PolicyInfo->GetResourceFromCSAlly(eResource) / 5;
			}
			else
			{
				yield[YIELD_GOLD] += PolicyInfo->GetResourceFromCSAlly(eResource) / 10;
			}
		}

		if (GC.getResourceInfo(eResource) != NULL && GC.getResourceInfo(eResource)->getPolicyReveal() == ePolicy)
		{
			yield[YIELD_GOLD] += 100;
		}

		int NumResources = pPlayer->CountAllResource(eResource);

		if (NumResources <= 0)
			continue;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetResourceYieldChanges(eResource, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetResourceYieldChanges(eResource, eYield) * 2 * NumResources;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetResourceYieldChanges(eResource, eYield) * NumResources;
				}
			}
		}
	}

	PlotTypes ePlot;
	for (int iI = 0; iI < GC.getNumPlotInfos(); iI++)
	{
		ePlot = (PlotTypes)iI;

		int NumPlot = pPlayer->CountAllPlotType(ePlot);

		if (NumPlot <= 0)
			continue;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetPlotYieldChanges(ePlot, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetPlotYieldChanges(ePlot, eYield) * 2 * NumPlot;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetPlotYieldChanges(ePlot, eYield) * NumPlot;
				}
			}
		}
	}

	FeatureTypes eFeature;
	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		eFeature = (FeatureTypes)iI;

		int NumFeature = pPlayer->CountAllFeature(eFeature);

		if (NumFeature <= 0)
			continue;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetFeatureYieldChanges(eFeature, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetFeatureYieldChanges(eFeature, eYield) * NumFeature * 2;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetFeatureYieldChanges(eFeature, eYield) * NumFeature;
				}
			}
			if (PolicyInfo->GetCityYieldFromUnimprovedFeature(eFeature, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetCityYieldFromUnimprovedFeature(eFeature, eYield) * NumFeature * 2; 
				}
				else
				{
					yield[eYield] += PolicyInfo->GetCityYieldFromUnimprovedFeature(eFeature, eYield) * NumFeature;
				}
			}
			if (PolicyInfo->GetUnimprovedFeatureYieldChanges(eFeature, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetUnimprovedFeatureYieldChanges(eFeature, eYield) * 2 * NumFeature;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetUnimprovedFeatureYieldChanges(eFeature, eYield) * NumFeature;
				}
			}
		}
	}

	TerrainTypes eTerrain;
	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		eTerrain = (TerrainTypes)iI;

		int NumTerrain = pPlayer->CountAllTerrain(eTerrain);

		if (NumTerrain <= 0)
			continue;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetTerrainYieldChanges(eTerrain, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetTerrainYieldChanges(eTerrain, eYield) * 2 * NumTerrain;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetTerrainYieldChanges(eTerrain, eYield) * NumTerrain;
				}
			}
		}
	}

	DomainTypes eDomain;
	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		eDomain = (DomainTypes)iI;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetTradeRouteYieldChange(eDomain, eYield) != 0)
			{
				if (pPlayerTraits->IsExpansionist())
				{
					yield[eYield] += PolicyInfo->GetTradeRouteYieldChange(eDomain, eYield) * 4;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetTradeRouteYieldChange(eDomain, eYield) * 2;
				}
			}
		}
	}

	SpecialistTypes eSpecialist;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		eSpecialist = (SpecialistTypes)iI;

		int NumSpecialists = 0;

		int iLoopCity;
		for (CvCity* pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity)) 
		{
			NumSpecialists += pLoopCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);
		}

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetSpecialistYieldChanges(eSpecialist, eYield) != 0)
			{
				if (pPlayerTraits->IsSmaller())
				{
					yield[eYield] += PolicyInfo->GetSpecialistYieldChanges(eSpecialist, eYield) * max(2, NumSpecialists) * 6;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetSpecialistYieldChanges(eSpecialist, eYield) * max(2, NumSpecialists) * 2;
				}
			}
		}
	}

	GreatPersonTypes eGreatPerson;
	for (int iI = 0; iI < GC.getNumGreatPersonInfos(); iI++)
	{
		eGreatPerson = (GreatPersonTypes)iI;

		if (PolicyInfo->GetGoldenAgeGreatPersonRateModifier(eGreatPerson) != 0)
		{
			if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
			{
				yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeGreatPersonRateModifier(eGreatPerson) * 6;
			}
			else
			{
				yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeGreatPersonRateModifier(eGreatPerson) * 2;
			}
		}

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (PolicyInfo->GetGreatPersonExpendedYield(eGreatPerson, eYield) != 0)
			{
				if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
				{
					yield[eYield] += PolicyInfo->GetGreatPersonExpendedYield(eGreatPerson, eYield) * 2;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetGreatPersonExpendedYield(eGreatPerson, eYield);
				}
			}
		}
	}

	PromotionTypes ePromotion;
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes)iI;

		if (PolicyInfo->IsFreePromotion(ePromotion))
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += 125;
			}
			else
			{
				yield[YIELD_GREAT_GENERAL_POINTS] += 25;
			}
		}

		for (int iJ = 0; iJ < GC.getNumUnitCombatClassInfos(); iJ++)
		{
			eUnitCombat = (UnitCombatTypes)iJ;

			if (PolicyInfo->IsFreePromotionUnitCombat(ePromotion, eUnitCombat))
			{
				if (pPlayerTraits->IsWarmonger())
				{
					yield[YIELD_GREAT_GENERAL_POINTS] += 125;
				}
				else
				{
					yield[YIELD_GREAT_GENERAL_POINTS] += 25;
				}
			}
		}
	}

	int NumSpecialists = 0;

	int iLoopCity;
	for (CvCity* pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity))
	{
		NumSpecialists += pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
	}

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eYield = (YieldTypes)i;

		if (PolicyInfo->GetCityYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetCityYieldChange(eYield) * 12 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCityYieldChange(eYield) * 6 * iNumCities;
			}
		}
		if (PolicyInfo->GetCoastalCityYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetCoastalCityYieldChange(eYield) * 8 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCoastalCityYieldChange(eYield) * 4 * iNumCities;
			}
		}
		if (PolicyInfo->GetCapitalYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldChange(eYield) * 15 * pPlayer->getCapitalCity()->getPopulation();
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldChange(eYield) * 10 * pPlayer->getCapitalCity()->getPopulation();
			}
		}
		if (PolicyInfo->GetCapitalYieldPerPopChange(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += (pPlayer->getCapitalCity()->getPopulation() * 100) / PolicyInfo->GetCapitalYieldPerPopChange(eYield);
			}
			else
			{
				yield[eYield] += (pPlayer->getCapitalCity()->getPopulation() * 50) / PolicyInfo->GetCapitalYieldPerPopChange(eYield);
			}
		}
		if (PolicyInfo->GetCapitalYieldPerPopChangeEmpire(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += (iPopulation * 100)  / PolicyInfo->GetCapitalYieldPerPopChangeEmpire(eYield);
			}
			else
			{
				yield[eYield] += (iPopulation * 50) / PolicyInfo->GetCapitalYieldPerPopChangeEmpire(eYield);
			}
		}
		if (PolicyInfo->GetCapitalYieldModifier(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldModifier(eYield) * pPlayer->getCapitalCity()->getPopulation();
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldModifier(eYield) * pPlayer->getCapitalCity()->getPopulation() / 2;
			}
		}
		if (PolicyInfo->GetGreatWorkYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetGreatWorkYieldChange(eYield) * 10 * max(1, pPlayer->GetCulture()->GetNumGreatWorks());
			}
			else
			{
				yield[eYield] += PolicyInfo->GetGreatWorkYieldChange(eYield) * 5 * max(1, pPlayer->GetCulture()->GetNumGreatWorks());
			}
		}
		if (PolicyInfo->GetSpecialistExtraYield(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism() || pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetSpecialistExtraYield(eYield) * 5 * max(2, NumSpecialists);
			}
			else
			{
				yield[eYield] += PolicyInfo->GetSpecialistExtraYield(eYield) * 2 * max(2, NumSpecialists);
			}
		}
		if (PolicyInfo->GetYieldFromBirth(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirth(eYield) * 5 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirth(eYield) * 2 * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldFromBirthCapital(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapital(eYield) * 4 * pPlayer->getCapitalCity()->getPopulation();
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapital(eYield) * 2 * pPlayer->getCapitalCity()->getPopulation();
			}
		}
		if (PolicyInfo->GetYieldFromBirthRetroactive(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthRetroactive(eYield) * 5 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthRetroactive(eYield) * 2 * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldFromBirthCapitalRetroactive(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapitalRetroactive(eYield) * 4 * pPlayer->getCapitalCity()->getPopulation();
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapitalRetroactive(eYield) * 2 * pPlayer->getCapitalCity()->getPopulation();
			}
		}
		if (PolicyInfo->GetYieldFromConstruction(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromConstruction(eYield) * 4 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromConstruction(eYield) * 2 * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldFromWonderConstruction(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism() || pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromWonderConstruction(eYield) * 2 * max(2, (iNumWonders / 2));
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromWonderConstruction(eYield) * max(2, (iNumWonders / 4));
			}
		}
		if (PolicyInfo->GetYieldFromTech(eYield) != 0)
		{
			if (pPlayerTraits->IsNerd() || pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromTech(eYield) * 15;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromTech(eYield) * 10;
			}
		}
		if (PolicyInfo->GetYieldFromBorderGrowth(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBorderGrowth(eYield) * 3 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBorderGrowth(eYield) * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldGPExpend(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldGPExpend(eYield) * 2;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldGPExpend(eYield);
			}
		}
		if (PolicyInfo->GetConquerorYield(eYield) != 0)
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[eYield] += PolicyInfo->GetConquerorYield(eYield) * 3;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetConquerorYield(eYield);
			}
		}
		if (PolicyInfo->GetFounderYield(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetFounderYield(eYield) * 3;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetFounderYield(eYield);
			}
		}
		if (PolicyInfo->GetReligionYieldMod(eYield) != 0)
		{
			if (pPlayerTraits->IsReligious())
			{
				yield[eYield] += PolicyInfo->GetReligionYieldMod(eYield) * 4 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetReligionYieldMod(eYield) * 2 * iNumCities;
			}
		}
		if (PolicyInfo->GetGoldenAgeYieldMod(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism() || pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetGoldenAgeYieldMod(eYield) * 2 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetGoldenAgeYieldMod(eYield) * 2;
			}
		}
		if (PolicyInfo->GetYieldFromKills(eYield) != 0)
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[eYield] += PolicyInfo->GetYieldFromKills(eYield) * 3;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromKills(eYield);
			}
		}
		if (PolicyInfo->GetYieldFromBarbarianKills(eYield) != 0)
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBarbarianKills(eYield) * 3;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBarbarianKills(eYield);
			}
		}
		if (PolicyInfo->GetYieldChangeTradeRoute(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldChangeTradeRoute(eYield) * 6 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldChangeTradeRoute(eYield) * 3 * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldChangesNaturalWonder(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldChangesNaturalWonder(eYield) * 5 * max(1, pPlayer->GetNumNaturalWondersInOwnedPlots());
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldChangesNaturalWonder(eYield) * max(1, pPlayer->GetNumNaturalWondersInOwnedPlots());
			}
		}
		if (PolicyInfo->GetYieldChangeWorldWonder(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetYieldChangeWorldWonder(eYield) * 3 * max(1, pPlayer->GetNumWonders());
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldChangeWorldWonder(eYield) * max(1, pPlayer->GetNumWonders());
			}
		}
		if (PolicyInfo->GetYieldFromMinorDemand(eYield) != 0)
		{
			if (pPlayerTraits->IsWarmonger())
			{
				yield[eYield] += PolicyInfo->GetYieldFromMinorDemand(eYield) * 4;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromMinorDemand(eYield);
			}
		}
		if (PolicyInfo->GetYieldFromWLTKD(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromWLTKD(eYield) * 2 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromWLTKD(eYield) * iNumCities;
			}
		}
		if (PolicyInfo->GetArtifactYieldChanges(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetArtifactYieldChanges(eYield) * 2;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetArtifactYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetArtYieldChanges(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetArtYieldChanges(eYield) * 2;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetArtYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetMusicYieldChanges(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetMusicYieldChanges(eYield) * 2;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetMusicYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetLitYieldChanges(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetLitYieldChanges(eYield) * 2;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetLitYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetYieldFromNonSpecialistCitizens(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromNonSpecialistCitizens(eYield) * 10;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromNonSpecialistCitizens(eYield) * 5;
			}
		}
		if (PolicyInfo->GetYieldModifierFromGreatWorks(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetYieldModifierFromGreatWorks(eYield) * 12 * max(1, pPlayer->GetCulture()->GetNumGreatWorks());
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldModifierFromGreatWorks(eYield) * 5 *  max(1, pPlayer->GetCulture()->GetNumGreatWorks());
			}
		}
		if (PolicyInfo->GetYieldModifierFromActiveSpies(eYield) != 0)
		{
			if (pPlayerTraits->IsDiplomat())
			{
				yield[eYield] += PolicyInfo->GetYieldModifierFromActiveSpies(eYield) * 5;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldModifierFromActiveSpies(eYield);
			}
		}
		if (PolicyInfo->GetYieldFromDelegateCount(eYield) != 0)
		{
			if (pPlayerTraits->IsDiplomat())
			{
				yield[eYield] += PolicyInfo->GetYieldFromDelegateCount(eYield) * 20;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromDelegateCount(eYield) * 5;
			}
		}
	}
	
	if (yield.size() != 0)
		LogPolicyAttributeYields(pPlayer, ePolicy, yield);

	return yield;
}

int CvPolicyAI::WeighPolicy(CvPlayer* pPlayer, PolicyTypes ePolicy)
{
	if (ePolicy == NO_POLICY)
		return 0;

	int iWeight = m_PolicyAIWeights.GetWeight(ePolicy);

	//Grand Strategy Considerations - if valid, it doubles our initial weighting.
	// == Grand Strategy ==

	// == Grand Strategy ==
	int iDiploInterest = 0;
	int iConquestInterest = 0;
	int iScienceInterest = 0;
	int iCultureInterest = 0;

	int iDiploValue = 0;
	int iScienceValue = 0;
	int iConquestValue = 0;
	int iCultureValue = 0;

	int iGrandStrategiesLoop;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy;
	CvString strGrandStrategyName;

	CvPlayerTraits* pPlayerTraits = pPlayer->GetPlayerTraits();

	// Loop through all GrandStrategies and get priority. Since these are usually 100+, we will divide by 10 later
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GC.GetGameAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes)iGrandStrategiesLoop;
		pGrandStrategy = GC.GetGameAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
		strGrandStrategyName = (CvString)pGrandStrategy->GetType();

		if (strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
		{
			iConquestInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
		{
			iCultureInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
		{
			iDiploInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
		{
			iScienceInterest += pPlayer->GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
	}
	if (pPlayerTraits->IsWarmonger())
	{
		iConquestInterest *= 3;
		iScienceInterest *= 2;
	}
	if (pPlayerTraits->IsExpansionist())
	{
		iConquestInterest *= 2;
		iCultureInterest *= 3;
	}
	if (pPlayerTraits->IsNerd())
	{
		iCultureInterest *= 2;
		iScienceInterest *= 3;
	}
	if (pPlayerTraits->IsDiplomat())
	{
		iConquestInterest *= 2;
		iDiploInterest *= 3;
	}
	if (pPlayerTraits->IsSmaller())
	{
		iCultureInterest *= 2;
		iScienceInterest *= 3;
	}
	if (pPlayerTraits->IsTourism())
	{
		iCultureInterest *= 3;
		iDiploInterest *= 2;
	}
	if (pPlayerTraits->IsReligious())
	{
		iCultureInterest *= 2;
		iDiploInterest *= 3;
	}

	CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
	if (pkPolicyInfo)
	{
		for (int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
		{
			FlavorTypes eFlavor = (FlavorTypes)iFlavor;
			if (eFlavor == NO_FLAVOR)
				continue;

			int iFlavorValue = pkPolicyInfo->GetFlavorValue(eFlavor);
			if (iFlavorValue > 0)
			{
				iWeight += pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavor);

				if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_DIPLOMACY")
				{
					iDiploValue += iFlavorValue;

					for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
					{
						PlayerTypes eMinor = (PlayerTypes)iMinorCivLoop;
						if (eMinor == NO_PLAYER)
							continue;

						// Loop through all minors - if we're itching to conquer, bail out on diplo policies.
						if (GET_PLAYER(eMinor).isMinorCiv() && GET_PLAYER(eMinor).isAlive())
						{
							if (pPlayer->GetDiplomacyAI()->GetMinorCivApproach(eMinor) >= MINOR_CIV_APPROACH_CONQUEST)
							{
								iDiploValue -= iFlavorValue;
								break;
							}
						}
					}
				}
				//War
				if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_OFFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_DEFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_CITY_DEFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MILITARY_TRAINING" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MOBILE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RANGED")
				{
					iConquestValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIR" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_ANTIAIR" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIR_CARRIER" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIRLIFT")
				{
					iConquestValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL_RECON" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RECON")
				{
					iConquestValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_EXPANSION" )
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_PRODUCTION")
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GOLD")
				{
					iDiploValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GREAT_PEOPLE")
				{
					iCultureValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_GROWTH")
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_HAPPINESS")
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_INFRASTRUCTURE")
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_I_LAND_TRADE_ROUTE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_I_LAND_TRADE_ROUTE")
				{
					iDiploValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_I_SEA_TRADE_ROUTE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_I_SEA_TRADE_ROUTE")
				{
					iDiploValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RELIGION")
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SCIENCE")
				{
					iScienceValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SPACESHIP")
				{
					iScienceValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_TILE_IMPROVEMENT")
				{
					iWeight += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_WONDER")
				{
					iCultureValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_WATER_CONNECTION" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_I_TRADE_DESTINATION")
				{
					iDiploValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_USE_NUKE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NUKE")
				{
					iConquestValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_ESPIONAGE")
				{
					iDiploValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_ARCHAEOLOGY")
				{
					iCultureValue += iFlavorValue;
				}
			}
		}
	}

	Firaxis::Array< int, NUM_YIELD_TYPES > yield = WeightPolicyAttributes(pPlayer, ePolicy);
	if (yield.size() != 0)
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			iWeight += yield[i];
		}		

		iConquestValue += yield[YIELD_GREAT_GENERAL_POINTS];
		iConquestValue += yield[YIELD_GREAT_ADMIRAL_POINTS];

		iCultureValue += yield[YIELD_TOURISM];

		iScienceValue += yield[YIELD_SCIENCE];

		iDiploValue += yield[YIELD_GOLD] / 2;
		iDiploValue += yield[YIELD_PRODUCTION] / 2;
	}

	iConquestValue *= (100 + (iConquestInterest / 10));
	iConquestValue /= 100;

	iCultureValue *= (100 + (iCultureInterest / 10));
	iCultureValue /= 100;

	iDiploValue *= (100 + (iDiploInterest / 10));
	iDiploValue /= 100;

	iScienceValue *= (100 + (iScienceInterest / 10));
	iScienceValue /= 100;

	//And now add them in. Halve if not our main focus.
	if (pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory() || pPlayer->GetDiplomacyAI()->IsCloseToCultureVictory())
	{
		iWeight += iCultureValue;
	}
	else
	{
		iWeight += (iCultureValue / 2);
	}
	if (pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() || pPlayer->GetDiplomacyAI()->IsCloseToDiploVictory())
	{
		iWeight += iDiploValue;
	}
	else
	{
		iWeight += (iDiploValue / 2);
	}
	if (pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory() || pPlayer->GetDiplomacyAI()->IsCloseToSSVictory())
	{
		iWeight += iScienceValue;
	}
	else
	{
		iWeight += (iScienceValue / 2);
	}

	if (pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest() || pPlayer->GetDiplomacyAI()->IsCloseToDominationVictory())
	{
		iWeight += iConquestValue;
	}
	else
	{
		iWeight += (iConquestValue / 2);
	}

	//If this is an ideology policy, let's snap those up.
	if (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetLevel() > 0)
	{
		iWeight *= (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetLevel() + 1);
	}
	if (!pPlayer->GetCorporations()->HasFoundedCorporation())
	{
		//Corporate-specific policies should only be taken if you have a corporation.
		if (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->IsCorporationOfficesAsFranchises() || m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->IsCorporationRandomForeignFranchise() || m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->IsCorporationFreeFranchiseAbovePopular())
		{
			iWeight = 0;
		}
	}
	//Older branches should be slowly phased out.
	PolicyBranchTypes ePolicyBranch = (PolicyBranchTypes)m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetPolicyBranchType();
	if (ePolicyBranch != NO_POLICY_BRANCH_TYPE)
	{
		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
		if (pkPolicyBranchInfo)
		{
			//If we're already in this branch, let's get a bonus based on how many we have in it (this will push the AI to finish branches quickly.
			if (m_pCurrentPolicies->GetNumPoliciesOwnedInBranch(ePolicyBranch) > 0 || m_pCurrentPolicies->IsPolicyBranchUnlocked(ePolicyBranch))
			{
				iWeight *= max(1, m_pCurrentPolicies->GetNumPoliciesOwnedInBranch(ePolicyBranch));
			}
			else
			{
				int iPolicyEra = pkPolicyBranchInfo->GetEraPrereq();
				int iPlayerEra = pPlayer->GetCurrentEra();
				if (iPolicyEra < iPlayerEra && iPlayerEra > 0)
				{
					iWeight /= max(2, ((iPlayerEra * 2) - iPolicyEra));
				}
			}
		}
		if (ePolicyBranch == (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_PATRONAGE", true))
		{
			if (GC.getGame().GetNumMinorCivsAlive() <= 0)
			{
				iWeight = 0;
			}
			else if (pPlayer->GetDiplomacyAI()->GetNumMinorCivApproach(MINOR_CIV_APPROACH_FRIENDLY) <= 0)
			{
				iWeight /= 5;
			}
		}
	}
	// Does this policy finish a branch for us?
	if (m_pCurrentPolicies->WillFinishBranchIfAdopted(ePolicy))
	{
		int iPolicyBranch = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetPolicyBranchType();
		if (iPolicyBranch != NO_POLICY_BRANCH_TYPE && m_pCurrentPolicies->GetPolicies()->GetPolicyBranchEntry(iPolicyBranch) != NULL)
		{
			int iFinisherPolicy = m_pCurrentPolicies->GetPolicies()->GetPolicyBranchEntry(iPolicyBranch)->GetFreeFinishingPolicy();
			if (iFinisherPolicy != NO_POLICY)
			{
				iWeight += m_PolicyAIWeights.GetWeight(iFinisherPolicy);
			}
		}
	}
	return iWeight;
}

/// Priority for opening up this branch
int CvPolicyAI::WeighBranch(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
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
					if (pkLoopPolicyInfo->GetPolicyBranchType() == eBranch)
					{
						iWeight += WeighPolicy(pPlayer, ePolicyLoop);
					}
				}
			}
		}
		//Free Policy
		iWeight += WeighPolicy(pPlayer, (PolicyTypes)pkPolicyBranchInfo->GetFreePolicy());
		iWeight += WeighPolicy(pPlayer, (PolicyTypes)pkPolicyBranchInfo->GetFreeFinishingPolicy());

		BuildingClassTypes eBuildingClass;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			eBuildingClass = (BuildingClassTypes)iI;

			if (pPlayer->getCapitalCity() != NULL)
			{
				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
				if (pkBuildingClassInfo)
				{
					const BuildingTypes eBuilding = ((BuildingTypes)(pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));
					if (NO_BUILDING != eBuilding)
					{
						CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
						if (pkBuildingInfo)
						{
							if (pkBuildingInfo->GetPolicyBranchType() != NO_POLICY_BRANCH_TYPE && pkBuildingInfo->GetPolicyBranchType() == eBranch)
							{
								int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 50, 10, 10, 10, false, true, true);
								if (iValue > 0)
								{
									if (pkBuildingInfo->GetFaithCost() != 0)
									{
										if (pPlayer->GetPlayerTraits()->IsReligious())
											iValue *= 2;
										else
											iValue /= 2;

										iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

										iWeight += max(0, iValue);
									}
									else
									{
										if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
											iValue /= 3;
										else if (pkBuildingClassInfo->getMaxGlobalInstances() == 1)
											iValue /= 2;

										iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

										iWeight += max(0, iValue);
									}
								}
							}
						}
					}
				}
			}
		}
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

void CvPolicyAI::LogPolicyAttributeYields(CvPlayer* pPlayer, PolicyTypes ePolicy, Firaxis::Array<int, NUM_YIELD_TYPES> yields)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		CvString strLogName;

		// Open the log file
		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "PolicyAIYieldAttributesLog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "PolicyAIYieldAttributesLog.csv";
		}

		// Find the name of this civ and city
		playerName = pPlayer->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
		const char* szPolicyType = (pPolicyEntry != NULL) ? pPolicyEntry->GetDescription() : "Unknown";
		int iTotal = 0;
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (GC.getYieldInfo((YieldTypes)i) == NULL)
				continue;

			int value = yields[i];

			if (value == 0)
				continue;

			iTotal += value;

			strTemp.Format("%s, %s, %d", szPolicyType, GC.getYieldInfo((YieldTypes)i)->GetDescription(), value);
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);

		}

		strTemp.Format("Policy Attributes Total: %d", iTotal);
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
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
				const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetDescription() : "Unknown";
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
		const char* szPolicyType = (pPolicyEntry != NULL) ? pPolicyEntry->GetDescription() : "Unknown";
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
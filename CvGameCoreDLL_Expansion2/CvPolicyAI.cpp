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
	m_iPolicyWeightPropagationLevels = /*2*/ GD_INT_GET(POLICY_WEIGHT_PROPAGATION_LEVELS);
	m_iPolicyWeightPercentDropNewBranch = /*90*/ max(GD_INT_GET(POLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH), 0);

	ASSERT_DEBUG(m_pCurrentPolicies != NULL, "Policy AI init failure: player policy data is NULL");
	if(m_pCurrentPolicies != NULL)
	{
		CvPolicyXMLEntries* pPolicyEntries = m_pCurrentPolicies->GetPolicies();
		ASSERT_DEBUG(pPolicyEntries != NULL, "Policy AI init failure: no policy data");
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

///
template<typename PolicyAI, typename Visitor>
void CvPolicyAI::Serialize(PolicyAI& policyAI, Visitor& visitor)
{
	visitor(policyAI.m_PolicyAIWeights);
}

/// Serialization read
void CvPolicyAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvPolicyAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator<<(FDataStream& saveTo, const CvPolicyAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvPolicyAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvPolicyAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent)
{
	if (iWeight==0)
		return;

	CvPolicyXMLEntries* pkPolicyEntries = m_pCurrentPolicies->GetPolicies();
	// Create a temporary array of weights
	vector<int> vTempWeights(pkPolicyEntries->GetNumPolicies(),0);

	// Loop through all our policies
	for(int iPolicy = 0; iPolicy < pkPolicyEntries->GetNumPolicies(); iPolicy++)
	{
		CvPolicyEntry* entry = pkPolicyEntries->GetPolicyEntry(iPolicy);

		// Set its weight by looking at policy's weight for this flavor and using iWeight multiplier passed in
		if(entry)
			vTempWeights[iPolicy] = entry->GetFlavorValue(eFlavor) * iWeight;
		else
			vTempWeights[iPolicy] = 0;
	}

	// Propagate these values left in the tree so prereqs get bought
	if(iPropagationPercent > 0)
	{
		WeightPrereqs(vTempWeights, iPropagationPercent);
	}

	// Add these weights over previous ones
	for(int iPolicy = 0; iPolicy < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicy++)
	{
		m_PolicyAIWeights.IncreaseWeight(iPolicy, vTempWeights[iPolicy]);
	}
}

/// Choose a player's next policy purchase (could be opening a branch)
int CvPolicyAI::ChooseNextPolicy(CvPlayer* pPlayer)
{
	if (!pPlayer->isMajorCiv())
		return 0;

	int iRtnValue = (int)NO_POLICY;
	int iPolicyLoop = 0;
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
			if(m_pCurrentPolicies->IsPolicyBranchUnlocked(ePolicyBranch))
			{
				if(pkPolicyBranchInfo->IsMutuallyExclusive())
				{
					bStartedAMutuallyExclusiveBranch = true;
				}
			}
		}
	}

	bool bNeedToFinish = false;
	for (int iBranchLoop2 = 0; iBranchLoop2 < GC.getNumPolicyBranchInfos(); iBranchLoop2++)
	{
		const PolicyBranchTypes ePolicyBranch2 = static_cast<PolicyBranchTypes>(iBranchLoop2);
		CvPolicyBranchEntry* pkPolicyBranchInfo2 = GC.getPolicyBranchInfo(ePolicyBranch2);
		// Do we already have a different policy branch unlocked?
		if (pkPolicyBranchInfo2 && m_pCurrentPolicies->IsPolicyBranchUnlocked(ePolicyBranch2))
		{
			// Have we not finished it yet? If we can finish it, let's not open a new one.
			PolicyTypes eFinisher = (PolicyTypes)pkPolicyBranchInfo2->GetFreeFinishingPolicy();
			if (eFinisher != NO_POLICY && !m_pCurrentPolicies->HasPolicy(eFinisher) && CanContinuePolicyBranch(ePolicyBranch2))
			{
				bNeedToFinish = true;
				break;
			}
		}
	}

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

				if(m_pCurrentPolicies->CanUnlockPolicyBranch(ePolicyBranch) && !m_pCurrentPolicies->IsPolicyBranchUnlocked(ePolicyBranch))
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
						iDivisor *= 2;
						iBranchWeight /= max(1, iDivisor);
					}

					m_AdoptablePolicies.push_back(iBranchLoop, iBranchWeight);
				}
			}
		}
	}

	m_AdoptablePolicies.StableSortItems();
	LogPossiblePolicies();

	// Make our policy choice from the top choices
	if (m_AdoptablePolicies.size() > 0)
	{
		iRtnValue = m_AdoptablePolicies.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getPolicyChoiceCutoffThreshold(), CvSeeder::fromRaw(0x5fef0474).mix(pPlayer->GetID()).mix(pPlayer->GetPlayerPolicies()->GetNumPoliciesOwned(false, false, true)));

		// Log our choice
		if (iRtnValue != (int)NO_POLICY)
		{
			if (iRtnValue >= GC.getNumPolicyBranchInfos())
			{
				LogPolicyChoice((PolicyTypes)(iRtnValue - GC.getNumPolicyBranchInfos()));
			}
			else
			{
				LogBranchChoice((PolicyBranchTypes)iRtnValue);
			}
		}
	}

	return iRtnValue;
}

bool CvPolicyAI::CanContinuePolicyBranch(PolicyBranchTypes ePolicyBranch)
{
	for (int iAdoptableIndex = 0; iAdoptableIndex < m_AdoptablePolicies.size(); ++iAdoptableIndex)
	{
		const PolicyTypes ePolicyIndex = PolicyTypes(m_AdoptablePolicies.GetElement(iAdoptableIndex) - GC.getNumPolicyBranchInfos());
		const CvPolicyEntry* pkPolicyEntry = GC.getPolicyInfo(ePolicyIndex);
		if (pkPolicyEntry == NULL)
			continue;

		const PolicyBranchTypes ePolicyBranchIndex = PolicyBranchTypes(pkPolicyEntry->GetPolicyBranchType());
		if (ePolicyBranchIndex == ePolicyBranch)
		{
			return true;
		}
	}

	return false;
}

void CvPolicyAI::DoChooseIdeology(CvPlayer *pPlayer)
{
	int iFreedomPriority = 0;
	int iAutocracyPriority = 0;
	int iOrderPriority = 0;
	int iFreedomMultiplier = 1;
	int iAutocracyMultiplier = 1;
	int iOrderMultiplier = 1;
	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_FREEDOM);
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_AUTOCRACY);
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_ORDER);
#if defined(MOD_ISKA_HERITAGE)
	PolicyBranchTypes eHeritageBranch = (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_HERITAGE);
#endif
	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return;
	}

	// Vassals are forced to choose the master's ideology
	TeamTypes eMasterTeam = GET_TEAM(pPlayer->getTeam()).GetMaster();
	if (eMasterTeam != NO_TEAM)
	{
		vector<PlayerTypes> vMasterTeam = GET_TEAM(eMasterTeam).getPlayers();
		for (size_t i=0; i<vMasterTeam.size(); i++)
		{
			PlayerTypes eMaster = GET_PLAYER(vMasterTeam[i]).GetID();

			// First player on the master's team that is alive and has > 0 cities is the one that counts
			if (GET_PLAYER(eMaster).isAlive() && GET_PLAYER(eMaster).getNumCities() > 0)
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

	// Team Leader used FOLLOW ME!
	TeamTypes eTeam = pPlayer->getTeam();
	PlayerTypes eTeamLeader = pPlayer->GetID();
	vector<PlayerTypes> vMyTeam = GET_TEAM(eTeam).getPlayers();
	int iBestScore = 0;
	int iMyScore = pPlayer->GetScore();
	if (pPlayer->GetDiplomacyAI()->IsCloseToCultureVictory())
		iMyScore += 100000;

	for (size_t i=0; i<vMyTeam.size(); i++)
	{
		PlayerTypes eTeamMember = GET_PLAYER(vMyTeam[i]).GetID();
		if (!GET_PLAYER(eTeamMember).isAlive() || GET_PLAYER(eTeamMember).getNumCities() == 0)
			continue;

		if (GET_PLAYER(eTeamMember).isHuman()) // AI shall bow before the human even if they're a weakling!
		{
			eTeamLeader = eTeamMember;
			break;
		}

		int iScore = GET_PLAYER(eTeamMember).GetScore();
		if (GET_PLAYER(eTeamMember).GetDiplomacyAI()->IsCloseToCultureVictory())
			iScore += 100000;

		if (iScore > iBestScore && iScore * 2 >= iMyScore * 3) // Must be at least 50% higher to justify this
		{
			eTeamLeader = eTeamMember;
			iBestScore = iScore;
		}
	}
	PolicyBranchTypes eLeaderIdeology = GET_PLAYER(eTeamLeader).GetPlayerPolicies()->GetLateGamePolicyTree();
	if (eTeamLeader != pPlayer->GetID() && eLeaderIdeology != NO_POLICY_BRANCH_TYPE)
	{
		// Sanity check - don't adopt the leader's ideology if it would result in us losing cities
		bool bFollowTheLeader = false;
		int iExtraUnhappiness = pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eLeaderIdeology);
		if (!MOD_BALANCE_VP)
		{
			bFollowTheLeader = pPlayer->GetExcessHappiness() - iExtraUnhappiness > /*-8*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD) + 2; // Add some margin of error
		}
		else
		{
			int iUnhappy = pPlayer->GetUnhappinessFromCitizenNeeds() + iExtraUnhappiness;
			int iHappy = pPlayer->GetHappinessFromCitizenNeeds();
			bFollowTheLeader = ((iHappy * 100) / max(1, iUnhappy) / 2) >= /*40*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD) + 5; // Add some margin of error
		}

		if (bFollowTheLeader)
		{
			pPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(eLeaderIdeology, true, false);
			LogBranchChoice(eLeaderIdeology);
			return;
		}
	}

	// == Grand Strategy ==
	int iDiploInterest = 0;
	int iConquestInterest = 0;
	int iScienceInterest = 0;
	int iCultureInterest = 0;

	//Grand Strategy Considerations - if valid, it doubles our initial weighting.
	// == Grand Strategy ==
	int iGrandStrategiesLoop = 0;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy = NULL;
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
		int iClearPrefPercent = /*25*/ GD_INT_GET(IDEOLOGY_PERCENT_CLEAR_VICTORY_PREF);
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
		int iPriorityToDivide = /*1000*/ GD_INT_GET(IDEOLOGY_SCORE_GRAND_STRATS);
		iFreedomPriority = (iFreedomTotal * iPriorityToDivide) / iGrandTotal;
		iAutocracyPriority = (iAutocracyTotal * iPriorityToDivide) / iGrandTotal;
		iOrderPriority = (iOrderTotal * iPriorityToDivide) / iGrandTotal;
	}

	CvString stage = "After Grand Strategies";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Next look at free policies we can get
	iFreedomPriority += PolicyHelpers::GetNumFreePolicies(eFreedomBranch) * /*45*/ GD_INT_GET(IDEOLOGY_SCORE_PER_FREE_TENET);
	iAutocracyPriority += PolicyHelpers::GetNumFreePolicies(eAutocracyBranch) * /*45*/ GD_INT_GET(IDEOLOGY_SCORE_PER_FREE_TENET);
	iOrderPriority += PolicyHelpers::GetNumFreePolicies(eOrderBranch) * /*45*/ GD_INT_GET(IDEOLOGY_SCORE_PER_FREE_TENET);

	stage = "After Free Policies";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Finally see what our friends (and enemies) have already chosen
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eLoopPlayer);
			PolicyBranchTypes eOtherPlayerIdeology;
			eOtherPlayerIdeology = kOtherPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

			if (pPlayer->GetDiplomacyAI()->GetBiggestCompetitor() == eLoopPlayer)
			{
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
					iOrderPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
					iOrderPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
					iFreedomPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
				}
			}
			else if (pPlayer->GetDiplomacyAI()->GetMostValuableAlly() == eLoopPlayer || pPlayer->GetDiplomacyAI()->GetMostValuableFriend() == eLoopPlayer)
			{
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += /*25*/ GD_INT_GET(IDEOLOGY_SCORE_FRIENDLY);
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += /*25*/ GD_INT_GET(IDEOLOGY_SCORE_FRIENDLY);
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += /*25*/ GD_INT_GET(IDEOLOGY_SCORE_FRIENDLY);
				}
			}

			switch (pPlayer->GetDiplomacyAI()->GetCivApproach(eLoopPlayer))
			{
			case CIV_APPROACH_HOSTILE:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
					iOrderPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
					iOrderPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
					iFreedomPriority += /*500*/ GD_INT_GET(IDEOLOGY_SCORE_HOSTILE);
				}
				break;
			case CIV_APPROACH_GUARDED:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iAutocracyPriority += /*250*/ GD_INT_GET(IDEOLOGY_SCORE_GUARDED);
					iOrderPriority += /*250*/ GD_INT_GET(IDEOLOGY_SCORE_GUARDED);
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iFreedomPriority += /*250*/ GD_INT_GET(IDEOLOGY_SCORE_GUARDED);
					iOrderPriority += /*250*/ GD_INT_GET(IDEOLOGY_SCORE_GUARDED);
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iAutocracyPriority += /*250*/ GD_INT_GET(IDEOLOGY_SCORE_GUARDED);
					iFreedomPriority += /*250*/ GD_INT_GET(IDEOLOGY_SCORE_GUARDED);
				}
				break;
			case CIV_APPROACH_AFRAID:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += /*150*/ GD_INT_GET(IDEOLOGY_SCORE_AFRAID);
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += /*150*/ GD_INT_GET(IDEOLOGY_SCORE_AFRAID);
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += /*150*/ GD_INT_GET(IDEOLOGY_SCORE_AFRAID);
				}
				break;
			case CIV_APPROACH_FRIENDLY:
				if (eOtherPlayerIdeology == eFreedomBranch)
				{
					iFreedomPriority += /*25*/ GD_INT_GET(IDEOLOGY_SCORE_FRIENDLY);
				}
				else if (eOtherPlayerIdeology == eAutocracyBranch)
				{
					iAutocracyPriority += /*25*/ GD_INT_GET(IDEOLOGY_SCORE_FRIENDLY);
				}
				else if (eOtherPlayerIdeology == eOrderBranch)
				{
					iOrderPriority += /*25*/ GD_INT_GET(IDEOLOGY_SCORE_FRIENDLY);
				}
				break;
			default:
				// No changes
				break;
			}
		}
	}

	stage = "After Relations";
	LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);

	// Look at Happiness impacts
	int iHappinessModifier = /*10*/ GD_INT_GET(IDEOLOGY_SCORE_HAPPINESS);

	// -- Happiness we could add through tenets
	int iHappinessDelta = 0;

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

	//This was a dumb reason to weigh an ideology branch.

	int iHappinessPoliciesInBranch = 0;
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

	bool bFirstIdeology = true;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
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

	if (!bFirstIdeology && pPlayer->IsEmpireUnhappy())
	{
		// -- Happiness we'd lose through Public Opinion
		// Recursive: Due to unhappiness being capped by population and Public Works existing, only consider this if already unhappy
		iHappinessDelta = max (0, 250 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eFreedomBranch));
		iFreedomPriority += iHappinessDelta * iHappinessModifier;
		iHappinessDelta = max (0, 250 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eAutocracyBranch));
		iAutocracyPriority += iHappinessDelta * iHappinessModifier;
		iHappinessDelta = max (0, 250 - pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(eOrderBranch));
		iOrderPriority += iHappinessDelta * iHappinessModifier;

		stage = "After Public Opinion Happiness (Empire Unhappy)";
		LogIdeologyChoice(stage, iFreedomPriority, iAutocracyPriority, iOrderPriority);
	}

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

	ReligionTypes ePlayerReligion = pPlayer->GetReligions()->GetOwnedReligion();
	if (MOD_ISKA_HERITAGE && pPlayer->GetReligions()->HasReligionInMostCities(ePlayerReligion) && ePlayerReligion > RELIGION_PANTHEON)
	{
		eChosenBranch = eHeritageBranch;
		if (iAutocracyPriority >= (iFreedomPriority + iOrderPriority) * 2)
		{
			eChosenBranch = eAutocracyBranch;
		}
		else if (iOrderPriority >= (iFreedomPriority + iAutocracyPriority) * 2)
		{
			eChosenBranch = eOrderBranch;
		}
		else if (iFreedomPriority >= (iOrderPriority + iAutocracyPriority) * 2)
		{
			eChosenBranch = eFreedomBranch;
		}
	}

	pPlayer->GetPlayerPolicies()->SetPolicyBranchUnlocked(eChosenBranch, true, false);
	LogBranchChoice(eChosenBranch);

	CvPlayerTraits* pPlayerTraits = pPlayer->GetPlayerTraits();
	CvCity* pCapital = pPlayer->getCapitalCity(); //Define capital
	int iPolicyGEorGM = pPlayerTraits->GetPolicyGEorGM();
	if (iPolicyGEorGM > 0 && pCapital != NULL)
	{
		pPlayer->doPolicyGEorGM(iPolicyGEorGM);
	}

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

/// Should the AI look at switching ideology branches?
/// Humans also call this function to force a switch if they're a vassal.
void CvPolicyAI::DoConsiderIdeologySwitch(CvPlayer* pPlayer)
{
	// Gather basic Ideology info
	PolicyBranchTypes eCurrentIdeology = pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	if (eCurrentIdeology == NO_POLICY_BRANCH_TYPE)
		return;

	TeamTypes eMasterTeam = GET_TEAM(pPlayer->getTeam()).GetMaster();
	if (eMasterTeam != NO_TEAM)
	{
		vector<PlayerTypes> vMasterTeam = GET_TEAM(eMasterTeam).getPlayers();
		for (size_t i=0; i<vMasterTeam.size(); i++)
		{
			PlayerTypes eMaster = GET_PLAYER(vMasterTeam[i]).GetID();

			// First player on the master's team that is alive and has > 0 cities is the one that counts
			if (GET_PLAYER(eMaster).isAlive() && GET_PLAYER(eMaster).getNumCities() > 0)
			{
				PolicyBranchTypes eMasterIdeology = GET_PLAYER(eMaster).GetPlayerPolicies()->GetLateGamePolicyTree();
				if (eMasterIdeology != NO_POLICY_BRANCH_TYPE && eMasterIdeology != eCurrentIdeology)
				{
					if (MOD_API_ACHIEVEMENTS && eMasterIdeology == GD_INT_GET(POLICY_BRANCH_FREEDOM) && eCurrentIdeology == GD_INT_GET(POLICY_BRANCH_ORDER))
					{
						PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
						if (GET_PLAYER(eActivePlayer).isAlive() && GET_PLAYER(eActivePlayer).isHuman() && GET_PLAYER(eActivePlayer).getTeam() == eMasterTeam)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
						}
					}

					// Cleared all obstacles -- REVOLUTION!
					pPlayer->SetAnarchyNumTurns(/*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS));
					pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(eMasterIdeology);
					Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
					Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
					pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), /*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS), -1);
					return;
				}
			}
		}
	}

	// Humans halt here!
	if (pPlayer->isHuman())
		return;

	int iPublicOpinionUnhappiness = pPlayer->GetCulture()->GetPublicOpinionUnhappiness();
	PolicyBranchTypes ePreferredIdeology = pPlayer->GetCulture()->GetPublicOpinionPreferredIdeology();

	// Can't switch.
	if (iPublicOpinionUnhappiness == 0 || ePreferredIdeology == NO_POLICY_BRANCH_TYPE || ePreferredIdeology == eCurrentIdeology)
		return;

	// Would switching cure our happiness problems?
	bool bVUnhappy = pPlayer->IsEmpireVeryUnhappy();
	bool bSUnhappy = pPlayer->IsEmpireSuperUnhappy();
	if (bSUnhappy)
	{
		int iNewUnhappiness = pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(ePreferredIdeology);
		if (!MOD_BALANCE_VP)
		{
			bSUnhappy = pPlayer->GetExcessHappiness() + iPublicOpinionUnhappiness - iNewUnhappiness > /*-20*/ GD_INT_GET(SUPER_UNHAPPY_THRESHOLD);
		}
		else
		{
			int iUnhappy = pPlayer->GetUnhappinessFromCitizenNeeds() - iPublicOpinionUnhappiness + iNewUnhappiness;
			int iHappy = pPlayer->GetHappinessFromCitizenNeeds();
			bSUnhappy = ((iHappy * 100) / max(1, iUnhappy) / 2) >= /*20*/ GD_INT_GET(SUPER_UNHAPPY_THRESHOLD);
		}
	}
	else if (bVUnhappy)
	{
		int iNewUnhappiness = pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(ePreferredIdeology);
		if (!MOD_BALANCE_VP)
		{
			bVUnhappy = pPlayer->GetExcessHappiness() + iPublicOpinionUnhappiness - iNewUnhappiness > /*-10*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD);
		}
		else
		{
			int iUnhappy = pPlayer->GetUnhappinessFromCitizenNeeds() - iPublicOpinionUnhappiness + iNewUnhappiness;
			int iHappy = pPlayer->GetHappinessFromCitizenNeeds();
			bVUnhappy = ((iHappy * 100) / max(1, iUnhappy) / 2) >= /*35*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD);
		}
	}

	// Team Leader used FOLLOW ME!
	TeamTypes eTeam = pPlayer->getTeam();
	PlayerTypes eTeamLeader = pPlayer->GetID();
	bool bTeamLeaderSwitchRequested = false;
	bool bTeamLeaderSwitchAvoided = false;
	if (!bSUnhappy)
	{
		vector<PlayerTypes> vMyTeam = GET_TEAM(eTeam).getPlayers();
		int iBestScore = 0;
		int iMyScore = pPlayer->GetScore();
		if (pPlayer->GetDiplomacyAI()->IsCloseToCultureVictory())
			iMyScore += 100000;

		for (size_t i=0; i<vMyTeam.size(); i++)
		{
			PlayerTypes eTeamMember = GET_PLAYER(vMyTeam[i]).GetID();
			if (!GET_PLAYER(eTeamMember).isAlive() || GET_PLAYER(eTeamMember).getNumCities() == 0)
				continue;

			if (GET_PLAYER(eTeamMember).isHuman()) // AI shall bow before the human even if they're a weakling!
			{
				eTeamLeader = eTeamMember;
				break;
			}

			int iScore = GET_PLAYER(eTeamMember).GetScore();
			if (GET_PLAYER(eTeamMember).GetDiplomacyAI()->IsCloseToCultureVictory())
				iScore += 100000;

			if (iScore > iBestScore && iScore * 2 >= iMyScore * 3) // Must be at least 50% higher to justify this
			{
				eTeamLeader = eTeamMember;
				iBestScore = iScore;
			}
		}
		if (eTeamLeader != pPlayer->GetID())
		{
			PolicyBranchTypes eTeamLeaderIdeology = GET_PLAYER(eTeamLeader).GetPlayerPolicies()->GetLateGamePolicyTree();
			if (eTeamLeaderIdeology == ePreferredIdeology)
			{
				// Switch if we wouldn't lose cities to unhappiness
				bTeamLeaderSwitchRequested = true;
			}
			else if (eTeamLeaderIdeology == eCurrentIdeology)
			{
				// Don't switch if we can avoid losing cities to unhappiness
				bTeamLeaderSwitchAvoided = true;
			}
		}
	}

	// Possible enough that we need to look at this in detail?
	if (bSUnhappy)
	{
		//Final sanity check - are we flip-flopping?
		if (!bTeamLeaderSwitchRequested && GC.getGame().getGameTurn() - pPlayer->GetCulture()->GetTurnIdeologySwitch() <= 30)
		{
			return;
		}

		if (MOD_API_ACHIEVEMENTS && ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_FREEDOM) && eCurrentIdeology == GD_INT_GET(POLICY_BRANCH_ORDER))
		{
			PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
			if (eMostPressure != NO_PLAYER && GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
			}
		}

		// Cleared all obstacles -- REVOLUTION!
		pPlayer->SetAnarchyNumTurns(/*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS));
		pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);
		Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
		Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
		pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), /*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS), -1);
	}
	else if (bTeamLeaderSwitchAvoided)
	{
		return;
	}
	else if (bTeamLeaderSwitchRequested)
	{
		//Sanity check - would a change to this branch put us at risk of losing cities? If so, don't do it.
		int iNewUnhappiness = pPlayer->GetCulture()->ComputeHypotheticalPublicOpinionUnhappiness(ePreferredIdeology);
		bool bSwitch = false;
		if (!MOD_BALANCE_VP)
		{
			bSwitch = pPlayer->GetExcessHappiness() + iPublicOpinionUnhappiness - iNewUnhappiness > /*-8*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD) + 2; // Add some margin of error
		}
		else
		{
			int iUnhappy = pPlayer->GetUnhappinessFromCitizenNeeds() - iPublicOpinionUnhappiness + iNewUnhappiness;
			int iHappy = pPlayer->GetHappinessFromCitizenNeeds();
			bSwitch = ((iHappy * 100) / max(1, iUnhappy) / 2) >= /*40*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD) + 5; // Add some margin of error
		}

		if (bSwitch)
		{
			if (MOD_API_ACHIEVEMENTS && ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_FREEDOM) && eCurrentIdeology == GD_INT_GET(POLICY_BRANCH_ORDER))
			{
				PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
				if (eMostPressure != NO_PLAYER && GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
				}
			}

			// Cleared all obstacles -- REVOLUTION!
			pPlayer->SetAnarchyNumTurns(/*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS));
			pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);
			Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
			Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
			pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), /*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS), -1);
		}
	}
	else if (bVUnhappy)
	{
		// Only switch ideologies if we're about to lose a city.
		if (pPlayer->GetCityRevoltCounter() <= 2 + /*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS))
			return;

		// Does the switch fight against our clearly preferred victory path?
		bool bDontSwitchFreedom = false;
		bool bDontSwitchOrder = false;
		bool bDontSwitchAutocracy = false;
		int iConquestPriority = pPlayer->GetGrandStrategyAI()->GetConquestPriority();
		int iDiploPriority = pPlayer->GetGrandStrategyAI()->GetUnitedNationsPriority();
		int iTechPriority = pPlayer->GetGrandStrategyAI()->GetSpaceshipPriority();
		int iCulturePriority = pPlayer->GetGrandStrategyAI()->GetCulturePriority();
		int iClearPrefPercent = /*25*/ GD_INT_GET(IDEOLOGY_PERCENT_CLEAR_VICTORY_PREF);
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

		if (bDontSwitchFreedom && ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_FREEDOM))
		{
			return;
		}
		if (bDontSwitchAutocracy && ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_AUTOCRACY))
		{
			return;
		}
		if (bDontSwitchOrder && ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_ORDER))
		{
			return;
		}
		//Sanity check - are we flip-flopping?
		if (GC.getGame().getGameTurn() - pPlayer->GetCulture()->GetTurnIdeologySwitch() <= 30)
		{
			return;
		}

		if (MOD_API_ACHIEVEMENTS && ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_FREEDOM) && eCurrentIdeology == GD_INT_GET(POLICY_BRANCH_ORDER))
		{
			PlayerTypes eMostPressure = pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
			if (eMostPressure != NO_PLAYER && GET_PLAYER(eMostPressure).GetID() == GC.getGame().getActivePlayer())
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_39);
			}
		}

		// Cleared all obstacles -- REVOLUTION!
		pPlayer->SetAnarchyNumTurns(/*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS));
		pPlayer->GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);	
		Localization::String strSummary = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS_SUMMARY");
		Localization::String strMessage = Localization::Lookup("TXT_KEY_ANARCHY_BEGINS");
		pPlayer->GetNotifications()->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pPlayer->GetID(), /*2 in CP, 3 in VP*/ GD_INT_GET(SWITCH_POLICY_BRANCHES_ANARCHY_TURNS), -1);
	}
}

/// What's the total Happiness benefit we could get from all policies/tenets in the branch based on our current buildings?
int CvPolicyAI::GetBranchBuildingHappiness(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{
	int iTotalHappiness = 0;
	for (int iI = 0; iI < GC.getNumPolicyInfos(); iI++)
	{
		PolicyTypes ePolicy = static_cast<PolicyTypes>(iI);
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if (!pkPolicyInfo)
			continue;

		if (pkPolicyInfo->GetPolicyBranchType() == eBranch)
		{
			for (int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
			{
				BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iJ);
				int iHappiness = pkPolicyInfo->GetBuildingClassHappiness(iJ);
				if (pkPolicyInfo->GetBuildingClassHappiness(iJ) != 0)
				{
					int iLoop = 0;
					for (CvCity* pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
					{
						int iNumBuilding = pCity->GetCityBuildings()->GetNumBuildingClass(eBuildingClass);
						iTotalHappiness += iHappiness * iNumBuilding;
					}
				}
			}
		}
	}

	return iTotalHappiness;
}

/// How many policies in this branch help happiness?
int CvPolicyAI::GetNumHappinessPolicies(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{
	int iRtnValue = 0;
	int iBuildingClassLoop = 0;
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
void CvPolicyAI::WeightPrereqs(const vector<int>& vTempWeights, int iPropagationPercent)
{
	// Loop through policies looking for ones that are just getting some new weight
	for(int iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		// If found one, call our recursive routine to weight everything to the left in the tree
		if(vTempWeights[iPolicyLoop] > 0)
		{
			PropagateWeights(iPolicyLoop, vTempWeights[iPolicyLoop], iPropagationPercent, 0);
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
		for(int iI = 0; iI < /*3*/ GD_INT_GET(NUM_OR_TECH_PREREQS); iI++)
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
		iNumCities = 4;
	}
	else
	{
		if (pPlayerTraits->IsExpansionist())
			iNumCities++;
		else if (pPlayerTraits->IsSmaller())
			iNumCities--;
		if (GC.getGame().getGameTurn() <= GC.getGame().getEstimateEndTurn() / 4)
		{
			//Estimation, because early game we assume we'll expand.
			iNumCities = (3 + iNumCities);
			iNumCities *= 100;
			iNumCities /= max(50, GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent());

			if (iNumCities <= 1)
				iNumCities = 1;
		}
	}

	int iPopulation = (int)(pPlayer->getAveragePopulation() + 0.5f) * max(1, (iNumCities / 2));
	iPopulation *= 100;
	iPopulation /= max(75, GC.getMap().getWorldInfo().getNumCitiesUnhappinessPercent());

	if (iPopulation <= 1)
		iPopulation = 1;

	int iNumWonders = pPlayer->GetNumWonders() + (pPlayer->GetDiplomacyAI()->GetWonderCompetitiveness() /2);
	
	//Capital pop estimate for early game.
	int iCapitalPop = max(10, pPlayer->getCapitalCity()->getPopulation());

	//Numbers of Technologies researched for early game
	int iTechnologiesResearched = max(15, GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown() - 1);
	
	if (PolicyInfo->GetPolicyCostModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPolicyCostModifier() * -5 * iNumCities;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetPolicyCostModifier() * -3 * iNumCities;
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
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureFromKills() * 5;
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
			yield[YIELD_CULTURE] += PolicyInfo->GetCultureFromBarbarianKills() * 5;
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
			yield[YIELD_GOLD] += PolicyInfo->GetGoldFromKills() * 5;
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
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTurns() * max(1, pPlayer->GetNumGoldenAges()) * 5;
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTurns() * max(1, pPlayer->GetNumGoldenAges()) * 2;
		}
	}
	if (PolicyInfo->GetGoldenAgeMeterMod() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeMeterMod() * 2 * max(1, pPlayer->GetNumGoldenAges());
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeMeterMod() * max(1, pPlayer->GetNumGoldenAges());
		}
	}
	if (PolicyInfo->GetGAPFromHappinessModifier() != 0)
	{
		int iTotalGAP = 0;
		int iLoop = 0;
		for (const CvCity* pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
		{
			int iGAP = pLoopCity->getHappinessDelta();
			if (iGAP > 0)
				iTotalGAP += iGAP;
		}
		if (pPlayerTraits->IsTourism() || pPlayerTraits->IsSmaller())
		{
			yield[YIELD_GOLDEN_AGE_POINTS] += iTotalGAP * PolicyInfo->GetGAPFromHappinessModifier() / 100 * 2;
		}
		else
		{
			yield[YIELD_GOLDEN_AGE_POINTS] += iTotalGAP * PolicyInfo->GetGAPFromHappinessModifier() / 100;
		}
	}
	if (PolicyInfo->GetGoldenAgeDurationMod() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeDurationMod() * 4 * max(1, pPlayer->GetNumGoldenAges());
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeDurationMod() * 2 * max(1, pPlayer->GetNumGoldenAges());
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
			yield[YIELD_PRODUCTION] += PolicyInfo->GetBuildingProductionModifier() * 10;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetBuildingProductionModifier() * 5;
		}
	}
	if (PolicyInfo->GetConquestPerEraBuildingProductionMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetConquestPerEraBuildingProductionMod() * 5 * max(1, pPlayer->GetNumPuppetCities());
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetConquestPerEraBuildingProductionMod() * 2 * max(1, pPlayer->GetNumPuppetCities());
		}
	}
	if (PolicyInfo->GetPuppetYieldPenaltyMod() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetPuppetYieldPenaltyMod() * 5 * pPlayer->GetNumPuppetCities();
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetPuppetYieldPenaltyMod() * 2 * pPlayer->GetNumPuppetCities();
		}
	}
	if (PolicyInfo->GetFlatDefenseFromAirUnits() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetFlatDefenseFromAirUnits() * 10 * iNumCities;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetFlatDefenseFromAirUnits() * 5 * iNumCities;
		}
	}
	if (PolicyInfo->GetNeedsModifierFromAirUnits() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetNeedsModifierFromAirUnits() * 10 * iNumCities;
		}
		else
		{
			yield[YIELD_PRODUCTION] += PolicyInfo->GetNeedsModifierFromAirUnits() * 5 * iNumCities;
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
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatWriterRateModifier() * 5 + pPlayer->getGreatWritersCreated(true);
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
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatArtistRateModifier() * 5 + pPlayer->getGreatArtistsCreated(true);
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
			yield[YIELD_TOURISM] += PolicyInfo->GetGreatMusicianRateModifier() * 5 + pPlayer->getGreatMusiciansCreated(true);
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
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCapitalUnhappinessMod();
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCapitalUnhappinessMod() * -3;
		}
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
	if (PolicyInfo->GetHappyPerMilitaryUnit() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += (PolicyInfo->GetHappyPerMilitaryUnit() / 5) * 50;
		}
		else
		{
			yield[YIELD_FOOD] += (PolicyInfo->GetHappyPerMilitaryUnit() / 10) * 50;
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
	
	if (PolicyInfo->IsNoXPLossUnitPurchase())
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += 50 * iNumCities;
		}
		else
		{
			yield[YIELD_GOLD] += 25 * iNumCities;
		}
	}


	if (pPlayer->GetCorporations()->GetFoundedCorporation() != NO_CORPORATION)
	{
		if (PolicyInfo->GetCorporationOfficesAsFranchises() != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[YIELD_GOLD] += 30 * iNumCities;
			}
			else
			{
				yield[YIELD_GOLD] += 15 * iNumCities;
			}
		}

		if (PolicyInfo->GetCorporationFreeFranchiseAbovePopular() != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[YIELD_TOURISM] += 1500;
			}
			else
			{
				yield[YIELD_TOURISM] += 250;
			}
		}
		if (PolicyInfo->GetCorporationRandomForeignFranchiseMod() != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[YIELD_GOLD] += 1500;
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
				yield[YIELD_GOLD] += PolicyInfo->GetAdditionalNumFranchisesMod() * 50;
			}
			else
			{
				yield[YIELD_GOLD] += PolicyInfo->GetAdditionalNumFranchisesMod() * 5;
			}
		}
	}
	if (PolicyInfo->IsUpgradeCSVassalTerritory())
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
			yield[YIELD_TOURISM] += PolicyInfo->GetArchaeologicalDigTourism() * 10 * pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetArchaeologicalDigTourism() * pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		}
	}
	if (PolicyInfo->GetLandmarksTourismPercent() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += 200;
		}
		else
		{
			yield[YIELD_TOURISM] += 100;
		}
	}
	if (PolicyInfo->GetGoldenAgeTourism() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetGoldenAgeTourism() * 33 * pPlayer->GetNumGoldenAges();
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
			yield[YIELD_SCIENCE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 30;
			yield[YIELD_CULTURE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 30;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 15;
			yield[YIELD_CULTURE] += PolicyInfo->GetExtraCultureandScienceTradeRoutes() * 15;
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
	if (PolicyInfo->GetHappinessPerCityOverStrengthThreshold() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_FOOD] += iNumCities * PolicyInfo->GetHappinessPerCityOverStrengthThreshold() * 10;
		}
		else
		{
			yield[YIELD_FOOD] += iNumCities * PolicyInfo->GetHappinessPerCityOverStrengthThreshold() * 5;
		}
	}
	if (PolicyInfo->GetHappinessPerXGreatWorks() != 0)
	{
		if (pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerXGreatWorks() * 25;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetHappinessPerXGreatWorks() * 5;
		}
	}
#if defined(MOD_BALANCE_CORE_POLICIES)
	if (PolicyInfo->GetExtraMissionaryStrength() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += PolicyInfo->GetExtraMissionaryStrength() * 4;
		}
		else
		{
			yield[YIELD_FAITH] += PolicyInfo->GetExtraMissionaryStrength() * 1;
		}
	}

	if (PolicyInfo->GetExtraMissionarySpreads() != 0)
	{
		if (pPlayerTraits->IsReligious())
		{
			yield[YIELD_FAITH] += PolicyInfo->GetExtraMissionarySpreads() * 80;
		}
		else
		{
			yield[YIELD_FAITH] += PolicyInfo->GetExtraMissionarySpreads() * 5;
		}
	}
#endif
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
			yield[YIELD_CULTURE] += PolicyInfo->GetNumCitiesPolicyCostDiscount() * -10;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetNumCitiesPolicyCostDiscount() * -2;
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
			yield[YIELD_GOLD] += PolicyInfo->GetUnitPurchaseCostModifier() * -4;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetUnitPurchaseCostModifier() * -2;
		}
	}
	if (PolicyInfo->GetNewFoundCityFreeBuilding() != NO_BUILDINGCLASS)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += 100;
		}
		else
		{
			yield[YIELD_GOLD] += 50;
		}
	}
	if (PolicyInfo->GetNewFoundCityFreeUnit() != NO_UNITCLASS)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GOLD] += 100;
		}
		else
		{
			yield[YIELD_GOLD] += 50;
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
	if (PolicyInfo->GetRigElectionInfluenceModifier() != 0)
	{
		if (pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetRigElectionInfluenceModifier() * 5;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetRigElectionInfluenceModifier();
		}
	}
	if (PolicyInfo->GetPassiveEspionageBonusModifier() != 0)
	{
		yield[YIELD_SCIENCE] += PolicyInfo->GetRigElectionInfluenceModifier() / 10;
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
			yield[YIELD_TOURISM] += PolicyInfo->GetThemingBonusMultiplier() * (pPlayer->GetCulture()->GetTotalThemingBonuses());
		}
		else
		{
			yield[YIELD_TOURISM] += PolicyInfo->GetThemingBonusMultiplier() * (pPlayer->GetCulture()->GetTotalThemingBonuses() / 2);
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
			yield[YIELD_PRODUCTION] += PolicyInfo->GetTradeRouteYieldModifierCapital();
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
					int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 10, true, true, true);
					if (iValue > 0)
					{
						iValue -= pPlayer->GetCurrentEra() * 25;

						if (iValue <= 0)
							iValue = 0;

						yield[YIELD_PRODUCTION] += min(250, iValue);
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
	if (PolicyInfo->GetMinimumAllyInfluenceIncreaseAtWar() > 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsDiplomat())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += (250 * PolicyInfo->GetMinimumAllyInfluenceIncreaseAtWar()) / 100;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += (50 * PolicyInfo->GetMinimumAllyInfluenceIncreaseAtWar()) / 100;
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
	if (PolicyInfo->GetVassalYieldBonusModifier() && GET_TEAM(pPlayer->getTeam()).GetNumVassals() > 0)
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
	if (PolicyInfo->GetCSYieldBonusModifier() && pPlayer->GetNumCSAllies() > 0)
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
		if (pPlayerTraits->IsTourism() || pPlayerTraits->IsSmaller())
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetHappinessToCulture() * 5;
		}
		else
		{
			yield[YIELD_CULTURE] += PolicyInfo->GetHappinessToCulture() * 2;
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
			yield[YIELD_FOOD] += 500;
		}
		else
		{
			yield[YIELD_FOOD] += 150;
		}
	}
	if (PolicyInfo->IsHalfSpecialistFoodCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
		{
			yield[YIELD_FOOD] += 500;
		}
		else
		{
			yield[YIELD_FOOD] += 150;
		}
	}

	if (PolicyInfo->GetDefenseBoost() != 0)
	{
		if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += (10 + (PolicyInfo->GetDefenseBoost()/100)) * iNumCities;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += (5 + (PolicyInfo->GetDefenseBoost() / 100)) * iNumCities;
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
			yield[YIELD_GOLD] += PolicyInfo->GetMonopolyModPercent() * 10;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetMonopolyModPercent() * 2;
		}
	}
	if (PolicyInfo->GetAdmiralLuxuryBonus() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GOLD] += PolicyInfo->GetAdmiralLuxuryBonus() * 50;
		}
		else
		{
			yield[YIELD_GOLD] += PolicyInfo->GetAdmiralLuxuryBonus() * 25;
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
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(pkBuildingInfo->GetBuildingClassType());
			if (pkBuildingClassInfo)
			{
				int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(PolicyInfo->GetFreeBuildingOnConquest(), 10, true, true, true);
				if (iValue > 0)
				{
					if (pPlayerTraits->IsWarmonger())
						iValue *= 3;

					yield[YIELD_PRODUCTION] += min(300, iValue);
				}
			}
		}
	}
	if (PolicyInfo->GetDistressFlatReduction() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromDistress() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetDistressFlatReduction() * 100;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetDistressFlatReduction() * 50;
		}
	}
	if (PolicyInfo->GetPovertyFlatReduction() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromPoverty() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPovertyFlatReduction() * 40;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetPovertyFlatReduction() * 20;
		}
	}
	if (PolicyInfo->GetIlliteracyFlatReduction() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromIlliteracy() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetIlliteracyFlatReduction() * 100;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetIlliteracyFlatReduction() * 50;
		}
	}
	if (PolicyInfo->GetBoredomFlatReduction() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromBoredom() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetBoredomFlatReduction() * 100;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetBoredomFlatReduction() * 50;
		}
	}
	if (PolicyInfo->GetReligiousUnrestFlatReduction() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromReligiousUnrest() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetReligiousUnrestFlatReduction() * 100;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetReligiousUnrestFlatReduction() * 50;
		}
	}
	if (PolicyInfo->GetBasicNeedsMedianModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromDistress() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetBasicNeedsMedianModifier() * -10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetBasicNeedsMedianModifier() * -5;
		}
	}
	if (PolicyInfo->GetGoldMedianModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromPoverty() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetGoldMedianModifier() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetGoldMedianModifier() * -2;
		}
	}
	if (PolicyInfo->GetScienceMedianModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromIlliteracy() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetScienceMedianModifier() * -10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetScienceMedianModifier() * -5;
		}
	}
	if (PolicyInfo->GetCultureMedianModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromBoredom() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCultureMedianModifier() * -10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCultureMedianModifier() * -5;
		}
	}
	if (PolicyInfo->GetReligiousUnrestModifier() != 0)
	{
		if (pPlayerTraits->IsExpansionist() || pPlayer->GetUnhappinessFromReligiousUnrest() > 0)
		{
			yield[YIELD_FOOD] += PolicyInfo->GetReligiousUnrestModifier() * -10;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetReligiousUnrestModifier() * -5;
		}
	}
	if (PolicyInfo->GetBasicNeedsMedianModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetBasicNeedsMedianModifierCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetBasicNeedsMedianModifierCapital() * -2;
		}
	}
	if (PolicyInfo->GetGoldMedianModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetGoldMedianModifierCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetGoldMedianModifierCapital() * -2;
		}
	}
	if (PolicyInfo->GetScienceMedianModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetScienceMedianModifierCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetScienceMedianModifierCapital() * -2;
		}
	}
	if (PolicyInfo->GetCultureMedianModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCultureMedianModifierCapital() * -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetCultureMedianModifierCapital() * -2;
		}
	}
	if (PolicyInfo->GetReligiousUnrestModifierCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetReligiousUnrestModifierCapital() *  -4;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetReligiousUnrestModifierCapital() * -2;
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
			yield[YIELD_FOOD] += PolicyInfo->GetSpecialistFoodChange() * -2 * max(1, (iPopulation / 3));
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetSpecialistFoodChange() * -1 * max(1, (iPopulation / 5));
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
	if (PolicyInfo->GetGarrisonsOccupiedUnhappinessMod() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGarrisonsOccupiedUnhappinessMod() * 10;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetGarrisonsOccupiedUnhappinessMod() * 2;
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
	if (PolicyInfo->GetXPopulationConscription() != 0)
	{
		if (pPlayerTraits->IsExpansionist())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetXPopulationConscription() * 10;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetXPopulationConscription() * 5;
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
	if (PolicyInfo->GetFreePopulationCapital() != 0)
	{
		if (pPlayerTraits->IsSmaller())
		{
			yield[YIELD_FOOD] += PolicyInfo->GetFreePopulationCapital() * 50;
		}
		else
		{
			yield[YIELD_FOOD] += PolicyInfo->GetFreePopulationCapital() * 10;
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
	if (PolicyInfo->GetExtraSupplyFlat() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExtraSupplyFlat() * 5;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetExtraSupplyFlat();
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
	if (PolicyInfo->GetCultureBombBoost() != 0)
	{
		if (pPlayerTraits->IsWarmonger())
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCultureBombBoost() * 250;
		}
		else
		{
			yield[YIELD_GREAT_GENERAL_POINTS] += PolicyInfo->GetCultureBombBoost() * 50;
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
			yield[YIELD_GOLD] += 2000 / PolicyInfo->GetFreeWCVotes();
		}
		else
		{
			yield[YIELD_GOLD] += 400 / PolicyInfo->GetFreeWCVotes();
		}
	}
	if (PolicyInfo->GetSpySecurityModifier() != 0)
	{
		if (pPlayerTraits->IsNerd())
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetSpySecurityModifier() * 2;
		}
		else
		{
			yield[YIELD_SCIENCE] += PolicyInfo->GetSpySecurityModifier();
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
			yield[YIELD_GOLD] += PolicyInfo->GetFreeTradeRoute() * 200;
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
	if (PolicyInfo->GetDoubleBorderGrowthGA() || PolicyInfo->GetDoubleBorderGrowthWLTKD())
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
					int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 10, true, true, true);
					if (iValue > 0)
					{
						if (pkBuildingInfo->GetFaithCost() != 0)
						{
							if (pPlayerTraits->IsReligious())
								iValue *= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							if (iValue <= 0)
								iValue = 0;

							yield[YIELD_FAITH] += min(250, iValue);
						}
						else
						{
							if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
								iValue /= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							yield[YIELD_PRODUCTION] += min(250, iValue);
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
		if (PolicyInfo->GetBuildingClassSecurityChange(eBuildingClass) != 0)
		{
			if (pPlayerTraits->IsNerd())
			{
				yield[YIELD_SCIENCE] += 10 * PolicyInfo->GetBuildingClassSecurityChange(eBuildingClass);
			}
			else
			{
				yield[YIELD_SCIENCE] += 5 * PolicyInfo->GetBuildingClassSecurityChange(eBuildingClass);
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
		if (PolicyInfo->GetFreeChosenBuilding(eBuildingClass) != 0) 
		{
			if (pkBuildingClassInfo)
			{
				const BuildingTypes eBuilding = ((BuildingTypes)(pPlayer->getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));
				if (NO_BUILDING != eBuilding)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
					if (pkBuildingInfo)
					{
						int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, (10 * PolicyInfo->GetFreeChosenBuilding(eBuildingClass)), true, true, true);
						if (iValue > 0)
						{
							if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
								iValue /= 2;

							else if (pkBuildingClassInfo->getMaxGlobalInstances() == 1)
								iValue /= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							if (iValue <= 0)
								iValue = 0;

							yield[YIELD_PRODUCTION] += min(250, iValue);
						}
					}
				}
			}
		}
		if (PolicyInfo->GetAllCityFreeBuilding() == eBuildingClass)
		{
			if (pkBuildingClassInfo)
			{
				const BuildingTypes eBuilding = ((BuildingTypes)(pPlayer->getCivilizationInfo().getCivilizationBuildings(PolicyInfo->GetAllCityFreeBuilding())));
				if (NO_BUILDING != eBuilding)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
					if (pkBuildingInfo)
					{
						int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 15, true, true, true);
						if (iValue > 0)
						{
							if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
								iValue /= 2;

							else if (pkBuildingClassInfo->getMaxGlobalInstances() == 1)
								iValue /= 2;

							iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

							if (iValue <= 0)
								iValue = 0;

							yield[YIELD_PRODUCTION] += min(250, iValue);
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

			if (PolicyInfo->GetYieldChangesPerReligionTimes100(i) != 0)
			{
				yield[eYield] += PolicyInfo->GetYieldChangesPerReligionTimes100(i) * iNumCities / 100;
			}
		}
	}

	// Unit related
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		const UnitTypes eUnit = pPlayer->GetSpecificUnitType(eUnitClass);
		if (eUnit == NO_UNIT)
			continue;

		CvUnitEntry* pUnitEntry = GC.getUnitInfo(eUnit);
		if (!pUnitEntry)
			continue;

		bool bCombat = pUnitEntry->GetCombat() > 0 || pUnitEntry->GetRangedCombat() > 0;
		int iProductionModifier = PolicyInfo->GetUnitClassProductionModifiers(eUnitClass);
		if (iProductionModifier != 0)
		{
			if (bCombat ? pPlayerTraits->IsWarmonger() : pPlayerTraits->IsExpansionist())
				yield[YIELD_GREAT_GENERAL_POINTS] += iProductionModifier * 2;
			else
				yield[YIELD_GREAT_GENERAL_POINTS] += iProductionModifier;
		}

		int iNumFreeUnitsByClass = PolicyInfo->GetNumFreeUnitsByClass(eUnitClass);
		if (iNumFreeUnitsByClass != 0)
		{
			if (bCombat ? pPlayerTraits->IsWarmonger() : pPlayerTraits->IsExpansionist())
				yield[YIELD_GREAT_GENERAL_POINTS] += iNumFreeUnitsByClass * 25;

			if (pUnitEntry->IsFound())
				yield[YIELD_FOOD] += iNumFreeUnitsByClass * (50 - (pPlayer->getNumCities() * 5));
			else if (pUnitEntry->GetWorkRate() > 0)
				yield[YIELD_FOOD] += iNumFreeUnitsByClass * 50 * pPlayer->getNumCities();
			else
				yield[YIELD_GREAT_GENERAL_POINTS] += iNumFreeUnitsByClass * 25;
		}

		int iTourismByUnitClassCreated = PolicyInfo->GetTourismByUnitClassCreated(eUnitClass);
		if (iTourismByUnitClassCreated != 0)
		{
			if (pPlayerTraits->IsTourism())
				yield[YIELD_TOURISM] += iTourismByUnitClassCreated * 5;
			else
				yield[YIELD_TOURISM] += iTourismByUnitClassCreated;
		}

		if (pPlayer->getCapitalCity())
		{
			int iBaseValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, 10, true, true);

			if (PolicyInfo->IsFaithPurchaseUnitClass(eUnitClass, /*INDUSTRIAL*/ GD_INT_GET(RELIGION_GP_FAITH_PURCHASE_ERA)) != 0)
			{
				if (iBaseValue > 0)
				{
					int iValue = iBaseValue;
					if (pPlayerTraits->IsReligious())
						iValue *= 2;

					yield[YIELD_FAITH] += min(225, iValue);
				}
			}

			if (pUnitEntry->GetPolicyType() == ePolicy)
			{
				if (iBaseValue > 0)
				{
					int iValue = iBaseValue;
					if (pPlayerTraits->IsWarmonger())
						iValue *= 2;

					if (pUnitEntry->GetDomainType() == DOMAIN_LAND || pUnitEntry->GetDomainType() == DOMAIN_AIR)
						yield[YIELD_GREAT_GENERAL_POINTS] += min(150, iValue);
					else
						yield[YIELD_GREAT_ADMIRAL_POINTS] += min(150, iValue);
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
				yield[YIELD_GOLD] += PolicyInfo->GetResourceFromCSAlly(eResource) / 10;
			}
			else
			{
				yield[YIELD_GOLD] += PolicyInfo->GetResourceFromCSAlly(eResource) / 15;
			}
		}

		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo != NULL && pkResourceInfo->getPolicyReveal() == ePolicy)
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
					yield[eYield] += PolicyInfo->GetTradeRouteYieldChange(eDomain, eYield) * 3;
				}
				else
				{
					yield[eYield] += PolicyInfo->GetTradeRouteYieldChange(eDomain, eYield);
				}
			}
		}
	}

	SpecialistTypes eSpecialist;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		eSpecialist = (SpecialistTypes)iI;

		int NumSpecialists = 0;

		int iLoopCity = 0;
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

	int iLoopCity = 0;
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
				yield[eYield] += PolicyInfo->GetCityYieldChange(eYield) * 15 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCityYieldChange(eYield) * 10 * iNumCities;
			}
		}
		if (PolicyInfo->GetCoastalCityYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetCoastalCityYieldChange(eYield) * 10 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCoastalCityYieldChange(eYield) * 5 * iNumCities;
			}
		}
		if (PolicyInfo->GetCapitalYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller() || pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldChange(eYield) * 10 * iCapitalPop;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldChange(eYield) * 5 * iCapitalPop;
			}
		}
		if (PolicyInfo->GetCapitalYieldPerPopChange(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += (iCapitalPop * 100) / PolicyInfo->GetCapitalYieldPerPopChange(eYield);
			}
			else
			{
				yield[eYield] += (iCapitalPop * 50) / PolicyInfo->GetCapitalYieldPerPopChange(eYield);
			}
		}
		if (PolicyInfo->GetCapitalYieldPerPopChangeEmpire(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += (iPopulation * 50)  / PolicyInfo->GetCapitalYieldPerPopChangeEmpire(eYield);
			}
			else
			{
				yield[eYield] += (iPopulation * 25) / PolicyInfo->GetCapitalYieldPerPopChangeEmpire(eYield);
			}
		}
		if (PolicyInfo->GetCapitalYieldModifier(eYield) != 0) 
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldModifier(eYield) * iCapitalPop;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetCapitalYieldModifier(eYield) * iCapitalPop / 2;
			}
		}
		if (PolicyInfo->GetGreatWorkYieldChange(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetGreatWorkYieldChange(eYield) * 20 * max(1, pPlayer->GetCulture()->GetNumGreatWorks());
			}
			else
			{
				yield[eYield] += PolicyInfo->GetGreatWorkYieldChange(eYield) * 10 * max(1, pPlayer->GetCulture()->GetNumGreatWorks());
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
				yield[eYield] += PolicyInfo->GetYieldFromBirth(eYield) * 2 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirth(eYield) * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldFromBirthCapital(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapital(eYield) * 2 * iCapitalPop;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapital(eYield) * iCapitalPop;
			}
		}
		if (PolicyInfo->GetYieldFromBirthRetroactive(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthRetroactive(eYield) * 3 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthRetroactive(eYield) * iNumCities;
			}
		}
		if (PolicyInfo->GetYieldFromBirthCapitalRetroactive(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapitalRetroactive(eYield) * 2 * iCapitalPop;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromBirthCapitalRetroactive(eYield) * iCapitalPop;
			}
		}
		if (PolicyInfo->GetYieldFromTechRetroactive(eYield) != 0)
		{
			if (pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromTechRetroactive(eYield) * 2 * iTechnologiesResearched;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromTechRetroactive(eYield) * iTechnologiesResearched;
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
		if (PolicyInfo->GetYieldFromWorldWonderConstruction(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism() || pPlayerTraits->IsSmaller())
			{
				yield[eYield] += PolicyInfo->GetYieldFromWorldWonderConstruction(eYield) * 5 * max(2, (iNumWonders / 2));
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromWorldWonderConstruction(eYield) * max(2, (iNumWonders / 4));
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
			if (pPlayerTraits->IsExpansionist() && pPlayerTraits->GetExtraFoundedCityTerritoryClaimRange() == 0)
			{
				yield[eYield] += PolicyInfo->GetYieldFromBorderGrowth(eYield) * 2 * iNumCities;
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
				yield[eYield] += PolicyInfo->GetGoldenAgeYieldMod(eYield) * 5 * iNumCities;
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
				yield[eYield] += PolicyInfo->GetYieldFromKills(eYield) * 5;
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
				yield[eYield] += PolicyInfo->GetYieldFromBarbarianKills(eYield) * 5;
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
				yield[eYield] += PolicyInfo->GetYieldChangeTradeRoute(eYield) * 5 * iNumCities;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldChangeTradeRoute(eYield) * 2 * iNumCities;
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
				yield[eYield] += PolicyInfo->GetYieldChangeWorldWonder(eYield) * 5 * max(1, pPlayer->GetNumWonders());
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
				yield[eYield] += PolicyInfo->GetArtifactYieldChanges(eYield) * 4;
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
				yield[eYield] += PolicyInfo->GetArtYieldChanges(eYield) * 4;
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
				yield[eYield] += PolicyInfo->GetMusicYieldChanges(eYield) * 4;
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
				yield[eYield] += PolicyInfo->GetLitYieldChanges(eYield) * 4;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetLitYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetFilmYieldChanges(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetFilmYieldChanges(eYield) * 4;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetFilmYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetRelicYieldChanges(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetRelicYieldChanges(eYield) * 4;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetRelicYieldChanges(eYield);
			}
		}
		if (PolicyInfo->GetYieldFromNonSpecialistCitizensTimes100(eYield) != 0)
		{
			if (pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += PolicyInfo->GetYieldFromNonSpecialistCitizensTimes100(eYield) / 10;
			}
			else
			{
				yield[eYield] += PolicyInfo->GetYieldFromNonSpecialistCitizensTimes100(eYield) / 20;
			}
		}
		if (PolicyInfo->GetYieldModifierFromGreatWorks(eYield) != 0)
		{
			if (pPlayerTraits->IsTourism())
			{
				yield[eYield] += PolicyInfo->GetYieldModifierFromGreatWorks(eYield) * 15 * max(1, pPlayer->GetCulture()->GetNumGreatWorks());
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
		if (PolicyInfo->GetYieldPerCityOverStrengthThreshold(eYield) != 0)
		{
			if (pPlayerTraits->IsWarmonger() || pPlayerTraits->IsExpansionist())
			{
				yield[eYield] += iNumCities * PolicyInfo->GetYieldPerCityOverStrengthThreshold(eYield) * 2;
			}
			else
			{
				yield[eYield] += iNumCities * PolicyInfo->GetYieldPerCityOverStrengthThreshold(eYield);
			}
		}
		if (PolicyInfo->GetYieldFromXMilitaryUnits(eYield) != 0)
		{
			// count units
			int iCount = 0;
			int iLoop = 0;
			for (const CvUnit* pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if (pLoopUnit->IsCivilianUnit() || pLoopUnit->isDelayedDeath())
					continue;

				iCount++;
			}
			if (pPlayerTraits->IsWarmonger())
			{
				yield[eYield] += iNumCities * (iCount / PolicyInfo->GetYieldFromXMilitaryUnits(eYield)) * 2;
			}
			else
			{
				yield[eYield] += iNumCities * (iCount / PolicyInfo->GetYieldFromXMilitaryUnits(eYield));
			}
		}
	}
	
	if (yield.size() != 0)
		LogPolicyAttributeYields(pPlayer, ePolicy, yield);

	return yield;
}

int CvPolicyAI::WeighPolicy(CvPlayer* pPlayer, PolicyTypes ePolicy)
{
	if (ePolicy == NO_POLICY || pPlayer == NULL)
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

	int iGrandStrategiesLoop = 0;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy = NULL;
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
							if (pPlayer->GetDiplomacyAI()->GetCivApproach(eMinor) >= CIV_APPROACH_HOSTILE)
							{
								iDiploValue -= iFlavorValue;
								break;
							}
						}
					}
				}
				//War
				if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_OFFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_DEFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_CITY_DEFENSE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MILITARY_TRAINING" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_MOBILE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RANGED" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_ARCHER" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SIEGE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SKIRMISHER")
				{
					iConquestValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIR" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_ANTIAIR" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIR_CARRIER" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_AIRLIFT" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_BOMBER" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_FIGHTER")
				{
					iConquestValue += iFlavorValue;
				}
				else if (GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL_RECON" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_RECON" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL_MELEE" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_NAVAL_RANGED" || GC.getFlavorTypes((FlavorTypes)iFlavor) == "FLAVOR_SUBMARINE")
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

	bool bSeriousMode = pPlayer->GetDiplomacyAI()->IsSeriousAboutVictory();
	bool bConquestFocus = (bSeriousMode && pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest()) || pPlayer->GetPlayerTraits()->IsWarmonger() || pPlayer->GetDiplomacyAI()->IsCloseToWorldConquest();
	bool bDiploFocus = (bSeriousMode && pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory()) || pPlayer->GetPlayerTraits()->IsDiplomat() || pPlayer->GetDiplomacyAI()->IsCloseToDiploVictory();
	bool bScienceFocus = (bSeriousMode && pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory()) || pPlayer->GetPlayerTraits()->IsNerd() || pPlayer->GetDiplomacyAI()->IsCloseToSpaceshipVictory();
	bool bCultureFocus = (bSeriousMode && pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory()) || pPlayer->GetPlayerTraits()->IsTourism() || pPlayer->GetDiplomacyAI()->IsCloseToCultureVictory();

	// And now add them in. Halve if not our main focus.
	iWeight += bConquestFocus ? iConquestValue : iConquestValue / 2;
	iWeight += bDiploFocus ? iDiploValue : iDiploValue / 2;
	iWeight += bScienceFocus ? iScienceValue : iScienceValue / 2;
	iWeight += bCultureFocus ? iCultureValue : iCultureValue / 2;

	//If this is an ideology policy, let's snap those up.
	if (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetLevel() > 0)
	{
		iWeight *= (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetLevel() + 1);
	}
	if (!pPlayer->GetCorporations()->HasFoundedCorporation())
	{
		//Corporate-specific policies should only be taken if you have a corporation.
		if (m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetCorporationOfficesAsFranchises() != 0 
			|| m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetCorporationRandomForeignFranchiseMod() != 0 
			|| m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->GetCorporationFreeFranchiseAbovePopular() != 0
			|| m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->IsNoFranchisesInForeignCities()
			|| m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(ePolicy)->IsNoForeignCorpsInCities())
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
					iWeight /= max(2, ((iPlayerEra * 5) - iPolicyEra));
				}
			}
		}
		if (ePolicyBranch == (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_PATRONAGE", true))
		{
			if (GC.getGame().GetNumMinorCivsAlive() <= 0)
			{
				iWeight = 0;
			}
			else if (pPlayer->GetNumCSFriends() <= 0)
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

	return max(iWeight, 0);
}

/// Priority for opening up this branch
int CvPolicyAI::WeighBranch(CvPlayer* pPlayer, PolicyBranchTypes eBranch)
{
	if (eBranch == NO_POLICY_BRANCH_TYPE || pPlayer == NULL)
		return 0;

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
								int iValue = pPlayer->getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, 50, true, true, true);
								if (iValue > 0)
								{
									if (pkBuildingInfo->GetFaithCost() != 0)
									{
										if (pPlayer->GetPlayerTraits()->IsReligious())
											iValue *= 2;
										else
											iValue /= 2;

										iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

										if (iValue <= 0)
											iValue = 0;

										iWeight += min(250, iValue);
									}
									else
									{
										if (pkBuildingInfo->IsCapitalOnly() && !pPlayer->GetPlayerTraits()->IsSmaller())
											iValue /= 2;
										else if (pkBuildingClassInfo->getMaxGlobalInstances() == 1)
											iValue /= 2;

										iValue -= pPlayer->getNumBuildings(eBuilding) * 10;

										if (iValue <= 0)
											iValue = 0;

										iWeight += min(250, iValue);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return max(iWeight, 0);
}

/// Based on game options (religion off, science off, etc.), would this branch do us any good?
bool CvPolicyAI::IsBranchEffectiveInGame(PolicyBranchTypes eBranch)
{
	CvPolicyBranchEntry* pBranchInfo = GC.getPolicyBranchInfo(eBranch);
	ASSERT_DEBUG(pBranchInfo, "Branch info not found!");
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

		FILogFile* pLog = NULL;
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

		FILogFile* pLog = NULL;
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

		FILogFile* pLog = NULL;
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

		FILogFile* pLog = NULL;
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

		FILogFile* pLog = NULL;
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
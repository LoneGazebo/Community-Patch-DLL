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
CvPolicyAI::CvPolicyAI (CvPlayerPolicies *currentPolicies):
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
	if (m_pCurrentPolicies != NULL)
	{
		CvPolicyXMLEntries* pPolicyEntries = m_pCurrentPolicies->GetPolicies();
		CvAssertMsg(pPolicyEntries != NULL, "Policy AI init failure: no policy data");
		if (pPolicyEntries != NULL)
		{
			// Loop through reading each one and add an entry with 0 weight to our vector
			const int nPolicyEntries = pPolicyEntries->GetNumPolicies();
			for (int i = 0; i < nPolicyEntries; i++)
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
	for (uint uiIndex = 0; uiIndex < uiPolicyArraySize; ++uiIndex)
		m_PolicyAIWeights.SetWeight(uiIndex, 0);

	if (uiVersion >= 2)
	{
		uint uiPolicyCount;
		kStream >> uiPolicyCount;

		for (uint uiIndex = 0; uiIndex < uiPolicyCount; ++uiIndex)
		{
			PolicyTypes ePolicy = (PolicyTypes)CvInfosSerializationHelper::ReadHashed(kStream);
			kStream >> iWeight;
			if (ePolicy != NO_POLICY && (uint)ePolicy < uiPolicyArraySize)
				m_PolicyAIWeights.SetWeight((uint)ePolicy, iWeight);
		}
	}
	else
	{
		uint uiPolicyCount = sizeof(CvInfosSerializationHelper::ms_V0PolicyTags) / sizeof(const char*);
		// Loop through reading each one and adding it to our vector
		for (uint i = 0; i < uiPolicyCount; i++)
		{
			PolicyTypes ePolicy = (PolicyTypes)CvInfosSerializationHelper::ConvertV0(static_cast<PolicyTypes>(i), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0PolicyTags) );
			kStream >> iWeight;
			if (ePolicy != NO_POLICY && (uint)ePolicy < uiPolicyArraySize)
				m_PolicyAIWeights.SetWeight((uint)ePolicy, iWeight);
		}
	}
}

/// Serialization write
void CvPolicyAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

	CvAssertMsg(m_pCurrentPolicies->GetPolicies() != NULL, "Policy AI serialization failure: no policy data");
	CvAssertMsg(m_pCurrentPolicies->GetPolicies()->GetNumPolicies() > 0, "Policy AI serialization failure: number of policies not greater than 0");

	// Loop through writing each entry
	uint uiPolicyCount = m_pCurrentPolicies->GetPolicies()->GetNumPolicies();
	kStream << uiPolicyCount;

	for (int i = 0; i < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); i++)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, static_cast<const PolicyTypes>(i));
		kStream << m_PolicyAIWeights.GetWeight(i);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvPolicyAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent)
{
	int iPolicy;
	CvPolicyEntry *entry;
	int *paiTempWeights;

	CvPolicyXMLEntries* pkPolicyEntries = m_pCurrentPolicies->GetPolicies();
	// Create a temporary array of weights
	paiTempWeights = (int*)_alloca( sizeof( int* ) * pkPolicyEntries->GetNumPolicies() );

	// Loop through all our policies
	for (iPolicy = 0; iPolicy < pkPolicyEntries->GetNumPolicies(); iPolicy++)
	{
		entry = pkPolicyEntries->GetPolicyEntry(iPolicy);

		// Set its weight by looking at policy's weight for this flavor and using iWeight multiplier passed in
		if (entry)
			paiTempWeights[iPolicy] = entry->GetFlavorValue(eFlavor) * iWeight;
		else
			paiTempWeights[iPolicy] = 0;
	}

	// Propagate these values left in the tree so prereqs get bought
	if (iPropagationPercent > 0)
	{
		WeightPrereqs(paiTempWeights, iPropagationPercent);
	}

	// Add these weights over previous ones
	for (iPolicy = 0; iPolicy < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicy++)
	{
		m_PolicyAIWeights.IncreaseWeight(iPolicy, paiTempWeights[iPolicy]);
	}
}

/// Choose a player's next policy purchase (could be opening a branch)
int CvPolicyAI::ChooseNextPolicy(CvPlayer *pPlayer)
{
	RandomNumberDelegate fcn;
	fcn = MakeDelegate (&GC.getGame(), &CvGame::getJonRandNum);
	int iRtnValue = (int)NO_POLICY;
	int iPolicyLoop;

	// Create a new vector holding only policies we can currently adopt
	m_AdoptablePolicies.clear();

	// Loop through adding the adoptable policies
	for (iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		if (m_pCurrentPolicies->CanAdoptPolicy((PolicyTypes) iPolicyLoop))
		{
			m_AdoptablePolicies.push_back(iPolicyLoop + GC.getNumPolicyBranchInfos(), m_PolicyAIWeights.GetWeight(iPolicyLoop));

			// Does this policy finish a branch for us?
			if (m_pCurrentPolicies->WillFinishBranchIfAdopted((PolicyTypes) iPolicyLoop))
			{
				int iPolicyBranch = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicyLoop)->GetPolicyBranchType();
				if (iPolicyBranch != NO_POLICY_BRANCH_TYPE)
				{
					int iFinisherPolicy = m_pCurrentPolicies->GetPolicies()->GetPolicyBranchEntry(iPolicyBranch)->GetFreeFinishingPolicy();
					if (iFinisherPolicy != NO_POLICY)
					{
						int iFinisherWeight = m_PolicyAIWeights.GetWeight(iFinisherPolicy);
						int iIndex = m_AdoptablePolicies.size() - 1;

						// Add in the weight for the branch finisher
						m_AdoptablePolicies.SetWeight(iIndex, (m_AdoptablePolicies.GetWeight(iIndex) + iFinisherWeight));
					}
				}
			}
		}
	}

	// Loop though the branches adding each as another possibility
	bool bStartedAMutuallyExclusiveBranch = false;
	for (int iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
	{
		const PolicyBranchTypes ePolicyBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
		if(pkPolicyBranchInfo)
		{
			if (pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
			{
				if (pkPolicyBranchInfo->IsMutuallyExclusive())
				{
					bStartedAMutuallyExclusiveBranch = true;
				}
			}
		}
	}

	AIGrandStrategyTypes eCultureGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	AIGrandStrategyTypes eCurrentGrandStrategy = pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy();

	for (int iBranchLoop = 0; iBranchLoop < GC.getNumPolicyBranchInfos(); iBranchLoop++)
	{
		const PolicyBranchTypes ePolicyBranch = static_cast<PolicyBranchTypes>(iBranchLoop);
		CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(ePolicyBranch);
		if(pkPolicyBranchInfo)
		{
			if (bStartedAMutuallyExclusiveBranch && pkPolicyBranchInfo->IsMutuallyExclusive())
			{
				continue;
			}

			if (pPlayer->GetPlayerPolicies()->CanUnlockPolicyBranch(ePolicyBranch) && !pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(ePolicyBranch))
			{
				int iBranchWeight = WeighBranch(ePolicyBranch);
				iBranchWeight *= (100 - m_iPolicyWeightPercentDropNewBranch);
				iBranchWeight /= 100;

				if (eCurrentGrandStrategy == eCultureGrandStrategy)
				{
					iBranchWeight /= 3;
				}

				m_AdoptablePolicies.push_back(iBranchLoop, iBranchWeight);
			}
		}
	}

	m_AdoptablePolicies.SortItems();
	LogPossiblePolicies();

	// If total weight is above 0, choose one above a threshold
	if (m_AdoptablePolicies.GetTotalWeight() > 0)
	{
		int iNumChoices = GC.getGame().getHandicapInfo().GetPolicyNumOptions();
		iRtnValue = m_AdoptablePolicies.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing policy from Top Choices");

		if (iRtnValue >= GC.getNumPolicyBranchInfos())
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

//=====================================
// PRIVATE METHODS
//=====================================
/// Add weights to policies that are prereqs for the ones already weighted in this strategy
void CvPolicyAI::WeightPrereqs (int *paiTempWeights, int iPropagationPercent)
{
	int iPolicyLoop;

	// Loop through policies looking for ones that are just getting some new weight
	for (iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
	{
		// If found one, call our recursive routine to weight everything to the left in the tree
		if (paiTempWeights[iPolicyLoop] > 0)
		{
			PropagateWeights(iPolicyLoop, paiTempWeights[iPolicyLoop], iPropagationPercent, 0);
		}
	}
}

/// Recursive routine to weight all prerequisite policies
void CvPolicyAI::PropagateWeights(int iPolicy, int iWeight, int iPropagationPercent, int iPropagationLevel)
{
	if (iPropagationLevel < m_iPolicyWeightPropagationLevels)
	{
		int iPropagatedWeight = iWeight * iPropagationPercent / 100;

		// Loop through all prerequisites
		for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
		{
			// Did we find a prereq?
			int iPrereq = m_pCurrentPolicies->GetPolicies()->GetPolicyEntry(iPolicy)->GetPrereqAndPolicies(iI);
			if (iPrereq != NO_POLICY)
			{
				// Apply reduced weight here.  Note that we apply these to the master weight array, not
				// the temporary one.  The temporary one is just used to hold the newly weighted policies
				// (from which this weight propagation must originate).
				m_PolicyAIWeights.IncreaseWeight (iPrereq, iPropagatedWeight);

				// Recurse to its prereqs (assuming we have any weight left)
				if (iPropagatedWeight > 0)
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
int CvPolicyAI::WeighBranch (PolicyBranchTypes eBranch)
{
	int iWeight = 0;

	CvPolicyBranchEntry* pkPolicyBranchInfo = GC.getPolicyBranchInfo(eBranch);
	if(pkPolicyBranchInfo)
	{
		for (int iPolicyLoop = 0; iPolicyLoop < m_pCurrentPolicies->GetPolicies()->GetNumPolicies(); iPolicyLoop++)
		{
			const PolicyTypes ePolicyLoop = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkLoopPolicyInfo = GC.getPolicyInfo(ePolicyLoop);
			if(pkLoopPolicyInfo)
			{
				// Policy we don't have?
				if (!m_pCurrentPolicies->HasPolicy(ePolicyLoop))
				{
					// From this branch we are considering opening?
					if (pkLoopPolicyInfo->GetPolicyBranchType() == eBranch)
					{
						// With no prereqs?
						if (pkLoopPolicyInfo->GetPrereqAndPolicies(0) == NO_POLICY)
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

/// Log all possible policy choices
void CvPolicyAI::LogPossiblePolicies()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		int iNumBranches = GC.getNumPolicyBranchInfos();

		// Dump out the weight of each possible policy
		for (int iI = 0; iI < m_AdoptablePolicies.size(); iI++)
		{
			int iWeight = m_AdoptablePolicies.GetWeight(iI);

			if (m_AdoptablePolicies.GetElement(iI) < iNumBranches)
			{
				strTemp.Format ("Branch %d, %d", m_AdoptablePolicies.GetElement(iI), iWeight);
			}
			else
			{

				PolicyTypes ePolicy = (PolicyTypes)(m_AdoptablePolicies.GetElement(iI) - iNumBranches);
				CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
				const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetType() : "Unknown";
				strTemp.Format ("%s, %d", szPolicyType, iWeight);
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log chosen policy
void CvPolicyAI::LogPolicyChoice(PolicyTypes ePolicy)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvPolicyEntry* pPolicyEntry = GC.getPolicyInfo(ePolicy);
		const char* szPolicyType = (pPolicyEntry != NULL)? pPolicyEntry->GetType() : "Unknown";
		strTemp.Format ("CHOSEN, %s", szPolicyType);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Log chosen policy
void CvPolicyAI::LogBranchChoice(PolicyBranchTypes eBranch)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pCurrentPolicies->GetPlayer()->getCivilizationShortDescription();

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		strTemp.Format ("CHOSEN, Branch %d", eBranch);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}
/// Build log filename
CvString CvPolicyAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if (GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PolicyAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PolicyAILog.csv";
	}

	return strLogName;
}
/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvTechAI.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#if defined(MOD_BALANCE_CORE)
#include "CvEconomicAI.h"
#endif

#include "LintFree.h"

/// Constructor
CvTechAI::CvTechAI(CvPlayerTechs* currentTechs):
	m_pCurrentTechs(currentTechs)
{
}

/// Destructor
CvTechAI::~CvTechAI(void)
{
}

/// Clear out AI local variables
void CvTechAI::Reset()
{
	CvAssertMsg(m_pCurrentTechs != NULL, "Tech AI init failure: player tech data is NULL");
	if(m_pCurrentTechs != NULL)
	{
		CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI init failure: no tech data");

		m_TechAIWeights.clear();

		// Loop through reading each one and add an entry with 0 weight to our vector
		for(int i = 0; i < m_pCurrentTechs->GetTechs()->GetNumTechs(); i++)
		{
			m_TechAIWeights.push_back(i, 0);
		}
	}
}

extern const char* ms_V0ExpansionTechTags[81];
/// Serialization read
void CvTechAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int iWeight;

	CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI serialization failure: no tech data");
	CvAssertMsg(m_pCurrentTechs->GetTechs()->GetNumTechs() > 0, "Tech AI serialization failure: number of techs not greater than 0");

	// Reset vector
	m_TechAIWeights.clear();
	int iTechCount = m_pCurrentTechs->GetTechs()->GetNumTechs();
	m_TechAIWeights.resize(iTechCount);
	for(int i = 0; i < iTechCount; ++i)
		m_TechAIWeights.SetWeight(i, 0);

	int iCount;
	kStream >> iCount;

	for(int i = 0; i < iCount; i++)
	{
		int iIndex = CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> iWeight;
		if(iIndex >= 0 && iIndex < iCount)
			m_TechAIWeights.SetWeight(iIndex, iWeight);
	}
}

/// Serialization write
void CvTechAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI serialization failure: no tech data");
	CvAssertMsg(m_pCurrentTechs->GetTechs()->GetNumTechs() > 0, "Tech AI serialization failure: number of techs not greater than 0");

	uint uiCount = m_pCurrentTechs->GetTechs()->GetNumTechs();
	kStream << uiCount;

	// Loop through writing each entry
	for(uint i = 0; i < uiCount; i++)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, (TechTypes)i);	// Write out the hash ID first
		kStream << m_TechAIWeights.GetWeight(i);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvTechAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent)
{
	if (iWeight==0)
		return;

	// Loop through all our techs
	for(int iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTech);
		CvTechEntry* entry = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
		if(entry)
		{			
			// Set its weight by looking at tech's weight for this flavor and using iWeight multiplier passed in
			int iTechWeight = entry->GetFlavorValue(eFlavor) * iWeight;

			// Multiply the weight by any special player-specific weighting (i.e. to prioritize civ unique bonuses)
			iTechWeight += m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetCivTechPriority(eTech);

			// Multiply the weight by any locale-specific weighting (i.e. to prioritize unlocking resources)
			iTechWeight += m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetLocaleTechPriority(eTech);

			// Multiply the weight by any locale-specific weighting (i.e. to prioritize unlocking grand strategy stuff)
			iTechWeight += m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetGSTechPriority(eTech);

			if(entry->IsAllowsEmbarking())
			{
				EconomicAIStrategyTypes eStrategyIslandStart = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ISLAND_START");
				if(eStrategyIslandStart != NO_ECONOMICAISTRATEGY)
				{
					if(m_pCurrentTechs->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eStrategyIslandStart))
					{
						iTechWeight += 10;
					}
				}
			}

			// Apply and propagate to prereqs
			PropagateWeights(iTech, iTechWeight, iPropagationPercent, 0);
		}
	}
}


/// Choose a player's next tech research project
TechTypes CvTechAI::ChooseNextTech(CvPlayer *pPlayer, bool bFreeTech)
{
	if (pPlayer->isMinorCiv())
		return NO_TECH;

	TechTypes rtnValue = NO_TECH;

	// Use the synchronous random number generate
	// Asynchronous one would be:
	//	fcn = MakeDelegate (&GC.getGame(), &CvGame::getAsyncRandNum);
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);

	// Create a new vector holding only techs we can currently research
	m_ResearchableTechs.clear();

	// Loop through adding the researchable techs
	for(int iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		if(m_pCurrentTechs->CanResearch((TechTypes)iTechLoop))
		{
			// For free techs, need an additional check
			if(bFreeTech)
			{
				if(m_pCurrentTechs->CanResearchForFree((TechTypes)iTechLoop))
				{
					m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
				}
			}
			else
			{
				m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
			}
		}
	}

	// Reweight our possible choices by their cost, but only if cost is actually a factor!
#if defined(MOD_AI_SMART_V3)
	if (MOD_AI_SMART_V3)
		ReweightByCost(pPlayer, bFreeTech);
	else
#endif
	if(!bFreeTech)
		ReweightByCost(pPlayer);

	m_ResearchableTechs.SortItems();
	LogPossibleResearch();

	// If total weight is above 0, choose one above a threshold
	if(m_ResearchableTechs.GetTotalWeight() > 0)
	{
		int iNumChoices =GC.getGame().getHandicapInfo().GetTechNumOptions();
		if (pPlayer->isBarbarian())
		{
			rtnValue = (TechTypes)m_ResearchableTechs.GetElement(0);
		}
		else
		{
			rtnValue = (TechTypes)m_ResearchableTechs.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing tech from Top Choices");
		}
		LogResearchChoice(rtnValue);
	}

	return rtnValue;
}

/// Choose a player's next tech research project
TechTypes CvTechAI::RecommendNextTech(CvPlayer *pPlayer, TechTypes eIgnoreTech /* = NO_TECH */)
{
	TechTypes rtnValue = NO_TECH;

	// Create a new vector holding only techs we can currently research
	m_ResearchableTechs.clear();

	// Loop through adding the researchable techs
	for(int iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		//if (m_pCurrentTechs->CanResearch((TechTypes) iTechLoop) &&
		//	iTechLoop != eIgnoreTech &&
		//	m_pCurrentTechs->GetTechs()->GetEntry(iTechLoop)->GetAdvisorType() != eIgnoreAdvisor)
		if(m_pCurrentTechs->CanResearch((TechTypes) iTechLoop) && iTechLoop != eIgnoreTech)
		{
			m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
		}
	}

	ReweightByCost(pPlayer);
	m_ResearchableTechs.SortItems();
	LogPossibleResearch();

	// If total weight is above 0, choose top choice as recommendation
	if(m_ResearchableTechs.GetTotalWeight() > 0)
	{

		rtnValue = (TechTypes) m_ResearchableTechs.GetElement(0);
		LogResearchChoice(rtnValue);
	}

	return rtnValue;
}

/// slewis'd!!
int CvTechAI::GetWeight(TechTypes eTech)
{
	return m_TechAIWeights.GetWeight(eTech);
}

struct LeaderWithNumTechs
{
	int m_iLeader;
	int m_iNumTechs;
};

struct LeaderWithNumTechsEval
{
	bool operator()(LeaderWithNumTechs const& a, LeaderWithNumTechs const& b) const
	{
		return a.m_iNumTechs > b.m_iNumTechs;
	}
};

float CvTechAI::GetTechRatio()
{
	// create list of all civs with the number of techs they control
	std::vector<LeaderWithNumTechs> aLeaderWithNumTechs;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		LeaderWithNumTechs kEvalChunk;
		kEvalChunk.m_iLeader = ui;
		kEvalChunk.m_iNumTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		aLeaderWithNumTechs.push_back(kEvalChunk);
	}

	std::stable_sort(aLeaderWithNumTechs.begin(), aLeaderWithNumTechs.end(), LeaderWithNumTechsEval());

	int iPlayerIndexInList = -1;
	for(uint ui = 0; ui < aLeaderWithNumTechs.size(); ui++)
	{
		if(aLeaderWithNumTechs[ui].m_iLeader == m_pCurrentTechs->GetPlayer()->GetID())
		{
			iPlayerIndexInList = ui;
			break;
		}
	}

	CvAssertMsg(iPlayerIndexInList != -1, "Could not find player in list");
	CvAssertMsg(aLeaderWithNumTechs.size() >= 2, "Only one player in the game? Huh?");
	float fTechPositionRatio = 0.0f;
	if(aLeaderWithNumTechs.size() >= 2)
	{
		int iNumerator = iPlayerIndexInList;
		int iDenominator = aLeaderWithNumTechs.size() - 1;
		fTechPositionRatio = iNumerator / (float)iDenominator;
	}

	return fTechPositionRatio;
}

//=====================================
// PRIVATE METHODS
//=====================================

/// Recursive routine to weight all prerequisite techs
void CvTechAI::PropagateWeights(int iTech, int iWeight, int iPropagationPercent, int iPropagationLevel)
{
	if (iWeight == 0)
		return;

	//first apply the weight to the tech itself
	m_TechAIWeights.IncreaseWeight(iTech, iWeight);

	//then see if we have prerequites to take care of
	if (iPropagationLevel >= GC.getTECH_WEIGHT_PROPAGATION_LEVELS())
		return;

	CvTechEntry* pkTechInfo = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
	if (!pkTechInfo)
		return;

	// Loop through all prerequisites
	vector<int> prereqTechs;
	for(int iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		// Did we find a prereq?
		int iPrereq = pkTechInfo->GetPrereqAndTechs(iI);
		if (iPrereq == NO_TECH)
			break;

		prereqTechs.push_back(iPrereq);
	}

	//nothing to do
	if (prereqTechs.empty())
		return;

	//split it evenly
	int iPropagatedWeight = (iWeight * iPropagationPercent) / 100;
	if (!pkTechInfo->IsRepeat())
		iPropagatedWeight /= prereqTechs.size();

	//next level of recursion
	for (size_t i = 0; i < prereqTechs.size(); i++)
		PropagateWeights(prereqTechs[i], iPropagatedWeight, iPropagationPercent, iPropagationLevel++);
}

/// Recompute weights taking into account tech cost
#if defined(MOD_AI_SMART_V3)
void CvTechAI::ReweightByCost(CvPlayer *pPlayer, bool bWantsExpensive)
#else
void CvTechAI::ReweightByCost(CvPlayer *pPlayer)
#endif
{
	TechTypes eTech;

	// April 2014 Balance Patch: if lots of science overflow, want to pick an expensive tech
	bool bNeedExpensiveTechs = pPlayer->getOverflowResearchTimes100() > (pPlayer->GetScienceTimes100() * 2);

#if defined(MOD_AI_SMART_V3)
	if (MOD_AI_SMART_V3 && bWantsExpensive)
	{
		bNeedExpensiveTechs = true;
	}
#endif

	for (int iI = 0; iI < m_ResearchableTechs.size(); iI++)
	{
		eTech = (TechTypes)m_ResearchableTechs.GetElement(iI);
		int iTurnsLeft = 0;

		iTurnsLeft = m_pCurrentTechs->GetResearchTurnsLeft(eTech, true);

		double fWeightDivisor;

		// 10 turns will add 0.02; 80 turns will add 0.16
		double fAdditionalTurnCostFactor = GC.getAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT() * iTurnsLeft;	// 0.015
		double fTotalCostFactor = GC.getAI_RESEARCH_WEIGHT_BASE_MOD() + fAdditionalTurnCostFactor;	// 0.15

		fWeightDivisor = pow((double)iTurnsLeft, fTotalCostFactor);

		int iNewWeight;
		if (bNeedExpensiveTechs)
		{
			iNewWeight = int(double(m_ResearchableTechs.GetWeight(iI)) * fWeightDivisor);
		}
		else
		{
			iNewWeight = int(double(m_ResearchableTechs.GetWeight(iI)) / fWeightDivisor);
		}

		if (pPlayer->GetPlayerTraits()->IsPermanentYieldsDecreaseEveryEra())
		{
			if(GC.getTechInfo(eTech)->GetEra() > pPlayer->GetCurrentEra())
			{
				iNewWeight /= 2;
			}
		}

		if (iNewWeight > 10000)
			iNewWeight = 10000;

		if (iNewWeight < 1)
			iNewWeight = 1;

		// Now actually change the weight
		m_ResearchableTechs.SetWeight(iI, iNewWeight);
	}
}

/// Log all possible tech choices
void CvTechAI::LogPossibleResearch()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;

		// Find the name of this civ
		playerName = m_pCurrentTechs->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Dump out the weight of each Researchable Tech
		for(int iI = 0; iI < m_ResearchableTechs.size(); iI++)
		{
			TechTypes eTech = (TechTypes) m_ResearchableTechs.GetElement(iI);
			int iWeight = m_ResearchableTechs.GetWeight(iI);

			CvTechEntry* pTechEntry = GC.getTechInfo(eTech);
			const char* szTechType = (pTechEntry != NULL)? pTechEntry->GetType() : "Unknown Tech";

			strTemp.Format("%s, %d", szTechType, iWeight);

			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log chosen tech
void CvTechAI::LogResearchChoice(TechTypes eTech)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString playerName;
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString strDesc;

		// Find the name of this civ
		playerName = m_pCurrentTechs->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvTechEntry* pTechEntry = GC.getTechInfo(eTech);
		const char* szTechType = (pTechEntry != NULL)? pTechEntry->GetType() : "Unknown Tech";

		int iNumTurns = m_pCurrentTechs->GetResearchTurnsLeft(eTech, true);

		strTemp.Format("CHOSEN, %s, TURNS: %d", szTechType, iNumTurns);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Log chosen tech
void CvTechAI::LogResearchCompleted(TechTypes eTech)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString playerName;
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString strDesc;

		// Find the name of this civ
		playerName = m_pCurrentTechs->GetPlayer()->getCivilizationShortDescription();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvTechEntry* pTechEntry = GC.getTechInfo(eTech);
		const char* szTechType = (pTechEntry != NULL) ? pTechEntry->GetType() : "Unknown Tech";

		strTemp.Format("COMPLETED, %s", szTechType);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvTechAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "TechAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "TechAILog.csv";
	}

	return strLogName;
}

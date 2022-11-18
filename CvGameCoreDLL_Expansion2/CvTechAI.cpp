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

///
template<typename TechAI, typename Visitor>
void CvTechAI::Serialize(TechAI& techAI, Visitor& visitor)
{
	visitor(techAI.m_TechAIWeights);
}

/// Serialization read
void CvTechAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvTechAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator<<(FDataStream& saveTo, const CvTechAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvTechAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
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

		if (m_pCurrentTechs->GetPlayer()->HasTech(eTech))
			continue;

		CvTechEntry* entry = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
		if(entry)
		{			
			// Ignore techs in the far future
			if (entry->GetEra() - m_pCurrentTechs->GetPlayer()->GetCurrentEra() > GD_INT_GET(NUM_OR_TECH_PREREQS))
				continue;

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
				if(m_pCurrentTechs->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eStrategyIslandStart))
				{
					iTechWeight += 10;
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
	if (!pPlayer->isMajorCiv())
		return NO_TECH;

	TechTypes rtnValue = NO_TECH;

	// Use the synchronous random number generate
	// Asynchronous one would be:
	//	fcn = MakeDelegate (&GC.getGame(), &CvGame::getAsyncRandNum);
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);

	// Create a new vector holding only techs we can currently research
	m_ResearchableTechs.clear();

	// Loop through adding the researchable techs
	for (int iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		if (m_pCurrentTechs->CanResearch((TechTypes)iTechLoop))
		{
			// For free techs, need an additional check
			if (bFreeTech)
			{
				if (m_pCurrentTechs->CanResearchForFree((TechTypes)iTechLoop))
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
	ReweightByCost(pPlayer, bFreeTech);

	m_ResearchableTechs.SortItems();
	LogPossibleResearch();

	// Make and log our tech choice
	if (m_ResearchableTechs.size() > 0)
	{
		rtnValue = (TechTypes)m_ResearchableTechs.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getTechChoiceCutoffThreshold(), &fcn, "Choosing tech from Top Choices");
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
	if (iPropagationLevel >= /*3*/ GD_INT_GET(TECH_WEIGHT_PROPAGATION_LEVELS))
		return;

	CvTechEntry* pkTechInfo = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
	if (!pkTechInfo)
		return;

	// Loop through all prerequisites
	int iPrereqCount = 0;
	for(int iI = 0; iI < /*6*/ GD_INT_GET(NUM_AND_TECH_PREREQS); iI++)
	{
		// Did we find a prereq?
		int iPrereq = pkTechInfo->GetPrereqAndTechs(iI);
		if (iPrereq == NO_TECH)
			break;

		iPrereqCount++;
	}

	//nothing to do
	if (iPrereqCount==0)
		return;

	//split it evenly
	int iPropagatedWeight = (iWeight * iPropagationPercent) / 100;
	if (!pkTechInfo->IsRepeat())
		iPropagatedWeight /= iPrereqCount;

	//next level of recursion
	for (int iI = 0; iI < /*6*/ GD_INT_GET(NUM_AND_TECH_PREREQS); iI++)
	{
		// Did we find a prereq?
		int iPrereq = pkTechInfo->GetPrereqAndTechs(iI);
		if (iPrereq != NO_TECH)
			PropagateWeights(iPrereq, iPropagatedWeight, iPropagationPercent, iPropagationLevel++);
	}
}

/// Recompute weights taking into account tech cost
void CvTechAI::ReweightByCost(CvPlayer *pPlayer, bool bWantsExpensive)
{
	TechTypes eTech;

	// April 2014 Balance Patch: if lots of science overflow, want to pick an expensive tech
	bool bNeedExpensiveTechs = pPlayer->getOverflowResearchTimes100() > (pPlayer->GetScienceTimes100() * 2);

	if (bWantsExpensive)
	{
		bNeedExpensiveTechs = true;
	}

	for (int iI = 0; iI < m_ResearchableTechs.size(); iI++)
	{
		eTech = (TechTypes)m_ResearchableTechs.GetElement(iI);
		int iTurnsLeft = m_pCurrentTechs->GetResearchTurnsLeft(eTech, true);

		//reweight by turns left
		double fTotalCostFactor = /*0.2f*/ GD_FLOAT_GET(AI_RESEARCH_WEIGHT_BASE_MOD) + (iTurnsLeft * /*0.035f*/ GD_FLOAT_GET(AI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT));
		double fWeightDivisor = pow((double)iTurnsLeft, fTotalCostFactor);

		int iNewWeight = 0;
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

		FILogFile* pLog = NULL;
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

		FILogFile* pLog = NULL;
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

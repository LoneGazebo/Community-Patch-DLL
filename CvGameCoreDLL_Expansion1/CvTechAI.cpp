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

	int iWeight;

	CvAssertMsg(m_pCurrentTechs->GetTechs() != NULL, "Tech AI serialization failure: no tech data");
	CvAssertMsg(m_pCurrentTechs->GetTechs()->GetNumTechs() > 0, "Tech AI serialization failure: number of techs not greater than 0");

	// Reset vector
	m_TechAIWeights.clear();
	int iTechCount = m_pCurrentTechs->GetTechs()->GetNumTechs();
	m_TechAIWeights.resize(iTechCount);
	for(int i = 0; i < iTechCount; ++i)
		m_TechAIWeights.SetWeight(i, 0);

	if(uiVersion >= 2)
	{
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
	else
	{
		int iCount = CVINFO_V0_TAG_COUNT(ms_V0ExpansionTechTags);

		// Loop through reading each one and adding it to our vector
		for(int i = 0; i < iCount; i++)
		{
			kStream >> iWeight;
			int iNewIndex = CvInfosSerializationHelper::ConvertV0(i, CVINFO_V0_TAGS(ms_V0ExpansionTechTags));
			if(iNewIndex >= 0 && iNewIndex < iTechCount)
				m_TechAIWeights.SetWeight(iNewIndex, iWeight);
		}
	}
}

/// Serialization write
void CvTechAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

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
	int* paiTempWeights;

	// Create a temporary array of weights
	paiTempWeights = (int*)_alloca(sizeof(int) * m_pCurrentTechs->GetTechs()->GetNumTechs());

	// Loop through all our techs
	for(int iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTech);
		CvTechEntry* entry = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
		if(entry)
		{
			// Set its weight by looking at tech's weight for this flavor and using iWeight multiplier passed in
			paiTempWeights[iTech] = entry->GetFlavorValue(eFlavor) * iWeight;

			// Multiply the weight by any special player-specific weighting (i.e. to prioritize civ unique bonuses)
			paiTempWeights[iTech] *= m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetCivTechPriority(eTech);

			// Multiply the weight by any locale-specific weighting (i.e. to prioritize unlocking resources)
			paiTempWeights[iTech] *= m_pCurrentTechs->GetPlayer()->GetPlayerTechs()->GetLocaleTechPriority(eTech);
		}
	}

	// Propagate these values left in the tree so prereqs get bought
	WeightPrereqs(paiTempWeights, iPropagationPercent);

	// Add these weights over previous ones
	for(int iTech = 0; iTech < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTech++)
	{
		CvTechEntry* entry = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
		if(entry)
		{
			m_TechAIWeights.IncreaseWeight(iTech, paiTempWeights[iTech]);
		}
	}
}


/// Choose a player's next tech research project
TechTypes CvTechAI::ChooseNextTech(bool bFreeTech)
{
	RandomNumberDelegate fcn;
	TechTypes rtnValue = NO_TECH;
	int iTechLoop;

	// Use the synchronous random number generate
	// Asynchronous one would be:
	//	fcn = MakeDelegate (&GC.getGame(), &CvGame::getAsyncRandNum);
	fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);

	// Create a new vector holding only techs we can currently research
	m_ResearchableTechs.clear();

	// Loop through adding the researchable techs
	for(iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
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
	if(!bFreeTech)
		ReweightByCost();

	m_ResearchableTechs.SortItems();
	LogPossibleResearch();

	// If total weight is above 0, choose one above a threshold
	if(m_ResearchableTechs.GetTotalWeight() > 0)
	{
		int iNumChoices =GC.getGame().getHandicapInfo().GetTechNumOptions();
		rtnValue = (TechTypes) m_ResearchableTechs.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing tech from Top Choices");
		LogResearchChoice(rtnValue);
	}

	return rtnValue;
}

/// Choose a player's next tech research project
TechTypes CvTechAI::RecommendNextTech(TechTypes eIgnoreTech /* = NO_TECH */)
{
	TechTypes rtnValue = NO_TECH;
	int iTechLoop;

	// Create a new vector holding only techs we can currently research
	m_ResearchableTechs.clear();

	// Loop through adding the researchable techs
	for(iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		//if (m_pCurrentTechs->CanResearch((TechTypes) iTechLoop) &&
		//	iTechLoop != eIgnoreTech &&
		//	m_pCurrentTechs->GetTechs()->GetEntry(iTechLoop)->GetAdvisorType() != eIgnoreAdvisor)
		if(m_pCurrentTechs->CanResearch((TechTypes) iTechLoop) && iTechLoop != eIgnoreTech)
		{
			m_ResearchableTechs.push_back(iTechLoop, m_TechAIWeights.GetWeight(iTechLoop));
		}
	}

	ReweightByCost();
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

//=====================================
// PRIVATE METHODS
//=====================================
/// Add weights to techs that are prereqs for the ones already weighted in this strategy
void CvTechAI::WeightPrereqs(int* paiTempWeights, int iPropagationPercent)
{
	int iTechLoop;

	// Loop through techs looking for ones that are just getting some new weight
	for(iTechLoop = 0; iTechLoop < m_pCurrentTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		// If found one, call our recursive routine to weight everything to the left in the tree
		if(paiTempWeights[iTechLoop] > 0)
		{
			PropagateWeights(iTechLoop, paiTempWeights[iTechLoop], iPropagationPercent, 0);
		}
	}
}

/// Recursive routine to weight all prerequisite techs
void CvTechAI::PropagateWeights(int iTech, int iWeight, int iPropagationPercent, int iPropagationLevel)
{
	CvTechEntry* pkTechInfo = m_pCurrentTechs->GetTechs()->GetEntry(iTech);
	if(pkTechInfo)
	{
		if(iPropagationLevel < GC.getTECH_WEIGHT_PROPAGATION_LEVELS())
		{
			int iPropagatedWeight = iWeight * iPropagationPercent / 100;

			// Loop through all prerequisites
			for(int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
			{
				// Did we find a prereq?
				int iPrereq = pkTechInfo->GetPrereqAndTechs(iI);
				if(iPrereq != NO_TECH)
				{
					// Apply reduced weight here.  Note that we apply these to the master weight array, not
					// the temporary one.  The temporary one is just used to hold the newly weighted techs
					// (from which this weight propagation must originate).
					m_TechAIWeights.IncreaseWeight(iPrereq, iPropagatedWeight);

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
}

/// Recompute weights taking into account tech cost
void CvTechAI::ReweightByCost()
{
	TechTypes eTech;

	for(int iI = 0; iI < m_ResearchableTechs.size(); iI++)
	{
		eTech = (TechTypes) m_ResearchableTechs.GetElement(iI);
		int iTurnsLeft = 0;

		iTurnsLeft = m_pCurrentTechs->GetResearchTurnsLeft(eTech, true);

		double fWeightDivisor;

		// 10 turns will add 0.02; 80 turns will add 0.16
		double fAdditionalTurnCostFactor = GC.getAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT() * iTurnsLeft;	// 0.015
		double fTotalCostFactor = GC.getAI_RESEARCH_WEIGHT_BASE_MOD() + fAdditionalTurnCostFactor;	// 0.15

		fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

		int iNewWeight = int(double(m_ResearchableTechs.GetWeight(iI)) / fWeightDivisor);

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

		strTemp.Format("CHOSEN, %s", szTechType);

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
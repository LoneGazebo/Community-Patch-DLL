/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvProjectProductionAI.h"
#if defined(MOD_BALANCE_CORE)
#include "CvEconomicAI.h"
#include "CvDiplomacyAI.h"
#endif

// include this after all other headers!
#include "LintFree.h"

/// Constructor
CvProjectProductionAI::CvProjectProductionAI(CvCity* pCity):
	m_pCity(pCity)
{
}

/// Destructor
CvProjectProductionAI::~CvProjectProductionAI(void)
{
}

/// Clear out AI local variables
void CvProjectProductionAI::Reset()
{
	m_ProjectAIWeights.clear();

	// Loop through reading each one and add an entry with 0 weight to our vector
	for(int i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
	{
		m_ProjectAIWeights.push_back(i, 0);
	}
}

/// Serialization read
void CvProjectProductionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int iWeight;

	// Reset vector
	m_ProjectAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	for(int i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
	{
		kStream >> iWeight;
		m_ProjectAIWeights.push_back(i, iWeight);
	}
}

/// Serialization write
void CvProjectProductionAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	// Loop through writing each entry
	for(int i = 0; i < GC.GetGameProjects()->GetNumProjects(); i++)
	{
		kStream << m_ProjectAIWeights.GetWeight(i);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvProjectProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	int iProject;
	CvProjectEntry* entry(NULL);

	// Loop through all projects
	for(iProject = 0; iProject < GC.GetGameProjects()->GetNumProjects(); iProject++)
	{
		entry = GC.GetGameProjects()->GetEntry(iProject);
		if(entry)
		{
			// Set its weight by looking at project's weight for this flavor and using iWeight multiplier passed in
			m_ProjectAIWeights.IncreaseWeight(iProject, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}

/// Retrieve sum of weights on one item
int CvProjectProductionAI::GetWeight(ProjectTypes eProject)
{
	return m_ProjectAIWeights.GetWeight(eProject);
}

/// Recommend highest-weighted Project
ProjectTypes CvProjectProductionAI::RecommendProject()
{
	CvAssert(m_pCity);
	if(!m_pCity)
		return NO_PROJECT;

	int iProjectLoop;
	int iWeight;
	int iTurnsLeft;

	// Reset list of all the possible projects
	m_Buildables.clear();

	// Loop through adding the available projects
	for(iProjectLoop = 0; iProjectLoop < GC.GetGameProjects()->GetNumProjects(); iProjectLoop++)
	{
		// Make sure this project can be built now
		if(m_pCity->canCreate((ProjectTypes)iProjectLoop))
		{
			// Update weight based on turns to construct
			iTurnsLeft = m_pCity->getProductionTurnsLeft((ProjectTypes) iProjectLoop, 0);
			iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_ProjectAIWeights.GetWeight((ProjectTypes)iProjectLoop), iTurnsLeft);
#if defined(MOD_BALANCE_CORE)
			iWeight += m_ProjectAIWeights.GetWeight((ProjectTypes)iProjectLoop);
			iWeight = CheckProjectBuildSanity((ProjectTypes)iProjectLoop, iWeight);
			if(iWeight <= 0)
			{
				continue;
			}
#endif
			m_Buildables.push_back(iProjectLoop, iWeight);
		}
	}

	// Sort items and grab the first one
	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds();
		return (ProjectTypes)m_Buildables.GetElement(0);
	}

	// Unless we didn't find any
	else
	{
		return NO_PROJECT;
	}
}
#if defined(MOD_BALANCE_CORE)
int CvProjectProductionAI::CheckProjectBuildSanity(ProjectTypes eProject, int iTempWeight)
{

	if(eProject == NO_PROJECT)
		return 0;

	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(!pkProjectInfo)
	{
		return 0;
	}

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if(iTempWeight == 0)
		return 0;

	//Sanitize...
	if(iTempWeight > 5000)
	{
		iTempWeight = 5000;
	}
	if (pkProjectInfo->IsRepeatable())
	{
		if (m_pCity->isUnderSiege() || m_pCity->IsResistance() || m_pCity->IsBlockaded(false) || m_pCity->IsBlockaded(true))
			return 0;
		
		if (iTempWeight > 350)
		{
			iTempWeight = 350;
		}
	}

	if(kPlayer.isMinorCiv())
	{
		return 0;
	}
	if(m_pCity->IsPuppet())
	{
		return 0;
	}

	if(kPlayer.IsAtWarAnyMajor())
	{
		if(pkProjectInfo->IsAllowsNukes())
		{
			iTempWeight *= 25;
		}
	}
	VictoryTypes ePrereqVictory = (VictoryTypes)pkProjectInfo->GetVictoryPrereq();
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_SPACE_RACE", true);
	if(eVictory != NO_VICTORY && ePrereqVictory == eVictory)
	{
		if (!GC.getGame().isVictoryValid(ePrereqVictory))
		{
			return 0;
		}
		else
		{
			iTempWeight *= 15;
			if(pkProjectInfo->IsSpaceship())
			{
				iTempWeight *= 10;
			}

			EconomicAIStrategyTypes eSpaceShipHomeStretch = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP_HOMESTRETCH");
			if(eSpaceShipHomeStretch != NO_ECONOMICAISTRATEGY)
			{
				if(kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory())
				{
					iTempWeight /= 2;
				}
				else if(kPlayer.GetDiplomacyAI()->IsGoingForWorldConquest())
				{
					iTempWeight /= 2;
				}
				else if(kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory())
				{
					iTempWeight /= 2;
				}
				else
				{
					iTempWeight *= 50;
				}
				if(kPlayer.GetEconomicAI()->IsUsingStrategy(eSpaceShipHomeStretch))
				{
					iTempWeight *= 50;
				}

			}
		}
	}

	eVictory = (VictoryTypes)GC.getInfoTypeForString("VICTORY_CULTURAL", true);
	if (eVictory != NO_VICTORY && ePrereqVictory == eVictory)
	{
		if (!GC.getGame().isVictoryValid(ePrereqVictory))
		{
			return 0;
		}
		else
		{
			iTempWeight *= 100;
		}
	}

	bool bGoodforHappiness = false;


	if (pkProjectInfo->GetHappiness() > 0)
	{
		bGoodforHappiness = true;
		iTempWeight += (30 * pkProjectInfo->GetHappiness());
	}

	if (pkProjectInfo->GetEmpireMod() < 0)
	{
		bGoodforHappiness = true;
		iTempWeight += ((m_pCity->getEmpireSizeMod()/2) * (pkProjectInfo->GetEmpireMod() * -1));
	}

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		int iMod = pkProjectInfo->GetHappinessNeedModifier(i) * -1;
		if (iMod == 0)
			continue;

		if (i == YIELD_GOLD && m_pCity->getUnhappinessFromGold() > 0)
		{
			iTempWeight += 5 * iMod * m_pCity->getUnhappinessFromGold() * m_pCity->getUnhappinessFromGold();
		}
		else if (i == YIELD_PRODUCTION && m_pCity->getUnhappinessFromDefense() > 0)
		{
			iTempWeight += 5 * iMod * m_pCity->getUnhappinessFromDefense() * m_pCity->getUnhappinessFromDefense();
		}
		else if (i == YIELD_CULTURE && m_pCity->getUnhappinessFromCulture() > 0)
		{
			iTempWeight += 5 * iMod * m_pCity->getUnhappinessFromCulture() * m_pCity->getUnhappinessFromCulture();
		}
		else if (i == YIELD_SCIENCE && m_pCity->getUnhappinessFromScience() > 0)
		{
			iTempWeight += 5 * iMod * m_pCity->getUnhappinessFromScience() * m_pCity->getUnhappinessFromScience();
		}
		else if (i == YIELD_FAITH && m_pCity->getUnhappinessFromReligion() > 0)
		{
			iTempWeight += 5 * iMod * m_pCity->getUnhappinessFromReligion() * m_pCity->getUnhappinessFromReligion();
		}
		bGoodforHappiness = true;
	}

	if (bGoodforHappiness && !GET_PLAYER(m_pCity->getOwner()).IsEmpireUnhappy())
		iTempWeight /= 50;

	return iTempWeight;
}
#endif
/// Log all potential builds
void CvProjectProductionAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;
		CvString strLogName;

		CvAssert(m_pCity);
		if(!m_pCity) return;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);
		CvAssert(pLog);
		if(!pLog) return;

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvProjectEntry* pProjectEntry = GC.GetGameProjects()->GetEntry(m_Buildables.GetElement(iI));

			strDesc = (pProjectEntry != NULL)? pProjectEntry->GetDescription() : "Unknown";
			strTemp.Format("Project, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


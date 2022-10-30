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

///
template<typename ProjectProductionAI, typename Visitor>
void CvProjectProductionAI::Serialize(ProjectProductionAI& projectProductionAI, Visitor& visitor)
{
	visitor(projectProductionAI.m_ProjectAIWeights);
}

/// Serialization read
void CvProjectProductionAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvProjectProductionAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, CvProjectProductionAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvProjectProductionAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvProjectProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	int iProject = 0;
	CvProjectEntry* entry(NULL);

	// Loop through all projects
	for(iProject = 0; iProject < GC.GetGameProjects()->GetNumProjects(); iProject++)
	{
		entry = GC.GetGameProjects()->GetEntry(iProject);
		if(entry != 0)
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
	if(m_pCity == 0)
		return NO_PROJECT;

	int iProjectLoop = 0;
	int iWeight = 0;
	int iTurnsLeft = 0;

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
		return SR_IMPOSSIBLE;

	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo == 0)
		return SR_IMPOSSIBLE;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if(iTempWeight < 1)
		return SR_IMPOSSIBLE;

	//this seems to work well to bring the raw flavor weight into a sensible range [0 ... 200]
	iTempWeight = sqrti(10 * iTempWeight);

	if (pkProjectInfo->IsRepeatable())
	{
		if (m_pCity->isUnderSiege() || m_pCity->IsResistance())
			return SR_STRATEGY;
	}

	if(kPlayer.isMinorCiv())
	{
		return SR_IMPOSSIBLE;
	}

	//no projects in puppets unless venice
	if(CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
	{
		return SR_IMPOSSIBLE;
	}

	if (pkProjectInfo->IsAllowsNukes())
	{
		if(kPlayer.IsAtWarAnyMajor())
			iTempWeight *= 25;
		else
			iTempWeight *= 10;
	}
	VictoryTypes ePrereqVictory = (VictoryTypes)pkProjectInfo->GetVictoryPrereq();
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_SPACE_RACE", true);
	if(eVictory != NO_VICTORY && ePrereqVictory == eVictory)
	{
		if (!GC.getGame().isVictoryValid(ePrereqVictory))
		{
			return SR_IMPOSSIBLE;
		}
		else
		{
			if(pkProjectInfo->IsSpaceship())
			{
				iTempWeight += m_pCity->getSpaceProductionModifier() * 10;

				if (kPlayer.GetBestProductionCity(NO_BUILDING, eProject) == m_pCity)
					iTempWeight += 5000 + (m_pCity->getSpaceProductionModifier() * 10);
				else if (kPlayer.IsCityCompetitive(m_pCity, NO_BUILDING, eProject))
					iTempWeight += 1000 + (m_pCity->getSpaceProductionModifier() * 10);
				else
					return SR_STRATEGY;
			}
			else
			{
				if (kPlayer.GetBestProductionCity(NO_BUILDING, eProject) == m_pCity)
					iTempWeight += 5000;
				else if (kPlayer.IsCityCompetitive(m_pCity, NO_BUILDING, eProject))
					iTempWeight += 1000;
				else
					return SR_STRATEGY;
			}
	
			if (kPlayer.GetDiplomacyAI()->IsGoingForSpaceshipVictory())
				iTempWeight *= 10;

			EconomicAIStrategyTypes eSpaceShipHomeStretch = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP_HOMESTRETCH");
			if (eSpaceShipHomeStretch != NO_ECONOMICAISTRATEGY)
				if(kPlayer.GetEconomicAI()->IsUsingStrategy(eSpaceShipHomeStretch))
					iTempWeight *= 10;

			return iTempWeight;
		}
	}

	eVictory = (VictoryTypes)GC.getInfoTypeForString("VICTORY_CULTURAL", true);
	if (eVictory != NO_VICTORY && ePrereqVictory == eVictory)
	{
		if (!GC.getGame().isVictoryValid(ePrereqVictory))
		{
			return SR_IMPOSSIBLE;
		}
		else
		{
			if (kPlayer.GetBestProductionCity(NO_BUILDING, eProject) == m_pCity)
				iTempWeight += 10000;
			else if (kPlayer.IsCityCompetitive(m_pCity, NO_BUILDING, eProject))
				iTempWeight += 5000;
			else
				return SR_STRATEGY;

			return iTempWeight;
		}
	}

	bool bGoodforHappiness = false;
	if (pkProjectInfo->GetHappiness() > 0)
	{
		bGoodforHappiness = true;
		iTempWeight += (30 * pkProjectInfo->GetHappiness());
	}

	if (pkProjectInfo->GetEmpireSizeModifierReduction() < 0)
	{
		bGoodforHappiness = true;
		iTempWeight += ((m_pCity->GetReducedEmpireSizeModifier(true,false)/2) * (pkProjectInfo->GetEmpireSizeModifierReduction() * -1));
	}

	if (pkProjectInfo->GetDistressFlatReduction() > 0)
	{
		bGoodforHappiness = true;
		int iDistress = m_pCity->GetDistress(false);
		if (iDistress > 0)
			iTempWeight += 75 * pkProjectInfo->GetDistressFlatReduction() * iDistress * iDistress;
	}

	if (pkProjectInfo->GetPovertyFlatReduction() > 0)
	{
		bGoodforHappiness = true;
		int iPoverty = m_pCity->GetPoverty(false);
		if (iPoverty > 0)
			iTempWeight += 75 * pkProjectInfo->GetPovertyFlatReduction() * iPoverty * iPoverty;
	}

	if (pkProjectInfo->GetIlliteracyFlatReduction() > 0)
	{
		bGoodforHappiness = true;
		int iIlliteracy = m_pCity->GetIlliteracy(false);
		if (iIlliteracy > 0)
			iTempWeight += 75 * pkProjectInfo->GetIlliteracyFlatReduction() * iIlliteracy * iIlliteracy;
	}

	if (pkProjectInfo->GetBoredomFlatReduction() > 0)
	{
		bGoodforHappiness = true;
		int iBoredom = m_pCity->GetBoredom(false);
		if (iBoredom > 0)
			iTempWeight += 75 * pkProjectInfo->GetBoredomFlatReduction() * iBoredom * iBoredom;
	}

	if (pkProjectInfo->GetReligiousUnrestFlatReduction() > 0)
	{
		bGoodforHappiness = true;
		int iReligiousUnrest = m_pCity->GetUnhappinessFromReligiousUnrest();
		if (iReligiousUnrest > 0)
			iTempWeight += 75 * pkProjectInfo->GetReligiousUnrestFlatReduction() * iReligiousUnrest * iReligiousUnrest;
	}

	if (pkProjectInfo->GetBasicNeedsMedianModifier() < 0)
	{
		bGoodforHappiness = true;
		int iDistress = m_pCity->GetDistress(false);
		if (iDistress > 0)
			iTempWeight += -5 * pkProjectInfo->GetBasicNeedsMedianModifier() * iDistress * iDistress;
	}

	if (pkProjectInfo->GetGoldMedianModifier() < 0)
	{
		bGoodforHappiness = true;
		int iPoverty = m_pCity->GetPoverty(false);
		if (iPoverty > 0)
			iTempWeight += -5 * pkProjectInfo->GetGoldMedianModifier() * iPoverty * iPoverty;
	}

	if (pkProjectInfo->GetScienceMedianModifier() < 0)
	{
		bGoodforHappiness = true;
		int iIlliteracy = m_pCity->GetIlliteracy(false);
		if (iIlliteracy > 0)
			iTempWeight += -5 * pkProjectInfo->GetScienceMedianModifier() * iIlliteracy * iIlliteracy;
	}

	if (pkProjectInfo->GetCultureMedianModifier() < 0)
	{
		bGoodforHappiness = true;
		int iBoredom = m_pCity->GetBoredom(false);
		if (iBoredom > 0)
			iTempWeight += -5 * pkProjectInfo->GetCultureMedianModifier() * iBoredom * iBoredom;
	}

	if (pkProjectInfo->GetReligiousUnrestModifier() < 0)
	{
		bGoodforHappiness = true;
		int iReligiousUnrest = m_pCity->GetUnhappinessFromReligiousUnrest();
		if (iReligiousUnrest > 0)
			iTempWeight += -5 * pkProjectInfo->GetReligiousUnrestModifier() * iReligiousUnrest * iReligiousUnrest;
	}

	if (pkProjectInfo->GetEspionageMod() < 0)
	{
		int iEsp = /*2000*/ (GD_INT_GET(ESPIONAGE_GATHERING_INTEL_COST_PERCENT) * 2) - m_pCity->GetEspionageRankingForEspionage();
		iTempWeight += (iEsp/2);
	}

	if (bGoodforHappiness && !GET_PLAYER(m_pCity->getOwner()).IsEmpireUnhappy())
		iTempWeight /= 50;

	return max(1,iTempWeight);
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
		if(m_pCity == 0) return;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog = NULL;
		pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);
		CvAssert(pLog);
		if(pLog == 0) return;

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


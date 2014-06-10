/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvBuildingProductionAI.h"

// include after all other headers
#include "LintFree.h"

/// Constructor
CvBuildingProductionAI::CvBuildingProductionAI (CvCity *pCity, CvCityBuildings *pCityBuildings):
m_pCity (pCity),
m_pCityBuildings (pCityBuildings)
{
}

/// Destructor
CvBuildingProductionAI::~CvBuildingProductionAI(void)
{
}

/// Clear out AI local variables
void CvBuildingProductionAI::Reset()
{
	CvAssertMsg (m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");

	m_BuildingAIWeights.clear();

	// Loop through reading each one and add an entry with 0 weight to our vector
	if(m_pCityBuildings)
	{
		for (int i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
		{
			m_BuildingAIWeights.push_back(i, 0);
		}
	}
}

/// Serialization read
void CvBuildingProductionAI::Read(FDataStream& kStream)
{
	int iWeight;

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	// Reset vector
	m_BuildingAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pCityBuildings)
	{
		if (uiVersion >= 2)
		{
			for (int i = 0; i < m_pCityBuildings->GetBuildings()->GetNumBuildings(); i++)
			{
				m_BuildingAIWeights.push_back(i, 0);
			}

			int iNumEntries;
			FStringFixedBuffer(sTemp, 64);
			int iType;

			kStream >> iNumEntries;

			for (int iI = 0; iI < iNumEntries; iI++)
			{
				kStream >> sTemp;
				if (sTemp != "NO_BUILDING")
				{
					iType = GC.getInfoTypeForString(sTemp);
					kStream >> iWeight;
					if (iType != -1)
					{
						m_BuildingAIWeights.IncreaseWeight(iType, iWeight);
					}
					else
					{
						CvString szError;
						szError.Format("LOAD ERROR: Building Type not found: %s", sTemp);
						GC.LogMessage(szError.GetCString());
						CvAssertMsg(false, szError);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < 89 /* Buildings in gold master */; i++)
			{
				kStream >> iWeight;
				m_BuildingAIWeights.push_back(i, iWeight);
			}
		}
	}
	else
	{
		CvAssertMsg (m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");
	}
}

/// Serialization write
void CvBuildingProductionAI::Write(FDataStream& kStream)
{
	CvAssertMsg (m_pCityBuildings != NULL, "Building Production AI init failure: city buildings are NULL");
	FStringFixedBuffer(sTemp, 64);

	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

	if (m_pCityBuildings)
	{
		int iNumBuildings = m_pCityBuildings->GetBuildings()->GetNumBuildings();
		kStream << iNumBuildings;

		// Loop through writing each entry
		for (int iI = 0; iI < iNumBuildings; iI++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				sTemp = pkBuildingInfo->GetType();
				kStream << sTemp;
				kStream << m_BuildingAIWeights.GetWeight(iI);
			}
			else
			{
				sTemp = "NO_BUILDING";
				kStream << sTemp;
			}
		}
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvBuildingProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	CvBuildingXMLEntries* pkBuildings = m_pCityBuildings->GetBuildings();

	// Loop through all buildings
	for (int iBuilding = 0; iBuilding < m_pCityBuildings->GetBuildings()->GetNumBuildings(); iBuilding++)
	{
		CvBuildingEntry *entry = pkBuildings->GetEntry(iBuilding);
		if(entry)
		{
			// Set its weight by looking at building's weight for this flavor and using iWeight multiplier passed in
			m_BuildingAIWeights.IncreaseWeight (iBuilding, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}

/// Retrieve sum of weights on one item
int CvBuildingProductionAI::GetWeight(BuildingTypes eBuilding)
{
	return m_BuildingAIWeights.GetWeight(eBuilding);
}

/// Recommend highest-weighted building
BuildingTypes CvBuildingProductionAI::RecommendBuilding()
{
	int iBldgLoop;
	int iWeight;
	int iTurnsLeft;

	// Reset list of all the possible buildings
	m_Buildables.clear();

	// Loop through adding the available buildings
	for (iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		// Make sure this building can be built now
		if (m_pCity->canConstruct((BuildingTypes)iBldgLoop))
		{
			// Update weight based on turns to construct
			iTurnsLeft = m_pCity->getProductionTurnsLeft((BuildingTypes) iBldgLoop, 0);
			iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_BuildingAIWeights.GetWeight((BuildingTypes)iBldgLoop), iTurnsLeft);
			m_Buildables.push_back (iBldgLoop, iWeight);
		}
	}

	// Sort items and grab the first one
	if (m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds();
		return (BuildingTypes)m_Buildables.GetElement(0);
	}

	// Unless we didn't find any
	else
	{
		return NO_BUILDING;
	}
}

/// Log all potential builds
void CvBuildingProductionAI::LogPossibleBuilds()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		// Dump out the weight of each buildable item
		for (int iI = 0; iI < m_Buildables.size(); iI++)
		{
			strDesc = GC.GetGameBuildings()->GetEntry(m_Buildables.GetElement(iI))->GetDescription();
			strTemp.Format("Building, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


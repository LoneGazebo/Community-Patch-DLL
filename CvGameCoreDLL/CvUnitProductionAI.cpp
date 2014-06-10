/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvUnitProductionAI.h"
#include "CvGameCoreUtils.h"
// include this after all other headers
#include "LintFree.h"


/// Constructor
CvUnitProductionAI::CvUnitProductionAI (CvCity *pCity,  CvUnitXMLEntries *pUnits):
m_pCity (pCity),
m_pUnits (pUnits)
{
}

/// Destructor
CvUnitProductionAI::~CvUnitProductionAI(void)
{
}

/// Clear out AI local variables
void CvUnitProductionAI::Reset()
{
	CvAssertMsg (m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");

	m_UnitAIWeights.clear();

	// Loop through reading each one and add an entry with 0 weight to our vector
	if(m_pUnits)
	{
		for (int i = 0; i < m_pUnits->GetNumUnits(); i++)
		{
			m_UnitAIWeights.push_back(i, 0);
		}
	}
}

/// Serialization read
void CvUnitProductionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;

	// Reset vector
	m_UnitAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pUnits)
	{
		if (uiVersion >= 2)
		{
			for (int i = 0; i < m_pUnits->GetNumUnits(); i++)
			{
				m_UnitAIWeights.push_back(i, 0);
			}

			int iNumEntries;
			FStringFixedBuffer(sTemp, 64);
			int iType;

			kStream >> iNumEntries;

			for (int iI = 0; iI < iNumEntries; iI++)
			{
				kStream >> sTemp;
				if (sTemp != "NO_UNIT")
				{
					iType = GC.getInfoTypeForString(sTemp);
					kStream >> iWeight;
					if (iType != -1)
					{
						m_UnitAIWeights.IncreaseWeight(iType, iWeight);
					}
					else
					{
						CvString szError;
						szError.Format("LOAD ERROR: Unit Type not found: %s", sTemp);
						GC.LogMessage(szError.GetCString());
						CvAssertMsg(false, szError);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < 90 /* Units in gold master */; i++)
			{
				kStream >> iWeight;
				m_UnitAIWeights.push_back(i, iWeight);
			}
		}
	}
	else
	{
		CvAssertMsg (m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");
	}
}

/// Serialization write
void CvUnitProductionAI::Write(FDataStream& kStream) const
{
	FStringFixedBuffer(sTemp, 64);

	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

	if (m_pUnits)
	{
		int iNumUnits = m_pUnits->GetNumUnits();
		kStream << iNumUnits;

		// Loop through writing each entry
		for (int iI = 0; iI < iNumUnits; iI++)
		{
			const UnitTypes eUnit = static_cast<UnitTypes>(iI);
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
			if(pkUnitInfo)
			{
				sTemp = pkUnitInfo->GetType();
				kStream << sTemp;
				kStream << m_UnitAIWeights.GetWeight(iI);
			}
			else
			{
				sTemp = "NO_UNIT";
				kStream << sTemp;
			}
		}
	}
	else
	{
		CvAssertMsg (m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvUnitProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	// Loop through all units
	for (int iUnit = 0; iUnit < m_pUnits->GetNumUnits(); iUnit++)
	{
		CvUnitEntry* entry = m_pUnits->GetEntry(iUnit);
		if(entry)
		{
			// Set its weight by looking at unit's weight for this flavor and using iWeight multiplier passed in
			m_UnitAIWeights.IncreaseWeight(iUnit, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}

/// Retrieve sum of weights on one item
int CvUnitProductionAI::GetWeight(UnitTypes eUnit)
{
	return m_UnitAIWeights.GetWeight(eUnit);
}

/// Recommend highest-weighted unit
UnitTypes CvUnitProductionAI::RecommendUnit(UnitAITypes eUnitAIType)
{
	int iUnitLoop;
	int iWeight;
	int iTurnsLeft;

	if (eUnitAIType <= NO_UNITAI)
	{
		return NO_UNIT;
	}

	// Reset list of all the possible units
	m_Buildables.clear();

	// Loop through adding the available units
	for (iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			// Make sure this unit can be built now
			if (m_pCity->canTrain(eUnit))
			{
				// Make sure it matches the requested unit AI type
				if (eUnitAIType == NO_UNITAI || pkUnitInfo->GetUnitAIType(eUnitAIType))
				{
					// Update weight based on turns to construct
					iTurnsLeft = m_pCity->getProductionTurnsLeft(eUnit, 0);
					iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_UnitAIWeights.GetWeight(eUnit), iTurnsLeft);
					m_Buildables.push_back (iUnitLoop, iWeight);
				}
			}
		}

	}

	// Sort items and grab the first one
	if (m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds(eUnitAIType);
		return (UnitTypes)m_Buildables.GetElement(0);
	}

	// Unless we didn't find any
	else
	{
		return NO_UNIT;
	}
}

/// Log all potential builds
void CvUnitProductionAI::LogPossibleBuilds(UnitAITypes eUnitAIType)
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
			strDesc = GC.GetGameUnits()->GetEntry(m_Buildables.GetElement(iI))->GetDescription();
			if (eUnitAIType == NO_UNITAI)
			{
				strTemp.Format("Unit, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
			}
			else
			{
				CvString strTempString;
				getUnitAIString(strTempString, eUnitAIType);
				strTemp.Format("Special request for unit of type: %s, %s, %d", strTempString.GetCString(), strDesc.GetCString(), m_Buildables.GetWeight(iI));
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}
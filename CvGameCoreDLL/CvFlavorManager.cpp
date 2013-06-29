/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvFlavorManager.h"
#include "CvMinorCivAI.h"

// must be included after all other headers
#include "LintFree.h"

/// Constructor
CvFlavorRecipient::CvFlavorRecipient():
m_piLatestFlavorValues(NULL)
{

}

/// Destructor
CvFlavorRecipient::~CvFlavorRecipient()
{

}

/// Initialize data
void CvFlavorRecipient::Init()
{
	int iNumFlavors = GC.getNumFlavorTypes();
	m_piLatestFlavorValues = FNEW(int[iNumFlavors], c_eCiv5GameplayDLL, 0);
	memset(m_piLatestFlavorValues, 0, iNumFlavors * sizeof(int));
}

/// Deallocate memory created in initialize
void CvFlavorRecipient::Uninit()
{
	SAFE_DELETE_ARRAY(m_piLatestFlavorValues);
}

/// Returns whether or not this Recipient is a City
bool CvFlavorRecipient::IsCity()
{
	return m_bIsCity;
}

/// Public function that other classes can call to set/reset this recipient's flavors
void CvFlavorRecipient::SetFlavors(int *piUpdatedFlavorValues)
{
	CvAssertMsg (piUpdatedFlavorValues != NULL, "Invalid array of flavor deltas passed to flavor recipient");

	if(!piUpdatedFlavorValues) return;

	int iNumFlavors = GC.getNumFlavorTypes();
	for (int iI = 0; iI < iNumFlavors; iI++)
	{
		bool bLogFlavor = false;

		if (m_piLatestFlavorValues[iI] != piUpdatedFlavorValues[iI])
		{
			bLogFlavor = true;
		}

		m_piLatestFlavorValues[iI] = piUpdatedFlavorValues[iI];

//		LogFlavors((FlavorTypes) iI);
	}

	FlavorUpdate();
}

/// Public function that other classes can call to change this recipient's flavors
void CvFlavorRecipient::ChangeFlavors(int *piDeltaFlavorValues, bool bDontLog)
{

	CvAssertMsg (piDeltaFlavorValues != NULL, "Invalid array of flavor deltas passed to flavor recipient");

	if(!piDeltaFlavorValues) return;

	int iFlavorMinValue = /*-1000*/ GC.getFLAVOR_MIN_VALUE();
	int iFlavorMaxValue = /*1000*/ GC.getFLAVOR_MAX_VALUE();

	int iNumFlavors = GC.getNumFlavorTypes();
	for (int iI = 0; iI < iNumFlavors; iI++)
	{
		if (piDeltaFlavorValues[iI] != 0)
		{
			m_piLatestFlavorValues[iI] += piDeltaFlavorValues[iI];

			// Make sure within range
			if (m_piLatestFlavorValues[iI] < iFlavorMinValue)
			{
				m_piLatestFlavorValues[iI] = iFlavorMinValue;
			}
			else if (m_piLatestFlavorValues[iI] > iFlavorMaxValue)
			{
				m_piLatestFlavorValues[iI] = iFlavorMaxValue;
			}

			if (!bDontLog)
			{
				LogFlavors((FlavorTypes) iI);
			}
		}
	}

	FlavorUpdate();
}

/// LatestFlavorValue Accessor Function
int CvFlavorRecipient::GetLatestFlavorValue(FlavorTypes eFlavor, bool bAllowNegative)
{
	CvAssertMsg (eFlavor > -1, "Out of bounds.");
	CvAssertMsg (eFlavor < GC.getNumFlavorTypes(), "Out of bounds.");

	if (m_piLatestFlavorValues[eFlavor] < 0 && !bAllowNegative)
	{
		return 0;
	}

	return m_piLatestFlavorValues[eFlavor];
}

/// Constructor
CvFlavorManager::CvFlavorManager(void):
m_piPersonalityFlavor(NULL),
m_piActiveFlavor(NULL)
{

}

/// Destructor
CvFlavorManager::~CvFlavorManager(void)
{
	Uninit();
}

/// Initialize
void CvFlavorManager::Init(CvPlayer *pPlayer)
{
	unsigned int iI;

	// Copy off inputs
	m_pPlayer = pPlayer;

	// Allocate memory
	m_piPersonalityFlavor = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);
	m_piActiveFlavor = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);
    m_FlavorTargetList.get_allocator().Reserve( (3*64)+100);

	// Clear variables
	Reset();

	// If this is a live player, go ahead and set up his flavor preferences
	PlayerTypes p = pPlayer->GetID();
	if (p != NO_PLAYER)
	{
		SlotStatus s = CvPreGame::slotStatus(p);
		if ((s == SS_TAKEN || s == SS_COMPUTER) && !pPlayer->isBarbarian())
		{
			// Copy over leaderhead defaults unless human
			if (!pPlayer->isHuman())
			{
				LeaderHeadTypes type = pPlayer->getPersonalityType();
				if (type != NO_LEADER)
				{
					CvLeaderHeadInfo* pkLeaderHeadInfo = GC.getLeaderHeadInfo(type);
					if(pkLeaderHeadInfo)
					{
						int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();
						unsigned int uiNumFlavorTypes = GC.getNumFlavorTypes();

						for (iI = 0; iI < uiNumFlavorTypes; iI++)
						{
							// Majors use Leader XML Flavors
							if (!pPlayer->isMinorCiv())
							{
								m_piPersonalityFlavor[iI] = pkLeaderHeadInfo->getFlavorValue(iI);
							}
							// Minors use Minor XML Flavors
							else
							{
								m_piPersonalityFlavor[iI] = GC.getMinorCivInfo(pPlayer->GetMinorCivAI()->GetMinorCivType())->getFlavorValue(iI);
							}

							// If no Flavor value is set in the XML use the Default
							if (m_piPersonalityFlavor[iI] == -1)
							{
								m_piPersonalityFlavor[iI] = iDefaultFlavorValue;
							}
						}
					}
				}

				// Tweak from default values
				RandomizeWeights();
			}

			// Human player, just set all flavors to average (5)
			else
			{
				int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();
				unsigned int uiNumFlavors = GC.getNumFlavorTypes();
				for (iI = 0; iI < uiNumFlavors; iI++)
				{
					m_piPersonalityFlavor[iI] = iDefaultFlavorValue;
				}
			}

			// Send out updated values to all recipients
			BroadcastBaseFlavors();

			ResetToBasePersonality();

			if (GC.getLogging() && GC.getAILogging())
			{
				LogFlavors();
			}
		}
	}
}

/// Deallocate memory created in initialize
void CvFlavorManager::Uninit()
{
	SAFE_DELETE_ARRAY(m_piPersonalityFlavor);
	SAFE_DELETE_ARRAY(m_piActiveFlavor);
	m_FlavorTargetList.clear();
}

/// Reset member variables
void CvFlavorManager::Reset()
{
	int iI;

	for (iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		m_piPersonalityFlavor[iI] = 0;
		m_piActiveFlavor[iI] = 0;
	}
}

/// Serialization read
void CvFlavorManager::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	CvAssertMsg (GC.getNumFlavorTypes() > 0, "Number of flavors to serialize is expected to greater than 0)");

	int iNumFlavors = 27;
	if (uiVersion >= 2)
	{
		kStream >> iNumFlavors;
	}

	ArrayWrapper<int> wrapm_piPersonalityFlavor(iNumFlavors, m_piPersonalityFlavor);
	kStream >> wrapm_piPersonalityFlavor;

	ArrayWrapper<int> wrapm_piActiveFlavor(iNumFlavors, m_piActiveFlavor);
	kStream >> wrapm_piActiveFlavor;
}

/// Serialization write
void CvFlavorManager::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

	CvAssertMsg (GC.getNumFlavorTypes() > 0, "Number of flavors to serialize is expected to greater than 0)");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piPersonalityFlavor);
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piActiveFlavor);
}

/// Register a new recipient of the player's global flavors
void CvFlavorManager::AddFlavorRecipient(CvFlavorRecipient *pTargetObject, bool bPropegateFlavorValues)
{
	// Add this one to our list
	m_FlavorTargetList.push_back (pTargetObject);

	// If we've already been initialized, then go ahead and send out current values
	if (m_piPersonalityFlavor != NULL && bPropegateFlavorValues)
	{
		pTargetObject->SetFlavors(m_piPersonalityFlavor);
	}
}

/// Remove the recipient
void CvFlavorManager::RemoveFlavorRecipient(CvFlavorRecipient *pTargetObject)
{
    Flavor_List::iterator iter = m_FlavorTargetList.begin();
    Flavor_List::iterator end  = m_FlavorTargetList.end();

    while(iter != end)
    {
        if(*iter == pTargetObject)
        {
            m_FlavorTargetList.erase(iter);
            return;
        }
		iter++;
    }
}

/// Update to a new set of flavors
void CvFlavorManager::ChangeFlavors(int *piDeltaFlavorValues, bool bDontUpdateCityFlavors)
{
	CvAssertMsg (piDeltaFlavorValues != NULL, "Invalid array of flavor deltas passed to flavor manager");

	if(!piDeltaFlavorValues) return;

	int iFlavorMinValue = /*-1000*/ GC.getFLAVOR_MIN_VALUE();
	int iFlavorMaxValue = /*1000*/ GC.getFLAVOR_MAX_VALUE();

	int iNumFlavors = GC.getNumFlavorTypes();
	for (int iI = 0; iI < iNumFlavors; iI++)
	{
		if (piDeltaFlavorValues[iI] != 0)
		{
			m_piActiveFlavor[iI] += piDeltaFlavorValues[iI];

			// Make sure within range
			if (m_piActiveFlavor[iI] < iFlavorMinValue)
			{
				m_piActiveFlavor[iI] =iFlavorMinValue;
			}
			else if (m_piActiveFlavor[iI] > iFlavorMaxValue)
			{
				m_piActiveFlavor[iI] = iFlavorMaxValue;
			}

			LogFlavors((FlavorTypes) iI);
		}
	}

	BroadcastFlavors(piDeltaFlavorValues, bDontUpdateCityFlavors);
}

/// Resets active settings to player's base personality
void CvFlavorManager::ResetToBasePersonality()
{
	int iI;

	for (iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		m_piActiveFlavor[iI] = m_piPersonalityFlavor[iI];
	}

	BroadcastBaseFlavors();
}

/// Make some adjustments to flavors based on the map we're on
void CvFlavorManager::AdjustWeightsForMap()
{
	int iTotalLandTiles;
	int iNumPlayers;
	double iTilesPerPlayer;
	double fAdjust;
	int iAdjust;

	iTotalLandTiles = GC.getMap().getLandPlots();
	iNumPlayers = GC.getGame().countMajorCivsAlive();

	if (iNumPlayers > 0)
	{
		int iNumFlavorTypes = GC.getNumFlavorTypes();
		// Find tiles per player
		iTilesPerPlayer = (double)iTotalLandTiles / (double)iNumPlayers;

		// Compute +/- addition
		//
		// We want this to be logarithmic, since that is the curve between lots of players on a duel map
		// and a few player on a huge map.  "FLAVOR_STANDARD_LOG10_TILES_PER_PLAYER" is the typical log10 of
		// tiles per player.  We go up and down from this point (multiplying by a coefficient) from here
		fAdjust = log10 (iTilesPerPlayer) - GC.getFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER();
		fAdjust *= (double)GC.getFLAVOR_EXPANDGROW_COEFFICIENT();
		iAdjust = (int)fAdjust;

		int iFlavorMaxValue = /*20*/ GC.getPERSONALITY_FLAVOR_MAX_VALUE();
		int iFlavorMinValue = /*0*/ GC.getPERSONALITY_FLAVOR_MIN_VALUE();

		int iExpansionIndex = GC.getInfoTypeForString("FLAVOR_EXPANSION");
		int iGrowthIndex = GC.getInfoTypeForString("FLAVOR_GROWTH");

		// Boost expansion
		CvAssert(iExpansionIndex >= 0 && iExpansionIndex < iNumFlavorTypes);
		if (iExpansionIndex >= 0 && iExpansionIndex < iNumFlavorTypes)
		{
			m_piPersonalityFlavor[iExpansionIndex] += iAdjust;
			if(m_piPersonalityFlavor[iExpansionIndex] > iFlavorMaxValue)
			{
				m_piPersonalityFlavor[iExpansionIndex] = iFlavorMaxValue;
			}
		}

		// Reduce growth
		CvAssert(iGrowthIndex >= 0 && iGrowthIndex < iNumFlavorTypes);
		if (iGrowthIndex >= 0 && iGrowthIndex < iNumFlavorTypes)
		{
			m_piPersonalityFlavor[iGrowthIndex] -= iAdjust;
			if(m_piPersonalityFlavor[iGrowthIndex] < iFlavorMinValue)
			{
				m_piPersonalityFlavor[iGrowthIndex] = iFlavorMinValue;
			}
		}

		// Save these off as our core personality and broadcast updates
		ResetToBasePersonality();
	}
}

/// Retrieve the current value of one flavor
int CvFlavorManager::GetIndividualFlavor (FlavorTypes eType)
{
	CvAssert((int)eType >= 0 && (int)eType < GC.getNumFlavorTypes());
	return m_piActiveFlavor[eType];
}

/// Retrieve the current value of all flavors
int *CvFlavorManager::GetAllFlavors()
{
	return m_piActiveFlavor;
}

/// Retrieve the value of one Personality flavor
int CvFlavorManager::GetPersonalityIndividualFlavor (FlavorTypes eType)
{
	CvAssert((int)eType >= 0 && (int)eType < GC.getNumFlavorTypes());
	return m_piPersonalityFlavor[eType];
}

/// Retrieve the value of all Personality flavors
int *CvFlavorManager::GetAllPersonalityFlavors()
{
	return m_piPersonalityFlavor;
}

// PRIVATE METHODS

/// Make a random adjustment to each flavor value for this leader so they don't play exactly the same
void CvFlavorManager::RandomizeWeights()
{
	int iI;
	int iMin, iMax, iPlusMinus;

	iMin = /*0*/ GC.getPERSONALITY_FLAVOR_MIN_VALUE();
	iMax = /*20*/ GC.getPERSONALITY_FLAVOR_MAX_VALUE();
	iPlusMinus = /*2*/ GC.getFLAVOR_RANDOMIZATION_RANGE();

	for (iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		// Don't modify it if it's zero-ed out in the XML
		if (m_piPersonalityFlavor[iI] != 0)
		{
			m_piPersonalityFlavor[iI] = GetAdjustedValue(m_piPersonalityFlavor[iI], iPlusMinus, iMin, iMax);
		}
	}
}

/// Add a random plus/minus to an integer (but keep it in range)
int CvFlavorManager::GetAdjustedValue (int iOriginalValue, int iPlusMinus, int iMin, int iMax)
{
	int iAdjust;
	int iRtnValue;

	iAdjust = GC.getGame().getJonRandNum((iPlusMinus * 2 + 1), "Adjusting Personality Flavor");
	iRtnValue = iOriginalValue + iAdjust - iPlusMinus;

	if (iRtnValue < iMin)
		iRtnValue = iMin;
	else if (iRtnValue > iMax)
		iRtnValue = iMax;

	return iRtnValue;
}

/// Sends current flavor settings to all recipients
void CvFlavorManager::BroadcastFlavors(int *piDeltaFlavorValues, bool bDontUpdateCityFlavors)
{
	Flavor_List::iterator it;

	for (it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); it++)
	{
		if (!bDontUpdateCityFlavors || !(*it)->IsCity())
		{
			(*it)->ChangeFlavors(piDeltaFlavorValues);
		}
	}
}

/// Sends base personality flavor settings to all recipients
void CvFlavorManager::BroadcastBaseFlavors()
{
	Flavor_List::iterator it;

	for (it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); it++)
	{
		(*it)->SetFlavors(m_piPersonalityFlavor);
	}
}

void CvFlavorManager::LogFlavors(FlavorTypes eFlavor)
{
	CvString strOutBuf;
	CvString strBaseString;
	CvString strTemp;
	CvString playerName;
	CvString strDesc;
	CvString strLogName;

	if (GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "FlavorAILog.csv";
		}

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Dump out the setting for each flavor
		if (eFlavor == NO_FLAVOR)
		{
			for (int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{
				// Only dump if non-zero
				//		if (m_piLatestFlavorValues[iI] > 0)
				{
					strTemp.Format ("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), GetIndividualFlavor((FlavorTypes) iI));
					strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
		else
		{
			strTemp.Format ("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), GetIndividualFlavor(eFlavor));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

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
#include "CvEnumMapSerialization.h"

// must be included after all other headers
#include "LintFree.h"

/// Constructor
CvFlavorRecipient::CvFlavorRecipient()
{

}

/// Destructor
CvFlavorRecipient::~CvFlavorRecipient()
{

}

/// Initialize data
void CvFlavorRecipient::Init()
{
	m_piLatestFlavorValues.init(0);
}

/// Deallocate memory created in initialize
void CvFlavorRecipient::Uninit()
{
	m_piLatestFlavorValues.uninit();
}

/// Returns whether or not this Recipient is a City
bool CvFlavorRecipient::IsCity()
{
	return m_bIsCity;
}

/// Public function that other classes can call to set/reset this recipient's flavors
void CvFlavorRecipient::SetFlavors(const CvEnumMap<FlavorTypes, int>& piUpdatedFlavorValues)
{
	CvAssertMsg(piUpdatedFlavorValues.valid(), "Invalid map of flavor deltas passed to flavor recipient");

	if(!piUpdatedFlavorValues.valid()) return;

	int iNumFlavors = GC.getNumFlavorTypes();
	for(int iI = 0; iI < iNumFlavors; iI++)
	{
		bool bLogFlavor = false;

		if(m_piLatestFlavorValues[iI] != piUpdatedFlavorValues[iI])
		{
			bLogFlavor = true;
		}

		m_piLatestFlavorValues[iI] = piUpdatedFlavorValues[iI];

//		LogFlavors((FlavorTypes) iI);
	}

	FlavorUpdate();
}

/// Public function that other classes can call to change this recipient's flavors
void CvFlavorRecipient::ChangeFlavors(const CvEnumMap<FlavorTypes, int>& piDeltaFlavorValues, bool bDontLog)
{

	CvAssertMsg(piDeltaFlavorValues.valid(), "Invalid map of flavor deltas passed to flavor recipient");

	if(!piDeltaFlavorValues.valid()) return;

	int iFlavorMinValue = /*-1000*/ GC.getFLAVOR_MIN_VALUE();
	int iFlavorMaxValue = /*1000*/ GC.getFLAVOR_MAX_VALUE();

	int iNumFlavors = GC.getNumFlavorTypes();
	for(int iI = 0; iI < iNumFlavors; iI++)
	{
		if(piDeltaFlavorValues[iI] != 0)
		{
			m_piLatestFlavorValues[iI] += piDeltaFlavorValues[iI];

			// Make sure within range
			if(m_piLatestFlavorValues[iI] < iFlavorMinValue)
			{
				m_piLatestFlavorValues[iI] = iFlavorMinValue;
			}
			else if(m_piLatestFlavorValues[iI] > iFlavorMaxValue)
			{
				m_piLatestFlavorValues[iI] = iFlavorMaxValue;
			}

			if(!bDontLog)
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
	CvAssertMsg(eFlavor > -1, "Out of bounds.");
	CvAssertMsg(eFlavor < GC.getNumFlavorTypes(), "Out of bounds.");

	if(m_piLatestFlavorValues[eFlavor] < 0 && !bAllowNegative)
	{
		return 0;
	}

	return m_piLatestFlavorValues[eFlavor];
}

/// Constructor
CvFlavorManager::CvFlavorManager(void)
{

}

/// Destructor
CvFlavorManager::~CvFlavorManager(void)
{
	Uninit();
}

/// Initialize
void CvFlavorManager::Init(CvPlayer* pPlayer)
{
	// Copy off inputs
	m_pPlayer = pPlayer;

	// Allocate memory
	m_piPersonalityFlavor.init();
	m_piActiveFlavor.init();
	m_FlavorTargetList.reserve((3*64)+100);

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
				LeaderHeadTypes leader = pPlayer->getPersonalityType();
				if (leader != NO_LEADER)
				{
					CvLeaderHeadInfo* pkLeaderHeadInfo = GC.getLeaderHeadInfo(leader);
					if (pkLeaderHeadInfo)
					{
						int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();
						int iNumFlavorTypes = GC.getNumFlavorTypes();

						for (int iI = 0; iI < iNumFlavorTypes; iI++)
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
				int iDefaultFlavorValue = /*5*/ GC.getGame().GetDefaultFlavorValue();
				int iNumFlavors = GC.getNumFlavorTypes();
				for (int iI = 0; iI < iNumFlavors; iI++)
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
	m_piPersonalityFlavor.uninit();
	m_piActiveFlavor.uninit();
	m_FlavorTargetList.clear();
}

/// Reset member variables
void CvFlavorManager::Reset()
{
	m_piPersonalityFlavor.assign(0);
	m_piActiveFlavor.assign(0);
}

template<typename FlavorManager, typename Visitor>
void CvFlavorManager::Serialize(FlavorManager& flavorManager, Visitor& visitor)
{
	visitor(flavorManager.m_piPersonalityFlavor);
	visitor(flavorManager.m_piActiveFlavor);
}

/// Serialization read
void CvFlavorManager::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvFlavorManager::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, CvFlavorManager& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvFlavorManager& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

/// Register a new recipient of the player's global flavors
void CvFlavorManager::AddFlavorRecipient(CvFlavorRecipient* pTargetObject, bool bPropegateFlavorValues)
{
	// Add this one to our list
	m_FlavorTargetList.push_back(pTargetObject);

	// If we've already been initialized, then go ahead and send out current values
	if(m_piPersonalityFlavor.valid() && bPropegateFlavorValues)
	{
		pTargetObject->SetFlavors(m_piPersonalityFlavor);
	}
}

/// Remove the recipient
void CvFlavorManager::RemoveFlavorRecipient(CvFlavorRecipient* pTargetObject)
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

#if defined(MOD_API_EXTENSIONS)
void CvFlavorManager::ChangeLeader(LeaderHeadTypes eOldLeader, LeaderHeadTypes eNewLeader)
{
	CvLeaderHeadInfo* pkOldLeaderHeadInfo = GC.getLeaderHeadInfo(eOldLeader);
	CvLeaderHeadInfo* pkNewLeaderHeadInfo = GC.getLeaderHeadInfo(eNewLeader);
	
	if(pkOldLeaderHeadInfo && pkNewLeaderHeadInfo)
	{
		CvEnumMap<FlavorTypes, int> aiTempFlavors;
		aiTempFlavors.init();
		
		for(std::size_t iI = 0; iI < aiTempFlavors.size(); iI++)
		{
			aiTempFlavors[iI] = pkNewLeaderHeadInfo->getFlavorValue(iI) - pkOldLeaderHeadInfo->getFlavorValue(iI);
		}
		
		ChangeFlavors(aiTempFlavors, true);

		aiTempFlavors.uninit();
	}
}
#endif

/// Update to a new set of flavors
void CvFlavorManager::ChangeFlavors(const CvEnumMap<FlavorTypes, int>& piDeltaFlavorValues, bool	bPlayerLevelUpdate)
{
	CvAssertMsg(piDeltaFlavorValues.valid(), "Invalid map of flavor deltas passed to flavor manager");

	if(!piDeltaFlavorValues.valid()) return;

	if (bPlayerLevelUpdate)
	{
		int iFlavorMinValue = /*-1000*/ GC.getFLAVOR_MIN_VALUE();
		int iFlavorMaxValue = /*1000*/ GC.getFLAVOR_MAX_VALUE();

		int iNumFlavors = GC.getNumFlavorTypes();
		for(int iI = 0; iI < iNumFlavors; iI++)
		{
			if(piDeltaFlavorValues[iI] != 0)
			{
				m_piActiveFlavor[iI] += piDeltaFlavorValues[iI];

				// Make sure within range
				if(m_piActiveFlavor[iI] < iFlavorMinValue)
				{
					m_piActiveFlavor[iI] =iFlavorMinValue;
				}
				else if(m_piActiveFlavor[iI] > iFlavorMaxValue)
				{
					m_piActiveFlavor[iI] = iFlavorMaxValue;
				}

				LogFlavors((FlavorTypes) iI);
			}
		}
	}

	BroadcastFlavors(piDeltaFlavorValues, bPlayerLevelUpdate);
}

/// Resets active settings to player's base personality
void CvFlavorManager::ResetToBasePersonality()
{
	for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		m_piActiveFlavor[iI] = m_piPersonalityFlavor[iI];
	}

	BroadcastBaseFlavors();
}

/// Make some adjustments to flavors based on the map we're on
void CvFlavorManager::AdjustWeightsForMap()
{
	int iTotalLandTiles = GC.getMap().getLandPlots();
	int iNumPlayers = GC.getGame().countMajorCivsAlive();

	if(iNumPlayers > 0)
	{
		int iNumFlavorTypes = GC.getNumFlavorTypes();
		// Find tiles per player
		float fTilesPerPlayer = iTotalLandTiles / (float)iNumPlayers;

		// Compute +/- addition
		//
		// We want this to be logarithmic, since that is the curve between lots of players on a duel map
		// and a few player on a huge map.  "FLAVOR_STANDARD_LOG10_TILES_PER_PLAYER" is the typical log10 of
		// tiles per player.  We go up and down from this point (multiplying by a coefficient) from here
		float fAdjust = log10(fTilesPerPlayer) - GC.getFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER();
		int iAdjust = (int)(fAdjust*GC.getFLAVOR_EXPANDGROW_COEFFICIENT());

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

/// Retrieve the value of one Personality flavor, typically in the range [0,10]
int CvFlavorManager::GetPersonalityIndividualFlavor(FlavorTypes eType)
{
	if ((int)eType < 0 || (int)eType >= GC.getNumFlavorTypes()) return 0;

	return range(m_piPersonalityFlavor[eType],0,30);
}

/// Retrieve the value of all Personality flavors
CvEnumMap<FlavorTypes, int>& CvFlavorManager::GetAllPersonalityFlavors()
{
	return m_piPersonalityFlavor;
}


/// Retrieve the value of one Personality flavor, modified for the diplomacy AI
int CvFlavorManager::GetPersonalityFlavorForDiplomacy(FlavorTypes eType)
{
	if ((int)eType < 0 || (int)eType >= GC.getNumFlavorTypes()) return 0;

	int iValue = m_piPersonalityFlavor[eType];

	// If the flavor is zeroed out or deleted, we have to account for that - use the default value
	if (iValue <= 0)
	{
		return /*5*/ GC.getGame().GetDefaultFlavorValue();
	}

	// Must be within upper and lower bounds
	int iMax = range(/*10*/ GC.getDIPLO_PERSONALITY_FLAVOR_MAX_VALUE(), 1, 20);
	int iMin = range(/*1*/ GC.getDIPLO_PERSONALITY_FLAVOR_MIN_VALUE(), 1, iMax);

	return range(iValue, iMin, iMax);
}

// PRIVATE METHODS

/// Make a random adjustment to each flavor value for this leader so they don't play exactly the same
void CvFlavorManager::RandomizeWeights()
{
	int iMax = range(/*20*/ GC.getPERSONALITY_FLAVOR_MAX_VALUE(), 1, 20);
	int iMin = range(/*0*/ GC.getPERSONALITY_FLAVOR_MIN_VALUE(), 1, iMax);
	int iPlusMinus = max(/*2*/ GC.getFLAVOR_RANDOMIZATION_RANGE(), 0);

	// Random seed to ensure the fake RNG doesn't return the same value repeatedly
	int iSeed = 0;

	for (int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
	{
		// Don't modify it if it's zero-ed out in the XML
		if (m_piPersonalityFlavor[iI] != 0)
		{
			m_piPersonalityFlavor[iI] = GetAdjustedValue(m_piPersonalityFlavor[iI], iPlusMinus, iMin, iMax, iSeed);
		}
	}
}

/// Add a random plus/minus to an integer (but keep it in range)
int CvFlavorManager::GetAdjustedValue(int iOriginalValue, int iPlusMinus, int iMin, int iMax, int& iSeed)
{
	// Increment the random seed (and make sure it's > 0)
	if (iSeed < 0)
		iSeed = 0;

	iSeed += (iOriginalValue + iPlusMinus) * 200;

	// Randomize!
	int iAdjust = GC.getGame().getSmallFakeRandNum((iPlusMinus * 2 + 1), (iOriginalValue * iSeed));
	int iRtnValue = iOriginalValue + iAdjust - iPlusMinus;

	return range(iRtnValue, iMin, iMax);
}

/// Sends current flavor settings to all recipients
void CvFlavorManager::BroadcastFlavors(const CvEnumMap<FlavorTypes, int>& piDeltaFlavorValues, bool bPlayerLevelUpdate)
{
	for(Flavor_List::iterator it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); it++)
	{
		if(bPlayerLevelUpdate && !(*it)->IsCity())
		{
			(*it)->ChangeFlavors(piDeltaFlavorValues);
		}
		else if (!bPlayerLevelUpdate && (*it)->IsCity())
		{
			(*it)->ChangeFlavors(piDeltaFlavorValues);
		}
	}
}

/// Sends base personality flavor settings to all recipients
void CvFlavorManager::BroadcastBaseFlavors()
{
	for(Flavor_List::iterator it = m_FlavorTargetList.begin(); it != m_FlavorTargetList.end(); it++)
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

	if(GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "FlavorAILog.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Dump out the setting for each flavor
		if(eFlavor == NO_FLAVOR)
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{
				// Only dump if non-zero
				//		if (m_piLatestFlavorValues[iI] > 0)
				{
					strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), m_piActiveFlavor[iI]);
					strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
		else
		{
			strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), m_piActiveFlavor[eFlavor]);
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

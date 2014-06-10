/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvWonderProductionAI.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvCitySpecializationAI.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvInfosSerializationHelper.h"
// include this after all other headers
#include "LintFree.h"


/// Constructor
CvWonderProductionAI::CvWonderProductionAI(CvPlayer* pPlayer, CvBuildingXMLEntries* pBuildings):
	m_pPlayer(pPlayer),
	m_pBuildings(pBuildings)
{
}

/// Destructor
CvWonderProductionAI::~CvWonderProductionAI(void)
{
}

/// Initialize
void CvWonderProductionAI::Init(CvBuildingXMLEntries* pBuildings, CvPlayer* pPlayer, bool bIsCity)
{
	// Init base class
	CvFlavorRecipient::Init();

	// Store off the pointer to the buildings for this game
	m_pBuildings = pBuildings;
	m_pPlayer = pPlayer;
	m_bIsCity = bIsCity;

	Reset();
}

/// Clear out AI local variables
void CvWonderProductionAI::Reset()
{
	CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");

	// Reset vector
	m_WonderAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pBuildings)
	{
		for(int i = 0; i < m_pBuildings->GetNumBuildings(); i++)
		{
			m_WonderAIWeights.push_back(i, 0);
		}
	}
}

/// Serialization read
void CvWonderProductionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> wrapm_piLatestFlavorValues(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapm_piLatestFlavorValues;

	CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");

	// Reset vector
	m_WonderAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pBuildings)
	{
		for(int i = 0; i < m_pBuildings->GetNumBuildings(); i++)
		{
			m_WonderAIWeights.push_back(i, 0);
		}

		int iNumEntries;
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			bool bValid = true;
			iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
			if(iType != -1 || !bValid)
			{
				kStream >> iWeight;
				if(iType != -1)
				{
					m_WonderAIWeights.IncreaseWeight(iType, iWeight);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Building Type not found");
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
		}
	}
}

/// Serialization write
void CvWonderProductionAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piLatestFlavorValues);

	if(m_pBuildings)
	{
		int iNumBuildings = m_pBuildings->GetNumBuildings();
		kStream << iNumBuildings;

		// Loop through writing each entry
		for(int iI = 0; iI < iNumBuildings; iI++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				CvInfosSerializationHelper::WriteHashed(kStream, pkBuildingInfo);
				kStream << m_WonderAIWeights.GetWeight(iI);
			}
			else
			{
				kStream << (int)0;
			}
		}
	}
	else
	{
		CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");
	}
}

/// Respond to a new set of flavor values
void CvWonderProductionAI::FlavorUpdate()
{
	// Broadcast to our sub AI objects
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes)iFlavor);
		AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvWonderProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	// Loop through all buildings (even though we're only go to do anything on wonders)
	for(int iBldg = 0; iBldg < m_pBuildings->GetNumBuildings(); iBldg++)
	{
		CvBuildingEntry* entry = m_pBuildings->GetEntry(iBldg);
		if(entry)
		{
			CvBuildingEntry& kBuilding = *entry;
			if(IsWonder(kBuilding))
			{
				// Set its weight by looking at wonder's weight for this flavor and using iWeight multiplier passed in
				m_WonderAIWeights.IncreaseWeight(iBldg, kBuilding.GetFlavorValue(eFlavor) * iWeight);
			}
		}
	}
}

/// Retrieve sum of weights on one item
int CvWonderProductionAI::GetWeight(BuildingTypes eBldg)
{
	return m_WonderAIWeights.GetWeight(eBldg);
}

/// Recommend highest-weighted wonder, also return total weight of all buildable wonders
BuildingTypes CvWonderProductionAI::ChooseWonder(bool bUseAsyncRandom, bool bAdjustForOtherPlayers, int& iWonderWeight)
{
	int iBldgLoop;
	int iWeight;
	int iTurnsRequired;
	int iEstimatedProductionPerTurn;
	int iCityLoop;
	RandomNumberDelegate fcn;
	BuildingTypes eSelection;

	// Use the asynchronous random number generate if "no random" is set
	if(bUseAsyncRandom)
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	}
	else
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	}

	// Reset list of all the possible wonders
	m_Buildables.clear();

	// Guess which city will be producing this (doesn't matter that much since weights are all relative)
	CvCity* pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
	if(pWonderCity == NULL)
	{
		pWonderCity = m_pPlayer->firstCity(&iCityLoop);
	}

	CvAssertMsg(pWonderCity, "Trying to choose the next wonder to build and the player has no cities!");
	if(pWonderCity == NULL)
		return NO_BUILDING;

	iEstimatedProductionPerTurn = pWonderCity->getCurrentProductionDifference(true, false);
	if(iEstimatedProductionPerTurn < 1)
	{
		iEstimatedProductionPerTurn = 1;
	}

	// Loop through adding the available wonders
	for(iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = m_pBuildings->GetEntry(eBuilding);
		if(pkBuildingInfo)
		{
			CvBuildingEntry& kBuilding = *pkBuildingInfo;
			const CvBuildingClassInfo& kBuildingClassInfo = kBuilding.GetBuildingClassInfo();

			// Make sure this wonder can be built now
			if(IsWonder(kBuilding) && HaveCityToBuild((BuildingTypes)iBldgLoop))
			{
				iTurnsRequired = std::max(1, kBuilding.GetProductionCost() / iEstimatedProductionPerTurn);

				// if we are forced to restart a wonder, give one that has been started already a huge bump
				bool bAlreadyStarted = pWonderCity->GetCityBuildings()->GetBuildingProduction(eBuilding) > 0;
				int iTempWeight = bAlreadyStarted ? m_WonderAIWeights.GetWeight(iBldgLoop) * 25 : m_WonderAIWeights.GetWeight(iBldgLoop);

				// Don't build the UN if you aren't going for the diplo victory
				if(pkBuildingInfo->IsDiplomaticVoting())
				{
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					int iSecuredVotes = 0;
					TeamTypes myTeamID = m_pPlayer->getTeam();
					PlayerTypes myPlayerID = m_pPlayer->GetID();

					// Loop through Players to see if they'll vote for this player
					PlayerTypes eLoopPlayer;
					TeamTypes eLoopTeam;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(eLoopPlayer).isAlive())
						{
							eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

							// Liberated?
							if(GET_TEAM(eLoopTeam).GetLiberatedByTeam() == myTeamID)
							{
								iSecuredVotes++;
							}

							// Minor civ?
							else if(GET_PLAYER(eLoopPlayer).isMinorCiv())
							{
								// Best Relations?
								if(GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == myPlayerID)
								{
									iSecuredVotes++;
								}
							}
						}
					}

					int iNumberOfPlayersWeNeedToBuyOff = MAX(0, iVotesNeededToWin - iSecuredVotes);

					if(!m_pPlayer->GetDiplomacyAI() || !m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() || m_pPlayer->GetTreasury()->GetGold() < iNumberOfPlayersWeNeedToBuyOff * 500 )
					{
						iTempWeight = 0;
					}
				}

				iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(iTempWeight, iTurnsRequired);

				if(bAdjustForOtherPlayers && ::isWorldWonderClass(kBuildingClassInfo))
				{
					// Adjust weight for this wonder down based on number of other players currently working on it
					int iNumOthersConstructing = 0;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
						if(GET_PLAYER(eLoopPlayer).getBuildingClassMaking((BuildingClassTypes)kBuilding.GetBuildingClassType()) > 0)
						{
							iNumOthersConstructing++;
						}
					}
					iWeight = iWeight / (1 + iNumOthersConstructing);
				}

				m_Buildables.push_back(iBldgLoop, iWeight);
			}
		}
	}

	// Sort items and grab the first one
	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleWonders();

		if(m_Buildables.GetTotalWeight() > 0)
		{
			int iNumChoices = GC.getGame().getHandicapInfo().GetCityProductionNumOptions();
			eSelection = (BuildingTypes)m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing wonder from Top Choices");
			iWonderWeight = m_Buildables.GetTotalWeight();
			return eSelection;
		}

		// Nothing with any weight
		else
		{
			return NO_BUILDING;
		}
	}

	// Unless we didn't find any
	else
	{
		return NO_BUILDING;
	}
}


/// Recommend highest-weighted wonder and what city to build it at
BuildingTypes CvWonderProductionAI::ChooseWonderForGreatEngineer(bool bUseAsyncRandom, int& iWonderWeight, CvCity*& pCityToBuildAt)
{
	int iBldgLoop;
	int iWeight;
	int iCityLoop;
	RandomNumberDelegate fcn;
	BuildingTypes eSelection;

	pCityToBuildAt = 0;
	iWonderWeight = 0;

	// Use the asynchronous random number generate if "no random" is set
	if (bUseAsyncRandom)
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	}
	else
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	}

	// Reset list of all the possible wonders
	m_Buildables.clear();

	// Guess which city will be producing this
	CvCity* pWonderCity = m_pPlayer->GetCitySpecializationAI()->GetWonderBuildCity();
	if (pWonderCity == NULL)
	{
		pWonderCity = m_pPlayer->firstCity(&iCityLoop);
	}

	CvAssertMsg(pWonderCity, "Trying to choose the next wonder to build and the player has no cities!");
	if (pWonderCity == NULL)
		return NO_BUILDING;

	// Loop through adding the available wonders
	for (iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = m_pBuildings->GetEntry(eBuilding);
		if (pkBuildingInfo)
		{
			CvBuildingEntry& kBuilding = *pkBuildingInfo;
			// Make sure this wonder can be built now
			if (IsWonder(kBuilding) && HaveCityToBuild((BuildingTypes)iBldgLoop))
			{
				iWeight = m_WonderAIWeights.GetWeight((UnitTypes)iBldgLoop); // use raw weight since this wonder is essentially free
				// Don't build the UN if you aren't going for the diplo victory and have a chance of winning it
				if(pkBuildingInfo->IsDiplomaticVoting())
				{
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					int iSecuredVotes = 0;
					TeamTypes myTeamID = m_pPlayer->getTeam();
					PlayerTypes myPlayerID = m_pPlayer->GetID();

					// Loop through Players to see if they'll vote for this player
					PlayerTypes eLoopPlayer;
					TeamTypes eLoopTeam;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(eLoopPlayer).isAlive())
						{
							eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

							// Liberated?
							if(GET_TEAM(eLoopTeam).GetLiberatedByTeam() == myTeamID)
							{
								iSecuredVotes++;
							}

							// Minor civ?
							else if(GET_PLAYER(eLoopPlayer).isMinorCiv())
							{
								// Best Relations?
								if(GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == myPlayerID)
								{
									iSecuredVotes++;
								}
							}
						}
					}

					int iNumberOfPlayersWeNeedToBuyOff = MAX(0, iVotesNeededToWin - iSecuredVotes);

					if(!m_pPlayer->GetDiplomacyAI() || !m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() || m_pPlayer->GetTreasury()->GetGold() < iNumberOfPlayersWeNeedToBuyOff * 500 )
					{
						iWeight = 0;
					}
				}
				// ??? do we want to weight it more for more expensive wonders?
				m_Buildables.push_back(iBldgLoop, iWeight);
			}
		}
	}

	// Sort items and grab the first one
	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleWonders();

		if(m_Buildables.GetTotalWeight() > 0)
		{
			int iNumChoices = 1;
			eSelection = (BuildingTypes)m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing wonder from Top Choices");
			iWonderWeight = m_Buildables.GetTotalWeight();

			// first check if the wonder city can build it
			if (pWonderCity->canConstruct(eSelection))
			{
				pCityToBuildAt = pWonderCity;
			}
			// if it can't then check for other cities
			else
			{
				CvCity* pLoopCity;
				int iLoop;
				for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
				{
					if(pLoopCity->canConstruct(eSelection))
					{
						pCityToBuildAt = pLoopCity;
						break; // todo: find the best city 
					}
				}
			}

			return eSelection;
		}

		// Nothing with any weight
		else
		{
			return NO_BUILDING;
		}
	}

	// Unless we didn't find any
	else
	{
		return NO_BUILDING;
	}
}


/// Log all potential builds
void CvWonderProductionAI::LogPossibleWonders()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ
		CvString playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog = LOGFILEMGR.GetLog(m_pPlayer->GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";



		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvString strOutBuf = strBaseString;

			CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(m_Buildables.GetElement(iI));
			if(pEntry != NULL)
			{
				CvString strDesc = pEntry->GetDescription();
				CvString strTemp;
				strTemp.Format("Wonder, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;

			}

			pLog->Msg(strOutBuf);
		}
	}
}

/// Stub - Probably don't need to log flavors to city specialization log -- is in enough places already
void CvWonderProductionAI::LogFlavors(FlavorTypes)
{
}

/// Check to make sure this is one of the buildings we consider to be a wonder
bool CvWonderProductionAI::IsWonder(const CvBuildingEntry& kBuilding) const
{
	const CvBuildingClassInfo& kBuildingClass = kBuilding.GetBuildingClassInfo();

	if(::isWorldWonderClass(kBuildingClass) ||
	        ::isTeamWonderClass(kBuildingClass) ||
	        ::isNationalWonderClass(kBuildingClass))
	{
		return true;
	}
	return false;
}

// PRIVATE METHODS

/// Check to make sure some city can build this wonder
bool CvWonderProductionAI::HaveCityToBuild(BuildingTypes eBuilding) const
{
	CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canConstruct(eBuilding))
		{
			return true;
		}
	}
	return false;
}
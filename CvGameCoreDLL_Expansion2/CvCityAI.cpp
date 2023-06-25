/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvCityAI.h"
#include "CvPlot.h"
#include "CvArea.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CvInfos.h"
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvEnumSerialization.h"
#include "CvCitySpecializationAI.h"
#include "CvWonderProductionAI.h"
#include "CvGrandStrategyAI.h"
#include "cvStopWatch.h"
#include "CvInternalGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"

OBJECT_VALIDATE_DEFINITION(CvCityAI)

// Public Functions...
CvCityAI::CvCityAI()
{
	OBJECT_ALLOCATED
	AI_reset();
}

CvCityAI::~CvCityAI()
{
	AI_uninit();
	OBJECT_DESTROYED
}

void CvCityAI::AI_init()
{
	VALIDATE_OBJECT
	AI_reset();
}

void CvCityAI::AI_uninit()
{
	VALIDATE_OBJECT
}

// FUNCTION: AI_reset()
// Initializes data members that are serialized.
void CvCityAI::AI_reset()
{
	VALIDATE_OBJECT
	AI_uninit();

	m_bChooseProductionDirty = false;

	m_mapPlotsAcquiredByOtherPlayers.clear();
	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;
}

void CvCityAI::AI_doTurn()
{
}

void CvCityAI::AI_chooseProduction(bool bInterruptWonders, bool bInterruptBuildings)
{
	VALIDATE_OBJECT
	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	CvCitySpecializationAI* pSpecializationAI = kOwner.GetCitySpecializationAI();
	bool bBuildWonder = false;

	bool bAlreadyBuildingWonder = false;

	const BuildingTypes eBuilding = getProductionBuilding();
	CvBuildingEntry* pkBuilding = (eBuilding != NO_BUILDING) ? GC.getBuildingInfo(eBuilding) : NULL;
	if (pkBuilding)
	{
		if (kOwner.GetWonderProductionAI()->IsWonder(*pkBuilding))
			bAlreadyBuildingWonder = true;
		if (bAlreadyBuildingWonder && !bInterruptWonders)
			return;
		else if (!bInterruptBuildings)
			return;
	}

	// Has the designated wonder been poached by another civ?
	BuildingTypes eNextWonder = pSpecializationAI->GetNextWonderDesired(); 

	// Is it still working on that wonder and we don't want to interrupt it?
	bool bAlreadyBuilt = false;
	if (eNextWonder != NO_BUILDING)
	{
		CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eNextWonder);
		if (pkBuilding)
		{
			const CvBuildingClassInfo& kBuildingClass = pkBuilding->GetBuildingClassInfo();
			if (::isWorldWonderClass(kBuildingClass))
			{
				bAlreadyBuilt = GC.getGame().getBuildingClassCreatedCount((BuildingClassTypes)GC.getBuildingInfo(eNextWonder)->GetBuildingClassType()) > 0;
			}
		}
	}

	if (bAlreadyBuilt)
	{
		// Reset city specialization
		kOwner.GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL);
	}

	if (!bAlreadyBuildingWonder && bInterruptWonders && eNextWonder != NO_BUILDING)
	{		
		CvBuildingEntry* pkBuilding = GC.getBuildingInfo(eNextWonder);
		if (pkBuilding)
		{
			if (IsBestForWonder(pkBuilding->GetBuildingClassType()))
			{
				if (kOwner.GetNumUnitsWithUnitAI(UNITAI_ENGINEER, false) > 0)
					bBuildWonder = true;
			}
		}
	}

	if(bBuildWonder)
	{
		CvCityBuildable buildable;
		buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
		buildable.m_iIndex = eNextWonder;
		buildable.m_iTurnsToConstruct = getProductionTurnsLeft((BuildingTypes)buildable.m_eBuildableType, 0);
		pushOrder(ORDER_CONSTRUCT, buildable.m_iIndex, -1, false, false, false, false);

		if(GC.getLogging() && GC.getAILogging())
		{
			m_pCityStrategyAI->LogCityProduction(buildable, false);

			CvString strBaseString;
			CvString strOutBuf;
			CvString playerName = kOwner.getCivilizationShortDescription();
			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";
			strOutBuf.Format("%s, WONDER - Started %s, Turns: %d", getName().GetCString(), GC.getBuildingInfo((BuildingTypes)buildable.m_iIndex)->GetDescription(), buildable.m_iTurnsToConstruct);
			strBaseString += strOutBuf;
			kOwner.GetCitySpecializationAI()->LogMsg(strBaseString);
		}
	}
	else
	{
#if defined(MOD_BALANCE_CORE)
		m_pCityStrategyAI->ChooseProduction(NO_BUILDING, NO_UNIT, bInterruptBuildings, bInterruptWonders);
#else
		m_pCityStrategyAI->ChooseProduction();
#endif
		AI_setChooseProductionDirty(false);
	}

	return;
}

bool CvCityAI::AI_isChooseProductionDirty()
{
	VALIDATE_OBJECT
	return m_bChooseProductionDirty;
}

void CvCityAI::AI_setChooseProductionDirty(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bChooseProductionDirty = bNewValue;
}

/// How many of our City's plots have been grabbed by someone else?
int CvCityAI::AI_GetNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer) const
{
	VALIDATE_OBJECT
	FAssert(ePlayer < MAX_PLAYERS);
	FAssert(ePlayer > -1);
	
	map<PlayerTypes,int>::const_iterator it = m_mapPlotsAcquiredByOtherPlayers.find(ePlayer);
	if (it != m_mapPlotsAcquiredByOtherPlayers.end())
		return it->second;

	return 0;
}

/// Changes how many of our City's plots have been grabbed by someone else
void CvCityAI::AI_ChangeNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer, int iChange)
{
	VALIDATE_OBJECT
	FAssert(ePlayer < MAX_PLAYERS);
	FAssert(ePlayer > -1);

	m_mapPlotsAcquiredByOtherPlayers[ePlayer] += iChange;

	if (m_mapPlotsAcquiredByOtherPlayers[ePlayer] == 0)
		m_mapPlotsAcquiredByOtherPlayers.erase(ePlayer);
}

#if defined(MOD_BALANCE_CORE_EVENTS)
void CvCityAI::AI_DoEventChoice(CityEventTypes eChosenEvent)
{
	if(eChosenEvent != NO_EVENT_CITY)
	{
		CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eChosenEvent);
		if(pkEventInfo != NULL)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString playerName;
				FILogFile* pLog = NULL;
				CvString strBaseString;
				CvString strOutBuf;
				CvString strFileName = "EventCityLogging.csv";
				playerName = getNameKey();
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
				strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
				strBaseString += playerName + ", ";
				strOutBuf.Format("AI considering Event choices for City: %s", pkEventInfo->GetDescription());
				strBaseString += strOutBuf;
				pLog->Msg(strBaseString);
			}
			//Lua Hook
			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_OverrideAICityEventChoice, getOwner(), GetID(), eChosenEvent) == GAMEEVENTRETURN_TRUE) 
			{
				return;
			}
			// Now let's get the event flavors.
			CvWeightedVector<int> flavorChoices;
			for(int iLoop = 0; iLoop < GC.getNumCityEventChoiceInfos(); iLoop++)
			{
				CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iLoop;
				if(eEventChoice != NO_EVENT_CHOICE_CITY)
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						if(IsCityEventChoiceValid(eEventChoice, eChosenEvent))
						{
							for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
							{
								if(pkEventChoiceInfo->getFlavorValue(iFlavor) > 0)
								{
									int iOurFlavor = GET_PLAYER(getOwner()).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavor);
									iOurFlavor += pkEventChoiceInfo->getFlavorValue(iFlavor);
									flavorChoices.push_back(eEventChoice, iOurFlavor);
								}
							}
						}
					}
				}
			}
			if(flavorChoices.size() > 0)
			{
				//sort em!
				flavorChoices.StableSortItems();
				
				//And grab the top selection.
				CityEventChoiceTypes eBestEventChoice = (CityEventChoiceTypes)flavorChoices.GetElement(0);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						CvString playerName;
						FILogFile* pLog = NULL;
						CvString strBaseString;
						CvString strOutBuf;
						CvString strFileName = "EventCityLogging.csv";
						playerName = getNameKey();
						pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
						strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
						strBaseString += playerName + ", ";
						strOutBuf.Format("AI made a flavor event choice for a City: %s", pkEventChoiceInfo->GetDescription());
						strBaseString += strOutBuf;
						pLog->Msg(strBaseString);
					}
				}

				//If didn't find something (probably because a modder forgot to set flavors...), do a random selection.
				if(eBestEventChoice != NO_EVENT_CHOICE_CITY)
				{
					DoEventChoice(eBestEventChoice, NO_EVENT_CITY, false);
					return;
				}
			}
			//If we got here, it is because we haven't made a choice yet. Do so now.
			CvWeightedVector<int> randomChoices;
			for(int iLoop = 0; iLoop < GC.getNumCityEventChoiceInfos(); iLoop++)
			{
				CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iLoop;
				if(eEventChoice != NO_EVENT_CHOICE_CITY)
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						if(IsCityEventChoiceValid(eEventChoice, eChosenEvent))
						{
							int iRandom = GC.getGame().getJonRandNum(pkEventInfo->getNumChoices(), "Random Event Choice");
							if(iRandom <= 0)
							{
								iRandom = 1;
							}
							randomChoices.push_back(eEventChoice, iRandom);
						}
					}
				}
			}
			randomChoices.StableSortItems();
				
			//And grab the top selection.
			CityEventChoiceTypes eBestEventChoice = (CityEventChoiceTypes)randomChoices.GetElement(0);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestEventChoice);
				if(pkEventChoiceInfo != NULL)
				{
					CvString playerName;
					FILogFile* pLog = NULL;
					CvString strBaseString;
					CvString strOutBuf;
					CvString strFileName = "EventCityLogging.csv";
					playerName = getNameKey();
					pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
					strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
					strBaseString += playerName + ", ";
					strOutBuf.Format("AI made a random event choice for City: %s", pkEventChoiceInfo->GetDescription());
					strBaseString += strOutBuf;
					pLog->Msg(strBaseString);
				}
			}

			//If didn't find something (probably because a modder forgot to set flavors...), do a random selection.
			if(eBestEventChoice != NO_EVENT_CHOICE_CITY)
			{
			    DoEventChoice(eBestEventChoice, NO_EVENT_CITY, false);
				return;
			}
		}
	}
}
#endif

template<typename CityAI, typename Visitor>
void CvCityAI::Serialize(CityAI& cityAI, Visitor& visitor)
{
	visitor(cityAI.m_bChooseProductionDirty);
	visitor(cityAI.m_iCachePlayerClosenessTurn);
	visitor(cityAI.m_iCachePlayerClosenessDistance);
	visitor(cityAI.m_mapPlotsAcquiredByOtherPlayers);
}

//
//
//
void CvCityAI::read(FDataStream& kStream)
{
	VALIDATE_OBJECT
	CvCity::read(kStream);

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

//
//
//
void CvCityAI::write(FDataStream& kStream) const
{
	VALIDATE_OBJECT
	CvCity::write(kStream);

	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator<<(FDataStream& saveTo, const CvCityAI& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvCityAI& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

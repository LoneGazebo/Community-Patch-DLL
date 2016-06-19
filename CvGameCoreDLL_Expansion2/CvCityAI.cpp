/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

	for(int iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		m_aiPlayerCloseness[iI] = 0;
		m_aiNumPlotsAcquiredByOtherPlayers[iI] = 0;
	}
	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;
}

void CvCityAI::AI_doTurn()
{
	AI_PERF_FORMAT("City-AI-perf.csv", ("CvCityAI::AI_doTurn, Turn %03d, %s, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription(), getName().c_str()) );
	VALIDATE_OBJECT
	if(!isHuman())
	{
		AI_stealPlots();
	}
}
#if defined(MOD_BALANCE_CORE)
void CvCityAI::AI_chooseProduction(bool bInterruptWonders, bool bInterruptBuildings)
#else
void CvCityAI::AI_chooseProduction(bool bInterruptWonders)
#endif
{
	VALIDATE_OBJECT
	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	CvCitySpecializationAI* pSpecializationAI = kOwner.GetCitySpecializationAI();
	bool bBuildWonder = false;

	// See if this is the one AI city that is supposed to be building wonders
	if(pSpecializationAI->GetWonderBuildCity() == this)
	{
		// Is it still working on that wonder and we don't want to interrupt it?
		if(!bInterruptWonders)
		{
			const BuildingTypes eBuilding = getProductionBuilding();
			CvBuildingEntry* pkBuilding = (eBuilding != NO_BUILDING)? GC.getBuildingInfo(eBuilding) : NULL;
			if(pkBuilding && kOwner.GetWonderProductionAI()->IsWonder(*pkBuilding))
			{
				return;  // Stay the course
			}
		}

		// So we're the wonder building city but it is not underway yet...

		// Has the designated wonder been poached by another civ?
		BuildingTypes eNextWonder = pSpecializationAI->GetNextWonderDesired();
		if(!canConstruct(eNextWonder))
		{
			// Reset city specialization
			kOwner.GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL);
		}
		else
		{
#if defined(MOD_AI_SMART_V3)
			bool checkBuildWonder = true;
			
			// All of this only has sense if the AI is able to expand...
			if (MOD_AI_SMART_V3 && !GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
			{
				int cityExpansionFlavor = m_pCityStrategyAI->GetLatestFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
				int cityWonderFlavor = m_pCityStrategyAI->GetLatestFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_WONDER"));
				// Check if at city, the player has more desire to expand than to build wonders.
				if ((cityExpansionFlavor - cityWonderFlavor) > 0)
				{
					int currentCityProd	 = 0;
					int allOthercitiesProd = 0;
					CvCity* pLoopCity = NULL;
					int iLoop = 0;
					// Lets check production of wonder city vs production in all cities.
					for(pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
					{
						int cityProd = pLoopCity->getCurrentProductionDifference(true, false);
						if (pLoopCity == this)
						{
							currentCityProd = cityProd;
						}
						else
						{
							allOthercitiesProd += cityProd;
						}
					}
					bool bMostProductionInCity = (currentCityProd - (allOthercitiesProd * 2)) > 0;
					// If the production of city equals 66% all empire production, lets stop wonder choose.
					checkBuildWonder = !bMostProductionInCity;
				}			
			}
			
			if (checkBuildWonder)
			{
#endif
			// to prevent us from continuously locking into building wonders in one city when there are other high priority items to build
			int iFlavorWonder = kOwner.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_WONDER"));
			int iFlavorGP = kOwner.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GREAT_PEOPLE"));
			int iFlavor = (iFlavorWonder > iFlavorGP ) ? iFlavorWonder : iFlavorGP;
			if (GC.getGame().getJonRandNum(11, "Random roll for whether to continue building wonders") <= iFlavor)
				bBuildWonder = true;
#if defined(MOD_AI_SMART_V3)
			}
#endif
		}
	}

	if(bBuildWonder)
	{
		CvCityBuildable buildable;
		buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
		buildable.m_iIndex = pSpecializationAI->GetNextWonderDesired();
		buildable.m_iTurnsToConstruct = getProductionTurnsLeft((BuildingTypes)buildable.m_eBuildableType, 0);
		pushOrder(ORDER_CONSTRUCT, buildable.m_iIndex, -1, false, false, false, false);

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString playerName;
			FILogFile* pLog;
			CvString strBaseString;
			CvString strOutBuf;

			m_pCityStrategyAI->LogCityProduction(buildable, false);

			playerName = kOwner.getCivilizationShortDescription();
			pLog = LOGFILEMGR.GetLog(kOwner.GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);
			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";
			strOutBuf.Format("%s, WONDER - Started %s, Turns: %d", getName().GetCString(), GC.getBuildingInfo((BuildingTypes)buildable.m_iIndex)->GetDescription(), buildable.m_iTurnsToConstruct);
			strBaseString += strOutBuf;
			pLog->Msg(strBaseString);
		}
	}

	else
	{
#if defined(MOD_BALANCE_CORE)
		m_pCityStrategyAI->ChooseProduction(NO_BUILDING, NO_UNIT, bInterruptBuildings);
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

void CvCityAI::AI_stealPlots()
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot = 0;
	int iI = 0;


	for(iI = 0; iI < GetNumWorkablePlots(); iI++)
	{
		pLoopPlot = iterateRingPlots(getX(),getY(),iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getWorkingCityOverride() == this)
			{
				if(pLoopPlot->getOwner() != getOwner())
				{
					pLoopPlot->setWorkingCityOverride(NULL);
				}
			}
		}
	}
}

/// How many of our City's plots have been grabbed by someone else?
int CvCityAI::AI_GetNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer) const
{
	VALIDATE_OBJECT
	FAssert(ePlayer < MAX_PLAYERS);
	FAssert(ePlayer > -1);

	return m_aiNumPlotsAcquiredByOtherPlayers[ePlayer];
}

/// Changes how many of our City's plots have been grabbed by someone else
void CvCityAI::AI_ChangeNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer, int iChange)
{
	VALIDATE_OBJECT
	FAssert(ePlayer < MAX_PLAYERS);
	FAssert(ePlayer > -1);

	m_aiNumPlotsAcquiredByOtherPlayers[ePlayer] += iChange;
}
#if defined(MOD_BALANCE_CORE_EVENTS)
void CvCityAI::AI_DoEventChoice(CityEventTypes eChosenEvent)
{
	if(eChosenEvent != NO_EVENT)
	{
		CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eChosenEvent);
		if(pkEventInfo != NULL)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString playerName;
				FILogFile* pLog;
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
			CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS * 2, true> flavorChoices;
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
				flavorChoices.SortItems();
				
				//And grab the top selection.
				CityEventChoiceTypes eBestEventChoice = (CityEventChoiceTypes)flavorChoices.GetElement(0);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						CvString playerName;
						FILogFile* pLog;
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
				if(eBestEventChoice != NO_EVENT_CHOICE)
				{
					DoEventChoice(eBestEventChoice);
					return;
				}
			}
			//If we got here, it is because we haven't made a choice yet. Do so now.
			CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> randomChoices;
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
			randomChoices.SortItems();
				
			//And grab the top selection.
			CityEventChoiceTypes eBestEventChoice = (CityEventChoiceTypes)randomChoices.GetElement(0);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestEventChoice);
				if(pkEventChoiceInfo != NULL)
				{
					CvString playerName;
					FILogFile* pLog;
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
			if(eBestEventChoice != NO_EVENT_CHOICE)
			{
				DoEventChoice(eBestEventChoice);
				return;
			}
		}
	}
}
#endif

//
//
//
void CvCityAI::read(FDataStream& kStream)
{
	VALIDATE_OBJECT
	CvCity::read(kStream);

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_bChooseProductionDirty;
	kStream >> m_iCachePlayerClosenessTurn;
	kStream >> m_iCachePlayerClosenessDistance;
	kStream >> m_aiPlayerCloseness;
	kStream >> m_aiNumPlotsAcquiredByOtherPlayers;
}

//
//
//
void CvCityAI::write(FDataStream& kStream) const
{
	VALIDATE_OBJECT
	CvCity::write(kStream);

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_bChooseProductionDirty;
	kStream << m_iCachePlayerClosenessTurn;
	kStream << m_iCachePlayerClosenessDistance;
	kStream << m_aiPlayerCloseness;
	kStream << m_aiNumPlotsAcquiredByOtherPlayers;
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

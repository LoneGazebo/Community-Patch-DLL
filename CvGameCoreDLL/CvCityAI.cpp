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

	for (int iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		m_aiPlayerCloseness[iI] = 0;
		m_aiNumPlotsAcquiredByOtherPlayers[iI] = 0;
	}
	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;
}

void CvCityAI::AI_doTurn()
{
	VALIDATE_OBJECT
    if (!isHuman())
	{
	    AI_stealPlots();
	}
}

void CvCityAI::AI_chooseProduction(bool bInterruptWonders)
{
	VALIDATE_OBJECT
	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	CvCitySpecializationAI *pSpecializationAI = kOwner.GetCitySpecializationAI();
	bool bBuildWonder = false;

	// See if this is the one AI city that is supposed to be building wonders
	if (pSpecializationAI->GetWonderBuildCity() == this)
	{
		// Is it still working on that wonder and we don't want to interrupt it?
		if (!bInterruptWonders)
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
		if (!canConstruct(eNextWonder))
		{
			// Reset city specialization
			kOwner.GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL);
		}
		else
		{
			bBuildWonder = true;
		}
	}

	if (bBuildWonder)
	{
		CvCityBuildable buildable;
		buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
		buildable.m_iIndex = pSpecializationAI->GetNextWonderDesired();
		buildable.m_iTurnsToConstruct = getProductionTurnsLeft((BuildingTypes)buildable.m_eBuildableType, 0);
		pushOrder(ORDER_CONSTRUCT, buildable.m_iIndex, -1, false, false, false, false);

		if (GC.getLogging() && GC.getAILogging())
		{
			CvString playerName;
			FILogFile *pLog;
			CvString strBaseString;
			CvString strOutBuf;

			m_pCityStrategyAI->LogCityProduction(buildable, false);

			playerName = kOwner.getCivilizationShortDescription();
			pLog = LOGFILEMGR.GetLog(kOwner.GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);
			strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";
			strOutBuf.Format("%s, WONDER - Started %s, Turns: %d", getName().GetCString(), GC.getBuildingInfo((BuildingTypes)buildable.m_iIndex)->GetDescription(), buildable.m_iTurnsToConstruct);
			strBaseString += strOutBuf;
			pLog->Msg(strBaseString);
		}
	}

	else
	{
		m_pCityStrategyAI->ChooseProduction(false);
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

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
    {
        pLoopPlot = plotCity(getX(),getY(),iI);

        if (pLoopPlot != NULL)
        {
            if (pLoopPlot->getWorkingCityOverride() == this)
            {
                if (pLoopPlot->getOwner() != getOwner())
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

	kStream << m_bChooseProductionDirty;
	kStream << m_iCachePlayerClosenessTurn;
	kStream << m_iCachePlayerClosenessDistance;
	kStream << m_aiPlayerCloseness;
	kStream << m_aiNumPlotsAcquiredByOtherPlayers;
}

FDataStream & operator<<(FDataStream & saveTo, const CvCityAI & readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}
FDataStream & operator>>(FDataStream & loadFrom, CvCityAI & writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

﻿/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "ICvDLLUserInterface.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "CvRandom.h"
#include "CvInfos.h"
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvEconomicAI.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "CvMinorCivAI.h"
#include "CvUnitCombat.h"
#include "CvDLLUtilDefines.h"
#include "CvInfosSerializationHelper.h"
#include "CvBarbarians.h"
#include "CvDiplomacyAI.h"
#include "CvDllPlot.h"
#include "CvDllUnit.h"
#include "CvUnitMovement.h"
#include "CvTargeting.h"
#include "CvTypes.h"
// Include this after all other headers.
#include "LintFree.h"

// Public Functions...

//------------------------------------------------------------------------------
// CvPlot Version History
// Version 5
//	 * ImprovementType is now hashed.
// Version 6
//	 * Improvement Revealed array is now hashed
// Version 7
//   * Added m_eImprovementTypeUnderConstruction variable to be serialized
//------------------------------------------------------------------------------
const int g_CurrentCvPlotVersion = 7;

//	--------------------------------------------------------------------------------
namespace FSerialization
{

std::set<int> plotsToCheck;

void SyncPlots()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes authoritativePlayer = GC.getGame().getActivePlayer();
		std::set<int>::const_iterator i;
		for(i = plotsToCheck.begin(); i != plotsToCheck.end(); ++i)
		{
			const CvPlot* plot = GC.getMap().plotByIndexUnchecked(*i);

			if(plot)
			{
				const FAutoArchive& archive = plot->getSyncArchive();
				if(archive.hasDeltas())
				{
					FMemoryStream memoryStream;
					std::vector<std::pair<std::string, std::string> > callStacks;
					archive.saveDelta(memoryStream, callStacks);
					gDLL->sendPlotSyncCheck(authoritativePlayer, plot->getX(), plot->getY(), memoryStream, callStacks);
				}
			}
		}
	}
}

// clears ALL deltas for ALL plots
void ClearPlotDeltas()
{
	std::set<int>::iterator i;
	for(i = plotsToCheck.begin(); i != plotsToCheck.end(); ++i)
	{
		CvPlot* plot = GC.getMap().plotByIndex(*i);

		if(plot)
		{
			FAutoArchive& archive = plot->getSyncArchive();

			archive.clearDelta();
		}
	}
}
}

//////////////////////////////////////////////////////////////////////////
// CvArchaeologyData serialization
//////////////////////////////////////////////////////////////////////////
FDataStream& operator>>(FDataStream& loadFrom, CvArchaeologyData& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iTemp;
	loadFrom >> iTemp;
	writeTo.m_eArtifactType = (GreatWorkArtifactClass)iTemp;
	loadFrom >> writeTo.m_eEra;
	loadFrom >> writeTo.m_ePlayer1;
	loadFrom >> writeTo.m_ePlayer2;
	if (uiVersion >= 2)
	{
		loadFrom >> iTemp;
		writeTo.m_eWork = (GreatWorkType)iTemp;
	}
	else
	{
		writeTo.m_eWork = NO_GREAT_WORK;
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvArchaeologyData& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_eArtifactType;
	saveTo << readFrom.m_eEra;
	saveTo << readFrom.m_ePlayer1;
	saveTo << readFrom.m_ePlayer2;
	saveTo << readFrom.m_eWork;

	return saveTo;
}

//////////////////////////////////////////////////////////////////////////
// CvPlot
//////////////////////////////////////////////////////////////////////////
CvPlot::CvPlot() :
	m_syncArchive(*this)
	, m_eFeatureType("CvPlot::m_eFeatureType", m_syncArchive, true)
{
	FSerialization::plotsToCheck.insert(m_iPlotIndex);

	m_szScriptData = NULL;

#if defined(MOD_API_EXTENSIONS)
    // allocates memory for an array only once
    m_aiArbitraryYields = FNEW(uint8[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
    ZeroMemory(m_aiArbitraryYields, sizeof(uint8) * NUM_YIELD_TYPES);
#endif

	reset(0, 0, true);
}


//	--------------------------------------------------------------------------------
CvPlot::~CvPlot()
{
#if defined(MOD_API_EXTENSIONS)
    // releases memory mirroring the constructor
    SAFE_DELETE_ARRAY(m_aiArbitraryYields);
#endif

	std::set<int>::iterator it = FSerialization::plotsToCheck.find(m_iPlotIndex);
	if (it!=FSerialization::plotsToCheck.end())
		FSerialization::plotsToCheck.erase(it);
	uninit();
}

//	--------------------------------------------------------------------------------
void CvPlot::init(int iX, int iY)
{
	//--------------------------------
	// Init saved data
	reset(iX, iY);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
}


//	--------------------------------------------------------------------------------
void CvPlot::uninit()
{
	SAFE_DELETE_ARRAY(m_szScriptData);

	m_pCenterUnit = NULL;

	m_units.clear();
}

//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlot::reset(int iX, int iY, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iX = iX;
	m_iY = iY;

#if defined(MOD_BALANCE_CORE)
	m_iPlotIndex = GC.getMap().plotNum(m_iX,m_iY);
#endif

	m_iArea = -1;
	m_iLandmass = -1;
	m_iDummy = 0;
	m_iOwnershipDuration = 0;
	m_iImprovementDuration = 0;
	m_iUpgradeProgress = 0;
	m_iNumMajorCivsRevealed = 0;
	m_iCityRadiusCount = 0;
	m_iReconCount = 0;
	m_iRiverCrossingCount = 0;
	m_iResourceNum = 0;
	m_cContinentType = 0;

	m_bStartingPlot = false;
	m_bHills = false;
	m_bNEOfRiver = false;
	m_bWOfRiver = false;
	m_bNWOfRiver = false;
	m_bPotentialCityWork = false;
	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_bImprovementPassable = false;
	m_bImprovementPillaged = false;
	m_bRoutePillaged = false;
	m_bBarbCampNotConverting = false;
	m_bHighMoveCost = false;
	m_bResourceLinkedCityActive = false;
	m_bImprovedByGiftFromMajor = false;
	m_bIsImpassable = false;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_bImprovementEmbassy = false;
#endif

	m_bIsFreshwater = false;
	m_bIsAdjacentToLand = false;
	m_bIsAdjacentToOcean = false;
	m_bIsLake = false;

	m_eOwner = NO_PLAYER;
	m_ePlotType = PLOT_OCEAN;
	m_eTerrainType = NO_TERRAIN;
	m_eFeatureType = NO_FEATURE;
	m_eResourceType = NO_RESOURCE;
	m_eImprovementType = NO_IMPROVEMENT;
	m_eImprovementTypeUnderConstruction = NO_IMPROVEMENT;
	m_ePlayerBuiltImprovement = NO_PLAYER;
	m_ePlayerResponsibleForImprovement = NO_PLAYER;
	m_ePlayerResponsibleForRoute = NO_PLAYER;
	m_ePlayerThatClearedBarbCampHere = NO_PLAYER;
#if defined(MOD_BALANCE_CORE)
	m_iUnitPlotExperience = 0;
	m_iUnitPlotGAExperience = 0;
	m_iPlotChangeMoves = 0;
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	m_ePlayerThatClearedDigHere = NO_PLAYER;
#endif
	m_eRouteType = NO_ROUTE;
#if defined(MOD_GLOBAL_STACKING_RULES)
	m_eUnitIncrement = 0;
#endif
	m_eWorldAnchor = NO_WORLD_ANCHOR;
	m_cWorldAnchorData = NO_WORLD_ANCHOR;
	m_eRiverEFlowDirection = NO_FLOWDIRECTION;
	m_eRiverSEFlowDirection = NO_FLOWDIRECTION;
	m_eRiverSWFlowDirection = NO_FLOWDIRECTION;
	m_cRiverCrossing = 0;

	m_plotCity.reset();
	m_owningCity.reset();
	m_owningCityOverride.reset();
	m_ResourceLinkedCity.reset();
	m_purchaseCity.reset();

	if(!bConstructorCall)
	{
		m_bfRevealed.ClearAll();

		for(int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			m_aiYield[iI] = 0;
#if defined(MOD_API_EXTENSIONS)
            m_aiArbitraryYields[iI] = 0;
#endif
		}
		for(int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			m_aiPlayerCityRadiusCount[iI] = 0;
			m_aiVisibilityCount[iI] = 0;
			m_aiRevealedOwner[iI] = -1;
			m_abResourceForceReveal[iI] = false;
			m_aeRevealedImprovementType[iI] = NO_IMPROVEMENT;
			m_aeRevealedRouteType[iI] = NO_ROUTE;
#if defined(MOD_BALANCE_CORE)
			m_abIsImpassable[iI] = false;
			m_abStrategicRoute[iI] = false;
#endif
		}
	}

	for(int iI = 0; iI < MAX_TEAMS; ++iI)
	{
		m_paiInvisibleVisibilityUnitCount[iI] = 0;
		for(int iJ = 0; iJ < NUM_INVISIBLE_TYPES; ++iJ)
		{
			m_apaiInvisibleVisibilityCount[iI][iJ] = 0;
		}
	}

	m_kArchaeologyData.Reset();
#if defined(MOD_BALANCE_CORE)
	m_bIsTradeUnitRoute = false;
	m_iLastTurnBuildChanged = 0;
#endif
}

//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlot::setupGraphical()
{
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	updateSymbols();

	updateVisibility();
}

//	--------------------------------------------------------------------------------
void CvPlot::erase(bool bEraseUnits)
{
	IDInfo* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	FFastSmallFixedList<IDInfo, 25, true, c_eCiv5GameplayDLL > oldUnits;

	// kill units
	if (bEraseUnits)
	{
		oldUnits.clear();

		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			oldUnits.insertAtEnd(pUnitNode);
			pUnitNode = nextUnitNode(pUnitNode);
		}

		pUnitNode = oldUnits.head();

		while (pUnitNode != NULL)
		{
			pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = oldUnits.next(pUnitNode);

			if (pLoopUnit != NULL)
			{
				pLoopUnit->kill(false);
			}
		}
	}

	// kill cities
	pCity = getPlotCity();
	if (pCity != NULL)
	{
		pCity->kill();
	}

	setResourceType(NO_RESOURCE, 0);
	setImprovementType(NO_IMPROVEMENT);
	setRouteType(NO_ROUTE);
	setFeatureType(NO_FEATURE);

	// disable rivers
	setNEOfRiver(false, NO_FLOWDIRECTION);
	setNWOfRiver(false, NO_FLOWDIRECTION);
	setWOfRiver(false, NO_FLOWDIRECTION);
}

//	--------------------------------------------------------------------------------
void CvPlot::doTurn()
{
	if(isOwned())
	{
		changeOwnershipDuration(1);
	}

	if(getImprovementType() != NO_IMPROVEMENT)
	{
		changeImprovementDuration(1);
	}

#if defined(MOD_BALANCE_CORE)
	if (GetArchaeologicalRecord().m_eWork == NO_GREAT_WORK_ARTIFACT_CLASS)
	{
		ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
		ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getHIDDEN_ARTIFACT_RESOURCE());

		if (getResourceType() == eArtifactResourceType || getResourceType() == eHiddenArtifactResourceType)
		{
			setResourceType(NO_RESOURCE, 0);
		}
	}
#endif

	verifyUnitValidPlot();

	// Clear world anchor
	SetWorldAnchor(NO_WORLD_ANCHOR);

	// XXX
#ifdef _DEBUG
	{
		IDInfo* pUnitNode;
		CvUnit* pLoopUnit;

		pUnitNode = headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = nextUnitNode(pUnitNode);

			if(pLoopUnit)
			{
				CvAssertMsg(pLoopUnit->atPlot(*this), "pLoopUnit is expected to be at the current plot instance");
				if(!pLoopUnit->atPlot(*this))
				{
					removeUnit(pLoopUnit, true);
				}
			}
		}
	}
#endif
	// XXX
}


//	--------------------------------------------------------------------------------
void CvPlot::doImprovement()
{
	CvCity* pCity;
	CvString strBuffer;
	int iI;
	int iResourceNum = 0;
	CvTeam& thisTeam = GET_TEAM(getTeam());

	CvAssert(isBeingWorked() && isOwned());

	if(m_eImprovementType != NO_IMPROVEMENT)
	{
		if(m_eResourceType == NO_RESOURCE)
		{
			CvAssertMsg((0 < GC.getNumResourceInfos()), "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvPlot::doImprovement");
			int iNumResourceInfos = GC.getNumResourceInfos();
			CvImprovementEntry* thisImprovementInfo = GC.getImprovementInfo((ImprovementTypes)m_eImprovementType);
			for(iI = 0; iI < iNumResourceInfos; ++iI)
			{
				CvResourceInfo* thisResourceInfo = GC.getResourceInfo((ResourceTypes) iI);
				if (thisResourceInfo)
				{
					if(thisTeam.GetTeamTechs()->HasTech((TechTypes)(thisResourceInfo->getTechReveal())))
					{
						if(thisImprovementInfo->GetImprovementResourceDiscoverRand(iI) > 0)
						{
							if (GC.getGame().getSmallFakeRandNum(thisImprovementInfo->GetImprovementResourceDiscoverRand(iI), iI) == 0)
							{
								iResourceNum = GC.getMap().getRandomResourceQuantity((ResourceTypes)iI);
								setResourceType((ResourceTypes)iI, iResourceNum);
								this->DoFindCityToLinkResourceTo();
								if(getOwner() == GC.getGame().getActivePlayer())
								{
									pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
									if(pCity != NULL)
									{
										if(strcmp(thisResourceInfo->GetType(), "RESOURCE_ARTIFACTS") == 0)
										{
											strBuffer = GetLocalizedText("TXT_KEY_MISC_DISCOVERED_ARTIFACTS_NEAR", pCity->getNameKey());
										}
										else if(strcmp(thisResourceInfo->GetType(), "RESOURCE_HIDDEN_ARTIFACTS") == 0)
										{
											strBuffer = GetLocalizedText("TXT_KEY_MISC_DISCOVERED_HIDDEN_ARTIFACTS_NEAR", pCity->getNameKey());
										}
										else
										{
											strBuffer = GetLocalizedText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE", thisResourceInfo->GetTextKey(), pCity->getNameKey());
										}

										GC.GetEngineUserInterface()->AddCityMessage(0, pCity->GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_DISCOVERRESOURCE", MESSAGE_TYPE_MINOR_EVENT, thisResourceInfo.GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX(), getY(), true, true*/);
									}
								}

								break;
							}
						}
					}
				}
			}
		}
	}

	doImprovementUpgrade();
}

//	--------------------------------------------------------------------------------
void CvPlot::doImprovementUpgrade()
{
	if(getImprovementType() != NO_IMPROVEMENT)
	{
		ImprovementTypes eImprovementUpdrade = (ImprovementTypes)GC.getImprovementInfo(getImprovementType())->GetImprovementUpgrade();
		if(eImprovementUpdrade != NO_IMPROVEMENT)
		{
			if(isBeingWorked() || GC.getImprovementInfo(eImprovementUpdrade)->IsOutsideBorders())
			{
				changeUpgradeProgress(GET_PLAYER(getOwner()).getImprovementUpgradeRate());

				if(getUpgradeProgress() >= (GC.getGame().getImprovementUpgradeTime(getImprovementType(), this)) * 100)
				{
					setImprovementType(eImprovementUpdrade, GetPlayerThatBuiltImprovement());
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
FogOfWarModeTypes CvPlot::GetActiveFogOfWarMode() const
{
	CvGame& Game = GC.getGame();
	const TeamTypes activeTeam = Game.getActiveTeam();
	CvAssert(activeTeam != NO_TEAM);

	FogOfWarModeTypes eFOWMode = FOGOFWARMODE_UNEXPLORED;

	bool bIsDebug = Game.isDebugMode();
	if(bIsDebug || activeTeam != NO_TEAM)
	{
		if(isRevealed(activeTeam, bIsDebug))
		{
			eFOWMode = (isVisible(activeTeam, bIsDebug))? FOGOFWARMODE_OFF : FOGOFWARMODE_NOVIS;
		}
	}

	return eFOWMode;
}

//	--------------------------------------------------------------------------------
/// The fog state for the plot has changed, send an update message.
void CvPlot::updateFog(bool bDefer)
{
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	FogOfWarModeTypes eFOWMode = GetActiveFogOfWarMode();

	if(!bDefer)
	{
		//apparently this hides/reveals any units as well. no need to touch them manually. in fact it can lead to infinity recursion?
		gDLL->GameplayFOWChanged(getX(), getY(), eFOWMode, false);
	}
	else
	{
		GC.getMap().deferredFogPlots().insert(this);
	}
}

//	---------------------------------------------------------------------------
void CvPlot::updateVisibility()
{
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	setLayoutDirty(true);

	// Any unit that has visibility rules (it can become invisible to another player) needs to update itself.
	const TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	CvAssert(eActiveTeam != NO_TEAM);
	if (eActiveTeam != NO_TEAM)
	{
		IDInfo* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit)
			{
				InvisibleTypes eInvisibleType = pLoopUnit->getInvisibleType();
				if (eInvisibleType != NO_INVISIBLE)
				{
					// This unit has visibility rules, send a message that it needs to update itself.
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
					gDLL->GameplayUnitVisibility(pDllUnit.get(), (pLoopUnit->getTeam() == eActiveTeam)?true:isInvisibleVisible(eActiveTeam, eInvisibleType), true, 0.01f);
				}
				if (pLoopUnit->IsHiddenByNearbyUnit(this))
				{
					auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
					gDLL->GameplayUnitVisibility(pDllUnit.get(), (pLoopUnit->getTeam() == eActiveTeam) ? true : isInvisibleVisibleUnit(eActiveTeam), true, 0.01f);
				}
			}
		}

		// Must update the other layers as well
		CvPlotManager& kPlotManager = GC.getMap().plotManager();
		int iNumLayers = kPlotManager.GetNumLayers();
		for (int iLayer = 0; iLayer < iNumLayers; ++iLayer)
		{	
			const CvIDInfoFixedVector &kUnits = kPlotManager.GetUnitsByIndex(m_iX, m_iY, iLayer);
			for (CvIDInfoFixedVector::const_iterator itrUnit = kUnits.begin(); itrUnit != kUnits.end(); ++itrUnit)
			{
				CvUnit* pLoopUnit = ::GetPlayerUnit(*itrUnit);
				if (pLoopUnit)
				{
					InvisibleTypes eInvisibleType = pLoopUnit->getInvisibleType();
					if (eInvisibleType != NO_INVISIBLE)
					{
						// This unit has visibility rules, send a message that it needs to update itself.
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitVisibility(pDllUnit.get(), (pLoopUnit->getTeam() == eActiveTeam)?true:isInvisibleVisible(eActiveTeam, eInvisibleType), true, 0.01f);
					}
					if (pLoopUnit->IsHiddenByNearbyUnit(this))
					{
						// This unit has visibility rules, send a message that it needs to update itself.
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitVisibility(pDllUnit.get(), (pLoopUnit->getTeam() == eActiveTeam) ? true : isInvisibleVisibleUnit(eActiveTeam), true, 0.01f);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::updateSymbols()
{
	auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(this));
	gDLL->GameplayYieldMightHaveChanged(pDllPlot.get());
}


//	--------------------------------------------------------------------------------
void CvPlot::updateCenterUnit()
{
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	if(!isActiveVisible(true))
	{
		setCenterUnit(NULL);
		return;
	}

	setCenterUnit(getSelectedUnit());

	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();

	if(!getCenterUnit())
	{
		CvUnit* hBestDefender = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer());
		if(hBestDefender && hBestDefender->getDomainType() != DOMAIN_AIR && !hBestDefender->isInvisible(eActiveTeam,false))
			setCenterUnit(hBestDefender);
	}

	// okay, all of the other checks failed - if there is any unit here, make it the center unit
	if(!getCenterUnit())
	{
		IDInfo* pUnitNode = headUnitNode();

		if(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			if(pLoopUnit && !pLoopUnit->IsGarrisoned() && pLoopUnit->getDomainType() != DOMAIN_AIR && !pLoopUnit->isInvisible(eActiveTeam,false))
			{
				setCenterUnit(pLoopUnit);
			}
		}
	}

	CvUnit* pCenterUnit = getCenterUnit();
	IDInfo* pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit)
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));

			if(pCenterUnit == pLoopUnit)
			{
				gDLL->GameplayUnitVisibility(pDllUnit.get(), true, false, 0.01f);
			}
			else
			{
				gDLL->GameplayUnitVisibility(pDllUnit.get(), false, false, 0.01f);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlot::verifyUnitValidPlot()
{
	FStaticVector<IDInfo, 50, true, c_eCiv5GameplayDLL, 0> oldUnitList;

	IDInfo* pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		oldUnitList.push_back(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);
	}

	for(size_t iVectorLoop = 0; iVectorLoop < oldUnitList.size(); ++iVectorLoop)
	{
		CvUnit* pLoopUnit = GetPlayerUnit(oldUnitList[iVectorLoop]);
		if(pLoopUnit != NULL)
		{
			if(!pLoopUnit->isDelayedDeath())
			{
				if(pLoopUnit->atPlot(*this))
				{
					if(!(pLoopUnit->isCargo()))
					{
						if(!(pLoopUnit->isInCombat()))
						{
							if (!pLoopUnit->canEndTurnAtPlot(this))
							{
								if (!pLoopUnit->jumpToNearestValidPlot())
									pLoopUnit->kill(true);
							}
						}
					}
				}
			}
		}
	}

	// Unit not allowed in a plot owned by someone?
	if(isOwned())
	{
		for(size_t iVectorLoop = 0; iVectorLoop < oldUnitList.size(); ++iVectorLoop)
		{
			CvUnit* pLoopUnit = GetPlayerUnit(oldUnitList[iVectorLoop]);
			if(pLoopUnit != NULL)
			{
				if(!pLoopUnit->isDelayedDeath())
				{
					if(pLoopUnit->atPlot(*this))  // it may have jumped
					{
						if(!(pLoopUnit->isInCombat()))
						{
							if(pLoopUnit->getTeam() != getTeam()) // && getTeam() == NO_TEAM)// || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
							{
								if(isVisibleEnemyUnit(pLoopUnit))
								{
									if(!(pLoopUnit->isInvisible(getTeam(), false)))
									{
										if (!pLoopUnit->jumpToNearestValidPlot())
											pLoopUnit->kill(true);
									}
								}
							}
						}
					}
				}
			}
		}
	}
#if defined(MOD_GLOBAL_STACKING_RULES)
	else
	{
		for(size_t iVectorLoop = 0; iVectorLoop < oldUnitList.size(); ++iVectorLoop)
		{
			CvUnit* pLoopUnit = GetPlayerUnit(oldUnitList[iVectorLoop]);
			if(pLoopUnit != NULL)
			{
				if(!pLoopUnit->isDelayedDeath())
				{
					if(pLoopUnit->atPlot(*this))  // it may have jumped
					{
						if(!(pLoopUnit->isInCombat()))
						{
							for(size_t iVectorLoop2 = iVectorLoop+1; iVectorLoop2 < oldUnitList.size(); ++iVectorLoop2)
							{
								CvUnit* pLoopUnit2 = GetPlayerUnit(oldUnitList[iVectorLoop2]);
								if(pLoopUnit2 != NULL)
								{
									if(!pLoopUnit2->isDelayedDeath())
									{
										if(pLoopUnit2->atPlot(*this))  // it may have jumped
										{
											if(!(pLoopUnit2->isInCombat()))
											{
												if(atWar(pLoopUnit->getTeam(), pLoopUnit2->getTeam()))
												{
													// We have to evict the weaker of pLoopUnit and pLoopUnit2
													if (pLoopUnit->GetPower() < pLoopUnit2->GetPower())
													{
														CUSTOMLOG("Evicting player %i's %s at (%i, %i)", pLoopUnit->getOwner(), pLoopUnit->getName().c_str(), getX(), getY());
														if (!pLoopUnit->jumpToNearestValidPlot())
															pLoopUnit->kill(true);
													}
													else
													{
														CUSTOMLOG("Evicting player %i's %s at (%i, %i)", pLoopUnit2->getOwner(), pLoopUnit2->getName().c_str(), getX(), getY());
														if (!pLoopUnit2->jumpToNearestValidPlot())
															pLoopUnit2->kill(true);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
#endif
}

//	--------------------------------------------------------------------------------
// Left-over method, primarily because it is exposed to Lua.
void CvPlot::nukeExplosion(int iDamageLevel, CvUnit*)
{
	CvUnitCombat::ApplyNuclearExplosionDamage(this, iDamageLevel);
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToArea(int iAreaID) const
{
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(m_iPlotIndex);
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		if(aNeighbors[iI] != NULL)
		{
			if(aNeighbors[iI]->getArea() == iAreaID)
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToArea(const CvArea* pArea) const
{
	return isAdjacentToArea(pArea->GetID());
}


//	--------------------------------------------------------------------------------
bool CvPlot::shareAdjacentArea(const CvPlot* pPlot) const
{
	int iLastArea = -1;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			int iCurrArea = pAdjacentPlot->getArea();

			if(iCurrArea != iLastArea)
			{
				if(pPlot->isAdjacentToArea(iCurrArea))
				{
					return true;
				}

				iLastArea = iCurrArea;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacent(const CvPlot* pPlot) const
{
	if(pPlot == NULL)
		return false;

	//or is it maybe faster to check distance == 1?
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot == pPlot)
			return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isDeepWater() const
{
	if(isWater())
	{
		TerrainTypes eDeepWater = (TerrainTypes) GC.getDEEP_WATER_TERRAIN();
		if(getTerrainType() == eDeepWater)
		{
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isShallowWater() const
{
	if(isWater())
	{
		TerrainTypes eShallowWater = (TerrainTypes) GC.getSHALLOW_WATER_TERRAIN();
		if(getTerrainType() == eShallowWater)
		{
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToShallowWater() const
{
	//the result of this check should be cached (see updateWaterFlags)
	//but right now it is called only rarely, so the inefficiency doesn't matter much
	TerrainTypes eShallowWater = (TerrainTypes) GC.getSHALLOW_WATER_TERRAIN();

#if defined(MOD_BALANCE_CORE)
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
#else
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
#endif
		if(pAdjacentPlot && pAdjacentPlot->getTerrainType() == eShallowWater)
		{
			return true;
		}
	}

	return false;
}


#if defined(MOD_PROMOTIONS_CROSS_ICE)
//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToIce() const
{
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isIce())
			{
				return true;
			}
		}
	}

	return false;
}
#endif

//	--------------------------------------------------------------------------------
int CvPlot::GetSizeLargestAdjacentWater() const
{
	int iRtnValue = 0;

	if(isWater())
	{
		return iRtnValue;
	}

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isWater())
			{
				CvLandmass* pAdjacentBodyOfWater = GC.getMap().getLandmass(pAdjacentPlot->getLandmass());
				if (pAdjacentBodyOfWater && pAdjacentBodyOfWater->getNumTiles() >= iRtnValue)
				{
					iRtnValue = pAdjacentBodyOfWater->getNumTiles();
				}
			}
		}
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isVisibleWorked() const
{
	if(isBeingWorked())
	{
		if((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer) const
{
	for(int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if(GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				if((eIgnorePlayer == NO_PLAYER) || (((PlayerTypes)iI) != eIgnorePlayer))
				{
					if(isPlayerCityRadius((PlayerTypes)iI))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isLake(bool bUseCachedValue) const
{
	if (!bUseCachedValue)
		updateWaterFlags();

	return m_bIsLake;
}

bool CvPlot::isFreshWater(bool bUseCachedValue) const
{
	if (!bUseCachedValue)
		updateWaterFlags();
	return m_bIsFreshwater;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isCoastalLand(int iMinWaterSize, bool bUseCachedValue) const
{
	if (!bUseCachedValue)
		updateWaterFlags();

	if (iMinWaterSize == -1)
		return m_bIsAdjacentToOcean;
	else
	{
		//we don't have a cached value for non-standard lake sizes
		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
		for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
		{
			const CvPlot* pAdjacentPlot = aPlotsToCheck[iCount];
			if(pAdjacentPlot && pAdjacentPlot->isWater() && pAdjacentPlot->getFeatureType()!=FEATURE_ICE)
			{
				if (iMinWaterSize < 2)
					return true;

				CvLandmass* pAdjacentBodyOfWater = GC.getMap().getLandmass(pAdjacentPlot->getLandmass());
				if(pAdjacentBodyOfWater && pAdjacentBodyOfWater->getNumTiles() >= iMinWaterSize)
					return true;
			}
		}

		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToLand(bool bUseCachedValue) const
{
	if (!bUseCachedValue)
		updateWaterFlags();

	return m_bIsAdjacentToLand;
}

void CvPlot::updateWaterFlags() const
{
	//------- first check lakes and coasts
	if (isWater())
	{
		CvLandmass* pLandmass = GC.getMap().getLandmass(m_iLandmass);
		m_bIsLake = pLandmass ? pLandmass->isLake() : false;
		m_bIsAdjacentToOcean = !m_bIsLake; //always false for ocean plots (by definition)
		m_bIsAdjacentToLand = false; //may be set to true later

		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
		for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
		{
			const CvPlot* pAdjacentPlot = aPlotsToCheck[iCount];
			if(pAdjacentPlot && !pAdjacentPlot->isWater())
			{
				m_bIsAdjacentToLand = true;
				break;
			}
		}
	}
	else //land plots
	{
		m_bIsLake = false;
		m_bIsAdjacentToOcean = false; //maybe set to true later
		m_bIsAdjacentToLand = false; //always false for land plots (by definition)

		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
		for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
		{
			const CvPlot* pAdjacentPlot = aPlotsToCheck[iCount];
			if(pAdjacentPlot && pAdjacentPlot->isWater() && pAdjacentPlot->getFeatureType()!=FEATURE_ICE)
			{
				CvLandmass* pAdjacentBodyOfWater = GC.getMap().getLandmass(pAdjacentPlot->getLandmass());
				if(pAdjacentBodyOfWater && pAdjacentBodyOfWater->getNumTiles() >= GC.getMIN_WATER_SIZE_FOR_OCEAN())
				{
					m_bIsAdjacentToOcean = true;
					break;
				}
			}
		}
	}

	//------ finally freshwater
	
	m_bIsFreshwater = isRiver();
	if(isWater() || isImpassable() || isMountain())
		return;

	//now the more complex checks
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	//+1 means we check the plot itself also!
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES+1; iCount++)
	{
		const CvPlot* pLoopPlot = aPlotsToCheck[iCount];

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->isLake())
			{
				m_bIsFreshwater = true;
				return;
			}

			FeatureTypes feature_type = pLoopPlot->getFeatureType();

			if(feature_type != NO_FEATURE)
			{
				if(GC.getFeatureInfo(feature_type)->isAddsFreshWater())
				{
					m_bIsFreshwater = true;
					return;
				}
			}

#if defined(MOD_API_EXTENSIONS)
			ImprovementTypes improvement_type = pLoopPlot->getImprovementType();

			if(improvement_type != NO_IMPROVEMENT)
			{
				if(GC.getImprovementInfo(improvement_type)->IsAddsFreshWater())
				{
					m_bIsFreshwater = true;
					return;
				}
			}

			if (pLoopPlot->isCity() && pLoopPlot->getPlotCity()->isAddsFreshWater())
			{
				m_bIsFreshwater = true;
				return;
			}
#endif
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvPlot::isRiverCrossingFlowClockwise(DirectionTypes eDirection) const
{
	CvPlot* pPlot;
	switch(eDirection)
	{
	case DIRECTION_NORTHEAST:
		pPlot = plotDirection(getX(), getY(), DIRECTION_NORTHEAST);
		if(pPlot != NULL)
		{
			return (pPlot->getRiverSWFlowDirection() == FLOWDIRECTION_SOUTHEAST);
		}
		break;
	case DIRECTION_EAST:
		return (getRiverEFlowDirection() == FLOWDIRECTION_SOUTH);
		break;
	case DIRECTION_SOUTHEAST:
		return (getRiverSEFlowDirection() == FLOWDIRECTION_SOUTHWEST);
		break;
	case DIRECTION_SOUTHWEST:
		return (getRiverSWFlowDirection() == FLOWDIRECTION_NORTHWEST);
		break;
	case DIRECTION_WEST:
		pPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
		if(pPlot != NULL)
		{
			return (pPlot->getRiverEFlowDirection() == FLOWDIRECTION_NORTH);
		}
		break;
	case DIRECTION_NORTHWEST:
		pPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
		if(pPlot != NULL)
		{
			return (pPlot->getRiverSEFlowDirection() == FLOWDIRECTION_NORTHEAST);
		}
		break;
	default:
		CvAssert(false);
		break;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isRiverSide() const
{
	CvPlot* pLoopPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			if(isRiverCrossing(directionXY(this, pLoopPlot)))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isRiverConnection(DirectionTypes eDirection) const
{
#if defined(MOD_BALANCE_CORE)
	switch(eDirection)
	{
	case NO_DIRECTION:
		return false;
		break;
#else
	if(eDirection == NO_DIRECTION)
	{
		return false;
	}

	switch(eDirection)
	{
#endif
	case DIRECTION_NORTHEAST:
		return (isRiverCrossing(DIRECTION_NORTHWEST) || isRiverCrossing(DIRECTION_EAST));
		break;

	case DIRECTION_EAST:
		return (isRiverCrossing(DIRECTION_NORTHEAST) || isRiverCrossing(DIRECTION_SOUTHEAST));
		break;

	case DIRECTION_SOUTHEAST:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_SOUTHWEST));
		break;

	case DIRECTION_SOUTHWEST:
		return (isRiverCrossing(DIRECTION_SOUTHEAST) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_WEST:
		return (isRiverCrossing(DIRECTION_NORTHWEST) || isRiverCrossing(DIRECTION_SOUTHWEST));
		break;

	case DIRECTION_NORTHWEST:
		return (isRiverCrossing(DIRECTION_NORTHEAST) || isRiverCrossing(DIRECTION_WEST));
		break;

	default:
		CvAssert(false);
		break;
	}

	return false;
}

//	--------------------------------------------------------------------------------
CvPlot* CvPlot::getNeighboringPlot(DirectionTypes eDirection) const
{
	return plotDirection(getX(), getY(), eDirection);
}

//	--------------------------------------------------------------------------------
CvPlot* CvPlot::getNearestLandPlotInternal(int iDistance) const
{
	if(iDistance > GC.getMap().getGridHeight() && iDistance > GC.getMap().getGridWidth())
	{
		return NULL;
	}

	for(int iDX = -iDistance; iDX <= iDistance; iDX++)
	{
		for(int iDY = -iDistance; iDY <= iDistance; iDY++)
		{
			// bkw - revisit this, it works but is inefficient
			CvPlot* pPlot = plotXY(getX(), getY(), iDX, iDY);
			if(pPlot != NULL && !pPlot->isWater() && plotDistance(getX(), getY(), pPlot->getX(), pPlot->getY()) == iDistance)
			{
				return pPlot;
			}
		}
	}
	return getNearestLandPlotInternal(iDistance + 1);
}


//	--------------------------------------------------------------------------------
int CvPlot::getNearestLandArea() const
{
	CvPlot* pPlot = getNearestLandPlot();
	return pPlot ? pPlot->getArea() : -1;
}


//	--------------------------------------------------------------------------------
CvPlot* CvPlot::getNearestLandPlot() const
{
	return getNearestLandPlotInternal(0);
}


//	--------------------------------------------------------------------------------
int CvPlot::seeFromLevel(TeamTypes eTeam) const
{
	int iLevel;

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	// Plot recon-ed?
	if(getReconCount() > 0)
	{
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
		// By my reckoning, this just needs to be a big number
		iLevel = GC.getRECON_VISIBILITY_RANGE() * 4;
#else
		iLevel = GC.getRECON_VISIBILITY_RANGE() * 2;
#endif
	}
	// Normal visibility
	else
	{
		iLevel = (getTerrainType()!=NO_TERRAIN) ? GC.getTerrainInfo(getTerrainType())->getSeeFromLevel() : 0;
	}

	if(isMountain())
	{
		iLevel += GC.getMOUNTAIN_SEE_FROM_CHANGE();
	}

	if(isHills())
	{
		iLevel += GC.getHILLS_SEE_FROM_CHANGE();
	}

	if (isWater())
	{
		iLevel += GC.getSEAWATER_SEE_FROM_CHANGE();

		if (eTeam != NO_TEAM && GET_TEAM(eTeam).isExtraWaterSeeFrom())
		{
			iLevel++;
		}
	}
	else
		iLevel++; //land plots are "higher" than water plots, limiting visibility and range attacks from water onto land

	return iLevel;
}


//	--------------------------------------------------------------------------------
int CvPlot::seeThroughLevel(bool bIncludeShubbery) const
{
	int iLevel;

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iLevel = (getTerrainType()!=NO_TERRAIN) ? GC.getTerrainInfo(getTerrainType())->getSeeThroughLevel() : 0;

	if(bIncludeShubbery && getFeatureType() != NO_FEATURE)
	{
		iLevel += GC.getFeatureInfo(getFeatureType())->getSeeThroughChange();
	}

#if defined(MOD_BALANCE_CORE)
	if (isMountain() && (getFeatureType() == NO_FEATURE))
#else
	if (isMountain())
#endif
	{
		iLevel += GC.getMOUNTAIN_SEE_THROUGH_CHANGE();
	}

	if(isHills())
	{
		iLevel += GC.getHILLS_SEE_THROUGH_CHANGE();
	}

	if(isWater())
	{
		iLevel += GC.getSEAWATER_SEE_THROUGH_CHANGE();
	}
	else
		iLevel++; //land plots are "higher" than water plots, limiting visibility and range attacks from water onto land

	return iLevel;
}


//	--------------------------------------------------------------------------------
#if defined(MOD_API_EXTENSIONS)
void CvPlot::changeSeeFromSight(TeamTypes eTeam, DirectionTypes eDirection, int iFromLevel, bool bIncrement, InvisibleTypes eSeeInvisible, CvUnit* pUnit)
#else
void CvPlot::changeSeeFromSight(TeamTypes eTeam, DirectionTypes eDirection, int iFromLevel, bool bIncrement, InvisibleTypes eSeeInvisible)
#endif
{
	CvPlot* pPlot;
	int iThroughLevel = seeThroughLevel();

	if(iFromLevel >= iThroughLevel)
	{
		pPlot = plotDirection(getX(), getY(), eDirection);

		if(pPlot != NULL)
		{
			if((iFromLevel > iThroughLevel) || (pPlot->seeFromLevel(eTeam) > iFromLevel))
			{
#if defined(MOD_API_EXTENSIONS)
				pPlot->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, false, pUnit);
#else
				pPlot->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, false);
#endif
			}
		}
	}
}

//	--------------------------------------------------------------------------------
// while this looks more complex than the previous version, it should run much faster
#if defined(MOD_API_EXTENSIONS)
void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, InvisibleTypes eSeeInvisible, DirectionTypes eFacingDirection, CvUnit* pUnit)
#else
void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, InvisibleTypes eSeeInvisible, DirectionTypes eFacingDirection, bool bBasedOnUnit)
#endif
{
	//do nothing if range is negative, this is invalid
	if (iRange < 0)
		return;

	//range zero is dangerous, it can lead to unit stacking problems, should happen only with trade units
	if (iRange==0)
	{
		//change the visibility of this plot only
#if defined(MOD_API_EXTENSIONS)
		changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, false, pUnit);
#else
		changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, false);
#endif
		return;
	}

	vector<int>& scratchpad = GC.getMap().GetVisibilityScratchpad();

#if defined(MOD_API_EXTENSIONS)
	bool bBasedOnUnit = (pUnit != NULL);
#endif
	//check one extra outer ring
	int iRangeWithOneExtraRing = iRange + 1;

	// start in the center going NE
	int thisRing = 0;
	DirectionTypes eDirectionOfNextTileToCheck = DIRECTION_NORTHWEST;
	CvPlot* pPlotToCheck = this;
	int iDX = 0;
	int iDY = 0;
	int iCenterLevel = seeFromLevel(eTeam);
	int iPlotCounter = 0;
	int iMaxPlotNumberOnThisRing = 0;

	while(thisRing <= iRangeWithOneExtraRing)
	{
		if(pPlotToCheck)
		{
			//always reveal adjacent plots when using line of sight - but then clear it out
			if(thisRing <= 1)
			{
				if(eFacingDirection != NO_DIRECTION)
				{
#if defined(MOD_API_EXTENSIONS)
					pPlotToCheck->changeVisibilityCount(eTeam, 1, eSeeInvisible, false /*bInformExplorationTracking*/, false, pUnit);
					pPlotToCheck->changeVisibilityCount(eTeam, -1, eSeeInvisible, false /*bInformExplorationTracking*/, false, pUnit);
#else
					pPlotToCheck->changeVisibilityCount(eTeam, 1, eSeeInvisible, false /*bInformExplorationTracking*/, false);
					pPlotToCheck->changeVisibilityCount(eTeam, -1, eSeeInvisible, false /*bInformExplorationTracking*/, false);
#endif
				}
			}

			// see if this plot is in the visibility wedge
			if(shouldProcessDisplacementPlot(iDX, iDY, iRange, eFacingDirection))
			{
				if(thisRing != 0)
				{
					CvPlot* pFirstInwardPlot = NULL;
					CvPlot* pSecondInwardPlot = NULL;
					const int INVALID_RING = -1;
					const int HALF_BLOCKED = 0x01000000;
					int iRingOfFirstInwardPlot = INVALID_RING;
					int iRingOfSecondInwardPlot = INVALID_RING;
					int iFirstInwardLevel = INVALID_RING;
					int iSecondInwardLevel = INVALID_RING;
					bool bFirstHalfBlocked = false;
					bool bSecondHalfBlocked = false;

					// try to look at the two plot inwards
					switch(eDirectionOfNextTileToCheck)
					{
					case DIRECTION_NORTHEAST:
						pFirstInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_EAST);
						pSecondInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_SOUTHEAST);
						break;
					case DIRECTION_EAST:
						pFirstInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_SOUTHWEST);
						pSecondInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_SOUTHEAST);
						break;
					case DIRECTION_SOUTHEAST:
						pFirstInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_SOUTHWEST);
						pSecondInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_WEST);
						break;
					case DIRECTION_SOUTHWEST:
						pFirstInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_WEST);
						pSecondInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_NORTHWEST);
						break;
					case DIRECTION_WEST:
						pFirstInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_NORTHWEST);
						pSecondInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_NORTHEAST);
						break;
					case DIRECTION_NORTHWEST:
						pFirstInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_EAST);
						pSecondInwardPlot = plotDirection(pPlotToCheck->getX(),pPlotToCheck->getY(),DIRECTION_NORTHEAST);
						break;
					}
					if(pFirstInwardPlot)
					{
						iRingOfFirstInwardPlot = plotDistance(getX(),getY(),pFirstInwardPlot->getX(),pFirstInwardPlot->getY());
						if(iRingOfFirstInwardPlot == thisRing - 1)
						{
							iFirstInwardLevel = scratchpad[ pFirstInwardPlot->GetPlotIndex() ];
							if(iFirstInwardLevel >= HALF_BLOCKED)
							{
								iFirstInwardLevel -= HALF_BLOCKED;
								bFirstHalfBlocked = true;
							}
						}
					}
					if(pSecondInwardPlot)
					{
						iRingOfSecondInwardPlot = plotDistance(getX(),getY(),pSecondInwardPlot->getX(),pSecondInwardPlot->getY());
						if(iRingOfSecondInwardPlot == thisRing - 1)
						{
							iSecondInwardLevel = scratchpad[ pSecondInwardPlot->GetPlotIndex() ];
							if(iSecondInwardLevel >= HALF_BLOCKED)
							{
								iSecondInwardLevel -= HALF_BLOCKED;
								bSecondHalfBlocked = true;
							}
						}
					}
					int iThisPlotLevel = pPlotToCheck->seeThroughLevel(thisRing != iRangeWithOneExtraRing);
					if(iFirstInwardLevel != INVALID_RING && iSecondInwardLevel != INVALID_RING && iFirstInwardLevel != iSecondInwardLevel && !bFirstHalfBlocked && !bSecondHalfBlocked)
					{
						double fP0X = (double) getX();
						double fP0Y = (double) getY();
						double fP1X = (double) pPlotToCheck->getX();
						double fP1Y = (double) pPlotToCheck->getY();
						if(getY() & 1)
						{
							fP0X += 0.5;
						}
						if(pPlotToCheck->getY() & 1)
						{
							fP1X += 0.5;
						}

						double a = fP1Y - fP0Y;
						double b = fP0X - fP1X;
						double c = fP0Y * fP1X - fP1Y * fP0X;

						double fFirstInwardX = (double) pFirstInwardPlot->getX();
						double fFirstInwardY = (double) pFirstInwardPlot->getY();
						if(pFirstInwardPlot->getY() & 1)
						{
							fFirstInwardX += 0.5;
						}
						double fFirstDist = a * fFirstInwardX + b * fFirstInwardY + c;
						fFirstDist = abs(fFirstDist);
						// skip the extra distance since it is the same for both equations

						double fSecondInwardX = (double) pSecondInwardPlot->getX();
						double fSecondInwardY = (double) pSecondInwardPlot->getY();
						if(pSecondInwardPlot->getY() & 1)
						{
							fSecondInwardX += 0.5;
						}
						double fSecondDist = a * fSecondInwardX + b * fSecondInwardY + c;
						fSecondDist = abs(fSecondDist);
						// skip the extra distance since it is the same for both equations

						if(fFirstDist - fSecondDist > 0.05)  // we are closer to the second point
						{
							int iHighestLevel = (iSecondInwardLevel > iThisPlotLevel) ? iSecondInwardLevel : iThisPlotLevel;
							scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
							if(iSecondInwardLevel < iThisPlotLevel || ((iCenterLevel >= iSecondInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
							{								
#if defined(MOD_API_EXTENSIONS)
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
							}
						}
						else if(fSecondDist - fFirstDist > 0.05)   // we are closer to the first point
						{
							int iHighestLevel = (iFirstInwardLevel > iThisPlotLevel) ? iFirstInwardLevel : iThisPlotLevel;
							scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
							if(iFirstInwardLevel < iThisPlotLevel || ((iCenterLevel >= iFirstInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
							{								
#if defined(MOD_API_EXTENSIONS)
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
							}
						}
						else
						{
							int iHighestInwardLevel = (iFirstInwardLevel > iSecondInwardLevel) ? iFirstInwardLevel : iSecondInwardLevel;
							int iLowestInwardLevel = (iFirstInwardLevel > iSecondInwardLevel) ? iSecondInwardLevel : iFirstInwardLevel;
							int iHighestLevel = (iHighestInwardLevel > iThisPlotLevel) ? iHighestInwardLevel : iThisPlotLevel;
							int iHighestLowestLevel = (iLowestInwardLevel > iThisPlotLevel) ? iLowestInwardLevel : iThisPlotLevel;
							if(iHighestInwardLevel > iThisPlotLevel)
							{
								scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLowestLevel + HALF_BLOCKED;
							}
							else
							{
								scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
							}
							if(iLowestInwardLevel < iThisPlotLevel || ((iCenterLevel >= iLowestInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
							{								
#if defined(MOD_API_EXTENSIONS)
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
							}
						}
					}
					else if(iFirstInwardLevel != INVALID_RING && !bFirstHalfBlocked)
					{
						int iHighestLevel = (iFirstInwardLevel > iThisPlotLevel) ? iFirstInwardLevel : iThisPlotLevel;
						scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
						if(iFirstInwardLevel < iThisPlotLevel || ((iCenterLevel >= iFirstInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{							
#if defined(MOD_API_EXTENSIONS)
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
						}
					}
					else if(iSecondInwardLevel != INVALID_RING && !bSecondHalfBlocked)
					{
						int iHighestLevel = (iSecondInwardLevel > iThisPlotLevel) ? iSecondInwardLevel : iThisPlotLevel;
						scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
						if(iSecondInwardLevel < iThisPlotLevel || ((iCenterLevel >= iSecondInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{							
#if defined(MOD_API_EXTENSIONS)
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
						}
					}
					else if(iFirstInwardLevel != INVALID_RING)
					{
						int iHighestLevel = (iFirstInwardLevel > iThisPlotLevel) ? iFirstInwardLevel : iThisPlotLevel;
						scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
						if(iFirstInwardLevel < iThisPlotLevel || ((iCenterLevel >= iFirstInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{
#if defined(MOD_API_EXTENSIONS)
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
						}
					}
					else if(iSecondInwardLevel != INVALID_RING)
					{
						int iHighestLevel = (iSecondInwardLevel > iThisPlotLevel) ? iSecondInwardLevel : iThisPlotLevel;
						scratchpad[ pPlotToCheck->GetPlotIndex() ] = iHighestLevel;
						if(iSecondInwardLevel < iThisPlotLevel || ((iCenterLevel >= iSecondInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{							
#if defined(MOD_API_EXTENSIONS)
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
						}
					}
					else // I have no idea how this can happen, but...
					{
						scratchpad[ pPlotToCheck->GetPlotIndex() ] = iThisPlotLevel;
					}
				}
				else // this is the center point
				{					
#if defined(MOD_API_EXTENSIONS)
					pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2) ? true : false, pUnit);
#else
					pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
#endif
					scratchpad[ pPlotToCheck->GetPlotIndex() ] = 0;
				}
			}

		}

		int iNextDX;
		int iNextDY;
		if(iPlotCounter >= iMaxPlotNumberOnThisRing)  // we have processed all of the plots in this ring
		{
			// if that fails go out one ring in the NE direction traveling E
			eDirectionOfNextTileToCheck = DIRECTION_NORTHEAST;
			iNextDX = iDX + GC.getPlotDirectionX()[eDirectionOfNextTileToCheck];
			iNextDY = iDY + GC.getPlotDirectionY()[eDirectionOfNextTileToCheck];
			thisRing++;
			iMaxPlotNumberOnThisRing += thisRing * 6;
			eDirectionOfNextTileToCheck = DIRECTION_EAST;
			// (if that is further out than the extended range, we are done)
		}
		else
		{
			iNextDX = iDX + GC.getPlotDirectionX()[eDirectionOfNextTileToCheck];
			iNextDY = iDY + GC.getPlotDirectionY()[eDirectionOfNextTileToCheck];
			if(hexDistance(iNextDX,iNextDY) > thisRing)
			{
				// try to turn right
				eDirectionOfNextTileToCheck = GC.getTurnRightDirection(eDirectionOfNextTileToCheck);
				iNextDX = iDX + GC.getPlotDirectionX()[eDirectionOfNextTileToCheck];
				iNextDY = iDY + GC.getPlotDirectionY()[eDirectionOfNextTileToCheck];
			}
		}

		iPlotCounter++;

		iDX = iNextDX;
		iDY = iNextDY;
		pPlotToCheck = plotXYWithRangeCheck(getX(),getY(),iDX,iDY,iRangeWithOneExtraRing);
	}

}

//	--------------------------------------------------------------------------------
bool CvPlot::canSeePlot(const CvPlot* pPlot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const
{
	if(pPlot == NULL)
	{
		return false;
	}

	if(pPlot == this)
	{
		return true;
	}

	int startX = getX();
	int startY = getY();
	int destX = pPlot->getX();
	int destY = pPlot->getY();

	int iDistance = plotDistance(startX, startY, destX,  destY);
	if(iDistance <= iRange)
	{
		//find displacement
		int dy = destY - startY;

		int iX1 = xToHexspaceX(destX,  destY);
		int iX2 = xToHexspaceX(startX, startY);
		 
		int dx = iX1 - iX2;

		dx = dxWrap(dx); //world wrap
		dy = dyWrap(dy);

		//check if in facing direction
		if(shouldProcessDisplacementPlot(dx, dy, iRange, eFacingDirection))
		{
			if(iDistance == 1)
			{
				return true;
			}

			//check if anything blocking the plot
			if (CvTargeting::CanSeeDisplacementPlot(startX, startY, dx, dy, seeFromLevel(eTeam)))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::shouldProcessDisplacementPlot(int dx, int dy, int, DirectionTypes eFacingDirection) const
{
	if(eFacingDirection == NO_DIRECTION)
	{
		return true;
	}
	else if((dx == 0) && (dy == 0)) //always process this plot
	{
		return true;
	}
	else
	{
		//							NE					E		SE					SW					W		NW
		double displacements[6][2] = { {0.5f, 0.866025f}, {1, 0}, {0.5f, -0.866025f}, {-0.5f, -0.866025f}, {-1, 0}, {-0.5f, -0.866025f}};

		double directionX = displacements[eFacingDirection][0];
		double directionY = displacements[eFacingDirection][1];

		//compute angle off of direction
		double crossProduct = directionX * dy - directionY * dx; //cross product
		double dotProduct = directionX * dx + directionY * dy; //dot product

		double theta = atan2(crossProduct, dotProduct);
		double spread = 75 * (double) M_PI / 180;
		if((abs(dx) <= 1) && (abs(dy) <= 1)) //close plots use wider spread
		{
			spread = 90 * (double) M_PI / 180;
		}

		if((theta >= -spread / 2) && (theta <= spread / 2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::updateSight(bool bIncrement)
{
	IDInfo* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	int iLoop;
	int iI;

	pCity = getPlotCity();

	// Owned
	if(isOwned())
	{
#if defined(MOD_API_EXTENSIONS)
		changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION);
#else
		changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION, false);
#endif

		// if this tile is owned by a minor share the visibility with my ally
		if(pCity)
		{
			for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes ePlayer = (PlayerTypes)ui;
				if(GET_PLAYER(ePlayer).GetEspionage()->HasEstablishedSurveillanceInCity(pCity))
				{
#if defined(MOD_API_EXTENSIONS)
					changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION);
#else
					changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION, false);
#endif
				}
			}
		}

		PlayerTypes ownerID = getOwner();
		if(ownerID >= MAX_MAJOR_CIVS && ownerID != BARBARIAN_PLAYER)
		{
			CvPlayer& thisPlayer = GET_PLAYER(ownerID);
			CvMinorCivAI* pMinorCivAI = thisPlayer.GetMinorCivAI();
			if(pMinorCivAI && pMinorCivAI->GetAlly() != NO_PLAYER)
			{
#if defined(MOD_API_EXTENSIONS)
				changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION);
#else
				changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION, false);
#endif
			}
		}
	}

	pUnitNode = headUnitNode();

	// Unit
	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit)
		{
			if (pLoopUnit->canChangeVisibility())
#if defined(MOD_API_EXTENSIONS)
				changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->visibilityRange(), bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true), pLoopUnit);
#else
				changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->visibilityRange(), bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true));
#endif
		}
	}

	if(getReconCount() > 0)
	{
#if !defined(MOD_PROMOTIONS_VARIABLE_RECON)
		int iRange = GC.getRECON_VISIBILITY_RANGE();
#endif
		for(iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
			{
				if(pLoopUnit->getReconPlot() == this && pLoopUnit->canChangeVisibility())
				{
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
#if defined(MOD_API_EXTENSIONS)
					changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->reconRange(), bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true), pLoopUnit);
#else
					changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->reconRange(), bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true));
#endif
#else
#if defined(MOD_API_EXTENSIONS)
					changeAdjacentSight(pLoopUnit->getTeam(), iRange, bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true), pLoopUnit);
#else
					changeAdjacentSight(pLoopUnit->getTeam(), iRange, bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true));
#endif
#endif
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlot::updateSeeFromSight(bool bIncrement, bool bRecalculate)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;

	int iRange = GC.getUNIT_VISIBILITY_RANGE() + 1;
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	int iReconRange = GC.getRECON_VISIBILITY_RANGE() + 1;
#endif
	for(int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iPromotion);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			iRange += pkPromotionInfo->GetVisibilityChange();
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
			iReconRange += std::max(0, pkPromotionInfo->GetReconChange());
#endif
		}
	}

#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	iRange = std::max(iReconRange, iRange);
#else
	iRange = std::max(GC.getRECON_VISIBILITY_RANGE() + 1, iRange);
#endif
	iRange = std::min(8, iRange); // I don't care, I'm not looking more than 8 out, deal

	for(iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(iDY = -iRange; iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iRange);

			if(pLoopPlot != NULL)
			{
				pLoopPlot->updateSight(bIncrement);

#if defined(MOD_BALANCE_CORE)
				//hack: don't do this during map generation
				if (bRecalculate && GC.getGame().getElapsedGameTurns()>0)
					GC.getMap().LineOfSightChanged(pLoopPlot);
#endif
			}
		}
	}
}


//	--------------------------------------------------------------------------------
#if defined(MOD_API_EXTENSIONS)
bool CvPlot::canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude, bool bIgnoreCiv) const
#else
bool CvPlot::canHaveResource(ResourceTypes eResource, bool bIgnoreLatitude) const
#endif
{
	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if(eResource == NO_RESOURCE)
	{
		return true;
	}

	if(getResourceType() != NO_RESOURCE)
	{
		return false;
	}

	if(isMountain())
	{
		return false;
	}

	CvResourceInfo& thisResourceInfo = *GC.getResourceInfo(eResource);
	if(getFeatureType() != NO_FEATURE)
	{
		if(!(thisResourceInfo.isFeature(getFeatureType())))
		{
			return false;
		}

		if(!(thisResourceInfo.isFeatureTerrain(getTerrainType())))
		{
			return false;
		}
	}
	else
	{
		if(!(thisResourceInfo.isTerrain(getTerrainType())))
		{
			return false;
		}
	}

	if(isHills())
	{
		if(!(thisResourceInfo.isHills()))
		{
			return false;
		}
	}
	else if(isFlatlands())
	{
		if(!(thisResourceInfo.isFlatlands()))
		{
			return false;
		}
	}

	if(thisResourceInfo.isNoRiverSide())
	{
		if(isRiver())
		{
			return false;
		}
	}

	if(thisResourceInfo.getMinAreaSize() != -1)
	{
		if(area()->getNumTiles() < thisResourceInfo.getMinAreaSize())
		{
			return false;
		}
	}

#if defined(MOD_API_EXTENSIONS)
	if(!bIgnoreCiv && thisResourceInfo.isOnlyMinorCivs())
#else
	if(thisResourceInfo.isOnlyMinorCivs())
#endif
	{
		return false;
	}

#if defined(MOD_API_EXTENSIONS)
	if (!bIgnoreCiv && thisResourceInfo.GetRequiredCivilization() != NO_CIVILIZATION)
#else
	if (thisResourceInfo.GetRequiredCivilization() != NO_CIVILIZATION)
#endif
	{
		return false;
	}

	if(!bIgnoreLatitude)
	{
		if(getLatitude() > thisResourceInfo.getMaxLatitude())
		{
			return false;
		}

		if(getLatitude() < thisResourceInfo.getMinLatitude())
		{
			return false;
		}
	}

	if(!isPotentialCityWork())
	{
		return false;
	}

	TerrainTypes eShallowWater = (TerrainTypes) GC.getSHALLOW_WATER_TERRAIN();
	if(getTerrainType() == eShallowWater)
	{
		if(!isAdjacentToLand(false))
		{
			return false;
		}
	}


	return true;
}


//	--------------------------------------------------------------------------------
bool CvPlot::canHaveImprovement(ImprovementTypes eImprovement, PlayerTypes ePlayer, bool) const
{
	CvPlot* pLoopPlot;
	bool bValid;
	int iI;

	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	if(pkImprovementInfo == NULL)
	{
		return false;
	}

#if defined(MOD_GLOBAL_ALPINE_PASSES)
	if (MOD_GLOBAL_ALPINE_PASSES && pkImprovementInfo->IsMountainsMakesValid() && isMountain())
	{
		return true;
	}
#endif

#if defined(MOD_BALANCE_CORE)
	if(getFeatureType() != NO_FEATURE)
	{
		if (pkImprovementInfo->GetCreatedFeature() != NO_FEATURE && getFeatureType() == pkImprovementInfo->GetCreatedFeature())
		{
			return false;
		}
#if defined(MOD_PSEUDO_NATURAL_WONDER)
		if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder(true))
#else
		if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder())
#endif
		{
			return false;
		}
	}
	if(getImprovementType() != NO_IMPROVEMENT)
	{
		if(pkImprovementInfo->IsNewOwner())
		{
			return false;
		}
	}
#endif

	bValid = false;

	if(isCity())
	{
		return false;
	}

	if(!isValidMovePlot(ePlayer))
	{
		return false;
	}

	if(pkImprovementInfo->IsWater() != isWater())
	{
		return false;
	}

	if(getFeatureType() != NO_FEATURE)
	{
		if(GC.getFeatureInfo(getFeatureType())->isNoImprovement())
		{
			return false;
		}
	}

	ResourceTypes thisResource = getResourceType( ePlayer!=NO_PLAYER ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM );
	// The functionality of this line is different in Civ 4: in that game a "Valid" Resource ALLOWS an Improvement on a Tile.  In Civ 5 this makes a Resource REQUIRE a certain Improvement
	if(thisResource != NO_RESOURCE &&
	        !pkImprovementInfo->IsBuildableOnResources() &&	// Some improvements can be built anywhere
	        !pkImprovementInfo->IsImprovementResourceMakesValid(thisResource))
	{
		return false;
	}
	// If there IS a valid resource here then set validity to true (because something has to)
	else if(thisResource != NO_RESOURCE)
	{
		bValid = true;
	}

	if(pkImprovementInfo->IsNoFreshWater() && isFreshWater())
	{
		return false;
	}

	if(pkImprovementInfo->IsRequiresFlatlands() && !isFlatlands())
	{
		return false;
	}

	if(pkImprovementInfo->IsRequiresFlatlandsOrFreshWater() && !isFlatlands() && !isFreshWater())
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(pkImprovementInfo->IsAdjacentCity() && !IsAdjacentCity())
	{
		return false;
	}
#endif
	if(pkImprovementInfo->IsRequiresFeature() && (getFeatureType() == NO_FEATURE))
	{
#if defined(MOD_BALANCE_CORE)
		//Polder-specific code for lakes
		bool bLake = false;
		if(MOD_BALANCE_CORE && pkImprovementInfo->IsAdjacentLake())
		{
			for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->isLake())
					{
						bLake = true;
						break;
					}
				}
			}
		}
		if(!bLake)
		{
#endif
		return false;
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}
#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_CORE && pkImprovementInfo->IsAdjacentLake())
	{
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->isLake())
				{
					bValid = true;
					break;
				}
			}
		}
	}
#endif

	if(pkImprovementInfo->IsRequiresImprovement())
	{
		if (getImprovementType() == NO_IMPROVEMENT)
		{
			return false;
		}
		else
		{
			bValid = false;
		}
	}

	if(pkImprovementInfo->IsCoastal() && !isCoastalLand())
	{
		return false;
	}

	if(pkImprovementInfo->IsHillsMakesValid() && isHills())
	{
		bValid = true;
	}

	if(pkImprovementInfo->IsWaterAdjacencyMakesValid())
	{
		if (isCoastalLand() || isFreshWater() || isRiver()) 
		{
			bValid = true;
		}
	}

	if(pkImprovementInfo->IsFreshWaterMakesValid() && isFreshWater())
	{
		bValid = true;
	}

	if(pkImprovementInfo->IsRiverSideMakesValid() && isRiver())
	{
		bValid = true;
	}

	if(pkImprovementInfo->GetTerrainMakesValid(getTerrainType()))
	{
		bValid = true;
	}

	if((getImprovementType() != NO_IMPROVEMENT) && pkImprovementInfo->GetImprovementMakesValid(getImprovementType()))
	{
		bValid = true;
	}

	if((getFeatureType() != NO_FEATURE) && pkImprovementInfo->GetFeatureMakesValid(getFeatureType()))
	{
		bValid = true;
	}

	if(!bValid)
	{
		return false;
	}

	if(pkImprovementInfo->IsRiverSideMakesValid())
	{
		bValid = false;

		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pLoopPlot != NULL)
			{
				if(isRiverCrossing(directionXY(this, pLoopPlot)))
				{
					if(pLoopPlot->getImprovementType() != eImprovement)
					{
						bValid = true;
						break;
					}
				}
			}
		}

		if(!bValid)
		{
			return false;
		}
	}

	int iRequiredAdjacentLand = pkImprovementInfo->GetRequiresXAdjacentLand();
	if (iRequiredAdjacentLand > -1)
	{
		int iAdjacentLand = 0;

		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pLoopPlot != NULL)
			{
				if (!pLoopPlot->isWater())
				{
					iAdjacentLand++;
				}
			}
		}

		if (iAdjacentLand < iRequiredAdjacentLand)
		{
			return false;
		}
	}

#if defined(MOD_API_EXTENSIONS)
	int iRequiredAdjacentWater = pkImprovementInfo->GetRequiresXAdjacentWater();
	if (iRequiredAdjacentWater > -1)
	{
		int iAdjacentWater = 0;

		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pLoopPlot != NULL)
			{
				if (pLoopPlot->isWater())
				{
					iAdjacentWater++;
				}
			}
		}

		if (iAdjacentWater < iRequiredAdjacentWater)
		{
			return false;
		}
	}
#endif

	for(iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		//Simplification - errata yields not worth considering.
		if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

#if defined(MOD_BALANCE_CORE)
		if (pkImprovementInfo->GetPrereqNatureYield(iI) > 0 && calculateNatureYield(((YieldTypes)iI), ePlayer, NULL) < pkImprovementInfo->GetPrereqNatureYield(iI))
#else
		if(calculateNatureYield(((YieldTypes)iI), eTeam) < pkImprovementInfo->GetPrereqNatureYield(iI))
#endif
		{
			return false;
		}
	}

#if defined(MOD_EVENTS_PLOT)
	if (MOD_EVENTS_PLOT) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlotCanImprove, getX(), getY(), eImprovement) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}


//	--------------------------------------------------------------------------------
bool CvPlot::canBuild(BuildTypes eBuild, PlayerTypes ePlayer, bool bTestVisible, bool bTestPlotOwner) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	ImprovementTypes eImprovement;
	ImprovementTypes eFinalImprovementType;
	RouteTypes eRoute;
	bool bValid = false;

	// Can't build nothing!
	if(eBuild == NO_BUILD)
	{
		return false;
	}

	// Repairing an Improvement that's been pillaged
	CvBuildInfo& thisBuildInfo = *GC.getBuildInfo(eBuild);
	if(thisBuildInfo.isRepair())
	{
#if defined(MOD_NO_REPAIR_FOREIGN_LANDS)
		if(MOD_NO_REPAIR_FOREIGN_LANDS)
		{
			//Can't repair outside of owned territory.
			if(ePlayer != NO_PLAYER && getOwner() != NO_PLAYER && getOwner() != ePlayer)
			{
				return false;
			}
		}
#endif
		if(IsImprovementPillaged() || IsRoutePillaged())
		{
			bValid = true;
		}
		else
		{
			return false;
		}
	}
	if(thisBuildInfo.IsRemoveRoute())
	{
		if(!getPlotCity() && getRouteType() != NO_ROUTE)
		{
			if(getOwner() == ePlayer)
			{
				bValid = true;
			}
			else if(getOwner() == NO_PLAYER && GetPlayerResponsibleForRoute() == ePlayer)
			{
				bValid = true;
			}
#if defined(MOD_BUGFIX_REMOVE_GHOST_ROUTES)
			else if(MOD_BUGFIX_REMOVE_GHOST_ROUTES && getOwner() == NO_PLAYER && (GetPlayerResponsibleForRoute() == NO_PLAYER || !GET_PLAYER(GetPlayerResponsibleForRoute()).isAlive()))
			{
				bValid = true;
			}
#endif
			else
			{
				return false;
			}
		}
#if defined(MOD_BALANCE_CORE)
		else if(isCity())
#else
		else
#endif
		{
			return false;
		}
#if defined(MOD_BALANCE_CORE)
		else if(getRouteType() != NO_ROUTE)
		{
			if(getOwner() == ePlayer)
			{
				bValid = true;
			}
			else if(getOwner() == NO_PLAYER && GetPlayerResponsibleForRoute() == ePlayer)
			{
				bValid = true;
			}
#if defined(MOD_BUGFIX_REMOVE_GHOST_ROUTES)
			else if(MOD_BUGFIX_REMOVE_GHOST_ROUTES && getOwner() == NO_PLAYER && (GetPlayerResponsibleForRoute() == NO_PLAYER || !GET_PLAYER(GetPlayerResponsibleForRoute()).isAlive()))
			{
				bValid = true;
			}
#endif
		}
#endif
	}

	eImprovement = ((ImprovementTypes)(thisBuildInfo.getImprovement()));

	// Improvement
	if(eImprovement != NO_IMPROVEMENT)
	{
		// Player must be able to build this Improvement
		if(!canHaveImprovement(eImprovement, ePlayer, bTestVisible))
		{
			return false;
		}

		// Already an improvement here
		if(getImprovementType() != NO_IMPROVEMENT)
		{

			if(GC.getImprovementInfo(getImprovementType())->IsPermanent())
			{
				return false;
			}

			if(getImprovementType() == eImprovement)
			{
				return false;
			}

			ImprovementTypes eFeitoria = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FEITORIA");
			if (eFeitoria != NO_IMPROVEMENT && getImprovementType() == eFeitoria && getOwner() != NO_PLAYER && GET_PLAYER(getOwner()).isMinorCiv())
				return false;

			eFinalImprovementType = finalImprovementUpgrade(getImprovementType());

			if(eFinalImprovementType != NO_IMPROVEMENT)
			{
				if(eFinalImprovementType == finalImprovementUpgrade(eImprovement))
				{
					if(!IsImprovementPillaged())
					{
						return false;
					}
				}
			}
		}

		// Requirements on adjacent plots?
		if (!bTestVisible)
		{
			CvImprovementEntry *pkImprovement = GC.getImprovementInfo(eImprovement);
			bool bHasLuxuryRequirement = pkImprovement->IsAdjacentLuxury();
			bool bHasNoAdjacencyRequirement = pkImprovement->IsNoTwoAdjacent();
			if (pkImprovement && (bHasLuxuryRequirement || bHasNoAdjacencyRequirement))
			{
				bool bLuxuryRequirementMet = !bHasLuxuryRequirement;
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					CvPlot *pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
					if (pAdjacentPlot != NULL)
					{
						if (bHasLuxuryRequirement)
						{
							ResourceTypes eResource = pAdjacentPlot->getResourceType();
							if (eResource != NO_RESOURCE)
							{
								CvResourceInfo *pkResourceInfo = GC.getResourceInfo(eResource);
								if (pkResourceInfo && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
								{
									bLuxuryRequirementMet = true;
								}
							}
						}
						if (bHasNoAdjacencyRequirement)
						{
							ImprovementTypes eAdjacentImprovement =  pAdjacentPlot->getImprovementType();
							if (eAdjacentImprovement != NO_IMPROVEMENT && eAdjacentImprovement == eImprovement)
							{
								return false;
							}
							CvImprovementEntry *pkImprovement2 = GC.getImprovementInfo(eAdjacentImprovement);
							if (pkImprovement2 && eAdjacentImprovement != NO_IMPROVEMENT && pkImprovement2->GetImprovementMakesValid(eImprovement))
							{
								return false;
							}
							int iBuildProgress = pAdjacentPlot->getBuildProgress(eBuild);
							if (iBuildProgress > 0)
							{
								return false;
							}
						}
					}
				}
				if (bHasLuxuryRequirement && !bLuxuryRequirementMet)
				{
					return false;
				}
			}
		}

		if(!bTestVisible)
		{
			if(!GC.getImprovementInfo(eImprovement)->IsIgnoreOwnership())
			{
				// Gifts for minors can ignore borders requirements
				if(bTestPlotOwner)
				{
					// Outside Borders - Can be built in or outside our lands, but not in other lands
					if(GC.getImprovementInfo(eImprovement)->IsOutsideBorders())
					{
						if (getTeam() != eTeam && getTeam() != NO_TEAM)
						{
							return false;
						}
					}
					// In Adjacent Friendly - Can be built in or adjacent to our lands
					else if (GC.getImprovementInfo(eImprovement)->IsInAdjacentFriendly())
					{
#if defined(MOD_BALANCE_CORE_MILITARY)
						//citadels only in adjacent _unowned_ territory
						if (getTeam() == NO_TEAM)
						{
							if (!isAdjacentTeam(eTeam, false))
								return false;
						}
						else if (getTeam() != eTeam)
							return false;
#else
						if (getTeam() != eTeam && !isAdjacentTeam(eTeam, false))
						{
							return false;
						}
#endif
					}
					// Only City State Territory - Can only be built in City-State territory (not our own lands)
					else if (GC.getImprovementInfo(eImprovement)->IsOnlyCityStateTerritory())
					{
						bool bCityStateTerritory = false;
						if (isOwned())
						{
							if (GET_PLAYER(getOwner()).isMinorCiv())
							{
								bCityStateTerritory = true;
#if defined(MOD_DIPLOMACY_CITYSTATES)
								//Let's check for Embassies.
								if(GC.getImprovementInfo(eImprovement)->IsEmbassy())
								{
									int iCityLoop;
									for (CvCity* pLoopCity = GET_PLAYER(getOwner()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iCityLoop))
									{
										if(pLoopCity != NULL)
										{
											for(int iI = 0; iI < pLoopCity->GetNumWorkablePlots(); iI++)
											{
												CvPlot* pCityPlot = pLoopCity->GetCityCitizens()->GetCityPlotFromIndex(iI);

												if(pCityPlot != NULL && pCityPlot->getOwner() == pLoopCity->getOwner())
												{
													if (pCityPlot->IsImprovementEmbassy())
														return false;
												}
											}
										}
									}
								}
#endif
							}
						}

						if (!bCityStateTerritory)
						{
							return false;
						}
					}
					else if(getTeam() != eTeam) 
					{//only buildable in own culture
						return false;
					}
				}
			}
		}

		bValid = true;
	}

	eRoute = ((RouteTypes)(GC.getBuildInfo(eBuild)->getRoute()));

	// Route
	if(eRoute != NO_ROUTE)
	{
		if(getRouteType() != NO_ROUTE)
		{
			if (isWater() && !thisBuildInfo.IsWater())
			{
				return false;
			}

			CvRouteInfo* pkPlotRoute = GC.getRouteInfo(getRouteType());
			CvRouteInfo* pkBuildRoute = GC.getRouteInfo(eRoute);
			if(pkPlotRoute && pkBuildRoute)
			{
				// Can't build an older, less useful Route over the top of an existing one
				if(pkPlotRoute->getValue() >= pkBuildRoute->getValue())
				{
					if(!IsRoutePillaged())
					{
						return false;
					}
				}
			}
		}

		//can't build roads in enemy territory and saddle them with the maintenance
		if (getOwner() != NO_PLAYER && ePlayer != NO_PLAYER)
		{
			CvPlayer& kOwner = GET_PLAYER(getOwner());
			if (kOwner.isMajorCiv() && kOwner.getTeam() != GET_PLAYER(ePlayer).getTeam())
			{
				if (!GET_TEAM(kOwner.getTeam()).IsVassal(GET_PLAYER(ePlayer).getTeam()))
					return false;
			}
		}

		bValid = true;
	}

	// In general, cannot clear features in the lands of players that aren't on our team or that we're at war with
	if(getFeatureType() != NO_FEATURE)
	{
		if(GC.getBuildInfo(eBuild)->isFeatureRemove(getFeatureType()))
		{
#if defined(MOD_BALANCE_CORE)
			if(getFeatureType() == FEATURE_FALLOUT && GC.getBuildInfo(eBuild)->isFeatureRemove(FEATURE_FALLOUT))
			{
				bValid = true;
			}
			else
#endif
			if(bTestPlotOwner)
			{
				if(isOwned() && (eTeam != getTeam()) && !atWar(eTeam, getTeam()))
				{
					if (eImprovement == NO_IMPROVEMENT)
					{
						return false;
					}

					// Some improvements are exceptions
					if (!GC.getImprovementInfo(eImprovement)->IsIgnoreOwnership() &&
						!GC.getImprovementInfo(eImprovement)->IsOnlyCityStateTerritory())
					{
						return false;
					}
				}
			}

			bValid = true;
		}
	}

	return bValid;
}


//	--------------------------------------------------------------------------------
int CvPlot::getBuildTime(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	int iTime;

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iTime = GC.getBuildInfo(eBuild)->getTime();
	if (ePlayer != NO_PLAYER)
	{
		TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
		if (eTeam != NO_TEAM)
		{
			iTime += GET_TEAM(eTeam).getBuildTimeChange(eBuild);
		}
	}
	// Repair is either 3 turns or the original build time, whichever is shorter
	if(GC.getBuildInfo(eBuild)->isRepair())
	{
		RouteTypes eRoute = getRouteType();

		if(eRoute != NO_ROUTE)
		{
			for(int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iBuildLoop);
				if(!pkBuildInfo)
				{
					continue;
				}

				if(pkBuildInfo->getRoute() == eRoute)
				{
					if(pkBuildInfo->getTime() < iTime)
					{
						iTime = pkBuildInfo->getTime();
					}
				}
			}
		}
	}
	// End Repair time mod

	if(getFeatureType() != NO_FEATURE)
	{
		iTime += GC.getBuildInfo(eBuild)->getFeatureTime(getFeatureType());
	}

	iTime *= std::max(0, (GC.getTerrainInfo(getTerrainType())->getBuildModifier() + 100));
	iTime /= 100;

	iTime *= GC.getGame().getGameSpeedInfo().getBuildPercent();
	iTime /= 100;

	iTime *= GC.getGame().getStartEraInfo().getBuildPercent();
	iTime /= 100;
#if defined(MOD_CIV6_WORKER)
	if (MOD_CIV6_WORKER)
	{
		iTime = 0;
	}
#endif
	return iTime;
}


//	--------------------------------------------------------------------------------
int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer, int iNowExtra, int iThenExtra) const
{
	int iBuildLeft = getBuildTime(eBuild, ePlayer);

	if(iBuildLeft == 0)
		return 0;

	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	int iNowBuildRate;
	int iThenBuildRate;
	int iTurnsLeft;

	iNowBuildRate = iNowExtra;
	iThenBuildRate = iThenExtra;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getBuildType() == eBuild)
		{
			if(pLoopUnit->canMove())
			{
				iNowBuildRate += pLoopUnit->workRate(false);
			}
			iThenBuildRate += pLoopUnit->workRate(true);
		}
	}

	if(iThenBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return INT_MAX;
	}

	iBuildLeft -= getBuildProgress(eBuild);
	iBuildLeft -= iNowBuildRate;

	iBuildLeft = std::max(0, iBuildLeft);

	iTurnsLeft = (iBuildLeft / iThenBuildRate);

	if((iTurnsLeft * iThenBuildRate) < iBuildLeft)
	{
		iTurnsLeft++;
	}

	//iTurnsLeft++;

	//return std::max(1, iTurnsLeft);
	return iTurnsLeft;
}


//	--------------------------------------------------------------------------------
int CvPlot::getBuildTurnsTotal(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	int iNowBuildRate = 0;
	int iThenBuildRate = 0;
	int iBuildLeft = 0;
	int iTurnsLeft = 0;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getBuildType() == eBuild)
		{
			if(pLoopUnit->canMove())
			{
				iNowBuildRate += pLoopUnit->workRate(false);
			}
			iThenBuildRate += pLoopUnit->workRate(true);
		}
	}

	if(iThenBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return INT_MAX;
	}

	iBuildLeft = getBuildTime(eBuild, ePlayer);

	iBuildLeft = std::max(0, iBuildLeft);

	iTurnsLeft = (iBuildLeft / iThenBuildRate);

	iTurnsLeft--;

	return std::max(1, iTurnsLeft);
}


//	--------------------------------------------------------------------------------
int CvPlot::getFeatureProduction(BuildTypes eBuild, PlayerTypes ePlayer, CvCity** ppCity) const
{
	int iProduction;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if(getFeatureType() == NO_FEATURE)
	{
		return 0;
	}

	*ppCity = getOwningCity();

	if(*ppCity == NULL)
	{
		*ppCity = GC.getMap().findCity(getX(), getY(), NO_PLAYER, eTeam, false);
	}

	if(*ppCity == NULL)
	{
		return 0;
	}

	// Base value
	if(GET_PLAYER(ePlayer).GetAllFeatureProduction() > 0)
	{
		iProduction = GET_PLAYER(ePlayer).GetAllFeatureProduction();
	}
	else
	{
		iProduction = GC.getBuildInfo(eBuild)->getFeatureProduction(getFeatureType());
	}

	if (MOD_BALANCE_CORE_SETTLER_ADVANCED)
	{
		iProduction *= std::max(0, (GET_PLAYER(ePlayer).getFeatureProductionModifier()));
		iProduction /= 100;
	}
	else
	{
		iProduction *= std::max(0, (GET_PLAYER(ePlayer).getFeatureProductionModifier() + 100));
		iProduction /= 100;
	}

	iProduction *= GC.getGame().getGameSpeedInfo().getFeatureProductionPercent();
	iProduction /= 100;

	if(getTeam() != eTeam)
	{
		iProduction *= GC.getDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT();
		iProduction /= 100;
	}

	return std::max(0, iProduction);
}


//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getBestGarrison(PlayerTypes eOwner) const
{
	const IDInfo* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit = NULL;
	CvUnit* pBestUnit = NULL;

	//we don't consider promotions here ...
	int iBestBaseCS = 0;
	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && (pLoopUnit->getOwner() == eOwner) && pLoopUnit->CanGarrison() && !pLoopUnit->isDelayedDeath())
		{
			int iBaseCS = max(pLoopUnit->GetBaseCombatStrength(),pLoopUnit->GetBaseRangedCombatStrength());
			//naval units considered weaker
			if (!pLoopUnit->isNativeDomain(this))
				iBaseCS /= 2;

			if(iBaseCS>iBestBaseCS)
			{
				pBestUnit = pLoopUnit;
				iBestBaseCS = iBaseCS;
			}
		}
	}

	return pBestUnit;
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bIgnoreVisibility, bool bTestCanMove, bool bNoncombatAllowed) const
{
	const IDInfo* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit = NULL;
	CvUnit* pBestUnit = NULL;

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && (bNoncombatAllowed || pLoopUnit->IsCanDefend()) && pLoopUnit != pAttacker)	// Does the unit exist, and can it fight, or do we care if it can't fight?
		{
			if((eOwner ==  NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
			{
				if((eAttackingPlayer == NO_PLAYER) || bIgnoreVisibility || (!(pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false)) && isVisible(GET_PLAYER(eAttackingPlayer).getTeam())))
				{
					if(!bTestAtWar || eAttackingPlayer == NO_PLAYER || pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this)))
					{
						if(!bTestCanMove || (pLoopUnit->canMove() && !(pLoopUnit->isCargo())))
						{
							if((pAttacker == NULL) || (pAttacker->getDomainType() != DOMAIN_AIR) || (pLoopUnit->getDamage() < pAttacker->GetRangedCombatLimit()))
							{
								if(pLoopUnit->isBetterDefenderThan(pBestUnit, pAttacker))
								{
									pBestUnit = pLoopUnit;
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

//	--------------------------------------------------------------------------------
bool CvPlot::HasAirCover(PlayerTypes eDefendingPlayer) const
{
	if (eDefendingPlayer == NO_PLAYER)
		return false;

	CvPlayerAI& kPlayer = GET_PLAYER(eDefendingPlayer);
	const std::vector<std::pair<int, int>>& possibleUnits = kPlayer.GetPossibleInterceptors();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		CvPlot* pInterceptorPlot = GC.getMap().plotByIndexUnchecked(it->second);
		CvUnit* pInterceptorUnit = kPlayer.getUnit(it->first);

		if (!pInterceptorUnit || pInterceptorUnit->isDelayedDeath())
			continue;

		// Must not have already intercepted this turn
		if (pInterceptorUnit->isOutOfInterceptions())
			continue;

		// Test range
		int iDistance = plotDistance(*pInterceptorPlot, *this);
		if (iDistance <= pInterceptorUnit->GetAirInterceptRange())
			return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlot::GetInterceptorCount(PlayerTypes eAttackingPlayer, CvUnit* pAttackingUnit /* = NULL */, bool bLandInterceptorsOnly /*false*/, bool bVisibleInterceptorsOnly /*false*/) const
{
	int iCount = 0;
	GetBestInterceptor(eAttackingPlayer, pAttackingUnit,bLandInterceptorsOnly,bVisibleInterceptorsOnly,&iCount);
	return iCount;
}


//	--------------------------------------------------------------------------------
CvUnit* CvPlot::GetBestInterceptor(PlayerTypes eAttackingPlayer, const CvUnit* pAttackingUnit /* = NULL */, 
	bool bLandInterceptorsOnly /*false*/, bool bVisibleInterceptorsOnly /*false*/, int* piNumPossibleInterceptors) const
{
	if (eAttackingPlayer == NO_PLAYER)
		return NULL;

	VALIDATE_OBJECT
	CvUnit* pBestUnit = 0;
	int iBestValue = 0;
	int iBestDistance = INT_MAX;

	// Loop through all players' Units (that we're at war with) to see if they can intercept
	// Note that the barbarians are not included here, to they can never intercept
	const std::vector<PlayerTypes>& vEnemies = GET_PLAYER(eAttackingPlayer).GetPlayersAtWarWith();

	for(size_t iI = 0; iI < vEnemies.size(); iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER(vEnemies[iI]);
		TeamTypes eLoopTeam = kLoopPlayer.getTeam();

		//stealth unit? no intercept
		if(pAttackingUnit && pAttackingUnit->isInvisible(eLoopTeam, false, false))
			continue;

		const std::vector<std::pair<int, int>>& possibleUnits = kLoopPlayer.GetPossibleInterceptors();
		for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
		{
			CvPlot* pInterceptorPlot = GC.getMap().plotByIndexUnchecked(it->second);
			CvUnit* pInterceptorUnit = kLoopPlayer.getUnit(it->first);

			if (!pInterceptorUnit || pInterceptorUnit->isDelayedDeath())
				continue;

			// Must not have already intercepted this turn
			if (pInterceptorUnit->isOutOfInterceptions())
				continue;

			// Check input booleans
			if (bLandInterceptorsOnly && pInterceptorUnit->getDomainType() != DOMAIN_LAND)
				continue;
			if (bVisibleInterceptorsOnly && !pInterceptorPlot->isVisible(getTeam()))
				continue;

			// Test range
			int iDistance = plotDistance(*pInterceptorPlot, *this);
			if (iDistance > pInterceptorUnit->GetAirInterceptRange())
				continue;
			
			//do not violate neutral players' airspace
			if (isOwned() && !kLoopPlayer.IsAtWarWith(getOwner()) && !IsFriendlyTerritory(kLoopPlayer.GetID()))
				continue;

			// we're fine with truncation here; take promotions boosting intercept strength into account
			int attackStrength = (pInterceptorUnit->GetBestAttackStrength() * (100 + pInterceptorUnit->GetInterceptionCombatModifier())) / 100;
			
			// interceptionProbability contains product of actual intercept chance and health percentage; lets be careful with air units at low health in case of air sweeps
			int healthFactor = pInterceptorUnit->interceptionProbability();
			if (pInterceptorUnit->getDomainType() == DOMAIN_AIR)
				healthFactor = (healthFactor * (pInterceptorUnit->GetCurrHitPoints() * 100) / pInterceptorUnit->GetMaxHitPoints()) / 100;
			
			int iValue = attackStrength * healthFactor;

			if (iValue>0 && piNumPossibleInterceptors)
				(*piNumPossibleInterceptors)++;

			if( iValue>iBestValue || (iValue==iBestValue && iDistance<iBestDistance) )
			{
				iBestDistance = iDistance;
				iBestValue = iValue;
				pBestUnit = pInterceptorUnit;
			}
		}
	}

	return pBestUnit;
}

CvCity* CvPlot::GetNukeInterceptor(PlayerTypes eAttackingPlayer) const
{
	if (eAttackingPlayer == NO_PLAYER)
		return NULL;

	CvCity* pCity = getOwningCity();
	if (pCity == NULL)
		return NULL;

	CvPlayerAI& kPlayer = GET_PLAYER(pCity->getOwner());
	if (kPlayer.isMinorCiv() || kPlayer.isBarbarian())
		return NULL;

	if (pCity->getNukeInterceptionChance() <= 0)
		return NULL;

	return pCity;
}


//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getSelectedUnit() const
{
	const IDInfo* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->IsSelected())
		{
			return pLoopUnit;
		}
	}

	return NULL;
}


//	--------------------------------------------------------------------------------
int CvPlot::getUnitPower(PlayerTypes eOwner) const
{
	const IDInfo* pUnitNode = headUnitNode();
	const CvUnit* pLoopUnit;
	int iCount = 0;

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(!pLoopUnit) continue;

		if((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{
				if(GC.getFlavorTypes((FlavorTypes)iI) == "FLAVOR_OFFENSE" ||
				        GC.getFlavorTypes((FlavorTypes)iI) == "FLAVOR_DEFENSE")
				{
					iCount += pLoopUnit->getUnitInfo().GetFlavorValue(iI);
				}
			}
		}
	}

	return iCount;
}

int CvPlot::GetEffectiveFlankingBonus(const CvUnit* pUnit, const CvUnit* pOtherUnit, const CvPlot* pOtherUnitPlot) const
{
	if (!pUnit || !pOtherUnit)
		return 0;

	const CvPlot* pOtherPlot = pOtherUnitPlot ? pOtherUnitPlot : pOtherUnit->plot();

	//our units are the enemy's enemies ...
	int iNumUnitsAdjacentToOther = pOtherPlot->GetNumEnemyUnitsAdjacent( pOtherUnit->getTeam(), pOtherUnit->getDomainType(), pUnit);
	int iNumUnitsAdjacentToHere = GetNumEnemyUnitsAdjacent( pUnit->getTeam(), pUnit->getDomainType(), pOtherUnit);

	if(iNumUnitsAdjacentToOther > iNumUnitsAdjacentToHere)
		return /*5*/ (GC.getBONUS_PER_ADJACENT_FRIEND() + pUnit->GetFlankAttackModifier()) * (iNumUnitsAdjacentToOther - iNumUnitsAdjacentToHere);

	return 0;
}


bool CvPlot::isRevealedFortification(TeamTypes eTeam) const
{
	static const ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
	static const ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");

	if ((eFort != NO_IMPROVEMENT && getRevealedImprovementType(eTeam) == eFort) ||
		(eCitadel != NO_IMPROVEMENT && getRevealedImprovementType(eTeam) == eCitadel))
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlot::defenseModifier(TeamTypes eDefender, bool bIgnoreImprovement, bool bIgnoreFeature, bool bForHelp) const
{
	// Cities also give a boost - damage is split between city and unit - assume a flat 100% defense bonus for simplicity
	if (isCity())
		return 100;

	int iModifier = 0;

	// Plot type
	if(isHills() || isMountain())
		iModifier += /*25*/ GC.getHILLS_EXTRA_DEFENSE();

	// Feature
	if(!bIgnoreFeature && getFeatureType() != NO_FEATURE)
		iModifier += GC.getFeatureInfo(getFeatureType())->getDefenseModifier();

	// Terrain
	if(getTerrainType() != NO_TERRAIN)
		iModifier += GC.getTerrainInfo(getTerrainType())->getDefenseModifier();

	if (!bIgnoreImprovement)
	{
		// Improvements count extra, but include them for tooltips only if the tile is revealed
		ImprovementTypes eImprovement = bForHelp ? getRevealedImprovementType(GC.getGame().getActiveTeam()) : getImprovementType();
		if(eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
		{
			//only friendly or unowned fortresses can be used for combat, but include them in the tooltips always
			if(bForHelp || (eDefender != NO_TEAM && (getTeam() == NO_TEAM || getTeam() == eDefender)))
			{
				CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eImprovement);
				if (pkImprovement)
					iModifier += pkImprovement->GetDefenseModifier();
			}
		}
	}

	return iModifier;
}

//	---------------------------------------------------------------------------
int CvPlot::movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining) const
{
	int iMaxMoves = pUnit->baseMoves( needsEmbarkation(pUnit) )*GC.getMOVE_DENOMINATOR();

	if (plotDistance(*this,*pFromPlot)>1)
		return iMaxMoves;

	return CvUnitMovement::MovementCost(pUnit, pFromPlot, this, iMovesRemaining, iMaxMoves);
}

//	---------------------------------------------------------------------------
int CvPlot::MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining) const
{
	int iMaxMoves = pUnit->baseMoves( needsEmbarkation(pUnit) )*GC.getMOVE_DENOMINATOR();

	if (plotDistance(*this,*pFromPlot)>1)
		return iMaxMoves;

	return CvUnitMovement::MovementCostNoZOC(pUnit, pFromPlot, this, iMovesRemaining, iMaxMoves);
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsAllowsWalkWater() const
{
	ImprovementTypes eImprovement = getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry *pkEntry = GC.getImprovementInfo(eImprovement);
		if (pkEntry)
			return pkEntry->IsAllowsWalkWater();
	}
	return false;
}

bool CvPlot::needsEmbarkation(const CvUnit* pUnit) const
{
	//embarkation only on water plots
	if (!isWater() || isIce() || IsAllowsWalkWater())
		return false;

    if (!pUnit)
        return true;

    //only land units need to embark
    if (pUnit->getDomainType()!=DOMAIN_LAND || pUnit->canMoveAllTerrain())
        return false;

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
    if (pUnit->IsEmbarkDeepWater())
        return isDeepWater();
#endif

	//some units can flip between different types
	if (pUnit->isConvertUnit())
		return false;

	//we know it's a land unit and a water plot by now
	return true;
}

//	--------------------------------------------------------------------------------
int CvPlot::getExtraMovePathCost() const
{
	return GC.getGame().getPlotExtraCost(getX(), getY());
}


//	--------------------------------------------------------------------------------
void CvPlot::changeExtraMovePathCost(int iChange)
{
	GC.getGame().changePlotExtraCost(getX(), getY(), iChange);
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentOwned() const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getOwner() != NO_PLAYER)
			{
				return true;
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getOwner() == ePlayer)
			{
				if(!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsAdjacentOwnedByTeamOtherThan(TeamTypes eTeam, bool bAllowNoTeam) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL && pAdjacentPlot->getTeam() != eTeam)
		{
			if (bAllowNoTeam || pAdjacentPlot->getTeam() != NO_TEAM)
				return true; 
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::IsAdjacentOwnedByEnemy(TeamTypes eTeam) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getTeam() != NO_TEAM && GET_TEAM(eTeam).isAtWar(pAdjacentPlot->getTeam()))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentTeam(TeamTypes eTeam, bool bLandOnly) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getTeam() == eTeam)
			{
				if(!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvPlot::IsAdjacentCity(TeamTypes eTeam) const
{
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot && pAdjacentPlot->isCity())
		{
			if (eTeam==NO_TEAM || pAdjacentPlot->getTeam()==eTeam)
				return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
CvCity* CvPlot::GetAdjacentFriendlyCity(TeamTypes eTeam, bool bLandOnly) const
{
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getTeam() == eTeam && pAdjacentPlot->isCity())
			{
				if(!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					CvCity* pCity = pAdjacentPlot->getPlotCity();
					if(pCity)
					{
						if(pCity->getTeam() == eTeam)
						{
							return pCity;
						}
					}
				}
			}
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
CvCity* CvPlot::GetAdjacentCity() const
{
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isCity())
			{
				CvCity* pCity = pAdjacentPlot->getPlotCity();
				if(pCity)
				{
					return pCity;
				}
			}
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Number of adjacent tiles owned by another team (or unowned)
int CvPlot::GetNumAdjacentDifferentTeam(TeamTypes eTeam, DomainTypes eDomain, bool bCountUnowned) const
{
	int iRtnValue = 0;

	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(GetPlotIndex());
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = aNeighbors[iI];
		if(pAdjacentPlot != NULL)
		{
			if(eDomain!=NO_DOMAIN && pAdjacentPlot->getDomain()!=eDomain)
			{
				continue;
			}

			if(pAdjacentPlot->getTeam() != eTeam && (bCountUnowned || pAdjacentPlot->isOwned()))
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

int CvPlot::GetNumAdjacentMountains() const
{
	int iNumMountains = 0;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isMountain())
			{
				iNumMountains++;
			}
		}
	}
	return iNumMountains;
}

int CvPlot::GetSeaBlockadeScore(PlayerTypes ePlayer) const
{
	int iScore = 0;
	int iRange = min(5,max(0,GC.getNAVAL_PLOT_BLOCKADE_RANGE()));

	for(int iI = 0; iI < RING_PLOTS[iRange]; iI++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(getX(), getY(), iI);
		if(pLoopPlot == NULL || pLoopPlot->getDomain() != DOMAIN_SEA || pLoopPlot->getArea() != getArea())
			continue;

		if (GET_PLAYER(ePlayer).IsAtWarWith(pLoopPlot->getOwner()))
		{
			//there should really be a function that gives you a weighted score of all yields ...
			iScore++;
			if (pLoopPlot->getResourceType() != NO_RESOURCE)
				iScore++;
			if (pLoopPlot->getFeatureType() != NO_FEATURE)
				iScore++;
			if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				iScore++;
		}

		iScore++;
	}

	return iScore;
}

int CvPlot::countPassableNeighbors(DomainTypes eDomain, CvPlot** aPassableNeighbors) const
{
	int iPassable = 0;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			if ( (eDomain==NO_DOMAIN || eDomain==pAdjacentPlot->getDomain()) && !pAdjacentPlot->isImpassable(BARBARIAN_TEAM))
			{
				if (aPassableNeighbors)
					aPassableNeighbors[iPassable] = pAdjacentPlot;
				iPassable++;
			}
		}
	}
	return iPassable;
}

bool CvPlot::IsBorderLand(PlayerTypes eDefendingPlayer) const
{
	//check distance to all major players
	//if homefront for at least one ...
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer == eDefendingPlayer)
			continue;

		if (!GET_PLAYER(eLoopPlayer).isAlive())
			continue;

		//we trust our friends
		if (GET_PLAYER(eDefendingPlayer).isMajorCiv() && !GET_PLAYER(eDefendingPlayer).GetDiplomacyAI()->IsPotentialMilitaryTargetOrThreat(eLoopPlayer))
		{
			continue;
		}
		else if (GET_PLAYER(eDefendingPlayer).isMinorCiv() && (GET_PLAYER(eDefendingPlayer).GetMinorCivAI()->IsFriends(eLoopPlayer) || (GET_PLAYER(eDefendingPlayer).GetMinorCivAI()->GetAlly() == eLoopPlayer)))
		{
			continue;
		}

		if (IsCloseToBorder(eLoopPlayer))
			return true;
	}

	return false;
}

bool CvPlot::IsChokePoint() const
{
	//only passable hill plots can be chokepoints
	if(!isHills() || isImpassable(BARBARIAN_TEAM))
		return false;

	CvPlot* aPassableNeighbors[NUM_DIRECTION_TYPES];
	int iPassable = countPassableNeighbors(DOMAIN_LAND, aPassableNeighbors);

	//a plot is a chokepoint if it has between two and four passable land plots as neighbors
	if (iPassable<2 || iPassable>4)
		return false;

	//each adjacent passable plot must have at least 3 passable neighbors (anti peninsula/dead end valley check)
	int iPassableAndNoPeninsula = 0;
	CvPlot* aPassableNeighborsNonPeninsula[NUM_DIRECTION_TYPES];
	for (int iI = 0; iI<iPassable; iI++)
	{
		if (aPassableNeighbors[iI]->countPassableNeighbors(DOMAIN_SEA,NULL)>2)
		{
			aPassableNeighborsNonPeninsula[iPassableAndNoPeninsula] = aPassableNeighbors[iI];
			iPassableAndNoPeninsula++;
		}
	}

	if (iPassableAndNoPeninsula<2)
		return false;
	else if (iPassableAndNoPeninsula==2)
	{
		//check they are not adjacent
		return !( aPassableNeighborsNonPeninsula[0]->isAdjacent(aPassableNeighborsNonPeninsula[1]) );
	}
	else if (iPassableAndNoPeninsula==3)
	{
		//three passable plots. not more than one pair may be adjacent
		int AB = int(aPassableNeighborsNonPeninsula[0]->isAdjacent(aPassableNeighborsNonPeninsula[1]));
		int AC = int(aPassableNeighborsNonPeninsula[0]->isAdjacent(aPassableNeighborsNonPeninsula[2]));
		int BC = int(aPassableNeighborsNonPeninsula[1]->isAdjacent(aPassableNeighborsNonPeninsula[2]));

		return (AB+AC+BC)<2;
	}
	else if (iPassableAndNoPeninsula==4)
	{
		//four passable plots. not more than two pairs may be adjacent
		int AB = int(aPassableNeighborsNonPeninsula[0]->isAdjacent(aPassableNeighborsNonPeninsula[1]));
		int AC = int(aPassableNeighborsNonPeninsula[0]->isAdjacent(aPassableNeighborsNonPeninsula[2]));
		int AD = int(aPassableNeighborsNonPeninsula[0]->isAdjacent(aPassableNeighborsNonPeninsula[3]));
		int BC = int(aPassableNeighborsNonPeninsula[1]->isAdjacent(aPassableNeighborsNonPeninsula[2]));
		int BD = int(aPassableNeighborsNonPeninsula[1]->isAdjacent(aPassableNeighborsNonPeninsula[3]));
		int CD = int(aPassableNeighborsNonPeninsula[2]->isAdjacent(aPassableNeighborsNonPeninsula[3]));

		return (AB+AC+AD+BC+BD+CD)<3;
	}

	return false;
}

bool CvPlot::IsLandbridge(int iMinDistanceSaved, int iMinOceanSize) const
{
	//only passable land plots can be chokepoints
	if(isWater() || isImpassable(BARBARIAN_TEAM))
		return false;

	SPathFinderUserData data(NO_PLAYER, PT_GENERIC_SAME_AREA, NO_PLAYER);

	const CvPlot *pFirstPlot = 0, *pSecondPlot = 0;
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		const CvPlot* pLoopPlot = aPlotsToCheck[iCount];

		if(pLoopPlot==NULL || !pLoopPlot->isWater())
			continue;

		pFirstPlot = pSecondPlot;
		pSecondPlot = pLoopPlot;

		if (pFirstPlot && pSecondPlot)
		{
			//don't use the pathfinder for direct neighbors
			if(plotDistance(pFirstPlot->getX(), pFirstPlot->getY(), pSecondPlot->getX(), pSecondPlot->getY())<2)
				continue;

			//how useful is the shortcut we could generate
			SPath path = GC.GetStepFinder().GetPath(pFirstPlot->getX(), pFirstPlot->getY(), pSecondPlot->getX(), pSecondPlot->getY(), data);
			if (!path)
			{
				//no path found, perhaps it's two different oceans?
				if (pFirstPlot->getArea()!=pSecondPlot->getArea() && pFirstPlot->area()->getNumTiles()>iMinOceanSize && pSecondPlot->area()->getNumTiles()>iMinOceanSize)
					return true;
			}
			else
			{
				if (path.vPlots.size()>=(size_t)iMinDistanceSaved)
					return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvPlot::plotAction(PlotUnitFunc func, int iData1, int iData2, PlayerTypes eOwner, TeamTypes eTeam)
{
	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit)
		{
			if((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
			{
				if((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
				{
					func(pLoopUnit, iData1, iData2);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlot::plotCount(ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit)
		{
			if((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
			{
				if((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
				{
					if((funcA == NULL) || funcA(pLoopUnit, iData1A, iData2A))
					{
						if((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B))
						{
							iCount++;
						}
					}
				}
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	const IDInfo* pUnitNode = headUnitNode();
	CvUnit* pLoopUnit;

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit)
		{
			if((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
			{
				if((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
				{
					if(funcA(pLoopUnit, iData1A, iData2A))
					{
						if((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B))
						{
							return pLoopUnit;
						}
					}
				}
			}
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isOwned() const
{
	return (getOwner() != NO_PLAYER);
}


//	--------------------------------------------------------------------------------
bool CvPlot::isBarbarian() const
{
	return (getOwner() == BARBARIAN_PLAYER);
}


//	--------------------------------------------------------------------------------
bool CvPlot::isRevealedBarbarian() const
{
	return (getRevealedOwner(GC.getGame().getActiveTeam(), true) == BARBARIAN_PLAYER);
}

//	--------------------------------------------------------------------------------
bool CvPlot::HasBarbarianCamp()
{
	return (getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT());
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//	--------------------------------------------------------------------------------
bool CvPlot::HasDig()
{
	return (getResourceType() == GC.getARTIFACT_RESOURCE());
}
#endif

bool CvPlot::isVisible(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return true;
	else
		return isVisible(eTeam);
}

bool CvPlot::isVisible(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
		return false;

	/*
	//experimental hack: observer visibility is determined by the player they will return as
	if (GET_TEAM(eTeam).isObserver() && GC.getGame().GetAutoPlayReturnPlayer() != NO_PLAYER)
		eTeam = GET_PLAYER(GC.getGame().GetAutoPlayReturnPlayer()).getTeam();
	*/

	return ((getVisibilityCount(eTeam) > 0));
}

//	--------------------------------------------------------------------------------
bool CvPlot::isActiveVisible(bool bDebug) const
{
	return isVisible(GC.getGame().getActiveTeam(), bDebug);
}

//	--------------------------------------------------------------------------------
bool CvPlot::isActiveVisible() const
{
	return isVisible(GC.getGame().getActiveTeam());
}

//	--------------------------------------------------------------------------------
#if defined(MOD_BALANCE_CORE)
bool CvPlot::isVisibleToCivTeam(bool bNoObserver, bool bNoMinor) const
#else
bool CvPlot::isVisibleToCivTeam() const
#endif
{
	int iI;

	for(iI = 0; iI < MAX_CIV_TEAMS; ++iI)
	{
#if defined(MOD_BALANCE_CORE)
		//Skip observer here.
		if(bNoObserver && GET_TEAM((TeamTypes)iI).isObserver())
		{
			continue;
		}
		if (bNoMinor && GET_TEAM((TeamTypes)iI).isMinorCiv())
		{
			continue;
		}
#endif
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			if(isVisible(((TeamTypes)iI)))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isVisibleToEnemy(PlayerTypes eFriendlyPlayer) const
{
	const std::vector<PlayerTypes>& vEnemies = GET_PLAYER(eFriendlyPlayer).GetPlayersAtWarWith();

	for (std::vector<PlayerTypes>::const_iterator it = vEnemies.begin(); it != vEnemies.end(); ++it)
	{
		CvPlayer& kEnemy = GET_PLAYER(*it);
		if (kEnemy.isAlive() && kEnemy.IsAtWarWith(eFriendlyPlayer))
		{
			if (isVisible(kEnemy.getTeam()))
			{
				return true;
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isVisibleToWatchingHuman() const
{
	for(int iI = 0; iI < MAX_CIV_PLAYERS; ++iI)
	{
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iI);
		if( (thisPlayer.isAlive() && thisPlayer.isHuman()) || ( CvPreGame::slotStatus((PlayerTypes)iI) == SS_OBSERVER && CvPreGame::slotClaim((PlayerTypes)iI) == SLOTCLAIM_ASSIGNED) )
		{
			if(isVisible(thisPlayer.getTeam()))
			{
				return true;
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentVisible(TeamTypes eTeam, bool bDebug) const
{
	CvPlot* pAdjacentPlot;
	int iI;

#if defined(MOD_BALANCE_CORE)
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = aPlotsToCheck[iI];
#else
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
#endif
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isVisible(eTeam, bDebug))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentNonvisible(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

#if defined(MOD_BALANCE_CORE)
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = aPlotsToCheck[iI];
#else
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
#endif
		if(pAdjacentPlot != NULL)
		{
			if(!pAdjacentPlot->isVisible(eTeam))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlot::countMatchingAdjacentPlots(DomainTypes eDomain, PlayerTypes eOwningPlayer, PlayerTypes eWarPlayer, PlayerTypes eInvisiblePlayer) const
{
	int iMatchingAdjacentCount = 0;
	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(this);
	for (int i = 0; i < 6; i++)
	{
		CvPlot* pNeighbor = aNeighbors[i];
		if (!pNeighbor)
			continue;

		if (eDomain != NO_DOMAIN && pNeighbor->getDomain() != eDomain)
			continue;

		if (eOwningPlayer != NO_PLAYER && pNeighbor->getOwner() != eOwningPlayer)
			continue;

		if (eWarPlayer != NO_PLAYER && !GET_PLAYER(eWarPlayer).IsAtWarWith(pNeighbor->getOwner()))
			continue;

		if (eInvisiblePlayer != NO_PLAYER && pNeighbor->isVisible(GET_PLAYER(eInvisiblePlayer).getTeam()))
			continue;

		iMatchingAdjacentCount++;
	}

	return iMatchingAdjacentCount;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isGoody(TeamTypes eTeam) const
{
	if((eTeam != NO_TEAM) && (GET_TEAM(eTeam).isBarbarian() || GET_TEAM(eTeam).isMinorCiv()))
	{
		return false;
	}

	return ((getImprovementType() == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getImprovementType())->IsGoody());
}


//	--------------------------------------------------------------------------------
bool CvPlot::isRevealedGoody(TeamTypes eTeam) const
{
	if(eTeam == NO_TEAM)
	{
		return isGoody();
	}

	if(GET_TEAM(eTeam).isBarbarian() || GET_TEAM(eTeam).isMinorCiv())
	{
		return false;
	}

	if(!isRevealed(eTeam))
	{
		return false;
	}

	return ((getRevealedImprovementType(eTeam) == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getRevealedImprovementType(eTeam))->IsGoody());
}


//	--------------------------------------------------------------------------------
void CvPlot::removeGoody()
{
	setImprovementType(NO_IMPROVEMENT);
	// Make sure the players redo their goody hut searches
	for(int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if(GET_PLAYER((PlayerTypes)i).isAlive())
			GET_PLAYER((PlayerTypes)i).GetEconomicAI()->UpdateExplorePlotsLocally(this);
	}
}

//	--------------------------------------------------------------------------------
bool CvPlot::isFriendlyCity(const CvUnit& kUnit) const
{
	if(!isCity())
	{
		return false;
	}

	TeamTypes ePlotTeam = getTeam();

	if(NO_TEAM != ePlotTeam)
	{
		TeamTypes eTeam = GET_PLAYER(kUnit.getCombatOwner(ePlotTeam, *this)).getTeam();

		if(eTeam == ePlotTeam)
		{
			return true;
		}

		if(GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eTeam))
		{
			return true;
		}
	}

	return false;
}

bool CvPlot::MeleeAttackerAdvances(TeamTypes eAttackerTeam) const
{
#if defined(MOD_GLOBAL_NO_FOLLOWUP_FROM_CITIES)
	if (MOD_GLOBAL_NO_FOLLOWUP_FROM_CITIES)
	{
		// If the attacker is in a city, fort or citadel, don't advance
		static const  ImprovementTypes eImprovementFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
		static const  ImprovementTypes eImprovementCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");
		static const  ImprovementTypes eImprovementCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

		if (isCity())
			return false;

		if (getTeam() == eAttackerTeam && !IsImprovementPillaged())
		{
			ImprovementTypes eImprovement = getImprovementType();
			if (eImprovement == eImprovementFort || eImprovement == eImprovementCitadel || eImprovement == eImprovementCamp)
				return false;
		}
	}
#endif
	return true;
}

bool CvPlot::isFriendlyCityOrPassableImprovement(PlayerTypes ePlayer, const CvUnit* pUnit) const
{
	return isCityOrPassableImprovement(ePlayer, true, pUnit);
}

bool CvPlot::isCityOrPassableImprovement(PlayerTypes ePlayer, bool bMustBeFriendly, const CvUnit* pUnit) const
{
	bool bIsCityOrPassable = isCity();
	if (MOD_GLOBAL_PASSABLE_FORTS)
		bIsCityOrPassable |= (IsImprovementPassable() && !IsImprovementPillaged());

	// Not a city or a fort
	if (!bIsCityOrPassable)
		return false;

	// that's it!
	if (!bMustBeFriendly)
		return true;

	// In friendly lands (ours, an allied CS or a major with open borders)
	if (IsFriendlyTerritory(ePlayer))
	{
#if defined(MOD_EVENTS_MINORS_INTERACTION)
		if (isCity() && MOD_EVENTS_MINORS_INTERACTION && GET_PLAYER(getOwner()).isMinorCiv())
		{
			CvCity* pPlotCity = getPlotCity();
			if (pUnit) 
			{
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanTransitMinorCity, ePlayer, pUnit->GetID(), pPlotCity->getOwner(), pPlotCity->GetID(), getX(), getY()) == GAMEEVENTRETURN_FALSE) 
				{
					return false;
				}
			} 
			else 
			{
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanTransitMinorCity, ePlayer, pPlotCity->getOwner(), pPlotCity->GetID(), getX(), getY()) == GAMEEVENTRETURN_FALSE) 
				{
					return false;
				}
			}
		}
#endif
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Is this a plot that's friendly to our team? (owned by us or someone we have Open Borders with)
bool CvPlot::IsFriendlyTerritory(PlayerTypes ePlayer) const
{
	// No friendly territory for barbs!
	if(ePlayer==NO_PLAYER || GET_PLAYER(ePlayer).isBarbarian())
	{
		return false;
	}

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	TeamTypes ePlotOwner = getTeam();

	// Nobody owns this plot
	if(ePlotOwner == NO_TEAM)
	{
		return false;
	}

	// Our territory
	if(ePlotOwner == eTeam)
	{
		return true;
	}

	// City State's territory we've earned OB with
	if(!GET_PLAYER(ePlayer).isMinorCiv())
	{
		if(GET_TEAM(ePlotOwner).isMinorCiv())
		{
			PlayerTypes eCityState = GET_TEAM(ePlotOwner).getLeaderID();

			if(GET_PLAYER(eCityState).GetMinorCivAI()->IsPlayerHasOpenBorders(ePlayer))
			{
				return true;
			}
		}
	}

	// Major's territory we have OB with
	if(GET_TEAM(ePlotOwner).IsAllowsOpenBordersToTeam(eTeam))
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isBeingWorked() const
{
	CvCity* pOwningCity = getOwningCity();

	if(pOwningCity != NULL)
	{
		return pOwningCity->GetCityCitizens()->IsWorkingPlot(this);
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isUnit() const
{
	return (getNumUnits() > 0);
}

//	-----------------------------------------------------------------------------------------------
inline static bool isEnemy(const CvUnit* pUnit, TeamTypes eOtherTeam, bool bAlwaysHostile)
{
	if(pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();
	return (bAlwaysHostile ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));
}

//	-----------------------------------------------------------------------------------------------
inline static bool isOtherTeam(const CvUnit* pUnit, TeamTypes eOtherTeam)
{
	if(pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (pUnit->getTeam() != eOtherTeam);
}

//	-----------------------------------------------------------------------------------------------
bool CvPlot::isVisibleEnemyDefender(const CvUnit* pUnit) const
{
	TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();

	if (!isVisible(eTeam))
		return false;

	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		bool bAlwaysHostile = pUnit->isAlwaysHostile(*this);

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit)
			{
				if(pLoopUnit->IsCanDefend() && isEnemy(pLoopUnit, eTeam, bAlwaysHostile))
				{
					return true;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}

//	-----------------------------------------------------------------------------------------------
//  ignores embarked units!
//	-----------------------------------------------------------------------------------------------
CvUnit* CvPlot::getVisibleEnemyDefender(PlayerTypes ePlayer) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (!isVisible(eTeam))
		return NULL;

	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		do
		{
			CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
			{
				if(pLoopUnit->IsCanDefend() && isEnemy(pLoopUnit, eTeam, false))
				{
					return pLoopUnit;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return NULL;
}

//	-----------------------------------------------------------------------------------------------
int CvPlot::getNumDefenders(PlayerTypes ePlayer) const
{
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode != NULL)
	{
		int iCount = 0;
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit)
			{
				if( (ePlayer==NO_PLAYER || pLoopUnit->getOwner()==ePlayer) && pLoopUnit->IsCanDefend() )
				{
					++iCount;
				}
			}
		}
		while(pUnitNode != NULL);
		return iCount;
	}

	return 0;
}
#if defined(MOD_BALANCE_CORE)
//	-----------------------------------------------------------------------------------------------
int CvPlot::getNumNavalDefenders(PlayerTypes ePlayer) const
{
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode != NULL)
	{
		int iCount = 0;
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit)
			{
				if( (ePlayer==NO_PLAYER || pLoopUnit->getOwner()==ePlayer) && pLoopUnit->IsCanDefend() && !pLoopUnit->isEmbarked() && (pLoopUnit->getDomainType() != DOMAIN_SEA))
				{
					++iCount;
				}
			}
		}
		while(pUnitNode != NULL);
		return iCount;
	}

	return 0;
}
#endif
//	-----------------------------------------------------------------------------------------------
int CvPlot::getNumVisibleEnemyDefenders(const CvUnit* pUnit) const
{
	TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();

	if (!isVisible(eTeam))
		return 0;

	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		bool bAlwaysHostile = pUnit->isAlwaysHostile(*this);
		int iCount = 0;

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false) && !pLoopUnit->IsDead())
			{
				if(pLoopUnit->IsCanDefend() && isEnemy(pLoopUnit, eTeam, bAlwaysHostile))
				{
					++iCount;
				}
			}
		}
		while(pUnitNode != NULL);
		return iCount;
	}
	return 0;
}

int CvPlot::getNumUnitsOfAIType(UnitAITypes eType, PlayerTypes ePlayer) const
{
	const IDInfo* pUnitNode = m_units.head();
	int iCount = 0;
	while(pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = m_units.next(pUnitNode);

		if(pLoopUnit && pLoopUnit->AI_getUnitAIType()==eType)
			if(ePlayer==NO_PLAYER || pLoopUnit->getOwner()==ePlayer)
				++iCount;
	}

	return iCount;
}

//	-----------------------------------------------------------------------------------------------
CvUnit* CvPlot::getFirstUnitOfAITypeSameTeam(TeamTypes eTeam, UnitAITypes eType) const
{
	const IDInfo* pUnitNode = m_units.head();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = m_units.next(pUnitNode);

		if (pLoopUnit && pLoopUnit->AI_getUnitAIType() == eType)
			if (eTeam == NO_TEAM || pLoopUnit->getTeam() == eTeam)
				return pLoopUnit;
	}

	return NULL;
}

//	-----------------------------------------------------------------------------------------------
CvUnit* CvPlot::getFirstUnitOfAITypeOtherTeam(TeamTypes eTeam, UnitAITypes eType) const
{
	const IDInfo* pUnitNode = m_units.head();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = m_units.next(pUnitNode);

		if (pLoopUnit && pLoopUnit->AI_getUnitAIType() == eType)
			if (eTeam == NO_TEAM || pLoopUnit->getTeam() != eTeam)
				return pLoopUnit;
	}

	return NULL;
}

//	-----------------------------------------------------------------------------------------------
bool CvPlot::isVisibleEnemyUnit(PlayerTypes ePlayer) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (!isVisible(eTeam))
		return false;

	CvAssertMsg(ePlayer != NO_PLAYER, "Source player must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false) && !pLoopUnit->IsDead())
			{
				if(isEnemy(pLoopUnit, eTeam, false))
				{
					return true;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}

//	-----------------------------------------------------------------------------------------------
bool CvPlot::isVisibleEnemyUnit(const CvUnit* pUnit) const
{
	TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();

	if (!isVisible(eTeam))
		return false;

	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		bool bAlwaysHostile = pUnit->isAlwaysHostile(*this);

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false) && !pLoopUnit->IsDead())
			{
				if(isEnemy(pLoopUnit, eTeam, bAlwaysHostile))
				{
					return true;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}

//	-----------------------------------------------------------------------------------------------
bool CvPlot::isVisibleOtherUnit(PlayerTypes ePlayer) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (!isVisible(eTeam))
		return false;

	CvAssertMsg(ePlayer != NO_PLAYER, "Source player must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false) && !pLoopUnit->IsDead())
			{
				if(isOtherTeam(pLoopUnit, eTeam))
				{
					return true;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}

bool CvPlot::isEnemyUnit(PlayerTypes ePlayer, bool bCombat, bool bCheckVisibility, bool bIgnoreBarbs) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (bCheckVisibility && !isVisible(eTeam))
		return false;

	CvAssertMsg(ePlayer != NO_PLAYER, "Source player must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false) && !pLoopUnit->IsDead())
			{
				if (bCombat && !pLoopUnit->IsCanDefend())
					continue;

				if (bIgnoreBarbs && pLoopUnit->isBarbarian())
					continue;

				if(isOtherTeam(pLoopUnit, eTeam) && isEnemy(pLoopUnit,eTeam,false))
				{
					return true;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}


bool CvPlot::isNeutralUnit(PlayerTypes ePlayer, bool bCombat, bool bCheckVisibility, bool bIgnoreMinors) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (bCheckVisibility && !isVisible(eTeam))
		return false;

	CvAssertMsg(ePlayer != NO_PLAYER, "Source player must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false) && !pLoopUnit->IsDead())
			{
				if (bCombat && !pLoopUnit->IsCanDefend())
					continue;

				if (bIgnoreMinors && GET_PLAYER(pLoopUnit->getOwner()).isMinorCiv())
					continue;

				if(isOtherTeam(pLoopUnit, eTeam) && !isEnemy(pLoopUnit,eTeam,false))
				{
					return true;
				}
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}

bool CvPlot::isNeutralUnitAdjacent(PlayerTypes ePlayer, bool bCombat, bool bCheckVisibility, bool bIgnoreMinors) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if (pAdjacentPlot != NULL)
			if (pAdjacentPlot->isNeutralUnit(ePlayer, bCombat, bCheckVisibility, bIgnoreMinors))
				return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
// does not check visibility!
bool CvPlot::IsBlockadeUnit(PlayerTypes ePlayer, bool bFriendly) const
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam& kTeam = GET_TEAM(eTeam);

	for(int iUnitLoop = 0; iUnitLoop < getNumUnits(); iUnitLoop++)
	{
		CvUnit* pkUnit = getUnitByIndex(iUnitLoop);
		if(pkUnit)
		{
			bool bCorrectOwner = false;
			if (bFriendly)
			{
				if ( pkUnit->getTeam()==eTeam )
				{
					bCorrectOwner = true;
				}
				else 
				{
					CvPlayer& kOwner = GET_PLAYER(pkUnit->getOwner());
					// Is it an allied minor or DoF major?
					if ( (kOwner.isMinorCiv() && kOwner.GetMinorCivAI()->GetAlly() == ePlayer) ||
						 (!kOwner.isBarbarian() && kOwner.GetDiplomacyAI()->IsDoFAccepted(ePlayer)) )
					{
						bCorrectOwner = true;
					}
				}
			}
			else
			{
				bCorrectOwner = kTeam.isAtWar(pkUnit->getTeam());
			}

			if (bCorrectOwner)
			{
				//exclude civilians
				if(pkUnit->IsCanAttack())
				{
					//exclude embarked units, ships in port etc
					if (pkUnit->isNativeDomain(this))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//	---------------------------------------------------------------------------
//	Return true if any fighting is occurring in the plot.
bool CvPlot::isFighting() const
{
	return (isUnitFighting() || isCityFighting());
}

//	---------------------------------------------------------------------------
//	Return true if any units are fighting in the plot.
bool CvPlot::isUnitFighting() const
{
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && pLoopUnit->isFighting())
			{
				return true;
			}
		}
		while(pUnitNode != NULL);
	}

	return false;
}

//	---------------------------------------------------------------------------
//	Return true if any cities are fighting in the plot.
bool CvPlot::isCityFighting() const
{
	CvCity* pkCity = getPlotCity();
	if(pkCity)
		return pkCity->isFighting();

	return false;
}

//	---------------------------------------------------------------------------
bool CvPlot::canHaveFeature(FeatureTypes eFeature) const
{
	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if(eFeature == NO_FEATURE)
	{
		return true;
	}

	if(getFeatureType() != NO_FEATURE)
	{
		return false;
	}

	if(isMountain())
	{
		return false;
	}

	if(isCity())
	{
		return false;
	}

	CvFeatureInfo* pkFeature = GC.getFeatureInfo(eFeature);
	if(pkFeature == NULL)
	{
		return false;
	}

	if(!pkFeature->isTerrain(getTerrainType()))
	{
		return false;
	}

	if(pkFeature->isNoCoast() && isCoastalLand())
	{
		return false;
	}

	if(pkFeature->isNoRiver() && isRiver())
	{
		return false;
	}

	if(pkFeature->isRequiresFlatlands() && isHills())
	{
		return false;
	}

	if(pkFeature->isNoAdjacent())
	{
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->getFeatureType() == eFeature)
				{
					return false;
				}
			}
		}
	}

	if(!isRiver() && pkFeature->isRequiresRiver())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isRoute() const
{
	return ((RouteTypes)m_eRouteType != NO_ROUTE);
}


//	--------------------------------------------------------------------------------
bool CvPlot::isValidRoute(const CvUnit* pUnit) const
{
	if((RouteTypes)m_eRouteType != NO_ROUTE && !m_bRoutePillaged)
	{
		if(!pUnit || !pUnit->isEnemy(getTeam(), this) || pUnit->isEnemyRoute())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetCityConnection(PlayerTypes ePlayer, bool bActive)
{
	if (ePlayer == NO_PLAYER)
		return;

	if( GET_PLAYER(ePlayer).UpdateCityConnection(this,bActive) )
	{
		for(int iI = 0; iI < MAX_TEAMS; ++iI)
		{
#ifdef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
			if ( GET_TEAM((TeamTypes)iI).isObserver() || ((GET_TEAM((TeamTypes)iI).isAlive()) && GC.getGame().getActiveTeam() == (TeamTypes)iI) )
#else
			if(GET_TEAM((TeamTypes)iI).isAlive() && GC.getGame().getActiveTeam() == (TeamTypes)iI)
#endif
			{
				if(isVisible((TeamTypes)iI))
				{
					setLayoutDirty(true);
				}
			}
		}
#if defined(MOD_BALANCE_CORE)
		updateYield();
#endif
	}
}


//	--------------------------------------------------------------------------------
bool CvPlot::IsCityConnection(PlayerTypes ePlayer) const
{
	if (ePlayer == NO_PLAYER)
		ePlayer = getOwner();

	//hack: for unowned plots, pretend there is no connection
	if (ePlayer == NO_PLAYER)
		return false;

	return GET_PLAYER(ePlayer).IsCityConnectionPlot(this);
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
void CvPlot::SetTradeUnitRoute(bool bActive)
{
	m_bIsTradeUnitRoute = bActive;
}


//	--------------------------------------------------------------------------------
bool CvPlot::IsTradeUnitRoute() const
{
	return m_bIsTradeUnitRoute;
}
#endif

//	--------------------------------------------------------------------------------
bool CvPlot::at(int iX, int iY) const
{
	return ((getX() == iX) && (getY() == iY));
}


//	--------------------------------------------------------------------------------
int CvPlot::getLatitude() const
{
	int iLatitude;

	if(GC.getMap().isWrapX() || !(GC.getMap().isWrapY()))
	{
		iLatitude = ((getY() * 100) / GC.getMap().getGridHeight());
	}
	else
	{
		iLatitude = ((getX() * 100) / GC.getMap().getGridWidth());
	}

	iLatitude = ((iLatitude * (GC.getMap().getTopLatitude() - GC.getMap().getBottomLatitude())) / 100);

	return abs(iLatitude + GC.getMap().getBottomLatitude());
}


//	--------------------------------------------------------------------------------
CvArea* CvPlot::area() const
{
	return GC.getMap().getArea(getArea());
}


//	--------------------------------------------------------------------------------
std::vector<int> CvPlot::getAllAdjacentAreas() const
{
	std::vector<int> result;

	CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(this);
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = aNeighbors[iI];
		//exclude our own area! also, simply deduplication, but no guarantee for unique values in result
		if(pAdjacentPlot != NULL && pAdjacentPlot->getArea() != getArea())
			if (result.empty() || result.back()!=pAdjacentPlot->getArea())
				result.push_back(pAdjacentPlot->getArea());
	}

	return result;
}

//	--------------------------------------------------------------------------------
void CvPlot::setArea(int iNewValue)
{
	bool bOldLake;

	if(getArea() != iNewValue)
	{
		bOldLake = isLake();

		if(area() != NULL)
		{
			processArea(area(), -1);
		}

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_AREA, m_iX, m_iY, 0, iNewValue, m_iArea, -1, -1);
		}
#endif

		m_iArea = iNewValue;

		if(area() != NULL)
		{
			processArea(area(), 1);

			updateYield();
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::setLandmass(int iNewValue)
{
	if(m_iLandmass != iNewValue)
	{
		// cleanup old one
		CvLandmass* pLandmass = GC.getMap().getLandmass(m_iLandmass);
		if(pLandmass != NULL)
		{
			pLandmass->changeNumTiles(-1);
			pLandmass->ChangeCentroidX(-m_iX);
			pLandmass->ChangeCentroidY(-m_iY);
		}

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_LANDMASS, m_iX, m_iY, 0, iNewValue, m_iLandmass, -1, -1);
		}
#endif

		m_iLandmass = iNewValue;

		pLandmass = GC.getMap().getLandmass(m_iLandmass);
		if(pLandmass != NULL)
		{
			pLandmass->changeNumTiles(1);
			pLandmass->ChangeCentroidX(m_iX);
			pLandmass->ChangeCentroidY(m_iY);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlot::getOwnershipDuration() const
{
	return m_iOwnershipDuration;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isOwnershipScore() const
{
	return (getOwnershipDuration() >= GC.getOWNERSHIP_SCORE_DURATION_THRESHOLD());
}


//	--------------------------------------------------------------------------------
void CvPlot::setOwnershipDuration(int iNewValue)
{
	bool bOldOwnershipScore;

	if(getOwnershipDuration() != iNewValue)
	{
		bOldOwnershipScore = isOwnershipScore();

		m_iOwnershipDuration = iNewValue;
		CvAssert(getOwnershipDuration() >= 0);

		if(bOldOwnershipScore != isOwnershipScore())
		{
			if(isOwned())
			{
				if(!isWater())
				{
					GET_PLAYER(getOwner()).changeTotalLandScored((isOwnershipScore()) ? 1 : -1);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlot::changeOwnershipDuration(int iChange)
{
	setOwnershipDuration(getOwnershipDuration() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlot::getImprovementDuration() const
{
	return m_iImprovementDuration;
}


//	--------------------------------------------------------------------------------
void CvPlot::setImprovementDuration(int iNewValue)
{
	m_iImprovementDuration = iNewValue;
	CvAssert(getImprovementDuration() >= 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::changeImprovementDuration(int iChange)
{
	setImprovementDuration(getImprovementDuration() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlot::getUpgradeProgress() const
{
	return m_iUpgradeProgress;
}


//	--------------------------------------------------------------------------------
int CvPlot::getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const
{
	int iUpgradeLeft;
	int iUpgradeRate;
	int iTurnsLeft;

	iUpgradeLeft = ((100 * GC.getGame().getImprovementUpgradeTime(eImprovement, this)) - ((getImprovementType() == eImprovement) ? getUpgradeProgress() : 0));
	iUpgradeLeft /= 100;

	if(ePlayer == NO_PLAYER)
	{
		return iUpgradeLeft;
	}

	iUpgradeRate = GET_PLAYER(ePlayer).getImprovementUpgradeRate();

	if(iUpgradeRate == 0)
	{
		return iUpgradeLeft;
	}

	// Upgrade rate is stored at 100x
	iTurnsLeft = (100 * iUpgradeLeft / iUpgradeRate);

	if((iTurnsLeft * iUpgradeRate) < iUpgradeLeft)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}


//	--------------------------------------------------------------------------------
void CvPlot::setUpgradeProgress(int iNewValue)
{
	m_iUpgradeProgress = iNewValue;
	CvAssert(getUpgradeProgress() >= 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::changeUpgradeProgress(int iChange)
{
	setUpgradeProgress(getUpgradeProgress() + iChange);
}

//	--------------------------------------------------------------------------------
#if defined(MOD_API_UNIFIED_YIELDS)
int CvPlot::ComputeYieldFromAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue, YieldTypes eYield) const
#else
int CvPlot::ComputeCultureFromAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue) const
#endif
{
	CvPlot* pAdjacentPlot;
	int iRtnValue = 0;

#if defined(MOD_API_UNIFIED_YIELDS)
	if(kImprovement.GetYieldAdjacentSameType(eYield) > 0)
#else
	if(kImprovement.GetCultureAdjacentSameType() > 0)
#endif
	{
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eValue)
			{
#if defined(MOD_API_UNIFIED_YIELDS)
				iRtnValue += kImprovement.GetYieldAdjacentSameType(eYield);
#else
				iRtnValue += kImprovement.GetCultureAdjacentSameType();
#endif
			}
		}
	}

	return iRtnValue;
}
#if defined(MOD_API_UNIFIED_YIELDS)
int CvPlot::ComputeYieldFromTwoAdjacentImprovement(CvImprovementEntry& kImprovement, ImprovementTypes eValue, YieldTypes eYield) const
{
	CvPlot* pAdjacentPlot;
	int iRtnValue = 0;

	if(kImprovement.GetYieldAdjacentTwoSameType(eYield) > 0)
	{
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eValue)
			{
				iRtnValue += kImprovement.GetYieldAdjacentTwoSameType(eYield);
			}
		}
	}

	iRtnValue /= 2;

	return iRtnValue;
}
int CvPlot::ComputeYieldFromOtherAdjacentImprovement(CvImprovementEntry& kImprovement, YieldTypes eYield) const
{
	CvPlot* pAdjacentPlot;
	int iRtnValue = 0;

	for(int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
	{
		ImprovementTypes eImprovement = (ImprovementTypes)iJ;
		if(eImprovement != NO_IMPROVEMENT)
		{
			if(kImprovement.GetAdjacentImprovementYieldChanges(eImprovement, eYield) > 0)
			{
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
					if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eImprovement)
					{
						iRtnValue += kImprovement.GetAdjacentImprovementYieldChanges(eImprovement, eYield);
					}
				}
			}
		}
	}

	return iRtnValue;
}
int CvPlot::ComputeYieldFromAdjacentResource(CvImprovementEntry& kImprovement, YieldTypes eYield) const
{
	CvPlot* pAdjacentPlot;
	int iRtnValue = 0;

	for(int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
	{
		ResourceTypes eResource = (ResourceTypes)iJ;
		if(eResource != NO_RESOURCE)
		{
			if(kImprovement.GetAdjacentResourceYieldChanges(eResource, eYield) > 0)
			{
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
					if(pAdjacentPlot && pAdjacentPlot->getResourceType() == eResource)
					{
						iRtnValue += kImprovement.GetAdjacentResourceYieldChanges(eResource, eYield);
					}
				}
			}
		}
	}

	return iRtnValue;
}
int CvPlot::ComputeYieldFromAdjacentTerrain(CvImprovementEntry& kImprovement, YieldTypes eYield) const
{
	CvPlot* pAdjacentPlot;
	int iRtnValue = 0;

	for(int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
	{
		TerrainTypes eTerrain = (TerrainTypes)iJ;
		if(eTerrain != NO_TERRAIN)
		{
			if(kImprovement.GetAdjacentTerrainYieldChanges(eTerrain, eYield) > 0)
			{
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
					if(pAdjacentPlot && pAdjacentPlot->getTerrainType() == eTerrain)
					{
						iRtnValue += kImprovement.GetAdjacentTerrainYieldChanges(eTerrain, eYield);
					}
				}
			}
		}
	}

	return iRtnValue;
}

int CvPlot::ComputeYieldFromAdjacentFeature(CvImprovementEntry& kImprovement, YieldTypes eYield) const
{
	CvPlot* pAdjacentPlot;
	int iRtnValue = 0;

	for(int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
	{
		FeatureTypes eFeature = (FeatureTypes)iJ;
		if (eFeature != NO_FEATURE)
		{
			if (kImprovement.GetAdjacentFeatureYieldChanges(eFeature, eYield) > 0)
			{
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
					if (pAdjacentPlot && pAdjacentPlot->getFeatureType() == eFeature)
					{
						iRtnValue += kImprovement.GetAdjacentFeatureYieldChanges(eFeature, eYield);
					}
				}
			}
		}
	}

	return iRtnValue;
}
#endif
//	--------------------------------------------------------------------------------
#if defined(MOD_GLOBAL_STACKING_RULES)
int CvPlot::getStackingUnits() const
{
    const IDInfo* pUnitNode = headUnitNode();
    CvUnit* pLoopUnit;

    while(pUnitNode != NULL)
    {
      pLoopUnit = GetPlayerUnit(*pUnitNode);
      pUnitNode = nextUnitNode(pUnitNode);

      if(pLoopUnit && pLoopUnit->getNumberStackingUnits() > 0)
      {
        return pLoopUnit->getNumberStackingUnits();
      }
    }
    return 0;
}
//	--------------------------------------------------------------------------------
int CvPlot::getAdditionalUnitsFromImprovement() const
{
	if(MOD_GLOBAL_STACKING_RULES)
	{
		return m_eUnitIncrement;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvPlot::calculateAdditionalUnitsFromImprovement()
{
	if(MOD_GLOBAL_STACKING_RULES)
	{
		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (IsImprovementPillaged())
			{
				m_eUnitIncrement = 0;
			}
			else
			{
				m_eUnitIncrement = GC.getImprovementInfo(getImprovementType())->GetAdditionalUnits();
			}
		}
		else
		{
			m_eUnitIncrement = 0;
		}
	}
}
#endif

//	--------------------------------------------------------------------------------
int CvPlot::getNumMajorCivsRevealed() const
{
	return m_iNumMajorCivsRevealed;
}

//	--------------------------------------------------------------------------------
void CvPlot::setNumMajorCivsRevealed(int iNewValue)
{
	m_iNumMajorCivsRevealed = iNewValue;
	CvAssert(getNumMajorCivsRevealed() >= 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::changeNumMajorCivsRevealed(int iChange)
{
	setNumMajorCivsRevealed(getNumMajorCivsRevealed() + iChange);
}


//	--------------------------------------------------------------------------------
int CvPlot::getCityRadiusCount() const
{
	return m_iCityRadiusCount;
}


//	--------------------------------------------------------------------------------
int CvPlot::isCityRadius() const
{
	return (getCityRadiusCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::changeCityRadiusCount(int iChange)
{
	m_iCityRadiusCount = (m_iCityRadiusCount + iChange);
	CvAssert(getCityRadiusCount() >= 0);
}


//	--------------------------------------------------------------------------------
bool CvPlot::isStartingPlot() const
{
	return m_bStartingPlot;
}


//	--------------------------------------------------------------------------------
void CvPlot::setStartingPlot(bool bNewValue)
{
	m_bStartingPlot = bNewValue;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isNEOfRiver() const
{
	return m_bNEOfRiver;
}


//	--------------------------------------------------------------------------------
void CvPlot::setNEOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir)
{
	CvPlot* pAdjacentPlot;
	int iI;

	if((isNEOfRiver() != bNewValue) || (eRiverDir != m_eRiverSWFlowDirection))
	{
		CvAssertMsg(m_eRiverSWFlowDirection == NO_FLOWDIRECTION && eRiverDir != NO_FLOWDIRECTION, "invalid parameter");
		if(isNEOfRiver() != bNewValue)
		{
#if defined(MOD_EVENTS_TERRAFORMING)
			if (MOD_EVENTS_TERRAFORMING) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_RIVER, m_iX, m_iY, DIRECTION_NORTHEAST, ((int) bNewValue), ((int) m_bNEOfRiver), eRiverDir, m_eRiverSWFlowDirection);
			}
#endif

			m_bNEOfRiver = bNewValue;

			updateRiverCrossing();
			updateYield();

			for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if(pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
			}

			if(area() != NULL)
			{
				area()->changeNumRiverEdges((isNEOfRiver()) ? 1 : -1);
			}
		}

		CvAssertMsg(eRiverDir == FLOWDIRECTION_NORTHWEST || eRiverDir == FLOWDIRECTION_SOUTHEAST || eRiverDir == NO_FLOWDIRECTION, "invalid parameter");
		m_eRiverSWFlowDirection = eRiverDir;
	}
}


//	--------------------------------------------------------------------------------
bool CvPlot::isWOfRiver() const
{
	return m_bWOfRiver;
}


//	--------------------------------------------------------------------------------
void CvPlot::setWOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir)
{
	CvPlot* pAdjacentPlot;
	int iI;

	if((isWOfRiver() != bNewValue) || (eRiverDir != m_eRiverEFlowDirection))
	{
		CvAssertMsg(m_eRiverEFlowDirection == NO_FLOWDIRECTION && eRiverDir != NO_FLOWDIRECTION, "invalid parameter");
		if(isWOfRiver() != bNewValue)
		{
#if defined(MOD_EVENTS_TERRAFORMING)
			if (MOD_EVENTS_TERRAFORMING) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_RIVER, m_iX, m_iY, DIRECTION_WEST, ((int) bNewValue), ((int) m_bWOfRiver), eRiverDir, m_eRiverEFlowDirection);
			}
#endif

			m_bWOfRiver = bNewValue;

			updateRiverCrossing();
			updateYield();

			for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if(pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
			}

			if(area())
			{
				area()->changeNumRiverEdges((isWOfRiver()) ? 1 : -1);
			}
		}

		CvAssertMsg(eRiverDir == FLOWDIRECTION_NORTH || eRiverDir == FLOWDIRECTION_SOUTH || eRiverDir == NO_FLOWDIRECTION, "invalid parameter");
		m_eRiverEFlowDirection= eRiverDir;
	}
}


//	--------------------------------------------------------------------------------
bool CvPlot::isNWOfRiver() const
{
	return m_bNWOfRiver;
}


//	--------------------------------------------------------------------------------
void CvPlot::setNWOfRiver(bool bNewValue, FlowDirectionTypes eRiverDir)
{
	CvPlot* pAdjacentPlot;
	int iI;

	if((isNWOfRiver() != bNewValue) || (eRiverDir != m_eRiverSEFlowDirection))
	{
		CvAssertMsg(m_eRiverSEFlowDirection == NO_FLOWDIRECTION && eRiverDir != NO_FLOWDIRECTION, "invalid parameter");
		if(isNWOfRiver() != bNewValue)
		{
#if defined(MOD_EVENTS_TERRAFORMING)
			if (MOD_EVENTS_TERRAFORMING) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_RIVER, m_iX, m_iY, DIRECTION_NORTHWEST, ((int) bNewValue), ((int) m_bNWOfRiver), eRiverDir, m_eRiverSEFlowDirection);
			}
#endif

			m_bNWOfRiver = bNewValue;

			updateRiverCrossing();
			updateYield();

			for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if(pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
			}

			if(area())
			{
				area()->changeNumRiverEdges((isNWOfRiver()) ? 1 : -1);
			}
		}

		CvAssertMsg(eRiverDir == FLOWDIRECTION_NORTHEAST || eRiverDir == FLOWDIRECTION_SOUTHWEST || eRiverDir == NO_FLOWDIRECTION, "invalid parameter");
		m_eRiverSEFlowDirection = eRiverDir;
	}
}


//	--------------------------------------------------------------------------------
FlowDirectionTypes CvPlot::getRiverEFlowDirection() const
{
	return (FlowDirectionTypes)m_eRiverEFlowDirection;
}


//	--------------------------------------------------------------------------------
FlowDirectionTypes CvPlot::getRiverSEFlowDirection() const
{
	return (FlowDirectionTypes)m_eRiverSEFlowDirection;
}

//	--------------------------------------------------------------------------------
FlowDirectionTypes CvPlot::getRiverSWFlowDirection() const
{
	return (FlowDirectionTypes)m_eRiverSWFlowDirection;
}


//	--------------------------------------------------------------------------------
// This function finds an *inland* corner of this plot at which to place a river.
// It then returns the plot with that corner at its SE.

CvPlot* CvPlot::getInlandCorner() const
{
	CvPlot* pRiverPlot = NULL; // will be a plot through whose SE corner we want the river to run
	int aiShuffle[4] = {0,1,2,3};
	shuffleArray(aiShuffle, 4, GC.getGame().getMapRand());

	for(int iI = 0; iI < 4; ++iI)
	{
		switch(aiShuffle[iI])
		{
		case 0:
			pRiverPlot = GC.getMap().plotCheckInvalid(getX(), getY());
			break;
		case 1:
#if defined(MOD_BALANCE_CORE)
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_NORTHEAST);
#else
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
#endif
			break;
		case 2:
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
			break;
		case 3:
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
			break;
		}
		if(pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
		{
			break;
		}
		else
		{
			pRiverPlot = NULL;
		}
	}

	return pRiverPlot;
}


//	--------------------------------------------------------------------------------
bool CvPlot::hasCoastAtSECorner() const
{
	CvPlot* pAdjacentPlot;

	if(isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_EAST);
	if(pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_SOUTHEAST);
	if(pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isPotentialCityWork() const
{
	return m_bPotentialCityWork;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isPotentialCityWorkForArea(CvArea* pArea) const
{
	CvPlot* pLoopPlot;
	int iI;


	for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
	{
		pLoopPlot = iterateRingPlots(getX(), getY(), iI);

		if(pLoopPlot != NULL)
		{
			if(!(pLoopPlot->isWater()) || GC.getWATER_POTENTIAL_CITY_WORK_FOR_AREA())
			{
				if(pLoopPlot->area() == pArea)
				{
					return true;
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvPlot::updatePotentialCityWork()
{
	CvPlot* pLoopPlot;
	bool bValid;
	int iI;

	bValid = false;


	for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
	{
		pLoopPlot = iterateRingPlots(getX(), getY(), iI);

		if(pLoopPlot != NULL)
		{
			if(!(pLoopPlot->isWater()))
			{
				bValid = true;
				break;
			}
		}
	}

	if(isPotentialCityWork() != bValid)
	{
		m_bPotentialCityWork = bValid;

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::setOwner(PlayerTypes eNewValue, int iAcquiringCityID, bool bCheckUnits, bool)
{
	IDInfo* pUnitNode;
	CvCity* pOldCity;
	CvString strBuffer;
	int iI;

	// Remove effects for old owner before changing the member
	if(getOwner() != eNewValue)
	{
		PlayerTypes eOldOwner = getOwner();

		GC.getGame().addReplayMessage(REPLAY_MESSAGE_PLOT_OWNER_CHANGE, eNewValue, "", getX(), getY());

		pOldCity = getPlotCity();

		{
			setOwnershipDuration(0);
#if defined(MOD_BALANCE_CORE)
			FeatureTypes eFeature = getFeatureType();
			if(eFeature != NO_FEATURE)
			{
				if (eNewValue != NO_PLAYER)
				{
					GET_PLAYER(eNewValue).SetNWOwned(eFeature, true);
				}
				PromotionTypes eFreePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getPromotionIfOwned();
				if(eFreePromotion != NO_PROMOTION && eNewValue != NO_PLAYER)
				{
					if (!GET_PLAYER(eNewValue).IsFreePromotion(eFreePromotion))
					{
						GET_PLAYER(eNewValue).ChangeFreePromotionCount(eFreePromotion, 1);
					}
				}
			}
#endif
			// Plot was owned by someone else
			if(isOwned())
			{
#if defined(MOD_BALANCE_CORE)
				if(eFeature != NO_FEATURE)
				{
					if (eOldOwner != NO_PLAYER)
					{
						GET_PLAYER(eOldOwner).SetNWOwned(eFeature, false);
					}
					PromotionTypes eFreePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getPromotionIfOwned();
					if(eFreePromotion != NO_PROMOTION && eOldOwner != NO_PLAYER)
					{
						if(GET_PLAYER(eOldOwner).IsFreePromotion(eFreePromotion))
						{
							GET_PLAYER(eOldOwner).ChangeFreePromotionCount(eFreePromotion, -1);
						}

					}
				}
#endif
#if defined(MOD_API_EXTENSIONS)
				changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), false, NO_INVISIBLE, NO_DIRECTION);
#else
				changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
#endif

				// if this tile is owned by a minor share the visibility with my ally
				if(pOldCity)
				{
					for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
					{
						PlayerTypes ePlayer = (PlayerTypes)ui;
						if(GET_PLAYER(ePlayer).GetEspionage()->HasEstablishedSurveillanceInCity(pOldCity))
						{
#if defined(MOD_API_EXTENSIONS)
							changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), false, NO_INVISIBLE, NO_DIRECTION);
#else
							changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
#endif
						}
					}
				}

				if(eOldOwner >= MAX_MAJOR_CIVS && eOldOwner != BARBARIAN_PLAYER)
				{
					CvPlayer& thisPlayer = GET_PLAYER(eOldOwner);
					CvMinorCivAI* pMinorCivAI = thisPlayer.GetMinorCivAI();
					if(pMinorCivAI && pMinorCivAI->GetAlly() != NO_PLAYER)
					{
#if defined(MOD_API_EXTENSIONS)
						changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), false, NO_INVISIBLE, NO_DIRECTION);
#else
						changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
#endif
					}
				}

				if(area())
				{
					area()->changeNumOwnedTiles(-1);
				}
				GC.getMap().changeOwnedPlots(-1);

				if(!isWater())
				{
					GET_PLAYER(getOwner()).changeTotalLand(-1);
					GET_TEAM(getTeam()).changeTotalLand(-1);

					if(isOwnershipScore())
					{
						GET_PLAYER(getOwner()).changeTotalLandScored(-1);
					}
				}

				// Improvement is here
				if(getImprovementType() != NO_IMPROVEMENT)
				{
					GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), -1);

					// Remove siphoned resources
					CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(getImprovementType());
					if (pImprovementInfo != NULL && pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor() > 0)
					{
						if (GetPlayerThatBuiltImprovement() != NO_PLAYER)
						{
							if (GET_PLAYER(eOldOwner).isMinorCiv())
							{
								GET_PLAYER(GetPlayerThatBuiltImprovement()).changeSiphonLuxuryCount(eOldOwner, -1 * pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor());
#if defined(MOD_BALANCE_CORE)
								GET_PLAYER(eOldOwner).GetMinorCivAI()->SetSiphoned(GetPlayerThatBuiltImprovement(), false);
#endif
							}
						}
					}
#if defined(MOD_BALANCE_CORE)
					if(pImprovementInfo->GetGrantsVision() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER)
					{
						int iPlotVisRange = pImprovementInfo->GetGrantsVision();
						changeAdjacentSight(GET_PLAYER(GetPlayerThatBuiltImprovement()).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
					}
					if (pImprovementInfo->GetUnitPlotExperience() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER)
					{
						ChangeUnitPlotExperience(pImprovementInfo->GetUnitPlotExperience() * -1);
					}
					if (pImprovementInfo->GetGAUnitPlotExperience() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER)
					{
						ChangeUnitPlotGAExperience(-1 * pImprovementInfo->GetGAUnitPlotExperience());
					}
					if (pImprovementInfo->GetMovesChange() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER)
					{
						ChangePlotMovesChange(-1 * pImprovementInfo->GetMovesChange());
					}
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES)
					// Embassy extra vote in WC mod
					if(MOD_DIPLOMACY_CITYSTATES && pImprovementInfo != NULL && pImprovementInfo->GetCityStateExtraVote() > 0)
					{
						if (GetPlayerThatBuiltImprovement() != NO_PLAYER)
						{
							if (GET_PLAYER(eOldOwner).isMinorCiv())
							{
								GET_PLAYER(GetPlayerThatBuiltImprovement()).ChangeImprovementLeagueVotes(pImprovementInfo->GetCityStateExtraVote() * -1);
							}
						}
					}
#endif
#if defined(MOD_BALANCE_CORE)
					//Resource from improvement - change ownership if needed.
					ResourceTypes eResourceFromImprovement = (ResourceTypes)pImprovementInfo->GetResourceFromImprovement();
					int iQuantity = pImprovementInfo->GetResourceQuantityFromImprovement();
					if(iQuantity <= 0)
					{
						iQuantity = 1;
					}

					if(eOldOwner != NO_PLAYER && eResourceFromImprovement != NO_RESOURCE && (getResourceType() != NO_RESOURCE && getResourceType() != eResourceFromImprovement))
					{
						setResourceType(NO_RESOURCE, 0);
					}
#endif
					// Maintenance change!
					if(MustPayMaintenanceHere(getOwner()))
					{
						GET_PLAYER(getOwner()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getImprovementInfo(getImprovementType())->GetGoldMaintenance());
					}
				}

				// Route is here
				if(getRouteType() != NO_ROUTE && !isCity())
				{
					// Maintenance change!
					if(MustPayMaintenanceHere(getOwner()))
					{
						GET_PLAYER(getOwner()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getRouteInfo(getRouteType())->GetGoldMaintenance());
					}
				}

				// Remove Resource Quantity from total
				if(getResourceType() != NO_RESOURCE)
				{
					// Disconnect resource link
					if(GetResourceLinkedCity() != NULL)
					{
						SetResourceLinkedCity(NULL);
					}

					if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechReveal()))
					{
#if defined(MOD_BALANCE_CORE)
						if(getImprovementType() != NO_IMPROVEMENT)
						{
							if (GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType()))
							{
								if(!IsImprovementPillaged())
								{
									GET_PLAYER(getOwner()).changeNumResourceTotal(getResourceType(), -getNumResourceForPlayer(getOwner()));
								}
							}
						}
#else
						if(getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType())->IsImprovementResourceTrade(getResourceType()))
						{
							if(!IsImprovementPillaged())
							{
								GET_PLAYER(getOwner()).changeNumResourceTotal(getResourceType(), -getNumResourceForPlayer(getOwner()));
							}
						}
#endif
					}
				}
			}
			// Plot is unowned
			else
			{
				// Someone paying for this improvement
				if(GetPlayerResponsibleForImprovement() != NO_PLAYER)
				{
					if(MustPayMaintenanceHere(GetPlayerResponsibleForImprovement()))
					{
						GET_PLAYER(GetPlayerResponsibleForImprovement()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getImprovementInfo(getImprovementType())->GetGoldMaintenance());
					}
					SetPlayerResponsibleForImprovement(NO_PLAYER);
				}
				// Someone paying for this Route
				if(GetPlayerResponsibleForRoute() != NO_PLAYER)
				{
					if(MustPayMaintenanceHere(GetPlayerResponsibleForRoute()))
					{
						GET_PLAYER(GetPlayerResponsibleForRoute()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getRouteInfo(getRouteType())->GetGoldMaintenance());
					}
					SetPlayerResponsibleForRoute(NO_PLAYER);
				}
			}

			// This plot is ABOUT TO BE owned. Pop Goody Huts/remove barb camps, etc. Otherwise it will try to reduce the # of Improvements we have in our borders, and these guys shouldn't apply to that count
			if(eNewValue != NO_PLAYER)
			{
				// Pop Goody Huts here
				if(isGoody())
				{
					GET_PLAYER(eNewValue).doGoody(this, NULL);
				}

				// If there's a camp here, clear it
				if(getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
				{
					setImprovementType(NO_IMPROVEMENT);
					CvBarbarians::DoBarbCampCleared(this, eNewValue);
					SetPlayerThatClearedBarbCampHere(eNewValue);

					// Don't give gold for Camps cleared by settling
				}
			}

			pUnitNode = headUnitNode();

			// ACTUALLY CHANGE OWNERSHIP HERE
			m_eOwner = eNewValue;

			setOwningCityOverride(NULL);
			updateOwningCity();

			// Post ownership switch

			if(isOwned())
			{
				CvPlayerAI& newPlayer = GET_PLAYER(eNewValue);
				if(iAcquiringCityID >= 0)
				{
					m_purchaseCity.eOwner = eNewValue;
					m_purchaseCity.iID = iAcquiringCityID;
				}
				else
				{
					m_purchaseCity.eOwner = NO_PLAYER;
					m_purchaseCity.iID = -1;
				}

#if defined(MOD_API_EXTENSIONS)
				changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), true, NO_INVISIBLE, NO_DIRECTION);
#else
				changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
#endif

				// if this tile is owned by a minor share the visibility with my ally
				if(pOldCity)
				{
					for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
					{
						PlayerTypes ePlayer = (PlayerTypes)ui;
						if(GET_PLAYER(ePlayer).GetEspionage()->HasEstablishedSurveillanceInCity(pOldCity))
						{
#if defined(MOD_API_EXTENSIONS)
							changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), true, NO_INVISIBLE, NO_DIRECTION);
#else
							changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
#endif
						}
					}
				}

				if(eNewValue >= MAX_MAJOR_CIVS && eNewValue != BARBARIAN_PLAYER)
				{
					CvPlayer& thisPlayer = GET_PLAYER(eNewValue);
					CvMinorCivAI* pMinorCivAI = thisPlayer.GetMinorCivAI();
					if(pMinorCivAI && pMinorCivAI->GetAlly() != NO_PLAYER)
					{
#if defined(MOD_API_EXTENSIONS)
						changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), true, NO_INVISIBLE, NO_DIRECTION);
#else
						changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
#endif
					}
				}

				if(area())
				{
					area()->changeNumOwnedTiles(1);
				}
				GC.getMap().changeOwnedPlots(1);

				if(!isWater())
				{
					GET_PLAYER(getOwner()).changeTotalLand(1);
					GET_TEAM(getTeam()).changeTotalLand(1);

					if(isOwnershipScore())
					{
						GET_PLAYER(getOwner()).changeTotalLandScored(1);
					}
				}

				// Improvement is here
				if(getImprovementType() != NO_IMPROVEMENT)
				{
					GET_PLAYER(eNewValue).changeImprovementCount(getImprovementType(), 1);

					// Add siphoned resources
					CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(getImprovementType());
					if (pImprovementInfo != NULL && pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor() > 0)
					{
						if (GetPlayerThatBuiltImprovement() != NO_PLAYER)
						{
							if (GET_PLAYER(eNewValue).isMinorCiv())
							{
								GET_PLAYER(GetPlayerThatBuiltImprovement()).changeSiphonLuxuryCount(eNewValue, pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor());
#if defined(MOD_BALANCE_CORE)
								GET_PLAYER(eNewValue).GetMinorCivAI()->SetSiphoned(GetPlayerThatBuiltImprovement(), true);
#endif
							}
						}
					}
#if defined(MOD_BALANCE_CORE)
					//Did someone else build this, and now you own it? Let's shift that around.
					if(pImprovementInfo->GetGrantsVision() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER && getOwner() != GetPlayerThatBuiltImprovement())
					{
						int iPlotVisRange = pImprovementInfo->GetGrantsVision();
						changeAdjacentSight(GET_PLAYER(GetPlayerThatBuiltImprovement()).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
						changeAdjacentSight(GET_PLAYER(getOwner()).getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION, false);
					}
					if (pImprovementInfo->GetUnitPlotExperience() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER && getOwner() != GetPlayerThatBuiltImprovement())
					{
						ChangeUnitPlotExperience(pImprovementInfo->GetUnitPlotExperience());
					}
					if (pImprovementInfo->GetGAUnitPlotExperience() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER && getOwner() != GetPlayerThatBuiltImprovement())
					{
						ChangeUnitPlotGAExperience(pImprovementInfo->GetGAUnitPlotExperience());
					}
					if (pImprovementInfo->GetMovesChange() > 0 && GetPlayerThatBuiltImprovement() != NO_PLAYER)
					{
						ChangePlotMovesChange(pImprovementInfo->GetMovesChange());
					}
#endif
#if defined(MOD_BALANCE_CORE)
					//Resource from improvement - change ownership if needed.
					ResourceTypes eResourceFromImprovement = (ResourceTypes)pImprovementInfo->GetResourceFromImprovement();
					int iQuantity = pImprovementInfo->GetResourceQuantityFromImprovement();
					if(iQuantity <= 0)
					{
						iQuantity = 1;
					}

					if(eResourceFromImprovement != NO_RESOURCE && (getResourceType() != NO_RESOURCE && getResourceType() != eResourceFromImprovement))
					{
						setResourceType(eResourceFromImprovement, iQuantity);
					}
#endif
					// Maintenance change!
					if(MustPayMaintenanceHere(eNewValue))
					{
						GET_PLAYER(eNewValue).GetTreasury()->ChangeBaseImprovementGoldMaintenance(GC.getImprovementInfo(getImprovementType())->GetGoldMaintenance());
					}
				}

#if defined(MOD_DIPLOMACY_CITYSTATES)
				// Embassy is here (somehow- city-state conquest/reconquest, perhaps?
				if(MOD_DIPLOMACY_CITYSTATES && getImprovementType() != NO_IMPROVEMENT)
				{
					// Add vote
					CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(getImprovementType());
					if (pImprovementInfo != NULL)
					{
						if (pImprovementInfo->GetCityStateExtraVote() > 0)
						{
							if (GetPlayerThatBuiltImprovement() != NO_PLAYER)
							{
								if (GET_PLAYER(eNewValue).isMinorCiv())
								{
									GET_PLAYER(GetPlayerThatBuiltImprovement()).ChangeImprovementLeagueVotes(pImprovementInfo->GetCityStateExtraVote());
								}
							}
						}
						if (pImprovementInfo->IsEmbassy())
							SetImprovementEmbassy(true);
						else
							SetImprovementEmbassy(false);
					}
				}
#endif
				// Route is here
				if(getRouteType() != NO_ROUTE && !isCity())
				{
					// Maintenance change!
					if(MustPayMaintenanceHere(eNewValue))
					{
						GET_PLAYER(eNewValue).GetTreasury()->ChangeBaseImprovementGoldMaintenance(GC.getRouteInfo(getRouteType())->GetGoldMaintenance());
					}
				}

				if(getResourceType() != NO_RESOURCE)
				{
					// Add Resource Quantity to total
					if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
					{
						if(getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType()))
						{
							if(!IsImprovementPillaged())
							{
								GET_PLAYER(getOwner()).changeNumResourceTotal(getResourceType(), getNumResourceForPlayer(getOwner()));
							}
						}
					}

					// Should we link the Resource here with a City so special Buildings may be constructed?
					if(GetResourceLinkedCity() == NULL)
					{
						DoFindCityToLinkResourceTo();
					}
				}

				// update the high water mark for player (if human)
				int iMaxCityHighWaterMark = GC.getMAX_CITY_DIST_HIGHWATER_MARK();
				if(newPlayer.isHuman() && newPlayer.GetCityDistanceHighwaterMark() < iMaxCityHighWaterMark)
				{
					int bestHighWaterMark = newPlayer.GetCityDistanceHighwaterMark();
					for(int iDX = -iMaxCityHighWaterMark; iDX <= iMaxCityHighWaterMark && bestHighWaterMark < iMaxCityHighWaterMark; iDX++)
					{
						for(int iDY = -iMaxCityHighWaterMark; iDY <= iMaxCityHighWaterMark && bestHighWaterMark < iMaxCityHighWaterMark; iDY++)
						{
							int hexRange;

							// I'm assuming iDX and iDY are in hex-space
							if((iDX >= 0) == (iDY >= 0))  // the signs match
							{
								int iAbsDX = iDX >= 0 ? iDX : -iDX;
								int iAbsDY = iDY >= 0 ? iDY : -iDY;
								hexRange = iAbsDX + iAbsDY;
							}
							else
							{
								int iAbsDX = iDX >= 0 ? iDX : -iDX;
								int iAbsDY = iDY >= 0 ? iDY : -iDY;
								hexRange = iAbsDX >= iAbsDY ? iAbsDX : iAbsDY;
							}

							if(hexRange > bestHighWaterMark && hexRange <= iMaxCityHighWaterMark)
							{
								CvPlot* pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
								if(pLoopPlot)
								{
									CvCity* thisCity = pLoopPlot->getPlotCity();
									if(thisCity)
									{
										if(thisCity->getOwner() == eNewValue)
										{
											bestHighWaterMark = hexRange;
										}
									}
								}
							}
						}
					}
					if(bestHighWaterMark > newPlayer.GetCityDistanceHighwaterMark())
					{
						newPlayer.SetCityDistanceHighwaterMark(bestHighWaterMark);
						if(GC.GetEngineUserInterface()->isCityScreenUp())
						{
							auto_ptr<ICvCity1> pHeadSelectedCity(GC.GetEngineUserInterface()->getHeadSelectedCity());
							if(pHeadSelectedCity.get())
							{
								CvCity* pkHeadSelectedCity = GC.UnwrapCityPointer(pHeadSelectedCity.get());
								auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pkHeadSelectedCity->plot());
								GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_CITY_ZOOM_IN);
							}
						}
					}
				}

#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
				if (MOD_EVENTS_TILE_IMPROVEMENTS) {
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileOwnershipChanged, getX(), getY(), getOwner(), eOldOwner);
				}
#endif
			}

			pUnitNode = headUnitNode();

			for(iI = 0; iI < MAX_TEAMS; ++iI)
			{
#ifdef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
				if (GET_TEAM((TeamTypes)iI).isObserver() || GET_TEAM((TeamTypes)iI).isAlive())
#else
				if(GET_TEAM((TeamTypes)iI).isAlive())
#endif
				{
					updateRevealedOwner((TeamTypes)iI);
				}
			}

			updateYield();

			// Update happiness if there's a luxury or natural wonder here
			if(isOwned())
			{
				bool bShouldUpdateHappiness = false;

				if(getFeatureType() != NO_FEATURE)
				{
#if defined(MOD_PSEUDO_NATURAL_WONDER)
					if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder(true))
#else
					if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder())
#endif
					{
						bShouldUpdateHappiness = true;
					}
				}

				if(!bShouldUpdateHappiness && getResourceType(getTeam()) != NO_RESOURCE)
				{
					if(GC.getResourceInfo(getResourceType(getTeam()))->getResourceUsage() == RESOURCEUSAGE_LUXURY)
					{
						if(getImprovementType() != NO_IMPROVEMENT)
						{
							bShouldUpdateHappiness = true;
						}
					}
				}

				if(bShouldUpdateHappiness)
				{
					GET_PLAYER(getOwner()).CalculateNetHappiness();
				}
			}

			if(bCheckUnits)
			{
				verifyUnitValidPlot();
			}

			if(GC.getGame().isDebugMode())
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(this);
				GC.GetEngineUserInterface()->UpdateCountryBorder(pDllPlot.get());
			}
		}

		auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(this);
		GC.GetEngineUserInterface()->UpdateCountryBorder(pDllPlot.get());
		GC.GetEngineUserInterface()->setDirty(NationalBorders_DIRTY_BIT, true);
		updateSymbols();
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::ClearCityPurchaseInfo(void)
{
	m_purchaseCity.eOwner = NO_PLAYER;
	m_purchaseCity.iID = -1;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvPlot::GetCityPurchaseOwner(void)
{
	return m_purchaseCity.eOwner;
}

//	--------------------------------------------------------------------------------
int CvPlot::GetCityPurchaseID(void)
{
	return m_purchaseCity.iID;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetCityPurchaseID(int iAcquiringCityID)
{
	m_purchaseCity.iID = iAcquiringCityID;
}

//	--------------------------------------------------------------------------------
/// Is this Plot within a certain range of any of a player's Cities?
bool CvPlot::IsCloseToBorder(PlayerTypes ePlayer) const
{
	if (ePlayer == NO_PLAYER)
		return false;

	//do not use estimated turns here, performance is not good
	int iDistance = GET_PLAYER(ePlayer).GetCityDistanceInPlots(this);
	int iRange = GC.getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT();
	return (iDistance < iRange);
}

//	--------------------------------------------------------------------------------
bool CvPlot::isBlockaded(PlayerTypes ePlayer)
{
	if (isCity())
		return false;

	// An enemy unit on the plot trumps all
	if (IsBlockadeUnit(ePlayer,false))
		return true;

	// An allied unit on the plot secures the plot
	if (IsBlockadeUnit(ePlayer,true))
		return false;

	//ships have additional range
	if (isWater()) 
	{
		// An enemy unit adjacent to the plot blockades it
		for (int iEnemyLoop = 0; iEnemyLoop < NUM_DIRECTION_TYPES; ++iEnemyLoop)
		{
			CvPlot* pEnemyPlot = plotDirection(getX(), getY(), ((DirectionTypes)iEnemyLoop));

			if (pEnemyPlot && pEnemyPlot->isWater()==isWater() && pEnemyPlot->IsBlockadeUnit(ePlayer,false))
				return true;
		}

		// An allied unit adjacent to the plot secures it
		for (int iAlliedLoop = 0; iAlliedLoop < NUM_DIRECTION_TYPES; ++iAlliedLoop)
		{
			CvPlot* pAlliedPlot = plotDirection(getX(), getY(), ((DirectionTypes)iAlliedLoop));

			if (pAlliedPlot && pAlliedPlot->isWater()==isWater() && pAlliedPlot->IsBlockadeUnit(ePlayer,true))
				return false;
		}

		//check ring 2 to N for enemies - rather inefficient
		int iRange = min(5,max(2,GC.getNAVAL_PLOT_BLOCKADE_RANGE()));
		for (int i=RING1_PLOTS; i<RING_PLOTS[iRange]; i++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(this, i);
			if (!pLoopPlot)
				continue;

			if (pLoopPlot->isWater()==isWater() && pLoopPlot->getArea()==getArea() && pLoopPlot->IsBlockadeUnit(ePlayer,false))
			{
				SPathFinderUserData data(NO_PLAYER,PT_GENERIC_SAME_AREA,-1,iRange);
				SPath path = GC.GetStepFinder().GetPath(pLoopPlot, this, data);
				if (!!path && path.length()<=iRange)
					return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isFlatlands() const
{
	return (getPlotType() == PLOT_LAND);
}

//	--------------------------------------------------------------------------------
void CvPlot::setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics, bool bEraseUnitsIfWater)
{
	CvArea* pNewArea;
	CvArea* pCurrArea;
	CvArea* pLastArea;
	CvPlot* pLoopPlot;
	bool bWasWater;
	bool bRecalculateAreas;
	int iAreaCount;
	int iI;
	
#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue <= NO_PLOT || eNewValue >= NUM_PLOT_TYPES) return;
#endif

	if(getPlotType() != eNewValue)
	{
		if((getPlotType() == PLOT_OCEAN) || (eNewValue == PLOT_OCEAN))
		{
			erase(bEraseUnitsIfWater);
		}

		bWasWater = isWater();

		updateSeeFromSight(false,bRecalculate);

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_PLOT, m_iX, m_iY, 0, eNewValue, m_ePlotType, -1, -1);
		}
#endif

		m_ePlotType = eNewValue;

		updateYield();
#if defined(MOD_BALANCE_CORE)
		updateImpassable();
#endif

		updateSeeFromSight(true,bRecalculate);

		if((getTerrainType() == NO_TERRAIN) || (GC.getTerrainInfo(getTerrainType())->isWater() != isWater()))
		{
			if(isWater())
			{
				if(isAdjacentToLand(false))
				{
					setTerrainType(((TerrainTypes)(GC.getSHALLOW_WATER_TERRAIN())), bRecalculate, bRebuildGraphics);
					m_bIsAdjacentToLand = true;
				}
				else
				{
					setTerrainType(((TerrainTypes)(GC.getDEEP_WATER_TERRAIN())), bRecalculate, bRebuildGraphics);
					m_bIsAdjacentToLand = false;
				}
			}
			else
			{
				setTerrainType(((TerrainTypes)(GC.getLAND_TERRAIN())), bRecalculate, bRebuildGraphics);
			}
		}

		if(bWasWater != isWater())
		{
			if(bRecalculate)
			{
				for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

					if(pLoopPlot != NULL)
					{
						if(pLoopPlot->isWater())
						{
							if(pLoopPlot->isAdjacentToLand(false))
							{
								pLoopPlot->setTerrainType(((TerrainTypes)(GC.getSHALLOW_WATER_TERRAIN())), bRecalculate, bRebuildGraphics);
								m_bIsAdjacentToLand = true;
							}
							else
							{
								pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDEEP_WATER_TERRAIN())), bRecalculate, bRebuildGraphics);
								m_bIsAdjacentToLand = false;
							}
						}
					}
				}
			}

			for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if(pLoopPlot != NULL)
				{
					pLoopPlot->updateYield();
				}
			}


			for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
			{
				pLoopPlot = iterateRingPlots(getX(), getY(), iI);

				if(pLoopPlot != NULL)
				{
					pLoopPlot->updatePotentialCityWork();
				}
			}

			GC.getMap().changeLandPlots((isWater()) ? -1 : 1);

			if(getResourceType() != NO_RESOURCE)
			{
				GC.getMap().changeNumResourcesOnLand(getResourceType(), ((isWater()) ? -1 : 1));
			}

			if(isOwned())
			{
				GET_PLAYER(getOwner()).changeTotalLand((isWater()) ? -1 : 1);
				GET_TEAM(getTeam()).changeTotalLand((isWater()) ? -1 : 1);
			}

			if(bRecalculate)
			{
				pNewArea = NULL;
				bRecalculateAreas = false;

				if(isWater())
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if(pLoopPlot != NULL && pLoopPlot->getArea()!=-1)
						{
							if(pLoopPlot->area()->isWater())
							{
								if(pNewArea == NULL)
								{
									pNewArea = pLoopPlot->area();
								}
								else if(pNewArea != pLoopPlot->area())
								{
									bRecalculateAreas = true;
									break;
								}
							}
						}
					}
				}
				else
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if(pLoopPlot != NULL && pLoopPlot->getArea()!=-1)
						{
							if(!(pLoopPlot->area()->isWater()))
							{
								if(pNewArea == NULL)
								{
									pNewArea = pLoopPlot->area();
								}
								else if(pNewArea != pLoopPlot->area())
								{
									bRecalculateAreas = true;
									break;
								}
							}
						}
					}
				}

				if(!bRecalculateAreas)
				{
					pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)(NUM_DIRECTION_TYPES - 1)));

					if(pLoopPlot != NULL)
					{
						pLastArea = pLoopPlot->area();
					}
					else
					{
						pLastArea = NULL;
					}

					iAreaCount = 0;

					for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if(pLoopPlot != NULL)
						{
							pCurrArea = pLoopPlot->area();
						}
						else
						{
							pCurrArea = NULL;
						}

						if(pCurrArea != pLastArea)
						{
							iAreaCount++;
						}

						pLastArea = pCurrArea;
					}

					if(iAreaCount > 2)
					{
						bRecalculateAreas = true;
					}
				}

				if(bRecalculateAreas)
				{
					GC.getMap().recalculateAreas();
				}
				else
				{
					setArea(-1);

					if((area() != NULL) && (area()->getNumTiles() == 1))
					{
						GC.getMap().deleteArea(getArea());
					}

					if(pNewArea == NULL)
					{
						pNewArea = GC.getMap().addArea();
						pNewArea->init(pNewArea->GetID(), isWater());
					}

					setArea(pNewArea->GetID());
				}
			}
		}

		if(bRebuildGraphics && GC.IsGraphicsInitialized())
		{
			//Update terrain graphical
			setLayoutDirty(true);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::setTerrainType(TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	bool bUpdateSight;

#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue <= NO_TERRAIN || eNewValue >= NUM_TERRAIN_TYPES) return;
#endif

	if(getTerrainType() != eNewValue)
	{
		if((getTerrainType() != NO_TERRAIN) &&
		        (eNewValue != NO_TERRAIN) &&
		        ((GC.getTerrainInfo(getTerrainType())->getSeeFromLevel() != GC.getTerrainInfo(eNewValue)->getSeeFromLevel()) ||
		         (GC.getTerrainInfo(getTerrainType())->getSeeThroughLevel() != GC.getTerrainInfo(eNewValue)->getSeeThroughLevel())))
		{
			bUpdateSight = true;
		}
		else
		{
			bUpdateSight = false;
		}

		if(bUpdateSight)
		{
			updateSeeFromSight(false,bRecalculate);
		}

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_TERRAIN, m_iX, m_iY, 0, eNewValue, m_eTerrainType, -1, -1);
		}
#endif

		m_eTerrainType = eNewValue;

		updateYield();
		updateImpassable();

#if defined(MOD_BALANCE_CORE)
		CvCity* pOwningCity = getOwningCity();

		if(pOwningCity != NULL)
		{
			for(int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
					break;

				pOwningCity->UpdateYieldPerXTerrain((YieldTypes)iI, getTerrainType());
				pOwningCity->UpdateYieldPerXTerrainFromReligion((YieldTypes)iI, getTerrainType());
			}
		}
#endif

		if(bUpdateSight)
		{
			updateSeeFromSight(true,bRecalculate);
		}

		const bool bTypeIsWater = GC.getTerrainInfo(getTerrainType())->isWater();
		if(bTypeIsWater != isWater())
		{
			setPlotType((bTypeIsWater)? PLOT_OCEAN : PLOT_LAND, bRecalculate, bRebuildGraphics);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::setFeatureType(FeatureTypes eNewValue)
{
	FeatureTypes eOldFeature;
	bool bUpdateSight;

#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue < NO_FEATURE) return;
	if (eNewValue > NO_FEATURE && GC.getFeatureInfo(eNewValue) == NULL) return;
#endif

	eOldFeature = getFeatureType();

	if(eOldFeature != eNewValue)
	{
		if((eOldFeature == NO_FEATURE) ||
		        (eNewValue == NO_FEATURE) ||
		        (GC.getFeatureInfo(eOldFeature)->getSeeThroughChange() != GC.getFeatureInfo(eNewValue)->getSeeThroughChange()))
		{
			bUpdateSight = true;
		}
		else
		{
			bUpdateSight = false;
		}

		if(bUpdateSight)
		{
			updateSeeFromSight(false,true);
		}

		auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(this));
		gDLL->GameplayFeatureChanged(pDllPlot.get(), eNewValue);

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_FEATURE, m_iX, m_iY, 0, eNewValue, m_eFeatureType.get(), -1, -1);
		}
#endif

		m_eFeatureType = eNewValue;
#if defined(MOD_BALANCE_CORE)
		CvCity* pOwningCity = getOwningCity();
		if(pOwningCity != NULL)
		{
			//City already working this plot? Adjust features being worked as needed.
			if(pOwningCity->GetCityCitizens()->IsWorkingPlot(this))
			{
				//New feature over old? Remove old, add new.
				if(eOldFeature != NO_FEATURE)
				{
					pOwningCity->ChangeNumFeatureWorked(eOldFeature, -1);
					//We added new improvement (wasn't deleted) - add here.
					if(eNewValue != NO_FEATURE)
					{
						pOwningCity->ChangeNumFeatureWorked(eNewValue, 1);
					}
					else
					{
						if(getTerrainType() != NO_TERRAIN)
						{
							pOwningCity->ChangeNumFeaturelessTerrainWorked(getTerrainType(), 1);
						}
					}
				}
				//New improvement over nothing? Add it in.
				else if(eNewValue != NO_FEATURE)
				{
					pOwningCity->ChangeNumFeatureWorked(eNewValue, 1);
				}
			}
		}
#endif
		updateYield();
		updateImpassable();
#if defined(MOD_BALANCE_CORE)
		if(pOwningCity != NULL)
		{
			for(int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
					break;

				pOwningCity->UpdateYieldPerXFeature((YieldTypes)iI, getFeatureType());
				pOwningCity->UpdateYieldPerXUnimprovedFeature((YieldTypes)iI, getFeatureType());
			}
		}

		if (getOwner()!=NO_PLAYER)
		{
			GET_PLAYER(getOwner()).countCityFeatures(eOldFeature, true);
			if(eNewValue != NO_FEATURE)
				GET_PLAYER(getOwner()).countCityFeatures(eNewValue, true);
		}
#endif

		if(bUpdateSight)
		{
			updateSeeFromSight(true,true);
		}

		if(eNewValue != NO_FEATURE)
		{
			// Now a Natural Wonder here
#if defined(MOD_PSEUDO_NATURAL_WONDER)
			if((eOldFeature == NO_FEATURE || !GC.getFeatureInfo(eOldFeature)->IsNaturalWonder(true)) && GC.getFeatureInfo(eNewValue)->IsNaturalWonder(true))
#else
			if((eOldFeature == NO_FEATURE || !GC.getFeatureInfo(eOldFeature)->IsNaturalWonder()) && GC.getFeatureInfo(eNewValue)->IsNaturalWonder())
#endif
			{
				GC.getMap().ChangeNumNaturalWonders(1);
				GC.getMap().getArea(getArea())->ChangeNumNaturalWonders(1);
			}
		}
		if(eOldFeature != NO_FEATURE)
		{
			// Was a Natural Wonder, isn't any more
#if defined(MOD_PSEUDO_NATURAL_WONDER)
			if(GC.getFeatureInfo(eOldFeature)->IsNaturalWonder(true) && (eNewValue == NO_FEATURE || !GC.getFeatureInfo(eNewValue)->IsNaturalWonder(true)))
#else
			if(GC.getFeatureInfo(eOldFeature)->IsNaturalWonder() && (eNewValue == NO_FEATURE || !GC.getFeatureInfo(eNewValue)->IsNaturalWonder()))
#endif
			{
				GC.getMap().ChangeNumNaturalWonders(-1);
				GC.getMap().getArea(getArea())->ChangeNumNaturalWonders(-1);
			}
		}

		if(getFeatureType() == NO_FEATURE)
		{
			if(getImprovementType() != NO_IMPROVEMENT)
			{
				if(GC.getImprovementInfo(getImprovementType())->IsRequiresFeature())
				{
					setImprovementType(NO_IMPROVEMENT);
				}
			}
		}

#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		if (MOD_EVENTS_TILE_IMPROVEMENTS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileFeatureChanged, getX(), getY(), getOwner(), eOldFeature, eNewValue);
		}
#endif
	}
}

//	--------------------------------------------------------------------------------
/// Does this plot have a natural wonder?
bool CvPlot::IsNaturalWonder(bool orPseudoNatural) const
{
	FeatureTypes eFeature = getFeatureType();
	if(eFeature == NO_FEATURE)
		return false;

#if defined(MOD_PSEUDO_NATURAL_WONDER)
	return GC.getFeatureInfo(eFeature)->IsNaturalWonder() || (orPseudoNatural && GC.getFeatureInfo(eFeature)->IsPseudoNaturalWonder());
#else
	orPseudoNatural; //ignore this
	return GC.getFeatureInfo(eFeature)->IsNaturalWonder();
#endif
}

//	--------------------------------------------------------------------------------
ResourceTypes CvPlot::getResourceType(TeamTypes eTeam) const
{
	if(eTeam != NO_TEAM)
	{
		if(m_eResourceType != NO_RESOURCE)
		{
#if defined(MOD_BALANCE_CORE_BARBARIAN_THEFT)
			if (MOD_BALANCE_CORE_BARBARIAN_THEFT && (getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT()))
				return NO_RESOURCE;
#endif
			CvGame& Game = GC.getGame();
			bool bDebug = Game.isDebugMode() || GET_TEAM(eTeam).isObserver();

			int iPolicyReveal = GC.getResourceInfo((ResourceTypes)m_eResourceType)->getPolicyReveal();
			if (!bDebug && iPolicyReveal != NO_POLICY)
			{
				if (!GET_TEAM(eTeam).HavePolicyInTeam((PolicyTypes)iPolicyReveal))
				{
					return NO_RESOURCE;
				}
			}

			if(!bDebug && !GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)(GC.getResourceInfo((ResourceTypes)m_eResourceType)->getTechReveal())) &&
			        !GET_TEAM(eTeam).isForceRevealedResource((ResourceTypes)m_eResourceType) &&
			        !IsResourceForceReveal(eTeam))
			{
				return NO_RESOURCE;
			}
		}
	}

	return (ResourceTypes)m_eResourceType;
}


//	--------------------------------------------------------------------------------
ResourceTypes CvPlot::getNonObsoleteResourceType(TeamTypes eTeam) const
{
	ResourceTypes eResource;

	CvAssert(eTeam != NO_TEAM);

	eResource = getResourceType(eTeam);

	if(eResource != NO_RESOURCE)
	{
		if((GC.getResourceInfo(eResource)->getTechObsolete() != NO_TECH) && GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)(GC.getResourceInfo(eResource)->getTechObsolete())))
		{
			return NO_RESOURCE;
		}
	}

	return eResource;
}


//	--------------------------------------------------------------------------------
void CvPlot::setResourceType(ResourceTypes eNewValue, int iResourceNum, bool bForMinorCivPlot)
{
#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue < NO_RESOURCE) return;
	if (eNewValue > NO_RESOURCE && GC.getResourceInfo(eNewValue) == NULL) return;
#endif

	if(m_eResourceType != eNewValue)
	{
		if (eNewValue != NO_RESOURCE)
		{
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eNewValue);
			if (pkResourceInfo)
			{
				if (pkResourceInfo->isOnlyMinorCivs())
				{
					if (!bForMinorCivPlot)
					{
						CvAssertMsg(false, "Tried to place a city-state unique luxury when not allowed.  PLEASE send Anton your save game, settings, and version!");
						return;
					}
				}
			}
		}

		if(m_eResourceType != NO_RESOURCE)
		{
			if(area())
			{
				area()->changeNumResources((ResourceTypes)m_eResourceType, -1);
			}
			GC.getMap().changeNumResources((ResourceTypes)m_eResourceType, -1);

			if(!isWater())
			{
				GC.getMap().changeNumResourcesOnLand((ResourceTypes)m_eResourceType, -1);
			}

		}

		m_eResourceType = eNewValue; // !!! Here is where we actually change the value

		setNumResource(iResourceNum);

		if(m_eResourceType != NO_RESOURCE)
		{
			if(area())
			{
				area()->changeNumResources((ResourceTypes)m_eResourceType, 1);
			}
			GC.getMap().changeNumResources((ResourceTypes)m_eResourceType, 1);

			if(!isWater())
			{
				GC.getMap().changeNumResourcesOnLand((ResourceTypes)m_eResourceType, 1);
			}
			DoFindCityToLinkResourceTo();
		}

		updateYield();

		if(GC.IsGraphicsInitialized())
		{
			setLayoutDirty(true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlot::getNumResource() const
{
	return m_iResourceNum;
}

//	--------------------------------------------------------------------------------
void CvPlot::setNumResource(int iNum)
{
	m_iResourceNum = iNum;
	CvAssert(getNumResource() >= 0);
	FAssertMsg(getResourceType() == NO_RESOURCE || m_iResourceNum > 0, "If a plot contains a Resource it should always have a quantity of at least 1.");
}

//	--------------------------------------------------------------------------------
void CvPlot::changeNumResource(int iChange)
{
	setNumResource(getNumResource() + iChange);
	CvAssert(getNumResource() >= 0);
}

//	--------------------------------------------------------------------------------
int CvPlot::getNumResourceForPlayer(PlayerTypes ePlayer) const
{
	int iRtnValue = m_iResourceNum;

	ResourceTypes eResource = getResourceType(getTeam());
	if(eResource != NO_RESOURCE)
	{
		CvResourceInfo *pkResource = GC.getResourceInfo(eResource);
		if (pkResource)
		{
			if (GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) pkResource->getTechReveal()) && 
				(pkResource->getPolicyReveal() == NO_POLICY || GET_PLAYER(ePlayer).GetPlayerPolicies()->HasPolicy((PolicyTypes)pkResource->getPolicyReveal())))
			{
				if(pkResource->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
				{
					int iQuantityMod = GET_PLAYER(ePlayer).GetPlayerTraits()->GetStrategicResourceQuantityModifier(getTerrainType());
					iRtnValue *= 100 + iQuantityMod;
					iRtnValue /= 100;
				}

				else if(pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					CvCity* pCity = getOwningCity();
					if(pCity)
					{
						if(pCity->IsExtraLuxuryResources())
						{
							iRtnValue++;
						}
					}
				}

				if(GET_PLAYER(ePlayer).GetPlayerTraits()->GetResourceQuantityModifier(eResource) > 0)
				{
					int iQuantityMod = GET_PLAYER(ePlayer).GetPlayerTraits()->GetResourceQuantityModifier(eResource);
					iRtnValue *= 100 + iQuantityMod;
					iRtnValue /= 100;
				}
			}
		}
	}

	return iRtnValue;
}

#if defined(MOD_GLOBAL_VENICE_KEEPS_RESOURCES)
//	--------------------------------------------------------------------------------
// Lifted from CvMinorCivAI::DoRemoveStartingResources()
void CvPlot::removeMinorResources(bool bVenice)
{
	bool bRemoveUniqueLuxury = false;

	if (GC.getMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED() == 1)
		bRemoveUniqueLuxury = true;

	if (MOD_BALANCE_CORE)
		bRemoveUniqueLuxury = false;
		
	if (bVenice)
		bRemoveUniqueLuxury = false;

	if (bRemoveUniqueLuxury)
	{
		ResourceTypes eOldResource = getResourceType();
		if (eOldResource != NO_RESOURCE)
		{
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eOldResource);
			if (pkResourceInfo && pkResourceInfo->isOnlyMinorCivs())
			{
				setResourceType(NO_RESOURCE, 0, true);
			}
		}
	}
}
#endif

//	--------------------------------------------------------------------------------
ImprovementTypes CvPlot::getImprovementType() const
{
	return (ImprovementTypes)m_eImprovementType;
}

//	--------------------------------------------------------------------------------
ImprovementTypes CvPlot::getImprovementTypeNeededToImproveResource(PlayerTypes ePlayer, bool bTestPlotOwner, bool bNonSpecialOnly)
{
	CvAssertMsg(ePlayer == NO_PLAYER || ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer == NO_PLAYER || ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	ResourceTypes eResource = NO_RESOURCE;
	if(ePlayer != NO_PLAYER)
	{
		eResource = getResourceType(GET_PLAYER(ePlayer).getTeam());
	}
	else
	{
		eResource = getResourceType();
	}

	if(eResource == NO_RESOURCE)
	{
		return NO_IMPROVEMENT;
	}

	if (IsResourceLinkedCityActive() && getImprovementType() != NO_IMPROVEMENT)
		return getImprovementType();		

	ImprovementTypes eImprovementNeeded = NO_IMPROVEMENT;

	// see if we can improve the resource
	for (int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);
		if (pBuildInfo == NULL)
			continue;

		if (!canBuild(eBuild, ePlayer, false /*bTestVisible*/, bTestPlotOwner))
			continue;

		if (ePlayer != NO_PLAYER)
		{
			if (!GET_PLAYER(ePlayer).canBuild(this, eBuild, false /*bTestEra*/, false /*bTestVisible*/, false /*bTestGold*/, bTestPlotOwner))
			{
				continue;
			}
		}

		ImprovementTypes eImprovement = (ImprovementTypes)pBuildInfo->getImprovement();
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (pImprovementInfo == NULL)
			continue;

		if (bNonSpecialOnly && !pImprovementInfo->IsImprovementResourceTrade(eResource))
			continue;
		else if (pImprovementInfo->IsExpandedImprovementResourceTrade(eResource, true))
			continue;

		if(pImprovementInfo->IsWater() != isWater())
			continue;

		eImprovementNeeded = eImprovement;
	}

	return eImprovementNeeded;
}


//	--------------------------------------------------------------------------------
void CvPlot::setImprovementType(ImprovementTypes eNewValue, PlayerTypes eBuilder)
{
	int iI;
	ImprovementTypes eOldImprovement = getImprovementType();
	bool bGiftFromMajor = false;

#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue < NO_IMPROVEMENT) return;
	if (eNewValue > NO_IMPROVEMENT && GC.getImprovementInfo(eNewValue) == NULL) return;
#endif

	if (eBuilder != NO_PLAYER)
	{
		if (eNewValue != NO_IMPROVEMENT && getOwner() != eBuilder && !GET_PLAYER(eBuilder).isMinorCiv())
		{
			bGiftFromMajor = true;
			if (GC.getImprovementInfo(eNewValue)->IsCreatedByGreatPerson())
				bGiftFromMajor = false;
		}
	}
	bool bIgnoreResourceTechPrereq = bGiftFromMajor; // If it is a gift from a major civ, our tech limitations do not apply

	if(eOldImprovement != eNewValue)
	{
#if defined(MOD_BALANCE_CORE)
		CvCity* pOwningCity = getOwningCity();
		if(pOwningCity != NULL)
		{
			//City already working this plot? Adjust improvements being worked as needed.
			if(pOwningCity->GetCityCitizens()->IsWorkingPlot(this))
			{
				//New improvement over old? Remove old, add new.
				if(eOldImprovement != NO_IMPROVEMENT)
				{
					pOwningCity->ChangeNumImprovementWorked(eOldImprovement, -1);
					//We added new improvement (wasn't deleted) - add here.
					if(eNewValue != NO_IMPROVEMENT)
					{
						pOwningCity->ChangeNumImprovementWorked(eNewValue, 1);
					}
				}
				//New improvement over nothing? Add it in.
				else if(eNewValue != NO_IMPROVEMENT)
				{
					pOwningCity->ChangeNumImprovementWorked(eNewValue, 1);
				}
			}
			pOwningCity->GetCityCitizens()->SetDirty(true);
		}
#endif
		PlayerTypes owningPlayerID = getOwner();
		if(eOldImprovement != NO_IMPROVEMENT)
		{
#if defined(MOD_BALANCE_CORE)
			if(IsImprovementPillaged())
			{
				SetImprovementPillaged(false, false);
			}
#endif
			CvImprovementEntry& oldImprovementEntry = *GC.getImprovementInfo(eOldImprovement);

#if defined(MOD_BUGFIX_MINOR)
			DomainTypes eTradeRouteDomain = NO_DOMAIN;
			if (oldImprovementEntry.IsAllowsWalkWater()) {
				eTradeRouteDomain = DOMAIN_LAND;
#if defined(MOD_GLOBAL_PASSABLE_FORTS)
			}
			else if (oldImprovementEntry.IsMakesPassable()) {
				eTradeRouteDomain = DOMAIN_SEA;
#endif
			}
#endif


#if defined(MOD_BUGFIX_MINOR)
			if (eTradeRouteDomain != NO_DOMAIN) {
				// Take away any trade routes of this domain that pass through the plot
				CvGameTrade* pTrade = GC.getGame().GetGameTrade();
				int iPlotX = getX();
				int iPlotY = getY();

				for (uint uiConnection = 0; uiConnection < pTrade->GetNumTradeConnections(); uiConnection++)
				{
					CvGameTrade* pTrade = GC.getGame().GetGameTrade();
					for (uint uiConnection = 0; uiConnection < pTrade->GetNumTradeConnections(); uiConnection++)
					{
						const TradeConnection* pConnection = &(pTrade->GetTradeConnection(uiConnection));
						if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
						{
							continue;
						}
						for (uint ui = 0; ui < pConnection->m_aPlotList.size(); ui++)
						{
							if (pConnection->m_eDomain == eTradeRouteDomain) {
								TradeConnectionPlotList aPlotList = pConnection->m_aPlotList;

								for (uint uiPlotIndex = 0; uiPlotIndex < aPlotList.size(); uiPlotIndex++) {
									if (aPlotList[uiPlotIndex].m_iX == iPlotX && aPlotList[uiPlotIndex].m_iY == iPlotY) 
									{
										CUSTOMLOG("Cancelling trade route for domain %i in plot (%i, %i) as enabling improvement destroyed", eTradeRouteDomain, iPlotX, iPlotY);
										pTrade->EndTradeRoute(pConnection->m_iID);
										break;
									}
								}
							}
						}
					}
				}
			}
#endif

			// If this improvement can add culture to nearby improvements, update them as well
			if(area())
			{
				area()->changeNumImprovements(eOldImprovement, -1);
			}
			// Someone owns this plot
			if(owningPlayerID != NO_PLAYER)
			{
				CvPlayer& owningPlayer = GET_PLAYER(owningPlayerID);
				owningPlayer.changeImprovementCount(eOldImprovement, -1);

				// Maintenance change!
				if(MustPayMaintenanceHere(owningPlayerID))
				{
					GET_PLAYER(owningPlayerID).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getImprovementInfo(getImprovementType())->GetGoldMaintenance());
				}

				// Siphon resource changes
				PlayerTypes eOldBuilder = GetPlayerThatBuiltImprovement();
				if(oldImprovementEntry.GetLuxuryCopiesSiphonedFromMinor() > 0 && eOldBuilder != NO_PLAYER)
				{
					if (owningPlayer.isMinorCiv())
					{
						GET_PLAYER(eOldBuilder).changeSiphonLuxuryCount(owningPlayerID, -1 * oldImprovementEntry.GetLuxuryCopiesSiphonedFromMinor());
#if defined(MOD_BALANCE_CORE)
						GET_PLAYER(owningPlayerID).GetMinorCivAI()->SetSiphoned(eOldBuilder, false);
#endif
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(oldImprovementEntry.GetGrantsVision() > 0 && eOldBuilder != NO_PLAYER)
				{
					int iPlotVisRange = oldImprovementEntry.GetGrantsVision();		
					changeAdjacentSight(GET_PLAYER(GetPlayerThatBuiltImprovement()).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
				}
				if (oldImprovementEntry.GetUnitPlotExperience() > 0)
				{
					ChangeUnitPlotExperience(-1 * oldImprovementEntry.GetUnitPlotExperience());
				}
				if (oldImprovementEntry.GetGAUnitPlotExperience() > 0)
				{
					ChangeUnitPlotGAExperience(-1 * oldImprovementEntry.GetGAUnitPlotExperience());
				}
				if (oldImprovementEntry.GetMovesChange() > 0)
				{
					ChangePlotMovesChange(-1 * oldImprovementEntry.GetMovesChange());
				}
#endif
#if defined(MOD_BALANCE_CORE)
				//Resource from improvement - change ownership if needed.
				ResourceTypes eResourceFromImprovement = (ResourceTypes)oldImprovementEntry.GetResourceFromImprovement();
				int iQuantity = oldImprovementEntry.GetResourceQuantityFromImprovement();
				if(iQuantity <= 0)
				{
					iQuantity = 1;
				}
				if(eResourceFromImprovement != NO_RESOURCE && (getResourceType() != NO_RESOURCE && getResourceType() != eResourceFromImprovement))
				{
					setResourceType(eResourceFromImprovement, iQuantity);
				}
#endif

				// Update the amount of a Resource used up by the previous Improvement that is being removed
				int iNumResourceInfos= GC.getNumResourceInfos();
				for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
				{
					if(oldImprovementEntry.GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						owningPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, -oldImprovementEntry.GetResourceQuantityRequirement(iResourceLoop));
					}
				}
#if defined(MOD_DIPLOMACY_CITYSTATES)
				// Embassy extra vote in WC mod
				if(MOD_DIPLOMACY_CITYSTATES && oldImprovementEntry.GetCityStateExtraVote() > 0 && eOldBuilder != NO_PLAYER)
				{
					if (owningPlayer.isMinorCiv())
					{
						GET_PLAYER(eOldBuilder).ChangeImprovementLeagueVotes(oldImprovementEntry.GetCityStateExtraVote() * -1);
					}
				}
#endif
			}
#if defined(MOD_BALANCE_CORE)
			else
			{
				PlayerTypes eOldBuilder = GetPlayerThatBuiltImprovement();
				if(oldImprovementEntry.GetGrantsVision() > 0 && eOldBuilder != NO_PLAYER)
				{
					int iPlotVisRange = oldImprovementEntry.GetGrantsVision();
					changeAdjacentSight(GET_PLAYER(GetPlayerThatBuiltImprovement()).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION, false);
				}
			}
#endif
			// Someone had built something here in an unowned plot, remove effects of the old improvement
			if(GetPlayerResponsibleForImprovement() != NO_PLAYER)
			{
				// Maintenance change!
				if(MustPayMaintenanceHere(GetPlayerResponsibleForImprovement()))
				{
					GET_PLAYER(GetPlayerResponsibleForImprovement()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getImprovementInfo(getImprovementType())->GetGoldMaintenance());
				}

				SetPlayerResponsibleForImprovement(NO_PLAYER);
			}
		}

		m_eImprovementType = eNewValue;
#if defined(MOD_GLOBAL_STACKING_RULES)
		calculateAdditionalUnitsFromImprovement();
#endif

		if (eOldImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry& oldImprovementEntry = *GC.getImprovementInfo(eOldImprovement);
			for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
					break;

				if (oldImprovementEntry.GetYieldAdjacentSameType((YieldTypes)iI) > 0 || oldImprovementEntry.GetYieldAdjacentTwoSameType((YieldTypes)iI) > 0)
				{
					for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));
						if (pAdjacentPlot && pAdjacentPlot->getImprovementType() == eOldImprovement)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
			if (oldImprovementEntry.IsEmbassy())
				SetImprovementEmbassy(false);
		}

		if(getImprovementType() == NO_IMPROVEMENT)
		{
			setImprovementDuration(0);
		}

		// Reset who cleared a Barb camp here last (if we're putting a new one down)
		if(eNewValue == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			SetPlayerThatClearedBarbCampHere(NO_PLAYER);
			
			// Alert the barbarian spawning code to this new camp
			CvBarbarians::DoCampActivationNotice(this);

#if defined(MOD_EVENTS_BARBARIANS)
			if (MOD_EVENTS_BARBARIANS) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansCampFounded, getX(), getY());
			}
#endif
		}

		setUpgradeProgress(0);

		// make sure this plot is not disabled
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		SetImprovementPillaged(false, false);
#else
		SetImprovementPillaged(false);
#endif

		for(iI = 0; iI < MAX_TEAMS; ++iI)
		{
#ifdef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
			if (GET_TEAM((TeamTypes)iI).isObserver() || GET_TEAM((TeamTypes)iI).isAlive())
#else
			if(GET_TEAM((TeamTypes)iI).isAlive())
#endif
			{
				if(isVisible((TeamTypes)iI))
				{
					setRevealedImprovementType((TeamTypes)iI, eNewValue);
				}
			}
		}

		if(m_eImprovementType != NO_IMPROVEMENT)
		{
			CvImprovementEntry& newImprovementEntry = *GC.getImprovementInfo(eNewValue);

#if defined(MOD_BALANCE_CORE)
			ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
			ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getHIDDEN_ARTIFACT_RESOURCE());
			if (newImprovementEntry.IsPermanent() || newImprovementEntry.IsCreatedByGreatPerson())
			{
				if (getOwner() != NO_PLAYER && (getResourceType(GET_PLAYER(getOwner()).getTeam()) == eArtifactResourceType || getResourceType(GET_PLAYER(getOwner()).getTeam()) == eHiddenArtifactResourceType))
				{
					if (MOD_BALANCE_CORE_ARCHAEOLOGY_FROM_GP && GetArchaeologicalRecord().m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
					{
						CvPlayer& kPlayer = GET_PLAYER(getOwner());
						kPlayer.SetNumArchaeologyChoices(kPlayer.GetNumArchaeologyChoices() + 1);
						kPlayer.GetCulture()->AddDigCompletePlot(this);

						if (kPlayer.isHuman())
						{
							CvNotifications* pNotifications;
							Localization::String locString;
							Localization::String locSummary;
							pNotifications = kPlayer.GetNotifications();
							if (pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHOOSE_ARCHAEOLOGY");
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_ARCHAEOLOGY");
								pNotifications->Add(NOTIFICATION_CHOOSE_ARCHAEOLOGY, strBuffer, strSummary, getX(), getY(), kPlayer.GetID());
								CancelActivePlayerEndTurn();
							}

#if !defined(NO_ACHIEVEMENTS)
							// Raiders of the Lost Ark achievement
							const char* szCivKey = kPlayer.getCivilizationTypeKey();
							if (getOwner() != NO_PLAYER && !GC.getGame().isNetworkMultiPlayer() && strcmp(szCivKey, "CIVILIZATION_AMERICA") == 0)
							{
								CvPlayer &kPlotOwner = GET_PLAYER(getOwner());
								szCivKey = kPlotOwner.getCivilizationTypeKey();
								if (strcmp(szCivKey, "CIVILIZATION_EGYPT") == 0)
								{
									for (int i = 0; i < MAX_MAJOR_CIVS; i++)
									{
										CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)i);
										if (kLoopPlayer.GetID() != NO_PLAYER && kLoopPlayer.isAlive())
										{
											szCivKey = kLoopPlayer.getCivilizationTypeKey();
											if (strcmp(szCivKey, "CIVILIZATION_GERMANY"))
											{
												CvUnit *pLoopUnit;
												int iUnitLoop;
												for (pLoopUnit = kLoopPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iUnitLoop))
												{
													if (strcmp(pLoopUnit->getUnitInfo().GetType(), "UNIT_ARCHAEOLOGIST") == 0)
													{
														if (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), getX(), getY()) <= 2)
														{
															gDLL->UnlockAchievement(ACHIEVEMENT_XP2_33);
														}
													}
												}
											}
										}
									}
								}
							}
#endif
						}
						else
						{
							ArchaeologyChoiceType eChoice = kPlayer.GetCulture()->GetArchaeologyChoice(this);
							kPlayer.GetCulture()->DoArchaeologyChoice(eChoice);
						}
					}
					else
					{
						setResourceType(NO_RESOURCE, 0);
						ClearArchaeologicalRecord();
					}
				}
				else if (getResourceType() == eArtifactResourceType || getResourceType() == eHiddenArtifactResourceType)
				{
					setResourceType(NO_RESOURCE, 0);
					ClearArchaeologicalRecord();
				}
			}
			if (newImprovementEntry.GetHappinessOnConstruction() != 0)
			{
				GET_TEAM(GET_PLAYER(eBuilder).getTeam()).ChangeNumLandmarksBuilt(newImprovementEntry.GetHappinessOnConstruction());
				if (getOwner() != NO_PLAYER && getOwner() != eBuilder && GET_PLAYER(getOwner()).isMajorCiv())
				{
					GET_TEAM(GET_PLAYER(getOwner()).getTeam()).ChangeNumLandmarksBuilt(newImprovementEntry.GetHappinessOnConstruction());
				}
			}
#endif
			// If this improvement can add culture to nearby improvements, update them as well
			for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iYield > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
					break;

				if (newImprovementEntry.GetYieldAdjacentSameType((YieldTypes)iYield) > 0 || newImprovementEntry.GetYieldAdjacentTwoSameType((YieldTypes)iYield) > 0)
				{
					for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));
						if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eNewValue)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}

			if(area())
			{
				area()->changeNumImprovements(eNewValue, 1);
			}
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
			if (MOD_IMPROVEMENTS_EXTENSIONS)
			{
				// creates feature
				if (newImprovementEntry.GetCreatedFeature() != NO_FEATURE)
				{
					setFeatureType(newImprovementEntry.GetCreatedFeature());
				}

				// chance to spawn a random resource, should be executed after creating feature
				int iResourceChance = newImprovementEntry.GetRandomResourceChance();
				if (iResourceChance > 0)
				{
					if (getResourceType() == NO_RESOURCE)
					{
						// first roll: can we get a resource on this plot?
						if (GC.getGame().getSmallFakeRandNum(100, GET_PLAYER(getOwner()).GetPseudoRandomSeed() + GC.getGame().getNumCities() + m_iPlotIndex) < iResourceChance)
						{
							// get list of valid resources for the plot
							vector<ResourceTypes> vPossibleResources;
							for (int iI = 0; iI < GC.getNumResourceInfos(); iI++)
							{
								ResourceTypes eResource = (ResourceTypes)iI;
								CvResourceInfo* pResourceInfo = GC.getResourceInfo((ResourceTypes)iI);

								if (eResource != NO_RESOURCE && pResourceInfo)
								{
									if (canHaveResource(eResource, false, true) && GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pResourceInfo->getTechReveal())))
									{
										vPossibleResources.push_back(eResource);
									}
								}
							}

							// second roll: which resource do we get on this plot?
							int iChoice = GC.getGame().getSmallFakeRandNum(vPossibleResources.size(), GET_PLAYER(getOwner()).GetPseudoRandomSeed() + GC.getGame().getNumCities() + m_iPlotIndex);
							ResourceTypes eSelectedResource = vPossibleResources.empty() ? NO_RESOURCE : vPossibleResources[iChoice];

							// now let's add a resource.
							if (eSelectedResource != NO_RESOURCE)
							{
								int iResourceQuantity = GC.getMap().getRandomResourceQuantity(eSelectedResource);
								setResourceType(eSelectedResource, iResourceQuantity); // note we do not need to check resource linking, as it is done in this very function later on
								// notification stuff
								if (getOwner() == GC.getGame().getActivePlayer())
								{
									if (!CvPreGame::loadWBScenario() || GC.getGame().getGameTurn() > 0)
									{
										CvString strBuffer;
										CvResourceInfo* pSelectedResourceInfo = GC.getResourceInfo(eSelectedResource);
										CvAssert(pSelectedResourceInfo);
										NotificationTypes eNotificationType = NO_NOTIFICATION_TYPE;
										strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RESOURCE", pSelectedResourceInfo->GetTextKey());

										CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RESOURCE", pSelectedResourceInfo->GetTextKey());

										switch (pSelectedResourceInfo->getResourceUsage())
										{
										case RESOURCEUSAGE_LUXURY:
											eNotificationType = NOTIFICATION_DISCOVERED_LUXURY_RESOURCE;
											break;
										case RESOURCEUSAGE_STRATEGIC:
											eNotificationType = NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE;
											break;
										case RESOURCEUSAGE_BONUS:
											eNotificationType = NOTIFICATION_DISCOVERED_BONUS_RESOURCE;
											break;
										}

										CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
										if (pNotifications)
										{
											pNotifications->Add(eNotificationType, strBuffer, strSummary, getX(), getY(), eSelectedResource);
										}
									}
								}
							}
						}
					}
				}
			}
#endif
			if(isOwned())
			{
				CvPlayer& owningPlayer = GET_PLAYER(owningPlayerID);
				owningPlayer.changeImprovementCount(eNewValue, 1);

#if !defined(NO_ACHIEVEMENTS)
				//DLC_04 Achievement
				if(owningPlayerID == GC.getGame().getActivePlayer() && strncmp(newImprovementEntry.GetType(), "IMPROVEMENT_MOTTE_BAILEY", 64) == 0)
				{
					//string compares are faster than testing if the mod is activated, so perform this after the compare test.
					if(gDLL->IsModActivated(CIV5_DLC_04_SCENARIO_MODID))
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_BUILD_MOTTE);
					}
				}
				
				// XP2 Achievement
				if (eBuilder != NO_PLAYER && !GC.getGame().isGameMultiPlayer())
				{
					if (GET_PLAYER(eBuilder).isHuman() && GET_PLAYER(eBuilder).isLocalPlayer() && strncmp(newImprovementEntry.GetType(), "IMPROVEMENT_FEITORIA", 64) == 0)
					{
						if (owningPlayer.isMinorCiv())
						{
							PlayerTypes eAlly = owningPlayer.GetMinorCivAI()->GetAlly();
							if (eAlly != NO_PLAYER && eAlly != eBuilder)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_24);
							}
						}
					}
				}
#endif
				//remember this to improve pathfinding performance
				SetImprovementPassable(newImprovementEntry.IsMakesPassable());

				// Maintenance
				if(MustPayMaintenanceHere(owningPlayerID))
				{
					GET_PLAYER(owningPlayerID).GetTreasury()->ChangeBaseImprovementGoldMaintenance(newImprovementEntry.GetGoldMaintenance());
				}

				// Siphon resource changes
				if(newImprovementEntry.GetLuxuryCopiesSiphonedFromMinor() > 0 && eBuilder != NO_PLAYER)
				{
					if (owningPlayer.isMinorCiv())
					{
						GET_PLAYER(eBuilder).changeSiphonLuxuryCount(owningPlayerID, newImprovementEntry.GetLuxuryCopiesSiphonedFromMinor());
#if defined(MOD_BALANCE_CORE)
						GET_PLAYER(owningPlayerID).GetMinorCivAI()->SetSiphoned(eBuilder, true);
#endif
					}
				}
#if defined(MOD_BALANCE_CORE)
				if(newImprovementEntry.GetGrantsVision() > 0 && eBuilder != NO_PLAYER)
				{
					int iPlotVisRange = newImprovementEntry.GetGrantsVision();
					changeAdjacentSight(GET_PLAYER(eBuilder).getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION, false);
				}
				if (newImprovementEntry.GetUnitPlotExperience() > 0)
				{
					ChangeUnitPlotExperience(newImprovementEntry.GetUnitPlotExperience());
				}
				if (newImprovementEntry.GetGAUnitPlotExperience() > 0)
				{
					ChangeUnitPlotGAExperience(newImprovementEntry.GetGAUnitPlotExperience());
				}
				if (newImprovementEntry.GetMovesChange() > 0 && getOwner() == eBuilder)
				{
					ChangePlotMovesChange(newImprovementEntry.GetMovesChange());
				}
				//Resource from improvement - change ownership if needed.
				ResourceTypes eResourceFromImprovement = (ResourceTypes)newImprovementEntry.GetResourceFromImprovement();
				int iQuantity = newImprovementEntry.GetResourceQuantityFromImprovement();
				if(iQuantity <= 0)
				{
					iQuantity = 1;
				}
				if(eResourceFromImprovement != NO_RESOURCE)
				{
					if (pOwningCity != NULL)
						SetResourceLinkedCity(pOwningCity);
					setResourceType(eResourceFromImprovement, iQuantity);
				}
#endif

				// Add Resource Quantity to total
				if(getResourceType() != NO_RESOURCE)
				{
					if(bIgnoreResourceTechPrereq || GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
					{
						if (newImprovementEntry.IsExpandedImprovementResourceTrade(getResourceType()))
						{
							owningPlayer.changeNumResourceTotal(getResourceType(), getNumResourceForPlayer(owningPlayerID));

							// Activate Resource city link?
							if(GetResourceLinkedCity() != NULL)
								SetResourceLinkedCityActive(true);
						}
					}
				}

				ResourceTypes eResource = getResourceType(getTeam());
				if(bIgnoreResourceTechPrereq)
					eResource = getResourceType();

				if(eResource != NO_RESOURCE)
				{
					if(newImprovementEntry.IsExpandedImprovementResourceTrade(eResource))
					{
						if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
						{
							owningPlayer.CalculateNetHappiness();
						}
					}
				}

#if defined(MOD_DIPLOMACY_CITYSTATES)
				// Embassy extra vote in WC mod
				if(MOD_DIPLOMACY_CITYSTATES && newImprovementEntry.GetCityStateExtraVote() > 0 && eBuilder != NO_PLAYER)
				{
					if (owningPlayer.isMinorCiv())
					{
						if (owningPlayer.getImprovementCount(eNewValue) <= 1)
						{
							GET_PLAYER(eBuilder).ChangeImprovementLeagueVotes(newImprovementEntry.GetCityStateExtraVote());
						}
					}
				}
				if (newImprovementEntry.IsEmbassy())
					SetImprovementEmbassy(true);
				else
					SetImprovementEmbassy(false);
#endif
			}
#if defined(MOD_BALANCE_CORE) || defined(MOD_IMPROVEMENTS_EXTENSIONS)
			else if(!isOwned())
			{
#if defined(MOD_BALANCE_CORE)
				if(newImprovementEntry.GetGrantsVision() > 0 && eBuilder != NO_PLAYER)
				{
					int iPlotVisRange = newImprovementEntry.GetGrantsVision();				
					changeAdjacentSight(GET_PLAYER(eBuilder).getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION, false);
				}	
#endif
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
				if (MOD_IMPROVEMENTS_EXTENSIONS && newImprovementEntry.IsNewOwner())
				{
					int iBestCityID = -1;
					int iBestCityDistance = -1;
					int iDistance;
					CvCity* pLoopCity = NULL;
					int iLoop = 0;
					for (pLoopCity = GET_PLAYER(eBuilder).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eBuilder).nextCity(&iLoop))
					{
						CvPlot* pPlot = pLoopCity->plot();
						if (pPlot)
						{
							iDistance = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());
							if (iBestCityDistance == -1 || iDistance < iBestCityDistance)
							{
								iBestCityID = pLoopCity->GetID();
								iBestCityDistance = iDistance;
							}
						}
					}
					setOwner(eBuilder, iBestCityID);
				}
#endif
			}
#endif
		}

		// If we're removing an Improvement that hooked up a resource then we need to take away the bonus
		if(eOldImprovement != NO_IMPROVEMENT && !isCity())
		{
			if(isOwned())
			{
				CvPlayer& owningPlayer = GET_PLAYER(owningPlayerID);
				// Remove Resource Quantity from total
				if(getResourceType() != NO_RESOURCE)
				{
					if(IsImprovedByGiftFromMajor() || // If old improvement was a gift, it ignored our tech limits, so be sure to remove resources properly
						GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
					{
						if (GC.getImprovementInfo(eOldImprovement)->IsExpandedImprovementResourceTrade(getResourceType()))
						{
							owningPlayer.changeNumResourceTotal(getResourceType(), -getNumResourceForPlayer(owningPlayerID));

							// Disconnect resource link
							if(GetResourceLinkedCity() != NULL)
								SetResourceLinkedCityActive(false);
						}
					}
				}

				ResourceTypes eResource = getResourceType(getTeam());

				if(eResource != NO_RESOURCE)
				{
					if(GC.getImprovementInfo(eOldImprovement)->IsExpandedImprovementResourceTrade(eResource))
					{
						if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
						{
							owningPlayer.CalculateNetHappiness();
						}
					}
				}
#if defined(MOD_BALANCE_CORE)
				//Resource from improvement - change ownership if needed.
				ResourceTypes eResourceFromImprovement = (ResourceTypes)GC.getImprovementInfo(eOldImprovement)->GetResourceFromImprovement();
				int iQuantity = (ResourceTypes)GC.getImprovementInfo(eOldImprovement)->GetResourceQuantityFromImprovement();
				if (iQuantity <= 0)
				{
					iQuantity = 1;
				}
				if(eResourceFromImprovement != NO_RESOURCE)
				{
					if(getResourceType() != NO_RESOURCE && getResourceType() == eResourceFromImprovement)
					{
						setResourceType(NO_RESOURCE, 0);
						if (GetResourceLinkedCity() != NULL)
							SetResourceLinkedCityActive(false);
					}
					else
					{
						owningPlayer.changeNumResourceTotal(eResourceFromImprovement, -iQuantity, true);
					}
				}
#endif
			}
		}

		updateYield();
#if defined(MOD_BALANCE_CORE)
		if(eBuilder != NO_PLAYER && eNewValue != NO_IMPROVEMENT && getOwner() == eBuilder)
		{
			CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(eNewValue);
			if(pImprovement2)
			{
				for(int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
				{
					CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT && pAdjacentPlot->getOwner() == eBuilder)
					{	
						bool bUp = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if(pImprovement2->GetAdjacentImprovementYieldChanges(pAdjacentPlot->getImprovementType(), (YieldTypes)iK) > 0)
							{
								bUp = true;								
								break;
							}
						}
						if(bUp)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
		if(eBuilder != NO_PLAYER && eOldImprovement != NO_IMPROVEMENT && getOwner() == eBuilder)
		{
			CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(eOldImprovement);
			if(pImprovement2)
			{
				for(int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
				{
					CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT && pAdjacentPlot->getOwner() == eBuilder)
					{	
						bool bUp = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if(pImprovement2->GetAdjacentImprovementYieldChanges(pAdjacentPlot->getImprovementType(), (YieldTypes)iK) > 0)
							{
								bUp = true;								
								break;
							}
						}
						if(bUp)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
		if(eBuilder != NO_PLAYER && eNewValue != NO_IMPROVEMENT && getOwner() == eBuilder)
		{
			CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(eNewValue);
			if(pImprovement2)
			{
				for(int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
				{
					CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));
					// Do not change!!
					if(pAdjacentPlot != NULL && pAdjacentPlot->getResourceType() != NO_RESOURCE)
					{	
						bool bUp = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if(pImprovement2->GetAdjacentResourceYieldChanges(pAdjacentPlot->getResourceType(), (YieldTypes)iK) > 0)
							{
								bUp = true;								
								break;
							}
						}
						if(bUp)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
		if(eBuilder != NO_PLAYER && eOldImprovement != NO_IMPROVEMENT && getOwner() == eBuilder)
		{
			CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(eOldImprovement);
			if(pImprovement2)
			{
				for(int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
				{
					CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getResourceType() != NO_RESOURCE)
					{	
						bool bUp = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if(pImprovement2->GetAdjacentResourceYieldChanges(pAdjacentPlot->getResourceType(), (YieldTypes)iK) > 0)
							{
								bUp = true;								
								break;
							}
						}
						if(bUp)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
		if(eBuilder != NO_PLAYER && eNewValue != NO_IMPROVEMENT && getOwner() == eBuilder)
		{
			CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(eNewValue);
			if(pImprovement2)
			{
				for(int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
				{
					CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getTerrainType() != NO_TERRAIN && pAdjacentPlot->getOwner() == eBuilder)
					{	
						bool bUp = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if(pImprovement2->GetAdjacentTerrainYieldChanges(pAdjacentPlot->getTerrainType(), (YieldTypes)iK) > 0)
							{
								bUp = true;								
								break;
							}
						}
						if(bUp)
						{
							pAdjacentPlot->updateYield();
						}
					}
					if(pAdjacentPlot != NULL && pAdjacentPlot->getFeatureType() != NO_FEATURE && pAdjacentPlot->getOwner() == eBuilder)
					{	
						bool bUp2 = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if (pImprovement2->GetAdjacentFeatureYieldChanges(pAdjacentPlot->getFeatureType(), (YieldTypes)iK) > 0)
							{
								bUp2 = true;								
								break;
							}
						}
						if(bUp2)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
		if(eBuilder != NO_PLAYER && eOldImprovement != NO_IMPROVEMENT && getOwner() == eBuilder)
		{
			CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(eOldImprovement);
			if(pImprovement2)
			{
				for(int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
				{
					CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));

					if(pAdjacentPlot != NULL && pAdjacentPlot->getTerrainType() != NO_TERRAIN && pAdjacentPlot->getOwner() == eBuilder)
					{	
						bool bUp = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if(pImprovement2->GetAdjacentTerrainYieldChanges(pAdjacentPlot->getTerrainType(), (YieldTypes)iK) > 0)
							{
								bUp = true;								
								break;
							}
						}
						if(bUp)
						{
							pAdjacentPlot->updateYield();
						}
					}
					if(pAdjacentPlot != NULL && pAdjacentPlot->getFeatureType() != NO_PLOT && pAdjacentPlot->getOwner() == eBuilder)
					{	
						bool bUp2 = false;
						for(int iK = 0; iK < NUM_YIELD_TYPES; ++iK)
						{
							//Simplification - errata yields not worth considering.
							if ((YieldTypes)iK > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
								break;

							if (pImprovement2->GetAdjacentFeatureYieldChanges(pAdjacentPlot->getFeatureType(), (YieldTypes)iK) > 0)
							{
								bUp2 = true;								
								break;
							}
						}
						if(bUp2)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
#endif

		// Update the amount of a Resource used up by this Improvement
		if(isOwned() && eNewValue != NO_IMPROVEMENT)
		{
			CvPlayer& owningPlayer = GET_PLAYER(owningPlayerID);
			CvImprovementEntry& newImprovementEntry = *GC.getImprovementInfo(eNewValue);
			int iNumResourceInfos = GC.getNumResourceInfos();
			for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
			{
				if(newImprovementEntry.GetResourceQuantityRequirement(iResourceLoop) > 0)
				{
					owningPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, newImprovementEntry.GetResourceQuantityRequirement(iResourceLoop));
				}
			}
		}

		// Update the most recent builder
		if (GetPlayerThatBuiltImprovement() != eBuilder)
		{
			SetPlayerThatBuiltImprovement(eBuilder);
		}

		SetImprovedByGiftFromMajor(bGiftFromMajor); // Assumes that only one tile improvement can be on this plot at a time

		if(GC.getGame().isDebugMode())
		{
			setLayoutDirty(true);
		}
		
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		if (MOD_EVENTS_TILE_IMPROVEMENTS)
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileImprovementChanged, getX(), getY(), getOwner(), eOldImprovement, eNewValue, IsImprovementPillaged());
		}
#endif
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
		if (MOD_IMPROVEMENTS_EXTENSIONS && eNewValue != NO_IMPROVEMENT)
		{
			// this should be called last
			if (GC.getImprovementInfo(eNewValue)->IsRemovesSelf())
			{
				setImprovementType(NO_IMPROVEMENT);
			}
		}
#endif
	}
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
//	--------------------------------------------------------------------------------
bool CvPlot::IsImprovementEmbassy() const
{
	return m_bImprovementEmbassy;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetImprovementEmbassy(bool bEmbassy)
{
	m_bImprovementEmbassy = bEmbassy;
}
#endif

//	--------------------------------------------------------------------------------
bool CvPlot::IsImprovementPassable() const
{
	return m_bImprovementPassable;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetImprovementPassable(bool bPassable)
{
	m_bImprovementPassable = bPassable;
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsImprovementPillaged() const
{
	return m_bImprovementPillaged;
}

//	--------------------------------------------------------------------------------
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
void CvPlot::SetImprovementPillaged(bool bPillaged, bool bEvents)
#else
void CvPlot::SetImprovementPillaged(bool bPillaged)
#endif
{
	bool bWasPillaged = m_bImprovementPillaged;

	if(bPillaged != bWasPillaged)
	{
		m_bImprovementPillaged = bPillaged;
#if defined(MOD_GLOBAL_STACKING_RULES)
		calculateAdditionalUnitsFromImprovement();
#endif
		if (getOwningCity() != NULL)
		{
			if (bPillaged)
			{
				getOwningCity()->ChangeNumPillagedPlots(1);
			}
			else
			{
				getOwningCity()->ChangeNumPillagedPlots(-1);
			}
		}
		updateYield();

		// Quantified Resource changes
		if(getResourceType() != NO_RESOURCE && getImprovementType() != NO_IMPROVEMENT)
		{
			if(getTeam() != NO_TEAM)
			{
				if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
				{
					if(GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType()))
					{
						if(bPillaged)
						{
							GET_PLAYER(getOwner()).changeNumResourceTotal(getResourceType(), -getNumResourceForPlayer(getOwner()));

							// Disconnect resource link
							if(GetResourceLinkedCity() != NULL)
								SetResourceLinkedCityActive(false);
						}
						else
						{
							GET_PLAYER(getOwner()).changeNumResourceTotal(getResourceType(), getNumResourceForPlayer(getOwner()));

							// Reconnect resource link
							if(GetResourceLinkedCity() != NULL)
								SetResourceLinkedCityActive(true);
						}
					}
				}
			}
		}
#if defined(MOD_BALANCE_CORE)
		// Quantified Resource changes for improvements
		if(getImprovementType() != NO_IMPROVEMENT)
		{
			//Resource from improvement - change ownership if needed.
			ResourceTypes eResourceFromImprovement = (ResourceTypes)GC.getImprovementInfo(getImprovementType())->GetResourceFromImprovement();
			int iQuantity = GC.getImprovementInfo(getImprovementType())->GetResourceQuantityFromImprovement();
			if(iQuantity <= 0)
			{
				iQuantity = 1;
			}

			if(bPillaged && (eResourceFromImprovement != NO_RESOURCE) && (getResourceType() != NO_RESOURCE && getResourceType() != eResourceFromImprovement))
			{
				GET_PLAYER(getOwner()).changeNumResourceTotal(eResourceFromImprovement, (-1 * iQuantity), true);
			}
			else if(!bPillaged && (eResourceFromImprovement != NO_RESOURCE) && (getResourceType() != NO_RESOURCE && getResourceType() != eResourceFromImprovement))
			{
				GET_PLAYER(getOwner()).changeNumResourceTotal(eResourceFromImprovement, iQuantity, true);
			}
			int iMoves = GC.getImprovementInfo(getImprovementType())->GetMovesChange();
			if (bPillaged && GetPlotMovesChange() > 0)
			{
				ChangePlotMovesChange(iMoves * -1);
			}
			else if (!bPillaged && iMoves > 0 && getOwner() == getOwner())
			{
				ChangePlotMovesChange(iMoves);
			}

			CvImprovementEntry& oldImprovementEntry = *GC.getImprovementInfo(getImprovementType());

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
					break;

				if (oldImprovementEntry.GetYieldAdjacentSameType((YieldTypes)iI) > 0 || oldImprovementEntry.GetYieldAdjacentTwoSameType((YieldTypes)iI) > 0)
				{
					for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iJ));
						if (pAdjacentPlot && pAdjacentPlot->getImprovementType() == getImprovementType())
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
		}
#endif
		
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		if (bEvents && MOD_EVENTS_TILE_IMPROVEMENTS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileImprovementChanged, getX(), getY(), getOwner(), getImprovementType(), getImprovementType(), IsImprovementPillaged());
		}
#endif
	}

	if(bWasPillaged != m_bImprovementPillaged)
	{
		setLayoutDirty(true);
	}
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsImprovedByGiftFromMajor() const
{
	return m_bImprovedByGiftFromMajor;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetImprovedByGiftFromMajor(bool bValue)
{
	m_bImprovedByGiftFromMajor = bValue;
}

//	--------------------------------------------------------------------------------
/// Does this plot have a special improvement that we shouldn't remove?
bool CvPlot::HasSpecialImprovement() const
{
	// Gifted improvements (if we are a minor civ)
	if (getOwner() != NO_PLAYER)
	{
		CvPlayer* pOwner = &GET_PLAYER(getOwner());
		if (pOwner->isMinorCiv())
		{
			if (IsImprovedByGiftFromMajor())
			{
				return true;
			}
		}
	}

	// Great person improvements
	ImprovementTypes eImprovement = getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (pImprovementInfo && pImprovementInfo->IsCreatedByGreatPerson())
		{
			return true;
		}
#if defined(MOD_BALANCE_CORE)
		//Works like GP improvement.
		if (pImprovementInfo && pImprovementInfo->IsAdjacentCity())
		{
			return true;
		}
		//Don't delete landmarks!
		ImprovementTypes eLandmark = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK");
		if (eLandmark != NO_IMPROVEMENT && eImprovement == eLandmark)
		{
			return true;
		}
#endif
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsBarbarianCampNotConverting() const
{
	return m_bBarbCampNotConverting;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetBarbarianCampNotConverting(bool bNotConverting)
{
	m_bBarbCampNotConverting = bNotConverting;
}

//	--------------------------------------------------------------------------------
GenericWorldAnchorTypes CvPlot::GetWorldAnchor() const
{
	return (GenericWorldAnchorTypes) m_eWorldAnchor;
}

//	--------------------------------------------------------------------------------
int CvPlot::GetWorldAnchorData() const
{
	return (int) m_cWorldAnchorData;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetWorldAnchor(GenericWorldAnchorTypes eAnchor, int iData1)
{
	GenericWorldAnchorTypes eOldAnchor = (GenericWorldAnchorTypes) m_eWorldAnchor;

	// Set the new anchor
	m_eWorldAnchor = eAnchor;
	m_cWorldAnchorData = iData1;

	// Remove old
	if(eOldAnchor != NO_WORLD_ANCHOR)
		gDLL->GameplayWorldAnchor(eOldAnchor, /*bAdd*/ false, getX(), getY(), iData1);

	// Add new
	if(eAnchor == WORLD_ANCHOR_NATURAL_WONDER)
		gDLL->GameplayWorldAnchor(eAnchor, /*bAdd*/ true, getX(), getY(), iData1);
}


//	--------------------------------------------------------------------------------
RouteTypes CvPlot::getRouteType() const
{
	return (RouteTypes)m_eRouteType;
}


//	--------------------------------------------------------------------------------
void CvPlot::setRouteType(RouteTypes eNewValue)
{
	RouteTypes eOldRoute = getRouteType();
	int iI;

#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue < NO_ROUTE) return;
	if (eNewValue > NO_ROUTE && GC.getRouteInfo(eNewValue) == NULL) return;
#endif

	if(eOldRoute != eNewValue || (eOldRoute == eNewValue && IsRoutePillaged()))
	{
		// Remove old effects
		if(eOldRoute != NO_ROUTE && !isCity())
		{
			// Owned by someone
			if(isOwned())
			{
				CvRouteInfo& oldRouteInfo = *GC.getRouteInfo(eOldRoute);
				CvPlayer& owningPlayer = GET_PLAYER(getOwner());

				// Maintenance change!
				if(MustPayMaintenanceHere(getOwner()))
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getRouteInfo(getRouteType())->GetGoldMaintenance());
				}

				// Update the amount of a Resource used up by a Route which was previously here
				int iNumResourceInfos = GC.getNumResourceInfos();
				for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
				{
					int iRequiredResources = oldRouteInfo.getResourceQuantityRequirement(iResourceLoop);
					if(iRequiredResources > 0)
					{
						owningPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, -iRequiredResources);
					}
				}
			}

			// Someone built a route here in an unowned plot, remove the effects of it (since we're changing it to something else)
			if(GetPlayerResponsibleForRoute() != NO_PLAYER)
			{
				// Maintenance change!
				if(MustPayMaintenanceHere(GetPlayerResponsibleForRoute()))
				{
					CvRouteInfo* pkRouteInfo = GC.getRouteInfo(getRouteType());
					if(pkRouteInfo)
					{
						GET_PLAYER(GetPlayerResponsibleForRoute()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-pkRouteInfo->GetGoldMaintenance());
					}
				}

				SetPlayerResponsibleForRoute(NO_PLAYER);
			}
		}

		// Route switch here!
		m_eRouteType = eNewValue;

		// Apply new effects
		if(isOwned() && eNewValue != NO_ROUTE && !isCity())
		{
			CvRouteInfo* newRouteInfo = GC.getRouteInfo(eNewValue);
			if(newRouteInfo)
			{
				CvPlayer& owningPlayer = GET_PLAYER(getOwner());

				// Maintenance
				if(MustPayMaintenanceHere(getOwner()))
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(newRouteInfo->GetGoldMaintenance());
				}

				// Update the amount of a Resource used up by this Route
				int iNumResourceInfos = GC.getNumResourceInfos();
				for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
				{
					int iRequiredResources = newRouteInfo->getResourceQuantityRequirement(iResourceLoop);
					if(iRequiredResources > 0)
					{
						owningPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, iRequiredResources);
					}
				}
			}
		}

		// make sure this plot is not disabled
		// important to call this because city connection update is hooked up there
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		SetRoutePillaged(false, false);
#else
		SetRoutePillaged(false);
#endif

		for(iI = 0; iI < MAX_TEAMS; ++iI)
		{
#ifdef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
			if (GET_TEAM((TeamTypes)iI).isObserver() || GET_TEAM((TeamTypes)iI).isAlive())
#else
			if(GET_TEAM((TeamTypes)iI).isAlive())
#endif
			{
				if(isVisible((TeamTypes)iI))
				{
					setRevealedRouteType((TeamTypes)iI, getRouteType());
				}
			}
		}

		updateYield();

#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		if (MOD_EVENTS_TILE_IMPROVEMENTS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileRouteChanged, getX(), getY(), getOwner(), eOldRoute, eNewValue, IsRoutePillaged());
		}
#endif
	}
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsRoutePillaged() const
{
	return m_bRoutePillaged;
}

//	--------------------------------------------------------------------------------
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
void CvPlot::SetRoutePillaged(bool bPillaged, bool bEvents)
#else
void CvPlot::SetRoutePillaged(bool bPillaged)
#endif
{
	if(m_bRoutePillaged != bPillaged)
	{
		for(int iI = 0; iI < MAX_TEAMS; ++iI)
		{
#ifdef AUI_PLOT_OBSERVER_SEE_ALL_PLOTS
			if ( GET_TEAM((TeamTypes)iI).isObserver() || ((GET_TEAM((TeamTypes)iI).isAlive()) && GC.getGame().getActiveTeam() == (TeamTypes)iI) )
#else
			if(GET_TEAM((TeamTypes)iI).isAlive() && GC.getGame().getActiveTeam() == (TeamTypes)iI)
#endif
			{
				if(isVisible((TeamTypes)iI))
				{
					setLayoutDirty(true);
				}
			}
		}
	}
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
	else
	{
		bEvents = false;
	}
#endif

	//city connections will be recalculated on turn start for each player!
	m_bRoutePillaged = bPillaged;

#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
	if (bEvents && MOD_EVENTS_TILE_IMPROVEMENTS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileRouteChanged, getX(), getY(), getOwner(), getRouteType(), getRouteType(), IsRoutePillaged());
	}
#endif
}

//	--------------------------------------------------------------------------------
void CvPlot::updateCityRoute()
{
	RouteTypes eCityRoute;

	if(isCity())
	{
		CvAssertMsg(isOwned(), "isOwned is expected to be true");

		eCityRoute = GET_PLAYER(getOwner()).getBestRoute();

		if(eCityRoute == NO_ROUTE)
		{
			eCityRoute = ((RouteTypes)(GC.getINITIAL_CITY_ROUTE_TYPE()));
		}

		setRouteType(eCityRoute);
	}
}

//	--------------------------------------------------------------------------------
/// Who built this Improvement?  Could be NO_PLAYER
PlayerTypes CvPlot::GetPlayerThatBuiltImprovement() const
{
	return (PlayerTypes) m_ePlayerBuiltImprovement;
}

//	--------------------------------------------------------------------------------
/// Who built this Improvement?  Could be NO_PLAYER
void CvPlot::SetPlayerThatBuiltImprovement(PlayerTypes eBuilder)
{
	m_ePlayerBuiltImprovement = eBuilder;
}

//	--------------------------------------------------------------------------------
/// Who pays maintenance for this Improvement?
PlayerTypes CvPlot::GetPlayerResponsibleForImprovement() const
{
	return (PlayerTypes) m_ePlayerResponsibleForImprovement;
}

//	--------------------------------------------------------------------------------
/// Who pays maintenance for this Improvement?
void CvPlot::SetPlayerResponsibleForImprovement(PlayerTypes eNewValue)
{
	if(GetPlayerResponsibleForImprovement() != eNewValue)
	{
		m_ePlayerResponsibleForImprovement = eNewValue;
	}
}

//	--------------------------------------------------------------------------------
/// Who pays maintenance for this Route?
PlayerTypes CvPlot::GetPlayerResponsibleForRoute() const
{
	return (PlayerTypes) m_ePlayerResponsibleForRoute;
}

//	--------------------------------------------------------------------------------
/// Who pays maintenance for this Route?
void CvPlot::SetPlayerResponsibleForRoute(PlayerTypes eNewValue)
{
	if(GetPlayerResponsibleForRoute() != eNewValue)
	{
		m_ePlayerResponsibleForRoute = eNewValue;
	}
}

//	--------------------------------------------------------------------------------
PlayerTypes CvPlot::GetPlayerThatClearedBarbCampHere() const
{
	return (PlayerTypes) m_ePlayerThatClearedBarbCampHere;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetPlayerThatClearedBarbCampHere(PlayerTypes eNewValue)
{
	m_ePlayerThatClearedBarbCampHere = eNewValue;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//	--------------------------------------------------------------------------------
PlayerTypes CvPlot::GetPlayerThatClearedDigHere() const
{
	return (PlayerTypes) m_ePlayerThatClearedDigHere;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetPlayerThatClearedDigHere(PlayerTypes eNewValue)
{
	m_ePlayerThatClearedDigHere = eNewValue;
}
#endif

//	--------------------------------------------------------------------------------
CvCity* CvPlot::GetResourceLinkedCity() const
{
	return ::GetPlayerCity(m_ResourceLinkedCity);
}

//	--------------------------------------------------------------------------------
/// Link the resource on this plot to pCity. Note that this does NOT set the link to be active - this must be done manually
void CvPlot::SetResourceLinkedCity(const CvCity* pCity)
{
	if (GetResourceLinkedCity() != pCity)
	{
		int iResourceChange = getNumResource();
		if (pCity == NULL)
		{
			if (GetResourceLinkedCity() != NULL)
				GetResourceLinkedCity()->ChangeNumResourceLocal(getResourceType(), -iResourceChange, true);

			//connected?
			ImprovementTypes eImprovement = getImprovementType();
			if (eImprovement != NO_IMPROVEMENT)
			{
				//connected?
				CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
				if (pImprovement && pImprovement->IsExpandedImprovementResourceTrade(getResourceType()))
				{
					SetResourceLinkedCityActive(false);
				}

			}
			m_ResourceLinkedCity.reset();
		}
		else if (GetResourceLinkedCity() != NULL)
		{
			GetResourceLinkedCity()->ChangeNumResourceLocal(getResourceType(), -iResourceChange, true);
			ImprovementTypes eImprovement = getImprovementType();
			if (eImprovement != NO_IMPROVEMENT)
			{
				//connected?
				CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
				if (pImprovement && pImprovement->IsExpandedImprovementResourceTrade(getResourceType()))
				{
					SetResourceLinkedCityActive(false);
				}

			}
		}
		
		if (pCity != NULL)
		{
			CvAssertMsg(pCity->getOwner() == getOwner(), "Argument city pNewValue's owner is expected to be the same as the current instance");
			m_ResourceLinkedCity = pCity->GetIDInfo();
			GetResourceLinkedCity()->ChangeNumResourceLocal(getResourceType(), iResourceChange, true);
			ImprovementTypes eImprovement = getImprovementType();
			if (eImprovement != NO_IMPROVEMENT)
			{
				//connected?
				CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
				if (pImprovement && pImprovement->IsExpandedImprovementResourceTrade(getResourceType()))
				{
					SetResourceLinkedCityActive(true);
				}

			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Is the Resource connection with the linked city active? (e.g. pillaging)
bool CvPlot::IsResourceLinkedCityActive() const
{
	return m_bResourceLinkedCityActive;
}

//	--------------------------------------------------------------------------------
/// Is the Resource connection with the linked city active? (e.g. pillaging)
void CvPlot::SetResourceLinkedCityActive(bool bValue)
{
	if(bValue != IsResourceLinkedCityActive())
	{
		m_bResourceLinkedCityActive = bValue;

		// Now change num resource local to linked city (new or former)

		FAssertMsg(GetResourceLinkedCity() != NULL, "Resource linked city is null for some reason. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		FAssertMsg(getOwner() != NO_PLAYER, "Owner of a tile with a resource linkned to a city is not valid. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		int iResourceChange = bValue ? getNumResource() : -getNumResource();
		GetResourceLinkedCity()->ChangeNumResourceLocal(getResourceType(), iResourceChange);
		GetResourceLinkedCity()->ChangeNumResourceLocal(getResourceType(), iResourceChange*-1, true);

	}
}

//	--------------------------------------------------------------------------------
/// Find the closest nearby city to link the Resource here to
void CvPlot::DoFindCityToLinkResourceTo(CvCity* pCityToExclude)
{
	// Make sure we can actually use this Resource first

	// No resource here period
	if(getResourceType() == NO_RESOURCE)
		return;

	ResourceTypes eResource = getResourceType(getTeam());

	// Owner can't see resource here yet
	if(eResource == NO_RESOURCE)
		return;

	CvCity* pBestCity = NULL;

	// Loop through nearby plots to find the closest city to link to
	CvPlot* pLoopPlot;

	for(int iJ = 0; iJ < MAX_CITY_PLOTS; iJ++)
	{
		// We're not actually looking around a City but Resources have to be within the RANGE of a City, so we can still use this
		pLoopPlot = iterateRingPlots(getX(), getY(), iJ);

		if(pLoopPlot != NULL)
		{
			CvCity* pLoopCity = pLoopPlot->getPlotCity();
			if(pLoopCity)
			{
				// Owner of the City must match the owner of the Resource Plot Tile (Don't want to give the Resource to another player!)
				if(pLoopCity->getOwner() == getOwner())
				{
					if(pLoopCity != pCityToExclude)
					{
						pBestCity = pLoopCity;

						break;
					}
				}
			}
		}
	}

	if(pBestCity != NULL)
	{
		SetResourceLinkedCity(pBestCity);
	}
}

//	-----------------------------------------------------------------------------------------------
void CvPlot::setPlotCity(CvCity* pNewValue)
{
	CvPlot* pLoopPlot;
	int iI;

	if(getPlotCity() != pNewValue)
	{
		if(isCity())
		{
			// Is a route is here?  If so, we may now need to pay maintenance for it.  Yes, yes, I know, we're removing a city
			// so most likely the owner will loose the plot shortly, but it is best to keep the costs in sync, else bad things happen
			// if it doesn't get properly updated (like it has been == negative maintenance costs asserts)
			if(getRouteType() != NO_ROUTE && getPlotCity()->getOwner() == getOwner())
			{
				// Maintenance change!
				if(MustPayMaintenanceHere(getOwner()))
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(GC.getRouteInfo(getRouteType())->GetGoldMaintenance());
				}
			}


			for(iI = 0; iI < getPlotCity()->GetNumWorkablePlots(); ++iI)
			{
				pLoopPlot = iterateRingPlots(getX(), getY(), iI);

				if(pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(-1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwner(), -1);
				}
			}
		}

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			int iNewOwner = (pNewValue != NULL) ? pNewValue->getOwner() : -1;
			int iNewCity = (pNewValue != NULL) ? pNewValue->GetID() : -1;
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_CITY, m_iX, m_iY, 0, iNewOwner, m_plotCity.eOwner, iNewCity, m_plotCity.iID);
		}
#endif

		if(pNewValue != NULL)
		{
			m_plotCity = pNewValue->GetIDInfo();
		}
		else
		{
			m_plotCity.reset();
		}

		if(isCity())
		{

			for(iI = 0; iI < getPlotCity()->GetNumWorkablePlots(); ++iI)
			{
				pLoopPlot = iterateRingPlots(getX(), getY(), iI);

				if(pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwner(), 1);
				}
			}
#if defined(MOD_BALANCE_CORE)
			if(isMountain())
			{
				ImprovementTypes eMachuPichu = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_JFD_MACHU_PICCHU");
				setImprovementType(eMachuPichu);
			}
#endif
			// Is a route is here?  If we already owned this plot, then we were paying maintenance, now we don't have to.
			if(getRouteType() != NO_ROUTE && getPlotCity()->getOwner() == getOwner())
			{
				// Maintenance change!
				if(MustPayMaintenanceHere(getOwner()))
				{
					GET_PLAYER(getOwner()).GetTreasury()->ChangeBaseImprovementGoldMaintenance(-GC.getRouteInfo(getRouteType())->GetGoldMaintenance());
				}
			}

		}

		updateYield();
	}
}

//	-----------------------------------------------------------------------------------------------
int CvPlot::getOwningCityID() const
{
	return m_owningCity.iID;
}

CvCity* CvPlot::getOwningCity() const
{
	return ::GetPlayerCity(m_owningCity);
}

//	--------------------------------------------------------------------------------
void CvPlot::updateOwningCity()
{
	CvCity* pBestCity = getPlotCity();
	if(pBestCity == NULL)
	{
		pBestCity = getOwningCityOverride();
		CvAssertMsg((pBestCity == NULL) || (pBestCity->getOwner() == getOwner()), "pBest city is expected to either be NULL or the current plot instance's");
	}

	if((pBestCity == NULL) && isOwned())
	{
		int iBestPlot = 0;
		for(int iI = 0; iI < MAX_CITY_PLOTS; ++iI)
		{
			CvPlot* pLoopPlot = iterateRingPlots(getX(), getY(), iI);
			if(pLoopPlot != NULL)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();
				if(pLoopCity != NULL)
				{
					if(pLoopCity->getOwner() == getOwner())
					{
						if((pBestCity == NULL) ||
						        (GC.getCityPlotPriority()[iI] < GC.getCityPlotPriority()[iBestPlot]) ||
						        ((GC.getCityPlotPriority()[iI] == GC.getCityPlotPriority()[iBestPlot]) &&
						         ((pLoopCity->getGameTurnAcquired() < pBestCity->getGameTurnAcquired()) ||
						          ((pLoopCity->getGameTurnAcquired() == pBestCity->getGameTurnAcquired()) &&
						           (pLoopCity->GetID() < pBestCity->GetID())))))
						{
							iBestPlot = iI;
							pBestCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	CvCity* pOldOwningCity = getOwningCity();

	if(pOldOwningCity != pBestCity)
	{
		// Remove Citizen from this plot if another City's using it
		if (pOldOwningCity != NULL)
		{
			// Remove citizen
			pOldOwningCity->GetCityCitizens()->SetWorkingPlot(this, false);
			SetResourceLinkedCity(NULL);
			// Re-add citizen somewhere else
			pOldOwningCity->GetCityCitizens()->DoAddBestCitizenFromUnassigned();
		}
		// Change what City's allowed to work this Plot
		if(pBestCity != NULL)
		{
#if defined(MOD_BALANCE_CORE)
			SetResourceLinkedCity(pBestCity);
#endif

			CvAssertMsg(isOwned(), "isOwned is expected to be true");
			CvAssertMsg(!isBeingWorked(), "isBeingWorked did not return false as expected");
			m_owningCity = pBestCity->GetIDInfo();
		}
		else
		{
			m_owningCity.reset();
		}

		updateYield();

		GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
CvCity* CvPlot::getOwningCityOverride() const
{
	return ::GetPlayerCity(m_owningCityOverride);
}


//	--------------------------------------------------------------------------------
void CvPlot::setOwningCityOverride(const CvCity* pNewValue)
{
	if(getOwningCityOverride() != pNewValue)
	{
		if(pNewValue != NULL)
		{
			CvAssertMsg(pNewValue->getOwner() == getOwner(), "Argument city pNewValue's owner is expected to be the same as the current instance");
			m_owningCityOverride = pNewValue->GetIDInfo();
		}
		else
		{
			m_owningCityOverride.reset();
		}

		updateOwningCity();
	}
}

//	--------------------------------------------------------------------------------
int CvPlot::getReconCount() const
{
	return m_iReconCount;
}


//	--------------------------------------------------------------------------------
void CvPlot::changeReconCount(int iChange)
{
	m_iReconCount = (m_iReconCount + iChange);
	CvAssert(getReconCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvPlot::getRiverCrossingCount() const
{
	return m_iRiverCrossingCount;
}


//	--------------------------------------------------------------------------------
void CvPlot::changeRiverCrossingCount(int iChange)
{
	m_iRiverCrossingCount = (m_iRiverCrossingCount + iChange);
	CvAssert(getRiverCrossingCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvPlot::getYield(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (int)(m_aiYield[eIndex]);
}

#if defined(MOD_API_EXTENSIONS)
void CvPlot::changeYield(YieldTypes eYield, int iChange)
{
    CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
    CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
    m_aiArbitraryYields[eYield] += iChange;
    updateYield();
}
#endif

int CvPlot::calculateNatureYield(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pOwningCity, bool bIgnoreFeature, bool bDisplay) const
{
	int iYield = 0;
	TeamTypes eTeam = (ePlayer!=NO_PLAYER) ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM;

	//performance critical ...
	static const ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
	static const ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");

	const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);
	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	// impassable terrain has no base yield (but do allow coast)
	// if worked by a city, it should have a yield.
	if( (!isValidMovePlot(ePlayer) && getOwner()!=ePlayer && (!isShallowWater()) || getTerrainType()==NO_TERRAIN))
	{
		iYield = 0;
	} 
	else
	{
		iYield = GC.getTerrainInfo(getTerrainType())->getYield(eYield);
		if (eYield == YIELD_PRODUCTION && GC.getMOD_BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT() > 0 && getTerrainType() == TERRAIN_DESERT && !isHills() && getFeatureType() == NO_FEATURE)
		{
			if (getResourceType(eTeam) != NO_RESOURCE && getImprovementType() != NO_IMPROVEMENT)
				iYield += GC.getMOD_BALANCE_CORE_PRODUCTION_DESERT_IMPROVEMENT();
		}
	}

	if (MOD_API_PLOT_YIELDS) 
	{
		iYield += GC.getPlotInfo(getPlotType())->getYield(eYield);
	}

	iYield += GC.getGame().getPlotExtraYield(m_iX, m_iY, eYield);

	if(isHills())
	{
		iYield += kYield.getHillsChange();
	}

	if(isMountain())
	{
		iYield += kYield.getMountainChange();
	}

	if(isLake())
	{
		iYield += kYield.getLakeChange();
	}
	if(!bIgnoreFeature)
	{
		if(getFeatureType() != NO_FEATURE)
		{
			CvFeatureInfo* pFeatureInfo = GC.getFeatureInfo(getFeatureType());

			// Some Features REPLACE the Yield of the Plot instead of adding to it
			int iYieldChange = pFeatureInfo->getYieldChange(eYield);

			if(pFeatureInfo->isYieldNotAdditive())
			{
				iYield = iYieldChange;
			}
			else
			{
				iYield += iYieldChange;
			}
		}
	}

	if(eTeam != NO_TEAM)
	{
		ResourceTypes eResource = getResourceType(eTeam);

		if(eResource != NO_RESOURCE)
		{
			iYield += GC.getResourceInfo(eResource)->getYieldChange(eYield);
		}
	}

	if(isRiver())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType())->getRiverYieldChange(eYield) : GC.getFeatureInfo(getFeatureType())->getRiverYieldChange(eYield));
	}

	if(isHills())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType())->getHillsYieldChange(eYield) : GC.getFeatureInfo(getFeatureType())->getHillsYieldChange(eYield));
	}

	if(isFreshWater())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType())->getFreshWaterYieldChange(eYield) : GC.getFeatureInfo(getFeatureType())->getFreshWaterYieldChange(eYield));
	}

	if(isCoastalLand())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType())->getCoastalLandYieldChange(eYield) : GC.getFeatureInfo(getFeatureType())->getCoastalLandYieldChange(eYield));
	}

	if (pOwningCity != NULL && pOwningCity->plot() == this && ePlayer != NO_PLAYER)
	{
		iYield = std::max(iYield, kYield.getMinCity());

		if (!bDisplay || pOwningCity->isRevealed(GC.getGame().getActiveTeam(), false))
		{
			iYield += kYield.getCityChange();

			if (kYield.getPopulationChangeDivisor() != 0)
			{
				iYield += (pOwningCity->getPopulation() + kYield.getPopulationChangeOffset()) / kYield.getPopulationChangeDivisor();
			}
		}

		// Mod for Player; used for Policies and such
		iYield += GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityYieldChanges(eYield);

		// Coastal City Mod
		if (pOwningCity->isCoastal())
		{
			iYield += GET_PLAYER(ePlayer).GetCoastalCityYieldChange(eYield);

			iYield += GET_PLAYER(ePlayer).GetPlayerTraits()->GetCoastalCityYieldChanges(eYield);

		}

		if (MOD_BALANCE_YIELD_SCALE_ERA)
		{
			//Flatland City Fresh Water yields
			if (!isHills() && !isMountain() && isFreshWater())
			{
				iYield += kYield.getMinCityFlatFreshWater();
			}
			//Flatland City No Fresh Water yields
			if (!isHills() && !isMountain() && !isFreshWater())
			{
				iYield += kYield.getMinCityFlatNoFreshWater();
			}
			// Hill City Fresh Water yields
			if (isHills() && isFreshWater())
			{
				iYield += kYield.getMinCityHillFreshWater();
			}
			// Hill City No Fresh Water yields
			if (isHills() && !isFreshWater())
			{
				iYield += kYield.getMinCityHillNoFreshWater();
			}
			// Mountain City Fresh Water yields
			if (isMountain() && isFreshWater())
			{
				iYield += kYield.getMinCityMountainFreshWater();
			}
			// Mountain City No Fresh Water yields
			if (isMountain() && !isFreshWater())
			{
				iYield += kYield.getMinCityMountainNoFreshWater();
			}
			if (pOwningCity->HasGarrison())
			{
				CvUnit* pUnit = pOwningCity->GetGarrisonedUnit();
				if (pUnit != NULL && pUnit->GetGarrisonYieldChange(eYield) > 0)
				{
					int iGarrisonStrength = pUnit->GetBaseCombatStrength();
					iYield += ((pUnit->GetGarrisonYieldChange(eYield) * iGarrisonStrength) / 8);
				}
			}
			if (pOwningCity->plot()->getImprovementType() == eFort || pOwningCity->plot()->getImprovementType() == eCitadel)
			{
			// Don't provide yield if tile is pillaged
				if (!pOwningCity->plot()->IsImprovementPillaged())
				{
					// If there are any Units here, meet their owners
					for (int iUnitLoop = 0; iUnitLoop < getNumUnits(); iUnitLoop++)
					{
						// If the AI spots a human Unit, don't meet - wait for the human to find the AI
						CvUnit* loopUnit = getUnitByIndex(iUnitLoop);
						if (!loopUnit)
							continue;
	
						if (loopUnit->GetFortificationYieldChange(eYield) > 0)
						{
							int iUnitStrength = loopUnit->GetBaseCombatStrength();
							iYield += ((loopUnit->GetFortificationYieldChange(eYield) * iUnitStrength) / 8);
						}
					}
				}
			}
		}

		int iTemp = GET_PLAYER(ePlayer).GetCityYieldChangeTimes100(eYield);	// In hundreds - will be added to capitalYieldChange below

																				// Capital Mod
		if (pOwningCity->isCapital())
		{
			iTemp += GET_PLAYER(ePlayer).GetCapitalYieldChangeTimes100(eYield);

			iYield += GET_PLAYER(ePlayer).GetPlayerTraits()->GetCapitalYieldChanges(eYield);

			int iPerPopYield = pOwningCity->getPopulation() * GET_PLAYER(getOwner()).GetCapitalYieldPerPopChange(eYield);
			iPerPopYield /= 100;
			iYield += iPerPopYield;

			if (GET_PLAYER(ePlayer).GetCapitalYieldPerPopChangeEmpire(eYield) != 0)
			{
				int iPerPopYieldEmpire = GET_PLAYER(ePlayer).getTotalPopulation() / GET_PLAYER(ePlayer).GetCapitalYieldPerPopChangeEmpire(eYield);
				iYield += iPerPopYieldEmpire;
			}
		}

		iYield += (iTemp / 100);
	}

	return std::max(0, iYield);
}

int CvPlot::calculateReligionNatureYield(YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pOwningCity, const CvReligion* pMajorityReligion, const CvBeliefEntry* pSecondaryPantheon) const
{
	if (!pMajorityReligion)
		return 0;

	if (ePlayer == NO_PLAYER)
		return 0;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	int iYield = 0;

	//Change for improvement/resource
	int iReligionChange = 0;
	bool bRequiresImprovement = pMajorityReligion->m_Beliefs.RequiresImprovement(ePlayer);
	bool bRequiresNoImprovement = pMajorityReligion->m_Beliefs.RequiresNoImprovement(ePlayer);
	bool bRequiresResource = pMajorityReligion->m_Beliefs.RequiresResource(ePlayer);
	bool bRequiresNoFeature = pMajorityReligion->m_Beliefs.RequiresNoFeature(ePlayer);
	bool bRequiresEmptyTile = (bRequiresResource && bRequiresNoFeature);
	bool bRequiresBoth = (bRequiresImprovement && bRequiresResource);
	int iValue = pMajorityReligion->m_Beliefs.GetTerrainYieldChange(getTerrainType(), eYield, ePlayer, pOwningCity);
	if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && (bRequiresImprovement || bRequiresResource || bRequiresNoImprovement))
	{
		if (bRequiresBoth)
		{
			if (getImprovementType() != NO_IMPROVEMENT && getResourceType(eTeam) != NO_RESOURCE)
			{
				if (GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType(eTeam)))
				{
					iReligionChange += iValue;
				}
			}
		}
		else if (bRequiresImprovement)
		{
			if (getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType(eTeam)))
				{
					iReligionChange += iValue;
				}
			}
		}
		else if (bRequiresResource)
		{
			if (getResourceType(eTeam) != NO_RESOURCE)
			{
				iReligionChange += iValue;
			}
		}
		else if (bRequiresEmptyTile)
		{
			if (getImprovementType() == NO_IMPROVEMENT && getFeatureType() == NO_FEATURE)
			{
				iReligionChange += iValue;
			}
		}
		else if (bRequiresNoImprovement)
		{
			if (getImprovementType() == NO_IMPROVEMENT)
			{
				iReligionChange += iValue;
			}
		}
		else if (bRequiresNoFeature)
		{
			if (getFeatureType() == NO_FEATURE)
			{
				iReligionChange += iValue;
			}
		}
		if (iReligionChange > iValue)
		{
			iReligionChange = iValue;
		}
	}
	else
	{
		iReligionChange = iValue;
	}

	iYield += iReligionChange;

	if (pSecondaryPantheon)
	{
		//Change for improvement/resource
		iReligionChange = 0;
		bool bRequiresImprovement = pSecondaryPantheon->RequiresImprovement();
		bool bRequiresNoImprovement = pSecondaryPantheon->RequiresNoImprovement();
		bool bRequiresResource = pSecondaryPantheon->RequiresResource();
		bool bRequiresNoFeature = pSecondaryPantheon->RequiresNoFeature();
		bool bRequiresEmptyTile = (bRequiresResource && bRequiresNoFeature);
		bool bRequiresBoth = (bRequiresImprovement && bRequiresResource);
		int iValue = pSecondaryPantheon->GetTerrainYieldChange(getTerrainType(), eYield);
		if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && (bRequiresImprovement || bRequiresResource || bRequiresNoImprovement))
		{
			if (bRequiresBoth)
			{
				if (getImprovementType() != NO_IMPROVEMENT && getResourceType(eTeam) != NO_RESOURCE)
				{
					if (GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType(eTeam)))
					{
						iReligionChange += iValue;
					}
				}
			}
			else if (bRequiresImprovement)
			{
				if (getImprovementType() != NO_IMPROVEMENT)
				{
					if (GC.getImprovementInfo(getImprovementType())->IsExpandedImprovementResourceTrade(getResourceType(eTeam)))
					{
						iReligionChange += iValue;
					}
				}
			}
			else if (bRequiresResource)
			{
				if (getResourceType(eTeam) != NO_RESOURCE)
				{
					iReligionChange += iValue;
				}
			}
			else if (bRequiresEmptyTile)
			{
				if (getImprovementType() == NO_IMPROVEMENT && getFeatureType() == NO_FEATURE)
				{
					iReligionChange += iValue;
				}
			}
			else if (bRequiresNoImprovement)
			{
				if (getImprovementType() == NO_IMPROVEMENT)
				{
					iReligionChange += iValue;
				}
			}
			else if (bRequiresNoFeature)
			{
				if (getFeatureType() == NO_FEATURE)
				{
					iReligionChange += iValue;
				}
			}
			if (iReligionChange > iValue)
			{
				iReligionChange = iValue;
			}
		}
		else
		{
			iReligionChange = iValue;
		}

		iYield += iReligionChange;
	}

	if (MOD_RELIGION_PLOT_YIELDS)
	{
		iYield += pMajorityReligion->m_Beliefs.GetPlotYieldChange(getPlotType(), eYield, ePlayer, pOwningCity);
		if (pSecondaryPantheon)
		{
			iYield += pSecondaryPantheon->GetPlotYieldChange(getPlotType(), eYield);
		}
	}

	if (isLake())
	{
		iYield += pMajorityReligion->m_Beliefs.GetLakePlotYieldChange(eYield, ePlayer, pOwningCity);
		if (pSecondaryPantheon)
		{
			iYield += pSecondaryPantheon->GetLakePlotYieldChange(eYield);
		}
	}

	if (getFeatureType() != NO_FEATURE)
	{
		iYield += pMajorityReligion->m_Beliefs.GetUnimprovedFeatureYieldChange(getFeatureType(), eYield, ePlayer, pOwningCity);
		if (pSecondaryPantheon)
		{
			iYield += pSecondaryPantheon->GetUnimprovedFeatureYieldChange(getFeatureType(), eYield);
		}

		//Change for improvement/resource
		iReligionChange = 0;
		bool bRequiresNoImprovement = pMajorityReligion->m_Beliefs.RequiresNoImprovement(ePlayer);
		bool bRequiresImprovement = pMajorityReligion->m_Beliefs.RequiresImprovement(ePlayer);
		if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresNoImprovement && getImprovementType() == NO_IMPROVEMENT)
		{
			iReligionChange += pMajorityReligion->m_Beliefs.GetFeatureYieldChange(getFeatureType(), eYield, ePlayer, pOwningCity);
		}
		else if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresImprovement && getImprovementType() != NO_IMPROVEMENT)
		{
			iReligionChange += pMajorityReligion->m_Beliefs.GetFeatureYieldChange(getFeatureType(), eYield, ePlayer, pOwningCity);
		}
		else
		{
			iReligionChange += pMajorityReligion->m_Beliefs.GetFeatureYieldChange(getFeatureType(), eYield, ePlayer, pOwningCity);
		}

		iYield += iReligionChange;

		if (pSecondaryPantheon)
		{
			//Change for improvement/resource
			iReligionChange = 0;
			bool bRequiresNoImprovement = pSecondaryPantheon->RequiresNoImprovement();
			bool bRequiresImprovement = pSecondaryPantheon->RequiresImprovement();
			if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresNoImprovement && getImprovementType() == NO_IMPROVEMENT)
			{
				iReligionChange += pSecondaryPantheon->GetFeatureYieldChange(getFeatureType(), eYield);
			}
			else if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresImprovement && getImprovementType() != NO_IMPROVEMENT)
			{
				iReligionChange += pSecondaryPantheon->GetFeatureYieldChange(getFeatureType(), eYield);
			}
			else
			{
				iReligionChange += pSecondaryPantheon->GetFeatureYieldChange(getFeatureType(), eYield);
			}

			iYield += iReligionChange;
		}

		if (IsNaturalWonder())
		{
			iYield += pMajorityReligion->m_Beliefs.GetYieldChangeNaturalWonder(eYield, ePlayer, pOwningCity);
			if (pSecondaryPantheon)
			{
				iYield += pSecondaryPantheon->GetYieldChangeNaturalWonder(eYield);
			}
		}
	}

	if (getResourceType(eTeam) != NO_RESOURCE)
	{
		iYield += pMajorityReligion->m_Beliefs.GetResourceYieldChange(getResourceType(eTeam), eYield, ePlayer, pOwningCity);
		if (pSecondaryPantheon)
		{
			iYield += pSecondaryPantheon->GetResourceYieldChange(getResourceType(eTeam), eYield);
		}
	}

	return iYield;
}

//	--------------------------------------------------------------------------------
int CvPlot::calculateBestNatureYield(YieldTypes eIndex, PlayerTypes ePlayer) const
{
	return std::max(calculateNatureYield(eIndex, ePlayer, NULL, false), calculateNatureYield(eIndex, ePlayer, NULL, true));
}


//	--------------------------------------------------------------------------------
int CvPlot::calculateTotalBestNatureYield(PlayerTypes ePlayer) const
{
	return (calculateBestNatureYield(YIELD_FOOD, ePlayer) + calculateBestNatureYield(YIELD_PRODUCTION, ePlayer) + calculateBestNatureYield(YIELD_GOLD, ePlayer));
}

int CvPlot::calculateReligionImprovementYield(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, const CvCity* pOwningCity, const CvReligion* pMajorityReligion, const CvBeliefEntry* pSecondaryPantheon) const
{
	if (!pOwningCity || !pMajorityReligion || ePlayer == NO_PLAYER)
		return 0;

	if (eImprovement == NO_IMPROVEMENT || IsImprovementPillaged())
		return 0;

	CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
	if (!pImprovement)
		return 0;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	int iReligionChange = 0;
	bool bRequiresResource = pMajorityReligion->m_Beliefs.RequiresResource(pOwningCity->getOwner());
	if (pImprovement->IsCreatedByGreatPerson() || pImprovement->IsAdjacentCity() || pImprovement->IsIgnoreOwnership())
	{
		bRequiresResource = false;
	}
	if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresResource)
	{
		if (bRequiresResource && (getResourceType(kPlayer.getTeam()) != NO_RESOURCE) && pImprovement->IsImprovementResourceMakesValid(getResourceType(kPlayer.getTeam())))
		{
			iReligionChange += pMajorityReligion->m_Beliefs.GetImprovementYieldChange(eImprovement, eYield, pOwningCity->getOwner(), pOwningCity);
		}
	}
	else
	{
		iReligionChange += pMajorityReligion->m_Beliefs.GetImprovementYieldChange(eImprovement, eYield, pOwningCity->getOwner(), pOwningCity);
	}
	
	if (!pSecondaryPantheon)
		return iReligionChange;

	bRequiresResource = pSecondaryPantheon->RequiresResource();
	if (pImprovement->IsCreatedByGreatPerson() || pImprovement->IsAdjacentCity())
	{
		bRequiresResource = false;
	}
	if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresResource)
	{
		if (bRequiresResource && (getResourceType(kPlayer.getTeam()) != NO_RESOURCE && pImprovement->IsImprovementResourceMakesValid(getResourceType(kPlayer.getTeam()))))
		{
			iReligionChange += pSecondaryPantheon->GetImprovementYieldChange(eImprovement, eYield);
		}
	}
	else
	{
		iReligionChange += pSecondaryPantheon->GetImprovementYieldChange(eImprovement, eYield);
	}

	return iReligionChange;
}
//	--------------------------------------------------------------------------------
int CvPlot::calculateImprovementYield(ImprovementTypes eImprovement, YieldTypes eYield, int iCurrentYield, PlayerTypes ePlayer, bool bOptimal, RouteTypes eAssumeThisRoute) const
{
	ResourceTypes eResource;
	int iBestYield;
	int iYield;
	int iI;

	if (eImprovement == NO_IMPROVEMENT || IsImprovementPillaged())
		return 0;

	CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
	if (!pImprovement)
		return 0;

	bool bIsFreshWater = isFreshWater();

	iYield = pImprovement->GetYieldChange(eYield);

	if (ePlayer != NO_PLAYER)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if (iYield > 0 || iCurrentYield > 0)
		{
			int iAdjacentYield = pImprovement->GetYieldAdjacentSameType(eYield);
			if (iAdjacentYield > 0)
			{
				iYield += ComputeYieldFromAdjacentImprovement(*pImprovement, eImprovement, eYield);
			}
	
			int iTwoAdjacentYield = pImprovement->GetYieldAdjacentTwoSameType(eYield);
			if (iTwoAdjacentYield > 0)
			{
				iYield += ComputeYieldFromTwoAdjacentImprovement(*pImprovement, eImprovement, eYield);
			}
		}

		int iYieldChangePerEra = pImprovement->GetYieldChangePerEra(eYield);
		if (iYieldChangePerEra > 0)
		{
			int iPlotEra = GetArchaeologicalRecord().m_eEra;
			int iNumEras = kPlayer.GetCurrentEra() - iPlotEra;

			if (iPlotEra != NO_ERA && iNumEras > 0)
			{
				iYield += (iNumEras * iYieldChangePerEra);
			}
		}

		if (getOwner() == ePlayer)
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pAdjacentPlot == NULL)
					continue;

				if (pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT && pAdjacentPlot->getOwner() == ePlayer)
				{
					CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(pAdjacentPlot->getImprovementType());
					if (pImprovement2)
					{
						iYield += pImprovement2->GetAdjacentImprovementYieldChanges(eImprovement, eYield);
					}
				}

				if (pAdjacentPlot->getFeatureType() != NO_FEATURE)
				{
					iYield += pImprovement->GetAdjacentFeatureYieldChanges(pAdjacentPlot->getFeatureType(), eYield);
				}

				if (pAdjacentPlot->getResourceType() != NO_RESOURCE)
				{
					iYield += pImprovement->GetAdjacentResourceYieldChanges(pAdjacentPlot->getResourceType(), eYield);
				}
			}
		}

		eResource = getResourceType(kPlayer.getTeam());

		if (eResource != NO_RESOURCE)
		{
			iYield += pImprovement->GetImprovementResourceYield(eResource, eYield);
		}
	}

	if(isRiver())
	{
		iYield += pImprovement->GetRiverSideYieldChange(eYield);
	}

	if(isCoastalLand())
	{
		iYield += pImprovement->GetCoastalLandYieldChange(eYield);
	}

	if(isHills())
	{
		iYield += pImprovement->GetHillsYieldChange(eYield);
	}
#if defined(MOD_BALANCE_CORE)
	if (getFeatureType() != NO_FEATURE)
	{
		iYield += pImprovement->GetFeatureYieldChanges(getFeatureType(), eYield);
	}
#endif

	// Check to see if there's a bonus to apply before doing any looping
	if(pImprovement->GetAdjacentCityYieldChange(eYield) > 0 ||
	        pImprovement->GetAdjacentMountainYieldChange(eYield) > 0)
	{
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isCity())
				{
					// Is the owner of this Plot (with the Improvement) also the owner of an adjacent City?
					if(pAdjacentPlot->getPlotCity()->getOwner() == getOwner())
					{
						iYield += pImprovement->GetAdjacentCityYieldChange(eYield);
					}
				}
				if(pAdjacentPlot->isMountain())
				{
					iYield += pImprovement->GetAdjacentMountainYieldChange(eYield);
				}
			}
		}
	}

	if (bIsFreshWater || bOptimal)
	{
		iYield += pImprovement->GetFreshWaterYieldChange(eYield);
	}

	if(bOptimal)
	{
		iBestYield = 0;

		for(iI = 0; iI < GC.getNumRouteInfos(); ++iI)
		{
			iBestYield = std::max(iBestYield, pImprovement->GetRouteYieldChanges(iI, eYield));
		}

		iYield += iBestYield;
	}
	else
	{
		RouteTypes eRouteType = NO_ROUTE;
		if(eAssumeThisRoute != NUM_ROUTE_TYPES)
		{
			eRouteType = eAssumeThisRoute;
		}
		else
		{
			eRouteType = getRouteType();
		}
		if(ePlayer != NO_PLAYER)
		{
			if(IsTradeUnitRoute())
			{
				if( GET_PLAYER(ePlayer).GetCurrentEra() >= 4 )
				{
					iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
				}
				else
				{
					iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
				}
			}

			if (eRouteType != NO_ROUTE)
			{
				if (MOD_BALANCE_YIELD_SCALE_ERA)
				{
					if (IsCityConnection(ePlayer))
					{
						if (IsRouteRailroad())
						{
							iYield += pImprovement->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
						}
						else if (IsRouteRoad())
						{
							iYield += pImprovement->GetRouteYieldChanges(ROUTE_ROAD, eYield);
						}
					}
				}
				else
				{
					iYield += pImprovement->GetRouteYieldChanges(eRouteType, eYield);
				}

				if (!IsRoutePillaged())
				{
					CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRouteType);
					if (pkRouteInfo)
					{
						iYield += pkRouteInfo->getYieldChange(eYield);
					}
				}
			}
		}
	}

	if(bOptimal || ePlayer == NO_PLAYER)
	{
		for(iI = 0; iI < GC.getNumTechInfos(); ++iI)
		{
			iYield += pImprovement->GetTechYieldChanges(iI, eYield);

			if(bIsFreshWater)
			{
				iYield += pImprovement->GetTechFreshWaterYieldChanges(iI, eYield);
			}
			else
			{
				iYield += pImprovement->GetTechNoFreshWaterYieldChanges(iI, eYield);
			}
		}

		for(iI = 0; iI < GC.getNumPolicyInfos(); ++iI)
		{
			const PolicyTypes ePolicy = static_cast<PolicyTypes>(iI);
			CvPolicyEntry* pkPolicyEntry = GC.getPolicyInfo(ePolicy);
			if(pkPolicyEntry)
			{
				iYield += pkPolicyEntry->GetImprovementYieldChanges(eImprovement, eYield);
			}
		}
	}

	return iYield;
}

int CvPlot::calculatePlayerYield(YieldTypes eYield, int iCurrentYield, PlayerTypes ePlayer, ImprovementTypes eImprovement, const CvCity* pOwningCity, const CvReligion* pMajorityReligion, const CvBeliefEntry* pSecondaryPantheon, bool bDisplay) const
{
	if (ePlayer == NO_PLAYER)
		return 0;

	int iYield = 0;

	const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	FeatureTypes eFeature = getFeatureType();
	TerrainTypes eTerrain = getTerrainType();

	//plot yields
	iYield += pTraits->GetPlotYieldChange(getPlotType(), eYield);
	iYield += kPlayer.getPlotYieldChange(getPlotType(), eYield);

	if (isMountain())
	{
#if defined(MOD_PSEUDO_NATURAL_WONDER)
		if (!IsNaturalWonder(true))
#else
		if (!IsNaturalWonder())
#endif
		{
			int iRangeYield = pTraits->GetMountainRangeYield(eYield);
			int iEra = kPlayer.GetCurrentEra();
			if (iEra <= 0)
			{
				iEra = 1;
			}
			iRangeYield *= iEra;
			iYield += iRangeYield;
		}
		else
		{
			iYield += pTraits->GetYieldChangeNaturalWonder(eYield);
			iYield += kPlayer.GetYieldChangesNaturalWonder(eYield);

			if (eFeature != NO_FEATURE)
			{
				CvFeatureInfo* pFeatureInfo = GC.getFeatureInfo(eFeature);
				if (pFeatureInfo)
				{
					int iNWYieldChange = pFeatureInfo->getYieldChange(eYield);

					int iMod = 0;
					iMod += pTraits->GetNaturalWonderYieldModifier();

					//we only do this here because we need the natural yield right away.
					if (pMajorityReligion)
					{
						iMod += pMajorityReligion->m_Beliefs.GetYieldModifierNaturalWonder(eYield, ePlayer, pOwningCity);
						if (pSecondaryPantheon)
						{
							iMod += pSecondaryPantheon->GetYieldModifierNaturalWonder(eYield);
						}
					}

					if (iMod != 0)
					{
						iNWYieldChange *= iMod;
						iNWYieldChange /= 100;

						iYield += iNWYieldChange;
					}
				}
			}
		}
	}
#if defined(MOD_PSEUDO_NATURAL_WONDER)
	else if (IsNaturalWonder(true))
#else
	else if (IsNaturalWonder())
#endif
	{

		iYield += pTraits->GetYieldChangeNaturalWonder(eYield);
		iYield += kPlayer.GetYieldChangesNaturalWonder(eYield);

		if (eFeature != NO_FEATURE)
		{
			CvFeatureInfo* pFeatureInfo = GC.getFeatureInfo(eFeature);
			if (pFeatureInfo)
			{
				int iNWYieldChange = pFeatureInfo->getYieldChange(eYield);

				int iMod = 0;
				iMod += pTraits->GetNaturalWonderYieldModifier();

				//we only do this here because we need the natural yield right away.
				if (pMajorityReligion)
				{
					iMod += pMajorityReligion->m_Beliefs.GetYieldModifierNaturalWonder(eYield, ePlayer, pOwningCity);
					if (pSecondaryPantheon)
					{
						iMod += pSecondaryPantheon->GetYieldModifierNaturalWonder(eYield);
					}
				}

				if (iMod != 0)
				{
					iNWYieldChange *= iMod;
					iNWYieldChange /= 100;

					iYield += iNWYieldChange;
				}
			}
		}
	}
	if (eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
	{
		// Policy improvement yield changes
		iYield += kPlayer.getImprovementYieldChange(eImprovement, eYield);

		if (!pTraits->IsTradeRouteOnly())
		{
			// Trait player improvement yield changes that don't require a trade route connection
			iYield += pTraits->GetImprovementYieldChange(eImprovement, eYield);
		}
		// Team Tech Yield Changes
		iYield += kTeam.getImprovementYieldChange(eImprovement, eYield);

		if (isFreshWater())
		{
			// Team Tech Yield Changes
			iYield += kTeam.getImprovementFreshWaterYieldChange(eImprovement, eYield);
		}
		else
		{
			// Team Tech Yield Changes
			iYield += kTeam.getImprovementNoFreshWaterYieldChange(eImprovement, eYield);
		}
	}
	
	// Trait player terrain/improvement (for features handled below) yield changes that don't require a trade route connection
	if (pTraits->IsTradeRouteOnly() && getOwner() == ePlayer)
	{
		if (eFeature == NO_FEATURE && !MOD_USE_TRADE_FEATURES)
		{
			int iBonus = pTraits->GetTerrainYieldChange(eTerrain, eYield);
			if (iBonus > 0)
			{
				if (IsCityConnection(ePlayer) || IsTradeUnitRoute())
				{
					int iScale = 0;
					int iEra = (kPlayer.GetCurrentEra() + 1);

					iScale = ((iBonus * iEra) / 4);

					if (iScale <= 0)
					{
						iScale = 1;
					}
					iYield += iScale;
				}
			}
		}
		else
		{
			int iBonus = pTraits->GetTerrainYieldChange(eTerrain, eYield);
			if (iBonus > 0)
			{
				if (IsCityConnection(ePlayer) || IsTradeUnitRoute())
				{
					int iScale = 0;
					int iEra = (kPlayer.GetCurrentEra() + 1);

					iScale = ((iBonus * iEra) / 4);

					if (iScale <= 0)
					{
						iScale = 1;
					}
					iYield += iScale;
				}
			}
		}
		if (eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
		{
			int iBonus2 = pTraits->GetImprovementYieldChange(eImprovement, eYield);
			if (iBonus2 > 0)
			{
				if (IsCityConnection(ePlayer) || IsTradeUnitRoute() || IsAdjacentToTradeRoute())
				{
					int iScale = 0;
					int iEra = (kPlayer.GetCurrentEra() + 1);

					iScale = ((iBonus2 * iEra) / 2);

					if (iScale <= 0)
					{
						iScale = 1;
					}
					iYield += iScale;
				}
			}
		}
	}
	else
	{
		iYield += pTraits->GetTerrainYieldChange(eTerrain, eYield);
	}

	iYield += kPlayer.getTerrainYieldChange(eTerrain, eYield);

	if (kPlayer.getExtraYieldThreshold(eYield) > 0)
	{
		if (iYield >= kPlayer.getExtraYieldThreshold(eYield))
		{
			iYield += GC.getEXTRA_YIELD();
		}
	}

	int iBonusYield = eFeature == NO_FEATURE ? kTeam.getTerrainYieldChange(eTerrain, eYield) : kTeam.getFeatureYieldChange(eFeature, eYield);
	if (IsNaturalWonder())
	{
		int iMod = pTraits->GetNaturalWonderYieldModifier();
		if (iMod > 0)
		{
			iBonusYield *= (100 + iMod);
			iBonusYield /= 100;
		}
	}
	iYield += iBonusYield;

	if (pOwningCity != NULL)
	{
		if (isValidMovePlot(ePlayer))
		{
			if (isWater())
			{
#if defined(MOD_API_UNIFIED_YIELDS)
				if (!isLake())
				{
					iYield += kPlayer.getSeaPlotYield(eYield);
					iYield += pTraits->GetSeaPlotYieldChanges(eYield);
				}
#else
				iYield += kPlayer.getSeaPlotYield(eYield);
#endif

				if (!bDisplay || pOwningCity->isRevealed(GC.getGame().getActiveTeam(), false))
				{
					int iCityYield = 0;
					if (isLake())
					{
						if (pOwningCity->getLakePlotYield(eYield) > 0)
						{
							iCityYield = pOwningCity->getLakePlotYield(eYield);
						}
					}
					else
					{
						iCityYield = pOwningCity->getSeaPlotYield(eYield);
					}
					iYield += iCityYield;
				}

				if (getResourceType(kPlayer.getTeam()) != NO_RESOURCE)
				{
					if (!bDisplay || pOwningCity->isRevealed(GC.getGame().getActiveTeam(), false))
					{
						iYield += pOwningCity->getSeaResourceYield(eYield);
					}
				}
			}
			else if (isRiver())
			{
				if (!bDisplay || pOwningCity->isRevealed(GC.getGame().getActiveTeam(), false))
				{
					iYield += pOwningCity->getRiverPlotYield(eYield);
				}
			}

			// Extra yield for terrain
			if (eTerrain != NO_TERRAIN)
			{
				iYield += pOwningCity->GetTerrainExtraYield(eTerrain, eYield);
			}
		}

		if (eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
		{
			if (pImprovement->IsCreatedByGreatPerson())
			{
				if (pOwningCity->GetWeLoveTheKingDayCounter() > 0)
				{
					if (pTraits->GetWLTKDGPImprovementModifier() > 0)
					{
						int iBoon = pTraits->GetWLTKDGPImprovementModifier();
						iYield *= (100 + iBoon);
						iYield /= 100;
					}
				}
			}

			// Extra yield for improvements
			iYield += pOwningCity->GetImprovementExtraYield(eImprovement, eYield);
			iYield += kPlayer.GetImprovementExtraYield(eImprovement, eYield);
		}


		// Extra yield for features
		if (eFeature != NO_FEATURE)
		{
			iYield += pOwningCity->GetFeatureExtraYield(eFeature, eYield);

			if (eImprovement != NO_IMPROVEMENT)
			{
				if (pImprovement)
				{
					iYield += pImprovement->GetFeatureYieldChanges(eFeature, eYield);
				}
			}
		}
		
		iYield += pOwningCity->GetPlotExtraYield(getPlotType(), eYield);

		ResourceTypes eResource = getResourceType(kPlayer.getTeam());
		if (eResource != NO_RESOURCE)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo)
			{
				if (kTeam.GetTeamTechs()->HasTech((TechTypes)pkResourceInfo->getTechReveal()))
				{
					if (pkResourceInfo->getPolicyReveal() == NO_POLICY || kPlayer.GetPlayerPolicies()->HasPolicy((PolicyTypes)pkResourceInfo->getPolicyReveal()))
					{
						// Extra yield from resources
						iYield += pOwningCity->GetResourceExtraYield(eResource, eYield);

						// Extra yield from Trait
						if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
						{
							iYield += pTraits->GetYieldChangeStrategicResources(eYield);
						}
					}
				}

				iYield += pOwningCity->GetEventResourceYield(getResourceType(), eYield);
				iYield += kPlayer.getResourceYieldChange(eResource, eYield);
				iYield += pTraits->GetResourceYieldChange(eResource, eYield);

				if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES && kPlayer.HasGlobalMonopoly(eResource))
				{
					int iTemp = GC.getResourceInfo(eResource)->getYieldChangeFromMonopoly(eYield);
					if (iTemp > 0)
					{
						iTemp += GET_PLAYER(ePlayer).GetMonopolyModFlat();
						iYield += iTemp;
					}
				}
			}
		}

		if (eFeature != NO_FEATURE)
		{
			iYield += pOwningCity->GetEventFeatureYield(eFeature, eYield);
		}
		if (eTerrain != NO_TERRAIN)
		{
			iYield += pOwningCity->GetEventTerrainYield(eTerrain, eYield);

			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL && pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT && pAdjacentPlot->getOwner() == ePlayer)
				{
					CvImprovementEntry* pImprovement2 = GC.getImprovementInfo(pAdjacentPlot->getImprovementType());
					if (pImprovement2 && pImprovement2->GetAdjacentTerrainYieldChanges(eTerrain, eYield) > 0)
					{
						iYield += pImprovement2->GetAdjacentTerrainYieldChanges(eTerrain, eYield);
					}
				}
			}
		}
		if (eImprovement != NO_IMPROVEMENT)
		{
			iYield += pOwningCity->GetEventImprovementYield(eImprovement, eYield);
		}
	}

	if (eFeature != NO_FEATURE)
	{
		// Player Trait
		if (eImprovement == NO_IMPROVEMENT)
		{
			iYield += pTraits->GetUnimprovedFeatureYieldChange(eFeature, eYield);
			iYield += kPlayer.getUnimprovedFeatureYieldChange(eFeature, eYield);
		}

		// Leagues
		iYield += GC.getGame().GetGameLeagues()->GetFeatureYieldChange(ePlayer, eFeature, eYield);

		iYield += kPlayer.getFeatureYieldChange(eFeature, eYield);
		iYield += pTraits->GetFeatureYieldChange(eFeature, eYield);
	}

	if (eImprovement != NO_IMPROVEMENT && eYield == YIELD_CULTURE)
	{
		iYield += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_EXTRA_CULTURE_FROM_IMPROVEMENTS);
		iYield += kPlayer.GetPlayerPolicies()->GetImprovementCultureChange(eImprovement);
	}

	if (kPlayer.isGoldenAge())
	{
		if ((iYield + iCurrentYield) >= kYield.getGoldenAgeYieldThreshold())
		{
			iYield += kYield.getGoldenAgeYield();
		}
	}

	return iYield;
}

int CvPlot::calculateYield(YieldTypes eYield, bool bDisplay)
{
	const CvCity* pOwningCity = getOwningCity();
	if(pOwningCity)
	{
		ReligionTypes eMajority = pOwningCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = pOwningCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();

		const CvReligion* pReligion = (eMajority != NO_RELIGION) ? GC.getGame().GetGameReligions()->GetReligion(eMajority, pOwningCity->getOwner()) : 0;
		const CvBeliefEntry* pBelief = (eSecondaryPantheon != NO_BELIEF) ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon) : 0;

		return calculateYieldFast(eYield, bDisplay, pOwningCity, pReligion, pBelief);
	}

	return calculateYieldFast(eYield, bDisplay, NULL, NULL, NULL);
}

//	--------------------------------------------------------------------------------
int CvPlot::calculateYieldFast(YieldTypes eYield, bool bDisplay, const CvCity* pOwningCity, const CvReligion* pMajorityReligion, const CvBeliefEntry* pSecondaryPantheon)
{
	ImprovementTypes eImprovement = NO_IMPROVEMENT;
	RouteTypes eRoute = NO_ROUTE;
	PlayerTypes ePlayer = NO_PLAYER;

	if(bDisplay && GC.getGame().isDebugMode())
	{
		return getYield(eYield);
	}

	if(getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}
#if defined(MOD_NO_YIELD_ICE)
	if(MOD_NO_YIELD_ICE)
	{
		if(isIce())
		{
			return 0;
		}
	}
#endif

	if(!isPotentialCityWork())
	{
		return 0;
	}

	if(bDisplay)
	{
		ePlayer = getRevealedOwner(GC.getGame().getActiveTeam());
		eImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam());
		eRoute = getRevealedRouteType(GC.getGame().getActiveTeam());

		if(ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGame().getActivePlayer();
		}
	}
	else
	{
		ePlayer = getOwner() != NO_PLAYER ? getOwner() : NO_PLAYER;
		eImprovement = getImprovementType() != NO_IMPROVEMENT ? getImprovementType() : NO_IMPROVEMENT;
		eRoute = getRouteType() != NO_ROUTE ? getRouteType() : NO_ROUTE;
	}

	int iYield = calculateNatureYield(eYield, ePlayer, pOwningCity, false, bDisplay);
	iYield += calculateReligionImprovementYield(eImprovement, eYield, ePlayer, pOwningCity, pMajorityReligion, pSecondaryPantheon);
	iYield += calculateReligionNatureYield(eYield, ePlayer, pOwningCity, pMajorityReligion, pSecondaryPantheon);
	iYield += calculateImprovementYield(eImprovement, eYield, iYield, ePlayer, false, eRoute);
	iYield += calculatePlayerYield(eYield, iYield, ePlayer, eImprovement, pOwningCity, pMajorityReligion, pSecondaryPantheon, bDisplay);

#if defined(MOD_API_EXTENSIONS)
    if (m_aiArbitraryYields[eYield])
    {
        iYield += m_aiArbitraryYields[eYield];
    }
#endif

	return std::max(0, iYield);
}


//	--------------------------------------------------------------------------------
bool CvPlot::hasYield() const
{
	for(int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		//Simplification - errata yields not worth considering.
		if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			return false;

		if(getYield((YieldTypes)iI) > 0)
			return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvPlot::updateYield()
{
	CvCity*	pOwningCity = getOwningCity();
	if (pOwningCity)
	{
		ReligionTypes eMajority = pOwningCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = pOwningCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();

		const CvReligion* pReligion = (eMajority != NO_RELIGION) ? GC.getGame().GetGameReligions()->GetReligion(eMajority, pOwningCity->getOwner()) : 0;
		const CvBeliefEntry* pBelief = (eSecondaryPantheon != NO_BELIEF) ? GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon) : 0;

		updateYieldFast(pOwningCity, pReligion, pBelief);
		return;
	}

	updateYieldFast(NULL, NULL, NULL);
}

void CvPlot::updateYieldFast(CvCity* pOwningCity, const CvReligion* pMajorityReligion, const CvBeliefEntry* pSecondaryPantheon)
{
	bool bChange = false;
	if(getArea() == -1)
		return;

	for(int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		YieldTypes eYield = (YieldTypes)iI;
		//Simplification - errata yields not worth considering.
		if (eYield > YIELD_CULTURE_LOCAL && !MOD_BALANCE_CORE_JFD)
			continue;

		int iNewYield = calculateYieldFast(eYield,false,pOwningCity,pMajorityReligion,pSecondaryPantheon);

		if(getYield(eYield) != iNewYield)
		{
			int iOldYield = getYield(eYield);
			m_aiYield[iI] = max(0, iNewYield);

			if(pOwningCity != NULL && pOwningCity->GetCityCitizens()->IsWorkingPlot(this))
			{
				int iDelta = iNewYield - iOldYield;
				pOwningCity->ChangeBaseYieldRateFromTerrain(eYield, iDelta);

#if defined(MOD_BALANCE_CORE)
				pOwningCity->UpdateCityYields(eYield);
				if(eYield == YIELD_CULTURE || eYield == YIELD_TOURISM)
				{
					pOwningCity->GetCityCulture()->CalculateBaseTourismBeforeModifiers();
					pOwningCity->GetCityCulture()->CalculateBaseTourism();
				}				
#endif
			}

			bChange = true;
		}
	}

	if(bChange)
	{
		updateSymbols();
	}
}

//	--------------------------------------------------------------------------------
int CvPlot::GetExplorationBonus(const CvPlayer* pPlayer, const CvUnit* pUnit)
{
	if (!pPlayer || !pUnit || pPlayer->getNumCities()==0)
		return 0;

	CvPlot* pRefPlot = pUnit->plot();

	//land based exploration - give a bonus to fertile tiles that are close to our own territory
	if (pUnit->getDomainType() == DOMAIN_LAND)
	{
		int iBonus = 0;
		if (pPlayer->getCapitalCity())
		{
			//do not use the founding values here, they are expensive to compute
			int iFertility = GC.getGame().GetSettlerSiteEvaluator()->PlotFertilityValue(this, true);
			int iRefFertility = GC.getGame().GetSettlerSiteEvaluator()->PlotFertilityValue(pPlayer->getCapitalCity()->plot(), true);
			iBonus = range((iFertility * 100) / MAX(1, iRefFertility), 0, 100);
		}

		if (getOwner() == NO_PLAYER)
			iBonus += 20;

		int iDistToOwnCities = pPlayer->GetCityDistancePathLength(this);
		int iDistRef = pPlayer->GetCityDistancePathLength(pRefPlot);
		if (iDistToOwnCities < iDistRef)
			iBonus += 20;

		return iBonus;
	}

	//naval exploration - the further away, the better
	return pPlayer->GetCityDistanceInPlots(this) - pPlayer->GetCityDistanceInPlots(pRefPlot);
}

//	--------------------------------------------------------------------------------
int CvPlot::getFoundValue(PlayerTypes eIndex)
{
	//this is just an indirection - the found values are stored in the player class now
	return GET_PLAYER(eIndex).getPlotFoundValue(getX(), getY() );
}


//	--------------------------------------------------------------------------------
bool CvPlot::isBestAdjacentFoundValue(PlayerTypes eIndex)
{
	CvPlayer& thisPlayer = GET_PLAYER(eIndex);
	int iPlotValue = getFoundValue(eIndex);

	if(iPlotValue == 0)
	{
		return false;
	}

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if((pAdjacentPlot != NULL) && pAdjacentPlot->isRevealed(thisPlayer.getTeam()))
		{
			if(pAdjacentPlot->getFoundValue(eIndex) > iPlotValue)
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvPlot::setFoundValue(PlayerTypes eIndex, int iNewValue)
{
	//this is just an indirection - the found values are stored in the player class now
	GET_PLAYER(eIndex).setPlotFoundValue(getX(), getY(), iNewValue);
}


//	--------------------------------------------------------------------------------
int CvPlot::getPlayerCityRadiusCount(PlayerTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_aiPlayerCityRadiusCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvPlot::isPlayerCityRadius(PlayerTypes eIndex) const
{
	return (getPlayerCityRadiusCount(eIndex) > 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(0 != iChange)
	{
		m_aiPlayerCityRadiusCount[eIndex] += iChange;
		CvAssert(getPlayerCityRadiusCount(eIndex) >= 0);
	}
}

void CvPlot::flipVisibility(TeamTypes eTeam)
{
	if (eTeam == NO_TEAM)
		return;

	//flip visibility
	if (m_aiVisibilityCountThisTurnMax[eTeam] != m_aiVisibilityCount[eTeam])
	{
		m_aiVisibilityCountThisTurnMax[eTeam] = m_aiVisibilityCount[eTeam];
		if (m_aiVisibilityCount[eTeam] == 0) //in case it's now invisible
			updateFog();
	}
}

//	--------------------------------------------------------------------------------
#if defined(MOD_API_EXTENSIONS)
PlotVisibilityChangeResult CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bInformExplorationTracking, bool bAlwaysSeeInvisible, CvUnit* pUnit)
#else
PlotVisibilityChangeResult CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bInformExplorationTracking, bool bAlwaysSeeInvisible)
#endif
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	PlotVisibilityChangeResult eResult = VISIBILTY_CHANGE_NONE;
	if (iChange == 0)
		return eResult;

	bool bOldVisibility = (m_aiVisibilityCount[eTeam]>0);

	//apparently it's legal to decrease sight below zero - so catch that
	if (iChange<0 && abs(iChange)>m_aiVisibilityCount[eTeam])
		m_aiVisibilityCount[eTeam] = 0;
	else
		m_aiVisibilityCount[eTeam] += iChange;

	//remember the maximum
	m_aiVisibilityCountThisTurnMax[eTeam] = max(m_aiVisibilityCountThisTurnMax[eTeam], m_aiVisibilityCount[eTeam]);

	if(eSeeInvisible != NO_INVISIBLE)
	{
		changeInvisibleVisibilityCount(eTeam, eSeeInvisible, iChange);
	}

	if(bAlwaysSeeInvisible)
	{
		changeInvisibleVisibilityCountUnit(eTeam, iChange);
		for(int iI = 0; iI < NUM_INVISIBLE_TYPES; iI++)
		{
			changeInvisibleVisibilityCount(eTeam, (InvisibleTypes) iI, iChange);
		}
	}

	// We couldn't see the Plot before but we can now
	// note: the isVisible check works even when MOD_CORE_DELAYED_VISIBILITY is active
	if (bOldVisibility == false && isVisible(eTeam))
	{
		eResult = VISIBILITY_CHANGE_TO_VISIBLE;

#if defined(MOD_API_EXTENSIONS)
		if (setRevealed(eTeam, true, pUnit))	// Change to revealed, returns true if the visibility was changed
#else
		if (setRevealed(eTeam, true))	// Change to revealed, returns true if the visibility was changed
#endif
		{
			//we are seeing this plot for the first time
			if (bInformExplorationTracking)
			{
				vector<PlayerTypes> vPlayers = GET_TEAM(eTeam).getPlayers();
				for (size_t i = 0; i < vPlayers.size(); i++)
					GET_PLAYER(vPlayers[i]).GetEconomicAI()->UpdateExplorePlotsLocally(this);
			}
		}
		else
		{
			// The visibility was not changed because it was already revealed, but we are changing to a visible state as well, so we must update.
			// Just trying to avoid redundant messages.
			if (eTeam == GC.getGame().getActiveTeam())
			{
				updateSymbols();
				updateFog(true);
				updateVisibility();
			}
		}

		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlot->updateRevealedOwner(eTeam);
			}
		}

		// If there are any Units here, meet their owners
		for (int iUnitLoop = 0; iUnitLoop < getNumUnits(); iUnitLoop++)
		{
			// If the AI spots a human Unit, don't meet - wait for the human to find the AI
			CvUnit* loopUnit = getUnitByIndex(iUnitLoop);
			if (!loopUnit)
				continue;

#if defined(MOD_BALANCE_CORE_MILITARY)
			//if it is an enemy unit, update the danger plots! 
			if (GET_TEAM(eTeam).isAtWar(loopUnit->getTeam()))
			{
				const std::vector<PlayerTypes>& aePlayers = GET_TEAM(eTeam).getPlayers();
				for (size_t iI = 0; iI < aePlayers.size(); iI++)
				{
					PlayerTypes ePlayer = (PlayerTypes)aePlayers[iI];
					if (ePlayer != NO_PLAYER)
					{
						if (GET_PLAYER(ePlayer).AddKnownAttacker(loopUnit) && pUnit)
							pUnit->SetSpottedEnemy(true);
					}
				}
			}
#endif
			//why shouldn't an AI meet a human?
			if (!GET_TEAM(eTeam).isHuman() && loopUnit->isHuman())
				continue;

			GET_TEAM(eTeam).meet(loopUnit->getTeam(), false);
		}

		// If there's a City here, meet its owner
		if (isCity())
		{
			// If the AI spots a human City, don't meet - wait for the human to find the AI
			if (GET_TEAM(eTeam).isHuman() || !getPlotCity()->isHuman())
			{
				GET_TEAM(eTeam).meet(getTeam(), false);	// If there's a City here, we can assume its owner is the same as the plot owner
			}
		}

		if (eTeam == GC.getGame().getActiveTeam())
		{
			updateCenterUnit();
		}

	}
//with delayed visibility we do this in setTurnActive()
#if !defined(MOD_CORE_DELAYED_VISIBILITY)
	// We could se the plot before but not anymore
	else if (bOldVisibility == true && !isVisible(eTeam))
	{
		eResult = VISIBILITY_CHANGE_TO_INVISIBLE;
		if (eTeam == GC.getGame().getActiveTeam())
		{
			updateFog(true);
			updateCenterUnit();
		}
	}
#endif

	return eResult;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvPlot::getRevealedOwner(TeamTypes eTeam, bool bDebug) const
{
	if(bDebug && GC.getGame().isDebugMode())
	{
		return getOwner();
	}
	else
	{
		CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		return (PlayerTypes)m_aiRevealedOwner[eTeam];
	}
}


//	--------------------------------------------------------------------------------
TeamTypes CvPlot::getRevealedTeam(TeamTypes eTeam, bool bDebug) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eRevealedOwner = getRevealedOwner(eTeam, bDebug);

	if(eRevealedOwner != NO_PLAYER)
	{
		return GET_PLAYER(eRevealedOwner).getTeam();
	}
	else
	{
		return NO_TEAM;
	}
}

//	--------------------------------------------------------------------------------
PlayerTypes CvPlot::getRevealedOwner(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	return (PlayerTypes)m_aiRevealedOwner[eTeam];
}


//	--------------------------------------------------------------------------------
TeamTypes CvPlot::getRevealedTeam(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eRevealedOwner = getRevealedOwner(eTeam);

	if(eRevealedOwner != NO_PLAYER)
	{
		return GET_PLAYER(eRevealedOwner).getTeam();
	}
	else
	{
		return NO_TEAM;
	}
}


//	--------------------------------------------------------------------------------
bool CvPlot::setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if(getRevealedOwner(eTeam, false) != eNewValue)
	{
		m_aiRevealedOwner[eTeam] = eNewValue;

		// Let the team know we've found someone (if eNewValue IS someone)
		if(eNewValue != NO_PLAYER)
		{
			GET_TEAM(eTeam).SetHasFoundPlayersTerritory(eNewValue, true);
		}
		return true;
	}

	CvAssert(m_aiRevealedOwner[eTeam] == eNewValue);
	return false;
}


//	--------------------------------------------------------------------------------
void CvPlot::updateRevealedOwner(TeamTypes eTeam)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	//we change the revealed owner when the plot or a neighbor is visible (not revealed)
	bool bRevealed = isVisible(eTeam);

	if(!bRevealed)
	{
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isVisible(eTeam))
				{
					bRevealed = true;
					break;
				}
			}
		}
	}

	//only change the status if we consider the plot ownership revealed
	if (bRevealed)
		setRevealedOwner(eTeam, getOwner());
}


//	--------------------------------------------------------------------------------
bool CvPlot::isRiverCrossing(DirectionTypes eIndex) const
{
	CvAssertMsg(eIndex < NUM_DIRECTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(eIndex == NO_DIRECTION)
	{
		return false;
	}

	int iFlowMask = 1 << eIndex;

	return ((m_cRiverCrossing & iFlowMask) != 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::updateRiverCrossing(DirectionTypes eIndex)
{

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DIRECTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	bool bValid = false;
	CvPlot* pPlot = plotDirection(getX(), getY(), eIndex);

	if((NULL == pPlot) || (!pPlot->isWater() && !isWater()))  // if there is no plot in that direction or both plots are land
	{
		switch(eIndex)
		{

		case DIRECTION_NORTHEAST:
		{
			if(pPlot && pPlot->isNEOfRiver())
			{
				bValid = true;
				break;
			}
		}
		break;

		case DIRECTION_EAST:
			bValid = isWOfRiver();
			break;

		case DIRECTION_SOUTHEAST:
			bValid = isNWOfRiver();
			break;

		case DIRECTION_SOUTHWEST:
			bValid = isNEOfRiver();
			break;

		case DIRECTION_WEST:
			if(pPlot != NULL)
			{
				bValid = pPlot->isWOfRiver();
			}
			break;

		case DIRECTION_NORTHWEST:
			if(pPlot != NULL)
			{
				bValid = pPlot->isNWOfRiver();
			}
			break;

		default:
			CvAssert(false);
			break;
		}
	}

	if(isRiverCrossing(eIndex) != bValid)
	{
		char iFlowMask = 1 << eIndex;
		if(bValid)
		{
			m_cRiverCrossing |= iFlowMask;
			changeRiverCrossingCount(1);
		}
		else
		{
			m_cRiverCrossing &= ~iFlowMask;
			changeRiverCrossingCount(-1);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlot::updateRiverCrossing()
{
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		updateRiverCrossing((DirectionTypes)iI);
	}
}

//	--------------------------------------------------------------------------------
/// Force reveal a Resource for a team (for Goody Huts)
bool CvPlot::IsResourceForceReveal(TeamTypes eTeam) const
{
	return m_abResourceForceReveal[eTeam];
}

//	--------------------------------------------------------------------------------
/// Set force reveal a Resource for a team (for Goody Huts)
void CvPlot::SetResourceForceReveal(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_abResourceForceReveal[eTeam] = bValue;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
inline bool CvPlot::IsTeamImpassable(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < REALLY_MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_abIsImpassable[eTeam];
}
//	--------------------------------------------------------------------------------
/// Set force reveal a Resource for a team (for Goody Huts)
void CvPlot::SetTeamImpassable(TeamTypes eTeam, bool bValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < REALLY_MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_abIsImpassable[eTeam] = bValue;
}
#endif
//	--------------------------------------------------------------------------------
#if defined(MOD_API_EXTENSIONS)
bool CvPlot::setRevealed(TeamTypes eTeam, bool bNewValue, CvUnit* pUnit, bool bTerrainOnly, TeamTypes eFromTeam)
#else
bool CvPlot::setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam)
#endif
{
	int iI;
	
#if defined(MOD_EVENTS_TILE_REVEALED)
	// We need to capture this value here, as a Natural Wonder may update it before we need it
	int iRevealedMajors = getNumMajorCivsRevealed();
#endif

	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	ICvUserInterface2* pInterface =  GC.GetEngineUserInterface();

	bool bVisbilityUpdated = false;
	if(isRevealed(eTeam) != bNewValue)
	{
		m_bfRevealed.ToggleBit(eTeam);

#if !defined(NO_ACHIEVEMENTS)
		bool bEligibleForAchievement = GET_PLAYER(GC.getGame().getActivePlayer()).isHuman() && !GC.getGame().isGameMultiPlayer();
#endif

		if(area())
		{
			area()->changeNumRevealedTiles(eTeam, (bNewValue ? 1 : -1));
		}

		// Natural Wonder
		if(eTeam != BARBARIAN_TEAM && bNewValue)
		{
			if(getFeatureType() != NO_FEATURE)
			{
#if defined(MOD_PSEUDO_NATURAL_WONDER)
				if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder(true))
#else
				if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder())
#endif
				{
					GET_TEAM(eTeam).ChangeNumNaturalWondersDiscovered(1);

					int iNumNaturalWondersLeft = GC.getMap().GetNumNaturalWonders() - GET_TEAM(eTeam).GetNumNaturalWondersDiscovered();

#if defined(MOD_EVENTS_NW_DISCOVERY)
					if (MOD_EVENTS_NW_DISCOVERY) {
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_NaturalWonderDiscovered, eTeam, getFeatureType(), getX(), getY(), (getNumMajorCivsRevealed() == 0), (pUnit?pUnit->getOwner():NO_PLAYER), (pUnit?pUnit->GetID():-1));
					} else {
#endif
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem) 
					{
						CvLuaArgsHandle args;
						args->Push(eTeam);
						args->Push(getFeatureType());
						args->Push(getX());
						args->Push(getY());
						args->Push((getNumMajorCivsRevealed() == 0)); // bFirst

						bool bResult = false;
						LuaSupport::CallHook(pkScriptSystem, "NaturalWonderDiscovered", args.get(), bResult);
					}
#if defined(MOD_EVENTS_NW_DISCOVERY)
					}
#endif
					
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_FOUND_NATURAL_WONDER");
					strText << iNumNaturalWondersLeft;
					strText << GC.getFeatureInfo(getFeatureType())->GetTextKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_NATURAL_WONDER");

					// Loop through all players to give them a Notification
					for(iI = 0; iI < MAX_MAJOR_CIVS; ++iI)
					{
						CvPlayerAI& playerI = GET_PLAYER((PlayerTypes)iI);
						if(playerI.isAlive())
						{
							if(playerI.getTeam() == eTeam)
							{
								// Num Natural Wonders found in a player's area
								if(playerI.getStartingPlot() != NULL)
								{
									if(getArea() == playerI.getStartingPlot()->getArea())
									{
										playerI.ChangeNumNaturalWondersDiscoveredInArea(1);
									}
								}
								playerI.CalculateNetHappiness();

								// Add World Anchor
								if(eTeam == eActiveTeam)
									SetWorldAnchor(WORLD_ANCHOR_NATURAL_WONDER, getFeatureType());

								// Notification
								CvNotifications* pNotifications = GET_PLAYER((PlayerTypes)iI).GetNotifications();
								if(pNotifications)
								{
									pNotifications->Add(NOTIFICATION_EXPLORATION_RACE, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getFeatureType());
								}
							}
						}
					}

					// FIRST (MAJOR CIV) FINDER?
					int iFinderGold = 0;
					bool bFirstFinder = false;
					CvTeam& kTeam = GET_TEAM(eTeam);
					if(!kTeam.isMinorCiv() && !kTeam.isBarbarian() && !kTeam.isObserver())
					{
						if(getNumMajorCivsRevealed() == 0)
						{
							changeNumMajorCivsRevealed(1);
							bFirstFinder = true;

							// Does it yields gold to the first major civ finder?
							iFinderGold = GC.getFeatureInfo(getFeatureType())->getFirstFinderGold();

							// Does a player on this team have a trait that gives first finder gold?
							for(iI = 0; iI < MAX_MAJOR_CIVS; ++iI)
							{
								CvPlayerAI& playerI = GET_PLAYER((PlayerTypes)iI);
								if(playerI.isAlive())
								{
									if(playerI.getTeam() == eTeam)
									{
										iFinderGold += playerI.GetPlayerTraits()->GetNaturalWonderFirstFinderGold();
									}
								}
							}
						}
						else
						{
							// Does a player on this team have a trait that gives subsequent finder gold?
							for(iI = 0; iI < MAX_MAJOR_CIVS; ++iI)
							{
								CvPlayerAI& playerI = GET_PLAYER((PlayerTypes)iI);
								if(playerI.isAlive())
								{
									if(playerI.getTeam() == eTeam)
									{
										iFinderGold += playerI.GetPlayerTraits()->GetNaturalWonderSubsequentFinderGold();
									}
								}
							}
						}

						// Scale up or down based on difficulty
						if(iFinderGold > 0)
						{
							const int iStandardHandicap = GC.getInfoTypeForString("HANDICAP_PRINCE");
							if(iStandardHandicap >= 0)
							{
								const CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo((HandicapTypes)iStandardHandicap);
								if(pkHandicapInfo)
								{
									iFinderGold *= pkHandicapInfo->getBarbCampGold();
									iFinderGold /= GC.getGame().getHandicapInfo().getBarbCampGold();
								}
							}
						}

						if(iFinderGold > 0)
						{
							// Compute gold per team member
							int iGoldPerTeamMember = iFinderGold;

							if(kTeam.getNumMembers() > 0)
							{
								iGoldPerTeamMember = iFinderGold / kTeam.getNumMembers();
							}

							for(iI = 0; iI < MAX_MAJOR_CIVS; ++iI)
							{
								CvPlayerAI& playerI = GET_PLAYER((PlayerTypes)iI);
								if(playerI.isAlive())
								{
									if(playerI.getTeam() == eTeam)
									{
										playerI.GetTreasury()->ChangeGold(iGoldPerTeamMember);
									}
								}
#if defined(MOD_BUGFIX_MINOR)
							}
#endif

								if(eTeam == eActiveTeam)
								{
									char text[256] = {0};
									sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iFinderGold);
									SHOW_PLOT_POPUP(this, NO_PLAYER, text);
								}
#if !defined(MOD_BUGFIX_MINOR)
							}
#endif
						}
					}

					// If it's the active team then tell them they found something
					if(eTeam == eActiveTeam)
					{
						bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();
#ifdef AUI_PLOT_OBSERVER_NO_NW_POPUPS
						if ( GET_TEAM((TeamTypes)iI).isObserver() )
							bDontShowRewardPopup = true;
#endif
						// Popup (no MP)
#if defined(MOD_API_EXTENSIONS)
						if(!GC.getGame().isReallyNetworkMultiPlayer() && !bDontShowRewardPopup)
#else
						if(!GC.getGame().isNetworkMultiPlayer() && !bDontShowRewardPopup)	// KWG: candidate for !GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS)
#endif
						{
							CvPopupInfo kPopupInfo(BUTTONPOPUP_NATURAL_WONDER_REWARD, getX(), getY(), iFinderGold, 0 /*iFlags */, bFirstFinder);
							pInterface->AddPopup(kPopupInfo);
							CvPlayer& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
							if (kActivePlayer.getTeam() == eActiveTeam)
							{
								// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
								CancelActivePlayerEndTurn();
							}

#if !defined(NO_ACHIEVEMENTS)
							//Add Stat and check for Achievement
							if(bEligibleForAchievement && !GC.getGame().isGameMultiPlayer())
							{
								gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_NATURALWONDERS, 100, ACHIEVEMENT_ALL_NATURALWONDER);
							}
#endif

						}

#if !defined(NO_ACHIEVEMENTS)
						//DLC2 Natural Wonder Achievements
						{
							CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(getFeatureType());
							if(pkFeatureInfo)
							{
								CvString strFeatureType = pkFeatureInfo->GetType();
								if(strFeatureType == "FEATURE_FOUNTAIN_YOUTH")
									gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_DISCOVER_FOUNTAIN);
								else if(strFeatureType == "FEATURE_EL_DORADO")
									gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_02_DISCOVER_EL_DORADO);
							}
						}
#endif

						auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(this));
						gDLL->GameplayNaturalWonderRevealed(pDllPlot.get());
					}
				}
			}
		}

		if(eTeam == eActiveTeam)
		{
			bVisbilityUpdated = true;
			updateSymbols();
			updateFog(true);
			updateVisibility();

			// Active player is seeing this Plot for the first time
			if(isRevealed(eTeam))
			{
				// update the resources
				if(getResourceType(eTeam) != NO_RESOURCE)
				{
					// If we've force revealed the resource for this team, then don't send another event
					if(!IsResourceForceReveal(eTeam))
					{
						setLayoutDirty(true);
					}
				}

				// Found a Goody Hut
				if(getImprovementType() != NO_IMPROVEMENT)
				{
					if(GC.getImprovementInfo(getImprovementType())->IsGoody())
					{
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_GOODY_HUT");
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_GOODY_HUT");
#if !defined(NO_ACHIEVEMENTS)
						if(bEligibleForAchievement)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_ANCIENT_RUIN);
							OutputDebugString("RUINS!");
						}
#endif
						for(iI = 0; iI < MAX_MAJOR_CIVS; ++iI)
						{
							CvPlayerAI& playerI = GET_PLAYER((PlayerTypes)iI);
							if(playerI.isAlive())
							{
								if(playerI.getTeam() == eTeam)
								{
									CvNotifications* pNotifications = playerI.GetNotifications();
									if(pNotifications)
									{
										pNotifications->Add(NOTIFICATION_GOODY, strBuffer, strSummary, getX(), getY(), -1);
									}
								}
							}
						}
					}
				}
#if !defined(NO_ACHIEVEMENTS)
				if(bEligibleForAchievement)
				{
					gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_TILESDISCOVERED, 1000, ACHIEVEMENT_1000TILES);
				}
#endif


			}
		}

		if(bNewValue)
		{
#if defined(MOD_BALANCE_CORE)
			if (pUnit && (pUnit->IsGainsXPFromScouting() || pUnit->IsGainsYieldFromScouting()) && !GET_TEAM(eTeam).isBarbarian() && !GET_TEAM(eTeam).isMinorCiv())
			{
				if(IsNaturalWonder())
				{
					pUnit->ChangeNumTilesRevealedThisTurn(GC.getBALANCE_SCOUT_XP_RANDOM_VALUE());
				}
				else if(isGoody())
				{
					pUnit->ChangeNumTilesRevealedThisTurn(GC.getBALANCE_SCOUT_XP_RANDOM_VALUE() / 3);
				}
				else if(getResourceType(pUnit->getTeam()) != NO_RESOURCE)
				{
					pUnit->ChangeNumTilesRevealedThisTurn(GC.getBALANCE_SCOUT_XP_RANDOM_VALUE() / 4);
				}
				else
				{
					pUnit->ChangeNumTilesRevealedThisTurn(1);
				}	
			}
#endif

			if(pInterface->GetHeadSelectedUnit() != NULL)
			{
				// This is what determines if the camera jumps quickly or slowly - if we're revealing new plots go slower.  The following function sets this flag
				pInterface->SetSelectedUnitRevealingNewPlots(true);
			}
		}
	}

	if(!bTerrainOnly)
	{
		bool bVisibilityChanged = false;
		bool bImprovementVisibilityChanged = false;
		if(isRevealed(eTeam))
		{
			// If this plot is owned by someone, let the team know
			if(getOwner() != NO_PLAYER)
			{
				bVisibilityChanged |= GET_TEAM(eTeam).SetHasFoundPlayersTerritory(getOwner(), true);
			}

			if(eFromTeam == NO_TEAM)
			{
				bVisibilityChanged |= setRevealedOwner(eTeam, getOwner());
				bImprovementVisibilityChanged |= setRevealedImprovementType(eTeam, getImprovementType());
				bVisibilityChanged |= setRevealedRouteType(eTeam, getRouteType());
			}
			else
			{
				if(getRevealedOwner(eFromTeam) == getOwner())
				{
					bVisibilityChanged |= setRevealedOwner(eTeam, getRevealedOwner(eFromTeam));
				}

				if(getRevealedImprovementType(eFromTeam) == getImprovementType())
				{
					bImprovementVisibilityChanged |= setRevealedImprovementType(eTeam, getRevealedImprovementType(eFromTeam));
				}

				if(getRevealedRouteType(eFromTeam) == getRouteType())
				{
					bVisibilityChanged |= setRevealedRouteType(eTeam, getRevealedRouteType(eFromTeam));
				}
			}
		}
		else
		{
			bVisibilityChanged |= setRevealedOwner(eTeam, NO_PLAYER);
			bImprovementVisibilityChanged |= setRevealedImprovementType(eTeam, NO_IMPROVEMENT);
			bVisibilityChanged |= setRevealedRouteType(eTeam, NO_ROUTE);
		}

		if (!bVisbilityUpdated && (bVisibilityChanged || bImprovementVisibilityChanged))
		{
			if(eTeam == eActiveTeam)
			{
				if (!bImprovementVisibilityChanged)	// Don't bother sending again if the improvement change already sent the message
					updateSymbols();

				updateFog(true);
				updateVisibility();
			}

			bVisbilityUpdated = true;
		}
	}
	
#if defined(MOD_EVENTS_TILE_REVEALED)
	CvTeam& kTeam = GET_TEAM(eTeam);
	
	if (MOD_EVENTS_TILE_REVEALED && bNewValue) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileRevealed, getX(), getY(), eTeam, eFromTeam, (kTeam.isMajorCiv() && iRevealedMajors == 0), (pUnit ? pUnit->getOwner() : NO_PLAYER), (pUnit ? pUnit->GetID() : -1));
	}

	// This is badly named, as it's actually an on/off "revealed to any major" flag, not a counter
	if (getNumMajorCivsRevealed() == 0) {
		if (kTeam.isMajorCiv()) {
			changeNumMajorCivsRevealed(1);
		}
	}
#endif

	return bVisbilityUpdated;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentRevealed(TeamTypes eTeam) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isRevealed(eTeam))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentNonrevealed(TeamTypes eTeam) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
		if(pAdjacentPlot != NULL)
		{
			if(!pAdjacentPlot->isRevealed(eTeam))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlot::getNumAdjacentNonrevealed(TeamTypes eTeam) const
{
	int iCount = 0;

	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int i=0; i<NUM_DIRECTION_TYPES; i++)
	{
		CvPlot* pAdjacentPlot = aPlotsToCheck[i];
		if(pAdjacentPlot != NULL)
		{
			if(!pAdjacentPlot->isRevealed(eTeam))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
ImprovementTypes CvPlot::getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const
{
	if(bDebug && GC.getGame().isDebugMode())
	{
		return getImprovementType();
	}
	else
	{
		CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		return (ImprovementTypes)m_aeRevealedImprovementType[eTeam];
	}
}

//	--------------------------------------------------------------------------------
ImprovementTypes CvPlot::getRevealedImprovementType(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	return (ImprovementTypes)m_aeRevealedImprovementType[eTeam];
}

//	--------------------------------------------------------------------------------
bool CvPlot::setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue < NO_IMPROVEMENT) return false;
	if (eNewValue > NO_IMPROVEMENT && GC.getImprovementInfo(eNewValue) == NULL) return false;
#endif

	ImprovementTypes eOldImprovementType = getRevealedImprovementType(eTeam);
	if(eOldImprovementType != eNewValue)
	{
		m_aeRevealedImprovementType[eTeam] = eNewValue;
		if(eTeam == GC.getGame().getActiveTeam())
		{
			updateSymbols();
			setLayoutDirty(true);
		}
		// Found a Barbarian Camp
		if(eNewValue == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_BARB_CAMP");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_BARB_CAMP");
			GET_TEAM(eTeam).AddNotification(NOTIFICATION_BARBARIAN, strBuffer, strSummary, getX(), getY());
		}
		return true;
	}
	return false;
}


//	--------------------------------------------------------------------------------
RouteTypes CvPlot::getRevealedRouteType(TeamTypes eTeam, bool bDebug) const
{
	if(bDebug && GC.getGame().isDebugMode())
	{
		return getRouteType();
	}
	else
	{
		CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		return (RouteTypes)m_aeRevealedRouteType[eTeam];
	}
}

//	--------------------------------------------------------------------------------
RouteTypes CvPlot::getRevealedRouteType(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	return (RouteTypes)m_aeRevealedRouteType[eTeam];
}

//	--------------------------------------------------------------------------------
bool CvPlot::setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue)
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

#if defined(MOD_BUGFIX_PLOT_VALIDATION)
	if (eNewValue < NO_ROUTE) return false;
	if (eNewValue > NO_ROUTE && GC.getRouteInfo(eNewValue) == NULL) return false;
#endif

	if(getRevealedRouteType(eTeam, false) != eNewValue)
	{
		if(eTeam == GC.getGame().getActiveTeam())
		{
			setLayoutDirty(true);
		}
		m_aeRevealedRouteType[eTeam] = eNewValue;
		return true;
	}
	return false;

}

//	--------------------------------------------------------------------------------
void CvPlot::SilentlyResetAllBuildProgress()
{
	m_buildProgress.clear();
}


//	--------------------------------------------------------------------------------
int CvPlot::getBuildProgress(BuildTypes eBuild) const
{
	map<BuildTypes,int>::const_iterator it = m_buildProgress.find(eBuild);

	if (it != m_buildProgress.end())
		return it->second;

	return 0;
}

//	--------------------------------------------------------------------------------
bool CvPlot::getAnyBuildProgress() const
{
	return !m_buildProgress.empty();
}


//	--------------------------------------------------------------------------------
// Returns true if build finished...
bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer, bool bNewBuild)
{
	CvCity* pCity;
	CvString strBuffer;
	int iProduction;
	bool bFinished = false;
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	CvAssert(pkBuildInfo);

	//This shouldn't happen.
	if(pkBuildInfo == NULL)
		return false;

	if (m_iLastTurnBuildChanged == GC.getGame().getGameTurn() && !bNewBuild)
		return false;

	if(iChange != 0)
	{
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (eImprovement != m_eImprovementTypeUnderConstruction)
			{
				SilentlyResetAllBuildProgress();
				m_eImprovementTypeUnderConstruction = eImprovement;
			}
		}

		m_iLastTurnBuildChanged = GC.getGame().getGameTurn();

		m_buildProgress[eBuild] += iChange;
		CvAssert(getBuildProgress(eBuild) >= 0);

		if(getBuildProgress(eBuild) >= getBuildTime(eBuild, ePlayer))
		{
			m_buildProgress.erase(eBuild);

			// Constructed Improvement
			if (eImprovement != NO_IMPROVEMENT)
			{
				setImprovementType(eImprovement, ePlayer);
				
#if defined(MOD_BUGFIX_MINOR)
				// Building a GP improvement on a resource needs to clear any previous pillaged state
				if (GC.getImprovementInfo(eImprovement)->IsCreatedByGreatPerson()) {
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
					SetImprovementPillaged(false, false);
#else
					SetImprovementPillaged(false);
#endif
				}
#endif

				// Unowned plot, someone has to foot the bill
#if defined(MOD_IMPROVEMENTS_EXTENSIONS)
				if(getOwner() == NO_PLAYER && !(MOD_IMPROVEMENTS_EXTENSIONS && GC.getImprovementInfo(eImprovement)->IsRemovesSelf()))
#else
				if(getOwner() == NO_PLAYER)
#endif
				{
					if(MustPayMaintenanceHere(ePlayer))
					{
						kPlayer.GetTreasury()->ChangeBaseImprovementGoldMaintenance(GC.getImprovementInfo(eImprovement)->GetGoldMaintenance());
					}
					SetPlayerResponsibleForImprovement(ePlayer);
				}

				CvImprovementEntry& newImprovementEntry = *GC.getImprovementInfo(eImprovement);

				// If this improvement removes the underlying resource, do that
				if (newImprovementEntry.IsRemovesResource())
				{
					if (getResourceType() != NO_RESOURCE)
					{
						setResourceType(NO_RESOURCE, 0);
					}
				}

				// If we want to prompt the user about archaeology, let's record that
				if (newImprovementEntry.IsPromptWhenComplete())
				{
					if (GetArchaeologicalRecord().m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS)
					{

						kPlayer.SetNumArchaeologyChoices(kPlayer.GetNumArchaeologyChoices() + 1);
						kPlayer.GetCulture()->AddDigCompletePlot(this);

						if (kPlayer.isHuman())
						{
							CvNotifications* pNotifications;
							Localization::String locString;
							Localization::String locSummary;
							pNotifications = kPlayer.GetNotifications();
							if (pNotifications)
							{
								strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHOOSE_ARCHAEOLOGY");
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_ARCHAEOLOGY");
								pNotifications->Add(NOTIFICATION_CHOOSE_ARCHAEOLOGY, strBuffer, strSummary, getX(), getY(), kPlayer.GetID());
								CancelActivePlayerEndTurn();
							}

#if !defined(NO_ACHIEVEMENTS)
							// Raiders of the Lost Ark achievement
							const char* szCivKey = kPlayer.getCivilizationTypeKey();
							if (getOwner() != NO_PLAYER && !GC.getGame().isNetworkMultiPlayer() && strcmp(szCivKey, "CIVILIZATION_AMERICA") == 0)
							{
								CvPlayer &kPlotOwner = GET_PLAYER(getOwner());
								szCivKey = kPlotOwner.getCivilizationTypeKey();
								if (strcmp(szCivKey, "CIVILIZATION_EGYPT") == 0)
								{
									for (int i = 0; i < MAX_MAJOR_CIVS; i++)
									{
										CvPlayer &kLoopPlayer = GET_PLAYER((PlayerTypes)i);
										if (kLoopPlayer.GetID() != NO_PLAYER && kLoopPlayer.isAlive())
										{
											szCivKey = kLoopPlayer.getCivilizationTypeKey();
											if (strcmp(szCivKey, "CIVILIZATION_GERMANY"))
											{
												CvUnit *pLoopUnit;
												int iUnitLoop;
												for (pLoopUnit = kLoopPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iUnitLoop))
												{
													if (strcmp(pLoopUnit->getUnitInfo().GetType(), "UNIT_ARCHAEOLOGIST") == 0)
													{
														if (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), getX(), getY()) <= 2)
														{
															gDLL->UnlockAchievement(ACHIEVEMENT_XP2_33);
														}
													}
												}
											}
										}
									}
								}
							}
#endif
						}
						else
						{
							ArchaeologyChoiceType eChoice = kPlayer.GetCulture()->GetArchaeologyChoice(this);
							kPlayer.GetCulture()->DoArchaeologyChoice(eChoice);
						}
					}
				}
			}

			// Constructed Route
			if(pkBuildInfo->getRoute() != NO_ROUTE)
			{
				const RouteTypes eRoute = (RouteTypes)pkBuildInfo->getRoute();
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					setRouteType(eRoute);

					// Unowned plot, someone has to foot the bill
					if(getOwner() == NO_PLAYER)
					{
						if(MustPayMaintenanceHere(ePlayer))
						{
							kPlayer.GetTreasury()->ChangeBaseImprovementGoldMaintenance(pkRouteInfo->GetGoldMaintenance());
						}
						SetPlayerResponsibleForRoute(ePlayer);
					}
				}
			}

			// Remove Feature
			if(getFeatureType() != NO_FEATURE)
			{
				if(pkBuildInfo->isFeatureRemove(getFeatureType()))
				{
					CvAssertMsg(ePlayer != NO_PLAYER, "ePlayer should be valid");

					iProduction = getFeatureProduction(eBuild, ePlayer, &pCity);

					if(iProduction > 0)
					{
						pCity->changeFeatureProduction(iProduction);
						if(pCity->getOwner() == GC.getGame().getActivePlayer())
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_CLEARING_FEATURE_RESOURCE", GC.getFeatureInfo(getFeatureType())->GetTextKey(), iProduction, pCity->getNameKey());
							GC.GetEngineUserInterface()->AddCityMessage(0, pCity->GetIDInfo(), pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer);
						}
					}

					setFeatureType(NO_FEATURE);
				}
			}

			// Repairing a Pillaged Tile
			if(pkBuildInfo->isRepair())
			{
				if(IsImprovementPillaged())
				{
					SetImprovementPillaged(false);
				}
				else if(IsRoutePillaged())
				{
					SetRoutePillaged(false);
				}
			}

			if(pkBuildInfo->IsRemoveRoute())
			{
				setRouteType(NO_ROUTE);
			}

			bFinished = true;

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if(pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(ePlayer);
				args->Push(getX());
				args->Push(getY());
				args->Push(eImprovement);

				bool bResult;
				LuaSupport::CallHook(pkScriptSystem, "BuildFinished", args.get(), bResult);
			}
		}
	}

	return bFinished;
}


//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getCenterUnit()
{
	return m_pCenterUnit;
}

//	--------------------------------------------------------------------------------
const CvUnit* CvPlot::getCenterUnit() const
{
	return m_pCenterUnit;
}


//	--------------------------------------------------------------------------------
const CvUnit* CvPlot::getDebugCenterUnit() const
{
	const CvUnit* pCenterUnit = getCenterUnit();

	if(pCenterUnit == NULL)
	{
		if(GC.getGame().isDebugMode())
		{
			const IDInfo* pUnitNode = headUnitNode();
			if(pUnitNode == NULL)
				pCenterUnit = NULL;
			else
				pCenterUnit = GetPlayerUnit(*pUnitNode);
		}
	}

	return pCenterUnit;
}


//	--------------------------------------------------------------------------------
void CvPlot::setCenterUnit(CvUnit* pNewValue)
{
	CvUnit* pOldValue = getCenterUnit();
	m_pCenterUnit = pNewValue;

	if(pOldValue != pNewValue)
	{
		CvUnit* newCenterUnit = getCenterUnit();
		if(newCenterUnit)
		{
			newCenterUnit->setInfoBarDirty(true);
		}
	}
}
int CvPlot::getInvisibleVisibilityCountUnit(TeamTypes eTeam) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_paiInvisibleVisibilityUnitCount[eTeam];
}
bool CvPlot::isInvisibleVisibleUnit(TeamTypes eTeam) const
{
	return (getInvisibleVisibilityCountUnit(eTeam) > 0);
}
void CvPlot::changeInvisibleVisibilityCountUnit(TeamTypes eTeam, int iChange)
{
	bool bOldInvisibleVisible;
	bool bNewInvisibleVisible;

	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	if (eTeam < 0 || eTeam >= MAX_TEAMS) return;

	if (iChange != 0)
	{
		bOldInvisibleVisible = isInvisibleVisibleUnit(eTeam);

		m_paiInvisibleVisibilityUnitCount[eTeam] = (m_paiInvisibleVisibilityUnitCount[eTeam] + iChange);

		CvAssertFmt(m_apaiInvisibleVisibilityCount[eTeam] >= 0, "Invisible Visibility going negative for %d, %d", m_iX, m_iY);

		bNewInvisibleVisible = isInvisibleVisibleUnit(eTeam);
		if (bOldInvisibleVisible != bNewInvisibleVisible)
		{
			TeamTypes activeTeam = GC.getGame().getActiveTeam();
			if (eTeam == activeTeam)
			{
				// for all (nominally invisible) units in this plot
				// tell the engine to flip whether they are being drawn or not
				IDInfo* pUnitNode;
				CvUnit* pLoopUnit = NULL;
				pUnitNode = headUnitNode();
				while (pUnitNode != NULL)
				{
					pLoopUnit = GetPlayerUnit(*pUnitNode);
					pUnitNode = nextUnitNode(pUnitNode);

					if (NULL != pLoopUnit && pLoopUnit->getTeam() != activeTeam && pLoopUnit->IsHiddenByNearbyUnit(pLoopUnit->plot()))
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitVisibility(pDllUnit.get(), bNewInvisibleVisible, true);
					}
				}

				updateCenterUnit();

			}
		}
	}
}
//	--------------------------------------------------------------------------------
int CvPlot::getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const
{
	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eInvisible >= 0, "eInvisible is expected to be non-negative (invalid Index)");
	CvAssertMsg(eInvisible < NUM_INVISIBLE_TYPES, "eInvisible is expected to be within maximum bounds (invalid Index)");
	if (eTeam < 0 || eTeam >= MAX_TEAMS) return 0;
	if (eInvisible < 0 || eInvisible >= NUM_INVISIBLE_TYPES) return 0;
	return m_apaiInvisibleVisibilityCount[eTeam][eInvisible];
}


//	--------------------------------------------------------------------------------
bool CvPlot::isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible)	const
{
	return (getInvisibleVisibilityCount(eTeam, eInvisible) > 0);
}


//	--------------------------------------------------------------------------------
void CvPlot::changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange)
{
	bool bOldInvisibleVisible;
	bool bNewInvisibleVisible;
	const int iNumInvisibleInfos = NUM_INVISIBLE_TYPES;
	DEBUG_VARIABLE(iNumInvisibleInfos);

	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(eInvisible >= 0, "eInvisible is expected to be non-negative (invalid Index)");
	CvAssertMsg(eInvisible < iNumInvisibleInfos, "eInvisible is expected to be within maximum bounds (invalid Index)");
	if (eTeam < 0 || eTeam >= MAX_TEAMS) return;
	if (eInvisible < 0 || eInvisible >= NUM_INVISIBLE_TYPES) return;

	if (iChange != 0)
	{
		bOldInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);

		m_apaiInvisibleVisibilityCount[eTeam][eInvisible] = (m_apaiInvisibleVisibilityCount[eTeam][eInvisible] + iChange);

		CvAssertFmt(m_apaiInvisibleVisibilityCount[eTeam][eInvisible] >= 0, "Invisible Visibility going negative for %d, %d", m_iX, m_iY);

		bNewInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);

		if (bOldInvisibleVisible != bNewInvisibleVisible)
		{
			TeamTypes activeTeam = GC.getGame().getActiveTeam();
			if (eTeam == activeTeam)
			{
				// for all (nominally invisible) units in this plot
				// tell the engine to flip whether they are being drawn or not
				IDInfo* pUnitNode;
				CvUnit* pLoopUnit = NULL;
				pUnitNode = headUnitNode();
				while (pUnitNode != NULL)
				{
					pLoopUnit = GetPlayerUnit(*pUnitNode);
					pUnitNode = nextUnitNode(pUnitNode);

					if (NULL != pLoopUnit && pLoopUnit->getTeam() != activeTeam && pLoopUnit->getInvisibleType() == eInvisible)
					{
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitVisibility(pDllUnit.get(), bNewInvisibleVisible, true);
					}
				}

				updateCenterUnit();

			}
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlot::getNumLayerUnits(int iLayerID /*= -1*/) const
{
	if (iLayerID == DEFAULT_UNIT_MAP_LAYER)
		// Base layer
		return m_units.getLength();
	else
		if (iLayerID == -1)
		{
			// All layers
			int iCount = m_units.getLength();
			const CvPlotManager& kManager = GC.getMap().plotManager();
			int iNumLayers = kManager.GetNumLayers();
			for (int iIndex = 0; iIndex < iNumLayers; ++iIndex)
			{		
				iCount += kManager.GetNumUnitsByIndex(m_iX, m_iY, iIndex);
			}
			return iCount;
		}
		else
		{
			// Specific layer
			return GC.getMap().plotManager().GetNumUnits(m_iX, m_iY, iLayerID);
		}
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getLayerUnit(int iIndex, int iLayerID /*= -1*/) const
{
	if (iIndex >= 0)
	{
		if (iLayerID == DEFAULT_UNIT_MAP_LAYER)
			// The base layer
			return getUnitByIndex(iIndex);
		else
		if (iLayerID == -1)
		{
			// All layers
			int iCount = m_units.getLength();
			if (iIndex < iCount)
				return getUnitByIndex(iIndex);
			else
			{
				const CvPlotManager& kManager = GC.getMap().plotManager();
				int iNumLayers = kManager.GetNumLayers();
				for (int iLayerIndex = 0; iLayerIndex < iNumLayers; ++iLayerIndex)
				{		
					const CvIDInfoFixedVector& kUnits = kManager.GetUnitsByIndex(m_iX, m_iY, iLayerIndex);
					if (iIndex < (iCount + (int)kUnits.size()))
						return ::GetPlayerUnit( kUnits[iIndex - iCount] );
					else
						iCount += kUnits.size();
				}				
			}
		}
		else
		{
			// Specific layer
			const CvIDInfoFixedVector& kUnits = GC.getMap().plotManager().GetUnits(m_iX, m_iY, iLayerID);
			if (iIndex < (int)kUnits.size())
				return ::GetPlayerUnit( kUnits[iIndex] );
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
int CvPlot::getNumUnits() const
{
	return m_units.getLength();
}
#if defined(MOD_BALANCE_CORE)
int CvPlot::GetUnitPlotExperience() const
{
	VALIDATE_OBJECT
	return m_iUnitPlotExperience;
}
void CvPlot::ChangeUnitPlotExperience(int iExperience)
{
	VALIDATE_OBJECT
	m_iUnitPlotExperience += iExperience;
}
int CvPlot::GetUnitPlotGAExperience() const
{
	VALIDATE_OBJECT
	return m_iUnitPlotGAExperience;
}
void CvPlot::ChangeUnitPlotGAExperience(int iExperience)
{
	VALIDATE_OBJECT
	m_iUnitPlotGAExperience += iExperience;
}
bool CvPlot::IsUnitPlotExperience() const
{
	if (GetUnitPlotExperience() > 0 || GetUnitPlotGAExperience() > 0)
	{
		return true;
	}
	return false;
}
int CvPlot::GetPlotMovesChange() const
{
	VALIDATE_OBJECT
	return m_iPlotChangeMoves;
}
void CvPlot::ChangePlotMovesChange(int iValue)
{
	VALIDATE_OBJECT
	m_iPlotChangeMoves += iValue;
}
#endif
//	--------------------------------------------------------------------------------
int CvPlot::GetNumCombatUnits()
{
	int iCount = 0;

	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);

		if(pLoopUnit && pLoopUnit->IsCanAttack())
		{
			iCount++;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getUnitByIndex(int iIndex) const
{
	const IDInfo* pUnitNode = m_units.getAt(iIndex);

	if(pUnitNode != NULL)
	{
		return GetPlayerUnit(*pUnitNode);
	}
	else
	{
		return NULL;
	}
}

//	---------------------------------------------------------------------------
//	Return the index of the unit in the plot's list.  -1 if it is not in the list.
int CvPlot::getUnitIndex(CvUnit* pUnit) const
{
	int iIndex = 0;
	if(pUnit)
	{
		IDInfo kUnitInfo = pUnit->GetIDInfo();

		const IDInfo* pUnitNode = headUnitNode();

		while(pUnitNode != NULL)
		{
			if(*pUnitNode == kUnitInfo)
				return iIndex;

			++iIndex;
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}
	return -1;
}

//	---------------------------------------------------------------------------
void CvPlot::addUnit(CvUnit* pUnit, bool bUpdate)
{
	CvAssertMsg(pUnit, "pUnit is expected to be non-NULL");
	if(pUnit == NULL)
		return;

	CvAssertMsg(pUnit->at(getX(), getY()), "pUnit is expected to be at getX_INLINE and getY_INLINE");

	IDInfo* pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		if(!isBeforeUnitCycle(pLoopUnit, pUnit))
		{
			break;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	if(pUnitNode != NULL)
	{
		IDInfo unitIDInfo = pUnit->GetIDInfo();
		m_units.insertBefore(&unitIDInfo, pUnitNode);
	}
	else
	{
		IDInfo unitIDInfo = pUnit->GetIDInfo();
		m_units.insertAtEnd(&unitIDInfo);
	}

	if(bUpdate)
	{
		updateCenterUnit();
	}
}


//	--------------------------------------------------------------------------------
void CvPlot::removeUnit(CvUnit* pUnit, bool bUpdate)
{
	IDInfo* pUnitNode;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		if(GetPlayerUnit(*pUnitNode) == pUnit)
		{
			CvAssertMsg(GetPlayerUnit(*pUnitNode)->at(getX(), getY()), "The current unit instance is expected to be at getX_INLINE and getY_INLINE");
			m_units.deleteNode(pUnitNode);
			break;
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	GC.getMap().plotManager().RemoveUnit(pUnit->GetIDInfo(), m_iX, m_iY, -1);

	if(bUpdate)
	{
		updateCenterUnit();
	}
}

//	--------------------------------------------------------------------------------
const IDInfo* CvPlot::nextUnitNode(const IDInfo* pNode) const
{
	return m_units.next(pNode);
}

//	--------------------------------------------------------------------------------
IDInfo* CvPlot::nextUnitNode(IDInfo* pNode)
{
	return m_units.next(pNode);
}

//	--------------------------------------------------------------------------------
const IDInfo* CvPlot::prevUnitNode(const IDInfo* pNode) const
{
	return m_units.prev(pNode);
}

//	--------------------------------------------------------------------------------
IDInfo* CvPlot::prevUnitNode(IDInfo* pNode)
{
	return m_units.prev(pNode);
}

//	--------------------------------------------------------------------------------
const IDInfo* CvPlot::headUnitNode() const
{
	return m_units.head();
}

//	--------------------------------------------------------------------------------
IDInfo* CvPlot::headUnitNode()
{
	return m_units.head();
}

//	--------------------------------------------------------------------------------
const IDInfo* CvPlot::tailUnitNode() const
{
	return m_units.tail();
}

//	--------------------------------------------------------------------------------
IDInfo* CvPlot::tailUnitNode()
{
	return m_units.tail();
}

//	--------------------------------------------------------------------------------
uint CvPlot::getUnits(IDInfoVector* pkInfoVector) const
{
	uint uiCount = 0;
	if (pkInfoVector)
	{
		pkInfoVector->clear();
	
		const IDInfo* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pkInfoVector->push_back(*pUnitNode);
			pUnitNode = nextUnitNode(pUnitNode);
			++uiCount;
		}
	}
	return uiCount;
}

//	--------------------------------------------------------------------------------
CvString CvPlot::getScriptData() const
{
	CvString scriptData = (m_szScriptData != NULL) ? m_szScriptData : "";
	return scriptData;
}

//	--------------------------------------------------------------------------------
void CvPlot::setScriptData(const char* szNewValue)
{
	SAFE_DELETE_ARRAY(m_szScriptData);
	m_szScriptData = _strdup(szNewValue);
}

void CvPlot::showPopupText(PlayerTypes ePlayer, const char* szMessage)
{
	if (ePlayer == NO_PLAYER || isVisible(GET_PLAYER(ePlayer).getTeam()))
	{
		//show the popup only if we're not on autoplay - too many stored popups seems to lead to crashes
		if (GC.getGame().getAIAutoPlay() < 10)
		{
			DLLUI->AddPopupText(getX(), getY(), szMessage, GC.getMap().GetPopupCount(m_iPlotIndex)*0.5f);
			GC.getMap().IncreasePopupCount(m_iPlotIndex);
		}
	}
}

// Protected Functions...

//	--------------------------------------------------------------------------------
void CvPlot::processArea(CvArea* pArea, int iChange)
{
	CvCity* pCity;
	int iI, iJ;

	pArea->changeNumTiles(iChange);

	if(isOwned())
	{
		pArea->changeNumOwnedTiles(iChange);
	}

	if(isNEOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}
	if(isNWOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}
	if(isWOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}

	if(getResourceType() != NO_RESOURCE)
	{
		pArea->changeNumResources(getResourceType(), iChange);
	}

	if(getImprovementType() != NO_IMPROVEMENT)
	{
		pArea->changeNumImprovements(getImprovementType(), iChange);
	}

	for(iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if(GET_PLAYER((PlayerTypes)iI).getStartingPlot() == this)
		{
			pArea->changeNumStartingPlots(iChange);
		}

		pArea->changeUnitsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isPlayer, iI) * iChange));
	}

	for(iI = 0; iI < MAX_TEAMS; ++iI)
	{
		if(isRevealed(((TeamTypes)iI)))
		{
			pArea->changeNumRevealedTiles(((TeamTypes)iI), iChange);
		}
	}

	pCity = getPlotCity();

	if(pCity != NULL)
	{
		// XXX make sure all of this syncs up...
		pArea->changeCitiesPerPlayer(pCity->getOwner(), iChange);
		pArea->changePopulationPerPlayer(pCity->getOwner(), (pCity->getPopulation() * iChange));

		for(iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				if(pCity->GetCityBuildings()->GetNumActiveBuilding(eBuilding) > 0)
				{
					for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						//Simplification - errata yields not worth considering.
						if ((YieldTypes)iJ > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
							break;

						pArea->changeYieldRateModifier(pCity->getOwner(), ((YieldTypes)iJ), (pkBuildingInfo->GetAreaYieldModifier(iJ) * iChange * pCity->GetCityBuildings()->GetNumActiveBuilding(eBuilding)));
					}
				}
			}
		}

		for(iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if(pArea->getTargetCity((PlayerTypes)iI) == pCity)
			{
				pArea->setTargetCity(((PlayerTypes)iI), NULL);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
//
// read object from a stream
// used during load
//
void CvPlot::read(FDataStream& kStream)
{
	// Init saved data
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	CvAssertMsg(uiVersion <= g_CurrentCvPlotVersion, "Unexpected Version.  This could be caused by serialization errors.");
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iX;
	kStream >> m_iY;
	m_iPlotIndex = GC.getMap().plotNum(m_iX,m_iY);

	kStream >> m_iArea;
	kStream >> m_iDummy;
	kStream >> m_iOwnershipDuration;
	kStream >> m_iImprovementDuration;
	kStream >> m_iUpgradeProgress;
	kStream >> m_iUpgradeProgress; //dummy
	kStream >> m_iNumMajorCivsRevealed;
	kStream >> m_iCityRadiusCount;
	kStream >> m_iReconCount;
	kStream >> m_iRiverCrossingCount;
	kStream >> m_iResourceNum;
	kStream >> m_iLandmass;

	// the following members specify bit packing and do not resolve to
	// any serializable type.
	bool bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bStartingPlot = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bHills = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bNEOfRiver = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bWOfRiver = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bNWOfRiver = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bPotentialCityWork = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bImprovementPassable = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bImprovementPillaged = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bRoutePillaged = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bBarbCampNotConverting = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bHighMoveCost = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bResourceLinkedCityActive = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bImprovedByGiftFromMajor = bitPackWorkaround;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	MOD_SERIALIZE_READ(49, kStream, bitPackWorkaround, 0);
	m_bImprovementEmbassy = bitPackWorkaround;
#endif
#if defined(MOD_BALANCE_CORE)
	kStream >> bitPackWorkaround;
	m_bIsImpassable = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_iUnitPlotExperience = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_iUnitPlotGAExperience = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_iPlotChangeMoves = bitPackWorkaround;
#endif

	kStream >> m_eOwner;
	kStream >> m_ePlotType;
	kStream >> m_eTerrainType;
	if (uiVersion >= 3)
		m_eFeatureType = (FeatureTypes) CvInfosSerializationHelper::ReadHashed(kStream);
	else
	{
		kStream >> m_eFeatureType;
		if ((int)m_eFeatureType >= GC.getNumFeatureInfos())
			m_eFeatureType = NO_FEATURE;
	}
	m_eResourceType = (ResourceTypes) CvInfosSerializationHelper::ReadHashed(kStream);

	if(uiVersion >= 5)
	{
		m_eImprovementType = (ImprovementTypes) CvInfosSerializationHelper::ReadHashed(kStream);
	}
	else
	{
		kStream >> m_eImprovementType;
		// Filter out improvements that have been removed
		if (m_eImprovementType != NO_IMPROVEMENT)
			if (GC.getImprovementInfo((ImprovementTypes)m_eImprovementType) == NULL)
				m_eImprovementType = NO_IMPROVEMENT;
	}

	if (uiVersion >= 7)
	{
		m_eImprovementTypeUnderConstruction = (ImprovementTypes)CvInfosSerializationHelper::ReadHashed(kStream);
	}
	else
	{
		m_eImprovementTypeUnderConstruction = NO_IMPROVEMENT;
	}

	if (uiVersion >= 2)
	{
		kStream >> m_ePlayerBuiltImprovement;
	}
	else
	{
		m_ePlayerBuiltImprovement = NO_PLAYER;
	}
	kStream >> m_ePlayerResponsibleForImprovement;
	kStream >> m_ePlayerResponsibleForRoute;
	kStream >> m_ePlayerThatClearedBarbCampHere;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	MOD_SERIALIZE_READ(39, kStream, m_ePlayerThatClearedDigHere, NO_PLAYER);
#endif
	kStream >> m_eRouteType;
#if defined(MOD_GLOBAL_STACKING_RULES)
	MOD_SERIALIZE_READ(30, kStream, m_eUnitIncrement, 0);
#endif
	kStream >> m_eWorldAnchor;
	kStream >> m_cWorldAnchorData;

	kStream >> m_eRiverEFlowDirection;
	kStream >> m_eRiverSEFlowDirection;
	kStream >> m_eRiverSWFlowDirection;

	kStream >> m_plotCity.eOwner;
	kStream >> m_plotCity.iID;
	kStream >> m_owningCity.eOwner;
	kStream >> m_owningCity.iID;
	kStream >> m_owningCityOverride.eOwner;
	kStream >> m_owningCityOverride.iID;
	kStream >> m_ResourceLinkedCity.eOwner;
	kStream >> m_ResourceLinkedCity.iID;
	kStream >> m_purchaseCity.eOwner;
	kStream >> m_purchaseCity.iID;

    for(uint i = 0; i < NUM_YIELD_TYPES; i++)
    {
        kStream >> m_aiYield[i];
#if defined(MOD_API_EXTENSIONS)
        kStream >> m_aiArbitraryYields[i];
#endif
    }

	for(int i = 0; i < MAX_TEAMS; i++)
	{
		kStream >> m_aiPlayerCityRadiusCount[i];
		kStream >> m_aiVisibilityCount[i];
		kStream >> m_aiVisibilityCountThisTurnMax[i];
		kStream >> m_aiRevealedOwner[i];
		kStream >> m_abResourceForceReveal[i];
		m_aeRevealedImprovementType[i] = (ImprovementTypes) CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> m_aeRevealedRouteType[i];
#if defined(MOD_BALANCE_CORE)
		kStream >> m_abIsImpassable[i];
		kStream >> m_abStrategicRoute[i];
#endif
	}

	for (uint i = 0; i<PlotBoolField::eCount; ++i)
	{
		kStream >> m_bfRevealed.m_dwBits[i];
	}
	kStream >> m_cRiverCrossing;

	bool hasScriptData = false;
	kStream >> hasScriptData;
	if(hasScriptData)
	{
		std::string scriptData;
		kStream >> scriptData;
		setScriptData(scriptData.c_str());
	}


	kStream >> m_buildProgress;
	kStream >> m_apaiInvisibleVisibilityCount;
	kStream >> m_paiInvisibleVisibilityUnitCount;

	//m_units.Read(kStream);
	UINT uLength;
	kStream >> uLength;
	for(UINT uIdx = 0; uIdx < uLength; ++uIdx)
	{
		IDInfo  Data;

		kStream >> Data.eOwner;
		kStream >> Data.iID;

		m_units.insertAtEnd(&Data);
	}

	kStream >> m_cContinentType;
	kStream >> m_kArchaeologyData;
#if defined(MOD_BALANCE_CORE)
	kStream >> m_bIsTradeUnitRoute;
	kStream >> m_iLastTurnBuildChanged;
#endif
}

//	--------------------------------------------------------------------------------
//
// write object to a stream
// used during save
//
void CvPlot::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = g_CurrentCvPlotVersion;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iX;
	kStream << m_iY;
	kStream << m_iArea;
	kStream << m_iDummy;
	kStream << m_iOwnershipDuration;
	kStream << m_iImprovementDuration;
	kStream << m_iUpgradeProgress;
	kStream << m_iUpgradeProgress; //dummy
	kStream << m_iNumMajorCivsRevealed;
	kStream << m_iCityRadiusCount;
	kStream << m_iReconCount;
	kStream << m_iRiverCrossingCount;
	kStream << m_iResourceNum;
	kStream << m_iLandmass;

	kStream << m_bStartingPlot;
	kStream << m_bHills;
	kStream << m_bNEOfRiver;
	kStream << m_bWOfRiver;
	kStream << m_bNWOfRiver;
	kStream << m_bPotentialCityWork;
	kStream << m_bImprovementPassable;
	kStream << m_bImprovementPillaged;
	kStream << m_bRoutePillaged;
	kStream << m_bBarbCampNotConverting;
	kStream << m_bHighMoveCost;
	kStream << m_bResourceLinkedCityActive;
	kStream << m_bImprovedByGiftFromMajor;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	MOD_SERIALIZE_WRITE(kStream, m_bImprovementEmbassy);
#endif
#if defined(MOD_BALANCE_CORE)
	kStream << m_bIsImpassable;
	kStream << m_iUnitPlotExperience;
	kStream << m_iUnitPlotGAExperience;
	kStream << m_iPlotChangeMoves;
#endif
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved

	kStream << m_eOwner;
	kStream << m_ePlotType;
	kStream << m_eTerrainType;

	CvInfosSerializationHelper::WriteHashed(kStream, (const FeatureTypes)m_eFeatureType.get());
	CvInfosSerializationHelper::WriteHashed(kStream, (const ResourceTypes)m_eResourceType);
	CvInfosSerializationHelper::WriteHashed(kStream, (const ImprovementTypes)m_eImprovementType);
	CvInfosSerializationHelper::WriteHashed(kStream, (const ImprovementTypes)m_eImprovementTypeUnderConstruction);
	kStream << m_ePlayerBuiltImprovement;
	kStream << m_ePlayerResponsibleForImprovement;
	kStream << m_ePlayerResponsibleForRoute;
	kStream << m_ePlayerThatClearedBarbCampHere;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	MOD_SERIALIZE_WRITE(kStream, m_ePlayerThatClearedDigHere);
#endif
	kStream << m_eRouteType;
#if defined(MOD_GLOBAL_STACKING_RULES)
	MOD_SERIALIZE_WRITE(kStream, m_eUnitIncrement);
#endif
	kStream << m_eWorldAnchor;
	kStream << m_cWorldAnchorData;
	kStream << m_eRiverEFlowDirection;
	kStream << m_eRiverSEFlowDirection;
	kStream << m_eRiverSWFlowDirection;

	kStream << m_plotCity.eOwner;
	kStream << m_plotCity.iID;
	kStream << m_owningCity.eOwner;
	kStream << m_owningCity.iID;
	kStream << m_owningCityOverride.eOwner;
	kStream << m_owningCityOverride.iID;
	kStream << m_ResourceLinkedCity.eOwner;
	kStream << m_ResourceLinkedCity.iID;
	kStream << m_purchaseCity.eOwner;
	kStream << m_purchaseCity.iID;

    for(uint i = 0; i < NUM_YIELD_TYPES; i++)
    {
        kStream << m_aiYield[i];
#if defined(MOD_API_EXTENSIONS)
        kStream << m_aiArbitraryYields[i];
#endif
    }

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		kStream << m_aiPlayerCityRadiusCount[i];
		kStream << m_aiVisibilityCount[i];
		kStream << m_aiVisibilityCountThisTurnMax[i];
		kStream << m_aiRevealedOwner[i];
		kStream << m_abResourceForceReveal[i];
		CvInfosSerializationHelper::WriteHashed(kStream, (const ImprovementTypes)m_aeRevealedImprovementType[i]);
		kStream << m_aeRevealedRouteType[i];
#if defined(MOD_BALANCE_CORE)
		kStream << m_abIsImpassable[i];
		kStream << m_abStrategicRoute[i];
#endif
	}

	for (uint i = 0; i<PlotBoolField::eCount; ++i)
	{
		kStream << m_bfRevealed.m_dwBits[i];
	}
	kStream << m_cRiverCrossing;

	// char * should have died in 1989...
	bool hasScriptData = (m_szScriptData != NULL);
	kStream << hasScriptData;
	if(hasScriptData)
	{
		const std::string scriptData(m_szScriptData);
		kStream << scriptData;
	}


	kStream << m_buildProgress;
	kStream << m_apaiInvisibleVisibilityCount;
	kStream << m_paiInvisibleVisibilityUnitCount;

	//  Write m_units.Write(kStream);
	UINT uLength = (UINT)m_units.getLength();
	kStream << uLength;
	for(UINT uIdx = 0; uIdx < uLength; ++uIdx)
	{
		const IDInfo* pData = m_units.getAt(uIdx);

		kStream << pData->eOwner;
		kStream << pData->iID;
	}

	kStream << m_cContinentType;
	kStream << m_kArchaeologyData;
#if defined(MOD_BALANCE_CORE)
	kStream << m_bIsTradeUnitRoute;
	kStream << m_iLastTurnBuildChanged;
#endif
}

//	--------------------------------------------------------------------------------
void CvPlot::setLayoutDirty(bool bDirty)
{
	if(bDirty == true)
	{
		GC.GetEngineUserInterface()->setDirty(PlotData_DIRTY_BIT,true);
	}
	m_bPlotLayoutDirty = bDirty;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isLayoutDirty() const
{
	return m_bPlotLayoutDirty;
}

enum eRoadTypes
{
    ROAD_REGULAR = 0,
    RR_REGULAR,

    ROAD_PILLAGED,
    RR_PILLAGED,

    ROAD_UNDER_CONSTRUCTION,
    RR_UNDER_CONSTRUCTION,

    ROAD_TRADE_ROUTE,
    RR_TRADE_ROUTE,

    NUM_ROAD_RR_TYPES,
};

//	--------------------------------------------------------------------------------
void CvPlot::updateLayout(bool bDebug)
{
	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();

	FogOfWarModeTypes eFOWMode = GetActiveFogOfWarMode();

	ResourceTypes eThisResource = (isCity() || eFOWMode == FOGOFWARMODE_UNEXPLORED) ? NO_RESOURCE : getResourceType(eActiveTeam);

	ImprovementTypes eThisImprovement = getRevealedImprovementType(eActiveTeam, bDebug);
	bool bShowHalfBuilt = false;
	if(eThisImprovement == NO_IMPROVEMENT && getAnyBuildProgress() && eFOWMode == FOGOFWARMODE_OFF)
	{
		// see if we are improving the tile
		for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
		{
			BuildTypes eBuild = (BuildTypes)iBuildIndex;
			CvBuildInfo* build = GC.getBuildInfo(eBuild);
			if(build)
			{
				ImprovementTypes eInnerImprovement = (ImprovementTypes)build->getImprovement();
				if(eInnerImprovement == NO_IMPROVEMENT)
				{
					continue;
				}

				int iProgress = getBuildProgress(eBuild);
				if(iProgress > 0)
				{
					eThisImprovement = eInnerImprovement;
					bShowHalfBuilt = true;
					break;
				}
			}

		}
	}

	int improvementState = 2; // half-built = 1, full built = 2, pillaged = 3
	if(IsImprovementPillaged() && eFOWMode == FOGOFWARMODE_OFF)
	{
		improvementState = 3;
	}
	else if(bShowHalfBuilt)
	{
		improvementState = 1;
	}

	RouteTypes eRoute = getRevealedRouteType(eActiveTeam, bDebug);
	byte eRoadTypeValue = NUM_ROAD_RR_TYPES;
	if(eRoute != NO_ROUTE)
	{
		switch(eRoute)
		{
		case ROUTE_ROAD:
			if(IsRoutePillaged())
			{
				eRoadTypeValue = ROAD_PILLAGED;
			}
			else if(IsCityConnection())
			{
				eRoadTypeValue = ROAD_TRADE_ROUTE;
			}
			else
			{
				eRoadTypeValue = ROAD_REGULAR;
			}
			break;
		case ROUTE_RAILROAD:
			if(IsRoutePillaged())
			{
				eRoadTypeValue = RR_PILLAGED;
			}
			else if(IsCityConnection())
			{
				eRoadTypeValue = RR_TRADE_ROUTE;
			}
			else
			{
				eRoadTypeValue = RR_REGULAR;
			}
			break;
		}
	}
	else
	{
		bShowHalfBuilt = false;
		if(eRoute == NO_ROUTE && getAnyBuildProgress() && eFOWMode == FOGOFWARMODE_OFF)
		{
			// see if we are improving the tile
			for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
			{
				BuildTypes eBuild = (BuildTypes)iBuildIndex;
				CvBuildInfo* build = GC.getBuildInfo(eBuild);
				if(build)
				{
					RouteTypes eInnerRoute = (RouteTypes)build->getRoute();
					if(eInnerRoute == NO_ROUTE)
					{
						continue;
					}

					int iProgress = getBuildProgress(eBuild);
					if(iProgress > 0)
					{
						eRoute = eInnerRoute;
						bShowHalfBuilt = true;
						break;
					}
				}

			}
		}
		if(bShowHalfBuilt)
		{
			switch(eRoute)
			{
			case ROUTE_ROAD:
				eRoadTypeValue = ROAD_UNDER_CONSTRUCTION;
				break;
			case ROUTE_RAILROAD:
				eRoadTypeValue = RR_UNDER_CONSTRUCTION;
				break;
			}
		}
	}

	auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(this));
	gDLL->GameplayPlotStateChange
	(
	    pDllPlot.get(),
	    eThisResource,
	    eThisImprovement,
	    improvementState,
	    eRoute,
	    eRoadTypeValue
	);
}

//	--------------------------------------------------------------------------------
bool CvPlot::isLayoutStateDifferent() const
{
	return (m_bLayoutStateWorked != isBeingWorked());
}

//	--------------------------------------------------------------------------------
void CvPlot::setLayoutStateToCurrent()
{
	m_bLayoutStateWorked = isBeingWorked();
}

//------------------------------------------------------------------------------------------------

void CvPlot::getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked)
{
	eType = NO_IMPROVEMENT;
	bWorked = false;

	if(GC.getGame().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	eType = getRevealedImprovementType(GC.getGame().getActiveTeam(), true);

	// worked state
	if(isActiveVisible(false) && isBeingWorked())
	{
		bWorked = true;
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::getVisibleResourceState(ResourceTypes& eType, bool& bImproved, bool& bWorked)
{
	eType = NO_RESOURCE;
	bImproved = false;
	bWorked = false;

	if(GC.getGame().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	if(GC.getGame().isDebugMode())
	{
		eType = getResourceType();
	}
	else if(isRevealed(GC.getGame().getActiveTeam()))
	{
		eType = getResourceType(GC.getGame().getActiveTeam());
	}

	// improved and worked states ...
	if(eType != NO_RESOURCE)
	{
		ImprovementTypes eRevealedImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam(), true);
#if defined(MOD_BALANCE_CORE)
		if ((eRevealedImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eRevealedImprovement)->IsExpandedImprovementResourceTrade(eType))
#else
		if((eRevealedImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eRevealedImprovement)->IsImprovementResourceTrade(eType))
#endif
		{
			bImproved = true;
			bWorked = isBeingWorked();
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlot::getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade, PlayerTypes ePlayer, const CvCity* pOwningCity, const CvReligion* pMajorityReligion, const CvBeliefEntry* pSecondaryPantheon) const
{
	int iYield;

	if(getTerrainType() == NO_TERRAIN)
		return 0;

	if(!isPotentialCityWork())
		return 0;

	// Will the build remove the feature?
	bool bIgnoreFeature = false;
	if(getFeatureType() != NO_FEATURE)
	{
		if(GC.getBuildInfo(eBuild)->isFeatureRemove(getFeatureType()))
			bIgnoreFeature = true;
	}

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();

	// Nature yield
	iYield = calculateNatureYield(eYield, ePlayer, pOwningCity, bIgnoreFeature);
	iYield += calculateReligionNatureYield(eYield, ePlayer, pOwningCity, pMajorityReligion, pSecondaryPantheon);

	// If we're not changing the improvement that's here, use the improvement that's here already
	if(eImprovement == NO_IMPROVEMENT)
	{
		if(!IsImprovementPillaged() || GC.getBuildInfo(eBuild)->isRepair())
		{
			eImprovement = getImprovementType();
		}
	}

	if(eImprovement != NO_IMPROVEMENT)
	{
		if(bWithUpgrade)
		{
			//in the case that improvements upgrade, use 2 upgrade levels higher for the
			//yield calculations.
			ImprovementTypes eUpgradeImprovement = (ImprovementTypes)GC.getImprovementInfo(eImprovement)->GetImprovementUpgrade();
			if(eUpgradeImprovement != NO_IMPROVEMENT)
			{
				//unless it's trade on a low food tile, in which case only use 1 level higher
				if((eYield != YIELD_GOLD) || (getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION()))
				{
					ImprovementTypes eUpgradeImprovement2 = (ImprovementTypes)GC.getImprovementInfo(eUpgradeImprovement)->GetImprovementUpgrade();
					if(eUpgradeImprovement2 != NO_IMPROVEMENT)
					{
						eUpgradeImprovement = eUpgradeImprovement2;
					}
				}
			}

			if((eUpgradeImprovement != NO_IMPROVEMENT) && (eUpgradeImprovement != eImprovement))
			{
				eImprovement = eUpgradeImprovement;
			}
		}

		iYield += calculateImprovementYield(eImprovement, eYield, iYield, ePlayer, false, getRouteType()) + calculateReligionImprovementYield(eImprovement, eYield, ePlayer, pOwningCity, pMajorityReligion, pSecondaryPantheon);
	}

	iYield += calculatePlayerYield(eYield, iYield, ePlayer, eImprovement, pOwningCity, pMajorityReligion, pSecondaryPantheon, false);

	RouteTypes eRoute = (RouteTypes)GC.getBuildInfo(eBuild)->getRoute();

	// If we're not changing the route that's here, use the route that's here already
	if(eRoute == NO_ROUTE)
	{
		if(!IsRoutePillaged() || GC.getBuildInfo(eBuild)->isRepair())
		{
			eRoute = getRouteType();
		}
	}

	if(eRoute != NO_ROUTE)
	{
		eImprovement = getImprovementType(); // potentially problematic - will ALWAYS calculate for existing improvement, no matter what the build is
		if(eImprovement != NO_IMPROVEMENT)
		{
			if(ePlayer != NO_PLAYER)
			{
				int iRouteYield = 0;
				if(IsCityConnection(ePlayer))
				{
					if(IsRouteRailroad())
					{
						iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
					}
					else if(IsRouteRoad())
					{
						iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
					}
				}
				if(IsTradeUnitRoute())
				{
					if( GET_PLAYER(ePlayer).GetCurrentEra()>=4 )
					{
						iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
					}
					else
					{
						iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
					}
				}
				if(iRouteYield > 0)
				{
					iYield += iRouteYield;
				}

				if(GET_PLAYER(ePlayer).GetPlayerTraits()->IsTradeRouteOnly())
				{
					if(IsCityConnection(ePlayer) || IsTradeUnitRoute())
					{
						if (getOwner() == GET_PLAYER(ePlayer).GetID())
						{
#if defined(MOD_USE_TRADE_FEATURES)
							if(getFeatureType() == NO_FEATURE && !MOD_USE_TRADE_FEATURES)
#endif
							{
								int iBonus = GET_PLAYER(ePlayer).GetPlayerTraits()->GetTerrainYieldChange(getTerrainType(), eYield);
								if(iBonus > 0)
								{
									int iScale = 0;
									int iEra = (GET_PLAYER(ePlayer).GetCurrentEra() + 1);

									iScale = ((iBonus * iEra) / 4);

									if(iScale <= 0)
									{
										iScale = 1;
									}
									iYield += iScale;
								}
							}
#if defined(MOD_USE_TRADE_FEATURES)
							else if(MOD_USE_TRADE_FEATURES)
#endif
							{
								int iBonus = GET_PLAYER(ePlayer).GetPlayerTraits()->GetTerrainYieldChange(getTerrainType(), eYield);
								if(iBonus > 0)
								{
									int iScale = 0;
									int iEra = (GET_PLAYER(ePlayer).GetCurrentEra() + 1);

									iScale = ((iBonus * iEra) / 4);

									if(iScale <= 0)
									{
										iScale = 1;
									}
									iYield += iScale;
								}
							}
						}
					}
					int iBonus2 = GET_PLAYER(ePlayer).GetPlayerTraits()->GetImprovementYieldChange(eImprovement, eYield);
					if(iBonus2 > 0)
					{
						if (getOwner() == GET_PLAYER(ePlayer).GetID())
						{
							if(IsCityConnection(ePlayer) || IsTradeUnitRoute() || IsAdjacentToTradeRoute())
							{
								int iScale = 0;
								int iEra = (GET_PLAYER(ePlayer).GetCurrentEra() + 1);

								iScale = ((iBonus2 * iEra) / 2);

								if(iScale <= 0)
								{
									iScale = 1;
								}
								iYield += iScale;
							}
						}
					}
				}
				else
				{
					iYield += GET_PLAYER(ePlayer).GetPlayerTraits()->GetTerrainYieldChange(getTerrainType(), eYield);
				}
			}
		}
	}

#if defined(MOD_API_EXTENSIONS)
    if (m_aiArbitraryYields[eYield])
    {
        iYield += m_aiArbitraryYields[eYield];
    }
#endif

	return std::max(0, iYield);
}

//	--------------------------------------------------------------------------------
bool CvPlot::canTrain(UnitTypes eUnit, bool, bool) const
{
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		return false;
	}

	CvUnitEntry& thisUnitEntry = *pkUnitInfo;
	DomainTypes thisUnitDomain = (DomainTypes) thisUnitEntry.GetDomainType();

	if(thisUnitEntry.IsPrereqResources())
	{
		if(thisUnitDomain == DOMAIN_SEA)
		{
			bool bValid = false;

			for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->isWater())
					{
						if(pLoopPlot->area()->getNumTotalResources() > 0)
						{
							bValid = true;
							break;
						}
					}
				}
			}

			if(!bValid)
			{
				return false;
			}
		}
		else if (thisUnitDomain == DOMAIN_LAND)
		{
			if(area()->getNumTotalResources() > 0)
			{
				return false;
			}
		}
	}

	if(isCity())
	{
		if(thisUnitDomain == DOMAIN_SEA)
		{
			//fast check for ocean (-1) or any lake (1) allows building ships
			if(!isWater() && !isCoastalLand(-1) && !isCoastalLand(1))
			{
				return false;
			}
		}
		else if (thisUnitDomain == DOMAIN_LAND)
		{
			if(area()->getNumTiles() < thisUnitEntry.GetMinAreaSize())
			{
				return false;
			}
		}
	}
	else
	{
		if(area()->getNumTiles() < thisUnitEntry.GetMinAreaSize())
		{
			return false;
		}

		if(thisUnitDomain == DOMAIN_SEA)
		{
			if(!isWater())
			{
				return false;
			}
		}
		else if(thisUnitDomain == DOMAIN_LAND)
		{
			if(isWater())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvPlot::countNumAirUnits(TeamTypes eTeam, bool bNoSuicide) const
{
	int iCount = 0;

	const IDInfo* pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit || pLoopUnit->isDelayedDeath())
			continue;

		if (pLoopUnit->getDomainType() != DOMAIN_AIR)
			continue;

		if (pLoopUnit->isCargo())
			continue;

		if(eTeam == NO_TEAM || pLoopUnit->getTeam() == eTeam)
		{
			if (bNoSuicide && !pLoopUnit->isSuicide())
				iCount += pLoopUnit->getUnitInfo().GetAirUnitCap();
			else
				iCount += pLoopUnit->getUnitInfo().GetAirUnitCap();
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvPlot::GetPlotIndex() const
{
	return m_iPlotIndex;
}

//	--------------------------------------------------------------------------------
char CvPlot::GetContinentType() const
{
	return m_cContinentType;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetContinentType(const char cContinent)
{
#if defined(MOD_EVENTS_TERRAFORMING)
	if (MOD_EVENTS_TERRAFORMING) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_CONTINENT, m_iX, m_iY, 0, cContinent, m_cContinentType, -1, -1);
	}
#endif

	m_cContinentType = cContinent;
}

//	--------------------------------------------------------------------------------
FAutoArchive& CvPlot::getSyncArchive()
{
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
const FAutoArchive& CvPlot::getSyncArchive() const
{
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
std::string CvPlot::debugDump(const FAutoVariableBase&) const
{
	std::string result = "Game Turn : ";
	char gameTurnBuffer[8] = {0};
	int gameTurn = GC.getGame().getGameTurn();
	sprintf_s(gameTurnBuffer, "%d\0", gameTurn);
	result += gameTurnBuffer;
	return result;
}

//	--------------------------------------------------------------------------------
std::string CvPlot::stackTraceRemark(const FAutoVariableBase& var) const
{
	return debugDump(var);
}

//	---------------------------------------------------------------------------
//	Validate the contents of the plot.  This will attempt to clean up inconsistencies.
//	Returns 0 if no errors or:
//		bit 0 = true, an error was found in the plot's data
//		bit 1 = true, the error was un-recoverable
int CvPlot::Validate(CvMap& kParentMap)
{
	//------------------------------
	// force mountain terrain on mountain plots for correct impassability & yields - map scripts are sloppy here
	if (m_eTerrainType == TERRAIN_MOUNTAIN && m_ePlotType != PLOT_MOUNTAIN)
	{
		if (getFeatureType()<FEATURE_NATURAL_WONDER)
			setFeatureType(NO_FEATURE);
		setPlotType(PLOT_MOUNTAIN);
		setResourceType(NO_RESOURCE,0);
	}

	if (m_ePlotType == PLOT_MOUNTAIN && m_eTerrainType != TERRAIN_MOUNTAIN)
	{
		if (getFeatureType()<FEATURE_NATURAL_WONDER)
			setFeatureType(NO_FEATURE);
		setTerrainType(TERRAIN_MOUNTAIN);
		setResourceType(NO_RESOURCE,0);
	}

	// force correct terrain for ocean plots - just a failsafe
	if (m_ePlotType == PLOT_OCEAN && m_eTerrainType != TERRAIN_COAST && m_eTerrainType != TERRAIN_OCEAN)
	{ 
		setTerrainType(TERRAIN_COAST);
	}

	if ( (m_eTerrainType == TERRAIN_COAST || m_eTerrainType == TERRAIN_OCEAN) && m_ePlotType != PLOT_OCEAN)
	{
		setPlotType(PLOT_OCEAN);
	}
	//------------------------------

	int iError = 0;
	IDInfo* pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		CvUnit* pkUnit = GetPlayerUnit(*pUnitNode);
		if(pkUnit == NULL)
		{
			m_units.deleteNode(pUnitNode);
			pUnitNode = headUnitNode();
			iError |= 1;
		}
		else
		{
			if(pkUnit->getX() != m_iX || pkUnit->getY() != m_iY)
			{
				iError |= 1;
				// Unit thinks it is in another plot.  Unit wins.
				m_units.deleteNode(pUnitNode);

				CvPlot* pkCorrectPlot = kParentMap.plot(pkUnit->getX(), pkUnit->getY());
				if(pkCorrectPlot)
				{
					if(pkCorrectPlot->getUnitIndex(pkUnit) < 0)	// Already in there?
						pkCorrectPlot->addUnit(pkUnit, false);		// Nope, add it.
				}
				else
				{
					// Drat, that plot does not exist.
					iError |= 2;
				}

				pUnitNode = headUnitNode();
			}
			else
				pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	return iError;
}

//	--------------------------------------------------------------------------------
/// Some reason we don't need to pay maintenance here?
bool CvPlot::MustPayMaintenanceHere(PlayerTypes ePlayer) const
{
	if(isHills() && GET_PLAYER(ePlayer).GetPlayerTraits()->IsNoHillsImprovementMaintenance())
	{
		return false;
	}

	return true;
}

#if defined(MOD_API_EXTENSIONS)
//	---------------------------------------------------------------------------
void CvPlot::SetArchaeologicalRecord(GreatWorkArtifactClass eType, PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	if (ePlayer1 != NO_PLAYER)
	{
		m_kArchaeologyData.m_eArtifactType = eType;
		m_kArchaeologyData.m_ePlayer1 = ePlayer1;
		m_kArchaeologyData.m_ePlayer2 = ePlayer2;
		m_kArchaeologyData.m_eEra = GET_PLAYER(ePlayer1).GetCurrentEra();
	}
}

//	---------------------------------------------------------------------------
void CvPlot::SetArchaeologicalRecord(GreatWorkArtifactClass eType, EraTypes eEra, PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	if (ePlayer1 != NO_PLAYER)
	{
		m_kArchaeologyData.m_eArtifactType = eType;
		m_kArchaeologyData.m_ePlayer1 = ePlayer1;
		m_kArchaeologyData.m_ePlayer2 = ePlayer2;
		m_kArchaeologyData.m_eEra = eEra;
	}
}
#endif

//	---------------------------------------------------------------------------
void CvPlot::AddArchaeologicalRecord(GreatWorkArtifactClass eType, PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	ImprovementTypes eImprovement = getImprovementType();
	if (eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement))
	{
		if (GC.getImprovementInfo(eImprovement)->IsPermanent() || GC.getImprovementInfo(eImprovement)->IsCreatedByGreatPerson())
			return;
	}
	// Make sure the new record is more significant
	if (!GC.getGame().IsArchaeologyTriggered() && eType > m_kArchaeologyData.m_eArtifactType)
	{
		if (ePlayer1 != NO_PLAYER)
		{
			m_kArchaeologyData.m_eArtifactType = eType;
			m_kArchaeologyData.m_ePlayer1 = ePlayer1;
			m_kArchaeologyData.m_ePlayer2 = ePlayer2;
			m_kArchaeologyData.m_eEra = GET_PLAYER(ePlayer1).GetCurrentEra();
		}
	}
}

//	---------------------------------------------------------------------------
void CvPlot::AddArchaeologicalRecord(GreatWorkArtifactClass eType, EraTypes eEra, PlayerTypes ePlayer1, PlayerTypes ePlayer2)
{
	ImprovementTypes eImprovement = getImprovementType();
	if (eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement))
	{
		if (GC.getImprovementInfo(eImprovement)->IsPermanent() || GC.getImprovementInfo(eImprovement)->IsCreatedByGreatPerson())
			return;
	}

	// Make sure the new record is more significant
	if (!GC.getGame().IsArchaeologyTriggered() && eType > m_kArchaeologyData.m_eArtifactType)
	{
		if (ePlayer1 != NO_PLAYER)
		{
			m_kArchaeologyData.m_eArtifactType = eType;
			m_kArchaeologyData.m_ePlayer1 = ePlayer1;
			m_kArchaeologyData.m_ePlayer2 = ePlayer2;
			m_kArchaeologyData.m_eEra = eEra;
		}
	}
}

//	---------------------------------------------------------------------------
void CvPlot::ClearArchaeologicalRecord()
{
	m_kArchaeologyData.m_eArtifactType = NO_GREAT_WORK_ARTIFACT_CLASS;
	m_kArchaeologyData.m_ePlayer1 = NO_PLAYER;
	m_kArchaeologyData.m_ePlayer2 = NO_PLAYER;
	m_kArchaeologyData.m_eEra = NO_ERA;
#if defined(MOD_BALANCE_CORE)
	ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GC.getARTIFACT_RESOURCE());
	ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GC.getHIDDEN_ARTIFACT_RESOURCE());
	if (getResourceType() == eArtifactResourceType || getResourceType() == eHiddenArtifactResourceType)
	{
		setResourceType(NO_RESOURCE, 0);
	}
#endif
}

//	---------------------------------------------------------------------------
CvArchaeologyData CvPlot::GetArchaeologicalRecord() const
{
	return m_kArchaeologyData; // return a copy of the record
}

//	---------------------------------------------------------------------------
void CvPlot::SetArtifactType(GreatWorkArtifactClass eType)
{
	m_kArchaeologyData.m_eArtifactType = eType;
}

//	---------------------------------------------------------------------------
void CvPlot::SetArtifactGreatWork(GreatWorkType eWork)
{
	m_kArchaeologyData.m_eWork = eWork;
}

//	---------------------------------------------------------------------------
bool CvPlot::HasWrittenArtifact() const
{
	bool bRtnValue = false;
	GreatWorkArtifactClass eArtifactClass = m_kArchaeologyData.m_eArtifactType;
	if (eArtifactClass == CvTypes::getARTIFACT_WRITING())
	{
		bRtnValue = true;
	}
	return bRtnValue;
}

//	--------------------------------------------------------------------------------
// Citadel
int CvPlot::GetDamageFromAdjacentPlots(PlayerTypes ePlayer) const
{
	VALIDATE_OBJECT
	int iDamage = 0;

	// Look around this unit to see if there's an adjacent citadel
	// But exclude this plot!
	for (int i=RING0_PLOTS; i<RING1_PLOTS; i++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(this,i);
		if(pLoopPlot != NULL)
		{
			ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

			// Citadel here?
			if(eImprovement != NO_IMPROVEMENT && !pLoopPlot->IsImprovementPillaged() && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() != 0)
			{
				if (pLoopPlot->getOwner() != NO_PLAYER && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(pLoopPlot->getTeam()))
					iDamage = max(iDamage, GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage());
			}

			// Unit here that acts like a citadel?
			for (int iZ = 0; iZ < pLoopPlot->getNumUnits(); iZ++)
			{
				CvUnit* pLoopUnit = pLoopPlot->getUnitByIndex(iZ);
				if (pLoopUnit && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(pLoopUnit->getTeam()))
					iDamage = max(iDamage, pLoopUnit->getNearbyEnemyDamage());
			}
		}
	}

	return iDamage;
}

//	---------------------------------------------------------------------------
void CvPlot::updateImpassable(TeamTypes eTeam)
{
	const TerrainTypes eTerrain = getTerrainType();
	const FeatureTypes eFeature = getFeatureType();

	//not really related but a good chance to update it
	m_bHighMoveCost = false;

	//only land is is passable by default
	m_bIsImpassable = isMountain();
	if (eTeam != NO_TEAM)
		SetTeamImpassable(eTeam, m_bIsImpassable);
	else
		for (size_t i=0; i<MAX_TEAMS; i++)
			SetTeamImpassable((TeamTypes)i, m_bIsImpassable);

	//if it's passable, check for blocking terrain/features
	if(eTerrain != NO_TERRAIN)
	{
		if(eFeature == NO_FEATURE)
		{
			CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
			if(pkTerrainInfo)
			{
				m_bIsImpassable = pkTerrainInfo->isImpassable();
				if (eTeam != NO_TEAM)
				{
					SetTeamImpassable(eTeam, m_bIsImpassable);

					if(m_bIsImpassable && (TechTypes)pkTerrainInfo->GetPrereqPassable() != NO_TECH)
					{
						if (GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)pkTerrainInfo->GetPrereqPassable()))
							SetTeamImpassable(eTeam, false);
					}
				}
				else
				{
					for (size_t i=0; i<MAX_TEAMS; i++)
					{
						SetTeamImpassable((TeamTypes)i, m_bIsImpassable);

						if(m_bIsImpassable && (TechTypes)pkTerrainInfo->GetPrereqPassable() != NO_TECH)
						{
							if (GET_TEAM((TeamTypes)i).GetTeamTechs()->HasTech((TechTypes)pkTerrainInfo->GetPrereqPassable()))
								SetTeamImpassable((TeamTypes)i, false);
						}
					}
				}

				//update the "rough plot" flag as well
				int iMoveCost = pkTerrainInfo->getMovementCost();
				if (isHills() || isMountain())
					iMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
				m_bHighMoveCost = (iMoveCost > 1);
			}
		}
		else
		{
			CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
			if(pkFeatureInfo)
			{
				m_bIsImpassable = pkFeatureInfo->isImpassable();
				if (eTeam != NO_TEAM)
				{
					SetTeamImpassable(eTeam, m_bIsImpassable);

					if(m_bIsImpassable && (TechTypes)pkFeatureInfo->GetPrereqPassable() != NO_TECH)
					{
						if (GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes)pkFeatureInfo->GetPrereqPassable()))
							SetTeamImpassable(eTeam, false);
					}
				}
				else
				{
					for (size_t i=0; i<MAX_TEAMS; i++)
					{
						SetTeamImpassable((TeamTypes)i, m_bIsImpassable);

						if(m_bIsImpassable && (TechTypes)pkFeatureInfo->GetPrereqPassable() != NO_TECH)
						{
							if (GET_TEAM((TeamTypes)i).GetTeamTechs()->HasTech((TechTypes)pkFeatureInfo->GetPrereqPassable()))
								SetTeamImpassable((TeamTypes)i, false);
						}
					}
				}	

				//update the "rough plot" flag as well
				int iMoveCost = pkFeatureInfo->getMovementCost();
				if (isHills() || isMountain())
					iMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
				m_bHighMoveCost = (iMoveCost > 1);
			}
		}
	}
}

bool CvPlot::isImpassable(TeamTypes eTeam) const
{
	if(eTeam != NO_TEAM)
	{
		return IsTeamImpassable(eTeam);
	}

	return m_bIsImpassable;
}

bool CvPlot::isSameOrAdjacentArea(CvPlot* pOtherPlot) const
{
	if (pOtherPlot == NULL)
		return false;

	if (getArea() == pOtherPlot->getArea())
		return true;

	std::vector<int> myAdjacent = getAllAdjacentAreas();
	std::vector<int>::iterator result = std::find(myAdjacent.begin(), myAdjacent.end(), pOtherPlot->getArea());
	return (result != myAdjacent.end());
}

//--------------------------------------------------------------------
// in updateImpassable we check terrain and features (per plot), combined with technologies (per team). here we additionally look at traits (per player). 
// result is a simplified version of canMoveInto. since we don't know the particular of the unit, we are more restrictive here
bool CvPlot::isValidMovePlot(PlayerTypes ePlayer, bool bCheckTerritory) const
{
	if ( getRouteType()!=NO_ROUTE && !IsRoutePillaged() && (!isCity() || getOwner()==ePlayer) ) //if it's a city, it needs to be our city
		return true;

	if (ePlayer==NO_PLAYER)
		return !m_bIsImpassable;
	else
	{
		TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
		if ( IsTeamImpassable(eTeam) )
		{
			bool bCanPassBecauseOfPlayerTrait = false;

			//check some special traits
			if (isIce() && GET_PLAYER(ePlayer).CanCrossIce() )
				bCanPassBecauseOfPlayerTrait = true;
			//inca
			if (isMountain() && GET_PLAYER(ePlayer).CanCrossMountain() )
				bCanPassBecauseOfPlayerTrait = true;
			//deep water...
			if (isDeepWater() && GET_PLAYER(ePlayer).CanCrossOcean())
				bCanPassBecauseOfPlayerTrait = true;
			//and shallow water... (this is necessary because of scenarios and tech situations where units can embark before techs, and vice-versa.
			if (isShallowWater() && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canEmbark())
				bCanPassBecauseOfPlayerTrait = true;

			if (!bCanPassBecauseOfPlayerTrait)
				return false;
		}

		//now check territory also - majors only (minors are always open)
		if ( bCheckTerritory && getTeam()!=NO_TEAM && getTeam()!=eTeam && !GET_TEAM(getTeam()).isMinorCiv())
		{
			if (!GET_TEAM(getTeam()).IsAllowsOpenBordersToTeam(eTeam) && !GET_TEAM(eTeam).isAtWar(getTeam()))
			{
				return false;
			}
		}

		//seems we're good
		return true;
	}
}

//----------------------------------------------------------
//conservative estimate whether we can put a combat unit here. does not check different domains etc
bool CvPlot::canPlaceCombatUnit(PlayerTypes ePlayer) const
{
	if (!isValidMovePlot(ePlayer))
		return false;

	if (getOwner()!=NO_PLAYER && ePlayer!=NO_PLAYER)
	{
		TeamTypes ePlotTeam = GET_PLAYER(getOwner()).getTeam();
		TeamTypes eTestTeam = GET_PLAYER(ePlayer).getTeam();
		if (ePlotTeam!=eTestTeam && !GET_TEAM(ePlotTeam).isMinorCiv() && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eTestTeam) && !GET_TEAM(eTestTeam).isAtWar(ePlotTeam))
			return false;
	}

	//can't place into a plot with another combat unit (owner does not matter)
	if(getNumDefenders(NO_PLAYER) >= getUnitLimit())
		return false;
			
	//can't place into a plot with a foreign city
	if(isCity() && getPlotCity()->getOwner()!=ePlayer)
		return false;

	return true;
}

#if defined(MOD_API_EXTENSIONS)
bool CvPlot::IsCivilization(CivilizationTypes iCivilizationType) const
{
	return (GET_PLAYER(getOwner()).getCivilizationType() == iCivilizationType);
}

bool CvPlot::HasFeature(FeatureTypes iFeatureType) const
{
	if (iFeatureType == (FeatureTypes)GC.getInfoTypeForString("FEATURE_LAKE")) {
		return IsFeatureLake();
	} else if (iFeatureType == (FeatureTypes)GC.getInfoTypeForString("FEATURE_RIVER")) {
		return IsFeatureRiver();
	}

	return (getFeatureType() == iFeatureType);
}

bool CvPlot::IsFeatureLake() const
{
	return isLake();
}

bool CvPlot::IsFeatureRiver() const
{
	return isRiver();
}

bool CvPlot::HasAnyNaturalWonder() const
{
	return IsNaturalWonder();
}

bool CvPlot::HasNaturalWonder(FeatureTypes iFeatureType) const
{
	return HasFeature(iFeatureType);
}

bool CvPlot::HasImprovement(ImprovementTypes iImprovementType) const
{
	return (getImprovementType() == iImprovementType);
}

bool CvPlot::HasPlotType(PlotTypes iPlotType) const
{
	return (getPlotType() == iPlotType);
}

bool CvPlot::HasResource(ResourceTypes iResourceType) const
{
	return (getResourceType() == iResourceType);
}

bool CvPlot::HasRoute(RouteTypes iRouteType) const
{
	return (getRouteType() == iRouteType);
}

bool CvPlot::HasTerrain(TerrainTypes iTerrainType) const
{
	return (getTerrainType() == iTerrainType);
}

bool CvPlot::IsAdjacentToFeature(FeatureTypes iFeatureType) const
{
	int iX = getX(); int iY = getY();

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) {
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);

		if (pLoopPlot != NULL && pLoopPlot->HasFeature(iFeatureType)) {
			return true;
		}
	}

	return false;
}

bool CvPlot::IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const
{
	int iX = getX(); int iY = getY();

	for (int i = -iDistance; i <= iDistance; ++i) {
		for (int j = -iDistance; j <= iDistance; ++j) {
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->HasFeature(iFeatureType)) {
				return true;
			}
		}
	}

	return false;
}

#if defined(MOD_BALANCE_CORE)
bool CvPlot::IsWithinDistanceOfUnit(PlayerTypes ePlayer, UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	if(iDistance >= 0 && this != NULL)
	{
		for(int iI = 0; iI < this->getNumUnits(); iI++)
		{
			pLoopUnit = this->getUnitByIndex(iI);
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getUnitType() == eOtherUnit)
				{
					if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
					{
						return true;
					}
					else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
					{
						return true;
					}
					else if(!bIsFriendly && !bIsEnemy)
					{
						return true;
					}
				}
			}
		}
	}
	for (int i = -iDistance; i <= iDistance; ++i) 
	{
		for (int j = -iDistance; j <= iDistance; ++j) 
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
			{
				for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
				{
					pLoopUnit = pLoopPlot->getUnitByIndex(iI);
					if(pLoopUnit != NULL)
					{
						if(pLoopUnit->getUnitType() == eOtherUnit)
						{
							if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
							{
								return true;
							}
							else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
							{
								return true;
							}
							else if(!bIsFriendly && !bIsEnemy)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsWithinDistanceOfCity(const CvUnit* eThisUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	if(eThisUnit != NULL)
	{
		for (int i = -iDistance; i <= iDistance; ++i) 
		{
			for (int j = -iDistance; j <= iDistance; ++j) 
			{
				int iX = eThisUnit->getX(); int iY = eThisUnit->getY();
				CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);		
				if (pLoopPlot != NULL)
				{
					if(pLoopPlot->isCity())
					{
						if(bIsFriendly && pLoopPlot->isFriendlyCity(*eThisUnit))
						{
							return true;
						}
						else if(bIsEnemy && pLoopPlot->isEnemyCity(*eThisUnit))
						{
							return true;
						}
						else if(!bIsFriendly && !bIsEnemy)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsWithinDistanceOfUnitCombatType(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	if(iDistance >= 0 && this != NULL)
	{
		for(int iI = 0; iI < this->getNumUnits(); iI++)
		{
			pLoopUnit = this->getUnitByIndex(iI);
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getUnitCombatType() == eUnitCombat)
				{
					if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
					{
						return true;
					}
					else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
					{
						return true;
					}
					else if(!bIsFriendly && !bIsEnemy)
					{
						return true;
					}
				}
			}
		}
	}
	for (int i = -iDistance; i <= iDistance; ++i) 
	{
		for (int j = -iDistance; j <= iDistance; ++j) 
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
			{
				for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
				{
					pLoopUnit = pLoopPlot->getUnitByIndex(iI);
					if(pLoopUnit != NULL)
					{
						if(pLoopUnit->getUnitCombatType() == eUnitCombat)
						{
							if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
							{
								return true;
							}
							else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
							{
								return true;
							}
							else if(!bIsFriendly && !bIsEnemy)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsWithinDistanceOfUnitClass(PlayerTypes ePlayer, UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	if(iDistance >= 0 && this != NULL)
	{
		for(int iI = 0; iI < this->getNumUnits(); iI++)
		{
			pLoopUnit = this->getUnitByIndex(iI);
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getUnitClassType() == eUnitClass)
				{
					if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
					{
						return true;
					}
					else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
					{
						return true;
					}
					else if(!bIsFriendly && !bIsEnemy)
					{
						return true;
					}
				}
			}
		}
	}
	for (int i = -iDistance; i <= iDistance; ++i) 
	{
		for (int j = -iDistance; j <= iDistance; ++j) 
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
			{
				for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
				{
					pLoopUnit = pLoopPlot->getUnitByIndex(iI);
					if(pLoopUnit != NULL)
					{
						if(pLoopUnit->getUnitClassType() == eUnitClass)
						{
							if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
							{
								return true;
							}
							else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
							{
								return true;
							}
							else if(!bIsFriendly && !bIsEnemy)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsWithinDistanceOfUnitPromotion(PlayerTypes ePlayer, PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	if(iDistance >= 0 && this != NULL)
	{
		for(int iI = 0; iI < this->getNumUnits(); iI++)
		{
			pLoopUnit = this->getUnitByIndex(iI);
			if(pLoopUnit != NULL)
			{
				for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
				{
					const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
					CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkPromotionInfo)
					{
						if(pLoopUnit->isHasPromotion(ePromotion) && ePromotion == eUnitPromotion)
						{
							if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
							{
								return true;
							}
							else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
							{
								return true;
							}
							else if(!bIsFriendly && !bIsEnemy)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	for (int i = -iDistance; i <= iDistance; ++i) 
	{
		for (int j = -iDistance; j <= iDistance; ++j) 
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
			{
				for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
				{
					pLoopUnit = pLoopPlot->getUnitByIndex(iI);
					if(pLoopUnit != NULL)
					{
						for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
						{
							const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
							if(pkPromotionInfo)
							{
								if(pLoopUnit->isHasPromotion(ePromotion) && ePromotion == eUnitPromotion)
								{
									if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
									{
										return true;
									}
									else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
									{
										return true;
									}
									else if(!bIsFriendly && !bIsEnemy)
									{
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsAdjacentToUnit(PlayerTypes ePlayer, UnitTypes eOtherUnit, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) 
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);
		
		if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
		{
			for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
			{
				pLoopUnit = pLoopPlot->getUnitByIndex(iI);
				if(pLoopUnit != NULL)
				{
					if(pLoopUnit->getUnitType() == eOtherUnit)
					{
						if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
						{
							return true;
						}
						else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
						{
							return true;
						}
						else if(!bIsFriendly && !bIsEnemy)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsAdjacentToUnitCombatType(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) 
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);
		
		if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
		{
			for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
			{
				pLoopUnit = pLoopPlot->getUnitByIndex(iI);
				if(pLoopUnit != NULL)
				{
					if(pLoopUnit->getUnitCombatType() == eUnitCombat)
					{
						if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
						{
							return true;
						}
						else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
						{
							return true;
						}
						else if(!bIsFriendly && !bIsEnemy)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsAdjacentToUnitClass(PlayerTypes ePlayer, UnitClassTypes eUnitClass, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) 
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);
		
		if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
		{
			for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
			{
				pLoopUnit = pLoopPlot->getUnitByIndex(iI);
				if(pLoopUnit != NULL)
				{
					if(pLoopUnit->getUnitClassType() == eUnitClass)
					{
						if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
						{
							return true;
						}
						else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
						{
							return true;
						}
						else if(!bIsFriendly && !bIsEnemy)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsAdjacentToUnitPromotion(PlayerTypes ePlayer, PromotionTypes eUnitPromotion, bool bIsFriendly, bool bIsEnemy) const
{
	int iX = getX(); int iY = getY();
	CvUnit* pLoopUnit;
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) 
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);
		
		if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0 && pLoopPlot != this)
		{
			for(int iI = 0; iI < pLoopPlot->getNumUnits(); iI++)
			{
				pLoopUnit = pLoopPlot->getUnitByIndex(iI);
				if(pLoopUnit != NULL)
				{
					for(int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
					{
						const PromotionTypes ePromotion = (PromotionTypes) iPromotionLoop;
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo)
						{
							if(pLoopUnit->isHasPromotion(ePromotion) && ePromotion == eUnitPromotion)
							{
								if(bIsFriendly && GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam())
								{
									return true;
								}
								else if(bIsEnemy && GET_TEAM(GET_PLAYER(pLoopUnit->getOwner()).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
								{
									return true;
								}
								else if(!bIsFriendly && !bIsEnemy)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CvPlot::IsAdjacentToTradeRoute() const
{
	int iX = getX(); int iY = getY();
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) 
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);		
		if (pLoopPlot != NULL && pLoopPlot->IsTradeUnitRoute())
		{
			return true;
		}
	}
	return false;
}

bool CvPlot::IsAdjacentToRoute(RouteTypes eType) const
{
	int iX = getX(); int iY = getY();
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) 
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);		
		if (pLoopPlot != NULL && pLoopPlot->isRoute())
		{
			return eType==ROUTE_ANY || pLoopPlot->getRouteType()==eType;
		}
	}
	return false;
}
#endif

bool CvPlot::IsAdjacentToImprovement(ImprovementTypes iImprovementType) const
{
	int iX = getX(); int iY = getY();

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) {
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);

		if (pLoopPlot != NULL && pLoopPlot->HasImprovement(iImprovementType)) {
			return true;
		}
	}

	return false;
}

bool CvPlot::IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const
{
	int iX = getX(); int iY = getY();

	for (int i = -iDistance; i <= iDistance; ++i) {
		for (int j = -iDistance; j <= iDistance; ++j) {
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->HasImprovement(iImprovementType)) {
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::IsAdjacentToPlotType(PlotTypes iPlotType) const
{
	int iX = getX(); int iY = getY();

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) {
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);

		if (pLoopPlot != NULL && pLoopPlot->HasPlotType(iPlotType)) {
			return true;
		}
	}

	return false;
}

bool CvPlot::IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const
{
	int iX = getX(); int iY = getY();

	for (int i = -iDistance; i <= iDistance; ++i) {
		for (int j = -iDistance; j <= iDistance; ++j) {
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->HasPlotType(iPlotType)) {
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::IsAdjacentToResource(ResourceTypes iResourceType) const
{
	int iX = getX(); int iY = getY();

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) {
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);

		if (pLoopPlot != NULL && pLoopPlot->HasResource(iResourceType)) {
			return true;
		}
	}

	return false;
}

bool CvPlot::IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const
{
	int iX = getX(); int iY = getY();

	for (int i = -iDistance; i <= iDistance; ++i) {
		for (int j = -iDistance; j <= iDistance; ++j) {
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->HasResource(iResourceType)) {
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::IsAdjacentToTerrain(TerrainTypes iTerrainType) const
{
	int iX = getX(); int iY = getY();

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++) {
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes) iDirection);

		if (pLoopPlot != NULL && pLoopPlot->HasTerrain(iTerrainType)) {
			return true;
		}
	}

	return false;
}

bool CvPlot::IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const
{
	int iX = getX(); int iY = getY();

	for (int i = -iDistance; i <= iDistance; ++i) {
		for (int j = -iDistance; j <= iDistance; ++j) {
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(iX, iY, i, j, iDistance);
		
			if (pLoopPlot != NULL && pLoopPlot->HasTerrain(iTerrainType)) {
				return true;
			}
		}
	}

	return false;
}
#endif

///-------------------------------------
/// Is an enemy city next to us?
bool CvPlot::IsEnemyCityAdjacent(TeamTypes eMyTeam, const CvCity* pSpecifyCity) const
{
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if(!pLoopPlot)
		{
			continue;
		}

		CvCity* pCity = pLoopPlot->getPlotCity();
		if(!pCity)
		{
			continue;
		}

		if (!pSpecifyCity || (pCity->getX() == pSpecifyCity->getX() && pCity->getY() == pSpecifyCity->getY()))
		{
			if(GET_TEAM(eMyTeam).isAtWar(pCity->getTeam()))
			{
				return true;
			}
		}
	}

	return false;
}

vector<CvUnit*> CvPlot::GetAdjacentEnemyUnits(TeamTypes eMyTeam, DomainTypes eDomain) const
{
	vector<CvUnit*> result;
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsShuffled(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		CvPlot *pLoopPlot = aPlotsToCheck[iCount];
		if(pLoopPlot != NULL && !pLoopPlot->isCity()) //ignore units in cities
		{
			IDInfo* pUnitNode = pLoopPlot->headUnitNode();

			// Loop through all units on this plot
			while(pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit)
				{
					// Must be a combat Unit
					if(pLoopUnit->IsCanAttack() && !pLoopUnit->isEmbarked() && !pLoopUnit->isDelayedDeath())
					{
						TeamTypes eTheirTeam = pLoopUnit->getTeam();

						// This team which this unit belongs to must be at war with us
						if(GET_TEAM(eTheirTeam).isAtWar(eMyTeam))
						{
							// Must be same domain
							if (pLoopUnit->getDomainType() == eDomain || pLoopUnit->getDomainType() == DOMAIN_HOVER || eDomain == NO_DOMAIN)
							{
								result.push_back(pLoopUnit);
							}
						}
					}
				}
			}
		}
	}

	return result;
}

//friendly, enemy power
pair<int,int> CvPlot::GetLocalUnitPower(PlayerTypes ePlayer, int iRange, bool bSameDomain) const
{
	int iFriendlyPower = 0;
	int iEnemyPower = 0;
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	iRange = range(iRange, 1, 5);
	for (int i = 0; i < RING_PLOTS[iRange]; i++)
	{
		CvPlot* pTestPlot = iterateRingPlots(this, i);
		if (!pTestPlot || !pTestPlot->isVisible(kPlayer.getTeam()))
			continue;

		//also take into account carriers!
		const IDInfo* pUnitNode = pTestPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pTestPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit && pLoopUnit->IsCanAttack())
			{
				//this check skips air units ...
				if (bSameDomain && pLoopUnit->getDomainType() != getDomain())
					continue;

				if (kPlayer.IsAtWarWith(pLoopUnit->getOwner()) && !pLoopUnit->isInvisible(kPlayer.getTeam(), false))
				{
					iEnemyPower += pLoopUnit->GetPower();
				}
				else if (kPlayer.getTeam() == pLoopUnit->getTeam())
				{
					iFriendlyPower += pLoopUnit->GetPower();
				}
			}
		}
	}

	return make_pair(iFriendlyPower,iEnemyPower);
}

int CvPlot::GetNumEnemyUnitsAdjacent(TeamTypes eMyTeam, DomainTypes eDomain, const CvUnit* pUnitToExclude, bool bCountRanged) const
{
	int iNumEnemiesAdjacent = 0;

	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		CvPlot* pLoopPlot = aPlotsToCheck[iCount];
		if(pLoopPlot != NULL && pLoopPlot->isVisible(eMyTeam))
		{
			IDInfo* pUnitNode = pLoopPlot->headUnitNode();

			// Loop through all units on this plot
			while(pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// No NULL, and no unit we want to exclude
				if(pLoopUnit && pLoopUnit != pUnitToExclude)
				{
					// Must be a combat Unit
					if(pLoopUnit->IsCanAttack() && !pLoopUnit->isEmbarked())
					{
						if (pLoopUnit->IsCanAttackRanged() && !bCountRanged)
							continue;

						TeamTypes eTheirTeam = pLoopUnit->getTeam();

						// This team which this unit belongs to must be at war with us
						if(GET_TEAM(eTheirTeam).isAtWar(eMyTeam))
						{
							// Must be same domain
							if (pLoopUnit->getDomainType() == eDomain || pLoopUnit->getDomainType() == DOMAIN_HOVER || eDomain == NO_DOMAIN)
							{
								iNumEnemiesAdjacent++;
							}
						}
					}
				}
			}
		}
	}

	return iNumEnemiesAdjacent;
}

int CvPlot::GetNumFriendlyUnitsAdjacent(TeamTypes eMyTeam, DomainTypes eDomain, const CvUnit* pUnitToExclude, bool bCountRanged) const
{
	int iNumFriendliesAdjacent = 0;

	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		CvPlot* pLoopPlot = aPlotsToCheck[iCount];
		if(pLoopPlot != NULL)
		{
			IDInfo* pUnitNode = pLoopPlot->headUnitNode();

			// Loop through all units on this plot
			while(pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// No NULL, and no unit we want to exclude
				if(pLoopUnit && pLoopUnit != pUnitToExclude)
				{
					// Must be a combat Unit
					if(pLoopUnit->IsCanAttack() && !pLoopUnit->isEmbarked())
					{
						if (pLoopUnit->IsCanAttackRanged() && !bCountRanged)
							continue;

						// Same team?
						if(pLoopUnit->getTeam() == eMyTeam)
						{
							// Must be same domain
							if (pLoopUnit->getDomainType() == eDomain || pLoopUnit->getDomainType() == DOMAIN_HOVER || eDomain == NO_DOMAIN)
							{
								iNumFriendliesAdjacent++;
							}
						}
					}
				}
			}
		}
	}

	return iNumFriendliesAdjacent;
}

#if defined(MOD_BALANCE_CORE)
int CvPlot::GetNumSpecificFriendlyUnitCombatsAdjacent(TeamTypes eMyTeam, UnitCombatTypes eUnitCombat, const CvUnit* pUnitToExclude) const
{
	int iNumber = 0;

	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		CvPlot* pLoopPlot = aPlotsToCheck[iCount];
		if(pLoopPlot != NULL)
		{
			IDInfo* pUnitNode = pLoopPlot->headUnitNode();

			// Loop through all units on this plot
			while(pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// No NULL, and no unit we want to exclude
				if(pLoopUnit && pLoopUnit != pUnitToExclude)
				{
					// Must be a combat Unit
					if(pLoopUnit->IsCanAttack() && !pLoopUnit->isEmbarked())
					{
						// Same team?
						if(pLoopUnit->getTeam() == eMyTeam)
						{
							// Must be same unit combat type
							if (pLoopUnit->getUnitCombatType() == eUnitCombat)
							{
								iNumber++;
							}
						}
					}
				}
			}
		}
	}

	return iNumber;
}
#endif

bool CvPlot::IsFriendlyUnitAdjacent(TeamTypes eMyTeam, bool bCombatUnit) const
{
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		CvPlot* pLoopPlot = aPlotsToCheck[iCount];
		if(pLoopPlot != NULL)
		{
			// Must be in same area
			if(pLoopPlot->getArea() == getArea())
			{
				IDInfo* pUnitNode = pLoopPlot->headUnitNode();

				while(pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if(pLoopUnit && pLoopUnit->getTeam() == eMyTeam)
					{
						// Combat Unit?
						if(!bCombatUnit || pLoopUnit->IsCanAttack())
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

int CvPlot::GetNumSpecificPlayerUnitsAdjacent(PlayerTypes ePlayer, const CvUnit* pUnitToExclude, const CvUnit* pExampleUnitType, bool bCombatOnly) const
{
	int iNumUnitsAdjacent = 0;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL)
		{
			IDInfo* pUnitNode = pLoopPlot->headUnitNode();

			// Loop through all units on this plot
			while(pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// No NULL, and no unit we want to exclude
				if(pLoopUnit && pLoopUnit != pUnitToExclude && pLoopUnit->getOwner()==ePlayer)
				{
					// Must be a combat Unit
					if(!bCombatOnly || pLoopUnit->IsCanAttack())
					{
						if(!pExampleUnitType || pLoopUnit->getUnitType() == pExampleUnitType->getUnitType())
						{
							iNumUnitsAdjacent++;
						}
					}
				}
			}
		}
	}

	return iNumUnitsAdjacent;
}

///-------------------------------------

#if defined(MOD_BALANCE_CORE)
int CvPlot::GetDefenseBuildValue(PlayerTypes eOwner)
{
	TeamTypes eTeam = GET_PLAYER(eOwner).getTeam();
	if(eTeam == NO_TEAM)
		return 0;

	static const ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
	static const ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");

	if(eFort == NO_IMPROVEMENT && eCitadel == NO_IMPROVEMENT)
		return 0;

	// See how many outside plots are nearby to monitor
	int iAdjacentUnowned = 0;
	int iAdjacentOwnedOther = 0;
	int iNearbyForts = 0;
	int iNearbyOwnedOther = 0;
	int iBadNearby = 0;
	int iBadAdjacent = 0;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		//Don't want them adjacent to cities, but we do want to check for plot ownership.
		if (pLoopAdjacentPlot != NULL)
		{	
			//Adjacent to city? Break!
			if(pLoopAdjacentPlot->isCity())
				return 0;
			//impassable is uninteresting
			if(pLoopAdjacentPlot->isImpassable(eTeam))
				continue;

			if(pLoopAdjacentPlot->getOwner() == eOwner)
			{
				//don't build other defenses near citadels (next to forts is ok)
				if (pLoopAdjacentPlot->getImprovementType() == eCitadel)
					return 0;
			}
			else if(pLoopAdjacentPlot->getOwner() == NO_PLAYER)
			{
				iAdjacentUnowned++;
			}
			else if(GET_PLAYER(pLoopAdjacentPlot->getOwner()).isMajorCiv())
			{
				iAdjacentOwnedOther++;
				if (GET_PLAYER(eOwner).GetDiplomacyAI()->GetMajorCivOpinion(pLoopAdjacentPlot->getOwner()) <= MAJOR_CIV_OPINION_NEUTRAL)
					iBadAdjacent++;
			}
			else if(GET_PLAYER(pLoopAdjacentPlot->getOwner()).isMinorCiv())
			{
				iAdjacentOwnedOther++;
				if (GET_PLAYER(eOwner).GetDiplomacyAI()->GetMinorCivApproach(pLoopAdjacentPlot->getOwner()) >= MINOR_CIV_APPROACH_CONQUEST)
					iBadAdjacent++;
			}
		}
	}

	//If there are unowned or enemy tiles, this is a nice 'frontier' position.
	if(iAdjacentUnowned + iAdjacentOwnedOther > 2 || iBadAdjacent > 0)
	{
		//check the wider area for enemy tiles. may also be on another landmass
		for (int i=RING1_PLOTS; i<RING2_PLOTS; i++)
		{
			CvPlot* pLoopNearbyPlot = iterateRingPlots(this, i);

			//Don't want them adjacent to cities, but we do want to check for plot ownership.
			if (pLoopNearbyPlot != NULL && pLoopNearbyPlot->isRevealed(eTeam))
			{
				if (!pLoopNearbyPlot->isOwned())
					continue;

				if (pLoopNearbyPlot->getOwner() != eOwner)
				{
					if (GET_PLAYER(pLoopNearbyPlot->getOwner()).isMajorCiv())
					{
						iNearbyOwnedOther++;
						if (GET_PLAYER(eOwner).GetDiplomacyAI()->GetMajorCivOpinion(pLoopNearbyPlot->getOwner()) <= MAJOR_CIV_OPINION_NEUTRAL)
							iBadNearby++;
					}
					else if (GET_PLAYER(pLoopNearbyPlot->getOwner()).isMinorCiv())
					{
						iNearbyOwnedOther++;
						if (GET_PLAYER(eOwner).GetDiplomacyAI()->GetMinorCivApproach(pLoopNearbyPlot->getOwner()) >= MINOR_CIV_APPROACH_CONQUEST)
							iBadNearby++;
					}
				}

					//Let's check for owned nearby forts as well
				if(pLoopNearbyPlot->getImprovementType() != NO_IMPROVEMENT && pLoopNearbyPlot->getOwner() == eOwner)
					if(eFort == pLoopNearbyPlot->getImprovementType() || eCitadel == pLoopNearbyPlot->getImprovementType())
						iNearbyForts++;
			}
		}

		//only build a fort if it's somewhat close to the enemy and there aren't forts nearby. We shouldn't be spamming them.
		if (iBadNearby == 0 || iNearbyForts > 2)
			return 0;

		// Get score for this sentry point (defense and danger)
		int iScore = defenseModifier(eTeam, true, true);

		//Bonus for nearby owned tiles
		iScore += (iNearbyOwnedOther * 3);
		
		//Big Bonus if adjacent to territory.
		iScore += (iAdjacentOwnedOther * 4);

		//Big Bonus if adjacent to enemy territory.
		iScore += (iBadAdjacent * 16);

		//Big Bonus if adjacent to enemy territory.
		iScore += (iBadNearby * 10);

		//Big bonus if chokepoint
		if(IsChokePoint())
			iScore += 17;

		return iScore;
	}

	return 0;
}

#endif

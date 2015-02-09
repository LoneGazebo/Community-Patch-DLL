/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

#if defined(MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES)
#include "CvDiplomacyAI.h"
#endif

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
std::set<CvPlot*> plotsToCheck;
void SyncPlots()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes authoritativePlayer = GC.getGame().getActivePlayer();
		std::set<CvPlot*>::const_iterator i;
		for(i = plotsToCheck.begin(); i != plotsToCheck.end(); ++i)
		{
			const CvPlot* plot = *i;

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
	std::set<CvPlot*>::iterator i;
	for(i = plotsToCheck.begin(); i != plotsToCheck.end(); ++i)
	{
		CvPlot* plot = *i;

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
	FSerialization::plotsToCheck.insert(this);
	m_paiBuildProgress = NULL;

	m_szScriptData = NULL;

#if defined(SHOW_PLOT_POPUP)
	m_fPopupDelay = 0.5;
#endif

	m_cContinentType = 0;
	m_cRiverCrossing = 0;

	reset(0, 0, true);
}


//	--------------------------------------------------------------------------------
CvPlot::~CvPlot()
{
	FSerialization::plotsToCheck.erase(this);
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

#if defined(SHOW_PLOT_POPUP)
	m_fPopupDelay = 0.5;
#endif

	m_pCenterUnit = NULL;

	SAFE_DELETE_ARRAY(m_paiBuildProgress);

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
	m_iArea = FFreeList::INVALID_INDEX;
	m_iLandmass = FFreeList::INVALID_INDEX;
	m_iFeatureVariety = 0;
	m_iOwnershipDuration = 0;
	m_iImprovementDuration = 0;
	m_iUpgradeProgress = 0;
	m_iCulture = 0;
	m_iNumMajorCivsRevealed = 0;
	m_iCityRadiusCount = 0;
	m_iReconCount = 0;
	m_iRiverCrossingCount = 0;
	m_iResourceNum = 0;
	m_cContinentType = 0;

	m_uiTradeRouteBitFlags = 0;

	m_bStartingPlot = false;
	m_bHills = false;
	m_bNEOfRiver = false;
	m_bWOfRiver = false;
	m_bNWOfRiver = false;
	m_bPotentialCityWork = false;
	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_bImprovementPillaged = false;
	m_bRoutePillaged = false;
	m_bBarbCampNotConverting = false;
	m_bRoughFeature = false;
	m_bResourceLinkedCityActive = false;
	m_bImprovedByGiftFromMajor = false;
	m_bIsAdjacentToLand = false;
	m_bIsImpassable = false;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	m_bImprovementEmbassy = false;
#endif

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

	m_cBuilderAIScratchPadPlayer = 0;
	m_sBuilderAIScratchPadTurn = 0;
	m_sBuilderAIScratchPadValue = 0;
	m_eBuilderAIScratchPadRoute = NO_ROUTE;

	m_plotCity.reset();
	m_workingCity.reset();
	m_workingCityOverride.reset();
	m_ResourceLinkedCity.reset();
	m_purchaseCity.reset();

	if(!bConstructorCall)
	{
		for(int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			m_aiYield[iI] = 0;
		}
		for(int iI = 0; iI < REALLY_MAX_PLAYERS; ++iI)
		{
			m_aiFoundValue[iI] = -1;
		}
		for(int iI = 0; iI < REALLY_MAX_PLAYERS; ++iI)
		{
			m_aiPlayerCityRadiusCount[iI] = 0;
		}
		for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
		{
			m_aiVisibilityCount[iI] = 0;
		}
		for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
		{
			m_aiRevealedOwner[iI] = -1;
		}
		for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
		{
			//m_abRevealed[iI] = false;
		}
		m_bfRevealed.ClearAll();
		for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
		{
			m_abResourceForceReveal[iI] = false;
		}
		for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
		{
			m_aeRevealedImprovementType[iI] = NO_IMPROVEMENT;
		}
		for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
		{
			m_aeRevealedRouteType[iI] = NO_ROUTE;
		}
		for(int iI = 0; iI < MAX_MAJOR_CIVS; ++iI)
		{
			m_abNoSettling[iI] = false;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			m_abAvoidMovement[iI] = false;
#endif
		}
	}
	for(int iI = 0; iI < REALLY_MAX_TEAMS; ++iI)
	{
		for(int iJ = 0; iJ < NUM_INVISIBLE_TYPES; ++iJ)
		{
			m_apaiInvisibleVisibilityCount[iI][iJ] = 0;
		}
	}

	m_kArchaeologyData.Reset();
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
#if defined(SHOW_PLOT_POPUP)
	m_fPopupDelay = 0.5;
#endif

	if(isOwned())
	{
		changeOwnershipDuration(1);
	}

	if(getImprovementType() != NO_IMPROVEMENT)
	{
		changeImprovementDuration(1);
	}

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
							if(GC.getGame().getJonRandNum(thisImprovementInfo->GetImprovementResourceDiscoverRand(iI), "Resource Discovery") == 0)
							{
								iResourceNum = GC.getMap().getRandomResourceQuantity((ResourceTypes)iI);
								setResourceType((ResourceTypes)iI, iResourceNum);
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
		gDLL->GameplayFOWChanged(getX(), getY(), eFOWMode, false);
	}
	else
	{
		CvMap::DeferredPlotArray& plotList = GC.getMap().m_vDeferredFogPlots;
		for (CvMap::DeferredPlotArray::const_iterator itr = plotList.begin(); itr != plotList.end(); ++itr)
		{
			if((*itr) == this)
				return;	// Already in
		}
		plotList.push_back(this);
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
				CvUnit* pLoopUnit = ::getUnit(*itrUnit);
				if (pLoopUnit)
				{
					InvisibleTypes eInvisibleType = pLoopUnit->getInvisibleType();
					if (eInvisibleType != NO_INVISIBLE)
					{
						// This unit has visibility rules, send a message that it needs to update itself.
						auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitVisibility(pDllUnit.get(), (pLoopUnit->getTeam() == eActiveTeam)?true:isInvisibleVisible(eActiveTeam, eInvisibleType), true, 0.01f);
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
		UnitHandle hBestDefender = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer());
		if(hBestDefender && hBestDefender->getDomainType() != DOMAIN_AIR && !hBestDefender->isInvisible(eActiveTeam,false))
			setCenterUnit(hBestDefender);
	}

	// okay, all of the other checks failed - if there is any unit here, make it the center unit
	if(!getCenterUnit())
	{
		IDInfo* pUnitNode;
		CvUnit* pLoopUnit;

		pUnitNode = headUnitNode();

		if(pUnitNode != NULL)
		{
			pLoopUnit = GetPlayerUnit(*pUnitNode);
			if(pLoopUnit && !pLoopUnit->IsGarrisoned() && pLoopUnit->getDomainType() != DOMAIN_AIR && !pLoopUnit->isInvisible(eActiveTeam,false))
			{
				setCenterUnit(pLoopUnit);
			}
		}
	}

	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pCenterUnit = getCenterUnit().pointer();
	pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
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

	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;

	oldUnitList.clear();

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		oldUnitList.push_back(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);
	}

	int iUnitListSize = (int) oldUnitList.size();
	for(int iVectorLoop = 0; iVectorLoop < (int) iUnitListSize; ++iVectorLoop)
	{
		pLoopUnit = GetPlayerUnit(oldUnitList[iVectorLoop]);
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
							// Unit not allowed to be here
#if defined(MOD_GLOBAL_STACKING_RULES)
							if(getNumFriendlyUnitsOfType(pLoopUnit) > /*1*/ getUnitLimit())
#else
							if(getNumFriendlyUnitsOfType(pLoopUnit) > /*1*/ GC.getPLOT_UNIT_LIMIT())
#endif
							{
								if (!pLoopUnit->jumpToNearestValidPlot())
								{
									pLoopUnit->kill(false);
									pLoopUnit = NULL;
								}
							}
							
							if (pLoopUnit != NULL)
							{
								if(!isValidDomainForLocation(*pLoopUnit) || !(pLoopUnit->canEnterTerritory(getTeam(), false /*bIgnoreRightOfPassage*/, isCity())))
								{
									if (!pLoopUnit->jumpToNearestValidPlot())
										pLoopUnit->kill(false);
								}
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
		for(int iVectorLoop = 0; iVectorLoop < (int) iUnitListSize; ++iVectorLoop)
		{
			pLoopUnit = GetPlayerUnit(oldUnitList[iVectorLoop]);
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
											pLoopUnit->kill(false);
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

//	--------------------------------------------------------------------------------
// Left-over method, primarily because it is exposed to Lua.
void CvPlot::nukeExplosion(int iDamageLevel, CvUnit*)
{
	CvUnitCombat::ApplyNuclearExplosionDamage(this, iDamageLevel);
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToArea(int iAreaID) const
{

	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getArea() == iAreaID)
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
	int iCurrArea;
	int iLastArea;
	CvPlot* pAdjacentPlot;
	int iI;

	iLastArea = FFreeList::INVALID_INDEX;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			iCurrArea = pAdjacentPlot->getArea();

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
	{
		return false;
	}

	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot == pPlot)
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToLand() const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(!(pAdjacentPlot->isWater()))
			{
				return true;
			}
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
	CvPlot* pAdjacentPlot;
	int iI;

	TerrainTypes eShallowWater = (TerrainTypes) GC.getSHALLOW_WATER_TERRAIN();

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getTerrainType() == eShallowWater)
			{
				return true;
			}
		}
	}

	return false;
}


#if defined(MOD_PROMOTIONS_CROSS_ICE)
//	--------------------------------------------------------------------------------
bool CvPlot::isAdjacentToIce() const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
bool CvPlot::isCoastalLand(int iMinWaterSize) const
{
#if !defined(MOD_BALANCE_CORE)
	CvPlot* pAdjacentPlot;
	int iI;
#endif
	if(isWater())
	{
		return false;
	}

	// If -1 was passed in (default argument) use min water size for ocean define
	if(iMinWaterSize == -1)
	{
		iMinWaterSize = GC.getMIN_WATER_SIZE_FOR_OCEAN();
	}
#if defined(MOD_BALANCE_CORE)
	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(this);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		const CvPlot* pAdjacentPlot = aPlotsToCheck[iCount];
#else
	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
#endif
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isWater())
			{
				if(iMinWaterSize <= 0)
				{
					return true;
				}
				CvLandmass* pAdjacentBodyOfWater = GC.getMap().getLandmass(pAdjacentPlot->getLandmass());
				if(pAdjacentBodyOfWater && pAdjacentBodyOfWater->getNumTiles() >= iMinWaterSize)
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvPlot::GetSizeLargestAdjacentWater() const
{
	CvPlot* pAdjacentPlot;
	int iI;
	int iRtnValue = 0;

	if(isWater())
	{
		return iRtnValue;
	}

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
	int iI;

	for(iI = 0; iI < MAX_PLAYERS; ++iI)
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
bool CvPlot::isLake() const
{
	CvLandmass* pLandmass = GC.getMap().getLandmass(m_iLandmass);

	if(pLandmass != NULL)
	{
		return pLandmass->isLake();
	}

	return false;
}


//	--------------------------------------------------------------------------------
// XXX precalculate this???
bool CvPlot::isFreshWater() const
{
#if !defined(MOD_BALANCE_CORE)
	CvPlot* pLoopPlot;
	int iDX, iDY;
#endif
	if(isWater() || isImpassable() || isMountain())
		return false;

#if defined(MOD_BALANCE_CORE)
		if(isRiver())
		return true;

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
				return true;
			}

			FeatureTypes feature_type = pLoopPlot->getFeatureType();

			if(feature_type != NO_FEATURE)
			{
				if(GC.getFeatureInfo(feature_type)->isAddsFreshWater())
				{
					return true;
				}
			}

#if defined(MOD_API_EXTENSIONS)
			ImprovementTypes improvement_type = pLoopPlot->getImprovementType();

			if(improvement_type != NO_IMPROVEMENT)
			{
				if(GC.getImprovementInfo(improvement_type)->IsAddsFreshWater())
				{
					return true;
				}
			}

			if (pLoopPlot->isCity() && pLoopPlot->getPlotCity()->isAddsFreshWater())
			{
				return true;
			}
#endif
		}
	}

	return false;
}
#else
	if(isRiver())
	{
		return true;
	}

	for(iDX = -1; iDX <= 1; iDX++)
	{
		for(iDY = -1; iDY <= 1; iDY++)
		{
			pLoopPlot	= plotXYWithRangeCheck(getX(), getY(), iDX, iDY, 1);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->isLake())
				{
					return true;
				}

				FeatureTypes feature_type = pLoopPlot->getFeatureType();

				if(feature_type != NO_FEATURE)
				{
					if(GC.getFeatureInfo(feature_type)->isAddsFreshWater())
					{
						return true;
					}
				}

#if defined(MOD_API_EXTENSIONS)
				ImprovementTypes improvement_type = pLoopPlot->getImprovementType();

				if(improvement_type != NO_IMPROVEMENT)
				{
					if(GC.getImprovementInfo(improvement_type)->IsAddsFreshWater())
					{
						return true;
					}
				}

				if (pLoopPlot->isCity() && pLoopPlot->getPlotCity()->isAddsFreshWater())
				{
					return true;
				}
#endif
			}
		}
	}

	return false;
}



#endif
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
		iLevel = GC.getTerrainInfo(getTerrainType())->getSeeFromLevel();
	}

	if(isMountain())
	{
		iLevel += GC.getMOUNTAIN_SEE_FROM_CHANGE();
	}

	if(isHills())
	{
		iLevel += GC.getHILLS_SEE_FROM_CHANGE();
	}

	if(isWater())
	{
		iLevel += GC.getSEAWATER_SEE_FROM_CHANGE();

		if(GET_TEAM(eTeam).isExtraWaterSeeFrom())
		{
			iLevel++;
		}
	}

	return iLevel;
}


//	--------------------------------------------------------------------------------
int CvPlot::seeThroughLevel(bool bIncludeShubbery) const
{
	int iLevel;

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iLevel = GC.getTerrainInfo(getTerrainType())->getSeeThroughLevel();

	if(bIncludeShubbery && getFeatureType() != NO_FEATURE)
	{
		iLevel += GC.getFeatureInfo(getFeatureType())->getSeeThroughChange();
	}

	if(isMountain())
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

	return iLevel;
}


//	--------------------------------------------------------------------------------
void CvPlot::changeSeeFromSight(TeamTypes eTeam, DirectionTypes eDirection, int iFromLevel, bool bIncrement, InvisibleTypes eSeeInvisible)
{
	CvPlot* pPlot;
	int iThroughLevel;

	iThroughLevel = seeThroughLevel();

	if(iFromLevel >= iThroughLevel)
	{
		pPlot = plotDirection(getX(), getY(), eDirection);

		if(pPlot != NULL)
		{
			if((iFromLevel > iThroughLevel) || (pPlot->seeFromLevel(eTeam) > iFromLevel))
			{
				pPlot->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, false);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
// while this looks more complex than the previous version, it should run much faster
void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, InvisibleTypes eSeeInvisible, DirectionTypes eFacingDirection, bool bBasedOnUnit)
{
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
					pPlotToCheck->changeVisibilityCount(eTeam, 1, eSeeInvisible, false /*bInformExplorationTracking*/, false);
					pPlotToCheck->changeVisibilityCount(eTeam, -1, eSeeInvisible, false /*bInformExplorationTracking*/, false);
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
							iFirstInwardLevel = pFirstInwardPlot->getScratchPad();
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
							iSecondInwardLevel = pSecondInwardPlot->getScratchPad();
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
							pPlotToCheck->setScratchPad(iHighestLevel);
							if(iSecondInwardLevel < iThisPlotLevel || ((iCenterLevel >= iSecondInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
							{								
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
							}
						}
						else if(fSecondDist - fFirstDist > 0.05)   // we are closer to the first point
						{
							int iHighestLevel = (iFirstInwardLevel > iThisPlotLevel) ? iFirstInwardLevel : iThisPlotLevel;
							pPlotToCheck->setScratchPad(iHighestLevel);
							if(iFirstInwardLevel < iThisPlotLevel || ((iCenterLevel >= iFirstInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
							{								
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
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
								pPlotToCheck->setScratchPad(iHighestLowestLevel + HALF_BLOCKED);
							}
							else
							{
								pPlotToCheck->setScratchPad(iHighestLevel);
							}
							if(iLowestInwardLevel < iThisPlotLevel || ((iCenterLevel >= iLowestInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
							{								
								pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
							}
						}
					}
					else if(iFirstInwardLevel != INVALID_RING && !bFirstHalfBlocked)
					{
						int iHighestLevel = (iFirstInwardLevel > iThisPlotLevel) ? iFirstInwardLevel : iThisPlotLevel;
						pPlotToCheck->setScratchPad(iHighestLevel);
						if(iFirstInwardLevel < iThisPlotLevel || ((iCenterLevel >= iFirstInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{							
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
						}
					}
					else if(iSecondInwardLevel != INVALID_RING && !bSecondHalfBlocked)
					{
						int iHighestLevel = (iSecondInwardLevel > iThisPlotLevel) ? iSecondInwardLevel : iThisPlotLevel;
						pPlotToCheck->setScratchPad(iHighestLevel);
						if(iSecondInwardLevel < iThisPlotLevel || ((iCenterLevel >= iSecondInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{							
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
						}
					}
					else if(iFirstInwardLevel != INVALID_RING)
					{
						int iHighestLevel = (iFirstInwardLevel > iThisPlotLevel) ? iFirstInwardLevel : iThisPlotLevel;
						pPlotToCheck->setScratchPad(iHighestLevel);
						if(iFirstInwardLevel < iThisPlotLevel || ((iCenterLevel >= iFirstInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
						}
					}
					else if(iSecondInwardLevel != INVALID_RING)
					{
						int iHighestLevel = (iSecondInwardLevel > iThisPlotLevel) ? iSecondInwardLevel : iThisPlotLevel;
						pPlotToCheck->setScratchPad(iHighestLevel);
						if(iSecondInwardLevel < iThisPlotLevel || ((iCenterLevel >= iSecondInwardLevel) && (thisRing < iRangeWithOneExtraRing)))
						{							
							pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
						}
					}
					else // I have no idea how this can happen, but...
					{
						// set our value in the scratch pad
						pPlotToCheck->setScratchPad(iThisPlotLevel);
					}
				}
				else // this is the center point
				{					
					pPlotToCheck->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), eSeeInvisible, true, (bBasedOnUnit && thisRing < 2)?true:false);
					pPlotToCheck->setScratchPad(0);
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
	iRange++;

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
		if(shouldProcessDisplacementPlot(dx, dy, iRange - 1, eFacingDirection))
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
		changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION, false);

		// if this tile is owned by a minor share the visibility with my ally
		if(pCity)
		{
			for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes ePlayer = (PlayerTypes)ui;
				if(GET_PLAYER(ePlayer).GetEspionage()->HasEstablishedSurveillanceInCity(pCity))
				{
					changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION, false);
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
				changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), bIncrement, NO_INVISIBLE, NO_DIRECTION, false);
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
				changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->visibilityRange(), bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true));
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
					changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->reconRange(), bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true));
#else
					changeAdjacentSight(pLoopUnit->getTeam(), iRange, bIncrement, pLoopUnit->getSeeInvisibleType(), pLoopUnit->getFacingDirection(true));
#endif
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvPlot::updateSeeFromSight(bool bIncrement)
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
		if(isRiverSide())
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
		if(!isAdjacentToLand())
		{
			return false;
		}
	}


	return true;
}


//	--------------------------------------------------------------------------------
bool CvPlot::canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam, bool) const
{
	CvPlot* pLoopPlot;
	bool bValid;
	int iI;

	CvAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
#if !defined(MOD_GLOBAL_ALPINE_PASSES)
	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");
#endif

	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	if(pkImprovementInfo == NULL)
	{
		return false;
	}

#if defined(MOD_GLOBAL_ALPINE_PASSES)
	if (MOD_GLOBAL_ALPINE_PASSES && pkImprovementInfo->IsMountainsMakesValid() && isMountain()) {
		return true;
	}

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");
#endif

	bValid = false;

	if(isCity())
	{
		return false;
	}

	if(isImpassable() || isMountain())
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

	ResourceTypes thisResource = getResourceType(eTeam);
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

	const bool bIsFreshWater = isFreshWater();

	if(pkImprovementInfo->IsNoFreshWater() && bIsFreshWater)
	{
		return false;
	}

	if(pkImprovementInfo->IsRequiresFlatlands() && !isFlatlands())
	{
		return false;
	}

	if(pkImprovementInfo->IsRequiresFlatlandsOrFreshWater() && !isFlatlands() && !bIsFreshWater)
	{
		return false;
	}

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

	if(pkImprovementInfo->IsFreshWaterMakesValid() && bIsFreshWater)
	{
		bValid = true;
	}

	if(pkImprovementInfo->IsRiverSideMakesValid() && isRiverSide())
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
		if(calculateNatureYield(((YieldTypes)iI), eTeam) < pkImprovementInfo->GetPrereqNatureYield(iI))
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
	bool bValid;

	// Can't build nothing!
	if(eBuild == NO_BUILD)
	{
		return false;
	}

	bValid = false;

	// Repairing an Improvement that's been pillaged
	CvBuildInfo& thisBuildInfo = *GC.getBuildInfo(eBuild);
	if(thisBuildInfo.isRepair())
	{
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
		else
		{
			return false;
		}
	}

	eImprovement = ((ImprovementTypes)(thisBuildInfo.getImprovement()));

	// Improvement
	if(eImprovement != NO_IMPROVEMENT)
	{
		// Player must be able to build this Improvement
		if(!canHaveImprovement(eImprovement, eTeam, bTestVisible))
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
						if (getTeam() != eTeam && !isAdjacentTeam(eTeam, false))
						{
							return false;
						}
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
									CvCity* pCity = GET_PLAYER(getOwner()).getCapitalCity();
									if(pCity != NULL)
									{
#if defined(MOD_GLOBAL_CITY_WORKING)
										for(int iI = 0; iI < pCity->GetNumWorkablePlots(); iI++)
#else
										for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
#endif
										{
											CvPlot* pCityPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);

											if(pCityPlot != NULL)
											{
												ImprovementTypes eEmbassy = (ImprovementTypes)GC.getEMBASSY_IMPROVEMENT();
												ImprovementTypes CSImprovement = pCityPlot->getImprovementType();
												if(CSImprovement == eEmbassy)
												{
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

		bValid = true;
	}

	// In general, cannot clear features in the lands of players that aren't on our team or that we're at war with
	if(getFeatureType() != NO_FEATURE)
	{
		if(GC.getBuildInfo(eBuild)->isFeatureRemove(getFeatureType()))
		{
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

	*ppCity = getWorkingCity();

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

	// Distance mod
	iProduction -= (std::max(0, (plotDistance(getX(), getY(), (*ppCity)->getX(), (*ppCity)->getY()) - 2)) * 5);

	iProduction *= std::max(0, (GET_PLAYER((*ppCity)->getOwner()).getFeatureProductionModifier() + 100));
	iProduction /= 100;

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
UnitHandle CvPlot::getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove, bool bNoncombatAllowed)
{
	// accesses another internal method, user code sees this (mutable) method and const guarantees are maintained
	return (const_cast<const CvPlot*>(this)->getBestDefender(eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, bNoncombatAllowed));
}

//	--------------------------------------------------------------------------------
const UnitHandle CvPlot::getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove, bool bNoncombatAllowed) const
{
	const IDInfo* pUnitNode;
	const UnitHandle pLoopUnit;
	const UnitHandle pBestUnit;

	pUnitNode = headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && (bNoncombatAllowed || pLoopUnit->IsCanDefend()) && pLoopUnit != pAttacker)	// Does the unit exist, and can it fight, or do we care if it can't fight?
		{
			if((eOwner ==  NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
			{
				if((eAttackingPlayer == NO_PLAYER) || !(pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false)))
				{
					if(!bTestAtWar || eAttackingPlayer == NO_PLAYER || pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this)))
					{
						if(!bTestPotentialEnemy || (eAttackingPlayer == NO_PLAYER) ||  pLoopUnit->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isPotentialEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this)))
						{
							if(!bTestCanMove || (pLoopUnit->canMove() && !(pLoopUnit->isCargo())))
							{
								if((pAttacker == NULL) || (pAttacker->getDomainType() != DOMAIN_AIR) || (pLoopUnit->getDamage() < pAttacker->GetRangedCombatLimit()))
								{
									if(pLoopUnit->isBetterDefenderThan(pBestUnit.pointer(), pAttacker))
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
	}

	return pBestUnit;
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlot::getSelectedUnit()
{
	return const_cast<CvUnit*>(const_cast<const CvPlot*>(this)->getSelectedUnit());
}

//	--------------------------------------------------------------------------------
const CvUnit* CvPlot::getSelectedUnit() const
{
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

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
	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = headUnitNode();

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

#if defined(MOD_BALANCE_CORE)

//	--------------------------------------------------------------------------------
int CvPlot::defenseModifier(TeamTypes eDefender, bool, bool bHelp) const
{
	int iModifier = 0;

	// Plot type
	if(isHills() || isMountain())
		iModifier += /*25*/ GC.getHILLS_EXTRA_DEFENSE();

	// Feature
	if(getFeatureType() != NO_FEATURE)
		iModifier += GC.getFeatureInfo(getFeatureType())->getDefenseModifier();

	// Terrain
	if(getTerrainType() != NO_TERRAIN)
		iModifier += GC.getTerrainInfo(getTerrainType())->getDefenseModifier();

	// Improvements count extra, but include them for tooltips only if the tile is revealed
	ImprovementTypes eImprovement = bHelp ? getRevealedImprovementType(GC.getGame().getActiveTeam()) : getImprovementType();
	if(eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
	{
		if(eDefender != NO_TEAM && (getTeam() == NO_TEAM || GET_TEAM(eDefender).isFriendlyTerritory(getTeam())))
		{
			CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eImprovement);
			if (pkImprovement)
				iModifier += pkImprovement->GetDefenseModifier();
		}
	}

	// Cities also give a boost
	if(!bHelp)
	{
		const CvCity* pCity = getPlotCity();

		if(pCity != NULL)
		{
			iModifier += pCity->getStrengthValue()/100;
		}
	}

	return iModifier;
}

#else

//	--------------------------------------------------------------------------------
int CvPlot::defenseModifier(TeamTypes eDefender, bool, bool bHelp) const
{
	CvCity* pCity;
	ImprovementTypes eImprovement;
	int iModifier;

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	// Can only get Defensive Bonus from ONE thing - they don't stack

	// Hill (and mountain)
	if(isHills() || isMountain())
	{
		iModifier = /*25*/ GC.getHILLS_EXTRA_DEFENSE();
	}
	// Feature
	else if(getFeatureType() != NO_FEATURE)
	{
		iModifier = GC.getFeatureInfo(getFeatureType())->getDefenseModifier();
	}
	// Terrain
	else
	{
		iModifier = GC.getTerrainInfo(getTerrainType())->getDefenseModifier();

		// Flat land gives defensive PENALTY
		if(!isWater())
		{
			iModifier += /*-25*/ GC.getFLAT_LAND_EXTRA_DEFENSE();
		}
	}

	if(bHelp)
	{
		eImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam());
	}
	else
	{
		eImprovement = getImprovementType();
	}

	if(eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
	{
		if(eDefender != NO_TEAM && (getTeam() == NO_TEAM || GET_TEAM(eDefender).isFriendlyTerritory(getTeam())))
		{
			CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eImprovement);
			if (pkImprovement)
				iModifier += pkImprovement->GetDefenseModifier();
		}
	}

	if(!bHelp)
	{
		pCity = getPlotCity();

		if(pCity != NULL)
		{
		}
	}

	return iModifier;
}

#endif //defined MOD_BALANCE_CORE_MILITARY

//	---------------------------------------------------------------------------
int CvPlot::movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining /*= 0*/) const
{
	return CvUnitMovement::MovementCost(pUnit, pFromPlot, this, pUnit->baseMoves(isWater()?DOMAIN_SEA:NO_DOMAIN), pUnit->maxMoves(), iMovesRemaining);
}

//	---------------------------------------------------------------------------
int CvPlot::MovementCostNoZOC(const CvUnit* pUnit, const CvPlot* pFromPlot, int iMovesRemaining /*= 0*/) const
{
	return CvUnitMovement::MovementCostNoZOC(pUnit, pFromPlot, this, pUnit->baseMoves(isWater()?DOMAIN_SEA:NO_DOMAIN), pUnit->maxMoves(), iMovesRemaining);
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
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
bool CvPlot::IsAdjacentOwnedByOtherTeam(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getTeam() != NO_TEAM && pAdjacentPlot->getTeam() != eTeam)
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
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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

//	--------------------------------------------------------------------------------
CvCity* CvPlot::GetAdjacentFriendlyCity(TeamTypes eTeam, bool bLandOnly) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
CvCity* CvPlot::GetAdjacentCity(bool bLandOnly) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isCity())
			{
				if(!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					CvCity* pCity = pAdjacentPlot->getPlotCity();
					if(pCity)
					{
						return pCity;
					}
				}
			}
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Number of adjacent tiles owned by another team (or unowned)
int CvPlot::GetNumAdjacentDifferentTeam(TeamTypes eTeam, bool bIgnoreWater) const
{
	CvPlot* pAdjacentPlot;
	int iI;
	int iRtnValue = 0;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(bIgnoreWater && pAdjacentPlot->isWater())
			{
				continue;
			}

			if(pAdjacentPlot->getTeam() != eTeam)
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

int CvPlot::GetNumAdjacentMountains() const
{
	CvPlot* pAdjacentPlot;
	int iI;
	int iNumMountains = 0;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
#if defined(MOD_BALANCE_CORE_SETTLER)
int CvPlot::countPassableLandNeighbors(CvPlot** aPassableNeighbors) const
{
	int iPassable = 0;
	CvPlot* pAdjacentPlot;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			if(!pAdjacentPlot->isWater() && !pAdjacentPlot->isImpassable())
			{
				if (aPassableNeighbors)
					aPassableNeighbors[iPassable] = pAdjacentPlot;
				iPassable++;
			}
		}
	}
	return iPassable;
}

bool CvPlot::IsChokePoint()
{
	//only passable land plots can be chokepoints
	if(isWater() || isImpassable())
		return false;

	CvPlot* aPassableNeighbors[NUM_DIRECTION_TYPES];
	int iPassable = countPassableLandNeighbors(aPassableNeighbors);

	//a plot is a chokepoint if it has between two and three passable land plots as neighbors
	//(with four passable plots it's not a real chokepoint ...)
	if (iPassable<2 || iPassable>4)
		return false;

	//each adjacent passable plot must have at least 3 passable neighbors (anti peninsula/dead end valley check)
	int iPassableAndNoPeninsula = 0;
	CvPlot* aPassableNeighborsNonPeninsula[NUM_DIRECTION_TYPES];
	for (int iI = 0; iI<iPassable; iI++)
	{
		if (aPassableNeighbors[iI]->countPassableLandNeighbors(NULL)>2)
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
#endif
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
const CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	return const_cast<CvPlot*>(this)->plotCheck(funcA, iData1A, iData2A, eOwner, eTeam, funcB, iData1B, iData2B);
}

//	--------------------------------------------------------------------------------
CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B)
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
bool CvPlot::isVisibleToCivTeam() const
{
	int iI;

	for(iI = 0; iI < MAX_CIV_TEAMS; ++iI)
	{
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
bool CvPlot::isVisibleToEnemyTeam(TeamTypes eFriendlyTeam) const
{
	int iI;

	for(iI = 0; iI < MAX_CIV_TEAMS; ++iI)
	{
		CvTeam& kTeam = GET_TEAM((TeamTypes)iI);

		if(kTeam.isAlive())
		{
			if(kTeam.isAtWar(eFriendlyTeam))
			{
				if(isVisible(((TeamTypes)iI)))
				{
					return true;
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isVisibleToWatchingHuman() const
{
	int iI;

	for(iI = 0; iI < MAX_CIV_PLAYERS; ++iI)
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

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
bool CvPlot::isAdjacentVisible(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isVisible(eTeam))
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

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
int CvPlot::getNumAdjacentNonvisible(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iCount = 0;

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(!pAdjacentPlot->isVisible(eTeam))
			{
				iCount++;
			}
		}
	}

	return iCount;
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
	// Make sure the player's redo their goody hut searches
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(GET_PLAYER((PlayerTypes)i).isAlive())
		{
			GET_PLAYER((PlayerTypes)i).GetEconomicAI()->m_bExplorationPlotsDirty = true;
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvPlot::isFriendlyCity(const CvUnit& kUnit, bool) const
{
	if(!getPlotCity())
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

#if defined(MOD_GLOBAL_PASSABLE_FORTS)
bool CvPlot::isFriendlyCityOrPassableImprovement(const CvUnit& kUnit, bool) const
{
	ImprovementTypes eImprovement = getImprovementType();
	CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
	bool bIsPassable = MOD_GLOBAL_PASSABLE_FORTS && pkImprovementInfo != NULL && pkImprovementInfo->IsMakesPassable();
	bool bIsCityOrPassable = getPlotCity() || bIsPassable;

	if (!bIsCityOrPassable) {
		// Not a city or a fort
		return false;
	}

	if (IsFriendlyTerritory(kUnit.getOwner())) {
		// In friendly lands (ours, an allied CS or a major with open borders)
		return true;
	}

	if (getTeam() == NO_TEAM) {
		// In no-mans land ...
		TeamTypes eTeam = kUnit.getTeam();

		if (getNumUnits() == 0) {
			return true;
		}

		// ... make sure there are only friendly units here
		const IDInfo* pUnitNode = headUnitNode();
		while (pUnitNode != NULL) {
			if ((pUnitNode->eOwner >= 0) && pUnitNode->eOwner < MAX_PLAYERS) {
				CvUnit* pLoopUnit = (GET_PLAYER(pUnitNode->eOwner).getUnit(pUnitNode->iID));

				if (pLoopUnit && (eTeam == pLoopUnit->getTeam())) {
					// Any friendly unit will do
					return true;
				}
			}

			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	return false;
}
#endif

//	--------------------------------------------------------------------------------
/// Is this a plot that's friendly to our team? (owned by us or someone we have Open Borders with)
bool CvPlot::IsFriendlyTerritory(PlayerTypes ePlayer) const
{
	// No friendly territory for barbs!
	if(GET_PLAYER(ePlayer).isBarbarian())
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
	CvCity* pWorkingCity;

	pWorkingCity = getWorkingCity();

	if(pWorkingCity != NULL)
	{
		return pWorkingCity->GetCityCitizens()->IsWorkingPlot(this);
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
inline static bool isPotentialEnemy(const CvUnit* pUnit, TeamTypes eOtherTeam, bool bAlwaysHostile)
{
	if(pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();
	return (bAlwaysHostile ? eOtherTeam != eOurTeam : isPotentialEnemy(eOtherTeam, eOurTeam));
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
	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();
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
CvUnit* CvPlot::getVisibleEnemyDefender(PlayerTypes ePlayer)
{
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
			{
				if(pLoopUnit->IsCanDefend() && isEnemy(pLoopUnit, eTeam, false))
				{
					return const_cast<CvUnit*>(pLoopUnit);
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
		CvAssertMsg(ePlayer != NO_PLAYER, "Player must be valid");
		int iCount = 0;

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit)
			{
				if(pLoopUnit->getOwner() == ePlayer && pLoopUnit->IsCanDefend())
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

//	-----------------------------------------------------------------------------------------------
int CvPlot::getNumVisibleEnemyDefenders(const CvUnit* pUnit) const
{
	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();
		bool bAlwaysHostile = pUnit->isAlwaysHostile(*this);
		int iCount = 0;

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
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

//	-----------------------------------------------------------------------------------------------
int CvPlot::getNumVisiblePotentialEnemyDefenders(const CvUnit* pUnit) const
{
	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		int iCount = 0;
		TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();
		bool bAlwaysHostile = pUnit->isAlwaysHostile(*this);

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
			{
				if(pLoopUnit->IsCanDefend() && isPotentialEnemy(pLoopUnit, eTeam, bAlwaysHostile))
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

//	-----------------------------------------------------------------------------------------------
bool CvPlot::isVisibleEnemyUnit(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer != NO_PLAYER, "Source player must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
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
	CvAssertMsg(pUnit, "Source unit must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnit && pUnitNode)
	{
		TeamTypes eTeam = GET_PLAYER(pUnit->getOwner()).getTeam();
		bool bAlwaysHostile = pUnit->isAlwaysHostile(*this);

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
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
	CvAssertMsg(ePlayer != NO_PLAYER, "Source player must be valid");
	const IDInfo* pUnitNode = m_units.head();
	if(pUnitNode)
	{
		TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

		do
		{
			const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
			pUnitNode = m_units.next(pUnitNode);

			if(pLoopUnit && !pLoopUnit->isInvisible(eTeam, false))
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

//	--------------------------------------------------------------------------------
/// Is there an enemy Unit on this Plot?  (Don't care if we can actually see it or not, so be careful with this)
#if defined(MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES)
bool CvPlot::IsActualEnemyUnit(PlayerTypes ePlayer, bool bCombatUnitsOnly, bool bNavalUnitsOnly) const
#else
bool CvPlot::IsActualEnemyUnit(PlayerTypes ePlayer, bool bCombatUnitsOnly) const
#endif
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam& kTeam = GET_TEAM(eTeam);

	for(int iUnitLoop = 0; iUnitLoop < getNumUnits(); iUnitLoop++)
	{
		CvUnit* pkUnit = getUnitByIndex(iUnitLoop);
		if(pkUnit)
		{
			if(kTeam.isAtWar(pkUnit->getTeam()))
			{
				if(!bCombatUnitsOnly || pkUnit->IsCombatUnit())
				{
#if defined(MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES)
					if (!bNavalUnitsOnly || pkUnit->getDomainType() == DOMAIN_SEA)
					{
#endif
						return true;
#if defined(MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES)
					}
#endif
				}
			}
		}
	}

	return false;
}

#if defined(MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES)
//	--------------------------------------------------------------------------------
/// Is there an allied Unit on this Plot?  (Don't care if we can actually see it or not, so be careful with this)
bool CvPlot::IsActualAlliedUnit(PlayerTypes ePlayer, bool bCombatUnitsOnly) const
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	for (int iUnitLoop = 0; iUnitLoop < getNumUnits(); iUnitLoop++) {
		CvUnit* pkUnit = getUnitByIndex(iUnitLoop);
		if (pkUnit) {
			PlayerTypes eOwner = ((PlayerTypes) pkUnit->getOwner());
			CvPlayer& kOwner = GET_PLAYER(eOwner);
			
			if (ePlayer == eOwner || kPlayer.getTeam() == kOwner.getTeam()) {
				// It's our unit (or a team member's)
				return (!bCombatUnitsOnly || pkUnit->IsCombatUnit());
			} else {
				// Is it an allied minor or DoF major?
				if (kOwner.isMinorCiv() && kOwner.GetMinorCivAI()->GetAlly() == ePlayer) {
					return (!bCombatUnitsOnly || pkUnit->IsCombatUnit());
				} else if (!kOwner.isBarbarian() && kOwner.GetDiplomacyAI()->IsDoFAccepted(ePlayer)) {
					return (!bCombatUnitsOnly || pkUnit->IsCombatUnit());
				}
			}
		}
	}

	return false;
}
#endif

//	--------------------------------------------------------------------------------
// Used to restrict number of units allowed on a plot at one time
int CvPlot::getNumFriendlyUnitsOfType(const CvUnit* pUnit, bool bBreakOnUnitLimit) const
{
	int iNumUnitsOfSameType = 0;

	bool bCombat = false;

	// slewis - trying to break the 1upt for trade units
	if (pUnit->isTrade())
	{
		return 0;
	}

	if(pUnit->IsCombatUnit())
	{
		bCombat = true;
	}

	bool bPretendEmbarked = false;
	if(isWater() && pUnit->canEmbarkOnto(*pUnit->plot(), *this))
	{
		bPretendEmbarked = true;
	}

	CvTeam& kUnitTeam = GET_TEAM(pUnit->getTeam());

	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

#if defined(MOD_GLOBAL_STACKING_RULES)
	int iPlotUnitLimit = getUnitLimit();
#else
	int iPlotUnitLimit = GC.getPLOT_UNIT_LIMIT();
#endif

	while(pUnitNode != NULL)
	{
		pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit != NULL)
		{
			// Don't include an enemy unit, or else it won't let us attack it :)
			if(!kUnitTeam.isAtWar(pLoopUnit->getTeam()))
			{
				// Units of the same type OR Units belonging to different civs
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS)
				if((!MOD_GLOBAL_BREAK_CIVILIAN_RESTRICTIONS && pUnit->getOwner() != pLoopUnit->getOwner()) || pLoopUnit->AreUnitsOfSameType(*pUnit, bPretendEmbarked))
#else
				if(pUnit->getOwner() != pLoopUnit->getOwner() || pLoopUnit->AreUnitsOfSameType(*pUnit, bPretendEmbarked))
#endif
				{
					// We should allow as many cargo units as we want
					if(!pLoopUnit->isCargo())
					{
						// Unit is the same domain & combat type, not allowed more than the limit
						iNumUnitsOfSameType++;
					}
				}

				// Does the calling function want us to break out? (saves processing time)
				if(bBreakOnUnitLimit)
				{
					if(iNumUnitsOfSameType > iPlotUnitLimit)
					{
						return iNumUnitsOfSameType;
					}
				}
			}
		}
	}
	return iNumUnitsOfSameType;
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
		if(!pUnit->isEnemy(getTeam(), this) || pUnit->isEnemyRoute())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetTradeRoute(PlayerTypes ePlayer, bool bActive)
{
	bool bWasTradeRoute = IsTradeRoute();

	uint uiNewBitValue = (1 << ePlayer);
	if(bActive)
	{
		m_uiTradeRouteBitFlags |= uiNewBitValue;
	}
	else
	{
		m_uiTradeRouteBitFlags &= ~uiNewBitValue;
	}

	if(IsTradeRoute() != bWasTradeRoute)
	{
		for(int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if(GET_TEAM((TeamTypes)iI).isAlive() && GC.getGame().getActiveTeam() == (TeamTypes)iI)
			{
				if(isVisible((TeamTypes)iI))
				{
					setLayoutDirty(true);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvPlot::IsTradeRoute(PlayerTypes ePlayer) const
{
	if(ePlayer == NO_PLAYER)
	{
		if(m_uiTradeRouteBitFlags > 0)
		{
			return true;
		}
	}
	else
	{
		uint uiNewBitValue = (1 << ePlayer);
		if(m_uiTradeRouteBitFlags & uiNewBitValue)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvPlot::isValidDomainForLocation(const CvUnit& unit) const
{
	if(isValidDomainForAction(unit))
	{
		return true;
	}

	if (unit.getDomainType() == DOMAIN_AIR && unit.canLoad(*this))
	{
		return true;
	}
	
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	if (unit.getDomainType() == DOMAIN_LAND && isIce() && unit.canCrossIce()) {
		return true;
	}
#endif

	if (unit.getDomainType() == DOMAIN_AIR && unit.canLoad(*this))
	{
		return true;
	}

#if defined(MOD_GLOBAL_PASSABLE_FORTS)
	return (unit.getDomainType() == DOMAIN_SEA) ? isFriendlyCityOrPassableImprovement(unit, true) : isCity();
#else
	return isCity();
#endif
}


//	--------------------------------------------------------------------------------
bool CvPlot::isValidDomainForAction(const CvUnit& unit) const
{
	switch(unit.getDomainType())
	{
	case DOMAIN_SEA:
		return (isWater() || unit.canMoveAllTerrain());
		break;

	case DOMAIN_AIR:
		return false;
		break;

	case DOMAIN_LAND:
	case DOMAIN_IMMOBILE:
#if defined(MOD_BUGFIX_HOVERING_PATHFINDER)
		// Only hover over shallow water or ice
		{ bool bOK = (!isWater() || (unit.IsHoveringUnit() && isShallowWater()) || unit.canMoveAllTerrain() || unit.isEmbarked() || IsAllowsWalkWater());
#if defined(MOD_PROMOTIONS_CROSS_ICE)
		bOK = bOK || (unit.IsHoveringUnit() && isIce());
#endif
		return bOK; }
#else
		return (!isWater() || unit.IsHoveringUnit() || unit.canMoveAllTerrain() || unit.isEmbarked() || IsAllowsWalkWater());
#endif
		break;

	default:
		CvAssert(false);
		break;
	}

	return false;
}

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
CvArea* CvPlot::waterArea() const
{
	CvArea* pBestArea;
	CvPlot* pAdjacentPlot;
	int iValue;
	int iBestValue;
	int iI;

	if(isWater())
	{
		return area();
	}

	iBestValue = 0;
	pBestArea = NULL;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isWater())
			{
				iValue = pAdjacentPlot->area()->getNumTiles();

				if(iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pAdjacentPlot->area();
				}
			}
		}
	}

	return pBestArea;
}

//	--------------------------------------------------------------------------------
CvArea* CvPlot::secondWaterArea() const
{

	CvArea* pWaterArea = waterArea();
	CvArea* pBestArea;
	CvPlot* pAdjacentPlot;
	int iValue;
	int iBestValue;
	int iI;

	CvAssert(!isWater());

	iBestValue = 0;
	pBestArea = NULL;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isWater() && (pAdjacentPlot->getArea() != pWaterArea->GetID()))
			{
				iValue = pAdjacentPlot->area()->getNumTiles();

				if(iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pAdjacentPlot->area();
				}
			}
		}
	}

	return pBestArea;

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
int CvPlot::getFeatureVariety() const
{
	return m_iFeatureVariety;
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

	iUpgradeLeft = ((100 * GC.getGame().getImprovementUpgradeTime(eImprovement, const_cast<CvPlot*>(this))) - ((getImprovementType() == eImprovement) ? getUpgradeProgress() : 0));
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
#endif

#if defined(MOD_GLOBAL_STACKING_RULES)
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
	int aiShuffle[4];

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

#if defined(MOD_GLOBAL_CITY_WORKING)
	for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
#else
	for(iI = 0; iI < NUM_CITY_PLOTS; ++iI)
#endif
	{
		pLoopPlot = plotCity(getX(), getY(), iI);

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

#if defined(MOD_GLOBAL_CITY_WORKING)
	for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
#else
	for(iI = 0; iI < NUM_CITY_PLOTS; ++iI)
#endif
	{
		pLoopPlot = plotCity(getX(), getY(), iI);

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
		PlayerTypes eOldOwner = getOwner();;

		GC.getGame().addReplayMessage(REPLAY_MESSAGE_PLOT_OWNER_CHANGE, eNewValue, "", getX(), getY());

		pOldCity = getPlotCity();

		{
			setOwnershipDuration(0);

			// Plot was owned by someone else
			if(isOwned())
			{
				changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);

				// if this tile is owned by a minor share the visibility with my ally
				if(pOldCity)
				{
					for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
					{
						PlayerTypes ePlayer = (PlayerTypes)ui;
						if(GET_PLAYER(ePlayer).GetEspionage()->HasEstablishedSurveillanceInCity(pOldCity))
						{
							changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
						}
					}
				}

				if(eOldOwner >= MAX_MAJOR_CIVS && eOldOwner != BARBARIAN_PLAYER)
				{
					CvPlayer& thisPlayer = GET_PLAYER(eOldOwner);
					CvMinorCivAI* pMinorCivAI = thisPlayer.GetMinorCivAI();
					if(pMinorCivAI && pMinorCivAI->GetAlly() != NO_PLAYER)
					{
						changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
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
							}
						}
					}
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

					if(eOldOwner != NO_PLAYER && eResourceFromImprovement != NO_IMPROVEMENT)
					{
						GET_PLAYER(eOldOwner).changeNumResourceTotal(eResourceFromImprovement, -1, true);
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

					if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
					{
						if(getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType())->IsImprovementResourceTrade(getResourceType()))
						{
							if(!IsImprovementPillaged())
							{
								GET_PLAYER(getOwner()).changeNumResourceTotal(getResourceType(), -getNumResourceForPlayer(getOwner()));
							}
						}
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

			setWorkingCityOverride(NULL);
			updateWorkingCity();

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

				changeAdjacentSight(getTeam(), GC.getPLOT_VISIBILITY_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);

				// if this tile is owned by a minor share the visibility with my ally
				if(pOldCity)
				{
					for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
					{
						PlayerTypes ePlayer = (PlayerTypes)ui;
						if(GET_PLAYER(ePlayer).GetEspionage()->HasEstablishedSurveillanceInCity(pOldCity))
						{
							changeAdjacentSight(GET_PLAYER(ePlayer).getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
						}
					}
				}

				if(eNewValue >= MAX_MAJOR_CIVS && eNewValue != BARBARIAN_PLAYER)
				{
					CvPlayer& thisPlayer = GET_PLAYER(eNewValue);
					CvMinorCivAI* pMinorCivAI = thisPlayer.GetMinorCivAI();
					if(pMinorCivAI && pMinorCivAI->GetAlly() != NO_PLAYER)
					{
						changeAdjacentSight(GET_PLAYER(pMinorCivAI->GetAlly()).getTeam(), GC.getPLOT_VISIBILITY_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
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
							}
						}
					}
#if defined(MOD_BALANCE_CORE)
					//Resource from improvement - change ownership if needed.
					ResourceTypes eResourceFromImprovement = (ResourceTypes)pImprovementInfo->GetResourceFromImprovement();

					if(eResourceFromImprovement != NO_RESOURCE)
					{
						GET_PLAYER(eNewValue).changeNumResourceTotal(eResourceFromImprovement, 1, true);

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
					GET_PLAYER(eNewValue).changeImprovementCount(getImprovementType(), 1);

					// Add vote
					CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(getImprovementType());
					if (pImprovementInfo != NULL && pImprovementInfo->GetCityStateExtraVote() > 0)
					{
						if (GetPlayerThatBuiltImprovement() != NO_PLAYER)
						{
							if (GET_PLAYER(eNewValue).isMinorCiv())
							{
								GET_PLAYER(GetPlayerThatBuiltImprovement()).ChangeImprovementLeagueVotes(pImprovementInfo->GetCityStateExtraVote());
								SetImprovementEmbassy(true);
							}
						}
					}
						// Maintenance change!
					if(MustPayMaintenanceHere(eNewValue))
					{
						GET_PLAYER(eNewValue).GetTreasury()->ChangeBaseImprovementGoldMaintenance(GC.getImprovementInfo(getImprovementType())->GetGoldMaintenance());
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
						if(getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType())->IsImprovementResourceTrade(getResourceType()))
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
				if(GET_TEAM((TeamTypes)iI).isAlive())
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
					if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder())
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
#if defined(MOD_BALANCE_CORE_HAPPINESS)
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(GET_PLAYER(getOwner()).isHuman())
						{
							GET_PLAYER(getOwner()).CalculateHappiness();
						}
					}
					else
					{
#endif
					GET_PLAYER(getOwner()).DoUpdateHappiness();
#if defined(MOD_BALANCE_CORE_HAPPINESS)
					}
#endif
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
bool CvPlot::IsHomeFrontForPlayer(PlayerTypes ePlayer) const
{
	// Owned?
	if(isOwned())
	{
		if(getOwner() == ePlayer)
		{
			return true;
		}
	}

	CvCity* pLoopCity;
	int iCityLoop;

	int iRange = GC.getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT();

	// Not owned by this player, so we have to check things the hard way, and see how close the Plot is to any of this Player's Cities
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
	{
		if(::plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY()) < iRange)
		{
			return true;
		}
	}

	return false;
}

#if defined(MOD_GLOBAL_ADJACENT_BLOCKADES)
//	--------------------------------------------------------------------------------
bool CvPlot::isBlockaded(PlayerTypes ePlayer)
{
	if (!isWater()) {
		return false;
	}

	// An enemy ship on the plot trumps all
	if (getVisibleEnemyDefender(ePlayer)) {
		return true;
	}

#if defined(MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES)
	if (MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES) {
		// An allied ship on the plot secures the plot
		if (IsActualAlliedUnit(ePlayer)) {
			return false;
		}
	}
#endif

	// An enemy ship adjacent to the plot blockades it
	for (int iEnemyLoop = 0; iEnemyLoop < NUM_DIRECTION_TYPES; ++iEnemyLoop) {
		CvPlot* pEnemyPlot = plotDirection(getX(), getY(), ((DirectionTypes)iEnemyLoop));

#if defined(MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES)
		if (pEnemyPlot && (pEnemyPlot->isWater() || pEnemyPlot->isCity()) && pEnemyPlot->IsActualEnemyUnit(ePlayer, true, true)) {
#else
		if (pEnemyPlot && (pEnemyPlot->isWater() || pEnemyPlot->isCity()) && pEnemyPlot->IsActualEnemyUnit(ePlayer)) {
#endif
			return true;
		}
	}

#if defined(MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES)
	if (MOD_GLOBAL_ALLIES_BLOCK_BLOCKADES) {
		// An allied ship adjacent to the plot secures it
		for (int iAlliedLoop = 0; iAlliedLoop < NUM_DIRECTION_TYPES; ++iAlliedLoop) {
			CvPlot* pAlliedPlot = plotDirection(getX(), getY(), ((DirectionTypes)iAlliedLoop));

			if (pAlliedPlot && (pAlliedPlot->isWater() || pAlliedPlot->isCity()) && pAlliedPlot->IsActualAlliedUnit(ePlayer)) {
				return false;
			}
		}
	}
#endif

	// An enemy ship 2 tiles away blockades it
	int iRange = 2;
	CvPlot* pLoopPlot = NULL;

	for (int iDX = -iRange; iDX <= iRange; iDX++) {
		for (int iDY = -iRange; iDY <= iRange; iDY++) {
			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
			if (!pLoopPlot || plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) != iRange) {
				continue;
			}

#if defined(MOD_GLOBAL_SHORT_EMBARKED_BLOCKADES)
			if (pLoopPlot->isWater() && pLoopPlot->IsActualEnemyUnit(ePlayer, true, true)) {
#else
			if (pLoopPlot->isWater() && pLoopPlot->IsActualEnemyUnit(ePlayer)) {
#endif
				return true;
			}
		}
	}

	return false;
}
#endif

//	--------------------------------------------------------------------------------
bool CvPlot::isFlatlands() const
{
	return (getPlotType() == PLOT_LAND);
}

//	--------------------------------------------------------------------------------
bool CvPlot::IsRoughFeature() const
{
	return m_bRoughFeature;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetRoughFeature(bool bValue)
{
	if(IsRoughFeature() != bValue)
	{
		m_bRoughFeature = bValue;
	}
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

	if(getPlotType() != eNewValue)
	{
		if((getPlotType() == PLOT_OCEAN) || (eNewValue == PLOT_OCEAN))
		{
			erase(bEraseUnitsIfWater);
		}

		bWasWater = isWater();

		updateSeeFromSight(false);

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

		updateSeeFromSight(true);

		if((getTerrainType() == NO_TERRAIN) || (GC.getTerrainInfo(getTerrainType())->isWater() != isWater()))
		{
			if(isWater())
			{
				if(isAdjacentToLand())
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

		GC.getStepFinder().ForceReset();

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
							if(pLoopPlot->isAdjacentToLand())
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

#if defined(MOD_GLOBAL_CITY_WORKING)
			for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
#else
			for(iI = 0; iI < NUM_CITY_PLOTS; ++iI)
#endif
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

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

						if(pLoopPlot != NULL)
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

						if(pLoopPlot != NULL)
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
					setArea(FFreeList::INVALID_INDEX);

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
			updateSeeFromSight(false);
		}

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_TERRAIN, m_iX, m_iY, 0, eNewValue, m_eTerrainType, -1, -1);
		}
#endif

		m_eTerrainType = eNewValue;

		updateYield();
		updateImpassable();

		if(bUpdateSight)
		{
			updateSeeFromSight(true);
		}

		const bool bTypeIsWater = GC.getTerrainInfo(getTerrainType())->isWater();
		if(bTypeIsWater != isWater())
		{
			setPlotType((bTypeIsWater)? PLOT_OCEAN : PLOT_LAND, bRecalculate, bRebuildGraphics);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvPlot::setFeatureType(FeatureTypes eNewValue, int iVariety)
{
	FeatureTypes eOldFeature;
	bool bUpdateSight;

	eOldFeature = getFeatureType();

	iVariety = 0;

	if((eOldFeature != eNewValue) || (m_iFeatureVariety != iVariety))
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
			updateSeeFromSight(false);
		}

		auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(this));
		gDLL->GameplayFeatureChanged(pDllPlot.get(), eNewValue);

#if defined(MOD_EVENTS_TERRAFORMING)
		if (MOD_EVENTS_TERRAFORMING) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TerraformingPlot, TERRAFORMINGEVENT_FEATURE, m_iX, m_iY, 0, eNewValue, m_eFeatureType, -1, -1);
		}
#endif

		m_eFeatureType = eNewValue;

		updateYield();
		updateImpassable();

		if(bUpdateSight)
		{
			updateSeeFromSight(true);
		}
		m_iFeatureVariety = iVariety;

		// Rough feature?
		bool bRough = false;
		if(eNewValue != NO_FEATURE)
			bRough = GC.getFeatureInfo(eNewValue)->IsRough();

		SetRoughFeature(bRough);

		if(eNewValue != NO_FEATURE)
		{
			// Now a Natural Wonder here
			if((eOldFeature == NO_FEATURE || !GC.getFeatureInfo(eOldFeature)->IsNaturalWonder()) && GC.getFeatureInfo(eNewValue)->IsNaturalWonder())
			{
				GC.getMap().ChangeNumNaturalWonders(1);
				GC.getMap().getArea(getArea())->ChangeNumNaturalWonders(1);
			}
		}
		if(eOldFeature != NO_FEATURE)
		{
			// Was a Natural Wonder, isn't any more
			if(GC.getFeatureInfo(eOldFeature)->IsNaturalWonder() && (eNewValue == NO_FEATURE || !GC.getFeatureInfo(eNewValue)->IsNaturalWonder()))
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
bool CvPlot::IsNaturalWonder() const
{
	FeatureTypes eFeature = getFeatureType();

	if(eFeature == NO_FEATURE)
		return false;

	return GC.getFeatureInfo(eFeature)->IsNaturalWonder();
}

//	--------------------------------------------------------------------------------
ResourceTypes CvPlot::getResourceType(TeamTypes eTeam) const
{
	if(eTeam != NO_TEAM)
	{
		if(m_eResourceType != NO_RESOURCE)
		{
			CvGame& Game = GC.getGame();
			bool bDebug = Game.isDebugMode();

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
	if(m_eResourceType != eNewValue)
	{
		if (eNewValue != -1)
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
					CvCity* pCity = getWorkingCity();
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
ImprovementTypes CvPlot::getImprovementTypeNeededToImproveResource(PlayerTypes ePlayer, bool bTestPlotOwner)
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

	ImprovementTypes eImprovementNeeded = NO_IMPROVEMENT;

	// see if we can improve the resource
	for(int iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		BuildTypes eBuild = (BuildTypes) iBuildIndex;
		CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);
		if(pBuildInfo == NULL)
			continue;

		if(!canBuild(eBuild, ePlayer, false /*bTestVisible*/, bTestPlotOwner))
			continue;

		if(ePlayer != NO_PLAYER)
		{
			if(!GET_PLAYER(ePlayer).canBuild(this, eBuild, false /*bTestEra*/, false /*bTestVisible*/, false /*bTestGold*/, bTestPlotOwner))
			{
				continue;
			}
		}

		ImprovementTypes eImprovement = (ImprovementTypes) pBuildInfo->getImprovement();
		if(eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(eImprovement);
		if(pImprovementInfo == NULL)
			continue;

		if(!pImprovementInfo->IsImprovementResourceTrade(eResource))
			continue;

		if(pImprovementInfo->IsCreatedByGreatPerson())
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
	if (eBuilder != NO_PLAYER)
	{
		if (getOwner() != eBuilder && !GET_PLAYER(eBuilder).isMinorCiv())
		{
			bGiftFromMajor = true;
		}
	}
	bool bIgnoreResourceTechPrereq = bGiftFromMajor; // If it is a gift from a major civ, our tech limitations do not apply

	if(eOldImprovement != eNewValue)
	{
		PlayerTypes owningPlayerID = getOwner();
		if(eOldImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry& oldImprovementEntry = *GC.getImprovementInfo(eOldImprovement);

			// If this improvement can add culture to nearby improvements, update them as well
#if defined(MOD_API_UNIFIED_YIELDS)
			for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				if(oldImprovementEntry.GetYieldAdjacentSameType((YieldTypes) iI) > 0)
#else
				if(oldImprovementEntry.GetCultureAdjacentSameType() > 0)
#endif
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eOldImprovement)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
#if defined(MOD_API_UNIFIED_YIELDS)
				if(oldImprovementEntry.GetYieldAdjacentTwoSameType((YieldTypes) iI) > 0)
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eOldImprovement)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
#endif

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
					}
				}
#if defined(MOD_BALANCE_CORE)
				//Resource from improvement - change ownership if needed.
				ResourceTypes eResourceFromImprovement = (ResourceTypes)oldImprovementEntry.GetResourceFromImprovement();

				if(eResourceFromImprovement != NO_RESOURCE)
				{
					GET_PLAYER(eOldBuilder).changeNumResourceTotal(eResourceFromImprovement, -1, true);
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

		if(getImprovementType() == NO_IMPROVEMENT)
		{
			setImprovementDuration(0);
		}

		// Reset who cleared a Barb camp here last (if we're putting a new one down)
		if(eNewValue == GC.getBARBARIAN_CAMP_IMPROVEMENT())
		{
			SetPlayerThatClearedBarbCampHere(NO_PLAYER);
			
#if defined(MOD_BUGFIX_BARB_CAMP_SPAWNING)
			// Alert the barbarian spawning code to this new camp
			CvBarbarians::DoCampActivationNotice(this);
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
			if(GET_TEAM((TeamTypes)iI).isAlive())
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

			// If this improvement can add culture to nearby improvements, update them as well
#if defined(MOD_API_UNIFIED_YIELDS)
			for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				if(newImprovementEntry.GetYieldAdjacentSameType((YieldTypes) iYield) > 0)
#else
				if(newImprovementEntry.GetCultureAdjacentSameType() > 0)
#endif
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eNewValue)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
#if defined(MOD_API_UNIFIED_YIELDS)
				if(newImprovementEntry.GetYieldAdjacentTwoSameType((YieldTypes) iYield) > 0)
				{
					for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
						if(pAdjacentPlot && pAdjacentPlot->getImprovementType() == eNewValue)
						{
							pAdjacentPlot->updateYield();
						}
					}
				}
			}
#endif

			if(area())
			{
				area()->changeNumImprovements(eNewValue, 1);
			}
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
					}
				}
#if defined(MOD_BALANCE_CORE)
				//Resource from improvement - change ownership if needed.
				ResourceTypes eResourceFromImprovement = (ResourceTypes)newImprovementEntry.GetResourceFromImprovement();

				if(eResourceFromImprovement != NO_RESOURCE)
				{
					owningPlayer.changeNumResourceTotal(eResourceFromImprovement, 1, true);
				}
#endif

				// Add Resource Quantity to total
				if(getResourceType() != NO_RESOURCE)
				{
					if(bIgnoreResourceTechPrereq || GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
					{
						if(newImprovementEntry.IsImprovementResourceTrade(getResourceType()))
						{
							owningPlayer.changeNumResourceTotal(getResourceType(), getNumResourceForPlayer(owningPlayerID));

							// Activate Resource city link?
							if(GetResourceLinkedCity() != NULL && !IsResourceLinkedCityActive())
								SetResourceLinkedCityActive(true);
						}
					}
				}

				ResourceTypes eResource = getResourceType(getTeam());
				if(bIgnoreResourceTechPrereq)
					eResource = getResourceType();

				if(eResource != NO_RESOURCE)
				{
					if(newImprovementEntry.IsImprovementResourceTrade(eResource))
					{
						if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
						{
#if defined(MOD_BALANCE_CORE_HAPPINESS)
							if(MOD_BALANCE_CORE_HAPPINESS)
							{
								if(owningPlayer.isHuman())
								{
									owningPlayer.CalculateHappiness();
								}
							}
							else
							{
#endif
							owningPlayer.DoUpdateHappiness();
#if defined(MOD_BALANCE_CORE_HAPPINESS)
							}
#endif
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
							SetImprovementEmbassy(true);
						}
					}
				}
#endif
			}
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
						if(GC.getImprovementInfo(eOldImprovement)->IsImprovementResourceTrade(getResourceType()))
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
					if(GC.getImprovementInfo(eOldImprovement)->IsImprovementResourceTrade(eResource))
					{
						if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
						{
#if defined(MOD_BALANCE_CORE_HAPPINESS)
							if(MOD_BALANCE_CORE_HAPPINESS)
							{
								if(owningPlayer.isHuman())
								{
									owningPlayer.CalculateHappiness();
								}
							}
							else
							{
#endif
							owningPlayer.DoUpdateHappiness();
#if defined(MOD_BALANCE_CORE_HAPPINESS)
							}	
#endif
						}
					}
				}
			}
		}

		updateYield();

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
		if (MOD_EVENTS_TILE_IMPROVEMENTS) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileImprovementChanged, getX(), getY(), getOwner(), eOldImprovement, eNewValue, IsImprovementPillaged());
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
	bool bWasEmbassy = m_bImprovementEmbassy;

	if(bEmbassy != bWasEmbassy)
	{
		m_bImprovementEmbassy = bEmbassy;
	}

	if(bWasEmbassy != m_bImprovementEmbassy)
	{
		setLayoutDirty(true);
	}
}
#endif

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
		updateYield();

		// Quantified Resource changes
		if(getResourceType() != NO_RESOURCE && getImprovementType() != NO_IMPROVEMENT)
		{
			if(getTeam() != NO_TEAM)
			{
				if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(getResourceType())->getTechCityTrade()))
				{
					if(GC.getImprovementInfo(getImprovementType())->IsImprovementResourceTrade(getResourceType()))
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

			if(bPillaged && (eResourceFromImprovement != NO_RESOURCE))
			{
				GET_PLAYER(getOwner()).changeNumResourceTotal(eResourceFromImprovement, -1, true);
			}
			else if(!bPillaged && (eResourceFromImprovement != NO_RESOURCE))
			{
				GET_PLAYER(getOwner()).changeNumResourceTotal(eResourceFromImprovement, 1, true);
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
	bool bOldRoute;
	RouteTypes eOldRoute = getRouteType();
	int iI;

	if(eOldRoute != eNewValue || (eOldRoute == eNewValue && IsRoutePillaged()))
	{
		bOldRoute = isRoute(); // XXX is this right???

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
#if defined(MOD_EVENTS_TILE_IMPROVEMENTS)
		SetRoutePillaged(false, false);
#else
		SetRoutePillaged(false);
#endif

		for(iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if(GET_TEAM((TeamTypes)iI).isAlive())
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
			if(GET_TEAM((TeamTypes)iI).isAlive() && GC.getGame().getActiveTeam() == (TeamTypes)iI)
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

	m_bRoutePillaged = bPillaged;

	if(bPillaged && IsTradeRoute(NO_PLAYER))
	{
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			PlayerTypes ePlayer = (PlayerTypes)i;
			if(GET_PLAYER(ePlayer).isAlive())
			{
				if(IsTradeRoute(ePlayer))
				{
					GET_PLAYER(ePlayer).GetCityConnections()->Update();
				}
			}
		}
	}

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
	return getCity(m_ResourceLinkedCity);
}

//	--------------------------------------------------------------------------------
/// Link the resource on this plot to pCity. Note that this does NOT set the link to be active - this must be done manually
void CvPlot::SetResourceLinkedCity(const CvCity* pCity)
{
	if(GetResourceLinkedCity() != pCity)
	{
		if(pCity != NULL)
		{
			CvAssertMsg(pCity->getOwner() == getOwner(), "Argument city pNewValue's owner is expected to be the same as the current instance");
			m_ResourceLinkedCity = pCity->GetIDInfo();
		}
		else
		{
			// Set to inactive BEFORE unassigning linked City
			SetResourceLinkedCityActive(false);

			m_ResourceLinkedCity.reset();
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
#if defined(MOD_GLOBAL_CITY_WORKING)
	for(int iJ = 0; iJ < MAX_CITY_PLOTS; iJ++)
#else
	for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
#endif
	{
		// We're not actually looking around a City but Resources have to be within the RANGE of a City, so we can still use this
		pLoopPlot = plotCity(getX(), getY(), iJ);

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

		// Already have a valid improvement here?
		if(isCity() || getImprovementType() != NO_IMPROVEMENT)
		{
			if(isCity() || GC.getImprovementInfo(getImprovementType())->IsImprovementResourceTrade(getResourceType()))
			{
				SetResourceLinkedCityActive(true);
			}
		}
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

#if defined(MOD_GLOBAL_CITY_WORKING)
			for(iI = 0; iI < getPlotCity()->GetNumWorkablePlots(); ++iI)
#else
			for(iI = 0; iI < NUM_CITY_PLOTS; ++iI)
#endif
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

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
#if defined(MOD_GLOBAL_CITY_WORKING)
			for(iI = 0; iI < getPlotCity()->GetNumWorkablePlots(); ++iI)
#else
			for(iI = 0; iI < NUM_CITY_PLOTS; ++iI)
#endif
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

				if(pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwner(), 1);
				}
			}

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
CvCity* CvPlot::getWorkingCity() const
{
	return getCity(m_workingCity);
}


//	--------------------------------------------------------------------------------
void CvPlot::updateWorkingCity()
{
	CvCity* pOldWorkingCity;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pLoopPlot;
	int iBestPlot;
	int iI;

	pBestCity = getPlotCity();

	if(pBestCity == NULL)
	{
		pBestCity = getWorkingCityOverride();
		CvAssertMsg((pBestCity == NULL) || (pBestCity->getOwner() == getOwner()), "pBest city is expected to either be NULL or the current plot instance's");
	}

	if((pBestCity == NULL) && isOwned())
	{
		iBestPlot = 0;

#if defined(MOD_GLOBAL_CITY_WORKING)
		for(iI = 0; iI < MAX_CITY_PLOTS; ++iI)
#else
		for(iI = 0; iI < NUM_CITY_PLOTS; ++iI)
#endif
		{
			pLoopPlot = plotCity(getX(), getY(), iI);

			if(pLoopPlot != NULL)
			{
				pLoopCity = pLoopPlot->getPlotCity();

				if(pLoopCity != NULL)
				{
					if(pLoopCity->getOwner() == getOwner())
					{
						// XXX use getGameTurnAcquired() instead???
						if((pBestCity == NULL) ||
						        (GC.getCityPlotPriority()[iI] < GC.getCityPlotPriority()[iBestPlot]) ||
						        ((GC.getCityPlotPriority()[iI] == GC.getCityPlotPriority()[iBestPlot]) &&
						         ((pLoopCity->getGameTurnFounded() < pBestCity->getGameTurnFounded()) ||
						          ((pLoopCity->getGameTurnFounded() == pBestCity->getGameTurnFounded()) &&
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

	pOldWorkingCity = getWorkingCity();

	if(pOldWorkingCity != pBestCity)
	{
		// Change what City's allowed to work this Plot
		if(pBestCity != NULL)
		{
			// Remove Citizen from this plot if another City's using it
			if(pOldWorkingCity != NULL)
			{
				// Remove citizen
				pOldWorkingCity->GetCityCitizens()->SetWorkingPlot(this, false);
			}

			CvAssertMsg(isOwned(), "isOwned is expected to be true");
			CvAssertMsg(!isBeingWorked(), "isBeingWorked did not return false as expected");
			m_workingCity = pBestCity->GetIDInfo();

			// If we told a City to stop working this plot, tell it to do something else instead
			if(pOldWorkingCity != NULL)
			{
				// Re-add citizen somewhere else
				pOldWorkingCity->GetCityCitizens()->DoAddBestCitizenFromUnassigned();
			}
		}
		else
		{
			m_workingCity.reset();
		}

		updateYield();

		GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
CvCity* CvPlot::getWorkingCityOverride() const
{
	return getCity(m_workingCityOverride);
}


//	--------------------------------------------------------------------------------
void CvPlot::setWorkingCityOverride(const CvCity* pNewValue)
{
	if(getWorkingCityOverride() != pNewValue)
	{
		if(pNewValue != NULL)
		{
			CvAssertMsg(pNewValue->getOwner() == getOwner(), "Argument city pNewValue's owner is expected to be the same as the current instance");
			m_workingCityOverride = pNewValue->GetIDInfo();
		}
		else
		{
			m_workingCityOverride.reset();
		}

		updateWorkingCity();
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
short* CvPlot::getYield()
{
	return m_aiYield;
}


//	--------------------------------------------------------------------------------
int CvPlot::getYield(YieldTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (int)(m_aiYield[eIndex]);
}


//	--------------------------------------------------------------------------------
int CvPlot::calculateNatureYield(YieldTypes eYield, TeamTypes eTeam, bool bIgnoreFeature) const
{
	ResourceTypes eResource;
	int iYield;
	ReligionTypes eMajority = NO_RELIGION;
	BeliefTypes eSecondaryPantheon = NO_BELIEF;
#if defined(MOD_BALANCE_CORE)
	if((YieldTypes)eYield > YIELD_FAITH)
	{
		return 0;
	}
#endif

#if !defined(MOD_RELIGION_PLOT_YIELDS) && !defined(MOD_API_PLOT_YIELDS)
	if(isImpassable() || isMountain())
	{
		// No Feature, or the Feature isn't a Natural Wonder (which are impassable but allowed to be worked)
		if(getFeatureType() == NO_FEATURE || !GC.getFeatureInfo(getFeatureType())->IsNaturalWonder())
		{
			return 0;
		}
	}
#endif

	const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);

	CvCity* pWorkingCity = getWorkingCity();
	if(pWorkingCity)
	{
		eMajority = pWorkingCity->GetCityReligions()->GetReligiousMajority();
		eSecondaryPantheon = pWorkingCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
	}

	CvAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

#if defined(MOD_RELIGION_PLOT_YIELDS) || defined(MOD_API_PLOT_YIELDS)
	// Impassable terrain and mountains have no base yield
	if(isImpassable() || isMountain()) {
		iYield = 0;
	} else
#endif
		iYield = GC.getTerrainInfo(getTerrainType())->getYield(eYield);

#if defined(MOD_API_PLOT_YIELDS)
	if (MOD_API_PLOT_YIELDS) {
		iYield += GC.getPlotInfo(getPlotType())->getYield(eYield);
	}
#endif

	// Extra yield for religion on this terrain
	if(pWorkingCity != NULL && eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pWorkingCity->getOwner());
		if(pReligion)
		{
#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
			//Change for improvement/resource
			int iReligionChange = 0;
			bool bRequiresImprovement = pReligion->m_Beliefs.RequiresImprovement();
			bool bRequiresResource = pReligion->m_Beliefs.RequiresResource();
			if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && (bRequiresImprovement || bRequiresResource))
			{	
				if(bRequiresImprovement && bRequiresResource && (getResourceType() != NO_RESOURCE) && (getImprovementType() != NO_IMPROVEMENT))
				{
					int iMod = pReligion->m_Beliefs.GetTerrainYieldChange(getTerrainType(), eYield);
					iReligionChange += iMod;
				}
				else if(bRequiresImprovement && !bRequiresResource && (getImprovementType() != NO_IMPROVEMENT))
				{
					int iMod = pReligion->m_Beliefs.GetTerrainYieldChange(getTerrainType(), eYield);
					iReligionChange += iMod;
				}
				else if(bRequiresResource && !bRequiresImprovement && (getResourceType() != NO_RESOURCE))
				{
					int iMod = pReligion->m_Beliefs.GetTerrainYieldChange(getTerrainType(), eYield);
					iReligionChange += iMod;
				}
			}
			else
			{
				iReligionChange = pReligion->m_Beliefs.GetTerrainYieldChange(getTerrainType(), eYield);
			}
#else
			int iReligionChange = pReligion->m_Beliefs.GetTerrainYieldChange(getTerrainType(), eYield);
#endif
			if (eSecondaryPantheon != NO_BELIEF)
			{
#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
				//Change for improvement/resource
				int iReligionChange = 0;
				bool bRequiresImprovement = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->RequiresImprovement();
				bool bRequiresResource = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->RequiresResource();
				if(MOD_BALANCE_CORE_BELIEFS_RESOURCE && (bRequiresImprovement || bRequiresResource))
				{		
					if(bRequiresImprovement && bRequiresResource && (getResourceType() != NO_RESOURCE) && (getImprovementType() != NO_IMPROVEMENT))
					{
						int iMod = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTerrainYieldChange(getTerrainType(), eYield);
						iReligionChange += iMod;
					}
					else if(bRequiresImprovement && !bRequiresResource && (getImprovementType() != NO_IMPROVEMENT))
					{
						int iMod = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTerrainYieldChange(getTerrainType(), eYield);
						iReligionChange += iMod;
					}
					else if(bRequiresResource && !bRequiresImprovement && (getResourceType() != NO_RESOURCE))
					{
						int iMod = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTerrainYieldChange(getTerrainType(), eYield);
						iReligionChange += iMod;
					}
				}
				else
				{
					iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTerrainYieldChange(getTerrainType(), eYield);
				}
#else
				iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetTerrainYieldChange(getTerrainType(), eYield);
#endif
			}
			
#if defined(MOD_RELIGION_PLOT_YIELDS)
			if (MOD_RELIGION_PLOT_YIELDS) {
				iReligionChange += pReligion->m_Beliefs.GetPlotYieldChange(getPlotType(), eYield);
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetPlotYieldChange(getPlotType(), eYield);
				}
			}
#endif

			iYield += iReligionChange;
		}
	}

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

			// Player Trait
			if(m_eOwner != NO_PLAYER && getImprovementType() == NO_IMPROVEMENT)
			{
				iYieldChange +=  GET_PLAYER((PlayerTypes)m_eOwner).GetPlayerTraits()->GetUnimprovedFeatureYieldChange(getFeatureType(), eYield);
#if defined(MOD_API_UNIFIED_YIELDS)
				iYieldChange +=  GET_PLAYER((PlayerTypes)m_eOwner).getUnimprovedFeatureYieldChange(getFeatureType(), eYield);

				if(pWorkingCity != NULL && eMajority != NO_RELIGION)
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pWorkingCity->getOwner());
					if(pReligion)
					{
						int iReligionChange = pReligion->m_Beliefs.GetUnimprovedFeatureYieldChange(getFeatureType(), eYield);
						if (eSecondaryPantheon != NO_BELIEF)
						{
							iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetUnimprovedFeatureYieldChange(getFeatureType(), eYield);
						}
						iYieldChange += iReligionChange;
					}
				}
#endif
			}

			// Leagues
			if(pWorkingCity != NULL)
			{
				iYieldChange += GC.getGame().GetGameLeagues()->GetFeatureYieldChange(pWorkingCity->getOwner(), getFeatureType(), eYield);
			}

			// Religion
			if(pWorkingCity != NULL && eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pWorkingCity->getOwner());
				if(pReligion)
				{
					int iReligionChange = pReligion->m_Beliefs.GetFeatureYieldChange(getFeatureType(), eYield);
					if (eSecondaryPantheon != NO_BELIEF)
					{
						iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFeatureYieldChange(getFeatureType(), eYield);
					}
					iYieldChange += iReligionChange;
				}
			}

#if defined(MOD_API_UNIFIED_YIELDS)
			if(m_eOwner != NO_PLAYER)
			{
				iYieldChange += GET_PLAYER((PlayerTypes)m_eOwner).getFeatureYieldChange(getFeatureType(), eYield);
				iYieldChange += GET_PLAYER((PlayerTypes)m_eOwner).GetPlayerTraits()->GetFeatureYieldChange(getFeatureType(), eYield);
			}
#endif

			// Natural Wonders
			if(m_eOwner != NO_PLAYER && pFeatureInfo->IsNaturalWonder())
			{
				int iMod = 0;

				// Boost from religion in nearby city?
				if(pWorkingCity && eMajority != NO_RELIGION)
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pWorkingCity->getOwner());
					if(pReligion)
					{
						int iReligionChange = pReligion->m_Beliefs.GetYieldChangeNaturalWonder(eYield);
						if (eSecondaryPantheon != NO_BELIEF)
						{
							iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldChangeNaturalWonder(eYield);
						}
						iYieldChange += iReligionChange;

						int iReligionMod = pReligion->m_Beliefs.GetYieldModifierNaturalWonder(eYield);
						if (eSecondaryPantheon != NO_BELIEF)
						{
							iReligionMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldModifierNaturalWonder(eYield);
						}
						iMod += iReligionMod;
					}
				}

				iYieldChange += GET_PLAYER((PlayerTypes)m_eOwner).GetPlayerTraits()->GetYieldChangeNaturalWonder(eYield);
#if defined(MOD_API_UNIFIED_YIELDS)
				iYieldChange += GET_PLAYER((PlayerTypes)m_eOwner).GetYieldChangesNaturalWonder(eYield);
#endif

				iMod += GET_PLAYER((PlayerTypes)m_eOwner).GetPlayerTraits()->GetNaturalWonderYieldModifier();
				if(iMod > 0)
				{
					iYieldChange *= (100 + iMod);
					iYieldChange /= 100;
				}
			}

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
		eResource = getResourceType(eTeam);

		if(eResource != NO_RESOURCE)
		{
			iYield += GC.getResourceInfo(eResource)->getYieldChange(eYield);

			// Extra yield for religion
			if(pWorkingCity != NULL && eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pWorkingCity->getOwner());
				if(pReligion)
				{
					int iReligionChange = pReligion->m_Beliefs.GetResourceYieldChange(eResource, eYield);
					if (eSecondaryPantheon != NO_BELIEF)
					{
						iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetResourceYieldChange(eResource, eYield);
					}
					iYield += iReligionChange;
				}
			}

#if defined(MOD_API_UNIFIED_YIELDS)
			if(m_eOwner != NO_PLAYER)
			{
				iYield += GET_PLAYER((PlayerTypes)m_eOwner).getResourceYieldChange(eResource, eYield);
				iYield += GET_PLAYER((PlayerTypes)m_eOwner).GetPlayerTraits()->GetResourceYieldChange(eResource, eYield);
			}
#endif
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

#if defined(MOD_API_UNIFIED_YIELDS)
	if(isFreshWater())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType())->getFreshWaterYieldChange(eYield) : GC.getFeatureInfo(getFeatureType())->getFreshWaterYieldChange(eYield));
	}

	if(isCoastalLand())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType())->getCoastalLandYieldChange(eYield) : GC.getFeatureInfo(getFeatureType())->getCoastalLandYieldChange(eYield));
	}

	if(eTeam != NO_TEAM)
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GET_TEAM(eTeam).getTerrainYieldChange(getTerrainType(), eYield) : GET_TEAM(eTeam).getFeatureYieldChange(getFeatureType(), eYield));
	}
#endif

	return std::max(0, iYield);
}


//	--------------------------------------------------------------------------------
int CvPlot::calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const
{
	return std::max(calculateNatureYield(eIndex, eTeam, false), calculateNatureYield(eIndex, eTeam, true));
}


//	--------------------------------------------------------------------------------
int CvPlot::calculateTotalBestNatureYield(TeamTypes eTeam) const
{
	return (calculateBestNatureYield(YIELD_FOOD, eTeam) + calculateBestNatureYield(YIELD_PRODUCTION, eTeam) + calculateBestNatureYield(YIELD_GOLD, eTeam));
}


//	--------------------------------------------------------------------------------
int CvPlot::calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal, RouteTypes eAssumeThisRoute) const
{
	ResourceTypes eResource;
	int iBestYield;
	int iYield;
	int iI;

	CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
	if (!pImprovement)
		return 0;

#if defined(MOD_BALANCE_CORE)
	if((YieldTypes)eYield > YIELD_FAITH)
	{
		return 0;
	}
#endif

	iYield = pImprovement->GetYieldChange(eYield);

	int iYieldChangePerEra = pImprovement->GetYieldChangePerEra(eYield);
	if (ePlayer != NO_PLAYER && iYieldChangePerEra > 0)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		int iPlotEra = GetArchaeologicalRecord().m_eEra;
		int iNumEras = kPlayer.GetCurrentEra() - iPlotEra;

		if (iPlotEra != NO_ERA && iNumEras > 0)
		{
			iYield += (iNumEras * iYieldChangePerEra);
		}
	}

	if(isRiverSide())
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
				else if(pAdjacentPlot->isMountain())
				{
					iYield += pImprovement->GetAdjacentMountainYieldChange(eYield);
				}
			}
		}
	}

#if defined(MOD_API_UNIFIED_YIELDS)
	if(isFreshWater() || bOptimal)
#else
	if(bOptimal)
#endif
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

		if(eRouteType != NO_ROUTE)
		{
#if defined(MOD_BALANCE_CORE_YIELDS)
			CvGameTrade* pTrade = GC.getGame().GetGameTrade();
			bool bTrade = false;
			for (uint uiConnection = 0; uiConnection < pTrade->m_aTradeConnections.size(); uiConnection++)
			{
				TradeConnection* pConnection = &(pTrade->m_aTradeConnections[uiConnection]);
				if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
				{
					continue;
				}
				for (uint ui = 0; ui < pConnection->m_aPlotList.size(); ui++)
				{
					if (pConnection->m_aPlotList[ui].m_iX == getX() && pConnection->m_aPlotList[ui].m_iY == getY())
					{
						bTrade = true;
						break;
					}
				}
			}
			if(IsTradeRoute(ePlayer))
			{
				if(IsRouteRailroad())
				{
					iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
				}
				else if(IsRouteRoad())
				{
					iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
				}
			}
			if(bTrade)
			{
				if(IsRouteRailroad())
				{
					iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
				}
				else if(IsRouteRoad())
				{
					iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
				}
#else
			iYield += pImprovement->GetRouteYieldChanges(eRouteType, eYield);
#endif
			}
		}
	}

	bool bIsFreshWater = isFreshWater();

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
	else
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

		iYield += kPlayer.getImprovementYieldChange(eImprovement, eYield);
		iYield += kPlayer.GetPlayerTraits()->GetImprovementYieldChange(eImprovement, eYield);
		iYield += kTeam.getImprovementYieldChange(eImprovement, eYield);

		if(bIsFreshWater)
		{
			iYield += kTeam.getImprovementFreshWaterYieldChange(eImprovement, eYield);
		}
		else
		{
			iYield += kTeam.getImprovementNoFreshWaterYieldChange(eImprovement, eYield);
		}
	}

	if(ePlayer != NO_PLAYER)
	{
		eResource = getResourceType(GET_PLAYER(ePlayer).getTeam());

		if(eResource != NO_RESOURCE)
		{
			iYield += pImprovement->GetImprovementResourceYield(eResource, eYield);
		}
	}

	// Working city
	CvCity* pWorkingCity = getWorkingCity();
	if(pWorkingCity)
	{
		ReligionTypes eMajority = pWorkingCity->GetCityReligions()->GetReligiousMajority();
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pWorkingCity->getOwner());
			if(pReligion)
			{
#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
				int iReligionChange = 0;
				bool bRequiresResource = pReligion->m_Beliefs.RequiresResource();
				if(pImprovement->IsCreatedByGreatPerson())
				{
					bRequiresResource = false;
				}
				if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresResource)
				{	
					if(bRequiresResource && (getResourceType(GET_PLAYER(pWorkingCity->getOwner()).getTeam()) != NO_RESOURCE))
					{		
						iReligionChange = pReligion->m_Beliefs.GetImprovementYieldChange(eImprovement, eYield);
					}
				}
				else
				{
					iReligionChange = pReligion->m_Beliefs.GetImprovementYieldChange(eImprovement, eYield);
				}
#else
				int iReligionChange = pReligion->m_Beliefs.GetImprovementYieldChange(eImprovement, eYield);
#endif			
				BeliefTypes eSecondaryPantheon = pWorkingCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
				if (eSecondaryPantheon != NO_BELIEF)
				{
#if defined(MOD_BALANCE_CORE_BELIEFS_RESOURCE)
					bRequiresResource = GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->RequiresResource();
					if(pImprovement->IsCreatedByGreatPerson())
					{
						bRequiresResource = false;
					}
					if (MOD_BALANCE_CORE_BELIEFS_RESOURCE && bRequiresResource)
					{	
						if(bRequiresResource && (getResourceType(GET_PLAYER(pWorkingCity->getOwner()).getTeam()) != NO_RESOURCE))
						{		
							iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetImprovementYieldChange(eImprovement, eYield);
						}
					}
					else
					{
						iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetImprovementYieldChange(eImprovement, eYield);
					}
#else
					iReligionChange += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetImprovementYieldChange(eImprovement, eYield);
#endif					
				}
				iYield += iReligionChange;
			}
		}
	}

	return iYield;
}


//	--------------------------------------------------------------------------------
int CvPlot::calculateYield(YieldTypes eYield, bool bDisplay)
{
	CvCity* pCity = 0;
	CvCity* pWorkingCity = 0;
	ImprovementTypes eImprovement = NO_IMPROVEMENT;
	RouteTypes eRoute = NO_ROUTE;
	PlayerTypes ePlayer = NO_PLAYER;
	bool bCity = false;
	int iYield = 0;

	const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);

	if(bDisplay && GC.getGame().isDebugMode())
	{
		return getYield(eYield);
	}

	if(getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}

	if(!isPotentialCityWork())
	{
		return 0;
	}

#if defined(MOD_BALANCE_CORE)
	if((YieldTypes)eYield > YIELD_FAITH)
	{
		return 0;
	}
#endif
	bCity = false;

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
		ePlayer = getOwner();
		eImprovement = getImprovementType();
		eRoute = getRouteType();
	}

	iYield = calculateNatureYield(eYield, ((ePlayer != NO_PLAYER) ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM));

#if defined(MOD_API_UNIFIED_YIELDS)
	if (ePlayer != NO_PLAYER) {
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
#if defined(MOD_API_PLOT_YIELDS)
		iYield += kPlayer.GetPlayerTraits()->GetPlotYieldChange(getPlotType(), eYield);
		iYield += kPlayer.getPlotYieldChange(getPlotType(), eYield);
#endif
		iYield += kPlayer.GetPlayerTraits()->GetTerrainYieldChange(getTerrainType(), eYield);
		iYield += kPlayer.getTerrainYieldChange(getTerrainType(), eYield);
	}
#endif

	if(eImprovement != NO_IMPROVEMENT && !IsImprovementPillaged())
	{
		int iCultureBoost = calculateImprovementYieldChange(eImprovement, eYield, ePlayer);
		iYield += iCultureBoost;

#if !defined(MOD_API_UNIFIED_YIELDS)
		if(eYield == YIELD_CULTURE)
#endif
		{
			CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
#if defined(MOD_API_UNIFIED_YIELDS)
			if(pImprovement && pImprovement->GetYieldChange(eYield) > 0)
#else
			if(pImprovement && pImprovement->GetYieldChange(YIELD_CULTURE) > 0)
#endif
			{
#if defined(MOD_API_UNIFIED_YIELDS)
				int iAdjacentCulture = pImprovement->GetYieldAdjacentSameType(eYield);
				iAdjacentCulture += pImprovement->GetYieldAdjacentTwoSameType(eYield);
#else
				int iAdjacentCulture = pImprovement->GetCultureAdjacentSameType();
#endif
				if(iAdjacentCulture > 0)
				{
#if defined(MOD_API_UNIFIED_YIELDS)
					iYield += ComputeYieldFromAdjacentImprovement(*pImprovement, eImprovement, eYield);
					iYield += ComputeYieldFromTwoAdjacentImprovement(*pImprovement, eImprovement, eYield);
#else
					iYield += ComputeCultureFromAdjacentImprovement(*pImprovement, eImprovement);
#endif
				}
			}
#if defined(MOD_API_UNIFIED_YIELDS)
			if(eYield == YIELD_CULTURE && getOwner() != NO_PLAYER)
#else
			if(getOwner() != NO_PLAYER)
#endif
			{
				iYield += GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_EXTRA_CULTURE_FROM_IMPROVEMENTS);
				iYield += GET_PLAYER(getOwner()).GetPlayerPolicies()->GetImprovementCultureChange(eImprovement);
			}
		}
	}

	if(eRoute != NO_ROUTE && !IsRoutePillaged())
	{
		CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
		if(pkRouteInfo)
		{
			iYield += pkRouteInfo->getYieldChange(eYield);
		}
	}

	if(ePlayer != NO_PLAYER)
	{
		pCity = getPlotCity();

		if(pCity != NULL)
		{
			if(!bDisplay || pCity->isRevealed(GC.getGame().getActiveTeam(), false))
			{
				iYield += kYield.getCityChange();

				if(kYield.getPopulationChangeDivisor() != 0)
				{
					iYield += (pCity->getPopulation() + kYield.getPopulationChangeOffset()) / kYield.getPopulationChangeDivisor();
				}
				bCity = true;
			}
		}

		pWorkingCity = getWorkingCity();

		if(isWater())
		{
			if(!isImpassable() && !isMountain())
			{
				iYield += GET_PLAYER(ePlayer).getSeaPlotYield(eYield);

				if(pWorkingCity != NULL)
				{
					if(!bDisplay || pWorkingCity->isRevealed(GC.getGame().getActiveTeam(), false))
					{
						int iCityYield = 0;
						if (isLake())
						{
							if (pWorkingCity->getLakePlotYield(eYield) > 0)
							{
								iCityYield = pWorkingCity->getLakePlotYield(eYield);
							}
						}
						else
						{
							iCityYield = pWorkingCity->getSeaPlotYield(eYield);
						}
						iYield += iCityYield;
					}
				}

				if(getResourceType(GET_PLAYER(ePlayer).getTeam()) != NO_RESOURCE)
				{
					if(pWorkingCity != NULL)
					{
						if(!bDisplay || pWorkingCity->isRevealed(GC.getGame().getActiveTeam(), false))
						{
							iYield += pWorkingCity->getSeaResourceYield(eYield);
						}
					}
				}

			}
		}

		if(isRiver())
		{
			if(!isImpassable() && !isMountain())
			{
				if(NULL != pWorkingCity)
				{
					if(!bDisplay || pWorkingCity->isRevealed(GC.getGame().getActiveTeam(), false))
					{
						iYield += pWorkingCity->getRiverPlotYield(eYield);
					}
				}
			}
		}

		// Extra yield for features
		if(getFeatureType() != NO_FEATURE)
		{
			if(pWorkingCity != NULL)
			{
				iYield += pWorkingCity->GetFeatureExtraYield(getFeatureType(), eYield);
			}
		}

#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
		// Extra yield for plot
		if(pWorkingCity != NULL)
		{
			iYield += pWorkingCity->GetPlotExtraYield(getPlotType(), eYield);
		}
#endif

		// Extra yield for terrain
		if(getTerrainType() != NO_TERRAIN)
		{
			if(pWorkingCity != NULL && !isImpassable() && !isMountain())
			{
				iYield += pWorkingCity->GetTerrainExtraYield(getTerrainType(), eYield);
			}
		}

		ResourceTypes eResource = getResourceType(GET_PLAYER(ePlayer).getTeam());
		if(eResource != NO_RESOURCE)
		{
			TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && eTeam != NO_TEAM && GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes) pkResourceInfo->getTechReveal()))
			{
				if (pkResourceInfo->getPolicyReveal() == NO_POLICY || GET_PLAYER(ePlayer).GetPlayerPolicies()->HasPolicy((PolicyTypes)pkResourceInfo->getPolicyReveal()))
				{
					// Extra yield from resources
					if(pWorkingCity != NULL)
						iYield += pWorkingCity->GetResourceExtraYield(eResource, eYield);

					// Extra yield from Trait
					if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
					{
						iYield += GET_PLAYER(ePlayer).GetPlayerTraits()->GetYieldChangeStrategicResources(eYield);
					}
				}
			}
		}
	}

	if(bCity)
	{
		iYield = std::max(iYield, kYield.getMinCity());

		// Mod for Player; used for Policies and such
#if defined(MOD_BUGFIX_MINOR)
		int iTemp = GET_PLAYER(getOwner()).GetCityYieldChangeTimes100(eYield);	// In hundreds - will be added to capitalYieldChange below
#else
		int iTemp = GET_PLAYER(getOwner()).GetCityYieldChange(eYield);	// In hundreds - will be added to capitalYieldChange below
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
		iYield += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCityYieldChanges(eYield);
#endif

		// Coastal City Mod
		if(pCity->isCoastal())
		{
			iYield += GET_PLAYER(getOwner()).GetCoastalCityYieldChange(eYield);
#if defined(MOD_API_UNIFIED_YIELDS)
			iYield += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCoastalCityYieldChanges(eYield);
#endif
		}
#if defined(MOD_BALANCE_CORE)
		//Non-hill, non-freshwater city plots should make one extra gold
		if(eYield == YIELD_GOLD && !isHills() && !isFreshWater())
		{
			iYield += 1;
		}
		//Non-hill, freshwater plots should make one extra food
		if(eYield == YIELD_FOOD && !isHills() && isFreshWater())
		{
			iYield += 1;
		}
#endif
		// Capital Mod
		if(pCity->isCapital())
		{
#if defined(MOD_BUGFIX_MINOR)
			iTemp += GET_PLAYER(getOwner()).GetCapitalYieldChangeTimes100(eYield);
#else
			iTemp += GET_PLAYER(getOwner()).GetCapitalYieldChange(eYield);
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
			iYield += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCapitalYieldChanges(eYield);
#endif

			int iPerPopYield = pCity->getPopulation() * GET_PLAYER(getOwner()).GetCapitalYieldPerPopChange(eYield);
			iPerPopYield /= 100;
			iYield += iPerPopYield;
		}

		iYield += (iTemp / 100);
	}

	iYield += GC.getGame().getPlotExtraYield(m_iX, m_iY, eYield);

	if(ePlayer != NO_PLAYER)
	{
		if(GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield) > 0)
		{
			if(iYield >= GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield))
			{
				iYield += GC.getEXTRA_YIELD();
			}
		}

		if(GET_PLAYER(ePlayer).isGoldenAge())
		{
			if(iYield >= kYield.getGoldenAgeYieldThreshold())
			{
				iYield += kYield.getGoldenAgeYield();
			}
		}
	}

	return std::max(0, iYield);
}


//	--------------------------------------------------------------------------------
bool CvPlot::hasYield() const
{
	int iI;

	for(iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if(getYield((YieldTypes)iI) > 0)
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvPlot::updateYield()
{
	CvCity* pWorkingCity;
	bool bChange;
	short iNewYield;
	int iOldYield;
	int iI;

	if(area() == NULL)
	{
		return;
	}

	bChange = false;

	for(iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		iNewYield = (short)calculateYield((YieldTypes)iI);

		if(getYield((YieldTypes)iI) != (int)iNewYield)
		{
			iOldYield = getYield((YieldTypes)iI);

			m_aiYield[iI] = iNewYield;
			CvAssertMsg(getYield((YieldTypes)iI) >= 0 && getYield((YieldTypes)iI) < 50, "GAMEPLAY: Yield for a plot is either negative or a ridiculously large number.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

			pWorkingCity = getWorkingCity();

			if(pWorkingCity != NULL)
			{
				if(isBeingWorked())
				{
					pWorkingCity->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), (getYield((YieldTypes)iI) - iOldYield));
				}

				// JON: New City Citizens AI shoulud update here 08/17/09
			}

			bChange = true;
		}
	}

	if(bChange)
	{
		updateSymbols();
	}
}

#if defined(MOD_BALANCE_CORE_SETTLER)

//	--------------------------------------------------------------------------------
int CvPlot::GetExplorationBonus(const CvPlayer* pPlayer, const CvPlot* pRefPlot)
{
	if (!pPlayer)
		return 0;

	//give a bonus to fertile tiles that are close to our own territory
	int iDistToOwnCities = pPlayer->GetCityDistance(this);
	int iDistRef = pPlayer->GetCityDistance(pRefPlot);
	
	int iFertilityBonus = 0;
	if ( pPlayer->GetFoundValueOfLastSettledCity()>0 )
		iFertilityBonus = max(0, (getFoundValue(pPlayer->GetID())*100) / pPlayer->GetFoundValueOfLastSettledCity() );

	if (iDistToOwnCities>iDistRef)
		return (iFertilityBonus*70)/100;
	else
		return iFertilityBonus;
}

#endif

//	--------------------------------------------------------------------------------
int CvPlot::getFoundValue(PlayerTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_aiFoundValue[eIndex] == -1)
	{
#ifdef MOD_BALANCE_CORE_SETTLER
		m_aiFoundValue[eIndex] = GET_PLAYER(eIndex).AI_foundValue(getX(), getY() );
#else
		m_aiFoundValue[eIndex] = GET_PLAYER(eIndex).AI_foundValue(getX(), getY(), -1, true);
#endif
	}

	return m_aiFoundValue[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvPlot::isBestAdjacentFound(PlayerTypes eIndex)
{
	CvPlot* pAdjacentPlot;
	int iI;

	CvPlayer& thisPlayer = GET_PLAYER(eIndex);
#if defined(MOD_BALANCE_CORE)
	int iPlotValue = getFoundValue(eIndex);
#else
	int iPlotValue = thisPlayer.AI_foundValue(getX(), getY());
#endif

	if(iPlotValue == 0)
	{
		return false;
	}

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if((pAdjacentPlot != NULL) && pAdjacentPlot->isRevealed(thisPlayer.getTeam()))
		{
#if defined(MOD_BALANCE_CORE)
			if(pAdjacentPlot->getFoundValue(eIndex) > iPlotValue)
#else
			if(thisPlayer.AI_foundValue(pAdjacentPlot->getX(), pAdjacentPlot->getY()) > iPlotValue)
#endif
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
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	m_aiFoundValue[eIndex] = iNewValue;
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

//	--------------------------------------------------------------------------------
PlotVisibilityChangeResult CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bInformExplorationTracking, bool bAlwaysSeeInvisible)
{
	CvCity* pCity;
	CvPlot* pAdjacentPlot;
	bool bOldVisible;

	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	PlotVisibilityChangeResult eResult = VISIBILTY_CHANGE_NONE;
	if(iChange != 0)
	{
		bOldVisible = isVisible(eTeam);

		m_aiVisibilityCount[eTeam] += iChange;
		CvAssertFmt(m_aiVisibilityCount[eTeam]>=0, "Changing plot X:%d, Y:%d to a negative visibility", getX(), getY());
		if (m_aiVisibilityCount[eTeam] < 0)
			m_aiVisibilityCount[eTeam] = 0;

		if(eSeeInvisible != NO_INVISIBLE)
		{
			changeInvisibleVisibilityCount(eTeam, eSeeInvisible, iChange);
		}

		if(bAlwaysSeeInvisible)
		{
			for(int iI = 0; iI < NUM_INVISIBLE_TYPES; iI++)
			{
				changeInvisibleVisibilityCount(eTeam, (InvisibleTypes) iI, iChange);
			}
		}

		if(bOldVisible != isVisible(eTeam))
		{
			// We couldn't see the Plot before but we can now
			if(isVisible(eTeam))
			{
				eResult = VISIBILITY_CHANGE_TO_VISIBLE;
				bool bOldRevealed = isRevealed(eTeam);
				if (!setRevealed(eTeam, true))	// Change to revealed, returns true if the visibility was changed
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

				if(bInformExplorationTracking && !bOldRevealed)
				{
					// slewis - ghetto-tastic hack. ugh
					for(int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						CvPlayerAI& playerI = GET_PLAYER((PlayerTypes)iI);
						if(playerI.isAlive())
						{
							if(playerI.getTeam() == eTeam)
							{
								playerI.GetEconomicAI()->m_bExplorationPlotsDirty = true;
							}
						}
					}
				}

				for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

					if(pAdjacentPlot != NULL)
					{
						pAdjacentPlot->updateRevealedOwner(eTeam);
					}
				}

				// If there are any Units here, meet their owners
				for(int iUnitLoop = 0; iUnitLoop < getNumUnits(); iUnitLoop++)
				{
					// If the AI spots a human Unit, don't meet - wait for the human to find the AI
					CvUnit* loopUnit = getUnitByIndex(iUnitLoop);

					if(!loopUnit) continue;

					if(!GET_TEAM(eTeam).isHuman() && loopUnit->isHuman())
					{
						continue;
					}

					GET_TEAM(eTeam).meet(loopUnit->getTeam(), false);
				}

				// If there's a City here, meet its owner
				if(isCity())
				{
					// If the AI spots a human City, don't meet - wait for the human to find the AI
					if(GET_TEAM(eTeam).isHuman() || !getPlotCity()->isHuman())
					{
						GET_TEAM(eTeam).meet(getTeam(), false);	// If there's a City here, we can assume its owner is the same as the plot owner
					}
				}
			}
			else
			{
				eResult = VISIBILITY_CHANGE_TO_INVISIBLE;
				if(eTeam == GC.getGame().getActiveTeam())
					updateFog(true);
			}

			pCity = getPlotCity();

			if(eTeam == GC.getGame().getActiveTeam())
			{
				//updateFog();
				updateCenterUnit();
			}
		}
	}

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
	CvPlot* pAdjacentPlot;
	bool bRevealed;
	int iI;

	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	bRevealed = false;

	if(!bRevealed)
	{
		if(isVisible(eTeam))
		{
			bRevealed = true;
		}
	}

	if(!bRevealed)
	{
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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

	setRevealedOwner(eTeam, ((bRevealed) ? getOwner() : NO_PLAYER));
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

//	--------------------------------------------------------------------------------
bool CvPlot::IsNoSettling(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	return m_abNoSettling[eMajor];
}

//	--------------------------------------------------------------------------------
void CvPlot::SetNoSettling(PlayerTypes eMajor, bool bValue)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if(bValue != IsNoSettling(eMajor))
		m_abNoSettling[eMajor] = bValue;
}

//	--------------------------------------------------------------------------------
bool CvPlot::setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam)
{
	int iI;
	
#if defined(MOD_EVENTS_TILE_REVEALED)
	// We need to capture this value here, as a Natural Wonder may update it before we need it
	int iRevealedMajors = getNumMajorCivsRevealed();
#endif

	CvAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	CvCity* pCity = getPlotCity();
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
		if(eTeam != BARBARIAN_TEAM)
		{
			if(getFeatureType() != NO_FEATURE)
			{
				if(GC.getFeatureInfo(getFeatureType())->IsNaturalWonder())
				{
					GET_TEAM(eTeam).ChangeNumNaturalWondersDiscovered(1);

					int iNumNaturalWondersLeft = GC.getMap().GetNumNaturalWonders() - GET_TEAM(eTeam).GetNumNaturalWondersDiscovered();

#if defined(MOD_EVENTS_NW_DISCOVERY)
					if (MOD_EVENTS_NW_DISCOVERY) {
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_NaturalWonderDiscovered, eTeam, getFeatureType(), getX(), getY(), (getNumMajorCivsRevealed() == 0));
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

#if defined(MOD_BALANCE_CORE_HAPPINESS)
								if(MOD_BALANCE_CORE_HAPPINESS)
								{
									if(playerI.isHuman())
									{
										playerI.CalculateHappiness();
									}
								}
								else
								{
#endif
								playerI.DoUpdateHappiness();
#if defined(MOD_BALANCE_CORE_HAPPINESS)
								}
#endif

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
#if !defined(SHOW_PLOT_POPUP)
									float fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 3;
#endif
									text[0] = NULL;
									sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iFinderGold);
#if defined(SHOW_PLOT_POPUP)
									SHOW_PLOT_POPUP(this, NO_PLAYER, text, 0.0f);
#else
									GC.GetEngineUserInterface()->AddPopupText(getX(), getY(), text, fDelay);
#endif
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

						// Popup (no MP)
						if(!GC.getGame().isNetworkMultiPlayer() && !bDontShowRewardPopup)	// KWG: candidate for !GC.getGame().isOption(GAMEOPTION_SIMULTANEOUS_TURNS)
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

				if(pCity != NULL)
				{
					bVisibilityChanged |= pCity->setRevealed(eTeam, true);
				}
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

				if(pCity != NULL)
				{
					if(pCity->isRevealed(eFromTeam, false))
					{
						bVisibilityChanged |= pCity->setRevealed(eTeam, true);
					}
				}
			}
		}
		else
		{
			bVisibilityChanged |= setRevealedOwner(eTeam, NO_PLAYER);
			bImprovementVisibilityChanged |= setRevealedImprovementType(eTeam, NO_IMPROVEMENT);
			bVisibilityChanged |= setRevealedRouteType(eTeam, NO_ROUTE);

			if(pCity != NULL)
			{
				bVisibilityChanged |= pCity->setRevealed(eTeam, false);
			}
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
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_TileRevealed, getX(), getY(), eTeam, eFromTeam, (kTeam.isMajorCiv() && iRevealedMajors == 0));
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
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
	CvPlot* pAdjacentPlot;
	int iI;

	for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

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
	CvPlot* pAdjacentPlot;
	int iCount = 0;

	for(int i = 0; i < NUM_DIRECTION_TYPES; ++i)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)i));

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
	if(NULL != m_paiBuildProgress)  // if it doesn't exist no point in clearing it out
	{
		int iNumBuildInfos = GC.getNumBuildInfos();
		for(int iThisBuild = 0 ; iThisBuild < iNumBuildInfos; iThisBuild++)
		{
			CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iThisBuild);
			if(!pkBuildInfo)
			{
				continue;
			}

			// Is this an Improvement?
			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				m_paiBuildProgress[iThisBuild] = 0;
			}
		}
	}
}


//	--------------------------------------------------------------------------------
int CvPlot::getBuildProgress(BuildTypes eBuild) const
{
	if(NULL == m_paiBuildProgress)
	{
		return 0;
	}

	return m_paiBuildProgress[eBuild];
}

//	--------------------------------------------------------------------------------
bool CvPlot::getAnyBuildProgress() const
{
	return (NULL != m_paiBuildProgress);
}


//	--------------------------------------------------------------------------------
// Returns true if build finished...
bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer)
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

	if(iChange != 0)
	{
		if(NULL == m_paiBuildProgress)
		{
			m_paiBuildProgress = FNEW(short[GC.getNumBuildInfos()], c_eCiv5GameplayDLL, 0);
			for(int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				m_paiBuildProgress[iI] = 0;
			}
		}

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (eImprovement != m_eImprovementTypeUnderConstruction)
			{
				SilentlyResetAllBuildProgress();
				m_eImprovementTypeUnderConstruction = eImprovement;
			}
		}

		m_paiBuildProgress[eBuild] += iChange;
		CvAssert(getBuildProgress(eBuild) >= 0);

		if(getBuildProgress(eBuild) >= getBuildTime(eBuild, ePlayer))
		{
			m_paiBuildProgress[eBuild] = 0;

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
				if(getOwner() == NO_PLAYER)
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
					CvAssertMsg (GetArchaeologicalRecord().m_eArtifactType != NO_GREAT_WORK_ARTIFACT_CLASS, "Archaeological dig complete but no archeology data found!");

					kPlayer.SetNumArchaeologyChoices(kPlayer.GetNumArchaeologyChoices() + 1);
					kPlayer.GetCulture()->AddDigCompletePlot(this);

					if(kPlayer.isHuman())
					{
						CvNotifications* pNotifications;
						Localization::String locString;
						Localization::String locSummary;
						pNotifications = kPlayer.GetNotifications();
						if(pNotifications)
						{
							strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHOOSE_ARCHAEOLOGY");
							CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_ARCHAEOLOGY");
							pNotifications->Add(NOTIFICATION_CHOOSE_ARCHAEOLOGY, strBuffer, strSummary, getX(), getY(), kPlayer.GetID());
							CancelActivePlayerEndTurn();
						}

#if !defined(NO_ACHIEVEMENTS)
						// Raiders of the Lost Ark achievement
						const char* szCivKey = kPlayer.getCivilizationTypeKey();
						if(getOwner() != NO_PLAYER && !GC.getGame().isNetworkMultiPlayer() && strcmp(szCivKey, "CIVILIZATION_AMERICA") == 0)
						{
							CvPlayer &kPlotOwner = GET_PLAYER(getOwner());
							szCivKey = kPlotOwner.getCivilizationTypeKey();
							if(strcmp(szCivKey, "CIVILIZATION_EGYPT") == 0)
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
											for(pLoopUnit = kLoopPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iUnitLoop))
											{
												if(strcmp(pLoopUnit->getUnitInfo().GetType(), "UNIT_ARCHAEOLOGIST") == 0)
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
						kPlayer.GetCulture()->DoArchaeologyChoice (eChoice);
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
UnitHandle CvPlot::getCenterUnit()
{
	return m_pCenterUnit;
}

//	--------------------------------------------------------------------------------
const UnitHandle CvPlot::getCenterUnit() const
{
	return m_pCenterUnit;
}


//	--------------------------------------------------------------------------------
const CvUnit* CvPlot::getDebugCenterUnit() const
{
	const CvUnit* pCenterUnit;

	pCenterUnit = getCenterUnit().pointer();

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
void CvPlot::setCenterUnit(UnitHandle pNewValue)
{
	UnitHandle pOldValue;

	pOldValue = getCenterUnit();

	m_pCenterUnit = pNewValue;
	m_pCenterUnit.ignoreDestruction(true);

	if(pOldValue != pNewValue)
	{
		UnitHandle newCenterUnit = getCenterUnit();
		if(newCenterUnit)
		{
			newCenterUnit->setInfoBarDirty(true);
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
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return 0;
	if(eInvisible < 0 || eInvisible >= NUM_INVISIBLE_TYPES) return 0;
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
	if(eTeam < 0 || eTeam >= MAX_TEAMS) return;
	if(eInvisible < 0 || eInvisible >= NUM_INVISIBLE_TYPES) return;

	if(iChange != 0)
	{
		bOldInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);

		m_apaiInvisibleVisibilityCount[eTeam][eInvisible] = (m_apaiInvisibleVisibilityCount[eTeam][eInvisible] + iChange);

		CvAssertFmt(m_apaiInvisibleVisibilityCount[eTeam][eInvisible] >= 0, "Invisible Visibility going negative for %d, %d", m_iX, m_iY);

		bNewInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);

		if(bOldInvisibleVisible != bNewInvisibleVisible)
		{
			TeamTypes activeTeam = GC.getGame().getActiveTeam();
			if(eTeam == activeTeam)
			{
				// for all (nominally invisible) units in this plot
				// tell the engine to flip whether they are being drawn or not
				IDInfo* pUnitNode;
				CvUnit* pLoopUnit = NULL;
				pUnitNode = headUnitNode();
				while(pUnitNode != NULL)
				{
					pLoopUnit = GetPlayerUnit(*pUnitNode);
					pUnitNode = nextUnitNode(pUnitNode);

					if(NULL != pLoopUnit && pLoopUnit->getTeam() != activeTeam && pLoopUnit->getInvisibleType() == eInvisible)
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
						return ::getUnit( kUnits[iIndex - iCount] );
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
				return ::getUnit( kUnits[iIndex] );
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
int CvPlot::getNumUnits() const
{
	return m_units.getLength();
}

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

		if(pLoopUnit && pLoopUnit->IsCombatUnit())
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
	const IDInfo* pUnitNode;

	pUnitNode = m_units.nodeNum(iIndex);

	if(pUnitNode != NULL)
	{
		return (CvUnit*)GetPlayerUnit(*pUnitNode);
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
	CvString scriptData = (m_szScriptData != NULL)? m_szScriptData : "";
	return scriptData;
}

//	--------------------------------------------------------------------------------
void CvPlot::setScriptData(const char* szNewValue)
{
	SAFE_DELETE_ARRAY(m_szScriptData);
	m_szScriptData = _strdup(szNewValue);
}

#if defined(SHOW_PLOT_POPUP)
void CvPlot::showPopupText(PlayerTypes ePlayer, const char* szMessage)
{
	if (ePlayer == NO_PLAYER || isVisible(GET_PLAYER(ePlayer).getTeam()))
	{
		DLLUI->AddPopupText(getX(), getY(), szMessage, m_fPopupDelay);
		m_fPopupDelay += 0.5;
	}
}
#endif

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
	int iCount;

	// Init saved data
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	CvAssertMsg(uiVersion <= g_CurrentCvPlotVersion, "Unexpected Version.  This could be caused by serialization errors.");
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iX;
	kStream >> m_iY;
	kStream >> m_iArea;
	kStream >> m_iFeatureVariety;
	kStream >> m_iOwnershipDuration;
	kStream >> m_iImprovementDuration;
	kStream >> m_iUpgradeProgress;
	kStream >> m_iCulture;
	kStream >> m_iNumMajorCivsRevealed;
	kStream >> m_iCityRadiusCount;
	kStream >> m_iReconCount;
	kStream >> m_iRiverCrossingCount;
	kStream >> m_iResourceNum;
	kStream >> m_cBuilderAIScratchPadPlayer;
	kStream >> m_sBuilderAIScratchPadTurn;
	kStream >> m_sBuilderAIScratchPadValue;
	kStream >> m_eBuilderAIScratchPadRoute;
	kStream >> m_iLandmass;
	kStream >> m_uiTradeRouteBitFlags;

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
	m_bImprovementPillaged = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bRoutePillaged = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bBarbCampNotConverting = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bRoughFeature = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bResourceLinkedCityActive = bitPackWorkaround;
	kStream >> bitPackWorkaround;
	m_bImprovedByGiftFromMajor = bitPackWorkaround;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	MOD_SERIALIZE_READ(49, kStream, bitPackWorkaround, 0);
	m_bImprovementEmbassy = bitPackWorkaround;
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
	kStream >> m_workingCity.eOwner;
	kStream >> m_workingCity.iID;
	kStream >> m_workingCityOverride.eOwner;
	kStream >> m_workingCityOverride.iID;
	kStream >> m_ResourceLinkedCity.eOwner;
	kStream >> m_ResourceLinkedCity.iID;
	kStream >> m_purchaseCity.eOwner;
	kStream >> m_purchaseCity.iID;

	for(uint i = 0; i < NUM_YIELD_TYPES; i++)
		kStream >> m_aiYield[i];

	for(uint i = 0; i < REALLY_MAX_PLAYERS; i++)
		kStream >> m_aiFoundValue[i];

	for(uint i = 0; i < REALLY_MAX_PLAYERS; i++)
		kStream >> m_aiPlayerCityRadiusCount[i];

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
	{
		kStream >> m_aiVisibilityCount[i];
		if (m_aiVisibilityCount[i] < 0)
			m_aiVisibilityCount[i] = 0;
	}

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream >> m_aiRevealedOwner[i];

	kStream >> m_cRiverCrossing;

	for(uint i = 0; i<PlotBoolField::eCount; ++i)
	{
		kStream >> m_bfRevealed.m_dwBits[i];
	}

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream >> m_abResourceForceReveal[i];

	if (uiVersion >= 6)
	{
		for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
			m_aeRevealedImprovementType[i] = (ImprovementTypes) CvInfosSerializationHelper::ReadHashed(kStream);
	}
	else
	{
		for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		{
			kStream >> m_aeRevealedImprovementType[i];
			if (m_aeRevealedImprovementType[i] != NO_IMPROVEMENT)
				if (GC.getImprovementInfo((ImprovementTypes)m_aeRevealedImprovementType[i]) == NULL)
					m_aeRevealedImprovementType[i] = NO_IMPROVEMENT;
		}
	}

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream >> m_aeRevealedRouteType[i];

	for(uint i = 0; i < MAX_MAJOR_CIVS; i++)
		kStream >> m_abNoSettling[i];

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	for(uint i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		MOD_SERIALIZE_READ(36, kStream, m_abAvoidMovement[i], false);
	}
#endif

	bool hasScriptData = false;
	kStream >> hasScriptData;
	if(hasScriptData)
	{
		std::string scriptData;
		kStream >> scriptData;
		setScriptData(scriptData.c_str());
	}

	SAFE_DELETE_ARRAY(m_paiBuildProgress);
	kStream >> iCount;
	if(iCount > 0)
	{
		const int iNumBuildInfos = GC.getNumBuildInfos();
		m_paiBuildProgress = FNEW(short[iNumBuildInfos], c_eCiv5GameplayDLL, 0);
		ZeroMemory(m_paiBuildProgress, sizeof(short) * iNumBuildInfos);
		
		BuildArrayHelpers::Read(kStream, m_paiBuildProgress);
	}

	kStream >> m_apaiInvisibleVisibilityCount;

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

	updateImpassable();
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
	kStream << m_iFeatureVariety;
	kStream << m_iOwnershipDuration;
	kStream << m_iImprovementDuration;
	kStream << m_iUpgradeProgress;
	kStream << m_iCulture;
	kStream << m_iNumMajorCivsRevealed;
	kStream << m_iCityRadiusCount;
	kStream << m_iReconCount;
	kStream << m_iRiverCrossingCount;
	kStream << m_iResourceNum;
	kStream << m_cBuilderAIScratchPadPlayer;
	kStream << m_sBuilderAIScratchPadTurn;
	kStream << m_sBuilderAIScratchPadValue;
	kStream << m_eBuilderAIScratchPadRoute;
	kStream << m_iLandmass;
	kStream << m_uiTradeRouteBitFlags;

	kStream << m_bStartingPlot;
	kStream << m_bHills;
	kStream << m_bNEOfRiver;
	kStream << m_bWOfRiver;
	kStream << m_bNWOfRiver;
	kStream << m_bPotentialCityWork;
	kStream << m_bImprovementPillaged;
	kStream << m_bRoutePillaged;
	kStream << m_bBarbCampNotConverting;
	kStream << m_bRoughFeature;
	kStream << m_bResourceLinkedCityActive;
	kStream << m_bImprovedByGiftFromMajor;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	MOD_SERIALIZE_WRITE(kStream, m_bImprovementEmbassy);
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
	kStream << m_workingCity.eOwner;
	kStream << m_workingCity.iID;
	kStream << m_workingCityOverride.eOwner;
	kStream << m_workingCityOverride.iID;
	kStream << m_ResourceLinkedCity.eOwner;
	kStream << m_ResourceLinkedCity.iID;
	kStream << m_purchaseCity.eOwner;
	kStream << m_purchaseCity.iID;

	for(uint i = 0; i < NUM_YIELD_TYPES; i++)
		kStream << m_aiYield[i];

	for(uint i = 0; i < REALLY_MAX_PLAYERS; i++)
		kStream << m_aiFoundValue[i];

	for(uint i = 0; i < REALLY_MAX_PLAYERS; i++)
		kStream << m_aiPlayerCityRadiusCount[i];

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream << m_aiVisibilityCount[i];

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream << m_aiRevealedOwner[i];

	kStream << m_cRiverCrossing;

	//for(uint i = 0; i < REALLY_MAX_TEAMS;i++)
	//	kStream << m_abRevealed[i];

	for(uint i = 0; i<PlotBoolField::eCount; ++i)
	{
		kStream << m_bfRevealed.m_dwBits[i];
	}

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream << m_abResourceForceReveal[i];

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, (const ImprovementTypes)m_aeRevealedImprovementType[i]);
	}

	for(uint i = 0; i < REALLY_MAX_TEAMS; i++)
		kStream << m_aeRevealedRouteType[i];

	for(uint i = 0; i < MAX_MAJOR_CIVS; i++)
		kStream << m_abNoSettling[i];

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	for(uint i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		MOD_SERIALIZE_WRITE(kStream, m_abAvoidMovement[i]);
	}
#endif

	// char * should have died in 1989...
	bool hasScriptData = (m_szScriptData != NULL);
	kStream << hasScriptData;
	if(hasScriptData)
	{
		const std::string scriptData(m_szScriptData);
		kStream << scriptData;
	}

	if(NULL == m_paiBuildProgress)
	{
		kStream << (int)0;
	}
	else
	{
		kStream << (int)GC.getNumBuildInfos();
		BuildArrayHelpers::Write(kStream, m_paiBuildProgress, GC.getNumBuildInfos());
	}

	kStream << m_apaiInvisibleVisibilityCount;

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
			else if(IsTradeRoute())
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
			else if(IsTradeRoute())
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

		if((eRevealedImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eRevealedImprovement)->IsImprovementResourceTrade(eType))
		{
			bImproved = true;
			bWorked = isBeingWorked();
		}
	}
}

//	--------------------------------------------------------------------------------
int CvPlot::getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade, PlayerTypes ePlayer) const
{
	int iYield;

	const CvYieldInfo& kYield = *GC.getYieldInfo(eYield);
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if(getTerrainType() == NO_TERRAIN)
		return 0;

	if(!isPotentialCityWork())
		return 0;

	bool bCity = false;

	// Will the build remove the feature?
	bool bIgnoreFeature = false;
	if(getFeatureType() != NO_FEATURE)
	{
		if(GC.getBuildInfo(eBuild)->isFeatureRemove(getFeatureType()))
			bIgnoreFeature = true;
	}

	// Nature yield
	iYield = calculateNatureYield(eYield, getTeam(), bIgnoreFeature);

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();

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

		iYield += calculateImprovementYieldChange(eImprovement, eYield, ePlayer, false);

#if defined(MOD_API_UNIFIED_YIELDS)
		if (getOwner() != NO_PLAYER)
#else
		if (eYield == YIELD_CULTURE && getOwner() != NO_PLAYER)
#endif
		{
			CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
#if defined(MOD_API_UNIFIED_YIELDS)
			if(pImprovement && pImprovement->GetYieldChange(eYield) > 0)
#else
			if(pImprovement && pImprovement->GetYieldChange(YIELD_CULTURE) > 0)
#endif
			{
#if defined(MOD_API_UNIFIED_YIELDS)
				int iAdjacentCulture = pImprovement->GetYieldAdjacentSameType(eYield);
				iAdjacentCulture += pImprovement->GetYieldAdjacentTwoSameType(eYield);
#else
				int iAdjacentCulture = pImprovement->GetCultureAdjacentSameType();
#endif
				if(iAdjacentCulture > 0)
				{
#if defined(MOD_API_UNIFIED_YIELDS)
					iYield += ComputeYieldFromAdjacentImprovement(*pImprovement, eImprovement, eYield);
					iYield += ComputeYieldFromTwoAdjacentImprovement(*pImprovement, eImprovement, eYield);
#else
					iYield += ComputeCultureFromAdjacentImprovement(*pImprovement, eImprovement);
#endif
				}
			}

#if defined(MOD_API_UNIFIED_YIELDS)
			if (eYield == YIELD_CULTURE)
			{
#endif
				iYield += GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_EXTRA_CULTURE_FROM_IMPROVEMENTS);
				iYield += GET_PLAYER(getOwner()).GetPlayerPolicies()->GetImprovementCultureChange(eImprovement);
#if defined(MOD_API_UNIFIED_YIELDS)
			}
#endif
		}
	}

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
		eImprovement = getImprovementType();
		if(eImprovement != NO_IMPROVEMENT)
		{
#if defined(MOD_BALANCE_CORE)
			int iRouteYield = 0;
			CvGameTrade* pTrade = GC.getGame().GetGameTrade();
			bool bTrade = false;
			for (uint uiConnection = 0; uiConnection < pTrade->m_aTradeConnections.size(); uiConnection++)
			{
				TradeConnection* pConnection = &(pTrade->m_aTradeConnections[uiConnection]);
				if (pTrade->IsTradeRouteIndexEmpty(uiConnection))
				{
					continue;
				}
				for (uint ui = 0; ui < pConnection->m_aPlotList.size(); ui++)
				{
					if (pConnection->m_aPlotList[ui].m_iX == getX() && pConnection->m_aPlotList[ui].m_iY == getY())
					{
						bTrade = true;
						break;
					}
				}
			}
			if(IsTradeRoute(ePlayer))
			{
				if(IsRouteRailroad())
				{
					iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
					if(getRouteType() != NO_ROUTE)
					{
						iRouteYield -= GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
					}
				}
				else if(IsRouteRoad())
				{
					iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
					if(getRouteType() != NO_ROUTE)
					{
						iRouteYield -= GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
					}
				}
			}
			if(bTrade)
			{
				if(IsRouteRailroad())
				{
					iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
					if(getRouteType() != NO_ROUTE)
					{
						iRouteYield -= GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_RAILROAD, eYield);
					}
				}
				else if(IsRouteRoad())
				{
					iRouteYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
					if(getRouteType() != NO_ROUTE)
					{
						iRouteYield -= GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(ROUTE_ROAD, eYield);
					}
				}
			}
			if(iRouteYield > 0)
			{
				iYield += iRouteYield;
			}
#else
			for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iYield += GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(eRoute, iI);
				if(getRouteType() != NO_ROUTE)
				{
					iYield -= GC.getImprovementInfo(eImprovement)->GetRouteYieldChanges(getRouteType(), iI);
				}
			}
#endif
		}
	}

	CvCity* pCity = getPlotCity();

	if(ePlayer != NO_PLAYER)
	{
		// City plot yield
		if(pCity != NULL)
		{
			if(pCity->isRevealed(eTeam, false))
			{
				iYield += kYield.getCityChange();

				if(kYield.getPopulationChangeDivisor() != 0)
					iYield += (pCity->getPopulation() + kYield.getPopulationChangeOffset()) / kYield.getPopulationChangeDivisor();

				bCity = true;
			}
		}

		CvCity* pWorkingCity = getWorkingCity();

		// Water plots
		if(isWater())
		{
			if(!isImpassable() && !isMountain())
			{
				// Player sea plot yield
				iYield += GET_PLAYER(ePlayer).getSeaPlotYield(eYield);

				if(pWorkingCity != NULL)
				{
					if(pWorkingCity->isRevealed(eTeam, false))
					{
						int iCityYield;

						// Worked lake plot
						if(pWorkingCity->getLakePlotYield(eYield) > 0 && isLake())
							iCityYield = pWorkingCity->getLakePlotYield(eYield);
						// Worked sea plot
						else
							iCityYield = pWorkingCity->getSeaPlotYield(eYield);

						iYield += iCityYield;
					}
				}

				// Worked water resources
				if(getResourceType(GET_PLAYER(ePlayer).getTeam()) != NO_RESOURCE)
				{
					if(pWorkingCity != NULL)
					{
						if(pWorkingCity->isRevealed(eTeam, false))
							iYield += pWorkingCity->getSeaResourceYield(eYield);
					}
				}

			}
		}

		// Worked river plot
		if(isRiver())
		{
			if(!isImpassable() && !isMountain())
			{
				if(NULL != pWorkingCity)
				{
					if(pWorkingCity->isRevealed(eTeam, false))
					{
						iYield += pWorkingCity->getRiverPlotYield(eYield);
					}
				}
			}
		}

		// Worked Feature extra yield (e.g. University bonus)
		if(getFeatureType() != NO_FEATURE)
		{
			if(pWorkingCity != NULL)
				iYield += pWorkingCity->GetFeatureExtraYield(getFeatureType(), eYield);
		}


#if defined(MOD_API_UNIFIED_YIELDS) && defined(MOD_API_PLOT_YIELDS)
		// Extra yield for plot
		if(pWorkingCity != NULL)
		{
			iYield += pWorkingCity->GetPlotExtraYield(getPlotType(), eYield);
		}
#endif

		// Extra yield for terrain
		if(getTerrainType() != NO_TERRAIN)
		{
			if(pWorkingCity != NULL && !isImpassable() && !isMountain())
			{
				iYield += pWorkingCity->GetTerrainExtraYield(getTerrainType(), eYield);
			}
		}

		ResourceTypes eResource = getResourceType(GET_PLAYER(ePlayer).getTeam());
		if(eResource != NO_RESOURCE)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && eTeam != NO_TEAM && GET_TEAM(eTeam).GetTeamTechs()->HasTech((TechTypes) pkResourceInfo->getTechReveal()))
			{
				if (pkResourceInfo->getPolicyReveal() == NO_POLICY || GET_PLAYER(ePlayer).GetPlayerPolicies()->HasPolicy((PolicyTypes)pkResourceInfo->getPolicyReveal()))
				{
					// Extra yield from resources
					if(pWorkingCity != NULL)
						iYield += pWorkingCity->GetResourceExtraYield(eResource, eYield);

					// Extra yield from Resources with Trait
					if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
						iYield += GET_PLAYER(ePlayer).GetPlayerTraits()->GetYieldChangeStrategicResources(eYield);
				}
			}
		}
	}

	if(bCity)
	{
		iYield = std::max(iYield, kYield.getMinCity());

		// Mod for Player; used for Policies and such
#if defined(MOD_BUGFIX_MINOR)
		int iTemp = GET_PLAYER(getOwner()).GetCityYieldChangeTimes100(eYield);	// In hundreds - will be added to capitalYieldChange below
#else
		int iTemp = GET_PLAYER(getOwner()).GetCityYieldChange(eYield);	// In hundreds - will be added to capitalYieldChange below
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
		iTemp += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCityYieldChanges(eYield);
#endif

		// Coastal City Mod
		if(pCity->isCoastal())
		{
			iYield += GET_PLAYER(getOwner()).GetCoastalCityYieldChange(eYield);
#if defined(MOD_API_UNIFIED_YIELDS)
			iYield += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCoastalCityYieldChanges(eYield);
#endif
		}

		// Capital Mod
		if(pCity->isCapital())
		{
#if defined(MOD_BUGFIX_MINOR)
			iTemp += GET_PLAYER(getOwner()).GetCapitalYieldChangeTimes100(eYield);
#else
			iTemp += GET_PLAYER(getOwner()).GetCapitalYieldChange(eYield);
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
			iYield += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCapitalYieldChanges(eYield);
#endif

			int iPerPopYield = pCity->getPopulation() * GET_PLAYER(getOwner()).GetCapitalYieldPerPopChange(eYield);
			iPerPopYield /= 100;
			iYield += iPerPopYield;
		}

		iYield += (iTemp / 100);
	}

	iYield += GC.getGame().getPlotExtraYield(m_iX, m_iY, eYield);

	if(ePlayer != NO_PLAYER)
	{
		if(GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield) > 0)
		{
			if(iYield >= GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield))
			{
				iYield += GC.getEXTRA_YIELD();
			}
		}

		if(GET_PLAYER(ePlayer).isGoldenAge())
		{
			if(iYield >= kYield.getGoldenAgeYieldThreshold())
			{
				iYield += kYield.getGoldenAgeYield();
			}
		}
	}

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
		else
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
			if(!isWater() && !isCoastalLand(thisUnitEntry.GetMinAreaSize()))
			{
				return false;
			}
		}
		else
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
int CvPlot::countNumAirUnits(TeamTypes eTeam) const
{
	int iCount = 0;

	const IDInfo* pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = nextUnitNode(pUnitNode);

		if(pLoopUnit && DOMAIN_AIR == pLoopUnit->getDomainType() && !pLoopUnit->isCargo() && pLoopUnit->getTeam() == eTeam)
		{
			iCount += pLoopUnit->getUnitInfo().GetAirUnitCap();
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvPlot::GetBuilderAIScratchPadPlayer() const
{
	return (PlayerTypes)m_cBuilderAIScratchPadPlayer;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetBuilderAIScratchPadPlayer(PlayerTypes ePlayer)
{
	m_cBuilderAIScratchPadPlayer = ePlayer;
}

//	--------------------------------------------------------------------------------
short CvPlot::GetBuilderAIScratchPadTurn() const
{
	return m_sBuilderAIScratchPadTurn;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetBuilderAIScratchPadTurn(short sNewTurnValue)
{
	m_sBuilderAIScratchPadTurn = sNewTurnValue;
}

//	--------------------------------------------------------------------------------
RouteTypes CvPlot::GetBuilderAIScratchPadRoute() const
{
	return m_eBuilderAIScratchPadRoute;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetBuilderAIScratchPadRoute(RouteTypes eRoute)
{
	m_eBuilderAIScratchPadRoute = eRoute;
}

//	--------------------------------------------------------------------------------
short CvPlot::GetBuilderAIScratchPadValue() const
{
	return m_sBuilderAIScratchPadValue;
}

//	--------------------------------------------------------------------------------
void CvPlot::SetBuilderAIScratchPadValue(short sNewValue)
{
	m_sBuilderAIScratchPadValue = sNewValue;
}

//	--------------------------------------------------------------------------------
int CvPlot::GetPlotIndex() const
{
	return GC.getMap().plotNum(getX(),getY());
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
#if defined(MOD_BALANCE_CORE)
// Citadel
int CvPlot::GetDamageFromNearByFeatures(PlayerTypes ePlayer) const
{
	int iCitadelRange = 1;

	CvPlot* pLoopPlot;
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	ImprovementTypes eImprovement;
	int iDamage = 0, iTotalDamage = 0;

	// Look around this Unit to see if there's an adjacent Citadel
	for(int iX = -iCitadelRange; iX <= iCitadelRange; iX++)
	{
		for(int iY = -iCitadelRange; iY <= iCitadelRange; iY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iCitadelRange);

			if(pLoopPlot != NULL && pLoopPlot->isRevealed(eTeam) )
			{
				eImprovement = pLoopPlot->getImprovementType();

				// Citadel here?
				if(eImprovement != NO_IMPROVEMENT && !pLoopPlot->IsImprovementPillaged())
				{
					iDamage = GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage();
					if(iDamage != 0)
					{
						if(pLoopPlot->getOwner() != NO_PLAYER)
						{
							if(GET_TEAM(eTeam).isAtWar(pLoopPlot->getTeam()))
							{
								iTotalDamage += iDamage;
							}
						}
					}
				}
			}
		}
	}

	return iTotalDamage;
}
#endif
//	---------------------------------------------------------------------------
void CvPlot::updateImpassable()
{
	const TerrainTypes eTerrain = getTerrainType();
	const FeatureTypes eFeature = getFeatureType();
#if defined(MOD_BALANCE_CORE)
	const PlotTypes ePlot = getPlotType();
#endif

	m_bIsImpassable = false;
#if defined(MOD_BALANCE_CORE)
		bool bA = false, bB = false, bC = false;

	if(ePlot != NO_PLOT)
	{
		CvPlotInfo* pkPlotInfo = GC.getPlotInfo(ePlot);
		if(pkPlotInfo)
			bA = pkPlotInfo->isImpassable();
	}
	if(eTerrain != NO_TERRAIN)
	{
		CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
		if(pkTerrainInfo)
			bB = pkTerrainInfo->isImpassable();
	}
	if(eFeature != NO_FEATURE)
	{
		CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
		if(pkFeatureInfo)
			bC = pkFeatureInfo->isImpassable();
	}

	m_bIsImpassable = bA || bB || bC;
}
#else
	if(eTerrain != NO_TERRAIN)
	{
		if(eFeature == NO_FEATURE)
		{
			CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
			if(pkTerrainInfo)
				m_bIsImpassable = pkTerrainInfo->isImpassable();
		}
		else
		{
			CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
			if(pkFeatureInfo)
				m_bIsImpassable = pkFeatureInfo->isImpassable();
		}
	}
}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
bool CvPlot::IsAvoidMovement(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be greater than or equal to 0");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be less than MAX_MAJOR_CIVS");

	return m_abAvoidMovement[ePlayer];
}

void CvPlot::SetAvoidMovement(PlayerTypes ePlayer, bool bNewValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be greater than or equal to 0");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be less than MAX_MAJOR_CIVS");

	m_abAvoidMovement[ePlayer] = bNewValue;
}
#endif

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
#if defined(MOD_BALANCE_CORE)
int CvPlot::GetDefenseBuildValue()
{
	TeamTypes eTeam = getTeam();
	if(eTeam == NO_TEAM)
	{
		return 0;
	}
	ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");

	// See how many outside plots are nearby to monitor
	int iAdjacentUnowned = 0;
	int iAdjacentOwned = 0;
	int iAdjacentForts = 0;
	int iNearbyOwned = 0;
	int iRange = 3;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		//Don't want them adjacent to cities, but we do want to check for plot ownership.
		if (pLoopAdjacentPlot != NULL)
		{	
			if(pLoopAdjacentPlot->isCity())
			{
				//Adjacent to city? Break!
				return 0;
			}
			else if(pLoopAdjacentPlot->isImpassable() || pLoopAdjacentPlot->isWater())
			{
				//don't consider impassable and water plots
				continue;
			}
			else if(pLoopAdjacentPlot->getOwner()==NO_PLAYER)
			{
				iAdjacentUnowned++;
			}
			else if(pLoopAdjacentPlot->getOwner()!=getOwner() && !(GET_PLAYER(pLoopAdjacentPlot->getOwner()).isMinorCiv()))
			{
				iAdjacentOwned++;
			}

			//Let's check for adjacent forts as well
			if(eFort != NO_IMPROVEMENT && pLoopAdjacentPlot->getImprovementType() == eFort)
			{
				iAdjacentForts++;
			}
		}
	}
	//If there are unowned or enemy tiles, this is a nice 'frontier' position.
	if( (iAdjacentUnowned > 2) || (iAdjacentOwned > 0) )
	{
		//check the wider area for enemy tiles. may also be on another landmass
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				CvPlot* pLoopNearbyPlot = plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);

				//Don't want them adjacent to cities, but we do want to check for plot ownership.
				if (pLoopNearbyPlot != NULL && pLoopNearbyPlot->isRevealed(eTeam) && (this != pLoopNearbyPlot))
				{
					if((pLoopNearbyPlot->getOwner() != getOwner()) && (pLoopNearbyPlot->getOwner() != NO_PLAYER) && !(GET_PLAYER(pLoopNearbyPlot->getOwner()).isMinorCiv()))
					{
						iNearbyOwned++;
					}
				}
			}
		}

		//only build a fort if it's somewhat close to the enemy
		if (iNearbyOwned==0)
			return 0;

		// Get score for this sentry point (defense and danger)
		int iScore = GET_PLAYER(getOwner()).GetPlotDanger(*this);

		iScore += defenseModifier(eTeam, true);
		iScore += iAdjacentUnowned;
		//Reduction if forts nearby.
		iScore -= iAdjacentForts*20;
		//Big Bonus if adjacent to enemy territory.
		iScore += iNearbyOwned;
		iScore += (iAdjacentOwned*10);

		//Big bonus if chokepoint
		if(IsChokePoint())
			iScore += 50;

		return iScore;
	}
	return 0;
}
#endif
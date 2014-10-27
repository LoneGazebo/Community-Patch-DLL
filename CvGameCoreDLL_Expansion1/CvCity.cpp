/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvCity.h"
#include "CvArea.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvUnit.h"
#include "CvInfos.h"
#include "CvRandom.h"
#include "CvImprovementClasses.h"
#include "CvCitySpecializationAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvNotifications.h"
#include "CvUnitCombat.h"
#include "CvTypes.h"

// interfaces used
#include "CvEnumSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvWonderProductionAI.h"

#include "CvDllCity.h"
#include "CvDllCombatInfo.h"
#include "CvDllPlot.h"
#include "CvDllUnit.h"
#include "CvGameQueries.h"

#include "CvInfosSerializationHelper.h"
// include after all other headers
#include "LintFree.h"

OBJECT_VALIDATE_DEFINITION(CvCity)

namespace
{
// debugging
YieldTypes s_lastYieldUsedToUpdateRateFromTerrain;
int        s_changeYieldFromTerreain;
}

//	--------------------------------------------------------------------------------
namespace FSerialization
{
std::set<CvCity*> citiesToCheck;
void SyncCities()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes authoritativePlayer = GC.getGame().getActivePlayer();

		std::set<CvCity*>::const_iterator i;
		for(i = citiesToCheck.begin(); i != citiesToCheck.end(); ++i)
		{
			const CvCity* city = *i;

			if(city)
			{
				const CvPlayer& player = GET_PLAYER(city->getOwner());
				if(city->getOwner() == authoritativePlayer || (gDLL->IsHost() && !player.isHuman() && player.isAlive()))
				{
					const FAutoArchive& archive = city->getSyncArchive();
					if(archive.hasDeltas())
					{
						FMemoryStream memoryStream;
						std::vector<std::pair<std::string, std::string> > callStacks;
						archive.saveDelta(memoryStream, callStacks);
						gDLL->sendCitySyncCheck(city->getOwner(), city->GetID(), memoryStream, callStacks);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
// clears ALL deltas for ALL units
void ClearCityDeltas()
{
	std::set<CvCity*>::iterator i;
	for(i = citiesToCheck.begin(); i != citiesToCheck.end(); ++i)
	{
		CvCity* city = *i;

		if(city)
		{
			FAutoArchive& archive = city->getSyncArchive();
			archive.clearDelta();
		}
	}
}
}


//	--------------------------------------------------------------------------------
// Public Functions...
CvCity::CvCity() :
	m_syncArchive(*this)
	, m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility("CvCity::m_strName", m_syncArchive, "")
	, m_eOwner("CvCity::m_eOwner", m_syncArchive, NO_PLAYER)
	, m_iX("CvCity::m_iX", m_syncArchive)
	, m_iY("CvCity::m_iY", m_syncArchive)
	, m_iID("CvCity::m_iID", m_syncArchive)
	, m_iRallyX("CvCity::m_iRallyX", m_syncArchive)
	, m_iRallyY("CvCity::m_iRallyY", m_syncArchive)
	, m_iGameTurnFounded("CvCity::m_iGameTurnFounded", m_syncArchive)
	, m_iGameTurnAcquired("CvCity::m_iGameTurnAcquired", m_syncArchive)
	, m_iGameTurnLastExpanded("CvCity::m_iGameTurnLastExpanded", m_syncArchive)
	, m_iPopulation("CvCity::m_iPopulation", m_syncArchive)
	, m_iHighestPopulation("CvCity::m_iHighestPopulation", m_syncArchive)
	, m_iExtraHitPoints(0)
	, m_iNumGreatPeople("CvCity::m_iNumGreatPeople", m_syncArchive)
	, m_iBaseGreatPeopleRate("CvCity::m_iBaseGreatPeopleRate", m_syncArchive)
	, m_iGreatPeopleRateModifier("CvCity::m_iGreatPeopleRateModifier", m_syncArchive)
	, m_iGreatPeopleProgress("CvCity::m_iGreatPeopleProgress", m_syncArchive)
	, m_iJONSCultureStored("CvCity::m_iJONSCultureStored", m_syncArchive, true)
	, m_iJONSCultureLevel("CvCity::m_iJONSCultureLevel", m_syncArchive)
	, m_iJONSCulturePerTurnFromBuildings("CvCity::m_iJONSCulturePerTurnFromBuildings", m_syncArchive)
	, m_iJONSCulturePerTurnFromPolicies("CvCity::m_iJONSCulturePerTurnFromPolicies", m_syncArchive)
	, m_iJONSCulturePerTurnFromSpecialists("CvCity::m_iJONSCulturePerTurnFromSpecialists", m_syncArchive)
	, m_iJONSCulturePerTurnFromReligion("CvCity::m_iJONSCulturePerTurnFromReligion", m_syncArchive)
	, m_iFaithPerTurnFromBuildings(0)
	, m_iFaithPerTurnFromPolicies(0)
	, m_iFaithPerTurnFromReligion(0)
	, m_iCultureRateModifier("CvCity::m_iCultureRateModifier", m_syncArchive)
	, m_iNumWorldWonders("CvCity::m_iNumWorldWonders", m_syncArchive)
	, m_iNumTeamWonders("CvCity::m_iNumTeamWonders", m_syncArchive)
	, m_iNumNationalWonders("CvCity::m_iNumNationalWonders", m_syncArchive)
	, m_iWonderProductionModifier("CvCity::m_iWonderProductionModifier", m_syncArchive)
	, m_iCapturePlunderModifier("CvCity::m_iCapturePlunderModifier", m_syncArchive)
	, m_iPlotCultureCostModifier("CvCity::m_iPlotCultureCostModifier", m_syncArchive)
	, m_iPlotBuyCostModifier(0)
	, m_iMaintenance("CvCity::m_iMaintenance", m_syncArchive)
	, m_iHealRate("CvCity::m_iHealRate", m_syncArchive)
	, m_iNoOccupiedUnhappinessCount("CvCity::m_iNoOccupiedUnhappinessCount", m_syncArchive)
	, m_iFood("CvCity::m_iFood", m_syncArchive)
	, m_iFoodKept("CvCity::m_iFoodKept", m_syncArchive)
	, m_iMaxFoodKeptPercent("CvCity::m_iMaxFoodKeptPercent", m_syncArchive)
	, m_iOverflowProduction("CvCity::m_iOverflowProduction", m_syncArchive)
	, m_iFeatureProduction("CvCity::m_iFeatureProduction", m_syncArchive)
	, m_iMilitaryProductionModifier("CvCity::m_iMilitaryProductionModifier", m_syncArchive)
	, m_iSpaceProductionModifier("CvCity::m_iSpaceProductionModifier", m_syncArchive)
	, m_iFreeExperience("CvCity::m_iFreeExperience", m_syncArchive)
	, m_iCurrAirlift("CvCity::m_iCurrAirlift", m_syncArchive)
	, m_iMaxAirlift("CvCity::m_iMaxAirlift", m_syncArchive)
	, m_iAirModifier("CvCity::m_iAirModifier", m_syncArchive)
	, m_iNukeModifier("CvCity::m_iNukeModifier", m_syncArchive)
	, m_iCultureUpdateTimer("CvCity::m_iCultureUpdateTimer", m_syncArchive)	// unused
	, m_iCitySizeBoost("CvCity::m_iCitySizeBoost", m_syncArchive)
	, m_iSpecialistFreeExperience("CvCity::m_iSpecialistFreeExperience", m_syncArchive)
	, m_iStrengthValue("CvCity::m_iStrengthValue", m_syncArchive, true)
	, m_iDamage("CvCity::m_iDamage", m_syncArchive)
	, m_iThreatValue("CvCity::m_iThreatValue", m_syncArchive, true)
	, m_iGarrisonedUnit("CvCity::m_iGarrisonedUnit", m_syncArchive)   // unused
	, m_iResourceDemanded("CvCity::m_iResourceDemanded", m_syncArchive)
	, m_iWeLoveTheKingDayCounter("CvCity::m_iWeLoveTheKingDayCounter", m_syncArchive)
	, m_iLastTurnGarrisonAssigned("CvCity::m_iLastTurnGarrisonAssigned", m_syncArchive)
	, m_iThingsProduced("CvCity::m_iThingsProduced", m_syncArchive)
	, m_iDemandResourceCounter("CvCity::m_iDemandResourceCounter", m_syncArchive, true)
	, m_iResistanceTurns("CvCity::m_iResistanceTurns", m_syncArchive)
	, m_iRazingTurns("CvCity::m_iRazingTurns", m_syncArchive)
	, m_iCountExtraLuxuries("CvCity::m_iCountExtraLuxuries", m_syncArchive)
	, m_iCheapestPlotInfluence("CvCity::m_iCheapestPlotInfluence", m_syncArchive)
	, m_iEspionageModifier(0)
	, m_unitBeingBuiltForOperation()
	, m_bNeverLost("CvCity::m_bNeverLost", m_syncArchive)
	, m_bDrafted("CvCity::m_bDrafted", m_syncArchive)
	, m_bAirliftTargeted("CvCity::m_bAirliftTargeted", m_syncArchive)
	, m_bProductionAutomated("CvCity::m_bProductionAutomated", m_syncArchive)
	, m_bLayoutDirty("CvCity::m_bLayoutDirty", m_syncArchive)
	, m_bMadeAttack("CvCity::m_bMadeAttack", m_syncArchive)
	, m_bOccupied("CvCity::m_bOccupied", m_syncArchive)
	, m_bPuppet("CvCity::m_bPuppet", m_syncArchive)
	, m_bEverCapital("CvCity::m_bEverCapital", m_syncArchive)
	, m_bIndustrialRouteToCapital("CvCity::m_bIndustrialRouteToCapital", m_syncArchive)
	, m_bFeatureSurrounded("CvCity::m_bFeatureSurrounded", m_syncArchive)
	, m_ePreviousOwner("CvCity::m_ePreviousOwner", m_syncArchive)
	, m_eOriginalOwner("CvCity::m_eOriginalOwner", m_syncArchive)
	, m_ePlayersReligion("CvCity::m_ePlayersReligion", m_syncArchive)
	, m_aiSeaPlotYield("CvCity::m_aiSeaPlotYield", m_syncArchive)
	, m_aiRiverPlotYield("CvCity::m_aiRiverPlotYield", m_syncArchive)
	, m_aiLakePlotYield("CvCity::m_aiLakePlotYield", m_syncArchive)
	, m_aiSeaResourceYield("CvCity::m_aiSeaResourceYield", m_syncArchive)
	, m_aiBaseYieldRateFromTerrain("CvCity::m_aiBaseYieldRateFromTerrain", m_syncArchive, true)
	, m_aiBaseYieldRateFromBuildings("CvCity::m_aiBaseYieldRateFromBuildings", m_syncArchive)
	, m_aiBaseYieldRateFromSpecialists("CvCity::m_aiBaseYieldRateFromSpecialists", m_syncArchive)
	, m_aiBaseYieldRateFromMisc("CvCity::m_aiBaseYieldRateFromMisc", m_syncArchive)
	, m_aiYieldRateModifier("CvCity::m_aiYieldRateModifier", m_syncArchive)
	, m_aiYieldPerPop("CvCity::m_aiYieldPerPop", m_syncArchive)
	, m_aiPowerYieldRateModifier("CvCity::m_aiPowerYieldRateModifier", m_syncArchive)
	, m_aiResourceYieldRateModifier("CvCity::m_aiResourceYieldRateModifier", m_syncArchive)
	, m_aiExtraSpecialistYield("CvCity::m_aiExtraSpecialistYield", m_syncArchive)
	, m_aiProductionToYieldModifier("CvCity::m_aiProductionToYieldModifier", m_syncArchive)
	, m_aiDomainFreeExperience("CvCity::m_aiDomainFreeExperience", m_syncArchive)
	, m_aiDomainProductionModifier("CvCity::m_aiDomainProductionModifier", m_syncArchive)
	, m_abEverOwned("CvCity::m_abEverOwned", m_syncArchive)
	, m_abRevealed("CvCity::m_abRevealed", m_syncArchive, true)
	, m_strScriptData("CvCity::m_strScriptData", m_syncArchive)
	, m_paiNoResource("CvCity::m_paiNoResource", m_syncArchive)
	, m_paiFreeResource("CvCity::m_paiFreeResource", m_syncArchive)
	, m_paiNumResourcesLocal("CvCity::m_paiNumResourcesLocal", m_syncArchive)
	, m_paiProjectProduction("CvCity::m_paiProjectProduction", m_syncArchive)
	, m_paiSpecialistProduction("CvCity::m_paiSpecialistProduction", m_syncArchive)
	, m_paiUnitProduction("CvCity::m_paiUnitProduction", m_syncArchive)
	, m_paiUnitProductionTime("CvCity::m_paiUnitProductionTime", m_syncArchive)
	, m_paiGreatPeopleUnitRate("CvCity::m_paiGreatPeopleUnitRate", m_syncArchive)
	, m_paiGreatPeopleUnitProgress("CvCity::m_paiGreatPeopleUnitProgress", m_syncArchive)
	, m_paiSpecialistCount("CvCity::m_paiSpecialistCount", m_syncArchive)
	, m_paiMaxSpecialistCount("CvCity::m_paiMaxSpecialistCount", m_syncArchive)
	, m_paiForceSpecialistCount("CvCity::m_paiForceSpecialistCount", m_syncArchive)
	, m_paiFreeSpecialistCount("CvCity::m_paiFreeSpecialistCount", m_syncArchive)
	, m_paiImprovementFreeSpecialists("CvCity::m_paiImprovementFreeSpecialists", m_syncArchive)
	, m_paiUnitCombatFreeExperience("CvCity::m_paiUnitCombatFreeExperience", m_syncArchive)
	, m_paiUnitCombatProductionModifier("CvCity::m_paiUnitCombatProductionModifier", m_syncArchive)
	, m_paiFreePromotionCount("CvCity::m_paiFreePromotionCount", m_syncArchive)
	, m_iBaseHappinessFromBuildings(0)
	, m_iUnmoddedHappinessFromBuildings(0)
	, m_bRouteToCapitalConnectedLastTurn(false)
	, m_bRouteToCapitalConnectedThisTurn(false)
	, m_strName("")
	, m_orderQueue()
	, m_aaiBuildingSpecialistUpgradeProgresses(0)
	, m_ppaiResourceYieldChange(0)
	, m_ppaiFeatureYieldChange(0)
	, m_ppaiTerrainYieldChange(0)
	, m_pCityBuildings(FNEW(CvCityBuildings, c_eCiv5GameplayDLL, 0))
	, m_pCityStrategyAI(FNEW(CvCityStrategyAI, c_eCiv5GameplayDLL, 0))
	, m_pCityCitizens(FNEW(CvCityCitizens, c_eCiv5GameplayDLL, 0))
	, m_pCityReligions(FNEW(CvCityReligions, c_eCiv5GameplayDLL, 0))
	, m_pEmphases(FNEW(CvCityEmphases, c_eCiv5GameplayDLL, 0))
	, m_pCityEspionage(FNEW(CvCityEspionage, c_eCiv5GameplayDLL, 0))
	, m_bombardCheckTurn(0)
	, m_iPopulationRank(0)
	, m_bPopulationRankValid(false)
	, m_aiBaseYieldRank("CvCity::m_aiBaseYieldRank", m_syncArchive)
	, m_abBaseYieldRankValid("CvCity::m_abBaseYieldRankValid", m_syncArchive)
	, m_aiYieldRank("CvCity::m_aiYieldRank", m_syncArchive)
	, m_abYieldRankValid("CvCity::m_abYieldRankValid", m_syncArchive)
	, m_bOwedCultureBuilding(false)
{
	OBJECT_ALLOCATED
	FSerialization::citiesToCheck.insert(this);

	reset(0, NO_PLAYER, 0, 0, true);
}

//	--------------------------------------------------------------------------------
CvCity::~CvCity()
{
	FSerialization::citiesToCheck.erase(this);

	uninit();

	delete m_pCityBuildings;
	delete m_pCityStrategyAI;
	delete m_pCityCitizens;
	delete m_pCityReligions;
	delete m_pEmphases;
	delete m_pCityEspionage;

	OBJECT_DESTROYED
}


//	--------------------------------------------------------------------------------
void CvCity::init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits)
{
	VALIDATE_OBJECT
	CvPlot* pAdjacentPlot;
	CvPlot* pPlot;
	BuildingTypes eLoopBuilding;
	int iI;

	pPlot = GC.getMap().plot(iX, iY);

	//--------------------------------
	// Init saved data
	reset(iID, eOwner, pPlot->getX(), pPlot->getY());

	CvPlayerAI& owningPlayer = GET_PLAYER(getOwner());

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	CvString strNewCityName = owningPlayer.getNewCityName();
	setName(strNewCityName.c_str());

	if(strcmp(strNewCityName.c_str(), "TXT_KEY_CITY_NAME_LLANFAIRPWLLGWYNGYLL") == 0)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_XP1_34);
	}

	// Plot Ownership
	setEverOwned(getOwner(), true);

	pPlot->setOwner(getOwner(), m_iID, bBumpUnits);
	// Clear the improvement before the city attaches itself to the plot, else the improvement does not
	// remove the resource allocation from the current owner.  This would result in double resource points because
	// the plot has already had setOwner called on it (above), giving the player the resource points.
	pPlot->setImprovementType(NO_IMPROVEMENT);
	pPlot->setPlotCity(this);
	pPlot->SetCityPurchaseID(m_iID);

	for(iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getOwner() == NO_PLAYER)
			{
				pAdjacentPlot->setOwner(getOwner(), m_iID, bBumpUnits);
			}
			if(pAdjacentPlot->getOwner() == getOwner())
			{
				pAdjacentPlot->SetCityPurchaseID(m_iID);
			}
		}
	}

	// this is a list of plot that are owned by the player
	owningPlayer.UpdatePlots();

	//SCRIPT call ' bool citiesDestroyFeatures(iX, iY);'
	if(pPlot->getFeatureType() != NO_FEATURE)
	{
		pPlot->setFeatureType(NO_FEATURE);
	}

	setupGraphical();

	pPlot->updateCityRoute();

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			if(pPlot->isVisible(((TeamTypes)iI)))
			{
				setRevealed(((TeamTypes)iI), true);
			}
		}
	}

	int iNumBuildingInfos = GC.getNumBuildingInfos();
	for(iI = 0; iI < iNumBuildingInfos; iI++)
	{
		if(owningPlayer.isBuildingFree((BuildingTypes)iI))
		{
			if(isValidBuildingLocation((BuildingTypes)iI))
			{
				m_pCityBuildings->SetNumFreeBuilding(((BuildingTypes)iI), 1);
			}
		}
	}

	area()->changeCitiesPerPlayer(getOwner(), 1);

	GET_TEAM(getTeam()).changeNumCities(1);

	GC.getGame().changeNumCities(1);

	int iGameTurn = GC.getGame().getGameTurn();
	setGameTurnFounded(iGameTurn);
	setGameTurnAcquired(iGameTurn);
	setGameTurnLastExpanded(iGameTurn);

	GC.getMap().updateWorkingCity(pPlot,NUM_CITY_RINGS*2);
	GetCityCitizens()->DoFoundCity();

	// Default starting population
	changePopulation(GC.getINITIAL_CITY_POPULATION() + GC.getGame().getStartEraInfo().getFreePopulation());
	// Free population from things (e.g. Policies)
	changePopulation(GET_PLAYER(getOwner()).GetNewCityExtraPopulation());

	// Free food from things (e.g. Policies)
	int iFreeFood = growthThreshold() * GET_PLAYER(getOwner()).GetFreeFoodBox();
	changeFoodTimes100(iFreeFood);

	owningPlayer.setFoundedFirstCity(true);
	owningPlayer.ChangeNumCitiesFounded(1);

	// Free resources under city?
	for(int i = 0; i < GC.getNumResourceInfos(); i++)
	{
		ResourceTypes eResource = (ResourceTypes)i;
		FreeResourceXCities freeResource = owningPlayer.GetPlayerTraits()->GetFreeResourceXCities(eResource);

		if(freeResource.m_iResourceQuantity > 0)
		{
			if(owningPlayer.GetNumCitiesFounded() <= freeResource.m_iNumCities)
			{
				plot()->setResourceType(NO_RESOURCE, 0);
				plot()->setResourceType(eResource, freeResource.m_iResourceQuantity);
			}
		}
	}
	if(owningPlayer.isMinorCiv())
	{
		owningPlayer.GetMinorCivAI()->DoAddStartingResources(plot());
	}

	// make sure that all the team members get the city connection update
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;
		if(GET_PLAYER(ePlayer).getTeam() == owningPlayer.getTeam())
		{
			GET_PLAYER(ePlayer).GetCityConnections()->Update();
		}
	}
	owningPlayer.DoUpdateHappiness();

	// Policy changes
	PolicyTypes ePolicy;
	for(int iPoliciesLoop = 0; iPoliciesLoop < GC.getNumPolicyInfos(); iPoliciesLoop++)
	{
		ePolicy = (PolicyTypes) iPoliciesLoop;

		if(owningPlayer.GetPlayerPolicies()->HasPolicy(ePolicy) && !owningPlayer.GetPlayerPolicies()->IsPolicyBlocked(ePolicy))
		{
			// Free Culture-per-turn in every City from Policies
			ChangeJONSCulturePerTurnFromPolicies(GC.getPolicyInfo(ePolicy)->GetCulturePerCity());
		}
	}

	// Add Resource Quantity to total
	if(plot()->getResourceType() != NO_RESOURCE)
	{
		if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(plot()->getResourceType())->getTechCityTrade()))
		{
			owningPlayer.changeNumResourceTotal(plot()->getResourceType(), plot()->getNumResourceForPlayer(getOwner()));
		}
	}

	CvPlot* pLoopPlot;

	// We may need to link Resources to this City if it's constructed within previous borders and the Resources were too far away for another City to link to
	for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
	{
		pLoopPlot = plotCity(getX(), getY(), iJ);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == getOwner())
			{
				if(pLoopPlot->getResourceType() != NO_RESOURCE)
				{
					// Is this Resource as of yet unlinked?
					if(pLoopPlot->GetResourceLinkedCity() == NULL)
					{
						pLoopPlot->DoFindCityToLinkResourceTo();
					}
				}
			}
		}
	}

	PlayerTypes ePlayer;

	// Update Proximity between this Player and all others
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(ePlayer != getOwner())
		{
			if(GET_PLAYER(ePlayer).isAlive())
			{
				// Players do NOT have to know one another in order to calculate proximity.  Having this info available (even whey they haven't met) can be useful
				owningPlayer.DoUpdateProximityToPlayer(ePlayer);
				GET_PLAYER(ePlayer).DoUpdateProximityToPlayer(getOwner());
			}
		}
	}

	// Free Buildings in the first City
	CvCivilizationInfo& thisCiv = getCivilizationInfo();
	if(GC.getGame().isFinalInitialized())
	{
		if(owningPlayer.getNumCities() == 1)
		{
			for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
				if(!pkBuildingClassInfo)
				{
					continue;
				}

				if(thisCiv.isCivilizationFreeBuildingClass(iI))
				{
					eLoopBuilding = ((BuildingTypes)(thisCiv.getCivilizationBuildings(iI)));

					if(eLoopBuilding != NO_BUILDING)
					{
						m_pCityBuildings->SetNumRealBuilding(eLoopBuilding, true);
					}
				}
			}

			if(!isHuman())
			{
				changeOverflowProduction(GC.getINITIAL_AI_CITY_PRODUCTION());
			}
		}
	}

	// How long before this City picks a Resource to demand?
	DoSeedResourceDemandedCountdown();

	// Update City Strength
	updateStrengthValue();

	// Update closest plot's influence cost
	DoUpdateCheapestPlotInfluence();

	// Update Unit Maintenance for the player
	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	kPlayer.UpdateUnitProductionMaintenanceMod();

	// Spread a pantheon here if one is active
	CvPlayerReligions* pReligions = kPlayer.GetReligions();
	if(pReligions->HasCreatedPantheon() && !pReligions->HasCreatedReligion())
	{
		GetCityReligions()->AddReligiousPressure(FOLLOWER_CHANGE_PANTHEON_FOUNDED, RELIGION_PANTHEON, GC.getRELIGION_ATHEISM_PRESSURE_PER_POP() * getPopulation() * 2);
	}

	// A new City might change our victory progress
	GET_TEAM(getTeam()).DoTestSmallAwards();

	DLLUI->setDirty(NationalBorders_DIRTY_BIT, true);

	// Garrisoned?
	if (GetGarrisonedUnit())
	{
		ChangeJONSCulturePerTurnFromPolicies(GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_GARRISON));
	}

	AI_init();

	if (GC.getGame().getGameTurn() == 0)
	{
		chooseProduction();
	}
}


//	--------------------------------------------------------------------------------
void CvCity::uninit()
{
	VALIDATE_OBJECT

	if(m_aaiBuildingSpecialistUpgradeProgresses)
	{
		for(int i=0; i < GC.getNumBuildingInfos(); i++)
		{
			SAFE_DELETE_ARRAY(m_aaiBuildingSpecialistUpgradeProgresses[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_aaiBuildingSpecialistUpgradeProgresses);

	if(m_ppaiResourceYieldChange)
	{
		for(int i=0; i < GC.getNumResourceInfos(); i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiResourceYieldChange[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_ppaiResourceYieldChange);

	if(m_ppaiFeatureYieldChange)
	{
		for(int i=0; i < GC.getNumFeatureInfos(); i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiFeatureYieldChange[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_ppaiFeatureYieldChange);

	if(m_ppaiTerrainYieldChange)
	{
		for(int i=0; i < GC.getNumTerrainInfos(); i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiTerrainYieldChange[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_ppaiTerrainYieldChange);

	m_pCityBuildings->Uninit();
	m_pCityStrategyAI->Uninit();
	m_pCityCitizens->Uninit();
	m_pCityReligions->Uninit();
	m_pEmphases->Uninit();
	m_pCityEspionage->Uninit();

	m_orderQueue.clear();
}

//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvCity::reset(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall)
{
	VALIDATE_OBJECT
	m_syncArchive.reset();

	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iX = iX;
	m_iY = iY;
	m_iRallyX = INVALID_PLOT_COORD;
	m_iRallyY = INVALID_PLOT_COORD;
	m_iGameTurnFounded = 0;
	m_iGameTurnAcquired = 0;
	m_iPopulation = 0;
	m_iHighestPopulation = 0;
	m_iExtraHitPoints = 0;
	m_iNumGreatPeople = 0;
	m_iBaseGreatPeopleRate = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatPeopleProgress = 0;
	m_iJONSCultureStored = 0;
	m_iJONSCultureLevel = 0;
	m_iJONSCulturePerTurnFromBuildings = 0;
	m_iJONSCulturePerTurnFromPolicies = 0;
	m_iJONSCulturePerTurnFromSpecialists = 0;
	m_iJONSCulturePerTurnFromReligion = 0;
	m_iFaithPerTurnFromBuildings = 0;
	m_iFaithPerTurnFromPolicies = 0;
	m_iFaithPerTurnFromReligion = 0;
	m_iCultureRateModifier = 0;
	m_iNumWorldWonders = 0;
	m_iNumTeamWonders = 0;
	m_iNumNationalWonders = 0;
	m_iWonderProductionModifier = 0;
	m_iCapturePlunderModifier = 0;
	m_iPlotCultureCostModifier = 0;
	m_iPlotBuyCostModifier = 0;
	m_iMaintenance = 0;
	m_iHealRate = 0;
	m_iEspionageModifier = 0;
	m_iNoOccupiedUnhappinessCount = 0;
	m_iFood = 0;
	m_iFoodKept = 0;
	m_iMaxFoodKeptPercent = 0;
	m_iOverflowProduction = 0;
	m_iFeatureProduction = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iFreeExperience = 0;
	m_iCurrAirlift = 0;
	m_iMaxAirlift = 0;
	m_iAirModifier = 0;
	m_iNukeModifier = 0;
	m_iCultureUpdateTimer = 0;
	m_iCitySizeBoost = 0;
	m_iSpecialistFreeExperience = 0;
	m_iStrengthValue = 0;
	m_iDamage = 0;
	m_iThreatValue = 0;
	m_iGarrisonedUnit = -1;    // unused
	m_iResourceDemanded = -1;
	m_iWeLoveTheKingDayCounter = 0;
	m_iLastTurnGarrisonAssigned = -1;
	m_iThingsProduced = 0;
	m_iDemandResourceCounter = 0;
	m_iResistanceTurns = 0;
	m_iRazingTurns = 0;
	m_iCountExtraLuxuries = 0;
	m_iCheapestPlotInfluence = 0;
	m_unitBeingBuiltForOperation.Invalidate();

	m_bNeverLost = true;
	m_bDrafted = false;
	m_bAirliftTargeted = false;
	m_bProductionAutomated = false;
	m_bLayoutDirty = false;
	m_bMadeAttack = false;
	m_bOccupied = false;
	m_bPuppet = false;
	m_bIgnoreCityForHappiness = false;
	m_bEverCapital = false;
	m_bIndustrialRouteToCapital = false;
	m_bFeatureSurrounded = false;
	m_bOwedCultureBuilding = false;

	m_eOwner = eOwner;
	m_ePreviousOwner = NO_PLAYER;
	m_eOriginalOwner = eOwner;
	m_ePlayersReligion = NO_PLAYER;


	m_aiSeaPlotYield.resize(NUM_YIELD_TYPES);
	m_aiRiverPlotYield.resize(NUM_YIELD_TYPES);
	m_aiSeaResourceYield.resize(NUM_YIELD_TYPES);
	m_aiLakePlotYield.resize(NUM_YIELD_TYPES);
	m_aiBaseYieldRateFromTerrain.resize(NUM_YIELD_TYPES);
	m_aiBaseYieldRateFromBuildings.resize(NUM_YIELD_TYPES);
	m_aiBaseYieldRateFromSpecialists.resize(NUM_YIELD_TYPES);
	m_aiBaseYieldRateFromMisc.resize(NUM_YIELD_TYPES);
	m_aiBaseYieldRateFromReligion.resize(NUM_YIELD_TYPES);
	m_aiYieldPerPop.resize(NUM_YIELD_TYPES);
	m_aiYieldRateModifier.resize(NUM_YIELD_TYPES);
	m_aiPowerYieldRateModifier.resize(NUM_YIELD_TYPES);
	m_aiResourceYieldRateModifier.resize(NUM_YIELD_TYPES);
	m_aiExtraSpecialistYield.resize(NUM_YIELD_TYPES);
	m_aiProductionToYieldModifier.resize(NUM_YIELD_TYPES);
	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield.setAt(iI, 0);
		m_aiRiverPlotYield.setAt(iI, 0);
		m_aiLakePlotYield.setAt(iI, 0);
		m_aiSeaResourceYield.setAt(iI, 0);
		m_aiBaseYieldRateFromTerrain.setAt(iI, 0);
		m_aiBaseYieldRateFromBuildings.setAt(iI, 0);
		m_aiBaseYieldRateFromSpecialists.setAt(iI, 0);
		m_aiBaseYieldRateFromMisc.setAt(iI, 0);
		m_aiBaseYieldRateFromReligion[iI] = 0;
		m_aiYieldPerPop.setAt(iI, 0);
		m_aiYieldRateModifier.setAt(iI, 0);
		m_aiPowerYieldRateModifier.setAt(iI, 0);
		m_aiResourceYieldRateModifier.setAt(iI, 0);
		m_aiExtraSpecialistYield.setAt(iI, 0);
		m_aiProductionToYieldModifier.setAt(iI, 0);
	}

	m_aiDomainFreeExperience.resize(NUM_DOMAIN_TYPES);
	m_aiDomainProductionModifier.resize(NUM_DOMAIN_TYPES);
	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainFreeExperience.setAt(iI, 0);
		m_aiDomainProductionModifier.setAt(iI, 0);
	}

	m_abEverOwned.resize(REALLY_MAX_PLAYERS);
	for(iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
	{
		m_abEverOwned.setAt(iI, false);
	}

	m_abRevealed.resize(REALLY_MAX_TEAMS);
	for(iI = 0; iI < REALLY_MAX_TEAMS; iI++)
	{
		m_abRevealed.setAt(iI, false);
	}

	m_strName = "";
	m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility = "";
	m_strScriptData = "";

	m_bPopulationRankValid = false;
	m_iPopulationRank = -1;

	m_abBaseYieldRankValid.resize(NUM_YIELD_TYPES);
	m_abYieldRankValid.resize(NUM_YIELD_TYPES);
	m_aiBaseYieldRank.resize(NUM_YIELD_TYPES);
	m_aiYieldRank.resize(NUM_YIELD_TYPES);
	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_abBaseYieldRankValid.setAt(iI, false);
		m_abYieldRankValid.setAt(iI, false);
		m_aiBaseYieldRank.setAt(iI, -1);
		m_aiYieldRank.setAt(iI, -1);
	}

	if(!bConstructorCall)
	{
		int iNumResources = GC.getNumResourceInfos();
		CvAssertMsg((0 < iNumResources),  "GC.getNumResourceInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiNoResource.clear();
		m_paiNoResource.resize(iNumResources);
		m_paiFreeResource.clear();
		m_paiFreeResource.resize(iNumResources);
		m_paiNumResourcesLocal.clear();
		m_paiNumResourcesLocal.resize(iNumResources);
		for(iI = 0; iI < iNumResources; iI++)
		{
			m_paiNoResource.setAt(iI, 0);
			m_paiFreeResource.setAt(iI, 0);
			m_paiNumResourcesLocal.setAt(iI, 0);
		}

		int iNumProjectInfos = GC.getNumProjectInfos();
		m_paiProjectProduction.clear();
		m_paiProjectProduction.resize(iNumProjectInfos);
		for(iI = 0; iI < iNumProjectInfos; iI++)
		{
			m_paiProjectProduction.setAt(iI, 0);
		}

		int iNumSpecialistInfos = GC.getNumSpecialistInfos();
		m_paiSpecialistProduction.clear();
		m_paiSpecialistProduction.resize(iNumSpecialistInfos);
		for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiSpecialistProduction.setAt(iI, 0);
		}

		m_pCityBuildings->Init(GC.GetGameBuildings(), this);

		int iNumUnitInfos = GC.getNumUnitInfos();
		CvAssertMsg((0 < iNumUnitInfos),  "GC.getNumUnitInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitProduction.clear();
		m_paiUnitProduction.resize(iNumUnitInfos);
		m_paiUnitProductionTime.clear();
		m_paiUnitProductionTime.resize(iNumUnitInfos);
		m_paiGreatPeopleUnitRate.clear();
		m_paiGreatPeopleUnitRate.resize(iNumUnitInfos);
		m_paiGreatPeopleUnitProgress.clear();
		m_paiGreatPeopleUnitProgress.resize(iNumUnitInfos);
		for(iI = 0; iI < iNumUnitInfos; iI++)
		{
			m_paiUnitProduction.setAt(iI, 0);
			m_paiUnitProductionTime.setAt(iI, 0);
			m_paiGreatPeopleUnitRate.setAt(iI, 0);
			m_paiGreatPeopleUnitProgress.setAt(iI, 0);
		}

		CvAssertMsg((0 < iNumSpecialistInfos),  "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiSpecialistCount.clear();
		m_paiSpecialistCount.resize(iNumSpecialistInfos);
		m_paiMaxSpecialistCount.clear();
		m_paiMaxSpecialistCount.resize(iNumSpecialistInfos);
		m_paiForceSpecialistCount.clear();
		m_paiForceSpecialistCount.resize(iNumSpecialistInfos);
		m_paiFreeSpecialistCount.clear();
		m_paiFreeSpecialistCount.resize(iNumSpecialistInfos);

		for(iI = 0; iI < iNumSpecialistInfos; iI++)
		{
			m_paiSpecialistCount.setAt(iI, 0);
			m_paiMaxSpecialistCount.setAt(iI, 0);
			m_paiForceSpecialistCount.setAt(iI, 0);
			m_paiFreeSpecialistCount.setAt(iI, 0);
		}

		int iNumImprovementInfos = GC.getNumImprovementInfos();
		CvAssertMsg((0 < iNumImprovementInfos),  "GC.getNumImprovementInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiImprovementFreeSpecialists.clear();
		m_paiImprovementFreeSpecialists.resize(iNumImprovementInfos);
		for(iI = 0; iI < iNumImprovementInfos; iI++)
		{
			m_paiImprovementFreeSpecialists.setAt(iI, 0);
		}

		int iNumUnitCombatClassInfos = GC.getNumUnitCombatClassInfos();
		CvAssertMsg((0 < iNumUnitCombatClassInfos),  "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitCombatFreeExperience.clear();
		m_paiUnitCombatFreeExperience.resize(iNumUnitCombatClassInfos);
		m_paiUnitCombatProductionModifier.clear();
		m_paiUnitCombatProductionModifier.resize(iNumUnitCombatClassInfos);
		for(iI = 0; iI < iNumUnitCombatClassInfos; iI++)
		{
			m_paiUnitCombatFreeExperience.setAt(iI, 0);
			m_paiUnitCombatProductionModifier.setAt(iI, 0);
		}

		int iNumPromotionInfos = GC.getNumPromotionInfos();
		CvAssertMsg((0 < iNumPromotionInfos),  "GC.getNumPromotionInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiFreePromotionCount.clear();
		m_paiFreePromotionCount.resize(iNumPromotionInfos);
		for(iI = 0; iI < iNumPromotionInfos; iI++)
		{
			m_paiFreePromotionCount.setAt(iI, 0);
		}

		int iJ;

		int iNumBuildingInfos = GC.getNumBuildingInfos();
		int iMAX_SPECIALISTS_FROM_BUILDING = GC.getMAX_SPECIALISTS_FROM_BUILDING();
		CvAssertMsg(m_aaiBuildingSpecialistUpgradeProgresses==NULL, "about to leak memory, CvCity::m_aaiBuildingSpecialistUpgradeProgresses");
		m_aaiBuildingSpecialistUpgradeProgresses = FNEW(int*[iNumBuildingInfos], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < iNumBuildingInfos; iI++)
		{
			m_aaiBuildingSpecialistUpgradeProgresses[iI] = FNEW(int[iMAX_SPECIALISTS_FROM_BUILDING], c_eCiv5GameplayDLL, 0);
			for(iJ = 0; iJ < iMAX_SPECIALISTS_FROM_BUILDING; iJ++)
			{
				m_aaiBuildingSpecialistUpgradeProgresses[iI][iJ] = -1;
			}
		}

		int iNumResourceInfos = GC.getNumResourceInfos();
		CvAssertMsg(m_ppaiResourceYieldChange==NULL, "about to leak memory, CvCity::m_ppaiResourceYieldChange");
		m_ppaiResourceYieldChange = FNEW(int*[iNumResourceInfos], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < iNumResourceInfos; iI++)
		{
			m_ppaiResourceYieldChange[iI] = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
			for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaiResourceYieldChange[iI][iJ] = 0;
			}
		}

		int iNumFeatureInfos = GC.getNumFeatureInfos();
		CvAssertMsg(m_ppaiFeatureYieldChange==NULL, "about to leak memory, CvCity::m_ppaiFeatureYieldChange");
		m_ppaiFeatureYieldChange = FNEW(int*[iNumFeatureInfos], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < iNumFeatureInfos; iI++)
		{
			m_ppaiFeatureYieldChange[iI] = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
			for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaiFeatureYieldChange[iI][iJ] = 0;
			}
		}

		int iNumTerrainInfos = GC.getNumTerrainInfos();
		CvAssertMsg(m_ppaiTerrainYieldChange==NULL, "about to leak memory, CvCity::m_ppaiTerrainYieldChange");
		m_ppaiTerrainYieldChange = FNEW(int*[iNumTerrainInfos], c_eCiv5GameplayDLL, 0);
		for(iI = 0; iI < iNumTerrainInfos; iI++)
		{
			m_ppaiTerrainYieldChange[iI] = FNEW(int[NUM_YIELD_TYPES], c_eCiv5GameplayDLL, 0);
			for(iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaiTerrainYieldChange[iI][iJ] = 0;
			}
		}
	}

	if(!bConstructorCall)
	{
		// Set up AI and hook it up to the flavor manager
		m_pCityStrategyAI->Init(GC.GetGameAICityStrategies(), this, true);
		if(m_eOwner != NO_PLAYER)
		{
			GET_PLAYER(getOwner()).GetFlavorManager()->AddFlavorRecipient(m_pCityStrategyAI);
			m_pCityStrategyAI->SetDefaultSpecialization(GET_PLAYER(getOwner()).GetCitySpecializationAI()->GetNextSpecializationDesired());
		}

		m_pCityCitizens->Init(this);
		m_pCityReligions->Init(this);
		m_pEmphases->Init(GC.GetGameEmphases(), this);
		m_pCityEspionage->Init(this);

		AI_reset();
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvCity::setupGraphical()
{
	VALIDATE_OBJECT
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	CvPlayer& player = GET_PLAYER(getOwner());
	EraTypes eCurrentEra =(EraTypes) player.GetCurrentEra();

	auto_ptr<ICvCity1> pkDllCity(new CvDllCity(this));
	gDLL->GameplayCityCreated(pkDllCity.get(), eCurrentEra);
	gDLL->GameplayCitySetDamage(pkDllCity.get(), getDamage(), 0);

	// setup the wonders
	setupWonderGraphics();

	// setup any special buildings
	setupBuildingGraphics();

	// setup the spaceship
	setupSpaceshipGraphics();

	setLayoutDirty(true);
}

//	--------------------------------------------------------------------------------
void CvCity::setupWonderGraphics()
{
	VALIDATE_OBJECT
	PlayerTypes ePlayerID = getOwner();
	for(int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(eBuildingType);
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo(eBuilding);

		//Only work with valid buildings.
		if(buildingInfo == NULL)
			continue;

		// if this building exists
		int iExists = m_pCityBuildings->GetNumRealBuilding(eBuilding);
		int iPreferredPosition = buildingInfo->GetPreferredDisplayPosition();
		if(iPreferredPosition > 0)
		{
			if(iExists > 0)
			{
				// display the wonder
				auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(plot()));
				gDLL->GameplayWonderCreated(ePlayerID, pDllPlot.get(), eBuilding, 1);
			}
			else
			{
				// else if we are currently in the process of building this wonder
				if(getProductionBuilding() == eBuilding)
				{
					// display the half built wonder
					if(isWorldWonderClass(buildingInfo->GetBuildingClassInfo()))
					{
						auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(plot()));
						gDLL->GameplayWonderCreated(ePlayerID, pDllPlot.get(), eBuilding, 0);
					}
				}
			}

		}
	}
}

//	--------------------------------------------------------------------------------
void CvCity::setupBuildingGraphics()
{
	VALIDATE_OBJECT
	for(int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(eBuildingType);
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo(eBuilding);

		if(buildingInfo)
		{
			int iExists = m_pCityBuildings->GetNumBuilding(eBuilding);
			if(iExists > 0 && buildingInfo->IsCityWall())
			{
				auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(plot()));
				gDLL->GameplayWallCreated(pDllPlot.get());
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvCity::setupSpaceshipGraphics()
{
	VALIDATE_OBJECT

	CvTeam& thisTeam = GET_TEAM(getTeam());

	ProjectTypes ApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();

	int spaceshipState = 0;

	if(isCapital() && thisTeam.getProjectCount((ProjectTypes)ApolloProgram) == 1)
	{
		ProjectTypes capsuleID = (ProjectTypes) GC.getSPACESHIP_CAPSULE();
		ProjectTypes boosterID = (ProjectTypes) GC.getSPACESHIP_BOOSTER();
		ProjectTypes stasisID = (ProjectTypes) GC.getSPACESHIP_STASIS();
		ProjectTypes engineID = (ProjectTypes) GC.getSPACESHIP_ENGINE();

		enum eSpaceshipState
		{
		    eUnderConstruction	= 0x0000,
		    eFrame				= 0x0001,
		    eCapsule			= 0x0002,
		    eStasis_Chamber		= 0x0004,
		    eEngine				= 0x0008,
		    eBooster1			= 0x0010,
		    eBooster2			= 0x0020,
		    eBooster3			= 0x0040,
		    eConstructed		= 0x0080,
		};

		auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(plot()));
		gDLL->GameplaySpaceshipRemoved(pDllPlot.get());
		gDLL->GameplaySpaceshipCreated(pDllPlot.get(), eUnderConstruction + eFrame);

		spaceshipState = eFrame;

		if((thisTeam.getProjectCount((ProjectTypes)capsuleID)) == 1)
		{
			spaceshipState += eCapsule;
		}

		if((thisTeam.getProjectCount((ProjectTypes)stasisID)) == 1)
		{
			spaceshipState += eStasis_Chamber;
		}

		if((thisTeam.getProjectCount((ProjectTypes)engineID)) == 1)
		{
			spaceshipState += eEngine;
		}

		if((thisTeam.getProjectCount((ProjectTypes)boosterID)) >= 1)
		{
			spaceshipState += eBooster1;
		}

		if((thisTeam.getProjectCount((ProjectTypes)boosterID)) >= 2)
		{
			spaceshipState += eBooster2;
		}

		if((thisTeam.getProjectCount((ProjectTypes)boosterID)) == 3)
		{
			spaceshipState += eBooster3;
		}

		gDLL->GameplaySpaceshipEdited(pDllPlot.get(), spaceshipState);
	}
}

//	--------------------------------------------------------------------------------
void CvCity::PreKill()
{
	VALIDATE_OBJECT

	PlayerTypes eOwner;
	if(isCitySelected())
	{
		DLLUI->clearSelectedCities();
	}

	setPopulation(0);

	CvPlot* pPlot = plot();

	// Update resources linked to this city
	for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot;
		pLoopPlot = GetCityCitizens()->GetCityPlotFromIndex(iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getWorkingCityOverride() == this)
			{
				pLoopPlot->setWorkingCityOverride(NULL);
			}

			// Unlink Resources from this City
			if(pLoopPlot->getOwner() == getOwner())
			{
				if(pLoopPlot->getResourceType() != NO_RESOURCE)
				{
					if(pLoopPlot->GetResourceLinkedCity() == this)
					{
						pLoopPlot->SetResourceLinkedCity(NULL);
						pLoopPlot->DoFindCityToLinkResourceTo(this);
					}
				}
			}
		}
	}

	// If this city was built on a Resource, remove its Quantity from total
	if(plot()->getResourceType() != NO_RESOURCE)
	{
		if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(plot()->getResourceType())->getTechCityTrade()))
		{
			GET_PLAYER(getOwner()).changeNumResourceTotal(plot()->getResourceType(), -plot()->getNumResourceForPlayer(getOwner()));
		}
	}

	if(GET_PLAYER(getOwner()).isMinorCiv())
	{
		GET_PLAYER(getOwner()).GetMinorCivAI()->DoRemoveStartingResources(plot());
	}

	for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		CvBuildingEntry* pkBuilding = GC.getBuildingInfo((BuildingTypes)iI);
		if(pkBuilding)
		{
			m_pCityBuildings->SetNumRealBuilding(((BuildingTypes)iI), 0);
			m_pCityBuildings->SetNumFreeBuilding(((BuildingTypes)iI), 0);
		}
	}

	clearOrderQueue();

	// Killing a city while in combat is not something we really expect to happen.
	// It is *mostly* safe for it to happen, but combat systems must be able to gracefully handle the disapperance of a city.
	CvAssertMsg_Debug(!isFighting(), "isFighting did not return false as expected");

	clearCombat();

	// Could also be non-garrisoned units here that we need to show
	CvUnit* pLoopUnit;
	for(int iUnitLoop = 0; iUnitLoop < plot()->getNumUnits(); iUnitLoop++)
	{
		pLoopUnit = plot()->getUnitByIndex(iUnitLoop);

		// Only show units that belong to this city's owner - that way we don't show units on EVERY city capture (since the old city is deleted in this case)
		if(getOwner() == pLoopUnit->getOwner())
		{
			auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
			gDLL->GameplayUnitVisibility(pDllUnit.get(), !pLoopUnit->isInvisible(GC.getGame().getActiveTeam(),true) /*bVisible*/);
		}
	}

	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if(NULL != pLoopPlot && pLoopPlot->GetCityPurchaseOwner() == getOwner() && pLoopPlot->GetCityPurchaseID() == GetID())
		{
			pLoopPlot->ClearCityPurchaseInfo();
			pLoopPlot->setOwner(NO_PLAYER, NO_PLAYER, /*bCheckUnits*/ true, /*bUpdateResources*/ true);
		}
	}

	pPlot->setPlotCity(NULL);

	area()->changeCitiesPerPlayer(getOwner(), -1);

	GET_TEAM(getTeam()).changeNumCities(-1);

	GC.getGame().changeNumCities(-1);

	CvAssertMsg(getNumGreatPeople() == 0, "getNumGreatPeople is expected to be 0");
	CvAssertMsg(!isProduction(), "isProduction is expected to be false");

	eOwner = getOwner();

	GET_PLAYER(getOwner()).GetFlavorManager()->RemoveFlavorRecipient(m_pCityStrategyAI);

	if(m_unitBeingBuiltForOperation.IsValid())
	{
		GET_PLAYER(getOwner()).CityUncommitToBuildUnitForOperationSlot(m_unitBeingBuiltForOperation);
		m_unitBeingBuiltForOperation.Invalidate();
	}
}

//	--------------------------------------------------------------------------------
void CvCity::PostKill(bool bCapital, CvPlot* pPlot, PlayerTypes eOwner)
{
	VALIDATE_OBJECT

	CvPlayer& owningPlayer = GET_PLAYER(eOwner);

	// Recompute Happiness
	owningPlayer.DoUpdateHappiness();

	// Update Unit Maintenance for the player
	owningPlayer.UpdateUnitProductionMaintenanceMod();

	// Update Proximity between this Player and all others
	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(ePlayer != eOwner)
		{
			if(GET_PLAYER(ePlayer).isAlive())
			{
				owningPlayer.DoUpdateProximityToPlayer(ePlayer);
				GET_PLAYER(ePlayer).DoUpdateProximityToPlayer(eOwner);
			}
		}
	}

	GC.getMap().updateWorkingCity(pPlot,NUM_CITY_RINGS*2);

	if(bCapital)
	{
		owningPlayer.findNewCapital();
		owningPlayer.SetHasLostCapital(true, getOwner());
		GET_TEAM(owningPlayer.getTeam()).resetVictoryProgress();
	}

	pPlot->setImprovementType((ImprovementTypes)(GC.getRUINS_IMPROVEMENT()));

	if(eOwner == GC.getGame().getActivePlayer())
	{
		DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	DLLUI->setDirty(NationalBorders_DIRTY_BIT, true);

	if(GC.getGame().getActivePlayer() == eOwner)
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
	}

}

//	--------------------------------------------------------------------------------
void CvCity::kill()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();
	PlayerTypes eOwner = getOwner();
	bool bCapital = isCapital();

	IDInfo* pUnitNode;
	CvUnit* pLoopUnit;
	pUnitNode = pPlot->headUnitNode();

	FFastSmallFixedList<IDInfo, 25, true, c_eCiv5GameplayDLL > oldUnits;

	while(pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode);
		pUnitNode = pPlot->nextUnitNode((IDInfo*)pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = oldUnits.next(pUnitNode);

		if(pLoopUnit)
		{
			if(pLoopUnit->IsImmobile())
			{
				pLoopUnit->kill(false, eOwner);
			}
		}
	}

	PreKill();

	// get spies out of city
	CvCityEspionage* pCityEspionage = GetCityEspionage();
	if(pCityEspionage)
	{
		for(int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			int iAssignedSpy = pCityEspionage->m_aiSpyAssignment[i];
			// if there is a spy in the city
			if(iAssignedSpy != -1)
			{
				GET_PLAYER((PlayerTypes)i).GetEspionage()->ExtractSpyFromCity(iAssignedSpy);
			}
		}
	}

	// Delete the city's information here!!!
	GET_PLAYER(getOwner()).deleteCity(m_iID);
	GET_PLAYER(eOwner).GetCityConnections()->Update();

	// clean up
	PostKill(bCapital, pPlot, eOwner);
}

//	--------------------------------------------------------------------------------
CvPlayer* CvCity::GetPlayer()
{
	VALIDATE_OBJECT
	return &GET_PLAYER(getOwner());
}

//	--------------------------------------------------------------------------------
void CvCity::doTurn()
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iI;

	if(getDamage() > 0)
	{
		CvAssertMsg(m_iDamage <= GetMaxHitPoints(), "Somehow a city has more damage than hit points. Please show this to a gameplay programmer immediately.");

		int iHitsHealed = GC.getCITY_HIT_POINTS_HEALED_PER_TURN();
		if(isCapital() && !GET_PLAYER(getOwner()).isMinorCiv())
		{
			iHitsHealed++;
		}
		int iBuildingDefense = m_pCityBuildings->GetBuildingDefense();
		iBuildingDefense *= (100 + m_pCityBuildings->GetBuildingDefenseMod());
		iBuildingDefense /= 100;
		iHitsHealed += iBuildingDefense / 500;
		changeDamage(-iHitsHealed);
	}
	if(getDamage() < 0)
	{
		setDamage(0);
	}

	setDrafted(false);
	setAirliftTargeted(false);
	setCurrAirlift(0);
	setMadeAttack(false);
	GetCityBuildings()->SetSoldBuildingThisTurn(false);

	DoUpdateFeatureSurrounded();

	GetCityStrategyAI()->DoTurn();

	GetCityCitizens()->DoTurn();

	AI_doTurn();

	bool bRazed = DoRazingTurn();

	if(!bRazed)
	{
		DoResistanceTurn();

		bool bAllowNoProduction = !doCheckProduction();

		doGrowth();

		DoUpdateIndustrialRouteToCapital();

		doProduction(bAllowNoProduction);

		doDecay();

		doMeltdown();

		for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = GetCityCitizens()->GetCityPlotFromIndex(iI);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getWorkingCity() == this)
				{
					if(pLoopPlot->isBeingWorked())
					{
						pLoopPlot->doImprovement();
					}
				}
			}
		}

		// Following function also looks at WLTKD stuff
		DoTestResourceDemanded();

		// Culture accumulation
		if(getJONSCulturePerTurn() > 0)
		{
			ChangeJONSCultureStored(getJONSCulturePerTurn());
		}

		// Enough Culture to acquire a new Plot?
		if(GetJONSCultureStored() >= GetJONSCultureThreshold())
		{
			DoJONSCultureLevelIncrease();
		}

		// Resource Demanded Counter
		if(GetResourceDemandedCountdown() > 0)
		{
			ChangeResourceDemandedCountdown(-1);

			if(GetResourceDemandedCountdown() == 0)
			{
				// Pick a Resource to demand
				DoPickResourceDemanded();
			}
		}

		updateStrengthValue();

		DoNearbyEnemy();

		//Check for Achievements
		if(isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
		{
			if(getJONSCulturePerTurn()>=100)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_CITY_100CULTURE);
			}
			if(getYieldRate(YIELD_GOLD)>=100)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_CITY_100GOLD);
			}
			if(getYieldRate(YIELD_SCIENCE)>=100)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_CITY_100SCIENCE);
			}
		}

		// sending notifications on when routes are connected to the capital
		if(!isCapital())
		{
			CvNotifications* pNotifications = GET_PLAYER(m_eOwner).GetNotifications();
			if(pNotifications)
			{
				CvCity* pPlayerCapital = GET_PLAYER(m_eOwner).getCapitalCity();
				CvAssertMsg(pPlayerCapital, "No capital city?");

				if(m_bRouteToCapitalConnectedLastTurn != m_bRouteToCapitalConnectedThisTurn && pPlayerCapital)
				{
					Localization::String strMessage;
					Localization::String strSummary;

					if(m_bRouteToCapitalConnectedThisTurn)  // connected this turn
					{
						strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_ROUTE_ESTABLISHED");
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_TRADE_ROUTE_ESTABLISHED");
						strMessage << getNameKey();
						strMessage << pPlayerCapital->getNameKey();
						pNotifications->Add(NOTIFICATION_TRADE_ROUTE, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
					else // lost connection this turn
					{
						strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_TRADE_ROUTE_BROKEN");
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_TRADE_ROUTE_BROKEN");
						strMessage << getNameKey();
						strMessage << pPlayerCapital->getNameKey();
						pNotifications->Add(NOTIFICATION_TRADE_ROUTE_BROKEN, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
				}
			}

			m_bRouteToCapitalConnectedLastTurn = m_bRouteToCapitalConnectedThisTurn;
		}

		// XXX
#ifdef _DEBUG
		{
			CvPlot* pPlot;
			int iCount;

			for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				CvAssert(getBaseYieldRate((YieldTypes)iI) >= 0);
				CvAssert(getYieldRate((YieldTypes)iI) >= 0);

				iCount = 0;

				for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					pPlot = GetCityCitizens()->GetCityPlotFromIndex(iJ);

					if(pPlot != NULL)
					{
						if(GetCityCitizens()->IsWorkingPlot(pPlot))
						{
							iCount += pPlot->getYield((YieldTypes)iI);
						}
					}
				}

				for(int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
				{
					iCount += (GET_PLAYER(getOwner()).specialistYield(((SpecialistTypes)iJ), ((YieldTypes)iI)) * (GetCityCitizens()->GetSpecialistCount((SpecialistTypes)iJ)));
				}

				for(int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					const BuildingTypes eBuilding = static_cast<BuildingTypes>(iJ);
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
					if(pkBuildingInfo)
					{
						iCount += m_pCityBuildings->GetNumActiveBuilding(eBuilding) * (pkBuildingInfo->GetYieldChange(iI) + m_pCityBuildings->GetBuildingYieldChange((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), (YieldTypes)iI));
					}
				}

				// Science from Population
				if((YieldTypes) iI == YIELD_SCIENCE)
				{
					iCount += getPopulation() * GC.getSCIENCE_PER_POPULATION();
				}

				CvAssert(iCount == getBaseYieldRate((YieldTypes)iI));
			}
		}
#endif
		// XXX
	}
}


//	--------------------------------------------------------------------------------
bool CvCity::isCitySelected()
{
	VALIDATE_OBJECT
	auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);

	return DLLUI->isCitySelected(pCity.get());
}


//	--------------------------------------------------------------------------------
bool CvCity::canBeSelected() const
{
	VALIDATE_OBJECT
	if((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvCity::updateYield()
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iI;

	for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = GetCityCitizens()->GetCityPlotFromIndex(iI);

		if(pLoopPlot != NULL)
		{
			pLoopPlot->updateYield();
		}
	}
}

//	--------------------------------------------------------------------------------
/// Connected to capital with industrial route? (Railroads)
bool CvCity::IsIndustrialRouteToCapital() const
{
	return m_bIndustrialRouteToCapital;
}

//	--------------------------------------------------------------------------------
/// Connected to capital with industrial route? (Railroads)
void CvCity::SetIndustrialRouteToCapital(bool bValue)
{
	if(bValue != IsIndustrialRouteToCapital())
	{
		m_bIndustrialRouteToCapital = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Connected to capital with industrial route? (Railroads)
void CvCity::DoUpdateIndustrialRouteToCapital()
{
	// Capital - what do we want to do about this?
	if(isCapital())
	{
	}
	// Non-capital city
	else
	{
		if(GET_PLAYER(getOwner()).IsCapitalConnectedToCity(this, GC.getGame().GetIndustrialRoute()))
		{
			SetIndustrialRouteToCapital(true);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvCity::SetRouteToCapitalConnected(bool bValue)
{
	bool bUpdateReligion = false;

	if(bValue != m_bRouteToCapitalConnectedThisTurn)
	{
		bUpdateReligion = true;
	}

	m_bRouteToCapitalConnectedThisTurn = bValue;

	if(bUpdateReligion)
	{
		UpdateReligion(GetCityReligions()->GetReligiousMajority());
	}

	if(GC.getGame().getGameTurn() == 0)
	{
		m_bRouteToCapitalConnectedLastTurn = bValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvCity::IsRouteToCapitalConnected(void)
{
	return m_bRouteToCapitalConnectedThisTurn;
}


//	--------------------------------------------------------------------------------
void CvCity::createGreatGeneral(UnitTypes eGreatPersonUnit)
{
	VALIDATE_OBJECT
	GET_PLAYER(getOwner()).createGreatGeneral(eGreatPersonUnit, getX(), getY());
}

//	--------------------------------------------------------------------------------
void CvCity::createGreatAdmiral(UnitTypes eGreatPersonUnit)
{
	VALIDATE_OBJECT
	GET_PLAYER(getOwner()).createGreatAdmiral(eGreatPersonUnit, getX(), getY());
}

//	--------------------------------------------------------------------------------
CityTaskResult CvCity::doTask(TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	VALIDATE_OBJECT
	CityTaskResult eResult = TASK_COMPLETED;
	switch(eTask)
	{
	case TASK_RAZE:
		GET_PLAYER(getOwner()).raze(this);
		break;

	case TASK_UNRAZE:
		GET_PLAYER(getOwner()).unraze(this);
		break;

	case TASK_DISBAND:
		GET_PLAYER(getOwner()).disband(this);
		break;

	case TASK_GIFT:
		GET_PLAYER((PlayerTypes)iData1).acquireCity(this, false, true);
		break;

	case TASK_SET_AUTOMATED_CITIZENS:
		break;

	case TASK_SET_AUTOMATED_PRODUCTION:
		setProductionAutomated(bOption, bAlt && bShift && bCtrl);
		break;

	case TASK_SET_EMPHASIZE:
		m_pEmphases->SetEmphasize(((EmphasizeTypes)iData1), bOption);
		break;

	case TASK_NO_AUTO_ASSIGN_SPECIALISTS:
		GetCityCitizens()->SetNoAutoAssignSpecialists(bOption);
		break;

	case TASK_ADD_SPECIALIST:
		GetCityCitizens()->DoAddSpecialistToBuilding(/*eBuilding*/ (BuildingTypes) iData2, true);
		break;

	case TASK_REMOVE_SPECIALIST:
		GetCityCitizens()->DoRemoveSpecialistFromBuilding(/*eBuilding*/ (BuildingTypes) iData2, true);
		GetCityCitizens()->DoAddBestCitizenFromUnassigned();
		break;

	case TASK_CHANGE_WORKING_PLOT:
		GetCityCitizens()->DoAlterWorkingPlot(/*CityPlotIndex*/ iData1);
		break;

	case TASK_REMOVE_SLACKER:
		if (GetCityCitizens()->GetNumDefaultSpecialists() > 0)
		{
			GetCityCitizens()->ChangeNumDefaultSpecialists(-1);
			GetCityCitizens()->DoReallocateCitizens();
		}
		break;

	case TASK_CLEAR_WORKING_OVERRIDE:
		clearWorkingOverride(iData1);
		break;

	case TASK_HURRY:
		hurry((HurryTypes)iData1);
		break;

	case TASK_CONSCRIPT:
		conscript();
		break;

	case TASK_CLEAR_ORDERS:
		clearOrderQueue();
		break;

	case TASK_RALLY_PLOT:
		setRallyPlot(GC.getMap().plot(iData1, iData2));
		break;

	case TASK_CLEAR_RALLY_PLOT:
		setRallyPlot(NULL);
		break;

	case TASK_RANGED_ATTACK:
		eResult = rangeStrike(iData1,iData2);
		break;

	case TASK_CREATE_PUPPET:
		DoCreatePuppet();
		break;

	case TASK_ANNEX_PUPPET:
		DoAnnex();
		break;

	default:
		CvAssertMsg(false, "eTask failed to match a valid option");
		break;
	}

	return eResult;
}


//	--------------------------------------------------------------------------------
void CvCity::chooseProduction(UnitTypes eTrainUnit, BuildingTypes eConstructBuilding, ProjectTypes eCreateProject, bool /*bFinish*/, bool /*bFront*/)
{
	VALIDATE_OBJECT
	CvString strTooltip = GetLocalizedText("TXT_KEY_NOTIFICATION_NEW_CONSTRUCTION", getNameKey());

	CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
	if(pNotifications)
	{
		// Figure out what we just finished so we can package it into something the lua will understand
		OrderTypes eOrder = NO_ORDER;
		int iItemID = -1;

		if(eTrainUnit != NO_UNIT)
		{
			eOrder = ORDER_TRAIN;
			iItemID = eTrainUnit;
		}
		else if(eConstructBuilding != NO_BUILDING)
		{
			eOrder = ORDER_CONSTRUCT;
			iItemID = eConstructBuilding;
		}
		else if(eCreateProject != NO_PROJECT)
		{
			eOrder = ORDER_CREATE;
			iItemID = eCreateProject;
		}

		pNotifications->Add(NOTIFICATION_PRODUCTION, strTooltip, strTooltip, getX(), getY(), eOrder, iItemID);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::clearWorkingOverride(int iIndex)
{
	VALIDATE_OBJECT
	CvPlot* pPlot;

	pPlot = GetCityCitizens()->GetCityPlotFromIndex(iIndex);

	if(pPlot != NULL)
	{
		pPlot->setWorkingCityOverride(NULL);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::countNumImprovedPlots(ImprovementTypes eImprovement, bool bPotential) const
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	CvCityCitizens* pCityCitizens = GetCityCitizens();

	for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = pCityCitizens->GetCityPlotFromIndex(iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getWorkingCity() == this)
			{
				if(eImprovement != NO_IMPROVEMENT)
				{
					if((pLoopPlot->getImprovementType() == eImprovement && !pLoopPlot->IsImprovementPillaged()) || (bPotential && pLoopPlot->canHaveImprovement(eImprovement, getTeam())))
					{
						++iCount;
					}
				}
				else if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT && !pLoopPlot->IsImprovementPillaged())
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvCity::countNumWaterPlots() const
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	CvCityCitizens* pCityCitizens = GetCityCitizens();

	for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = pCityCitizens->GetCityPlotFromIndex(iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->isWater())
			{
				if(pLoopPlot->getWorkingCity() == this)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvCity::countNumRiverPlots() const
{
	VALIDATE_OBJECT
	int iCount = 0;

	CvCityCitizens* pCityCitizens = GetCityCitizens();

	for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = pCityCitizens->GetCityPlotFromIndex(iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->isRiver())
			{
				if(pLoopPlot->getWorkingCity() == this)
				{
					++iCount;
				}
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvCity::countNumForestPlots() const
{
	VALIDATE_OBJECT
	int iCount = 0;

	for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = GetCityCitizens()->GetCityPlotFromIndex(iI);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getWorkingCity() == this)
			{
				if(pLoopPlot->getFeatureType() == FEATURE_FOREST)
				{
					++iCount;
				}
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
int CvCity::findPopulationRank()
{
	VALIDATE_OBJECT
	if(!m_bPopulationRankValid)
	{
		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for(pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoop))
		{
			if((pLoopCity->getPopulation() > getPopulation()) ||
			        ((pLoopCity->getPopulation() == getPopulation()) && (pLoopCity->GetID() < GetID())))
			{
				iRank++;
			}
		}

		// shenanigans are to get around the const check
		m_bPopulationRankValid = true;
		m_iPopulationRank = iRank;
	}

	return m_iPopulationRank;
}


//	--------------------------------------------------------------------------------
int CvCity::findBaseYieldRateRank(YieldTypes eYield)
{
	VALIDATE_OBJECT
	if(!m_abBaseYieldRankValid[eYield])
	{
		int iRate = getBaseYieldRate(eYield);

		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for(pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoop))
		{
			if((pLoopCity->getBaseYieldRate(eYield) > iRate) ||
			        ((pLoopCity->getBaseYieldRate(eYield) == iRate) && (pLoopCity->GetID() < GetID())))
			{
				iRank++;
			}
		}

		m_abBaseYieldRankValid.setAt(eYield, true);
		m_aiBaseYieldRank.setAt(eYield, iRank);
	}

	return m_aiBaseYieldRank[eYield];
}


//	--------------------------------------------------------------------------------
int CvCity::findYieldRateRank(YieldTypes eYield)
{
	if(!m_abYieldRankValid[eYield])
	{
		int iRate = getYieldRateTimes100(eYield);

		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for(pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoop))
		{
			if((pLoopCity->getYieldRateTimes100(eYield) > iRate) ||
			        ((pLoopCity->getYieldRateTimes100(eYield) == iRate) && (pLoopCity->GetID() < GetID())))
			{
				iRank++;
			}
		}

		m_abYieldRankValid.setAt(eYield, true);
		m_aiYieldRank.setAt(eYield, iRank);
	}

	return m_aiYieldRank[eYield];
}


//	--------------------------------------------------------------------------------
// Returns one of the upgrades...
UnitTypes CvCity::allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount) const
{
	VALIDATE_OBJECT
	UnitTypes eUpgradeUnit;
	bool bUpgradeFound;
	bool bUpgradeAvailable;
	bool bUpgradeUnavailable;

	CvAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
		return NO_UNIT;

	if(iUpgradeCount > GC.getNumUnitClassInfos())
	{
		return NO_UNIT;
	}

	eUpgradeUnit = NO_UNIT;

	bUpgradeFound = false;
	bUpgradeAvailable = false;
	bUpgradeUnavailable = false;

	CvCivilizationInfo& thisCiv = getCivilizationInfo();

	for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			if(pkUnitInfo->GetUpgradeUnitClass(iI))
			{
				const UnitTypes eLoopUnit = (UnitTypes) thisCiv.getCivilizationUnits(iI);

				if(eLoopUnit != NO_UNIT)
				{
					bUpgradeFound = true;

					const UnitTypes eTempUnit = allUpgradesAvailable(eLoopUnit, (iUpgradeCount + 1));

					if(eTempUnit != NO_UNIT)
					{
						eUpgradeUnit = eTempUnit;
						bUpgradeAvailable = true;
					}
					else
					{
						bUpgradeUnavailable = true;
					}
				}
			}
		}
	}

	if(iUpgradeCount > 0)
	{
		if(bUpgradeFound && bUpgradeAvailable)
		{
			CvAssertMsg(eUpgradeUnit != NO_UNIT, "eUpgradeUnit is expected to be assigned (not NO_UNIT)");
			return eUpgradeUnit;
		}

		if(canTrain(eUnit, false, false, false, true))
		{
			return eUnit;
		}
	}
	else
	{
		if(bUpgradeFound && !bUpgradeUnavailable)
		{
			return eUpgradeUnit;
		}
	}

	return NO_UNIT;
}


//	--------------------------------------------------------------------------------
bool CvCity::isWorldWondersMaxed() const
{
	VALIDATE_OBJECT
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	if(GC.getMAX_WORLD_WONDERS_PER_CITY() == -1)
	{
		return false;
	}

	if(getNumWorldWonders() >= GC.getMAX_WORLD_WONDERS_PER_CITY())
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isTeamWondersMaxed() const
{
	VALIDATE_OBJECT
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	if(GC.getMAX_TEAM_WONDERS_PER_CITY() == -1)
	{
		return false;
	}

	if(getNumTeamWonders() >= GC.getMAX_TEAM_WONDERS_PER_CITY())
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isNationalWondersMaxed() const
{
	VALIDATE_OBJECT
	int iMaxNumWonders = (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()) ? GC.getMAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC() : GC.getMAX_NATIONAL_WONDERS_PER_CITY();

	if(iMaxNumWonders == -1)
	{
		return false;
	}

	if(getNumNationalWonders() >= iMaxNumWonders)
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isBuildingsMaxed() const
{
	VALIDATE_OBJECT
	if(GC.getMAX_BUILDINGS_PER_CITY() == -1)
	{
		return false;
	}

	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	if(m_pCityBuildings->GetNumBuildings() >= GC.getMAX_BUILDINGS_PER_CITY())
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool /*bIgnoreUpgrades*/, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	if(eUnit == NO_UNIT)
	{
		return false;
	}

	CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
	if(pkUnitEntry == NULL)
	{
		return false;
	}

	if(!(GET_PLAYER(getOwner()).canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost, false, toolTipSink)))
	{
		return false;
	}

	if(!bTestVisible)
	{
		CvUnitEntry& thisUnitInfo = *pkUnitEntry;
		// Settlers may not be trained in Cities that are too small
		if(thisUnitInfo.IsFound() || thisUnitInfo.IsFoundAbroad())
		{
			int iSizeRequirement = /*2*/ GC.getCITY_MIN_SIZE_FOR_SETTLERS();
			if(getPopulation() < iSizeRequirement)
			{
				GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_SETTLER_SIZE_LIMIT", "", "", iSizeRequirement);
				if(toolTipSink == NULL)
					return false;
			}
		}

		// See if there are any BuildingClass requirements
		const int iNumBuildingClassInfos = GC.getNumBuildingClassInfos();
		CvCivilizationInfo& thisCivilization = getCivilizationInfo();
		for(int iBuildingClassLoop = 0; iBuildingClassLoop < iNumBuildingClassInfos; iBuildingClassLoop++)
		{
			const BuildingClassTypes eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;
			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			// Requires Building
			if(thisUnitInfo.GetBuildingClassRequireds(eBuildingClass))
			{
				const BuildingTypes ePrereqBuilding = (BuildingTypes)(thisCivilization.getCivilizationBuildings(eBuildingClass));

				if(GetCityBuildings()->GetNumBuilding(ePrereqBuilding) == 0)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(ePrereqBuilding);
					if(pkBuildingInfo)
					{
						GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_UNIT_REQUIRES_BUILDING", pkBuildingInfo->GetDescriptionKey());
						if(toolTipSink == NULL)
							return false;
					}
				}
			}
		}
	}

	// JON: Not using Unit Upgrades as a training prevention
	//if (!bIgnoreUpgrades)
	//{
	//	if (allUpgradesAvailable(eUnit) != NO_UNIT)
	//	{
	//		return false;
	//	}
	//}

	if(!plot()->canTrain(eUnit, bContinue, bTestVisible))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(eUnit);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanTrain", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvCity::canTrain(UnitCombatTypes eUnitCombat) const
{
	VALIDATE_OBJECT
	for(int i = 0; i < GC.getNumUnitInfos(); i++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(i);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			if(pkUnitInfo->GetUnitCombatType() == eUnitCombat)
			{
				if(canTrain(eUnit))
				{
					return true;
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	BuildingTypes ePrereqBuilding;
	int iI;

	if(eBuilding == NO_BUILDING)
	{
		return false;
	}

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return false;
	}

	if(!(GET_PLAYER(getOwner()).canConstruct(eBuilding, bContinue, bTestVisible, bIgnoreCost, toolTipSink)))
	{
		return false;
	}

	if(m_pCityBuildings->GetNumBuilding(eBuilding) >= GC.getCITY_MAX_NUM_BUILDINGS())
	{
		return false;
	}

	if(!isValidBuildingLocation(eBuilding))
	{
		return false;
	}

	// Local Resource requirements met?
	if(!IsBuildingLocalResourceValid(eBuilding, bTestVisible, toolTipSink))
	{
		return false;
	}

	// Holy city requirement
	if (pkBuildingInfo->IsRequiresHolyCity() && !GetCityReligions()->IsHolyCityAnyReligion())
	{
		return false;
	}

	CvCivilizationInfo& thisCivInfo = *GC.getCivilizationInfo(getCivilizationType());
	int iNumBuildingClassInfos = GC.getNumBuildingClassInfos();

	// Can't construct a building to reduce occupied unhappiness if the city isn't occupied
	if(pkBuildingInfo->IsNoOccupiedUnhappiness() && !IsOccupied())
		return false;

	// Does this city have prereq buildings?
	for(iI = 0; iI < iNumBuildingClassInfos; iI++)
	{
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo((BuildingClassTypes)iI);
		if(!pkBuildingClassInfo)
		{
			continue;
		}

		if(pkBuildingInfo->IsBuildingClassNeededInCity(iI))
		{
			ePrereqBuilding = ((BuildingTypes)(thisCivInfo.getCivilizationBuildings(iI)));

			if(ePrereqBuilding != NO_BUILDING)
			{
				if(0 == m_pCityBuildings->GetNumBuilding(ePrereqBuilding) /* && (bContinue || (getFirstBuildingOrder(ePrereqBuilding) == -1))*/)
				{
					return false;
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	// Everything above this is what is checked to see if Building shows up in the list of construction items
	///////////////////////////////////////////////////////////////////////////////////

	const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
	if(!bTestVisible)
	{
		if(!bContinue)
		{
			if(getFirstBuildingOrder(eBuilding) != -1)
			{
				return false;
			}
		}

		if(!(kBuildingClassInfo.isNoLimit()))
		{
			if(isWorldWonderClass(kBuildingClassInfo))
			{
				if(isWorldWondersMaxed())
				{
					return false;
				}
			}
			else if(isTeamWonderClass(kBuildingClassInfo))
			{
				if(isTeamWondersMaxed())
				{
					return false;
				}
			}
			else if(isNationalWonderClass(kBuildingClassInfo))
			{
				if(isNationalWondersMaxed())
				{
					return false;
				}
			}
			else
			{
				if(isBuildingsMaxed())
				{
					return false;
				}
			}
		}
	}

	// Locked Buildings (Mutually Exclusive Buildings?) - not quite sure how this works
	for(iI = 0; iI < iNumBuildingClassInfos; iI++)
	{
		BuildingClassTypes eLockedBuildingClass = (BuildingClassTypes) pkBuildingInfo->GetLockedBuildingClasses(iI);

		if(eLockedBuildingClass != NO_BUILDINGCLASS)
		{
			BuildingTypes eLockedBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(eLockedBuildingClass));

			if(eLockedBuilding != NO_BUILDING)
			{
				if(m_pCityBuildings->GetNumBuilding(eLockedBuilding) > 0)
				{
					return false;
				}
			}
		}
	}

	// Mutually Exclusive Buildings 2
	if(pkBuildingInfo->GetMutuallyExclusiveGroup() != -1)
	{
		int iNumBuildingInfos = GC.getNumBuildingInfos();
		for(iI = 0; iI < iNumBuildingInfos; iI++)
		{
			const BuildingTypes eBuildingLoop = static_cast<BuildingTypes>(iI);

			CvBuildingEntry* pkLoopBuilding = GC.getBuildingInfo(eBuildingLoop);
			if(pkLoopBuilding)
			{
				// Buildings are in a Mutually Exclusive Group, so only one is allowed
				if(pkLoopBuilding->GetMutuallyExclusiveGroup() == pkBuildingInfo->GetMutuallyExclusiveGroup())
				{
					if(m_pCityBuildings->GetNumBuilding(eBuildingLoop) > 0)
					{
						return false;
					}
				}
			}
		}
	}


	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(eBuilding);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanConstruct", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvCity::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	VALIDATE_OBJECT

	if(!(GET_PLAYER(getOwner()).canCreate(eProject, bContinue, bTestVisible)))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(eProject);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanCreate", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvCity::canPrepare(SpecialistTypes eSpecialist, bool bContinue) const
{
	VALIDATE_OBJECT

	if(!(GET_PLAYER(getOwner()).canPrepare(eSpecialist, bContinue)))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(eSpecialist);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanPrepare", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvCity::canMaintain(ProcessTypes eProcess, bool bContinue) const
{
	VALIDATE_OBJECT

	if(!(GET_PLAYER(getOwner()).canMaintain(eProcess, bContinue)))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(eProcess);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanMaintain", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvCity::canJoin() const
{
	VALIDATE_OBJECT
	return true;
}

//	--------------------------------------------------------------------------------
// Are there a lot of clearable features around this city?
bool CvCity::IsFeatureSurrounded() const
{
	return m_bFeatureSurrounded;
}

//	--------------------------------------------------------------------------------
// Are there a lot of clearable features around this city?
void CvCity::SetFeatureSurrounded(bool bValue)
{
	if(IsFeatureSurrounded() != bValue)
		m_bFeatureSurrounded = bValue;
}

//	--------------------------------------------------------------------------------
// Are there a lot of clearable features around this city?
void CvCity::DoUpdateFeatureSurrounded()
{
	int iTotalPlots = 0;
	int iFeaturedPlots = 0;

	// Look two tiles around this city in every direction to see if at least half the plots are covered in a removable feature
	const int iRange = 2;

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iRange);

			// Increase total plot count
			iTotalPlots++;

			if(pLoopPlot == NULL)
				continue;

			if(pLoopPlot->getFeatureType() == NO_FEATURE)
				continue;

			// Must be able to remove this thing?
			if(!GC.getFeatureInfo(pLoopPlot->getFeatureType())->IsClearable())
				continue;

			iFeaturedPlots++;
		}
	}

	bool bFeatureSurrounded = false;

	// At least half have coverage?
	if(iFeaturedPlots >= iTotalPlots / 2)
		bFeatureSurrounded = true;

	SetFeatureSurrounded(bFeatureSurrounded);
}

//	--------------------------------------------------------------------------------
/// Extra yield for a resource this city is working?
int CvCity::GetResourceExtraYield(ResourceTypes eResource, YieldTypes eYield) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eResource > -1 && eResource < GC.getNumResourceInfos(), "Invalid resource index.");
	CvAssertMsg(eYield > -1 && eYield < NUM_YIELD_TYPES, "Invalid yield index.");

	return m_ppaiResourceYieldChange[eResource][eYield];
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeResourceExtraYield(ResourceTypes eResource, YieldTypes eYield, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eResource > -1 && eResource < GC.getNumResourceInfos(), "Invalid resource index.");
	CvAssertMsg(eYield > -1 && eYield < NUM_YIELD_TYPES, "Invalid yield index.");

	if(iChange != 0)
	{
		m_ppaiResourceYieldChange[eResource][eYield] += iChange;

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
/// Extra yield for a Feature this city is working?
int CvCity::GetFeatureExtraYield(FeatureTypes eFeature, YieldTypes eYield) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature > -1 && eFeature < GC.getNumFeatureInfos(), "Invalid Feature index.");
	CvAssertMsg(eYield > -1 && eYield < NUM_YIELD_TYPES, "Invalid yield index.");

	return m_ppaiFeatureYieldChange[eFeature][eYield];
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeFeatureExtraYield(FeatureTypes eFeature, YieldTypes eYield, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature > -1 && eFeature < GC.getNumFeatureInfos(), "Invalid Feature index.");
	CvAssertMsg(eYield > -1 && eYield < NUM_YIELD_TYPES, "Invalid yield index.");

	if(iChange != 0)
	{
		m_ppaiFeatureYieldChange[eFeature][eYield] += iChange;

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
/// Extra yield for a Terrain this city is working?
int CvCity::GetTerrainExtraYield(TerrainTypes eTerrain, YieldTypes eYield) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain > -1 && eTerrain < GC.getNumTerrainInfos(), "Invalid Terrain index.");
	CvAssertMsg(eYield > -1 && eYield < NUM_YIELD_TYPES, "Invalid yield index.");

	return m_ppaiTerrainYieldChange[eTerrain][eYield];
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeTerrainExtraYield(TerrainTypes eTerrain, YieldTypes eYield, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain > -1 && eTerrain < GC.getNumTerrainInfos(), "Invalid Terrain index.");
	CvAssertMsg(eYield > -1 && eYield < NUM_YIELD_TYPES, "Invalid yield index.");

	if(iChange != 0)
	{
		m_ppaiTerrainYieldChange[eTerrain][eYield] += iChange;

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
/// Does this City have eResource nearby?
bool CvCity::IsHasResourceLocal(ResourceTypes eResource, bool bTestVisible) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eResource > -1 && eResource < GC.getNumResourceInfos(), "Invalid resource index.");

	// Actually check to see if we have this Resource to use right now
	if(!bTestVisible)
	{
		return m_paiNumResourcesLocal[eResource] > 0;
	}

	// See if we have the resource linked to this city, but not connected yet

	bool bFoundResourceLinked = false;

	// Loop through all plots near this City to see if we can find eResource - tests are ordered to optimize performance
	CvPlot* pLoopPlot;
	for(int iCityPlotLoop = 0; iCityPlotLoop < NUM_CITY_PLOTS; iCityPlotLoop++)
	{
		pLoopPlot = plotCity(getX(), getY(), iCityPlotLoop);

		// Invalid plot
		if(pLoopPlot == NULL)
			continue;

		// Doesn't have the resource (ignore team first to save time)
		if(pLoopPlot->getResourceType() != eResource)
			continue;

		// Not owned by this player
		if(pLoopPlot->getOwner() != getOwner())
			continue;

		// Team can't see the resource here
		if(pLoopPlot->getResourceType(getTeam()) != eResource)
			continue;

		// Resource not linked to this city
		if(pLoopPlot->GetResourceLinkedCity() != this)
			continue;

		bFoundResourceLinked = true;
		break;
	}

	return bFoundResourceLinked;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeNumResourceLocal(ResourceTypes eResource, int iChange)
{
	VALIDATE_OBJECT

	CvAssertMsg(eResource >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eResource < GC.getNumResourceInfos(), "eIndex expected to be < GC.getNumResourceInfos()");

	if(iChange != 0)
	{
		bool bOldHasResource = IsHasResourceLocal(eResource, /*bTestVisible*/ false);

		m_paiNumResourcesLocal.setAt(eResource, m_paiNumResourcesLocal[eResource] + iChange);

		if(bOldHasResource != IsHasResourceLocal(eResource, /*bTestVisible*/ false))
		{
			if(IsHasResourceLocal(eResource, /*bTestVisible*/ false))
			{
				processResource(eResource, 1);

				// Notification letting player know his city gets a bonus for wonders
				int iWonderMod = GC.getResourceInfo(eResource)->getWonderProductionMod();
				if(iWonderMod != 0)
				{
					CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
					if(pNotifications)
					{
						Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_RESOURCE_WONDER_MOD");
						strText << getNameKey() << GC.getResourceInfo(eResource)->GetTextKey() << iWonderMod;
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_RESOURCE_WONDER_MOD_SUMMARY");
						strSummary << getNameKey() << GC.getResourceInfo(eResource)->GetTextKey();
						pNotifications->Add(NOTIFICATION_DISCOVERED_BONUS_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), eResource);
					}
				}
			}
			else
			{
				processResource(eResource, -1);
			}
		}

		// Building Culture change for a local resource
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				// Do we have this building?
				if(GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Does eBuilding give culture with eResource?
					int iCulture = pkBuildingInfo->GetResourceCultureChange(eResource);

					if(iCulture != 0)
						ChangeJONSCulturePerTurnFromBuildings(iCulture * iChange);

					// Does eBuilding give faith with eResource?
					int iFaith = pkBuildingInfo->GetResourceFaithChange(eResource);

					if(iFaith != 0)
						ChangeFaithPerTurnFromBuildings(iFaith * iChange);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Does eBuilding pass the local resource requirement test?
bool CvCity::IsBuildingLocalResourceValid(BuildingTypes eBuilding, bool bTestVisible, CvString* toolTipSink) const
{
	VALIDATE_OBJECT

	int iResourceLoop;
	ResourceTypes eResource;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return false;

	// ANDs: City must have ALL of these nearby
	for(iResourceLoop = 0; iResourceLoop < GC.getNUM_BUILDING_RESOURCE_PREREQS(); iResourceLoop++)
	{
		eResource = (ResourceTypes) pkBuildingInfo->GetLocalResourceAnd(iResourceLoop);

		// Doesn't require a resource in this AND slot
		if(eResource == NO_RESOURCE)
			continue;

		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource == NULL)
			continue;

		// City doesn't have resource locally - return false immediately
		if(!IsHasResourceLocal(eResource, bTestVisible))
		{
			GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_LOCAL_RESOURCE", pkResource->GetTextKey(), pkResource->GetIconString());
			return false;
		}
	}

	int iOrResources = 0;

	// ORs: City must have ONE of these nearby
	for(iResourceLoop = 0; iResourceLoop < GC.getNUM_BUILDING_RESOURCE_PREREQS(); iResourceLoop++)
	{
		eResource = (ResourceTypes) pkBuildingInfo->GetLocalResourceOr(iResourceLoop);

		// Doesn't require a resource in this AND slot
		if(eResource == NO_RESOURCE)
			continue;

		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource == NULL)
			continue;

		// City has resource locally - return true immediately
		if(IsHasResourceLocal(eResource, bTestVisible))
			return true;

		// If we get here it means we passed the AND tests but not one of the OR tests
		GC.getGame().BuildCannotPerformActionHelpText(toolTipSink, "TXT_KEY_NO_ACTION_BUILDING_LOCAL_RESOURCE", pkResource->GetTextKey(), pkResource->GetIconString());

		// Increment counter for OR we don't have
		iOrResources++;
	}

	// No OR resource requirements (and passed the AND test above)
	if(iOrResources == 0)
		return true;

	return false;
}


//	--------------------------------------------------------------------------------
/// What Resource does this City want so that it goes into WLTKD?
ResourceTypes CvCity::GetResourceDemanded(bool bHideUnknown) const
{
	VALIDATE_OBJECT
	ResourceTypes eResourceDemanded = static_cast<ResourceTypes>(m_iResourceDemanded.get());

	// If we're not hiding the result then don't bother with looking at tech
	if(!bHideUnknown)
	{
		return eResourceDemanded;
	}

	if(eResourceDemanded != NO_RESOURCE)
	{
		TechTypes eRevealTech = (TechTypes) GC.getResourceInfo(eResourceDemanded)->getTechReveal();

		// Is there no Reveal Tech or do we have it?
		if(eRevealTech == NO_TECH || GET_TEAM(getTeam()).GetTeamTechs()->HasTech(eRevealTech))
		{
			return eResourceDemanded;
		}
	}

	// We don't have the Tech to reveal the currently demanded Resource
	return NO_RESOURCE;
}

//	--------------------------------------------------------------------------------
/// Sets what Resource this City wants so that it goes into WLTKD
void CvCity::SetResourceDemanded(ResourceTypes eResource)
{
	VALIDATE_OBJECT
	m_iResourceDemanded = (ResourceTypes) eResource;
}

//	--------------------------------------------------------------------------------
/// Picks a Resource for this City to want
void CvCity::DoPickResourceDemanded(bool bCurrentResourceInvalid)
{
	VALIDATE_OBJECT
	// Create the list of invalid Luxury Resources
	FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veInvalidLuxuryResources;
	CvPlot* pLoopPlot;
	ResourceTypes eResource;

	// Loop through all resource infos and invalidate resources that only come from minor civs
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
		{
			if (pkResource->isOnlyMinorCivs())
			{
				veInvalidLuxuryResources.push_back(eResource);
			}
		}
	}

	// Loop through all Plots near this City to see if there's Luxuries we should invalidate
	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		pLoopPlot = plotCity(getX(), getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			eResource = pLoopPlot->getResourceType();

			if(eResource != NO_RESOURCE)
			{
				if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					veInvalidLuxuryResources.push_back(eResource);
				}
			}
		}
	}

	// Current Resource demanded may not be a valid choice
	ResourceTypes eCurrentResource = GetResourceDemanded(false);
	if(bCurrentResourceInvalid && eCurrentResource != NO_RESOURCE)
	{
		veInvalidLuxuryResources.push_back(eCurrentResource);
	}

	// Create list of valid Luxuries
	FStaticVector<ResourceTypes, 64, true, c_eCiv5GameplayDLL, 0> veValidLuxuryResources;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		// Is this a Luxury Resource?
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
		{
			// Is the Resource actually on the map?
			if(GC.getMap().getNumResources(eResource) > 0)
			{
				// Can't be a minor civ only resource!
				if(!pkResourceInfo->isOnlyMinorCivs())
				{
					// We must not have this already
					if(GET_PLAYER(getOwner()).getNumResourceAvailable(eResource) == 0)
						veValidLuxuryResources.push_back(eResource);
				}
			}
		}
	}

	// Is there actually anything in our vector? - 0 can be valid if we already have everything, for example
	if(veValidLuxuryResources.size() == 0)
	{
		return;
	}

	// Now pick a Luxury we can use
	int iNumAttempts = 0;
	int iVectorLoop;
	int iVectorIndex;
	bool bResourceValid;

	do
	{
		iVectorIndex = GC.getGame().getJonRandNum(veValidLuxuryResources.size(), "Picking random Luxury for City to demand.");
		eResource = (ResourceTypes) veValidLuxuryResources[iVectorIndex];
		bResourceValid = true;

		// Look at all invalid Resources found to see if our randomly-picked Resource matches any
		for(iVectorLoop = 0; iVectorLoop < (int) veInvalidLuxuryResources.size(); iVectorLoop++)
		{
			if(eResource == veInvalidLuxuryResources[iVectorLoop])
			{
				bResourceValid = false;
				break;
			}
		}

		// Not found nearby?
		if(bResourceValid)
		{
			SetResourceDemanded(eResource);

			// Notification
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if(pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_RESOURCE_DEMAND");
				strText << getNameKey() << GC.getResourceInfo(eResource)->GetTextKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_RESOURCE_DEMAND");
				strSummary << getNameKey() << GC.getResourceInfo(eResource)->GetTextKey();
				pNotifications->Add(NOTIFICATION_REQUEST_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), eResource);
			}

			return;
		}

		iNumAttempts++;
	}
	while(iNumAttempts < 500);

	// If we're on the debug map it's too small for us to care
	if(GC.getMap().getWorldSize() != WORLDSIZE_DEBUG)
	{
		CvAssertMsg(false, "Gameplay: Didn't find a Luxury for City to demand.");
	}
}

//	--------------------------------------------------------------------------------
/// Checks to see if we have the Resource demanded and if so starts WLTKD in this City
void CvCity::DoTestResourceDemanded()
{
	VALIDATE_OBJECT
	ResourceTypes eResource = GetResourceDemanded();

	if(GetWeLoveTheKingDayCounter() > 0)
	{
		ChangeWeLoveTheKingDayCounter(-1);

		// WLTKD over!
		if(GetWeLoveTheKingDayCounter() == 0)
		{
			DoPickResourceDemanded();

			if(getOwner() == GC.getGame().getActivePlayer())
			{
				Localization::String localizedText;
				// Know what the next Demanded Resource is
				if(GetResourceDemanded() != NO_RESOURCE)
				{
					localizedText = Localization::Lookup("TXT_KEY_MISC_CITY_WLTKD_ENDED_KNOWN_RESOURCE");
					localizedText << getNameKey() << GC.getResourceInfo(GetResourceDemanded())->GetTextKey();
				}
				// Don't know what the next Demanded Resource is
				else
				{
					localizedText = Localization::Lookup("TXT_KEY_MISC_CITY_WLTKD_ENDED_UNKNOWN_RESOURCE");
					localizedText << getNameKey();
				}

				DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());
			}
		}
	}
	else
	{
		if(eResource != NO_RESOURCE)
		{
			// Do we have the right Resource?
			if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource) > 0)
			{
				SetWeLoveTheKingDayCounter(/*20*/ GC.getCITY_RESOURCE_WLTKD_TURNS());

				CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
				if(pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD");
					strText << GC.getResourceInfo(eResource)->GetTextKey() << getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD");
					strSummary << getNameKey();
					pNotifications->Add(NOTIFICATION_REQUEST_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), eResource);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Figure out how long it should be before this City demands a Resource
void CvCity::DoSeedResourceDemandedCountdown()
{
	VALIDATE_OBJECT

	int iNumTurns = /*15*/ GC.getRESOURCE_DEMAND_COUNTDOWN_BASE();

	if(isCapital())
	{
		iNumTurns += /*25*/ GC.getRESOURCE_DEMAND_COUNTDOWN_CAPITAL_ADD();
	}

	int iRand = /*10*/ GC.getRESOURCE_DEMAND_COUNTDOWN_RAND();
	iNumTurns += GC.getGame().getJonRandNum(iRand, "City Resource demanded rand.");

	SetResourceDemandedCountdown(iNumTurns);
}

//	--------------------------------------------------------------------------------
/// How long before we pick a Resource to demand
int CvCity::GetResourceDemandedCountdown() const
{
	VALIDATE_OBJECT
	return m_iDemandResourceCounter;
}

//	--------------------------------------------------------------------------------
/// How long before we pick a Resource to demand
void CvCity::SetResourceDemandedCountdown(int iValue)
{
	VALIDATE_OBJECT
	m_iDemandResourceCounter = iValue;
}

//	--------------------------------------------------------------------------------
/// How long before we pick a Resource to demand
void CvCity::ChangeResourceDemandedCountdown(int iChange)
{
	VALIDATE_OBJECT
	SetResourceDemandedCountdown(GetResourceDemandedCountdown() + iChange);
}

//	--------------------------------------------------------------------------------
int CvCity::getFoodTurnsLeft() const
{
	VALIDATE_OBJECT
	int iFoodLeft;
	int iTurnsLeft;

	iFoodLeft = (growthThreshold() * 100 - getFoodTimes100());

	if(foodDifferenceTimes100() <= 0)
	{
		return iFoodLeft;
	}

	iTurnsLeft = (iFoodLeft / foodDifferenceTimes100());

	if((iTurnsLeft * foodDifferenceTimes100()) <  iFoodLeft)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}


//	--------------------------------------------------------------------------------
bool CvCity::isProduction() const
{
	VALIDATE_OBJECT
	return (headOrderQueueNode() != NULL);
}


//	--------------------------------------------------------------------------------
bool CvCity::isProductionLimited() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)(pOrderNode->iData1));
			if(pkUnitInfo)
			{
				return isLimitedUnitClass((UnitClassTypes)(pkUnitInfo->GetUnitClassType()));
			}
		}
		break;

		case ORDER_CONSTRUCT:
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo((BuildingTypes)pOrderNode->iData1);
			if(pkBuildingInfo)
			{
				return isLimitedWonderClass(pkBuildingInfo->GetBuildingClassInfo());
			}
		}
		break;

		case ORDER_CREATE:
			return isLimitedProject((ProjectTypes)(pOrderNode->iData1));
			break;

		case ORDER_PREPARE:
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isProductionUnit() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		return (pOrderNode->eOrderType == ORDER_TRAIN);
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isProductionBuilding() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		return (pOrderNode->eOrderType == ORDER_CONSTRUCT);
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isProductionProject() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		return (pOrderNode->eOrderType == ORDER_CREATE);
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isProductionSpecialist() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		return (pOrderNode->eOrderType == ORDER_PREPARE);
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isProductionProcess() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		return (pOrderNode->eOrderType == ORDER_MAINTAIN);
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::canContinueProduction(OrderData order)
{
	VALIDATE_OBJECT
	switch(order.eOrderType)
	{
	case ORDER_TRAIN:
		return canTrain((UnitTypes)(order.iData1), true);
		break;

	case ORDER_CONSTRUCT:
		return canConstruct((BuildingTypes)(order.iData1), true);
		break;

	case ORDER_CREATE:
		return canCreate((ProjectTypes)(order.iData1), true);
		break;

	case ORDER_PREPARE:
		return canPrepare((SpecialistTypes)(order.iData1), true);
		break;

	case ORDER_MAINTAIN:
		return canMaintain((ProcessTypes)(order.iData1), true);
		break;

	default:
		CvAssertMsg(false, "order.eOrderType failed to match a valid option");
		break;
	}

	return false;
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionExperience(UnitTypes eUnit)
{
	VALIDATE_OBJECT
	int iExperience;

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	iExperience = getFreeExperience();
	iExperience += kOwner.getFreeExperience();

	if(eUnit != NO_UNIT)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			if(pkUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT)
			{
				iExperience += getUnitCombatFreeExperience((UnitCombatTypes)(pkUnitInfo->GetUnitCombatType()));
				iExperience += kOwner.getUnitCombatFreeExperiences((UnitCombatTypes) pkUnitInfo->GetUnitCombatType());
			}
			iExperience += getDomainFreeExperience((DomainTypes)(pkUnitInfo->GetDomainType()));

			iExperience += getSpecialistFreeExperience();
		}
	}

	return std::max(0, iExperience);
}


//	--------------------------------------------------------------------------------
void CvCity::addProductionExperience(CvUnit* pUnit, bool bConscript)
{
	VALIDATE_OBJECT

	if(pUnit->canAcquirePromotionAny())
	{
		pUnit->changeExperience(getProductionExperience(pUnit->getUnitType()) / ((bConscript) ? 2 : 1));
	}

	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			if(isFreePromotion(ePromotion))
			{
				if((pUnit->getUnitCombatType() != NO_UNITCOMBAT) && pkPromotionInfo->GetUnitCombatClass(pUnit->getUnitCombatType()))
				{
					pUnit->setHasPromotion(ePromotion, true);
				}
			}
		}
	}

	pUnit->testPromotionReady();
}


//	--------------------------------------------------------------------------------
UnitTypes CvCity::getProductionUnit() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitTypes)(pOrderNode->iData1));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_PREPARE:
		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNIT;
}


//	--------------------------------------------------------------------------------
UnitAITypes CvCity::getProductionUnitAI() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitAITypes)(pOrderNode->iData2));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_PREPARE:
		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNITAI;
}


//	--------------------------------------------------------------------------------
BuildingTypes CvCity::getProductionBuilding() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			break;

		case ORDER_CONSTRUCT:
			return ((BuildingTypes)(pOrderNode->iData1));
			break;

		case ORDER_CREATE:
		case ORDER_PREPARE:
		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_BUILDING;
}


//	--------------------------------------------------------------------------------
ProjectTypes CvCity::getProductionProject() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
			break;

		case ORDER_CREATE:
			return ((ProjectTypes)(pOrderNode->iData1));
			break;

		case ORDER_PREPARE:
		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_PROJECT;
}


//	--------------------------------------------------------------------------------
SpecialistTypes CvCity::getProductionSpecialist() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
			break;

		case ORDER_PREPARE:
			return ((SpecialistTypes)(pOrderNode->iData1));
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_SPECIALIST;
}

//	--------------------------------------------------------------------------------
ProcessTypes CvCity::getProductionProcess() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_PREPARE:
			break;

		case ORDER_MAINTAIN:
			return ((ProcessTypes)(pOrderNode->iData1));
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_PROCESS;
}


//	--------------------------------------------------------------------------------
const char* CvCity::getProductionName() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)pOrderNode->iData1);
			if(pkUnitInfo)
			{
				return pkUnitInfo->GetDescription();
			}
		}
		break;

		case ORDER_CONSTRUCT:
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo((BuildingTypes)pOrderNode->iData1);
			if(pkBuildingInfo)
			{
				return pkBuildingInfo->GetDescription();
			}
		}
		break;

		case ORDER_CREATE:
		{
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo((ProjectTypes)pOrderNode->iData1);
			if(pkProjectInfo)
			{
				return pkProjectInfo->GetDescription();
			}
		}
		break;

		case ORDER_PREPARE:
		{
			CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo((SpecialistTypes)pOrderNode->iData1);
			if(pkSpecialistInfo)
			{
				return pkSpecialistInfo->GetDescription();
			}
		}
		break;

		case ORDER_MAINTAIN:
		{
			CvProcessInfo* pkProcessInfo = GC.getProcessInfo((ProcessTypes)pOrderNode->iData1);
			if(pkProcessInfo)
			{
				return pkProcessInfo->GetDescription();
			}
		}
		break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return "";
}


//	--------------------------------------------------------------------------------
int CvCity::getGeneralProductionTurnsLeft() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft((UnitTypes)pOrderNode->iData1, 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft((BuildingTypes)pOrderNode->iData1, 0);
			break;

		case ORDER_CREATE:
			return getProductionTurnsLeft((ProjectTypes)pOrderNode->iData1, 0);
			break;

		case ORDER_PREPARE:
			return getProductionTurnsLeft((SpecialistTypes)pOrderNode->iData1, 0);
			break;

		case ORDER_MAINTAIN:
			return 0;
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


//	--------------------------------------------------------------------------------
const char* CvCity::getProductionNameKey() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)pOrderNode->iData1);
			if(pkUnitInfo)
			{
				return pkUnitInfo->GetTextKey();
			}
		}
		break;

		case ORDER_CONSTRUCT:
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo((BuildingTypes)pOrderNode->iData1);
			if(pkBuildingInfo)
			{
				return pkBuildingInfo->GetTextKey();
			}
		}
		break;

		case ORDER_CREATE:
		{
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo((ProjectTypes)pOrderNode->iData1);
			if(pkProjectInfo)
			{
				return pkProjectInfo->GetTextKey();
			}
		}
		break;

		case ORDER_PREPARE:
		{
			CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo((SpecialistTypes)pOrderNode->iData1);
			if(pkSpecialistInfo)
			{
				return pkSpecialistInfo->GetTextKey();
			}
		}
		break;

		case ORDER_MAINTAIN:
		{
			CvProcessInfo* pkProcessInfo = GC.getProcessInfo((ProcessTypes)pOrderNode->iData1);
			if(pkProcessInfo)
			{
				return pkProcessInfo->GetTextKey();
			}
		}
		break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return "";
}


//	--------------------------------------------------------------------------------
bool CvCity::isFoodProduction() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return isFoodProduction((UnitTypes)(pOrderNode->iData1));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_PREPARE:
		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvCity::isFoodProduction(UnitTypes eUnit) const
{
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
		return false;

	if(pkUnitInfo->IsFoodProduction())
	{
		return true;
	}

	if(GET_PLAYER(getOwner()).isMilitaryFoodProduction())
	{
		if(pkUnitInfo->IsMilitaryProduction())
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
int CvCity::getFirstUnitOrder(UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	int iCount = 0;

	const OrderData* pOrderNode = headOrderQueueNode();

	while(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_TRAIN)
		{
			if(pOrderNode->iData1 == eUnit)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


//	--------------------------------------------------------------------------------
int CvCity::getFirstBuildingOrder(BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT
	int iCount = 0;

	const OrderData* pOrderNode = headOrderQueueNode();

	while(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_CONSTRUCT)
		{
			if(pOrderNode->iData1 == eBuilding)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


//	--------------------------------------------------------------------------------
int CvCity::getFirstProjectOrder(ProjectTypes eProject) const
{
	VALIDATE_OBJECT
	int iCount = 0;

	const OrderData* pOrderNode = headOrderQueueNode();

	while(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_CREATE)
		{
			if(pOrderNode->iData1 == eProject)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


//	--------------------------------------------------------------------------------
int CvCity::getFirstSpecialistOrder(SpecialistTypes eSpecialist) const
{
	VALIDATE_OBJECT
	int iCount = 0;

	const OrderData* pOrderNode = headOrderQueueNode();

	while(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_PREPARE)
		{
			if(pOrderNode->iData1 == eSpecialist)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}

//	--------------------------------------------------------------------------------
int CvCity::getNumTrainUnitAI(UnitAITypes eUnitAI) const
{
	VALIDATE_OBJECT
	int iCount = 0;

	const OrderData* pOrderNode = headOrderQueueNode();

	while(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_TRAIN)
		{
			if(pOrderNode->iData2 == eUnitAI)
			{
				iCount++;
			}
		}

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return iCount;
}


//	--------------------------------------------------------------------------------
int CvCity::getProduction() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return getUnitProduction((UnitTypes)(pOrderNode->iData1));
			break;

		case ORDER_CONSTRUCT:
			return m_pCityBuildings->GetBuildingProduction((BuildingTypes)(pOrderNode->iData1));
			break;

		case ORDER_CREATE:
			return getProjectProduction((ProjectTypes)(pOrderNode->iData1));
			break;

		case ORDER_PREPARE:
			return getSpecialistProduction((SpecialistTypes)(pOrderNode->iData1));
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionTimes100() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return getUnitProductionTimes100((UnitTypes)(pOrderNode->iData1));
			break;

		case ORDER_CONSTRUCT:
			return m_pCityBuildings->GetBuildingProductionTimes100((BuildingTypes)(pOrderNode->iData1));
			break;

		case ORDER_CREATE:
			return getProjectProductionTimes100((ProjectTypes)(pOrderNode->iData1));
			break;

		case ORDER_PREPARE:
			return getSpecialistProductionTimes100((SpecialistTypes)(pOrderNode->iData1));
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionNeeded() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionNeeded((UnitTypes)(pOrderNode->iData1));
			break;

		case ORDER_CONSTRUCT:
			return getProductionNeeded((BuildingTypes)(pOrderNode->iData1));
			break;

		case ORDER_CREATE:
			return getProductionNeeded((ProjectTypes)(pOrderNode->iData1));
			break;

		case ORDER_PREPARE:
			return getProductionNeeded((SpecialistTypes)(pOrderNode->iData1));
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return INT_MAX;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionNeeded(UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	int iNumProductionNeeded = GET_PLAYER(getOwner()).getProductionNeeded(eUnit);

	return iNumProductionNeeded;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionNeeded(BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT
	int iNumProductionNeeded = GET_PLAYER(getOwner()).getProductionNeeded(eBuilding);

	return iNumProductionNeeded;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionNeeded(ProjectTypes eProject) const
{
	VALIDATE_OBJECT
	int iNumProductionNeeded = GET_PLAYER(getOwner()).getProductionNeeded(eProject);

	return iNumProductionNeeded;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionNeeded(SpecialistTypes eSpecialist) const
{
	VALIDATE_OBJECT
	int iNumProductionNeeded = GET_PLAYER(getOwner()).getProductionNeeded(eSpecialist);

	return iNumProductionNeeded;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionTurnsLeft() const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft(((UnitTypes)(pOrderNode->iData1)), 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft(((BuildingTypes)(pOrderNode->iData1)), 0);
			break;

		case ORDER_CREATE:
			return getProductionTurnsLeft(((ProjectTypes)(pOrderNode->iData1)), 0);
			break;

		case ORDER_PREPARE:
			return getProductionTurnsLeft(((SpecialistTypes)(pOrderNode->iData1)), 0);
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return INT_MAX;
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionTurnsLeft(UnitTypes eUnit, int iNum) const
{
	VALIDATE_OBJECT
	int iProduction;
	int iFirstUnitOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstUnitOrder = getFirstUnitOrder(eUnit);

	if((iFirstUnitOrder == -1) || (iFirstUnitOrder == iNum))
	{
		iProduction += getUnitProductionTimes100(eUnit);
	}

	iProductionNeeded = getProductionNeeded(eUnit) * 100;
	iProductionModifier = getProductionModifier(eUnit);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, isFoodProduction(eUnit), (iNum == 0)), getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, isFoodProduction(eUnit), false));
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const
{
	VALIDATE_OBJECT
	int iProduction;
	int iFirstBuildingOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstBuildingOrder = getFirstBuildingOrder(eBuilding);

	if((iFirstBuildingOrder == -1) || (iFirstBuildingOrder == iNum))
	{
		iProduction += m_pCityBuildings->GetBuildingProductionTimes100(eBuilding);
	}

	iProductionNeeded = getProductionNeeded(eBuilding) * 100;

	iProductionModifier = getProductionModifier(eBuilding);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, false, (iNum == 0)), getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, false, false));
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionTurnsLeft(ProjectTypes eProject, int iNum) const
{
	VALIDATE_OBJECT
	int iProduction;
	int iFirstProjectOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstProjectOrder = getFirstProjectOrder(eProject);

	if((iFirstProjectOrder == -1) || (iFirstProjectOrder == iNum))
	{
		iProduction += getProjectProductionTimes100(eProject);
	}

	iProductionNeeded = getProductionNeeded(eProject) * 100;
	iProductionModifier = getProductionModifier(eProject);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, false, (iNum == 0)), getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, false, false));
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionTurnsLeft(SpecialistTypes eSpecialist, int iNum) const
{
	VALIDATE_OBJECT
	int iProduction;
	int iFirstSpecialistOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstSpecialistOrder = getFirstSpecialistOrder(eSpecialist);

	if((iFirstSpecialistOrder == -1) || (iFirstSpecialistOrder == iNum))
	{
		iProduction += getSpecialistProductionTimes100(eSpecialist);
	}

	iProductionNeeded = getProductionNeeded(eSpecialist) * 100;
	iProductionModifier = getProductionModifier(eSpecialist);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, false, (iNum == 0)), getProductionDifferenceTimes100(iProductionNeeded, iProduction, iProductionModifier, false, false));
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const
{
	VALIDATE_OBJECT
	int iProductionLeft;
	int iTurnsLeft;

	iProductionLeft = std::max(0, (iProductionNeeded - iProduction - iFirstProductionDifference));

	if(iProductionDifference == 0)
	{
		return iProductionLeft + 1;
	}

	iTurnsLeft = (iProductionLeft / iProductionDifference);

	if((iTurnsLeft * iProductionDifference) < iProductionLeft)
	{
		iTurnsLeft++;
	}

	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}

//	--------------------------------------------------------------------------------
int CvCity::GetPurchaseCost(UnitTypes eUnit)
{
	VALIDATE_OBJECT

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		//Should never happen
		return 0;
	}

	int iModifier = pkUnitInfo->GetHurryCostModifier();

	if(iModifier == -1)
		return -1;

	int iCost = GetPurchaseCostFromProduction(getProductionNeeded(eUnit));
	iCost *= (100 + iModifier);
	iCost /= 100;

	// Cost of purchasing units modified?
	iCost *= (100 + GET_PLAYER(getOwner()).GetUnitPurchaseCostModifier());
	iCost /= 100;

	// Make the number not be funky
	int iDivisor = /*10*/ GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPurchaseCost(UnitTypes eUnit, bool bIncludeBeliefDiscounts)
{
	VALIDATE_OBJECT

	int iCost = 0;
	CvPlayer &kPlayer = GET_PLAYER(m_eOwner);

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		//Should never happen
		return iCost;
	}

	// LATE-GAME GREAT PERSON
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");
	if (pkUnitInfo->GetSpecialUnitType() == eSpecialUnitGreatPerson)
	{
		// We must be into the industrial era
		if(kPlayer.GetCurrentEra() >= GC.getInfoTypeForString("ERA_INDUSTRIAL", true /*bHideAssert*/))
		{
			// Must be proper great person for our civ
			const UnitClassTypes eUnitClass = (UnitClassTypes)pkUnitInfo->GetUnitClassType();
			if (eUnitClass != NO_UNITCLASS)
			{
				const UnitTypes eThisPlayersUnitType = (UnitTypes)kPlayer.getCivilizationInfo().getCivilizationUnits(eUnitClass);

				if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_PROPHET", true /*bHideAssert*/)) //here
				{
					// Can't be bought if didn't start religion
					if (kPlayer.GetReligions()->GetReligionCreatedByPlayer() == NO_RELIGION)
					{
						iCost = -1;
					}
					else
					{
						iCost = kPlayer.GetReligions()->GetCostNextProphet(true /*bIncludeBeliefDiscounts*/, false /*bAdjustForSpeedDifficulty*/);
					}
				}
				else if (eThisPlayersUnitType == eUnit)
				{
					PolicyBranchTypes eBranch = NO_POLICY_BRANCH_TYPE;
					int iNum = 0;

					// Check social policy tree
					if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST", true /*bHideAssert*/))
					{
						eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_FREEDOM", true /*bHideAssert*/);
						iNum = kPlayer.getArtistsFromFaith();
					}
					else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST", true /*bHideAssert*/))
					{
						eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_RATIONALISM", true /*bHideAssert*/);
						iNum = kPlayer.getScientistsFromFaith();
					}
					else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT", true /*bHideAssert*/))
					{
						eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_COMMERCE", true /*bHideAssert*/);
						iNum = kPlayer.getMerchantsFromFaith();
					}
					else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER", true /*bHideAssert*/))
					{
						eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_ORDER", true /*bHideAssert*/);
						iNum = kPlayer.getEngineersFromFaith();
					}
					else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_GENERAL", true /*bHideAssert*/))
					{
						eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_AUTOCRACY", true /*bHideAssert*/);
						iNum = kPlayer.getGeneralsFromFaith();
					}
					else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_ADMIRAL", true /*bHideAssert*/))
					{
						eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_AUTOCRACY", true /*bHideAssert*/);
						iNum = kPlayer.getAdmiralsFromFaith();
					}

					if (eBranch != NO_POLICY_BRANCH_TYPE && kPlayer.GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch) && !kPlayer.GetPlayerPolicies()->IsPolicyBranchBlocked(eBranch))
					{
						iCost = GC.getGame().GetGameReligions()->GetFaithGreatPersonNumber(iNum + 1);	
					}
				}
			}
		}
	}

	// ALL OTHERS
	else
	{
		// Cost goes up in later eras
		iCost = pkUnitInfo->GetFaithCost();
		EraTypes eEra = GET_TEAM(GET_PLAYER(getOwner()).getTeam()).GetCurrentEra();
		int iMultiplier = GC.getEraInfo(eEra)->getFaithCostMultiplier();
		iCost = iCost * iMultiplier / 100;

		if (pkUnitInfo->IsSpreadReligion() || pkUnitInfo->IsRemoveHeresy())
		{
			iMultiplier = (100 + GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_FAITH_COST_MODIFIER));
			iCost = iCost * iMultiplier / 100;
		}
	}

	// Adjust for game speed
	iCost *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iCost /= 100;

	// Adjust for difficulty
	if(!isHuman() && !GET_PLAYER(getOwner()).IsAITeammateOfHuman() && !isBarbarian())
	{
		iCost *= GC.getGame().getHandicapInfo().getAITrainPercent();
		iCost /= 100;
	}

	// Modify by any beliefs
	if(bIncludeBeliefDiscounts && (pkUnitInfo->IsSpreadReligion() || pkUnitInfo->IsRemoveHeresy()) && !pkUnitInfo->IsFoundReligion())
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
		if(eMajority > RELIGION_PANTHEON)
		{
			const CvReligion* pReligion = pReligions->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				int iReligionCostMod = pReligion->m_Beliefs.GetMissionaryCostModifier();

				if(iReligionCostMod != 0)
				{
					iCost *= (100 + iReligionCostMod);
					iCost /= 100;
				}
			}
		}
	}

	// Make the number not be funky
	int iDivisor = /*10*/ GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

//	--------------------------------------------------------------------------------
int CvCity::GetPurchaseCost(BuildingTypes eBuilding)
{
	VALIDATE_OBJECT

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return -1;

	int iModifier = pkBuildingInfo->GetHurryCostModifier();

	if(iModifier == -1)
		return -1;

	int iCost = GetPurchaseCostFromProduction(getProductionNeeded(eBuilding));
	iCost *= (100 + iModifier);
	iCost /= 100;

	// Make the number not be funky
	int iDivisor = /*10*/ GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPurchaseCost(BuildingTypes eBuilding)
{
	int iCost;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		//Should never happen
		return 0;
	}

	// Cost goes up in later eras
	iCost = pkBuildingInfo->GetFaithCost();
	EraTypes eEra = GET_TEAM(GET_PLAYER(getOwner()).getTeam()).GetCurrentEra();
	int iMultiplier = GC.getEraInfo(eEra)->getFaithCostMultiplier();
	iCost = iCost * iMultiplier / 100;
	iMultiplier = (100 + GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_FAITH_COST_MODIFIER));
	iCost = iCost * iMultiplier / 100;

	// Adjust for game speed
	iCost *= GC.getGame().getGameSpeedInfo().getConstructPercent();
	iCost /= 100;

	// Adjust for difficulty
	if(!isHuman() && !GET_PLAYER(getOwner()).IsAITeammateOfHuman() && !isBarbarian())
	{
		iCost *= GC.getGame().getHandicapInfo().getAIConstructPercent();
		iCost /= 100;
	}

	// Make the number not be funky
	int iDivisor = /*10*/ GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

//	--------------------------------------------------------------------------------
int CvCity::GetPurchaseCost(ProjectTypes eProject)
{
	VALIDATE_OBJECT

	int iCost = GetPurchaseCostFromProduction(getProductionNeeded(eProject));

	// Make the number not be funky
	int iDivisor = /*10*/ GC.getGOLD_PURCHASE_VISIBLE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

//	--------------------------------------------------------------------------------
/// Cost of Purchasing something based on the amount of Production it requires to construct
int CvCity::GetPurchaseCostFromProduction(int iProduction)
{
	VALIDATE_OBJECT
	int iPurchaseCost;

	// Gold per Production
	int iPurchaseCostBase = iProduction* /*30*/ GC.getGOLD_PURCHASE_GOLD_PER_PRODUCTION();
	// Cost ramps up
	iPurchaseCost = (int) pow((double) iPurchaseCostBase, (double) /*0.75f*/ GC.getHURRY_GOLD_PRODUCTION_EXPONENT());

	// Hurry Mod (Policies, etc.)
	HurryTypes eHurry = (HurryTypes) GC.getInfoTypeForString("HURRY_GOLD");

	if(eHurry != NO_HURRY)
	{
		int iHurryMod = GET_PLAYER(getOwner()).getHurryModifier(eHurry);

		if(iHurryMod != 0)
		{
			iPurchaseCost *= (100 + iHurryMod);
			iPurchaseCost /= 100;
		}
	}

	// Game Speed modifier
	iPurchaseCost *= GC.getGame().getGameSpeedInfo().getHurryPercent();
	iPurchaseCost /= 100;

	return iPurchaseCost;
}

//	--------------------------------------------------------------------------------
void CvCity::setProduction(int iNewValue)
{
	VALIDATE_OBJECT
	if(isProductionUnit())
	{
		setUnitProduction(getProductionUnit(), iNewValue);
	}
	else if(isProductionBuilding())
	{
		m_pCityBuildings->SetBuildingProduction(getProductionBuilding(), iNewValue);
	}
	else if(isProductionProject())
	{
		setProjectProduction(getProductionProject(), iNewValue);
	}
	else if(isProductionSpecialist())
	{
		setSpecialistProduction(getProductionSpecialist(), iNewValue);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::changeProduction(int iChange)
{
	VALIDATE_OBJECT
	if(isProductionUnit())
	{
		changeUnitProduction(getProductionUnit(), iChange);
	}
	else if(isProductionBuilding())
	{
		m_pCityBuildings->ChangeBuildingProduction(getProductionBuilding(), iChange);
	}
	else if(isProductionProject())
	{
		changeProjectProduction(getProductionProject(), iChange);
	}
	else if(isProductionSpecialist())
	{
		changeSpecialistProduction(getProductionSpecialist(), iChange);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::setProductionTimes100(int iNewValue)
{
	VALIDATE_OBJECT
	if(isProductionUnit())
	{
		setUnitProductionTimes100(getProductionUnit(), iNewValue);
	}
	else if(isProductionBuilding())
	{
		m_pCityBuildings->SetBuildingProductionTimes100(getProductionBuilding(), iNewValue);
	}
	else if(isProductionProject())
	{
		setProjectProductionTimes100(getProductionProject(), iNewValue);
	}
	else if(isProductionSpecialist())
	{
		setSpecialistProductionTimes100(getProductionSpecialist(), iNewValue);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::changeProductionTimes100(int iChange)
{
	VALIDATE_OBJECT
	if(isProductionUnit())
	{
		changeUnitProductionTimes100(getProductionUnit(), iChange);
	}
	else if(isProductionBuilding())
	{
		m_pCityBuildings->ChangeBuildingProductionTimes100(getProductionBuilding(), iChange);
	}
	else if(isProductionProject())
	{
		changeProjectProductionTimes100(getProductionProject(), iChange);
	}
	else if(isProductionSpecialist())
	{
		changeSpecialistProductionTimes100(getProductionSpecialist(), iChange);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionModifier(CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	const OrderData* pOrderNode = headOrderQueueNode();

	int iMultiplier = 0;

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			iMultiplier += getProductionModifier((UnitTypes)(pOrderNode->iData1), toolTipSink);
			break;

		case ORDER_CONSTRUCT:
			iMultiplier += getProductionModifier((BuildingTypes)(pOrderNode->iData1), toolTipSink);
			break;

		case ORDER_CREATE:
			iMultiplier += getProductionModifier((ProjectTypes)(pOrderNode->iData1), toolTipSink);
			break;

		case ORDER_PREPARE:
			iMultiplier += getProductionModifier((SpecialistTypes)(pOrderNode->iData1), toolTipSink);
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType failed to match a valid option");
			break;
		}
	}

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvCity::getGeneralProductionModifiers(CvString* toolTipSink) const
{
	int iMultiplier = 0;

	// Railroad to capital?
	if(IsIndustrialRouteToCapital())
	{
		const int iTempMod = GC.getINDUSTRIAL_ROUTE_PRODUCTION_MOD();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_RAILROAD_CONNECTION", iTempMod);
		}
	}

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionModifier(UnitTypes eUnit, CvString* toolTipSink) const
{
	VALIDATE_OBJECT

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		//Unit type doesn't exist!
		return 0;
	}

	CvPlayerAI& thisPlayer = GET_PLAYER(getOwner());

	int iMultiplier = getGeneralProductionModifiers(toolTipSink);

	iMultiplier += thisPlayer.getProductionModifier(eUnit, toolTipSink);

	int iTempMod;

	// Capital Settler bonus
	if(isCapital() && pkUnitInfo->IsFound())
	{
		iTempMod = GET_PLAYER(getOwner()).getCapitalSettlerProductionModifier();
		iMultiplier += iTempMod;
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_CAPITAL_SETTLER_PLAYER", iTempMod);
	}

	// Domain bonus
	iTempMod = getDomainProductionModifier((DomainTypes)(pkUnitInfo->GetDomainType()));
	iMultiplier += iTempMod;
	if(toolTipSink && iTempMod)
	{
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_DOMAIN", iTempMod);
	}

	// UnitCombat class bonus
	UnitCombatTypes eUnitCombatType = (UnitCombatTypes)(pkUnitInfo->GetUnitCombatType());
	if(eUnitCombatType != NO_UNITCOMBAT)
	{
		iTempMod = getUnitCombatProductionModifier(eUnitCombatType);
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_COMBAT_TYPE", iTempMod);
		}
	}

	// Military production bonus
	if(pkUnitInfo->IsMilitaryProduction())
	{
		iTempMod = getMilitaryProductionModifier();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_MILITARY", iTempMod);
		}
	}

	// City Space mod
	if(pkUnitInfo->GetSpaceshipProject() != NO_PROJECT)
	{
		iTempMod = getSpaceProductionModifier();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPACE", iTempMod);
		}

		iTempMod = thisPlayer.getSpaceProductionModifier();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPACE_PLAYER", iTempMod);
		}
	}

	// Production bonus for having a particular building
	iTempMod = 0;
	int iBuildingMod = 0;
	BuildingTypes eBuilding;
	for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		eBuilding = (BuildingTypes) iI;
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			if(GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				iTempMod = pkUnitInfo->GetBuildingProductionModifier(eBuilding);

				if(iTempMod != 0)
				{
					iBuildingMod += iTempMod;
					if(toolTipSink && iTempMod)
					{
						GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_UNIT_WITH_BUILDING", iTempMod, pkBuildingInfo->GetDescription());
					}
				}
			}
		}
	}
	if(iBuildingMod != 0)
	{
		iMultiplier += iBuildingMod;
	}

	return iMultiplier;
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionModifier(BuildingTypes eBuilding, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	int iMultiplier = getGeneralProductionModifiers(toolTipSink);
	iMultiplier += GET_PLAYER(getOwner()).getProductionModifier(eBuilding, toolTipSink);

	CvBuildingEntry* thisBuildingEntry = GC.getBuildingInfo(eBuilding);
	if(thisBuildingEntry == NULL)	//should never happen
		return -1;

	const CvBuildingClassInfo& kBuildingClassInfo = thisBuildingEntry->GetBuildingClassInfo();

	int iTempMod;

	// Wonder bonus
	if(::isWorldWonderClass(kBuildingClassInfo) ||
	        ::isTeamWonderClass(kBuildingClassInfo) ||
	        ::isNationalWonderClass(kBuildingClassInfo))
	{
		iTempMod = GetWonderProductionModifier();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_CITY", iTempMod);
		}

		iTempMod = GET_PLAYER(getOwner()).getWonderProductionModifier();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_PLAYER", iTempMod);
		}

		iTempMod = GetLocalResourceWonderProductionMod(eBuilding, toolTipSink);
		iMultiplier += iTempMod;

		ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				// Depends on era of wonder
				EraTypes eEra;
				TechTypes eTech = (TechTypes)thisBuildingEntry->GetPrereqAndTech();
				if(eTech != NO_TECH)
				{
					CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eTech);
					if(pEntry)
					{
						eEra = (EraTypes)pEntry->GetEra();
						if(eEra != NO_ERA)
						{
							iTempMod = pReligion->m_Beliefs.GetWonderProductionModifier(eEra);
							iMultiplier += iTempMod;
							if(toolTipSink && iTempMod)
							{
								GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_RELIGION", iTempMod);
							}
						}
					}
				}
			}
		}
	}
	// Not-wonder bonus
	else
	{
		iTempMod = m_pCityBuildings->GetBuildingProductionModifier();
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_BUILDING_CITY", iTempMod);
		}
	}

	// From policies
	iTempMod = GET_PLAYER(getOwner()).GetPlayerPolicies()->GetBuildingClassProductionModifier((BuildingClassTypes)kBuildingClassInfo.GetID());
	if(iTempMod != 0)
	{
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_BUILDING_POLICY", iTempMod);
		}
	}

	// From traits
	iTempMod = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCapitalBuildingDiscount(eBuilding);
	if(iTempMod != 0)
	{
		iMultiplier += iTempMod;
		if(toolTipSink && iTempMod)
		{
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_CAPITAL_BUILDING_TRAIT", iTempMod);
		}
	}

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionModifier(ProjectTypes eProject, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	int iMultiplier = getGeneralProductionModifiers(toolTipSink);
	iMultiplier += GET_PLAYER(getOwner()).getProductionModifier(eProject, toolTipSink);

	int iTempMod;

	// City Space mod
	if(GC.getProjectInfo(eProject)->IsSpaceship())
	{
		iTempMod = getSpaceProductionModifier();
		iMultiplier += iTempMod;
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_SPACE", iTempMod);
	}

	return iMultiplier;
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionModifier(SpecialistTypes eSpecialist, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	int iMultiplier = getGeneralProductionModifiers(toolTipSink);
	iMultiplier += GET_PLAYER(getOwner()).getProductionModifier(eSpecialist, toolTipSink);

	return iMultiplier;
}

//	--------------------------------------------------------------------------------
int CvCity::getProductionDifference(int /*iProductionNeeded*/, int /*iProduction*/, int iProductionModifier, bool bFoodProduction, bool bOverflow) const
{
	VALIDATE_OBJECT
	// If we're in anarchy, then no Production is done!
	if(GET_PLAYER(getOwner()).IsAnarchy())
	{
		return 0;
	}
	// If we're in Resistance, then no Production is done!
	if(IsResistance() || IsRazing())
	{
		return 0;
	}

	int iFoodProduction = ((bFoodProduction) ? (GetFoodProduction(getYieldRate(YIELD_FOOD) - foodConsumption(true))) : 0);

	int iOverflow = ((bOverflow) ? (getOverflowProduction() + getFeatureProduction()) : 0);

	// Sum up difference
	int iBaseProduction = getBaseYieldRate(YIELD_PRODUCTION) * 100;
	iBaseProduction += (GetYieldPerPopTimes100(YIELD_PRODUCTION) * getPopulation());

	int iModifiedProduction = iBaseProduction * getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier);
	iModifiedProduction /= 10000;

	iModifiedProduction += iOverflow;
	iModifiedProduction += iFoodProduction;

	return iModifiedProduction;

}


//	--------------------------------------------------------------------------------
int CvCity::getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const
{
	VALIDATE_OBJECT
	return getProductionDifference(getProductionNeeded(), getProduction(), getProductionModifier(), (!bIgnoreFood && isFoodProduction()), bOverflow);
}

//	--------------------------------------------------------------------------------
// What is the production of this city, not counting modifiers specific to what we happen to be building?
int CvCity::getRawProductionDifference(bool bIgnoreFood, bool bOverflow) const
{
	VALIDATE_OBJECT
	return getProductionDifference(getProductionNeeded(), getProduction(), getGeneralProductionModifiers(), (!bIgnoreFood && isFoodProduction()), bOverflow);
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionDifferenceTimes100(int /*iProductionNeeded*/, int /*iProduction*/, int iProductionModifier, bool bFoodProduction, bool bOverflow) const
{
	VALIDATE_OBJECT
	// If we're in anarchy, then no Production is done!
	if(GET_PLAYER(getOwner()).IsAnarchy())
	{
		return 0;
	}
	// If we're in Resistance, then no Production is done!
	if(IsResistance() || IsRazing())
	{
		return 0;
	}

	int iFoodProduction = ((bFoodProduction) ? GetFoodProduction(getYieldRate(YIELD_FOOD) - foodConsumption(true)) : 0);
	iFoodProduction *= 100;

	int iOverflow = ((bOverflow) ? (getOverflowProductionTimes100() + getFeatureProduction() * 100) : 0);

	// Sum up difference
	int iBaseProduction = getBaseYieldRate(YIELD_PRODUCTION) * 100;
	iBaseProduction += (GetYieldPerPopTimes100(YIELD_PRODUCTION) * getPopulation());

	int iModifiedProduction = iBaseProduction * getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier);
	iModifiedProduction /= 100;

	iModifiedProduction += iOverflow;
	iModifiedProduction += iFoodProduction;

	return iModifiedProduction;
}


//	--------------------------------------------------------------------------------
int CvCity::getCurrentProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow) const
{
	VALIDATE_OBJECT
	return getProductionDifferenceTimes100(getProductionNeeded(), getProductionTimes100(), getProductionModifier(), (!bIgnoreFood && isFoodProduction()), bOverflow);
}

//	--------------------------------------------------------------------------------
// What is the production of this city, not counting modifiers specific to what we happen to be building?
int CvCity::getRawProductionDifferenceTimes100(bool bIgnoreFood, bool bOverflow) const
{
	VALIDATE_OBJECT
	return getProductionDifferenceTimes100(getProductionNeeded(), getProductionTimes100(), getGeneralProductionModifiers(), (!bIgnoreFood && isFoodProduction()), bOverflow);
}


//	--------------------------------------------------------------------------------
int CvCity::getExtraProductionDifference(int iExtra) const
{
	VALIDATE_OBJECT
	return getExtraProductionDifference(iExtra, getProductionModifier());
}

//	--------------------------------------------------------------------------------
int CvCity::getExtraProductionDifference(int iExtra, UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	return getExtraProductionDifference(iExtra, getProductionModifier(eUnit));
}

//	--------------------------------------------------------------------------------
int CvCity::getExtraProductionDifference(int iExtra, BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT
	return getExtraProductionDifference(iExtra, getProductionModifier(eBuilding));
}

//	--------------------------------------------------------------------------------
int CvCity::getExtraProductionDifference(int iExtra, ProjectTypes eProject) const
{
	VALIDATE_OBJECT
	return getExtraProductionDifference(iExtra, getProductionModifier(eProject));
}

//	--------------------------------------------------------------------------------
int CvCity::getExtraProductionDifference(int iExtra, int iModifier) const
{
	VALIDATE_OBJECT
	return ((iExtra * getBaseYieldRateModifier(YIELD_PRODUCTION, iModifier)) / 100);
}

//	--------------------------------------------------------------------------------
/// Convert extra food to production if building a unit built partially from food
int CvCity::GetFoodProduction(int iExcessFood) const
{
	int iRtnValue;

	if(iExcessFood <= 0)
	{
		iRtnValue = 0;
	}
	else if(iExcessFood <= 2)
	{
		iRtnValue = iExcessFood * 100;
	}
	else if(iExcessFood > 2 && iExcessFood <= 4)
	{
		iRtnValue = 200 + (iExcessFood - 2) * 50;
	}
	else
	{
		iRtnValue = 300 + (iExcessFood - 4) * 25;
	}

	return (iRtnValue / 100);
}

//	--------------------------------------------------------------------------------
bool CvCity::canHurry(HurryTypes eHurry, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(!(GET_PLAYER(getOwner()).IsHasAccessToHurry(eHurry)))
	{
		return false;
	}

	if(getProduction() >= getProductionNeeded())
	{
		return false;
	}

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo == NULL)
		return false;


	// City cannot Hurry Player-level things
	if(pkHurryInfo->getGoldPerBeaker() > 0 || pkHurryInfo->getGoldPerCulture() > 0)
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(!isProductionUnit() && !isProductionBuilding())
		{
			return false;
		}

		if(GET_PLAYER(getOwner()).GetTreasury()->GetGold() < hurryGold(eHurry))
		{
			return false;
		}

		if(maxHurryPopulation() < hurryPopulation(eHurry))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvCity::canHurryUnit(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	if(!(GET_PLAYER(getOwner()).IsHasAccessToHurry(eHurry)))
	{
		return false;
	}

	if(getUnitProduction(eUnit) >= getProductionNeeded(eUnit))
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).GetTreasury()->GetGold() < getHurryGold(eHurry, getHurryCost(eHurry, false, eUnit, bIgnoreNew), getProductionNeeded(eUnit)))
	{
		return false;
	}

	if(maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(eHurry, true, eUnit, bIgnoreNew)))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvCity::canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	if(!(GET_PLAYER(getOwner()).IsHasAccessToHurry(eHurry)))
	{
		return false;
	}

	if(m_pCityBuildings->GetBuildingProduction(eBuilding) >= getProductionNeeded(eBuilding))
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).GetTreasury()->GetGold() < getHurryGold(eHurry, getHurryCost(eHurry, false, eBuilding, bIgnoreNew), getProductionNeeded(eBuilding)))
	{
		return false;
	}

	if(maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(eHurry, true, eBuilding, bIgnoreNew)))
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvCity::hurry(HurryTypes eHurry)
{
	VALIDATE_OBJECT
	int iHurryGold;
	int iHurryPopulation;

	if(!canHurry(eHurry))
	{
		return;
	}

	iHurryGold = hurryGold(eHurry);
	iHurryPopulation = hurryPopulation(eHurry);

	changeProduction(hurryProduction(eHurry));

	GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(-(iHurryGold));
	changePopulation(-(iHurryPopulation));

	if((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
	{
		DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
UnitTypes CvCity::getConscriptUnit() const
{
	VALIDATE_OBJECT
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;

	for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			const UnitTypes eLoopUnit = (UnitTypes)getCivilizationInfo().getCivilizationUnits(iI);
			if(eLoopUnit != NO_UNIT)
			{
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
				if(pkUnitInfo)
				{
					if(canTrain(eLoopUnit))
					{
						const int iValue = pkUnitInfo->GetConscriptionValue();
						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestUnit = eLoopUnit;
						}
					}
				}
			}
		}
	}

	return eBestUnit;
}


//	--------------------------------------------------------------------------------
int CvCity::getConscriptPopulation() const
{
	VALIDATE_OBJECT
	UnitTypes eConscriptUnit = getConscriptUnit();

	if(eConscriptUnit == NO_UNIT)
	{
		return 0;
	}

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eConscriptUnit);
	if(pkUnitInfo == NULL)
	{
		return 0;
	}

	int iConscriptPopulationPerCost = GC.getCONSCRIPT_POPULATION_PER_COST();
	if(iConscriptPopulationPerCost == 0)
	{
		return 0;
	}

	return std::max(1, ((pkUnitInfo->GetProductionCost()) / iConscriptPopulationPerCost));
}


//	--------------------------------------------------------------------------------
int CvCity::conscriptMinCityPopulation() const
{
	VALIDATE_OBJECT
	int iPopulation = GC.getCONSCRIPT_MIN_CITY_POPULATION();

	iPopulation += getConscriptPopulation();

	return iPopulation;
}


//	--------------------------------------------------------------------------------
bool CvCity::canConscript() const
{
	VALIDATE_OBJECT
	if(isDrafted())
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).getConscriptCount() >= GET_PLAYER(getOwner()).getMaxConscript())
	{
		return false;
	}

	if(getPopulation() <= getConscriptPopulation())
	{
		return false;
	}

	if(getPopulation() < conscriptMinCityPopulation())
	{
		return false;
	}

	if(getConscriptUnit() == NO_UNIT)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
CvUnit* CvCity::initConscriptedUnit()
{
	VALIDATE_OBJECT
	UnitAITypes eCityAI = NO_UNITAI;
	UnitTypes eConscriptUnit = getConscriptUnit();

	if(NO_UNIT == eConscriptUnit)
	{
		return NULL;
	}

	CvUnit* pUnit = GET_PLAYER(getOwner()).initUnit(eConscriptUnit, getX(), getY(), eCityAI);
	CvAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	if(NULL != pUnit)
	{
		addProductionExperience(pUnit, true);

		pUnit->setMoves(pUnit->maxMoves());
	}

	return pUnit;
}


//	--------------------------------------------------------------------------------
void CvCity::conscript()
{
	VALIDATE_OBJECT
	if(!canConscript())
	{
		return;
	}

	changePopulation(-(getConscriptPopulation()));

	setDrafted(true);

	GET_PLAYER(getOwner()).changeConscriptCount(1);

	CvUnit* pUnit = initConscriptedUnit();
	CvAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	if(NULL != pUnit)
	{
		if(GC.getGame().getActivePlayer() == getOwner())
		{
			auto_ptr<ICvUnit1> pDllUnit = GC.WrapUnitPointer(pUnit);
			DLLUI->selectUnit(pDllUnit.get(), true, false, true);
		}
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getResourceYieldRateModifier(YieldTypes eIndex, ResourceTypes eResource) const
{
	VALIDATE_OBJECT
	int iModifier = 0;

	for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			iModifier += m_pCityBuildings->GetNumActiveBuilding(eBuilding) * pkBuildingInfo->GetResourceYieldModifier(eResource, eIndex);
		}
	}

	return iModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::processResource(ResourceTypes eResource, int iChange)
{
	VALIDATE_OBJECT

	// Yield modifier for having a local resource
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		const YieldTypes eYield = static_cast<YieldTypes>(iI);
		changeResourceYieldRateModifier(eYield, (getResourceYieldRateModifier(eYield, eResource) * iChange));
	}
}


//	--------------------------------------------------------------------------------
void CvCity::processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst, bool bObsolete, bool /*bApplyingAllCitiesBonus*/)
{
	VALIDATE_OBJECT

	CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);

	if(pBuildingInfo == NULL)
		return;

	BuildingClassTypes eBuildingClass = (BuildingClassTypes) pBuildingInfo->GetBuildingClassType();

	CvPlayer& owningPlayer = GET_PLAYER(getOwner());
	CvTeam& owningTeam = GET_TEAM(getTeam());
	CvCivilizationInfo& thisCiv = getCivilizationInfo();

	if(!(owningTeam.isObsoleteBuilding(eBuilding)) || bObsolete)
	{
		// One-shot items
		if(bFirst && iChange > 0)
		{
			// Capital
			if(pBuildingInfo->IsCapital())
				owningPlayer.setCapitalCity(this);

			// Free Units
			CvUnit* pFreeUnit;

			int iFreeUnitLoop;

			for(int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
			{
				const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
				if(pkUnitInfo)
				{
					for(iFreeUnitLoop = 0; iFreeUnitLoop < pBuildingInfo->GetNumFreeUnits(iUnitLoop); iFreeUnitLoop++)
					{
						// Get the right unit of this class for this civ
						const UnitTypes eFreeUnitType = (UnitTypes)thisCiv.getCivilizationUnits((UnitClassTypes)pkUnitInfo->GetUnitClassType());

						// Great prophet?
						if(GC.GetGameUnits()->GetEntry(eFreeUnitType)->IsFoundReligion())
						{
							GetCityCitizens()->DoSpawnGreatPerson(eFreeUnitType, true /*bIncrementCount*/, true);
						}
						else
						{
							pFreeUnit = owningPlayer.initUnit(eFreeUnitType, getX(), getY());

							// Bump up the count
							if(pFreeUnit->IsGreatGeneral())
							{
								owningPlayer.incrementGreatGeneralsCreated();
								if (!pFreeUnit->jumpToNearestValidPlot())
									pFreeUnit->kill(false);	// Could not find a valid spot!
							}
							else if(pFreeUnit->IsGreatAdmiral())
							{
								owningPlayer.incrementGreatAdmiralsCreated();
								CvPlot *pSpawnPlot = owningPlayer.GetGreatAdmiralSpawnPlot(pFreeUnit);
								if (pFreeUnit->plot() != pSpawnPlot)
								{
									pFreeUnit->setXY(pSpawnPlot->getX(), pSpawnPlot->getY());
								}
							}
							else if (pFreeUnit->IsGreatPerson())
							{
								owningPlayer.incrementGreatPeopleCreated();
								if (!pFreeUnit->jumpToNearestValidPlot())
									pFreeUnit->kill(false);	// Could not find a valid spot!
							}
						}
					}
				}
			}

			// Free building
			BuildingClassTypes eFreeBuildingClassThisCity = (BuildingClassTypes)pBuildingInfo->GetFreeBuildingThisCity();
			if(eFreeBuildingClassThisCity != NO_BUILDINGCLASS)
			{
				BuildingTypes eFreeBuildingThisCity = (BuildingTypes)(thisCiv.getCivilizationBuildings(eFreeBuildingClassThisCity));

				if (eFreeBuildingThisCity != NO_BUILDING)
				{
					m_pCityBuildings->SetNumRealBuilding(eFreeBuildingThisCity, 0);
					m_pCityBuildings->SetNumFreeBuilding(eFreeBuildingThisCity, 1);
				}
			}

			// Tech boost for science buildings in capital
			if(owningPlayer.GetPlayerTraits()->IsTechBoostFromCapitalScienceBuildings())
			{
				if(isCapital())
				{
					if(pBuildingInfo->IsScienceBuilding())
					{
						int iMedianTechResearch = owningPlayer.GetPlayerTechs()->GetMedianTechResearch();
						iMedianTechResearch = (iMedianTechResearch * owningPlayer.GetMedianTechPercentage()) / 100;

						TechTypes eCurrentTech = owningPlayer.GetPlayerTechs()->GetCurrentResearch();
						if(eCurrentTech == NO_TECH)
						{
							owningPlayer.changeOverflowResearch(iMedianTechResearch);
						}
						else
						{
							owningTeam.GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iMedianTechResearch, owningPlayer.GetID());
						}
					}
				}
			}
		}

		if(pBuildingInfo->GetTrainedFreePromotion() != NO_PROMOTION)
		{
			changeFreePromotionCount(((PromotionTypes)(pBuildingInfo->GetTrainedFreePromotion())), iChange);
		}

		changeGreatPeopleRateModifier(pBuildingInfo->GetGreatPeopleRateModifier() * iChange);
		changeFreeExperience(pBuildingInfo->GetFreeExperience() * iChange);
		changeMaxAirlift(pBuildingInfo->GetAirlift() * iChange);
		changeAirModifier(pBuildingInfo->GetAirModifier() * iChange);
		changeNukeModifier(pBuildingInfo->GetNukeModifier() * iChange);
		changeHealRate(pBuildingInfo->GetHealRateChange() * iChange);
		ChangeExtraHitPoints(pBuildingInfo->GetExtraCityHitPoints() * iChange);

		ChangeNoOccupiedUnhappinessCount(pBuildingInfo->IsNoOccupiedUnhappiness() * iChange);

		if(pBuildingInfo->GetHappiness() > 0)
		{
			ChangeBaseHappinessFromBuildings(pBuildingInfo->GetHappiness() * iChange);
		}

		if(pBuildingInfo->GetUnmoddedHappiness() > 0)
		{
			ChangeUnmoddedHappinessFromBuildings(pBuildingInfo->GetUnmoddedHappiness() * iChange);
		}

		if(pBuildingInfo->GetUnhappinessModifier() != 0)
		{
			owningPlayer.ChangeUnhappinessMod(pBuildingInfo->GetUnhappinessModifier() * iChange);
		}

		int iBuildingCulture = pBuildingInfo->GetYieldChange(YIELD_CULTURE);
		if(iBuildingCulture > 0)
		{
			iBuildingCulture += owningPlayer.GetPlayerTraits()->GetCultureBuildingYieldChange();
		}
		ChangeJONSCulturePerTurnFromBuildings(iBuildingCulture * iChange);
		changeCultureRateModifier(pBuildingInfo->GetCultureRateModifier() * iChange);
		changePlotCultureCostModifier(pBuildingInfo->GetPlotCultureCostModifier() * iChange);
		changePlotBuyCostModifier(pBuildingInfo->GetPlotBuyCostModifier() * iChange);

		int iBuildingFaith = pBuildingInfo->GetYieldChange(YIELD_FAITH);
		ChangeFaithPerTurnFromBuildings(iBuildingFaith * iChange);
		m_pCityReligions->ChangeReligiousPressureModifier(pBuildingInfo->GetReligiousPressureModifier() * iChange);

		PolicyTypes ePolicy;
		for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
		{
			ePolicy = (PolicyTypes) iPolicyLoop;

			if(owningPlayer.GetPlayerPolicies()->HasPolicy(ePolicy) && !owningPlayer.GetPlayerPolicies()->IsPolicyBlocked(ePolicy))
			{
				ChangeJONSCulturePerTurnFromPolicies(GC.getPolicyInfo(ePolicy)->GetBuildingClassCultureChange(eBuildingClass) * iChange);
				ChangeFaithPerTurnFromPolicies(GC.getPolicyInfo(ePolicy)->GetBuildingClassYieldChanges(eBuildingClass, YIELD_FAITH) * iChange);
			}
		}

		changeMaxFoodKeptPercent(pBuildingInfo->GetFoodKept() * iChange);
		changeMilitaryProductionModifier(pBuildingInfo->GetMilitaryProductionModifier() * iChange);
		changeSpaceProductionModifier(pBuildingInfo->GetSpaceProductionModifier() * iChange);
		m_pCityBuildings->ChangeBuildingProductionModifier(pBuildingInfo->GetBuildingProductionModifier() * iChange);
		m_pCityBuildings->ChangeMissionaryExtraSpreads(pBuildingInfo->GetExtraMissionarySpreads() * iChange);
		ChangeWonderProductionModifier(pBuildingInfo->GetWonderProductionModifier() * iChange);
		changeCapturePlunderModifier(pBuildingInfo->GetCapturePlunderModifier() * iChange);
		ChangeEspionageModifier(pBuildingInfo->GetEspionageModifier() * iChange);

		if (pBuildingInfo->AffectSpiesNow() && iChange > 0)
		{
			for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes ePlayer = (PlayerTypes)ui;
				GET_PLAYER(ePlayer).GetEspionage()->UpdateCity(this);
			}
		}

		// Resource loop
		int iCulture, iFaith;
		ResourceTypes eResource;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			// Does this building add resources?
			int iNumResource = pBuildingInfo->GetResourceQuantity(iResourceLoop) * iChange;
			if(iNumResource != 0)
			{
				owningPlayer.changeNumResourceTotal(eResource, iNumResource);
			}

			// Do we have this resource local?
			if(IsHasResourceLocal(eResource, /*bTestVisible*/ false))
			{
				// Our Building does give culture with eResource
				iCulture = GC.getBuildingInfo(eBuilding)->GetResourceCultureChange(eResource);

				if(iCulture != 0)
				{
					ChangeJONSCulturePerTurnFromBuildings(iCulture * m_paiNumResourcesLocal[eResource]);
				}

				// What about faith?
				iFaith = GC.getBuildingInfo(eBuilding)->GetResourceFaithChange(eResource);

				if(iFaith != 0)
				{
					ChangeFaithPerTurnFromBuildings(iFaith * m_paiNumResourcesLocal[eResource]);
				}
			}
		}

		if(pBuildingInfo->IsExtraLuxuries())
		{
			CvPlot* pLoopPlot;

			// Subtract off old luxury counts
			for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
			{
				pLoopPlot = plotCity(getX(), getY(), iJ);

				if(pLoopPlot != NULL && pLoopPlot->getOwner() == getOwner())
				{
					ResourceTypes eLoopResource = pLoopPlot->getResourceType();
					if(eLoopResource != NO_RESOURCE && GC.getResourceInfo(eLoopResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
					{
						if(owningTeam.GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(eLoopResource)->getTechCityTrade()))
						{
							if(pLoopPlot == plot() || (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getImprovementType())->IsImprovementResourceTrade(eLoopResource)))
							{
								if(!pLoopPlot->IsImprovementPillaged())
								{
									owningPlayer.changeNumResourceTotal(pLoopPlot->getResourceType(), -pLoopPlot->getNumResourceForPlayer(getOwner()), /*bIgnoreResourceWarning*/ true);
								}
							}
						}
					}
				}
			}

			ChangeExtraLuxuryResources(iChange);

			// Add in new luxury counts
			for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
			{
				pLoopPlot = plotCity(getX(), getY(), iJ);

				if(pLoopPlot != NULL && pLoopPlot->getOwner() == getOwner())
				{
					ResourceTypes eLoopResource = pLoopPlot->getResourceType();
					if(eLoopResource != NO_RESOURCE && GC.getResourceInfo(eLoopResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
					{
						if(owningTeam.GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(eLoopResource)->getTechCityTrade()))
						{
							if(pLoopPlot == plot() || (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getImprovementType())->IsImprovementResourceTrade(eLoopResource)))
							{
								if(!pLoopPlot->IsImprovementPillaged())
								{
									owningPlayer.changeNumResourceTotal(pLoopPlot->getResourceType(), pLoopPlot->getNumResourceForPlayer(getOwner()));
								}
							}
						}
					}
				}
			}
		}

		YieldTypes eYield;

		for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			eYield = (YieldTypes) iI;

			changeSeaPlotYield(eYield, (pBuildingInfo->GetSeaPlotYieldChange(eYield) * iChange));
			changeRiverPlotYield(eYield, (pBuildingInfo->GetRiverPlotYieldChange(eYield) * iChange));
			changeLakePlotYield(eYield, (pBuildingInfo->GetLakePlotYieldChange(eYield) * iChange));
			changeSeaResourceYield(eYield, (pBuildingInfo->GetSeaResourceYieldChange(eYield) * iChange));
			ChangeBaseYieldRateFromBuildings(eYield, ((pBuildingInfo->GetYieldChange(eYield) + m_pCityBuildings->GetBuildingYieldChange(eBuildingClass, eYield)) * iChange));
			ChangeYieldPerPopTimes100(eYield, pBuildingInfo->GetYieldChangePerPop(eYield) * iChange);
			changeYieldRateModifier(eYield, (pBuildingInfo->GetYieldModifier(eYield) * iChange));

			CvPlayerPolicies* pPolicies = GET_PLAYER(getOwner()).GetPlayerPolicies();
			changeYieldRateModifier(eYield, pPolicies->GetBuildingClassYieldModifier(eBuildingClass, eYield) * iChange);
			ChangeBaseYieldRateFromBuildings(eYield, pPolicies->GetBuildingClassYieldChange(eBuildingClass, eYield) * iChange);

			for(int iJ = 0; iJ < GC.getNumResourceInfos(); iJ++)
			{
				ChangeResourceExtraYield(((ResourceTypes)iJ), eYield, (GC.getBuildingInfo(eBuilding)->GetResourceYieldChange(iJ, eYield) * iChange));
			}

			for(int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
			{
				ChangeFeatureExtraYield(((FeatureTypes)iJ), eYield, (GC.getBuildingInfo(eBuilding)->GetFeatureYieldChange(iJ, eYield) * iChange));
			}

			for(int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
			{
				ChangeTerrainExtraYield(((TerrainTypes)iJ), eYield, (GC.getBuildingInfo(eBuilding)->GetTerrainYieldChange(iJ, eYield) * iChange));
			}

			if(pBuildingInfo->GetEnhancedYieldTech() != NO_TECH)
			{
				if(owningTeam.GetTeamTechs()->HasTech((TechTypes)pBuildingInfo->GetEnhancedYieldTech()))
				{
					if(eYield == YIELD_CULTURE)
					{
						ChangeJONSCulturePerTurnFromBuildings(pBuildingInfo->GetTechEnhancedYieldChange(eYield) * iChange);
					}
					else if(eYield == YIELD_FAITH)
					{
						ChangeFaithPerTurnFromBuildings(pBuildingInfo->GetTechEnhancedYieldChange(eYield) * iChange);
					}
					else
					{
						ChangeBaseYieldRateFromBuildings(eYield, pBuildingInfo->GetTechEnhancedYieldChange(eYield) * iChange);
					}
				}
			}

			int iBuildingClassBonus = owningPlayer.GetBuildingClassYieldChange(eBuildingClass, eYield);
			if(iBuildingClassBonus > 0)
			{
				if(eYield == YIELD_CULTURE)
				{
					ChangeJONSCulturePerTurnFromBuildings(iBuildingClassBonus * iChange);
				}
				else if(eYield == YIELD_FAITH)
				{
					ChangeFaithPerTurnFromBuildings(iBuildingClassBonus * iChange);
				}
				else
				{
					ChangeBaseYieldRateFromBuildings(eYield, iBuildingClassBonus * iChange);
				}
			}
		}

		if(GC.getBuildingInfo(eBuilding)->GetSpecialistType() != NO_SPECIALIST)
		{
			GetCityCitizens()->ChangeBuildingGreatPeopleRateChanges((SpecialistTypes) GC.getBuildingInfo(eBuilding)->GetSpecialistType(), pBuildingInfo->GetGreatPeopleRateChange() * iChange);
		}

		for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
			if(pkUnitCombatClassInfo)
			{
				changeUnitCombatFreeExperience(eUnitCombatClass, pBuildingInfo->GetUnitCombatFreeExperience(iI) * iChange);
				changeUnitCombatProductionModifier(eUnitCombatClass, pBuildingInfo->GetUnitCombatProductionModifier(iI) * iChange);
			}
		}

		for(int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			changeDomainFreeExperience(((DomainTypes)iI), pBuildingInfo->GetDomainFreeExperience(iI) * iChange);
			changeDomainProductionModifier(((DomainTypes)iI), pBuildingInfo->GetDomainProductionModifier(iI) * iChange);
		}

		// Process for our player
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				if(pBuildingInfo->IsTeamShare() || (iI == getOwner()))
				{
					GET_PLAYER((PlayerTypes)iI).processBuilding(eBuilding, iChange, bFirst, area());
				}
			}
		}

		// Process for our team
		owningTeam.processBuilding(eBuilding, iChange, bFirst);
	}

	if(!bObsolete)
	{
		m_pCityBuildings->ChangeBuildingDefense(pBuildingInfo->GetDefenseModifier() * iChange);

		owningTeam.changeBuildingClassCount(eBuildingClass, iChange);
		owningPlayer.changeBuildingClassCount(eBuildingClass, iChange);

		owningPlayer.changeWondersScore(getWonderScore((BuildingClassTypes)(eBuildingClass)) * iChange);
	}

	UpdateReligion(GetCityReligions()->GetReligiousMajority());

	owningPlayer.DoUpdateHappiness();

	setLayoutDirty(true);
}


//	--------------------------------------------------------------------------------
void CvCity::processProcess(ProcessTypes eProcess, int iChange)
{
	VALIDATE_OBJECT
	int iI;

	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeProductionToYieldModifier(((YieldTypes)iI), (GC.getProcessInfo(eProcess)->getProductionToYieldModifier(iI) * iChange));
	}
}


//	--------------------------------------------------------------------------------
void CvCity::processSpecialist(SpecialistTypes eSpecialist, int iChange)
{
	VALIDATE_OBJECT
	UnitTypes eGreatPeopleUnit;
	int iI;

	CvSpecialistInfo* pkSpecialist = GC.getSpecialistInfo(eSpecialist);
	if(pkSpecialist == NULL)
	{
		//This function requires a valid specialist type.
		return;
	}

	if(pkSpecialist->getGreatPeopleUnitClass() != NO_UNITCLASS)
	{
		eGreatPeopleUnit = ((UnitTypes)(getCivilizationInfo().getCivilizationUnits(pkSpecialist->getGreatPeopleUnitClass())));

		if(eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleUnitRate(eGreatPeopleUnit, pkSpecialist->getGreatPeopleRateChange() * iChange);
		}
	}

	changeBaseGreatPeopleRate(pkSpecialist->getGreatPeopleRateChange() * iChange);

	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		ChangeBaseYieldRateFromSpecialists(((YieldTypes)iI), (pkSpecialist->getYieldChange(iI) * iChange));
	}

	updateExtraSpecialistYield();

	changeSpecialistFreeExperience(pkSpecialist->getExperience() * iChange);

	// Culture
	int iCulturePerSpecialist = GetCultureFromSpecialist(eSpecialist);
	ChangeJONSCulturePerTurnFromSpecialists(iCulturePerSpecialist * iChange);
}

//	--------------------------------------------------------------------------------
/// Process the majority religion changing for a city
void CvCity::UpdateReligion(ReligionTypes eNewMajority)
{
	updateYield();

	// Reset city level yields
	m_iJONSCulturePerTurnFromReligion = 0;
	m_iFaithPerTurnFromReligion = 0;
	for(int iYield = 0; iYield <= YIELD_SCIENCE; iYield++)
	{
		m_aiBaseYieldRateFromReligion[iYield] = 0;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		if(eNewMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eNewMajority, getOwner());
			if(pReligion)
			{
				int iFollowers = GetCityReligions()->GetNumFollowers(eNewMajority);

				switch(iYield)
				{
				case YIELD_CULTURE:
					ChangeJONSCulturePerTurnFromReligion(pReligion->m_Beliefs.GetCityYieldChange(getPopulation(), (YieldTypes)iYield));
					break;
				case YIELD_FAITH:
					ChangeFaithPerTurnFromReligion(pReligion->m_Beliefs.GetCityYieldChange(getPopulation(), (YieldTypes)iYield));
					break;
				default:
					ChangeBaseYieldRateFromReligion((YieldTypes)iYield, pReligion->m_Beliefs.GetCityYieldChange(getPopulation(), (YieldTypes)iYield));
					break;
				}

				if(IsRouteToCapitalConnected())
				{
					switch(iYield)
					{
					case YIELD_CULTURE:
						ChangeJONSCulturePerTurnFromReligion(pReligion->m_Beliefs.GetYieldChangeTradeRoute((YieldTypes)iYield));
						break;
					case YIELD_FAITH:
						ChangeFaithPerTurnFromReligion(pReligion->m_Beliefs.GetYieldChangeTradeRoute((YieldTypes)iYield));
						break;
					default:
						ChangeBaseYieldRateFromReligion((YieldTypes)iYield, pReligion->m_Beliefs.GetYieldChangeTradeRoute((YieldTypes)iYield));
						break;
					}
				}
				
				if (GetCityCitizens()->GetTotalSpecialistCount() > 0)
				{
					switch(iYield)
					{
					case YIELD_CULTURE:
						ChangeJONSCulturePerTurnFromReligion(pReligion->m_Beliefs.GetYieldChangeAnySpecialist((YieldTypes)iYield));
						break;
					case YIELD_FAITH:
						ChangeFaithPerTurnFromReligion(pReligion->m_Beliefs.GetYieldChangeAnySpecialist((YieldTypes)iYield));
						break;
					default:
						ChangeBaseYieldRateFromReligion((YieldTypes)iYield, pReligion->m_Beliefs.GetYieldChangeAnySpecialist((YieldTypes)iYield));
						break;
					}
				}

				// Buildings
				for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
				{
					BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if(!pkBuildingClassInfo)
					{
						continue;
					}

					CvCivilizationInfo& playerCivilizationInfo = getCivilizationInfo();
					BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

					if(eBuilding != NO_BUILDING)
					{
						if(GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							int iYieldFromBuilding = pReligion->m_Beliefs.GetBuildingClassYieldChange(eBuildingClass, (YieldTypes)iYield, iFollowers);

							if (isWorldWonderClass(*pkBuildingClassInfo))
							{
								iYieldFromBuilding += pReligion->m_Beliefs.GetYieldChangeWorldWonder((YieldTypes)iYield);
							}

							switch(iYield)
							{
							case YIELD_CULTURE:
								ChangeJONSCulturePerTurnFromReligion(iYieldFromBuilding);
								break;
							case YIELD_FAITH:
								ChangeFaithPerTurnFromReligion(iYieldFromBuilding);
								break;
							default:
								ChangeBaseYieldRateFromReligion((YieldTypes)iYield, iYieldFromBuilding);
								break;
							}
						}
					}
				}
			}
		}
	}

	GET_PLAYER(getOwner()).UpdateReligion();
}

//	--------------------------------------------------------------------------------
/// Culture from eSpecialist
int CvCity::GetCultureFromSpecialist(SpecialistTypes eSpecialist) const
{
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pkSpecialistInfo == NULL)
	{
		//This function REQUIRES a valid specialist type.
		return 0;
	}

	int iCulture = pkSpecialistInfo->getCulturePerTurn();
	iCulture += GET_PLAYER(getOwner()).GetSpecialistCultureChange();

	return iCulture;
}

//	--------------------------------------------------------------------------------
CvHandicapInfo& CvCity::getHandicapInfo() const
{
	return GET_PLAYER(getOwner()).getHandicapInfo();
}

//	--------------------------------------------------------------------------------
HandicapTypes CvCity::getHandicapType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getHandicapType();
}

//	--------------------------------------------------------------------------------
CvCivilizationInfo& CvCity::getCivilizationInfo() const
{
	return GET_PLAYER(getOwner()).getCivilizationInfo();
}

//	--------------------------------------------------------------------------------
CivilizationTypes CvCity::getCivilizationType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationType();
}


//	--------------------------------------------------------------------------------
LeaderHeadTypes CvCity::getPersonalityType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getPersonalityType();
}


//	--------------------------------------------------------------------------------
ArtStyleTypes CvCity::getArtStyleType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getArtStyleType();
}


//	--------------------------------------------------------------------------------
CitySizeTypes CvCity::getCitySizeType() const
{
	VALIDATE_OBJECT
	return ((CitySizeTypes)(range((getPopulation() / 7), 0, (NUM_CITYSIZE_TYPES - 1))));
}


//	--------------------------------------------------------------------------------
bool CvCity::isBarbarian() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isBarbarian();
}


//	--------------------------------------------------------------------------------
bool CvCity::isHuman() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isHuman();
}


//	--------------------------------------------------------------------------------
bool CvCity::isVisible(TeamTypes eTeam, bool bDebug) const
{
	VALIDATE_OBJECT
	return plot()->isVisible(eTeam, bDebug);
}


//	--------------------------------------------------------------------------------
bool CvCity::isCapital() const
{
	VALIDATE_OBJECT
	return (GET_PLAYER(getOwner()).getCapitalCity() == this);
}

//	--------------------------------------------------------------------------------
/// Was this city originally any player's capital?
bool CvCity::IsOriginalCapital() const
{
	VALIDATE_OBJECT

	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if(getX() == kPlayer.GetOriginalCapitalX() &&
		        getY() == kPlayer.GetOriginalCapitalY())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvCity::IsEverCapital() const
{
	VALIDATE_OBJECT
	return m_bEverCapital;
}

//	--------------------------------------------------------------------------------
void CvCity::SetEverCapital(bool bValue)
{
	VALIDATE_OBJECT
	if(IsEverCapital() != bValue)
	{
		m_bEverCapital = bValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvCity::isCoastal(int iMinWaterSize) const
{
	VALIDATE_OBJECT
	return plot()->isCoastalLand(iMinWaterSize);
}

//	--------------------------------------------------------------------------------
int CvCity::foodConsumption(bool /*bNoAngry*/, int iExtra) const
{
	VALIDATE_OBJECT
	int iPopulation = getPopulation() + iExtra;

	int iFoodPerPop = /*2*/ GC.getFOOD_CONSUMPTION_PER_POPULATION();

	int iNum = iPopulation * iFoodPerPop;

	// Specialists eat less food? (Policies, etc.)
	if(GET_PLAYER(getOwner()).isHalfSpecialistFood())
	{
		int iFoodReduction = GetCityCitizens()->GetTotalSpecialistCount() * iFoodPerPop;
		iFoodReduction /= 2;
		iNum -= iFoodReduction;
	}

	return iNum;
}

//	--------------------------------------------------------------------------------
int CvCity::foodDifference(bool bBottom) const
{
	VALIDATE_OBJECT
	return foodDifferenceTimes100(bBottom) / 100;
}


//	--------------------------------------------------------------------------------
int CvCity::foodDifferenceTimes100(bool bBottom, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	int iDifference;

	if(isFoodProduction())
	{
		iDifference = std::min(0, GetFoodProduction(getYieldRate(YIELD_FOOD) - foodConsumption()) * 100);
	}
	else
	{
		iDifference = (getYieldRateTimes100(YIELD_FOOD) - foodConsumption() * 100);
	}

	if(bBottom)
	{
		if((getPopulation() == 1) && (getFood() == 0))
		{
			iDifference = std::max(0, iDifference);
		}
	}

	// Growth Mods - Only apply if the City is growing (and not starving, otherwise it would actually have the OPPOSITE of the intended effect!)
	if(iDifference > 0)
	{
		int iTotalMod = 100;

		// Capital Mod for player. Used for Policies and such
		if(isCapital())
		{
			int iCapitalGrowthMod = GET_PLAYER(getOwner()).GetCapitalGrowthMod();
			if(iCapitalGrowthMod != 0)
			{
				iTotalMod += iCapitalGrowthMod;
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_FOODMOD_CAPITAL", iCapitalGrowthMod);
			}
		}

		// City Mod for player. Used for Policies and such
		int iCityGrowthMod = GET_PLAYER(getOwner()).GetCityGrowthMod();
		if(iCityGrowthMod != 0)
		{
			iTotalMod += iCityGrowthMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_FOODMOD_PLAYER", iCityGrowthMod);
		}

		// Religion growth mod
		int iReligionGrowthMod = 0;
		ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				bool bAtPeace = GET_TEAM(getTeam()).getAtWarCount(false) == 0;
				iReligionGrowthMod = pReligion->m_Beliefs.GetCityGrowthModifier(bAtPeace);
				iTotalMod += iReligionGrowthMod;
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_FOODMOD_RELIGION", iReligionGrowthMod);
			}
		}

		// Cities stop growing when empire is very unhappy
		if(GET_PLAYER(getOwner()).IsEmpireVeryUnhappy())
		{
			int iMod = /*-100*/ GC.getVERY_UNHAPPY_GROWTH_PENALTY();
			iTotalMod += iMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_FOODMOD_UNHAPPY", iMod);
		}
		// Cities grow slower if the player is over his Happiness Limit
		else if(GET_PLAYER(getOwner()).IsEmpireUnhappy())
		{
			int iMod = /*-75*/ GC.getUNHAPPY_GROWTH_PENALTY();
			iTotalMod += iMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_FOODMOD_UNHAPPY", iMod);
		}
		// WLTKD Growth Bonus
		else if(GetWeLoveTheKingDayCounter() > 0)
		{
			int iMod = /*25*/ GC.getWLTKD_GROWTH_MULTIPLIER();
			iTotalMod += iMod;
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_FOODMOD_WLTKD", iMod);
		}

		iDifference *= iTotalMod;
		iDifference /= 100;
	}

	return iDifference;
}


//	--------------------------------------------------------------------------------
int CvCity::growthThreshold() const
{
	VALIDATE_OBJECT
	int iNumFoodNeeded = GET_PLAYER(getOwner()).getGrowthThreshold(getPopulation());

	return (iNumFoodNeeded);
}


//	--------------------------------------------------------------------------------
int CvCity::productionLeft() const
{
	VALIDATE_OBJECT
	return (getProductionNeeded() - getProduction());
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCostModifier(HurryTypes eHurry, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	int iModifier = 100;
	const OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		switch(pOrderNode->eOrderType)
		{
		case ORDER_TRAIN:
			iModifier = getHurryCostModifier(eHurry, (UnitTypes) pOrderNode->iData1, bIgnoreNew);
			break;

		case ORDER_CONSTRUCT:
			iModifier = getHurryCostModifier(eHurry, (BuildingTypes) pOrderNode->iData1, bIgnoreNew);
			break;

		case ORDER_CREATE:
		case ORDER_PREPARE:
		case ORDER_MAINTAIN:
			break;

		default:
			CvAssertMsg(false, "pOrderNode->eOrderType did not match a valid option");
			break;
		}
	}

	return iModifier;
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCostModifier(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo)
	{
		return getHurryCostModifier(eHurry, pkUnitInfo->GetHurryCostModifier(), getUnitProduction(eUnit), bIgnoreNew);
	}

	return 0;
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCostModifier(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo)
	{
		return getHurryCostModifier(eHurry, pkBuildingInfo->GetHurryCostModifier(), m_pCityBuildings->GetBuildingProduction(eBuilding), bIgnoreNew);
	}

	return 0;
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCostModifier(HurryTypes eHurry, int iBaseModifier, int iProduction, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	int iModifier = 100;
	iModifier *= std::max(0, iBaseModifier + 100);
	iModifier /= 100;

	if(iProduction == 0 && !bIgnoreNew)
	{
		iModifier *= std::max(0, (GC.getNEW_HURRY_MODIFIER() + 100));
		iModifier /= 100;
	}

	// Some places just don't care what kind of Hurry it is (leftover from Civ 4)
	if(eHurry != NO_HURRY)
	{
		if(GET_PLAYER(getOwner()).getHurryModifier(eHurry) != 0)
		{
			iModifier *= (100 + GET_PLAYER(getOwner()).getHurryModifier(eHurry));
			iModifier /= 100;
		}
	}

	return iModifier;
}


//	--------------------------------------------------------------------------------
int CvCity::hurryCost(HurryTypes eHurry, bool bExtra) const
{
	VALIDATE_OBJECT
	return (getHurryCost(bExtra, productionLeft(), getHurryCostModifier(eHurry), getProductionModifier()));
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCost(HurryTypes eHurry, bool bExtra, UnitTypes eUnit, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	int iProductionLeft = getProductionNeeded(eUnit) - getUnitProduction(eUnit);

	return getHurryCost(bExtra, iProductionLeft, getHurryCostModifier(eHurry, eUnit, bIgnoreNew), getProductionModifier(eUnit));
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCost(HurryTypes eHurry, bool bExtra, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	VALIDATE_OBJECT
	int iProductionLeft = getProductionNeeded(eBuilding) - m_pCityBuildings->GetBuildingProduction(eBuilding);

	return getHurryCost(bExtra, iProductionLeft, getHurryCostModifier(eHurry, eBuilding, bIgnoreNew), getProductionModifier(eBuilding));
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const
{
	VALIDATE_OBJECT
	int iProduction = (iProductionLeft * iHurryModifier + 99) / 100; // round up

	if(bExtra)
	{
		int iExtraProduction = getExtraProductionDifference(iProduction, iModifier);
		if(iExtraProduction > 0)
		{
			int iAdjustedProd = iProduction * iProduction;

			// round up
			iProduction = (iAdjustedProd + (iExtraProduction - 1)) / iExtraProduction;
		}
	}

	return std::max(0, iProduction);
}

//	--------------------------------------------------------------------------------
int CvCity::hurryGold(HurryTypes eHurry) const
{
	VALIDATE_OBJECT
	int iFullCost = getProductionNeeded();

	return getHurryGold(eHurry, hurryCost(eHurry, false), iFullCost);
}

//	--------------------------------------------------------------------------------
/// Amount of Gold required to hurry Production in a City.  Full cost is the original Production cost of whatever we're rushing - the more expensive the ORIGINAL cost, the more it also costs to Gold rush
int CvCity::getHurryGold(HurryTypes /*eHurry*/, int /*iHurryCost*/, int /*iFullCost*/) const
{
	VALIDATE_OBJECT

	// This should not be used any more. Check GetPurchaseCostFromProduction()
	return 0;
}


//	--------------------------------------------------------------------------------
int CvCity::hurryPopulation(HurryTypes eHurry) const
{
	VALIDATE_OBJECT
	return (getHurryPopulation(eHurry, hurryCost(eHurry, true)));
}

//	--------------------------------------------------------------------------------
int CvCity::getHurryPopulation(HurryTypes eHurry, int iHurryCost) const
{
	VALIDATE_OBJECT
	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo == NULL)
	{
		return 0;
	}

	if(pkHurryInfo->getProductionPerPopulation() == 0)
	{
		return 0;
	}

	int iPopulation = (iHurryCost - 1) / GC.getGame().getProductionPerPopulation(eHurry);

	return std::max(1, (iPopulation + 1));
}

//	--------------------------------------------------------------------------------
int CvCity::hurryProduction(HurryTypes eHurry) const
{
	VALIDATE_OBJECT
	int iProduction = 0;

	CvHurryInfo* pkHurryInfo = GC.getHurryInfo(eHurry);
	if(pkHurryInfo)
	{
		if(pkHurryInfo->getProductionPerPopulation() > 0)
		{
			iProduction = (100 * getExtraProductionDifference(hurryPopulation(eHurry) * GC.getGame().getProductionPerPopulation(eHurry))) / std::max(1, getHurryCostModifier(eHurry));
			CvAssert(iProduction >= productionLeft());
		}
		else
		{
			iProduction = productionLeft();
		}
	}

	return iProduction;
}

//	--------------------------------------------------------------------------------
int CvCity::maxHurryPopulation() const
{
	VALIDATE_OBJECT
	return (getPopulation() / 2);
}

//	--------------------------------------------------------------------------------
bool CvCity::hasActiveWorldWonder() const
{
	VALIDATE_OBJECT

	CvTeam& kTeam = GET_TEAM(getTeam());

	for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			if(isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
			{
				if(m_pCityBuildings->GetNumRealBuilding(eBuilding) > 0 && !(kTeam.isObsoleteBuilding(eBuilding)))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvCity::getIndex() const
{
	VALIDATE_OBJECT
	return (GetID() & FLTA_INDEX_MASK);
}


//	--------------------------------------------------------------------------------
IDInfo CvCity::GetIDInfo() const
{
	VALIDATE_OBJECT
	IDInfo city(getOwner(), GetID());
	return city;
}


//	--------------------------------------------------------------------------------
void CvCity::SetID(int iID)
{
	VALIDATE_OBJECT
	m_iID = iID;
}

//	--------------------------------------------------------------------------------
CvPlot* CvCity::plot() const 
{ 
	if ((m_iX != INVALID_PLOT_COORD) && (m_iY != INVALID_PLOT_COORD))
		return GC.getMap().plotUnchecked(m_iX, m_iY); 
	return NULL;
}

//	--------------------------------------------------------------------------------
bool CvCity::at(int iX,  int iY) const
{
	VALIDATE_OBJECT
	return ((getX() == iX) && (getY() == iY));
}


//	--------------------------------------------------------------------------------
bool CvCity::at(CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	return (plot() == pPlot);
}

//	--------------------------------------------------------------------------------
int CvCity::getArea() const
{
	VALIDATE_OBJECT
	return plot()->getArea();
}

//	--------------------------------------------------------------------------------
CvArea* CvCity::area() const
{
	VALIDATE_OBJECT
	return plot()->area();
}


//	--------------------------------------------------------------------------------
CvArea* CvCity::waterArea() const
{
	VALIDATE_OBJECT
	return plot()->waterArea();
}

//	--------------------------------------------------------------------------------
CvUnit* CvCity::GetGarrisonedUnit() const
{
	CvUnit* pGarrison = NULL;

	CvPlot* pPlot = plot();
	if(pPlot)
	{
		UnitHandle garrison = pPlot->getBestDefender(getOwner());
		if(garrison)
		{
			pGarrison = garrison.pointer();
		}
	}

	return pGarrison;
}

//	--------------------------------------------------------------------------------
CvPlot* CvCity::getRallyPlot() const
{
	VALIDATE_OBJECT
	if ((m_iRallyX != INVALID_PLOT_COORD) && (m_iRallyY != INVALID_PLOT_COORD))
	{
		return GC.getMap().plotUnchecked(m_iRallyX, m_iRallyY);
	}
	else
		return NULL;
}


//	--------------------------------------------------------------------------------
void CvCity::setRallyPlot(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if(getRallyPlot() != pPlot)
	{
		if(pPlot != NULL)
		{
			m_iRallyX = pPlot->getX();
			m_iRallyY = pPlot->getY();
		}
		else
		{
			m_iRallyX = INVALID_PLOT_COORD;
			m_iRallyY = INVALID_PLOT_COORD;
		}

		if(isCitySelected())
		{
			DLLUI->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getGameTurnFounded() const
{
	VALIDATE_OBJECT
	return m_iGameTurnFounded;
}


//	--------------------------------------------------------------------------------
void CvCity::setGameTurnFounded(int iNewValue)
{
	VALIDATE_OBJECT
	if(m_iGameTurnFounded != iNewValue)
	{
		m_iGameTurnFounded = iNewValue;
		CvAssert(getGameTurnFounded() >= 0);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getGameTurnAcquired() const
{
	VALIDATE_OBJECT
	return m_iGameTurnAcquired;
}


//	--------------------------------------------------------------------------------
void CvCity::setGameTurnAcquired(int iNewValue)
{
	VALIDATE_OBJECT
	m_iGameTurnAcquired = iNewValue;
	CvAssert(getGameTurnAcquired() >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getGameTurnLastExpanded() const
{
	VALIDATE_OBJECT
	return m_iGameTurnLastExpanded;
}


//	--------------------------------------------------------------------------------
void CvCity::setGameTurnLastExpanded(int iNewValue)
{
	VALIDATE_OBJECT
	if(m_iGameTurnLastExpanded != iNewValue)
	{
		m_iGameTurnLastExpanded = iNewValue;
		CvAssert(m_iGameTurnLastExpanded >= 0);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getPopulation() const
{
	VALIDATE_OBJECT
	return m_iPopulation;
}

//	---------------------------------------------------------------------------------
//	Be very careful with setting bReassignPop to false.  This assumes that the caller
//  is manually adjusting the worker assignments *and* handling the setting of
//  the CityCitizens unassigned worker value.
void CvCity::setPopulation(int iNewValue, bool bReassignPop /* = true */)
{
	VALIDATE_OBJECT
	int iOldPopulation;

	iOldPopulation = getPopulation();
	int iPopChange = iNewValue - iOldPopulation;

	if(iOldPopulation != iNewValue)
	{
		// If we are reducing population, remove the workers first
		if(bReassignPop)
		{
			if(iPopChange < 0)
			{
				// Need to Remove Citizens
				for(int iNewPopLoop = -iPopChange; iNewPopLoop--;)
				{
					GetCityCitizens()->DoRemoveWorstCitizen(false, NO_SPECIALIST, iNewValue);
				}

				// Fixup the unassigned workers
				int iUnassignedWorkers = GetCityCitizens()->GetNumUnassignedCitizens();
				CvAssert(iUnassignedWorkers >= -iPopChange);
				GetCityCitizens()->ChangeNumUnassignedCitizens(std::max(iPopChange, -iUnassignedWorkers));
			}
		}

		m_iPopulation = iNewValue;

		CvAssert(getPopulation() >= 0);

		GET_PLAYER(getOwner()).invalidatePopulationRankCache();

		if(getPopulation() > getHighestPopulation())
		{
			setHighestPopulation(getPopulation());
		}

		area()->changePopulationPerPlayer(getOwner(), (getPopulation() - iOldPopulation));
		GET_PLAYER(getOwner()).changeTotalPopulation(getPopulation() - iOldPopulation);
		GET_TEAM(getTeam()).changeTotalPopulation(getPopulation() - iOldPopulation);
		GC.getGame().changeTotalPopulation(getPopulation() - iOldPopulation);

		plot()->updateYield();

		UpdateReligion(GetCityReligions()->GetReligiousMajority());

		ChangeBaseYieldRateFromMisc(YIELD_SCIENCE, (iNewValue - iOldPopulation) * GC.getSCIENCE_PER_POPULATION());

		if(iPopChange > 0)
		{
			// Give new Population something to do in the City
			if(bReassignPop)
			{
				GetCityCitizens()->ChangeNumUnassignedCitizens(iPopChange);

				// Need to Add Citizens
				for(int iNewPopLoop = 0; iNewPopLoop < iPopChange; iNewPopLoop++)
				{
					GetCityCitizens()->DoAddBestCitizenFromUnassigned();
				}
			}
		}

		setLayoutDirty(true);
		{
			auto_ptr<ICvCity1> pkDllCity(new CvDllCity(this));
			gDLL->GameplayCityPopulationChanged(pkDllCity.get(), iNewValue);
		}

		plot()->plotAction(PUF_makeInfoBarDirty);

		if((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
		}

		// Update Unit Maintenance for the player
		GET_PLAYER(getOwner()).UpdateUnitProductionMaintenanceMod();

		GET_PLAYER(getOwner()).DoUpdateHappiness();

		//updateGenericBuildings();
		updateStrengthValue();

		DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getX());
		args->Push(getY());
		args->Push(iOldPopulation);
		args->Push(iNewValue);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "SetPopulation", args.get(), bResult);
	}
}

//	---------------------------------------------------------------------------------
//	Be very careful with setting bReassignPop to false.  This assumes that the caller
//  is manually adjusting the worker assignments *and* handling the setting of
//  the CityCitizens unassigned worker value.
void CvCity::changePopulation(int iChange, bool bReassignPop)
{
	VALIDATE_OBJECT
	setPopulation(getPopulation() + iChange, bReassignPop);

	// Update the religious system
	GetCityReligions()->DoPopulationChange(iChange);
}


//	--------------------------------------------------------------------------------
long CvCity::getRealPopulation() const
{
	VALIDATE_OBJECT
	return (((long)(pow((double)getPopulation(), 2.8))) * 1000);
}

//	--------------------------------------------------------------------------------
int CvCity::getHighestPopulation() const
{
	VALIDATE_OBJECT
	return m_iHighestPopulation;
}


//	--------------------------------------------------------------------------------
void CvCity::setHighestPopulation(int iNewValue)
{
	VALIDATE_OBJECT
	m_iHighestPopulation = iNewValue;
	CvAssert(getHighestPopulation() >= 0);
}

//	--------------------------------------------------------------------------------
int CvCity::getNumGreatPeople() const
{
	VALIDATE_OBJECT
	return m_iNumGreatPeople;
}


//	--------------------------------------------------------------------------------
void CvCity::changeNumGreatPeople(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iNumGreatPeople = (m_iNumGreatPeople + iChange);
		CvAssert(getNumGreatPeople() >= 0);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getBaseGreatPeopleRate() const
{
	VALIDATE_OBJECT
	return m_iBaseGreatPeopleRate;
}


//	--------------------------------------------------------------------------------
int CvCity::getGreatPeopleRate() const
{
	VALIDATE_OBJECT
	return ((getBaseGreatPeopleRate() * getTotalGreatPeopleRateModifier()) / 100);
}


//	--------------------------------------------------------------------------------
int CvCity::getTotalGreatPeopleRateModifier() const
{
	VALIDATE_OBJECT
	int iModifier;

	iModifier = getGreatPeopleRateModifier();

	iModifier += GET_PLAYER(getOwner()).getGreatPeopleRateModifier();

	if(GET_PLAYER(getOwner()).isGoldenAge())
	{
		iModifier += GC.getGOLDEN_AGE_GREAT_PEOPLE_MODIFIER();
	}

	return std::max(0, (iModifier + 100));
}


//	--------------------------------------------------------------------------------
void CvCity::changeBaseGreatPeopleRate(int iChange)
{
	VALIDATE_OBJECT
	m_iBaseGreatPeopleRate = (m_iBaseGreatPeopleRate + iChange);
	CvAssert(getBaseGreatPeopleRate() >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getGreatPeopleRateModifier() const
{
	VALIDATE_OBJECT
	return m_iGreatPeopleRateModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changeGreatPeopleRateModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatPeopleRateModifier = (m_iGreatPeopleRateModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getGreatPeopleProgress() const
{
	VALIDATE_OBJECT
	return m_iGreatPeopleProgress;
}


//	--------------------------------------------------------------------------------
void CvCity::changeGreatPeopleProgress(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatPeopleProgress = (m_iGreatPeopleProgress + iChange);
	CvAssert(getGreatPeopleProgress() >= 0);
}


//	--------------------------------------------------------------------------------
/// Amount of Culture in this City
int CvCity::GetJONSCultureStored() const
{
	VALIDATE_OBJECT
	return m_iJONSCultureStored;
}

//	--------------------------------------------------------------------------------
/// Sets the amount of Culture in this City
void CvCity::SetJONSCultureStored(int iValue)
{
	VALIDATE_OBJECT
	m_iJONSCultureStored = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes the amount of Culture in this City
void CvCity::ChangeJONSCultureStored(int iChange)
{
	VALIDATE_OBJECT
	SetJONSCultureStored(GetJONSCultureStored() + iChange);
}


//	--------------------------------------------------------------------------------
/// Culture level of this City
int CvCity::GetJONSCultureLevel() const
{
	VALIDATE_OBJECT
	return m_iJONSCultureLevel;
}

//	--------------------------------------------------------------------------------
/// Sets the Culture level of this City
void CvCity::SetJONSCultureLevel(int iValue)
{
	VALIDATE_OBJECT
	m_iJONSCultureLevel = iValue;
}

//	--------------------------------------------------------------------------------
/// Changes the Culture level of this City
void CvCity::ChangeJONSCultureLevel(int iChange)
{
	VALIDATE_OBJECT
	SetJONSCultureLevel(GetJONSCultureLevel() + iChange);
}

//	--------------------------------------------------------------------------------
/// What happens when you have enough Culture to acquire a new Plot?
void CvCity::DoJONSCultureLevelIncrease()
{
	VALIDATE_OBJECT

	int iOverflow = GetJONSCultureStored() - GetJONSCultureThreshold();
	SetJONSCultureStored(iOverflow);
	ChangeJONSCultureLevel(1);

	CvPlot* pPlotToAcquire = GetNextBuyablePlot();

	// maybe the player owns ALL of the plots or there are none avaialable?
	if(pPlotToAcquire)
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			CvString playerName;
			FILogFile* pLog;
			CvString strBaseString;
			CvString strOutBuf;
			playerName = kOwner.getCivilizationShortDescription();
			pLog = LOGFILEMGR.GetLog(kOwner.GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);
			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";
			strOutBuf.Format("%s, City Culture Leveled Up. Level: %d Border Expanded, X: %d, Y: %d", getName().GetCString(), 
												GetJONSCultureLevel(), pPlotToAcquire->getX(), pPlotToAcquire->getY());
			strBaseString += strOutBuf;
			pLog->Msg(strBaseString);
		}
		DoAcquirePlot(pPlotToAcquire->getX(), pPlotToAcquire->getY());


		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(GetID());
			args->Push(pPlotToAcquire->getX());
			args->Push(pPlotToAcquire->getY());
			args->Push(false); // bGold
			args->Push(true); // bFaith/bCulture

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "CityBoughtPlot", args.get(), bResult);
		}
	}
}

//	--------------------------------------------------------------------------------
/// Amount of Culture needed in this City to acquire a new Plot
int CvCity::GetJONSCultureThreshold() const
{
	VALIDATE_OBJECT
	int iCultureThreshold = /*15*/ GC.getCULTURE_COST_FIRST_PLOT();

	float fExponent = /*1.1f*/ GC.getCULTURE_COST_LATER_PLOT_EXPONENT();

	int iPolicyExponentMod = GET_PLAYER(m_eOwner).GetPlotCultureExponentModifier();
	if(iPolicyExponentMod != 0)
	{
		fExponent = fExponent * (float)((100 + iPolicyExponentMod));
		fExponent /= 100.0f;
	}

	int iAdditionalCost = GetJONSCultureLevel() * /*8*/ GC.getCULTURE_COST_LATER_PLOT_MULTIPLIER();
	iAdditionalCost = (int) pow((double) iAdditionalCost, (double)fExponent);

	iCultureThreshold += iAdditionalCost;

	// More expensive for Minors to claim territory
	if(GET_PLAYER(getOwner()).isMinorCiv())
	{
		iCultureThreshold *= /*150*/ GC.getMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER();
		iCultureThreshold /= 100;
	}

	// Religion modifier
	int iReligionMod = 0;
	ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
	if(eMajority != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
		if(pReligion)
		{
			iReligionMod = pReligion->m_Beliefs.GetPlotCultureCostModifier();
		}
	}

	// -50 = 50% cost
	int iModifier = GET_PLAYER(getOwner()).GetPlotCultureCostModifier() + m_iPlotCultureCostModifier + iReligionMod;
	if(iModifier != 0)
	{
		iModifier = max(iModifier, /*-85*/ GC.getCULTURE_PLOT_COST_MOD_MINIMUM());	// value cannot reduced by more than 85%
		iCultureThreshold *= (100 + iModifier);
		iCultureThreshold /= 100;
	}

	// Game Speed Mod
	iCultureThreshold *= GC.getGame().getGameSpeedInfo().getCulturePercent();
	iCultureThreshold /= 100;

	// Make the number not be funky
	int iDivisor = /*5*/ GC.getCULTURE_COST_VISIBLE_DIVISOR();
	if(iCultureThreshold > iDivisor * 2)
	{
		iCultureThreshold /= iDivisor;
		iCultureThreshold *= iDivisor;
	}

	return iCultureThreshold;
}


//	--------------------------------------------------------------------------------
int CvCity::getJONSCulturePerTurn() const
{
	VALIDATE_OBJECT

	// No culture during Resistance
	if(IsResistance() || IsRazing())
	{
		return 0;
	}

	int iCulture = GetBaseJONSCulturePerTurn();

	int iModifier = 100;

	// City modifier
	iModifier += getCultureRateModifier();
	// Player modifier
	iModifier += GET_PLAYER(getOwner()).GetJONSCultureCityModifier();

	// Wonder here?
	if(getNumWorldWonders() > 0)
		iModifier += GET_PLAYER(getOwner()).GetCultureWonderMultiplier();

	// Puppet?
	if(IsPuppet())
	{
		iModifier += GC.getPUPPET_CULTURE_MODIFIER();
	}

	iCulture *= iModifier;
	iCulture /= 100;

	return iCulture;
}

//	--------------------------------------------------------------------------------
int CvCity::GetBaseJONSCulturePerTurn() const
{
	VALIDATE_OBJECT

	int iCulturePerTurn = 0;
	iCulturePerTurn += GetJONSCulturePerTurnFromBuildings();
	iCulturePerTurn += GetJONSCulturePerTurnFromPolicies();
	iCulturePerTurn += GetJONSCulturePerTurnFromSpecialists();
	iCulturePerTurn += GetBaseYieldRateFromTerrain(YIELD_CULTURE);
	iCulturePerTurn += GetJONSCulturePerTurnFromTraits();
	iCulturePerTurn += GetJONSCulturePerTurnFromReligion();

	return iCulturePerTurn;
}

//	--------------------------------------------------------------------------------
int CvCity::GetJONSCulturePerTurnFromBuildings() const
{
	VALIDATE_OBJECT
	return m_iJONSCulturePerTurnFromBuildings;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeJONSCulturePerTurnFromBuildings(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iJONSCulturePerTurnFromBuildings = (m_iJONSCulturePerTurnFromBuildings + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetJONSCulturePerTurnFromPolicies() const
{
	VALIDATE_OBJECT
	return m_iJONSCulturePerTurnFromPolicies;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeJONSCulturePerTurnFromPolicies(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iJONSCulturePerTurnFromPolicies = (m_iJONSCulturePerTurnFromPolicies + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetJONSCulturePerTurnFromSpecialists() const
{
	VALIDATE_OBJECT
	return m_iJONSCulturePerTurnFromSpecialists;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeJONSCulturePerTurnFromSpecialists(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iJONSCulturePerTurnFromSpecialists = (m_iJONSCulturePerTurnFromSpecialists + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetJONSCulturePerTurnFromTraits() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(m_eOwner).GetPlayerTraits()->GetCityCultureBonus();
}

//	--------------------------------------------------------------------------------
int CvCity::GetJONSCulturePerTurnFromReligion() const
{
	VALIDATE_OBJECT
	return m_iJONSCulturePerTurnFromReligion;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeJONSCulturePerTurnFromReligion(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iJONSCulturePerTurnFromReligion = (m_iJONSCulturePerTurnFromReligion + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPerTurn() const
{
	VALIDATE_OBJECT

	// No faith during Resistance
	if(IsResistance() || IsRazing())
	{
		return 0;
	}

	int iFaith = GetFaithPerTurnFromBuildings();
	iFaith += GetBaseYieldRateFromTerrain(YIELD_FAITH);
	iFaith += GetFaithPerTurnFromPolicies();
	iFaith += GetFaithPerTurnFromTraits();
	iFaith += GetFaithPerTurnFromReligion();

	// Puppet?
	int iModifier = 0;
	if(IsPuppet())
	{
		iModifier = GC.getPUPPET_FAITH_MODIFIER();
		iFaith *= (100 + iModifier);
		iFaith /= 100;
	}

	return iFaith;
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPerTurnFromBuildings() const
{
	VALIDATE_OBJECT
	return m_iFaithPerTurnFromBuildings;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeFaithPerTurnFromBuildings(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFaithPerTurnFromBuildings = (m_iFaithPerTurnFromBuildings + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPerTurnFromPolicies() const
{
	VALIDATE_OBJECT
	return m_iFaithPerTurnFromPolicies;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeFaithPerTurnFromPolicies(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFaithPerTurnFromPolicies = (m_iFaithPerTurnFromPolicies + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPerTurnFromTraits() const
{
	VALIDATE_OBJECT

	int iRtnValue = 0;

	if(GET_PLAYER(m_eOwner).GetPlayerTraits()->IsFaithFromUnimprovedForest())
	{
		// See how many tiles adjacent to city are unimproved forest
		int iAdjacentForests = 0;

		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iDirectionLoop));
			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->getFeatureType() == FEATURE_FOREST && pAdjacentPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					iAdjacentForests++;
				}
			}
		}

		// If 3 or more, bonus is +2
		if(iAdjacentForests > 2)
		{
			iRtnValue = 2;
		}
		else if(iAdjacentForests > 0)
		{
			iRtnValue = 1;
		}
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
int CvCity::GetFaithPerTurnFromReligion() const
{
	VALIDATE_OBJECT
	return m_iFaithPerTurnFromReligion;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeFaithPerTurnFromReligion(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFaithPerTurnFromReligion = (m_iFaithPerTurnFromReligion + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getCultureRateModifier() const
{
	VALIDATE_OBJECT
	return m_iCultureRateModifier;
}

//	--------------------------------------------------------------------------------
void CvCity::changeCultureRateModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCultureRateModifier = (m_iCultureRateModifier + iChange);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getNumWorldWonders() const
{
	VALIDATE_OBJECT
	return m_iNumWorldWonders;
}


//	--------------------------------------------------------------------------------
void CvCity::changeNumWorldWonders(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iNumWorldWonders = (m_iNumWorldWonders + iChange);
		CvAssert(getNumWorldWonders() >= 0);

		// Extra culture for Wonders (Policies, etc.)
		ChangeJONSCulturePerTurnFromPolicies(GET_PLAYER(getOwner()).GetCulturePerWonder() * iChange);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getNumTeamWonders() const
{
	VALIDATE_OBJECT
	return m_iNumTeamWonders;
}


//	--------------------------------------------------------------------------------
void CvCity::changeNumTeamWonders(int iChange)
{
	VALIDATE_OBJECT
	m_iNumTeamWonders = (m_iNumTeamWonders + iChange);
	CvAssert(getNumTeamWonders() >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getNumNationalWonders() const
{
	VALIDATE_OBJECT
	return m_iNumNationalWonders;
}


//	--------------------------------------------------------------------------------
void CvCity::changeNumNationalWonders(int iChange)
{
	VALIDATE_OBJECT
	m_iNumNationalWonders = (m_iNumNationalWonders + iChange);
	CvAssert(getNumNationalWonders() >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::GetWonderProductionModifier() const
{
	VALIDATE_OBJECT
	return m_iWonderProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::ChangeWonderProductionModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iWonderProductionModifier = (m_iWonderProductionModifier + iChange);
	CvAssert(GetWonderProductionModifier() >= 0);
}

//	--------------------------------------------------------------------------------
int CvCity::GetLocalResourceWonderProductionMod(BuildingTypes eBuilding, CvString* toolTipSink) const
{
	VALIDATE_OBJECT

	int iMultiplier = 0;

	CvAssertMsg(eBuilding > -1 && eBuilding < GC.getNumBuildingInfos(), "Invalid building index. Please show Jon.");
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo)
	{
		// Is this even a wonder?
		const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
		if(!::isWorldWonderClass(kBuildingClassInfo) &&
		        !::isTeamWonderClass(kBuildingClassInfo) &&
		        !::isNationalWonderClass(kBuildingClassInfo))
		{
			return 0;
		}

		// Resource wonder bonus
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if(pkResource)
			{
				int iBonus = pkResource->getWonderProductionMod();
				if(iBonus != 0)
				{
					if(IsHasResourceLocal(eResource, /*bTestVisible*/ false))
					{
						iMultiplier += iBonus;
						GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_WONDER_LOCAL_RES", iBonus, pkBuildingInfo->GetDescription());
					}
				}
			}
		}

	}

	return iMultiplier;
}


//	--------------------------------------------------------------------------------
int CvCity::getCapturePlunderModifier() const
{
	VALIDATE_OBJECT
	return m_iCapturePlunderModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changeCapturePlunderModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iCapturePlunderModifier = (m_iCapturePlunderModifier + iChange);
	CvAssert(m_iCapturePlunderModifier >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getPlotCultureCostModifier() const
{
	VALIDATE_OBJECT
	return m_iPlotCultureCostModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changePlotCultureCostModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iPlotCultureCostModifier = (m_iPlotCultureCostModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getPlotBuyCostModifier() const
{
	VALIDATE_OBJECT
	return m_iPlotBuyCostModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changePlotBuyCostModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iPlotBuyCostModifier = (m_iPlotBuyCostModifier + iChange);
}

//	--------------------------------------------------------------------------------
int CvCity::getHealRate() const
{
	VALIDATE_OBJECT
	return m_iHealRate;
}

//	--------------------------------------------------------------------------------
void CvCity::changeHealRate(int iChange)
{
	VALIDATE_OBJECT
	m_iHealRate = (m_iHealRate + iChange);
	CvAssert(getHealRate() >= 0);
}

//	--------------------------------------------------------------------------------
int CvCity::GetEspionageModifier() const
{
	VALIDATE_OBJECT
	return m_iEspionageModifier;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeEspionageModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iEspionageModifier = (m_iEspionageModifier + iChange);
}

//	--------------------------------------------------------------------------------
/// Does this city not produce occupied Unhappiness?
bool CvCity::IsNoOccupiedUnhappiness() const
{
	VALIDATE_OBJECT
	return GetNoOccupiedUnhappinessCount() > 0;
}

//	--------------------------------------------------------------------------------
/// Does this city not produce occupied Unhappiness?
int CvCity::GetNoOccupiedUnhappinessCount() const
{
	VALIDATE_OBJECT
	return m_iNoOccupiedUnhappinessCount;
}

//	--------------------------------------------------------------------------------
/// Does this city not produce occupied Unhappiness?
void CvCity::ChangeNoOccupiedUnhappinessCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
		m_iNoOccupiedUnhappinessCount += iChange;
}


//	--------------------------------------------------------------------------------
int CvCity::getFood() const
{
	VALIDATE_OBJECT
	return m_iFood / 100;
}

//	--------------------------------------------------------------------------------
int CvCity::getFoodTimes100() const
{
	VALIDATE_OBJECT
	return m_iFood;
}


//	--------------------------------------------------------------------------------
void CvCity::setFood(int iNewValue)
{
	VALIDATE_OBJECT
	setFoodTimes100(iNewValue*100);
}

//	--------------------------------------------------------------------------------
void CvCity::setFoodTimes100(int iNewValue)
{
	VALIDATE_OBJECT
	if(getFoodTimes100() != iNewValue)
	{
		m_iFood = iNewValue;
	}
}


//	--------------------------------------------------------------------------------
void CvCity::changeFood(int iChange)
{
	VALIDATE_OBJECT
	setFoodTimes100(getFoodTimes100() + 100 * iChange);
}


//	--------------------------------------------------------------------------------
void CvCity::changeFoodTimes100(int iChange)
{
	VALIDATE_OBJECT
	setFoodTimes100(getFoodTimes100() + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getFoodKept() const
{
	VALIDATE_OBJECT
	return m_iFoodKept;
}

//	--------------------------------------------------------------------------------
void CvCity::setFoodKept(int iNewValue)
{
	VALIDATE_OBJECT
	m_iFoodKept = iNewValue;
}


//	--------------------------------------------------------------------------------
void CvCity::changeFoodKept(int iChange)
{
	VALIDATE_OBJECT
	setFoodKept(getFoodKept() + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getMaxFoodKeptPercent() const
{
	VALIDATE_OBJECT
	return m_iMaxFoodKeptPercent;
}


//	--------------------------------------------------------------------------------
void CvCity::changeMaxFoodKeptPercent(int iChange)
{
	VALIDATE_OBJECT
	m_iMaxFoodKeptPercent = (m_iMaxFoodKeptPercent + iChange);
	CvAssert(getMaxFoodKeptPercent() >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getOverflowProduction() const
{
	VALIDATE_OBJECT
	return m_iOverflowProduction / 100;
}


//	--------------------------------------------------------------------------------
void CvCity::setOverflowProduction(int iNewValue)
{
	VALIDATE_OBJECT
	setOverflowProductionTimes100(iNewValue * 100);
}


//	--------------------------------------------------------------------------------
void CvCity::changeOverflowProduction(int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(iChange >= 0, "Production overflow is too low.  Please send a save to Ed.");
	CvAssertMsg(iChange < 250, "Production overflow is too high.  Please send a save to Ed.");
	changeOverflowProductionTimes100(iChange * 100);
}


//	--------------------------------------------------------------------------------
int CvCity::getOverflowProductionTimes100() const
{
	VALIDATE_OBJECT
	return m_iOverflowProduction;
}


//	--------------------------------------------------------------------------------
void CvCity::setOverflowProductionTimes100(int iNewValue)
{
	VALIDATE_OBJECT
	m_iOverflowProduction = iNewValue;
	CvAssert(getOverflowProductionTimes100() >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeOverflowProductionTimes100(int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(iChange >= 0, "Production overflow is too low.  Please send a save to Ed.");
	CvAssertMsg(iChange < 25000, "Production overflow is too high.  Please send a save to Ed.");
	setOverflowProductionTimes100(getOverflowProductionTimes100() + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getFeatureProduction() const
{
	VALIDATE_OBJECT
	return m_iFeatureProduction;
}


//	--------------------------------------------------------------------------------
void CvCity::setFeatureProduction(int iNewValue)
{
	VALIDATE_OBJECT
	m_iFeatureProduction = iNewValue;
	CvAssert(getFeatureProduction() >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeFeatureProduction(int iChange)
{
	VALIDATE_OBJECT
	setFeatureProduction(getFeatureProduction() + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getMilitaryProductionModifier()	const
{
	VALIDATE_OBJECT
	return m_iMilitaryProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changeMilitaryProductionModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getSpaceProductionModifier() const
{
	VALIDATE_OBJECT
	return m_iSpaceProductionModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changeSpaceProductionModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getFreeExperience() const
{
	VALIDATE_OBJECT
	return m_iFreeExperience;
}


//	--------------------------------------------------------------------------------
void CvCity::changeFreeExperience(int iChange)
{
	VALIDATE_OBJECT
	m_iFreeExperience = (m_iFreeExperience + iChange);
	CvAssert(getFreeExperience() >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getCurrAirlift() const
{
	VALIDATE_OBJECT
	return m_iCurrAirlift;
}


//	--------------------------------------------------------------------------------
void CvCity::setCurrAirlift(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCurrAirlift = iNewValue;
	CvAssert(getCurrAirlift() >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeCurrAirlift(int iChange)
{
	VALIDATE_OBJECT
	setCurrAirlift(getCurrAirlift() + iChange);
}


//	--------------------------------------------------------------------------------
int CvCity::getMaxAirlift() const
{
	VALIDATE_OBJECT
	return m_iMaxAirlift;
}


//	--------------------------------------------------------------------------------
void CvCity::changeMaxAirlift(int iChange)
{
	VALIDATE_OBJECT
	m_iMaxAirlift = (m_iMaxAirlift + iChange);
	CvAssert(getMaxAirlift() >= 0);
}

//	--------------------------------------------------------------------------------
int CvCity::getAirModifier() const
{
	VALIDATE_OBJECT
	return m_iAirModifier;
}

//	--------------------------------------------------------------------------------
void CvCity::changeAirModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iAirModifier += iChange;
}

//	--------------------------------------------------------------------------------
int CvCity::getNukeModifier() const
{
	VALIDATE_OBJECT
	return m_iNukeModifier;
}


//	--------------------------------------------------------------------------------
void CvCity::changeNukeModifier(int iChange)
{
	VALIDATE_OBJECT
	m_iNukeModifier = (m_iNukeModifier + iChange);
}

//	--------------------------------------------------------------------------------
bool CvCity::IsResistance() const
{
	VALIDATE_OBJECT
	return GetResistanceTurns() > 0;
}

//	--------------------------------------------------------------------------------
int CvCity::GetResistanceTurns() const
{
	VALIDATE_OBJECT
	return m_iResistanceTurns;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeResistanceTurns(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iResistanceTurns += iChange;

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
		DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}

//	--------------------------------------------------------------------------------
void CvCity::DoResistanceTurn()
{
	VALIDATE_OBJECT
	if(IsResistance())
	{
		ChangeResistanceTurns(-1);
	}
}

//	--------------------------------------------------------------------------------
bool CvCity::IsRazing() const
{
	VALIDATE_OBJECT
	return GetRazingTurns() > 0;
}

//	--------------------------------------------------------------------------------
int CvCity::GetRazingTurns() const
{
	VALIDATE_OBJECT
	return m_iRazingTurns;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeRazingTurns(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iRazingTurns += iChange;


		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
		DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}

//	--------------------------------------------------------------------------------
bool CvCity::DoRazingTurn()
{
	VALIDATE_OBJECT

	if(IsRazing())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		int iPopulationDrop = 1;
		iPopulationDrop *= (100 + kPlayer.GetPlayerTraits()->GetRazeSpeedModifier());
		iPopulationDrop /= 100;

		ChangeRazingTurns(-1);

		// Counter has reached 0, disband the City
		if(GetRazingTurns() <= 0 || getPopulation() <= 1)
		{
			CvPlot* pkPlot = plot();
			kPlayer.disband(this);
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_CITY_DESTROYED, getOwner(), "", pkPlot->getX(), pkPlot->getY());
			return true;
		}
		// Counter is positive, reduce population
		else
		{
			changePopulation(-iPopulationDrop, true);
		}
	}

	return false;
}

/// Has this City been taken from its owner?
//	--------------------------------------------------------------------------------
bool CvCity::IsOccupied() const
{
	VALIDATE_OBJECT

	// If we're a puppet then we don't count as an occupied city
	if(IsPuppet())
		return false;

	return m_bOccupied;
}

//	--------------------------------------------------------------------------------
/// Has this City been taken from its owner?
void CvCity::SetOccupied(bool bValue)
{
	VALIDATE_OBJECT
	if(IsOccupied() != bValue)
	{
		m_bOccupied = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Has this City been turned into a puppet by someone capturing it?
bool CvCity::IsPuppet() const
{
	VALIDATE_OBJECT
	return m_bPuppet;
}

//	--------------------------------------------------------------------------------
/// Has this City been turned into a puppet by someone capturing it?
void CvCity::SetPuppet(bool bValue)
{
	VALIDATE_OBJECT
	if(IsPuppet() != bValue)
	{
		m_bPuppet = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Turn this City into a puppet
void CvCity::DoCreatePuppet()
{
	VALIDATE_OBJECT

	// Turn this off - used to display info for annex/puppet/raze popup
	SetIgnoreCityForHappiness(false);

	SetPuppet(true);

	setProductionAutomated(true, true);

	int iForceWorkingPuppetRange = 2;

	CvPlot* pLoopPlot;

	// Loop through all plots near this City
	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		pLoopPlot = plotCity(getX(), getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			// Cut off areas around the city we don't care about
			pLoopPlot = plotXYWithRangeCheck(pLoopPlot->getX(), pLoopPlot->getY(), getX(), getY(), iForceWorkingPuppetRange);

			if(pLoopPlot != NULL)
			{
				pLoopPlot->setWorkingCityOverride(this);
			}
		}
	}

	GET_PLAYER(getOwner()).DoUpdateHappiness();
	GET_PLAYER(getOwner()).DoUpdateNextPolicyCost();

	DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
	DLLUI->setDirty(GameData_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
/// Un-puppet a City and force it into the empire
void CvCity::DoAnnex()
{
	VALIDATE_OBJECT

	// Turn this off - used to display info for annex/puppet/raze popup
	SetIgnoreCityForHappiness(false);

	SetPuppet(false);

	setProductionAutomated(false, true);

	GET_PLAYER(getOwner()).DoUpdateHappiness();

	if(getOriginalOwner() != GetID())
	{
		if(GET_PLAYER(getOriginalOwner()).isMinorCiv())
		{
			if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(getOwner()).isHuman())
			{
				bool bUsingXP1Scenario1 = gDLL->IsModActivated(CIV5_XP1_SCENARIO1_MODID);
				if(!bUsingXP1Scenario1)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_CITYSTATE_ANNEX);
				}
			}
		}
	}

	GET_PLAYER(getOwner()).DoUpdateNextPolicyCost();

	DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
	DLLUI->setDirty(GameData_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
int CvCity::GetLocalHappiness() const
{
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);

	int iLocalHappiness = GetBaseHappinessFromBuildings();

	int iHappinessPerGarrison = kPlayer.GetHappinessPerGarrisonedUnit();
	if(iHappinessPerGarrison > 0)
	{
		if(GetGarrisonedUnit() != NULL)
		{
			iLocalHappiness++;
		}
	}

	// Follower beliefs
	int iHappinessFromReligion = 0;
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
	if(eMajority != NO_RELIGION)
	{
		int iFollowers = GetCityReligions()->GetNumFollowers(eMajority);

		const CvReligion* pReligion = pReligions->GetReligion(eMajority, kPlayer.GetID());
		if(pReligion)
		{
			iHappinessFromReligion += pReligion->m_Beliefs.GetHappinessPerCity(getPopulation());
			if(plot()->isRiver())
			{
				iHappinessFromReligion += pReligion->m_Beliefs.GetRiverHappiness();
			}

			// Buildings
			for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
			{
				BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
				if(!pkBuildingClassInfo)
				{
					continue;
				}

				CvCivilizationInfo& playerCivilizationInfo = kPlayer.getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

				if(eBuilding != NO_BUILDING)
				{
					if(GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						iHappinessFromReligion += pReligion->m_Beliefs.GetBuildingClassHappiness(eBuildingClass, iFollowers);
					}
				}
			}
		}
		iLocalHappiness += iHappinessFromReligion;
	}

	// Policy Building Mods
	int iSpecialPolicyBuildingHappiness = 0;
	int iBuildingClassLoop;
	BuildingClassTypes eBuildingClass;
	for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
	{
		PolicyTypes ePolicy = (PolicyTypes)iPolicyLoop;
		CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(ePolicy);
		if(pkPolicyInfo)
		{
			if(kPlayer.GetPlayerPolicies()->HasPolicy(ePolicy) && !kPlayer.GetPlayerPolicies()->IsPolicyBlocked(ePolicy))
			{
				for(iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					eBuildingClass = (BuildingClassTypes) iBuildingClassLoop;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if(!pkBuildingClassInfo)
					{
						continue;
					}

					BuildingTypes eBuilding = (BuildingTypes)kPlayer.getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
					if(eBuilding != NO_BUILDING && GetCityBuildings()->GetNumBuilding(eBuilding) > 0) // slewis - added the NO_BUILDING check for the ConquestDLX scenario which has civ specific wonders
					{
						if(pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass) != 0)
						{
							iSpecialPolicyBuildingHappiness += pkPolicyInfo->GetBuildingClassHappiness(eBuildingClass);
						}
					}
				}
			}
		}
	}

	iLocalHappiness += iSpecialPolicyBuildingHappiness;
	int iLocalHappinessCap = getPopulation();

	// India has unique way to compute local happiness cap
	if(kPlayer.GetPlayerTraits()->GetCityUnhappinessModifier() != 0)
	{
		// 0.67 per population, rounded up
		iLocalHappinessCap = (iLocalHappinessCap * 20) + 15;
		iLocalHappinessCap /= 30;
	}

	if(iLocalHappinessCap < iLocalHappiness)
	{
		return iLocalHappinessCap;
	}
	else
	{
		return iLocalHappiness;
	}
}

//	--------------------------------------------------------------------------------
int CvCity::GetHappinessFromBuildings() const
{
	return GetUnmoddedHappinessFromBuildings();
}

//	--------------------------------------------------------------------------------
int CvCity::GetBaseHappinessFromBuildings() const
{
	return m_iBaseHappinessFromBuildings;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeBaseHappinessFromBuildings(int iChange)
{
	m_iBaseHappinessFromBuildings += iChange;
}

//	--------------------------------------------------------------------------------
int CvCity::GetUnmoddedHappinessFromBuildings() const
{
	return m_iUnmoddedHappinessFromBuildings;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeUnmoddedHappinessFromBuildings(int iChange)
{
	m_iUnmoddedHappinessFromBuildings += iChange;
}

//	--------------------------------------------------------------------------------
/// Used when gathering info for "Annex/Puppet/Raze" popup
bool CvCity::IsIgnoreCityForHappiness() const
{
	return m_bIgnoreCityForHappiness;
}

//	--------------------------------------------------------------------------------
/// Used when gathering info for "Annex/Puppet/Raze" popup
void CvCity::SetIgnoreCityForHappiness(bool bValue)
{
	m_bIgnoreCityForHappiness = bValue;
}

//	--------------------------------------------------------------------------------
/// Find the non-wonder building that provides the highest culture at the least cost
BuildingTypes CvCity::ChooseFreeCultureBuilding() const
{
	BuildingTypes eRtnValue = NO_BUILDING;
	int iNumBuildingInfos = GC.getNumBuildingInfos();
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> buildingChoices;

	for(int iI = 0; iI < iNumBuildingInfos; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
			if(!isWorldWonderClass(kBuildingClassInfo) && !isNationalWonderClass(kBuildingClassInfo))
			{
				int iCulture = pkBuildingInfo->GetYieldChange(YIELD_CULTURE);
				int iCost = pkBuildingInfo->GetProductionCost();
				if(getFirstBuildingOrder(eBuilding) != -1 || canConstruct(eBuilding))
				{
					if(iCulture > 0 && iCost > 0)
					{
						int iWeight = iCulture * 10000 / iCost;

						if(iWeight > 0)
						{
							buildingChoices.push_back(iI, iWeight);
						}
					}
				}
			}
		}
	}

	if(buildingChoices.size() > 0)
	{
		buildingChoices.SortItems();
		eRtnValue = (BuildingTypes)buildingChoices.GetElement(0);
	}

	return eRtnValue;
}

//	--------------------------------------------------------------------------------
/// Find the non-wonder building that provides the highest culture at the least cost
BuildingTypes CvCity::ChooseFreeFoodBuilding() const
{
	BuildingTypes eRtnValue = NO_BUILDING;
	int iNumBuildingInfos = GC.getNumBuildingInfos();
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> buildingChoices;

	for(int iI = 0; iI < iNumBuildingInfos; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
			if(!isWorldWonderClass(kBuildingClassInfo) && !isNationalWonderClass(kBuildingClassInfo))
			{
				int iFood = pkBuildingInfo->GetFoodKept();
				int iCost = pkBuildingInfo->GetProductionCost();
				if(iFood > 0 && iCost > 0)
				{
					int iWeight = iFood * 10000 / iCost;

					if(iWeight > 0)
					{
						buildingChoices.push_back(iI, iWeight);
					}
				}
			}
		}
	}

	if(buildingChoices.size() > 0)
	{
		buildingChoices.SortItems();
		eRtnValue = (BuildingTypes)buildingChoices.GetElement(0);
	}

	return eRtnValue;
}

//	--------------------------------------------------------------------------------
int CvCity::getCitySizeBoost() const
{
	VALIDATE_OBJECT
	return m_iCitySizeBoost;
}


//	--------------------------------------------------------------------------------
void CvCity::setCitySizeBoost(int iBoost)
{
	VALIDATE_OBJECT
	if(getCitySizeBoost() != iBoost)
	{
		m_iCitySizeBoost = iBoost;

		setLayoutDirty(true);
	}
}


//	--------------------------------------------------------------------------------
bool CvCity::isNeverLost() const
{
	VALIDATE_OBJECT
	return m_bNeverLost;
}


//	--------------------------------------------------------------------------------
void CvCity::setNeverLost(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bNeverLost = bNewValue;
}


//	--------------------------------------------------------------------------------
bool CvCity::isDrafted() const
{
	VALIDATE_OBJECT
	return m_bDrafted;
}


//	--------------------------------------------------------------------------------
void CvCity::setDrafted(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bDrafted = bNewValue;
}


//	--------------------------------------------------------------------------------
bool CvCity::isAirliftTargeted() const
{
	VALIDATE_OBJECT
	return m_bAirliftTargeted;
}


//	--------------------------------------------------------------------------------
void CvCity::setAirliftTargeted(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bAirliftTargeted = bNewValue;
}

//	--------------------------------------------------------------------------------
bool CvCity::IsOwedCultureBuilding() const
{
	return m_bOwedCultureBuilding;
}

//	--------------------------------------------------------------------------------
void CvCity::SetOwedCultureBuilding(bool bNewValue)
{
	m_bOwedCultureBuilding = bNewValue;
}

//	--------------------------------------------------------------------------------
bool CvCity::IsBlockaded() const
{
	VALIDATE_OBJECT
	bool bHasWaterRouteBuilding = false;

	CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();

	// Loop through adding the available buildings
	for(int i = 0; i < GC.GetGameBuildings()->GetNumBuildings(); i++)
	{
		BuildingTypes eBuilding = (BuildingTypes)i;
		CvBuildingEntry* pkBuildingInfo = pkGameBuildings->GetEntry(i);
		if(pkBuildingInfo)
		{
			if(pkBuildingInfo->AllowsWaterRoutes())
			{
				if(GetCityBuildings()->GetNumActiveBuilding(eBuilding) > 0)
				{
					bHasWaterRouteBuilding = true;
					break;
				}
			}
		}
	}

	// there is no water route building, so it can't be blockaded
	if(!bHasWaterRouteBuilding)
	{
		return false;
	}

	int iRange = 2;
	CvPlot* pLoopPlot = NULL;

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iRange);
			if(!pLoopPlot)
			{
				continue;
			}

			if(pLoopPlot->isWater() && pLoopPlot->getVisibleEnemyDefender(getOwner()))
			{
				return true;
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Amount of turns left in WLTKD
int CvCity::GetWeLoveTheKingDayCounter() const
{
	VALIDATE_OBJECT
	return m_iWeLoveTheKingDayCounter;
}

//	--------------------------------------------------------------------------------
///Sets number of turns left in WLTKD
void CvCity::SetWeLoveTheKingDayCounter(int iValue)
{
	VALIDATE_OBJECT
	m_iWeLoveTheKingDayCounter = iValue;
}

//	--------------------------------------------------------------------------------
///Changes number of turns left in WLTKD
void CvCity::ChangeWeLoveTheKingDayCounter(int iChange)
{
	VALIDATE_OBJECT
	SetWeLoveTheKingDayCounter(GetWeLoveTheKingDayCounter() + iChange);
}

//	--------------------------------------------------------------------------------
/// Turn number when AI placed a garrison here
int CvCity::GetLastTurnGarrisonAssigned() const
{
	VALIDATE_OBJECT
	return m_iLastTurnGarrisonAssigned;
}

//	--------------------------------------------------------------------------------
/// Sets turn number when AI placed a garrison: AI sets to INT_MAX if city has walls and doesn't need a garrison to fire
void CvCity::SetLastTurnGarrisonAssigned(int iValue)
{
	VALIDATE_OBJECT
	m_iLastTurnGarrisonAssigned = iValue;
}

//	--------------------------------------------------------------------------------
int CvCity::GetNumThingsProduced() const
{
	VALIDATE_OBJECT
	return m_iThingsProduced;
}

//	--------------------------------------------------------------------------------
bool CvCity::isProductionAutomated() const
{
	VALIDATE_OBJECT
	return m_bProductionAutomated;
}


//	--------------------------------------------------------------------------------
void CvCity::setProductionAutomated(bool bNewValue, bool bClear)
{
	VALIDATE_OBJECT
	if(isProductionAutomated() != bNewValue)
	{
		m_bProductionAutomated = bNewValue;

		if((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}

		// if automated and not network game and all 3 modifiers down, clear the queue and choose again
		if(bNewValue && bClear)
		{
			clearOrderQueue();
		}
		if(!isProduction())
		{
			AI_chooseProduction(false /*bInterruptWonders*/);
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvCity::isLayoutDirty() const
{
	VALIDATE_OBJECT
	return m_bLayoutDirty;
}


//	--------------------------------------------------------------------------------
void CvCity::setLayoutDirty(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bLayoutDirty = bNewValue;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvCity::getPreviousOwner() const
{
	VALIDATE_OBJECT
	return m_ePreviousOwner;
}


//	--------------------------------------------------------------------------------
void CvCity::setPreviousOwner(PlayerTypes eNewValue)
{
	VALIDATE_OBJECT
	m_ePreviousOwner = eNewValue;
}


//	--------------------------------------------------------------------------------
PlayerTypes CvCity::getOriginalOwner() const
{
	VALIDATE_OBJECT
	return m_eOriginalOwner;
}


//	--------------------------------------------------------------------------------
void CvCity::setOriginalOwner(PlayerTypes eNewValue)
{
	VALIDATE_OBJECT
	m_eOriginalOwner = eNewValue;
}


//	--------------------------------------------------------------------------------
PlayerTypes CvCity::GetPlayersReligion() const
{
	VALIDATE_OBJECT
	return m_ePlayersReligion;
}


//	--------------------------------------------------------------------------------
void CvCity::SetPlayersReligion(PlayerTypes eNewValue)
{
	VALIDATE_OBJECT
	m_ePlayersReligion = eNewValue;
}

//	--------------------------------------------------------------------------------
TeamTypes CvCity::getTeam() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getTeam();
}


//	--------------------------------------------------------------------------------
int CvCity::getSeaPlotYield(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiSeaPlotYield[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiSeaPlotYield.setAt(eIndex, m_aiSeaPlotYield[eIndex] + iChange);
		CvAssert(getSeaPlotYield(eIndex) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getRiverPlotYield(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiRiverPlotYield[eIndex];
}

//	--------------------------------------------------------------------------------
void CvCity::changeRiverPlotYield(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiRiverPlotYield.setAt(eIndex, m_aiRiverPlotYield[eIndex] + iChange);
		CvAssert(getRiverPlotYield(eIndex) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getLakePlotYield(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiLakePlotYield[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeLakePlotYield(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiLakePlotYield.setAt(eIndex, m_aiLakePlotYield[eIndex] + iChange);
		CvAssert(getLakePlotYield(eIndex) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getSeaResourceYield(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiSeaResourceYield[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeSeaResourceYield(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiSeaResourceYield.setAt(eIndex, m_aiSeaResourceYield[eIndex] + iChange);
		CvAssert(getSeaResourceYield(eIndex) >= 0);

		updateYield();
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getBaseYieldRateModifier(YieldTypes eIndex, int iExtra, CvString* toolTipSink) const
{
	VALIDATE_OBJECT
	int iModifier = 0;
	int iTempMod;

	// Yield Rate Modifier
	iTempMod = getYieldRateModifier(eIndex);
	iModifier += iTempMod;
	if(toolTipSink)
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD", iTempMod);

	// Resource Yield Rate Modifier
	iTempMod = getResourceYieldRateModifier(eIndex);
	iModifier += iTempMod;
	if(toolTipSink)
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_RESOURCES", iTempMod);

	// Happiness Yield Rate Modifier
	iTempMod = getHappinessModifier(eIndex);
	iModifier += iTempMod;
	if(toolTipSink)
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_HAPPINESS", iTempMod);

	// Area Yield Rate Modifier
	if(area() != NULL)
	{
		iTempMod = area()->getYieldRateModifier(getOwner(), eIndex);
		iModifier += iTempMod;
		if(toolTipSink)
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_AREA", iTempMod);
	}

	// Player Yield Rate Modifier
	iTempMod = GET_PLAYER(getOwner()).getYieldRateModifier(eIndex);
	iModifier += iTempMod;
	if(toolTipSink)
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_PLAYER", iTempMod);

	// Player Capital Yield Rate Modifier
	if(isCapital())
	{
		iTempMod = GET_PLAYER(getOwner()).getCapitalYieldRateModifier(eIndex);
		iModifier += iTempMod;
		if(toolTipSink)
			GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_CAPITAL", iTempMod);
	}

	// Golden Age Yield Modifier
	if(GET_PLAYER(getOwner()).isGoldenAge())
	{
		CvYieldInfo* pYield = GC.getYieldInfo(eIndex);
		if(pYield)
		{
			iTempMod = pYield->getGoldenAgeYieldMod();
			iModifier += iTempMod;
			if(toolTipSink)
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_GOLDEN_AGE", iTempMod);
		}
	}

	// Religion Yield Rate Modifier
	ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
	if(pReligion)
	{
		int iReligionYieldMaxFollowers = pReligion->m_Beliefs.GetMaxYieldModifierPerFollower(eIndex);
		if (iReligionYieldMaxFollowers > 0)
		{
			int iFollowers = GetCityReligions()->GetNumFollowers(eMajority);
			iTempMod = min(iFollowers, iReligionYieldMaxFollowers);
			iModifier += iTempMod;
			if(toolTipSink)
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_YIELD_BELIEF", iTempMod);
		}
	}

	// Puppet
	if(IsPuppet())
	{
		switch(eIndex)
		{
		case YIELD_SCIENCE:
			iTempMod = GC.getPUPPET_SCIENCE_MODIFIER();
			iModifier += iTempMod;
			if(iTempMod != 0 && toolTipSink)
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_PUPPET", iTempMod);
		case YIELD_GOLD:
			iTempMod = GC.getPUPPET_GOLD_MODIFIER();
			iModifier += iTempMod;
			if(iTempMod != 0 && toolTipSink)
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_PRODMOD_PUPPET", iTempMod);
		}
	}

	iModifier += iExtra;

	// note: player->invalidateYieldRankCache() must be called for anything that is checked here
	// so if any extra checked things are added here, the cache needs to be invalidated

	return std::max(0, (iModifier + 100));
}

//	--------------------------------------------------------------------------------
int CvCity::getHappinessModifier(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	int iModifier = 0;

	// Production slows down when Empire is Very Unhappy
	if(eIndex == YIELD_PRODUCTION)
	{
		if(GET_PLAYER(getOwner()).IsEmpireVeryUnhappy())
		{
			iModifier += /*-50*/ GC.getVERY_UNHAPPY_PRODUCTION_PENALTY();
		}
	}

	return iModifier;
}

//	--------------------------------------------------------------------------------
int CvCity::getYieldRate(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	return (getYieldRateTimes100(eIndex) / 100);
}

//	--------------------------------------------------------------------------------
int CvCity::getYieldRateTimes100(YieldTypes eIndex) const
{
	VALIDATE_OBJECT

	// Resistance - no Science, Gold or Production (Prod handled in ProductionDifference)
	if(IsResistance() || IsRazing())
	{
		if(eIndex == YIELD_GOLD || eIndex == YIELD_SCIENCE)
		{
			return 0;
		}
	}

	int iProcessYield = 0;

	if(getProductionToYieldModifier(eIndex) != 0)
	{
		CvAssertMsg(eIndex != YIELD_PRODUCTION, "GAMEPLAY: should not be trying to convert Production into Production via process.");

		iProcessYield = getYieldRateTimes100(YIELD_PRODUCTION) * getProductionToYieldModifier(eIndex) / 100;
	}

	// Sum up yield rate
	int iBaseYield = getBaseYieldRate(eIndex) * 100;
	iBaseYield += (GetYieldPerPopTimes100(eIndex) * getPopulation());

	int iModifiedYield = iBaseYield * getBaseYieldRateModifier(eIndex);
	iModifiedYield /= 100;

	iModifiedYield += iProcessYield;

	return iModifiedYield;
}


//	--------------------------------------------------------------------------------
int CvCity::getBaseYieldRate(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	int iValue = 0;
	iValue += GetBaseYieldRateFromTerrain(eIndex);
	iValue += GetBaseYieldRateFromBuildings(eIndex);
	iValue += GetBaseYieldRateFromSpecialists(eIndex);
	iValue += GetBaseYieldRateFromMisc(eIndex);
	iValue += GetBaseYieldRateFromReligion(eIndex);

	return iValue;
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Terrain
int CvCity::GetBaseYieldRateFromTerrain(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiBaseYieldRateFromTerrain[eIndex];
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Terrain
void CvCity::ChangeBaseYieldRateFromTerrain(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiBaseYieldRateFromTerrain.setAt(eIndex, m_aiBaseYieldRateFromTerrain[eIndex] + iChange);

		// JAR - debugging
		s_lastYieldUsedToUpdateRateFromTerrain = eIndex;
		s_changeYieldFromTerreain = iChange;


		if(getTeam() == GC.getGame().getActiveTeam())
		{
			if(isCitySelected())
			{
				DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Buildings
int CvCity::GetBaseYieldRateFromBuildings(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiBaseYieldRateFromBuildings[eIndex];
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Buildings
void CvCity::ChangeBaseYieldRateFromBuildings(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiBaseYieldRateFromBuildings.setAt(eIndex, m_aiBaseYieldRateFromBuildings[eIndex] + iChange);

		if(getTeam() == GC.getGame().getActiveTeam())
		{
			if(isCitySelected())
			{
				DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
				//DLLUI->setDirty(InfoPane_DIRTY_BIT, true );
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Specialists
int CvCity::GetBaseYieldRateFromSpecialists(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiBaseYieldRateFromSpecialists[eIndex];
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Specialists
void CvCity::ChangeBaseYieldRateFromSpecialists(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiBaseYieldRateFromSpecialists.setAt(eIndex, m_aiBaseYieldRateFromSpecialists[eIndex] + iChange);

		if(getTeam() == GC.getGame().getActiveTeam())
		{
			if(isCitySelected())
			{
				DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Misc
int CvCity::GetBaseYieldRateFromMisc(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiBaseYieldRateFromMisc[eIndex];
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Misc
void CvCity::ChangeBaseYieldRateFromMisc(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiBaseYieldRateFromMisc.setAt(eIndex, m_aiBaseYieldRateFromMisc[eIndex] + iChange);

		if(getTeam() == GC.getGame().getActiveTeam())
		{
			if(isCitySelected())
			{
				DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Religion
int CvCity::GetBaseYieldRateFromReligion(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiBaseYieldRateFromReligion[eIndex];
}

//	--------------------------------------------------------------------------------
/// Base yield rate from Religion
void CvCity::ChangeBaseYieldRateFromReligion(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiBaseYieldRateFromReligion[eIndex] = m_aiBaseYieldRateFromReligion[eIndex] + iChange;

		if(getTeam() == GC.getGame().getActiveTeam())
		{
			if(isCitySelected())
			{
				DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Extra yield for each pop point
int CvCity::GetYieldPerPopTimes100(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	return m_aiYieldPerPop[eIndex];
}

//	--------------------------------------------------------------------------------
/// Extra yield for each pop point
void CvCity::ChangeYieldPerPopTimes100(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
		m_aiYieldPerPop.setAt(eIndex, m_aiYieldPerPop[eIndex] + iChange);
}

//	--------------------------------------------------------------------------------
int CvCity::getYieldRateModifier(YieldTypes eIndex)	const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiYieldRateModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiYieldRateModifier.setAt(eIndex, m_aiYieldRateModifier[eIndex] + iChange);
		CvAssert(getYieldRate(eIndex) >= 0);

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eIndex);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getResourceYieldRateModifier(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiResourceYieldRateModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeResourceYieldRateModifier(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiResourceYieldRateModifier.setAt(eIndex, m_aiResourceYieldRateModifier[eIndex] + iChange);
		CvAssert(getYieldRate(eIndex) >= 0);

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eIndex);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getExtraSpecialistYield(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiExtraSpecialistYield[eIndex];
}


//	--------------------------------------------------------------------------------
int CvCity::getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	CvAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	CvAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos expected to be >= 0");

	if (eSpecialist == GC.getDEFAULT_SPECIALIST())
	{
		return 0;
	}

	int iYieldMultiplier = GET_PLAYER(getOwner()).getSpecialistExtraYield(eSpecialist, eIndex) +
	                       GET_PLAYER(getOwner()).getSpecialistExtraYield(eIndex) +
	                       GET_PLAYER(getOwner()).GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, eIndex);
	int iExtraYield = GetCityCitizens()->GetSpecialistCount(eSpecialist) * iYieldMultiplier;

	return iExtraYield;
}


//	--------------------------------------------------------------------------------
void CvCity::updateExtraSpecialistYield(YieldTypes eYield)
{
	VALIDATE_OBJECT
	int iOldYield;
	int iNewYield;
	int iI;

	CvAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield expected to be < NUM_YIELD_TYPES");

	iOldYield = getExtraSpecialistYield(eYield);

	iNewYield = 0;

	for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iNewYield += getExtraSpecialistYield(eYield, ((SpecialistTypes)iI));
	}

	if(iOldYield != iNewYield)
	{
		m_aiExtraSpecialistYield.setAt(eYield, iNewYield);
		CvAssert(getExtraSpecialistYield(eYield) >= 0);

		ChangeBaseYieldRateFromSpecialists(eYield, (iNewYield - iOldYield));
	}
}


//	--------------------------------------------------------------------------------
void CvCity::updateExtraSpecialistYield()
{
	VALIDATE_OBJECT
	int iI;

	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		updateExtraSpecialistYield((YieldTypes)iI);
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getProductionToYieldModifier(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiProductionToYieldModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeProductionToYieldModifier(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if(iChange != 0)
	{
		m_aiProductionToYieldModifier.setAt(eIndex, m_aiProductionToYieldModifier[eIndex] + iChange);
	}
}

//	--------------------------------------------------------------------------------
int CvCity::getDomainFreeExperience(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	return m_aiDomainFreeExperience[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeDomainFreeExperience(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_aiDomainFreeExperience.setAt(eIndex, m_aiDomainFreeExperience[eIndex] + iChange);
	CvAssert(getDomainFreeExperience(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getDomainProductionModifier(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	return m_aiDomainProductionModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeDomainProductionModifier(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_aiDomainProductionModifier.setAt(eIndex, m_aiDomainProductionModifier[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
bool CvCity::isEverOwned(PlayerTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_abEverOwned[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setEverOwned(PlayerTypes eIndex, bool bNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	m_abEverOwned.setAt(eIndex, bNewValue);
}

//	--------------------------------------------------------------------------------
bool CvCity::isRevealed(TeamTypes eIndex, bool bDebug) const
{
	VALIDATE_OBJECT
	if(bDebug && GC.getGame().isDebugMode())
	{
		return true;
	}
	else
	{
		CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
		CvAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

		return m_abRevealed[eIndex];
	}
}


//	--------------------------------------------------------------------------------
bool CvCity::setRevealed(TeamTypes eIndex, bool bNewValue)
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot;
	int iI;

	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

	if(isRevealed(eIndex, false) != bNewValue)
	{
		m_abRevealed.setAt(eIndex, bNewValue);

		if(eIndex == GC.getGame().getActiveTeam())
		{
			for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
			{
				pLoopPlot = GetCityCitizens()->GetCityPlotFromIndex(iI);

			}
		}
		return true;
	}
	return false;
}


//	--------------------------------------------------------------------------------
const char* CvCity::getNameKey() const
{
	VALIDATE_OBJECT
	return m_strName.c_str();
}


//	--------------------------------------------------------------------------------
const CvString CvCity::getName() const
{
	VALIDATE_OBJECT
	return GetLocalizedText(m_strName.c_str());
}


//	--------------------------------------------------------------------------------
void CvCity::setName(const char* szNewValue, bool bFound)
{
	VALIDATE_OBJECT
	CvString strName(szNewValue);
	gDLL->stripSpecialCharacters(strName);

	if(!strName.IsEmpty())
	{
		if(GET_PLAYER(getOwner()).isCityNameValid(strName, false))
		{
			m_strName = strName;

			if(isCitySelected())
			{
				DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			}


			auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
			DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
		}
		if(bFound)
		{
			doFoundMessage();
		}
	}
}


//	--------------------------------------------------------------------------------
void CvCity::doFoundMessage()
{
	VALIDATE_OBJECT
	if(getOwner() == GC.getGame().getActivePlayer())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_CITY_HAS_BEEN_FOUNDED");
		localizedText << getNameKey();
		DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, -1, localizedText.toUTF8(), NULL /*ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath()*/, MESSAGE_TYPE_MAJOR_EVENT, NULL, NO_COLOR, getX(), getY());
	}

	Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_CITY_IS_FOUNDED");
	localizedText << getNameKey();
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_CITY_FOUNDED, getOwner(), localizedText.toUTF8(), getX(), getY());
}

//	--------------------------------------------------------------------------------
bool CvCity::IsExtraLuxuryResources()
{
	return (m_iCountExtraLuxuries > 0);
}

//	--------------------------------------------------------------------------------
void CvCity::SetExtraLuxuryResources(int iNewValue)
{
	m_iCountExtraLuxuries = iNewValue;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeExtraLuxuryResources(int iChange)
{
	m_iCountExtraLuxuries += iChange;
}

//	--------------------------------------------------------------------------------
int CvCity::getProjectProduction(ProjectTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");
	return m_paiProjectProduction[eIndex] / 100;
}


//	--------------------------------------------------------------------------------
void CvCity::setProjectProduction(ProjectTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	setProjectProductionTimes100(eIndex, iNewValue*100);
}


//	--------------------------------------------------------------------------------
void CvCity::changeProjectProduction(ProjectTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	changeProjectProductionTimes100(eIndex, iChange*100);
}

//	--------------------------------------------------------------------------------
int CvCity::getProjectProductionTimes100(ProjectTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");
	return m_paiProjectProduction[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setProjectProductionTimes100(ProjectTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");

	if(getProjectProductionTimes100(eIndex) != iNewValue)
	{
		m_paiProjectProduction.setAt(eIndex, iNewValue);
		CvAssert(getProjectProductionTimes100(eIndex) >= 0);

		if((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
		{
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
		}

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
		DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::changeProjectProductionTimes100(ProjectTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex expected to be < GC.getNumProjectInfos()");
	setProjectProductionTimes100(eIndex, (getProjectProductionTimes100(eIndex) + iChange));
}


//	--------------------------------------------------------------------------------
int CvCity::getSpecialistProduction(SpecialistTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiSpecialistProduction[eIndex] / 100;
}


//	--------------------------------------------------------------------------------
void CvCity::setSpecialistProduction(SpecialistTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	setSpecialistProductionTimes100(eIndex, iNewValue*100);
}


//	--------------------------------------------------------------------------------
void CvCity::changeSpecialistProduction(SpecialistTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	changeSpecialistProductionTimes100(eIndex,iChange*100);
}

//	--------------------------------------------------------------------------------
int CvCity::getSpecialistProductionTimes100(SpecialistTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	return m_paiSpecialistProduction[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setSpecialistProductionTimes100(SpecialistTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");

	if(getSpecialistProductionTimes100(eIndex) != iNewValue)
	{
		m_paiSpecialistProduction.setAt(eIndex, iNewValue);
		CvAssert(getSpecialistProductionTimes100(eIndex) >= 0);

		if((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
		{
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
		}

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
		DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::changeSpecialistProductionTimes100(SpecialistTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex expected to be < GC.getNumSpecialistInfos()");
	setSpecialistProductionTimes100(eIndex, (getSpecialistProductionTimes100(eIndex) + iChange));
}

//	--------------------------------------------------------------------------------
CvCityBuildings* CvCity::GetCityBuildings() const
{
	VALIDATE_OBJECT
	return m_pCityBuildings;
}

//	--------------------------------------------------------------------------------
int CvCity::getUnitProduction(UnitTypes eIndex)	const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProduction[eIndex] / 100;
}


//	--------------------------------------------------------------------------------
void CvCity::setUnitProduction(UnitTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	setUnitProductionTimes100(eIndex, iNewValue * 100);
}


//	--------------------------------------------------------------------------------
void CvCity::changeUnitProduction(UnitTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	changeUnitProductionTimes100(eIndex, iChange * 100);
}


//	--------------------------------------------------------------------------------
int CvCity::getUnitProductionTimes100(UnitTypes eIndex)	const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProduction[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setUnitProductionTimes100(UnitTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");

	if(getUnitProductionTimes100(eIndex) != iNewValue)
	{
		m_paiUnitProduction.setAt(eIndex, iNewValue);
		CvAssert(getUnitProductionTimes100(eIndex) >= 0);

		if((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
		{
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
		}

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
		DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_BANNER);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::changeUnitProductionTimes100(UnitTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	setUnitProductionTimes100(eIndex, (getUnitProductionTimes100(eIndex) + iChange));
}


//	--------------------------------------------------------------------------------
int CvCity::getUnitProductionTime(UnitTypes eIndex)	const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProductionTime[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setUnitProductionTime(UnitTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiUnitProductionTime.setAt(eIndex, iNewValue);
	CvAssert(getUnitProductionTime(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeUnitProductionTime(UnitTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	setUnitProductionTime(eIndex, (getUnitProductionTime(eIndex) + iChange));
}


//	--------------------------------------------------------------------------------
int CvCity::getGreatPeopleUnitRate(UnitTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiGreatPeopleUnitRate[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiGreatPeopleUnitRate.setAt(eIndex, iNewValue);
	CvAssert(getGreatPeopleUnitRate(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeGreatPeopleUnitRate(UnitTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	setGreatPeopleUnitRate(eIndex, (getGreatPeopleUnitRate(eIndex) + iChange));
}


//	--------------------------------------------------------------------------------
int CvCity::getGreatPeopleUnitProgress(UnitTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiGreatPeopleUnitProgress[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::setGreatPeopleUnitProgress(UnitTypes eIndex, int iNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiGreatPeopleUnitProgress.setAt(eIndex, iNewValue);
	CvAssert(getGreatPeopleUnitProgress(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeGreatPeopleUnitProgress(UnitTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	setGreatPeopleUnitProgress(eIndex, (getGreatPeopleUnitProgress(eIndex) + iChange));
}


//	--------------------------------------------------------------------------------
int CvCity::getUnitCombatFreeExperience(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatFreeExperience[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatFreeExperience.setAt(eIndex, m_paiUnitCombatFreeExperience[eIndex] + iChange);
	CvAssert(getUnitCombatFreeExperience(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getUnitCombatProductionModifier(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatProductionModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvCity::changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatProductionModifier.setAt(eIndex, m_paiUnitCombatProductionModifier[eIndex] + iChange);
	CvAssert(getUnitCombatProductionModifier(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getFreePromotionCount(PromotionTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	return m_paiFreePromotionCount[eIndex];
}


//	--------------------------------------------------------------------------------
bool CvCity::isFreePromotion(PromotionTypes eIndex) const
{
	VALIDATE_OBJECT
	return (getFreePromotionCount(eIndex) > 0);
}


//	--------------------------------------------------------------------------------
void CvCity::changeFreePromotionCount(PromotionTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	m_paiFreePromotionCount.setAt(eIndex, m_paiFreePromotionCount[eIndex] + iChange);
	CvAssert(getFreePromotionCount(eIndex) >= 0);
}


//	--------------------------------------------------------------------------------
int CvCity::getSpecialistFreeExperience() const
{
	VALIDATE_OBJECT
	return m_iSpecialistFreeExperience;
}


//	--------------------------------------------------------------------------------
void CvCity::changeSpecialistFreeExperience(int iChange)
{
	VALIDATE_OBJECT
	m_iSpecialistFreeExperience += iChange;
	CvAssert(m_iSpecialistFreeExperience >= 0);
}


//	--------------------------------------------------------------------------------
void CvCity::updateStrengthValue()
{
	VALIDATE_OBJECT
	// Default Strength
	int iStrengthValue = /*600*/ GC.getCITY_STRENGTH_DEFAULT();

	// Population mod
	iStrengthValue += getPopulation() * /*25*/ GC.getCITY_STRENGTH_POPULATION_CHANGE();

	// Building Defense
	int iBuildingDefense = m_pCityBuildings->GetBuildingDefense();

	iBuildingDefense *= (100 + m_pCityBuildings->GetBuildingDefenseMod());
	iBuildingDefense /= 100;

	iStrengthValue += iBuildingDefense;

	// Garrisoned Unit
	CvUnit* pGarrisonedUnit = GetGarrisonedUnit();
	int iStrengthFromUnits = 0;
	if(pGarrisonedUnit)
	{
		if(!pGarrisonedUnit->isOutOfAttacks())
		{
			int iMaxHits = GC.getMAX_HIT_POINTS();
			iStrengthFromUnits = pGarrisonedUnit->GetBaseCombatStrength() * 100 * (iMaxHits - pGarrisonedUnit->getDamage()) / iMaxHits;
		}
	}

	iStrengthValue += ((iStrengthFromUnits * 100) / /*300*/ GC.getCITY_STRENGTH_UNIT_DIVISOR());

	// Tech Progress increases City Strength
	int iTechProgress = GET_TEAM(getTeam()).GetTeamTechs()->GetNumTechsKnown() * 100 / GC.getNumTechInfos();

	// Want progress to be a value between 0 and 5
	double fTechProgress = iTechProgress / 100.0 * /*5*/ GC.getCITY_STRENGTH_TECH_BASE();
	double fTechExponent = /*2.0f*/ GC.getCITY_STRENGTH_TECH_EXPONENT();
	int iTechMultiplier = /*2*/ GC.getCITY_STRENGTH_TECH_MULTIPLIER();

	// The way all of this adds up...
	// 25% of the way through the game provides an extra 3.12
	// 50% of the way through the game provides an extra 12.50
	// 75% of the way through the game provides an extra 28.12
	// 100% of the way through the game provides an extra 50.00

	double fTechMod = pow(fTechProgress, fTechExponent);
	fTechMod *= iTechMultiplier;

	fTechMod *= 100;	// Bring it back into hundreds
	iStrengthValue += (int)(fTechMod + 0.005);	// Adding a small amount to prevent small fp accuracy differences from generating a different integer result on the Mac and PC. Assuming fTechMod is positive, round to nearest hundredth

	int iStrengthMod = 0;

	// Player-wide strength mod (Policies, etc.)
	iStrengthMod += GET_PLAYER(getOwner()).GetCityStrengthMod();

	// Apply Mod
	iStrengthValue *= (100 + iStrengthMod);
	iStrengthValue /= 100;

	m_iStrengthValue = iStrengthValue;

	// Terrain mod
	if(plot()->isHills())
	{
		m_iStrengthValue += /*3*/ GC.getCITY_STRENGTH_HILL_CHANGE();
	}

	DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
}

//	--------------------------------------------------------------------------------
int CvCity::getStrengthValue(bool bForRangeStrike) const
{
	VALIDATE_OBJECT
	// Strike strikes are weaker
	if(bForRangeStrike)
	{
		int iValue = m_iStrengthValue;

		iValue -= m_pCityBuildings->GetBuildingDefense();

		CvAssertMsg(iValue > 0, "City strength should always be greater than zero. Please show Jon this and send your last 5 autosaves.");

		iValue *= /*40*/ GC.getCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER();
		iValue /= 100;

		if(GetGarrisonedUnit())
		{
			iValue *= (100 + GET_PLAYER(m_eOwner).GetGarrisonedCityRangeStrikeModifier());
			iValue /= 100;
		}

		// Religion city strike mod
		int iReligionCityStrikeMod = 0;
		ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				iReligionCityStrikeMod = pReligion->m_Beliefs.GetCityRangeStrikeModifier();
				if(iReligionCityStrikeMod > 0)
				{
					iValue *= (100 + iReligionCityStrikeMod);
					iValue /= 100;
				}
			}
		}

		return iValue;
	}

	return m_iStrengthValue;
}

//	--------------------------------------------------------------------------------
int CvCity::GetPower() const
{
	VALIDATE_OBJECT
	return int(pow((double) getStrengthValue() / 100, 1.5));		// This is the same math used to calculate Unit Power in CvUnitEntry
}


//	--------------------------------------------------------------------------------
int CvCity::getDamage() const
{
	VALIDATE_OBJECT
	return m_iDamage;
}

//	--------------------------------------------------------------------------------
void CvCity::setDamage(int iValue, bool noMessage)
{
	float fDelay = 0.0f;

	VALIDATE_OBJECT

	if(iValue < 0)
		iValue = 0;
	else if(iValue > GetMaxHitPoints())
		iValue = GetMaxHitPoints();

	if(iValue != getDamage())
	{
		int iOldValue = getDamage();
		auto_ptr<ICvCity1> pDllCity(new CvDllCity(this));
		gDLL->GameplayCitySetDamage(pDllCity.get(), iValue, iOldValue);

		// send the popup text if the player can see this plot
		if(!noMessage && plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
		{
			char text[256];
			text[0] = NULL;
			int iNewValue = MIN(GetMaxHitPoints(),iValue);
			int iDiff = iOldValue - iNewValue;
			if(iNewValue < iOldValue)
			{
				sprintf_s(text, "[COLOR_GREEN]+%d[ENDCOLOR]", iDiff);
				fDelay = GC.getPOST_COMBAT_TEXT_DELAY() * 2;
			}
			else
			{
				sprintf_s(text, "[COLOR_RED]%d[ENDCOLOR]", iDiff);
			}

			DLLUI->AddPopupText(m_iX, m_iY, text, fDelay);
		}
		m_iDamage = iValue;
	}
}

//	--------------------------------------------------------------------------------
void CvCity::changeDamage(int iChange)
{
	VALIDATE_OBJECT
	if(0 != iChange)
	{
		setDamage(getDamage() + iChange);
	}
}

//	--------------------------------------------------------------------------------
/// Can a specific plot be bought for the city
bool CvCity::CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost)
{
	VALIDATE_OBJECT
	CvPlot* pTargetPlot = NULL;

	if(GC.getBUY_PLOTS_DISABLED())
	{
		return false;
	}

	pTargetPlot = GC.getMap().plot(iPlotX, iPlotY);

	if(!pTargetPlot)
	{
		// no plot to buy
		return false;
	}

	// if this plot belongs to someone, bail!
	if(pTargetPlot->getOwner() != NO_PLAYER)
	{
		return false;
	}

	// Must be adjacent to a plot owned by this city
	CvPlot* pAdjacentPlot;
	bool bFoundAdjacent = false;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pTargetPlot->getX(), pTargetPlot->getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->getOwner() == getOwner())
			{
				if(pAdjacentPlot->GetCityPurchaseID() == GetID())
				{
					bFoundAdjacent = true;
					break;
				}
			}
		}
	}

	if(!bFoundAdjacent)
		return false;

	// Max range of 3
	const int iMaxRange = /*3*/ GC.getMAXIMUM_BUY_PLOT_DISTANCE();
	if(plotDistance(iPlotX, iPlotY, getX(), getY()) > iMaxRange)
		return false;

	// check money
	if(!bIgnoreCost)
	{
		if(GET_PLAYER(getOwner()).GetTreasury()->GetGold() < GetBuyPlotCost(pTargetPlot->getX(), pTargetPlot->getY()))
		{
			return false;
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(iPlotX);
		args->Push(iPlotY);

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanBuyPlot", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
/// Can this city buy a plot, any plot?
bool CvCity::CanBuyAnyPlot(void)
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot = NULL;
	CvPlot* pThisPlot = plot();
	const int iMaxRange = GC.getMAXIMUM_BUY_PLOT_DISTANCE();
	CvMap& thisMap = GC.getMap();

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());

		// Attempt to execute the game events.
		// Will return false if there are no registered listeners.
		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "CityCanBuyAnyPlot", args.get(), bResult))
		{
			// Check the result.
			if(bResult == false)
			{
				return false;
			}
		}
	}

	for(int iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
	{
		for(int iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMaxRange);
			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getOwner() != NO_PLAYER)
				{
					continue;
				}

				// we can use the faster, but slightly inaccurate pathfinder here - after all we just need the existence of a path
				int iInfluenceCost = thisMap.calculateInfluenceDistance(pThisPlot, pLoopPlot, iMaxRange, false);

				if(iInfluenceCost > 0)
				{
					return true;
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
/// Which plot will we buy next
CvPlot* CvCity::GetNextBuyablePlot(void)
{
	VALIDATE_OBJECT
	std::vector<int> aiPlotList;
	aiPlotList.resize(20, -1);
	GetBuyablePlotList(aiPlotList);

	int iListLength = 0;
	for(uint ui = 0; ui < aiPlotList.size(); ui++)
	{
		if(aiPlotList[ui] >= 0)
		{
			iListLength++;
		}
		else
		{
			break;
		}
	}

	CvPlot* pPickedPlot = NULL;
	if(iListLength > 0)
	{
		int iPickedIndex = GC.getGame().getJonRandNum(iListLength, "GetNextBuyablePlot picker");
		pPickedPlot = GC.getMap().plotByIndex(aiPlotList[iPickedIndex]);
	}

	return pPickedPlot;
}

//	--------------------------------------------------------------------------------
void CvCity::GetBuyablePlotList(std::vector<int>& aiPlotList)
{
	VALIDATE_OBJECT
	aiPlotList.resize(20, -1);
	int iResultListIndex = 0;

	int iLowestCost = INT_MAX;
	CvPlot* pLoopPlot = NULL;
	CvPlot* pThisPlot = plot();
	const int iMaxRange = /*5*/ GC.getMAXIMUM_ACQUIRE_PLOT_DISTANCE();
	CvMap& thisMap = GC.getMap();
	TeamTypes thisTeam = getTeam();

	int iPLOT_INFLUENCE_DISTANCE_MULTIPLIER =	/*100*/ GC.getPLOT_INFLUENCE_DISTANCE_MULTIPLIER();
	int iPLOT_INFLUENCE_RING_COST =				/*100*/ GC.getPLOT_INFLUENCE_RING_COST();
	int iPLOT_INFLUENCE_WATER_COST =			/* 25*/ GC.getPLOT_INFLUENCE_WATER_COST();
	int iPLOT_INFLUENCE_IMPROVEMENT_COST =		/* -5*/ GC.getPLOT_INFLUENCE_IMPROVEMENT_COST();
	int iPLOT_INFLUENCE_ROUTE_COST =			/*0*/	GC.getPLOT_INFLUENCE_ROUTE_COST();
	int iPLOT_INFLUENCE_RESOURCE_COST =			/*-105*/ GC.getPLOT_INFLUENCE_RESOURCE_COST();
	int iPLOT_INFLUENCE_NW_COST =				/*-105*/ GC.getPLOT_INFLUENCE_NW_COST();
	int iPLOT_INFLUENCE_YIELD_POINT_COST =		/*-1*/	GC.getPLOT_INFLUENCE_YIELD_POINT_COST();

	int iPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST = /*1000*/ GC.getPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST();

	int iYieldLoop;

	int iDirectionLoop;
	bool bFoundAdjacentOwnedByCity;

	int iDX, iDY;

	ImprovementTypes eBarbCamptype = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

	for (iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
	{
		for (iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMaxRange);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getOwner() != NO_PLAYER)
				{
					continue;
				}

				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(pLoopPlot->getX());
					args->Push(pLoopPlot->getY());

					bool bResult = false;
					if (LuaSupport::CallTestAll(pkScriptSystem, "CityCanAcquirePlot", args.get(), bResult))
					{
						if (bResult == false) {
							continue;
						}
					}
				}

				// we can use the faster, but slightly inaccurate pathfinder here - after all we are using a rand in the equation
				int iInfluenceCost = thisMap.calculateInfluenceDistance(pThisPlot, pLoopPlot, iMaxRange, false) * iPLOT_INFLUENCE_DISTANCE_MULTIPLIER;

				if (iInfluenceCost > 0)
				{
					// Modifications for tie-breakers in a ring

					// Resource Plots claimed first
					ResourceTypes eResource = pLoopPlot->getResourceType(thisTeam);
					if (eResource != NO_RESOURCE)
					{
						iInfluenceCost += iPLOT_INFLUENCE_RESOURCE_COST;
						bool bBonusResource = GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_BONUS;
						if (bBonusResource)
						{
							if (plotDistance(pLoopPlot->getX(),pLoopPlot->getY(),getX(),getY()) > NUM_CITY_RINGS)
							{
								// undo the bonus - we can't work this tile from this city
								iInfluenceCost -= iPLOT_INFLUENCE_RESOURCE_COST;
							}
							else
							{
								// very slightly decrease value of bonus resources
								++iInfluenceCost;
							}
						}
					}
					else 
					{

						// Water Plots claimed later
						if (pLoopPlot->isWater())
						{
							iInfluenceCost += iPLOT_INFLUENCE_WATER_COST;
						}

						// if we can't work this tile in this city make it much less likely to be picked
						if (plotDistance(pLoopPlot->getX(),pLoopPlot->getY(),getX(),getY()) > NUM_CITY_RINGS)
						{
							iInfluenceCost += iPLOT_INFLUENCE_RING_COST;
						}

					}

					// improved tiles get a slight priority (unless they are barbarian camps!)
					ImprovementTypes thisImprovement = pLoopPlot->getImprovementType();
					if (thisImprovement != NO_IMPROVEMENT)
					{
						if (thisImprovement == eBarbCamptype)
						{
							iInfluenceCost += iPLOT_INFLUENCE_RING_COST;
						}
						else
						{
							iInfluenceCost += iPLOT_INFLUENCE_IMPROVEMENT_COST;
						}
					}

					// roaded tiles get a priority - [not any more: weight above is 0 by default]
					if (pLoopPlot->getRouteType() != NO_ROUTE)
					{
						iInfluenceCost += iPLOT_INFLUENCE_ROUTE_COST;
					}

					// while we're at it grab Natural Wonders quickly also
					if (pLoopPlot->IsNaturalWonder())
					{
						iInfluenceCost += iPLOT_INFLUENCE_NW_COST;
					}

					// More Yield == more desirable
					for (iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
					{
						iInfluenceCost += (iPLOT_INFLUENCE_YIELD_POINT_COST * pLoopPlot->getYield((YieldTypes) iYieldLoop));
					}

					// all other things being equal move towards unclaimed resources
					bool bUnownedNaturalWonderAdjacentCount = false;
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX(), pLoopPlot->getY(), ((DirectionTypes)iI));

						if (pAdjacentPlot != NULL)
						{
							if (pAdjacentPlot->getOwner() == NO_PLAYER)
							{
								int iPlotDistance = plotDistance(getX(), getY(), pAdjacentPlot->getX(), pAdjacentPlot->getY());
								ResourceTypes eAdjacentResource = pAdjacentPlot->getResourceType(thisTeam);
								if (eAdjacentResource != NO_RESOURCE)
								{
									// if we are close enough to work, or this is not a bonus resource
									if (iPlotDistance <= NUM_CITY_RINGS || GC.getResourceInfo(eAdjacentResource)->getResourceUsage() != RESOURCEUSAGE_BONUS)
									{
										--iInfluenceCost;
									}
								}
								if (pAdjacentPlot->IsNaturalWonder())
								{
									if (iPlotDistance <= NUM_CITY_RINGS) // grab for this city
									{
										bUnownedNaturalWonderAdjacentCount = true;
									}
									--iInfluenceCost; // but we will slightly grow towards it for style in any case
								}
							}
						}
					}

					// move towards unclaimed NW
					iInfluenceCost += bUnownedNaturalWonderAdjacentCount ? -1 : 0;

					// Plots not adjacent to another Plot acquired by this City are pretty much impossible to get
					bFoundAdjacentOwnedByCity = false;
					for (iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
					{
						CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX(), pLoopPlot->getY(), (DirectionTypes) iDirectionLoop);

						if (pAdjacentPlot != NULL)
						{
							// Have to check plot ownership first because the City IDs match between different players!!!
							if (pAdjacentPlot->getOwner() == getOwner() && pAdjacentPlot->GetCityPurchaseID() == GetID())
							{
								bFoundAdjacentOwnedByCity = true;
								break;
							}
						}
					}
					if (!bFoundAdjacentOwnedByCity)
					{
						iInfluenceCost += iPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST;
					}

					// Are we cheap enough to get picked next?
					if (iInfluenceCost < iLowestCost)
					{
						// clear reset list
						for(uint ui = 0; ui < aiPlotList.size(); ui++)
						{
							aiPlotList[ui] = -1;
						}
						iResultListIndex = 0;
						iLowestCost = iInfluenceCost;
						// this will "fall through" to the next conditional
					}

					if (iInfluenceCost == iLowestCost)
					{
						aiPlotList[iResultListIndex] = pLoopPlot->GetPlotIndex();
						iResultListIndex++;
					}
				}
			}
		}
	}

}

//	--------------------------------------------------------------------------------
/// How much will purchasing this plot cost -- (-1,-1) will return the generic price
int CvCity::GetBuyPlotCost(int iPlotX, int iPlotY) const
{
	VALIDATE_OBJECT
	if(iPlotX == -1 && iPlotY == -1)
	{
		return GET_PLAYER(getOwner()).GetBuyPlotCost();
	}

	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(!pPlot)
	{
		return -1;
	}

	// Base cost
	int iCost = GET_PLAYER(getOwner()).GetBuyPlotCost();

	// Influence cost factor (e.g. Hills are more expensive than flat land)
	CvMap& thisMap = GC.getMap();
	CvPlot* pThisPlot = plot();
	const int iMaxRange = /*3*/ GC.getMAXIMUM_BUY_PLOT_DISTANCE();
	if(plotDistance(iPlotX, iPlotY, getX(), getY()) > iMaxRange)
		return 9999; // Critical hit!

	int iPLOT_INFLUENCE_BASE_MULTIPLIER = /*100*/ GC.getPLOT_INFLUENCE_BASE_MULTIPLIER();
	int iPLOT_INFLUENCE_DISTANCE_MULTIPLIER = /*100*/ GC.getPLOT_INFLUENCE_DISTANCE_MULTIPLIER();
	int iPLOT_INFLUENCE_DISTANCE_DIVISOR = /*3*/ GC.getPLOT_INFLUENCE_DISTANCE_DIVISOR();
	int iPLOT_BUY_RESOURCE_COST = /*-100*/ GC.getPLOT_BUY_RESOURCE_COST();
	int iDistance = thisMap.calculateInfluenceDistance(pThisPlot, pPlot, iMaxRange, false);
	iDistance -= GetCheapestPlotInfluence(); // Reduce distance by the cheapest available (so that the costs don't ramp up ridiculously fast)

	int iInfluenceCostFactor = iPLOT_INFLUENCE_BASE_MULTIPLIER;
	iInfluenceCostFactor += (iDistance * iPLOT_INFLUENCE_DISTANCE_MULTIPLIER) / iPLOT_INFLUENCE_DISTANCE_DIVISOR;
	if(pPlot->getResourceType(getTeam()) != NO_RESOURCE)
		iInfluenceCostFactor += iPLOT_BUY_RESOURCE_COST;

	if(iInfluenceCostFactor > 100)
	{
		iCost *= iInfluenceCostFactor;
		iCost /= 100;
	}

	// Game Speed Mod
	iCost *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iCost /= 100;

	iCost *= (100 + getPlotBuyCostModifier());
	iCost /= 100;

	// Now round so the number looks neat
	int iDivisor = /*5*/ GC.getPLOT_COST_APPEARANCE_DIVISOR();
	iCost /= iDivisor;
	iCost *= iDivisor;

	return iCost;
}

//	--------------------------------------------------------------------------------
/// Buy the plot and set it's owner to us (executed by the network code)
void CvCity::BuyPlot(int iPlotX, int iPlotY)
{
	VALIDATE_OBJECT
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(!pPlot)
	{
		return;
	}

	int iCost = GetBuyPlotCost(iPlotX, iPlotY);
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	thisPlayer.GetTreasury()->ChangeGold(-iCost);
	thisPlayer.ChangeNumPlotsBought(1);

	// See if there's anyone else nearby that could get upset by this action
	CvCity* pNearbyCity;
	for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pPlot = plotCity(iPlotX, iPlotY, iI);

		if(pPlot != NULL)
		{
			pNearbyCity = pPlot->getPlotCity();

			if(pNearbyCity)
			{
				if(pNearbyCity->getOwner() != getOwner())
				{
					pNearbyCity->AI_ChangeNumPlotsAcquiredByOtherPlayer(getOwner(), 1);
				}
			}
		}
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		CvPlayerAI& kOwner = GET_PLAYER(getOwner());
		CvString playerName;
		FILogFile* pLog;
		CvString strBaseString;
		CvString strOutBuf;
		playerName = kOwner.getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(kOwner.GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";
		strOutBuf.Format("%s, City Plot Purchased, X: %d, Y: %d", getName().GetCString(), iPlotX, iPlotY);
		strBaseString += strOutBuf;
		pLog->Msg(strBaseString);
	}
	DoAcquirePlot(iPlotX, iPlotY);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) 
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(plot()->getX());
		args->Push(plot()->getY());
		args->Push(true); // bGold
		args->Push(false); // bFaith/bCulture

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "CityBoughtPlot", args.get(), bResult);
	}

	//Achievement test for purchasing 1000 tiles
	if(thisPlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_TILESPURCHASED, 1000, ACHIEVEMENT_PURCHASE_1000TILES);
	}
}

//	--------------------------------------------------------------------------------
/// Acquire the plot and set it's owner to us
void CvCity::DoAcquirePlot(int iPlotX, int iPlotY)
{
	VALIDATE_OBJECT
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(!pPlot)
	{
		return;
	}

	GET_PLAYER(getOwner()).AddAPlot(pPlot);
	pPlot->setOwner(getOwner(), GetID(), /*bCheckUnits*/ true, /*bUpdateResources*/ true);

	DoUpdateCheapestPlotInfluence();
}

//	--------------------------------------------------------------------------------
/// Compute how valuable buying a plot is to this city
int CvCity::GetBuyPlotScore(int& iBestX, int& iBestY)
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot = NULL;
	const int iMaxRange = /*3*/ GC.getMAXIMUM_BUY_PLOT_DISTANCE();

	int iBestScore = -1;
	int iTempScore;

	int iDX, iDY;

	for(iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
	{
		for(iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMaxRange);
			if(pLoopPlot != NULL)
			{
				// Can we actually buy this plot?
				if(CanBuyPlot(pLoopPlot->getX(), pLoopPlot->getY()))
				{
					iTempScore = GetIndividualPlotScore(pLoopPlot);

					if(iTempScore > iBestScore)
					{
						iBestScore = iTempScore;
						iBestX = pLoopPlot->getX();
						iBestY = pLoopPlot->getY();
					}
				}
			}
		}
	}

	return iBestScore;
}

//	--------------------------------------------------------------------------------
/// Compute value of a plot we might buy
int CvCity::GetIndividualPlotScore(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iRtnValue = 0;
	ResourceTypes eResource;
	int iYield;
	int iI;
	YieldTypes eSpecializationYield = NO_YIELD;
	CitySpecializationTypes eSpecialization;
	CvCity* pCity;

	eSpecialization = GetCityStrategyAI()->GetSpecialization();

	if(eSpecialization != NO_CITY_SPECIALIZATION)
	{
		eSpecializationYield = GC.getCitySpecializationInfo(eSpecialization)->GetYieldType();
	}

	// Does it have a resource?
	eResource = pPlot->getResourceType(getTeam());
	if(eResource != NO_RESOURCE)
	{
		if(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) GC.getResourceInfo(eResource)->getTechReveal()))
		{
			ResourceUsageTypes eResourceUsage = GC.getResourceInfo(eResource)->getResourceUsage();
			if(eResourceUsage == RESOURCEUSAGE_STRATEGIC)
			{
				iRtnValue += /* 50 */ GC.getAI_PLOT_VALUE_STRATEGIC_RESOURCE();
			}

			// Luxury resource?
			else if(eResourceUsage == RESOURCEUSAGE_LUXURY)
			{
				int iLuxuryValue = /* 40 */ GC.getAI_PLOT_VALUE_LUXURY_RESOURCE();

				// Luxury we don't have yet?
				if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource) == 0)
					iLuxuryValue *= 2;

				iRtnValue += iLuxuryValue;
			}
		}
	}

	int iYieldValue = 0;
	int iTempValue;

	YieldTypes eYield;

	CvCityStrategyAI* pCityStrategyAI = GetCityStrategyAI();

	// Valuate the yields from this plot
	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		eYield = (YieldTypes) iI;

		iYield = pPlot->calculateNatureYield(eYield, getTeam());
		iTempValue = 0;

		if(eYield == eSpecializationYield)
			iTempValue += iYield* /*20*/ GC.getAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER();

		else
			iTempValue += iYield* /*10*/ GC.getAI_PLOT_VALUE_YIELD_MULTIPLIER();

		// Deficient? If so, give it a boost
		if(pCityStrategyAI->IsYieldDeficient(eYield))
			iTempValue *= /*5*/ GC.getAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER();

		iYieldValue += iTempValue;
	}

	iRtnValue += iYieldValue;

	// For each player not on our team, check how close their nearest city is to this plot
	CvPlayer& owningPlayer = GET_PLAYER(m_eOwner);
	CvDiplomacyAI* owningPlayerDiploAI = owningPlayer.GetDiplomacyAI();
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		CvPlayer& loopPlayer = GET_PLAYER((PlayerTypes)iI);
		if(loopPlayer.isAlive())
		{
			if(loopPlayer.getTeam() != getTeam())
			{
				DisputeLevelTypes eLandDisputeLevel = owningPlayerDiploAI->GetLandDisputeLevel((PlayerTypes)iI);

				if(eLandDisputeLevel != NO_DISPUTE_LEVEL && eLandDisputeLevel != DISPUTE_LEVEL_NONE)
				{
					pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), (PlayerTypes)iI, NO_TEAM, true /*bSameArea */);

					if(pCity)
					{
						int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pCity->getX(), pCity->getY());

						// Only want to account for civs with a city within 10 tiles
						if(iDistance < 10)
						{
							switch(eLandDisputeLevel)
							{
							case DISPUTE_LEVEL_FIERCE:
								iRtnValue += (10 - iDistance) * /* 6 */ GC.getAI_PLOT_VALUE_FIERCE_DISPUTE();
								break;
							case DISPUTE_LEVEL_STRONG:
								iRtnValue += (10 - iDistance) * /* 4 */GC.getAI_PLOT_VALUE_STRONG_DISPUTE();
								break;
							case DISPUTE_LEVEL_WEAK:
								iRtnValue += (10 - iDistance) * /* 2 */ GC.getAI_PLOT_VALUE_WEAK_DISPUTE();
								break;
							}
						}
					}
				}
			}
		}
	}

	// Modify value based on cost - the higher it is compared to the "base" cost the less the value
	int iCost = GetBuyPlotCost(pPlot->getX(), pPlot->getY());
	iRtnValue *= GET_PLAYER(getOwner()).GetBuyPlotCost();

	// Protect against div by 0.
	CvAssertMsg(iCost != 0, "Plot cost is 0");
	if(iCost != 0)
		iRtnValue /= iCost;
	else
		iRtnValue = 0;

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
/// What is the cheapest plot we can get
int CvCity::GetCheapestPlotInfluence() const
{
	return m_iCheapestPlotInfluence;
}

//	--------------------------------------------------------------------------------
/// What is the cheapest plot we can get
void CvCity::SetCheapestPlotInfluence(int iValue)
{
	if(m_iCheapestPlotInfluence != iValue)
		m_iCheapestPlotInfluence = iValue;

	CvAssertMsg(m_iCheapestPlotInfluence > 0, "Cheapest plot influence should never be 0 or less.");
}

//	--------------------------------------------------------------------------------
/// What is the cheapest plot we can get
void CvCity::DoUpdateCheapestPlotInfluence()
{
	int iLowestCost = INT_MAX;

	CvPlot* pLoopPlot = NULL;
	CvPlot* pThisPlot = plot();
	const int iMaxRange = /*5*/ GC.getMAXIMUM_ACQUIRE_PLOT_DISTANCE();
	CvMap& thisMap = GC.getMap();

	int iDX, iDY;

	for(iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
	{
		for(iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMaxRange);
			if(pLoopPlot != NULL)
			{
				// If the plot's not owned by us, it doesn't matter
				if(pLoopPlot->getOwner() != NO_PLAYER)
					continue;

				// we can use the faster, but slightly inaccurate pathfinder here - after all we are using a rand in the equation
				int iInfluenceCost = thisMap.calculateInfluenceDistance(pThisPlot, pLoopPlot, iMaxRange, false);

				if(iInfluenceCost > 0)
				{
					// Are we the cheapest yet?
					if(iInfluenceCost < iLowestCost)
						iLowestCost = iInfluenceCost;
				}
			}
		}
	}

	SetCheapestPlotInfluence(iLowestCost);
}

//	--------------------------------------------------------------------------------
/// Setting the danger value threat amount
void CvCity::SetThreatValue(int iThreatValue)
{
	VALIDATE_OBJECT
	m_iThreatValue = iThreatValue;
}

//	--------------------------------------------------------------------------------
/// Getting the danger value threat amount
int CvCity::getThreatValue(void)
{
	VALIDATE_OBJECT
	return m_iThreatValue;
}

//	--------------------------------------------------------------------------------
void CvCity::clearOrderQueue()
{
	VALIDATE_OBJECT
	while(headOrderQueueNode() != NULL)
	{
		popOrder(0);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bRush)
{
	VALIDATE_OBJECT
	OrderData order;
	bool bValid;

	if(bPop)
	{
		clearOrderQueue();
	}

	bValid = false;

	switch(eOrder)
	{
	case ORDER_TRAIN:
		if(canTrain((UnitTypes)iData1))
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo((UnitTypes)iData1);
			if(pkUnitInfo)
			{
				if(iData2 == -1)
				{
					iData2 = pkUnitInfo->GetDefaultUnitAIType();
				}

				GET_PLAYER(getOwner()).changeUnitClassMaking(((UnitClassTypes)(pkUnitInfo->GetUnitClassType())), 1);

				bValid = true;
			}
		}
		break;

	case ORDER_CONSTRUCT:
		if(canConstruct((BuildingTypes)iData1))
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo((BuildingTypes)iData1);
			if(pkBuildingInfo)
			{
				GET_PLAYER(getOwner()).changeBuildingClassMaking(((BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType())), 1);

				bValid = true;
			}
		}
		break;

	case ORDER_CREATE:
		if(canCreate((ProjectTypes)iData1))
		{
			GET_TEAM(getTeam()).changeProjectMaking(((ProjectTypes)iData1), 1);
			GET_PLAYER(getOwner()).changeProjectMaking(((ProjectTypes)iData1), 1);

			bValid = true;
		}
		break;

	case ORDER_PREPARE:
		if(canPrepare((SpecialistTypes)iData1))
		{
			bValid = true;
		}
		break;

	case ORDER_MAINTAIN:
		if(canMaintain((ProcessTypes)iData1))
		{
			bValid = true;
		}
		break;

	default:
		CvAssertMsg(false, "iOrder did not match a valid option");
		break;
	}

	if(!bValid)
	{
		return;
	}

	order.eOrderType = eOrder;
	order.iData1 = iData1;
	order.iData2 = iData2;
	order.bSave = bSave;
	order.bRush = bRush;

	if(bAppend)
	{
		m_orderQueue.insertAtEnd(&order);
	}
	else
	{
		stopHeadOrder();
		m_orderQueue.insertAtBeginning(&order);
	}

	if(!bAppend || (getOrderQueueLength() == 1))
	{
		startHeadOrder();
	}

	if((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		if(isCitySelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	DLLUI->setDirty(CityInfo_DIRTY_BIT, true);

	auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(this);
	DLLUI->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_PRODUCTION);
}


//	--------------------------------------------------------------------------------
void CvCity::popOrder(int iNum, bool bFinish, bool bChoose)
{
	VALIDATE_OBJECT

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());	//Used often later on

	OrderData* pOrderNode;
	SpecialistTypes eSpecialist;
	ProjectTypes eCreateProject;
	BuildingTypes eConstructBuilding;
	UnitTypes eTrainUnit;
	UnitAITypes eTrainAIUnit;
	bool bWasFoodProduction;
	bool bStart;
	bool bMessage;
	int iCount;
	int iProductionNeeded;

	bWasFoodProduction = isFoodProduction();

	if(iNum == -1)
	{
		iNum = (getOrderQueueLength() - 1);
	}

	iCount = 0;

	pOrderNode = headOrderQueueNode();

	while(pOrderNode != NULL)
	{
		if(iCount == iNum)
		{
			break;
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	if(pOrderNode == NULL)
	{
		return;
	}

	if(bFinish)
	{
		m_iThingsProduced++;
	}

	if(bFinish && pOrderNode->bSave)
	{
		pushOrder(pOrderNode->eOrderType, pOrderNode->iData1, pOrderNode->iData2, true, false, true);
	}

	eTrainUnit = NO_UNIT;
	eConstructBuilding = NO_BUILDING;
	eCreateProject = NO_PROJECT;
	eSpecialist = NO_SPECIALIST;

	switch(pOrderNode->eOrderType)
	{
	case ORDER_TRAIN:
		eTrainUnit = ((UnitTypes)(pOrderNode->iData1));
		eTrainAIUnit = ((UnitAITypes)(pOrderNode->iData2));
		CvAssertMsg(eTrainUnit != NO_UNIT, "eTrainUnit is expected to be assigned a valid unit type");
		CvAssertMsg(eTrainAIUnit != NO_UNITAI, "eTrainAIUnit is expected to be assigned a valid unit AI type");

		kOwner.changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo(eTrainUnit)->GetUnitClassType())), -1);

		if(bFinish)
		{
			int iResult = CreateUnit(eTrainUnit, eTrainAIUnit);
			if(iResult != FFreeList::INVALID_INDEX)
			{
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(GET_PLAYER(getOwner()).getUnit(iResult)->GetID()); // This is probably just iResult
					args->Push(false); // bGold
					args->Push(false); // bFaith/bCulture

					bool bResult;
					LuaSupport::CallHook(pkScriptSystem, "CityTrained", args.get(), bResult);
				}

				iProductionNeeded = getProductionNeeded(eTrainUnit) * 100;

				// max overflow is the value of the item produced (to eliminate prebuild exploits)
				int iOverflow = getUnitProductionTimes100(eTrainUnit) - iProductionNeeded;
				int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifferenceTimes100(false, false));
				int iLostProduction = std::max(0, iOverflow - iMaxOverflow);
				iOverflow = std::min(iMaxOverflow, iOverflow);
				if(iOverflow > 0)
				{
					changeOverflowProductionTimes100(iOverflow);
				}
				setUnitProduction(eTrainUnit, 0);

				int iProductionGold = ((iLostProduction * GC.getMAXED_UNIT_GOLD_PERCENT()) / 100);
				if(iProductionGold > 0)
				{
					kOwner.GetTreasury()->ChangeGoldTimes100(iProductionGold);
				}
			}
			else
			{
				// create notification
				setUnitProduction(eTrainUnit, getProductionNeeded(eTrainUnit) - 1);

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_REMOVED_UNIT");
					strText << getNameKey();
					strText << GC.getUnitInfo(eTrainUnit)->GetDescription();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_REMOVED_UNIT");
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), -1);
				}
			}
		}
		break;

	case ORDER_CONSTRUCT:
	{
		eConstructBuilding = ((BuildingTypes)(pOrderNode->iData1));

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eConstructBuilding);

		if(pkBuildingInfo)
		{
			kOwner.changeBuildingClassMaking(((BuildingClassTypes)(pkBuildingInfo->GetBuildingClassType())), -1);

			if(bFinish)
			{
				bool bResult = CreateBuilding(eConstructBuilding);
				DEBUG_VARIABLE(bResult);
				CvAssertMsg(bResult, "CreateBuilding failed");

				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(eConstructBuilding);
					args->Push(false); // bGold
					args->Push(false); // bFaith/bCulture

					bool bScriptResult;
					LuaSupport::CallHook(pkScriptSystem, "CityConstructed", args.get(), bScriptResult);
				}

				iProductionNeeded = getProductionNeeded(eConstructBuilding) * 100;
				// max overflow is the value of the item produced (to eliminate prebuild exploits)
				int iOverflow = m_pCityBuildings->GetBuildingProductionTimes100(eConstructBuilding) - iProductionNeeded;
				int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifferenceTimes100(false, false));
				int iLostProduction = std::max(0, iOverflow - iMaxOverflow);
				iOverflow = std::min(iMaxOverflow, iOverflow);
				if(iOverflow > 0)
				{
					changeOverflowProductionTimes100(iOverflow);
				}
				m_pCityBuildings->SetBuildingProduction(eConstructBuilding, 0);

				int iProductionGold = ((iLostProduction * GC.getMAXED_BUILDING_GOLD_PERCENT()) / 100);
				if(iProductionGold > 0)
				{
					kOwner.GetTreasury()->ChangeGoldTimes100(iProductionGold);
				}

				if(GC.getLogging() && GC.getAILogging())
				{
					CvBuildingEntry* pkConstructBuildingInfo = GC.getBuildingInfo(eConstructBuilding);
					if(pkConstructBuildingInfo)
					{
						if(kOwner.GetWonderProductionAI()->IsWonder(*pkConstructBuildingInfo))
						{
							CvString playerName;
							FILogFile* pLog;
							CvString strBaseString;
							CvString strOutBuf;
							playerName = kOwner.getCivilizationShortDescription();
							pLog = LOGFILEMGR.GetLog(kOwner.GetCitySpecializationAI()->GetLogFileName(playerName), FILogFile::kDontTimeStamp);
							strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
							strBaseString += playerName + ", ";
							strOutBuf.Format("%s, WONDER - Finished %s", getName().GetCString(), pkConstructBuildingInfo->GetDescription());
							strBaseString += strOutBuf;
							pLog->Msg(strBaseString);
						}
					}

				}
			}
		}
		break;
	}

	case ORDER_CREATE:
		eCreateProject = ((ProjectTypes)(pOrderNode->iData1));

		GET_TEAM(getTeam()).changeProjectMaking(eCreateProject, -1);
		kOwner.changeProjectMaking(eCreateProject, -1);

		if(bFinish)
		{
			bool bResult = CreateProject(eCreateProject);
			DEBUG_VARIABLE(bResult);
			CvAssertMsg(bResult, "Failed to create project");

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(eCreateProject);
				args->Push(false); // bGold
				args->Push(false); // bFaith/bCulture

				bool bScriptResult;
				LuaSupport::CallHook(pkScriptSystem, "CityCreated", args.get(), bScriptResult);
			}

			iProductionNeeded = getProductionNeeded(eCreateProject) * 100;
			// max overflow is the value of the item produced (to eliminate prebuild exploits)
			int iOverflow = getProjectProductionTimes100(eCreateProject) - iProductionNeeded;
			int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifferenceTimes100(false, false));
			int iLostProduction = std::max(0, iOverflow - iMaxOverflow);
			iOverflow = std::min(iMaxOverflow, iOverflow);
			if(iOverflow > 0)
			{
				changeOverflowProductionTimes100(iOverflow);
			}
			setProjectProduction(eCreateProject, 0);

			int iProductionGold = ((iLostProduction * GC.getMAXED_PROJECT_GOLD_PERCENT()) / 100);
			if(iProductionGold > 0)
			{
				kOwner.GetTreasury()->ChangeGoldTimes100(iProductionGold);
			}
		}
		break;

	case ORDER_PREPARE:

		if(bFinish)
		{
			eSpecialist = ((SpecialistTypes)(pOrderNode->iData1));

			iProductionNeeded = getProductionNeeded(eSpecialist) * 100;

			// max overflow is the value of the item produced (to eliminate prebuild exploits)
			int iOverflow = getSpecialistProductionTimes100(eSpecialist) - iProductionNeeded;
			int iMaxOverflow = std::max(iProductionNeeded, getCurrentProductionDifferenceTimes100(false, false));
			iOverflow = std::min(iMaxOverflow, iOverflow);
			if(iOverflow > 0)
			{
				changeOverflowProductionTimes100(iOverflow);
			}

			setSpecialistProduction(eSpecialist, 0);
		}

		break;

	case ORDER_MAINTAIN:
		break;

	default:
		CvAssertMsg(false, "pOrderNode->eOrderType is not a valid option");
		break;
	}

	if(m_unitBeingBuiltForOperation.IsValid())
	{
		kOwner.CityUncommitToBuildUnitForOperationSlot(m_unitBeingBuiltForOperation);
		m_unitBeingBuiltForOperation.Invalidate();
	}

	if(pOrderNode == headOrderQueueNode())
	{
		bStart = true;
		stopHeadOrder();
	}
	else
	{
		bStart = false;
	}

	m_orderQueue.deleteNode(pOrderNode);
	pOrderNode = NULL;
	if(bFinish)
	{
		CleanUpQueue(); // cleans out items from the queue that may be invalidated by the recent construction
	}

	if(bStart)
	{
		startHeadOrder();
	}

	if((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		if(isCitySelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}

	bMessage = false;

	if(bChoose)
	{
		if(getOrderQueueLength() == 0)
		{
			if(!isHuman() || isProductionAutomated())
			{
				AI_chooseProduction(false /*bInterruptWonders*/);
			}
			else
			{
				chooseProduction(eTrainUnit, eConstructBuilding, eCreateProject, bFinish);

				bMessage = true;
			}
		}
	}

	if(bFinish && !bMessage)
	{
		if(getOwner() == GC.getGame().getActivePlayer())
		{
			Localization::String localizedText;
			if(eTrainUnit != NO_UNIT)
			{
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eTrainUnit);
				if(pkUnitInfo)
				{
					localizedText = Localization::Lookup(((isLimitedUnitClass((UnitClassTypes)(pkUnitInfo->GetUnitClassType()))) ? "TXT_KEY_MISC_TRAINED_UNIT_IN_LIMITED" : "TXT_KEY_MISC_TRAINED_UNIT_IN"));
					localizedText << pkUnitInfo->GetTextKey() << getNameKey();
				}
			}
			else if(eConstructBuilding != NO_BUILDING)
			{
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eConstructBuilding);
				if(pkBuildingInfo)
				{
					localizedText = Localization::Lookup(((isLimitedWonderClass(pkBuildingInfo->GetBuildingClassInfo())) ? "TXT_KEY_MISC_CONSTRUCTED_BUILD_IN_LIMITED" : "TXT_KEY_MISC_CONSTRUCTED_BUILD_IN"));
					localizedText << pkBuildingInfo->GetTextKey() << getNameKey();
				}
			}
			else if(eCreateProject != NO_PROJECT)
			{
				localizedText = Localization::Lookup(((isLimitedProject(eCreateProject)) ? "TXT_KEY_MISC_CREATED_PROJECT_IN_LIMITED" : "TXT_KEY_MISC_CREATED_PROJECT_IN"));
				localizedText << GC.getProjectInfo(eCreateProject)->GetTextKey() << getNameKey();
			}
			if(isProduction())
			{
				localizedText = Localization::Lookup(((isProductionLimited()) ? "TXT_KEY_MISC_WORK_HAS_BEGUN_LIMITED" : "TXT_KEY_MISC_WORK_HAS_BEGUN"));
				localizedText << getProductionNameKey();
			}
			DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8()/*, szSound, MESSAGE_TYPE_MINOR_EVENT, szIcon, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX(), getY(), true, true*/);
		}
	}

	if((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		if(isCitySelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
		DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
void CvCity::swapOrder(int iNum)
{
	// okay, this only swaps the order with the next one up in the queue
	VALIDATE_OBJECT

	if(iNum == 0)
	{
		stopHeadOrder();
	}

	m_orderQueue.swapUp(iNum);

	if(iNum == 0)
	{
		startHeadOrder();
	}

	if((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		if(isCitySelected())
		{
			//DLLUI->setDirty(InfoPane_DIRTY_BIT, true );
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(CityScreen_DIRTY_BIT, true);
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
		DLLUI->setDirty(CityInfo_DIRTY_BIT, true);
	}
}


//	--------------------------------------------------------------------------------
void CvCity::startHeadOrder()
{
	VALIDATE_OBJECT
	OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_MAINTAIN)
		{
			processProcess(((ProcessTypes)(pOrderNode->iData1)), 1);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvCity::stopHeadOrder()
{
	VALIDATE_OBJECT
	OrderData* pOrderNode = headOrderQueueNode();

	if(pOrderNode != NULL)
	{
		if(pOrderNode->eOrderType == ORDER_MAINTAIN)
		{
			processProcess(((ProcessTypes)(pOrderNode->iData1)), -1);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvCity::getOrderQueueLength()
{
	VALIDATE_OBJECT
	return m_orderQueue.getLength();
}


//	--------------------------------------------------------------------------------
OrderData* CvCity::getOrderFromQueue(int iIndex)
{
	VALIDATE_OBJECT
	OrderData* pOrderNode;

	pOrderNode = m_orderQueue.nodeNum(iIndex);

	if(pOrderNode != NULL)
	{
		return pOrderNode;
	}
	else
	{
		return NULL;
	}
}


//	--------------------------------------------------------------------------------
OrderData* CvCity::nextOrderQueueNode(OrderData* pNode)
{
	VALIDATE_OBJECT
	return m_orderQueue.next(pNode);
}

//	--------------------------------------------------------------------------------
const OrderData* CvCity::nextOrderQueueNode(const OrderData* pNode) const
{
	VALIDATE_OBJECT
	return m_orderQueue.next(pNode);
}


//	--------------------------------------------------------------------------------
const OrderData* CvCity::headOrderQueueNode() const
{
	VALIDATE_OBJECT
	return m_orderQueue.head();
}

//	--------------------------------------------------------------------------------
OrderData* CvCity::headOrderQueueNode()
{
	VALIDATE_OBJECT
	return m_orderQueue.head();
}


//	--------------------------------------------------------------------------------
const OrderData* CvCity::tailOrderQueueNode() const
{
	VALIDATE_OBJECT
	return m_orderQueue.tail();
}

//	--------------------------------------------------------------------------------
/// remove items in the queue that are no longer valid
bool CvCity::CleanUpQueue(void)
{
	VALIDATE_OBJECT
	bool bOK = true;

	for(int iI = (getOrderQueueLength() - 1); iI >= 0; iI--)
	{
		OrderData* pOrder = getOrderFromQueue(iI);

		if(pOrder != NULL)
		{
			if(!canContinueProduction(*pOrder))
			{
				popOrder(iI, false, true);
				bOK = false;
			}
		}
	}

	return bOK;
}

//	--------------------------------------------------------------------------------
int CvCity::CreateUnit(UnitTypes eUnitType, UnitAITypes eAIType, bool bUseToSatisfyOperation)
{
	VALIDATE_OBJECT
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	CvUnit* pUnit = thisPlayer.initUnit(eUnitType, getX(), getY(), eAIType);
	CvAssertMsg(pUnit, "");
	if(!pUnit)
	{
		CvAssertMsg(false, "CreateUnit failed");
		return FFreeList::INVALID_INDEX;
	}

	if(pUnit->IsHasNoValidMove())
	{
		pUnit->kill(false);
		return FFreeList::INVALID_INDEX;
	}

	addProductionExperience(pUnit);

	CvPlot* pRallyPlot = getRallyPlot();
	if(pRallyPlot != NULL)
	{
		pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pRallyPlot->getX(), pRallyPlot->getY());
	}

	if(bUseToSatisfyOperation && m_unitBeingBuiltForOperation.IsValid())
	{
		thisPlayer.CityFinishedBuildingUnitForOperationSlot(m_unitBeingBuiltForOperation, pUnit);
		m_unitBeingBuiltForOperation.Invalidate();
	}

	// Any AI unit with explore AI as a secondary unit AI (e.g. warriors) are assigned that unit AI if this AI player needs to explore more
	else if(!pUnit->isHuman() && !thisPlayer.isMinorCiv())
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON");
		if(thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategy))
		{
			if(pUnit->getUnitInfo().GetUnitAIType(UNITAI_EXPLORE) && pUnit->AI_getUnitAIType() != UNITAI_EXPLORE)
			{

				// Now make sure there isn't a critical military threat
				CvMilitaryAI* thisPlayerMilAI = thisPlayer.GetMilitaryAI();
				int iThreat = thisPlayerMilAI->GetThreatTotal();
				iThreat += thisPlayerMilAI->GetBarbarianThreatTotal();
				if(iThreat < thisPlayerMilAI->GetThreatWeight(THREAT_CRITICAL))
				{
					pUnit->AI_setUnitAIType(UNITAI_EXPLORE);
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Assigning explore unit AI to %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						thisPlayer.GetHomelandAI()->LogHomelandMessage(strLogString);
					}
				}
				else
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Not assigning explore AI to %s due to threats, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						thisPlayer.GetHomelandAI()->LogHomelandMessage(strLogString);
					}
				}
			}
		}
		eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		EconomicAIStrategyTypes eOtherStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA");
		if(thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategy) || thisPlayer.GetEconomicAI()->IsUsingStrategy(eOtherStrategy))
		{
			if(pUnit->getUnitInfo().GetUnitAIType(UNITAI_EXPLORE_SEA))
			{
				pUnit->AI_setUnitAIType(UNITAI_EXPLORE_SEA);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Assigning explore sea unit AI to %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
					thisPlayer.GetHomelandAI()->LogHomelandMessage(strLogString);
				}
			}
		}
	}
	//Increment for stat tracking and achievements
	if(pUnit->isHuman())
	{
		IncrementUnitStatCount(pUnit);
	}

	return pUnit->GetID();
}

//	--------------------------------------------------------------------------------
bool CvCity::CreateBuilding(BuildingTypes eBuildingType)
{
	VALIDATE_OBJECT

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuildingType);
	if(pkBuildingInfo == NULL)
		return false;

	const BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetBuildingClassType();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	if(kPlayer.isBuildingClassMaxedOut(eBuildingClass, 0))
	{
		kPlayer.removeBuildingClass(eBuildingClass);
	}

	m_pCityBuildings->SetNumRealBuilding(eBuildingType, m_pCityBuildings->GetNumRealBuilding(eBuildingType) + 1);

	//Achievements
	if(kPlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		CheckForAchievementBuilding(eBuildingType);
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvCity::CreateProject(ProjectTypes eProjectType)
{
	VALIDATE_OBJECT

	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	CvTeam& thisTeam = GET_TEAM(getTeam());
	thisTeam.changeProjectCount(eProjectType, 1);

	ProjectTypes ApolloProgram = (ProjectTypes) GC.getSPACE_RACE_TRIGGER_PROJECT();
	ProjectTypes capsuleID = (ProjectTypes) GC.getSPACESHIP_CAPSULE();
	ProjectTypes boosterID = (ProjectTypes) GC.getSPACESHIP_BOOSTER();
	ProjectTypes stasisID = (ProjectTypes) GC.getSPACESHIP_STASIS();
	ProjectTypes engineID = (ProjectTypes) GC.getSPACESHIP_ENGINE();

	enum eSpaceshipState
	{
	    eUnderConstruction	= 0x0000,
	    eFrame				= 0x0001,
	    eCapsule			= 0x0002,
	    eStasis_Chamber		= 0x0004,
	    eEngine				= 0x0008,
	    eBooster1			= 0x0010,
	    eBooster2			= 0x0020,
	    eBooster3			= 0x0040,
	    eConstructed		= 0x0080,
	};

	if(eProjectType == ApolloProgram)
	{
		CvCity* theCapital = thisPlayer.getCapitalCity();
		if(theCapital)
		{
			auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(theCapital->plot()));
			gDLL->GameplaySpaceshipRemoved(pDllPlot.get());
			gDLL->GameplaySpaceshipCreated(pDllPlot.get(), eUnderConstruction + eFrame);
		}
	}
	else if(GC.getProjectInfo(eProjectType)->IsSpaceship())
	{
		VictoryTypes eVictory = (VictoryTypes)GC.getProjectInfo(eProjectType)->GetVictoryPrereq();

		if(NO_VICTORY != eVictory && GET_TEAM(getTeam()).canLaunch(eVictory))
		{
			auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(plot()));
			gDLL->GameplaySpaceshipEdited(pDllPlot.get(), eConstructed);
			gDLL->sendLaunch(getOwner(), eVictory);
		}
		else
		{
			//show the spaceship progress

			// this section is kind of hard-coded but it is completely hard-coded on the engine-side so I have to give it the numbers it expects
			int spaceshipState = eFrame;

			if((thisTeam.getProjectCount((ProjectTypes)capsuleID)) == 1)
			{
				spaceshipState += eCapsule;
			}

			if((thisTeam.getProjectCount((ProjectTypes)stasisID)) == 1)
			{
				spaceshipState += eStasis_Chamber;
			}

			if((thisTeam.getProjectCount((ProjectTypes)engineID)) == 1)
			{
				spaceshipState += eEngine;
			}

			if((thisTeam.getProjectCount((ProjectTypes)boosterID)) >= 1)
			{
				spaceshipState += eBooster1;
			}

			if((thisTeam.getProjectCount((ProjectTypes)boosterID)) >= 2)
			{
				spaceshipState += eBooster2;
			}

			if((thisTeam.getProjectCount((ProjectTypes)boosterID)) == 3)
			{
				spaceshipState += eBooster3;
			}

			auto_ptr<ICvPlot1> pDllPlot(new CvDllPlot(plot()));
			gDLL->GameplaySpaceshipEdited(pDllPlot.get(), spaceshipState);
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvCity::CanPlaceUnitHere(UnitTypes eUnitType)
{
	VALIDATE_OBJECT
	bool bCombat = false;

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
	if(pkUnitInfo == NULL)
		return false;

	if(pkUnitInfo->GetCombat() > 0 || pkUnitInfo->GetRange() > 0)
	{
		bCombat = true;
	}

	CvPlot* pPlot = plot();

	const IDInfo* pUnitNode;
	const CvUnit* pLoopUnit;

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit != NULL)
		{
			// Units of the same type OR Units belonging to different civs
			if(CvGameQueries::AreUnitsSameType(eUnitType, pLoopUnit->getUnitType()))
			{
				return false;
			}
		}
	}
	return true;
}

//	--------------------------------------------------------------------------------
// Is this city allowed to purchase something right now?
bool CvCity::IsCanPurchase(bool bTestPurchaseCost, bool bTestTrainable, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield)
{
	CvAssertMsg(eUnitType >= 0 || eBuildingType >= 0 || eProjectType >= 0, "No valid passed in");
	CvAssertMsg(!(eUnitType >= 0 && eBuildingType >= 0) && !(eUnitType >= 0 && eProjectType >= 0) && !(eBuildingType >= 0 && eProjectType >= 0), "Only one being passed");

	// Can't purchase anything in a puppeted city
	if (IsPuppet())
	{
		return false;
	}

	// Check situational reasons we can't purchase now (similar to not having enough gold or faith)
	if(bTestPurchaseCost)
	{
		// Can't purchase things if the city is in resistance or is being razed
		if(IsResistance() || IsRazing())
			return false;

		// if we're purchasing a unit
		if(eUnitType >= 0)
		{
			// if we can't add this unit to this tile, then don't!
			if(!CanPlaceUnitHere(eUnitType))
				return false;
		}
	}

	// What are we buying this with?
	switch(ePurchaseYield)
	{
	case YIELD_GOLD:
	{
		int iGoldCost = -1;

		// Unit
		if(eUnitType != NO_UNIT)
		{
			if(!canTrain(eUnitType, false, !bTestTrainable))
				return false;

			iGoldCost = GetPurchaseCost(eUnitType);
		}
		// Building
		else if(eBuildingType != NO_BUILDING)
		{
			if(!canConstruct(eBuildingType, false, !bTestTrainable))
			{
				bool bAlreadyUnderConstruction = canConstruct(eBuildingType, true, !bTestTrainable) && getFirstBuildingOrder(eBuildingType) != -1;
				if(!bAlreadyUnderConstruction)
				{
					return false;
				}
			}

			iGoldCost = GetPurchaseCost(eBuildingType);
		}
		// Project
		else if(eProjectType != NO_PROJECT)
		{
			if(/*1*/ GC.getPROJECT_PURCHASING_DISABLED() == 1)
				return false;

			if(!canCreate(eProjectType, false, !bTestTrainable))
				return false;

			iGoldCost = GetPurchaseCost(eProjectType);
		}

		if(iGoldCost == -1)
		{
			return false;
		}
		else
		{
			if(bTestPurchaseCost)
			{
				// Trying to buy something when you don't have enough money!!
				if(iGoldCost > GET_PLAYER(getOwner()).GetTreasury()->GetGold())
					return false;
			}
		}
	}
	break;

	case YIELD_FAITH:
	{
		int iFaithCost = -1;

		// Does this city have a majority religion?
		ReligionTypes eReligion = GetCityReligions()->GetReligiousMajority();
		if(eReligion <= RELIGION_PANTHEON)
		{
			return false;
		}

		// Unit
		if(eUnitType != NO_UNIT)
		{
			iFaithCost = GetFaithPurchaseCost(eUnitType, true);
			if(iFaithCost < 1)
			{
				return false;
			}

			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
			if(pkUnitInfo)
			{
				if (pkUnitInfo->IsRequiresEnhancedReligion() && !(GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER)->m_bEnhanced))
				{
					return false;
				}

				
				if (pkUnitInfo->IsRequiresFaithPurchaseEnabled())
				{
					const CvReligion *pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_eOwner);
					if (!pReligion->m_Beliefs.IsFaithBuyingEnabled())
					{
						return false;
					}

					if(!canTrain(eUnitType, false, !bTestTrainable))
					{
						return false;
					}
				}
			}
		}
		// Building
		else if(eBuildingType != NO_BUILDING)
		{
			CvBuildingEntry* pkBuildingInfo = GC.GetGameBuildings()->GetEntry(eBuildingType);

			// Religion-enabled building
			if(pkBuildingInfo && pkBuildingInfo->IsUnlockedByBelief())
			{
				ReligionTypes eMajority = GetCityReligions()->GetReligiousMajority();
				if(eMajority <= RELIGION_PANTHEON)
				{
					return false;
				}
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
				if(pReligion == NULL || !pReligion->m_Beliefs.IsBuildingClassEnabled((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType()))
				{
					return false;
				}

				if(GetCityBuildings()->GetNumBuilding(eBuildingType) > 0)
				{
					return false;
				}
			}

			iFaithCost = GetFaithPurchaseCost(eBuildingType);
			if(iFaithCost < 1) return false;
		}

		if(iFaithCost > 0)
		{
			if(bTestPurchaseCost)
			{
				// Trying to buy something when you don't have enough faith!!
				if(iFaithCost > GET_PLAYER(getOwner()).GetFaith())
				{
					return false;
				}
			}
		}
	}
	break;
	}

	return true;
}

//	--------------------------------------------------------------------------------
// purchase something at the city
void CvCity::Purchase(UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, YieldTypes ePurchaseYield)
{
	VALIDATE_OBJECT

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	switch(ePurchaseYield)
	{
	case YIELD_GOLD:
	{
		// Can we actually buy this thing?
		if(!IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eUnitType, eBuildingType, eProjectType, YIELD_GOLD))
			return;

		int iGoldCost = 0;

		// Unit
		if(eUnitType != NO_UNIT)
			iGoldCost = GetPurchaseCost(eUnitType);
		// Building
		else if(eBuildingType != NO_BUILDING)
			iGoldCost = GetPurchaseCost(eBuildingType);
		// Project
		else if(eProjectType != NO_PROJECT)
			iGoldCost = GetPurchaseCost(eProjectType);

		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(-iGoldCost);

		bool bResult = false;
		if(eUnitType >= 0)
		{
			int iResult = CreateUnit(eUnitType);
			CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create unit");
			if (iResult != FFreeList::INVALID_INDEX)
			{
				CvUnit* pUnit = kPlayer.getUnit(iResult);
				pUnit->setMoves(0);

				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(pUnit->GetID());
					args->Push(true); // bGold
					args->Push(false); // bFaith/bCulture

					bool bScriptResult;
					LuaSupport::CallHook(pkScriptSystem, "CityTrained", args.get(), bScriptResult);
				}
			}
		}
		else if(eBuildingType >= 0)
		{
			bResult = CreateBuilding(eBuildingType);

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(eBuildingType);
				args->Push(true); // bGold
				args->Push(false); // bFaith/bCulture

				bool bScriptResult;
				LuaSupport::CallHook(pkScriptSystem, "CityConstructed", args.get(), bScriptResult);
			}


			CleanUpQueue(); // cleans out items from the queue that may be invalidated by the recent construction
			CvAssertMsg(bResult, "Unable to create building");
		}
		else if(eProjectType >= 0)
		{
			bResult = CreateProject(eProjectType);
			CvAssertMsg(bResult, "Unable to create project");

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(eProjectType);
				args->Push(true); // bGold
				args->Push(false); // bFaith/bCulture

				bool bScriptResult;
				LuaSupport::CallHook(pkScriptSystem, "CityCreated", args.get(), bScriptResult);
			}
		}
	}
	break;
	case YIELD_FAITH:
	{
		int iFaithCost = 0;

		// Can we actually buy this thing?
		if(!IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eUnitType, eBuildingType, eProjectType, YIELD_FAITH))
			return;

		// Unit
		if(eUnitType != NO_UNIT)
			iFaithCost = GetFaithPurchaseCost(eUnitType, true  /*bIncludeBeliefDiscounts*/);
		// Building
		else if(eBuildingType != NO_BUILDING)
			iFaithCost = GetFaithPurchaseCost(eBuildingType);

		if(eUnitType >= 0)
		{
			int iResult = CreateUnit(eUnitType);
			CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create unit");
			if (iResult == FFreeList::INVALID_INDEX)
				return;	// Can't create the unit, most likely we have no place for it.  We have not deducted the cost yet so just exit.

			CvUnit* pUnit = kPlayer.getUnit(iResult);
			pUnit->setMoves(0);

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(pUnit->GetID());
				args->Push(false); // bGold
				args->Push(true); // bFaith/bCulture

				bool bResult;
				LuaSupport::CallHook(pkScriptSystem, "CityTrained", args.get(), bResult);
			}

			// Prophets are always of the religion the player founded
			ReligionTypes eReligion;
			if(pUnit->getUnitInfo().IsFoundReligion())
			{
				eReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
			}
			else
			{
				eReligion = GetCityReligions()->GetReligiousMajority();
			}
			pUnit->GetReligionData()->SetReligion(eReligion);

			int iReligionSpreads = pUnit->getUnitInfo().GetReligionSpreads();
			int iReligiousStrength = pUnit->getUnitInfo().GetReligiousStrength();

			// Missionary strength
			if(iReligionSpreads > 0 && eReligion > RELIGION_PANTHEON)
			{
				pUnit->GetReligionData()->SetSpreadsLeft(iReligionSpreads + GetCityBuildings()->GetMissionaryExtraSpreads());
				pUnit->GetReligionData()->SetReligiousStrength(iReligiousStrength);
			}

			kPlayer.ChangeFaith(-iFaithCost);

			UnitClassTypes eUnitClass = pUnit->getUnitClassType();
			if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
			{
				kPlayer.incrementArtistsFromFaith();
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
			{
				kPlayer.incrementScientistsFromFaith();
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
			{
				kPlayer.incrementMerchantsFromFaith();
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
			{
				kPlayer.incrementEngineersFromFaith();
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_GENERAL"))
			{
				kPlayer.incrementGeneralsFromFaith();
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_ADMIRAL"))
			{
				kPlayer.incrementAdmiralsFromFaith();
				CvPlot *pSpawnPlot = kPlayer.GetGreatAdmiralSpawnPlot(pUnit);
				if (pUnit->plot() != pSpawnPlot)
				{
					pUnit->setXY(pSpawnPlot->getX(), pSpawnPlot->getY());
				}
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_PROPHET"))
			{
				kPlayer.GetReligions()->ChangeNumProphetsSpawned(1);
			}

			if(GC.getLogging())
			{
				CvString strLogMsg;
				CvString temp;
				strLogMsg = kPlayer.getCivilizationShortDescription();
				strLogMsg += ", FAITH UNIT PURCHASE, ";
				strLogMsg += pUnit->getName();
				strLogMsg += ", ";
				strLogMsg += getName();
				strLogMsg += ", Faith Cost: ";
				temp.Format("%d", iFaithCost);
				strLogMsg += temp;
				strLogMsg += ", Faith Left: ";
				temp.Format("%d", kPlayer.GetFaith());
				strLogMsg += temp;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}
		}

		else if(eBuildingType >= 0)
		{
			bool bResult = false;
			bResult = CreateBuilding(eBuildingType);
			CleanUpQueue(); // cleans out items from the queue that may be invalidated by the recent construction
			CvAssertMsg(bResult, "Unable to create building");

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(eBuildingType);
				args->Push(false); // bGold
				args->Push(true); // bFaith/bCulture

				bool bScriptResult;
				LuaSupport::CallHook(pkScriptSystem, "CityConstructed", args.get(), bScriptResult);
			}

			kPlayer.ChangeFaith(-iFaithCost);

			if(GC.getLogging())
			{
				CvString strLogMsg;
				CvString temp;
				strLogMsg = kPlayer.getCivilizationShortDescription();
				strLogMsg += ", FAITH BUILDING PURCHASE, ";

				CvBuildingXMLEntries* pGameBuildings = GC.GetGameBuildings();
				if(pGameBuildings != NULL)
				{
					CvBuildingEntry* pBuildingEntry = pGameBuildings->GetEntry(eBuildingType);
					if(pBuildingEntry != NULL)
					{
						strLogMsg += pBuildingEntry->GetDescription();
						strLogMsg += ", ";
					}
				}
				strLogMsg += getName();
				strLogMsg += ", Faith Cost: ";
				temp.Format("%d", iFaithCost);
				strLogMsg += temp;
				strLogMsg += ", Faith Left: ";
				temp.Format("%d", kPlayer.GetFaith());
				strLogMsg += temp;
				GC.getGame().GetGameReligions()->LogReligionMessage(strLogMsg);
			}
		}
	}
	break;
	}
}


// Protected Functions...

//	--------------------------------------------------------------------------------
void CvCity::doGrowth()
{
	VALIDATE_OBJECT
	// here would be a good place to override this in Lua

	// No growth or starvation if being razed
	if(IsRazing())
	{
		return;
	}

	int iDiff = foodDifferenceTimes100();

	if(iDiff < 0)
	{
		CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
		if(pNotifications)
		{
			Localization::String text = Localization::Lookup("TXT_KEY_NTFN_CITY_STARVING");
			text << getNameKey();
			Localization::String summary = Localization::Lookup("TXT_KEY_NTFN_CITY_STARVING_S");
			summary << getNameKey();

			pNotifications->Add(NOTIFICATION_STARVING, text.toUTF8(), summary.toUTF8(), getX(), getY(), -1);
		}
	}

	changeFoodTimes100(iDiff);
	changeFoodKept(iDiff/100);

	setFoodKept(range(getFoodKept(), 0, ((growthThreshold() * getMaxFoodKeptPercent()) / 100)));

	if(getFood() >= growthThreshold())
	{
		if(GetCityCitizens()->IsForcedAvoidGrowth())  // don't grow a city if we are at avoid growth
		{
			setFood(growthThreshold());
		}
		else
		{
			changeFood(-(std::max(0, (growthThreshold() - getFoodKept()))));
			changePopulation(1);

			// Only show notification if the city is small
			if(getPopulation() <= 5)
			{
				CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
				if(pNotifications)
				{
					Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_GROWTH");
					localizedText << getNameKey() << getPopulation();
					Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_GROWTH");
					localizedSummary << getNameKey();
					pNotifications->Add(NOTIFICATION_CITY_GROWTH, localizedText.toUTF8(), localizedSummary.toUTF8(), getX(), getY(), GetID());
				}
			}
		}
	}
	else if(getFood() < 0)
	{
		changeFood(-(getFood()));

		if(getPopulation() > 1)
		{
			changePopulation(-1);
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvCity::doCheckProduction()
{
	VALIDATE_OBJECT
	OrderData* pOrderNode;
	UnitTypes eUpgradeUnit;
	int iUpgradeProduction;
	int iProductionGold;
	int iI;
	bool bOK = true;

	int iMaxedUnitGoldPercent = GC.getMAXED_UNIT_GOLD_PERCENT();
	int iMaxedBuildingGoldPercent = GC.getMAXED_BUILDING_GOLD_PERCENT();
	int iMaxedProjectGoldPercent = GC.getMAXED_PROJECT_GOLD_PERCENT();

	CvPlayerAI& thisPlayer = GET_PLAYER(getOwner());

	int iNumUnitInfos = GC.getNumUnitInfos();
	for(iI = 0; iI < iNumUnitInfos; iI++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iI);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			int iUnitProduction = getUnitProduction(eUnit);
			if(iUnitProduction > 0)
			{
				if(thisPlayer.isProductionMaxedUnitClass((UnitClassTypes)(pkUnitInfo)->GetUnitClassType()))
				{
					iProductionGold = ((iUnitProduction * iMaxedUnitGoldPercent) / 100);

					if(iProductionGold > 0)
					{
						thisPlayer.GetTreasury()->ChangeGold(iProductionGold);

						if(getOwner() == GC.getGame().getActivePlayer())
						{
							Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED");
							localizedText << getNameKey() << GC.getUnitInfo((UnitTypes)iI)->GetTextKey() << iProductionGold;
							DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());
						}
					}

					setUnitProduction(((UnitTypes)iI), 0);
				}
			}
		}
	}

	int iNumBuildingInfos = GC.getNumBuildingInfos();

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;

	for(iI = 0; iI < iNumBuildingInfos; iI++)
	{
		const BuildingTypes eExpiredBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkExpiredBuildingInfo = GC.getBuildingInfo(eExpiredBuilding);

		//skip if null
		if(pkExpiredBuildingInfo == NULL)
			continue;

		int iBuildingProduction = m_pCityBuildings->GetBuildingProduction(eExpiredBuilding);
		if(iBuildingProduction > 0)
		{
			const BuildingClassTypes eExpiredBuildingClass = (BuildingClassTypes)(pkExpiredBuildingInfo->GetBuildingClassType());

			if(thisPlayer.isProductionMaxedBuildingClass(eExpiredBuildingClass))
			{
				// Beaten to a world wonder by someone?
				if(isWorldWonderClass(pkExpiredBuildingInfo->GetBuildingClassInfo()))
				{
					for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						// Found the culprit
						if(GET_PLAYER(eLoopPlayer).getBuildingClassCount(eExpiredBuildingClass) > 0)
						{
							GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeNumWondersBeatenTo(eLoopPlayer, 1);
							break;
						}
					}

					auto_ptr<ICvCity1> pDllCity(new CvDllCity(this));
					DLLUI->AddDeferredWonderCommand(WONDER_REMOVED, pDllCity.get(), (BuildingTypes) eExpiredBuilding, 0);
					//Add "achievement" for sucking it up
					gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BEATWONDERS, 10, ACHIEVEMENT_SUCK_AT_WONDERS);
				}

				iProductionGold = ((iBuildingProduction * iMaxedBuildingGoldPercent) / 100);

				if(iProductionGold > 0)
				{
					thisPlayer.GetTreasury()->ChangeGold(iProductionGold);

					if(getOwner() == GC.getGame().getActivePlayer())
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED");
						localizedText << getNameKey() << pkExpiredBuildingInfo->GetTextKey() << iProductionGold;
						DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());
					}
				}

				m_pCityBuildings->SetBuildingProduction(eExpiredBuilding, 0);
			}
		}
	}

	int iNumProjectInfos = GC.getNumProjectInfos();
	for(iI = 0; iI < iNumProjectInfos; iI++)
	{
		int iProjectProduction = getProjectProduction((ProjectTypes)iI);
		if(iProjectProduction > 0)
		{
			if(thisPlayer.isProductionMaxedProject((ProjectTypes)iI))
			{
				iProductionGold = ((iProjectProduction * iMaxedProjectGoldPercent) / 100);

				if(iProductionGold > 0)
				{
					thisPlayer.GetTreasury()->ChangeGold(iProductionGold);

					if(getOwner() == GC.getGame().getActivePlayer())
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED");
						localizedText << getNameKey() << GC.getProjectInfo((ProjectTypes)iI)->GetTextKey() << iProductionGold;
						DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());
					}
				}

				setProjectProduction(((ProjectTypes)iI), 0);
			}
		}
	}

	if(!isProduction() && isHuman() && !isProductionAutomated())
	{
		chooseProduction();
		return bOK;
	}

	// Can now construct an Upgraded version of this Unit
	for(iI = 0; iI < iNumUnitInfos; iI++)
	{
		if(getFirstUnitOrder((UnitTypes)iI) != -1)
		{
			// If we can still actually train this Unit type then don't auto-upgrade it yet
			if(canTrain((UnitTypes)iI, true))
			{
				continue;
			}

			eUpgradeUnit = allUpgradesAvailable((UnitTypes)iI);

			if(eUpgradeUnit != NO_UNIT)
			{
				CvAssertMsg(eUpgradeUnit != iI, "Trying to upgrade a Unit to itself");
				iUpgradeProduction = getUnitProduction((UnitTypes)iI);
				setUnitProduction(((UnitTypes)iI), 0);
				setUnitProduction(eUpgradeUnit, iUpgradeProduction);

				pOrderNode = headOrderQueueNode();

				while(pOrderNode != NULL)
				{
					if(pOrderNode->eOrderType == ORDER_TRAIN)
					{
						if(pOrderNode->iData1 == iI)
						{
							thisPlayer.changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)(pOrderNode->iData1))->GetUnitClassType())), -1);
							pOrderNode->iData1 = eUpgradeUnit;
							thisPlayer.changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)(pOrderNode->iData1))->GetUnitClassType())), 1);
						}
					}

					pOrderNode = nextOrderQueueNode(pOrderNode);
				}
			}
		}
	}

	// Can now construct an Upgraded version of this Building
	for(iI = 0; iI < iNumBuildingInfos; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo)
		{
			if(getFirstBuildingOrder(eBuilding) != -1)
			{
				BuildingClassTypes eBuildingClass = (BuildingClassTypes) pkBuildingInfo->GetReplacementBuildingClass();

				if(eBuildingClass != NO_BUILDINGCLASS)
				{
					BuildingTypes eUpgradeBuilding = ((BuildingTypes)(thisPlayer.getCivilizationInfo().getCivilizationBuildings(eBuildingClass)));

					if(canConstruct(eUpgradeBuilding))
					{
						CvAssertMsg(eUpgradeBuilding != iI, "Trying to upgrade a Building to itself");
						iUpgradeProduction = m_pCityBuildings->GetBuildingProduction(eBuilding);
						m_pCityBuildings->SetBuildingProduction((eBuilding), 0);
						m_pCityBuildings->SetBuildingProduction(eUpgradeBuilding, iUpgradeProduction);

						pOrderNode = headOrderQueueNode();

						while(pOrderNode != NULL)
						{
							if(pOrderNode->eOrderType == ORDER_CONSTRUCT)
							{
								if(pOrderNode->iData1 == iI)
								{
									CvBuildingEntry* pkOrderBuildingInfo = GC.getBuildingInfo((BuildingTypes)pOrderNode->iData1);
									CvBuildingEntry* pkUpgradeBuildingInfo = GC.getBuildingInfo(eUpgradeBuilding);

									if(NULL != pkOrderBuildingInfo && NULL != pkUpgradeBuildingInfo)
									{
										const BuildingClassTypes eOrderBuildingClass = (BuildingClassTypes)pkOrderBuildingInfo->GetBuildingClassType();
										const BuildingClassTypes eUpgradeBuildingClass = (BuildingClassTypes)pkUpgradeBuildingInfo->GetBuildingClassType();

										thisPlayer.changeBuildingClassMaking(eOrderBuildingClass, -1);
										pOrderNode->iData1 = eUpgradeBuilding;
										thisPlayer.changeBuildingClassMaking(eUpgradeBuildingClass, 1);

									}
								}
							}

							pOrderNode = nextOrderQueueNode(pOrderNode);
						}
					}
				}
			}
		}
	}

	bOK = CleanUpQueue();

	return bOK;
}


//	--------------------------------------------------------------------------------
void CvCity::doProduction(bool bAllowNoProduction)
{
	VALIDATE_OBJECT

	if(!isHuman() || isProductionAutomated())
	{
		if(!isProduction() || isProductionProcess() || AI_isChooseProductionDirty())
		{
			AI_chooseProduction(false /*bInterruptWonders*/);
		}
	}

	if(!bAllowNoProduction && !isProduction())
	{
		return;
	}

	if(isProductionProcess())
	{
		return;
	}

	if(isProduction())
	{

		if(isProductionBuilding())
		{
			const OrderData* pOrderNode = headOrderQueueNode();
			int iData1 = -1;
			if(pOrderNode != NULL)
			{
				iData1 = pOrderNode->iData1;
			}

			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iData1);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				if(isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
				{
					if(m_pCityBuildings->GetBuildingProduction(eBuilding) == 0)  // otherwise we are probably already showing this
					{
						auto_ptr<ICvCity1> pDllCity(new CvDllCity(this));
						DLLUI->AddDeferredWonderCommand(WONDER_CREATED, pDllCity.get(), eBuilding, 0);
					}
				}
			}
		}

		changeProductionTimes100(getCurrentProductionDifferenceTimes100(false, true));

		setOverflowProduction(0);
		setFeatureProduction(0);

		if(getProduction() >= getProductionNeeded())
		{
			popOrder(0, true, true);
		}
	}
	else
	{
		changeOverflowProductionTimes100(getCurrentProductionDifferenceTimes100(false, false));
	}
}


//	--------------------------------------------------------------------------------
void CvCity::doDecay()
{
	VALIDATE_OBJECT
	int iI;

	int iBuildingProductionDecayTime = GC.getBUILDING_PRODUCTION_DECAY_TIME();
	int iBuildingProductionDecayPercent = GC.getBUILDING_PRODUCTION_DECAY_PERCENT();

	int iNumBuildingInfos = GC.getNumBuildingInfos();
	for(iI = 0; iI < iNumBuildingInfos; iI++)
	{
		if(getProductionBuilding() != ((BuildingTypes)iI))
		{
			if(m_pCityBuildings->GetBuildingProduction((BuildingTypes)iI) > 0)
			{
				m_pCityBuildings->ChangeBuildingProductionTime(((BuildingTypes)iI), 1);

				if(isHuman())
				{
					if(m_pCityBuildings->GetBuildingProductionTime((BuildingTypes)iI) > iBuildingProductionDecayTime)
					{
						m_pCityBuildings->SetBuildingProduction(((BuildingTypes)iI), ((m_pCityBuildings->GetBuildingProduction((BuildingTypes)iI) * iBuildingProductionDecayPercent) / 100));
					}
				}
			}
			else
			{
				m_pCityBuildings->SetBuildingProductionTime(((BuildingTypes)iI), 0);
			}
		}
	}

	int iUnitProductionDecayTime = GC.getUNIT_PRODUCTION_DECAY_TIME();
	int iUnitProductionDecayPercent = GC.getUNIT_PRODUCTION_DECAY_PERCENT();

	int iNumUnitInfos = GC.getNumUnitInfos();
	for(iI = 0; iI < iNumUnitInfos; iI++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iI);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			if(getProductionUnit() != eUnit)
			{
				if(getUnitProduction(eUnit) > 0)
				{
					changeUnitProductionTime(eUnit, 1);

					if(isHuman())
					{
						if(getUnitProductionTime(eUnit) > iUnitProductionDecayTime)
						{
							setUnitProduction(eUnit, ((getUnitProduction(eUnit) * iUnitProductionDecayPercent) / 100));
						}
					}
				}
				else
				{
					setUnitProductionTime(eUnit, 0);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvCity::doMeltdown()
{
	VALIDATE_OBJECT

	int iNumBuildingInfos = GC.getNumBuildingInfos();
	for(int iI = 0; iI < iNumBuildingInfos; iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(NULL != pkBuildingInfo && m_pCityBuildings->GetNumBuilding((BuildingTypes)iI) > 0)
		{
			if(pkBuildingInfo->GetNukeExplosionRand() != 0)
			{
				if(GC.getGame().getJonRandNum(pkBuildingInfo->GetNukeExplosionRand(), "Meltdown!!!") == 0)
				{
					if(m_pCityBuildings->GetNumRealBuilding((BuildingTypes)iI) > 0)
					{
						m_pCityBuildings->SetNumRealBuilding(((BuildingTypes)iI), 0);
					}

					CvUnitCombat::ApplyNuclearExplosionDamage(plot(), 1);

					if(getOwner() == GC.getGame().getActivePlayer())
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_MELTDOWN_CITY");
						localizedText << getNameKey();

						DLLUI->AddCityMessage(0, GetIDInfo(), getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8()/*, "AS2D_MELTDOWN", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX(), getY(), true, true*/);
					}

					break;
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
CvCityStrategyAI* CvCity::GetCityStrategyAI() const
{
	VALIDATE_OBJECT
	return m_pCityStrategyAI;
}

//	--------------------------------------------------------------------------------
CvCityCitizens* CvCity::GetCityCitizens() const
{
	VALIDATE_OBJECT
	return m_pCityCitizens;
}

//	--------------------------------------------------------------------------------
CvCityReligions* CvCity::GetCityReligions() const
{
	VALIDATE_OBJECT
	return m_pCityReligions;
}

//	--------------------------------------------------------------------------------
CvCityEmphases* CvCity::GetCityEmphases() const
{
	VALIDATE_OBJECT
	return m_pEmphases;
}

//	--------------------------------------------------------------------------------
CvCityEspionage* CvCity::GetCityEspionage() const
{
	VALIDATE_OBJECT
	return m_pCityEspionage;
}

// Private Functions...

//	--------------------------------------------------------------------------------
void CvCity::read(FDataStream& kStream)
{
	VALIDATE_OBJECT
	// Init data before load
	reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iID;
	kStream >> m_iX;
	kStream >> m_iY;
	kStream >> m_iRallyX;
	kStream >> m_iRallyY;
	kStream >> m_iGameTurnFounded;
	kStream >> m_iGameTurnAcquired;
	kStream >> m_iPopulation;
	kStream >> m_iHighestPopulation;
	kStream >> m_iNumGreatPeople;
	kStream >> m_iBaseGreatPeopleRate;
	kStream >> m_iGreatPeopleRateModifier;
	kStream >> m_iGreatPeopleProgress;
	kStream >> m_iJONSCultureStored;
	kStream >> m_iJONSCultureLevel;
	kStream >> m_iJONSCulturePerTurnFromBuildings;
	kStream >> m_iJONSCulturePerTurnFromPolicies;
	kStream >> m_iJONSCulturePerTurnFromSpecialists;
	kStream >> m_iJONSCulturePerTurnFromReligion;
	if(uiVersion >= 16)
	{
		kStream >> m_iFaithPerTurnFromBuildings;
	}
	else
	{
		m_iFaithPerTurnFromBuildings = 0;
	}

	if(uiVersion >= 20)
	{
		kStream >> m_iFaithPerTurnFromPolicies;
	}
	else
	{
		if(uiVersion >= 16)
		{
			kStream >> m_iFaithPerTurnFromPolicies;  // temp read, used to be FaithPerTurnFromTerrain
		}
		m_iFaithPerTurnFromPolicies = 0;
	}

	if(uiVersion >= 21)
	{
		kStream >> m_iFaithPerTurnFromReligion;
	}
	else
	{
		m_iFaithPerTurnFromReligion = 0;
	}

	kStream >> m_iCultureRateModifier;
	kStream >> m_iNumWorldWonders;
	kStream >> m_iNumTeamWonders;
	kStream >> m_iNumNationalWonders;
	kStream >> m_iWonderProductionModifier;
	if(uiVersion >= 3)
	{
		kStream >> m_iCapturePlunderModifier;
		kStream >> m_iPlotCultureCostModifier;
	}
	else
	{
		m_iCapturePlunderModifier = 0;
		m_iPlotCultureCostModifier = 0;
	}
	if(uiVersion >= 12)
	{
		kStream >> m_iPlotBuyCostModifier;
	}
	else
	{
		m_iPlotBuyCostModifier = 0;
	}
	kStream >> m_iMaintenance;
	kStream >> m_iHealRate;
	if(uiVersion >= 23)
	{
		kStream >> m_iEspionageModifier;
	}
	else
	{
		m_iEspionageModifier = 0;
	}
	kStream >> m_iNoOccupiedUnhappinessCount;
	kStream >> m_iFood;
	kStream >> m_iFoodKept;
	kStream >> m_iMaxFoodKeptPercent;
	kStream >> m_iOverflowProduction;
	kStream >> m_iFeatureProduction;
	kStream >> m_iMilitaryProductionModifier;
	kStream >> m_iSpaceProductionModifier;
	kStream >> m_iFreeExperience;
	kStream >> m_iCurrAirlift;
	kStream >> m_iMaxAirlift;
	kStream >> m_iAirModifier;
	kStream >> m_iNukeModifier;
	kStream >> m_iCultureUpdateTimer;
	kStream >> m_iCitySizeBoost;
	kStream >> m_iSpecialistFreeExperience;
	kStream >> m_iStrengthValue;
	kStream >> m_iDamage;
	kStream >> m_iThreatValue;
	kStream >> m_iGarrisonedUnit;
	if(uiVersion >= 13)
	{
		m_iResourceDemanded = CvInfosSerializationHelper::ReadHashed(kStream);
	}
	else
	{
		kStream >> m_iResourceDemanded;
		m_iResourceDemanded = CvInfosSerializationHelper::ConvertV0((ResourceTypes)(m_iResourceDemanded.get()), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
	}
	kStream >> m_iWeLoveTheKingDayCounter;
	kStream >> m_iLastTurnGarrisonAssigned;
	kStream >> m_iThingsProduced;
	kStream >> m_iDemandResourceCounter;
	kStream >> m_iResistanceTurns;
	kStream >> m_iRazingTurns;
	if(uiVersion < 2)
	{
		int iLakeYieldModifier;
		kStream >> iLakeYieldModifier;   // m_iLakeYieldModifier Removed
	}
	kStream >> m_iCountExtraLuxuries;
	kStream >> m_iCheapestPlotInfluence;
	kStream >> m_unitBeingBuiltForOperation.m_iOperationID;
	kStream >> m_unitBeingBuiltForOperation.m_iArmyID;
	kStream >> m_unitBeingBuiltForOperation.m_iSlotID;

	kStream >> m_bNeverLost;
	kStream >> m_bDrafted;
	kStream >> m_bAirliftTargeted;
	kStream >> m_bProductionAutomated;
	kStream >> m_bMadeAttack;
	kStream >> m_bOccupied;
	kStream >> m_bPuppet;
	kStream >> m_bEverCapital;
	kStream >> m_bIndustrialRouteToCapital;
	kStream >> m_bFeatureSurrounded;

	kStream >> m_eOwner;
	kStream >> m_ePreviousOwner;
	kStream >> m_eOriginalOwner;
	kStream >> m_ePlayersReligion;

	kStream >> m_aiSeaPlotYield;
	kStream >> m_aiRiverPlotYield;
	if(uiVersion >= 2)
	{
		kStream >> m_aiLakePlotYield;
	}
	else
	{
		m_aiLakePlotYield.resize(NUM_YIELD_TYPES);
		for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_aiLakePlotYield.setAt(iI, 0);
		}
	}
	kStream >> m_aiSeaResourceYield;
	kStream >> m_aiBaseYieldRateFromTerrain;
	kStream >> m_aiBaseYieldRateFromBuildings;
	kStream >> m_aiBaseYieldRateFromSpecialists;
	kStream >> m_aiBaseYieldRateFromMisc;
	if(uiVersion >= 18)
	{
		kStream >> m_aiBaseYieldRateFromReligion;
	}
	kStream >> m_aiYieldPerPop;
	kStream >> m_aiYieldRateModifier;
	kStream >> m_aiPowerYieldRateModifier;
	kStream >> m_aiResourceYieldRateModifier;
	kStream >> m_aiExtraSpecialistYield;
	kStream >> m_aiProductionToYieldModifier;
	if(uiVersion >= 24)
	{
		kStream >> m_aiDomainFreeExperience;
		kStream >> m_aiDomainProductionModifier;
	}
	else
	{
		int iOldNumDomainTypes = 4; // Before version 24, NUM_DOMAIN_TYPES was 4
		kStream >> m_aiDomainFreeExperience;
		kStream >> m_aiDomainProductionModifier;
		m_aiDomainFreeExperience.resize(NUM_DOMAIN_TYPES);
		m_aiDomainProductionModifier.resize(NUM_DOMAIN_TYPES);

		// Set any new DomainTypes to 0
		for (int iI = iOldNumDomainTypes; iI < NUM_DOMAIN_TYPES; iI++)
		{
			m_aiDomainFreeExperience.setAt(iI, 0);
			m_aiDomainProductionModifier.setAt(iI, 0);
		}
	}

	kStream >> m_abEverOwned;
	kStream >> m_abRevealed;

	kStream >> m_strName;
	kStream >> m_strScriptData;

	if(uiVersion >= 14)
	{
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNoResource.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiFreeResource.dirtyGet());
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiNumResourcesLocal.dirtyGet());
	}
	else
	{
		std::vector<int> aTemp;
		CvInfosSerializationHelper::ReadV0DataArray<int>(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
		m_paiNoResource = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray<int>(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
		m_paiFreeResource = aTemp;
		CvInfosSerializationHelper::ReadV0DataArray<int>(kStream, aTemp, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
		m_paiNumResourcesLocal = aTemp;
	}
	kStream >> m_paiSpecialistProduction;
	kStream >> m_paiProjectProduction;

	m_pCityBuildings->Read(kStream);

	if(uiVersion >= 8)
	{
		int iNumEntries;
		FStringFixedBuffer(sTemp, 64);
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> sTemp;
			if(sTemp != "NO_UNIT")
			{
				iType = GC.getInfoTypeForString(sTemp);
				if(iType != -1)
				{
					int iTempValue;
					kStream >> iTempValue;
					m_paiUnitProduction.setAt(iType, iTempValue);
					kStream >> iTempValue;
					m_paiUnitProductionTime.setAt(iType, iTempValue);
					kStream >> iTempValue;
					m_paiGreatPeopleUnitRate.setAt(iType, iTempValue);
					kStream >> iTempValue;
					m_paiGreatPeopleUnitProgress.setAt(iType, iTempValue);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Unit Type not found: %s", sTemp);
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);

					int iTempValue;
					kStream >> iTempValue;
					kStream >> iTempValue;
					kStream >> iTempValue;
					kStream >> iTempValue;
				}
			}
		}
	}
	else
	{
		int iNumEntries;
		int iTempValue;
		kStream >> iNumEntries;
		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> iTempValue;
			m_paiUnitProduction.setAt(iI, iTempValue);
		}
		kStream >> iNumEntries;
		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> iTempValue;
			m_paiUnitProductionTime.setAt(iI, iTempValue);
		}
		kStream >> iNumEntries;
		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> iTempValue;
			m_paiGreatPeopleUnitRate.setAt(iI, iTempValue);
		}
		kStream >> iNumEntries;
		for(int iI = 0; iI < iNumEntries; iI++)
		{
			kStream >> iTempValue;
			m_paiGreatPeopleUnitProgress.setAt(iI, iTempValue);
		}
	}
	kStream >> m_paiSpecialistCount;
	kStream >> m_paiMaxSpecialistCount;
	kStream >> m_paiForceSpecialistCount;
	kStream >> m_paiFreeSpecialistCount;
	kStream >> m_paiImprovementFreeSpecialists;
	kStream >> m_paiUnitCombatFreeExperience;
	kStream >> m_paiUnitCombatProductionModifier;

	if(uiVersion >= 8)
	{
		int iNumEntries;
		FStringFixedBuffer(sTemp, 64);
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
		{
			int iTempValue;
			kStream >> sTemp;
			kStream >> iTempValue;
			if(sTemp != "PROMOTION_OLIGARCHY")
			{
				iType = GC.getInfoTypeForString(sTemp);
				m_paiFreePromotionCount.setAt(iType, iTempValue);
			}
		}
	}
	else
	{
		kStream >> m_paiFreePromotionCount;
	}

	UINT uLength;
	kStream >> uLength;
	for(UINT uIdx = 0; uIdx < uLength; ++uIdx)
	{
		OrderData  Data;

		kStream >> Data.eOrderType;
		kStream >> Data.iData1;
		kStream >> Data.iData2;
		kStream >> Data.bSave;
		kStream >> Data.bRush;

		m_orderQueue.insertAtEnd(&Data);
	}

	if(uiVersion <= 12)
		CvInfosSerializationHelper::ReadV0DataArray(kStream, m_ppaiResourceYieldChange, 4, GC.getNumResourceInfos(), CVINFO_V0_TAGS(CvInfosSerializationHelper::ms_V0ResourceTags));
	else if(uiVersion <=14)
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_ppaiResourceYieldChange, 4, GC.getNumResourceInfos());
	else
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_ppaiResourceYieldChange, NUM_YIELD_TYPES, GC.getNumResourceInfos());

	if(uiVersion == 4)
	{
		for(int i = 0; i < GC.getNumFeatureInfos(); ++i)
		{
			ArrayWrapper<int>wrapm_ppaiFeatureYieldChange(4, m_ppaiFeatureYieldChange[i]);
			kStream >> wrapm_ppaiFeatureYieldChange;
		}
		int temp[4];
		for(int i = 0; i < 2; ++i)
		{
			kStream >> temp;
		}
	}
	else if(uiVersion < 9)
	{
		for(int i = 0; i < 14; ++i)
		{
			ArrayWrapper<int>wrapm_ppaiFeatureYieldChange(4, m_ppaiFeatureYieldChange[i]);
			kStream >> wrapm_ppaiFeatureYieldChange;
		}
	}
	else if(uiVersion < 11)
	{
		for(int i = 0; i < 17; ++i)
		{
			ArrayWrapper<int>wrapm_ppaiFeatureYieldChange(4, m_ppaiFeatureYieldChange[i]);
			kStream >> wrapm_ppaiFeatureYieldChange;
		}
	}
	else if(uiVersion < 15)
	{
		for(int i = 0; i < 18; ++i)
		{
			ArrayWrapper<int>wrapm_ppaiFeatureYieldChange(4, m_ppaiFeatureYieldChange[i]);
			kStream >> wrapm_ppaiFeatureYieldChange;
		}
	}
	else
	{
		for(int i = 0; i < GC.getNumFeatureInfos(); ++i)
		{
			ArrayWrapper<int>wrapm_ppaiFeatureYieldChange(NUM_YIELD_TYPES, m_ppaiFeatureYieldChange[i]);
			kStream >> wrapm_ppaiFeatureYieldChange;
		}
	}

	if(uiVersion >= 22)
	{
		for(int i = 0; i < GC.getNumTerrainInfos(); ++i)
		{
			ArrayWrapper<int>wrapm_ppaiTerrainYieldChange(NUM_YIELD_TYPES, m_ppaiTerrainYieldChange[i]);
			kStream >> wrapm_ppaiTerrainYieldChange;
		}
	}

	kStream >> m_iPopulationRank;
	kStream >> m_bPopulationRankValid;
	kStream >> m_aiBaseYieldRank;
	kStream >> m_abBaseYieldRankValid;
	kStream >> m_aiYieldRank;
	kStream >> m_abYieldRankValid;

	kStream >> m_iGameTurnLastExpanded;
	m_strName = "";

	// New City Building Happiness
	if(uiVersion >= 10)
	{
		kStream >> m_iBaseHappinessFromBuildings;
		kStream >> m_iUnmoddedHappinessFromBuildings;
	}
	else
	{
		int iHappiness = 0;

		BuildingTypes eBuilding;
		CvBuildingEntry* buildingInfo;
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes) iBuildingLoop;

			if(GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				buildingInfo = GC.getBuildingInfo(eBuilding);

				if(buildingInfo)
					iHappiness += buildingInfo->GetHappiness();
			}
		}

		m_iBaseHappinessFromBuildings = 0;
		m_iUnmoddedHappinessFromBuildings = iHappiness;
	}

	if(uiVersion >= 6)
	{
		kStream >> m_bRouteToCapitalConnectedLastTurn;
		kStream >> m_bRouteToCapitalConnectedThisTurn;
		if(uiVersion >= 7)
		{
			kStream >> m_strName;
		}
	}
	else
	{
		m_bRouteToCapitalConnectedLastTurn = false;
		m_bRouteToCapitalConnectedThisTurn = false;
	}

	if (uiVersion >= 26)
	{
		kStream >> m_bOwedCultureBuilding;
	}
	else
	{
		m_bOwedCultureBuilding = false;
	}

	m_pCityStrategyAI->Read(kStream);
	if(m_eOwner != NO_PLAYER)
	{
		GET_PLAYER(getOwner()).GetFlavorManager()->AddFlavorRecipient(m_pCityStrategyAI, false /* bPropogateFlavorValues */);
	}
	m_pCityCitizens->Read(kStream);
	if(uiVersion >= 17)
	{
		kStream >> *m_pCityReligions;
	}
	m_pEmphases->Read(kStream);

	if(uiVersion >= 19)
	{
		kStream >> *m_pCityEspionage;
	}

	if(uiVersion >= 27)
	{
		kStream >> m_iExtraHitPoints;
	}
	else
	{
		// Recalculate
		int iTotalExtraHitPoints = 0;
		for(int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(eBuildingType);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

			if (pkBuildingInfo)
			{
				int iCount = m_pCityBuildings->GetNumBuilding(eBuilding);
				if(iCount > 0)
				{
					iTotalExtraHitPoints += (pkBuildingInfo->GetExtraCityHitPoints() * iCount);
				}
			}
		}

		// Change all at once, rather than one by one, else the clamping might adjust the current damage.
		ChangeExtraHitPoints(iTotalExtraHitPoints);
	}
}

//	--------------------------------------------------------------------------------
void CvCity::write(FDataStream& kStream) const
{
	VALIDATE_OBJECT

	// Current version number
	uint uiVersion = 27;
	kStream << uiVersion;

	kStream << m_iID;
	kStream << m_iX;
	kStream << m_iY;
	kStream << m_iRallyX;
	kStream << m_iRallyY;
	kStream << m_iGameTurnFounded;
	kStream << m_iGameTurnAcquired;
	kStream << m_iPopulation;
	kStream << m_iHighestPopulation;
	kStream << m_iNumGreatPeople;
	kStream << m_iBaseGreatPeopleRate;
	kStream << m_iGreatPeopleRateModifier;
	kStream << m_iGreatPeopleProgress;
	kStream << m_iJONSCultureStored;
	kStream << m_iJONSCultureLevel;
	kStream << m_iJONSCulturePerTurnFromBuildings;
	kStream << m_iJONSCulturePerTurnFromPolicies;
	kStream << m_iJONSCulturePerTurnFromSpecialists;
	kStream << m_iJONSCulturePerTurnFromReligion;
	kStream << m_iFaithPerTurnFromBuildings;
	kStream << m_iFaithPerTurnFromPolicies;
	kStream << m_iFaithPerTurnFromReligion;
	kStream << m_iCultureRateModifier;
	kStream << m_iNumWorldWonders;
	kStream << m_iNumTeamWonders;
	kStream << m_iNumNationalWonders;
	kStream << m_iWonderProductionModifier;
	kStream << m_iCapturePlunderModifier;  // Added for Version 3
	kStream << m_iPlotCultureCostModifier; // Added for Version 3
	kStream << m_iPlotBuyCostModifier; // Added for Version 12
	kStream << m_iMaintenance;
	kStream << m_iHealRate;
	kStream << m_iEspionageModifier;
	kStream << m_iNoOccupiedUnhappinessCount;
	kStream << m_iFood;
	kStream << m_iFoodKept;
	kStream << m_iMaxFoodKeptPercent;
	kStream << m_iOverflowProduction;
	kStream << m_iFeatureProduction;
	kStream << m_iMilitaryProductionModifier;
	kStream << m_iSpaceProductionModifier;
	kStream << m_iFreeExperience;
	kStream << m_iCurrAirlift;
	kStream << m_iMaxAirlift;
	kStream << m_iAirModifier;
	kStream << m_iNukeModifier;
	kStream << m_iCultureUpdateTimer;
	kStream << m_iCitySizeBoost;
	kStream << m_iSpecialistFreeExperience;
	kStream << m_iStrengthValue;
	kStream << m_iDamage;
	kStream << m_iThreatValue;
	kStream << m_iGarrisonedUnit;
	CvInfosSerializationHelper::WriteHashed(kStream, (ResourceTypes)(m_iResourceDemanded.get()));
	kStream << m_iWeLoveTheKingDayCounter;
	kStream << m_iLastTurnGarrisonAssigned;
	kStream << m_iThingsProduced;
	kStream << m_iDemandResourceCounter;
	kStream << m_iResistanceTurns;
	kStream << m_iRazingTurns;
	kStream << m_iCountExtraLuxuries;
	kStream << m_iCheapestPlotInfluence;
	kStream << m_unitBeingBuiltForOperation.m_iOperationID;
	kStream << m_unitBeingBuiltForOperation.m_iArmyID;
	kStream << m_unitBeingBuiltForOperation.m_iSlotID;

	kStream << m_bNeverLost;
	kStream << m_bDrafted;
	kStream << m_bAirliftTargeted;
	kStream << m_bProductionAutomated;
	kStream << m_bMadeAttack;
	kStream << m_bOccupied;
	kStream << m_bPuppet;
	kStream << m_bEverCapital;
	kStream << m_bIndustrialRouteToCapital;
	kStream << m_bFeatureSurrounded;

	kStream << m_eOwner;
	kStream << m_ePreviousOwner;
	kStream << m_eOriginalOwner;
	kStream << m_ePlayersReligion;

	kStream << m_aiSeaPlotYield;
	kStream << m_aiRiverPlotYield;
	kStream << m_aiLakePlotYield;   // Added in Version 2
	kStream << m_aiSeaResourceYield;
	kStream << m_aiBaseYieldRateFromTerrain;
	kStream << m_aiBaseYieldRateFromBuildings;
	kStream << m_aiBaseYieldRateFromSpecialists;
	kStream << m_aiBaseYieldRateFromMisc;
	kStream << m_aiBaseYieldRateFromReligion;
	kStream << m_aiYieldPerPop;
	kStream << m_aiYieldRateModifier;
	kStream << m_aiPowerYieldRateModifier;
	kStream << m_aiResourceYieldRateModifier;
	kStream << m_aiExtraSpecialistYield;
	kStream << m_aiProductionToYieldModifier;
	kStream << m_aiDomainFreeExperience;
	kStream << m_aiDomainProductionModifier;

	kStream << m_abEverOwned;
	kStream << m_abRevealed;

	kStream << m_strName;
	kStream << m_strScriptData;

	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiNoResource);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiFreeResource);
	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes, int>(kStream, m_paiNumResourcesLocal);

	kStream << m_paiSpecialistProduction;
	kStream << m_paiProjectProduction;

	m_pCityBuildings->Write(kStream);

	// Added for version 8
	FStringFixedBuffer(sTemp, 64);
	int iArraySize = GC.getNumUnitInfos();
	kStream << iArraySize;
	for(int iI = 0; iI < iArraySize; iI++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iI);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			sTemp = pkUnitInfo->GetType();
			kStream << sTemp;
			kStream << m_paiUnitProduction[iI];
			kStream << m_paiUnitProductionTime[iI];
			kStream << m_paiGreatPeopleUnitRate[iI];
			kStream << m_paiGreatPeopleUnitProgress[iI];
		}
		else
		{
			sTemp = "NO_UNIT";
			kStream << sTemp;
		}
	}

	kStream << m_paiSpecialistCount;
	kStream << m_paiMaxSpecialistCount;
	kStream << m_paiForceSpecialistCount;
	kStream << m_paiFreeSpecialistCount;
	kStream << m_paiImprovementFreeSpecialists;
	kStream << m_paiUnitCombatFreeExperience;
	kStream << m_paiUnitCombatProductionModifier;

	// Added for version 8
	iArraySize = GC.getNumPromotionInfos();
	kStream << iArraySize;
	for(int iI = 0; iI < iArraySize; iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			sTemp = pkPromotionInfo->GetType();
			kStream << sTemp;
			kStream << m_paiFreePromotionCount[iI];
		}
	}

	//  Write m_orderQueue
	UINT uLength = (UINT)m_orderQueue.getLength();
	kStream << uLength;
	for(UINT uIdx = 0; uIdx < uLength; ++uIdx)
	{
		OrderData* pData = m_orderQueue.getAt(uIdx);

		kStream << pData->eOrderType;
		kStream << pData->iData1;
		kStream << pData->iData2;
		kStream << pData->bSave;
		kStream << pData->bRush;
	}

	CvInfosSerializationHelper::WriteHashedDataArray<ResourceTypes>(kStream, m_ppaiResourceYieldChange, NUM_YIELD_TYPES, GC.getNumResourceInfos());

	for(int i = 0; i < GC.getNumFeatureInfos(); ++i)
	{
		kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_ppaiFeatureYieldChange[i]);
	}

	for(int i = 0; i < GC.getNumTerrainInfos(); ++i)
	{
		kStream << ArrayWrapper<int>(NUM_YIELD_TYPES, m_ppaiTerrainYieldChange[i]);
	}

	kStream << m_iPopulationRank;
	kStream << m_bPopulationRankValid;
	kStream << m_aiBaseYieldRank;
	kStream << m_abBaseYieldRankValid;
	kStream << m_aiYieldRank;
	kStream << m_abYieldRankValid;

	kStream << m_iGameTurnLastExpanded;

	kStream << m_iBaseHappinessFromBuildings;
	kStream << m_iUnmoddedHappinessFromBuildings;

	kStream << m_bRouteToCapitalConnectedLastTurn;
	kStream << m_bRouteToCapitalConnectedThisTurn;
	kStream << m_strName;
	kStream << m_bOwedCultureBuilding;

	m_pCityStrategyAI->Write(kStream);
	m_pCityCitizens->Write(kStream);
	kStream << *m_pCityReligions;
	m_pEmphases->Write(kStream);
	kStream << *m_pCityEspionage;

	kStream << m_iExtraHitPoints;
}


//	--------------------------------------------------------------------------------
bool CvCity::isValidBuildingLocation(BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return false;

	// Requires coast
	if(pkBuildingInfo->IsWater())
	{
		if(!isCoastal(pkBuildingInfo->GetMinAreaSize()))
			return false;
	}

	// Requires River
	if(pkBuildingInfo->IsRiver())
	{
		if(!(plot()->isRiver()))
			return false;
	}

	// Requires Fresh Water
	if(pkBuildingInfo->IsFreshWater())
	{
		if(!plot()->isFreshWater())
			return false;
	}

	// Requires adjacent Mountain
	if(pkBuildingInfo->IsMountain())
	{
		bool bFoundMountain = false;

		CvPlot* pAdjacentPlot;
		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iDirectionLoop));

			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isMountain())
				{
					bFoundMountain = true;
					break;
				}
			}
		}

		if(!bFoundMountain)
			return false;
	}

	// Requires nearby Mountain (within 2 tiles)
	if(pkBuildingInfo->IsNearbyMountainRequired())
	{
		bool bFoundMountain = false;

		const int iMountainRange = 2;
		CvPlot* pLoopPlot;

		for(int iDX = -iMountainRange; iDX <= iMountainRange; iDX++)
		{
			for(int iDY = -iMountainRange; iDY <= iMountainRange; iDY++)
			{
				pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMountainRange);
				if(pLoopPlot)
				{
					if(pLoopPlot->isMountain() && !pLoopPlot->IsNaturalWonder() && pLoopPlot->getOwner() == getOwner())
					{
						bFoundMountain = true;
						break;
					}
				}
			}

			if(bFoundMountain == true)
				break;
		}

		if(!bFoundMountain)
			return false;
	}

	// Requires Hills
	if(pkBuildingInfo->IsHill())
	{
		if(!plot()->isHills())
			return false;
	}

	// Requires Flat
	if(pkBuildingInfo->IsFlat())
	{
		if(plot()->isHills())
			return false;
	}

	// Requires city not built on certain terrain?
	TerrainTypes eTerrainProhibited = (TerrainTypes) pkBuildingInfo->GetProhibitedCityTerrain();
	if(eTerrainProhibited != NO_TERRAIN)
	{
		if(plot()->getTerrainType() == eTerrainProhibited)
		{
			return false;
		}
	}

	// Requires city to be on or next to a particular terrain type?
	TerrainTypes eTerrainRequired = (TerrainTypes) pkBuildingInfo->GetNearbyTerrainRequired();
	if(eTerrainRequired != NO_TERRAIN)
	{
		bool bFoundTerrain = false;

		// City on the right terrain?
		if(plot()->getTerrainType() == eTerrainRequired)
			bFoundTerrain = true;

		// Check adjacent plots
		if(!bFoundTerrain)
		{
			CvPlot* pAdjacentPlot;
			for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iDirectionLoop));

				if(pAdjacentPlot != NULL)
				{
					// City adjacent to the right terrain?
					if(pAdjacentPlot->getTerrainType() == eTerrainRequired)
					{
						bFoundTerrain = true;
						break;
					}
				}
			}
		}

		// Didn't find nearby required terrain
		if(!bFoundTerrain)
			return false;
	}

	return true;
}


// CACHE: cache frequently used values
///////////////////////////////////////

//	--------------------------------------------------------------------------------
void CvCity::invalidatePopulationRankCache()
{
	VALIDATE_OBJECT
	m_bPopulationRankValid = false;
}

//	--------------------------------------------------------------------------------
void CvCity::invalidateYieldRankCache(YieldTypes eYield)
{
	VALIDATE_OBJECT
	CvAssertMsg(eYield >= NO_YIELD && eYield < NUM_YIELD_TYPES, "invalidateYieldRankCache passed bogus yield index");

	if(eYield == NO_YIELD)
	{
		for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_abBaseYieldRankValid.setAt(iI, false);
			m_abYieldRankValid.setAt(iI, false);
		}
	}
	else
	{
		m_abBaseYieldRankValid.setAt(eYield, false);
		m_abYieldRankValid.setAt(eYield, false);
	}
}

//	--------------------------------------------------------------------------------
bool CvCity::isMadeAttack() const
{
	VALIDATE_OBJECT
	return m_bMadeAttack;
}

//	--------------------------------------------------------------------------------
void CvCity::setMadeAttack(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bMadeAttack = bNewValue;
}

//	--------------------------------------------------------------------------------
bool CvCity::canRangeStrike() const
{
	VALIDATE_OBJECT

	// Can't shoot more than once per turn
	if(isMadeAttack())
		return false;

	// Can't shoot when in resistance
	if(IsResistance() || IsRazing())
		return false;

	// Can't shoot if we have no HP left (shouldn't really ever happen)
	if(getDamage() == GetMaxHitPoints())
		return false;

	// Apparently it's possible for someone to fire during another player's turn
	if(!GET_PLAYER(getOwner()).isTurnActive())
		return false;

	return true;
}

//	--------------------------------------------------------------------------------
bool CvCity::CanRangeStrikeNow() const
{
	if(!canRangeStrike())
	{
		return false;
	}

	int iRange = GC.getCITY_ATTACK_RANGE();
	bool bIndirectFireAllowed = GC.getCAN_CITY_USE_INDIRECT_FIRE();
	CvPlot* pPlot = plot();
	int iX = getX();
	int iY = getY();
	TeamTypes eTeam = getTeam();
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	int iGameTurn = GC.getGame().getGameTurn();

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pTargetPlot = plotXY(iX, iY, iDX, iDY);
			bool bCanRangeStrike = true;

			if(!pTargetPlot)
			{
				continue;
			}

			if(!bIndirectFireAllowed)
			{
				if(!pPlot->canSeePlot(pTargetPlot, eTeam, iRange, NO_DIRECTION))
				{
					bCanRangeStrike = false;
				}
			}


			if(bCanRangeStrike)
			{
				if(pTargetPlot->isVisible(eTeam))
				{
					int iTargetPlotX = pTargetPlot->getX();
					int iTargetPlotY = pTargetPlot->getY();
					int iPlotDistance = plotDistance(iX, iY, iTargetPlotX, iTargetPlotY);
					if(iPlotDistance <= iRange)
					{
						if(canRangeStrikeAt(iTargetPlotX, iTargetPlotY))
						{
							if(m_eOwner == eActivePlayer)
							{
								if(iGameTurn != m_bombardCheckTurn)
								{
									m_bombardCheckTurn = iGameTurn;
									GC.getGame().endTurnTimerSemaphoreIncrement();
								}
							}

							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Does this City have a Building that allows it to Range Strike?
bool CvCity::IsHasBuildingThatAllowsRangeStrike() const
{
	VALIDATE_OBJECT
	bool bHasBuildingThatAllowsRangeStrike = false;

	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			// Has this Building
			if(GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Does it grant Range Strike ability?
				if(pkBuildingInfo->IsAllowsRangeStrike())
				{
					bHasBuildingThatAllowsRangeStrike = true;
					break;
				}
			}
		}
	}

	return bHasBuildingThatAllowsRangeStrike;
}

//	--------------------------------------------------------------------------------
bool CvCity::canRangeStrikeAt(int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canRangeStrike())
	{
		return false;
	}

	const CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	if(NULL == pTargetPlot)
	{
		return false;
	}

	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

	int iAttackRange = GC.getCITY_ATTACK_RANGE();

	if(plotDistance(plot()->getX(), plot()->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > iAttackRange)
	{
		return false;
	}

	if(!GC.getCAN_CITY_USE_INDIRECT_FIRE())
	{
		if(!plot()->canSeePlot(pTargetPlot, getTeam(), iAttackRange, NO_DIRECTION))
		{
			return false;
		}
	}

	// If it's NOT a city, see if there are any units to aim for
	if(!pTargetPlot->isCity())
	{
		if(!canRangedStrikeTarget(*pTargetPlot))
		{
			return false;
		}
	}
	else // I don't want cities attacking each other directly
	{
		return false;
	}

	return true;
}

//	----------------------------------------------------------------------------
CityTaskResult CvCity::rangeStrike(int iX, int iY)
{
	VALIDATE_OBJECT
	CvUnit* pDefender;

	CityTaskResult eResult = TASK_ABORTED;

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(NULL == pPlot)
	{
		return eResult;
	}

	if(!canRangeStrikeAt(iX, iY))
	{
		return eResult;
	}

	setMadeAttack(true);

	// No City
	if(!pPlot->isCity())
	{
		pDefender = rangedStrikeTarget(pPlot);

		CvAssert(pDefender != NULL);
		if(!pDefender) return TASK_ABORTED;

		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateRangedCombatInfo(*this, pDefender, *pPlot, &kCombatInfo);

		uint uiParentEventID = 0;
		if(!CvPreGame::quickCombat())
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible(false);
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				DLLUI->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}

			kCombatInfo.setVisualizeCombat(pPlot->isActiveVisible(false));

			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayCityCombat(pDllCombatInfo.get());

			// Set the combat units so that other missions do not continue until combat is over.
			pDefender->setCombatCity(this);
			setCombatUnit(pDefender);
			eResult = TASK_QUEUED;
		}
		else
		{
			eResult = TASK_COMPLETED;
		}

		CvUnitCombat::ResolveCombat(kCombatInfo, uiParentEventID);
	}

	if(m_eOwner == GC.getGame().getActivePlayer())
	{
		GC.getGame().endTurnTimerSemaphoreDecrement();
	}

	return eResult;
}

//	--------------------------------------------------------------------------------
bool CvCity::canRangedStrikeTarget(const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT
	return (const_cast<CvCity*>(this)->rangedStrikeTarget(const_cast<CvPlot*>(&targetPlot)) != 0);
}

//	--------------------------------------------------------------------------------
CvUnit* CvCity::rangedStrikeTarget(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	UnitHandle pDefender = pPlot->getBestDefender(NO_PLAYER, getOwner(), NULL, true, false, false, /*bNoncombatAllowed*/ true);

	if(pDefender)
	{
		if(!pDefender->IsDead())
		{
			return pDefender.pointer();
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
int CvCity::rangeCombatUnitDefense(const CvUnit* pDefender) const
{
	int iDefenderStrength = 0;

	// Use Ranged combat value for defender, UNLESS it's a boat
	if (pDefender->isEmbarked())
	{
		iDefenderStrength = pDefender->GetEmbarkedUnitDefense();;
	}

	else if(pDefender->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, false, false) > 0 && !pDefender->getDomainType() == DOMAIN_SEA)
	{
		iDefenderStrength = pDefender->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, false, false);

		// Ranged units take less damage from one another
		iDefenderStrength *= /*125*/ GC.getRANGE_ATTACK_RANGED_DEFENDER_MOD();
		iDefenderStrength /= 100;
	}
	else
	{
		iDefenderStrength = pDefender->GetMaxDefenseStrength(pDefender->plot(), NULL, /*bFromRangedAttack*/ true);
	}

	return iDefenderStrength;
}

//	--------------------------------------------------------------------------------
int CvCity::rangeCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand) const
{
	VALIDATE_OBJECT
	int iAttackerStrength;

	iAttackerStrength = getStrengthValue(true);

	int iDefenderStrength;

	// No City
	if(pCity == NULL)
	{
		// If this is a defenseless unit, do a fixed amount of damage
		if(!pDefender->IsCanDefend())
		{
			return GC.getNONCOMBAT_UNIT_RANGED_DAMAGE();
		}

		iDefenderStrength = rangeCombatUnitDefense(pDefender);

	}
	// City
	else
	{
		iDefenderStrength = pCity->getStrengthValue();
	}

	// The roll will vary damage between 30 and 40 (out of 100) for two units of identical strength

	int iAttackerDamage = /*250*/ GC.getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE();

	int iAttackerRoll = 0;
	if(bIncludeRand)
	{
		iAttackerRoll = GC.getGame().getJonRandNum(/*300*/ GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE(), "City Ranged Attack Damage");
	}
	else
	{
		iAttackerRoll = /*300*/ GC.getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE();
		iAttackerRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iAttackerRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iAttackerDamage += iAttackerRoll;


	double fStrengthRatio = (double(iAttackerStrength) / iDefenderStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if(iDefenderStrength > iAttackerStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iAttackerDamage = int(iAttackerDamage * fStrengthRatio);

	// Bring it back out of hundreds
	iAttackerDamage /= 100;

	// Always do at least 1 damage
	int iMinDamage = /*1*/ GC.getMIN_CITY_STRIKE_DAMAGE();
	if(iAttackerDamage < iMinDamage)
		iAttackerDamage = iMinDamage;

	return iAttackerDamage;
}

//	--------------------------------------------------------------------------------
int CvCity::GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand) const
{
	int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, false);
	int iDefenderStrength = getStrengthValue(false);

	// The roll will vary damage between 2 and 3 (out of 10) for two units of identical strength

	int iDefenderDamageRatio = GetMaxHitPoints() - getDamage();
	int iDefenderDamage = /*200*/ GC.getAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE() * iDefenderDamageRatio / GetMaxHitPoints();

	int iDefenderRoll = 0;
	if(bIncludeRand)
	{
		iDefenderRoll = /*200*/ GC.getGame().getJonRandNum(GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE(), "Unit Air Strike Combat Damage");
		iDefenderRoll *= iDefenderDamageRatio;
		iDefenderRoll /= GetMaxHitPoints();
	}
	else
	{
		iDefenderRoll = /*200*/ GC.getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE();
		iDefenderRoll -= 1;	// Subtract 1 here, because this is the amount normally "lost" when doing a rand roll
		iDefenderRoll *= iDefenderDamageRatio;
		iDefenderRoll /= GetMaxHitPoints();
		iDefenderRoll /= 2;	// The divide by 2 is to provide the average damage
	}
	iDefenderDamage += iDefenderRoll;

	double fStrengthRatio = (double(iDefenderStrength) / iAttackerStrength);

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result
	if (iAttackerStrength > iDefenderStrength)
	{
		fStrengthRatio = (double(iAttackerStrength) / iDefenderStrength);
	}

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	fStrengthRatio = (fStrengthRatio + 1) / 2;

	if (iAttackerStrength > iDefenderStrength)
	{
		fStrengthRatio = 1 / fStrengthRatio;
	}

	iDefenderDamage = int(iDefenderDamage * fStrengthRatio);

	// Bring it back out of hundreds
	iDefenderDamage /= 100;

	// Always do at least 1 damage
	int iMinDamage = /*1*/ GC.getMIN_CITY_STRIKE_DAMAGE();
	if(iDefenderDamage < iMinDamage)
		iDefenderDamage = iMinDamage;

	return iDefenderDamage;
}

//	--------------------------------------------------------------------------------
void CvCity::DoNearbyEnemy()
{
	// Can't actually range strike
	if(!canRangeStrike())
		return;

	int iSearchRange = GC.getCITY_ATTACK_RANGE();
	CvPlot* pBestPlot = NULL;

	bool bFoundEnemy = false;

	for(int iDX = -(iSearchRange); iDX <= iSearchRange && !pBestPlot; iDX++)
	{
		for(int iDY = -(iSearchRange); iDY <= iSearchRange && !pBestPlot; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iSearchRange);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->isVisibleEnemyUnit(getOwner()))
				{
					if(canRangeStrikeAt(pLoopPlot->getX(), pLoopPlot->getY()))
					{
						bFoundEnemy = true;

						// Notification
						CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
						if(pNotifications)
						{
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_CAN_SHOOT");
							strText << getNameKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_CAN_SHOOT");
							strSummary << getNameKey();
							pNotifications->Add(NOTIFICATION_CITY_RANGE_ATTACK, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), GetID());
						}

						break;
					}
				}
			}
		}

		if(bFoundEnemy)
			break;
	}
}

//	--------------------------------------------------------------------------------
void CvCity::CheckForAchievementBuilding(BuildingTypes eBuilding)
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
		return;

	const char* szBuildingTypeChar = pkBuildingInfo->GetType();
	CvString szBuilding = szBuildingTypeChar;

	if(szBuilding == "BUILDING_LONGHOUSE")
	{
		CvPlot* pLoopPlot;
		int nForests = 0;
		for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = plotCity(getX(), getY(), iI);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getOwner() == getOwner())
				{
					if(pLoopPlot->getFeatureType() == FEATURE_FOREST)
					{
						nForests++;
					}
				}
			}
		}
		if(nForests >=4)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_LONGHOUSE);
		}
	}
	if(szBuilding == "BUILDING_FLOATING_GARDENS")
	{
		int iCityX = getX();
		int iCityY = getY();
		PlayerTypes eCityOwner = getOwner();
		for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			CvPlot* pLoopPlot = plotCity(iCityX, iCityY, iI);

			if(pLoopPlot != NULL && pLoopPlot->getOwner() == eCityOwner && pLoopPlot->isLake())
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_GARDENS);
				break;
			}
		}
	}
	//DLC_06 achievement: Build Statue of Zeus and Temple of Artemis in same city
	if(szBuilding == "BUILDING_STATUE_ZEUS" || szBuilding == "BUILDING_TEMPLE_ARTEMIS")
	{
		CvString szOtherWonder = "";
		if(szBuilding == "BUILDING_STATUE_ZEUS")
		{
			szOtherWonder = "BUILDING_TEMPLE_ARTEMIS";
		}
		else
		{
			szOtherWonder = "BUILDING_STATUE_ZEUS";
		}
		BuildingTypes eOtherWonder = (BuildingTypes)GC.getInfoTypeForString(szOtherWonder);
		PlayerTypes eCityOwner = getOwner();
		if(GetCityBuildings()->GetNumBuilding(eOtherWonder) > 0)
		{
			if(GetCityBuildings()->GetBuildingOriginalOwner(eOtherWonder) == eCityOwner)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_ZEUS_AND_ARTEMIS);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvCity::IncrementUnitStatCount(CvUnit* pUnit)
{
	CvString szUnitType = pUnit->getUnitInfo().GetType();

	if(szUnitType == "UNIT_WARRIOR")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_WARRIOR);
	}
	else if(szUnitType == "UNIT_SETTLER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SETTLER);
	}
	else if(szUnitType == "UNIT_WORKER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_WORKER);
	}
	else if(szUnitType == "UNIT_WORKBOAT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_WORKBOAT);
	}
	else if(szUnitType == "UNIT_GREAT_GENERAL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GREATGENERALS);
	}
	else if(szUnitType == "UNIT_SS_STASIS_CHAMBER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SSSTASISCHAMBER);
	}
	else if(szUnitType == "UNIT_SS_ENGINE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SSENGINE);
	}
	else if(szUnitType == "UNIT_SS_COCKPIT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SSCOCKPIT);
	}
	else if(szUnitType == "UNIT_SS_BOOSTER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SSBOOSTER);
	}
	else if(szUnitType == "UNIT_MISSILE_CRUISER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MISSILECRUISER);
	}
	else if(szUnitType == "UNIT_NUCLEAR_SUBMARINE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NUCLEARSUBMARINE);
	}
	else if(szUnitType == "UNIT_CARRIER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CARRIER);
	}
	else if(szUnitType == "UNIT_BATTLESHIP")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BATTLESHIP);
	}
	else if(szUnitType == "UNIT_SUBMARINE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SUBMARINE);
	}
	else if(szUnitType == "UNIT_DESTROYER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_DESTROYER);
	}
	else if(szUnitType == "UNIT_IRONCLAD")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_IRONCLAD);
	}
	else if(szUnitType == "UNIT_FRIGATE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_FRIGATE);
	}
	else if(szUnitType == "UNIT_ENGLISH_SHIPOFTHELINE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SHIPOFTHELINE);
	}
	else if(szUnitType == "UNIT_CARAVEL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CARAVEL);
	}
	else if(szUnitType == "UNIT_TRIREME")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TRIREME);
	}
	else if(szUnitType == "UNIT_MECH")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GIANTDEATHROBOT);
	}
	else if(szUnitType == "UNIT_NUCLEAR_MISSILE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NUCLEARMISSILE);
	}
	else if(szUnitType == "UNIT_STEALTH_BOMBER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_STEALTHBOMBER);
	}
	else if(szUnitType == "UNIT_JET_FIGHTER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_JETFIGHTER);
	}
	else if(szUnitType == "UNIT_GUIDED_MISSILE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_GUIDEDMISSILE);
	}
	else if(szUnitType == "UNIT_MODERN_ARMOR")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MODERNARMOR);
	}
	else if(szUnitType == "UNIT_HELICOPTER_GUNSHIP")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HELICOPTERGUNSHIP);
	}
	else if(szUnitType == "UNIT_MOBILE_SAM")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MOBILESAM);
	}
	else if(szUnitType == "UNIT_ROCKET_ARTILLERY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ROCKETARTILLERY);
	}
	else if(szUnitType == "UNIT_MECHANIZED_INFANTRY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MECHANIZEDINFANTRY);
	}
	else if(szUnitType == "UNIT_ATOMIC_BOMB")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ATOMICBOMB);
	}
	else if(szUnitType == "UNIT_BOMBER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BOMBER);
	}
	else if(szUnitType == "UNIT_AMERICAN_B17")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_B17);
	}
	else if(szUnitType == "UNIT_FIGHTER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_FIGHTER);
	}
	else if(szUnitType == "UNIT_JAPANESE_ZERO")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ZERO);
	}
	else if(szUnitType == "UNIT_PARATROOPER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PARATROOPER);
	}
	else if(szUnitType == "UNIT_TANK")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TANK);
	}
	else if(szUnitType == "UNIT_GERMAN_PANZER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PANZER);
	}
	else if(szUnitType == "UNIT_ARTILLERY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ARTILLERY);
	}
	else if(szUnitType == "UNIT_ANTI_AIRCRAFT_GUN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ANTIAIRCRAFTGUN);
	}
	else if(szUnitType == "UNIT_ANTI_TANK_GUN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ANTITANKGUN);
	}
	else if(szUnitType == "UNIT_INFANTRY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_INFANTRY);
	}
	else if(szUnitType == "UNIT_FRENCH_FOREIGNLEGION")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_FOREIGNLEGION);
	}
	else if(szUnitType == "UNIT_CAVALRY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CAVALRY);
	}
	else if(szUnitType == "UNIT_RUSSIAN_COSSACK")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_COSSACK);
	}
	else if(szUnitType == "UNIT_RIFLEMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_RIFLEMAN);
	}
	else if(szUnitType == "UNIT_LANCER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LANCER);
	}
	else if(szUnitType == "UNIT_OTTOMAN_SIPAHI")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SIPAHI);
	}
	else if(szUnitType == "UNIT_CANNON")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CANNON);
	}
	else if(szUnitType == "UNIT_MUSKETMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MUSKETMAN);
	}
	else if(szUnitType == "UNIT_AMERICAN_MINUTEMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MINUTEMAN);
	}
	else if(szUnitType == "UNIT_FRENCH_MUSKETEER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MUSKETEER);
	}
	else if(szUnitType == "UNIT_OTTOMAN_JANISSARY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_JANISSARY);
	}
	else if(szUnitType == "UNIT_LONGSWORDSMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LONGSWORDSMAN);
	}
	else if(szUnitType == "UNIT_JAPANESE_SAMURAI")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SAMURAI);
	}
	else if(szUnitType == "UNIT_TREBUCHET")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_TREBUCHET);
	}
	else if(szUnitType == "UNIT_KNIGHT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_KNIGHT);
	}
	else if(szUnitType == "UNIT_SIAMESE_WARELEPHANT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_NARESUANSELEPHANT);
	}
	else if(szUnitType == "UNIT_SONGHAI_MUSLIMCAVALRY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MANDEKALUCAVALRY);
	}
	else if(szUnitType == "UNIT_CROSSBOWMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CROSSBOWMAN);
	}
	else if(szUnitType == "UNIT_CHINESE_CHUKONU")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CHUKONU);
	}
	else if(szUnitType == "UNIT_ARABIAN_CAMELARCHER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CAMELARCHER);
	}
	else if(szUnitType == "UNIT_ENGLISH_LONGBOWMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LONGBOWMAN);
	}
	else if(szUnitType == "UNIT_PIKEMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_PIKEMAN);
	}
	else if(szUnitType == "UNIT_GERMAN_LANDSKNECHT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LANDSKNECHT);
	}
	else if(szUnitType == "UNIT_CATAPULT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CATAPULT);
	}
	else if(szUnitType == "UNIT_ROMAN_BALLISTA")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_BALLISTA);
	}
	else if(szUnitType == "UNIT_HORSEMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HORSEMAN);
	}
	else if(szUnitType == "UNIT_GREEK_COMPANIONCAVALRY")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_COMPANIONCAVALRY);
	}
	else if(szUnitType == "UNIT_SWORDSMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SWORDSMAN);
	}
	else if(szUnitType == "UNIT_IROQUOIAN_MOHAWKWARRIOR")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_MOHAWKWARRIOR);
	}
	else if(szUnitType == "UNIT_ROMAN_LEGION")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_LEGION);
	}
	else if(szUnitType == "UNIT_CHARIOT_ARCHER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_CHARIOTARCHER);
	}
	else if(szUnitType == "UNIT_EGYPTIAN_WARCHARIOT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_WARCHARIOT);
	}
	else if(szUnitType == "UNIT_INDIAN_WARELEPHANT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_WARELEPHANT);
	}
	else if(szUnitType == "UNIT_SPEARMAN")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SPEARMAN);
	}
	else if(szUnitType == "UNIT_GREEK_HOPLITE")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_HOPLITE);
	}
	else if(szUnitType == "UNIT_PERSIAN_IMMORTAL")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_IMMORTAL);
	}
	else if(szUnitType == "UNIT_ARCHER")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_ARCHER);
	}
	else if(szUnitType == "UNIT_SCOUT")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_SCOUT);
	}
	else if(szUnitType == "UNIT_AZTEC_JAGUAR")
	{
		gDLL->IncrementSteamStat(ESTEAMSTAT_JAGUAR);
	}
	else
	{
		OutputDebugString("\nNo stat for selected unit type.\n");
	}

	bool bAllUnitsUnlocked;

	bAllUnitsUnlocked = AreAllUnitsBuilt();
	if(bAllUnitsUnlocked)
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_ALL_UNITS);
	}
}

//	--------------------------------------------------------------------------------
// Check to see if all the units have been built
bool CvCity::AreAllUnitsBuilt()
{
	int iI;
	int iUnitStatStart = 1;   //As they're defined on the backend
	int iUnitStatEnd = 79;
	int32 nStat = 0;

	for(iI = iUnitStatStart; iI < iUnitStatEnd; iI++)
	{
		if(gDLL->GetSteamStat((ESteamStat)iI, &nStat))
		{
			if(nStat <= 0)
			{
				return false;
			}
		}
	}
	//Whoops, one is out of order
	if(gDLL->GetSteamStat(ESTEAMSTAT_CAVALRY, &nStat))
	{
		if(nStat <=0)
		{
			return false;
		}
	}
	return true;

}

//	--------------------------------------------------------------------------------
/// Build a unit needed to fill in an army for an operation
bool CvCity::CommitToBuildingUnitForOperation()
{
	VALIDATE_OBJECT
	UnitTypes eBestUnit;
	UnitAITypes eUnitAI;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	OperationSlot thisOperationSlot = kPlayer.PeekAtNextUnitToBuildForOperationSlot(getArea());

	if(thisOperationSlot.IsValid())
	{

		CvArmyAI* pThisArmy = kPlayer.getArmyAI(thisOperationSlot.m_iArmyID);

		if(pThisArmy)
		{
			// figure out the primary and secondary unit type to potentially build
			int iFormationIndex = pThisArmy->GetFormationIndex();
			CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iFormationIndex);
			if(thisFormation)
			{
				const CvFormationSlotEntry& slotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);

				eUnitAI = (UnitAITypes)slotEntry.m_primaryUnitType;
				eBestUnit = m_pCityStrategyAI->GetUnitProductionAI()->RecommendUnit(eUnitAI);
				if(eBestUnit == NO_UNIT)
				{
					eUnitAI = (UnitAITypes)slotEntry.m_secondaryUnitType;
					eBestUnit = m_pCityStrategyAI->GetUnitProductionAI()->RecommendUnit(eUnitAI);
				}

				if(eBestUnit != NO_UNIT)
				{
					// Always try to rush units for operational AI if possible
					pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI, false, false, false, true /*bRush*/);
					OperationSlot thisOperationSlot2 = kPlayer.CityCommitToBuildUnitForOperationSlot(getArea(), getProductionTurnsLeft(), this);
					m_unitBeingBuiltForOperation = thisOperationSlot2;
					return true;
				}
			}
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
/// Which unit would we build if we are building one for an operation?
UnitTypes CvCity::GetUnitForOperation()
{
	VALIDATE_OBJECT
	UnitTypes eBestUnit;
	UnitAITypes eUnitAI;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	OperationSlot thisOperationSlot = kPlayer.PeekAtNextUnitToBuildForOperationSlot(getArea());

	if(thisOperationSlot.IsValid())
	{
		CvArmyAI* pThisArmy = kPlayer.getArmyAI(thisOperationSlot.m_iArmyID);

		if(pThisArmy)
		{
			// figure out the primary and secondary unit type to potentially build
			int iFormationIndex = pThisArmy->GetFormationIndex();
			CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(iFormationIndex);
			if(thisFormation)
			{
				const CvFormationSlotEntry& slotEntry = thisFormation->getFormationSlotEntry(thisOperationSlot.m_iSlotID);

				eUnitAI = (UnitAITypes)slotEntry.m_primaryUnitType;
				eBestUnit = m_pCityStrategyAI->GetUnitProductionAI()->RecommendUnit(eUnitAI);
				if(eBestUnit == NO_UNIT)
				{
					eUnitAI = (UnitAITypes)slotEntry.m_secondaryUnitType;
					eBestUnit = m_pCityStrategyAI->GetUnitProductionAI()->RecommendUnit(eUnitAI);
				}

				if(eBestUnit != NO_UNIT)
				{
					return eBestUnit;
				}
			}
		}
	}
	return NO_UNIT;
}

//	--------------------------------------------------------------------------------
/// What does a City shoot when attacking a Unit?
const char* CvCity::GetCityBombardEffectTag() const
{
	EraTypes eCityEra = GET_TEAM(getTeam()).GetCurrentEra();

	return GC.getEraInfo(eCityEra)->GetCityBombardEffectTag();
}

//	--------------------------------------------------------------------------------
uint CvCity::GetCityBombardEffectTagHash() const
{
	EraTypes eCityEra = GET_TEAM(getTeam()).GetCurrentEra();

	return GC.getEraInfo(eCityEra)->GetCityBombardEffectTagHash();
}

//	---------------------------------------------------------------------------
int CvCity::GetMaxHitPoints() const
{
	return GC.getMAX_CITY_HIT_POINTS() + m_iExtraHitPoints;
}

//	--------------------------------------------------------------------------------
int CvCity::GetExtraHitPoints() const
{
	return m_iExtraHitPoints;
}

//	--------------------------------------------------------------------------------
void CvCity::ChangeExtraHitPoints(int iValue)
{
	if (iValue != 0)
	{
		m_iExtraHitPoints += iValue;
		FAssertMsg(m_iExtraHitPoints >= 0, "Trying to set ExtraHitPoints to a negative value");
		if (m_iExtraHitPoints < 0)
			m_iExtraHitPoints = 0;

		int iCurrentDamage = getDamage();
		FAssertMsg(iCurrentDamage <= GetMaxHitPoints(), "Loss of extra hit points will bring the city's damage to maximum");
		if (iCurrentDamage > GetMaxHitPoints())
			setDamage(iCurrentDamage);		// Call setDamage, it will clamp the value.
	}
}

//	--------------------------------------------------------------------------------
const FAutoArchive& CvCity::getSyncArchive() const
{
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
FAutoArchive& CvCity::getSyncArchive()
{
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
std::string CvCity::debugDump(const FAutoVariableBase& /*var*/) const
{
	std::string result = "Game Turn : ";
	char gameTurnBuffer[8] = {0};
	int gameTurn = GC.getGame().getGameTurn();
	sprintf_s(gameTurnBuffer, "%d\0", gameTurn);
	result += gameTurnBuffer;
	return result;
}

//	--------------------------------------------------------------------------------
std::string CvCity::stackTraceRemark(const FAutoVariableBase& var) const
{
	std::string result = debugDump(var);
	if(&var == &m_aiBaseYieldRateFromTerrain)
	{
		result += std::string("\nlast yield used to update from terrain = ") + FSerialization::toString(s_lastYieldUsedToUpdateRateFromTerrain) + std::string("\n");
		result += std::string("change value used for update = ") + FSerialization::toString(s_changeYieldFromTerreain) + std::string("\n");
	}
	return result;
}

//	---------------------------------------------------------------------------
bool CvCity::IsBusy() const
{
	return getCombatUnit() != NULL;
}

//	---------------------------------------------------------------------------
const CvUnit* CvCity::getCombatUnit() const
{
	return ::getUnit(m_combatUnit);
}

//	---------------------------------------------------------------------------
CvUnit* CvCity::getCombatUnit()
{
	return ::getUnit(m_combatUnit);
}

//	---------------------------------------------------------------------------
void CvCity::setCombatUnit(CvUnit* pCombatUnit, bool /*bAttacking*/)
{
	if(pCombatUnit != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL , "Combat Unit is not expected to be assigned");
		CvAssertMsg(!(plot()->isCityFighting()), "(plot()->isCityFighting()) did not return false as expected");
		m_combatUnit = pCombatUnit->GetIDInfo();
	}
	else
	{
		clearCombat();
	}
}

//	----------------------------------------------------------------------------
void CvCity::clearCombat()
{
	if(getCombatUnit() != NULL)
	{
		CvAssertMsg(plot()->isCityFighting(), "plot()->isCityFighting is expected to be true");
		m_combatUnit.reset();
	}
}

//	----------------------------------------------------------------------------
//	Return true if the city is fighting with someone.	Equivalent to the CvUnit call.
bool CvCity::isFighting() const
{
	return getCombatUnit() != NULL;
}

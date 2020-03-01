/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "FBatchAllocate.h"
// team.h

#ifndef CIV5_TEAM_H
#define CIV5_TEAM_H

class CvArea;
class CvTeamTechs;

class CvTeam
{
private:
	CvTeam(const CvTeam&); //hide copy constructor
	CvTeam& operator=(const CvTeam&); //hide assignment operator

public:
	CvTeam();
	~CvTeam();

	// inlined for performance reasons, only in the dll
	static CvTeam& getTeam(TeamTypes eTeam);
	static void initStatics();
	static void freeStatics();

	void init(TeamTypes eID);
	void reset(TeamTypes eID = (TeamTypes)0, bool bConstructorCall = false);
	void uninit();


	void addTeam(TeamTypes eTeam);
	void shareItems(TeamTypes eTeam);
	void shareCounters(TeamTypes eTeam);
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bFirst);

	void doTurn();

	void DoBarbarianTech();
	void DoMinorCivTech();

	void updateYield();

	bool canChangeWarPeace(TeamTypes eTeam) const;
#if defined(MOD_EVENTS_WAR_AND_PEACE)
	bool canDeclareWar(TeamTypes eTeam, PlayerTypes eOriginatingPlayer = NO_PLAYER);
	void declareWar(TeamTypes eTeam, bool bDefensivePact = false, PlayerTypes eOriginatingPlayer = NO_PLAYER);
	void makePeace(TeamTypes eTeam, bool bBumpUnits = true, bool bSuppressNotification = false, PlayerTypes eOriginatingPlayer = NO_PLAYER);
#else
	bool canDeclareWar(TeamTypes eTeam) const;
	void declareWar(TeamTypes eTeam, bool bDefensivePact = false);
	void makePeace(TeamTypes eTeam, bool bBumpUnits = true, bool bSuppressNotification = false);
#endif

	int GetTurnMadePeaceTreatyWithTeam(TeamTypes eTeam) const;
	void SetTurnMadePeaceTreatyWithTeam(TeamTypes eTeam, int iNewValue);
	bool IsHasBrokenPeaceTreaty() const;
	void SetHasBrokenPeaceTreaty(bool bValue);

	void meet(TeamTypes eTeam, bool bSuppressMessages);

	int getPower() const;
	int getDefensivePower() const;
	int getEnemyPower() const;
	int getNumNukeUnits() const;

	// DEPRECATED
	TeamTypes GetTeamVotingForInDiplo() const;
	int GetProjectedVotesFromMinorAllies() const;
	int GetProjectedVotesFromLiberatedMinors() const;
	int GetProjectedVotesFromCivs() const;
	int GetTotalProjectedVotes() const;
	int GetTotalSecuredVotes() const;
	// End DEPRECATED

	int getAtWarCount(bool bIgnoreMinors) const;
	int getHasMetCivCount(bool bIgnoreMinors) const;
	bool hasMetHuman() const;
	int getDefensivePactCount(TeamTypes eTeam = NO_TEAM) const;

	int getUnitClassMaking(UnitClassTypes eUnitClass) const;
	int getUnitClassCountPlusMaking(UnitClassTypes eIndex) const;
	int getBuildingClassMaking(BuildingClassTypes eBuildingClass) const;
	int getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const;

	int countNumUnitsByArea(CvArea* pArea) const;
	int countNumCitiesByArea(CvArea* pArea) const;
	int countTotalPopulationByArea(CvArea* pArea) const;
	int countEnemyDangerByArea(CvArea* pArea) const;

	bool isHuman() const;
	bool isObserver() const;
	bool isBarbarian() const;
	bool isMinorCiv() const;
#if defined(MOD_API_EXTENSIONS)
	bool isMajorCiv() const;
#endif

	int GetNumMinorCivsAttacked() const;
	void SetNumMinorCivsAttacked(int iValue);
	void ChangeNumMinorCivsAttacked(int iChange);
	bool IsMinorCivAggressor() const;
	bool IsMinorCivWarmonger() const;

	// Some diplo stuff
	bool IsBrokenMilitaryPromise() const;
	void SetBrokenMilitaryPromise(bool bValue);
	bool IsBrokenExpansionPromise() const;
	void SetBrokenExpansionPromise(bool bValue);
	bool IsBrokenBorderPromise() const;
	void SetBrokenBorderPromise(bool bValue);
	bool IsBrokenCityStatePromise() const;
	void SetBrokenCityStatePromise(bool bValue);
#if defined(MOD_BALANCE_CORE)
	bool IsCivilianKiller() const;
	void SetCivilianKiller(bool bValue);
#endif

	PlayerTypes getLeaderID() const;
	PlayerTypes getSecretaryID() const;
	HandicapTypes getHandicapType() const;
	CvString getName() const;
	CvString getNameKey() const;

	int getNumMembers() const;
	void changeNumMembers(int iChange);

#if defined(MOD_BALANCE_CORE)
	bool addPlayer(PlayerTypes eID);
	void removePlayer(PlayerTypes eID);
	const std::vector<PlayerTypes>& getPlayers();
	bool isMember(PlayerTypes eID) const;
	void updateTeamStatus();
	void ClearWarDeclarationCache();

	int getCorporationsEnabledCount() const;
	bool IsCorporationsEnabled() const;
	void changeCorporationsEnabledCount(int iChange);
#endif

	int getAliveCount() const;
	void changeAliveCount(int iChange);

	bool isAlive() const
	{
		return m_iAliveCount > 0;
	}

	int getEverAliveCount() const;
	int isEverAlive() const;
	void changeEverAliveCount(int iChange);

	int getNumCities() const;
	void changeNumCities(int iChange);

	int getTotalPopulation() const;
	void changeTotalPopulation(int iChange);

	int getTotalLand() const;
	void changeTotalLand(int iChange);

	int getNukeInterception() const;
	void changeNukeInterception(int iChange);

	int getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource) const;
	bool isForceTeamVoteEligible(VoteSourceTypes eVoteSource) const;
	void changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange);

	int getExtraWaterSeeFromCount() const;
	bool isExtraWaterSeeFrom() const;
	void changeExtraWaterSeeFromCount(int iChange);

	int getMapTradingCount() const;
	bool isMapTrading() const;
	void changeMapTradingCount(int iChange);

	int getTechTradingCount() const;
	bool isTechTrading() const;
	void changeTechTradingCount(int iChange);

	int getGoldTradingCount() const;
	bool isGoldTrading() const;
	void changeGoldTradingCount(int iChange);

	bool HavePolicyInTeam(PolicyTypes ePolicy);

	int getAllowEmbassyTradingAllowedCount() const;
	bool isAllowEmbassyTradingAllowed() const;
	void changeAllowEmbassyTradingAllowedCount(int iChange);

	int getOpenBordersTradingAllowedCount() const;
	bool isOpenBordersTradingAllowed() const;
	bool isOpenBordersTradingAllowedWithTeam(TeamTypes eTeam) const;
	void changeOpenBordersTradingAllowedCount(int iChange);

	int getDefensivePactTradingAllowedCount() const;
	bool isDefensivePactTradingAllowed() const;
	bool isDefensivePactTradingAllowedWithTeam(TeamTypes eTeam) const;
	void changeDefensivePactTradingAllowedCount(int iChange);

	int GetResearchAgreementTradingAllowedCount() const;
	bool IsResearchAgreementTradingAllowed() const;
	bool IsResearchAgreementTradingAllowedWithTeam(TeamTypes eTeam) const;
	void ChangeResearchAgreementTradingAllowedCount(int iChange);

	int GetTradeAgreementTradingAllowedCount() const;
	bool IsTradeAgreementTradingAllowed() const;
	void ChangeTradeAgreementTradingAllowedCount(int iChange);

	int getPermanentAllianceTradingCount() const;
	bool isPermanentAllianceTrading() const;
	void changePermanentAllianceTradingCount(int iChange);

#if defined(MOD_TECHS_CITY_WORKING)
	int GetCityWorkingChange() const;
	bool isCityWorkingChange() const;
	void changeCityWorkingChange(int iChange);
#endif
	
#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	int GetCityAutomatonWorkersChange() const;
	bool isCityAutomatonWorkersChange() const;
	void changeCityAutomatonWorkersChange(int iChange);
#endif

	int getBridgeBuildingCount() const;
	bool isBridgeBuilding() const;
	void changeBridgeBuildingCount(int iChange);

#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
	int getCityLessEmbarkCost() const;
	bool isCityLessEmbarkCost() const;
	void changeCityLessEmbarkCost(int iChange);

	int getCityNoEmbarkCost() const;
	bool isCityNoEmbarkCost() const;
	void changeCityNoEmbarkCost(int iChange);
#endif

	int getWaterWorkCount() const;
	bool isWaterWork() const;
	void changeWaterWorkCount(int iChange);

	int getBorderObstacleCount() const;
	bool isBorderObstacle() const;
	void changeBorderObstacleCount(int iChange);

	bool isMapCentering() const;
	void setMapCentering(bool bNewValue);

	TeamTypes GetID() const;

	int getTechShareCount(int iIndex) const;
	bool isTechShare(int iIndex) const;
	void changeTechShareCount(int iIndex, int iChange);

	int getExtraMoves(DomainTypes eIndex) const;
	void changeExtraMoves(DomainTypes eIndex, int iChange);

	bool canEmbark() const;
	int getCanEmbarkCount() const;
	void changeCanEmbarkCount(int iChange);

	bool canDefensiveEmbark() const;
	int getDefensiveEmbarkCount() const;
	void changeDefensiveEmbarkCount(int iChange);
	void UpdateEmbarkGraphics();

	bool canEmbarkAllWaterPassage() const;
	int getEmbarkedAllWaterPassage() const;
	void changeEmbarkedAllWaterPassage(int iChange);

	int getEmbarkedExtraMoves() const;
	void changeEmbarkedExtraMoves(int iChange);

	int GetNumNaturalWondersDiscovered() const;
	void ChangeNumNaturalWondersDiscovered(int iChange);

	int GetNumLandmarksBuilt() const;
	void ChangeNumLandmarksBuilt(int iChange);

	bool isHasMet(TeamTypes eIndex) const;
	void makeHasMet(TeamTypes eIndex, bool bSuppressMessages);

	int GetTurnsSinceMeetingTeam(TeamTypes eTeam) const;
	int GetTurnTeamMet(TeamTypes eTeam) const;
	void SetTurnTeamMet(TeamTypes eTeam, int iTurn);

	bool IsHasFoundPlayersTerritory(PlayerTypes ePlayer) const;
	bool SetHasFoundPlayersTerritory(PlayerTypes ePlayer, bool bValue);

#if defined(MOD_EVENTS_WAR_AND_PEACE)
	bool isAggressor(TeamTypes eIndex) const;
	bool isPacifier(TeamTypes eIndex) const;
#endif

	bool isAtWar(TeamTypes eIndex) const;
#if defined(MOD_EVENTS_WAR_AND_PEACE)
	void setAtWar(TeamTypes eIndex, bool bNewValue, bool bAggressorPacifier);
#else
	void setAtWar(TeamTypes eIndex, bool bNewValue);
#endif
	bool HasCommonEnemy(TeamTypes eOtherTeam) const;

	int GetNumTurnsAtWar(TeamTypes eTeam) const;
	void SetNumTurnsAtWar(TeamTypes eTeam, int iValue);
	void ChangeNumTurnsAtWar(TeamTypes eTeam, int iChange);

	int GetNumTurnsLockedIntoWar(TeamTypes eTeam) const;
	void SetNumTurnsLockedIntoWar(TeamTypes eTeam, int iValue);
	void ChangeNumTurnsLockedIntoWar(TeamTypes eTeam, int iChange);

	bool isPermanentWarPeace(TeamTypes eIndex) const;
	void setPermanentWarPeace(TeamTypes eIndex, bool bNewValue);

	TeamTypes GetLiberatedByTeam() const;
	void SetLiberatedByTeam(TeamTypes eIndex);

	TeamTypes GetKilledByTeam() const;
	void SetKilledByTeam(TeamTypes eIndex);

	void CloseEmbassyAtTeam(TeamTypes eIndex);
	bool HasEmbassyAtTeam(TeamTypes eIndex) const;
	void SetHasEmbassyAtTeam(TeamTypes eIndex, bool bNewValue);

#if defined(MOD_API_EXTENSIONS)
	bool HasSpyAtTeam(TeamTypes eIndex) const;
#endif

	void EvacuateDiplomatsAtTeam(TeamTypes eIndex);

	bool IsAllowsOpenBordersToTeam(TeamTypes eIndex) const;
	void SetAllowsOpenBordersToTeam(TeamTypes eIndex, bool bNewValue);

	bool IsHasDefensivePact(TeamTypes eIndex) const;
	void SetHasDefensivePact(TeamTypes eIndex, bool bNewValue);

	int GetTotalNumResearchAgreements() const;
	bool IsHasResearchAgreement(TeamTypes eIndex) const;
	void SetHasResearchAgreement(TeamTypes eIndex, bool bNewValue);
	void CancelResearchAgreement(TeamTypes eIndex);

	bool IsHasTradeAgreement(TeamTypes eIndex) const;
	void SetHasTradeAgreement(TeamTypes eIndex, bool bNewValue);

	bool isForcePeace(TeamTypes eIndex) const;
	void setForcePeace(TeamTypes eIndex, bool bNewValue);

	int getRouteChange(RouteTypes eIndex) const;
	void changeRouteChange(RouteTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	int getTradeRouteDomainExtraRange(DomainTypes eIndex) const;
	void changeTradeRouteDomainExtraRange(DomainTypes eIndex, int iChange);
#endif

	int getBuildTimeChange(BuildTypes eIndex) const;
	void changeBuildTimeChange(BuildTypes eIndex, int iChange);

	RouteTypes GetBestPossibleRoute();
	void SetBestPossibleRoute(RouteTypes eRoute);
	void DoUpdateBestRoute();

	int getProjectCount(ProjectTypes eIndex) const;
	int getProjectDefaultArtType(ProjectTypes eIndex) const;
	void setProjectDefaultArtType(ProjectTypes eIndex, int value);
	int getProjectArtType(ProjectTypes eIndex, int number) const;
	void setProjectArtType(ProjectTypes eIndex, int number, int value);
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra = 0) const;
	bool isProjectAndArtMaxedOut(ProjectTypes eIndex) const;
	void changeProjectCount(ProjectTypes eIndex, int iChange);
	void finalizeProjectArtTypes();

	int getProjectMaking(ProjectTypes eIndex) const;
	void changeProjectMaking(ProjectTypes eIndex, int iChange);

	int getUnitClassCount(UnitClassTypes eIndex) const;
	bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra = 0) const;
	void changeUnitClassCount(UnitClassTypes eIndex, int iChange);

	int getBuildingClassCount(BuildingClassTypes eIndex) const;
	bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra = 0) const;
	void changeBuildingClassCount(BuildingClassTypes eIndex, int iChange);

	int getObsoleteBuildingCount(BuildingTypes eIndex) const;
	bool isObsoleteBuilding(BuildingTypes eIndex) const;
	void changeObsoleteBuildingCount(BuildingTypes eIndex, int iChange);

	void enhanceBuilding(BuildingTypes eIndex, int iChange);

	int getTerrainTradeCount(TerrainTypes eIndex) const;
	bool isTerrainTrade(TerrainTypes eIndex) const;
	void changeTerrainTradeCount(TerrainTypes eIndex, int iChange);

#if defined(MOD_BALANCE_CORE)
	void setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce, bool bNoBonus = false);
#else
	void setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce);
#endif
	CvTeamTechs* GetTeamTechs() const;

#if defined(MOD_API_UNIFIED_YIELDS)
	int getFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2) const;
	void changeFeatureYieldChange(FeatureTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	void changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange);
#endif

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getImprovementNoFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementNoFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getImprovementFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	bool isAtWarWithHumans() const;
	bool isSimultaneousTurns() const;

	// DEPRECATED
	bool IsHomeOfUnitedNations() const;
	void SetHomeOfUnitedNations(bool bValue);
	// End DEPRECATED

	int getVictoryCountdown(VictoryTypes eIndex) const;
	void setVictoryCountdown(VictoryTypes eIndex, int iTurnsLeft);
	void changeVictoryCountdown(VictoryTypes eIndex, int iChange);
	int getVictoryDelay(VictoryTypes eVictory) const;
	bool canLaunch(VictoryTypes eVictory) const;
	void setCanLaunch(VictoryTypes eVictory, bool bCan);
	void resetVictoryProgress();

	bool isVictoryAchieved(VictoryTypes eVictory) const;
	void setVictoryAchieved(VictoryTypes eVictory, bool bValue);

	int GetScore() const;

	int getVictoryPoints() const;
	void changeVictoryPoints(int iChange);

	void DoTestSmallAwards();
	bool IsSmallAwardAchieved(SmallAwardTypes eAward) const;
	void SetSmallAwardAchieved(SmallAwardTypes eAward, bool bValue);

	void verifySpyUnitsValidPlot();

	void setForceRevealedResource(ResourceTypes eResource, bool bRevealed);
	bool isForceRevealedResource(ResourceTypes eResource) const;

	bool IsResourceObsolete(ResourceTypes eResource);

	bool HasTechForWorldCongress() const;
	void SetHasTechForWorldCongress(bool bValue);

	EraTypes GetCurrentEra() const;
	void SetCurrentEra(EraTypes eNewValue);

	int countNumHumanGameTurnActive() const;
	void setTurnActive(bool bNewValue, bool bTurn = true);
	bool isTurnActive() const;

	void PushIgnoreWarning (TeamTypes eTeam);
	void PopIgnoreWarning (TeamTypes eTeam);
	int GetIgnoreWarningCount (TeamTypes eTeam);

	void setDynamicTurnsSimultMode(bool simultaneousTurns);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool IsVoluntaryVassal(TeamTypes eIndex) const;
	bool IsVassal(TeamTypes eIndex) const;
	void setVassal(TeamTypes eIndex, bool bNewValue, bool bVoluntary = false);

	TeamTypes GetMaster() const;

	bool IsVassalOfSomeone() const;
	
	bool canBecomeVassal(TeamTypes eTeam, bool bIgnoreAlreadyVassal = false) const;
	bool CanMakeVassal(TeamTypes eTeam, bool bIgnoreAlreadyVassal = false) const;
	void DoBecomeVassal(TeamTypes eTeam, bool bVoluntary = false);
	bool canEndVassal(TeamTypes eTeam) const;
	bool canEndAllVassal();
	void DoEndVassal(TeamTypes eTeam, bool bPeaceful, bool bSuppressNotification);

	void DoLiberateVassal(TeamTypes eTeam);
	bool CanLiberateVassal(TeamTypes eTeam) const;

	void DoUpdateVassalWarPeaceRelationships();

	int getNumCitiesWhenVassalMade() const;
	void setNumCitiesWhenVassalMade(int iValue);
	int getTotalPopulationWhenVassalMade() const;
	void setTotalPopulationWhenVassalMade(int iValue);

	int GetNumTurnsIsVassal() const;
	void SetNumTurnsIsVassal(int iValue);
	void ChangeNumTurnsIsVassal(int iChange);

	int GetNumTurnsSinceVassalEnded(TeamTypes eTeam) const;
	void SetNumTurnsSinceVassalEnded(TeamTypes eTeam, int iValue);
	void ChangeNumTurnsSinceVassalEnded(TeamTypes eTeam, int iChange);
	bool IsTooSoonForVassal(TeamTypes eTeam) const;

	bool IsVassalLockedIntoWar(TeamTypes eOtherTeam) const;

	int getVassalageTradingAllowedCount() const;
	bool IsVassalageTradingAllowed() const;
	void changeVassalageTradingAllowedCount(int iChange);

	int GetNumVassals();

	void InitAdvancedActionsEspionage();

	bool IsTradeTech(TechTypes eTech) const;
	void SetTradeTech(TechTypes eTech, bool bValue);

	void AcquireMap(TeamTypes eIndex, bool bTerritoryOnly = false);

	void DoApplyVassalTax(PlayerTypes ePlayer, int iPercent);
	bool CanSetVassalTax(PlayerTypes ePlayer) const;
	void SetVassalTax(PlayerTypes ePlayer, int iPercent);
	int GetVassalTax(PlayerTypes ePlayer) const;

	int GetNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer) const;
	void SetNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer, int iValue);
	void ChangeNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer, int iChange);

	int GetSSProjectCount();
#endif

	// Wrapper for giving Players on this Team a notification message
	void AddNotification(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX = -1, int iY = -1, int iGameDataIndex = -1, int iExtraGameData = -1);

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

protected:

	TeamTypes m_eID;

	static CvTeam* m_aTeams;

#if defined(MOD_BALANCE_CORE)
	//we care about iteration speed, so use a vector over a set
	std::vector<PlayerTypes> m_members;
	std::map<std::pair<TeamTypes,PlayerTypes>,bool> m_cacheCanDeclareWar;
	bool m_bIsMinorTeam;
	bool m_bIsObserverTeam;
	int m_iCorporationsEnabledCount;
#endif

	int m_iNumMembers;
	int m_iAliveCount;
	int m_iEverAliveCount;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iNukeInterception;
	int m_iExtraWaterSeeFromCount;
	int m_iMapTradingCount;
	int m_iTechTradingCount;
	int m_iGoldTradingCount;
	int m_iAllowEmbassyTradingAllowedCount;
	int m_iOpenBordersTradingAllowedCount;
	int m_iDefensivePactTradingAllowedCount;
	int m_iResearchAgreementTradingAllowedCount;
	int m_iTradeAgreementTradingAllowedCount;
	int m_iPermanentAllianceTradingCount;
#if defined(MOD_TECHS_CITY_WORKING)
	int m_iCityWorkingChange;
#endif
#if defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	int m_iCityAutomatonWorkersChange;
#endif
	int m_iBridgeBuildingCount;
#if defined(MOD_BALANCE_CORE_EMBARK_CITY_NO_COST)
	int m_iCityLessEmbarkCost;
	int m_iCityNoEmbarkCost;
#endif
	int m_iWaterWorkCount;
	int m_iRiverTradeCount;
	int m_iBorderObstacleCount;
	int m_iVictoryPoints;
	int m_iEmbarkedExtraMoves;
	int m_iCanEmbarkCount;
	int m_iDefensiveEmbarkCount;
	int m_iEmbarkedAllWaterPassageCount;
	int m_iNumNaturalWondersDiscovered;
	int m_iNumLandmarksBuilt;
	int m_iBestPossibleRoute;
	int m_iNumMinorCivsAttacked;

	bool m_bMapCentering;
	bool m_bHasBrokenPeaceTreaty;
	bool m_bHomeOfUnitedNations;
	bool m_bHasTechForWorldCongress;

	bool m_bBrokenMilitaryPromise;
	bool m_bBrokenExpansionPromise;
	bool m_bBrokenBorderPromise;
	bool m_bBrokenCityStatePromise;
#if defined(MOD_BALANCE_CORE)
	bool m_bCivilianKiller;
#endif

	EraTypes m_eCurrentEra;

	TeamTypes m_eLiberatedByTeam;
	TeamTypes m_eKilledByTeam;

	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiTechShareCount;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiNumTurnsAtWar;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiNumTurnsLockedIntoWar;
	Firaxis::Array< int, NUM_DOMAIN_TYPES > m_aiExtraMoves;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_paiTurnMadePeaceTreatyWithTeam;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiIgnoreWarningCount;

	Firaxis::Array< bool, REALLY_MAX_PLAYERS > m_abHasFoundPlayersTerritory;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abHasMet;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abAtWar;
#if defined(MOD_EVENTS_WAR_AND_PEACE)
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abAggressorPacifier;
#endif
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abPermanentWarPeace;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abEmbassy;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abOpenBorders;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abDefensivePact;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abResearchAgreement;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abTradeAgreement;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abForcePeace;
	Firaxis::Array< int, REALLY_MAX_PLAYERS > m_aiTurnTeamMet;

	typedef
	FAllocArrayType< int,
	                 FAllocArrayType< bool,
	                 FAllocArrayType< bool,
	                 FAllocArrayType< bool,
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
					 FAllocArrayType< bool,
#endif
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
					 FAllocArrayType< int,
	                 FAllocArrayType< int,
	                 FAllocArrayType< int,
#if defined(MOD_API_UNIFIED_YIELDS)
					 FAllocArrayType< int,
	                 FAllocArray2DType< int,
	                 FAllocArray2DType< int,
#endif
	                 FAllocArray2DType< int,
	                 FAllocArray2DType< int,
	                 FAllocArray2DType< int,
#if defined(MOD_DIPLOMACY_CIV4_FEATURES) || defined(MOD_API_UNIFIED_YIELDS)
	                 FAllocBase< 0, 0 >
					 > > > > > > > > > > > > > > > > >
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
					 >
#endif
#if defined(MOD_API_UNIFIED_YIELDS)
					 > > >
#endif
					 CvTeamData;
#else
	                 FAllocBase< 0, 0 > > > > > > > > > > > > > > > > > > CvTeamData;
#endif
	CvTeamData m_BatchData;

	int* m_aiForceTeamVoteEligibilityCount;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int m_iVassalageTradingAllowedCount;
	bool* m_pabTradeTech;

	TeamTypes m_eMaster;
	bool m_bIsVoluntaryVassal;
	int m_iNumTurnsIsVassal;
	int m_iNumCitiesWhenVassalMade;
	int m_iTotalPopulationWhenVassalMade;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiNumTurnsSinceVassalEnded;

	// Only major civs can be taxed
	Firaxis::Array< int, MAX_MAJOR_CIVS > m_aiNumTurnsSinceVassalTaxSet;
	Firaxis::Array< int, MAX_MAJOR_CIVS > m_aiVassalTax;
#endif

	bool* m_abCanLaunch;
	bool* m_abVictoryAchieved;
	bool* m_abSmallAwardAchieved;

	int* m_paiRouteChange;
	int* m_paiBuildTimeChange;

	int* m_paiProjectCount;
	int* m_paiProjectDefaultArtTypes;
	int* m_paiProjectMaking;

	int* m_paiUnitClassCount;
	int* m_paiBuildingClassCount;
	int* m_paiObsoleteBuildingCount;
	int* m_paiTerrainTradeCount;
	int* m_aiVictoryCountdown;

#if defined(MOD_API_UNIFIED_YIELDS)
	int* m_paiTradeRouteDomainExtraRange;
	int** m_ppaaiFeatureYieldChange;
	int** m_ppaaiTerrainYieldChange;
#endif

	int** m_ppaaiImprovementYieldChange;
	int** m_ppaaiImprovementNoFreshWaterYieldChange;
	int** m_ppaaiImprovementFreshWaterYieldChange;

	std::vector<int>* m_pavProjectArtTypes;
	std::vector<ResourceTypes> m_aeRevealedResources;

	CvTeamTechs* m_pTeamTechs;

	void updateTechShare(TechTypes eTech);
	void updateTechShare();

	void testCircumnavigated();
#if defined(MOD_BALANCE_CORE)
	void processTech(TechTypes eTech, int iChange, bool bNoBonus = false);
#else
	void processTech(TechTypes eTech, int iChange);
#endif
	void cancelDefensivePacts();
	void announceTechToPlayers(TechTypes eIndex, bool bPartial = false);

	void DoNowAtWarOrPeace(TeamTypes eTeam, bool bWar);
#if defined(MOD_EVENTS_WAR_AND_PEACE)
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	void DoDeclareWar(PlayerTypes eOriginatingPlayer, bool bAggressor, TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact = false);
#else
	void DoDeclareWar(PlayerTypes eOriginatingPlayer, bool bAggressor, TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact = false);
#endif
#else
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	void DoDeclareWar(TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact = false, bool bVassal = false);
#else
	void DoDeclareWar(TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact = false);
#endif
#endif
#if defined(MOD_EVENTS_WAR_AND_PEACE)
	void DoMakePeace(PlayerTypes eOriginatingPlayer, bool bPacifier, TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification = false);
#else
	void DoMakePeace(TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification = false);
#endif
};

// helper for accessing static functions
#define GET_TEAM CvTeam::getTeam

#endif

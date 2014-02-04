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

public:
	CvTeam();
	~CvTeam();

	// inlined for performance reasons, only in the dll
	static CvTeam& getTeam(TeamTypes eTeam)
	{
		CvAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
		CvAssertMsg(eTeam < MAX_TEAMS, "eTeam is not assigned a valid value");
		return m_aTeams[eTeam];
	}

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
	bool canDeclareWar(TeamTypes eTeam) const;
	void declareWar(TeamTypes eTeam, bool bDefensivePact = false);
	void makePeace(TeamTypes eTeam, bool bBumpUnits = true, bool bSuppressNotification = false);

	int GetTurnMadePeaceTreatyWithTeam(TeamTypes eTeam) const;
	void SetTurnMadePeaceTreatyWithTeam(TeamTypes eTeam, int iNewValue);
	bool IsHasBrokenPeaceTreaty() const;
	void SetHasBrokenPeaceTreaty(bool bValue);

	void meet(TeamTypes eTeam, bool bSuppressMessages);

	int getPower() const;
	int getDefensivePower() const;
	int getEnemyPower() const;
	int getNumNukeUnits() const;

	TeamTypes GetTeamVotingForInDiplo() const;
	int GetProjectedVotesFromMinorAllies() const;
	int GetProjectedVotesFromLiberatedMinors() const;
	int GetProjectedVotesFromCivs() const;
	int GetTotalProjectedVotes() const;
	int GetTotalSecuredVotes() const; // DEPRECATED, use GetTotalProjectedVotes instead

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

	PlayerTypes getLeaderID() const;
	PlayerTypes getSecretaryID() const;
	HandicapTypes getHandicapType() const;
	CvString getName() const;
	CvString getNameKey() const;

	int getNumMembers() const;
	void changeNumMembers(int iChange);

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

	int getBridgeBuildingCount() const;
	bool isBridgeBuilding() const;
	void changeBridgeBuildingCount(int iChange);

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

	bool isHasMet(TeamTypes eIndex) const;
	void makeHasMet(TeamTypes eIndex, bool bSuppressMessages);

	int GetTurnsSinceMeetingTeam(TeamTypes eTeam) const;
	int GetTurnTeamMet(TeamTypes eTeam) const;
	void SetTurnTeamMet(TeamTypes eTeam, int iTurn);

	bool IsHasFoundPlayersTerritory(PlayerTypes ePlayer) const;
	bool SetHasFoundPlayersTerritory(PlayerTypes ePlayer, bool bValue);

	bool isAtWar(TeamTypes eIndex) const;
	void setAtWar(TeamTypes eIndex, bool bNewValue);
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

	bool IsAllowsOpenBordersToTeam(TeamTypes eIndex) const;
	void SetAllowsOpenBordersToTeam(TeamTypes eIndex, bool bNewValue);

	bool IsHasDefensivePact(TeamTypes eIndex) const;
	void SetHasDefensivePact(TeamTypes eIndex, bool bNewValue);

	int GetTotalNumResearchAgreements() const;
	bool IsHasResearchAgreement(TeamTypes eIndex) const;
	void SetHasResearchAgreement(TeamTypes eIndex, bool bNewValue);

	bool IsHasTradeAgreement(TeamTypes eIndex) const;
	void SetHasTradeAgreement(TeamTypes eIndex, bool bNewValue);

	bool isForcePeace(TeamTypes eIndex) const;
	void setForcePeace(TeamTypes eIndex, bool bNewValue);

	int getRouteChange(RouteTypes eIndex) const;
	void changeRouteChange(RouteTypes eIndex, int iChange);

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

	void setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce);
	CvTeamTechs* GetTeamTechs() const;

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getImprovementNoFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementNoFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getImprovementFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementFreshWaterYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	bool isFriendlyTerritory(TeamTypes eTeam) const;
	bool isAtWarWithHumans() const;
	bool isSimultaneousTurns() const;

	bool IsHomeOfUnitedNations() const;
	void SetHomeOfUnitedNations(bool bValue);

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

	EraTypes GetCurrentEra() const;
	void SetCurrentEra(EraTypes eNewValue);

	int countNumHumanGameTurnActive() const;
	void setTurnActive(bool bNewValue, bool bTurn = true);
	bool isTurnActive() const;

	void setDynamicTurnsSimultMode(bool simultaneousTurns);

	// Wrapper for giving Players on this Team a notification message
	void AddNotification(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX = -1, int iY = -1, int iGameDataIndex = -1, int iExtraGameData = -1);

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

protected:

	TeamTypes m_eID;

	static CvTeam* m_aTeams;

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
	int m_iBridgeBuildingCount;
	int m_iWaterWorkCount;
	int m_iRiverTradeCount;
	int m_iBorderObstacleCount;
	int m_iVictoryPoints;
	int m_iEmbarkedExtraMoves;
	int m_iCanEmbarkCount;
	int m_iDefensiveEmbarkCount;
	int m_iEmbarkedAllWaterPassageCount;
	int m_iNumNaturalWondersDiscovered;
	int m_iBestPossibleRoute;
	int m_iNumMinorCivsAttacked;

	bool m_bMapCentering;
	bool m_bHasBrokenPeaceTreaty;
	bool m_bHomeOfUnitedNations;

	bool m_bBrokenMilitaryPromise;
	bool m_bBrokenExpansionPromise;
	bool m_bBrokenBorderPromise;
	bool m_bBrokenCityStatePromise;

	EraTypes m_eCurrentEra;

	TeamTypes m_eLiberatedByTeam;
	TeamTypes m_eKilledByTeam;

	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiTechShareCount;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiNumTurnsAtWar;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_aiNumTurnsLockedIntoWar;
	Firaxis::Array< int, NUM_DOMAIN_TYPES > m_aiExtraMoves;
	Firaxis::Array< int, REALLY_MAX_TEAMS > m_paiTurnMadePeaceTreatyWithTeam;

	Firaxis::Array< bool, REALLY_MAX_PLAYERS > m_abHasFoundPlayersTerritory;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abHasMet;
	Firaxis::Array< bool, REALLY_MAX_TEAMS > m_abAtWar;
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
	                 FAllocArray2DType< int,
	                 FAllocArray2DType< int,
	                 FAllocArray2DType< int,
	                 FAllocBase< 0, 0 > > > > > > > > > > > > > > > > > > CvTeamData;
	CvTeamData m_BatchData;

	int* m_aiForceTeamVoteEligibilityCount;

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

	int** m_ppaaiImprovementYieldChange;
	int** m_ppaaiImprovementNoFreshWaterYieldChange;
	int** m_ppaaiImprovementFreshWaterYieldChange;

	std::vector<int>* m_pavProjectArtTypes;
	std::vector<ResourceTypes> m_aeRevealedResources;

	CvTeamTechs* m_pTeamTechs;

	void updateTechShare(TechTypes eTech);
	void updateTechShare();

	void testCircumnavigated();

	void processTech(TechTypes eTech, int iChange);

	void cancelDefensivePacts();
	void announceTechToPlayers(TechTypes eIndex, bool bPartial = false);

	void DoNowAtWarOrPeace(TeamTypes eTeam, bool bWar);
	void DoDeclareWar(TeamTypes eTeam, bool bDefensivePact, bool bMinorAllyPact = false);
	void DoMakePeace(TeamTypes eTeam, bool bBumpUnits, bool bSuppressNotification = false);
};

// helper for accessing static functions
#define GET_TEAM CvTeam::getTeam

#endif

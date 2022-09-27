/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvWeightedVector.h"
#include "CvBeliefClasses.h"

#ifndef CIV5_RELIGION_CLASSES_H
#define CIV5_RELIGION_CLASSES_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvReligionEntry
//!  \brief		A single entry in the religion XML file
//
//!  Key Attributes:
//!  - Populated from XML\Religions\CIV5Religions.xml
//!  - Array of these contained in CvReligionXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionEntry: public CvBaseInfo
{
public:
	CvReligionEntry(void);
	~CvReligionEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);
	
	CvString GetIconString() const;

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	bool IsLocalReligion() const;
#endif

protected:
	CvString m_strIconString;

private:
	CvReligionEntry(const CvReligionEntry&);
	CvReligionEntry& operator=(const CvReligionEntry&);

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	int m_iLocalReligion;
#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvReligionXMLEntries
//!  \brief		Game-wide information about religions
//
//! Key Attributes:
//! - Populated from XML\Religions\CIV5Religions.xml
//! - Contains an array of CvReligionEntry from the above XML file
//! - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionXMLEntries
{
public:
	CvReligionXMLEntries(void);
	~CvReligionXMLEntries(void);

	// Accessor functions
	std::vector<CvReligionEntry*>& GetReligionEntries();
	int GetNumReligions();
	CvReligionEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvReligionEntry*> m_paReligionEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvReligion
//!  \brief All the information about a single religion
//
//!  Key Attributes:
//!  - Stores the founder and holy city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligion
{
public:
	CvReligion();
	CvReligion(ReligionTypes eReligion, PlayerTypes eFounder, CvCity* pHolyCity, bool bPantheon);

	template<typename Religion, typename Visitor>
	static void Serialize(Religion& religion, Visitor& visitor);

	CvString GetName() const;
	CvCity* GetHolyCity() const;

	// Public data
	ReligionTypes m_eReligion;
	PlayerTypes m_eFounder;
	int m_iHolyCityX;
	int m_iHolyCityY;
	int m_iTurnFounded;
	bool m_bPantheon;
	bool m_bEnhanced;
#if defined(MOD_BALANCE_CORE)
	bool m_bReformed;
#endif
	char m_szCustomName[128];
	CvReligionBeliefs m_Beliefs;
};

FDataStream& operator>>(FDataStream&, CvReligion&);
FDataStream& operator<<(FDataStream&, const CvReligion&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvReligionInCity
//!  \brief All the information about a single religion inside a city
//
//!  Key Attributes:
//!  - Stores the number of followers, pressure level, is it Holy City
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionInCity
{
public:
	CvReligionInCity();
	CvReligionInCity(ReligionTypes eReligion, int iFollowers, int iPressure);

	template<typename ReligionInCity, typename Visitor>
	static void Serialize(ReligionInCity& religionInCity, Visitor& visitor);

	// Public data
	ReligionTypes m_eReligion;
	int m_iFollowers;
	int m_iPressure;
	int m_iNumTradeRoutesApplyingPressure;
};

typedef vector<CvReligion> ReligionList;
typedef vector<CvReligionInCity> ReligionInCityList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvGameReligions
//!  \brief		All the information about religions founded and active in the game
//
//!  Key Attributes:
//!  - Core data in this class is a list of CvReligions
//!  - This object is created inside the CvGame object and accessed through CvGame
//!  - Provides convenience functions to the other game subsystems to quickly summarize
//!    information on the religions in place
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameReligions
{
public:
	CvGameReligions(void);
	~CvGameReligions(void);

	enum CLOSED_ENUM FOUNDING_RESULT
	{
	    FOUNDING_OK = 0,
	    FOUNDING_BELIEF_IN_USE,
	    FOUNDING_RELIGION_IN_USE,
	    FOUNDING_NOT_ENOUGH_FAITH,
	    FOUNDING_NO_RELIGIONS_AVAILABLE,
	    FOUNDING_INVALID_PLAYER,
	    FOUNDING_PLAYER_ALREADY_CREATED_RELIGION,
	    FOUNDING_PLAYER_ALREADY_CREATED_PANTHEON,
	    FOUNDING_NAME_IN_USE,
	    FOUNDING_RELIGION_ENHANCED,
		FOUNDING_NO_BELIEFS_AVAILABLE,
	};

	void Init();

	template<typename GameReligions, typename Visitor>
	static void Serialize(GameReligions& gameReligions, Visitor& visitor);

	// Functions invoked each game turn
	void DoTurn();
	void SpreadReligion();
	void SpreadReligionToOneCity(CvCity* pCity);
	bool IsCityConnectedToCity(ReligionTypes eReligion, CvCity* pLoopCity, CvCity* pCity, bool& bConnectedWithTrade, int& iRelativeDistancePercent);
	bool IsValidTarget(ReligionTypes eReligion, CvCity* pFromCity, CvCity* pToCity);
	// Functions invoked each player turn
	EraTypes GetFaithPurchaseGreatPeopleEra(CvPlayer* pPlayer);
	void DoPlayerTurn(CvPlayer& kPlayer);
	FOUNDING_RESULT CanCreatePantheon(PlayerTypes ePlayer, bool bCheckFaithTotal);
	FOUNDING_RESULT CanFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity);
	FOUNDING_RESULT CanEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2);

	// Functions for religious actions
	ReligionTypes GetReligionToFound(PlayerTypes ePlayer);
	void FoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief);
	void FoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity);
	void EnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2=NO_BELIEF, bool bNotify=true, bool bSetAsEnhanced = true);
	void AddReformationBelief(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1);

	ReligionTypes GetHolyCityReligion(const CvCity* pkTestCity) const;
	void SetHolyCity(ReligionTypes eReligion, const CvCity* pkHolyCity);
	void SetFounder(ReligionTypes eReligion, PlayerTypes eFounder);

	void SetFoundYear(ReligionTypes eReligion, int iValue);
	int GetFoundYear(ReligionTypes eReligion);

	void UpdateAllCitiesThisReligion(ReligionTypes eReligion);

	// General religion information functions
	const CvReligion* GetReligion(ReligionTypes eReligion, PlayerTypes ePlayer) const;
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool IsInSomeReligion(BeliefTypes eBelief, PlayerTypes ePlayer=NO_PLAYER) const;
#else
	bool IsInSomeReligion(BeliefTypes eBelief) const;
#endif

	// Pantheon information functions
	void SetMinimumFaithNextPantheon(int iMinFaith)
	{
		m_iMinimumFaithForNextPantheon = iMinFaith;
	};
	int GetMinimumFaithNextPantheon() const
	{
		return m_iMinimumFaithForNextPantheon;
	};
	BeliefTypes GetBeliefInPantheon(PlayerTypes ePlayer) const;
	bool HasCreatedPantheon(PlayerTypes ePlayer) const;
	int GetNumPantheonsCreated() const;
	int GetNumPantheonsPossible(bool bExcludeUnique = false) const;
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
	std::vector<BeliefTypes> GetAvailablePantheonBeliefs(PlayerTypes ePlayer=NO_PLAYER);
#else
	std::vector<BeliefTypes> GetAvailablePantheonBeliefs();
#endif
#if defined(MOD_TRAITS_ANY_BELIEF)
	bool IsPantheonBeliefAvailable(BeliefTypes eBelief, PlayerTypes ePlayer=NO_PLAYER);
#else
	bool IsPantheonBeliefAvailable(BeliefTypes eBelief);
#endif

	// Main religion information functions
#if defined(MOD_BALANCE_CORE)
	int GetNumFollowers(ReligionTypes eReligion, PlayerTypes ePlayer = NO_PLAYER) const;
#else
	int GetNumFollowers(ReligionTypes eReligion) const;
#endif
	int GetNumCitiesFollowing(ReligionTypes eReligion) const;
	int GetNumDomesticCitiesFollowing(ReligionTypes eReligion, PlayerTypes ePlayer) const;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	bool HasCreatedReligion(PlayerTypes ePlayer, bool bIgnoreLocal = false) const;
#else
	bool HasCreatedReligion(PlayerTypes ePlayer) const;
#endif
	bool HasAddedReformationBelief(PlayerTypes ePlayer) const;
	bool IsCityStateFriendOfReligionFounder(ReligionTypes eReligion, PlayerTypes ePlayer);
	ReligionTypes GetReligionCreatedByPlayer(PlayerTypes ePlayer, bool bIncludePantheon = false) const;
	ReligionTypes GetPantheonCreatedByPlayer(PlayerTypes ePlayer) const;
	ReligionTypes GetOriginalReligionCreatedByPlayer(PlayerTypes ePlayer) const;

#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	int GetNumReligionsFounded(bool bIgnoreLocal = false) const;
#else
	int GetNumReligionsFounded() const;
#endif
	int GetNumReligionsEnhanced() const;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	int GetNumReligionsStillToFound(bool bIgnoreLocal = false, PlayerTypes ePlayer = NO_PLAYER) const;
#else
	int GetNumReligionsStillToFound() const;
#endif
#if defined(MOD_EVENTS_ACQUIRE_BELIEFS) || defined(MOD_TRAITS_ANY_BELIEF)
	std::vector<BeliefTypes> GetAvailableFounderBeliefs(PlayerTypes ePlayer=NO_PLAYER, ReligionTypes eReligion=NO_RELIGION);
	std::vector<BeliefTypes> GetAvailableFollowerBeliefs(PlayerTypes ePlayer=NO_PLAYER, ReligionTypes eReligion=NO_RELIGION);
	std::vector<BeliefTypes> GetAvailableEnhancerBeliefs(PlayerTypes ePlayer=NO_PLAYER, ReligionTypes eReligion=NO_RELIGION);
	std::vector<BeliefTypes> GetAvailableBonusBeliefs(PlayerTypes ePlayer=NO_PLAYER, ReligionTypes eReligion=NO_RELIGION);
	std::vector<BeliefTypes> GetAvailableReformationBeliefs(PlayerTypes ePlayer=NO_PLAYER, ReligionTypes eReligion=NO_RELIGION);
#else
	std::vector<BeliefTypes> GetAvailableFounderBeliefs();
	std::vector<BeliefTypes> GetAvailableFollowerBeliefs();
	std::vector<BeliefTypes> GetAvailableEnhancerBeliefs();
	std::vector<BeliefTypes> GetAvailableBonusBeliefs();
	std::vector<BeliefTypes> GetAvailableReformationBeliefs();
#endif

	int GetAdjacentCityReligiousPressure(ReligionTypes eReligion, CvCity *pFromCity, CvCity *pToCity, int& iNumTradeRoutesInfluencing, bool bActualValue, 
		bool bPretendTradeConnection, bool bConnectedWithTrade, int iRelativeDistancePercent);

	// Great Prophet/Person information functions
	int GetFaithGreatProphetNumber(int iNum) const;
	int GetFaithGreatPersonNumber(int iNum) const;

	// Complex belief convenience functions
	int GetBeliefYieldForKill(YieldTypes eYield, int iX, int iY, PlayerTypes eWinningPlayer);

	static void NotifyPlayer(PlayerTypes ePlayer, CvGameReligions::FOUNDING_RESULT eResult);

	// Public logging functions
	CvString GetLogFileName() const;
	void LogReligionMessage(CvString& strMsg);

	ReligionList m_CurrentReligions;

private:

	bool HasBeenFounded(ReligionTypes eReligion);
	bool IsPreferredByCivInGame(ReligionTypes eReligion);

	// Functions invoked each player turn
	bool CheckSpawnGreatProphet(CvPlayer& kPlayer);

	int m_iMinimumFaithForNextPantheon;

	friend FDataStream& operator>>(FDataStream&, CvGameReligions&);
	friend FDataStream& operator<<(FDataStream&, const CvGameReligions&);
};

FDataStream& operator>>(FDataStream&, CvGameReligions&);
FDataStream& operator<<(FDataStream&, const CvGameReligions&);

enum CvReligiousFollowChangeReason
{
    FOLLOWER_CHANGE_POP_CHANGE,
    FOLLOWER_CHANGE_HOLY_CITY,
    FOLLOWER_CHANGE_ADJACENT_PRESSURE,
    FOLLOWER_CHANGE_RELIGION_FOUNDED,
    FOLLOWER_CHANGE_PANTHEON_FOUNDED,
    FOLLOWER_CHANGE_CONQUEST,
    FOLLOWER_CHANGE_MISSIONARY,
    FOLLOWER_CHANGE_PROPHET,
    FOLLOWER_CHANGE_REMOVE_HERESY,
	FOLLOWER_CHANGE_SCRIPTED_CONVERSION,
	FOLLOWER_CHANGE_SPY_PRESSURE,
	FOLLOWER_CHANGE_INSTANT_YIELD,
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	FOLLOWER_CHANGE_ADOPT_FULLY,
#endif
#if defined(MOD_BALANCE_CORE_PANTHEON_RESET_FOUND)
	FOLLOWER_CHANGE_PANTHEON_OBSOLETE,
#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvPlayerReligions
//!  \brief		All the information about religious activity for a player
//
//!  Key Attributes:
//!  - Core data in this class is a list of how many great prophets this player has spawned
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerReligions
{
public:
	CvPlayerReligions(void);
	~CvPlayerReligions(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	template<typename PlayerReligions, typename Visitor>
	static void Serialize(PlayerReligions& playerReligions, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Data accessors
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int GetNumProphetsSpawned(bool bExcludeFree) const;
	void ChangeNumProphetsSpawned(int iValue, bool bIsFree);
	int GetCostNextProphet(bool bIncludeBeliefDiscounts, bool bAdjustForSpeedDifficulty, bool bExcludeFree) const;
#else
	int GetNumProphetsSpawned() const;
	void ChangeNumProphetsSpawned(int iValue);
	int GetCostNextProphet(bool bIncludeBeliefDiscounts, bool bAdjustForSpeedDifficulty) const;
#endif
	bool IsFoundingReligion() const
	{
		return m_bFoundingReligion;
	};
	void SetFoundingReligion(bool bNewValue)
	{
		m_bFoundingReligion = bNewValue;
	};

	// State information
	bool HasCreatedPantheon() const;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	bool HasCreatedReligion(bool bIgnoreLocal = false) const;
#else
	bool HasCreatedReligion() const;
#endif
	bool HasAddedReformationBelief() const;
	ReligionTypes GetReligionCreatedByPlayer(bool bIncludePantheon = false) const;
	ReligionTypes GetOriginalReligionCreatedByPlayer() const;

	bool CanAffordNextPurchase();
	void SetFaithAtLastNotify(int iFaith);
	bool CanAffordFaithPurchase(int iMinimumFaith) const;
	bool HasCityWithMajorityReligion(ReligionTypes eReligionToCheck) const;
	bool HasOthersReligionInMostCities(PlayerTypes eOtherPlayer) const;
	bool HasReligionInMostCities(ReligionTypes eReligion) const;
	ReligionTypes GetReligionInMostCities() const;

	bool UpdateStateReligion();
	void SetStateReligionOverride(ReligionTypes eReligion);
	ReligionTypes GetStateReligion(bool bIncludePantheon = true) const;
	ReligionTypes GetOwnedReligion() const;
	int GetNumCitiesWithStateReligion(ReligionTypes eReligion);

	int GetCityStateMinimumInfluence(ReligionTypes eReligion, PlayerTypes ePlayer) const;
	int GetCityStateInfluenceModifier(PlayerTypes ePlayer) const;
	int GetCityStateYieldModifier(PlayerTypes ePlayer) const;
	int GetSpyPressure(PlayerTypes ePlayer) const;
	int GetNumForeignCitiesFollowing(ReligionTypes eReligion) const;
	int GetNumForeignFollowers(bool bAtPeace, ReligionTypes eReligion) const;
	int GetNumDomesticFollowers(ReligionTypes eReligion) const;
	bool ComputeMajority(bool bNotifications = false);

private:
	bool SetStateReligion(ReligionTypes eReligion, bool bOwnsReligion);
	CvPlayer* m_pPlayer;

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int m_iNumFreeProphetsSpawned;
#endif
	int m_iNumProphetsSpawned;
	bool m_bFoundingReligion; //seems to be used to suppress further prophet use before the religion has been customized
	int m_iFaithAtLastNotify;

	ReligionTypes m_eMajorityReligion; //this is the majority religion in at least half of our cities
	ReligionTypes m_eStateReligion; //this is our founded religion > the religion whose holy city we control with the most domestic followers > m_eMajorityReligion
	ReligionTypes m_eStateReligionOverride; //to be set from lua
	bool m_bOwnsStateReligion; //do we own the holy city of our state religion
};

FDataStream& operator>>(FDataStream&, CvPlayerReligions&);
FDataStream& operator<<(FDataStream&, const CvPlayerReligions&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvCityReligions
//!  \brief		All the information about religious activity in a city
//
//!  Key Attributes:
//!  - Core data in this class is a list of how citizens break down between religions
//!  - This object is created inside the CvCity object and accessed through CvCity
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityReligions
{
public:
	CvCityReligions(void);
	~CvCityReligions(void);
	void Init(CvCity* pCity);
	void Uninit();
	void Copy(CvCityReligions* pOldCity);

	template<typename CityReligions, typename Visitor>
	static void Serialize(CityReligions& cityReligions, Visitor& visitor);

	// Data accessors
	int GetNumFollowers(ReligionTypes eReligion);
	int GetNumSimulatedFollowers(ReligionTypes eReligion);
	int GetNumReligionsWithFollowers();
	bool IsReligionInCity();
	bool IsHolyCityForReligion(ReligionTypes eReligion);
	bool IsHolyCityAnyReligion();
#if defined(MOD_BALANCE_CORE)
	ReligionTypes GetReligionForHolyCity();
#endif
	bool IsReligionHereOtherThan(ReligionTypes eReligion, int iMinFollowers = 0);
	bool IsDefendedByOurInquisitor(ReligionTypes eReligion, CvUnit* pIgnoreUnit = NULL);
	bool IsDefendedAgainstSpread(ReligionTypes eReligion);
	bool IsForeignMissionaryNearby(ReligionTypes eReligion);
	ReligionTypes GetReligiousMajority() const;
	ReligionTypes GetSimulatedReligiousMajority();
	ReligionTypes GetReligionByAccumulatedPressure(size_t iIndex) const;
	BeliefTypes GetSecondaryReligionPantheonBelief();
	int GetFollowersOtherReligions(ReligionTypes eReligion, bool bIncludePantheons = false);
	int GetReligiousPressureModifier(ReligionTypes eReligion) const;
	void SetReligiousPressureModifier(ReligionTypes eReligion, int iNewValue);
	void ChangeReligiousPressureModifier(ReligionTypes eReligion, int iNewValue);
	int GetTotalAccumulatedPressure(bool bIncludePantheon) const;
	int GetPressureAccumulated(ReligionTypes eReligion) const;
	int GetPressurePerTurn(ReligionTypes eReligion, int* piNumSourceCities = 0);
	int GetNumTradeRouteConnections (ReligionTypes eReligion);
	bool WouldExertTradeRoutePressureToward (CvCity* pTargetCity, ReligionTypes& eReligion, int& iAmount);

	// Routines to update religious status of citizens
	void DoPopulationChange(int iChange);
	void DoReligionFounded(ReligionTypes eReligion);
	void AddMissionarySpread(ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer);
	void AddProphetSpread(ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer);
	void AddReligiousPressure(CvReligiousFollowChangeReason eReason, ReligionTypes eReligion, int iPressureChange, PlayerTypes eResponsiblePlayer=NO_PLAYER);
	void ErodeOtherReligiousPressure(CvReligiousFollowChangeReason eReason, ReligionTypes eExemptedReligion, int iErosionPercent, bool bAllowRetention, bool bLeaveAtheists, PlayerTypes eResponsiblePlayer=NO_PLAYER);

	void SimulateProphetSpread(ReligionTypes eReligion, int iPressure);
	void SimulateReligiousPressure(ReligionTypes eReligion, int iPressure);
	void ConvertPercentFollowers(ReligionTypes eToReligion, ReligionTypes eFromReligion, int iPercent);
#if defined(MOD_BALANCE_CORE)
	void ConvertPercentAllOtherFollowers(ReligionTypes eToReligion, int iPercent);
	void ConvertNumberAllOtherFollowers(ReligionTypes eToReligion, int iPop);
#endif
	void AddHolyCityPressure();
	void AddSpyPressure(ReligionTypes eReligion, int iBasePressure);
	void AdoptReligionFully(ReligionTypes eReligion);
	void RemoveFormerPantheon();
	void RemoveOtherReligions(ReligionTypes eReligion, PlayerTypes eResponsiblePlayer);
	void IncrementNumTradeRouteConnections(ReligionTypes eReligion, int iNum);

	void UpdateNumTradeRouteConnections(CvCity* pOtherCity);

	// Routines to precompute results of possible religion spreads
	int GetNumFollowersAfterSpread(ReligionTypes eReligion, int iConversionStrength);
	int GetNumFollowersAfterProphetSpread(ReligionTypes eReligion, int iConversionStrength);
	ReligionTypes GetMajorityReligionAfterSpread(ReligionTypes eReligion, int iConversionStrength);
	ReligionTypes GetMajorityReligionAfterProphetSpread(ReligionTypes eReligion, int iConversionStrength);

	void ResetNumTradeRoutePressure();

	void RecomputeFollowers(CvReligiousFollowChangeReason eReason, PlayerTypes eResponsibleParty=NO_PLAYER);
	const CvReligion* GetMajorityReligion(); //performance optimized

	void LogPressureChange(CvReligiousFollowChangeReason eReason, ReligionTypes eReligion, int iPressureChange, int iAccPressure, PlayerTypes eResponsiblePlayer);
protected:
	bool ComputeReligiousMajority(bool bNotifications = false);
	void SimulateFollowers();
	void CityConvertsReligion(ReligionTypes eMajority, ReligionTypes eOldMajority, PlayerTypes eResponsibleParty);
#if defined(MOD_ISKA_PANTHEONS)
	void CityConvertsPantheon();
#endif
	void LogFollowersChange(CvReligiousFollowChangeReason eReason);

	CvCity* m_pCity; //not serialized
	ReligionInCityList m_SimulatedStatus; //not serialized

	ReligionInCityList m_ReligionStatus;
	ReligionTypes m_eMajorityCityReligion;

	const CvReligion* m_pMajorityReligionCached; //for faster access, not serialized

	friend FDataStream& operator>>(FDataStream&, CvCityReligions&);
	friend FDataStream& operator<<(FDataStream&, const CvCityReligions&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvReligionAI
//!  \brief		Player level AI to manage a civ's efforts with religion
//
//!  Key Attributes:
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionAI
{
public:
	CvReligionAI(void);
	~CvReligionAI(void);
	void Init(CvBeliefXMLEntries* pBeliefs, CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	template<typename ReligionAI, typename Visitor>
	static void Serialize(ReligionAI& religionAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void DoTurn();

#if defined(MOD_EVENTS_ACQUIRE_BELIEFS)
	BeliefTypes ChoosePantheonBelief(PlayerTypes ePlayer/*=NO_PLAYER*/);
	BeliefTypes ChooseFounderBelief(PlayerTypes ePlayer/*=NO_PLAYER*/, ReligionTypes eReligion/*=NO_RELIGION*/);
	BeliefTypes ChooseFollowerBelief(PlayerTypes ePlayer/*=NO_PLAYER*/, ReligionTypes eReligion/*=NO_RELIGION*/);
	BeliefTypes ChooseEnhancerBelief(PlayerTypes ePlayer/*=NO_PLAYER*/, ReligionTypes eReligion/*=NO_RELIGION*/);
	BeliefTypes ChooseBonusBelief(PlayerTypes ePlayer/*=NO_PLAYER*/, ReligionTypes eReligion/*=NO_RELIGION*/, int iExcludeBelief1, int iExcludeBelief2, int iExcludeBelief3);
	BeliefTypes ChooseReformationBelief(PlayerTypes ePlayer/*=NO_PLAYER*/, ReligionTypes eReligion/*=NO_RELIGION*/);
#else
	BeliefTypes ChoosePantheonBelief();
	BeliefTypes ChooseFounderBelief();
	BeliefTypes ChooseFollowerBelief();
	BeliefTypes ChooseEnhancerBelief();
	BeliefTypes ChooseBonusBelief(int iExcludeBelief1, int iExcludeBelief2, int iExcludeBelief3);
	BeliefTypes ChooseReformationBelief();
#endif
#if defined(MOD_BALANCE_CORE)
	int GetNumCitiesWithReligionCalculator(ReligionTypes eReligion = NO_RELIGION, bool bForPantheon = false) const;
#endif
	CvCity* ChooseMissionaryTargetCity(CvUnit* pUnit, const vector<pair<int,int>>& vIgnoreTargets, int* piTurns = NULL) const;
	CvCity* ChooseInquisitorTargetCity(CvUnit* pUnit, const vector<pair<int,int>>& vIgnoreTargets, int* piTurns = NULL) const;
	CvCity *ChooseProphetConversionCity(CvUnit* pUnit = NULL, int* piTurns = NULL) const;
	ReligionTypes GetReligionToSpread(bool bConsiderForeign) const;
	ReligionTypes GetFavoriteForeignReligion(bool bForInternalSpread) const;

private:
#if defined(MOD_BALANCE_CORE)
	bool DoFaithPurchasesInCities(CvCity* pCity);
	bool DoReligionDefenseInCities();
#endif
	int GetSpreadScore() const;
	bool DoFaithPurchases();
	bool BuyMissionaryOrInquisitor(ReligionTypes eReligion);
	bool BuyMissionary(ReligionTypes eReligion);
	bool BuyInquisitor(ReligionTypes eReligion);
	bool BuyGreatPerson(UnitTypes eUnit, ReligionTypes eReligion = NO_RELIGION);
	bool BuyFaithBuilding(CvCity* pCity, BuildingTypes eBuilding);
#if defined(MOD_BALANCE_CORE_BELIEFS)
	bool BuyAnyAvailableNonFaithUnit();
#endif
	bool BuyAnyAvailableNonFaithBuilding();
	bool BuyAnyAvailableFaithBuilding();

	int ScoreBelief(CvBeliefEntry* pEntry, bool bForBonus = false, bool bConsiderFutureTech = true) const;
	int ScoreBeliefAtPlot(CvBeliefEntry* pEntry, CvPlot* pPlot, bool bConsiderFutureTech) const;
	int ScoreBeliefAtCity(CvBeliefEntry* pEntry, CvCity* pCity) const;
	int ScoreBeliefForPlayer(CvBeliefEntry* pEntry, bool bReturnConquest = false, bool bReturnCulture = false, bool bReturnScience = false, bool bReturnDiplo = false) const;
	int GetValidPlotYield(CvBeliefEntry* pEntry, CvPlot* pPlot, YieldTypes eYield, bool bConsiderFutureTech) const;

	int ScoreCityForMissionary(CvCity* pCity, CvUnit* pUnit, ReligionTypes eReligion) const;
	int ScoreCityForInquisitorOffensive(CvCity* pCity, CvUnit* pUnit, ReligionTypes eReligion) const;
	int ScoreCityForInquisitorDefensive(CvCity* pCity, CvUnit* pUnit, ReligionTypes eReligion) const;

	bool AreAllOurCitiesConverted(ReligionTypes eReligion, bool bIncludePuppets) const;
	bool AreAllOurCitiesHaveFaithBuilding(ReligionTypes eReligion, bool bIncludePuppets) const;
	bool HaveNearbyConversionTarget(ReligionTypes eReligion, bool bCanIncludeReligionStarter, bool bHeathensOnly) const;
	bool HaveEnoughInquisitors(ReligionTypes eReligion) const;
	bool CanHaveInquisitors(ReligionTypes eReligion) const;
	BuildingClassTypes FaithBuildingAvailable(ReligionTypes eReligion, CvCity* pCity = NULL, bool bEvaluateBestPurchase = false) const;
#if defined(MOD_BALANCE_CORE_BELIEFS)
	bool IsProphetGainRateAcceptable();
	bool CanBuyNonFaithUnit() const;
#endif
	bool CanBuyNonFaithBuilding() const;
	UnitTypes GetDesiredFaithGreatPerson() const;
	void LogBeliefChoices(CvWeightedVector<BeliefTypes>& beliefChoices, int iChoice);

	CvBeliefXMLEntries* m_pBeliefs;
	CvPlayer* m_pPlayer;

	mutable ReligionTypes m_eReligionToSpread; //not serialized
	mutable int m_iTurnReligionToSpreadUpdated; //not serialized
};

FDataStream& operator>>(FDataStream&, CvReligionAI&);
FDataStream& operator<<(FDataStream&, const CvReligionAI&);

namespace CvReligionAIHelpers
{
	CvCity *GetBestCityFaithUnitPurchase(CvPlayer &kPlayer, UnitTypes eUnit, ReligionTypes eReligion);
	CvCity *GetBestCityFaithBuildingPurchase(CvPlayer &kPlayer, BuildingTypes eBuilding, ReligionTypes eReligion);
	bool DoesUnitPassFaithPurchaseCheck(CvPlayer &kPlayer, UnitTypes eUnit);
	bool ShouldRemoveHeresy(CvCity* pCity, ReligionTypes eTrueReligion);
}

#endif //CIV5_RELIGION_CLASSES_H
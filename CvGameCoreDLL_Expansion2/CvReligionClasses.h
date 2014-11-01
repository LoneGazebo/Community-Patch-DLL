/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvWeightedVector.h"

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

	CvString GetName() const;

	// Public data
	ReligionTypes m_eReligion;
	PlayerTypes m_eFounder;
	int m_iHolyCityX;
	int m_iHolyCityY;
	int m_iTurnFounded;
	bool m_bPantheon;
	bool m_bEnhanced;
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
	CvReligionInCity(ReligionTypes eReligion, bool bFoundedHere, int iFollowers, int iPressure);

	// Public data
	ReligionTypes m_eReligion;
	bool m_bFoundedHere;
	int m_iFollowers;
	int m_iPressure;
	int m_iNumTradeRoutesApplyingPressure;
	int m_iTemp;
};

typedef FStaticVector<CvReligion, 16, false, c_eCiv5GameplayDLL > ReligionList;
typedef FStaticVector<CvReligionInCity, 8, false, c_eCiv5GameplayDLL > ReligionInCityList;

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

	enum FOUNDING_RESULT
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

	// Functions invoked each game turn
	void DoTurn();
	void SpreadReligion();
	void SpreadReligionToOneCity(CvCity* pCity);

	// Functions invoked each player turn
	void DoPlayerTurn(CvPlayer& kPlayer);
	FOUNDING_RESULT CanCreatePantheon(PlayerTypes ePlayer, bool bCheckFaithTotal);
	FOUNDING_RESULT CanFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity);
	FOUNDING_RESULT CanEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2);

	// Functions for religious actions
	ReligionTypes GetReligionToFound(PlayerTypes ePlayer);
	void FoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief);
	void FoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, CvCity* pkHolyCity);
#if defined(MOD_API_RELIGION)
	void EnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2=NO_BELIEF, bool bNotify=true);
#else
	void EnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1, BeliefTypes eBelief2);
#endif
	void AddReformationBelief(PlayerTypes ePlayer, ReligionTypes eReligion, BeliefTypes eBelief1);
	void SetHolyCity(ReligionTypes eReligion, CvCity* pkHolyCity);
	void SetFounder(ReligionTypes eReligion, PlayerTypes eFounder);
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
	int GetNumFollowers(ReligionTypes eReligion) const;
	int GetNumCitiesFollowing(ReligionTypes eReligion) const;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	bool HasCreatedReligion(PlayerTypes ePlayer, bool bIgnoreLocal = false) const;
#else
	bool HasCreatedReligion(PlayerTypes ePlayer) const;
#endif
	bool HasAddedReformationBelief(PlayerTypes ePlayer) const;
	bool IsEligibleForFounderBenefits(ReligionTypes eReligion, PlayerTypes ePlayer) const;
	bool IsCityStateFriendOfReligionFounder(ReligionTypes eReligion, PlayerTypes ePlayer);
	ReligionTypes GetReligionCreatedByPlayer(PlayerTypes ePlayer) const;
	ReligionTypes GetFounderBenefitsReligion(PlayerTypes ePlayer) const;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	int GetNumReligionsFounded(bool bIgnoreLocal = false) const;
#else
	int GetNumReligionsFounded() const;
#endif
	int GetNumReligionsEnhanced() const;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
	int GetNumReligionsStillToFound(bool bIgnoreLocal = false) const;
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

	int GetAdjacentCityReligiousPressure (ReligionTypes eReligion, CvCity *pFromCity, CvCity *pToCity, int& iNumTradeRoutesInfluencing, bool bPretendTradeConnection);

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
#if defined(MOD_GLOBAL_RELIGIOUS_SETTLERS)
	FOLLOWER_CHANGE_ADOPT_FULLY,
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
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

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
#if defined(MOD_API_EXTENSIONS)
	ReligionTypes GetReligionCreatedByPlayer(bool bIncludePantheon = false) const;
#else
	ReligionTypes GetReligionCreatedByPlayer() const;
#endif
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	bool CanAffordNextPurchase();
	void SetFaithAtLastNotify(int iFaith);
	bool CanAffordFaithPurchase(int iMinimumFaith) const;
#else
	bool CanAffordFaithPurchase() const;
#endif
	bool HasReligiousCity() const;
	bool HasOthersReligionInMostCities(PlayerTypes eOtherPlayer) const;
	bool HasReligionInMostCities(ReligionTypes eReligion) const;
	ReligionTypes GetReligionInMostCities() const;
	int GetCityStateMinimumInfluence(ReligionTypes eReligion) const;
	int GetCityStateInfluenceModifier() const;
	int GetSpyPressure() const;
	int GetNumForeignCitiesFollowing() const;
	int GetNumForeignFollowers(bool bAtPeace) const;

private:
	CvPlayer* m_pPlayer;

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
	int m_iNumFreeProphetsSpawned;
#endif
	int m_iNumProphetsSpawned;
	int m_bFoundingReligion;
#if defined(MOD_RELIGION_RECURRING_PURCHASE_NOTIFIY)
	int m_iFaithAtLastNotify;
#endif
};

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

	// Data accessors
	int GetNumFollowers(ReligionTypes eReligion);
	int GetNumSimulatedFollowers(ReligionTypes eReligion);
	int GetNumReligionsWithFollowers();
	bool IsReligionInCity();
	bool IsHolyCityForReligion(ReligionTypes eReligion);
	bool IsHolyCityAnyReligion();
	bool IsReligionHereOtherThan(ReligionTypes eReligion);
	bool IsDefendedAgainstSpread(ReligionTypes eReligion);
	ReligionTypes GetReligiousMajority();
	ReligionTypes GetSimulatedReligiousMajority();
	ReligionTypes GetSecondaryReligion();
	BeliefTypes GetSecondaryReligionPantheonBelief();
	int GetFollowersOtherReligions(ReligionTypes eReligion);
	bool HasPaidAdoptionBonus() const
	{
		return m_bHasPaidAdoptionBonus;
	};
	void SetPaidAdoptionBonus(bool bNewValue)
	{
		m_bHasPaidAdoptionBonus = bNewValue;
	};
	int GetReligiousPressureModifier() const
	{
		return m_iReligiousPressureModifier;
	};
	void SetReligiousPressureModifier(int iNewValue)
	{
		m_iReligiousPressureModifier = iNewValue;
	};
	void ChangeReligiousPressureModifier(int iNewValue)
	{
		SetReligiousPressureModifier(m_iReligiousPressureModifier + iNewValue);
	}
	int GetTotalPressure();
	int GetPressure(ReligionTypes eReligion);
	int GetPressurePerTurn(ReligionTypes eReligion, int& iNumTradeRoutesInvolved);
	int GetNumTradeRouteConnections (ReligionTypes eReligion);
	bool WouldExertTradeRoutePressureToward (CvCity* pTargetCity, ReligionTypes& eReligion, int& iAmount);

	// Routines to update religious status of citizens
	void DoPopulationChange(int iChange);
	void DoReligionFounded(ReligionTypes eReligion);
	void AddProphetSpread(ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer);
	void AddReligiousPressure(CvReligiousFollowChangeReason eReason, ReligionTypes eReligion, int iPressure, PlayerTypes eResponsiblePlayer=NO_PLAYER);
	void SimulateProphetSpread(ReligionTypes eReligion, int iPressure);
	void SimulateReligiousPressure(ReligionTypes eReligion, int iPressure);
	void ConvertPercentFollowers(ReligionTypes eToReligion, ReligionTypes eFromReligion, int iPercent);
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

	ReligionInCityList m_ReligionStatus;
	ReligionInCityList m_SimulatedStatus;

private:
	void RecomputeFollowers(CvReligiousFollowChangeReason eReason, ReligionTypes eOldMajorityReligion, PlayerTypes eResponsibleParty=NO_PLAYER);
	void SimulateFollowers();
	void CopyToSimulatedStatus();
	void CityConvertsReligion(ReligionTypes eMajority, ReligionTypes eOldMajority, PlayerTypes eResponsibleParty);
	void LogFollowersChange(CvReligiousFollowChangeReason eReason);

	CvCity* m_pCity;
	bool m_bHasPaidAdoptionBonus;
	int m_iReligiousPressureModifier;
};

FDataStream& operator>>(FDataStream&, CvCityReligions&);
FDataStream& operator<<(FDataStream&, const CvCityReligions&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvUnitReligion
//!  \brief		Information about the religious affiliation of a single unit
//
//!  Key Attributes:
//!  - One instance for each unit
//!  - Accessed by any class that needs to check religious information for this unit
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitReligion
{
public:
	CvUnitReligion(void);
	void Init();

	// Accessors
	ReligionTypes GetReligion() const
	{
		return m_eReligion;
	};
	void SetReligion(ReligionTypes eReligion)
	{
		m_eReligion = eReligion;
	};
	int GetReligiousStrength() const
	{
		return m_iStrength;
	};
	void SetReligiousStrength(int iValue)
	{
		m_iStrength = iValue;
	};
	int GetSpreadsLeft() const
	{
		return m_iSpreadsLeft;
	};
	void SetSpreadsLeft(int iValue)
	{
		m_iSpreadsLeft = iValue;
	};

private:
	ReligionTypes m_eReligion;
	int m_iStrength;
	int m_iSpreadsLeft;
};

FDataStream& operator>>(FDataStream&, CvUnitReligion&);
FDataStream& operator<<(FDataStream&, const CvUnitReligion&);

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
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

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

	CvCity* ChooseMissionaryTargetCity(UnitHandle pUnit);
	CvPlot* ChooseMissionaryTargetPlot(UnitHandle pUnit, int* piTurns = NULL);
	CvCity* ChooseInquisitorTargetCity(UnitHandle pUnit);
	CvPlot* ChooseInquisitorTargetPlot(UnitHandle pUnit, int* piTurns = NULL);
	CvCity *ChooseProphetConversionCity(bool bOnlyBetterThanEnhancingReligion) const;
	CvPlot* ChooseProphetTargetPlot(UnitHandle pUnit, int* piTurns = NULL);

	CvPlayer* GetPlayer();
	ReligionTypes GetReligionToSpread() const;

private:
	void DoFaithPurchases();
	void BuyMissionary(ReligionTypes eReligion);
	void BuyInquisitor(ReligionTypes eReligion);
	void BuyGreatPerson(UnitTypes eUnit);
	void BuyFaithBuilding(ReligionTypes eReligion, BuildingClassTypes eBuildingClass);
	bool BuyAnyAvailableNonFaithBuilding();
	bool BuyAnyAvailableFaithBuilding();

	int ScoreBelief(CvBeliefEntry* pEntry);
	int ScoreBeliefAtPlot(CvBeliefEntry* pEntry, CvPlot* pPlot);
	int ScoreBeliefAtCity(CvBeliefEntry* pEntry, CvCity* pCity);
	int ScoreBeliefForPlayer(CvBeliefEntry* pEntry);

	int ScoreCityForMissionary(CvCity* pCity, UnitHandle pUnit);
	int ScoreCityForInquisitor(CvCity* pCity, UnitHandle pUnit);

	bool ShouldBecomeNewMajority(CvCity* pCity, ReligionTypes eReligion, int iNewPressure) const;
	bool AreAllOurCitiesConverted(ReligionTypes eReligion, bool bIncludePuppets) const;
	bool AreAllOurCitiesHaveFaithBuilding(ReligionTypes eReligion, bool bIncludePuppets) const;
	bool HaveNearbyConversionTarget(ReligionTypes eReligion, bool bCanIncludeReligionStarter) const;
	bool HaveEnoughInquisitors(ReligionTypes eReligion) const;
	BuildingClassTypes FaithBuildingAvailable(ReligionTypes eReligion) const;
	bool CanBuyNonFaithBuilding() const;
	UnitTypes GetDesiredFaithGreatPerson() const;

	void LogBeliefChoices(CvWeightedVector<BeliefTypes, SAFE_ESTIMATE_NUM_BELIEFS, true>& beliefChoices, int iChoice);

	CvBeliefXMLEntries* m_pBeliefs;
	CvPlayer* m_pPlayer;

};

namespace CvReligionAIHelpers
{
	CvCity *GetBestCityFaithUnitPurchase(CvPlayer &kPlayer, UnitTypes eUnit, ReligionTypes eReligion);
	CvCity *GetBestCityFaithBuildingPurchase(CvPlayer &kPlayer, BuildingTypes eBuilding, ReligionTypes eReligion);
	bool DoesUnitPassFaithPurchaseCheck(CvPlayer &kPlayer, UnitTypes eUnit);
}

#endif //CIV5_RELIGION_CLASSES_H
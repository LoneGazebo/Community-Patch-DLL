/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_FLAVOR_MANAGER_H
#define CIV5_FLAVOR_MANAGER_H

#include "CvEnumMap.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvFlavorRecipient
//!  \brief		Based class - all classes that are going to receive flavor updates derive from this
//
//!  Key Attributes:
//!  - All classes derived from this must implement the FlavorUpdate() method
//!  - Stores the local version of the flavors; all deriving classes should call StoreLatestFlavors()
//!    at the start of FlavorUpdate()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFlavorRecipient
{
public:
	CvFlavorRecipient();
	virtual ~CvFlavorRecipient();
	virtual void Init();
	virtual void Uninit();

	bool IsCity() const;

	void SetFlavors(const CvEnumMap<FlavorTypes, int>& piUpdatedFlavorValues);
	void ChangeFlavors(const CvEnumMap<FlavorTypes, int>& piDeltaFlavorValues, bool bDontLog = false);

	int GetLatestFlavorValue(FlavorTypes eFlavor, bool bAllowNegative = false);

protected:

	virtual void FlavorUpdate() = 0;
	virtual void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR) = 0;

	bool m_bIsCity;

	CvEnumMap<FlavorTypes, int> m_piLatestFlavorValues;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvFlavorManager
//!  \brief		Handles AI requests for updates to a player's flavor settings
//
//!  Key Attributes:
//!  - One flavor manager per player
//!  - Primary input is default flavor data from Leaderheads XML file
//!  - Secondary input is request for flavor updates based on strategic concerns
//!  - Creates commands to flavor-based AI decision subsystems to recalibrate based
//!    on new flavor data
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFlavorManager
{
public:
	CvFlavorManager(void);
	~CvFlavorManager(void);

	// Setup stuff
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	template<typename FlavorManager, typename Visitor>
	static void Serialize(FlavorManager& flavorManager, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Routines to add/remove a target object to receive flavor updates
	void AddFlavorRecipient(CvFlavorRecipient* pTargetObject, bool bPropogateFlavorValues = true);
	void RemoveFlavorRecipient(CvFlavorRecipient* pTargetObject);

	void ChangeLeader(LeaderHeadTypes eOldLeader, LeaderHeadTypes eNewLeader);

	// External routines used to set flavors - each call broadcasts a flavor update to all recipients
	void ChangeFlavors(const CvEnumMap<FlavorTypes, int>& piDeltaFlavorValues, bool bDontUpdateCityFlavors);
	void ResetToBasePersonality();
	void AdjustWeightsForMap();

	// External routines to retrieve Default Personality flavor values
	int GetPersonalityIndividualFlavor(FlavorTypes eType);
	CvEnumMap<FlavorTypes, int>& GetAllPersonalityFlavors();
	int GetPersonalityFlavorForDiplomacy(FlavorTypes eType);

	int GetAdjustedValue(int iOriginalValue, int iPlusMinus, int iMin, int iMax, const CvSeeder& seed);

private:

	void RandomizeWeights();
	void BroadcastFlavors(const CvEnumMap<FlavorTypes, int>& piDeltaFlavorValues, bool bDontUpdateCityFlavors);
	void BroadcastBaseFlavors();

	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);

	CvEnumMap<FlavorTypes, int> m_piPersonalityFlavor;
	CvEnumMap<FlavorTypes, int> m_piActiveFlavor;
	CvPlayer* m_pPlayer;
	typedef vector<CvFlavorRecipient*> Flavor_List;
	Flavor_List m_FlavorTargetList;
};

FDataStream& operator>>(FDataStream&, CvFlavorManager&);
FDataStream& operator<<(FDataStream&, const CvFlavorManager&);

#endif // CIV5_FLAVOR_MANAGER_H
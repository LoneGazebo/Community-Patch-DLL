/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_FLAVOR_MANAGER_H
#define CIV5_FLAVOR_MANAGER_H

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
	~CvFlavorRecipient();
	virtual void Init();
	virtual void Uninit();

	bool IsCity();

	void SetFlavors(int* piUpdatedFlavorValues);
	void ChangeFlavors(int* piDeltaFlavorValues, bool bDontLog = false);

	int GetLatestFlavorValue(FlavorTypes eFlavor, bool bAllowNegative = false);

protected:

	virtual void FlavorUpdate() = 0;
	virtual void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR) = 0;

	bool m_bIsCity;

	int* m_piLatestFlavorValues;
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
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	// Routines to add/remove a target object to receive flavor updates
	void AddFlavorRecipient(CvFlavorRecipient* pTargetObject, bool bPropogateFlavorValues = true);
	void RemoveFlavorRecipient(CvFlavorRecipient* pTargetObject);

	// External routines used to set flavors - each call broadcasts a flavor update to all recipients
	void ChangeFlavors(int* piDeltaFlavorValues, bool bDontUpdateCityFlavors);
	void ResetToBasePersonality();
	void AdjustWeightsForMap();

	// External routines to retrieve flavor values
	int GetIndividualFlavor(FlavorTypes eType);
	int* GetAllFlavors();

	// External routines to retrieve Default Personality flavor values
	int GetPersonalityIndividualFlavor(FlavorTypes eType);
	int* GetAllPersonalityFlavors();

	int GetAdjustedValue(int iOriginalValue, int iPlusMinus, int iMin, int iMax);

private:

	void RandomizeWeights();
	void BroadcastFlavors(int* piDeltaFlavorValues, bool bDontUpdateCityFlavors);
	void BroadcastBaseFlavors();

	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);

	int* m_piPersonalityFlavor;
	int* m_piActiveFlavor;
	CvPlayer* m_pPlayer;
	// std::list<CvFlavorRecipient *> m_FlavorTargetList;
	typedef FFastList<CvFlavorRecipient*, c_eCiv5GameplayDLL, 0> Flavor_List;
	Flavor_List m_FlavorTargetList;
};

#endif // CIV5_FLAVOR_MANAGER_H
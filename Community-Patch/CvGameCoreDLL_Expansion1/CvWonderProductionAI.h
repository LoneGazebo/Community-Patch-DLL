/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_WONDER_PRODUCTION_AI_H
#define CIV5_WONDER_PRODUCTION_AI_H

// Allocate array for 50% more wonders than we have now (34)
//   May want to tune this number closer to shipping, though safe enough
//   given that each entry is only 8 bytes
#define SAFE_ESTIMATE_NUM_WONDERS 50

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvWonderProductionAI
//!  \brief		Handles wonder production decisions for one player
//
//!  Key Attributes:
//!  - Object created by CvEconomicAI (so one per player)
//!  - Uses flavor-based weighting choices (similar to one used for policies and techs)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWonderProductionAI: public CvFlavorRecipient
{
public:
	CvWonderProductionAI(CvPlayer* pPlayer, CvBuildingXMLEntries* pBuildings);
	~CvWonderProductionAI(void);
	void Init(CvBuildingXMLEntries* pBuildings, CvPlayer* pPlayer, bool bIsCity);

	// Initialization
	void Reset();

	// Serialization routines
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Flavor recipient required function
	void FlavorUpdate();

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(BuildingTypes eBuilding);

	// Recommend highest-weighted wonder
	BuildingTypes ChooseWonder(bool bUseAsyncRandom, bool bAdjustForOtherPlayers, int& iWonderWeight);
	BuildingTypes ChooseWonderForGreatEngineer(bool bUseAsyncRandom, int& iWonderWeight, CvCity*& pCityToBuildAt);

	// Logging
	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);
	void LogPossibleWonders();

	bool IsWonder(const CvBuildingEntry& kBuilding) const;

private:
	bool HaveCityToBuild(BuildingTypes eBuilding) const;

	// Private data
	CvPlayer* m_pPlayer;
	CvBuildingXMLEntries* m_pBuildings;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> m_WonderAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_WONDERS, true> m_Buildables;
};

#endif //CIV5_WONDER_PRODUCTION_AI_H
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
	template<typename WonderProductionAI, typename Visitor>
	static void Serialize(WonderProductionAI& wonderProductionAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Flavor recipient required function
	void FlavorUpdate();

	// Establish/retrieve weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(BuildingTypes eBuilding);

	// Recommend highest-weighted wonder
	BuildingTypes ChooseWonder(bool bAdjustForOtherPlayers, int& iWonderWeight);

	// Logging
	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);
	void LogPossibleWonders();

	bool IsWonder(const CvBuildingEntry& kBuilding) const;
	bool IsWonderNotNationalUnique(const CvBuildingEntry& kBuilding) const;

private:
	bool HaveCityToBuild(BuildingTypes eBuilding) const;

	// Private data
	CvPlayer* m_pPlayer;
	CvBuildingXMLEntries* m_pBuildings;
	CvWeightedVector<int> m_WonderAIWeights;
	CvWeightedVector<int> m_Buildables;
};

FDataStream& operator>>(FDataStream&, CvWonderProductionAI&);
FDataStream& operator<<(FDataStream&, const CvWonderProductionAI&);

#endif //CIV5_WONDER_PRODUCTION_AI_H
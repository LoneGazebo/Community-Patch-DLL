/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CIV5_AREA_H
#define CIV5_AREA_H

class CvCity;
class CvPlot;

//////////////////////////////////////////////////////////////////////////
struct CvAreaBoundaries
{
	CvAreaBoundaries() :
		m_iNorthEdge(0)
		, m_iSouthEdge(0)
		, m_iEastEdge(0)
		, m_iWestEdge(0)
	{
	}

	int m_iNorthEdge;
	int m_iSouthEdge;
	int m_iEastEdge;
	int m_iWestEdge;
};

//////////////////////////////////////////////////////////////////////////
// an CvArea is a subdivision of a CvLandmass
//////////////////////////////////////////////////////////////////////////
class CvArea
{

public:

	CvArea();
	virtual ~CvArea();

	void init(int iID, bool bWater);
	void uninit();
	void reset(int iID = 0, bool bWater = false);

	int calculateTotalBestNatureYield() const;

	int countCoastalLand() const;
	int countNumUniqueResourceTypes() const;

	void SetID(int iID);
	inline int GetID() const
	{
		return m_iID;
	}

	int getNumTiles() const;
	void changeNumTiles(int iChange);

	int getNumOwnedTiles() const;
	int getNumUnownedTiles() const;
	void changeNumOwnedTiles(int iChange);

	int getNumRiverEdges() const;
	void changeNumRiverEdges(int iChange);

	int getNumCities() const;

	int getNumUnits() const;

	int getTotalPopulation() const;

	int getNumStartingPlots() const;
	void changeNumStartingPlots(int iChange);

	bool isWater() const;

	bool IsMountains() const;
	void SetMountains(bool bValue);

	int getUnitsPerPlayer(PlayerTypes eIndex) const;
	void changeUnitsPerPlayer(PlayerTypes eIndex, int iChange);
	int getEnemyUnits(PlayerTypes eIndex) const;

	int getCitiesPerPlayer(PlayerTypes eIndex) const;
	void changeCitiesPerPlayer(PlayerTypes eIndex, int iChange);

	int getPopulationPerPlayer(PlayerTypes eIndex) const;
	void changePopulationPerPlayer(PlayerTypes eIndex, int iChange);

	int getFreeSpecialist(PlayerTypes eIndex) const;
	void changeFreeSpecialist(PlayerTypes eIndex, int iChange);

	int getTotalFoundValue() const;
	void setTotalFoundValue(int iNewValue);

	int getNumRevealedTiles(TeamTypes eIndex) const;
	int getNumUnrevealedTiles(TeamTypes eIndex) const;
	void changeNumRevealedTiles(TeamTypes eIndex, int iChange);

	CvCity* getTargetCity(PlayerTypes eIndex) const;
	void setTargetCity(PlayerTypes eIndex, CvCity* pNewValue);

	int getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const;
	void changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getNumResources(ResourceTypes eResource) const;
	int getNumTotalResources() const;
	void changeNumResources(ResourceTypes eResource, int iChange);

	int getNumImprovements(ImprovementTypes eImprovement) const;
	void changeNumImprovements(ImprovementTypes eImprovement, int iChange);

	CvAreaBoundaries getAreaBoundaries() const;
	void FindBoundaries(const vector<bool>& occupiedCols, const vector<bool>& occupiedRows);

	void GetTopAndBottomLatitudes(int& iTopLatitude, int& iBottomLatitude) const;
	int GetAreaMaxLatitude();
	int GetAreaMinLatitude();

	int GetNumNaturalWonders() const;
	void ChangeNumNaturalWonders(int iChange);

	int GetNumBadPlots() const;
	void UpdateBadPlotsCount(CvPlot* pPlot);

	// for serialization
	template<typename Area, typename Visitor>
	static void Serialize(Area& area, Visitor& visitor);
	virtual void read(FDataStream& kStream);
	virtual void write(FDataStream& kStream) const;

protected:

	int m_iID;
	int m_iNumTiles;
	int m_iNumOwnedTiles;
	int m_iNumRiverEdges;
	int m_iNumUnits;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iNumStartingPlots;
	int m_iNumNaturalWonders;
	int m_iBadPlots;
	unsigned int m_iTotalFoundValue;

	map<PlayerTypes,int> m_aiUnitsPerPlayer;
	map<PlayerTypes,int> m_aiCitiesPerPlayer;
	map<PlayerTypes,int> m_aiPopulationPerPlayer;
	map<PlayerTypes,int> m_aiFreeSpecialist;
	map<TeamTypes,int> m_aiNumRevealedTiles;
	map<PlayerTypes,IDInfo> m_aTargetCities;
	map<PlayerTypes,vector<int>> m_aaiYieldRateModifier;

	map<ResourceTypes,int> m_aiNumResources;
	map<ImprovementTypes, int> m_aiNumImprovements;

	CvAreaBoundaries m_Boundaries;

	bool m_bWater;
	bool m_bMountains;
};

FDataStream& operator<<(FDataStream&, const CvArea&);
FDataStream& operator>>(FDataStream&, CvArea&);


#endif

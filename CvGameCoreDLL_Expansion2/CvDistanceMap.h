#pragma once

#ifndef CIV5_DISTANCE_MAP_H
#define CIV5_DISTANCE_MAP_H

#include "CvEnums.h"

struct STiebreakGenerator
{
	virtual int operator()(int iOwner, int iFeatureID) const = 0;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvDistanceMap
//!  \brief		Used to calculate the distance transform for given map features (typically cities)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDistanceMap
{
public:
	CvDistanceMap(void);
	~CvDistanceMap(void);

	void Reset(int nPlots, int iDefaultDistance);
	bool UpdateDistanceIfLower(int iPlotIndex, int iOwner, int iID, int iDistance, const STiebreakGenerator& tiebreaker);

	//not const because of deferred updates ...
	int GetClosestFeatureDistance(const CvPlot& pPlot);
	int GetClosestFeatureID(const CvPlot& pPlot);
	int GetClosestFeatureOwner(const CvPlot& pPlot);

	void Dump(const char* filename);

protected:
	std::vector<int> m_vClosestFeature; //plot index to feature
	std::vector<int> m_vDistance;		//plot index to distance
};

class CvDistanceMapWrapper
{
protected:
	bool m_bDirty;
	CvDistanceMap allPlayers;
	CvDistanceMap allMajorPlayers;
	vector<CvDistanceMap> majorPlayers; 

	virtual void Update() = 0;

public:
	void SetDirty();
	bool IsDirty() const { return m_bDirty;	}

	int GetDistance(const CvPlot& plot, bool bMajorsOnly, PlayerTypes eSpecificPlayer);
	int GetFeatureId(const CvPlot& plot, bool bMajorsOnly, PlayerTypes eSpecificPlayer);
	int GetFeatureOwner(const CvPlot& plot, bool bMajorsOnly, PlayerTypes eSpecificPlayer);
};

class CvDistanceMapByTurns : public CvDistanceMapWrapper
{
protected:
	virtual void Update();
};

class CvDistanceMapByPlots : public CvDistanceMapWrapper
{
protected:
	virtual void Update();
};

#endif //CIV5_PROJECT_CLASSES_H

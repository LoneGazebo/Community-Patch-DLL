#pragma once

#ifndef CIV5_DISTANCE_MAP_H
#define CIV5_DISTANCE_MAP_H

#include "CvEnums.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvDistanceMap
//!  \brief		Used to calculate the distance transform for given map features (typically cities)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDistanceMap
{
public:
	CvDistanceMap(void);
	~CvDistanceMap(void);

	virtual void SetPlayer(PlayerTypes ePlayer);
	virtual void Reset();

	//not const because of deferred updates ...
	virtual int GetClosestFeatureDistance(const CvPlot& pPlot);
	virtual int GetClosestFeatureID(const CvPlot& pPlot);
	virtual int GetClosestFeatureOwner(const CvPlot& pPlot);

	void Dump(const char* filename);
	void SetDirty();
	bool IsDirty() const
	{
		return m_bDirty;
	}

protected:

	virtual void Update() = 0;

	PlayerTypes m_ePlayer; //may be no_player
	bool m_bArrayAllocated;
	bool m_bDirty;

	std::vector<int> m_vClosestFeature; //plot index to feature
	std::vector<int> m_vDistance;		//plot index to distance
};

class CvDistanceMapTurns : public CvDistanceMap
{
protected:
	virtual void Update();
};

class CvDistanceMapPlots : public CvDistanceMap
{
protected:
	virtual void Update();
};

#endif //CIV5_PROJECT_CLASSES_H
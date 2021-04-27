#pragma once

#ifndef CIV5_DISTANCE_MAP_H
#define CIV5_DISTANCE_MAP_H

#include "CvEnums.h"

struct STiebreakGenerator
{
	//return 1 if B wins, -1 if A wins
	virtual int operator()(int iOwnerA, int iFeatureA, int iOwnerB, int iFeatureB) const = 0;
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

	void Reset(int nPlots, unsigned short iDefaultDistance);
	bool UpdateDistanceIfLower(int iPlotIndex, int iOwner, int iID, int iDistance, const STiebreakGenerator& tiebreaker);

	//not const because of deferred updates ...
	int GetClosestFeatureDistance(const CvPlot& pPlot);
	int GetClosestFeatureID(const CvPlot& pPlot);
	PlayerTypes GetClosestFeatureOwner(const CvPlot& pPlot);

	void Dump(const char* filename);

protected:
	struct SContent {
		enum { INVALID = 0xFFFF };

		SContent(unsigned short _distance) : distance(_distance), owner(INVALID), feature(INVALID) {}
		SContent(int _distance, int _owner, int _feature) : distance((unsigned short)_distance), owner((unsigned short)_owner), feature((unsigned short)_feature) {}

		//try and save some memory here
		unsigned short distance;
		unsigned short owner;
		unsigned short feature;
	};

	std::vector<SContent> m_vData;
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
	PlayerTypes GetFeatureOwner(const CvPlot& plot, bool bMajorsOnly, PlayerTypes eSpecificPlayer);
};

class CvDistanceMapByPathLength : public CvDistanceMapWrapper
{
public:
	CvDistanceMapByPathLength(DomainTypes eDomain) : m_domain(eDomain) {}
protected:
	virtual void Update();
	DomainTypes m_domain;
};

class CvDistanceMapByPlots : public CvDistanceMapWrapper
{
protected:
	virtual void Update();
};

#endif //CIV5_PROJECT_CLASSES_H

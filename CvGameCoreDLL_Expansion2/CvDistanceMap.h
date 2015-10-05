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

	void Init(PlayerTypes ePlayer, bool bAllocate);
	void Uninit();
	void Reset();

	void Update();
	int GetDistanceFromFriendlyCity(const CvPlot& pPlot) const;
	int GetClosestFriendlyCity(const CvPlot& pPlot) const;

	void SetDirty();
	bool IsDirty() const
	{
		return m_bDirty;
	}

protected:

	PlayerTypes m_ePlayer;
	bool m_bArrayAllocated;
	bool m_bDirty;

	FFastVector<int, true, c_eCiv5GameplayDLL, 0> m_vDistance;
	FFastVector<int, true, c_eCiv5GameplayDLL, 0> m_vCityID;
};

#endif //CIV5_PROJECT_CLASSES_H
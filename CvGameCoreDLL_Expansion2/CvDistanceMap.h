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
	int GetDanger(const CvPlot& pPlot) const;

	void SetDirty();
	bool IsDirty() const
	{
		return m_bDirty;
	}

protected:

	PlayerTypes m_ePlayer;
	bool m_bArrayAllocated;
	bool m_bDirty;

	FFastVector<uint, true, c_eCiv5GameplayDLL, 0> m_values;
};

#endif //CIV5_PROJECT_CLASSES_H
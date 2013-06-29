#ifndef _CVPLOTMANAGER_H_
#define _CVPLOTMANAGER_H_

#pragma once

#include "FireWorks/FObjectPool.h"

#define DEFAULT_INFO_ARRAY_SIZE	8

class CvIDInfoFixedVector : public FStaticVector<IDInfo, DEFAULT_INFO_ARRAY_SIZE, true, c_eCiv5GameplayDLL >
{
public:

	IDInfo* Find(const IDInfo& kInfo)
	{
		for (iterator itr = begin(); itr != end(); ++itr)
		{
			if ((*itr) == kInfo)
				return &(*itr);
		}
		return NULL;
	}

	const IDInfo* Find(const IDInfo& kInfo) const
	{
		for (const_iterator itr = begin(); itr != end(); ++itr)
		{
			if ((*itr) == kInfo)
				return &(*itr);
		}
		return NULL;
	}

	void Add(const IDInfo& kInfo)
	{
		// Only if it is not already in.
		if (!Find(kInfo))
			push_back(kInfo);
	}

	bool Remove(const IDInfo& kInfo)
	{
		for (iterator itr = begin(); itr != end(); ++itr)
		{
			if ((*itr) == kInfo)
			{
				erase(itr);
				return true;
			}
		}
		return false;
	}
};

typedef FObjectPool<CvIDInfoFixedVector> CvIDInfoFixedVectorAllocator;

class CvSparseIDInfoGrid
{
	uint	m_uiWidth;
	uint	m_uiHeight;
	uint	m_uiMaxIndex;
	uint	m_uiNumAllocated;

	CvIDInfoFixedVector**	m_paEntries;
	CvIDInfoFixedVectorAllocator* m_pkAllocator;

public:

	CvSparseIDInfoGrid(uint uiWidth, uint uiHeight, CvIDInfoFixedVectorAllocator* pkAllocator);
	~CvSparseIDInfoGrid();

	const CvIDInfoFixedVector *Get(int iX, int iY) const;
	bool Add(const IDInfo& kInfo, int iX, int iY);
	void Remove(const IDInfo& kInfo, int iX, int iY);
};


struct CvSparseIDInfoGridEntry
{
	uint				m_uiID;
	CvSparseIDInfoGrid*	m_pkGrid;

	CvSparseIDInfoGridEntry();
	CvSparseIDInfoGridEntry(uint uiID, CvSparseIDInfoGrid* pkGrid) : m_uiID(uiID), m_pkGrid(pkGrid) {}

	uint GetID() const { return m_uiID; };
	CvSparseIDInfoGrid*	GetGrid() const { return m_pkGrid; };
};

typedef std::vector<CvSparseIDInfoGridEntry> CvSparseIDInfoGridVector;

class CvPlotManager
{
public:
	CvPlotManager();
	~CvPlotManager();

	void Init(uint uiWidth, uint uiHeight);
	void Uninit();

	int GetNumLayers() const;
	int GetLayerIDIndex(uint uiLayerID) const;
	uint GetLayerIndexID(int iLayerIndex) const;

	const CvIDInfoFixedVector &GetUnits(int iX, int iY, uint uiLayerID) const;
	const CvIDInfoFixedVector &GetUnitsByIndex(int iX, int iY, int iLayerIndex) const;
	void AddUnit(const IDInfo& kInfo, int iX, int iY, uint uiLayerID);
	void RemoveUnit(const IDInfo& kInfo, int iX, int iY, uint uiLayerID);
	int  GetNumUnits(int iX, int iY, uint uiLayerID) const;
	int  GetNumUnitsByIndex(int iX, int iY, int iLayerIndex) const;
	
	const CvSparseIDInfoGridVector& GetLayers() const { return m_aLayers; };
	const CvSparseIDInfoGrid*		FindLayer(uint uiLayerID) const;


protected:

	CvSparseIDInfoGrid*	AddLayer(uint uiLayerID);
	CvSparseIDInfoGrid*	FindLayer(uint uiLayerID);

	uint	m_uiWidth;
	uint	m_uiHeight;

	CvIDInfoFixedVector			m_kEmpty;

	CvSparseIDInfoGridVector	m_aLayers;

	CvIDInfoFixedVectorAllocator m_kAllocator;
};


#endif // _CVPLOTMANAGER_H_
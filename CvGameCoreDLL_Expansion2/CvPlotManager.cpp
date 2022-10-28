/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvPlotManager.h"

//////////////////////////////////////////////////////////////////////////
// CvSparseIDInfoGrid
//////////////////////////////////////////////////////////////////////////

//	---------------------------------------------------------------------------
CvSparseIDInfoGrid::CvSparseIDInfoGrid(uint uiWidth, uint uiHeight, CvIDInfoFixedVectorAllocator* pkAllocator)
{
	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;
	m_uiMaxIndex = uiWidth * uiHeight;
	m_uiNumAllocated = 0;

	m_paEntries = FNEW(CvIDInfoFixedVector*[m_uiMaxIndex], c_eCiv5GameplayDLL, 0);
	memset( &m_paEntries[0], 0, sizeof(CvIDInfoFixedVector*) * m_uiMaxIndex);

	m_pkAllocator = pkAllocator;
}

//	---------------------------------------------------------------------------
CvSparseIDInfoGrid::~CvSparseIDInfoGrid()
{
	if (m_paEntries)
	{
		CvIDInfoFixedVector** pkEntry = m_paEntries;
		for (uint uiIndex = m_uiMaxIndex; --uiIndex;)
		{
			if (*pkEntry)
				m_pkAllocator->Release(*pkEntry);

			pkEntry++;
		}
		delete []m_paEntries;
	}
}
//	---------------------------------------------------------------------------
const CvIDInfoFixedVector *CvSparseIDInfoGrid::Get(int iX, int iY) const
{
	uint uiIndex = ((uint) iY * m_uiWidth) + (uint) iX;
	if (uiIndex < m_uiMaxIndex)
	{
		CvIDInfoFixedVector* pkEntry = m_paEntries[uiIndex];
		if (pkEntry)
			return pkEntry;
	}

	return NULL;
}

//	---------------------------------------------------------------------------
bool CvSparseIDInfoGrid::Add(const IDInfo& kInfo, int iX, int iY)
{
	uint uiIndex = ((uint) iY * m_uiWidth) + (uint) iX;
	if (uiIndex < m_uiMaxIndex)
	{
		CvIDInfoFixedVector* pkEntry = m_paEntries[uiIndex];
		if (!pkEntry)
		{
			pkEntry = m_pkAllocator->GetFreeObject();
			m_paEntries[uiIndex] = pkEntry;
		}

		if (pkEntry->Find(kInfo) == NULL)
		{
			pkEntry->push_back(kInfo);
		}
		return true;
	}

	return false;
}

//	---------------------------------------------------------------------------
void CvSparseIDInfoGrid::Remove(const IDInfo& kInfo, int iX, int iY)
{
	uint uiIndex = ((uint) iY * m_uiWidth) + (uint) iX;
	if (uiIndex < m_uiMaxIndex)
	{
		CvIDInfoFixedVector* pkEntry = m_paEntries[uiIndex];
		if (pkEntry)
		{
			if (pkEntry->Remove(kInfo))
			{
				if (pkEntry->size() == 0)
				{
					// Nothing there, release the array
					m_pkAllocator->Release(pkEntry);
					m_paEntries[uiIndex] = NULL;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CvPlotManager
//////////////////////////////////////////////////////////////////////////

//	---------------------------------------------------------------------------
CvPlotManager::CvPlotManager()
{
	m_uiWidth = 0;
	m_uiHeight = 0;
}

//	---------------------------------------------------------------------------
CvPlotManager::~CvPlotManager()
{
	
}

//	---------------------------------------------------------------------------
void CvPlotManager::Init(uint uiWidth, uint uiHeight)
{
	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;
}

//	---------------------------------------------------------------------------
void CvPlotManager::Uninit()
{
	m_aLayers.clear();
}

//	---------------------------------------------------------------------------
int CvPlotManager::GetNumLayers() const
{
	return (int)m_aLayers.size();
}

//	---------------------------------------------------------------------------
int CvPlotManager::GetLayerIDIndex(uint uiLayerID) const
{
	int iIndex = 0;
	for (CvSparseIDInfoGridVector::const_iterator itr = m_aLayers.begin(); itr != m_aLayers.end(); ++itr)
	{
		if ((*itr).m_uiID == uiLayerID)
		{
			return iIndex;
		}
		++iIndex;
	}

	return -1;
}

//	---------------------------------------------------------------------------
uint CvPlotManager::GetLayerIndexID(int iLayerIndex) const
{
	if (iLayerIndex >= 0 && iLayerIndex < (int)m_aLayers.size())
		return m_aLayers[iLayerIndex].GetID();

	return (uint)-1;
}

//	---------------------------------------------------------------------------
CvSparseIDInfoGrid* CvPlotManager::AddLayer(uint uiLayerID)
{
	for (CvSparseIDInfoGridVector::const_iterator itr = m_aLayers.begin(); itr != m_aLayers.end(); ++itr)
	{
		if ((*itr).m_uiID == uiLayerID)
		{
			return (*itr).m_pkGrid;
		}
	}

	CvSparseIDInfoGrid* pkLayer = new CvSparseIDInfoGrid(m_uiWidth, m_uiHeight, &m_kAllocator);
	m_aLayers.push_back(CvSparseIDInfoGridEntry(uiLayerID, pkLayer));

	return pkLayer;
}

//	---------------------------------------------------------------------------
CvSparseIDInfoGrid* CvPlotManager::FindLayer(uint uiLayerID)
{
	for (CvSparseIDInfoGridVector::const_iterator itr = m_aLayers.begin(); itr != m_aLayers.end(); ++itr)
	{
		if ((*itr).m_uiID == uiLayerID)
		{
			return (*itr).m_pkGrid;
		}
	}

	return NULL;
}

//	---------------------------------------------------------------------------
const CvSparseIDInfoGrid* CvPlotManager::FindLayer(uint uiLayerID) const
{
	for (CvSparseIDInfoGridVector::const_iterator itr = m_aLayers.begin(); itr != m_aLayers.end(); ++itr)
	{
		if ((*itr).m_uiID == uiLayerID)
		{
			return (*itr).m_pkGrid;
		}
	}

	return NULL;
}

//	---------------------------------------------------------------------------
const CvIDInfoFixedVector &CvPlotManager::GetUnits(int iX, int iY, uint uiLayerID) const
{
	if (uiLayerID != (uint)-1)
	{
		for (CvSparseIDInfoGridVector::const_iterator itr = m_aLayers.begin(); itr != m_aLayers.end(); ++itr)
		{
			if ((*itr).m_uiID == uiLayerID)
			{
				const CvIDInfoFixedVector* pkEntry = (*itr).m_pkGrid->Get(iX, iY);
				if (pkEntry)
					return *pkEntry;
			}
		}
	}

	return m_kEmpty;
}

//	---------------------------------------------------------------------------
const CvIDInfoFixedVector &CvPlotManager::GetUnitsByIndex(int iX, int iY, int iLayerIndex) const
{
	if (iLayerIndex >= 0 && iLayerIndex < (int)m_aLayers.size())
	{
		const CvIDInfoFixedVector* pkEntry = m_aLayers[iLayerIndex].GetGrid()->Get(iX, iY);
		if (pkEntry)
			return *pkEntry;
	}

	return m_kEmpty;
}

//	---------------------------------------------------------------------------
void CvPlotManager::AddUnit(const IDInfo& kInfo, int iX, int iY, uint uiLayerID)
{
	CvSparseIDInfoGrid* pkLayer = AddLayer(uiLayerID);
	if (pkLayer)
	{
		pkLayer->Add(kInfo, iX, iY);
	}
}

//	---------------------------------------------------------------------------
void CvPlotManager::RemoveUnit(const IDInfo& kInfo, int iX, int iY, uint uiLayerID)
{
	if (uiLayerID != -1)
	{
		CvSparseIDInfoGrid* pkLayer = FindLayer(uiLayerID);
		if (pkLayer)
		{
			pkLayer->Remove(kInfo, iX, iY);
		}
	}
	else
	{
		int iIndex = 0;
		for (CvSparseIDInfoGridVector::const_iterator itr = m_aLayers.begin(); itr != m_aLayers.end(); ++itr)
		{
			(*itr).GetGrid()->Remove(kInfo, iX, iY);
		}
	}
}

//	---------------------------------------------------------------------------
int CvPlotManager::GetNumUnits(int iX, int iY, uint uiLayerID) const
{
	const CvSparseIDInfoGrid* pkLayer = FindLayer(uiLayerID);
	if (pkLayer)
	{
		const CvIDInfoFixedVector* pkUnits = pkLayer->Get(iX, iY);
		if (pkUnits)
			return pkUnits->size();
	}

	return 0;
}

//	---------------------------------------------------------------------------
int CvPlotManager::GetNumUnitsByIndex(int iX, int iY, int iLayerIndex) const
{
	if (iLayerIndex >= 0 && iLayerIndex < (int)m_aLayers.size())
	{
		const CvIDInfoFixedVector* pkEntry = m_aLayers[iLayerIndex].GetGrid()->Get(iX, iY);
		if (pkEntry)
			return (int)pkEntry->size();
	}

	return 0;
}

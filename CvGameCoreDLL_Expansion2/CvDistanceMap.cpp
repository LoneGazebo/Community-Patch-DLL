/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDistanceMap.h"
#include "CvGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"

/// Constructor
CvDistanceMap::CvDistanceMap(void)
	: m_ePlayer(NO_PLAYER)
	, m_bArrayAllocated(false)
	, m_bDirty(false)
{
}

/// Destructor
CvDistanceMap::~CvDistanceMap(void)
{
	Uninit();
}

/// Initialize
void CvDistanceMap::Init(PlayerTypes ePlayer, bool bAllocate)
{
	Uninit();
	m_ePlayer = ePlayer;

	if(bAllocate)
	{
		int iGridSize = GC.getMap().numPlots();
		CvAssertMsg(iGridSize > 0, "iGridSize is zero");
		m_vDistance.resize(iGridSize);
		m_vCityID.resize(iGridSize);
		m_bArrayAllocated = true;
		for(int i = 0; i < iGridSize; i++)
		{
			m_vDistance[i] = INT_MAX;
			m_vCityID[i] = -1;
		}
	}
}

/// Uninitialize
void CvDistanceMap::Uninit()
{
	m_ePlayer = NO_PLAYER;
	m_vDistance.clear();
	m_vCityID.clear();
	m_bArrayAllocated = false;
	m_bDirty = false;
}

/// Updates the danger plots values to reflect threats across the map
void CvDistanceMap::Update()
{
	//plots have not been initialized yet, so no way to update
	if(!m_bArrayAllocated)
	{
		return;
	}

	// wipe out values
	int iGridSize = GC.getMap().numPlots();
	CvAssertMsg(iGridSize == m_values.size(), "CvDistanceMap: iGridSize does not match");
	for(int i = 0; i < iGridSize; i++)
	{
		m_vDistance[i] = MAX_INT;
	}

	// since we know there are very few cities compared to the number of plots,
	// we don't need to do the full distance transform

	// for each city
	CvPlayer& thisPlayer = GET_PLAYER(m_ePlayer);
	int iCityIndex, iPlotIndex;
	CvCity* pLoopCity;

	const CvMap& map = GC.getMap();
	int nPlots = map.numPlots();

	for(pLoopCity = thisPlayer.firstCity(&iCityIndex); pLoopCity != NULL; pLoopCity = thisPlayer.nextCity(&iCityIndex))
	{
		CvPlot* pCityPlot = pLoopCity->plot();

		for (iPlotIndex=0; iPlotIndex<nPlots; iPlotIndex++)
		{
			CvPlot* pPlot = map.plotByIndexUnchecked(iPlotIndex);
			if (pPlot)
			{
				int iDistance = plotDistance( pCityPlot->getX(),pCityPlot->getY(),pPlot->getX(),pPlot->getY() );
				if (iDistance < m_vDistance[iPlotIndex])
				{
					m_vDistance[iPlotIndex] = iDistance;
					m_vCityID[iPlotIndex] = pLoopCity->GetID();
				}
			}
		}
	}

	m_bDirty = false;
}

//	-----------------------------------------------------------------------------------------------
void CvDistanceMap::SetDirty()
{
	m_bDirty = true;
}

//	-----------------------------------------------------------------------------------------------
int CvDistanceMap::GetDistanceFromFriendlyCity(const CvPlot& plot) const
{
	if (m_bArrayAllocated)
		return m_vDistance[ GC.getMap().plotNum( plot.getX(), plot.getY() ) ]; 

	return INT_MAX;
}

//	-----------------------------------------------------------------------------------------------
int CvDistanceMap::GetClosestFriendlyCity(const CvPlot& plot) const
{
	if (m_bArrayAllocated)
		return m_vCityID[ GC.getMap().plotNum( plot.getX(), plot.getY() ) ];

	return -1;
}
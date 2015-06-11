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
		m_values.resize(iGridSize);
		m_bArrayAllocated = true;
		for(int i = 0; i < iGridSize; i++)
		{
			m_values[i] = INT_MAX;
		}
	}
}

/// Uninitialize
void CvDistanceMap::Uninit()
{
	m_ePlayer = NO_PLAYER;
	m_values.clear();
	m_bArrayAllocated = false;
	m_bDirty = false;
}

/// Updates the danger plots values to reflect threats across the map
void CvDistanceMap::Update()
{
	// danger plots have not been initialized yet, so no need to update
	if(!m_bArrayAllocated)
	{
		return;
	}

	// wipe out values
	int iGridSize = GC.getMap().numPlots();
	CvAssertMsg(iGridSize == m_values.size(), "CvDistanceMap: iGridSize does not match");
	for(int i = 0; i < iGridSize; i++)
	{
		m_values[i] = MAX_INT;
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
				m_values[iPlotIndex] = min<int>( m_values[iPlotIndex], plotDistance( pCityPlot->getX(),pCityPlot->getY(),pPlot->getX(),pPlot->getY() ) );
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
	return m_values[ GC.getMap().plotNum( plot.getX(), plot.getY() ) ]; 
}

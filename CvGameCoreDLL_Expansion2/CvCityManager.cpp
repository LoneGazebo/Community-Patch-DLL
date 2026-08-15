/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvCityManager.h"
#include "CvGameCoreUtils.h"

typedef std::map<CvCity*, CvCityManager::CityList> CityMap;

static CityMap ms_kCityMap;

static CvCityManager::CityList ms_kEmptyList;

//	---------------------------------------------------------------------------
void CvCityManager::Reset()
{
	ms_kCityMap.clear();
}

//	---------------------------------------------------------------------------
void CvCityManager::Shutdown()
{
	ms_kCityMap.clear();
}

//	---------------------------------------------------------------------------
static void AddToSortedList(CvCityManager::CityList &kCityList, int iFromX, int iFromY, CvCity* pkAddCity)
{
	if (kCityList.size() == 0)
	{
		// First one
		kCityList.push_back(pkAddCity);
	}
	else
	{
		// Add to the pre-sorted list.
		int iAddDistance = plotDistance(iFromX, iFromY, pkAddCity->getX(), pkAddCity->getY());
		bool bAdded = false;
		for (CvCityManager::CityList::iterator itrNearby = kCityList.begin(); itrNearby != kCityList.end(); ++itrNearby)
		{
			CvCity* pkNearbyCity = (*itrNearby);
			int iDistance = plotDistance(iFromX, iFromY, pkNearbyCity->getX(), pkNearbyCity->getY());
			if (iAddDistance <= iDistance)
			{
				kCityList.insert(itrNearby, pkAddCity);
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
			kCityList.push_back(pkAddCity);
	}
}

//	---------------------------------------------------------------------------
void CvCityManager::OnCityCreated(CvCity* pkAddCity)
{
	// Registering the same city twice would append it to every other city's list a second time, and
	// OnCityDestroyed only removes one occurrence per list - the leftover copy would be left dangling
	// once the city is freed. The lists are already correct from the first call, so do nothing.
	if (ms_kCityMap.find(pkAddCity) != ms_kCityMap.end())
		return;

	// First add it to all the other city lists
	for (CityMap::iterator itr = ms_kCityMap.begin(); itr != ms_kCityMap.end(); ++itr)
	{
		CvCity* pkCity = (*itr).first;
		if ((*itr).first != pkAddCity)		// It should not be in there, but check anyhow
		{
			CityList &kCityList = (*itr).second;

			AddToSortedList(kCityList, pkCity->getX(), pkCity->getY(), pkAddCity);
		}
	}

	// Then make a list for it
	std::pair<CityMap::iterator, bool> kResult = ms_kCityMap.insert(CityMap::value_type(pkAddCity, ms_kEmptyList));

	CityList& kCityList = (*kResult.first).second;
	kCityList.reserve(256);		// Reserving an arbitrary number of cities.  Large games can probably have more than this, but this is a good middle ground.

	// Instead of adding all the cities, then sorting the list, this will add them to their correctly sorted position one at a time.
	// This might be a tad slower, but city creation is not happening all the time and if we did the sort, the list would have to store the distance
	int iFromX = pkAddCity->getX();
	int iFromY = pkAddCity->getY();
	for (CityMap::iterator itr = ms_kCityMap.begin(); itr != ms_kCityMap.end(); ++itr)
	{
		CvCity* pkCity = (*itr).first;
		if ((*itr).first != pkAddCity)
		{
			AddToSortedList(kCityList, iFromX, iFromY, pkCity);
		}
	}
}

//	---------------------------------------------------------------------------
void CvCityManager::OnCityDestroyed(CvCity* pkCity)
{
	// Remove it from all the other city lists
	for (CityMap::iterator itr = ms_kCityMap.begin(); itr != ms_kCityMap.end(); ++itr)
	{
		if ((*itr).first != pkCity)
		{
			// erase every occurrence - the city is about to be freed, so a copy left behind here would dangle
			CityList &kCityList = (*itr).second;
			kCityList.erase(std::remove(kCityList.begin(), kCityList.end(), pkCity), kCityList.end());
		}
	}

	// Then remove its list
	CityMap::iterator itr = ms_kCityMap.find(pkCity);
	if (itr != ms_kCityMap.end())
	{
		ms_kCityMap.erase(itr);
	}
}

//	---------------------------------------------------------------------------
const CvCityManager::CityList& CvCityManager::GetNearbyCities(CvCity* pkCity)
{
	ASSERT(pkCity);
	if (pkCity)
	{
		CityMap::const_iterator itr = ms_kCityMap.find(pkCity);

		ASSERT(itr != ms_kCityMap.end());
		if (itr != ms_kCityMap.end())
		{
			return (*itr).second;
		}
	}

	// This should not happen, but I wont' assume.
	return ms_kEmptyList;
}

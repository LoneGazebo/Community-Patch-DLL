/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvInternalGameCoreUtils.h"

//------------------------------------------------------------------------------
bool isWorldWonderClass(const CvBuildingClassInfo& kBuildingClass)
{
	return kBuildingClass.getMaxGlobalInstances() != -1;
}
//------------------------------------------------------------------------------
bool isTeamWonderClass(const CvBuildingClassInfo& kBuildingClass)
{
	return kBuildingClass.getMaxTeamInstances() != -1;
}
//------------------------------------------------------------------------------
bool isNationalWonderClass(const CvBuildingClassInfo& kBuildingClass)
{
	return kBuildingClass.getMaxPlayerInstances() != -1;
}
//------------------------------------------------------------------------------
bool isLimitedWonderClass(const CvBuildingClassInfo& kBuildingClass)
{
	return (isWorldWonderClass(kBuildingClass) || isTeamWonderClass(kBuildingClass) || isNationalWonderClass(kBuildingClass));
}
//------------------------------------------------------------------------------
int limitedWonderClassLimit(const CvBuildingClassInfo& kBuildingClass)
{
	int iCount = 0;
	bool bIsLimited = false;

	int iMax = -1;
	iMax = kBuildingClass.getMaxGlobalInstances();
	if(iMax != -1)
	{
		iCount += iMax;
		bIsLimited = true;
	}

	iMax = kBuildingClass.getMaxTeamInstances();
	if(iMax != -1)
	{
		iCount += iMax;
		bIsLimited = true;
	}

	iMax = kBuildingClass.getMaxPlayerInstances();
	if(iMax != -1)
	{
		iCount += iMax;
		bIsLimited = true;
	}

	return bIsLimited ? iCount : -1;
}
//------------------------------------------------------------------------------
#if defined(MOD_TRADE_WONDER_RESOURCE_ROUTES)
ResourceTypes getWonderResource()
{
	// Find the wonder resource, we don't assume it's Marble, but we do assume there's only one
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++) {
		const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource) {
			if (pkResource->getWonderProductionMod() != 0) {
				return eResource;
			}
		}
	}
	
	return NO_RESOURCE;
}
#endif
/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once

#include "CvDllInterfaces.h"

class CvDllScriptSystemUtility : public ICvScriptSystemUtility1
{
public:
	CvDllScriptSystemUtility();
	~CvDllScriptSystemUtility();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new (size_t bytes);

	void DLLCALL RegisterScriptLibraries(lua_State* L);

	void DLLCALL PushCvCityInstance(lua_State* L, ICvCity1* pkCity);
	ICvCity1* DLLCALL GetCvCityInstance(lua_State* L, int index, bool bErrorOnFail = true);

	void DLLCALL PushCvDealInstance(lua_State* L, ICvDeal1* pkDeal);
	ICvDeal1* DLLCALL GetCvDealInstance(lua_State* L, int index, bool bErrorOnFail = true);

	void DLLCALL PushCvPlotInstance(lua_State* L, ICvPlot1* pkPlot);
	ICvPlot1* DLLCALL GetCvPlotInstance(lua_State* L, int index, bool bErrorOnFail = true);

	void DLLCALL PushCvUnitInstance(lua_State* L, ICvUnit1* pkUnit);
	ICvUnit1* DLLCALL GetCvUnitInstance(lua_State* L, int index, bool bErrorOnFail = true);

	void DLLCALL PushReplayFromStream(lua_State* L, FDataStream& stream);

private:
	void DLLCALL Destroy();
};
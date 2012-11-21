/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllScriptSystemUtility.h"
#include "CvDllContext.h"

#include "CvDllCity.h"
#include "CvDllDeal.h"
#include "CvDllPlot.h"
#include "CvDllUnit.h"

#include "Lua\CvLuaSupport.h"
#include "Lua\CvLuaCity.h"
#include "Lua\CvLuaDeal.h"
#include "Lua\CvLuaPlot.h"
#include "Lua\CvLuaUnit.h"

#include "CvReplayInfo.h"
#include "CvReplayMessage.h"
//////////////////////////////////////////////////////////////////////////
CvDllScriptSystemUtility::CvDllScriptSystemUtility()
{
}
//------------------------------------------------------------------------------
CvDllScriptSystemUtility::~CvDllScriptSystemUtility()
{
}
//------------------------------------------------------------------------------
void* CvDllScriptSystemUtility::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvScriptSystemUtility1::GetInterfaceId())
	{
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::Destroy()
{
	//Do Nothing, object is managed externally.
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllScriptSystemUtility::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::RegisterScriptLibraries(lua_State* L)
{
	LuaSupport::RegisterScriptData(L);
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::PushCvCityInstance(lua_State* L, ICvCity1* pkCity)
{
	if(NULL != L && NULL != pkCity)
	{
		CvDllCity* pkDllCity = static_cast<CvDllCity*>(pkCity);
		CvLuaCity::Push(L, pkDllCity->GetInstance());
	}
	else
	{
		lua_pushnil(L);
	}
}
//------------------------------------------------------------------------------
ICvCity1* CvDllScriptSystemUtility::GetCvCityInstance(lua_State* L, int index, bool bErrorOnFail)
{
	CvCity* pkCity = CvLuaCity::GetInstance(L, index, bErrorOnFail);
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::PushCvDealInstance(lua_State* L, ICvDeal1* pkDeal)
{
	if(NULL != L && NULL != pkDeal)
	{
		CvDllDeal* pkDllDeal = static_cast<CvDllDeal*>(pkDeal);
		CvLuaDeal::Push(L, pkDllDeal->GetInstance());
	}
	else
	{
		lua_pushnil(L);
	}
}
//------------------------------------------------------------------------------
ICvDeal1* CvDllScriptSystemUtility::GetCvDealInstance(lua_State* L, int index, bool bErrorOnFail)
{
	CvDeal* pkDeal = CvLuaDeal::GetInstance(L, index, bErrorOnFail);
	return (NULL != pkDeal)? new CvDllDeal(pkDeal) : NULL;
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::PushCvPlotInstance(lua_State* L, ICvPlot1* pkPlot)
{
	if(NULL != L && NULL != pkPlot)
	{
		CvDllPlot* pkDllPlot = static_cast<CvDllPlot*>(pkPlot);
		CvLuaPlot::Push(L, pkDllPlot->GetInstance());
	}
	else
	{
		lua_pushnil(L);
	}
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllScriptSystemUtility::GetCvPlotInstance(lua_State* L, int index, bool bErrorOnFail)
{
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, index, bErrorOnFail);
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::PushCvUnitInstance(lua_State* L, ICvUnit1* pkUnit)
{
	if(NULL != L && NULL != pkUnit)
	{
		CvDllUnit* pkDllUnit = static_cast<CvDllUnit*>(pkUnit);
		CvLuaUnit::Push(L, pkDllUnit->GetInstance());
	}
	else
	{
		lua_pushnil(L);
	}
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllScriptSystemUtility::GetCvUnitInstance(lua_State* L, int index, bool bErrorOnFail)
{
	CvUnit* pkUnit = CvLuaUnit::GetInstance(L, index, bErrorOnFail);
	return (NULL != pkUnit)? new CvDllUnit(pkUnit) : NULL;
}
//------------------------------------------------------------------------------
void CvDllScriptSystemUtility::PushReplayFromStream(lua_State* L, FDataStream& stream)
{
	CvReplayInfo* pkReplay = FNEW(CvReplayInfo(), c_eMPoolTypeGame, 0);
	if(pkReplay)
	{
		if(!pkReplay->read(stream))
		{
			delete pkReplay;
			pkReplay = NULL;
		}
	}

	if(pkReplay)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lua_pushstring(L, pkReplay->getMapScriptName());
		lua_setfield(L, t, "MapScriptName");

		lua_pushinteger(L, pkReplay->getActivePlayer());
		lua_setfield(L, t, "ActivePlayer");

		lua_pushinteger(L, pkReplay->getWorldSize());
		lua_setfield(L, t, "WorldSize");

		lua_pushinteger(L, pkReplay->getClimate());
		lua_setfield(L, t, "Climate");

		lua_pushinteger(L, pkReplay->getSeaLevel());
		lua_setfield(L, t, "SeaLevel");

		lua_pushinteger(L, pkReplay->getEra());
		lua_setfield(L, t, "Era");

		lua_pushinteger(L, pkReplay->getGameSpeed());
		lua_setfield(L, t, "GameSpeed");

		lua_pushinteger(L, pkReplay->getVictoryType());
		lua_setfield(L, t, "VictoryType");

		lua_pushinteger(L, pkReplay->getGameType());
		lua_setfield(L, t, "GameType");

		lua_pushinteger(L, pkReplay->getInitialTurn());
		lua_setfield(L, t, "InitialTurn");

		lua_pushinteger(L, pkReplay->getFinalTurn());
		lua_setfield(L, t, "FinalTurn");

		lua_pushinteger(L, pkReplay->getStartYear());
		lua_setfield(L, t, "StartYear");

		lua_pushstring(L, pkReplay->getFinalDate());
		lua_setfield(L, t, "FinalDate");

		lua_pushinteger(L, pkReplay->getCalendar());
		lua_setfield(L, t, "Calendar");

		lua_pushinteger(L, pkReplay->getNormalizedScore());
		lua_setfield(L, t, "NormalizedScore");

		lua_pushinteger(L, pkReplay->getMapHeight());
		lua_setfield(L, t, "MapHeight");

		lua_pushinteger(L, pkReplay->getMapWidth());
		lua_setfield(L, t, "MapWidth");

		const int numPlayers = pkReplay->getNumPlayers();
		lua_createtable(L, numPlayers, 0);

		const unsigned int uiNumDataSets = pkReplay->getNumPlayerDataSets();

		for(int iPlayer = 0; iPlayer < numPlayers; iPlayer++)
		{
			lua_createtable(L, 0, 0);
			const int playerIdx = lua_gettop(L);

			lua_pushinteger(L, pkReplay->getPlayerCivilization(iPlayer));
			lua_setfield(L, playerIdx, "Civilization");

			lua_pushinteger(L, pkReplay->getPlayerLeader(iPlayer));
			lua_setfield(L, playerIdx, "Leader");

			lua_pushinteger(L, pkReplay->getPlayerColor(iPlayer));
			lua_setfield(L, playerIdx, "PlayerColor");

			lua_pushinteger(L, pkReplay->getPlayerDifficulty(iPlayer));
			lua_setfield(L, playerIdx, "Difficulty");

			lua_pushstring(L , pkReplay->getPlayerLeaderName(iPlayer));
			lua_setfield(L, playerIdx, "LeaderName");

			lua_pushstring(L, pkReplay->getPlayerCivDescription(iPlayer));
			lua_setfield(L, playerIdx, "CivDescription");

			lua_pushstring(L, pkReplay->getPlayerShortCivDescription(iPlayer));
			lua_setfield(L, playerIdx, "CivShortDescription");

			lua_pushstring(L, pkReplay->getPlayerCivAdjective(iPlayer));
			lua_setfield(L, playerIdx, "CivAdjective");

			lua_createtable(L, 0, 0);
			int iFinalTurn = pkReplay->getFinalTurn();

			for(int iTurn = pkReplay->getInitialTurn(); iTurn <= iFinalTurn; ++iTurn)
			{
				lua_createtable(L, 0, 0);

				for(unsigned int uiDataSet = 0; uiDataSet < uiNumDataSets; ++uiDataSet)
				{
					int iValue = 0;
					if(pkReplay->getPlayerDataSetValue(iPlayer, uiDataSet, iTurn, iValue))
					{
						lua_pushinteger(L, iValue);
						lua_setfield(L, -2, pkReplay->getPlayerDataSetName(uiDataSet));
					}
				}

				lua_rawseti(L, -2, iTurn);
			}
			lua_setfield(L, playerIdx, "Scores");

			lua_rawseti(L, -2, iPlayer + 1);	//Adjust iPlayer to be 1-based instead of 0-based.
		}
		lua_setfield(L, t, "PlayerInfo");

		const int numReplayMessages = pkReplay->getNumReplayMessages();
		lua_createtable(L, numReplayMessages, 0);
		for(int i = 0; i < numReplayMessages; i++)
		{
			const CvReplayMessage* pkMessage = pkReplay->getReplayMessage(i);
			lua_createtable(L, 0, 0);

			lua_pushinteger(L, pkMessage->getTurn());
			lua_setfield(L, -2, "Turn");

			lua_pushinteger(L, pkMessage->getType());
			lua_setfield(L, -2, "Type");

			int iPlayer = pkMessage->getPlayer();
			lua_pushinteger(L, (iPlayer > -1)? iPlayer + 1 : iPlayer);	//Adjust iPlayer to match the 1-based index scheme.
			lua_setfield(L, -2, "Player");

			lua_pushstring(L, pkMessage->getText());
			lua_setfield(L, -2, "Text");

			const int numPlots = pkMessage->getNumPlots();
			lua_createtable(L, numPlots, 0);
			for(int iPlot = 0; iPlot < numPlots; ++iPlot)
			{
				int iPlotX = 0;
				int iPlotY = 0;
				pkMessage->getPlot(iPlot, iPlotX, iPlotY);

				lua_createtable(L, 0, 2);
				lua_pushinteger(L, iPlotX);
				lua_setfield(L, -2, "X");

				lua_pushinteger(L, iPlotY);
				lua_setfield(L, -2, "Y");

				lua_rawseti(L, -2, iPlot + 1);
			}
			lua_setfield(L, -2, "Plots");

			lua_rawseti(L, -2, i);
		}
		lua_setfield(L, t, "Messages");

		const int uiHeight = pkReplay->getMapHeight();
		const int uiWidth = pkReplay->getMapWidth();

		lua_createtable(L, uiHeight * uiWidth, 0);
		int idx = 1;
		for(int uiY = 0; uiY < uiHeight; ++uiY)
		{
			for(int uiX = 0; uiX < uiWidth; ++uiX)
			{
				lua_createtable(L, 0, 0);
				for(int i = pkReplay->getInitialTurn(); i <= pkReplay->getFinalTurn(); ++i)
				{
					CvReplayInfo::PlotState plotState;
					if(pkReplay->getPlotState(uiX, uiY, i, plotState))
					{
						lua_createtable(L, 6, 0);

						lua_pushinteger(L, plotState.m_ePlotType);
						lua_setfield(L, -2, "PlotType");

						lua_pushinteger(L, plotState.m_eTerrain);
						lua_setfield(L, -2, "TerrainType");

						lua_pushinteger(L, plotState.m_eFeature);
						lua_setfield(L, -2, "FeatureType");

						lua_pushinteger(L, plotState.m_bNEOfRiver);
						lua_setfield(L, -2, "NEOfRiver");

						lua_pushinteger(L, plotState.m_bWOfRiver);
						lua_setfield(L, -2, "WOfRiver");

						lua_pushinteger(L, plotState.m_bNWOfRiver);
						lua_setfield(L, -2, "NWOfRiver");

						lua_rawseti(L, -2, i);
					}
				}

				lua_rawseti(L, -2, idx++);
			}
		}
		lua_setfield(L, t, "Plots");

		delete pkReplay;
	}
}
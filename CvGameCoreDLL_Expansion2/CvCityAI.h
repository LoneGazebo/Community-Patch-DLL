/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// cityAI.h

#ifndef CIV5_CITY_AI_H
#define CIV5_CITY_AI_H

#include "CvCity.h"
#include "CvUnit.h"

class CvCityAI : public CvCity
{
public:
	CvCityAI();
	virtual ~CvCityAI();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_doTurn();
	void AI_chooseProduction(bool bInterruptWonders, bool bInterruptBuildings = false);
	bool AI_isChooseProductionDirty();
	void AI_setChooseProductionDirty(bool bNewValue);

	int AI_GetNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer) const;
	void AI_ChangeNumPlotsAcquiredByOtherPlayer(PlayerTypes ePlayer, int iChange);

	void AI_DoEventChoice(CityEventTypes eEvent);

	template<typename CityAI, typename Visitor>
	static void Serialize(CityAI& cityAI, Visitor& visitor);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

protected:
	bool m_bChooseProductionDirty;
	int m_iCachePlayerClosenessTurn;
	int m_iCachePlayerClosenessDistance;
	map<PlayerTypes,int> m_mapPlotsAcquiredByOtherPlayers;
};

FDataStream& operator<<(FDataStream&, const CvCityAI&);
FDataStream& operator>>(FDataStream&, CvCityAI&);

#endif

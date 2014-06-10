/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// playerAI.h

#ifndef CIV5_PLAYER_AI_H
#define CIV5_PLAYER_AI_H

#include "CvPlayer.h"

class CvEventTriggerInfo;

class CvPlayerAI : public CvPlayer
{
public:
	CvPlayerAI();
	virtual ~CvPlayerAI();

  // inlined for performance reasons
  static CvPlayerAI& getPlayer(PlayerTypes ePlayer)
  {
	  CvAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
	  CvAssertMsg(ePlayer < MAX_PLAYERS, "Player is not assigned a valid value");
	  return m_aPlayers[ePlayer];
  }

	static void initStatics();
	static void freeStatics();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_doTurnPre();
	void AI_doTurnPost();
	void AI_doTurnUnitsPre();
	void AI_doTurnUnitsPost();

	void AI_updateFoundValues(bool bStartingLoc = false);

	void AI_unitUpdate();

	void AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner);
	bool AI_captureUnit(UnitTypes eUnit, CvPlot* pPlot);

	int AI_foundValue(int iX, int iY, int iMinRivalRange = -1, bool bStartingLoc = false);

	void AI_chooseFreeGreatPerson();
	void AI_chooseFreeTech();
	void AI_chooseResearch();

	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int iRange = 0);

	virtual void AI_launch(VictoryTypes eVictory);

	void ProcessGreatPeople (void);
	GreatPeopleDirectiveTypes GetDirectiveArtist    (CvUnit* pGreatArtist);
	GreatPeopleDirectiveTypes GetDirectiveEngineer  (CvUnit* pGreatEngineer);
	GreatPeopleDirectiveTypes GetDirectiveMerchant  (CvUnit* pGreatMerchant);
	GreatPeopleDirectiveTypes GetDirectiveScientist (CvUnit* pGreatScientist);
	GreatPeopleDirectiveTypes GetDirectiveGeneral   (CvUnit* pGreatGeneral);

	CvPlot* FindBestMerchantTargetPlot (CvUnit* pGreatMerchant, bool bOnlySafePaths);
	CvPlot* FindBestArtistTargetPlot (CvUnit* pGreatArtist, int& iScore);

	// this are used by a city AI to signal that it is committing to build a unit for one of my operations
	virtual OperationSlot PeekAtNextUnitToBuildForOperationSlot(int iAreaID);
	virtual OperationSlot CityCommitToBuildUnitForOperationSlot(int iAreaID, int iTurns, CvCity *pCity);
	virtual void CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot);
	virtual void CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit);
	virtual int GetNumUnitsNeededToBeBuilt();

	// for serialization
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

protected:

	static CvPlayerAI* m_aPlayers;

	void AI_doResearch();
};

// helper for accessing static functions
#define GET_PLAYER CvPlayerAI::getPlayer

#endif

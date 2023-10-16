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

	static CvPlayerAI& getPlayer(PlayerTypes ePlayer);
	static bool IsValid(PlayerTypes ePlayer)
	{
		return ((int)ePlayer >= 0 && (int)ePlayer < MAX_PLAYERS);
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

	void AI_unitUpdate();
	void AI_conquerCity(CvCity* pCity, bool bGift, bool bAllowSphereRemoval);

	void AI_chooseFreeGreatPerson();
	void AI_chooseFreeTech();
	void AI_chooseResearch();

	void AI_considerAnnex();
	void AI_considerRaze();
	int AI_computeHappinessFromRazing(CvCity* pCity, int iCurrentHappy, int iCurrentUnhappy);

	virtual void AI_launch(VictoryTypes eVictory);

	void ProcessGreatPeople(void);
	GreatPeopleDirectiveTypes GetDirectiveWriter(CvUnit* pGreatWriter);
	GreatPeopleDirectiveTypes GetDirectiveArtist(CvUnit* pGreatArtist);
	GreatPeopleDirectiveTypes GetDirectiveMusician(CvUnit* pGreatMusician);
	GreatPeopleDirectiveTypes GetDirectiveEngineer(CvUnit* pGreatEngineer);
	GreatPeopleDirectiveTypes GetDirectiveMerchant(CvUnit* pGreatMerchant);
	GreatPeopleDirectiveTypes GetDirectiveScientist(CvUnit* pGreatScientist);
	GreatPeopleDirectiveTypes GetDirectiveGeneral(CvUnit* pGreatGeneral);
	GreatPeopleDirectiveTypes GetDirectiveProphet(CvUnit* pGreatProphet);
	GreatPeopleDirectiveTypes GetDirectiveAdmiral(CvUnit* pGreatAdmiral);
	GreatPeopleDirectiveTypes GetDirectiveDiplomat(CvUnit* pGreatDiplomat);

	bool GreatMerchantWantsCash();
	CvPlot* FindBestMerchantTargetPlotForCash(CvUnit* pGreatMerchant);
	CvPlot* FindBestMerchantTargetPlotForPuppet(CvUnit* pGreatMerchant);

	//For Great Diplomats
	CvPlot* FindBestDiplomatTargetPlot(CvUnit* pUnit);
	//And for messengers
	CvCity* FindBestMessengerTargetCity(CvUnit* pUnit, const vector<int>& vIgnoreCities);
	int ScoreCityForMessenger(CvCity* pCity, CvUnit* pUnit);
	CvPlot* ChooseMessengerTargetPlot(CvUnit* pUnit, vector<int>* pvIgnoreCities = NULL);

	CvPlot* FindBestCultureBombPlot(CvUnit* pUnit, BuildTypes eBuild, const std::vector<CvPlot*>& vPlotsToAvoid, bool bMustBeWorkable);
	CvPlot* FindBestMusicianTargetPlot(CvUnit* pGreatMerchant);

	// this are used by a city AI to signal that it is committing to build a unit for one of my operations
	virtual OperationSlot PeekAtNextUnitToBuildForOperationSlot(CvCity* pCity, bool& bCitySameAsMuster);
	virtual void CityCommitToBuildUnitForOperationSlot(OperationSlot thisSlot);
	virtual void CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot);
	virtual void CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit);
	virtual int GetNumUnitsNeededToBeBuilt();

#if defined(MOD_BALANCE_CORE_EVENTS)
	void AI_DoEventChoice(EventTypes eEvent);
	bool AI_DoEspionageEventChoice(CityEventTypes eEvent, int uiSpyIndex, CvCity* pCity);
#endif

	// for serialization
	template<typename PlayerAI, typename Visitor>
	static void Serialize(PlayerAI& playerAI, Visitor& visitor);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

protected:
	void AI_doResearch();
};

// helper for accessing static functions
#define GET_PLAYER CvPlayerAI::getPlayer

#endif

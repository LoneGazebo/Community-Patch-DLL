/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//	CvAchievementUnlocker
//
//	Utility methods to test the parameters for individual achievements.
//	Most of the tests in this file are for achievements that have multiple parameters
//	that can be achieved in any order.  This keeps the code in a single place.
//	It is not strictly necessary to put all achievement tests in this file especially if the
//	test requires many parameters or is adequately tested in a single location in the game core.

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvAchievementUnlocker.h"
#include "CvMinorCivAI.h"

#ifdef FINAL_RELEASE
#define DEBUG_RELEASE_VALUE(a, b) (b)
#else
#define DEBUG_RELEASE_VALUE(a, b) (a)
#endif

#define UNDEFINED_TYPE (-999)

int CvAchievementUnlocker::ms_iNumImprovementsPillagedPerTurn = 0;
//	---------------------------------------------------------------------------
//	Test the conditions for the ACHIEVEMENT_PSG
bool CvAchievementUnlocker::Check_PSG()
{
	if (!MOD_API_ACHIEVEMENTS)
		return false;

	const int PSG_STAT_MATCH_VALUE = 100;

	int32 iGeneralsStat = 0;
	int32 iWinsStat = 0;
	int32 iArmyStat = 0;
	if(gDLL->GetSteamStat(ESTEAMSTAT_GREATGENERALS, &iGeneralsStat) &&
	        gDLL->GetSteamStat(ESTEAMSTAT_TOTAL_WINS, &iWinsStat) &&
	        gDLL->GetSteamStat(ESTEAMSTAT_STANDINGARMY, &iArmyStat))
	{
		if(iGeneralsStat >= PSG_STAT_MATCH_VALUE && iWinsStat >= PSG_STAT_MATCH_VALUE && iArmyStat >= PSG_STAT_MATCH_VALUE)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_PSG);
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------
void CvAchievementUnlocker::FarmImprovementPillaged()
{
	if (!MOD_API_ACHIEVEMENTS)
		return;

	ms_iNumImprovementsPillagedPerTurn++;

	if (ms_iNumImprovementsPillagedPerTurn >= 9)
		gDLL->UnlockAchievement(ACHIEVEMENT_SCENARIO_04_PILLAGE);
}
//------------------------------------------------------------------------------
void CvAchievementUnlocker::EndTurn()
{
	//Reset the per turn counters.
	ms_iNumImprovementsPillagedPerTurn = 0;
}

//------------------------------------------------------------------------------
void CvAchievementUnlocker::AlexanderConquest(PlayerTypes ePlayer)
{
	if (!MOD_API_ACHIEVEMENTS)
		return;

	//Test For Alexander Conquest
	CvGame& kGame = GC.getGame();
	if (ePlayer == kGame.getActivePlayer())
	{
		CvString szLeaderName = (CvString)GET_PLAYER(ePlayer).getLeaderTypeKey();
		if(szLeaderName == "LEADER_ALEXANDER")
		{
			if(kGame.getGameTurnYear() <= 350)
			{
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
				{
					CvPlayer* pPlayer = &GET_PLAYER((PlayerTypes) iPlayerLoop);

					//All known players must be dead and killed by us
					if(GET_TEAM(pPlayer->getTeam()).isHasMet(GET_PLAYER(kGame.getActivePlayer()).getTeam()))
					{
						if(!pPlayer->isBarbarian() && !pPlayer->isMinorCiv())
						{
							if(pPlayer->isAlive() && pPlayer->GetID() != GET_PLAYER(kGame.getActivePlayer()).GetID())
							{
								return;	// Nope.
							}
						}
					}
				}
				// Yep.
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_CONQUEST_WORLD);
			}
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CvAchievementUnlocker::UnlockFromDatabase()
{
	if (!MOD_API_ACHIEVEMENTS)
		return;

	GUID guid;

	ExtractGUID("7459BA32-5764-44ae-8E95-01AD0E0EFD48", guid);
	bool bDLC_DX = gDLL->IsDLCValid(guid); if (bDLC_DX) CUSTOMLOG("Found DLC Deluxe");
	ExtractGUID("293C1EE3-1176-44f6-AC1F-59663826DE74", guid);
	bool bDLC_01 = gDLL->IsDLCValid(guid); if (bDLC_01) CUSTOMLOG("Found DLC 1");
	ExtractGUID("B685D5DE-7CCA-4e75-81B4-2F60754E6330", guid);
	bool bDLC_02 = gDLL->IsDLCValid(guid); if (bDLC_02) CUSTOMLOG("Found DLC 2");
	ExtractGUID("ECF7C605-BA11-4CAC-8D80-D71306AAC471", guid);
	bool bDLC_03 = gDLL->IsDLCValid(guid); if (bDLC_03) CUSTOMLOG("Found DLC 3");
	ExtractGUID("B3030D39-C0D8-4bc7-91B1-7AD1CAF585AB", guid);
	bool bDLC_04 = gDLL->IsDLCValid(guid); if (bDLC_04) CUSTOMLOG("Found DLC 4");
	ExtractGUID("112C22B2-5308-42b6-B734-171CCAB3037B", guid);
	bool bDLC_05 = gDLL->IsDLCValid(guid); if (bDLC_05) CUSTOMLOG("Found DLC 5");
	ExtractGUID("BBB0D085-A0B1-4475-B007-3E549CF3ADC3", guid);
	bool bDLC_06 = gDLL->IsDLCValid(guid); if (bDLC_06) CUSTOMLOG("Found DLC 6");
	ExtractGUID("EA67AED5-5859-4875-BF3A-360FE9E55D1B", guid);
	bool bDLC_07 = gDLL->IsDLCValid(guid); if (bDLC_07) CUSTOMLOG("Found DLC 7");
	ExtractGUID("0E3751A1-F840-4e1b-9706-519BF484E59D", guid);
	bool bEXP_01 = gDLL->IsDLCValid(guid); if (bEXP_01) CUSTOMLOG("Found G&K");
	ExtractGUID("6DA07636-4123-4018-B643-6575B4EC336B", guid);
	bool bEXP_02 = gDLL->IsDLCValid(guid); if (bEXP_02) CUSTOMLOG("Found BNW");

	for (int iAchievement = 0; iAchievement < NUM_ACHIEVEMENTS; iAchievement++) {
		EAchievement eAchievement = (EAchievement) iAchievement;

		// Multiplayer achievements - can't have these!
		if (eAchievement == ACHIEVEMENT_WIN_MULTIPLAYER)
			continue;

		// Broken achievements - can't have these!
		if (   eAchievement == ACHIEVEMENT_HUNDRED_WAR
		    || eAchievement == ACHIEVEMENT_CATFIGHT
		    || eAchievement == ACHIEVEMENT_MANIFEST_DESTINY
		    || eAchievement == ACHIEVEMENT_UNIT_20BATTLES
		    || eAchievement == ACHIEVEMENT_ALL_WAR
		    || eAchievement == ACHIEVEMENT_PEACEFULPLAY
		    || eAchievement == ACHIEVEMENT_LOSE_MULTIPLAYER
		    || eAchievement == ACHIEVEMENT_SIX_DEGREES
		    || eAchievement == ACHIEVEMENT_DEFPACT
		    || eAchievement == ACHIEVEMENT_RESEARCHPACT
		    || eAchievement == ACHIEVEMENT_SPECIAL_ELEPHANTBATTLE
		    || eAchievement == ACHIEVEMENT_CIVILOPEDIA
		    || eAchievement == ACHIEVEMENT_SPECIAL_HWATCH_OUT
		    || eAchievement == ACHIEVEMENT_SCENARIO_05_QING_TAKES_MING
		    || eAchievement == ACHIEVEMENT_SPECIAL_ANCIENT_WONDERS
		    || eAchievement == ACHIEVEMENT_XP1_49)
			continue;

		// DLC/Expansion achievements - can only have these if own the DLC/Expansion
		if (   (eAchievement >= ACHIEVEMENT_WIN_NEBUCHADNEZZAR && eAchievement <= ACHIEVEMENT_WIN_NEBUCHADNEZZAR && !bDLC_DX)
		    || (eAchievement >= ACHIEVEMENT_WIN_GENGHIS && eAchievement <= ACHIEVEMENT_LOSE_SCENARIO_01 && !bDLC_01)
		    || (eAchievement >= ACHIEVEMENT_WIN_ISABELLA && eAchievement <= ACHIEVEMENT_SCENARIO_02_RETURN_TREASURE && !(bDLC_02 || bDLC_07))
		    || (eAchievement >= ACHIEVEMENT_WIN_KAMEHAMEHA && eAchievement <= ACHIEVEMENT_SCENARIO_03_FIND_NEW_ZEALAND && !bDLC_03)
		    || (eAchievement >= ACHIEVEMENT_WIN_BLUETOOTH && eAchievement <= ACHIEVEMENT_SCENARIO_04_WIN_DEITY && !bDLC_04)
		    || (eAchievement >= ACHIEVEMENT_WIN_SEJONG && eAchievement <= ACHIEVEMENT_SCENARIO_05_QING_TAKES_MING && !bDLC_05)
		    || (eAchievement >= ACHIEVEMENT_SPECIAL_ROME_GETS_ZEUS && eAchievement <= ACHIEVEMENT_SCENARIO_06_ORACLE_CONSULT && !bDLC_06)
		    || (eAchievement >= ACHIEVEMENT_XP1_01 && eAchievement <= ACHIEVEMENT_XP1_52 && !bEXP_01)
		    || (eAchievement >= ACHIEVEMENT_XP2_01 && eAchievement <= ACHIEVEMENT_XP2_60 && !bEXP_02))
			continue;

		CvAchievementInfo* achievementInfo = GC.getAchievementInfo(eAchievement);

		if(achievementInfo == NULL)
			continue;

		if (achievementInfo->isAchieved()) {
			if (!gDLL->IsAchievementUnlocked(eAchievement)) {
				CUSTOMLOG("Unlocking: %i", eAchievement);
				gDLL->UnlockAchievement(eAchievement);
			}
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CvPlayerAchievements::CvPlayerAchievements(const CvPlayer& kPlayer)
: m_kPlayer(kPlayer)
, m_iAchievement_XP1_32_Progress(0)
, m_iAchievement_XP1_33_Progress(0)
, m_ePapalPrimacyType(static_cast<BeliefTypes>(UNDEFINED_TYPE))
, m_eCollossusType(static_cast<BuildingTypes>(UNDEFINED_TYPE))
, m_ePetraType(static_cast<BuildingTypes>(UNDEFINED_TYPE))
, m_eCaravansaryType(static_cast<BuildingTypes>(UNDEFINED_TYPE))
, m_eHarborType(static_cast<BuildingTypes>(UNDEFINED_TYPE))
, m_eEthiopiaType(static_cast<CivilizationTypes>(UNDEFINED_TYPE))
, m_eByzantinesType(static_cast<CivilizationTypes>(UNDEFINED_TYPE))
, m_eGreekType(static_cast<CivilizationTypes>(UNDEFINED_TYPE))
, m_eCarthageType(static_cast<CivilizationTypes>(UNDEFINED_TYPE))
, m_eRomeType(static_cast<CivilizationTypes>(UNDEFINED_TYPE))
, m_eSwedenType(static_cast<CivilizationTypes>(UNDEFINED_TYPE))
, m_eDromonType(static_cast<UnitTypes>(UNDEFINED_TYPE))
, m_eGreatProphetType(static_cast<UnitTypes>(UNDEFINED_TYPE))
, m_eAfricanForestElephantType(static_cast<UnitTypes>(UNDEFINED_TYPE))
, m_eGreatGeneralType(static_cast<UnitTypes>(UNDEFINED_TYPE))
, m_eHakkapelittaType(static_cast<UnitTypes>(UNDEFINED_TYPE))
, m_eSkyFortress(static_cast<UnitTypes>(UNDEFINED_TYPE))
{
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::AlliedWithCityState(PlayerTypes eNewCityStateAlly)
{
	if (!MOD_API_ACHIEVEMENTS || m_kPlayer.GetID() != GC.getGame().getActivePlayer())
		return;

	//Cache value if needed
	if(m_ePapalPrimacyType == UNDEFINED_TYPE)
	{
		m_ePapalPrimacyType = (BeliefTypes)GC.getInfoTypeForString("BELIEF_PAPAL_PRIMACY", true);
	}
	
	if(m_ePapalPrimacyType != NO_BELIEF)
	{
		const ReligionTypes eReligion = m_kPlayer.GetReligions()->GetReligionCreatedByPlayer();
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, m_kPlayer.GetID());
			if(pReligion != NULL)
			{
				if(pReligion->m_Beliefs.HasBelief(m_ePapalPrimacyType))
				{
					int iNumAllies = 0;
					//We've got the belief! How many city state Allies do we have??
					for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(i);
						CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
						if(kPlayer.isAlive() && kPlayer.isMinorCiv())
						{
							const PlayerTypes eAlly = kPlayer.GetMinorCivAI()->GetAlly();
							if(eAlly == m_kPlayer.GetID())
								iNumAllies++;
						}
					}

					if(iNumAllies >= 12)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_27);
					}
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::AddUnit(CvUnit* pUnit)
{
	if (!MOD_API_ACHIEVEMENTS || m_kPlayer.GetID() != GC.getGame().getActivePlayer())
		return;

	//Attempt to read from cache
	if(m_eEthiopiaType == UNDEFINED_TYPE)
	{
		m_eEthiopiaType = (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ETHIOPIA", true);
	}

	if(m_eGreatProphetType == UNDEFINED_TYPE)
	{
		m_eGreatProphetType = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET", true);
	}

	if(m_eEthiopiaType != NO_CIVILIZATION && m_eGreatProphetType != NO_UNIT)
	{
		//* ACHIEVEMENT_XP1_32 - As Ethiopia, earn 5 great prophets
		if(m_kPlayer.getCivilizationType() == m_eEthiopiaType && pUnit->getUnitType() == m_eGreatProphetType)
		{
			m_iAchievement_XP1_32_Progress++;
			if(m_iAchievement_XP1_32_Progress >= 5)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP1_32);
			}
		}	
	}
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::AttackedUnitWithUnit(CvUnit* pAttackingUnit, CvUnit* pDefendingUnit)
{
	if (!MOD_API_ACHIEVEMENTS || m_kPlayer.GetID() != GC.getGame().getActivePlayer())
		return;

	if(m_eCarthageType == UNDEFINED_TYPE)
	{
		m_eCarthageType = (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_CARTHAGE", true);
	}

	if(m_eRomeType == UNDEFINED_TYPE)
	{
		m_eRomeType = (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ROME", true);
	}

	if(m_eAfricanForestElephantType == UNDEFINED_TYPE)
	{
		m_eAfricanForestElephantType = (UnitTypes)GC.getInfoTypeForString("UNIT_CARTHAGINIAN_FOREST_ELEPHANT", true);
	}
	
	if(m_eCarthageType != NO_CIVILIZATION && m_eRomeType != NO_CIVILIZATION && m_eAfricanForestElephantType != NO_UNIT)
	{
		if(pAttackingUnit->getCivilizationType() == m_eCarthageType &&
			pAttackingUnit->getUnitType() == m_eAfricanForestElephantType &&
			pAttackingUnit->plot()->isMountain() &&
			pDefendingUnit->getCivilizationType() == m_eRomeType)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_29);
		}
	}
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::BoughtCityState(int iNumUnits)
{
	if (!MOD_API_ACHIEVEMENTS || iNumUnits < 15)
		return;

	gDLL->UnlockAchievement(ACHIEVEMENT_XP1_35);
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::KilledUnitWithUnit(CvUnit* pKillingUnit, CvUnit* pKilledUnit)
{
	if (!MOD_API_ACHIEVEMENTS || m_kPlayer.GetID() != GC.getGame().getActivePlayer())
		return;

	if(m_eByzantinesType == UNDEFINED_TYPE)
	{
		m_eByzantinesType = (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_BYZANTIUM", true);
	}

	if(m_eGreekType == UNDEFINED_TYPE)
	{
		m_eGreekType = (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_GREECE", true);
	}

	if(m_eDromonType == UNDEFINED_TYPE)
	{
		m_eDromonType = (UnitTypes)GC.getInfoTypeForString("UNIT_BYZANTINE_DROMON", true);
	}

	if(m_eDromonType != NO_UNIT && m_eByzantinesType != NO_CIVILIZATION && m_eGreekType != NO_CIVILIZATION)
	{
		//* ACHIEVEMENT_XP1_33 - As Byzantines, sink 10 Greek ships with a Dromon
		if(	pKillingUnit->getCivilizationType() == m_eByzantinesType && 
			pKillingUnit->getUnitType() == m_eDromonType &&
			pKilledUnit->getCivilizationType() == m_eGreekType &&
			pKilledUnit->getDomainType() == DOMAIN_SEA)
		{
			m_iAchievement_XP1_33_Progress++;
			if(m_iAchievement_XP1_33_Progress >= 10)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP1_33);
			}
		}
	}

	bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);
	if(bUsingXP1Scenario3)
	{
		if(m_eSkyFortress == UNDEFINED_TYPE)
		{
			m_eSkyFortress = (UnitTypes)GC.getInfoTypeForString("UNIT_SKY_FORTRESS" , true);
		}

		if(pKillingUnit->getDomainType() == DOMAIN_AIR)
		{
			const int iX = pKillingUnit->getX();
			const int iY = pKillingUnit->getY();

			CvPlot* pkPlot = GC.getMap().plot(iX, iY);
			if(pkPlot != NULL)
			{
				const int iNumUnits = pkPlot->getNumUnits();
				for(int i = 0; i < iNumUnits; ++i)
				{
					CvUnit* pUnit = pkPlot->getUnitByIndex(i);
					if(pUnit && pUnit->getUnitType() == m_eSkyFortress)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_46);
					}
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::StartTurn()
{
	if (!MOD_API_ACHIEVEMENTS || m_kPlayer.GetID() != GC.getGame().getActivePlayer())
		return;

	if(m_eSwedenType == UNDEFINED_TYPE)
	{
		m_eSwedenType = (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_SWEDEN", true);
	}

	if(m_eGreatGeneralType == UNDEFINED_TYPE)
	{
		m_eGreatGeneralType = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL", true);
	}

	if(m_eHakkapelittaType == UNDEFINED_TYPE)
	{
		m_eHakkapelittaType = (UnitTypes)GC.getInfoTypeForString("UNIT_SWEDISH_HAKKAPELIITTA", true);
	}

	if(m_eSwedenType != NO_CIVILIZATION && m_eGreatGeneralType != NO_UNIT && m_eHakkapelittaType != NO_UNIT)
	{
		if(m_kPlayer.getCivilizationType() == m_eSwedenType)
		{
			int iLoop = 0;
			for(const CvUnit* pUnit = m_kPlayer.firstUnit(&iLoop); pUnit != NULL; pUnit = m_kPlayer.nextUnit(&iLoop))
			{
				if(pUnit->getUnitType() == m_eGreatGeneralType)
				{
					CvPlot* pPlot = pUnit->plot();
					if(pPlot != NULL)
					{
						int iNumUnits = pPlot->getNumUnits();
						for(int i = 0; i < iNumUnits; ++i)
						{
							CvUnit* pStackedUnit = pPlot->getUnitByIndex(i);
							if(pStackedUnit->getUnitType() == m_eHakkapelittaType)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_31);
								return;
							}
						}
					}	
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::EndTurn()
{
}
//-------------------------------------------------------------------------
void CvPlayerAchievements::FinishedBuilding(CvCity* pkCity, BuildingTypes eBuilding)
{
	if (!MOD_API_ACHIEVEMENTS)
		return;

	if(m_eCollossusType == UNDEFINED_TYPE)
	{
		m_eCollossusType = (BuildingTypes)GC.getInfoTypeForString("BUILDING_COLOSSUS", true);
	}

	if(m_ePetraType == UNDEFINED_TYPE)
	{
		m_ePetraType = (BuildingTypes)GC.getInfoTypeForString("BUILDING_PETRA", true);
	}

	if(m_eCaravansaryType == UNDEFINED_TYPE)
	{
		m_eCaravansaryType = (BuildingTypes)GC.getInfoTypeForString("BUILDING_CARAVANSARY", true);
	}

	if(m_eHarborType == UNDEFINED_TYPE)
	{
		m_eHarborType = (BuildingTypes)GC.getInfoTypeForString("BUILDING_HARBOR", true);
	}

	if(m_kPlayer.isLocalPlayer() && m_kPlayer.isHuman() && m_eCollossusType != NO_BUILDING && m_ePetraType != NO_BUILDING && m_eCaravansaryType != NO_BUILDING && m_eHarborType != NO_BUILDING)
	{
		CvCityBuildings* pkBuildings = pkCity->GetCityBuildings();
		if(pkBuildings != NULL)
		{
			if(pkBuildings->GetNumBuilding(m_eCollossusType) > 0 &&
				pkBuildings->GetNumBuilding(m_ePetraType) > 0 &&
				pkBuildings->GetNumBuilding(m_eCaravansaryType) > 0 &&
				pkBuildings->GetNumBuilding(m_eHarborType) > 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_30);
			}
		}
	}
}
//------------------------------------------------------------------------------
template<typename PlayerAchievements, typename Visitor>
void CvPlayerAchievements::Serialize(PlayerAchievements& playerAchievements, Visitor& visitor)
{
	visitor(playerAchievements.m_iAchievement_XP1_32_Progress);
	visitor(playerAchievements.m_iAchievement_XP1_33_Progress);
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}
//------------------------------------------------------------------------------
void CvPlayerAchievements::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}
//------------------------------------------------------------------------------

FDataStream& operator>>(FDataStream& stream, CvPlayerAchievements& playerAchievements)
{
	playerAchievements.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvPlayerAchievements& playerAchievements)
{
	playerAchievements.Write(stream);
	return stream;
}
﻿/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"
#include "CvUnitCombat.h"
#include "CvUnitMission.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvDiplomacyAI.h"
#include "CvTypes.h"

#include "CvDllCity.h"
#include "CvDllUnit.h"
#include "CvDllCombatInfo.h"

// include this after all other headers
#include "LintFree.h"

// Maximum damage members for the nuke, units and cities
#define MAX_NUKE_DAMAGE_MEMBERS	64

#define POST_QUICK_COMBAT_DELAY	110
#define POST_COMBAT_DELAY		1

//	---------------------------------------------------------------------------
static int GetPostCombatDelay()
{
	return CvPreGame::quickCombat()?POST_QUICK_COMBAT_DELAY:POST_COMBAT_DELAY;
}

//	---------------------------------------------------------------------------
// Find a object in the combat member array
static CvCombatMemberEntry* FindCombatMember(CvCombatMemberEntry* pkArray, int iMembers, IDInfo kMember, CvCombatMemberEntry::MEMBER_TYPE eType)
{
	if(iMembers > 0)
	{
		while(iMembers--)
		{
			if(pkArray->IsType(eType))
			{
				if(pkArray->GetID() == kMember.iID && pkArray->GetPlayer() == kMember.eOwner)
					return pkArray;
			}
			++pkArray;
		}
	}

	return NULL;
}

//	---------------------------------------------------------------------------
// Add a member to the combat array
static CvCombatMemberEntry* AddCombatMember(CvCombatMemberEntry* pkArray, int* piMembers, int iMaxMembers, IDInfo kMember, CvCombatMemberEntry::MEMBER_TYPE eType, int iX, int iY, EraTypes eEra)
{
	if(*piMembers < iMaxMembers)
	{
		int iCount = *piMembers;
		if(!FindCombatMember(pkArray, iCount, kMember, eType))
		{
			CvCombatMemberEntry& kEntry = pkArray[iCount];
			kEntry.SetPlayer(kMember.eOwner);
			kEntry.SetID(kMember.iID, eType);
			kEntry.SetDamage(0);
			kEntry.SetFinalDamage(0);
			kEntry.SetMaxHitPoints(0);
			kEntry.SetPlot(iX, iY);
			kEntry.SetEra(eEra);
			*piMembers += 1;
			return &(pkArray[iCount]);
		}
	}
	return NULL;
}

//	---------------------------------------------------------------------------
// Add a unit member to the combat array
static CvCombatMemberEntry* AddCombatMember(CvCombatMemberEntry* pkArray, int* piMembers, int iMaxMembers, CvUnit* pkMember)
{
	if(pkMember)
		return AddCombatMember(pkArray, piMembers, iMaxMembers, pkMember->GetIDInfo(), CvCombatMemberEntry::MEMBER_UNIT, pkMember->getX(), pkMember->getY(), GET_PLAYER(pkMember->getOwner()).GetCurrentEra());

	return NULL;
}

//	---------------------------------------------------------------------------
// Add a city member to the combat array
static CvCombatMemberEntry* AddCombatMember(CvCombatMemberEntry* pkArray, int* piMembers, int iMaxMembers, CvCity* pkMember)
{
	if(pkMember)
		return AddCombatMember(pkArray, piMembers, iMaxMembers, pkMember->GetIDInfo(), CvCombatMemberEntry::MEMBER_CITY, pkMember->getX(), pkMember->getY(), GET_PLAYER(pkMember->getOwner()).GetCurrentEra());

	return NULL;
}

//	---------------------------------------------------------------------------
void CvUnitCombat::GenerateMeleeCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& plot, CvCombatInfo* pkCombatInfo)
{
	BATTLE_STARTED(BATTLE_TYPE_MELEE, plot);

	bool bIncludeRand = !GC.getGame().isGameMultiPlayer();

	pkCombatInfo->setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, pkDefender);
	pkCombatInfo->setHasMoved(BATTLE_UNIT_ATTACKER, kAttacker.hasMoved());
	pkCombatInfo->setPlot(&plot);

	// Attacking a City
	if(plot.isCity())
	{
		// Unit vs. City (non-ranged so the city will retaliate)
		CvCity* pkCity = plot.getPlotCity();
		BATTLE_JOINED(pkCity, BATTLE_UNIT_DEFENDER, true);

		int iAttackerStrength = kAttacker.GetMaxAttackStrength(kAttacker.plot(), &plot, NULL);

		CvUnit* pGarrison = pkCity->GetGarrisonedUnit();
		int iGarrisonMaxHP = 0;
		if (pGarrison && !pGarrison->IsDead())
			iGarrisonMaxHP = pGarrison->GetMaxHitPoints();

		int iDefenderDamageInflicted = 0;
		int iGarrisonShare = 0;
		int iAttackerDamageInflicted = kAttacker.getMeleeCombatDamageCity(iAttackerStrength, pkCity, iDefenderDamageInflicted, iGarrisonMaxHP, iGarrisonShare, bIncludeRand);

		if (pGarrison && iGarrisonShare > 0)
		{
			//add the garrison as a bystander
			int iDamageMembers = 0;
			CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkCombatInfo->getDamageMembers(), &iDamageMembers, pkCombatInfo->getMaxDamageMemberCount(), pGarrison);
			if(pkDamageEntry)
			{
#if defined(MOD_EVENTS_BATTLES)
				BATTLE_JOINED(pGarrison, BATTLE_UNIT_COUNT, false); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER
#endif
				pkDamageEntry->SetDamage(iGarrisonShare);
				pkDamageEntry->SetFinalDamage(std::min(iGarrisonShare + pGarrison->getDamage(), pGarrison->GetMaxHitPoints()));
				pkDamageEntry->SetMaxHitPoints(pGarrison->GetMaxHitPoints());
				pkCombatInfo->setDamageMemberCount(iDamageMembers);
			}
		}

		// Finally, cap the damage dealt to the city at its current health
		iAttackerDamageInflicted = min(iAttackerDamageInflicted, pkCity->GetMaxHitPoints() - pkCity->getDamage());

		pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderDamageInflicted + kAttacker.getDamage());
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);
		pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerDamageInflicted + pkCity->getDamage());
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);

		int iExperience = /*5*/ GD_INT_GET(EXPERIENCE_ATTACKING_CITY_MELEE);
		pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, pkCity->maxXPValue());
		pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == pkCity->getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
		}
		else
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !kAttacker.isBarbarian());
		}
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif
		pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, 0);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, kAttacker.maxXPValue());
		pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, true);
		}
		else
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !pkCity->isBarbarian()); // Kind of irrelevant, as cities don't get XP!
		}
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !pkCity->isBarbarian());
#endif
		pkCombatInfo->setAttackIsRanged(false);
		pkCombatInfo->setDefenderRetaliates(true);
	}
	// Attacking a Unit
	else if (pkDefender)
	{
		int iDefenderStrength = pkDefender->GetMaxDefenseStrength(&plot, &kAttacker, kAttacker.plot());
		int iAttackerStrength = kAttacker.GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true);
		if(iAttackerStrength > 0 && kAttacker.getDomainType() == DOMAIN_AIR)
		{
			if(pkDefender->getDomainType() != DOMAIN_AIR)
			{
				iDefenderStrength /= 2;
			}
		}
		else
		{
			iAttackerStrength = kAttacker.GetMaxAttackStrength(kAttacker.plot(), &plot, pkDefender);
		}

		int iDefenderDamageInflicted = 0; // passed by reference
		int iAttackerDamageInflicted = kAttacker.getMeleeCombatDamage(iAttackerStrength, iDefenderStrength, iDefenderDamageInflicted, /*bIncludeRand*/ bIncludeRand, pkDefender);

		//Chance to spread promotion?
		kAttacker.DoPlagueTransfer(*pkDefender, true);
		if (!pkDefender->IsCanAttackRanged())
			pkDefender->DoPlagueTransfer(kAttacker, false);
	

		pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderDamageInflicted + kAttacker.getDamage());
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);
		pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerDamageInflicted + pkDefender->getDamage());
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);

		int iExperience = /*4*/ GD_INT_GET(EXPERIENCE_DEFENDING_UNIT_MELEE);
		pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, iExperience);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, kAttacker.maxXPValue());
		pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == pkDefender->getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, true);
		}
		else
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, pkDefender->isGGFromBarbarians() || !kAttacker.isBarbarian());
		}
#else	
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !kAttacker.isBarbarian());
#endif
		//iExperience = ((iExperience * iDefenderEffectiveStrength) / iAttackerEffectiveStrength);
		//iExperience = range(iExperience, GD_INT_GET(MIN_EXPERIENCE_PER_COMBAT), GD_INT_GET(MAX_EXPERIENCE_PER_COMBAT));
		iExperience = /*5*/ GD_INT_GET(EXPERIENCE_ATTACKING_UNIT_MELEE);
		pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, pkDefender->maxXPValue());
		pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == kAttacker.getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
		}
		else
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !pkDefender->isBarbarian());
		}
#else	
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !pkDefender->isBarbarian());
#endif
		pkCombatInfo->setAttackIsRanged(false);

		bool bAdvance = true;
		if(plot.getNumDefenders(pkDefender->getOwner()) > 1)
		{
			bAdvance = false;
		}
		else if (iAttackerDamageInflicted >= pkDefender->GetCurrHitPoints() && kAttacker.IsCaptureDefeatedEnemy() && kAttacker.getDomainType() == pkDefender->getDomainType())
		{
			int iCaptureRoll = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x3b80ed93).mix(pkDefender->GetID()).mix(plot.GetPseudoRandomSeed()));

			if (iCaptureRoll <= kAttacker.GetCaptureChance(pkDefender))
			{
				bAdvance = false;
				pkCombatInfo->setDefenderCaptured(true);
			}
		}
		else if (kAttacker.IsCanHeavyCharge() && !pkDefender->isDelayedDeath())
		{
			if (!MOD_BALANCE_VP && iAttackerDamageInflicted + pkDefender->getDamage() > iDefenderDamageInflicted + kAttacker.getDamage())
				bAdvance = true;
			if (MOD_BALANCE_VP && iAttackerStrength > iDefenderStrength)
				bAdvance = true;
		}

		if (kAttacker.plot()->isFortification(kAttacker.getTeam()))
			bAdvance = false;

		pkCombatInfo->setAttackerAdvances(bAdvance);
		pkCombatInfo->setDefenderRetaliates(true);
		pkCombatInfo->setAttackerIsStronger(iAttackerStrength > iDefenderStrength);
	}

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}


//	--------------------------------------------------------------------------------
/// Is this a Barbarian Unit threatening a nearby Minor?
void DoTestBarbarianThreatToMinorsWithThisUnitsDeath(CvUnit* pUnit, PlayerTypes eKillingPlayer)
{
	// Need valid input
	if(eKillingPlayer == NO_PLAYER || pUnit == NULL)
		return;

	// No minors
	if(GET_PLAYER(eKillingPlayer).isMinorCiv())
		return;

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iMinorLoop;

		if(GET_PLAYER(eMinor).isAlive())
		{
			if (pUnit->plot()->isAdjacentTeam( GET_PLAYER(eMinor).getTeam() ))
				GET_PLAYER(eMinor).GetMinorCivAI()->DoThreateningBarbKilled(eKillingPlayer, pUnit->getX(), pUnit->getY());
		}
	}
}


//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveMeleeCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	// After combat stuff
	CvString strBuffer;
	bool bAttackerDead = false;
	bool bDefenderDead = false;

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	if(!pkTargetPlot && pkDefender)
		pkTargetPlot = pkDefender->plot();

	ASSERT_DEBUG(pkAttacker && pkDefender && pkTargetPlot);

	int iActivePlayerID = GC.getGame().getActivePlayer();

	bool bAttackerDidMoreDamage = false;

	if(pkAttacker != NULL && pkDefender != NULL && pkTargetPlot != NULL &&
	        pkDefender->IsCanDefend()) 		// Did the defender actually defend?
	{
		// Internal variables
		int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
		int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

		bAttackerDidMoreDamage = iAttackerDamageInflicted > iDefenderDamageInflicted;

		//One Hit
		if (MOD_API_ACHIEVEMENTS && iAttackerDamageInflicted > pkDefender->GetCurrHitPoints() && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);

		pkDefender->changeDamage(iAttackerDamageInflicted, pkAttacker->getOwner());
		pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner(), -1.f); // Signal that we don't want the popup text.  It will be added later when the unit is at its final location

#if defined(MOD_CORE_PER_TURN_DAMAGE)
		//don't count the "self-inflicted" damage on the attacker
		pkDefender->addDamageReceivedThisTurn(iAttackerDamageInflicted, pkAttacker);
#endif
	
		pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

		// Update experience for both sides.
		pkDefender->changeExperienceTimes100(100 *
		    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
		    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
		    true,
		    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
		    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER),
			pkAttacker->isHuman());

		// only gain XP for the first attack made per turn.
		if(!MOD_BALANCE_CORE_XP_ON_FIRST_ATTACK || pkAttacker->getNumAttacksMadeThisTurn() <= 1)
		{
			pkAttacker->changeExperienceTimes100(100 * kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
				kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
				true,
				kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
				kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER),
				pkDefender->isHuman());
		}

		// Anyone eat it?
		bAttackerDead = (pkAttacker->getDamage() >= pkAttacker->GetMaxHitPoints());
		bDefenderDead = (pkDefender->getDamage() >= pkDefender->GetMaxHitPoints());

		if (MOD_API_ACHIEVEMENTS)
		{
			CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
			kAttackerOwner.GetPlayerAchievements().AttackedUnitWithUnit(pkAttacker, pkDefender);
		}

		// Attacker died
		if(bAttackerDead)
		{
			if (MOD_API_ACHIEVEMENTS)
			{
				CvPlayerAI& kDefenderOwner = GET_PLAYER(pkDefender->getOwner());
				kDefenderOwner.GetPlayerAchievements().KilledUnitWithUnit(pkDefender, pkAttacker);
			}

			CvInterfacePtr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
			gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());

			if(iActivePlayerID == pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted, /*iAttackerFearDamageInflicted*/ 0);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
			}
			if(iActivePlayerID == pkDefender->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted, /*iAttackerFearDamageInflicted*/ 0 , pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
			}
			pkDefender->testPromotionReady();

			ApplyPostKillTraitEffects(pkDefender, pkAttacker);

		}
		// Defender died
		else if(bDefenderDead)
		{
			if (MOD_API_ACHIEVEMENTS)
			{
				CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
				kAttackerOwner.GetPlayerAchievements().KilledUnitWithUnit(pkAttacker, pkDefender);
			}

			CvInterfacePtr<ICvUnit1> pDefender = GC.WrapUnitPointer(pkDefender);
			gDLL->GameplayUnitDestroyedInCombat(pDefender.get());

			if(iActivePlayerID == pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
			}

			if(pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted);
			}
			else
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
			}
			if(iActivePlayerID == pkDefender->getOwner())
			{
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), ((pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner()) ? NO_PLAYER : pkAttacker->getOwner()));
			}
			CvNotifications* pNotification = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
			if(pNotification)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
				pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
			}

			pkAttacker->testPromotionReady();

			ApplyPostKillTraitEffects(pkAttacker, pkDefender);

			// If defender captured, mark who captured him
			if (kCombatInfo.getDefenderCaptured())
			{
				pkDefender->setCapturingPlayer(pkAttacker->getOwner());
				pkDefender->SetCapturedAsIs(true);

				// Other military units escorted by the captured unit withdraw if possible
				pkDefender->DoFallBack(*pkAttacker, false, true);
				if (pkAttacker->GetCapturedUnitsConscriptedCount() > 0)
				{
					pkDefender->SetCapturedAsConscript(true);
				}
			}
		}
		// Nobody died
		else
		{
			if(iActivePlayerID == pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
			}
			if(iActivePlayerID == pkDefender->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
			}

			pkDefender->testPromotionReady();
			pkAttacker->testPromotionReady();

		}

		// Minors want Barbs near them dead
		if (bAttackerDead)
		{
			if (pkAttacker->isBarbarian())
				DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkAttacker, pkDefender->getOwner());
		}
		else if (bDefenderDead)
		{
			if (pkDefender->isBarbarian())
				DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender, pkAttacker->getOwner());
		}
	}

	if(pkAttacker)
	{
		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(false,GetPostCombatDelay());
	}
	if(pkDefender)
	{
		pkDefender->setCombatUnit(NULL);
		pkDefender->ClearMissionQueue();
	}
	else
		bDefenderDead = true;

	if(pkAttacker)
	{
		if(pkAttacker->isSuicide())
		{
			pkAttacker->setCombatUnit(NULL);	// Must clear this if doing a delayed kill, should this be part of the kill method?
			pkAttacker->kill(true);
		}
		else
		{
			if(pkTargetPlot)
			{
				if (pkAttacker->IsCanHeavyCharge() && !pkDefender->isDelayedDeath())
				{
					if (MOD_ATTRITION && (pkDefender->plot()->isFortification(pkDefender->getTeam()) || pkDefender->plot()->HasBarbarianCamp()))
					{ }
					else if (!MOD_BALANCE_VP && bAttackerDidMoreDamage)
					{
						pkDefender->DoFallBack(*pkAttacker);
					}
					else if (MOD_BALANCE_VP && kCombatInfo.getAttackerIsStronger())
					{
						pkDefender->DoFallBack(*pkAttacker);
					}
					//no notifications?
				}

				bool bCanAdvance = kCombatInfo.getAttackerAdvances() && pkTargetPlot->getNumVisibleEnemyDefenders(pkAttacker) == 0;
				if(bCanAdvance)
				{
					if(kCombatInfo.getAttackerAdvancedVisualization())
						// The combat vis has already 'moved' the unit.  Have the game side just do its movement calculations and pop the unit to the new location.
						pkAttacker->move(*pkTargetPlot, false);
					else
						pkAttacker->UnitMove(pkTargetPlot, true, pkAttacker);

					pkAttacker->PublishQueuedVisualizationMoves();

					if (bDefenderDead)
						pkAttacker->DoAdjacentPlotDamage(pkTargetPlot, pkAttacker->getAOEDamageOnKill(), "TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_AOE_STRIKE");
				}
				else
				{
					pkAttacker->changeMoves(-1 * std::max(GD_INT_GET(MOVE_DENOMINATOR), pkTargetPlot->movementCost(pkAttacker, pkAttacker->plot(),pkAttacker->getMoves())));

					if(!pkAttacker->canMove() || !pkAttacker->isBlitz() || pkAttacker->isOutOfAttacks())
					{
						if(pkAttacker->IsSelected())
						{
							if(GC.GetEngineUserInterface()->GetLengthSelectionList() > 1)
							{
								CvInterfacePtr<ICvUnit1> pDllAttacker = GC.WrapUnitPointer(pkAttacker);
								GC.GetEngineUserInterface()->RemoveFromSelectionList(pDllAttacker.get());
							}
						}
					}
				}
			}

			// If a Unit loses his moves after attacking, do so
			if(!pkAttacker->canMoveAfterAttacking() && pkAttacker->isOutOfAttacks())
			{
				pkAttacker->finishMoves();
				GC.GetEngineUserInterface()->changeCycleSelectionCounter(1);
			}
		}
	}

	BATTLE_FINISHED();
}

//	---------------------------------------------------------------------------
//	Function: GenerateRangedCombatInfo
//	Take the input parameters and fill in a CvCombatInfo definition assuming a
//	ranged combat.
//
//	Parameters:
//		pkDefender   	-	Defending unit.  Can be null, in which case the input plot must have a city
//		plot         	-	The plot of the defending unit/city
//		pkCombatInfo 	-	Output combat info
//	---------------------------------------------------------------------------
void CvUnitCombat::GenerateRangedCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& plot, CvCombatInfo* pkCombatInfo)
{
	BATTLE_STARTED(BATTLE_TYPE_RANGED, plot);
	pkCombatInfo->setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, pkDefender);
	pkCombatInfo->setHasMoved(BATTLE_UNIT_ATTACKER, kAttacker.hasMoved());
	pkCombatInfo->setPlot(&plot);

	//////////////////////////////////////////////////////////////////////

	bool bIncludeRand = !GC.getGame().isGameMultiPlayer();

	bool bBarbarian = false;
	int iExperience = 0;
	int iMaxXP = 0;
	int iDamage = 0;
	int iTotalDamage = 0;
	if(!plot.isCity())
	{
		ASSERT_DEBUG(pkDefender != NULL);

		iExperience = /*2*/ GD_INT_GET(EXPERIENCE_ATTACKING_UNIT_RANGED);
		if(pkDefender->isBarbarian())
			bBarbarian = true;
		iMaxXP = pkDefender->maxXPValue();

		//ASSERT_DEBUG(pkDefender->IsCanDefend());
		int iUnusedReferenceVariable = 0;
		iDamage = kAttacker.GetRangeCombatDamage(pkDefender, /*pCity*/ NULL, 0, iUnusedReferenceVariable, /*bIncludeRand*/ bIncludeRand);

		if (iDamage + pkDefender->getDamage() > pkDefender->GetMaxHitPoints())
		{
			iDamage = pkDefender->GetMaxHitPoints() - pkDefender->getDamage();
		}

		iTotalDamage = std::max(pkDefender->getDamage(), pkDefender->getDamage() + iDamage);

		//Chance to spread promotion?
		kAttacker.DoPlagueTransfer(*pkDefender, true);
	}
	else
	{
		if (kAttacker.isRangedSupportFire()) return; // can't attack cities with this

		CvCity* pCity = plot.getPlotCity();
		ASSERT_DEBUG(pCity != NULL);
		if(!pCity) return;
		BATTLE_JOINED(pCity, BATTLE_UNIT_DEFENDER, true);

		iExperience = /*3 in CP, 2 in VP*/ GD_INT_GET(EXPERIENCE_ATTACKING_CITY_RANGED);
		if(pCity->isBarbarian())
			bBarbarian = true;
		iMaxXP = pCity->maxXPValue();

		//if there is a garrison, the unit absorbs part of the damage!
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		int iGarrisonMaxHP = 0;
		if (pGarrison && !pGarrison->IsDead())
			iGarrisonMaxHP = pGarrison->GetMaxHitPoints();

		int iGarrisonDamage = 0;
		iDamage = kAttacker.GetRangeCombatDamage(/*pDefender*/ NULL, pCity, iGarrisonMaxHP, iGarrisonDamage,
			/*bIncludeRand*/ bIncludeRand, 0, NULL, NULL, false, false);

		if(pGarrison && iGarrisonDamage > 0)
		{
			//add the garrison as a bystander
			int iDamageMembers = 0;
			CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkCombatInfo->getDamageMembers(), &iDamageMembers, pkCombatInfo->getMaxDamageMemberCount(), pGarrison);
			if(pkDamageEntry)
			{
#if defined(MOD_EVENTS_BATTLES)
				BATTLE_JOINED(pGarrison, BATTLE_UNIT_COUNT, false); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER
#endif
				pkDamageEntry->SetDamage(iGarrisonDamage);
				pkDamageEntry->SetFinalDamage(std::min(iGarrisonDamage + pGarrison->getDamage(), pGarrison->GetMaxHitPoints()));
				pkDamageEntry->SetMaxHitPoints(pGarrison->GetMaxHitPoints());
				pkCombatInfo->setDamageMemberCount(iDamageMembers);
			}
		}

		// Cities can't be knocked to less than 1 HP
		if(iDamage + pCity->getDamage() >= pCity->GetMaxHitPoints())
		{
			iDamage = pCity->GetMaxHitPoints() - pCity->getDamage() - 1;
		}

		iTotalDamage = std::max(pCity->getDamage(), pCity->getDamage() + iDamage);
	}
	//////////////////////////////////////////////////////////////////////

	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);				// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iDamage);		// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iTotalDamage);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);			// Damage inflicted this round

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, iMaxXP);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == kAttacker.getOwner());

	bool bGeneralsXP = !kAttacker.isBarbarian();
	if (!plot.isCity())
	{
		bGeneralsXP = !pkDefender->isBarbarian();
	} 
	else
	{
		bGeneralsXP = !plot.getPlotCity()->isBarbarian();
	}

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || bGeneralsXP);
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif

	iExperience = /*2*/ GD_INT_GET(EXPERIENCE_DEFENDING_UNIT_RANGED);
	pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, iExperience);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, kAttacker.maxXPValue());
	pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, true);
	}
	else
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, (!plot.isCity() && pkDefender->isGGFromBarbarians()) || (!bBarbarian && !kAttacker.isBarbarian()));
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !bBarbarian && !kAttacker.isBarbarian());
#endif

	pkCombatInfo->setAttackIsRanged(true);
	// Defender doesn't retaliate.  We'll keep this separate from the ranged attack flag in case something changes to allow
	// some units to retaliate on a ranged attack (Archer vs. Archers maybe?)
	pkCombatInfo->setDefenderRetaliates(false);

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	---------------------------------------------------------------------------
//	Function: GenerateRangedCombatInfo
//	Take the input parameters and fill in a CvCombatInfo definition assuming a
//	ranged combat from a city.
//
//	Parameters:
//		kAttacker		-	Attacking city.
//		pkDefender   	-	Defending unit.
//		plot         	-	The plot of the defending unit
//		pkCombatInfo 	-	Output combat info
//	---------------------------------------------------------------------------
void CvUnitCombat::GenerateRangedCombatInfo(CvCity& kAttacker, CvUnit* pkDefender, CvPlot& plot, CvCombatInfo* pkCombatInfo)
{
	BATTLE_STARTED(BATTLE_TYPE_RANGED, plot);
	pkCombatInfo->setCity(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, pkDefender);
	pkCombatInfo->setPlot(&plot);

	//////////////////////////////////////////////////////////////////////
	bool bIncludeRand = !GC.getGame().isGameMultiPlayer();

	bool bBarbarian = false;
	int iDamage = 0;
	int iTotalDamage = 0;
	if(!plot.isCity())
	{
		ASSERT_DEBUG(pkDefender != NULL);

		if(pkDefender->isBarbarian())
			bBarbarian = true;

		//ASSERT_DEBUG(pkDefender->IsCanDefend());
		iDamage = kAttacker.rangeCombatDamage(pkDefender,bIncludeRand);

		if(iDamage + pkDefender->getDamage() > pkDefender->GetMaxHitPoints())
		{
			iDamage = pkDefender->GetMaxHitPoints() - pkDefender->getDamage();
		}

		iTotalDamage = std::max(pkDefender->getDamage(), pkDefender->getDamage() + iDamage);
	}
	else
	{
		ASSERT_DEBUG(false, "City vs. City not supported.");	// Don't even think about it Jon....
	}

	//////////////////////////////////////////////////////////////////////
	int iMaxXP = kAttacker.maxXPValue();
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, iMaxXP);
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);				// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iDamage);		// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iTotalDamage);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);			// Damage inflicted this round

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == kAttacker.getOwner());

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian()); // Kind of irrelevant, as cities don't get XP!
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif

	int iExperience = /*2*/ GD_INT_GET(EXPERIENCE_DEFENDING_UNIT_RANGED);
	pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, iExperience);
	pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, true);
	}
	else
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, pkDefender->isGGFromBarbarians() || (!bBarbarian && !kAttacker.isBarbarian()));
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !bBarbarian && !kAttacker.isBarbarian());
#endif

	pkCombatInfo->setAttackIsRanged(true);
	// Defender doesn't retaliate.  We'll keep this separate from the ranged attack flag in case something changes to allow
	// some units to retaliate on a ranged attack (Archer vs. Archers maybe?)
	pkCombatInfo->setDefenderRetaliates(false);

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	---------------------------------------------------------------------------
//	Function: ResolveRangedUnitVsCombat
//	Resolve ranged combat where the attacker is a unit.  This will handle
//  unit vs. unit and unit vs. city
//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveRangedUnitVsCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	bool bTargetDied = false;
	bool bTargetIsHuman = false;
	int iDamage = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
//	int iExperience = kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER);
//	int iMaxXP = kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER);

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	ASSERT_DEBUG(pkAttacker);

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	ASSERT_DEBUG(pkTargetPlot);

	ICvUserInterface2* pkDLLInterface = GC.GetEngineUserInterface();
	CvString strBuffer;

	if(pkTargetPlot)
	{
		if(!pkTargetPlot->isCity())
		{
			// Unit
			CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
			ASSERT_DEBUG(pkDefender != NULL);
			if(pkDefender)
			{
				if(pkAttacker)
				{
					if (pkDefender->isHuman())
						bTargetIsHuman = true;

					pkAttacker->DoAdjacentPlotDamage(pkTargetPlot,min(iDamage,pkAttacker->getSplashDamage()),"TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_SPLASH");
					if (!kCombatInfo.getHasMoved(BATTLE_UNIT_ATTACKER))
					{
						pkAttacker->DoExtraPlotDamage(pkTargetPlot, min(iDamage, pkAttacker->GetTileDamageIfNotMoved()), "TXT_KEY_MISC_TILE_DAMAGE_NOT_MOVED");
					}
					pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

					// Defender died
					if(iDamage + pkDefender->getDamage() >= pkDefender->GetMaxHitPoints())
					{
						// Units with Ranged Support Fire don't normally consume an attack, because the subsequent attack will. However, we need to make an exception if this attack kills the defender
						if(pkAttacker->isRangedSupportFire())
							pkAttacker->setMadeAttack(true);

						//One Hit
						if (MOD_API_ACHIEVEMENTS && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
							gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);

						if(pkAttacker->getOwner() == GC.getGame().getActivePlayer())
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_AND_DEATH", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
						}

						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR_AND_DEATH", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str());
						CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
						if(pNotifications)
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
						}

						bTargetDied = true;

						if (MOD_API_ACHIEVEMENTS)
						{
							CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
							kAttackerOwner.GetPlayerAchievements().KilledUnitWithUnit(pkAttacker, pkDefender);
						}

						ApplyPostKillTraitEffects(pkAttacker, pkDefender);

						if(pkDefender->isBarbarian())
							DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender, pkAttacker->getOwner());
					}
					// Nobody died
					else
					{
						if(pkAttacker->getOwner() == GC.getGame().getActivePlayer())
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iDamage);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
						}
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDamage);
#if defined(MOD_BALANCE_CORE)
						if (pkAttacker->GetMoraleBreakChance() > 0 && !pkDefender->isDelayedDeath() && pkDefender->GetNumFallBackPlotsAvailable(*pkAttacker) > 0)
						{
							int iRand = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0xd7e83836).mix(pkDefender->GetID()).mix(pkDefender->plot()->GetPseudoRandomSeed()));
							if (iRand <= pkAttacker->GetMoraleBreakChance())
							{
								if (pkDefender->DoFallBack(*pkAttacker))
								{
									CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
									if (pNotifications)
									{
										Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK");
										strMessage << pkAttacker->getUnitInfo().GetTextKey();
										strMessage << pkDefender->getUnitInfo().GetTextKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK_S");
										strSummary << pkDefender->getUnitInfo().GetTextKey();
										pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int)pkDefender->getUnitType(), pkDefender->getOwner());
									}
									CvNotifications* pNotificationsOther = GET_PLAYER(pkAttacker->getOwner()).GetNotifications();
									if (pNotificationsOther)
									{
										Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK_THEM");
										strMessage << pkAttacker->getUnitInfo().GetTextKey();
										strMessage << pkDefender->getUnitInfo().GetTextKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK_THEM_S");
										strSummary << pkDefender->getUnitInfo().GetTextKey();
										pNotificationsOther->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int)pkDefender->getUnitType(), pkDefender->getOwner());
									}
								}
							}
						}
#endif
					}

					//red icon over attacking unit
					if(pkDefender->getOwner() == GC.getGame().getActivePlayer())
					{
						pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkAttacker->m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkAttacker->getX(), pkAttacker->getY(), true, true*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
					}
					//white icon over defending unit
					//pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), false, 0, ""/*, "AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pkDefender->getX(), pkDefender->getY(), true, true*/);

					//set damage but don't update entity damage visibility
					pkDefender->changeDamage(iDamage, pkAttacker->getOwner());

#if defined(MOD_CORE_PER_TURN_DAMAGE)
					pkDefender->addDamageReceivedThisTurn(iDamage, pkAttacker);
#endif

					// Update experience
					pkDefender->changeExperienceTimes100(100 * 
					    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
					    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
					    true,
					    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
					    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER),
						pkAttacker->isHuman());
				}

				pkDefender->setCombatUnit(NULL);
				if(!CvUnitMission::IsHeadMission(pkDefender, CvTypes::getMISSION_WAIT_FOR()))		// If the top mission was not a 'wait for', then clear it.
					pkDefender->ClearMissionQueue();
			}
			else
				bTargetDied = true;
		}
		else
		{
			// City
			CvCity* pCity = pkTargetPlot->getPlotCity();
			ASSERT_DEBUG(pCity != NULL);
			if(pCity)
			{
				if(pkAttacker)
				{
					if (pCity->isHuman())
						bTargetIsHuman = true;

					pkAttacker->DoAdjacentPlotDamage(pkTargetPlot,min(iDamage,pkAttacker->getSplashDamage()),"TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_SPLASH");
					if (!kCombatInfo.getHasMoved(BATTLE_UNIT_ATTACKER))
					{
						pkAttacker->DoExtraPlotDamage(pkTargetPlot, min(iDamage, pkAttacker->GetTileDamageIfNotMoved()), "TXT_KEY_MISC_TILE_DAMAGE_NOT_MOVED");
					}

					// City not already at 0 HP
					if(pCity->getDamage() != pCity->GetMaxHitPoints())
					{
						ApplyPostCityCombatEffects(pkAttacker, pCity, iDamage);
					}

					pCity->changeDamage(iDamage);

#if defined(MOD_CORE_PER_TURN_DAMAGE)
					pCity->addDamageReceivedThisTurn(iDamage, pkAttacker);
#endif

					if(pCity->getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOUR_CITY_ATTACKED_BY_AIR", pCity->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDamage);
						//red icon over attacking unit
						pkDLLInterface->AddMessage(uiParentEventID, pCity->getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkAttacker->m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkAttacker->getX(), pkAttacker->getY(), true, true*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(pCity->getOwner(), strBuffer.c_str(), pCity->plot(), pkAttacker->getOwner());
					}
#if defined(MOD_BALANCE_CORE)
					else if(pkAttacker->getOwner() == GC.getGame().getActivePlayer() && (pCity->getOwner() != pkAttacker->getOwner()))
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACKED_CITY_CP", pCity->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDamage);
						//red icon over attacking unit
						pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkAttacker->m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkAttacker->getX(), pkAttacker->getY(), true, true*/);
					}
#endif
#if defined(MOD_BALANCE_CORE_MILITARY)

					//apply damage to garrison
					CvUnitCombat::ApplyExtraUnitDamage(pkAttacker, kCombatInfo, uiParentEventID);
#endif
				}

				pCity->clearCombat();
			}
			else
				bTargetDied = true;
		}
	}
	else
		bTargetDied = true;

	if(pkAttacker)
	{
		// Unit gains XP for executing a Range Strike
		if(iDamage > 0) // && iDefenderStrength > 0)
		{
			// only gain XP for the first attack made per turn. A Ranged Support Fire attack shouldn't reward XP unless it kills the target
			if(!MOD_BALANCE_CORE_XP_ON_FIRST_ATTACK || (pkAttacker->getNumAttacksMadeThisTurn() <= 1 && (bTargetDied || !pkAttacker->isRangedSupportFire())))
			{
				pkAttacker->changeExperienceTimes100(100 * kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
					kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
					true,
					kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
					kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER),
					bTargetIsHuman);
				
				pkAttacker->testPromotionReady();
			}
		}

		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(false,GetPostCombatDelay());
	}
	
	BATTLE_FINISHED();
}

//	---------------------------------------------------------------------------
//	Function: ResolveRangedCityVsUnitCombat
//	Resolve ranged combat where the attacker is a city
//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveRangedCityVsUnitCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	int iDamage = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	bool bBarbarian = false;

	CvCity* pkAttacker = kCombatInfo.getCity(BATTLE_UNIT_ATTACKER);
	ASSERT_DEBUG(pkAttacker);

	pkAttacker->clearCombat();

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	ASSERT_DEBUG(pkTargetPlot);
	ASSERT_DEBUG(!pkTargetPlot->isCity());

	ICvUserInterface2* pkDLLInterface = GC.GetEngineUserInterface();
	int iActivePlayerID = GC.getGame().getActivePlayer();

	CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
	ASSERT_DEBUG(pkDefender);

	bBarbarian = pkDefender->isBarbarian();

	pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

	// Info message for the attacking player
	if(iActivePlayerID == pkAttacker->getOwner())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOUR_CITY_RANGE_ATTACK");
		localizedText << pkAttacker->getNameKey() << (pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str()) << iDamage;
		pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), localizedText.toUTF8());//, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX(), pPlot->getY());
		if (MOD_WH_MILITARY_LOG)
			MILITARYLOG(pkAttacker->getOwner(), localizedText.toUTF8(), pkAttacker->plot(), pkDefender->getOwner());
	}

	// Red icon over defending unit
	if(iActivePlayerID == pkDefender->getOwner())
	{
		Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_CITY");
		localizedText << (pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str()) << pkAttacker->getNameKey() << iDamage;
		pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), localizedText.toUTF8());//, "AS2D_COMBAT", MESSAGE_TYPE_COMBAT_MESSAGE, pDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pDefender->getX(), pDefender->getY(), true, true);
		if (MOD_WH_MILITARY_LOG)
			MILITARYLOG(pkDefender->getOwner(), localizedText.toUTF8(), pkDefender->plot(), pkAttacker->getOwner());
	}

	if(iDamage + pkDefender->getDamage() >= pkDefender->GetMaxHitPoints())
	{
		CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
		if(pNotifications)
		{
			Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_CITY");
			localizedText << (pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str()) << pkAttacker->getNameKey() << iDamage;
			Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
			pNotifications->Add(NOTIFICATION_UNIT_DIED, localizedText.toUTF8(), strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
		}

		// Earn bonuses for kills?
		CvPlayer& kAttackingPlayer = GET_PLAYER(pkAttacker->getOwner());
		kAttackingPlayer.DoYieldsFromKill(NULL, pkDefender, pkAttacker);

		if (bBarbarian)
			DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender, pkAttacker->getOwner());
	}

	//set damage but don't update entity damage visibility
	pkDefender->changeDamage(iDamage, pkAttacker->getOwner());

#if defined(MOD_CORE_PER_TURN_DAMAGE)
	pkDefender->addDamageReceivedThisTurn(iDamage);
#endif

	// Update experience
	pkDefender->changeExperienceTimes100(100 * 
		kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
		kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
		true,
		kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
		kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER),
		pkAttacker->isHuman());

	pkDefender->setCombatUnit(NULL);
	if(!CvUnitMission::IsHeadMission(pkDefender, CvTypes::getMISSION_WAIT_FOR()))		// If the top mission was not a 'wait for', then clear it.
		pkDefender->ClearMissionQueue();

	BATTLE_FINISHED();
}

//	---------------------------------------------------------------------------
//	Function: ResolveCityMeleeCombat
//
//	Resolves combat between a melee unit and a city.
//  The unit does not have to be a hand-to-hand combat type unit, just a unit doing
//  a non-ranged attack to an adjacent city.  The visualization of the attack will
//	usually appear as if it is ranged, simply because we don't want the unit members
//	running through a city and they wouldn't have anything to attack.
//	This is also the case where a city is able to attack back.
void CvUnitCombat::ResolveCityMeleeCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvCity* pkDefender = kCombatInfo.getCity(BATTLE_UNIT_DEFENDER);

	ASSERT_DEBUG(pkAttacker && pkDefender);

	CvPlot* pkPlot = kCombatInfo.getPlot();
	if(!pkPlot && pkDefender)
		pkPlot = pkDefender->plot();

	ASSERT_DEBUG(pkPlot);

	int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	if(pkAttacker && pkDefender)
	{
		pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner());
		pkDefender->changeDamage(iAttackerDamageInflicted);

#if defined(MOD_CORE_PER_TURN_DAMAGE)
		pkDefender->addDamageReceivedThisTurn(iAttackerDamageInflicted, pkAttacker);
#endif
		pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);
		
		// only gain XP for the first attack made per turn.
		if(!MOD_BALANCE_CORE_XP_ON_FIRST_ATTACK || pkAttacker->getNumAttacksMadeThisTurn() <= 1)
		{
			pkAttacker->changeExperienceTimes100(100 * kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
				kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
				true,
				false,
				kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER),
				pkDefender->isHuman());
		}
	}

	if(pkDefender)
		pkDefender->clearCombat();

	if(pkAttacker)
	{
		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(false,GetPostCombatDelay());

		if(pkAttacker->isSuicide())
		{
			pkAttacker->setCombatUnit(NULL);	// Must clear this if doing a delayed kill, should this be part of the kill method?
			pkAttacker->kill(true);
		}
	}

	CvString strBuffer;
	int iActivePlayerID = GC.getGame().getActivePlayer();

	// Barbarians don't capture Cities in Community Patch only
	if (pkAttacker && pkDefender)
	{
		if (pkAttacker->isBarbarian() && (pkDefender->getDamage() >= pkDefender->GetMaxHitPoints()) && !MOD_BALANCE_VP)
		{
			// 1 HP left
			pkDefender->setDamage(pkDefender->GetMaxHitPoints() - 1);

			int iNumGoldStolen = max(/*200*/ GD_INT_GET(BARBARIAN_CITY_GOLD_RANSOM), GET_PLAYER(pkDefender->getOwner()).GetTreasury()->GetGold());

			// City is ransomed for Gold
			GET_PLAYER(pkDefender->getOwner()).GetTreasury()->ChangeGold(-iNumGoldStolen);

			if (pkDefender->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CITY_RANSOMED_BY_BARBARIANS", pkDefender->getNameKey(), iNumGoldStolen);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
			}

			if (MOD_API_ACHIEVEMENTS && pkDefender->getOwner() == GC.getGame().getActivePlayer() && pkDefender->isHuman() && !GC.getGame().isGameMultiPlayer())
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_REALLY_SUCK);
			}

			// Barb goes away after ransom
			pkAttacker->kill(true, NO_PLAYER);
		}
		// Attacker died
		else if (pkAttacker->IsDead())
		{
			CvInterfacePtr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
			gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());
			if(pkAttacker->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING_CITY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
			}
			if(pkDefender->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT_CITY", pkDefender->getNameKey(), iAttackerDamageInflicted, pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkPlot->getX(), pkPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
			}
			ApplyPostCityCombatEffects(pkAttacker, pkDefender, iAttackerDamageInflicted);
		}
		// City conquest
		else if(pkDefender->getDamage() >= pkDefender->GetMaxHitPoints())
		{
			if(!pkAttacker->isNoCapture())
			{
				if(pkAttacker->getOwner() == iActivePlayerID)
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_CAPTURED_ENEMY_CITY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->getNameKey());
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkPlot->getX(), pkPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
				}
				if(pkDefender->getOwner() == iActivePlayerID)
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CITY_WAS_CAPTURED", pkDefender->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
				}

				ApplyPostCityCombatEffects(pkAttacker, pkDefender, iAttackerDamageInflicted);
				pkAttacker->UnitMove(pkPlot, true, pkAttacker);
			}
		}
		// Neither side lost
		else
		{
			if(pkAttacker->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW_CITY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkPlot->getX(), pkPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
			}
			if(pkDefender->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW_CITY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
			}
			ApplyPostCityCombatEffects(pkAttacker, pkDefender, iAttackerDamageInflicted);
			pkAttacker->changeMoves(-1 * std::max(GD_INT_GET(MOVE_DENOMINATOR), pkPlot->movementCost(pkAttacker, pkAttacker->plot(), pkAttacker->getMoves())));
		}

#if defined(MOD_BALANCE_CORE_MILITARY)
		//apply damage to garrison
		CvUnitCombat::ApplyExtraUnitDamage(pkAttacker, kCombatInfo, uiParentEventID);
#endif
	}

	//extra checks in case the attacking unit was killed because of a failed teleport
	if(pkAttacker && !pkAttacker->isDelayedDeath() && pkAttacker->plot())
	{
		pkAttacker->PublishQueuedVisualizationMoves();
		if(!pkAttacker->canMoveAfterAttacking() && pkAttacker->isOutOfAttacks())
		{
			pkAttacker->finishMoves();
			GC.GetEngineUserInterface()->changeCycleSelectionCounter(1);
		}
	}
	
	BATTLE_FINISHED();
}

//	GenerateAirCombatInfo
//	Function: GenerateRangedCombatInfo
//	Take the input parameters and fill in a CvCombatInfo definition assuming a
//	air bombing mission.
//
//	Parameters:
//		pkDefender   	-	Defending unit.  Can be null, in which case the input plot must have a city
//		plot         	-	The plot of the defending unit/city
//		pkCombatInfo 	-	Output combat info
//	---------------------------------------------------------------------------
void CvUnitCombat::GenerateAirCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& plot, CvCombatInfo* pkCombatInfo)
{
	BATTLE_STARTED(BATTLE_TYPE_AIR, plot);
	int iExperience = 0;

	pkCombatInfo->setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, pkDefender);
	pkCombatInfo->setPlot(&plot);

	//////////////////////////////////////////////////////////////////////

	// Any interception to be done?
	// Note: The defender may be the same unit as the interceptor!
	CvUnit* pInterceptor = plot.GetBestInterceptor(kAttacker.getOwner(), &kAttacker);
	if(pInterceptor != NULL)
	{
		// Does the attacker evade?
		int iInterceptionDamage = 0;
		if(kAttacker.evasionProbability()==0 || GC.getGame().randRangeInclusive(1, 100, CvSeeder(plot.GetPseudoRandomSeed()).mix(pInterceptor->GetID()).mix(pInterceptor->getMadeInterceptionCount())) > kAttacker.evasionProbability())
		{
			// Is the interception successful?
			if (pInterceptor->interceptionProbability()>=100 || GC.getGame().randRangeInclusive(1, 100, CvSeeder(plot.GetPseudoRandomSeed()).mix(kAttacker.GetID()).mix(kAttacker.getDamage())) <= pInterceptor->interceptionProbability())
			{
				bool bIncludeRand = !GC.getGame().isGameMultiPlayer();
				iInterceptionDamage = pInterceptor->GetInterceptionDamage(&kAttacker, bIncludeRand, &plot);
			}
		}

		//set these even if the interception failed (ie zero damage)
		pkCombatInfo->setUnit(BATTLE_UNIT_INTERCEPTOR, pInterceptor);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_INTERCEPTOR, iInterceptionDamage);
		pkCombatInfo->setInBorders( BATTLE_UNIT_INTERCEPTOR, plot.getOwner() == kAttacker.getOwner() );
		pkCombatInfo->setUpdateGlobal( BATTLE_UNIT_INTERCEPTOR, true );
		pkCombatInfo->setAttackIsBombingMission(true);
		pkCombatInfo->setDefenderRetaliates(false);
		//no experience by default, may be overwritten
		pkCombatInfo->setExperience( BATTLE_UNIT_INTERCEPTOR, iExperience );

		//is the attack aborted?
		if (iInterceptionDamage > 0)
		{
			int iExperience = /*2*/ GD_INT_GET(EXPERIENCE_DEFENDING_AIR_SWEEP_GROUND);
			pkCombatInfo->setExperience( BATTLE_UNIT_INTERCEPTOR, iExperience );
			pkCombatInfo->setMaxExperienceAllowed( BATTLE_UNIT_INTERCEPTOR, MAX_INT );

			//make sure we have zero values everywhere else
			pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, 0);
			pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);

			pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() != kAttacker.getOwner());
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);

			pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, 0);
			pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, 0);
			pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, false);

			//attack is aborted after successful interception
			GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////

	bool bBarbarian = false;
	int iMaxXP = 0;

	int iAttackerDamageInflicted = 0;
	int iDefenderDamageInflicted = 0;

	int iAttackerTotalDamageInflicted = 0;
	int iDefenderTotalDamageInflicted = 0;

	PlayerTypes eDefenderOwner;

	// Target is a Unit
	// Special: Missiles always attack the unit, even when it's in a city
	if(!plot.isCity() || kAttacker.AI_getUnitAIType() == UNITAI_MISSILE_AIR )
	{
		ASSERT_DEBUG(pkDefender != NULL);
		if(!pkDefender)
			return;

		eDefenderOwner = pkDefender->getOwner();

		iExperience = /*4*/ GD_INT_GET(EXPERIENCE_ATTACKING_UNIT_AIR);
		if(pkDefender->isBarbarian())
			bBarbarian = true;
		iMaxXP = pkDefender->maxXPValue();

		// Calculate attacker damage
		bool bIncludeRand = !GC.getGame().isGameMultiPlayer();
		int iUnusedReferenceVariable = 0;
		iAttackerDamageInflicted = kAttacker.GetAirCombatDamage(pkDefender, /*pCity*/ NULL, 0, iUnusedReferenceVariable, /*bIncludeRand*/ bIncludeRand);

		if(iAttackerDamageInflicted + pkDefender->getDamage() > pkDefender->GetMaxHitPoints())
		{
			iAttackerDamageInflicted = pkDefender->GetMaxHitPoints() - pkDefender->getDamage();
		}

		iAttackerTotalDamageInflicted = std::max(pkDefender->getDamage(), pkDefender->getDamage() + iAttackerDamageInflicted);

		// Calculate defense damage
		iDefenderDamageInflicted = pkDefender->GetAirStrikeDefenseDamage(&kAttacker);

		if(iDefenderDamageInflicted + kAttacker.getDamage() > kAttacker.GetMaxHitPoints())
		{
			iDefenderDamageInflicted = kAttacker.GetMaxHitPoints() - kAttacker.getDamage();
		}

		iDefenderTotalDamageInflicted = std::max(kAttacker.getDamage(), kAttacker.getDamage() + iDefenderDamageInflicted);
	}
	// Target is a City
	else
	{
		CvCity* pCity = plot.getPlotCity();
		ASSERT_DEBUG(pCity != NULL);
		if(!pCity) return;
		BATTLE_JOINED(pCity, BATTLE_UNIT_DEFENDER, true);

		eDefenderOwner = plot.getOwner();

		iExperience = /*4*/ GD_INT_GET(EXPERIENCE_ATTACKING_CITY_AIR);
		if(pCity->isBarbarian())
			bBarbarian = true;
		iMaxXP = pCity->maxXPValue();

		//if there is a garrison, the unit absorbs part of the damage!
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		int iGarrisonMaxHP = 0;
		if (pGarrison && !pGarrison->IsDead())
			iGarrisonMaxHP = pGarrison->GetMaxHitPoints();

		int iGarrisonDamage = 0;
		iAttackerDamageInflicted = kAttacker.GetAirCombatDamage(/*pUnit*/ NULL, pCity, iGarrisonMaxHP, iGarrisonDamage, /*bIncludeRand*/ true, 0, NULL, NULL, false);

		if(pGarrison && iGarrisonDamage > 0)
		{
			//add the garrison as a bystander
			int iDamageMembers = 0;
			CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkCombatInfo->getDamageMembers(), &iDamageMembers, pkCombatInfo->getMaxDamageMemberCount(), pGarrison);
			if(pkDamageEntry)
			{
#if defined(MOD_EVENTS_BATTLES)
				BATTLE_JOINED(pGarrison, BATTLE_UNIT_COUNT, false); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER
#endif
				pkDamageEntry->SetDamage(iGarrisonDamage);
				pkDamageEntry->SetFinalDamage(std::min(iGarrisonDamage + pGarrison->getDamage(), pGarrison->GetMaxHitPoints()));
				pkDamageEntry->SetMaxHitPoints(pGarrison->GetMaxHitPoints());
				pkCombatInfo->setDamageMemberCount(iDamageMembers);
			}
		}

		// Cities can't be knocked to less than 1 HP
		if(iAttackerDamageInflicted + pCity->getDamage() >= pCity->GetMaxHitPoints())
		{
			iAttackerDamageInflicted = pCity->GetMaxHitPoints() - pCity->getDamage() - 1;
		}

		iAttackerTotalDamageInflicted = std::max(pCity->getDamage(), pCity->getDamage() + iAttackerDamageInflicted);

		// Calculate defense damage
		iDefenderDamageInflicted = pCity->GetAirStrikeDefenseDamage(&kAttacker);

		if(iDefenderDamageInflicted + kAttacker.getDamage() > kAttacker.GetMaxHitPoints())
		{
			// Surely!!!
			iDefenderDamageInflicted = kAttacker.GetMaxHitPoints() - kAttacker.getDamage();
		}

		iDefenderTotalDamageInflicted = std::max(kAttacker.getDamage(), kAttacker.getDamage() + iDefenderDamageInflicted);

		if (MOD_API_ACHIEVEMENTS)
		{
			//Achievement for Washington
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(kAttacker.getUnitType());
			if(pkUnitInfo)
			{
				if(kAttacker.isHuman() && !GC.getGame().isGameMultiPlayer() && _stricmp(pkUnitInfo->GetType(), "UNIT_AMERICAN_B17") == 0)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_B17);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////

	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderTotalDamageInflicted);				// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);		// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerTotalDamageInflicted);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);			// Damage inflicted this round

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, iMaxXP);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == eDefenderOwner);

	bool bGeneralsXP = !kAttacker.isBarbarian();
	if (!plot.isCity())
	{
		bGeneralsXP = !pkDefender->isBarbarian();
	}
	else
	{
		bGeneralsXP = !plot.getPlotCity()->isBarbarian();
	}

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || bGeneralsXP);
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif

	iExperience = /*2*/ GD_INT_GET(EXPERIENCE_DEFENDING_UNIT_AIR);
	pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, iExperience);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, MAX_INT);
	pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, true);
	}
	else
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, (!plot.isCity() && pkDefender->isGGFromBarbarians()) || !bBarbarian);
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !bBarbarian);
#endif

	pkCombatInfo->setAttackIsBombingMission(true);
	pkCombatInfo->setDefenderRetaliates(true);

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	ResolveAirUnitVsCombat
//	Function: ResolveRangedUnitVsCombat
//	Resolve air combat where the attacker is a unit.  This will handle
//  unit vs. unit and unit vs. city
//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveAirUnitVsCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	bool bTargetIsHuman = false;
	int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);

	// If there's no valid attacker, then get out of here
	ASSERT_DEBUG(pkAttacker);

	// Interception?
	int iInterceptionDamage = kCombatInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);
	if(iInterceptionDamage > 0)
		iDefenderDamageInflicted += iInterceptionDamage;

	//shouldn't happen...
	if (iDefenderDamageInflicted <= 0)
		iDefenderDamageInflicted = 1;

	CvUnit* pInterceptor = kCombatInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
	ASSERT_DEBUG(pInterceptor);
	if(pInterceptor)
	{
		pInterceptor->increaseInterceptionCount();
		pInterceptor->setCombatUnit(NULL);
		pInterceptor->changeExperienceTimes100(100 * 
			kCombatInfo.getExperience(BATTLE_UNIT_INTERCEPTOR),
			kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_INTERCEPTOR),
			true,
			kCombatInfo.getInBorders(BATTLE_UNIT_INTERCEPTOR),
			kCombatInfo.getUpdateGlobal(BATTLE_UNIT_INTERCEPTOR),
			pkAttacker->isHuman());
	}

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	ASSERT_DEBUG(pkTargetPlot);

	ICvUserInterface2* pkDLLInterface = GC.GetEngineUserInterface();
	int iActivePlayerID = GC.getGame().getActivePlayer();
	CvString strBuffer;

	if(pkTargetPlot)
	{
		if(!pkTargetPlot->isCity() || pkAttacker->AI_getUnitAIType()==UNITAI_MISSILE_AIR)
		{
			// Target was a Unit
			CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
			ASSERT_DEBUG(pkDefender != NULL);

			if(pkDefender)
			{
				if (pkDefender->isHuman())
					bTargetIsHuman = true;

				//One Hit
				if (MOD_API_ACHIEVEMENTS && iAttackerDamageInflicted > pkDefender->GetCurrHitPoints() && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
					gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);

				pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner());
				pkDefender->changeDamage(iAttackerDamageInflicted, pkAttacker->getOwner());

#if defined(MOD_CORE_PER_TURN_DAMAGE)
				//don't count the "self-inflicted" damage on the attacker
				pkDefender->addDamageReceivedThisTurn(iAttackerDamageInflicted, pkAttacker);
#endif

				pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

				// Update experience
				pkDefender->changeExperienceTimes100(100 * 
					kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
					kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
					true,
					kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
					kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER),
					pkAttacker->isHuman());

				// Attacker died
				if(pkAttacker->IsDead())
				{
					CvInterfacePtr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
					gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());

					if (MOD_API_ACHIEVEMENTS)
					{
						CvPlayerAI& kDefenderOwner = GET_PLAYER(pkDefender->getOwner());
						kDefenderOwner.GetPlayerAchievements().KilledUnitWithUnit(pkDefender, pkAttacker);
					}

					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_INTERCEPTED_KILLED", pInterceptor->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
						}
					}
					if(iActivePlayerID == pkDefender->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_DESTROYED", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted, 0, pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
						}
					}

					CvNotifications* pNotifications = GET_PLAYER(pkAttacker->getOwner()).GetNotifications();
					if (pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkAttacker->getX(), pkAttacker->getY(), (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
					}

					ApplyPostKillTraitEffects(pkDefender, pkAttacker);
				}
				// Defender died
				else if(pkDefender->IsDead())
				{
					if (MOD_API_ACHIEVEMENTS)
					{
						CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
						kAttackerOwner.GetPlayerAchievements().KilledUnitWithUnit(pkAttacker, pkDefender);
					}

					//if the defender died, there was no interception because it would have aborted the attack!
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iInterceptionDamage, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
						}
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_AND_DEATH", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
						pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
					}
					else if(iActivePlayerID == pkDefender->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pInterceptor->plot(), pkAttacker->getOwner());
						}

						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR_AND_DEATH", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str());
						pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
					}

					CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
					}

					ApplyPostKillTraitEffects(pkAttacker, pkDefender);

					// Friendship from barb death via air-strike
					if(pkDefender->isBarbarian())
					{
						DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender, pkAttacker->getOwner());
					}
				}
				// Nobody died
				else
				{
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_UNIT_BY_AIR_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iInterceptionDamage);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pInterceptor->plot(), pkAttacker->getOwner());
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
						}
					}
					else if(iActivePlayerID == pkDefender->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iInterceptionDamage, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pInterceptor->plot(), pkAttacker->getOwner());
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
						}
					}
				}

				pkDefender->setCombatUnit(NULL);
				if(!CvUnitMission::IsHeadMission(pkDefender, CvTypes::getMISSION_WAIT_FOR()))		// If the top mission was not a 'wait for', then clear it.
					pkDefender->ClearMissionQueue();
			}
		}
		else
		{
			// Target was a City
			CvCity* pCity = pkTargetPlot->getPlotCity();
			ASSERT_DEBUG(pCity != NULL);

			if(pCity)
			{
				if (pCity->isHuman())
					bTargetIsHuman = true;

				pCity->clearCombat();
				pCity->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);
				pCity->changeDamage(iAttackerDamageInflicted);
#if defined(MOD_CORE_PER_TURN_DAMAGE)
				pCity->addDamageReceivedThisTurn(iAttackerDamageInflicted, pkAttacker);
#endif
				pkAttacker->changeDamage(iDefenderDamageInflicted, pCity->getOwner());

				if(pkAttacker->IsDead())
				{
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_INTERCEPTED_KILLED", pInterceptor->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pCity->getOwner());
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING_CITY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pCity->getNameKey(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
							if (MOD_WH_MILITARY_LOG)
								MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pCity->getOwner());
						}
					}

					CvNotifications* pNotifications = GET_PLAYER(pkAttacker->getOwner()).GetNotifications();
					if (pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkAttacker->getX(), pkAttacker->getY(), (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
					}
				}

				if(iAttackerDamageInflicted > 0 || iDefenderDamageInflicted > 0)
				{
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pCity->getName().GetCString());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else if (iInterceptionDamage <= 0)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_ACTUALLY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pCity->getName().GetCString(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}
					if(iActivePlayerID == pCity->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_INTERCEPTED_CITY", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(GET_PLAYER(pCity->getOwner()).getTeam()), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pCity->getName().GetCString());
							pkDLLInterface->AddMessage(uiParentEventID, pCity->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else if (iInterceptionDamage <= 0)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR_CITY", pCity->getName().GetCString(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pCity->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}

					ApplyPostCityCombatEffects(pkAttacker, pCity, iAttackerDamageInflicted);
				}

#if defined(MOD_BALANCE_CORE_MILITARY)
				//apply damage to garrison
				CvUnitCombat::ApplyExtraUnitDamage(pkAttacker, kCombatInfo, uiParentEventID);
#endif
			}
		}
	}

	if(pkAttacker)
	{
		// Suicide Unit (e.g. Missiles)
		if(pkAttacker->isSuicide())
		{
			pkAttacker->setCombatUnit(NULL);	// Must clear this if doing a delayed kill, should this be part of the kill method?
			pkAttacker->kill(true);
		}
		else
		{
			// Experience
			if(iAttackerDamageInflicted > 0)
			{
				// only gain XP for the first attack made per turn.
				if (!MOD_BALANCE_CORE_XP_ON_FIRST_ATTACK || pkAttacker->getNumAttacksMadeThisTurn() <= 1)
				{
					pkAttacker->changeExperienceTimes100(100 * kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
						kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
						true,
						kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
						kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER),
						bTargetIsHuman);
					
					// Promotion time?
					pkAttacker->testPromotionReady();
				}
			}

			// Clean up some stuff
			pkAttacker->setCombatUnit(NULL);
			pkAttacker->ClearMissionQueue(false,GetPostCombatDelay());

			// Spend a move for this attack
			pkAttacker->changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));

			// Can't move or attack again
			if(!pkAttacker->canMoveAfterAttacking() && pkAttacker->isOutOfAttacks())
			{
				pkAttacker->finishMoves();
			}
		}
	}
	
	BATTLE_FINISHED();
}

//	---------------------------------------------------------------------------
void CvUnitCombat::GenerateAirSweepCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& plot, CvCombatInfo* pkCombatInfo)
{
	BATTLE_STARTED(BATTLE_TYPE_SWEEP, plot);

	int iAttackerMaxHP = kAttacker.GetMaxHitPoints();

	pkCombatInfo->setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, pkDefender);
	pkCombatInfo->setPlot(&plot);

	// Unit vs. Unit
	int iAttackerStrength = kAttacker.GetMaxRangedCombatStrength(pkDefender, /*pCity*/ NULL, true);

	// Mod to air sweep strength
	iAttackerStrength *= (100 + kAttacker.GetAirSweepCombatModifier());
	iAttackerStrength /= 100;

	int iDefenderStrength = 0;
	int iDefenderExperience = 0;

	// Ground AA interceptor
	if(pkDefender->getDomainType() != DOMAIN_AIR)
	{
		bool bIncludeRand = !GC.getGame().isGameMultiPlayer();
		int iInterceptionDamage = pkDefender->GetInterceptionDamage(&kAttacker, bIncludeRand, &plot);

		// Reduce damage for performing a sweep
		iInterceptionDamage *= 100 + /*-50*/ GD_INT_GET(AIR_SWEEP_INTERCEPTION_DAMAGE_MOD);
		iInterceptionDamage /= 100;

		iDefenderExperience = /*2*/ GD_INT_GET(EXPERIENCE_DEFENDING_AIR_SWEEP_GROUND);

		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iInterceptionDamage);		// Damage inflicted this round
	}
	// Air interceptor
	else
	{
		iDefenderStrength = pkDefender->GetMaxRangedCombatStrength(&kAttacker, /*pCity*/ NULL, false);
		int iDefenderMaxHP = pkDefender->GetMaxHitPoints();

		bool bIncludeRand = !GC.getGame().isGameMultiPlayer();

		int iDefenderDamageInflicted = 0; // passed by reference
		int iAttackerDamageInflicted = kAttacker.getMeleeCombatDamage(iAttackerStrength, iDefenderStrength, iDefenderDamageInflicted, /*bIncludeRand*/ bIncludeRand, pkDefender);

		iDefenderExperience = /*5*/ GD_INT_GET(EXPERIENCE_DEFENDING_AIR_SWEEP_AIR);

		pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderDamageInflicted + kAttacker.getDamage());
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);
		pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerDamageInflicted + pkDefender->getDamage());
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);

		int iAttackerEffectiveStrength = iAttackerStrength * (iAttackerMaxHP - range(kAttacker.getDamage(), 0, iAttackerMaxHP - 1)) / iAttackerMaxHP;
		iAttackerEffectiveStrength = iAttackerEffectiveStrength > 0 ? iAttackerEffectiveStrength : 1;
		int iDefenderEffectiveStrength = iDefenderStrength * (iDefenderMaxHP - range(pkDefender->getDamage(), 0, iDefenderMaxHP - 1)) / iDefenderMaxHP;
		iDefenderEffectiveStrength = iDefenderEffectiveStrength > 0 ? iDefenderEffectiveStrength : 1;

		pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, iDefenderExperience);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, pkDefender->maxXPValue());
		pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == pkDefender->getOwner());

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
		}
		else
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !kAttacker.isBarbarian());
		}
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif

		//iExperience = ((iExperience * iDefenderEffectiveStrength) / iAttackerEffectiveStrength);
		//iExperience = range(iExperience, GD_INT_GET(MIN_EXPERIENCE_PER_COMBAT), GD_INT_GET(MAX_EXPERIENCE_PER_COMBAT));
		int iExperience = /*5*/ GD_INT_GET(EXPERIENCE_ATTACKING_AIR_SWEEP);
		pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, kAttacker.maxXPValue());
		pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, true);
		}
		else
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, pkDefender->isGGFromBarbarians() || !pkDefender->isBarbarian());
		}
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !pkDefender->isBarbarian());
#endif
	}

	pkCombatInfo->setAttackIsRanged(false);
	pkCombatInfo->setAttackIsAirSweep(true);
	pkCombatInfo->setAttackerAdvances(false);
	pkCombatInfo->setDefenderRetaliates(true);

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveAirSweep(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	// After combat stuff
	CvString strBuffer;
	bool bAttackerDead = false;
	bool bDefenderDead = false;

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	if(!pkTargetPlot && pkDefender)
		pkTargetPlot = pkDefender->plot();

	ASSERT_DEBUG(pkAttacker && pkDefender && pkTargetPlot);

	// Internal variables
	int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	// Made interception!
	if(pkDefender)
	{
		pkDefender->increaseInterceptionCount();
		if(pkAttacker && pkTargetPlot)
		{
			//One Hit
			if (MOD_API_ACHIEVEMENTS && iAttackerDamageInflicted > pkDefender->GetCurrHitPoints() && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
				gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);

			pkDefender->changeDamage(iAttackerDamageInflicted, pkAttacker->getOwner());
			pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner());

			// Update experience for both sides.
			pkDefender->changeExperienceTimes100(100 *
			    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
			    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
			    true,
			    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
			    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER),
				pkAttacker->isHuman());

			pkAttacker->changeExperienceTimes100(100 * 
			    kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
			    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
			    true,
			    kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
			    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER),
				pkDefender->isHuman());

			// Anyone eat it?
			bAttackerDead = (pkAttacker->getDamage() >= pkAttacker->GetMaxHitPoints());
			bDefenderDead = (pkDefender->getDamage() >= pkDefender->GetMaxHitPoints());

			int iActivePlayerID = GC.getGame().getActivePlayer();

			//////////////////////////////////////////////////////////////////////////
			// Attacker died
			if(bAttackerDead)
			{
				CvInterfacePtr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
				gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());

				if(iActivePlayerID == pkAttacker->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted, 0);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
				}
				if(iActivePlayerID == pkDefender->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted, 0, pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
				}

				CvNotifications* pNotifications = GET_PLAYER(pkAttacker->getOwner()).GetNotifications();
				if (pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
					pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkAttacker->getX(), pkAttacker->getY(), (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
				}

				pkDefender->testPromotionReady();

				ApplyPostKillTraitEffects(pkDefender, pkAttacker);
			}
			// Defender died
			else if(bDefenderDead)
			{
				CvInterfacePtr<ICvUnit1> pDefender = GC.WrapUnitPointer(pkDefender);
				gDLL->GameplayUnitDestroyedInCombat(pDefender.get());

				if(iActivePlayerID == pkAttacker->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str());
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkDefender->getOwner());
				}
				if(iActivePlayerID == pkDefender->getOwner())
				{
					if(pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted);
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
					}
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), ((pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner()) ? NO_PLAYER : pkAttacker->getOwner()));
				}

				CvNotifications* pNotification = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
				if(pNotification)
				{
					if(pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted);
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
					}
					Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
					pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
				}

				pkAttacker->testPromotionReady();

				ApplyPostKillTraitEffects(pkAttacker, pkDefender);
			}
			// Nobody died
			else
			{
				if(iActivePlayerID == pkAttacker->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkDefender->getOwner());
				}
				if(iActivePlayerID == pkDefender->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), iDefenderDamageInflicted, pkDefender->m_strName.IsEmpty() ? pkDefender->getNameKey() : (const char*) (pkDefender->getName()).c_str(), iAttackerDamageInflicted);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					if (MOD_WH_MILITARY_LOG)
						MILITARYLOG(pkDefender->getOwner(), strBuffer.c_str(), pkDefender->plot(), pkAttacker->getOwner());
				}

				pkDefender->testPromotionReady();
				pkAttacker->testPromotionReady();
			}
		}
	}
	else
		bDefenderDead = true;

	// Clean up some stuff
	if(pkDefender)
	{
		pkDefender->setCombatUnit(NULL);
		pkDefender->ClearMissionQueue();
	}

	if(pkAttacker)
	{
		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(false,GetPostCombatDelay());

		// Spend a move for this attack
		pkAttacker->changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));

		// Can't move or attack again
		if(!pkAttacker->canMoveAfterAttacking() && pkAttacker->isOutOfAttacks())
		{
			pkAttacker->finishMoves();
		}
	}
	
	BATTLE_FINISHED();
}

//	GenerateNuclearCombatInfo
//	Function: GenerateNuclearCombatInfo
//	Take the input parameters and fill in a CvCombatInfo definition assuming a
//	mission to do a nuclear attack.
//
//	Parameters:
//		pkDefender   	-	Defending unit.  Can be null, in which case the input plot must have a city
//		plot         	-	The plot of the defending unit/city
//		pkCombatInfo 	-	Output combat info
//	---------------------------------------------------------------------------
void CvUnitCombat::GenerateNuclearCombatInfo(CvUnit& kAttacker, CvPlot& plot, CvCombatInfo* pkCombatInfo)
{
	BATTLE_STARTED(BATTLE_TYPE_NUKE, plot);
	pkCombatInfo->setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, NULL);
	pkCombatInfo->setPlot(&plot);

	if (plot.isCity())
	{
		BATTLE_JOINED(plot.getPlotCity(), BATTLE_UNIT_DEFENDER, true);
	}
	else
	{
		CvUnit* pBestDefender = plot.getBestDefender(NO_PLAYER, kAttacker.getOwner());
		BATTLE_JOINED(pBestDefender, BATTLE_UNIT_DEFENDER, false);
	}

	// Any interception to be done?
	CvCity* pInterceptionCity = plot.GetNukeInterceptor(kAttacker.getOwner());
	bool bInterceptionSuccess = false;
	bool bPartialInterception = false;
	CvPlot* pInterceptionCityPlot = NULL;

	if (pInterceptionCity != NULL)
	{
		pInterceptionCityPlot = pInterceptionCity->plot();
		if (pInterceptionCityPlot)
		{
			BATTLE_JOINED(plot.getPlotCity(), BATTLE_UNIT_DEFENDER, true);
			pkCombatInfo->setCity(BATTLE_UNIT_DEFENDER, pInterceptionCity);

			if (GC.getGame().randRangeInclusive(1, 100, CvSeeder(plot.GetPseudoRandomSeed()).mix(kAttacker.GetID())) <= pInterceptionCity->getNukeInterceptionChance())
			{
				bInterceptionSuccess = true;
			}
			if (bInterceptionSuccess)
			{
				if (kAttacker.GetNukeDamageLevel() == 1) // Atomic Bombs are destroyed outright
				{
					pkCombatInfo->setDamageInflicted(BATTLE_UNIT_INTERCEPTOR, kAttacker.GetCurrHitPoints());
					kAttacker.kill(true, pInterceptionCity->getOwner());
				}
				else // Nuclear Missiles are converted to Atomic Bombs
				{
					bPartialInterception = true;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////

	bool abTeamsAffected[MAX_TEAMS];
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		abTeamsAffected[iI] = kAttacker.isNukeVictim(&plot, ((TeamTypes)iI));
	}

	int iPlotTeam = plot.getTeam();
	bool bWar = false;
	bool bBystander = false;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (abTeamsAffected[iI])
		{
			if (!kAttacker.isEnemy((TeamTypes)iI))
			{
				if (GET_TEAM((TeamTypes)iI).IsVassalOfSomeone())
				{
					GET_PLAYER(kAttacker.getOwner()).GetDiplomacyAI()->DeclareWar(GET_TEAM((TeamTypes)iI).GetMaster());
				}
				else
				{
					GET_PLAYER(kAttacker.getOwner()).GetDiplomacyAI()->DeclareWar((TeamTypes)iI);
				}

				if (iPlotTeam == iI) 
				{
					bWar = true;
				} 
				else 
				{
					bBystander = true;
				}
			}
		}
	}

	if (MOD_EVENTS_NUCLEAR_DETONATION) 
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_NuclearDetonation, kAttacker.getOwner(), plot.getX(), plot.getY(), bWar, bBystander);
	} 
	else
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{	
			CvLuaArgsHandle args;

			args->Push(kAttacker.getOwner());
			args->Push(plot.getX());
			args->Push(plot.getY());
			args->Push(bWar);
			args->Push(bBystander);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "NuclearDetonation", args.get(), bResult);
		}
	}

	kAttacker.setReconPlot(&plot);

	//////////////////////////////////////////////////////////////////////

	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, 0);	// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, 0);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);	// Damage inflicted this round

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() != kAttacker.getOwner());	// Not really correct
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !kAttacker.isBarbarian());

	pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, 0);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, 0);
	pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, false);

	if (bInterceptionSuccess)
	{
		// Send out notifications to the world
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (GET_PLAYER(eLoopPlayer).isObserver() || (GET_PLAYER(eLoopPlayer).isHuman() && GET_PLAYER(eLoopPlayer).isAlive()))
			{
				if (!GET_PLAYER(eLoopPlayer).isObserver())
				{
					if (!GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isHasMet(GET_PLAYER(pInterceptionCity->getOwner()).getTeam()))
						continue;

					if (!GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isHasMet(GET_PLAYER(kAttacker.getOwner()).getTeam()))
						continue;
				}

				CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
				if (pNotifications)
				{
					if (!bPartialInterception)
					{
						if (eLoopPlayer == kAttacker.getOwner())
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_ATOMIC_BOMB_INTERCEPTED_US_S");
							Localization::String strBuffer = Localization::Lookup("TXT_KEY_ATOMIC_BOMB_INTERCEPTED_US");
							strBuffer << pInterceptionCity->getNameKey();
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pInterceptionCity->getX(), pInterceptionCity->getY(), (int)kAttacker.getUnitType(), pInterceptionCity->getOwner());
						}
						else if (GET_PLAYER(eLoopPlayer).isObserver() || pInterceptionCityPlot->isRevealed(GET_PLAYER(eLoopPlayer).getTeam()))
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_ATOMIC_BOMB_INTERCEPTED_S");
							Localization::String strBuffer = Localization::Lookup("TXT_KEY_ATOMIC_BOMB_INTERCEPTED");
							strBuffer << GET_PLAYER(kAttacker.getOwner()).getCivilizationAdjectiveKey();
							strBuffer << pInterceptionCity->getNameKey();
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pInterceptionCity->getX(), pInterceptionCity->getY(), (int)kAttacker.getUnitType(), pInterceptionCity->getOwner());
						}
						else
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_ATOMIC_BOMB_INTERCEPTED_S");
							Localization::String strBuffer = Localization::Lookup("TXT_KEY_ATOMIC_BOMB_INTERCEPTED");
							strBuffer << GET_PLAYER(kAttacker.getOwner()).getCivilizationShortDescription();
							strBuffer << pInterceptionCity->getNameKey();
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), -1, -1, (int)kAttacker.getUnitType(), pInterceptionCity->getOwner());							
						}
					}
					else
					{
						if (eLoopPlayer == kAttacker.getOwner())
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NUCLEAR_MISSILE_INTERCEPTED_US_S");
							Localization::String strBuffer = Localization::Lookup("TXT_KEY_NUCLEAR_MISSILE_INTERCEPTED_US");
							strBuffer << pInterceptionCity->getNameKey();
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pInterceptionCity->getX(), pInterceptionCity->getY(), (int)kAttacker.getUnitType(), pInterceptionCity->getOwner());
						}
						else if (GET_PLAYER(eLoopPlayer).isObserver() || pInterceptionCityPlot->isRevealed(GET_PLAYER(eLoopPlayer).getTeam()))
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NUCLEAR_MISSILE_INTERCEPTED_S");
							Localization::String strBuffer = Localization::Lookup("TXT_KEY_NUCLEAR_MISSILE_INTERCEPTED");
							strBuffer << GET_PLAYER(kAttacker.getOwner()).getCivilizationAdjectiveKey();
							strBuffer << pInterceptionCity->getNameKey();
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pInterceptionCity->getX(), pInterceptionCity->getY(), (int)kAttacker.getUnitType(), pInterceptionCity->getOwner());
						}
						else
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NUCLEAR_MISSILE_INTERCEPTED_S");
							Localization::String strBuffer = Localization::Lookup("TXT_KEY_NUCLEAR_MISSILE_INTERCEPTED");
							strBuffer << GET_PLAYER(kAttacker.getOwner()).getCivilizationShortDescription();
							strBuffer << pInterceptionCity->getNameKey();
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), -1, -1, (int)kAttacker.getUnitType(), pInterceptionCity->getOwner());							
						}
					}
				}
			}
		}

		// If completely intercepted, apply diplomacy penalty and end here.
		if (!bPartialInterception)
		{
			if (GET_PLAYER(pInterceptionCity->getOwner()).isMajorCiv() && GET_PLAYER(kAttacker.getOwner()).isMajorCiv())
				GET_PLAYER(pInterceptionCity->getOwner()).GetDiplomacyAI()->ChangeNumTimesNuked(kAttacker.getOwner(), 1);

			return;
		}
	}

	int iNukeDamageLevel = bPartialInterception ? 1 : kAttacker.GetNukeDamageLevel();
	pkCombatInfo->setAttackIsBombingMission(true);
	pkCombatInfo->setDefenderRetaliates(false);
	pkCombatInfo->setAttackNuclearLevel(iNukeDamageLevel + 1);

	// Set all of the units in the blast radius to defenders and calculate their damage
	int iDamageMembers = GenerateNuclearExplosionDamage(&plot, iNukeDamageLevel, &kAttacker, pkCombatInfo->getDamageMembers(), pkCombatInfo->getMaxDamageMemberCount());
	pkCombatInfo->setDamageMemberCount(iDamageMembers);

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	-------------------------------------------------------------------------------------
uint CvUnitCombat::ApplyNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* /* pkAttacker = NULL*/)
{
	CvCombatMemberEntry kDamageMembers[MAX_NUKE_DAMAGE_MEMBERS];
	int iDamageMembers = GenerateNuclearExplosionDamage(pkTargetPlot, iDamageLevel, NULL, &kDamageMembers[0], MAX_NUKE_DAMAGE_MEMBERS);
	return ApplyNuclearExplosionDamage(&kDamageMembers[0], iDamageMembers, NULL, pkTargetPlot, iDamageLevel);
}

//	-------------------------------------------------------------------------------------
uint CvUnitCombat::ApplyNuclearExplosionDamage(const CvCombatMemberEntry* pkDamageArray, int iDamageMembers, CvUnit* pkAttacker, CvPlot* pkTargetPlot, int iDamageLevel)
{
	uint uiOpposingDamageCount = 0;
	PlayerTypes eAttackerOwner = pkAttacker?pkAttacker->getOwner():NO_PLAYER;
	std::vector<PlayerTypes> vAffectedPlayers;

	// Do all the units first
	for(int i = 0; i < iDamageMembers; ++i)
	{
		const CvCombatMemberEntry& kEntry = pkDamageArray[i];
		if(kEntry.IsUnit())
		{
			CvUnit* pkUnit = GET_PLAYER(kEntry.GetPlayer()).getUnit(kEntry.GetUnitID());
			if(pkUnit && !pkUnit->isNukeImmune())
			{
				// Apply the damage
				pkUnit->setCombatUnit(NULL);
				pkUnit->ClearMissionQueue();
				pkUnit->SetAutomateType(NO_AUTOMATE); // kick unit out of automation

				if((eAttackerOwner == NO_PLAYER || pkUnit->getOwner() != eAttackerOwner) && !pkUnit->isBarbarian())
					uiOpposingDamageCount++;	// Count the number of non-barbarian opposing units

				if(pkUnit->IsCombatUnit() || pkUnit->IsCanAttackRanged())
				{
					pkUnit->changeDamage(kEntry.GetDamage(), eAttackerOwner);
				}
				else if(kEntry.GetDamage() >= /*6*/ GD_INT_GET(NUKE_NON_COMBAT_DEATH_THRESHOLD))
				{
					pkUnit->kill(false, eAttackerOwner);
				}

				if (GET_PLAYER(kEntry.GetPlayer()).isMajorCiv() && GET_PLAYER(kEntry.GetPlayer()).isAlive())
				{
					if (std::find(vAffectedPlayers.begin(), vAffectedPlayers.end(), kEntry.GetPlayer()) == vAffectedPlayers.end())
						vAffectedPlayers.push_back(kEntry.GetPlayer());
				}
			}
		}
	}

	// Then the terrain effects
	int iBlastRadius = /*2*/ range(GD_INT_GET(NUKE_BLAST_RADIUS), 1, 5);

	for(int iDX = -(iBlastRadius); iDX <= iBlastRadius; iDX++)
	{
		for(int iDY = -(iBlastRadius); iDY <= iBlastRadius; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pkTargetPlot->getX(), pkTargetPlot->getY(), iDX, iDY, iBlastRadius);

			if(pLoopPlot != NULL)
			{
				// if we remove roads, don't remove them on the city... XXX
				CvCity* pLoopCity = pLoopPlot->getPlotCity();

				if(pLoopCity == NULL)
				{
					if(!pLoopPlot->isWater() && pLoopPlot->isValidMovePlot( pLoopPlot->getOwner() ))
					{
						if(pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(pLoopPlot->getFeatureType());
							if(pkFeatureInfo && !pkFeatureInfo->isNukeImmune())
							{
								if (pLoopPlot == pkTargetPlot || GC.getGame().randRangeExclusive(0, 100, CvSeeder(pLoopPlot->GetPseudoRandomSeed())) < /*50*/ GD_INT_GET(NUKE_FALLOUT_PROB))
								{
									if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
									{
										pLoopPlot->SetImprovementPillaged(true);
									}
									pLoopPlot->setFeatureType((FeatureTypes)(GD_INT_GET(NUKE_FEATURE)));
								}
							}
						}
						else
						{
							if(pLoopPlot == pkTargetPlot || GC.getGame().randRangeExclusive(0, 100, CvSeeder(pLoopPlot->GetPseudoRandomSeed())) < /*50*/ GD_INT_GET(NUKE_FALLOUT_PROB))
							{
								if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
								{
									pLoopPlot->SetImprovementPillaged(true);
								}
								pLoopPlot->setFeatureType((FeatureTypes)(GD_INT_GET(NUKE_FEATURE)));
							}
						}
#if defined(MOD_GLOBAL_NUKES_MELT_ICE)
					} else if (MOD_GLOBAL_NUKES_MELT_ICE && pLoopPlot->getFeatureType() == FEATURE_ICE) {
						if (pLoopPlot == pkTargetPlot || GC.getGame().randRangeExclusive(0, 100, CvSeeder(pLoopPlot->GetPseudoRandomSeed())) < /*50*/ GD_INT_GET(NUKE_FALLOUT_PROB)) {
							pLoopPlot->setFeatureType(NO_FEATURE);
						}
#endif
					}
				}
			}
		}
	}

	// Then the cities
	for(int i = 0; i < iDamageMembers; ++i)
	{
		const CvCombatMemberEntry& kEntry = pkDamageArray[i];
		if(kEntry.IsCity())
		{
			CvCity* pkCity = GET_PLAYER(kEntry.GetPlayer()).getCity(kEntry.GetCityID());
			if(pkCity)
			{
				pkCity->setCombatUnit(NULL);

				if(eAttackerOwner == NO_PLAYER || pkCity->getOwner() != eAttackerOwner)
					uiOpposingDamageCount++;

				if (GET_PLAYER(pkCity->getOwner()).isMajorCiv() && GET_PLAYER(pkCity->getOwner()).isAlive())
				{
					if (std::find(vAffectedPlayers.begin(), vAffectedPlayers.end(), pkCity->getOwner()) == vAffectedPlayers.end())
						vAffectedPlayers.push_back(pkCity->getOwner());
				}

				// Putting this here, since the city may not exist later
				ApplyPostCityCombatEffects(pkAttacker, pkCity, kEntry.GetDamage());

				if(kEntry.GetFinalDamage() >= pkCity->GetMaxHitPoints() && !pkCity->IsOriginalCapital())
				{
					CvInterfacePtr<ICvCity1> pkDllCity(new CvDllCity(pkCity));
					gDLL->GameplayCitySetDamage(pkDllCity.get(), 0, pkCity->getDamage()); // to stop the fires
					gDLL->GameplayCityDestroyed(pkDllCity.get(), NO_PLAYER);

					PlayerTypes eOldOwner = pkCity->getOwner();
					pkCity->kill();

					// slewis - check for killing a player
					GET_PLAYER(pkAttacker->getOwner()).CheckForMurder(eOldOwner);
				}
				else
				{
					// Unlike the city hit points, the population damage is calculated when the pre-calculated damage is applied.
					// This is simply to save space in the damage array, since the combat visualization does not need it.
					// It can be moved into the pre-calculated damage array if needed.
					int iBaseDamage = 0;
					int iRandDamage1 = 0;
					int iRandDamage2 = 0;
					// How much destruction is unleashed on nearby Cities?
					if(iDamageLevel == 1)
					{
						iBaseDamage = /*30*/ GD_INT_GET(NUKE_LEVEL1_POPULATION_DEATH_BASE);
						iRandDamage1 = GC.getGame().randRangeExclusive(0, /*20*/ GD_INT_GET(NUKE_LEVEL1_POPULATION_DEATH_RAND_1), CvSeeder(pkCity->getPopulation()).mix(i));
						iRandDamage2 = GC.getGame().randRangeExclusive(0, /*20*/ GD_INT_GET(NUKE_LEVEL1_POPULATION_DEATH_RAND_2), CvSeeder(pkCity->GetPower()).mix(i));
					}
					else
					{
						iBaseDamage = /*60*/ GD_INT_GET(NUKE_LEVEL2_POPULATION_DEATH_BASE);
						iRandDamage1 = GC.getGame().randRangeExclusive(0, /*10*/ GD_INT_GET(NUKE_LEVEL2_POPULATION_DEATH_RAND_1), CvSeeder(pkCity->getPopulation()).mix(i));
						iRandDamage2 = GC.getGame().randRangeExclusive(0, /*10*/ GD_INT_GET(NUKE_LEVEL2_POPULATION_DEATH_RAND_2), CvSeeder(pkCity->GetPower()).mix(i));
					}

					int iNukedPopulation = pkCity->getPopulation() * (iBaseDamage + iRandDamage1 + iRandDamage2) / 100;

					iNukedPopulation *= std::max(0, (pkCity->getNukeModifier() + 100));
					iNukedPopulation /= 100;

					pkCity->changePopulation(-(std::min((pkCity->getPopulation() - 1), iNukedPopulation)));

					// Add damage to the city
					pkCity->setDamage(kEntry.GetFinalDamage());
				}
			}
		}
	}

	// Send out notifications to the world
	for (std::vector<PlayerTypes>::iterator iter = vAffectedPlayers.begin(); iter != vAffectedPlayers.end(); ++iter)
	{
		if (*iter == pkAttacker->getOwner())
			continue;

		if (GET_PLAYER(pkAttacker->getOwner()).isMajorCiv() && GET_PLAYER(pkAttacker->getOwner()).getTeam() != GET_PLAYER(*iter).getTeam())
		{
			GET_PLAYER(*iter).GetDiplomacyAI()->ChangeNumTimesNuked(pkAttacker->getOwner(), 1);
		}

		if (GET_PLAYER(*iter).isHuman() && GET_PLAYER(*iter).isAlive())
		{
			CvNotifications* pNotifications = GET_PLAYER(*iter).GetNotifications();
			if (pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NUKE_STRIKE_AFFECTED_S");
				Localization::String strBuffer = Localization::Lookup("TXT_KEY_NUKE_STRIKE_AFFECTED");
				strBuffer << GET_PLAYER(pkAttacker->getOwner()).getCivilizationAdjectiveKey();
				pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pkTargetPlot->getX(), pkTargetPlot->getY(), (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
			}
		}
	}
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(eLoopPlayer).isObserver() || (GET_PLAYER(eLoopPlayer).isHuman() && GET_PLAYER(eLoopPlayer).isAlive()))
		{
			if (!GET_PLAYER(eLoopPlayer).isObserver())
			{
				if (eLoopPlayer == pkAttacker->getOwner())
					continue;

				if (pkTargetPlot->isOwned() && !pkTargetPlot->isRevealed(GET_PLAYER(eLoopPlayer).getTeam()) && !GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isHasMet(GET_PLAYER(pkTargetPlot->getOwner()).getTeam()))
					continue;

				if (std::find(vAffectedPlayers.begin(), vAffectedPlayers.end(), eLoopPlayer) != vAffectedPlayers.end())
					continue;
			}

			CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
			if (pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NUKE_STRIKE_S");

				if (pkTargetPlot->isOwned())
				{
					Localization::String strBuffer = Localization::Lookup("TXT_KEY_NUKE_STRIKE_OWNED_TERRITORY");
					strBuffer << GET_PLAYER(pkAttacker->getOwner()).getCivilizationShortDescription();
					strBuffer << GET_PLAYER(pkTargetPlot->getOwner()).getCivilizationShortDescription();

					if (pkTargetPlot->isRevealed(GET_PLAYER(eLoopPlayer).getTeam()))
					{
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pkTargetPlot->getX(), pkTargetPlot->getY(), (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
					}
					else
					{
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), -1, -1, (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
					}
				}
				else
				{
					Localization::String strBuffer = Localization::Lookup("TXT_KEY_NUKE_STRIKE_UNOWNED_TERRITORY");
					strBuffer << GET_PLAYER(pkAttacker->getOwner()).getCivilizationShortDescription();

					if (pkTargetPlot->isRevealed(GET_PLAYER(eLoopPlayer).getTeam()))
					{
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), pkTargetPlot->getX(), pkTargetPlot->getY(), (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
					}
					else
					{
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer.toUTF8(), strSummary.toUTF8(), -1, -1, (int)pkAttacker->getUnitType(), pkAttacker->getOwner());
					}
				}
			}
		}
	}

	return uiOpposingDamageCount;
}

//	-------------------------------------------------------------------------------------
//	Generate nuclear explosion damage for all the units and cities in the radius of the specified plot.
//	The attacker is optional - this is also called for a meltdown
//	Returns the number of damaged entities (cities or units)
int CvUnitCombat::GenerateNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* pkAttacker, CvCombatMemberEntry* pkDamageArray, const int iMaxDamageMembers)
{
	const int iBlastRadius = /*2*/ range(GD_INT_GET(NUKE_BLAST_RADIUS), 1, 5);

	CvCity* pDefenderCity = NULL;
	CvUnit* pDefenderUnit = NULL;
	if (MOD_EVENTS_BATTLES)
	{
		if (pkTargetPlot->isCity())
		{
			pDefenderCity = pkTargetPlot->getPlotCity();
		}
		else
		{
			pDefenderUnit = pkTargetPlot->getBestDefender(NO_PLAYER, pkAttacker->getOwner());
		}
	}

	int iDamageMembers = 0;

	for (int iDX = -iBlastRadius; iDX <= iBlastRadius; iDX++)
	{
		for (int iDY = -iBlastRadius; iDY <= iBlastRadius; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pkTargetPlot->getX(), pkTargetPlot->getY(), iDX, iDY, iBlastRadius);
			if (pLoopPlot)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();

				FFastSmallFixedList<IDInfo, 25, true, c_eCiv5GameplayDLL> oldUnits;
				IDInfo* pUnitNode = pLoopPlot->headUnitNode();
				while (pUnitNode)
				{
					oldUnits.insertAtEnd(pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();
				while (pUnitNode)
				{
					CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
					pUnitNode = oldUnits.next(pUnitNode);

					if (pLoopUnit && pLoopUnit != pkAttacker && !pLoopUnit->isNukeImmune() && !pLoopUnit->isDelayedDeath())
					{
						bool bTradeUnit = pLoopUnit->isTrade();
						if (bTradeUnit)
						{
							CvPlayer& kPlayer = GET_PLAYER(pLoopUnit->getOwner());
							CorporationTypes eCorporation = kPlayer.GetCorporations()->GetFoundedCorporation();
							if (eCorporation != NO_CORPORATION)
							{
								CvCorporationEntry* pkCorporationInfo = GC.getCorporationInfo(eCorporation);
								if (pkCorporationInfo && pkCorporationInfo->IsTradeRoutesInvulnerable())
								{
									// If the nuclear strike destroyed the corporate HQ by destroying the city it's in, the trade unit should also die.
									// Outside of this rare circumstance, the trade unit is invulnerable.
									CvCity* pHQ = kPlayer.GetCorporations()->GetHeadquarters();

									// City isn't being hit
									if (plotDistance(*pHQ->plot(), *pkTargetPlot) > iBlastRadius)
										continue;

									// City won't be destroyed by this nuke
									if (!pHQ->IsNukeKillable(iDamageLevel))
										continue;
								}
							}
						}

						// Nuke level 1: units on impact plot are dealt lethal damage unless in a protected city; those in blast radius are dealt random damage
						// Nuke level 2: all units on blast radius are dealt lethal damage unless in a protected city
						// Trade units are always killed unless invulnerable
						int iNukeDamage = 0;
						if (iDamageLevel >= 2 || pLoopPlot == pkTargetPlot || bTradeUnit)
						{
							iNukeDamage = pLoopUnit->GetMaxHitPoints();
						}
						else
						{
							iNukeDamage = /*30*/ GD_INT_GET(NUKE_UNIT_DAMAGE_BASE) + /*40*/ GC.getGame().randRangeExclusive(0, GD_INT_GET(NUKE_UNIT_DAMAGE_RAND_1), CvSeeder(pLoopPlot->GetPseudoRandomSeed())) + /*40*/ GC.getGame().randRangeExclusive(0, GD_INT_GET(NUKE_UNIT_DAMAGE_RAND_2), CvSeeder(pLoopUnit->GetID()).mix(iDX).mix(iDY));
						}

						if (pLoopCity && !bTradeUnit)
						{
							iNukeDamage *= max(0, pLoopCity->getNukeModifier() + 100);
							iNukeDamage /= 100;
						}

						CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkDamageArray, &iDamageMembers, iMaxDamageMembers, pLoopUnit);
						if (pkDamageEntry)
						{
							if (pLoopUnit != pDefenderUnit)
								BATTLE_JOINED(pLoopUnit, BATTLE_UNIT_COUNT, false); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER

							pkDamageEntry->SetDamage(iNukeDamage);
							pkDamageEntry->SetFinalDamage(min(iNukeDamage + pLoopUnit->getDamage(), pLoopUnit->GetMaxHitPoints()));
							pkDamageEntry->SetMaxHitPoints(pLoopUnit->GetMaxHitPoints());

							if (pkAttacker)
								pLoopUnit->setCombatUnit(pkAttacker);
						}
					}
				}

				if (pLoopCity)
				{
					int iTotalDamage = 0;
					if (pLoopCity->IsNukeKillable(iDamageLevel))
					{
						iTotalDamage = pLoopCity->GetMaxHitPoints();
					}
					else
					{
						// Add damage to the city
						iTotalDamage = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) * /*50*/ GD_INT_GET(NUKE_CITY_HIT_POINT_DAMAGE);
						iTotalDamage /= 100;

						iTotalDamage += pLoopCity->getDamage();

						// Can't bring a city below 1 HP
						iTotalDamage = min(iTotalDamage, pLoopCity->GetMaxHitPoints() - 1);
					}

					CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkDamageArray, &iDamageMembers, iMaxDamageMembers, pLoopCity);
					if (pkDamageEntry)
					{
						if (pLoopCity != pDefenderCity)
							BATTLE_JOINED(pLoopCity, BATTLE_UNIT_COUNT, true); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER

						pkDamageEntry->SetDamage(iTotalDamage - pLoopCity->getDamage());
						pkDamageEntry->SetFinalDamage(iTotalDamage);
						pkDamageEntry->SetMaxHitPoints(pLoopCity->GetMaxHitPoints());

						if (pkAttacker)
							pLoopCity->setCombatUnit(pkAttacker);
					}
				}
			}
		}
	}

	return iDamageMembers;
}

//	---------------------------------------------------------------------------
//	Function: ResolveNuclearCombat
//	Resolve combat from a nuclear attack.
void CvUnitCombat::ResolveNuclearCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	UNREFERENCED_PARAMETER(uiParentEventID);

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	ASSERT_DEBUG(pkAttacker);

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	ASSERT_DEBUG(pkTargetPlot);

	CvString strBuffer;

	GC.getGame().changeNukesExploded(1);

	if(pkAttacker && !pkAttacker->isDelayedDeath())
	{
		// Make sure we are disconnected from any unit transporting the attacker (i.e. it's a missile)
		pkAttacker->setTransportUnit(NULL);

		if(pkTargetPlot)
		{
			if(ApplyNuclearExplosionDamage(kCombatInfo.getDamageMembers(), kCombatInfo.getDamageMemberCount(), pkAttacker, pkTargetPlot, kCombatInfo.getAttackNuclearLevel() - 1) > 0)
			{
				if (MOD_API_ACHIEVEMENTS && pkAttacker->getOwner() == GC.getGame().getActivePlayer())
				{
					// Must damage someone to get the achievement.
					gDLL->UnlockAchievement(ACHIEVEMENT_DROP_NUKE);

					if (GC.getGame().getGameTurnYear() == 2012)
					{
						CvPlayerAI& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());
						if(strncmp(kPlayer.getCivilizationTypeKey(), "CIVILIZATION_MAYA", 32) == 0)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_36);
						}
					}
				}

#if defined(MOD_EVENTS_NUCLEAR_DETONATION)
				// While we should really send the NuclearDetonation event here, we don't still have all the info at this point
				// so we send it while calculating the combat info, just after we declare war (if appropriate) from firing one
#endif
			}
		}

		// Suicide Unit (currently all nuclear attackers are)
		if(pkAttacker->isSuicide())
		{
			pkAttacker->setCombatUnit(NULL);	// Must clear this if doing a delayed kill, should this be part of the kill method?
			pkAttacker->setAttackPlot(NULL, false);
			pkAttacker->kill(true);
		}
		else
		{
			ASSERT_DEBUG(pkAttacker->isSuicide(), "A nuke unit that is not a one time use?");

			// Clean up some stuff
			pkAttacker->setCombatUnit(NULL);
			pkAttacker->ClearMissionQueue(false,GetPostCombatDelay());
			pkAttacker->SetAutomateType(NO_AUTOMATE); // kick unit out of automation

			// Spend a move for this attack
			pkAttacker->changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));

			// Can't move or attack again
			if(!pkAttacker->canMoveAfterAttacking() && pkAttacker->isOutOfAttacks())
			{
				pkAttacker->finishMoves();
			}
		}
	}
	
	BATTLE_FINISHED();
}

#if defined(MOD_GLOBAL_PARATROOPS_AA_DAMAGE)
//	---------------------------------------------------------------------------
bool CvUnitCombat::ParadropIntercept(CvUnit& paraUnit, CvPlot& dropPlot) {
	ASSERT_DEBUG(!paraUnit.isDelayedDeath(), "Trying to paradrop and the unit is already dead!");
	ASSERT_DEBUG(paraUnit.getCombatTimer() == 0);

	// Any interception to be done?
	CvUnit* pInterceptor = dropPlot.GetBestInterceptor(paraUnit.getOwner(), &paraUnit);
	if (pInterceptor) {
		uint uiParentEventID = 0;
		int iInterceptionDamage = 0;

		// Is the interception successful?
		if(GC.getGame().randRangeExclusive(0, 100, CvSeeder(dropPlot.GetPseudoRandomSeed()).mix(paraUnit.GetID()).mix(paraUnit.getDamage())) < pInterceptor->interceptionProbability())
		{
			iInterceptionDamage = pInterceptor->GetInterceptionDamage(&paraUnit, true, &dropPlot);
		}
	
		if (iInterceptionDamage > 0) {
#if defined(MOD_EVENTS_BATTLES)
			if (MOD_EVENTS_BATTLES) {
				BATTLE_STARTED(BATTLE_TYPE_PARADROP, dropPlot);
				BATTLE_JOINED(&paraUnit, BATTLE_UNIT_ATTACKER, false);
				BATTLE_JOINED(pInterceptor, BATTLE_UNIT_INTERCEPTOR, false);

				if (MOD_EVENTS_BATTLES_DAMAGE) {
					int iValue = 0;
					if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_BattleDamageDelta, BATTLE_UNIT_INTERCEPTOR, iInterceptionDamage) == GAMEEVENTRETURN_VALUE) {
						if (iValue != 0) {
							if (iValue < 0) {
								// Decreasing the amount of damage, in which case it can't be more than the amount inflicted (as that's called 'healing'!)
								if (iInterceptionDamage + iValue < 0) {
									iValue = -iInterceptionDamage;
								}
							} else {
								// Increasing the amount of damage, in which case we can't exceed unit's hit points
								if (iInterceptionDamage + iValue > paraUnit.GetCurrHitPoints()) {
									iValue = paraUnit.GetCurrHitPoints() - iInterceptionDamage;
								}
							}
				
							iInterceptionDamage += iValue;
						}
					}
				}
			}
#endif

			CUSTOMLOG("Paradrop: hostile AA did %i damage", iInterceptionDamage);
			// Play the AA animations here ... but without an air attacker it's just not possible!!!
			// if (!CvPreGame::quickCombat()) {
				// Center camera here (pDropPlot MUST be visible or couldn't drop onto it!)
				// CvInterfacePtr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&pDropPlot);
				// GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
				// kCombatInfo.setVisualizeCombat(true);

				// CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
				// uiParentEventID = gDLL->GameplayUnitCombat(pDllCombatInfo.get());

				// Set the combat units so that other missions do not continue until combat is over.
				// pInterceptor->setCombatUnit(&pParaUnit, false);
			// }

			pInterceptor->increaseInterceptionCount();
			pInterceptor->setCombatUnit(NULL);

			// Killing the unit during the drop is a really bad idea, the game crashes at random after the drop
			int iEffectiveDamage = std::min(paraUnit.GetCurrHitPoints() - 1, iInterceptionDamage);
			paraUnit.changeDamage(iEffectiveDamage, pInterceptor->getOwner());

			if (GC.getGame().getActivePlayer() == paraUnit.getOwner()) 
			{
				CvString strBuffer;

				if (paraUnit.IsDead()) //cannot occur, see above
					strBuffer = GetLocalizedText("TXT_KEY_PARADROP_AA_KILLED", paraUnit.getNameKey(), pInterceptor->getNameKey());
				else
					strBuffer = GetLocalizedText("TXT_KEY_PARADROP_AA_DAMAGED", paraUnit.getNameKey(), pInterceptor->getNameKey(), iInterceptionDamage);

				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, paraUnit.getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer);
				if (MOD_WH_MILITARY_LOG)
					MILITARYLOG(paraUnit.getOwner(), strBuffer.c_str(), paraUnit.plot(), pInterceptor->getOwner());
			}
			
			BATTLE_FINISHED();
		}
	}
	
	return paraUnit.IsDead();
}
#endif

//result is times 100
int CvUnitCombat::DoDamageMath(int iAttackerStrength100, int iDefenderStrength100, int iDefaultDamage100, int iMaxRandomDamage100, bool bIncludeRand, const CvSeeder& randomSeed, int iModifierPercent)
{
	// Base damage for two units of identical strength
	int iDamage = iDefaultDamage100;

	// Don't use rand when calculating projected combat results
	if (bIncludeRand && iMaxRandomDamage100 > 0)
	{
		iDamage += GC.getGame().randRangeExclusive(0, iMaxRandomDamage100, randomSeed);
	}
	else
	{
		//just use the expected value of the random roll
		iDamage += iMaxRandomDamage100 / 2;
	}

	// Calculations performed to dampen amount of damage by units that are close in strength
	// RATIO = (((((ME / OPP) + 3) / 4) ^ 4) + 1) / 2
	// Examples:
	// 1.301 = (((((9 / 6) + 3) / 4) ^ 4) + 1 / 2
	// 17.5 = (((((40 / 6) + 3) / 4) ^ 4) + 1 / 2

	// In case our strength is less than the other guy's, we'll do things in reverse then make the ratio 1 over the result (we need a # above 1.0)
	double fStrengthRatio = (iDefenderStrength100 > iAttackerStrength100) ?
		(double(iDefenderStrength100) / max(1,iAttackerStrength100)) : (double(iAttackerStrength100) / max(1,iDefenderStrength100));

	fStrengthRatio = (fStrengthRatio + 3) / 4;
	fStrengthRatio = pow(fStrengthRatio, 4.0);
	//avoid overflows later ...
	fStrengthRatio = MIN(1e3, (fStrengthRatio + 1) / 2);

	//undo the inversion if needed
	if(iDefenderStrength100 > iAttackerStrength100)
		fStrengthRatio = 1 / fStrengthRatio;

	//this is it
	iDamage = int(iDamage * fStrengthRatio);
	if(iModifierPercent!=0)
	{
		iDamage *= (100+iModifierPercent);
		iDamage /= 100;
	}

	return max(iDamage,0);
}

//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveCombat(const CvCombatInfo& kInfo, uint uiParentEventID /* = 0 */)
{
	PlayerTypes eAttackingPlayer = NO_PLAYER;
	// Restore visibility
	CvUnit* pAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER);

	const TeamTypes eActiveTeam = GC.getGame().getActiveTeam();

	if(pAttacker)
	{
		eAttackingPlayer = pAttacker->getOwner();
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(pAttacker));
		gDLL->GameplayUnitVisibility(pDllUnit.get(), !pAttacker->isInvisible(eActiveTeam, false));
	}

	CvUnit* pDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER);
	if(pDefender)
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(pDefender));
		gDLL->GameplayUnitVisibility(pDllUnit.get(), !pDefender->isInvisible(eActiveTeam, false));
	}

	CvUnit* pDefenderSupport = kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
	if(pDefenderSupport)
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(pDefenderSupport));
		gDLL->GameplayUnitVisibility(pDllUnit.get(), !pDefenderSupport->isInvisible(eActiveTeam, false));
	}

	bool bCanAttack = true;
	if (MOD_EVENTS_RED_COMBAT_ABORT || MOD_EVENTS_RED_COMBAT_RESULT) {
		// RED <<<<<

		// CombatResult
		// iAttackingPlayer, iAttackingUnit, attackerDamage, attackerFinalDamage, attackerMaxHP
		// iDefendingPlayer, iDefendingUnit, defenderDamage, defenderFinalDamage, defenderMaxHP
		// iInterceptingPlayer, iInterceptingUnit, interceptorDamage
		// plotX, plotY		    
		// bool bCanAttack = true;
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{	
			PlayerTypes iAttackingPlayer = NO_PLAYER;
			PlayerTypes iDefendingPlayer = NO_PLAYER;
			PlayerTypes iInterceptingPlayer = NO_PLAYER;

			int plotX = -1;
			int plotY = -1;

			int iAttackingUnit = -1;
			int iDefendingUnit = -1;
			int iInterceptingUnit = -1;

			// Generic values for max hit points, changed below when we have the specific units available
			int attackerMaxHP = GD_INT_GET(MAX_HIT_POINTS);
			if (pAttacker)
				attackerMaxHP = pAttacker->GetMaxHitPoints();
			int defenderMaxHP = GD_INT_GET(MAX_HIT_POINTS);
			if (pDefender)
				defenderMaxHP = pDefender->GetMaxHitPoints();
		
			int attackerDamage = kInfo.getDamageInflicted( BATTLE_UNIT_DEFENDER );
			int defenderDamage = kInfo.getDamageInflicted( BATTLE_UNIT_ATTACKER );
			int interceptorDamage = kInfo.getDamageInflicted( BATTLE_UNIT_INTERCEPTOR );
		
			int attackerFinalDamage = kInfo.getFinalDamage( BATTLE_UNIT_ATTACKER );
			int defenderFinalDamage = kInfo.getFinalDamage( BATTLE_UNIT_DEFENDER );

			CvUnit* pkAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER);
			CvUnit* pkDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER);
			CvUnit* pInterceptor = kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
			CvPlot* pkTargetPlot = kInfo.getPlot();
		
			if (pkTargetPlot)
			{
				plotX = pkTargetPlot->getX();
				plotY = pkTargetPlot->getY();
				CvCity* pCity = pkTargetPlot->getPlotCity();			
				if (pCity)
				{
					iDefendingPlayer = pCity->getOwner();
					defenderMaxHP = pCity->GetMaxHitPoints();
				}
			}
			if (pkAttacker)
			{
				iAttackingPlayer = pAttacker->getOwner();
				iAttackingUnit = pAttacker->GetID();
				attackerMaxHP = pAttacker->GetMaxHitPoints();
				// Generic values for max hit points, changed below when we have the specific units available
			}
			if (pkDefender)
			{
				iDefendingPlayer = pkDefender->getOwner();
				iDefendingUnit = pkDefender->GetID();
				defenderMaxHP = pkDefender->GetMaxHitPoints();
				// Generic values for max hit points, changed below when we have the specific units available
			}
			if (pInterceptor)
			{
				iInterceptingPlayer = pInterceptor->getOwner();
				iInterceptingUnit = pInterceptor->GetID();
			}

			CvLuaArgsHandle args;

			args->Push(iAttackingPlayer);
			args->Push(iAttackingUnit);
			args->Push(attackerDamage);
			args->Push(attackerFinalDamage);
			args->Push(attackerMaxHP);
			args->Push(iDefendingPlayer);
			args->Push(iDefendingUnit);
			args->Push(defenderDamage);
			args->Push(defenderFinalDamage);
			args->Push(defenderMaxHP);
			args->Push(iInterceptingPlayer);
			args->Push(iInterceptingUnit);
			args->Push(interceptorDamage);		
			args->Push(plotX);
			args->Push(plotY);

			bool bResult = false;
		
			if(MOD_EVENTS_RED_COMBAT_ABORT && LuaSupport::CallTestAll(pkScriptSystem, "MustAbortAttack", args.get(), bResult))
			{
				// Check the result.
				if(bResult)
				{
					// Abort the attack
					// to do : this is a hack, fix that in CvTacticalAI.cpp instead.
					bCanAttack = false;
				}
			}

			if (bCanAttack && MOD_EVENTS_RED_COMBAT_RESULT) {
				// If we can attack, send the CombatResult to Lua
				LuaSupport::CallHook(pkScriptSystem, "CombatResult", args.get(), bResult);
			}
		}
	}
	
	if (!MOD_EVENTS_RED_COMBAT_ABORT || bCanAttack)
	{
		// Nuclear Mission
		if(kInfo.getAttackIsNuclear())
		{
			ResolveNuclearCombat(kInfo, uiParentEventID);
		}

		// Bombing Mission
		else if(kInfo.getAttackIsBombingMission())
		{
			ResolveAirUnitVsCombat(kInfo, uiParentEventID);
		}

		// Air Sweep Mission
		else if(kInfo.getAttackIsAirSweep())
		{
			ResolveAirSweep(kInfo, uiParentEventID);
		}

		// Ranged Attack
		else if(kInfo.getAttackIsRanged())
		{
			if(kInfo.getUnit(BATTLE_UNIT_ATTACKER))
			{
				ResolveRangedUnitVsCombat(kInfo, uiParentEventID);
				CvPlot *pPlot = kInfo.getPlot();
				if (kInfo.getUnit(BATTLE_UNIT_ATTACKER) && kInfo.getUnit(BATTLE_UNIT_DEFENDER) && pPlot)
				{
					pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BATTLE_RANGED(), kInfo.getUnit(BATTLE_UNIT_ATTACKER)->getOwner(), kInfo.getUnit(BATTLE_UNIT_DEFENDER)->getOwner());
				}
			}
			else
			{
				ResolveRangedCityVsUnitCombat(kInfo, uiParentEventID);
				CvPlot *pPlot = kInfo.getPlot();
				if (kInfo.getCity(BATTLE_UNIT_ATTACKER) && kInfo.getUnit(BATTLE_UNIT_DEFENDER) && pPlot)
				{
					pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BATTLE_RANGED(), kInfo.getCity(BATTLE_UNIT_ATTACKER)->getOwner(), kInfo.getUnit(BATTLE_UNIT_DEFENDER)->getOwner());
				}
			}
		}

		// Melee Attack
		else
		{
			if(kInfo.getCity(BATTLE_UNIT_DEFENDER))
			{
				ResolveCityMeleeCombat(kInfo, uiParentEventID);
			}
			else
			{
				ResolveMeleeCombat(kInfo, uiParentEventID);
				CvPlot *pPlot = kInfo.getPlot();
				if (kInfo.getUnit(BATTLE_UNIT_ATTACKER) && kInfo.getUnit(BATTLE_UNIT_DEFENDER) && pPlot)
				{
					pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BATTLE_MELEE(), kInfo.getUnit(BATTLE_UNIT_ATTACKER)->getOwner(), kInfo.getUnit(BATTLE_UNIT_DEFENDER)->getOwner());
				}
			}
		}

		if (pAttacker)
		{
			pAttacker->ProcessAttackForPromotionSameAttackBonus();
		}

		if (MOD_EVENTS_RED_COMBAT_ENDED) 
		{
			// RED : CombatEnded
			// iAttackingPlayer, iAttackingUnit, attackerDamage, attackerFinalDamage, attackerMaxHP
			// iDefendingPlayer, iDefendingUnit, defenderDamage, defenderFinalDamage, defenderMaxHP
			// iInterceptingPlayer, iInterceptingUnit, interceptorDamage
			// plotX, plotY		    

			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if(pkScriptSystem)
			{	
				PlayerTypes iAttackingPlayer = NO_PLAYER;
				PlayerTypes iDefendingPlayer = NO_PLAYER;
				PlayerTypes iInterceptingPlayer = NO_PLAYER;

				int plotX = -1;
				int plotY = -1;

				int iAttackingUnit = -1;
				int iDefendingUnit = -1;
				int iInterceptingUnit = -1;

				// Generic values for max hit points, changed below when we have the specific units available
				int attackerMaxHP = GD_INT_GET(MAX_HIT_POINTS);
				if (pAttacker)
					attackerMaxHP = pAttacker->GetMaxHitPoints();
				int defenderMaxHP = GD_INT_GET(MAX_HIT_POINTS);
				if (pDefender)
					defenderMaxHP = pDefender->GetMaxHitPoints();
		
				int attackerDamage = kInfo.getDamageInflicted( BATTLE_UNIT_DEFENDER );
				int defenderDamage = kInfo.getDamageInflicted( BATTLE_UNIT_ATTACKER );
				int interceptorDamage = kInfo.getDamageInflicted( BATTLE_UNIT_INTERCEPTOR );
		
				int attackerFinalDamage = kInfo.getFinalDamage( BATTLE_UNIT_ATTACKER );
				int defenderFinalDamage = kInfo.getFinalDamage( BATTLE_UNIT_DEFENDER );

				CvUnit* pkAttacker = kInfo.getUnit(BATTLE_UNIT_ATTACKER);
				CvUnit* pkDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER);
				CvUnit* pInterceptor = kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
				CvPlot* pkTargetPlot = kInfo.getPlot();
		
				if (pkTargetPlot)
				{
					plotX = pkTargetPlot->getX();
					plotY = pkTargetPlot->getY();
					CvCity* pCity = pkTargetPlot->getPlotCity();			
					if (pCity)
					{
						iDefendingPlayer = pCity->getOwner();
						defenderMaxHP = pCity->GetMaxHitPoints();
					}
				}
				if (pkAttacker)
				{
					iAttackingPlayer = pAttacker->getOwner();
					iAttackingUnit = pAttacker->GetID();
					attackerMaxHP = pAttacker->GetMaxHitPoints();
				}
				if (pkDefender)
				{
					iDefendingPlayer = pkDefender->getOwner();
					iDefendingUnit = pkDefender->GetID();
					defenderMaxHP = pkDefender->GetMaxHitPoints();
				}
				if (pInterceptor)
				{
					iInterceptingPlayer = pInterceptor->getOwner();
					iInterceptingUnit = pInterceptor->GetID();
				}

				CvLuaArgsHandle args;

				args->Push(iAttackingPlayer);
				args->Push(iAttackingUnit);
				args->Push(attackerDamage);
				args->Push(attackerFinalDamage);
				args->Push(attackerMaxHP);
				args->Push(iDefendingPlayer);
				args->Push(iDefendingUnit);
				args->Push(defenderDamage);
				args->Push(defenderFinalDamage);
				args->Push(defenderMaxHP);
				args->Push(iInterceptingPlayer);
				args->Push(iInterceptingUnit);
				args->Push(interceptorDamage);		
				args->Push(plotX);
				args->Push(plotY);

				bool bResult = false;
				LuaSupport::CallHook(pkScriptSystem, "CombatEnded", args.get(), bResult);
			}
		}
	}
	else
	// RED: Attack has been aborted, report that to tactical AI and reset attacker/defender states.
	{	
		if (pAttacker && pAttacker->plot()) //try and make sure this isn't an invalid pointer (unit might have been killed in a conquer/liberate combo)
		{
			pAttacker->setCombatUnit(NULL);
			pAttacker->ClearMissionQueue( false, /*iUnitCycleTimer*/ 110);
		}
		
		if (pDefender && pDefender->plot())
		{
			pDefender->setCombatUnit(NULL);
			pDefender->ClearMissionQueue();
		}

		CvCity* pCity = kInfo.getCity(BATTLE_UNIT_DEFENDER);
		if (pCity)
		{
			pCity->clearCombat();
		}
	}
	// RED >>>>>


	// Clear popup blocking after combat resolves
	if(eAttackingPlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->SetDontShowPopups(false);
	}
}

//	----------------------------------------------------------------------------
CvUnitCombat::ATTACK_RESULT CvUnitCombat::Attack(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption)
{
	CvString strBuffer;

	//VALIDATE_OBJECT();
	ASSERT_DEBUG(kAttacker.canMoveInto(targetPlot, CvUnit::MOVEFLAG_ATTACK ));
	ASSERT_DEBUG(kAttacker.getCombatTimer() == 0);

	CvUnitCombat::ATTACK_RESULT eResult = CvUnitCombat::ATTACK_ABORTED;

	ASSERT_DEBUG(kAttacker.getCombatTimer() == 0);
	//	ASSERT_DEBUG(pDefender != NULL);
	ASSERT_DEBUG(!kAttacker.isFighting());

	CvUnit* pDefender = targetPlot.getBestDefender(NO_PLAYER, kAttacker.getOwner(), &kAttacker, true);
	if(!pDefender)
	{
		return eResult;
	}

	kAttacker.SetAutomateType(NO_AUTOMATE);
	pDefender->SetAutomateType(NO_AUTOMATE);

	if(kAttacker.getOwner() == GC.getGame().getActivePlayer())
	{
		GC.getGame().SetEverAttackedTutorial(true);
	}

	// handle the Zulu special thrown spear first attack
	ATTACK_RESULT eFireSupportResult = ATTACK_ABORTED;
	if (kAttacker.isRangedSupportFire() && pDefender->IsCanDefend())
	{
		eFireSupportResult = AttackRanged(kAttacker, pDefender->getX(), pDefender->getY(), CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT);
		if (pDefender->isDelayedDeath())
		{
			if (!kAttacker.plot()->isFortification(kAttacker.getTeam()))
			{
				// Killed him, move to the plot if we can.
				if (targetPlot.getNumVisibleEnemyDefenders(&kAttacker) == 0)
				{
					if (kAttacker.UnitMove(&targetPlot, true, &kAttacker, true))
						if (!kAttacker.canMoveAfterAttacking())
							kAttacker.finishMoves();
				}
			}

			return eFireSupportResult;
		}
	}

	CvCombatInfo kCombatInfo;
	GenerateMeleeCombatInfo(kAttacker, pDefender, targetPlot, &kCombatInfo);

	ASSERT_DEBUG(!kAttacker.isDelayedDeath() && !pDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");

	if (pDefender->CheckWithdrawal(kAttacker) && pDefender->DoFallBack(kAttacker, true))
	{
		if (kAttacker.getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDREW", pDefender->getNameKey());
			GC.GetEngineUserInterface()->AddMessage(0, kAttacker.getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer);
			if (MOD_WH_MILITARY_LOG)
				MILITARYLOG(kAttacker.getOwner(), strBuffer.c_str(), kAttacker.plot(), pDefender->getOwner());
		}
		else if (pDefender->getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_FRIENDLY_UNIT_WITHDREW", pDefender->getNameKey());
			GC.GetEngineUserInterface()->AddMessage(0, pDefender->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer);
			if (MOD_WH_MILITARY_LOG)
				MILITARYLOG(pDefender->getOwner(), strBuffer.c_str(), pDefender->plot(), kAttacker.getOwner());
		}

		// Move forward if able
		bool bCanAdvance = kCombatInfo.getAttackerAdvances() && targetPlot.getNumVisibleEnemyDefenders(&kAttacker) == 0;
		if (bCanAdvance)
		{
			kAttacker.UnitMove(&targetPlot, true, &kAttacker);
		}
		else
		{
			kAttacker.ClearMissionQueue(false, GetPostCombatDelay());
			if (!CvPreGame::quickMovement())
			{
				// move into tile then back out (at the movement cost of one tile)
				CvPlot* pFromPlot = kAttacker.plot();
				kAttacker.UnitMove(&targetPlot, true, &kAttacker);
				kAttacker.setXY(pFromPlot->getX(), pFromPlot->getY(), true, true, true, true);
				kAttacker.PublishQueuedVisualizationMoves();
			}
			else
			{
				// Reduce moves left without playing animation
				int iMoveCost = targetPlot.movementCost(&kAttacker, kAttacker.plot(), kAttacker.getMoves());
				kAttacker.changeMoves(-iMoveCost);
			}
		}
		//		kAttacker.setMadeAttack(true);   /* EFB: Doesn't work, causes tactical AI to not dequeue this attack; but we've decided you don't lose your attack anyway */
		eResult = ATTACK_COMPLETED;
	}

	else if(!pDefender->IsCanDefend())
	{
		CvMissionDefinition kMission;
		kMission.setMissionTime(kAttacker.getCombatTimer() * gDLL->getSecsPerTurn());
		kMission.setMissionType(CvTypes::getMISSION_SURRENDER());
		kMission.setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
		kMission.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
		kMission.setPlot(&targetPlot);

		// Surrender mission
		CvMissionInfo* pkSurrenderMission = GC.getMissionInfo(CvTypes::getMISSION_SURRENDER());
		if(pkSurrenderMission == NULL)
		{
			ASSERT_DEBUG(false);
		}
		else
		{
			kAttacker.setCombatTimer(pkSurrenderMission->getTime());
		}

		// Kill them!
		pDefender->setDamage(pDefender->GetMaxHitPoints());

		Localization::String strMessage;
		Localization::String strSummary;

		// Some units can't capture civilians. Embarked units are also not captured, they're simply killed. And some aren't a type that gets captured.
		if(!kAttacker.isNoCapture() && (!pDefender->isEmbarked() || pDefender->getUnitInfo().IsCaptureWhileEmbarked()) && pDefender->getCaptureUnitType(pDefender->getOwner()) != NO_UNIT)
		{
			pDefender->setCapturingPlayer(kAttacker.getOwner());

			if(kAttacker.isBarbarian())
			{
				strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS_DETAILED");
				strMessage << pDefender->getUnitInfo().GetTextKey() << GET_PLAYER(kAttacker.getOwner()).getNameKey();
				strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS");
			}
			else
			{
				strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_DETAILED");
				strMessage << pDefender->getUnitInfo().GetTextKey();
				strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED");
			}
		}
		// Unit was killed instead
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_UNIT_LOST");
			strSummary = strMessage;
		}

		CvNotifications* pNotification = GET_PLAYER(pDefender->getOwner()).GetNotifications();
		if(pNotification)
			pNotification->Add(NOTIFICATION_UNIT_DIED, strMessage.toUTF8(), strSummary.toUTF8(), pDefender->getX(), pDefender->getY(), (int) pDefender->getUnitType(), pDefender->getOwner());

		bool bAdvance = kAttacker.canAdvance(targetPlot, ((pDefender->IsCanDefend()) ? 1 : 0));

		// Move forward
		if(targetPlot.getNumVisibleEnemyDefenders(&kAttacker) == 0)
		{
			kAttacker.UnitMove(&targetPlot, true, ((bAdvance) ? &kAttacker : NULL));
		}

		// KWG: Should this be called? The defender is killed above and the unit.
		//      If anything, the above code should be put in the ResolveCombat method.
		ResolveCombat(kCombatInfo);
		eResult = ATTACK_COMPLETED;
	}
	else
	{
		ATTACK_RESULT eSupportResult = ATTACK_ABORTED;
		if(eOption != ATTACK_OPTION_NO_DEFENSIVE_SUPPORT)
		{
			// Ranged fire support from artillery units
			CvUnit* pFireSupportUnit = GetFireSupportUnit(pDefender->getOwner(), pDefender->getX(), pDefender->getY(), kAttacker.getX(), kAttacker.getY());
			if(pFireSupportUnit != NULL)
			{
				ASSERT_DEBUG(!pFireSupportUnit->isDelayedDeath(), "Supporting battle unit is already dead!");
				eSupportResult = AttackRanged(*pFireSupportUnit, kAttacker.getX(), kAttacker.getY(), CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT);
			}

			if(eSupportResult == ATTACK_QUEUED)
			{
				// The supporting unit has queued their attack (against the attacker), we must have the attacker queue its attack.
				// Also, flag the current mission that the next time through, the defender doesn't get any defensive support.
				const_cast<MissionData*>(kAttacker.GetHeadMissionData())->iFlags |= CvUnit::MOVEFLAG_NO_DEFENSIVE_SUPPORT;
				CvUnitMission::WaitFor(&kAttacker, pFireSupportUnit);
				eResult = ATTACK_QUEUED;
			}
		}

		if(eResult != ATTACK_QUEUED)
		{
			kAttacker.setMadeAttack(true);

			uint uiParentEventID = 0;
			// Send the combat message if the target plot is visible.
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible();
			bool quickCombat = CvPreGame::quickCombat();
			if(!quickCombat)
			{
				// Center camera here!
				if(isTargetVisibleToActivePlayer)
				{
					CvInterfacePtr<ICvPlot1> pDefenderPlot = GC.WrapPlotPointer(pDefender->plot());
					GC.GetEngineUserInterface()->lookAt(pDefenderPlot.get(), CAMERALOOKAT_NORMAL);
				}
				kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

				CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
				uiParentEventID = gDLL->GameplayUnitCombat(pDllCombatInfo.get());

				// Set the combat units so that other missions do not continue until combat is over.
				kAttacker.setCombatUnit(pDefender, true);
				pDefender->setCombatUnit(&kAttacker, false);

				eResult = ATTACK_QUEUED;
			}
			else
				eResult = ATTACK_COMPLETED;

			// Resolve combat here.
			ResolveCombat(kCombatInfo, uiParentEventID);

		}
	}

	return eResult;
}

//	---------------------------------------------------------------------------
CvUnitCombat::ATTACK_RESULT CvUnitCombat::AttackRanged(CvUnit& kAttacker, int iX, int iY, CvUnitCombat::ATTACK_OPTION /* eOption */)
{
	//VALIDATE_OBJECT();
	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	ATTACK_RESULT eResult = ATTACK_ABORTED;

	ASSERT_DEBUG(kAttacker.getDomainType() != DOMAIN_AIR, "Air units should not use AttackRanged, they should just MoveTo the target");

	if(NULL == pPlot)
	{
		return eResult;
	}

	if (!kAttacker.isRangedSupportFire())
	{
		if(!kAttacker.canRangeStrikeAt(iX, iY))
		{
			return eResult;
		}
	}

	kAttacker.SetAutomateType(NO_AUTOMATE);

	bool bDoImmediate = CvPreGame::quickCombat();
	// Range-striking a Unit
	if(!pPlot->isCity())
	{
		CvUnit* pDefender = kAttacker.rangeStrikeTarget(*pPlot, true);

#if defined(MOD_EVENTS_UNIT_RANGEATTACK)
		if (!pDefender) {
			if (MOD_EVENTS_UNIT_RANGEATTACK) {
				int iValue = 0;
				if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_UnitRangeAttackAt, kAttacker.getOwner(), kAttacker.GetID(), iX, iY) == GAMEEVENTRETURN_VALUE) {
					if (iValue > 0) {
						return CvUnitCombat::ATTACK_COMPLETED;
					}

					return CvUnitCombat::ATTACK_ABORTED;
				}
			}
		}
#endif

		if(!pDefender) 
			return ATTACK_ABORTED;

		pDefender->SetAutomateType(NO_AUTOMATE);

		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateRangedCombatInfo(kAttacker, pDefender, *pPlot, &kCombatInfo);
		ASSERT_DEBUG(!kAttacker.isDelayedDeath() && !pDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");
		
		if (!kAttacker.isRangedSupportFire())
		{
			kAttacker.setMadeAttack(true);
			kAttacker.changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));
		}

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible();
			if(isTargetVisibleToActivePlayer)
			{
				CvInterfacePtr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayUnitCombat(pDllCombatInfo.get());

			// Set the combat units so that other missions do not continue until combat is over.
			kAttacker.setCombatUnit(pDefender, true);
			pDefender->setCombatUnit(&kAttacker, false);
			eResult = ATTACK_QUEUED;
		}
		else
			eResult = ATTACK_COMPLETED;

		ResolveCombat(kCombatInfo, uiParentEventID);
	}
	// Range-striking a City
	else if (!kAttacker.isRangedSupportFire())
	{
		CvCombatInfo kCombatInfo;
		GenerateRangedCombatInfo(kAttacker, NULL, *pPlot, &kCombatInfo);
		ASSERT_DEBUG(!kAttacker.isDelayedDeath(), "Trying to battle and the attacker is already dead!");
		kAttacker.setMadeAttack(true);
		kAttacker.changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible();
			if(isTargetVisibleToActivePlayer)
			{
				CvInterfacePtr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}

			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayCityCombat(pDllCombatInfo.get());

			CvCity* pkDefender = pPlot->getPlotCity();
			kAttacker.setCombatCity(pkDefender);
			if(pkDefender)
				pkDefender->setCombatUnit(&kAttacker);
			eResult = ATTACK_QUEUED;
		}
		else
			eResult = ATTACK_COMPLETED;

		ResolveCombat(kCombatInfo, uiParentEventID);
	}

	if(!kAttacker.canMoveAfterAttacking() && !kAttacker.isRangedSupportFire() && kAttacker.isOutOfAttacks())
	{
		kAttacker.finishMoves();
		GC.GetEngineUserInterface()->changeCycleSelectionCounter(1);
	}

	return eResult;
}

//	----------------------------------------------------------------------------
CvUnitCombat::ATTACK_RESULT CvUnitCombat::AttackAir(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION /* eOption */)
{
	//VALIDATE_OBJECT();
	ASSERT_DEBUG(kAttacker.getCombatTimer() == 0);

	CvUnitCombat::ATTACK_RESULT eResult = CvUnitCombat::ATTACK_ABORTED;

	// Can we actually hit the target?
	if(!kAttacker.canRangeStrikeAt(targetPlot.getX(), targetPlot.getY()))
	{
		return eResult;
	}

	bool bDoImmediate = CvPreGame::quickCombat();
	kAttacker.SetAutomateType(NO_AUTOMATE);

	// Bombing a Unit
	if(!targetPlot.isCity() || kAttacker.AI_getUnitAIType()==UNITAI_MISSILE_AIR)
	{
		CvUnit* pDefender = kAttacker.rangeStrikeTarget(targetPlot, true);

#if defined(MOD_EVENTS_UNIT_RANGEATTACK)
		if (!pDefender) {
			if (MOD_EVENTS_UNIT_RANGEATTACK) {
				int iValue = 0;
				if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_UnitRangeAttackAt, kAttacker.getOwner(), kAttacker.GetID(), targetPlot.getX(), targetPlot.getY()) == GAMEEVENTRETURN_VALUE) {
					if (iValue > 0) {
						return CvUnitCombat::ATTACK_COMPLETED;
					}

					return CvUnitCombat::ATTACK_ABORTED;
				}
			}
		}
#endif

		if(!pDefender) 
			return CvUnitCombat::ATTACK_ABORTED;

		pDefender->SetAutomateType(NO_AUTOMATE);

		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateAirCombatInfo(kAttacker, pDefender, targetPlot, &kCombatInfo);
		ASSERT_DEBUG(!kAttacker.isDelayedDeath() && !pDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");
		kAttacker.setMadeAttack(true);

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible();
			if(isTargetVisibleToActivePlayer)
			{
				CvInterfacePtr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&targetPlot);
				GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayUnitCombat(pDllCombatInfo.get());

			// Set the combat units so that other missions do not continue until combat is over.
			kAttacker.setCombatUnit(pDefender, true);
			pDefender->setCombatUnit(&kAttacker, false);
			CvUnit* pDefenderSupport = kCombatInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
			if(pDefenderSupport)
				pDefenderSupport->setCombatUnit(&kAttacker, false);

			eResult = ATTACK_QUEUED;
		}
		else
			eResult = ATTACK_COMPLETED;

		ResolveCombat(kCombatInfo, uiParentEventID);
	}
	// Bombing a City
	else
	{
		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateAirCombatInfo(kAttacker, NULL, targetPlot, &kCombatInfo);
		ASSERT_DEBUG(!kAttacker.isDelayedDeath(), "Trying to battle and the attacker is already dead!");
		kAttacker.setMadeAttack(true);

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible();
			if(isTargetVisibleToActivePlayer)
			{
				CvInterfacePtr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&targetPlot);
				GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
			}

			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);
			CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayCityCombat(pDllCombatInfo.get());

			CvCity* pkDefender = targetPlot.getPlotCity();
			kAttacker.setCombatCity(pkDefender);
			if(pkDefender)
				pkDefender->setCombatUnit(&kAttacker);
			CvUnit* pDefenderSupport = kCombatInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
			if(pDefenderSupport)
				pDefenderSupport->setCombatUnit(&kAttacker, false);
			eResult = ATTACK_QUEUED;
		}
		else
			eResult = ATTACK_COMPLETED;

		ResolveCombat(kCombatInfo, uiParentEventID);
	}

	return eResult;
}

//	----------------------------------------------------------------------------
CvUnitCombat::ATTACK_RESULT CvUnitCombat::AttackAirSweep(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION /* eOption */)
{
	//VALIDATE_OBJECT();
	ASSERT_DEBUG(kAttacker.getCombatTimer() == 0);

	CvUnitCombat::ATTACK_RESULT eResult = CvUnitCombat::ATTACK_ABORTED;

	// Can we actually hit the target?
	if(!kAttacker.canAirSweepAt(targetPlot.getX(), targetPlot.getY()))
	{
		return eResult;
	}

	CvUnit* pInterceptor = targetPlot.GetBestInterceptor(kAttacker.getOwner(), &kAttacker);
	kAttacker.SetAutomateType(NO_AUTOMATE);

	// Any interceptor to sweep for?
	if(pInterceptor != NULL)
	{
		// CUSTOMLOG("AttackAirSweep: At (%i, %i) Attacker: %i-%i Interceptor: %i-%i", targetPlot.getX(), targetPlot.getY(), kAttacker.getOwner(), kAttacker.GetID(), pInterceptor->getOwner(), pInterceptor->GetID());
		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateAirSweepCombatInfo(kAttacker, pInterceptor, targetPlot, &kCombatInfo);
		kAttacker.setMadeAttack(true);

		CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
		pkDefender->SetAutomateType(NO_AUTOMATE);
		ASSERT_DEBUG(!kAttacker.isDelayedDeath() && !pkDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");

		uint uiParentEventID = 0;
		bool bDoImmediate = CvPreGame::quickCombat();
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible();
			if(isTargetVisibleToActivePlayer)
			{
				CvInterfacePtr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&targetPlot);
				GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayUnitCombat(pDllCombatInfo.get());

			// Set the combat units so that other missions do not continue until combat is over.
			kAttacker.setCombatUnit(pInterceptor, true);
			pInterceptor->setCombatUnit(&kAttacker, false);
			eResult = ATTACK_QUEUED;
		}
		else
			eResult = ATTACK_COMPLETED;

		ResolveCombat(kCombatInfo, uiParentEventID);
	}
	else
	{
		bool bFallbackAttack = false;
		if (MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			bFallbackAttack = kAttacker.attemptGroundAttacks(targetPlot);

		if (bFallbackAttack)
		{
			int iExperience = /*5*/ GD_INT_GET(EXPERIENCE_ATTACKING_AIR_SWEEP);
			PlayerTypes eUnitOwner = kAttacker.getOwner();
			PlayerTypes ePlotOwner = targetPlot.getOwner();
			kAttacker.changeExperienceTimes100(100 * iExperience, -1, true, eUnitOwner == ePlotOwner, true, eUnitOwner != ePlotOwner && GET_PLAYER(ePlotOwner).isHuman());
			kAttacker.testPromotionReady();

			// attempted to do a sweep in a plot that had no interceptors
			// consume the movement and finish its moves
			if (kAttacker.getOwner() == GC.getGame().getActivePlayer())
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_AIR_PATROL_BOMBED_GROUND_TARGETS");
				localizedText << kAttacker.getUnitInfo().GetTextKey();
				GC.GetEngineUserInterface()->AddMessage(0, kAttacker.getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), localizedText.toUTF8());
			}
		}
		else
		{
			if (kAttacker.getOwner() == GC.getGame().getActivePlayer())
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_AIR_PATROL_FOUND_NOTHING");
				localizedText << kAttacker.getUnitInfo().GetTextKey();
				GC.GetEngineUserInterface()->AddMessage(0, kAttacker.getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), localizedText.toUTF8());
			}
		}

		// Spend a move for this attack
		kAttacker.changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));

		// Can't move or attack again (no attack was spent but anyway)
		if(!kAttacker.canMoveAfterAttacking())
		{
			kAttacker.finishMoves();
		}
	}

	return eResult;
}

//	---------------------------------------------------------------------------
CvUnitCombat::ATTACK_RESULT CvUnitCombat::AttackCity(CvUnit& kAttacker, CvPlot& plot, CvUnitCombat::ATTACK_OPTION eOption)
{
	//VALIDATE_OBJECT();

	ATTACK_RESULT eResult = ATTACK_ABORTED;
	CvCity* pCity = plot.getPlotCity();
	ASSERT_DEBUG(pCity != NULL, "If this unit is attacking a NULL city then something funky is goin' down");
	if(!pCity) return eResult;

	kAttacker.SetAutomateType(NO_AUTOMATE);

	if(eOption != ATTACK_OPTION_NO_DEFENSIVE_SUPPORT)
	{
		// See if the city has some supporting fire to fend off the attacker
		CvUnit* pFireSupportUnit = GetFireSupportUnit(pCity->getOwner(), pCity->getX(), pCity->getY(), kAttacker.getX(), kAttacker.getY());

		ATTACK_RESULT eSupportResult = ATTACK_ABORTED;
		if(pFireSupportUnit)
			eSupportResult = AttackRanged(*pFireSupportUnit, kAttacker.getX(), kAttacker.getY(), CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT);

		if(eSupportResult == ATTACK_QUEUED)
		{
			// The supporting unit has queued their attack (against the attacker), we must have the attacker queue its attack.
			// Also, flag the current mission that the next time through, the defender doesn't get any defensive support.
			const_cast<MissionData*>(kAttacker.GetHeadMissionData())->iFlags |= CvUnit::MOVEFLAG_NO_DEFENSIVE_SUPPORT;
			CvUnitMission::WaitFor(&kAttacker, pFireSupportUnit);
			eResult = ATTACK_QUEUED;
		}
	}

	if(eResult != ATTACK_QUEUED)
	{
		// We are doing a non-ranged attack on a city
		CvCombatInfo kCombatInfo;
		GenerateMeleeCombatInfo(kAttacker, NULL, plot, &kCombatInfo);
		ASSERT_DEBUG(!kAttacker.isDelayedDeath(), "Trying to battle and the attacker is already dead!");
		kAttacker.setMadeAttack(true);

		// Send the combat message if the target plot is visible.
		bool isTargetVisibleToActivePlayer = plot.isActiveVisible();

		uint uiParentEventID = 0;
		bool bDoImmediate = CvPreGame::quickCombat();
		if(!bDoImmediate)
		{
			// Center camera here!
			if(isTargetVisibleToActivePlayer)
			{
				CvInterfacePtr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(&plot);
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
			uiParentEventID = gDLL->GameplayCityCombat(pDllCombatInfo.get());

			CvCity* pkDefender = plot.getPlotCity();
			kAttacker.setCombatCity(pkDefender);
			if(pkDefender)
				pkDefender->setCombatUnit(&kAttacker);
			eResult = ATTACK_QUEUED;
		}
		else
			eResult = ATTACK_COMPLETED;

		ResolveCombat(kCombatInfo, uiParentEventID);
	}
	return eResult;
}

//	-------------------------------------------------------------------------------------------
//	Return a ranged unit that will defend the supplied location against the attacker at the specified location.
CvUnit* CvUnitCombat::GetFireSupportUnit(PlayerTypes eDefender, int iDefendX, int iDefendY, int iAttackX, int iAttackY)
{
	if (/*1*/ GD_INT_GET(FIRE_SUPPORT_DISABLED) == 1)
		return NULL;

	CvPlot* pAdjacentPlot = NULL;
	CvPlot* pPlot = GC.getMap().plot(iDefendX, iDefendY);

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			for(int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
			{
				CvUnit* pLoopUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);

				// Unit owned by same player?
				if(pLoopUnit->getOwner() == eDefender)
				{
					// Can this unit perform a ranged strike on the attacker's plot?
					if(pLoopUnit->canRangeStrikeAt(iAttackX, iAttackY))
					{
						// Range strike would be calculated here, so get the estimated damage
						return pLoopUnit;
					}
				}
			}
		}
	}

	return NULL;
}

//	----------------------------------------------------------------------------
CvUnitCombat::ATTACK_RESULT CvUnitCombat::AttackNuclear(CvUnit& kAttacker, int iX, int iY, ATTACK_OPTION /* eOption */)
{
	ATTACK_RESULT eResult = ATTACK_ABORTED;

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(NULL == pPlot)
		return eResult;

	bool bDoImmediate = CvPreGame::quickCombat();
	CvCombatInfo kCombatInfo;
	CvUnitCombat::GenerateNuclearCombatInfo(kAttacker, *pPlot, &kCombatInfo);
	if (kAttacker.isDelayedDeath())
	{
		kCombatInfo.setAttackIsBombingMission(true);
		kCombatInfo.setAttackNuclearLevel(0);
		kCombatInfo.setDefenderRetaliates(true);
	}
	kAttacker.setMadeAttack(true);
	uint uiParentEventID = 0;
	if(!bDoImmediate)
	{
		// Nuclear attacks are different in that you can target a plot you can't see, so check to see if the active player
		// is involved in the combat
		TeamTypes eActiveTeam = GC.getGame().getActiveTeam();

		bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible();
		if(!isTargetVisibleToActivePlayer)
		{
			// Is the attacker part of the local team?
			isTargetVisibleToActivePlayer = (kAttacker.getTeam() != NO_TEAM && eActiveTeam == kAttacker.getTeam());

			if(!isTargetVisibleToActivePlayer)
			{
				// Are any of the teams effected by the blast in the local team?
				for(int i = 0; i < MAX_TEAMS && !isTargetVisibleToActivePlayer; ++i)
				{
					if(kAttacker.isNukeVictim(pPlot, ((TeamTypes)i)))
					{
						isTargetVisibleToActivePlayer = eActiveTeam == ((TeamTypes)i);
					}
				}
			}
		}

		if(isTargetVisibleToActivePlayer)
		{
			CvInterfacePtr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
			GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
		}
		kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

		// Set a combat unit/city.  Not really needed for the combat since we are killing everyone, but it is currently the only way a unit is marked that it is 'in-combat'
		if(pPlot->getPlotCity())
			kAttacker.setCombatCity(pPlot->getPlotCity());
		else
		{
			if(pPlot->getNumUnits())
				kAttacker.setCombatUnit(pPlot->getUnitByIndex(0), true);
			else
				kAttacker.setAttackPlot(pPlot, false);
		}

		CvInterfacePtr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
		uiParentEventID = gDLL->GameplayUnitCombat(pDllCombatInfo.get());

		eResult = ATTACK_QUEUED;
	}
	else
	{
		eResult = ATTACK_COMPLETED;
		// Set the plot, just so the unit is marked as 'in-combat'
		kAttacker.setAttackPlot(pPlot, false);
	}

	if (kAttacker.isDelayedDeath())
	{
		GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNumTimesNuked(kAttacker.getOwner(), 1);
	}

	ResolveCombat(kCombatInfo,  uiParentEventID);

	return eResult;
}

//	---------------------------------------------------------------------------
// winner has killed loser
//	---------------------------------------------------------------------------
void CvUnitCombat::ApplyPostKillTraitEffects(CvUnit* pkWinner, CvUnit* pkLoser)
{
	// "Heal if defeat enemy" promotion; doesn't apply if defeat a barbarian
	if(pkWinner->getHPHealedIfDefeatEnemy() > 0 && (pkLoser->getOwner() != BARBARIAN_PLAYER || !(pkWinner->IsHealIfDefeatExcludeBarbarians())) && !pkWinner->IsCannotHeal())
	{
		if(pkWinner->getHPHealedIfDefeatEnemy() > pkWinner->getDamage())
		{
			pkWinner->changeDamage(-pkWinner->getDamage());
		}
		else
		{
			pkWinner->changeDamage(-pkWinner->getHPHealedIfDefeatEnemy());
		}
	}
#if defined(MOD_BALANCE_CORE)
	if(pkWinner->isExtraAttackHealthOnKill())
	{
		int iHealAmount = min(pkWinner->getDamage(), /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT));
		pkWinner->changeMoves(GD_INT_GET(MOVE_DENOMINATOR));
		pkWinner->setMadeAttack(false);
		if (!pkWinner->IsCannotHeal())
		{
			pkWinner->changeDamage(-iHealAmount);
		}
	}
#endif
	// If the modder wants the healing to be negative (ie additional damage), then let it be
	else if(pkWinner->getHPHealedIfDefeatEnemy() < 0 && (pkLoser->getOwner() != BARBARIAN_PLAYER || !(pkWinner->IsHealIfDefeatExcludeBarbarians()) || !(pkWinner->isExtraAttackHealthOnKill())))
	{
		if(pkWinner->getHPHealedIfDefeatEnemy() <= (pkWinner->getDamage() - pkWinner->GetMaxHitPoints()))
		{
			// The graphics engine expects the unit to be alive here, so do NOT inflict enough damage to kill it!
			pkWinner->setDamage(pkWinner->GetMaxHitPoints()-1);
		}
		else
		{
			pkWinner->changeDamage(-pkWinner->getHPHealedIfDefeatEnemy());
		}
	}

	CvPlayer& kPlayer = GET_PLAYER(pkWinner->getOwner());

	// Earn bonuses for kills?
	kPlayer.DoYieldsFromKill(pkWinner, pkLoser);
#if defined(MOD_BALANCE_CORE)
	if(pkLoser->getOwner() == BARBARIAN_PLAYER && pkLoser->plot()->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
	{
		GET_PLAYER(pkWinner->getOwner()).GetPlayerTraits()->SetDefeatedBarbarianCampGuardType(pkLoser->getUnitType());
	}
#endif

	//Achievements and Stats
	if (MOD_API_ACHIEVEMENTS && pkWinner->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		CvString szUnitType;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pkWinner->getUnitType());
		if(pkUnitInfo)
			szUnitType = pkUnitInfo->GetType();

		//Elizabeth Special Achievement
		if((CvString)kPlayer.getLeaderTypeKey() == "LEADER_ELIZABETH" && pkLoser->getDomainType() == DOMAIN_SEA)
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BRITISHNAVY, 357, ACHIEVEMENT_SPECIAL_ARMADA);
		}
		//Ramkang's Special Achievement
		if(szUnitType == "UNIT_SIAMESE_WARELEPHANT")
		{
			//CvString szUnitTypeLoser = (CvString) GC.getUnitInfo(pkLoser->getUnitType())->GetType();
		}

		//Oda's Special Achievement
		if((CvString)kPlayer.getLeaderTypeKey() == "LEADER_ODA_NOBUNAGA" && (pkWinner->GetMaxHitPoints() - pkWinner->getDamage() == 1))
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_KAMIKAZE);
		}
		//Napoleon's Special Achievement
		if(szUnitType == "UNIT_FRENCH_MUSKETEER")
		{
			if(pkLoser->GetNumOwningPlayerUnitsAdjacent(pkLoser, pkWinner) >=3)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_MUSKETEERS);
			}
		}

		//DLC_05 Sejong's Turtle Boat Achievement
		if(szUnitType == "UNIT_KOREAN_TURTLE_SHIP")
		{
			CvString szLoserUnitType;
			CvUnitEntry* pkLoserUnitInfo = GC.getUnitInfo(pkLoser->getUnitType());
			if(pkLoserUnitInfo)
			{
				szLoserUnitType = pkLoserUnitInfo->GetType();
			}
			if(szLoserUnitType == "UNIT_IRONCLAD")
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_IRONCLAD_TURTLE);
			}
		}

		//DLC_05 Sejong's Hwacha Achievement
		if(szUnitType == "UNIT_KOREAN_HWACHA")
		{
			gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_HWACHAKILLS, 99, ACHIEVEMENT_SPECIAL_HWATCH_OUT);
		}

	}
}

void CvUnitCombat::ApplyPostCityCombatEffects(CvUnit* pkAttacker, CvCity* pkDefender, int iAttackerDamageInflicted)
{
	int iPlunderModifier = pkAttacker->GetCityAttackPlunderModifier();
	if (iPlunderModifier > 0)
	{
		int iGoldPlundered = iAttackerDamageInflicted * iPlunderModifier;
		iGoldPlundered /= 100;

		if (iGoldPlundered > 0)
		{
			iGoldPlundered *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
			iGoldPlundered /= 100;

			iGoldPlundered = min(iGoldPlundered, (pkDefender->getStrengthValue() / 100) * 10);

			GET_PLAYER(pkAttacker->getOwner()).GetTreasury()->ChangeGold(iGoldPlundered);

			CvPlayer& kCityPlayer = GET_PLAYER(pkDefender->getOwner());
			int iDeduction = min(iGoldPlundered, kCityPlayer.GetTreasury()->GetGold());
			kCityPlayer.GetTreasury()->ChangeGold(-iDeduction);

			if (pkAttacker->getOwner() == GC.getGame().getActivePlayer())
			{
				char text[256] = {0};
				CvString colorString = "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]";
				sprintf_s(text, colorString, iGoldPlundered);
				SHOW_PLOT_POPUP(pkAttacker->plot(), pkAttacker->getOwner(), text);
			}
		}
	}
	pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

	// Instant yield when dealing damage to city
	// Only pass in the attacker, its city, and the damage
	CvCity* pOriginCity = pkAttacker->getOriginCity();
	CvPlayer& kUnitOwner = GET_PLAYER(pkAttacker->getOwner());
	if (!pOriginCity)
		pOriginCity = kUnitOwner.getCapitalCity();
	kUnitOwner.doInstantYield(INSTANT_YIELD_TYPE_CITY_DAMAGE, false, NO_GREATPERSON, NO_BUILDING, iAttackerDamageInflicted, true, NO_PLAYER, NULL, false, pOriginCity, false, true, false, NO_YIELD, pkAttacker);

	// Update war value
	if (!pkDefender->isBarbarian() && !pkAttacker->isBarbarian())
	{
		int iCityValue = pkDefender->GetWarValue();

		int iDamagePermyriad = pkDefender->GetDamagePermyriad(pkAttacker->getOwner());
		int iNewPermyriad = iAttackerDamageInflicted * 10000 / pkDefender->GetMaxHitPoints();

		// Cap damage at 10000 permyriad
		if (iDamagePermyriad + iNewPermyriad > 10000)
		{
			iNewPermyriad = 10000 - iDamagePermyriad;
		}

		iCityValue *= iNewPermyriad;
		iCityValue /= 10000;

		// Dramatically reduce the value if attacker has owned the city before
		int iNumTimesOwned = pkDefender->GetNumTimesOwned(pkAttacker->getOwner());
		if (iNumTimesOwned > 0)
		{
			iCityValue /= iNumTimesOwned * 3;
		}

		pkDefender->ChangeDamagePermyriad(pkAttacker->getOwner(), iNewPermyriad);
		kUnitOwner.ApplyWarDamage(pkDefender->getOwner(), iCityValue);
	}
}

void CvUnitCombat::ApplyExtraUnitDamage(CvUnit* pkAttacker, const CvCombatInfo & kCombatInfo, uint uiParentEventID)
{
	for(int i = 0; i < kCombatInfo.getDamageMemberCount(); ++i)
	{
		const CvCombatMemberEntry& kEntry = kCombatInfo.getDamageMembers()[i];
		if(kEntry.IsUnit())
		{
			CvUnit* pkUnit = GET_PLAYER(kEntry.GetPlayer()).getUnit(kEntry.GetUnitID());
			if(pkUnit)
			{
				pkUnit->changeDamage(kEntry.GetDamage(), pkAttacker->getOwner());
#if defined(MOD_CORE_PER_TURN_DAMAGE)
				pkUnit->addDamageReceivedThisTurn(kEntry.GetDamage(), pkAttacker);
#endif

				if (pkUnit->IsDead())
				{
					//unit kill is delayed. in case of multiple attacks this turn we remove / switch the garrison manually
					if (pkUnit->IsGarrisoned())
					{
						CvCity* pCity = pkUnit->GetGarrisonedCity();
						pCity->SetGarrison( pCity->plot()->getBestGarrison(pCity->getOwner()) );
					}

					CvString strBuffer;
					int iActivePlayerID = GC.getGame().getActivePlayer();

					CvInterfacePtr<ICvUnit1> pDefender = GC.WrapUnitPointer(pkUnit);
					gDLL->GameplayUnitDestroyedInCombat(pDefender.get());

					if(iActivePlayerID == pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), kEntry.GetDamage(), pkUnit->m_strName.IsEmpty() ? pkUnit->getNameKey() : (const char*) (pkUnit->getName()).c_str());
						GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(pkAttacker->getOwner(), strBuffer.c_str(), pkAttacker->plot(), pkUnit->getOwner());
					}

					if(pkAttacker->getVisualOwner(pkUnit->getTeam()) != pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkUnit->m_strName.IsEmpty() ? pkUnit->getNameKey() : (const char*) (pkUnit->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), kEntry.GetDamage());
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkUnit->m_strName.IsEmpty() ? pkUnit->getNameKey() : (const char*) (pkUnit->getName()).c_str(), pkAttacker->m_strName.IsEmpty() ? pkAttacker->getNameKey() : (const char*) (pkAttacker->getName()).c_str(), pkAttacker->getVisualCivAdjective(pkUnit->getTeam()), kEntry.GetDamage());
					}
					if(iActivePlayerID == pkUnit->getOwner())
					{
						GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkUnit->getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					}
					CvNotifications* pNotification = GET_PLAYER(pkUnit->getOwner()).GetNotifications();
					if(pNotification)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkUnit->getX(), pkUnit->getY(), (int) pkUnit->getUnitType(), pkUnit->getOwner());
					}

					pkAttacker->testPromotionReady();

					CvUnitCombat::ApplyPostKillTraitEffects(pkAttacker, pkUnit);
				}
			}
		}
	}
}
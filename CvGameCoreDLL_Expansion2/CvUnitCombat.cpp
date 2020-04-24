/*	-------------------------------------------------------------------------------------------------------
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
#if defined(MOD_UNITS_MAX_HP)
	int iAttackerMaxHP = kAttacker.GetMaxHitPoints();
#else
	int iMaxHP = GC.getMAX_HIT_POINTS();
#endif

	pkCombatInfo->setUnit(BATTLE_UNIT_ATTACKER, &kAttacker);
	pkCombatInfo->setUnit(BATTLE_UNIT_DEFENDER, pkDefender);
	pkCombatInfo->setPlot(&plot);

	// Attacking a City
	if(plot.isCity())
	{
		// Unit vs. City (non-ranged so the city will retaliate)
		CvCity* pkCity = plot.getPlotCity();
		BATTLE_JOINED(pkCity, BATTLE_UNIT_DEFENDER, true);
		int iMaxCityHP = pkCity->GetMaxHitPoints();

		int iAttackerStrength = kAttacker.GetMaxAttackStrength(kAttacker.plot(), &plot, NULL);
		int iDefenderStrength = pkCity->getStrengthValue();

		bool bIncludeRand = !GC.getGame().isGameMultiPlayer();
		int iAttackerDamageInflicted = kAttacker.getCombatDamage(iAttackerStrength, iDefenderStrength, /*bIncludeRand*/ bIncludeRand, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ true);
		int iDefenderDamageInflicted = kAttacker.getCombatDamage(iDefenderStrength, iAttackerStrength, /*bIncludeRand*/ bIncludeRand, /*bAttackerIsCity*/ true, /*bDefenderIsCity*/ false);

#if defined(MOD_BALANCE_CORE)
		if(kAttacker.getForcedDamageValue() != 0)
		{
			iDefenderDamageInflicted = kAttacker.getForcedDamageValue();
		}
		if(kAttacker.getChangeDamageValue() != 0)
		{
			iDefenderDamageInflicted += kAttacker.getChangeDamageValue();
			if (iDefenderDamageInflicted <= 0)
				iDefenderDamageInflicted = 0;
		}
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
		//if there is a garrison, the unit absorbs part of the damage!
		CvUnit* pGarrison = pkCity->GetGarrisonedUnit();
		if(pGarrison)
		{
			//make sure there are no rounding errors
			int iGarrisonShare = (iAttackerDamageInflicted*2*pGarrison->GetMaxHitPoints()) / (pkCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());
			
			/*
			//garrison can not be killed, only reduce to 10 hp
			iGarrisonShare = min(iGarrisonShare,pGarrison->GetCurrHitPoints()-10);
			*/

			if (iGarrisonShare>0)
			{
				iAttackerDamageInflicted -= iGarrisonShare;

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
		}
#endif

		int iAttackerTotalDamageInflicted = iAttackerDamageInflicted + pkCity->getDamage();
		int iDefenderTotalDamageInflicted = iDefenderDamageInflicted + kAttacker.getDamage();

		// Will both the attacker die, and the city fall? If so, the unit wins
#if defined(MOD_UNITS_MAX_HP)
		if (iAttackerTotalDamageInflicted >= iMaxCityHP && iDefenderTotalDamageInflicted >= iAttackerMaxHP)
		{
			iDefenderDamageInflicted = iAttackerMaxHP - kAttacker.getDamage() - 1;
			iDefenderTotalDamageInflicted = iAttackerMaxHP - 1;
		}
#else
		if (iAttackerTotalDamageInflicted >= iMaxCityHP && iDefenderTotalDamageInflicted >= iMaxHP)
		{
			iDefenderDamageInflicted = iMaxHP - kAttacker.getDamage() - 1;
			iDefenderTotalDamageInflicted = iMaxHP - 1;
		}
#endif

		pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderTotalDamageInflicted);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);
		pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerTotalDamageInflicted);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);

		int iExperience = /*5*/ GC.getEXPERIENCE_ATTACKING_CITY_MELEE();
		pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
		pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, MAX_INT);
		pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == pkCity->getOwner());
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
		if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
		{
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
		}
		else
		{
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !kAttacker.isBarbarian());
#else
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !pkCity->isBarbarian()); // Kind of irrelevant, as cities don't get XP!
#else
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !pkCity->isBarbarian());
#endif
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
#if defined(MOD_UNITS_MAX_HP)
		int iDefenderMaxHP = pkDefender->GetMaxHitPoints();
#endif

		int iDefenderStrength = pkDefender->GetMaxDefenseStrength(&plot, &kAttacker, kAttacker.plot());
		int iAttackerStrength = 0;
		if(kAttacker.GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true) > 0 && kAttacker.getDomainType() == DOMAIN_AIR)
		{
			iAttackerStrength = kAttacker.GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true);
			if(pkDefender->getDomainType() != DOMAIN_AIR)
			{
				iDefenderStrength /= 2;
			}
		}
		else
		{
			iAttackerStrength = kAttacker.GetMaxAttackStrength(kAttacker.plot(), &plot, pkDefender);
		}

		if (kAttacker.IsCanHeavyCharge() && !pkDefender->CanFallBack(kAttacker,false))
		{
			iAttackerStrength = (iAttackerStrength * 150) / 100;
		}

		bool bIncludeRand = !GC.getGame().isGameMultiPlayer();

		int iAttackerDamageInflicted = kAttacker.getCombatDamage(iAttackerStrength, iDefenderStrength, /*bIncludeRand*/ bIncludeRand, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
		int iDefenderDamageInflicted = pkDefender->getCombatDamage(iDefenderStrength, iAttackerStrength, /*bIncludeRand*/ bIncludeRand, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);

		if(kAttacker.getDomainType() == DOMAIN_AIR && pkDefender->getDomainType() != DOMAIN_AIR)
		{
			iAttackerDamageInflicted /= 2;
			iDefenderDamageInflicted /= 3;
		}

#if defined(MOD_BALANCE_CORE)
		if(kAttacker.getForcedDamageValue() != 0)
		{
			iDefenderDamageInflicted   = kAttacker.getForcedDamageValue();
		}
		if(pkDefender->getForcedDamageValue() != 0)
		{
			iAttackerDamageInflicted= pkDefender->getForcedDamageValue();
		}
		if(kAttacker.getChangeDamageValue() != 0)
		{
			iDefenderDamageInflicted += kAttacker.getChangeDamageValue();
			if (iDefenderDamageInflicted <= 0)
				iDefenderDamageInflicted = 0;
		}
		if(pkDefender->getChangeDamageValue() != 0)
		{
			iAttackerDamageInflicted += pkDefender->getChangeDamageValue();
			if (iAttackerDamageInflicted <= 0)
				iAttackerDamageInflicted = 0;
		}
		//Chance to spread promotion?
		if(kAttacker.getPlagueChance() > 0)
		{
			kAttacker.DoPlagueTransfer(*pkDefender);
		}
		if (pkDefender->getPlagueChance() > 0 && !pkDefender->isRanged())
		{
			pkDefender->DoPlagueTransfer(kAttacker);
		}
#endif
		int iAttackerTotalDamageInflicted = iAttackerDamageInflicted + pkDefender->getDamage();
		int iDefenderTotalDamageInflicted = iDefenderDamageInflicted + kAttacker.getDamage();

		// Will both units be killed by this? :o If so, take drastic corrective measures
#if defined(MOD_UNITS_MAX_HP)
		if (iAttackerTotalDamageInflicted >= iDefenderMaxHP && iDefenderTotalDamageInflicted >= iAttackerMaxHP)
#else
		if (iAttackerTotalDamageInflicted >= iMaxHP && iDefenderTotalDamageInflicted >= iMaxHP)
#endif
		{
			// He who hath the least amount of damage survives with 1 HP left
			if(iAttackerTotalDamageInflicted > iDefenderTotalDamageInflicted)
			{
#if defined(MOD_UNITS_MAX_HP)
				iDefenderDamageInflicted = iAttackerMaxHP - kAttacker.getDamage() - 1;
				iDefenderTotalDamageInflicted = iAttackerMaxHP - 1;
				iAttackerTotalDamageInflicted = iDefenderMaxHP;
#else
				iDefenderDamageInflicted = iMaxHP - kAttacker.getDamage() - 1;
				iDefenderTotalDamageInflicted = iMaxHP - 1;
				iAttackerTotalDamageInflicted = iMaxHP;
#endif
			}
			else
			{
#if defined(MOD_UNITS_MAX_HP)
				iAttackerDamageInflicted = iDefenderMaxHP - pkDefender->getDamage() - 1;
				iAttackerTotalDamageInflicted = iDefenderMaxHP - 1;
				iDefenderTotalDamageInflicted = iAttackerMaxHP;
#else
				iAttackerDamageInflicted = iMaxHP - pkDefender->getDamage() - 1;
				iAttackerTotalDamageInflicted = iMaxHP - 1;
				iDefenderTotalDamageInflicted = iMaxHP;
#endif
			}
		}

		pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderTotalDamageInflicted);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);
		pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerTotalDamageInflicted);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);

		// Fear Damage
		pkCombatInfo->setFearDamageInflicted(BATTLE_UNIT_ATTACKER, kAttacker.getCombatDamage(iAttackerStrength, iDefenderStrength, bIncludeRand, false, true));

#if defined(MOD_UNITS_MAX_HP)
		int iAttackerEffectiveStrength = iAttackerStrength * (iAttackerMaxHP - range(kAttacker.getDamage(), 0, iAttackerMaxHP - 1)) / iAttackerMaxHP;
#else
		int iAttackerEffectiveStrength = iAttackerStrength * (iMaxHP - range(kAttacker.getDamage(), 0, iMaxHP - 1)) / iMaxHP;
#endif
		iAttackerEffectiveStrength = iAttackerEffectiveStrength > 0 ? iAttackerEffectiveStrength : 1;
#if defined(MOD_UNITS_MAX_HP)
		int iDefenderEffectiveStrength = iDefenderStrength * (iDefenderMaxHP - range(pkDefender->getDamage(), 0, iDefenderMaxHP - 1)) / iDefenderMaxHP;
#else
		int iDefenderEffectiveStrength = iDefenderStrength * (iMaxHP - range(pkDefender->getDamage(), 0, iMaxHP - 1)) / iMaxHP;
#endif
		iDefenderEffectiveStrength = iDefenderEffectiveStrength > 0 ? iDefenderEffectiveStrength : 1;

		//int iExperience = kAttacker.defenseXPValue();
		//iExperience = ((iExperience * iAttackerEffectiveStrength) / iDefenderEffectiveStrength); // is this right? looks like more for less [Jon: Yes, it's XP for the defender]
		//iExperience = range(iExperience, GC.getMIN_EXPERIENCE_PER_COMBAT(), GC.getMAX_EXPERIENCE_PER_COMBAT());
		int iExperience = /*4*/ GC.getEXPERIENCE_DEFENDING_UNIT_MELEE();
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, pkDefender->isGGFromBarbarians() || !kAttacker.isBarbarian());
#else
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !kAttacker.isBarbarian());
#endif
		}
#else	
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !kAttacker.isBarbarian());
#endif
		//iExperience = ((iExperience * iDefenderEffectiveStrength) / iAttackerEffectiveStrength);
		//iExperience = range(iExperience, GC.getMIN_EXPERIENCE_PER_COMBAT(), GC.getMAX_EXPERIENCE_PER_COMBAT());
		iExperience = /*6*/ GC.getEXPERIENCE_ATTACKING_UNIT_MELEE();
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !pkDefender->isBarbarian());
#else
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !pkDefender->isBarbarian());
#endif
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
#if defined(MOD_UNITS_MAX_HP)
		else if (iAttackerTotalDamageInflicted >= iDefenderMaxHP && kAttacker.IsCaptureDefeatedEnemy() && kAttacker.AreUnitsOfSameType(*pkDefender))
#else
		else if (iAttackerTotalDamageInflicted >= iMaxHP && kAttacker.IsCaptureDefeatedEnemy() && kAttacker.AreUnitsOfSameType(*pkDefender))
#endif
		{
			int iCaptureRoll = GC.getGame().getSmallFakeRandNum(50, plot) + GC.getGame().getSmallFakeRandNum(50, pkDefender->GetID());

			if (iCaptureRoll < kAttacker.GetCaptureChance(pkDefender))
			{
				bAdvance = false;
				pkCombatInfo->setDefenderCaptured(true);
			}
		}
		else if (kAttacker.IsCanHeavyCharge() && !pkDefender->isDelayedDeath() && (iAttackerDamageInflicted > iDefenderDamageInflicted) )
		{
			bAdvance = true;
		}

		if (!kAttacker.plot()->MeleeAttackerAdvances())
			bAdvance = false;

		pkCombatInfo->setAttackerAdvances(bAdvance);
		pkCombatInfo->setDefenderRetaliates(true);
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
	int iAttackerDamageDelta = 0;

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	if(!pkTargetPlot && pkDefender)
		pkTargetPlot = pkDefender->plot();

	CvAssert_Debug(pkAttacker && pkDefender && pkTargetPlot);

	int iActivePlayerID = GC.getGame().getActivePlayer();

	bool bAttackerDidMoreDamage = false;

	if(pkAttacker != NULL && pkDefender != NULL && pkTargetPlot != NULL &&
	        pkDefender->IsCanDefend()) 		// Did the defender actually defend?
	{
		// Internal variables
		int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
		int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);
		int iAttackerFearDamageInflicted = 0;//pInfo->getFearDamageInflicted( BATTLE_UNIT_ATTACKER );

		bAttackerDidMoreDamage = iAttackerDamageInflicted > iDefenderDamageInflicted;

#if !defined(NO_ACHIEVEMENTS)
		//One Hit
		if(iAttackerDamageInflicted > pkDefender->GetCurrHitPoints() && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);
#endif

		pkDefender->changeDamage(iAttackerDamageInflicted, pkAttacker->getOwner());
		iAttackerDamageDelta = pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner(), -1.f);		// Signal that we don't want the popup text.  It will be added later when the unit is at its final location

#if defined(MOD_CORE_PER_TURN_DAMAGE)
		//don't count the "self-inflicted" damage on the attacker
		pkDefender->addDamageReceivedThisTurn(iAttackerDamageInflicted, pkAttacker);
#endif
	
		pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

		// Update experience for both sides.
#if defined(MOD_UNITS_XP_TIMES_100)
		pkDefender->changeExperienceTimes100(100 *
#else
		pkDefender->changeExperience(
#endif
		    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
		    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
		    true,
		    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
		    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER));

#if defined(MOD_UNITS_XP_TIMES_100)
		pkAttacker->changeExperienceTimes100(100 * 
#else
		pkAttacker->changeExperience(
#endif
		    kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
		    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
		    true,
		    kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
		    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER));

		// Anyone eat it?
#if defined(MOD_UNITS_MAX_HP)
		bAttackerDead = (pkAttacker->getDamage() >= pkAttacker->GetMaxHitPoints());
		bDefenderDead = (pkDefender->getDamage() >= pkDefender->GetMaxHitPoints());
#else
		bAttackerDead = (pkAttacker->getDamage() >= GC.getMAX_HIT_POINTS());
		bDefenderDead = (pkDefender->getDamage() >= GC.getMAX_HIT_POINTS());
#endif

#if !defined(NO_ACHIEVEMENTS)
		CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
		kAttackerOwner.GetPlayerAchievements().AttackedUnitWithUnit(pkAttacker, pkDefender);
#endif

		// Attacker died
		if(bAttackerDead)
		{
#if !defined(NO_ACHIEVEMENTS)
			CvPlayerAI& kDefenderOwner = GET_PLAYER(pkDefender->getOwner());
			kDefenderOwner.GetPlayerAchievements().KilledUnitWithUnit(pkDefender, pkAttacker);
#endif

			auto_ptr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
			gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());

			if(iActivePlayerID == pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", pkAttacker->getNameKey(), pkDefender->getNameKey(), iAttackerDamageInflicted, iAttackerFearDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
			}
			if(iActivePlayerID == pkDefender->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pkDefender->getNameKey(), iAttackerDamageInflicted, iAttackerFearDamageInflicted, pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
			}
			pkDefender->testPromotionReady();

			ApplyPostKillTraitEffects(pkDefender, pkAttacker);

		}
		// Defender died
		else if(bDefenderDead)
		{
#if !defined(NO_ACHIEVEMENTS)
			kAttackerOwner.GetPlayerAchievements().KilledUnitWithUnit(pkAttacker, pkDefender);
#endif

			auto_ptr<ICvUnit1> pDefender = GC.WrapUnitPointer(pkDefender);
			gDLL->GameplayUnitDestroyedInCombat(pDefender.get());

			if(iActivePlayerID == pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey());
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
			}

			if(pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkDefender->getNameKey(), pkAttacker->getNameKey(), iDefenderDamageInflicted);
			}
			else
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkDefender->getNameKey(), pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
			}
			if(iActivePlayerID == pkDefender->getOwner())
			{
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
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
			}
		}
		// Nobody died
		else
		{
			if(iActivePlayerID == pkAttacker->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
			}
			if(iActivePlayerID == pkDefender->getOwner())
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
			}

			pkDefender->testPromotionReady();
			pkAttacker->testPromotionReady();

		}

		// Minors want Barbs near them dead
		if(bAttackerDead)
		{
			if(pkAttacker->isBarbarian())
				DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkAttacker,pkDefender->getOwner());
		}
		else if(bDefenderDead)
		{
			if(pkDefender->isBarbarian())
				DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender,pkAttacker->getOwner());
		}
	}

	if(pkAttacker)
	{
		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(GetPostCombatDelay());
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
				if (pkAttacker->IsCanHeavyCharge() && !pkDefender->isDelayedDeath() && bAttackerDidMoreDamage)
				{
					pkDefender->DoFallBack(*pkAttacker);
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
#if defined(MOD_BALANCE_CORE)
					if (pkAttacker->getAOEDamageOnKill() != 0)
					{
						CvPlot* pAdjacentPlot = NULL;
						CvPlot* pPlot = GC.getMap().plot(pkAttacker->getX(), pkAttacker->getY());

						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

							if (pAdjacentPlot != NULL)
							{
								for (int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
								{
									CvUnit* pEnemyUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
									if (pEnemyUnit != NULL && pEnemyUnit->isEnemy(pkAttacker->getTeam()))
									{
										CvString strAppendText = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_AOE_STRIKE");
										pEnemyUnit->changeDamage(pkAttacker->getAOEDamageOnKill(), pkAttacker->getOwner(), 0.0, &strAppendText);
									}
								}
							}
						}
					}
#endif
				}
				else
				{
					pkAttacker->changeMoves(-1 * std::max(GC.getMOVE_DENOMINATOR(), pkTargetPlot->movementCost(pkAttacker, pkAttacker->plot(),pkAttacker->getMoves())));
#if defined(MOD_BALANCE_CORE)
					if(!pkAttacker->canMove() || !pkAttacker->isBlitz() || pkAttacker->isOutOfAttacks())
#else
					if(!pkAttacker->canMove() || !pkAttacker->isBlitz())
#endif
					{
						if(pkAttacker->IsSelected())
						{
							if(GC.GetEngineUserInterface()->GetLengthSelectionList() > 1)
							{
								auto_ptr<ICvUnit1> pDllAttacker = GC.WrapUnitPointer(pkAttacker);
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
	pkCombatInfo->setPlot(&plot);

	//////////////////////////////////////////////////////////////////////

	bool bBarbarian = false;
	int iExperience = 0;
	int iMaxXP = 0;
	int iDamage = 0;
	int iTotalDamage = 0;
	PlayerTypes eDefenderOwner;
	if(!plot.isCity())
	{
		CvAssert(pkDefender != NULL);

		eDefenderOwner = pkDefender->getOwner();

		iExperience = /*2*/ GC.getEXPERIENCE_ATTACKING_UNIT_RANGED();
		if(pkDefender->isBarbarian())
			bBarbarian = true;
		iMaxXP = pkDefender->maxXPValue();

		//CvAssert(pkDefender->IsCanDefend());

		iDamage = kAttacker.GetRangeCombatDamage(pkDefender, /*pCity*/ NULL, /*bIncludeRand*/ true);

#if defined(MOD_BALANCE_CORE)
		if(pkDefender->getForcedDamageValue() != 0)
		{
			iDamage = pkDefender->getForcedDamageValue();
		}
		if(pkDefender->getChangeDamageValue() != 0)
		{
			iDamage += pkDefender->getChangeDamageValue();
			if (iDamage <= 0)
				iDamage = 0;
		}
#endif
#if defined(MOD_UNITS_MAX_HP)
		if (iDamage + pkDefender->getDamage() > pkDefender->GetMaxHitPoints())
		{
			iDamage = pkDefender->GetMaxHitPoints() - pkDefender->getDamage();
		}
#else
		if(iDamage + pkDefender->getDamage() > GC.getMAX_HIT_POINTS())
		{
			iDamage = GC.getMAX_HIT_POINTS() - pkDefender->getDamage();
		}
#endif
#if defined(MOD_BALANCE_CORE)
		if (kAttacker.GetMoraleBreakChance() > 0 && !pkDefender->CanFallBack(kAttacker,false))
		{
			iDamage = (iDamage * 150) / 100;
		}
#endif

		iTotalDamage = std::max(pkDefender->getDamage(), pkDefender->getDamage() + iDamage);

		//Chance to spread promotion?
		if (kAttacker.getPlagueChance() > 0)
		{
			kAttacker.DoPlagueTransfer(*pkDefender);
		}
	}
	else
	{
		if (kAttacker.isRangedSupportFire()) return; // can't attack cities with this

		CvCity* pCity = plot.getPlotCity();
		CvAssert(pCity != NULL);
		if(!pCity) return;
		BATTLE_JOINED(pCity, BATTLE_UNIT_DEFENDER, true);

		eDefenderOwner = plot.getOwner();

		iExperience = /*3*/ GC.getEXPERIENCE_ATTACKING_CITY_RANGED();
		if(pCity->isBarbarian())
			bBarbarian = true;
		iMaxXP = 1000;

		iDamage = kAttacker.GetRangeCombatDamage(/*pDefender*/ NULL, pCity, /*bIncludeRand*/ true);

#if defined(MOD_BALANCE_CORE_MILITARY)
		//if there is a garrison, the unit absorbs part of the damage!
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		if(pGarrison && !pGarrison->IsDead())
		{
			//make sure there are no rounding errors
			int iGarrisonShare = (iDamage*2*pGarrison->GetMaxHitPoints()) / (pCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());

			/*
			//garrison can not be killed, only reduce to 10 hp
			iGarrisonShare = min(iGarrisonShare,pGarrison->GetCurrHitPoints()-10);
			*/

			if (iGarrisonShare>0)
			{
				iDamage -= iGarrisonShare;

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
		}
#endif

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

	// Fear Damage
	pkCombatInfo->setFearDamageInflicted(BATTLE_UNIT_ATTACKER, 0);
	// pkCombatInfo->setFearDamageInflicted( BATTLE_UNIT_DEFENDER, 0 );

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, iMaxXP);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == eDefenderOwner);
#if defined(MOD_BUGFIX_BARB_GP_XP)
	bool bGeneralsXP = !kAttacker.isBarbarian();
	if (MOD_BUGFIX_BARB_GP_XP)
	{
		if (!plot.isCity())
		{
			bGeneralsXP = !pkDefender->isBarbarian();
		} 
		else
		{
			bGeneralsXP = !plot.getPlotCity()->isBarbarian();
		}
	}
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || bGeneralsXP);
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif

#else
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || bGeneralsXP);
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif
#endif

	iExperience = /*2*/ GC.getEXPERIENCE_DEFENDING_UNIT_RANGED();
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, (!plot.isCity() && pkDefender->isGGFromBarbarians()) || (!bBarbarian && !kAttacker.isBarbarian()));
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !bBarbarian && !kAttacker.isBarbarian());
#endif
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

	bool bBarbarian = false;
	int iDamage = 0;
	int iTotalDamage = 0;
	PlayerTypes eDefenderOwner = NO_PLAYER;
	if(!plot.isCity())
	{
		CvAssert(pkDefender != NULL);

		eDefenderOwner = pkDefender->getOwner();

		if(pkDefender->isBarbarian())
			bBarbarian = true;

		//CvAssert(pkDefender->IsCanDefend());

		iDamage = kAttacker.rangeCombatDamage(pkDefender);

#if defined(MOD_BALANCE_CORE)
		if(pkDefender->getForcedDamageValue() != 0)
		{
			iDamage = pkDefender->getForcedDamageValue();
		}
		if(pkDefender->getChangeDamageValue() != 0)
		{
			iDamage += pkDefender->getChangeDamageValue();
			if (iDamage <= 0)
				iDamage = 0;
		}
#endif
#if defined(MOD_UNITS_MAX_HP)
		if(iDamage + pkDefender->getDamage() > pkDefender->GetMaxHitPoints())
		{
			iDamage = pkDefender->GetMaxHitPoints() - pkDefender->getDamage();
		}
#else
		if(iDamage + pkDefender->getDamage() > GC.getMAX_HIT_POINTS())
		{
			iDamage = GC.getMAX_HIT_POINTS() - pkDefender->getDamage();
		}
#endif

		iTotalDamage = std::max(pkDefender->getDamage(), pkDefender->getDamage() + iDamage);
	}
	else
	{
		FAssertMsg(false, "City vs. City not supported.");	// Don't even think about it Jon....
	}

	//////////////////////////////////////////////////////////////////////

	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);				// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iDamage);		// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iTotalDamage);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);			// Damage inflicted this round

	// Fear Damage
	pkCombatInfo->setFearDamageInflicted(BATTLE_UNIT_ATTACKER, 0);
	// pkCombatInfo->setFearDamageInflicted( BATTLE_UNIT_DEFENDER, 0 );

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == eDefenderOwner);

#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian()); // Kind of irrelevant, as cities don't get XP!
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif

	int iExperience = /*2*/ GC.getEXPERIENCE_DEFENDING_UNIT_RANGED();
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, pkDefender->isGGFromBarbarians() || !bBarbarian && !kAttacker.isBarbarian());
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !bBarbarian && !kAttacker.isBarbarian());
#endif
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
	int iDamage = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
//	int iExperience = kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER);
//	int iMaxXP = kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER);
	bool bBarbarian = false;

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvAssert_Debug(pkAttacker);

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	CvAssert_Debug(pkTargetPlot);

	ICvUserInterface2* pkDLLInterface = GC.GetEngineUserInterface();
	CvString strBuffer;

	if(pkTargetPlot)
	{
		if(!pkTargetPlot->isCity())
		{
			// Unit
			CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
			CvAssert_Debug(pkDefender != NULL);
			if(pkDefender)
			{
				if(pkAttacker)
				{
#if defined(MOD_BALANCE_CORE)
					if (pkAttacker->getSplashDamage() != 0)
					{
						CvPlot* pAdjacentPlot = NULL;
						CvPlot* pPlot = GC.getMap().plot(pkDefender->getX(), pkDefender->getY());

						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

							if (pAdjacentPlot != NULL && pkAttacker->canEverRangeStrikeAt(pAdjacentPlot->getX(), pAdjacentPlot->getY(), pkAttacker->plot(), false))
							{
								for (int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
								{
									CvUnit* pEnemyUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
									if (pEnemyUnit != NULL && pEnemyUnit->isEnemy(pkAttacker->getTeam()))
									{
										CvString strAppendText = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_SPLASH");
										pEnemyUnit->changeDamage(pkAttacker->getSplashDamage(), pkAttacker->getOwner(), 0.0, &strAppendText);
									}
								}
							}
						}
					}
					pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);
#endif

					// Defender died
#if defined(MOD_UNITS_MAX_HP)
					if(iDamage + pkDefender->getDamage() >= pkDefender->GetMaxHitPoints())
#else
					if(iDamage + pkDefender->getDamage() >= GC.getMAX_HIT_POINTS())
#endif
					{
#if !defined(NO_ACHIEVEMENTS)
						//One Hit
						if(!pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
							gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);
#endif

						if(pkAttacker->getOwner() == GC.getGame().getActivePlayer())
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_AND_DEATH", pkAttacker->getNameKey(), pkDefender->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}

						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR_AND_DEATH", pkDefender->getNameKey(), pkAttacker->getNameKey());
						CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
						if(pNotifications)
						{
							Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
							pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
						}

						bTargetDied = true;

#if !defined(NO_ACHIEVEMENTS)
						CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
						kAttackerOwner.GetPlayerAchievements().KilledUnitWithUnit(pkAttacker, pkDefender);
#endif

						ApplyPostKillTraitEffects(pkAttacker, pkDefender);

						if(pkDefender->isBarbarian())
						{
							DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender, pkAttacker->getOwner());
						}
					}
					// Nobody died
					else
					{
						if(pkAttacker->getOwner() == GC.getGame().getActivePlayer())
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR", pkAttacker->getNameKey(), pkDefender->getNameKey(), iDamage);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR", pkDefender->getNameKey(), pkAttacker->getNameKey(), iDamage);
#if defined(MOD_BALANCE_CORE)
						if(pkDefender->IsFortified() || (pkDefender->GetActivityType() == ACTIVITY_HEAL))
						{
							pkDefender->SetActivityType(ACTIVITY_AWAKE);
						}
						if (pkAttacker->GetMoraleBreakChance() > 0 && !pkDefender->isDelayedDeath() && pkDefender->CanFallBack(*pkAttacker,false))
						{
							int iRand = GC.getGame().getSmallFakeRandNum(100, pkDefender->GetID()+pkDefender->plot()->GetPlotIndex());
							if(iRand <= pkAttacker->GetMoraleBreakChance())
							{
								pkDefender->DoFallBack(*pkAttacker);

								CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
								if(pNotifications)
								{
									Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK");
									strMessage << pkAttacker->getUnitInfo().GetTextKey();
									strMessage << pkDefender->getUnitInfo().GetTextKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK_S");
									strSummary << pkDefender->getUnitInfo().GetTextKey();
									pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
								}
								CvNotifications* pNotificationsOther = GET_PLAYER(pkAttacker->getOwner()).GetNotifications();
								if(pNotificationsOther)
								{
									Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK_THEM");
									strMessage << pkAttacker->getUnitInfo().GetTextKey();
									strMessage << pkDefender->getUnitInfo().GetTextKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_MORALE_FALL_BACK_S");
									strSummary << pkDefender->getUnitInfo().GetTextKey();
									pNotificationsOther->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
								}
							}
						}
#endif
					}

					//red icon over attacking unit
					if(pkDefender->getOwner() == GC.getGame().getActivePlayer())
					{
						pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkAttacker->m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkAttacker->getX(), pkAttacker->getY(), true, true*/);
					}
					//white icon over defending unit
					//pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), false, 0, ""/*, "AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pkDefender->getX(), pkDefender->getY(), true, true*/);

					//set damage but don't update entity damage visibility
					pkDefender->changeDamage(iDamage, pkAttacker->getOwner());

#if defined(MOD_CORE_PER_TURN_DAMAGE)
					pkDefender->addDamageReceivedThisTurn(iDamage, pkAttacker);
#endif

					// Update experience
#if defined(MOD_UNITS_XP_TIMES_100)
					pkDefender->changeExperienceTimes100(100 * 
#else
					pkDefender->changeExperience(
#endif
					    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
					    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
					    true,
					    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
					    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER));
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
			CvAssert_Debug(pCity != NULL);
			if(pCity)
			{
				if(pkAttacker)
				{
					bBarbarian = pCity->isBarbarian();
#if defined(MOD_BALANCE_CORE)
					if(pCity->getDamage() != pCity->GetMaxHitPoints())
					{
						ApplyPostCityCombatEffects(pkAttacker, pCity, iDamage);
					}
#endif
					pCity->changeDamage(iDamage);

#if defined(MOD_CORE_PER_TURN_DAMAGE)
					pCity->addDamageReceivedThisTurn(iDamage);
#endif

					if(pCity->getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOUR_CITY_ATTACKED_BY_AIR", pCity->getNameKey(), pkAttacker->getNameKey(), iDamage);
						//red icon over attacking unit
						pkDLLInterface->AddMessage(uiParentEventID, pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkAttacker->m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkAttacker->getX(), pkAttacker->getY(), true, true*/);
					}
#if defined(MOD_BALANCE_CORE)
					else if(pkAttacker->getOwner() == GC.getGame().getActivePlayer() && (pCity->getOwner() != pkAttacker->getOwner()))
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACKED_CITY_CP", pCity->getNameKey(), pkAttacker->getNameKey(), iDamage);
						//red icon over attacking unit
						pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkAttacker->m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkAttacker->getX(), pkAttacker->getY(), true, true*/);
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
#if defined(MOD_UNITS_XP_TIMES_100)
			pkAttacker->changeExperienceTimes100(100 * 
#else
			pkAttacker->changeExperience(
#endif
			    kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
			    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
			    true,
			    kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
			    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER));
		}

		pkAttacker->testPromotionReady();

		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(GetPostCombatDelay());
	}
	
	BATTLE_FINISHED();
}

//	---------------------------------------------------------------------------
//	Function: ResolveRangedCityVsUnitCombat
//	Resolve ranged combat where the attacker is a city
//	---------------------------------------------------------------------------
void CvUnitCombat::ResolveRangedCityVsUnitCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	bool bTargetDied = false;
	int iDamage = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	bool bBarbarian = false;

	CvCity* pkAttacker = kCombatInfo.getCity(BATTLE_UNIT_ATTACKER);
	CvAssert_Debug(pkAttacker);

	if(pkAttacker)
		pkAttacker->clearCombat();

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	CvAssert_Debug(pkTargetPlot);

	ICvUserInterface2* pkDLLInterface = GC.GetEngineUserInterface();
	int iActivePlayerID = GC.getGame().getActivePlayer();

	if(pkTargetPlot)
	{
		if(!pkTargetPlot->isCity())
		{
			CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
			CvAssert_Debug(pkDefender != NULL);
			if(pkDefender)
			{
				bBarbarian = pkDefender->isBarbarian();

				if(pkAttacker)
				{
					pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

					// Info message for the attacking player
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOUR_CITY_RANGE_ATTACK");
						localizedText << pkAttacker->getNameKey() << pkDefender->getNameKey() << iDamage;
						pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());//, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX(), pPlot->getY());
					}

					// Red icon over defending unit
					if(iActivePlayerID == pkDefender->getOwner())
					{
						Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_CITY");
						localizedText << pkDefender->getNameKey() << pkAttacker->getNameKey() << iDamage;
						pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());//, "AS2D_COMBAT", MESSAGE_TYPE_COMBAT_MESSAGE, pDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pDefender->getX(), pDefender->getY(), true, true);
					}

#if defined(MOD_UNITS_MAX_HP)
					if(iDamage + pkDefender->getDamage() >= pkDefender->GetMaxHitPoints())
#else
					if(iDamage + pkDefender->getDamage() >= GC.getMAX_HIT_POINTS())
#endif
					{
						CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
						if(pNotifications)
						{
							Localization::String localizedText = Localization::Lookup("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_CITY");
							localizedText << pkDefender->getNameKey() << pkAttacker->getNameKey() << iDamage;
							Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
							pNotifications->Add(NOTIFICATION_UNIT_DIED, localizedText.toUTF8(), strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
						}
						bTargetDied = true;

						// Earn bonuses for kills?
						CvPlayer& kAttackingPlayer = GET_PLAYER(pkAttacker->getOwner());
						kAttackingPlayer.DoYieldsFromKill(NULL, pkDefender, pkAttacker);
					}

					//set damage but don't update entity damage visibility
					pkDefender->changeDamage(iDamage, pkAttacker->getOwner());

#if defined(MOD_CORE_PER_TURN_DAMAGE)
					pkDefender->addDamageReceivedThisTurn(iDamage);
#endif

					// Update experience
#if defined(MOD_UNITS_XP_TIMES_100)
					pkDefender->changeExperienceTimes100(100 * 
#else
					pkDefender->changeExperience(
#endif
					    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
					    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
					    true,
					    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
					    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER));
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
			CvAssert(false);	// Left as an exercise for the reader
			bTargetDied = true;
		}
	}

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

	CvAssert_Debug(pkAttacker && pkDefender);

	CvPlot* pkPlot = kCombatInfo.getPlot();
	if(!pkPlot && pkDefender)
		pkPlot = pkDefender->plot();

	CvAssert_Debug(pkPlot);

	int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	if(pkAttacker && pkDefender)
	{
		pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner());
		pkDefender->changeDamage(iAttackerDamageInflicted);

#if defined(MOD_CORE_PER_TURN_DAMAGE)
		pkDefender->addDamageReceivedThisTurn(iAttackerDamageInflicted);
#endif
		pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

#if defined(MOD_UNITS_XP_TIMES_100)
		pkAttacker->changeExperienceTimes100(100 * kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
#else
		pkAttacker->changeExperience(kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
#endif
		                             kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
		                             true,
		                             false,
		                             kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER));
	}

	if(pkDefender)
		pkDefender->clearCombat();

	if(pkAttacker)
	{
		pkAttacker->setCombatUnit(NULL);
		pkAttacker->ClearMissionQueue(GetPostCombatDelay());

		if(pkAttacker->isSuicide())
		{
			pkAttacker->setCombatUnit(NULL);	// Must clear this if doing a delayed kill, should this be part of the kill method?
			pkAttacker->kill(true);
		}
	}

	CvString strBuffer;
	int iActivePlayerID = GC.getGame().getActivePlayer();

	// Barbarians don't capture Cities
	if(pkAttacker && pkDefender)
	{
		if(pkAttacker->isBarbarian() && (pkDefender->getDamage() >= pkDefender->GetMaxHitPoints()))
		{
			// 1 HP left
			pkDefender->setDamage(pkDefender->GetMaxHitPoints() - 1);

			int iNumGoldStolen = GC.getBARBARIAN_CITY_GOLD_RANSOM();	// 200

			if(iNumGoldStolen > GET_PLAYER(pkDefender->getOwner()).GetTreasury()->GetGold())
			{
				iNumGoldStolen = GET_PLAYER(pkDefender->getOwner()).GetTreasury()->GetGold();
			}

			// City is ransomed for Gold
			GET_PLAYER(pkDefender->getOwner()).GetTreasury()->ChangeGold(-iNumGoldStolen);

			if(pkDefender->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CITY_RANSOMED_BY_BARBARIANS", pkDefender->getNameKey(), iNumGoldStolen);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
			}

#if !defined(NO_ACHIEVEMENTS)
			if( pkDefender->GetPlayer()->GetID() == GC.getGame().getActivePlayer() && pkDefender->isHuman() && !GC.getGame().isGameMultiPlayer())
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_REALLY_SUCK);
			}
#endif

			// Barb goes away after ransom
			pkAttacker->kill(true, NO_PLAYER);
		}
		// Attacker died
		else if(pkAttacker->IsDead())
		{
			auto_ptr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
			gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());
			if(pkAttacker->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING_CITY", pkAttacker->getNameKey(), pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
			}
			if(pkDefender->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT_CITY", pkDefender->getNameKey(), iAttackerDamageInflicted, pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkPlot->getX(), pkPlot->getY()*/);
			}
		}
		// City conquest
		else if(pkDefender->getDamage() >= pkDefender->GetMaxHitPoints())
		{
			if(!pkAttacker->isNoCapture())
			{
				if(pkAttacker->getOwner() == iActivePlayerID)
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_CAPTURED_ENEMY_CITY", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey());
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkPlot->getX(), pkPlot->getY()*/);
				}
				if(pkDefender->getOwner() == iActivePlayerID)
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CITY_WAS_CAPTURED", pkDefender->getNameKey(), pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
				}

				pkAttacker->UnitMove(pkPlot, true, pkAttacker);
			}
		}
		// Neither side lost
		else
		{
			if(pkAttacker->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW_CITY", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkPlot->getX(), pkPlot->getY()*/);
			}
			if(pkDefender->getOwner() == iActivePlayerID)
			{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW_CITY", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkPlot->getX(), pkPlot->getY()*/);
			}
			pkAttacker->changeMoves(-1 * std::max(GC.getMOVE_DENOMINATOR(), pkPlot->movementCost(pkAttacker, pkAttacker->plot(), pkAttacker->getMoves())));

			ApplyPostCityCombatEffects(pkAttacker, pkDefender, iAttackerDamageInflicted);
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
		if(kAttacker.evasionProbability()==0 || GC.getGame().getSmallFakeRandNum(100, plot.GetPlotIndex()+pInterceptor->GetID()+pInterceptor->getMadeInterceptionCount()) >= kAttacker.evasionProbability())
		{
			// Is the interception successful?
			if (pInterceptor->interceptionProbability()>=100 || GC.getGame().getSmallFakeRandNum(100, plot.GetPlotIndex()+kAttacker.GetID()+kAttacker.getDamage()) <= pInterceptor->interceptionProbability())
			{
				iInterceptionDamage = pInterceptor->GetInterceptionDamage(&kAttacker, true, &plot);
			}
		}
		if (iInterceptionDamage > 0)
		{
			pkCombatInfo->setUnit(BATTLE_UNIT_INTERCEPTOR, pInterceptor);
			pkCombatInfo->setDamageInflicted(BATTLE_UNIT_INTERCEPTOR, iInterceptionDamage);		// Damage inflicted this round

			int iExperience = /*2*/ GC.getEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND();
			pkCombatInfo->setExperience( BATTLE_UNIT_INTERCEPTOR, iExperience );
			pkCombatInfo->setMaxExperienceAllowed( BATTLE_UNIT_INTERCEPTOR, MAX_INT );
			pkCombatInfo->setInBorders( BATTLE_UNIT_INTERCEPTOR, plot.getOwner() == kAttacker.getOwner() );
			pkCombatInfo->setUpdateGlobal( BATTLE_UNIT_INTERCEPTOR, true );
			pkCombatInfo->setAttackIsBombingMission(true);
			pkCombatInfo->setDefenderRetaliates(false);

			//make sure we have zero values everywhere else
			pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, 0);
			pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);

			pkCombatInfo->setFearDamageInflicted(BATTLE_UNIT_ATTACKER, 0);

			pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, 0);
			pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() != kAttacker.getOwner());	// Not really correct
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

	int iAttackerDamageInflicted;
	int iDefenderDamageInflicted;

	int iAttackerTotalDamageInflicted;
	int iDefenderTotalDamageInflicted;

	PlayerTypes eDefenderOwner;

	// Target is a Unit
	// Special: Missiles always attack the unit, even when it's in a city
	if(!plot.isCity() || kAttacker.AI_getUnitAIType() == UNITAI_MISSILE_AIR )
	{
		CvAssert(pkDefender != NULL);
		if(!pkDefender)
			return;

		eDefenderOwner = pkDefender->getOwner();

		iExperience = /*4*/ GC.getEXPERIENCE_ATTACKING_UNIT_AIR();
		if(pkDefender->isBarbarian())
			bBarbarian = true;
		iMaxXP = pkDefender->maxXPValue();

		// Calculate attacker damage
		iAttackerDamageInflicted = kAttacker.GetAirCombatDamage(pkDefender, /*pCity*/ NULL, /*bIncludeRand*/ true);

#if defined(MOD_BALANCE_CORE)
		if(pkDefender->getForcedDamageValue() != 0)
		{
			iAttackerDamageInflicted = pkDefender->getForcedDamageValue();
		}
		if(pkDefender->getChangeDamageValue() != 0)
		{
			iAttackerDamageInflicted += pkDefender->getChangeDamageValue();
			if (iAttackerDamageInflicted <= 0)
				iAttackerDamageInflicted = 0;
		}
#endif
#if defined(MOD_UNITS_MAX_HP)
		if(iAttackerDamageInflicted + pkDefender->getDamage() > pkDefender->GetMaxHitPoints())
		{
			iAttackerDamageInflicted = pkDefender->GetMaxHitPoints() - pkDefender->getDamage();
		}
#else
		if(iAttackerDamageInflicted + pkDefender->getDamage() > GC.getMAX_HIT_POINTS())
		{
			iAttackerDamageInflicted = GC.getMAX_HIT_POINTS() - pkDefender->getDamage();
		}
#endif

		iAttackerTotalDamageInflicted = std::max(pkDefender->getDamage(), pkDefender->getDamage() + iAttackerDamageInflicted);

		// Calculate defense damage
		iDefenderDamageInflicted = pkDefender->GetAirStrikeDefenseDamage(&kAttacker);

#if defined(MOD_BALANCE_CORE)
		if(kAttacker.getForcedDamageValue() != 0)
		{
			iDefenderDamageInflicted = kAttacker.getForcedDamageValue();
		}
		if(kAttacker.getChangeDamageValue() != 0)
		{
			iDefenderDamageInflicted += kAttacker.getChangeDamageValue();
			if (iDefenderDamageInflicted <= 0)
				iDefenderDamageInflicted = 0;
		}
#endif
#if defined(MOD_UNITS_MAX_HP)
		if(iDefenderDamageInflicted + kAttacker.getDamage() > kAttacker.GetMaxHitPoints())
		{
			iDefenderDamageInflicted = kAttacker.GetMaxHitPoints() - kAttacker.getDamage();
		}
#else
		if(iDefenderDamageInflicted + kAttacker.getDamage() > GC.getMAX_HIT_POINTS())
		{
			iDefenderDamageInflicted = GC.getMAX_HIT_POINTS() - kAttacker.getDamage();
		}
#endif

		iDefenderTotalDamageInflicted = std::max(kAttacker.getDamage(), kAttacker.getDamage() + iDefenderDamageInflicted);
	}
	// Target is a City
	else
	{
		CvCity* pCity = plot.getPlotCity();
		CvAssert(pCity != NULL);
		if(!pCity) return;
		BATTLE_JOINED(pCity, BATTLE_UNIT_DEFENDER, true);

		eDefenderOwner = plot.getOwner();

		iExperience = /*4*/ GC.getEXPERIENCE_ATTACKING_CITY_AIR();
		if(pCity->isBarbarian())
			bBarbarian = true;
		iMaxXP = 1000;

		iAttackerDamageInflicted = kAttacker.GetAirCombatDamage(/*pUnit*/ NULL, pCity, /*bIncludeRand*/ true);

#if defined(MOD_BALANCE_CORE_MILITARY)
		//if there is a garrison, the unit absorbs part of the damage!
		CvUnit* pGarrison = pCity->GetGarrisonedUnit();
		if(pGarrison)
		{
			//make sure there are no rounding errors
			int iGarrisonShare = (iAttackerDamageInflicted*2*pGarrison->GetMaxHitPoints()) / (pCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());

			/*
			//garrison can not be killed, only reduce to 10 hp
			iGarrisonShare = min(iGarrisonShare,pGarrison->GetCurrHitPoints()-10);
			*/

			if (iGarrisonShare>0)
			{
				iAttackerDamageInflicted -= iGarrisonShare;

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
		}
#endif

		// Cities can't be knocked to less than 1 HP
		if(iAttackerDamageInflicted + pCity->getDamage() >= pCity->GetMaxHitPoints())
		{
			iAttackerDamageInflicted = pCity->GetMaxHitPoints() - pCity->getDamage() - 1;
		}

		iAttackerTotalDamageInflicted = std::max(pCity->getDamage(), pCity->getDamage() + iAttackerDamageInflicted);

		// Calculate defense damage
		iDefenderDamageInflicted = pCity->GetAirStrikeDefenseDamage(&kAttacker);

		if(iDefenderDamageInflicted + kAttacker.getDamage() > pCity->GetMaxHitPoints())
		{
#if defined(MOD_BUGFIX_MINOR)
			// Surely!!!
			iDefenderDamageInflicted = kAttacker.GetMaxHitPoints() - kAttacker.getDamage();
#else
			iDefenderDamageInflicted = GC.getMAX_HIT_POINTS() - kAttacker.getDamage();
#endif
		}

		iDefenderTotalDamageInflicted = std::max(kAttacker.getDamage(), kAttacker.getDamage() + iDefenderDamageInflicted);

#if !defined(NO_ACHIEVEMENTS)
		//Achievement for Washington
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(kAttacker.getUnitType());
		if(pkUnitInfo)
		{
			if(kAttacker.isHuman() && !GC.getGame().isGameMultiPlayer() && _stricmp(pkUnitInfo->GetType(), "UNIT_AMERICAN_B17") == 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_B17);
			}
		}
#endif
	}
	//////////////////////////////////////////////////////////////////////

	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderTotalDamageInflicted);				// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);		// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerTotalDamageInflicted);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);			// Damage inflicted this round

	// Fear Damage
	pkCombatInfo->setFearDamageInflicted(BATTLE_UNIT_ATTACKER, 0);
	// pkCombatInfo->setFearDamageInflicted( BATTLE_UNIT_DEFENDER, 0 );

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, iExperience);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, iMaxXP);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() == eDefenderOwner);
#if defined(MOD_BUGFIX_BARB_GP_XP)
	bool bGeneralsXP = !kAttacker.isBarbarian();
	if (MOD_BUGFIX_BARB_GP_XP)
	{
		if (!plot.isCity())
		{
			bGeneralsXP = !pkDefender->isBarbarian();
		}
		else
		{
			bGeneralsXP = !plot.getPlotCity()->isBarbarian();
		}
	}
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || bGeneralsXP);
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif

#else
#if defined(MOD_BARBARIAN_GG_GA_POINTS)
	if(GC.getGame().isOption(GAMEOPTION_BARB_GG_GA_POINTS))
	{
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, true);
	}
	else
	{
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || bGeneralsXP);
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif
	}
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, bGeneralsXP);
#endif
#endif

	iExperience = /*2*/ GC.getEXPERIENCE_DEFENDING_UNIT_AIR();
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, (!plot.isCity() && pkDefender->isGGFromBarbarians()) || !bBarbarian);
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !bBarbarian);
#endif
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
	bool bTargetDied = false;
	int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);

	// If there's no valid attacker, then get out of here
	CvAssert_Debug(pkAttacker);

	// Interception?
	int iInterceptionDamage = kCombatInfo.getDamageInflicted(BATTLE_UNIT_INTERCEPTOR);
	if(iInterceptionDamage > 0)
		iDefenderDamageInflicted += iInterceptionDamage;

	//shouldn't happen...
	if (iDefenderDamageInflicted <= 0)
		iDefenderDamageInflicted = 1;

	CvUnit* pInterceptor = kCombatInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
	CvAssert_Debug(pInterceptor);
	if(pInterceptor)
	{
		pInterceptor->increaseInterceptionCount();
		pInterceptor->setCombatUnit(NULL);
#if defined(MOD_UNITS_XP_TIMES_100)
		pInterceptor->changeExperienceTimes100(100 * 
#else
		pInterceptor->changeExperience(
#endif
			kCombatInfo.getExperience(BATTLE_UNIT_INTERCEPTOR),
			kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_INTERCEPTOR),
			true,
			kCombatInfo.getInBorders(BATTLE_UNIT_INTERCEPTOR),
			kCombatInfo.getUpdateGlobal(BATTLE_UNIT_INTERCEPTOR));
	}

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	CvAssert_Debug(pkTargetPlot);

	ICvUserInterface2* pkDLLInterface = GC.GetEngineUserInterface();
	int iActivePlayerID = GC.getGame().getActivePlayer();
	CvString strBuffer;

	if(pkTargetPlot)
	{
		if(!pkTargetPlot->isCity() || pkAttacker->AI_getUnitAIType()==UNITAI_MISSILE_AIR)
		{
			// Target was a Unit
			CvUnit* pkDefender = kCombatInfo.getUnit(BATTLE_UNIT_DEFENDER);
			CvAssert_Debug(pkDefender != NULL);

			if(pkDefender)
			{
#if !defined(NO_ACHIEVEMENTS)
				//One Hit
				if(iAttackerDamageInflicted > pkDefender->GetCurrHitPoints() && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
					gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);
#endif

				pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner());
				pkDefender->changeDamage(iAttackerDamageInflicted, pkAttacker->getOwner());

#if defined(MOD_CORE_PER_TURN_DAMAGE)
				//don't count the "self-inflicted" damage on the attacker
				pkDefender->addDamageReceivedThisTurn(iAttackerDamageInflicted, pkAttacker);
#endif

				pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);

					// Update experience
#if defined(MOD_UNITS_XP_TIMES_100)
				pkDefender->changeExperienceTimes100(100 * 
#else
				pkDefender->changeExperience(
#endif
					kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
					kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
					true,
					kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
					kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER));

				// Attacker died
				if(pkAttacker->IsDead())
				{
					auto_ptr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
					gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());

#if !defined(NO_ACHIEVEMENTS)
					CvPlayerAI& kDefenderOwner = GET_PLAYER(pkDefender->getOwner());
					kDefenderOwner.GetPlayerAchievements().KilledUnitWithUnit(pkDefender, pkAttacker);
#endif

					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_INTERCEPTED_KILLED", pInterceptor->getNameKey(), pkAttacker->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", pkAttacker->getNameKey(), pkDefender->getNameKey(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}
					if(iActivePlayerID == pkDefender->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_DESTROYED", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), pkAttacker->getNameKey(), pkDefender->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pkDefender->getNameKey(), iAttackerDamageInflicted, 0, pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
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
#if !defined(NO_ACHIEVEMENTS)
					CvPlayerAI& kAttackerOwner = GET_PLAYER(pkAttacker->getOwner());
					kAttackerOwner.GetPlayerAchievements().KilledUnitWithUnit(pkAttacker, pkDefender);
#endif

					//if the defender died, there was no interception because it would have aborted the attack!
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getNameKey(), iInterceptionDamage, pkDefender->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_AND_DEATH", pkAttacker->getNameKey(), pkDefender->getNameKey());
						pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					}
					else if(iActivePlayerID == pkDefender->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}

						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR_AND_DEATH", pkDefender->getNameKey(), pkAttacker->getNameKey());
						pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					}

					CvNotifications* pNotifications = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotifications->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), pkDefender->getX(), pkDefender->getY(), (int) pkDefender->getUnitType(), pkDefender->getOwner());
					}

					bTargetDied = true;

					ApplyPostKillTraitEffects(pkAttacker, pkDefender);

#if defined(MOD_BUGFIX_MINOR)
					// Friendship from barb death via air-strike
					if(pkDefender->isBarbarian())
					{
						DoTestBarbarianThreatToMinorsWithThisUnitsDeath(pkDefender, pkAttacker->getOwner());
					}
#endif
				}
				// Nobody died
				else
				{
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_UNIT_BY_AIR_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getNameKey(), iInterceptionDamage);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR", pkAttacker->getNameKey(), pkDefender->getNameKey(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}
					else if(iActivePlayerID == pkDefender->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), pkAttacker->getNameKey(), iInterceptionDamage, pkDefender->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR", pkDefender->getNameKey(), pkAttacker->getNameKey(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}
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
			// Target was a City
			CvCity* pCity = pkTargetPlot->getPlotCity();
			CvAssert_Debug(pCity != NULL);

			if(pCity)
			{
				pCity->clearCombat();
				pCity->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);
				pCity->changeDamage(iAttackerDamageInflicted);
				pkAttacker->changeDamage(iDefenderDamageInflicted, pCity->getOwner());

				if(pkAttacker->IsDead())
				{
					if(iActivePlayerID == pkAttacker->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_INTERCEPTED_KILLED", pInterceptor->getNameKey(), pkAttacker->getNameKey());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING_CITY", pkAttacker->getNameKey(), pCity->getNameKey(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
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
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_INTERCEPTED", pInterceptor->getNameKey(), pkAttacker->getNameKey(), iDefenderDamageInflicted, pCity->getName().GetCString());
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else if (iInterceptionDamage <= 0)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ATTACK_BY_AIR_ACTUALLY", pkAttacker->getNameKey(), pCity->getName().GetCString(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}
					if(iActivePlayerID == pCity->getOwner())
					{
						if (iInterceptionDamage > 0 && pInterceptor)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_AIR_UNIT_INTERCEPTED_CITY", pInterceptor->getNameKey(), pkAttacker->getVisualCivAdjective(GET_PLAYER(pCity->getOwner()).getTeam()), pkAttacker->getNameKey(), iDefenderDamageInflicted, pCity->getName().GetCString());
							pkDLLInterface->AddMessage(uiParentEventID, pCity->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
						else if (iInterceptionDamage <= 0)
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_ARE_ATTACKED_BY_AIR_CITY", pCity->getName().GetCString(), pkAttacker->getNameKey(), iAttackerDamageInflicted);
							pkDLLInterface->AddMessage(uiParentEventID, pCity->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_COMBAT", MESSAGE_TYPE_INFO, pkDefender->getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}
					}
				}

#if defined(MOD_BALANCE_CORE_MILITARY)
				//apply damage to garrison
				CvUnitCombat::ApplyExtraUnitDamage(pkAttacker, kCombatInfo, uiParentEventID);
#endif
			}
			else
				bTargetDied = true;
		}
	}
	else
		bTargetDied = true;

	// Suicide Unit (e.g. Missiles)
	if(pkAttacker)
	{
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
#if defined(MOD_UNITS_XP_TIMES_100)
				pkAttacker->changeExperienceTimes100(100 * kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
#else
				pkAttacker->changeExperience(kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
#endif
				                             kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
				                             true,
				                             kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
				                             kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER));

				// Promotion time?
				pkAttacker->testPromotionReady();

			}

			// Clean up some stuff
			pkAttacker->setCombatUnit(NULL);
			pkAttacker->ClearMissionQueue(GetPostCombatDelay());

			// Spend a move for this attack
			pkAttacker->changeMoves(-GC.getMOVE_DENOMINATOR());

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
#if defined(MOD_UNITS_MAX_HP)
	int iAttackerMaxHP = kAttacker.GetMaxHitPoints();
#else
	int iMaxHP = GC.getMAX_HIT_POINTS();
#endif

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
		int iInterceptionDamage = pkDefender->GetInterceptionDamage(&kAttacker, true, &plot);

		// Reduce damage for performing a sweep
		iInterceptionDamage *= 100 + GC.getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD();
		iInterceptionDamage /= 100;

		iDefenderExperience = /*2*/ GC.getEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND();

		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iInterceptionDamage);		// Damage inflicted this round
	}
	// Air interceptor
	else
	{
		iDefenderStrength = pkDefender->GetMaxRangedCombatStrength(&kAttacker, /*pCity*/ NULL, false);
#if defined(MOD_UNITS_MAX_HP)
		int iDefenderMaxHP = pkDefender->GetMaxHitPoints();
#endif

		bool bIncludeRand = !GC.getGame().isGameMultiPlayer();

		int iAttackerDamageInflicted = kAttacker.getCombatDamage(iAttackerStrength, iDefenderStrength, /*bIncludeRand*/ bIncludeRand, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
		int iDefenderDamageInflicted = pkDefender->getCombatDamage(iDefenderStrength, iAttackerStrength, /*bIncludeRand*/ bIncludeRand, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);

		int iAttackerTotalDamageInflicted = iAttackerDamageInflicted + pkDefender->getDamage();
		int iDefenderTotalDamageInflicted = iDefenderDamageInflicted + kAttacker.getDamage();

		// Will both units be killed by this? :o If so, take drastic corrective measures
#if defined(MOD_UNITS_MAX_HP)
		if (iAttackerTotalDamageInflicted >= iDefenderMaxHP && iDefenderTotalDamageInflicted >= iAttackerMaxHP)
#else
		if (iAttackerTotalDamageInflicted >= iMaxHP && iDefenderTotalDamageInflicted >= iMaxHP)
#endif
		{
			// He who hath the least amount of damage survives with 1 HP left
			if(iAttackerTotalDamageInflicted > iDefenderTotalDamageInflicted)
			{
#if defined(MOD_UNITS_MAX_HP)
				iDefenderDamageInflicted = iAttackerMaxHP - kAttacker.getDamage() - 1;
				iDefenderTotalDamageInflicted = iAttackerMaxHP - 1;
				iAttackerTotalDamageInflicted = iDefenderMaxHP;
#else
				iDefenderDamageInflicted = iMaxHP - kAttacker.getDamage() - 1;
				iDefenderTotalDamageInflicted = iMaxHP - 1;
				iAttackerTotalDamageInflicted = iMaxHP;
#endif
			}
			else
			{
#if defined(MOD_UNITS_MAX_HP)
				iAttackerDamageInflicted = iDefenderMaxHP - pkDefender->getDamage() - 1;
				iAttackerTotalDamageInflicted = iDefenderMaxHP - 1;
				iDefenderTotalDamageInflicted = iAttackerMaxHP;
#else
				iAttackerDamageInflicted = iMaxHP - pkDefender->getDamage() - 1;
				iAttackerTotalDamageInflicted = iMaxHP - 1;
				iDefenderTotalDamageInflicted = iMaxHP;
#endif
			}
		}

		iDefenderExperience = /*6*/ GC.getEXPERIENCE_DEFENDING_AIR_SWEEP_AIR();

		pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, iDefenderTotalDamageInflicted);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, iAttackerDamageInflicted);
		pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, iAttackerTotalDamageInflicted);
		pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, iDefenderDamageInflicted);

		// Fear Damage
		//pkCombatInfo->setFearDamageInflicted( BATTLE_UNIT_ATTACKER, kAttacker.getCombatDamage(iAttackerStrength, iDefenderStrength, kAttacker.getDamage(), true, false, true) );
		//	pkCombatInfo->setFearDamageInflicted( BATTLE_UNIT_DEFENDER, getCombatDamage(iDefenderStrength, iAttackerStrength, pDefender->getDamage(), true, false, true) );

#if defined(MOD_UNITS_MAX_HP)
		int iAttackerEffectiveStrength = iAttackerStrength * (iAttackerMaxHP - range(kAttacker.getDamage(), 0, iAttackerMaxHP - 1)) / iAttackerMaxHP;
#else
		int iAttackerEffectiveStrength = iAttackerStrength * (iMaxHP - range(kAttacker.getDamage(), 0, iMaxHP - 1)) / iMaxHP;
#endif
		iAttackerEffectiveStrength = iAttackerEffectiveStrength > 0 ? iAttackerEffectiveStrength : 1;
#if defined(MOD_UNITS_MAX_HP)
		int iDefenderEffectiveStrength = iDefenderStrength * (iDefenderMaxHP - range(pkDefender->getDamage(), 0, iDefenderMaxHP - 1)) / iDefenderMaxHP;
#else
		int iDefenderEffectiveStrength = iDefenderStrength * (iMaxHP - range(pkDefender->getDamage(), 0, iMaxHP - 1)) / iMaxHP;
#endif
		iDefenderEffectiveStrength = iDefenderEffectiveStrength > 0 ? iDefenderEffectiveStrength : 1;

		//int iExperience = kAttacker.defenseXPValue();
		//iExperience = ((iExperience * iAttackerEffectiveStrength) / iDefenderEffectiveStrength); // is this right? looks like more for less [Jon: Yes, it's XP for the defender]
		//iExperience = range(iExperience, GC.getMIN_EXPERIENCE_PER_COMBAT(), GC.getMAX_EXPERIENCE_PER_COMBAT());
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !kAttacker.isBarbarian());
#else
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif
		}
#else
		pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif

		//iExperience = ((iExperience * iDefenderEffectiveStrength) / iAttackerEffectiveStrength);
		//iExperience = range(iExperience, GC.getMIN_EXPERIENCE_PER_COMBAT(), GC.getMAX_EXPERIENCE_PER_COMBAT());
		int iExperience = /*6*/ GC.getEXPERIENCE_ATTACKING_AIR_SWEEP();
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
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, pkDefender->isGGFromBarbarians() || !pkDefender->isBarbarian());
#else
			pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, !pkDefender->isBarbarian());
#endif
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

	CvAssert_Debug(pkAttacker && pkDefender && pkTargetPlot);

	// Internal variables
	int iAttackerDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_ATTACKER);
	int iDefenderDamageInflicted = kCombatInfo.getDamageInflicted(BATTLE_UNIT_DEFENDER);

	// Made interception!
	if(pkDefender)
	{
		pkDefender->increaseInterceptionCount();
		if(pkAttacker && pkTargetPlot)
		{
#if !defined(NO_ACHIEVEMENTS)
			//One Hit
			if(iAttackerDamageInflicted > pkDefender->GetCurrHitPoints() && !pkDefender->IsHurt() && pkAttacker->isHuman() && !GC.getGame().isGameMultiPlayer())
				gDLL->UnlockAchievement(ACHIEVEMENT_ONEHITKILL);
#endif

			pkDefender->changeDamage(iAttackerDamageInflicted, pkAttacker->getOwner());
			pkAttacker->changeDamage(iDefenderDamageInflicted, pkDefender->getOwner());

			// Update experience for both sides.
#if defined(MOD_UNITS_XP_TIMES_100)
			pkDefender->changeExperienceTimes100(100 *
#else
			pkDefender->changeExperience(
#endif
			    kCombatInfo.getExperience(BATTLE_UNIT_DEFENDER),
			    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_DEFENDER),
			    true,
			    kCombatInfo.getInBorders(BATTLE_UNIT_DEFENDER),
			    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_DEFENDER));

#if defined(MOD_UNITS_XP_TIMES_100)
			pkAttacker->changeExperienceTimes100(100 * 
#else
			pkAttacker->changeExperience(
#endif
			    kCombatInfo.getExperience(BATTLE_UNIT_ATTACKER),
			    kCombatInfo.getMaxExperienceAllowed(BATTLE_UNIT_ATTACKER),
			    true,
			    kCombatInfo.getInBorders(BATTLE_UNIT_ATTACKER),
			    kCombatInfo.getUpdateGlobal(BATTLE_UNIT_ATTACKER));

			// Anyone eat it?
#if defined(MOD_UNITS_MAX_HP)
			bAttackerDead = (pkAttacker->getDamage() >= pkAttacker->GetMaxHitPoints());
			bDefenderDead = (pkDefender->getDamage() >= pkDefender->GetMaxHitPoints());
#else
			bAttackerDead = (pkAttacker->getDamage() >= GC.getMAX_HIT_POINTS());
			bDefenderDead = (pkDefender->getDamage() >= GC.getMAX_HIT_POINTS());
#endif

			int iActivePlayerID = GC.getGame().getActivePlayer();

			//////////////////////////////////////////////////////////////////////////
			// Attacker died
			if(bAttackerDead)
			{
				auto_ptr<ICvUnit1> pAttacker = GC.WrapUnitPointer(pkAttacker);
				gDLL->GameplayUnitDestroyedInCombat(pAttacker.get());

				if(iActivePlayerID == pkAttacker->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", pkAttacker->getNameKey(), pkDefender->getNameKey(), iAttackerDamageInflicted, 0);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				}
				if(iActivePlayerID == pkDefender->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pkDefender->getNameKey(), iAttackerDamageInflicted, 0, pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()));
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
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
				auto_ptr<ICvUnit1> pDefender = GC.WrapUnitPointer(pkDefender);
				gDLL->GameplayUnitDestroyedInCombat(pDefender.get());

				if(iActivePlayerID == pkAttacker->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey());
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				}
				if(iActivePlayerID == pkDefender->getOwner())
				{
					if(pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkDefender->getNameKey(), pkAttacker->getNameKey(), iDefenderDamageInflicted);
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkDefender->getNameKey(), pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
					}
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				}

				CvNotifications* pNotification = GET_PLAYER(pkDefender->getOwner()).GetNotifications();
				if(pNotification)
				{
					if(pkAttacker->getVisualOwner(pkDefender->getTeam()) != pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkDefender->getNameKey(), pkAttacker->getNameKey(), iDefenderDamageInflicted);
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkDefender->getNameKey(), pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkDefender->getTeam()), iDefenderDamageInflicted);
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
					strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
				}
				if(iActivePlayerID == pkDefender->getOwner())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", pkAttacker->getNameKey(), iDefenderDamageInflicted, pkDefender->getNameKey(), iAttackerDamageInflicted);
					GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
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
		pkAttacker->ClearMissionQueue(GetPostCombatDelay());

		// Spend a move for this attack
		pkAttacker->changeMoves(-GC.getMOVE_DENOMINATOR());

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

#if defined(MOD_EVENTS_BATTLES)
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
	int iInterceptionDamage = 0;

	if (pInterceptionCity != NULL)
	{
		BATTLE_JOINED(plot.getPlotCity(), BATTLE_UNIT_DEFENDER, true);
		pkCombatInfo->setCity(BATTLE_UNIT_DEFENDER, pInterceptionCity);

		// Does the attacker evade?
		if (GC.getGame().getSmallFakeRandNum(100, plot.GetPlotIndex() + kAttacker.GetID()) <= pInterceptionCity->getNukeInterceptionChance())
		{
			iInterceptionDamage = kAttacker.GetCurrHitPoints();
		}
		if (iInterceptionDamage > 0)
		{
			pkCombatInfo->setDamageInflicted(BATTLE_UNIT_INTERCEPTOR, iInterceptionDamage);		// Damage inflicted this round
			kAttacker.kill(true, pInterceptionCity->getOwner());
		}
	}
#endif

	//////////////////////////////////////////////////////////////////////

	CvString strBuffer;
	bool abTeamsAffected[MAX_TEAMS];
	int iI;
	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		abTeamsAffected[iI] = kAttacker.isNukeVictim(&plot, ((TeamTypes)iI));
	}

	int iPlotTeam = plot.getTeam();
	bool bWar = false;
	bool bBystander = false;

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(abTeamsAffected[iI])
		{
			if (!kAttacker.isEnemy((TeamTypes)iI))
			{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
				if(GET_TEAM((TeamTypes)iI).IsVassalOfSomeone())
				{
					GET_PLAYER((PlayerTypes)kAttacker.getOwner()).GetDiplomacyAI()->DeclareWar(GET_TEAM((TeamTypes)iI).GetMaster());
				}
				else
				{
					GET_PLAYER((PlayerTypes)kAttacker.getOwner()).GetDiplomacyAI()->DeclareWar((TeamTypes)iI);
				}
#else
				GET_TEAM(kAttacker.getTeam()).declareWar(((TeamTypes)iI));
#endif

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

#if defined(MOD_EVENTS_NUCLEAR_DETONATION)
	if (MOD_EVENTS_NUCLEAR_DETONATION) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_NuclearDetonation, kAttacker.getOwner(), plot.getX(), plot.getY(), bWar, bBystander);
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem) 
	{	
		CvLuaArgsHandle args;

		args->Push(kAttacker.getOwner());
		args->Push(plot.getX());
		args->Push(plot.getY());
		args->Push(bWar);
		args->Push(bBystander);

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "NuclearDetonation", args.get(), bResult);
	}
#if defined(MOD_EVENTS_NUCLEAR_DETONATION)
	}
#endif

	kAttacker.setReconPlot(&plot);

	//////////////////////////////////////////////////////////////////////

	pkCombatInfo->setFinalDamage(BATTLE_UNIT_ATTACKER, 0);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_ATTACKER, 0);	// Damage inflicted this round
	pkCombatInfo->setFinalDamage(BATTLE_UNIT_DEFENDER, 0);		// Total damage to the unit
	pkCombatInfo->setDamageInflicted(BATTLE_UNIT_DEFENDER, 0);	// Damage inflicted this round

	pkCombatInfo->setFearDamageInflicted(BATTLE_UNIT_ATTACKER, 0);

	pkCombatInfo->setExperience(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, 0);
	pkCombatInfo->setInBorders(BATTLE_UNIT_ATTACKER, plot.getOwner() != kAttacker.getOwner());	// Not really correct
#if defined(MOD_TRAITS_GG_FROM_BARBARIANS) || defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, kAttacker.isGGFromBarbarians() || !kAttacker.isBarbarian());
#else
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_ATTACKER, !kAttacker.isBarbarian());
#endif

	pkCombatInfo->setExperience(BATTLE_UNIT_DEFENDER, 0);
	pkCombatInfo->setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, 0);
	pkCombatInfo->setInBorders(BATTLE_UNIT_DEFENDER, plot.getOwner() == kAttacker.getOwner());
	pkCombatInfo->setUpdateGlobal(BATTLE_UNIT_DEFENDER, false);

	if (iInterceptionDamage > 0)
	{
		return;
	}

	pkCombatInfo->setAttackIsBombingMission(true);
	pkCombatInfo->setDefenderRetaliates(false);
	pkCombatInfo->setAttackNuclearLevel(kAttacker.GetNukeDamageLevel() + 1);

	// Set all of the units in the blast radius to defenders and calculate their damage
	int iDamageMembers = 0;
	GenerateNuclearExplosionDamage(&plot, kAttacker.GetNukeDamageLevel(), &kAttacker, pkCombatInfo->getDamageMembers(), &iDamageMembers, pkCombatInfo->getMaxDamageMemberCount());
	pkCombatInfo->setDamageMemberCount(iDamageMembers);

	GC.GetEngineUserInterface()->setDirty(UnitInfo_DIRTY_BIT, true);
}

//	-------------------------------------------------------------------------------------
uint CvUnitCombat::ApplyNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* /* pkAttacker = NULL*/)
{
	CvCombatMemberEntry kDamageMembers[MAX_NUKE_DAMAGE_MEMBERS];
	int iDamageMembers = 0;
	GenerateNuclearExplosionDamage(pkTargetPlot, iDamageLevel, NULL, &kDamageMembers[0], &iDamageMembers, MAX_NUKE_DAMAGE_MEMBERS);
	return ApplyNuclearExplosionDamage(&kDamageMembers[0], iDamageMembers, NULL, pkTargetPlot, iDamageLevel);
}

//	-------------------------------------------------------------------------------------
uint CvUnitCombat::ApplyNuclearExplosionDamage(const CvCombatMemberEntry* pkDamageArray, int iDamageMembers, CvUnit* pkAttacker, CvPlot* pkTargetPlot, int iDamageLevel)
{
	uint uiOpposingDamageCount = 0;
	PlayerTypes eAttackerOwner = pkAttacker?pkAttacker->getOwner():NO_PLAYER;

	std::vector<int> affectedPlayers;

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
				else if(kEntry.GetDamage() >= /*6*/ GC.getNUKE_NON_COMBAT_DEATH_THRESHOLD())
				{
					pkUnit->kill(false, eAttackerOwner);
				}

				for (uint j = 0; j < affectedPlayers.size(); j++)
				{
					if (affectedPlayers[j] == kEntry.GetPlayer())
						continue;

					affectedPlayers.push_back(kEntry.GetPlayer());
				}
			}
		}
	}

	// Then the terrain effects
	int iBlastRadius = GC.getNUKE_BLAST_RADIUS();

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
								if (pLoopPlot == pkTargetPlot || GC.getGame().getSmallFakeRandNum(100, *pLoopPlot) < GC.getNUKE_FALLOUT_PROB())
								{
									if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
									{
										pLoopPlot->SetImprovementPillaged(true);
									}
									pLoopPlot->setFeatureType((FeatureTypes)(GC.getNUKE_FEATURE()));
								}
							}
						}
						else
						{
							if(pLoopPlot == pkTargetPlot || GC.getGame().getSmallFakeRandNum(100, *pLoopPlot) < GC.getNUKE_FALLOUT_PROB())
							{
								if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
								{
									pLoopPlot->SetImprovementPillaged(true);
								}
								pLoopPlot->setFeatureType((FeatureTypes)(GC.getNUKE_FEATURE()));
							}
						}
#if defined(MOD_GLOBAL_NUKES_MELT_ICE)
					} else if (MOD_GLOBAL_NUKES_MELT_ICE && pLoopPlot->getFeatureType() == FEATURE_ICE) {
						if (pLoopPlot == pkTargetPlot || GC.getGame().getSmallFakeRandNum(100, *pLoopPlot) < GC.getNUKE_FALLOUT_PROB()) {
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

				if(kEntry.GetFinalDamage() >= pkCity->GetMaxHitPoints() && !pkCity->IsOriginalCapital())
				{
					auto_ptr<ICvCity1> pkDllCity(new CvDllCity(pkCity));
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
					int iBaseDamage, iRandDamage1, iRandDamage2;
					// How much destruction is unleashed on nearby Cities?
					if(iDamageLevel == 1)
					{
						iBaseDamage = /*30*/ GC.getNUKE_LEVEL1_POPULATION_DEATH_BASE();
						iRandDamage1 = GC.getGame().getSmallFakeRandNum(/*20*/ GC.getNUKE_LEVEL1_POPULATION_DEATH_RAND_1(), pkCity->getPopulation() + i);
						iRandDamage2 = GC.getGame().getSmallFakeRandNum(/*20*/ GC.getNUKE_LEVEL1_POPULATION_DEATH_RAND_2(), pkCity->GetPower() +i);
					}
					else
					{
						iBaseDamage = /*60*/ GC.getNUKE_LEVEL2_POPULATION_DEATH_BASE();
						iRandDamage1 = GC.getGame().getSmallFakeRandNum(/*20*/ GC.getNUKE_LEVEL2_POPULATION_DEATH_RAND_1(), pkCity->getPopulation() + i);
						iRandDamage2 = GC.getGame().getSmallFakeRandNum(/*20*/ GC.getNUKE_LEVEL2_POPULATION_DEATH_RAND_2(), pkCity->GetPower() + i);
					}

					int iNukedPopulation = pkCity->getPopulation() * (iBaseDamage + iRandDamage1 + iRandDamage2) / 100;

					iNukedPopulation *= std::max(0, (pkCity->getNukeModifier() + 100));
					iNukedPopulation /= 100;

					pkCity->changePopulation(-(std::min((pkCity->getPopulation() - 1), iNukedPopulation)));

					// Add damage to the city
					pkCity->setDamage(kEntry.GetFinalDamage());

					for (uint j = 0; j < affectedPlayers.size(); j++)
					{
						if (affectedPlayers[j] == pkCity->getOwner())
							continue;

						affectedPlayers.push_back(pkCity->getOwner());
					}
				}
			}
		}
	}

	for (uint j = 0; j < affectedPlayers.size(); j++)
	{
		if ((PlayerTypes)affectedPlayers[j] == NO_PLAYER)
			continue;

		GET_PLAYER((PlayerTypes)affectedPlayers[j]).GetDiplomacyAI()->ChangeNumTimesNuked(pkAttacker->getOwner(), 1);
	}

	return uiOpposingDamageCount;
}

//	-------------------------------------------------------------------------------------
//	Generate nuclear explosion damage for all the units and cities in the radius of the specified plot.
//	The attacker is optional, this is also called for a meltdown
void CvUnitCombat::GenerateNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* pkAttacker, CvCombatMemberEntry* pkDamageArray, int* piDamageMembers, int iMaxDamageMembers)
{
	int iBlastRadius = GC.getNUKE_BLAST_RADIUS();

#if defined(MOD_EVENTS_BATTLES)
	CvCity* pDefenderCity = NULL;
	CvUnit* pDefenderUnit = NULL;

	if (pkTargetPlot->isCity())
	{
		pDefenderCity = pkTargetPlot->getPlotCity();
	}
	else
	{
		pDefenderUnit = pkTargetPlot->getBestDefender(NO_PLAYER, pkAttacker->getOwner());
	}
#endif

	*piDamageMembers = 0;

	for(int iDX = -(iBlastRadius); iDX <= iBlastRadius; iDX++)
	{
		for(int iDY = -(iBlastRadius); iDY <= iBlastRadius; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pkTargetPlot->getX(), pkTargetPlot->getY(), iDX, iDY, iBlastRadius);

			if(pLoopPlot != NULL)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();

				FFastSmallFixedList<IDInfo, 25, true, c_eCiv5GameplayDLL > oldUnits;
				IDInfo* pUnitNode = pLoopPlot->headUnitNode();

				while(pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while(pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(*pUnitNode);
					pUnitNode = oldUnits.next(pUnitNode);

					if(pLoopUnit != NULL)
					{
						if(pLoopUnit != pkAttacker)
						{
							if(!pLoopUnit->isNukeImmune() && !pLoopUnit->isDelayedDeath())
							{
								int iNukeDamage;
								// How much destruction is unleashed on nearby Units?
								if(iDamageLevel == 1 && pLoopPlot != pkTargetPlot)	// Nuke level 1, but NOT the plot that got hit directly (units there are killed)
								{
									iNukeDamage = (/*3*/ GC.getNUKE_UNIT_DAMAGE_BASE() + /*4*/ GC.getGame().getSmallFakeRandNum(GC.getNUKE_UNIT_DAMAGE_RAND_1(), *pLoopPlot) + /*4*/ GC.getGame().getSmallFakeRandNum(GC.getNUKE_UNIT_DAMAGE_RAND_2(), pLoopUnit->GetID() + iDX + iDY));
								}
								// Wipe everything out
								else
								{
#if defined(MOD_UNITS_MAX_HP)
									iNukeDamage = pLoopUnit->GetMaxHitPoints();
#else
									iNukeDamage = GC.getMAX_HIT_POINTS();
#endif
								}

								if(pLoopCity != NULL)
								{
									iNukeDamage *= std::max(0, (pLoopCity->getNukeModifier() + 100));
									iNukeDamage /= 100;
								}

								CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkDamageArray, piDamageMembers, iMaxDamageMembers, pLoopUnit);
								if(pkDamageEntry)
								{
#if defined(MOD_EVENTS_BATTLES)
									if (pLoopUnit != pDefenderUnit)
									{
										BATTLE_JOINED(pLoopUnit, BATTLE_UNIT_COUNT, false); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER
									}
#endif
									pkDamageEntry->SetDamage(iNukeDamage);
#if defined(MOD_UNITS_MAX_HP)
									pkDamageEntry->SetFinalDamage(std::min(iNukeDamage + pLoopUnit->getDamage(), pLoopUnit->GetMaxHitPoints()));
									pkDamageEntry->SetMaxHitPoints(pLoopUnit->GetMaxHitPoints());
#else
									pkDamageEntry->SetFinalDamage(std::min(iNukeDamage + pLoopUnit->getDamage(), GC.getMAX_HIT_POINTS()));
									pkDamageEntry->SetMaxHitPoints(GC.getMAX_HIT_POINTS());
#endif
									if(pkAttacker)
										pLoopUnit->setCombatUnit(pkAttacker);
								}
								else
								{
									CvAssertMsg(*piDamageMembers < iMaxDamageMembers, "Ran out of entries for the nuclear damage array");
								}
							}
						}
					}
				}

				if(pLoopCity != NULL)
				{
					bool bKillCity = false;

					// Is the city wiped out? - no capitals!
					if(!pLoopCity->IsOriginalCapital())
					{
						if(iDamageLevel > 2)
						{
							bKillCity = true;
						}
						else if(iDamageLevel > 1)
						{
							if(pLoopCity->getPopulation() < /*5*/ GC.getNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD())
							{
								bKillCity = true;
							}
						}
					}

					int iTotalDamage;
					if(bKillCity)
					{
						iTotalDamage = pLoopCity->GetMaxHitPoints();
					}
					else
					{
						// Add damage to the city
						iTotalDamage = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) * /*50*/ GC.getNUKE_CITY_HIT_POINT_DAMAGE();
						iTotalDamage /= 100;

						iTotalDamage += pLoopCity->getDamage();

						// Can't bring a city below 1 HP
						iTotalDamage = min(iTotalDamage, pLoopCity->GetMaxHitPoints() - 1);
					}

					CvCombatMemberEntry* pkDamageEntry = AddCombatMember(pkDamageArray, piDamageMembers, iMaxDamageMembers, pLoopCity);
					if(pkDamageEntry)
					{
#if defined(MOD_EVENTS_BATTLES)
						if (pLoopCity != pDefenderCity)
						{
							BATTLE_JOINED(pLoopCity, BATTLE_UNIT_COUNT, true); // Bit of a fudge, as BATTLE_UNIT_COUNT happens to correspond to BATTLEUNIT_BYSTANDER
						}
#endif
						pkDamageEntry->SetDamage(iTotalDamage - pLoopCity->getDamage());
						pkDamageEntry->SetFinalDamage(iTotalDamage);
						pkDamageEntry->SetMaxHitPoints(pLoopCity->GetMaxHitPoints());

						if(pkAttacker)
							pLoopCity->setCombatUnit(pkAttacker);
					}
					else
					{
						CvAssertMsg(*piDamageMembers < iMaxDamageMembers, "Ran out of entries for the nuclear damage array");
					}
				}
			}
		}
	}
}

//	---------------------------------------------------------------------------
//	Function: ResolveNuclearCombat
//	Resolve combat from a nuclear attack.
void CvUnitCombat::ResolveNuclearCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID)
{
	UNREFERENCED_PARAMETER(uiParentEventID);

	CvUnit* pkAttacker = kCombatInfo.getUnit(BATTLE_UNIT_ATTACKER);
	CvAssert_Debug(pkAttacker);

	CvPlot* pkTargetPlot = kCombatInfo.getPlot();
	CvAssert_Debug(pkTargetPlot);

	CvString strBuffer;

	GC.getGame().changeNukesExploded(1);

	if(pkAttacker && !pkAttacker->isDelayedDeath())
	{
		// Make sure we are disconnected from any unit transporting the attacker (i.e. its a missile)
		pkAttacker->setTransportUnit(NULL);

		if(pkTargetPlot)
		{
			if(ApplyNuclearExplosionDamage(kCombatInfo.getDamageMembers(), kCombatInfo.getDamageMemberCount(), pkAttacker, pkTargetPlot, kCombatInfo.getAttackNuclearLevel() - 1) > 0)
			{
#if !defined(NO_ACHIEVEMENTS)
				if(pkAttacker->getOwner() == GC.getGame().getActivePlayer())
				{
					// Must damage someone to get the achievement.
					gDLL->UnlockAchievement(ACHIEVEMENT_DROP_NUKE);

					if(GC.getGame().getGameTurnYear() == 2012)
					{
						CvPlayerAI& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());
						if(strncmp(kPlayer.getCivilizationTypeKey(), "CIVILIZATION_MAYA", 32) == 0)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_36);
						}

					}

				}
#endif

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
			CvAssertMsg(pkAttacker->isSuicide(), "A nuke unit that is not a one time use?");

			// Clean up some stuff
			pkAttacker->setCombatUnit(NULL);
			pkAttacker->ClearMissionQueue(GetPostCombatDelay());
			pkAttacker->SetAutomateType(NO_AUTOMATE); // kick unit out of automation

			// Spend a move for this attack
			pkAttacker->changeMoves(-GC.getMOVE_DENOMINATOR());

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
	CvAssertMsg(!paraUnit.isDelayedDeath(), "Trying to paradrop and the unit is already dead!");
	CvAssert(paraUnit.getCombatTimer() == 0);

	// Any interception to be done?
	CvUnit* pInterceptor = dropPlot.GetBestInterceptor(paraUnit.getOwner(), &paraUnit);
	if (pInterceptor) {
		uint uiParentEventID = 0;
		int iInterceptionDamage = 0;

		// Is the interception successful?
		if(GC.getGame().getSmallFakeRandNum(100, dropPlot.GetPlotIndex()+paraUnit.GetID()+paraUnit.getDamage()) < pInterceptor->interceptionProbability())
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
				// auto_ptr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&pDropPlot);
				// GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
				// kCombatInfo.setVisualizeCombat(true);

				// auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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

				GC.GetEngineUserInterface()->AddMessage(uiParentEventID, paraUnit.getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
			}
			
			BATTLE_FINISHED();
		}
	}
	
	return paraUnit.IsDead();
}
#endif

//result is times 100
int CvUnitCombat::DoDamageMath(int iAttackerStrength100, int iDefenderStrength100, int iDefaultDamage100, int iMaxRandomDamage100, int iRandomSeed, int iModifierPercent)
{
	// Base damage for two units of identical strength
	int iDamage = iDefaultDamage100;

	// Don't use rand when calculating projected combat results
	if(iRandomSeed>0)
	{
		iDamage += GC.getGame().getSmallFakeRandNum(iMaxRandomDamage100, iRandomSeed);
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
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pAttacker));
		gDLL->GameplayUnitVisibility(pDllUnit.get(), !pAttacker->isInvisible(eActiveTeam, false));
	}

	CvUnit* pDefender = kInfo.getUnit(BATTLE_UNIT_DEFENDER);
	if(pDefender)
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pDefender));
		gDLL->GameplayUnitVisibility(pDllUnit.get(), !pDefender->isInvisible(eActiveTeam, false));
	}

	CvUnit* pDefenderSupport = kInfo.getUnit(BATTLE_UNIT_INTERCEPTOR);
	if(pDefenderSupport)
	{
		auto_ptr<ICvUnit1> pDllUnit(new CvDllUnit(pDefenderSupport));
		gDLL->GameplayUnitVisibility(pDllUnit.get(), !pDefenderSupport->isInvisible(eActiveTeam, false));
	}
#if defined(MOD_EVENTS_RED_COMBAT_ABORT) || defined(MOD_EVENTS_RED_COMBAT_RESULT)
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

#if defined(MOD_UNITS_MAX_HP)
			// Generic values for max hit points, changed below when we have the specific units available
#endif
			int attackerMaxHP = GC.getMAX_HIT_POINTS();
			if (pAttacker)
				attackerMaxHP = pAttacker->GetMaxHitPoints();
			int defenderMaxHP = GC.getMAX_HIT_POINTS();
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
#if defined(MOD_UNITS_MAX_HP)
				attackerMaxHP = pAttacker->GetMaxHitPoints();
				// Generic values for max hit points, changed below when we have the specific units available
#endif
			}
			if (pkDefender)
			{
				iDefendingPlayer = pkDefender->getOwner();
				iDefendingUnit = pkDefender->GetID();
#if defined(MOD_UNITS_MAX_HP)
				defenderMaxHP = pkDefender->GetMaxHitPoints();
				// Generic values for max hit points, changed below when we have the specific units available
#endif
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
				if(bResult == true)
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
#endif
	
#if defined(MOD_EVENTS_RED_COMBAT_ABORT)
	if (!MOD_EVENTS_RED_COMBAT_ABORT || bCanAttack)
	{
#endif
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

#if defined(MOD_EVENTS_RED_COMBAT_ENDED)
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

#if defined(MOD_UNITS_MAX_HP)
				// Generic values for max hit points, changed below when we have the specific units available
#endif
				int attackerMaxHP = GC.getMAX_HIT_POINTS();
				if (pAttacker)
					attackerMaxHP = pAttacker->GetMaxHitPoints();
				int defenderMaxHP = GC.getMAX_HIT_POINTS();
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
#if defined(MOD_UNITS_MAX_HP)
					attackerMaxHP = pAttacker->GetMaxHitPoints();
#endif
				}
				if (pkDefender)
				{
					iDefendingPlayer = pkDefender->getOwner();
					iDefendingUnit = pkDefender->GetID();
#if defined(MOD_UNITS_MAX_HP)
					defenderMaxHP = pkDefender->GetMaxHitPoints();
#endif
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

				bool bResult;
				LuaSupport::CallHook(pkScriptSystem, "CombatEnded", args.get(), bResult);
			}
		}
#endif

#if defined(MOD_EVENTS_RED_COMBAT_ABORT)
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
#endif

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

	//VALIDATE_OBJECT
	CvAssert(kAttacker.canMoveInto(targetPlot, CvUnit::MOVEFLAG_ATTACK ));
	CvAssert(kAttacker.getCombatTimer() == 0);

	CvUnitCombat::ATTACK_RESULT eResult = CvUnitCombat::ATTACK_ABORTED;

	CvAssert(kAttacker.getCombatTimer() == 0);
	//	CvAssert(pDefender != NULL);
	CvAssert(!kAttacker.isFighting());

	CvUnit* pDefender = targetPlot.getBestDefender(NO_PLAYER, kAttacker.getOwner(), &kAttacker, true);
	if(!pDefender)
	{
		return eResult;
	}

	kAttacker.SetAutomateType(NO_AUTOMATE);
	pDefender->SetAutomateType(NO_AUTOMATE);
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
	// We want to "wake up" a unit that is being attacked by a hidden enemy (probably being bombed, indirect naval fire,
	// or sneaky long-range archers) so the player can consider what to do with them
	if (MOD_BUGFIX_UNITS_AWAKE_IN_DANGER) {
		pDefender->SetActivityType(ACTIVITY_AWAKE); 
	}
#endif

#if !defined(NO_TUTORIALS)
	// slewis - tutorial'd
	if(kAttacker.getOwner() == GC.getGame().getActivePlayer())
	{
		GC.getGame().SetEverAttackedTutorial(true);
	}
	// end tutorial'd
#endif

	// handle the Zulu special thrown spear first attack
	ATTACK_RESULT eFireSupportResult = ATTACK_ABORTED;
	if (kAttacker.isRangedSupportFire() && pDefender->IsCanDefend())
	{
		eFireSupportResult = AttackRanged(kAttacker, pDefender->getX(), pDefender->getY(), CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT);
		if (pDefender->isDelayedDeath())
		{
			if (kAttacker.plot()->MeleeAttackerAdvances())
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

	CvAssertMsg(!kAttacker.isDelayedDeath() && !pDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");

	if(pDefender->getExtraWithdrawal() > 0 && pDefender->CanFallBack(kAttacker,true))
	{
		pDefender->DoFallBack(kAttacker);

		if(kAttacker.getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_ENEMY_UNIT_WITHDREW", pDefender->getNameKey());
			GC.GetEngineUserInterface()->AddMessage(0, kAttacker.getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
		}
		else if(pDefender->getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_FRIENDLY_UNIT_WITHDREW", pDefender->getNameKey());
			GC.GetEngineUserInterface()->AddMessage(0, pDefender->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer);
		}

		// Move forward
		if(targetPlot.getNumVisibleEnemyDefenders(&kAttacker) == 0)
		{
			kAttacker.UnitMove(&targetPlot, true, &kAttacker);
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
			CvAssert(false);
		}
		else
		{
			kAttacker.setCombatTimer(pkSurrenderMission->getTime());
		}

		// Kill them!
#if defined(MOD_UNITS_MAX_HP)
		pDefender->setDamage(pDefender->GetMaxHitPoints());
#else
		pDefender->setDamage(GC.getMAX_HIT_POINTS());
#endif

		Localization::String strMessage;
		Localization::String strSummary;

		// Some units can't capture civilians. Embarked units are also not captured, they're simply killed. And some aren't a type that gets captured.
		if(!kAttacker.isNoCapture() && (!pDefender->isEmbarked() || pDefender->getUnitInfo().IsCaptureWhileEmbarked()) && pDefender->getCaptureUnitType(GET_PLAYER(pDefender->getOwner()).getCivilizationType()) != NO_UNIT)
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

		bool bAdvance;
		bAdvance = kAttacker.canAdvance(targetPlot, ((pDefender->IsCanDefend()) ? 1 : 0));

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
				CvAssertMsg(!pFireSupportUnit->isDelayedDeath(), "Supporting battle unit is already dead!");
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
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible(false);
			bool quickCombat = CvPreGame::quickCombat();
			if(!quickCombat)
			{
				// Center camera here!
				if(isTargetVisibleToActivePlayer)
				{
					auto_ptr<ICvPlot1> pDefenderPlot = GC.WrapPlotPointer(pDefender->plot());
					GC.GetEngineUserInterface()->lookAt(pDefenderPlot.get(), CAMERALOOKAT_NORMAL);
				}
				kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

				auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
	//VALIDATE_OBJECT
	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	ATTACK_RESULT eResult = ATTACK_ABORTED;

	CvAssertMsg(kAttacker.getDomainType() != DOMAIN_AIR, "Air units should not use AttackRanged, they should just MoveTo the target");

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
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
		if (MOD_BUGFIX_UNITS_AWAKE_IN_DANGER) {
			pDefender->SetActivityType(ACTIVITY_AWAKE); 
		}
#endif

		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateRangedCombatInfo(kAttacker, pDefender, *pPlot, &kCombatInfo);
		CvAssertMsg(!kAttacker.isDelayedDeath() && !pDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");
		
		if (!kAttacker.isRangedSupportFire())
		{
			kAttacker.setMadeAttack(true);
		}
		kAttacker.changeMoves(-GC.getMOVE_DENOMINATOR());

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible(false);
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
		CvAssertMsg(!kAttacker.isDelayedDeath(), "Trying to battle and the attacker is already dead!");
		kAttacker.setMadeAttack(true);
		kAttacker.changeMoves(-GC.getMOVE_DENOMINATOR());

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible(false);
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}

			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
	//VALIDATE_OBJECT
	CvAssert(kAttacker.getCombatTimer() == 0);

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
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
		if (MOD_BUGFIX_UNITS_AWAKE_IN_DANGER) {
			pDefender->SetActivityType(ACTIVITY_AWAKE); 
		}
#endif

		CvCombatInfo kCombatInfo;
		CvUnitCombat::GenerateAirCombatInfo(kAttacker, pDefender, targetPlot, &kCombatInfo);
		CvAssertMsg(!kAttacker.isDelayedDeath() && !pDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");
		kAttacker.setMadeAttack(true);

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible(false);
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&targetPlot);
				GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
		CvAssertMsg(!kAttacker.isDelayedDeath(), "Trying to battle and the attacker is already dead!");
		kAttacker.setMadeAttack(true);

		uint uiParentEventID = 0;
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible(false);
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&targetPlot);
				GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
			}

			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);
			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
	//VALIDATE_OBJECT
	CvAssert(kAttacker.getCombatTimer() == 0);

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
#if defined(MOD_BUGFIX_UNITS_AWAKE_IN_DANGER)
		if (MOD_BUGFIX_UNITS_AWAKE_IN_DANGER) {
			pkDefender->SetActivityType(ACTIVITY_AWAKE); 
		}
#endif
		CvAssertMsg(!kAttacker.isDelayedDeath() && !pkDefender->isDelayedDeath(), "Trying to battle and one of the units is already dead!");

		uint uiParentEventID = 0;
		bool bDoImmediate = CvPreGame::quickCombat();
		if(!bDoImmediate)
		{
			// Center camera here!
			bool isTargetVisibleToActivePlayer = targetPlot.isActiveVisible(false);
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllTargetPlot = GC.WrapPlotPointer(&targetPlot);
				GC.GetEngineUserInterface()->lookAt(pDllTargetPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
		bool bNothing = false;
		if (MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			bNothing = kAttacker.attemptGroundAttacks(targetPlot);

		if (bNothing)
		{
			// attempted to do a sweep in a plot that had no interceptors
			// consume the movement and finish its moves
			if (kAttacker.getOwner() == GC.getGame().getActivePlayer())
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_AIR_PATROL_BOMBED_GROUND_TARGETS");
				localizedText << kAttacker.getUnitInfo().GetTextKey();
				GC.GetEngineUserInterface()->AddMessage(0, kAttacker.getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());
			}
		}
		else
		{
			// attempted to do a sweep in a plot that had no interceptors
			// consume the movement and finish its moves
			if (kAttacker.getOwner() == GC.getGame().getActivePlayer())
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_AIR_PATROL_FOUND_NOTHING");
				localizedText << kAttacker.getUnitInfo().GetTextKey();
				GC.GetEngineUserInterface()->AddMessage(0, kAttacker.getOwner(), false, GC.getEVENT_MESSAGE_TIME(), localizedText.toUTF8());
			}
		}

		// Spend a move for this attack
		kAttacker.changeMoves(-GC.getMOVE_DENOMINATOR());

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
	//VALIDATE_OBJECT

	ATTACK_RESULT eResult = ATTACK_ABORTED;
	CvCity* pCity = plot.getPlotCity();
	CvAssertMsg(pCity != NULL, "If this unit is attacking a NULL city then something funky is goin' down");
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
		CvAssertMsg(!kAttacker.isDelayedDeath(), "Trying to battle and the attacker is already dead!");
		kAttacker.setMadeAttack(true);

		// Send the combat message if the target plot is visible.
		bool isTargetVisibleToActivePlayer = plot.isActiveVisible(false);

		uint uiParentEventID = 0;
		bool bDoImmediate = CvPreGame::quickCombat();
		if(!bDoImmediate)
		{
			// Center camera here!
			if(isTargetVisibleToActivePlayer)
			{
				auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(&plot);
				GC.GetEngineUserInterface()->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
			}
			kCombatInfo.setVisualizeCombat(isTargetVisibleToActivePlayer);

			auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
	VALIDATE_OBJECT

	if(GC.getFIRE_SUPPORT_DISABLED() == 1)
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

		bool isTargetVisibleToActivePlayer = pPlot->isActiveVisible(false);
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
			auto_ptr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
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

		auto_ptr<ICvCombatInfo1> pDllCombatInfo(new CvDllCombatInfo(&kCombatInfo));
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
	// Clear cached danger in the vicinity
	GET_PLAYER(pkWinner->getOwner()).ResetDangerCache(*pkLoser->plot(),3);

	// "Heal if defeat enemy" promotion; doesn't apply if defeat a barbarian
	if(pkWinner->getHPHealedIfDefeatEnemy() > 0 && (pkLoser->getOwner() != BARBARIAN_PLAYER || !(pkWinner->IsHealIfDefeatExcludeBarbarians())))
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
		int iHealAmount = min(pkWinner->getDamage(), GC.getPILLAGE_HEAL_AMOUNT());
		pkWinner->changeMoves(GC.getMOVE_DENOMINATOR());
		pkWinner->setMadeAttack(false);
		pkWinner->changeDamage(-iHealAmount);
	}
#endif
#if defined(MOD_BUGFIX_MINOR)
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
#endif

	CvPlayer& kPlayer = GET_PLAYER(pkWinner->getOwner());
	if (pkWinner->GetGoldenAgeValueFromKills() > 0)
	{
#if defined(MOD_API_EXTENSIONS)
		int iCombatStrength = max(pkLoser->GetBaseCombatStrength(), pkLoser->GetBaseRangedCombatStrength());
#else
		int iCombatStrength = max(pkLoser->getUnitInfo().GetCombat(), pkLoser->getUnitInfo().GetRangedCombat());
#endif
		if(iCombatStrength > 0)
		{
			int iValue = iCombatStrength * pkWinner->GetGoldenAgeValueFromKills() / 100;
			kPlayer.ChangeGoldenAgeProgressMeter(iValue);

#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
			CvYieldInfo* pYieldInfo = GC.getYieldInfo(YIELD_GOLDEN_AGE_POINTS);
			CvString yieldString;
			yieldString.Format("%s+%%d[ENDCOLOR]%s", pYieldInfo->getColorString(), pYieldInfo->getIconString());
#else
			CvString yieldString = "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GOLDEN_AGE]";
#endif

			if(pkWinner->getOwner() == GC.getGame().getActivePlayer())
			{
				char text[256] = {0};
				sprintf_s(text, yieldString, iValue);
				SHOW_PLOT_POPUP(pkLoser->plot(), pkWinner->getOwner(), text);
			}
		}
	}

	// Earn bonuses for kills?
	kPlayer.DoYieldsFromKill(pkWinner, pkLoser);
#if defined(MOD_BALANCE_CORE)
	if(pkLoser->getOwner() == BARBARIAN_PLAYER && pkLoser->plot()->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
	{
		GET_PLAYER(pkWinner->getOwner()).GetPlayerTraits()->SetDefeatedBarbarianCampGuardType(pkLoser->getUnitType());
	}
#endif

#if !defined(NO_ACHIEVEMENTS)
	//Achievements and Stats
	if(pkWinner->isHuman() && !GC.getGame().isGameMultiPlayer())
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
#endif
}

void CvUnitCombat::ApplyPostCityCombatEffects(CvUnit* pkAttacker, CvCity* pkDefender, int iAttackerDamageInflicted)
{
	CvString colorString;
	int iPlunderModifier = pkAttacker->GetCityAttackPlunderModifier();
	if(iPlunderModifier > 0)
	{
		int iGoldPlundered = iAttackerDamageInflicted * iPlunderModifier;
		iGoldPlundered /= 100;

		if(iGoldPlundered > 0)
		{
#if defined(MOD_BALANCE_CORE)
			iGoldPlundered *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
			iGoldPlundered /= 100;

			if(iGoldPlundered > (pkDefender->getStrengthValue()/100) * 10)
			{
				iGoldPlundered = ((pkDefender->getStrengthValue()/100) * 10);
			}
#endif
			GET_PLAYER(pkAttacker->getOwner()).GetTreasury()->ChangeGold(iGoldPlundered);

			CvPlayer& kCityPlayer = GET_PLAYER(pkDefender->getOwner());
			int iDeduction = min(iGoldPlundered, kCityPlayer.GetTreasury()->GetGold());
			kCityPlayer.GetTreasury()->ChangeGold(-iDeduction);

			if(pkAttacker->getOwner() == GC.getGame().getActivePlayer())
			{
				char text[256] = {0};
				colorString = "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]";
				sprintf_s(text, colorString, iGoldPlundered);
				SHOW_PLOT_POPUP(pkAttacker->plot(), pkAttacker->getOwner(), text);
			}
		}
	}
	pkDefender->ChangeNumTimesAttackedThisTurn(pkAttacker->getOwner(), 1);
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

					auto_ptr<ICvUnit1> pDefender = GC.WrapUnitPointer(pkUnit);
					gDLL->GameplayUnitDestroyedInCombat(pDefender.get());

					if(iActivePlayerID == pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", pkAttacker->getNameKey(), kEntry.GetDamage(), pkUnit->getNameKey());
						GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkAttacker->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
					}

					if(pkAttacker->getVisualOwner(pkUnit->getTeam()) != pkAttacker->getOwner())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pkUnit->getNameKey(), pkAttacker->getNameKey(), kEntry.GetDamage());
					}
					else
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pkUnit->getNameKey(), pkAttacker->getNameKey(), pkAttacker->getVisualCivAdjective(pkUnit->getTeam()), kEntry.GetDamage());
					}
					if(iActivePlayerID == pkUnit->getOwner())
					{
						GC.GetEngineUserInterface()->AddMessage(uiParentEventID, pkUnit->getOwner(), true, GC.getEVENT_MESSAGE_TIME(), strBuffer/*,GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
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

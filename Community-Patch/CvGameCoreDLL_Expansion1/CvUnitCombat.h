/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#ifndef CVUNITCOMBAT_H
#define CVUNITCOMBAT_H

#pragma once

#include "CvUnit.h"
#include "CvPlot.h"

// Combat controller for CvUnits
class CvUnitCombat
{
public:
	// Results from requesting an attack
	enum ATTACK_RESULT
	{
	    ATTACK_ABORTED		= 0,			// It was determined that the unit cannot make the attack, no attack was issued.
	    ATTACK_COMPLETED	= 1,			// The unit attacked and resolved the combat immediately
	    ATTACK_QUEUED		= 2				// The unit has queued the attack.  This may have occurred because the defender is busy or the attack is going to be visualized
	};

	enum ATTACK_OPTION
	{
	    ATTACK_OPTION_NONE	= 0,
	    ATTACK_OPTION_NO_DEFENSIVE_SUPPORT = 1
	};

	static void GenerateMeleeCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& kPlot, CvCombatInfo* pkCombatInfo);
	static void GenerateRangedCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& kPlot, CvCombatInfo* pkCombatInfo);
	static void GenerateRangedCombatInfo(CvCity& kAttacker, CvUnit* pkDefender, CvPlot& plot, CvCombatInfo* pkCombatInfo);
	static void GenerateAirCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& kPlot, CvCombatInfo* pkCombatInfo);
	static void GenerateAirSweepCombatInfo(CvUnit& kAttacker, CvUnit* pkDefender, CvPlot& kPlot, CvCombatInfo* pkCombatInfo);
	static void GenerateNuclearCombatInfo(CvUnit& kAttacker, CvPlot& plot, CvCombatInfo* pkCombatInfo);

	static void ResolveCombat(const CvCombatInfo& kInfo, uint uiParentEventID = 0);

	static ATTACK_RESULT Attack(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackRanged(CvUnit& kAttacker, int iX, int iY, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackAir(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackAirSweep(CvUnit& kAttacker, CvPlot& targetPlot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackCity(CvUnit& kAttacker, CvPlot& plot, ATTACK_OPTION eOption);
	static ATTACK_RESULT AttackNuclear(CvUnit& kAttacker, int iX, int iY, ATTACK_OPTION eOption);

	//	Return a ranged unit that will defend the supplied location against the attacker at the specified location.
	static CvUnit*		GetFireSupportUnit(PlayerTypes eDefender, int iDefendX, int iDefendY, int iAttackX, int iAttackY);
	static uint			ApplyNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* pkAttacker = NULL);

protected:
	static void ResolveRangedUnitVsCombat(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveRangedCityVsUnitCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);
	static void ResolveMeleeCombat(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveCityMeleeCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);
	static void ResolveAirUnitVsCombat(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveAirSweep(const CvCombatInfo& kInfo, uint uiParentEventID);
	static void ResolveNuclearCombat(const CvCombatInfo& kCombatInfo, uint uiParentEventID);

	static void GenerateNuclearExplosionDamage(CvPlot* pkTargetPlot, int iDamageLevel, CvUnit* pkAttacker, CvCombatMemberEntry* pkDamageArray, int* piDamageMembers, int iMaxDamageMembers);
	static uint ApplyNuclearExplosionDamage(const CvCombatMemberEntry* pkDamageArray, int iDamageMembers, CvUnit* pkAttacker, CvPlot* pkTargetPlot, int iDamageLevel);

	static void ApplyPostCombatTraitEffects(CvUnit* pkWinner, CvUnit* pkLoser);
	static void ApplyPostCityCombatEffects(CvUnit* pkAttacker, CvCity* pkDefender, int iAttackerDamageInflicted);
};

#endif // CVUNITCOMBAT_H
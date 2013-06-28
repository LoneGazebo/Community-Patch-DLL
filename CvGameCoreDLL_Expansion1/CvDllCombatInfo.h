/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllCombatInfo : public ICvCombatInfo1
{
public:
	CvDllCombatInfo(_In_ CvCombatInfo* pCombatInfo);
	~CvDllCombatInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	ICvUnit1* DLLCALL GetUnit(BattleUnitTypes unitType) const;
	ICvCity1* DLLCALL GetCity(BattleUnitTypes unitType) const;
	ICvPlot1* DLLCALL GetPlot() const;

	bool DLLCALL GetAttackerAdvances() const;
	bool DLLCALL GetVisualizeCombat() const;
	bool DLLCALL GetDefenderRetaliates() const;
	int	 DLLCALL GetNuclearDamageLevel() const;
	bool DLLCALL GetAttackIsRanged() const;
	bool DLLCALL GetAttackIsBombingMission() const;
	bool DLLCALL GetAttackIsAirSweep() const;
	bool DLLCALL GetDefenderCaptured() const;

	int DLLCALL GetDamageInflicted(BattleUnitTypes unitType) const;
	int DLLCALL GetFinalDamage(BattleUnitTypes unitType) const;

	const CvCombatMemberEntry* DLLCALL GetDamageMember(int iIndex) const;
	int DLLCALL GetDamageMemberCount() const;
	int DLLCALL GetMaxDamageMemberCount() const;

	void* DLLCALL TEMPGetRawCombatInfo() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvCombatInfo* m_pCombatInfo;
};
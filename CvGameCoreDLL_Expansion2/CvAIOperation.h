/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// CvAIOperation.h

#ifndef CIV5_OPERATION_H
#define CIV5_OPERATION_H

#include "CvGlobals.h"
#include "CvGame.h"
#include "CvWeightedVector.h"

class CvArmyAI;

// it is important to add a new entry whenever a new (subclassed) operation is added to the list
enum AIOperationTypes
{
    INVALID_AI_OPERATION = -1,
    AI_OPERATION_CITY_CLOSE_DEFENSE,
    AI_OPERATION_RAPID_RESPONSE,
    AI_OPERATION_BASIC_CITY_ATTACK,
    AI_OPERATION_DESTROY_BARBARIAN_CAMP,
    AI_OPERATION_FOUND_CITY,
    AI_OPERATION_PILLAGE_ENEMY,
    AI_OPERATION_SNEAK_CITY_ATTACK,
    AI_OPERATION_SMALL_CITY_ATTACK,
    AI_OPERATION_MERCHANT_DELEGATION,
    AI_OPERATION_NAVAL_BOMBARDMENT,
    AI_OPERATION_NAVAL_SUPERIORITY,
    AI_OPERATION_NAVAL_COLONIZATION,
    AI_OPERATION_QUICK_COLONIZE,
    AI_OPERATION_NAVAL_ATTACK,
    AI_OPERATION_NAVAL_SNEAK_ATTACK,
    AI_OPERATION_CITY_STATE_ATTACK,
    AI_OPERATION_CITY_STATE_NAVAL_ATTACK,
    AI_OPERATION_NUKE_ATTACK,
	AI_OPERATION_PURE_NAVAL_CITY_ATTACK,
	AI_OPERATION_CONCERT_TOUR,
#if defined(MOD_DIPLOMACY_CITYSTATES)
	AI_OPERATION_DIPLOMAT_DELEGATION,
	AI_OPERATION_ALLY_DEFENSE,
#endif
#if defined(MOD_BALANCE_CORE)
	AI_OPERATION_CITY_CLOSE_DEFENSE_PEACE,
#endif
    NUM_AI_OPERATIONS,
};

enum AIOperationState
{
    INVALID_AI_OPERATION_STATE = -1,
    AI_OPERATION_STATE_ABORTED,
    AI_OPERATION_STATE_RECRUITING_UNITS,
    AI_OPERATION_STATE_GATHERING_FORCES,
    AI_OPERATION_STATE_MOVING_TO_TARGET,
    AI_OPERATION_STATE_AT_TARGET,
    AI_OPERATION_STATE_SUCCESSFUL_FINISH,
};

enum AIOperationMovementType
{
    INVALID_AI_OPERATION_MOVE_TYPE = -1,
    AI_OPERATION_MOVETYPE_ESCORT,	//try to move somewhere while avoiding combat
    AI_OPERATION_MOVETYPE_COMBAT,	//try to move somewhere while expecting combat
    AI_OPERATION_MOVETYPE_ROAMING,	//no target, but expect combat
};

enum AIOperationAbortReason
{
    NO_ABORT_REASON = -1,
    AI_ABORT_SUCCESS,
    AI_ABORT_NO_TARGET,
    AI_ABORT_REPEAT_TARGET,
    AI_ABORT_LOST_TARGET,
    AI_ABORT_TARGET_ALREADY_CAPTURED,
    AI_ABORT_NO_ROOM_DEPLOY,
    AI_ABORT_HALF_STRENGTH,
    AI_ABORT_NO_MUSTER,
    AI_ABORT_LOST_CIVILIAN,
    AI_ABORT_ESCORT_DIED,
    AI_ABORT_NO_NUKES,
    AI_ABORT_KILLED,
	AI_ABORT_WAR_STATE_CHANGE,
	AI_ABORT_DIPLO_OPINION_CHANGE,
	AI_ABORT_LOST_PATH,
#if defined(MOD_BALANCE_CORE)
	AI_ABORT_TIMED_OUT,
	AI_ABORT_NO_UNITS,
#endif
};

FDataStream& operator<<(FDataStream&, const AIOperationState&);
FDataStream& operator>>(FDataStream&, AIOperationState&);

typedef CvWeightedVector<int, 128, true> WeightedUnitIdVector;

struct OperationSlot
{
	OperationSlot()
	{
		m_iOperationID = -1;
		m_iArmyID = -1;
		m_iSlotID = -1;
	}

	bool operator==(const OperationSlot& other) const
	{
		return (other.m_iArmyID == m_iArmyID && other.m_iSlotID == m_iSlotID && other.m_iOperationID == m_iOperationID);
	}

	bool IsValid() const
	{
		return (m_iArmyID != -1 && m_iSlotID != -1 && m_iOperationID != -1);
	}

	void Invalidate()
	{
		m_iOperationID = -1;
		m_iArmyID = -1;
		m_iSlotID = -1;
	}

	int m_iOperationID;
	int m_iArmyID;
	int m_iSlotID;
};

FDataStream& operator<<(FDataStream&, const OperationSlot&);
FDataStream& operator>>(FDataStream&, OperationSlot&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperation
//!  \brief		Operational maneuvers performed by the AI
//
//!  Key Attributes:
//!  - Based class: behavior is inherited from this class for each individual maneuver
//!  - AI operations are launched by some player strategies
//!  - Each operations manages one or more armies (multiple armies in an operation not yet tested)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperation
{
public:
	CvAIOperation();
	virtual ~CvAIOperation();

	// simple factory method to create new subclassed operations - use it instead of the constructor, please (I'm not going to force the issue as this is a very simple facotry)
	static CvAIOperation* CreateOperation(AIOperationTypes eAIOperationType);

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL) = 0;
	virtual void Reset(int iID=-1, PlayerTypes eOwner=NO_PLAYER, PlayerTypes eEnemy=NO_PLAYER);

	//pure virtual methods for subclassing
	virtual int GetOperationType() const = 0;
	virtual MultiunitFormationTypes GetFormation() const = 0;
	virtual AIOperationMovementType GetMoveType() const = 0;
	virtual int GetDeployRange() const = 0;
	virtual bool IsCivilianOperation() const = 0;
	virtual bool IsNavalOperation() const = 0;
	virtual const char* GetOperationName() const = 0;
	virtual bool CheckOnTarget() = 0;
	virtual bool VerifyTarget(CvArmyAI* pArmy) { return true; } //making this pure virtual gives a linker error???
	virtual AITacticalTargetType GetTargetType() const = 0;

	//virtual methods with a sane default
	virtual int GetMaximumRecruitTurns() const;
	virtual bool CanTacticalAIInterruptOperation() const
	{
		return false;
	}

	//accessors
	AIOperationState GetOperationState()
	{
		return m_eCurrentState;
	};
	int GetLastTurnMoved() const
	{
		return m_iLastTurnMoved;
	};
	void SetLastTurnMoved(int iValue)
	{
		m_iLastTurnMoved = iValue;
	};
	int GetTurnStarted() const
	{
		return m_iTurnStarted;
	};
	void SetTurnStarted(int iValue)
	{
		m_iTurnStarted = iValue;
	};
	int GetID() const
	{
		return m_iID;
	}
	inline PlayerTypes GetOwner() const
	{
		return m_eOwner;
	}
	PlayerTypes GetEnemy() const
	{
		return m_eEnemy;
	}

	bool HasTargetPlot() const { return (m_iTargetX!=INVALID_PLOT_COORD && m_iTargetY!=INVALID_PLOT_COORD); }
	bool HasMusterPlot() const { return (m_iMusterX!=INVALID_PLOT_COORD && m_iMusterY!=INVALID_PLOT_COORD); }

	CvPlot* GetTargetPlot() const;
	void SetTargetPlot(CvPlot* pTarget);
	CvPlot* GetMusterPlot() const;
	void SetMusterPlot(CvPlot* pTarget);
	virtual int GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const;

	virtual int  PercentFromMusterPointToTarget();
	virtual bool ShouldAbort();
	virtual void SetToAbort(AIOperationAbortReason eReason);
	virtual void Kill(AIOperationAbortReason eReason );
	virtual void DoTurn();
	virtual bool DoDelayedDeath();
	virtual void Move();

	virtual void BuildListOfUnitsWeStillNeedToBuild();
	size_t GetNumUnitsNeededToBeBuilt()	{ return m_viListOfUnitsWeStillNeedToBuild.size(); }
	size_t GetNumUnitsCommittedToBeBuilt()	{ return m_viListOfUnitsCitiesHaveCommittedToBuild.size(); }

	virtual OperationSlot PeekAtNextUnitToBuild(int iAreaID);
	virtual OperationSlot CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity* pCity);
	virtual bool UncommitToBuild(OperationSlot thisOperationSlot);
	virtual bool FinishedBuilding(OperationSlot thisOperationSlot);

	virtual bool RecruitUnit(CvUnit* pUnit);
	virtual bool GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot);
	bool DeleteArmyAI(int iID);
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvPlot* ComputeTargetPlotForThisTurn(CvArmyAI* pArmy) const;

	bool HasOneMoreSlotToFill() const;
	bool BuyFinalUnit();

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	void LogOperationStart();
	void LogOperationStatus();
	void LogOperationSpecialMessage(const CvString& strMsg);
	void LogOperationEnd();

	virtual const char* GetInfoString();

protected:
	static CvString GetLogFileName(CvString& playerName);

	CvPlot* GetPlotXInStepPath(CvPlot* pCurrentPosition, CvPlot* pTarget, int iStep, bool bForward) const;
	int GetStepDistanceBetweenPlots(CvPlot* pCurrentPosition, CvPlot* pTarget) const;

	virtual bool SetupWithSingleArmy(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvPlot* pDeployPlot=NULL, CvUnit* pInitialUnit=NULL);
	virtual CvArmyAI* AddArmy();
	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector& UnitChoices);

	std::vector<int> m_viArmyIDs;
	std::deque<OperationSlot> m_viListOfUnitsWeStillNeedToBuild;
	std::vector<OperationSlot> m_viListOfUnitsCitiesHaveCommittedToBuild;

	int m_iID;
	AIOperationState m_eCurrentState;
	AIOperationAbortReason m_eAbortReason;
	int m_iLastTurnMoved;
	int m_iTurnStarted;

	PlayerTypes m_eOwner;
	PlayerTypes m_eEnemy;

	// Coordinates of muster plot
	int m_iMusterX;
	int m_iMusterY;

	// Coordinates of target plot
	int m_iTargetX;
	int m_iTargetY;

	// Calculate only once, ideally
	int m_iDistanceMusterToTarget;

	// for debugging
	CvString m_strInfoString;
};

FDataStream& operator<<(FDataStream&, const AIOperationMovementType&);
FDataStream& operator>>(FDataStream&, AIOperationMovementType&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationMilitary
//!  \brief		Base class for a military operations
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationMilitary : public CvAIOperation
{
public:
	CvAIOperationMilitary() {}
	virtual ~CvAIOperationMilitary() {}

	virtual AIOperationMovementType GetMoveType() const { return AI_OPERATION_MOVETYPE_COMBAT; }
	virtual bool IsCivilianOperation() const { return false; }
	virtual bool IsNavalOperation() const { return false; }
	virtual int GetDeployRange() const { return 2; }
	virtual bool VerifyTarget(CvArmyAI * pArmy) { return true; }
	virtual bool CheckOnTarget();
	virtual void SetToAbort(AIOperationAbortReason eReason);

protected:
	//the default version returns the initial target always
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefensive
//!  \brief		Base class for defensive operations which primarily happen at home
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefensive : public CvAIOperationMilitary
{
public:
	CvAIOperationDefensive() {}
	virtual ~CvAIOperationDefensive() {}

	virtual AITacticalTargetType GetTargetType() const { return AI_TACTICAL_TARGET_CITY_TO_DEFEND; }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensive
//!  \brief		Base class for offensive operations which primarily happen in enemy lands
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensive : public CvAIOperationMilitary
{
public:
	CvAIOperationOffensive() {}
	virtual ~CvAIOperationOffensive() {}

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual AITacticalTargetType GetTargetType() const { return AI_TACTICAL_TARGET_CITY; }

	virtual int GetMaximumRecruitTurns() const;
	virtual bool VerifyTarget(CvArmyAI* pArmy);

protected:
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityAttackBasic
//!  \brief		Attack a city with a small force
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityAttackBasic : public CvAIOperationOffensive
{
public:

	CvAIOperationCityAttackBasic();
	virtual ~CvAIOperationCityAttackBasic();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_BASIC_CITY_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_BASIC_CITY_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const;

	virtual bool ShouldAbort();

protected:
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCitySneakAttack
//!  \brief		Same as Basic City attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCitySneakAttack : public CvAIOperationCityAttackBasic
{
public:
	CvAIOperationCitySneakAttack() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_SNEAK_CITY_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_SNEAK_CITY_ATTACK");
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityAttackSmall
//!  \brief		Same as Basic City attack except always small formation
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityAttackSmall : public CvAIOperationCityAttackBasic
{
public:
	CvAIOperationCityAttackSmall() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_SMALL_CITY_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_SMALL_CITY_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_SMALL_CITY_ATTACK_FORCE;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityStateAttack
//!  \brief		Same as Basic City attack except a smaller formation
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityStateAttack : public CvAIOperationCityAttackBasic
{
public:
	CvAIOperationCityStateAttack() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_STATE_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_CITY_STATE_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CITY_STATE_ATTACK_FORCE;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveAntiBarbarian
//!  \brief		Send out a squad of units to take out a barbarian camp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveAntiBarbarian : public CvAIOperationOffensive
{
public:

	CvAIOperationOffensiveAntiBarbarian();
	virtual ~CvAIOperationOffensiveAntiBarbarian();

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_DESTROY_BARBARIAN_CAMP;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_DESTROY_BARBARIAN_CAMP");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_ANTI_BARBARIAN_TEAM;
	}
	virtual AITacticalTargetType GetTargetType() const
	{
		return AI_TACTICAL_TARGET_BARBARIAN_CAMP;
	}

	virtual int GetDeployRange() const;
	virtual bool ShouldAbort();
	virtual bool VerifyTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
	int m_iUnitToRescue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationPillageEnemy
//!  \brief		Create a fast strike team to harass the enemy
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationPillageEnemy : public CvAIOperationOffensive
{
public:

	CvAIOperationPillageEnemy();
	virtual ~CvAIOperationPillageEnemy();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_PILLAGE_ENEMY;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_PILLAGE_ENEMY");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_FAST_PILLAGERS;
	}
	virtual int GetDeployRange() const;
	virtual AITacticalTargetType GetTargetType() const
	{
		//this should attract the pillagers!
		return AI_TACTICAL_TARGET_CITADEL;
	}
	virtual bool CanTacticalAIInterruptOperation() const
	{
		return true;
	}

	virtual bool VerifyTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilian
//!  \brief		Base class for operations that are one military unit and one civilian
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilian : public CvAIOperation
{
public:

	CvAIOperationCivilian();
	virtual ~CvAIOperationCivilian();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual AIOperationMovementType GetMoveType() const { return AI_OPERATION_MOVETYPE_ESCORT; }
	virtual int GetDeployRange() const { return 1; }
	virtual bool IsCivilianOperation() const { return true; }
	virtual bool IsNavalOperation() const { return false; }
	virtual AITacticalTargetType GetTargetType() const { return AI_TACTICAL_TARGET_NONE; }
	virtual bool CheckOnTarget();
	virtual UnitAITypes GetCivilianType() const = 0;

	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvUnit* FindBestCivilian();
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths) = 0;

	virtual bool RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy);
	virtual bool VerifyTarget(CvArmyAI* pArmy);
	virtual bool IsEscorted();

	//we have arrived. subclass needs to decide what happens
	virtual bool PerformMission(CvUnit* pUnit) = 0;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianFoundCity
//!  \brief		Find a place to utilize a new settler
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianFoundCity : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianFoundCity();
	virtual ~CvAIOperationCivilianFoundCity();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_FOUND_CITY;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_FOUND_CITY");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_SETTLER_ESCORT;
	}
	virtual UnitAITypes GetCivilianType() const
	{
		return UNITAI_SETTLE;
	}

	virtual bool PerformMission(CvUnit* pUnit);
	virtual bool VerifyTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTargetIncludingCurrent(CvUnit* pUnit, bool bEscorted);
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianQuickColonize
//!  \brief		Send a settler alone to a nearby island
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianQuickColonize : public CvAIOperationCivilianFoundCity
{
public:

	CvAIOperationCivilianQuickColonize();
	virtual ~CvAIOperationCivilianQuickColonize();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_QUICK_COLONIZE;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_QUICK_COLONIZE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_QUICK_COLONY_SETTLER;
	}

private:

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianMerchantDelegation
//!  \brief		Send a merchant to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianMerchantDelegation : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianMerchantDelegation();
	virtual ~CvAIOperationCivilianMerchantDelegation();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_MERCHANT_DELEGATION;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_MERCHANT_DELEGATION");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_MERCHANT_ESCORT;
	}
	virtual UnitAITypes GetCivilianType() const
	{
		return UNITAI_MERCHANT;
	}

	virtual bool PerformMission(CvUnit* pUnit);
	virtual bool VerifyTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths);
};

#if defined(MOD_DIPLOMACY_CITYSTATES)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianDiplomatDelegation
//!  \brief		Send a diplomat to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianDiplomatDelegation : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianDiplomatDelegation();
	virtual ~CvAIOperationCivilianDiplomatDelegation();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_DIPLOMAT_DELEGATION;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_DIPLOMAT_DELEGATION");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_DIPLOMAT_ESCORT;
	}
	virtual UnitAITypes GetCivilianType() const
	{
		return UNITAI_DIPLOMAT;
	}

	virtual bool PerformMission(CvUnit* pUnit);
	virtual bool VerifyTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefendCity
//!  \brief		Defend a specific city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefendAlly : public CvAIOperationDefensive
{
public:

	CvAIOperationDefendAlly();
	virtual ~CvAIOperationDefendAlly();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_ALLY_DEFENSE;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_ALLY_DEFENSE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CLOSE_CITY_DEFENSE;
	}
	virtual bool ShouldAbort();

private:
};
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianConcertTour
//!  \brief		Send a merchant to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianConcertTour : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianConcertTour();
	virtual ~CvAIOperationCivilianConcertTour();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CONCERT_TOUR;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_CONCERT_TOUR");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CONCERT_TOUR;
	}
	virtual UnitAITypes GetCivilianType() const
	{
		return UNITAI_MUSICIAN;
	}
	
	virtual bool PerformMission(CvUnit* pUnit);
	virtual bool VerifyTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, bool bOnlySafePaths);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNaval
//!  \brief		Base class for offensive "pure" naval operations without land units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalBasic : public CvAIOperationOffensive
{
public:

	CvAIOperationOffensiveNavalBasic();
	virtual ~CvAIOperationOffensiveNavalBasic();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_ATTACK");
	}
	virtual bool ShouldAbort();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_SQUADRON;
	}
	virtual bool IsNavalOperation() const
	{
		return true;
	};
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalBombardment
//!  \brief		Send out a squadron of naval units to bomb enemy forces on the coast
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalBombardment : public CvAIOperationOffensiveNavalBasic
{
public:

	CvAIOperationOffensiveNavalBombardment();
	virtual ~CvAIOperationOffensiveNavalBombardment();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_BOMBARDMENT;
	}
	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_BOMBARDMENT;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_BOMBARDMENT");
	}
	virtual AITacticalTargetType GetTargetType() const 
	{ 
		return AI_TACTICAL_TARGET_BOMBARDMENT_ZONE;
	}

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalSuperiority
//!  \brief		Send out a squadron of naval units to rule the seas
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalSuperiority : public CvAIOperationOffensiveNavalBasic
{
public:

	CvAIOperationOffensiveNavalSuperiority();
	virtual ~CvAIOperationOffensiveNavalSuperiority();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_SUPERIORITY;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_SUPERIORITY");
	}

	virtual bool ShouldAbort();
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_SQUADRON;
	}
	virtual AITacticalTargetType GetTargetType() const 
	{ 
		return AI_TACTICAL_TARGET_BOMBARDMENT_ZONE;
	}

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalOnlyCityAttack
//!  \brief		Try to take out an enemy city from the sea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalOnlyCityAttack : public CvAIOperationOffensiveNavalBasic
{
public:

	CvAIOperationOffensiveNavalOnlyCityAttack();
	virtual ~CvAIOperationOffensiveNavalOnlyCityAttack();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_PURE_NAVAL_CITY_ATTACK;
	}
	virtual int GetOperationType() const
	{
		return AI_OPERATION_PURE_NAVAL_CITY_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_PURE_NAVAL_CITY_ATTACK");
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefendCity
//!  \brief		Defend a specific city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefendCity : public CvAIOperationDefensive
{
public:

	CvAIOperationDefendCity();
	virtual ~CvAIOperationDefendCity();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_CLOSE_DEFENSE;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_CITY_CLOSE_DEFENSE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CLOSE_CITY_DEFENSE;
	}

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefendCity
//!  \brief		Defend a specific city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefendCityPeace : public CvAIOperationDefendCity
{
public:

	CvAIOperationDefendCityPeace();
	virtual ~CvAIOperationDefendCityPeace();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_CLOSE_DEFENSE_PEACE;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_CITY_CLOSE_DEFENSE_PEACE");
	}
	virtual bool CanTacticalAIInterruptOperation() const
	{
		return true;
	}

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefenseRapidResponse
//!  \brief		Mobile force that can defend where threatened
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefenseRapidResponse : public CvAIOperationDefensive
{
public:

	CvAIOperationDefenseRapidResponse();
	virtual ~CvAIOperationDefenseRapidResponse();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_RAPID_RESPONSE;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_RAPID_RESPONSE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_RAPID_RESPONSE_FORCE;
	}

	virtual bool VerifyTarget(CvArmyAI* pArmy);

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalEscorted
//!  \brief		Base class for operations that require a naval escort for land units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalEscorted : public CvAIOperationOffensive
{
public:

	CvAIOperationOffensiveNavalEscorted();
	virtual ~CvAIOperationOffensiveNavalEscorted();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual bool VerifyTarget(CvArmyAI* pArmy);

	virtual bool IsNavalOperation() const
	{
		return true;
	};

	virtual bool CheckOnTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalAttack
//!  \brief		Attack a city from the sea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalAttack : public CvAIOperationOffensiveNavalEscorted
{
public:

	CvAIOperationOffensiveNavalAttack() {}
	virtual ~CvAIOperationOffensiveNavalAttack() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_INVASION;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianColonization
//!  \brief		Found a city on a different continent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianColonization : public CvAIOperationCivilianFoundCity
{
public:

	CvAIOperationCivilianColonization() {}
	virtual ~CvAIOperationCivilianColonization() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_COLONIZATION;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_COLONIZATION");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_COLONIZATION_PARTY;
	}
	virtual bool IsNavalOperation() const
	{
		return true;
	};

protected:
	virtual bool RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalSneakAttack
//!  \brief		Same as basic naval attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalSneakAttack : public CvAIOperationOffensiveNavalAttack
{
public:
	CvAIOperationOffensiveNavalSneakAttack();
	virtual ~CvAIOperationOffensiveNavalSneakAttack();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_SNEAK_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_SNEAK_ATTACK");
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationOffensiveNavalCityStateAttack
//!  \brief		Same as basic naval attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationOffensiveNavalCityStateAttack : public CvAIOperationOffensiveNavalEscorted
{
public:
	CvAIOperationOffensiveNavalCityStateAttack() {}
	virtual ~CvAIOperationOffensiveNavalCityStateAttack() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_STATE_NAVAL_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_CITY_STATE_NAVAL_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CITY_STATE_INVASION;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNukeAttack
//!  \brief		When you care enough to send the very best
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNukeAttack : public CvAIOperationMilitary
{
public:

	CvAIOperationNukeAttack();
	virtual ~CvAIOperationNukeAttack();
	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NUKE_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return CvString("AI_OPERATION_NUKE_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NUKE_ATTACK;
	}
	virtual int GetDeployRange() const
	{
		return 10;
	}
	virtual AITacticalTargetType GetTargetType() const
	{ 
		return AI_TACTICAL_TARGET_NONE;
	}

	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector& UnitChoices);

	virtual bool CheckOnTarget();

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};


namespace OperationalAIHelpers
{
	int GetGatherRangeForXUnits(int iTotalUnits);
	CvPlot* FindBestBarbarianBombardmentTarget(PlayerTypes ePlayer);
	CvPlot* FindBestBarbCamp(PlayerTypes ePlayer);
	CvPlot* FindEnemies(PlayerTypes ePlayer, PlayerTypes eEnemy, DomainTypes eDomain, bool bHomelandOnly, int iRefArea, CvPlot* pRefPlot);
	bool IsSlotRequired(PlayerTypes ePlayer, const OperationSlot& thisOperationSlot);
	bool IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool bMustNaval, bool bMustBeDeepWaterNaval, int& iDistance);
	bool NeedOceanMoves(PlayerTypes ePlayer, CvPlot* pMusterPlot, CvPlot* pTargetPlot);
}

#endif

/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
	AI_OPERATION_TYPE_INVALID = -1,
	AI_OPERATION_FOUND_CITY,
    AI_OPERATION_FOUND_CITY_QUICK,
	AI_OPERATION_FOUND_CITY_OVERSEAS, //not used anymore, too many pathfinding issues with the naval escort

	AI_OPERATION_DESTROY_BARBARIAN_CAMP,
    AI_OPERATION_PILLAGE_ENEMY,

	AI_OPERATION_CITY_CLOSE_DEFENSE,
	AI_OPERATION_BULLY_CITY_STATE,
    AI_OPERATION_RAPID_RESPONSE,

	AI_OPERATION_CITY_BASIC_ATTACK,
    AI_OPERATION_CITY_SNEAK_ATTACK,
    AI_OPERATION_CITY_STATE_ATTACK,

    AI_OPERATION_NAVAL_INVASION,
    AI_OPERATION_NAVAL_INVASION_SNEAKY,
    AI_OPERATION_NAVAL_INVASION_CITY_STATE,

	AI_OPERATION_NAVAL_ONLY_CITY_ATTACK,
    AI_OPERATION_NAVAL_SUPERIORITY,
	AI_OPERATION_NAVAL_BOMBARDMENT,
	AI_OPERATION_NAVAL_BULLY_CITY_STATE,

	AI_OPERATION_NUKE_ATTACK,
	AI_OPERATION_ALLY_DEFENSE,

	AI_OPERATION_CONCERT_TOUR,
    AI_OPERATION_MERCHANT_DELEGATION,
	AI_OPERATION_DIPLOMAT_DELEGATION,

	NUM_AI_OPERATIONS,
};

enum AIOperationState
{
    AI_OPERATION_STATE_INVALID = -1,
    AI_OPERATION_STATE_ABORTED,
    AI_OPERATION_STATE_RECRUITING_UNITS,
    AI_OPERATION_STATE_GATHERING_FORCES,
    AI_OPERATION_STATE_MOVING_TO_TARGET,
    AI_OPERATION_STATE_AT_TARGET,
    AI_OPERATION_STATE_SUCCESSFUL_FINISH,
};

enum AIOperationMovementType
{
    AI_OPERATION_MOVE_TYPE_INVALID = -1,
    AI_OPERATION_MOVETYPE_ESCORT,	//try to move somewhere while avoiding combat
    AI_OPERATION_MOVETYPE_COMBAT,	//try to move somewhere while expecting combat
	AI_OPERATION_MOVETYPE_COMBAT_ESCORT, //Hybrid
};

enum AIOperationAbortReason
{
    NO_ABORT_REASON = -1,
    AI_ABORT_SUCCESS,
	//these are the bad outcomes
    AI_ABORT_NO_TARGET,
    AI_ABORT_LOST_TARGET,
	AI_ABORT_LOST_PATH,
    AI_ABORT_NO_ROOM_DEPLOY,
    AI_ABORT_NO_MUSTER,
	AI_ABORT_NO_UNITS,
	//these are normal outcomes	
	AI_ABORT_CANCELLED,
    AI_ABORT_TARGET_ALREADY_CAPTURED,
    AI_ABORT_HALF_STRENGTH,
    AI_ABORT_LOST_CIVILIAN,
    AI_ABORT_ESCORT_DIED,
    AI_ABORT_TOO_DANGEROUS,
    AI_ABORT_KILLED,
	AI_ABORT_WAR_STATE_CHANGE,
	AI_ABORT_DIPLO_OPINION_CHANGE,
	AI_ABORT_TIMED_OUT,
};

FDataStream& operator<<(FDataStream&, const AIOperationState&);
FDataStream& operator>>(FDataStream&, AIOperationState&);

typedef CvWeightedVector<int, 128, true> WeightedUnitIdVector;

struct OperationSlot
{
	explicit OperationSlot(int opId=-1, int armyId=-1, int slotId=-1)
	{
		m_iOperationID = opId;
		m_iArmyID = armyId;
		m_iSlotID = slotId;
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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false) = 0; 
	virtual void Reset(int iID=-1, PlayerTypes eOwner=NO_PLAYER, PlayerTypes eEnemy=NO_PLAYER);

	//pure virtual methods for subclassing
	virtual int GetOperationType() const = 0;
	virtual MultiunitFormationTypes GetFormation() const = 0;
	virtual AIOperationMovementType GetMoveType() const = 0;
	virtual int GetDeployRange() const = 0;
	virtual bool IsCivilianOperation() const = 0;
	virtual bool IsNavalOperation() const = 0;
	virtual const char* GetOperationName() const = 0;
	virtual bool CheckTransitionToNextStage() = 0;
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy) = 0;

	//virtual methods with a sane default
	virtual int GetMaximumRecruitTurnsBase() const;
	virtual int GetMaximumRecruitTurns() const;
	virtual bool IsOffensive() const { return false; }
	virtual bool IsAllowedDuringPeace() const { return false; }
	virtual bool IsDefensive() const { return false; }
	virtual bool IsShowOfForce() const { return false; }
	virtual void OnSuccess() const {}

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
	virtual void Kill();
	virtual bool DoTurn();
	virtual bool Move();

	virtual void BuildListOfUnitsWeStillNeedToBuild();
	size_t GetNumUnitsNeededToBeBuilt()	{ return m_viListOfUnitsWeStillNeedToBuild.size(); }
	size_t GetNumUnitsCommittedToBeBuilt()	{ return m_viListOfUnitsCitiesHaveCommittedToBuild.size(); }

	virtual OperationSlot PeekAtNextUnitToBuild();
	virtual OperationSlot CommitToBuildNextUnit();
	virtual bool UncommitToBuild(OperationSlot thisOperationSlot);
	virtual bool FinishedBuilding(OperationSlot thisOperationSlot);

	virtual bool RecruitUnit(CvUnit* pUnit);
	virtual bool GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvArmyAI* pThisArmy = NULL);
	bool DeleteArmyAI(int iID);
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvPlot* ComputeTargetPlotForThisTurn(CvArmyAI* pArmy) const;

	bool BuyFinalUnit();

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	void LogOperationStart() const;
	void LogOperationStatus(bool bPreTurn) const;
	void LogOperationSpecialMessage(const CvString& strMsg) const;
	void LogOperationEnd() const;

	virtual const char* GetInfoString();

protected:
	static CvString GetLogFileName(CvString& playerName);

	CvPlot* GetPlotXInStepPath(CvPlot* pCurrentPosition, CvPlot* pTarget, int iStep, bool bForward) const;
	int GetStepDistanceBetweenPlots(CvPlot* pCurrentPosition, CvPlot* pTarget) const;

	virtual bool SetupWithSingleArmy(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvPlot* pDeployPlot = NULL, CvUnit* pInitialUnit = NULL, bool bOceanMoves = false, bool bSkipRecruiting = false);
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
	virtual int GetDeployRange() const { return 3; }
	virtual bool CheckTransitionToNextStage();
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

	virtual bool IsDefensive() const { return true; }

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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetMaximumRecruitTurnsBase() const;
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
	virtual bool IsOffensive() const { return true; }
	virtual bool IsShowOfForce() const;

protected:
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityBasicAttack
//!  \brief		Attack a city with a small force
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityBasicAttack : public CvAIOperationOffensive
{
public:

	CvAIOperationCityBasicAttack();
	virtual ~CvAIOperationCityBasicAttack();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_BASIC_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_CITY_BASIC_ATTACK";
	}
	virtual MultiunitFormationTypes GetFormation() const;

	virtual void OnSuccess() const;

protected:
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCitySneakAttack
//!  \brief		Same as Basic City attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCitySneakAttack : public CvAIOperationCityBasicAttack
{
public:
	CvAIOperationCitySneakAttack() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_SNEAK_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_CITY_SNEAK_ATTACK";
	}
	virtual bool IsAllowedDuringPeace() const 
	{ 
		return true; 
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityStateAttack
//!  \brief		Same as Basic City attack except a smaller formation
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityStateAttack : public CvAIOperationCityBasicAttack
{
public:
	CvAIOperationCityStateAttack() {}

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_STATE_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_CITY_STATE_ATTACK";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CITY_STATE_ATTACK_FORCE;
	}
	virtual bool IsAllowedDuringPeace() const
	{ 
		return true; 
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationAntiBarbarian
//!  \brief		Send out a squad of units to take out a barbarian camp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationAntiBarbarian : public CvAIOperationOffensive
{
public:

	CvAIOperationAntiBarbarian();
	virtual ~CvAIOperationAntiBarbarian();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_DESTROY_BARBARIAN_CAMP;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_DESTROY_BARBARIAN_CAMP";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_ANTI_BARBARIAN_TEAM;
	}
	virtual bool IsAllowedDuringPeace() const
	{ 
		return true; 
	}
	virtual int GetMaximumRecruitTurnsBase() const
	{
		return 4; //we don't have many units at the beginning. if there aren't enough, don't keep the available ones waiting 
	}
	virtual int GetMaximumRecruitTurns() const
	{
		return GetMaximumRecruitTurnsBase(); //don't extend this
	}

	virtual int GetDeployRange() const;
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_PILLAGE_ENEMY;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_PILLAGE_ENEMY";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_FAST_PILLAGERS;
	}
	virtual int GetDeployRange() const;
	virtual void OnSuccess() const;

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual AIOperationMovementType GetMoveType() const { return AI_OPERATION_MOVETYPE_ESCORT; }
	virtual int GetDeployRange() const { return 1; }
	virtual bool IsCivilianOperation() const { return true; }
	virtual bool IsNavalOperation() const { return false; }
	virtual bool CheckTransitionToNextStage();
	virtual UnitAITypes GetCivilianType() const = 0;

	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvUnit* FindBestCivilian();
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, int iAreaID) = 0;

	virtual bool RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy);
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
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
		return "AI_OPERATION_FOUND_CITY";
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
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTargetIncludingCurrent(CvUnit* pUnit, int iAreaID);
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, int iAreaID);
	virtual void	LogSettleTarget(const char* hint, CvPlot* pTarget) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianFoundCityQuick
//!  \brief		Send a settler alone
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianFoundCityQuick : public CvAIOperationCivilianFoundCity
{
public:

	CvAIOperationCivilianFoundCityQuick();
	virtual ~CvAIOperationCivilianFoundCityQuick();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_FOUND_CITY_QUICK;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_FOUND_CITY_QUICK";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_QUICK_COLONY_SETTLER;
	}

private:

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianConcertTour
//!  \brief		Send a Great Musician to a city state
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
		return "AI_OPERATION_CONCERT_TOUR";
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
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, int iAreaID);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianMerchantDelegation
//!  \brief		Send a Great Merchant to a city state
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
		return "AI_OPERATION_MERCHANT_DELEGATION";
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
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, int iAreaID);
};

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
		return "AI_OPERATION_DIPLOMAT_DELEGATION";
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
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit, int iAreaID);
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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_ALLY_DEFENSE;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_ALLY_DEFENSE";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CLOSE_CITY_DEFENSE;
	}
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

private:
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalOnlyBasic
//!  \brief		Base class for offensive "pure" naval operations without land units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalOnlyBasic : public CvAIOperationOffensive
{
public:

	CvAIOperationNavalOnlyBasic() {}
	virtual ~CvAIOperationNavalOnlyBasic() {}

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
	virtual int GetDeployRange() const { return 3; }
	virtual bool IsNavalOperation() const
	{
		return true;
	};
	virtual MultiunitFormationTypes GetFormation() const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalBombardment
//!  \brief		Send out a squadron of naval units to bomb enemy forces on the coast
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalBombardment : public CvAIOperationNavalOnlyBasic
{
public:

	CvAIOperationNavalBombardment();
	virtual ~CvAIOperationNavalBombardment();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

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
		return "AI_OPERATION_NAVAL_BOMBARDMENT";
	}

	virtual bool IsDefensive() const { return false; }
	virtual bool IsOffensive() const { return true; }

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalSuperiority
//!  \brief		Send out a squadron of naval units to rule the seas
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalSuperiority : public CvAIOperationNavalOnlyBasic
{
public:

	CvAIOperationNavalSuperiority();
	virtual ~CvAIOperationNavalSuperiority();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_SUPERIORITY;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NAVAL_SUPERIORITY";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_SQUADRON;
	}
	virtual bool IsDefensive() const { return true; }
	virtual bool IsOffensive() const { return false; }
	virtual void OnSuccess() const;

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalOnlyCityAttack
//!  \brief		Try to take out an enemy city from the sea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalOnlyCityAttack : public CvAIOperationNavalOnlyBasic
{
public:

	CvAIOperationNavalOnlyCityAttack();
	virtual ~CvAIOperationNavalOnlyCityAttack();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_PURE_NAVAL_CITY_ATTACK;
	}
	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_ONLY_CITY_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NAVAL_ONLY_CITY_ATTACK";
	}
	virtual bool IsAllowedDuringPeace() const
	{ 
		return true; 
	}

	virtual void OnSuccess() const;
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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_CLOSE_DEFENSE;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_CITY_CLOSE_DEFENSE";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CLOSE_CITY_DEFENSE;
	}
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationBullyCityState
//!  \brief		Move units next to a city state, but don't attack. Allowed during peace.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationBullyCityState : public CvAIOperationCityStateAttack
{
public:

	CvAIOperationBullyCityState();
	virtual ~CvAIOperationBullyCityState();

	virtual int GetOperationType() const
	{
		return 	AI_OPERATION_BULLY_CITY_STATE;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_BULLY_CITY_STATE";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CITY_STATE_ATTACK_FORCE;
	}
	virtual int GetDeployRange() const
	{ 
		return 4;
	}
	virtual bool DoTurn();
	virtual bool IsShowOfForce() const { return true; }

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalBullyCityState
//!  \brief		Move units next to a city state, but don't attack. Allowed during peace.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalBullyCityState : public CvAIOperationNavalOnlyCityAttack
{
public:

	CvAIOperationNavalBullyCityState();
	virtual ~CvAIOperationNavalBullyCityState();

	virtual int GetOperationType() const
	{
		return 	AI_OPERATION_NAVAL_BULLY_CITY_STATE;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NAVAL_BULLY_CITY_STATE";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_SQUADRON;
	}
	virtual int GetDeployRange() const
	{ 
		return 4;
	}
	virtual bool IsShowOfForce() const { return true; }

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

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_RAPID_RESPONSE;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_RAPID_RESPONSE";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_RAPID_RESPONSE_FORCE;
	}

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalInvasion
//!  \brief		Attack a city from the sea - includes land units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalInvasion : public CvAIOperationOffensive
{
public:

	CvAIOperationNavalInvasion() {}
	virtual ~CvAIOperationNavalInvasion() {}
	virtual AIOperationMovementType GetMoveType() const { return AI_OPERATION_MOVETYPE_COMBAT_ESCORT; }
	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual bool IsNavalOperation() const
	{
		return true;
	};

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_INVASION;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NAVAL_INVASION";
	}
	virtual MultiunitFormationTypes GetFormation() const;

	virtual void OnSuccess() const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalInvasionSneaky
//!  \brief		Same as basic naval invasion except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalInvasionSneaky : public CvAIOperationNavalInvasion
{
public:
	CvAIOperationNavalInvasionSneaky();
	virtual ~CvAIOperationNavalInvasionSneaky();
	virtual AIOperationMovementType GetMoveType() const { return AI_OPERATION_MOVETYPE_COMBAT_ESCORT; }
	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_INVASION_SNEAKY;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NAVAL_INVASION_SNEAKY";
	}
	virtual bool IsAllowedDuringPeace() const
	{ 
		return true; 
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalInvasionCityState
//!  \brief		Same as basic naval attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalInvasionCityState : public CvAIOperationNavalInvasionSneaky
{
public:
	CvAIOperationNavalInvasionCityState() {}
	virtual ~CvAIOperationNavalInvasionCityState() {}
	virtual AIOperationMovementType GetMoveType() const { return AI_OPERATION_MOVETYPE_COMBAT_ESCORT; }
	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_INVASION_CITY_STATE;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NAVAL_INVASION_CITY_STATE";
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
	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iAreaID, CvCity* pTarget = NULL, CvCity* pMuster = NULL, bool bOceanMoves = false);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NUKE_ATTACK;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_NUKE_ATTACK";
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NUKE_ATTACK;
	}
	virtual int GetDeployRange() const
	{
		return 12;
	}
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, WeightedUnitIdVector& UnitChoices);

	virtual bool CheckTransitionToNextStage();

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};


namespace OperationalAIHelpers
{
	int GetGatherRangeForXUnits(int iTotalUnits);
	CvPlot* FindBestCoastalBombardmentTarget(PlayerTypes ePlayer, PlayerTypes eEnemy, CvPlot** ppMuster);
	CvPlot* FindBestBarbCamp(PlayerTypes ePlayer, CvPlot** ppMuster);
	CvPlot* FindEnemiesNearPlot(PlayerTypes ePlayer, PlayerTypes eEnemy, DomainTypes eDomain, bool bHomelandOnly, int iRefArea, CvPlot* pRefPlot);
	bool IsSlotRequired(PlayerTypes ePlayer, const OperationSlot& thisOperationSlot);
	bool IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, CvPlot* pMusterPlot, const ReachablePlots& turnsFromMuster, CvPlot* pTargetPlot, 
				bool bMustNaval, bool bMustBeDeepWaterNaval, CvMultiUnitFormationInfo* thisFormation = NULL, CvArmyAI* pThisArmy = NULL);
	CvCity* GetNearestCoastalCityFriendly(PlayerTypes ePlayer, CvPlot* pRefPlot);
	CvCity* GetNearestCoastalCityFriendly(PlayerTypes ePlayer, PlayerTypes eEnemy);
	CvCity* GetNearestCoastalCityEnemy(PlayerTypes ePlayer, PlayerTypes eEnemy);
}

#endif

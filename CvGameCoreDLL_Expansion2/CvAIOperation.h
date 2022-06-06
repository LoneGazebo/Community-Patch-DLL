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
#include "CvGameCoreUtils.h"

class CvArmyAI;
struct CvAttackTarget;

FDataStream& operator<<(FDataStream&, const ArmyType&);
FDataStream& operator>>(FDataStream&, ArmyType&);

// it is important to add a new entry whenever a new (subclassed) operation is added to the list
enum AIOperationTypes
{
	AI_OPERATION_TYPE_UNKNOWN = -1,

	AI_OPERATION_FOUND_CITY,
    AI_OPERATION_PILLAGE_ENEMY,

	AI_OPERATION_CITY_ATTACK_LAND,
	AI_OPERATION_CITY_ATTACK_NAVAL,
	AI_OPERATION_CITY_ATTACK_COMBINED,

	AI_OPERATION_CITY_DEFENSE,
    AI_OPERATION_RAPID_RESPONSE,
	AI_OPERATION_NAVAL_SUPERIORITY,

	AI_OPERATION_NUKE_ATTACK,
	AI_OPERATION_CARRIER_GROUP,

	AI_OPERATION_MUSICIAN_CONCERT_TOUR,
    AI_OPERATION_MERCHANT_DELEGATION,
	AI_OPERATION_DIPLOMAT_DELEGATION,

	NUM_AI_OPERATIONS,
};

FDataStream& operator<<(FDataStream&, const AIOperationTypes&);
FDataStream& operator>>(FDataStream&, AIOperationTypes&);

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

FDataStream& operator<<(FDataStream&, const AIOperationState&);
FDataStream& operator>>(FDataStream&, AIOperationState&);

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
    AI_ABORT_TARGET_NOT_VALID,
    AI_ABORT_TOO_DANGEROUS,
    AI_ABORT_KILLED,
	AI_ABORT_WAR_STATE_CHANGE,
	AI_ABORT_DIPLO_OPINION_CHANGE,
	AI_ABORT_TIMED_OUT,
};

FDataStream& operator<<(FDataStream&, const AIOperationAbortReason&);
FDataStream& operator>>(FDataStream&, AIOperationAbortReason&);
const char* AbortReasonString(AIOperationAbortReason eReason);

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
	CvAIOperation(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, AIOperationTypes eType, ArmyType eMoveType);
	virtual ~CvAIOperation();

	//pure virtual methods for subclassing
	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL) = 0;
	virtual bool CheckTransitionToNextStage() = 0;
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy) = 0;
	virtual int GetDeployRange() const = 0;

	//todo: replace init with this
	//virtual void Init(const SPath& path) = 0; 
	virtual bool PreconditionsAreMet(CvPlot* pMusterPlot, CvPlot* pTargetPlot, int iMaxMissingUnits);

	//accessors
	AIOperationState GetOperationState() { return m_eCurrentState; }
	int GetLastTurnMoved() const { return m_iLastTurnMoved; }
	void SetLastTurnMoved(int iValue) { m_iLastTurnMoved = iValue; }
	int GetTurnStarted() const { return m_iTurnStarted; }
	void SetTurnStarted(int iValue) { m_iTurnStarted = iValue; }
	int GetID() const { return m_iID; }
	inline PlayerTypes GetOwner() const { return m_eOwner; }
	PlayerTypes GetEnemy() const { return m_eEnemy; }

	AIOperationTypes GetOperationType() const { return m_eType; }
	ArmyType GetArmyType() const { return m_eArmyType; }
	bool IsCivilianOperation() const { return m_eArmyType == ARMY_TYPE_ESCORT_LAND; }
	bool IsNavalOperation() const { return m_eArmyType == ARMY_TYPE_NAVAL || m_eArmyType == ARMY_TYPE_COMBINED; }
	const char* GetOperationName() const;
	CvArmyAI* GetArmy(size_t iIndex) const;

	bool HasTargetPlot() const { return (m_iTargetX != INVALID_PLOT_COORD && m_iTargetY != INVALID_PLOT_COORD); }
	bool HasMusterPlot() const { return (m_iMusterX != INVALID_PLOT_COORD && m_iMusterY != INVALID_PLOT_COORD); }

	CvPlot* GetTargetPlot() const;
	void SetTargetPlot(CvPlot* pTarget);
	CvPlot* GetMusterPlot() const;
	void SetMusterPlot(CvPlot* pTarget);

	size_t GetNumUnitsNeededToBeBuilt() { return m_viListOfUnitsWeStillNeedToBuild.size(); }
	size_t GetNumUnitsCommittedToBeBuilt() { return m_viListOfUnitsCitiesHaveCommittedToBuild.size(); }

	// virtual methods with a sane default
	virtual int GetMaximumRecruitTurns() const;
	virtual void OnSuccess() const {}
	virtual void Reset();
	virtual bool IsOffensive() const;
	virtual bool IsNeverEnding() const { return false; }

	virtual int GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const;
	virtual bool ShouldAbort();
	virtual void SetToAbort(AIOperationAbortReason eReason);
	virtual void Kill();
	virtual bool DoTurn();
	virtual bool Move();

	virtual int  PercentFromMusterPointToTarget() const;

	virtual OperationSlot PeekAtNextUnitToBuild();
	virtual bool CommitToBuildNextUnit(OperationSlot thisOperationSlot);
	virtual bool UncommitToBuildUnit(OperationSlot thisOperationSlot);
	virtual bool FinishedBuildingUnit(OperationSlot thisOperationSlot);
	virtual bool DeleteArmyAI(int iID);
	virtual bool BuyFinalUnit();

	virtual bool RecruitUnit(CvUnit* pUnit);
	virtual int GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvArmyAI* pThisArmy = NULL);
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvPlot* ComputeTargetPlotForThisTurn(CvArmyAI* pArmy) const;

	template<typename AIOperation, typename Visitor>
	static void Serialize(AIOperation& aiOperation, Visitor& visitor);
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

	virtual CvArmyAI* AddArmy(MultiunitFormationTypes eFormation);
	virtual bool SetUpArmy(CvArmyAI* pArmyAI, CvPlot* pMusterPlot, CvPlot* pTargetPlot, CvPlot* pDeployPlot = NULL);
	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, vector<OptionWithScore<int>>& choices);

	std::vector<int> m_viArmyIDs;
	std::deque<OperationSlot> m_viListOfUnitsWeStillNeedToBuild;
	std::vector<OperationSlot> m_viListOfUnitsCitiesHaveCommittedToBuild;

	//constants
	int m_iID;
	AIOperationTypes m_eType;
	//we only support one army ...
	ArmyType m_eArmyType;

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

	// Abort if we're stalling
	deque<int> m_progressToTarget;

	// for debugging
	CvString m_strInfoString;
};

FDataStream& operator>>(FDataStream&, CvAIOperation&);
FDataStream& operator<<(FDataStream&, const CvAIOperation&);

// simple factory method to create new subclassed operations
CvAIOperation* CreateAIOperation(AIOperationTypes eAIOperationType, int iID, PlayerTypes eOwner, PlayerTypes eEnemy);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationMilitary
//!  \brief		Base class for a military operations
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationMilitary : public CvAIOperation
{
public:
	CvAIOperationMilitary(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, AIOperationTypes eType, ArmyType eMoveType) : 
		CvAIOperation(iID,eOwner,eEnemy,eType,eMoveType) {}
	virtual ~CvAIOperationMilitary() {}

	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual int GetDeployRange() const { return 3; }
	virtual int GetMaximumRecruitTurns() const;
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

	virtual bool CheckTransitionToNextStage();
	virtual void OnSuccess() const;

protected:
	//default version does nothing
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityAttackLand
//!  \brief		Attack a city with a small force
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityAttackLand : public CvAIOperationMilitary
{
public:

	CvAIOperationCityAttackLand(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_CITY_ATTACK_LAND,ARMY_TYPE_LAND) {}
	virtual ~CvAIOperationCityAttackLand() {}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityAttackNaval
//!  \brief		Try to take out an enemy city from the sea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityAttackNaval : public CvAIOperationMilitary
{
public:

	CvAIOperationCityAttackNaval(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_CITY_ATTACK_NAVAL,ARMY_TYPE_NAVAL) {}
	virtual ~CvAIOperationCityAttackNaval() {}
	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityAttackCombined
//!  \brief		Attack a city from the sea - includes land units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityAttackCombined : public CvAIOperationMilitary
{
public:

	CvAIOperationCityAttackCombined(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_CITY_ATTACK_COMBINED,ARMY_TYPE_COMBINED) {}
	virtual ~CvAIOperationCityAttackCombined() {}
	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCarrierGroup
//!  \brief		Protect an aircraft carrier. The aircraft themselves are not included in the operation!
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCarrierGroup : public CvAIOperationMilitary
{
public:

	CvAIOperationCarrierGroup(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_CARRIER_GROUP,ARMY_TYPE_NAVAL) {}
	virtual ~CvAIOperationCarrierGroup() {}
	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
	virtual bool IsNeverEnding() const { return true; }
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
protected:
	set<int> GetPossibleDeploymentZones() const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNukeAttack
//!  \brief		When you care enough to send the very best
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNukeAttack : public CvAIOperationMilitary
{
public:

	CvAIOperationNukeAttack(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_NUKE_ATTACK, ARMY_TYPE_AIR) {}
	virtual ~CvAIOperationNukeAttack() {}
	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual bool PreconditionsAreMet(CvPlot* pMusterPlot, CvPlot* pTargetPlot, int iMaxMissingUnits);

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, vector<OptionWithScore<int>>& choices);
	virtual bool CheckTransitionToNextStage();

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationPillageEnemy
//!  \brief		Create a fast strike team to harass the enemy
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationPillageEnemy : public CvAIOperationMilitary
{
public:

	CvAIOperationPillageEnemy(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_PILLAGE_ENEMY,ARMY_TYPE_LAND) {}
	virtual ~CvAIOperationPillageEnemy() {}

	virtual bool PreconditionsAreMet(CvPlot* pMusterPlot, CvPlot* pTargetPlot, int iMaxMissingUnits);

	virtual AIOperationTypes GetOperationType() const
	{
		return AI_OPERATION_PILLAGE_ENEMY;
	}
	virtual const char* GetOperationName() const
	{
		return "AI_OPERATION_PILLAGE_ENEMY";
	}

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalSuperiority
//!  \brief		Send out a squadron of naval units to rule the seas
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalSuperiority : public CvAIOperationMilitary
{
public:

	CvAIOperationNavalSuperiority(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_NAVAL_SUPERIORITY, ARMY_TYPE_NAVAL) {}
	virtual ~CvAIOperationNavalSuperiority() {}

	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefendCity
//!  \brief		Defend a specific city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefendCity : public CvAIOperationMilitary
{
public:

	CvAIOperationDefendCity(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_CITY_DEFENSE,ARMY_TYPE_LAND) {}
	virtual ~CvAIOperationDefendCity() {}

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDefenseRapidResponse
//!  \brief		Mobile force that can defend where threatened
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDefenseRapidResponse : public CvAIOperationMilitary
{
public:

	CvAIOperationDefenseRapidResponse(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationMilitary(iID,eOwner,eEnemy,AI_OPERATION_RAPID_RESPONSE,ARMY_TYPE_LAND) {}
	virtual ~CvAIOperationDefenseRapidResponse() {}

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

private:
	virtual CvPlot* FindBestTarget(CvPlot** ppMuster) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilian
//!  \brief		Base class for operations that are one military unit and one civilian
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilian : public CvAIOperation
{
public:

	CvAIOperationCivilian(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, AIOperationTypes eType, UnitAITypes eCivilianType) : 
		CvAIOperation(iID, eOwner, eEnemy, eType, ARMY_TYPE_ESCORT_LAND), m_eCivilianType(eCivilianType) {}
	virtual ~CvAIOperationCivilian() {}

	virtual void Init(CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetDeployRange() const { return 1; }
	virtual bool CheckTransitionToNextStage();

	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit) = 0;

	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
	virtual bool IsEscorted() const;
	virtual bool IsOffensive() const { return false; }

	virtual UnitAITypes GetCivilianType() const
	{
		return m_eCivilianType;
	}

	//we have arrived. subclass needs to decide what happens
	virtual bool PerformMission(CvUnit* pUnit) = 0;

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

protected:
	virtual CvUnit* FindBestCivilian();
	virtual bool RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy);

	UnitAITypes m_eCivilianType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianFoundCity
//!  \brief		Find a place to utilize a new settler
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianFoundCity : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianFoundCity(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationCivilian(iID,eOwner,eEnemy,AI_OPERATION_FOUND_CITY, UNITAI_SETTLE) {}
	virtual ~CvAIOperationCivilianFoundCity() {}

	virtual bool PerformMission(CvUnit* pUnit);
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);

protected:
	virtual CvUnit* FindBestCivilian();
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit);
	virtual void	LogSettleTarget(const char* hint, CvPlot* pTarget) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianConcertTour
//!  \brief		Send a Great Musician to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianConcertTour : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianConcertTour(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationCivilian(iID,eOwner,eEnemy,AI_OPERATION_MUSICIAN_CONCERT_TOUR, UNITAI_MUSICIAN) {}
	virtual ~CvAIOperationCivilianConcertTour() {}
	virtual bool PerformMission(CvUnit* pUnit);
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianMerchantDelegation
//!  \brief		Send a Great Merchant to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianMerchantDelegation : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianMerchantDelegation(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationCivilian(iID,eOwner,eEnemy,AI_OPERATION_MERCHANT_DELEGATION, UNITAI_MERCHANT) {}
	virtual ~CvAIOperationCivilianMerchantDelegation() {}
	virtual bool PerformMission(CvUnit* pUnit);
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCivilianDiplomatDelegation
//!  \brief		Send a diplomat to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCivilianDiplomatDelegation : public CvAIOperationCivilian
{
public:

	CvAIOperationCivilianDiplomatDelegation(int iID, PlayerTypes eOwner, PlayerTypes eEnemy) : 
		CvAIOperationCivilian(iID,eOwner,eEnemy,AI_OPERATION_DIPLOMAT_DELEGATION, UNITAI_DIPLOMAT) {}
	virtual ~CvAIOperationCivilianDiplomatDelegation() {}
	virtual bool PerformMission(CvUnit* pUnit);
	virtual AIOperationAbortReason VerifyOrAdjustTarget(CvArmyAI* pArmy);
private:
	virtual CvPlot* FindBestTargetForUnit(CvUnit* pUnit);
};


namespace OperationalAIHelpers
{
	MultiunitFormationTypes GetArmyFormationForOpType(AIOperationTypes eType);
	CvPlot* FindClosestBarbarianCamp(PlayerTypes ePlayer, CvPlot** ppMuster);
	CvPlot* FindEnemiesNearHomelandPlot(PlayerTypes ePlayer, PlayerTypes eEnemy, DomainTypes eDomain, CvPlot* pRefPlot, int iMaxDistance);
	bool IsSlotRequired(PlayerTypes ePlayer, const OperationSlot& thisOperationSlot);
	int IsUnitSuitableForRecruitment(CvUnit* pLoopUnit, const ReachablePlots& turnsFromMuster, CvPlot* pTarget,	bool bMustEmbark, bool bMustBeDeepWaterNaval, const vector<pair<size_t,CvFormationSlotEntry>>& availableSlots);
	CvCity* GetClosestFriendlyCoastalCity(PlayerTypes ePlayer, const CvPlot* pRefPlot);
	pair<CvCity*, CvCity*> GetClosestCoastalCityPair(PlayerTypes ePlayerA, PlayerTypes ePlayerB);
}

#endif

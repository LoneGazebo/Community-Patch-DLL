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
    AI_OPERATION_COLONIZE,
    AI_OPERATION_QUICK_COLONIZE,
    AI_OPERATION_NAVAL_ATTACK,
    AI_OPERATION_NAVAL_SNEAK_ATTACK,
    AI_OPERATION_CITY_STATE_ATTACK,
    AI_OPERATION_CITY_STATE_NAVAL_ATTACK,
    AI_OPERATION_NUKE_ATTACK,
	AI_OPERATION_PURE_NAVAL_CITY_ATTACK,
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
    AI_OPERATION_MOVETYPE_SINGLE_HEX,
    AI_OPERATION_MOVETYPE_ENEMY_TERRITORY,
    AI_OPERATION_MOVETYPE_NAVAL_ESCORT,
    AI_OPERATION_MOVETYPE_FREEFORM_NAVAL,
    AI_OPERATION_MOVETYPE_REBASE,
    AI_OPERATION_MOVETYPE_STATIC,
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
};

FDataStream& operator<<(FDataStream&, const AIOperationState&);
FDataStream& operator>>(FDataStream&, AIOperationState&);

struct OperationSlot
{
	OperationSlot()
	{
		m_iOperationID = -1;
		m_iArmyID = FFreeList::INVALID_INDEX;
		m_iSlotID = -1;
	}

	bool operator==(const OperationSlot& other) const
	{
		return (other.m_iArmyID == m_iArmyID && other.m_iSlotID == m_iSlotID && other.m_iOperationID == m_iOperationID);
	}

	bool IsValid() const
	{
		return (m_iArmyID != FFreeList::INVALID_INDEX && m_iSlotID != -1 && m_iOperationID != -1);
	}

	void Invalidate()
	{
		m_iOperationID = -1;
		m_iArmyID = FFreeList::INVALID_INDEX;
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
	static CvAIOperation* CreateOperation(AIOperationTypes eAIOperationType, PlayerTypes ePlayer);

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Uninit();
	virtual void Reset();

	AIOperationState GetOperationState() {return m_eCurrentState;};

	virtual int GetOperationType() const
	{
		return INVALID_AI_OPERATION;
	}
	virtual bool CanTacticalAIInterruptOperation() const
	{
		return false;
	}
	virtual bool NeedToCheckPathToTarget() const
	{
		return true;
	}
	virtual int GetMaximumRecruitTurns() const;
	virtual MultiunitFormationTypes GetFormation() const
	{
		return NO_MUFORMATION;
	}
	virtual AIOperationMovementType GetMoveType() const
	{
		return m_eMoveType;
	}

	int GetLastTurnMoved() const
	{
		return m_iLastTurnMoved;
	};
	void SetLastTurnMoved(int iValue)
	{
		m_iLastTurnMoved = iValue;
	};

	int GetID() const;
	void SetID(int iID);

	inline PlayerTypes GetOwner() const
	{
		return m_eOwner;
	}

	void SetOwner(PlayerTypes ePlayer);

	PlayerTypes GetEnemy() const;
	void SetEnemy(PlayerTypes eEnemy);

	int GetDefaultArea() const;
	void SetDefaultArea(int iDefaultArea);
	virtual bool IsAllNavalOperation() const
	{
		return false;
	};
	virtual bool IsMixedLandNavalOperation() const
	{
		return false;
	};

	CvPlot* GetTargetPlot() const;
	void SetTargetPlot(CvPlot* pTarget);
	CvPlot* GetMusterPlot() const;
	void SetMusterPlot(CvPlot* pTarget);
	CvPlot* GetStartCityPlot() const;
	void SetStartCityPlot(CvPlot* pTarget);
	virtual int GetGatherTolerance(CvArmyAI* pArmy, CvPlot* pPlot) const;

	virtual CvCity* GetOperationStartCity() const;

	int GetFirstArmyID();

	virtual bool CheckOnTarget();
	virtual bool ArmyInPosition(CvArmyAI* pArmy);
	virtual bool ArmyMoved(CvArmyAI* pArmy)
	{
		return false;
	};
	virtual int  PercentFromMusterPointToTarget();
	virtual bool ShouldAbort();
	virtual void SetToAbort(AIOperationAbortReason eReason);
	virtual void Kill(AIOperationAbortReason eReason );
	virtual void DoTurn();
	virtual bool DoDelayedDeath();

	virtual void BuildListOfUnitsWeStillNeedToBuild();
	unsigned int GetNumUnitsNeededToBeBuilt()
	{
		return m_viListOfUnitsWeStillNeedToBuild.size();
	}
	virtual OperationSlot PeekAtNextUnitToBuild(int iAreaID);
	virtual OperationSlot CommitToBuildNextUnit(int iAreaID, int iTurns, CvCity* pCity);
	virtual bool UncommitToBuild(OperationSlot thisOperationSlot);
	virtual bool FinishedBuilding(OperationSlot thisOperationSlot);
	virtual bool GrabUnitsFromTheReserves(CvPlot* pMusterPlot, CvPlot* pTargetPlot);
	bool DeleteArmyAI(int iID);
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvPlot* ComputeCenterOfMassForTurn(CvArmyAI* pArmy, CvPlot **ppClosestCurrentCOMonPath) const;

	bool HasOneMoreSlotToFill() const;
	bool BuyFinalUnit();

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	void LogOperationStart();
	void LogOperationStatus();
	void LogOperationSpecialMessage(const CvString& strMsg);
	void LogOperationEnd();

protected:
	CvString GetLogFileName(CvString& playerName) const;
	virtual CvString GetOperationName() const
	{
		return CvString("INVALID_AI_OPERATION");
	}

	virtual CvPlot* SelectInitialMusterPoint(CvArmyAI* pThisArmy);
	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired);

	std::vector<int> m_viArmyIDs;
	std::vector<OperationSlot> m_viListOfUnitsWeStillNeedToBuild;
	std::vector<OperationSlot> m_viListOfUnitsCitiesHaveCommittedToBuild;
	int m_iID;
	AIOperationState m_eCurrentState;
	AIOperationAbortReason m_eAbortReason;
	int m_iDefaultArea;
	int m_iLastTurnMoved;
	PlayerTypes m_eOwner;
	PlayerTypes m_eEnemy;
	bool m_bShouldReplaceLossesWithReinforcements;
	AIOperationMovementType m_eMoveType;

	// Coordinates of start city
	int m_iStartCityX;
	int m_iStartCityY;

	// Coordinates of muster plot
	int m_iMusterX;
	int m_iMusterY;

	// Coordinates of target plot
	int m_iTargetX;
	int m_iTargetY;
};

FDataStream& operator<<(FDataStream&, const AIOperationMovementType&);
FDataStream& operator>>(FDataStream&, AIOperationMovementType&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIEnemyTerritoryOperation
//!  \brief		Base class for operations that involve large collection of forces
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define SAFE_ESTIMATE_MAX_PATH_LEN 200

class CvAIEnemyTerritoryOperation : public CvAIOperation
{
public:
	CvAIEnemyTerritoryOperation();
	virtual ~CvAIEnemyTerritoryOperation();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetDeployRange() const;
	virtual int GetMaximumRecruitTurns() const;

protected:
	virtual CvPlot* FindBestTarget() = 0;
	virtual CvPlot* SelectInitialMusterPoint(CvArmyAI* pThisArmy);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationBasicCityAttack
//!  \brief		Attack a city with a small force
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationBasicCityAttack : public CvAIEnemyTerritoryOperation
{
public:

	CvAIOperationBasicCityAttack();
	virtual ~CvAIOperationBasicCityAttack();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_BASIC_CITY_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_BASIC_CITY_ATTACK");
	}
	virtual bool NeedToCheckPathToTarget() const
	{
		return false;
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return (GC.getGame().getHandicapInfo().GetID() > 4) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	}

	virtual bool ShouldAbort();
	virtual bool ArmyInPosition(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationSneakCityAttack
//!  \brief		Same as Basic City attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationSneakCityAttack : public CvAIOperationBasicCityAttack
{
public:
	CvAIOperationSneakCityAttack();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_SNEAK_CITY_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_SNEAK_CITY_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return (GC.getGame().getHandicapInfo().GetID() > 4) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	}
};

class CvAIOperationQuickSneakCityAttack : public CvAIOperationSneakCityAttack
{
public:
	CvAIOperationQuickSneakCityAttack();
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_EARLY_RUSH;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationSmallCityAttack
//!  \brief		Same as Basic City attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationSmallCityAttack : public CvAIOperationBasicCityAttack
{
public:
	CvAIOperationSmallCityAttack();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_SMALL_CITY_ATTACK;
	}
	virtual CvString GetOperationName() const
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
class CvAIOperationCityStateAttack : public CvAIOperationBasicCityAttack
{
public:
	CvAIOperationCityStateAttack();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_STATE_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_CITY_STATE_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CITY_STATE_ATTACK_FORCE;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationDestroyBarbarianCamp
//!  \brief		Send out a squad of units to take out a barbarian camp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationDestroyBarbarianCamp : public CvAIEnemyTerritoryOperation
{
public:

	CvAIOperationDestroyBarbarianCamp();
	virtual ~CvAIOperationDestroyBarbarianCamp();

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_DESTROY_BARBARIAN_CAMP;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_DESTROY_BARBARIAN_CAMP");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_ANTI_BARBARIAN_TEAM;
	}
	virtual int GetDeployRange() const;

	virtual bool ShouldAbort();
	virtual bool ArmyInPosition(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();
	bool m_bCivilianRescue;
	int m_iUnitToRescue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationPillageEnemy
//!  \brief		Create a fast strike team to harass the enemy
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationPillageEnemy : public CvAIEnemyTerritoryOperation
{
public:

	CvAIOperationPillageEnemy();
	virtual ~CvAIOperationPillageEnemy();

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_PILLAGE_ENEMY;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_PILLAGE_ENEMY");
	}
	virtual bool NeedToCheckPathToTarget() const
	{
		return false;
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_FAST_PILLAGERS;
	}
	virtual int GetDeployRange() const;

	virtual bool ArmyInPosition(CvArmyAI* pArmy);
	virtual bool ArmyMoved(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIEscortedOperation
//!  \brief		Base class for operations that are one military unit and one civilian
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIEscortedOperation : public CvAIOperation
{
public:

	CvAIEscortedOperation();
	virtual ~CvAIEscortedOperation();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	virtual CvUnit* FindBestCivilian();
	virtual CvPlot* FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths) = 0;

	bool RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy);

protected:
	bool m_bEscorted;
	UnitAITypes m_eCivilianType;
	int m_iTargetArea;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationFoundCity
//!  \brief		Find a place to utilize a new settler
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationFoundCity : public CvAIEscortedOperation
{
public:

	CvAIOperationFoundCity();
	virtual ~CvAIOperationFoundCity();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_FOUND_CITY;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_FOUND_CITY");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_SETTLER_ESCORT;
	}
	virtual bool ArmyInPosition(CvArmyAI* pArmy);
	virtual bool ShouldAbort();

private:
	virtual CvPlot* FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationQuickColonize
//!  \brief		Send a settler alone to a nearby island
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationQuickColonize : public CvAIOperationFoundCity
{
public:

	CvAIOperationQuickColonize();
	virtual ~CvAIOperationQuickColonize();
	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual int GetOperationType() const
	{
		return AI_OPERATION_QUICK_COLONIZE;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_QUICK_COLONIZE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_QUICK_COLONY_SETTLER;
	}
	virtual CvUnit* FindBestCivilian();

private:
	CvPlot* FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationMerchantDelegation
//!  \brief		Send a merchant to a city state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationMerchantDelegation : public CvAIEscortedOperation
{
public:

	CvAIOperationMerchantDelegation();
	virtual ~CvAIOperationMerchantDelegation();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_MERCHANT_DELEGATION;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_MERCHANT_DELEGATION");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_MERCHANT_ESCORT;
	}
	virtual bool ArmyInPosition(CvArmyAI* pArmy);

private:
	CvPlot* FindBestTarget(CvUnit* pUnit, bool bOnlySafePaths);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAINavalOperation
//!  \brief		Send out a squadron of naval units to bomb enemy forces on the coast
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAINavalOperation : public CvAIEnemyTerritoryOperation
{
public:

	CvAINavalOperation();
	virtual ~CvAINavalOperation();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL) = 0;
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_SQUADRON;
	}
	virtual int GetDeployRange() const;
	virtual CvCity* GetOperationStartCity() const;
	virtual bool IsAllNavalOperation() const
	{
		return true;
	};

	virtual bool ArmyInPosition(CvArmyAI* pArmy) = 0;
	virtual int GetMaximumRecruitTurns() const {return MAX_INT;};   // Let naval units come from afar

protected:
	virtual CvPlot* SelectInitialMusterPoint(CvArmyAI* pThisArmy);
	virtual CvUnit* FindInitialUnit();
	virtual CvPlot* FindBestTarget() = 0;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalBombardment
//!  \brief		Send out a squadron of naval units to bomb enemy forces on the coast
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalBombardment : public CvAINavalOperation
{
public:

	CvAIOperationNavalBombardment();
	virtual ~CvAIOperationNavalBombardment();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_BOMBARDMENT;
	}
	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_BOMBARDMENT;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_BOMBARDMENT");
	}

	virtual bool ArmyInPosition(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalSuperiority
//!  \brief		Send out a squadron of naval units to rule the seas
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalSuperiority : public CvAINavalOperation
{
public:

	CvAIOperationNavalSuperiority();
	virtual ~CvAIOperationNavalSuperiority();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_SUPERIORITY;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_SUPERIORITY");
	}
	virtual bool CanTacticalAIInterruptOperation() const
	{
		return true;
	}

	virtual bool ArmyInPosition(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationPureNavalCityAttack
//!  \brief		Try to take out an enemy city from the sea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationPureNavalCityAttack : public CvAINavalOperation
{
public:

	CvAIOperationPureNavalCityAttack();
	virtual ~CvAIOperationPureNavalCityAttack();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_PURE_NAVAL_CITY_ATTACK;
	}
	virtual int GetOperationType() const
	{
		return AI_OPERATION_PURE_NAVAL_CITY_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_PURE_NAVAL_CITY_ATTACK");
	}
	virtual bool CanTacticalAIInterruptOperation() const
	{
		return true;
	}
	virtual int GetDeployRange() const;

	virtual bool ShouldAbort();
	virtual bool ArmyInPosition(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationCityCloseDefense
//!  \brief		Defend a specific city
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationCityCloseDefense : public CvAIOperation
{
public:

	CvAIOperationCityCloseDefense();
	virtual ~CvAIOperationCityCloseDefense();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_CLOSE_DEFENSE;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_CITY_CLOSE_DEFENSE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_CLOSE_CITY_DEFENSE;
	}

private:
	CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationRapidResponse
//!  \brief		Mobile force that can defend where threatened
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationRapidResponse : public CvAIOperation
{
public:

	CvAIOperationRapidResponse();
	virtual ~CvAIOperationRapidResponse();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_RAPID_RESPONSE;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_RAPID_RESPONSE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_RAPID_RESPONSE_FORCE;
	}

	virtual bool ArmyInPosition(CvArmyAI* pArmy);
	virtual bool ArmyMoved(CvArmyAI* pArmy);

private:
	bool RetargetDefensiveArmy(CvArmyAI* pArmy);
	CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAINavalEscortedOperation
//!  \brief		Base class for operations that require a naval escort for land units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAINavalEscortedOperation : public CvAIOperation
{
public:

	CvAINavalEscortedOperation();
	virtual ~CvAINavalEscortedOperation();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_COLONIZE;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_COLONIZE");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_COLONIZATION_PARTY;
	}
	virtual CvCity* GetOperationStartCity() const;
	virtual bool IsMixedLandNavalOperation() const
	{
		return true;
	};
	virtual bool IsCivilianRequired() const
	{
		return true;
	};

	virtual bool ArmyInPosition(CvArmyAI* pArmy);
	virtual CvUnit* FindBestCivilian();
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);
	bool RetargetCivilian(CvUnit* pCivilian, CvArmyAI* pArmy);

protected:
	CvPlot* FindBestTarget(CvUnit* pUnit);

	UnitAITypes m_eCivilianType;
	int m_iInitialAreaID;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalAttack
//!  \brief		Attack a city from the sea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalAttack : public CvAINavalEscortedOperation
{
public:

	CvAIOperationNavalAttack();
	virtual ~CvAIOperationNavalAttack();

	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_INVASION;
	}
	virtual CvCity* GetOperationStartCity() const;
	virtual bool IsCivilianRequired() const
	{
		return false;
	};

	virtual bool ArmyInPosition(CvArmyAI* pArmy);
	virtual void UnitWasRemoved(int iArmyID, int iSlotID);

protected:
	virtual CvPlot* FindBestTarget();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalSneakAttack
//!  \brief		Same as basic naval attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalSneakAttack : public CvAIOperationNavalAttack
{
public:
	CvAIOperationNavalSneakAttack();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NAVAL_SNEAK_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_NAVAL_SNEAK_ATTACK");
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NAVAL_INVASION;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAIOperationNavalCityStateAttack
//!  \brief		Same as basic naval attack except allowed when not at war
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAIOperationNavalCityStateAttack : public CvAIOperationNavalAttack
{
public:
	CvAIOperationNavalCityStateAttack();

	virtual int GetOperationType() const
	{
		return AI_OPERATION_CITY_STATE_NAVAL_ATTACK;
	}
	virtual CvString GetOperationName() const
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
class CvAIOperationNukeAttack : public CvAIOperation
{
public:

	CvAIOperationNukeAttack();
	virtual ~CvAIOperationNukeAttack();
	virtual void Init(int iID, PlayerTypes eOwner, PlayerTypes eEnemy, int iDefaultArea = -1, CvCity* pTarget = NULL, CvCity* pMuster = NULL);

	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	virtual int GetOperationType() const
	{
		return AI_OPERATION_NUKE_ATTACK;
	}
	virtual CvString GetOperationName() const
	{
		return CvString("AI_OPERATION_NUKE_ATTACK");
	}
	virtual bool NeedToCheckPathToTarget() const
	{
		return false;
	}
	virtual MultiunitFormationTypes GetFormation() const
	{
		return MUFORMATION_NUKE_ATTACK;
	}
	virtual int GetDeployRange() const
	{
		return 10;
	}
	virtual bool FindBestFitReserveUnit(OperationSlot thisOperationSlot, CvPlot* pMusterPlot, CvPlot* pTargetPlot, bool* bRequired);

	virtual CvCity* GetOperationStartCity() const;
	virtual bool ArmyInPosition(CvArmyAI* pArmy);

protected:
	virtual CvPlot* FindBestTarget();

	int m_iBestUnitID;
};


namespace OperationalAIHelpers
{
int GetGatherRangeForXUnits(int iTotalUnits);
}

#endif

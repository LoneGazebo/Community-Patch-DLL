/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// CvArmyAI.h

#ifndef CIV5_ARMYAI_H
#define CIV5_ARMYAI_H

class CvPlot;

class CvArmyFormationSlot
{
public:
	CvArmyFormationSlot()
	{
		m_iUnitID = -1;
		m_iEstimatedTurnsToCheckpoint = -1;
		m_iPrevEstimatedTurnsToCheckpoint = -1;
	};

	int GetUnitID() const
	{
		return m_iUnitID;
	};
	void SetUnitID(int iValue)
	{
		m_iUnitID = iValue;
	};
	int GetTurnsToCheckpoint() const
	{
		return m_iEstimatedTurnsToCheckpoint;
	};
	void SetTurnsToCheckpoint(int iValue)
	{
		m_iPrevEstimatedTurnsToCheckpoint = m_iEstimatedTurnsToCheckpoint;
		m_iEstimatedTurnsToCheckpoint = iValue;
	};

	bool IsMakingProgressTowardsCheckpoint(int iNextValue);

	bool IsFree() const { return m_iUnitID == -1; }
	bool IsUsed() const { return m_iUnitID >= 0; }

	int m_iUnitID;
	int m_iEstimatedTurnsToCheckpoint;
	int m_iPrevEstimatedTurnsToCheckpoint;
};

enum ArmyAIState
{
    NO_ARMYAISTATE = -1,
    ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE,
    ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP,
    ARMYAISTATE_MOVING_TO_DESTINATION,
    ARMYAISTATE_AT_DESTINATION,
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvArmyAI
//!  \brief		One army in an operational maneuver
//
//!  Key Attributes:
//!  - Manages the turn-by-turn movement of an army
//!  - Created and owned by one AI operation
//!  - Uses step path finder to find muster points before it has units
//!  - Uses main path finder to plot route once it has units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArmyAI
{

public:

	CvArmyAI();
	virtual ~CvArmyAI();

	// Initialization/destruction routines
	void ReleaseAllUnits();
	void Reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, int iOperationID = -1);
	void Kill();

	// for serialization
	virtual void Read(FDataStream& kStream);
	virtual void Write(FDataStream& kStream) const;

	// Accessors
	int GetID();
	void SetID(int iID);

	TeamTypes GetTeam() const;

	ArmyAIState GetArmyAIState() const;
	void SetArmyAIState(ArmyAIState eNewArmyAIState);

	int GetMovementRate();
	CvPlot* GetCenterOfMass(bool bClampToUnit=true, float* pfVarX=NULL, float* pfVarY=NULL);
	int GetFurthestUnitDistance(CvPlot* pPlot);

	void SetOwner(PlayerTypes eOwner) { m_eOwner=eOwner; }
	inline PlayerTypes GetOwner() const { return m_eOwner; }
	void SetOperationID(int iID) { m_iOperationID = iID; }
	inline int GetOperationID() const { return m_iOperationID; }

	// Formation accessors
	int GetFormationIndex() const;
	void SetFormationIndex(int iFormationIndex);
	int GetNumFormationEntries() const;
	int GetNumSlotsFilled() const;
	CvArmyFormationSlot* GetFormationSlot(int iSlotID) { return &m_FormationEntries[iSlotID]; }

	int GetTurnOfLastUnitAtNextCheckpoint() const;
	void UpdateCheckpointTurnsAndRemoveBadUnits();
	int GetUnitsOfType(MultiunitPositionTypes ePosition) const;
	bool IsAllOceanGoing();

	//Army strength accessors
	int GetTotalPower();

	// Position accessors
	int GetX() const;
	int GetY() const;
	void SetXY(int iX, int iY);
	CvPlot* GetCurrentPlot() const;
	int GetArea() const;
	DomainTypes GetDomainType() const;
	void SetDomainType(DomainTypes domainType);

	// Goal accessors
	void SetGoalPlot(CvPlot* pGoalPlot);
	CvPlot* GetGoalPlot() const;
	int GetGoalX() const;
	int GetGoalY() const;

#if defined(MOD_BALANCE_CORE)
	//Water parameters
	void SetOceanMoves(bool bValue);
	bool NeedOceanMoves() const;
#endif

	// Unit handling
	void AddUnit(int iUnitId, int iSlotNum);
	bool RemoveUnit(int iUnitId);
	CvUnit* GetFirstUnit();
	CvUnit* GetNextUnit(CvUnit* pCurUnit);

	// Per turn processing
	bool IsDelayedDeath();

protected:
	int m_iID;
	PlayerTypes m_eOwner;
	int m_iOperationID;
	int m_iCurrentX;
	int m_iCurrentY;
	int m_iGoalX;
	int m_iGoalY;
#if defined(MOD_BALANCE_CORE)
	bool m_bOceanMoves;
#endif
	int m_eDomainType; // DomainTypes
	int m_iFormationIndex;
	int m_eAIState; // ArmyAIState
	vector<CvArmyFormationSlot> m_FormationEntries;
	vector<CvArmyFormationSlot>::iterator m_CurUnitIter;
};

FDataStream& operator<<(FDataStream&, const CvArmyAI&);
FDataStream& operator>>(FDataStream&, CvArmyAI&);

FDataStream& operator<<(FDataStream&, const CvArmyFormationSlot&);
FDataStream& operator>>(FDataStream&, CvArmyFormationSlot&);

#endif

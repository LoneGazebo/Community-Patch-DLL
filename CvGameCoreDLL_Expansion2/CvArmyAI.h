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

#include <deque>
class CvPlot;

class CvArmyFormationSlot
{
public:
	CvArmyFormationSlot(int iUnitID=-1, bool bIsRequired=false)
	{
		m_iUnitID = iUnitID;
		m_bIsRequired = bIsRequired;
	};

	void Clear()
	{
		m_iUnitID = -1;
		m_estTurnsToCheckpoint.clear();
		m_bIsRequired = false;
	}

	int GetUnitID() const
	{
		return m_iUnitID;
	};

	int GetTurnsToCheckpoint(size_t iIndex) const
	{
		if (iIndex < m_estTurnsToCheckpoint.size())
			return m_estTurnsToCheckpoint[iIndex];
		
		return -1;
	};

	void SetCurrentTurnsToCheckpoint(int iValue)
	{
		m_estTurnsToCheckpoint.push_front(iValue);
		if (m_estTurnsToCheckpoint.size()>3)
			m_estTurnsToCheckpoint.pop_back();
	};

	void ResetTurnsToCheckpoint();
	bool IsMakingProgressTowardsCheckpoint() const;

	bool IsFree() const { return m_iUnitID == -1; }
	bool IsUsed() const { return m_iUnitID >= 0; }
	bool IsRequired() const { return m_bIsRequired; }

	friend FDataStream& operator<<(FDataStream& saveTo, const CvArmyFormationSlot& readFrom);
	friend FDataStream& operator>>(FDataStream& loadFrom, CvArmyFormationSlot& writeTo);

protected:
	int m_iUnitID;
	std::deque<int> m_estTurnsToCheckpoint;
	bool m_bIsRequired;
};

enum ArmyAIState
{
    NO_ARMYAISTATE = -1,
    ARMYAISTATE_WAITING_FOR_UNITS_TO_REINFORCE,
    ARMYAISTATE_WAITING_FOR_UNITS_TO_CATCH_UP,
    ARMYAISTATE_MOVING_TO_DESTINATION,
    ARMYAISTATE_AT_DESTINATION,
};

FDataStream& operator<<(FDataStream&, const ArmyAIState&);
FDataStream& operator>>(FDataStream&, ArmyAIState&);

FDataStream& operator<<(FDataStream&, const MultiunitFormationTypes&);
FDataStream& operator>>(FDataStream&, MultiunitFormationTypes&);

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
	template<typename ArmyAI, typename Visitor>
	static void Serialize(ArmyAI& armyAI, Visitor& visitor);
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
	int GetClosestUnitDistance(CvPlot* pPlot);
	int GetFurthestUnitDistance(CvPlot* pPlot);

	void SetOwner(PlayerTypes eOwner) { m_eOwner=eOwner; }
	inline PlayerTypes GetOwner() const { return m_eOwner; }
	void SetOperationID(int iID) { m_iOperationID = iID; }
	inline int GetOperationID() const { return m_iOperationID; }

	// Formation accessors
	CvMultiUnitFormationInfo* GetFormation() const;
	void SetFormation(MultiunitFormationTypes eFormation);
	size_t GetNumFormationEntries() const;
	size_t GetNumSlotsFilled() const;
	CvArmyFormationSlot* GetSlotStatus(size_t iSlotID) { return &m_FormationEntries[iSlotID]; }
	const vector<CvArmyFormationSlot>& GetSlotStatus() const { return m_FormationEntries; }
	CvFormationSlotEntry GetSlotInfo(size_t iSlotID) const;
	vector<size_t> GetOpenSlots(bool bRequiredOnly) const;
	DomainTypes GetDomainType() const;
	void SetType(ArmyType eType);
	ArmyType GetType() const;

	void UpdateCheckpointTurnsAndRemoveBadUnits();
	bool IsAllOceanGoing();
	CvPlot* GetCurrentPlot();

	//Army strength accessors
	int GetTotalPower();

	// Goal accessors
	void SetGoalPlot(CvPlot* pGoalPlot);
	CvPlot* GetGoalPlot() const;

	// Unit handling
	void AddUnit(int iUnitId, int iSlotNum, bool bIsRequired);
	int RemoveUnit(int iUnitId, bool bTemporary);
	CvUnit* GetFirstUnit();
	CvUnit* GetNextUnit(CvUnit* pCurUnit);

	// Per turn processing
	bool IsDelayedDeath();

protected:
	int m_iID;
	PlayerTypes m_eOwner;
	int m_iOperationID;
	int m_iGoalX;
	int m_iGoalY;

	ArmyType					m_eType;
	MultiunitFormationTypes		m_eFormation;
	ArmyAIState					m_eAIState;
	vector<CvArmyFormationSlot> m_FormationEntries;
};

FDataStream& operator<<(FDataStream&, const CvArmyAI&);
FDataStream& operator>>(FDataStream&, CvArmyAI&);

FDataStream& operator<<(FDataStream&, const CvArmyFormationSlot&);
FDataStream& operator>>(FDataStream&, CvArmyFormationSlot&);

namespace OperationalAIHelpers
{
	bool HaveEnoughUnits(const vector<CvArmyFormationSlot>& slotStatus, int iMaxMissingUnits);
}

#endif

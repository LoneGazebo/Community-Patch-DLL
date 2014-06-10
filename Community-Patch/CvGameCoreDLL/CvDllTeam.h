/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllTeam : public ICvTeam1
{
public:
	CvDllTeam(_In_ CvTeam* pTeam);
	~CvDllTeam();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvTeam* GetInstance();

	bool DLLCALL CanEmbarkAllWaterPassage() const;
	int DLLCALL GetAtWarCount(bool bIgnoreMinors) const;
	EraTypes DLLCALL GetCurrentEra() const;
	PlayerTypes DLLCALL GetLeaderID() const;
	int DLLCALL GetProjectCount(ProjectTypes eIndex) const;
	int DLLCALL GetTotalSecuredVotes() const;
	void DLLCALL Init(TeamTypes eID);
	bool DLLCALL IsAlive() const;
	bool DLLCALL IsAtWar(TeamTypes eIndex) const;
	bool DLLCALL IsBarbarian() const;
	bool DLLCALL IsBridgeBuilding() const;
	bool DLLCALL IsHasMet(TeamTypes eIndex) const;
	bool DLLCALL IsHomeOfUnitedNations() const;
	void DLLCALL Uninit();
	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;

	//! Get the number of Techs the player has fully researched
	virtual int DLLCALL GetTechCount() const;
	//! Get all the techs the player has researched
	virtual int DLLCALL GetTechs(TechTypes* pkTechArray, uint uiArraySize) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvTeam* m_pTeam;
};
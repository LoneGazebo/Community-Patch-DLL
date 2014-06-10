/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllCity : public ICvCity1
{
public:
	CvDllCity(_In_ CvCity* pCity);
	~CvDllCity();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvCity* GetInstance();

	int DLLCALL GetID() const;
	PlayerTypes DLLCALL GetOwner() const;

	int DLLCALL GetMaxHitPoints() const;
	void DLLCALL GetPosition(int& iX, int& iY) const;

	ICvPlot1* DLLCALL Plot() const;

	int DLLCALL GetPopulation() const;

	const char* DLLCALL GetName() const;

	bool DLLCALL CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost = false);
	CitySizeTypes DLLCALL GetCitySizeType() const;

	const char* DLLCALL GetCityBombardEffectTag() const;

	TeamTypes DLLCALL GetTeam() const;

	bool DLLCALL IsPuppet() const;

	int DLLCALL GetX() const;
	int DLLCALL GetY() const;

	int DLLCALL GetStrengthValue(bool bForRangeStrike = false) const;
	int DLLCALL FoodDifference(bool bBottom = true) const;
	int DLLCALL GetFoodTurnsLeft() const;
	int DLLCALL GetYieldRate(int eIndex) const;
	int DLLCALL GetJONSCulturePerTurn() const;
	IDInfo DLLCALL GetIDInfo() const;

	bool DLLCALL IsWorkingPlot(ICvPlot1* pPlot) const;
	bool DLLCALL CanWork(ICvPlot1* pPlot) const;
	ICvPlot1* DLLCALL GetCityPlotFromIndex(int iIndex) const;

	FAutoArchive& DLLCALL GetSyncArchive();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvCity* m_pCity;
};
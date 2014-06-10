/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllPlot : public ICvPlot1
{
public:
	CvDllPlot(_In_ CvPlot* pPlot);
	~CvDllPlot();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvPlot* GetInstance();

	TeamTypes DLLCALL GetTeam() const;
	FogOfWarModeTypes DLLCALL GetActiveFogOfWarMode() const;
	void DLLCALL UpdateCenterUnit();
	bool DLLCALL IsAdjacent(ICvPlot1* pPlot) const;
	bool DLLCALL IsRiver() const;
	ICvPlot1* DLLCALL GetNeighboringPlot(DirectionTypes eDirection) const;
	int DLLCALL GetBuildTime(BuildTypes eBuild, PlayerTypes ePlayer) const;
	bool DLLCALL IsVisible(TeamTypes eTeam, bool bDebug) const;
	bool DLLCALL IsCity() const;
	bool DLLCALL IsEnemyCity(ICvUnit1* pUnit) const;
	bool DLLCALL IsFighting() const;
	bool DLLCALL IsTradeRoute(PlayerTypes ePlayer = NO_PLAYER) const;
	bool DLLCALL IsImpassable() const;
	void DLLCALL GetPosition(int& iX, int& iY) const;
	bool DLLCALL IsNEOfRiver() const;
	bool DLLCALL IsWOfRiver() const;
	bool DLLCALL IsNWOfRiver() const;
	FlowDirectionTypes DLLCALL GetRiverEFlowDirection() const;
	FlowDirectionTypes DLLCALL GetRiverSEFlowDirection() const;
	FlowDirectionTypes DLLCALL GetRiverSWFlowDirection() const;
	PlayerTypes DLLCALL GetOwner() const;
	PlotTypes DLLCALL GetPlotType() const;
	bool DLLCALL IsWater() const;
	bool DLLCALL IsHills() const;
	bool DLLCALL IsOpenGround() const;
	bool DLLCALL IsMountain() const;
	TerrainTypes DLLCALL GetTerrainType() const;
	FeatureTypes DLLCALL GetFeatureType() const;
	ResourceTypes DLLCALL GetResourceType(TeamTypes eTeam = NO_TEAM) const;
	int DLLCALL GetNumResource() const;
	ImprovementTypes DLLCALL GetImprovementType() const;
	bool DLLCALL IsImprovementPillaged() const;
	GenericWorldAnchorTypes DLLCALL GetWorldAnchor() const;
	int DLLCALL GetWorldAnchorData() const;
	RouteTypes DLLCALL GetRouteType() const;
	bool DLLCALL IsRoutePillaged() const;
	ICvCity1* DLLCALL GetPlotCity() const;
	ICvCity1* DLLCALL GetWorkingCity() const;
	bool DLLCALL IsRevealed(TeamTypes eTeam, bool bDebug) const;
	ImprovementTypes DLLCALL GetRevealedImprovementType(TeamTypes eTeam, bool bDebug) const;
	int DLLCALL GetBuildProgress(BuildTypes eBuild) const;
	bool DLLCALL GetAnyBuildProgress() const;
	void DLLCALL UpdateLayout(bool bDebug);
	ICvUnit1* DLLCALL GetCenterUnit();
	int DLLCALL GetNumUnits() const;
	ICvUnit1* DLLCALL GetUnitByIndex(int iIndex) const;
	void DLLCALL AddUnit(ICvUnit1* pUnit, bool bUpdate = true);
	void DLLCALL RemoveUnit(ICvUnit1* pUnit, bool bUpdate = true);
	const IDInfo* DLLCALL NextUnitNode(const IDInfo* pNode) const;
	IDInfo* DLLCALL NextUnitNode(IDInfo* pNode);
	IDInfo* DLLCALL HeadUnitNode();
	int DLLCALL GetPlotIndex() const;
	char DLLCALL GetContinentType() const;

	FAutoArchive& DLLCALL GetSyncArchive();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvPlot* m_pPlot;
};
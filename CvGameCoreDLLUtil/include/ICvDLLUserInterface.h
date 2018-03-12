#pragma once


// {02891D70-8B76-412a-A8A5-5363DBF0B22B}
static const GUID guidICvUserInterface1 = 
{ 0x2891d70, 0x8b76, 0x412a, { 0xa8, 0xa5, 0x53, 0x63, 0xdb, 0xf0, 0xb2, 0x2b } };

// {9EB813D2-A2EE-4fbf-9433-4A8A811922EC}
static const GUID guidICvUserInterface2 = 
{ 0x9eb813d2, 0xa2ee, 0x4fbf, { 0x94, 0x33, 0x4a, 0x8a, 0x81, 0x19, 0x22, 0xec } };


//
// abstract class containing InterfaceBuddy functions that the DLL needs
//
#include "LinkedList.h"

class ICvEnumerator;
class ICvCity1;
class ICvDeal1;

typedef std::list<CvPopupInfo*> CvPopupQueue;

//This interface has been deprecated and should no longer be used. 
//Please use the versioned interfaces instead.
//PROGRAMMERS: This interface is locked, do not modify this interface in ANY way.
class CvDLLInterfaceIFaceBase
{
public:
	enum TURN_STATUS_TYPE {
		TURN_START = 0,
		TURN_END = 1,
		TURN_STEP = 2
	};

	virtual void lookAtSelectionPlot(bool bRelease = false) = 0;

	//virtual bool canHandleAction(int iAction, ICvPlot1* pPlot = NULL, bool bTestVisible = false) = 0;
	virtual bool CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisible = false) = 0;

	_Ret_maybenull_ virtual ICvPlot1* getSelectionPlot() = 0;
	virtual ICvUnit1* GetHeadSelectedUnit() = 0;
	virtual bool DoAutoUnitCycle() = 0;
	
	virtual ICvDeal1* GetScratchDeal() = 0;
	virtual void SetScratchDeal(ICvDeal1* pDeal) = 0;

	virtual bool isPopupUp() = 0;
	virtual bool isPopupQueued() = 0;
	virtual bool isDiploOrPopupWaiting() = 0;
	virtual bool isDiploActive() = 0;

	virtual bool IsUnitSelected(PlayerTypes playerID, int unitID) = 0;

	virtual void selectUnit(ICvUnit1* pUnit, bool bClear, bool bToggle = false, bool bSound = false) = 0;
	virtual void selectGroup(ICvUnit1* pUnit, bool bShift, bool bCtrl, bool bAlt) = 0;
	virtual void selectAll(ICvPlot1* pPlot) = 0;

	virtual bool RemoveFromSelectionList(ICvUnit1* pUnit) = 0;
	virtual void makeSelectionListDirty() = 0;
	virtual bool CanSelectionListFound() = 0;

	virtual ICvUnit1* getLastSelectedUnit() = 0;
	virtual void setLastSelectedUnit(ICvUnit1* pUnit) = 0;
	virtual void changePlotListColumn(int iChange) = 0;
	virtual int getOriginalPlotIndex() = 0;

	virtual void ClearSelectionList() = 0;
	virtual void InsertIntoSelectionList(ICvUnit1* pUnit, bool bClear, bool bToggle, bool bGroup = false, bool bSound = false, bool bMinimalChange = false) = 0;
	virtual int GetLengthSelectionList() = 0;
	virtual void VisuallyDeselectUnit(ICvUnit1* pUnit) = 0;
	virtual ICvEnumerator* GetSelectedUnits() = 0;

	virtual void selectCity(ICvCity1* pNewValue, bool bTestProduction = false) = 0;
	virtual void selectLookAtCity(bool bAdd = false) = 0;
	virtual void addSelectedCity(ICvCity1* pNewValue, bool bToggle = false) = 0;
	virtual void clearSelectedCities() = 0;
	virtual bool isCitySelected(ICvCity1 *pCity) = 0;
	virtual ICvCity1* getHeadSelectedCity() = 0;
	virtual bool isCitySelection() = 0;
	virtual IDInfo* nextSelectedCitiesNode(IDInfo* pNode) = 0;
	virtual const IDInfo* nextSelectedCitiesNode(const IDInfo* pNode) const = 0;
	virtual IDInfo* headSelectedCitiesNode() = 0;
	virtual void updateCityScreen(ICvCity1* pOldCity = NULL) = 0;

	virtual uint AddMessage(uint uiParentEvent, PlayerTypes ePlayer, bool bForce, int iLength, const char* strString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual uint AddUnitMessage(uint uiParentEvent, const IDInfo& unitID, PlayerTypes ePlayer, bool bForce, int iLength, const char* szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual uint AddPlotMessage(uint uiParentEvent, int iPlotID, PlayerTypes ePlayer, bool bForce, int iLength, const char* szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual uint AddCityMessage(uint uiParentEvent, const IDInfo& cityID, PlayerTypes ePlayer, bool bForce, int iLength, const char* szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual void clearEventMessages() = 0;
	virtual void AddPopup(const CvPopupInfo& kPopup) = 0;
	virtual void AddPopupText( int x, int y, const char *szText, float fDelay = 0 ) = 0;
	virtual void PublishActivePlayerTurnStart() = 0;	// Only the 'active' (local human) player
    virtual void PublishActivePlayerTurnEnd() = 0;		// Only the 'active' (local human) player
	virtual void PublishRemotePlayerTurnEnd() = 0;
	virtual void PublishPlayerTurnStatus(TURN_STATUS_TYPE eStatus, PlayerTypes ePlayer, const char* pszTag = NULL) = 0;
	virtual void AddNotification( int iID, NotificationTypes type, const char *cTooltip, const char* cSummary, int iGameData, int iExtraGameData, PlayerTypes ePlayer, int iX = -1, int iY = -1) = 0;
	virtual void RemoveNotification( int iID, PlayerTypes ePlayer ) = 0;
	virtual void ActivateNotification (int iID, NotificationTypes type, const char* cString, int iX, int iY, int iGameIndex, int iExtraGameDataValue, PlayerTypes ePlayer) = 0;

	virtual int getCycleSelectionCounter() = 0;
	virtual void setCycleSelectionCounter(int iNewValue) = 0;
	virtual void changeCycleSelectionCounter(int iChange) = 0;

	virtual bool IsSelectedUnitRevealingNewPlots() const = 0;
	virtual void SetSelectedUnitRevealingNewPlots(bool bValue) = 0;

	virtual int getEndTurnCounter() = 0;
	virtual void setEndTurnCounter(int iNewValue) = 0;
	virtual void changeEndTurnCounter(int iChange) = 0;

	virtual bool isCombatFocus() = 0;
	virtual void setCombatFocus(bool bNewValue) = 0;

	virtual bool isDirty(InterfaceDirtyBits eDirtyItem) = 0;
	virtual void setDirty(InterfaceDirtyBits eDirtyItem, bool bNewValue) = 0;
	virtual void makeInterfaceDirty() = 0;
	virtual bool updateCursorType() = 0;

	virtual void lookAt(const ICvPlot1* pPlot, CameraLookAtTypes type) = 0;
	virtual void centerCamera(ICvUnit1*) = 0;
	virtual void releaseLockedCamera() = 0;
	virtual bool isFocusedWidget() = 0;
	virtual bool isFocused() = 0;
	virtual void refreshYieldVisibleMode() = 0;
	virtual void toggleYieldVisibleMode() = 0;
	virtual void toggleResourceVisibleMode() = 0;
	virtual void toggleGridVisibleMode() = 0;

	virtual int getOriginalPlotCount() = 0;
	virtual bool isCityScreenUp() = 0;
	virtual void setCityScreenUp(bool bUp) = 0;

	virtual void setNoSelectionListCycle(bool bValue) = 0;

	virtual bool canEndTurn() const = 0;
	virtual void UpdateEndTurn() = 0;
	virtual void SetEndTurnBlockingChanged(EndTurnBlockingTypes ePrevBlockingType, EndTurnBlockingTypes eNewBlockingType) = 0;
	virtual void setInterfaceMode(InterfaceModeTypes eNewValue) = 0;
	virtual InterfaceModeTypes getInterfaceMode() = 0;
	virtual int GetInterfaceModeValue() = 0;
	virtual void setFlashing(PlayerTypes eWho, bool bFlashing = true) = 0;
	virtual bool isFlashing(PlayerTypes eWho) = 0;
	virtual void setDiplomacyLocked(bool bLocked) = 0;
	virtual bool isDiplomacyLocked() = 0;

	virtual void setCanEndTurn(bool bNewValue) = 0;
	virtual void updateEndTurnTimer(float percentComplete) const = 0;
	virtual bool waitingForRemotePlayers() const = 0;

	virtual bool isHasMovedUnit() = 0;
	virtual void setHasMovedUnit(bool bNewValue) = 0;

	virtual bool isForcePopup() = 0;
	virtual void setForcePopup(bool bNewValue) = 0;

	virtual void lookAtCityOffset(int iCity) = 0;

	virtual int getPlotListColumn() = 0;
	virtual void verifyPlotListColumn() = 0;
	virtual int getPlotListOffset() = 0;

	virtual void exitingToMainMenu(const char* szLoadFile=NULL) = 0;
	
	virtual bool isInAdvancedStart() const = 0;
	virtual void setInAdvancedStart(bool bAdvancedStart) = 0;

	virtual void setBusy(bool bBusy) = 0;

	virtual void UpdateCountryBorder(ICvPlot1* pThisPlot) = 0;
	virtual void UpdateAllCountryBorders() = 0;

	virtual void DebugCityRoutePlots (ICvPlot1* pPlot, bool bOn) = 0;

	virtual void DirectionAction(DirectionTypes eDirection) = 0;

	virtual void SetSpecificCityInfoDirty(ICvCity1* pCity, CityUpdateTypes eUpdateType) = 0;

	virtual void AddHexToUIRange(ICvPlot1* pThisPlot) = 0;

	virtual void OpenEndGameMenu() = 0;

	virtual void doTurn() = 0;

	virtual bool IsOptionNoRewardPopups() = 0;

	virtual bool IsAIRequestingConcessions() const = 0;
	virtual void SetAIRequestingConcessions(bool bValue) = 0;
	virtual bool IsHumanMakingDemand() const = 0;
	virtual void SetHumanMakingDemand(bool bValue) = 0;

	virtual int GetStartDiploRepeatCount() const = 0;
	virtual void SetStartDiploRepeatCount(int iValue) = 0;
	virtual void ChangeStartDiploRepeatCount(int iChange) = 0;
	virtual int GetOfferTradeRepeatCount() const = 0;
	virtual void SetOfferTradeRepeatCount(int iValue) = 0;
	virtual void ChangeOfferTradeRepeatCount(int iChange) = 0;

	virtual void OpenTechTree() = 0;
	virtual void OpenPlayerDealScreen( PlayerTypes ePlayer ) = 0;

	virtual bool IsPolicyNotificationSeen() const = 0;
	virtual void SetPolicyNotificationSeen(bool bValue) = 0;

	virtual bool IsForceDiscussionModeQuitOnBack() const = 0;
	virtual void SetForceDiscussionModeQuitOnBack(bool bValue) = 0;

	virtual void SetTempString(CvString strString) = 0;
	virtual void PublishEndTurnDirty() const = 0;

	virtual void AddDeferredWonderCommand(WonderCommandType eCommandType, ICvCity1* pThisCity, BuildingTypes eBuildingIndex, int iState) = 0;

	virtual bool IsLoadedGame() const = 0;

	virtual void ToggleStrategicView() = 0;
	virtual bool IsPopupQueueEmpty() = 0;
	virtual bool IsModalStackEmpty() = 0;

	virtual bool IsDontShowPopups() const = 0;
	virtual void SetDontShowPopups(bool bValue) = 0;

	virtual bool IsDealInTransit() = 0;
	virtual void SetDealInTransit(bool bValue) = 0;

	virtual bool IsSPAutoEndTurnEnabled() = 0;
	virtual void SetSPAutoEndTurnEnabled(bool bEnable) = 0;

	virtual bool IsMPAutoEndTurnEnabled() = 0;
	virtual void SetMPAutoEndTurnEnabled(bool bEnable) = 0;

	virtual bool IsSPQuickCombatEnabled() = 0;
	virtual void SetSPQuickCombatEnabled(bool bEnable) = 0;

	virtual bool IsMPQuickCombatEnabled() = 0;
	virtual void SetMPQuickCombatEnabled(bool bEnable) = 0;
};


//THIS INTERFACE IS LOCKED.  DO NOT MODIFY IN ANY WAY.
//This interface was used for base game/expansion 1 support.
class ICvUserInterface1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvUserInterface1; }

	virtual void lookAtSelectionPlot(bool bRelease = false) = 0;

	virtual bool CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisible = false) = 0;

	_Ret_maybenull_ virtual ICvPlot1* getSelectionPlot() = 0;
	virtual ICvUnit1* GetHeadSelectedUnit() = 0;
	virtual bool DoAutoUnitCycle() = 0;

	virtual ICvDeal1* GetScratchDeal() = 0;
	virtual void SetScratchDeal(ICvDeal1* pDeal) = 0;

	virtual bool isPopupUp() = 0;
	virtual bool isPopupQueued() = 0;
	virtual bool isDiploOrPopupWaiting() = 0;
	virtual bool isDiploActive() = 0;

	virtual bool IsUnitSelected(PlayerTypes playerID, int unitID) = 0;

	virtual void selectUnit(ICvUnit1* pUnit, bool bClear, bool bToggle = false, bool bSound = false) = 0;
	virtual void selectGroup(ICvUnit1* pUnit, bool bShift, bool bCtrl, bool bAlt) = 0;
	virtual void selectAll(ICvPlot1* pPlot) = 0;

	virtual bool RemoveFromSelectionList(ICvUnit1* pUnit) = 0;
	virtual void makeSelectionListDirty() = 0;
	virtual bool CanSelectionListFound() = 0;

	virtual ICvUnit1* getLastSelectedUnit() = 0;
	virtual void setLastSelectedUnit(ICvUnit1* pUnit) = 0;
	virtual void changePlotListColumn(int iChange) = 0;
	virtual int getOriginalPlotIndex() = 0;

	virtual void ClearSelectionList() = 0;
	virtual void InsertIntoSelectionList(ICvUnit1* pUnit, bool bClear, bool bToggle, bool bGroup = false, bool bSound = false, bool bMinimalChange = false) = 0;
	virtual int GetLengthSelectionList() = 0;
	virtual void VisuallyDeselectUnit(ICvUnit1* pUnit) = 0;
	virtual ICvEnumerator* GetSelectedUnits() = 0;

	virtual void selectCity(ICvCity1* pNewValue, bool bTestProduction = false) = 0;
	virtual void selectLookAtCity(bool bAdd = false) = 0;
	virtual void addSelectedCity(ICvCity1* pNewValue, bool bToggle = false) = 0;
	virtual void clearSelectedCities() = 0;
	virtual bool isCitySelected(ICvCity1 *pCity) = 0;
	virtual ICvCity1* getHeadSelectedCity() = 0;
	virtual bool isCitySelection() = 0;
	virtual IDInfo* nextSelectedCitiesNode(IDInfo* pNode) = 0;
	virtual const IDInfo* nextSelectedCitiesNode(const IDInfo* pNode) const = 0;
	virtual IDInfo* headSelectedCitiesNode() = 0;
	virtual void updateCityScreen(ICvCity1* pOldCity = NULL) = 0;

	virtual uint AddMessage(uint uiParentEvent, PlayerTypes ePlayer, bool bForce, int iLength, const char* strString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual uint AddUnitMessage(uint uiParentEvent, const IDInfo& unitID, PlayerTypes ePlayer, bool bForce, int iLength, const char* szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual uint AddPlotMessage(uint uiParentEvent, int iPlotID, PlayerTypes ePlayer, bool bForce, int iLength, const char* szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual uint AddCityMessage(uint uiParentEvent, const IDInfo& cityID, PlayerTypes ePlayer, bool bForce, int iLength, const char* szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	virtual void clearEventMessages() = 0;
	virtual void AddPopup(const CvPopupInfo& kPopup) = 0;
	virtual void AddPopupText( int x, int y, const char *szText, float fDelay = 0 ) = 0;
	virtual void PublishActivePlayerTurnStart() = 0;	// Only the 'active' (local human) player
	virtual void PublishActivePlayerTurnEnd() = 0;		// Only the 'active' (local human) player
	virtual void PublishRemotePlayerTurnEnd() = 0;
	virtual void PublishPlayerTurnStatus(CvDLLInterfaceIFaceBase::TURN_STATUS_TYPE eStatus, PlayerTypes ePlayer, const char* pszTag = NULL) = 0;
	virtual void AddNotification( int iID, NotificationTypes type, const char *cTooltip, const char* cSummary, int iGameData, int iExtraGameData, PlayerTypes ePlayer, int iX = -1, int iY = -1) = 0;
	virtual void RemoveNotification( int iID, PlayerTypes ePlayer ) = 0;
	virtual void ActivateNotification (int iID, NotificationTypes type, const char* cString, int iX, int iY, int iGameIndex, int iExtraGameDataValue, PlayerTypes ePlayer) = 0;

	virtual int getCycleSelectionCounter() = 0;
	virtual void setCycleSelectionCounter(int iNewValue) = 0;
	virtual void changeCycleSelectionCounter(int iChange) = 0;

	virtual bool IsSelectedUnitRevealingNewPlots() const = 0;
	virtual void SetSelectedUnitRevealingNewPlots(bool bValue) = 0;

	virtual int getEndTurnCounter() = 0;
	virtual void setEndTurnCounter(int iNewValue) = 0;
	virtual void changeEndTurnCounter(int iChange) = 0;

	virtual bool isCombatFocus() = 0;
	virtual void setCombatFocus(bool bNewValue) = 0;

	virtual bool isDirty(InterfaceDirtyBits eDirtyItem) = 0;
	virtual void setDirty(InterfaceDirtyBits eDirtyItem, bool bNewValue) = 0;
	virtual void makeInterfaceDirty() = 0;
	virtual bool updateCursorType() = 0;

	virtual void lookAt(const ICvPlot1* pPlot, CameraLookAtTypes type) = 0;
	virtual void centerCamera(ICvUnit1*) = 0;
	virtual void releaseLockedCamera() = 0;
	virtual bool isFocusedWidget() = 0;
	virtual bool isFocused() = 0;
	virtual void refreshYieldVisibleMode() = 0;
	virtual void toggleYieldVisibleMode() = 0;
	virtual void toggleResourceVisibleMode() = 0;
	virtual void toggleGridVisibleMode() = 0;

	virtual int getOriginalPlotCount() = 0;
	virtual bool isCityScreenUp() = 0;
	virtual void setCityScreenUp(bool bUp) = 0;

	virtual void setNoSelectionListCycle(bool bValue) = 0;

	virtual bool canEndTurn() const = 0;
	virtual void UpdateEndTurn() = 0;
	virtual void SetEndTurnBlockingChanged(EndTurnBlockingTypes ePrevBlockingType, EndTurnBlockingTypes eNewBlockingType) = 0;
	virtual void setInterfaceMode(InterfaceModeTypes eNewValue) = 0;
	virtual InterfaceModeTypes getInterfaceMode() = 0;
	virtual int GetInterfaceModeValue() = 0;
	virtual void setFlashing(PlayerTypes eWho, bool bFlashing = true) = 0;
	virtual bool isFlashing(PlayerTypes eWho) = 0;
	virtual void setDiplomacyLocked(bool bLocked) = 0;
	virtual bool isDiplomacyLocked() = 0;

	virtual void setCanEndTurn(bool bNewValue) = 0;
	virtual void updateEndTurnTimer(float percentComplete) const = 0;
	virtual bool waitingForRemotePlayers() const = 0;

	virtual bool isHasMovedUnit() = 0;
	virtual void setHasMovedUnit(bool bNewValue) = 0;

	virtual bool isForcePopup() = 0;
	virtual void setForcePopup(bool bNewValue) = 0;

	virtual void lookAtCityOffset(int iCity) = 0;

	virtual int getPlotListColumn() = 0;
	virtual void verifyPlotListColumn() = 0;
	virtual int getPlotListOffset() = 0;

	virtual void exitingToMainMenu(const char* szLoadFile=NULL) = 0;

	virtual bool isInAdvancedStart() const = 0;
	virtual void setInAdvancedStart(bool bAdvancedStart) = 0;

	virtual void setBusy(bool bBusy) = 0;

	virtual void UpdateCountryBorder(ICvPlot1* pThisPlot) = 0;
	virtual void UpdateAllCountryBorders() = 0;

	virtual void DebugCityRoutePlots (ICvPlot1* pPlot, bool bOn) = 0;

	virtual void DirectionAction(DirectionTypes eDirection) = 0;

	virtual void SetSpecificCityInfoDirty(ICvCity1* pCity, CityUpdateTypes eUpdateType) = 0;

	virtual void AddHexToUIRange(ICvPlot1* pThisPlot) = 0;

	virtual void OpenEndGameMenu() = 0;

	virtual void doTurn() = 0;

	virtual bool IsOptionNoRewardPopups() = 0;

	virtual bool IsAIRequestingConcessions() const = 0;
	virtual void SetAIRequestingConcessions(bool bValue) = 0;
	virtual bool IsHumanMakingDemand() const = 0;
	virtual void SetHumanMakingDemand(bool bValue) = 0;

	virtual int GetStartDiploRepeatCount() const = 0;
	virtual void SetStartDiploRepeatCount(int iValue) = 0;
	virtual void ChangeStartDiploRepeatCount(int iChange) = 0;
	virtual int GetOfferTradeRepeatCount() const = 0;
	virtual void SetOfferTradeRepeatCount(int iValue) = 0;
	virtual void ChangeOfferTradeRepeatCount(int iChange) = 0;

	virtual void OpenTechTree() = 0;
	virtual void OpenPlayerDealScreen( PlayerTypes ePlayer ) = 0;

	virtual bool IsPolicyNotificationSeen() const = 0;
	virtual void SetPolicyNotificationSeen(bool bValue) = 0;

	virtual bool IsForceDiscussionModeQuitOnBack() const = 0;
	virtual void SetForceDiscussionModeQuitOnBack(bool bValue) = 0;

	virtual void SetTempString(CvString strString) = 0;
	virtual void PublishEndTurnDirty() const = 0;

	virtual void AddDeferredWonderCommand(WonderCommandType eCommandType, ICvCity1* pThisCity, BuildingTypes eBuildingIndex, int iState) = 0;

	virtual bool IsLoadedGame() const = 0;

	virtual void ToggleStrategicView() = 0;
	virtual bool IsPopupQueueEmpty() = 0;
	virtual bool IsModalStackEmpty() = 0;

	virtual bool IsDontShowPopups() const = 0;
	virtual void SetDontShowPopups(bool bValue) = 0;

	virtual bool IsDealInTransit() = 0;
	virtual void SetDealInTransit(bool bValue) = 0;

	virtual bool IsSPAutoEndTurnEnabled() = 0;
	virtual void SetSPAutoEndTurnEnabled(bool bEnable) = 0;

	virtual bool IsMPAutoEndTurnEnabled() = 0;
	virtual void SetMPAutoEndTurnEnabled(bool bEnable) = 0;

	virtual bool IsSPQuickCombatEnabled() = 0;
	virtual void SetSPQuickCombatEnabled(bool bEnable) = 0;

	virtual bool IsMPQuickCombatEnabled() = 0;
	virtual void SetMPQuickCombatEnabled(bool bEnable) = 0;
};

//This interface contains methods necessary to support the second expansion and updates to the XP1 and base DLL
class ICvUserInterface2 : public ICvUserInterface1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvUserInterface2; }

	using ICvUserInterface1::AddPopup;
	virtual void PublishRemotePlayerTurnStart() = 0;	// Only remote human players
	virtual void AddPopup(int iPopupType, int iIntArraySize, const int *pIntArray, int iBoolArraySize, const bool *pBoolArray) = 0;
};
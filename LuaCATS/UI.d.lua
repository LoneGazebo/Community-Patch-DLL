--- @meta

--- @class UI
UI = {}

--- TODO docs
--- @param notificationIdx integer # TODO docs
function UI.ActivateNotification(notificationIdx) end

--- TODO docs
--- @param onSystemUpdateUIHandler fun(...) # TODO docs
function UI.Add(onSystemUpdateUIHandler) end

--- TODO docs
--- @class UI.PopupOptions
--- @field Type ButtonPopupType # TODO docs
--- @field Data1 unknown # TODO docs
--- @field Option1 boolean # TODO docs
--- @field Text? string

--- TODO docs
--- @param options UI.PopupOptions
function UI.AddPopup(options) end

--- Checks if `ALT` key is being pressed
--- @return boolean
function UI.AltKeyDown() end

--- TODO docs
--- @return boolean
function UI.AreMediumLeadersAllowed() end

--- TODO docs
--- @param systemUpdateUIType SystemUpdateUIType
function UI.CallImmediate(systemUpdateUIType) end

--- TODO docs
--- @param interfaceModeType InterfaceModeType
function UI.CanDoInterfaceMode(interfaceModeType) end

--- TODO docs
--- @return boolean
function UI.CanEndTurn() end

--- TODO docs. Seems to check if unit can be placed at plot
--- @param unit Unit
--- @param plot Plot
--- @return boolean
function UI.CanPlaceUnitAt(unit, plot) end

--- TODO docs
--- @return boolean
function UI.CanSelectionListFound() end

--- TODO docs
--- @return boolean
function UI.CanSelectionListWork() end

--- TODO docs
--- @param arg0 integer # TODO types
function UI.ChangeStartDiploRepeatCount(arg0) end

--- TODO docs
function UI.ClearPlaceUnit() end

--- TODO docs
function UI.ClearSelectedCities() end

--- TODO docs
--- @param ... unknown # TODO types
function UI.CompareFileTime(...) end

--- TODO docs
--- @param arg0 string
function UI.CopyLastAutoSave(arg0) end

--- Checks if `CTRL` key is being pressed.
--- @return boolean
function UI.CtrlKeyDown() end

--- TODO docs
--- @return boolean
function UI.DebugFlag() end

--- TODO docs
--- @param ... unknown
function UI.DebugKeyHandler(...) end

--- TODO docs
--- @param arg0 unknown
function UI.DeleteReplayFile(arg0) end

--- TODO docs
--- @param arg0 unknown
function UI.DeleteSavedGame(arg0) end

--- TODO docs
function UI.DoDemand() end

--- TODO docs
function UI.DoEqualizeDealWithHuman() end

--- TODO docs
--- @param initiatorPlayerId PlayerId
--- @param recipientPlayerId PlayerId
--- @param isAccepted boolean # TODO types
function UI.DoFinalizePlayerDeal(initiatorPlayerId, recipientPlayerId, isAccepted) end

--- TODO docs
function UI.DoProposeDeal() end

--- TODO docs
--- @param plot Plot
function UI.DoSelectCityAtPlot(plot) end

--- TODO docs
function UI.DoWhatDoesAIWant() end

--- TODO docs
function UI.DoWhatWillAIGive() end

--- TODO docs
function UI.ExitGame() end

--- TODO docs
--- @return unknown[]
function UI.GetAvailableLeaderboards() end

--- TODO docs
--- @return unknown
function UI.GetCredits() end

--- TODO docs
--- @return GameStateType
function UI.GetCurrentGameState() end

--- TODO docs
--- @return string
function UI.GetDllGUID() end

--- TODO docs
--- @deprected # It's commented out everywhere
--- @return unknown
function UI.GetGotoPlot() end

--- TODO docs
--- @return unknown[]
function UI.GetHallofFameData() end

--- TODO docs
--- @return City | nil
function UI.GetHeadSelectedCity() end

--- TODO docs
--- @return Unit | nil
function UI.GetHeadSelectedUnit() end

--- TODO docs
--- @return InterfaceModeType
function UI.GetInterfaceMode() end

--- TODO docs
--- @return integer
function UI.GetInterfaceModeDebugItemID1() end

--- TODO docs
--- @return integer
function UI.GetInterfaceModeDebugItemID2() end

--- TODO docs
--- @return PlayerId # Is it though?
function UI.GetInterfaceModeValue() end

--- TODO docs
--- @return Unit | nil
function UI.GetLastSelectedUnit() end

--- TODO docs
--- @return unknown # TODO types
function UI.GetLeaderHeadRootUp() end

--- TODO docs
--- @return unknown[]
function UI.GetLeaderboardScores() end

--- TODO docs
--- @return string
--- @deprected # No usage in code found?
function UI.GetLeaderboardTitle() end

--- TODO docs
--- @param mapFileName unknown # TODO types
--- @return unknown[] # Some table of players?
function UI.GetMapPlayers(mapFileName) end

--- TODO docs
--- @param mapFileName unknown # TODO types
--- @return unknown
function UI.GetMapPreview(mapFileName) end

--- TODO docs
--- @return integer, integer # Hex X, Hex Y
function UI.GetMouseOverHex() end

--- TODO docs
--- @return MultiplayerLobbyMode
function UI.GetMultiplayerLobbyMode() end

--- Gets the amount of current deals for specified player by player id.
--- @param playerId PlayerId
--- @return integer
function UI.GetNumCurrentDeals(playerId) end

--- Gets the amount of historic deals for specified player by player id.
--- @param playerId PlayerId
--- @return integer
function UI.GetNumHistoricDeals(playerId) end

--- TODO docs
--- @return Unit | nil
function UI.GetPlaceUnit() end

--- TODO docs
--- @param file unknown
--- @return unknown | nil
function UI.GetReplayFileHeader(file) end

--- TODO docs
--- @return unknown[]
function UI.GetReplayFiles() end

--- TODO docs
--- @param file unknown
function UI.GetReplayInfo(file) end

--- TODO docs
--- @param file unknown
--- @return string | nil # Date
function UI.GetReplayModificationTime(file) end

--- TODO docs
--- @param file unknown
--- @return unknown, unknown # Some high and low?
function UI.GetReplayModificationTimeRaw(file) end

--- TODO docs
--- @param file unknown
--- @return string | nil # Date
function UI.GetSavedGameModificationTime(file) end

--- TODO docs
--- @param file unknown
--- @return unknown, unknown # Some high and low?
function UI.GetSavedGameModificationTime(file) end

--- TODO docs
--- @return Deal
function UI.GetScratchDeal() end

--- If some unit is selected gets it's id, otherwise -1
--- @return (-1 | UnitId)
function UI.GetSelectedUnitID() end

--- TODO docs
--- @return string
function UI.GetTempString() end

--- TODO docs
--- @param unit Unit
--- @return unknown, unknown # Offset, Atlas
function UI.GetUnitFlagIcon(unit) end

--- TODO docs
--- @return unknown, unknown
function UI.GetUnitPortraitIcon(...) end

--- Gets game version information
--- ```lua
--- -- Example
--- print(UI.GetVersionInfo()) -- 1.0.3.279 (403694)
--- ```
--- @return string
function UI.GetVersionInfo() end

--- Gets the ID of the player who received the proposal from specified player.
--- @param initiatorPlayerId PlayerId
--- @return -1 | PlayerId # Proposal recipient player id
function UI.HasMadeProposal(initiatorPlayerId) end

--- TODO docs
--- @param unit Unit
--- @param plot Plot # City plot
function UI.HighlightCanPlacePlots(unit, plot) end

--- TODO docs
--- @return boolean
function UI.IsAIRequestingConcessions() end

--- TODO docs
--- @return boolean
function UI.IsCameraMoving() end

--- Checks if City Screen is currently up
--- @return boolean
function UI.IsCityScreenUp() end

--- TODO docs
--- @return boolean
function UI.IsCityScreenViewingMode() end

--- Checks if game is running using DX9 graphics API.
--- If graphics api is DX9 returns `true`, otherwise `nil`
--- ```lua
--- -- Example
--- function IsDX11()
--- 	return not UI.IsDX9()
--- end
--- ```
--- @return true | nil
function UI.IsDX9() end

--- TODO docs
--- @param mapScriptFileName unknown # TODO types
--- @return boolean
function UI.IsMapScenario(mapScriptFileName) end

--- Checks if popup of specified type is open.
--- @param buttonPopupType ButtonPopupType
--- @return boolean
function UI.IsPopupTypeOpen(buttonPopupType) end

--- Checks if any popup is open.
--- @return boolean
function UI.IsPopupUp() end

--- Checks if touchscreen is enabled.
--- @return boolean
function UI.IsTouchScreenEnabled() end

--- comment
--- @param playerId PlayerId
--- @param dealIdx integer
function UI.LoadCurrentDeal(playerId, dealIdx) end

--- TODO docs
--- @param inititorPlayerId PlayerId
--- @param recipientPlayerId PlayerId
function UI.LoadHistoricDeal(inititorPlayerId, recipientPlayerId) end

--- TODO docs
--- @param initiatorPlayerId PlayerId
--- @param recipientPlayerId PlayerId
function UI.LoadProposedDeal(initiatorPlayerId, recipientPlayerId) end

--- TODO docs
--- @param plot Plot
--- @param isCtrlPressed boolean
--- @param isAltPressed boolean
--- @param isShiftPressed boolean
function UI.LocationSelect(plot, isCtrlPressed, isAltPressed, isShiftPressed) end

--- Focuses camera on specified plot
--- @param plot Plot
--- @param zoom? 0 | 1 | 2 # If 1 zooms out, other values doesn't seem to change anything.
function UI.LookAt(plot, zoom) end

--- Focuses camera on currently selected plot.
--- For example focuses plot with unit that is currently selected.
--- @param arg0 integer # TODO doesn't seem to do anything
function UI.LookAtSelectionPlot(arg0) end

--- TODO docs
--- @param arg0 unknown
--- @param arg1 unknown
function UI.MoveScenarioPlayerToSlot(arg0, arg1) end

--- TODO docs
--- @param playerId PlayerId
function UI.OnHumanDemand(playerId) end

--- TODO docs
--- @param playerId PlayerId
function UI.OnHumanOpenedTradeScreen(playerId) end

--- Simulates keyboard key presses using ASCII codes.
--- Can be used in touchscreen environment, or to programatically press keyboard keys.
--- @param asciiCharIdx integer # `8` is `BACKSPACE`, `13` is `ENTER`, `49` is `1` and etc.
--- @see https://www.ascii-code.com/
function UI.PostKeyMessage(asciiCharIdx) end

--- Checks if proposal exists between specified players by player ids.
--- @param initiatorPlayerId PlayerId
--- @param recipientPlayerId PlayerId
--- @return boolean
function UI.ProposedDealExists(initiatorPlayerId, recipientPlayerId) end

--- TODO docs
function UI.PushScratchDeal() end

--- Quicksaves the current game.
function UI.QuickSave() end

--- TODO docs
function UI.RebroadcastNotifications() end

--- TODO docs
function UI.RefreshYieldVisibleMode() end

--- TODO docs
--- @param notificationIdx integer
function UI.RemoveNotification(notificationIdx) end

--- TODO docs
function UI.RequestLeaderboardScores() end

--- TODO docs
function UI.RequestLeaveLeader() end

--- TODO docs
--- @param arg0? boolean
function UI.ResetScenarioPlayerSlots(arg0) end

--- TODO docs
--- @param fileList unknown
--- @param gameType unknown
--- @param showAutoSaves boolean
--- @param arg4 boolean
function UI.SaveFileList(fileList, gameType, showAutoSaves, arg4) end

--- Saves current game under specified name.
--- @param gameName string
function UI.SaveGame(gameName) end

--- Saves current map under specified name.
--- @param mapName string
function UI.SaveMap(mapName) end

--- TODO docs
function UI.ScrollLeaderboardDown() end

--- TODO docs
function UI.ScrollLeaderboardUp() end

--- Selects and focuses specified city.
--- @param city City
function UI.SelectCity(city) end

--- Selects and focuses specified unit.
--- @param unit Unit
function UI.SelectUnit(unit) end

--- TODO docs
function UI.SendPathfinderUpdate() end

--- TODO docs
--- @param arg0 unknown
--- @param arg1 boolean
function UI.SetAdvisorMessageHasBeenSeen(arg0, arg1) end

--- TODO docs
--- @param state boolean
function UI.SetCityScreenUp(state) end

--- TODO docs
--- @param state boolean
function UI.SetCityScreenViewingMode(state) end

--- TODO docs
--- @param interfaceDirtyBit InterfaceDirtyBit
--- @param state boolean
function UI.SetDirty(interfaceDirtyBit, state) end

--- TODO docs
--- @param state boolean
function UI.SetDontShowPopups(state) end

--- Changes the state of tile's grid visibility.
--- @param isTileGridVisible boolean # If true shows tile's grid, otherwise hides tile's grid.
function UI.SetGridVisibleMode(isTileGridVisible) end

--- TODO docs
--- @param interfaceModeType InterfaceModeType
function UI.SetInterfaceMode(interfaceModeType) end

--- TODO docs
--- @param minorCivPlayerId PlayerId
function UI.SetInterfaceModeValue(minorCivPlayerId) end

--- TODO docs
--- @param state boolean
function UI.SetLeaderHeadRootUp(state) end

--- TODO docs
--- @param arg0 unknown
--- @param arg1 string
function UI.SetLeaderboard(arg0, arg1) end

--- TODO docs
--- @param arg0 unknown
function UI.SetLeaderboardCategory(arg0) end

--- TODO docs
--- @param multiplayerLobbyMode MultiplayerLobbyMode
function UI.SetMultiplayerLobbyMode(multiplayerLobbyMode) end

--- TODO docs
--- @param nextGameState unknown
--- @param aiPlayerId PlayerId
--- @deprecated # Commented out everywhere
function UI.SetNextGameState(nextGameState, aiPlayerId) end

--- TODO docs
--- @param num integer
function UI.SetOfferTradeRepeatCount(num) end

--- TODO docs
--- @param unit Unit
function UI.SetPlaceUnit(unit) end

--- TODO docs
--- @param playerId PlayerId
function UI.SetRepeatActionPlayer(playerId) end

--- Changes the state of tile resource icons visibility.
--- @param isTileResourceVisible boolean # If true shows tile resource icons, otherwise hides them.
function UI.SetResourceVisibleMode(isTileResourceVisible) end

--- Changes the state of tile yields visibility.
--- @param isTileYieldVisible boolean # If true shows tile yields, otherwise hides them.
function UI.SetYieldVisibleMode(isTileYieldVisible) end

--- Checks if `SHIFT` key is being pressed.
--- @return boolean
function UI.ShiftKeyDown() end

--- Toggles tile hex grid visibility.
--- @see UI.SetGridVisibleMode
function UI.ToggleGridVisibleMode() end

--- Toggle tile resources visiblity.
--- @see UI.SetResourceVisibleMode
function UI.ToggleResourceVisibleMode() end

--- Toggles tile yields visiblity.
--- @see UI.SetYieldVisibleMode
function UI.ToggleYieldVisibleMode() end

--- TODO docs
--- @param achivement string
function UI.UnlockAchievement(achivement) end

--- TODO docs
function UI.UpdateCityScreen() end

--- TODO docs, Checks if is waiting for remote players
--- @return boolean
function UI.WaitingForRemotePlayers() end

--- TODO docs
function UI.decTurnTimerSemaphore() end

--- TODO docs
function UI.incTurnTimerSemaphore() end

--- TODO docs
function UI.interruptTurnTimer() end

--- Checks if current game is loaded from save file.
--- @return boolean
function UI.IsLoadedGame() end

--- TODO docs
function UI.RequestMinimapBroadcast() end

--- TODO docs
--- @return boolean
function UI.CheckForCommandLineInvitation() end

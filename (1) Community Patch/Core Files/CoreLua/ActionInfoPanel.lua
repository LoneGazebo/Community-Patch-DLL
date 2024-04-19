include("FLuaVector")
include("IconSupport")

local MOD_BALANCE_VP = GameInfo.CustomModOptions("Name = 'BALANCE_VP'")().Value == 1

--- @type fun(txtKey: string, ...: string): string
local L = Locale.ConvertTextKey

local FlashingStates = {
	NO_FLASH   = 0,
	END_TURN   = 1,
	SCIENCE    = 2,
	PRODUCTION = 3,
	FREE_TECH  = 4,
}

--------------------
--- Pre Start
local Map = { __index = {} }
local Utils = {}
local Actions = {}
local Listeners = {}
local EndTurnBlockingTypeEffectMap = nil
local EndTurnBlockingTypeToActivateNotificationMap = nil
--- Pre End
--------------------

--------------------
do -- Map
	function Map.new()
		local instance = {
			_type = Map,
			_table = {},
		}

		return setmetatable(instance, { __index = Map.__index })
	end

	function Map.__index.set(self, key, val)
		if key ~= nil and val ~= nil then
			self._table[key] = val
		end

		return self
	end

	function Map.__index.get(self, key)
		return self._table[key]
	end

	function Map.__index.map(self, mapper)
		local map = Map.new()

		for key, val in pairs(self._table) do
			local newKey, newVal = mapper(key, val)

			map:set(newKey, newVal)
		end

		return map
	end
end -- Map
--------------------

--------------------
do -- Utils
	function Utils.IsFunction(val)
		return type(val) == "function"
	end

	function Utils.IsUnitStandby(unit)
		if unit == nil then
			return false
		end

		return unit:IsReadyToMove()
			and not unit:IsAutomated()
			and not unit:IsDelayedDeath()
	end

	function Utils.GetActiveTurnPlayer()
		local player = Players[Game.GetActivePlayer()]

		if player == nil or not player:IsTurnActive() then
			return nil
		end

		return player
	end

	function Utils.GetActiveLeagueName()
		if Game.GetNumActiveLeagues() > 0 then
			local league = Game.GetActiveLeague()

			if league ~= nil then
				return league:GetName()
			end
		end

		return nil
	end
end -- Utils
--------------------

--------------------
do -- EndTurnBlockingTypeEffectMap
	local t = EndTurnBlockingTypes

	EndTurnBlockingTypeEffectMap = Map.new()
		:set(t.ENDTURN_BLOCKING_POLICY, {
			message = "TXT_KEY_CHOOSE_POLICY",
			tooltip = function()
				return L(
					Game.IsOption(GameOptionTypes.GAMEOPTION_POLICY_SAVING)
					and "TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY_DISMISS"
					or "TXT_KEY_NOTIFICATION_ENOUGH_CULTURE_FOR_POLICY"
				)
			end,
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_RESEARCH, {
			message = "TXT_KEY_CHOOSE_RESEARCH",
			tooltip = "TXT_KEY_CHOOSE_RESEARCH_TT",
			flashing = FlashingStates.SCIENCE,
		})
		:set(t.ENDTURN_BLOCKING_FREE_TECH, {
			message = "TXT_KEY_CHOOSE_FREE_TECH",
			tooltip = "TXT_KEY_CHOOSE_FREE_TECH_TT",
			flashing = FlashingStates.FREE_TECH,
		})
		:set(t.ENDTURN_BLOCKING_PRODUCTION, {
			message = "TXT_KEY_CHOOSE_PRODUCTION",
			tooltip = "TXT_KEY_CHOOSE_PRODUCTION_TT",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_UNITS, {
			message = "TXT_KEY_UNIT_NEEDS_ORDERS",
			tooltip = "TXT_KEY_UNIT_NEEDS_ORDERS_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_STACKED_UNITS, {
			message = "TXT_KEY_MOVE_STACKED_UNIT",
			tooltip = "TXT_KEY_MOVE_STACKED_UNIT_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_UNIT_NEEDS_ORDERS, {
			message = "TXT_KEY_UNIT_NEEDS_ORDERS",
			tooltip = "TXT_KEY_UNIT_NEEDS_ORDERS_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_UNIT_PROMOTION, {
			message = "TXT_KEY_UNIT_PROMOTION",
			tooltip = "TXT_KEY_UNIT_PROMOTION_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_CITY_RANGE_ATTACK, {
			message = "TXT_KEY_CITY_RANGE_ATTACK",
			tooltip = "TXT_KEY_CITY_RANGE_ATTACK_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_DIPLO_VOTE, {
			message = "TXT_KEY_DIPLO_VOTE",
			tooltip = "TXT_KEY_DIPLO_VOTE_TT",
			flashing = FlashingStates.END_TURN
		})
		:set(t.ENDTURN_BLOCKING_FREE_ITEMS, function(player)
			local effect = {
				message = "",
				tooltip = "",
				flashing = FlashingStates.PRODUCTION,
			}

			local notificationsNumber = player:GetNumNotifications()
			local blockingNotificationIndex = player:GetEndTurnBlockingNotificationIndex();

			for i = 0, notificationsNumber - 1 do
				if (blockingNotificationIndex == player:GetNotificationIndex(i)) then
					effect.message = player:GetNotificationSummaryStr(i)
					effect.tooltip = player:GetNotificationStr(i)
					break
				end
			end

			return effect
		end)
		:set(t.ENDTURN_BLOCKING_FREE_POLICY, {
			message = "TXT_KEY_CHOOSE_POLICY",
			tooltip = "TXT_KEY_NOTIFICATION_FREE_POLICY",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_FOUND_PANTHEON, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_FAITH_FOR_PANTHEON",
			tooltip = "TXT_KEY_NOTIFICATION_ENOUGH_FAITH_FOR_PANTHEON",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_FOUND_RELIGION, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RELIGION",
			tooltip = "TXT_KEY_NOTIFICATION_FOUND_RELIGION",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_ENHANCE_RELIGION, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_ENHANCE_RELIGION",
			tooltip = "TXT_KEY_NOTIFICATION_ENHANCE_RELIGION",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_ADD_REFORMATION_BELIEF, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_ADD_REFORMATION_BELIEF",
			tooltip = "TXT_KEY_NOTIFICATION_ADD_REFORMATION_BELIEF",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_CHOOSE_ARCHAEOLOGY, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_ARCHAEOLOGY",
			tooltip = "TXT_KEY_NOTIFICATION_CHOOSE_ARCHAEOLOGY",
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_STEAL_TECH, {
			message = "TXT_KEY_NOTIFICATION_SPY_STEAL_BLOCKING",
			tooltip = "TXT_KEY_NOTIFICATION_SPY_STEAL_BLOCKING_TT",
			flashing = FlashingStates.FREE_TECH,
		})
		:set(t.ENDTURN_BLOCKING_MAYA_LONG_COUNT, {
			message = "TXT_KEY_NOTIFICATION_MAYA_LONG_COUNT",
			tooltip = "TXT_KEY_NOTIFICATION_MAYA_LONG_COUNT_TT",
			flashing = FlashingStates.FREE_TECH,
		})
		:set(t.ENDTURN_BLOCKING_FAITH_GREAT_PERSON, {
			message = "TXT_KEY_NOTIFICATION_FAITH_GREAT_PERSON",
			tooltip = "TXT_KEY_NOTIFICATION_FAITH_GREAT_PERSON_TT",
			flashing = FlashingStates.FREE_TECH,
		})
		:set(t.ENDTURN_BLOCKING_LEAGUE_CALL_FOR_PROPOSALS, {
			message = "TXT_KEY_NOTIFICATION_LEAGUE_PROPOSALS_NEEDED",
			tooltip = function()
				local leagueName = Utils.GetActiveLeagueName()

				return leagueName
					and L("TXT_KEY_NOTIFICATION_LEAGUE_PROPOSALS_NEEDED_TT", leagueName)
					or ""
			end,
			flashing = FlashingStates.PRODUCTION,
		})
		:set(t.ENDTURN_BLOCKING_LEAGUE_CALL_FOR_VOTES, {
			message = "TXT_KEY_NOTIFICATION_LEAGUE_VOTES_NEEDED",
			tooltip = function()
				local leagueName = Utils.GetActiveLeagueName()

				return leagueName
					and L("TXT_KEY_NOTIFICATION_LEAGUE_VOTES_NEEDED_TT", leagueName)
					or ""
			end,
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_CHOOSE_IDEOLOGY, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_IDEOLOGY",
			tooltip = function(player)
				return L(
					player:GetCurrentEra() > GameInfo.Eras["ERA_INDUSTRIAL"].ID
					and "TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_ERA"
					or "TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_FACTORIES"
				)
			end,
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_PENDING_DEAL, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_DIPLOMATIC_REQUEST",
			tooltip = "TXT_KEY_NOTIFICATION_SUMMARY_DIPLOMATIC_REQUEST_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_EVENT_CHOICE, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_EVENT",
			tooltip = "TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_EVENT_TT",
			flashing = FlashingStates.END_TURN,
		})
		:set(t.ENDTURN_BLOCKING_CHOOSE_CITY_FATE, {
			message = "TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_CITY_FATE",
			tooltip = "TXT_KEY_NOTIFICATION_SUMMARY_CHOOSE_CITY_FATE_TT",
			flashing = FlashingStates.END_TURN,
		})
		:map(function(key, val)
			if Utils.IsFunction(val) then
				return key, val
			end

			return key, {
				message = Utils.IsFunction(val.message) and val.message or L(val.message),
				tooltip = Utils.IsFunction(val.tooltip) and val.tooltip or L(val.tooltip),
				flashing = val.flashing,
				disabled = val.disabled
			}
		end)
end -- EndTurnBlockingTypeEffectMap
--------------------

--------------------
do -- EndTurnBlockingTypeToActivateNotificationMap
	local t = EndTurnBlockingTypes

	local blockingTypes = {
		t.ENDTURN_BLOCKING_RESEARCH,
		t.ENDTURN_BLOCKING_FREE_TECH,
		t.ENDTURN_BLOCKING_PRODUCTION,
		t.ENDTURN_BLOCKING_DIPLO_VOTE,
		t.ENDTURN_BLOCKING_FREE_ITEMS,
		t.ENDTURN_BLOCKING_FREE_POLICY,
		t.ENDTURN_BLOCKING_FOUND_PANTHEON,
		t.ENDTURN_BLOCKING_FOUND_RELIGION,
		t.ENDTURN_BLOCKING_ENHANCE_RELIGION,
		t.ENDTURN_BLOCKING_ADD_REFORMATION_BELIEF,
		t.ENDTURN_BLOCKING_STEAL_TECH,
		t.ENDTURN_BLOCKING_MAYA_LONG_COUNT,
		t.ENDTURN_BLOCKING_FAITH_GREAT_PERSON,
		t.ENDTURN_BLOCKING_CHOOSE_ARCHAEOLOGY,
		t.ENDTURN_BLOCKING_LEAGUE_CALL_FOR_PROPOSALS,
		t.ENDTURN_BLOCKING_LEAGUE_CALL_FOR_VOTES,
		t.ENDTURN_BLOCKING_PENDING_DEAL,
		t.ENDTURN_BLOCKING_EVENT_CHOICE,
		t.ENDTURN_BLOCKING_CHOOSE_CITY_FATE,
		t.ENDTURN_BLOCKING_CHOOSE_IDEOLOGY,
	}

	-- Using Map allows to omit nil keys
	EndTurnBlockingTypeToActivateNotificationMap = Map.new()

	for index, blockingType in ipairs(blockingTypes) do
		EndTurnBlockingTypeToActivateNotificationMap:set(blockingType, true)
	end
end -- EndTurnBlockingTypeToActivateNotificationMap
--------------------

--------------------
do -- Actions
	local t = EndTurnBlockingTypes

	local txtNewline = "[NEWLINE]"
	local txtUnmetPlayer = L("TXT_KEY_POP_VOTE_RESULTS_UNMET_PLAYER")

	local txtNextTurn = L("TXT_KEY_NEXT_TURN")
	local txtNextTurnTt = L("TXT_KEY_NEXT_TURN_TT")

	local txtPleaseWait = L("TXT_KEY_PLEASE_WAIT")
	local txtPleaseWaitTt = L("TXT_KEY_PLEASE_WAIT_TT")

	local txtWaitingForPlayers = L("TXT_KEY_WAITING_FOR_PLAYERS")
	local txtWaitingForPlayersTt = L("TXT_KEY_WAITING_FOR_PLAYERS_TT")

	local effectForNoEndTurnBlockingType = {
		message = txtNextTurn,
		tooltip = function()
			return not OptionsManager.IsNoBasicHelp()
				and txtNextTurnTt
				or ""
		end,
		flashing = FlashingStates.END_TURN,
	}

	local effectForWaitingRemotePlayers = {
		message = txtWaitingForPlayers,
		tooltip = txtWaitingForPlayersTt,
		flashing = FlashingStates.NO_FLASH,
		disabled = true
	}

	function Actions.SelectUnit(unit, withFx)
		if unit == nil then
			return false
		end

		local plot = unit:GetPlot()

		if plot == nil then
			return false
		end

		UI.LookAt(plot, 0)
		UI.SelectUnit(unit)

		if withFx then
			local hex = ToHexFromGrid(Vector2(plot:GetX(), plot:GetY()))
			Events.GameplayFX(hex.x, hex.y, -1)
		end

		return true
	end

	function Actions.SelectPlayerUnitWithPromotionReady(player, withFx)
		if player == nil then
			return false
		end

		for unit in player:Units() do
			if unit ~= nil and unit:IsPromotionReady() then
				return Actions.SelectUnit(unit, withFx)
			end
		end

		return false
	end

	function Actions.ApplyEndTurnFlashingState(flashingState)
		local shouldHideEndTurnFlash = true
		local shouldHideScienceFlash = true
		local shouldHideFreeTechFlash = true
		local shouldHideProductionFlash = true
		local shouldHideNormalMouseover = false

		if (flashingState == FlashingStates.END_TURN) then
			shouldHideEndTurnFlash = false
			shouldHideNormalMouseover = true
		elseif (flashingState == FlashingStates.SCIENCE) then
			shouldHideScienceFlash = false
		elseif (flashingState == FlashingStates.PRODUCTION) then
			shouldHideProductionFlash = false
		elseif (flashingState == FlashingStates.FREE_TECH) then
			shouldHideFreeTechFlash = false
		end

		Controls.EndTurnButtonEndTurnAlpha:SetHide(shouldHideEndTurnFlash)
		Controls.EndTurnButtonScienceAlpha:SetHide(shouldHideScienceFlash)
		Controls.EndTurnButtonFreeTechAlpha:SetHide(shouldHideFreeTechFlash)
		Controls.EndTurnButtonProductionAlpha:SetHide(shouldHideProductionFlash)
		Controls.EndTurnButtonMouseOverAlpha:SetHide(shouldHideNormalMouseover)
	end

	function Actions.ApplyNoEndTurnBlockingTypeEffect(player)
		local effect = effectForNoEndTurnBlockingType

		if UI.WaitingForRemotePlayers() then
			effect = effectForWaitingRemotePlayers
		end

		Actions.ApplyEndTurnBlockingTypeEffect(effect, player, t.NO_ENDTURN_BLOCKING_TYPE)
	end

	function Actions.ApplyEndTurnBlockingTypeEffect(effect, player, blockingType)
		if Utils.IsFunction(effect) then
			effect = effect(player, blockingType)
		end

		local message = effect.message
		local tooltip = effect.tooltip

		if Utils.IsFunction(message) then
			message = message(player, blockingType)
		end

		if Utils.IsFunction(tooltip) then
			tooltip = tooltip(player, blockingType)
		end

		Controls.EndTurnText:SetText(message)
		Controls.EndTurnButton:SetToolTipString(tooltip)
		Controls.EndTurnButton:SetDisabled(effect.disabled or false) -- coerce to false if nil

		Actions.ApplyEndTurnFlashingState(effect.flashing or FlashingStates.NO_FLASH)
	end

	function Actions.SetEndTurnWaiting()
		local isMultiplayer = PreGame.IsMultiplayerGame()

		if isMultiplayer and Network.HasSentNetTurnAllComplete() then
			Controls.EndTurnButton:SetDisabled(true)
		end

		local activePlayerId = Game.GetActivePlayer()
		local activePlayer = Players[activePlayerId]

		local activePlayerTeamId = activePlayer:GetTeam()
		local activePlayerTeam = Teams[activePlayerTeamId]

		local waiting = 0
		local tooltip = txtWaitingForPlayersTt

		if isMultiplayer and Network.HasSentNetTurnComplete() then
			waiting = 1
		end

		for playerId = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
			if playerId ~= activePlayerId then
				local player = Players[playerId]

				if player ~= nil and player:IsHuman() and not player:HasReceivedNetTurnComplete() then
					local playerCivKey = txtUnmetPlayer

					if activePlayerTeam:IsHasMet(player:GetTeam()) then
						playerCivKey = player:GetCivilizationShortDescriptionKey()
					end

					waiting = waiting + 1
					tooltip = tooltip .. txtNewline .. player:GetName() .. "(" .. L(playerCivKey) .. ")"
				end
			end
		end

		if waiting == 0 then
			Controls.EndTurnText:SetText(txtPleaseWait)
			Controls.EndTurnButton:SetToolTipString(txtPleaseWaitTt)
		else
			Controls.EndTurnText:SetText(txtWaitingForPlayers)
			Controls.EndTurnButton:SetToolTipString(tooltip);
		end

		Actions.ApplyEndTurnFlashingState(FlashingStates.NO_FLASH)
	end

	function Actions.SetEndTurnDirty()
		local player = Utils.GetActiveTurnPlayer()

		if player == nil or (PreGame.IsMultiplayerGame() and Network.HasSentNetTurnComplete()) then
			Actions.SetEndTurnWaiting()
			return
		end

		local blockingType = player:GetEndTurnBlockingType()

		if blockingType == t.NO_ENDTURN_BLOCKING_TYPE or blockingType == nil then
			Actions.ApplyNoEndTurnBlockingTypeEffect(player)
			return
		end

		local effect = EndTurnBlockingTypeEffectMap:get(blockingType)

		if effect ~= nil then
			Actions.ApplyEndTurnBlockingTypeEffect(effect, player, blockingType)
			return
		end

		Actions.ApplyNoEndTurnBlockingTypeEffect(player)
	end
end -- Actions
--------------------

--------------------
do -- Listeners
	local t = EndTurnBlockingTypes

	function Listeners.OnEndTurnLeftClicked()
		local player = Utils.GetActiveTurnPlayer()

		if player == nil or Game.IsProcessingMessages() then
			return
		end

		if PreGame.IsMultiplayerGame() and Network.HasSentNetTurnComplete() then
			if Network.SendTurnUnready() then
				Actions.SetEndTurnDirty()
			end

			return
		end

		if MOD_BALANCE_VP then
			player:EndTurnsForReadyUnits(true)

			if UIManager:GetShift() then
				player:EndTurnsForReadyUnits(false)
			end
		end

		local blockingType = player:GetEndTurnBlockingType()

		if blockingType == t.ENDTURN_BLOCKING_UNITS
			or blockingType == t.ENDTURN_BLOCKING_STACKED_UNITS
			or blockingType == t.ENDTURN_BLOCKING_UNIT_NEEDS_ORDERS
		then
			Actions.SelectUnit(player:GetFirstReadyUnit(), true)
		elseif blockingType == t.ENDTURN_BLOCKING_POLICY
			or blockingType == t.ENDTURN_BLOCKING_CITY_RANGE_ATTACK
			or EndTurnBlockingTypeToActivateNotificationMap:get(blockingType)
		then
			UI.ActivateNotification(player:GetEndTurnBlockingNotificationIndex())
		elseif blockingType == t.ENDTURN_BLOCKING_UNIT_PROMOTION then
			Actions.SelectPlayerUnitWithPromotionReady(player, true)
		else
			Game.DoControl(GameInfoTypes.CONTROL_ENDTURN)
		end
	end

	function Listeners.OnEndTurnRightClicked()
		local player = Utils.GetActiveTurnPlayer()

		if player == nil or (PreGame.IsMultiplayerGame() and Network.HasSentNetTurnComplete()) then
			return
		end

		local blockingType = player:GetEndTurnBlockingType()

		if blockingType == t.ENDTURN_BLOCKING_UNITS
			or blockingType == t.ENDTURN_BLOCKING_STACKED_UNITS
			or blockingType == t.ENDTURN_BLOCKING_UNIT_NEEDS_ORDERS
		then
			Actions.SelectUnit(player:GetFirstReadyUnit(), true)
		elseif blockingType == t.ENDTURN_BLOCKING_POLICY then
			UI.RemoveNotification(player:GetEndTurnBlockingNotificationIndex())
		elseif EndTurnBlockingTypeToActivateNotificationMap:get(blockingType) then
			UI.ActivateNotification(player:GetEndTurnBlockingNotificationIndex())
		elseif blockingType == t.ENDTURN_BLOCKING_UNIT_PROMOTION then
			Actions.SelectPlayerUnitWithPromotionReady(player, true)
		elseif blockingType == t.ENDTURN_BLOCKING_CITY_RANGE_ATTACK then
			local city = UI.GetHeadSelectedCity()

			if city ~= nil then
				UI.LookAt(city:Plot(), 0)
			end
		end
	end

	function Listeners.OnEndTurnBlockingChanged(prevBlockingType, currBlockingType)
		-- Why arguments are ignored?
		-- Maybe nevertheless there is some blocking type ui should select unit that requires some action...

		local player = Utils.GetActiveTurnPlayer()

		-- Shall we check if player is human, do bots also use this listener
		if player == nil or not OptionsManager.GetAutoUnitCycle() then
			return
		end

		local selectedUnit = UI.GetHeadSelectedUnit()

		if Utils.IsUnitStandby(selectedUnit) then
			-- As I understand,
			-- if selected unit is head,
			-- it means that it is already selected,
			-- so there is no need to select any units
			return
		end

		for unit in player:Units() do
			if Utils.IsUnitStandby(unit) then
				Actions.SelectUnit(unit)

				return
			end
		end
	end

	function Listeners.OnEndTurnDirty()
		Actions.SetEndTurnDirty()
	end

	function Listeners.OnActivePlayerTurnEnd()
		Actions.SetEndTurnWaiting()
	end

	function Listeners.OnRemotePlayerTurnEnd()
		Actions.SetEndTurnDirty()
	end
end -- Listeners
--------------------

--------------------
do -- Set Listeners
	Controls.EndTurnButton:RegisterCallback(Mouse.eLClick, Listeners.OnEndTurnLeftClicked)
	Controls.EndTurnButton:RegisterCallback(Mouse.eRClick, Listeners.OnEndTurnRightClicked)

	Events.SerialEventEndTurnDirty.Add(Listeners.OnEndTurnDirty)

	Events.RemotePlayerTurnEnd.Add(Listeners.OnRemotePlayerTurnEnd)
	Events.ActivePlayerTurnEnd.Add(Listeners.OnActivePlayerTurnEnd)

	Events.EndTurnBlockingChanged.Add(Listeners.OnEndTurnBlockingChanged)
end -- Set Listeners
--------------------

-- I don't know why, but it is called one time in previous version so I kept calling it
Listeners.OnEndTurnDirty()

-- Uncomment for debug
--ActionInfoPanel = {
--  MOD_BALANCE_VP = MOD_BALANCE_VP,
--  L = L,
--	FlashingStates = FlashingStates,
--	Map = Map,
--	Utils = Utils,
--	Actions = Actions,
--	Listeners = Listeners,
--	EndTurnBlockingTypeEffectMap = EndTurnBlockingTypeEffectMap,
--	EndTurnBlockingTypeToActivateNotificationMap = EndTurnBlockingTypeToActivateNotificationMap,
--}
include('CPK.lua')
include('IconSupport')

local lua_next = next
local lua_table_sort = table.sort
local lua_table_insert = table.insert

local C = Controls
local L = Locale.Lookup

local Show = CPK.UI.Control.Show
local Hide = CPK.UI.Control.Hide
local Refresh = CPK.UI.Control.Refresh
local MakeInstance = CPK.UI.Control.Instance.Make
local FreeInstance = CPK.UI.Control.Instance.Free

local IYT = InstantYieldType

local VIEW_LOGS = 1
local VIEW_OPTS = 2

local IYK_CITY = 1
local IYK_MILITARY = 2
local IYK_CIVILIAN = 3
local IYK_ESPIONAGE = 4
local IYK_MISC = 5
local IYK_SKIP = 6

--------------------------------------------------------------------------------

local GetInstantYieldText = (function()
	local iyt_name_map = {}

	for key, val in lua_next, IYT do
		iyt_name_map[val] = L('TXT_KEY_' .. key .. '_SHORT')
	end

	return function(iyt)
		return iyt_name_map[iyt] or L('TXT_KEY_INSTANT_YIELD_UNKNOWN', iyt)
	end
end)()

local GetInstantYieldKind = (function()
	local iyt_skip = {
		IYT.INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE,
		IYT.INSTANT_YIELD_TYPE_TR_PRODUCTION_SIPHON,
		IYT.INSTANT_YIELD_TYPE_CONVERSION_EXPO,
		IYT.INSTANT_YIELD_TYPE_PROMOTION_OBTAINED,
	}

	local iyt_city = {
		IYT.INSTANT_YIELD_TYPE_BIRTH,
		IYT.INSTANT_YIELD_TYPE_BIRTH_RETROACTIVE,
		IYT.INSTANT_YIELD_TYPE_CONSTRUCTION,
		IYT.INSTANT_YIELD_TYPE_BORDERS,
		IYT.INSTANT_YIELD_TYPE_U_PROD,
		IYT.INSTANT_YIELD_TYPE_PURCHASE,
		IYT.INSTANT_YIELD_TYPE_TILE_PURCHASE,
		IYT.INSTANT_YIELD_TYPE_FOUND,
		IYT.INSTANT_YIELD_TYPE_CONSTRUCTION_WONDER,
		IYT.INSTANT_YIELD_TYPE_FAITH_PURCHASE,
		IYT.INSTANT_YIELD_TYPE_REFUND,
		IYT.INSTANT_YIELD_TYPE_FAITH_REFUND,
		IYT.INSTANT_YIELD_TYPE_BIRTH_HOLY_CITY,
		IYT.INSTANT_YIELD_TYPE_WLTKD_START,
	}

	local iyt_military = {
		IYT.INSTANT_YIELD_TYPE_DEATH,
		IYT.INSTANT_YIELD_TYPE_F_CONQUEST,
		IYT.INSTANT_YIELD_TYPE_VICTORY,
		IYT.INSTANT_YIELD_TYPE_SCOUTING,
		IYT.INSTANT_YIELD_TYPE_ANCIENT_RUIN,
		IYT.INSTANT_YIELD_TYPE_UNIT_GIFT,
		IYT.INSTANT_YIELD_TYPE_LEVEL_UP,
		IYT.INSTANT_YIELD_TYPE_PILLAGE,
		IYT.INSTANT_YIELD_TYPE_VICTORY_GLOBAL,
		IYT.INSTANT_YIELD_TYPE_PILLAGE_GLOBAL,
		IYT.INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED,
		IYT.INSTANT_YIELD_TYPE_PILLAGE_UNIT,
		IYT.INSTANT_YIELD_TYPE_BULLY,
		IYT.INSTANT_YIELD_TYPE_CITY_DAMAGE,
		IYT.INSTANT_YIELD_TYPE_PLUNDER_TRADE_ROUTE,
	}

	local iyt_civilian = {
		IYT.INSTANT_YIELD_TYPE_GP_USE,
		IYT.INSTANT_YIELD_TYPE_GP_BORN,
		IYT.INSTANT_YIELD_TYPE_F_SPREAD,
		IYT.INSTANT_YIELD_TYPE_CONVERSION,
		IYT.INSTANT_YIELD_TYPE_SPREAD,
		IYT.INSTANT_YIELD_TYPE_CULTURE_BOMB,
		IYT.INSTANT_YIELD_TYPE_REMOVE_HERESY,
		IYT.INSTANT_YIELD_TYPE_TR_END,
		IYT.INSTANT_YIELD_TYPE_TR_MOVEMENT,
		IYT.INSTANT_YIELD_TYPE_TR_MOVEMENT_IN_FOREIGN,
		IYT.INSTANT_YIELD_TYPE_IMPROVEMENT_BUILD,
	}

	local iyt_espionage = {
		IYT.INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE,
		IYT.INSTANT_YIELD_TYPE_HEALING,
		IYT.INSTANT_YIELD_TYPE_TR_PRODUCTION_SIPHON,
		IYT.INSTANT_YIELD_TYPE_CONVERSION_EXPO,
		IYT.INSTANT_YIELD_TYPE_PROMOTION_OBTAINED,
	}

	local iyt_kind_map = {}

	for _, iyt in lua_next, iyt_skip do
		iyt_kind_map[iyt] = IYK_SKIP
	end

	for _, iyt in lua_next, iyt_city do
		iyt_kind_map[iyt] = IYK_CITY
	end

	for _, iyt in lua_next, iyt_military do
		iyt_kind_map[iyt] = IYK_MILITARY
	end

	for _, iyt in lua_next, iyt_civilian do
		iyt_kind_map[iyt] = IYK_CIVILIAN
	end

	for _, iyt in lua_next, iyt_espionage do
		iyt_kind_map[iyt] = IYK_ESPIONAGE
	end

	return function(iyt)
		return iyt_kind_map[iyt] or IYK_MISC
	end
end)()

--------------------------------------------------------------------------------

--- @type PlayerId | nil
local m_player_id = nil

--- @type integer | nil
local m_turn = nil

--- Current View
--- @type integer | nil
local m_view = nil

--- Opt instances
--- @type table<integer, table<string, Control>>
local m_opts = {}

--- Log instances
--- @type table<integer, table<string, Control>>
local m_logs = {}

--- Popup Info
--- @type table | nil
local m_info = nil

--------------------------------------------------------------------------------

local function FreeLogs()
	for _, log_inst in lua_next, m_logs do
		FreeInstance(log_inst)
	end
	m_logs = {}
end

--------------------------------------------------------------------------------

--- @param utmost boolean?
local function ShowPopup(utmost)
	UIManager:QueuePopup(
		ContextPtr,
		utmost and PopupPriority.InGameUtmost or PopupPriority.NotificationLog
	)
end

local function HidePopup()
	UIManager:DequeuePopup(ContextPtr)
end

--- @param log_n_id integer # Notification index
local function GoToEvent(log_n_id)
	HidePopup()
	UI.ActivateNotification(log_n_id)
end

--------------------------------------------------------------------------------

local function ShowLogsView()
	if m_view == VIEW_LOGS then return end

	Hide(
		C.OptsView,
		C.OptsViewDecor,
		C.EnableCurrOptsButton,
		C.DisableCurrOptsButton
	)
	Show(C.LogsView, C.LogsViewDecor)

	m_view = VIEW_LOGS
end

local function ShowOptsView()
	if m_view == VIEW_OPTS then return end

	Hide(C.LogsView, C.LogsViewDecor)
	Show(
		C.OptsView,
		C.OptsViewDecor,
		C.EnableCurrOptsButton,
		C.DisableCurrOptsButton
	)

	m_view = VIEW_OPTS
end

--------------------------------------------------------------------------------

--- @param opt_type integer
--- @param opt_enabled boolean
local function TickOpt(opt_type, opt_enabled)
	m_opts[opt_type].OptCheckbox:SetCheck(opt_enabled)
	Players[m_player_id]:SetInstantYieldNotificationDisabled(
		opt_type,
		not opt_enabled
	)
end

--- @param opt_enabled boolean
local function TickOpts(opt_enabled)
	for opt_type, opt_inst in lua_next, m_opts do
		if not opt_inst.OptButton:IsHidden() then
			TickOpt(opt_type, opt_enabled)
		end
	end
end

--- Shows options of specified kind. Hides other options.
--- @param opt_kind integer
local function FilterOpts(opt_kind)
	for _, opt_inst in lua_next, m_opts do
		if opt_kind == 0 then
			opt_inst.OptButton:SetHide(false)
		else
			opt_inst.OptButton:SetHide(opt_kind ~= opt_inst.OptButton:GetVoid2())
		end
	end

	Refresh(C.OptsStack, C.OptsScroll)
end

--------------------------------------------------------------------------------

local function FillTreatment()
	CivIconHookup(
		m_player_id,
		64,
		C.CivIcon,
		C.CivIconBg,
		C.CivIconShadow,
		false,
		true
	)
end

local function FillOptsView()
	local player = Players[m_player_id]

	for opt_type, opt_inst in lua_next, m_opts do
		opt_inst.OptCheckbox:SetCheck(
			not player:IsInstantYieldNotificationDisabled(opt_type)
		)
	end
end

local function FillLogsView()
	local player = Players[m_player_id]
	local count = player:GetNumNotifications() - 1
	local n_logs = {}

	for i = 0, count do
		local log_n_id = player:GetNotificationIndex(i)
		local log_dism = player:GetNotificationDismissed(i)
		local log_inst = m_logs[log_n_id]

		if log_inst then
			log_inst.LogButton:SetDisabled(log_dism)
			n_logs[log_n_id] = log_inst
			m_logs[log_n_id] = nil
		else
			local log_text = player:GetNotificationStr(i)
			local log_turn = player:GetNotificationTurn(i)

			log_inst = MakeInstance('LogInstance', C.LogsStack)

			log_inst.LogTurnLabel:SetText(L('TXT_KEY_TP_TURN_COUNTER', log_turn))
			log_inst.LogTextLabel:SetText(log_text)

			Refresh(log_inst.LogTextStack)

			local height = log_inst.LogTextStack:GetSizeY()

			log_inst.LogTextHoverElem:SetSizeY(height)
			log_inst.LogTextHoverAnim:SetSizeY(height)

			log_inst.LogButton:SetVoid1(log_n_id)
			log_inst.LogButton:SetSizeY(height)
			log_inst.LogButton:SetDisabled(log_dism)
			log_inst.LogButton:RegisterCallback(Mouse.eLClick, GoToEvent)

			n_logs[log_n_id] = log_inst
		end
	end

	FreeLogs()
	m_logs = n_logs

	Refresh(C.LogsStack, C.LogsScroll)
end

--------------------------------------------------------------------------------

local function InitOptsView()
	local IYK_TEXT_MAP = {
		[IYK_CITY] = L('TXT_KEY_NOTIFICATION_SETTINGS_CITY_BUTTON'),
		[IYK_CIVILIAN] = L('TXT_KEY_NOTIFICATION_SETTINGS_CIVILIAN_BUTTON'),
		[IYK_MILITARY] = L('TXT_KEY_NOTIFICATION_SETTINGS_MILITARY_BUTTON'),
		[IYK_ESPIONAGE] = L('TXT_KEY_NOTIFICATION_SETTINGS_SPIES_BUTTON'),
		[IYK_MISC] = L('TXT_KEY_NOTIFICATION_SETTINGS_MISC_BUTTON')
	}

	local opt_kind_callback = function(isChecked, opt_kind, _void2, _control)
		if isChecked then
			FilterOpts(opt_kind)
		end
	end

	for opt_kind = 0, 5 do
		local opt_kind_inst = MakeInstance('OptKindInstance', C.OptKindsStack)
		local opt_kind_text = IYK_TEXT_MAP[opt_kind]

		opt_kind_inst.OptKindButton:SetVoid1(opt_kind)
		opt_kind_inst.OptKindButton:SetCheck(not opt_kind_text)
		opt_kind_inst.OptKindButton:GetTextButton():SetText(opt_kind_text or 'All')
		opt_kind_inst.OptKindButton:RegisterCheckHandler(opt_kind_callback)
	end

	local array = {}
	local count = IYT.NUM_INSTANT_YIELD_TYPES - 1

	for opt_type = 0, count do
		local opt_kind = GetInstantYieldKind(opt_type)

		if opt_kind ~= IYK_SKIP then
			lua_table_insert(array, {
				Kind = opt_kind,
				Type = opt_type,
				KindText = IYK_TEXT_MAP[opt_kind],
				TypeText = GetInstantYieldText(opt_type)
			})
		end
	end

	lua_table_sort(array, function(a, b)
		if a.Kind == b.Kind then
			return a.TypeText < b.TypeText
		end
		return a.Kind < b.Kind
	end)

	local opt_check_callback = function(checked, opt_type, _, _)
		TickOpt(opt_type, checked)
	end

	local opt_click_callback = function(opt_type, _, _)
		local opt_inst = m_opts[opt_type]
		TickOpt(opt_type, not opt_inst.OptCheckbox:IsChecked())
	end

	for i = 1, #array do
		local opt_data = array[i]
		local opt_inst = MakeInstance('OptInstance', C.OptsStack)

		opt_inst.OptTextLabel:SetText(opt_data.TypeText)
		opt_inst.OptKindLabel:SetText(opt_data.KindText)

		opt_inst.OptCheckbox:SetVoid1(opt_data.Type)
		opt_inst.OptCheckbox:SetVoid2(opt_data.Kind)

		opt_inst.OptButton:SetVoid1(opt_data.Type)
		opt_inst.OptButton:SetVoid2(opt_data.Kind)

		opt_inst.OptButton:RegisterCallback(Mouse.eLClick, opt_click_callback)

		opt_inst.OptCheckbox:RegisterCheckHandler(opt_check_callback)

		m_opts[opt_data.Type] = opt_inst
	end

	Refresh(C.OptKindsStack, C.OptsStack, C.OptsScroll)
end

local function Init()
	m_player_id = Game.GetActivePlayer()

	FillTreatment()

	InitOptsView()

	FillOptsView()
	FillLogsView()

	ShowLogsView()

	C.CloseButton:RegisterCallback(Mouse.eLClick, HidePopup)

	C.ShowLogsView:RegisterCallback(Mouse.eLClick, ShowLogsView)
	C.ShowOptsView:RegisterCallback(Mouse.eLClick, ShowOptsView)

	C.EnableCurrOptsButton:RegisterCallback(Mouse.eLClick, function()
		TickOpts(true)
	end)

	C.DisableCurrOptsButton:RegisterCallback(Mouse.eLClick, function()
		TickOpts(false)
	end)
end

--------------------------------------------------------------------------------

ContextPtr:SetInputHandler(function(uiMsg, wParam)
	if uiMsg ~= KeyEvents.KeyDown then return end

	if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
		HidePopup()
		return true
	end
end)

ContextPtr:SetShowHideHandler(function(isHide, isInit)
	if isInit then
		Init()
		return
	end

	local turn = Game.GetGameTurn()
	local player_id = Game.GetActivePlayer()

	if player_id ~= m_player_id then
		m_player_id = player_id

		FreeLogs()
		FillOptsView()
		FillLogsView()
		FillTreatment()
	elseif m_turn ~= turn then
		m_turn = turn
		FillLogsView()
	end

	if isHide then
		UI.decTurnTimerSemaphore()
		Events.SerialEventGameMessagePopupProcessed.CallImmediate(
			ButtonPopupTypes.BUTTONPOPUP_NOTIFICATION_LOG,
			0
		)
	else
		UI.incTurnTimerSemaphore()
		Events.SerialEventGameMessagePopupShown(m_info)
	end
end)

--------------------------------------------------------------------------------

Events.GameplaySetActivePlayer.Add(HidePopup)

Events.SerialEventGameMessagePopup.Add(function(info)
	if info.Type ~= ButtonPopupTypes.BUTTONPOPUP_NOTIFICATION_LOG then return end

	m_info = info

	if info.Data1 ~= 1 then
		ShowPopup(false)
		return
	end

	if ContextPtr:IsHidden() then
		ShowPopup(true)
	else
		HidePopup()
	end
end)

--------------------------------------------------------------------------------

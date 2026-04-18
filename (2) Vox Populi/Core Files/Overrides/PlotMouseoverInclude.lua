include('CPK.lua')

local lua_next = next
local lua_math_floor = math.floor
local lua_string_format = string.format

local civ_db_query = DB.Query
local civ_db_create_query = DB.CreateQuery
local Memoize1 = CPK.Cache.Memoize1
local TableHashify = CPK.Table.Hashify
local StringBuilder = CPK.Util.StringBuilder

local ColorPositive = CPK.Text.Color.Positive
local ColorNegative = CPK.Text.Color.Negative
local ColorWhite = CPK.Text.Color.White

local L = Locale.Lookup
local ML = Memoize1(L)

--------------------------------------------------------------------------------

--- @param table_name string
local MakeDescGetter = function(table_name)
	local query = civ_db_create_query(
		lua_string_format(
			'SELECT Description FROM %s WHERE ID = :id LIMIT 1',
			table_name
		)
	)

	--- -1 indicates row not found or localization returned nil
	--- @type table<integer, -1 | string>
	local cache = {}

	--- @param id integer
	--- @return string
	return function(id)
		local str = cache[id]

		if str == nil then
			local row = query(id)()

			str = (row and row.Description and L(row.Description)) or -1
			cache[id] = str
		end

		return str ~= -1 and str --[[@as string]] or ''
	end
end

local GetRouteText = MakeDescGetter('Routes')
local GetFeatureText = MakeDescGetter('Features')
local GetTerrainText = MakeDescGetter('Terrains')
local GetImprovementText = MakeDescGetter('Improvements')

--------------------------------------------------------------------------------

GetYieldString = (function()
	--- @param sb StringBuilder
	--- @param yieldId integer
	local append_yield = function(plot, sb, icon, yieldId)
		local num = plot:CalculateYield(yieldId, true)

		if num > 0 then
			sb:Append(icon .. ' ' .. num)
		end
	end

	local YIELD_FOOD = YieldTypes.YIELD_FOOD
	local YIELD_PROD = YieldTypes.YIELD_PRODUCTION
	local YIELD_GOLD = YieldTypes.YIELD_GOLD
	local YIELD_SCIENCE = YieldTypes.YIELD_SCIENCE
	local YIELD_CULTURE = YieldTypes.YIELD_CULTURE
	local YIELD_FAITH = YieldTypes.YIELD_FAITH

	local NO_FEATURE = FeatureTypes.NO_FEATURE

	local gp_rate_modifier_map = {}
	local in_border_happiness_map = {}

	for row in civ_db_query('SELECT ID, InBorderHappiness FROM Features') do
		in_border_happiness_map[row.ID] = row.InBorderHappiness or 0
	end

	for row in civ_db_query('SELECT ID, GreatPersonRateModifier From Improvements') do
		gp_rate_modifier_map[row.ID] = row.GreatPersonRateModifier or 0
	end

	--- @param plot Plot
	--- @return string
	return function(plot)
		local text = StringBuilder.New()
		local featureId = plot:GetFeatureType()
		local improvementId = plot:GetRevealedImprovementType(
			Game.GetActiveTeam(),
			Game.IsDebugMode()
		)

		append_yield(plot, text, '[ICON_FOOD]', YIELD_FOOD)
		append_yield(plot, text, '[ICON_PRODUCTION]', YIELD_PROD)
		append_yield(plot, text, '[ICON_GOLD]', YIELD_GOLD)
		append_yield(plot, text, '[ICON_RESEARCH]', YIELD_SCIENCE)
		append_yield(plot, text, '[ICON_CULTURE]', YIELD_CULTURE)
		append_yield(plot, text, '[ICON_PEACE]', YIELD_FAITH)

		if featureId ~= NO_FEATURE then
			local playerId = Game.GetActivePlayer()
			local modifier = Players[playerId]:GetNaturalWonderYieldModifier()
			local happiness = in_border_happiness_map[featureId]

			if happiness and modifier > 0 then
				happiness = lua_math_floor(
					(happiness * (100 * modifier)) / 100
				)

				if happiness > 0 then
					text:Append('[ICON_HAPPINESS_1] ' .. happiness)
				end
			end
		end

		if improvementId >= 0 and not plot:IsImprovementPillaged() then
			local modifier = gp_rate_modifier_map[improvementId]
			if modifier ~= 0 then
				text:Append('[ICON_GREAT_PEOPLE] ' .. modifier .. '%')
			end
		end

		return text:Concat(' ')
	end
end)()

GetNatureString = (function()
	local is_special = TableHashify({
		GameInfoTypes.FEATURE_JUNGLE,
		GameInfoTypes.FEATURE_MARSH,
		GameInfoTypes.FEATURE_OASIS,
		GameInfoTypes.FEATURE_ICE,
	})

	local is_wonder = {}

	for row in civ_db_query('SELECT ID FROM Features WHERE NaturalWonder = 1') do
		is_wonder[row.ID] = true
	end

	local TEXT_LAKE = L('TXT_KEY_PLOTROLL_LAKE')
	local TEXT_HILL = L('TXT_KEY_PLOTROLL_HILL')
	local TEXT_RIVER = L('TXT_KEY_PLOTROLL_RIVER')
	local TEXT_MOUNTAIN = L('TXT_KEY_PLOTROLL_MOUNTAIN')

	return function(plot)
		local text = StringBuilder.New()
		local featureId = plot:GetFeatureType()

		local hasFeature = featureId > -1
		local isMountain = not hasFeature and plot:IsMountain()
		local shouldShowTerrain = not isMountain and not (
			(hasFeature and (is_special[featureId] or is_wonder[featureId]))
		)

		if hasFeature then
			text:Append(GetFeatureText(featureId))
		elseif isMountain then
			text:Append(TEXT_MOUNTAIN)
		end

		if shouldShowTerrain then
			if plot:IsLake() then
				text:Append(TEXT_LAKE)
			else
				text:Append(GetTerrainText(plot:GetTerrainType()))
			end
		end

		if plot:IsHills() then text:Append(TEXT_HILL) end
		if plot:IsRiver() then text:Append(TEXT_RIVER) end

		return text:Concat(', ')
	end
end)()

--- @param plot Plot
--- @param isLongForm boolean | nil
GetResourceString = function(plot, isLongForm)
	local resourceId = plot:GetResourceType(Game.GetActiveTeam())

	if resourceId < 0 then
		return ''
	end

	local text = StringBuilder.New()
	local player = Players[Game.GetActivePlayer()]
	local resource = GameInfo.Resources[resourceId]
	local resourceNum = plot:GetNumResource()
	local resourceStr = ML(resource.Description)

	if resourceNum > 1 then
		text:Append(resourceNum)
	end

	text:Append(resource.IconString):Append(resourceStr)

	if not player:IsResourceImproveable(resourceId) then
		local imprTechType = GameInfoTypes[resource.TechImproveable]
		local imprTechDesc = GameInfo.Technologies[imprTechType].Description

		if isLongForm then
			text:Append(L('TXT_KEY_PLOTROLL_REQUIRES_TECH_TO_USE', imprTechDesc))
		else
			text:Append(L('TXT_KEY_PLOTROLL_REQUIRES_TECH', imprTechDesc))
		end
	end

	return text:Concat(' ')
end

local GetRouteString = function(plot)
	local routeId = plot:GetRevealedRouteType(
		Game.GetActiveTeam(),
		Game.IsDebugMode()
	)

	if routeId > -1 then
		if plot:IsRoutePillaged() then
			return GetRouteText(routeId) .. ' ' .. ML('TXT_KEY_PLOTROLL_PILLAGED')
		else
			return GetRouteText(routeId)
		end
	end

	return ''
end

GetImprovementString = (function()
	local TEXT_PILLAGED = L('TXT_KEY_PLOTROLL_PILLAGED')
	local TEXT_EMBASSY_UNMET = L('TXT_KEY_PLOTROLL_EMBASSY_UNMET')

	--- @param plot Plot
	return function(plot)
		local activeTeamId = Game.GetActiveTeam()
		local activeTeam = Teams[activeTeamId]
		local text = ''

		local improvementId = plot:GetRevealedImprovementType(activeTeamId, Game.IsDebugMode())

		if improvementId > -1 then
			if plot:IsImprovementPillaged() then
				text = GetImprovementText(improvementId) .. ' ' .. TEXT_PILLAGED
			else
				text = GetImprovementText(improvementId)
			end

			if plot:IsImprovementEmbassy() then
				local otherPlayerId = plot:GetPlayerThatBuiltImprovement()
				local otherPlayer = otherPlayerId and Players[otherPlayerId]
				local otherTeamId = otherPlayer and otherPlayer:GetTeam()

				if otherPlayer and activeTeam:IsHasMet(otherTeamId) then
					text = text .. ' ' .. L(
						'TXT_KEY_PLOTROLL_EMBASSY',
						otherPlayer:GetCivilizationShortDescriptionKey()
					)
				else
					text = text .. ' ' .. TEXT_EMBASSY_UNMET
				end
			end
		end

		local routeText = GetRouteString(plot)

		if routeText ~= '' then
			if text ~= '' then
				return text .. ', ' .. routeText
			else
				return routeText
			end
		end

		return text
	end
end)()

GetCivStateQuestString = (function()
	local mcqt = MinorCivQuestTypes
	local fallback = ColorPositive(L('TXT_KEY_CITY_STATE_BARB_QUEST_SHORT'))

	local quests = {
		[mcqt.MINOR_CIV_QUEST_KILL_CAMP] = 'TXT_KEY_CITY_STATE_BARB_QUEST_LONG',
		[mcqt.MINOR_CIV_QUEST_ARCHAEOLOGY] = 'TXT_KEY_CITY_STATE_ARCHAEOLOGY_QUEST_LONG',
		[mcqt.MINOR_CIV_QUEST_ACQUIRE_CITY] = 'TXT_KEY_CITY_STATE_ACQUIRE_CITY_QUEST_LONG',
		[mcqt.MINOR_CIV_QUEST_EXPLORE_AREA] = 'TXT_KEY_CITY_STATE_EXPLORE_AREA_QUEST_LONG'
	}

	--- @param plot Plot
	--- @param isShortForm boolean | nil
	return function(plot, isShortForm)
		local text = StringBuilder.New()

		local activeTeamId = Game.GetActiveTeam()
		local activePlayerId = Game.GetActivePlayer()
		local activeTeam = Teams[activeTeamId]

		local px, py = plot:GetX(), plot:GetY()
		local from_idx = GameDefines.MAX_MAJOR_CIVS
		local upto_idx = GameDefines.MAX_CIV_PLAYERS - 1

		for otherPlayerId = from_idx, upto_idx do
			local otherPlayer = Players[otherPlayerId]
			local otherTeamId = otherPlayer:GetTeam()

			local cond = activeTeamId ~= otherTeamId
					and otherPlayer:IsMinorCiv()
					and otherPlayer:IsAlive()
					and activeTeam:IsHasMet(otherTeamId)

			if cond then
				for questId, questDesc in lua_next, quests do
					if otherPlayer:IsMinorCivDisplayedQuestForPlayer(activePlayerId, questId) then
						local x = otherPlayer:GetQuestData1(activePlayerId, questId)
						local y = otherPlayer:GetQuestData2(activePlayerId, questId)

						if x == px and y == py then
							if isShortForm then
								return fallback
							else
								text:Append(
									ColorPositive(
										L(questDesc, otherPlayer:GetCivilizationShortDescriptionKey())
									)
								)
							end
						end
					end
				end
			end
		end

		return text:Concat('[NEWLINE]')
	end
end)()

--- @param plot Plot
--- @return string
GetInternationalTradeRouteString = function(plot)
	local activePlayerId = Game.GetActivePlayer()
	local activePlayer = Players[activePlayerId]
	local routes = activePlayer:GetInternationalTradeRoutePlotToolTip(plot)

	if not routes or not routes[1] then
		return ''
	end

	local text = StringBuilder.New()

	text:Append(ML('TXT_KEY_TRADE_ROUTE_TT_PLOT_HEADING'))

	for i = 1, #routes do
		text:Append(routes[i].String)
	end

	return text:Concat('[NEWLINE]')
end

--- @param plot Plot
--- @return string
GetOwnerString = function(plot)
	if plot:IsCity() then
		local city = plot:GetPlotCity()

		if not city then return '' end

		local ownerId = city:GetOwner()
		local owner = Players[ownerId]

		if not owner then return '' end

		if owner:IsMinorCiv() then
			return L(
				'TXT_KEY_CITY_STATE_OF',
				owner:GetCivilizationShortDescriptionKey()
			)
		else
			return L(
				'TXT_KEY_CITY_OF',
				owner:GetCivilizationAdjectiveKey(),
				city:GetName()
			)
		end
	end

	local activeTeamId = Game.GetActiveTeam()
	local activeTeam = Teams[activeTeamId]
	local ownerId = plot:GetRevealedOwner(activeTeamId, Game.IsDebugMode())

	if ownerId < 0 then return '' end

	local owner = Players[ownerId]
	local ownerNick = owner:GetNickName()
	local ownerTeamId = owner:GetTeam()

	local ownerTitle = (ownerNick and ownerNick ~= '')
			and L('TXT_KEY_PLOTROLL_OWNED_PLAYER', ownerNick)
			or L('TXT_KEY_PLOTROLL_OWNED_CIV', owner:GetCivilizationShortDescriptionKey())

	if ownerTeamId == activeTeamId then
		return ColorWhite(ownerTitle)
	end

	if activeTeam:IsAtWar(ownerTeamId) then
		return ColorNegative(ownerTitle)
	end

	return ColorPositive(ownerTitle)
end

--- @param plot Plot
--- @return string
GetUnitsString = function(plot)
	local count = plot:GetNumUnits()

	if count == 0 then return '' end

	local activeTeamId = Game.GetActiveTeam()
	local activeTeam = Teams[activeTeamId]
	local debug = Game.IsDebugMode()
	local text = StringBuilder.New()

	for i = 0, count - 1 do
		local unit = plot:GetUnit(i) --[[@as Unit]]

		if unit
				and not unit:IsInvisible(activeTeamId, debug)
				and not (
					unit:IsCargo()
					and unit:GetTransportUnit():IsInvisible(activeTeamId, debug)
				)
		then
			local unitStr = ''
			local unitTeamId = unit:GetTeam()

			local unitOwner = Players[unit:GetOwner()]
			local unitOwnerAdj = unitOwner:GetCivilizationAdjectiveKey()
			local unitOwnerNick = unitOwner:GetNickName()

			local unitDamage = unit:GetDamage()
			local unitNameKey = unit:GetNameKey()
			local unitStrength = unit:GetBaseCombatStrength()

			if unitOwnerNick and unitOwnerNick ~= '' then
				unitStr = L(
					'TXT_KEY_MULTIPLAYER_UNIT_TT',
					unitOwnerNick,
					unitOwnerAdj,
					unitNameKey
				)
			elseif unit:HasName() then
				unitStr = lua_string_format(
					'%s (%s)',
					ML(unit:GetNameNoDesc()),
					L(
						'TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV',
						unitOwnerAdj,
						unitNameKey
					)
				)
			else
				unitStr = L(
					'TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV',
					unitOwnerAdj,
					unitNameKey
				)
			end

			if unitTeamId == activeTeamId then
				unitStr = ColorWhite(unitStr)
			elseif activeTeam:IsAtWar(unitTeamId) then
				unitStr = ColorNegative(unitStr)
			else
				unitStr = ColorPositive(unitStr)
			end

			if OptionsManager:IsDebugMode() then
				unitStr = unitStr .. " (" .. unit:GetOwner() .. " - " .. unit:GetID() .. ")"
			end

			if unitStrength > 0 then
				unitStr = unitStr .. ", [ICON_STRENGTH]" .. unitStrength
			end

			if unitDamage > 0 then
				local max = unit:GetMaxHitPoints()
				unitStr = unitStr .. ", " .. L(
					'TXT_KEY_PLOTROLL_UNIT_HP',
					max - unitDamage
				) .. '/' .. max
			end

			if unit:IsEmbarked() then
				unitStr = unitStr .. ', ' .. ML('TXT_KEY_PLOTROLL_EMBARKED')
			end


			text:Append(unitStr)
		end
	end

	return text:Concat('[NEWLINE]')
end

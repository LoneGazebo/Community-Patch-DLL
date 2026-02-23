include('CPK.lua')

local StringBuilder = CPK.Util.StringBuilder
local AsPercentage = CPK.Misc.AsPercentage
local Memoize1 = CPK.Cache.Memoize1

local Cyan = CPK.Text.Color.Cyan
local Yellow = CPK.Text.Color.Yellow

local lua_type = type
local lua_math_floor = math.floor
local lua_string_format = string.format

local civ_locale_lookup = Locale.Lookup
local civ_locale_toupper = Locale.ToUpper
local civ_db_create_query = DB.CreateQuery

local L = civ_locale_lookup

local ICON_BULLET = '[ICON_BULLET]'

--- @param row { Description: string, Adjective: string? }
--- @return string
local function BulletAdj(row)
	local str = L(row.Description)
	local adj = row.Adjective

	if not adj then
		return ICON_BULLET .. str
	end

	return ICON_BULLET .. Yellow(L(adj)) .. ' ' .. str
end

--- @param row { Description: string, Civilization: string? }
--- @return string
local function BulletCiv(row)
	local str = L(row.Description)
	local civ = row.Civilization

	if not civ then
		return ICON_BULLET .. str
	end

	return ICON_BULLET .. str .. ' ' .. Yellow('(' .. L(civ) .. ')')
end

--- @param row { Description: string }
--- @return string
local function BulletLoc(row)
	return ICON_BULLET .. L(row.Description)
end

--- @generic Row : { Description: string }
--- @param text StringBuilder
--- @param type string # Tech type, e.g. 'TECH_COMPUTERS'
--- @param iter fun(): Row | nil
--- @param map fun(row: Row, type: string): string | nil
local function WriteContent(text, type, iter, map)
	local str = nil

	for row in iter do
		str = map(row, type)

		if str then
			text:Append(str)
		end
	end
end

--- @generic Row : { Description: string }
--- @param sql string
--- @param map fun(row: Row, type: string): string | nil
--- @return fun(text: StringBuilder, type: string): nil
local function PrepareContent(sql, map)
	local query = civ_db_create_query(sql)

	return function(text, type)
		WriteContent(text, type, query(type), map)
	end
end

--- @generic Row : { Description: string }
--- @param name string # Section name, e.g. 'Builds Unlocked'
--- @param text StringBuilder
--- @param type string # Tech type, e.g. 'TECH_GUILDS'
--- @param iter fun(): Row | nil
--- @param map fun(row: Row, type: string): string | nil
local function WriteSection(name, text, type, iter, map)
	local row
	local str

	repeat
		row = iter()

		if not row then
			return
		end

		str = map(row, type)
	until str

	text:Append(name):Append(str --[[@as string]])
	WriteContent(text, type, iter, map)
end

--- @generic Row : { Description: string }
--- @param name string # Section name text key e.g. 'TXT_KEY_TECH_GUILDS'
--- @param sql string
--- @param map fun(row: Row, type: string): string | nil
--- @return fun(text: StringBuilder, type: string): nil
local function PrepareSection(name, sql, map)
	local query = civ_db_create_query(sql)
	name = Cyan(L(name))

	return function(text, type)
		WriteSection(name, text, type, query(type), map)
	end
end

local WriteLeadsTo = PrepareSection(
	'TXT_KEY_TECH_HELP_LEADS_TO',
	[[
		SELECT t.Description
		FROM Technologies t
		INNER JOIN Technology_PrereqTechs p ON p.TechType = t.Type
		WHERE p.PrereqTech = ?
	]],
	BulletLoc
)

local WriteUnits = PrepareSection(
	'TXT_KEY_TECH_HELP_UNITS_UNLOCKED',
	[[
		SELECT u.Description, c.Adjective
		FROM Units u
		LEFT JOIN Civilization_UnitClassOverrides o ON o.UnitType = u.Type
		LEFT JOIN Civilizations c ON c.Type = o.CivilizationType
		WHERE u.ShowInPedia = 1 AND u.PrereqTech = ?
		ORDER BY c.Adjective IS NOT NULL, o.UnitClassType, c.Adjective
	]],
	BulletAdj
)

local buildingsSqlTemplate = [[
	SELECT b.Description, c.Adjective
	FROM Buildings b
	INNER JOIN BuildingClasses bc ON bc.Type = b.BuildingClass
	LEFT JOIN Civilization_BuildingClassOverrides o ON o.BuildingType = b.Type
	LEFT JOIN Civilizations c
		ON c.Type = o.CivilizationType
		OR c.Type = b.CivilizationRequired
	WHERE b.ShowInPedia = 1
		AND bc.MaxGlobalInstances %s 0
		AND b.PrereqTech = ?
	ORDER BY
		c.Adjective IS NOT NULL,
		b.Description,
		c.Adjective
]]

local WriteWonders = PrepareSection(
	'TXT_KEY_TECH_HELP_WONDERS_UNLOCKED',
	lua_string_format(buildingsSqlTemplate, '>'),
	BulletAdj
)

local WriteBuildings = PrepareSection(
	'TXT_KEY_TECH_HELP_BUILDINGS_UNLOCKED',
	lua_string_format(buildingsSqlTemplate, '<='),
	BulletAdj
)

local WriteProjects = PrepareSection(
	'TXT_KEY_TECH_HELP_PROJECTS_UNLOCKED',
	[[
		SELECT Description
		FROM Projects
		WHERE TechPrereq = ?
		ORDER BY Description
	]],
	BulletLoc
)

local WriteProcesses = PrepareSection(
	'TXT_KEY_TECH_HELP_PROCESSES_UNLOCKED',
	[[
		SELECT Description
		FROM Processes
		WHERE TechPrereq = ?
		ORDER BY Description
	]],
	BulletLoc
)

local WriteCorporations = PrepareSection(
	'TXT_KEY_TECH_HELP_CORPS_UNLOCKED',
	[[ SELECT Description FROM Corporations ORDER BY Description ]],
	BulletLoc
)

local WriteResources = PrepareSection(
	'TXT_KEY_TECH_HELP_RESOURCES_UNLOCKED',
	[[
		SELECT Description, IconString
		FROM Resources WHERE TechReveal = ?
		ORDER BY Description
	]],
	function(row)
		return ICON_BULLET .. row.IconString .. ' ' .. L(row.Description)
	end
)

local WriteBuilds = (function()
	local FillBuildsDefault = PrepareContent(
		[[
			SELECT
				b.Description as Description,
				c.ShortDescription as Civilization
			FROM Builds b
			INNER JOIN Improvements i ON i.Type = b.ImprovementType
			LEFT JOIN Civilizations c ON c.Type = i.CivilizationType
			WHERE ShowInPedia = 1 AND PrereqTech = ?
			ORDER BY Civilization IS NOT NULL, Description, Civilization
		]],
		BulletCiv
	)

	local FillBuildsReduction = PrepareContent(
		[[
			SELECT
				CASE WHEN b.Time IS NULL THEN (
					SELECT bf.Time FROM BuildFeatures bf WHERE bf.BuildType = c.BuildType
				) ELSE b.Time
				END as Time,
				c.TimeChange as Change,
				b.Description as Description,
				civ.ShortDescription as Civilization
			FROM Build_TechTimeChanges c
			INNER JOIN Builds b ON b.Type = c.BuildType
			LEFT JOIN Improvements i ON i.Type = b.ImprovementType
			LEFT JOIN Civilizations civ ON civ.Type = i.CivilizationType
			WHERE c.TechType = ?
			ORDER BY Civilization IS NOT NULL, Description, Civilization
		]],
		function(row)
			local time = row.Time
			local change = row.Change
			local description = row.Description
			local civilization = row.Civilization

			local percent = AsPercentage(change / time)
			local str = ICON_BULLET .. L(
				'TXT_KEY_TECH_HELP_BUILD_REDUCTION',
				description,
				percent
			)

			if civilization then
				return str .. ' ' .. Yellow('(' .. L(civilization) .. ')')
			end

			return str
		end
	)

	--- @type fun(text: StringBuilder, type: string): nil
	return function(text, type)
		local size = text:Size()

		FillBuildsDefault(text, type)
		FillBuildsReduction(text, type)

		if size ~= text:Size() then
			text:Insert(size + 1, Cyan(L('TXT_KEY_TECH_HELP_BUILDS_UNLOCKED')))
		end
	end
end)()

local WriteAbilities = (function()
	--- @param text StringBuilder
	--- @param type string
	--- @param tech { FeatureProductionModifier: nil | number }
	local function FillAbilityRemoveForestBonus(text, type, tech)
		local prop = tech.FeatureProductionModifier

		if lua_type(prop) ~= 'number' then
			return
		end

		if prop <= 0 then
			return
		end

		local hasGame = not not Game
		-- Take standard game speed if not game
		local speedType = hasGame and Game.GetGameSpeedType() or 'GAMESPEED_STANDARD'

		local speed = GameInfo.GameSpeeds[speedType]
		local feature = GameInfo.BuildFeatures({
			BuildType = 'BUILD_REMOVE_FOREST'
		})()
		local percent = speed.BuildPercent

		local baseChopYield = feature and feature.Production or 0
		local gameChopYield = baseChopYield * percent / 100

		local bonusChopYield = lua_math_floor(gameChopYield * prop / 100)

		local str = ICON_BULLET
				.. L('TXT_KEY_ABLTY_TECH_BOOST_CHOP', bonusChopYield)

		-- If there is no Game
		-- Indicate that bonus is on standard speed
		if not hasGame then
			str = str .. Yellow('(' .. L(speed.Description) .. ')')
		end

		text:Append(str)
	end

	local strSeaTrade = ICON_BULLET .. L('TXT_KEY_EXTENDS_SEA_TRADE_ROUTE_RANGE')
	local strLandTrade = ICON_BULLET .. L('TXT_KEY_EXTENDS_LAND_TRADE_ROUTE_RANGE')

	local FillAbilityTradeRouteRangeExtend = PrepareContent(
		[[
			SELECT d.Type = 'DOMAIN_SEA' as IsSea
			FROM Technology_TradeRouteDomainExtraRange r
			INNER JOIN Domains d ON d.Type = r.DomainType
			WHERE r.DomainType IN ('DOMAIN_SEA', 'DOMAIN_LAND')
				AND r.Range IS NOT NULL
				AND r.Range > 0
				AND r.TechType = ?
		]],
		function(row)
			return row.IsSea ~= 0 and strSeaTrade or strLandTrade
		end
	)

	local function PrepareNumericAbility(key, txtKey)
		return function(text, type, tech)
			local prop = tech[key]

			if lua_type(prop) == 'number' and prop > 0 then
				local str = ICON_BULLET .. L(txtKey, prop)
				text:Append(str)
			end
		end
	end

	local FillAbilityFreePromotions = PrepareContent(
		[[
			SELECT up.Description, up.Help
			FROM Technology_FreePromotions fp
			INNER JOIN UnitPromotions up ON up.Type = fp.PromotionType
			WHERE fp.TechType = ?
			ORDER BY up.Description
		]],
		function(row)
			return ICON_BULLET .. L(
				'TXT_KEY_FREE_PROMOTION_FROM_TECH',
				row.Description,
				row.Help
			)
		end
	)

	local abilities = { -- order is important!
		FillAbilityRemoveForestBonus,

		{ 'EmbarkedMoveChange',       'TXT_KEY_ABLTY_FAST_EMBARK_STRING' },
		{ 'AllowsEmbarking',          'TXT_KEY_ALLOWS_EMBARKING' },
		{ 'AllowsDefensiveEmbarking', 'TXT_KEY_ABLTY_DEFENSIVE_EMBARK_STRING' },
		{ 'EmbarkedAllWaterPassage',  'TXT_KEY_ABLTY_OCEAN_EMBARK_STRING' },

		PrepareNumericAbility('Happiness', 'TXT_KEY_TECH_HELP_HAPPINESS_GLOBAL'),

		{ 'BombardRange',                   'TXT_KEY_ABLTY_CITY_RANGE_INCREASE' },
		{ 'BombardIndirect',                'TXT_KEY_ABLTY_CITY_INDIRECT_INCREASE' },
		{ 'CityLessEmbarkCost',             'TXT_KEY_TECH_HELP_REDUCED_EMBARK_COST' },
		{ 'CityNoEmbarkCost',               'TXT_KEY_TECH_HELP_NO_EMBARK_COST' },
		{ 'AllowEmbassyTradingAllowed',     'TXT_KEY_ABLTY_ALLOW_EMBASSY_STRING' },
		{ 'OpenBordersTradingAllowed',      'TXT_KEY_ABLTY_OPEN_BORDER_STRING' },
		{ 'DefensivePactTradingAllowed',    'TXT_KEY_ABLTY_D_PACT_STRING' },
		{ 'BridgeBuilding',                 'TXT_KEY_ABLTY_BRIDGE_STRING' },
		{ 'MapVisible',                     'TXT_KEY_REVEALS_ENTIRE_MAP' },
		{ 'InternationalTradeRoutesChange', 'TXT_KEY_ADDITIONAL_INTERNATIONAL_TRADE_ROUTE' },
		FillAbilityTradeRouteRangeExtend,

		{ 'InfluenceSpreadModifier', 'TXT_KEY_DOUBLE_TOURISM' },
		{ 'AllowsWorldCongress',     'TXT_KEY_ALLOWS_WORLD_CONGRESS' },

		PrepareNumericAbility('ExtraVotesPerDiplomat', 'TXT_KEY_EXTRA_VOTES_FROM_DIPLOMATS'),
		FillAbilityFreePromotions,

		{ 'ResearchAgreementTradingAllowed', 'TXT_KEY_ABLTY_R_PACT_STRING' },
		{ 'MapTrading',                      'TXT_KEY_ABLTY_MAP_TRADING_STRING' },
		{ 'TechTrading',                     'TXT_KEY_ABLTY_TECH_TRADING_STRING' },
		{ 'VassalageTradingAllowed',         'TXT_KEY_ABLTY_VASSALAGE_STRING' },
		{ 'CorporationsEnabled',             'TXT_KEY_ABLTY_ENABLES_CORPORATIONS' },
	}

	local len = #abilities

	-- Prepare static translations
	for i = 1, len do
		local ability = abilities[i]

		if lua_type(ability) == 'table' then
			local str = ability[2]

			if lua_type(str) == 'string' then
				ability[2] = L(str)
			end
		end
	end

	--- @param text StringBuilder
	--- @param type string
	--- @param tech table
	return function(text, type, tech)
		local size = text:Size()

		for i = 1, len do
			local ability = abilities[i]

			if lua_type(ability) == 'function' then
				ability(text, type, tech)
			else
				local prop = tech[ability[1]]
				local str = ability[2]

				local condA = lua_type(prop) == 'boolean' and prop
				local condB = lua_type(prop) == 'number' and prop > 0

				if condA or condB then
					text:Append(ICON_BULLET .. str)
				end
			end
		end

		if size ~= text:Size() then
			text:Insert(size + 1, Cyan(L('TXT_KEY_TECH_HELP_ACTIONS_UNLOCKED')))
		end
	end
end)()


local WriteSpecialistYieldChanges = PrepareSection(
	'TXT_KEY_TECH_HELP_SPECIALIST_YIELDS_UNLOCKED',
	[[
		SELECT
			c.Yield as Yield,
			y.IconString as YieldIconString,
			y.Description as YieldDescription,
			s.Description as Description
		FROM Tech_SpecialistYieldChanges c
		INNER JOIN Yields y ON y.Type = c.YieldType
		INNER JOIN Specialists s ON s.Type = c.SpecialistType
		WHERE c.TechType = ?
	]],
	function(row)
		return ICON_BULLET .. L(
			'TXT_KEY_SPECIALIST_YIELD_CHANGE',
			row.Description,
			row.YieldDescription,
			row.Yield,
			row.YieldIconString
		)
	end
)

local function GetYieldChangesSql()
	local sql = StringBuilder.New()

	local template = [[
		SELECT
			'%s' as TxtKey,
			t.Yield as Yield,
			y.IconString as YieldIconString,
			y.Description as YieldDescription,
			s.Description as Description,
			c.Adjective as Adjective
		FROM %s t
		INNER JOIN Yields y ON y.Type = t.YieldType
		INNER JOIN Improvements s ON s.Type = t.ImprovementType
		LEFT JOIN Civilizations c ON c.Type = s.CivilizationType
		WHERE t.TechType = :type
	]]

	local selects = {
		{
			tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_YIELDCHANGES',
			src = 'Improvement_TechYieldChanges'
		},
		{
			tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_NOFRESHWATERYIELDCHANGES',
			src = 'Improvement_TechNoFreshWaterYieldChanges',
		},
		{
			tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_FRESHWATERYIELDCHANGES',
			src = 'Improvement_TechFreshWaterYieldChanges'
		}
	}

	for i = 1, #selects do
		local select = selects[i]
		local tag = select.tag
		local src = select.src

		sql:Append(lua_string_format(template, tag, src))
	end

	return 'SELECT * FROM (' .. sql:Concat(' UNION ALL ') .. [[
		) ORDER BY
			Adjective IS NOT NULL,
			Adjective
	]]
end

local WriteYieldChanges = PrepareSection(
	'TXT_KEY_TECH_HELP_YIELDS_UNLOCKED',
	GetYieldChangesSql(),
	function(row)
		local str = L(
			row.TxtKey,
			row.Description,
			row.YieldDescription,
			row.Yield,
			row.YieldIconString
		)

		local adj = row.Adjective

		if adj then
			return ICON_BULLET .. Yellow(L(adj)) .. ' ' .. str
		end

		return ICON_BULLET .. str
	end
)

local NL = '[NEWLINE]'

--- @param tech table
--- @return string
local function BuildTechHelpContent(tech)
	local type = tech.Type
	local text = StringBuilder.New()

	WriteLeadsTo(text, type)
	WriteUnits(text, type)
	WriteWonders(text, type)
	WriteBuildings(text, type)
	WriteProjects(text, type)
	WriteProcesses(text, type)

	if tech.CorporationsEnabled then
		WriteCorporations(text, type)
	end

	WriteResources(text, type)
	WriteBuilds(text, type)
	WriteAbilities(text, type, tech)
	WriteSpecialistYieldChanges(text, type)
	WriteYieldChanges(text, type)

	local str = text:Concat(NL)
	text:Clear()

	return str
end

local GetTechById = Memoize1(function(techId)
	return GameInfo.Technologies[techId]
end)

local GetTechNameById = Memoize1(function(techId)
	return civ_locale_toupper(L(GetTechById(techId).Description))
end)

GetShortHelpTextForTech = Memoize1(function(techId)
	return BuildTechHelpContent(GetTechById(techId))
end)

local GetHelpForCompleteTech = Memoize1(function(techId)
	local help = GetShortHelpTextForTech(techId)

	return GetTechNameById(techId)
			.. ' [COLOR_POSITIVE_TEXT](' .. L('TXT_KEY_RESEARCHED') .. ')[ENDCOLOR]'
			.. (help == '' and '' or (NL .. help))
end)

local function GetHelpForPendingTech(techId, cost, progress)
	local help = GetShortHelpTextForTech(techId)

	return GetTechNameById(techId)
			.. NL .. L('TXT_KEY_TECH_HELP_COST_WITH_PROGRESS', progress, cost)
			.. (help == '' and '' or (NL .. help))
end

local function GetHelpForUnstartedTech(techId, cost)
	local help = GetShortHelpTextForTech(techId)

	return GetTechNameById(techId)
			.. NL .. L('TXT_KEY_TECH_HELP_COST', cost)
			.. (help == '' and '' or (NL .. help))
end

--- @param techId TechType
--- @param player Player
--- @return number
local function GetTechProgressById(techId, player)
	local progress = 0

	progress = player:GetResearchProgressTimes100(techId) / 100

	return progress
end

--- @param techId TechType? # Technology Id
--- @param isShort boolean? # Shall help text be short (Without tech name and player info)
--- @param playerId PlayerId? # Relative playerId, if none specified then active
--- @return string
function GetHelpTextForTech(techId, isShort, playerId)
	if not techId then
		return ''
	end

	if isShort or not Game then
		return GetShortHelpTextForTech(techId)
	end

	playerId = playerId or Game.GetActivePlayer()
	local player = Players[playerId]

	if not player then
		return GetShortHelpTextForTech(techId)
	end

	local has = player:HasTech(techId)
	local cost = player:GetResearchCost(techId)
	local progress = GetTechProgressById(techId, player)

	if has then
		return GetHelpForCompleteTech(techId)
	end

	if progress == 0 then
		return GetHelpForUnstartedTech(techId, cost)
	end

	return GetHelpForPendingTech(techId, cost, progress)
end

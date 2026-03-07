-- Luacheck configuration for Community Patch DLL
-- Enhanced configuration for Civilization V modding

std = "lua51"
max_line_length = 120

ignore = {
	'.luacheckrc',
	'614', --[[ Trailing whitespace in a comment. ]]
	'631', --[[ Line is too long. ]]
	'612', --[[ Line contains trailing whitespace. ]]
	'611', --[[ Line contains only whitespace. ]]
	'621', --[[ Inconsistent indentation (SPACE followed by TAB). ]]
	'212', --[[ Unused argument (common in event callbacks) ]]
	'213', --[[ Unused loop variable ]]
	'311', --[[ Value assigned to variable is unused (common in UI code) ]]
	'314', --[[ Value of field is unused (common in data structures) ]]
	'542', --[[ Empty if branch (common in conditional compilation) ]]
	'581', --[[ Negation of a relational operator can be simplified ]]
	'211', --[[ Unused local variable (common in event handlers) ]]
	'231', --[[ Variable is set but never accessed (common in data setup) ]]
}

-- Define Civilization V standard library
stds.CIV = {
	globals = {
		-- Original globals
		'Locale',
		'include',
		'StateName',
		
		-- Core game objects
		'Game', 'Players', 'Teams', 'Map', 'UI', 'Controls', 'ContextPtr',
		'Events', 'GameInfo', 'GameInfoTypes', 'GameDefines',
		'PreGame', 'Network', 'Steam', 'Modding', 'DB',
		
		-- Common functions
		'print', 'Dprint',
		
		-- Event systems
		'GameEvents', 'LuaEvents',
		
		-- UI specific
		'ButtonPopupTypes', 'InterfaceModeTypes', 'CityUpdateTypes',
		'YieldTypes', 'CityAIFocusTypes', 'GameMessageTypes',
		'GameOptionTypes', 'KeyEvents', 'Keys', 'NotificationTypes',
		
		-- Influence and culture
		'InfluenceLevelTypes',
		
		-- Utility functions
		'HexToWorld', 'WorldToHex', 'InStrategicView',
		
		-- Common Civ5 constants
		'PlayerTypes', 'TeamTypes', 'CivilizationTypes', 'LeaderHeadTypes',
		'TechTypes', 'PolicyTypes', 'BuildingTypes', 'UnitTypes',
		'PromotionTypes', 'ImprovementTypes', 'ResourceTypes', 'FeatureTypes',
		'TerrainTypes', 'RouteTypes', 'BuildTypes', 'ProjectTypes',
		'BuildingClassTypes', 'UnitClassTypes', 'UnitCombatTypes',
		'BeliefTypes', 'ReligionTypes', 'EraTypes', 'HandicapTypes',
		'GameSpeedTypes', 'WorldSizeTypes', 'ClimateTypes', 'SeaLevelTypes',
		
		-- UI Manager and related
		'UIManager', 'LookUpControl', 'InstanceManager',
		'PopulatePullDown', 'GenerationalInstanceManager',
		
		-- Diplomacy system
		'DiploUIStateTypes', 'FromUIDiploEventTypes',
		
		-- Options and settings
		'OptionsManager', 'UserInterfaceSettings',
		
		-- Common modding globals
		'MapModData', 'g_SaveData', 'Modding', 'ContentManager',
		
		-- Frequently used UI globals
		'TooltipTypes', 'OrderTypes', 'MissionTypes', 'CommandTypes',
		'ActionSubTypes', 'ActivityTypes', 'AutomateTypes',
	}
}

std = 'min+CIV'

-- File-specific rules
files["**/UI_bc1/**"] = {
	globals = {
		"EUI", "IconHookup", "CivIconHookup", "StackInstanceManager",
		"TruncateString", "GetHelpTextForUnit", "GetHelpTextForBuilding"
	}
}

files["**/Kit/**"] = {
	globals = {"CPK"}
}

files["**/LUA/**"] = {
	globals = {
		"InfoTooltipInclude", "TradeLogic", "SupportFunctions"
	}
}

files["**/Overrides/**"] = {
	globals = {
		-- Common override patterns
		"iAgainstPlayer"
	}
}

-- Exclude very large files for initial adoption
exclude_files = {
	"**/AssignStartingPlots.lua",  -- Very large, likely third-party
	"**/Communitu_79a.lua",        -- Generated mapscript
	"**/CivilopediaScreen.lua"     -- Very large UI file
}

-- Allow some common Lua patterns
allow_defined_top = true  -- Allow defining globals at module level

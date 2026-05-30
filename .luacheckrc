-- Luacheck configuration for Community Patch DLL (Civilization V mod)
-- Globals sourced from: CvGameCoreDLL C++, Civ5XP, LuaCATS

std = "lua51+CIV"

allow_defined_top = true

ignore = {
	"611", -- Line contains only whitespace.
	"612", -- Line contains trailing whitespace.
	"614", -- Trailing whitespace in a comment.
	"621", -- Inconsistent indentation (SPACE followed by TAB).
	"631", -- Line is too long.
}

-- Engine-provided Lua environment for Civilization V
stds.CIV = {
	-- read_globals: provided by the engine, mods should not overwrite these
	read_globals = {
		-- Core game singletons (CvLuaStaticInstance / per-context)
		"Game", "Map", "Players", "Teams",
		"ContextPtr", "Controls", "Events", "LuaEvents",

		-- Engine library tables (ExposeLibraryToThread)
		"ContentManager", "DB", "GameDefines", "GameInfo",
		"Locale", "Matchmaking", "Modding", "Network",
		"OptionsManager", "Path", "Steam", "UI",

		-- Engine globals (lua_setfield LUA_GLOBALSINDEX)
		"ExposedMembers", "ExposedTypes",
		"GameCore", "GameEventFactory", "GameEvents",
		"LuaEventFactory", "PreGame",
		"UIManager", "TTManager",

		-- Engine-registered data tables (lua_setglobal)
		"Fractal", "GameInfoActions", "GameInfoTypes",

		-- DLL enum tables (CvLuaEnums::EnumStart) — all 62 tables
		"AccomplishmentTypes", "ActionSubTypes", "ActivityTypes",
		"AdvisorTypes", "AggressivePostureTypes",
		"BeliefTypes", "ButtonPopupTypes",
		"ChatTargetTypes", "CityAIFocusTypes", "CityUpdateTypes",
		"CivApproachTypes", "CivOpinionTypes", "CombatPredictionTypes",
		"CommandTypes", "ContractTypes", "ControlTypes", "CoopWarStates",
		"CorporationTypes", "DiploUIStateTypes", "DirectionTypes",
		"DisputeLevelTypes", "DomainTypes", "EndTurnBlockingTypes",
		"FaithPurchaseTypes", "FeatureTypes", "FlowDirectionTypes",
		"FogOfWarModeTypes", "FromUIDiploEventTypes",
		"GameMessageTypes", "GameOptionTypes", "GameStates",
		"GameplayGameStateTypes", "GenericWorldAnchorTypes",
		"InfluenceLevelTrend", "InfluenceLevelTypes",
		"InstantYieldType", "InterfaceDirtyBits", "InterfaceModeTypes",
		"LeaderheadAnimationTypes", "MajorCivApproachTypes",
		"MinorCivPersonalityTypes", "MinorCivQuestTypes",
		"MinorCivTraitTypes", "MissionTypes", "NotificationTypes",
		"OrderTypes", "PeaceTreatyTypes", "PlotTypes",
		"PolicyBranchTypes", "PublicOpinionTypes", "ReligionTypes",
		"ResolutionDecisionTypes", "ResourceUsageTypes",
		"RoutePlanTypes",
		"StrengthTypes", "TaskTypes", "TerraformingEventTypes",
		"TerrainTypes", "ThreatTypes", "TradeConnectionTypes",
		"TradeableItems", "YieldTypes",

		-- Binary-only enum tables (cvLuaUIEnumsLibrary)
		"ButtonStates", "ContentType", "EndGameTypes",
		"GameStateTypes", "GameTypes", "GameViewTypes",
		"InfoCornerID", "KeyEvents", "Keys",
		"Mouse", "MouseEvents", "MultiplayerLobbyMode",
		"NetErrors", "NetKicked",
		"PopupPriorities", "PopupPriority",
		"SlotClaim", "SlotStatus",
		"SystemUpdateUIType", "YieldDisplayTypes",

		-- Standalone engine functions (LuaThunk)
		"GridToWorld", "HexToWorld", "HexVertexToWorld",
		"ToGridFromHex", "ToHexFromGrid",
		"InStrategicView", "IsGameCoreBusy",
		"MouseOverStrategicViewResource",
		"ProcessStrategicViewMouseClick",
		"StrategicViewShowFeatures", "StrategicViewShowFogOfWar",
		"SetStrategicViewOverlay", "SetStrategicViewIconSetting",
		"GetStrategicViewOverlays", "GetStrategicViewIconSettings",
		"GetGameViewRenderType", "SetGameViewRenderType",
		"GetOverlayLegend", "SystemBuildYield",
		"GetVolumeKnobValue", "SetVolumeKnobValue",
		"GetVolumeKnobIDFromName",
		"ToggleStrategicView", "UnitMoving",
		"TruncateString", "LookUpControl",
		"IsNull",

		-- Engine utility globals
		"IncludeFileList", "RefreshIncludeFileList",
		"Profiler", "Automation", "FLua",
		"ContentManagerEnums",

		-- Firaxis extensions to standard libraries
		math = { fields = { "clamp" } },
		table = { fields = { "fill", "count" } },

		-- Vanilla game Lua utility classes/functions (from base game includes)
		"FractalWorld", "MultilayeredFractal", "TerrainGenerator",
		"GenerationalInstanceManager",
		"GetShuffledCopyOfTable",
		"ApplyGenericEntrySettings",
		"PrintContentsOfTable",
		"GameplayUtilities",

		-- FLuaVector (include("FLuaVector")) — constructors + vector math
		"Vector2", "Vector3", "Vector4", "Color",
		"VecAdd", "VecSub", "VecSubtract",
	},

	-- globals: things mod code legitimately reads AND writes
	globals = {
		"include",        -- engine include function
		"InstanceManager", -- LuaCATS InstanceManager class
		"MapModData",     -- cross-context shared data table
		"StateName",      -- context state tracking
		"R",              -- localization shortcut table
		"LuaTypes",       -- type registry
	},
}

---------------------------------------------------------------------------
-- Per-path overrides
---------------------------------------------------------------------------

-- LuaCATS type-definition stubs are not runnable code
files["LuaCATS/**"] = { exclude_files = { "**" } }

-- Map scripts use many vanilla globals from base-game includes
files["**/Mapscripts/**"] = {
	globals = {
		"MG",
		"GetNumNaturalWondersToPlace",
		"AdjacentToSaltWater",
		"GetMatchingPlots",
		"ObtainLandmassBoundaries",
		"PlaceBonusResources", "PlaceFish", "PlacePossibleFish",
		"PlaceResourcesAndCityStates", "PlaceStrategicAndBonusResources",
		"MeasureStartPlacementFertilityOfPlot",
		"GenerateNextToCoastalLandDataTables",
		"GetNumStartRegionAvoidForCiv", "GetNumStartRegionPriorityForCiv",
		"GetStartRegionAvoidListForCiv_GetIDs",
		"GetStartRegionPriorityListForCiv_GetIDs",
		"CivNeedsCoastalStart", "CivNeedsPlaceFirstCoastalStart",
		"CivNeedsRiverStart",
		"CreateAxisChainWithDots", "CreateAxisChainWithDoubleDots",
		"CreateAxisChainWithShiftedDots", "CreateSingleAxisIslandChain",
		"CreateLineSegment",
		"BuffIslands", "ApplyHexAdjustment", "ShiftMultiplier",
		"PolarToCartesian", "VecSub",
		"NO_RESOURCE",
	},
}

-- UI code from base game (UI_bc1) uses vanilla UI utility includes
files["UI_bc1/**"] = {
	globals = {
		"SwapWorkLineInstance",
		"RegisterCollapseBehavior", "RegisterTabBehavior",
		"SizeParentToChildContent",
		"PopulateAndAdd", "AddToPullDown", "PopulatePullDown",
		"PopulateButton",
		"GetBonusTips", "AddArticle",
		"IdentifyTableIndex",
		"GetPlayerAndTeamInfo",
		"TestMembership", "TestEspionageStarted",
		"IsUniqueColor",
		"GetCivName",
		"SetCurrentSelection",
		"GetHighlightTexture",
		"CityAvailableForRelocate",
		"CityScreenClosed",
		"AddRequiredBuildingButton",
	},
}

-- EUI compatibility files
files["**/VP - EUI Compatibility Files/**"] = {
	globals = {
		"CPK",
		"SwapWorkLineInstance",
		"RegisterCollapseBehavior",
		"SizeParentToChildContent",
		"IdentifyTableIndex",
		"GetBonusTips", "AddArticle",
		"GetPlayerAndTeamInfo",
		"TestMembership",
	},
}

-- Kit modules define the CPK global
files["**/Kit/**"] = {
	globals = { "CPK" },
}

-- Override Lua files that extend vanilla includes
files["**/Overrides/**"] = {
	globals = {
		"SwapWorkLineInstance",
		"RegisterCollapseBehavior",
		"SizeParentToChildContent",
		"PopulateAndAdd", "PopulatePullDown",
		"GetBonusTips", "AddArticle",
		"IdentifyTableIndex",
		"GetPlayerAndTeamInfo",
		"TestMembership",
	},
}

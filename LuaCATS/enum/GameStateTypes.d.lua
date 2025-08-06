--- @meta

--- Unlike most other enums, this one doesn't come from the game core DLL<br>
--- We'll have to trust modiki on the values of these<br>
--- Only CIV5_GS_MAIN_MENU and CIV5_GS_MAINGAMEVIEW are used
--- @enum GameStateType
GameStateTypes = {
	CIV5_GS_UNKNOWN = -1,
	CIV5_GS_EXIT = 0,
	CIV5_GS_OPENING_MOVIE = 1,
	CIV5_GS_LOAD_GAME = 2,
	CIV5_GS_MAIN_MENU = 3,
	CIV5_GS_MULTIPLAYER_MENU = 4,
	CIV5_GS_LANLOBBY = 5,
	CIV5_GS_INETLOBBY = 6,
	CIV5_GS_STAGING = 7,
	CIV5_GS_SCENARIOSETUP = 8,
	CIV5_GS_MAINGAMEVIEW = 9,
	CIV5_GS_UI_VIEWER = 10,
	CIV5_GS_LEADER_HEAD_BENCHMARK = 11,
	CIV5_GS_LEADER_HEAD_VIEW = 12,
	CIV5_GS_NEXUS = 13,
	CIV5_GS_WORLD_BUILDER_LOAD = 14,
	CIV5_GS_CREDITS = 15,
}

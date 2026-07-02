--- @meta

--- Unlike the other database types, FeatureTypes is a closed enum defined in the DLL and exposed to Lua.<br>
--- As such, adding/removing features in the database requires also modifying the DLL if you want them to stay synced.<br>
--- There isn't really much point using these though, as `GameInfoTypes` does just the same and is auto-populated.<br>
--- The only needed value is `NO_FEATURE`.
--- @type table<string, FeatureType>
FeatureTypes = {
	NO_FEATURE = -1,
	FEATURE_ICE = 0,
	FEATURE_JUNGLE = 1,
	FEATURE_MARSH = 2,
	FEATURE_OASIS = 3,
	FEATURE_FLOOD_PLAINS = 4,
	FEATURE_FOREST = 5,
	FEATURE_FALLOUT = 6,
	FEATURE_CRATER = 7,
	FEATURE_FUJI = 8,
	FEATURE_MESA = 9,
	FEATURE_REEF = 10,
	FEATURE_VOLCANO = 11,
	FEATURE_GIBRALTAR = 12,
	FEATURE_GEYSER = 13,
	FEATURE_FOUNTAIN_YOUTH = 14,
	FEATURE_POTOSI = 15,
	FEATURE_EL_DORADO = 16,
	FEATURE_ATOLL = 17,
	FEATURE_SRI_PADA = 18,
	FEATURE_MT_SINAI = 19,
	FEATURE_MT_KAILASH = 20,
	FEATURE_ULURU = 21,
	FEATURE_LAKE_VICTORIA = 22,
	FEATURE_KILIMANJARO = 23,
	FEATURE_SOLOMONS_MINES = 24,
	NUM_FEATURE_TYPES = 25,
}

--- @meta

--- Unlike the other database types, PlotTypes is a closed enum defined in the DLL and exposed to Lua.<br>
--- As such, adding/removing features in the database requires also modifying the DLL if you want them to stay synced.<br>
--- There isn't really much point using these though, as `GameInfoTypes` does just the same and is auto-populated.<br>
--- The only needed value is `NO_PLOT`.
--- @type table<string, PlotType>
PlotTypes = {
	NO_PLOT = -1,
	PLOT_MOUNTAIN = 0,
	PLOT_HILLS = 1,
	PLOT_LAND = 2,
	PLOT_OCEAN = 3,
	NUM_PLOT_TYPES = 4,
}

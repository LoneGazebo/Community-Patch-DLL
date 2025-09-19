--- @meta

--- Unlike most other enums, this one doesn't come from the game core DLL<br>
--- This is used in [Events.OpenInfoCorner](lua://Events.OpenInfoCorner) and corresponds to the panel opened<br>
--- It is possible to add more panels to the info corner through Lua
--- @enum InfoCornerId
InfoCornerID = {
	None = 0,
	Tech = 1,
	Units = 2,
	Cities = 3,
	Diplo = 4,
	Resources = 5,
	GP = 6,
}

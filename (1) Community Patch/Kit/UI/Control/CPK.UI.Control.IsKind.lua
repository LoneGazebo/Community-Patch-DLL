local lua_getmetatable = getmetatable

local Curry = CPK.FP.Curry
local AssertIsTable = CPK.Assert.IsTable
local AssertIsString = CPK.Assert.IsString

--- Checks whether a control is of a given kind or derives from it.
---
--- This walks the control's metatable hierarchy (`CTypeName` and `CBaseType`)
--- to check for inheritance, so both exact matches and base kinds are accepted.
---
--- ```lua
--- local IsKind = CPK.UI.Control.IsKind
---
--- local btn = ContextPtr:LookUpControl("/Some/Path/To/Button")
---
--- print(IsKind("ButtonControl", btn))       -- true (exact match)
--- print(IsKind("ButtonControlBase", btn))   -- true (ancestor)
--- print(IsKind("ControlBase", btn))         -- true (root ancestor)
--- print(IsKind("SliderControl", btn))       -- false
--- ```
---
--- @overload fun(kind: ControlKind, control: Control): boolean
--- @overload fun(kind: ControlKind): (fun(control: Control): boolean)
--- @nodiscard
local ControlIsKind = Curry(2, function(kind, control)
	AssertIsString(kind)
	AssertIsTable(control)

	local base = lua_getmetatable(control)
	local name = nil

	repeat
		name = base.CTypeName

		if kind == name then
			return true
		end

		base = base.CBaseType
	until not base

	return false
end)

CPK.UI.Control.IsKind = ControlIsKind

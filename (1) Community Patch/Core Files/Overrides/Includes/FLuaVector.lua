(function()
	if Vector2
			and Vector3
			and Vector4
			and Color
			and VecAdd
			and VecSub
			and VecSubtract
	then
		return
	end

	if MapModData and MapModData.FLuaVector then
		Vector2 = MapModData.FLuaVector.Vector2
		Vector3 = MapModData.FLuaVector.Vector3
		Vector4 = MapModData.FLuaVector.Vector4
		Color = MapModData.FLuaVector.Color
		VecAdd = MapModData.FLuaVector.VecAdd
		VecSub = MapModData.FLuaVector.VecSub
		VecSubtract = MapModData.FLuaVector.VecSubtract
	end

	if Vector2
			and Vector3
			and Vector4
			and Color
			and VecAdd
			and VecSub
			and VecSubtract
	then
		return
	end

	--- @class Vector2
	--- @field x number
	--- @field y number

	--- @class Vector3 : Vector2
	--- @field z number

	--- @class Vector4 : Vector3
	--- @field w number

	--- @alias Vector
	--- | Vector2
	--- | Vector3
	--- | Vector4

	--- @param x number
	--- @param y number
	--- @return Vector2
	function Vector2(x, y)
		return {
			x = x,
			y = y,
		}
	end

	--- @param x number
	--- @param y number
	--- @param z number
	--- @return Vector3
	function Vector3(x, y, z)
		return {
			x = x,
			y = y,
			z = z,
		}
	end

	--- @param x number
	--- @param y number
	--- @param z number
	--- @param w number
	--- @return Vector4
	function Vector4(x, y, z, w)
		return {
			x = x,
			y = y,
			z = z,
			w = w,
		}
	end

	--- @type fun(r: number, g: number, b: number, a: number): Vector4
	Color = Vector4

	--- @param va Vector
	--- @param vb Vector
	--- @return Vector
	--- @overload fun(va2: Vector2, vb2: Vector2): Vector2
	--- @overload fun(va3: Vector3, vb3: Vector3): Vector3
	--- @overload fun(va4: Vector4, vb4: Vector4): Vector4
	function VecAdd(va, vb)
		return {
			x = va.x and vb.x and va.x + vb.x,
			y = va.y and vb.y and va.y + vb.y,
			z = va.z and vb.z and va.z + vb.z,
			w = va.w and vb.w and va.w + vb.w,
		}
	end

	--- @param va Vector
	--- @param vb Vector
	--- @return Vector
	function VecSub(va, vb)
		return {
			x = va.x and vb.x and va.x - vb.x,
			y = va.y and vb.y and va.y - vb.y,
			z = va.z and vb.z and va.z - vb.z,
			w = va.w and vb.w and va.w - vb.w,
		}
	end

	-- compatiblity
	VecSubtract = VecSub

	if MapModData then
		MapModData.FLuaVector = {
			Vector2 = Vector2,
			Vector3 = Vector3,
			Vector4 = Vector4,
			Color = Color,
			VecAdd = VecAdd,
			VecSub = VecSub,
			VecSubtract = VecSubtract,
		}
	end
end)()

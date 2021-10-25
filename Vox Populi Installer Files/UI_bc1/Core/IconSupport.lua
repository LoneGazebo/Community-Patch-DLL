if not EUI then
	include( "EUI_utilities" )
end
IconLookup = EUI.IconLookup
IconHookup = EUI.IconHookup
CivIconHookup = EUI.CivIconHookup
SimpleCivIconHookup = EUI.SimpleCivIconHookup

--FLuaVector--needed for compatibility

function Vector2( i, j ) return { x = i, y = j } end
function Vector3( i, j, k ) return { x = i, y = j, z = k } end
function Vector4( i, j, k, l ) return { x = i, y = j, z = k, w = l } end
Color = Vector4

function VecAdd( u, v ) 
	return {
		x = u.x and v.x and u.x + v.x,
		y = u.y and v.y and u.y + v.y,
		z = u.z and v.z and u.z + v.z,
		w = u.w and v.w and u.w + v.w,
	}
end

function VecSubtract( u, v ) 
	return {
		x = u.x and v.x and u.x - v.x,
		y = u.y and v.y and u.y - v.y,
		z = u.z and v.z and u.z - v.z,
		w = u.w and v.w and u.w - v.w,
	}
end

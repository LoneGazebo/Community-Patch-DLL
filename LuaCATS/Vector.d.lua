--- @meta

--- @alias Vector2 {x: number, y: number}
--- @alias Vector3 {x: number, y: number, z: number}
--- @alias Vector4 {x: number, y: number, z: number, w: number}

--- Constructor of Vector2
--- @param i number
--- @param j number
--- @return Vector2
function Vector2(i, j) end

--- Constructor of Vector3
--- @param i number
--- @param j number
--- @param k number
--- @return Vector3
function Vector3(i, j, k) end

--- Constructor of Vector4
--- @param i number
--- @param j number
--- @param k number
--- @param l number
--- @return Vector4
function Vector4(i, j, k, l) end

--- Add v2 to v1
--- @param v1 Vector2
--- @param v2 Vector2
--- @return Vector2
--- @overload fun(v1: Vector3, v2: Vector3): Vector3
--- @overload fun(v1: Vector4, v2: Vector4): Vector4
function VecAdd(v1, v2) end

--- Subtract v2 from v1
--- @param v1 Vector2
--- @param v2 Vector2
--- @return Vector2
--- @overload fun(v1: Vector3, v2: Vector3): Vector3
--- @overload fun(v1: Vector4, v2: Vector4): Vector4
function VecSubtract(v1, v2) end

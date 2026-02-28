(function()
	local IsTable = CPK.Type.IsTable

	-- Hey, that's pretty good
	if IsTable(bit) then
		CPK.Bit.Polyfill = {
			tobit = bit.tobit,
			bnot = bit.bnot,
			band = bit.band,
			bor = bit.bor,
			bxor = bit.bxor,
			lshift = bit.lshift,
			rshift = bit.rshift,
			arshift = bit.arshift,
			rol = bit.rol,
			ror = bit.ror,
		}

		return
	end

	local lua_select = select
	local lua_math_floor = math.floor

	local U32 = 2 ^ 32
	local U31 = 2 ^ 31
	local M32 = U32 - 1

	--- @type fun(x: integer): integer
	local function to_u32(x)
		return x % U32
	end

	--- @type fun(x: integer): integer
	local function to_s32(x)
		x = x % U32

		if x >= U31 then
			return x - U32
		end

		return x
	end

	-- Precomputed
	local BITR = {} -- Reversed to have ascending loop in bitop
	do
		local v = U31

		for i = 1, 32 do
			BITR[i] = v
			v = v / 2
		end
	end

	-- Precomputed
	local POW2 = {}
	do
		local v = 1
		POW2[0] = 1

		for i = 1, 32 do
			v = v * 2
			POW2[i] = v
		end
	end

	--- @param a integer
	--- @param b integer
	--- @param fn fun(aa: integer, bb: integer): boolean
	--- @return integer
	local function bitop(a, b, fn)
		local r = 0
		local ua = to_u32(a)
		local ub = to_u32(b)

		for i = 1, 32 do
			local v = BITR[i]
			local aa = (ua >= v) and 1 or 0
			local bb = (ub >= v) and 1 or 0

			if aa == 1 then ua = ua - v end
			if bb == 1 then ub = ub - v end

			if fn(aa, bb) then
				r = r + v
			end
		end

		return to_s32(r)
	end

	--- @param fn fun(a: integer, b: integer): boolean
	--- @return fun(a: integer, b: integer, ...: integer): integer
	local function Prepare(fn)
		return function(a, b, ...)
			local n = lua_select('#', ...)
			local r = bitop(a, b, fn)

			if n > 8 then
				local v = { ... }
				for i = 1, n do
					r = bitop(r, v[i], fn)
				end
			else
				for i = 1, n do
					r = bitop(r, lua_select(i, ...), fn)
				end
			end

			return r
		end
	end

	local Polyfill = {}

	Polyfill.tobit = to_s32

	local function bit_fn_xor(aa, bb) return aa ~= bb end
	local function bit_fn_and(aa, bb) return aa == 1 and bb == 1 end
	local function bit_fn_or(aa, bb) return aa == 1 or bb == 1 end

	Polyfill.band = Prepare(bit_fn_and)
	Polyfill.bxor = Prepare(bit_fn_xor)
	Polyfill.bor = Prepare(bit_fn_or)

	--- @param a integer
	--- @return integer
	function Polyfill.bnot(a)
		return to_s32(M32 - to_u32(a))
	end

	--- @param a integer
	--- @param n integer
	--- @return integer
	function Polyfill.lshift(a, n)
		return to_s32((to_u32(a) * POW2[n % 32]) % U32)
	end

	--- @param a integer
	--- @param n integer
	--- @return integer
	function Polyfill.rshift(a, n)
		return lua_math_floor(to_u32(a) / POW2[n % 32])
	end

	--- @param a integer
	--- @param n integer
	--- @return integer
	function Polyfill.arshift(a, n)
		n = n % 32
		a = to_s32(a)

		if a >= 0 then
			return lua_math_floor(a / POW2[n])
		end

		return to_s32(lua_math_floor(to_u32(a) / POW2[n]))
	end

	--- @param a integer
	--- @param n integer
	--- @return integer
	function Polyfill.rol(a, n)
		n       = n % 32
		a       = to_u32(a)
		local l = (a * POW2[n]) % U32
		local r = lua_math_floor(a / POW2[32 - n])
		return to_s32((l + r) % U32)
	end

	--- @param a integer
	--- @param n integer
	--- @return integer
	function Polyfill.ror(a, n)
		n       = n % 32
		a       = to_u32(a)
		local r = lua_math_floor(a / POW2[n])
		local l = (a * POW2[32 - n]) % U32
		return to_s32((l + r) % U32)
	end

	CPK.Bit.Polyfill = Polyfill
end)()

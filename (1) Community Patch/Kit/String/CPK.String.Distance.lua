local lua_math_min = math.min
local lua_math_huge = math.huge
local lua_string_len = string.len
local lua_string_byte = string.byte

local civ_locale_len = Locale.Length
local civ_locale_cmp = Locale.Compare
local civ_locale_sub = Locale.Substring
local civ_locale_isascii = Locale.IsASCII

local AssertIsNumber = CPK.Assert.IsNumber
local AssertIsString = CPK.Assert.IsString

local LenUtf8 = civ_locale_len
local LenAscii = lua_string_len

--- @type fun(charA: string, charB: string): boolean
local function EqlUtf8(charA, charB)
	return civ_locale_cmp(charA, charB) == 0
end

--- @type fun(str: string, i: integer): string
local function SubUtf8(str, i)
	return civ_locale_sub(str, i, 1)
end

--- @type fun(charA: integer, charB: integer): boolean
local function EqlAscii(charA, charB)
	return charA == charB
end

--- ASCII byte extraction
--- @type fun(str: string, i: integer): integer
local function SubAscii(str, i)
	return lua_string_byte(str, i)
end

--- Calculates Damerau–Levenshtein edit distance between two strings.
--- <br> Supports both ASCII and UTF-8 strings via Locale.
--- <br> Uses dynamic programming with row reuse/sliding window for efficiency.
--- <br> Use max parameter to specify fast cutoff to not calculate full distance.
--- ```lua
--- -- Example
--- local StringDistance = CPK.String.Distance
--- Distance('kitten', 'kiten') -- 1
--- Distance('piktish', 'pcitish') -- 2
--- Distance('warroir', 'warrior') -- 2
--- Distance('Źdźbło', 'Zdżblo') -- 3
--- Distance('bżęczyszczykiewić', 'brzęczyszczykiewicz') -- 4
--- ```
--- @param strA string
--- @param strB string
--- @param max? integer # Optional cutoff: if distance > max, returns max + 1
--- @return integer # Distance or max + 1
function CPK.String.Distance(strA, strB, max)
	AssertIsString(strA)
	AssertIsString(strB)

	if max == nil then
		max = lua_math_huge
	end

	AssertIsNumber(max)

	-- Fast exact match check
	if strA == strB then
		return 0
	end

	local Eql, Sub, Len

	-- Pick strategy once: ASCII uses byte ops, UTF-8 uses Locale
	if civ_locale_isascii(strA) and civ_locale_isascii(strB) then
		Eql, Sub, Len = EqlAscii, SubAscii, LenAscii
	else
		Eql, Sub, Len = EqlUtf8, SubUtf8, LenUtf8
	end

	local lenA, lenB = Len(strA), Len(strB)

	-- Ensure lenA >= lenB, (this should reduce memory usage)
	if lenA < lenB then
		strA, lenA, strB, lenB = strB, lenB, strA, lenA
	end

	-- Cutoff: lengths differ more than max, skip full calculation
	if lenA - lenB > max then
		return max + 1
	end

	-- Cutoff: if shorter string is empty, distance is length of longer
	if lenB == 0 then
		return lenA
	end

	local prevA, prevB
	local prevRow, currRow, nextRow = {}, {}, {}

	for iB = 0, lenB do
		currRow[iB] = iB
	end

	for iA = 1, lenA do
		local rmin = iA -- row minimum value
		local charA = Sub(strA, iA)

		nextRow[0] = iA

		for iB = 1, lenB do
			local charB = Sub(strB, iB)

			-- Substitution cost
			local cost = Eql(charA, charB) and 0 or 1

			-- Standard Levenshtein recurrence:
			local dist = lua_math_min(
				currRow[iB] + 1,   -- deletion
				nextRow[iB - 1] + 1, -- insertion
				currRow[iB - 1] + cost -- substitution
			)

			-- Damerau transposition check
			if iA > 1 and iB > 1 then
				if Eql(charA, prevB) and Eql(prevA, charB) then
					dist = lua_math_min(dist, (prevRow[iB - 2] or 0) + 1)
				end
			end

			nextRow[iB] = dist

			if dist < rmin then
				rmin = dist
			end

			-- Reuse already extracted char instead of another Sub call
			prevB = charB
		end

		if rmin > max then
			return max + 1
		end

		-- Reuse already extracted char instead of another Sub call
		prevA = charA

		-- Slide/Reuse window
		prevRow, currRow, nextRow = currRow, nextRow, prevRow
	end

	local final = currRow[lenB]

	if final > max then
		return max + 1
	end

	return final
end

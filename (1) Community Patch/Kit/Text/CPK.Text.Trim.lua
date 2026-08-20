local lua_string_sub = string.sub
local lua_string_byte = string.byte

local TrimDirection = CPK.Text.TrimDirection

local WHITESPACE_TOKENS = {
	'[TAB]',
	'[SPACE]',
	'[NEWLINE]',
}

local WHITESPACE_BYTES = {
	[9] = true, -- \t
	[10] = true, -- \n
	[11] = true, -- \v
	[13] = true, -- \r
	[32] = true, -- space
}

--- Checks whether one of WHITESPACE_TOKENS sits at the scan boundary and, if
--- so, returns the index just past it ('Leading') or just before it ('Trailing').
--- @param str string
--- @param idx number
--- @param dir TrimDirection
--- @return number | nil
local function SkipToken(str, idx, dir)
	for t = 1, #WHITESPACE_TOKENS do
		local tok = WHITESPACE_TOKENS[t]
		local len = #tok
		local i, j

		if dir == TrimDirection.Leading then
			i, j = idx, idx + len - 1
		else
			i, j = idx - len + 1, idx
		end

		-- Guard against `i` going non-positive: string.sub treats
		-- indices <= 0 as counting from the end of the string
		if i >= 1 and tok == lua_string_sub(str, i, j) then
			return dir == TrimDirection.Leading
					and (j + 1)
					or (i - 1)
		end
	end

	return nil
end

--- Removes real whitespace (\t \n \v \r space) and Civ5 markup whitespace
--- tokens (`[NEWLINE]` `[SPACE]` `[TAB]`) from a string.
--- Finds the trim boundary/boundaries first, then extracts the result with
--- a single `string.sub` call.
---
--- Example:
--- ```lua
--- Trim('[NEWLINE]  Hello[TAB]\n') -- 'Hello'
--- Trim('[NEWLINE]  Hello[TAB]\n', nil) -- 'Hello'
--- Trim('[NEWLINE]  Hello[TAB]\n', TrimDirection.Both) -- 'Hello'
--- Trim('[NEWLINE]  Hello[TAB]\n', TrimDirection.Leading) -- 'Hello[TAB]\n'
--- Trim('[NEWLINE]  Hello[TAB]\n', TrimDirection.Trailing) -- '[NEWLINE]  Hello'
--- ```
---
--- @param str string
--- @param dir? TrimDirection
--- @return string
local function TextTrim(str, dir)
	local len = #str
	local i = 1
	local j = len

	if dir ~= TrimDirection.Trailing then
		while i <= len do
			local byte = lua_string_byte(str, i)

			if WHITESPACE_BYTES[byte] then
				i = i + 1
			else
				local next_i = SkipToken(str, i, TrimDirection.Leading)

				if not next_i then
					break
				end

				i = next_i
			end
		end
	end

	if dir ~= TrimDirection.Leading then
		while j >= i do
			local byte = lua_string_byte(str, j)

			if WHITESPACE_BYTES[byte] then
				j = j - 1
			else
				local next_j = SkipToken(str, j, TrimDirection.Trailing)

				if not next_j then
					break
				end

				j = next_j
			end
		end
	end

	if i > j then
		return ''
	end

	return lua_string_sub(str, i, j)
end

CPK.Text.Trim = TextTrim

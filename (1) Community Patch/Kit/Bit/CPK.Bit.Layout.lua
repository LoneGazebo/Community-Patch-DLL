local lua_next = next
local lua_table_sort = table.sort

local AssertError = CPK.Assert.Error
local AssertIsTable = CPK.Assert.IsTable
local AssertIsString = CPK.Assert.IsString
local AssertIsInteger = CPK.Assert.IsInteger

local BitLayout = {}

local WORD_BITS = 32

--- Assign 32-bit word slots and bit offsets to items with `bit_len`.
---
--- Uses a best-fit decreasing bin-packing strategy.
--- Each item in `arr` is mutated in-place by assigning:
--- - `slot`    - 1-based index of the 32-bit word
--- - `bit_pos` - 0-based bit offset inside that word
---
--- Example:
--- ```lua
--- local cols = {
---   { name = 'A', bit_len = 10 },
---   { name = 'B', bit_len = 6 },
---   { name = 'C', bit_len = 16 },
---   { name = 'D', bit_len = 8 },
---   { name = 'E', bit_len = 4 },
--- }
---
--- local slots = CPK.Bit.Layout.Assign32(cols)
---
--- -- Possible result (input order is mutated due to sorting):
--- -- slot 1: C(16 bits at 0), A(10 bits at 16), B(6 bits at 26)
--- -- slot 2: D(8 bits at 0), E(4 bits at 8)
---
--- -- cols now contains:
--- -- {
--- --   { name='C', bit_len=16, slot=1, bit_pos=0  },
--- --   { name='A', bit_len=10, slot=1, bit_pos=16 },
--- --   { name='B', bit_len=6,  slot=1, bit_pos=26 },
--- --   { name='D', bit_len=8,  slot=2, bit_pos=0  },
--- --   { name='E', bit_len=4,  slot=2, bit_pos=8  },
--- --
--- -- }
--- ```
--- @param arr { name: string, bit_len: integer, [any]: any }[]
--- @return integer # Number of allocated 32-bit slots
--- @return { slot: integer, bit_pos: integer, name: string, bit_len: integer, [any]: any }[] # The same array reference, mutated in-place
function BitLayout.Assign32(arr)
	AssertIsTable(arr)

	if #arr == 0 then
		return 0, arr
	end

	for i = 1, #arr do
		local itm = arr[i]

		AssertIsTable(itm)
		AssertIsString(itm.name)
		AssertIsInteger(itm.bit_len)

		if itm.bit_len < 1 or itm.bit_len > 32 then
			AssertError(itm.bit_len, '>=1 and <=32', 'Invalid ' .. itm.name .. ' at position ' .. i)
		end
	end

	lua_table_sort(arr, function(a, b)
		return a.bit_len > b.bit_len
	end)

	local bins = {}
	local slot = 0

	for i, itm in lua_next, arr do
		local best, best_left

		for _, bin in lua_next, bins do
			local free = 32 - bin.used

			if itm.bit_len <= free then
				if free == itm.bit_len then
					best = bin
					break
				end

				if not best or free < best_left then
					best = bin
					best_left = free
				end
			end
		end

		if not best then
			slot = slot + 1
			best = { slot = slot, used = 0 }
			bins[#bins + 1] = best
		end

		itm.slot = best.slot
		itm.bit_pos = best.used
		best.used = best.used + itm.bit_len

		if best.used > WORD_BITS then
			AssertError(
				best.used,
				'<=32',
				'Word overflow after assigning ' .. itm.name .. ' at position ' .. i
			)
		end
	end

	return slot, arr
end

CPK.Bit.Layout = BitLayout

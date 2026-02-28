local function toFixed(n, digits)
	return string.format("%." .. digits .. "f", n)
end

local function gc()
	collectgarbage('collect')
	collectgarbage('collect')
	collectgarbage('collect')
end

--- Measures Lua heap usage (in MB) of Gi after loading given tables
--- @param tables string[] # List of GameInfo table names
--- @return number # Memory usage in megabytes
local function MeasureGiMemoryMB(tables)
	local Gi = CPK.DB.Gi

	-- Step 1: clean baseline
	gc()

	-- Step 2: force Gi initialization
	for i = 1, #tables do
		local name = tables[i]
		-- Access triggers creation + caching
		local _ = Gi[name]
	end

	-- Clear holder
	for i = 1, #tables do
		local name = tables[i]
		-- Access triggers creation + caching
		Gi[name] = nil
	end

	gc()

	local before_kb = collectgarbage('count')

	-- Recreate holder
	for i = 1, #tables do
		local name = tables[i]
		-- Access triggers creation + caching
		local _ = Gi[name]

		if Gi == GameInfo then
			for r in Gi[name]() do end
		end
	end

	-- Step 3: stabilize heap
	gc()

	local after_kb = collectgarbage('count')

	-- Step 4: delta in MB
	return (after_kb - before_kb) / 1024
end

--- Benchmarks a function with timing + heap statistics
--- @param fn function
--- @param iterations? integer
--- @return table
local function Benchmark(fn, iterations)
	iterations = iterations or 5000

	-- stabilize environment
	gc()

	-- retained memory baseline
	local mem_before_kb = collectgarbage("count")

	-- allocation baseline
	local alloc_before_kb = collectgarbage("count")

	local t0 = os.clock()

	for i = 1, iterations do
		fn()
	end

	local t1 = os.clock()

	-- allocation snapshot BEFORE gc
	local alloc_after_kb = collectgarbage("count")

	-- retained memory AFTER gc
	gc()
	local mem_after_kb = collectgarbage("count")

	local dt = t1 - t0

	local retained_kb = mem_after_kb - mem_before_kb
	local allocated_kb = alloc_after_kb - alloc_before_kb

	return {
		iterations = iterations,

		-- timing
		time_sec = dt,
		time_ms = dt * 1000,
		time_per_call_us = (dt / iterations) * 1e6,
		time_per_call_ms = (dt / iterations) * 1000,

		-- retained memory
		mem_retained_kb = retained_kb,
		mem_retained_mb = retained_kb / 1024,
		mem_retained_per_call_bytes = (retained_kb * 1024) / iterations,

		-- allocated memory (approx)
		mem_allocated_kb = allocated_kb,
		mem_allocated_mb = allocated_kb / 1024,
		mem_allocated_per_call_bytes = (allocated_kb * 1024) / iterations,
	}
end

--- Runs benchmark and prints formatted statistics
--- @param fn function
--- @param title string
--- @param iterations integer?
local function PrintBenchmark(fn, title, iterations)
	local r = Benchmark(fn, iterations)

	print("------------------------------------------------------------")
	print("Benchmark:", title)
	print("Iterations:", r.iterations)
	print("")

	print("Time:")
	print("  total:           ", toFixed(r.time_ms, 4), "ms")
	print("  per call:        ", toFixed(r.time_per_call_us, 4), "us")
	print("")

	print("Memory (retained after GC):")
	print("  total:           ", toFixed(r.mem_retained_mb, 4), "MB")
	print("  per call:        ", toFixed(r.mem_retained_per_call_bytes, 4), "bytes")
	print("")

	print("Memory (allocated, approx):")
	print("  total:           ", toFixed(r.mem_allocated_mb, 4), "MB")
	print("  per call:        ", toFixed(r.mem_allocated_per_call_bytes, 4), "bytes")

	print("------------------------------------------------------------")

	gc()
end

CPK.DB.GiTest = function()
	local Gi = CPK.DB.Gi
	local tbl_names = {}

	local sql = [[
		SELECT name FROM sqlite_master
		WHERE type = 'table' AND name NOT LIKE 'sqlite%'
	]]

	for row in DB.Query(sql) do
		tbl_names[#tbl_names + 1] = row.name
	end

	print(#tbl_names .. ' Gi tables used mb .. ', MeasureGiMemoryMB(tbl_names))

	for _, name in next, tbl_names do
		local newIter = Gi[name]()
		local oldIter = DB.CreateQuery('SELECT * FROM ' .. name)

		for i = 1, #Gi[name] do
			local new_row = newIter()
			local old_row = oldIter()

			for old_key, old_val in next, old_row do
				local new_val = new_row[old_key]

				if new_val ~= old_val then
					print(name, 'Mismatch ' .. name .. ' ' .. tostring(old_key) .. 'old_val: ', old_val, 'new_val:', new_val)
				end
			end
		end
	end

	gc()

	-- init GameInfo
	for _, name in next, tbl_names do
		local _ = GameInfo[name]()
	end

	gc()

	local GameInfo = GameInfo
	local Gi = Gi

	print('==== Simple Iterations ====')

	PrintBenchmark(function()
		for row in GameInfo.Policies() do end
	end, 'GameInfo.Policies()')

	PrintBenchmark(function()
		for row in Gi.Policies() do end
	end, 'Gi.Policies()')

	print('==== SQL Filters ====')

	PrintBenchmark(function()
		for row in GameInfo.Policies('PortraitIndex > 50') do end
	end, "GameInfo.Policies(' PortraitIndex > 50 ')")

	PrintBenchmark(function()
		for row in Gi.Policies('PortraitIndex > 50') do end
	end, "Gi.Policies(' PortraitIndex > 50 ')")

	PrintBenchmark(function()
		for row in Gi.Policies('PortraitIndex > ?', 50) do end
	end, "Gi.Policies(' PortraitIndex > ? ', 50)")

	print('==== Table Filters ====')

	PrintBenchmark(function()
		for row in GameInfo.Policies({ CultureCost = 10 }) do end
	end, "GameInfo.Policies({ CultureCost = 10 })")

	PrintBenchmark(function()
		for row in Gi.Policies({ CultureCost = 10 }) do end
	end, "Gi.Policies({ CultureCost = 10 })")

	print('==== Row access ====')

	PrintBenchmark(function()
		local p = GameInfo.Policies.POLICY_LIBERTY
		local _ = p.Description
		_ = p.ID
		_ = p.FreeBuildingOnConquest
		_ = p.GridX
		_ = p.GridY
	end, "GameInfo column access")

	PrintBenchmark(function()
		local p = Gi.Policies.POLICY_LIBERTY --[[@as GiRow]]
		local _ = p.Description
		_ = p.ID
		_ = p.FreeBuildingOnConquest
		_ = p.GridX
		_ = p.GridY
	end, "Gi column access")

	PrintBenchmark(function()
		local _, _, _, _ = Gi.Policies.POLICY_LIBERTY(
			'Description',
			nil,
			'missin',
			'GridX'
		)
	end, "Gi column existence checks")
end

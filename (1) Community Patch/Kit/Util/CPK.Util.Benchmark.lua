local lua_print = print
local lua_tostring = tostring
local lua_collectgarbage = collectgarbage

local lua_os_clock = os.clock

local lua_math_ceil = math.ceil
local lua_math_floor = math.floor

local civ_table_fill = table.fill
local lua_table_sort = table.sort
local lua_table_insert = table.insert

local lua_string_rep = string.rep

--------------------------------------------------------------------------------

local IsNumber = CPK.Type.IsNumber

local AssertError = CPK.Assert.Error
local AssertIsTable = CPK.Assert.IsTable
local AssertIsNumber = CPK.Assert.IsNumber

local TableCopy = CPK.Table.Copy

local StringPadStart = CPK.String.PadStart

local FormatByteSize = CPK.Util.Format.ByteSize
local FormatDuration = CPK.Util.Format.Duration

local StringBuilder = CPK.Util.StringBuilder

--------------------------------------------------------------------------------

local gc_start = function()
	lua_collectgarbage('restart')
end

local gc_pause = function()
	lua_collectgarbage('stop')
end

local gc_count = function()
	return 1024 * lua_collectgarbage('count')
end

local gc_run = function(n)
	n = n or 1
	for _ = 1, n do
		lua_collectgarbage('collect')
	end

	return gc_count()
end

--------------------------------------------------------------------------------

--- @param arr number[]
--- @return { p99: number, p95: number, med: number, avg: number, max: number, min: number }
local function stats(arr)
	AssertIsTable(arr)

	local len = #arr

	if len == 0 then
		AssertError(len, '>0')
	end

	if len == 1 then
		local v = arr[1]
		return {
			p99 = v,
			p95 = v,
			med = v,
			avg = v,
			max = v,
			min = v,
		}
	end

	arr = TableCopy(arr)
	lua_table_sort(arr)

	local sum = 0
	for i = 1, len do
		sum = sum + arr[i]
	end

	local min = arr[1]
	local max = arr[len]
	local avg = sum / len

	local med = nil
	if len % 2 == 1 then
		med = arr[(len + 1) / 2]
	else
		local idx = lua_math_floor((len + 1) / 2)
		med = 0.5 * (arr[idx] + arr[idx + 1])
	end

	local p95 = arr[lua_math_ceil(0.95 * len)]
	local p99 = arr[lua_math_ceil(0.99 * len)]

	return {
		p99 = p99,
		p95 = p95,
		med = med,
		avg = avg,
		max = max,
		min = min,
	}
end

--------------------------------------------------------------------------------

--- @param times integer
--- @param loops integer
--- @param fn function
--- @return { live: number, peak: number, durs: number[], mems: number[] }
local benchmark = function(times, loops, fn)
	local time_prior, time_after
	local heap_prior, heap_after
	local live_prior, live_after
	local heap_point

	local durs = civ_table_fill(0, times)
	local mems = civ_table_fill(0, times)

	gc_run()
	gc_pause()

	live_prior = gc_count()
	heap_point = live_prior

	for i = 1, times do
		time_prior, heap_prior = lua_os_clock(), gc_count()
		for _ = 1, loops do fn() end
		time_after, heap_after = lua_os_clock(), gc_count()

		if heap_after > heap_point then
			heap_point = heap_after
		end

		durs[i] = (time_after - time_prior) / loops
		mems[i] = (heap_after - heap_prior) / loops
	end

	gc_start()
	live_after = gc_run()

	return {
		live = live_after - live_prior,
		peak = heap_point - live_prior,
		durs = durs,
		mems = mems,
	}
end

--------------------------------------------------------------------------------

local NA = 'N/A'

--- @param dur number
--- @return string
local format_dur = function(dur)
	if not IsNumber(dur) then
		return NA
	end

	return FormatDuration(dur)
end

--- @param mem number
--- @return string
local format_mem = function(mem)
	if not IsNumber(mem) then
		return NA
	end

	return FormatByteSize(mem)
end

--------------------------------------------------------------------------------

--- @class BenchData
--- @field p99 number
--- @field p95 number
--- @field med number
--- @field avg number
--- @field max number
--- @field min number

--- @class BenchInfo
--- @field test_name string # Test name
--- @field case_name? string # Case name
--- @field heap_peak number # Heap peak during tests
--- @field heap_live number # Heap post GC
--- @field heap_data BenchData
--- @field time_data BenchData

--- @alias BenchTests<Case> { name: string, make: fun(case: Case): fun() }[]

--- @alias BenchCases<Case> { name: string, make: fun(): Case }[]

--- @alias BenchOpts<Case> {
---   title?: string,
---   times: integer,
---   tests: BenchTests<Case>,
---   loops?: integer,
---   cases?: BenchCases<Case>,
---   print?: boolean,
--- }

--- @generic Case
--- @param opts BenchOpts<Case>
--- @return fun(): BenchInfo[]
local function Benchmark(opts)
	AssertIsTable(opts)

	local title = opts.title or NA
	local times = opts.times
	local loops = opts.loops or 1
	local tests = opts.tests
	local cases = opts.cases
	local print = opts.print

	if cases ~= nil then
		AssertIsTable(cases)

		if #cases < 1 then
			AssertError(#cases, '>0')
		end
	end

	AssertIsTable(tests)
	AssertIsNumber(times)
	AssertIsNumber(loops)

	if #tests < 1 then
		AssertError(#tests, '>0')
	end

	return function()
		local results = {} ---@type BenchInfo[]

		local test_run = function(case_name, test_name, test_call)
			local bench_data = benchmark(times, loops, test_call)

			results[#results + 1] = {
				case_name = case_name,
				test_name = test_name,
				heap_peak = bench_data.peak,
				heap_live = bench_data.live,
				heap_data = stats(bench_data.mems),
				time_data = stats(bench_data.durs),
			}
		end

		gc_run()

		if cases then
			for i = 1, #cases do
				local case = cases[i]
				local case_name = case.name
				local case_make = case.make
				local case_data = case_make()

				for j = 1, #tests do
					local test = tests[j]
					local test_name = test.name
					local test_make = test.make
					local test_call = test_make(case_data)

					test_run(case_name, test_name, test_call)
				end
			end
		else
			for j = 1, #tests do
				local test = tests[j]
				local test_name = test.name
				local test_make = test.make
				local test_call = test_make(nil)

				test_run(nil, test_name, test_call)
			end
		end

		gc_run()

		if not print then
			return results
		end

		local TC = 'T/C '
		local HC = 'H/C '
		local P99, P95, MED, AVG, MAX, MIN = 'P99', 'P95', 'MED', 'AVG', 'MAX', 'MIN'

		local amor = '(amortized over ' .. loops .. ' calls)'
		local info = ''
				.. '\n\tTitle = ' .. title
				.. '\n\tTimes = ' .. times
				.. '\n\tLoops = ' .. loops
				.. '\n\tCalls = ' .. (times * loops)
				.. '\n'
				.. '\n\tT/C -> Time per call ' .. amor
				.. '\n\tH/C -> Live heap delta per call ' .. amor
				.. '\n\tHP  -> Heap peak (max live during benchmark)'
				.. '\n\tHR  -> Heap retained (live after forced GC)'
				.. '\n'

		local headers = {
			'Test', 'Case', 'HP', 'HR',
			TC .. P99, TC .. P95, TC .. MED, TC .. AVG, TC .. MAX, TC .. MIN,
			HC .. P99, HC .. P95, HC .. MED, HC .. AVG, HC .. MAX, HC .. MIN,
		}

		local widths = {}
		for i = 1, #headers do
			widths[i] = #headers[i]
		end

		local resrows = {}

		for i = 1, #results do
			local result = results[i]
			local resrow = {
				result.test_name,
				result.case_name or NA,
				format_mem(result.heap_peak),
				format_mem(result.heap_live),
				format_dur(result.time_data.p99),
				format_dur(result.time_data.p95),
				format_dur(result.time_data.med),
				format_dur(result.time_data.avg),
				format_dur(result.time_data.max),
				format_dur(result.time_data.min),
				format_mem(result.heap_data.p99),
				format_mem(result.heap_data.p95),
				format_mem(result.heap_data.med),
				format_mem(result.heap_data.avg),
				format_mem(result.heap_data.max),
				format_mem(result.heap_data.min),
			}

			lua_table_insert(resrows, resrow)

			for j = 1, #resrow do
				local len = #lua_tostring(resrow[j])
				local wid = widths[j]

				if len > wid then widths[j] = len end
			end
		end

		lua_table_sort(resrows, function(a, b)
			if a[2] ~= b[2] then
				return a[2] < b[2]
			end

			return a[1] < b[1]
		end)

		local psb = StringBuilder.New():Append(info)
		local sub = StringBuilder.New()

		for i = 1, #widths do
			sub:Append(lua_string_rep('-', widths[i]))
		end

		local sep = sub:Concat('-+-')
		psb:Append(sep)
		sub:Clear()

		for i = 1, #headers do
			sub:Append(StringPadStart(headers[i], widths[i]))
		end

		psb:Append(sub:Concat(' | ')):Append(sep)
		sub:Clear()

		local last_case_name = resrows[1][2]

		for i = 1, #resrows do
			local resrow = resrows[i]
			local next_case_name = resrow[2]

			for j = 1, #resrow do
				sub:Append(StringPadStart(resrow[j], widths[j]))
			end

			if last_case_name ~= next_case_name then
				psb:Append(sep)
				last_case_name = next_case_name
			end

			psb:Append(sub:Concat(' | '))
			sub:Clear()
		end

		lua_print('[Benchmark] START')
		lua_print('FireTuner may truncate long output; check Lua.log if needed!')
		lua_print(psb:Append(sep):Append(''):Concat('\n'))
		lua_print('[Benchmark] END')

		return results
	end
end

CPK.Util.Benchmark = Benchmark

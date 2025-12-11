(function()
	if CPK then
		return
	end

	local global = MapModData or _G

	if global and global.CPK then
		CPK = global.CPK
		return
	end

	local Var = {
		VERBOSE = false,
	}

	local lua_type = type
	local lua_next = next
	local lua_error = error
	local lua_tostring = tostring

	local lua_string_match = string.match
	local lua_table_insert = table.insert
	local lua_table_concat = table.concat

	local lua_getmetatable = getmetatable
	local lua_setmetatable = setmetatable

	local EmulateInclude = (function()
		local function GetAvailablePaths()
			local list = IncludeFileList --[[@as string[] | nil]]
			local refresh = RefreshIncludeFileList --[[@as function | nil]]

			-- We are still inside Civ5.
			-- Somewhere like 'Main State'
			if list then
				if refresh then
					refresh()
				end

				return list
			end

			-- We are not inside Civ5 Lua but somewhere else...
			local hasOs = os and lua_type(os.getenv) == 'function'

			if not hasOs then
				error('Error in include emulation: Cannot determine OS')
			end

			local isWin = (os.getenv('OS') or ''):lower():match('window')
			local winCmd = 'dir /s /b *.lua'
			local linuxCmd = 'find . -type f -name *.lua'

			local stream = io.popen(isWin and winCmd or linuxCmd)
			local files = {}

			if stream == nil then
				return files
			end

			for line in stream:lines() do
				lua_table_insert(files, line)
			end

			return files
		end

		--- @param filename string
		local function GetCandidatePath(filename)
			for _, path in lua_next, GetAvailablePaths(), nil do
				if string.sub(path, - #filename) == filename then
					return path
				end
			end

			return filename
		end

		--- @param filename string
		return function(filename)
			if not filename:match('%.lua$') then
				filename = filename .. '.lua'
			end

			if lua_type(dofile) ~= 'function' then
				error('Error in include emulation: dofile is not a function')
			end

			local success, result = pcall(dofile, GetCandidatePath(filename))

			if not success then
				if result:find('^cannot open') then
					return {}
				end

				error('Error in include emulation: ' .. result, 3)
			end

			return { filename }
		end
	end)()

	local civ_include = include or EmulateInclude

	-- If include is not set, then set global emulated version of include
	if not include then
		include = EmulateInclude
	end

	local function Info(message)
		if Var.VERBOSE then
			print('CPK:', message)
		end
	end

	--- Includes specified file from VFS. Ensures only 1 file is included.
	--- Note that it does not check if included file was run correctly.
	--- Only ensures that only 1 file is included.
	--- If file was found and was 1 it's included. There's no indicate that file inclusion failed due t runtime error except logs
	--- @param filename string
	--- @return nil
	local function Import(filename)
		local type = lua_type(filename)

		if type ~= 'string' then
			local message = 'Failed to include file because filename is not string but ' .. type '.'
			lua_error(message)
		end

		local included = civ_include(filename)

		if #included == 1 then
			Info('Included file "' .. filename .. '"')
			return nil
		end

		local failed = 'Failed to include file "' .. filename .. '".'

		if #included < 1 then
			local message = failed .. ' File not found.'
					.. '\n\t' .. 'Please verify if specified file is registered in VFS.'
					.. '\n\t' .. 'Please verify if specified file exists.'

			lua_error(message)
		end

		local tokens = {}

		for _, path in lua_next, included, nil do
			lua_table_insert(tokens, '"' .. path .. '"')
		end

		local message = failed
				.. ' Filename is not unique enough. Multiple candidates found: {'
				.. '\n\t' .. lua_table_concat(tokens, ',\n\t')
				.. '\n}'

		lua_error(message)
	end

	local Node = {}
	Node.__index = {}

	--- @overload fun(name?: string): table
	local Module = {}
	Module.Node = Node

	local function Error(lvl, mes)
		if lua_type(mes) == 'table' then
			mes = lua_table_concat(mes, '\n\t')
		end

		lua_error(mes, lvl)
	end
	Node.Error = Error

	local keyPattern = '^[%w-_]+$'

	--- @param key any
	--- @return boolean
	local function IsValidKey(key)
		return lua_type(key) == 'string' and lua_string_match(key, keyPattern)
	end
	Module.IsValidKey = IsValidKey

	--- @param val any
	--- @return boolean
	local function IsValidVal(val)
		return val ~= nil
	end
	Module.IsValidVal = IsValidVal

	--- @param val any
	--- @return nil
	local function GetNode(val)
		if lua_type(val) ~= 'table' then
			return nil
		end

		local node = lua_getmetatable(val)

		if node == nil then
			return nil
		end

		local meta = lua_getmetatable(node)

		if meta ~= Node then
			return nil
		end

		return node
	end
	Module.GetNode = GetNode

	--- @param val any
	--- @return boolean
	local function IsModule(val)
		return GetNode(val) ~= nil
	end
	Module.IsModule = IsModule

	--- @param val any
	--- @return string
	local function GetInfo(val)
		return '(' .. lua_type(val) .. ') ' .. lua_tostring(val)
	end
	Node.GetInfo = GetInfo

	function Node.New(name)
		local node = lua_setmetatable({}, Node)

		node.name = name
		node.parent = node
		node.children = {}

		node.__index = function(_, key)
			return node:Resolve(key)
		end

		node.__newindex = function(_, key, val)
			return node:Assign(key, val)
		end

		return node
	end

	--- @param name? string
	--- @return string
	function Node.__index:GetPath(name)
		local path = { self.name or '?' } -- '?' means that this module does not have parent yet
		local this = self

		if name ~= nil then
			lua_table_insert(path, name)
		end

		while not this:IsRoot() do
			this = this.parent
			lua_table_insert(path, 1, this.name)
		end

		return lua_table_concat(path, '.')
	end

	--- @param name? string
	--- @return string
	function Node.__index:GetFile(name)
		return self:GetPath(name) .. '.lua'
	end

	--- @return boolean
	function Node.__index:IsRoot()
		return self == self.parent
	end

	--- @param key string
	--- @param val any
	--- @return nil
	function Node.__index:Assign(key, val)
		if not Module.IsValidKey(key) then
			Error(4, {
				'Failed to assign property within "' .. self:GetPath() .. '".',
				'Key must be a string that matches "' .. keyPattern .. '".',
				'Key is ' .. GetInfo(key),
			})
		end

		local pathname = self:GetPath(key)

		if not Module.IsValidVal(val) then
			Error(4, {
				'Failed to assign property "' .. pathname .. '".',
				'Value must not be nil.',
				'Value is ' .. GetInfo(val)
			})
		end

		local child = self.children[key]

		if child ~= nil then
			Error(4, {
				'Failed to assign property "' .. pathname .. '".',
				'The specified key is already assigned, and reassignment is not allowed.',
				'The current value assigned to the specified key is ' .. GetInfo(child) .. '.',
				'The value that was attempted to be assigned is ' .. GetInfo(val) .. '.',
			})
		end

		local node = GetNode(val)

		if node ~= nil then
			if not node:IsRoot() then
				Error(4, {
					'Failed to assign property "' .. pathname .. '".',
					'Multi-ownership is prohibited.',
					'The specified module is already adopted by another module ' .. node:GetPath(),
				})
			end

			node.parent = self
			node.name = key
		end

		self.children[key] = val
		Info('Assigned ' .. lua_type(val) .. ' ' .. self:GetPath(key))
	end

	--- @param key string
	--- @return any
	function Node.__index:Resolve(key)
		if not Module.IsValidKey(key) then
			Error(4, {
				'Failed to resolve property within "' .. self:GetPath() .. '".',
				'Key must be a string that matches "' .. keyPattern .. '".',
				'Key is ' .. GetInfo(key),
			})
		end

		local child = self.children[key]

		if child ~= nil then
			return child
		end

		local pathname = self:GetPath(key)
		local filename = self:GetFile(key)

		if filename:match('^%?') then
			Error(4, {
				'Failed to resolve property "' .. pathname .. '" using file "' .. filename .. '".',
				'Path is ambiguous; no clear root module specified.',
				'Please verify that your root module is created with the correct name and filename.',
				'Please verify that your module tree is built in the correct order.'
			})
		end

		local success, result = pcall(Import, filename)

		if not success then
			Error(4, {
				'Failed to resolve property "' .. pathname .. '" using file "' .. filename .. '".',
				result,
			})
		end

		child = self.children[key]

		if child ~= nil then
			Info('Resolved ' .. lua_type(child) .. ' ' .. pathname)
			return child
		end

		Error(4, {
			'Failed to resolve property "' .. pathname .. '" using file "' .. filename .. '".',
			'The file was included, but no value was assigned under the key "' .. key .. '".',
			'Please verify that the property is being assigned within the included file.'
		})
	end

	--- Creates new module. If name is specified module is considered root
	--- @param name? string
	--- @return table
	function Module.New(name)
		local module = lua_setmetatable({}, Node.New(name))

		return module
	end

	lua_setmetatable(Module --[[@as table]], {
		__call = Module.New
	})

	local M = Module.New

	local kit = M('CPK')

	kit.Module = Module
	kit.Import = Import
	kit.Var = Var

	kit.Args = M()  -- Variadic Arguments
	kit.Assert = M() -- Assertion utilities
	kit.Cache = M() -- Caching
	kit.Misc = M()  -- Miscellaneous
	kit.String = M() -- String utilities
	kit.Table = M() -- Table utilities
	kit.Text = M()  -- Text utilities
	kit.Type = M()  -- Type utilities
	kit.DB = M()    -- Database utilities
	kit.FP = M()    -- Functional Programming
	kit.UI = M()
	kit.UI.Control = M()
	kit.UI.Control.Instance = M()
	kit.Util = M() -- General Purpose Utilities	
	kit.Bit = M() -- Bit utilities

	if global then
		global.CPK = kit
	end

	CPK = kit
end)()

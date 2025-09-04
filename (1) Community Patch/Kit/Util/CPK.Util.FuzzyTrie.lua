local lua_next = next
local lua_tostring = tostring

local lua_math_max = math.max
local lua_math_ceil = math.floor

local lua_table_sort = table.sort
local lua_table_insert = table.insert
local lua_table_remove = table.remove

local lua_setmetatable = setmetatable

local lua_coroutine_wrap = coroutine.wrap
local lua_coroutine_yield = coroutine.yield

local civ_locale_compare = Locale.Compare
local civ_locale_tolower = Locale.ToLower
local civ_locale_length = Locale.Length

local Stack = CPK.Util.Stack
local StringBuilder = CPK.Util.StringBuilder
local StringDistance = CPK.String.Distance

---- ---- ----

--- @package
--- @generic D
--- @class FuzzyTrieNode<D>
--- @field protected data nil | D
--- @field protected edges table<string, FuzzyTrieNode<D>>
local FuzzyTrieNodePrototype = {}

--- Adds a child node to this node's edges.
--- @param word string # The word representing the edge.
--- @param node FuzzyTrieNode<D> # The child node.
--- @return self
function FuzzyTrieNodePrototype:SetEdge(word, node)
	self.edges[word] = node

	return self
end

--- Gets a child node by its edge word.
--- @param word string # The word representing the edge.
--- @return nil | FuzzyTrieNode<D> # The child node or nil if not found.
function FuzzyTrieNodePrototype:GetEdge(word)
	return self.edges[word]
end

--- Gets the table of child nodes.
--- @return table<string, FuzzyTrieNode<D>> # A table of the node's children.
function FuzzyTrieNodePrototype:GetEdges()
	return self.edges
end

--- Gets the data stored at this node or nil.
--- @return D
function FuzzyTrieNodePrototype:GetData()
	return self.data
end

--- Sets the data for this node, making it terminal.
--- @param data D
--- @return self
function FuzzyTrieNodePrototype:SetData(data)
	self.data = data

	return self
end

--- Gets the unique memory address of the node object as a string.
--- @return string # The memory address (e.g., "0x1A2B3C").
function FuzzyTrieNodePrototype:GetAddr()
	return lua_tostring(self):match("table: (.*)")
end

--- Checks if this node represents the end of a complete phrase.
--- @return boolean # True if it is a terminal node, false otherwise.
function FuzzyTrieNodePrototype:Terminal()
	return self.data ~= nil
end

--- Generates a unique signature for this node based on its children.
--- <br> _This is used for DAWG compression to identify and merge duplicate subtrees._
--- @return string # A unique string representing the node's structure.
function FuzzyTrieNodePrototype:GetHash()
	local words = {}
	local edges = self.edges
	local hashb = StringBuilder.New()

	for word in lua_next, edges, nil do
		lua_table_insert(words, word)
	end

	lua_table_sort(words, function(a, b)
		return civ_locale_compare(a, b) < 0
	end)

	for _, word in lua_next, words, nil do
		hashb:Append(word .. '=' .. self:GetEdge(word):GetAddr())
	end

	hashb:Append(self:Terminal() and 'terminal=true' or 'terminal=false')

	return hashb:Concat('&')
end

--- @package
--- @class FuzzyTrieNodeMetatable
local FuzzyTrieNodeMetatable = {}

--- @package
FuzzyTrieNodeMetatable.__index = FuzzyTrieNodePrototype

function FuzzyTrieNodeMetatable.New(data)
	local this = {
		data = data,
		edges = {}
	}

	return lua_setmetatable(this, FuzzyTrieNodeMetatable)
end

local FuzzyTrieNode = FuzzyTrieNodeMetatable

---- ---- ----

--- Split and sanitize a string into a table of words.
--- @param phrase string # The phrase to tokenize.
--- @return string[] # A table of words.
local function Tokenize(phrase)
	phrase = civ_locale_tolower(
		(phrase or '')
		-- Replace special characters with spaces
		:gsub('[_.,;:(){}\"\'%[%]%-]', ' ')
		-- Trim whitespaces
		:match("^%s*(.-)%s*$")
	)

	local words = {}

	for word in phrase:gmatch('[^%s]+') do
		lua_table_insert(words, word)
	end

	return words
end

---- ---- ----

--- @generic D
--- @class FuzzyTrie<D>
--- @field protected root FuzzyTrieNode<{ term: string, data: D }>
--- @field protected dawg nil | table<string, FuzzyTrieNode<{ term: string, data: D }>>
FuzzyTriePrototype = {}


--- Initializes the Trie by inserting all data and performing DAWG compression.
--- @protected
--- @param dict table<string, D> # The data dictionary.
--- @return self
function FuzzyTriePrototype:Init(dict)
	local dawg = {}
	self.dawg = dawg

	for term, data in lua_next, dict, nil do
		self:Insert(term, data)
	end

	self.dawg = nil -- It's needed only during the insertion

	return self
end

--- Inserts a single term and its data into the Trie,
--- building the DAWG structure.
--- @protected
--- @param term string # The term to insert.
--- @param data D # The data associated with the term.
function FuzzyTriePrototype:Insert(term, data)
	local words = Tokenize(term)
	local dawg = self.dawg
	local len = #words

	if len == 0 then
		return
	end

	--[[@cast dawg table<string, FuzzyTrieNode>]]

	local function recur(idx, parent)
		local word = words[idx]
		local child = parent:GetEdge(word)

		if not child then
			child = FuzzyTrieNode.New()
			parent:SetEdge(word, child)
		end

		if idx == len then
			child:SetData({
				term = term,
				data = data
			})
		else
			recur(idx + 1, child)
		end

		local hash = child:GetHash()

		if dawg[hash] then
			parent:SetEdge(word, dawg[hash])
		else
			dawg[hash] = child
		end
	end

	recur(1, self.root)
end

--- Returns an iterator that yields all terminal nodes from a starting point.
--- @protected
--- @param node FuzzyTrieNode<{ term: string, data: D }> # The node to start traversal from.
--- @return fun(): string, D
function FuzzyTriePrototype:Traverse(node)
	local stack = Stack.New():Push(node)

	return lua_coroutine_wrap(function()
		while not stack:Empty() do
			local curr = stack:Pop() --[[@as FuzzyTrieNode<{ term: string, data: D }>]]

			if curr:Terminal() then
				local val = curr:GetData()

				lua_coroutine_yield(val.term, val.data)
			end

			local edges = curr:GetEdges()

			for _, edge in lua_next, edges do
				stack:Push(edge)
			end
		end
	end)
end

function FuzzyTriePrototype:Search(phrase)
	local words = Tokenize(phrase)
	local len = #words

	if len == 0 or (len == 1 and #words[1] <= 3) then
		return lua_coroutine_wrap(function() end)
	end

	-- TODO I stopped here

	local stack = Stack.New():Push({
		idx = 1,
		dist = 0,
		node = self.root
	})

	return lua_coroutine_wrap(function()
		while not stack:Empty() do
			local curr = stack:Pop() --[[@as { idx: integer, dist: integer, node: FuzzyTrieNode }]]
			local node = curr.node
			local dist = curr.dist
			local idx = curr.idx

			if idx > len then
				for term, data in self:Traverse(node) do
					lua_coroutine_yield(term, data, dist)
				end
			else
				local edges = node:GetEdges()
				local word = words[idx]
				local nidx = idx + 1

				if word == '*' then
					stack:Push({
						idx = nidx,
						dist = dist,
						node = node
					})

					for _, edge in lua_next, edges, nil do
						stack:Push({
							idx = idx,
							dist = dist,
							node = edge
						})
					end
				elseif word == '?' then
					for _, edge in lua_next, edges, nil do
						stack:Push({
							idx = nidx,
							dist = dist,
							node = edge
						})
					end
				else
					-- Allow a 1 mistake per 3 chars
					local tolerance = lua_math_ceil(civ_locale_length(word) / 3)

					for nword, edge in lua_next, edges, nil do
						local ndist = StringDistance(word, nword, tolerance)

						if ndist <= tolerance then
							stack:Push({
								node = edge,
								next = next,
								dist = dist + ndist
							})
						end
					end
				end
			end
		end
	end)
end

--- @class FuzzyTrieMetatable
local FuzzyTrieMetatable = {}

--- @package
FuzzyTrieMetatable.__index = FuzzyTriePrototype

--- Creates and initializes a new FuzzyTrie from a dictionary table.
--- @generic D
--- @param dict table<string, D> # A table where keys are string terms and values are the data for lookup.
--- @return FuzzyTrie<D>
function FuzzyTrieMetatable.New(dict)
	local this = {
		root = FuzzyTrieNode.New()
	}

	lua_setmetatable(this, FuzzyTrieMetatable)

	return this:Init(dict)
end

CPK.Util.FuzzyTrie = FuzzyTrieMetatable

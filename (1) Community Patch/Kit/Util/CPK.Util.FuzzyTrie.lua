local _lua_setmetatable = setmetatable

local Distance = CPK.String.Distance

local FuzzyTrie = {}

--- @class FuzzyTrie
FuzzyTrie.__index = {}

function FuzzyTrie.New()
	error('Not implemented')
end

function FuzzyTrie.__index:Search(sentence)
	error('Not implemented')
end

CPK.Util.FuzzyTrie = FuzzyTrie

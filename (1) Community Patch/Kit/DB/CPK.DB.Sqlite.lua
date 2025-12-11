--- Checks whether a SQLite column declaration has numeric affinity.
---
--- SQLite numeric affinity applies to column types whose declared type
--- begins with one of the numeric prefixes recognized by SQLite.
---
--- This matches the rules described in the SQLite documentation:
--- INTEGER, REAL, FLOAT, DOUBLE, etc.
---
--- @param decl_type string # Column type declaration (as returned by PRAGMA table_info)
--- @return boolean # true if the column has numeric affinity
local function IsNumericAffinity(decl_type)
	decl_type = decl_type:lower()

	return decl_type:match('^int')
			or decl_type:match('^real')
			or decl_type:match('^float')
			or decl_type:match('^doub')
			~= nil
end

--- Checks whether a SQLite column declaration is a proper boolean type.
---
--- IMPORTANT:
--- Only columns declared exactly as `boolean` are treated correctly
--- by the Civ5 engine. Other variants (e.g. `bool`) behave as mixed types.
---
--- The Civ5 game engine treats columns declared as `bool` as untyped,
--- allowing mixed values (strings, numbers, nil) without normalization.
---
--- Columns declared explicitly as `boolean` are handled correctly by the
--- engine and are consistently serialized and deserialized like:
--- - `1`    -> `true`
--- - `0`    -> `false`
--- - `NULL` -> `false`
---
--- For this reason, only the `boolean` prefix is considered valid and `bool` not.
---
--- @param decl_type string # Column type declaration
--- @return boolean # true if the column is declared as boolean
local function IsBooleanAffinity(decl_type)
	return decl_type:lower() == 'boolean'
end

--- Checks whether a SQLite column declaration has string (TEXT) affinity.
---
--- SQLite string affinity applies to column types whose declared type
--- contains one of the text-related keywords recognized by SQLite:
--- TEXT, CHAR, CLOB.
---
--- @param decl_type string # Column type declaration (as returned by PRAGMA table_info)
--- @return boolean # true if the column has string affinity
local function IsStringAffinity(decl_type)
	decl_type = decl_type:lower()

	return decl_type:match('^text')
			or decl_type:match('^char')
			or decl_type:match('^clob')
			or decl_type:match('^str')
			~= nil
end

--- @enum SqliteAffinity
--- Normalized column affinity categories derived from SQLite declarations
--- and Civ5 engine behavior.
---
--- Values:
--- - Unknown : Unrecognized or engine-unsafe declaration (variant behavior)
--- - Boolean : Proper boolean column (`boolean` only)
--- - Number  : Numeric affinity (INTEGER, REAL, FLOAT, DOUBLE, etc.)
--- - String  : Text affinity (TEXT, CHAR, CLOB)
local SqliteAffinity = {
	Unknown = 'unknown',
	Boolean = 'boolean',
	Number = 'number',
	String = 'string',
}

--- Normalize a SQLite column declaration into a stable logical affinity.
---
--- This function combines SQLite type affinity rules with Civ5 engine
--- behavior to produce a deterministic classification suitable for
--- schema inspection and storage decisions.
---
--- IMPORTANT:
--- - Columns declared as `bool` are intentionally classified as `unknown`
---   due to incorrect handling by the Civ5 engine.
--- - No guessing or fallback coercion is performed.
---
--- @param decl_type string # Column type declaration (as returned by PRAGMA table_info)
--- @return SqliteAffinity # Normalized affinity category
local function NormalizeAffinity(decl_type)
	if IsBooleanAffinity(decl_type) then
		return SqliteAffinity.Boolean
	end

	if IsNumericAffinity(decl_type) then
		return SqliteAffinity.Number
	end

	if IsStringAffinity(decl_type) then
		return SqliteAffinity.String
	end

	return SqliteAffinity.Unknown
end

CPK.DB.Sqlite = {
	IsBooleanAffinity = IsBooleanAffinity,
	IsNumericAffinity = IsNumericAffinity,
	IsStringAffinity = IsStringAffinity,
	NormalizeAffinity = NormalizeAffinity
}

# DB

SQLite Query Parametrization in Civilization 5

The SQLite implementation in CIV5 supports two types of query parametrization:

**Positional Parametrization**:

- Parameters are bound to placeholders (`?`) in the query based solely on their position.
- Each `?` in the SQL query corresponds to an argument in the order provided.
- Example:
  ```lua
  local sql = "SELECT ? as A, ? as B, ? as C, ? as D"
  for row in DB.Query(sql, "A", "B", "C") do
  	print(row.A, row.B, row.C, row.D) -- A B C nil
  end
  ```

**Named Parametrization**:

- Parameters are bound to named placeholders (e.g., `:a`, `:b`, `:example`)
  in the query.
- Named parameters allow reusing the same value multiple times
  in a query and improve readability.
- The order of arguments matches the order
  of unique named placeholders in the query.
- Example:
  ```lua
  local sql = "SELECT :a as A, :b as B, :a as A2, :c as C"
  for row in DB.Query(sql, "A", "B", "C") do
  		print(row.A, row.B, row.A2, row.C) -- A B A C
  end
  ```
- In this example:
  - `:a` is bound to "A" (used twice in the query).
  - `:b` is bound to "B".
  - `:c` is bound to "C".

**Usage Notes**:

- Named parametrization enhances query reusability
  and clarity by associating parameters with meaningful names.
- Ensure the number and order of arguments match the placeholders
  in the query to avoid errors.
  Especially if using `?` instead of named parameters!

# GameInfo vs DB Queries

Efficient and performant strategies for retrieving data from the Civilization V database.

## `GameInfo.*` Access (Slowest)

Accessing tables via `GameInfo.*` (e.g., `GameInfo.Buildings`, `GameInfo.Units`) is the slowest method.

- Internally performs filtering and wraps data into Lua objects.
- Not optimized for large-scale iteration or frequent lookups.
- Avoid using in performance-critical code or tight loops.

## `DB.Query(sql, ...)` Function (Medium Performance)

Using `DB.Query(sql, ...)` is faster than `GameInfo` and allows for more efficient queries.

- Enables precise data selection and joins directly in SQL.
- Reduces Lua-side filtering and processing.
- Each call parses and compiles the SQL query on the fly, which adds overhead in loops or repeated usage.

## `DB.CreateQuery(sql)` Function (Best Performance)

Use `DB.CreateQuery(sql)` to precompile SQL statements for reuse.

- Offers all the benefits of `DB.Query`.
- Avoids the repeated parsing and compiling overhead.
- Ideal for repeated queries or high-frequency operations in loops or UI refreshes.

## Summary

| Method                | Performance | Notes                                         |
| --------------------- | ----------- | --------------------------------------------- |
| `GameInfo.*`          | Slow        | Avoid in loops; acceptable for simple lookups |
| `DB.Query(sql)`       | Medium      | Good for ad-hoc or infrequent queries         |
| `DB.CreateQuery(sql)` | Fast        | Best for repeated or parameterized queries    |

**Recommendation:**  
Use `DB.CreateQuery` when performance matters, especially inside loops or UI update logic.

**Test**

```lua
local function consume(...) end

function measure(fn)
	local before = os.clock()
	for i = 1, 1000 do
		fn()
	end
	local after = os.clock()

	print('Time:', after - before)
end

sql =
[[SELECT u.Description, c.Adjective FROM Units u LEFT JOIN Civilization_UnitClassOverrides o 	ON o.UnitType = u.Type LEFT JOIN Civilizations c ON c.Type = o.CivilizationType]]

function UsingGameInfo()
	for u in GameInfo.Units() do
		local o = GameInfo.Civilization_UnitClassOverrides({ UnitType = u.Type })()
		local c

		if o then
			c = GameInfo.Civilizations({ Type = o.CivilizationType })()
		end

		consume(u.Description, c and c.Adjective or nil)
	end
end

function UsingDbQuery()
	for row in DB.Query(sql) do
		consume(row.Description, row.Adjective)
	end
end

precompiled = DB.CreateQuery(sql)

function UsingDbCreateQuery()
	for row in precompiled() do
		consume(row.Description, row.Adjective)
	end
end

measure(UsingGameInfo)
-- Time:	9.3469999999998
measure(UsingDbQuery)
-- Time:	0.7510000000002
measure(UsingDbCreateQuery)
-- Time:	0.70600000000013

```

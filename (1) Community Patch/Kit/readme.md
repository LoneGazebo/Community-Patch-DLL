# Community Patch Kit (CPK)

CPK is a kit of reusable utilities for writing Lua scripts in Civilization V.

It gives you an automatic file loader that bases on VFS
so you don’t have to `include(...)` everything by hand.

The core idea behind CPK is simple:

- Everything is lazy-loaded — nothing gets loaded until you actually access it.
- Each module/file is included only once, no matter how many times you use it.
  _(Unless you're in a state where `MapModData` is not available)_
- Accessing something won’t pull in unrelated code — only what’s needed for that specific property gets loaded.

This keeps things clean, fast, and avoids cluttering memory or execution with unused stuff.

# How to use

```lua
-- Include CPK Autoloader
include('CPK.lua')

local AsPercentage = CPK.Misc.AsPercentage
local StringBuilder = CPK.Util.StringBuilder

local text = StringBuilder.New()

text
	:Append('Research')
	:Append('is')
	:Append(AsPercentage(0.25))
	:Append('percent.')

print(text:Concat(' ')) -- Research is 25 percent.
```

## How it works

`CPK` lets you load code like this:

```lua
include('CPK.lua')

local Fn1 = CPK.Util.Fn1
local Fn2 = CPK.Util.Fn2
```

Behind the scenes:

1. Tries to import `CPK.Util.lua` from VFS
2. Tries to import `CPK.Util.Fn1.lua` from VFS
3. Tries to import `CPK.Util.Fn2.lua` from VFS
4. Makes sure each file is only included once
5. On **each** import Checks that each part of the chain
   (like `CPK.Util`, then `CPK.Util.Fn1`) actually sets corresponding
   property to something that's not nil
   If not, it raises an error.

After something's been loaded once,
accessing the same path again won’t re-include anything.
It just reuses what was already loaded.

## Notes

- Files must be in VFS and named exactly like the path (`CPK.Util.StringBuilder.lua`)
- Each file must assign something
  at its path (`CPK.Util.StringBuilder = <non-nil>`) or error is raised.
- Reassigning properties after they’re set is not allowed.
  Files are loaded only once.
- Modules are hierarchical you can think of them like nested folders or objects
- The loader handles nested modules, so feel free to go deep with paths like
  e.g. `CPK.Data.Units.Military` but note that to handle such file either
  - corresponding files should exist:
    - `CPK.Data.lua` - should `CPK.Data = CPK.Module()`
    - `CPK.Data.Units.lua` - should `CPK.Data.Units = CPK.Module()`
    - `CPK.Data.Units.Military.lua` - should `CPK.Data.Units.Military = <non-nil>`
  - or loaded file should contain all or partial definitions that allow further loading
    e.g. `CPK.Data.lua` contains following code
    ```lua
    CPK.Data = CPK.Module()
    CPK.Data.Units = CPK.Module()
    CPK.Data.Units.Military = CPK.Module()
    ```
- **A property gets auto-included only if its parent is a CPK.Module, and the property hasn’t been defined yet.**

## Debug

On `include('CPK.lua')` only defines only 2 global variables

- `CPK` - itself
- `include` - Emulated `include` function. (Only if include do not exist yet.)

```lua
include('CPK.lua')

CPK.Var.VERBOSE = true

local Fn1 = CPK.Util.Fn1 -- This would print debug information
```

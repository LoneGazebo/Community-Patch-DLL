--- @meta

--- Features that work through the game EXE (CvExe.h). They need the
--- BIN_HOOKS option and a known Steam EXE whose addresses verified at load
--- (see CustomMods.log).
---
--- CanX: whether X would work right now; silent, fine to call every turn.
--- TryX: does X, or says why not; logged.
--- Both return `ok, reason`.
--- @class Exe
Exe = {}

--- @alias ExeRefusal
--- | "bin_hooks_off"    # the BIN_HOOKS option is off
--- | "unsupported_exe"  # unknown EXE, or an address did not verify
--- | "not_network_game" # only in network multiplayer
--- | "not_host"         # only on the host

--- The running EXE.
--- @return "DX11"|"DX9"|"Tablet"|"Unknown"
function Exe.GetBuildName() end

--- Whether TryScheduleResync would work now (host of a network game).
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.CanScheduleResync() end

--- Asks the engine to resync all players at its next sync check, and
--- announces it in chat. Host only.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.TryScheduleResync() end

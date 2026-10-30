--- @meta

--- Features that work through the game EXE (CvExe.h). They need the
--- BIN_HOOKS option and a known Steam EXE whose addresses verified at load
--- (see CustomMods.log).
---
--- CanX: whether X would work right now; silent, fine to call every turn.
--- TryX: does X, or says why not; logged.
--- Both return `ok, reason`; a TryGetX returns `ok, result` when ok.
--- @class Exe
Exe = {}

--- @alias ExeRefusal
--- | "bin_hooks_off"    # the BIN_HOOKS option is off
--- | "unsupported_exe"  # unknown EXE, or an address did not verify
--- | "not_network_game" # only in network multiplayer
--- | "not_host"         # only on the host
--- | "unavailable"      # supported, but the engine object is not there now
--- | "pending"          # started; call again next frame
--- | "not_implemented"  # not done for this EXE yet

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

--- Whether TryDisableEngineYieldIconManager would work now.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.CanDisableEngineYieldIconManager() end

--- Unsubscribes the engine's C++ YieldIconManager from its events, so it
--- stops tracking the camera and emitting Events.ShowHexYield. Safe to call
--- more than once.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.TryDisableEngineYieldIconManager() end

--- Whether TryGrowUICommandStream would work now.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.CanGrowUICommandStream() end

--- Grows the engine's per-frame UI draw list, which crashes the game with
--- many unit flags on screen. Call once per frame while it returns
--- "pending".
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.TryGrowUICommandStream() end

--- Whether TryGetUICommandStreamUsage would work now.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.CanGetUICommandStreamUsage() end

--- How much of the UI draw list the last frame used. DX11 and Tablet.
--- @return false ok
--- @return ExeRefusal reason
--- @overload fun(): true, { used: number, capacity: number }
function Exe.TryGetUICommandStreamUsage() end

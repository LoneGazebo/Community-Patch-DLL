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
--- | "unavailable"      # supported, but the engine object is not there now
--- | "not_multiplayer"  # only in multiplayer (network or hot seat)
--- | "tuner_off"        # config.ini has EnableTuner = 0
--- | "already_enabled"  # already done

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

--- Whether TryEnableTunerInMultiplayer would work now. Needs
--- EnableTuner = 1 in config.ini.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.CanEnableTunerInMultiplayer() end

--- Reopens the FireTuner listener (port 4318), which the engine closes in
--- multiplayer games, and announces it in chat, again after each resync.
--- Call from UI Lua.
--- Tuner Lua runs only on this machine: changing game state from it
--- desyncs the game.
--- @return boolean ok
--- @return ExeRefusal? reason
function Exe.TryEnableTunerInMultiplayer() end

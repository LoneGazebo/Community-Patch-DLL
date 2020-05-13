# Community-Patch-DLL
This is the repository for the Civ V SDK + Vox Populi Mod

## What is Vox Populi
VP is a collection of mods for Civ V with the Brave New World Expansion.

* The Community Patch (CP) is the base mod. It contains bugfixes (also for multiplayer), performance improvements and many AI enhancements, but minimal gameplay changes. It can be used standalone and is the basis for many other mods. 
* The Community Balance Overhaul (CBO) changes and improves many aspects of the game.
* City State Diplomacy, Civ4 Diplomacy Features and More Luxuries add features to the game.
* EUI improves the user interface.

## Where can I learn more

Check out the forum at https://forums.civfanatics.com/forums/community-patch-project.497/

## How can I play this

* You can subscribe to CP on the Steam Workshop, but that version is relatively old.
* One-click installers for the complete VP pack are available on the forum.
* Do not mix both options!

## How can I build this

* Just clone the repo. Visual Studio solution files are included in the Expansion2 folder
* You need the Visual C++ 2008 SP1 compiler to actually link the resulting game core DLL
* It is possible to use a recent IDE like Visual Studio 2017 Community though, just make sure to use the correct toolset
* Always install different Visual Studio editions in chronological order, eg 2008 before 2017
* Significant portions of the mods are Lua / SQL files. Those can be modified without rebuilding the game core

## How do I debug this

* Use Visual Studio to build the DEBUG configuration of the project (as opposed to the RELEASE config)
* Place the generated dll and pdb file (from the BuildOutput folder) in the mods folder (Community Patch Core), replacing the dll there.
* Start CiV and load the mod
* In the Visual Studio debugger menu select "Attach to process" and pick Civilization5.exe
* Start the game. In case it crashes the debugger will show you where and why. 
 * It may be you only see an address, no code. In that case the crash is outside of the game core dll ...
 * However it may still be possible to learn something from the callstack!
* You can also set (conditional) breakpoints in the code to inspect the value of interesting variables.

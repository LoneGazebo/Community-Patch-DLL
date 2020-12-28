# Community-Patch-DLL

This is the repository for the Civ V SDK + Vox Populi Mod. 

## What is Vox Populi

Started in 2014, Vox Populi (formerly known as the "Community Balance Patch") is a collaborative effort to improve Civilization V's AI and gameplay. It consists of a collection of mods (see below) that are designed to work together seamlessly.

* The Community Patch (CP) is the base mod
	* Contains the gamecore DLL, which is based on C++ code linked against the official Civ V SDK
    * Contains bugfixes (also for multiplayer), performance improvements and many AI enhancements, but minimal gameplay changes
    * Can be used standalone and is the basis for many other mods
* The Community Balance Overhaul (CBO)
	* Expands and changes the core mechanics of the game, offering an entirely new Civilization V experience that feels and plays like an evolution of the series
* City State Diplomacy, Civ4 Diplomacy Features and More Luxuries
	* Additional optional features
* EUI (optional)
	* Enhanced User Interface

## Where can I learn more

Check out the forum at https://forums.civfanatics.com/forums/community-patch-project.497/

## How can I play this

* You can subscribe to CP on the Steam Workshop, but that version is relatively old.
* One-click installers for the complete VP pack are available on the forum.
* Do not mix both options!
* You need the latest version of Civilization V (1.0.3.279) with all expansions and DLC.

## How can I build this

* Just clone the repo. Visual Studio solution files are included in the Expansion2 folder
* You need the Visual C++ 2008 SP1 compiler to actually link the resulting game core DLL
* It is possible to use a recent IDE like Visual Studio 2017 Community though, just make sure to use the correct toolset
* Always install different Visual Studio editions in chronological order, eg 2008 before 2017
* Significant portions of the mods are Lua / SQL files. Those can be modified without rebuilding the game core

## How do I debug this

* Use Visual Studio to build the DEBUG configuration of the project (as opposed to the RELEASE config)
* Place the generated dll and pdb file (from the BuildOutput folder) in the mods folder (Community Patch Core), replacing the dll there.
* Start Civ V and load the mod
* In the Visual Studio debugger menu select "Attach to process" and pick Civilization5.exe
* Start the game. In case it crashes the debugger will show you where and why. 
 * It may be you only see an address, no code. In that case the crash is outside of the game core dll ...
 * However it may still be possible to learn something from the callstack!
* You can also set (conditional) breakpoints in the code to inspect the value of interesting variables.
* You do not have to play yourself - the FireTuner from the Civ V SDK has an autoplay feature, so you can lean back and watch the AI
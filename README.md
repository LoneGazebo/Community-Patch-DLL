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

* Just clone the repo. The Visual Studio solution file VoxPopuli_vs2013.sln is included in the repository folder
* Significant portions of the mods are Lua / SQL / XML files. Those can be modified without rebuilding the game core
* You need the Visual C++ 2008 SP1 compiler to actually link the resulting game core DLL
* It is possible to use a recent IDE like Visual Studio 2019 Community though, just make sure to use the correct toolset
* Always install different Visual Studio editions in chronological order, eg 2008 before 2019
* If prompted on loading the solution file whether you want to retarget projects, select "No Upgrade" for both options and continue
* If you encounter an "unexpected precompiler header error", install [this hotfix](http://thehotfixshare.net/board/index.php?autocom=downloads&showfile=11640)
* A tutorial with visual aids has been posted [here](https://forums.civfanatics.com/threads/how-to-compile-the-vox-populi-dll.665916/), courtesy of ASCII Guy

## How do I debug this

### To enable logging (for bug reports)
* Logs provide some useful information on AI decisions and other problems
* Logging can be enabled in My Games\Sid Meier's Civilization V\config.ini
* Set the following options to 1:
  * ValidateGameDatabase
  * LoggingEnabled
  * MessageLog
  * AILog
  * AIPerfLog
  * BuilderAILog
  * PlayerAndCityAILogSplit
* Make sure to save your changes! Enabling logging only needs to be done once every time the game is installed.
* The log files will now be written to My Games\Sid Meier's Civilization V\Logs
  * Place them in a .zip file and attach them to your bug report!

### To debug the GameCoreDLL
* Use Visual Studio to build the DEBUG configuration of the project (as opposed to the RELEASE config)
* Place the generated .dll and .pdb file (from the BuildOutput folder) in the mods folder (Community Patch Core), replacing the dll there.
* Start Civ V and load the mod
* Make sure Visual Studio is open with the solution file loaded
* In the Visual Studio debugger menu select "Attach to process" and pick Civilization5.exe
* Start the game. In case it crashes the debugger will show you where and why.
  * It may be you only see an address, no code. In that case the crash is outside of the game core dll ...
  * However it may still be possible to learn something from the callstack!
* You do not have to play yourself - the FireTuner from the Civ V SDK has an autoplay feature under the Game tab, so you can lean back and watch the AI
  * You must first enable the tuner in My Games\Sid Meier's Civilization V\config.ini (first option)
* You can also set (conditional) breakpoints in the code to inspect the value of interesting variables.
  * It is useful to place breakpoints where bugged functions are called, as you will see the callstack leading to that function call, and the value of the variables passed to it
* Do not make changes to the code while the mod is running, this will desync the debugger
* You may encounter benign exceptions, usually soon after beginning the debugging process
  * An example of this is an error informing you that a PDB file was not found.
  * These are nothing to worry about, you only want to catch unhandled exceptions (i.e. crashes)
  * If you encounter these exceptions, uncheck the box labelled "Break when this exception is thrown"
  * Then detach the debugger and reattach it to the process to resume debugging. You may have to do this a few times
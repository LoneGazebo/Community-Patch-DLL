# Community-Patch-DLL
This is the repository for the Vox Populi Mod plus parts of the Civ V SDK required to make it build.

## What is Vox Populi
VP is a collection of mods for Civ V with the Brave New World Expansion.

* The Community Patch (CP) is the base mod. It contains bugfixes (also for multiplayer), performance improvements and many AI enhancements, but minimal gameplay changes. It can be used standalone and is the basis for many other mods. 
* The Community Balance Overhaul (CBO) changes and improves many aspects of the game.
* City State Diplomacy, Civ4 Diplomacy Features and More Luxuries add features to the game.
* EUI improves the user interface (third party component)

## What is the status of the project

We have been in an 'almost-done' state for quite a while. It is very playable. At this point, no new features are being added to VP (but might be done as a modmod). Right now the priority is on fixing the remaining bugs, balancing and polishing.

## Where can I learn more

Check out the forum at https://forums.civfanatics.com/forums/community-patch-project.497/

## How can I play this

* You can subscribe to CP on the Steam Workshop, but that version is relatively old.
* One-click installers for the complete VP pack are available on the forum.
* Do not mix both options!

## How can I build this

* Just clone the repo. Visual Studio solution files are included
* You need the Visual C++ 2008 compiler to actually link the resulting game core DLL
* It is possible to use a recent IDE like Visual Studio 2017 Community though, just make sure to use the correct toolset
* Significant portions of the mods are Lua / SQL files. Those can be modified without rebuilding the game core

## How can I support this project

* Contributions (code, artwork, ...) are welcome
* We don't take money. Instead, we encourage people to donate to charity
* If you like VP, please go ahead and spread the word! Tell your friends, write it up on your website, etc

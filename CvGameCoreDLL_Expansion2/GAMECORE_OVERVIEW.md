# How does all of this work?

The Gamecore has multiple purposes:

1. Load configuration from the game database
2. Implement the game logic
3. Implement AI player logic
4. Provide hooks for the lua-based UI (running on a separate thread)

Below some comments about the most important pieces and how they work. The "Cv" prefix used by all classes is omitted. The code base has a long history, so unfortunately there are many inconsistencies and design oddities.

## Game

The game is processed in turns. Within each turn there are "slices" for each player. The game object is a singleton which holds references to everything else. There is a shorthand macro "GC" for looking up items in the gamecore.

The turn logic is unfortunately quite complex in order to allow for singleplayer, multiplayer, simultaneous multiplayer modes. The most important method is CvGame::CheckPlayerTurnDeactivate().

In networked games, each client holds state for the complete game. For human players, messages for actions are sent over the network which update the state. However, AI actions are generated locally on each machine!
Unfortunately this can easily lead to "out of sync" errors if the AI logic gives different results on different machines. Therefore it's important to avoid eg iterating a std::map with pointers as keys because the memory layout is different on both ends.
Another critical issue are lua hooks being called from the UI. These hooks must not change any state in the gamecore, otherwise a desync may occur.

## Map and Pathfinder

The map consists of hex plots (also called tiles). See [here](https://www.redblobgames.com/grids/hexagons/) for details on the handling of coordinates. Iterating over the neighborhood of a given plot is a very common problem solved by the iterateRingPlots() function.

The map also groups contiguous sets of land/water plots into landmasses; this name is misleading as oceans are also "landmasses"! A landmass may be made up of multiple "areas" which are connected by an isthmus or a channel.

Pathfinding is based on the well-known A* algorithm. In fact there are multiple pathfinders with different rulesets. The unit pathfinder is the most complex because the movement rules (where can a unit end the turn etc) are complex. Often it's important to know where a unit can go before generating a path to go there, this is supported by the GetReachablePlots() method.

Since pathfinding is also used for visual feedback in the UI, there are separate pathfinder instance for gamecore and UI to avoid interference and desyncs.

## Players and AI players

Player are organized in teams, but most of the time there is only a single player per team. Technologies and visibility are shared between team members but each player has separate policies and traits. 

There is an AI subsystem for each area of the game:

* Grand Strategy sets the victory conditions to pursue
* Military AI handles the selection of cities to attack or defend
* Religion AI selects beliefs and targets for missionaries
* Diplomacy AI handles the approach towards other players depending on their past observed behavior
* Deal AI determines which deals to offer to other players or to accept from other players
* Trade AI selects targets for trade units
* Espionage AI handles spies (obviously)
* Policy AI selects policies to adopt
* Tech AI selects technologies to research
* League AI handles world congress resolutions
* MinorCiv AI tracks friendship etc

AI unit movement is managed by the following subsystems:

* Homeland AI handles civilians and also combat units in peacetime
	* Automated human units are also handled by homeland AI
	* Worker logic is in BuilderTaskingAI
* Operational AI sets up armies (which can also be civilians, eg a settler with escort)
* Army AI handles multi-turn movement of units across the map
* Tactical AI handles unit combat behavior for the current turn
	* the core function is ProcessDominanceZones()
	* visible tactical targets (eg enemy units and cities) are prioritized
	* the map is broken up into "tactical dominanace zones" around each city
	* for each zone, the friendly/enemy strength is estimated
	* depending on the ratio, the AI uses different "postures" which determine how aggressively they behave
	* friendly units which can theoretically reach a target this turn are recruited for a combat simulation
	* if the simulation yields a good result, the moves are executed

Note that both homeland AI and tactical AI have a hardcoded order of moves to try and greedily assign the first possible move without checking alternatives. 

Each AI player also maintains a cache of "dangerplots", ie they track which plots can be attacked by the enemy and avoid them as much as possible.

## Difficulty and Flavors

Difficulty level impacts the decision making: Typically the AI logic will score multiple alternatives and then pick from the top N choices. 
In higher difficulties N is lower, meaning the AI players will only pick the subjectively better options; but of course the scoring is not perfect.
All difficulty settings are encapsulated in HandicapInfo.

AI players also have personality flavors which influence the decisions they make. The flavors can be temporarily modified by "strategies".
There are economic and military strategies, which are re-evaluated after a configurable amount of turns. This is supposed to create long-term consistency in the AI player behavior.

## Units

Units have extremely many attributes to track their various abilities. These abilities can be modified by applying promotions. There is also an AI subsystem to select promotions.

## Cities

Cities have citizens which work plots to generate yields. Building also generate yields, sometimes depending on the presence of specialist citizens or great works. Policies and traits set yield modifiers on top.

The governor tries to optimize the yields by assigning citizens to work plots or specialist slots. Human players can constrain the governor by "forcing" certain plots or slots to be worked.

Cities also have AI subsystems:

* Specialization AI determines where world wonders should be built
* Strategy AI decides which units or building to produce
	* the scoring of options is delegated to unit / building / project production AI

## Database

Many constants are not hardcoded but loaded from the database at startup. They can be defined and accessed with the GD_INT_* respectively MOD_OPT_* macros.

In particular the gamecore defines many "open enums" as placeholders. The actual numeric values come from the database via getInfoTypeForString().

## Lua Interface

Many C++ classes have related "lua classes" which offer a set of static methods to be called from lua on the UI thread. These are defined with the LUAAPIEXTN / Method macros.

There are also hooks in the other directions for the gamecore to call lua methods on the gamecore thread. This is done with the LuaSupport::CallHook() method or the GAMEEVENTINVOKE_HOOK() macro.

Unfortunately, lua hooks are a source of instability. If there are no active mods which rely on the hooks, they should be disabled with the CORE_DISABLE_LUA_HOOKS define.

## Performance

Most of the time is ultimately spent for pathfinding and combat simulation.

## Serialization

The gamecore uses a brittle approach to binary serialization for generating savegames. Any change to the memory layout or database layout will break the format, leading to crashes during deserialization.

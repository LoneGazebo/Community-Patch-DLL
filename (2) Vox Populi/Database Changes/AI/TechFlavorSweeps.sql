-- Notes on Format: Eras are separated by dashes, NO UNIQUES in comments since flavors are general. Please update the comments if any tech gets changed.

/*
Flavors used here, somewhat grouped:
Strategy: FLAVOR_EXPANSION, FLAVOR_WONDER, FLAVOR_CITY_DEFENSE, FLAVOR_ESPIONAGE, FLAVOR_SPACESHIP, FLAVOR_NAVAL_GROWTH, FLAVOR_DIPLOMACY
Yield: FLAVOR_GROWTH, FLAVOR_PRODUCTION, FLAVOR_SCIENCE, FLAVOR_GOLD, FLAVOR_RELIGION, FLAVOR_CULTURE,
FLAVOR_TILE_IMPROVEMENT, FLAVOR_NAVAL_TILE_IMPROVEMENT
FLAVOR_I_SEA_TRADE_ROUTE, FLAVOR_I_LAND_TRADE_ROUTE, FLAVOR_INFRASTRUCTURE,
FLAVOR_GREAT_PEOPLE, FLAVOR_HAPPINESS, FLAVOR_WATER_CONNECTION
FLAVOR_RECON, FLAVOR_NAVAL_RECON
Military: FLAVOR_MILITARY_TRAINING, FLAVOR_OFFENSE, FLAVOR_DEFENSE, FLAVOR_MOBILE, FLAVOR_RANGED, FLAVOR_AIR, FLAVOR_ANTIAIR, FLAVOR_NUKE
NavalMil: FLAVOR_AIR_CARRIER, FLAVOR_NAVAL
*/

DELETE FROM Technology_Flavors;

INSERT INTO Technology_Flavors
	(TechType, FlavorType, Flavor)
VALUES

	-- NoTech: Units: Worker, Pathfinder, Warrior, Buildings: Monument

	('TECH_AGRICULTURE', 'FLAVOR_GROWTH', 10), -- Buildings: Shrine, Reveals: Wheat, Banana, Improvement: Farm

	-----------------------------------------------------------

	('TECH_POTTERY', 'FLAVOR_GROWTH', 20), -- Units: Settler, Buildings: Granary, Reveals: Fish, Process: Farming
	('TECH_POTTERY', 'FLAVOR_EXPANSION', 25),
	('TECH_POTTERY', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 5),

	('TECH_TRAPPING', 'FLAVOR_GROWTH', 10), -- Buildings: Smokehouse, Reveals: Bison, Deer, Improvement: Camp
	('TECH_TRAPPING', 'FLAVOR_TILE_IMPROVEMENT', 15),

	('TECH_THE_WHEEL', 'FLAVOR_TILE_IMPROVEMENT', 5), -- Buildings: Council, Wonders: Stonehenge, Reveals: Stone, Improvement: Road
	('TECH_THE_WHEEL', 'FLAVOR_INFRASTRUCTURE', 10),
	('TECH_THE_WHEEL', 'FLAVOR_SCIENCE', 10),
	('TECH_THE_WHEEL', 'FLAVOR_WONDER', 10),

	('TECH_ANIMAL_HUSBANDRY', 'FLAVOR_TILE_IMPROVEMENT', 15), -- Units: Chariot, Reveals: Horse, Cattle, Sheep, Improvement: Pasture
	('TECH_ANIMAL_HUSBANDRY', 'FLAVOR_MOBILE', 5),
	('TECH_ANIMAL_HUSBANDRY', 'FLAVOR_RANGED', 5),

	('TECH_MINING', 'FLAVOR_PRODUCTION', 10), -- Buildings: Well, Wonders: Pyramids, Improvement: Mine, ChopForest
	('TECH_MINING', 'FLAVOR_TILE_IMPROVEMENT', 15),
	('TECH_MINING', 'FLAVOR_WONDER', 10),

	-----------------------------------------------------------------

	('TECH_SAILING', 'FLAVOR_NAVAL', 15), -- Fishing // Units: Galley, Workboat, Improvement: Fishing Boats, Ability: Embark
	('TECH_SAILING', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 25),
	('TECH_SAILING', 'FLAVOR_EXPANSION', 5),

	('TECH_HORSEBACK_RIDING', 'FLAVOR_GOLD', 25), -- Trade // Units: Caravan, Buildings: Market, Wonders: Petra, Ability: +1 Trade Route
	('TECH_HORSEBACK_RIDING', 'FLAVOR_WONDER', 10),
	('TECH_HORSEBACK_RIDING', 'FLAVOR_I_LAND_TRADE_ROUTE', 25),

	('TECH_CALENDAR', 'FLAVOR_TILE_IMPROVEMENT', 15), -- Units: Archer, Buildings: Herbalist, Wonders: Artemis, Improvement: Plantation, ChopJungle
	('TECH_CALENDAR', 'FLAVOR_WONDER', 10),
	('TECH_CALENDAR', 'FLAVOR_GROWTH', 5),
	('TECH_CALENDAR', 'FLAVOR_PRODUCTION', 5),
	('TECH_TRAPPING', 'FLAVOR_RANGED', 10),

	('TECH_MASONRY', 'FLAVOR_CITY_DEFENSE', 20), -- Construction // Buildings: Wall, StoneWorks, Wonders: Halicarnussus, Improvement: Quarry, ClearMarsh
	('TECH_MASONRY', 'FLAVOR_WONDER', 10),
	('TECH_MASONRY', 'FLAVOR_TILE_IMPROVEMENT', 15),

	('TECH_ARCHERY', 'FLAVOR_MILITARY_TRAINING', 15), -- Military Strategy // Units: Horseman, War Elephant, Buildings: Barracks, Process: Defense
	('TECH_ARCHERY', 'FLAVOR_OFFENSE', 15),
	('TECH_ARCHERY', 'FLAVOR_MOBILE', 10),
	('TECH_ARCHERY', 'FLAVOR_CITY_DEFENSE', 5),

	('TECH_BRONZE_WORKING', 'FLAVOR_DEFENSE', 10), -- Units: Spearman, Buildings: Forge, Wonders: Zeus, Reveals: Iron, Ability: prod from chopping
	('TECH_BRONZE_WORKING', 'FLAVOR_TILE_IMPROVEMENT', 10),
	('TECH_BRONZE_WORKING', 'FLAVOR_PRODUCTION', 10),
	('TECH_BRONZE_WORKING', 'FLAVOR_WONDER', 10),
	('TECH_BRONZE_WORKING', 'FLAVOR_OFFENSE', 5),

	---------------------------------------------------------

	('TECH_OPTICS', 'FLAVOR_NAVAL', 15), -- Sailing // Units: Scout, Cargo Ship, Liburna, Buildings: Lighthouse, Wonders: Great Lighthouse, Ability: +1 TR
	('TECH_OPTICS', 'FLAVOR_NAVAL_GROWTH', 15),
	('TECH_OPTICS', 'FLAVOR_RECON', 10),
	('TECH_OPTICS', 'FLAVOR_WONDER', 10),
	('TECH_OPTICS', 'FLAVOR_I_SEA_TRADE_ROUTE', 20),
	('TECH_OPTICS', 'FLAVOR_WATER_CONNECTION', 20),

	('TECH_WRITING', 'FLAVOR_SCIENCE', 15), -- Units: Emissary, Buildings: Library, Scrivener's Office, Wonders: Great Library, Ability: Embassies
	('TECH_WRITING', 'FLAVOR_WONDER', 10),
	('TECH_WRITING', 'FLAVOR_DIPLOMACY', 15),

	('TECH_MATHEMATICS', 'FLAVOR_RANGED', 10), -- Units: Skirmisher, Wonders: Hanging Gardens, Roman Forum
	('TECH_MATHEMATICS', 'FLAVOR_MOBILE', 10),
	('TECH_MATHEMATICS', 'FLAVOR_WONDER', 20),

	('TECH_CONSTRUCTION', 'FLAVOR_PRODUCTION', 15), -- Masonry // Units: Catapult, Buildings: WaterMill, Arena, Wonders: Terracotta, Ability: Faster on Road
	('TECH_CONSTRUCTION', 'FLAVOR_GROWTH', 5),
	('TECH_CONSTRUCTION', 'FLAVOR_CULTURE', 5),
	('TECH_CONSTRUCTION', 'FLAVOR_WONDER', 10),
	('TECH_CONSTRUCTION', 'FLAVOR_RANGED', 10),
	('TECH_CONSTRUCTION', 'FLAVOR_OFFENSE', 10),

	('TECH_IRON_WORKING', 'FLAVOR_OFFENSE', 15), -- Units: Swordsman, Buildings: HeroicEpic, Wonders: Colossus
	('TECH_IRON_WORKING', 'FLAVOR_MILITARY_TRAINING', 10),
	('TECH_IRON_WORKING', 'FLAVOR_WONDER', 10),

	-------------------------------------------------------------------

	('TECH_DRAMA', 'FLAVOR_CULTURE', 25), -- Buildings: Amphitheater, Writers' Guild, NationalMonument, Wonders: Parthenon, Process: Culture
	('TECH_DRAMA', 'FLAVOR_GREAT_PEOPLE', 10),
	('TECH_DRAMA', 'FLAVOR_WONDER', 10),

	('TECH_PHILOSOPHY', 'FLAVOR_NAVAL', 15), -- Units: Trireme, Buildings: Courthouse, Temple, SchoolofPhilosophy, Wonders: Oracle
	('TECH_PHILOSOPHY', 'FLAVOR_SCIENCE', 10),
	('TECH_PHILOSOPHY', 'FLAVOR_RELIGION', 10),
	('TECH_PHILOSOPHY', 'FLAVOR_WONDER', 10),

	('TECH_CURRENCY', 'FLAVOR_GOLD', 15), -- Units: Composite Bowman, Buildings: Caravansary, Wonders: AngkorWat, Process: Wealth, Improvement: Village, Ability: +1 TR, Yields: Merchant +1 Gold
	('TECH_CURRENCY', 'FLAVOR_TILE_IMPROVEMENT', 15),
	('TECH_CURRENCY', 'FLAVOR_WONDER', 10),
	('TECH_CURRENCY', 'FLAVOR_I_LAND_TRADE_ROUTE', 10),
	('TECH_CURRENCY', 'FLAVOR_RANGED', 10),

	('TECH_ENGINEERING', 'FLAVOR_DEFENSE', 10), -- Buildings: Aqueduct, Wonders: GreatWall, Improvement: Fort, Ability: Bridge over Rivers
	('TECH_ENGINEERING', 'FLAVOR_GROWTH', 15),
	('TECH_ENGINEERING', 'FLAVOR_INFRASTRUCTURE', 10),
	('TECH_ENGINEERING', 'FLAVOR_WONDER', 10),

	('TECH_METAL_CASTING', 'FLAVOR_PRODUCTION', 15), -- Buildings: Baths, CircusMaximus, Improvement: LumberMill, Yields: Manufactory +3 Prod
	('TECH_METAL_CASTING', 'FLAVOR_GOLD', 15),
	('TECH_METAL_CASTING', 'FLAVOR_CULTURE', 10),
	('TECH_METAL_CASTING', 'FLAVOR_TILE_IMPROVEMENT', 15),

	---------------------------------------------------------

	('TECH_EDUCATION', 'FLAVOR_SCIENCE', 25), -- Buildings: University, Chancery, Oxford, Wonders: Sankore, Process: Research, Ability: Research Agreements
	('TECH_EDUCATION', 'FLAVOR_WONDER', 5),
	('TECH_EDUCATION', 'FLAVOR_DIPLOMACY', 10),

	('TECH_THEOLOGY', 'FLAVOR_RELIGION', 15), -- Buildings: Garden, GrandTemple, Wonders: HagiaSophia, Borobodur
	('TECH_THEOLOGY', 'FLAVOR_GREAT_PEOPLE', 10),
	('TECH_THEOLOGY', 'FLAVOR_WONDER', 20),

	('TECH_CHIVALRY', 'FLAVOR_OFFENSE', 15), -- Units: Knight, Buildings: Stable, Castle, Wonders: Alhambra, Ability: DP
	('TECH_CHIVALRY', 'FLAVOR_MOBILE', 10),
	('TECH_CHIVALRY', 'FLAVOR_CITY_DEFENSE', 20),
	('TECH_CHIVALRY', 'FLAVOR_WONDER', 5),
	('TECH_CHIVALRY', 'FLAVOR_PRODUCTION', 5),

	('TECH_PHYSICS', 'FLAVOR_RANGED', 20), -- Units: Trebuchet, HeavySkirmisher, Buildings: Circus, Wonders: MachuPichu, Yields: Academy +3S
	('TECH_PHYSICS', 'FLAVOR_OFFENSE', 10),
	('TECH_PHYSICS', 'FLAVOR_MOBILE', 10),
	('TECH_PHYSICS', 'FLAVOR_WONDER', 10),
	('TECH_PHYSICS', 'FLAVOR_CULTURE', 5),

	('TECH_STEEL', 'FLAVOR_OFFENSE', 15), -- Units: Longswordsman, Pikeman, FCompany, Buildings: Armory, Yields: Mine +1 P
	('TECH_STEEL', 'FLAVOR_DEFENSE', 15),
	('TECH_STEEL', 'FLAVOR_MILITARY_TRAINING', 15),
	('TECH_STEEL', 'FLAVOR_PRODUCTION', 5),
	('TECH_STEEL', 'FLAVOR_SCIENCE', 5),

	------------------------------------------------------

	('TECH_CIVIL_SERVICE', 'FLAVOR_TILE_IMPROVEMENT', 10), -- Units: Envoy, Buildings: Workshop, Wonders: ForbiddenPalace, Ability: OpenBorders, Yields: Laborer +1G, Farm +1F, Pasture +2F 
	('TECH_CIVIL_SERVICE', 'FLAVOR_GROWTH', 10),
	('TECH_CIVIL_SERVICE', 'FLAVOR_PRODUCTION', 10),
	('TECH_CIVIL_SERVICE', 'FLAVOR_WONDER', 5),
	('TECH_CIVIL_SERVICE', 'FLAVOR_DIPLOMACY', 15),

	('TECH_GUILDS', 'FLAVOR_GOLD', 15), -- Units: Galleass, Buildings: Customs House, Artists' Guild, EastIndia, Wonders: Karlstejn, Yields: Camp & Village +1G
	('TECH_GUILDS', 'FLAVOR_CULTURE', 25),
	('TECH_GUILDS', 'FLAVOR_WONDER', 10),
	('TECH_GUILDS', 'FLAVOR_NAVAL', 15),
	('TECH_GUILDS', 'FLAVOR_TILE_IMPROVEMENT', 10),

	('TECH_COMPASS', 'FLAVOR_NAVAL', 15), -- Units: Caravel, Explorer, Buildings: Harbor, Ability: +1 TR and higher sea TR range, Yields: Boats +1F
	('TECH_COMPASS', 'FLAVOR_RECON', 10),
	('TECH_COMPASS', 'FLAVOR_NAVAL_RECON', 20),
	('TECH_COMPASS', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 10),
	('TECH_COMPASS', 'FLAVOR_NAVAL_GROWTH', 10),
	('TECH_COMPASS', 'FLAVOR_I_SEA_TRADE_ROUTE', 10),

	('TECH_MACHINERY', 'FLAVOR_RANGED', 10), -- Units: Crossbow, Buildings: Ironworks, Wonders: NotreDame, Project: PublicWorks, Improvement: FasterChop, Ability: Faster move on Road, higher land TR range, Yields: Engineer&Quaary +1P
	('TECH_MACHINERY', 'FLAVOR_I_LAND_TRADE_ROUTE', 10),
	('TECH_MACHINERY', 'FLAVOR_TILE_IMPROVEMENT', 5),
	('TECH_MACHINERY', 'FLAVOR_PRODUCTION', 10),
	('TECH_MACHINERY', 'FLAVOR_WONDER', 10),
	('TECH_MACHINERY', 'FLAVOR_HAPPINESS', 10),

	-----------------------------------------------------

	('TECH_BANKING', 'FLAVOR_GOLD', 15), -- Units: Pioneer, Buildings: Bank, Constabulary, Yields: Town +3G
	('TECH_BANKING', 'FLAVOR_SCIENCE', 5),
	('TECH_BANKING', 'FLAVOR_EXPANSION', 15),
	('TECH_BANKING', 'FLAVOR_ESPIONAGE', 10),

	('TECH_PRINTING_PRESS', 'FLAVOR_CULTURE', 5), -- Buildings: Printing Press, Wonders: GlobeTheatre, Ability: WC, Yeild: Writers +1C
	('TECH_PRINTING_PRESS', 'FLAVOR_WONDER', 10),
	('TECH_PRINTING_PRESS', 'FLAVOR_DIPLOMACY', 25),

	('TECH_ASTRONOMY', 'FLAVOR_WONDER', 10),-- Buildings: Observatory, Wonders: ChichenItza, Ability: Faster embark&cross oceans, Yields: Scientist +1S
	('TECH_ASTRONOMY', 'FLAVOR_NAVAL_RECON', 20),
	('TECH_ASTRONOMY', 'FLAVOR_SCIENCE', 5),
	('TECH_ASTRONOMY', 'FLAVOR_EXPANSION', 5),

	('TECH_GUNPOWDER', 'FLAVOR_DEFENSE', 10), -- Units: Cannon, Tercio, Wonders: Himeji, Ability: FasterRoad Yields: Camp +1G
	('TECH_GUNPOWDER', 'FLAVOR_OFFENSE', 20),
	('TECH_GUNPOWDER', 'FLAVOR_RANGED', 10),
	('TECH_GUNPOWDER', 'FLAVOR_WONDER', 10),
	('TECH_GUNPOWDER', 'FLAVOR_TILE_IMPROVEMENT', 5),

	('TECH_CHEMISTRY', 'FLAVOR_GROWTH', 15), -- Buildings: Grocer, Wonders: Pisa, Yields: Plantation +1G, Fort&Citadel +2S
	('TECH_CHEMISTRY', 'FLAVOR_WONDER', 10),
	('TECH_CHEMISTRY', 'FLAVOR_HAPPINESS', 10),
	('TECH_CHEMISTRY', 'FLAVOR_TILE_IMPROVEMENT', 10),

	--------------------------------------------------

	('TECH_ARCHITECTURE', 'FLAVOR_WONDER', 20), -- Buildings: Royal Collection, Wonders: PorcelainTower, TajMahal, Yields: Artists +1C, Town +3F
	('TECH_ARCHITECTURE', 'FLAVOR_CULTURE', 15),

	('TECH_ECONOMICS', 'FLAVOR_GOLD', 5), -- Buildings: Windmill, Wonders: Uffizi, Ability: +1 TR, Yields: Merchant&Plantation +1G
	('TECH_ECONOMICS', 'FLAVOR_GROWTH', 5),
	('TECH_ECONOMICS', 'FLAVOR_PRODUCTION', 20),
	('TECH_ECONOMICS', 'FLAVOR_WONDER', 10),
	('TECH_ECONOMICS', 'FLAVOR_TILE_IMPROVEMENT', 5),

	('TECH_ACOUSTICS', 'FLAVOR_CULTURE', 25), -- Buildings: OperaHouse, Musicians' Guild, Wonders: Sistine, SummerPalace, Yields: HolySite +4Faith
	('TECH_ACOUSTICS', 'FLAVOR_WONDER', 20),

	('TECH_NAVIGATION', 'FLAVOR_NAVAL', 30), -- Units: Frigate, Corvette, Buildings: Bastion Fort, Ability: City embark costs 1 move, Yields: Servant +1G, Boats +1F
	('TECH_NAVIGATION', 'FLAVOR_CITY_DEFENSE', 20),
	('TECH_NAVIGATION', 'FLAVOR_GOLD', 5),
	('TECH_NAVIGATION', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 5),

	('TECH_METALLURGY', 'FLAVOR_OFFENSE', 20), -- Units: Lancer, Musketman, Cuirassier, Wonders: RedFort, Yields: Lumber +1P +1G
	('TECH_METALLURGY', 'FLAVOR_MOBILE', 20),
	('TECH_METALLURGY', 'FLAVOR_RANGED', 20),
	('TECH_METALLURGY', 'FLAVOR_TILE_IMPROVEMENT', 5),
	('TECH_METALLURGY', 'FLAVOR_WONDER', 5),

	------------------------------------------------------

	('TECH_SCIENTIFIC_THEORY', 'FLAVOR_SCIENCE', 30), -- Buildings: Zoo, PublicSchool, Ability: TechTrade, Yields: Scientist +1S, Academy +3S
	('TECH_SCIENTIFIC_THEORY', 'FLAVOR_CULTURE', 10),

	('TECH_RAILROAD', 'FLAVOR_INFRASTRUCTURE', 10), -- Units: Colonist, Commando, Buildings: Hotel, CoalingStation, Wonders: Neuschwanstein, Ability: +1TR, Yields: Town +3F, Village +1C
	('TECH_RAILROAD', 'FLAVOR_MOBILE', 10),
	('TECH_RAILROAD', 'FLAVOR_PRODUCTION', 15),
	('TECH_RAILROAD', 'FLAVOR_GOLD', 10),
	('TECH_RAILROAD', 'FLAVOR_CULTURE', 15),
	('TECH_RAILROAD', 'FLAVOR_WONDER', 10),
	('TECH_RAILROAD', 'FLAVOR_EXPANSION', 15),
	('TECH_RAILROAD', 'FLAVOR_TILE_IMPROVEMENT', 5),

	('TECH_STEAM_POWER', 'FLAVOR_TILE_IMPROVEMENT', 20), -- Buildings: Seaport, Wonders: Soho Foundry, Reveals: Coal, Ability: FasterEmbarked, Yields: Mine +2P, Quarry +1P
	('TECH_STEAM_POWER', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 10),
	('TECH_STEAM_POWER', 'FLAVOR_PRODUCTION', 15),
	('TECH_STEAM_POWER', 'FLAVOR_GOLD', 5),
	('TECH_STEAM_POWER', 'FLAVOR_WONDER', 10),
	('TECH_STEAM_POWER', 'FLAVOR_MILITARY_TRAINING', 5),

	('TECH_RIFLING', 'FLAVOR_DEFENSE', 10), -- Units: Fusilier, Field Gun, ForeignLegion, Yields: Camp +1G
	('TECH_RIFLING', 'FLAVOR_OFFENSE', 20),
	('TECH_RIFLING', 'FLAVOR_RANGED', 10),
	('TECH_RIFLING', 'FLAVOR_TILE_IMPROVEMENT', 5),

	-------------------------------------------------------

	('TECH_ARCHAEOLOGY', 'FLAVOR_WONDER', 5), -- Units: Archeologist, Buildings: Museum, Wonders: Louvre, Reveals&Improve:Antiquity, Yields: HolySite +4C
	('TECH_ARCHAEOLOGY', 'FLAVOR_CULTURE', 35),
	('TECH_ARCHAEOLOGY', 'FLAVOR_TILE_IMPROVEMENT', 5),

	('TECH_FERTILIZER', 'FLAVOR_GROWTH', 5), -- Buildings: Agribusiness, Yields: Laborer +2P, Manufactory +3P, Farm +1F, Pasture +2G
	('TECH_FERTILIZER', 'FLAVOR_PRODUCTION', 5),
	('TECH_FERTILIZER', 'FLAVOR_GOLD', 10),
	('TECH_FERTILIZER', 'FLAVOR_TILE_IMPROVEMENT', 10),

	('TECH_INDUSTRIALIZATION', 'FLAVOR_PRODUCTION', 25), -- Units: Diplomat, Ironclad, Buildings: Factory, Wonders: Westminster, Reveals: Aluminum, Yields: Laborer +1G, Eng. +1P
	('TECH_INDUSTRIALIZATION', 'FLAVOR_WONDER', 5),
	('TECH_INDUSTRIALIZATION', 'FLAVOR_DIPLOMACY', 15),
	('TECH_INDUSTRIALIZATION', 'FLAVOR_NAVAL', 15),
	('TECH_INDUSTRIALIZATION', 'FLAVOR_TILE_IMPROVEMENT', 10),

	('TECH_DYNAMITE', 'FLAVOR_RANGED', 10), -- Units: GatlingGun, Cruiser, Wonders: Eiffel, Yields: Quarry +2P
	('TECH_DYNAMITE', 'FLAVOR_NAVAL', 15),
	('TECH_DYNAMITE', 'FLAVOR_TILE_IMPROVEMENT', 5),
	('TECH_DYNAMITE', 'FLAVOR_WONDER', 10),

	('TECH_MILITARY_SCIENCE', 'FLAVOR_RANGED', 10), -- Units: Cavalry, Buildings: Mil.Academy, Wonders: Brandenburg, Ability:MapTrade, Yields: Writers +1C, Fort +2 BorderGrowth, Citadel +2P
	('TECH_MILITARY_SCIENCE', 'FLAVOR_MOBILE', 10),
	('TECH_MILITARY_SCIENCE', 'FLAVOR_MILITARY_TRAINING', 15),
	('TECH_MILITARY_SCIENCE', 'FLAVOR_WONDER', 10),
	('TECH_MILITARY_SCIENCE', 'FLAVOR_CULTURE', 5),

	--------------------------------------------------------------

	('TECH_BIOLOGY', 'FLAVOR_GREAT_PEOPLE', 25), -- Buildings: Hospital, International Finance Center, Wonders: StatueofLiberty, Improvement: Faster Chop
	('TECH_BIOLOGY', 'FLAVOR_GROWTH', 15),
	('TECH_BIOLOGY', 'FLAVOR_WONDER', 5),

	('TECH_ELECTRICITY', 'FLAVOR_GOLD', 15), -- Buildings: StockExchange, Wonders: EmpireState, Ability: +1 TR
	('TECH_ELECTRICITY', 'FLAVOR_WONDER', 10),

	('TECH_CORPORATIONS', 'FLAVOR_GOLD', 30), -- Buildings: Corporations // Not changing this, it's supposed to be rushed I guess
	('TECH_CORPORATIONS', 'FLAVOR_GROWTH', 30),
	('TECH_CORPORATIONS', 'FLAVOR_PRODUCTION', 30),
	('TECH_CORPORATIONS', 'FLAVOR_SCIENCE', 30),
	('TECH_CORPORATIONS', 'FLAVOR_CULTURE', 30),

	('TECH_REPLACEABLE_PARTS', 'FLAVOR_OFFENSE', 10), -- Units: Rifleman, Buildings: WireService, Wonders: Kremlin, Yield: Servants +1S
	('TECH_REPLACEABLE_PARTS', 'FLAVOR_DEFENSE', 10),
	('TECH_REPLACEABLE_PARTS', 'FLAVOR_WONDER', 5),
	('TECH_REPLACEABLE_PARTS', 'FLAVOR_DIPLOMACY', 10),
	('TECH_REPLACEABLE_PARTS', 'FLAVOR_SCIENCE', 10),
	('TECH_REPLACEABLE_PARTS', 'FLAVOR_CULTURE', 5),

	('TECH_COMBUSTION', 'FLAVOR_OFFENSE', 20), -- Units: Landship, Buildings: Arsenal, MineField, Reveals: Oil, Improvement: OilWell, Ability: FasteronRails, ExtendedLandTR, Yield: Mine +1P, Lumber +1P 1G
	('TECH_COMBUSTION', 'FLAVOR_MOBILE', 10),
	('TECH_COMBUSTION', 'FLAVOR_TILE_IMPROVEMENT', 20),
	('TECH_COMBUSTION', 'FLAVOR_CITY_DEFENSE', 20),

	-------------------------------------------------------------

	('TECH_PLASTIC', 'FLAVOR_TILE_IMPROVEMENT', 10), -- Units: Submarine, Buildings: ResearchLab, PalaceofC&S, Wonders: CristoRedentor, Yields: Artists +1C, Plants +1G, OilWell +2P
	('TECH_PLASTIC', 'FLAVOR_WONDER', 10),
	('TECH_PLASTIC', 'FLAVOR_SCIENCE', 20),
	('TECH_PLASTIC', 'FLAVOR_CULTURE', 5),
	('TECH_PLASTIC', 'FLAVOR_NAVAL', 15),

	('TECH_RADIO', 'FLAVOR_CULTURE', 35), -- Units: Destroyer, Dreadnought, Buildings: BroadcastTower, ForeignBureau, Wonders: Broadway, Yield: Musicians +2C
	('TECH_RADIO', 'FLAVOR_WONDER', 5),
	('TECH_RADIO', 'FLAVOR_NAVAL', 30),
	('TECH_RADIO', 'FLAVOR_DIPLOMACY', 10),

	('TECH_FLIGHT', 'FLAVOR_AIR', 25), -- Units: Paratrooper, Triplane, Bomber, Wonders: Prora, Yields: HolySite +4T
	('TECH_FLIGHT', 'FLAVOR_OFFENSE', 10),
	('TECH_FLIGHT', 'FLAVOR_WONDER', 5),

	('TECH_BALLISTICS', 'FLAVOR_MOBILE', 10), -- Units: Artillery, AAGun, LightTank, MachineGun, Buildings: HallofHonor, Ability: EngineeringCorps
	('TECH_BALLISTICS', 'FLAVOR_RANGED', 30),
	('TECH_BALLISTICS', 'FLAVOR_ANTIAIR', 10),
	('TECH_BALLISTICS', 'FLAVOR_OFFENSE', 10),

	--------------------------------------------------------------

	('TECH_PENICILIN', 'FLAVOR_GROWTH', 25), -- Units: Mercs, Buildings: MedicalLab, Ability: FalloutResistance, Yields: Scientist +1S
	('TECH_PENICILIN', 'FLAVOR_SCIENCE', 5),
	('TECH_PENICILIN', 'FLAVOR_OFFENSE', 5),

	('TECH_REFRIGERATION', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 20), -- Buildings: Stadium, Improvement: Offshore, Ability: ExtendedSeaTR, Yield: Boats +2F, Camp +2G, Town +3G
	('TECH_REFRIGERATION', 'FLAVOR_TILE_IMPROVEMENT', 5),
	('TECH_REFRIGERATION', 'FLAVOR_CULTURE', 20),
	('TECH_REFRIGERATION', 'FLAVOR_DIPLOMACY', 10),

	('TECH_ATOMIC_THEORY', 'FLAVOR_NUKE', 25), -- Units: Ambassador, Reveals: Uranium, Projects: Manhattan
	('TECH_ATOMIC_THEORY', 'FLAVOR_DIPLOMACY', 15),
	('TECH_ATOMIC_THEORY', 'FLAVOR_TILE_IMPROVEMENT', 10),

	('TECH_ROCKETRY', 'FLAVOR_AIR', 20), -- Units: Rocket Missile, Buildings: Airport, Ability: NoMovementCityEmbark, Yield: Academy +3S

	('TECH_COMBINED_ARMS', 'FLAVOR_OFFENSE', 15), -- Units: Tank, Infantry, Wonders: Pentagon, Ability: FasteronRail, Yields: Manufactory +3P
	('TECH_COMBINED_ARMS', 'FLAVOR_MOBILE', 15),
	('TECH_COMBINED_ARMS', 'FLAVOR_DEFENSE', 5),
	('TECH_COMBINED_ARMS', 'FLAVOR_WONDER', 5),

	----------------------------------------------------------

	('TECH_ELECTRONICS', 'FLAVOR_HAPPINESS', 15), -- Units: Attack Submarine, Buildings: Recycling, PoliceStation, Wonders: MotherlandCalls, Yields: Well +2P, Fort +4BG, Citadel +4P
	('TECH_ELECTRONICS', 'FLAVOR_WONDER', 10),
	('TECH_ELECTRONICS', 'FLAVOR_TILE_IMPROVEMENT', 5),
	('TECH_ELECTRONICS', 'FLAVOR_ESPIONAGE', 10),
	('TECH_ELECTRONICS', 'FLAVOR_NAVAL', 15),

	('TECH_COMPUTERS', 'FLAVOR_AIR_CARRIER', 10), -- Units: SpecForces, Carrier, Buildings: NationalIntelligence, Wonders: Bletchley, Ability: +1 TR, Yield: Artists +1C
	('TECH_COMPUTERS', 'FLAVOR_NAVAL', 10),
	('TECH_COMPUTERS', 'FLAVOR_OFFENSE', 10),
	('TECH_COMPUTERS', 'FLAVOR_ESPIONAGE', 20),
	('TECH_COMPUTERS', 'FLAVOR_CULTURE', 5),
	('TECH_COMPUTERS', 'FLAVOR_WONDER', 5),

	('TECH_NUCLEAR_FISSION', 'FLAVOR_NUKE', 30), -- Units: Fleet Destroyer, Battleship, A-Bomb, Buildings: Str.Defense, Yields: Academy +3S
	('TECH_NUCLEAR_FISSION', 'FLAVOR_NAVAL', 30),
	('TECH_NUCLEAR_FISSION', 'FLAVOR_CITY_DEFENSE', 5),

	('TECH_RADAR', 'FLAVOR_AIR', 25), -- Units: HeavyBomber, Fighter, Buildings: Military Base
	('TECH_RADAR', 'FLAVOR_OFFENSE', 10),
	('TECH_RADAR', 'FLAVOR_CULTURE', 10),
	('TECH_RADAR', 'FLAVOR_CITY_DEFENSE', 20),
	('TECH_RADAR', 'FLAVOR_ESPIONAGE', 10),

	-------------------------------------------------------

	('TECH_ECOLOGY', 'FLAVOR_PRODUCTION', 20), -- Alternative Energy // Buildings: Hydro, Solar, Nuclear, Wind, Tidal
	('TECH_ECOLOGY', 'FLAVOR_TILE_IMPROVEMENT', 20),
	('TECH_ECOLOGY', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 20),

	('TECH_TELECOM', 'FLAVOR_WONDER', 10), -- Buildings: Interpretive Center, Wonders: CN Tower, Yields: Landmark +2C
	('TECH_TELECOM', 'FLAVOR_CULTURE', 20),

	('TECH_SATELLITES', 'FLAVOR_SCIENCE', 20), -- Wonders: Sydney, Projects: Apollo, Ability: RevealsMap, Yields: Musicians&Landmarks +2C
	('TECH_SATELLITES', 'FLAVOR_CULTURE', 10),
	('TECH_SATELLITES', 'FLAVOR_WONDER', 10),
	('TECH_SATELLITES', 'FLAVOR_SPACESHIP', 75),

	('TECH_ADVANCED_BALLISTICS', 'FLAVOR_NAVAL', 15), -- Units: ModernArmor, RocketART, SAM, Guided Missile
	('TECH_ADVANCED_BALLISTICS', 'FLAVOR_OFFENSE', 30),
	('TECH_ADVANCED_BALLISTICS', 'FLAVOR_RANGED', 10),
	('TECH_ADVANCED_BALLISTICS', 'FLAVOR_MOBILE', 10),
	('TECH_ADVANCED_BALLISTICS', 'FLAVOR_ANTIAIR', 10),
	('TECH_ADVANCED_BALLISTICS', 'FLAVOR_AIR', 10),

	('TECH_MOBILE_TACTICS', 'FLAVOR_OFFENSE', 10), -- Units: Helicopter, Mech.Inf, Bazooka, Ability: FasteronRails
	('TECH_MOBILE_TACTICS', 'FLAVOR_DEFENSE', 10),
	('TECH_MOBILE_TACTICS', 'FLAVOR_MOBILE', 10),
	('TECH_MOBILE_TACTICS', 'FLAVOR_RANGED', 20),

	-------------------------------------------------------------

	('TECH_ROBOTICS', 'FLAVOR_SPACESHIP', 50), -- Units: Nuclear Submarine, Supercarrier, GDR, Buildings: SpaceshipFactory, Wonders: Hubble, Yields: Farm&Pasture +3F, Mine +3P
	('TECH_ROBOTICS', 'FLAVOR_OFFENSE', 25),
	('TECH_ROBOTICS', 'FLAVOR_MOBILE', 20),
	('TECH_ROBOTICS', 'FLAVOR_NAVAL', 25),
	('TECH_ROBOTICS', 'FLAVOR_AIR', 10),
	('TECH_ROBOTICS', 'FLAVOR_TILE_IMPROVEMENT', 10),
	('TECH_ROBOTICS', 'FLAVOR_WONDER', 10),

	('TECH_INTERNET', 'FLAVOR_CULTURE', 50), -- Wonders: Firewall, Ability: TourismBoost, Yields: Writers +2C, Merchants +3G, Servants +1C
	('TECH_INTERNET', 'FLAVOR_GOLD', 15),

	('TECH_LASERS', 'FLAVOR_OFFENSE', 10), -- Units: Missile Cruiser, Nuke, JetFighter, XCOM, Yields: Eng. +3P
	('TECH_LASERS', 'FLAVOR_NUKE', 30),
	('TECH_LASERS', 'FLAVOR_AIR', 15),
	('TECH_LASERS', 'FLAVOR_NAVAL', 15),
	('TECH_LASERS', 'FLAVOR_PRODUCTION', 15),

	('TECH_STEALTH', 'FLAVOR_AIR', 15), -- Units: Sensor Combat Ship, StealthBomber, Yields: Fort&Citadel +4S
	('TECH_STEALTH', 'FLAVOR_NAVAL', 15),

	---------------------------------------------------------

	('TECH_NANOTECHNOLOGY', 'FLAVOR_SPACESHIP', 20), -- Units: SS Stasis, Ability: FalloutImmunity

	('TECH_PARTICLE_PHYSICS', 'FLAVOR_SPACESHIP', 20), -- Units: SS Engine, Wonders: CERN
	('TECH_PARTICLE_PHYSICS', 'FLAVOR_WONDER', 10),

	('TECH_GLOBALIZATION', 'FLAVOR_DIPLOMACY', 20), -- Units: SS Cockpit, Ability: + Delegates
	('TECH_GLOBALIZATION', 'FLAVOR_SPACESHIP', 20),

	('TECH_NUCLEAR_FUSION', 'FLAVOR_SPACESHIP', 20), -- Units: SS Booster, Yields: Scientist +3S
	('TECH_NUCLEAR_FUSION', 'FLAVOR_SCIENCE', 15),

	---------------------------------------------------------

	('TECH_FUTURE_TECH', 'FLAVOR_HAPPINESS', 10); -- Ability: +1 Happiness in all cities bleh

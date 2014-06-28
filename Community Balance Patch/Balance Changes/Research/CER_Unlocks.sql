-- CER_Unlocks.sql created by Techs spreadsheet, Unlocks tab:
-- https://drive.google.com/folderview?id=0B58Ehya83q19eVlvWXdmLXZ6UUE

UPDATE Resources SET TechReveal='TECH_MINING',                   TechCityTrade='TECH_MINING'                   WHERE Type='RESOURCE_IRON';
UPDATE Resources SET TechReveal='TECH_METALLURGY',               TechCityTrade='TECH_STEAM_POWER'              WHERE Type='RESOURCE_COAL';
UPDATE Resources SET TechReveal='TECH_CHEMISTRY',                TechCityTrade='TECH_DYNAMITE'                 WHERE Type='RESOURCE_OIL';
UPDATE Resources SET TechReveal='TECH_CHEMISTRY',                TechCityTrade='TECH_FLIGHT'                   WHERE Type='RESOURCE_ALUMINUM';



UPDATE Builds SET PrereqTech='TECH_ARCHERY'                  WHERE Type='BUILD_CAMP';
UPDATE Builds SET PrereqTech='TECH_ANIMAL_HUSBANDRY'         WHERE Type='BUILD_REMOVE_FOREST';
UPDATE Builds SET PrereqTech='TECH_THE_WHEEL'                WHERE Type='BUILD_REMOVE_JUNGLE';
UPDATE Builds SET PrereqTech='TECH_BRONZE_WORKING'           WHERE Type='BUILD_REMOVE_MARSH';
UPDATE Builds SET PrereqTech='TECH_TRAPPING'                 WHERE Type='BUILD_TRADING_POST';
UPDATE Builds SET PrereqTech='TECH_TRAPPING'                 WHERE Type='BUILD_ROAD';
UPDATE Builds SET PrereqTech='TECH_IRON_WORKING'             WHERE Type='BUILD_LUMBERMILL';
UPDATE Builds SET PrereqTech='TECH_DYNAMITE'                 WHERE Type='BUILD_WELL';
UPDATE Builds SET PrereqTech='TECH_DYNAMITE'                 WHERE Type='BUILD_OFFSHORE_PLATFORM';
UPDATE Builds SET PrereqTech='TECH_ENGINEERING'              WHERE Type='BUILD_TERRACE_FARM';
UPDATE Builds SET PrereqTech='TECH_MASONRY'                  WHERE Type='BUILD_FORT';
UPDATE Builds SET PrereqTech='TECH_IRON_WORKING'             WHERE Type='BUILD_BRAZILWOOD_CAMP';
UPDATE Builds SET PrereqTech='TECH_CIVIL_SERVICE'            WHERE Type='BUILD_POLDER';



UPDATE Buildings SET PrereqTech='TECH_COMBUSTION'               WHERE BuildingClass='BUILDINGCLASS_AIRPORT';
UPDATE Buildings SET PrereqTech='TECH_ENGINEERING'              WHERE BuildingClass='BUILDINGCLASS_AQUEDUCT';
UPDATE Buildings SET PrereqTech='TECH_MACHINERY'                WHERE BuildingClass='BUILDINGCLASS_ARMORY';
UPDATE Buildings SET PrereqTech='TECH_MINING'                   WHERE BuildingClass='BUILDINGCLASS_BARRACKS';
UPDATE Buildings SET PrereqTech='TECH_ACOUSTICS'                WHERE BuildingClass='BUILDINGCLASS_CONSTABLE';
UPDATE Buildings SET PrereqTech='TECH_HORSEBACK_RIDING'         WHERE BuildingClass='BUILDINGCLASS_CIRCUS';
UPDATE Buildings SET PrereqTech='TECH_INDUSTRIALIZATION'        WHERE BuildingClass='BUILDINGCLASS_FACTORY';
UPDATE Buildings SET PrereqTech='TECH_TRAPPING'                 WHERE BuildingClass='BUILDINGCLASS_CARAVANSARY';
UPDATE Buildings SET PrereqTech='TECH_OPTICS'                   WHERE BuildingClass='BUILDINGCLASS_LIGHTHOUSE';
UPDATE Buildings SET PrereqTech='TECH_CURRENCY'                 WHERE BuildingClass='BUILDINGCLASS_MARKET';
UPDATE Buildings SET PrereqTech='TECH_ECOLOGY'                  WHERE BuildingClass='BUILDINGCLASS_MEDICAL_LAB';
UPDATE Buildings SET PrereqTech='TECH_RADIO'                    WHERE BuildingClass='BUILDINGCLASS_MILITARY_BASE';
UPDATE Buildings SET PrereqTech='TECH_ARCHITECTURE'             WHERE BuildingClass='BUILDINGCLASS_OPERA_HOUSE';
UPDATE Buildings SET PrereqTech='TECH_NULL'                     WHERE BuildingClass='BUILDINGCLASS_SHRINE';
UPDATE Buildings SET PrereqTech='TECH_NUCLEAR_FISSION'          WHERE BuildingClass='BUILDINGCLASS_SPACESHIP_FACTORY';
UPDATE Buildings SET PrereqTech='TECH_MASONRY'                  WHERE BuildingClass='BUILDINGCLASS_STONEWORKS';
UPDATE Buildings SET PrereqTech='TECH_THE_WHEEL'                WHERE BuildingClass='BUILDINGCLASS_STABLE';
UPDATE Buildings SET PrereqTech='TECH_REPLACEABLE_PARTS'        WHERE BuildingClass='BUILDINGCLASS_STADIUM';
UPDATE Buildings SET PrereqTech='TECH_ELECTRONICS'              WHERE BuildingClass='BUILDINGCLASS_STOCK_EXCHANGE';
UPDATE Buildings SET PrereqTech='TECH_MASONRY'                  WHERE BuildingClass='BUILDINGCLASS_STONE_WORKS';
UPDATE Buildings SET PrereqTech='TECH_MASONRY'                  WHERE BuildingClass='BUILDINGCLASS_WALLS';
UPDATE Buildings SET PrereqTech='TECH_METAL_CASTING'            WHERE BuildingClass='BUILDINGCLASS_WINDMILL';
UPDATE Buildings SET PrereqTech='TECH_BRONZE_WORKING'           WHERE BuildingClass='BUILDINGCLASS_WORKSHOP';
UPDATE Buildings SET PrereqTech='TECH_HORSEBACK_RIDING'         WHERE BuildingClass='BUILDINGCLASS_CIRCUS_MAXIMUS';
UPDATE Buildings SET PrereqTech='TECH_THEOLOGY'                 WHERE BuildingClass='BUILDINGCLASS_GRAND_TEMPLE';
UPDATE Buildings SET PrereqTech='TECH_ARCHAEOLOGY'              WHERE BuildingClass='BUILDINGCLASS_HERMITAGE';
UPDATE Buildings SET PrereqTech='TECH_MINING'                   WHERE BuildingClass='BUILDINGCLASS_HEROIC_EPIC';
UPDATE Buildings SET PrereqTech='TECH_STEEL'                    WHERE BuildingClass='BUILDINGCLASS_IRONWORKS';
UPDATE Buildings SET PrereqTech='TECH_ARCHITECTURE'             WHERE BuildingClass='BUILDINGCLASS_MUSICIANS_GUILD';
UPDATE Buildings SET PrereqTech='TECH_PHILOSOPHY'               WHERE BuildingClass='BUILDINGCLASS_WRITERS_GUILD';
UPDATE Buildings SET PrereqTech='TECH_PHYSICS'                  WHERE BuildingClass='BUILDINGCLASS_ALHAMBRA';
UPDATE Buildings SET PrereqTech='TECH_GUILDS'                   WHERE BuildingClass='BUILDINGCLASS_ANGKOR_WAT';
UPDATE Buildings SET PrereqTech='TECH_ENGINEERING'              WHERE BuildingClass='BUILDINGCLASS_BANAUE_RICE_TERRACES';
UPDATE Buildings SET PrereqTech='TECH_ECONOMICS'                WHERE BuildingClass='BUILDINGCLASS_BIG_BEN';
UPDATE Buildings SET PrereqTech='TECH_REPLACEABLE_PARTS'        WHERE BuildingClass='BUILDINGCLASS_BROADWAY';
UPDATE Buildings SET PrereqTech='TECH_CIVIL_SERVICE'            WHERE BuildingClass='BUILDINGCLASS_CHICHEN_ITZA';
UPDATE Buildings SET PrereqTech='TECH_PHILOSOPHY'               WHERE BuildingClass='BUILDINGCLASS_CHURCHES_LALIBELA';
UPDATE Buildings SET PrereqTech='TECH_CURRENCY'                 WHERE BuildingClass='BUILDINGCLASS_COLOSSUS';
UPDATE Buildings SET PrereqTech='TECH_ECOLOGY'                  WHERE BuildingClass='BUILDINGCLASS_CRISTO_REDENTOR';
UPDATE Buildings SET PrereqTech='TECH_ELECTRICITY'              WHERE BuildingClass='BUILDINGCLASS_EIFFEL_TOWER';
UPDATE Buildings SET PrereqTech='TECH_PRINTING_PRESS'           WHERE BuildingClass='BUILDINGCLASS_FORBIDDEN_PALACE';
UPDATE Buildings SET PrereqTech='TECH_BANKING'                  WHERE BuildingClass='BUILDINGCLASS_GLOBE_THEATER';
UPDATE Buildings SET PrereqTech='TECH_OPTICS'                   WHERE BuildingClass='BUILDINGCLASS_GREAT_LIGHTHOUSE';
UPDATE Buildings SET PrereqTech='TECH_MASONRY'                  WHERE BuildingClass='BUILDINGCLASS_GREAT_WALL';
UPDATE Buildings SET PrereqTech='TECH_CONSTRUCTION'             WHERE BuildingClass='BUILDINGCLASS_HAGIA_SOPHIA';
UPDATE Buildings SET PrereqTech='TECH_CONSTRUCTION'             WHERE BuildingClass='BUILDINGCLASS_HANGING_GARDEN';
UPDATE Buildings SET PrereqTech='TECH_CHIVALRY'                 WHERE BuildingClass='BUILDINGCLASS_HIMEJI_CASTLE';
UPDATE Buildings SET PrereqTech='TECH_REPLACEABLE_PARTS'        WHERE BuildingClass='BUILDINGCLASS_HOLLYWOOD';
UPDATE Buildings SET PrereqTech='TECH_ROCKETRY'                 WHERE BuildingClass='BUILDINGCLASS_HUBBLE';
UPDATE Buildings SET PrereqTech='TECH_ELECTRONICS'              WHERE BuildingClass='BUILDINGCLASS_ITAIPU_DAM';
UPDATE Buildings SET PrereqTech='TECH_RAILROAD'                 WHERE BuildingClass='BUILDINGCLASS_KREMLIN';
UPDATE Buildings SET PrereqTech='TECH_PARTICLE_PHYSICS'         WHERE BuildingClass='BUILDINGCLASS_LARGE_HADRON_COLLIDER';
UPDATE Buildings SET PrereqTech='TECH_BANKING'                  WHERE BuildingClass='BUILDINGCLASS_LEANING_TOWER';
UPDATE Buildings SET PrereqTech='TECH_ENGINEERING'              WHERE BuildingClass='BUILDINGCLASS_MACHU_PICHU';
UPDATE Buildings SET PrereqTech='TECH_HORSEBACK_RIDING'         WHERE BuildingClass='BUILDINGCLASS_MAUSOLEUM_HALICARNASSUS';
UPDATE Buildings SET PrereqTech='TECH_RADIO'                    WHERE BuildingClass='BUILDINGCLASS_NEUSCHWANSTEIN';
UPDATE Buildings SET PrereqTech='TECH_DRAMA'                    WHERE BuildingClass='BUILDINGCLASS_ORACLE';
UPDATE Buildings SET PrereqTech='TECH_DYNAMITE'                 WHERE BuildingClass='BUILDINGCLASS_PANAMA_CANAL';
UPDATE Buildings SET PrereqTech='TECH_DRAMA'                    WHERE BuildingClass='BUILDINGCLASS_PARTHENON';
UPDATE Buildings SET PrereqTech='TECH_CONSTRUCTION'             WHERE BuildingClass='BUILDINGCLASS_PETRA';
UPDATE Buildings SET PrereqTech='TECH_EDUCATION'                WHERE BuildingClass='BUILDINGCLASS_PORCELAIN_TOWER';
UPDATE Buildings SET PrereqTech='TECH_REPLACEABLE_PARTS'        WHERE BuildingClass='BUILDINGCLASS_PRORA_RESORT';
UPDATE Buildings SET PrereqTech='TECH_TRAPPING'                 WHERE BuildingClass='BUILDINGCLASS_PYRAMID';
UPDATE Buildings SET PrereqTech='TECH_ARCHITECTURE'             WHERE BuildingClass='BUILDINGCLASS_SISTINE_CHAPEL';
UPDATE Buildings SET PrereqTech='TECH_REFRIGERATION'            WHERE BuildingClass='BUILDINGCLASS_STATUE_OF_LIBERTY';
UPDATE Buildings SET PrereqTech='TECH_MATHEMATICS'              WHERE BuildingClass='BUILDINGCLASS_STATUE_ZEUS';
UPDATE Buildings SET PrereqTech='TECH_CALENDAR'                 WHERE BuildingClass='BUILDINGCLASS_STONEHENGE';
UPDATE Buildings SET PrereqTech='TECH_PRINTING_PRESS'           WHERE BuildingClass='BUILDINGCLASS_TAJ_MAHAL';
UPDATE Buildings SET PrereqTech='TECH_CALENDAR'                 WHERE BuildingClass='BUILDINGCLASS_TERRACOTTA_ARMY';
UPDATE Buildings SET PrereqTech='TECH_ACOUSTICS'                WHERE BuildingClass='BUILDINGCLASS_UFFIZI';
UPDATE Buildings SET PrereqTech='TECH_ARCHITECTURE'             WHERE BuildingClass='BUILDINGCLASS_WAT_PHRA_KAEW';


UPDATE Buildings SET PrereqTech='TECH_STEAM_POWER'              WHERE Type='BUILDING_ENGLISH_STEAM_MILL';






UPDATE Units SET PrereqTech='TECH_BRONZE_WORKING'           WHERE Type='UNIT_ROMAN_LEGION';






UPDATE Units SET PrereqTech='TECH_SAILING'                  WHERE Class='UNITCLASS_CARGO_SHIP';
UPDATE Units SET PrereqTech='TECH_COMPASS'                  WHERE Class='UNITCLASS_CARAVEL';
UPDATE Units SET PrereqTech='TECH_COMPASS'                  WHERE Class='UNITCLASS_GALLEASS';
UPDATE Units SET PrereqTech='TECH_MINING'                   WHERE Class='UNITCLASS_SENTINEL';
UPDATE Units SET PrereqTech='TECH_MATHEMATICS'              WHERE Class='UNITCLASS_COMPOSITE_BOWMAN';
UPDATE Units SET PrereqTech='TECH_METAL_CASTING'            WHERE Class='UNITCLASS_PIKEMAN';
UPDATE Units SET PrereqTech='TECH_BIOLOGY'                  WHERE Class='UNITCLASS_CONSCRIPT';
UPDATE Units SET PrereqTech='TECH_STEAM_POWER'              WHERE Class='UNITCLASS_GATLINGGUN';
UPDATE Units SET PrereqTech='TECH_INDUSTRIALIZATION'        WHERE Class='UNITCLASS_GREAT_WAR_INFANTRY';
UPDATE Units SET PrereqTech='TECH_INDUSTRIALIZATION'        WHERE Class='UNITCLASS_IRONCLAD';
UPDATE Units SET PrereqTech='TECH_RAILROAD'                 WHERE Class='UNITCLASS_WWI_TANK';
UPDATE Units SET PrereqTech='TECH_COMBUSTION'               WHERE Class='UNITCLASS_ANTI_AIRCRAFT_GUN';
UPDATE Units SET PrereqTech='TECH_COMBUSTION'               WHERE Class='UNITCLASS_ANTI_TANK_GUN';
UPDATE Units SET PrereqTech='TECH_COMBUSTION'               WHERE Class='UNITCLASS_BATTLESHIP';
UPDATE Units SET PrereqTech='TECH_COMPUTERS'                WHERE Class='UNITCLASS_BAZOOKA';
UPDATE Units SET PrereqTech='TECH_COMBINED_ARMS'            WHERE Class='UNITCLASS_BOMBER';
UPDATE Units SET PrereqTech='TECH_COMBUSTION'               WHERE Class='UNITCLASS_CARRIER';
UPDATE Units SET PrereqTech='TECH_REFRIGERATION'            WHERE Class='UNITCLASS_DESTROYER';
UPDATE Units SET PrereqTech='TECH_COMBINED_ARMS'            WHERE Class='UNITCLASS_FIGHTER';
UPDATE Units SET PrereqTech='TECH_ROBOTICS'                 WHERE Class='UNITCLASS_HELICOPTER_GUNSHIP';
UPDATE Units SET PrereqTech='TECH_COMBUSTION'               WHERE Class='UNITCLASS_INFANTRY';
UPDATE Units SET PrereqTech='TECH_ELECTRONICS'              WHERE Class='UNITCLASS_MACHINE_GUN';
UPDATE Units SET PrereqTech='TECH_COMBINED_ARMS'            WHERE Class='UNITCLASS_MARINE';
UPDATE Units SET PrereqTech='TECH_ROBOTICS'                 WHERE Class='UNITCLASS_MECHANIZED_INFANTRY';
UPDATE Units SET PrereqTech='TECH_COMPUTERS'                WHERE Class='UNITCLASS_MISSILE_DESTROYER';
UPDATE Units SET PrereqTech='TECH_PARTICLE_PHYSICS'         WHERE Class='UNITCLASS_NUCLEAR_MISSILE';
UPDATE Units SET PrereqTech='TECH_COMPUTERS'                WHERE Class='UNITCLASS_NUCLEAR_SUBMARINE';
UPDATE Units SET PrereqTech='TECH_COMBINED_ARMS'            WHERE Class='UNITCLASS_PARATROOPER';
UPDATE Units SET PrereqTech='TECH_RADIO'                    WHERE Class='UNITCLASS_SUBMARINE';
UPDATE Units SET PrereqTech='TECH_COMPUTERS'                WHERE Class='UNITCLASS_MOBILE_SAM';
UPDATE Units SET PrereqTech='TECH_SATELLITES'               WHERE Class='UNITCLASS_GUIDED_MISSILE';
UPDATE Units SET PrereqTech='TECH_SATELLITES'               WHERE Class='UNITCLASS_SS_BOOSTER';
UPDATE Units SET PrereqTech='TECH_ADVANCED_BALLISTICS'      WHERE Class='UNITCLASS_SS_ENGINE';
UPDATE Units SET PrereqTech='TECH_PARTICLE_PHYSICS'         WHERE Class='UNITCLASS_SS_COCKPIT';
UPDATE Units SET PrereqTech='TECH_NUCLEAR_FUSION'           WHERE Class='UNITCLASS_XCOM_SQUAD';


UPDATE LoadedFile SET Value=1 WHERE Type='CER_Unlocks.sql';
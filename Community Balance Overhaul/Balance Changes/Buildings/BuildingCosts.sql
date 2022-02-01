-- ANCIENT

UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_GRANARY';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_YURT';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_MONUMENT';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_MAYA_PYRAMID';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_SHRINE';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_STELE';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_WELL';
UPDATE Buildings SET Cost = '65' WHERE Type = 'BUILDING_GROVE';

UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_HERBALIST';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_BARRACKS';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_IKANDA';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_BAZAAR';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_MARKET';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_STONE_WORKS';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_LONGHOUSE';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_WALLS';
UPDATE Buildings SET Cost = '110' WHERE Type = 'BUILDING_WALLS_OF_BABYLON';


UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_SHRINE';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_MAYA_PYRAMID';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_GROVE';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_MONUMENT';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_STELE';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_BARRACKS';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_IKANDA';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_GRANARY';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_YURT';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_STONE_WORKS';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_LONGHOUSE';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_WALLS';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_WALLS_OF_BABYLON';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_WELL';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_HERBALIST';

-- CLASSICAL
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_FLOATING_GARDENS';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_COLOSSEUM';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_FLAVIAN_COLOSSEUM';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_FORGE';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_SIEGE_WORKSHOP';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_LIBRARY';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_PAPER_MAKER';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_LIGHTHOUSE';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_JELLING_STONES';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_ROYAL_LIBRARY';
UPDATE Buildings SET Cost = '150' WHERE Type = 'BUILDING_WATERMILL';

UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_AMPHITHEATER';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_ODEON';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_WRITERS_GUILD';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_AQUEDUCT';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_INDUS_CANAL';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_BURIAL_TOMB';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_CARAVANSARY';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_COURTHOUSE';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_SATRAPS_COURT';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_TEMPLE';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_BASILICA';
UPDATE Buildings SET Cost = '200' WHERE Type = 'BUILDING_BATH';


UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_CARAVANSARY';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_BURIAL_TOMB';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_AMPHITHEATER';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_ODEON';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_AQUEDUCT';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_INDUS_CANAL';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_COLOSSEUM';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_FLAVIAN_COLOSSEUM';

UPDATE Buildings SET GoldMaintenance = '3' WHERE Type = 'BUILDING_COURTHOUSE';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_SATRAPS_COURT';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_FLOATING_GARDENS';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_FORGE';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_SIEGE_WORKSHOP';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_LIBRARY';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_PAPER_MAKER';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_LIGHTHOUSE';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_JELLING_STONES';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_ROYAL_LIBRARY';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_TEMPLE';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_BASILICA';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_BATH';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_WATERMILL';
UPDATE Buildings SET GoldMaintenance = '1' WHERE Type = 'BUILDING_WRITERS_GUILD';

UPDATE Buildings SET FaithCost = '200' WHERE Type = 'BUILDING_AMPHITHEATER';
UPDATE Buildings SET FaithCost = '200' WHERE Type = 'BUILDING_ODEON';

-- MEDIEVAL
--UPDATE Buildings SET Cost = '275' WHERE Type = 'BUILDING_MISSION';

UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_ARMORY';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_DOJO';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_CANDI';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_CASTLE';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_STABLE';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_DUCAL_STABLE';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_CEILIDH_HALL';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_CIRCUS';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_GARDEN';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_UNIVERSITY';
UPDATE Buildings SET Cost = '300' WHERE Type = 'BUILDING_SEOWON';

UPDATE Buildings SET Cost = '350' WHERE Type = 'BUILDING_WAT';
UPDATE Buildings SET Cost = '350' WHERE Type = 'BUILDING_HARBOR';
UPDATE Buildings SET Cost = '350' WHERE Type = 'BUILDING_MINT';
UPDATE Buildings SET Cost = '350' WHERE Type = 'BUILDING_HANSE';
UPDATE Buildings SET Cost = '350' WHERE Type = 'BUILDING_WORKSHOP';
UPDATE Buildings SET Cost = '350' WHERE Type = 'BUILDING_ARTISTS_GUILD';

UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_MINT';
UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_HANSE';

UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_ARMORY';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_DOJO';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_CANDI';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_CASTLE';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_STABLE';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_DUCAL_STABLE';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_CEILIDH_HALL';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_CIRCUS';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_HARBOR';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_UNIVERSITY';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_SEOWON';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_WORKSHOP';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_ARTISTS_GUILD';
UPDATE Buildings SET GoldMaintenance = '3' WHERE Type = 'BUILDING_GARDEN';
UPDATE Buildings SET GoldMaintenance = '2' WHERE Type = 'BUILDING_WAT';
--UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_MISSION';

UPDATE Buildings SET FaithCost = '0' WHERE Type = 'BUILDING_CEILIDH_HALL';
UPDATE Buildings SET FaithCost = '0' WHERE Type = 'BUILDING_WAT';

UPDATE Buildings SET FaithCost = '300' WHERE Type = 'BUILDING_UNIVERSITY';
UPDATE Buildings SET FaithCost = '300' WHERE Type = 'BUILDING_SEOWON';

-- RENAISSANCE
UPDATE Buildings SET Cost = '500' WHERE Type = 'BUILDING_BANK';
UPDATE Buildings SET Cost = '500' WHERE Type = 'BUILDING_COFFEE_HOUSE';
UPDATE Buildings SET Cost = '500' WHERE Type = 'BUILDING_CONSTABLE';
UPDATE Buildings SET Cost = '500' WHERE Type = 'BUILDING_OBSERVATORY';
UPDATE Buildings SET Cost = '500' WHERE Type = 'BUILDING_GROCER';

UPDATE Buildings SET Cost = '600' WHERE Type = 'BUILDING_OPERA_HOUSE';
UPDATE Buildings SET Cost = '600' WHERE Type = 'BUILDING_WINDMILL';
UPDATE Buildings SET Cost = '600' WHERE Type = 'BUILDING_MUSICIANS_GUILD';

UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_COFFEE_HOUSE';
UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_CONSTABLE';
UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_OBSERVATORY';
UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_OPERA_HOUSE';
UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_WINDMILL';
UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_MUSICIANS_GUILD';
UPDATE Buildings SET GoldMaintenance = '4' WHERE Type = 'BUILDING_GROCER';

UPDATE Buildings SET FaithCost = '350' WHERE Type = 'BUILDING_OPERA_HOUSE';

-- INDUSTRIAL
UPDATE Buildings SET Cost = '800' WHERE Type = 'BUILDING_KREPOST';
UPDATE Buildings SET Cost = '800' WHERE Type = 'BUILDING_SKOLA';

UPDATE Buildings SET Cost = '1000' WHERE Type = 'BUILDING_ARSENAL';
UPDATE Buildings SET Cost = '1000' WHERE Type = 'BUILDING_HOTEL';
UPDATE Buildings SET Cost = '1000' WHERE Type = 'BUILDING_PUBLIC_SCHOOL';
UPDATE Buildings SET Cost = '1000' WHERE Type = 'BUILDING_THEATRE';

UPDATE Buildings SET Cost = '1250' WHERE Type = 'BUILDING_FACTORY';
UPDATE Buildings SET Cost = '1250' WHERE Type = 'BUILDING_MILITARY_ACADEMY';
UPDATE Buildings SET Cost = '1250' WHERE Type = 'BUILDING_STOCKYARD';
UPDATE Buildings SET Cost = '1250' WHERE Type = 'BUILDING_MUSEUM';
UPDATE Buildings SET Cost = '1250' WHERE Type = 'BUILDING_TRAINSTATION';

UPDATE Buildings SET Cost = '1500' WHERE Type = 'BUILDING_SEAPORT';

UPDATE Buildings SET GoldMaintenance = '5' WHERE Type = 'BUILDING_KREPOST';
UPDATE Buildings SET GoldMaintenance = '5' WHERE Type = 'BUILDING_SKOLA';


UPDATE Buildings SET GoldMaintenance = '6' WHERE Type = 'BUILDING_HOTEL';
UPDATE Buildings SET GoldMaintenance = '6' WHERE Type = 'BUILDING_MILITARY_ACADEMY';
UPDATE Buildings SET GoldMaintenance = '6' WHERE Type = 'BUILDING_MUSEUM';
UPDATE Buildings SET GoldMaintenance = '6' WHERE Type = 'BUILDING_PUBLIC_SCHOOL';
UPDATE Buildings SET GoldMaintenance = '6' WHERE Type = 'BUILDING_THEATRE';
UPDATE Buildings SET GoldMaintenance = '6' WHERE Type = 'BUILDING_ARSENAL';

UPDATE Buildings SET GoldMaintenance = '7' WHERE Type = 'BUILDING_STOCKYARD';
UPDATE Buildings SET GoldMaintenance = '7' WHERE Type = 'BUILDING_FACTORY';
UPDATE Buildings SET GoldMaintenance = '7' WHERE Type = 'BUILDING_TRAINSTATION';

UPDATE Buildings SET GoldMaintenance = '7' WHERE Type = 'BUILDING_SEAPORT';

UPDATE Buildings SET FaithCost = '400' WHERE Type = 'BUILDING_MUSEUM';
UPDATE Buildings SET FaithCost = '400' WHERE Type = 'BUILDING_PUBLIC_SCHOOL';

-- MODERN

UPDATE Buildings SET Cost = '1600' WHERE Type = 'BUILDING_MINEFIELD';

UPDATE Buildings SET Cost = '1800' WHERE Type = 'BUILDING_HOSPITAL';
UPDATE Buildings SET Cost = '1800' WHERE Type = 'BUILDING_COAL_PLANT';
UPDATE Buildings SET Cost = '1800' WHERE Type = 'BUILDING_STOCK_EXCHANGE';

UPDATE Buildings SET Cost = '2000' WHERE Type = 'BUILDING_LABORATORY';
UPDATE Buildings SET Cost = '2000' WHERE Type = 'BUILDING_BROADCAST_TOWER';

UPDATE Buildings SET GoldMaintenance = '0' WHERE Type = 'BUILDING_STOCK_EXCHANGE';

UPDATE Buildings SET GoldMaintenance = '8' WHERE Type = 'BUILDING_MINEFIELD';
UPDATE Buildings SET GoldMaintenance = '8' WHERE Type = 'BUILDING_HOSPITAL';

UPDATE Buildings SET GoldMaintenance = '8' WHERE Type = 'BUILDING_BROADCAST_TOWER';
UPDATE Buildings SET GoldMaintenance = '8' WHERE Type = 'BUILDING_COAL_PLANT';

UPDATE Buildings SET GoldMaintenance = '8' WHERE Type = 'BUILDING_LABORATORY';

UPDATE Buildings SET FaithCost = '500' WHERE Type = 'BUILDING_BROADCAST_TOWER';
UPDATE Buildings SET FaithCost = '500' WHERE Type = 'BUILDING_LABORATORY';

-- ATOMIC

UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_MEDICAL_LAB';

UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_MILITARY_BASE';
UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_POLICE_STATION';
UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_STADIUM';
UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_BOMB_SHELTER';
UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_RECYCLING_CENTER';

UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_AIRPORT';

UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_BOMB_SHELTER';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_MEDICAL_LAB';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_MILITARY_BASE';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_POLICE_STATION';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_STADIUM';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_RECYCLING_CENTER';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_AIRPORT';

UPDATE Buildings SET FaithCost = '600' WHERE Type = 'BUILDING_STADIUM';
-- INFORMATION

UPDATE Buildings SET Cost = '2500' WHERE Type = 'BUILDING_NUCLEAR_PLANT';
UPDATE Buildings SET Cost = '2250' WHERE Type = 'BUILDING_SOLAR_PLANT';
UPDATE Buildings SET Cost = '2500' WHERE Type = 'BUILDING_WIND_PLANT';
UPDATE Buildings SET Cost = '2500' WHERE Type = 'BUILDING_HYDRO_PLANT';

UPDATE Buildings SET Cost = '2750' WHERE Type = 'BUILDING_SPACESHIP_FACTORY';

UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_HYDRO_PLANT';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_WIND_PLANT';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_NUCLEAR_PLANT';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_SOLAR_PLANT';
UPDATE Buildings SET GoldMaintenance = '10' WHERE Type = 'BUILDING_SPACESHIP_FACTORY';




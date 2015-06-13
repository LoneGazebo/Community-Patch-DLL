
-- Difficulty Mod Value Indicators
-- 0 = Prince (Default)
-- 1 = King
-- 2 = Emperor
-- 3 = Immortal
-- 4 = Deity
-- 5 = Super Deity

-- Era bonus multiplier
INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GAME_DIFFICULTY_MULTIPLIER', '1'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 0 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GAME_DIFFICULTY_MULTIPLIER', '2'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 1 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GAME_DIFFICULTY_MULTIPLIER', '3'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 2 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GAME_DIFFICULTY_MULTIPLIER', '4'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 3 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GAME_DIFFICULTY_MULTIPLIER', '5'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 4 );

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_GAME_DIFFICULTY_MULTIPLIER', '7'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 5 );

--Difficulties
DELETE FROM HandicapInfo_AIFreeTechs WHERE HandicapType IN ('HANDICAP_KING' , 'HANDICAP_EMPEROR' , 'HANDICAP_IMMORTAL', 'HANDICAP_DEITY');

UPDATE HandicapInfos
SET AIPerEraModifier='-4' , HappinessDefault='7', Gold='0' , AIStartingDefenseUnits = '1' , AIDeclareWarProb='125', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , ProductionFreeUnits='8' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='2' , TechNumOptionsConsidered='2' , PolicyNumOptionsConsidered='2' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='50' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='75' , AIUnitUpgradePercent='75', AIAdvancedStartPercent='130', AIFreeXP='15' , AIFreeXPPercent='30'
WHERE Type = 'HANDICAP_KING';

UPDATE HandicapInfos
SET AIPerEraModifier='-6' , HappinessDefault='6', Gold='0' , AIStartingDefenseUnits = '2' , AIDeclareWarProb='150', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , ProductionFreeUnits='7' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='2' , TechNumOptionsConsidered='2' , PolicyNumOptionsConsidered='2' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='50' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='75' , AIUnitUpgradePercent='75', AIAdvancedStartPercent='130', AIFreeXP='15' , AIFreeXPPercent='30'
WHERE Type = 'HANDICAP_EMPEROR';

UPDATE HandicapInfos
SET AIPerEraModifier='-8' , HappinessDefault='5', Gold='0' , AIStartingDefenseUnits = '3' , AIDeclareWarProb='175', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , ProductionFreeUnits='6' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='2' , TechNumOptionsConsidered='2' , PolicyNumOptionsConsidered='2' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='50' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='75' , AIUnitUpgradePercent='75', AIAdvancedStartPercent='130', AIFreeXP='15' , AIFreeXPPercent='30'
WHERE Type = 'HANDICAP_IMMORTAL';

UPDATE HandicapInfos
SET AIPerEraModifier='-10' , HappinessDefault='4', Gold='0' , AIStartingDefenseUnits = '3' , AIDeclareWarProb='200', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0' , ProductionFreeUnits='8' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='2' , TechNumOptionsConsidered='3' , PolicyNumOptionsConsidered='3' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='100' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='75' , AIUnitUpgradePercent='75', AIAdvancedStartPercent='130', AIFreeXP='15' , AIFreeXPPercent='30'
WHERE Type = 'HANDICAP_DEITY';

UPDATE HandicapInfos
SET AIPerEraModifier='0' , HappinessDefault='6', Gold='0' , AIStartingDefenseUnits = '1' , AIDeclareWarProb='100', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0', ProductionFreeUnits='8' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='12' , TechNumOptionsConsidered='1' , PolicyNumOptionsConsidered='1' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='100' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='100' , AIUnitUpgradePercent='100', AIAdvancedStartPercent='100', AIFreeXP='0' , AIFreeXPPercent='0'
WHERE Type = 'HANDICAP_AI_DEFAULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 0 );

UPDATE HandicapInfos
SET AIPerEraModifier='-4' , HappinessDefault='7', Gold='100' , AIStartingDefenseUnits = '2' , AIDeclareWarProb='100', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0', ProductionFreeUnits='7' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='2' , TechNumOptionsConsidered='2' , PolicyNumOptionsConsidered='2' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='100' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='90' , AIUnitUpgradePercent='90', AIAdvancedStartPercent='110', AIFreeXP='5' , AIFreeXPPercent='10'
WHERE Type = 'HANDICAP_AI_DEFAULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 1 );

UPDATE HandicapInfos
SET AIPerEraModifier='-6' , HappinessDefault='8', Gold='200' , AIStartingDefenseUnits = '2' , AIDeclareWarProb='100', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0', ProductionFreeUnits='6' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='5' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='3' , TechNumOptionsConsidered='3' , PolicyNumOptionsConsidered='3' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='100' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='85' , AIUnitUpgradePercent='85', AIAdvancedStartPercent='120', AIFreeXP='10' , AIFreeXPPercent='20'
WHERE Type = 'HANDICAP_AI_DEFAULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 2 );

UPDATE HandicapInfos
SET AIPerEraModifier='-8' , HappinessDefault='9', Gold='300' , AIStartingDefenseUnits = '3' , AIDeclareWarProb='110', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0', ProductionFreeUnits='6' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='4' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='100' , PolicyPercent='100' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='3' , TechNumOptionsConsidered='3' , PolicyNumOptionsConsidered='3' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='150' , AIBarbarianBonus='50' , AIWorkRateModifier='0' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='80' , AIUnitUpgradePercent='80', AIAdvancedStartPercent='130', AIFreeXP='15' , AIFreeXPPercent='30'
WHERE Type = 'HANDICAP_AI_DEFAULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 3 );

UPDATE HandicapInfos
SET AIPerEraModifier='-10' , HappinessDefault='10', Gold='400' , AIStartingDefenseUnits = '3' , AIDeclareWarProb='120', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0', ProductionFreeUnits='5' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='4' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='90' , PolicyPercent='90' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='3' , TechNumOptionsConsidered='3' , PolicyNumOptionsConsidered='3' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='150' , AIBarbarianBonus='50' , AIWorkRateModifier='10' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='75' , AIUnitUpgradePercent='75', AIAdvancedStartPercent='140', AIFreeXP='20' , AIFreeXPPercent='40'
WHERE Type = 'HANDICAP_AI_DEFAULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 4 );

UPDATE HandicapInfos
SET AIPerEraModifier='-12' , HappinessDefault='12', Gold='500' , AIStartingDefenseUnits = '3' , AIDeclareWarProb='130', NumCitiesUnhappinessMod='100' , PopulationUnhappinessMod='100' , AIStartingUnitMultiplier = '0', ProductionFreeUnits='5' , AIStartingWorkerUnits='0', ProductionFreeUnitsPopulationPercent='50' , ProductionFreeUnitsPerCity='4' , RouteCostPercent='100' , UnitCostPercent='100' , BuildingCostPercent='100' , ResearchPercent='80' , PolicyPercent='80' , ImprovementCostPercent='100' , CityProductionNumOptionsConsidered='3' , TechNumOptionsConsidered='3' , PolicyNumOptionsConsidered='3' , AttitudeChange='-1' , NoTechTradeModifier='40' , BarbCampGold='200' , AIBarbarianBonus='50' , AIWorkRateModifier='10' ,  AIUnhappinessPercent='100' , AIGrowthPercent='100' , AITrainPercent='100' , AICreatePercent='100' , AIConstructPercent='100', AIBuildingCostPercent='100' , AIUnitCostPercent='100' , AIUnitSupplyPercent='75' , AIUnitUpgradePercent='75', AIAdvancedStartPercent='150', AIFreeXP='25' , AIFreeXPPercent='50'
WHERE Type = 'HANDICAP_AI_DEFAULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='BALANCE_GLOBAL_DIFFICULTY_LEVEL' AND Value= 5 );

-- Minor Civ Free Units

UPDATE Eras
SET StartingMinorDefenseUnits = '0'
WHERE Type = 'ERA_ANCIENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '0'
WHERE Type = 'ERA_CLASSICAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '1'
WHERE Type = 'ERA_MEDIEVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '1'
WHERE Type = 'ERA_RENAISSANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '2'
WHERE Type = 'ERA_INDUSTRIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '2'
WHERE Type = 'ERA_MODERN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '3'
WHERE Type = 'ERA_POSTMODERN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
SET StartingMinorDefenseUnits = '3'
WHERE Type = 'ERA_FUTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
SET StartingMinorDefenseUnits = '1'
WHERE Type = 'HANDICAP_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
SET StartingMinorDefenseUnits = '1'
WHERE Type = 'HANDICAP_EMPEROR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
SET StartingMinorDefenseUnits = '2'
WHERE Type = 'HANDICAP_IMMORTAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
SET StartingMinorDefenseUnits = '3'
WHERE Type = 'HANDICAP_DEITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

-- Update World Size Tech/Policy city rate

UPDATE Worlds
SET NumCitiesPolicyCostMod = '15'
WHERE Type = 'WORLDSIZE_DUEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '14'
WHERE Type = 'WORLDSIZE_TINY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '12'
WHERE Type = 'WORLDSIZE_SMALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '11'
WHERE Type = 'WORLDSIZE_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '10'
WHERE Type = 'WORLDSIZE_LARGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesPolicyCostMod = '9'
WHERE Type = 'WORLDSIZE_HUGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );


UPDATE Worlds
SET NumCitiesTechCostMod = '14'
WHERE Type = 'WORLDSIZE_DUEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '12'
WHERE Type = 'WORLDSIZE_TINY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '10'
WHERE Type = 'WORLDSIZE_SMALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '8'
WHERE Type = 'WORLDSIZE_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '7'
WHERE Type = 'WORLDSIZE_LARGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Worlds
SET NumCitiesTechCostMod = '6'
WHERE Type = 'WORLDSIZE_HUGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

-- Goody Hut Payout

UPDATE GoodyHuts
SET GoldRandAmount = '20'
WHERE Type = 'GOODY_GOLD';

UPDATE GoodyHuts
SET GoldRandAmount = '20'
WHERE Type = 'GOODY_LOW_GOLD';

UPDATE GoodyHuts
SET GoldRandAmount = '20'
WHERE Type = 'GOODY_HIGH_GOLD';

UPDATE GoodyHuts
SET Experience = '20'
WHERE Type = 'GOODY_EXPERIENCE';

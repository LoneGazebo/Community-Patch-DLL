DELETE FROM HandicapInfos;

CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM HandicapInfos ORDER BY ID;
UPDATE HandicapInfos SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE HandicapInfos.Type = IDRemapper.Type);
DROP TABLE IDRemapper;

UPDATE sqlite_sequence
SET seq = (SELECT COUNT(ID) FROM HandicapInfos)-1
WHERE name = 'HandicapInfos';

-- Player Bonuses
ALTER TABLE HandicapInfos ADD COLUMN 'MapPlacementPriority' INTEGER DEFAULT 1;
ALTER TABLE HandicapInfos ADD COLUMN 'StartingGold' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HappinessDefaultCapital' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'EmpireSizeUnhappinessMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'MaintenanceFreeUnits' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitSupplyBase' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitSupplyPerCity' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitSupplyPopulationPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitSupplyPerEraFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitSupplyPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitSupplyBonusPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'StartingUnitMultiplier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'WorkRateModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitUpgradePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'UnitUpgradePerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'GrowthPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'GrowthPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'ResearchPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'TechCatchUpMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'PolicyPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'PolicyCatchUpMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'ProphetPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'GreatPeoplePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'GoldenAgePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CivilianPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CivilianPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'TrainPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'TrainPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'WorldTrainPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'ConstructPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'ConstructPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'WorldConstructPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CreatePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CreatePerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'WorldCreatePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'ProcessBonus' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'ProcessPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'FreeXP' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'FreeXPPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CombatBonus' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'ResistanceCap' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'VisionBonus' INTEGER DEFAULT 0;
-- VP Difficulty Bonus
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusTurnInterval' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusBase' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusA' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusB' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusC' INTEGER DEFAULT 0;

-- AI Bonuses
ALTER TABLE HandicapInfos ADD COLUMN 'AIStartingGold' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIStartingPolicyPoints' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIAdvancedStartPointsMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIHappinessDefault' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIHappinessDefaultCapital' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIExtraHappinessPerLuxury' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIEmpireSizeUnhappinessMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIPopulationUnhappinessMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIFreeCulturePerTurn' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIMaintenanceFreeUnits' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitSupplyBase' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitSupplyPerCity' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitSupplyPopulationPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitSupplyPerEraFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitSupplyPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitSupplyBonusPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIImprovementCostPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AIUnitUpgradePerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIGrowthPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIResearchPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AIResearchPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AITechCatchUpMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIPolicyPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AIPolicyPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIPolicyCatchUpMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIProphetPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AIGreatPeoplePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AIGoldenAgePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AICivilianPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'AICivilianPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AITrainPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIConstructPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AICreatePerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIProcessBonus' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIProcessPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AICombatBonus' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIResistanceCap' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIVisionBonus' INTEGER DEFAULT 0;
-- VP Difficulty Bonus
ALTER TABLE HandicapInfos ADD COLUMN 'AIDifficultyBonusTurnInterval' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIDifficultyBonusBase' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIDifficultyBonusA' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIDifficultyBonusB' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIDifficultyBonusC' INTEGER DEFAULT 0;

-- City-States
ALTER TABLE HandicapInfos ADD COLUMN 'StartingCityStateWorkerUnits' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'StartingCityStateDefenseUnits' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateUnitSupplyBase' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateUnitSupplyPerCity' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateUnitSupplyPopulationPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateUnitSupplyPerEraFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateUnitSupplyPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateUnitSupplyBonusPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateWorkRateModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateGrowthPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateGrowthPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateCivilianPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateCivilianPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateTrainPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateTrainPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateConstructPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateConstructPerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateCreatePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateCreatePerEraModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateFreeXP' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateFreeXPPercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateCombatBonus' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'CityStateVisionBonus' INTEGER DEFAULT 0;

-- Barbarians
ALTER TABLE HandicapInfos ADD COLUMN 'BonusVSBarbarians' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIBonusVSBarbarians' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'BarbarianCampGold' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIBarbarianCampGold' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'BarbarianSpawnDelay' INTEGER DEFAULT 0;

-- AI Behavior Modifiers
-- Weighted Randomized Choices
ALTER TABLE HandicapInfos ADD COLUMN 'CityProductionChoiceCutoffThreshold' INTEGER DEFAULT 90;
ALTER TABLE HandicapInfos ADD COLUMN 'TechChoiceCutoffThreshold' INTEGER DEFAULT 90;
ALTER TABLE HandicapInfos ADD COLUMN 'PolicyChoiceCutoffThreshold' INTEGER DEFAULT 90;
ALTER TABLE HandicapInfos ADD COLUMN 'BeliefChoiceCutoffThreshold' INTEGER DEFAULT 90;
-- Tactical AI
ALTER TABLE HandicapInfos ADD COLUMN 'TacticalSimMaxCompletedPositions' INTEGER DEFAULT 640;
ALTER TABLE HandicapInfos ADD COLUMN 'TacticalSimMaxBranches' INTEGER DEFAULT 4;
ALTER TABLE HandicapInfos ADD COLUMN 'TacticalSimMaxChoicesPerUnit' INTEGER DEFAULT 4;
-- Diplomacy AI
ALTER TABLE HandicapInfos ADD COLUMN 'LandDisputePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'WonderDisputePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'MinorCivDisputePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'VictoryDisputePercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'VictoryDisputeMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'VictoryBlockPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'VictoryBlockMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'WonderBlockPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'WonderBlockMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'TechBlockPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'TechBlockMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'PolicyBlockPercent' INTEGER DEFAULT 100;
ALTER TABLE HandicapInfos ADD COLUMN 'PolicyBlockMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'PeaceTreatyDampenerTurns' INTEGER DEFAULT 20;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanStrengthPerceptionMod' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanTradeModifier' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AggressionIncrease' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanOpinionChange' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanWarApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanWarApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanHostileApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanHostileApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanDeceptiveApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanDeceptiveApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanGuardedApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanGuardedApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanAfraidApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanAfraidApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanNeutralApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanNeutralApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanFriendlyApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'HumanFriendlyApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIOpinionChange' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIWarApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIWarApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIHostileApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIHostileApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIDeceptiveApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIDeceptiveApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIGuardedApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIGuardedApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIAfraidApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIAfraidApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AINeutralApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AINeutralApproachChangePercent' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIFriendlyApproachChangeFlat' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'AIFriendlyApproachChangePercent' INTEGER DEFAULT 0;

-- Add support for new rewards from Ancient Ruins
INSERT INTO GoodyHuts
	(Type, Description, ChooseDescription, Sound, Production, GoldenAge, FreeTiles, Science)
VALUES
	('GOODY_PRODUCTION', 'TXT_KEY_GOODY_PRODUCTION', 'TXT_KEY_GOODY_CHOOSE_PRODUCTION', 'AS2D_GOODY_WARRIOR', 30, 0, 0, 0),
	('GOODY_GOLDEN_AGE', 'TXT_KEY_GOODY_GOLDEN_AGE', 'TXT_KEY_GOODY_CHOOSE_GOLDEN_AGE', 'AS2D_GOODY_WARRIOR', 0, 200, 0, 0),
	('GOODY_TILES', 'TXT_KEY_GOODY_TILES', 'TXT_KEY_GOODY_CHOOSE_FREE_TILES', 'AS2D_GOODY_WARRIOR', 0, 0, 4, 0);
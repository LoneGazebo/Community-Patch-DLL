----------------------------------------------------------
-- New city names
----------------------------------------------------------
INSERT INTO Civilization_CityNames
	(CivilizationType, CityName)
VALUES
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_CATTARO'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_CORFU'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_CORON'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_CANDIA'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_CHIOGGIA'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_MODON'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_ERACLEA'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_AQUILEA'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_SPALATO'),
	('CIVILIZATION_VENICE', 'TXT_KEY_CITY_NAME_ZADAR');

----------------------------------------------------------
-- Unique Ability: Serenissima
----------------------------------------------------------
UPDATE Traits
SET
	FreeUnitPrereqTech = 'TECH_HORSEBACK_RIDING', -- Trade
	PuppetYieldAndSupplyModifierChange = 20
WHERE Type = 'TRAIT_SUPER_CITY_STATE';

INSERT INTO Trait_NoTrain
	(TraitType, UnitClassType)
VALUES
	('TRAIT_SUPER_CITY_STATE', 'UNITCLASS_PIONEER'),
	('TRAIT_SUPER_CITY_STATE', 'UNITCLASS_COLONIST');

----------------------------------------------------------
-- Unique Unit: Merchant of Venice (Great Merchant)
----------------------------------------------------------
UPDATE Units
SET
	Help = 'TXT_KEY_UNIT_HELP_VENETIAN_MERCHANT',
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_MERCHANT') + 2,
	Found = 1,
	FoundColony = 9999,
	CanBuyCityState = 1
WHERE Type = 'UNIT_VENETIAN_MERCHANT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_VENETIAN_MERCHANT', 'PROMOTION_TRADE_MISSION_BONUS');

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
VALUES
	('UNIT_VENETIAN_MERCHANT', 'BUILDINGCLASS_MONUMENT'),
	('UNIT_VENETIAN_MERCHANT', 'BUILDINGCLASS_MARKET');

----------------------------------------------------------
-- Unique Unit: Great Galleass (Galleass)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_GUNPOWDER',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_GALLEASS'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_GALLEASS') + 6,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_GALLEASS') + 6
WHERE Type = 'UNIT_VENETIAN_GALLEASS';

UPDATE Unit_ClassUpgrades
SET UnitClassType = 'UNITCLASS_CRUISER'
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_VENETIAN_GALLEASS', 'PROMOTION_BOMBARDMENT_1');

INSERT INTO Unit_UniqueNames
	(UnitType, UniqueName)
VALUES
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_01'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_02'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_03'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_04'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_05'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_06'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_07'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_08'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_09'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_10'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_11'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_12'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_13'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_14'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_15'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_16'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_17'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_18'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_19'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_20'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_21'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_22'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_23'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_24'),
	('UNIT_VENETIAN_GALLEASS', 'TXT_KEY_UNIT_GREAT_GALLEASS_25');

----------------------------------------------------------
-- Unique Unit: Fusta
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_VENICE', 'UNITCLASS_FUSTA', 'UNIT_FUSTA');

UPDATE Units
SET
	Combat = 14,
	RangedCombat = 21,
	"Range" = 1,
	Moves = 5
WHERE Type = 'UNIT_FUSTA';

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_FUSTA', 'UNITCLASS_FRIGATE');

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_FUSTA', 'PROMOTION_SHALLOW_DRAFT'),
	('UNIT_FUSTA', 'PROMOTION_NAVAL_INACCURACY'),
	('UNIT_FUSTA', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_FUSTA', 'UNITAI_ASSAULT_SEA'),
	('UNIT_FUSTA', 'UNITAI_RESERVE_SEA'),
	('UNIT_FUSTA', 'UNITAI_ESCORT_SEA');

----------------------------------------------------------
-- Unique Building: Piazza San Marco (National Monument)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_VENICE', 'BUILDINGCLASS_NATIONAL_EPIC', 'BUILDING_PIAZZA_SAN_MARCO');

-- No Monument requirement, but requires to be built in the capital
UPDATE Building_ClassesNeededInCity
SET BuildingClassType = 'BUILDINGCLASS_PALACE'
WHERE BuildingType = 'BUILDING_PIAZZA_SAN_MARCO';

UPDATE Buildings
SET
	PrereqTech = 'TECH_WRITING',
	GreatPeopleRateModifier = 33,
	CitySupplyFlat = 5,
	PurchaseCooldownReduction = 1,
	PurchaseCooldownReductionCivilian = 3,
	SpySecurityModifier = 10
WHERE Type = 'BUILDING_PIAZZA_SAN_MARCO';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_FOOD', 2),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_PRODUCTION', 2),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_GOLD', 2),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_SCIENCE', 1);

----------------------------------------------------------
-- Extra Buildings:
-- Arsenale di Venezia
-- Rialto District
-- Murano Glassworks
----------------------------------------------------------
INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_ARSENALE_DI_VENEZIA', 'BUILDINGCLASS_NATIONAL_EPIC'),
	('BUILDING_RIALTO_DISTRICT', 'BUILDINGCLASS_NATIONAL_EPIC'),
	('BUILDING_MURANO_GLASSWORKS', 'BUILDINGCLASS_NATIONAL_EPIC');

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ARSENALE_DI_VENEZIA', 'YIELD_PRODUCTION', 15);

INSERT INTO Building_YieldFromInternalTR
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ARSENALE_DI_VENEZIA', 'YIELD_PRODUCTION', 4);

INSERT INTO Building_YieldChangesPerPopInEmpire
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_RIALTO_DISTRICT', 'YIELD_GOLD', 34);

INSERT INTO Building_HurryModifiers
	(BuildingType, HurryType, HurryCostModifier)
VALUES
	('BUILDING_RIALTO_DISTRICT', 'HURRY_GOLD', -10);

INSERT INTO Building_HurryModifiersLocal
	(BuildingType, HurryType, HurryCostModifier)
VALUES
	('BUILDING_RIALTO_DISTRICT', 'HURRY_GOLD', -5);

CREATE TEMP TABLE Helper (
	YieldType TEXT
);

INSERT INTO Helper
VALUES
	('YIELD_FOOD'),
	('YIELD_TOURISM');

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	'BUILDING_MURANO_GLASSWORKS', a.Type, b.YieldType, 2
FROM Improvements a, Helper b
WHERE a.CreatedByGreatPerson = 1 OR a.Type = 'IMPROVEMENT_TRADING_POST';

DROP TABLE Helper;

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresOwner)
VALUES
	('BUILDING_MURANO_GLASSWORKS', 'TXT_KEY_THEMING_BONUS_MURANO_GLASSWORKS', 1);

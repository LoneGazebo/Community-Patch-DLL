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
	ReducePuppetPenalties = 30
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
	('BUILDING_VENETIAN_ARSENALE', 'BUILDINGCLASS_NATIONAL_EPIC'),
	('BUILDING_RIALTO_DISTRICT', 'BUILDINGCLASS_NATIONAL_EPIC'),
	('BUILDING_MURANO_GLASSWORKS', 'BUILDINGCLASS_NATIONAL_EPIC');

INSERT INTO Building_YieldModifiers
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_VENETIAN_ARSENALE', 'YIELD_PRODUCTION', 15);

INSERT INTO Building_YieldFromInternalTR
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_VENETIAN_ARSENALE', 'YIELD_PRODUCTION', 4);

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

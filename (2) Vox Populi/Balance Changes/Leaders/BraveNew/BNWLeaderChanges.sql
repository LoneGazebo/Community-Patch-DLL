-- Ahmad -- Boost Kasbah, Ability
DELETE FROM Trait_YieldChangesPerTradePartner
WHERE TraitType = 'TRAIT_GATEWAY_AFRICA';

DELETE FROM Trait_YieldChangesIncomingTradeRoute
WHERE TraitType = 'TRAIT_GATEWAY_AFRICA';

INSERT INTO Trait_YieldChangesPerTradePartner
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_GATEWAY_AFRICA', 'YIELD_GOLD', 1),
	('TRAIT_GATEWAY_AFRICA', 'YIELD_CULTURE', 1),
	('TRAIT_GATEWAY_AFRICA', 'YIELD_SCIENCE', 1),
	('TRAIT_GATEWAY_AFRICA', 'YIELD_FAITH', 1),
	('TRAIT_GATEWAY_AFRICA', 'YIELD_FOOD', 1),
	('TRAIT_GATEWAY_AFRICA', 'YIELD_PRODUCTION', 1),
	('TRAIT_GATEWAY_AFRICA', 'YIELD_GOLDEN_AGE_POINTS', 1);

UPDATE Improvement_Yields
SET Yield = '2'
WHERE YieldType = 'YIELD_GOLD' AND ImprovementType = 'IMPROVEMENT_KASBAH';

UPDATE Improvement_Yields
SET Yield = '1'
WHERE YieldType = 'YIELD_FOOD' AND ImprovementType = 'IMPROVEMENT_KASBAH';

UPDATE Improvement_Yields
SET Yield = '2'
WHERE YieldType = 'YIELD_PRODUCTION' AND ImprovementType = 'IMPROVEMENT_KASBAH';

UPDATE Improvements
SET Cityside = '1'
WHERE Type = 'IMPROVEMENT_KASBAH';

UPDATE Improvements
SET DefenseModifier = '30'
WHERE Type = 'IMPROVEMENT_KASBAH';

UPDATE Improvements
SET BuildableOnResources = '1'
WHERE Type = 'IMPROVEMENT_KASBAH';

UPDATE Improvements
SET NearbyEnemyDamage = '5'
WHERE Type = 'IMPROVEMENT_KASBAH';

UPDATE Builds
SET PrereqTech = 'TECH_CHIVALRY'
WHERE Type = 'BUILD_KASBAH';

UPDATE Builds
SET Time = '1000'
WHERE Type = 'BUILD_KASBAH';

UPDATE Traits
SET IgnoreTradeDistanceScaling = 'true'
WHERE Type = 'TRAIT_GATEWAY_AFRICA';

UPDATE Traits
SET CanPlunderWithoutWar = 'true'
WHERE Type = 'TRAIT_GATEWAY_AFRICA';


-- Ashurbanipal -- Boost Royal Library
UPDATE Buildings
SET GreatWorkCount = '3'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET NoUnhappfromXSpecialists = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET FreeGreatWork = 'GREAT_WORK_FLOOD_TABLET'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'YIELD_SCIENCE', 20);

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET GoldMaintenance = '0'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET GreatPeopleRateChange = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET NumCityCostMod = '10'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Building_YieldChangesPerPop
SET Yield = '50'
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY';

DELETE FROM Building_DomainFreeExperiencePerGreatWork
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET ThemingBonusHelp = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_THEMING_BONUS_HELP'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET Cost = '125'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET NationalPopRequired = '15'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET BuildingClass = 'BUILDINGCLASS_NATIONAL_COLLEGE'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_NATIONAL_COLLEGE'
WHERE CivilizationType = 'CIVILIZATION_ASSYRIA';

UPDATE Buildings
SET FreeBuildingThisCity = 'BUILDINGCLASS_LIBRARY'
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

-- Enrico - Unique Building (Piazza) -- SEE XML
UPDATE Traits
SET FreeUnitPrereqTech = 'TECH_HORSEBACK_RIDING'
WHERE Type = 'TRAIT_SUPER_CITY_STATE';

UPDATE Traits
SET ReducePuppetPenalties = '30'
WHERE Type = 'TRAIT_SUPER_CITY_STATE';

UPDATE Units
SET Found = '1', FoundColony = '99', Moves = '4', BaseGold = '0', NumGoldPerEra = '0', BaseWLTKDTurns = '5', BaseGoldTurnsToCount = '5'
WHERE Type = 'UNIT_VENETIAN_MERCHANT';

-- Gajah Mada -- Boost Candi, No Isolation Unhappiness

UPDATE Buildings
SET GrantsRandomResourceTerritory = '1'
WHERE Type = 'BUILDING_CANDI';

DELETE FROM Building_YieldChangesPerReligion
WHERE BuildingType = 'BUILDING_CANDI';

UPDATE Traits
SET UniqueLuxuryRequiresNewArea = '0'
WHERE Type = 'TRAIT_SPICE';

UPDATE Traits
SET UniqueLuxuryQuantity = '1'
WHERE Type = 'TRAIT_SPICE';

UPDATE Traits
SET UniqueLuxuryCities = '100'
WHERE Type = 'TRAIT_SPICE';

UPDATE Traits 
SET MonopolyModFlat = 2 WHERE 
Type = 'TRAIT_SPICE';

UPDATE Traits 
SET MonopolyModPercent = 5 
WHERE Type = 'TRAIT_SPICE';

UPDATE Buildings
SET NoUnhappfromXSpecialists = '1'
WHERE Type = 'BUILDING_CANDI';

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'FEATURE_OASIS', 'YIELD_GOLD', 2);

-- Maria I

UPDATE Builds
SET PrereqTech = 'TECH_COMPASS'
WHERE Type = 'BUILD_FEITORIA';

UPDATE Improvements
SET GrantsVisionXTiles = '2'
WHERE Type = 'IMPROVEMENT_FEITORIA';

UPDATE Improvements
SET OnlyCityStateTerritory = '0'
WHERE Type = 'IMPROVEMENT_FEITORIA';

UPDATE Improvements
SET NoTwoAdjacent = '1'
WHERE Type = 'IMPROVEMENT_FEITORIA';

UPDATE Improvements
SET DefenseModifier = '25'
WHERE Type = 'IMPROVEMENT_FEITORIA';

UPDATE Traits
SET TradeRouteResourceModifier = '0'
WHERE Type = 'TRAIT_EXTRA_TRADE';


-- Shoshone


-- Casimir III -- Ducal Stable +1 Production
UPDATE Buildings
SET PrereqTech = 'TECH_CHIVALRY'
WHERE Type = 'BUILDING_DUCAL_STABLE';

UPDATE Buildings
SET CitySupplyModifier = '10'
WHERE Type = 'BUILDING_DUCAL_STABLE';

UPDATE Building_YieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

UPDATE Building_ResourceYieldChanges
SET Yield = '3'
WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = '50'
WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_DUCAL_STABLE', 'RESOURCE_HORSE', 1);

INSERT INTO Building_InstantYield
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_DUCAL_STABLE', 'YIELD_CULTURE', 100);

DELETE FROM Building_LocalResourceOrs
WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

UPDATE Traits
SET FreeSocialPoliciesPerEra = '1'
WHERE Type = 'TRAIT_SOLIDARITY';

UPDATE Traits
SET ExtraTenetsFirstAdoption = '2'
WHERE Type = 'TRAIT_SOLIDARITY';

UPDATE Traits
SET IsOddEraScaler = 'true'
WHERE Type = 'TRAIT_SOLIDARITY';

-- Pedro -- Improve Brazilwood Camp, increase all GP production during GA

UPDATE Traits
SET GoldenAgeTourismModifier = '0'
WHERE Type = 'TRAIT_CARNIVAL';

UPDATE Traits
SET GoldenAgeGreatArtistRateModifier = '0'
WHERE Type = 'TRAIT_CARNIVAL';

UPDATE Traits
SET GoldenAgeGreatMusicianRateModifier = '0'
WHERE Type = 'TRAIT_CARNIVAL';

UPDATE Traits
SET GoldenAgeGreatWriterRateModifier = '0'
WHERE Type = 'TRAIT_CARNIVAL';

UPDATE Traits
SET WLTKDFromGATurns = '10'
WHERE Type = 'TRAIT_CARNIVAL';

UPDATE Traits
SET WLTKDUnhappinessNeedsMod = '-50'
WHERE Type = 'TRAIT_CARNIVAL';

UPDATE Builds
SET PrereqTech = 'TECH_CALENDAR'
WHERE Type = 'BUILD_BRAZILWOOD_CAMP';

UPDATE Improvements
SET NoTwoAdjacent = '1'
WHERE Type = 'IMPROVEMENT_BRAZILWOOD_CAMP';

UPDATE Improvement_Yields
SET Yield = '1'
WHERE ImprovementType = 'IMPROVEMENT_BRAZILWOOD_CAMP';

UPDATE Improvements
SET ImprovementResource = 'RESOURCE_BRAZILWOOD'
WHERE Type = 'IMPROVEMENT_BRAZILWOOD_CAMP';

UPDATE Improvements
SET ImprovementResourceQuantity = '1'
WHERE Type = 'IMPROVEMENT_BRAZILWOOD_CAMP';

UPDATE Improvements
SET NoFreshWater = '1'
WHERE Type = 'IMPROVEMENT_BRAZILWOOD_CAMP';

INSERT INTO Improvement_ValidFeatures
	(ImprovementType, FeatureType)
VALUES
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'FEATURE_FOREST');

INSERT INTO Improvement_ResourceTypes
	(ImprovementType, ResourceType, ResourceMakesValid, ResourceTrade, DiscoveryRand, QuantityRequirement)
VALUES
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'RESOURCE_BRAZILWOOD', '1', '1', '0', '0');

INSERT INTO Units
	(Class, Type, Combat, Cost, FaithCost, WorkRate, RequiresFaithPurchaseEnabled, Moves, PrereqTech, ObsoleteTech, CombatClass, Domain, NoBadGoodies, DefaultUnitAI, Description, Civilopedia, Strategy, Help, Pillage, MilitarySupport, GoodyHutUpgradeUnitClass, MilitaryProduction, AdvancedStartCost, XPValueAttack, XPValueDefense, UnitArtInfo, UnitFlagAtlas, UnitFlagIconOffset, PortraitIndex, IconAtlas, MoveRate, BaseSightRange)
VALUES
	('UNITCLASS_EXPLORER', 'UNIT_BANDEIRANTES', 19, 180, 200, 100, 1, 4, 'TECH_COMPASS', 'TECH_ROCKETRY', 'UNITCOMBAT_RECON', 'DOMAIN_LAND', 1, 'UNITAI_EXPLORE', 'TXT_KEY_UNIT_BANDEIRANTES', 'TXT_KEY_UNIT_BANDEIRANTES_TEXT', 'TXT_KEY_UNIT_BANDEIRANTES_STRATEGY', 'TXT_KEY_UNIT_BANDEIRANTES_HELP', 1, 1, 'UNITCLASS_COMMANDO', 1, 30, 3, 3, 'ART_DEF_UNIT_BANDEIRANTES', 'BANDEIRANTES_FLAG_ATLAS', 0, 4, 'COMMUNITY_2_ATLAS', 'BIPED', 3);

INSERT INTO UnitGameplay2DScripts
	(UnitType, SelectionSound, FirstSelectionSound)
VALUES
	('UNIT_BARBARIAN_HORSEMAN', 'AS2D_SELECT_HORSEMAN', 'AS2D_BIRTH_HORSEMAN'),
	('UNIT_BANDEIRANTES', 'AS2D_SELECT_SCOUT', 'AS2D_BIRTH_SCOUT');

-- Zulu -- Ikanda gives promotions to all melee infantry

UPDATE Buildings
SET CitySupplyFlat = '2'
WHERE Type = 'BUILDING_IKANDA';

UPDATE Buildings
SET TrainedFreePromotion = 'PROMOTION_IKLWA'
WHERE Type = 'BUILDING_IKANDA';

INSERT INTO Trait_MaintenanceModifierUnitCombats
	(TraitType, UnitCombatType, MaintenanceModifier)
VALUES
	('TRAIT_BUFFALO_HORNS', 'UNITCOMBAT_GUN', -50);

UPDATE Traits
SET CSBullyMilitaryStrengthModifier = '50'
WHERE Type = 'TRAIT_BUFFALO_HORNS';

UPDATE Traits
SET CSBullyValueModifier = '0'
WHERE Type = 'TRAIT_BUFFALO_HORNS';

UPDATE Traits 
SET IgnoreBullyPenalties = 1 
WHERE Type = 'TRAIT_BUFFALO_HORNS';

-- New BNW Leader Data and Yields

-- New Improvement

INSERT INTO Improvements
	(Type, Description, Civilopedia, Help, ArtDefineTag, SpecificCivRequired, CivilizationType, RequiresFlatlands, NoTwoAdjacent, PortraitIndex, PillageGold, DefenseModifier, NearbyEnemyDamage, IconAtlas)
VALUES
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TXT_KEY_IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TXT_KEY_CIV5_IMPROVEMENTS_ENCAMPMENT_SHOSHONE_TEXT', 'TXT_KEY_CIV5_IMPROVEMENTS_ENCAMPMENT_SHOSHONE_HELP', 'ART_DEF_IMPROVEMENT_TIPIS', 1, 'CIVILIZATION_SHOSHONE', 1, 1, 0, 20, 15, 5, 'TERRAIN_IMPROVEMENT_ENCAMPMENT');

INSERT INTO Builds
	(Type, Time, ImprovementType, PrereqTech, Description, Help, Recommendation, EntityEvent, HotKey, OrderPriority, IconIndex, IconAtlas)
VALUES
	('BUILD_ENCAMPMENT_SHOSHONE', 700, 'IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_HORSEBACK_RIDING', 'TXT_KEY_BUILD_ENCAMPMENT_SHOSHONE', 'TXT_KEY_BUILD_ENCAMPMENT_SHOSHONE_HELP', 'TXT_KEY_BUILD_ENCAMPMENT_SHOSHONE_REC', 'ENTITY_EVENT_BUILD', 'KB_E', 1, 0, 'UNIT_ACTION_ENCAMPMENT');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KASBAH', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_FOOD', 2),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_CULTURE', 1);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'YIELD_FAITH', 15),
	('BUILDING_CANDI', 'YIELD_CULTURE', 15);

-- all terrains are valid for these improvements
DELETE FROM Improvement_ValidTerrains
WHERE ImprovementType IN ('IMPROVEMENT_KASBAH', 'IMPROVEMENT_ENCAMPMENT_SHOSHONE');

INSERT INTO Building_DomainFreeExperiencePerGreatWorkGlobal
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'DOMAIN_LAND', 5),
	('BUILDING_ROYAL_LIBRARY', 'DOMAIN_SEA', 5),
	('BUILDING_ROYAL_LIBRARY', 'DOMAIN_AIR', 5);

INSERT INTO Trait_GreatWorkYieldChanges
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_SLAYER_OF_TIAMAT', 'YIELD_SCIENCE', 3);

INSERT INTO Trait_GAPToYield
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CARNIVAL', 'YIELD_TOURISM', 30),
	('TRAIT_CARNIVAL', 'YIELD_GOLD', 30);

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'YIELD_SCIENCE', 5),
	('BUILDING_ROYAL_LIBRARY', 'YIELD_CULTURE', 3),
	('BUILDING_DUCAL_STABLE', 'YIELD_PRODUCTION', 3),
	('BUILDING_IKANDA', 'YIELD_SCIENCE', 1),
	('BUILDING_IKANDA', 'YIELD_CULTURE', 1),
	('BUILDING_CANDI', 'YIELD_CULTURE', 3),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_CULTURE', 1),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_GOLD', 2),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_SCIENCE', 1),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_PRODUCTION', 2),
	('BUILDING_PIAZZA_SAN_MARCO', 'YIELD_FOOD', 2);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, Bonus, RequiresAnyButOwner, AIPriority)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'TXT_KEY_THEMING_BONUS_ROYAL_LIBRARY', 6, 1, 2);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'BUILDINGCLASS_LIBRARY', 'YIELD_SCIENCE', 3);

INSERT INTO Building_SpecialistYieldChangesLocal
		(BuildingType, 		SpecialistType, 		YieldType, 			Yield)
VALUES	('BUILDING_CANDI', 'SPECIALIST_ARTIST', 	'YIELD_TOURISM', 	1),
		('BUILDING_CANDI', 'SPECIALIST_MUSICIAN', 	'YIELD_TOURISM', 	1),
		('BUILDING_CANDI', 'SPECIALIST_WRITER', 	'YIELD_TOURISM', 	1);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'RESOURCE_CLOVES', 'YIELD_FAITH', 1),
	('BUILDING_CANDI', 'RESOURCE_CLOVES', 'YIELD_CULTURE', 1),
	('BUILDING_CANDI', 'RESOURCE_PEPPER', 'YIELD_FAITH', 1),
	('BUILDING_CANDI', 'RESOURCE_PEPPER', 'YIELD_GOLD', 1),
	('BUILDING_CANDI', 'RESOURCE_NUTMEG', 'YIELD_PRODUCTION', 1),
	('BUILDING_CANDI', 'RESOURCE_NUTMEG', 'YIELD_CULTURE', 1),
	('BUILDING_CANDI', 'RESOURCE_CITRUS', 'YIELD_GOLD', 1),
	('BUILDING_CANDI', 'RESOURCE_CITRUS', 'YIELD_FOOD', 1),
	('BUILDING_CANDI', 'RESOURCE_COCOA', 'YIELD_GOLD', 1),
	('BUILDING_CANDI', 'RESOURCE_COCOA', 'YIELD_FOOD', 1);

INSERT INTO Trait_YieldFromRouteMovement
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_EXTRA_TRADE', 'YIELD_GREAT_ADMIRAL_POINTS', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_GREAT_GENERAL_POINTS', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_SCIENCE', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_GOLD', 4);

INSERT INTO UnitPromotions_UnitCombats
	(PromotionType, UnitCombatType)
VALUES
	('PROMOTION_BUFFALO_HORNS', 'UNITCOMBAT_GUN'),
	('PROMOTION_BUFFALO_CHEST', 'UNITCOMBAT_GUN'),
	('PROMOTION_BUFFALO_LOINS', 'UNITCOMBAT_GUN');

INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_VENICE', 'BUILDINGCLASS_NATIONAL_EPIC', 'BUILDING_PIAZZA_SAN_MARCO');

INSERT INTO Building_ClassesNeededInCity
	(BuildingType, BuildingClassType)
VALUES
	('BUILDING_PIAZZA_SAN_MARCO', 'BUILDINGCLASS_PALACE'),
	('BUILDING_RIALTO_DISTRICT', 'BUILDINGCLASS_NATIONAL_EPIC'),
	('BUILDING_VENETIAN_ARSENALE', 'BUILDINGCLASS_NATIONAL_EPIC'),
	('BUILDING_MURANO_GLASSWORKS', 'BUILDINGCLASS_NATIONAL_EPIC');

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_ACADEMY', 'YIELD_TOURISM', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_TOURISM', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_MANUFACTORY', 'YIELD_TOURISM', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_CITADEL', 'YIELD_TOURISM', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_TOURISM', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_ACADEMY', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_MANUFACTORY', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_CITADEL', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_TRADING_POST', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_TRADING_POST', 'YIELD_TOURISM', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_HOLY_SITE', 'YIELD_FOOD', 2),
	('BUILDING_MURANO_GLASSWORKS', 'IMPROVEMENT_HOLY_SITE', 'YIELD_TOURISM', 2);

INSERT INTO Building_HurryModifiers
	(BuildingType, HurryType, HurryCostModifier)
VALUES
	('BUILDING_RIALTO_DISTRICT', 'HURRY_GOLD', -10);

INSERT INTO Building_HurryModifiersLocal
	(BuildingType, HurryType, HurryCostModifier)
VALUES
	('BUILDING_RIALTO_DISTRICT', 'HURRY_GOLD', -5);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, Bonus, RequiresOwner, AIPriority)
VALUES
	('BUILDING_MURANO_GLASSWORKS', 'TXT_KEY_THEMING_BONUS_MURANO_GLASSWORKS', 8, 1, 4);

INSERT INTO Building_ThemingYieldBonus
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MURANO_GLASSWORKS', 'YIELD_SCIENCE', 5);

INSERT INTO BuildFeatures
	(BuildType, FeatureType, PrereqTech, Time, Production, Remove)
VALUES -- GitHub #4097 fix
	('BUILD_ENCAMPMENT_SHOSHONE', 'FEATURE_JUNGLE', 'TECH_CALENDAR', 400, 20, 1),
	('BUILD_ENCAMPMENT_SHOSHONE', 'FEATURE_FOREST', 'TECH_MINING', 300, 30, 1),
	('BUILD_ENCAMPMENT_SHOSHONE', 'FEATURE_MARSH', 'TECH_IRON_WORKING', 600, 0, 1);

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_ENCAMPMENT_SHOSHONE');

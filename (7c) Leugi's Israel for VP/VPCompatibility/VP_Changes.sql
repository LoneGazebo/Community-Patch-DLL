--------------------------------	
-- Civilization_BuildingClassOverrides
--------------------------------
INSERT INTO	Civilization_BuildingClassOverrides
			(CivilizationType,				BuildingClassType,				BuildingType)
VALUES		('CIVILIZATION_LEUGI_ISRAEL', 	'BUILDINGCLASS_GRAND_TEMPLE', 	'BUILDING_LEUGI_JERUSALEM_TEMPLE');
--==========================================================================================================================
-- TRAITS
--==========================================================================================================================
--------------------------------	
-- Trait_GreatWorkYieldChanges
--------------------------------
INSERT INTO Trait_GreatWorkYieldChanges
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CHOSEN_PEOPLE', 'YIELD_FAITH', 2),
	('TRAIT_CHOSEN_PEOPLE', 'YIELD_CULTURE', 2);
--------------------------------	
-- Trait_ImprovementYieldChanges
--------------------------------
INSERT INTO Trait_ImprovementYieldChanges
		(TraitType, 			ImprovementType,	YieldType,			Yield)
SELECT	'TRAIT_CHOSEN_PEOPLE', 	Type, 				'YIELD_CULTURE', 	2
FROM Improvements WHERE CreatedByGreatPerson = 1;

INSERT INTO Trait_ImprovementYieldChanges
		(TraitType, 			ImprovementType,	YieldType,			Yield)
SELECT	'TRAIT_CHOSEN_PEOPLE', 	Type, 				'YIELD_FAITH', 		2
FROM Improvements WHERE CreatedByGreatPerson = 1;
--------------------------------	
-- Trait_YieldFromBarbarianCampClears
--------------------------------
INSERT INTO Trait_YieldFromBarbarianCampClears
		(TraitType, 			YieldType,		Yield, 	IsEraScaling)
VALUES	('TRAIT_CHOSEN_PEOPLE', 'YIELD_FAITH',	40, 	1);
--------------------------------	
-- Trait_FreePromotionUnitCombats
--------------------------------
INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_RECON', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_ARCHER', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_MELEE', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_SIEGE', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_GUN', 			'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_HELICOPTER', 	'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_NAVALMELEE', 	'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_NAVALRANGED', 	'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_FIGHTER', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_BOMBER', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_MOUNTED', 		'PROMOTION_PROMISED_LAND'),
	('TRAIT_CHOSEN_PEOPLE', 'UNITCOMBAT_ARMOR', 		'PROMOTION_PROMISED_LAND');
--==========================================================================================================================
-- BUILDINGS
--==========================================================================================================================
-----------------------------
--BUILDINGS
-----------------------------
INSERT INTO	Buildings
			(Type,								Description,								Civilopedia,									Strategy,											Help,											GoldMaintenance, Cost, HurryCostModifier, MinAreaSize, ConquestProb, BuildingClass, PrereqTech, 		PortraitIndex, 	IconAtlas,					GlobalConversionModifier, 	ReligiousPressureModifier, 	GreatWorkYieldType,	GoldenAge,	NeverCapture, NukeImmune, NumCityCostMod, GreatWorkSlotType, 		ThemingBonusHelp, 								GreatWorkCount,	NationalPopRequired, 	MinorityHappinessChange, 	MinorityHappinessChangeGlobal)
SELECT		'BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'TXT_KEY_BUILDING_LEUGI_JERUSALEM_TEMPLE', 'TXT_KEY_BUILDING_LEUGI_JERUSALEM_TEMPLE_TEXT',	'TXT_KEY_BUILDING_LEUGI_JERUSALEM_TEMPLE_STRATEGY',	'TXT_KEY_BUILDING_LEUGI_JERUSALEM_TEMPLE_HELP', GoldMaintenance, Cost, HurryCostModifier, MinAreaSize, ConquestProb, BuildingClass, 'TECH_MASONRY', 	0,				'JERUSALEM_TEMPLE_ATLAS',	-20, 						100, 						GreatWorkYieldType,	GoldenAge,	NeverCapture, NukeImmune, NumCityCostMod, 'GREAT_WORK_SLOT_MUSIC', 	'TXT_KEY_THEMING_BONUS_JERUSALEM_TEMPLE_HELP', 	3,				6, 						0,							-10
FROM Buildings WHERE Type = 'BUILDING_GRAND_TEMPLE';
--------------------------------	
-- Building_Flavors
--------------------------------	
INSERT INTO	Building_Flavors
			(BuildingType,						FlavorType,					Flavor)
VALUES		('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'FLAVOR_RELIGION',			140),
			('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'FLAVOR_WONDER',			60),
			('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'FLAVOR_GOLD',				30),
			('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'FLAVOR_CULTURE',			50);
--------------------------------	
-- Building_YieldChanges
--------------------------------	
INSERT INTO Building_YieldChanges
			(BuildingType,						YieldType, 		 	Yield)
VALUES		('BUILDING_LEUGI_JERUSALEM_TEMPLE', 'YIELD_FAITH', 	 	5),
			('BUILDING_LEUGI_JERUSALEM_TEMPLE', 'YIELD_CULTURE', 	2),
			('BUILDING_LEUGI_JERUSALEM_TEMPLE', 'YIELD_GOLD', 		1);
------------------------------	
-- Building_YieldFromPolicyUnlock
------------------------------
INSERT INTO Building_YieldFromPolicyUnlock
			(BuildingType,						YieldType,		Yield)
VALUES		('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'YIELD_FAITH',	50);
--------------------------------	
-- Building_YieldChangesPerPopInEmpire
--------------------------------	
INSERT INTO Building_YieldChangesPerPopInEmpire
			(BuildingType,						YieldType,		Yield)
VALUES		('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'YIELD_FAITH',	25);
------------------------------
-- Building_ResourceYieldChangesGlobal
------------------------------
INSERT INTO Building_ResourceYieldChangesGlobal
	(BuildingType, 							ResourceType, 		YieldType, 			Yield)
VALUES
	('BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'RESOURCE_INCENSE', 'YIELD_CULTURE', 	1),
	('BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'RESOURCE_INCENSE', 'YIELD_GOLD', 	 	1),
	('BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'RESOURCE_WINE', 	'YIELD_CULTURE', 	1),
	('BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'RESOURCE_WINE', 	'YIELD_GOLD', 	 	1),
	('BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'RESOURCE_AMBER', 	'YIELD_CULTURE', 	1),
	('BUILDING_LEUGI_JERUSALEM_TEMPLE', 	'RESOURCE_AMBER', 	'YIELD_GOLD', 	 	1);
------------------------------	
-- Building_ThemingYieldBonus
------------------------------
INSERT INTO Building_ThemingYieldBonus
			(BuildingType, YieldType, Yield)
VALUES		('BUILDING_LEUGI_JERUSALEM_TEMPLE', 'YIELD_CULTURE', 	5),
			('BUILDING_LEUGI_JERUSALEM_TEMPLE', 'YIELD_GOLD',		5);
------------------------------	
-- Building_ThemingBonuses
------------------------------	
INSERT INTO Building_ThemingBonuses	
			(BuildingType,						Description,								  	Bonus,	SameEra,	RequiresSamePlayer,	AIPriority)
VALUES		('BUILDING_LEUGI_JERUSALEM_TEMPLE',	'TXT_KEY_THEMING_BONUS_JERUSALEM_TEMPLE',		10,		0,		    1,			     2);
--==========================================================================================================================	
-- PROMOTIONS
--==========================================================================================================================	
INSERT INTO IconTextureAtlases 
		(Atlas, 									IconSize, 	Filename, 							IconsPerRow, 	IconsPerColumn)
VALUES	('ISRAEL_PROMOTION_ATLAS', 					16, 		'Israel_PI_016.dds',				4, 				1),
		('ISRAEL_PROMOTION_ATLAS', 					32, 		'Israel_PI_032.dds',				4, 				1),
		('ISRAEL_PROMOTION_ATLAS', 					45, 		'Israel_PI_045.dds',				4, 				1),
		('ISRAEL_PROMOTION_ATLAS', 					64, 		'Israel_PI_064.dds',				4, 				1),
		('ISRAEL_PROMOTION_ATLAS', 					256, 		'Israel_PI_256.dds',				4, 				1);
--------------------------------	
-- UnitPromotions
--------------------------------	
INSERT INTO UnitPromotions
		(Type,							Description,							Help,										CannotBeChosen,	Sound,				PortraitIndex,		IconAtlas,					PediaType,		PediaEntry,								AuraEffectChange, 	LostWithUpgrade, 	BarbarianCombatBonus)
VALUES	
		('PROMOTION_PROMISED_LAND',		'TXT_KEY_PROMOTION_PROMISED_LAND',		'TXT_KEY_PROMOTION_PROMISED_LAND_HELP',		1,				'AS2D_IF_LEVELUP',	2,					'ISRAEL_PROMOTION_ATLAS',	'PEDIA_MELEE',	'TXT_KEY_PROMOTION_PROMISED_LAND',		0, 					0, 					25);
--------------------------------	
-- Unit_FreePromotions
--------------------------------	
INSERT INTO		Unit_FreePromotions
				(UnitType, 				PromotionType)
SELECT			'UNIT_ISRAEL_MACCABEE', PromotionType
FROM Unit_FreePromotions WHERE UnitType = 'UNIT_SWORDSMAN';

INSERT INTO Unit_FreePromotions
			(UnitType, 					PromotionType)
VALUES		
			('UNIT_ISRAEL_MACCABEE', 	'PROMOTION_HASMONEAN');

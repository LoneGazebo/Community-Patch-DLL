-- CIVILIANS AND GREAT PEOPLE

	-- Moved SS Parts to last 4 techs - makes Science Victory as difficult as other victories.
	UPDATE Units SET PrereqTech = 'TECH_NUCLEAR_FUSION' WHERE Type = 'UNIT_SS_BOOSTER';
	UPDATE Units SET PrereqTech = 'TECH_GLOBALIZATION' WHERE Type = 'UNIT_SS_COCKPIT';

	-- Caravans moved to Pottery
	UPDATE Units SET PrereqTech = 'TECH_HORSEBACK_RIDING', MilitarySupport = '0', ProductionCostAddedPerEra = '75' WHERE Type = 'UNIT_CARAVAN';
	
	-- Cargo Ship -- Move to Optics

	UPDATE Units SET PrereqTech = 'TECH_OPTICS', MilitarySupport = '0', ProductionCostAddedPerEra = '75' WHERE Type = 'UNIT_CARGO_SHIP';

	-- Settler moved to Pottery
	UPDATE Units SET PrereqTech = 'TECH_POTTERY' WHERE Type = 'UNIT_SETTLER';

	-- Workers Reduced Work Rate to slow down early growth
	UPDATE Units SET WorkRate = '90' WHERE Type = 'UNIT_WORKER';

	-- Great Prophets no longer capturable
	UPDATE Units Set Capture = NULL WHERE Type = 'UNIT_PROPHET';

	-- Great Writer culture boost lowered slightly, added scaling bonus for num owned GWS
	UPDATE Units SET BaseCultureTurnsToCount = '5', ScaleFromNumGWs = '3' WHERE Type = 'UNIT_WRITER';


	-- Great Scientist science boost lowered slightly.
	UPDATE Units SET BaseBeakersTurnsToCount = '3' WHERE Type = 'UNIT_SCIENTIST';

	-- Admiral movement buff, ability addition
	UPDATE Units SET Moves = '5', NumFreeLux = '2' WHERE Type = 'UNIT_GREAT_ADMIRAL';

	-- Merchant gold boosted, CS ability buff
	UPDATE Units SET NumGoldPerEra = '150' WHERE Type = 'UNIT_MERCHANT';

	-- Reduced base golden age turns of artist, added scaling bonus for num themed GWs
	UPDATE Units SET GoldenAgeTurns = '0', BaseTurnsForGAPToCount = '10', ScaleFromNumThemes = '20' WHERE Type = 'UNIT_ARTIST';

	-- Engineer production boost nerfed slightly.
	UPDATE Units SET BaseHurry = '100', HurryMultiplier = '20' WHERE Type = 'UNIT_ENGINEER';

	-- Musician changes
	UPDATE Units SET OneShotTourismPercentOthers = '0', OneShotTourism = '0', TourismBonusTurns = '10' WHERE Type = 'UNIT_MUSICIAN';

	-- work boat sight penalty
	UPDATE Units SET BaseSightRange = '1', Moves = '2', MilitarySupport = '0' WHERE Type = 'UNIT_WORKBOAT';

	UPDATE Units
	SET RequiresFaithPurchaseEnabled = '1'
	WHERE Type = 'UNIT_ARCHAEOLOGIST';

-- cap for archaeologists
UPDATE UnitClasses SET MaxPlayerInstances = '3' WHERE Type = 'UNITCLASS_ARCHAEOLOGIST';

--modder please note still does +2 turns just like custom house, but weird bug i dont ask why.
INSERT INTO Unit_ScalingFromOwnedImprovements
	(UnitType, ImprovementType, Amount)
VALUES
	('UNIT_SCIENTIST', 'IMPROVEMENT_ACADEMY', '10'),
	('UNIT_ENGINEER', 'IMPROVEMENT_MANUFACTORY', '25'),
	('UNIT_MERCHANT', 'IMPROVEMENT_CUSTOMS_HOUSE', '25'),
	('UNIT_VENETIAN_MERCHANT', 'IMPROVEMENT_CUSTOMS_HOUSE', '25');

INSERT INTO Missions
	(Type, Description, Help, DisabledHelp, EntityEventType, Time, Target, Build, Sound, HotKey, AltDown, ShiftDown, CtrlDown, HotKeyPriority, HotKeyAlt, AltDownAlt, ShiftDownAlt, CtrlDownAlt, HotKeyPriorityAlt, OrderPriority, Visible, IconIndex, IconAtlas)
VALUES
	('MISSION_FREE_LUXURY', 'TXT_KEY_MISSION_FREE_LUXURY', 'TXT_KEY_MISSION_FREE_LUXURY_HELP', 'TXT_KEY_MISSION_FREE_LUXURY_HELP_DISABLED', 'ENTITY_EVENT_GREAT_EVENT', 20, 0, 0, 0, NULL, 0, 0, 0, 0, NULL, 0, 0, 0, 0, 199, 1, 16, 'UNIT_ACTION_ATLAS');

-- All civs start with a pathfinder

--Replace Starting Warrior with a Pathfinder
UPDATE Eras SET StartingDefenseUnits = '1', StartingExploreUnits = '1' WHERE Type='ERA_ANCIENT';

--Assigns UnitCombatInfos to civilian units -- Can now grant production bonuses and free promotions via buildings, traits and policies
INSERT INTO UnitCombatInfos  	
			(Type,					Description)
VALUES		
	('UNITCOMBAT_SETTLER',	'TXT_KEY_UNITCOMBAT_SETTLER'),
	('UNITCOMBAT_WORKER',	'TXT_KEY_UNITCOMBAT_WORKER'),
	('UNITCOMBAT_WORKBOAT',	'TXT_KEY_UNITCOMBAT_WORKBOAT'),
	('UNITCOMBAT_CARGO',	'TXT_KEY_UNITCOMBAT_CARGO'),
	('UNITCOMBAT_CARAVAN',	'TXT_KEY_UNITCOMBAT_CARAVAN'),
	('UNITCOMBAT_INQUISITOR',	'TXT_KEY_UNITCOMBAT_INQUISITOR'),
	('UNITCOMBAT_MISSIONARY',	'TXT_KEY_UNITCOMBAT_MISSIONARY');
	
UPDATE Units SET CombatClass = 'UNITCOMBAT_SETTLER' WHERE Type IN ('UNIT_SETTLER', 'UNIT_PIONEER', 'UNIT_COLONIST');
UPDATE Units SET CombatClass = 'UNITCOMBAT_WORKER' WHERE Type = 'UNIT_WORKER';
UPDATE Units SET CombatClass = 'UNITCOMBAT_WORKBOAT' WHERE Type = 'UNIT_WORKBOAT';
UPDATE Units SET CombatClass = 'UNITCOMBAT_CARGO' WHERE Type = 'UNIT_CARGO_SHIP';
UPDATE Units SET CombatClass = 'UNITCOMBAT_CARAVAN' WHERE Type = 'UNIT_CARAVAN';
UPDATE Units SET CombatClass = 'UNITCOMBAT_INQUISITOR' WHERE Type = 'UNIT_INQUISITOR';
UPDATE Units SET CombatClass = 'UNITCOMBAT_MISSIONARY' WHERE Type = 'UNIT_MISSIONARY';

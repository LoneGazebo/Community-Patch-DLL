-- Change Value = 1 in CustomModOptions to enable Cargo Ships Mod
UPDATE CustomModOptions SET Value = 0 WHERE Name = 'CARGO_SHIPS';

ALTER TABLE Units
  ADD CargoCombat INTEGER DEFAULT 0;

INSERT INTO SpecialUnits
			(Type,						Description,						Valid,	CityLoad)
SELECT		'SPECIALUNIT_CARGO_ARMY',	'TXT_KEY_SPECIALUNIT_CARGO_ARMY',	1,		0
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO SpecialUnit_CarrierUnitAI
			(SpecialUnitType,			UnitAIType)
SELECT		'SPECIALUNIT_CARGO_ARMY',	'UNITAI_ESCORT_SEA'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

UPDATE Units
SET Special = 'SPECIALUNIT_CARGO_ARMY'
WHERE Type IN (SELECT Type FROM Units WHERE CombatClass = 'UNITCOMBAT_MELEE' OR CombatClass = 'UNITCOMBAT_ARCHER' OR CombatClass = 'UNITCOMBAT_GUN' OR CombatClass = 'UNITCOMBAT_HELICOPTER' OR CombatClass	= 'UNITCOMBAT_MOUNTED' OR CombatClass = 'UNITCOMBAT_ARMOR' OR CombatClass = 'UNITCOMBAT_SIEGE' OR CombatClass = 'UNITCOMBAT_RECON') AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

UPDATE Units
SET SpecialCargo = 'SPECIALUNIT_CARGO_ARMY', DomainCargo = 'DOMAIN_LAND', CargoCombat = 20
WHERE Type IN (SELECT Type FROM Units WHERE CombatClass = 'UNITCOMBAT_NAVALMELEE') AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO Unit_FreePromotions
		(UnitType,	PromotionType)
SELECT	Type,		('PROMOTION_CARGO_I')
FROM Units WHERE (Class = 'UNITCLASS_TRIREME') AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO Unit_FreePromotions
		(UnitType,	PromotionType)
SELECT	Type,		('PROMOTION_CARGO_II')
FROM Units WHERE (Class = 'UNITCLASS_CARAVEL' OR Class = 'UNITCLASS_PRIVATEER' OR Class = 'UNITCLASS_IRONCLAD') AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO Unit_FreePromotions
		(UnitType,	PromotionType)
SELECT	Type,		('PROMOTION_CARGO_III')
FROM Units WHERE (Class = 'UNITCLASS_DESTROYER') AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO UnitPromotions
			(Type,						Description,						Help,									Sound,				RangedSupportFire,	CannotBeChosen,	PortraitIndex,	IconAtlas,			PediaType,			PediaEntry)
SELECT		'PROMOTION_ARMY_SUPPORT',	'TXT_KEY_PROMOTION_ARMY_SUPPORT',	'TXT_KEY_PROMOTION_ARMY_SUPPORT_HELP',	'AS2D_IF_LEVELUP',	1,					1,				59,				'ABILITY_ATLAS',	'PEDIA_ATTRIBUTES',	'TXT_KEY_PROMOTION_ARMY_SUPPORT'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO UnitPromotions
			(Type,							Description,							Help,										Sound,				PortraitIndex,	CannotBeChosen,	ExtraNavalMovement,	IconAtlas,			PediaType,			PediaEntry)
SELECT		'PROMOTION_POLYNESIA_CARGO',	'TXT_KEY_PROMOTION_POLYNESIA_CARGO',	'TXT_KEY_PROMOTION_POLYNESIA_CARGO_HELP',	'AS2D_IF_LEVELUP',	59,				1,				1,					'ABILITY_ATLAS',	'PEDIA_ATTRIBUTES',	'TXT_KEY_PROMOTION_POLYNESIA_CARGO_HELP'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO UnitPromotions
			(Type,							Description,							Help,										Sound,				VisibilityChange,	CannotBeChosen,	PortraitIndex,	IconAtlas,			PediaType,		PediaEntry)
SELECT		'PROMOTION_ARMY_SCOUT_SHIP',	'TXT_KEY_PROMOTION_ARMY_SCOUT_SHIP',	'TXT_KEY_PROMOTION_ARMY_SCOUT_SHIP_HELP',	'AS2D_IF_LEVELUP',	2,					1,				59,				'ABILITY_ATLAS',	'PEDIA_SHARED', 'TXT_KEY_PROMOTION_ARMY_SCOUT_SHIP'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO UnitPromotions
			(Type,						Description,						Help,									Sound,				CannotBeChosen,	PortraitIndex,	IconAtlas,			PediaType,		PediaEntry)
SELECT		'PROMOTION_ARMY_ON_SHIP',	'TXT_KEY_PROMOTION_ARMY_ON_SHIP',	'TXT_KEY_PROMOTION_ARMY_ON_SHIP_HELP',	'AS2D_IF_LEVELUP',	1,				59,				'ABILITY_ATLAS',	'PEDIA_SHARED',	'TXT_KEY_PROMOTION_ARMY_ON_SHIP'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO UnitPromotions
			(Type,							Description,							Help,											Sound,				CombatPercent,	RangeChange,	CannotBeChosen,	PortraitIndex,	IconAtlas,			PediaType,		PediaEntry)
SELECT		'PROMOTION_ARMY_RANGE_PENALTY',	'TXT_KEY_PROMOTION_ARMY_RANGE_PENALTY',	'TXT_KEY_PROMOTION_ARMY_RANGE_PENALTY_HELP',	'AS2D_IF_LEVELUP',	-25,			-1,				1,				57,				'ABILITY_ATLAS',	'PEDIA_SHARED', 'TXT_KEY_PROMOTION_ARMY_RANGE_PENALTY'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

INSERT INTO Language_en_US
			(Tag,													Text)
SELECT		'TXT_KEY_PROMOTION_ARMY_SUPPORT',						'Pirate Ship' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_SUPPORT_HELP',					'Before engaging in naval melee combat, this unit has an extra attack' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_ON_SHIP',						'Military Unit on Ship' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_ON_SHIP_HELP',					'This unit is cargo and can unload from Ship to Land only.' UNION ALL
SELECT		'TXT_KEY_SPECIALUNIT_CARGO_ARMY',						'Army Cargo Unit' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_RANGE_PENALTY',					'Ranged Naval Penalty' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_RANGE_PENALTY_HELP',			'-25% [ICON_STRENGTH] Ranged Strength and -1 Range when using Ranged Attacks' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_SCOUT_SHIP',					'Recon Unit on Ship' UNION ALL
SELECT		'TXT_KEY_PROMOTION_ARMY_SCOUT_SHIP_HELP',				'+2 Visibility Range' UNION ALL
SELECT		'TXT_KEY_PROMOTION_POLYNESIA_CARGO',					'Polynesia Cargo Ship Movement' UNION ALL
SELECT		'TXT_KEY_PROMOTION_POLYNESIA_CARGO_HELP',				'+1 Extra Movement for Naval Units with Land Cargo' UNION ALL
SELECT		'TXT_KEY_ARMY_CARGO_UNIT_1',							'[NEWLINE]Can carry 1 Land Unit.' UNION ALL
SELECT		'TXT_KEY_ARMY_CARGO_UNIT_2',							'[NEWLINE]Can carry 2 Land Units.' UNION ALL
SELECT		'TXT_KEY_ARMY_CARGO_UNIT_3',							'[NEWLINE]Can carry 3 Land Units.' UNION ALL
SELECT		'TXT_KEY_ARMY_CARGO_UNIT_4',							'[NEWLINE]Can carry 4 Land Units.'
WHERE EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

UPDATE Language_en_US 
SET Text = Text||(SELECT ' '||Text FROM Language_en_US WHERE Tag = 'TXT_KEY_ARMY_CARGO_UNIT_1')
WHERE Tag IN (SELECT Help FROM Units WHERE SpecialCargo = 'SPECIALUNIT_CARGO_ARMY' AND Type IN (SELECT UnitType FROM Unit_FreePromotions WHERE PromotionType = 'PROMOTION_CARGO_I')) AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

UPDATE Language_en_US 
SET Text = Text||(SELECT ' '||Text FROM Language_en_US WHERE Tag = 'TXT_KEY_ARMY_CARGO_UNIT_2')
WHERE Tag IN (SELECT Help FROM Units WHERE SpecialCargo = 'SPECIALUNIT_CARGO_ARMY' AND Type IN (SELECT UnitType FROM Unit_FreePromotions WHERE PromotionType = 'PROMOTION_CARGO_II')) AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );

UPDATE Language_en_US 
SET Text = Text||(SELECT ' '||Text FROM Language_en_US WHERE Tag = 'TXT_KEY_ARMY_CARGO_UNIT_3')
WHERE Tag IN (SELECT Help FROM Units WHERE SpecialCargo = 'SPECIALUNIT_CARGO_ARMY' AND Type IN (SELECT UnitType FROM Unit_FreePromotions WHERE PromotionType = 'PROMOTION_CARGO_III')) AND EXISTS (SELECT * FROM CustomModOptions WHERE Name='CARGO_SHIPS' AND Value= 1 );
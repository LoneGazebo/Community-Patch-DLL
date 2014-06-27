-- 

--
-- Obsoletes
--

UPDATE Units
SET ObsoleteTech = (
	SELECT unitNew.PrereqTech
	FROM Unit_ClassUpgrades upgrade, Units unitOld, Units unitNew, UnitClasses unitClass
	WHERE (	unitOld.Type	= Units.Type
		AND unitOld.Type	= upgrade.UnitType
		AND unitClass.Type	= upgrade.UnitClassType
		AND unitNew.Type	= unitClass.DefaultUnit
	)
) WHERE Type IN (SELECT UnitType FROM Unit_ClassUpgrades);


UPDATE Units SET ObsoleteTech = 'TECH_CHIVALRY' WHERE Class = 'UNITCLASS_CHARIOT_ARCHER';
UPDATE Units SET ObsoleteTech = NULL WHERE Class = 'UNITCLASS_MODERN_ARMOR';
UPDATE Units SET ObsoleteTech = 'TECH_METAL_CASTING' WHERE Class = 'UNITCLASS_SCOUT';
UPDATE Units SET ObsoleteTech = 'TECH_AGRICULTURE' WHERE Class = 'UNITCLASS_ANTI_TANK_GUN';
UPDATE Units SET ObsoleteTech = 'TECH_AGRICULTURE' WHERE Class = 'UNITCLASS_MARINE';


-- This CER_End.sql data automatically created by:										
-- Tech_Deletes tab of GEM_Details.xlsspreadsheet (in mod folder).										
UPDATE Units SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_BALLISTICS';	UPDATE Builds SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_BALLISTICS';	UPDATE Buildings SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_BALLISTICS';	UPDATE Projects SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';	UPDATE Processes SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';	UPDATE Traits SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_BALLISTICS';	UPDATE Policies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';	UPDATE UnitPromotions SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';	UPDATE AICityStrategies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';	UPDATE AIEconomicStrategies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';	UPDATE AIMilitaryStrategies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_BALLISTICS';
UPDATE Units SET PrereqTech='TECH_INTERNET'  WHERE PrereqTech='TECH_TELECOM';	UPDATE Builds SET PrereqTech='TECH_INTERNET'  WHERE PrereqTech='TECH_TELECOM';	UPDATE Buildings SET PrereqTech='TECH_INTERNET'  WHERE PrereqTech='TECH_TELECOM';	UPDATE Projects SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';	UPDATE Processes SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';	UPDATE Traits SET PrereqTech='TECH_INTERNET'  WHERE PrereqTech='TECH_TELECOM';	UPDATE Policies SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';	UPDATE UnitPromotions SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';	UPDATE AICityStrategies SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';	UPDATE AIEconomicStrategies SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';	UPDATE AIMilitaryStrategies SET TechPrereq='TECH_INTERNET'  WHERE TechPrereq='TECH_TELECOM';
UPDATE Units SET PrereqTech='TECH_REFRIGERATION'  WHERE PrereqTech='TECH_PENICILIN';	UPDATE Builds SET PrereqTech='TECH_REFRIGERATION'  WHERE PrereqTech='TECH_PENICILIN';	UPDATE Buildings SET PrereqTech='TECH_REFRIGERATION'  WHERE PrereqTech='TECH_PENICILIN';	UPDATE Projects SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';	UPDATE Processes SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';	UPDATE Traits SET PrereqTech='TECH_REFRIGERATION'  WHERE PrereqTech='TECH_PENICILIN';	UPDATE Policies SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';	UPDATE UnitPromotions SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';	UPDATE AICityStrategies SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';	UPDATE AIEconomicStrategies SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';	UPDATE AIMilitaryStrategies SET TechPrereq='TECH_REFRIGERATION'  WHERE TechPrereq='TECH_PENICILIN';
UPDATE Units SET PrereqTech='TECH_LASERS'  WHERE PrereqTech='TECH_MOBILE_TACTICS';	UPDATE Builds SET PrereqTech='TECH_LASERS'  WHERE PrereqTech='TECH_MOBILE_TACTICS';	UPDATE Buildings SET PrereqTech='TECH_LASERS'  WHERE PrereqTech='TECH_MOBILE_TACTICS';	UPDATE Projects SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';	UPDATE Processes SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';	UPDATE Traits SET PrereqTech='TECH_LASERS'  WHERE PrereqTech='TECH_MOBILE_TACTICS';	UPDATE Policies SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';	UPDATE UnitPromotions SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';	UPDATE AICityStrategies SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';	UPDATE AIEconomicStrategies SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';	UPDATE AIMilitaryStrategies SET TechPrereq='TECH_LASERS'  WHERE TechPrereq='TECH_MOBILE_TACTICS';
UPDATE Units SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_RADAR';	UPDATE Builds SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_RADAR';	UPDATE Buildings SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_RADAR';	UPDATE Projects SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';	UPDATE Processes SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';	UPDATE Traits SET PrereqTech='TECH_ELECTRONICS'  WHERE PrereqTech='TECH_RADAR';	UPDATE Policies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';	UPDATE UnitPromotions SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';	UPDATE AICityStrategies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';	UPDATE AIEconomicStrategies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';	UPDATE AIMilitaryStrategies SET TechPrereq='TECH_ELECTRONICS'  WHERE TechPrereq='TECH_RADAR';


--/*

DELETE FROM Technologies WHERE Type IN (
	'TECH_BALLISTICS'				,
	'TECH_TELECOM'					,
	'TECH_PENICILIN'				,
	'TECH_MOBILE_TACTICS'			,
	'TECH_RADAR'					
);

/* remap IDs
DROP TABLE IF EXISTS IDRemapper;
CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
INSERT INTO IDRemapper (Type) SELECT Type FROM Technologies;
UPDATE Technologies SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE Technologies.Type = IDRemapper.Type);
UPDATE sqlite_sequence SET seq = (SELECT COUNT(ID) FROM Technologies)-1 WHERE name = 'Technologies';

--*/

--TECH_NULL to NULL
UPDATE Buildings SET PrereqTech = NULL WHERE PrereqTech = 'TECH_NULL';

--Sync EnhancedYieldTech with PrereqTech
UPDATE Buildings SET EnhancedYieldTech = PrereqTech WHERE BuildingClass = 'BUILDINGCLASS_EIFFEL_TOWER';

UPDATE LoadedFile SET Value=1 WHERE Type='CER_End.sql';
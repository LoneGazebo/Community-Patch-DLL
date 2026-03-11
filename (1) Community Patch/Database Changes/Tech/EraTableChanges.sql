-- Extra units of UNITAI_DEFENSE to all city states when starting a game in this era
ALTER TABLE Eras ADD StartingCityStateDefenseUnits integer DEFAULT 0;

-- Extra unit supply to all major civ players when starting a game in this era
ALTER TABLE Eras ADD UnitSupplyBase integer DEFAULT 0;

-- Enables gaining vassals
ALTER TABLE Eras ADD VassalageEnabled boolean DEFAULT 0;

-- New era popup splash image (used to be hardcoded, see NewEraPopup.lua)
ALTER TABLE Eras ADD EraSplashImage text DEFAULT 'ERA_Medievel.dds';

-- Era Food Scaling for Specialists. If no value is specified (-1), the value from the latest previous era that is larger than -1 is used as fallback. This avoids issues with modmods that add an additional era and don't set a value for this column
ALTER TABLE Eras ADD SpecialistExtraFoodCost integer DEFAULT -1;

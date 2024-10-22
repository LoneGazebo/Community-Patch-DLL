-- This minor civ always has this personality instead of picking a random one
-- Overrides the BALANCE_CITY_STATE_PERSONALITIES option
ALTER TABLE MinorCivilizations ADD FixedPersonality text REFERENCES MinorCivPersonalityTypes (Type);

-- The unit class given to the bully when this minor civ is heavy tributed
-- Overridden by the BALANCE_CORE_MINOR_VARIABLE_BULLYING option, which does not give units
ALTER TABLE MinorCivilizations ADD BullyUnitClass text REFERENCES UnitClasses (Type) DEFAULT 'UNITCLASS_WORKER';

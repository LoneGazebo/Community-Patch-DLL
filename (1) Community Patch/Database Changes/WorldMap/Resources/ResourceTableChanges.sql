-- Monopoly
ALTER TABLE Resources ADD IsMonopoly boolean DEFAULT 0;
ALTER TABLE Resources ADD MonopolyHappiness integer DEFAULT 0;
ALTER TABLE Resources ADD MonopolyGALength integer DEFAULT 0;
ALTER TABLE Resources ADD MonopolyAttackBonus integer DEFAULT 0;
ALTER TABLE Resources ADD MonopolyDefenseBonus integer DEFAULT 0;
ALTER TABLE Resources ADD MonopolyMovementBonus integer DEFAULT 0;
ALTER TABLE Resources ADD MonopolyHealBonus integer DEFAULT 0;
ALTER TABLE Resources ADD MonopolyXPBonus integer DEFAULT 0;

-- Additional help text for strategic monopolies
ALTER TABLE Resources ADD StrategicHelp text REFERENCES Language_en_US (Tag);

-- Used in EUI top panel to order strategic resources. MUST be set to a distinct positive integer or the resource wont show up.
ALTER TABLE Resources ADD StrategicPriority integer DEFAULT 0;

-- New column to separate improveable from tradeable for resources
ALTER TABLE Resources ADD TechImproveable text REFERENCES Technologies (Type);

-- Support column for SQL queries (used in modmods only)
ALTER TABLE Resources ADD LandResource boolean DEFAULT 0;
ALTER TABLE Resources ADD SeaResource boolean DEFAULT 0;
ALTER TABLE Resources ADD AnimalResource boolean DEFAULT 0;
ALTER TABLE Resources ADD PlantResource boolean DEFAULT 0;
ALTER TABLE Resources ADD RockResource boolean DEFAULT 0;

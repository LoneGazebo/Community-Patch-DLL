-- TODO: Documentation
ALTER TABLE Projects ADD FreeBuildingClassIfFirst text REFERENCES BuildingClasses (Type);
ALTER TABLE Projects ADD FreePolicyIfFirst text REFERENCES Policies (Type);

ALTER TABLE Projects ADD CostScalerEra integer DEFAULT 0;
ALTER TABLE Projects ADD CostScalerNumRepeats integer DEFAULT 0;
ALTER TABLE Projects ADD IsRepeatable boolean DEFAULT 0;
ALTER TABLE Projects ADD Happiness integer DEFAULT 0;
ALTER TABLE Projects ADD EmpireSizeModifierReduction integer DEFAULT 0;
ALTER TABLE Projects ADD DistressFlatReduction integer DEFAULT 0;
ALTER TABLE Projects ADD PovertyFlatReduction integer DEFAULT 0;
ALTER TABLE Projects ADD IlliteracyFlatReduction integer DEFAULT 0;
ALTER TABLE Projects ADD BoredomFlatReduction integer DEFAULT 0;
ALTER TABLE Projects ADD ReligiousUnrestFlatReduction integer DEFAULT 0;
ALTER TABLE Projects ADD BasicNeedsMedianModifier integer DEFAULT 0;
ALTER TABLE Projects ADD GoldMedianModifier integer DEFAULT 0;
ALTER TABLE Projects ADD ScienceMedianModifier integer DEFAULT 0;
ALTER TABLE Projects ADD CultureMedianModifier integer DEFAULT 0;
ALTER TABLE Projects ADD ReligiousUnrestModifier integer DEFAULT 0;
ALTER TABLE Projects ADD SpySecurityModifier integer DEFAULT 0;

ALTER TABLE Projects ADD InfluenceAllRequired boolean DEFAULT 0;
ALTER TABLE Projects ADD IdeologyRequired boolean DEFAULT 0;

-- require x tier 3 tenets prior to construction
ALTER TABLE Projects ADD NumRequiredTier3Tenets integer DEFAULT 0;

-- Modifier to the EMPIRE_SIZE_NEED_MODIFIER_CITIES variable
ALTER TABLE Projects ADD EmpireSizeModifierPerCityMod integer DEFAULT 0;

-- The civilization that can exclusively work on this
ALTER TABLE Projects ADD CivilizationType text REFERENCES Civilizations (Type);

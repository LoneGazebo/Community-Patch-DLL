-- Build
CREATE INDEX IF NOT EXISTS idx_BuildFeatures_ON_BuildType
ON BuildFeatures (BuildType);

-- Building
CREATE INDEX IF NOT EXISTS idx_Building_ThemingBonuses_ON_BuildingType
ON Building_ThemingBonuses (BuildingType);

CREATE INDEX IF NOT EXISTS idx_Building_YieldChanges_ON_BuildingType_AND_YieldType
ON Building_YieldChanges (BuildingType, YieldType);

-- Civilization
CREATE INDEX IF NOT EXISTS idx_Civilization_BuildingClassOverrides_ON_CivilizationType
ON Civilization_BuildingClassOverrides (CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_BuildingClassOverrides_ON_BuildingClassType_AND_CivilizationType
ON Civilization_BuildingClassOverrides (BuildingClassType, CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_BuildingClassOverrides_ON_BuildingType_AND_CivilizationType
ON Civilization_BuildingClassOverrides (BuildingType, CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_UnitClassOverrides_ON_CivilizationType
ON Civilization_UnitClassOverrides (CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_UnitClassOverrides_ON_UnitClassType_AND_CivilizationType
ON Civilization_UnitClassOverrides (UnitClassType, CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_UnitClassOverrides_ON_UnitType_AND_CivilizationType
ON Civilization_UnitClassOverrides (UnitType, CivilizationType);

-- Concept
CREATE INDEX IF NOT EXISTS idx_Concepts_RelatedConcept_ON_ConceptType
ON Concepts_RelatedConcept (ConceptType);

-- Feature
CREATE INDEX IF NOT EXISTS idx_Feature_YieldChanges_ON_FeatureType
ON Feature_YieldChanges (FeatureType);

-- Improvement
CREATE INDEX IF NOT EXISTS idx_Improvement_ResourceType_Yields_ON_ImprovementType
ON Improvement_ResourceType_Yields (ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Improvement_ResourceType_Yields_ON_ResourceType
ON Improvement_ResourceType_Yields (ResourceType);

CREATE INDEX IF NOT EXISTS idx_Improvement_ResourceTypes_ON_ImprovementType
ON Improvement_ResourceTypes (ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Improvement_ResourceTypes_ON_ResourceType
ON Improvement_ResourceTypes (ResourceType);

CREATE INDEX IF NOT EXISTS idx_Improvement_RouteYieldChanges_ON_ImprovementType
ON Improvement_RouteYieldChanges (ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Improvement_TechYieldChanges_ON_ImprovementType
ON Improvement_TechYieldChanges (ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Improvement_TechYieldChanges_ON_TechType
ON Improvement_TechYieldChanges (TechType);

CREATE INDEX IF NOT EXISTS idx_Improvement_ValidTerrains_ON_ImprovementType
ON Improvement_ValidTerrains (ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Improvement_Yields_ON_ImprovementType
ON Improvement_Yields (ImprovementType);

-- Leader
CREATE INDEX IF NOT EXISTS idx_Leader_Traits_ON_LeaderType
ON Leader_Traits (LeaderType);

CREATE INDEX IF NOT EXISTS idx_Diplomacy_Responses_ON_LeaderType_AND_ResponseType
ON Diplomacy_Responses (LeaderType, ResponseType);

-- Policy
CREATE INDEX IF NOT EXISTS idx_Policy_ImprovementYieldChanges_ON_PolicyType_AND_ImprovementType
ON Policy_ImprovementYieldChanges (PolicyType, ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Policy_ImprovementYieldChanges_ON_ImprovementType
ON Policy_ImprovementYieldChanges (ImprovementType);

-- Resource
CREATE INDEX IF NOT EXISTS idx_Resource_TerrainBooleans_ON_ResourceType
ON Resource_TerrainBooleans (ResourceType);

CREATE INDEX IF NOT EXISTS idx_Resource_TerrainBooleans_ON_TerrainType
ON Resource_TerrainBooleans (TerrainType);

CREATE INDEX IF NOT EXISTS idx_Resource_YieldChanges_ON_ResourceType
ON Resource_YieldChanges (ResourceType);

-- Specialist
CREATE INDEX IF NOT EXISTS idx_SpecialistYields_ON_SpecialistType
ON SpecialistYields (SpecialistType);

-- Technology
CREATE INDEX IF NOT EXISTS idx_Technology_PrereqTechs_ON_TechType
ON Technology_PrereqTechs (TechType);

CREATE INDEX IF NOT EXISTS idx_Technology_PrereqTechs_ON_PrereqTech
ON Technology_PrereqTechs (PrereqTech);

-- Unit
CREATE INDEX IF NOT EXISTS idx_Unit_BuildingClassPurchaseRequireds_ON_UnitType_AND_BuildingClassType
ON Unit_BuildingClassPurchaseRequireds (UnitType, BuildingClassType);

CREATE INDEX IF NOT EXISTS idx_Unit_Builds_ON_UnitType
ON Unit_Builds (UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_ClassUpgrades_ON_UnitType
ON Unit_ClassUpgrades (UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_ClassUpgrades_ON_UnitClassType
ON Unit_ClassUpgrades (UnitClassType);

CREATE INDEX IF NOT EXISTS idx_Unit_FreePromotions_ON_UnitType_AND_PromotionType
ON Unit_FreePromotions (UnitType, PromotionType);

-- UnitPromotion
CREATE INDEX IF NOT EXISTS idx_UnitPromotions_UnitCombats_ON_PromotionType_AND_UnitCombatType
ON UnitPromotions_UnitCombats (PromotionType, UnitCombatType);

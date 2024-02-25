CREATE INDEX IF NOT EXISTS idx_Diplomacy_Responses_ON_LeaderType_AND_ResponseType
ON Diplomacy_Responses(LeaderType, ResponseType);

CREATE INDEX IF NOT EXISTS idx_BuildFeatures_ON_BuildType
ON BuildFeatures(BuildType);

-- AI

CREATE INDEX IF NOT EXISTS idx_AIMilitaryStrategy_City_Flavors_ON_AIMilitaryStrategyType
ON AIMilitaryStrategy_City_Flavors(AIMilitaryStrategyType);

CREATE INDEX IF NOT EXISTS idx_AIEconomicStrategy_Player_Flavors_ON_AIEconomicStrategyType
ON AIEconomicStrategy_Player_Flavors(AIEconomicStrategyType);

CREATE INDEX IF NOT EXISTS idx_AIEconomicStrategy_City_Flavors_ON_AIEconomicStrategyType
ON AIEconomicStrategy_City_Flavors(AIEconomicStrategyType);

CREATE INDEX IF NOT EXISTS idx_AICityStrategy_Flavors_ON_AICityStrategyType
ON AICityStrategy_Flavors(AICityStrategyType);

-- Building

CREATE INDEX IF NOT EXISTS idx_Building_Flavors_ON_BuildingType
ON Building_Flavors(BuildingType);

CREATE INDEX IF NOT EXISTS idx_Building_ThemingBonuses_ON_BuildingType
ON Building_ThemingBonuses(BuildingType);

CREATE INDEX IF NOT EXISTS idx_Building_YieldChanges_ON_BuildingType
ON Building_YieldChanges(BuildingType);

-- CitySpecialization

CREATE INDEX IF NOT EXISTS idx_CitySpecialization_Flavors_ON_CitySpecializationType
ON CitySpecialization_Flavors(CitySpecializationType);

CREATE INDEX IF NOT EXISTS idx_CitySpecialization_TargetYields_ON_CitySpecializationType
ON CitySpecialization_TargetYields(CitySpecializationType);

-- Civilization

CREATE INDEX IF NOT EXISTS idx_Civilization_BuildingClassOverrides_ON_CivilizationType
ON Civilization_BuildingClassOverrides(CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_CityNames_ON_CivilizationType
ON Civilization_CityNames(CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_SpyNames_ON_CivilizationType
ON Civilization_SpyNames(CivilizationType);

CREATE INDEX IF NOT EXISTS idx_Civilization_UnitClassOverrides_ON_CivilizationType
ON Civilization_UnitClassOverrides(CivilizationType);

-- Concept

CREATE INDEX IF NOT EXISTS idx_Concepts_RelatedConcept_ON_ConceptType
ON Concepts_RelatedConcept(ConceptType);

-- EventChoice

-- CREATE INDEX IF NOT EXISTS idx_EventChoiceFlavors_ON_EventChoiceType
-- ON EventChoiceFlavors(EventChoiceType);

-- Feature
CREATE INDEX IF NOT EXISTS idx_Feature_YieldChanges_ON_FeatureType
ON Feature_YieldChanges(FeatureType);

-- GameSpeed

CREATE INDEX IF NOT EXISTS idx_GameSpeed_Turns_ON_GameSpeedType
ON GameSpeed_Turns(GameSpeedType);

-- Improvement

CREATE INDEX IF NOT EXISTS idx_Improvement_ResourceType_Yields_ON_ImprovementType_AND_ResourceType
ON Improvement_ResourceType_Yields(ImprovementType, ResourceType);

CREATE INDEX IF NOT EXISTS idx_Improvement_ResourceTypes_ON_ImprovementType_AND_ResourceType
ON Improvement_ResourceTypes(ImprovementType, ResourceType);

CREATE INDEX IF NOT EXISTS idx_Improvement_RouteYieldChanges_ON_ImprovementType_AND_RouteType
ON Improvement_RouteYieldChanges(ImprovementType, RouteType);

CREATE INDEX IF NOT EXISTS idx_Improvement_TechYieldChanges_ON_ImprovementType_AND_TechType
ON Improvement_TechYieldChanges(ImprovementType, TechType);

CREATE INDEX IF NOT EXISTS idx_Improvement_ValidTerrains_ON_ImprovementType
ON Improvement_ValidTerrains(ImprovementType);

CREATE INDEX IF NOT EXISTS idx_Improvement_Yields_ON_ImprovementType
ON Improvement_Yields(ImprovementType);

-- Leader

CREATE INDEX IF NOT EXISTS idx_Leader_Flavors_ON_LeaderType
ON Leader_Flavors(LeaderType);

CREATE INDEX IF NOT EXISTS idx_Leader_MajorCivApproachBiases_ON_LeaderType
ON Leader_MajorCivApproachBiases(LeaderType);

CREATE INDEX IF NOT EXISTS idx_Leader_MinorCivApproachBiases_ON_LeaderType
ON Leader_MinorCivApproachBiases(LeaderType);

CREATE INDEX IF NOT EXISTS idx_Leader_Traits_ON_LeaderType
ON Leader_Traits(LeaderType);

-- MinorCivilization

CREATE INDEX IF NOT EXISTS idx_MinorCivilization_Flavors_ON_MinorCivType
ON MinorCivilization_Flavors(MinorCivType);

CREATE INDEX IF NOT EXISTS idx_MinorCivilization_CityNames_ON_MinorCivType
ON MinorCivilization_CityNames(MinorCivType);

-- MovementRates

CREATE INDEX IF NOT EXISTS idx_MovementRates_ON_Type
ON MovementRates(Type);

-- MultiUnitFormation

CREATE INDEX IF NOT EXISTS idx_MultiUnitFormation_SlotEntries_ON_MultiUnitFormationType
ON MultiUnitFormation_SlotEntries(MultiUnitFormationType);

-- Policy

CREATE INDEX IF NOT EXISTS idx_Policy_Flavors_ON_PolicyType
ON Policy_Flavors(PolicyType);

CREATE INDEX IF NOT EXISTS idx_Policy_ImprovementYieldChanges_ON_PolicyType
ON Policy_ImprovementYieldChanges(PolicyType);

-- Process

CREATE INDEX IF NOT EXISTS idx_Process_Flavors_ON_ProcessType
ON Process_Flavors(ProcessType);

-- Resource

CREATE INDEX IF NOT EXISTS idx_Resource_Flavors_ON_ResourceType
ON Resource_Flavors(ResourceType);

CREATE INDEX IF NOT EXISTS idx_Resource_TerrainBooleans_ON_ResourceType
ON Resource_TerrainBooleans(ResourceType);

CREATE INDEX IF NOT EXISTS idx_Resource_YieldChanges_ON_ResourceType
ON Resource_YieldChanges(ResourceType);

-- Specialist

CREATE INDEX IF NOT EXISTS idx_SpecialistFlavors_ON_SpecialistType
ON SpecialistFlavors(SpecialistType);

CREATE INDEX IF NOT EXISTS idx_SpecialistYields_ON_SpecialistType
ON SpecialistYields(SpecialistType);

-- Technology

CREATE INDEX IF NOT EXISTS idx_Technology_Flavors_ON_TechType
ON Technology_Flavors(TechType);

CREATE INDEX IF NOT EXISTS idx_Technology_PrereqTechs_ON_TechType
ON Technology_PrereqTechs(TechType);

-- Unit

CREATE INDEX IF NOT EXISTS idx_Unit_AITypes_ON_UnitType
ON Unit_AITypes(UnitType);

-- CREATE INDEX IF NOT EXISTS idx_Unit_BuildingClassPurchaseRequireds_ON_UnitType
-- ON Unit_BuildingClassPurchaseRequireds(UnitType);
--
-- CREATE INDEX IF NOT EXISTS idx_Unit_BuildOnFound_ON_UnitType
-- ON Unit_BuildOnFound(UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_Builds_ON_UnitType
ON Unit_Builds(UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_ClassUpgrades_ON_UnitType
ON Unit_ClassUpgrades(UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_Flavors_ON_UnitType
ON Unit_Flavors(UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_FreePromotions_ON_UnitType
ON Unit_FreePromotions(UnitType);

CREATE INDEX IF NOT EXISTS idx_Unit_UniqueNames_ON_UnitType
ON Unit_UniqueNames(UnitType);

CREATE INDEX IF NOT EXISTS idx_UnitGameplay2DScripts_ON_UnitType
ON UnitGameplay2DScripts(UnitType);

-- UnitPromotion

CREATE INDEX IF NOT EXISTS idx_UnitPromotions_UnitCombats_ON_PromotionType
ON UnitPromotions_UnitCombats(PromotionType);



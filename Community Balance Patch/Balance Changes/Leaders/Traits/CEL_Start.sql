--
-- Indonesia
--

DELETE from Building_YieldChangesPerReligion WHERE BuildingType="BUILDING_CANDI";

UPDATE Civilization_BuildingClassOverrides SET BuildingClassType="BUILDINGCLASS_SHRINE" WHERE BuildingType="BUILDING_CANDI";


UPDATE LoadedFile SET Value=1 WHERE Type='CEL_Start.sql';
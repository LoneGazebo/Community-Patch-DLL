-- Availability: Buildings
--
CREATE TABLE IF NOT EXISTS
	Technology_Flavors_Human (
	TechType		text REFERENCES Technologies(Type),
	FlavorType		text REFERENCES Flavors(Type),
	Flavor			integer
);
CREATE TABLE IF NOT EXISTS
	Building_Flavors_Human (
	BuildingType	text REFERENCES Buildings(Type),
	FlavorType		text REFERENCES Flavors(Type),
	Flavor			integer
);

CREATE TABLE IF NOT EXISTS
	Build_Flavors (
	BuildType		text REFERENCES Builds(Type),
	FlavorType		text REFERENCES Flavors(Type),
	Flavor			integer
);

ALTER TABLE Buildings		ADD AIAvailability						integer default 0;
	 UPDATE Buildings		SET AIAvailability						= 0; 

ALTER TABLE Builds			ADD AIAvailability						integer default 0;


UPDATE Buildings SET AIAvailability = 8;

UPDATE Buildings SET AIAvailability = 4
WHERE (Water = 1
	OR River = 1
	OR FreshWater = 1
	OR Hill = 1
	OR Flat = 1
	OR Mountain = 1
	OR NearbyMountainRequired = 1
	OR MutuallyExclusiveGroup = 1
	OR NoOccupiedUnhappiness = 1
	OR NearbyTerrainRequired IS NOT NULL
);

UPDATE Buildings SET AIAvailability = 4
WHERE (Type IN (SELECT BuildingType FROM Building_ResourceQuantityRequirements)
	OR Type IN (SELECT BuildingType FROM Building_LocalResourceOrs)
	OR Type IN (SELECT BuildingType FROM Building_LocalResourceAnds)
	--OR Type IN (SELECT BuildingType FROM Building_ResourceYieldModifiers)
);

UPDATE Buildings SET AIAvailability = 2
WHERE Type IN (SELECT building.Type
FROM Buildings building, BuildingClasses class
WHERE (building.BuildingClass = class.Type AND (
	   class.MaxGlobalInstances = 1
	OR class.MaxPlayerInstances = 1
	OR class.MaxTeamInstances = 1
)));

UPDATE Buildings SET AIAvailability = 0
WHERE Cost = -1;
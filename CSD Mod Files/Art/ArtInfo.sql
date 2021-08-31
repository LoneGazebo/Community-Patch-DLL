-- UnitArtInfo --

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_EMISSARY', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_ENVOY', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_DIPLOMAT', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_AMBASSADOR', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_GREAT_DIPLOMAT', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation, IconAtlas, PortraitIndex)
SELECT 'ART_DEF_UNIT_GREAT_DIPLOMAT_RENAISSANCE', '1','UnFormed', 'CSD_ATLAS', '11';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation, IconAtlas, PortraitIndex)
SELECT 'ART_DEF_UNIT_GREAT_DIPLOMAT_MODERN', '1','UnFormed', 'CSD_ATLAS', '19';

-- Unit Members --

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_EMISSARY','ART_DEF_UNIT_MEMBER_SETTLERS_ASIAN_F2', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_ENVOY','ART_DEF_UNIT_MEMBER_MISSIONARY_01', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_DIPLOMAT','ART_DEF_UNIT_MEMBER_EUROFEMALE18', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_AMBASSADOR','ART_DEF_UNIT_MEMBER_ARCHAEOLOGIST', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_GREAT_DIPLOMAT','ART_DEF_UNIT_MEMBER_AFRIMALE3', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_GREAT_DIPLOMAT_RENAISSANCE','ART_DEF_UNIT_MEMBER_EUROFEMALE18', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_GREAT_DIPLOMAT_MODERN','ART_DEF_UNIT_MEMBER_GREAT_ARTIST_F2', '1';

-- Embassy

INSERT INTO ArtDefine_LandmarkTypes(Type, LandmarkType, FriendlyName)
SELECT 'ART_DEF_IMPROVEMENT_EMBASSY', 'Improvement', 'EMBASSY';

INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
SELECT 'Any', 'UnderConstruction', 1.2,  'ART_DEF_IMPROVEMENT_EMBASSY', 'RANDOM', 'ART_DEF_RESOURCE_NONE', 'Embassy.fxsxml', 1 UNION ALL
SELECT 'Any', 'Constructed', 1.2,  'ART_DEF_IMPROVEMENT_EMBASSY', 'RANDOM', 'ART_DEF_RESOURCE_NONE', 'Embassy.fxsxml', 1 UNION ALL
SELECT 'Any', 'Pillaged', 1.2,  'ART_DEF_IMPROVEMENT_EMBASSY', 'RANDOM', 'ART_DEF_RESOURCE_NONE', 'Embassy.fxsxml', 1;

-- Changes to Barbarians (enables their city artstyle)

UPDATE Civilizations
Set ArtStyleType = 'ARTSTYLE_EUROPEAN'
WHERE Type = 'CIVILIZATION_BARBARIAN';

UPDATE Civilizations
Set ArtStyleSuffix = '_EURO'
WHERE Type = 'CIVILIZATION_BARBARIAN';

UPDATE Civilizations
Set ArtStylePrefix = 'EUROPEAN'
WHERE Type = 'CIVILIZATION_BARBARIAN';

UPDATE Civilizations
Set ArtDefineTag = 'ART_DEF_CIVILIZATION_RUSSIA'
WHERE Type = 'CIVILIZATION_BARBARIAN';
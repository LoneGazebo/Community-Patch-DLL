--MACCABEE

INSERT INTO ArtDefine_UnitInfos (Type,DamageStates,Formation)
	SELECT	('ART_DEF_UNIT_MACCABEE'), DamageStates, Formation
	FROM ArtDefine_UnitInfos WHERE (Type = 'ART_DEF_UNIT_SWORDSMAN');

INSERT INTO ArtDefine_UnitInfoMemberInfos	VALUES	('ART_DEF_UNIT_MACCABEE', 'ART_DEF_UNIT_MEMBER_MACCABEE', "14");

INSERT INTO ArtDefine_UnitMemberCombats (UnitMemberType, EnableActions, DisableActions, MoveRadius, ShortMoveRadius, ChargeRadius, AttackRadius, RangedAttackRadius, MoveRate, ShortMoveRate, TurnRateMin, TurnRateMax, TurnFacingRateMin, TurnFacingRateMax, RollRateMin, RollRateMax, PitchRateMin, PitchRateMax, LOSRadiusScale, TargetRadius, TargetHeight, HasShortRangedAttack, HasLongRangedAttack, HasLeftRightAttack, HasStationaryMelee, HasStationaryRangedAttack, HasRefaceAfterCombat, ReformBeforeCombat, HasIndependentWeaponFacing, HasOpponentTracking, HasCollisionAttack, AttackAltitude, AltitudeDecelerationDistance, OnlyTurnInMovementActions, RushAttackFormation)
	SELECT	('ART_DEF_UNIT_MEMBER_MACCABEE'), EnableActions, DisableActions, MoveRadius, ShortMoveRadius, ChargeRadius, AttackRadius, RangedAttackRadius, MoveRate, ShortMoveRate, TurnRateMin, TurnRateMax, TurnFacingRateMin, TurnFacingRateMax, RollRateMin, RollRateMax, PitchRateMin, PitchRateMax, LOSRadiusScale, TargetRadius, TargetHeight, HasShortRangedAttack, HasLongRangedAttack, HasLeftRightAttack, HasStationaryMelee, HasStationaryRangedAttack, HasRefaceAfterCombat, ReformBeforeCombat, HasIndependentWeaponFacing, HasOpponentTracking, HasCollisionAttack, AttackAltitude, AltitudeDecelerationDistance, OnlyTurnInMovementActions, RushAttackFormation
	FROM ArtDefine_UnitMemberCombats WHERE (UnitMemberType = 'ART_DEF_UNIT_MEMBER_SWORDSMAN');

INSERT INTO ArtDefine_UnitMemberCombatWeapons (UnitMemberType, "Index", SubIndex, ID, VisKillStrengthMin, VisKillStrengthMax, ProjectileSpeed, ProjectileTurnRateMin, ProjectileTurnRateMax, HitEffect, HitEffectScale, HitRadius, ProjectileChildEffectScale, AreaDamageDelay, ContinuousFire, WaitForEffectCompletion, TargetGround, IsDropped, WeaponTypeTag, WeaponTypeSoundOverrideTag)
	SELECT ('ART_DEF_UNIT_MEMBER_MACCABEE'), "Index", SubIndex, ID, VisKillStrengthMin, VisKillStrengthMax, ProjectileSpeed, ProjectileTurnRateMin, ProjectileTurnRateMax, HitEffect, HitEffectScale, HitRadius, ProjectileChildEffectScale, AreaDamageDelay, ContinuousFire, WaitForEffectCompletion, TargetGround, IsDropped, WeaponTypeTag, WeaponTypeSoundOverrideTag
	FROM ArtDefine_UnitMemberCombatWeapons WHERE (UnitMemberType = 'ART_DEF_UNIT_MEMBER_SWORDSMAN');

INSERT INTO ArtDefine_UnitMemberInfos (Type, Scale, ZOffset, Domain, Model, MaterialTypeTag, MaterialTypeSoundOverrideTag)
	SELECT	('ART_DEF_UNIT_MEMBER_MACCABEE'), Scale, ZOffset, Domain, ('maccabee.fxsxml'), MaterialTypeTag, MaterialTypeSoundOverrideTag
	FROM ArtDefine_UnitMemberInfos WHERE (Type = 'ART_DEF_UNIT_MEMBER_SWORDSMAN');

INSERT INTO ArtDefine_StrategicView (StrategicViewType, TileType, Asset )
	SELECT	('ART_DEF_UNIT_MACCABEE'), TileType, ('sv_MACCABEE.dds')
	FROM ArtDefine_StrategicView WHERE (StrategicViewType = 'ART_DEF_UNIT_SWORDSMAN');



	--

--INSERT INTO ArtDefine_LandmarkTypes(Type, LandmarkType, FriendlyName)
--SELECT 'ART_DEF_IMPROVEMENT_KIBBUTZ', 'Improvement', 'KIBBUTZ';


--INSERT INTO ArtDefine_Landmarks(Era, State, Scale, ImprovementType, LayoutHandler, ResourceType, Model, TerrainContour)
--SELECT 'Any', 'UnderConstruction', 0.8,  'ART_DEF_IMPROVEMENT_KIBBUTZ', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'kibbutz_hb.fxsxml', 1 UNION ALL
--SELECT 'Any', 'Constructed', 0.8,  'ART_DEF_IMPROVEMENT_KIBBUTZ', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'kibbutz.fxsxml', 1 UNION ALL
--SELECT 'Any', 'Pillaged', 0.6,  'ART_DEF_IMPROVEMENT_KIBBUTZ', 'SNAPSHOT', 'ART_DEF_RESOURCE_ALL', 'kibbutz_pl.fxsxml', 1;


--INSERT INTO ArtDefine_StrategicView(StrategicViewType, TileType, Asset)
--SELECT 'ART_DEF_IMPROVEMENT_KIBBUTZ', 'Improvement', 'sv_KIBBUTZ.dds';

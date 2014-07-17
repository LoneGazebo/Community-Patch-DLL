/*
Unit Movement Animation Duration
The animation time required for a unit to visually move between tiles.
The default Community Balance Patch values are 50% of vanilla (half duration = twice as fast).
*/

UPDATE MovementRates SET
TotalTime			= 0.5 * TotalTime,
EaseIn				= 0.5 * EaseIn,
EaseOut				= 0.5 * EaseOut,
IndividualOffset	= 0.5 * IndividualOffset,
RowOffset			= 0.5 * RowOffset
AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_UNITMOVEMENT_INCREASE' AND Value= 1 );


/*
Aircraft Move Speed
The speed of aircraft movement.
The default Community Balance Patch values are 400% of vanilla (four times as fast).
*/

UPDATE ArtDefine_UnitMemberCombats
SET MoveRate = 4 * MoveRate AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_UNITMOVEMENT_INCREASE' AND Value= 1 );

UPDATE ArtDefine_UnitMemberCombats
SET TurnRateMin = 4 * TurnRateMin
WHERE MoveRate > 0 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_UNITMOVEMENT_INCREASE' AND Value= 1 );

UPDATE ArtDefine_UnitMemberCombats
SET TurnRateMax = 4 * TurnRateMax
WHERE MoveRate > 0 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_UNITMOVEMENT_INCREASE' AND Value= 1 );
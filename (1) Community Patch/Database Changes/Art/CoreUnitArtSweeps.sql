-- Faster aircraft
UPDATE ArtDefine_UnitMemberCombats SET MoveRate = MoveRate * 3;
UPDATE ArtDefine_UnitMemberCombats SET TurnRateMin = TurnRateMin * 3 WHERE MoveRate IS NOT NULL;
UPDATE ArtDefine_UnitMemberCombats SET TurnRateMax = TurnRateMax * 3 WHERE MoveRate IS NOT NULL;

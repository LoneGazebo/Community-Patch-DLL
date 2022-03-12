UPDATE ArtDefine_UnitMemberCombats SET MoveRate = 3*MoveRate;
UPDATE ArtDefine_UnitMemberCombats SET TurnRateMin = 3*TurnRateMin WHERE MoveRate > 0;
UPDATE ArtDefine_UnitMemberCombats SET TurnRateMax = 3*TurnRateMax WHERE MoveRate > 0;
-- gives the most recent era's value (rather than adding all valid eras)
INSERT INTO MinorCivilizationTraits_FriendYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_ANCIENT', 'YIELD_FAITH', 200),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_MEDIEVAL', 'YIELD_FAITH', 400),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_INDUSTRIAL', 'YIELD_FAITH', 800),
    ('MINOR_TRAIT_CULTURED', 'ERA_ANCIENT', 'YIELD_CULTURE', 300),
    ('MINOR_TRAIT_CULTURED', 'ERA_MEDIEVAL', 'YIELD_CULTURE', 600),
    ('MINOR_TRAIT_CULTURED', 'ERA_INDUSTRIAL', 'YIELD_CULTURE', 1300);

INSERT INTO MinorCivilizationTraits_AllyYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_ANCIENT', 'YIELD_FAITH', 200),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_MEDIEVAL', 'YIELD_FAITH', 400),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_INDUSTRIAL', 'YIELD_FAITH', 800),
    ('MINOR_TRAIT_CULTURED', 'ERA_ANCIENT', 'YIELD_CULTURE', 300),
    ('MINOR_TRAIT_CULTURED', 'ERA_MEDIEVAL', 'YIELD_CULTURE', 600),
    ('MINOR_TRAIT_CULTURED', 'ERA_INDUSTRIAL', 'YIELD_CULTURE', 1300);

INSERT INTO MinorCivilizationTraits_FriendCityYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield, CapitalOnly)
VALUES
    ('MINOR_TRAIT_MARITIME', 'ERA_ANCIENT', 'YIELD_FOOD', 200, 1);

INSERT INTO MinorCivilizationTraits_AllyCityYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield, CapitalOnly)
VALUES
    ('MINOR_TRAIT_MARITIME', 'ERA_ANCIENT', 'YIELD_FOOD', 100, 0);

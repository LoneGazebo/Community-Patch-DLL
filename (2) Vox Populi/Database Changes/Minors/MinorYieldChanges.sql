-----------------------------------------------------------------------------------------------
-- Maritime: Food on capital tile and other city tiles x100 
-----------------------------------------------------------------------------------------------
INSERT INTO MinorCivilizationTraits_FriendCityYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield, CapitalOnly)
VALUES
    ('MINOR_TRAIT_MARITIME', 'ERA_ANCIENT', 'YIELD_FOOD', 200, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_CLASSICAL', 'YIELD_FOOD', 300, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_MEDIEVAL', 'YIELD_FOOD', 500, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_RENAISSANCE', 'YIELD_FOOD', 600, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_INDUSTRIAL', 'YIELD_FOOD', 800, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_MODERN', 'YIELD_FOOD', 900, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_POSTMODERN', 'YIELD_FOOD', 1100, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_FUTURE', 'YIELD_FOOD', 1200, 1),
  
    --('MINOR_TRAIT_MARITIME', 'ERA_ANCIENT', 'YIELD_FOOD', 0, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_CLASSICAL', 'YIELD_FOOD', 100, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_MEDIEVAL', 'YIELD_FOOD', 100, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_RENAISSANCE', 'YIELD_FOOD', 200, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_INDUSTRIAL', 'YIELD_FOOD', 200, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_MODERN', 'YIELD_FOOD', 300, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_POSTMODERN', 'YIELD_FOOD', 300, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_FUTURE', 'YIELD_FOOD', 400, 0);

INSERT INTO MinorCivilizationTraits_AllyCityYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield, CapitalOnly)
VALUES
    ('MINOR_TRAIT_MARITIME', 'ERA_ANCIENT', 'YIELD_FOOD', 300, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_CLASSICAL', 'YIELD_FOOD', 600, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_MEDIEVAL', 'YIELD_FOOD', 900, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_RENAISSANCE', 'YIELD_FOOD', 1200, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_INDUSTRIAL', 'YIELD_FOOD', 1500, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_MODERN', 'YIELD_FOOD', 1800, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_POSTMODERN', 'YIELD_FOOD', 2100, 1),
    ('MINOR_TRAIT_MARITIME', 'ERA_FUTURE', 'YIELD_FOOD', 2400, 1),
  
    ('MINOR_TRAIT_MARITIME', 'ERA_ANCIENT', 'YIELD_FOOD', 100, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_CLASSICAL', 'YIELD_FOOD', 200, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_MEDIEVAL', 'YIELD_FOOD', 300, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_RENAISSANCE', 'YIELD_FOOD', 400, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_INDUSTRIAL', 'YIELD_FOOD', 500, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_MODERN', 'YIELD_FOOD', 600, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_POSTMODERN', 'YIELD_FOOD', 700, 0),
    ('MINOR_TRAIT_MARITIME', 'ERA_FUTURE', 'YIELD_FOOD', 800, 0);

-----------------------------------------------------------------------------------------------
-- Merchantile: Gold on empire
-- Note also gives Happiness (and CS-only Luxury on Ally)
-----------------------------------------------------------------------------------------------
INSERT INTO MinorCivilizationTraits_FriendYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_MERCANTILE', 'ERA_ANCIENT', 'YIELD_GOLD', 2),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_CLASSICAL', 'YIELD_GOLD', 4),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_MEDIEVAL', 'YIELD_GOLD', 8),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_RENAISSANCE', 'YIELD_GOLD', 12),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_INDUSTRIAL', 'YIELD_GOLD', 16),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_MODERN', 'YIELD_GOLD', 20),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_POSTMODERN', 'YIELD_GOLD', 24),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_FUTURE', 'YIELD_GOLD', 28);

INSERT INTO MinorCivilizationTraits_AllyYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_MERCANTILE', 'ERA_ANCIENT', 'YIELD_GOLD', 4),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_CLASSICAL', 'YIELD_GOLD', 8),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_MEDIEVAL', 'YIELD_GOLD', 16),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_RENAISSANCE', 'YIELD_GOLD', 24),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_INDUSTRIAL', 'YIELD_GOLD', 32),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_MODERN', 'YIELD_GOLD', 40),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_POSTMODERN', 'YIELD_GOLD', 48),
    ('MINOR_TRAIT_MERCANTILE', 'ERA_FUTURE', 'YIELD_GOLD', 56);

-----------------------------------------------------------------------------------------------
-- Militaristic: Science on empire
-- Note also gives Unit Spawn
-----------------------------------------------------------------------------------------------
INSERT INTO MinorCivilizationTraits_FriendYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_ANCIENT', 'YIELD_SCIENCE', 1),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_CLASSICAL', 'YIELD_SCIENCE', 1),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_MEDIEVAL', 'YIELD_SCIENCE', 2),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_RENAISSANCE', 'YIELD_SCIENCE', 3),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_INDUSTRIAL', 'YIELD_SCIENCE', 4),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_MODERN', 'YIELD_SCIENCE', 5),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_POSTMODERN', 'YIELD_SCIENCE', 6),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_FUTURE', 'YIELD_SCIENCE', 7);

INSERT INTO MinorCivilizationTraits_AllyYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_ANCIENT', 'YIELD_SCIENCE', 2),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_CLASSICAL', 'YIELD_SCIENCE', 3),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_MEDIEVAL', 'YIELD_SCIENCE', 6),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_RENAISSANCE', 'YIELD_SCIENCE', 9),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_INDUSTRIAL', 'YIELD_SCIENCE', 12),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_MODERN', 'YIELD_SCIENCE', 15),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_POSTMODERN', 'YIELD_SCIENCE', 18),
    ('MINOR_TRAIT_MILITARISTIC', 'ERA_FUTURE', 'YIELD_SCIENCE', 21);

-----------------------------------------------------------------------------------------------
-- Cultured: Culture on empire
-----------------------------------------------------------------------------------------------
INSERT INTO MinorCivilizationTraits_FriendYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_CULTURED', 'ERA_ANCIENT', 'YIELD_CULTURE', 1),
    ('MINOR_TRAIT_CULTURED', 'ERA_CLASSICAL', 'YIELD_CULTURE', 3),
    ('MINOR_TRAIT_CULTURED', 'ERA_MEDIEVAL', 'YIELD_CULTURE', 6),
    ('MINOR_TRAIT_CULTURED', 'ERA_RENAISSANCE', 'YIELD_CULTURE', 9),
    ('MINOR_TRAIT_CULTURED', 'ERA_INDUSTRIAL', 'YIELD_CULTURE', 12),
    ('MINOR_TRAIT_CULTURED', 'ERA_MODERN', 'YIELD_CULTURE', 15),
    ('MINOR_TRAIT_CULTURED', 'ERA_POSTMODERN', 'YIELD_CULTURE', 18),
    ('MINOR_TRAIT_CULTURED', 'ERA_FUTURE', 'YIELD_CULTURE', 21);

INSERT INTO MinorCivilizationTraits_AllyYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_CULTURED', 'ERA_ANCIENT', 'YIELD_CULTURE', 3),
    ('MINOR_TRAIT_CULTURED', 'ERA_CLASSICAL', 'YIELD_CULTURE', 6),
    ('MINOR_TRAIT_CULTURED', 'ERA_MEDIEVAL', 'YIELD_CULTURE', 12),
    ('MINOR_TRAIT_CULTURED', 'ERA_RENAISSANCE', 'YIELD_CULTURE', 18),
    ('MINOR_TRAIT_CULTURED', 'ERA_INDUSTRIAL', 'YIELD_CULTURE', 24),
    ('MINOR_TRAIT_CULTURED', 'ERA_MODERN', 'YIELD_CULTURE', 30),
    ('MINOR_TRAIT_CULTURED', 'ERA_POSTMODERN', 'YIELD_CULTURE', 36),
    ('MINOR_TRAIT_CULTURED', 'ERA_FUTURE', 'YIELD_CULTURE', 42);

-----------------------------------------------------------------------------------------------
-- Religious: Faith on empire
-- Different yields per era up to Industrial
-----------------------------------------------------------------------------------------------
INSERT INTO MinorCivilizationTraits_FriendYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_ANCIENT', 'YIELD_FAITH', 1),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_CLASSICAL', 'YIELD_FAITH', 3),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_MEDIEVAL', 'YIELD_FAITH', 6),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_RENAISSANCE', 'YIELD_FAITH', 9),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_INDUSTRIAL', 'YIELD_FAITH', 12),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_MODERN', 'YIELD_FAITH', 15),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_POSTMODERN', 'YIELD_FAITH', 18),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_FUTURE', 'YIELD_FAITH', 21);

INSERT INTO MinorCivilizationTraits_AllyYieldBonuses
    (MinorCivTraitType, EraType, YieldType, Yield)
VALUES
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_ANCIENT', 'YIELD_FAITH', 3),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_CLASSICAL', 'YIELD_FAITH', 6),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_MEDIEVAL', 'YIELD_FAITH', 12),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_RENAISSANCE', 'YIELD_FAITH', 18),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_INDUSTRIAL', 'YIELD_FAITH', 24),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_MODERN', 'YIELD_FAITH', 30),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_POSTMODERN', 'YIELD_FAITH', 36),
    ('MINOR_TRAIT_RELIGIOUS', 'ERA_FUTURE', 'YIELD_FAITH', 42);

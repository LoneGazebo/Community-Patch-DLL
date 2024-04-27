-- Plunder Trade Route
UPDATE Language_en_US
SET Text = 'Destroy this trade route to gain [ICON_GOLD] Gold.[NEWLINE][NEWLINE]If you are not at war with the owner of the trade route, you will need to declare war before you plunder (unless an ability allows otherwise). Plundering a trade route going to another civilization will damage your relationship with the trade route owner and the destination civilization if you''re not currently at war.'
WHERE Tag = 'TXT_KEY_MISSION_PLUNDER_TRADE_ROUTE_HELP';

-- Create Great Work
UPDATE Language_en_US
SET Text = 'This order will consume the Great Person and create a new Great Work in the nearest city with a Great Work slot of the appropriate type. Great Works will improve a Great Person''s ability to [COLOR_MAGENTA]{TXT_KEY_MISSION_GIVE_POLICIES}[ENDCOLOR], [COLOR_MAGENTA]{TXT_KEY_MISSION_START_GOLDENAGE}[ENDCOLOR] if themed, and [COLOR_MAGENTA]{TXT_KEY_MISSION_ONE_SHOT_TOURISM}[ENDCOLOR] if a Great Work of Music.'
WHERE Tag = 'TXT_KEY_MISSION_CREATE_GREAT_WORK_HELP';

-- Write Political Treatise
UPDATE Language_en_US
SET Text = 'This order will consume the Great Person and will grant a large amount of [ICON_CULTURE] Culture, scaling by [COLOR_POSITIVE_TEXT]3%[ENDCOLOR] for every owned [ICON_GREAT_WORK] Great Work.'
WHERE Tag = 'TXT_KEY_MISSION_GIVE_POLICIES_HELP';

-- Start a Golden Age
UPDATE Language_en_US
SET Text = '+{1_Num} [ICON_GOLDEN_AGE] Golden Age Points[NEWLINE][NEWLINE]This order will consume the Great Artist and create Golden Age Points, which may trigger a [ICON_GOLDEN_AGE] Golden Age (extra [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture). Scales based on the [ICON_TOURISM] Tourism and [ICON_GOLDEN_AGE] Golden Age Point output of the past 10 turns, and is increased by 10% for every owned [COLOR_POSITIVE_TEXT]themed[ENDCOLOR] Great Work set.'
WHERE Tag = 'TXT_KEY_MISSION_START_GOLDENAGE_HELP';

-- Perform Concert Tour
UPDATE Language_en_US
SET Text = 'When you perform a Concert Tour, your [ICON_TOURISM] Tourism with the target Civilization increases by 100%, and all other Civilizations by 50%, for [COLOR_POSITIVE_TEXT]10[ENDCOLOR] Turns (plus 1 additional Turn for every owned [ICON_VP_GREATMUSIC] Great Work of Music). You also receive 1 [ICON_HAPPINESS_1] Happiness in every City. This action consumes the unit.[NEWLINE][NEWLINE]Cannot perform this action if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [COLOR_MAGENTA]Influential[ENDCOLOR] or greater.'
WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP';

UPDATE Language_en_US
SET Text = 'Unit must be in a tile owned by another major Civilization that you are not at war with and that you are not Influential over.'
WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_DISABLED_HELP';

-- Discover Technology
UPDATE Language_en_US
SET Text = 'This will have the Great Person help research a technology. Every owned Academy you''ve created and own increases the amount of science gained by [COLOR_POSITIVE_TEXT]10%[ENDCOLOR]. It consumes the Great Person.'
WHERE Tag = 'TXT_KEY_MISSION_DISCOVER_TECH_HELP';

UPDATE Language_en_US
SET Text = 'Conduct Diplomatic Mission'
WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION';

UPDATE Language_en_US
SET Text = 'If the unit is inside City-State territory that you are not at war with, this order will expend the unit.[COLOR_POSITIVE_TEXT] There are three units capable of this Mission, each with different abilities:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]Diplomatic Units:[ENDCOLOR] Receive [ICON_INFLUENCE] Influence with the City-State based on the unit''s promotions.[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]Great Diplomats:[ENDCOLOR] Receive a large amount of [ICON_INFLUENCE] Influence with the City-State, increased with each Era, and the [ICON_INFLUENCE] Influence of all other major civilizations known to the City-State will be decreased with this City-State by the same amount. Also increase the [ICON_INFLUENCE] Resting Influence with the City-State by 30.[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]Great Merchant:[ENDCOLOR] Receive a large amount of [ICON_GOLD] Gold and an instant "We Love the King Day" in all owned cities for 5 turns, both scaling by [COLOR_POSITIVE_TEXT]25%[ENDCOLOR] for every owned Town that you control. This action will consume the unit.'
WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP';

-- Hurry Production
UPDATE Language_en_US
SET Text = 'This order will hurry production on the city''s current effort. Every owned Manufactory you''ve created and own increases the amount of production gained by [COLOR_POSITIVE_TEXT]10%[ENDCOLOR]. It consumes the Great Person.'
WHERE Tag = 'TXT_KEY_MISSION_HURRY_PRODUCTION_HELP';

-- Repair Fleet
UPDATE Language_en_US
SET Text = 'This order will consume the Great Admiral and heal all of your Naval Units and Embarked Units in this tile and all adjacent tiles. It also increases your [ICON_WAR] Military Supply by 1.'
WHERE Tag = 'TXT_KEY_MISSION_REPAIR_FLEET_HELP';

-- Remove Heresy
UPDATE Language_en_US
SET Text = 'This order will remove ' || (SELECT Value FROM Defines WHERE Name = 'INQUISITION_EFFECTIVENESS') || '% of pressure from other religions from the nearby, friendly-controlled city. This will consume the unit.'
WHERE Tag = 'TXT_KEY_MISSION_REMOVE_HERESY_HELP';

-- Sell Exotic Goods
UPDATE Language_en_US
SET Text = 'This unit will sell its cargo, earning [ICON_GOLD] Gold and XP. More will be earned for selling farther from your capital. This may only be done twice with this unit.'
WHERE Tag = 'TXT_KEY_MISSION_SELL_EXOTIC_GOODS_HELP';

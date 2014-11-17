-- Piety Opener
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Piety[ENDCOLOR] increases the [ICON_PEACE] Faith and [ICON_GOLD] Gold of empires.[NEWLINE][NEWLINE]Adopting Piety gives you +1 [ICON_PEACE] Faith in your [ICON_CAPITAL] Capital. Shrines and Temples produce +1 [ICON_CULTURE] Culture. Purchases made with [ICON_PEACE] Faith cost 4% less, with each additional policy unlocked in Piety reducing this by an additional 4%. Unlocks building Stonehenge.[NEWLINE][NEWLINE]Adopting all Policies in the Piety tree causes Holy Sites to produce +2 [ICON_GOLD] Gold and +3 [ICON_CULTURE] Culture, and all Specialists to produce +1 [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PIETY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Organized Religion
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Organized Religion[ENDCOLOR][NEWLINE] Temples and Shrines produce +1 [ICON_PEACE] Faith, and can be built 50% faster.'
WHERE Tag = 'TXT_KEY_POLICY_ORGANIZED_RELIGION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Mandate of Heaven (Now Trade Fairs)
UPDATE Language_en_US
SET Text = 'Trade Fairs'
WHERE Tag = 'TXT_KEY_POLICY_MANDATE_OF_HEAVEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Trade Fairs[ENDCOLOR][NEWLINE]+2 [ICON_GOLD] Gold from Temples and +2 [ICON_GOLD] Gold from Markets in a city if your majority [ICON_RELIGION] Religion is present. [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold, and your majority religion spreads 50% more quickly over [ICON_INTERNATIONAL_TRADE] Trade Routes.'
WHERE Tag = 'TXT_KEY_POLICY_MANDATE_OF_HEAVEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Market town or market right is a legal term, originating in the medieval period, for a European settlement that has the right to host markets, or trade faires, thus distinguishing it from a village and city. A town may be correctly described as a "market town" or as having "market rights", even if it no longer holds a market, provided the legal right to do so still exists.'
WHERE Tag = 'TXT_KEY_POLICY_MANDATEOFHEAVEN_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Theocracy (Now Divine Right)
UPDATE Language_en_US
SET Text = 'Divine Right'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Divine Right[ENDCOLOR][NEWLINE] Cities that follow your majority [ICON_RELIGION] Religion generate +2 [ICON_PRODUCTION] Production and +2 [ICON_PEACE] Faith. Border growth is doubled during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The divine right of kings, or divine-right theory of kingship, is a political and religious doctrine of royal and political legitimacy. It asserts that a monarch is subject to no earthly authority, deriving the right to rule directly from the will of God. The king is thus not subject to the will of his people, the aristocracy, or any other estate of the realm, including (in the view of some, especially in Protestant countries) the Church. According to this doctrine, only God can judge an unjust king. The doctrine implies that any attempt to depose the king or to restrict his powers runs contrary to the will of God and may constitute a sacrilegious act. It is often expressed in the phrase "by the Grace of God," attached to the titles of a reigning monarch.'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Reformation (Now Monasticism)
UPDATE Language_en_US
SET Text = 'Monasticism'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Monasticism[ENDCOLOR][NEWLINE] Can purchase Monasteries with [ICON_PEACE] Faith (+1 [ICON_PEACE] Faith, +2 [ICON_FOOD] Food, +1 [ICON_RESEARCH] Science, 1 Scientist slot).'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Monasticism or monkhood is a religious way of life in which one renounces worldly pursuits to devote oneself fully to spiritual work. Monastic life plays an important role in many Christian churches, especially in the Catholic and Orthodox traditions. Similar forms of religious life also exist in other faiths, most notably in Buddhism, but also in Hinduism and Jainism, although the expressions differ considerably.'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Free Religion (Now Tolerance)
UPDATE Language_en_US
SET Text = 'Tolerance'
WHERE Tag = 'TXT_KEY_POLICY_FREE_RELIGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Tolerance[ENDCOLOR][NEWLINE]Cities with a majority [ICON_RELIGION] Religion also get the Pantheon belief bonus of the second most popular [ICON_RELIGION] Religion. Reduces [ICON_HAPPINESS_3] Unhappiness from Religious Strife by 50% in all Cities and an additional 50% in the [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_POLICY_FREE_RELIGION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tolerance or toleration to tolerate, or put up with, conditionally, also to suggest a fair, objective, and permissive attitude toward those whose opinions, practices, race, religion, nationality, etc., differ from your own; freedom from bigotry.'
WHERE Tag = 'TXT_KEY_POLICY_FREERELIGION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monastery
UPDATE Language_en_US
SET Text = '+2 [ICON_FOOD] Food, +1 [ICON_RESEARCH] Science, +1 [ICON_PEACE] Faith, and 1 Scientist slot.'
WHERE Tag = 'TXT_KEY_BUILDING_MONASTERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Monasteries increase [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_RESEARCH] Science output, especially if you can afford to use the Scientist specialist. Can only be built if you have unlocked the Monasticism Policy in the Piety branch. Construct this building by purchasing it with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BUILDING_MONASTERY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

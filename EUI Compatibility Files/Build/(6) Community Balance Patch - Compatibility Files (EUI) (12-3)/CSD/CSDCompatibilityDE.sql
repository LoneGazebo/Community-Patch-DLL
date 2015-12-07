-- Policy Changes

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Patronat[ENDCOLOR] verbessert die Belohnungen von Stadtstaaten Freundschaften und globaler Diplomatie.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Patronat freischalten gewährt:[ENDCOLOR] [NEWLINE] [ICON_BULLET] [ICON_INFLUENCE] Einfluss bei Stadtstaaten um sinkt 25% langsamer als normalerweise. [NEWLINE] [ICON_BULLET] +5 [ICON_INFLUENCE] Einfluss auf die Basispunkte von Stadtstaaten. [NEWLINE] [ICON_BULLET] Schaltet die [COLOR_CYAN]Verbotene Stadt[ENDCOLOR] frei.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Jede Politik im Patronat-Baum gewährt: [ENDCOLOR] [NEWLINE] [ICON_BULLET] +3 [ICON_INFLUENCE] Einfluss auf die Basispunkte von Stadtstaaten.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Die Übernahme aller Sozialpolitiken im Patronat-Baum gewährt:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +10 [ICON_INFLUENCE] Einfluss bei allen bekannten Stadtstaaten, jedes Mal wenn Ihr eine [ICON_GREAT_PEOPLE] Große Persönlichkeit verbraucht. [NEWLINE] [ICON_BULLET] Verbündete Stadtstaaten werden euch von Zeit zu Zeit [ICON_GREAT_PEOPLE] Große Persönlichkeiten schenken.[NEWLINE][ICON_BULLET]Ermöglicht den Zugriff auf Renaissance-Ära Politik Zweige, Era Anforderung ignoriert.[NEWLINE] [ICON_BULLET] Gewährt [COLOR_CYAN]1[ENDCOLOR] Punkt (von [COLOR_CYAN]3[ENDCOLOR] benötigten insgesamt) für das Freischalten von Ideologien.[NEWLINE] [ICON_BULLET] Erlaubt außerdem den Kauf von [ICON_DIPLOMAT] Großen Diplomaten mit [ICON_PEACE] Glauben, ab Beginn des Industriezeitalters.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP';

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Scholastik[ENDCOLOR][NEWLINE]Erhaltet Große Diplomaten 25% schneller. Alle Stadtstaaten mit denen Ihr [COLOR_POSITIVE_TEXT]Verbündet[ENDCOLOR] seid, gewähren einen [ICON_RESEARCH] Wissenschaft-Bonus in Höhe von 33% ihrer eigenen Produktion.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP';
	
-- TEXT CHANGES

UPDATE Language_de_DE
SET Text = 'Setzt eine [ICON_CITIZEN] landesweite Bevölkerung von mindestens 20 voraus, bevor es gebaut werden kann. +1 [ICON_RES_PAPER] Papier. +10% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Kann nur in der Hauptstadt gebaut werden. [NEWLINE][NEWLINE] +10% der [ICON_PRODUCTION] Stadtproduktion wird zur aktuellen [ICON_PRODUCTION] Produktion hinzugefügt, jedes Mal wenn in der Stadt ein neuer [ICON_CITIZEN] Bürger geboren wird.'
WHERE Tag = 'TXT_KEY_BUILDING_COURT_SCRIBE_HELP';

UPDATE Language_de_DE
SET Text = 'Dieses Nationale Wunder is einzigartig, welches Spieler nur in ihrer Hauptstadt bauen dürfen, so lang Ihr eine [ICON_CITIZEN] landesweite Bevölkerung von 20 aufweißt. Es gewährt einen kleinen Produktionsvorteil beim Bau von Diplomatischen Einheiten in der Hauptstadt und eine [ICON_RES_PAPER] Papierquelle. Baut dieses Gebäude um Euch ein oder zwei Stadtstaaten Bündnisse in den ersten Epochen zu sichern. Da es zusätzlich [ICON_PRODUCTION] Produktion durch Stadtwachstum gewährt, ist es wichtig dieses Gebäude früh zu bauen um den größtmöglichsten Vorteil aus diesem Gebäude zu ziehen.'
WHERE Tag = 'TXT_KEY_BUILDING_COURT_SCRIBE_STRATEGY';

UPDATE Language_de_DE
SET Text = 'Dieses nationale Wunder kann erst gebaut werden, wenn die Stadt eine Öffentliche Schule besitzt und und Eure [ICON_CITIZEN] landesweite Bevölkerung mindestens 70 beträgt. Baut dieses Nationale Wunder um weitere Stimmen für den Weltkongress zu erhalten, basierend auf der Anzahl von aktuell vereinbarten Forschungsabkommen mit anderen Spielern. Ihr erhaltet außerdem einen großzügigen Bonus bei der Generierung von Kultur und Wissenschaft in der Stadt in der es gebaut wurde.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = 'Benötigt Ordnung. +15% [ICON_CULTURE] Kultur und [ICON_RESEARCH] Wissenschaft in der Stadt, in der er gebaut wurde. +2 [ICON_RES_PAPER] Papier. Erhaltet eine Stimme für jedes [COLOR_POSITIVE_TEXT]Forschungsabkommen[ENDCOLOR] das Ihr z.zt. mit anderen Spielern abgeschlossen habt. [NEWLINE][NEWLINE]Benötigt eine Öffentliche Schule in der Stadt und mindestens eine [ICON_CITIZEN] landesweite Bevölkerung von 70 bevor es gebaut werden kann. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_HELP';

UPDATE Language_de_DE
SET Text = 'Dieses nationale Wunder kann erst gebaut werden, wenn die Stadt eine Militärakademie besitzt und Eure [ICON_CITIZEN] landesweite Bevölkerung mindestens 70 beträgt. Baut dieses Wunder um weitere Stimmen im Weltkongress zu erhalten, basierend auf der Anzahl eroberter Hauptstädte. Ihr erhaltet außerdem einen Bonus bei der Generierung von Tourismus und Glauben in der Stadt in der es gebaut wurde.'
WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = 'Benötigt Autokratie. +15 [ICON_TOURISM] Tourismus und +15 [ICON_PEACE] Glauben in der Stadt, in der sie gebaut wurde. +2 [ICON_RES_PAPER] Papier. Erhaltet eine Stimme für jede [COLOR_POSITIVE_TEXT]Feindliche Hauptstadt[ENDCOLOR] unter Eurer Kontrolle. [NEWLINE][NEWLINE]Benötigt eine Militär Akademie in der Stadt und mindestens eine [ICON_CITIZEN] landesweite Bevölkerung von 70 bevor es gebaut werden kann. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_HELP';


UPDATE Language_de_DE
SET Text = 'Dieses nationale Wunder kann erst gebaut werden, wenn die Stadt eine Börse besitzt und Eure [ICON_CITIZEN] landesweite Bevölkerung mindestens 70 beträgt. Baut dieses Wunder um weitere Stimmen im Weltkongress zu erhalten, basierend auf der Anzahl aktueller Freundschaftserklärung mit anderen Zivilisationen. Ihr erhaltet außerdem einen Bonus bei der Generierung von Nahrung und Gold in der Stadt in der es gebaut wurde.'
WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = 'Benötigt Freiheit. +15% [ICON_FOOD] Nahrung und [ICON_GOLD] Gold in der Stadt, in der es gebaut wurde. +2 [ICON_RES_PAPER] Papier. Erhaltet eine Stimme für jede [COLOR_POSITIVE_TEXT]Freundschaftserklärung[ENDCOLOR] die Ihr z.zt. mit anderen Spielern vereinbart habt. [NEWLINE][NEWLINE]Benötigt eine Börse in der Stadt und mindestens eine [ICON_CITIZEN] landesweite Bevölkerung von 70 bevor es gebaut werden kann. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_HELP';


UPDATE Language_de_DE
SET Text = 'Dieses Wunder kann erst gebaut werden, wenn die Stadt eine Kanzlei besitzt  und Eure [ICON_CITIZEN] landesweite Bevölkerung mindestens 40 beträgt. Baut es um ein Produktionsbonus auf Diplomatische Einheiten in der Stadt zu erhalten, in der sie gebaut wurde. Erhöht Fortbewegung und Einfluss aller Diplomatischer Einheiten und gewährt die Fähigkeit negative Geländemodifikatoren zu ignorieren.'
WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = '+2 [ICON_RES_PAPER] Papier. +20% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Alle Diplomatischen Einheiten erhalten die Beförderung [COLOR_POSITIVE_TEXT]Bildung[ENDCOLOR].[NEWLINE][NEWLINE]Benötigt eine Kanzlei in der Städten und mindestens eine [ICON_CITIZEN] landesweite Bevölkerung vom 40 bevor es gebaut werden kann. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_HELP';


UPDATE Language_de_DE
SET Text = 'Dieses Wunder kann erst gebaut werden, wenn die Stadt eine Presseagentur besitzt und Eure [ICON_CITIZEN] landesweite Bevölkerung mindestens 60 beträgt. Baue es um einen Produktionsbonus auf Diplomatische Einheiten in der Stadt zu erhalten, in der es gebaut wurde. Erhöht Fortbewegung und Einfluss für alle Diplomatischen Einheiten und gewährt die Fähigkeit Diplomatische Einheiten durch gegnerisches Gelände zu bewegen ohne [COLOR_POSITIVE_TEXT]Offene Grenzen[ENDCOLOR] vereinbart zu haben.'
WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = '+2 [ICON_RES_PAPER] Papier. +20% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Alle Diplomatischen Einheiten erhalten die Beförderung [COLOR_POSITIVE_TEXT]Diplomatische Immunität[ENDCOLOR].[NEWLINE][NEWLINE]Benötigt Nachrichtendienst in der Stadt und mindestens eine landesweite Bevölkerung vom 60 bevor es gebaut werden kann. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_HELP';

-- Temple Help Text
UPDATE Language_de_DE
SET Text = 'Verdoppelt den religiösen Druck von dieser Stadt und gewährt 1 Stimme im Weltkongress für jede [COLOR_POSITIVE_TEXT]6. Stadt[ENDCOLOR] die dieser Relegion folgt. [NEWLINE][NEWLINE]Muss in einer Heiligen Stadt bebaut werden und mindestens eine [ICON_CITIZEN] landesweite Bevölkerung vom 35 bevor es gebaut werden kann. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP';

-- Text for NW Religion

UPDATE Language_de_DE
SET Text = 'Erhaltet [ICON_PEACE] Glauben wenn eine eigene Einheit im Kampf fällt. Der Bonus steigt mit jeder Era. Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_MAUSOLEUM_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_RELIQUARY_HELP';

UPDATE Language_de_DE
SET Text = '+15% Militäreinheiten [ICON_PRODUCTION] Produktion. Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_GREAT_ALTAR_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_HEAVENLY_THRONE_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_DIVINE_COURT_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_SACRED_GARDEN_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_HOLY_COUNCIL_HELP';

UPDATE Language_de_DE
SET Text = '+4 [ICON_GOLDEN_AGE] Punkte für ein Goldenes Zeitalter. Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_APOSTOLIC_PALACE_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bevölkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Religöse Spaltung und erlaubt Euch das Wählen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verstärkt den Druck der [ICON_RELIGION] religiösen Mehrheit die von dieser Stadt ausgeht um 25% und erhöht die Stadtresistenz gegenüber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress für jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_OSSUARY_HELP';

-- Sweden Trait 

--Sweden Trait (Bonus Text)
UPDATE Language_de_DE
SET Text = 'Alle Diplomatischen Einheiten starten mit der Nobelpreisträger Beförderung, welche +25% [ICON_INFLUENCE] Einfluss für Diplomatische Missionen gewährt. Wenn eine Freundschaftserklärung vereinbart wird, erlangt Schweden und sein Freund +10% auf die Generierung von Großen Persönlichkeiten.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE';
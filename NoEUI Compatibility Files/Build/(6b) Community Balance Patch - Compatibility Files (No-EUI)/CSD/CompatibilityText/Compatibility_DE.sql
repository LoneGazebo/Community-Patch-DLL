-- Policy Changes

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Patronat[ENDCOLOR] verbessert die Belohnungen von Stadtstaaten Freundschaften und globaler Diplomatie.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Patronat freischalten gew�hrt:[ENDCOLOR] [NEWLINE] [ICON_BULLET] [ICON_INFLUENCE] Einfluss bei Stadtstaaten um sinkt 25% langsamer als normalerweise. [NEWLINE] [ICON_BULLET] +5 [ICON_INFLUENCE] Einfluss auf die Basispunkte von Stadtstaaten. [NEWLINE] [ICON_BULLET] Schaltet die [COLOR_CYAN]Verbotene Stadt[ENDCOLOR] frei.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Jede Politik im Patronat-Baum gew�hrt: [ENDCOLOR] [NEWLINE] [ICON_BULLET] +3 [ICON_INFLUENCE] Einfluss auf die Basispunkte von Stadtstaaten.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Die �bernahme aller Sozialpolitiken im Patronat-Baum gew�hrt:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +10 [ICON_INFLUENCE] Einfluss bei allen bekannten Stadtstaaten, jedes Mal wenn Ihr eine [ICON_GREAT_PEOPLE] Gro�e Pers�nlichkeit verbraucht. [NEWLINE] [ICON_BULLET] Verb�ndete Stadtstaaten werden euch von Zeit zu Zeit [ICON_GREAT_PEOPLE] Gro�e Pers�nlichkeiten schenken.[NEWLINE][ICON_BULLET]Erm�glicht den Zugriff auf Renaissance-�ra Politik Zweige, Era Anforderung ignoriert.[NEWLINE] [ICON_BULLET] Gew�hrt [COLOR_CYAN]1[ENDCOLOR] Punkt (von [COLOR_CYAN]3[ENDCOLOR] ben�tigten insgesamt) f�r das Freischalten von Ideologien.[NEWLINE] [ICON_BULLET] Erlaubt au�erdem den Kauf von [ICON_DIPLOMAT] Gro�en Diplomaten mit [ICON_PEACE] Glauben, ab Beginn des Industriezeitalters.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP';

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Scholastik[ENDCOLOR][NEWLINE]Erhaltet Gro�e Diplomaten 25% schneller. Alle Stadtstaaten mit denen Ihr [COLOR_POSITIVE_TEXT]Verb�ndet[ENDCOLOR] seid, gew�hren einen [ICON_RESEARCH] Wissenschaft-Bonus in H�he von 33% ihrer eigenen Produktion.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP';
	
-- TEXT CHANGES

UPDATE Language_de_DE
SET Text = 'Setzt eine [ICON_CITIZEN] landesweite Bev�lkerung von mindestens 20 voraus, bevor es gebaut werden kann. +1 [ICON_RES_PAPER] Papier. +10% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Kann nur in der Hauptstadt gebaut werden. [NEWLINE][NEWLINE] +10% der [ICON_PRODUCTION] Stadtproduktion wird zur aktuellen [ICON_PRODUCTION] Produktion hinzugef�gt, jedes Mal wenn in der Stadt ein neuer [ICON_CITIZEN] B�rger geboren wird.'
WHERE Tag = 'TXT_KEY_BUILDING_COURT_SCRIBE_HELP';

UPDATE Language_de_DE
SET Text = 'Dieses Nationale Wunder is einzigartig, welches Spieler nur in ihrer Hauptstadt bauen d�rfen, so lang Ihr eine [ICON_CITIZEN] landesweite Bev�lkerung von 20 aufwei�t. Es gew�hrt einen kleinen Produktionsvorteil beim Bau von Diplomatischen Einheiten in der Hauptstadt und eine [ICON_RES_PAPER] Papierquelle. Baut dieses Geb�ude um Euch ein oder zwei Stadtstaaten B�ndnisse in den ersten Epochen zu sichern. Da es zus�tzlich [ICON_PRODUCTION] Produktion durch Stadtwachstum gew�hrt, ist es wichtig dieses Geb�ude fr�h zu bauen um den gr��tm�glichsten Vorteil aus diesem Geb�ude zu ziehen.'
WHERE Tag = 'TXT_KEY_BUILDING_COURT_SCRIBE_STRATEGY';

UPDATE Language_de_DE
SET Text = 'Dieses nationale Wunder kann erst gebaut werden, wenn die Stadt eine �ffentliche Schule besitzt und und Eure [ICON_CITIZEN] landesweite Bev�lkerung mindestens 70 betr�gt. Baut dieses Nationale Wunder um weitere Stimmen f�r den Weltkongress zu erhalten, basierend auf der Anzahl von aktuell vereinbarten Forschungsabkommen mit anderen Spielern. Ihr erhaltet au�erdem einen gro�z�gigen Bonus bei der Generierung von Kultur und Wissenschaft in der Stadt in der es gebaut wurde.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = 'Ben�tigt Ordnung. +15% [ICON_CULTURE] Kultur und [ICON_RESEARCH] Wissenschaft in der Stadt, in der er gebaut wurde. +2 [ICON_RES_PAPER] Papier. Erhaltet eine Stimme f�r jedes [COLOR_POSITIVE_TEXT]Forschungsabkommen[ENDCOLOR] das Ihr z.zt. mit anderen Spielern abgeschlossen habt. [NEWLINE][NEWLINE]Ben�tigt eine �ffentliche Schule in der Stadt und mindestens eine [ICON_CITIZEN] landesweite Bev�lkerung von 70 bevor es gebaut werden kann. Je mehr St�dte ein Reich hat, desto h�her sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_HELP';

UPDATE Language_de_DE
SET Text = 'Dieses nationale Wunder kann erst gebaut werden, wenn die Stadt eine Milit�rakademie besitzt und Eure [ICON_CITIZEN] landesweite Bev�lkerung mindestens 70 betr�gt. Baut dieses Wunder um weitere Stimmen im Weltkongress zu erhalten, basierend auf der Anzahl eroberter Hauptst�dte. Ihr erhaltet au�erdem einen Bonus bei der Generierung von Tourismus und Glauben in der Stadt in der es gebaut wurde.'
WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = 'Ben�tigt Autokratie. +15 [ICON_TOURISM] Tourismus und +15 [ICON_PEACE] Glauben in der Stadt, in der sie gebaut wurde. +2 [ICON_RES_PAPER] Papier. Erhaltet eine Stimme f�r jede [COLOR_POSITIVE_TEXT]Feindliche Hauptstadt[ENDCOLOR] unter Eurer Kontrolle. [NEWLINE][NEWLINE]Ben�tigt eine Milit�r Akademie in der Stadt und mindestens eine [ICON_CITIZEN] landesweite Bev�lkerung von 70 bevor es gebaut werden kann. Je mehr St�dte ein Reich hat, desto h�her sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_HELP';


UPDATE Language_de_DE
SET Text = 'Dieses nationale Wunder kann erst gebaut werden, wenn die Stadt eine B�rse besitzt und Eure [ICON_CITIZEN] landesweite Bev�lkerung mindestens 70 betr�gt. Baut dieses Wunder um weitere Stimmen im Weltkongress zu erhalten, basierend auf der Anzahl aktueller Freundschaftserkl�rung mit anderen Zivilisationen. Ihr erhaltet au�erdem einen Bonus bei der Generierung von Nahrung und Gold in der Stadt in der es gebaut wurde.'
WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = 'Ben�tigt Freiheit. +15% [ICON_FOOD] Nahrung und [ICON_GOLD] Gold in der Stadt, in der es gebaut wurde. +2 [ICON_RES_PAPER] Papier. Erhaltet eine Stimme f�r jede [COLOR_POSITIVE_TEXT]Freundschaftserkl�rung[ENDCOLOR] die Ihr z.zt. mit anderen Spielern vereinbart habt. [NEWLINE][NEWLINE]Ben�tigt eine B�rse in der Stadt und mindestens eine [ICON_CITIZEN] landesweite Bev�lkerung von 70 bevor es gebaut werden kann. Je mehr St�dte ein Reich hat, desto h�her sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_HELP';


UPDATE Language_de_DE
SET Text = 'Dieses Wunder kann erst gebaut werden, wenn die Stadt eine Kanzlei besitzt  und Eure [ICON_CITIZEN] landesweite Bev�lkerung mindestens 40 betr�gt. Baut es um ein Produktionsbonus auf Diplomatische Einheiten in der Stadt zu erhalten, in der sie gebaut wurde. Erh�ht Fortbewegung und Einfluss aller Diplomatischer Einheiten und gew�hrt die F�higkeit negative Gel�ndemodifikatoren zu ignorieren.'
WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = '+2 [ICON_RES_PAPER] Papier. +20% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Alle Diplomatischen Einheiten erhalten die Bef�rderung [COLOR_POSITIVE_TEXT]Bildung[ENDCOLOR].[NEWLINE][NEWLINE]Ben�tigt eine Kanzlei in der St�dten und mindestens eine [ICON_CITIZEN] landesweite Bev�lkerung vom 40 bevor es gebaut werden kann. Je mehr St�dte ein Reich hat, desto h�her sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_HELP';


UPDATE Language_de_DE
SET Text = 'Dieses Wunder kann erst gebaut werden, wenn die Stadt eine Presseagentur besitzt und Eure [ICON_CITIZEN] landesweite Bev�lkerung mindestens 60 betr�gt. Baue es um einen Produktionsbonus auf Diplomatische Einheiten in der Stadt zu erhalten, in der es gebaut wurde. Erh�ht Fortbewegung und Einfluss f�r alle Diplomatischen Einheiten und gew�hrt die F�higkeit Diplomatische Einheiten durch gegnerisches Gel�nde zu bewegen ohne [COLOR_POSITIVE_TEXT]Offene Grenzen[ENDCOLOR] vereinbart zu haben.'
WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_STRATEGY';

-- Help Text

UPDATE Language_de_DE
SET Text = '+2 [ICON_RES_PAPER] Papier. +20% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Alle Diplomatischen Einheiten erhalten die Bef�rderung [COLOR_POSITIVE_TEXT]Diplomatische Immunit�t[ENDCOLOR].[NEWLINE][NEWLINE]Ben�tigt Nachrichtendienst in der Stadt und mindestens eine landesweite Bev�lkerung vom 60 bevor es gebaut werden kann. Je mehr St�dte ein Reich hat, desto h�her sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_HELP';

-- Temple Help Text
UPDATE Language_de_DE
SET Text = 'Verdoppelt den religi�sen Druck von dieser Stadt und gew�hrt 1 Stimme im Weltkongress f�r jede [COLOR_POSITIVE_TEXT]6. Stadt[ENDCOLOR] die dieser Relegion folgt. [NEWLINE][NEWLINE]Muss in einer Heiligen Stadt bebaut werden und mindestens eine [ICON_CITIZEN] landesweite Bev�lkerung vom 35 bevor es gebaut werden kann. Je mehr St�dte ein Reich hat, desto h�her sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP';

-- Text for NW Religion

UPDATE Language_de_DE
SET Text = 'Erhaltet [ICON_PEACE] Glauben wenn eine eigene Einheit im Kampf f�llt. Der Bonus steigt mit jeder Era. Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_MAUSOLEUM_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_RELIQUARY_HELP';

UPDATE Language_de_DE
SET Text = '+15% Milit�reinheiten [ICON_PRODUCTION] Produktion. Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_GREAT_ALTAR_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_HEAVENLY_THRONE_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_DIVINE_COURT_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_SACRED_GARDEN_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_HOLY_COUNCIL_HELP';

UPDATE Language_de_DE
SET Text = '+4 [ICON_GOLDEN_AGE] Punkte f�r ein Goldenes Zeitalter. Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_APOSTOLIC_PALACE_HELP';

UPDATE Language_de_DE
SET Text = 'Darf nur in einer Heiligen Stadt gebaut werden und nur wenn mindestens 20% der weltweiten Bev�lkerung Eurer Religion folgt. Reduziert [ICON_HAPPINESS_3] Relig�se Spaltung und erlaubt Euch das W�hlen einer Glaubensverbesserung. [NEWLINE][NEWLINE]Verst�rkt den Druck der [ICON_RELIGION] religi�sen Mehrheit die von dieser Stadt ausgeht um 25% und erh�ht die Stadtresistenz gegen�ber Bekehrung um 20%. [NEWLINE][NEWLINE]Erhaltet 1 weiteren Delegierten im Weltkongress f�r jede 6. Stadt die Eurer Religion folgt.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_OSSUARY_HELP';

-- Sweden Trait 

--Sweden Trait (Bonus Text)
UPDATE Language_de_DE
SET Text = 'Alle Diplomatischen Einheiten starten mit der Nobelpreistr�ger Bef�rderung, welche +25% [ICON_INFLUENCE] Einfluss f�r Diplomatische Missionen gew�hrt. Wenn eine Freundschaftserkl�rung vereinbart wird, erlangt Schweden und sein Freund +10% auf die Generierung von Gro�en Pers�nlichkeiten.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE';
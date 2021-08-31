
--German Translation--
UPDATE Language_de_DE
SET Text = 'Verdoppelt religiösen Druck von dieser Stadt und gewährt 1 Stimme im Weltkongress für jede [COLOR_POSITIVE_TEXT]6. Stadt[ENDCOLOR] die dieser Relegion folgt. [NEWLINE][NEWLINE]Muss in einer Heiligen Stadt bebaut werden. Benötigt einen Tempel in allen Städten. Je mehr Städte ein Reich hat, desto höher sind die Kosten.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

--German Translation--

		-- Belief Text Changes

UPDATE Language_de_DE
SET Text = '+15 auf den Nullpunkt von [ICON_INFLUENCE] Einflusspunkten mit Stadtstaaten die dieser Relgion folgen.'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS';

UPDATE Language_de_DE
SET Text = 'Göttliches Recht'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS_SHORT';

UPDATE Language_de_DE
SET Text = 'Erhaltet 15 [ICON_PEACE] Glaube jedes Mal wenn eine Große Persönlichkeit verbraucht wird.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_BONUSES' AND Value= 1 );

--Great Merchant Influence Remover (Text)
UPDATE Language_de_DE
SET Text = 'Der Große Händler kann das spezielle Zollamt bauen. Wenn bewirtschaftet produziert es eine Menge [ICON_GOLD] Gold. Der Große Händler kann auch eine Reise zu einem Stadtstaat antreten und eine "Handelsmission" durchführen, welche haufenweise [ICON_GOLD] Gold produziert, aber keinen [ICON_INFLUENCE] Einfluss. Der Große Händler wird bei beiden Vorgehen verbraucht.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY';

--Sweden Trait (Bonus Text)
UPDATE Language_de_DE
SET Text = 'Alle Diplomatischen Einheiten starten mit der Nobelpreisträger Beförderung, welche +10% [ICON_INFLUENCE] Einfluss für Diplomatische Missionen gewährt. Wenn eine Freundschaftserklärung vereinbart wird, erlangt Schweden und sein Freund +10% auf die Generierung von Großen Persönlichkeiten.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE'
AND EXISTS (SELECT * FROM CSD WHERE Type='SWEDEN_TRAIT' AND Value = 1 );


UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Kulturelle Diplomatie[ENDCOLOR][NEWLINE] Erhaltet 1 [ICON_INFLUENCE] Einfluss pro Runde bei Stadtstaaten mit denen Ihr eine Handelsroute eingerichtet habt. Das [COLOR_POSITIVE_TEXT]Auswärtige Amt[ENDCOLOR] produziert +2 [ICON_CULTURE] Kultur.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Philanthropie[ENDCOLOR][NEWLINE]Menge an verschenkten Ressourcen von Stadtstaaten um 100% erhöht. [ICON_HAPPINESS] Zufriedenheit von geschenkten Luxusgütern um 50% erhöht. Die [COLOR_POSITIVE_TEXT]Kanzlei[ENDCOLOR] produziert 1 [ICON_HAPPINESS_1] Zufriedenheit.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Konsulat[ENDCOLOR][NEWLINE]Der Nullpunkt von [ICON_INFLUENCE] Einflusspunkten bei Stadtstaaten ist um 15 erhöht. Die [COLOR_POSITIVE_TEXT]Buchpresse[ENDCOLOR] verstärkt die [ICON_GOLD] Gold-Produktion um 10%.'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Handelsbund[ENDCOLOR][NEWLINE]Erhaltet Große Diplomaten 25% schneller und +3 [ICON_GOLD] Gold für Handelsrouten mit Stadtstaaten.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Scholastik[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Verbündete[ENDCOLOR] Stadtstaaten liefern einen [ICON_RESEARCH] Wissenschaft-Bonus in Höhe von 25% von ihrer eigenen Produktion. Der [COLOR_POSITIVE_TEXT]Nachrichtendienst[ENDCOLOR] verstärkt [ICON_RESEARCH] Wissenschaft Produktion um 5%.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value=1);

		-- Ideology Text Changes

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Einheitsfront[ENDCOLOR][NEWLINE]Erhaltet 2 mehr [ICON_INFLUENCE] Einfluss pro Runde (auf Standard Geschwindigkeit) bei Stadtstaaten mit denen Ihr eine Handelsroute eingerichtet habt.'
WHERE Tag = 'TXT_KEY_POLICY_TREATY_ORGANIZATION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Kanonenbootdiplomatie[ENDCOLOR][NEWLINE]Erhaltet 3 mehr [ICON_INFLUENCE] Einfluss pro Runde (auf Standard Geschwindigkeit) bei Stadtstaaten von denen Ihr Tribut verlangen könnt.  Eure Streitkräfte sind 50% effektiver beim Einschüchtern von Stadtstaaten.'
WHERE Tag = 'TXT_KEY_POLICY_GUNBOAT_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

-- Patronage Finisher

UPDATE Language_de_DE
SET Text = '[COLOR_POSITIVE_TEXT]Patronat[ENDCOLOR] verbessert die Belohnungen von Stadtstaaten Freundschaften.[NEWLINE][NEWLINE]Patronat wird den [ICON_INFLUENCE] Einfluss bei Stadtstaaten um 25% langsamer sinken lassen als normalerweise. Schaltet die Verbotene Stadt frei.[NEWLINE][NEWLINE]Der Abschluss aller Politiken im Patronat-Baum wird Stadtstaaten gelegentlich dazu veranlassen euch [ICON_GREAT_PEOPLE] Große Persönlichkeiten zu schenken. Es erlaubt außerdem das Kaufen von Großen Diplomaten mit [ICON_PEACE] Glauben mit Beginn des Industriezeitalters.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_POLICIES' AND Value= 1 );

--German Translation--

--Tourism Early Boosts

UPDATE Language_de_DE
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Broadcast Tower[ENDCOLOR], a building which greatly increases the [ICON_CULTURE] Cultural output of a city, and increases [ICON_TOURISM] Tourism output of all cities by 15%.'
WHERE Tag = 'TXT_KEY_TECH_RADIO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Helicopter Gunship[ENDCOLOR], a unit specialized in fighting enemy tanks, and increases [ICON_TOURISM] Tourism output of all cities by 15%.'
WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Greatly increases empire-wide [ICON_TOURISM] Tourism output.'
WHERE Tag = 'TXT_KEY_DOUBLE_TOURISM' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

--Resolution Changes


-- Sanctions 

UPDATE Language_de_DE
SET Text = 'Sanction City-States'
WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Sanctions placed on City-States'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_CITY_STATES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Sanctions placed on: {1_CivsList}'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_PLAYERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'No [ICON_INTERNATIONAL_TRADE] Trade Routes can be established with City-States. Warmonger penalty greatly reduced when declaring war on City-States or conquering cities owned by them.'
WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Sanction a Civilization'
WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'No [ICON_INTERNATIONAL_TRADE] Trade Routes can be established between the chosen Civilization and any other Civilization. Warmonger penalty greatly reduced when declaring war on the sanctioned Civilization or conquering cities owned by them.'
WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Peace

UPDATE Language_de_DE
SET Text = 'Global Peace Accords'
WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Unit maintenance costs are raised by 25% [ICON_GOLD] Gold.[ICON_VICTORY_DOMINATION] Warmonger penalties for capturing cities and declaring war are greatly increased, and [ICON_VICTORY_DOMINATION] Warmonger scores decay much more slowly than normal.'
WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '+{1_UnitMaintenancePercent}% [ICON_GOLD] Gold cost for Unit Maintenance. [ICON_VICTORY_DOMINATION] Warmonger penalties greatly increased'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_UNIT_MAINTENANCE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Religion

UPDATE Language_de_DE
SET Text = 'Designate a religion as the official World Religion.  Civilizations who have a majority of the cities in their empire following the religion get an additional Delegate. The founder of the religion and/or the controller of its Holy City receive 1 additional vote for every other Civilization following the religion. The religion spreads 25% faster, and its Holy City receives +50% [ICON_TOURISM] Tourism.  There can only be one World Religion at a time.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_RELIGION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Ideology

UPDATE Language_de_DE
SET Text = 'Designate an Ideology as the official World Ideology.  Civilizations following the Ideology get an additional Delegate, plus 1 Delegate for every other Civilization that follows the Ideology.  Public opinion in favor of that Ideology is increased for all Civilizations. Enables the [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] if the United Nations has been constructed. [NEWLINE][NEWLINE]Available once any Civilization discovers Radio.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_IDEOLOGY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Scholars in Residence

UPDATE Language_de_DE
SET Text = 'Civilizations research a technology 20% more quickly if it has been discovered by at least one other Civilization. This value is increased by an additional 5% for every City-State ally the Civilization currently maintains.'
WHERE Tag = 'TXT_KEY_RESOLUTION_MEMBER_DISCOVERED_TECH_DISCOUNT_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '-{1_ResearchCostPercent}% [ICON_RESEARCH] Research cost for Technologies already discovered by another Civilization. Additional -5% for every City-State ally'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_DISCOVERED_TECH_MODIFIER' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

--Wonder Culture

UPDATE Language_de_DE
SET Text = '+{1_CulturePerWorldWonder} [ICON_CULTURE] Culture from World Wonders, +1 [ICON_RESEARCH] Science from Great Works'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_WONDERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Each World Wonder provides +2 [ICON_CULTURE] Culture to its city, and each Great Work provides +1 [ICON_RESEARCH] Science to its city.'
WHERE Tag = 'TXT_KEY_RESOLUTION_CULTURAL_HERITAGE_SITES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Natural Wonder

UPDATE Language_de_DE
SET Text = 'Each Natural Wonder provides +2 to all Yields when worked.'
WHERE Tag = 'TXT_KEY_RESOLUTION_NATURAL_HERITAGE_SITES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '+{1_CulturePerWorldWonder} to yields from Natural Wonders'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_NATURAL_WONDERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Improvement/Landmarks

UPDATE Language_de_DE
SET Text = 'Great Person tile improvements provide +1 to [ICON_PRODUCTION] Production, [ICON_FOOD] Food, and [ICON_GOLD] Gold when worked, and Landmarks built by Archaeologists provide +1 [ICON_CULTURE] Culture, [ICON_RESEARCH] Science, and [ICON_PEACE] Faith when worked.[NEWLINE][NEWLINE]Available once any Civilization discovers Archaeology.'
WHERE Tag = 'TXT_KEY_RESOLUTION_HISTORICAL_LANDMARKS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '+{1_NumCulture} [ICON_PRODUCTION]/[ICON_FOOD]/[ICON_GOLD] from Great Person improvements'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_TILE_IMPROVEMENT_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '+{1_NumCulture} [ICON_CULTURE]/[ICON_RESEARCH]/[ICON_PEACE] from Landmarks'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_LANDMARK_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );


-- World's Fair

UPDATE Language_de_DE
SET Text = 'Free Social Policy.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '[ICON_CULTURE] Culture increases by 100% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Crystal Palace wonder appears in [ICON_CAPITAL] Capital. ({TXT_KEY_BUILDING_CRYSTAL_PALACE_HELP})'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Begins the World`s Fair project.  Once underway, Civilizations can contribute [ICON_PRODUCTION] Production towards its completion by selecting it in the city production list.  When complete, Civilizations receive bonuses based on how much they contributed.[NEWLINE][NEWLINE][ICON_TROPHY_GOLD]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP}[NEWLINE][ICON_TROPHY_SILVER]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP}[NEWLINE][ICON_TROPHY_BRONZE]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP} [NEWLINE][NEWLINE]Available once any Civilization discovers Industrialization.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_FAIR_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Games

UPDATE Language_de_DE
SET Text = '[ICON_TOURISM] Tourism increases by 100% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_1_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = ' Receive a free Great Person of your choice.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_2_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Olympic Village wonder appears in [ICON_CAPITAL] Capital. ({TXT_KEY_BUILDING_OLYMPIC_VILLAGE_HELP})'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Arts/Sciences Funding

UPDATE Language_de_DE
SET Text = 'Increases generation of Great Writers, Artists, and Musicians by 33%.  Decreases generation of Great Scientists, Engineers, and Merchants by 33%. [NEWLINE][NEWLINE]Civilizations with Scores [COLOR_POSITIVE_TEXT]below the global average[ENDCOLOR] will receive a boost to [ICON_PRODUCTION] Production and [ICON_CULTURE] Culture anywhere from 10-30%. [NEWLINE][NEWLINE]Details: The boost is the difference between the score of the Civilization and the leader, divided by 50.'
WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Endowment for the Arts'
WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Increases generation of Great Scientists, Engineers, and Merchants by 33%.  Decreases generation of Great Writers, Artists, and Musicians by 33%. [NEWLINE][NEWLINE]Civilizations with Scores [COLOR_POSITIVE_TEXT]below the global average[ENDCOLOR] will receive a boost to [ICON_FOOD] Food and [ICON_RESEARCH] Science anywhere from 10-30%. [NEWLINE][NEWLINE]Details: The boost is the difference between the score of the Civilization and the leader, divided by 50.'
WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'World Science Initiative'
WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Leader

UPDATE Language_de_DE
SET Text = 'Global Hegemony'
WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '{1_CivName} has attained Global Hegemony.'
WHERE Tag = 'TXT_KEY_VP_DIPLO_SOMEONE_WON' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Designate a player that could achieve Global Hegemony.  If a Civilization receives support from enough Delegates in the Congress, that leader will attain [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR].  The number of Delegates needed depends on the number of Civilizations and City-States in the game, and can be seen on the World Congress screen.[NEWLINE][NEWLINE]If no Civilization receives enough support to win, the two Civilizations who received the most support permanently gain an [COLOR_POSITIVE_TEXT]additional Delegate[ENDCOLOR] to use in future sessions.[NEWLINE][NEWLINE]May not be proposed by a Civilization.  Automatically proposed at regular intervals once a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active.'
WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Turns until attempt at Global Hegemony: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_SESSION' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Delegates needed for Global Hegemony: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_VOTES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Since the World Congress has moved to the newly-constructed United Nations, Diplomatic Victory is now possible.  To enable the Diplomatic Victory special sessions, a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] must be active. Once active, a Civilization must receive the support of {1_DelegatesForWin} or more Delegates on a Global Hegemony proposal.  This number is based on the number of Civilizations and City-States in the game.[NEWLINE][NEWLINE]The Global Hegemony proposal is made to the Congress every other session once a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active, alternating with regular sessions of regular proposals.'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '[NEWLINE][ICON_BULLET]{1_NumVotes} from previous attmepts at Global Hegemony'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_EXTRA_VOTES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]Global Hegemony[ENDCOLOR] proposal on alternating sessions if a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active'
WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_DIPLO_VICTORY_POSSIBLE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '{@1_CivName} achieves Global Hegemony'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Global Hegemony not achieved'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Garnering the required support of at least {1_NumDelegates} {1_NumDelegates: plural 1?Delegate; other?Delegates;}, {@2_CivName} has achieved Global Hegemony.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS_DETAILS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Without a clear winner garnering the support of at least {1_NumDelegates} {1_NumDelegates: plural 1?Delegate; other?Delegates;}, the Global Hegemony proposal fails. The top {2_NumCivilizations} {2_NumCivilizations: plural 1?Civilization has; other?Civilizations have;} permanently gained an additional Delegate.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL_DETAILS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'The World Congress is reborn as the United Nations.  A special session to is called to choose the host of this powerful new diplomatic force.  The United Nations will continue as before, but regular sessions will alternate with sessions allowing civilizations to try and claim Global Hegemony, making [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] possible. To enable these sessions, a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] must be active.'
WHERE Tag = 'TXT_KEY_LEAGUE_SPECIAL_SESSION_START_UNITED_NATIONS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Diplomacy[NEWLINE][NEWLINE]Turns until the United Nations convenes: {1_TurnsUntilSession}[NEWLINE][NEWLINE]Turns until the next [COLOR_POSITIVE_TEXT]Global Hegemony[ENDCOLOR] proposal: {2_TurnsUntilVictorySession}[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] is now possible, if a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active.'
WHERE Tag = 'TXT_KEY_EO_DIPLOMACY_AND_VICTORY_SESSION' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'The world has entered the {@1_EraName}, either because of technological advances or completed resolutions.'
WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_GAME_ERA' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Wie wirken sich Diplomatische Missionen auf unsere Beziehung aus?'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_ADV_QUEST' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Die effektivste Methode Euren Einfluss in einem Stadtstaat zu erhöhen ist das Ausbilden von Diplomatischen Einheiten und sie auf eine Diplomatische Mission zu schicken. Eine Diplomatische Mission erlaubt Diplomatischen Einheiten Euren Einfluss mit Stadtstaaten sofort zu erhöhen. Um eine Diplomatische Mission durchzuführen, bewegt Ihr eure Einheit in das Gelände eines Stadtstaates und drückt den Diplomatische Mission Knopf. Die Basismenge an Einfluss von einer Diplomatischen Mission steigt mit dem Freischalten von neuen Beförderungen, Einheiten, Wunder und Gebäuden. [ENDLINE][ENDLINE]Spieler dürfen nur eine begrenzte Anzahl an Diplomatischen Einheiten gleichzeitig besitzen. Die maximale Anzahl an Diplomatischen Einheiten steigt mit der Menge an [ICON_RES_PAPER] Papier die der Spieler unter seiner Kontrolle hält. Durch einmalige Benutzung der Einheit wird diese verbraucht und eine neue Einheit muss gebaut werden.'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Diplomatische Missionen und Ihr'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Diplomatische Missionen'
WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Ihr habt etwas zum Missfallen von {1_CityStateName:textkey} getan. Vielleicht seid Ihr durch ihre Grenzen geschritten oder habt kürzlich den Krieg erklärt.[NEWLINE][NEWLINE]Euer [ICON_INFLUENCE] Einfluss erholt sich um {2_Num} pro Runde. Ihr solltet eine Diplomatische Einheit zu ihnen schicken um die Erholung Eures [ICON_INFLUENCE] Einflusses zu beschleunigen.'
WHERE Tag = 'TXT_KEY_ANGRY_CSTATE_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = '{1_CityStateName:textkey} mag noch hasst Euch. Ihr solltet eine Diplomatische Einheit zu ihnen senden um Euren [ICON_INFLUENCE] Einfluss zu erhöhen.'
WHERE Tag = 'TXT_KEY_NEUTRAL_CSTATE_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Diplomatische Mission durchführen'
WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Wenn sich die Einheit innerhalb des Geländes des Stadtstaates befindet und Ihr euch mit ihnen nicht im Krieg befindet, wird dieser Befehl die Einheit eine Diplomatische Mission durchführen lassen und Euch eine große Menge an [ICON_INFLUENCE] Einfluss in diesem Stadtstaat gewähren. Wenn Ihr den Großen Händler auf dem gleichen Wege benutzt, wird er ebenfalls eine große Menge an [ICON_GOLD] Gold gewähren. Diese Aktion verbraucht die Einheit.'
WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Ihr gewannt {1_Num} [ICON_GOLD] Gold und {2_Num} [ICON_INFLUENCE] Einfluss durch diese Diplomatische Mission!'
WHERE Tag = 'TXT_KEY_MERCHANT_RESULT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Ihr könnt eine der folgenden Diplomatischen Missionen durchführen.'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_GOLD_GIFT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Ihr dürft kein [ICON_GOLD] Gold an Stadtstaaten verschenken. Sendet stattdessen eine Diplomatische Einheit.'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_GOLD_GIFT_CANT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_de_DE
SET Text = 'Ihr dürft eine Diplomatische Mission durchführen, die Euren [ICON_INFLUENCE] Einfluss mit diesem Stadtstaat verbessern kann.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

INSERT INTO Language_de_DE (
  Tag, Text)
SELECT 'TXT_KEY_BUILDING_COURT_SCRIBE_HELP', '+1 [ICON_RES_PAPER] Papier. +10% [ICON_PRODUCTION] Produktion auf Diplomatische Einheiten. Kann nur in der Hauptstadt gebaut werden. [NEWLINE][NEWLINE] +15% der [ICON_PRODUCTION] Stadtproduktion wird zur aktuellen [ICON_PRODUCTION] Produktion hinzugefügt, jedes Mal wenn die Stadt einen neuen [ICON_CITIZEN] Bürger erhält.';

INSERT INTO Language_de_DE (
  Tag, Text)
SELECT 'TXT_KEY_BUILDING_COURT_SCRIBE_STRATEGY', 'Dieses Nationale Wunder is einzigartig, welches Spieler in ihrer Hauptstadt ohne weitere Vorraussetzungen bauen können. Es gewährt einen kleinen Produktionsvorteil beim Bau von Diplomatischen Einheiten in der Hauptstadt und eine [ICON_RES_PAPER] Papierquelle. Baut dieses Gebäude um Euch ein oder zwei Stadtstaaten Bündnisse in den ersten Epochen zu sichern. Da es zusätzlich [ICON_PRODUCTION] Produktion durch Stadtwachstum gewährt, ist es wichtig dieses Gebäude früh zu bauen um den größtmöglichsten Vorteil aus diesem Gebäude zu ziehen.';

INSERT INTO Language_de_DE (
  Tag, Text)
SELECT 'TXT_KEY_BUILDING_CHANCERY_HELP', '+1 [ICON_RES_PAPER] Papier. +10% [ICON_PRODUCTION] Produktion von Diplomatischen Einheiten. Gewährt die Beförderung [COLOR_POSITIVE_TEXT]Königliches Siegel[ENDCOLOR] für Diplomatische Einheiten in der Stadt in der sie gebaut wurden. [NEWLINE][NEWLINE] +15% der [ICON_PRODUCTION] Stadtproduktion wird zur aktuellen [ICON_PRODUCTION] Produktion hinzugefügt, jedes Mal wenn die Stadt einen neuen [ICON_CITIZEN] Bürger erhält.';

INSERT INTO Language_de_DE (
  Tag, Text)
SELECT 'TXT_KEY_BUILDING_CHANCERY_STRATEGY', 'Erhöht die Produktionsgeschwindigkeit von Diplomatischen Einheiten und erhöht den gewährten Einluss von Diplomatischen Einheiten in der Stadt in der sie gebaut wurden. Der Spezialist Beamte bietet ein wenig [ICON_CULTURE] Kultur, [ICON_RESEARCH] Wissenschaft und [ICON_GOLD] Gold, im Austausch für [ICON_FOOD] Nahrung und [ICON_PRODUCTION] Produktion. Da es zusätzlich [ICON_PRODUCTION] Produktion durch Stadtwachstum gewährt, ist es wichtig dieses Gebäude früh zu bauen um den größtmöglichsten Vorteil aus diesem Gebäude zu ziehen.';

INSERT INTO Language_de_DE (
  Tag, Text)
SELECT 'TXT_KEY_BUILDING_WIRE_SERVICE_STRATEGY', 'Erhöht die Produktionsgeschwindigkeit von Diplomatischen Einheiten und erhöht den gewährten Einluss von Diplomatischen Einheiten in der Stadt in der sie gebaut wurden. Der Spezialist Beamte bietet ein wenig [ICON_CULTURE] Kultur, [ICON_RESEARCH] Wissenschaft und [ICON_GOLD] Gold, im Austausch für [ICON_FOOD] Nahrung und [ICON_PRODUCTION] Produktion. Da es zusätzlich [ICON_PRODUCTION] Produktion durch Stadtwachstum gewährt, ist es wichtig dieses Gebäude früh zu bauen um den größtmöglichsten Vorteil aus diesem Gebäude zu ziehen.';

INSERT INTO Language_de_DE (
  Tag, Text)
SELECT 'TXT_KEY_BUILDING_WIRE_SERVICE_HELP', '+1 [ICON_RES_PAPER] Papier. +10% [ICON_PRODUCTION] Produktion von Diplomatischen Einheiten. Gewährt die Beförderung [COLOR_POSITIVE_TEXT]Nachrichtendienst[ENDCOLOR] für Diplomatische Einheiten in der Stadt in der sie gebaut wurden. [NEWLINE][NEWLINE] +15% der [ICON_PRODUCTION] Stadtproduktion wird zur aktuellen [ICON_PRODUCTION] Produktion hinzugefügt, jedes Mal wenn die Stadt einen neuen [ICON_CITIZEN] Bürger erhält.';


--Halicarnassus Fix German
UPDATE Language_de_DE
SET Text = 'Leitet ein [ICON_GOLDEN_AGE] Goldenes Zeitalter im Reich ein. Erhaltet 20 [ICON_GOLD] Gold, jedes Mal wenn eine Große Persönlichkeit aufgebraucht wird. Jede Quelle [ICON_RES_MARBLE] Marmor oder [ICON_RES_STONE] Stein die von dieser Stadt bewirtschaftet wird, produziert +2 [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP'
AND EXISTS (SELECT * FROM CSD WHERE Type='ANCIENT_WONDERS' AND Value= 1 );

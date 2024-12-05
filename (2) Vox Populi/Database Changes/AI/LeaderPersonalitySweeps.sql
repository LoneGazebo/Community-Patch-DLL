-- Reset these values to avoid carryover from the Community Patch / base game
UPDATE Leaders SET PrimaryVictoryPursuit = NULL, SecondaryVictoryPursuit = NULL;
UPDATE Leader_MinorCivApproachBiases SET Bias = -1 WHERE MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
DELETE FROM Leader_MajorCivApproachBiases;
DELETE FROM Leader_MinorCivApproachBiases WHERE MinorCivApproachType <> 'MINOR_CIV_APPROACH_FRIENDLY';

-- Ahmad al-Mansur (Morocco)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 7,
	Boldness = 6,
	DiploBalance = 7,
	WarmongerHate = 5,
	DoFWillingness = 7,
	DenounceWillingness = 6,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 6,
	Loyalty = 4,
	Forgiveness = 5,
	Neediness = 6,
	Meanness = 5
WHERE Type = 'LEADER_AHMAD_ALMANSUR';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_WAR', 3),
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_DECEPTIVE', 9),
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_NEUTRAL', 4),
	('LEADER_AHMAD_ALMANSUR', 'MAJOR_CIV_APPROACH_FRIENDLY', 7);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_AHMAD_ALMANSUR', 'MINOR_CIV_APPROACH_IGNORE', 4),
	('LEADER_AHMAD_ALMANSUR', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_AHMAD_ALMANSUR', 'MINOR_CIV_APPROACH_BULLY', 6),
	('LEADER_AHMAD_ALMANSUR', 'MINOR_CIV_APPROACH_CONQUEST', 3);

-- Alexander (Greece)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 3,
	MinorCivCompetitiveness = 10,
	Boldness = 10,
	DiploBalance = 7,
	WarmongerHate = 2,
	DoFWillingness = 5,
	DenounceWillingness = 7,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 7,
	Loyalty = 7,
	Forgiveness = 4,
	Neediness = 3,
	Meanness = 8
WHERE Type = 'LEADER_ALEXANDER';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_WAR', 9),
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_HOSTILE', 7),
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_DECEPTIVE', 2),
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_AFRAID', 1),
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_NEUTRAL', 4),
	('LEADER_ALEXANDER', 'MAJOR_CIV_APPROACH_FRIENDLY', 7);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ALEXANDER', 'MINOR_CIV_APPROACH_IGNORE', 1),
	('LEADER_ALEXANDER', 'MINOR_CIV_APPROACH_PROTECTIVE', 9),
	('LEADER_ALEXANDER', 'MINOR_CIV_APPROACH_BULLY', 2),
	('LEADER_ALEXANDER', 'MINOR_CIV_APPROACH_CONQUEST', 6);

-- Ashurbanipal (Assyria)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 5,
	MinorCivCompetitiveness = 2,
	Boldness = 8,
	DiploBalance = 10,
	WarmongerHate = 7,
	DoFWillingness = 3,
	DenounceWillingness = 6,
	WorkWithWillingness = 3,
	WorkAgainstWillingness = 6,
	Loyalty = 3,
	Forgiveness = 5,
	Neediness = 9,
	Meanness = 8
WHERE Type = 'LEADER_ASHURBANIPAL';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_WAR', 8),
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_HOSTILE', 8),
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_ASHURBANIPAL', 'MAJOR_CIV_APPROACH_FRIENDLY', 3);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ASHURBANIPAL', 'MINOR_CIV_APPROACH_IGNORE', 6),
	('LEADER_ASHURBANIPAL', 'MINOR_CIV_APPROACH_PROTECTIVE', 3),
	('LEADER_ASHURBANIPAL', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_ASHURBANIPAL', 'MINOR_CIV_APPROACH_CONQUEST', 9);

-- Askia (Songhai)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 4,
	Boldness = 7,
	DiploBalance = 4,
	WarmongerHate = 7,
	DoFWillingness = 4,
	DenounceWillingness = 7,
	WorkWithWillingness = 4,
	WorkAgainstWillingness = 7,
	Loyalty = 1,
	Forgiveness = 3,
	Neediness = 6,
	Meanness = 9
WHERE Type = 'LEADER_ASKIA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_WAR', 8),
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 9),
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_ASKIA', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ASKIA', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_ASKIA', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_ASKIA', 'MINOR_CIV_APPROACH_BULLY', 9),
	('LEADER_ASKIA', 'MINOR_CIV_APPROACH_CONQUEST', 7);

-- Attila (The Huns)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 4,
	MinorCivCompetitiveness = 4,
	Boldness = 9,
	DiploBalance = -1,
	WarmongerHate = 2,
	DoFWillingness = 3,
	DenounceWillingness = 5,
	WorkWithWillingness = 3,
	WorkAgainstWillingness = 5,
	Loyalty = 3,
	Forgiveness = 8,
	Neediness = 10,
	Meanness = 10
WHERE Type = 'LEADER_ATTILA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_WAR', 9),
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_HOSTILE', 9),
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_GUARDED', 3),
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_NEUTRAL', 3),
	('LEADER_ATTILA', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ATTILA', 'MINOR_CIV_APPROACH_IGNORE', 4),
	('LEADER_ATTILA', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_ATTILA', 'MINOR_CIV_APPROACH_BULLY', 9),
	('LEADER_ATTILA', 'MINOR_CIV_APPROACH_CONQUEST', 7);

-- Augustus Caesar (Rome)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 8,
	MinorCivCompetitiveness = 2,
	Boldness = 7,
	DiploBalance = 3,
	WarmongerHate = 5,
	DoFWillingness = 2,
	DenounceWillingness = 2,
	WorkWithWillingness = 2,
	WorkAgainstWillingness = 2,
	Loyalty = 3,
	Forgiveness = 1,
	Neediness = 4,
	Meanness = 8
WHERE Type = 'LEADER_AUGUSTUS';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_WAR', 9),
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_HOSTILE', 3),
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_DECEPTIVE', 1),
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_AUGUSTUS', 'MAJOR_CIV_APPROACH_FRIENDLY', 2);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_AUGUSTUS', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_AUGUSTUS', 'MINOR_CIV_APPROACH_PROTECTIVE', 2),
	('LEADER_AUGUSTUS', 'MINOR_CIV_APPROACH_BULLY', 3),
	('LEADER_AUGUSTUS', 'MINOR_CIV_APPROACH_CONQUEST', 12);

-- Bismarck (Germany)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 5,
	MinorCivCompetitiveness = 9,
	Boldness = 5,
	DiploBalance = 9,
	WarmongerHate = 8,
	DoFWillingness = 8,
	DenounceWillingness = 5,
	WorkWithWillingness = 8,
	WorkAgainstWillingness = 5,
	Loyalty = 6,
	Forgiveness = 8,
	Neediness = 5,
	Meanness = 9
WHERE Type = 'LEADER_BISMARCK';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_WAR', 6),
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_GUARDED', 8),
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_NEUTRAL', 10),
	('LEADER_BISMARCK', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_BISMARCK', 'MINOR_CIV_APPROACH_IGNORE', 1),
	('LEADER_BISMARCK', 'MINOR_CIV_APPROACH_PROTECTIVE', 10),
	('LEADER_BISMARCK', 'MINOR_CIV_APPROACH_BULLY', 2),
	('LEADER_BISMARCK', 'MINOR_CIV_APPROACH_CONQUEST', 4);

-- Boudicca (The Celts); generalist civ with no victory pursuit bias
UPDATE Leaders
SET
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 4,
	Boldness = 8,
	DiploBalance = 6,
	WarmongerHate = 9,
	DoFWillingness = 6,
	DenounceWillingness = 6,
	WorkWithWillingness = 6,
	WorkAgainstWillingness = 6,
	Loyalty = 6,
	Forgiveness = 6,
	Neediness = 7,
	Meanness = 5
WHERE Type = 'LEADER_BOUDICCA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_WAR', 7),
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_HOSTILE', 7),
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_AFRAID', 2),
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_BOUDICCA', 'MAJOR_CIV_APPROACH_FRIENDLY', 4);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_BOUDICCA', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_BOUDICCA', 'MINOR_CIV_APPROACH_PROTECTIVE', 5),
	('LEADER_BOUDICCA', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_BOUDICCA', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Casimir III (Poland); generalist civ with no victory pursuit bias
UPDATE Leaders
SET
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 4,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 5,
	Boldness = 5,
	DiploBalance = 8,
	WarmongerHate = 7,
	DoFWillingness = 5,
	DenounceWillingness = 7,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 7,
	Loyalty = 9,
	Forgiveness = 2,
	Neediness = 8,
	Meanness = 5
WHERE Type = 'LEADER_CASIMIR';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_WAR', 4),
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_HOSTILE', 6),
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_GUARDED', 9),
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_CASIMIR', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_CASIMIR', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_CASIMIR', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_CASIMIR', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_CASIMIR', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Catherine (Russia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 5,
	MinorCivCompetitiveness = 6,
	Boldness = 8,
	DiploBalance = 3,
	WarmongerHate = 4,
	DoFWillingness = 6,
	DenounceWillingness = 9,
	WorkWithWillingness = 6,
	WorkAgainstWillingness = 9,
	Loyalty = 7,
	Forgiveness = 6,
	Neediness = 9,
	Meanness = 9
WHERE Type = 'LEADER_CATHERINE';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_WAR', 7),
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_HOSTILE', 7),
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_DECEPTIVE', 2),
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_NEUTRAL', 3),
	('LEADER_CATHERINE', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_CATHERINE', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_CATHERINE', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_CATHERINE', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_CATHERINE', 'MINOR_CIV_APPROACH_CONQUEST', 6);

-- Darius I (Persia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 8,
	MinorCivCompetitiveness = 3,
	Boldness = 6,
	DiploBalance = 6,
	WarmongerHate = 4,
	DoFWillingness = 5,
	DenounceWillingness = 6,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 6,
	Loyalty = 5,
	Forgiveness = 5,
	Neediness = 5,
	Meanness = 4
WHERE Type = 'LEADER_DARIUS';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_WAR', 7),
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_GUARDED', 8),
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_AFRAID', 6),
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_NEUTRAL', 7),
	('LEADER_DARIUS', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_DARIUS', 'MINOR_CIV_APPROACH_IGNORE', 6),
	('LEADER_DARIUS', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_DARIUS', 'MINOR_CIV_APPROACH_BULLY', 4),
	('LEADER_DARIUS', 'MINOR_CIV_APPROACH_CONQUEST', 10);

-- Dido (Carthage)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 4,
	MinorCivCompetitiveness = 8,
	Boldness = 8,
	DiploBalance = 6,
	WarmongerHate = 6,
	DoFWillingness = 6,
	DenounceWillingness = 4,
	WorkWithWillingness = 6,
	WorkAgainstWillingness = 4,
	Loyalty = 8,
	Forgiveness = 3,
	Neediness = 8,
	Meanness = 6
WHERE Type = 'LEADER_DIDO';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_WAR', 7),
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_DIDO', 'MAJOR_CIV_APPROACH_FRIENDLY', 7);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_DIDO', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_DIDO', 'MINOR_CIV_APPROACH_PROTECTIVE', 8),
	('LEADER_DIDO', 'MINOR_CIV_APPROACH_BULLY', 6),
	('LEADER_DIDO', 'MINOR_CIV_APPROACH_CONQUEST', 4);

-- Elizabeth (England)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 5,
	MinorCivCompetitiveness = 8,
	Boldness = 7,
	DiploBalance = 10,
	WarmongerHate = 5,
	DoFWillingness = 6,
	DenounceWillingness = 8,
	WorkWithWillingness = 6,
	WorkAgainstWillingness = 8,
	Loyalty = 6,
	Forgiveness = 4,
	Neediness = 7,
	Meanness = 8
WHERE Type = 'LEADER_ELIZABETH';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_WAR', 6),
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_HOSTILE', 8),
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_DECEPTIVE', 8),
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_GUARDED', 10),
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_NEUTRAL', 7),
	('LEADER_ELIZABETH', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ELIZABETH', 'MINOR_CIV_APPROACH_IGNORE', 2),
	('LEADER_ELIZABETH', 'MINOR_CIV_APPROACH_PROTECTIVE', 9),
	('LEADER_ELIZABETH', 'MINOR_CIV_APPROACH_BULLY', 4),
	('LEADER_ELIZABETH', 'MINOR_CIV_APPROACH_CONQUEST', 7);

-- Enrico Dandolo (Venice)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 3,
	WonderCompetitiveness = 10,
	MinorCivCompetitiveness = 10,
	Boldness = 2,
	DiploBalance = 10,
	WarmongerHate = 12,
	DoFWillingness = 10,
	DenounceWillingness = 7,
	WorkWithWillingness = 10,
	WorkAgainstWillingness = 7,
	Loyalty = 5,
	Forgiveness = 7,
	Neediness = 7,
	Meanness = 3
WHERE Type = 'LEADER_ENRICO_DANDOLO';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_WAR', 2),
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_HOSTILE', 3),
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_DECEPTIVE', 10),
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_AFRAID', 7),
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_ENRICO_DANDOLO', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ENRICO_DANDOLO', 'MINOR_CIV_APPROACH_IGNORE', 1),
	('LEADER_ENRICO_DANDOLO', 'MINOR_CIV_APPROACH_PROTECTIVE', 10),
	('LEADER_ENRICO_DANDOLO', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_ENRICO_DANDOLO', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Gajah Mada (Indonesia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 4,
	MinorCivCompetitiveness = 5,
	Boldness = 8,
	DiploBalance = 4,
	WarmongerHate = 4,
	DoFWillingness = 7,
	DenounceWillingness = 6,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 6,
	Loyalty = 10,
	Forgiveness = 7,
	Neediness = 3,
	Meanness = 9
WHERE Type = 'LEADER_GAJAH_MADA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_WAR', 7),
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_HOSTILE', 10),
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 4),
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_AFRAID', 2),
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_GAJAH_MADA', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_GAJAH_MADA', 'MINOR_CIV_APPROACH_IGNORE', 4),
	('LEADER_GAJAH_MADA', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_GAJAH_MADA', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_GAJAH_MADA', 'MINOR_CIV_APPROACH_CONQUEST', 8);

-- Gandhi (India)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 4,
	WonderCompetitiveness = 9,
	MinorCivCompetitiveness = 6,
	Boldness = 5,
	DiploBalance = 8,
	WarmongerHate = 12,
	DoFWillingness = 10,
	DenounceWillingness = 8,
	WorkWithWillingness = 10,
	WorkAgainstWillingness = 8,
	Loyalty = 10,
	Forgiveness = 7,
	Neediness = 5,
	Meanness = 4
WHERE Type = 'LEADER_GANDHI';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_WAR', 3),
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_GUARDED', 8),
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_AFRAID', 1),
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_GANDHI', 'MAJOR_CIV_APPROACH_FRIENDLY', 10);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_GANDHI', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_GANDHI', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_GANDHI', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_GANDHI', 'MINOR_CIV_APPROACH_CONQUEST', 4);

-- Genghis Khan (Mongolia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 10,
	WonderCompetitiveness = 1,
	MinorCivCompetitiveness = 1,
	Boldness = 10,
	DiploBalance = 4,
	WarmongerHate = 3,
	DoFWillingness = 7,
	DenounceWillingness = 4,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 4,
	Loyalty = 8,
	Forgiveness = 1,
	Neediness = 8,
	Meanness = 10
WHERE Type = 'LEADER_GENGHIS_KHAN';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_WAR', 9),
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_HOSTILE', 9),
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_DECEPTIVE', 1),
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_NEUTRAL', 3),
	('LEADER_GENGHIS_KHAN', 'MAJOR_CIV_APPROACH_FRIENDLY', 7);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_GENGHIS_KHAN', 'MINOR_CIV_APPROACH_IGNORE', 1),
	('LEADER_GENGHIS_KHAN', 'MINOR_CIV_APPROACH_PROTECTIVE', 3),
	('LEADER_GENGHIS_KHAN', 'MINOR_CIV_APPROACH_BULLY', 12),
	('LEADER_GENGHIS_KHAN', 'MINOR_CIV_APPROACH_CONQUEST', 6);

-- Gustavus Adolphus (Sweden)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 4,
	Boldness = 10,
	DiploBalance = 2,
	WarmongerHate = 4,
	DoFWillingness = 2,
	DenounceWillingness = 6,
	WorkWithWillingness = 2,
	WorkAgainstWillingness = 6,
	Loyalty = 2,
	Forgiveness = 4,
	Neediness = 7,
	Meanness = 8
WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_WAR', 10),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_HOSTILE', 7),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_GUARDED', 4),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_AFRAID', 1),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MAJOR_CIV_APPROACH_FRIENDLY', 2);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_GUSTAVUS_ADOLPHUS', 'MINOR_CIV_APPROACH_IGNORE', 7),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MINOR_CIV_APPROACH_PROTECTIVE', 6),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_GUSTAVUS_ADOLPHUS', 'MINOR_CIV_APPROACH_CONQUEST', 10);

-- Haile Selassie (Ethiopia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 2,
	WonderCompetitiveness = 7,
	MinorCivCompetitiveness = 6,
	Boldness = 5,
	DiploBalance = 7,
	WarmongerHate = 8,
	DoFWillingness = 6,
	DenounceWillingness = 6,
	WorkWithWillingness = 6,
	WorkAgainstWillingness = 6,
	Loyalty = 5,
	Forgiveness = 5,
	Neediness = 5,
	Meanness = 5
WHERE Type = 'LEADER_SELASSIE';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_SELASSIE', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_SELASSIE', 'MINOR_CIV_APPROACH_IGNORE', 6),
	('LEADER_SELASSIE', 'MINOR_CIV_APPROACH_PROTECTIVE', 6),
	('LEADER_SELASSIE', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_SELASSIE', 'MINOR_CIV_APPROACH_CONQUEST', 3);

-- Harald Bluetooth (Denmark)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 5,
	Boldness = 10,
	DiploBalance = 5,
	WarmongerHate = 1,
	DoFWillingness = 8,
	DenounceWillingness = 5,
	WorkWithWillingness = 8,
	WorkAgainstWillingness = 5,
	Loyalty = 3,
	Forgiveness = 4,
	Neediness = 8,
	Meanness = 9
WHERE Type = 'LEADER_HARALD';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_WAR', 10),
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_HOSTILE', 9),
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_GUARDED', 3),
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_AFRAID', 2),
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_NEUTRAL', 3),
	('LEADER_HARALD', 'MAJOR_CIV_APPROACH_FRIENDLY', 7);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_HARALD', 'MINOR_CIV_APPROACH_IGNORE', 2),
	('LEADER_HARALD', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_HARALD', 'MINOR_CIV_APPROACH_BULLY', 6),
	('LEADER_HARALD', 'MINOR_CIV_APPROACH_CONQUEST', 9);

-- Harun al-Rashid (Arabia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 3,
	WonderCompetitiveness = 10,
	MinorCivCompetitiveness = 7,
	Boldness = 5,
	DiploBalance = 9,
	WarmongerHate = 9,
	DoFWillingness = 8,
	DenounceWillingness = 8,
	WorkWithWillingness = 8,
	WorkAgainstWillingness = 8,
	Loyalty = 3,
	Forgiveness = 4,
	Neediness = 8,
	Meanness = 7
WHERE Type = 'LEADER_HARUN_AL_RASHID';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_HOSTILE', 7),
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_DECEPTIVE', 10),
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_GUARDED', 9),
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_AFRAID', 7),
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_HARUN_AL_RASHID', 'MAJOR_CIV_APPROACH_FRIENDLY', 10);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_HARUN_AL_RASHID', 'MINOR_CIV_APPROACH_IGNORE', 4),
	('LEADER_HARUN_AL_RASHID', 'MINOR_CIV_APPROACH_PROTECTIVE', 8),
	('LEADER_HARUN_AL_RASHID', 'MINOR_CIV_APPROACH_BULLY', 3),
	('LEADER_HARUN_AL_RASHID', 'MINOR_CIV_APPROACH_CONQUEST', 1);

-- Hiawatha (The Iroquois)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 2,
	MinorCivCompetitiveness = 8,
	Boldness = 7,
	DiploBalance = 6,
	WarmongerHate = 7,
	DoFWillingness = 8,
	DenounceWillingness = 7,
	WorkWithWillingness = 8,
	WorkAgainstWillingness = 7,
	Loyalty = 9,
	Forgiveness = 10,
	Neediness = 3,
	Meanness = 8
WHERE Type = 'LEADER_HIAWATHA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_WAR', 6),
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 3),
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_AFRAID', 5),
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_NEUTRAL', 7),
	('LEADER_HIAWATHA', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_HIAWATHA', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_HIAWATHA', 'MINOR_CIV_APPROACH_PROTECTIVE', 8),
	('LEADER_HIAWATHA', 'MINOR_CIV_APPROACH_BULLY', 3),
	('LEADER_HIAWATHA', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Isabella (Spain)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 5,
	MinorCivCompetitiveness = 7,
	Boldness = 9,
	DiploBalance = 4,
	WarmongerHate = 4,
	DoFWillingness = 4,
	DenounceWillingness = 8,
	WorkWithWillingness = 4,
	WorkAgainstWillingness = 8,
	Loyalty = 6,
	Forgiveness = 4,
	Neediness = 8,
	Meanness = 8
WHERE Type = 'LEADER_ISABELLA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_WAR', 7),
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 3),
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_AFRAID', 2),
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_NEUTRAL', 3),
	('LEADER_ISABELLA', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ISABELLA', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_ISABELLA', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_ISABELLA', 'MINOR_CIV_APPROACH_BULLY', 7),
	('LEADER_ISABELLA', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Kamehameha (Polynesia)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 7,
	MinorCivCompetitiveness = 7,
	Boldness = 7,
	DiploBalance = 8,
	WarmongerHate = 7,
	DoFWillingness = 9,
	DenounceWillingness = 4,
	WorkWithWillingness = 9,
	WorkAgainstWillingness = 4,
	Loyalty = 8,
	Forgiveness = 6,
	Neediness = 4,
	Meanness = 3
WHERE Type = 'LEADER_KAMEHAMEHA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_WAR', 6),
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 4),
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_GUARDED', 4),
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_KAMEHAMEHA', 'MAJOR_CIV_APPROACH_FRIENDLY', 9);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_KAMEHAMEHA', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_KAMEHAMEHA', 'MINOR_CIV_APPROACH_PROTECTIVE', 6),
	('LEADER_KAMEHAMEHA', 'MINOR_CIV_APPROACH_BULLY', 4),
	('LEADER_KAMEHAMEHA', 'MINOR_CIV_APPROACH_CONQUEST', 2);

-- Maria I (Portugal)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 4,
	WonderCompetitiveness = 4,
	MinorCivCompetitiveness = 9,
	Boldness = 4,
	DiploBalance = 7,
	WarmongerHate = 9,
	DoFWillingness = 8,
	DenounceWillingness = 8,
	WorkWithWillingness = 8,
	WorkAgainstWillingness = 8,
	Loyalty = 9,
	Forgiveness = 7,
	Neediness = 3,
	Meanness = 6
WHERE Type = 'LEADER_MARIA_I';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_DECEPTIVE', 7),
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_GUARDED', 8),
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_AFRAID', 6),
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_NEUTRAL', 7),
	('LEADER_MARIA_I', 'MAJOR_CIV_APPROACH_FRIENDLY', 4);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_MARIA_I', 'MINOR_CIV_APPROACH_IGNORE', 2),
	('LEADER_MARIA_I', 'MINOR_CIV_APPROACH_PROTECTIVE', 9),
	('LEADER_MARIA_I', 'MINOR_CIV_APPROACH_BULLY', 4),
	('LEADER_MARIA_I', 'MINOR_CIV_APPROACH_CONQUEST', 3);

-- Maria Theresa (Austria)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 4,
	WonderCompetitiveness = 8,
	MinorCivCompetitiveness = 10,
	Boldness = 6,
	DiploBalance = 10,
	WarmongerHate = 10,
	DoFWillingness = 7,
	DenounceWillingness = 9,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 9,
	Loyalty = 5,
	Forgiveness = 6,
	Neediness = 8,
	Meanness = 4
WHERE Type = 'LEADER_MARIA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 7),
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_NEUTRAL', 7),
	('LEADER_MARIA', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_MARIA', 'MINOR_CIV_APPROACH_IGNORE', -1),
	('LEADER_MARIA', 'MINOR_CIV_APPROACH_PROTECTIVE', 10),
	('LEADER_MARIA', 'MINOR_CIV_APPROACH_BULLY', 1),
	('LEADER_MARIA', 'MINOR_CIV_APPROACH_CONQUEST', 1);

-- Montezuma (The Aztecs)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 7,
	MinorCivCompetitiveness = 3,
	Boldness = 12,
	DiploBalance = 6,
	WarmongerHate = -1,
	DoFWillingness = 5,
	DenounceWillingness = 7,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 7,
	Loyalty = 4,
	Forgiveness = 3,
	Neediness = 9,
	Meanness = 10
WHERE Type = 'LEADER_MONTEZUMA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_WAR', 10),
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_HOSTILE', 9),
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 8),
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_GUARDED', 4),
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_AFRAID', 2),
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_MONTEZUMA', 'MAJOR_CIV_APPROACH_FRIENDLY', 3);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_MONTEZUMA', 'MINOR_CIV_APPROACH_IGNORE', 7),
	('LEADER_MONTEZUMA', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_MONTEZUMA', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_MONTEZUMA', 'MINOR_CIV_APPROACH_CONQUEST', 10);

-- Napoleon (France)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 9,
	MinorCivCompetitiveness = 5,
	Boldness = 8,
	DiploBalance = 5,
	WarmongerHate = 3,
	DoFWillingness = 7,
	DenounceWillingness = 8,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 8,
	Loyalty = 4,
	Forgiveness = 8,
	Neediness = 7,
	Meanness = 6
WHERE Type = 'LEADER_NAPOLEON';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_WAR', 9),
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_DECEPTIVE', 8),
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_NAPOLEON', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_NAPOLEON', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_NAPOLEON', 'MINOR_CIV_APPROACH_PROTECTIVE', 5),
	('LEADER_NAPOLEON', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_NAPOLEON', 'MINOR_CIV_APPROACH_CONQUEST', 7);

-- Nebuchadnezzar II (Babylon)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 3,
	WonderCompetitiveness = 8,
	MinorCivCompetitiveness = 7,
	Boldness = 5,
	DiploBalance = 9,
	WarmongerHate = 8,
	DoFWillingness = 3,
	DenounceWillingness = 5,
	WorkWithWillingness = 3,
	WorkAgainstWillingness = 5,
	Loyalty = 8,
	Forgiveness = 5,
	Neediness = 2,
	Meanness = 7
WHERE Type = 'LEADER_NEBUCHADNEZZAR';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_WAR', 3),
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_DECEPTIVE', 2),
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_NEUTRAL', 10),
	('LEADER_NEBUCHADNEZZAR', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_NEBUCHADNEZZAR', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_NEBUCHADNEZZAR', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_NEBUCHADNEZZAR', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_NEBUCHADNEZZAR', 'MINOR_CIV_APPROACH_CONQUEST', 1);

-- Oda Nobunaga (Japan)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 7,
	MinorCivCompetitiveness = 6,
	Boldness = 6,
	DiploBalance = 2,
	WarmongerHate = 5,
	DoFWillingness = 6,
	DenounceWillingness = 6,
	WorkWithWillingness = 6,
	WorkAgainstWillingness = 6,
	Loyalty = 2,
	Forgiveness = 1,
	Neediness = 7,
	Meanness = 10
WHERE Type = 'LEADER_ODA_NOBUNAGA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_WAR', 9),
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_HOSTILE', 8),
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 8),
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_ODA_NOBUNAGA', 'MAJOR_CIV_APPROACH_FRIENDLY', 4);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_ODA_NOBUNAGA', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_ODA_NOBUNAGA', 'MINOR_CIV_APPROACH_PROTECTIVE', 4),
	('LEADER_ODA_NOBUNAGA', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_ODA_NOBUNAGA', 'MINOR_CIV_APPROACH_CONQUEST', 8);

-- Pacal (The Maya)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 5,
	WonderCompetitiveness = 9,
	MinorCivCompetitiveness = 6,
	Boldness = 7,
	DiploBalance = 7,
	WarmongerHate = 6,
	DoFWillingness = 7,
	DenounceWillingness = 6,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 6,
	Loyalty = 6,
	Forgiveness = 6,
	Neediness = 5,
	Meanness = 7
WHERE Type = 'LEADER_PACAL';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_AFRAID', 2),
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_NEUTRAL', 9),
	('LEADER_PACAL', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_PACAL', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_PACAL', 'MINOR_CIV_APPROACH_PROTECTIVE', 5),
	('LEADER_PACAL', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_PACAL', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Pachacuti (The Inca)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 6,
	Boldness = 9,
	DiploBalance = 4,
	WarmongerHate = 7,
	DoFWillingness = 4,
	DenounceWillingness = 6,
	WorkWithWillingness = 4,
	WorkAgainstWillingness = 6,
	Loyalty = 7,
	Forgiveness = 5,
	Neediness = 5,
	Meanness = 7
WHERE Type = 'LEADER_PACHACUTI';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_HOSTILE', 7),
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_GUARDED', 4),
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_AFRAID', 1),
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_PACHACUTI', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_PACHACUTI', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_PACHACUTI', 'MINOR_CIV_APPROACH_PROTECTIVE', 5),
	('LEADER_PACHACUTI', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_PACHACUTI', 'MINOR_CIV_APPROACH_CONQUEST', 5);

-- Pedro II (Brazil)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 2,
	WonderCompetitiveness = 10,
	MinorCivCompetitiveness = 7,
	Boldness = 3,
	DiploBalance = 10,
	WarmongerHate = 10,
	DoFWillingness = 10,
	DenounceWillingness = 8,
	WorkWithWillingness = 10,
	WorkAgainstWillingness = 8,
	Loyalty = 9,
	Forgiveness = 3,
	Neediness = 8,
	Meanness = 2
WHERE Type = 'LEADER_PEDRO';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_WAR', 2),
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_AFRAID', 6),
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_PEDRO', 'MAJOR_CIV_APPROACH_FRIENDLY', 10);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_PEDRO', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_PEDRO', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_PEDRO', 'MINOR_CIV_APPROACH_BULLY', 4),
	('LEADER_PEDRO', 'MINOR_CIV_APPROACH_CONQUEST', 1);

-- Pocatello (The Shoshone); generalist civ with no victory pursuit bias
UPDATE Leaders
SET
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 3,
	MinorCivCompetitiveness = 7,
	Boldness = 9,
	DiploBalance = 5,
	WarmongerHate = 8,
	DoFWillingness = 5,
	DenounceWillingness = 7,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 7,
	Loyalty = 9,
	Forgiveness = 2,
	Neediness = 6,
	Meanness = 5
WHERE Type = 'LEADER_POCATELLO';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_WAR', 4),
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_GUARDED', 8),
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_NEUTRAL', 9),
	('LEADER_POCATELLO', 'MAJOR_CIV_APPROACH_FRIENDLY', 6);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_POCATELLO', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_POCATELLO', 'MINOR_CIV_APPROACH_PROTECTIVE', 8),
	('LEADER_POCATELLO', 'MINOR_CIV_APPROACH_BULLY', 7),
	('LEADER_POCATELLO', 'MINOR_CIV_APPROACH_CONQUEST', 3);

-- Ramesses II (Egypt)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_CULTURE',
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 12,
	MinorCivCompetitiveness = 5,
	Boldness = 4,
	DiploBalance = 8,
	WarmongerHate = 8,
	DoFWillingness = 3,
	DenounceWillingness = 8,
	WorkWithWillingness = 3,
	WorkAgainstWillingness = 8,
	Loyalty = 5,
	Forgiveness = 4,
	Neediness = 9,
	Meanness = 8
WHERE Type = 'LEADER_RAMESSES';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_WAR', 4),
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_HOSTILE', 8),
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_DECEPTIVE', 6),
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_GUARDED', 8),
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_NEUTRAL', 9),
	('LEADER_RAMESSES', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_RAMESSES', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_RAMESSES', 'MINOR_CIV_APPROACH_PROTECTIVE', 5),
	('LEADER_RAMESSES', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_RAMESSES', 'MINOR_CIV_APPROACH_CONQUEST', 4);

-- Ramkhamhaeng (Siam)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 4,
	WonderCompetitiveness = 6,
	MinorCivCompetitiveness = 12,
	Boldness = 6,
	DiploBalance = 12,
	WarmongerHate = 9,
	DoFWillingness = 9,
	DenounceWillingness = 6,
	WorkWithWillingness = 9,
	WorkAgainstWillingness = 6,
	Loyalty = 7,
	Forgiveness = 7,
	Neediness = 4,
	Meanness = 5
WHERE Type = 'LEADER_RAMKHAMHAENG';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_WAR', 5),
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_HOSTILE', 4),
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_DECEPTIVE', 10),
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_NEUTRAL', 7),
	('LEADER_RAMKHAMHAENG', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_RAMKHAMHAENG', 'MINOR_CIV_APPROACH_IGNORE', -1),
	('LEADER_RAMKHAMHAENG', 'MINOR_CIV_APPROACH_PROTECTIVE', 12),
	('LEADER_RAMKHAMHAENG', 'MINOR_CIV_APPROACH_BULLY', -1),
	('LEADER_RAMKHAMHAENG', 'MINOR_CIV_APPROACH_CONQUEST', 2);

-- Sejong (Korea)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 2,
	WonderCompetitiveness = 10,
	MinorCivCompetitiveness = 8,
	Boldness = 3,
	DiploBalance = 10,
	WarmongerHate = 10,
	DoFWillingness = 9,
	DenounceWillingness = 4,
	WorkWithWillingness = 9,
	WorkAgainstWillingness = 4,
	Loyalty = 9,
	Forgiveness = 4,
	Neediness = 3,
	Meanness = 2
WHERE Type = 'LEADER_SEJONG';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_WAR', 3),
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_HOSTILE', 3),
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_DECEPTIVE', 7),
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_AFRAID', 6),
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_NEUTRAL', 12),
	('LEADER_SEJONG', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_SEJONG', 'MINOR_CIV_APPROACH_IGNORE', 4),
	('LEADER_SEJONG', 'MINOR_CIV_APPROACH_PROTECTIVE', 8),
	('LEADER_SEJONG', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_SEJONG', 'MINOR_CIV_APPROACH_CONQUEST', 2);

-- Shaka (The Zulus)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_CONQUEROR',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 2,
	MinorCivCompetitiveness = -1,
	Boldness = 12,
	DiploBalance = 1,
	WarmongerHate = 1,
	DoFWillingness = 5,
	DenounceWillingness = 7,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 7,
	Loyalty = 3,
	Forgiveness = 2,
	Neediness = 5,
	Meanness = 12
WHERE Type = 'LEADER_SHAKA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_WAR', 12),
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_HOSTILE', 9),
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 7),
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_GUARDED', 1),
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_AFRAID', -1),
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_NEUTRAL', 2),
	('LEADER_SHAKA', 'MAJOR_CIV_APPROACH_FRIENDLY', 5);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_SHAKA', 'MINOR_CIV_APPROACH_IGNORE', 6),
	('LEADER_SHAKA', 'MINOR_CIV_APPROACH_PROTECTIVE', 2),
	('LEADER_SHAKA', 'MINOR_CIV_APPROACH_BULLY', 12),
	('LEADER_SHAKA', 'MINOR_CIV_APPROACH_CONQUEST', 8);

-- Suleiman (The Ottomans)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 7,
	WonderCompetitiveness = 7,
	MinorCivCompetitiveness = 8,
	Boldness = 7,
	DiploBalance = 8,
	WarmongerHate = 4,
	DoFWillingness = 7,
	DenounceWillingness = 2,
	WorkWithWillingness = 7,
	WorkAgainstWillingness = 2,
	Loyalty = 8,
	Forgiveness = 7,
	Neediness = 8,
	Meanness = 7
WHERE Type = 'LEADER_SULEIMAN';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_WAR', 8),
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_HOSTILE', 6),
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_DECEPTIVE', 2),
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_GUARDED', 5),
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_NEUTRAL', 6),
	('LEADER_SULEIMAN', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_SULEIMAN', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_SULEIMAN', 'MINOR_CIV_APPROACH_PROTECTIVE', 7),
	('LEADER_SULEIMAN', 'MINOR_CIV_APPROACH_BULLY', 9),
	('LEADER_SULEIMAN', 'MINOR_CIV_APPROACH_CONQUEST', 3);

-- Theodora (Byzantium); generalist civ with no victory pursuit bias
UPDATE Leaders
SET
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 6,
	WonderCompetitiveness = 9,
	MinorCivCompetitiveness = 8,
	Boldness = 4,
	DiploBalance = 9,
	WarmongerHate = 8,
	DoFWillingness = 10,
	DenounceWillingness = 8,
	WorkWithWillingness = 10,
	WorkAgainstWillingness = 8,
	Loyalty = 5,
	Forgiveness = 7,
	Neediness = 9,
	Meanness = 6
WHERE Type = 'LEADER_THEODORA';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_WAR', 3),
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_HOSTILE', 3),
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_DECEPTIVE', 12),
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_NEUTRAL', 5),
	('LEADER_THEODORA', 'MAJOR_CIV_APPROACH_FRIENDLY', 8);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_THEODORA', 'MINOR_CIV_APPROACH_IGNORE', 5),
	('LEADER_THEODORA', 'MINOR_CIV_APPROACH_PROTECTIVE', 8),
	('LEADER_THEODORA', 'MINOR_CIV_APPROACH_BULLY', 8),
	('LEADER_THEODORA', 'MINOR_CIV_APPROACH_CONQUEST', 3);

-- Washington (America); generalist civ with no victory pursuit bias
UPDATE Leaders
SET
	Personality = 'PERSONALITY_COALITION',
	VictoryCompetitiveness = 8,
	WonderCompetitiveness = 4,
	MinorCivCompetitiveness = 8,
	Boldness = 8,
	DiploBalance = 10,
	WarmongerHate = 6,
	DoFWillingness = 8,
	DenounceWillingness = 9,
	WorkWithWillingness = 8,
	WorkAgainstWillingness = 9,
	Loyalty = 5,
	Forgiveness = 4,
	Neediness = 10,
	Meanness = 7
WHERE Type = 'LEADER_WASHINGTON';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_WAR', 6),
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_DECEPTIVE', 5),
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_WASHINGTON', 'MAJOR_CIV_APPROACH_FRIENDLY', 7);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_WASHINGTON', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_WASHINGTON', 'MINOR_CIV_APPROACH_PROTECTIVE', 10),
	('LEADER_WASHINGTON', 'MINOR_CIV_APPROACH_BULLY', 7),
	('LEADER_WASHINGTON', 'MINOR_CIV_APPROACH_CONQUEST', 4);

-- William (The Netherlands)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_DIPLOMACY',
	Personality = 'PERSONALITY_DIPLOMAT',
	VictoryCompetitiveness = 3,
	WonderCompetitiveness = 4,
	MinorCivCompetitiveness = 10,
	Boldness = 6,
	DiploBalance = 12,
	WarmongerHate = 8,
	DoFWillingness = 10,
	DenounceWillingness = 3,
	WorkWithWillingness = 10,
	WorkAgainstWillingness = 3,
	Loyalty = 6,
	Forgiveness = 8,
	Neediness = 8,
	Meanness = 9
WHERE Type = 'LEADER_WILLIAM';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_WAR', 2),
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_HOSTILE', 3),
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_DECEPTIVE', 8),
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_GUARDED', 6),
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_AFRAID', 3),
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_NEUTRAL', 8),
	('LEADER_WILLIAM', 'MAJOR_CIV_APPROACH_FRIENDLY', 10);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_WILLIAM', 'MINOR_CIV_APPROACH_IGNORE', 4),
	('LEADER_WILLIAM', 'MINOR_CIV_APPROACH_PROTECTIVE', 10),
	('LEADER_WILLIAM', 'MINOR_CIV_APPROACH_BULLY', 6),
	('LEADER_WILLIAM', 'MINOR_CIV_APPROACH_CONQUEST', 4);

-- Wu Zetian (China)
UPDATE Leaders
SET
	PrimaryVictoryPursuit = 'VICTORY_PURSUIT_SCIENCE',
	SecondaryVictoryPursuit = 'VICTORY_PURSUIT_DOMINATION',
	Personality = 'PERSONALITY_EXPANSIONIST',
	VictoryCompetitiveness = 10,
	WonderCompetitiveness = 8,
	MinorCivCompetitiveness = 8,
	Boldness = 6,
	DiploBalance = 8,
	WarmongerHate = 9,
	DoFWillingness = 5,
	DenounceWillingness = 10,
	WorkWithWillingness = 5,
	WorkAgainstWillingness = 10,
	Loyalty = 4,
	Forgiveness = 5,
	Neediness = 9,
	Meanness = 9
WHERE Type = 'LEADER_WU_ZETIAN';

INSERT INTO Leader_MajorCivApproachBiases
	(LeaderType, MajorCivApproachType, Bias)
VALUES
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_WAR', 6),
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_HOSTILE', 5),
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_DECEPTIVE', 8),
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_GUARDED', 7),
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_AFRAID', 4),
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_NEUTRAL', 10),
	('LEADER_WU_ZETIAN', 'MAJOR_CIV_APPROACH_FRIENDLY', 4);

INSERT INTO Leader_MinorCivApproachBiases
	(LeaderType, MinorCivApproachType, Bias)
VALUES
	('LEADER_WU_ZETIAN', 'MINOR_CIV_APPROACH_IGNORE', 3),
	('LEADER_WU_ZETIAN', 'MINOR_CIV_APPROACH_PROTECTIVE', 9),
	('LEADER_WU_ZETIAN', 'MINOR_CIV_APPROACH_BULLY', 5),
	('LEADER_WU_ZETIAN', 'MINOR_CIV_APPROACH_CONQUEST', 5);

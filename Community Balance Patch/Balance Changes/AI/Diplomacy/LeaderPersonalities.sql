ALTER TABLE Leaders ADD COLUMN 'Personality' text default NULL;

-- Defaults
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR'; 		-- basic default
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION'		WHERE DoFWillingness >= 7;
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION'		WHERE Loyalty >= 7;
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT'			WHERE DiploBalance >= 7;
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT'			WHERE MinorCivCompetitiveness >= 7;
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST'		WHERE Loyalty <= 3;
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST'		WHERE WonderCompetitiveness <= 3;

-- Ahmad al-Mansur (Morocco)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET Boldness = 6 					WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET DiploBalance = 7 				WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET WarmongerHate = 5 				WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET DenounceWillingness = 6 			WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET DoFWillingness = 7 				WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET Loyalty = 4 						WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET Forgiveness = 5 					WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_AHMAD_ALMANSUR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_AHMAD_ALMANSUR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Alexander (Greece)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET WonderCompetitiveness = 3 		WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET MinorCivCompetitiveness = 10 	WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET Boldness = 10 					WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET DiploBalance = 7 				WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET WarmongerHate = 2 				WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET Neediness = 3 					WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leaders SET Meanness = 8 					WHERE Type = 'LEADER_ALEXANDER';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_ALEXANDER' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Ashurbanipal (Assyria)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET WonderCompetitiveness = 5 		WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET MinorCivCompetitiveness = 2 		WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET DiploBalance = 10 				WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET DenounceWillingness = 6 			WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET DoFWillingness = 3 				WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET Neediness = 9 					WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET Forgiveness = 5 					WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leaders SET Meanness = 8 					WHERE Type = 'LEADER_ASHURBANIPAL';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ASHURBANIPAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Askia (Songhai)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET VictoryCompetitiveness = 10 		WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET MinorCivCompetitiveness = 4 		WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET Boldness = 7 					WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET DiploBalance = 4 				WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET DoFWillingness = 4 				WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET Loyalty = 1 						WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET Forgiveness = 3 					WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ASKIA';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_ASKIA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ASKIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Attila (The Huns)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET WonderCompetitiveness = 4 		WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET MinorCivCompetitiveness = 4 		WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET Boldness = 9 					WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET DiploBalance = 1 				WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET WarmongerHate = 2 				WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET DoFWillingness = 3 				WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET Neediness = 10 					WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ATTILA';
UPDATE Leaders SET Meanness = 10 					WHERE Type = 'LEADER_ATTILA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_ATTILA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Augustus Caesar (Rome)
UPDATE Leaders SET Personality= 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET MinorCivCompetitiveness = 2 		WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET Boldness = 7 					WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET DiploBalance = 3 				WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET WarmongerHate = 5 				WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET DenounceWillingness = 2 			WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET DoFWillingness = 2 				WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET Forgiveness = 1 					WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leaders SET Meanness = 8 					WHERE Type = 'LEADER_AUGUSTUS';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_AUGUSTUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Bismarck (Germany)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET WonderCompetitiveness = 5 		WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET MinorCivCompetitiveness = 9 		WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET Boldness = 5 					WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET DiploBalance = 9 				WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET WarmongerHate = 8 				WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET Loyalty = 6 						WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_BISMARCK';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_BISMARCK';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_BISMARCK' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Boudicca (The Celts)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET VictoryCompetitiveness = 10 		WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET MinorCivCompetitiveness = 4 		WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET DenounceWillingness = 6 			WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET Loyalty = 6 						WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET Neediness = 7 					WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_BOUDICCA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_BOUDICCA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Casimir III (Poland)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET VictoryCompetitiveness = 6 		WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET MinorCivCompetitiveness = 5 		WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET Boldness = 5 					WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET Forgiveness = 2 					WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_CASIMIR';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_CASIMIR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_CASIMIR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Catherine (Russia)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET VictoryCompetitiveness = 9 		WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET WonderCompetitiveness = 5 		WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET MinorCivCompetitiveness = 6 		WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET DiploBalance = 3 				WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET WarmongerHate = 4 				WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET Neediness = 9 					WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_CATHERINE';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_CATHERINE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_CATHERINE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Darius I (Persia)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET MinorCivCompetitiveness = 3 		WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET Boldness = 6 					WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET WarmongerHate = 4 				WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET DenounceWillingness = 6 			WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET Forgiveness = 5 					WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_DARIUS';
UPDATE Leaders SET Meanness = 4 					WHERE Type = 'LEADER_DARIUS';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_DARIUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Dido (Carthage)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET WonderCompetitiveness = 5 		WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET MinorCivCompetitiveness = 8 		WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET WarmongerHate = 4 				WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET DoFWillingness = 4 				WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET Loyalty = 8 						WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET Forgiveness = 3 					WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_DIDO';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_DIDO';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_DIDO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_DIDO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_DIDO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_DIDO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_DIDO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_DIDO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Elizabeth (England)
UPDATE Leaders SET Personality= 'PERSONALITY_COALITION'	WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET VictoryCompetitiveness = 9 		WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET Boldness = 7 					WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET DiploBalance = 10 				WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET WarmongerHate = 8 				WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET DenounceWillingness = 8 			WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leaders SET Meanness = 7 					WHERE Type = 'LEADER_ELIZABETH';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ELIZABETH' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Enrico Dandolo (Venice)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET WonderCompetitiveness = 7 		WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET MinorCivCompetitiveness = 10 	WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET Boldness = 3 					WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET DiploBalance = 10 				WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET WarmongerHate = 12 				WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leaders SET Meanness = 4 					WHERE Type = 'LEADER_ENRICO_DANDOLO';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ENRICO_DANDOLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Gajah Mada (Indonesia)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET WonderCompetitiveness = 4 		WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET MinorCivCompetitiveness = 5 		WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET DiploBalance = 5 				WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET WarmongerHate = 5 				WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET Loyalty = 10 					WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_GAJAH_MADA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GAJAH_MADA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Gandhi (India)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET VictoryCompetitiveness = 6 		WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET WonderCompetitiveness = 9 		WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET MinorCivCompetitiveness = 4 		WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET Boldness = 5 					WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET WarmongerHate = 12 				WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET DenounceWillingness = 8 			WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET Loyalty = 6 						WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_GANDHI';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_GANDHI';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GANDHI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Genghis Khan (Mongolia)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET WonderCompetitiveness = 2 		WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET MinorCivCompetitiveness = 3 		WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET Boldness = 9 					WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET DiploBalance = 5 				WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET WarmongerHate = 2 				WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET DoFWillingness = 7 				WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET Neediness = 7 					WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_GENGHIS_KHAN';
UPDATE Leader_MajorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_GENGHIS_KHAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Gustavus Adolphus (Sweden)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET VictoryCompetitiveness = 9 		WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET WonderCompetitiveness = 5 		WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET MinorCivCompetitiveness = 5 		WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET Boldness = 10 					WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET WarmongerHate = 3 				WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET DenounceWillingness = 6 			WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET Forgiveness = 7 					WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leaders SET Meanness = 6 					WHERE Type = 'LEADER_GUSTAVUS_ADOLPHUS';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Haile Selassie (Ethiopia)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET WonderCompetitiveness = 7 		WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET MinorCivCompetitiveness = 6 		WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET Boldness = 6 					WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET DiploBalance = 7 				WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET WarmongerHate = 8 				WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET DenounceWillingness = 6 			WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_SELASSIE';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_SELASSIE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_SELASSIE' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Harald Bluetooth (Denmark)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET WonderCompetitiveness = 4 		WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET MinorCivCompetitiveness = 4 		WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET Boldness = 9 					WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET WarmongerHate = 1 				WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET DenounceWillingness = 4 			WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET Neediness = 9 					WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET Forgiveness = 5 					WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_HARALD';
UPDATE Leaders SET Meanness = 8 					WHERE Type = 'LEADER_HARALD';
UPDATE Leader_MajorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARALD' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARALD' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARALD' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARALD' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HARALD' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARALD' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Harun al-Rashid (Arabia)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET WonderCompetitiveness = 9 		WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET Boldness = 5 					WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET DiploBalance = 10 				WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET WarmongerHate = 6 				WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET Loyalty = 4 						WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET Forgiveness = 5 					WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leaders SET Meanness = 4 					WHERE Type = 'LEADER_HARUN_AL_RASHID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HARUN_AL_RASHID' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Hiawatha (The Iroquois)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET VictoryCompetitiveness = 7 		WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET MinorCivCompetitiveness = 8 		WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET Boldness = 7 					WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET DoFWillingness = 7 				WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET Neediness = 5 					WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leaders SET Meanness = 8 					WHERE Type = 'LEADER_HIAWATHA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_HIAWATHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Isabella (Spain)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ISABELLA';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_ISABELLA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ISABELLA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Kamehameha (Polynesia)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_KAMEHAMEHA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_KAMEHAMEHA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Maria I (Portugal)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_MARIA_I';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_MARIA_I';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA_I' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Maria Theresa (Austria)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET MinorCivCompetitiveness = 9 		WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET Boldness = 2 					WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET DenounceWillingness = 3 			WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_MARIA';
UPDATE Leaders SET Meanness = 3 					WHERE Type = 'LEADER_MARIA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MARIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MARIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MARIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_MARIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MARIA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Montezuma (The Aztecs)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET VictoryCompetitiveness = 10 		WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET MinorCivCompetitiveness = 1 		WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET Boldness = 10 					WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET DiploBalance = 1 				WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET WarmongerHate = -1 				WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET Loyalty = 4 						WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET Neediness = 2 					WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET Forgiveness = 2 					WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leaders SET Meanness = 10 					WHERE Type = 'LEADER_MONTEZUMA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_MONTEZUMA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Napoleon (France)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET WonderCompetitiveness = 4 		WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET MinorCivCompetitiveness = 3 		WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET Boldness = 6 					WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET DiploBalance = 4 				WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET WarmongerHate = 3 				WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET DoFWillingness = 2 				WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET Forgiveness = 2 					WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_NAPOLEON';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NAPOLEON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Nebuchadnezzar II (Babylon)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET WonderCompetitiveness = 2 		WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET MinorCivCompetitiveness = 5 		WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET DiploBalance = 2 				WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET WarmongerHate = 5 				WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET DoFWillingness = 4 				WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET Neediness = 2 					WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leaders SET Meanness = 7 					WHERE Type = 'LEADER_NEBUCHADNEZZAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_NEBUCHADNEZZAR' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Oda Nobunaga (Japan)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET WonderCompetitiveness = 4 		WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET MinorCivCompetitiveness = 3 		WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET Boldness = 6 					WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET DiploBalance = 4 				WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET WarmongerHate = 3 				WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET DoFWillingness = 2 				WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET Forgiveness = 2 					WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leaders SET Meanness = 9 					WHERE Type = 'LEADER_ODA_NOBUNAGA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_ODA_NOBUNAGA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Pacal (The Maya)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_PACAL';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_PACAL';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACAL' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PACAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PACAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PACAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_PACAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACAL' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Pachacuti (The Inca)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_PACHACUTI';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PACHACUTI' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Pedro II (Brazil)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET MinorCivCompetitiveness = 9 		WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET Boldness = 2 					WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET DenounceWillingness = 3 			WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_PEDRO';
UPDATE Leaders SET Meanness = 3 					WHERE Type = 'LEADER_PEDRO';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_PEDRO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Pocatello (The Shoshone)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET MinorCivCompetitiveness = 9 		WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET Boldness = 2 					WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET DenounceWillingness = 3 			WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_POCATELLO';
UPDATE Leaders SET Meanness = 3 					WHERE Type = 'LEADER_POCATELLO';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_POCATELLO' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Ramesses II (Egypt)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET VictoryCompetitiveness = 6 		WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET WonderCompetitiveness = 10 		WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET MinorCivCompetitiveness = 9 		WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET DiploBalance = 5 				WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET DenounceWillingness = 3 			WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_RAMESSES';
UPDATE Leaders SET Meanness = 7 					WHERE Type = 'LEADER_RAMESSES';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_RAMESSES' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Ramkhamhaeng (Siam)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET MinorCivCompetitiveness = 10 	WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET Boldness = 2 					WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET DenounceWillingness = 3 			WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leaders SET Meanness = 3 					WHERE Type = 'LEADER_RAMKHAMHAENG';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 7 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_RAMKHAMHAENG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Sejong (Korea)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET VictoryCompetitiveness = 4 		WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET WonderCompetitiveness = 10 		WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET MinorCivCompetitiveness = 8 		WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET WarmongerHate = 8 				WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET DoFWillingness = 5 				WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET Forgiveness = 5 					WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_SEJONG';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_SEJONG';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SEJONG' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Shaka (The Zulus)
UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET VictoryCompetitiveness = 9 		WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET WonderCompetitiveness = 4 		WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET MinorCivCompetitiveness = 1 		WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET Boldness = 10 					WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET DiploBalance = 4 				WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET WarmongerHate = 2 				WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET DenounceWillingness = 7 			WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET DoFWillingness = 2 				WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET Loyalty = 5 						WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET Neediness = 6 					WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET Forgiveness = 2 					WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_SHAKA';
UPDATE Leaders SET Meanness = 10 					WHERE Type = 'LEADER_SHAKA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 9 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 3 	WHERE LeaderType = 'LEADER_SHAKA'	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_SHAKA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Suleiman (The Ottomans)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET WonderCompetitiveness = 2 		WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET MinorCivCompetitiveness = 5 		WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET DiploBalance = 2 				WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET WarmongerHate = 5 				WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET DoFWillingness = 4 				WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET Neediness = 2 					WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leaders SET Meanness = 7 					WHERE Type = 'LEADER_SULEIMAN';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_SULEIMAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Theodora (Byzantium)
UPDATE Leaders SET Personality = 'PERSONALITY_DIPLOMAT' WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET MinorCivCompetitiveness = 9 		WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET Boldness = 2 					WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET DiploBalance = 6 				WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET DenounceWillingness = 3 			WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET DoFWillingness = 8 				WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET Loyalty = 9 						WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET Neediness = 8 					WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET Forgiveness = 6 					WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_THEODORA';
UPDATE Leaders SET Meanness = 3 					WHERE Type = 'LEADER_THEODORA';
UPDATE Leader_MajorCivApproachBiases SET Bias = 1 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_THEODORA' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Washington (America)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET VictoryCompetitiveness = 6 		WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET WonderCompetitiveness = 6 		WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET MinorCivCompetitiveness = 7 		WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET DiploBalance = 8 				WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET WarmongerHate = 7 				WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET Loyalty = 7 						WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_WASHINGTON';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WASHINGTON' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- William (The Netherlands)
UPDATE Leaders SET Personality = 'PERSONALITY_COALITION' WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET VictoryCompetitiveness = 5 		WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET WonderCompetitiveness = 8 		WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET MinorCivCompetitiveness = 8 		WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET Boldness = 4 					WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET DiploBalance = 10 				WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET WarmongerHate = 9 				WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET DenounceWillingness = 9 			WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET DoFWillingness = 6 				WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET Loyalty = 10 					WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET Neediness = 4 					WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET Forgiveness = 8 					WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_WILLIAM';
UPDATE Leaders SET Meanness = 5 					WHERE Type = 'LEADER_WILLIAM';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 5 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 10 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WILLIAM' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Wu Zetian (China)
UPDATE Leaders SET Personality = 'PERSONALITY_EXPANSIONIST' WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET VictoryCompetitiveness = 8 		WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET WonderCompetitiveness = 2 		WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET MinorCivCompetitiveness = 5 		WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET Boldness = 8 					WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET DiploBalance = 2 				WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET WarmongerHate = 5 				WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET DenounceWillingness = 5 			WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET DoFWillingness = 4 				WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET Loyalty = 3 						WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET Neediness = 2 					WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET Forgiveness = 4 					WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET Chattiness = 4 					WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leaders SET Meanness = 7 					WHERE Type = 'LEADER_WU_ZETIAN';
UPDATE Leader_MajorCivApproachBiases SET Bias = 2 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_WAR';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_HOSTILE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_DECEPTIVE';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_GUARDED';
UPDATE Leader_MajorCivApproachBiases SET Bias = 4 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_AFRAID';
UPDATE Leader_MajorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MajorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MajorCivApproachType = 'MAJOR_CIV_APPROACH_NEUTRAL';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY';
UPDATE Leader_MinorCivApproachBiases SET Bias = 6 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST';
UPDATE Leader_MinorCivApproachBiases SET Bias = 8 	WHERE LeaderType = 'LEADER_WU_ZETIAN' 	AND MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY';

-- Start biases
INSERT INTO Civilization_Start_Along_River
	(CivilizationType, StartAlongRiver)
VALUES
	('CIVILIZATION_EGYPT', '1'),
	('CIVILIZATION_INDIA', '1'),
	('CIVILIZATION_BABYLON', '1'),
	('CIVILIZATION_CHINA', '1'),
	('CIVILIZATION_ASSYRIA', '1');

INSERT INTO Civilization_Start_Along_Ocean
	(CivilizationType, StartAlongOcean)
VALUES
	('CIVILIZATION_NETHERLANDS', '1');

DELETE FROM Civilization_Start_Region_Avoid
WHERE CivilizationType = 'CIVILIZATION_SIAM';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_INDIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_CELTS';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_AUSTRIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_FRANCE';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_GERMANY';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_CELTS';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_ROME';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_PERSIA';

DELETE FROM Civilization_Start_Region_Priority
WHERE CivilizationType = 'CIVILIZATION_ETHIOPIA';

INSERT INTO Civilization_Start_Region_Priority
	(CivilizationType, RegionType)
VALUES
	('CIVILIZATION_OTTOMAN', 'REGION_HILLS'),
	('CIVILIZATION_GREECE', 'REGION_HILLS'),
	('CIVILIZATION_AMERICA', 'REGION_FOREST'),
	('CIVILIZATION_SIAM', 'REGION_JUNGLE'),
	('CIVILIZATION_SIAM', 'REGION_FOREST'),
	('CIVILIZATION_HUNS', 'REGION_PLAINS'),
	('CIVILIZATION_ZULU', 'REGION_PLAINS'),
	('CIVILIZATION_POLAND', 'REGION_GRASS'),
	('CIVILIZATION_FRANCE', 'REGION_GRASS'),
	('CIVILIZATION_GERMANY', 'REGION_GRASS'),
	('CIVILIZATION_CELTS', 'REGION_HILLS'),
	('CIVILIZATION_ROME', 'REGION_GRASS'),
	('CIVILIZATION_SHOSHONE', 'REGION_GRASS'),
	('CIVILIZATION_GERMANY', 'REGION_FOREST'),
	('CIVILIZATION_MAYA', 'REGION_JUNGLE'),
	('CIVILIZATION_PERSIA', 'REGION_HILLS'),
	('CIVILIZATION_ETHIOPIA', 'REGION_HILLS'),
	('CIVILIZATION_EGYPT', 'REGION_DESERT');

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_KOREA';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_JAPAN';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_BYZANTIUM';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_INDONESIA';

DELETE FROM Civilization_Start_Along_Ocean
WHERE CivilizationType = 'CIVILIZATION_OTTOMAN';
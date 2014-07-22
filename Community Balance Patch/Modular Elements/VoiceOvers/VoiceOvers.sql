/*
Speech is enabled even where the text is displayed onscreen.
Each place where this occurs:
On game start with the civilization screen,
on the completion of Wonders,
when new TECH is researched or
upon reaching a new era, can be disabled by replacing (Value=1) with (Value=0) or globally with the CommunityOptions.sql.
*/

UPDATE Civilizations SET DawnOfManAudio = "" WHERE EXISTS 
(SELECT Value FROM COMMUNITY WHERE Type='PLAY_SPEECH_START' AND Value=1);

UPDATE Buildings SET WonderSplashAudio = "" WHERE EXISTS 
(SELECT Value FROM COMMUNITY WHERE Type='PLAY_SPEECH_WONDERS' AND Value=1);

UPDATE Technologies SET AudioIntroHeader = "" WHERE EXISTS 
(SELECT Value FROM COMMUNITY WHERE Type='PLAY_SPEECH_TECHS' AND Value=1);

UPDATE Technologies SET AudioIntro = "" WHERE EXISTS 
(SELECT Value FROM COMMUNITY WHERE Type='PLAY_SPEECH_TECHS' AND Value=1);

UPDATE Era_NewEraVOs SET VOScript = "" WHERE EXISTS 
(SELECT Value FROM COMMUNITY WHERE Type='PLAY_ERA_VOS' AND Value=1);
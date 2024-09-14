--------------------------------------------------
-- Great Works of Writing
--------------------------------------------------
CREATE TEMP TABLE GreatWriting (
	ID TEXT
);

INSERT INTO GreatWriting
VALUES
	('010'), ('020'), ('030'), ('040'), ('050'), ('060'), ('070'), ('080'), ('090'), ('100'),
	('110'), ('120'), ('130'), ('140'), ('150'), ('160'), ('170'), ('180'), ('190'), ('200'),
	('210'), ('220'), ('230'), ('240'), ('250'), ('260'), ('270'), ('280'), ('290'), ('300'),
	('310'), ('320'), ('330'), ('340'), ('350'), ('360'), ('370'), ('380'), ('390'), ('400'),
	('410'), ('420'), ('430'), ('440'), ('450'), ('460'), ('470'), ('480'), ('490'), ('500'),
	('510'), ('520'), ('530'), ('540');

INSERT INTO Audio_Sounds
	(SoundID, FileName, LoadType, DontCache)
SELECT
	'SND_GREAT_WRITER_W__' || ID, 'W__' || ID, 'Streamed', 1
FROM GreatWriting;

INSERT INTO Audio_2DSounds
	(ScriptID, SoundID, SoundType, TaperSoundtrackVolume)
SELECT
	'AS2D_GREAT_WRITER_W__' || ID, 'SND_GREAT_WRITER_W__' || ID, 'GAME_SPEECH', 0.95
FROM GreatWriting;

INSERT INTO GreatWorks
	(Type, GreatWorkClassType, Description, Quote, Audio, Image)
SELECT
	'GREAT_WORK_WW_' || ID, 'GREAT_WORK_LITERATURE', 'TXT_KEY_GREAT_WORK_WW_' || ID, 'TXT_KEY_GREAT_WORK_QUOTE_WQ_' || ID, 'AS2D_GREAT_WRITER_W__' || ID, 'GreatWriter_Background.dds'
FROM GreatWriting;

DROP TABLE GreatWriting;

--------------------------------------------------
-- Great Works of Art
--------------------------------------------------
CREATE TEMP TABLE GreatArt (
	ID TEXT
);

INSERT INTO GreatArt
VALUES
	('010'), ('020'), ('030'), ('040'), ('050'), ('060'), ('070'), ('080'), ('090'), ('100'),
	('110'), ('120'), ('130'), ('140'), ('150'), ('160'), ('170'), ('180'), ('190'), ('200'),
	('210'), ('220'), ('230'), ('240'), ('250'), ('260'), ('270'), ('280'), ('290'), ('300'),
	('310'), ('320'), ('330'), ('340'), ('350'), ('360'), ('370'), ('380'), ('390'), ('400'),
	('410'), ('420'), ('430'), ('440'), ('450'), ('460'), ('470'), ('480'), ('490'), ('500'),
	('510'), ('520'), ('530'), ('540'), ('550'), ('560'), ('570'), ('580'), ('590'), ('600'),
	('610'), ('620'), ('630'), ('640'), ('650'), ('660'), ('670'), ('680'), ('690'), ('700'),
	('710'), ('720'), ('730'), ('740'), ('750'), ('760'), ('770'), ('780'), ('790'), ('800'),
	('810'), ('820'), ('830'), ('840'), ('850'), ('860'), ('870'), ('880'), ('890'), ('900');

INSERT INTO GreatWorks
	(Type, GreatWorkClassType, Description, Audio, Image)
SELECT
	'GREAT_WORK_AW_' || ID, 'GREAT_WORK_ART', 'TXT_KEY_GREAT_WORK_AW_' || ID, 'AS2D_GREAT_ARTIST_ARTWORK', 'A__' || ID || '.dds'
FROM GreatArt;

DROP TABLE GreatArt;

--------------------------------------------------
-- Great Works of Music
--------------------------------------------------
CREATE TEMP TABLE GreatMusic (
	ID TEXT,
	Image TEXT
);

INSERT INTO GreatMusic
VALUES
	('010', 'BG_grc_01.dds'),
	('020', 'BG_cls_07.dds'),
	('030', 'BG_med_01.dds'),
	('040', 'BG_opr_02.dds'),
	('050', 'BG_scd_08.dds'),
	('060', 'BG_ori_09.dds'),
	('070', 'BG_mic_03.dds'),
	('080', 'GreatMusician_Background.dds'),
	('090', 'BG_opr_01.dds'),
	('100', 'BG_opr_01.dds'),
	('110', 'BG_cls_07.dds'),
	('120', 'BG_mic_03.dds'),
	('130', 'BG_gtr_01.dds'),
	('140', 'BG_opr_02.dds'),
	('150', 'BG_mic_03.dds'),
	('160', 'BG_ori_07.dds'),
	('170', 'BG_opr_02.dds'),
	('180', 'BG_cls_03.dds'),
	('190', 'BG_cls_03.dds'),
	('200', 'BG_gtr_01.dds'),
	('210', 'BG_jaz_01.dds'),
	('220', 'BG_ori_03.dds'),
	('230', 'BG_jaz_02.dds'),
	('240', 'BG_cls_03.dds'),
	('250', 'BG_lat_01.dds'),
	('260', 'BG_jaz_01.dds'),
	('270', 'BG_jaz_02.dds'),
	('280', 'BG_jaz_01.dds'),
	('290', 'BG_mic_05.dds'),
	('300', 'BG_acd_01.dds'),
	('310', 'BG_mic_04.dds'),
	('320', 'BG_ind_01.dds'),
	('330', 'BG_lat_02.dds'),
	('340', 'BG_lat_02.dds'),
	('350', 'BG_mic_05.dds'),
	('360', 'BG_scn_07.dds'),
	('370', 'BG_rcp_08.dds'),
	('380', 'BG_rcp_08.dds'),
	('390', 'BG_ind_02.dds'),
	('400', 'BG_scn_07.dds'),
	('410', 'BG_scn_07.dds'),
	('420', 'BG_scn_07.dds'),
	('430', 'BG_rcp_08.dds'),
	('440', 'BG_scn_08.dds'),
	('450', 'BG_scn_08.dds'),
	('460', 'BG_mic_05.dds'),
	('470', 'BG_scn_03.dds'),
	('480', 'BG_scn_07.dds'),
	('490', 'BG_scn_07.dds'),
	('500', 'BG_spl_01.dds'),
	('510', 'BG_rcp_08.dds'),
	('520', 'BG_cls_01.dds'),
	('530', 'BG_scn_09.dds'),
	('540', 'BG_scn_08.dds'),
	('550', 'BG_scn_03.dds'),
	('560', 'BG_scn_07.dds'),
	('570', 'BG_cls_05.dds'),
	('580', 'BG_scn_07.dds'),
	('590', 'BG_fms_01.dds'),
	('600', 'BG_fms_01.dds'),
	('610', 'BG_scn_01.dds'),
	('620', 'BG_rap_01.dds'),
	('630', 'BG_cls_03.dds'),
	('640', 'BG_spl_01.dds'),
	('650', 'BG_jaz_01.dds'),
	('660', 'BG_cls_03.dds'),
	('670', 'BG_scn_03.dds'),
	('680', 'BG_cls_04.dds'),
	('690', 'BG_ind_01.dds'),
	('700', 'BG_cls_10.dds'),
	('710', 'BG_cls_04.dds'),
	('720', 'BG_cls_03.dds'),
	('730', 'BG_lat_01.dds'),
	('740', 'BG_cls_10.dds'),
	('750', 'BG_scn_01.dds'),
	('760', 'BG_scn_03.dds'),
	('770', 'BG_cls_01.dds'),
	('780', 'BG_ftv_01.dds'),
	('790', 'BG_cls_10.dds'),
	('800', 'BG_scn_07.dds'),
	('810', 'BG_ftv_01.dds'),
	('820', 'BG_scn_01.dds'),
	('830', 'BG_scn_03.dds'),
	('840', 'BG_rap_01.dds'),
	('850', 'BG_cls_10.dds'),
	('860', 'BG_ftv_01.dds'),
	('870', 'BG_cls_10.dds'),
	('880', 'BG_scn_07.dds'),
	('890', 'BG_fms_01.dds'),
	('900', 'BG_scn_08.dds');

INSERT INTO Audio_Sounds
	(SoundID, FileName, LoadType, DontCache)
SELECT
	'SND_GREAT_MUSICIAN_M__' || ID, 'M__' || ID, 'Streamed', 0
FROM GreatMusic;

INSERT INTO Audio_2DSounds
	(ScriptID, SoundID, SoundType, TaperSoundtrackVolume)
SELECT
	'AS2D_GREAT_MUSICIAN_M__' || ID, 'SND_GREAT_MUSICIAN_M__' || ID, 'GAME_MUSIC_STINGS', 0.0
FROM GreatMusic;

INSERT INTO GreatWorks
	(Type, GreatWorkClassType, Description, Audio, Image)
SELECT
	'GREAT_WORK_MW_' || ID, 'GREAT_WORK_MUSIC', 'TXT_KEY_GREAT_WORK_MW_' || ID, 'AS2D_GREAT_MUSICIAN_M__' || ID, Image
FROM GreatMusic;

DROP TABLE GreatMusic;

--------------------------------------------------
-- Artifacts
--------------------------------------------------
CREATE TEMP TABLE Artifacts (
	ID TEXT
);

INSERT INTO Artifacts
VALUES
	('1'), ('2'), ('3'), ('4'), ('5'), ('6'), ('7'), ('8'), ('9'), ('10'),
	('11'), ('12'), ('13'), ('14'), ('15'), ('16'), ('17');

INSERT INTO GreatWorks
	(Type, GreatWorkClassType, ArtifactClassType, Description, Image)
SELECT
	'GREAT_WORK_EGYPT_SARCOPHAGUS_' || ID, 'GREAT_WORK_ARTIFACT', 'ARTIFACT_SARCOPHAGUS', 'TXT_KEY_ARTIFACT_SARCOPHAGUS_' || ID, 'Sarcophagus.dds'
FROM Artifacts;

DROP TABLE Artifacts;

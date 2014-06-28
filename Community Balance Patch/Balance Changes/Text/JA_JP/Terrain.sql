/* The sql commands and text below were formulated in the Terrain tab of the Google Spreadsheet 'Cep_Language'.
https://docs.google.com/spreadsheets/d/1ptQRfVsW7UT_8gPexioizS31sM7K_3eBT3tjr4jruTs/edit#gid=1001822344
If you make any changes be sure to update the spreadsheet also.*/

REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_RESOURCE_MOVIES', '作品', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_RESOURCE_MOVIES_PEDIA', '映画は、プロジェクタを介して実行し、画面に表示される、プラスチックのストリップ上に一連の静止画像であり、膜またはモーションピクチャと呼ばれる動画像の錯覚を作成する。フィルムは、動画カメラで実際のシーンを撮影することにより作成され、従来のアニメーション技術を使用して図面やミニチュアモデルを撮影して、CGIやコンピュータアニメーションによって、またはこれらの技術および他の視覚効果の一部または全部の組み合わせによって。映画制作のプロセスは、芸術と産業の両方である。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_RESOURCE_TYRIAN_PURPLE', '古代紫', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_RESOURCE_TYRIAN_PURPLE_PEDIA', 'また、ロイヤルパープル、皇室紫や皇室の染料として知られている紫色の貝は、赤紫色の天然色素である。色素は紫がかった赤として知ら捕食海のカタツムリで生成分泌である。この染料は、おそらく早ければ1600として紀元前の古代フェニキア人によって使用された。色は簡単に消え、代わりに風化し、日光で明るくなっていなかったため、色素が大幅に、古代に珍重された。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_BUILD_FORT_REC', 'それは、このタイルに駐留軍事ユニットの[ICON_STRENGTH]防衛を改善し、隣接する敵に20のダメージを与えるでしょう。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_BUILD_FORT_HELP', '[COLOR_NEGATIVE_TEXT]コストは[ENDCOLOR] 3 [ICON_GOLD]ゴールドターンに維持することができる。[NEWLINE] [NEWLINE] 50パーセント[ICON_STRENGTH]守備このタイルに駐留任意のユニットの強度、および、隣接する敵に20のダメージを与える。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIV5_IMPROVEMENTS_FORT_TEXT', '[COLOR_NEGATIVE_TEXT]コストは[ENDCOLOR] 3 [ICON_GOLD]ゴールドターンに維持することができる。[NEWLINE] [NEWLINE]砦ヘクスの防御ボーナスが向上し、特別な改善であり、隣接する敵に20のダメージを与える。しかし、これらの効果は、敵地では適用されません。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIV5_FEATURES_EL_DORADO_HELP', 'それを発見した最初のプレイヤーに[ICON_GOLD]ゴールド100が付与されます。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_FEATURE_ATOLL', 'アイルズ', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIV5_FEATURES_ATOLL', 'アイルズ', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIV5_FEATURES_ATOLL_TEXT', '島は水に囲まれている亜大陸の土地のどの部分である。非常に小さな島が島々、岩礁またはキーを呼び出すことができます。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_SPECIALISTSANDGP_CITADEL_HEADING4_BODY', '{TXT}', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_BUILD_CITADEL_HELP', '[ICON_STRENGTH] 100パーセントは[COLOR_POSITIVE_TEXT]防衛[ENDCOLORこのタイル[NEWLINE] [ICON_RANGE_STRENGTH]近くの敵に[ENDCOLOR]ターンごとに30 [COLOR_POSITIVE_TEXT]のダメージを与える。[NEWLINE] [ICON_CULTURE]近くのタイルに国境を展開します。 [NEWLINE] [ICON_LOCKED]フレンドリー領土が必要です。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIV5_IMPROVEMENTS_POLDER_HELP', '[ICON_FOOD]フード：2 [NEWLINE] [ICON_LOCKED]必須：沿岸の土地を', '', '');





UPDATE LoadedFile SET Value=1, JA_JP=1 Where Type='Terrain.sql';



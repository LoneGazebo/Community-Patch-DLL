/* The sql commands and text below were formulated in the Armies tab of the Google Spreadsheet 'Cep_Language'.
https://docs.google.com/spreadsheets/d/1ptQRfVsW7UT_8gPexioizS31sM7K_3eBT3tjr4jruTs/edit#gid=742165834
If you make any changes be sure to update the spreadsheet also.*/

REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_GUN', '取り付けられた射手', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_CIVILIAN', '民間人のユニット', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_COMMAND', 'サポートユニット', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_MELEE', '兵士ユニット', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_RESOURCE_COAL_HELP', '工場で使用される。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_NAVALMELEE', '護衛艦', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_NAVALRANGED', '共通船', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_SUBMARINE', 'ハンター船', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNITCOMBAT_RECON', 'ヴァンガードユニット', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_PROMOTIONS_SECTION_5', 'ヴァンガードプロモーション', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DUMMY_COAL', '_', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DUMMY_COAL_HELP', 'これは無視してください。それは単に（無単位は、リソースを使用していない場合、それは取引することはできません）ゲーム内の戦略的なリソースのバグを回避します。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_GAME_OPTION_PROMOTION_SAVING', 'プロモーション情報はありません保存しない', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_GAME_OPTION_PROMOTION_SAVING_HELP', '使用可能な場合プレイヤーはすぐにプロモーションを選択する必要があります。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_REFUGEES_OPTION', '[ICON_CITIZEN]戦争難民', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_CAPTURED_MARITIME_OPTION', '[ICON_WAR]キャプチャ海上シティステイト', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_CAPTURED_CULTURAL_OPTION', '[ICON_WAR]キャプチャ文化シティステイト', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_CAPTURED_MILITARISTIC_OPTION', '[ICON_WAR]キャプチャ軍国シティステイト', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_CAPTURED_RELIGIOUS_OPTION', '[ICON_WAR]キャプチャ宗教シティステイト', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_CAPTURED_MERCANTILE_OPTION', '[ICON_WAR]キャプチャマーカンタイルシティステイト', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CNOTIFICATION_CAPTURED_OTHER_OPTION', '[ICON_WAR]その他のシティステイトキャプチャ', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_POPUP_CITY_CAPTURE_INFO', '私たちは、反対意見をどのように扱うのですか？', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CAPTURE_PUPPET', 'マーシー', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CAPTURE_PUPPET_HELP', '死傷者や破壊を避ける。反対意見長引くと、[ICON_HAPPINESS_4] {1}であなたの幸せを低減します。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CAPTURE_PILLAGE', '力', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CAPTURE_PILLAGE_HELP', 'すぐに人口の半分を失って、すべての反対者を殺す。あなたの幸せは、{1} [ICON_HAPPINESS_4]低下する。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CAPTURE_RAZE', '激怒', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CAPTURE_RAZE_HELP', 'すぐに地面に都市を燃やす、その後、人口の半分を殺す。あなたの幸せは、一時的に都市を壊滅させるように[ICON_HAPPINESS_4] {1}、その後徐々に回復し低下する。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_PARATROOPER', '空中', '', '');
/* ユニット */
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_HELP_CHARIOR_ARCHER', '騎手とは違って、槍兵に対して脆弱ではありません速くマウント射手。', '', '');
/*  Footsoldiers  */
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_HELP_HOPLITE', 'ギリシャ人に固有の兵士。重装歩兵は彼らが交換する槍とは異なり、ファランクスのプロモーションを開始します。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_HELP_SCOUT', 'スカウトは探索に優れ、かつ安価な街の守備隊として有用である。', '', '');
/* 携帯 */
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_CAVALRY', 'ドラグーン', '', '');
/* 船 */
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_CARAVEL', 'キャラック船', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIVILOPEDIA_UNITS_RENAISSANCE_CARAVEL_TEXT', 'Carracksは1400年代、1500年代から欧州の主要な軍艦だった。彼らは大西洋で使用するために開発された、荒海で安定していることに十分な大きさ、および長い航海のための規定を運ぶのに十分な広々とした。両端に背の高い「城」敵船を下に発射射手に高さの優位性を与えた。 carracksが戦争と探査で使用されていたのでcaravelsだけの探査のために使用されたが、これらは、caravelsを交換してください。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_GALLEY', '海賊', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_LIBURNA_PEDIA', 'この軽くて速いハンターは古典時代の支配的な船だった。後年、それが2オールのほとり（2段櫂船）、より速く、軽く、そしてtriremesよりもアジャイルを装備している間、それは元々、それぞれの側に25櫂で1台を持っていた。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_SHIP_OF_THE_LINE', '戦列艦', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_SHIP_OF_THE_LINE_PEDIA', '舷側がプライマリ海軍戦術となったため、海軍の戦争の最初の大きな変化は、1600年代に発生した。船は、長い単一ファイルの行を結成し、遠くから敵艦隊を連打。これは、「戦いの行」と呼ばれた船の城によって所定の高さの優位性はほぼなくなり、今では白兵戦はあまり不可欠であった。城は、軽量化と、より機動性のラインの船を作り、縮小、及び船体が大きく、強くなった。最終的に短縮フレーズ「戦艦のライン」「戦艦。 "', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_PRIVATEER', 'ガレ​​オン船', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIV5_PRIVATEER_TEXT', 'ガレ​​オン船はcarracksを置き換え1500年代、1700年代から使用される大規模なマルチ飾ら帆船であった。大砲は海軍の攻撃に射手を置き換えるようキャラックの高い船首楼は徐々に重要性を失った。ガレ​​オン船は船が海兵隊に搭乗した後、敵を突っ込んだとして、敵を爆破する前向き大砲に依存していました。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_CIVILOPEDIA_UNITS_RENAISSANCE_IRONCLAD_TEXT', '鉄壁の鉄鋼装甲板で保護されて1800年代半ばの蒸気推進軍艦だった。鉄壁のは、爆発や焼夷弾の殻に木造軍艦の脆弱性の結果として開発されました。まず鉄壁の戦艦、ラグルワールモンは、1859年11月にフランス海軍によって立ち上げられた。[NEWLINE] [NEWLINE]装甲艦が公海の戦艦、沿岸防衛船、長距離クルーザーとしてなど、いくつかの役割、のために設計された。 19世紀後半の軍艦設計の急速な進化は、鉄鋼、組み込み、小塔の戦艦と20世紀でおなじみの巡洋艦にその蒸気機関を補完するために帆を実施し、木製のハル容器から鉄壁を変形させた。この変更は、より重い海軍銃（1880の装甲艦がこれまで海に装着された最も重い銃のいくつかを実施し）、より洗練された蒸気機関、鉄鋼、造船を可能にした冶金の進歩の開発によって進められた。[NEWLINE] [NL ]アイアンクラッド期変更の急速なペースは多くの船がすぐに彼らは完了したように廃止されたようで、海軍戦術は流動的な状態にあったことをことを意味した。多くの装甲艦は海軍のデザイナーの数は海軍の戦闘の重要な武器とみなさRAMや魚雷、利用するために建設された。そこに鉄壁の期間に明確な終わりはありませんが、1890年代の終わりに向かって長期鉄壁の使用を中退。新しい船は、ますます標準パターンと指定された戦艦や装甲巡洋艦に構築した。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_MISSILE_DESTROYER', 'ミサイル駆逐艦', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_MISSILE_DESTROYER_PEDIA', '誘導ミサイル駆逐艦はミサイルを発射するように設計された駆逐艦である。多くは、対潜水艦、対空を実行するために装備されており、抗表面操作は[NEWLINE] [NEWLINE]駆逐艦が持つ銃に加えて、誘導ミサイル駆逐艦は通常、二つの大きなミサイル雑誌が装備されている店船のミサイル。いくつかの誘導ミサイル駆逐艦は、米国のイージス戦闘システムのように、強力な兵器システムレーダーが含まれており、対ミサイルの役割や弾道ミサイル防衛の役割で使用するために採用してもよい。他の船は、ギャップを埋めるために採用されなければならないので、これは、もはや巡洋艦を操作しないで海軍の特に当てはまります。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BIREME', 'Bireme', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BIREME_PEDIA', '2段櫂船は漕い船の古代のタイプだった。商人と戦争船の区別がなかったとき、彼らは時代に出現した。彼らは、貨物や部隊を輸送することができる海の貿易、海賊行為や戦争のために使用される汎用性の高い、長距離船だった。帆と船体中央マストも追い風の下で船を推進することができます。', '', '');
/* その他 */
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_MISSIONARY_LALIBELA', '宣教師', '', '');


UPDATE LoadedFile SET Value=1, JA_JP=1 Where Type='Armies.sql';

UPDATE Language_JA_JP SET Text = REPLACE( Text, ' (', ': ' )WHERE Tag LIKE 'TXT_KEY_PROMOTION_%';

UPDATE Language_JA_JP SET Text = REPLACE( Text, ')', '' )WHERE Tag LIKE 'TXT_KEY_PROMOTION_%';


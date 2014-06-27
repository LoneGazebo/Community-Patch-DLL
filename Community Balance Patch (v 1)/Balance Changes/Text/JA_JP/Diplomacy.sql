/* The sql commands and text below were formulated in the Diplomacy tab of the Google Spreadsheet 'Cep_Language'.
https://docs.google.com/spreadsheets/d/1ptQRfVsW7UT_8gPexioizS31sM7K_3eBT3tjr4jruTs/edit#gid=1931353934
If you make any changes be sure to update the spreadsheet also.*/

REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP_TT', 'このリーダーと[ICON_RESEARCH]研究契約および[ICON_GOLD]相互国境開放からダブルス利益は[NEWLINE] [NEWLINE]同盟は友好的な外交を推進し、外部の侵略を抑止する。連合軍は外交や経済支援を要請することができる、そしてあなたの同盟国を支援するための失敗は彼らを混乱させる場合があります。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_OPEN_BORDERS_TT', '[COLOR_YIELD_GOLD]相互開放国境[/色]、両方のプレイヤーの組み合わせグロス[ICON_GOLD]所得があなたの金率（2％アライドする場合）に追加します。[NEWLINE] [NEWLINE]他のプレイヤーのユニットを通過させるの1％あなたの領土を通して[NEWLINE] [NEWLINE]（{1}ターン持続します。）', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED', 'あなたは{1}の近くに蛮族を殺した！彼らは感謝して、あなたは彼らと15 [ICON_INFLUENCE]影響力を獲得！', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_NTFN_RA_FREE_TECH', '{1}を使用して研究契約の有効期限が切れています。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_PEACE_TREATY', '平和条約（20ターン）', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_PEACE_TREATY_TURNS', '平和条約（20ターン）', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_PLAYER', '[NEWLINE] [NEWLINE]エンバシー（展覧会）を確立するための技術を持っていません。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_OTHER_PLAYER', '[NEWLINE] [NEWLINE]彼らは大使館（展覧会）を確立するための技術を持っていません。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_OPEN_BORDERS_NO_TECH', '[NEWLINE] [NEWLINE]どちらのプレイヤーは、まだこのアイテム（通貨）を取引する技術を持っています。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_DEF_PACT_NO_TECH', '[NEWLINE] [NEWLINE]どちらのプレイヤーは、まだこのアイテム（ギルド）を取引する技術を持っています。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_TECH', '[NEWLINE] [NEWLINE]どちらのプレイヤーは、まだこのアイテム（哲学）を取引する技術を持っています。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_BORDER_AGREEMENT', '[COLOR_YIELD_GOLD]相互国境開放は[ENDCOLOR] [NEWLINE]両方のプレイヤーの合計総所得の1％があなたの[ICON_GOLD]ゴールド率（2％アライドする場合）に追加します。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_BORDERS_YES_TT', '（連合軍があれば、2％）。[NEWLINE] [NEWLINE] {1}になりラスツ[COLOR_YIELD_GOLD]相互国境開放を提案[ENDCOLOR] [NEWLINE]両方のプレイヤーの合計総所得の1％があなたの[ICON_GOLD]ゴールドレートに追加されます。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_BORDERS_NO_TT', 'アクティブ相互国境開放。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_BORDERS_US_TT', '彼らは私たちに国境開放を与えた。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_BORDERS_THEM_TT', '我々は彼らに国境開放を与えた。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_RESEARCH_AGREEMENT', '[COLOR_RESEARCH_STORED]研究契約は、[ENDCOLOR] [NEWLINE]両方のプレイヤーの組み合わせ研究の4％（8％アライドば）契約の最後に[ICON_RESEARCH]科学に追加されます。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_RA_YES_TT', '[COLOR_RESEARCH_STORED] [NEWLINE] [NEWLINE] {1ラスツ[ENDCOLOR] [NEWLINE]両方のプレイヤーの組み合わせ研究の4％は契約の最後に[ICON_RESEARCH]科学へ（連合国があれば、8％）を付加する研究契約を提案}になります。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_RA_NO_TT', 'アクティブな研究契約。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_DEFENSIVE_PACT', '部外者がどちらかのプレーヤーとの戦争を宣言している場合、[COLOR_CITY_BROWN]守備協定は、[ENDCOLOR] [NEWLINE]は、両方のプレイヤーが部外者に宣戦布告。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_DEFENSE_YES_TT', '[COLOR_CITY_BROWN]守備協定は、[ENDCOLOR] [NEWLINE]部外者がどちらかのプレーヤーに戦争を宣言した場合は、両方のプレイヤーが部外者に宣戦布告。[NEWLINE] [NEWLINE] {1}は変わりラスツ提案する。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_DEFENSE_NO_TT', 'アクティブな防御的な協定。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_GOLD_STORED', 'ゴールドストアド', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_GOLD_PROFIT', 'ゴールド利益', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_ALLIANCE', '[COLOR_GREEN]アリー[ENDCOLOR] [NEWLINE]相互国境開放し、研究契約からの収入を2倍、との友好関係を促進する。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_ALLIANCE_YES_TT', '[COLOR_GREEN]同盟相互国境開放し、研究契約から[ENDCOLOR] [NEWLINE]ダブルス収入を提案し、友好関係を促進します。[NEWLINE] [NEWLINE]ラスツ{1}になります。', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_DEAL_STATUS_ALLIANCE_NO_TT', '我々は同盟している。', '', '');




UPDATE LoadedFile SET Value=1, JA_JP=1 Where Type='Diplomacy.sql';







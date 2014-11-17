	-- Commerce

	UPDATE Policy_CapitalYieldModifiers
	SET Yield = '0'
	WHERE PolicyType = 'POLICY_COMMERCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '[COLOR_POSITIVE_TEXT]Commerce[ENDCOLOR] provides bonuses to naval empires, and those focused on [ICON_GOLD] Gold.[NEWLINE][NEWLINE]Adopting Commerce will grant 25 [ICON_GOLD] Gold every time you construct a building.[NEWLINE][NEWLINE]Adopting all Policies in the Commerce tree will grant +1 [ICON_GOLD] Gold from every Trading Post and double [ICON_GOLD] Gold from Great Merchant trade missions.[NEWLINE][NEWLINE]May purchase Great Merchants with [ICON_PEACE] Faith.'
	WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

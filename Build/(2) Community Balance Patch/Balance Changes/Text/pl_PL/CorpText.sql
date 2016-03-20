--translated by mrlg

UPDATE Language_pl_PL
SET Text = 'Świat wypełniony jest różnego rodzaju surowcami, takimi jak bydło, pszenica, węgiel, żelazo, wieloryby czy ropa, które mogą wspomóc rozwój cywilizacji. Dobrze jest budować miasta w pobliżu surowców, dzięki czemu otrzymają one związane z nimi premie. Niektóre z nich, jak złoto czy ryby, widoczne są już na początku gry, podczas gdy inne, jak aluminium, ropa czy uran, są ukryte do czasu odkrycia przez cywilizację odpowiedniej technologii.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]System korporacji[ENDCOLOR] jest zbudowany wokół monopoli na surowce. Zobacz niżej aby uzyskać więcej informacji o związku pomiędzy korporacjami i surowcami.[NEWLINE][NEWLINE]{TXT_KEY_HAPPINESS_CORPORATION_TYPES_BODY}'
WHERE Tag = 'TXT_KEY_PEDIA_RESOURCES_HELP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

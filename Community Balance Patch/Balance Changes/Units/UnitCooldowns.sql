
-- Cooldowns

UPDATE Units SET PurchaseCooldown =     1  WHERE Cost <=  100 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN' AND Value= 1 );
UPDATE Units SET PurchaseCooldown =     1  WHERE Cost >  100 AND Cost <=  200 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN' AND Value= 1 );
UPDATE Units SET PurchaseCooldown =     1  WHERE Cost >  200 AND Cost <=  300 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN' AND Value= 1 );
UPDATE Units SET PurchaseCooldown =     1  WHERE Cost >  300 AND Cost <=  400 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN' AND Value= 1 );
UPDATE Units SET PurchaseCooldown =     1  WHERE Cost >  400 AND Cost <=  500 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN' AND Value= 1 );
UPDATE Units SET PurchaseCooldown =		1  WHERE Cost >=  500 AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN' AND Value= 1 );

UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_ARTIST';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_SCIENTIST';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_MERCHANT';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_ENGINEER';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_GREAT_GENERAL';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_MONGOLIAN_KHAN';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_PROPHET';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_GREAT_ADMIRAL';
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_WRITER'; 
UPDATE Units SET GlobalFaithPurchaseCooldown =		15  WHERE Type = 'UNIT_MUSICIAN'; 
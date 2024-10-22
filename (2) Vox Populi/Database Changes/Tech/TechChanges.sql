-- Art swap for Metal Casting/Steel/Metallurgy
UPDATE Technologies SET PortraitIndex = 28 WHERE Type = 'TECH_METAL_CASTING';
UPDATE Technologies SET PortraitIndex = 33 WHERE Type = 'TECH_STEEL';
UPDATE Technologies SET PortraitIndex = 14 WHERE Type = 'TECH_METALLURGY';

-- Art/name swap for Construction and Masonry
UPDATE Technologies
SET
	Civilopedia = 'TXT_KEY_TECH_CONSTRUCTION_DESC',
	Description = 'TXT_KEY_TECH_CONSTRUCTION_TITLE',
	PortraitIndex = 15
WHERE Type = 'TECH_MASONRY'; -- Construction

UPDATE Technologies
SET
	Civilopedia = 'TXT_KEY_TECH_MASONRY_DESC',
	Description = 'TXT_KEY_TECH_MASONRY_TITLE',
	PortraitIndex = 6
WHERE Type = 'TECH_CONSTRUCTION'; -- Masonry

-- Move Trade to Ancient T2
UPDATE Technologies
SET Era = 'ERA_ANCIENT', GoodyTech = 1
WHERE Type = 'TECH_HORSEBACK_RIDING';

-- Move Writing to Classical T1
UPDATE Technologies
SET Era = 'ERA_CLASSICAL', GoodyTech = 0
WHERE Type = 'TECH_WRITING';

-- Move Metal Casting to Classical T2
UPDATE Technologies
SET Era = 'ERA_CLASSICAL'
WHERE Type = 'TECH_METAL_CASTING';

-- Move Railroad to Industrial T1
UPDATE Technologies
SET Era = 'ERA_INDUSTRIAL'
WHERE Type = 'TECH_RAILROAD';

-- Move Biology and Electricity to Modern T1
UPDATE Technologies
SET Era = 'ERA_MODERN'
WHERE Type IN ('TECH_BIOLOGY', 'TECH_ELECTRICITY');

-- Move Refrigeration and Electronics to Atomic T1/T2
UPDATE Technologies
SET Era = 'ERA_POSTMODERN'
WHERE Type IN ('TECH_REFRIGERATION', 'TECH_ELECTRONICS');

-- Move Ecology to Information T1
UPDATE Technologies
SET Era = 'ERA_FUTURE'
WHERE Type = 'TECH_ECOLOGY';

-- Move Embarkation to Fishing
UPDATE Technologies
SET
	AllowsEmbarking = 1,
	IconAtlas = 'COMMUNITY_ATLAS',
	PortraitIndex = 26
WHERE Type = 'TECH_SAILING'; -- Fishing

UPDATE Technologies
SET
	AllowsEmbarking = 0,
	PortraitIndex = 13
WHERE Type = 'TECH_OPTICS'; -- Sailing

-- Move Trade Route slots
UPDATE Technologies SET InternationalTradeRoutesChange = 0;

UPDATE Technologies
SET InternationalTradeRoutesChange = 1
WHERE Type IN (
	'TECH_HORSEBACK_RIDING', -- Trade
	'TECH_OPTICS', -- Sailing
	'TECH_CURRENCY',
	'TECH_COMPASS',
	'TECH_ECONOMICS',
	'TECH_RAILROAD',
	'TECH_ELECTRICITY',
	'TECH_REFRIGERATION',
	'TECH_TELECOM'
);

-- Enable Corporations
UPDATE Technologies SET CorporationsEnabled = 1 WHERE Type = 'TECH_CORPORATIONS';

-- Enable World Map trading
UPDATE Technologies SET MapTrading = 1 WHERE Type = 'TECH_MILITARY_SCIENCE';

-- Enable Tech trading
UPDATE Technologies SET TechTrading = 1 WHERE Type = 'TECH_SCIENTIFIC_THEORY';

-- Feature removal yield%
UPDATE Technologies SET FeatureProductionModifier = 100 WHERE Type = 'TECH_BRONZE_WORKING';

-- Reduce Tourism modifier from Internet
UPDATE Technologies SET InfluenceSpreadModifier = 50 WHERE Type = 'TECH_INTERNET';

-- 1 move cost to embark from/disembark into cities or canal tiles
UPDATE Technologies SET CityLessEmbarkCost = 1 WHERE Type = 'TECH_NAVIGATION';

-- 0.1 move cost to embark from/disembark into cities or canal tiles
UPDATE Technologies SET CityNoEmbarkCost = 1 WHERE Type = 'TECH_ROCKETRY';

-- Some bonus Happiness for Future Tech
UPDATE Technologies SET Happiness = 1 WHERE Type = 'TECH_FUTURE_TECH';

-- Free Promotions for lategame civilians
INSERT INTO Technology_FreePromotions
	(TechType, PromotionType)
VALUES
	('TECH_PENICILIN', 'PROMOTION_FALLOUT_REDUCTION'),
	('TECH_NANOTECHNOLOGY', 'PROMOTION_FALLOUT_IMMUNITY'),
	('TECH_BALLISTICS', 'PROMOTION_ICE_BREAKERS');

-- Add Land Trade Route extension to Machinery
INSERT INTO Technology_TradeRouteDomainExtraRange
	(TechType, DomainType, "Range")
VALUES
	('TECH_MACHINERY', 'DOMAIN_LAND', 10);

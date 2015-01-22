-- Compatibility for H&P

INSERT INTO UnitPromotions (Type,	CannotBeChosen,	DefenseMod,	Description,					Help,								PediaEntry,						IconAtlas,			PortraitIndex,	Sound,				PediaType)
SELECT 'PROMOTION_PLAGUED',			1,				-50,		'TXT_KEY_PROMOTION_PLAGUED',	'TXT_KEY_PROMOTION_PLAGUED_HELP',	'TXT_KEY_PROMOTION_PLAGUED',	'PROMOTION_ATLAS',	57,				'AS2D_IF_LEVELUP',	'PEDIA_ATTRIBUTES';	
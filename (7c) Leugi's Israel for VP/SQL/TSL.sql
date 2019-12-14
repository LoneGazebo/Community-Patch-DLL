--==========================================================================================================================
-- Civilizations_YagemStartPosition (Earth Giant)
--==========================================================================================================================
DELETE FROM MinorCiv_YagemStartPosition WHERE Type IN ('MINOR_CIV_JERUSALEM');
INSERT INTO Civilizations_YagemStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('CIVILIZATION_LEUGI_ISRAEL',				37,		50,		null,	null);			
			
INSERT INTO MinorCiv_YagemStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('MINOR_CIV_JERUSALEM',						54,		54,		null,	null); -- Balkh



--==========================================================================================================================
-- Civilizations_YahemStartPosition (Earth Huge)
--==========================================================================================================================
DELETE FROM MinorCiv_YahemStartPosition WHERE Type IN ('MINOR_CIV_JERUSALEM');
INSERT INTO Civilizations_YahemStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('CIVILIZATION_LEUGI_ISRAEL',				73,		48,		null,	null);			
			
INSERT INTO MinorCiv_YagemStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('MINOR_CIV_JERUSALEM',						84,		50,		null,	null); -- Balkh



--==========================================================================================================================
-- Civilizations_CordiformStartPosition (Earth Standard)
--==========================================================================================================================
DELETE FROM MinorCiv_CordiformStartPosition WHERE Type IN ('MINOR_CIV_JERUSALEM');
INSERT INTO Civilizations_CordiformStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('CIVILIZATION_LEUGI_ISRAEL',				47,		19,		null,	null);			
			
INSERT INTO MinorCiv_YagemStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('MINOR_CIV_JERUSALEM',						54,		27,		null,	null); -- Balkh



--==========================================================================================================================
-- Civilizations_GreatestEarthStartPosition (Earth Greatest)
--==========================================================================================================================
DELETE FROM MinorCiv_GreatestEarthStartPosition WHERE Type IN ('MINOR_CIV_JERUSALEM');
INSERT INTO Civilizations_GreatestEarthStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('CIVILIZATION_LEUGI_ISRAEL',				62,		37,		null,	null);			
			
INSERT INTO MinorCiv_YagemStartPosition
			(Type,										X,		Y,		AltX,	AltY)
VALUES		('MINOR_CIV_JERUSALEM',						73,		42,		null,	null); -- Balkh


--==========================================================================================================================
-- Civilizations_YagemRequestedResource (Earth Giant)
--==========================================================================================================================

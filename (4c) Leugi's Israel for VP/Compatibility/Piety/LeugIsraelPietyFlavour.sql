--==========================================================================================================================   
-- Flavors
--==========================================================================================================================           
INSERT OR REPLACE INTO Flavors
            (Type)
VALUES      ('FLAVOR_JFD_RELIGIOUS_INTOLERANCE');
 
INSERT INTO Leader_Flavors
            (LeaderType,            FlavorType,                             Flavor)
VALUES      ('LEADER_DAVID',    'FLAVOR_JFD_RELIGIOUS_INTOLERANCE',     12);

--==========================================================================================================================       
--	Basically, a value of 0 means there is no effect on relations, a value of 1-5 is a small diplo penalty with 
--	heathen civs (“We dislike heathens (but we tolerate them)”), and 5-12 is a large diplo penalty with heathen 
--	civs (“We dislike heathens”).
--==========================================================================================================================
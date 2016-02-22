-- HH_MOD 
--HH_MOD_NATURAL_WONDER_MODULARITY
ALTER TABLE Policies ADD COLUMN 'ExtraNaturalWonderHappiness' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD CONSTRAINT ck_ExtraNaturalWonderHappiness CHECK (ExtraNaturalWonderHappiness >= 0);

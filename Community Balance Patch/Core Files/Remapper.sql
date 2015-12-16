CREATE TABLE IDRemapper ( id INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT );
	INSERT INTO IDRemapper (Type) SELECT Type FROM UnitPromotions ORDER BY ID;
	UPDATE UnitPromotions SET ID = ( SELECT IDRemapper.id-1 FROM IDRemapper WHERE UnitPromotions.Type = IDRemapper.Type);
	DROP TABLE IDRemapper;

	UPDATE sqlite_sequence
	SET seq = (SELECT COUNT(ID) FROM UnitPromotions)-1
	WHERE name = 'UnitPromotions';
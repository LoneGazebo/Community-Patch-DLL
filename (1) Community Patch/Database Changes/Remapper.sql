-- All tables that are used in Community Patch should be remapped, with the exceptions listed below
INSERT INTO Remapper (name)
	SELECT name FROM sqlite_sequence
	WHERE name != 'GreatWorkClasses'
	AND name != 'MultiplayerOptions'
	AND name != 'ReplayDataSets'
	AND name != 'HistoricRankings';

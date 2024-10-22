-- Requires construction of this building
ALTER TABLE LeagueSpecialSessions ADD BuildingTrigger text REFERENCES Buildings (Type);

-- Requires passing this resolution
ALTER TABLE LeagueSpecialSessions ADD TriggerResolution text REFERENCES Resolutions (Type);

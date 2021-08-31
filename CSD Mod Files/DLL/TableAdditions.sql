-- Insert SQL Rules Here 

ALTER TABLE Resolutions ADD COLUMN 'OpenDoor' boolean default 0;

ALTER TABLE Resolutions ADD COLUMN 'SphereOfInfluence' boolean default 0;

ALTER TABLE Resolutions ADD COLUMN 'Decolonization' boolean default 0;

ALTER TABLE Resolutions ADD COLUMN 'SpaceshipProductionMod' integer default 0;

ALTER TABLE Resolutions ADD COLUMN 'SpaceshipPurchaseMod' integer default 0;

ALTER TABLE Resolutions ADD COLUMN 'IsWorldWar' integer default 0;

ALTER TABLE Resolutions ADD COLUMN 'EmbargoIdeology' boolean default false;

ALTER TABLE LeagueProjectRewards ADD COLUMN 'AttackBonusTurns' integer default 0;

ALTER TABLE LeagueProjectRewards ADD COLUMN 'BaseFreeUnits' integer default 0;

ALTER TABLE LeagueProjectRewards ADD COLUMN 'GetNumFreeGreatPeople' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'FaithToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'CapitalsToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'DoFToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'RAToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'GPExpendInfluence' integer default 0;

ALTER TABLE Units ADD COLUMN 'NumInfPerEra' integer default 0;

ALTER TABLE Policies ADD COLUMN 'GreatDiplomatRateModifier' integer default 0;

ALTER TABLE Improvements ADD COLUMN 'ImprovementLeagueVotes' integer default 0;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'BuildingTrigger' text default NULL;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'TriggerResolution' text default NULL;

ALTER TABLE Improvements ADD COLUMN 'IsEmbassy' boolean default false;

-- CSD
ALTER TABLE Buildings ADD COLUMN 'DPToVotes' integer default 0;

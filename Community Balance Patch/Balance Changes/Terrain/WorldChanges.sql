ALTER TABLE Resources		ADD NumPerTerritory						variant default 0;
ALTER TABLE Resources		ADD MutuallyExclusiveGroup				integer default -1;
ALTER TABLE Resources		ADD AIAvailability						integer default 0;
ALTER TABLE Resources		ADD Water								boolean;
ALTER TABLE Resources		ADD Land								boolean;
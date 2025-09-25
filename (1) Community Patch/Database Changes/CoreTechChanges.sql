-- Community Patch activates this unused BNW column, so make Mining (which unlocks chopping) grant 100% chopping bonuses
UPDATE Technologies SET FeatureProductionModifier = 100 WHERE Type = 'TECH_MINING';

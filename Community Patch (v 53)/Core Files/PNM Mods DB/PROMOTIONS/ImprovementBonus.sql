ALTER TABLE UnitPromotions
  ADD NearbyImprovementCombatBonus INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions
  ADD NearbyImprovementBonusRange INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions
  ADD CombatBonusImprovement INTEGER DEFAULT -1;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('PROMOTIONS_IMPROVEMENT_BONUS', 1);
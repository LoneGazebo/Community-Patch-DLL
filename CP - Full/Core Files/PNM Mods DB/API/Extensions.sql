ALTER TABLE Buildings
  ADD AddsFreshWater INTEGER DEFAULT 0;
ALTER TABLE Buildings
  ADD PurchaseOnly INTEGER DEFAULT 0;
ALTER TABLE Building_ThemingBonuses
  ADD ConsecutiveEras INTEGER DEFAULT 0;

ALTER TABLE Improvements
  ADD AddsFreshWater INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('API_EXTENSIONS', 1);

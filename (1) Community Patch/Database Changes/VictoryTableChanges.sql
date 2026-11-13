-- DLL needs to know what a Victory's preferred strategy is (victory may exist when unique strategy doesn't)
ALTER TABLE Victories ADD PreferredGrandStrategy text REFERENCES AIGrandStrategies (Type);

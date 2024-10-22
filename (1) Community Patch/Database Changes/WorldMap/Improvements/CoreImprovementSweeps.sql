-- Unhardcode GPTIs connecting all resources, but still make it do so by default
UPDATE Improvements SET ConnectsAllResources = 1 WHERE CreatedByGreatPerson = 1;

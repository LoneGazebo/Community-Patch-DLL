-- For some reason the Firaxis DLL hardcoded "City-State Territory = Friendly Territory" to Greece's CityStateFriendshipModifier bonus,
-- despite having a separate, functional trait column for this ability.
-- The Community Patch unhardcodes it and assigns the ability to Greece for identical functionality.
-- Additionally, the Community Patch decouples the Influence decay and recovery modifiers for greater modularity.
UPDATE Traits
SET
	AngerFreeIntrusionOfCityStates = 1,
	CityStateRecoveryModifier = 100
WHERE Type = 'TRAIT_CITY_STATE_FRIENDSHIP';

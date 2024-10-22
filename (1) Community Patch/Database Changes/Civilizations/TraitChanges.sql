-- For some reason the Firaxis DLL hardcoded "City-State Territory = Friendly Territory" to Greece's CityStateFriendshipModifier bonus,
-- despite having a separate, functional trait column for this ability.
-- The Community Patch unhardcodes it and assigns the ability to Greece for identical functionality.
UPDATE Traits
SET AngerFreeIntrusionOfCityStates = 1
WHERE Type = 'TRAIT_CITY_STATE_FRIENDSHIP';

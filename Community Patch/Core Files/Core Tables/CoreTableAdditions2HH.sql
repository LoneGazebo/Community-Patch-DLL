-- BUILDING: PlayerBorderGainlessPillage & CityGainlessPillage
-- If such a building's effect applies, other teams get neither gold nor heal from pillaging the appropriate tiles.
-- CityGainlessPillage affects the constructing city's worked tiles, PlayerBorderGainlessPillage proofs every tile of the player
ALTER TABLE Buildings ADD PlayerBorderGainlessPillage BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD CityGainlessPillage BOOLEAN DEFAULT 0;
	
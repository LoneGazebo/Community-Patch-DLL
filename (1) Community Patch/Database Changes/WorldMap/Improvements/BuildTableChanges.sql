-- Disables this build for players who own this tech
ALTER TABLE Builds ADD ObsoleteTech text REFERENCES Technologies (Type);

-- Only civilians are consumed on completing this build if Kill = true
ALTER TABLE Builds ADD KillOnlyCivilian boolean DEFAULT 0;

-- Spawn best Melee Type Unit on finishing a Build (accounts for Domain of Build)
ALTER TABLE Builds ADD IsFreeBestDomainUnit boolean DEFAULT 0;

-- Build adds an instant yield of culture to Player's culture pool.
ALTER TABLE Builds ADD CultureBoost boolean DEFAULT 0;

-- This build only removes the specified feature but doesn't create an improvement
ALTER TABLE BuildFeatures ADD RemoveOnly integer DEFAULT 0;

-- Only works for RemoveOnly rows. Disables this build AND all other builds that remove this feature for players who own this tech.
ALTER TABLE BuildFeatures ADD ObsoleteTech text REFERENCES Technologies (Type);

-- The game engine picks the worker's tool animation by the build's Type (a hammer for any build it does not know).
-- Set this to a base game build's Type to use that build's animation. Game rules still use this build's own Type.
ALTER TABLE Builds ADD EngineType text REFERENCES Builds (Type);

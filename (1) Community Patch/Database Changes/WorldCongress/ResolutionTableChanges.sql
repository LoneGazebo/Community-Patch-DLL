-- Tourism modifier between all player pairs
ALTER TABLE Resolutions ADD TourismMod integer DEFAULT 0;

-- The target city state meets all players. Influence of the target city state for all players is set to 40.
-- No players can be allied to the target city state.
ALTER TABLE Resolutions ADD OpenDoor boolean DEFAULT 0;

-- Proposer is permanently allied to the target city state. Can only be proposed when allied to the target city state.
ALTER TABLE Resolutions ADD SphereOfInfluence boolean DEFAULT 0;

-- Influence of all city states for all players is lowered to 50 if above, unless permanently allied.
ALTER TABLE Resolutions ADD Decolonization boolean DEFAULT 0;

-- Production modifier towards spaceship parts
ALTER TABLE Resolutions ADD SpaceshipProductionMod integer DEFAULT 0;

-- Investment cost modifier for spaceship parts
ALTER TABLE Resolutions ADD SpaceshipPurchaseMod integer DEFAULT 0;

-- 2x warmonger score decay speed, 0.5x warmonger score changes from declaring war, using nukes, and capturing/liberating cities
ALTER TABLE Resolutions ADD IsWorldWar integer DEFAULT 0;

-- Cannot send trade routes to major civs with differing ideology and non-allied city states
-- Influence of allied city states doesn't decay
ALTER TABLE Resolutions ADD EmbargoIdeology boolean DEFAULT 0;

-- Modifier for vassal maintenance
ALTER TABLE Resolutions ADD VassalMaintenanceGoldPercent integer DEFAULT 0;

-- All vassals are immediately liberated. Policies.VassalsNoRebel column prevents this if master has the policy.
ALTER TABLE Resolutions ADD EndAllCurrentVassals boolean DEFAULT 0;

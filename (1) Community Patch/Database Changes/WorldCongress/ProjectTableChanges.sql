-- +POLICY_ATTACK_BONUS_MOD% attack for all melee units for the specified number of turns. Duration stacks with Policies.AttackBonusTurns.
ALTER TABLE LeagueProjectRewards ADD AttackBonusTurns integer DEFAULT 0;

-- Number of units that become maintenance-free
ALTER TABLE LeagueProjectRewards ADD BaseFreeUnits integer DEFAULT 0;

-- Number of free great people
ALTER TABLE LeagueProjectRewards ADD GetNumFreeGreatPeople integer DEFAULT 0;

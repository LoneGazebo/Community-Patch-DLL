print("This is the 'Global - City Bombard Range' mod script.")


local gTeamRange = {}
local gBombardTechs = {}

local gDefaultRange = GameDefines.MAX_CITY_ATTACK_RANGE * ((GameDefines.CAN_CITY_USE_INDIRECT_FIRE == 1) and -1 or 1)

function Initialise()
  for row in DB.Query("SELECT ID, BombardRange, BombardIndirect FROM Technologies WHERE BombardRange > 0 ORDER BY BombardRange ASC, BombardIndirect ASC") do
    local iTech = row.ID
    local iRange = math.min(GameDefines.MAX_CITY_ATTACK_RANGE, row.BombardRange) * ((row.BombardIndirect == 1) and -1 or 1)
  
    gBombardTechs[iTech] = iRange
  
    for iTeam = 0, GameDefines.MAX_CIV_TEAMS, 1 do
      if (Teams[iTeam]:IsHasTech(iTech)) then
	    gTeamRange[iTeam] = iRange
	  end
	end
  end
end

function OnTechResearched(iTeam, iTech, iChange)
	if (iChange == 1 and gBombardTechs[iTech]) then
	  gTeamRange[iTeam] = gBombardTechs[iTech]
	end
end
GameEvents.TeamTechResearched.Add(OnTechResearched)

function OnGetBombardRange(iPlayer, iCity)
  return gTeamRange[Players[iPlayer]:GetTeam()] or gDefaultRange
end
GameEvents.GetBombardRange.Add(OnGetBombardRange)

Initialise()

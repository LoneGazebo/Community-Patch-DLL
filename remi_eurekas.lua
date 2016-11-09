
include("PlotIterators")

GameEvents.PlayerDoTurn.Add(function(iPlayer)
-- euraka that are re-computed at the begining of the turn (ex: gold in bank)
if(iPlayer >=0) then
	local pPlayer = Players[iPlayer];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pTeamTech = pTeam:GetTeamTechs();
	
	-- max culture per turn for Drama
	if(not pTeamTech:HasTech(GameInfo.Technologies.TECH_DRAMA.ID) then
		local iTech = GameInfo.Technologies.TECH_DRAMA.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		local culturePerTurn = player:GetTotalJONSCulturePerTurn();
		if(nbEureka<culturePerTurn) then
			pTeamTech:ChangeEurekaCounter(iTech, min(30,culturePerTurn));
		end
	end

	-- cs friends for philosophy
	if(not pTeamTech:HasTech(GameInfo.Technologies.TECH_PHILOSOPHY.ID) then
		local nbFriends = 0
		for i = 0, GameDefines.MAX_PLAYERS - 1 do
			if i ~= iPlayer then
				if Players[i]:IsMinorCiv() then
					if 1 == Players[i]:GetMinorCivFriendshipLevelWithMajor(iPlayer) then
						nbFriends = nbFriends +1
					end
				end
			end
		end
		local iTech = GameInfo.Technologies.TECH_PHILOSOPHY.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		local culturePerTurn = player:GetTotalJONSCulturePerTurn();
		if(nbEureka<nbFriends) then
			pTeamTech:ChangeEurekaCounter(iTech, min(5,nbFriends));
		end
	end

	-- max income per turn for CURRENCY
	if(not pTeamTech:HasTech(GameInfo.Technologies.TECH_CURRENCY.ID) then
		local iTech = GameInfo.Technologies.TECH_CURRENCY.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		local goldPerTurn = player:CalculateGoldRate();
		if(nbEureka<goldPerTurn) then
			pTeamTech:ChangeEurekaCounter(iTech, min(30,goldPerTurn));
		end
	end
end
end);


GameEvents.TileFeatureChanged.Add(function(x, y, iPlayer, iFeature, eNewFeature)
if(iPlayer >=0 && iFeature == FeatureTypes.FEATURE_FOREST && eNewFeature == FeatureTypes.NO_FEATURE) then
	local pPlayer = Players[iPlayer];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pTeamTech = pTeam:GetTeamTechs();
	-- nbForest cleared for metal casting
	if(not pTeamTech:HasTech(GameInfo.Technologies.TECH_METAL_CASTING.ID) then
		local iTech = GameInfo.Technologies.TECH_METAL_CASTING.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<6) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
end
end);

--activate TileFeatureChanged event
UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'EVENTS_TILE_IMPROVEMENTS';



GameEvents.GreatPersonExpended.Add(function(iPlayer, iUnit, iUnitType, iX, iY)
if(iPlayer >=0 && iUnitType == GameInfo.Units.UNIT_ENGINEER.ID) then
	local pPlayer = Players[iPlayer];
	local pTeam = Teams[pPlayer:GetTeam()];
	local pTeamTech = pTeam:GetTeamTechs();
	-- nbGreat engineer used for ENGINEERING
	if(not pTeamTech:HasTech(GameInfo.Technologies.TECH_ENGINEERING.ID) then
		local iTech = GameInfo.Technologies.TECH_ENGINEERING.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<2) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
end
end);

--activate TileFeatureChanged event
UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'EVENTS_GREAT_PEOPLE';

GameEvents.CityTrained.Add(function(iPlayer, iCity, iUnit, bGoldBuy, bFaithBuy)
if(iPlayer >=0 && iUnit>=0) then
	local pPlayer = Players[iPlayer];
	local pUnit = pPlayer:GetUnitByID(iUnit);
	local iUnitType = pUnit:GetUnitType();
	--scout for trapping
	if(iUnitType == GameInfo.Units.UNIT_SCOUT.ID or iUnitType == GameInfo.Units.UNIT_SHOSHONE_PATHFINDER.ID) then 
		local iTech = GameInfo.Technologies["TECH_TRAPPING"].ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<3) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	--workers for animal husbandery
	if(iUnitType == GameInfo.Units.UNIT_WORKER.ID) then 
		local iTech = GameInfo.Technologies["TECH_ANIMAL_HUSBANDRY"].ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<3) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	--warriors for mining
	if(iUnitType == GameInfo.Units.UNIT_WARRIOR.ID or iUnitType == GameInfo.Units.UNIT_AZTEC_JAGUAR.ID) then 
		local iTech = GameInfo.Technologies["TECH_MINING"].ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<5) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	--archers for military training
	if(iUnitType == GameInfo.Units.UNIT_ARCHER.ID or iUnitType == GameInfo.Units.UNIT_INCAN_SLINGER.ID) then 
		local iTech = GameInfo.Technologies["TECH_HORSEBACK_RIDING"].ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<6) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	-- fishing boat for sailing
	if(iUnitType == GameInfo.Units.UNIT_WORKBOAT.ID) then 
		local iTech = GameInfo.Technologies["TECH_OPTICS"].ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<6) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	-- SPEARMAN for iron working
	if(iUnitType == GameInfo.Units.UNIT_SPEARMAN.ID or iUnitType == GameInfo.Units.UNIT_GREEK_HOPLITE.ID or iUnitType == GameInfo.Units.UNIT_CELT_PICTISH_WARRIOR.ID ) then 
		local iTech = GameInfo.Technologies["TECH_IRON_WORKING"].ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<6) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
end
end);

GameEvents.CityConstructed.Add(function(iPlayer, iCity, iBuilding, bGoldBuy, bFaithBuy)
if(iPlayer >=0 && iBuilding>=0) then
	local pTeamTech = Teams[Players[iPlayer]:GetTeam()]:GetTeamTechs();
	-- shrine and monument for wheel
	if(iBuilding == GameInfo.Buildings.BUILDING_SHRINE.ID or iBuilding == GameInfo.Buildings.BUILDING_MONUMENT.ID) then 
		local iTech = GameInfo.Technologies.TECH_THE_WHEEL.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<3) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	-- buildings for construction (named masonery, but it's construction)
	if(not pTeamTech:HasTech(GameInfo.Technologies.TECH_MASONRY.ID) then
		local iTech = GameInfo.Technologies.TECH_MASONRY.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<10) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	-- market for mathematics
	if(iBuilding == GameInfo.Buildings.BUILDING_MARKET.ID or iBuilding == GameInfo.Buildings.BUILDING_BAZAAR.ID) then 
		local iTech = GameInfo.Technologies.TECH_MATHEMATICS.ID;
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<3) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
end
end);


--activate CityTrained event
UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'EVENTS_CITY';

GameEvents.PlayerAdoptPolicy.Add(function(iPlayer, iPolicy)
if(iPlayer >=0 && iPolicy>=0) then
	-- number of policies for pottery 
	local iTech = GameInfo.Technologies.TECH_POTTERY.ID;
	local pTeamTech = Teams[Players[iPlayer]:GetTeam()]:GetTeamTechs();
	local nbEureka = pTeamTech:GetEurekaCounter(iTech);
	if(nbEureka<3) then
		pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
	end
end
end);


Events.SerialEventCityCreated.Add( function(vHexPos, iPlayer, iCity, artStyleType, iEra, iContinent, iPopulationSize, iSize, iFogState)
if(iPlayer >=0 && iCity>=0) then
	local pPlayer = Players[iPlayer];
	local pCity = pPlayer:GetCityByID(iCity);
	local pTeam = Teams[pPlayer:GetTeam()];
	-- number of coastal city for fishing
	if(pCity:IsCoastal(0)) then
		-- (note: TECH_SAILING => fishing and TECH_OPTICS => sailing) 
		local iTech = GameInfo.Technologies.TECH_SAILING.ID;
		local pTeamTech = Teams[pPlayer:GetTeam()]:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<3) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
	-- count nb of thing near founded cities (at 2 hex)
	local nbLuxPlant = 0
	local nbForest = 0
	local nbDesert = 0
	local nbMountain = 0
	for pAreaPlot in PlotAreaSweepIterator(pCity:Plot(), 2) do
		if(pAreaPlot:GetRevealedImprovementType(pTeam, false) == GameInfo.Improvements.IMPROVEMENT_PLANTATION.ID) then 
			nbLuxPlant = nbLuxPlant + 1
		end
		if(pAreaPlot:GetFeatureType() == FeatureTypes.FEATURE_FOREST) then 
			nbForest = nbForest + 1
		end
		if(pAreaPlot:GetTerrainType () == TerrainTypes.TERRAIN_DESERT) then 
			nbDesert = nbDesert + 1
		end
		if(pAreaPlot:GetTerrainType () == TerrainTypes.TERRAIN_MOUNTAIN) then 
			nbMountain = nbMountain + 1
		end
	end
	--number of nearby (lux, because bananas aren't revealed) plantation at 2 hex of a new city for CALENDAR
	if(nbLuxPlant>0) then
		local iTech = GameInfo.Technologies.TECH_CALENDAR.ID;
		local pTeamTech = pTeam:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<5) then
			pTeamTech:ChangeEurekaCounter(iTech, min(5, nbLuxPlant+nbEureka));
		end
	end
	--number of nearby desert & montains at 2 hex of a new city for trading
	if(nbDesert+nbMountain>0) then
		local iTech = GameInfo.Technologies.TECH_ARCHERY.ID;
		local pTeamTech = pTeam:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<15) then
			pTeamTech:ChangeEurekaCounter(iTech, min(15, nbMountain+nbMountain+nbDesert+nbEureka));
		end
	end
	--number of nearby forest at 2 hex of a new city for Bronze
	if(nbForest>0) then
		local iTech = GameInfo.Technologies.TECH_BRONZE_WORKING.ID;
		local pTeamTech = pTeam:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<15) then
			pTeamTech:ChangeEurekaCounter(iTech, min(15, nbForest+nbEureka));
		end
	end
	--number of fresh water city for masonery
	if(pCity:Plot():IsFreshWater()) then
		local iTech = GameInfo.Technologies.TECH_CONSTRUCTION.ID;
		local pTeamTech = pTeam:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<3) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end
end
end);

GameEvents.TeamTechResearched.Add(function(iTeam, iTech, iChange)
if(iTeam >=0 && iTech>=0) then
	local pTeam = Teams[iTeam];
	-- number of classical era tech for writing
	if(iTech == GameInfo.Technologies.TECH_OPTICS.ID or
	iTech == GameInfo.Technologies.TECH_MATHEMATICS.ID or
	iTech == GameInfo.Technologies.TECH_CONSTRUCTION.ID or
	iTech == GameInfo.Technologies.TECH_IRON_WORKING.ID or
	iTech == GameInfo.Technologies.TECH_CURRENCY.ID or
	iTech == GameInfo.Technologies.TECH_METAL_CASTING.ID) then
		local iTech = GameInfo.Technologies.TECH_WRITING.ID;
		local pTeamTech = pTeam:GetTeamTechs();
		local nbEureka = pTeamTech:GetEurekaCounter(iTech);
		if(nbEureka<6) then
			pTeamTech:ChangeEurekaCounter(iTech, nbEureka+1);
		end
	end

end);







ALTER TABLE Technology ADD EurekaPerMillion INTEGER DEFAULT 0;

-- 10 000 = 1% reduction per eureka count
--ancient
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_TRAPPING';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_POTTERY';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_THE_WHEEL';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_ANIMAL_HUSBANDRY';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_MINING';

UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_SAILING';
UPDATE Technology SET EurekaPerMillion = 20000 WHERE Type = 'TECH_ARCHERY';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_CALENDAR';
UPDATE Technology SET EurekaPerMillion = 50000 WHERE Type = 'TECH_HORSEBACK_RIDING';
UPDATE Technology SET EurekaPerMillion = 30000 WHERE Type = 'TECH_MASONRY';
UPDATE Technology SET EurekaPerMillion = 20000 WHERE Type = 'TECH_BRONZE_WORKING';
--classical
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_OPTICS';
UPDATE Technology SET EurekaPerMillion = 50000 WHERE Type = 'TECH_WRITING';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_MATHEMATICS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_CONSTRUCTION';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_IRON_WORKING';

UPDATE Technology SET EurekaPerMillion = 10000 WHERE Type = 'TECH_DRAMA';
UPDATE Technology SET EurekaPerMillion = 50000 WHERE Type = 'TECH_PHILOSOPHY';
UPDATE Technology SET EurekaPerMillion = 150000 WHERE Type = 'TECH_ENGINEERING';
UPDATE Technology SET EurekaPerMillion = 10000 WHERE Type = 'TECH_CURRENCY';
UPDATE Technology SET EurekaPerMillion = 50000 WHERE Type = 'TECH_METAL_CASTING';

--- done to here
--mediaval
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_EDUCATION';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_THEOLOGY';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_CHIVALRY';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_PHYSICS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_STEEL';

UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_CIVIL_SERVICE';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_GUILDS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_COMPASS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_MACHINERY';
--renaissance
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_BANKING';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_PRINTING_PRESS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_ASTRONOMY';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_GUNPOWDER';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_CHEMISTRY';

UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_ARCHITECTURE';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_ECONOMICS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_ACOUSTICS';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_NAVIGATION';
UPDATE Technology SET EurekaPerMillion = 100000 WHERE Type = 'TECH_METALLURGY';






--- to separate
--
-- Plot iterator functions
--   All functions create an iterator centered on pPlot starting in the specified sector and working in the given direction around the ring/area
--   PlotRingIterator(pPlot, r, sector, anticlock) returns each plot around a border of radius r
--   PlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre) returns each plot within an area of radius r in concentric rings
--   PlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre) returns each plot within an area of radius r by radial
--
-- By converting the Map (x,y) co-ordinates to hex (x,y,z) co-ordinates we can simply walk the edges of the bounding hex
-- To walk an edge we keep one of x, y or z invariant, iterate the others from 0 to r-1 and r-1 to 0  (r-1 otherwise we include each vertex twice)
--
-- By using Lua coroutines we can keep the walking code (fairly) simple
--
-- Use these functions as follows
--
--    for pEdgePlot in PlotRingIterator(pPlot, r, sector, anticlock) do
--      print(pEdgePlot:GetX(), pEdgePlot:GetY())
--    end
--
--    for pAreaPlot in PlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre) do
--      print(pAreaPlot:GetX(), pAreaPlot:GetY())
--    end
--
--    for pAreaPlot in PlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre) do
--      print(pAreaPlot:GetX(), pAreaPlot:GetY())
--    end
--

SECTOR_NORTH = 1
SECTOR_NORTHEAST = 2
SECTOR_SOUTHEAST = 3
SECTOR_SOUTH = 4
SECTOR_SOUTHWEST = 5
SECTOR_NORTHWEST = 6

DIRECTION_CLOCKWISE = false
DIRECTION_ANTICLOCKWISE = true

DIRECTION_OUTWARDS = false
DIRECTION_INWARDS = true

CENTRE_INCLUDE = true
CENTRE_EXCLUDE = false

function PlotRingIterator(pPlot, r, sector, anticlock)
  --print(string.format("PlotRingIterator((%i, %i), r=%i, s=%i, d=%s)", pPlot:GetX(), pPlot:GetY(), r, (sector or SECTOR_NORTH), (anticlock and "rev" or "fwd")))
  -- The important thing to remember with hex-coordinates is that x+y+z = 0
  -- so we never actually need to store z as we can always calculate it as -(x+y)
  -- See http://keekerdc.com/2011/03/hexagon-grids-coordinate-systems-and-distance-calculations/

  if (pPlot ~= nil and r > 0) then
    local hex = ToHexFromGrid({x=pPlot:GetX(), y=pPlot:GetY()})
    local x, y = hex.x, hex.y

    -- Along the North edge of the hex (x-r, y+r, z) to (x, y+r, z-r)
    local function north(x, y, r, i) return {x=x-r+i, y=y+r} end
    -- Along the North-East edge (x, y+r, z-r) to (x+r, y, z-r)
    local function northeast(x, y, r, i) return {x=x+i, y=y+r-i} end
    -- Along the South-East edge (x+r, y, z-r) to (x+r, y-r, z)
    local function southeast(x, y, r, i) return {x=x+r, y=y-i} end
    -- Along the South edge (x+r, y-r, z) to (x, y-r, z+r)
    local function south(x, y, r, i) return {x=x+r-i, y=y-r} end
    -- Along the South-West edge (x, y-r, z+r) to (x-r, y, z+r)
    local function southwest(x, y, r, i) return {x=x-i, y=y-r+i} end
    -- Along the North-West edge (x-r, y, z+r) to (x-r, y+r, z)
    local function northwest(x, y, r, i) return {x=x-r, y=y+i} end

    local side = {north, northeast, southeast, south, southwest, northwest}
    if (sector) then
      for i=(anticlock and 1 or 2), sector, 1 do
        table.insert(side, table.remove(side, 1))
      end
    end

    -- This coroutine walks the edges of the hex centered on pPlot at radius r
    local next = coroutine.create(function ()
      if (anticlock) then
        for s=6, 1, -1 do
          for i=r, 1, -1 do
            coroutine.yield(side[s](x, y, r, i))
          end
        end
      else
        for s=1, 6, 1 do
          for i=0, r-1, 1 do
            coroutine.yield(side[s](x, y, r, i))
          end
        end
      end

      return nil
    end)

    -- This function returns the next edge plot in the sequence, ignoring those that fall off the edges of the map
    return function ()
      local pEdgePlot = nil
      local success, hex = coroutine.resume(next)
      --if (hex ~= nil) then print(string.format("hex(%i, %i, %i)", hex.x, hex.y, -1 * (hex.x+hex.y))) else print("hex(nil)") end

      while (success and hex ~= nil and pEdgePlot == nil) do
        pEdgePlot = Map.GetPlot(ToGridFromHex(hex.x, hex.y))
        if (pEdgePlot == nil) then success, hex = coroutine.resume(next) end
      end

      return success and pEdgePlot or nil
    end
  else
    -- Iterators have to return a function, so return a function that returns nil
    return function () return nil end
  end
end


function PlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre)
  --print(string.format("PlotAreaSpiralIterator((%i, %i), r=%i, s=%i, d=%s, w=%s, c=%s)", pPlot:GetX(), pPlot:GetY(), r, (sector or SECTOR_NORTH), (anticlock and "rev" or "fwd"), (inwards and "in" or "out"), (centre and "yes" or "no")))
  -- This coroutine walks each ring in sequence
  local next = coroutine.create(function ()
    if (centre and not inwards) then
      coroutine.yield(pPlot)
    end

    if (inwards) then
      for i=r, 1, -1 do
        for pEdgePlot in PlotRingIterator(pPlot, i, sector, anticlock) do
          coroutine.yield(pEdgePlot)
        end
      end
    else
      for i=1, r, 1 do
        for pEdgePlot in PlotRingIterator(pPlot, i, sector, anticlock) do
          coroutine.yield(pEdgePlot)
        end
      end
    end

    if (centre and inwards) then
      coroutine.yield(pPlot)
    end

    return nil
  end)

  -- This function returns the next plot in the sequence
  return function ()
    local success, pAreaPlot = coroutine.resume(next)
    return success and pAreaPlot or nil
  end
end


function PlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre)
  print(string.format("PlotAreaSweepIterator((%i, %i), r=%i, s=%i, d=%s, w=%s, c=%s)", pPlot:GetX(), pPlot:GetY(), r, (sector or SECTOR_NORTH), (anticlock and "rev" or "fwd"), (inwards and "in" or "out"), (centre and "yes" or "no")))
  -- This coroutine walks each radial in sequence
  local next = coroutine.create(function ()
    if (centre and not inwards) then
      coroutine.yield(pPlot)
    end

    local iterators = {}
    for i=1, r, 1 do
      iterators[i] = PlotRingIterator(pPlot, i, sector, anticlock)
    end

    for s=1, 6, 1 do
      -- In ring (n+1) there are (n+1)*6 or 6n + 6 plots,
      -- so we need to call the (n+1)th iterator six more times than the nth, or once more per outer loop
      -- eg for 3 rings we need a plot from ring 1, 2, 3, 2, 3, 3 repeated 6 times
      if (inwards) then
        for i=r, 1, -1 do
          for j=r, i, -1 do
            coroutine.yield(iterators[j]())
          end
        end
      else
        for i=1, r, 1 do
          for j=i, r, 1 do
            coroutine.yield(iterators[j]())
          end
        end
      end
    end

    if (centre and inwards) then
      coroutine.yield(pPlot)
    end

    return nil
  end)

  -- This function returns the next plot in the sequence
  return function ()
    local success, pAreaPlot = coroutine.resume(next)
    return success and pAreaPlot or nil
  end
end


--
-- Test functions
--

highlights = {
  RED     = {x=1.0, y=0.0, z=0.0, w=1.0},
  GREEN   = {x=0.0, y=1.0, z=0.0, w=1.0},
  BLUE    = {x=0.0, y=0.0, z=1.0, w=1.0},
  CYAN    = {x=0.0, y=1.0, z=1.0, w=1.0},
  YELLOW  = {x=1.0, y=1.0, z=0.0, w=1.0},
  MAGENTA = {x=1.0, y=0.0, z=1.0, w=1.0},
  BLACK   = {x=0.5, y=0.5, z=0.5, w=1.0}
}

function TestPlotHighlight(pPlot, highlight)
  print(pPlot:GetX(), pPlot:GetY())
  if (highlight ~= nil) then
    Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY())), true, highlight)
  end
end

function TestPlotRingIterator(pPlot, r, sector, anticlock, highlight)
  for pEdgePlot in PlotRingIterator(pPlot, r, sector, anticlock) do
    TestPlotHighlight(pEdgePlot, highlight)
  end
end

function TestPlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre, highlight)
  for pAreaPlot in PlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre) do
    TestPlotHighlight(pAreaPlot, highlight)
  end
end

function TestPlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre, highlight)
  for pAreaPlot in PlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre) do
    TestPlotHighlight(pAreaPlot, highlight)
  end
end

-- TestPlotRingIterator(Players[0]:GetCapitalCity():Plot(), 4, SECTOR_NORTH, DIRECTION_CLOCKWISE, highlights.RED)
-- TestPlotAreaSpiralIterator(Players[0]:GetCapitalCity():Plot(), 3, SECTOR_SOUTH, DIRECTION_ANTICLOCKWISE, DIRECTION_OUTWARDS, CENTRE_INCLUDE)
-- TestPlotAreaSweepIterator(Players[0]:GetCapitalCity():Plot(), 3, SECTOR_SOUTH, DIRECTION_ANTICLOCKWISE, DIRECTION_OUTWARDS, CENTRE_INCLUDE)

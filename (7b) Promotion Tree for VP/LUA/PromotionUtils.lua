--
-- Promotion Utility functions
--

--
-- Unit promotion functions
--

function HasPromotion(pUnit, iPromotion)
  return (pUnit and pUnit:IsHasPromotion(iPromotion))
end

function CanAcquirePromotion(pUnit, iPromotion)
  if (pUnit and pUnit:CanAcquirePromotion(iPromotion)) then
    local iAction = GetActionForPromotion(iPromotion)
	return (iAction and Game.CanHandleAction(iAction))
  end

  return false
end


--
-- Action promotion functions
--

function GetActionForPromotion(iPromotion)
  local promotion = GameInfo.UnitPromotions[iPromotion]

  for iAction, action in pairs(GameInfoActions) do
    if (action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION and action.Type == promotion.Type) then
	  return iAction
    end
  end

  return nil
end


--
-- Promotions database lookup functions
--
local sMatchRank = "_[0-9IV]+$"  -- UndeadDevel: improved this a tad

function IsRankedPromotion(sPromotion)  -- UndeadDevel: need a lot more exceptions here...ranked promos without a rank identifier
  if (sPromotion == "PROMOTION_MEDIC" or sPromotion == "PROMOTION_CHARGE" or sPromotion == "PROMOTION_ANTI_AIR" or sPromotion == "PROMOTION_SPLASH" or sPromotion == "PROMOTION_NAVAL_SENTRY") then
    return true
  end

  return (sPromotion:match(sMatchRank) ~= nil)
end

function GetPromotionBase(sPromotion)
  local sRank = sPromotion:match(sMatchRank) or ""
  return sPromotion:sub(1, sPromotion:len()-sRank:len())
end

function GetNextPromotion(sPromotion, sCombatClass)  -- UndeadDevel: we will now be checking whether the chained promos are class compatible as well (needed for Splash II, for example)
  if (not IsRankedPromotion(sPromotion)) then
    return nil
  end

  local sBase = GetPromotionBase(sPromotion)

  local promotions = {}
  local sPrereqs = "p1.Type = p2.PromotionPrereqOr1 OR p1.Type = p2.PromotionPrereqOr2 OR p1.Type = p2.PromotionPrereqOr3 OR p1.Type = p2.PromotionPrereqOr4 OR p1.Type = p2.PromotionPrereqOr5 OR p1.Type = p2.PromotionPrereqOr6 OR p1.Type = p2.PromotionPrereqOr7 OR p1.Type = p2.PromotionPrereqOr8 OR p1.Type = p2.PromotionPrereqOr9"
  local sQuery = "SELECT p2.Type FROM UnitPromotions p1, UnitPromotions p2, UnitPromotions_UnitCombats c WHERE p1.Type = ? AND (" .. sPrereqs .. ") AND p2.Type = c.PromotionType AND c.UnitCombatType = ? AND p2.Type LIKE ?"
  for row in DB.Query(sQuery, sPromotion, sCombatClass, sBase .. "%") do
    table.insert(promotions, row.Type)
  end
  
  return promotions[1]
end

function GetPromotionChain(sPromotion, sCombatClass)
  local promotions = {}

  repeat
	table.insert(promotions, sPromotion)

	sPromotion = GetNextPromotion(sPromotion, sCombatClass)
  until (sPromotion == nil)

  return promotions
end

function GetBasicPromotions(sCombatClass)
  local promotions = {}

  local sQuery = "SELECT p.Type FROM UnitPromotions p, UnitPromotions_UnitCombats c WHERE c.UnitCombatType = ? AND c.PromotionType = p.Type AND p.PromotionPrereqOr1 IS NULL AND NOT p.CannotBeChosen"
  for row in DB.Query(sQuery, sCombatClass) do
    if (IsRankedPromotion(row.Type)) then
      table.insert(promotions, row.Type)
	end
  end

  return promotions
end

function GetBasicPromotionsSpecialCases(typeOfUnit)  -- UndeadDevel: because of VP's great variability in units, which the vanilla system isn't geared for, we need a lot of exceptions
  local promotions = {}
  local sQuery = ""

  if (typeOfUnit == 0) then -- Helis: special Heli promo lines hook on the hovering unit promotion
    sQuery = "SELECT p.Type FROM UnitPromotions p, UnitPromotions_UnitCombats c WHERE c.UnitCombatType = \"UNITCOMBAT_ARCHER\" AND c.PromotionType = p.Type AND (p.PromotionPrereqOr1 IS NULL OR p.PromotionPrereqOr1 = \"PROMOTION_HOVERING_UNIT\") AND NOT p.CannotBeChosen"
  elseif (typeOfUnit == 1) then -- Mounted Archers: not the case for current base promos, but may be added in future: Mounted Archers can't use promos with the MinimumRangeRequired attribute
    sQuery = "SELECT p.Type FROM UnitPromotions p, UnitPromotions_UnitCombats c WHERE c.UnitCombatType = \"UNITCOMBAT_ARCHER\" AND c.PromotionType = p.Type AND p.PromotionPrereqOr1 IS NULL AND NOT p.CannotBeChosen AND IFNULL(p.MinimumRangeRequired, 0) = 0"
  elseif (typeOfUnit == 2) then -- regular Archers: same as above but with the MaximumRangeRequired attribute instead
    sQuery = "SELECT p.Type FROM UnitPromotions p, UnitPromotions_UnitCombats c WHERE c.UnitCombatType = \"UNITCOMBAT_ARCHER\" AND c.PromotionType = p.Type AND p.PromotionPrereqOr1 IS NULL AND NOT p.CannotBeChosen AND IFNULL(p.MaximumRangeRequired, 0) = 0"  
  elseif (typeOfUnit == 3) then -- Siege Weapons: normal, non-special SQL query; the special part comes after this at the bottom of this function
    sQuery = "SELECT p.Type FROM UnitPromotions p, UnitPromotions_UnitCombats c WHERE c.UnitCombatType = \"UNITCOMBAT_SIEGE\" AND c.PromotionType = p.Type AND p.PromotionPrereqOr1 IS NULL AND NOT p.CannotBeChosen"
  elseif (typeOfUnit == 4) then -- AA-Units: same as above with Siege Weapons
    sQuery = "SELECT p.Type FROM UnitPromotions p, UnitPromotions_UnitCombats c WHERE c.UnitCombatType = \"UNITCOMBAT_GUN\" AND c.PromotionType = p.Type AND p.PromotionPrereqOr1 IS NULL AND NOT p.CannotBeChosen"
  end

  for row in DB.Query(sQuery) do
    if (IsRankedPromotion(row.Type)) then
      table.insert(promotions, row.Type)
	end
  end
  
  if (typeOfUnit == 3) then  --Siege Weapons: insert Cover I after the rest since it has PrereqOrXs and adding it to the SQL query has bad results w.r.t. line ordering
    table.insert(promotions, "PROMOTION_COVER_1")
  elseif (typeOfUnit == 4) then  -- AA-Units: since these don't have UnitCombats for Gun units we need to add them individually or else they'll get chosen multiple times and as primaries
    table.insert(promotions, "PROMOTION_ANTI_AIR")
	table.insert(promotions, "PROMOTION_INTERCEPTION_I")
  end
  
  return promotions
end

function GetBasePromotions(sCombatClass)   -- UndeadDevel: special treatment needed for many unit classes
  local promotions = {}
  
  if (sCombatClass == "UNITCOMBAT_ARCHER") then
    for _, sPromotion in ipairs(GetBasicPromotionsSpecialCases(2)) do
	  table.insert(promotions, GetPromotionChain(sPromotion, sCombatClass))
    end
  elseif (sCombatClass == "UNITCOMBAT_MOUNTED_ARCHER") then
    for _, sPromotion in ipairs(GetBasicPromotionsSpecialCases(1)) do
	  table.insert(promotions, GetPromotionChain(sPromotion, "UNITCOMBAT_ARCHER"))
    end
  elseif (sCombatClass == "UNITCOMBAT_HELICOPTER") then
    for _, sPromotion in ipairs(GetBasicPromotionsSpecialCases(0)) do
	  table.insert(promotions, GetPromotionChain(sPromotion, "UNITCOMBAT_ARCHER"))
    end
  elseif (sCombatClass == "UNITCOMBAT_SIEGE") then
    for _, sPromotion in ipairs(GetBasicPromotionsSpecialCases(3)) do
	  table.insert(promotions, GetPromotionChain(sPromotion, sCombatClass))
    end
  elseif (sCombatClass == "UNITCOMBAT_AA") then
    for _, sPromotion in ipairs(GetBasicPromotionsSpecialCases(4)) do
	  table.insert(promotions, GetPromotionChain(sPromotion, "UNITCOMBAT_GUN"))
    end
  else
    for _, sPromotion in ipairs(GetBasicPromotions(sCombatClass)) do
	  table.insert(promotions, GetPromotionChain(sPromotion, sCombatClass))
    end
  end
  
  return promotions
end

function GetDependentPromotions(sCombatClass, sPromotion)  -- UndeadDevel: still some special treatment needed for dependent promos (mostly archery unit distinctions)
  local promotions = {}

  if (IsRankedPromotion(sPromotion)) then
    local sBase = GetPromotionBase(sPromotion)
    local sPrereqs = "p1.Type = p2.PromotionPrereqOr1 OR p1.Type = p2.PromotionPrereqOr2 OR p1.Type = p2.PromotionPrereqOr3 OR p1.Type = p2.PromotionPrereqOr4 OR p1.Type = p2.PromotionPrereqOr5 OR p1.Type = p2.PromotionPrereqOr6 OR p1.Type = p2.PromotionPrereqOr7 OR p1.Type = p2.PromotionPrereqOr8 OR p1.Type = p2.PromotionPrereqOr9"
	
	if (sCombatClass == "UNITCOMBAT_HELICOPTER") then
	  sCombatClass = "UNITCOMBAT_MOUNTED_ARCHER"        -- special treatment was only necessary for base promos; now it behaves like mounted archer (still kinda special, I suppose)
	elseif (sCombatClass == "UNITCOMBAT_AA") then
	  sCombatClass = "UNITCOMBAT_GUN"                   -- special treatment was only necessary for base promos
	end
	
	local sQuery = ""
    if (sCombatClass == "UNITCOMBAT_ARCHER") then
    sQuery = "SELECT p2.Type FROM UnitPromotions p1, UnitPromotions p2, UnitPromotions_UnitCombats c WHERE p1.Type = ? AND (" .. sPrereqs .. ") AND p2.Type = c.PromotionType AND c.UnitCombatType = ? AND p2.Type NOT LIKE ? AND IFNULL(p2.MaximumRangeRequired, 0) = 0"
      for row in DB.Query(sQuery, sPromotion, sCombatClass, sBase .. "%") do
	    table.insert(promotions, GetPromotionChain(row.Type, sCombatClass))
      end
    elseif (sCombatClass == "UNITCOMBAT_MOUNTED_ARCHER") then
      sQuery = "SELECT p2.Type FROM UnitPromotions p1, UnitPromotions p2, UnitPromotions_UnitCombats c WHERE p1.Type = ? AND (" .. sPrereqs .. ") AND p2.Type = c.PromotionType AND c.UnitCombatType = \"UNITCOMBAT_ARCHER\" AND p2.Type NOT LIKE ? AND IFNULL(p2.MinimumRangeRequired, 0) = 0"
      for row in DB.Query(sQuery, sPromotion, sBase .. "%") do
	    table.insert(promotions, GetPromotionChain(row.Type, "UNITCOMBAT_ARCHER"))
      end
    else
      sQuery = "SELECT p2.Type FROM UnitPromotions p1, UnitPromotions p2, UnitPromotions_UnitCombats c WHERE p1.Type = ? AND (" .. sPrereqs .. ") AND p2.Type = c.PromotionType AND c.UnitCombatType = ? AND p2.Type NOT LIKE ?"
      for row in DB.Query(sQuery, sPromotion, sCombatClass, sBase .. "%") do
        table.insert(promotions, GetPromotionChain(row.Type, sCombatClass))
      end
    end	
  end

  return promotions
end
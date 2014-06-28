-- CEA_Promotions
-- Author: Thalassicus
-- DateCreated: 7/13/2013 4:27:24 PM
--------------------------------------------------------------

include("MT_Events.lua")

local log = Events.LuaLogger:New()
log:SetLevel("TRACE")

print("CEA_Promotions.lua")

--[[

function UpdatePromotions(unit, isUpgrading)
	if not unit or not unit:IsCombatUnit() then
		return
	end

	local needsAstronomy = GameInfo.UnitPromotions.PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY.ID
	if unit:IsHasPromotion(needsAstronomy) and Players[unit:GetOwner()]:GetTraitInfo().EmbarkedAllWater then
		unit:SetHasPromotion(needsAstronomy, false)
	end

	if unit:GetDomainType() == DomainTypes.DOMAIN_SEA then
		return
	end

	local unitInfo = GameInfo.Units[unit:GetUnitType()]
	if isUpgrading then
		unitInfo = GameInfo.Units[unit:GetUpgradeUnitType()]
		log:Debug("UpdatePromotions upgrade %s to %s", GameInfo.Units[unit:GetUnitType()].Type, tostring(unitInfo.Type))
	end
	
	log:Info("Debug %s", unitInfo.Type)
		
	local upgradingUnitPromotionSet = GameInfo.UnitCombatInfos[unitInfo.CombatClass].PromotionCategory

	if isUpgrading then
		log:Debug("New promotion column: %s", upgradingUnitPromotionSet)
	end

	if GameInfo.UnitPromotions.PROMOTION_REPAIR and (unitInfo.CombatClass == "_UNITCOMBAT_ARMOR" or unitInfo.CombatClass == "UNITCOMBAT_ARMOR") then
		CheckReplacePromotion(
			unit,
			GameInfo.UnitPromotions.PROMOTION_MARCH.ID,
			GameInfo.UnitPromotions.PROMOTION_REPAIR.ID
			)
	end

	for swapRow in GameInfo.UnitPromotions_Equivilancy() do
		local newPromo = swapRow[upgradingUnitPromotionSet]
		local newPromoID = -1
		if newPromo then
			if GameInfo.UnitPromotions[newPromo] then
				newPromoID = GameInfo.UnitPromotions[newPromo].ID
			else
				log:Warn("UpdatePromotions: %s does not exist in UnitPromotions!", newPromo)					
			end
		end
		for column, oldPromoType in pairs(swapRow) do
			if (oldPromoType == "PROMOTION_SIEGE" and (unitInfo.CombatClass == "_UNITCOMBAT_SIEGE" or unitInfo.CombatClass == "UNITCOMBAT_SIEGE")
				or (oldPromoType == "PROMOTION_SIEGE" and unitInfo.Range > 0)
				) then
				-- do not replace
			elseif column ~= upgradingUnitPromotionSet then
				if GameInfo.UnitPromotions[oldPromoType] then
					CheckReplacePromotion(unit, GameInfo.UnitPromotions[oldPromoType].ID, newPromoID)
				else
					log:Warn("UpdatePromotions: %s does not exist in UnitPromotions!", oldPromoType)
				end
			end
		end
	end
end

LuaEvents.ActivePlayerTurnStart_Unit.Add(UpdatePromotions)
LuaEvents.UnitUpgraded.Add(function(unit) UpdatePromotions(unit, true) end)

function CheckReplacePromotion(unit, oldPromoID, newPromoID)
	if unit:IsHasPromotion(oldPromoID) and (oldPromoID ~= newPromoID) then
		--log:Error("%s replace %s with %s", unit:GetName(), GameInfo.UnitPromotions[oldPromoID].Type, newPromoID and GameInfo.UnitPromotions[newPromoID].Type or "none")
		if newPromoID == -1 then
			
		else
			unit:SetHasPromotion(oldPromoID, false)
			unit:SetHasPromotion(newPromoID, true)
			--LuaEvents.RefreshUnitFlagPromotions(unit)
		end
	end
end

--]]
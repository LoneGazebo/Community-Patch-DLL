-- CBP_IncaFunctions
-- Author: Deo-e
-- DateCreated: 12/12/2015 3:29:45 PM
--==========================================================================================================================
-- UTILITY FUNCTIONS
--==========================================================================================================================
-- MOD CHECKS
--------------------------------------------------------------------------------------------------------------------------
-- JFD_IsCivilisationActive
function JFD_IsCivilisationActive(civilizationID)
	for iSlot = 0, GameDefines.MAX_MAJOR_CIVS-1, 1 do
		local slotStatus = PreGame.GetSlotStatus(iSlot)
		if (slotStatus == SlotStatus["SS_TAKEN"] or slotStatus == SlotStatus["SS_COMPUTER"]) then
			if PreGame.GetCivilization(iSlot) == civilizationID then
				return true
			end
		end
	end
	return false
end
--------------------------------------------------------------------------------------------------------------------------
-- USER SETTINGS
--------------------------------------------------------------------------------------------------------------------------
-- CBP_GetUserSetting
function CBP_GetUserSetting(type)
	for row in GameInfo.COMMUNITY("Type = '" .. type .. "'") do
		return row.Value
	end
end
--==========================================================================================================================
-- GLOBALS
--==========================================================================================================================
-- GLOBALS
----------------------------------------------------------------------------------------------------------------------------
local activePlayerID	= Game.GetActivePlayer()
local activePlayer		= Players[activePlayerID]
local civilizationID 	= GameInfoTypes["CIVILIZATION_INCA"]
local isIncaCivActive	= JFD_IsCivilisationActive(civilizationID)
local mathFloor			= math.floor
if isIncaCivActive then
	print("Sapa Inca Pachacuti is in this game")
end
local userSettingInca = CBP_GetUserSetting("COMMUNITY_CORE_BALANCE_CIVS") == 1
--==========================================================================================================================
-- UNIQUE FUNCTIONS
--==========================================================================================================================
-- GREAT ANDEAN ROAD
----------------------------------------------------------------------------------------------------------------------------
-- JFD_CBP_Inca_MachuPicchu
local improvementMachuID = GameInfoTypes["IMPROVEMENT_JFD_MACHU_PICCHU"]
function JFD_CBP_Inca_MachuPicchu(playerID, plotX, plotY)
	local player = Players[playerID]
	if (player:IsAlive() and player:GetCivilizationType() == civilizationID) then
		local cityPlot = Map.GetPlot(plotX,plotY)
		if cityPlot:IsMountain() then
			cityPlot:SetImprovementType(improvementMachuID)
		end
	end
end
if (userSettingInca and isIncaCivActive) then
	GameEvents.PlayerCityFounded.Add(JFD_CBP_Inca_MachuPicchu)
end
--==========================================================================================================================
--==========================================================================================================================
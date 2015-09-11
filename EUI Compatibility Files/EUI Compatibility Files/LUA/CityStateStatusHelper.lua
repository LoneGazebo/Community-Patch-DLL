print("This is the modded CityStateStatusHelper from EUI - CBP- CSD")
-- modified by bc1 from 1.0.3.144 brave new world & civ BE code
-- code is common using gk_mode, bnw_mode, and civ5_mode switches

local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil
local civ5_mode = type( MouseOverStrategicViewResource ) == "function"
local L = Locale.ConvertTextKey
local newLine = civ5_mode and "[NEWLINE]" or "/n"

------------------------------------------------------
-- CityStateStatusHelper.lua
-- Author: Anton Strenger
--
-- Consolidation of code associated with displaying
-- the friendship status of a player with a city-state
------------------------------------------------------

include( "IconSupport" ); local IconHookup = IconHookup
--[[
local GetCityStateStatusRow = GetCityStateStatusRow
local GetCityStateStatusType = GetCityStateStatusType
local UpdateCityStateStatusBar = UpdateCityStateStatusBar
local UpdateCityStateStatusIconBG = UpdateCityStateStatusIconBG
local UpdateCityStateStatusUI = UpdateCityStateStatusUI
local GetCityStateStatusText = GetCityStateStatusText
local GetCityStateResourceExports = GetCityStateResourceExports
local GetCityStateBonuses = GetCityStateBonuses
local GetCityStateStatusToolTip = GetCityStateStatusToolTip
local GetAllyText = GetAllyText
local GetAllyToolTip = GetAllyToolTip
local GetActiveQuestText = GetActiveQuestText
local GetGreatPersonQuestIconText = GetGreatPersonQuestIconText
local GetActiveQuestToolTip = GetActiveQuestToolTip
local GetCityStateTraitText = GetCityStateTraitText
local GetCityStateTraitToolTip = GetCityStateTraitToolTip
local GetCityStateStatusAlly = GetCityStateStatusAlly
--]]

------------------------------------------------------
-- Global Constants
------------------------------------------------------
local kPosInfRange = math.abs( GameDefines.FRIENDSHIP_THRESHOLD_ALLIES - GameDefines.FRIENDSHIP_THRESHOLD_NEUTRAL )
local kNegInfRange = math.abs( GameDefines.MINOR_FRIENDSHIP_AT_WAR - GameDefines.FRIENDSHIP_THRESHOLD_NEUTRAL )
local kPosBarRange = 81
local kNegBarRange = 81
local kBarIconAtlas = "CITY_STATE_INFLUENCE_METER_ICON_ATLAS"
local kBarIconNeutralIndex = 4

local kMinorWar, kMinorAllies, kMinorFriends, kMinorAfraid, kMinorAngry, kMinorNeutral, ktQuestsDisplayOrder
local ktQuestsIcon = {
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_ROUTE or -1 ] = function() return "[ICON_CONNECTED]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP or -1 ] = function() return "[ICON_WAR]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONNECT_RESOURCE or -1 ] = function(i) local row = GameInfo.Resources[i] return row and row.IconString or "" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONSTRUCT_WONDER ]	= function() return "[ICON_GOLDEN_AGE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_GREAT_PERSON or -1 ] = function(i) return GetGreatPersonQuestIconText(i) end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CITY_STATE or -1 ] = function() return "[ICON_RAZING]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_PLAYER or -1 ] = function(i) return Players[i]:IsMinorCiv() and "[ICON_CITY_STATE]" or "[ICON_CAPITAL]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_NATURAL_WONDER or -1 ] = function() return "[ICON_HAPPINESS_1]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_GIVE_GOLD or -1 ] = function() return "[ICON_GOLD]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_PLEDGE_TO_PROTECT or -1 ] = function() return "[ICON_STRENGTH]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_CULTURE or -1 ] = function() return "[ICON_CULTURE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_FAITH or -1 ] = function() return "[ICON_PEACE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_TECHS or -1 ] = function() return "[ICON_RESEARCH]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_INVEST or -1 ] = function() return "[ICON_INVEST]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_BULLY_CITY_STATE or -1 ] = function() return "[ICON_PIRATE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_DENOUNCE_MAJOR or -1 ] = function() return "[ICON_DENOUNCE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_SPREAD_RELIGION or -1 ] = function(i) local row = GameInfo.Religions[i] return row and row.IconString or "" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_TRADE_ROUTE or -1 ] = function() return "[ICON_INTERNATIONAL_TRADE]" end,
-- CBP
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_WAR or -1 ] = function() return "[ICON_SILVER_FIST]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER or -1 ] = function() return "[ICON_TRADE_WHITE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_CITY_STATE or -1 ] = function() return "[ICON_CITY_STATE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_INFLUENCE or -1 ] = function() return "[ICON_INFLUENCE]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_TOURISM or -1 ] = function() return "[ICON_TOURISM]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY or -1 ] = function() return "[ICON_RES_ARTIFACTS]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_CIRCUMNAVIGATION or -1 ] = function() return "[ICON_TURNS_REMAINING]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_LIBERATION or -1 ] = function() return "[ICON_OCCUPIED]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_HORDE or -1 ] = function() return "[ICON_HAPPINESS_3]" end,
	[ MinorCivQuestTypes.MINOR_CIV_QUEST_REBELLION or -1 ] = function() return "[ICON_HAPPINESS_4]" end,
-- END
} ktQuestsIcon[-1] = nil

if gk_mode then
	-- The order of precedence in which the quest icons and tooltip points are displayed
	ktQuestsDisplayOrder = {
		-- Global quests are first
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_CULTURE,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_FAITH,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_TECHS,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_INVEST,		-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_TOURISM, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_INFLUENCE, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP,		-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_HORDE, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_REBELLION, -- CSD
		-- Then personal support quests
		MinorCivQuestTypes.MINOR_CIV_QUEST_GIVE_GOLD,		-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_PLEDGE_TO_PROTECT,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_DENOUNCE_MAJOR,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_WAR, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_LIBERATION, -- CSD
		-- Then other pesonal quests
		MinorCivQuestTypes.MINOR_CIV_QUEST_TRADE_ROUTE,		-- bnw+
		MinorCivQuestTypes.MINOR_CIV_QUEST_SPREAD_RELIGION,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_BULLY_CITY_STATE,	-- g&k+
		MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_NATURAL_WONDER,	-- vanilla+ but NOT civBE
		MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_PLAYER,		-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CITY_STATE,	-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_CITY_STATE, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_GREAT_PERSON,	-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONSTRUCT_WONDER,	-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONNECT_RESOURCE,	-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_ROUTE,		-- vanilla+
		MinorCivQuestTypes.MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY, -- CSD
		MinorCivQuestTypes.MINOR_CIV_QUEST_CIRCUMNAVIGATION, -- CSD
	}
	kMinorWar = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_STATUS_WAR
	kMinorAllies = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_STATUS_ALLIES
	kMinorFriends = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_STATUS_FRIENDS
	kMinorAfraid = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_STATUS_AFRAID
	kMinorAngry = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_STATUS_ANGRY
	kMinorNeutral = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_STATUS_NEUTRAL
else
	kMinorWar = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_AT_WAR
	kMinorAllies = GameInfo.MinorCivTraits_Status.FRIENDSHIP_THRESHOLD_ALLIES
	kMinorFriends = GameInfo.MinorCivTraits_Status.FRIENDSHIP_THRESHOLD_FRIENDS
	kMinorAfraid = GameInfo.MinorCivTraits_Status.FRIENDSHIP_THRESHOLD_NEUTRAL
	kMinorAngry = GameInfo.MinorCivTraits_Status.MINOR_FRIENDSHIP_AT_WAR
	kMinorNeutral = GameInfo.MinorCivTraits_Status.FRIENDSHIP_THRESHOLD_NEUTRAL
end

------------------------------------------------------

function GetCityStateStatusRow( majorPlayerID, minorPlayerID )
	local majorPlayer = Players[majorPlayerID]
	local minorPlayer = Players[minorPlayerID]

	if majorPlayer and minorPlayer then

		-- War
		if Teams[majorPlayer:GetTeam()]:IsAtWar(minorPlayer:GetTeam()) then
			return kMinorWar

		-- Allies
		elseif minorPlayer:IsAllies(majorPlayerID) then
			return kMinorAllies

		-- Friends
		elseif minorPlayer:IsFriends(majorPlayerID) then
			return kMinorFriends

		-- Able to bully?
		elseif gk_mode and minorPlayer:CanMajorBullyGold(majorPlayerID) then
			return kMinorAfraid

		-- Angry
		elseif minorPlayer:GetMinorCivFriendshipWithMajor(majorPlayerID) < GameDefines.FRIENDSHIP_THRESHOLD_NEUTRAL then
			return kMinorAngry

		-- Neutral
		else
			return kMinorNeutral
		end

	else
		print("Lua error - invalid player index")
	end
end

function GetCityStateStatusType(majorPlayerID, minorPlayerID)
	local row = GetCityStateStatusRow(majorPlayerID, minorPlayerID)
	return row and row.Type
end

function UpdateCityStateStatusBar(majorPlayerID, minorPlayerID, posBarCtrl, negBarCtrl, barMarkerCtrl)
	local majorPlayer = Players[majorPlayerID]
	local minorPlayer = Players[minorPlayerID]

	if majorPlayer and minorPlayer then

		local info = GetCityStateStatusRow(majorPlayerID, minorPlayerID)
		local iInf = minorPlayer:GetMinorCivFriendshipWithMajor(majorPlayerID)

		if iInf >= 0 then
			local percentFull = math.abs(iInf) / kPosInfRange
			local xOffset = math.min(percentFull * kPosBarRange, kPosBarRange)
			barMarkerCtrl:SetOffsetX(xOffset)
			if gk_mode and info and info.PositiveStatusMeter then
				posBarCtrl:SetTexture(info.PositiveStatusMeter)
			end
			posBarCtrl:SetPercent(percentFull)
			posBarCtrl:SetHide(false)
			negBarCtrl:SetHide(true)
		else
			local percentFull = math.abs(iInf) / kNegInfRange
			local xOffset = - math.min(percentFull * kNegBarRange, kNegBarRange)
			barMarkerCtrl:SetOffsetX(xOffset)
			if gk_mode and info and info.NegativeStatusMeter then
				negBarCtrl:SetTexture(info.NegativeStatusMeter)
			end
			negBarCtrl:SetPercent(percentFull)
			negBarCtrl:SetHide(false)
			posBarCtrl:SetHide(true)
		end

		if gk_mode then
			-- Bubble icon for meter
			local size = barMarkerCtrl:GetSize().x
			-- Special case when INF = 0
			if (iInf == 0) then
				IconHookup(kBarIconNeutralIndex, size, kBarIconAtlas, barMarkerCtrl)
			elseif info and info.StatusMeterIconAtlasIndex then
				IconHookup(info.StatusMeterIconAtlasIndex, size, kBarIconAtlas, barMarkerCtrl)
			end
		end
	else
		print("Lua error - invalid player index")
	end
end

function UpdateCityStateStatusIconBG(majorPlayerID, minorPlayerID, iconBGCtrl)
	local info = GetCityStateStatusRow(majorPlayerID, minorPlayerID)

	if info and info.StatusIcon then
		iconBGCtrl:SetTexture( info.StatusIcon )
	end
end

function UpdateCityStateStatusUI(majorPlayerID, minorPlayerID, posBarCtrl, negBarCtrl, barMarkerCtrl, iconBGCtrl)
	UpdateCityStateStatusBar(majorPlayerID, minorPlayerID, posBarCtrl, negBarCtrl, barMarkerCtrl)
	UpdateCityStateStatusIconBG(majorPlayerID, minorPlayerID, iconBGCtrl)
end

function GetCityStateStatusText( majorPlayerID, minorPlayerID )
	local majorPlayer = Players[majorPlayerID]
	local minorPlayer = Players[minorPlayerID]
	local strStatusText = ""

	if majorPlayer and minorPlayer then

		local majorTeamID = majorPlayer:GetTeam()
		local minorTeamID = minorPlayer:GetTeam()
		local majorTeam = Teams[majorTeamID]
		local minorTeam = Teams[minorTeamID]

		local isAtWar = majorTeam:IsAtWar(minorTeamID)
		local majorInfluenceWithMinor = minorPlayer:GetMinorCivFriendshipWithMajor(majorPlayerID)

		-- Status

		if minorPlayer:IsAllies(majorPlayerID) then			-- Allies
			strStatusText = "[COLOR_CYAN]" .. L("TXT_KEY_ALLIES")

		elseif minorPlayer:IsFriends(majorPlayerID) then		-- Friends
			strStatusText = "[COLOR_POSITIVE_TEXT]" .. L("TXT_KEY_FRIENDS")

		elseif minorPlayer:IsMinorPermanentWar(majorTeamID) then	-- Permanent War
			strStatusText = "[COLOR_NEGATIVE_TEXT]" .. L("TXT_KEY_PERMANENT_WAR")

		elseif minorPlayer:IsPeaceBlocked(majorTeamID) then		-- Peace blocked by being at war with ally
			strStatusText = "[COLOR_NEGATIVE_TEXT]" .. L("TXT_KEY_PEACE_BLOCKED")

		elseif (isAtWar) then		-- War
			strStatusText = "[COLOR_NEGATIVE_TEXT]" .. L("TXT_KEY_WAR")

		elseif majorInfluenceWithMinor < GameDefines.FRIENDSHIP_THRESHOLD_NEUTRAL then
			-- Afraid
			if gk_mode and minorPlayer:CanMajorBullyGold(majorPlayerID) then
				strStatusText = "[COLOR_PLAYER_LIGHT_ORANGE_TEXT]"..L("TXT_KEY_AFRAID").."[ENDCOLOR]"
			-- Angry
			else
				strStatusText = "[COLOR_MAGENTA]"..L("TXT_KEY_ANGRY")
			end

		else		-- Neutral
			strStatusText = "[COLOR_WHITE]" .. L("TXT_KEY_CITY_STATE_PERSONALITY_NEUTRAL")
		end
		strStatusText = strStatusText .. "[ENDCOLOR]"
		if not isAtWar then
			strStatusText = strStatusText .. " " .. majorInfluenceWithMinor .. "[ICON_INFLUENCE]"
			if not gk_mode and majorInfluenceWithMinor ~= 0 then
				strStatusText = strStatusText .. (" (%+g[ICON_INFLUENCE] / "):format(minorPlayer:GetFriendshipChangePerTurnTimes100(majorPlayerID) / 100).. L"TXT_KEY_DO_TURN" .. ")" --"TXT_KEY_CITY_STATE_TITLE_TOOL_TIP_CURRENT"
			end
		end
	else
		print("Lua error - invalid player index")
	end

	return strStatusText
end

function GetCityStateResourceExports( minorPlayer )
	-- Resources
	local resourceExports = {}

	if minorPlayer and minorPlayer.GetResourceExport then
		for resource in GameInfo.Resources("ResourceClassType<>'RESOURCECLASS_BONUS'") do

			local resourceExport = minorPlayer:GetResourceExport(resource.ID)

			if resourceExport > 0 then

				table.insert( resourceExports, "[COLOR_POSITIVE_TEXT]" .. resourceExport .. resource.IconString .. "[ENDCOLOR] " .. L(resource.Description) )
			end
		end
	end
	return table.concat( resourceExports, ", ")
end

function GetCityStateBonuses( majorPlayerID, minorPlayerID )
	local tips = {}
	local minorPlayer = Players[minorPlayerID]

	if minorPlayer then

		local capitalFoodBonus = minorPlayer:GetCurrentCapitalFoodBonus(majorPlayerID) / 100
		local otherCityFoodBonus = minorPlayer:GetCurrentOtherCityFoodBonus(majorPlayerID) / 100
		if capitalFoodBonus ~= 0 or otherCityFoodBonus ~= 0 then
			table.insert( tips, L("TXT_KEY_CSTATE_FOOD_BONUS", capitalFoodBonus, otherCityFoodBonus) )
		end

		local unitSpawnEstimate = minorPlayer:GetCurrentSpawnEstimate(majorPlayerID)
		if unitSpawnEstimate ~= 0 then
			table.insert( tips, L("TXT_KEY_CSTATE_MILITARY_BONUS", unitSpawnEstimate) )
		end

		local scienceBonusTimes100 = minorPlayer:GetCurrentScienceFriendshipBonusTimes100(majorPlayerID)
		if scienceBonusTimes100 ~= 0 then
			table.insert( tips, L("TXT_KEY_CSTATE_SCIENCE_BONUS", scienceBonusTimes100 / 100) )
		end

		if gk_mode then
			local cultureBonus = minorPlayer:GetMinorCivCurrentCultureBonus(majorPlayerID)
			if cultureBonus ~= 0 then
				table.insert( tips, L("TXT_KEY_CSTATE_CULTURE_BONUS", cultureBonus) )
			end

			local happinessBonus = civ5_mode and minorPlayer:GetMinorCivCurrentHappinessBonus(majorPlayerID) or minorPlayer:GetMinorCivCurrentHealthBonus(majorPlayerID)
			if happinessBonus ~= 0 then
				table.insert( tips, L(civ5_mode and "TXT_KEY_CSTATE_HAPPINESS_BONUS" or "TXT_KEY_CSTATE_HEALTH_BONUS", happinessBonus) )
			end

			local faithBonus = minorPlayer:GetMinorCivCurrentFaithBonus(majorPlayerID)
			if faithBonus ~= 0 then
				table.insert( tips,  L("TXT_KEY_CSTATE_FAITH_BONUS", faithBonus) )
			end
		else
			local cultureBonus = minorPlayer:GetCurrentCultureBonus(majorPlayerID);
			if cultureBonus ~= 0 then
				table.insert( tips, L("TXT_KEY_CSTATE_CULTURE_BONUS", cultureBonus) )
			end
		end

		-- Resources
		local resourceExports = GetCityStateResourceExports( minorPlayer )

		if minorPlayer:IsAllies(majorPlayerID) and #resourceExports > 0 then

			table.insert( tips, L( "TXT_KEY_CSTATE_RESOURCES_RECEIVED", resourceExports ) )
		end
	end
	return tips
end

function GetCityStateStatusToolTip( majorPlayerID, minorPlayerID, isFullInfo )
	local majorPlayer = Players[majorPlayerID]
	local minorPlayer = Players[minorPlayerID]
	local tips = {}

	if majorPlayer and minorPlayer then

		local majorTeamID = majorPlayer:GetTeam()
		local minorTeamID = minorPlayer:GetTeam()
		local majorTeam = Teams[majorTeamID]
		local minorTeam = Teams[minorTeamID]

		local isAtWar = majorTeam:IsAtWar(minorTeamID)

		local strShortDesc = minorPlayer:GetCivilizationShortDescription()
		local influenceAccumulated = minorPlayer:GetMinorCivFriendshipWithMajor(majorPlayerID)

		-- Name
		local tip = strShortDesc
		-- Resources
		for resource in GameInfo.Resources( "ResourceClassType<>'RESOURCECLASS_BONUS'" ) do

			local resourceForExport = minorPlayer:GetResourceExport(resource.ID)
						+ minorPlayer:GetNumResourceAvailable(resource.ID, false)

			if resourceForExport > 0 then
				tip = tip .. " " .. resourceForExport .. resource.IconString
			end
		end
		-- Status
		tip = tip .. " " .. GetCityStateStatusText( majorPlayerID, minorPlayerID )
-- CBP
		local iJerk = minorPlayer:GetJerk(majorTeamID);
		if(iJerk > 0) then
			tip = tip .. "[NEWLINE][NEWLINE]" .. L("TXT_KEY_CSTATE_JERK_STATUS", iJerk)
		end
-- END
		table.insert( tips, tip )

		-- Influence change
		if gk_mode then
			local influenceAnchor = minorPlayer:GetMinorCivFriendshipAnchorWithMajor(majorPlayerID)
			if influenceAccumulated ~= influenceAnchor then
				table.insert( tips, L( "TXT_KEY_CSTATE_INFLUENCE_RATE", minorPlayer:GetFriendshipChangePerTurnTimes100(majorPlayerID) / 100, influenceAnchor ) )
			end
			-- Bullying
			if minorPlayer:CanMajorBullyGold(majorPlayerID) then
				table.insert( tips, L"TXT_KEY_CSTATE_CAN_BULLY" )
			else
				table.insert( tips, L"TXT_KEY_CSTATE_CANNOT_BULLY" )
			end
		end

		if isFullInfo then
			-- Bonuses
			local bonuses = GetCityStateBonuses( majorPlayerID, minorPlayerID )
			if #bonuses > 0 then
				table.insert( tips, table.concat( bonuses, newLine ) )
			end

			-- Protected by anyone?
			local protectors={}
			for playerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do
				local player = Players[playerID]
				if player
					and player:IsAlive()
					and majorTeam:IsHasMet(player:GetTeam())
					and player:IsProtectingMinor(minorPlayerID)
				then
					table.insert( protectors, player:GetCivilizationShortDescription() )
				end
			end
			if #protectors > 0 then
				table.insert( tips, L"TXT_KEY_POP_CSTATE_PLEDGE_TO_PROTECT" .. ": " .. table.concat(protectors, ", ") )
			end

			-- At war with anyone ?
			local wars={}
			for playerID = 0, GameDefines.MAX_CIV_PLAYERS - 1 do
				local player = Players[playerID]
				if player
					and playerID ~= minorPlayerID
					and player:IsAlive()
					and majorTeam:IsHasMet(player:GetTeam())
					and minorTeam:IsAtWar(player:GetTeam())
				then
					table.insert( wars, player:GetCivilizationShortDescription() )
				end
			end
			if #wars > 0 then
				table.insert( tips, L("TXT_KEY_AT_WAR_WITH", table.concat(wars, ", ") ) )
			end

		end
	else
		print("Lua error - invalid player index")
	end

	return table.concat( tips, newLine )
end
function GetAllyText( majorPlayerID, minorPlayerID )

	local majorPlayer = Players[ majorPlayerID ]
	local minorPlayer = Players[ minorPlayerID ]

	if majorPlayer and minorPlayer then
		local allyID = minorPlayer:GetAlly()
		local ally = Players[ allyID ]
		-- Has an ally
		if ally then
			-- Not us
			if allyID ~= majorPlayerID then
				-- Someone we know
				if Teams[majorPlayer:GetTeam()]:IsHasMet( ally:GetTeam() ) then
					return L( ally:GetCivilizationShortDescriptionKey() )
				-- Someone we haven't met
				else
					return L"TXT_KEY_UNMET_PLAYER"
				end
			-- Us
			else
				return "[COLOR_POSITIVE_TEXT]" .. L"TXT_KEY_YOU" .. "[ENDCOLOR]"
			end
		-- No ally
		else
			return L"TXT_KEY_CITY_STATE_NOBODY"
		end
	end

	return ""
end

function GetAllyToolTip( majorPlayerID, minorPlayerID )
	local toolTip = ""

	local majorPlayer = Players[majorPlayerID]
	local minorPlayer = Players[minorPlayerID]
	if majorPlayer and minorPlayer then
		local minorCivFriendshipWithMajor = minorPlayer:GetMinorCivFriendshipWithMajor(majorPlayerID)
		local allyID = minorPlayer:GetAlly()
		local ally = Players[allyID]
		-- Has an ally
		if ally then
			local minorCivFriendshipWithAlly = minorPlayer:GetMinorCivFriendshipWithMajor(allyID)
			-- Not us
			if allyID ~= majorPlayerID then
				-- Someone we know
				local influenceUntilAllied = minorCivFriendshipWithAlly - minorCivFriendshipWithMajor + 1	-- needs to pass up the current ally, not just match
				if not gk_mode then
					toolTip = L("TXT_KEY_CITY_STATE_TITLE_TOOL_TIP_ALLIES", influenceUntilAllied, minorPlayer:GetCivilizationShortDescription() )
				elseif Teams[majorPlayer:GetTeam()]:IsHasMet(ally:GetTeam()) then
					toolTip = L("TXT_KEY_CITY_STATE_ALLY_TT", Players[allyID]:GetCivilizationShortDescription(), influenceUntilAllied)
				-- Someone we haven't met
				else
					toolTip = L("TXT_KEY_CITY_STATE_ALLY_UNKNOWN_TT", influenceUntilAllied)
				end
				-- Remove Firaxis' tedious drone blurb
				return toolTip:sub(1,(toolTip:find("[NEWLINE]",1,true) or 999)-1)

			-- Us
			else
				toolTip = L("TXT_KEY_CITY_STATE_RELATIONSHIP_ALLIES")
				-- Bonuses
				local bonuses = GetCityStateBonuses( majorPlayerID, minorPlayerID )
				if #bonuses > 0 then
					toolTip = toolTip .. " " .. table.concat( bonuses, " " )
				end
			end
		-- No ally
		else
			local influenceUntilAllied = GameDefines.FRIENDSHIP_THRESHOLD_ALLIES - minorCivFriendshipWithMajor
			if gk_mode then
				toolTip = L( "TXT_KEY_CITY_STATE_ALLY_NOBODY_TT", influenceUntilAllied )
			else
				toolTip = L("TXT_KEY_CITY_STATE_TITLE_TOOL_TIP_ALLIES", influenceUntilAllied, minorPlayer:GetCivilizationShortDescription() )
			end
		end
	end

	return toolTip
end

local isMinorCivQuestForPlayer
if gk_mode then
	if bnw_mode then
		isMinorCivQuestForPlayer = Players[1].IsMinorCivDisplayedQuestForPlayer
	else
		isMinorCivQuestForPlayer = Players[1].IsMinorCivActiveQuestForPlayer
	end
	function GetActiveQuestText(majorPlayerID, minorPlayerID)

		local minorPlayer = Players[minorPlayerID]
		local sIconText = ""

		if minorPlayer then
			for i, questID in pairs(ktQuestsDisplayOrder) do

				if isMinorCivQuestForPlayer( minorPlayer, majorPlayerID, questID ) then
					sIconText = sIconText .. ktQuestsIcon[questID]( minorPlayer:GetQuestData1(majorPlayerID, questID) )
				end
			end

			-- Threatening Barbarians event
			if minorPlayer:IsThreateningBarbariansEventActiveForPlayer(majorPlayerID) then
				sIconText = sIconText .. "[ICON_RAZING]"
			end

			-- Proxy War event
			if bnw_mode and minorPlayer:IsProxyWarActiveForMajor(majorPlayerID) then
				sIconText = sIconText .. "[ICON_RESISTANCE]"
			end

		else
			print("Lua error - invalid player index")
		end

		return sIconText
	end
else
	function GetActiveQuestText(majorPlayerID, minorPlayerID)

		local minorPlayer = Players[minorPlayerID]

		if minorPlayer then
			local questID = minorPlayer:GetActiveQuestForPlayer(majorPlayerID)
			local questIcon = questID and ktQuestsIcon[questID]
			if questIcon then
				return questIcon( minorPlayer:GetQuestData1(majorPlayerID) )
			end
		else
			print("Lua error - invalid player index")
		end
		return ""
	end
end

local kUnitIcons = {
	UNIT_ENGINEER = "[ICON_GREAT_ENGINEER]",
	UNIT_GREAT_GENERAL = "[ICON_GREAT_GENERAL]",
	UNIT_SCIENTIST = "[ICON_GREAT_SCIENTIST]",
	UNIT_MERCHANT = "[ICON_GREAT_MERCHANT]",
	UNIT_ARTIST = "[ICON_GREAT_ARTIST]",
	UNIT_MUSICIAN = "[ICON_GREAT_MUSICIAN]",
	UNIT_WRITER = "[ICON_GREAT_WRITER]",
	UNIT_GREAT_ADMIRAL = "[ICON_GREAT_ADMIRAL]",
	UNIT_PROPHET = "[ICON_PROPHET]",
-- CBP
	UNIT_GREAT_DIPLOMAT = "[ICON_DIPLOMAT]"}
-- END

for unit in GameInfo.Units() do
	local icon = unit.Type and kUnitIcons[unit.Type]
	if icon then
		kUnitIcons[unit.ID] = icon
	end
end

function GetGreatPersonQuestIconText(unitID)
	return bnw_mode and kUnitIcons[unitID] or "[ICON_GREAT_PEOPLE]"
end

local function QuestString(majorPlayerID, minorPlayer, questID, questData1)
	if questID == MinorCivQuestTypes.MINOR_CIV_QUEST_ROUTE then
		return L( "TXT_KEY_CITY_STATE_QUEST_ROUTE_FORMAL" )
	elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP then
		return L( "TXT_KEY_CITY_STATE_QUEST_KILL_CAMP_FORMAL" )
	elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONNECT_RESOURCE then
		return L( "TXT_KEY_CITY_STATE_QUEST_CONNECT_RESOURCE_FORMAL", GameInfo.Resources[questData1].Description )
	elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONSTRUCT_WONDER then
		return L( "TXT_KEY_CITY_STATE_QUEST_CONSTRUCT_WONDER_FORMAL", GameInfo.Buildings[questData1].Description )
	elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_GREAT_PERSON then
		return L( "TXT_KEY_CITY_STATE_QUEST_GREAT_PERSON_FORMAL", GameInfo.Units[questData1].Description )
	elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CITY_STATE then
		return L( "TXT_KEY_CITY_STATE_QUEST_KILL_CITY_STATE_FORMAL", Players[questData1]:GetNameKey() )
	elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_PLAYER then
		return L( "TXT_KEY_CITY_STATE_QUEST_FIND_PLAYER_FORMAL", Players[questData1]:GetCivilizationShortDescriptionKey() )
	elseif civ5_mode and questID == MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_NATURAL_WONDER then
		return L( "TXT_KEY_CITY_STATE_QUEST_FIND_NATURAL_WONDER_FORMAL" )
	elseif gk_mode then
		if questID == MinorCivQuestTypes.MINOR_CIV_QUEST_GIVE_GOLD then
			return L( "TXT_KEY_CITY_STATE_QUEST_GIVE_GOLD_FORMAL", Players[questData1]:GetCivilizationShortDescriptionKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_PLEDGE_TO_PROTECT then
			return L( "TXT_KEY_CITY_STATE_QUEST_PLEDGE_TO_PROTECT_FORMAL", Players[questData1]:GetCivilizationShortDescriptionKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_CULTURE then
			local iLeaderScore = minorPlayer:GetMinorCivContestValueForLeader(questID)
			local iMajorScore = minorPlayer:GetMinorCivContestValueForPlayer(majorPlayerID, questID)
			if minorPlayer:IsMinorCivContestLeader(majorPlayerID, questID) then
				return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_CULTURE_WINNING_FORMAL", iMajorScore )
			else
				return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_CULTURE_LOSING_FORMAL", iLeaderScore, iMajorScore )
			end
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_FAITH then
			local iLeaderScore = minorPlayer:GetMinorCivContestValueForLeader(questID)
			local iMajorScore = minorPlayer:GetMinorCivContestValueForPlayer(majorPlayerID, questID)
			if minorPlayer:IsMinorCivContestLeader(majorPlayerID, questID) then
				return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_WINNING_FORMAL", iMajorScore )
			else
				return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_LOSING_FORMAL", iLeaderScore, iMajorScore )
			end
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_TECHS then
			local iLeaderScore = minorPlayer:GetMinorCivContestValueForLeader(questID)
			local iMajorScore = minorPlayer:GetMinorCivContestValueForPlayer(majorPlayerID, questID)
			if minorPlayer:IsMinorCivContestLeader(majorPlayerID, questID) then
				return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_TECHS_WINNING_FORMAL", iMajorScore )
			else
				return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_TECHS_LOSING_FORMAL", iLeaderScore, iMajorScore )
			end
-- CBP
		elseif (questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONTEST_TOURISM) then
				local iLeaderScore = minorPlayer:GetMinorCivContestValueForLeader(questID);
				local iMajorScore = minorPlayer:GetMinorCivContestValueForPlayer(majorPlayerID, questID);
				if (minorPlayer:IsMinorCivContestLeader(majorPlayerID, questID)) then
					return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_TOURISM_WINNING_FORMAL", iMajorScore );
				else
					return L( "TXT_KEY_CITY_STATE_QUEST_CONTEST_TOURISM_LOSING_FORMAL", iLeaderScore, iMajorScore );
				end
-- END
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_INVEST then
			return L( "TXT_KEY_CITY_STATE_QUEST_INVEST_FORMAL" )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_BULLY_CITY_STATE then
			return L( "TXT_KEY_CITY_STATE_QUEST_BULLY_CITY_STATE_FORMAL", Players[questData1]:GetCivilizationShortDescriptionKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_DENOUNCE_MAJOR then
			return L( "TXT_KEY_CITY_STATE_QUEST_DENOUNCE_MAJOR_FORMAL", Players[questData1]:GetCivilizationShortDescriptionKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_SPREAD_RELIGION then
			return L( "TXT_KEY_CITY_STATE_QUEST_SPREAD_RELIGION_FORMAL", Game.GetReligionName(questData1) )
-- CBP
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_WAR then
			return L( "TXT_KEY_CITY_STATE_QUEST_WAR_FORMAL", Players[questData1]:GetCivilizationShortDescriptionKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER then
			return L( "TXT_KEY_CITY_STATE_QUEST_CONSTRUCT_NATIONAL_WONDER_FORMAL", GameInfo.Buildings[questData1].Description )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_FIND_CITY_STATE then
			return L( "TXT_KEY_CITY_STATE_QUEST_FIND_CITY_STATE_FORMAL", Players[questData1]:GetNameKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_INFLUENCE then
			return L( "TXT_KEY_CITY_STATE_QUEST_INFLUENCE_FORMAL" )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY then
			return L( "TXT_KEY_CITY_STATE_QUEST_ARCHAEOLOGY_FORMAL" )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_CIRCUMNAVIGATION then
			return L( "TXT_KEY_CITY_STATE_QUEST_CIRCUMNAVIGATION_FORMAL" )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_LIBERATION then
			return L( "TXT_KEY_CITY_STATE_QUEST_LIBERATION_FORMAL", Players[questData1]:GetNameKey() )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_HORDE then
			return L( "TXT_KEY_CITY_STATE_QUEST_HORDE_FORMAL" )
		elseif questID == MinorCivQuestTypes.MINOR_CIV_QUEST_REBELLION then
			return L( "TXT_KEY_CITY_STATE_QUEST_REBELLION_FORMAL" )
-- END
		elseif bnw_mode then
			if questID == MinorCivQuestTypes.MINOR_CIV_QUEST_TRADE_ROUTE then
				return L("TXT_KEY_CITY_STATE_QUEST_TRADE_ROUTE_FORMAL")
			end
		end
	end
	return "CityStateStatusHelper script error - unknown quest"
end

if gk_mode then
	function GetActiveQuestToolTip(majorPlayerID, minorPlayerID)
		local minorPlayer = Players[minorPlayerID]
		local tips = {}
		if minorPlayer then
			for i, questID in pairs(ktQuestsDisplayOrder) do

				if isMinorCivQuestForPlayer( minorPlayer, majorPlayerID, questID ) then
					local questData1 = minorPlayer:GetQuestData1(majorPlayerID, questID)
					local questString = QuestString(majorPlayerID, minorPlayer, questID, questData1)
					local turnsRemaining = minorPlayer:GetQuestTurnsRemaining(majorPlayerID, questID, Game.GetGameTurn() - 1)	-- add 1 since began on CS's turn (1 before), and avoids "0 turns remaining"
					if turnsRemaining >= 0 then
						questString = questString .. " " .. L( "TXT_KEY_CITY_STATE_QUEST_TURNS_REMAINING_FORMAL", turnsRemaining )
					end
					table.insert( tips, ktQuestsIcon[questID]( questData1 ) .. " " .. questString )
				end
			end

			-- Threatening Barbarians event
			if minorPlayer:IsThreateningBarbariansEventActiveForPlayer(majorPlayerID) then
				table.insert( tips,"[ICON_BULLET]" .. L("TXT_KEY_CITY_STATE_QUEST_INVADING_BARBS_FORMAL") )
			end

			-- Proxy War event
			if bnw_mode and minorPlayer:IsProxyWarActiveForMajor(majorPlayerID) then
				table.insert( tips,"[ICON_BULLET]" .. L("TXT_KEY_CITY_STATE_QUEST_GIFT_UNIT_FORMAL") )
			end
		else
			print("Lua error - invalid player index")
		end -- minorPlayer

		if #tips > 0 then
			return table.concat( tips, newLine )
		else
			return L("TXT_KEY_CITY_STATE_QUEST_NONE_FORMAL")
		end
	end
else
	function GetActiveQuestToolTip(majorPlayerID, minorPlayerID)
		local minorPlayer = Players[minorPlayerID]
		if minorPlayer then
			local questID = minorPlayer:GetActiveQuestForPlayer(majorPlayerID)
			if questID and questID >= 0 then
				return QuestString( majorPlayerID, minorPlayer, questID, minorPlayer:GetQuestData1(majorPlayerID) )
			end
		else
			print("Lua error - invalid player index")
		end -- minorPlayer
		return L("TXT_KEY_CITY_STATE_BARB_QUEST_SHORT")..": "..L("TXT_KEY_SV_ICONS_NONE")
	end
end -- gk_mode

function GetCityStateTraitText(minorPlayerID)

	local minorPlayer = Players[minorPlayerID]

	if minorPlayer then
		local minorCivTraitID = minorPlayer:GetMinorCivTrait()
		for key, value in pairs(MinorCivTraitTypes) do
			if minorCivTraitID == value then
				return L("TXT_KEY_CITY_STATE" .. key:sub(16) .. "_ADJECTIVE")
			end
		end
	else
		print("Lua error - invalid player index")
	end

	return ""
end

function GetCityStateTraitToolTip( minorPlayerID )
	local toolTip = ""
	local minorPlayer = Players[minorPlayerID]

	if minorPlayer then
		local minorCivTraitID = minorPlayer:GetMinorCivTrait()

		if gk_mode and minorCivTraitID == MinorCivTraitTypes.MINOR_CIV_TRAIT_MILITARISTIC then
			toolTip = L("TXT_KEY_CITY_STATE_MILITARISTIC_NO_UU_TT")
			if minorPlayer:IsMinorCivHasUniqueUnit() then
				local eUniqueUnit = minorPlayer:GetMinorCivUniqueUnit()
				if GameInfo.Units[eUniqueUnit] then
					local ePrereqTech = GameInfo.Units[eUniqueUnit].PrereqTech
					if not ePrereqTech then
						-- If no prereq then just make it Agriculture, but make sure that Agriculture is in our database. Otherwise, show the fallback tooltip.
						ePrereqTech = GameInfo.Technologies.TECH_AGRICULTURE
						ePrereqTech = ePrereqTech and ePrereqTech.ID
					end

					if ePrereqTech then
						if GameInfo.Technologies[ePrereqTech] then
							toolTip = L("TXT_KEY_CITY_STATE_MILITARISTIC_TT", GameInfo.Units[eUniqueUnit].Description, GameInfo.Technologies[ePrereqTech].Description)
						end
					end
				else
					print("Scripting error - City-State's unique unit not found!")
				end
			end
		else
			for key,value in pairs(MinorCivTraitTypes) do
				if minorCivTraitID == value then
					toolTip = L("TXT_KEY_CITY_STATE" .. key:sub(16) .. "_TT")
					break
				end
			end
		end
	else
		print("Lua error - invalid player index")
	end

	return toolTip
end

function GetCityStateStatusAlly( majorPlayerID, minorPlayerID, isFullInfo )
	local majorPlayer = Players[majorPlayerID]
	local minorPlayer = Players[minorPlayerID]
	local toolTip = ""
	if not gk_mode then
		toolTip = L("TXT_KEY_CITY_STATE_ALLY_TT")
	elseif majorPlayer and minorPlayer then
		local allyID = minorPlayer:GetAlly()
		local minorCivFriendshipWithMajor = minorPlayer:GetMinorCivFriendshipWithMajor(majorPlayerID)
		if allyID and allyID ~= -1 then -- already has an ally

			local minorCivFriendshipAlly = minorPlayer:GetMinorCivFriendshipWithMajor(allyID) - minorCivFriendshipWithMajor + 1	-- needs to pass up the current ally, not just match
			local ally = Players[allyID]

			if Teams[majorPlayer:GetTeam()]:IsHasMet(ally:GetTeam()) then
				toolTip = L("TXT_KEY_CITY_STATE_ALLY_TT", ally:GetCivilizationShortDescriptionKey(), minorCivFriendshipAlly )
			else
				toolTip = L("TXT_KEY_CITY_STATE_ALLY_UNKNOWN_TT", minorCivFriendshipAlly )
			end
		else
			toolTip = L("TXT_KEY_CITY_STATE_ALLY_NOBODY_TT", GameDefines.FRIENDSHIP_THRESHOLD_ALLIES - minorCivFriendshipWithMajor )
		end
		if not isFullInfo then
			return toolTip:sub(1,(toolTip:find("[NEWLINE]",1,true) or 999)-1)
		end
	else
		print("Lua error - invalid player index")
	end
	return toolTip
end

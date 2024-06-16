local math_max = math.max
local string_format = string.format
local tostring = tostring

local DomainTypes_DOMAIN_AIR = DomainTypes.DOMAIN_AIR
local Game = Game
local GameDefines_MOVE_DENOMINATOR = GameDefines.MOVE_DENOMINATOR
local GameInfo = EUI.GameInfoCache
local L = Locale.ConvertTextKey
local Players = Players
local PreGame = PreGame
local Teams = Teams

function EUI.ShortUnitTip( unit )
	local activePlayerID = Game.GetActivePlayer()
--	local activePlayer = Players[activePlayerID]
	local activeTeamID = Game.GetActiveTeam()
	local activeTeam = Teams[activeTeamID]

	local unitOwnerID = unit:GetOwner()
	local unitOwner = Players[unitOwnerID]
	local unitTeamID = unit:GetTeam()

	local civAdjective = unitOwner:GetCivilizationAdjective()
	local nickName = unitOwner:GetNickName()

	local unitTip
	if activeTeamID == unitTeamID or ( unitOwner:IsMinorCiv() and unitOwner:IsAllies( activePlayerID ) ) then
		unitTip = "[COLOR_POSITIVE_TEXT]"
	elseif activeTeam:IsAtWar( unitTeamID ) then
		unitTip = "[COLOR_NEGATIVE_TEXT]"
	else
		unitTip = "[COLOR_WHITE]"
	end
	unitTip = unitTip .. L(unit:GetNameKey()) .. "[ENDCOLOR]"

	-- Player using nickname
	if PreGame.IsMultiplayerGame() and nickName and #nickName > 0 then

		unitTip = L( "TXT_KEY_MULTIPLAYER_UNIT_TT", nickName, civAdjective, unitTip )

	elseif activeTeam:IsHasMet( unitTeamID ) then

		unitTip = L( "TXT_KEY_PLOTROLL_UNIT_DESCRIPTION_CIV", civAdjective, unitTip )
		if unit:HasName() then
			unitTip = L(unit:GetNameNoDesc()) .. " (" .. unitTip .. ")"
		end
	end

	local originalOwnerID = unit:GetOriginalOwner()
	local originalOwner = originalOwnerID and Players[originalOwnerID]
	if originalOwner and originalOwnerID ~= unitOwnerID and activeTeam:IsHasMet( originalOwner:GetTeam() ) then
		unitTip = unitTip .. " (" .. originalOwner:GetCivilizationAdjective() .. ")"
	end

	-- Debug stuff
	if Game.IsDebugMode() then
		unitTip = unitTip .. " (".. tostring(unitOwnerID) ..":" .. tostring(unit:GetID()) .. ")"
	end

	-- Moves & Combat Strength
	local unitMoves = 0
	local unitStrength = unit.GetBaseCombatStrength and unit:GetBaseCombatStrength() or unit:GetCombatStrength() --BE stupid function name change
	-- todo unit:GetMaxDefenseStrength()
	local rangedStrength = unit.GetBaseRangedCombatStrength and unit:GetBaseRangedCombatStrength() or unit:GetRangedCombatStrength() --BE stupid function name change

	if unit:GetDomainType() == DomainTypes_DOMAIN_AIR then
		unitStrength = rangedStrength
		rangedStrength = 0
		unitMoves = unit:Range()
	else
		if unitOwnerID == activePlayerID then
			unitMoves = unit:MovesLeft()
		else
			unitMoves = unit:MaxMoves()
		end
		unitMoves = unitMoves / GameDefines_MOVE_DENOMINATOR
	end

	-- In Orbit?
	if unit.IsInOrbit and unit:IsInOrbit() then
		unitTip = unitTip .. " " .. "[COLOR_CYAN]" .. L( "TXT_KEY_PLOTROLL_ORBITING" ) .. "[ENDCOLOR]"

	else
		-- Moves
		if unitMoves > 0 then
			unitTip = string_format("%s %.3g[ICON_MOVES]", unitTip, unitMoves )
		end

		-- Strength
		if unitStrength > 0 then
			local adjustedUnitStrength = (math_max(100 + unit:GetStrategicResourceCombatPenalty(), 10) * unitStrength) / 100
			--todo other modifiers eg unhappy...
			if adjustedUnitStrength < unitStrength then
				adjustedUnitStrength = string_format(" [COLOR_NEGATIVE_TEXT]%.3g[ENDCOLOR]", adjustedUnitStrength )
			end
			unitTip = unitTip .. " " .. adjustedUnitStrength .. "[ICON_STRENGTH]"
		end

		-- Ranged Strength
		if rangedStrength > 0 then
			unitTip = unitTip .. " " .. rangedStrength .. "[ICON_RANGE_STRENGTH]"..unit:Range().." "
		end

		-- Religious Fervor
		if unit.GetReligion then
			local religionID = unit:GetReligion()
			if religionID > 0 then
				local spreadsLeft = unit:GetSpreadsLeft()
				unitTip = unitTip .. " "
				if spreadsLeft > 0 then
					unitTip = unitTip .. spreadsLeft
				end
				unitTip = unitTip .. tostring( (GameInfo.Religions[ religionID ] or {}).IconString ) .. L( Game.GetReligionName( religionID ) )
			end
		end

		-- Hit Points
		if unit:IsHurt() then
			unitTip = unitTip .. " " .. L( "TXT_KEY_PLOTROLL_UNIT_HP", unit:GetCurrHitPoints() )
		end
	end

	-- Embarked?
	if unit:IsEmbarked() then
		unitTip = unitTip .. " " .. L( "TXT_KEY_PLOTROLL_EMBARKED" )
	end
	
	return unitTip
end
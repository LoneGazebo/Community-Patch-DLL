-------------------------------------------------
-- NotificationPanel.lua
-- coded by bc1 from 1.0.3.276 code
-- to add a civilization list in notification panel
-- and minimize notification clutter
-- code is common using gk_mode and bnw_mode switches
-------------------------------------------------
include( "EUI_tooltips" )

Events.SequenceGameInitComplete.Add(function()
print("Loading EUI notification panel",ContextPtr,os.clock(),[[ 
 _   _       _   _  __ _           _   _             ____                  _ 
| \ | | ___ | |_(_)/ _(_) ___ __ _| |_(_) ___  _ __ |  _ \ __ _ _ __   ___| |
|  \| |/ _ \| __| | |_| |/ __/ _` | __| |/ _ \| '_ \| |_) / _` | '_ \ / _ \ |
| |\  | (_) | |_| |  _| | (_| (_| | |_| | (_) | | | |  __/ (_| | | | |  __/ |
|_| \_|\___/ \__|_|_| |_|\___\__,_|\__|_|\___/|_| |_|_|   \__,_|_| |_|\___|_|
]])
-------------------------------------------------
-- Minor lua optimizations
-------------------------------------------------
local ipairs = ipairs
local math_floor = math.floor
local math_huge = math.huge
local math_min = math.min
local pairs = pairs
local print = print
local table_concat = table.concat
local table_insert = table.insert
local table_remove = table.remove
local tostring = tostring
local unpack = unpack

--EUI_utilities
local IconLookup = EUI.IconLookup
local IconHookup = EUI.IconHookup
local CivIconHookup = EUI.CivIconHookup
local PushScratchDeal = EUI.PushScratchDeal
local PopScratchDeal = EUI.PopScratchDeal
local table = EUI.table
local Color = EUI.Color
local PrimaryColors = EUI.PrimaryColors
local GameInfo = EUI.GameInfoCache -- warning! use iterator ONLY with table field conditions, NOT string SQL query

include( "CityStateStatusHelper" )
local UpdateCityStateStatusIconBG = UpdateCityStateStatusIconBG
local GetCityStateStatusToolTip = GetCityStateStatusToolTip
local GetAllyToolTip = GetAllyToolTip
local GetActiveQuestText = GetActiveQuestText
local GetActiveQuestToolTip = GetActiveQuestToolTip

--EUI_tooltips
local GetMoodInfo = EUI.GetMoodInfo

local ButtonPopupTypes = ButtonPopupTypes
local ContextPtr = ContextPtr
local Controls = Controls
local Controls_SmallStack = Controls.SmallStack
local Controls_Scrap = Controls.Scrap
local Events = Events
local FromUIDiploEventTypes = FromUIDiploEventTypes
local Game = Game
local GameDefines = GameDefines
local GameInfoTypes = GameInfoTypes
local GameOptionTypes = GameOptionTypes
local IsGameCoreBusy = IsGameCoreBusy
local L = Locale.ConvertTextKey
local MajorCivApproachTypes = MajorCivApproachTypes
local Map_GetPlot = Map.GetPlot
local Map_GetPlotByIndex = Map.GetPlotByIndex
local Map_PlotDistance = Map.PlotDistance
local Matchmaking = Matchmaking
local MinorCivQuestTypes = MinorCivQuestTypes
local Mouse = Mouse
local Network = Network
local NotificationTypes = NotificationTypes
local Players = Players
local PreGame = PreGame
local ResourceUsageTypes = ResourceUsageTypes
local Teams = Teams
local ToHexFromGrid = ToHexFromGrid
local TradeableItems = TradeableItems
local UI = UI
local UIManager = UIManager

-------------------------------------------------
-- Globals
-------------------------------------------------
local gk_mode = Game.GetReligionName ~= nil
local bnw_mode = Game.GetActiveLeague ~= nil
local g_deal = UI.GetScratchDeal()

local g_tipControls = {}
TTManager:GetTypeControlTable( "EUI_CivilizationTooltip", g_tipControls )
local g_minorControlTable = {}
local g_majorControlTable = {}

local g_activePlayerID = Game.GetActivePlayer()
local g_activePlayer = Players[ g_activePlayerID ]
local g_activeTeamID = g_activePlayer:GetTeam()
local g_activeTeam = Teams[ g_activeTeamID ]

--local g_isOptionAlwaysWar = Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR )
--local g_isOptionAlwaysPeace = Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE )
--local g_isOptionNoChangingWarPeace = Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE )
local g_isNetworkMultiPlayer = Game.IsNetworkMultiPlayer()
local g_isHotSeatGame = PreGame.IsHotSeatGame()

--local g_colorWhite = Color( 1, 1, 1, 1 )
local g_colorWar = Color( 1, 0, 0, 1 )		-- "Red"
local g_colorDenounce = Color( 1, 0, 1, 1 )	-- "Orange"
local g_colorHuman = Color( 1, 1, 1, 1 )	-- "White"
local g_colorMajorCivApproach = {
[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_WAR ] = g_colorWar,
[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE ] = Color( 1, 0.5, 1, 1 ),		-- "Orange"
[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_GUARDED ] = Color( 1, 1, 0.5, 1 ),		-- "Yellow"
[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_AFRAID ] = Color( 1, 1, 0.5, 1 ),		-- "Yellow"
[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_FRIENDLY ] = Color( 0.5, 1, 0.5, 1 ),	-- "Green"
[ MajorCivApproachTypes.MAJOR_CIV_APPROACH_NEUTRAL ] = Color( 1, 1, 1, 1 ),		-- "White"
}

local g_leaderMode, g_LeaderPopups, g_leaderID
local g_screenWidth , g_screenHeight = UIManager:GetScreenSizeVal()
local g_chatPanelHeight = 170
local g_diploButtonsHeight = 108
local g_maxTotalStackHeight, g_isShowCivList, g_isUpdateCivList
local g_civPanelOffsetY = g_diploButtonsHeight
local g_alertMessages = {}
local g_alertButton

--[[ 
 _   _       _   _  __ _           _   _                   ____  _ _     _                 
| \ | | ___ | |_(_)/ _(_) ___ __ _| |_(_) ___  _ __  ___  |  _ \(_) |__ | |__   ___  _ __  
|  \| |/ _ \| __| | |_| |/ __/ _` | __| |/ _ \| '_ \/ __| | |_) | | '_ \| '_ \ / _ \| '_ \ 
| |\  | (_) | |_| |  _| | (_| (_| | |_| | (_) | | | \__ \ |  _ <| | |_) | |_) | (_) | | | |
|_| \_|\___/ \__|_|_| |_|\___\__,_|\__|_|\___/|_| |_|___/ |_| \_\_|_.__/|_.__/ \___/|_| |_|
]]
local g_mouseExit = true
local g_SpareNotifications = {}
local g_ActiveNotifications = {}
local g_NotificationButtons = {}
--[[
	{ (controls) Button etc..., (bundle) { { Id, type, toolTip, strSummary, iGameValue, iExtraGameData, playerID }, ... } }
--]]

-------------------------------------------------
-- List of notification types we can handle
-------------------------------------------------
local g_notificationNames = {}
local g_notificationBundled = {}
for k, v, w in ([[
	NOTIFICATION_POLICY				SocialPolicy
	NOTIFICATION_MET_MINOR				CityState		B
	NOTIFICATION_MINOR				CityState		B
	NOTIFICATION_MINOR_QUEST			CityState		special
	NOTIFICATION_ENEMY_IN_TERRITORY			EnemyInTerritory	B
	NOTIFICATION_REBELS				EnemyInTerritory	B
	NOTIFICATION_CITY_RANGE_ATTACK			CityCanBombard		B
	NOTIFICATION_BARBARIAN				Barbarian		B
	NOTIFICATION_GOODY				AncientRuins		B
	NOTIFICATION_BUY_TILE				BuyTile
	NOTIFICATION_CITY_GROWTH			CityGrowth
	NOTIFICATION_CITY_TILE				CityTile
	NOTIFICATION_DEMAND_RESOURCE			BonusResource
	NOTIFICATION_UNIT_PROMOTION			UnitPromoted		B
	NOTIFICATION_WONDER_STARTED			WonderConstructed
	NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER	WonderConstructed
	NOTIFICATION_WONDER_COMPLETED			WonderConstructed
	NOTIFICATION_WONDER_BEATEN			WonderConstructed
	NOTIFICATION_GOLDEN_AGE_BEGUN_ACTIVE_PLAYER	GoldenAge
	NOTIFICATION_GOLDEN_AGE_BEGUN			GoldenAge
	NOTIFICATION_GOLDEN_AGE_ENDED_ACTIVE_PLAYER	GoldenAgeComplete
	NOTIFICATION_GOLDEN_AGE_ENDED			GoldenAgeComplete
	NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER		GreatPerson
	NOTIFICATION_GREAT_PERSON			GreatPerson
	NOTIFICATION_STARVING				Starving		B
	NOTIFICATION_WAR_ACTIVE_PLAYER			War			B
	NOTIFICATION_WAR				WarOther		B
	NOTIFICATION_PEACE_ACTIVE_PLAYER		Peace			B
	NOTIFICATION_PEACE				PeaceOther		B
	NOTIFICATION_VICTORY				Victory
	NOTIFICATION_UNIT_DIED				UnitDied
	NOTIFICATION_CITY_LOST				CapitalLost
	NOTIFICATION_CAPITAL_LOST_ACTIVE_PLAYER		CapitalLost
	NOTIFICATION_CAPITAL_LOST			CapitalLost
	NOTIFICATION_CAPITAL_RECOVERED			CapitalRecovered
	NOTIFICATION_PLAYER_KILLED			CapitalLost
	NOTIFICATION_DISCOVERED_LUXURY_RESOURCE		LuxuryResource		B
	NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE	StrategicResource	B
	NOTIFICATION_DISCOVERED_BONUS_RESOURCE		BonusResource		B
	NOTIFICATION_POLICY_ADOPTION			Generic			B
	NOTIFICATION_DIPLO_VOTE				Generic
	NOTIFICATION_RELIGION_RACE			Generic
	NOTIFICATION_EXPLORATION_RACE			NaturalWonder
	NOTIFICATION_DIPLOMACY_DECLARATION		Diplomacy		B
	NOTIFICATION_DEAL_EXPIRED_GPT			DiplomacyX		B
	NOTIFICATION_DEAL_EXPIRED_RESOURCE		DiplomacyX		B
	NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS		DiplomacyX		B
	NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT	DiplomacyX		B
	NOTIFICATION_DEAL_EXPIRED_RESEARCH_AGREEMENT	ResearchAgreementX	B
	NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT	DiplomacyX		B
	NOTIFICATION_TECH_AWARD				TechAward
	NOTIFICATION_PLAYER_DEAL			Diplomacy
	NOTIFICATION_PLAYER_DEAL_RECEIVED		DiplomacyPlayer		
	NOTIFICATION_PLAYER_DEAL_RESOLVED		Diplomacy		B
	NOTIFICATION_PROJECT_COMPLETED			ProjectConstructed

	NOTIFICATION_TECH				Tech
	NOTIFICATION_PRODUCTION				Production
	NOTIFICATION_FREE_TECH				FreeTech
	NOTIFICATION_SPY_STOLE_TECH			StealTech
	NOTIFICATION_FREE_POLICY			FreePolicy
	NOTIFICATION_FREE_GREAT_PERSON			FreeGreatPerson

	NOTIFICATION_DENUNCIATION_EXPIRED		Diplomacy		B
	NOTIFICATION_FRIENDSHIP_EXPIRED			FriendshipX		B

	NOTIFICATION_FOUND_PANTHEON			FoundPantheon
	NOTIFICATION_FOUND_RELIGION			FoundReligion
	NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER	PantheonFounded
	NOTIFICATION_PANTHEON_FOUNDED			PantheonFounded		B
	NOTIFICATION_RELIGION_FOUNDED_ACTIVE_PLAYER	ReligionFounded
	NOTIFICATION_RELIGION_FOUNDED			ReligionFounded
	NOTIFICATION_ENHANCE_RELIGION			EnhanceReligion
	NOTIFICATION_RELIGION_ENHANCED_ACTIVE_PLAYER	ReligionEnhanced
	NOTIFICATION_RELIGION_ENHANCED			ReligionEnhanced	B
	NOTIFICATION_RELIGION_SPREAD			ReligionSpread		B

	NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER		NewSpy			B
	NOTIFICATION_SPY_CANT_STEAL_TECH		SpyCannotSteal		B
	NOTIFICATION_SPY_EVICTED			Spy			B
	NOTIFICATION_TECH_STOLEN_SPY_DETECTED		Spy			B
	NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED		Spy			B
	NOTIFICATION_SPY_KILLED_A_SPY			SpyKilledASpy		B
	NOTIFICATION_SPY_WAS_KILLED			SpyWasKilled		B
	NOTIFICATION_SPY_REPLACEMENT			Spy			B
	NOTIFICATION_SPY_PROMOTION			SpyPromotion		B
	NOTIFICATION_INTRIGUE_DECEPTION			Spy			B

	NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_ARMY			Spy	B
	NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_AMPHIBIOUS			Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN		Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN			Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN	Spy	B
	NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN		Spy	B
	NOTIFICATION_INTRIGUE_CONSTRUCTING_WONDER				Spy	B

	NOTIFICATION_SPY_RIG_ELECTION_SUCCESS		Spy				B
	NOTIFICATION_SPY_RIG_ELECTION_FAILURE		Spy				B
	NOTIFICATION_SPY_RIG_ELECTION_ALERT		Spy				B
	NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS		Spy				B
	NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE		SpyWasKilled			B
	NOTIFICATION_SPY_STAGE_COUP_SUCCESS		Spy				B
	NOTIFICATION_SPY_STAGE_COUP_FAILURE		Spy				B
	NOTIFICATION_DIPLOMAT_EJECTED			Diplomat			B

	NOTIFICATION_CAN_BUILD_MISSIONARY		EnoughFaith			B
	NOTIFICATION_AUTOMATIC_FAITH_PURCHASE_STOPPED	AutomaticFaithStop		B
	NOTIFICATION_OTHER_PLAYER_NEW_ERA		OtherPlayerNewEra		B

	NOTIFICATION_MAYA_LONG_COUNT			FreeGreatPerson
	NOTIFICATION_FAITH_GREAT_PERSON			FreeGreatPerson

	NOTIFICATION_EXPANSION_PROMISE_EXPIRED		Diplomacy			B
	NOTIFICATION_BORDER_PROMISE_EXPIRED		Diplomacy			B

	NOTIFICATION_TRADE_ROUTE			TradeRoute			B
	NOTIFICATION_TRADE_ROUTE_BROKEN			TradeRouteBroken		B

	NOTIFICATION_RELIGION_SPREAD_NATURAL		ReligionNaturalSpread		B

	NOTIFICATION_MINOR_BUYOUT			CityState			B

	NOTIFICATION_REQUEST_RESOURCE			BonusResource

	NOTIFICATION_ADD_REFORMATION_BELIEF		AddReformationBelief
	NOTIFICATION_LEAGUE_CALL_FOR_PROPOSALS		LeagueCallForProposals

	NOTIFICATION_CHOOSE_ARCHAEOLOGY			ChooseArchaeology
	NOTIFICATION_LEAGUE_CALL_FOR_VOTES		LeagueCallForVotes

	NOTIFICATION_REFORMATION_BELIEF_ADDED_ACTIVE_PLAYER	ReformationBeliefAdded
	NOTIFICATION_REFORMATION_BELIEF_ADDED			ReformationBeliefAdded	B

	NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER		GreatWork

	NOTIFICATION_LEAGUE_VOTING_DONE				LeagueVotingDone
	NOTIFICATION_LEAGUE_VOTING_SOON				LeagueVotingSoon

	NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL	CultureVictoryPositive	B
	NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO			CultureVictoryNegative	B
	NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER	CultureVictoryPositive	B
	NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE			CultureVictoryNegative	B
	NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER	CultureVictoryPositive	B
	NOTIFICATION_CULTURE_VICTORY_NO_LONGER_INFLUENTIAL	CultureVictoryNegative	B

	NOTIFICATION_CHOOSE_IDEOLOGY			ChooseIdeology			popup
	NOTIFICATION_IDEOLOGY_CHOSEN			IdeologyChosen			popup

	NOTIFICATION_LIBERATED_MAJOR_CITY		CapitalRecovered
	NOTIFICATION_RESURRECTED_MAJOR_CIV		CapitalRecovered

	NOTIFICATION_PLAYER_RECONNECTED			PlayerReconnected
	NOTIFICATION_PLAYER_DISCONNECTED		PlayerDisconnected
	NOTIFICATION_TURN_MODE_SEQUENTIAL		SequentialTurns
	NOTIFICATION_TURN_MODE_SIMULTANEOUS		SimultaneousTurns
	NOTIFICATION_HOST_MIGRATION			HostMigration
	NOTIFICATION_PLAYER_CONNECTING			PlayerConnecting
	NOTIFICATION_PLAYER_KICKED			PlayerKicked

	NOTIFICATION_CITY_REVOLT_POSSIBLE		Generic
	NOTIFICATION_CITY_REVOLT			Generic

	NOTIFICATION_LEAGUE_PROJECT_COMPLETE		LeagueProjectComplete
	NOTIFICATION_LEAGUE_PROJECT_PROGRESS		LeagueProjectProgress

	NOTIFICATION_INSTANT_YIELD			InstantYieldItem B
	NOTIFICATION_EVENT_CHOICE			ChooseEvent
	NOTIFICATION_EVENT_CHOICE_CITY		ChooseCityEvent
	
	NOTIFICATION_PRODUCTION_COST_MODIFIERS_FROM_RESOURCES		BonusResource

	NOTIFICATION_PRODUCTION_COST_MODIFIERS_FROM_RESOURCES		BonusResource

]]):gmatch("(%S+)[^%S\n\r]*(%S*)[^%S\n\r]*(%S*)[^\n\r]*") do
	local n = NotificationTypes[k]
	if n then
		g_notificationNames[ n ] = v
		g_notificationBundled[ n ] = w ~= ""
	end
end
--for k,v in pairs(NotificationTypes) do print( k, g_notificationNames[v], g_notificationBundled[v] and "Bundled" or "Single" ) end

-------------------------------------------------
-- Process Stack Sizes
-------------------------------------------------
local function ProcessStackSizes( resetCivPanelElevator )

	local maxTotalStackHeight, smallStackHeight
	if g_leaderMode then
		maxTotalStackHeight = g_screenHeight
		smallStackHeight = 0
	else
		Controls.BigStack:CalculateSize()
		Controls_SmallStack:CalculateSize()
		maxTotalStackHeight = g_maxTotalStackHeight - Controls.BigStack:GetSizeY()
		smallStackHeight = Controls_SmallStack:GetSizeY()
	end

	if g_isShowCivList then
		Controls.MinorStack:CalculateSize()
		Controls.MajorStack:CalculateSize()
		Controls.CivStack:CalculateSize()
		local halfTotalStackHeight = math_floor(maxTotalStackHeight / 2)
		local civStackHeight = Controls.CivStack:GetSizeY()

		if smallStackHeight + civStackHeight <= maxTotalStackHeight then
			halfTotalStackHeight = false
		elseif civStackHeight <= halfTotalStackHeight then
			smallStackHeight = maxTotalStackHeight - civStackHeight
			halfTotalStackHeight = false
		elseif smallStackHeight <= halfTotalStackHeight then
			civStackHeight = maxTotalStackHeight - smallStackHeight
		else
			civStackHeight = halfTotalStackHeight
			smallStackHeight = halfTotalStackHeight
		end

		Controls.CivScrollPanel:SetHide( not halfTotalStackHeight )
		if halfTotalStackHeight then
			Controls.CivStack:ChangeParent( Controls.CivScrollPanel )
			Controls.CivScrollPanel:SetSizeY( civStackHeight )
			Controls.CivScrollPanel:CalculateInternalSize()
			if resetCivPanelElevator then
				Controls.CivScrollPanel:SetScrollValue( 0 )
			end
		else
			Controls.CivStack:ChangeParent( Controls.CivPanel )
		end
		Controls.CivPanel:ReprocessAnchoring()
--		Controls.CivPanel:SetSizeY( civStackHeight )
	else
		smallStackHeight = math_min( smallStackHeight, maxTotalStackHeight )
	end

	if not g_leaderMode then
		Controls.SmallScrollPanel:SetSizeY( smallStackHeight )
		Controls.SmallScrollPanel:ReprocessAnchoring()
		Controls.SmallScrollPanel:CalculateInternalSize()
		if Controls.SmallScrollPanel:GetRatio() < 1 then
			Controls.SmallScrollPanel:SetOffsetX( 18 )
		else
			Controls.SmallScrollPanel:SetOffsetX( 0 )
		end
		Controls.OuterStack:CalculateSize()
		Controls.OuterStack:ReprocessAnchoring()
	end
end

-------------------------------------------------
-- Setup Notification
-------------------------------------------------

local function SetupNotification( instance, sequence, Id, type, toolTip, strSummary, iGameValue, iExtraGameData, playerID )

	if sequence then
		instance.Sequence = sequence
		Id, type, toolTip, strSummary, iGameValue, iExtraGameData, playerID = unpack( instance[ sequence ] )
--DEBUG analysis ONLY
--for k,v in pairs(NotificationTypes) do if v==type then toolTip = "[COLOR_RED]" .. k .. "[/COLOR][NEWLINE]" .. toolTip break end end
--toolTip = " [COLOR_YELLOW]Id = "..Id..", data1 = "..tostring(iGameValue)..", data2 = "..tostring(iExtraGameData).."[/COLOR][NEWLINE]"..toolTip
		local tips = {}
		for i = 1, #instance do
			local summary = instance[ i ][ 4 ]
			if #instance > 1 then
				summary = i .. ") " .. summary
			end
			if i == sequence then
				table_insert( tips, summary )
				if toolTip ~= summary then
					table_insert( tips, ( toolTip:gsub("%[NEWLINE%].*","") ) )
				end
			else
				table_insert( tips, "[COLOR_LIGHT_GREY]"..summary.."[ENDCOLOR]" )
			end
		end
		toolTip = table_concat( tips, "[NEWLINE]" )
	elseif toolTip ~= strSummary then
		toolTip = strSummary .. "[NEWLINE][NEWLINE]" .. toolTip
	end
	instance.Button:SetToolTipString( toolTip )
	if instance.Container then
		instance.FingerTitle:SetText( strSummary )
		local itemInfo, itemImage, smallCivFrame
-- todo reset finger animation - requires style modification
		if type == NotificationTypes.NOTIFICATION_WONDER_COMPLETED_ACTIVE_PLAYER
		or type == NotificationTypes.NOTIFICATION_WONDER_COMPLETED
		or type == NotificationTypes.NOTIFICATION_WONDER_BEATEN
		then
			itemInfo = GameInfo.Buildings[ iGameValue ]
			itemImage = instance.WonderConstructedAlphaAnim
			smallCivFrame = instance.WonderSmallCivFrame

		elseif type == NotificationTypes.NOTIFICATION_PROJECT_COMPLETED then
			itemInfo = GameInfo.Projects[ iGameValue ]
			itemImage = instance.ProjectConstructedAlphaAnim
			smallCivFrame = instance.ProjectSmallCivFrame

		elseif type == NotificationTypes.NOTIFICATION_DISCOVERED_LUXURY_RESOURCE
		or type == NotificationTypes.NOTIFICATION_DISCOVERED_STRATEGIC_RESOURCE
		or type == NotificationTypes.NOTIFICATION_DISCOVERED_BONUS_RESOURCE
		or type == NotificationTypes.NOTIFICATION_DEMAND_RESOURCE
		or type == NotificationTypes.NOTIFICATION_REQUEST_RESOURCE
		or type == NotificationTypes.NOTIFICATION_PRODUCTION_COST_MODIFIERS_FROM_RESOURCES
		then
			itemInfo = GameInfo.Resources[ iGameValue ]
			itemImage = instance.ResourceImage

		elseif type == NotificationTypes.NOTIFICATION_EXPLORATION_RACE then
			itemInfo = GameInfo.Features[ iGameValue ]
			itemImage = instance.NaturalWonderImage

		elseif type == NotificationTypes.NOTIFICATION_TECH_AWARD then
			itemInfo = GameInfo.Technologies[ iExtraGameData ]
			itemImage = instance.TechAwardImage

		elseif type == NotificationTypes.NOTIFICATION_PLAYER_DEAL_RECEIVED then
			local index = iGameValue;
			CivIconHookup( index, 80, instance.DiplomacyPlayerImage, instance.CivIconBG, instance.CivIconShadow, false, true );
		elseif type == NotificationTypes.NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER then
			smallCivFrame = instance.WonderSmallCivFrame

		elseif type == NotificationTypes.NOTIFICATION_UNIT_PROMOTION
		or type == NotificationTypes.NOTIFICATION_UNIT_DIED
		or type == NotificationTypes.NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER
		or type == NotificationTypes.NOTIFICATION_ENEMY_IN_TERRITORY
		or type == NotificationTypes.NOTIFICATION_REBELS
		then
			local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon( iGameValue, playerID )
			return IconHookup( portraitOffset, 80, portraitAtlas, instance.UnitImage )

		elseif type == NotificationTypes.NOTIFICATION_WAR_ACTIVE_PLAYER then
			return CivIconHookup( iGameValue, 80, instance.WarImage, instance.CivIconBG, instance.CivIconShadow, false, true )

		elseif type == NotificationTypes.NOTIFICATION_WAR then
			CivIconHookup( iGameValue, 45, instance.War1Image, instance.Civ1IconBG, instance.Civ1IconShadow, false, true )
			local team = Teams[ iExtraGameData ]
			return CivIconHookup( team and team:GetLeaderID() or -1, 45, instance.War2Image, instance.Civ2IconBG, instance.Civ2IconShadow, false, true )

		elseif type == NotificationTypes.NOTIFICATION_PEACE_ACTIVE_PLAYER then
			return CivIconHookup( iGameValue, 80, instance.PeaceImage, instance.CivIconBG, instance.CivIconShadow, false, true )

		elseif type == NotificationTypes.NOTIFICATION_PEACE then
			CivIconHookup( iGameValue, 45, instance.Peace1Image, instance.Civ1IconBG, instance.Civ1IconShadow, false, true )
			local team = Teams[iExtraGameData]
			return CivIconHookup( team and team:GetLeaderID() or -1, 45, instance.Peace2Image, instance.Civ2IconBG, instance.Civ2IconShadow, false, true )

		elseif type == NotificationTypes.NOTIFICATION_CITY_TILE then
			local plot = Map_GetPlotByIndex( iGameValue )
			local info = plot and plot:GetResourceType( g_activeTeamID )
			info = info and GameInfo.Resources[info]	-- or GameInfo.Terrains[info]
			local offset, texture
			if info then
				offset, texture = IconLookup( info.PortraitIndex, 80, info.IconAtlas )
				if texture then
					instance.ResourceImage:SetTextureOffsetVal( offset.x, offset.y +2 )
					instance.ResourceImage:SetTexture( texture or "NotificationTileGlow.dds" )
				end
			end
			return instance.ResourceImage:SetHide( not texture )
		end

		if itemInfo and itemImage then
			IconHookup( itemInfo.PortraitIndex, 80, itemInfo.IconAtlas, itemImage )
		end
		if smallCivFrame then
			if iExtraGameData ~= -1 then
				CivIconHookup( iExtraGameData, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true )
				smallCivFrame:SetHide( false )
			else
--				CivIconHookup( 22, 45, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true )
				smallCivFrame:SetHide( true )
			end
		end
	end
end

-------------------------------------------------
-- Notification Click Handlers
-------------------------------------------------
local function MouseExit()
	g_mouseExit = true
end

local function GenericLeftClick( Id )
	local index = g_ActiveNotifications[ Id ]
	local instance = g_NotificationButtons[ index ]
	if instance and #instance > 0 then
		local sequence = instance.Sequence
		if g_mouseExit then
			g_mouseExit = false
		else
			sequence = sequence % #instance + 1
			SetupNotification( instance, sequence )
		end
		local data = instance[ sequence ]
		Id = data[1]
		-- Special kludge to work around DLL's stupid city state popups
		if data[2] == NotificationTypes.NOTIFICATION_MINOR_QUEST then
			local minorPlayer = Players[ data[5] ]
			if minorPlayer then
				local city = minorPlayer:GetCapitalCity()
-- todo: doesn't seem to work
				local plot = Map_GetPlot( minorPlayer:GetQuestData1( data[7], data[6] ),
							  minorPlayer:GetQuestData2( data[7], data[6] ) )
						or ( city and city:Plot() )
				if plot then
					UI.LookAt(plot, 0)
					local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
					Events.GameplayFX( hex.x, hex.y, -1 )
					return
				end
			end
		end
	end
	UI.ActivateNotification( Id )
end

local function GenericRightClick ( Id )
	g_mouseExit = true
	local instance = g_NotificationButtons[ g_ActiveNotifications[ Id ] ]
	if instance and #instance > 0 then
		if UI.ShiftKeyDown() then
			UI.RemoveNotification( instance[instance.Sequence][1] )
		else
			for sequence = 1, #instance do
				UI.RemoveNotification( instance[sequence][1] )
			end
		end
	else
		UI.RemoveNotification( Id )
	end
end

for _, button in pairs( Controls ) do
	if button.ClearCallback then
		button:RegisterCallback( Mouse.eLClick, GenericLeftClick )
		button:RegisterCallback( Mouse.eRClick, GenericRightClick )
		if UI.IsTouchScreenEnabled() then
			button:RegisterCallback( Mouse.eLDblClick, GenericRightClick )
		end
	end
end

-------------------------------------------------
-- Notification Added
-------------------------------------------------
Events.NotificationAdded.Add(
function( Id, type, ... ) -- toolTip, strSummary, iGameValue, iExtraGameData, playerID )

	local name = not g_ActiveNotifications[ Id ] and (g_notificationNames[ type ] or "Generic")
	if name then
		local button = Controls[ name ]
		local bundled = button or g_notificationBundled[ type ]
		local index = bundled and name or Id
		g_ActiveNotifications[ Id ] = index
		local instance = g_NotificationButtons[ index ]
		if not instance then
			if button then
				button:SetHide( false )
				instance = { Button = button }
				if type == NotificationTypes.NOTIFICATION_FOUND_RELIGION
				   or type == NotificationTypes.NOTIFICATION_ENHANCE_RELIGION
				   or type == NotificationTypes.NOTIFICATION_ADD_REFORMATION_BELIEF
				then
					UI.ActivateNotification( Id )
				end
			else
				instance = g_SpareNotifications[ name ]
				instance = instance and table_remove( instance )
				if instance then
					instance.Container:ChangeParent( Controls_SmallStack )
					button = instance.Button
				else
					instance = {}
					ContextPtr:BuildInstanceForControl( name, instance, Controls_SmallStack )
					instance.Name = name
					button = instance.Button
					button:RegisterCallback( Mouse.eLClick, GenericLeftClick )
					button:RegisterCallback( Mouse.eRClick, GenericRightClick )
					button:RegisterCallback( Mouse.eMouseExit, MouseExit )
					if UI.IsTouchScreenEnabled() then
						button:RegisterCallback( Mouse.eLDblClick, GenericRightClick )
					end
				end
				instance.Container:BranchResetAnimation()
			end
			g_NotificationButtons[ index ] = instance
			button:SetVoid1( Id )
		end
		if bundled then
			table_insert( instance, { Id, type, ... } )
			SetupNotification( instance, #instance )
		else
			SetupNotification( instance, nil, Id, type, ... )
		end

		ProcessStackSizes( true )
	end
end)

-------------------------------------------------
-- Remove Notification
-------------------------------------------------

local function RemoveNotificationID( Id )

	local index = g_ActiveNotifications[ Id ]
	g_ActiveNotifications[ Id ] = nil
	local instance = g_NotificationButtons[ index ]
	if instance then
		for i = 1, #instance do
			-- Remove bundle item which corresponds to Id
			if instance[i][ 1 ] == Id then
				table_remove( instance, i )
				break
			end
		end
		local button = instance.Button
		-- Is bundle now empty ?
		if #instance <= 0 then
			local name = instance.Name
			if name then
				local spares = g_SpareNotifications[ name ]
				if not spares then
					spares = {}
					g_SpareNotifications[ name ] = spares
				end
				table_insert( spares, instance )
				instance.Container:ChangeParent( Controls_Scrap )
			else
				button:SetHide( true )
			end
			g_NotificationButtons[ index ] = nil
		-- Update notification
		else
			local sequence = instance.Sequence
			if sequence > #instance then
				sequence = 1
			end
			instance.Button:SetVoid1( instance[ sequence ][1] )
			SetupNotification( instance, sequence )
		end
	end
end

Events.NotificationRemoved.Add(
function( Id )

--print( "removing Notification " .. Id .. " " .. tostring( g_ActiveNotifications[ Id ] ) .. " " .. tostring( g_notificationNames[ g_ActiveNotifications[ Id ] ] ) )

	RemoveNotificationID( Id )
	ProcessStackSizes()

end)

-------------------------------------------------
-- Additional notifications
-------------------------------------------------
GameEvents.SetPopulation.Add(
function( x, y, oldPopulation, newPopulation )
	local plot = Map_GetPlot( x, y )
	local city = plot and plot:GetPlotCity()
	local playerID = city and city:GetOwner()
	--print("Player#", playerID, "City:", city and city:GetName(), x, y, plot)
	if playerID == g_activePlayerID	-- active player only
		and newPopulation > 5			-- game engine already does up to 5 pop
		and newPopulation > oldPopulation	-- growth only
		and not city:IsPuppet()			-- who cares ? nothing to be done
		and not city:IsResistance()		-- who cares ? nothing to be done
		and Game.GetGameTurn() > city:GetGameTurnAcquired() -- inhibit upon city creation & capture
	then
		g_activePlayer:AddNotification(NotificationTypes.NOTIFICATION_CITY_GROWTH,
			L("TXT_KEY_NOTIFICATION_CITY_GROWTH", city:GetName(), newPopulation ),
			L("TXT_KEY_NOTIFICATION_SUMMARY_CITY_GROWTH", city:GetName() ),
			x, y, plot:GetPlotIndex() )	--iGameDataIndex, int iExtraGameData
		--print( "Notification sent:", NotificationTypes.NOTIFICATION_CITY_GROWTH, sTip, sTitle, x, y )
	end
end)

GameEvents.CityBoughtPlot.Add(
function( playerID, cityID, x, y, isWithGold, isWithFaithOrCulture )

	if isWithFaithOrCulture and playerID == g_activePlayerID then
		--print( "Border growth at coordinates: ", x, y, "playerID:", playerID, "isWithGold", isWithGold, "isWithFaithOrCulture", isWithFaithOrCulture )
		local plot = Map_GetPlot( x, y )
		local city = g_activePlayer:GetCityByID( cityID )
		--print( "CityTileNotification:", city and city:GetName(), x, y, plot, city and city:GetCityPlotIndex(plot) )

		if plot and city and ( ( plot:GetWorkingCity() and not city:IsPuppet() ) or Game.GetResourceUsageType( plot:GetResourceType( g_activeTeamID ) ) > 0 )
		-- valid plot, either worked by city which is not a puppet, or has some kind of resource we can use
		then
			g_activePlayer:AddNotification( NotificationTypes.NOTIFICATION_CITY_TILE,
				L( "TXT_KEY_NOTIFICATION_CITY_CULTURE_ACQUIRED_NEW_PLOT", city:GetName() ),
				L( "TXT_KEY_NOTIFICATION_SUMMARY_CITY_CULTURE_ACQUIRED_NEW_PLOT", city:GetName() ),
				x, y, plot:GetPlotIndex() )	--iGameDataIndex, int iExtraGameData
			--print( "CityTileNotification sent:", NotificationTypes.NOTIFICATION_CITY_TILE, city:GetName(), x, y )
		end
	end
end)

--[[ 
  ____ _       _ _ _          _   _                   ____  _ _     _                 
 / ___(_)_   _(_) (_)______ _| |_(_) ___  _ __  ___  |  _ \(_) |__ | |__   ___  _ __  
| |   | \ \ / / | | |_  / _` | __| |/ _ \| '_ \/ __| | |_) | | '_ \| '_ \ / _ \| '_ \ 
| |___| |\ V /| | | |/ / (_| | |_| | (_) | | | \__ \ |  _ <| | |_) | |_) | (_) | | | |
 \____|_| \_/ |_|_|_/___\__,_|\__|_|\___/|_| |_|___/ |_| \_\_|_.__/|_.__/ \___/|_| |_|
]]

-------------------------------------------------
-- Sort Functions
-------------------------------------------------

local function SortStack( instance1, instance2 )

	if instance1 and instance2 then
		for i = 3, 1, -1 do
			if instance1[i] ~= instance2[i] then
				return instance1[i] > instance2[i]
			end
		end
	end
end

local function SortMajorStack( control1, control2 )
	return SortStack( g_majorControlTable[ control1:GetVoid1() ], g_majorControlTable[ control2:GetVoid1() ] )
end

local function SortMinorStack( control1, control2 )
	return SortStack( g_minorControlTable[ control1:GetVoid1() ], g_minorControlTable[ control2:GetVoid1() ] )
end

-------------------------------------------------
-- Utility Functions
-------------------------------------------------
local questKillCamp = MinorCivQuestTypes.MINOR_CIV_QUEST_KILL_CAMP
local isQuestKillCamp
-- CBP
local questDig = MinorCivQuestTypes.MINOR_CIV_QUEST_ARCHAEOLOGY
local isQuestDig

local questPlot = MinorCivQuestTypes.MINOR_CIV_QUEST_DISCOVER_PLOT
local isQuestPlot

local questCity = MinorCivQuestTypes.MINOR_CIV_QUEST_UNIT_GET_CITY
local isQuestCity
-- END
if bnw_mode then
	function isQuestKillCamp( minorPlayer )
		return minorPlayer:IsMinorCivDisplayedQuestForPlayer( g_activePlayerID, questKillCamp )
	end
-- CBP
	function isQuestDig( minorPlayer )
		return minorPlayer:IsMinorCivDisplayedQuestForPlayer( g_activePlayerID, questDig )
	end
	isQuestPlot = function( minorPlayer )
		return minorPlayer:IsMinorCivDisplayedQuestForPlayer( g_activePlayerID, questPlot )
	end
	isQuestCity = function( minorPlayer )
		return minorPlayer:IsMinorCivDisplayedQuestForPlayer( g_activePlayerID, questCity )
	end
-- END
elseif gk_mode then
	function isQuestKillCamp( minorPlayer )
		return minorPlayer:IsMinorCivActiveQuestForPlayer( g_activePlayerID, questKillCamp )
	end
else
	function isQuestKillCamp( minorPlayer )
		return minorPlayer:GetActiveQuestForPlayer( g_activePlayerID ) == questKillCamp
	end
end

local function ShowSimpleTip( toolTip )
	g_tipControls.Text:SetText( toolTip )
	g_tipControls.PortraitFrame:SetHide( true )
	g_tipControls.Box:SetHide( false )
	g_tipControls.Box:DoAutoSize()
end

local function FindPlayerID( controlTable, control )
	local controlName = control:GetID()
	for ID, instance in pairs( controlTable ) do
		if instance[ controlName ] == control then
			return ID
		end
	end
	return -1
end

local function ShowSimpleTipWithRemainingTurns( toolTip, remainingTurns )
	toolTip = L( toolTip )
	if remainingTurns and remainingTurns > 0 then
		toolTip = toolTip .. " (" .. L( "TXT_KEY_STR_TURNS", remainingTurns ) .. ")"
	end
	ShowSimpleTip( toolTip )
end

local function ShowSimpleTipAndGetRemainingTurns( toolTip, tradeableItemID, fromPlayerID, toPlayerID )
	PushScratchDeal()
	for i = 0, UI.GetNumCurrentDeals( g_activePlayerID ) - 1 do
		UI.LoadCurrentDeal( g_activePlayerID, i )
		if not toPlayerID or toPlayerID == g_activePlayerID or toPlayerID == g_deal:GetOtherPlayer( g_activePlayerID ) then
			g_deal:ResetIterator()
			local itemID
			repeat
				local item = { g_deal:GetNextItem() }
				itemID = item[1]
				if itemID == tradeableItemID and item[#item] == fromPlayerID then
					PopScratchDeal()
					return ShowSimpleTipWithRemainingTurns( toolTip, item[3] - Game.GetGameTurn() + 1 )
				end
			until not itemID
		end
	end
	PopScratchDeal()
	ShowSimpleTip( L(toolTip) )
end

local function FindPlayer( controlTable, control )
	local playerID = FindPlayerID( controlTable, control )
	return Players[ playerID ], playerID
end


local g_civListInstanceToolTips = { -- the tooltip function names need to match associated instance control ID defined in xml

	Button = function( control )
		local playerID = control:GetVoid1()
		local player = Players[playerID]
		if player then
			local isShowPortrait = false
			if player:IsMinorCiv() then
				g_tipControls.Text:SetText( GetCityStateStatusToolTip( g_activePlayerID, playerID, true ) )
			else
				g_tipControls.Text:SetText( GetMoodInfo( playerID, true ) )
				local leader = GameInfo.Leaders[player:GetLeaderType()]
				isShowPortrait = IconHookup( leader.PortraitIndex, g_tipControls.Portrait:GetSizeY(), leader.IconAtlas, g_tipControls.Portrait )
				CivIconHookup( playerID, g_tipControls.CivIconBG:GetSizeY(), g_tipControls.CivIcon, g_tipControls.CivIconBG, g_tipControls.CivIconShadow, false, true )
			end
			g_tipControls.PortraitFrame:SetHide( not isShowPortrait )
			g_tipControls.Box:SetHide( false )
			g_tipControls.Box:DoAutoSize()
		end
	end;

	Quests = function( control )
		ShowSimpleTip( GetActiveQuestToolTip( g_activePlayerID, control:GetVoid1() ) )
	end;

	Ally = function( control )
		ShowSimpleTip( GetAllyToolTip( g_activePlayerID, FindPlayerID( g_minorControlTable, control ) ) )
	end;

	Pledge1 = function( control )
		local minorPlayer = FindPlayer( g_minorControlTable, control )
		local toolTip = L"TXT_KEY_POP_CSTATE_PLEDGE_TO_PROTECT"
		if minorPlayer and gk_mode then
			toolTip = L( "TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_PLEDGE_TO_PROTECT", minorPlayer:GetCivilizationShortDescriptionKey() )
			if minorPlayer:CanMajorWithdrawProtection( g_activePlayerID ) then
				toolTip = toolTip .. "[NEWLINE][NEWLINE]" .. L"TXT_KEY_POP_CSTATE_REVOKE_PROTECTION_TT"
			else
-- CBP
				toolTip = toolTip .. L("TXT_KEY_POP_CSTATE_REVOKE_PROTECTION_DISABLED_COMMITTED_TT", minorPlayer:GetTurnLastPledgedProtectionByMajor( g_activePlayerID ) + GameDefines.BALANCE_MINOR_PROTECTION_MINIMUM_DURATION - Game.GetGameTurn() )
				--toolTip = toolTip .. L("TXT_KEY_POP_CSTATE_REVOKE_PROTECTION_DISABLED_COMMITTED_TT", minorPlayer:GetTurnLastPledgedProtectionByMajor( g_activePlayerID ) + 10 - Game.GetGameTurn() )
-- END
			end
		end
		ShowSimpleTip( toolTip )
	end;

	Spy = function( control )
		local player, playerID = FindPlayer( g_minorControlTable, control )
		local spy
		for _, s in ipairs( g_activePlayer:GetEspionageSpies() ) do
			local plot = Map_GetPlot( s.CityX, s.CityY )
			local city = plot and plot:GetPlotCity()
			if city and city:GetOwner() == playerID then
				spy = s
				break
			end
		end
		if spy and player then
			ShowSimpleTip( L( "TXT_KEY_CITY_SPY_CITY_STATE_TT", spy.Rank, spy.Name, player:GetCivilizationShortDescriptionKey(), spy.Rank, spy.Name) )
		end
	end;

	DeclarationOfFriendship = function( control )
		local playerID = FindPlayerID( g_majorControlTable, control )
		local toolTipKey = "TXT_KEY_DIPLOMACY_FRIENDSHIP_ADV_QUEST"
		if bnw_mode then
			ShowSimpleTipWithRemainingTurns( toolTipKey, GameDefines.DOF_EXPIRATION_TIME - g_activePlayer:GetDoFCounter( playerID ) )
		else
			ShowSimpleTip( L(toolTipKey) )
		end
	end;

	ResearchAgreement = function( control )
		local playerID = FindPlayerID( g_majorControlTable, control )
		ShowSimpleTipAndGetRemainingTurns( "TXT_KEY_DO_RESEARCH_AGREEMENT", TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, playerID )
	end;

	DefenseAgreement = function( control )
		local playerID = FindPlayerID( g_majorControlTable, control )
		ShowSimpleTipAndGetRemainingTurns( "TXT_KEY_DO_PACT", TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, playerID )
	end;

	--C4DF
	Master = function( control )
		local player = FindPlayer( g_majorControlTable, control )
		local team = Teams[ player:GetTeam() ]
		if team:IsVassal( g_activeTeamID ) then
			ShowSimpleTip( L( "TXT_KEY_DIPLO_MASTER_OF", player:GetCivilizationShortDescriptionKey() ) )
		end
	end;

	Vassal = function( control )
		local player = FindPlayer( g_majorControlTable, control )
		if Teams[g_activeTeamID]:IsVassal( player:GetTeam() ) then
			ShowSimpleTip( L( "TXT_KEY_DIPLO_VASSAL_OF", player:GetCivilizationShortDescriptionKey() ) )
		end
	end;
	--END

	TheirBordersClosed = function( control )
		ShowSimpleTip( L"TXT_KEY_EUI_CLOSED_BORDERS_THEIR" )	--( "Their borders are closed" )
	end;

	OurBordersClosed = function( control )
		ShowSimpleTip( L"TXT_KEY_EUI_CLOSED_BORDERS_YOUR" )	--( "Your borders are closed" )
	end;

	TheirBordersOpen = function( control )
		local playerID = FindPlayerID( g_majorControlTable, control )
		local toolTip = "TXT_KEY_EUI_OPEN_BORDERS_THEIR"
		if not Locale.HasTextKey( toolTip ) then
			toolTip = L( "TXT_KEY_DO_THEY_PROVIDE", "TXT_KEY_DO_OPEN_BORDERS" )
		end
		ShowSimpleTipAndGetRemainingTurns( toolTip, TradeableItems.TRADE_ITEM_OPEN_BORDERS, playerID )
	end;

	OurBordersOpen = function( control )
		local playerID = FindPlayerID( g_majorControlTable, control )
		local toolTip = "TXT_KEY_EUI_OPEN_BORDERS_YOUR"
		if not Locale.HasTextKey( toolTip ) then
			toolTip = L( "TXT_KEY_DO_WE_PROVIDE", "TXT_KEY_DO_OPEN_BORDERS" )
		end
		ShowSimpleTipAndGetRemainingTurns( toolTip, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_activePlayerID, playerID )
	end;

	ActivePlayer = function()-- control )
		ShowSimpleTip( L"TXT_KEY_YOU" )
	end;

	War = function( control )
		local player = FindPlayer( g_majorControlTable, control )
		local playerID = FindPlayerID( g_majorControlTable, control ) -- UndeadDevel
		local tips = table() -- UndeadDevel: don't initialize with a first element since it doesn't seem to work anyway
		local teamID = player:GetTeam()
		local lockedWarTurns = g_activeTeam:GetNumTurnsLockedIntoWar( teamID )
		if lockedWarTurns > 0 then
			tips:insert( L( "TXT_KEY_DIPLO_NEGOTIATE_PEACE_BLOCKED_TT", lockedWarTurns ) .. "[NEWLINE][NEWLINE]") -- UndeadDevel: add newlines here instead to avoid having unused space
		elseif not g_activeTeam:CanChangeWarPeace( teamID ) then
			tips:insert( L"TXT_KEY_PEACE_BLOCKED"  .. "[NEWLINE][NEWLINE]") -- UndeadDevel: add newlines here instead to avoid having unused space
		end
				
-- UndeadDevel: update war status and peace willingness and add war info to tool tip
		if (not Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR ) and playerID ~= g_leaderID and not g_leaderMode and playerID ~= g_activePlayerID and not player:IsHuman() and not player:IsMinorCiv() and not player:IsBarbarian() and not IsGameCoreBusy()) then

			player:DoUpdateWarDamageLevel();
			player:DoUpdatePeaceTreatyWillingness();
			
			local strWarInfo = "";
	
			if(player:IsWillingToMakePeaceWithHuman(g_activePlayerID)) then
				local iPeaceValue = player:GetTreatyWillingToOffer(g_activePlayerID);
				if(iPeaceValue >  PeaceTreatyTypes.PEACE_TREATY_WHITE_PEACE) then
					if( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_ARMISTICE ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_ARMISTICE" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_SETTLEMENT ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_SETTLEMENT" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_BACKDOWN ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_BACKDOWN" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_SUBMISSION ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_SUBMISSION" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_SURRENDER ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_SURRENDER" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_CESSION ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_CESSION" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_CAPITULATION ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_CAPITULATION" );
					elseif( iPeaceValue == PeaceTreatyTypes.PEACE_TREATY_UNCONDITIONAL_SURRENDER ) then
						strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_OFFER_PEACE_TREATY_UNCONDITIONAL_SURRENDER" );
					end
				end
			else
				strWarInfo = strWarInfo .. Locale.ConvertTextKey( "TXT_KEY_WAR_NO_PEACE_OFFER" );
			end
					
			local iStrengthAverage = g_activePlayer:GetPlayerMilitaryStrengthComparedToUs(playerID);
			if( iStrengthAverage == StrengthTypes.STRENGTH_PATHETIC ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_PATHETIC" );
			elseif( iStrengthAverage == StrengthTypes.STRENGTH_WEAK ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_WEAK" );
			elseif( iStrengthAverage == StrengthTypes.STRENGTH_POOR ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_POOR" );
			elseif( iStrengthAverage == StrengthTypes.STRENGTH_AVERAGE ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_AVERAGE" );
			elseif( iStrengthAverage == StrengthTypes.STRENGTH_STRONG ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_STRONG" );
			elseif( iStrengthAverage == StrengthTypes.STRENGTH_POWERFUL ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_POWERFUL" );
			elseif( iStrengthAverage == StrengthTypes.STRENGTH_IMMENSE ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_STRENGTH_IMMENSE" );
			end
					
			local iEconomicAverage = g_activePlayer:GetPlayerEconomicStrengthComparedToUs(playerID);
			if( iEconomicAverage == StrengthTypes.STRENGTH_PATHETIC ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_PATHETIC" );
			elseif( iEconomicAverage == StrengthTypes.STRENGTH_WEAK ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_WEAK" );
			elseif( iEconomicAverage == StrengthTypes.STRENGTH_POOR ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_POOR" );
			elseif( iEconomicAverage == StrengthTypes.STRENGTH_AVERAGE ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_AVERAGE" );
			elseif( iEconomicAverage == StrengthTypes.STRENGTH_STRONG ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_STRONG" );
			elseif( iEconomicAverage == StrengthTypes.STRENGTH_POWERFUL ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_POWERFUL" );
			elseif( iEconomicAverage == StrengthTypes.STRENGTH_IMMENSE ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_ECONOMY_IMMENSE" );
			end
					
			local iUsWarDamage = player:GetWarDamageLevel(g_activePlayerID);
			local iThemWarDamage = g_activePlayer:GetWarDamageLevel(playerID);
			if(iUsWarDamage > WarDamageLevelTypes.WAR_DAMAGE_LEVEL_NONE)then
				if( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MINOR ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_MINOR" );
				elseif( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MAJOR ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_MAJOR" );
				elseif( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_SERIOUS ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_SERIOUS" );
				elseif( iUsWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_CRIPPLED ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_US_CRIPPLED" );
				end
			elseif(iThemWarDamage > WarDamageLevelTypes.WAR_DAMAGE_LEVEL_NONE)then
				if( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MINOR ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_MINOR" );
				elseif( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_MAJOR ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_MAJOR" );
				elseif( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_SERIOUS ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_SERIOUS" );
				elseif( iThemWarDamage == WarDamageLevelTypes.WAR_DAMAGE_LEVEL_CRIPPLED ) then
					strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_DAMAGE_THEM_CRIPPLED" );
				end
			end
					
			local iTheirWarWeariness = player:GetWarWeariness();
			if(iTheirWarWeariness <= 0)then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_WEARINESS_THEM_NONE" );
			elseif( iTheirWarWeariness <= 25 ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_WEARINESS_THEM_MINOR" );
			elseif( iTheirWarWeariness <= 50 ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_WEARINESS_THEM_MAJOR" );
			elseif( iTheirWarWeariness <= 75 ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_WEARINESS_THEM_SERIOUS" );
			elseif( iTheirWarWeariness > 75 ) then
				strWarInfo = strWarInfo .. '[NEWLINE]' .. Locale.ConvertTextKey( "TXT_KEY_WAR_WEARINESS_THEM_CRIPPLED" );
			end
				
			tips:insert(strWarInfo);
		end
-- UndeadDevel end

-- todo TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR & permanent war
		ShowSimpleTip( tips:concat() ) -- UndeadDevel: don't add new lines here...there will be empty lines at the top of the tool tip otherwise
	end;

	Score = function( control )
		local player = FindPlayer( g_majorControlTable, control )
		local tips = table(	L"TXT_KEY_POP_SCORE" .. " " .. player:GetScore(),	--TXT_KEY_VP_SCORE
					"----------------",
					L("TXT_KEY_DIPLO_MY_SCORE_CITIES", player:GetScoreFromCities() ),
					L("TXT_KEY_DIPLO_MY_SCORE_POPULATION", player:GetScoreFromPopulation() ),
					L("TXT_KEY_DIPLO_MY_SCORE_LAND", player:GetScoreFromLand() ),
					L("TXT_KEY_DIPLO_MY_SCORE_WONDERS", player:GetScoreFromWonders() ) )
		if not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
			tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_TECH", player:GetScoreFromTechs() ) )
			tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_FUTURE_TECH", player:GetScoreFromFutureTech() ) )
		end
-- Putmalk
		if (not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_VASSALAGE)) then
			tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_VASSALS", player:GetScoreFromVassals() ) )
		end
--END 
		--CBP
		tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_ALLIES", player:GetScoreFromMinorAllies() ) )
		tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_MILITARY", player:GetScoreFromMilitarySize() ) )
-- END
		if bnw_mode then
			if not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_RELIGION) then
				tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_RELIGION", player:GetScoreFromReligion() ) )
			end
			if not Game.IsOption(GameOptionTypes.GAMEOPTION_NO_POLICIES) then
				tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_POLICIES", player:GetScoreFromPolicies() ) )
			end
			tips:insert( L("TXT_KEY_DIPLO_MY_SCORE_GREAT_WORKS", player:GetScoreFromGreatWorks() ) )
			if PreGame.GetLoadWBScenario() then
				for i = 1, 4 do
					local key = "TXT_KEY_DIPLO_MY_SCORE_SCENARIO"..i
					if Locale.HasTextKey( key ) then
						tips:insert( L( key, player["GetScoreFromScenario"..i](player) ) )
					end
				end
			end
		end
		ShowSimpleTip( tips:concat("[NEWLINE]" ) )
	end;

	Gold = function( control )
		local player = FindPlayer( g_majorControlTable, control )
		local team = Teams[ player:GetTeam() ]
		if team:IsAtWar( g_activeTeamID ) then
			ShowSimpleTip( L"TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR" )
		elseif not bnw_mode or player:IsDoF( g_activePlayerID ) then
			ShowSimpleTip( L"TXT_KEY_REPLAY_DATA_TOTALGOLD" )
		else
			ShowSimpleTip( L"TXT_KEY_REPLAY_DATA_GOLDPERTURN" )
		end
	end;
	
	TheirTradeItems = function( control )
		local player = FindPlayer( g_majorControlTable, control )
		ShowSimpleTip( L( "TXT_KEY_DIPLO_ITEMS_LABEL", player:GetCivilizationAdjective() ) )
	end;

	OurTradeItems = function()-- control )
		ShowSimpleTip( L"TXT_KEY_DIPLO_YOUR_ITEMS_LABEL" )
	end;

	Host = function()
		ShowSimpleTip( L"TXT_KEY_HOST" )
	end;

	Connection = function( control )
		local player, playerID = FindPlayer( g_majorControlTable, control )
		local toolTip
		if Network.IsPlayerHotJoining(playerID) then
			toolTip = L"TXT_KEY_MP_PLAYER_CONNECTING"
		elseif player:IsConnected() then
			toolTip = L"TXT_KEY_MP_PLAYER_CONNECTED"
		else
			toolTip = L"TXT_KEY_MP_PLAYER_NOTCONNECTED"
		end
		if Matchmaking.GetHostID() == playerID then
			toolTip = L"TXT_KEY_HOST" .. ", "..toolTip
		end
		local playerInfo
		local ping = ""
		if playerID == g_activePlayerID then
			playerInfo = Network.GetLocalTurnSliceInfo()
		else
			playerInfo = Network.GetPlayerTurnSliceInfo( playerID )
		end
		if PreGame.IsInternetGame() then
			ping = Network.GetPingTime( playerID )
			if ping < 0 then
				ping = ""
			elseif ping == 0 then
				ping= L"TXT_KEY_STAGING_ROOM_UNDER_1_MS"
			elseif ping < 1000 then
				ping = ping .. L"TXT_KEY_STAGING_ROOM_TIME_MS"
			else
				ping = ("%.2f"):format( ping / 1000) .. L"TXT_KEY_STAGING_ROOM_TIME_S"
			end
			if ping>"" then
				ping = L"TXT_KEY_ACTION_PING".." "..ping.." "
			end
		end
		ShowSimpleTip( toolTip .. "[NEWLINE][NEWLINE]"..ping.."Network turn slice: "
			.. playerInfo.Shortest .. " ("
			.. playerInfo.Average .. ") "
			.. playerInfo.Longest )
	end;

	Diplomacy = function( control )
		local playerID = FindPlayerID( g_majorControlTable, control )
		if UI.ProposedDealExists( playerID, g_activePlayerID ) then
			ShowSimpleTip( L"TXT_KEY_DIPLO_REQUEST_INCOMING" )
		elseif UI.ProposedDealExists( g_activePlayerID, playerID ) then
			ShowSimpleTip( L"TXT_KEY_DIPLO_REQUEST_OUTGOING" )
		end
	end;
}-- /g_civListInstanceToolTips
g_civListInstanceToolTips.Pledge2 = g_civListInstanceToolTips.Pledge1

local g_civListInstanceCallBacks = {-- the callback function table names need to match associated instance control ID defined in xml
	Button = {
		[Mouse.eLClick] = function( playerID )
			local player = playerID and Players[playerID]
			local teamID = player:GetTeam()
			if player and playerID ~= g_leaderID and ( not g_leaderMode or UI.GetLeaderHeadRootUp() ) and not IsGameCoreBusy() then
				-- player
				if playerID == g_activePlayerID then
					if not g_leaderMode then
						Events.SerialEventGameMessagePopup{ Type = ButtonPopupTypes.BUTTONPOPUP_ADVISOR_COUNSEL, Data1 = 1 }
					end
				-- war declaration
				elseif bnw_mode and UI.CtrlKeyDown() and g_activeTeam:CanChangeWarPeace( teamID ) then
					if g_activeTeam:IsAtWar( teamID ) then
					-- Asking for Peace (currently at war) - bring up the trade screen
						player:DoUpdateWarDamageLevel(); -- UndeadDevel: since we're bypassing the default diplo screen, which would update these two things we need to do it manually here
						player:DoUpdatePeaceTreatyWillingness();
						Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_HUMAN_NEGOTIATE_PEACE, playerID, 0, 0 )
					elseif g_activeTeam:CanDeclareWar( teamID ) then
					-- Declaring War - bring up the BNW popup
						UI.AddPopup{ Type = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARMOVE, Data1 = teamID, Option1 = true }
					end
				-- human player
				elseif player:IsHuman() then
					Events.OpenPlayerDealScreenEvent( playerID )

				-- city state
				elseif player:IsMinorCiv() then
					Events.SerialEventGameMessagePopup{ Type = ButtonPopupTypes.BUTTONPOPUP_CITY_STATE_DIPLO, Data1 = playerID }

				-- AI player
				elseif not player:IsBarbarian() then
					UI.SetRepeatActionPlayer( playerID )
					UI.ChangeStartDiploRepeatCount( 1 )
					player:DoBeginDiploWithHuman()
				end
				if g_leaderMode then
					g_deal:ClearItems()
					UIManager:DequeuePopup( g_LeaderPopups[ g_leaderMode ] )
					UI.SetLeaderHeadRootUp( false )
					UI.RequestLeaveLeader()
				end
			end
		end;
		
		[Mouse.eRClick] = function( playerID )
			local player = Players[ playerID ]
			if player then
				if player:IsMinorCiv() then
					local city = player:GetCapitalCity()
					local plot = city and city:Plot()
					if plot and not g_leaderMode then
						UI.LookAt(plot, 0)
					end
				else
					Events.SearchForPediaEntry( player:GetCivilizationShortDescription() )
				end
			end
		end;
		[Mouse.eMouseEnter] = nil;
		[Mouse.eMouseExit] = nil;
	},--/Button

	Spy = {
		[Mouse.eLClick] = function()
			Events.SerialEventGameMessagePopup{ Type = ButtonPopupTypes.BUTTONPOPUP_ESPIONAGE_OVERVIEW }
		end;
	},--/Spy

	Quests = {
		[Mouse.eLClick] = function( minorPlayerID )
			local minorPlayer = Players[ minorPlayerID ]
			if minorPlayer and not g_leaderMode then
				if isQuestKillCamp( minorPlayer ) then
					local plot = Map_GetPlot( minorPlayer:GetQuestData1( g_activePlayerID, questKillCamp ),
								  minorPlayer:GetQuestData2( g_activePlayerID, questKillCamp ) )
					if plot then
						UI.LookAt( plot, 0 )
						local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
						Events.GameplayFX( hex.x, hex.y, -1 )
					end
				end
-- CBP
				if isQuestDig( minorPlayer ) then
					local plot = Map.GetPlot( minorPlayer:GetQuestData1( g_activePlayerID, questDig ),
								  minorPlayer:GetQuestData2( g_activePlayerID, questDig ) )
					if plot then
						UI.LookAt( plot, 0 )
						local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
						Events.GameplayFX( hex.x, hex.y, -1 )
					end
				end
				if isQuestPlot( minorPlayer ) then
					local plot = Map.GetPlot( minorPlayer:GetQuestData1( g_activePlayerID, questPlot ),
								  minorPlayer:GetQuestData2( g_activePlayerID, questPlot ) )
					if plot then
						UI.LookAt( plot, 0 )
						local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
						Events.GameplayFX( hex.x, hex.y, -1 )
					end
				end
				if isQuestCity( minorPlayer ) then
					local plot = Map.GetPlot( minorPlayer:GetQuestData1( g_activePlayerID, questCity ),
								  minorPlayer:GetQuestData2( g_activePlayerID, questCity ) )
					if plot then
						UI.LookAt( plot, 0 )
						local hex = ToHexFromGrid{ x=plot:GetX(), y=plot:GetY() }
						Events.GameplayFX( hex.x, hex.y, -1 )
					end
				end
-- END
			end
		end;
	},--/Quests

	Connection = {
		[Mouse.eLClick] = function( playerID )
			local player = Players[playerID]
			if Matchmaking.IsHost()
				and playerID ~= g_activePlayerID
				and (Network.IsPlayerConnected(playerID) or (player:IsHuman() and not player:IsObserver()))
			then
				UIManager:PushModal( Controls.ConfirmKick, true )
				LuaEvents.SetKickPlayer( playerID, Players[playerID]:GetName() )
			end
		end;
	},--/Connection
}--g_civListInstanceCallBacks
-------------------------------------------------
-- Update the civ list
-------------------------------------------------
local function UpdateCivListNow()
	if IsGameCoreBusy() then
		return
	end
	g_isUpdateCivList = false
	ContextPtr:ClearUpdate()

	-- Find the Spies
	local spies = {}
	if gk_mode then
		for _, spy in ipairs( g_activePlayer:GetEspionageSpies() ) do
			local plot = Map_GetPlot( spy.CityX, spy.CityY )
			local city = plot and plot:GetPlotCity()
			local cityOwnerID = city and city:GetOwner()
			if cityOwnerID then
				spies[ cityOwnerID ] = true
			end
		end
	end
	-- Update the Majors

	for playerID, instance in pairs( g_majorControlTable ) do

		local player = Players[playerID]
		local teamID = player:GetTeam()
		local team = Teams[ teamID ]

		-- have we met ?

		if player:IsAlive()
			and g_activeTeam:IsHasMet( teamID )
		then
			if team:GetNumMembers() > 1 then
				instance.TeamIcon:SetText( "[ICON_TEAM_" .. team:GetID() + 1 .. "]" )
				instance.TeamIcon:SetHide( false )
				instance.CivIconBG:SetHide( true )
			else
				instance.TeamIcon:SetHide( true )
				instance.CivIconBG:SetHide( false )
			end

			-- Setup status flags

			local isAtWar = team:IsAtWar( g_activeTeamID )
			local isDoF = player:IsDoF( g_activePlayerID )
			--C4DF
			local isMasterOfOther = team:IsVassal(g_activeTeamID)
			local isVassalOfOther = Teams[g_activeTeamID]:IsVassal(teamID)
			--END
			local isActivePlayer = playerID == g_activePlayerID
			instance.War:SetHide( not isAtWar )
			if g_isNetworkMultiPlayer or g_isHotSeatGame then
				if g_isNetworkMultiPlayer then
					if Matchmaking.GetHostID() == playerID then
						instance.Connection:SetTextureOffsetVal(4,68)
					elseif Network.IsPlayerHotJoining(playerID) then
						instance.Connection:SetTextureOffsetVal(4,36)
					elseif player:IsConnected() then
						instance.Connection:SetTextureOffsetVal(4,4)
					else
						instance.Connection:SetTextureOffsetVal(4,100)
					end
				end
				if UI.ProposedDealExists( playerID, g_activePlayerID ) then
					--They proposed something to us
					instance.Diplomacy:SetHide(false)
					instance.Diplomacy:SetAlpha( 1.0 )
				elseif UI.ProposedDealExists( g_activePlayerID, playerID ) then
					-- We proposed something to them
					instance.Diplomacy:SetHide(false)
					instance.Diplomacy:SetAlpha( 0.5 )
				else
					instance.Diplomacy:SetHide(true)
				end
			end
			instance.ActivePlayer:SetHide( not isActivePlayer )
			instance.ResearchAgreement:SetHide( not team:IsHasResearchAgreement( g_activeTeamID ) )
			instance.DefenseAgreement:SetHide( not team:IsDefensivePact( g_activeTeamID ) )
			instance.DeclarationOfFriendship:SetHide( not isDoF )
			--C4DF
			instance.Master:SetHide(not isMasterOfOther)
			instance.Vassal:SetHide(not isVassalOfOther)
			--END
			local isOurBorderOpen = g_activeTeam:IsAllowsOpenBordersToTeam( teamID )
			local isTheirBorderOpen = team:IsAllowsOpenBordersToTeam( g_activeTeamID )
			instance.TheirBordersClosed:SetHide( isActivePlayer or isAtWar or isTheirBorderOpen )
			instance.OurBordersClosed:SetHide( isActivePlayer or isAtWar or isOurBorderOpen )
			instance.TheirBordersOpen:SetHide( isActivePlayer or not isTheirBorderOpen )
			instance.OurBordersOpen:SetHide( isActivePlayer or not isOurBorderOpen )

			local color
			if isAtWar then
				color = g_colorWar
			elseif player:IsDenouncingPlayer( g_activePlayerID ) then
				color = g_colorDenounce
			elseif player:IsHuman() or team:IsHuman() then
				color = g_colorHuman
			else
				color = g_colorMajorCivApproach[ g_activePlayer:GetApproachTowardsUsGuess( playerID ) ]
			end
			instance.Button:SetColor( color )

			-- Set Score
			instance.Score:SetText( player:GetScore() )

			local theirTradeItems, ourTradeItems = {}, {}

			if isActivePlayer then
				-- Resources we can trade
--[[ too much stuff
--				ourTradeItems[1] = tostring( not g_leaderMode or UI.GetLeaderHeadRootUp() )
				for resource in GameInfo.Resources() do
					for playerID = 0, GameDefines.MAX_MAJOR_CIVS-1 do
						local player = Players[playerID]
						if player
							and player:IsAlive()
							and g_activeTeam:IsHasMet( player:GetTeam() )
							and not g_activeTeam:IsAtWar( player:GetTeam() )
							and g_deal:IsPossibleToTradeItem( g_activePlayerID, playerID, TradeableItems.TRADE_ITEM_RESOURCES, resource.ID, 1 )
						then
							table_insert( ourTradeItems, resource.IconString )
							break
						end
					end
				end
--]]

			elseif isAtWar then
				instance.Gold:SetText( "[COLOR_RED]" .. L("TXT_KEY_DIPLO_MAJOR_CIV_DIPLO_STATE_WAR") .. "[/COLOR]" )
			else
				-- Gold available
				local gold = 0
				local goldRate = player:CalculateGoldRate()
				if g_deal:IsPossibleToTradeItem( playerID, g_activePlayerID, TradeableItems.TRADE_ITEM_GOLD, 1 ) then -- includes DoF check
					gold = player:GetGold()
					instance.Gold:SetText( "[COLOR_YELLOW]" .. gold .. "[/COLOR]" )	--[ICON_GOLD]
				else
					instance.Gold:SetText()
					if not g_deal:IsPossibleToTradeItem( playerID, g_activePlayerID, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1 ) then
						goldRate = 0
					end
				end
				local minKeepLuxuries = 1
				-- Reasonable trade is not possible if hostile
				if player:GetMajorCivApproach( g_activePlayerID ) ~= MajorCivApproachTypes.MAJOR_CIV_APPROACH_HOSTILE then --MAJOR_CIV_APPROACH_DECEPTIVE then
					-- Luxuries available from them
					for resource in GameInfo.Resources() do
						local resourceID = resource.ID
						if Game.GetResourceUsageType( resourceID ) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY
							and player:GetNumResourceAvailable( resourceID, true ) > 1 -- single resources (including imports) are too expensive (3x)
							and g_deal:IsPossibleToTradeItem( playerID, g_activePlayerID, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1 )
						then
							table_insert( theirTradeItems, resource.IconString )
							minKeepLuxuries = 0	-- if they have luxes to trade, we can trade even our last one
						end
					end
					local dealDuration = Game.GetDealDuration()
					if goldRate > 0 then
						gold = dealDuration * goldRate + gold
					end
					local canPayForLux = gold >= dealDuration * 5  or minKeepLuxuries == 0 -- they can pay for it or trade for another lux
					local canPayForStrat = gold >= dealDuration -- they can pay at least 1GPT for it
					local happyWithoutLux = minKeepLuxuries == 0 or g_activePlayer:GetExcessHappiness() > 4 + g_activePlayer:GetNumCities() -- approximation... should take actual happy value + scan city growth
					-- Resources available from us
					for resource in GameInfo.Resources() do
						local resourceID = resource.ID
						-- IsPossibleToTradeItem includes check on min quantity, banned luxes and obsolete strategics
						if g_deal:IsPossibleToTradeItem( g_activePlayerID, playerID, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1 ) then
							local usage = Game.GetResourceUsageType( resourceID )
							if usage == ResourceUsageTypes.RESOURCEUSAGE_LUXURY
								and canPayForLux
								and player:GetNumResourceAvailable( resourceID, true ) == 0 -- they do not already have or import
							then
								if g_activePlayer:GetNumResourceAvailable( resourceID, true ) > 1 then
									table_insert( ourTradeItems, 1, resource.IconString )
								elseif happyWithoutLux then
									table_insert( ourTradeItems, resource.IconString )
								end
							elseif usage == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC
								and canPayForStrat
								and player:GetNumResourceAvailable( resourceID, true ) <= player:GetNumCities() -- game limit on AI trading
								and (not resource.AIStopTradingEra or player:GetCurrentEra() < GameInfoTypes[resource.AIStopTradingEra]) -- not obsolete
							then
								table_insert( ourTradeItems, resource.IconString )
							end
						end
					end
				end
			end
			instance.TheirTradeItems:SetText( table_concat( theirTradeItems ) )
			if #ourTradeItems < 4 then
				instance.OurTradeItems:SetText( table_concat( ourTradeItems ) )
			else
				ourTradeItems[4] = "..." --"[ICON_PLUS]"
				instance.OurTradeItems:SetText( table_concat( ourTradeItems, nil, 1, 4 ) )
			end

			-- disable the button if we have a pending deal with this player
			instance.Button:SetDisabled( playerID == UI.HasMadeProposal( g_activePlayerID ) )

			instance.Button:SetHide( false )
			instance[3] = team:GetScore()
			instance[2] = player:GetScore()
		else
			instance.Button:SetHide( true )
		end
	end
	Controls.MajorStack:SortChildren( SortMajorStack )

	-- Show the CityStates we know

	local capital = g_activePlayer:GetCapitalCity()
	for minorPlayerID, instance in pairs( g_minorControlTable ) do

		local minorPlayer = Players[ minorPlayerID ]

		if minorPlayer
			and minorPlayer:IsAlive()
			and g_activeTeam:IsHasMet( minorPlayer:GetTeam() )
		then
			instance.Button:SetHide( false )

			-- Update Background
			UpdateCityStateStatusIconBG( g_activePlayerID, minorPlayerID, instance.Portrait )

			-- Update Allies
			local allyID = minorPlayer:GetAlly()
			local ally = Players[ allyID ]

			if ally then
				CivIconHookup( g_activeTeam:IsHasMet( ally:GetTeam() ) and allyID or -1, 32, instance.AllyIcon, instance.AllyBG, instance.AllyShadow, false, true )
				instance.Ally:SetHide(false)
			else
				instance.Ally:SetHide(true)
			end

			-- Update Spies
			instance.Spy:SetHide( not spies[ minorPlayerID ] )

			-- Update Quests
			instance.Quests:SetText( GetActiveQuestText( g_activePlayerID, minorPlayerID ) )

			-- Update Pledge
			if gk_mode then
				local pledge = g_activePlayer:IsProtectingMinor( minorPlayerID )
				local free = pledge and minorPlayer:CanMajorWithdrawProtection( g_activePlayerID )
				instance.Pledge1:SetHide( not pledge or free )
				instance.Pledge2:SetHide( not free )
			end
			instance[3] = minorPlayer:GetMinorCivFriendshipWithMajor( g_activePlayerID )
			local minorCapital = minorPlayer:GetCapitalCity()
			instance[2] = -(capital and minorCapital and Map_PlotDistance( capital:GetX(), capital:GetY(), minorCapital:GetX(), minorCapital:GetY() ) or math_huge)
		else
			instance.Button:SetHide( true )
		end
	end
	Controls.MinorStack:SortChildren( SortMinorStack )

	ProcessStackSizes()
end
local function UpdateCivList()
	if g_isShowCivList then
		if g_leaderMode then
			UpdateCivListNow()
		elseif not g_isUpdateCivList then
			g_isUpdateCivList = true
			ContextPtr:SetUpdate( UpdateCivListNow )
		end
	end
end
----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
local function OnSetActivePlayer()	--activePlayerID, prevActivePlayerID )
	-- update globals

	g_activePlayerID = Game.GetActivePlayer()
	g_activePlayer = Players[ g_activePlayerID ]
	g_activeTeamID = g_activePlayer:GetTeam()
	g_activeTeam = Teams[ g_activeTeamID ]

	-- Remove all the UI notifications. The new player will rebroadcast any persistent ones from their last turn
	for Id in pairs( g_ActiveNotifications ) do
		RemoveNotificationID( Id )
	end
	UI.RebroadcastNotifications()

	-- update the civ list
	UpdateCivList()
end

-------------------------------------------------
-- Civ List Init
-------------------------------------------------

for playerID = 0, GameDefines.MAX_CIV_PLAYERS-1 do

	local player = Players[ playerID ]
	if player and (player:IsEverAlive() or player:IsPotentiallyAlive()) then
		--print( "Setting up civilization ribbon player ID", playerID )
		local instance = { -playerID, 0, 0, 0 }

		if player:IsMinorCiv() then

			-- Create instance
			ContextPtr:BuildInstanceForControl( "CityStateInstance", instance, Controls.MinorStack )
			g_minorControlTable[playerID] = instance

			-- Setup icons
			instance.StatusIcon:SetTexture(GameInfo.MinorCivTraits[GameInfo.MinorCivilizations[player:GetMinorCivType()].MinorCivTrait].TraitIcon)
			instance.StatusIcon:SetColor( PrimaryColors[playerID] )
		else

			-- Create instance
			ContextPtr:BuildInstanceForControl( "LeaderButtonInstance", instance, Controls.MajorStack )
			g_majorControlTable[playerID] = instance

			-- Setup icons
			local leader = GameInfo.Leaders[player:GetLeaderType()]
			IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, instance.Portrait )
			CivIconHookup( playerID, 32, instance.CivIcon, instance.CivIconBG, instance.CivIconShadow, false, true )
			instance.Connection:SetHide( not g_isNetworkMultiPlayer )
			instance.Diplomacy:SetHide( not g_isNetworkMultiPlayer and not g_isHotSeatGame )
		end

		local control
		-- Setup Tootips
		for name, callback in pairs( g_civListInstanceToolTips ) do
			control = instance[name]
			if control then
				control:SetToolTipCallback( callback )
				control:SetToolTipType( "EUI_CivilizationTooltip" )
			end
		end
		-- Setup Callbacks
		for name, eventCallbacks in pairs( g_civListInstanceCallBacks ) do
			control = instance[name]
			if control then
				for event, callback in pairs( eventCallbacks ) do
					control:SetVoid1( playerID )
					control:RegisterCallback( event, callback )
				end
			end
		end
	end
end

local function OnChatToggle( isChatOpen )
	g_civPanelOffsetY = g_diploButtonsHeight + (isChatOpen and g_chatPanelHeight or 0)
	g_maxTotalStackHeight = g_screenHeight - Controls.OuterStack:GetOffsetY() - g_civPanelOffsetY
	if not g_leaderMode then
		Controls.CivPanel:SetOffsetY( g_civPanelOffsetY )
		ProcessStackSizes( true )
	end
end

local EUI_options = Modding.OpenUserData( "Enhanced User Interface Options", 1);

local function OnOptionsChanged()
	g_isShowCivList = EUI_options.GetValue( "CivRibbon" ) ~= 0
	Controls.CivPanel:SetHide( not g_isShowCivList )
	UpdateCivList()
end

local diploChatPanel = LookUpControl( "/InGame/WorldView/DiploCorner/ChatPanel" )
if diploChatPanel then
	g_chatPanelHeight = diploChatPanel:GetSizeY()
end

local diploButtons = {}
local predefined = {
	[L"TXT_KEY_ADVISOR_COUNSEL"] = "",-- "DC45_AdvisorCounsel.dds",
	[L"TXT_KEY_ADVISOR_SCREEN_TECH_TREE_DISPLAY"] = "",-- "DC45_TechTree.dds",
	[L"TXT_KEY_DIPLOMACY_OVERVIEW"] = "",--"DC45_DiplomacyOverview.dds",
	[L"TXT_KEY_MILITARY_OVERVIEW"] = "DC45_MilitaryOverview.dds",--{ "MainUnitButton.dds", "MainUnitButtonHL.dds" },--
	[L"TXT_KEY_ECONOMIC_OVERVIEW"] = "",-- "DC45_EconomicOverview.dds",
	[L"TXT_KEY_VP_TT"] = "DC45_VictoryProgress.dds",
	[L"TXT_KEY_DEMOGRAPHICS"] = "DC45_Demographics.dds",
	[L"TXT_KEY_POP_NOTIFICATION_LOG"] = "DC45_NotificationLog.dds",
	[L"TXT_KEY_TRADE_ROUTE_OVERVIEW"] = "",-- "DC45_TradeRouteOverview.dds",
	[L"TXT_KEY_EO_TITLE"] = "",--"DC45_EspionageOverview.dds",
	[L"TXT_KEY_RELIGION_OVERVIEW"] = "",--"DC45_ReligionOverview.dds",
	[L"TXT_KEY_LEAGUE_OVERVIEW"] = "DC45_WorldCongress.dds",
	[L"TXT_KEY_INFOADDICT_MAIN_TITLE"] = "DC45_InfoAddict.dds",
	[L"TXT_KEY_C4DF_VASSAL_OVERVIEW"] = "DC45_VassalOverview.dds",
	[L"TXT_KEY_EVENT_OVERVIEW"] = "DC45_EventsOverview.dds",
	[L"TXT_KEY_PROMO_GROUP_BASE"] = "DC45_UnitPromotions.dds",
	[L"TXT_KEY_CPO"] = "DC45_Corporations.dds",
}

LuaEvents.AdditionalInformationDropdownGatherEntries.Add(
function( diploButtonEntries )
	local diploButtonStack = LookUpControl( "/InGame/WorldView/DiploCorner/DiploCornerStack" )
	if diploButtonStack then
		local n, instance = 1
		local c = LookUpControl( "/InGame/WorldView/DiploCorner/CultureOverviewButton" )
		if c then
			c:SetHide( not bnw_mode or Game.IsOption("GAMEOPTION_NO_CULTURE_OVERVIEW_UI") )
		end

		local DiploCorner = LookUpControl( "/InGame/WorldView/DiploCorner" )
		if DiploCorner then
			for _, v in ipairs( diploButtonEntries ) do
				local t = v.art or predefined[ v.text ]
				if not t or #t > 0 then
					if n>#diploButtons then
						instance={}
						DiploCorner:BuildInstanceForControl( "DiploCornerButton", instance, diploButtonStack )
						diploButtons[n] = instance
					else
						instance = diploButtons[n]
						instance.Button:SetHide( false )
					end
					n = n+1
					if t then
						instance.Button:SetTexture( t )
						if t == "DC45_NotificationLog.dds" then
							g_alertButton = instance.Button
							g_alertMessages = { v.text }
						end
					end
					instance.Button:SetText( not t and v.text:sub(1,3) )
					instance.Button:RegisterCallback( Mouse.eLClick, v.call )
					instance.Button:SetToolTipString( v.text )
				end
			end
			for i = n, #diploButtons do
				diploButtons[i].Button:SetHide( true )
			end
			diploButtonStack:SortChildren(
			function(a,b)
				return (a:GetToolTipString() or "") > (b:GetToolTipString() or "")
			end)
		else
			print("Error: could not find DiploCorner lua context, probably a mod conflict")
		end
		g_diploButtonsHeight = 28 + diploButtonStack:GetSizeY()
	else
		print("Error: could not find DiploCorner button stack, probably a mod conflict")
	end
end)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries()

if g_alertButton and g_alertButton.SetToolTipString then
	Events.GameplayAlertMessage.Add(
	function( messageText )
		table_insert( g_alertMessages, messageText )
		g_alertButton:SetToolTipString( table_concat(g_alertMessages,"[NEWLINE]") )
	end)
end

OnChatToggle( PreGame.IsMultiplayerGame() )
OnOptionsChanged()
OnSetActivePlayer()
Events.GameOptionsChanged.Add( OnOptionsChanged )
Events.GameplaySetActivePlayer.Add( OnSetActivePlayer )
LuaEvents.ChatShow.Add( OnChatToggle )
Events.SerialEventGameDataDirty.Add( UpdateCivList )
Events.SerialEventScoreDirty.Add( UpdateCivList )
Events.SerialEventCityInfoDirty.Add( UpdateCivList )
Events.SerialEventImprovementCreated.Add( UpdateCivList )	-- required to update trades when a resource gets hooked up
Events.WarStateChanged.Add( UpdateCivList )			-- update when war is declared
Events.MultiplayerGamePlayerDisconnected.Add( UpdateCivList )
Events.MultiplayerGamePlayerUpdated.Add( UpdateCivList )
Events.MultiplayerHotJoinStarted.Add(
function()
	Controls.HotJoinNotice:SetHide(false)
	return UpdateCivList()
end )
Events.MultiplayerHotJoinCompleted.Add(
function()
	Controls.HotJoinNotice:SetHide(true)
	return UpdateCivList()
end )
local function HighlightAndUpdateCivList( playerID, isActive )
	local instance = g_majorControlTable[ playerID ]
	if instance then
		instance.Portrait:ChangeParent( isActive and instance.ActiveTurn or instance.NotYourTurn )
	else
		instance = g_minorControlTable[ playerID ]
		if instance then
			instance.Active:SetHide( not isActive )
		end
	end
	return UpdateCivList()
end
Events.RemotePlayerTurnStart.Add( function() return HighlightAndUpdateCivList( g_activePlayerID, true ) end)
Events.RemotePlayerTurnEnd.Add( function() return HighlightAndUpdateCivList( g_activePlayerID, false ) end)
Events.ActivePlayerTurnStart.Add( function() return HighlightAndUpdateCivList( g_activePlayerID, true ) end)
Events.ActivePlayerTurnEnd.Add( function() g_alertMessages = {g_alertMessages[1]} return HighlightAndUpdateCivList( g_activePlayerID, false ) end)
Events.AIProcessingStartedForPlayer.Add( function( playerID ) return HighlightAndUpdateCivList( playerID, true ) end)
Events.AIProcessingEndedForPlayer.Add( function( playerID ) return HighlightAndUpdateCivList( playerID, false ) end)

g_LeaderPopups = { LookUpControl( "/LeaderHeadRoot" ), LookUpControl( "/LeaderHeadRoot/DiploTrade" ), LookUpControl( "/LeaderHeadRoot/DiscussionDialog" ) }
if #g_LeaderPopups > 0 then
	Events.LeavingLeaderViewMode.Add(
	function()
--print("LeavingLeaderViewMode event in leader mode", g_leaderMode )
		if g_leaderMode then
			g_leaderMode = false
--				g_isLeaderLock = false
			g_leaderID = false
			Controls.CivPanel:SetOffsetY( g_civPanelOffsetY )
			Controls.CivPanel:ChangeParent( ContextPtr )
		end
		UpdateCivList()
	end)
	Events.AILeaderMessage.Add(
	function( playerID ) --, diploUIStateID, leaderMessage, animationAction, data1 )
--local d = "?"; for k,v in pairs( DiploUIStateTypes ) do if v == diploUIStateID then d = k break end end
--print("AILeaderMessage event", Players[playerID]:GetCivilizationShortDescription(), diploUIStateID, d, "during my turn", g_activePlayer:IsTurnActive(), "IsGameCoreBusy", IsGameCoreBusy() )
		g_leaderID = playerID
		for i=1, #g_LeaderPopups do
			if not g_LeaderPopups[i]:IsHidden() then
				if i ~= g_leaderMode then
					g_leaderMode = i
					Controls.CivPanel:ChangeParent( g_LeaderPopups[i] )
					Controls.CivPanel:SetOffsetY( 0 )
					UpdateCivList()
--print("enter leader mode", g_LeaderPopups[i]:GetID() )
				end
				break
			end
		end
		if g_leaderMode and g_isUpdateCivList then
			UpdateCivListNow()
		end
	end)
	LuaEvents.EUILeaderHeadRoot.Add(
	function()
		if not g_leaderMode or g_leaderMode>1 then
			g_leaderMode = 1
			Controls.CivPanel:ChangeParent( g_LeaderPopups[1] )
			Controls.CivPanel:SetOffsetY( 0 )
			UpdateCivList()
		end
--print("enter leader mode", 1 )
	end)
end

print("Finished loading EUI notification panel",os.clock())
end)

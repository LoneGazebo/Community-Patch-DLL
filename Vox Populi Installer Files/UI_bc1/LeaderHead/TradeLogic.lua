-- modified by bc1 from 1.0.3.276 brave new world code
-- code is common using gk_mode and bnw_mode switches
-- show missing cash for trade agreements
local civ5_mode = InStrategicView ~= nil
local bnw_mode = not civ5_mode or Game.GetActiveLeague ~= nil
local gk_mode = bnw_mode or Game.GetReligionName ~= nil

local ipairs = ipairs
local math_huge = math.huge
local math_min = math.min
local pairs = pairs
local print = print
local table_insert = table.insert
local tonumber = tonumber

if not civ5_mode then --no glass panel in civBE
	Controls.UsGlass = Controls.UsPanel
	Controls.ThemGlass = Controls.ThemPanel
end
----------------------------------------------------------------
----------------------------------------------------------------
include( "IconSupport" ) local IconHookup = IconHookup local CivIconHookup = CivIconHookup
include( "InstanceManager" ) local GenerationalInstanceManager = GenerationalInstanceManager
include( "SupportFunctions" ) local TruncateString = TruncateString

local g_UsTableCitiesIM		= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.UsTableCitiesStack )
local g_UsPocketCitiesIM	= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.UsPocketCitiesStack )
local g_ThemTableCitiesIM	= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.ThemTableCitiesStack )
local g_ThemPocketCitiesIM	= GenerationalInstanceManager:new( "CityInstance", "Button", Controls.ThemPocketCitiesStack )
local g_UsTableVoteIM, g_UsPocketVoteIM, g_ThemTableVoteIM, g_ThemPocketVoteIM
if bnw_mode then
	g_UsTableVoteIM		= GenerationalInstanceManager:new( "TableVote", "Container", Controls.UsTableVoteStack )
	g_UsPocketVoteIM	= GenerationalInstanceManager:new( "PocketVote", "Button", Controls.UsPocketVoteStack )
	g_ThemTableVoteIM	= GenerationalInstanceManager:new( "TableVote", "Container", Controls.ThemTableVoteStack )
	g_ThemPocketVoteIM	= GenerationalInstanceManager:new( "PocketVote", "Button", Controls.ThemPocketVoteStack )
end
local g_bAlwaysWar		= Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR )
local g_bAlwaysPeace		= Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE )
local g_bNoChangeWar		= Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE )

----------------------------------------------------------------
-- local storage
----------------------------------------------------------------
local g_Deal = UI.GetScratchDeal()
local g_diploUIStateID
local g_PVPTrade
local g_bTradeReview = false
local g_iNumOthers
local g_OfferIsFixed = false	-- Is the human allowed to change the offer?  Set to true when the AI is demanding or making a request
local g_bEnableThirdParty = true

local g_iConcessionsPreviousDiploUIState = -1
local g_bHumanOfferingConcessions = false

local g_iDealDuration = Game.GetDealDuration()
local g_iPeaceDuration = gk_mode and Game.GetPeaceDuration() or GameDefines.PEACE_TREATY_LENGTH or 10

local g_iUs = -1 --Game.GetActivePlayer()
local g_iThem = -1
local g_pUs
local g_pThem
local g_iUsTeam = -1
local g_iThemTeam = -1
local g_pUsTeam
local g_pThemTeam
local g_UsPocketResources = {}
local g_ThemPocketResources = {}
local g_UsTableResources = {}
local g_ThemTableResources = {}
local g_TableLeaderButtons = {}
local g_PocketLeaderButtons = {}
local g_LeagueVoteList = {}

local g_bMessageFromDiploAI = false
local g_bAIMakingOffer = false

local g_OtherPlayersButtons = {}

local offsetOfString = 32
local bonusPadding = 16
local innerFrameWidth = 524
local outerFrameWidth = 520
local offsetsBetweenFrames = 4

local oldCursor = 0
local DiploUIStateTypes = DiploUIStateTypes

local g_usPocketPeaceButtons, g_usPocketWarButtons, g_themPocketPeaceButtons, g_themPocketWarButtons, g_usTablePeaceButtons, g_usTableWarButtons, g_themTablePeaceButtons, g_themTableWarButtons

local g_borderTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_OPEN_BORDERS_TT", g_iDealDuration )
local g_embassyTooltip = Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_TT", g_iDealDuration)
local g_defensivePactToolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_TT", g_iDealDuration )

local g_SubStacks = {
	UsPocketOtherPlayer = Controls.UsPocketOtherPlayerStack,
	ThemPocketOtherPlayer = Controls.ThemPocketOtherPlayerStack,
	UsPocketLeaderClose = Controls.UsPocketLeaderStack,
	ThemPocketLeaderClose = Controls.ThemPocketLeaderStack,

	UsPocketStrategic = Controls.UsPocketStrategicStack,
	ThemPocketStrategic = Controls.ThemPocketStrategicStack,
	UsPocketLuxury = Controls.UsPocketLuxuryStack,
	ThemPocketLuxury = Controls.ThemPocketLuxuryStack,
	UsPocketCities = Controls.UsPocketCitiesStack,
	ThemPocketCities = Controls.ThemPocketCitiesStack,

	UsPocketVote = Controls.UsPocketVoteStack,
	ThemPocketVote = Controls.ThemPocketVoteStack,
}

local g_SubLabels = {
	UsPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
	ThemPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
	UsPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",
	ThemPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",

	UsPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_STRATEGIC_RESOURCES",
	ThemPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_STRATEGIC_RESOURCES",
	UsPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES",
	ThemPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_ITEMS_LUXURY_RESOURCES",
	UsPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_CITIES",
	ThemPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_CITIES",

	UsPocketVote = Locale.ConvertTextKey"TXT_KEY_TRADE_ITEM_VOTES",
	ThemPocketVote = Locale.ConvertTextKey"TXT_KEY_TRADE_ITEM_VOTES",
}

local g_SubTooltipsYes = {
--	UsPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
--	ThemPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_MAKE_PEACE_WITH",
--	UsPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",
--	ThemPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_DECLARE_WAR_ON",

	UsPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES",
	ThemPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_YES_THEM",
	UsPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES",
	ThemPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_YES_THEM",
	UsPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_TT",
	ThemPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_TT",

	UsPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_YES",
	ThemPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_YES_THEM",
}

local g_SubTooltipsNo = {
--	UsPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS",
--	ThemPocketOtherPlayer = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM",
--	UsPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS",
--	ThemPocketLeaderClose = Locale.ConvertTextKey"TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM",

--	UsPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO",
--	ThemPocketStrategic = Locale.ConvertTextKey"TXT_KEY_DIPLO_STRAT_RESCR_TRADE_NO_THEM",
--	UsPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO",
--	ThemPocketLuxury = Locale.ConvertTextKey"TXT_KEY_DIPLO_LUX_RESCR_TRADE_NO_THEM",
--	UsPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT",
--	ThemPocketCities = Locale.ConvertTextKey"TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT",

	UsPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_NO",
	ThemPocketVote = Locale.ConvertTextKey"TXT_KEY_DIPLO_VOTE_TRADE_NO_THEM",
}
for k, v in pairs(g_SubTooltipsNo) do g_SubTooltipsNo[k] = "[COLOR_WARNING_TEXT]" .. v .. "[ENDCOLOR]" end

local g_itemControls = {
[ TradeableItems.TRADE_ITEM_PEACE_TREATY or false ] = { Controls.UsPocketPeaceTreaty, Controls.UsTablePeaceTreaty, Controls.ThemPocketPeaceTreaty, Controls.ThemTablePeaceTreaty },
[ TradeableItems.TRADE_ITEM_ALLOW_EMBASSY or false ] = { Controls.UsPocketAllowEmbassy, Controls.UsTableAllowEmbassy, Controls.ThemPocketAllowEmbassy, Controls.ThemTableAllowEmbassy },
[ TradeableItems.TRADE_ITEM_OPEN_BORDERS or false ] = { Controls.UsPocketOpenBorders, Controls.UsTableOpenBorders, Controls.ThemPocketOpenBorders, Controls.ThemTableOpenBorders },
[ TradeableItems.TRADE_ITEM_DEFENSIVE_PACT or false ] = { Controls.UsPocketDefensivePact, Controls.UsTableDefensivePact, Controls.ThemPocketDefensivePact, Controls.ThemTableDefensivePact },
[ TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT or false ] = { Controls.UsPocketResearchAgreement, Controls.UsTableResearchAgreement, Controls.ThemPocketResearchAgreement, Controls.ThemTableResearchAgreement },
[ TradeableItems.TRADE_ITEM_TRADE_AGREEMENT or false ] = { Controls.UsPocketTradeAgreement, Controls.UsTableTradeAgreement, Controls.ThemPocketTradeAgreement, Controls.ThemTableTradeAgreement },
[ TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP or false ] = { Controls.UsPocketDoF, Controls.UsTableDoF, Controls.ThemPocketDoF, Controls.ThemTableDoF, Controls.ThemTableDoF },
}
g_itemControls[ false ] = nil

local g_tableControls = {
	Controls.UsTableVoteStack,
	Controls.ThemTableVoteStack,
	Controls.UsTableGold,
	Controls.ThemTableGold,
	Controls.UsTableGoldPerTurn,
	Controls.ThemTableGoldPerTurn,
	Controls.UsTableStrategicStack,
	Controls.ThemTableStrategicStack,
	Controls.UsTableLuxuryStack,
	Controls.ThemTableLuxuryStack,
	Controls.UsTableMakePeaceStack,
	Controls.ThemTableMakePeaceStack,
	Controls.UsTableDeclareWarStack,
	Controls.ThemTableDeclareWarStack,
}
local g_stacks = {
	Controls.UsPocketStrategicStack,
	Controls.ThemPocketStrategicStack,
	Controls.UsTableStrategicStack,
	Controls.ThemTableStrategicStack,
	Controls.UsPocketLuxuryStack,
	Controls.ThemPocketLuxuryStack,
	Controls.UsTableLuxuryStack,
	Controls.ThemTableLuxuryStack,
	Controls.UsPocketVoteStack,
	Controls.ThemPocketVoteStack,
	Controls.UsTableVoteStack,
	Controls.ThemTableVoteStack,
	Controls.UsTableCitiesStack,
	Controls.ThemTableCitiesStack,
	Controls.UsPocketLeaderStack,
	Controls.ThemPocketLeaderStack,
	Controls.UsTableMakePeaceStack,
	Controls.UsTableDeclareWarStack,
	Controls.ThemTableMakePeaceStack,
	Controls.ThemTableDeclareWarStack,
	Controls.UsPocketStack,
	Controls.ThemPocketStack,
	Controls.UsTableStack,
	Controls.ThemTableStack,
}
g_panels = {
	Controls.UsPocketPanel,
	Controls.ThemPocketPanel,
	Controls.UsTablePanel,
	Controls.ThemTablePanel,
}

Controls.UsPocketOtherPlayer:SetVoid1( 1 )
Controls.UsPocketLeaderClose:SetVoid1( 1 )
Controls.UsPocketStrategic:SetVoid1( 1 )
Controls.UsPocketLuxury:SetVoid1( 1 )

Controls.UsPocketCities:SetVoid1( 1 )

Controls.UsPocketOtherPlayerStack:SetHide( true )
Controls.ThemPocketOtherPlayerStack:SetHide( true )
Controls.UsPocketLeaderStack:SetHide( true )
Controls.ThemPocketLeaderStack:SetHide( true )
Controls.UsPocketCitiesStack:SetHide( true )
Controls.ThemPocketCitiesStack:SetHide( true )

local function SetEnabled( control, isEnabled )
	control:SetDisabled( not isEnabled )
	control:GetTextControl():SetColorByName( isEnabled and "Beige_Black" or "Gray_Black" )
end
local function SetEnabledAndToolTip( control, isEnabled, toolTipString )
	SetEnabled( control, isEnabled )
	control:SetToolTipString( toolTipString )
end
local function SetEnabledAndTextAndToolTip( control, isEnabled, text, toolTipString )
	control:SetText( text )
	SetEnabledAndToolTip( control, isEnabled, toolTipString )
end
local function SetEnabledAndText( control, isEnabled, text )
	control:SetText( text )
	SetEnabled( control, isEnabled )
end

function ResizeStacks()
	for k, stack in pairs( g_stacks ) do
		stack:CalculateSize()
	end
	for k, panel in pairs( g_panels ) do
		panel:CalculateInternalSize()
		panel:ReprocessAnchoring()
	end
end
local ResizeStacks = ResizeStacks

---------------------------------------------------------
-- stacks management
---------------------------------------------------------
-- hack existing XML
Controls.UsPocketCitiesStack:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketCitiesStack:SetOffsetVal( 0, 0 )
Controls.UsPocketCitiesClose:SetHide( true )
Controls.UsPocketOtherPlayer:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketOtherPlayerStack:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketLeaderClose:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketLeaderStack:ChangeParent( Controls.UsPocketStack )
Controls.UsPocketLeaderStack:SetOffsetVal( 0, 0 )
Controls.UsPocketOtherPlayerPeace:SetHide( true ) -- UsPocketOtherPlayer
Controls.UsPocketOtherPlayerWar:SetHide( true ) -- UsPocketLeaderClose

Controls.ThemPocketCitiesStack:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketCitiesStack:SetOffsetVal( 0, 0 )
Controls.ThemPocketCitiesClose:SetHide( true )
Controls.ThemPocketOtherPlayer:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketOtherPlayerStack:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketLeaderClose:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketLeaderStack:ChangeParent( Controls.ThemPocketStack )
Controls.ThemPocketLeaderStack:SetOffsetVal( 0, 0 )
Controls.ThemPocketOtherPlayerPeace:SetHide( true ) -- ThemPocketOtherPlayer
Controls.ThemPocketOtherPlayerWar:SetHide( true ) -- ThemPocketLeaderClose

local function SubStackToggle( button, isToggle )
	local buttonID = button:GetID()
	local stack = g_SubStacks[ buttonID ]
	stack:CalculateSize()
	if stack:GetSizeY() == 0 then
		local tip = g_SubTooltipsNo[ buttonID ]
		if tip then
			button:SetDisabled( true )
			button:GetTextControl():SetColorByName("Gray_Black")
			button:SetToolTipString( g_SubTooltipsNo[ buttonID ] )
			button:SetText( g_SubLabels[ buttonID ] )
		else
			button:SetHide( true )
		end
	else
		button:SetDisabled( false )
		button:SetHide( false )
		button:GetTextControl():SetColorByName("Beige_Black")
		button:SetToolTipString( g_SubTooltipsYes[ buttonID ] )
		if isToggle == stack:IsHidden() then
			stack:SetHide( false )
			button:SetText( "[ICON_MINUS]" .. g_SubLabels[ buttonID ] )
		else
			stack:SetHide( true )
			button:SetText( "[ICON_PLUS]" .. g_SubLabels[ buttonID ] )
		end
	end
end

local function SubStackHandler( bIsUs, none, control )
	SubStackToggle( control, true )

	if bIsUs == 1 then
		Controls.UsPocketStack:CalculateSize()
		Controls.UsPocketPanel:CalculateInternalSize()
		Controls.UsPocketPanel:ReprocessAnchoring()
	else
		Controls.ThemPocketStack:CalculateSize()
		Controls.ThemPocketPanel:CalculateInternalSize()
		Controls.ThemPocketPanel:ReprocessAnchoring()
	end
end
for k,v in pairs( g_SubStacks ) do
	Controls[ k ]:RegisterCallback( Mouse.eLClick, SubStackHandler )
end

if bnw_mode then
	Controls.UsPocketVote:SetVoid1( 1 )
	Controls.UsPocketVoteStack:SetHide( true )
	Controls.ThemPocketVoteStack:SetHide( true )

	if not Game.IsOption("GAMEOPTION_NO_LEAGUES") then
		local function VoteStackHandler( bIsUs, ... )
			RefreshPocketVotes( bIsUs )
			SubStackHandler( bIsUs, ... )
		end
		Controls.ThemPocketVote:RegisterCallback( Mouse.eLClick, VoteStackHandler )
		Controls.UsPocketVote:RegisterCallback( Mouse.eLClick, VoteStackHandler )
	end
end


---------------------------------------------------------
-- Clear all items off the table (both UI and CvDeal)
---------------------------------------------------------

function DoClearTable()
	g_UsTableCitiesIM:ResetInstances()
	g_ThemTableCitiesIM:ResetInstances()
	if bnw_mode then
		g_UsTableVoteIM:ResetInstances()
		g_ThemTableVoteIM:ResetInstances()
	end
	for k, control in pairs( g_tableControls ) do
		control:SetHide( true )
	end
	ResizeStacks()
end
local DoClearTable = DoClearTable

function DoClearDeal()

--print("Clearing Table")
	g_Deal:ClearItems()
	DoClearTable()
end
local DoClearDeal = DoClearDeal
Events.ClearDiplomacyTradeTable.Add( DoClearDeal )

---------------------------------------------------------
-- Update buttons at the bottom
---------------------------------------------------------
function DoUpdateButtons( diploMessage )

	if g_bTradeReview then
	elseif g_PVPTrade then -- Dealing with a human in a MP game
		--print( "PVP Updating ProposeButton" )
		local propose
		if g_PVPTrade == 2 then -- this is their proposal
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_ACCEPT" )
			Controls.CancelButton:SetHide( false )
			Controls.ModifyButton:SetHide( false )
			Controls.Pockets:SetHide( true )
			Controls.ModificationBlock:SetHide( false )

		elseif g_PVPTrade == 1 then -- this is our proposal
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_WITHDRAW" )
			Controls.CancelButton:SetHide( true )
			Controls.ModifyButton:SetHide( true )
			Controls.Pockets:SetHide( true )
			Controls.ModificationBlock:SetHide( false )

		else -- this is a new deal
			propose = g_Deal:GetNumItems() == 0
			Controls.ProposeButton:LocalizeAndSetText( propose and "TXT_KEY_DIPLO_DECLARE_WAR" or "TXT_KEY_DIPLO_PROPOSE" )
			propose = propose and not ( bnw_mode and g_pUsTeam:CanChangeWarPeace( g_iThemTeam ) and not g_pUsTeam:IsAtWar( g_iThemTeam ) and g_pUsTeam:CanDeclareWar( g_iThemTeam ) )
			Controls.CancelButton:SetHide( true )
			Controls.ModifyButton:SetHide( true )
			Controls.Pockets:SetHide( false )
			Controls.ModificationBlock:SetHide( true )

		end
		Controls.ProposeButton:SetHide( propose )
		Controls.MainStack:CalculateSize()
		Controls.MainGrid:DoAutoSize()

	else -- Dealing with an AI

		if diploMessage then
			-- Resize the height of the box to fit the text
			Controls.DiscussionText:SetText( diploMessage )
			local contentSize = Controls.DiscussionText:GetSize().y + offsetOfString + bonusPadding
			Controls.LeaderSpeechBorderFrame:SetSizeVal( innerFrameWidth, contentSize )
			Controls.LeaderSpeechFrame:SetSizeVal( outerFrameWidth, contentSize - offsetsBetweenFrames )
		end
		local numItemsFromUs = 0
		local numItemsFromThem = 0
		local isAtWar = g_pUsTeam:IsAtWar( g_iThemTeam )
		local isAIRequestingConcessions = not isAtWar and UI.IsAIRequestingConcessions()

		-- Table is empty
		if g_Deal:GetNumItems() == 0 then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_GOODBYE_BUTTON" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_CANCEL" )

		-- Human is making a demand
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_DEMAND_BUTTON" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_CANCEL" )

		-- AI is making a demand or Request, or the AI made an offer for the human to respond with
		elseif isAtWar or g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER
			or g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND
			or g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST
		then
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_ACCEPT" )
			Controls.CancelButton:LocalizeAndSetText( "TXT_KEY_DIPLO_REFUSE" )

		else
			Controls.ProposeButton:LocalizeAndSetText( "TXT_KEY_DIPLO_PROPOSE" )
			Controls.CancelButton:LocalizeAndSetText( bnw_mode and "TXT_KEY_CLEAR_BUTTON" or "TXT_KEY_CANCEL_BUTTON" )
			if not isAtWar and not isAIRequestingConcessions then

				-- Loop through deal items to see what the situation is
				g_Deal:ResetIterator()
				repeat
					local deal = { g_Deal:GetNextItem() }
					local n = #deal
					if n < 1 then
						break
					elseif deal[n] == g_iUs then
						numItemsFromUs = numItemsFromUs + 1
					else
						numItemsFromThem = numItemsFromThem + 1
					end
				until false
			end
		end

		-- Depending on what's on the table we can ask the other player about what they think of the deal
		Controls.WhatDoYouWantButton:SetHide( numItemsFromUs > 0 or numItemsFromThem == 0 )
		Controls.WhatWillYouGiveMeButton:SetHide( numItemsFromUs == 0 or numItemsFromThem > 0 )
		Controls.WhatWillMakeThisWorkButton:SetHide( numItemsFromUs == 0 or numItemsFromThem == 0 )
		Controls.WhatWillEndThisWarButton:SetHide( not isAtWar )
		Controls.WhatConcessionsButton:SetHide( not isAIRequestingConcessions )
		Controls.CancelButton:SetHide( false )
		Controls.ProposeButton:SetHide( false )
	end

end
local DoUpdateButtons = DoUpdateButtons

---------------------------------------------------------
-- AI is making a demand of the human
---------------------------------------------------------
function DoDemandState(bDemandOn)

	-- AI is demanding something, hide options that are not useful
	Controls.UsPanel:SetHide( bDemandOn )
	Controls.UsGlass:SetHide( bDemandOn )
	Controls.ThemPanel:SetHide( bDemandOn )
	Controls.ThemGlass:SetHide( bDemandOn )
	Controls.UsTableCover:SetHide( not bDemandOn )
	Controls.ThemTableCover:SetHide( not bDemandOn )

end
local DoDemandState = DoDemandState

local DoUIDealChangedByHuman -- Cross references

----------------------------------------------------------------------------------
-- Pocket Cities
----------------------------------------------------------------------------------
local function AddCityTrade( playerID, cityID )
	local player = Players[ playerID ]
	if player and player:GetCityByID( cityID ) then
		g_Deal:AddCityTrade( playerID, cityID )
		DoUIDealChangedByHuman()
	else
		print( "Error - city not found", playerID, cityID )
	end
end

local function RemoveCityTrade( playerID, cityID )
	local player = Players[ playerID ]
	if player and player:GetCityByID( cityID ) then
		g_Deal:RemoveCityTrade( playerID, cityID )
		DoUIDealChangedByHuman( true )
	else
		print( "Error - city not found", playerID, cityID )
	end
end

local function SetPocketCities( InstanceManager, fromPlayer, fromPlayerID, toPlayerID )
	InstanceManager:ResetInstances()
	if fromPlayer then
		for city in fromPlayer:Cities() do
			if g_Deal:IsPossibleToTradeItem( fromPlayerID, toPlayerID, TradeableItems.TRADE_ITEM_CITIES, city:GetX(), city:GetY() ) then
				local instance = InstanceManager:GetInstance()
				local cityID = city:GetID()
				instance.CityName:SetText( city:GetName() )
				instance.CityPop:SetText( city:GetPopulation().."[ICON_CITIZEN]" )
				local button = instance.Button
				button:SetVoids( fromPlayerID, cityID )
				button:RegisterCallback( Mouse.eLClick, AddCityTrade )
			end
		end
	end
end

local function SetPocketCivs( pocketButtonList, fromPlayerID, toPlayerID, tradeType )
	for playerID, button in pairs( pocketButtonList ) do
		local player = Players[ playerID ]
		if player then
			local teamID = player:GetTeam()
			button:SetHide( not( player:IsAlive()
				and g_iUsTeam ~= teamID
				and g_iThemTeam ~= teamID
				and g_pUsTeam:IsHasMet( teamID )
				and g_pThemTeam:IsHasMet( teamID )
				and g_Deal:IsPossibleToTradeItem( fromPlayerID, toPlayerID, tradeType, teamID ) )
				or ( player:IsMinorCiv() and Players[ player:GetAlly() ] ) )
--[[
				if g_Deal:IsPossibleToTradeItem( fromPlayerID, toPlayerID, tradeType, teamID ) then

					button:SetDisabled( false )
					button:SetAlpha( 1 )
				else

					button:SetDisabled( true )
					button:SetAlpha( 0.5 )
					local fromPlayer = Players[fromPlayerID]
					local fromTeamID = fromPlayer:GetTeam()

					-- Why won't they make peace?
					if tradeType == TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE then

						-- Not at war
						if not team:IsAtWar( fromTeamID ) then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_NOT_AT_WAR")

						-- Minor that won't make peace
						elseif player:IsMinorCiv() then

							local ally = Players[ player:GetAlly() ]

							-- Minor in permanent war with this guy
							if player:IsMinorPermanentWar( fromPlayerID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_PERMANENT_WAR")
							-- Minor allied to a player
							elseif ally and Teams[ally:GetTeam()]:IsAtWar( fromTeamID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_ALLY_AT_WAR")
							end

						-- Major that won't make peace
						else
							-- AI don't want no peace!
							if not fromPlayer:IsWillAcceptPeaceWithPlayer( playerID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_THIS_GUY_WANTS_WAR")
							-- Other AI don't want no peace!
							elseif not player:IsWillAcceptPeaceWithPlayer( fromPlayerID ) then
								strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_MINOR_OTHER_GUY_WANTS_WAR")
							end
						end

					-- Why won't they make war?
					elseif tradeType == TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR then
						-- Already at war
						if team:IsAtWar( fromTeamID ) then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_ALREADY_AT_WAR")

						-- Locked in to peace
						elseif Teams[fromTeamID]:IsForcePeace(teamID) then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_FORCE_PEACE")

						-- City-State ally
						elseif player:IsMinorCiv() and player:GetAlly() == fromPlayerID then
							strToolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_NO_WAR_ALLIES")
						end
					end
				end
--]]
		end
	end
end

local function ShowTableCiv( tableButtonList, pocketButtonList, teamID )
	for playerID, button in pairs( tableButtonList ) do
		local player = Players[ playerID ]
		if player and player:IsAlive() and player:GetTeam() == teamID then
			button:SetHide( false )
			tableButtonList.Parent:SetHide( false )
			pocketButtonList[ playerID ]:SetHide( true )
		end
	end
end

----------------------------------------------------------------
----------------------------------------------------------------

function ResetDisplay( diploMessage )

	--print("ResetDisplay")

	if g_iUs == -1 or g_iThem == -1 then
		return
	end

	Controls.UsPocketStack:SetHide( false )
	Controls.ThemPocketStack:SetHide( false )

	Controls.UsPocketGold:SetHide( false )
	Controls.ThemPocketGold:SetHide( false )
	Controls.UsPocketGoldPerTurn:SetHide( false )
	Controls.ThemPocketGoldPerTurn:SetHide( false )
	Controls.UsPocketOpenBorders:SetHide( false )
	Controls.ThemPocketOpenBorders:SetHide( false )
	Controls.UsPocketDefensivePact:SetHide( false )
	Controls.ThemPocketDefensivePact:SetHide( false )
	Controls.UsPocketResearchAgreement:SetHide( false )
	Controls.ThemPocketResearchAgreement:SetHide( false )
--	Controls.UsPocketTradeAgreement:SetHide( false ) --Trade agreement disabled for now
--	Controls.ThemPocketTradeAgreement:SetHide( false ) --Trade agreement disabled for now
	if gk_mode then
		Controls.UsPocketAllowEmbassy:SetHide( false )
		Controls.ThemPocketAllowEmbassy:SetHide( false )
		if Controls.UsPocketDoF then
			Controls.UsPocketDoF:SetHide( false )
		end
		if Controls.ThemPocketDoF then
			Controls.ThemPocketDoF:SetHide( false )
		end
	end -- gk_mode

	if not g_bMessageFromDiploAI then
		DoClearTable()
	end

	-- Research Agreement cost
	local strRAString = Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_US", Game.GetResearchAgreementCost( g_iUs, g_iThem ) )
	Controls.UsTableResearchAgreement:SetText( strRAString )
	Controls.ThemTableResearchAgreement:SetText( strRAString )


	if g_bTradeReview then
		-- review mode

		Controls.TradeDetails:SetHide( false )

		CivIconHookup( g_iUs, 64, Controls.UsSymbolShadow, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true )
		CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true )

		Controls.UsText:LocalizeAndSetText( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pUs:GetNameKey() )

		if m_bIsMulitplayer and pOtherPlayer:IsHuman() then
			Controls.ThemText:LocalizeAndSetText( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetNickName() )
		else
			Controls.ThemText:LocalizeAndSetText( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetName() )
		end

	elseif g_PVPTrade then
		-- PvP mode

		CivIconHookup( g_iUs, 64, Controls.UsSymbolShadow, Controls.UsCivIconBG, Controls.UsCivIconShadow, false, true )
		CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true )

		TruncateString(Controls.ThemName, Controls.ThemTablePanel:GetSizeX() - Controls.ThemTablePanel:GetOffsetX(),
							Locale.ConvertTextKey( "TXT_KEY_DIPLO_ITEMS_LABEL", g_pThem:GetNickName() ))
		Controls.ThemCiv:SetText( "(" .. Locale.ConvertTextKey( GameInfo.Civilizations[ g_pThem:GetCivilizationType() ].ShortDescription ) .. ")" )

	else
		Controls.NameText:LocalizeAndSetText( "TXT_KEY_DIPLO_LEADER_SAYS", g_pThem:GetName() )

		if not g_bMessageFromDiploAI then
			diploMessage = Locale.ConvertTextKey("TXT_KEY_DIPLO_HERE_OFFER")
		end

		-- Set Civ Icon
		CivIconHookup( g_iThem, 64, Controls.ThemSymbolShadow, Controls.ThemCivIconBG, Controls.ThemCivIconShadow, false, true )

		Controls.LeaderNameItems:SetText(Locale.ConvertTextKey("TXT_KEY_DIPLO_ITEMS_LABEL",Locale.ConvertTextKey(g_pThem:GetNameKey())))

		-- set up their portrait
--		local themCivInfo = GameInfo.Civilizations[ g_pThem:GetCivilizationType() ]
		local themLeaderInfo = GameInfo.Leaders[ g_pThem:GetLeaderType() ]
		IconHookup( themLeaderInfo.PortraitIndex, 128, themLeaderInfo.IconAtlas, Controls.ThemPortrait )

		-- Is the AI is making demand?
		Controls.WhatConcessionsButton:SetHide( not UI.IsAIRequestingConcessions() )

		-- If we're at war with the other guy then show the "what will end this war" button
		Controls.WhatWillEndThisWarButton:SetHide( not g_pUsTeam:IsAtWar(g_iThemTeam) )

	end


	local toolTip, isEnabled
	local isSameTeam = g_iUsTeam == g_iThemTeam
	----------------------------------------------------------------------------------
	-- pocket Gold
	----------------------------------------------------------------------------------

	-- Us pocket Gold
	isEnabled = g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD, 1 ) -- 1 here is 1 Gold, which is the minimum possible
	SetEnabledAndTextAndToolTip( Controls.UsPocketGold, isEnabled,
		g_Deal:GetGoldAvailable( g_iUs, -1 ) .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD"), -- This is -1 because we're not changing anything right now
		bnw_mode and not isEnabled and "[COLOR_WARNING_TEXT]"..Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_DOF_TT_ONE_LINE").."[ENDCOLOR]" )

	-- Them pocket Gold
	isEnabled = g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD, 1 ) -- 1 here is 1 Gold, which is the minimum possible
	SetEnabledAndTextAndToolTip( Controls.ThemPocketGold, isEnabled,
		g_Deal:GetGoldAvailable( g_iThem, -1 ) .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD"), -- This is -1 because we're not changing anything right now
		bnw_mode and not isEnabled and "[COLOR_WARNING_TEXT]"..Locale.ConvertTextKey("TXT_KEY_DIPLO_NEED_DOF_TT_ONE_LINE").."[ENDCOLOR]" )

	-- Us pocket Gold Per Turn
	SetEnabledAndText( Controls.UsPocketGoldPerTurn,
		g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration ), -- 1 here is 1 GPT, which is the minimum possible
		g_pUs:CalculateGoldRate() .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN") )

	-- Them pocket Gold Per Turn
	SetEnabledAndText( Controls.ThemPocketGoldPerTurn,
		g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_GOLD_PER_TURN, 1, g_iDealDuration ), -- 1 here is 1 GPT, which is the minimum possible
		g_pThem:CalculateGoldRate() .. " " .. Locale.ConvertTextKey("TXT_KEY_DIPLO_GOLD_PER_TURN") )


	----------------------------------------------------------------------------------
	-- pocket Allow Embassy
	----------------------------------------------------------------------------------
	if gk_mode then

		-- Us
		toolTip = g_embassyTooltip
--			toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_ALLOW_EMBASSY_THEY_HAVE" ):gsub("%[COLOR_NEGATIVE_TEXT%]","[COLOR_POSITIVE_TEXT]")
		if not g_pThemTeam:IsAllowEmbassyTradingAllowed() then
			toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_OTHER_PLAYER" ) .. "[ENDCOLOR]"
		end
		SetEnabledAndToolTip( Controls.UsPocketAllowEmbassy, g_Deal:IsPossibleToTradeItem(g_iUs, g_iThem, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration), toolTip )
		Controls.UsPocketAllowEmbassy:SetHide( isSameTeam or g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam) )

		-- Them
		toolTip = g_embassyTooltip
--			toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_ALLOW_EMBASSY_HAVE" ):gsub("%[COLOR_NEGATIVE_TEXT%]","[COLOR_POSITIVE_TEXT]")
		if not g_pUsTeam:IsAllowEmbassyTradingAllowed() then
			toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_ALLOW_EMBASSY_NO_TECH_PLAYER" ) .. "[ENDCOLOR]"
		end
		SetEnabledAndToolTip( Controls.ThemPocketAllowEmbassy, g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, g_iDealDuration), toolTip )
		Controls.ThemPocketAllowEmbassy:SetHide( isSameTeam or g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) )
	end -- gk_mode
	----------------------------------------------------------------------------------
	-- pocket Open Borders
	----------------------------------------------------------------------------------

	-- Are we not allowed to give OB?
	toolTip = g_borderTooltip
--		toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_OPEN_BORDERS_HAVE" ):gsub("%[COLOR_NEGATIVE_TEXT%]","[COLOR_POSITIVE_TEXT]")
	if not g_pThemTeam:IsOpenBordersTradingAllowed() then
		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey(gk_mode and "TXT_KEY_DIPLO_OPEN_BORDERS_NO_TECH" or "TXT_KEY_DIPLO_NEED_WRITING_TT") .. "[ENDCOLOR]"
	elseif gk_mode and not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) then
		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_YOU_NEED_EMBASSY_TT" ) .. "[ENDCOLOR]"
	end
	SetEnabledAndToolTip( Controls.UsPocketOpenBorders, g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration), toolTip )
	Controls.UsPocketOpenBorders:SetHide( isSameTeam or g_pUsTeam:IsAllowsOpenBordersToTeam(g_iThemTeam) )

	-- Are they not allowed to give OB?
	toolTip = g_borderTooltip
--		toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_OPEN_BORDERS_THEY_HAVE" ):gsub("%[COLOR_NEGATIVE_TEXT%]","[COLOR_POSITIVE_TEXT]")
	if not g_pUsTeam:IsOpenBordersTradingAllowed() then
		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey(gk_mode and "TXT_KEY_DIPLO_OPEN_BORDERS_NO_TECH" or "TXT_KEY_DIPLO_NEED_WRITING_TT") .. "[ENDCOLOR]"
	elseif gk_mode and not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam) then
		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_THEY_NEED_EMBASSY_TT" ) .. "[ENDCOLOR]"
	end
	SetEnabledAndToolTip( Controls.ThemPocketOpenBorders, g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_OPEN_BORDERS, g_iDealDuration), toolTip )
	Controls.ThemPocketOpenBorders:SetHide( isSameTeam or g_pThemTeam:IsAllowsOpenBordersToTeam(g_iUsTeam) )

	----------------------------------------------------------------------------------
	-- pocket Defensive Pact
	----------------------------------------------------------------------------------


	toolTip = g_defensivePactToolTip
--		toolTip = toolTip .. "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_EXISTS" ) .. "[ENDCOLOR]"
	if not g_pUsTeam:IsDefensivePactTradingAllowed() and not g_pThemTeam:IsDefensivePactTradingAllowed() then
		toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_NO_TECH" )
	elseif gk_mode and ( not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) or not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam) ) then
		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT" ) .. "[ENDCOLOR]"
--	else
--		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_DEF_PACT_NO_AGREEMENT" ) .. "[ENDCOLOR]"
	end
	SetEnabledAndToolTip( Controls.UsPocketDefensivePact, g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration), toolTip )
	SetEnabledAndToolTip( Controls.ThemPocketDefensivePact, g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iDealDuration), toolTip )
	Controls.UsPocketDefensivePact:SetHide( isSameTeam or g_pUsTeam:IsDefensivePact(g_iThemTeam) )
	Controls.ThemPocketDefensivePact:SetHide( isSameTeam or g_pThemTeam:IsDefensivePact(g_iUsTeam) )

	----------------------------------------------------------------------------------
	-- pocket Research Agreement
	----------------------------------------------------------------------------------

	local researchAgreementCost = Game.GetResearchAgreementCost(g_iThem, g_iUs)

	toolTip = ""


	if Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE) then
		toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_SCIENCE_OFF" )
	elseif g_pUsTeam:GetTeamTechs():HasResearchedAllTechs() or g_pThemTeam:GetTeamTechs():HasResearchedAllTechs() then
		toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_ALL_TECHS_RESEARCHED" )
	elseif g_pUsTeam:IsHasResearchAgreement(g_iThemTeam) then
		toolTip = "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_EXISTS" )
	else
		if not g_pUsTeam:IsResearchAgreementTradingAllowed() and not g_pThemTeam:IsResearchAgreementTradingAllowed() then
			toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_TECH" )
		end
		if gk_mode then
			if not g_pUsTeam:HasEmbassyAtTeam(g_iThemTeam) or not g_pThemTeam:HasEmbassyAtTeam(g_iUsTeam) then
				toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_BOTH_NEED_EMBASSY_TT" )
			end
			if not g_pUs:IsDoF(g_iThem) or not g_pThem:IsDoF(g_iUs) then
				toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_NEED_DOF_TT" )
			end
		end
		local ourCash = g_pUs:GetGold() - researchAgreementCost
		local theirCach = g_pThem:GetGold() - researchAgreementCost
		if ourCash < 0 or theirCach < 0 then
			toolTip = toolTip .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_NO_AGREEMENT" )
			local spacer = " "
			if theirCach < 0 then
				toolTip = toolTip .. spacer .. g_pThem:GetName() ..": " .. theirCach
				spacer = ", "
			end
			if ourCash < 0 then
				toolTip = toolTip .. spacer .. Locale.ConvertTextKey("TXT_KEY_YOU") ..": " .. ourCash
			end
		end
	end
	toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_RESCH_AGREEMENT_TT", researchAgreementCost, g_iDealDuration ) .. "[COLOR_WARNING_TEXT]" .. toolTip .. "[ENDCOLOR]"
	SetEnabledAndToolTip( Controls.UsPocketResearchAgreement, g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration), toolTip )
	SetEnabledAndToolTip( Controls.ThemPocketResearchAgreement, g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iDealDuration), toolTip )
	Controls.UsPocketResearchAgreement:SetHide( isSameTeam or g_pUsTeam:IsHasResearchAgreement(g_iThemTeam) )
	Controls.ThemPocketResearchAgreement:SetHide( isSameTeam or g_pThemTeam:IsHasResearchAgreement(g_iUsTeam) )

	----------------------------------------------------------------------------------
	-- pocket Trade Agreement
	----------------------------------------------------------------------------------

	-- Are we not allowed to give TA? (don't have tech, or are already providing it to them)

	toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_TT" )
	if g_pUsTeam:IsHasTradeAgreement(g_iThemTeam) then
		toolTip = toolTip .. "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_EXISTS" ) .. "[ENDCOLOR]"
	elseif not g_pUsTeam:IsTradeAgreementTradingAllowed() and not g_pThemTeam:IsTradeAgreementTradingAllowed() then
		toolTip = toolTip .. "[COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_NO_TECH" ) .. "[ENDCOLOR]"
--	else
--		toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_TRADE_AGREEMENT_NO_AGREEMENT" )
	end
	SetEnabledAndToolTip( Controls.UsPocketTradeAgreement, g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iDealDuration), toolTip )
	SetEnabledAndToolTip( Controls.ThemPocketTradeAgreement, g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iDealDuration), toolTip )

	----------------------------------------------------------------------------------
	-- Pocket Declaration of Friendship
	----------------------------------------------------------------------------------

	if gk_mode and Controls.UsPocketDoF and Controls.ThemPocketDoF then
		Controls.UsPocketDoF:SetHide( not g_PVPTrade )
		Controls.ThemPocketDoF:SetHide( not g_PVPTrade )
		if g_PVPTrade then	-- Only PvP trade, with the AI there is a dedicated interface for this trade.

			toolTip = Locale.ConvertTextKey("TXT_KEY_DIPLO_DISCUSS_MESSAGE_DEC_FRIENDSHIP_TT")
			if g_pUs:IsDoF(g_iThem) or g_pThem:IsDoF(g_iUs) then
				toolTip = toolTip .. "[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DECLARATION_OF_FRIENDSHIP_ALREADY_EXISTS" ).. "[ENDCOLOR]"
			elseif g_pUsTeam:IsAtWar( g_iThemTeam ) then
				toolTip = toolTip .. "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]" .. Locale.ConvertTextKey("TXT_KEY_DIPLO_DECLARATION_OF_FRIENDSHIP_AT_WAR" ) .. "[ENDCOLOR]"
			end
			SetEnabledAndToolTip( Controls.UsPocketDoF, g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iDealDuration), toolTip )
			SetEnabledAndToolTip( Controls.ThemPocketDoF, g_Deal:IsPossibleToTradeItem( g_iThem, g_iUs, TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iDealDuration), toolTip )
		end
	end

	-- Pocket Cities
	SetPocketCities( g_UsPocketCitiesIM, g_pUs, g_iUs, g_iThem )
	SetPocketCities( g_ThemPocketCitiesIM, g_pThem, g_iThem, g_iUs )

	-- pocket other civs
	SetPocketCivs( g_usPocketPeaceButtons, g_iUs, g_iThem, TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE )
	SetPocketCivs( g_themPocketPeaceButtons, g_iThem, g_iUs, TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE )
	SetPocketCivs( g_usPocketWarButtons, g_iUs, g_iThem, TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR )
	SetPocketCivs( g_themPocketWarButtons, g_iThem, g_iUs, TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR )

	----------------------------------------------------------------------------------
	-- pocket resources for us
	----------------------------------------------------------------------------------
	for resourceID, instance in pairs( g_UsPocketResources ) do

		if g_Deal:IsPossibleToTradeItem( g_iUs, g_iThem, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1 ) then	-- 1 here is 1 quanity of the Resource, which is the minimum possible
			instance.Button:SetHide( false )
			local resource = GameInfo.Resources[resourceID]
			instance.Button:SetText( resource.IconString .. Locale.ConvertTextKey(resource.Description) .. " (" .. ( gk_mode and g_Deal:GetNumResource(g_iUs, resourceID) or g_pUs:GetNumResourceAvailable( resourceID, false ) ) .. ")" )
		else
			instance.Button:SetHide( true )
		end
	end

	----------------------------------------------------------------------------------
	-- pocket resources for them
	----------------------------------------------------------------------------------
	for resourceID, instance in pairs( g_ThemPocketResources ) do
		if g_Deal:IsPossibleToTradeItem(g_iThem, g_iUs, TradeableItems.TRADE_ITEM_RESOURCES, resourceID, 1) then -- 1 here is 1 quanity of the Resource, which is the minimum possible
			instance.Button:SetHide( false )
			local resource = GameInfo.Resources[resourceID]

			instance.Button:SetText( resource.IconString .. " " .. Locale.ConvertTextKey(resource.Description) .. " (" .. (gk_mode and g_Deal:GetNumResource(g_iThem, resourceID) or g_pThem:GetNumResourceAvailable( resourceID, false )) .. ")" )
		else
			instance.Button:SetHide( true )
		end
	end

	----------------------------------------------------------------------------------
	-- Votes
	----------------------------------------------------------------------------------
	if bnw_mode then
		local isLeagues = not Game.IsOption("GAMEOPTION_NO_LEAGUES")
		Controls.UsPocketVote:SetHide( not isLeagues )
		Controls.ThemPocketVote:SetHide( not isLeagues )
		if isLeagues then

			RefreshPocketVotes(1)

			RefreshPocketVotes(0)
		end
	end --bnw_mode

	ResizeStacks()
	-- Propose button could have had its text changed to "ACCEPT" if the AI made an offer to the human
	return DoUpdateButtons( diploMessage )
end
local ResetDisplay = ResetDisplay

----------------------------------------------------------------
----------------------------------------------------------------
function DisplayDeal(...)
	ResetDisplay(...)
	g_UsTableCitiesIM:ResetInstances()
	g_ThemTableCitiesIM:ResetInstances()
	if bnw_mode then
		g_UsTableVoteIM:ResetInstances()
		g_ThemTableVoteIM:ResetInstances()
	end
	if g_iUs == -1 or g_iThem == -1 then
		return
	end

	--print("Displaying Deal")

	local itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID, toPlayerID

	local toolTip

	local numItemsFromUs = 0
	local numItemsFromThem = 0

	g_Deal:ResetIterator()

	--print("Going through check")
	repeat
		if bnw_mode then
			itemType, duration, finalTurn, data1, data2, data3, flag1, fromPlayerID = g_Deal:GetNextItem()
		else
			itemType, duration, finalTurn, data1, data2, fromPlayerID = g_Deal:GetNextItem()
		end
		if not itemType then break end
		local isFromUs = fromPlayerID == g_iUs

		--print("Adding trade item to active deal: " .. itemType)

		if isFromUs then
			toPlayerID = g_iThem
			numItemsFromUs = numItemsFromUs + 1
		else
			toPlayerID = g_iUs
			numItemsFromThem = numItemsFromThem + 1
		end

		local itemControls = g_itemControls[ itemType ]
		if itemControls then
			local pocketControl = itemControls[ isFromUs and 1 or 3 ]
			local tableControl = itemControls[ isFromUs and 2 or 4 ]
			if pocketControl then
				pocketControl:SetHide( true )
			end
			if tableControl then
				tableControl:SetHide( false )
			end

		elseif TradeableItems.TRADE_ITEM_GOLD == itemType then

			Controls.UsPocketGold:SetHide( true )
			Controls.ThemPocketGold:SetHide( true )
			Controls.UsTableGold:SetHide( not isFromUs )
			Controls.ThemTableGold:SetHide( isFromUs )

			-- update quantity
			;(isFromUs and Controls.UsGoldAmount or Controls.ThemGoldAmount):SetText( data1 )
			;(isFromUs and Controls.UsTableGold or Controls.ThemTableGold):LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(fromPlayerID, -1) ) -- This is -1 because we're not changing anything right now

		elseif TradeableItems.TRADE_ITEM_GOLD_PER_TURN == itemType then

			Controls.UsPocketGoldPerTurn:SetHide( true )
			Controls.ThemPocketGoldPerTurn:SetHide( true )

			if isFromUs then
				Controls.UsTableGoldPerTurn:SetHide( false )
				Controls.UsGoldPerTurnTurns:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration )
				Controls.UsGoldPerTurnAmount:SetText( data1 )
				Controls.UsTableGoldPerTurn:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GPT", g_pUs:CalculateGoldRate() - data1 )
			else
				Controls.ThemTableGoldPerTurn:SetHide( false )
				Controls.ThemGoldPerTurnTurns:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration )
				Controls.ThemGoldPerTurnAmount:SetText( data1 )
				Controls.ThemTableGoldPerTurn:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GPT", g_pThem:CalculateGoldRate() - data1 )
			end

		elseif TradeableItems.TRADE_ITEM_CITIES == itemType then

			local plot = Map.GetPlot( data1, data2 )
			local city = plot and plot:GetPlotCity()
			local instance = (isFromUs and g_UsTableCitiesIM or g_ThemTableCitiesIM):GetInstance()
			if city then
				instance.Button:SetVoids( fromPlayerID, city:GetID() )
				instance.Button:RegisterCallback( Mouse.eLClick, RemoveCityTrade )
				instance.CityName:SetText( city:GetName() )
				instance.CityPop:SetText( city:GetPopulation().."[ICON_CITIZEN]" )
			else
				instance.Button:SetVoids( -1, -1 )
				instance.Button:ClearCallback( Mouse.eLClick )
				instance.CityName:LocalizeAndSetText( "TXT_KEY_RAZED_CITY" )
				instance.CityPop:SetText()
			end

		elseif TradeableItems.TRADE_ITEM_THIRD_PARTY_PEACE == itemType then
			if isFromUs then
				ShowTableCiv( g_usTablePeaceButtons, g_usPocketPeaceButtons, data1 )
			else
				ShowTableCiv( g_themTablePeaceButtons, g_themPocketPeaceButtons, data1 )
			end

		elseif TradeableItems.TRADE_ITEM_THIRD_PARTY_WAR == itemType then
			if isFromUs then
				ShowTableCiv( g_usTableWarButtons, g_usPocketWarButtons, data1 )
			else
				ShowTableCiv( g_themTableWarButtons, g_themPocketWarButtons, data1 )
			end

		elseif TradeableItems.TRADE_ITEM_RESOURCES == itemType then

			local instance = ( isFromUs and g_UsTableResources or g_ThemTableResources )[ data1 ]
			if instance then

				instance.Container:SetHide( false )
				instance.DurationEdit:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", duration )
				if instance.AmountEdit then -- is strategic
					( isFromUs and Controls.UsTableStrategicStack or Controls.ThemTableStrategicStack ):SetHide( false )
					instance.AmountEdit:SetText( data2 )
				else
					( isFromUs and Controls.UsTableLuxuryStack or Controls.ThemTableLuxuryStack ):SetHide( false )
				end
				g_UsPocketResources[ data1 ].Button:SetHide( true )
				g_ThemPocketResources[ data1 ].Button:SetHide( true )
			end

		elseif bnw_mode and TradeableItems.TRADE_ITEM_VOTE_COMMITMENT == itemType then
			--print("==debug== VOTE_COMMITMENT found in DisplayDeal")

			local activeLeague = Game.GetNumActiveLeagues() > 0 and Game.GetActiveLeague()
			if activeLeague then
				local voteIndex = GetLeagueVoteIndexFromData(data1, data2, flag1)
				local resolution = g_LeagueVoteList[voteIndex]
				if resolution then
					local instance = (isFromUs and g_UsTableVoteIM or g_ThemTableVoteIM):GetInstance()
					instance.ProposalLabel:SetText( GetVoteText(activeLeague, voteIndex, flag1, data3) )
					instance.VoteLabel:SetText( activeLeague:GetTextForChoice(resolution.VoteDecision, resolution.VoteChoice) )
					instance.Button:SetToolTipString( GetVoteTooltip(activeLeague, voteIndex, flag1, data3) )
					instance.Button:SetVoids( fromPlayerID, voteIndex )
					instance.Button:RegisterCallback( Mouse.eLClick, OnChooseTableVote )
					;(isFromUs and Controls.UsTableVoteStack or Controls.ThemTableVoteStack):SetHide( false )
				end
			end
		end
	until false

	for k in pairs( g_SubStacks ) do
		SubStackToggle( Controls[k], false ) -- false = no state change
	end

	ResizeStacks()
end
local DisplayDeal = DisplayDeal

---------------------------------------------------------
-- LEADER MESSAGE HANDLER
---------------------------------------------------------
function LeaderMessageHandler( diploPlayerID, diploUIstateID, diploMessage, animationActionID, diploData )
--local dps, pdpsfor k,v in pairs(DiploUIStateTypes) do if v==diploUIstateID then dps=k end if v==g_diploUIStateID then pdps=k end end diploMessage = "Previous: "..tostring(pdps).."[NEWLINE]Current: "..tostring(dps)..tostring(diploData).."[NEWLINE]"..diploMessage
	g_PVPTrade = false
	g_bTradeReview = false

	g_iUs = Game.GetActivePlayer()
	g_pUs = Players[ g_iUs ]
	g_iUsTeam = g_pUs:GetTeam()
	g_pUsTeam = Teams[ g_iUsTeam ]
	g_iThem = diploPlayerID
	g_pThem = Players[ g_iThem ]
	g_iThemTeam = g_pThem:GetTeam()
	g_pThemTeam = Teams[ g_iThemTeam ]

	-- skip trade deal discussion blurb
	if UI.GetLeaderHeadRootUp() and diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_BLANK_DISCUSSION and
			( g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER
				or g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE ) then
--		g_Deal:ClearItems()
--		diploMessage = diploMessage .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_DIPLOMACY_ANYTHING_ELSE" )
		diploUIstateID = DiploUIStateTypes.DIPLO_UI_STATE_TRADE
	-- If we were just in discussion mode and the human offered to make concessions, make a note of that
	elseif diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS then
		if g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV then
			--print("Human offers concessions for killing Minor Civ")
			g_iConcessionsPreviousDiploUIState = g_diploUIStateID
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING then
			--print("Human offers concessions for expansion")
			g_iConcessionsPreviousDiploUIState = g_diploUIStateID
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING then
			--print("Human offers concessions for plot buying")
			g_iConcessionsPreviousDiploUIState = g_diploUIStateID
		end
	end

	g_diploUIStateID = diploUIstateID

	-- Are we in Trade Mode?
	if diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER
		or diploUIstateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND
	then

		--print("TradeScreen: It's MY mode!")

		if ContextPtr:IsHidden() then
			UIManager:QueuePopup( ContextPtr, PopupPriority.LeaderTrade )
		end

print("Handling LeaderMessage: " .. diploUIstateID .. ", ".. diploMessage)

		g_Deal:SetFromPlayer(g_iUs)
		g_Deal:SetToPlayer(g_iThem)

		-- Unhide our pocket, in case the last thing we were doing in this screen was a human demand
		Controls.UsPanel:SetHide(false)
		Controls.UsGlass:SetHide(false)

		local bClearTableAndDisplayDeal = false

		-- Is this a UI State where we should be displaying a deal?
		if g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE then
			--print("DiploUIState: Default Trade")
			bClearTableAndDisplayDeal = true
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND then
			--print("DiploUIState: AI making demand")
			bClearTableAndDisplayDeal = true

			DoDemandState(true)

		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST then
			--print("DiploUIState: AI making Request")
			bClearTableAndDisplayDeal = true

			DoDemandState(true)

		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
			--print("DiploUIState: Human Demand")
			bClearTableAndDisplayDeal = true
			-- If we're demanding something, there's no need to show OUR items
			Controls.UsPanel:SetHide(true)
			Controls.UsGlass:SetHide(true)

		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_HUMAN_OFFERS_CONCESSIONS then
			--print("DiploUIState: Human offers concessions")
			bClearTableAndDisplayDeal = true
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER then
			--print("DiploUIState: AI making offer")
			bClearTableAndDisplayDeal = true
			g_bAIMakingOffer = true
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_ACCEPTS_OFFER then
			--print("DiploUIState: AI accepted offer")
			g_iConcessionsPreviousDiploUIState = -1		-- Clear out the fact that we were offering concessions if the AI has agreed to a deal
			bClearTableAndDisplayDeal = true

		-- If the AI rejects a deal, don't clear the table: keep the items where they are in case the human wants to change things
		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER then
			--print("DiploUIState: AI rejects offer")
			bClearTableAndDisplayDeal = false
		else
			--print("DiploUIState: ?????")
		end

		-- Clear table and display the deal currently stored in InterfaceBuddy
		if bClearTableAndDisplayDeal then
			g_bMessageFromDiploAI = true
			DoClearTable()
			DisplayDeal()

		-- Don't clear the table, leave things as they are
		else

			--print("NOT clearing table")

			g_bMessageFromDiploAI = true

		end

		DoUpdateButtons( diploMessage )

	-- Not in trade mode
	else

		--print("TradeScreen: NOT my mode! Hiding!")
		--print("diploUIstateID: " .. diploUIstateID)

		g_Deal:ClearItems()

		if not ContextPtr:IsHidden() then
			ContextPtr:SetHide( true )
		end

	end

end
--Events.AILeaderMessage.Add( LeaderMessageHandler )
--Events.LeavingLeaderViewMode.Add(
--function()
--	g_isHumanInitiatedTrade = false
--	g_iConcessionsPreviousDiploUIState = -1
--	g_diploUIStateID = -1
--end)

----------------------------------------------------------------
-- Used by SimpleDiploTrade to display pvp deals
----------------------------------------------------------------
function OnOpenPlayerDealScreen( playerID )
	g_iUs = Game.GetActivePlayer()
	g_pUs = Players[ g_iUs ]
	g_iUsTeam = g_pUs:GetTeam()
	g_pUsTeam = Teams[ g_iUsTeam ]

	g_iThem = playerID
	g_pThem = Players[ g_iThem ]
	g_iThemTeam = g_pThem:GetTeam()
	g_pThemTeam = Teams[ g_iThemTeam ]

	local isAtWar = g_pUsTeam:IsAtWar( g_iThemTeam )

	if g_iUs == g_iThem then
		return print( "ERROR: OpenPlayerDealScreen called with local player" ) -- cannot deal with ourselves
	
	elseif isAtWar and (g_bAlwaysWar or g_bNoChangeWar) then
		return -- someone is trying to open the deal screen when they are not allowed to negotiate peace

	elseif UI.ProposedDealExists( g_iThem, g_iUs ) then
		g_PVPTrade = 2 -- is this is their proposal
		DoClearTable()
		UI.LoadProposedDeal( g_iThem, g_iUs )

	elseif UI.ProposedDealExists( g_iUs, g_iThem ) then
		g_PVPTrade = 1 -- this is our proposal
		DoClearTable()
		UI.LoadProposedDeal( g_iUs, g_iThem )
	else
		g_PVPTrade = 0 -- setup a new deal...
		DoClearDeal()
		g_Deal:SetFromPlayer( g_iUs )
		g_Deal:SetToPlayer( g_iThem )
		if isAtWar then
			g_Deal:AddPeaceTreaty( g_iUs, g_iPeaceDuration )
			g_Deal:AddPeaceTreaty( g_iThem, g_iPeaceDuration )
		end
	end

	ContextPtr:SetHide( false )
	DisplayDeal()
end


----------------------------------------------------------------
-- used by DiploOverview to display active deals
----------------------------------------------------------------
function OpenDealReview()

	--print( "OpenDealReview" )

	g_iUs = Game:GetActivePlayer()
	g_pUs = Players[ g_iUs ]
	g_iUsTeam = g_pUs:GetTeam()
	g_pUsTeam = Teams[ g_iUsTeam ]

	g_iThem = g_Deal:GetOtherPlayer( g_iUs )
	g_pThem = Players[ g_iThem ]
	g_iThemTeam = g_pThem:GetTeam()
	g_pThemTeam = Teams[ g_iThemTeam ]

	if g_iUs == g_iThem then
		print( "ERROR: OpenDealReview called with local player" )
	end

	g_PVPTrade = false
	g_bTradeReview = true

	DoClearTable()
	DisplayDeal()
end


----------------------------------------------------------------
-- BACK
----------------------------------------------------------------
function OnBack( flag )

	if g_bTradeReview or g_PVPTrade then

		if g_PVPTrade and flag then
			if g_PVPTrade == 2 then -- refuse their proposal
				UI.DoFinalizePlayerDeal( g_iThem, g_iUs, false )

			elseif g_PVPTrade == 1 then -- withdraw our proposal
				UI.DoFinalizePlayerDeal( g_iUs, g_iThem, false )

			else -- new deal
				g_Deal:ClearItems()
			end
		end

		ContextPtr:SetHide( true )
		ContextPtr:CallParentShowHideHandler( false )
	else
		-- Human refused to give into an AI demand - this button doubles as the Demand "Refuse" option
		if g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND then
			DoDemandState(false)
			DoClearDeal()
			DisplayDeal()
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL, g_iThem, 0, 0 )

		elseif g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST then
			DoDemandState(false)
			DoClearDeal()
			DisplayDeal()
			Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL, g_iThem, gk_mode and g_Deal and g_pUs:GetDealMyValue(g_Deal) or 0, 0 )

		elseif g_Deal:GetNumItems() ~= 0 and not g_pUsTeam:IsAtWar( g_iThemTeam ) and UI.GetLeaderHeadRootUp() then
			DoDemandState(false)
			DoClearDeal()
			return DisplayDeal( Locale.ConvertTextKey"TXT_KEY_DIPLOMACY_ANYTHING_ELSE" )

		else -- Exit screen normally

			g_Deal:ClearItems()
			if g_pThem then
				g_pThem:DoTradeScreenClosed(g_bAIMakingOffer)
			end

			-- Don't assume we have a message from the AI leader any more
			g_bMessageFromDiploAI = false

			-- If the AI opened the screen to make an offer, clear that status
			g_bAIMakingOffer = false

			-- Reset value that keeps track of how many times we offered a bad deal (for dialogue purposes)
			UI.SetOfferTradeRepeatCount(0)

			-- If the human was offering concessions, he backed out
			if g_iConcessionsPreviousDiploUIState ~= -1 then
				--print("Human backing out of Concessions Offer. Reseting DiploUIState to trade.")

				iButtonID = 1		-- This corresponds with the buttons in DiscussionDialog.lua

				-- AI scolded us for killing a Minor - we tell him to go away
				if g_iConcessionsPreviousDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_KILLED_MINOR_CIV then
					Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE, g_iThem, iButtonID, 0 )
				-- AI seriously warning us about expansion - we tell him to go away
				elseif g_iConcessionsPreviousDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING then
					Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE, g_iThem, iButtonID, 0 )
				-- AI seriously warning us about plot buying - we tell him to go away
				elseif g_iConcessionsPreviousDiploUIState == DiploUIStateTypes.DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING then
					Game.DoFromUIDiploEvent( FromUIDiploEventTypes.FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE, g_iThem, iButtonID, 0 )
				end

				g_iConcessionsPreviousDiploUIState = -1
			end

			-- Exiting human demand mode, unhide his items
			if g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
				Controls.UsPanel:SetHide(false)
				Controls.UsGlass:SetHide(false)
			end
			UI.RequestLeaveLeader()
		end
		UIManager:DequeuePopup( ContextPtr )
	end
	g_diploUIStateID = false
end
local OnBack = OnBack
Controls.CancelButton:RegisterCallback( Mouse.eLClick, OnBack )

----------------------------------------------------------------
-- SHOW/HIDE
----------------------------------------------------------------
function OnShowHide( isHide, bIsInit )

	-- WARNING: Don't put anything important in here related to gameplay. This function may be called when you don't expect it

	if not bIsInit then
		-- Showing screen
		if not isHide then
			oldCursor = UIManager:SetUICursor(0) -- make sure we start with the default cursor

			if g_pUs then g_Deal:SetFromPlayer( g_iUs ) end
			if g_pThem then g_Deal:SetToPlayer( g_iThem ) end

			DoClearTable()
			DisplayDeal()
			LuaEvents.TryQueueTutorial("DIPLO_TRADE_SCREEN", true)

		-- Hiding screen
		else
			UIManager:SetUICursor(oldCursor) -- make sure we retrun the cursor to the previous state
			LuaEvents.TryDismissTutorial("DIPLO_TRADE_SCREEN")

		end
	end

end
ContextPtr:SetShowHideHandler( OnShowHide )

----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then

			-- Don't allow any keys to exit screen when AI is asking for something - want to make sure the human has to click something
			if (g_diploUIStateID ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND and
				g_diploUIStateID ~= DiploUIStateTypes.DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST) then

				OnBack()
			end

			return true
		-- Consume enter here otherwise it may be
		-- interpretted as dismissing the dialog.
		elseif wParam == Keys.VK_RETURN then
			return true
		end
	end
	return false
end

---------------------------------------------------------
-- UI Deal was modified somehow
---------------------------------------------------------
function DoUIDealChangedByHuman( isClearTable )

	-- Set state to the default so that it doesn't cause any funny behavior later
	if g_diploUIStateID ~= DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
		g_diploUIStateID = DiploUIStateTypes.DIPLO_UI_STATE_TRADE
	end

	if isClearTable then
		DoClearTable()
	end
	DisplayDeal()
end

----------------------------------------------------------------
-- Proposal and Negotiate functions
----------------------------------------------------------------

function OnModify()
	g_PVPTrade = 0 -- this is now a new deal
	DoUpdateButtons()
end


----------------------------------------------------------------
----------------------------------------------------------------
Controls.ProposeButton:RegisterCallback( Mouse.eLClick,
function()

	-- Trade between humans
	if g_PVPTrade then

		if g_PVPTrade == 2 then -- accept their proposal
			UI.DoFinalizePlayerDeal( g_iThem, g_iUs, true )

		elseif g_PVPTrade == 1 then -- withdraw our proposal
			UI.DoFinalizePlayerDeal( g_iUs, g_iThem, false )

		else -- this is a new deal
			if g_Deal:GetNumItems() == 0 then
				if bnw_mode and g_pUsTeam:CanChangeWarPeace( g_iThemTeam ) and not g_pUsTeam:IsAtWar( g_iThemTeam ) and g_pUsTeam:CanDeclareWar( g_iThemTeam ) then
					-- Declaring War - bring up the BNW popup
					UI.AddPopup{ Type = ButtonPopupTypes.BUTTONPOPUP_DECLAREWARMOVE, Data1 = g_iThemTeam, Option1 = true }
				end
				return
			else
				-- Set the from/to players in the deal.  We do this because we might have not done this already.
				g_Deal:SetFromPlayer( g_iUs )
				g_Deal:SetToPlayer( g_iThem )
				UI.DoProposeDeal()
			end
		end

		ContextPtr:SetHide( true )
		ContextPtr:CallParentShowHideHandler( false )

	-- Trade between a human & an AI
	else
		if g_Deal:GetNumItems() == 0 then
			UI.SetLeaderHeadRootUp( false )
			UI.RequestLeaveLeader()
		else
	    		-- Set the from/to players in the deal.  We do this because we might have not done this already.
			g_Deal:SetFromPlayer( g_iUs )
			g_Deal:SetToPlayer( g_iThem )
			-- Human Demanding something
			if g_diploUIStateID == DiploUIStateTypes.DIPLO_UI_STATE_HUMAN_DEMAND then
				UI.DoDemand()

			-- Trade offer
			else
				UI.DoProposeDeal()
			end

			-- Don't assume we have a message from the AI leader any more
			g_bMessageFromDiploAI = false

			-- If the AI was demanding something, we're not in that state any more
			DoDemandState( false )
		end
		UIManager:DequeuePopup( ContextPtr )
	end
--	g_diploUIStateID = false
end)


----------------------------------------------------------------
----------------------------------------------------------------
function OnEqualizeDeal()

	UI.DoEqualizeDealWithHuman()

	-- Don't assume we have a message from the AI leader any more
	g_bMessageFromDiploAI = false

end


----------------------------------------------------------------
----------------------------------------------------------------
function OnWhatDoesAIWant()

	UI.DoWhatDoesAIWant()

	-- Don't assume we have a message from the AI leader any more
	g_bMessageFromDiploAI = false

end


----------------------------------------------------------------
----------------------------------------------------------------
function OnWhatWillAIGive()

	UI.DoWhatWillAIGive()

	-- Don't assume we have a message from the AI leader any more
	g_bMessageFromDiploAI = false

end

-----------------------------------------------------------------------------------------------------------------------
-- Gold Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function PocketGoldHandler( isUs )

		local iAmount = UI.ShiftKeyDown() and math_huge or 30

		local iAmountAvailable

		if isUs == 1 then

			iAmountAvailable = g_Deal:GetGoldAvailable(g_iUs, -1 ) -- This is -1 because we're not changing anything right now
			if iAmount > iAmountAvailable then
				iAmount = iAmountAvailable
			end
			g_Deal:AddGoldTrade( g_iUs, iAmount )

		else
			iAmountAvailable = g_Deal:GetGoldAvailable(g_iThem, -1 ) -- This is -1 because we're not changing anything right now
			if iAmount > iAmountAvailable then
				iAmount = iAmountAvailable
			end
			g_Deal:AddGoldTrade( g_iThem, iAmount )
		end
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketGold:RegisterCallback( Mouse.eLClick, PocketGoldHandler )
	Controls.ThemPocketGold:RegisterCallback( Mouse.eLClick, PocketGoldHandler )
	Controls.UsPocketGold:SetVoid1( 1 )
	Controls.ThemPocketGold:SetVoid1( 0 )

	function TableGoldHandler( isUs )
		if isUs == 1 then
			g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD, g_iUs )
		else
			g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD, g_iThem )
		end
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableGold:RegisterCallback( Mouse.eLClick, TableGoldHandler )
	Controls.ThemTableGold:RegisterCallback( Mouse.eLClick, TableGoldHandler )
	Controls.UsTableGold:SetVoid1( 1 )
	Controls.ThemTableGold:SetVoid1( 0 )

	function ChangeGoldAmount( string, control )

		local iGold = 0
		if string and string ~= "" then
			iGold = tonumber(string)
		else
			control:SetText( 0 )
		end

		local iAmountAvailable

		-- Gold from us
		if control:GetVoid1() == 1 then

			iAmountAvailable = g_Deal:GetGoldAvailable(g_iUs, TradeableItems.TRADE_ITEM_GOLD)
			if iGold > iAmountAvailable then
				iGold = iAmountAvailable
				Controls.UsGoldAmount:SetText(iGold)
			end

			g_Deal:ChangeGoldTrade( g_iUs, iGold )

			local toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iUs, -1) ) -- This is -1 because we're not changing anything right now
			Controls.UsTableGold:SetToolTipString( toolTip )

		-- Gold from them
		else

			iAmountAvailable = g_Deal:GetGoldAvailable(g_iThem, TradeableItems.TRADE_ITEM_GOLD)
			if iGold > iAmountAvailable then
				iGold = iAmountAvailable
				Controls.ThemGoldAmount:SetText(iGold)
			end

			g_Deal:ChangeGoldTrade( g_iThem, iGold )

			local toolTip = Locale.ConvertTextKey( "TXT_KEY_DIPLO_CURRENT_GOLD", g_Deal:GetGoldAvailable(g_iThem, -1 ) ) -- This is -1 because we're not changing anything right now
			Controls.ThemTableGold:SetToolTipString( toolTip )
		end
	end
	Controls.UsGoldAmount:RegisterCallback( ChangeGoldAmount )
	Controls.UsGoldAmount:SetVoid1( 1 )
	Controls.ThemGoldAmount:RegisterCallback( ChangeGoldAmount )
	Controls.ThemGoldAmount:SetVoid1( 0 )
end

-----------------------------------------------------------------------------------------------------------------------
-- Gold Per Turn Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddGoldPerTurnTrade( playerID, player, goldPerTurn )
		g_Deal:AddGoldPerTurnTrade( playerID, math_min( goldPerTurn, player:CalculateGoldRate() ), g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketGoldPerTurn:RegisterCallback( Mouse.eLClick, function() AddGoldPerTurnTrade( g_iUs, g_pUs, UI.ShiftKeyDown() and math_huge or 1 ) end )
	Controls.ThemPocketGoldPerTurn:RegisterCallback( Mouse.eLClick, function() AddGoldPerTurnTrade( g_iThem, g_pThem, UI.ShiftKeyDown() and math_huge or 2 ) end )

	function RemoveGoldPerTurnTrade( playerID )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_GOLD_PER_TURN, playerID )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableGoldPerTurnButton:RegisterCallback( Mouse.eLClick, function() RemoveGoldPerTurnTrade( g_iUs ) end )
	Controls.ThemTableGoldPerTurnButton:RegisterCallback( Mouse.eLClick, function() RemoveGoldPerTurnTrade( g_iThem ) end )

	local function ChangeGoldPerTurnTrade( string, control, playerID, player, parentControl )
		local goldRate = player:CalculateGoldRate()
		local goldPerTurn = math_min( tonumber( string ) or 0, goldRate )
		control:SetText( goldPerTurn )

		g_Deal:ChangeGoldPerTurnTrade( playerID, goldPerTurn, g_iDealDuration )
		parentControl:LocalizeAndSetToolTip( "TXT_KEY_DIPLO_CURRENT_GPT", goldRate - goldPerTurn )
	end
	Controls.UsGoldPerTurnAmount:RegisterCallback( function( string, control ) ChangeGoldPerTurnTrade( string, control, g_iUs, g_pUs, Controls.UsTableGoldPerTurn ) end )
	Controls.ThemGoldPerTurnAmount:RegisterCallback( function( string, control ) ChangeGoldPerTurnTrade( string, control, g_iThem, g_pThem, Controls.ThemTableGoldPerTurn ) end )
end
-----------------------------------------------------------------------------------------------------------------------
-- Allow Embassy Handlers
-----------------------------------------------------------------------------------------------------------------------
if gk_mode then
	local function AddAllowEmbassy( playerID )
		g_Deal:AddAllowEmbassy( playerID )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketAllowEmbassy:RegisterCallback( Mouse.eLClick, function() AddAllowEmbassy( g_iUs ) end )
	Controls.ThemPocketAllowEmbassy:RegisterCallback( Mouse.eLClick, function() AddAllowEmbassy( g_iThem ) end )

	local function RemoveAllowEmbassy( playerID )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_ALLOW_EMBASSY, playerID )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableAllowEmbassy:RegisterCallback( Mouse.eLClick, function() RemoveAllowEmbassy( g_iUs ) end )
	Controls.ThemTableAllowEmbassy:RegisterCallback( Mouse.eLClick, function() RemoveAllowEmbassy( g_iThem ) end )

end

-----------------------------------------------------------------------------------------------------------------------
-- Declaration of Friendship Handlers
-----------------------------------------------------------------------------------------------------------------------
if Controls.UsTableDoF and Controls.ThemTableDoF and Controls.UsPocketDoF and Controls.ThemPocketDoF then
	local function AddDeclarationOfFriendship()
		-- The Declaration of Friendship must be on both sides of the deal
		g_Deal:AddDeclarationOfFriendship( g_iUs )
		g_Deal:AddDeclarationOfFriendship( g_iThem )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketDoF:RegisterCallback( Mouse.eLClick, AddDeclarationOfFriendship )
	Controls.ThemPocketDoF:RegisterCallback( Mouse.eLClick, AddDeclarationOfFriendship )

	local function RemoveDeclarationOfFriendship()
		-- If removing the Declaration of Friendship, it must be removed from both sides
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableDoF:RegisterCallback( Mouse.eLClick, RemoveDeclarationOfFriendship )
	Controls.ThemTableDoF:RegisterCallback( Mouse.eLClick, RemoveDeclarationOfFriendship )
end

-----------------------------------------------------------------------------------------------------------------------
-- Open Borders Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddOpenBorders( playerID )

		g_Deal:AddOpenBorders( playerID, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketOpenBorders:RegisterCallback( Mouse.eLClick, function() AddOpenBorders( g_iUs ) end )
	Controls.ThemPocketOpenBorders:RegisterCallback( Mouse.eLClick, function() AddOpenBorders( g_iThem ) end )

	local function RemoveOpenBorders( playerID )

		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_OPEN_BORDERS, playerID )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableOpenBorders:RegisterCallback( Mouse.eLClick, function() RemoveOpenBorders( g_iUs ) end )
	Controls.ThemTableOpenBorders:RegisterCallback( Mouse.eLClick, function() RemoveOpenBorders( g_iThem ) end )
end

-----------------------------------------------------------------------------------------------------------------------
-- Defensive Pact Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddDefensivePact()

		-- Defensive Pact is required on both sides

		g_Deal:AddDefensivePact( g_iUs, g_iDealDuration )
		g_Deal:AddDefensivePact( g_iThem, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketDefensivePact:RegisterCallback( Mouse.eLClick, AddDefensivePact )
	Controls.ThemPocketDefensivePact:RegisterCallback( Mouse.eLClick, AddDefensivePact )

	local function RemoveDefensivePact()
		-- Remove from BOTH sides of the table
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_DEFENSIVE_PACT, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableDefensivePact:RegisterCallback( Mouse.eLClick, RemoveDefensivePact )
	Controls.ThemTableDefensivePact:RegisterCallback( Mouse.eLClick, RemoveDefensivePact )
end

-----------------------------------------------------------------------------------------------------------------------
-- Research Agreement Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddResearchAgreement()

		-- Research Agreement is required on both sides
		g_Deal:AddResearchAgreement( g_iUs, g_iDealDuration )
		g_Deal:AddResearchAgreement( g_iThem, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketResearchAgreement:RegisterCallback( Mouse.eLClick, AddResearchAgreement )
	Controls.ThemPocketResearchAgreement:RegisterCallback( Mouse.eLClick, AddResearchAgreement )

	local function RemoveResearchAgreement()
		-- Remove from BOTH sides of the table
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_RESEARCH_AGREEMENT, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableResearchAgreement:RegisterCallback( Mouse.eLClick, RemoveResearchAgreement )
	Controls.ThemTableResearchAgreement:RegisterCallback( Mouse.eLClick, RemoveResearchAgreement )
end

-----------------------------------------------------------------------------------------------------------------------
-- Trade Agreement Handlers
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddTradeAgreement()

		-- Trade Agreement is required on both sides

		g_Deal:AddTradeAgreement( g_iUs, g_iDealDuration )
		g_Deal:AddTradeAgreement( g_iThem, g_iDealDuration )
		DoUIDealChangedByHuman()
	end
	Controls.UsPocketTradeAgreement:RegisterCallback( Mouse.eLClick, AddTradeAgreement )
	Controls.ThemPocketTradeAgreement:RegisterCallback( Mouse.eLClick, AddTradeAgreement )

	local function RemoveTradeAgreement()
		-- Remove from BOTH sides of the table
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iUs )
		g_Deal:RemoveByType( TradeableItems.TRADE_ITEM_TRADE_AGREEMENT, g_iThem )
		DoUIDealChangedByHuman( true )
	end
	Controls.UsTableTradeAgreement:RegisterCallback( Mouse.eLClick, RemoveTradeAgreement )
	Controls.ThemTableTradeAgreement:RegisterCallback( Mouse.eLClick, RemoveTradeAgreement )
end

-----------------------------------------------------------------------------------------------------------------------
-- Handle the strategic and luxury resources
-----------------------------------------------------------------------------------------------------------------------
do
	local function AddResourceTrade( playerID, resourceID )

		local resourceQuantity = Game.GetResourceUsageType(resourceID) == ResourceUsageTypes.RESOURCEUSAGE_STRATEGIC and UI.ShiftKeyDown() and math_huge or 1

		if gk_mode then
			resourceQuantity = math_min( g_Deal:GetNumResource( playerID, resourceID ), resourceQuantity )
		else
			resourceQuantity = math_min( Players[playerID]:GetNumResourceAvailable( resourceID, false ), resourceQuantity )
		end
		g_Deal:AddResourceTrade( playerID, resourceID, resourceQuantity, g_iDealDuration )
		DoUIDealChangedByHuman()
	end

	local function AddResourceTradeUs( resourceID )
		AddResourceTrade( g_iUs, resourceID )
	end

	local function AddResourceTradeThem( resourceID )
		AddResourceTrade( g_iThem, resourceID )
	end

	local function RemoveResourceTrade( resourceID )
		g_Deal:RemoveResourceTrade( resourceID )
		DoUIDealChangedByHuman( true )
	end

	local function AddResourceInstance( resource, instanceName, stack, list, clickAction, changeAction )
		local instance = {}
		ContextPtr:BuildInstanceForControl( instanceName, instance, stack )
		instance.Button:SetText( resource.IconString .. Locale.ConvertTextKey( resource.Description ) )
		instance.Button:SetVoid1( resource.ID )
		instance.Button:RegisterCallback( Mouse.eLClick, clickAction )
		list[ resource.ID ] = instance
		if changeAction then
			instance.AmountEdit:SetVoid1( resource.ID )
			instance.AmountEdit:RegisterCallback( changeAction )
		end
	end

	local function ChangeResourceAmount( string, control, playerID )

		local resourceID = control:GetVoid1()
		local resourceQuantity = tonumber(string) or 0

		-- Can't offer more than someone has
		if gk_mode then
			resourceQuantity = math_min( g_Deal:GetNumResource( playerID, resourceID ), resourceQuantity )
		else
			resourceQuantity = math_min( Players[ playerID ]:GetNumResourceAvailable( resourceID, false ), resourceQuantity )
		end

		control:SetText( resourceQuantity )

		g_Deal:ChangeResourceTrade( playerID, resourceID, resourceQuantity, g_iDealDuration )
	end

	local function ChangeResourceAmountUs( string, control )
		ChangeResourceAmount( string, control, g_iUs )
	end

	local function ChangeResourceAmountThem( string, control )
		ChangeResourceAmount( string, control, g_iThem )
	end

	--print( "adding strategic resources" )
	for row in GameInfo.Resources( "ResourceUsage = 1" ) do
		AddResourceInstance( row, "PocketResource", Controls.UsPocketStrategicStack, g_UsPocketResources, AddResourceTradeUs )
		AddResourceInstance( row, "PocketResource", Controls.ThemPocketStrategicStack, g_ThemPocketResources, AddResourceTradeThem )
		AddResourceInstance( row, "TableStrategic", Controls.UsTableStrategicStack, g_UsTableResources, RemoveResourceTrade, ChangeResourceAmountUs )
		AddResourceInstance( row, "TableStrategic", Controls.ThemTableStrategicStack, g_ThemTableResources, RemoveResourceTrade, ChangeResourceAmountThem )
	end
	--print( "adding luxuries" )
	for row in GameInfo.Resources( "ResourceUsage = 2" ) do
		AddResourceInstance( row, "PocketResource", Controls.UsPocketLuxuryStack, g_UsPocketResources, AddResourceTradeUs )
		AddResourceInstance( row, "PocketResource", Controls.ThemPocketLuxuryStack, g_ThemPocketResources, AddResourceTradeThem )
		AddResourceInstance( row, "TableLuxury", Controls.UsTableLuxuryStack, g_UsTableResources, RemoveResourceTrade )
		AddResourceInstance( row, "TableLuxury", Controls.ThemTableLuxuryStack, g_ThemTableResources, RemoveResourceTrade )
	end
end

if bnw_mode then
	-----------------------------------------------------------------------------------------------------------------------
	-- Handle votes
	-----------------------------------------------------------------------------------------------------------------------

	-- Update a global table, refer to proposal and vote pair as a single integer index because of SetVoids limit
	local function UpdateLeagueVotes()
		g_LeagueVoteList = {}

		local activeLeague = Game.GetNumActiveLeagues() > 0 and Game.GetActiveLeague()
		if activeLeague then
			local tEnactProposals = activeLeague:GetEnactProposals()
			for i,t in ipairs(tEnactProposals) do
				local iVoterDecision = GameInfo.ResolutionDecisions[GameInfo.Resolutions[t.Type].VoterDecision].ID
				local tVoterChoices = activeLeague:GetChoicesForDecision(iVoterDecision)
				for i,iChoice in ipairs(tVoterChoices) do
					local tLeagueVote = { Type = t.Type, VoteDecision = iVoterDecision, ID = t.ID, ProposerChoice = t.ProposerDecision, VoteChoice = iChoice, Repeal = false }
					table_insert(g_LeagueVoteList, tLeagueVote)
				end
			end

			local tRepealProposals = activeLeague:GetRepealProposals()
			for i,t in ipairs(tRepealProposals) do
				local iVoterDecision = GameInfo.ResolutionDecisions["RESOLUTION_DECISION_REPEAL"].ID --antonjs: temp
				local tVoterChoices = activeLeague:GetChoicesForDecision(iVoterDecision)
				for i,iChoice in ipairs(tVoterChoices) do
					local tLeagueVote = { Type = t.Type, VoteDecision = iVoterDecision, ID = t.ID, ProposerChoice = t.ProposerDecision, VoteChoice = iChoice, Repeal = true }
					table_insert(g_LeagueVoteList, tLeagueVote)
				end
			end
		end
	end

	function GetLeagueVoteIndexFromData( voteID, voteChoice, bRepeal)
		for index, resolution in ipairs(g_LeagueVoteList) do
			if resolution.ID == voteID and resolution.VoteChoice == voteChoice and resolution.Repeal == bRepeal then
				return index
			end
		end
	end

	function OnChoosePocketVote( fromPlayerID, voteIndex )
		local activeLeague = Game.GetNumActiveLeagues() > 0 and Game.GetActiveLeague()
		local resolution = g_LeagueVoteList[voteIndex]

		if activeLeague and resolution then
			g_Deal:AddVoteCommitment(fromPlayerID, resolution.ID, resolution.VoteChoice, activeLeague:GetCoreVotesForMember(fromPlayerID), resolution.Repeal)
			DoUIDealChangedByHuman()
		else
			print("SCRIPTING ERROR: Could not find valid vote when pocket vote was selected")
		end
	end

	local function RefreshPocketVotesRaw( IM, fromID, toID )
		UpdateLeagueVotes()
		IM:ResetInstances()

		local activeLeague = Game.GetNumActiveLeagues() > 0 and Game.GetActiveLeague()

		if activeLeague then
			for i,resolution in ipairs(g_LeagueVoteList) do
				local voteCount = activeLeague:GetCoreVotesForMember( fromID )
				if g_Deal:IsPossibleToTradeItem( fromID, toID, TradeableItems.TRADE_ITEM_VOTE_COMMITMENT, resolution.ID, resolution.VoteChoice, voteCount, resolution.Repeal ) then
					local instance = IM:GetInstance()
					instance.ProposalLabel:SetText( GetVoteText(activeLeague, i, resolution.Repeal, voteCount) )
					instance.VoteLabel:SetText( activeLeague:GetTextForChoice(resolution.VoteDecision, resolution.VoteChoice) )
					instance.Button:SetToolTipString( GetVoteTooltip(activeLeague, i, resolution.Repeal, voteCount) )
					instance.Button:SetVoids( fromID, i )
					instance.Button:RegisterCallback( Mouse.eLClick, OnChoosePocketVote )
				end
			end
		end
	end

	function RefreshPocketVotes(iIsUs)
		if iIsUs == 1 then
			RefreshPocketVotesRaw( g_UsPocketVoteIM, g_iUs, toID )
		else
			RefreshPocketVotesRaw( g_ThemPocketVoteIM, g_iThem, toID )
		end
	end

	function OnChooseTableVote(fromPlayerID, voteIndex)
		local activeLeague = Game.GetNumActiveLeagues() > 0 and Game.GetActiveLeague()
		local resolution = g_LeagueVoteList[voteIndex]
		if activeLeague and resolution then
			g_Deal:RemoveVoteCommitment(fromPlayerID, resolution.ID, resolution.VoteChoice, activeLeague:GetCoreVotesForMember(fromPlayerID), resolution.Repeal)

			DoUIDealChangedByHuman( true )
		else
			print("SCRIPTING ERROR: Could not find valid vote when pocket vote was selected")
		end
	end

	function GetVoteText(activeLeague, voteIndex, bRepeal, voteCount)
		local resolution = g_LeagueVoteList[voteIndex]
		return activeLeague and resolution and (resolution.Repeal and "[COLOR_WARNING_TEXT]" or "[COLOR_POSITIVE_TEXT]") .. activeLeague:GetResolutionName(resolution.Type, resolution.ID, resolution.ProposerChoice, false) .. "[ENDCOLOR]" or ""
	end

	function GetVoteTooltip(activeLeague, voteIndex, bRepeal, voteCount)
		local resolution = g_LeagueVoteList[voteIndex]
		return activeLeague and resolution and Locale.Lookup(bRepeal and "TXT_KEY_DIPLO_VOTE_TRADE_REPEAL_TT" or "TXT_KEY_DIPLO_VOTE_TRADE_ENACT_TT", voteCount, activeLeague:GetTextForChoice(resolution.VoteDecision, resolution.VoteChoice), (GameInfo.Resolutions[resolution.Type] or {}).Help or "") or ""

	end
end --bnw_mode

----------------------------------------------------
-- handlers for when a leader is clicked
----------------------------------------------------
local function AddThirdPartyPeace( firstPartyID, playerID )
	g_Deal:AddThirdPartyPeace( firstPartyID, Players[ playerID ]:GetTeam(), g_iPeaceDuration )
	DoUIDealChangedByHuman()
end

local function AddThirdPartyWar( firstPartyID, playerID )
	g_Deal:AddThirdPartyWar( firstPartyID, Players[ playerID ]:GetTeam() )
	DoUIDealChangedByHuman()
end

local function RemoveThirdPartyPeace( firstPartyID, playerID )

	local player = Players[ playerID ]
	-- if this is a peace negotiation
	local allyID = g_pUsTeam:IsAtWar( g_iThemTeam ) and player:IsMinorCiv() and player:GetAlly()
	if not allyID or ( allyID ~= g_iUs and allyID ~= g_iThem ) then
		--print("Removing peace deal")
		g_Deal:RemoveThirdPartyPeace( firstPartyID, player:GetTeam() )
		DoUIDealChangedByHuman( true )
	end
end

local function RemoveThirdPartyWar( firstPartyID, playerID )
	g_Deal:RemoveThirdPartyWar( firstPartyID, Players[ playerID ]:GetTeam() )
	DoUIDealChangedByHuman( true )
end


-----------------------------------------------------------------------------------------------------------------------
-- Build the THIRD_PARTY lists. These are also know as thirdparty if you're doing a text search
-- the only thing we trade in third party mode is war/peace, so make sure those aren't disabled
-----------------------------------------------------------------------------------------------------------------------

if Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_WAR )
	or Game.IsOption( GameOptionTypes.GAMEOPTION_ALWAYS_PEACE )
	or Game.IsOption( GameOptionTypes.GAMEOPTION_NO_CHANGING_WAR_PEACE )
then

	g_bEnableThirdParty = false

else
	g_bEnableThirdParty = true

	local function InitLeaderStack( stack, onButtonClick, masterButtonList )
		local buttonList = {Parent = stack}
--print("init leader stack", stack:GetID(), onButtonClick )
		for playerID = 0, GameDefines.MAX_CIV_PLAYERS-1 do

			local player = Players[ playerID ]

			if player and player:IsEverAlive() then

				local leaderName
				if player:IsHuman() then
					leaderName = player:GetNickName()
				else
					leaderName = player:GetName()
				end
				leaderName = leaderName .. " (" .. Locale.ConvertTextKey(GameInfo.Civilizations[player:GetCivilizationType()].ShortDescription) .. ")"

				local instance = {}
				ContextPtr:BuildInstanceForControl( "OtherPlayerEntry", instance, stack )
				CivIconHookup( playerID, 32, instance.CivSymbol, instance.CivIconBG, instance.CivIconShadow, false, true )
				TruncateString( instance.Name, instance.ButtonSize:GetSizeX() - instance.Name:GetOffsetX(), leaderName )
				local button = instance.Button
				button:SetVoid1( playerID )
				button:RegisterCallback( Mouse.eLClick, onButtonClick )
				buttonList[ playerID ] = button
				table_insert( masterButtonList, button )
			end
		end
		return buttonList
	end
	g_usPocketPeaceButtons = InitLeaderStack( Controls.UsPocketOtherPlayerStack, function( playerID ) AddThirdPartyPeace( g_iUs, playerID, g_usPocketPeaceButtons ) end, g_PocketLeaderButtons )
	g_usPocketWarButtons = InitLeaderStack( Controls.UsPocketLeaderStack, function( playerID ) AddThirdPartyWar( g_iUs, playerID, g_usPocketWarButtons ) end, g_PocketLeaderButtons )
	g_themPocketPeaceButtons = InitLeaderStack( Controls.ThemPocketOtherPlayerStack, function( playerID ) AddThirdPartyPeace( g_iThem, playerID, g_themPocketPeaceButtons ) end, g_PocketLeaderButtons )
	g_themPocketWarButtons = InitLeaderStack( Controls.ThemPocketLeaderStack, function( playerID ) AddThirdPartyWar( g_iThem, playerID, g_themPocketWarButtons ) end, g_PocketLeaderButtons )
	g_usTablePeaceButtons = InitLeaderStack( Controls.UsTableMakePeaceStack, function( playerID ) RemoveThirdPartyPeace( g_iUs, playerID, g_usPocketPeaceButtons ) end, g_TableLeaderButtons )
	g_usTableWarButtons = InitLeaderStack( Controls.UsTableDeclareWarStack, function( playerID ) RemoveThirdPartyWar( g_iUs, playerID, g_usPocketWarButtons ) end, g_TableLeaderButtons )
	g_themTablePeaceButtons = InitLeaderStack( Controls.ThemTableMakePeaceStack, function( playerID ) RemoveThirdPartyPeace( g_iThem, playerID, g_themPocketPeaceButtons ) end, g_TableLeaderButtons )
	g_themTableWarButtons = InitLeaderStack( Controls.ThemTableDeclareWarStack, function( playerID ) RemoveThirdPartyWar( g_iThem, playerID, g_themPocketWarButtons ) end, g_TableLeaderButtons )
end
for i, button in pairs( g_TableLeaderButtons ) do
	table_insert( g_tableControls, button )
end
for n, instance in pairs( g_UsTableResources ) do
	table_insert( g_tableControls, instance.Container )
end
for n, instance in pairs( g_ThemTableResources ) do
	table_insert( g_tableControls, instance.Container )
end
for tradeableItem, controls in pairs( g_itemControls ) do
	table_insert( g_tableControls, controls[2] )
	table_insert( g_tableControls, controls[4] )
end

Controls.UsMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.UsDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.ThemMakePeaceDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.ThemDeclareWarDuration:LocalizeAndSetText( "TXT_KEY_DIPLO_TURNS", g_iPeaceDuration )
Controls.UsTablePeaceTreaty:LocalizeAndSetText( "TXT_KEY_DIPLO_PEACE_TREATY", g_iPeaceDuration )
Controls.ThemTablePeaceTreaty:LocalizeAndSetText( "TXT_KEY_DIPLO_PEACE_TREATY", g_iPeaceDuration )

DisplayDeal()

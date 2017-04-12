--------------------------------------------------------------
-- LeagueOverview.lua
-- Author: Anton Strenger
-- DateCreated: 10/19/2012
--
-- Shows information about the members, resolutions, and votes
-- of Leagues in the game
--------------------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );
include( "CommonBehaviors" );

-----------------------------
-- Globals --
-----------------------------
-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

local m_PopupInfo = nil;
local m_ActiveEffectsManager = InstanceManager:new("ActiveEffectSummaryEntry", "EffectStack", Controls.ActiveEffectsStack );
local m_ViewAllResolutionsActiveResolutionManager = InstanceManager:new( "ViewAllResolutionsEntry", "Button", Controls.ActiveResolutionStack2 );
local m_ViewAllResolutionsInactiveResolutionManager = InstanceManager:new( "ViewAllResolutionsEntry", "Button", Controls.InactiveResolutionStack2 );
local m_ViewAllResolutionsDisabledResolutionManager = InstanceManager:new( "ViewAllResolutionsEntry", "Button", Controls.DisabledResolutionStack2 );
local m_ProposalManager = InstanceManager:new( "ProposedResolutionInstance", "ResolutionButton", Controls.ProposalStack2 );

local m_ActiveResolutionManager = InstanceManager:new( "ResolutionInstance", "ResolutionButton", Controls.ActiveResolutionStack );
local m_InactiveResolutionManager = InstanceManager:new( "ResolutionInstance", "ResolutionButton", Controls.InactiveResolutionStack );
local m_DisabledResolutionManager = InstanceManager:new( "ResolutionInstance", "ResolutionButton", Controls.DisabledResolutionStack );
local m_ResolutionChoiceManager = InstanceManager:new( "ResolutionChoiceInstance", "ResolutionChoiceButton", Controls.ResolutionChoiceStack );

local m_iCurrentLeague = -1;
local m_tResolutionChoices = {};

-- Constants
local kDebug = false;
local kStateVoteEnact = 100;
local kStateVoteRepeal = 101;
local kStateProposeEnact = 102;
local kStateProposeRepeal = 103;
local kChoiceNone = -1;
local kChoiceNo = 0;
local kChoiceYes = 1;
local kRepealDecision = "RESOLUTION_DECISION_REPEAL";
local kNoDecision = "RESOLUTION_DECISION_NONE";
local kNoPlayer = -1;

g_strEnactPrefix = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_ENACT");
g_strRepealPrefix = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_REPEAL");
g_strProposeButtonCaption = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSE_BUTTON");
g_strProposeButtonToolTip = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSE_BUTTON_TT");
g_strWorldCongressImage = "WorldCongress.dds";
g_strUnitedNationsImage = "WonderConceptUN_small.dds";


local _, screenSizeY = UIManager:GetScreenSizeVal();
if(screenSizeY <= 768) then
	local offsetX, offsetY = Controls.MainFrame:GetOffsetVal();
	Controls.MainFrame:SetOffsetVal(offsetX, offsetY + 30);
end



-----------------------------
-- General UI Handlers --
-----------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_LEAGUE_OVERVIEW ) then
		return;
	end
	
	m_PopupInfo = popupInfo;
	m_iCurrentLeague = popupInfo.Data1;
	
	UpdateFull();

	UIManager:QueuePopup( ContextPtr, PopupPriority.VoteResults );
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
        
			if(not Controls.ChooseProposalPopup:IsHidden()) then
				CloseProposeResolutionPopup();
			elseif(not Controls.ViewAllResolutionsPopup:IsHidden()) then
				CloseViewAllResolutionsPopup();
			elseif(not Controls.ResolutionChoicePopup:IsHidden()) then
				CloseResolutionChoicePopup();
			elseif(not Controls.ChangeNamePopup:IsHidden()) then
				CloseRenameLeaguePopup();
			else
				OnClose();
			end
            return true;
        end
    end 
end
ContextPtr:SetInputHandler( InputHandler );

function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function OnClose()
    Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_LEAGUE_OVERVIEW, 0);
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-- 'Active' (local human) player has changed
Events.GameplaySetActivePlayer.Add(OnClose);

function Update( bForceViewModel )
	local iActivePlayer = Game.GetActivePlayer();
	local strBackgroundImage = g_strWorldCongressImage;
	
	local pLeague;
	if (m_iCurrentLeague ~= -1 and Game.GetNumActiveLeagues() > 0) then
		pLeague = Game.GetLeague(m_iCurrentLeague);
		if(pLeague == nil) then
			print("SCRIPTING ERROR: League with ID=" .. m_iCurrentLeague .. " is nil.");
		end
	end
	
	if(pLeague ~= nil) then
		Controls.WorldCongressContent:SetHide(false);
		Controls.WorldCongressNotStartedContent:SetHide(true);
	
	
		Controls.TitleLabel:SetText(pLeague:GetName());
		local model = {
			TurnString = "",
			VictorySessionString = "",
			VictoryVotesString = "",
			VictoryTooltip = "",
		}
		
		model.CanRename = pLeague:CanChangeCustomName(iActivePlayer);
	
		model.ViewMode = "View";
		
		model.LeagueId = m_iCurrentLeague;
		model.ActivePlayer = iActivePlayer;
				

		if (not pLeague:IsInSession()) then
		
			model.TurnString = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_TURNS_UNTIL_SESSION", math.max(pLeague:GetTurnsUntilSession(), 0));
			model.ProposalsAvailable = pLeague:GetRemainingProposalsForMember(iActivePlayer);
			if(pLeague:CanPropose(iActivePlayer) and model.ProposalsAvailable > 0) then
				model.ViewMode = "Propose";
				
				local pActivePlayer = Players[iActivePlayer];
				local leaderInfo = GameInfo.Leaders[pActivePlayer:GetLeaderType()];
				model.ProposalMainIcon = {
					PortraitIndex = leaderInfo.PortraitIndex;
					IconAtlas = leaderInfo.IconAtlas;
				}
				
				local civInfo = GameInfo.Civilizations[pActivePlayer:GetCivilizationType()];
				model.ProposalSubIcon = {
					PortraitIndex = civInfo.PortraitIndex;
					IconAtlas = civInfo.IconAtlas; 
				}
			end
		else
			if (pLeague:IsInSpecialSession()) then
				model.TurnString = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_IN_SPECIAL_SESSION");
			else
				model.TurnString = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_IN_SESSION");
			end
		
			model.VotesAvailable = pLeague:GetRemainingVotesForMember(iActivePlayer);
			if(model.VotesAvailable > 0) then
				model.ViewMode = "Vote";
			end
		end
		
		if (Game.IsUnitedNationsActive()) then
			strBackgroundImage = g_strUnitedNationsImage;
			if (PreGame.IsVictory(GameInfo.Victories["VICTORY_DIPLOMATIC"].ID)) then
				model.VictorySessionString = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_SESSION", pLeague:GetTurnsUntilVictorySession());
				model.VictoryVotesString = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_VOTES", Game.GetVotesNeededForDiploVictory());
				model.VictoryTooltip = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_TT", Game.GetVotesNeededForDiploVictory());
			end
		end
		
		model.Members = GetLeagueMembers(pLeague);
		model.Proposals = GetProposals(pLeague, iActivePlayer);
		model.ActiveResolutions = GetActiveResolutions(pLeague, iActivePlayer);
		model.InactiveResolutions = GetInactiveResolutions(pLeague, iActivePlayer);

		model.ActiveEffectsSummary = pLeague:GetCurrentEffectsSummary();
		
		if (bForceViewModel) then
			View(model);
		else
			-- Only refresh the model if we need to
			-- "Vote" mode is independent from other player activity and we don't want to wipe the scratch player votes
			if (model.ViewMode ~= "Vote") then
				View(model);
			end
		end
	else
		Controls.WorldCongressContent:SetHide(true);
		Controls.WorldCongressNotStartedContent:SetHide(false);
		Controls.RenameButton:SetHide(true);
		
		local requiredTech = GameInfo.Technologies("AllowsWorldCongress == 1")();
		
		if (requiredTech == nil or Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
			Controls.LeagueNotFoundedString:LocalizeAndSetText("TXT_KEY_LEAGUE_NOT_FOUNDED_GAME_SETTINGS");
		else
			Controls.LeagueNotFoundedString:LocalizeAndSetText("TXT_KEY_LEAGUE_NOT_FOUNDED", requiredTech.Description);
		end
	end
	
	Controls.LeagueImage:SetTexture(strBackgroundImage);
	
end

function UpdateFull()
	Update(true);
end

function UpdatePartial()
	Update(false);
end
Events.SerialEventLeagueScreenDirty.Add(UpdatePartial);

function GetActiveResolutions(league, activePlayerId)
	
	local activeResolutions = {};
	
	-- Active Resolutions
	for _, t in ipairs(league:GetActiveResolutions()) do
		local resolutionType = t.Type;
		local resolutionId = t.ID;
	
		local proposerDecision = t.ProposerDecision or kChoiceNone;
		
		local canProposeRepeal = league:CanProposeRepeal(resolutionId, activePlayerId);
		local text = league:GetResolutionName(resolutionType, resolutionId, proposerDecision, false);
		local toolTip = league:GetResolutionDetails(resolutionType, activePlayerId, resolutionId, proposerDecision); 
		local proposeText = text;
		local proposeToolTip = toolTip;
		
		if(not canProposeRepeal) then
			proposeText = "[COLOR_GREY]" .. text .. "[ENDCOLOR]";
		end	
		
		local resolution = {	
			Type = resolutionType,
			Id = resolutionId,
			Disabled = not canProposeRepeal,
			Text = text,
			ToolTip = toolTip,
			ProposeText = proposeText,
			ProposeToolTip = proposeToolTip,
			ProposerDecision = t.ProposerDecision,
		}
		
		table.insert(activeResolutions, resolution);

	end

	return activeResolutions;
end

function GetInactiveResolutions(league, activePlayerId)
	
	local resolutions = {};
	
	-- Resolutions
	for _, t in ipairs(league:GetInactiveResolutions()) do
		local resolutionType = t.Type;
		local resolutionId = -1;
	
		local proposerDecision = t.ProposerDecision or kChoiceNone;
		
		local canProposeEnact = league:CanProposeEnactAnyChoice(resolutionType, activePlayerId);
		local possibleProposeEnact = league:CanProposeEnactAnyChoice(resolutionType, kNoPlayer);
		local text = league:GetResolutionName(resolutionType, resolutionId, proposerDecision, false);
		local toolTip = league:GetResolutionDetails(resolutionType, activePlayerId, resolutionId, proposerDecision);
		local proposeText = text;
		local proposeToolTip = toolTip;
		
		if(not canProposeEnact) then
			proposeText = "[COLOR_GREY]" .. text .. "[ENDCOLOR]";
		end	
		
		local resolutionDecisionType = GameInfo.Resolutions[resolutionType].ProposerDecision;
		local resolutionDecisionId = GameInfo.ResolutionDecisions[resolutionDecisionType].ID;
		local choices = league:GetChoicesForDecision(resolutionDecisionId, activePlayerId);
		
		local resolutionChoices = {};
		
		for _, choiceId in ipairs(choices) do
			local choice = {
				Id = choiceId,
				Text = league:GetTextForChoice(resolutionDecisionId, choiceId),
				ToolTip = league:GetResolutionDetails(resolutionType, activePlayerId, resolutionId, choiceId);
				Disabled = not league:CanProposeEnact(resolutionType, activePlayerId, choiceId)
			}
			
			GatherAdditionalChoiceInfoBasedOnDecisionType(resolutionDecisionType, choice);
						
			table.insert(resolutionChoices, choice);
		end
		
		local resolution = {
			Type = resolutionType,
			Id = resolutionId,
			Disabled = not canProposeEnact,
			Possible = possibleProposeEnact,
			Text = text,
			ToolTip = toolTip,
			ProposeText = proposeText,
			ProposeToolTip = proposeToolTip,
			ProposerChoices = resolutionChoices,
		}
		
		table.insert(resolutions, resolution);
	end

	return resolutions;
end

function GetLeagueMembers(league)
	
	function IsLeagueMember(player)
		return (player ~= nil and player:IsAlive() and not player:IsMinorCiv() and league:IsMember(player:GetID()));
	end
	
	
	local iActivePlayer = Game.GetActivePlayer();
	local pActivePlayer = Players[iActivePlayer];
	
	local members = {};
	if(league ~= nil) then
		local hostPlayerId = league:GetHostMember();
		for iPlayer = 0, GameDefines.MAX_MAJOR_CIVS - 1, 1 do
			local pPlayer = Players[iPlayer];
			if (pPlayer ~= nil and IsLeagueMember(pPlayer)) then
						
				local member = {};
			
				local leader = GameInfo.Leaders[pPlayer:GetLeaderType()];
				
				member.PlayerId = iPlayer;
				member.PortraitIndex = leader.PortraitIndex;
				member.IconAtlas = leader.IconAtlas;	
				member.NumVotes = league:CalculateStartingVotesForMember(iPlayer);
				if (league:IsInSession()) then
					member.NumVotes = league:GetRemainingVotesForMember(iPlayer) + league:GetSpentVotesForMember(iPlayer);
				end
				member.ToolTip = league:GetMemberDetails(iPlayer, iActivePlayer);
				member.ShowSpy = (iPlayer ~= iActivePlayer) and (pActivePlayer:IsMyDiplomatVisitingThem(iPlayer)); 
				member.ShowScroll = league:CanPropose(iPlayer);
				
				if(iPlayer == hostPlayerId) then
					table.insert(members, 1, member);
				else
					table.insert(members, member);
				end
			end
		end
	end
	
	return members;
end

function GetProposals(league, activePlayerId)
	
	local enactProposals = league:GetEnactProposals();
	local repealProposals = league:GetRepealProposals();
	local enactProposalsOnHold = league:GetEnactProposalsOnHold();
	local repealProposalsOnHold = league:GetRepealProposalsOnHold();
	
	local GetChoicesForResolution = function(resolutionType) 
		
		local resolutionChoices = nil;
		
		local decisionType = GameInfo.Resolutions[resolutionType].VoterDecision;
		if(decisionType ~= "RESOLUTION_DECISION_YES_OR_NO") then
			resolutionChoices = {};
			local decisionId = GameInfo.ResolutionDecisions[decisionType].ID;
			local choices = league:GetChoicesForDecision(decisionId, activePlayerId);
		
			for _, choiceId in ipairs(choices) do
				local choice = {
					Id = choiceId,
					Text = league:GetTextForChoice(decisionId, choiceId),
				}
				
				GatherAdditionalChoiceInfoBasedOnDecisionType(decisionType, choice);
								
				table.insert(resolutionChoices, choice);
			end
		end
		
		return resolutionChoices;
	end	
	
	local GetLeaderIcon = function(proposalPlayer)
		if(proposalPlayer ~= nil and proposalPlayer ~= -1) then
			local player = Players[proposalPlayer];
			local leader = GameInfo.Leaders[player:GetLeaderType()];
			
			return {
				PortraitIndex = leader.PortraitIndex,
				IconAtlas = leader.IconAtlas,
			};
		end
	end
	
	local proposals = {}
	for i,v in ipairs(enactProposals) do
		
		local mainIcon = GetLeaderIcon(v.ProposalPlayer);
		local choice = v.ProposerDecision or kChoiceNone;
		local proposal = {
			ID = v.ID,
			MainIcon = mainIcon,
			ProposalPlayer = v.ProposalPlayer,
			ProposerChoice = choice,
			VoterChoices = GetChoicesForResolution(v.Type),
			Direction = "Enact",
			Text = league:GetResolutionName(v.Type, v.ID, choice, false);
			ToolTip = league:GetResolutionDetails(v.Type, activePlayerId, v.ID, choice);
			OnHold = false;
		};
		
		table.insert(proposals, proposal);
	end
	
	for i,v in ipairs(repealProposals) do
		
		local mainIcon = GetLeaderIcon(v.ProposalPlayer);
		local choice = v.ProposerDecision or kChoiceNone;
		local proposal = {
			ID = v.ID,
			MainIcon = mainIcon,
			ProposalPlayer = v.ProposalPlayer,
			ProposerChoice = choice,
			VoterChoices = GetChoicesForResolution(v.Type),
			Direction = "Retract",
			Text = league:GetResolutionName(v.Type, v.ID, choice, false);
			ToolTip = league:GetResolutionDetails(v.Type, activePlayerId, v.ID, choice);
			OnHold = false;
		};
		
		table.insert(proposals, proposal);
	end
	
	for i,v in ipairs(enactProposalsOnHold) do
		
		local mainIcon = GetLeaderIcon(v.ProposalPlayer);
		local choice = v.ProposerDecision or kChoiceNone;
		local proposal = {
			ID = v.ID,
			MainIcon = mainIcon,
			ProposalPlayer = v.ProposalPlayer,
			ProposerChoice = choice,
			VoterChoices = GetChoicesForResolution(v.Type),
			Direction = "Enact",
			Text = "[COLOR_GREY]" .. league:GetResolutionName(v.Type, v.ID, choice, false) .. "[ENDCOLOR]";
			ToolTip = "[COLOR_WARNING_TEXT]" .. Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_ON_HOLD_TT") .. "[ENDCOLOR][NEWLINE][NEWLINE]" .. league:GetResolutionDetails(v.Type, activePlayerId, v.ID, choice);
			OnHold = true;
		};
		
		table.insert(proposals, proposal);
	end
	
	for i,v in ipairs(repealProposalsOnHold) do
		
		local mainIcon = GetLeaderIcon(v.ProposalPlayer);
		local choice = v.ProposerDecision or kChoiceNone;
		local proposal = {
			ID = v.ID,
			MainIcon = mainIcon,
			ProposalPlayer = v.ProposalPlayer,
			ProposerChoice = choice,
			VoterChoices = GetChoicesForResolution(v.Type),
			Direction = "Retract",
			Text = "[COLOR_GREY]" .. league:GetResolutionName(v.Type, v.ID, choice, false) .. "[ENDCOLOR]";
			ToolTip = "[COLOR_WARNING_TEXT]" .. Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_ON_HOLD_TT") .. "[ENDCOLOR][NEWLINE][NEWLINE]" .. league:GetResolutionDetails(v.Type, activePlayerId, v.ID, choice);
			OnHold = true;
		};
		
		table.insert(proposals, proposal);
	end
	
	return proposals;
end

-- Populates a choice table with additional data based on the decision type.
-- Ie Icon data for resources, or civs, etc
function GatherAdditionalChoiceInfoBasedOnDecisionType(decisionType, choice)
	if(decisionType == "RESOLUTION_DECISION_MAJOR_CIV_MEMBER" or decisionType == "RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER") then
		local player = Players[choice.Id];
		local leader = GameInfo.Leaders[player:GetLeaderType()];
		local civ = GameInfo.Civilizations[player:GetCivilizationType()];
		
		choice.MainIcon = {
			PortraitIndex = leader.PortraitIndex,
			IconAtlas = leader.IconAtlas,
		};
		
		choice.UsePlayerIdForSubIcon = true;
		choice.PlayerId = choice.Id;
	end
	
	if(decisionType == "RESOLUTION_DECISION_ANY_LUXURY_RESOURCE") then
		
		local resource = GameInfo.Resources[choice.Id];
		choice.MainIcon = {
			PortraitIndex = resource.PortraitIndex,
			IconAtlas = resource.IconAtlas
		};
		
	end
	
	if(decisionType == "RESOLUTION_DECISION_RELIGION") then
		local religion = GameInfo.Religions[choice.Id];
		choice.MainIcon = {
			PortraitIndex = religion.PortraitIndex,
			IconAtlas = religion.IconAtlas
		};
	end	
end

-----------------------------
-- View Functions
-----------------------------
function View(model)
	Controls.TurnsUntilVoteFrame:SetDisabled(true);
	Controls.TurnsUntilVote:SetText(model.TurnString);
	local frameW, frameH = Controls.TurnsUntilVoteFrame:GetSizeVal();
	local labelW, labelH = Controls.TurnsUntilVote:GetSizeVal();
	Controls.TurnsUntilVoteFrame:SetSizeVal(labelW + 100, frameH);
	Controls.TurnsUntilVoteFrame:ReprocessAnchoring();
	
	if (model.VictorySessionString ~= nil and model.VictoryVotesString ~= nil and model.VictoryTooltip ~= nil) then
		Controls.VictorySessionLabel:SetHide(false);
		Controls.VictorySessionLabel:SetText(model.VictorySessionString);
		Controls.VictorySessionLabel:SetToolTipString(model.VictoryTooltip);
		Controls.VictoryVotesLabel:SetHide(false);
		Controls.VictoryVotesLabel:SetText(model.VictoryVotesString);
		Controls.VictoryVotesLabel:SetToolTipString(model.VictoryTooltip);
	else
		Controls.VictorySessionLabel:SetHide(true);
		Controls.VictoryVotesLabel:SetHide(true);
	end
	
	Controls.RenameButton:ClearCallback(Mouse.eLClick);
	if(model.CanRename == true) then
		Controls.RenameButton:RegisterCallback(Mouse.eLClick, RenameLeague);
		Controls.RenameButton:SetHide(false);
	else
		Controls.RenameButton:SetHide(true);
	end
	
	
	ViewLeagueMembers(model.Members);
	ViewCurrentProposals(model);
	ViewActiveEffectsSummary(model.ActiveEffectsSummary);
	PopulateViewAllResolutionsPopup(model.ActiveResolutions, model.InactiveResolutions);
end

function ViewActiveEffectsSummary(activeEffects)
	m_ActiveEffectsManager:ResetInstances();
	
	for _, v in ipairs(activeEffects) do
		local instance = m_ActiveEffectsManager:GetInstance();
		instance.EffectLabel:SetText(v);
		instance.EffectStack:CalculateSize();
		instance.EffectStack:ReprocessAnchoring();
	end
	
	Controls.ActiveEffectsStack:CalculateSize();
	Controls.ActiveEffectsStack:ReprocessAnchoring();
	Controls.ActiveEffectsScrollPanel:CalculateInternalSize();

end

function ViewCurrentProposals(model)
	
	m_ProposalManager:ResetInstances();
	
	
	local voteEntries = {};
	
	for i,v in ipairs(model.Proposals) do
		local instance = m_ProposalManager:GetInstance();
		
		instance.ResolutionButton:ClearCallback(Mouse.eLClick);
		
		local prefix = "";
		if(v.Direction == "Enact") then
			prefix = g_strEnactPrefix;
		else
			prefix = g_strRepealPrefix;
		end
		
		instance.ResolutionName:SetText(prefix .. v.Text);
		instance.ResolutionButton:SetToolTipString(v.ToolTip);
		instance.ResolutionButton:SetDisabled(true);
		
		if(v.MainIcon ~= nil) then
			IconHookup(v.MainIcon.PortraitIndex, 64, v.MainIcon.IconAtlas, instance.Icon);
		else
			IconHookup(0, 64, "INTL_PROJECT_ATLAS_EXP2", instance.Icon);
		end
		
		instance.SubIconFrame:SetHide(true);
		if (v.ProposalPlayer ~= nil and v.ProposalPlayer ~= -1) then
			CivIconHookup(v.ProposalPlayer, 32, instance.SubIcon, instance.SubIconBG, instance.SubIconShadow, true, true);
			instance.SubIconFrame:SetHide(false);
		end
		
		if(v.AltIcon ~= nil) then
			IconHookup(v.AltIcon.PortraitIndex, 64, v.AltIcon.IconAtlas, instance.AltIcon);
			instance.AltIconFrame:SetHide(false);
		else
			instance.AltIconFrame:SetHide(true);
		end
		
		instance.VoteStatus:SetHide(not instance.AltIconFrame:IsHidden());
		
		if(model.ViewMode == "Vote" and not v.OnHold) then			
			instance.VoteBox:SetHide(false);
			table.insert(voteEntries, {
				Votes = 0,
				ResolutionId = v.ID,
				VoterChoices = v.VoterChoices,
				Direction = v.Direction,
				Instance = instance,
			});						
		else
			instance.VoteBox:SetHide(true);
		end
		
		local width, _ = instance.ResolutionButton:GetSizeVal();
		local _, height = instance.ResolutionName:GetSizeVal();
		local newheight = math.max(64, height + 30);
		instance.ResolutionButton:SetSizeVal(width, newheight);
	end
		
	Controls.ResetButton:ClearCallback( Mouse.eLClick );
	Controls.CommitButton:ClearCallback( Mouse.eLClick );
	
	if(model.ViewMode == "View") then
		Controls.ResetButton:SetHide(true);
		Controls.CommitButton:SetHide(true);
		Controls.VotesRemainingLabel:SetHide(true);		
		
	elseif(model.ViewMode == "Propose") then
	
		Controls.VotesRemainingLabel:SetHide(true);
		
		ProposalController:Initialize{
			LeagueId = model.LeagueId,
			ActivePlayerId = model.ActivePlayer,
			ResetButton = Controls.ResetButton,
			CommitButton = Controls.CommitButton,
			InstanceManager = m_ProposalManager,
			NumProposals = model.ProposalsAvailable,
			ProposalMainIcon = model.ProposalMainIcon,
			ProposalSubIcon = model.ProposalSubIcon,
			ActiveResolutions = model.ActiveResolutions,
			InactiveResolutions = model.InactiveResolutions
		};
		
	elseif(model.ViewMode == "Vote") then
	
		VoteController:Initialize{
			LeagueId = model.LeagueId,
			ActivePlayerId = model.ActivePlayer,
			ResetButton = Controls.ResetButton,
			CommitButton = Controls.CommitButton,	
			VotesRemainingLabel = Controls.VotesRemainingLabel,
			TotalVotes = model.VotesAvailable,	
			VoteEntries = voteEntries,
		};		
	end
	
	Controls.ProposalStack2:CalculateSize();
	Controls.ProposalStack2:ReprocessAnchoring();
	Controls.ProposalScrollPanel2:CalculateInternalSize();
end

function ViewLeagueMembers(members)
	local maxMemberControls = 61;
	local maxFullMemberControls = 33;
	
	for i = 1, maxMemberControls, 1 do
		local buttonControlID = "MemberButton" .. i;
		local control = Controls[buttonControlID];
		if(control ~= nil) then
			control:SetHide(true);
		end
	end

	-- If we have an even number of members, use the position under the leader, otherwise don't (to keep things balanced)
	local offset = (#members % 2)
	
	for i, member in ipairs(members) do
		if (i > maxMemberControls) then return end
		
		local buttonControl;
		local iconSize;
		local iconControl; 
		local votesControl;
		local scrollControl;
		local spyControl;
		
		local subIconSize;
		local subIcon;
		local subIconBG;
		local subIconShadow;
		local subIconHighlight;
			
		-- Assume the first member is the leader.
		if (i == 1) then
			buttonControl = Controls.LeaderButton;
			iconControl = Controls.LeaderIcon;
			iconSize = 128;
			votesControl = Controls.LeaderVotes;
			scrollControl = Controls.LeaderScroll;
			spyControl = Controls.LeaderSpy;
			
			subIconSize = 45;
			subIcon = Controls.LeaderSubIcon;
			subIconBG = Controls.LeaderSubIconBG;
			subIconShadow = Controls.LeaderSubIconShadow;
			subIconHighlight = Controls.LeaderSubIconHighlight;
		elseif (i <= maxFullMemberControls) then
			local index = tostring(i + offset - 1);
			buttonControl = Controls["MemberButton" .. index];
			iconControl = Controls["MemberIcon" .. index];
			iconSize = 64;
			votesControl = Controls["MemberVotes" .. index];
			scrollControl = Controls["MemberScroll" .. index];
			spyControl = Controls["MemberSpy".. index];
			
			subIconSize = 32;
			subIcon = Controls["MemberSubIcon" .. index];
			subIconBG = Controls["MemberSubIconBG" .. index];
			subIconShadow = Controls["MemberSubIconShadow" .. index];
			subIconHighlight = Controls["MemberSubIconHighlight" .. index];
		else
			local index = tostring(i + offset - 1);
			buttonControl = Controls["MemberButton" .. index];
			iconControl = nil
			
			subIconSize = 32;
			subIcon = Controls["MemberSubIcon" .. index];
			subIconBG = Controls["MemberSubIconBG" .. index];
			subIconShadow = Controls["MemberSubIconShadow" .. index];
			subIconHighlight = Controls["MemberSubIconHighlight" .. index];
		end
	
		buttonControl:SetHide(false);
		buttonControl:RegisterCallback( Mouse.eLClick, function() OnLeaderSelected(member.PlayerId); end );
		CivIconHookup(member.PlayerId, subIconSize, subIcon, subIconBG, subIconShadow, true, true, subIconHighlight);

		if (iconControl) then
			IconHookup(member.PortraitIndex, iconSize, member.IconAtlas, iconControl);

			votesControl:SetText(member.NumVotes);
			scrollControl:SetHide(not member.ShowScroll);
			spyControl:SetHide(not member.ShowSpy);

			buttonControl:SetToolTipString(member.ToolTip);
		else
			buttonControl:SetToolTipString(string.format("Votes: %i[NEWLINE]", member.NumVotes) .. member.ToolTip);
		end
	end

	Controls.MemberStack:CalculateSize();
	Controls.MemberStack:ReprocessAnchoring();
end

function PopulateProposeResolutionPopup(activeResolutions, inactiveResolutions, currentProposals, selectFunction)

	m_ActiveResolutionManager:ResetInstances();
	m_InactiveResolutionManager:ResetInstances();
	m_DisabledResolutionManager:ResetInstances();
	
	local tick = true;
	
	activeResolutions = activeResolutions or {};
	for _, v in ipairs(activeResolutions) do
		local instance = m_ActiveResolutionManager:GetInstance();
		v.Direction = "Retract";
		instance.ResolutionName:SetText(v.ProposeText);
		instance.ResolutionButton:SetToolTipString(v.ProposeToolTip);
		instance.ResolutionButton:SetDisabled(v.Disabled);
		
		local color = tick and ltBlue or dkBlue;
		tick = not tick;
			
		instance.Box:SetColorVal(unpack(color));
		
		instance.ResolutionButton:RegisterCallback(Mouse.eLClick, function() 
			CloseProposeResolutionPopup();
			selectFunction(v) 
		end);
	end
	
	inactiveResolutions = inactiveResolutions or {};
	for _, v in ipairs(inactiveResolutions) do
		if (not v.Disabled) then
			local instance = m_InactiveResolutionManager:GetInstance();
			v.Direction = "Enact";
			instance.ResolutionName:SetText(v.ProposeText);
			instance.ResolutionButton:SetToolTipString(v.ProposeToolTip);
			instance.ResolutionButton:SetDisabled(v.Disabled);
			
			local color = tick and ltBlue or dkBlue;
			tick = not tick;
				
			instance.Box:SetColorVal(unpack(color));
			
			instance.ResolutionButton:RegisterCallback(Mouse.eLClick, function() 
				CloseProposeResolutionPopup();
				selectFunction(v);
			end);
		end
	end
	for _, v in ipairs(inactiveResolutions) do
		if (v.Disabled) then
			local instance = m_DisabledResolutionManager:GetInstance();
			v.Direction = "Enact";
			instance.ResolutionName:SetText(v.ProposeText);
			instance.ResolutionButton:SetToolTipString(v.ProposeToolTip);
			instance.ResolutionButton:SetDisabled(v.Disabled);
			
			local color = tick and ltBlue or dkBlue;
			tick = not tick;
				
			instance.Box:SetColorVal(unpack(color));
			
			instance.ResolutionButton:RegisterCallback(Mouse.eLClick, function() 
				CloseProposeResolutionPopup();
				selectFunction(v);
			end);
		end
	end
	
	Controls.ActiveResolutionStack:CalculateSize();
	Controls.ActiveResolutionStack:ReprocessAnchoring();
	Controls.InactiveResolutionStack:CalculateSize();
	Controls.InactiveResolutionStack:ReprocessAnchoring();
	Controls.DisabledResolutionStack:CalculateSize();
	Controls.DisabledResolutionStack:ReprocessAnchoring();
	Controls.ChooseProposalStack:CalculateSize();
	Controls.ChooseProposalStack:ReprocessAnchoring();
	Controls.ChooseProposalScrollPanel:CalculateInternalSize();
		
end

function PopulateResolutionChoicePopup(proposerChoices, selectFunction)
	m_ResolutionChoiceManager:ResetInstances();
	
	local tick = true;
	
	for _, v in ipairs(proposerChoices) do
		local instance = m_ResolutionChoiceManager:GetInstance();
		
		if(v.MainIcon ~= nil) then
			IconHookup(v.MainIcon.PortraitIndex, 64, v.MainIcon.IconAtlas, instance.Icon);
			instance.IconFrame:SetHide(false);
		else
			instance.IconFrame:SetHide(true);
		end
		
		instance.SubIconFrame:SetHide(true);
		if(v.UsePlayerIdForSubIcon) then
			
			CivIconHookup(v.PlayerId, 32, instance.SubIcon, instance.SubIconBG, instance.SubIconShadow, true, true);

			instance.SubIconHighlight:SetHide(false);
			instance.SubIconFrame:SetHide(false);
		else
			if(v.SubIcon ~= nil) then
				instance.SubIconBG:SetHide(true);
				instance.SubIconShadow:SetHide(true);
				instance.SubIconHighlight:SetHide(true);
				
				IconHookup(v.SubIcon.PortraitIndex, 32, v.SubIcon.IconAtlas, instance.SubIcon);
				instance.SubIconFrame:SetHide(false);
			end
		end
		
		local sText = v.Text;
		if (v.Disabled) then
			sText = "[COLOR_GREY]" .. sText .. "[ENDCOLOR]";
		end
		instance.ResolutionChoiceName:SetText(sText);
		instance.ResolutionChoiceButton:SetToolTipString(v.ToolTip);
		instance.ResolutionChoiceButton:SetDisabled(v.Disabled);
		
		instance.ResolutionChoiceStack:CalculateSize();
		instance.ResolutionChoiceStack:ReprocessAnchoring();
		
		local color = tick and ltBlue or dkBlue;
		tick = not tick;
			
		instance.Box:SetColorVal(unpack(color));
		
		instance.ResolutionChoiceButton:RegisterCallback(Mouse.eLClick, function() 
			CloseResolutionChoicePopup();
			selectFunction(v.Id) 
		end);
	end
	
	Controls.ResolutionChoiceStack:CalculateSize();
	Controls.ResolutionChoiceStack:ReprocessAnchoring();
	Controls.ChoiceScrollPanel:CalculateInternalSize();
end

function PopulateViewAllResolutionsPopup(activeResolutions, inactiveResolutions)
	m_ViewAllResolutionsActiveResolutionManager:ResetInstances();
	m_ViewAllResolutionsInactiveResolutionManager:ResetInstances();
	m_ViewAllResolutionsDisabledResolutionManager:ResetInstances();
	
	local tick = true;	
	activeResolutions = activeResolutions or {};
	for _, v in ipairs(activeResolutions) do
		local instance = m_ViewAllResolutionsActiveResolutionManager:GetInstance();
		instance.ResolutionName:SetText(v.Text);
		instance.Button:SetToolTipString(v.ToolTip);
		instance.Button:SetDisabled(true);
		
		local color = tick and ltBlue or dkBlue;
		tick = not tick;
			
		instance.Box:SetColorVal(unpack(color));
	end
	
	inactiveResolutions = inactiveResolutions or {};
	for _, v in ipairs(inactiveResolutions) do
		if (v.Possible) then
			local instance = m_ViewAllResolutionsInactiveResolutionManager:GetInstance();
			instance.ResolutionName:SetText(v.Text);
			instance.Button:SetToolTipString(v.ToolTip);
			instance.Button:SetDisabled(true);
			
			local color = tick and ltBlue or dkBlue;
			tick = not tick;
				
			instance.Box:SetColorVal(unpack(color));
		end
	end
	for _, v in ipairs(inactiveResolutions) do
		if (not v.Possible) then
			local instance = m_ViewAllResolutionsDisabledResolutionManager:GetInstance();
			instance.ResolutionName:SetText(v.Text);
			instance.Button:SetToolTipString(v.ToolTip);
			instance.Button:SetDisabled(true);
			
			local color = tick and ltBlue or dkBlue;
			tick = not tick;
				
			instance.Box:SetColorVal(unpack(color));
		end
	end
	
	Controls.ActiveResolutionStack2:CalculateSize();
	Controls.ActiveResolutionStack2:ReprocessAnchoring();
	Controls.InactiveResolutionStack2:CalculateSize();
	Controls.InactiveResolutionStack2:ReprocessAnchoring();
	Controls.DisabledResolutionStack2:CalculateSize();
	Controls.DisabledResolutionStack2:ReprocessAnchoring();
	Controls.ViewAllResolutionsScrollPanel:CalculateInternalSize();
	
	Controls.ViewAllResolutionsButton:RegisterCallback(Mouse.eLClick, function()
		Controls.ViewAllResolutionsPopup:SetHide(false);
		Controls.BGBlock:SetHide(true);
	end);	
end

function RenameLeague()

	local pLeague = Game.GetLeague(m_iCurrentLeague);
	local defaultName = pLeague:GetName();

	Controls.NewName:SetText(defaultName);
	Controls.ChangeNameError:SetHide(true);
	Controls.ChangeNamePopup:SetHide(false);
end

function OnChangeNameOK()

	local name = Controls.NewName:GetText();
	if(Locale.IsNilOrEmpty(name)) then
		Controls.ChangeNameError:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_ERROR_NAME_EMPTY");
		Controls.ChangeNameError:SetHide(false);
	else
		Network.SendLeagueEditName(m_iCurrentLeague, Game.GetActivePlayer(), name);
		Controls.ChangeNameError:SetHide(true);
		Controls.TitleLabel:SetText(name);
		Controls.ChangeNamePopup:SetHide(true);
	end
end
Controls.ChangeNameOKButton:RegisterCallback(Mouse.eLClick, OnChangeNameOK);

function CloseRenameLeaguePopup()
	Controls.ChangeNamePopup:SetHide(true);
end
Controls.ChangeNameCancelButton:RegisterCallback(Mouse.eLClick, CloseRenameLeaguePopup);

function OnLeaderSelected(ePlayer)
	if (not Players[Game.GetActivePlayer()]:IsTurnActive() or Game.IsProcessingMessages()) then
		return;
	end
	
	if (Game.GetActivePlayer() == ePlayer) then
		return;
	end

    if (Players[ePlayer] ~= nil) then
		if (Players[ePlayer]:IsHuman()) then
			OnClose();
			Events.OpenPlayerDealScreenEvent( ePlayer );
		else
			UI.SetRepeatActionPlayer(ePlayer);
			UI.ChangeStartDiploRepeatCount(1);
    		Players[ePlayer]:DoBeginDiploWithHuman();
		end
	end
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function CloseViewAllResolutionsPopup()
	Controls.ViewAllResolutionsPopup:SetHide(true);
	Controls.BGBlock:SetHide(false);	
end
Controls.ViewAllResolutionsCloseButton:RegisterCallback( Mouse.eLClick, CloseViewAllResolutionsPopup);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function CloseProposeResolutionPopup()
	Controls.ChooseProposalPopup:SetHide(true);
	Controls.BGBlock:SetHide(false);	
end
Controls.ChooseProposalCloseButton:RegisterCallback( Mouse.eLClick, CloseProposeResolutionPopup);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function CloseResolutionChoicePopup()
	Controls.ResolutionChoicePopup:SetHide(true);
	Controls.BGBlock:SetHide(false);	
end
Controls.CloseChoicePopupButton:RegisterCallback( Mouse.eLClick, CloseResolutionChoicePopup );



------------------------------
-- Vote Behavior Controller --
------------------------------
VoteController = {
	TotalVotes = 0,
	VotesAvailable = 0,
	VoteInstances = {},	
};
	
function VoteController:Initialize(data)
	local controller = self;
	
	self.LeagueId = data.LeagueId;
	self.ActivePlayerId = data.ActivePlayerId;
	self.TotalVotes = data.TotalVotes;
	self.VotesAvailable = data.TotalVotes;
	self.VoteEntries = data.VoteEntries;
	self.CommitButton = data.CommitButton;
	self.ResetButton = data.ResetButton;
	self.VotesRemainingLabel = data.VotesRemainingLabel;
	
		
	self.ResetButton:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_RESET_VOTES");
	self.ResetButton:RegisterCallback(Mouse.eLClick, function()
		controller:ResetVotes();
	end);
	self.ResetButton:SetDisabled(true);
	self.ResetButton:SetHide(false);
		
	self.CommitButton:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_COMMIT_VOTES");
	self.CommitButton:RegisterCallback(Mouse.eLClick, function() 
		controller:CommitVotes();
	end);
	
	self.CommitButton:SetDisabled(true);		
	self.CommitButton:SetHide(false);

	self.VotesRemainingLabel:SetHide(false);	
	
	for _, entry in ipairs(self.VoteEntries) do
	
		if(entry.VoterChoices == nil) then
			entry.VoteController = VoteYesNoController.new(controller, entry);
		else
			entry.VoteController = VoteMajorCivController.new(controller, entry);	
		end
	end

	self:ResetVotes();
end

function VoteController:ResetVotes()
	self.VotesAvailable = self.TotalVotes;
	self.CommitButton:SetHide(false);
	self.ResetButton:SetHide(false);
		
	for _, entry in ipairs(self.VoteEntries) do
		entry.Votes = 0;
		entry.Choice = nil;
		entry.VoteController:UpdateVoteInstance();
	end
		
	self:UpdateVoteState();
end
	
function VoteController:CommitVotes()

	local controller = self;
	local bConfirmUnused = (controller.VotesAvailable > 0);
	local confirmAction = function()
		for _, entry in ipairs(controller.VoteEntries) do
	
			if(entry.Votes ~= 0) then
				local choice = entry.Choice or kChoiceNone;
				local votes = math.abs(entry.Votes);
				
				if(entry.Direction == "Enact") then		
					Network.SendLeagueVoteEnact(controller.LeagueId, entry.ResolutionId, controller.ActivePlayerId, votes, choice);	
				else
					Network.SendLeagueVoteRepeal(controller.LeagueId, entry.ResolutionId, controller.ActivePlayerId, votes, choice);	
				end
			end
		end
		
		if (controller.VotesAvailable > 0) then
			Network.SendLeagueVoteAbstain(controller.LeagueId, controller.ActivePlayerId, controller.VotesAvailable);
		end
	end

	CommitConfirm(confirmAction, bConfirmUnused);
end
	
function VoteController:UpdateVoteState()
	local votesAvailable = self.VotesAvailable or 0;
	self.VotesRemainingLabel:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_VOTES_REMAINING", votesAvailable);		
	
	for _, voteEntry in ipairs(self.VoteEntries) do
		voteEntry.VoteController:UpdateVoteState(votesAvailable);
	end
	
	self.ResetButton:SetDisabled(self.VotesAvailable == self.TotalVotes);
	self.CommitButton:SetDisabled(false);
end

-------------------------------------
-- Vote Decision Yes/No Controller --
-------------------------------------
VoteYesNoController = {};
	
function VoteYesNoController.new(voteController, entry)
	
	local entryController = {
		VoteController = voteController,
		Entry = entry,
		
		UpdateVoteInstance = VoteYesNoController.UpdateVoteInstance,
		UpdateVoteState = VoteYesNoController.UpdateVoteState
	};
	
	local instance = entry.Instance;
	
	instance.VoteUpButton:RegisterCallback(Mouse.eLClick, function()
		local votes = entry.Votes;
		if(votes >= 0) then
			voteController.VotesAvailable = voteController.VotesAvailable - 1;
		else
			voteController.VotesAvailable = voteController.VotesAvailable + 1;
		end	
		entry.Votes = votes + 1;	
		
		entryController:UpdateVoteInstance();
		voteController:UpdateVoteState();
	end);
	
	instance.VoteUpButton:RegisterCallback(Mouse.eRClick, function()
		local votes = entry.Votes;
		local availableVotes = voteController.VotesAvailable;
		if(votes >= 0) then
			entry.Votes = votes + availableVotes;
			voteController.VotesAvailable = 0;
		else
			entry.Votes = 0;
			voteController.VotesAvailable = availableVotes - votes;
		end
		
		entryController:UpdateVoteInstance();
		voteController:UpdateVoteState();
	end);
	
	instance.VoteDownButton:RegisterCallback(Mouse.eLClick, function()
		local votes = entry.Votes;
		if(votes > 0) then
			voteController.VotesAvailable = voteController.VotesAvailable + 1;
		else
			voteController.VotesAvailable = voteController.VotesAvailable - 1;
		end	
		entry.Votes = votes - 1;	
		
		entryController:UpdateVoteInstance();
		voteController:UpdateVoteState();
	end);
	
	instance.VoteDownButton:RegisterCallback(Mouse.eRClick, function()
		local votes = entry.Votes;
		local availableVotes = voteController.VotesAvailable;
		if(votes > 0) then
			entry.Votes = 0
			voteController.VotesAvailable = availableVotes + votes;
		else
			entry.Votes = votes - availableVotes;
			voteController.VotesAvailable = 0;
		end
		
		entryController:UpdateVoteInstance();
		voteController:UpdateVoteState();
	end);
	
	return entryController;
end
		
function VoteYesNoController:UpdateVoteInstance()
	local instance = self.Entry.Instance;
	local votes = self.Entry.Votes;

	if(votes > 0) then
		instance.VoteStatus:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_VOTE_YAY");
	elseif(votes == 0) then
		instance.VoteStatus:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_VOTE_ABSTAIN");
	else
		instance.VoteStatus:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_VOTE_NAY");
	end		
	
	instance.VoteCount:SetText(votes);
end

function VoteYesNoController:UpdateVoteState(votesAvailable)
	local entry = self.Entry;
	local votes = entry.Votes;
	entry.Instance.VoteUpButton:SetDisabled(votes >= 0 and votesAvailable == 0);
	entry.Instance.VoteDownButton:SetDisabled(votes <= 0 and votesAvailable == 0);
	
	if(votes > 0) then
		entry.Choice = kChoiceYes;
	elseif(votes < 0) then
		entry.Choice = kChoiceNo;
	end
end

-------------------------------------
-- Vote Major Civ Controller --
-------------------------------------
VoteMajorCivController = {};
	
function VoteMajorCivController.new(voteController, entry)
	
	local entryController = {
		VoteController = voteController,
		Entry = entry,
		UpdateVoteInstance = VoteMajorCivController.UpdateVoteInstance,
		UpdateVoteState = VoteMajorCivController.UpdateVoteState
	};
	
	local instance = entry.Instance;
	
	instance.VoteUpButton:RegisterCallback(Mouse.eLClick, function()
		
		local voteUp = function()
			local votes = entry.Votes;
			if(votes >= 0) then
				voteController.VotesAvailable = voteController.VotesAvailable - 1;
			end	
			entry.Votes = votes + 1;	
			
			entryController:UpdateVoteInstance();
			voteController:UpdateVoteState();
		end
		
		if(entry.Choice == nil) then
			PopulateResolutionChoicePopup(entry.VoterChoices, function(choiceId)			
				entry.Choice = choiceId;
				voteUp(choiceId);	
			end);
			
			Controls.ResolutionChoicePopup:SetHide(false);
			Controls.BGBlock:SetHide(true);
		else
			voteUp();
		end
	end);
	
	instance.VoteUpButton:RegisterCallback(Mouse.eRClick, function()
		
		local voteUp = function()
			local votes = entry.Votes;
			local availableVotes = voteController.VotesAvailable;
			if(votes >= 0) then
				entry.Votes = votes + availableVotes;
				voteController.VotesAvailable = 0;
			end	
			
			entryController:UpdateVoteInstance();
			voteController:UpdateVoteState();
		end
		
		if(entry.Choice == nil) then
			PopulateResolutionChoicePopup(entry.VoterChoices, function(choiceId)			
				entry.Choice = choiceId;
				voteUp(choiceId);	
			end);
			
			Controls.ResolutionChoicePopup:SetHide(false);
			Controls.BGBlock:SetHide(true);
		else
			voteUp();
		end
	end);
	
	instance.VoteDownButton:RegisterCallback(Mouse.eLClick, function()
		local votes = entry.Votes;
		if(votes > 0) then
			voteController.VotesAvailable = voteController.VotesAvailable + 1;
		end	
		entry.Votes = votes - 1;	
		
		if(entry.Votes == 0) then
			entry.Choice = nil;
		end
		
		entryController:UpdateVoteInstance();
		voteController:UpdateVoteState();
	end);
	
	instance.VoteDownButton:RegisterCallback(Mouse.eRClick, function()
		local votes = entry.Votes;
		local availableVotes = voteController.VotesAvailable;
		if(votes > 0) then
			entry.Votes = 0;
			voteController.VotesAvailable = availableVotes + votes;
		end	
		
		if(entry.Votes == 0) then
			entry.Choice = nil;
		end
		
		entryController:UpdateVoteInstance();
		voteController:UpdateVoteState();
	end);
	
	return entryController;
end
		
function VoteMajorCivController:UpdateVoteInstance()
	local instance = self.Entry.Instance;
	local votes = self.Entry.Votes;

	if(votes == 0) then
		instance.VoteStatus:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_VOTE_ABSTAIN");
		instance.VoteStatus:SetHide(false);
		instance.AltIconFrame:SetHide(true);
	else
		instance.VoteStatus:SetHide(true);
		instance.AltIconFrame:SetHide(false);
	end		
	
	local choice = self.Entry.Choice;
	if(choice ~= nil) then
		local mainIcon = nil;
		local choicePlayerId;
		for i,v in ipairs(self.Entry.VoterChoices) do
			if(v.Id == choice) then
				mainIcon = v.MainIcon;
				choicePlayerId = v.PlayerId;
			end
		end
		
		IconHookup(mainIcon.PortraitIndex, 64, mainIcon.IconAtlas, instance.AltIcon);
		CivIconHookup(choicePlayerId, 32, instance.AltSubIcon, instance.AltSubIconBG, instance.AltSubIconShadow, true, true);
	end
	
	instance.VoteCount:SetText(votes);
end

function VoteMajorCivController:UpdateVoteState(votesAvailable)
	local entry = self.Entry;
	local votes = entry.Votes;
	entry.Instance.VoteUpButton:SetDisabled(votes >= 0 and votesAvailable == 0);
	entry.Instance.VoteDownButton:SetDisabled(votes <= 0);
end
	
---------------------------------
--Proposal Beahvior Controller --
---------------------------------						
ProposalController = {
}

function ProposalController:Initialize(data)
	
	self.PendingProposals = {};
	self.ActivePlayerId = data.ActivePlayerId;
	self.LeagueId = data.LeagueId;
	self.ResetButton = data.ResetButton;
	self.CommitButton = data.CommitButton;
	self.ActiveResolutions = data.ActiveResolutions;
	self.InactiveResolutions = data.InactiveResolutions;
	self.ProposalMainIcon = data.ProposalMainIcon;
	self.ProposalSubIcon = data.ProposalSubIcon;
	self.ProposalInstances = {};
		
	for i = 1, data.NumProposals, 1 do
		local pendingProposal = {
			Controller = self,
			Index = i,
		};
		
		-- Refer back to the controller to obtain the instance.
		-- This is because the instances may change if a refresh event occurs.
		self.ProposalInstances[i] = data.InstanceManager:GetInstance();
	
		table.insert(self.PendingProposals, pendingProposal);	
		self:UpdatePendingProposalInstance(pendingProposal);
	end
	
	-- Close any popups that are up
	OnConfirmNo();
	CloseResolutionChoicePopup();
	CloseProposeResolutionPopup();
	
	self.ResetButton:SetHide(false);
	self.CommitButton:SetHide(false);
		
	local controller = self;
	self.ResetButton:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_RESET_PROPOSALS");
	Controls.ResetButton:RegisterCallback(Mouse.eLClick, function() controller:ResetProposals() end);
	self.ResetButton:SetDisabled(true);
	
	self.CommitButton:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_COMMIT_PROPOSALS");
	Controls.CommitButton:RegisterCallback(Mouse.eLClick, function() controller:CommitProposals() end);
	self.CommitButton:SetDisabled(true);
end

function ProposalController:UpdatePendingProposalInstance(pendingProposal)
	
	-- Refer back to the controller to obtain the instance.
	-- This is because the instances may change if a refresh event occurs.
	local instance = pendingProposal.Controller.ProposalInstances[pendingProposal.Index];
	
	local controller = self;
	instance.ResolutionButton:ClearCallback(Mouse.eLClick);
	instance.ResolutionButton:SetDisabled(false);
	instance.ResolutionButton:RegisterCallback(Mouse.eLClick, function()
		PopulateProposeResolutionPopup(self.ActiveResolutions, self.InactiveResolutions, self.PendingProposals, function(proposal)
			
			local SelectProposal = function(choiceId)
				
				
				local league = Game.GetLeague(controller.LeagueId);
				local text;
				local toolTip;
				
				local prefix = "";
				if(proposal.Direction == "Enact") then
					prefix = g_strEnactPrefix;
					text = league:GetResolutionName(proposal.Type, -1, choiceId, false);
					toolTip = league:GetResolutionDetails(proposal.Type, controller.ActivePlayerId, -1, choiceId); 				
				else
					prefix = g_strRepealPrefix;
					text = league:GetResolutionName(proposal.Type, -1, proposal.ProposerDecision, false);
					toolTip = league:GetResolutionDetails(proposal.Type, controller.ActivePlayerId, -1, proposal.ProposerDecision); 	
				end
						
				
				pendingProposal.SelectedProposal = {
					Id = proposal.Id,
					Type = proposal.Type,
					ChoiceId = choiceId,
					Text = prefix .. text,
					ToolTip = toolTip,
					Direction = proposal.Direction,
				};
				
				local canCommit = true;
			
				for i,v in ipairs(controller.PendingProposals) do
					if(v.SelectedProposal == nil) then
						canCommit = false;
						break;
					end
				end
				
				controller.CommitButton:SetDisabled(not canCommit);
				controller.ResetButton:SetDisabled(false);
			
				controller:UpdatePendingProposalInstance(pendingProposal);
			end
			
			if(proposal.ProposerChoices ~= nil and #proposal.ProposerChoices > 0) then
				PopulateResolutionChoicePopup(proposal.ProposerChoices, function(choiceId)
					SelectProposal(choiceId);
				end);
				
				Controls.ResolutionChoicePopup:SetHide(false);
				Controls.BGBlock:SetHide(true);
			else
				SelectProposal(kChoiceNone)
			end		
		end);
	
		Controls.ChooseProposalPopup:SetHide(false);
		Controls.BGBlock:SetHide(true);
	end);	
			
	if(pendingProposal.SelectedProposal ~= nil) then
		instance.ResolutionName:SetText(pendingProposal.SelectedProposal.Text);
		instance.ResolutionButton:SetToolTipString(pendingProposal.SelectedProposal.ToolTip);	
	else
		instance.ResolutionName:SetText(g_strProposeButtonCaption);
		instance.ResolutionButton:SetToolTipString(g_strProposeButtonToolTip);
	end
	
	
	instance.SubIconFrame:SetHide(false);
	if(self.ProposalMainIcon ~= nil) then
		IconHookup(self.ProposalMainIcon.PortraitIndex, 64, self.ProposalMainIcon.IconAtlas, instance.Icon);
	else
		IconHookup(0, 64, "INTL_PROJECT_ATLAS_EXP2", instance.Icon);
	end
	
	CivIconHookup(self.ActivePlayerId, 32, instance.SubIcon, instance.SubIconBG, instance.SubIconShadow, true, true);
		
	instance.AltIconFrame:SetHide(true);
	instance.VoteBox:SetHide(true);
end

function ProposalController:ResetProposals()
	for i, proposal in ipairs(self.PendingProposals) do
		proposal.SelectedProposal = nil;
		self:UpdatePendingProposalInstance(proposal);
	end
	
	self.ResetButton:SetDisabled(true);
	self.CommitButton:SetDisabled(true);
end

function ProposalController:CommitProposals()

	local controller = self;
	local confirmAction = function()
		for i, pendingProposal in ipairs(controller.PendingProposals) do
			local proposal = pendingProposal.SelectedProposal;
			if(proposal.Direction == "Enact") then
				local choice = proposal.ChoiceId or kChoiceNone;		
				Network.SendLeagueProposeEnact(controller.LeagueId, proposal.Type, controller.ActivePlayerId, choice);
			else
				Network.SendLeagueProposeRepeal(controller.LeagueId, proposal.Id, controller.ActivePlayerId);
			end
		end	
	end
	
	CommitConfirm(confirmAction);
end

-------------------------------
-- Collapse/Expand Behaviors --
-------------------------------
function OnCollapseExpand()
	Controls.ActiveResolutionStack:CalculateSize();
	Controls.ActiveResolutionStack:ReprocessAnchoring();
	Controls.InactiveResolutionStack:CalculateSize();
	Controls.InactiveResolutionStack:ReprocessAnchoring();
	Controls.DisabledResolutionStack:CalculateSize();
	Controls.DisabledResolutionStack:ReprocessAnchoring();
	Controls.ChooseProposalStack:CalculateSize();
	Controls.ChooseProposalStack:ReprocessAnchoring();
	Controls.ChooseProposalScrollPanel:CalculateInternalSize();
end

function OnCollapseExpand2()
	Controls.ActiveResolutionStack2:CalculateSize();
	Controls.ActiveResolutionStack2:ReprocessAnchoring();
	Controls.InactiveResolutionStack2:CalculateSize();
	Controls.InactiveResolutionStack2:ReprocessAnchoring();
	Controls.DisabledResolutionStack2:CalculateSize();
	Controls.DisabledResolutionStack2:ReprocessAnchoring();
	Controls.ViewAllResolutionsScrollPanel:CalculateInternalSize();
end

local activeResolutionsText = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_ACTIVE_RESOLUTIONS");
RegisterCollapseBehavior{	
	Header = Controls.ActiveResolutionsHeader, 
	HeaderLabel = Controls.ActiveResolutionsHeaderLabel, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. activeResolutionsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. activeResolutionsText,
	Panel = Controls.ActiveResolutionStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

RegisterCollapseBehavior{	
	Header = Controls.ActiveResolutionsHeader2, 
	HeaderLabel = Controls.ActiveResolutionsHeaderLabel2, 
	HeaderExpandedLabel = "[ICON_MINUS] " .. activeResolutionsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. activeResolutionsText,
	Panel = Controls.ActiveResolutionStack2,
	Collapsed = false,
	OnCollapse = OnCollapseExpand2,
	OnExpand = OnCollapseExpand2,
};
							
local possibleResolutionsText = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_INACTIVE_RESOLUTIONS");
RegisterCollapseBehavior{
	Header = Controls.PossibleResolutionsHeader,
	HeaderLabel = Controls.PossibleResolutionsHeaderLabel,
	HeaderExpandedLabel = "[ICON_MINUS] " .. possibleResolutionsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. possibleResolutionsText,
	Panel = Controls.InactiveResolutionStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

RegisterCollapseBehavior{
	Header = Controls.PossibleResolutionsHeader2,
	HeaderLabel = Controls.PossibleResolutionsHeaderLabel2,
	HeaderExpandedLabel = "[ICON_MINUS] " .. possibleResolutionsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. possibleResolutionsText,
	Panel = Controls.InactiveResolutionStack2,
	Collapsed = false,
	OnCollapse = OnCollapseExpand2,
	OnExpand = OnCollapseExpand2,
};

local disabledResolutionsText = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW_OTHER_RESOLUTIONS");
RegisterCollapseBehavior{
	Header = Controls.DisabledResolutionsHeader,
	HeaderLabel = Controls.DisabledResolutionsHeaderLabel,
	HeaderExpandedLabel = "[ICON_MINUS] " .. disabledResolutionsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. disabledResolutionsText,
	Panel = Controls.DisabledResolutionStack,
	Collapsed = false,
	OnCollapse = OnCollapseExpand,
	OnExpand = OnCollapseExpand,
};

RegisterCollapseBehavior{
	Header = Controls.DisabledResolutionsHeader2,
	HeaderLabel = Controls.DisabledResolutionsHeaderLabel2,
	HeaderExpandedLabel = "[ICON_MINUS] " .. disabledResolutionsText,
	HeaderCollapsedLabel = "[ICON_PLUS] " .. disabledResolutionsText,
	Panel = Controls.DisabledResolutionStack2,
	Collapsed = false,
	OnCollapse = OnCollapseExpand2,
	OnExpand = OnCollapseExpand2,
};

function CommitConfirm(action, bConfirmUnused)

	g_ConfirmAction = action;
	if (bConfirmUnused ~= nil and bConfirmUnused) then
		Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_CONFIRM_MISSING_VOTES");
	else
		Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_LEAGUE_OVERVIEW_CONFIRM");
	end
	Controls.ChooseConfirm:SetHide(false);
end

function OnConfirmYes( )
	Controls.ChooseConfirm:SetHide(true);
	
	if(g_ConfirmAction ~= nil) then
		g_ConfirmAction();
		g_ConfirmAction = nil;
	end
		
	OnClose();	
end
Controls.ConfirmYes:RegisterCallback( Mouse.eLClick, OnConfirmYes );

function OnConfirmNo( )
	Controls.ChooseConfirm:SetHide(true);
end
Controls.ConfirmNo:RegisterCallback( Mouse.eLClick, OnConfirmNo );

-----------------------------------------------------------------
-- Add Espionage Overview to Dropdown
-----------------------------------------------------------------
LuaEvents.AdditionalInformationDropdownGatherEntries.Add(function(entries)
	if (not Game.IsOption("GAMEOPTION_NO_LEAGUES")) then
		table.insert(entries, {
			text = Locale.Lookup("TXT_KEY_LEAGUE_OVERVIEW"),
			call = function() 
				Events.SerialEventGameMessagePopup{ 
					Type = ButtonPopupTypes.BUTTONPOPUP_LEAGUE_OVERVIEW,
				};
			end,
		});
	end
end);

-- Just in case :)
LuaEvents.RequestRefreshAdditionalInformationDropdownEntries();
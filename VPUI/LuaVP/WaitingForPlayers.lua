if CodeBuddy.vpRequestReinit then CodeBuddy.vpRequestReinit() end

if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end

if CodeBuddy.vpSetGameCore then CodeBuddy.vpSetGameCore({
	Game = Game;
	GameInfoTypes = GameInfoTypes;
	GameOptionTypes = GameOptionTypes;
	Map = Map;
	Player = Player;
	Players = Players;
	Team = Team;
	Teams = Teams;
	YieldTypes = YieldTypes;
--[[
	ActionSubTypes = ActionSubTypes;
	ActivityTypes = ActivityTypes;
	AdvisorTypes = AdvisorTypes;
	BeliefTypes = BeliefTypes;
	ButtonPopupTypes = ButtonPopupTypes;
	ChatTargetTypes = ChatTargetTypes;
	CityAIFocusTypes = CityAIFocusTypes;
	CityUpdateTypes = CityUpdateTypes;
	CombatPredictionTypes = CombatPredictionTypes;
	CommandTypes = CommandTypes;
	ControlTypes = ControlTypes;
	CoopWarStates = CoopWarStates;
	DiploUIStateTypes = DiploUIStateTypes;
	DirectionTypes = DirectionTypes;
	DisputeLevelTypes = DisputeLevelTypes;
	DomainTypes = DomainTypes;
	EndTurnBlockingTypes = EndTurnBlockingTypes;
	FaithPurchaseTypes = FaithPurchaseTypes;
	FeatureTypes = FeatureTypes;
	FlowDirectionTypes = FlowDirectionTypes;
	FogOfWarModeTypes = FogOfWarModeTypes;
	Fractal = Fractal;
	FromUIDiploEventTypes = FromUIDiploEventTypes;
	GameInfoActions = GameInfoActions;
	GameMessageTypes = GameMessageTypes;
	GameStates = GameStates;
	GameplayGameStateTypes = GameplayGameStateTypes;
	GenericWorldAnchorTypes = GenericWorldAnchorTypes;
	InfluenceLevelTrend = InfluenceLevelTrend;
	InfluenceLevelTypes = InfluenceLevelTypes;
	InterfaceDirtyBits = InterfaceDirtyBits;
	InterfaceModeTypes = InterfaceModeTypes;
	LeaderheadAnimationTypes = LeaderheadAnimationTypes;
	MajorCivApproachTypes = MajorCivApproachTypes;
	MapModData = MapModData;
	MinorCivPersonalityTypes = MinorCivPersonalityTypes;
	MinorCivQuestTypes = MinorCivQuestTypes;
	MinorCivTraitTypes = MinorCivTraitTypes;
	MissionTypes = MissionTypes;
	NotificationTypes = NotificationTypes;
	OrderTypes = OrderTypes;
	PlotTypes = PlotTypes;
	PolicyBranchTypes = PolicyBranchTypes;
	PublicOpinionTypes = PublicOpinionTypes;
	ReligionTypes = ReligionTypes;
	ResolutionDecisionTypes = ResolutionDecisionTypes;
	ResourceUsageTypes = ResourceUsageTypes;
	TaskTypes = TaskTypes;
	TerrainTypes = TerrainTypes;
	ThreatTypes = ThreatTypes;
	TradeableItems = TradeableItems;
--]]
}) end

function ShowHide( isHide )
    if( isHide == true ) then
        -- SetUICursor( 0 );
    else
        -- SetUICursor( 1 );
    end
end
ContextPtr:SetShowHideHandler( ShowHide );
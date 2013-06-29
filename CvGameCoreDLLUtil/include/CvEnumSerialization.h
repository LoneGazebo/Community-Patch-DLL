#ifndef INCLUDED_CvEnumSerialization_H
#define INCLUDED_CvEnumSerialization_H

FDataStream & operator<<(FDataStream &, const PlayerColorTypes &);
FDataStream & operator>>(FDataStream &, PlayerColorTypes &);

FDataStream & operator<<(FDataStream &, const PlayerOptionTypes &);
FDataStream & operator>>(FDataStream &, PlayerOptionTypes &);

FDataStream & operator<<(FDataStream &, const ClimateTypes &);
FDataStream & operator>>(FDataStream &, ClimateTypes &);

FDataStream & operator<<(FDataStream &, const SeaLevelTypes &);
FDataStream & operator>>(FDataStream &, SeaLevelTypes &);

FDataStream & operator<<(FDataStream &, const WorldSizeTypes &);
FDataStream & operator>>(FDataStream &, WorldSizeTypes &);

FDataStream & operator<<(FDataStream &, const HandicapTypes &);
FDataStream & operator>>(FDataStream &, HandicapTypes &);

FDataStream & operator<<(FDataStream &, const GameSpeedTypes &);
FDataStream & operator>>(FDataStream &, GameSpeedTypes &);

FDataStream & operator<<(FDataStream &, const TurnTimerTypes &);
FDataStream & operator>>(FDataStream &, TurnTimerTypes &);

FDataStream & operator<<(FDataStream &, const EraTypes &);
FDataStream & operator>>(FDataStream &, EraTypes &);

FDataStream & operator<<(FDataStream &, const CivilizationTypes &);
FDataStream & operator>>(FDataStream &, CivilizationTypes &);

FDataStream & operator<<(FDataStream &, const LeaderHeadTypes &);
FDataStream & operator>>(FDataStream &, LeaderHeadTypes &);

FDataStream & operator<<(FDataStream &, const ChatTargetTypes &);
FDataStream & operator>>(FDataStream &, ChatTargetTypes &);

FDataStream & operator<<(FDataStream &, const TeamTypes &);
FDataStream & operator>>(FDataStream &, TeamTypes &);

FDataStream & operator<<(FDataStream &, const PlayerTypes &);
FDataStream & operator>>(FDataStream &, PlayerTypes &);

FDataStream & operator<<(FDataStream &, const ReligionTypes &);
FDataStream & operator>>(FDataStream &, ReligionTypes &);

FDataStream & operator<<(FDataStream &, const PlayerVoteTypes &);
FDataStream & operator>>(FDataStream &, PlayerVoteTypes &);

FDataStream & operator<<(FDataStream &, const OrderTypes &);
FDataStream & operator>>(FDataStream &, OrderTypes &);

FDataStream & operator<<(FDataStream &, const TaskTypes &);
FDataStream & operator>>(FDataStream &, TaskTypes &);

FDataStream & operator<<(FDataStream &, const CalendarTypes &);
FDataStream & operator>>(FDataStream &, CalendarTypes &);

FDataStream & operator<<(FDataStream &, const MissionTypes &);
FDataStream & operator>>(FDataStream &, MissionTypes &);

FDataStream & operator<<(FDataStream &, const CommandTypes &);
FDataStream & operator>>(FDataStream &, CommandTypes &);

FDataStream & operator<<(FDataStream &, const TechTypes &);
FDataStream & operator>>(FDataStream &, TechTypes &);

FDataStream & operator<<(FDataStream &, const PolicyTypes &);
FDataStream & operator>>(FDataStream &, PolicyTypes &);

FDataStream & operator<<(FDataStream &, const NetContactTypes &);
FDataStream & operator>>(FDataStream &, NetContactTypes &);

FDataStream & operator<<(FDataStream &, const SaveGameTypes &);
FDataStream & operator>>(FDataStream &, SaveGameTypes &);

FDataStream & operator<<(FDataStream &, const GameTypes &);
FDataStream & operator>>(FDataStream &, GameTypes &);

FDataStream & operator<<(FDataStream &, const GameMapTypes &);
FDataStream & operator>>(FDataStream &, GameMapTypes &);

FDataStream & operator<<(FDataStream &, const GameMode &);
FDataStream & operator>>(FDataStream &, GameMode &);

FDataStream & operator<<(FDataStream &, const SlotClaim &);
FDataStream & operator>>(FDataStream &, SlotClaim &);

FDataStream & operator<<(FDataStream &, const SlotStatus &);
FDataStream & operator>>(FDataStream &, SlotStatus &);

FDataStream & operator<<(FDataStream &, const CivLoginStates &);
FDataStream & operator>>(FDataStream &, CivLoginStates &);

FDataStream & operator<<(FDataStream &, const VersionTypes &);
FDataStream & operator>>(FDataStream &, VersionTypes &);

FDataStream & operator<<(FDataStream &, const BandwidthType &);
FDataStream & operator>>(FDataStream &, BandwidthType &);

FDataStream & operator<<(FDataStream &, const VictoryTypes &);
FDataStream & operator>>(FDataStream &, VictoryTypes &);

FDataStream & operator<<(FDataStream &, const SmallAwardTypes &);
FDataStream & operator>>(FDataStream &, SmallAwardTypes &);

FDataStream & operator<<(FDataStream &, const AdvancedStartActionTypes &);
FDataStream & operator>>(FDataStream &, AdvancedStartActionTypes &);

FDataStream & operator<<(FDataStream &, const UnitAITypes &);
FDataStream & operator>>(FDataStream &, UnitAITypes &);

FDataStream & operator<<(FDataStream &, const VoteSourceTypes &);
FDataStream & operator>>(FDataStream &, VoteSourceTypes &);

FDataStream & operator<<(FDataStream &, const VoteTypes &);
FDataStream & operator>>(FDataStream &, VoteTypes &);

FDataStream & operator<<(FDataStream &, const BuildingClassTypes &);
FDataStream & operator>>(FDataStream &, BuildingClassTypes &);

FDataStream & operator<<(FDataStream &, const ColorTypes &);
FDataStream & operator>>(FDataStream &, ColorTypes &);

FDataStream & operator<<(FDataStream &, const InterfaceMessageTypes &);
FDataStream & operator>>(FDataStream &, InterfaceMessageTypes &);

FDataStream & operator<<(FDataStream &, const DirectionTypes &);
FDataStream & operator>>(FDataStream &, DirectionTypes &);
namespace FSerialization { std::string toString(const DirectionTypes &); }

FDataStream & operator<<(FDataStream &, const UnitTypes &);
FDataStream & operator>>(FDataStream &, UnitTypes &);

FDataStream & operator<<(FDataStream &, const BuildingTypes &);
FDataStream & operator>>(FDataStream &, BuildingTypes &);

FDataStream & operator<<(FDataStream &, const ReplayMessageTypes &);
FDataStream & operator>>(FDataStream &, ReplayMessageTypes &);

FDataStream & operator<<(FDataStream &, const ResourceTypes &);
FDataStream & operator>>(FDataStream &, ResourceTypes &);

FDataStream & operator<<(FDataStream &, const MissionAITypes &);
FDataStream & operator>>(FDataStream &, MissionAITypes &);

FDataStream & operator<<(FDataStream &, const ActivityTypes &);
FDataStream & operator>>(FDataStream &, ActivityTypes &);
namespace FSerialization { std::string toString(const ActivityTypes &); }

FDataStream & operator<<(FDataStream &, const AutomateTypes &);
FDataStream & operator>>(FDataStream &, AutomateTypes &);

FDataStream & operator<<(FDataStream &, const GameOptionTypes &);
FDataStream & operator>>(FDataStream &, GameOptionTypes &);

FDataStream & operator<<(FDataStream &, const ButtonPopupTypes &);
FDataStream & operator>>(FDataStream &, ButtonPopupTypes &);

FDataStream & operator<<(FDataStream &, const UnitClassTypes &);
FDataStream & operator>>(FDataStream &, UnitClassTypes &);

FDataStream & operator<<(FDataStream &, const MinorCivTypes &);
FDataStream & operator>>(FDataStream &, MinorCivTypes &);

FDataStream & operator<<(FDataStream &, const ArtStyleTypes &);
FDataStream & operator>>(FDataStream &, ArtStyleTypes &);

FDataStream & operator<<(FDataStream &, const DiploCommentTypes &);
FDataStream & operator>>(FDataStream &, DiploCommentTypes &);

FDataStream & operator<<(FDataStream &, const GameStateTypes &);
FDataStream & operator>>(FDataStream &, GameStateTypes &);

FDataStream & operator<<(FDataStream &, const BuildTypes &);
FDataStream & operator>>(FDataStream &, BuildTypes &);

FDataStream & operator<<(FDataStream &, const CultureLevelTypes &);
FDataStream & operator>>(FDataStream &, CultureLevelTypes &);

FDataStream & operator<<(FDataStream &, const TacticalAIMoveTypes &);
FDataStream & operator>>(FDataStream &, TacticalAIMoveTypes &);

FDataStream & operator<<(FDataStream &, const CitySpecializationTypes &);
FDataStream & operator>>(FDataStream &, CitySpecializationTypes &);

FDataStream & operator<<(FDataStream &, const InvisibleTypes &);
FDataStream & operator>>(FDataStream &, InvisibleTypes &);

FDataStream & operator<<(FDataStream &, const PolicyBranchTypes &);
FDataStream & operator>>(FDataStream &, PolicyBranchTypes &);

FDataStream & operator<<(FDataStream &, const EndTurnBlockingTypes &);
FDataStream & operator>>(FDataStream &, EndTurnBlockingTypes &);

FDataStream & operator<<(FDataStream &, const NotificationTypes &);
FDataStream & operator>>(FDataStream &, NotificationTypes &);

FDataStream & operator<<(FDataStream &, const FromUIDiploEventTypes &);
FDataStream & operator>>(FDataStream &, FromUIDiploEventTypes &);

FDataStream & operator<<(FDataStream &, const RouteTypes &);
FDataStream & operator>>(FDataStream &, RouteTypes &);

FDataStream & operator<<(FDataStream &, const ImprovementTypes &);
FDataStream & operator>>(FDataStream &, ImprovementTypes &);

FDataStream & operator<<(FDataStream &, const CityAIFocusTypes &);
FDataStream & operator>>(FDataStream &, CityAIFocusTypes &);

FDataStream & operator<<(FDataStream &, const GreatPeopleDirectiveTypes &);
FDataStream & operator>>(FDataStream &, GreatPeopleDirectiveTypes &);

FDataStream & operator<<(FDataStream &, const DiploUIStateTypes &);
FDataStream & operator>>(FDataStream &, DiploUIStateTypes &);

FDataStream & operator<<(FDataStream &, const LeaderheadAnimationTypes &);
FDataStream & operator>>(FDataStream &, LeaderheadAnimationTypes &);

FDataStream & operator<<(FDataStream &, const GameExecutableTypes &);
FDataStream & operator>>(FDataStream &, GameExecutableTypes &);

FDataStream & operator<<(FDataStream &, const BeliefTypes &);
FDataStream & operator>>(FDataStream &, BeliefTypes &);

FDataStream & operator<<(FDataStream&, const StorageLocation&);
FDataStream & operator>>(FDataStream&, StorageLocation&);

FDataStream & operator<<(FDataStream&, const LeagueTypes&);
FDataStream & operator>>(FDataStream&, LeagueTypes&);

FDataStream & operator<<(FDataStream&, const LeagueSpecialSessionTypes&);
FDataStream & operator>>(FDataStream&, LeagueSpecialSessionTypes&);

FDataStream & operator<<(FDataStream&, const LeagueNameTypes&);
FDataStream & operator>>(FDataStream&, LeagueNameTypes&);

FDataStream & operator<<(FDataStream&, const LeagueProjectTypes&);
FDataStream & operator>>(FDataStream&, LeagueProjectTypes&);

FDataStream & operator<<(FDataStream&, const ResolutionTypes&);
FDataStream & operator>>(FDataStream&, ResolutionTypes&);

FDataStream & operator<<(FDataStream&, const ResolutionDecisionTypes&);
FDataStream & operator>>(FDataStream&, ResolutionDecisionTypes&);


#endif//INCLUDED_CvEnumSerialization_H

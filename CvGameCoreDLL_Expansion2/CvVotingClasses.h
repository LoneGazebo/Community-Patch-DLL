/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once

#ifndef CIV5_VOTING_CLASSES_H
#define CIV5_VOTING_CLASSES_H

#define DEBUG_LEAGUES false


// Forward declarations
class CvLeague;


// Helper namespace
namespace LeagueHelpers
{
	CvString GetTextForChoice(ResolutionDecisionTypes eDecision, int iChoice);
	EraTypes GetGameEraForTrigger();
	EraTypes GetNextGameEraForTrigger();
	EraTypes GetNextGameEraForTrigger(EraTypes eThisEra);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	BuildingTypes GetBuildingForTrigger(BuildingTypes eBuilding);
	ResolutionTypes IsResolutionForTriggerActive(ResolutionTypes eType);
#endif

	typedef FStaticVector<PlayerTypes, MAX_CIV_PLAYERS, true, c_eCiv5GameplayDLL> PlayerList;

	struct VoteTextSortElement
	{
		CvString sText;
		int iChoice;
		int iChoiceVotesTotal;
		int iChoiceVotesThisPlayer;
	};

	struct VoteTextSorter
	{
		bool operator()(const VoteTextSortElement& a, const VoteTextSortElement& b) const
		{
			if (a.iChoiceVotesTotal != b.iChoiceVotesTotal)
			{
				return a.iChoiceVotesTotal > b.iChoiceVotesTotal;
			}
			else if (a.iChoice != b.iChoice)
			{
				return a.iChoice > b.iChoice;
			}
			else
			{
				return a.iChoiceVotesThisPlayer > b.iChoiceVotesThisPlayer;
			}
		}
	};

	struct VoteOpinionIntrigueElement
	{
		int iChoice;
		int iNumCivs;
		int iNumDelegates;
	};

	struct VoteOpinionIntrigueSorter
	{
		bool operator()(const VoteOpinionIntrigueElement& a, const VoteOpinionIntrigueElement& b) const
		{
			return a.iNumDelegates > b.iNumDelegates;
		}
	};

	const int CHOICE_NONE = -1;
	const int CHOICE_NO = 0;
	const int CHOICE_YES = 1;

	const int AI_CHOOSE_PROPOSAL_FROM_TOP = 3;

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	// Moved to xml, use
	// GC.getLEAGUE_PROPOSERS_PER_SESSION()
	// GC.getLEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES()
	// GC.getLEAGUE_PROJECT_PROGRESS_PERCENT_WARNING()
#else
	//antonjs: todo: xml
	const int PROPOSERS_PER_SESSION = 2;
	const int NUM_LEADERS_FOR_EXTRA_VOTES = 2;
	const int PROJECT_PROGRESS_PERCENT_WARNING = 33;
#endif
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:     CvResolutionEffects
//!  \brief		Data structure for effects from Resolutions
//
//!  Key Attributes:
//!  - May be used for one Resolution's effects or a sum of Resolutions' effects
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvResolutionEffects
{
	CvResolutionEffects(void);
	CvResolutionEffects(ResolutionTypes eType);
	~CvResolutionEffects(void);

	bool HasOngoingEffects() const;
	void AddOngoingEffects(const CvResolutionEffects* pOtherEffects);

	bool bDiplomaticVictory;
	bool bChangeLeagueHost;
	int iOneTimeGold;
	int iOneTimeGoldPercent;
	bool bRaiseCityStateInfluenceToNeutral;
	LeagueProjectTypes eLeagueProjectEnabled;
	int iGoldPerTurn;
	int iResourceQuantity;
	bool bEmbargoCityStates;
	bool bEmbargoPlayer;
	bool bNoResourceHappiness;
	int iUnitMaintenanceGoldPercent;
	int iMemberDiscoveredTechMod;
	int iCulturePerWonder;
	int iCulturePerNaturalWonder;
	bool bNoTrainingNuclearWeapons;
	int iVotesForFollowingReligion;
	int iHolyCityTourism;
	int iReligionSpreadStrengthMod;
	int iVotesForFollowingIdeology;
	int iOtherIdeologyRebellionMod;
	int iArtsyGreatPersonRateMod;
	int iScienceyGreatPersonRateMod;
	int iGreatPersonTileImprovementCulture;
	int iLandmarkCulture;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	bool bOpenDoor;
	bool bSphereOfInfluence;
	bool bDecolonization;
	int iLimitSpaceshipProduction;
	int iLimitSpaceshipPurchase;
	int iIsWorldWar;
	bool bEmbargoIdeology;
#endif
#if defined(MOD_BALANCE_CORE)
	int iChangeTourism;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int iVassalMaintenanceGoldPercent;
	bool bEndAllCurrentVassals;
#endif
};

FDataStream& operator>>(FDataStream&, CvResolutionEffects&);
FDataStream& operator<<(FDataStream&, const CvResolutionEffects&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvResolutionDecision
//!  \brief		Abstract base class for a decision about a CvResolution that must be resolved
//
//!  Key Attributes:
//!  - Abstract base class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvResolutionDecision
{
public:
	CvResolutionDecision(void);
	CvResolutionDecision(ResolutionDecisionTypes eType);
	~CvResolutionDecision(void);

	struct PlayerVote {
		PlayerVote(void);
		~PlayerVote(void);

		PlayerTypes ePlayer;
		int iNumVotes;
		int iChoice; // Interpreted differently based on ResolutionDecisionTypes
	};
	typedef FStaticVector<PlayerVote, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> PlayerVoteList;

	// Pure virtual functions
	virtual int GetDecision() = 0;
	
	ResolutionDecisionTypes GetType() const;

	ResolutionDecisionTypes m_eType;

protected:
};

FDataStream& operator>>(FDataStream&, CvResolutionDecision::PlayerVote&);
FDataStream& operator<<(FDataStream&, const CvResolutionDecision::PlayerVote&);

FDataStream& operator>>(FDataStream&, CvResolutionDecision&);
FDataStream& operator<<(FDataStream&, const CvResolutionDecision&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvProposerDecision
//!  \brief		Decision about a CvResolution to be resolved by the proposer
//
//!  Key Attributes:
//!  - Stores one PlayerVote, from the proposer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProposerDecision : public CvResolutionDecision
{
public:
	CvProposerDecision(void);
	CvProposerDecision(ResolutionDecisionTypes eType, PlayerTypes eProposer, int iChoice);
	~CvProposerDecision(void);

	int GetDecision();
	PlayerTypes GetProposer();

	PlayerVote m_sVote;

private:
};

FDataStream& operator>>(FDataStream&, CvProposerDecision&);
FDataStream& operator<<(FDataStream&, const CvProposerDecision&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvVoterDecision
//!  \brief		Decision about a CvResolution to be resolved by the voters
//
//!  Key Attributes:
//!  - Stores multiple PlayerVotes, one from each voter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoterDecision : public CvResolutionDecision
{
public:
	CvVoterDecision(void);
	CvVoterDecision(ResolutionDecisionTypes eType);
	~CvVoterDecision(void);

	int GetDecision();
	bool IsTie();
	std::vector<int> GetTopVotedChoices(int iNumTopChoices);
	int GetVotesCast();
	int GetVotesCastForChoice(int iChoice);
	int GetVotesMarginOfTopChoice();
	int GetVotesCastByPlayer(PlayerTypes ePlayer);
	LeagueHelpers::PlayerList GetPlayersVotingForChoice(int iChoice);
	void ProcessVote(PlayerTypes eVoter, int iNumVotes, int iChoice);
	CvString GetVotesAsText(CvLeague* pLeague);
	bool ComparePlayerVote(const PlayerVote& lhs, const PlayerVote& rhs);

	PlayerVoteList m_vVotes;

private:
};

FDataStream& operator>>(FDataStream&, CvVoterDecision&);
FDataStream& operator<<(FDataStream&, const CvVoterDecision&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvResolution
//!  \brief		Abstract base class for a game-changing effect
//
//!  Key Attributes:
//!  - Abstract base class
//!  - Contains data about a Resolution's type and effects
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvResolution
{
public:
	CvResolution(void);
	CvResolution(int iID, ResolutionTypes eType, LeagueTypes eLeague);
	~CvResolution(void);

	// Pure virtual functions
	virtual void Init() = 0;

	int GetID() const;
	ResolutionTypes GetType() const;
	LeagueTypes GetLeague() const;
	CvResolutionEffects* GetEffects();
	CvVoterDecision* GetVoterDecision();
	CvProposerDecision* GetProposerDecision();
	CvString GetName();

	int m_iID;
	ResolutionTypes m_eType;
	LeagueTypes m_eLeague;
	CvResolutionEffects m_sEffects;
	CvVoterDecision m_VoterDecision;
	CvProposerDecision m_ProposerDecision;

protected:
};

FDataStream& operator>>(FDataStream&, CvResolution&);
FDataStream& operator<<(FDataStream&, const CvResolution&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvProposal
//!  \brief		Abstract base class for a game-changing effect considered by a CvLeague
//
//!  Key Attributes:
//!  - Abstract base class
//!  - Contains data about how it was proposed
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProposal : public CvResolution
{
public:
	CvProposal(void);
	CvProposal(int iID, ResolutionTypes eType, LeagueTypes eLeague, PlayerTypes eProposalPlayer);
	~CvProposal(void);

	// Pure virtual functions
	virtual bool IsPassed(int iTotalSessionVotes) = 0;
	virtual CvString GetProposalName(bool bForLogging) = 0;

	PlayerTypes GetProposalPlayer() const;

	PlayerTypes m_eProposalPlayer;

protected:
};

FDataStream& operator>>(FDataStream&, CvProposal&);
FDataStream& operator<<(FDataStream&, const CvProposal&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvEnactProposal
//!  \brief		Proposal to a enact a new Resolution
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEnactProposal : public CvProposal
{
public:
	CvEnactProposal(void);
	CvEnactProposal(int iID, ResolutionTypes eType, LeagueTypes eLeague, PlayerTypes eProposalPlayer, int iChoice = LeagueHelpers::CHOICE_NONE);
	~CvEnactProposal(void);

	void Init();
	bool IsPassed(int iTotalSessionVotes);
	CvString GetProposalName(bool bForLogging = false);

private:
};

FDataStream& operator>>(FDataStream&, CvEnactProposal&);
FDataStream& operator<<(FDataStream&, const CvEnactProposal&);

typedef FStaticVector<CvEnactProposal, 2, false, c_eCiv5GameplayDLL> EnactProposalList;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvActiveResolution
//!  \brief		A Resolution that has been enacted by a League, with active CvResolutionEffects
//
//!  Key Attributes:
//!  - Instantiated using data from an existing CvEnactProposal
//!  - Assigned a unique ID by the CvLeague that enacts it
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvActiveResolution : public CvResolution
{
public:
	CvActiveResolution(void);
	CvActiveResolution(CvEnactProposal* pResolution);
	~CvActiveResolution(void);

	void Init();
	void DoEffects(PlayerTypes ePlayer);
	void RemoveEffects(PlayerTypes ePlayer);
	bool HasOngoingEffects();

	int GetTurnEnacted() const;

	int m_iTurnEnacted;

private:
};

FDataStream& operator>>(FDataStream&, CvActiveResolution&);
FDataStream& operator<<(FDataStream&, const CvActiveResolution&);

typedef FStaticVector<CvActiveResolution, 12, false, c_eCiv5GameplayDLL> ActiveResolutionList;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvRepealProposal
//!  \brief		Proposal to repeal an existing CvActiveResolution
//
//!  Key Attributes:
//!  - Contains the unique ID of the CvActiveResolution to be repealed
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRepealProposal : public CvProposal
{
public:
	CvRepealProposal(void);
	CvRepealProposal(CvActiveResolution* pResolution, PlayerTypes eProposalPlayer);
	~CvRepealProposal(void);

	void Init();
	bool IsPassed(int iTotalSessionVotes);
	CvString GetProposalName(bool bForLogging = false);

	int GetTargetResolutionID() const;
	CvVoterDecision* GetRepealDecision();

	int m_iTargetResolutionID;
	CvVoterDecision m_RepealDecision;

private:
};

FDataStream& operator>>(FDataStream&, CvRepealProposal&);
FDataStream& operator<<(FDataStream&, const CvRepealProposal&);

typedef FStaticVector<CvRepealProposal, 2, false, c_eCiv5GameplayDLL> RepealProposalList;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeague
//!  \brief		A group of players that makes collective decisions on Resolutions
//
//!  Key Attributes:
//!  - Tracks resolutions that are active, up for enactment, and up for repeal
//!  - Tracks members and their statuses
//!  - Queried for certain effects of active Resolutions by other game classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeague
{
public:
	CvLeague(void);
	CvLeague(LeagueTypes eID);
	~CvLeague(void);

	struct Member {
		Member(void);
		~Member(void);
		
		PlayerTypes ePlayer;
		int iExtraVotes;
		CvString sVoteSources;

		bool bMayPropose;
		int iProposals;
		int iVotes;
		int iAbstainedVotes;

		bool bEverBeenHost;
		bool bAlwaysBeenHost;
	};
	typedef FStaticVector<Member, MAX_CIV_PLAYERS, false, c_eCiv5GameplayDLL> MemberList;

	typedef FStaticVector<int, MAX_MAJOR_CIVS, true, c_eCiv5GameplayDLL> ProjectProductionList;
	struct Project {
		Project(void);
		~Project(void);

		LeagueProjectTypes eType;
		ProjectProductionList vProductionList;
		bool bComplete;
		bool bProgressWarningSent;
	};
	typedef FStaticVector<Project, 3, false, c_eCiv5GameplayDLL> ProjectList;

	enum ContributionTier {
		CONTRIBUTION_TIER_0,
		CONTRIBUTION_TIER_1,
		CONTRIBUTION_TIER_2,
		CONTRIBUTION_TIER_3,

		NUM_CONTRIBUTION_TIERS,
	};

	void Init(LeagueSpecialSessionTypes eGoverningSpecialSession);

	void DoTurn(LeagueSpecialSessionTypes eTriggeredSpecialSession = NO_LEAGUE_SPECIAL_SESSION);
	LeagueTypes GetID() const;
	Localization::String GetName();
	bool CanChangeCustomName(PlayerTypes ePlayer);
	void DoChangeCustomName(PlayerTypes ePlayer, const char* szCustomName);

	// Sessions
	int GetSessionTurnInterval();
	bool IsInSession();
	bool IsInSpecialSession();
	void SetInSession(bool bInSession);
	void SetInSession(LeagueSpecialSessionTypes eSpecialSession);
	int GetTurnsUntilSession() const;
	void SetTurnsUntilSession(int iTurns);
	void ChangeTurnsUntilSession(int iChange);
	void ResetTurnsUntilSession();
	int GetTurnsUntilVictorySession();
	int GetVotesSpentThisSession();
	LeagueSpecialSessionTypes GetLastSpecialSession() const;
	LeagueSpecialSessionTypes GetCurrentSpecialSession() const;
	bool CanStartSpecialSession(LeagueSpecialSessionTypes eSpecialSession);

	// Diplomatic Victory
	bool IsUnitedNations() const;
	void SetUnitedNations(bool bValue);

	// Resolutions
	void DoVoteEnact(int iID, PlayerTypes eVoter, int iNumVotes, int iChoice);
	void DoVoteRepeal(int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice);
	void DoVoteAbstain(PlayerTypes eVoter, int iNumVotes);
	void DoProposeEnact(ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice = LeagueHelpers::CHOICE_NONE);
	void DoProposeRepeal(int iResolutionID, PlayerTypes eProposer);
	bool CanProposeEnactAnyChoice(ResolutionTypes eResolution, PlayerTypes eProposer);
	bool CanProposeEnact(ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice, CvString* sTooltipSink = NULL);
	bool CanProposeRepeal(int iTargetResolutionID, PlayerTypes eProposer, CvString* sTooltipSink = NULL);
	bool IsResolutionEffectsValid(ResolutionTypes eResolution, int iProposerChoice, CvString* sTooltipSink = NULL);
	bool IsAnythingProposed();
	bool IsActiveResolution(ResolutionTypes eResolution, int iProposerChoice);
	bool IsProposed(int iResolutionID, bool bRepeal, bool bCheckOnHold = false);
	bool IsEnactProposed(ResolutionTypes eResolution, int iProposerChoice);
	bool IsRepealProposed(int iResolutionID) const;
	std::vector<int> GetChoicesForDecision(ResolutionDecisionTypes eDecision, PlayerTypes eDecider);
	CvString GetTextForChoice(ResolutionDecisionTypes eDecision, int iChoice);
	std::vector<ResolutionTypes> GetInactiveResolutions() const;
	CvEnactProposal* GetEnactProposal(int iResolutionID);
	EnactProposalList GetEnactProposals() const;
	EnactProposalList GetEnactProposalsOnHold() const;
	CvRepealProposal* GetRepealProposal(int iResolutionID);
	RepealProposalList GetRepealProposals() const;
	RepealProposalList GetRepealProposalsOnHold() const;
	CvActiveResolution* GetActiveResolution(int iResolutionID, int iValue = -1);
	ActiveResolutionList GetActiveResolutions() const;
	int GetNumResolutionsEverEnacted() const;
	int GetNumProposersPerSession() const;

	// Members
	void AddMember(PlayerTypes ePlayer);
	void RemoveMember(PlayerTypes ePlayer);
	bool IsMember(PlayerTypes ePlayer) const;
	int GetNumMembers() const;
	bool CanVote(PlayerTypes ePlayer);
	bool CanEverVote(PlayerTypes ePlayer);
	int GetRemainingVotesForMember(PlayerTypes ePlayer);
	int GetSpentVotesForMember(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE_DEALS)
	int GetPotentialVotesForMember(PlayerTypes ePlayer, PlayerTypes eFromPlayer);
#endif
	int GetCoreVotesForMember(PlayerTypes ePlayer);
	int CalculateStartingVotesForMember(PlayerTypes ePlayer, bool bForceUpdateSources = false);
	bool CanPropose(PlayerTypes ePlayer);
	bool CanEverPropose(PlayerTypes ePlayer);
	int GetRemainingProposalsForMember(PlayerTypes ePlayer);
	int GetNumProposalsByMember(PlayerTypes ePlayer);
	LeagueHelpers::PlayerList GetMembersThatLikeProposal(ResolutionTypes eResolution, PlayerTypes eObserver, int iProposerChoice);
	LeagueHelpers::PlayerList GetMembersThatLikeProposal(int iTargetResolutionID, PlayerTypes eObserver);
	LeagueHelpers::PlayerList GetMembersThatDislikeProposal(ResolutionTypes eResolution, PlayerTypes eObserver, int iProposerChoice);
	LeagueHelpers::PlayerList GetMembersThatDislikeProposal(int iTargetResolutionID, PlayerTypes eObserver);

	// Host
	bool HasHostMember() const;
	bool IsHostMember(PlayerTypes ePlayer) const;
	PlayerTypes GetHostMember() const;
	void SetHostMember(PlayerTypes ePlayer);	
	int GetConsecutiveHostedSessions() const;
	void SetConsecutiveHostedSessions(int iValue);
	void ChangeConsecutiveHostedSessions(int iChange);
	bool HasMemberEverBeenHost(PlayerTypes ePlayer);
	bool HasMemberAlwaysBeenHost(PlayerTypes ePlayer);

	// Projects
	bool IsProjectActive(LeagueProjectTypes eLeagueProject) const;
	bool IsProjectComplete(LeagueProjectTypes eLeagueProject) const;
	int GetProjectCostPerPlayer(LeagueProjectTypes eLeagueProject) const;
	int GetProjectBuildingCostPerPlayer(BuildingTypes eRewardBuilding) const;
	int GetProjectCost(LeagueProjectTypes eLeagueProject) const;
	int GetProjectProgress(LeagueProjectTypes eLeagueProject);
	bool CanMemberContribute(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject) const;
	int GetMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject) const;
	void SetMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, int iValue);
	void ChangeMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, int iChange);
	ContributionTier GetMemberContributionTier(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject);
	float GetContributionTierThreshold(ContributionTier eTier, LeagueProjectTypes eLeagueProject);

	// Resolution Effect Queries
	bool IsTradeEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient);
#if defined(MOD_API_EXTENSIONS)
	bool IsPlayerEmbargoed(PlayerTypes ePlayer);
#endif
	bool IsLuxuryHappinessBanned(ResourceTypes eLuxury);
	int GetResearchMod(TechTypes eTech);
	int GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield);
	int GetWorldWonderYieldChange(YieldTypes eYield);
	bool IsNoTrainingNuclearWeapons();
	int GetExtraVotesForFollowingReligion(PlayerTypes ePlayer);
	int GetCityTourismModifier(const CvCity* pCity);
	int GetReligionSpreadStrengthModifier(ReligionTypes eReligion);
	int GetExtraVotesForFollowingIdeology(PlayerTypes ePlayer);
	int GetPressureForIdeology(PolicyBranchTypes eIdeology);
	int GetArtsyGreatPersonRateModifier();
	int GetScienceyGreatPersonRateModifier();
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
	int GetSpaceShipProductionMod();
	int GetSpaceShipPurchaseMod();
	int GetWorldWar();
	int GetUnitMaintenanceMod();
	bool IsIdeologyEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient);
#endif
#if defined(MOD_BALANCE_CORE)
	int GetTourismMod();
	void DoEnactResolutionPublic(CvEnactProposal* pProposal);
	void DoRepealResolutionPublic(CvRepealProposal* pProposal);

#endif

	// Text composition for UI
	CvString GetResolutionName(ResolutionTypes eResolution, int iResolutionID, int iProposerChoice, bool bIncludePrefix);
	CvString GetResolutionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iResolutionID, int iProposerChoice);
	CvString GetResolutionVoteOpinionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iResolutionID);
	CvString GetResolutionProposeOpinionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iProposerChoice);
	CvString GetResolutionProposeOpinionDetails(int iTargetResolutionID, PlayerTypes eObserver);
	CvString GetMemberDetails(PlayerTypes eMember, PlayerTypes eObserver);
	CvString GetMemberDelegationDetails(PlayerTypes eMember, PlayerTypes eObserver);
	CvString GetMemberKnowledgeDetails(PlayerTypes eMember, PlayerTypes eObserver);
	CvString GetMemberVoteOpinionDetails(PlayerTypes eMember, PlayerTypes eObserver);
	CvString GetProjectDetails(LeagueProjectTypes eProject, PlayerTypes eObserver);
	CvString GetProjectProgressDetails(LeagueProjectTypes eProject, PlayerTypes eObserver);
	CvString GetProjectRewardDetails(LeagueProjectTypes eProject, PlayerTypes eObserver);
	CvString GetProjectRewardTierDetails(int iTier, LeagueProjectTypes eProject, PlayerTypes eObserver);
	std::vector<CvString> GetCurrentEffectsSummary(PlayerTypes eObserver);
	CvString GetLeagueSplashTitle(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
	CvString GetLeagueSplashDescription(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
	CvString GetLeagueSplashThisEraDetails(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
	CvString GetLeagueSplashNextEraDetails(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
	CvString GetGreatPersonRateModifierDetails(UnitClassTypes eGreatPersonClass);

	// Achievements
	void CheckProjectAchievements();

	LeagueTypes m_eID;
	bool m_bUnitedNations;
	bool m_bInSession;
	int m_iTurnsUntilSession;
	int m_iNumResolutionsEverEnacted;
	EnactProposalList m_vEnactProposals;
	RepealProposalList m_vRepealProposals;
	ActiveResolutionList m_vActiveResolutions;
	MemberList m_vMembers;
	PlayerTypes m_eHost;
	ProjectList m_vProjects;
	int m_iConsecutiveHostedSessions;
	LeagueNameTypes m_eAssignedName;
	char m_szCustomName[128];
	LeagueSpecialSessionTypes m_eLastSpecialSession;
	LeagueSpecialSessionTypes m_eCurrentSpecialSession;
	EnactProposalList m_vEnactProposalsOnHold;
	RepealProposalList m_vRepealProposalsOnHold;

private:
	void CheckStartSession();
	void StartSession();
	void CheckStartSpecialSession(LeagueSpecialSessionTypes eSpecialSession);
	void StartSpecialSession(LeagueSpecialSessionTypes eSpecialSession);
	void CheckFinishSession();
	void FinishSession();
	void AssignStartingVotes();
	void ClearProposalPrivileges();
	void AssignProposalPrivileges();

	void CheckProposalsValid();
	void CheckResolutionsValid();
	void AssignNewHost();
	
	void DoEnactResolution(CvEnactProposal* pProposal);
	void DoRepealResolution(CvRepealProposal* pProposal);

	void DoClearProposals();
	void DoPutProposalsOnHold();
	bool HasProposalsOnHold();

	void NotifySessionDone();
	void NotifyProposalResult(CvEnactProposal* pProposal);
	void NotifyProposalResult(CvRepealProposal* pProposal);
	void NotifySessionSoon(int iTurnsLeft);
	void NotifyProjectComplete(LeagueProjectTypes eProject);
	void NotifyProjectProgress(LeagueProjectTypes eProject);

	void StartProject(LeagueProjectTypes eLeagueProject);
	void CheckProjectsProgress();
	void DoProjectRewards(LeagueProjectTypes eLeagueProject);
	void DoProjectReward(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, ContributionTier eTier);

	void UpdateName();

	// Logging
	void LogProposalResolved(CvEnactProposal* pProposal);
	void LogProposalResolved(CvRepealProposal* pProposal);

	// Accessors
	Member* GetMember(PlayerTypes ePlayer);
	Project* GetProject(LeagueProjectTypes eLeagueProject);
};

FDataStream& operator>>(FDataStream&, CvLeague&);
FDataStream& operator<<(FDataStream&, const CvLeague&);

typedef FStaticVector<CvLeague, 2, false, c_eCiv5GameplayDLL> LeagueList;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvGameLeagues
//!  \brief		All the information about active Leagues and their Resolutions
//
//!  Key Attributes:
//!  - Contains list of active Leagues in the game
//!  - Queried for information about player membership (CvLeague) and gameplay effects (CvResolution)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameLeagues
{
public:
	CvGameLeagues(void);
	~CvGameLeagues(void);

	void Init();

	// Called each game turn
	void DoTurn();

	// Called each player turn
	void DoPlayerTurn(CvPlayer& kPlayer);

	void FoundLeague(PlayerTypes eFounder = NO_PLAYER);
	void DoPlayerAliveStatusChanged(PlayerTypes ePlayer);
	void DoUnitedNationsBuilt(PlayerTypes eBuilder);
	
	int GetNumActiveLeagues();
	int GetNumLeaguesEverFounded() const;
	EraTypes GetLastEraTrigger() const;
	void SetLastEraTrigger(EraTypes eLastEraTrigger);
	CvLeague* GetLeague(LeagueTypes eLeague);
	CvLeague* GetActiveLeague();

	int GenerateResolutionUniqueID();

	// Projects
	bool CanContributeToLeagueProject(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject);
	void DoLeagueProjectContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, int iValue);

	// Resolution Effects
	PlayerTypes GetDiplomaticVictor() const;
	void SetDiplomaticVictor(PlayerTypes ePlayer);
	bool IsTradeEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient);
	bool IsLuxuryHappinessBanned(PlayerTypes ePlayer, ResourceTypes eLuxury);
	int GetResearchMod(PlayerTypes ePlayer, TechTypes eTech);
	int GetFeatureYieldChange(PlayerTypes ePlayer, FeatureTypes eFeature, YieldTypes eYield);
	int GetWorldWonderYieldChange(PlayerTypes ePlayer, YieldTypes eYield);
	bool IsNoTrainingNuclearWeapons(PlayerTypes ePlayer);
	int GetCityTourismModifier(PlayerTypes ePlayer, const CvCity* pCity);
	int GetReligionSpreadStrengthModifier(PlayerTypes ePlayer, ReligionTypes eReligion);
	int GetPressureForIdeology(PlayerTypes ePlayer, PolicyBranchTypes eIdeology);
	int GetArtsyGreatPersonRateModifier(PlayerTypes ePlayer);
	int GetScienceyGreatPersonRateModifier(PlayerTypes ePlayer);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int GetSpaceShipProductionMod(PlayerTypes ePlayer);
	int GetSpaceShipPurchaseMod(PlayerTypes ePlayer);
	int IsWorldWar(PlayerTypes ePlayer);
	int GetUnitMaintenanceMod(PlayerTypes ePlayer);
	bool IsIdeologyEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient);
#endif

	// General Logging
	CvString GetLogFileName() const;
	void LogLeagueMessage(CvString& kMessage);

	int m_iGeneratedIDCount;
	LeagueList m_vActiveLeagues;
	int m_iNumLeaguesEverFounded;
	PlayerTypes m_eDiplomaticVictor;
	EraTypes m_eLastEraTrigger;

private:
	// Logging
	void LogLeagueFounded(PlayerTypes eFounder);
	void LogSpecialSession(LeagueSpecialSessionTypes eSpecialSession);
};

FDataStream& operator>>(FDataStream&, CvGameLeagues&);
FDataStream& operator<<(FDataStream&, const CvGameLeagues&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueAI
//!  \brief		Player-level AI to direct decisions about Leagues and Resolutions
//
//!  Key Attributes:
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueAI
{
public:
	CvLeagueAI(void);
	~CvLeagueAI(void);
	
	// Increments of how much knowledge we have of another player's desires
	enum KnowledgeLevels {
		KNOWLEDGE_NONE,
		KNOWLEDGE_PARTIAL,
		KNOWLEDGE_INTIMATE,

		NUM_KNOWLEDGE_LEVELS,
	};

	// Increments of how much we want a certain outcome
	enum DesireLevels {
		DESIRE_NEVER,
		DESIRE_STRONG_DISLIKE,
		DESIRE_DISLIKE,
		DESIRE_WEAK_DISLIKE,
		DESIRE_NEUTRAL,
		DESIRE_WEAK_LIKE,
		DESIRE_LIKE,
		DESIRE_STRONG_LIKE,
		DESIRE_ALWAYS,
		
		NUM_DESIRE_LEVELS,
	};

	// Increments of how much we want a diplomat in a civ's capital (for trades, etc.)
	enum DiplomatUsefulnessLevels {
		DIPLOMAT_USEFULNESS_NONE,
		DIPLOMAT_USEFULNESS_LOW,
		DIPLOMAT_USEFULNESS_MEDIUM,
		DIPLOMAT_USEFULNESS_HIGH,

		NUM_DIPLOMAT_USEFULNESS_LEVELS,
	};

	// Increments of how much we think our interests are aligned with another player's
	enum AlignmentLevels {
		ALIGNMENT_WAR,
		
		ALIGNMENT_ENEMY,
		ALIGNMENT_HATRED,
		ALIGNMENT_RIVAL,
		ALIGNMENT_NEUTRAL,
		ALIGNMENT_FRIEND,
		ALIGNMENT_CONFIDANT,
		ALIGNMENT_ALLY,
		
		ALIGNMENT_SELF,
		ALIGNMENT_LEADER,
		ALIGNMENT_LIBERATOR,

		NUM_ALIGNMENT_LEVELS,
	};

	struct VoteCommitment {
		VoteCommitment(void);
		VoteCommitment(PlayerTypes player, int resolutionID, int voteChoice, int numVotes, bool enact);
		~VoteCommitment(void);

		PlayerTypes eToPlayer;
		int iResolutionID;
		int iVoteChoice;
		int iNumVotes;
		bool bEnact;
	};
	typedef FStaticVector<VoteCommitment, 4, false, c_eCiv5GameplayDLL> VoteCommitmentList;

	struct VoteConsideration {
		VoteConsideration(void);
		VoteConsideration(bool enact, int id, int choice);
		~VoteConsideration(void);

		bool bEnact;
		int iID;
		int iChoice;
	};
	typedef CvWeightedVector<VoteConsideration, 32, false> VoteConsiderationList;

	struct ProposalConsideration {
		ProposalConsideration(void);
		ProposalConsideration(bool enact, int index, int choice);
		~ProposalConsideration(void);

		bool bEnact;
		int iIndex;
		int iChoice;
	};
	typedef CvWeightedVector<ProposalConsideration, 128, false> ProposalConsiderationList;

	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	void DoTurn();

	void DoVotes(CvLeague* pLeague);
	void DoAbstainAllVotes(CvLeague* pLeague);
	void DoProposals(CvLeague* pLeague);

	// Deals for votes
	VoteCommitmentList GetDesiredVoteCommitments(PlayerTypes eFromPlayer, bool bFlippedLogic = false);
	bool HasVoteCommitment() const;
	int GetVoteCommitment(PlayerTypes eToPlayer, int iResolutionID, int iVoteChoice, bool bRepeal);
	bool CanCommitVote(PlayerTypes eToPlayer, CvString* sTooltipSink = NULL);
	void AddVoteCommitment(PlayerTypes eToPlayer, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal);
	void CancelVoteCommitmentsToPlayer(PlayerTypes eToPlayer);
	void DoVoteCommitments(CvLeague* pLeague);

	// Naked knowledge for other players
	DesireLevels EvaluateVoteForTrade(int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal);
	DesireLevels EvaluateProposalForProposer(CvLeague* pLeague, PlayerTypes eProposer, ResolutionTypes eResolution, int iProposerChoice = LeagueHelpers::CHOICE_NONE);
	DesireLevels EvaluateProposalForProposer(CvLeague* pLeague, PlayerTypes eProposer, int iTargetResolutionID);
	AlignmentLevels EvaluateAlignment(PlayerTypes ePlayer);

	// Masked knowledge for other players
	KnowledgeLevels GetKnowledgeGivenToOtherPlayer(PlayerTypes eToPlayer, CvString* sTooltipSink = NULL);
	int EvaluateVoteForOtherPlayerKnowledge(CvLeague* pLeague, PlayerTypes eToPlayer, CvEnactProposal* pProposal, CvString* sTooltipSink = NULL);
	int EvaluateVoteForOtherPlayerKnowledge(CvLeague* pLeague, PlayerTypes eToPlayer, CvRepealProposal* pProposal, CvString* sTooltipSink = NULL);

	// Diplomat AI
	DiplomatUsefulnessLevels GetDiplomatUsefulnessAtCiv(PlayerTypes ePlayer);
	int GetExtraVotesPerDiplomat();
	int GetExtraVotesPerCityStateAlly();

	// String composition for UI
	CvString GetCommitVoteDetails(PlayerTypes eToPlayer);

	CvPlayer* GetPlayer();

	CvPlayer* m_pPlayer;

	VoteCommitmentList m_vVoteCommitmentList;

private:
	
	CvString GetTextForDesire(DesireLevels eDesire);
	DesireLevels EvaluateDesire(int iRawEvaluationScore);

	// Voting
	void AllocateVotes(CvLeague* pLeague);
	void FindBestVoteChoices(CvEnactProposal* pProposal, VoteConsiderationList& considerations);
	void FindBestVoteChoices(CvRepealProposal* pProposal, VoteConsiderationList& considerations);
	int ScoreVoteChoice(CvEnactProposal* pProposal, int iChoice);
	int ScoreVoteChoice(CvRepealProposal* pProposal, int iChoice);
	int ScoreVoteChoiceYesNo(CvProposal* pProposal, int iChoice, bool bEnact);
	int ScoreVoteChoicePlayer(CvProposal* pProposal, int iChoice, bool bEnact);

	// Proposing
	void AllocateProposals(CvLeague* pLeague);
	int ScoreProposal(CvLeague* pLeague, ResolutionTypes eResolution, int iChoice = LeagueHelpers::CHOICE_NONE, bool bIsProposing = false);
	int ScoreProposal(CvLeague* pLeague, CvActiveResolution* pResolution, bool bIsProposing = false);

	// Logging
	void LogProposalConsidered(ProposalConsideration* pProposal, int iChoice, int iScore, bool bPre);
	void LogVoteChoiceConsidered(CvEnactProposal* pProposal, int iChoice, int iScore);
	void LogVoteChoiceConsidered(CvRepealProposal* pProposal, int iChoice, int iScore);
	void LogVoteChoiceCommitted(CvEnactProposal* pProposal, int iChoice, int iVotes);
	void LogVoteChoiceCommitted(CvRepealProposal* pProposal, int iChoice, int iVotes);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueSpecialSessionEntry
//!  \brief		A single entry in the special session XML file
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Array of these contained in CvLeagueSpecialSessionXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueSpecialSessionEntry: public CvBaseInfo
{
public:
	CvLeagueSpecialSessionEntry(void);
	~CvLeagueSpecialSessionEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	EraTypes GetEraTrigger() const;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	BuildingTypes GetBuildingTrigger() const;
	ResolutionTypes GetResolutionTrigger() const;
#endif
	ResolutionTypes GetImmediateProposal() const;
	ResolutionTypes GetRecurringProposal() const;
	int GetTurnsBetweenSessions() const;
	int GetCivDelegates() const;
	int GetHostDelegates() const;
	int GetCityStateDelegates() const;
	bool IsUnitedNations() const;

protected:
	EraTypes m_eEraTrigger;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	BuildingTypes m_eBuildingTrigger;
	ResolutionTypes m_eResolutionTrigger;
#endif
	ResolutionTypes m_eImmediateProposal;
	ResolutionTypes m_eRecurringProposal;
	int m_iTurnsBetweenSessions;
	int m_iCivDelegates;
	int m_iHostDelegates;
	int m_iCityStateDelegates;
	bool m_bUnitedNations;


private:
	CvLeagueSpecialSessionEntry(const CvLeagueSpecialSessionEntry&);
	CvLeagueSpecialSessionEntry& operator=(const CvLeagueSpecialSessionEntry&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueSpecialSessionXMLEntries
//!  \brief		Game-wide information about league special sessions
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Contains an array of CvLeagueSpecialSessionEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueSpecialSessionXMLEntries
{
public:
	CvLeagueSpecialSessionXMLEntries(void);
	~CvLeagueSpecialSessionXMLEntries(void);

	// Accessor functions
	std::vector<CvLeagueSpecialSessionEntry*>& GetLeagueSpecialSessionEntries();
	int GetNumLeagueSpecialSessions();
	CvLeagueSpecialSessionEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvLeagueSpecialSessionEntry*> m_paLeagueSpecialSessionEntries;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueNameEntry
//!  \brief		A single entry in the league name variation XML file
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Array of these contained in CvLeagueNameXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueNameEntry
{
public:
	CvLeagueNameEntry(void);
	~CvLeagueNameEntry(void);

	int GetID() const
	{
		return m_iID;
	}

	const char* GetNameKey() const
	{
		return m_strNameKey.c_str();
	}

	const char* GetType() const
	{
		return m_strType.c_str();
	}

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:

private:
	CvLeagueNameEntry(const CvLeagueNameEntry&);
	CvLeagueNameEntry& operator=(const CvLeagueNameEntry&);
	
	int m_iID;
	CvString m_strNameKey;
	CvString m_strType;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueNameXMLEntries
//!  \brief		Game-wide information about league name variations
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Contains an array of CvLeagueNameEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueNameXMLEntries
{
public:
	CvLeagueNameXMLEntries(void);
	~CvLeagueNameXMLEntries(void);

	// Accessor functions
	std::vector<CvLeagueNameEntry*>& GetLeagueNameEntries();
	int GetNumLeagueNames();
	CvLeagueNameEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvLeagueNameEntry*> m_paLeagueNameEntries;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueProjectRewardEntry
//!  \brief		A single entry in the league project reward XML file
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Array of these contained in CvLeagueProjectRewardXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueProjectRewardEntry: public CvBaseInfo
{
public:
	CvLeagueProjectRewardEntry(void);
	~CvLeagueProjectRewardEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	BuildingTypes GetBuilding() const;
	int GetHappiness() const;
	int GetFreeSocialPolicies() const;
	int GetCultureBonusTurns() const;
	int GetTourismBonusTurns() const;
	int GetGoldenAgePoints() const;
	int GetCityStateInfluenceBoost() const;
	int GetBaseBeakersTurnsToCount() const;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int GetAttackBonusTurns() const;
	int GetBaseFreeUnits() const;
	int GetNumFreeGreatPeople() const;
#endif
	UnitClassTypes GetFreeUnitClass() const;

protected:
	BuildingTypes m_eBuilding;
	int m_iHappiness;
	int m_iFreeSocialPolicies;
	int m_iCultureBonusTurns;
	int m_iTourismBonusTurns;
	int m_iGoldenAgePoints;
	int m_iCityStateInfluenceBoost;
	int m_iBaseBeakersTurnsToCount;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int m_iGetAttackBonusTurns;
	int m_iGetBaseFreeUnits;
	int m_iGetNumFreeGreatPeople;
#endif
	UnitClassTypes m_eFreeUnitClass;


private:
	CvLeagueProjectRewardEntry(const CvLeagueProjectRewardEntry&);
	CvLeagueProjectRewardEntry& operator=(const CvLeagueProjectRewardEntry&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueProjectRewardXMLEntries
//!  \brief		Game-wide information about league projects
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Contains an array of CvLeagueProjectRewardEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueProjectRewardXMLEntries
{
public:
	CvLeagueProjectRewardXMLEntries(void);
	~CvLeagueProjectRewardXMLEntries(void);

	// Accessor functions
	std::vector<CvLeagueProjectRewardEntry*>& GetLeagueProjectRewardEntries();
	int GetNumLeagueProjectRewards();
	CvLeagueProjectRewardEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvLeagueProjectRewardEntry*> m_paLeagueProjectRewardEntries;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueProjectEntry
//!  \brief		A single entry in the league project XML file
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Array of these contained in CvLeagueProjectXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueProjectEntry: public CvBaseInfo
{
public:
	CvLeagueProjectEntry(void);
	~CvLeagueProjectEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	ProcessTypes GetProcess() const;
	int GetCostPerPlayer() const;
	LeagueProjectRewardTypes GetRewardTier1() const;
	LeagueProjectRewardTypes GetRewardTier2() const;
	LeagueProjectRewardTypes GetRewardTier3() const;

protected:
	ProcessTypes m_eProcess;
	int m_iCostPerPlayer;
	LeagueProjectRewardTypes m_eRewardTier1;
	LeagueProjectRewardTypes m_eRewardTier2;
	LeagueProjectRewardTypes m_eRewardTier3;

private:
	CvLeagueProjectEntry(const CvLeagueProjectEntry&);
	CvLeagueProjectEntry& operator=(const CvLeagueProjectEntry&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvLeagueProjectXMLEntries
//!  \brief		Game-wide information about league projects
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Contains an array of CvLeagueProjectEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLeagueProjectXMLEntries
{
public:
	CvLeagueProjectXMLEntries(void);
	~CvLeagueProjectXMLEntries(void);

	// Accessor functions
	std::vector<CvLeagueProjectEntry*>& GetLeagueProjectEntries();
	int GetNumLeagueProjects();
	CvLeagueProjectEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvLeagueProjectEntry*> m_paLeagueProjectEntries;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvResolutionEntry
//!  \brief		A single entry in the resolution XML file
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Array of these contained in CvResolutionXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvResolutionEntry: public CvBaseInfo
{
public:
	CvResolutionEntry(void);
	~CvResolutionEntry(void);
	
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	ResolutionDecisionTypes GetVoterDecision() const;
	ResolutionDecisionTypes GetProposerDecision() const;
	TechTypes GetTechPrereqAnyMember() const;
	bool IsAutomaticProposal() const;
	bool IsUniqueType() const;
	bool IsNoProposalByPlayer() const;
	int GetQuorumPercent() const;
	int GetLeadersVoteBonusOnFail() const;
	bool IsDiplomaticVictory() const;
	bool IsChangeLeagueHost() const;
	int GetOneTimeGold() const;
	int GetOneTimeGoldPercent() const;
	bool IsRaiseCityStateInfluenceToNeutral() const;
	LeagueProjectTypes GetLeagueProjectEnabled() const;
	int GetGoldPerTurn() const;
	int GetResourceQuantity() const;
	bool IsEmbargoCityStates() const;
	bool IsEmbargoPlayer() const;
	bool IsNoResourceHappiness() const;
	int GetUnitMaintenanceGoldPercent() const;
	int GetMemberDiscoveredTechMod() const;
	int GetCulturePerWonder() const;
	int GetCulturePerNaturalWonder() const;
	bool IsNoTrainingNuclearWeapons() const;
	int GetVotesForFollowingReligion() const;
	int GetHolyCityTourism() const;
	int GetReligionSpreadStrengthMod() const;
	int GetVotesForFollowingIdeology() const;
	int GetOtherIdeologyRebellionMod() const;
	int GetArtsyGreatPersonRateMod() const;
	int GetScienceyGreatPersonRateMod() const;
	int GetGreatPersonTileImprovementCulture() const;
	int GetLandmarkCulture() const;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	bool IsOpenDoor() const;
	bool IsSphereOfInfluence() const;
	bool IsDecolonization() const;
	int GetSpaceShipProductionMod() const;
	int GetSpaceShipPurchaseMod() const;
	int GetWorldWar() const;
	bool IsEmbargoIdeology() const;
#endif
#if defined(MOD_BALANCE_CORE)
	int GetTourismMod() const;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int GetVassalMaintenanceGoldPercent() const;
	bool IsEndAllCurrentVassals() const;
#endif

protected:
	ResolutionDecisionTypes m_eVoterDecision;
	ResolutionDecisionTypes m_eProposerDecision;
	TechTypes m_eTechPrereqAnyMember;
	bool m_bAutomaticProposal;
	bool m_bUniqueType;
	bool m_bNoProposalByPlayer;
	int m_iQuorumPercent;
	int m_iLeadersVoteBonusOnFail;
	bool m_bDiplomaticVictory;
	bool m_bChangeLeagueHost;
	int m_iOneTimeGold;
	int m_iOneTimeGoldPercent;
	bool m_bRaiseCityStateInfluenceToNeutral;
	LeagueProjectTypes m_eLeagueProjectEnabled;
	int m_iGoldPerTurn;
	int m_iResourceQuantity;
	bool m_bEmbargoCityStates;
	bool m_bEmbargoPlayer;
	bool m_bNoResourceHappiness;
	int m_iUnitMaintenanceGoldPercent;
	int m_iMemberDiscoveredTechMod;
	int m_iCulturePerWonder;
	int m_iCulturePerNaturalWonder;
	bool m_bNoTrainingNuclearWeapons;
	int m_iVotesForFollowingReligion;
	int m_iHolyCityTourism;
	int m_iReligionSpreadStrengthMod;
	int m_iVotesForFollowingIdeology;
	int m_iOtherIdeologyRebellionMod;
	int m_iArtsyGreatPersonRateMod;
	int m_iScienceyGreatPersonRateMod;
	int m_iGreatPersonTileImprovementCulture;
	int m_iLandmarkCulture;
#if defined(MOD_BALANCE_CORE)
	int m_iTourismMod;
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	bool m_bOpenDoor;
	bool m_bSphereOfInfluence;
	bool m_bDecolonization;
	int m_iSpaceshipProductionMod;
	int m_iSpaceshipPurchaseMod;
	int m_iIsWorldWar;
	bool m_bEmbargoIdeology;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int m_iVassalMaintenanceGoldPercent;
	bool m_bEndAllCurrentVassals;
#endif

private:
	CvResolutionEntry(const CvResolutionEntry&);
	CvResolutionEntry& operator=(const CvResolutionEntry&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvResolutionXMLEntries
//!  \brief		Game-wide information about resolutions
//
//!  Key Attributes:
//!  - Populated from XML\GameInfo\CIV5Resolutions.xml
//!  - Contains an array of CvResolutionEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvResolutionXMLEntries
{
public:
	CvResolutionXMLEntries(void);
	~CvResolutionXMLEntries(void);

	std::vector<CvResolutionEntry*>& GetResolutionEntries();
	int GetNumResolutions();
	CvResolutionEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvResolutionEntry*> m_paResolutionEntries;
};

#endif //CIV5_VOTING_CLASSES_H

/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvCitySpecializationAI.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"
#include "ICvDLLUserInterface.h"

// must be included after all other headers
#include "LintFree.h"

//------------------------------------------------------------------------------
CvAIGrandStrategyXMLEntry::CvAIGrandStrategyXMLEntry(void):
m_piFlavorValue(NULL),
m_piSpecializationBoost(NULL),
m_piFlavorModValue(NULL)
{
}
//------------------------------------------------------------------------------
CvAIGrandStrategyXMLEntry::~CvAIGrandStrategyXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piSpecializationBoost);
	SAFE_DELETE_ARRAY(m_piFlavorModValue);
}
//------------------------------------------------------------------------------
bool CvAIGrandStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "AIGrandStrategy_Flavors", "AIGrandStrategyType", szType);
	kUtility.SetYields(m_piSpecializationBoost, "AIGrandStrategy_Yields", "AIGrandStrategyType", szType);
	kUtility.SetFlavors(m_piFlavorModValue, "AIGrandStrategy_FlavorMods", "AIGrandStrategyType", szType);

	return true;
}

/// What Flavors will be added by adopting this Grand Strategy?
int CvAIGrandStrategyXMLEntry::GetFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}

/// What Flavors will be added by adopting this Grand Strategy?
int CvAIGrandStrategyXMLEntry::GetSpecializationBoost(YieldTypes eYield) const
{
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(eYield > -1, "Index out of bounds");
	return m_piSpecializationBoost ? m_piSpecializationBoost[(int)eYield] : 0;
}

/// What Flavors will be added by adopting this Grand Strategy?
int CvAIGrandStrategyXMLEntry::GetFlavorModValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorModValue ? m_piFlavorModValue[i] : 0;
}



//=====================================
// CvAIGrandStrategyXMLEntries
//=====================================
/// Constructor
CvAIGrandStrategyXMLEntries::CvAIGrandStrategyXMLEntries(void)
{

}

/// Destructor
CvAIGrandStrategyXMLEntries::~CvAIGrandStrategyXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of AIStrategy entries
std::vector<CvAIGrandStrategyXMLEntry*>& CvAIGrandStrategyXMLEntries::GetAIGrandStrategyEntries()
{
	return m_paAIGrandStrategyEntries;
}

/// Number of defined AIStrategies
int CvAIGrandStrategyXMLEntries::GetNumAIGrandStrategies()
{
	return m_paAIGrandStrategyEntries.size();
}

/// Clear AIStrategy entries
void CvAIGrandStrategyXMLEntries::DeleteArray()
{
	for (std::vector<CvAIGrandStrategyXMLEntry*>::iterator it = m_paAIGrandStrategyEntries.begin(); it != m_paAIGrandStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIGrandStrategyEntries.clear();
}

/// Get a specific entry
CvAIGrandStrategyXMLEntry *CvAIGrandStrategyXMLEntries::GetEntry(int index)
{
	return m_paAIGrandStrategyEntries[index];
}



//=====================================
// CvGrandStrategyAI
//=====================================
/// Constructor
CvGrandStrategyAI::CvGrandStrategyAI ():
m_paiGrandStrategyPriority(NULL),
m_eGuessOtherPlayerActiveGrandStrategy(NULL),
m_eGuessOtherPlayerActiveGrandStrategyConfidence(NULL)
{
}

/// Destructor
CvGrandStrategyAI::~CvGrandStrategyAI (void)
{
}

/// Initialize
void CvGrandStrategyAI::Init(CvAIGrandStrategyXMLEntries *pAIGrandStrategies, CvPlayer *pPlayer)
{
	// Store off the pointer to the AIStrategies active for this game
	m_pAIGrandStrategies = pAIGrandStrategies;

	m_pPlayer = pPlayer;

	// Initialize AIGrandStrategy status array
	FAssertMsg(m_paiGrandStrategyPriority==NULL, "about to leak memory, CvGrandStrategyAI::m_paiGrandStrategyPriority");
	m_paiGrandStrategyPriority = FNEW(int[m_pAIGrandStrategies->GetNumAIGrandStrategies()], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_eGuessOtherPlayerActiveGrandStrategy==NULL, "about to leak memory, CvGrandStrategyAI::m_eGuessOtherPlayerActiveGrandStrategy");
	m_eGuessOtherPlayerActiveGrandStrategy = FNEW(int[MAX_MAJOR_CIVS], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_eGuessOtherPlayerActiveGrandStrategyConfidence==NULL, "about to leak memory, CvGrandStrategyAI::m_eGuessOtherPlayerActiveGrandStrategyConfidence");
	m_eGuessOtherPlayerActiveGrandStrategyConfidence = FNEW(int[MAX_MAJOR_CIVS], c_eCiv5GameplayDLL, 0);

	Reset();
}

/// Deallocate memory created in initialize
void CvGrandStrategyAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_paiGrandStrategyPriority);
	SAFE_DELETE_ARRAY(m_eGuessOtherPlayerActiveGrandStrategy);
	SAFE_DELETE_ARRAY(m_eGuessOtherPlayerActiveGrandStrategyConfidence);
}

/// Reset AIStrategy status array to all false
void CvGrandStrategyAI::Reset()
{
	int iI;

	m_iNumTurnsSinceActiveSet = 0;

	m_eActiveGrandStrategy = NO_AIGRANDSTRATEGY;

	for (iI = 0; iI < m_pAIGrandStrategies->GetNumAIGrandStrategies(); iI++)
	{
		m_paiGrandStrategyPriority[iI] = -1;
	}

	for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_eGuessOtherPlayerActiveGrandStrategy[iI] = NO_AIGRANDSTRATEGY;
		m_eGuessOtherPlayerActiveGrandStrategyConfidence[iI] = NO_GUESS_CONFIDENCE_TYPE;
	}
}

/// Serialization read
void CvGrandStrategyAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iNumTurnsSinceActiveSet;
	kStream >> (int&)m_eActiveGrandStrategy;

	FAssertMsg(m_pAIGrandStrategies != NULL && m_pAIGrandStrategies->GetNumAIGrandStrategies() > 0, "Number of AIGrandStrategies to serialize is expected to greater than 0");
#ifdef _MSC_VER
// JAR - if m_pAIGrandStrategies can be NULL at this point,
// the load will fail if the data isn't read. Better to crash
// here where the problem is than defer it.
#pragma warning ( push )
#pragma warning ( disable : 6011 )
#endif//_MSC_VER
	ArrayWrapper<int> wrapm_paiGrandStrategyPriority(m_pAIGrandStrategies->GetNumAIGrandStrategies(), m_paiGrandStrategyPriority);
#ifdef _MSC_VER
#pragma warning ( pop )
#endif//_MSC_VER

	kStream >> wrapm_paiGrandStrategyPriority;

	ArrayWrapper<int> wrapm_eGuessOtherPlayerActiveGrandStrategy(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategy);
	kStream >> wrapm_eGuessOtherPlayerActiveGrandStrategy;

	ArrayWrapper<int> wrapm_eGuessOtherPlayerActiveGrandStrategyConfidence(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategyConfidence);
	kStream >> wrapm_eGuessOtherPlayerActiveGrandStrategyConfidence;

}

/// Serialization write
void CvGrandStrategyAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iNumTurnsSinceActiveSet;
	kStream << m_eActiveGrandStrategy;

	FAssertMsg(GC.getNumAIGrandStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << ArrayWrapper<int>(m_pAIGrandStrategies->GetNumAIGrandStrategies(), m_paiGrandStrategyPriority);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategy);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_eGuessOtherPlayerActiveGrandStrategyConfidence);
}

/// Returns the Player object the Strategies are associated with
CvPlayer* CvGrandStrategyAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns AIGrandStrategies object stored in this class
CvAIGrandStrategyXMLEntries* CvGrandStrategyAI::GetAIGrandStrategies()
{
	return m_pAIGrandStrategies;
}

/// Runs every turn to determine what the player's Active Grand Strategy is and to change Priority Levels as necessary
void CvGrandStrategyAI::DoTurn()
{
	DoGuessOtherPlayersActiveGrandStrategy();

	int iGrandStrategiesLoop;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy;
	CvString strGrandStrategyName;

	// Loop through all GrandStrategies to set their Priorities
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;
		pGrandStrategy = GetAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
		strGrandStrategyName = (CvString) pGrandStrategy->GetType();

		// Base Priority looks at Personality Flavors (0 - 10) and multiplies * the Flavors attached to a Grand Strategy (0-10),
		// so expect a number between 0 and 100 back from this
		int iPriority = GetBaseGrandStrategyPriority(eGrandStrategy);

		if (strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
		{
			iPriority += GetConquestPriority();
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
		{
			iPriority += GetCulturePriority();
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
		{
			iPriority += GetUnitedNationsPriority();
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
		{
			iPriority += GetSpaceshipPriority();
		}

		// Random element
		iPriority += GC.getGame().getJonRandNum(/*50*/ GC.getAI_GS_RAND_ROLL(), "Grand Strategy AI: GS rand roll.");

		// Give a boost to the current strategy so that small fluctuation doesn't cause a big change
		if (GetActiveGrandStrategy() == eGrandStrategy && GetActiveGrandStrategy() != NO_AIGRANDSTRATEGY)
		{
			iPriority += /*50*/ GC.getAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT();
		}

		SetGrandStrategyPriority(eGrandStrategy, iPriority);
	}

	// Now look at what we think the other players in the game are up to - we might have an opportunity to capitalize somewhere
	int iNumPlayersAliveAndMet = 0;

	int iMajorLoop;

	for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if (GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
		{
			if (GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER((PlayerTypes) iMajorLoop).getTeam()))
			{
				iNumPlayersAliveAndMet++;
			}
		}
	}

	FStaticVector< int, 5, true, c_eCiv5GameplayDLL > viNumGrandStrategiesAdopted;
	int iNumPlayers;

	// Init vector
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		iNumPlayers = 0;

		// Tally up how many players we think are pusuing each Grand Strategy
		for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			if (GetGuessOtherPlayerActiveGrandStrategy((PlayerTypes) iMajorLoop) == (AIGrandStrategyTypes) iGrandStrategiesLoop)
			{
				iNumPlayers++;
			}
		}

		viNumGrandStrategiesAdopted.push_back(iNumPlayers);
	}

	FStaticVector< int, 5, true, c_eCiv5GameplayDLL > viGrandStrategyChangeForLogging;

	int iChange;

	// Now modify our preferences based on how many people are going for stuff
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;
		// If EVERYONE else we know is also going for this Grand Strategy, reduce our Priority by 50%
		iChange = GetGrandStrategyPriority(eGrandStrategy) * /*50*/ GC.getAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER();
		iChange = iChange * viNumGrandStrategiesAdopted[eGrandStrategy] / iNumPlayersAliveAndMet;
		iChange /= 100;

		ChangeGrandStrategyPriority(eGrandStrategy, -iChange);

		viGrandStrategyChangeForLogging.push_back(-iChange);
	}

	ChangeNumTurnsSinceActiveSet(1);

	// Now see which Grand Strategy should be active, based on who has the highest Priority right now
	// Grand Strategy must be run for at least 10 turns
	if (GetActiveGrandStrategy() == NO_AIGRANDSTRATEGY || GetNumTurnsSinceActiveSet() >= /*10*/ GC.getAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE())
	{
		int iBestPriority = -1;
		int iPriority;

		AIGrandStrategyTypes eBestGrandStrategy = NO_AIGRANDSTRATEGY;

		for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
		{
			eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;

			iPriority = GetGrandStrategyPriority(eGrandStrategy);

			if (iPriority > iBestPriority)
			{
				iBestPriority = iPriority;
				eBestGrandStrategy = eGrandStrategy;
			}
		}

		if (eBestGrandStrategy != GetActiveGrandStrategy())
		{
			SetActiveGrandStrategy(eBestGrandStrategy);
			m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NEW_GRAND_STRATEGY);
		}
	}

	LogGrandStrategies(viGrandStrategyChangeForLogging);
}

/// Returns Priority for Conquest Grand Strategy
int CvGrandStrategyAI::GetConquestPriority()
{
	int iPriority = 0;

	// If Conquest Victory isn't even available then don't bother with anything
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DOMINATION", true);
	if (eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		if (!GC.getGame().areNoVictoriesValid())
		{
			return -100;
		}
	}

	// Boldness gives the base weight for Conquest (no flavors added earlier)
	iPriority += (GetPlayer()->GetDiplomacyAI()->GetBoldness() * 10);

	CvTeam& pTeam = GET_TEAM(GetPlayer()->getTeam());

	// How many turns must have passed before we test for having met nobody?
	if (GC.getGame().getElapsedGameTurns() >= /*20*/ GC.getAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN())
	{
		// If we haven't met any Major Civs yet, then we probably shouldn't be planning on conquering the world
		bool bHasMetMajor = false;

		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			if (pTeam.GetID() != iTeamLoop && !GET_TEAM((TeamTypes) iTeamLoop).isMinorCiv())
			{
				if (pTeam.isHasMet((TeamTypes) iTeamLoop))
				{
					bHasMetMajor = true;
					break;
				}
			}
		}
		if (!bHasMetMajor)
		{
			iPriority += /*-50*/ GC.getAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT();
		}
	}

	// How many turns must have passed before we test for us having a weak military?
	if (GC.getGame().getElapsedGameTurns() >= /*60*/ GC.getAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN())
	{
		// Compare our military strength to the rest of the world
		int iWorldMilitaryStrength = GC.getGame().GetWorldMilitaryStrengthAverage(GetPlayer()->GetID(), true, true);

		if (iWorldMilitaryStrength > 0)
		{
			int iMilitaryRatio = (GetPlayer()->GetMilitaryMight() - iWorldMilitaryStrength) * /*100*/ GC.getAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER() / iWorldMilitaryStrength;

			// Make the likelihood of BECOMING a warmonger lower than dropping the bad behavior
			if (iMilitaryRatio > 0)
				iMilitaryRatio /= 2;

			iPriority += iMilitaryRatio;	// This will add between -100 and 100 depending on this player's MilitaryStrength relative the world average. The number will typically be near 0 though, as it's fairly hard to get away from the world average
		}
	}

	// If we're at war, then boost the weight a bit
	if (pTeam.getAtWarCount(/*bIgnoreMinors*/ false) > 0)
	{
		iPriority += /*10*/ GC.getAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT();
	}

	// If our neighbors are cramping our style, consider less... scrupulous means of obtaining more land
	if (GetPlayer()->IsCramped())
	{
		PlayerTypes ePlayer;
		int iNumPlayersMet = 1;	// Include 1 for me!
		int iTotalLandMe = 0;
		int iTotalLandPlayersMet = 0;

		// Count the number of Majors we know
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			ePlayer = (PlayerTypes) iMajorLoop;

			if (GET_PLAYER(ePlayer).isAlive() && iMajorLoop != GetPlayer()->GetID())
			{
				if (pTeam.isHasMet(GET_PLAYER(ePlayer).getTeam()))
				{
					iNumPlayersMet++;
				}
			}
		}

		if (iNumPlayersMet > 0)
		{
			// Check every plot for ownership
			for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
			{
				if (GC.getMap().plotByIndexUnchecked(iPlotLoop)->isOwned())
				{
					ePlayer = GC.getMap().plotByIndexUnchecked(iPlotLoop)->getOwner();

					if (ePlayer == GetPlayer()->GetID())
					{
						iTotalLandPlayersMet++;
						iTotalLandMe++;
					}
					else if (!GET_PLAYER(ePlayer).isMinorCiv() && pTeam.isHasMet(GET_PLAYER(ePlayer).getTeam()))
					{
						iTotalLandPlayersMet++;
					}
				}
			}

			iTotalLandPlayersMet /= iNumPlayersMet;

			if (iTotalLandMe > 0)
			{
				if (iTotalLandPlayersMet / iTotalLandMe > 0)
				{
					iPriority += /*20*/ GC.getAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT();
				}
			}
		}
	}

	return iPriority;
}

/// Returns Priority for Culture Grand Strategy
int CvGrandStrategyAI::GetCulturePriority()
{
	int iPriority = 0;

	// If Culture Victory isn't even available then don't bother with anything
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_CULTURAL", true);
	if (eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		return -100;
	}

	// for every city lower this
	int iNumCities = m_pPlayer->getNumCities() - m_pPlayer->GetNumPuppetCities();
	int iMaxCultureCities = GC.getAI_GS_CULTURE_MAX_CITIES() + 1;

	iPriority += (iMaxCultureCities - iNumCities) * GC.getAI_GS_CULTURE_MAX_CITIES_MODIFIER();

	// for every completed policy branch increase this
	int iNumBranchesCompleted = m_pPlayer->GetPlayerPolicies()->GetNumPolicyBranchesFinished();
	iPriority += iNumBranchesCompleted * GC.getAI_GS_CULTURE_POLICY_BRANCH_MOD();

	// If we somehow got to 5 branches...
	if (iNumBranchesCompleted > 4)
	{
		return 1000;
	}

	return iPriority;
}

/// Returns Priority for United Nations Grand Strategy
int CvGrandStrategyAI::GetUnitedNationsPriority()
{
	int iPriority = 0;

	// If UN Victory isn't even available then don't bother with anything
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
	if (eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		return -100;
	}

	int iNumMinorsAttacked = GET_TEAM(GetPlayer()->getTeam()).GetNumMinorCivsAttacked();
	iPriority += (iNumMinorsAttacked * /*-30*/ GC.getAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT());

	TeamTypes eTeam = m_pPlayer->getTeam();
	PlayerTypes ePlayer = m_pPlayer->GetID();

	int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
	int iSecuredVotes = 1;
	// if we own the UN for some reason
	if (GET_TEAM(eTeam).IsHomeOfUnitedNations())
	{
		iSecuredVotes += 1;
	}

	// Loop through Players to see if they'll vote for this player
	PlayerTypes eLoopPlayer;
	TeamTypes eLoopTeam;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(eLoopPlayer).isAlive())
		{
			eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

			// Liberated?
			if (GET_TEAM(eLoopTeam).GetLiberatedByTeam() == eTeam)
			{
				iSecuredVotes++;
			}

			// Minor civ?
			else if (GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				// Best Relations?
				if (GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == ePlayer)
				{
					iSecuredVotes++;
				}
			}
		}
	}

	// assume we can get a vote for every 750 gold (but don't bother if the UN isn't in play yet)
	//if (GC.getGame().GetUnitedNationsCountdown() > 0)
	{
		int iGold = max( 0, m_pPlayer->GetTreasury()->GetGold() );
		iSecuredVotes += iGold / 750;
	}

	int iUNPriority = iSecuredVotes * 100;
	iUNPriority /= iVotesNeededToWin;

	iPriority += iUNPriority;

	// if by some fluke we are very close to winning the election, by all means switch focus
	if (iSecuredVotes - iVotesNeededToWin > -1)
	{
		return 1000;
	}

	// factor in some traits that could be useful (or harmful)
	iPriority += m_pPlayer->GetPlayerTraits()->GetCityStateFriendshipModifier();
	iPriority += m_pPlayer->GetPlayerTraits()->GetCityStateBonusModifier();
	iPriority -= m_pPlayer->GetPlayerTraits()->GetCityStateCombatModifier();

	return iPriority;
}

/// Returns Priority for Spaceship Grand Strategy
int CvGrandStrategyAI::GetSpaceshipPriority()
{
	int iPriority = 0;

	// If SS Victory isn't even available then don't bother with anything
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_SPACE_RACE", true);
	if (eVictory == NO_VICTORY || !GC.getGame().isVictoryValid(eVictory))
	{
		return -100;
	}

	// the later the game the greater the chance
	iPriority += m_pPlayer->GetCurrentEra() * 5;

	// if I already built the Apollo Program I am very likely to follow through
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if (eApolloProgram != NO_PROJECT)
	{
		if (GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			iPriority += /*150*/ GC.getAI_GS_SS_HAS_APOLLO_PROGRAM();
		}
	}

	return iPriority;
}

/// Get the base Priority for a Grand Strategy; these are elements common to ALL Grand Strategies
int CvGrandStrategyAI::GetBaseGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy)
{
	CvAIGrandStrategyXMLEntry* pGrandStrategy = GetAIGrandStrategies()->GetEntry(eGrandStrategy);

	int iPriority = 0;

	// Personality effect on Priority
	for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if (pGrandStrategy->GetFlavorValue(iFlavorLoop) != 0)
		{
			iPriority += (pGrandStrategy->GetFlavorValue(iFlavorLoop) * GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes) iFlavorLoop));
		}
	}

	return iPriority;
}

/// Get the base Priority for a Grand Strategy; these are elements common to ALL Grand Strategies
int CvGrandStrategyAI::GetPersonalityAndGrandStrategy(FlavorTypes eFlavorType)
{
	if (m_eActiveGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		CvAIGrandStrategyXMLEntry* pGrandStrategy = GetAIGrandStrategies()->GetEntry(m_eActiveGrandStrategy);
		int iModdedFlavor = pGrandStrategy->GetFlavorModValue(eFlavorType) + m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorType);
		iModdedFlavor = max(0,iModdedFlavor);
		return iModdedFlavor;
	}
	return m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorType);
}

/// Returns the Active Grand Strategy for this Player: how am I trying to win right now?
AIGrandStrategyTypes CvGrandStrategyAI::GetActiveGrandStrategy() const
{
	return m_eActiveGrandStrategy;
}

/// Sets the Active Grand Strategy for this Player: how am I trying to win right now?
void CvGrandStrategyAI::SetActiveGrandStrategy(AIGrandStrategyTypes eGrandStrategy)
{
	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		m_eActiveGrandStrategy = eGrandStrategy;

		SetNumTurnsSinceActiveSet(0);
	}
}

/// The number of turns since the Active Strategy was last set
int CvGrandStrategyAI::GetNumTurnsSinceActiveSet() const
{
	return m_iNumTurnsSinceActiveSet;
}

/// Set the number of turns since the Active Strategy was last set
void CvGrandStrategyAI::SetNumTurnsSinceActiveSet(int iValue)
{
	m_iNumTurnsSinceActiveSet = iValue;
	FAssert(m_iNumTurnsSinceActiveSet >= 0);
}

/// Change the number of turns since the Active Strategy was last set
void CvGrandStrategyAI::ChangeNumTurnsSinceActiveSet(int iChange)
{
	if (iChange != 0)
	{
		m_iNumTurnsSinceActiveSet += iChange;
	}

	FAssert(m_iNumTurnsSinceActiveSet >= 0);
}

/// Returns the Priority Level the player has for a particular Grand Strategy
int CvGrandStrategyAI::GetGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy) const
{
	FAssert(eGrandStrategy != NO_AIGRANDSTRATEGY);
	return m_paiGrandStrategyPriority[eGrandStrategy];
}

/// Sets the Priority Level the player has for a particular Grand Strategy
void CvGrandStrategyAI::SetGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy, int iValue)
{
	FAssert(eGrandStrategy != NO_AIGRANDSTRATEGY);
	m_paiGrandStrategyPriority[eGrandStrategy] = iValue;
}

/// Changes the Priority Level the player has for a particular Grand Strategy
void CvGrandStrategyAI::ChangeGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy, int iChange)
{
	FAssert(eGrandStrategy != NO_AIGRANDSTRATEGY);

	if (iChange != 0)
	{
		m_paiGrandStrategyPriority[eGrandStrategy] += iChange;
	}
}



// **********
// Stuff relating to guessing what other Players are up to
// **********



/// Runs every turn to try and figure out what other known Players' Grand Strategies are
void CvGrandStrategyAI::DoGuessOtherPlayersActiveGrandStrategy()
{
	CvWeightedVector<int, 5, true> vGrandStrategyPriorities;
	FStaticVector< int, 5, true, c_eCiv5GameplayDLL >  vGrandStrategyPrioritiesForLogging;

	GuessConfidenceTypes eGuessConfidence = NO_GUESS_CONFIDENCE_TYPE;

	int iGrandStrategiesLoop = 0;
	AIGrandStrategyTypes eGrandStrategy = NO_AIGRANDSTRATEGY;
	CvAIGrandStrategyXMLEntry* pGrandStrategy = 0;
	CvString strGrandStrategyName;

	CvTeam& pTeam = GET_TEAM(GetPlayer()->getTeam());

	int iMajorLoop = 0;
	PlayerTypes eMajor = NO_PLAYER;

	int iPriority = 0;

	// Establish world Military strength average
	int iWorldMilitaryAverage = GC.getGame().GetWorldMilitaryStrengthAverage(GetPlayer()->GetID(), true, true);

	// Establish world culture average
	int iNumPlayersAlive = 0;
	int iWorldCultureAverage = 0;
	for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if (GET_PLAYER(eMajor).isAlive())
		{
			iWorldCultureAverage += GET_PLAYER(eMajor).GetJONSCultureEverGenerated();
			iNumPlayersAlive++;
		}
	}
	iWorldCultureAverage /= iNumPlayersAlive;

	// Establish world Tech progress average
	iNumPlayersAlive = 0;
	int iWorldNumTechsAverage = 0;
	TeamTypes eTeam;
	for (int iTeamLoop = 0; iTeamLoop < MAX_MAJOR_CIVS; iTeamLoop++)	// Looping over all MAJOR teams
	{
		eTeam = (TeamTypes) iTeamLoop;

		if (GET_TEAM(eTeam).isAlive())
		{
			iWorldNumTechsAverage += GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown();
			iNumPlayersAlive++;
		}
	}
	iWorldNumTechsAverage /= iNumPlayersAlive;

	// Look at every Major we've met
	for (iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if (GET_PLAYER(eMajor).isAlive() && iMajorLoop != GetPlayer()->GetID())
		{
			if (pTeam.isHasMet(GET_PLAYER(eMajor).getTeam()))
			{
				for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GetAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
				{
					eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategiesLoop;
					pGrandStrategy = GetAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
					strGrandStrategyName = (CvString) pGrandStrategy->GetType();

					if (strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
					{
						iPriority = GetGuessOtherPlayerConquestPriority(eMajor, iWorldMilitaryAverage);
					}
					else if (strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
					{
						iPriority = GetGuessOtherPlayerCulturePriority(eMajor, iWorldCultureAverage);
					}
					else if (strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
					{
						iPriority = GetGuessOtherPlayerUnitedNationsPriority(eMajor);
					}
					else if (strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
					{
						iPriority = GetGuessOtherPlayerSpaceshipPriority(eMajor, iWorldNumTechsAverage);
					}

					vGrandStrategyPriorities.push_back(iGrandStrategiesLoop, iPriority);
					vGrandStrategyPrioritiesForLogging.push_back(iPriority);
				}

				if (vGrandStrategyPriorities.size() > 0)
				{
					// Add "No Grand Strategy" in case we just don't have enough info to go on
					iPriority = /*40*/ GC.getAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT();

					vGrandStrategyPriorities.push_back(NO_AIGRANDSTRATEGY, iPriority);
					vGrandStrategyPrioritiesForLogging.push_back(iPriority);

					vGrandStrategyPriorities.SortItems();

					eGrandStrategy = (AIGrandStrategyTypes) vGrandStrategyPriorities.GetElement(0);
					iPriority = vGrandStrategyPriorities.GetWeight(0);
					eGuessConfidence = NO_GUESS_CONFIDENCE_TYPE;

					// How confident are we in our Guess?
					if (eGrandStrategy != NO_AIGRANDSTRATEGY)
					{
						if (iPriority >= /*120*/ GC.getAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD())
						{
							eGuessConfidence = GUESS_CONFIDENCE_POSITIVE;
						}
						else if (iPriority >= /*70*/ GC.getAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD())
						{
							eGuessConfidence = GUESS_CONFIDENCE_LIKELY;
						}
						else
						{
							eGuessConfidence = GUESS_CONFIDENCE_UNSURE;
						}
					}

					SetGuessOtherPlayerActiveGrandStrategy(eMajor, eGrandStrategy, eGuessConfidence);

					LogGuessOtherPlayerGrandStrategy(vGrandStrategyPrioritiesForLogging, eMajor);
				}

				vGrandStrategyPriorities.clear();
				vGrandStrategyPrioritiesForLogging.clear();
			}
		}
	}
}

/// What does this AI BELIEVE another player's Active Grand Strategy to be?
AIGrandStrategyTypes CvGrandStrategyAI::GetGuessOtherPlayerActiveGrandStrategy(PlayerTypes ePlayer) const
{
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	return (AIGrandStrategyTypes) m_eGuessOtherPlayerActiveGrandStrategy[ePlayer];
}

/// How confident is the AI in its guess of what another player's Active Grand Strategy is?
GuessConfidenceTypes CvGrandStrategyAI::GetGuessOtherPlayerActiveGrandStrategyConfidence(PlayerTypes ePlayer) const
{
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	return (GuessConfidenceTypes) m_eGuessOtherPlayerActiveGrandStrategyConfidence[ePlayer];
}

/// Sets what this AI BELIEVES another player's Active Grand Strategy to be
void CvGrandStrategyAI::SetGuessOtherPlayerActiveGrandStrategy(PlayerTypes ePlayer, AIGrandStrategyTypes eGrandStrategy, GuessConfidenceTypes eGuessConfidence)
{
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	m_eGuessOtherPlayerActiveGrandStrategy[ePlayer] = eGrandStrategy;
	m_eGuessOtherPlayerActiveGrandStrategyConfidence[ePlayer] = eGuessConfidence;
}

/// Guess as to how much another Player is prioritizing Conquest as his means of winning the game
int CvGrandStrategyAI::GetGuessOtherPlayerConquestPriority(PlayerTypes ePlayer, int iWorldMilitaryAverage)
{
	int iConquestPriority = 0;

	// Compare their Military to the world average; Possible range is 100 to -100 (but will typically be around -20 to 20)
	if (iWorldMilitaryAverage > 0)
	{
		iConquestPriority += (GET_PLAYER(ePlayer).GetMilitaryMight() - iWorldMilitaryAverage) * /*100*/ GC.getAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER() / iWorldMilitaryAverage;
	}

	// Minors attacked
	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMinorsAttacked(ePlayer) * /*5*/ GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED());

	// Minors Conquered
	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMinorsConquered(ePlayer) * /*10*/ GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED());

	// Majors attacked
	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMajorsAttacked(ePlayer) * /*10*/ GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED());

	// Majors Conquered
	iConquestPriority += (GetPlayer()->GetDiplomacyAI()->GetOtherPlayerNumMajorsConquered(ePlayer) * /*15*/ GC.getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED());

	return iConquestPriority;
}

/// Guess as to how much another Player is prioritizing Culture as his means of winning the game
int CvGrandStrategyAI::GetGuessOtherPlayerCulturePriority(PlayerTypes ePlayer, int iWorldCultureAverage)
{
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_CULTURAL", true);

	// If Culture Victory isn't even available then don't bother with anything
	if (eVictory == NO_VICTORY)
	{
		return -100;
	}

	int iCulturePriority = 0;

	// Compare their Culture to the world average; Possible range is 150 to -150
	if (iWorldCultureAverage > 0)
	{
		iCulturePriority += (GET_PLAYER(ePlayer).GetJONSCultureEverGenerated() - iWorldCultureAverage) * /*150*/ GC.getAI_GS_CULTURE_RATIO_MULTIPLIER() / iWorldCultureAverage;
	}

	// If we're early in the game, reduce the priority
	if (GC.getGame().getElapsedGameTurns() < /*50*/ GC.getAI_GS_CULTURE_RATIO_EARLY_TURN_THRESHOLD())
	{
		iCulturePriority /= /*2*/ GC.getAI_GS_CULTURE_RATIO_EARLY_DIVISOR();
	}

	return iCulturePriority;
}

/// Guess as to how much another Player is prioritizing the UN as his means of winning the game
int CvGrandStrategyAI::GetGuessOtherPlayerUnitedNationsPriority(PlayerTypes ePlayer)
{
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);

	// If UN Victory isn't even available then don't bother with anything
	if (eVictory == NO_VICTORY)
	{
		return -100;
	}

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();

	int iSecuredVotes = 0;

	// Loop through Players to see if they'll vote for this player
	PlayerTypes eLoopPlayer;
	TeamTypes eLoopTeam;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(eLoopPlayer).isAlive())
		{
			eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

			// Liberated?
			if (GET_TEAM(eLoopTeam).GetLiberatedByTeam() == eTeam)
			{
				iSecuredVotes++;
			}

			// Minor civ?
			else if (GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				// Best Relations?
				if (GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == ePlayer)
				{
					iSecuredVotes++;
				}
			}
		}
	}

	int iUNPriority = iSecuredVotes * /*300*/ GC.getAI_GS_UN_SECURED_VOTE_MOD();
	iUNPriority /= iVotesNeededToWin;

	return iUNPriority;
}

/// Guess as to how much another Player is prioritizing the SS as his means of winning the game
int CvGrandStrategyAI::GetGuessOtherPlayerSpaceshipPriority(PlayerTypes ePlayer, int iWorldNumTechsAverage)
{
	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_SPACE_RACE", true);

	// If SS Victory isn't even available then don't bother with anything
	if (eVictory == NO_VICTORY)
	{
		return -100;
	}

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	// If the player has the Apollo Program we're pretty sure he's going for the SS
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if (eApolloProgram != NO_PROJECT)
	{
		if (GET_TEAM(eTeam).getProjectCount(eApolloProgram) > 0)
		{
			return /*150*/ GC.getAI_GS_SS_HAS_APOLLO_PROGRAM();
		}
	}

	int iNumTechs = GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown();

	// Don't divide by zero, okay?
	if (iWorldNumTechsAverage == 0)
		iWorldNumTechsAverage = 1;

	int iSSPriority = (iNumTechs - iWorldNumTechsAverage) * /*300*/ GC.getAI_GS_SS_TECH_PROGRESS_MOD() / iWorldNumTechsAverage;

	return iSSPriority;
}


// PRIVATE METHODS

/// Log GrandStrategy state: what are the Priorities and who is Active?
void CvGrandStrategyAI::LogGrandStrategies(const FStaticVector< int, 5, true, c_eCiv5GameplayDLL >& vModifiedGrandStrategyPriorities)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "GrandStrategyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "GrandStrategyAI_Log.csv";
		}

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		AIGrandStrategyTypes eGrandStrategy;

		// Loop through Grand Strategies
		for (int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
		{
			// Get the leading info for this line
			strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";

			eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategyLoop;

			// GrandStrategy Info
			if (GetActiveGrandStrategy() == eGrandStrategy)
			{
				strTemp.Format ("*** %s, %d, %d", GC.getAIGrandStrategyInfo(eGrandStrategy)->GetType(), GetGrandStrategyPriority(eGrandStrategy), vModifiedGrandStrategyPriorities[eGrandStrategy]);
			}
			else
			{
				strTemp.Format ("%s, %d, %d", GC.getAIGrandStrategyInfo(eGrandStrategy)->GetType(), GetGrandStrategyPriority(eGrandStrategy), vModifiedGrandStrategyPriorities[eGrandStrategy]);
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log our guess as to other Players' Active Grand Strategy
void CvGrandStrategyAI::LogGuessOtherPlayerGrandStrategy(const FStaticVector< int, 5, true, c_eCiv5GameplayDLL >& vGrandStrategyPriorities, PlayerTypes ePlayer)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "GrandStrategyAI_Guess_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "GrandStrategyAI_Guess_Log.csv";
		}

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		AIGrandStrategyTypes eGrandStrategy;
		int iPriority;

		// Loop through Grand Strategies
		for (int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
		{
			// Get the leading info for this line
			strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
			strBaseString += playerName + ", ";

			eGrandStrategy = (AIGrandStrategyTypes) iGrandStrategyLoop;
			iPriority = vGrandStrategyPriorities[iGrandStrategyLoop];

			// GrandStrategy Info
			if (GetActiveGrandStrategy() == eGrandStrategy)
			{
				strTemp.Format ("*** %s, %d", GC.getAIGrandStrategyInfo(eGrandStrategy)->GetType(), iPriority);
			}
			else
			{
				strTemp.Format ("%s, %d", GC.getAIGrandStrategyInfo(eGrandStrategy)->GetType(), iPriority);
			}
			otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
			strOutBuf = strBaseString + otherPlayerName + ", " + strTemp;

			if (GetGuessOtherPlayerActiveGrandStrategy(ePlayer) == eGrandStrategy)
			{
				// Confidence in our guess
				switch (GetGuessOtherPlayerActiveGrandStrategyConfidence(ePlayer))
				{
				case GUESS_CONFIDENCE_POSITIVE:
					strTemp.Format ("Positive");
					break;
				case GUESS_CONFIDENCE_LIKELY:
					strTemp.Format ("Likely");
					break;
				case GUESS_CONFIDENCE_UNSURE:
					strTemp.Format ("Unsure");
					break;
				default:
					strTemp.Format ("XXX");
					break;
				}

				strOutBuf += ", " + strTemp;
			}

			pLog->Msg(strOutBuf);
		}

		// One more entry for NO GRAND STRATEGY
		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		iPriority = vGrandStrategyPriorities[GC.getNumAIGrandStrategyInfos()];

		// GrandStrategy Info
		strTemp.Format ("NO_GRAND_STRATEGY, %d", iPriority);
		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + otherPlayerName + ", " + strTemp;
		pLog->Msg(strOutBuf);
	}
}
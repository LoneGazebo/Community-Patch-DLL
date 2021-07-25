/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "CvMinorCivAI.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvTechAI.h"
#include "cvStopWatch.h"
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
#include "CvMilitaryAI.h"
#endif

#include "LintFree.h"


// ================================================================================
//			LeagueHelpers
// ================================================================================
CvString LeagueHelpers::GetTextForChoice(ResolutionDecisionTypes eDecision, int iChoice)
{
	CvString s = "";
	switch (eDecision)
	{
	case RESOLUTION_DECISION_YES_OR_NO:
	case RESOLUTION_DECISION_REPEAL:
		{
			CvAssertMsg(iChoice == CHOICE_NO || iChoice == CHOICE_YES, "Unexpected choice when handling a Yes/No decision. Please send Anton your save file and version.");
			if (iChoice == CHOICE_NO)
			{
				s = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_NO").toUTF8();
			}
			else if (iChoice == CHOICE_YES)
			{
				s = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_YES").toUTF8();
			}
			break;
		}
	case RESOLUTION_DECISION_ANY_MEMBER:
	case RESOLUTION_DECISION_MAJOR_CIV_MEMBER:
	case RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER:
		{
			CvAssertMsg(iChoice >= 0 && iChoice < MAX_CIV_PLAYERS, "Unexpected choice when handling a decision on a player. Please send Anton your save file and version.");
			if (iChoice >= 0 && iChoice < MAX_CIV_PLAYERS)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_PLAYER");
				sTemp << GET_PLAYER((PlayerTypes)iChoice).getCivilizationShortDescriptionKey();
				s = sTemp.toUTF8();
			}
			break;
		}
	case RESOLUTION_DECISION_CITY:
		{
			s = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_CITY").toUTF8();
			break;
		}
	case RESOLUTION_DECISION_ANY_LUXURY_RESOURCE:
		{		
			CvResourceInfo* pInfo = GC.getResourceInfo((ResourceTypes)iChoice);
			CvAssertMsg(pInfo != NULL, "Unexpected choice when handling a decision on a resource. Please send Anton your save file and version.");
			if (pInfo != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_RESOURCE");
				sTemp << pInfo->GetIconString() << pInfo->GetDescriptionKey();
				s = sTemp.toUTF8();
			}
			break;
		}
	case RESOLUTION_DECISION_RELIGION:
		{
			CvReligionEntry* pInfo = GC.getReligionInfo((ReligionTypes)iChoice);
			CvAssertMsg(pInfo != NULL, "Unexpected choice when handling a decision on a religion. Please send Anton your save file and version.");
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion((ReligionTypes)iChoice, NO_PLAYER);
			CvAssert(pReligion != NULL);
			if (pInfo != NULL && pReligion != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_RELIGION");
				sTemp << pInfo->GetIconString() << pReligion->GetName();
				s = sTemp.toUTF8();
			}
			break;
		}
	case RESOLUTION_DECISION_IDEOLOGY:
		{
			CvPolicyBranchEntry* pInfo = GC.getPolicyBranchInfo((PolicyBranchTypes)iChoice);
			CvAssertMsg(pInfo != NULL, "Unexpected choice when handling a decision on an ideology. Please send Anton your save file and version.");
			if (pInfo != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_IDEOLOGY");
				sTemp << pInfo->GetDescriptionKey();
				s = sTemp.toUTF8();
			}
			break;
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	case RESOLUTION_DECISION_CITY_CSD:
		{
			if (iChoice >= 0 && iChoice < MAX_CIV_PLAYERS)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_RESOLUTION_CHOICE_CITY_CSD");
				sTemp << GET_PLAYER((PlayerTypes)iChoice).getCivilizationShortDescriptionKey();
				s = sTemp.toUTF8();
			}
			break;
		}
#endif
	default:
		{
			break;
		}
	}
	return s;
}

EraTypes LeagueHelpers::GetGameEraForTrigger()
{
	EraTypes eGameEra = NO_ERA;

	// Game era is one era less than the most advanced player
	EraTypes eMostAdvancedEra = NO_ERA;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			EraTypes e = GET_PLAYER((PlayerTypes)i).GetCurrentEra();
			if (GET_PLAYER((PlayerTypes)i).GetCurrentEra() > eMostAdvancedEra)
			{
				eMostAdvancedEra = e;
			}
		}
	}
	if (eMostAdvancedEra - 1 > NO_ERA)
	{
		eGameEra = (EraTypes) ((int)eMostAdvancedEra - 1);
	}

	// Unless half or more civs are in this era too, then it is this era
	int iInMostAdvancedEra = 0;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			EraTypes e = GET_PLAYER((PlayerTypes)i).GetCurrentEra();
			CvAssert(e <= eMostAdvancedEra);
			if (e == eMostAdvancedEra)
			{
				iInMostAdvancedEra++;
			}
		}
	}
	if (iInMostAdvancedEra >= (GC.getGame().countMajorCivsAlive() / 2))
	{
		eGameEra = eMostAdvancedEra;
	}

	return eGameEra;
}

EraTypes LeagueHelpers::GetNextGameEraForTrigger()
{
	return LeagueHelpers::GetNextGameEraForTrigger(LeagueHelpers::GetGameEraForTrigger());
}

EraTypes LeagueHelpers::GetNextGameEraForTrigger(EraTypes eThisEra)
{
	EraTypes eNextEra = (EraTypes) ((int)eThisEra + 1);

	if (eNextEra >= GC.getNumEraInfos())
	{
		eNextEra = NO_ERA;
	}

	return eNextEra;
}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
BuildingTypes LeagueHelpers::GetBuildingForTrigger(BuildingTypes eBuilding)
{
	if(eBuilding != NO_BUILDING)
	{
		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAlive())
			{
				CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)i);
				
				CvCity* pLoopCity;
				int iLoop;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if(pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						return eBuilding;
						break;
					}
				}
			}
		}
	}
	return NO_BUILDING;
}
ResolutionTypes LeagueHelpers::IsResolutionForTriggerActive(ResolutionTypes eType)
{
	if(eType != NO_RESOLUTION)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		ActiveResolutionList vActiveResolutions = pLeague->GetActiveResolutions();
		for (ActiveResolutionList::iterator it = vActiveResolutions.begin(); it != vActiveResolutions.end(); ++it)
		{
			if (it->GetType() == eType)
			{
				return eType;
				break;
			}
		}
	}
	return NO_RESOLUTION;
}
#endif


// ================================================================================
//			CvResolutionEffects
// ================================================================================
CvResolutionEffects::CvResolutionEffects(void)
{
	bDiplomaticVictory = false;
	bChangeLeagueHost = false;
	iOneTimeGold = 0;
	iOneTimeGoldPercent = 0;
	bRaiseCityStateInfluenceToNeutral = false;
	eLeagueProjectEnabled = NO_LEAGUE_PROJECT;
	iGoldPerTurn = 0;
	iResourceQuantity = 0;
	bEmbargoCityStates = false;
	bEmbargoPlayer = false;
	bNoResourceHappiness = false;
	iUnitMaintenanceGoldPercent = 0;
	iMemberDiscoveredTechMod = 0;
	iCulturePerWonder = 0;
	iCulturePerNaturalWonder = 0;
	bNoTrainingNuclearWeapons = false;
	iVotesForFollowingReligion = 0;
	iHolyCityTourism = 0;
	iReligionSpreadStrengthMod = 0;
	iVotesForFollowingIdeology = 0;
	iOtherIdeologyRebellionMod = 0;
	iArtsyGreatPersonRateMod = 0;
	iScienceyGreatPersonRateMod = 0;
	iGreatPersonTileImprovementCulture = 0;
	iLandmarkCulture = 0;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	bOpenDoor = false;
	bSphereOfInfluence = false;
	bDecolonization = false;
	iLimitSpaceshipProduction = 0;
	iLimitSpaceshipPurchase = 0;
	iIsWorldWar = 0;
	bEmbargoIdeology = false;
#endif
#if defined(MOD_BALANCE_CORE)
	iChangeTourism = 0;
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	iVassalMaintenanceGoldPercent = 0;
	bEndAllCurrentVassals = false;
#endif
}

bool CvResolutionEffects::SetType(ResolutionTypes eType)
{
	CvResolutionEntry* pInfo = GC.getResolutionInfo(eType);	
	CvAssertMsg(pInfo, "Resolution info is null when instantiating ResolutionEffects. Please send Anton your save file and version.");
	if (pInfo)
	{
		bDiplomaticVictory					= pInfo->IsDiplomaticVictory();
		bChangeLeagueHost					= pInfo->IsChangeLeagueHost();
		iOneTimeGold						= pInfo->GetOneTimeGold();
		iOneTimeGoldPercent					= pInfo->GetOneTimeGoldPercent();
		bRaiseCityStateInfluenceToNeutral	= pInfo->IsRaiseCityStateInfluenceToNeutral();
		eLeagueProjectEnabled				= pInfo->GetLeagueProjectEnabled();
		iGoldPerTurn						= pInfo->GetGoldPerTurn();
		iResourceQuantity					= pInfo->GetResourceQuantity();
		bEmbargoCityStates					= pInfo->IsEmbargoCityStates();
		bEmbargoPlayer						= pInfo->IsEmbargoPlayer();
		bNoResourceHappiness				= pInfo->IsNoResourceHappiness();
		iUnitMaintenanceGoldPercent			= pInfo->GetUnitMaintenanceGoldPercent();
		iMemberDiscoveredTechMod			= pInfo->GetMemberDiscoveredTechMod();
		iCulturePerWonder					= pInfo->GetCulturePerWonder();
		iCulturePerNaturalWonder			= pInfo->GetCulturePerNaturalWonder();
		bNoTrainingNuclearWeapons			= pInfo->IsNoTrainingNuclearWeapons();
		iVotesForFollowingReligion			= pInfo->GetVotesForFollowingReligion();
		iHolyCityTourism					= pInfo->GetHolyCityTourism();
		iReligionSpreadStrengthMod			= pInfo->GetReligionSpreadStrengthMod();
		iVotesForFollowingIdeology			= pInfo->GetVotesForFollowingIdeology();
		iOtherIdeologyRebellionMod			= pInfo->GetOtherIdeologyRebellionMod();
		iArtsyGreatPersonRateMod			= pInfo->GetArtsyGreatPersonRateMod();
		iScienceyGreatPersonRateMod			= pInfo->GetScienceyGreatPersonRateMod();
		iGreatPersonTileImprovementCulture	= pInfo->GetGreatPersonTileImprovementCulture();
		iLandmarkCulture					= pInfo->GetLandmarkCulture();
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		bOpenDoor							= pInfo->IsOpenDoor();
		bSphereOfInfluence					= pInfo->IsSphereOfInfluence();
		bDecolonization						= pInfo->IsDecolonization();
		iLimitSpaceshipProduction			= pInfo->GetSpaceShipProductionMod();
		iLimitSpaceshipPurchase				= pInfo->GetSpaceShipPurchaseMod();
		iIsWorldWar							= pInfo->GetWorldWar();
		bEmbargoIdeology					= pInfo->IsEmbargoIdeology();
#endif
#if defined(MOD_BALANCE_CORE)
		iChangeTourism						= pInfo->GetTourismMod();
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		iVassalMaintenanceGoldPercent		= pInfo->GetVassalMaintenanceGoldPercent();
		bEndAllCurrentVassals				= pInfo->IsEndAllCurrentVassals();
#endif
		return true;
	}
	return false;
}

CvResolutionEffects::~CvResolutionEffects(void)
{
}

bool CvResolutionEffects::HasOngoingEffects() const
{
	if (iGoldPerTurn != 0)
		return true;

	if (iResourceQuantity != 0)
		return true;

	if (bEmbargoCityStates)
		return true;

	if (bEmbargoPlayer)
		return true;

	if (bNoResourceHappiness)
		return true;

	if (iUnitMaintenanceGoldPercent != 0)
		return true;

	if (iMemberDiscoveredTechMod != 0)
		return true;

	if (iCulturePerWonder != 0)
		return true;

	if (iCulturePerNaturalWonder != 0)
		return true;

	if (bNoTrainingNuclearWeapons)
		return true;

	if (iVotesForFollowingReligion != 0)
		return true;

	if (iHolyCityTourism != 0)
		return true;

	if (iReligionSpreadStrengthMod != 0)
		return true;

	if (iVotesForFollowingIdeology != 0)
		return true;

	if (iOtherIdeologyRebellionMod != 0)
		return true;

	if (iArtsyGreatPersonRateMod != 0)
		return true;

	if (iScienceyGreatPersonRateMod != 0)
		return true;

	if (iGreatPersonTileImprovementCulture != 0)
		return true;

	if (iLandmarkCulture != 0)
		return true;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && iVassalMaintenanceGoldPercent != 0)
		return true;
#endif

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
		if (iLimitSpaceshipProduction != 0)
			return true;

		if (iLimitSpaceshipPurchase != 0)
			return true;

		if (iIsWorldWar != 0)
			return true;

		if (bEmbargoIdeology)
			return true;

		if(bOpenDoor)
			return true;

		if(bSphereOfInfluence)
			return true;
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if(iChangeTourism != 0)
		return true;
#endif

	return false;
}

// Used to sum multiple CvResolutionEffects and reflect them in a single struct.
// Some information may be incomplete (ex. trade embargo which targets a player
// chosen by proposer), so client should track additional information as necessary.
void CvResolutionEffects::AddOngoingEffects(const CvResolutionEffects* pOtherEffects)
{
	if (!pOtherEffects->HasOngoingEffects())
		return;

	iGoldPerTurn							+= pOtherEffects->iGoldPerTurn;
	iResourceQuantity						+= pOtherEffects->iResourceQuantity; // target resource
	bEmbargoCityStates						|= pOtherEffects->bEmbargoCityStates;
	bEmbargoPlayer							|= pOtherEffects->bEmbargoPlayer; // target player
	bNoResourceHappiness					|= pOtherEffects->bNoResourceHappiness; // target resource
	iUnitMaintenanceGoldPercent				+= pOtherEffects->iUnitMaintenanceGoldPercent;
	iMemberDiscoveredTechMod				+= pOtherEffects->iMemberDiscoveredTechMod;
	iCulturePerWonder						+= pOtherEffects->iCulturePerWonder;
	iCulturePerNaturalWonder				+= pOtherEffects->iCulturePerNaturalWonder;
	bNoTrainingNuclearWeapons				|= pOtherEffects->bNoTrainingNuclearWeapons;
	iVotesForFollowingReligion				+= pOtherEffects->iVotesForFollowingReligion; // target religion
	iHolyCityTourism						+= pOtherEffects->iHolyCityTourism; // target religion
	iReligionSpreadStrengthMod				+= pOtherEffects->iReligionSpreadStrengthMod; // target religion
	iVotesForFollowingIdeology				+= pOtherEffects->iVotesForFollowingIdeology; // target ideology
	iOtherIdeologyRebellionMod				+= pOtherEffects->iOtherIdeologyRebellionMod; // target ideology
	iArtsyGreatPersonRateMod				+= pOtherEffects->iArtsyGreatPersonRateMod;
	iScienceyGreatPersonRateMod				+= pOtherEffects->iScienceyGreatPersonRateMod;
	iGreatPersonTileImprovementCulture		+= pOtherEffects->iGreatPersonTileImprovementCulture;
	iLandmarkCulture						+= pOtherEffects->iLandmarkCulture;

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	iLimitSpaceshipProduction				+= pOtherEffects->iLimitSpaceshipProduction; //Global
	iLimitSpaceshipPurchase					+= pOtherEffects->iLimitSpaceshipPurchase; //Global
	iIsWorldWar								+= pOtherEffects->iIsWorldWar; //Global
	bEmbargoIdeology						|= pOtherEffects->bEmbargoIdeology; // target ideology
	bOpenDoor								|= pOtherEffects->bOpenDoor;
	bSphereOfInfluence						|= pOtherEffects->bSphereOfInfluence;
#endif
#if defined(MOD_BALANCE_CORE)
	iChangeTourism							+= pOtherEffects->iChangeTourism; //Global
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	iVassalMaintenanceGoldPercent			+= pOtherEffects->iVassalMaintenanceGoldPercent;
#endif
}

template<typename ResolutionEffects, typename Visitor>
void CvResolutionEffects::Serialize(ResolutionEffects& resolutionEffects, Visitor& visitor)
{
	visitor(resolutionEffects.bDiplomaticVictory);
	visitor(resolutionEffects.bChangeLeagueHost);
	visitor(resolutionEffects.iOneTimeGold);
	visitor(resolutionEffects.iOneTimeGoldPercent);
	visitor(resolutionEffects.bRaiseCityStateInfluenceToNeutral);
	visitor(resolutionEffects.eLeagueProjectEnabled);
	visitor(resolutionEffects.iGoldPerTurn);
	visitor(resolutionEffects.iResourceQuantity);
	visitor(resolutionEffects.bEmbargoCityStates);
	visitor(resolutionEffects.bEmbargoPlayer);
	visitor(resolutionEffects.bNoResourceHappiness);
	visitor(resolutionEffects.iUnitMaintenanceGoldPercent);
	visitor(resolutionEffects.iMemberDiscoveredTechMod);
	visitor(resolutionEffects.iCulturePerWonder);
	visitor(resolutionEffects.iCulturePerNaturalWonder);
	visitor(resolutionEffects.bNoTrainingNuclearWeapons);
	visitor(resolutionEffects.iVotesForFollowingReligion);
	visitor(resolutionEffects.iHolyCityTourism);
	visitor(resolutionEffects.iReligionSpreadStrengthMod);
	visitor(resolutionEffects.iVotesForFollowingIdeology);
	visitor(resolutionEffects.iOtherIdeologyRebellionMod);
	visitor(resolutionEffects.iArtsyGreatPersonRateMod);
	visitor(resolutionEffects.iScienceyGreatPersonRateMod);
	visitor(resolutionEffects.iGreatPersonTileImprovementCulture);
	visitor(resolutionEffects.iLandmarkCulture);
	visitor(resolutionEffects.bOpenDoor);
	visitor(resolutionEffects.bSphereOfInfluence);
	visitor(resolutionEffects.bDecolonization);
	visitor(resolutionEffects.iLimitSpaceshipProduction);
	visitor(resolutionEffects.iLimitSpaceshipPurchase);
	visitor(resolutionEffects.iIsWorldWar);
	visitor(resolutionEffects.bEmbargoIdeology);
	visitor(resolutionEffects.iChangeTourism);
	visitor(resolutionEffects.iVassalMaintenanceGoldPercent);
	visitor(resolutionEffects.bEndAllCurrentVassals);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvResolutionEffects& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvResolutionEffects::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvResolutionEffects& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvResolutionEffects::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvResolutionDecision
// ================================================================================
CvResolutionDecision::CvResolutionDecision(void)
{
	m_eType = RESOLUTION_DECISION_NONE;
}

CvResolutionDecision::CvResolutionDecision(ResolutionDecisionTypes eType)
{
	m_eType = eType;
}

CvResolutionDecision::~CvResolutionDecision(void)
{
}

CvResolutionDecision::PlayerVote::PlayerVote(void)
{
	ePlayer = NO_PLAYER;
	iNumVotes = 0;
	iChoice = LeagueHelpers::CHOICE_NONE;
}

CvResolutionDecision::PlayerVote::~PlayerVote(void)
{
}

ResolutionDecisionTypes CvResolutionDecision::GetType() const
{
	return m_eType;
}

template<typename PlayerVoteT, typename Visitor>
void CvResolutionDecision::PlayerVote::Serialize(PlayerVoteT& playerVote, Visitor& visitor)
{
	visitor(playerVote.ePlayer);
	visitor(playerVote.iNumVotes);
	visitor(playerVote.iChoice);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvResolutionDecision::PlayerVote& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvResolutionDecision::PlayerVote::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvResolutionDecision::PlayerVote& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvResolutionDecision::PlayerVote::Serialize(readFrom, serialVisitor);
	return saveTo;
}

template<typename ResolutionDecision, typename Visitor>
void CvResolutionDecision::Serialize(ResolutionDecision& resolutionDecision, Visitor& visitor)
{
	visitor(resolutionDecision.m_eType);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvResolutionDecision& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvResolutionDecision::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvResolutionDecision& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvResolutionDecision::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvProposerDecision
// ================================================================================
CvProposerDecision::CvProposerDecision(void)
{
}

// Constructor sets the proposer's decision, made beforehand
CvProposerDecision::CvProposerDecision(ResolutionDecisionTypes eType, PlayerTypes eProposalPlayer, int iChoice) : CvResolutionDecision(eType)
{
	m_sVote.ePlayer = eProposalPlayer;
	m_sVote.iNumVotes = 1;
	m_sVote.iChoice = iChoice;
}

CvProposerDecision::~CvProposerDecision(void)
{
}

int CvProposerDecision::GetDecision()
{
	return m_sVote.iChoice;
}

PlayerTypes CvProposerDecision::GetProposer()
{
	return m_sVote.ePlayer;
}

template<typename ProposerDecision, typename Visitor>
void CvProposerDecision::Serialize(ProposerDecision& proposerDecision, Visitor& visitor)
{
	proposerDecision.CvResolutionDecision::serialize(visitor);
	visitor(proposerDecision.m_sVote);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvProposerDecision& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvProposerDecision::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvProposerDecision& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvProposerDecision::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvVoterDecision
// ================================================================================
CvVoterDecision::CvVoterDecision(void)
{
	m_vVotes.clear();
}

CvVoterDecision::CvVoterDecision(ResolutionDecisionTypes eType) : CvResolutionDecision(eType)
{
	m_vVotes.clear();
}

CvVoterDecision::~CvVoterDecision(void)
{
}

int CvVoterDecision::GetDecision()
{
	CvWeightedVector<int> vChoices;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); it++)
	{
		bool bFirst = true;
		
		// Add the votes to an existing choice tally
		for (int i = 0; i < vChoices.size(); i++)
		{
			if (vChoices.GetElement(i) == it->iChoice)
			{
				bFirst = false;
				vChoices.SetWeight(i, vChoices.GetWeight(i) + it->iNumVotes);
				break;
			}
		}

		// Add the votes to a new tally for a new choice
		if (bFirst)
		{
			vChoices.push_back(it->iChoice, it->iNumVotes);
		}
	}

	if (vChoices.size() > 0)
	{
		vChoices.SortItems();
		
		// Is there a tie?
		if (vChoices.size() > 1)
		{
			if (vChoices.GetWeight(0) == vChoices.GetWeight(1))
			{
				switch (GetType())
				{
					// Decisions about players go to the host in a tie, as long as the host is part of the tie
					case RESOLUTION_DECISION_ANY_MEMBER:
					case RESOLUTION_DECISION_MAJOR_CIV_MEMBER:
					case RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER:
						{
							PlayerTypes eHost = NO_PLAYER;
							CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
							if (pLeague != NULL)
							{
								eHost = pLeague->GetHostMember();
							}

							int iWeightAtTop = vChoices.GetWeight(0);
							for (int i = 0; i < vChoices.size(); i++)
							{
								CvAssert(vChoices.GetWeight(i) <= iWeightAtTop);
								if (vChoices.GetWeight(i) < iWeightAtTop)
								{
									break;
								}

								if ((PlayerTypes)vChoices.GetElement(i) == eHost)
								{
									return eHost;
								}
							}
						}
						break;
					// All other decisions are CHOICE_NONE in a tie, so nothing happens
					default:
						{
							return LeagueHelpers::CHOICE_NONE;
						}
				}
			}
		}
		
		return vChoices.GetElement(0);
	}
	return LeagueHelpers::CHOICE_NONE;
}

bool CvVoterDecision::IsTie()
{
	std::vector<int> vTop = GetTopVotedChoices(1);

	return (vTop.size() != 1);
}

std::vector<int> CvVoterDecision::GetTopVotedChoices(int iNumTopChoices)
{
	CvWeightedVector<int> vChoices;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); it++)
	{
		bool bFirst = true;

		// Add the votes to an existing choice tally
		for (int i = 0; i < vChoices.size(); i++)
		{
			if (vChoices.GetElement(i) == it->iChoice)
			{
				bFirst = false;
				vChoices.SetWeight(i, vChoices.GetWeight(i) + it->iNumVotes);
				break;
			}
		}

		// Add the votes to a new tally for a new choice
		if (bFirst)
		{
			vChoices.push_back(it->iChoice, it->iNumVotes);
		}
	}

	std::vector<int> vTopChoices;
	if (vChoices.size() > 0 && iNumTopChoices > 0)
	{
		vChoices.SortItems();
		int iCurrentWeight = 0;
		for (int i = 0; i < vChoices.size(); i++)
		{
			if ((int)vTopChoices.size() < iNumTopChoices)
			{
				vTopChoices.push_back(vChoices.GetElement(i));
				iCurrentWeight = vChoices.GetWeight(i);
			}
			else if (iCurrentWeight == vChoices.GetWeight(i))
			{
				// Include tied choices
				vTopChoices.push_back(vChoices.GetElement(i));
			}
			else
			{
				break;
			}
		}
	}

	return vTopChoices;
}

int CvVoterDecision::GetVotesCast()
{
	int iCount = 0;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); it++)
	{
		iCount += it->iNumVotes;
	}
	return iCount;
}

int CvVoterDecision::GetVotesCastForChoice(int iChoice)
{
	int iCount = 0;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); it++)
	{
		if (it->iChoice == iChoice)
		{
			iCount += it->iNumVotes;
		}
	}
	return iCount;
}

int CvVoterDecision::GetVotesMarginOfTopChoice()
{
	int iDelta = 0;
	std::vector<int> vTopChoices = GetTopVotedChoices(2);
	if (vTopChoices.size() >= 2)
	{
		iDelta = GetVotesCastForChoice(vTopChoices[0]) - GetVotesCastForChoice(vTopChoices[1]);
	}
	else if (vTopChoices.size() >= 1)
	{
		iDelta = GetVotesCastForChoice(vTopChoices[0]);
	}
	CvAssert(iDelta >= 0);
	return iDelta;
}

int CvVoterDecision::GetVotesCastByPlayer(PlayerTypes ePlayer)
{
	int iCount = 0;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); ++it)
	{
		if (it->ePlayer == ePlayer)
		{
			iCount += it->iNumVotes;
		}
	}
	return iCount;
}

int CvVoterDecision::GetPercentContributionToOutcome(PlayerTypes eVoter, int iChoice, bool bChangeHost)
{
	if (eVoter < 0 || eVoter >= MAX_MAJOR_CIVS) return 0;

	int iTotalVotes = 0;
	int iVotes = 0;

	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); it++)
	{
		if (it->iChoice == iChoice)
		{
			iTotalVotes += it->iNumVotes;

			if (it->ePlayer == eVoter)
			{
				iVotes += it->iNumVotes;
			}
		}
		else if (!bChangeHost && it->ePlayer == eVoter)
		{
			iVotes -= it->iNumVotes;
		}
	}

	if (iVotes <= 0)
		return 0;

	return (iVotes * 100) / max(iTotalVotes, 1);
}

LeagueHelpers::PlayerList CvVoterDecision::GetPlayersVotingForChoice(int iChoice)
{
	LeagueHelpers::PlayerList v;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); ++it)
	{
		if (it->iChoice == iChoice)
		{
			v.push_back(it->ePlayer);
		}
	}
	return v;
}

void CvVoterDecision::ProcessVote(PlayerTypes eVoter, int iNumVotes, int iChoice)
{
	bool bFound = false;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); ++it)
	{
		if (it->ePlayer == eVoter && it->iChoice == iChoice)
		{
			bFound = true;
			it->iNumVotes += iNumVotes;
		}
	}

	if (!bFound)
	{
		PlayerVote vote;
		vote.ePlayer = eVoter;
		vote.iNumVotes = iNumVotes;
		vote.iChoice = iChoice;
		m_vVotes.push_back(vote);
	}
}

// For use with notifications
CvString CvVoterDecision::GetVotesAsText(CvLeague* pLeague)
{
	CvString s = "";

	std::vector<LeagueHelpers::VoteTextSortElement> vVoteText;
	for (PlayerVoteList::iterator it = m_vVotes.begin(); it != m_vVotes.end(); it++)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_MEMBER_VOTE");
		sTemp << it->iNumVotes << pLeague->GetTextForChoice(GetType(), it->iChoice) << GET_PLAYER(it->ePlayer).getCivilizationShortDescriptionKey();
		
		LeagueHelpers::VoteTextSortElement voteTextData;
		voteTextData.sText = sTemp.toUTF8();
		voteTextData.iChoice = it->iChoice;
		voteTextData.iChoiceVotesTotal = GetVotesCastForChoice(it->iChoice);
		voteTextData.iChoiceVotesThisPlayer = it->iNumVotes;

		vVoteText.push_back(voteTextData);
	}

	std::stable_sort(vVoteText.begin(), vVoteText.end(), LeagueHelpers::VoteTextSorter());
	int iNumToShow = MIN(16, (int)vVoteText.size());
	for (int i = 0; i < iNumToShow; i++)
	{
		s += vVoteText[i].sText;
		if (i < iNumToShow - 1)
		{
			s += "[NEWLINE]";
		}
	}

	return s;
}

template<typename VoterDecision, typename Visitor>
void CvVoterDecision::Serialize(VoterDecision& voterDecision, Visitor& visitor)
{
	voterDecision.CvResolutionDecision::serialize(visitor);
	visitor(voterDecision.m_vVotes);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvVoterDecision& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvVoterDecision::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvVoterDecision& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvVoterDecision::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvResolution
// ================================================================================
CvResolution::CvResolution(void)
{
	m_iID = -1;
	m_eType = NO_RESOLUTION;
	m_eLeague = NO_LEAGUE;
	m_sEffects = CvResolutionEffects();
}

CvResolution::CvResolution(int iID, ResolutionTypes eType, LeagueTypes eLeague)
{
	m_iID = iID;
	m_eType = eType;
	m_eLeague = eLeague;
	m_sEffects.SetType(m_eType);
}

CvResolution::~CvResolution(void)
{
}

int CvResolution::GetID() const
{
	return m_iID;
}

ResolutionTypes CvResolution::GetType() const
{
	return m_eType;
}

LeagueTypes CvResolution::GetLeague() const
{
	return m_eLeague;
}

CvResolutionEffects* CvResolution::GetEffects()
{
	return &m_sEffects;
}

CvVoterDecision* CvResolution::GetVoterDecision()
{
	return &m_VoterDecision;
}

CvProposerDecision* CvResolution::GetProposerDecision()
{
	return &m_ProposerDecision;
}

CvString CvResolution::GetName()
{
	CvString s = "";

	CvResolutionEntry* pInfo = GC.getResolutionInfo(GetType());
	CvAssert(pInfo);
	if (pInfo)
	{
		s += Localization::Lookup(pInfo->GetDescriptionKey()).toUTF8();
		if (GetProposerDecision()->GetDecision() != LeagueHelpers::CHOICE_NONE)
		{
			s += ": "; //antonjs: temp
			s += LeagueHelpers::GetTextForChoice(GetProposerDecision()->GetType(), GetProposerDecision()->GetDecision());
		}
	}

	return s;
}

template<typename Resolution, typename Visitor>
void CvResolution::Serialize(Resolution& resolution, Visitor& visitor)
{
	visitor(resolution.m_iID);
	visitor(resolution.m_eType);
	visitor(resolution.m_eLeague);
	visitor(resolution.m_sEffects);
	visitor(resolution.m_VoterDecision);
	visitor(resolution.m_ProposerDecision);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvResolution& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvResolution::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvResolution& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvResolution::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvProposal
// ================================================================================
CvProposal::CvProposal(void)
{
	m_eProposalPlayer = NO_PLAYER;
}

CvProposal::CvProposal(int iID, ResolutionTypes eType, LeagueTypes eLeague, PlayerTypes eProposalPlayer) : CvResolution(iID, eType, eLeague)
{
	m_eProposalPlayer = eProposalPlayer;
}

CvProposal::~CvProposal(void)
{
}

PlayerTypes CvProposal::GetProposalPlayer() const
{
	return m_eProposalPlayer;
}

template<typename Proposal, typename Visitor>
void CvProposal::Serialize(Proposal& proposal, Visitor& visitor)
{
	proposal.CvResolution::serialize(visitor);
	visitor(proposal.m_eProposalPlayer);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvProposal& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvProposal::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvProposal& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvProposal::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvEnactProposal
// ================================================================================
CvEnactProposal::CvEnactProposal(void)
{
}

CvEnactProposal::CvEnactProposal(int iID, ResolutionTypes eType, LeagueTypes eLeague, PlayerTypes eProposalPlayer, int iChoice) : CvProposal(iID, eType, eLeague, eProposalPlayer)
{
	CvResolutionEntry* pInfo = GC.getResolutionInfo(eType);
	CvAssertMsg(pInfo, "Resolution info is null. Please send Anton your save file and version.");
	if (pInfo)
	{
		m_VoterDecision = CvVoterDecision(pInfo->GetVoterDecision());
		m_ProposerDecision = CvProposerDecision(pInfo->GetProposerDecision(), eProposalPlayer, iChoice);
	}
}

CvEnactProposal::~CvEnactProposal(void)
{
}

void CvEnactProposal::Init()
{
}

bool CvEnactProposal::IsPassed(int iTotalSessionVotes)
{
	CvResolutionEntry* pInfo = GC.getResolutionInfo(GetType());
	CvAssert(pInfo);
	if (!pInfo)
	{
		return false;
	}

	int iQuorumPercent = pInfo->GetQuorumPercent();
	bool bQuorum = ((float)GetVoterDecision()->GetVotesCast() / (float)iTotalSessionVotes) >= ((float)iQuorumPercent / 100.0f);
	if (!bQuorum)
	{
		return false;
	}

	int iDecision = GetVoterDecision()->GetDecision();

	if (GetEffects()->bDiplomaticVictory)
	{
		// World Leader only passes if there is no tie and someone has enough votes cast for them
		if (!GetVoterDecision()->IsTie())
		{
			int iVotesForTop = GetVoterDecision()->GetVotesCastForChoice(iDecision);
			int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
			if (iVotesForTop >= iVotesNeededToWin)
			{
				return true;
			}
		}
		return false;
	}
	else if (GetEffects()->bChangeLeagueHost)
	{
		// Choose Host always passes, it's just a matter of who won
		return true;
	}
	else if (GetVoterDecision()->GetType() == RESOLUTION_DECISION_YES_OR_NO)
	{
		// Normal resolutions only pass on YES, not NO or NONE
		return (iDecision == LeagueHelpers::CHOICE_YES);
	}

	CvAssertMsg(false, "Unexpected case when determining whether an enact proposal has passed. Please send Anton your save file and version.");
	return false;
}

CvString CvEnactProposal::GetProposalName(bool bForLogging)
{
	CvString s = "";

	if (bForLogging)
	{
		s += "ENACT: ";
	}
	else
	{
		s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_ENACT").toUTF8();
	}
	s += CvResolution::GetName();

	return s;
}

template<typename EnactProposal, typename Visitor>
void CvEnactProposal::Serialize(EnactProposal& enactProposal, Visitor& visitor)
{
	enactProposal.CvProposal::serialize(visitor);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvEnactProposal& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvEnactProposal::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvEnactProposal& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvEnactProposal::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvActiveResolution
// ================================================================================
CvActiveResolution::CvActiveResolution(void)
{
	m_iTurnEnacted = -1;
}

CvActiveResolution::CvActiveResolution(CvEnactProposal* pResolution) : CvResolution(pResolution->GetID(), pResolution->GetType(), pResolution->GetLeague())
{
	m_iTurnEnacted = -1;
	m_VoterDecision = (*(pResolution->GetVoterDecision()));
	m_ProposerDecision = (*(pResolution->GetProposerDecision()));
}

CvActiveResolution::~CvActiveResolution(void)
{
}

void CvActiveResolution::Init()
{
}

void CvActiveResolution::DoEffects(PlayerTypes ePlayer)
{
	CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
	CvAssertMsg(pPlayer != NULL, "Player is null when doing effects of an active resolution. Please send Anton your save file and version.");
	if (pPlayer == NULL) return;

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(GetLeague());
	CvAssertMsg(pLeague != NULL, "League is null when doing effects of an active resolution. Please send Anton your save file and version.");
	if (pLeague == NULL) return;

	// == Proposer Choices ==
	ResolutionDecisionTypes eProposerDecision = GetProposerDecision()->GetType();
	PlayerTypes eTargetPlayer = NO_PLAYER;
	if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
		eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
		eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
	{
		eTargetPlayer = (PlayerTypes) GetProposerDecision()->GetDecision();
	}
	ResourceTypes eTargetLuxury = NO_RESOURCE;
	if (eProposerDecision == RESOLUTION_DECISION_ANY_LUXURY_RESOURCE)
	{
		CvResourceInfo* pInfo = GC.getResourceInfo((ResourceTypes) GetProposerDecision()->GetDecision());
		if (pInfo && pInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
		{
			eTargetLuxury = (ResourceTypes) GetProposerDecision()->GetDecision();
		}
	}
	ReligionTypes eTargetReligion = NO_RELIGION;
	if (eProposerDecision == RESOLUTION_DECISION_RELIGION)
	{
		eTargetReligion = (ReligionTypes) GetProposerDecision()->GetDecision();
	}
	PolicyBranchTypes eTargetIdeology = NO_POLICY_BRANCH_TYPE;
	if (eProposerDecision == RESOLUTION_DECISION_IDEOLOGY)
	{
		eTargetIdeology = (PolicyBranchTypes) GetProposerDecision()->GetDecision();
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	PlayerTypes eTargetCityState = NO_PLAYER;
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eProposerDecision == RESOLUTION_DECISION_CITY_CSD)
	{
		eTargetCityState = (PlayerTypes) GetProposerDecision()->GetDecision();
	}
#endif

	// == Voter Choices ==
	ResolutionDecisionTypes eVoterDecision = GetVoterDecision()->GetType();
	PlayerTypes eVotedPlayer = NO_PLAYER;
	if (eVoterDecision == RESOLUTION_DECISION_ANY_MEMBER ||
		eVoterDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		eVoterDecision == RESOLUTION_DECISION_CITY_CSD ||
#endif
		eVoterDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
	{
		eVotedPlayer = (PlayerTypes) GetVoterDecision()->GetDecision();
	}

	// == One Time Effects ==
	if (GetEffects()->bDiplomaticVictory)
	{
		CvAssertMsg(eTargetPlayer != NO_PLAYER || eVotedPlayer != NO_PLAYER, "Diplomatic Victory voted for NO_PLAYER. Please send Anton your save file and version.");
		CvAssertMsg(eTargetPlayer == NO_PLAYER || eVotedPlayer == NO_PLAYER, "Ambiguous target when setting Diplomatic Victory. Please send Anton your save file and version");
		if (ePlayer == eVotedPlayer || ePlayer == eTargetPlayer)
		{
			GC.getGame().GetGameLeagues()->SetDiplomaticVictor(ePlayer);
		}
	}
	if (GetEffects()->bChangeLeagueHost)
	{
		CvAssertMsg(eTargetPlayer != NO_PLAYER || eVotedPlayer != NO_PLAYER, "Changing host to NO_PLAYER. Please send Anton your save file and version.");
		CvAssertMsg(eTargetPlayer == NO_PLAYER || eVotedPlayer == NO_PLAYER, "Ambiguous target when changing host. Please send Anton your save file and version");
		if (ePlayer == eVotedPlayer || ePlayer == eTargetPlayer)
		{
			pLeague->SetHostMember(ePlayer);
		}
	}
	if (GetEffects()->iOneTimeGold != 0)
	{
		pPlayer->GetTreasury()->ChangeGold(GetEffects()->iOneTimeGold);
	}
	if (GetEffects()->iOneTimeGoldPercent != 0)
	{
		int iGold = pPlayer->GetTreasury()->GetGold();
		int iChange = (iGold * GetEffects()->iOneTimeGoldPercent) / 100;
		pPlayer->GetTreasury()->ChangeGold(iChange);
	}
	if (GetEffects()->bRaiseCityStateInfluenceToNeutral)
	{
		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{
			int iNeutral = GC.getMINOR_FRIENDSHIP_ANCHOR_DEFAULT();
			for (int iMinor = MAX_MAJOR_CIVS; iMinor < MAX_CIV_PLAYERS; iMinor++)
			{
				PlayerTypes eMinor = (PlayerTypes) iMinor;
				if(eMinor == NO_PLAYER)
				{
					continue;
				}
				if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).GetMinorCivAI()->GetBaseFriendshipWithMajor(ePlayer) < iNeutral)
				{
					if (pLeague->IsMember(eMinor))
					{
						GET_PLAYER(eMinor).GetMinorCivAI()->SetFriendshipWithMajor(ePlayer, iNeutral);
					}
				}
			}
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
	{
		if (GetEffects()->bOpenDoor)
		{
			if (!GET_PLAYER(ePlayer).isMinorCiv())
			{
				if (GET_PLAYER(ePlayer).isAlive())
				{
					if (GET_PLAYER(eTargetCityState).isMinorCiv() && eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isAlive())
					{
						if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eTargetCityState).getTeam()))
						{
							GET_TEAM(GET_PLAYER(ePlayer).getTeam()).meet(GET_PLAYER(eTargetCityState).getTeam(), false);
						}
						GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetFriendshipWithMajor(ePlayer, 40);
					}
					GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetNoAlly(true);
					GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetAlly(NO_PLAYER);
				}
			}	
		}
	} 
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
	{
		if (GetEffects()->bSphereOfInfluence)
		{
			PlayerTypes eOriginalProposer = GetProposerDecision()->GetProposer();
			if(eOriginalProposer != NO_PLAYER && GET_PLAYER(eOriginalProposer).isAlive() && eOriginalProposer == ePlayer)
			{
				if (eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isMinorCiv() && GET_PLAYER(eTargetCityState).isAlive())
				{
					GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetAlly(eOriginalProposer);
					GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetPermanentAlly(eOriginalProposer);
				}
			}
		}	

		if (GetEffects()->bDecolonization)
		{
			if (eTargetPlayer != NO_PLAYER && GET_PLAYER(eTargetPlayer).isAlive())
			{
				PlayerTypes eLoopPlayer;
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					eLoopPlayer = (PlayerTypes) iPlayerLoop;
					if(GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMinorCiv() && (GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == eTargetPlayer) && GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetPermanentAlly() != eTargetPlayer)
					{
						GET_PLAYER(eLoopPlayer).GetMinorCivAI()->SetFriendshipWithMajor(eTargetPlayer, 50);
					}
				}
			}
		}
	}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && GetEffects()->bEndAllCurrentVassals)
	{
		TeamTypes eTeam = pPlayer->getTeam();
		if(eTeam != NO_TEAM && GET_TEAM(eTeam).GetNumVassals() > 0 && !pPlayer->IsVassalsNoRebel())
		{
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if(GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsVassal(eTeam))
				{
					GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).DoEndVassal(eTeam, true, true);
				}
			}
		}
	}
#endif
	// == Ongoing Effects ==
	if (GetEffects()->iGoldPerTurn != 0)
	{
		pPlayer->GetTreasury()->ChangeGoldPerTurnFromDiplomacy(GetEffects()->iGoldPerTurn);
	}
	if (GetEffects()->iResourceQuantity != 0)
	{
		CvAssertMsg(eTargetLuxury != NO_RESOURCE, "Adding NO_RESOURCE for a player. Please send Anton your save file and version.");
		pPlayer->changeNumResourceTotal(eTargetLuxury, GetEffects()->iResourceQuantity);
	}
	if (GetEffects()->bEmbargoCityStates)
	{	
		// Refresh trade routes
		GC.getGame().GetGameTrade()->ClearAllCityStateTradeRoutes();

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv())
			{
				GET_PLAYER(eLoopPlayer).GetCorporations()->ClearCorporationFromForeignCities(true);
			}
		}
	}
	if (GetEffects()->bEmbargoPlayer)
	{
		CvAssertMsg(eTargetPlayer != NO_PLAYER, "Making an embargo on NO_PLAYER. Please send Anton your save file and version.");
		// Refresh trade routes
		GC.getGame().GetGameTrade()->ClearAllCivTradeRoutes(eTargetPlayer, true);
		GET_PLAYER(eTargetPlayer).GetCorporations()->ClearCorporationFromForeignCities(false, true, true);

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

			if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv())
			{
				if (GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsVassal(GET_PLAYER(eTargetPlayer).getTeam()))
					continue;

				GC.getGame().GetGameDeals().DoCancelDealsBetweenPlayers(eLoopPlayer, eTargetPlayer);
			}
		}

		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			GET_PLAYER(eTargetPlayer).GetCorporations()->ClearCorporationFromCity(pLoopCity, GET_PLAYER(eTargetPlayer).GetCorporations()->GetFoundedCorporation(), true);
		}
	}
	if (GetEffects()->bNoResourceHappiness)
	{
		CvAssertMsg(eTargetLuxury != NO_RESOURCE, "Banning Happiness for NO_RESOURCE. Please send Anton your save file and version.");
		// Refresh happiness
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		{
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(pPlayer->GetID(), eResourceLoop))
				{
					pPlayer->CheckForMonopoly(eResourceLoop);
					CvCity* pLoopCity;
					int iLoop;
					for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
					{
						if (pLoopCity->GetResourceDemanded() == eResourceLoop)
							pLoopCity->DoPickResourceDemanded();
					}
				}
			}
		}
#endif
	}
	if (GetEffects()->iUnitMaintenanceGoldPercent != 0)
	{
		pPlayer->ChangeUnitGoldMaintenanceMod(GetEffects()->iUnitMaintenanceGoldPercent);
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && GetEffects()->iVassalMaintenanceGoldPercent != 0)
	{
		pPlayer->ChangeVassalGoldMaintenanceMod(GetEffects()->iVassalMaintenanceGoldPercent);
	}
#endif
	if (GetEffects()->iMemberDiscoveredTechMod != 0)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			pPlayer->SetLeagueScholar(true);
		}
#endif
		// Refresh research
	}
	if (GetEffects()->iCulturePerWonder != 0)
	{
		// Refresh yields
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			//Modifying Science of Great Works
			GET_PLAYER(ePlayer).ChangeGreatWorkYieldChange(YIELD_SCIENCE, GC.getSCIENCE_LEAGUE_GREAT_WORK_MODIFIER());
			pPlayer->SetLeagueArt(true);
		}
#endif
	}
	if (GetEffects()->iCulturePerNaturalWonder != 0)
	{
		// Refresh yields
	}
	if (GetEffects()->bNoTrainingNuclearWeapons)
	{
	}
	if (GetEffects()->iVotesForFollowingReligion != 0)
	{
		CvAssertMsg(eTargetReligion != NO_RELIGION, "No target religion when one was expected. Please send Anton your save file and version.");
	}
	if (GetEffects()->iHolyCityTourism != 0)
	{
		CvAssertMsg(eTargetReligion != NO_RELIGION, "No target religion when one was expected. Please send Anton your save file and version.");
	}
	if (GetEffects()->iReligionSpreadStrengthMod != 0)
	{
		CvAssertMsg(eTargetReligion != NO_RELIGION, "No target religion when one was expected. Please send Anton your save file and version.");
		// Updated on CvGameReligions::DoTurn
	}
	if (GetEffects()->iVotesForFollowingIdeology != 0)
	{
		CvAssertMsg(eTargetIdeology != NO_POLICY_BRANCH_TYPE, "No target ideology when one was expected. Please send Anton your save file and version.");
	}
	if (GetEffects()->iOtherIdeologyRebellionMod != 0)
	{
		CvAssertMsg(eTargetIdeology != NO_POLICY_BRANCH_TYPE, "No target ideology when one was expected. Please send Anton your save file and version.");
		/*
		if (eTargetIdeology != NO_POLICY_BRANCH_TYPE)
		{
			if (GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree() == eTargetIdeology)
			{
				//antonjs: temp:
				GET_PLAYER(ePlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(GetEffects()->iOtherIdeologyRebellionMod);
			}
		}
		//antonjs: todo: OnIdeologyChanged()
		*/
	}
	if (GetEffects()->iArtsyGreatPersonRateMod != 0)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			pPlayer->SetLeagueAid(true);
		}
#endif
	}
	if (GetEffects()->iScienceyGreatPersonRateMod != 0)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			pPlayer->SetLeagueAid(true);
		}
#endif
	}
	if (GetEffects()->iGreatPersonTileImprovementCulture != 0)
	{
		// Loop through all Great Person tile improvements
		for (int i = 0; i < GC.getNumImprovementInfos(); i++)
		{
			CvImprovementEntry* pInfo = GC.getImprovementInfo((ImprovementTypes)i);
			if (pInfo != NULL && pInfo->IsCreatedByGreatPerson())
			{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
				if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_FOOD, GetEffects()->iGreatPersonTileImprovementCulture);
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_GOLD, GetEffects()->iGreatPersonTileImprovementCulture);
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_PRODUCTION, GetEffects()->iGreatPersonTileImprovementCulture);
				}
				else
#endif
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_CULTURE, GetEffects()->iGreatPersonTileImprovementCulture);
			}
		}
		// Refresh yield
	}
	if (GetEffects()->iLandmarkCulture != 0)
	{
		CvImprovementEntry* pLandmarkInfo = GC.getImprovementInfo((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK"));
		if (pLandmarkInfo != NULL)
		{
			GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pLandmarkInfo->GetID(), YIELD_CULTURE, GetEffects()->iLandmarkCulture);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
				GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pLandmarkInfo->GetID(), YIELD_SCIENCE, GetEffects()->iLandmarkCulture);
				GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pLandmarkInfo->GetID(), YIELD_FAITH, GetEffects()->iLandmarkCulture);
			}
#endif
		}
		// Refresh yield
	}
#if defined(MOD_BALANCE_CORE)
	if (GetEffects()->iChangeTourism != 0)
	{
	}
#endif
#if defined (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
		if (GetEffects()->iLimitSpaceshipProduction != 0)
		{
		}
		if (GetEffects()->iLimitSpaceshipPurchase != 0)
		{
		}
		if (GetEffects()->iIsWorldWar != 0)
		{
		}
		if (GetEffects()->bEmbargoIdeology)
		{
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					PolicyBranchTypes eOurIdeology = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
					PolicyBranchTypes eTheirIdeology = GET_PLAYER(eLoopPlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
					if ((eOurIdeology != eTheirIdeology) && (eOurIdeology != NO_POLICY_BRANCH_TYPE) && (eTheirIdeology != NO_POLICY_BRANCH_TYPE))
					{
						GC.getGame().GetGameTrade()->ClearTradePlayerToPlayer(ePlayer, eLoopPlayer);
					}
				}
			}
			GC.getGame().GetGameTrade()->ClearAllCityStateTradeRoutesSpecial();
		}
	}
	GET_PLAYER(ePlayer).ProcessLeagueResolutions();
#endif

	m_iTurnEnacted = GC.getGame().getGameTurn();
}

void CvActiveResolution::RemoveEffects(PlayerTypes ePlayer)
{
	CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
	CvAssertMsg(pPlayer != NULL, "Player is null when doing effects of an active resolution. Please send Anton your save file and version.");
	if (pPlayer == NULL) return;

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(GetLeague());
	CvAssertMsg(pLeague != NULL, "League is null when doing effects of an active resolution. Please send Anton your save file and version.");
	if (pLeague == NULL) return;

	// == Proposer Choices ==
	ResolutionDecisionTypes eProposerDecision = GetProposerDecision()->GetType();
	PlayerTypes eTargetPlayer = NO_PLAYER;
	if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
		eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
		eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
	{
		eTargetPlayer = (PlayerTypes) GetProposerDecision()->GetDecision();
	}
	ResourceTypes eTargetLuxury = NO_RESOURCE;
	if (eProposerDecision == RESOLUTION_DECISION_ANY_LUXURY_RESOURCE)
	{
		CvResourceInfo* pInfo = GC.getResourceInfo((ResourceTypes) GetProposerDecision()->GetDecision());
		if (pInfo && pInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
		{
			eTargetLuxury = (ResourceTypes) GetProposerDecision()->GetDecision();
		}
	}
	ReligionTypes eTargetReligion = NO_RELIGION;
	if (eProposerDecision == RESOLUTION_DECISION_RELIGION)
	{
		eTargetReligion = (ReligionTypes) GetProposerDecision()->GetDecision();
	}
	PolicyBranchTypes eTargetIdeology = NO_POLICY_BRANCH_TYPE;
	if (eProposerDecision == RESOLUTION_DECISION_IDEOLOGY)
	{
		eTargetIdeology = (PolicyBranchTypes) GetProposerDecision()->GetDecision();
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	PlayerTypes eTargetCityState = NO_PLAYER;
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eProposerDecision == RESOLUTION_DECISION_CITY_CSD)
	{
		eTargetCityState = (PlayerTypes) GetProposerDecision()->GetDecision();
	}
#endif
	// == Voter Choices ==
	ResolutionDecisionTypes eVoterDecision = GetVoterDecision()->GetType();
	PlayerTypes eVotedPlayer = NO_PLAYER;
	if (eVoterDecision == RESOLUTION_DECISION_ANY_MEMBER ||
		eVoterDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		eVoterDecision == RESOLUTION_DECISION_CITY_CSD ||
#endif
		eVoterDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
	{
		eVotedPlayer = (PlayerTypes) GetVoterDecision()->GetDecision();
	}

	// == One Time Effects are not removed ==

	// == Ongoing Effects ==
	if (GetEffects()->iGoldPerTurn != 0)
	{
		pPlayer->GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-1 * GetEffects()->iGoldPerTurn);
	}
	if (GetEffects()->iResourceQuantity != 0)
	{
		CvAssertMsg(eTargetLuxury != NO_RESOURCE, "Subtracting NO_RESOURCE for a player. Please send Anton your save file and version.");
		pPlayer->changeNumResourceTotal(eTargetLuxury, -1 * GetEffects()->iResourceQuantity);
	}
	if (GetEffects()->bEmbargoCityStates)
	{
		// Refresh trade routes
	}
	if (GetEffects()->bEmbargoPlayer)
	{
		CvAssertMsg(eTargetPlayer != NO_PLAYER, "Repealing an embargo on NO_PLAYER. Please send Anton your save file and version.");
		// Refresh trade routes
	}
	if (GetEffects()->bNoResourceHappiness)
	{
		CvAssertMsg(eTargetLuxury != NO_RESOURCE, "Repealing a band on Happiness for NO_RESOURCE. Please send Anton your save file and version.");
		// Refresh happiness
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		{
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				if(!GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(pPlayer->GetID(), eResourceLoop))
				{
					pPlayer->CheckForMonopoly(eResourceLoop);
				}
			}
		}
#endif
	}
	if (GetEffects()->iUnitMaintenanceGoldPercent != 0)
	{
		pPlayer->ChangeUnitGoldMaintenanceMod(-1 * GetEffects()->iUnitMaintenanceGoldPercent);
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && GetEffects()->iVassalMaintenanceGoldPercent != 0)
	{
		pPlayer->ChangeVassalGoldMaintenanceMod(-1 * GetEffects()->iVassalMaintenanceGoldPercent);
	}
#endif
	if (GetEffects()->iMemberDiscoveredTechMod != 0)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			pPlayer->SetLeagueScholar(false);
		}
#endif
		// Refresh research
	}
	if (GetEffects()->iCulturePerWonder != 0)
	{
		// Refresh yields
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			//Modifying Science of Great Works
			GET_PLAYER(ePlayer).ChangeGreatWorkYieldChange(YIELD_SCIENCE, -1 * GC.getSCIENCE_LEAGUE_GREAT_WORK_MODIFIER());
			pPlayer->SetLeagueArt(false);
		}
#endif
	}
	if (GetEffects()->iCulturePerNaturalWonder != 0)
	{
		// Refresh yields
	}
	if (GetEffects()->bNoTrainingNuclearWeapons)
	{
	}
	if (GetEffects()->iVotesForFollowingReligion != 0)
	{
	}
	if (GetEffects()->iHolyCityTourism != 0)
	{
		CvAssertMsg(eTargetReligion != NO_RELIGION, "No target religion when one was expected. Please send Anton your save file and version.");
	}
	if (GetEffects()->iReligionSpreadStrengthMod != 0)
	{
		CvAssertMsg(eTargetReligion != NO_RELIGION, "No target religion when one was expected. Please send Anton your save file and version.");
		// Updated on CvGameReligions::DoTurn
	}
	if (GetEffects()->iVotesForFollowingIdeology != 0)
	{
	}
	if (GetEffects()->iOtherIdeologyRebellionMod != 0)
	{
		CvAssertMsg(eTargetIdeology != NO_POLICY_BRANCH_TYPE, "No target ideology when one was expected. Please send Anton your save file and version.");
		/*
		if (eTargetIdeology != NO_POLICY_BRANCH_TYPE)
		{
			if (GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree() == eTargetIdeology)
			{
				//antonjs: temp:
				GET_PLAYER(ePlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-1 * 100);
			}
		}
		//antonjs: todo: OnIdeologyChanged()
		*/
	}
	if (GetEffects()->iArtsyGreatPersonRateMod != 0)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			pPlayer->SetLeagueAid(false);
		}
#endif
	}
	if (GetEffects()->iScienceyGreatPersonRateMod != 0)
	{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			pPlayer->SetLeagueAid(false);
		}
#endif
	}
	if (GetEffects()->iGreatPersonTileImprovementCulture != 0)
	{
		// Loop through all Great Person tile improvements
		for (int i = 0; i < GC.getNumImprovementInfos(); i++)
		{
			CvImprovementEntry* pInfo = GC.getImprovementInfo((ImprovementTypes)i);
			if (pInfo != NULL && pInfo->IsCreatedByGreatPerson())
			{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
				if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_FOOD, -1 * GetEffects()->iGreatPersonTileImprovementCulture);
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_GOLD, -1 * GetEffects()->iGreatPersonTileImprovementCulture);
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_PRODUCTION, -1 * GetEffects()->iGreatPersonTileImprovementCulture);
				}
				else
#endif
					GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pInfo->GetID(), YIELD_CULTURE, -1 * GetEffects()->iGreatPersonTileImprovementCulture);
			}
		}
		// Refresh yield
	}
	if (GetEffects()->iLandmarkCulture != 0)
	{
		CvImprovementEntry* pLandmarkInfo = GC.getImprovementInfo((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK"));
		if (pLandmarkInfo != NULL)
		{
			GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pLandmarkInfo->GetID(), YIELD_CULTURE, -1 * GetEffects()->iLandmarkCulture);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
				GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pLandmarkInfo->GetID(), YIELD_SCIENCE, -1 * GetEffects()->iLandmarkCulture);
				GET_PLAYER(ePlayer).changeImprovementYieldChange((ImprovementTypes)pLandmarkInfo->GetID(), YIELD_FAITH, -1 * GetEffects()->iLandmarkCulture);
			}
#endif
		}
		// Refresh yield
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
	{
		if (GetEffects()->bOpenDoor)
		{
			if (ePlayer == eTargetCityState)
			{
				PlayerTypes eAlly = NO_PLAYER;
				GET_PLAYER(ePlayer).GetMinorCivAI()->SetNoAlly(false);
				GET_PLAYER(ePlayer).GetMinorCivAI()->GetMostFriendshipWithAnyMajor(/*Passed by address*/ eAlly);
#if defined(MOD_CITY_STATE_SCALE)
				if (eAlly != NO_PLAYER && GET_PLAYER(ePlayer).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlly) > GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlliesThreshold(eAlly))
#else
				if(eAlly != NO_PLAYER && GET_PLAYER(ePlayer).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlly) > GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlliesThreshold())
#endif
				{
					GET_PLAYER(ePlayer).GetMinorCivAI()->SetAlly(eAlly);
				}
			}	
		}
		if (GetEffects()->bSphereOfInfluence)
		{
			if (ePlayer == eTargetCityState)
			{
				GET_PLAYER(ePlayer).GetMinorCivAI()->SetPermanentAlly(NO_PLAYER);
				PlayerTypes eAlly = NO_PLAYER;
				GET_PLAYER(ePlayer).GetMinorCivAI()->GetMostFriendshipWithAnyMajor(/*Passed by address*/ eAlly);
#if defined(MOD_CITY_STATE_SCALE)
				if(eAlly != NO_PLAYER && GET_PLAYER(ePlayer).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlly) > GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlliesThreshold(eAlly))
#else
				if(eAlly != NO_PLAYER && GET_PLAYER(ePlayer).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlly) > GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlliesThreshold())
#endif
				{
					GET_PLAYER(ePlayer).GetMinorCivAI()->SetAlly(eAlly);
				}
			}
		}	
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if (GetEffects()->iChangeTourism != 0)
	{
	}
#endif	
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
		if (GetEffects()->iLimitSpaceshipProduction != 0)
		{
		}
		if (GetEffects()->iLimitSpaceshipPurchase != 0)
		{
		}
		if (GetEffects()->iIsWorldWar != 0)
		{
		}
		if (GetEffects()->bEmbargoIdeology)
		{
		}
	}
	GET_PLAYER(ePlayer).ProcessLeagueResolutions();
#endif

	m_iTurnEnacted = -1;
}

bool CvActiveResolution::HasOngoingEffects()
{
	return GetEffects()->HasOngoingEffects();
}

int CvActiveResolution::GetTurnEnacted() const
{
	return m_iTurnEnacted;
}

template<typename ActiveResolution, typename Visitor>
void CvActiveResolution::Serialize(ActiveResolution& activeResolution, Visitor& visitor)
{
	activeResolution.CvResolution::serialize(visitor);
	visitor(activeResolution.m_iTurnEnacted);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvActiveResolution& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvActiveResolution::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvActiveResolution& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvActiveResolution::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvRepealProposal
// ================================================================================
CvRepealProposal::CvRepealProposal(void)
{
	m_iTargetResolutionID = -1;
}

CvRepealProposal::CvRepealProposal(CvActiveResolution* pResolution, PlayerTypes eProposalPlayer) : CvProposal(pResolution->GetID(), pResolution->GetType(), pResolution->GetLeague(), eProposalPlayer)
{
	m_iTargetResolutionID = pResolution->GetID();
	m_VoterDecision = (*(pResolution->GetVoterDecision()));
	m_ProposerDecision = (*(pResolution->GetProposerDecision()));
	m_RepealDecision = CvVoterDecision(RESOLUTION_DECISION_REPEAL);
}

CvRepealProposal::~CvRepealProposal(void)
{
}

void CvRepealProposal::Init()
{
}

bool CvRepealProposal::IsPassed(int iTotalSessionVotes)
{
	CvResolutionEntry* pInfo = GC.getResolutionInfo(GetType());
	CvAssert(pInfo);
	if (!pInfo)
	{
		return false;
	}

	int iQuorumPercent = pInfo->GetQuorumPercent();
	bool bQuorum = ((float)GetRepealDecision()->GetVotesCast() / (float)iTotalSessionVotes) >= ((float)iQuorumPercent / 100.0f);
	if (!bQuorum)
	{
		return false;
	}

	int iDecision = GetRepealDecision()->GetDecision();
	if (iDecision == LeagueHelpers::CHOICE_NONE)
	{
		// No votes, tied votes, etc.
		return false;
	}

	CvAssertMsg(GetRepealDecision()->GetType() == RESOLUTION_DECISION_REPEAL, "Unexpected decision type for repeal proposal. Please send Anton your save file and version.");
	if (GetRepealDecision()->GetType() == RESOLUTION_DECISION_REPEAL)
	{
		return (iDecision == LeagueHelpers::CHOICE_YES);
	}
	else
	{
		return false;
	}
}

CvString CvRepealProposal::GetProposalName(bool bForLogging)
{
	CvString s = "";

	if (bForLogging)
	{
		s += "REPEAL: ";
	}
	else
	{
		s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_REPEAL").toUTF8();
	}
	s += CvResolution::GetName();

	return s;
}

int CvRepealProposal::GetTargetResolutionID() const
{
	return m_iTargetResolutionID;
}

CvVoterDecision* CvRepealProposal::GetRepealDecision()
{
	return &m_RepealDecision;
}

template<typename RepealProposal, typename Visitor>
void CvRepealProposal::Serialize(RepealProposal& repealProposal, Visitor& visitor)
{
	repealProposal.CvProposal::serialize(visitor);
	visitor(repealProposal.m_iTargetResolutionID);
	visitor(repealProposal.m_RepealDecision);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvRepealProposal& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvRepealProposal::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvRepealProposal& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvRepealProposal::Serialize(readFrom, serialVisitor);
	return saveTo;
}


// ================================================================================
//			CvLeague
// ================================================================================
CvLeague::CvLeague(void)
{
	m_eID = NO_LEAGUE;
	m_bUnitedNations = false;
	m_bInSession = false;
	m_iTurnsUntilSession = MAX_INT;
	m_iNumResolutionsEverEnacted = 0;
	m_vLastTurnEnactProposals.clear();
	m_vLastTurnRepealProposals.clear();
	m_vEnactProposals.clear();
	m_vRepealProposals.clear();
	m_vActiveResolutions.clear();
	m_vMembers.clear();
	m_eHost = NO_PLAYER;
	m_vProjects.clear();
	m_iConsecutiveHostedSessions = 0;
	m_eAssignedName = NO_LEAGUE_NAME;
	ZeroMemory(m_szCustomName, sizeof(m_szCustomName));
	m_eLastSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
	m_eCurrentSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
	m_vEnactProposalsOnHold.clear();
	m_vRepealProposalsOnHold.clear();
}

CvLeague::CvLeague(LeagueTypes eID)
{
	m_eID = eID;
	m_bUnitedNations = false;
	m_bInSession = false;
	m_iTurnsUntilSession = MAX_INT;
	m_iNumResolutionsEverEnacted = 0;
	m_vEnactProposals.clear();
	m_vRepealProposals.clear();
	m_vActiveResolutions.clear();
	m_vMembers.clear();
	m_eHost = NO_PLAYER;
	m_vProjects.clear();
	m_iConsecutiveHostedSessions = 0;
	m_eAssignedName = NO_LEAGUE_NAME;
	ZeroMemory(m_szCustomName, sizeof(m_szCustomName));
	m_eLastSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
	m_eCurrentSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
	m_vEnactProposalsOnHold.clear();
	m_vRepealProposalsOnHold.clear();
	m_vLastTurnEnactProposals.clear();
	m_vLastTurnRepealProposals.clear();
}

CvLeague::~CvLeague(void)
{
}

CvLeague::Member::Member(void)
{
	ePlayer = NO_PLAYER;
	iExtraVotes = 0;
	sVoteSources = "";
	bMayPropose = false;
	iProposals = 0;
	iVotes = 0;
	iAbstainedVotes = 0;
	bEverBeenHost = false;
	bAlwaysBeenHost = true;
	m_startingVotesCacheTime = 0;
	m_startingVotesCached = 0;
}

CvLeague::Member::~Member(void)
{
}

CvLeague::Project::Project(void)
{
	eType = NO_LEAGUE_PROJECT;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		vProductionList.push_back(0);
	}
	bComplete = false;
	bProgressWarningSent = false;
}

CvLeague::Project::~Project(void)
{
}

void CvLeague::Init(LeagueSpecialSessionTypes eGoverningSpecialSession)
{
	CvAssertMsg(m_eID != NO_LEAGUE, "Initializing a CvLeague without a proper ID. Please send Anton your save file and verison.");
	CvAssert(eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION);
	m_eLastSpecialSession = eGoverningSpecialSession; // Fake the last special session so we have data to inform the World Congress's status
	AssignProposalPrivileges();
	ResetTurnsUntilSession();
}

void CvLeague::DoTurn(LeagueSpecialSessionTypes eTriggeredSpecialSession)
{
	CvAssert(eTriggeredSpecialSession == NO_LEAGUE_SPECIAL_SESSION || CanStartSpecialSession(eTriggeredSpecialSession));

	// Special Session this turn, put everything else on hold
	if (eTriggeredSpecialSession != NO_LEAGUE_SPECIAL_SESSION && CanStartSpecialSession(eTriggeredSpecialSession))
	{
		CheckStartSpecialSession(eTriggeredSpecialSession);
	}
	else if (!IsInSession())
	{
		ChangeTurnsUntilSession(-1);
		CvAssertMsg(GetTurnsUntilSession() >= 0, "Turns left until League session is negative. Please send Anton your save file and version.");
		if (GetTurnsUntilSession() <= 0)
		{
			CheckStartSession();
		}
		else if (GetTurnsUntilSession() == GC.getLEAGUE_SESSION_SOON_WARNING_TURNS())
		{
			NotifySessionSoon(GetTurnsUntilSession());
		}
	}
	else
	{
		CheckFinishSession();
	}

	CheckProjectsProgress();
}

LeagueTypes CvLeague::GetID() const
{
	return m_eID;
}

Localization::String CvLeague::GetName()
{
	Localization::String sName = Localization::Lookup("TXT_KEY_LEAGUE_WORLD_CONGRESS_GENERIC");
	if (m_szCustomName == NULL || strlen(m_szCustomName) == 0)
	{
		if (IsUnitedNations())
		{
			sName = Localization::Lookup("TXT_KEY_LEAGUE_UNITED_NATIONS");
		}
		else if (HasHostMember() && m_eAssignedName != NO_LEAGUE_NAME)
		{
			PlayerTypes eHost = GetHostMember();
			CvCity* pCapital = GET_PLAYER(eHost).getCapitalCity();
			CvLeagueNameEntry* pInfo = GC.getLeagueNameInfo(m_eAssignedName);
			if (pCapital && pInfo)
			{
				CvString sOrdinalKey = "";
				sOrdinalKey.Format("TXT_KEY_LEAGUE_ORDINAL_%d", GetConsecutiveHostedSessions() + 1);
				Localization::String sOrdinal = Localization::Lookup(sOrdinalKey);

				sName = Localization::Lookup(pInfo->GetNameKey());
				//antonjs: temp: Did our lookup return the same thing (ie. we don't have that text key)?
				if (sOrdinalKey.compare(sOrdinal.toUTF8()) == 0)
				{
					sName << "" << pCapital->getNameKey();
				}
				else
				{
					sName << sOrdinal << pCapital->getNameKey();
				}
			}
		}
	}
	// Custom name
	else
	{
		return m_szCustomName;
	}

	return sName;
}

bool CvLeague::CanChangeCustomName(PlayerTypes ePlayer)
{
	if (!IsInSession())
	{
		if (IsHostMember(ePlayer))
		{
			return true;
		}
	}
	
	return false;
}

void CvLeague::DoChangeCustomName(PlayerTypes ePlayer, const char* szCustomName)
{
	CvAssertMsg(CanChangeCustomName(ePlayer), "Player is not allowed to edit league name.");
	if (CanChangeCustomName(ePlayer))
	{
		CvAssertMsg(strlen(szCustomName) <= sizeof(m_szCustomName), "Custom name for league is too long.");
		if(szCustomName != NULL && strlen(szCustomName) <= sizeof(m_szCustomName))
		{
			strcpy_s(m_szCustomName, szCustomName);
		}
	}
}

int CvLeague::GetSessionTurnInterval()
{
	// Determined by most recent special session
	int iInterval = GC.getLEAGUE_SESSION_INTERVAL_BASE_TURNS();
	if (GetCurrentSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
	{
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(GetCurrentSpecialSession());
		CvAssert(pInfo != NULL);
		if (pInfo != NULL)
		{
			iInterval = pInfo->GetTurnsBetweenSessions();
		}
	}
	else if (GetLastSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
	{
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(GetLastSpecialSession());
		CvAssert(pInfo != NULL);
		if (pInfo != NULL)
		{
			iInterval = pInfo->GetTurnsBetweenSessions();
		}
	}

	// Modified by game speed
	iInterval = (iInterval * GC.getGame().getGameSpeedInfo().getLeaguePercent()) / 100;

	if (DEBUG_LEAGUES)
	{
		return 2;
	}

	return iInterval;
}

bool CvLeague::IsInSession()
{
	return m_bInSession;
}

bool CvLeague::IsInSpecialSession()
{
	return (IsInSession() && GetCurrentSpecialSession() != NO_LEAGUE_SPECIAL_SESSION);
}

void CvLeague::SetInSession(bool bInSession)
{
	m_bInSession = bInSession;
	if (bInSession)
	{
	}
	else
	{
		if (m_eCurrentSpecialSession != NO_LEAGUE_SPECIAL_SESSION)
		{
			m_eLastSpecialSession = m_eCurrentSpecialSession;
		}
		m_eCurrentSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
	}
}

void CvLeague::SetInSession(LeagueSpecialSessionTypes eSpecialSession)
{
	CvAssertMsg(eSpecialSession != NO_LEAGUE_SPECIAL_SESSION, "Attempting to set the league in session with an invalid special session type. Please send Anton your save file and version.");
	if (eSpecialSession != NO_LEAGUE_SPECIAL_SESSION)
	{
		SetInSession(true);
		m_eCurrentSpecialSession = eSpecialSession;
	}
}

int CvLeague::GetTurnsUntilSession() const
{
	return m_iTurnsUntilSession;
}

void CvLeague::SetTurnsUntilSession(int iTurns)
{
	CvAssertMsg(iTurns >= 0, "Cannot have negative turns until next League session. Please send Anton your save file and version.");

	if (iTurns >= 0)
	{
		m_iTurnsUntilSession = iTurns;
	}
}

void CvLeague::ChangeTurnsUntilSession(int iChange)
{
	SetTurnsUntilSession(GetTurnsUntilSession() + iChange);
}

void CvLeague::ResetTurnsUntilSession()
{
	SetTurnsUntilSession(GetSessionTurnInterval());
}

int CvLeague::GetTurnsUntilVictorySession()
{
	VictoryTypes eDiploVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
	if (eDiploVictory == NO_VICTORY)
	{
		return 999;
	}
	if (!GC.getGame().isVictoryValid(eDiploVictory))
	{
		return 999;
	}

	int iValue = 999;

	bool bVictoryProposed = false;
	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
	{
		if (it->GetEffects()->bDiplomaticVictory)
		{
			bVictoryProposed = true;
		}
	}

	// Is victory already proposed?
	if (bVictoryProposed)
	{
		iValue = GetTurnsUntilSession();
	}
	// Is there a recurring victory proposal, for the following session?
	else
	{
		LeagueSpecialSessionTypes eGoverningSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
		if (GetCurrentSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
		{
			eGoverningSpecialSession = GetCurrentSpecialSession();
		}
		else if (GetLastSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
		{
			eGoverningSpecialSession = GetLastSpecialSession();
		}
		CvAssert(eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION);

		bool bRecurringVictoryProposal = false;
		if (eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION)
		{
			CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);
			CvAssert(pInfo != NULL);
			if (pInfo != NULL)
			{
				ResolutionTypes e = pInfo->GetRecurringProposal();
				if (e != NO_RESOLUTION)
				{
					CvResolutionEntry* p = GC.getResolutionInfo(e);
					if (p != NULL && p->IsDiplomaticVictory())
					{
						bRecurringVictoryProposal = true;
					}
				}
			}
		}

		if (bRecurringVictoryProposal)
		{
			// Time until next session, plus the interval until the following session
			iValue = GetTurnsUntilSession() + GetSessionTurnInterval();
		}
	}

	return iValue;
}

int CvLeague::GetVotesSpentThisSession()
{
	int iVotes = 0;
	if (IsInSession())
	{
		for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
		{
			iVotes += it->GetVoterDecision()->GetVotesCast();
		}
		for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
		{
			iVotes += it->GetRepealDecision()->GetVotesCast();
		}
	}
	return iVotes;
}

LeagueSpecialSessionTypes CvLeague::GetLastSpecialSession() const
{
	return m_eLastSpecialSession;
}

LeagueSpecialSessionTypes CvLeague::GetCurrentSpecialSession() const
{
	return m_eCurrentSpecialSession;
}

bool CvLeague::CanStartSpecialSession(LeagueSpecialSessionTypes /*eSpecialSession*/)
{
	// Cannot already be in session
	if (IsInSession())
		return false;

	// Cannot have proposals on hold
	if (HasProposalsOnHold())
		return false;

	// Cannot be awaiting proposals or votes from anyone (UI complications with overview screen)
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (CanPropose(it->ePlayer) || CanVote(it->ePlayer))
		{
			return false;
		}
	}
	
	return true;
}

bool CvLeague::IsUnitedNations() const
{
	return m_bUnitedNations;
}

void CvLeague::SetUnitedNations(bool bValue)
{
	if (m_bUnitedNations != bValue)
	{
		m_bUnitedNations = bValue;
	}
}

void CvLeague::DoVoteEnact(int iID, PlayerTypes eVoter, int iNumVotes, int iChoice)
{
	bool bProcessed = false;
	if (CanVote(eVoter) && GetRemainingVotesForMember(eVoter) >= iNumVotes)
	{
		for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); it++)
		{
			if (it->GetID() == iID)
			{
				it->GetVoterDecision()->ProcessVote(eVoter, iNumVotes, iChoice);
				GetMember(eVoter)->iVotes -= iNumVotes;
				CvAssertMsg(GetRemainingVotesForMember(eVoter) >= 0, "A voter now has negative votes remaining. Please send Anton your save file and version.");
				bProcessed = true;
				break;
			}
		}
	}
	CvAssertMsg(bProcessed, "Attempt to vote on an enact resolution when not allowed to. Please send Anton your save file and version.");
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

void CvLeague::DoVoteRepeal(int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice)
{
	bool bProcessed = false;
	if (CanVote(eVoter) && GetRemainingVotesForMember(eVoter) >= iNumVotes)
	{
		if (IsRepealProposed(iResolutionID))
		{
			for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); it++)
			{
				if (it->GetTargetResolutionID() == iResolutionID)
				{
					it->GetRepealDecision()->ProcessVote(eVoter, iNumVotes, iChoice);
					GetMember(eVoter)->iVotes -= iNumVotes;
					CvAssertMsg(GetRemainingVotesForMember(eVoter) >= 0, "A voter now has negative votes remaining. Please send Anton your save file and version.");
					bProcessed = true;
					break;
				}
			}
		}
	}
	CvAssertMsg(bProcessed, "Attempt to vote on a repeal resolution when not allowed to. Please send Anton your save file and version.");
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

void CvLeague::DoVoteAbstain(PlayerTypes eVoter, int iNumVotes)
{
	bool bProcessed = false;
	if (CanVote(eVoter) && GetRemainingVotesForMember(eVoter) >= iNumVotes)
	{
		GetMember(eVoter)->iVotes -= iNumVotes;
		GetMember(eVoter)->iAbstainedVotes += iNumVotes;
		CvAssertMsg(GetRemainingVotesForMember(eVoter) >= 0, "A voter now has negative votes remaining. Please send Anton your save file and version.");
		bProcessed = true;
	}
	CvAssertMsg(bProcessed, "Attempt to abstain votes but not allowed to. Please send Anton your save file and version.");
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

void CvLeague::DoProposeEnact(ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice)
{
	if (!CanProposeEnact(eResolution, eProposer, iChoice))
	{
		CvAssertMsg(false, "Attempt to propose enact resolution when not allowed to. Please send Anton your save file and version.");
		return;
	}

	if (eProposer != NO_PLAYER)
	{
		// Decrement remaining proposals
		if (IsMember(eProposer))
		{
			GetMember(eProposer)->iProposals = GetMember(eProposer)->iProposals - 1;
			CvAssertMsg(GetMember(eProposer)->iProposals >= 0, "Attempt to propose enact resolution when not allowed to. Please send Anton your save file and version.");
		}
		else
		{
			CvAssertMsg(false, "Attempt to propose enact resolution when not allowed to. Please send Anton your save file and version.");
		}

		// Proposals made by players could affect Diplomacy AI
		// Return values are discarded!
		LeagueHelpers::PlayerList vLikers = GetMembersThatLikeProposal(eResolution, eProposer, iChoice, true);
		LeagueHelpers::PlayerList vDislikers = GetMembersThatDislikeProposal(eResolution, eProposer, iChoice, true);
	}
	
	CvEnactProposal proposal(GC.getGame().GetGameLeagues()->GenerateResolutionUniqueID(), eResolution, GetID(), eProposer, iChoice);
	m_vEnactProposals.push_back(proposal);
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

void CvLeague::DoProposeRepeal(int iResolutionID, PlayerTypes eProposer)
{
	if (!CanProposeRepeal(iResolutionID, eProposer))
	{
		CvAssertMsg(false, "Attempt to propose repeal resolution when not allowed to. Please send Anton your save file and version.");
		return;
	}
	
	int iFound = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetID() == iResolutionID)
		{
			if (eProposer != NO_PLAYER)
			{
				// Decrement remaining proposals
				if (IsMember(eProposer))
				{
					GetMember(eProposer)->iProposals = GetMember(eProposer)->iProposals - 1;
					CvAssertMsg(GetMember(eProposer)->iProposals >= 0, "Attempt to propose enact resolution when not allowed to. Please send Anton your save file and version.");
				}
				else
				{
					CvAssertMsg(false, "Attempt to propose enact resolution when not allowed to. Please send Anton your save file and version.");
				}

				// Proposals made by players could affect Diplomacy AI
				// Return values are discarded!
				LeagueHelpers::PlayerList vLikers = GetMembersThatLikeProposal(iResolutionID, eProposer, true);
				LeagueHelpers::PlayerList vDislikers = GetMembersThatDislikeProposal(iResolutionID, eProposer, true);
			}

			CvRepealProposal proposal(&(*it), eProposer);
			m_vRepealProposals.push_back(proposal);
			iFound++;

#if defined(MOD_API_ACHIEVEMENTS)
			// XP2 Achievement
			if (!GC.getGame().isGameMultiPlayer())
			{
				PlayerTypes eOriginalProposer = it->GetProposerDecision()->GetProposer();
				if (eOriginalProposer != NO_PLAYER && eOriginalProposer == eProposer)
				{
					if (GET_PLAYER(eProposer).isHuman() && GET_PLAYER(eProposer).isLocalPlayer())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_42);
					}
				}
			}
#endif
		}

	}
	CvAssertMsg(iFound == 1, "Unexpected number of active resolutions with this ID. Please send Anton your save file and version.");
	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

bool CvLeague::CanProposeEnactAnyChoice(ResolutionTypes eResolution, PlayerTypes eProposer)
{
	CvAssertMsg(eResolution >= 0, "Invalid resolution type. Please send Anton your save file and version.");
	CvAssertMsg(eResolution < GC.getNumResolutionInfos(), "Resolution index out of bounds. Please send Anton your save file and version.");

	CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
	CvAssertMsg(pInfo, "Resolution info is null. Please send Anton your save file and version.");
	if (!pInfo) return false;

	std::vector<int> vValidChoices = GetChoicesForDecision(pInfo->GetProposerDecision(), eProposer);
	if (vValidChoices.empty())
	{
		if (pInfo->GetProposerDecision() == RESOLUTION_DECISION_NONE)
		{
			if (CanProposeEnact(eResolution, eProposer, LeagueHelpers::CHOICE_NONE))
			{
				return true;
			}
		}
	}
	else
	{
		for (uint i = 0; i < vValidChoices.size(); i++)
		{
			if (CanProposeEnact(eResolution, eProposer, vValidChoices[i]))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvLeague::CanProposeEnact(ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice, CvString* sTooltipSink)
{
	CvAssertMsg(eResolution >= 0, "Invalid resolution type. Please send Anton your save file and version.");
	CvAssertMsg(eResolution < GC.getNumResolutionInfos(), "Resolution index out of bounds. Please send Anton your save file and version.");

	CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
	CvAssertMsg(pInfo, "Resolution info is null. Please send Anton your save file and version.");
	if (!pInfo) return false;

	bool bValid = true;
	
	// Is a player proposing this?
	if (eProposer != NO_PLAYER)
	{
		// Must be a proposal that can be made by players
		if (pInfo->IsNoProposalByPlayer())
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_NO_PLAYER_PROPOSAL").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			bValid = false;
		}

		// Must be a member
		if (!IsMember(eProposer))
			bValid = false;

		// Must have right to propose
		if (!CanPropose(eProposer))
			bValid = false;
	}

	// Is this a valid choice for this proposal?
	bool bValidChoice = false;
	std::vector<int> vValidChoices = GetChoicesForDecision(pInfo->GetProposerDecision(), eProposer);
	if (vValidChoices.empty())
	{
		if (pInfo->GetProposerDecision() == RESOLUTION_DECISION_NONE && LeagueHelpers::CHOICE_NONE == iChoice)
		{
			bValidChoice = true;
		}
	}
	else
	{
		for (uint i = 0; i < vValidChoices.size(); i++)
		{
			if (vValidChoices[i] == iChoice)
			{
				bValidChoice = true;
				break;
			}
		}
	}
	if (!bValidChoice)
	{
		bValid = false;
	}
	else
	{
		// Effects must be allowed in our game
		if (!IsResolutionEffectsValid(eResolution, iChoice, sTooltipSink))
		{
			bValid = false;
		}

		// Must not already be proposed
		if (IsEnactProposed(eResolution, iChoice))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_ALREADY_PROPOSED").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			bValid = false;
		}

		// Must not already be active
		if (IsActiveResolution(eResolution, iChoice))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_ALREADY_ENACTED").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			bValid = false;
		}

		//projects can always be attempted
		if (MOD_BALANCE_CORE && pInfo->GetLeagueProjectEnabled() == NO_LEAGUE_PROJECT)
		{
			for (EnactProposalList::iterator it = m_vLastTurnEnactProposals.begin(); it != m_vLastTurnEnactProposals.end(); it++)
			{
				if (it->GetType() == eResolution)
				{
					if (it->GetProposerDecision()->GetDecision() == iChoice)
					{
						if (sTooltipSink != NULL)
						{
							(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
							(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_ATTEMPTED_LAST_SESSION").toUTF8();
							(*sTooltipSink) += "[ENDCOLOR]";
						}
						bValid = false;
					}
				}

			}

			for (RepealProposalList::iterator it = m_vLastTurnRepealProposals.begin(); it != m_vLastTurnRepealProposals.end(); it++)
			{
				if (it->GetType() == eResolution)
				{
					if (it->GetProposerDecision()->GetDecision() == iChoice)
					{
						if (sTooltipSink != NULL)
						{
							(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
							(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_ATTEMPTED_LAST_SESSION").toUTF8();
							(*sTooltipSink) += "[ENDCOLOR]";
						}
						bValid = false;
					}
				}
			}
		}
	}
	
	// Prereq tech
	if (pInfo->GetTechPrereqAnyMember() != NO_TECH)
	{
		bool bMemberHasTech = false;
		for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
		{
			if (GET_TEAM(GET_PLAYER(it->ePlayer).getTeam()).GetTeamTechs()->HasTech(pInfo->GetTechPrereqAnyMember()))
			{
				bMemberHasTech = true;
				break;
			}
		}

		if (!bMemberHasTech)
		{
			if (sTooltipSink != NULL)
			{
				CvTechEntry* pTechInfo = GC.getTechInfo(pInfo->GetTechPrereqAnyMember());
				if (pTechInfo != NULL)
				{
					Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_NO_TECH");
					sTemp << pTechInfo->GetDescriptionKey();

					(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
					(*sTooltipSink) += sTemp.toUTF8();
					(*sTooltipSink) += "[ENDCOLOR]";
				}
			}
			bValid = false;
		}
	}

	// League Project
	if (pInfo->GetLeagueProjectEnabled() != NO_LEAGUE_PROJECT)
	{
		// Is the project already underway or completed??
		if (IsProjectActive(pInfo->GetLeagueProjectEnabled()) || IsProjectComplete(pInfo->GetLeagueProjectEnabled()))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_ALREADY_ENACTED").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			bValid = false;
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	{	
		//Resolutions Cancel Each Other Out?
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->GetWorldWar() > 0)
		{
			for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
			{
				if (it->GetEffects()->iUnitMaintenanceGoldPercent > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetEffects()->iUnitMaintenanceGoldPercent > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
		}
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->GetUnitMaintenanceGoldPercent() > 0)
		{
			for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
			{
				if (it->GetEffects()->iIsWorldWar > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetEffects()->iIsWorldWar > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
		}
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->GetArtsyGreatPersonRateMod() > 0)
		{
			for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
			{
				if (it->GetEffects()->iScienceyGreatPersonRateMod > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetEffects()->iScienceyGreatPersonRateMod > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
		}
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->GetScienceyGreatPersonRateMod() > 0)
		{
			for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
			{
				if (it->GetEffects()->iArtsyGreatPersonRateMod > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetEffects()->iArtsyGreatPersonRateMod > 0)
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
		}
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->IsOpenDoor())
		{
			for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
			{
				if (it->GetEffects()->bSphereOfInfluence && ((PlayerTypes)iChoice == (PlayerTypes)it->GetProposerDecision()->GetDecision()))
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetEffects()->bSphereOfInfluence && ((PlayerTypes)iChoice == (PlayerTypes)it->GetProposerDecision()->GetDecision()))
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
		}
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->IsSphereOfInfluence())
		{
			for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
			{
				if (it->GetEffects()->bOpenDoor && ((PlayerTypes)iChoice == (PlayerTypes)it->GetProposerDecision()->GetDecision()))
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetEffects()->bOpenDoor && ((PlayerTypes)iChoice == (PlayerTypes)it->GetProposerDecision()->GetDecision()))
				{
					if (sTooltipSink != NULL)
					{
						(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION").toUTF8();
						(*sTooltipSink) += "[ENDCOLOR]";
					}
				bValid = false;
				}
			}
		}
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if (pInfo->GetTourismMod() > 0)
	{
		for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
		{
			if (it->GetEffects()->iChangeTourism < 0)
			{
				if (sTooltipSink != NULL)
				{
					(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
					(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION_CBP").toUTF8();
					(*sTooltipSink) += "[ENDCOLOR]";
				}
				bValid = false;
			}
		}
		for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
		{
			if (it->GetEffects()->iChangeTourism < 0)
			{
				if (sTooltipSink != NULL)
				{
					(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
					(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION_CBP").toUTF8();
					(*sTooltipSink) += "[ENDCOLOR]";
				}
				bValid = false;
			}
		}
	}
	if (pInfo->GetTourismMod() < 0)
	{
		for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
		{
			if (it->GetEffects()->iChangeTourism > 0)
			{
				if (sTooltipSink != NULL)
				{
					(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
					(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION_CBP").toUTF8();
					(*sTooltipSink) += "[ENDCOLOR]";
				}
				bValid = false;
			}
		}
		for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
		{
			if (it->GetEffects()->iChangeTourism > 0)
			{
				if (sTooltipSink != NULL)
				{
					(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
					(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_CONTRADICTION_CBP").toUTF8();
					(*sTooltipSink) += "[ENDCOLOR]";
				}
				bValid = false;
			}
		}
	}
#endif

#if defined(MOD_EVENTS_RESOLUTIONS)
	if (MOD_EVENTS_RESOLUTIONS) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanPropose, eProposer, eResolution, iChoice, true) == GAMEEVENTRETURN_FALSE) {
			bValid = false;
		}
	}
#endif
	
	return bValid;
}

bool CvLeague::CanProposeRepeal(int iResolutionID, PlayerTypes eProposer, CvString* sTooltipSink)
{
	bool bValid = true;

	// Is a player proposing this?
	if (eProposer != NO_PLAYER)
	{
		// Must be a member
		if (!IsMember(eProposer))
			bValid = false;

		// Must have right to propose
		if (!CanPropose(eProposer))
			bValid = false;
	}
	CvAssertMsg(eProposer != NO_PLAYER, "Checking to see if a NO_PLAYER can propose a repeal. Please send Anton your save file and version.");

	// Must not already be proposed
	if (IsRepealProposed(iResolutionID))
	{
		if (sTooltipSink != NULL)
		{
			(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_ALREADY_PROPOSED").toUTF8();
			(*sTooltipSink) += "[ENDCOLOR]";
		}
		bValid = false;
	}

	int iChoice = -1;
	// Must already be active
	for (uint iIndex = 0; iIndex < m_vActiveResolutions.size(); iIndex++)
	{
		if (m_vActiveResolutions[iIndex].GetID() == iResolutionID)
		{
			iChoice = m_vActiveResolutions[iIndex].GetProposerDecision()->GetDecision();
		}
	}

#if defined(MOD_EVENTS_RESOLUTIONS)
	if (MOD_EVENTS_RESOLUTIONS) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanPropose, eProposer, iResolutionID, iChoice, false) == GAMEEVENTRETURN_FALSE) {
			bValid = false;
		}
	}
#endif
	
	
	// Must already be active
	for (uint iIndex = 0; iIndex < m_vActiveResolutions.size(); iIndex++)
	{
		if (m_vActiveResolutions[iIndex].GetID() == iResolutionID)
		{
			return bValid;
		}
	}
	CvAssertMsg(false, "Resolution ID does not exist as an Active Resolution. Please send Anton your save file and version.");
	return false;
}

// Check if a resolution's effects are valid in the game (Game Options, Embargo on a civ that is now dead, etc.)
bool CvLeague::IsResolutionEffectsValid(ResolutionTypes eResolution, int iProposerChoice, CvString* sTooltipSink)
{
	if (eResolution == NO_RESOLUTION)
	{
		return false;
	}
	CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
	if (pInfo == NULL)
	{
		return false;
	}

	if (pInfo->IsDiplomaticVictory())
	{
		VictoryTypes eDiploVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
		if (eDiploVictory == NO_VICTORY)
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
		else
		{
			if (!GC.getGame().isVictoryValid(eDiploVictory))
			{
				if (sTooltipSink != NULL)
				{
					(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
					(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
					(*sTooltipSink) += "[ENDCOLOR]";
				}
				return false;
			}
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eDiploVictory != NO_VICTORY)
		{
			for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
			{
				LeagueSpecialSessionTypes e = (LeagueSpecialSessionTypes)i;
				CvLeagueSpecialSessionEntry* pSessionInfo = GC.getLeagueSpecialSessionInfo(e);
				if(pSessionInfo->IsUnitedNations())
				{
					ResolutionTypes eResolution = LeagueHelpers::IsResolutionForTriggerActive(pSessionInfo->GetResolutionTrigger());
					if(eResolution == NO_RESOLUTION)
					{
						if (sTooltipSink != NULL)
						{
							(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
							(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_PREREQ").toUTF8();
							(*sTooltipSink) += "[ENDCOLOR]";
						}
						return false;
					}
				}
			}
		}
#endif
	}
	if (pInfo->IsEmbargoCityStates())
	{
		if (GC.getGame().GetNumMinorCivsEver() <= 0)
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}
	if (pInfo->IsEmbargoPlayer())
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) iProposerChoice;
		CvAssert(eTargetPlayer >= 0 && eTargetPlayer < MAX_MAJOR_CIVS);
		if (eTargetPlayer >= 0 && eTargetPlayer < MAX_MAJOR_CIVS)
		{
			// Player is dead
			if (!GET_PLAYER(eTargetPlayer).isAlive())
			{
				return false;
			}

			// Player is alive but no cities left (Complete Kills option)
			if (GET_PLAYER(eTargetPlayer).getNumCities() <= 0)
			{
				return false;
			}
		}
	}
	if (pInfo->IsNoResourceHappiness())
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}
	if (pInfo->GetMemberDiscoveredTechMod() != 0)
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}
	if (pInfo->GetVotesForFollowingReligion() != 0 ||
		pInfo->GetHolyCityTourism() != 0 ||
		pInfo->GetReligionSpreadStrengthMod() != 0)
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}
	if (pInfo->GetVotesForFollowingIdeology() != 0 ||
		pInfo->GetOtherIdeologyRebellionMod() != 0)
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && (pInfo->IsOpenDoor() || pInfo->IsSphereOfInfluence()))
	{
		if (GC.getGame().GetNumMinorCivsEver() <= 0)
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
		PlayerTypes eTargetPlayer = (PlayerTypes) iProposerChoice;
		if (!GET_PLAYER(eTargetPlayer).isMinorCiv())
		{
			return false;
		}
		if (eTargetPlayer >= 0 && GET_PLAYER(eTargetPlayer).isMinorCiv())
		{
			// Player is dead
			if (!GET_PLAYER(eTargetPlayer).isAlive())
			{
				return false;
			}

			// Player is alive but no cities left (Complete Kills option)
			if (GET_PLAYER(eTargetPlayer).getNumCities() <= 0)
			{
				return false;
			}
		}
	}

	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->IsEmbargoIdeology())
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_POLICIES))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES && (pInfo->GetVassalMaintenanceGoldPercent() != 0 || pInfo->IsEndAllCurrentVassals()))
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_GAMEOPTION").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
		bool bValid = false;
		PlayerTypes eLoopPlayer;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if((eLoopPlayer != NO_PLAYER) && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				if(GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).GetNumVassals() > 0)
				{
					bValid = true;
					break;
				}
			}
		}
		if(!bValid)
		{
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_NO_VASSALS").toUTF8();
				(*sTooltipSink) += "[ENDCOLOR]";
			}
			return false;
		}
	}
#endif

	return true;
}

bool CvLeague::IsAnythingProposed()
{
	bool bEnactProposals = !m_vEnactProposals.empty();
	bool bRepealProposals = !m_vRepealProposals.empty();

	return (bEnactProposals || bRepealProposals);
}

//antonjs: todo: refactor:
bool CvLeague::IsActiveResolution(ResolutionTypes eResolution, int iProposerChoice)
{
	CvAssertMsg(eResolution >= 0, "Invalid resolution type. Please send Anton your save file and version.");
	CvAssertMsg(eResolution < GC.getNumResolutionInfos(), "Resolution index out of bounds. Please send Anton your save file and version.");

	for (uint i = 0; i < m_vActiveResolutions.size(); i++)
	{
		if (m_vActiveResolutions[i].GetType() == eResolution)
		{
			CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
			if (pInfo != NULL)
			{
				// Unique Type resolutions can only have one active resolution of its type at once
				if (pInfo->IsUniqueType())
				{
					return true;
				}

				if (m_vActiveResolutions[i].GetProposerDecision()->GetDecision() == iProposerChoice)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvLeague::IsProposed(int iResolutionID, bool bRepeal, bool bCheckOnHold)
{
	bool bFound = false;
	if (bRepeal)
	{
		for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
		{
			if (it->GetID() == iResolutionID)
			{
				bFound = true;
				break;
			}
		}
		if (bCheckOnHold)
		{
			for (RepealProposalList::iterator it = m_vRepealProposalsOnHold.begin(); it != m_vRepealProposalsOnHold.end(); ++it)
			{
				if (it->GetID() == iResolutionID)
				{
					bFound = true;
					break;
				}
			}
		}
	}
	else
	{
		for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
		{
			if (it->GetID() == iResolutionID)
			{
				bFound = true;
				break;
			}
		}
		if (bCheckOnHold)
		{
			for (EnactProposalList::iterator it = m_vEnactProposalsOnHold.begin(); it != m_vEnactProposalsOnHold.end(); ++it)
			{
				if (it->GetID() == iResolutionID)
				{
					bFound = true;
					break;
				}
			}
		}
	}
	return bFound;
}

//antonjs: todo: refactor:
bool CvLeague::IsEnactProposed(ResolutionTypes eResolution, int iProposerChoice)
{
	CvAssertMsg(eResolution >= 0, "Invalid resolution type. Please send Anton your save file and version.");
	CvAssertMsg(eResolution < GC.getNumResolutionInfos(), "Resolution index out of bounds. Please send Anton your save file and version.");

	for (uint i = 0; i < m_vEnactProposals.size(); i++)
	{
		if (m_vEnactProposals[i].GetType() == eResolution)
		{
			CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
			if (pInfo != NULL)
			{
				// Unique Type resolutions can only have one active resolution of its type at once
				if (pInfo->IsUniqueType())
				{
					return true;
				}

				if (m_vEnactProposals[i].GetProposerDecision()->GetDecision() == iProposerChoice)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvLeague::IsRepealProposed(int iResolutionID) const
{
	for (uint i = 0; i < m_vRepealProposals.size(); i++)
	{
		if (m_vRepealProposals[i].GetTargetResolutionID() == iResolutionID)
		{
			return true;
		}
	}

	return false;
}

std::vector<int> CvLeague::GetChoicesForDecision(ResolutionDecisionTypes eDecision, PlayerTypes eDecider)
{
	std::vector<int> vChoices;
	switch(eDecision)
	{
	case RESOLUTION_DECISION_NONE:
		break;
	case RESOLUTION_DECISION_YES_OR_NO:
	case RESOLUTION_DECISION_REPEAL:
		vChoices.push_back(LeagueHelpers::CHOICE_YES);
		vChoices.push_back(LeagueHelpers::CHOICE_NO);
		break;
	case RESOLUTION_DECISION_ANY_MEMBER:
		for (uint i = 0; i < m_vMembers.size(); i++)
		{
			vChoices.push_back(m_vMembers[i].ePlayer);
		}
		break;
	case RESOLUTION_DECISION_CITY:
		break;
	case RESOLUTION_DECISION_ANY_LUXURY_RESOURCE:
		for (int i = 0; i < GC.getNumResourceInfos(); i++)
		{
			CvResourceInfo* pInfo = GC.getResourceInfo((ResourceTypes)i);
			if (pInfo && pInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				if (GC.getMap().getNumResources((ResourceTypes)i) > 0)
				{
					vChoices.push_back(i);
				}
			}
		}
		break;
	case RESOLUTION_DECISION_MAJOR_CIV_MEMBER:
		for (uint i = 0; i < m_vMembers.size(); i++)
		{
			if (!GET_PLAYER(m_vMembers[i].ePlayer).isMinorCiv())
			{
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
				if(!GET_TEAM(GET_PLAYER(m_vMembers[i].ePlayer).getTeam()).IsVassalOfSomeone())
				{
#endif
					vChoices.push_back(m_vMembers[i].ePlayer);
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
				}
#endif
			}
		}
		break;
	case RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER:
		for (uint i = 0; i < m_vMembers.size(); i++)
		{
			if (m_vMembers[i].ePlayer != eDecider && !GET_PLAYER(m_vMembers[i].ePlayer).isMinorCiv())
			{
				vChoices.push_back(m_vMembers[i].ePlayer);
			}
		}
		break;
	case RESOLUTION_DECISION_RELIGION:
		for (uint i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes e = (PlayerTypes) i;
#if defined(MOD_RELIGION_LOCAL_RELIGIONS)
			if (GET_PLAYER(e).isAlive() && GET_PLAYER(e).GetReligions()->HasCreatedReligion(true))
#else
			if (GET_PLAYER(e).isAlive() && GET_PLAYER(e).GetReligions()->HasCreatedReligion())
#endif
			{
				vChoices.push_back(GET_PLAYER(e).GetReligions()->GetReligionCreatedByPlayer());
			}
		}
		break;
	case RESOLUTION_DECISION_IDEOLOGY:
		for (int i = 0; i < GC.getNumPolicyBranchInfos(); i++)
		{
			CvPolicyBranchEntry* pInfo = GC.getPolicyBranchInfo((PolicyBranchTypes)i);
			if (pInfo != NULL)
			{
				if (pInfo->IsPurchaseByLevel())
				{
					vChoices.push_back(pInfo->GetID());
				}
			}
		}
		break;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	case RESOLUTION_DECISION_CITY_CSD:
		for (uint i = 0; i < m_vMembers.size(); i++)
		{
			if (GET_PLAYER(m_vMembers[i].ePlayer).isMinorCiv())
			{
				vChoices.push_back(m_vMembers[i].ePlayer);
			}
		}
		break;
#endif
	default:
		break;
	}
	return vChoices;
}

CvString CvLeague::GetTextForChoice(ResolutionDecisionTypes eDecision, int iChoice)
{
	return LeagueHelpers::GetTextForChoice(eDecision, iChoice);
}

// Returns all possible resolutions, even ones that might already be active
std::vector<ResolutionTypes> CvLeague::GetInactiveResolutions() const
{
	std::vector<ResolutionTypes> v;
	for (int i = 0; i < GC.getNumResolutionInfos(); i++)
	{
		ResolutionTypes e = (ResolutionTypes)i;
		CvResolutionEntry* pInfo = GC.getResolutionInfo(e);
		if (pInfo)
		{
			v.push_back(e);
		}
	}

	return v;
}

CvEnactProposal* CvLeague::GetEnactProposal(int iResolutionID)
{
	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
	{
		if (it->GetID() == iResolutionID)
		{
			return &(*it);
		}
	}
	return NULL;
}

EnactProposalList CvLeague::GetEnactProposals() const
{
	return m_vEnactProposals;
}

EnactProposalList CvLeague::GetEnactProposalsOnHold() const
{
	return m_vEnactProposalsOnHold;
}

CvRepealProposal* CvLeague::GetRepealProposal(int iResolutionID)
{
	for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
	{
		if (it->GetID() == iResolutionID)
		{
			return &(*it);
		}
	}
	return NULL;
}

RepealProposalList CvLeague::GetRepealProposals() const
{
	return m_vRepealProposals;
}

RepealProposalList CvLeague::GetRepealProposalsOnHold() const
{
	return m_vRepealProposalsOnHold;
}

CvActiveResolution* CvLeague::GetActiveResolution(int iResolutionID, int iValue)
{
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetType() == iResolutionID)
		{
			if (iValue == -1)
				return &(*it);
			else if (iValue == it->GetProposerDecision()->GetDecision())
				return &(*it);
		}
	}
	return NULL;
}

ActiveResolutionList CvLeague::GetActiveResolutions() const
{
	return m_vActiveResolutions;
}

int CvLeague::GetNumResolutionsEverEnacted() const
{
	return m_iNumResolutionsEverEnacted;
}

int CvLeague::GetNumProposersPerSession() const
{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int iProposers = MIN(GC.getLEAGUE_PROPOSERS_PER_SESSION(), GetNumMembers());
#else
	int iProposers = MIN(LeagueHelpers::PROPOSERS_PER_SESSION, GetNumMembers());
#endif
	return iProposers;
}

void CvLeague::AddMember(PlayerTypes ePlayer)
{
	if (ePlayer < 0 || ePlayer >= MAX_CIV_PLAYERS)
	{
		CvAssertMsg(false, "Trying to add a player to a League who has an invalid Player ID.  Please send Anton your save file and version.");
		return;
	}
	if (IsMember(ePlayer))
	{
		CvAssertMsg(false, "Trying to add a player to a League that they are already a member of.  Please send Anton your save file and verison.");
		return;
	}

	Member member;
	member.ePlayer = ePlayer;
	m_vMembers.push_back(member);

	// Majors meet each other and reveal their capitals
	if (GET_PLAYER(ePlayer).isAlive() && !GET_PLAYER(ePlayer).isMinorCiv())
	{
		for (MemberList::const_iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
		{
			PlayerTypes eOtherPlayer = it->ePlayer;
			if (eOtherPlayer != NO_PLAYER && eOtherPlayer != ePlayer && GET_PLAYER(eOtherPlayer).isAlive() && !GET_PLAYER(eOtherPlayer).isMinorCiv())
			{
				TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
				TeamTypes eOtherTeam = GET_PLAYER(eOtherPlayer).getTeam();
				if (!GET_TEAM(eTeam).isHasMet(eOtherTeam))
				{
					GET_TEAM(eTeam).meet(eOtherTeam, /*bSuppressMessages*/ false);
				}

				CvCity* pCapital = GET_PLAYER(ePlayer).getCapitalCity();
				if (pCapital && pCapital->plot() && !pCapital->plot()->isRevealed(eOtherTeam))
				{
					pCapital->plot()->setRevealed(eOtherTeam, true);
				}
				CvCity* pOtherCapital = GET_PLAYER(eOtherPlayer).getCapitalCity();
				if (pOtherCapital && pOtherCapital->plot() && !pOtherCapital->plot()->isRevealed(eTeam))
				{
					pOtherCapital->plot()->setRevealed(eTeam, true);
				}
			}
		}
	}

	// Apply effects of any existing active resolutions
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		it->DoEffects(ePlayer);
	}

	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

void CvLeague::RemoveMember(PlayerTypes ePlayer)
{
	if (!IsMember(ePlayer))
	{
		CvAssertMsg(false, "Trying to remove a player from a League that they are not a member of.  Please send Anton your save file and version.");
		return;
	}

	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		if (it->ePlayer == ePlayer)
		{
			m_vMembers.erase(it);
			it--;
		}
	}

	// Remove effects of any existing active resolutions
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		it->RemoveEffects(ePlayer);
	}

	// Check host
	if (IsHostMember(ePlayer))
	{
		AssignNewHost();
	}

	// Check current proposals and active resolutions to see if they are still valid
	CheckProposalsValid();
	CheckResolutionsValid();

	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

bool CvLeague::IsMember(PlayerTypes ePlayer) const
{
	for (uint i = 0; i < m_vMembers.size(); i++)
	{
		if (m_vMembers[i].ePlayer == ePlayer)
		{
			return true;
		}
	}
	return false;
}

int CvLeague::GetNumMembers() const
{
	return m_vMembers.size();
}

bool CvLeague::CanVote(PlayerTypes ePlayer)
{
	if (CanEverVote(ePlayer))
	{
		return (GetRemainingVotesForMember(ePlayer) > 0);
	}
	return false;
}

bool CvLeague::CanEverVote(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index). Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index). Please send Anton your save file and version.");

	if (IsMember(ePlayer))
	{
		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{
			return true;
		}
	}
	return false;
}

int CvLeague::GetRemainingVotesForMember(PlayerTypes ePlayer)
{
	if (CanEverVote(ePlayer))
	{
		return (GetMember(ePlayer)->iVotes);
	}
	return 0;
}

int CvLeague::GetSpentVotesForMember(PlayerTypes ePlayer)
{
	int iVotes = 0;

	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
	{
		iVotes += it->GetVoterDecision()->GetVotesCastByPlayer(ePlayer);
	}
	for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
	{
		iVotes += it->GetRepealDecision()->GetVotesCastByPlayer(ePlayer);
	}
	
	if (IsMember(ePlayer))
	{
		iVotes += GetMember(ePlayer)->iAbstainedVotes;
	}

	if (iVotes > 0)
	{
		CvAssert(CanEverVote(ePlayer));
	}

	return iVotes;
}

int CvLeague::GetPotentialVotesForMember(PlayerTypes ePlayer, PlayerTypes eFromPlayer)
{
	if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague)
		{
			//# of votes you can buy based on diplomat strength.
			int iNumVotes = 0;
			int iVotes = pLeague->CalculateStartingVotesForMember(eFromPlayer);
			if (GET_PLAYER(eFromPlayer).GetLeagueAI()->HasVoteCommitment())
			{
				for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
				{
					std::vector<int> vChoices = GetChoicesForDecision(it->GetVoterDecision()->GetType(), NO_PLAYER);
					for (uint i = 0; i < vChoices.size(); i++)
					{
						PlayerTypes eLoopPlayer;
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if((eLoopPlayer != NO_PLAYER) && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && (eLoopPlayer != eFromPlayer))
							{
								iNumVotes += GET_PLAYER(eFromPlayer).GetLeagueAI()->GetVoteCommitment(eLoopPlayer, it->GetID(), vChoices[i], /*bEnact*/ true);
								iNumVotes += GET_PLAYER(eFromPlayer).GetLeagueAI()->GetVoteCommitment(eLoopPlayer, it->GetID(), vChoices[i], /*bEnact*/ false);
							}
						}
					}
				}
			}
			iVotes = (iVotes - iNumVotes);
			if((iVotes > 0) && GET_PLAYER(ePlayer).GetEspionage()->IsMyDiplomatVisitingThem(eFromPlayer, false))
			{
				int iSpyIndex = GET_PLAYER(ePlayer).GetEspionage()->GetSpyIndexInCity(GET_PLAYER(eFromPlayer).getCapitalCity());
				if(iSpyIndex != -1)
				{
					CvEspionageSpy* pSpy = GET_PLAYER(ePlayer).GetEspionage()->GetSpyByID(iSpyIndex);
					if (pSpy != NULL)
					{
						int iRank = pSpy->GetSpyRank(ePlayer);
						iRank = (5 - iRank);
						if (iRank > 0)
						{
							iVotes /= iRank;
							if (iVotes <= 0)
							{
								iVotes = 1;
							}
							return iVotes;
						}
					}
				}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
				// They are our vassal, so yes, we have a diplomat already
				else if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).GetMaster() == GET_PLAYER(ePlayer).getTeam())
				{
					int iRank = 2;
					iVotes /= iRank;
					if(iVotes <= 0)
					{
						iVotes = 1;
					}
					return iVotes;
				}
#endif
			}
		}
	}
	return 0;
}

int CvLeague::GetCoreVotesForMember(PlayerTypes ePlayer)
{
	int iVotes = 0;
	if (CanEverVote(ePlayer))
	{
		LeagueSpecialSessionTypes eGoverningSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
		if (GetCurrentSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
		{
			eGoverningSpecialSession = GetCurrentSpecialSession();
		}
		else if (GetLastSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
		{
			eGoverningSpecialSession = GetLastSpecialSession();
		}

		CvAssert(eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION);
		if (eGoverningSpecialSession == NO_LEAGUE_SPECIAL_SESSION) return GC.getLEAGUE_MEMBER_VOTES_BASE();
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);
		CvAssert(pInfo != NULL);
		if (pInfo == NULL) return GC.getLEAGUE_MEMBER_VOTES_BASE();
		Member* pMember = GetMember(ePlayer);
		CvAssert(pMember != NULL);
		if (pMember == NULL) return GC.getLEAGUE_MEMBER_VOTES_BASE();

		iVotes += pInfo->GetCivDelegates();
	}
	return iVotes;
}

int CvLeague::CalculateStartingVotesForMember(PlayerTypes ePlayer, bool bForceUpdateSources)
{
	//try the cached value first
	Member* thisMember = GetMember(ePlayer);
	if (thisMember != NULL && thisMember->ePlayer != NO_PLAYER)
	{
		if (GC.getGame().getTurnSlice() == thisMember->m_startingVotesCacheTime)
			return thisMember->m_startingVotesCached;
	}

	int iVotes = 0;
#if defined(MOD_BATTLE_ROYALE)
	// if battle royale is enabled, the human player is the observer, and should not be allowed votes
	if (MOD_BATTLE_ROYALE && GET_PLAYER(ePlayer).isHuman())
	{
		return 0;
	}
#endif
	if (CanEverVote(ePlayer))
	{
		LeagueSpecialSessionTypes eGoverningSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
		if (GetCurrentSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
		{
			eGoverningSpecialSession = GetCurrentSpecialSession();
		}
		else if (GetLastSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
		{
			eGoverningSpecialSession = GetLastSpecialSession();
		}
		
		CvAssert(eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION);
		if (eGoverningSpecialSession == NO_LEAGUE_SPECIAL_SESSION) return GC.getLEAGUE_MEMBER_VOTES_BASE();
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);
		CvAssert(pInfo != NULL);
		if (pInfo == NULL) return GC.getLEAGUE_MEMBER_VOTES_BASE();
		Member* pMember = GetMember(ePlayer);
		CvAssert(pMember != NULL);
		if (pMember == NULL) return GC.getLEAGUE_MEMBER_VOTES_BASE();

		// Base votes
		int iBaseVotes = GetCoreVotesForMember(ePlayer);
		iVotes += iBaseVotes;

		// Extra votes (ie. leading in previous failed Diplo Victory proposals)
		int iExtraVotes = pMember->iExtraVotes;
		iVotes += iExtraVotes;

		// Hosting the league
		int iHostVotes = 0;
		if (IsHostMember(ePlayer))
		{
			iHostVotes += pInfo->GetHostDelegates();
		}
		iVotes += iHostVotes;

		// City-State allies
		int iCityStateVotes = 0;
		for (int i = MAX_MAJOR_CIVS; i < MAX_CIV_PLAYERS; i++)
		{
			PlayerTypes eMinor = (PlayerTypes) i;
			if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
			{
				iCityStateVotes += pInfo->GetCityStateDelegates();
			}
		}
		iVotes += iCityStateVotes;

		// Diplomats after Globalization tech
		int iDiplomatVotes = 0;
		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes eMajor = (PlayerTypes) i;
			if (GET_PLAYER(eMajor).isAlive())
			{
				if (GET_PLAYER(ePlayer).GetEspionage()->IsMyDiplomatVisitingThem(eMajor))
				{
					iDiplomatVotes += GET_PLAYER(ePlayer).GetExtraVotesPerDiplomat();
				}
			}
		}
		iVotes += iDiplomatVotes;

		// Wonders
		int iWonderVotes = GET_PLAYER(ePlayer).GetExtraLeagueVotes();
#if defined(MOD_BALANCE_CORE)
		if(iWonderVotes > 0)
		{
			int iNumMinor = (GC.getGame().GetNumMinorCivsEver(true) / 8);
			if(iNumMinor > 0)
			{
				iWonderVotes = (iWonderVotes * iNumMinor);
			}
		}
		iWonderVotes += GET_PLAYER(ePlayer).GetSingleVotes();
#endif
		iVotes += iWonderVotes;

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		// Improvements
		int iImprovementVotes = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? GET_PLAYER(ePlayer).GetImprovementLeagueVotes() : 0;
		iVotes += iImprovementVotes;

		// Religion Founder
		int iFaithVotes = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? GET_PLAYER(ePlayer).GetFaithToVotes() : 0;
		iVotes += iFaithVotes;

		// Freedom Follower
		int iGetDoFToVotes = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? GET_PLAYER(ePlayer).GetDoFToVotes() : 0;
		iVotes += iGetDoFToVotes;

		// Autocracy Follower
		int iCapitalsToVotes = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? GET_PLAYER(ePlayer).GetCapitalsToVotes() : 0;
		iVotes += iCapitalsToVotes;

		// Order Follower
		int iRAToVotes = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? GET_PLAYER(ePlayer).GetRAToVotes() : 0;
		iVotes += iRAToVotes;

		// Order Alt.
		int iDPToVotes = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? GET_PLAYER(ePlayer).GetDefensePactsToVotes() : 0;
		iVotes += iDPToVotes;
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
		int iReligionVotes = 0;
		if(MOD_BALANCE_CORE_BELIEFS)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(ePlayer);
			bool bFounded = true;
			if(eFoundedReligion == NO_RELIGION)
			{
				bFounded = false;
				eFoundedReligion = GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
			}
			if(eFoundedReligion != NO_RELIGION)
			{
				const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, ePlayer);
				if(pReligion)
				{
					CvCity* pHolyCity = bFounded ? pReligion->GetHolyCity() : GET_PLAYER(ePlayer).getCapitalCity();
					if (pHolyCity != NULL)
					{
						int iExtraVotes = pReligion->m_Beliefs.GetExtraVotes(ePlayer, pHolyCity, true);
						if (iExtraVotes > 0)
						{
							int iNumMinor = (GC.getGame().GetNumMinorCivsEver(true) / 8);
							if ((iNumMinor) > 0)
							{
								iReligionVotes = (iExtraVotes * iNumMinor);
							}
							iVotes += iReligionVotes;
						}

						int iNumImprovementInfos = GC.getNumImprovementInfos();
						for (int jJ = 0; jJ < iNumImprovementInfos; jJ++)
						{
							int iPotentialVotes = pReligion->m_Beliefs.GetVoteFromOwnedImprovement((ImprovementTypes)jJ);
							if (iPotentialVotes > 0)
							{
								int numImprovements = GET_PLAYER(ePlayer).getImprovementCount((ImprovementTypes)jJ);
								int iTempVotes = numImprovements / iPotentialVotes;
								iReligionVotes += iTempVotes;
								iVotes += iTempVotes;
							}
						}
					}
				}
			}
		}
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
		int iPolicyVotes = 0;
		if(MOD_BALANCE_CORE_POLICIES)
		{
			iPolicyVotes = GET_PLAYER(ePlayer).GetFreeWCVotes();
			if(iPolicyVotes > 0)
			{
				//1 vote per 8 CS in game.
				int iNumMinor = (GC.getGame().GetNumMinorCivsEver(true) / 8);
				if((iNumMinor) > 0)
				{
					 iPolicyVotes = (iPolicyVotes * iNumMinor);
				}
				iVotes += iPolicyVotes;
			}
		}
#endif
#if defined(MOD_BALANCE_CORE)
		int iTraitVotes = 0;
		if(MOD_BALANCE_CORE_POLICIES)
		{
			int iTraitPotential = 0;
			//The Base INTs
			int iNumAllies = 0;
			int iNumMinorFollowingRel = 0;
			// Loop through all minors and get the total number we've met.
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

				if (eMinor != ePlayer && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
				{
					if (GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
					{
						iNumAllies++;
					}
					if (GET_PLAYER(eMinor).GetMinorCivAI()->IsSameReligionAsMajor(ePlayer))
					{
						iNumMinorFollowingRel++;
					}
				}
			}
			if(iNumAllies != 0)
			{
				iTraitPotential = GET_PLAYER(ePlayer).GetPlayerTraits()->GetVotePerXCSAlliance();
				if(iTraitPotential != 0)
				{
					iTraitVotes = (iNumAllies / iTraitPotential);
				}
			}
			if(iNumMinorFollowingRel != 0)
			{
				iTraitPotential = GET_PLAYER(ePlayer).GetPlayerTraits()->GetVotePerXCSFollowingYourReligion();
				if(iTraitPotential != 0)
				{
					iTraitVotes = (iNumMinorFollowingRel / iTraitPotential);
				}
			}

			iVotes += iTraitVotes;
		}
		int iGPTVotes = 0;
		//Votes from GPT
		int iVotesPerGPT = GET_PLAYER(ePlayer).GetVotesPerGPT();
		if(iVotesPerGPT > 0)
		{
			int iGPT = GET_PLAYER(ePlayer).GetTreasury()->CalculateGrossGold();
			if(iGPT > 0)
			{
				iGPTVotes = (iGPT / iVotesPerGPT);
				//Capped at 1/4 # of minor civs ever in game.
				int iCap = GC.getGame().GetNumMinorCivsEver(true);
				iCap /= 4;
				if(iCap <= 0)
				{
					iCap = 1;
				}
				if(iGPTVotes > iCap)
				{
					iGPTVotes = iCap;
				}
				iVotes += iGPTVotes;
			}
		}
#endif

		// World Religion
		int iWorldReligionVotes = GetExtraVotesForFollowingReligion(ePlayer);
		iVotes += iWorldReligionVotes;

		// World Ideology
		int iWorldIdeologyVotes = GetExtraVotesForFollowingIdeology(ePlayer);
		iVotes += iWorldIdeologyVotes;

#if defined(MOD_BALANCE_CORE)
		int iNumMarried = 0;
		if (GET_PLAYER(ePlayer).IsDiplomaticMarriage())
		{
			
			// Loop through all minors and get the total number we've met.
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes)iPlayerLoop;

				if (eMinor != ePlayer && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
				{
					if (GET_PLAYER(eMinor).GetMinorCivAI()->IsMarried(ePlayer) && !GET_PLAYER(eMinor).IsAtWarWith(ePlayer))
					{
						iNumMarried++;
					}
				}
			}
			if (iNumMarried > 0)
			{
				iVotes += iNumMarried;
			}
		}
#endif

		// Vote Sources - Normally this is only updated when we are not in session
		if (bForceUpdateSources || !IsInSession())
		{
			pMember->sVoteSources = "";
			if (iBaseVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_BASE_VOTES");
				sTemp << iBaseVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (iExtraVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_EXTRA_VOTES");
				sTemp << iExtraVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (iHostVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_HOST_VOTES");
				sTemp << iHostVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (pInfo->GetCityStateDelegates() > 0) // Show even if we have none from this source, to remind players that CS can give Delegates
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_CS_VOTES");
				sTemp << iCityStateVotes;
				sTemp << pInfo->GetCityStateDelegates();
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (iDiplomatVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_DIPLOMAT_VOTES");
				sTemp << iDiplomatVotes;
				sTemp << GET_PLAYER(ePlayer).GetExtraVotesPerDiplomat();
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (iWonderVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_WONDER_VOTES");
				sTemp << iWonderVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && iImprovementVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_IMPROVEMENT_VOTES");
				sTemp << iImprovementVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && iFaithVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_FAITH_VOTES");
				sTemp << iFaithVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && iCapitalsToVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_CAPITALS_VOTES");
				sTemp << iCapitalsToVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && iGetDoFToVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_DOF_VOTES");
				sTemp << iGetDoFToVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && iRAToVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_RA_VOTES");
				sTemp << iRAToVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && iDPToVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_DP_VOTES");
				sTemp << iDPToVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
#endif
#if defined(MOD_BALANCE_CORE_BELIEFS)
			if (MOD_BALANCE_CORE_BELIEFS && iReligionVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_RELIGION_REFORMATION_VOTES");
				sTemp << iReligionVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
#endif
#if defined(MOD_BALANCE_CORE_POLICIES)
			if(MOD_BALANCE_CORE_POLICIES && iPolicyVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_PATRONAGE_FINISHER_VOTES");
				sTemp << iPolicyVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
#endif
#if defined(MOD_BALANCE_CORE)
			if(MOD_BALANCE_CORE && iTraitVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_TRAIT_VOTES");
				sTemp << iTraitVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if(MOD_BALANCE_CORE && iNumMarried > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_MARRIED_VOTES");
				sTemp << iNumMarried;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if(MOD_BALANCE_CORE && iGPTVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_GPT_VOTES");
				sTemp << iGPTVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
#endif
			if (iWorldReligionVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_WORLD_RELIGION_VOTES");
				sTemp << iWorldReligionVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
			if (iWorldIdeologyVotes > 0)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_WORLD_IDEOLOGY_VOTES");
				sTemp << iWorldIdeologyVotes;
				pMember->sVoteSources += sTemp.toUTF8();
			}
		}
	}

	//update cache
	if (thisMember != NULL && thisMember->ePlayer != NO_PLAYER)
	{
		thisMember->m_startingVotesCacheTime = GC.getGame().getTurnSlice();
		thisMember->m_startingVotesCached = iVotes;
	}

	return iVotes;
}

bool CvLeague::CanPropose(PlayerTypes ePlayer)
{
	if (CanEverPropose(ePlayer))
	{
		if (GetMember(ePlayer)->bMayPropose)
		{
			return (GetRemainingProposalsForMember(ePlayer) > 0);
		}
	}
	return false;
}

bool CvLeague::CanEverPropose(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index). Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index). Please send Anton your save file and version.");

	if (IsMember(ePlayer))
	{
		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{
			return true;
		}
	}
	return false;
}

int CvLeague::GetRemainingProposalsForMember(PlayerTypes ePlayer)
{
	int iRemaining = 0;
	if (IsMember(ePlayer))
	{
		iRemaining += GetMember(ePlayer)->iProposals;
	}
	return iRemaining;
}

int CvLeague::GetNumProposalsByMember(PlayerTypes ePlayer)
{
	int iValue = 0;
	if (IsMember(ePlayer))
	{
		for (uint i = 0; i < m_vEnactProposals.size(); i++)
		{
			if (m_vEnactProposals[i].GetProposalPlayer() == ePlayer)
			{
				iValue++;
			}
		}
		for (uint i = 0; i < m_vRepealProposals.size(); i++)
		{
			if (m_vRepealProposals[i].GetProposalPlayer() == ePlayer)
			{
				iValue++;
			}
		}

		// Also check proposals on hold
		for (uint i = 0; i < m_vEnactProposalsOnHold.size(); i++)
		{
			if (m_vEnactProposalsOnHold[i].GetProposalPlayer() == ePlayer)
			{
				iValue++;
			}
		}
		for (uint i = 0; i < m_vRepealProposalsOnHold.size(); i++)
		{
			if (m_vRepealProposalsOnHold[i].GetProposalPlayer() == ePlayer)
			{
				iValue++;
			}
		}
	}
	return iValue;
}

LeagueHelpers::PlayerList CvLeague::GetMembersThatLikeProposal(ResolutionTypes eResolution, PlayerTypes eObserver, int iProposerChoice, bool bChosen)
{
	LeagueHelpers::PlayerList vMembersSupporting;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (it->ePlayer != eObserver && CanEverVote(it->ePlayer) && !GET_PLAYER(it->ePlayer).isHuman())
		{
			CvLeagueAI::DesireLevels eDesire = GET_PLAYER(it->ePlayer).GetLeagueAI()->EvaluateProposalForProposer(this, eObserver, eResolution, iProposerChoice);
			if (eDesire > CvLeagueAI::DESIRE_WEAK_LIKE)
			{
				vMembersSupporting.push_back(it->ePlayer);

				// This proposal was chosen - how does this affect Diplomacy AI?
				if (bChosen)
				{
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeLikedTheirProposalTurn(eObserver, GC.getGame().getGameTurn());
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeDislikedTheirProposalTurn(eObserver, -1);

					switch (eDesire)
					{
					case CvLeagueAI::DESIRE_ALWAYS:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*-45*/ GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_OVERWHELMING());
						break;
					case CvLeagueAI::DESIRE_STRONG_LIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*-30*/ GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_STRONG());
						break;
					case CvLeagueAI::DESIRE_LIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*-15*/ GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL());
						break;
					}
				}
			}
		}
	}
	return vMembersSupporting;
}

LeagueHelpers::PlayerList CvLeague::GetMembersThatLikeProposal(int iTargetResolutionID, PlayerTypes eObserver, bool bChosen)
{
	LeagueHelpers::PlayerList vMembersSupporting;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (it->ePlayer != eObserver && CanEverVote(it->ePlayer) && !GET_PLAYER(it->ePlayer).isHuman())
		{
			CvLeagueAI::DesireLevels eDesire = GET_PLAYER(it->ePlayer).GetLeagueAI()->EvaluateProposalForProposer(this, eObserver, iTargetResolutionID);
			if (eDesire > CvLeagueAI::DESIRE_WEAK_LIKE)
			{
				vMembersSupporting.push_back(it->ePlayer);

				// This proposal was chosen - how does this affect Diplomacy AI?
				if (bChosen)
				{
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeLikedTheirProposalTurn(eObserver, GC.getGame().getGameTurn());
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeDislikedTheirProposalTurn(eObserver, -1);

					switch (eDesire)
					{
					case CvLeagueAI::DESIRE_ALWAYS:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*-45*/ GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_OVERWHELMING());
						break;
					case CvLeagueAI::DESIRE_STRONG_LIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*-30*/ GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_STRONG());
						break;
					case CvLeagueAI::DESIRE_LIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*-15*/ GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL());
						break;
					}
				}
			}
		}
	}
	return vMembersSupporting;
}

LeagueHelpers::PlayerList CvLeague::GetMembersThatDislikeProposal(ResolutionTypes eResolution, PlayerTypes eObserver, int iProposerChoice, bool bChosen)
{
	LeagueHelpers::PlayerList vMembersDissuading;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (it->ePlayer != eObserver && CanEverVote(it->ePlayer) && !GET_PLAYER(it->ePlayer).isHuman())
		{
			CvLeagueAI::DesireLevels eDesire = GET_PLAYER(it->ePlayer).GetLeagueAI()->EvaluateProposalForProposer(this, eObserver, eResolution, iProposerChoice);
			if (eDesire < CvLeagueAI::DESIRE_WEAK_DISLIKE)
			{
				vMembersDissuading.push_back(it->ePlayer);

				// This proposal was chosen - how does this affect Diplomacy AI?
				if (bChosen)
				{
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeDislikedTheirProposalTurn(eObserver, GC.getGame().getGameTurn());
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeLikedTheirProposalTurn(eObserver, -1);

					switch (eDesire)
					{
					case CvLeagueAI::DESIRE_NEVER:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*45*/ GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_OVERWHELMING());
						break;
					case CvLeagueAI::DESIRE_STRONG_DISLIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*30*/ GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_STRONG());
						break;
					case CvLeagueAI::DESIRE_DISLIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*15*/ GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL());
						break;
					}
				}
			}
		}
	}
	return vMembersDissuading;
}

LeagueHelpers::PlayerList CvLeague::GetMembersThatDislikeProposal(int iTargetResolutionID, PlayerTypes eObserver, bool bChosen)
{
	LeagueHelpers::PlayerList vMembersDissuading;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (it->ePlayer != eObserver && CanEverVote(it->ePlayer))
		{
			CvLeagueAI::DesireLevels eDesire = GET_PLAYER(it->ePlayer).GetLeagueAI()->EvaluateProposalForProposer(this, eObserver, iTargetResolutionID);
			if (eDesire < CvLeagueAI::DESIRE_WEAK_DISLIKE)
			{
				vMembersDissuading.push_back(it->ePlayer);

				// This proposal was chosen - how does this affect Diplomacy AI?
				if (bChosen)
				{
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeDislikedTheirProposalTurn(eObserver, GC.getGame().getGameTurn());
					GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetWeLikedTheirProposalTurn(eObserver, -1);

					switch (eDesire)
					{
					case CvLeagueAI::DESIRE_NEVER:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*45*/ GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_OVERWHELMING());
						break;
					case CvLeagueAI::DESIRE_STRONG_DISLIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*30*/ GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_STRONG());
						break;
					case CvLeagueAI::DESIRE_DISLIKE:
						GET_PLAYER(it->ePlayer).GetDiplomacyAI()->SetLikedTheirProposalValue(eObserver, /*15*/ GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL());
						break;
					}
				}
			}
		}
	}
	return vMembersDissuading;
}

bool CvLeague::HasHostMember() const
{
	return (m_eHost != NO_PLAYER);
}

bool CvLeague::IsHostMember(PlayerTypes ePlayer) const
{
	return (m_eHost == ePlayer);
}

PlayerTypes CvLeague::GetHostMember() const
{
	return m_eHost;
}

void CvLeague::SetHostMember(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0 && ePlayer < MAX_CIV_PLAYERS, "ePlayer is out of bounds (invalid Index)");
	bool bFound = false;
	if (ePlayer >= 0 && ePlayer < MAX_CIV_PLAYERS)
	{
		for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
		{
			if (ePlayer == it->ePlayer)
			{
				bFound = true;
				if (ePlayer != GetHostMember())
				{
					m_eHost = ePlayer;
					it->bEverBeenHost = true;
					UpdateName();
				}
			}
			else
			{
				it->bAlwaysBeenHost = false;
			}
		}
	}
	CvAssertMsg(bFound, "Could not find the correct player to set as the World Congress host. Please send Anton your save file and version.");
}

int CvLeague::GetConsecutiveHostedSessions() const
{
	return m_iConsecutiveHostedSessions;
}

void CvLeague::SetConsecutiveHostedSessions(int iValue)
{
	m_iConsecutiveHostedSessions = iValue;
}

void CvLeague::ChangeConsecutiveHostedSessions(int iChange)
{
	SetConsecutiveHostedSessions(GetConsecutiveHostedSessions() + iChange);
}

bool CvLeague::HasMemberEverBeenHost(PlayerTypes ePlayer)
{
	bool b = false;
	if (IsMember(ePlayer))
	{
		b = GetMember(ePlayer)->bEverBeenHost;
	}
	return b;
}

bool CvLeague::HasMemberAlwaysBeenHost(PlayerTypes ePlayer)
{
	bool b = false;
	if (IsMember(ePlayer))
	{
		b = GetMember(ePlayer)->bAlwaysBeenHost;
	}
	return b;
}

// Is the project underway, but not yet completed?
bool CvLeague::IsProjectActive(LeagueProjectTypes eLeagueProject) const
{
	for (ProjectList::const_iterator it = m_vProjects.begin(); it != m_vProjects.end(); it++)
	{
		if (it->eType == eLeagueProject)
		{
			if (!it->bComplete)
			{
				return true;
			}
		}
	}
	return false;
}

// Is the project completed?
bool CvLeague::IsProjectComplete(LeagueProjectTypes eLeagueProject) const
{
	for (ProjectList::const_iterator it = m_vProjects.begin(); it != m_vProjects.end(); it++)
	{
		if (it->eType == eLeagueProject)
		{
			if (it->bComplete)
			{
				return true;
			}
		}
	}
	return false;
}

int CvLeague::GetProjectCostPerPlayer(LeagueProjectTypes eLeagueProject) const
{
	CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(eLeagueProject);
	CvAssertMsg(pProjectInfo != NULL, "Looking up project cost for a project that does not exist. Please send Anton your save file and version.");
	int iCost = 0;
	if (pProjectInfo)
	{
		iCost += pProjectInfo->GetCostPerPlayer() * 100;

		iCost *= GC.getGame().getGameSpeedInfo().getConstructPercent();
		iCost /= 100;

		iCost *= GC.getGame().getStartEraInfo().getConstructPercent();
		iCost /= 100;

#if defined(MOD_BALANCE_CORE)
		if (MOD_BALANCE_CORE)
		{
			const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(pProjectInfo->GetProcess());
			if (pkProcessInfo)
			{
				TechTypes eTech = ((TechTypes)pkProcessInfo->getTechPrereq());
				if(eTech != NO_TECH)
				{
					const CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
					if(pkTechInfo)
					{
						if(pkTechInfo->GetEra() < GC.getGame().getCurrentEra())
						{
							int iEraDiff = (GC.getGame().getCurrentEra() - pkTechInfo->GetEra());
							iEraDiff += 1;
							if(iEraDiff <= 0)
							{
								iEraDiff = 1;
							}
							iCost *= (iCost * iEraDiff);
							iCost /= 100;
						}
					}
				}
			}
		}
#endif
	}
	return iCost;
}

// Helper function to lookup cost per player by a BuildingType that is part of the project rewards.
// Assumes that this BuildingType is a reward for at most 1 project.
int CvLeague::GetProjectBuildingCostPerPlayer(BuildingTypes eRewardBuilding) const
{
	int iCost = 0;

	// Is it part of an international project?
	for (int i = 0; i < GC.getNumLeagueProjectInfos(); i++)
	{
		LeagueProjectTypes eProject = (LeagueProjectTypes)i;
		CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(eProject);
		if (pProjectInfo != NULL && pProjectInfo->GetRewardTier3() != NO_LEAGUE_PROJECT_REWARD) // Only check top reward tier
		{
			CvLeagueProjectRewardEntry* pRewardInfo = GC.getLeagueProjectRewardInfo(pProjectInfo->GetRewardTier3());
			if (pRewardInfo != NULL && pRewardInfo->GetBuilding() == eRewardBuilding)
			{
				iCost = GetProjectCostPerPlayer(eProject) / 100;
				break;
			}
		}
	}

	return iCost;
}

int CvLeague::GetProjectCost(LeagueProjectTypes eLeagueProject) const
{
	CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(eLeagueProject);
	CvAssertMsg(pProjectInfo != NULL, "Looking up project cost for a project that does not exist. Please send Anton your save file and version.");
	int iCost = 0;
	if (pProjectInfo)
	{
		iCost += GetProjectCostPerPlayer(eLeagueProject) * GC.getGame().countMajorCivsEverAlive();
	}
	return iCost;
}

int CvLeague::GetProjectProgress(LeagueProjectTypes eProject)
{
	CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(eProject);
	CvAssertMsg(pProjectInfo != NULL, "Looking up project progress for a project that does not exist. Please send Anton your save file and version.");
	int iProgress = 0;
	if (pProjectInfo)
	{
		if (IsProjectActive(eProject) || IsProjectComplete(eProject))
		{
			Project* pProject = GetProject(eProject);
			CvAssert(pProject != NULL)
			if (pProject)
			{
				for (uint i = 0; i < pProject->vProductionList.size(); i++)
				{
					iProgress += pProject->vProductionList[i];
				}
			}
		}
	}
	return iProgress;
}

bool CvLeague::CanMemberContribute(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject) const
{
	if (IsMember(ePlayer))
	{
		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{
			if (IsProjectActive(eLeagueProject))
			{
				return true;
			}
		}
	}
	return false;
}

int CvLeague::GetMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject) const
{
	int iValue = 0;
	if (ePlayer >= 0 && ePlayer < MAX_MAJOR_CIVS)
	{
		for (ProjectList::const_iterator it = m_vProjects.begin(); it != m_vProjects.end(); it++)
		{
			if (it->eType == eLeagueProject)
			{
				iValue += it->vProductionList[ePlayer];
				break;
			}
		}
	}
	return iValue;
}

void CvLeague::SetMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, int iValue)
{
#if !defined(MOD_BALANCE_CORE)
	bool bCanContribute = CanMemberContribute(ePlayer, eLeagueProject);
	CvAssertMsg(bCanContribute, "Attempting to make a contribution to a League Project when not allowed. Please send Anton your save file and version.");
	if (!bCanContribute) return;
#endif
	int iMatches = 0;
	for (ProjectList::iterator it = m_vProjects.begin(); it != m_vProjects.end(); it++)
	{
		if (it->eType == eLeagueProject)
		{
			iMatches++;
			it->vProductionList[ePlayer] = iValue;
		}
	}
	CvAssertMsg(iMatches == 1, "Unexpected case when contributing to a League Project. Please send Anton your save file and version.");
}

void CvLeague::ChangeMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, int iChange)
{
#if !defined(MOD_BALANCE_CORE)
	bool bCanContribute = CanMemberContribute(ePlayer, eLeagueProject);
	CvAssertMsg(bCanContribute, "Attempting to make a contribution to a League Project when not allowed. Please send Anton your save file and version.");
	if (!bCanContribute) return;
#endif

	SetMemberContribution(ePlayer, eLeagueProject, GetMemberContribution(ePlayer, eLeagueProject) + iChange);
}

CvLeague::ContributionTier CvLeague::GetMemberContributionTier(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject)
{
	float fContribution = (float) GetMemberContribution(ePlayer, eLeagueProject);
	ContributionTier eTier = CONTRIBUTION_TIER_0;
	if (fContribution >= GetContributionTierThreshold(CONTRIBUTION_TIER_3, eLeagueProject))
	{
		eTier = CONTRIBUTION_TIER_3;
	}
	else if (fContribution >= GetContributionTierThreshold(CONTRIBUTION_TIER_2, eLeagueProject))
	{
		eTier = CONTRIBUTION_TIER_2;
	}
	else if (fContribution >= GetContributionTierThreshold(CONTRIBUTION_TIER_1, eLeagueProject))
	{
		eTier = CONTRIBUTION_TIER_1;
	}
	return eTier;
}

// Get the contribution value which a player needs to meet or exceed to qualify for a reward tier level
float CvLeague::GetContributionTierThreshold(ContributionTier eTier, LeagueProjectTypes eLeagueProject)
{
	float fThreshold = 0.0f;
	Project* pProject = GetProject(eLeagueProject);
	CvAssertMsg(pProject, "Could not find league project. Please send Anton your save file and version.");
	if (!pProject) return 0.0f;

	switch (eTier)
	{
	case CONTRIBUTION_TIER_1:
		{
			fThreshold = GC.getLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD() * GetProjectCostPerPlayer(eLeagueProject);
			break;
		}
	case CONTRIBUTION_TIER_2:
		{
			fThreshold = GC.getLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD() * GetProjectCostPerPlayer(eLeagueProject);
			break;
		}
	case CONTRIBUTION_TIER_3:
		{
			int iBestContribution = 0;
			for (uint i = 0; i < pProject->vProductionList.size(); i++)
			{
				int iContribution = pProject->vProductionList[i];
				if (iContribution > iBestContribution && GET_PLAYER((PlayerTypes)i).isAlive() && IsMember((PlayerTypes)i))
				{
					iBestContribution = iContribution;
				}
			}
			fThreshold = MAX((float)iBestContribution, GC.getLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD() * GetProjectCostPerPlayer(eLeagueProject));
			break;
		}
	default:
		break;
	}
	return fThreshold;
}

bool CvLeague::IsTradeEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient)
{
	CvAssertMsg(eTrader >= 0 && eTrader < MAX_CIV_PLAYERS, "Invalid index for eTrader. Please send Anton your save file and version.");
	if (eTrader < 0 || eTrader >= MAX_CIV_PLAYERS) return false;
	CvAssertMsg(eRecipient >= 0 && eRecipient < MAX_CIV_PLAYERS, "Invalid index for eRecipient. Please send Anton your save file and version.");
	if (eRecipient < 0 || eRecipient >= MAX_CIV_PLAYERS) return false;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		// Trade route involving a minor civ
		if (GET_PLAYER(eTrader).isMinorCiv() || GET_PLAYER(eRecipient).isMinorCiv())
		{
			if (it->GetEffects()->bEmbargoCityStates)
			{
				return true;
			}
		}
		// Trade route between two majors
		else
		{			
			if (it->GetEffects()->bEmbargoPlayer)
			{
				PlayerTypes eEmbargoedMajor = (PlayerTypes) it->GetProposerDecision()->GetDecision();
				CvAssert(eEmbargoedMajor != NO_PLAYER);
				if (eEmbargoedMajor == eTrader || eEmbargoedMajor == eRecipient)
				{
					//does not affect teammates
					if (GET_PLAYER(eTrader).getTeam() == GET_PLAYER(eRecipient).getTeam())
						continue;

					//does not affect vassals
					if (GET_TEAM(GET_PLAYER(eTrader).getTeam()).IsVassal(GET_PLAYER(eRecipient).getTeam()) || GET_TEAM(GET_PLAYER(eRecipient).getTeam()).IsVassal(GET_PLAYER(eTrader).getTeam()))
						continue;

					return true;
				}
			}
		}
	}
	return false;
}

bool CvLeague::IsPlayerEmbargoed(PlayerTypes ePlayer)
{
	if (ePlayer < 0 || ePlayer >= MAX_CIV_PLAYERS) return false;
	bool bMinor = GET_PLAYER(ePlayer).isMinorCiv();

	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++) {
		if (bMinor) {
			if (it->GetEffects()->bEmbargoCityStates) {
				return true;
			}
		} else {			
			if (it->GetEffects()->bEmbargoPlayer && ePlayer == (PlayerTypes) it->GetProposerDecision()->GetDecision()) {
				return true;
			}
		}
	}

	return false;
}

bool CvLeague::IsLuxuryHappinessBanned(ResourceTypes eResource)
{
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->bNoResourceHappiness && it->GetProposerDecision()->GetDecision() == eResource)
		{
			return true;
		}
	}
	return false;
}

int CvLeague::GetResearchMod(TechTypes eTech)
{
	int iValue = 0;

	int iKnownByMemberMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->iMemberDiscoveredTechMod != 0)
		{
			iKnownByMemberMod += it->GetEffects()->iMemberDiscoveredTechMod;
		}
	}
	if (iKnownByMemberMod != 0)
	{
		// Does any member have this tech?
		for (uint i = 0; i < m_vMembers.size(); i++)
		{
			PlayerTypes eMember = m_vMembers[i].ePlayer;
			if (GET_TEAM(GET_PLAYER(eMember).getTeam()).GetTeamTechs()->HasTech(eTech))
			{
				iValue += iKnownByMemberMod;
				break;
			}
		}
	}

	return iValue;
}

int CvLeague::GetFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield)
{
	CvAssertMsg(eFeature != NO_FEATURE, "Looking for yield changes to NO_FEATURE. Please send Anton your save file and version.");
	int iValue = 0;

	CvFeatureInfo* pInfo = GC.getFeatureInfo(eFeature);
	if (pInfo)
	{
		// Natural Wonders
#if defined(MOD_PSEUDO_NATURAL_WONDER)
		if (pInfo->IsNaturalWonder(true))
#else
		if (pInfo->IsNaturalWonder())
#endif
		{
			int iNaturalWonderMod = 0;
			if (eYield == YIELD_CULTURE)
			{
				for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
				{
					if (it->GetEffects()->iCulturePerNaturalWonder != 0)
					{
						iNaturalWonderMod += it->GetEffects()->iCulturePerNaturalWonder;
					}
				}
			}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eYield == YIELD_FOOD)
			{
				for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
				{
					if (it->GetEffects()->iCulturePerNaturalWonder != 0)
					{
						iNaturalWonderMod += it->GetEffects()->iCulturePerNaturalWonder;
					}
				}
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eYield == YIELD_PRODUCTION)
			{
				for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
				{
					if (it->GetEffects()->iCulturePerNaturalWonder != 0)
					{
						iNaturalWonderMod += it->GetEffects()->iCulturePerNaturalWonder;
					}
				}
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eYield == YIELD_GOLD)
			{
				for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
				{
					if (it->GetEffects()->iCulturePerNaturalWonder != 0)
					{
						iNaturalWonderMod += it->GetEffects()->iCulturePerNaturalWonder;
					}
				}
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eYield == YIELD_SCIENCE)
			{
				for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
				{
					if (it->GetEffects()->iCulturePerNaturalWonder != 0)
					{
						iNaturalWonderMod += it->GetEffects()->iCulturePerNaturalWonder;
					}
				}
			}
			if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eYield == YIELD_FAITH)
			{
				for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
				{
					if (it->GetEffects()->iCulturePerNaturalWonder != 0)
					{
						iNaturalWonderMod += it->GetEffects()->iCulturePerNaturalWonder;
					}
				}
			}
#endif
			iValue += iNaturalWonderMod;
		}
	}

	return iValue;
}

int CvLeague::GetWorldWonderYieldChange(YieldTypes eYield)
{
	int iValue = 0;

	if (eYield == YIELD_CULTURE)
	{
		for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
		{
			if (it->GetEffects()->iCulturePerWonder != 0)
			{
				iValue += it->GetEffects()->iCulturePerWonder;
			}
		}
	}

	return iValue;
}

bool CvLeague::IsNoTrainingNuclearWeapons()
{
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->bNoTrainingNuclearWeapons)
		{
			return true;
		}
	}
	return false;
}

int CvLeague::GetExtraVotesForFollowingReligion(PlayerTypes ePlayer)
{
	int iVotes = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iVotesForFollowingReligion != 0)
		{
			ReligionTypes eReligion = (ReligionTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eReligion != NO_RELIGION);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
			if (GET_PLAYER(ePlayer).GetReligions()->HasReligionInMostCities(eReligion) || (GC.getGame().GetGameReligions()->GetReligionCreatedByPlayer(ePlayer) == eReligion))
#else
			if (GET_PLAYER(ePlayer).GetReligions()->HasReligionInMostCities(eReligion))
#endif
			{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
				if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, ePlayer);
					CvPlot* pkPlot = NULL;
					int iReligionAlly = 0;
					if(pReligion)
					{
						pkPlot = GC.getMap().plot(pReligion->m_iHolyCityX, pReligion->m_iHolyCityY);
						PlayerTypes eLoopPlayer;
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if((eLoopPlayer != NO_PLAYER) && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && (eLoopPlayer != ePlayer))
							{
								if (GET_PLAYER(eLoopPlayer).GetReligions()->HasReligionInMostCities(eReligion))
								{
									iReligionAlly++;
								}
							}
						}
					}
					//Did you found the religion and own the Holy City? More votes for you.
					if(pkPlot != NULL && pkPlot->getOwner() == ePlayer)
					{
						iVotes += it->GetEffects()->iVotesForFollowingReligion;
						iVotes += iReligionAlly;
						return iVotes;
					}
					//What if we share the faith, but didn't create it?
					else if(GET_PLAYER(ePlayer).GetReligions()->HasReligionInMostCities(eReligion))
					{
						iVotes += it->GetEffects()->iVotesForFollowingReligion;
						return iVotes;
					}
				}
				else
				{
#endif
					iVotes += it->GetEffects()->iVotesForFollowingReligion;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
				}
#endif

			}
		}
	}
	return iVotes;
}

int CvLeague::GetCityTourismModifier(const CvCity* pCity)
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		// Holy City tourism
		if (it->GetEffects()->iHolyCityTourism != 0)
		{
			ReligionTypes eReligion = (ReligionTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eReligion != NO_RELIGION);
			if (pCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
			{
				iMod += it->GetEffects()->iHolyCityTourism;
			}
		}
	}
	return iMod;
}

int CvLeague::GetReligionSpreadStrengthModifier(ReligionTypes eReligion)
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iReligionSpreadStrengthMod != 0)
		{
			ReligionTypes e = (ReligionTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(e != NO_RELIGION);
			if (e != NO_RELIGION && e == eReligion)
			{
				iMod += it->GetEffects()->iReligionSpreadStrengthMod;
			}
		}
	}
	return iMod;
}

int CvLeague::GetExtraVotesForFollowingIdeology(PlayerTypes ePlayer)
{
	int iVotes = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iVotesForFollowingIdeology != 0)
		{
			PolicyBranchTypes eIdeology = (PolicyBranchTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eIdeology != NO_POLICY_BRANCH_TYPE);
			PolicyBranchTypes ePlayerIdeology = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
			if (ePlayerIdeology != NO_POLICY_BRANCH_TYPE && eIdeology != NO_POLICY_BRANCH_TYPE && ePlayerIdeology == eIdeology)
			{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
				if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
					//More votes per follower
					PlayerTypes eLoopPlayer;
					int iIdeologyAlly = 0;
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;
						if (GET_PLAYER(eLoopPlayer).GetPlayerPolicies()->GetLateGamePolicyTree() == eIdeology && (eLoopPlayer != ePlayer))
						{
							iIdeologyAlly++;
						}
					}
					iVotes += it->GetEffects()->iVotesForFollowingIdeology;
					iVotes += iIdeologyAlly++;
				}
				else
#endif
					iVotes += it->GetEffects()->iVotesForFollowingIdeology;
			}
		}
	}
	return iVotes;
}

int CvLeague::GetPressureForIdeology(PolicyBranchTypes eIdeology)
{
	int iPressure = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iOtherIdeologyRebellionMod != 0)
		{
			PolicyBranchTypes e = (PolicyBranchTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(e != NO_POLICY_BRANCH_TYPE);
			if (e != NO_POLICY_BRANCH_TYPE && eIdeology != NO_POLICY_BRANCH_TYPE && e == eIdeology)
			{
				iPressure += it->GetEffects()->iOtherIdeologyRebellionMod;
			}
		}
	}
	return iPressure;
}

int CvLeague::GetArtsyGreatPersonRateModifier()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iArtsyGreatPersonRateMod != 0)
		{
			iMod += it->GetEffects()->iArtsyGreatPersonRateMod;
		}
	}
	return iMod;
}

int CvLeague::GetScienceyGreatPersonRateModifier()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iScienceyGreatPersonRateMod != 0)
		{
			iMod += it->GetEffects()->iScienceyGreatPersonRateMod;
		}
	}
	return iMod;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
int CvLeague::GetSpaceShipProductionMod()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iLimitSpaceshipProduction != 0)
		{
			iMod += it->GetEffects()->iLimitSpaceshipProduction;
		}
	}
	return iMod;
}
int CvLeague::GetSpaceShipPurchaseMod()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iLimitSpaceshipPurchase != 0)
		{
			iMod += it->GetEffects()->iLimitSpaceshipPurchase;
		}
	}
	return iMod;
}
int CvLeague::GetWorldWar()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->iIsWorldWar != 0)
		{
			iMod += it->GetEffects()->iIsWorldWar;
		}
	}
	return iMod;
}
int CvLeague::GetUnitMaintenanceMod()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->iUnitMaintenanceGoldPercent != 0)
		{
			iMod += it->GetEffects()->iUnitMaintenanceGoldPercent;
		}
	}
	return iMod;
}
bool CvLeague::IsCityStateEmbargo()
{
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->bEmbargoCityStates)
		{
			return true;
		}
	}

	return false;
}
bool CvLeague::IsIdeologyEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient)
{
	CvAssertMsg(eTrader >= 0 && eTrader < MAX_CIV_PLAYERS, "Invalid index for eTrader. Please send Anton your save file and version.");
	if (eTrader < 0 || eTrader >= MAX_CIV_PLAYERS) return false;

	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{ 
		if (it->GetEffects()->bEmbargoIdeology)
		{
			if(GET_PLAYER(eTrader).isMinorCiv() && !GET_PLAYER(eTrader).GetMinorCivAI()->IsAllies(eRecipient))
			{
				return true;
			}
			else if(GET_PLAYER(eRecipient).isMinorCiv() && !GET_PLAYER(eRecipient).GetMinorCivAI()->IsAllies(eTrader))
			{
				return true;
			}
			else
			{
				PolicyBranchTypes eOurIdeology = GET_PLAYER(eTrader).GetPlayerPolicies()->GetLateGamePolicyTree();
				PolicyBranchTypes eTheirIdeology = GET_PLAYER(eRecipient).GetPlayerPolicies()->GetLateGamePolicyTree();
				if (eOurIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE)
				{
					if(eTheirIdeology != eOurIdeology)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
#endif
#if defined(MOD_BALANCE_CORE)
int CvLeague::GetTourismMod()
{
	int iMod = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetEffects()->iChangeTourism != 0)
		{
			iMod += it->GetEffects()->iChangeTourism;
		}
	}
	return iMod;
}
void CvLeague::DoEnactResolutionPublic(CvEnactProposal* pProposal)
{
	DoEnactResolution(pProposal);
}
void CvLeague::DoRepealResolutionPublic(CvRepealProposal* pProposal)
{
	DoRepealResolution(pProposal);
}
#endif
CvString CvLeague::GetResolutionName(ResolutionTypes eResolution, int iResolutionID, int iProposerChoice, bool bIncludePrefix)
{
	CvString s = "";

	CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
	CvAssertMsg(pInfo, "Resolution info not found when assembling name text. Please send Anton your save file and version.");
	if (!pInfo)
	{
		CvAssert(false);
		return "";
	}

	CvString sPrefix = "";
	// An existing resolution
	if (iResolutionID != -1)
	{
		// Propose Enact
		if (IsProposed(iResolutionID, /*bRepeal*/false))
		{
			sPrefix += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_ENACT").toUTF8();
		}
		// Propose Repeal
		else if (IsProposed(iResolutionID, /*bRepeal*/true))
		{
			sPrefix += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_REPEAL").toUTF8();
		}
		// Active
		else if (GetActiveResolution(iResolutionID) != NULL)
		{
		}
	}
	// A new resolution
	else
	{
		// Inactive
	}

	CvString sSuffix = "";
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if(MOD_DIPLOMACY_CITYSTATES && iResolutionID != -1)
	{
		bool bDone = false;
		for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
		{
			if (it->GetID() == iResolutionID && it->GetEffects()->bSphereOfInfluence)
			{
				if(it->GetProposerDecision()->GetProposer() != NO_PLAYER && it->GetProposerDecision()->GetType() == RESOLUTION_DECISION_CITY_CSD)
				{
					PlayerTypes ePlayer = it->GetProposerDecision()->GetProposer();
					Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_PROPOSER");
					sTemp << GET_PLAYER(ePlayer).getCivilizationAdjectiveKey();
					s += sTemp.toUTF8();
					s += " ";
					bDone = true;
					break;
				}
			}
		}
		if(!bDone)
		{
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				if (it->GetID() == iResolutionID && it->GetEffects()->bSphereOfInfluence)
				{
					if(it->GetProposerDecision()->GetProposer() != NO_PLAYER && it->GetProposerDecision()->GetType() == RESOLUTION_DECISION_CITY_CSD)
					{
						PlayerTypes ePlayer = it->GetProposerDecision()->GetProposer();
						Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_PROPOSER");
						sTemp << GET_PLAYER(ePlayer).getCivilizationAdjectiveKey();
						s += sTemp.toUTF8();
						s += " ";
						bDone = true;
						break;
					}
				}
			}
		}
		else if(!bDone)
		{
			for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
			{
				if (it->GetID() == iResolutionID && it->GetEffects()->bSphereOfInfluence)
				{
					if(it->GetProposerDecision()->GetProposer() != NO_PLAYER && it->GetProposerDecision()->GetType() == RESOLUTION_DECISION_CITY_CSD)
					{
						PlayerTypes ePlayer = it->GetProposerDecision()->GetProposer();
						Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PREFIX_PROPOSER");
						sTemp << GET_PLAYER(ePlayer).getCivilizationAdjectiveKey();
						s += sTemp.toUTF8();
						s += " ";
						break;
					}
				}
			}
		}
	}
#endif
	if (iProposerChoice != LeagueHelpers::CHOICE_NONE)
	{
		sSuffix += ": ";
		sSuffix += LeagueHelpers::GetTextForChoice(pInfo->GetProposerDecision(), iProposerChoice);
	}

	if (bIncludePrefix)
	{
		s = sPrefix + s;
	}
	s += Localization::Lookup(pInfo->GetDescriptionKey()).toUTF8();
	s += sSuffix;

	return s;
}

CvString CvLeague::GetResolutionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iResolutionID, int iProposerChoice)
{
	CvString s = "";
	
	CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
	CvAssertMsg(pInfo, "Resolution info not found when assembling details text. Please send Anton your save file and version.");
	if (!pInfo)
	{
		CvAssert(false);
		return "";
	}

	// Resolution type info
	s += Localization::Lookup(pInfo->GetHelp()).toUTF8();

	// Refers to an existing resolution
	if (iResolutionID != -1)
	{
		CvString sDisabled = "";

		// Would vote to enact or repeal
		if (IsProposed(iResolutionID, /*bRepeal*/false) || IsProposed(iResolutionID, /*bRepeal*/true))
		{
			s += GetResolutionVoteOpinionDetails(eResolution, eObserver, iResolutionID);
		}
		// It's on hold
		else if (IsProposed(iResolutionID, /*bRepeal*/false, /*bCheckOnHold*/true) || IsProposed(iResolutionID, /*bRepeal*/true, /*bCheckOnHold*/true))
		{			
		}
		// Would propose repeal
		else if (CanProposeRepeal(iResolutionID, eObserver, &sDisabled))
		{
			s += GetResolutionProposeOpinionDetails(iResolutionID, eObserver);
		}
		else
		{
			s += sDisabled;
		}
	}
	// A new resolution
	else
	{
		// Check our choice
		CvString sDisabled = "";
		if (CanProposeEnact(eResolution, eObserver, iProposerChoice, &sDisabled))
		{
			s += GetResolutionProposeOpinionDetails(eResolution, eObserver, iProposerChoice);
		}
		else
		{
			s += sDisabled;
		}

		// Additional tooltip if there are no valid choices
		if (strcmp(sDisabled, "") == 0)
		{
			if (pInfo->GetProposerDecision() != RESOLUTION_DECISION_NONE && !CanProposeEnactAnyChoice(eResolution, eObserver))
			{
				s += "[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]";
				s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_INVALID_RESOLUTION_NO_CHOICES").toUTF8();
				s += "[ENDCOLOR]";
			}
		}
	}

	return s;
}

CvString CvLeague::GetResolutionVoteOpinionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iResolutionID)
{
	CvResolutionEntry* pInfo = GC.getResolutionInfo(eResolution);
	if (!pInfo)
	{
		return "";
	}
	// Must be proposed as either enact or repeal, but not both
	if (!IsProposed(iResolutionID, /*bRepeal*/false) && !IsProposed(iResolutionID, /*bRepeal*/true))
	{
		CvAssert(false);
		return "";
	}
	if (IsProposed(iResolutionID, /*bRepeal*/false) && IsProposed(iResolutionID, /*bRepeal*/true))
	{
		CvAssert(false);
		return "";
	}
	
	CvString s = "";
	s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_VOTE_OPINIONS").toUTF8();

	bool bEnact = IsProposed(iResolutionID, /*bRepeal*/false);
	ResolutionDecisionTypes eDecision = pInfo->GetVoterDecision();
	if (!bEnact)
	{
		eDecision = RESOLUTION_DECISION_REPEAL;
	}
	vector<int> vChoices = GetChoicesForDecision(eDecision, NO_PLAYER);

	// Discover what choices we can of the other players
	vector<pair<PlayerTypes, int>> vMemberOpinions;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (it->ePlayer != eObserver && CanEverVote(it->ePlayer))
		{
			int iMemberChoice = LeagueHelpers::CHOICE_NONE;
			if (bEnact)
			{
				iMemberChoice = GET_PLAYER(it->ePlayer).GetLeagueAI()->EvaluateVoteForOtherPlayerKnowledge(this, eObserver, GetEnactProposal(iResolutionID));
			}
			else
			{
				iMemberChoice = GET_PLAYER(it->ePlayer).GetLeagueAI()->EvaluateVoteForOtherPlayerKnowledge(this, eObserver, GetRepealProposal(iResolutionID));
			}
			std::pair<PlayerTypes, int> pr = std::pair<PlayerTypes, int>(it->ePlayer, iMemberChoice);
			vMemberOpinions.push_back(pr);
		}
	}

	vector<LeagueHelpers::VoteOpinionIntrigueElement> vChoiceCommitments;
	vector<LeagueHelpers::VoteOpinionIntrigueElement> vChoiceLeanings;
	for (uint iChoiceIndex = 0; iChoiceIndex < vChoices.size(); iChoiceIndex++)
	{
		int iChoice = vChoices[iChoiceIndex];
		int iNumCivsCommitted = 0;
		int iNumCivsLeaning = 0;
		int iNumDelegatesCommitted = 0;
		int iNumDelegatesLeaning = 0;

		for (uint iMemberIndex = 0; iMemberIndex < vMemberOpinions.size(); iMemberIndex++)
		{
			PlayerTypes eMember = vMemberOpinions[iMemberIndex].first;
			int iMemberChoice = vMemberOpinions[iMemberIndex].second;
			int iMemberDelegates = CalculateStartingVotesForMember(eMember);

			// Vote Commitment to us
			int iCommitted = GET_PLAYER(eMember).GetLeagueAI()->GetVoteCommitment(eObserver, iResolutionID, iChoice, bEnact); //here
			if (iCommitted > 0)
			{
				iNumCivsCommitted++;
				iNumDelegatesCommitted += iCommitted;
				iMemberDelegates -= iCommitted;
				CvAssert(iMemberDelegates >= 0);
			}

			// Known leaning
			if (iMemberChoice == iChoice)
			{
				if (iMemberDelegates > 0)
				{
					iNumCivsLeaning++;
					iNumDelegatesLeaning += iMemberDelegates;
				}
			}
		}

		if (iNumCivsCommitted > 0)
		{
			LeagueHelpers::VoteOpinionIntrigueElement temp;
			temp.iChoice = iChoice;
			temp.iNumCivs = iNumCivsCommitted;
			temp.iNumDelegates = iNumDelegatesCommitted;
			vChoiceCommitments.push_back(temp);
		}

		if (iNumCivsLeaning > 0)
		{
			LeagueHelpers::VoteOpinionIntrigueElement temp;
			temp.iChoice = iChoice;
			temp.iNumCivs = iNumCivsLeaning;
			temp.iNumDelegates = iNumDelegatesLeaning;
			vChoiceLeanings.push_back(temp);
		}
	}

	// Votes committed to us (show all of them - don't address if a player makes ones for different choices since that's weird)
	if (!vChoiceCommitments.empty())
	{
		std::stable_sort(vChoiceCommitments.begin(), vChoiceCommitments.end(), LeagueHelpers::VoteOpinionIntrigueSorter());
		for (uint i = 0; i < vChoiceCommitments.size(); i++)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_VOTE_OPINIONS_COMMITMENTS");
			sTemp << vChoiceCommitments[i].iNumDelegates << vChoiceCommitments[i].iNumCivs << LeagueHelpers::GetTextForChoice(eDecision, vChoiceCommitments[i].iChoice);
			s += sTemp.toUTF8();
		}
	}

	// Known leanings (but only show the top ones, lest the tooltip get cut off)
	int iNumLeaningsShown = 0;
	int iNumLeaningsToShow = 11;
	if (!vChoiceLeanings.empty())
	{
		std::stable_sort(vChoiceLeanings.begin(), vChoiceLeanings.end(), LeagueHelpers::VoteOpinionIntrigueSorter());
		for (uint i = 0; i < vChoiceLeanings.size(); i++)
		{
			if (iNumLeaningsShown <= iNumLeaningsToShow)
			{
				iNumLeaningsShown++;
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_VOTE_OPINIONS_CHOICE");
				sTemp << vChoiceLeanings[i].iNumDelegates << vChoiceLeanings[i].iNumCivs << LeagueHelpers::GetTextForChoice(eDecision, vChoiceLeanings[i].iChoice);
				s += sTemp.toUTF8();
			}
			else
			{
				break;
			}
		}
	}

	// Unknown leanings
	int iNumCivsUnaccounted = 0;
	int iNumDelegatesUnaccounted = 0;
	for (uint iMemberIndex = 0; iMemberIndex < vMemberOpinions.size(); iMemberIndex++)
	{
		PlayerTypes eMember = vMemberOpinions[iMemberIndex].first;
		int iMemberChoice = vMemberOpinions[iMemberIndex].second;
		int iMemberDelegates = CalculateStartingVotesForMember(eMember);

		// Vote Commitment to us
		for (uint i = 0; i < vChoices.size(); i++)
		{
			int iCommitted = GET_PLAYER(eMember).GetLeagueAI()->GetVoteCommitment(eObserver, iResolutionID, vChoices[i], bEnact);
			if (iCommitted > 0)
			{
				iMemberDelegates -= iCommitted;
				CvAssert(iMemberDelegates >= 0);
			}
		}

		// Unknown leaning
		if (iMemberChoice == LeagueHelpers::CHOICE_NONE)
		{
			if (iMemberDelegates > 0)
			{
				iNumCivsUnaccounted++;
				iNumDelegatesUnaccounted += iMemberDelegates;
			}
		}
	}
	if (iNumCivsUnaccounted > 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_VOTE_OPINIONS_UNACCOUNTED");
		sTemp << iNumDelegatesUnaccounted << iNumCivsUnaccounted;
		s += sTemp.toUTF8();
	}

	// Our delegation
	int iOurDelegates = CalculateStartingVotesForMember(eObserver);
	if (iOurDelegates > 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_VOTE_OPINIONS_OURS");
		sTemp << iOurDelegates;
		s += sTemp.toUTF8();
	}

	return s;
}

CvString CvLeague::GetResolutionProposeOpinionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iProposerChoice)
{
	CvString s = "";
	bool bShowAllValues = GC.getGame().IsShowAllOpinionValues();
	std::vector<pair<int, PlayerTypes>> vScores;


	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		if (CanEverVote(it->ePlayer))
		{
			int iScore = GET_PLAYER(it->ePlayer).GetLeagueAI()->ScoreProposal(this, eResolution, iProposerChoice, eObserver);
			vScores.push_back(std::make_pair(iScore, it->ePlayer));
		}
	}
	std::sort(vScores.begin(), vScores.end());
	std::reverse(vScores.begin(), vScores.end());
	bool bPositiveDone = false;
	bool bNegativeDone = false;
	bool bNeutralDone = false;

	for (std::vector<pair<int, PlayerTypes>>::iterator it = vScores.begin(); it != vScores.end(); it++)
	{
		CvString strOut = "";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINION");
		sTemp << GET_PLAYER(it->second).getCivilizationShortDescriptionKey();
		strOut += sTemp.toUTF8();

		if (bShowAllValues)
		{
			CvString strTemp;
			strTemp.Format(" (%d)", it->first);
			strOut += strTemp;
		}

		CvLeagueAI::DesireLevels eDesire = GET_PLAYER(it->second).GetLeagueAI()->EvaluateDesire(it->first);
		if (eDesire > CvLeagueAI::DESIRE_WEAK_LIKE)
		{
			if (!bPositiveDone)
			{
				s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_POSITIVE").toUTF8();
				bPositiveDone = true;
			}
			strOut.insert(0, "[COLOR_POSITIVE_TEXT]");
			s += strOut;
		}
		else if (eDesire > CvLeagueAI::DESIRE_NEUTRAL)
		{
			if (!bPositiveDone)
			{
				s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_POSITIVE").toUTF8();
				bPositiveDone = true;
			}
			strOut.insert(0, "[COLOR_FADING_POSITIVE_TEXT]");
			s += strOut;
		}
		else if (eDesire > CvLeagueAI::DESIRE_WEAK_DISLIKE)
		{
			if (bShowAllValues)
			{
				if (!bNeutralDone)
				{
					CvString sNeg = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEUTRAL").toUTF8();
					sNeg.insert(0, "[ENDCOLOR]");
					s += sNeg;
					bNeutralDone = true;
				}
				s += strOut;
			}
		}
		else if (eDesire > CvLeagueAI::DESIRE_DISLIKE)
		{
			if (!bNegativeDone)
			{
				CvString sNeg = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEGATIVE").toUTF8();
				sNeg.insert(0, "[ENDCOLOR]");
				s += sNeg;
				bNegativeDone = true;
			}
			strOut.insert(0, "[COLOR_FADING_NEGATIVE_TEXT]");
			s += strOut;
		}
		else
		{
			if (!bNegativeDone)
			{
				CvString sNeg = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEGATIVE").toUTF8();
				sNeg.insert(0, "[ENDCOLOR]");
				s += sNeg;
				bNegativeDone = true;
			}
			strOut.insert(0, "[COLOR_NEGATIVE_TEXT]");
			s += strOut;
		}
	}
	CvString sEnd = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_END").toUTF8();
	sEnd.insert(0, "[COLOR_WHITE]");
	s += sEnd;

	return s;
}

CvString CvLeague::GetResolutionProposeOpinionDetails(int iTargetResolutionID, PlayerTypes eObserver)
{
	CvString s = "";
	bool bShowAllValues = GC.getGame().IsShowAllOpinionValues();
	std::vector<pair<int, PlayerTypes>> vScores;

	ActiveResolutionList vActiveResolutions = this->GetActiveResolutions();
	for (ActiveResolutionList::iterator itRes = vActiveResolutions.begin(); itRes != vActiveResolutions.end(); ++itRes)
	{
		if (itRes->GetID() == iTargetResolutionID)
		{
			for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
			{
				if (CanEverVote(it->ePlayer))
				{
					int iScore = GET_PLAYER(it->ePlayer).GetLeagueAI()->ScoreProposal(this, &(*itRes), eObserver);
					vScores.push_back(std::make_pair(iScore, it->ePlayer));
				}
			}
		}
	}
	std::sort(vScores.begin(), vScores.end());
	std::reverse(vScores.begin(), vScores.end());
	bool bPositiveDone = false;
	bool bNegativeDone = false;
	bool bNeutralDone = false;

	for (std::vector<pair<int, PlayerTypes>>::iterator it = vScores.begin(); it != vScores.end(); it++)
	{
		CvString strOut = "";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINION");
		sTemp << GET_PLAYER(it->second).getCivilizationShortDescriptionKey();
		strOut += sTemp.toUTF8();

		if (bShowAllValues)
		{
			CvString strTemp;
			strTemp.Format(" (%d)", it->first);
			strOut += strTemp;
		}

		CvLeagueAI::DesireLevels eDesire = GET_PLAYER(it->second).GetLeagueAI()->EvaluateDesire(it->first);
		if (eDesire > CvLeagueAI::DESIRE_WEAK_LIKE)
		{
			if (!bPositiveDone)
			{
				s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_POSITIVE").toUTF8();
				bPositiveDone = true;
			}
			strOut.insert(0, "[COLOR_POSITIVE_TEXT]");
			s += strOut;
		}
		else if (eDesire > CvLeagueAI::DESIRE_NEUTRAL)
		{
			if (!bPositiveDone)
			{
				s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_POSITIVE").toUTF8();
				bPositiveDone = true;
			}
			strOut.insert(0, "[COLOR_FADING_POSITIVE_TEXT]");
			s += strOut;
		}
		else if (eDesire > CvLeagueAI::DESIRE_WEAK_DISLIKE)
		{
			if (bShowAllValues)
			{
				if (!bNeutralDone)
				{
					CvString sNeg = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEUTRAL").toUTF8();
					sNeg.insert(0, "[ENDCOLOR]");
					s += sNeg;
					bNeutralDone = true;
				}
				s += strOut;
			}
		}
		else if (eDesire > CvLeagueAI::DESIRE_DISLIKE)
		{
			if (!bNegativeDone)
			{
				CvString sNeg = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEGATIVE").toUTF8();
				sNeg.insert(0, "[ENDCOLOR]");
				s += sNeg;
				bNegativeDone = true;
			}
			strOut.insert(0, "[COLOR_FADING_NEGATIVE_TEXT]");
			s += strOut;
		}
		else
		{
			if (!bNegativeDone)
			{
				CvString sNeg = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_NEGATIVE").toUTF8();
				sNeg.insert(0, "[ENDCOLOR]");
				s += sNeg;
				bNegativeDone = true;
			}
			strOut.insert(0, "[COLOR_NEGATIVE_TEXT]");
			s += strOut;
		}
	}
	CvString sEnd = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_PROPOSAL_OPINIONS_END").toUTF8();
	sEnd.insert(0, "[COLOR_WHITE]");
	s += sEnd;

	return s;
}

CvString CvLeague::GetMemberDetails(PlayerTypes eMember, PlayerTypes eObserver)
{
	if (!IsMember(eMember) || (!IsMember(eObserver) && !GET_PLAYER(eObserver).isObserver()))
	{
		CvAssertMsg(false, "Attempting to get detail string for a player that is not a league member. Please send Anton your save file and version.");
		return "";
	}
	CvString s = "";

	// Delegation info
	s += GetMemberDelegationDetails(eMember, eObserver);
	
	// Vote leanings and commitments
	s += GetMemberKnowledgeDetails(eMember, eObserver);
	s += GetMemberVoteOpinionDetails(eMember, eObserver);

	return s;
}

CvString CvLeague::GetMemberDelegationDetails(PlayerTypes eMember, PlayerTypes eObserver)
{
	if (!IsMember(eMember) || (!IsMember(eObserver) && !GET_PLAYER(eObserver).isObserver()))
	{
		CvAssertMsg(false, "Attempting to get detail string for a player that is not a league member. Please send Anton your save file and version.");
		return "";
	}
	Member* pMember = GetMember(eMember);
	if (pMember == NULL)
	{
		CvAssertMsg(false, "Attempting to get detail string for a player that is has no member data. Please send Anton your save file and version.");
		return "";
	}

	CvString s = "";

	int iVotes = CalculateStartingVotesForMember(eMember); // Updates vote sources string
	if (IsInSession())
	{
		iVotes = GetRemainingVotesForMember(eMember) + GetSpentVotesForMember(eMember);
	}
	CvPlayerAI& kMember = GET_PLAYER(eMember);
	Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS");
	sTemp << kMember.getNameKey();
	sTemp << kMember.getCivilizationShortDescriptionKey();
	sTemp << iVotes;
	s += sTemp.toUTF8();
	s += pMember->sVoteSources;

	return s;
}

CvString CvLeague::GetMemberKnowledgeDetails(PlayerTypes eMember, PlayerTypes eObserver)
{
	if (!IsMember(eMember) || (!IsMember(eObserver) && !GET_PLAYER(eObserver).isObserver()))
	{
		CvAssertMsg(false, "Attempting to get detail string for a player that is not a league member. Please send Anton your save file and version.");
		return "";
	}

	CvString s = "";

	if (eMember != eObserver)
	{
		GET_PLAYER(eMember).GetLeagueAI()->GetKnowledgeGivenToOtherPlayer(eObserver, &s);
	}

	return s;
}

CvString CvLeague::GetMemberVoteOpinionDetails(PlayerTypes eMember, PlayerTypes eObserver)
{
	if (!IsMember(eMember) || !IsMember(eObserver))
	{
		CvAssertMsg(false, "Attempting to get detail string for a player that is not a league member. Please send Anton your save file and version.");
		return "";
	}

	CvString s = "";
	if (!GetEnactProposals().empty() || !GetRepealProposals().empty())
	{
		// Vote Leanings
		if (eMember != eObserver)
		{
			s += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTES").toUTF8();
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				GET_PLAYER(eMember).GetLeagueAI()->EvaluateVoteForOtherPlayerKnowledge(this, eObserver, &(*it), &s);
			}
			for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
			{
				GET_PLAYER(eMember).GetLeagueAI()->EvaluateVoteForOtherPlayerKnowledge(this, eObserver, &(*it), &s);
			}
		}
		
		// Vote Commitments
		if (GET_PLAYER(eMember).GetLeagueAI()->HasVoteCommitment())
		{
			for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
			{
				std::vector<int> vChoices = GetChoicesForDecision(it->GetVoterDecision()->GetType(), NO_PLAYER);
				for (uint i = 0; i < vChoices.size(); i++)
				{
					int iNumVotes = 0;
					if (eMember != eObserver)
					{
						iNumVotes += GET_PLAYER(eMember).GetLeagueAI()->GetVoteCommitment(eObserver, it->GetID(), vChoices[i], /*bEnact*/ true);
					}
					else
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							iNumVotes += GET_PLAYER(eMember).GetLeagueAI()->GetVoteCommitment((PlayerTypes)iPlayerLoop, it->GetID(), vChoices[i], /*bEnact*/ true);
						}
					}
					if (iNumVotes > 0)
					{
						Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_COMMITMENT");
						sTemp << it->GetProposalName() << LeagueHelpers::GetTextForChoice(it->GetVoterDecision()->GetType(), vChoices[i]) << iNumVotes;
						s += sTemp.toUTF8();
					}
				}
			}
			for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
			{
				std::vector<int> vChoices = GetChoicesForDecision(it->GetRepealDecision()->GetType(), NO_PLAYER);
				for (uint i = 0; i < vChoices.size(); i++)
				{
					int iNumVotes = 0;
					if (eMember != eObserver)
					{
						iNumVotes += GET_PLAYER(eMember).GetLeagueAI()->GetVoteCommitment(eObserver, it->GetID(), vChoices[i], /*bEnact*/ false);
					}
					else
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							iNumVotes += GET_PLAYER(eMember).GetLeagueAI()->GetVoteCommitment((PlayerTypes)iPlayerLoop, it->GetID(), vChoices[i], /*bEnact*/ false);
						}
					}
					if (iNumVotes > 0)
					{
						Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_COMMITMENT");
						sTemp << it->GetProposalName() << LeagueHelpers::GetTextForChoice(it->GetRepealDecision()->GetType(), vChoices[i]) << iNumVotes;
						s += sTemp.toUTF8();
					}
				}
			}
		}
	}

	return s;
}

CvString CvLeague::GetProjectDetails(LeagueProjectTypes eProject, PlayerTypes eObserver)
{
	CvString s = "";
	CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eProject);
	if (!IsProjectActive(eProject) && !IsProjectComplete(eProject))
	{
		return "";
	}
	if (!pInfo)
	{
		return "";
	}

	// Progress text
	s += GetProjectProgressDetails(eProject, eObserver);

	// Rewards text
	s += "[NEWLINE][NEWLINE]";
	s += GetProjectRewardDetails(eProject, eObserver);

	return s;
}

CvString CvLeague::GetProjectProgressDetails(LeagueProjectTypes eProject, PlayerTypes eObserver)
{
	CvString s = "";
	if (!IsProjectActive(eProject) && !IsProjectComplete(eProject))
	{
		return "";
	}

	// Total cost
	if (eObserver != NO_PLAYER && IsProjectActive(eProject) && GetProjectCost(eProject)>0)
	{
		int iPercentCompleted = (int) (((float)GetProjectProgress(eProject) / (float)GetProjectCost(eProject)) * 100);
		iPercentCompleted = MIN(100, iPercentCompleted);
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_PROJECT_POPUP_PROGRESS_COST");
		sTemp << iPercentCompleted;
		sTemp << GetMemberContribution(eObserver, eProject) / 100;
		s += sTemp.toUTF8();
	}
	else
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_PROJECT_POPUP_TOTAL_COST");
		sTemp << GetProjectCost(eProject) / 100;
		s += sTemp.toUTF8();
	}

	return s;
}

CvString CvLeague::GetProjectRewardDetails(LeagueProjectTypes eProject, PlayerTypes eObserver)
{
	CvString s = "";
	CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eProject);
	if (!IsProjectActive(eProject) && !IsProjectComplete(eProject))
	{
		return "";
	}
	if (!pInfo)
	{
		return "";
	}
	
	s += GetProjectRewardTierDetails(3, eProject, eObserver);
	s += "[NEWLINE][NEWLINE]";
	s += GetProjectRewardTierDetails(2, eProject, eObserver);
	s += "[NEWLINE][NEWLINE]";
	s += GetProjectRewardTierDetails(1, eProject, eObserver);

	return s;
}

CvString CvLeague::GetProjectRewardTierDetails(int iTier, LeagueProjectTypes eProject, PlayerTypes /*eObserver*/)
{
	CvString s = "";
	CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eProject);
	if (!IsProjectActive(eProject) && !IsProjectComplete(eProject))
	{
		return "";
	}
	if (!pInfo)
	{
		return "";
	}

	CvLeagueProjectRewardEntry* pRewardInfo = NULL;
	CvString sRewardIcon = "";
	CvString sContribution = "";
	if (iTier == 3)
	{
		pRewardInfo = GC.getLeagueProjectRewardInfo(pInfo->GetRewardTier3());
		sRewardIcon = "[ICON_TROPHY_GOLD]";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_PROJECT_REWARD_TIER_3");
		sContribution = sTemp.toUTF8();
	}
	else if (iTier == 2)
	{
		pRewardInfo = GC.getLeagueProjectRewardInfo(pInfo->GetRewardTier2());
		sRewardIcon = "[ICON_TROPHY_SILVER]";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_PROJECT_REWARD_TIER_2");
		sTemp << GetContributionTierThreshold(CONTRIBUTION_TIER_2, eProject) / 100;
		sContribution = sTemp.toUTF8();
	}
	else if (iTier == 1)
	{
		pRewardInfo = GC.getLeagueProjectRewardInfo(pInfo->GetRewardTier1());
		sRewardIcon = "[ICON_TROPHY_BRONZE]";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_PROJECT_REWARD_TIER_1");
		sTemp << GetContributionTierThreshold(CONTRIBUTION_TIER_1, eProject) / 100;
		sContribution = sTemp.toUTF8();
	}

	CvAssert(pRewardInfo);
	if (pRewardInfo != NULL)
	{
		Localization::String sResult = Localization::Lookup("TXT_KEY_LEAGUE_PROJECT_REWARD_DESC");
		sResult << sRewardIcon << pRewardInfo->GetDescriptionKey() << sContribution << pRewardInfo->GetHelp();
		s = sResult.toUTF8();
	}

	return s;
}

std::vector<CvString> CvLeague::GetCurrentEffectsSummary(PlayerTypes /*eObserver*/)
{
	std::vector<CvString> vsEffects;

	// League status
	if (HasHostMember())
	{
		PlayerTypes eHost = GetHostMember();
		if (eHost != NO_PLAYER)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_HOST");
			sTemp << GET_PLAYER(eHost).getCivilizationShortDescriptionKey();
			vsEffects.push_back(sTemp.toUTF8());
		}
	}

	// Active projects
	int iProjects = 0;
	CvString sProjects = "";
	for (ProjectList::const_iterator it = m_vProjects.begin(); it != m_vProjects.end(); ++it)
	{
		if (IsProjectActive(it->eType) && !IsProjectComplete(it->eType))
		{
			CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(it->eType);
			if (pInfo)
			{
				if (iProjects != 0)
				{
					sProjects += ", ";
				}
				iProjects++;
				sProjects += pInfo->GetDescription();
			}
		}
	}
	if (iProjects > 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_ACTIVE_PROJECTS");
		sTemp << sProjects;
		vsEffects.push_back(sTemp.toUTF8());
	}

	// Ongoing resolution effects
	ReligionTypes eWorldReligion = NO_RELIGION;
	PolicyBranchTypes eWorldIdeology = NO_POLICY_BRANCH_TYPE;
	vector<PlayerTypes> veEmbargoedPlayers;
	vector<ResourceTypes> veBannedResources;
	CvResolutionEffects effects;
#if defined(MOD_DIPLOMACY_CITYSTATES)
	vector<PlayerTypes> veOpenMinors;
	vector<PlayerTypes> vePermanentMinors;
#endif
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		effects.AddOngoingEffects(it->GetEffects());

		if (it->GetEffects()->iResourceQuantity != 0)
		{
			//antonjs: todo: for modders
		}

		if (it->GetEffects()->bEmbargoPlayer)
		{
			PlayerTypes eEmbargoedPlayer = (PlayerTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eEmbargoedPlayer != NO_PLAYER);
			veEmbargoedPlayers.push_back(eEmbargoedPlayer);
		}

		if (it->GetEffects()->bNoResourceHappiness)
		{
			ResourceTypes eBannedResource = (ResourceTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eBannedResource != NO_RESOURCE);
			veBannedResources.push_back(eBannedResource);
		}

		if (it->GetEffects()->iVotesForFollowingReligion != 0)
		{
			ReligionTypes eReligion = (ReligionTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eReligion != NO_RELIGION);
			CvAssert(eWorldReligion == NO_RELIGION);
			eWorldReligion = eReligion;
		}

		if (it->GetEffects()->iVotesForFollowingIdeology != 0)
		{
			PolicyBranchTypes eIdeology = (PolicyBranchTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eIdeology != NO_POLICY_BRANCH_TYPE);
			CvAssert(eWorldIdeology == NO_POLICY_BRANCH_TYPE);
			eWorldIdeology = eIdeology;
		}
#if defined(MOD_DIPLOMACY_CITYSTATES)
		if (MOD_DIPLOMACY_CITYSTATES && it->GetEffects()->bOpenDoor)
		{
			PlayerTypes eOpenMinor = (PlayerTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eOpenMinor != NO_PLAYER);
			veOpenMinors.push_back(eOpenMinor);
		}
		if (MOD_DIPLOMACY_CITYSTATES && it->GetEffects()->bSphereOfInfluence)
		{
			PlayerTypes eSphereMinor = (PlayerTypes) it->GetProposerDecision()->GetDecision();
			CvAssert(eSphereMinor != NO_PLAYER);
			vePermanentMinors.push_back(eSphereMinor);
		}
#endif
	}
	if (eWorldReligion != NO_RELIGION)
	{
		CvReligionEntry* pInfo = GC.getReligionInfo(eWorldReligion);
		CvAssert(pInfo != NULL);
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eWorldReligion, NO_PLAYER);
		CvAssert(pReligion != NULL);
		if (pInfo != NULL && pReligion != NULL)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_WORLD_RELIGION");
			sTemp << pInfo->GetIconString() << pReligion->GetName();
			vsEffects.push_back(sTemp.toUTF8());
		}
	}
	if (eWorldIdeology != NO_POLICY_BRANCH_TYPE)
	{
		CvPolicyBranchEntry* pInfo = GC.getPolicyBranchInfo(eWorldIdeology);
		CvAssert(pInfo != NULL);
		if (pInfo != NULL)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_WORLD_IDEOLOGY");
			sTemp << pInfo->GetDescriptionKey();
			vsEffects.push_back(sTemp.toUTF8());
		}
	}
	if (effects.iGoldPerTurn != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GPT");
		sTemp << effects.iGoldPerTurn;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iResourceQuantity != 0)
	{
		//antonjs: todo: for modders
	}
	if (effects.bEmbargoCityStates)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_CITY_STATES");
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.bEmbargoPlayer)
	{
		CvAssert(!veEmbargoedPlayers.empty());
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_PLAYERS");
		CvString sEntries = "";
		for (uint i = 0; i < veEmbargoedPlayers.size(); i++)
		{
			Localization::String sTempEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_PLAYERS_ENTRY");
			CvAssert(veEmbargoedPlayers[i] != NO_PLAYER);
			if (veEmbargoedPlayers[i] != NO_PLAYER)
			{
				if (i != 0)
				{
					sEntries += ", ";
				}
				sTempEntry << GET_PLAYER(veEmbargoedPlayers[i]).getCivilizationShortDescriptionKey();
				sEntries += sTempEntry.toUTF8();
			}
		}
		sTemp << sEntries;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.bNoResourceHappiness)
	{
		CvAssert(!veBannedResources.empty());
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NO_RESOURCE_HAPPINESS");
		CvString sEntries = "";
		for (uint i = 0; i < veBannedResources.size(); i++)
		{
			Localization::String sTempEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NO_RESOURCE_HAPPINESS_ENTRY");
			CvResourceInfo* pInfo = GC.getResourceInfo(veBannedResources[i]);
			CvAssert(pInfo);
			if (pInfo)
			{
				if (i != 0)
				{
					sEntries += ", ";
				}
				sTempEntry << pInfo->GetDescriptionKey();
				sEntries += pInfo->GetIconString();
				sEntries += sTempEntry.toUTF8();
			}
		}
		sTemp << sEntries;
		vsEffects.push_back(sTemp.toUTF8());
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.bOpenDoor)
	{
		CvAssert(!veOpenMinors.empty());
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_OPEN_DOOR");
		CvString sEntries = "";
		for (uint i = 0; i < veOpenMinors.size(); i++)
		{
			Localization::String sTempEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_OPEN_DOOR_ENTRY");
			CvAssert(veOpenMinors[i] != NO_PLAYER);
			if (veOpenMinors[i] != NO_PLAYER)
			{
				if (i != 0)
				{
					sEntries += ", ";
				}
				sTempEntry << GET_PLAYER(veOpenMinors[i]).getCivilizationShortDescriptionKey();
				sEntries += sTempEntry.toUTF8();
			}
		}
		sTemp << sEntries;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.bSphereOfInfluence)
	{
		CvAssert(!vePermanentMinors.empty());
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_SOI");
		CvString sEntries = "";
		for (uint i = 0; i < vePermanentMinors.size(); i++)
		{
			Localization::String sTempEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_SOI_ENTRY");
			CvAssert(vePermanentMinors[i] != NO_PLAYER);
			if (vePermanentMinors[i] != NO_PLAYER)
			{
				PlayerTypes eAlly = GET_PLAYER(vePermanentMinors[i]).GetMinorCivAI()->GetPermanentAlly();
				if(eAlly != NO_PLAYER)
				{
					if (i != 0)
					{
						sEntries += ", ";
					}
					sTempEntry << GET_PLAYER(vePermanentMinors[i]).getCivilizationShortDescriptionKey() << GET_PLAYER(eAlly).getCivilizationShortDescriptionKey();
					sEntries += sTempEntry.toUTF8();
				}
			}
		}
		sTemp << sEntries;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.bEmbargoIdeology)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_IDEOLOGY");
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.iLimitSpaceshipProduction != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_SPACESHIP_LIMIT");
		sTemp << effects.iLimitSpaceshipProduction;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.iLimitSpaceshipPurchase != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_SPACESHIP_PURCHASE");
		sTemp << effects.iLimitSpaceshipPurchase;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.iIsWorldWar != 0 && effects.iUnitMaintenanceGoldPercent < 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_WORLD_WAR");
		sTemp << effects.iUnitMaintenanceGoldPercent;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.iUnitMaintenanceGoldPercent > 0 && effects.iIsWorldWar == 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_UNIT_MAINTENANCE");
		sTemp << effects.iUnitMaintenanceGoldPercent;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.iUnitMaintenanceGoldPercent > 0 && effects.iIsWorldWar != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_WAR_PEACE");
		sTemp << effects.iUnitMaintenanceGoldPercent;
		vsEffects.push_back(sTemp.toUTF8());
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if(effects.iChangeTourism != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_TOURISM");
		sTemp << effects.iChangeTourism;
		vsEffects.push_back(sTemp.toUTF8());
	}
#endif
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (!MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && effects.iUnitMaintenanceGoldPercent != 0)
#else
	if (effects.iUnitMaintenanceGoldPercent != 0)
#endif
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_UNIT_MAINTENANCE");
		sTemp << effects.iUnitMaintenanceGoldPercent;
		vsEffects.push_back(sTemp.toUTF8());
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && effects.iVassalMaintenanceGoldPercent != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_VASSAL_MAINTENANCE");
		sTemp << effects.iVassalMaintenanceGoldPercent;
		vsEffects.push_back(sTemp.toUTF8());
	}
#endif
	if (effects.iMemberDiscoveredTechMod != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_DISCOVERED_TECH_MODIFIER");
		sTemp << effects.iMemberDiscoveredTechMod;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iCulturePerWonder != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_WONDERS");
		sTemp << effects.iCulturePerWonder;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iCulturePerNaturalWonder != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_NATURAL_WONDERS");
		sTemp << effects.iCulturePerNaturalWonder;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iArtsyGreatPersonRateMod != 0)
	{
		int iMod = effects.iArtsyGreatPersonRateMod;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		CvString sTxtKey = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? "TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_POSITIVE_GREAT_PERSON_RATE_ARTSY" : "TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_POSITIVE_GREAT_PERSON_RATE";
		Localization::String sTemp = (iMod > 0) ? Localization::Lookup(sTxtKey) : Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NEGATIVE_GREAT_PERSON_RATE");
#else
		Localization::String sTemp = (iMod > 0) ? Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_POSITIVE_GREAT_PERSON_RATE") : Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NEGATIVE_GREAT_PERSON_RATE");
#endif
		sTemp << iMod;
		
		CvString sList = "";
		CvUnitClassInfo* pInfo = GC.getUnitClassInfo((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_WRITER"));
		if (pInfo != NULL)
		{
			if (sList != "")
				sList += ", ";
			Localization::String sEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_RATE_ENTRY");
			sEntry << pInfo->GetDescriptionKey();
			sList += sEntry.toUTF8();
		}
		pInfo = GC.getUnitClassInfo((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ARTIST"));
		if (pInfo != NULL)
		{
			if (sList != "")
				sList += ", ";
			Localization::String sEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_RATE_ENTRY");
			sEntry << pInfo->GetDescriptionKey();
			sList += sEntry.toUTF8();
		}
		pInfo = GC.getUnitClassInfo((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MUSICIAN"));
		if (pInfo != NULL)
		{
			if (sList != "")
				sList += ", ";
			Localization::String sEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_RATE_ENTRY");
			sEntry << pInfo->GetDescriptionKey();
			sList += sEntry.toUTF8();
		}
		sTemp << sList;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iScienceyGreatPersonRateMod != 0)
	{
		int iMod = effects.iScienceyGreatPersonRateMod;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		CvString sTxtKey = MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS ? "TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_POSITIVE_GREAT_PERSON_RATE_SCIENCEY" : "TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_POSITIVE_GREAT_PERSON_RATE";
		Localization::String sTemp = (iMod > 0) ? Localization::Lookup(sTxtKey) : Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NEGATIVE_GREAT_PERSON_RATE");
#else
		Localization::String sTemp = (iMod > 0) ? Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_POSITIVE_GREAT_PERSON_RATE") : Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NEGATIVE_GREAT_PERSON_RATE");
#endif
		sTemp << iMod;

		CvString sList = "";
		CvUnitClassInfo* pInfo = GC.getUnitClassInfo((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_SCIENTIST"));
		if (pInfo != NULL)
		{
			if (sList != "")
				sList += ", ";
			Localization::String sEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_RATE_ENTRY");
			sEntry << pInfo->GetDescriptionKey();
			sList += sEntry.toUTF8();
		}
		pInfo = GC.getUnitClassInfo((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ENGINEER"));
		if (pInfo != NULL)
		{
			if (sList != "")
				sList += ", ";
			Localization::String sEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_RATE_ENTRY");
			sEntry << pInfo->GetDescriptionKey();
			sList += sEntry.toUTF8();
		}
		pInfo = GC.getUnitClassInfo((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MERCHANT"));
		if (pInfo != NULL)
		{
			if (sList != "")
				sList += ", ";
			Localization::String sEntry = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_RATE_ENTRY");
			sEntry << pInfo->GetDescriptionKey();
			sList += sEntry.toUTF8();
		}
		sTemp << sList;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iGreatPersonTileImprovementCulture != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_TILE_IMPROVEMENT_CULTURE");
		sTemp << effects.iGreatPersonTileImprovementCulture;
		vsEffects.push_back(sTemp.toUTF8());
	}
	if (effects.iLandmarkCulture != 0)
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_LANDMARK_CULTURE");
		sTemp << effects.iLandmarkCulture;
		vsEffects.push_back(sTemp.toUTF8());
	}

	if (vsEffects.empty())
	{
		vsEffects.push_back(Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_NONE").toUTF8());
	}

	return vsEffects;
}

CvString CvLeague::GetLeagueSplashTitle(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded)
{
	CvString s = "";
	CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);

	if (!bJustFounded && pInfo != NULL)
	{
		Localization::String sTemp = Localization::Lookup(pInfo->GetDescriptionKey());
		s += sTemp.toUTF8();
	}
	else
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_TITLE_FOUNDED");
		s += sTemp.toUTF8();
	}

	return s;
}

CvString CvLeague::GetLeagueSplashDescription(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded)
{
	CvString s = "";
	CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);
	
	if (!bJustFounded && pInfo != NULL)
	{
		EraTypes eEra = pInfo->GetEraTrigger();
		CvEraInfo* pEraInfo = GC.getEraInfo(eEra);
		CvAssert(pEraInfo != NULL);
		if (pEraInfo != NULL)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_GAME_ERA");
			sTemp << pEraInfo->GetDescriptionKey();
			s += sTemp.toUTF8();
			s += "[NEWLINE][NEWLINE]";
		}
		
		s += Localization::Lookup(pInfo->GetHelp()).toUTF8();
	}
	else
	{
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_FOUNDED");
		CvAssert(HasHostMember());
		if (!HasHostMember())
		{
			sTemp << "Nobody";
		}
		else
		{
			sTemp << GET_PLAYER(GetHostMember()).getCivilizationShortDescriptionKey();
		}
		sTemp << GetName() << GetTurnsUntilSession();
		s += sTemp.toUTF8();
	}

	return s;
}

CvString CvLeague::GetLeagueSplashThisEraDetails(LeagueSpecialSessionTypes eGoverningSpecialSession, bool /*bJustFounded*/)
{
	CvString s = "";
	CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);

	s += Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_THIS_ERA").toUTF8();

	if (pInfo != NULL)
	{
		s += "[NEWLINE]";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_HOST_DELEGATES");
		sTemp << pInfo->GetHostDelegates() + pInfo->GetCivDelegates();
		s += sTemp.toUTF8();

		s += "[NEWLINE]";
		sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_CIV_DELEGATES");
		sTemp << pInfo->GetCivDelegates();
		s += sTemp.toUTF8();

		s += "[NEWLINE]";
		sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_CITYSTATE_DELEGATES");
		sTemp << pInfo->GetCityStateDelegates();
		s += sTemp.toUTF8();

		if (pInfo->IsUnitedNations())
		{
			VictoryTypes eDiploVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
			if (eDiploVictory != NO_VICTORY)
			{
				if (GC.getGame().isVictoryValid(eDiploVictory))
				{
					s += "[NEWLINE]";
					sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_DIPLO_VICTORY_POSSIBLE");
					s += sTemp.toUTF8();
				}
			}
		}
	}
	else
	{
		s += "[NEWLINE]";
		s += Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_NO_CHANGE").toUTF8();
	}

	return s;
}

CvString CvLeague::GetLeagueSplashNextEraDetails(LeagueSpecialSessionTypes eGoverningSpecialSession, bool /*bJustFounded*/)
{
	CvString s = "";
	CvLeagueSpecialSessionEntry* pThisSessionInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);
	CvLeagueSpecialSessionEntry* pInfo = NULL;
	if (pThisSessionInfo != NULL)
	{
		EraTypes eNextEra = LeagueHelpers::GetNextGameEraForTrigger(pThisSessionInfo->GetEraTrigger());
		for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
		{
			LeagueSpecialSessionTypes e = (LeagueSpecialSessionTypes)i;
			CvLeagueSpecialSessionEntry* p = GC.getLeagueSpecialSessionInfo(e);
			if (p != NULL && p->GetEraTrigger() == eNextEra)
			{
				pInfo = GC.getLeagueSpecialSessionInfo(e);
				break;
			}
		}
	}

	s += Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_NEXT_ERA").toUTF8();

	if (pInfo != NULL)
	{
		s += "[NEWLINE]";
		Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_HOST_DELEGATES");
		sTemp << pInfo->GetHostDelegates() + pInfo->GetCivDelegates();
		s += sTemp.toUTF8();

		s += "[NEWLINE]";
		sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_CIV_DELEGATES");
		sTemp << pInfo->GetCivDelegates();
		s += sTemp.toUTF8();

		s += "[NEWLINE]";
		sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_CITYSTATE_DELEGATES");
		sTemp << pInfo->GetCityStateDelegates();
		s += sTemp.toUTF8();

		if (pInfo->IsUnitedNations())
		{
			VictoryTypes eDiploVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
			if (eDiploVictory != NO_VICTORY)
			{
				if (GC.getGame().isVictoryValid(eDiploVictory))
				{
					s += "[NEWLINE]";
					sTemp = Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_DIPLO_VICTORY_POSSIBLE");
					s += sTemp.toUTF8();
				}
			}
		}
	}
	else
	{
		s += "[NEWLINE]";
		s += Localization::Lookup("TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_NO_CHANGE").toUTF8();
	}

	return s;
}

CvString CvLeague::GetGreatPersonRateModifierDetails(UnitClassTypes /*eGreatPersonClass*/)
{
	CvString s = "";

	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (it->GetEffects()->iArtsyGreatPersonRateMod != 0 ||
			it->GetEffects()->iScienceyGreatPersonRateMod != 0)
		{
			CvResolutionEntry* pInfo = GC.getResolutionInfo(it->GetType());
			CvAssert(pInfo);
			if (pInfo)
			{
				s += "[NEWLINE]";
				s += "[ICON_BULLET] ";
				s += Localization::Lookup(pInfo->GetDescriptionKey()).toUTF8();
				s += " (";
				s += Localization::Lookup(pInfo->GetHelp()).toUTF8();
				s += ")";
			}
		}
	}

	return s;
}

void CvLeague::CheckProjectAchievements()
{
	for (MemberList::const_iterator member = m_vMembers.begin(); member != m_vMembers.end(); ++member)
	{
		if (member->ePlayer != NO_PLAYER && GET_PLAYER(member->ePlayer).isAlive() && GET_PLAYER(member->ePlayer).isHuman() && GET_PLAYER(member->ePlayer).isLocalPlayer())
		{
			int iHighestContributorProjects = 0;

			for (ProjectList::const_iterator project = m_vProjects.begin(); project != m_vProjects.end(); ++project)
			{
				if (project->bComplete)
				{
					if (GetMemberContributionTier(member->ePlayer, project->eType) == CONTRIBUTION_TIER_3)
					{
						iHighestContributorProjects++;
					}
				}
			}

#if defined(MOD_API_ACHIEVEMENTS)
			if (iHighestContributorProjects >= GC.getNumLeagueProjectInfos() && GC.getNumLeagueProjectInfos() > 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_44);
			}
#endif
		}
	}
}

void CvLeague::CheckStartSession()
{
	if (!IsInSession())
	{
		bool bCanStart = true;
		for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
		{
			if (CanPropose(it->ePlayer))
			{
				bCanStart = false;
				break;
			}
		}
		if (bCanStart)
		{
			StartSession();
		}
	}
}

void CvLeague::StartSession()
{
	SetInSession(true);

	m_vLastTurnEnactProposals.clear();
	m_vLastTurnRepealProposals.clear();

	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); it++)
	{
		m_vLastTurnEnactProposals.push_back(*it);
	}

	for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); it++)
	{
		m_vLastTurnRepealProposals.push_back(*it);
	}

	// Distribute Votes
	AssignStartingVotes();
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = PlayerTypes(iPlayerLoop);
		if (ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive())
		{
			int iDelegates = CalculateStartingVotesForMember(ePlayer);
			if (iDelegates > 0)
			{
				GET_PLAYER(ePlayer).doInstantYield(INSTANT_YIELD_TYPE_DELEGATES, false, NO_GREATPERSON, NO_BUILDING, iDelegates);
			}
		}
	}
	CheckFinishSession();
}

void CvLeague::CheckStartSpecialSession(LeagueSpecialSessionTypes eSpecialSession)
{
	if (CanStartSpecialSession(eSpecialSession))
	{
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eSpecialSession);
		CvAssert(pInfo);
		if (pInfo != NULL)
		{
			CvGame& kGame = GC.getGame();
			CvPlayer& kActivePlayer = GET_PLAYER(kGame.getActivePlayer());

			if(!kGame.isReallyNetworkMultiPlayer() && !kActivePlayer.isObserver())
			{
				// Show splash screen
				CvPopupInfo kPopup(BUTTONPOPUP_LEAGUE_SPLASH, GetID(), GetHostMember(), eSpecialSession, 0, /*bJustFounded*/ false);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}

			DLLUI->AddMessage(0, kGame.getActivePlayer(), false, GC.getEVENT_MESSAGE_TIME(), 
				GetLocalizedText(pInfo->GetDescriptionKey()).GetCString());

			// Becomes United Nations?
			CvAssert(!(!pInfo->IsUnitedNations() && IsUnitedNations())); // UN shouldn't be reversible
			if (pInfo->IsUnitedNations())
			{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
				//UN needs a building to be founded.
				bool bTrigger = true;
				if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) bTrigger = (LeagueHelpers::GetBuildingForTrigger(pInfo->GetBuildingTrigger()) != NO_BUILDING);
				if (bTrigger)
#endif
					SetUnitedNations(true);
			}

			// Only actually hold a session if there is a proposal to be decided
			CvAssertMsg(pInfo->GetImmediateProposal() != NO_RESOLUTION, "Cannot hold special session because there is no proposal to decide on. Please send Anton your save file and version.");
			if (pInfo->GetImmediateProposal() != NO_RESOLUTION)
			{
				StartSpecialSession(eSpecialSession);
			}
		}
	}
}

void CvLeague::StartSpecialSession(LeagueSpecialSessionTypes eSpecialSession)
{
	SetInSession(eSpecialSession);

	// Immediate Proposal
	CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eSpecialSession);
	CvAssert(pInfo != NULL);
	if (pInfo != NULL)
	{
		ResolutionTypes eProposal = pInfo->GetImmediateProposal();
		CvAssert(eProposal != NO_RESOLUTION)
		if (eProposal != NO_RESOLUTION)
		{
			// Put other proposals on hold
			DoPutProposalsOnHold();

			// Add the immediate proposal
			DoProposeEnact(eProposal, NO_PLAYER, LeagueHelpers::CHOICE_NONE);
		}
	}

	// Distribute Votes
	AssignStartingVotes();

	CheckFinishSession();
}

void CvLeague::CheckFinishSession()
{
	if (IsInSession())
	{
		bool bFinished = true;
		for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
		{
			if (CanVote(it->ePlayer))
			{
				bFinished = false;
				break;
			}
		}
		if (bFinished)
		{
			FinishSession();
		}
	}
}

void CvLeague::FinishSession()
{
	PlayerTypes eOldHost = GetHostMember();
	int iTotalSessionVotes = GetVotesSpentThisSession();

	// Find out if we have a recurring proposal
	ResolutionTypes eRecurringProposal = NO_RESOLUTION;
	bool bRecurringProposalThisSession = false;
	if (GetLastSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
	{
		CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(GetLastSpecialSession());
		CvAssert(pInfo != NULL);
		if (pInfo != NULL)
		{
			eRecurringProposal = pInfo->GetRecurringProposal();
		}
	}

	// Notify players of vote results
	NotifySessionDone();

	// Resolve Proposals on table
	for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); it++)
	{
		if (eRecurringProposal != NO_RESOLUTION && eRecurringProposal == it->GetType())
		{
			bRecurringProposalThisSession = true;
		}

		PlayerTypes eProposer = it->GetProposalPlayer();
		LeagueHelpers::PlayerList vHelpedOutcome = it->GetRepealDecision()->GetPlayersVotingForChoice(it->GetRepealDecision()->GetDecision());
		if (it->IsPassed(iTotalSessionVotes))
		{
			// Proposer is grateful to people who helped it pass
			if (eProposer != NO_PLAYER)
			{
				for (LeagueHelpers::PlayerList::iterator playerIt = vHelpedOutcome.begin(); playerIt != vHelpedOutcome.end(); ++playerIt)
				{
					if (GET_PLAYER(*playerIt).getTeam() == GET_PLAYER(eProposer).getTeam())
						continue;

					int iVotePercent = it->GetRepealDecision()->GetPercentContributionToOutcome(*playerIt, it->GetRepealDecision()->GetDecision(), false);

					if (iVotePercent > 0)
					{
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetSupportedOurProposalValue(*playerIt, -iVotePercent);
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheySupportedOurProposalTurn(*playerIt, (GC.getGame().getGameTurn()+1));
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheyFoiledOurProposalTurn(*playerIt, -1);
					}
				}

				GET_PLAYER(eProposer).doInstantYield(INSTANT_YIELD_TYPE_PROPOSAL);
			}

			DoRepealResolution(&(*it));
		}
		else
		{
			// Proposer is angry at people who helped it fail
			if (eProposer != NO_PLAYER)
			{
				for (LeagueHelpers::PlayerList::iterator playerIt = vHelpedOutcome.begin(); playerIt != vHelpedOutcome.end(); ++playerIt)
				{
					if (GET_PLAYER(*playerIt).getTeam() == GET_PLAYER(eProposer).getTeam())
						continue;

					int iVotePercent = it->GetRepealDecision()->GetPercentContributionToOutcome(*playerIt, it->GetRepealDecision()->GetDecision(), false);

					if (iVotePercent > 0)
					{
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetSupportedOurProposalValue(*playerIt, iVotePercent);
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheyFoiledOurProposalTurn(*playerIt, (GC.getGame().getGameTurn()+1));
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheySupportedOurProposalTurn(*playerIt, -1);
					}
				}
			}
		}

		LogProposalResolved(&(*it));
	}
	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); it++)
	{
		if (eRecurringProposal != NO_RESOLUTION && eRecurringProposal == it->GetType())
		{
			bRecurringProposalThisSession = true;
		}

		PlayerTypes eProposer = it->GetProposalPlayer();
		LeagueHelpers::PlayerList vHelpedOutcome = it->GetVoterDecision()->GetPlayersVotingForChoice(it->GetVoterDecision()->GetDecision());
		if (it->IsPassed(iTotalSessionVotes))
		{
			// Proposer is grateful to people who helped it pass
			if (eProposer != NO_PLAYER)
			{
				for (LeagueHelpers::PlayerList::iterator playerIt = vHelpedOutcome.begin(); playerIt != vHelpedOutcome.end(); ++playerIt)
				{
					if (GET_PLAYER(*playerIt).GetID() == GET_PLAYER(eProposer).GetID())
						continue;

					int iVotePercent = it->GetVoterDecision()->GetPercentContributionToOutcome(*playerIt, it->GetVoterDecision()->GetDecision(), false);

					if (iVotePercent > 0)
					{
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetSupportedOurProposalValue(*playerIt, -iVotePercent);
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheySupportedOurProposalTurn(*playerIt, (GC.getGame().getGameTurn()+1));
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheyFoiledOurProposalTurn(*playerIt, -1);
					}
				}

				GET_PLAYER(eProposer).doInstantYield(INSTANT_YIELD_TYPE_PROPOSAL);
			}

			DoEnactResolution(&(*it));

			// Host is grateful to people who helped him keep/gain his position
			PlayerTypes eNewHost = GetHostMember();
			if (it->GetEffects()->bChangeLeagueHost && !vHelpedOutcome.empty() && eNewHost != NO_PLAYER)
			{
				for (LeagueHelpers::PlayerList::iterator playerIt = vHelpedOutcome.begin(); playerIt != vHelpedOutcome.end(); ++playerIt)
				{
					if (GET_PLAYER(*playerIt).GetID() == GET_PLAYER(eNewHost).GetID())
						continue;

					int iVotePercent = it->GetVoterDecision()->GetPercentContributionToOutcome(*playerIt, it->GetVoterDecision()->GetDecision(), true);

					if (iVotePercent > 0)
					{
						GET_PLAYER(eNewHost).GetDiplomacyAI()->SetSupportedOurHostingValue(*playerIt, iVotePercent);
						GET_PLAYER(eNewHost).GetDiplomacyAI()->SetTheySupportedOurHostingTurn(*playerIt, (GC.getGame().getGameTurn()+1));
					}
				}
			}
		}
		else
		{
			// Proposer is angry at people who helped it fail
			if (eProposer != NO_PLAYER)
			{
				for (LeagueHelpers::PlayerList::iterator playerIt = vHelpedOutcome.begin(); playerIt != vHelpedOutcome.end(); ++playerIt)
				{
					if (GET_PLAYER(*playerIt).GetID() == GET_PLAYER(eProposer).GetID())
						continue;

					int iVotePercent = it->GetVoterDecision()->GetPercentContributionToOutcome(*playerIt, it->GetVoterDecision()->GetDecision(), false);

					if (iVotePercent > 0)
					{
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetSupportedOurProposalValue(*playerIt, iVotePercent);
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheyFoiledOurProposalTurn(*playerIt, (GC.getGame().getGameTurn()+1));
						GET_PLAYER(eProposer).GetDiplomacyAI()->SetTheySupportedOurProposalTurn(*playerIt, -1);
					}
				}
			}

			// Since it failed, it may provide members with extra votes
			CvResolutionEntry* pInfo = GC.getResolutionInfo(it->GetType());
			CvAssert(pInfo != NULL);
			if (pInfo != NULL)
			{
				if (pInfo->GetLeadersVoteBonusOnFail() > 0)
				{
					bool bChoicesArePlayers = (pInfo->GetVoterDecision() == RESOLUTION_DECISION_MAJOR_CIV_MEMBER || pInfo->GetVoterDecision() == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER);
					CvAssertMsg(bChoicesArePlayers, "Trying to give bonus delegates since a resolution failed, but voter decision is unexpected type. Please send Anton your save file and version.");
					if (bChoicesArePlayers)
					{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
						std::vector<int> vBonusVotePlayers = it->GetVoterDecision()->GetTopVotedChoices(GC.getLEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES());
#else
						std::vector<int> vBonusVotePlayers = it->GetVoterDecision()->GetTopVotedChoices(LeagueHelpers::NUM_LEADERS_FOR_EXTRA_VOTES);
#endif
						for (uint i = 0; i < vBonusVotePlayers.size(); i++)
						{
							Member* pMember = GetMember((PlayerTypes)vBonusVotePlayers[i]);
							CvAssert(pMember != NULL);
							if (pMember != NULL)
							{
								pMember->iExtraVotes = pMember->iExtraVotes + pInfo->GetLeadersVoteBonusOnFail();
							}
						}
					}
				}
			}
		}

		LogProposalResolved(&(*it));
	}
	PlayerTypes eNewHost = GetHostMember();

	// Update number of sessions held by consecutively by host
	if (eNewHost == eOldHost)
	{
		ChangeConsecutiveHostedSessions(1);
	}
	else
	{
		SetConsecutiveHostedSessions(0);
	}

	// Clear processed proposals, and bring back any that were on hold
	DoClearProposals();

	// Clear existing proposal privileges
	ClearProposalPrivileges();

	bool bRegularSession = (GetCurrentSpecialSession() == NO_LEAGUE_SPECIAL_SESSION);
	if (bRegularSession)
	{
		// Prepare next proposals
		if (eRecurringProposal != NO_RESOLUTION && !bRecurringProposalThisSession && IsResolutionEffectsValid(eRecurringProposal, LeagueHelpers::CHOICE_NONE))
		{
			// We have a recurring proposal that was not proposed this time, so auto propose it for next time
			DoProposeEnact(eRecurringProposal, NO_PLAYER, LeagueHelpers::CHOICE_NONE);
		}
		else
		{
			// Players get to make the proposals for next session
			CvAssertMsg(!IsAnythingProposed(), "Assigning proposal privileges to players when something is already proposed. Please send Anton your save file and version.");
			AssignProposalPrivileges();
		}

		// Reset counter
		ResetTurnsUntilSession();
	}

	SetInSession(false);
}

void CvLeague::AssignStartingVotes()
{
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		if (CanEverVote(it->ePlayer))
		{
			it->iVotes = CalculateStartingVotesForMember(it->ePlayer, /*bForceUpdateSources*/ true);
			it->iAbstainedVotes = 0;
		}
	}
}

void CvLeague::ClearProposalPrivileges()
{
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		it->bMayPropose = false;
		it->iProposals = 0;
	}
}

void CvLeague::AssignProposalPrivileges()
{
	CvWeightedVector<Member*> vpPossibleProposers;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		if (CanEverPropose(it->ePlayer))
		{
			int iVotes = CalculateStartingVotesForMember(it->ePlayer);
			vpPossibleProposers.push_back(&(*it), iVotes);
		}
		else
		{
			CvAssertMsg(!it->bMayPropose, "Found a member with proposal rights that should not have them. Please send Anton your save file and version.");
			CvAssertMsg(it->iProposals == 0, "Found a member with remaining proposals that should not have them. Please send Anton your save file and version.");
		}
	}
	vpPossibleProposers.SortItems();

	int iPrivileges = GetNumProposersPerSession();

	// Host gets one
	PlayerTypes eHost = GetHostMember();
	if (eHost != NO_PLAYER)
	{
		GetMember(eHost)->bMayPropose = true;
		GetMember(eHost)->iProposals = GC.getLEAGUE_MEMBER_PROPOSALS_BASE();
		iPrivileges--;
	}

	// Give rest to largest delegations
	for (int i = 0; i < vpPossibleProposers.size(); i++)
	{
		if (iPrivileges == 0)
		{
			break;
		}
		
		// Only one privilege per player
		if (!vpPossibleProposers.GetElement(i)->bMayPropose)
		{
			vpPossibleProposers.GetElement(i)->bMayPropose = true;
			vpPossibleProposers.GetElement(i)->iProposals = GC.getLEAGUE_MEMBER_PROPOSALS_BASE();
			iPrivileges--;
		}
	}

	CvAssert(iPrivileges == 0);
}

void CvLeague::CheckProposalsValid()
{
	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
	{
		if (!IsResolutionEffectsValid(it->GetType(), it->GetProposerDecision()->GetDecision()))
		{
			m_vEnactProposals.erase(it);
			it--;
		}
	}
	for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
	{
		if (!IsResolutionEffectsValid(it->GetType(), it->GetProposerDecision()->GetDecision()))
		{
			m_vRepealProposals.erase(it);
			it--;
		}
	}
}

void CvLeague::CheckResolutionsValid()
{
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); ++it)
	{
		if (!IsResolutionEffectsValid(it->GetType(), it->GetProposerDecision()->GetDecision()))
		{
			for (uint i = 0; i < m_vMembers.size(); i++)
			{
				it->RemoveEffects(m_vMembers[i].ePlayer);
			}
			m_vActiveResolutions.erase(it);
			//antonjs: todo: relocate these league-level effects:
			for (uint i = 0; i < m_vMembers.size(); i++)
			{
				GET_PLAYER(m_vMembers[i].ePlayer).CalculateNetHappiness();
				GET_PLAYER(m_vMembers[i].ePlayer).updateYield();
				GET_PLAYER(m_vMembers[i].ePlayer).recomputeGreatPeopleModifiers();
			}
			it--;
		}
	}
}

void CvLeague::AssignNewHost()
{
	// Set host as remaining player with most votes
	PlayerTypes eNewHost = NO_PLAYER;
	int iHighestVotes = -1;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
	{
		int iVotes = CalculateStartingVotesForMember(it->ePlayer);
		if (iVotes > iHighestVotes)
		{
			eNewHost = it->ePlayer;
			iHighestVotes = iVotes;
		}
	}
	CvAssert(eNewHost != NO_PLAYER);
	SetHostMember(eNewHost);
}

void CvLeague::DoEnactResolution(CvEnactProposal* pProposal)
{
	CvAssertMsg(pProposal->IsPassed(GetVotesSpentThisSession()), "Doing a proposal that has not been passed. Please send Anton your save file and version.");

	CvActiveResolution resolution(pProposal);
	for (uint i = 0; i < m_vMembers.size(); i++)
	{
		resolution.DoEffects(m_vMembers[i].ePlayer);
	}
	
	// Active Resolutions with only one-time effects immediately expire
	if (resolution.HasOngoingEffects())
	{
		m_vActiveResolutions.push_back(resolution);
	}
	
	//antonjs: todo: relocate these league-level effects:
	for (uint i = 0; i < m_vMembers.size(); i++)
	{
		GET_PLAYER(m_vMembers[i].ePlayer).CalculateNetHappiness();
		GET_PLAYER(m_vMembers[i].ePlayer).updateYield();
		GET_PLAYER(m_vMembers[i].ePlayer).recomputeGreatPeopleModifiers();
	}
	LeagueProjectTypes eProject = resolution.GetEffects()->eLeagueProjectEnabled;
	if (eProject != NO_LEAGUE_PROJECT)
	{
		CvAssertMsg(!IsProjectActive(eProject), "Trying to start a League Project that is already active. Please send Anton your save file and version.");
		StartProject(eProject);
	}

	m_iNumResolutionsEverEnacted++;
}

void CvLeague::DoRepealResolution(CvRepealProposal* pProposal)
{
	CvAssertMsg(pProposal->IsPassed(GetVotesSpentThisSession()), "Doing a proposal that has not been passed. Please send Anton your save file and version.");

	int iFound = 0;
	for (ActiveResolutionList::iterator it = m_vActiveResolutions.begin(); it != m_vActiveResolutions.end(); it++)
	{
		if (it->GetID() == pProposal->GetTargetResolutionID())
		{
			for (uint i = 0; i < m_vMembers.size(); i++)
			{
				it->RemoveEffects(m_vMembers[i].ePlayer);
			}
			m_vActiveResolutions.erase(it);
			//antonjs: todo: relocate these league-level effects:
			for (uint i = 0; i < m_vMembers.size(); i++)
			{
				GET_PLAYER(m_vMembers[i].ePlayer).CalculateNetHappiness();
				GET_PLAYER(m_vMembers[i].ePlayer).updateYield();
				GET_PLAYER(m_vMembers[i].ePlayer).recomputeGreatPeopleModifiers();
			}
			it--;
			iFound++;
		}
	}
	CvAssertMsg(iFound == 1, "Unexpected number of active resolutions with this ID. Please send Anton your save file and version.");
}

void CvLeague::DoClearProposals()
{
	m_vEnactProposals.clear();
	m_vRepealProposals.clear();

	// If we had proposals on hold, put them back up
	if (HasProposalsOnHold())
	{
		for (EnactProposalList::const_iterator it = m_vEnactProposalsOnHold.begin(); it != m_vEnactProposalsOnHold.end(); ++it)
		{
			m_vEnactProposals.push_back(*it);
		}
		m_vEnactProposalsOnHold.clear();

		for (RepealProposalList::const_iterator it = m_vRepealProposalsOnHold.begin(); it != m_vRepealProposalsOnHold.end(); ++it)
		{
			m_vRepealProposals.push_back(*it);
		}
		m_vRepealProposalsOnHold.clear();
	}

	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

void CvLeague::DoPutProposalsOnHold()
{
	// Should not already have proposals on hold!
	CvAssertMsg(!HasProposalsOnHold(), "Attempting to put proposals on hold (for a special session), but proposals are already on hold. Please send Anton your save file and version.");
	m_vEnactProposalsOnHold.clear();
	m_vRepealProposalsOnHold.clear();

	for (EnactProposalList::const_iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); ++it)
	{
		m_vEnactProposalsOnHold.push_back(*it);
	}
	m_vEnactProposals.clear();

	for (RepealProposalList::const_iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); ++it)
	{
		m_vRepealProposalsOnHold.push_back(*it);
	}
	m_vRepealProposals.clear();

	GC.GetEngineUserInterface()->setDirty(LeagueScreen_DIRTY_BIT, true);
}

bool CvLeague::HasProposalsOnHold()
{
	bool bEnactProposalOnHold = !m_vEnactProposalsOnHold.empty();
	bool bRepealProposalOnHold = !m_vRepealProposalsOnHold.empty();

	return (bEnactProposalOnHold || bRepealProposalOnHold);
}

// Should be called before the list of proposals is resolved and cleared
void CvLeague::NotifySessionDone()
{
	for (EnactProposalList::iterator it = m_vEnactProposals.begin(); it != m_vEnactProposals.end(); it++)
	{
		NotifyProposalResult(&(*it));
	}
	for (RepealProposalList::iterator it = m_vRepealProposals.begin(); it != m_vRepealProposals.end(); it++)
	{
		NotifyProposalResult(&(*it));
	}
}

void CvLeague::NotifyProposalResult(CvEnactProposal* pProposal)
{
	CvAssert(pProposal != NULL);
	if (pProposal == NULL) return;

#if defined(MOD_EVENTS_RESOLUTIONS)
	int iDecision = -1;
#endif

	CvString sSummary = "";
	CvString sMessage = "";
	if (pProposal->GetEffects()->bDiplomaticVictory)
	{
		// World Leader results are handled a little differently
		if (pProposal->IsPassed(GetVotesSpentThisSession()))
		{
			PlayerTypes eWinner = (PlayerTypes) pProposal->GetVoterDecision()->GetDecision();
			CvAssert(eWinner != NO_PLAYER);
			if (eWinner != NO_PLAYER)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS");
				sTemp << GET_PLAYER(eWinner).getCivilizationShortDescriptionKey();
				sSummary += sTemp.toUTF8();

				sTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS_DETAILS");
				sTemp << GC.getGame().GetVotesNeededForDiploVictory() << GET_PLAYER(eWinner).getCivilizationShortDescriptionKey();
				sMessage += sTemp.toUTF8();
			}
		}
		else
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL");
			sSummary += sTemp.toUTF8();
			
			sTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL_DETAILS");
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
			sTemp << GC.getGame().GetVotesNeededForDiploVictory() << GC.getLEAGUE_NUM_LEADERS_FOR_EXTRA_VOTES();
#else
			sTemp << GC.getGame().GetVotesNeededForDiploVictory() << LeagueHelpers::NUM_LEADERS_FOR_EXTRA_VOTES;
#endif
			sMessage += sTemp.toUTF8();
		}
		sMessage += "[NEWLINE]" + pProposal->GetVoterDecision()->GetVotesAsText(this);
	
#if defined(MOD_EVENTS_RESOLUTIONS)
		iDecision = pProposal->GetVoterDecision()->GetDecision();
#endif
	}
	else if (pProposal->GetEffects()->bChangeLeagueHost)
	{
		// Change Host results are handled a little differently
		PlayerTypes eOldHost = GetHostMember();
		PlayerTypes eNewHost = eOldHost;
		CvAssert(pProposal->IsPassed(GetVotesSpentThisSession()));
		if (pProposal->IsPassed(GetVotesSpentThisSession()))
		{
			eNewHost = (PlayerTypes) pProposal->GetVoterDecision()->GetDecision();
		}

		CvString sHostKey = "Nobody";
		CvAssertMsg(eNewHost != NO_PLAYER, "Could not determine the new host. Please send Anton your save file and version.");
		if (eNewHost != NO_PLAYER)
		{
			sHostKey = GET_PLAYER(eNewHost).getCivilizationShortDescriptionKey();
		}
		
		if (eNewHost != eOldHost)
		{
			Localization::String sSummaryTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_HOST_CHANGED_SUMMARY");
			sSummaryTemp << sHostKey;
			sSummary += sSummaryTemp.toUTF8();
		}
		else
		{
			Localization::String sSummaryTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_HOST_SAME_SUMMARY");
			sSummaryTemp << sHostKey;
			sSummary += sSummaryTemp.toUTF8();
		}

		Localization::String sMessageTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_HOST_DETAILS");
		sMessageTemp << GetName() << sHostKey;
		sMessage += sMessageTemp.toUTF8();
		sMessage += "[NEWLINE]" + pProposal->GetVoterDecision()->GetVotesAsText(this);
	
#if defined(MOD_EVENTS_RESOLUTIONS)
		iDecision = pProposal->GetVoterDecision()->GetDecision();
#endif
	}
	else
	{
		Localization::String sSummaryTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_FAIL_SUMMARY");
		Localization::String sMessageTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_ENACT_FAIL");
		CvString sName = GetResolutionName(pProposal->GetType(), pProposal->GetID(), pProposal->GetProposerDecision()->GetDecision(), /*bIncludePrefix*/ true);
		if (pProposal->IsPassed(GetVotesSpentThisSession()))
		{
			sSummaryTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_PASS_SUMMARY");
			sMessageTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_ENACT_PASS");
		}
		sSummaryTemp << sName;
		sMessageTemp << sName;
		sSummary = sSummaryTemp.toUTF8();
		sMessage = sMessageTemp.toUTF8();
		sMessage += "[NEWLINE]" + pProposal->GetVoterDecision()->GetVotesAsText(this);
	
#if defined(MOD_EVENTS_RESOLUTIONS)
		iDecision = pProposal->GetProposerDecision()->GetDecision();
#endif
	}

#if defined(MOD_EVENTS_RESOLUTIONS)
	if (MOD_EVENTS_RESOLUTIONS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ResolutionResult, pProposal->GetType(), pProposal->GetProposalPlayer(), iDecision, true, pProposal->IsPassed(GetVotesSpentThisSession()));
	}
#endif

	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		PlayerTypes eMember = it->ePlayer;
		if (GET_PLAYER(eMember).isHuman())
		{
			CvNotifications* pNotifications = GET_PLAYER(eMember).GetNotifications();
			if (pNotifications)
			{
				pNotifications->Add(NOTIFICATION_LEAGUE_VOTING_DONE, sMessage, sSummary, -1, -1, GetID());
			}				
		}
	}
}

void CvLeague::NotifyProposalResult(CvRepealProposal* pProposal)
{
	CvAssert(pProposal != NULL);
	if (pProposal == NULL) return;

	Localization::String sSummaryTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_FAIL_SUMMARY");
	Localization::String sMessageTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_REPEAL_FAIL");
	CvString sName = GetResolutionName(pProposal->GetType(), pProposal->GetID(), pProposal->GetProposerDecision()->GetDecision(), /*bIncludePrefix*/ true);
	int iTotalSessionVotes = GetVotesSpentThisSession();
	if (pProposal->IsPassed(iTotalSessionVotes))
	{
		sSummaryTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_PASS_SUMMARY");
		sMessageTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_REPEAL_PASS");
	}
	sSummaryTemp << sName;
	sMessageTemp << sName;
	CvString sSummary = sSummaryTemp.toUTF8();
	CvString sMessage = sMessageTemp.toUTF8();
	sMessage += "[NEWLINE]" + pProposal->GetRepealDecision()->GetVotesAsText(this);

#if defined(MOD_EVENTS_RESOLUTIONS)
	if (MOD_EVENTS_RESOLUTIONS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ResolutionResult, pProposal->GetType(), pProposal->GetProposalPlayer(), pProposal->GetProposerDecision()->GetDecision(), false, pProposal->IsPassed(iTotalSessionVotes));
	}
#endif

	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		PlayerTypes eMember = it->ePlayer;
		if (GET_PLAYER(eMember).isHuman())
		{
			CvNotifications* pNotifications = GET_PLAYER(eMember).GetNotifications();
			if (pNotifications)
			{
				pNotifications->Add(NOTIFICATION_LEAGUE_VOTING_DONE, sMessage, sSummary, -1, -1, GetID());
			}				
		}
	}
}

void CvLeague::NotifySessionSoon(int iTurnsLeft)
{
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		PlayerTypes eMember = it->ePlayer;
		if (GET_PLAYER(eMember).isHuman())
		{
			CvNotifications* pNotifications = GET_PLAYER(eMember).GetNotifications();
			if (pNotifications)
			{
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_SOON");

				Localization::String strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTING_SOON_TT");
				strTemp << GetName() << iTurnsLeft;
				CvString strInfo = strTemp.toUTF8();

				for (EnactProposalList::iterator itProposal = m_vEnactProposals.begin(); itProposal != m_vEnactProposals.end(); ++itProposal)
				{
					strInfo += "[NEWLINE][ICON_BULLET]";
					strInfo += GetResolutionName(itProposal->GetType(), itProposal->GetID(), itProposal->GetProposerDecision()->GetDecision(), /*bIncludePrefix*/ true);
				}
				for (RepealProposalList::iterator itProposal = m_vRepealProposals.begin(); itProposal != m_vRepealProposals.end(); ++itProposal)
				{
					strInfo += "[NEWLINE][ICON_BULLET]";
					strInfo += GetResolutionName(itProposal->GetType(), itProposal->GetID(), itProposal->GetProposerDecision()->GetDecision(), /*bIncludePrefix*/ true);
				}

				pNotifications->Add(NOTIFICATION_LEAGUE_VOTING_SOON, strInfo, strSummary, -1, -1, GetID());
			}
		}
	}
}

void CvLeague::NotifyProjectComplete(LeagueProjectTypes eProject)
{
	CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eProject);
	CvAssertMsg(pInfo, "Could not find League Project info when sending out a progress update. Please send Anton your save file and version.");
	if (pInfo)
	{
		for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
		{
			PlayerTypes eMember = it->ePlayer;
			CvPlayer& kPlayer = GET_PLAYER(eMember);
			if (kPlayer.isHuman())
			{
				if (kPlayer.isLocalPlayer() && !kPlayer.isObserver())
				{
					CvNotifications* pNotifications = kPlayer.GetNotifications();
					if (pNotifications)
					{
						Localization::String sSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_PROJECT_COMPLETE");
						sSummary << pInfo->GetDescriptionKey();

						Localization::String sMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_PROJECT_COMPLETE_TT");
						sMessage << pInfo->GetDescriptionKey();

						pNotifications->Add((NotificationTypes)FString::Hash("NOTIFICATION_LEAGUE_PROJECT_COMPLETE"), sMessage.toUTF8(), sSummary.toUTF8(), -1, -1, GetID(), eProject);
					}
				}
			}
		}
	}
}

void CvLeague::NotifyProjectProgress(LeagueProjectTypes eProject)
{
	CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eProject);
	CvAssertMsg(pInfo, "Could not find League Project info when sending out a progress update. Please send Anton your save file and version.");
	if (pInfo)
	{
		for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); ++it)
		{
			PlayerTypes eMember = it->ePlayer;
			CvPlayer& kPlayer = GET_PLAYER(eMember);
			if (kPlayer.isHuman())
			{
				if (kPlayer.isLocalPlayer() && !kPlayer.isObserver())
				{
					CvNotifications* pNotifications = kPlayer.GetNotifications();
					if (pNotifications)
					{
						int iPercentCompleted = (int) ((float)GetProjectProgress(eProject) / max(1,GetProjectCost(eProject)) * 100);
						iPercentCompleted = MIN(100, iPercentCompleted);

						Localization::String sSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_PROJECT_PROGRESS");
						sSummary << pInfo->GetDescriptionKey();

						Localization::String sMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_PROJECT_PROGRESS_TT");
						sMessage << pInfo->GetDescriptionKey() << iPercentCompleted;

						pNotifications->Add((NotificationTypes)FString::Hash("NOTIFICATION_LEAGUE_PROJECT_PROGRESS"), sMessage.toUTF8(), sSummary.toUTF8(), -1, -1, GetID(), eProject);
					}
				}
			}
		}
	}
}

void CvLeague::StartProject(LeagueProjectTypes eLeagueProject)
{
	CvAssertMsg(!IsProjectActive(eLeagueProject), "Attempting to start a League Project which is already underway. Please send Anton your save file and version.");
	if (!IsProjectActive(eLeagueProject))
	{
		Project project;
		project.eType = eLeagueProject;
		m_vProjects.push_back(project);
	}
}

void CvLeague::CheckProjectsProgress()
{
	for (ProjectList::iterator it = m_vProjects.begin(); it != m_vProjects.end(); it++)
	{
		if (IsProjectActive(it->eType) && !IsProjectComplete(it->eType))
		{
			CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(it->eType);
			if (pProjectInfo)
			{
#if defined(MOD_BALANCE_CORE)
				//We do the process production here, as otherwise some players might be skipped!
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
					if (eLoopPlayer == NO_PLAYER || !GET_PLAYER(eLoopPlayer).isAlive())
						continue;

					CvPlayer &kPlayer = GET_PLAYER(eLoopPlayer);

					CvCity* pLoopCity;
					int iLoop;
					for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
					{
						//not working on the project? ignore.
						if (pLoopCity->getProductionProcess() != pProjectInfo->GetProcess())
							continue;

						GC.getGame().GetGameLeagues()->DoLeagueProjectContribution(eLoopPlayer, it->eType, pLoopCity->getCurrentProductionDifferenceTimes100(false, true));

					}
				}
#endif
				// How much do we need?
				int iNeeded = GetProjectCost(it->eType);
				CvAssertMsg(iNeeded != 0, "Invalid cost for League Project. Please send Anton your save file and version.");

				// How much do we have?
				int iTotal = GetProjectProgress(it->eType);

				// Is it finished?
				if (iTotal >= iNeeded)
				{
					CvAssert(!it->bComplete);
					if (!it->bComplete)
					{
						DoProjectRewards(it->eType);
						NotifyProjectComplete(it->eType);
						it->bComplete = true;

						CvGame& kGame = GC.getGame();
						CvPlayer& kActivePlayer = GET_PLAYER(kGame.getActivePlayer());

						if(!kGame.isReallyNetworkMultiPlayer() && !kActivePlayer.isObserver())
						{
							CvPopupInfo kPopup(BUTTONPOPUP_LEAGUE_PROJECT_COMPLETED, GetID(), it->eType);
							GC.GetEngineUserInterface()->AddPopup(kPopup);
						}
						CheckProjectAchievements();
					}
				}
				// How close is it?
				else
				{
					int iPercentCompleted = (int) (((float)iTotal / max(1,iNeeded)) * 100);
					iPercentCompleted = MIN(100, iPercentCompleted);

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
					if (!it->bProgressWarningSent && iPercentCompleted >= GC.getLEAGUE_PROJECT_PROGRESS_PERCENT_WARNING())
#else
					if (!it->bProgressWarningSent && iPercentCompleted >= LeagueHelpers::PROJECT_PROGRESS_PERCENT_WARNING)
#endif
					{
						NotifyProjectProgress(it->eType);
						it->bProgressWarningSent = true;
					}
				}
			}
		}
	}
}

void CvLeague::DoProjectRewards(LeagueProjectTypes eLeagueProject)
{
	CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(eLeagueProject);
	CvAssert(pProjectInfo);
	if (!pProjectInfo) return;

	int iTopTierRecipients = 0;
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		PlayerTypes eMember = it->ePlayer;
		if (GET_PLAYER(eMember).isAlive() && !GET_PLAYER(eMember).isMinorCiv())
		{
			ContributionTier eTier = GetMemberContributionTier(eMember, eLeagueProject);

			// Break ties for top reward tier - Only one can get it, give it to the first
			if (eTier == CONTRIBUTION_TIER_3)
			{
				iTopTierRecipients++;
			}
			if (iTopTierRecipients > 1)
			{
				eTier = CONTRIBUTION_TIER_2;
				iTopTierRecipients--;
			}

			DoProjectReward(eMember, eLeagueProject, eTier);
			
			//antonjs: todo: undo rewards when killed and/or project is repeated
		}
	}
}

void CvLeague::DoProjectReward(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, ContributionTier eTier)
{
	CvLeagueProjectEntry* pProjectInfo = GC.getLeagueProjectInfo(eLeagueProject);
	CvAssert(pProjectInfo);
	if (!pProjectInfo) return;
	
	// Which rewards do we get?
	//antonjs: A switch statement in its natural habitat without break statements...a rare sight indeed
	vector<LeagueProjectRewardTypes> veRewards;
	switch (eTier)
	{
	case CONTRIBUTION_TIER_3:
		{
			veRewards.push_back(pProjectInfo->GetRewardTier3());
		}
	case CONTRIBUTION_TIER_2:
		{
			veRewards.push_back(pProjectInfo->GetRewardTier2());
		}
	case CONTRIBUTION_TIER_1:
		{
			veRewards.push_back(pProjectInfo->GetRewardTier1());
		}
	default:
		{
			break;
		}
	}

	for (uint i = 0; i < veRewards.size(); i++)
	{
		CvLeagueProjectRewardEntry* pRewardInfo = GC.getLeagueProjectRewardInfo(veRewards[i]);
		CvAssert(pRewardInfo);
		if (!pRewardInfo) continue;

		// Free Building in Capital
		if (pRewardInfo->GetBuilding() != NO_BUILDING)
		{
			CvCity* pCapital = GET_PLAYER(ePlayer).getCapitalCity();
			CvAssertMsg(pCapital, "Player does not have a capital city. Please send Anton your save file and version.");
			if (pCapital)
			{
				CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(pRewardInfo->GetBuilding());
				if (pBuildingInfo)
				{
					pCapital->CreateBuilding(pRewardInfo->GetBuilding());
					pCapital->CleanUpQueue(); // cleans out items from the queue that may be invalidated by the recent construction
				}
			}
		}

		// Happiness
		if (pRewardInfo->GetHappiness() != 0)
		{
			GET_PLAYER(ePlayer).ChangeHappinessFromLeagues(pRewardInfo->GetHappiness());
			GET_PLAYER(ePlayer).CalculateNetHappiness();
		}

		// Free Social Policy
		if (pRewardInfo->GetFreeSocialPolicies() > 0)
		{
			GET_PLAYER(ePlayer).ChangeNumFreePolicies(pRewardInfo->GetFreeSocialPolicies());
		}

		// Temporary Culture Modifier
		if (pRewardInfo->GetCultureBonusTurns() > 0)
		{
			GET_PLAYER(ePlayer).ChangeCultureBonusTurns(pRewardInfo->GetCultureBonusTurns());
		}

		// Temporary Tourism Modifier
		if (pRewardInfo->GetTourismBonusTurns() > 0)
		{
			GET_PLAYER(ePlayer).ChangeTourismBonusTurns(pRewardInfo->GetTourismBonusTurns());
		}

		// Golden Age Points
		if (pRewardInfo->GetGoldenAgePoints() > 0)
		{
			GET_PLAYER(ePlayer).ChangeGoldenAgeProgressMeter(pRewardInfo->GetGoldenAgePoints());
		}

		// City-State Influence Boost
		//antonjs: todo: ordering, to prevent ally / no longer ally notif spam
		if (pRewardInfo->GetCityStateInfluenceBoost() > 0)
		{
			for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
			{
				PlayerTypes eMinorCivLoop = (PlayerTypes) iMinorCivLoop;
				if (GET_PLAYER(eMinorCivLoop).isAlive() && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eMinorCivLoop).getTeam()))
				{
					GET_PLAYER(eMinorCivLoop).GetMinorCivAI()->ChangeFriendshipWithMajor(ePlayer, pRewardInfo->GetCityStateInfluenceBoost());
				}
			}
		}

		// Beaker boost based on previous turns
		if (pRewardInfo->GetBaseBeakersTurnsToCount() > 0)
		{
			int iPreviousTurnsToCount = pRewardInfo->GetBaseBeakersTurnsToCount();
			int iBeakersBonus = GET_PLAYER(ePlayer).getYieldPerTurnHistory(YIELD_SCIENCE, iPreviousTurnsToCount);
			TechTypes eCurrentTech = GET_PLAYER(ePlayer).GetPlayerTechs()->GetCurrentResearch();
			if(eCurrentTech == NO_TECH)
			{
				GET_PLAYER(ePlayer).changeOverflowResearch(iBeakersBonus);
			}
			else
			{
				GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iBeakersBonus, ePlayer);
			}
		}

		// Free unit class
		if (pRewardInfo->GetFreeUnitClass() != NO_UNITCLASS)
		{
			UnitTypes eUnit = GET_PLAYER(ePlayer).GetSpecificUnitType(pRewardInfo->GetFreeUnitClass());
			if (eUnit != NO_UNIT)
			{
				CvCity* pCapital = GET_PLAYER(ePlayer).getCapitalCity();
				if (pCapital)
				{
					CvPlot* pSpawnPlot = pCapital->plot();

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
					CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
					if(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pkUnitInfo)
					{
						if(pkUnitInfo->GetDomainType() == DOMAIN_SEA)
						{
							CvPlot* pWaterSpawnPlot = GET_PLAYER(ePlayer).GetBestCoastalSpawnPlot(NULL);
							if (pWaterSpawnPlot)
								pSpawnPlot = pWaterSpawnPlot;
						}
					}
#endif
					CvUnit* pUnit = GET_PLAYER(ePlayer).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY());
					pCapital->addProductionExperience(pUnit);
					pUnit->jumpToNearestValidPlot();
				}
			}
		}
		
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
			//CSD Project Rewards
			if (pRewardInfo->GetAttackBonusTurns() > 0)
			{
				GET_PLAYER(ePlayer).ChangeAttackBonusTurns(pRewardInfo->GetAttackBonusTurns());
			}
			if (pRewardInfo->GetBaseFreeUnits() > 0)
			{
				GET_PLAYER(ePlayer).changeBaseFreeUnits(pRewardInfo->GetBaseFreeUnits());
			}
			// Temporary Culture Modifier
			if (pRewardInfo->GetNumFreeGreatPeople() > 0)
			{
				GET_PLAYER(ePlayer).ChangeNumFreeGreatPeople(pRewardInfo->GetNumFreeGreatPeople());
			}
		}
#endif
	}
}

void CvLeague::UpdateName()
{
	// Roll for a new name type
	int iRoll = GC.getGame().getSmallFakeRandNum(GC.getNumLeagueNameInfos(), GC.getNumLeagueNameInfos());
	CvLeagueNameEntry* pInfo = GC.getLeagueNameInfo((LeagueNameTypes)iRoll);
	if (pInfo)
	{
		m_eAssignedName = (LeagueNameTypes) GC.getInfoTypeForString(pInfo->GetType());
	}
}

// Must be called while the Congress is still in session, before the proposal is deleted
void CvLeague::LogProposalResolved(CvEnactProposal* pProposal)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return;
	CvString sMessage = "";

	sMessage += ",Congress";
	sMessage += ",- - -";
	if (pProposal->IsPassed(GetVotesSpentThisSession()))
	{
		sMessage += ",Proposal Passed";
	}
	else
	{
		sMessage += ",Proposal Failed";
	}

	CvAssert(pProposal != NULL);
	if (pProposal != NULL && pProposal->GetVoterDecision() != NULL)
	{
		sMessage += ",";
		sMessage += pProposal->GetProposalName(/*bForLogging*/ true);

		sMessage += ",";
		int iDecision = pProposal->GetVoterDecision()->GetDecision();
		if (iDecision == LeagueHelpers::CHOICE_NONE)
		{
			sMessage += "None";
		}
		else
		{
			sMessage += LeagueHelpers::GetTextForChoice(pProposal->GetVoterDecision()->GetType(), iDecision);
		}
		
		CvString sTemp;
		sTemp.Format(",Won by %d", pProposal->GetVoterDecision()->GetVotesMarginOfTopChoice());
		sMessage += sTemp;
	}

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}

// Must be called while the Congress is still in session, before the proposal is deleted
void CvLeague::LogProposalResolved(CvRepealProposal* pProposal)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return;
	CvString sMessage = "";

	sMessage += ",Congress";
	sMessage += ",- - -";
	if (pProposal->IsPassed(GetVotesSpentThisSession()))
	{
		sMessage += ",Proposal Passed";
	}
	else
	{
		sMessage += ",Proposal Failed";
	}

	CvAssert(pProposal != NULL);
	if (pProposal != NULL && pProposal->GetRepealDecision() != NULL)
	{
		sMessage += ",";
		sMessage += pProposal->GetProposalName(/*bForLogging*/ true);

		sMessage += ",";
		int iDecision = pProposal->GetRepealDecision()->GetDecision();
		if (iDecision == LeagueHelpers::CHOICE_NONE)
		{
			sMessage += "None";
		}
		else
		{
			sMessage += LeagueHelpers::GetTextForChoice(pProposal->GetRepealDecision()->GetType(), iDecision);
		}
		
		CvString sTemp;
		sTemp.Format(",Won by %d", pProposal->GetRepealDecision()->GetVotesMarginOfTopChoice());
		sMessage += sTemp;
	}

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}

CvLeague::Member* CvLeague::GetMember(PlayerTypes ePlayer)
{
	for (MemberList::iterator it = m_vMembers.begin(); it != m_vMembers.end(); it++)
	{
		if (it->ePlayer == ePlayer)
		{
			return &(*it);
		}
	}
	return NULL;
}

CvLeague::Project* CvLeague::GetProject(LeagueProjectTypes eLeagueProject)
{
	for (ProjectList::iterator it = m_vProjects.begin(); it != m_vProjects.end(); it++)
	{
		if (it->eType == eLeagueProject)
		{
			return &(*it);
		}
	}
	return NULL;
}

template<typename MemberT, typename Visitor>
void CvLeague::Member::Serialize(MemberT& member, Visitor& visitor)
{
	visitor(member.ePlayer);
	visitor(member.iExtraVotes);
	visitor(member.sVoteSources);
	visitor(member.bMayPropose);
	visitor(member.iProposals);
	visitor(member.iVotes);
	visitor(member.iAbstainedVotes);
	visitor(member.bEverBeenHost);
	visitor(member.bAlwaysBeenHost);
}

FDataStream& operator>>(FDataStream& stream, CvLeague::Member& member)
{
	CvStreamLoadVisitor serialVisitor(stream);
	CvLeague::Member::Serialize(member, serialVisitor);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvLeague::Member& member)
{
	CvStreamSaveVisitor serialVisitor(stream);
	CvLeague::Member::Serialize(member, serialVisitor);
	return stream;
}

template<typename ProjectT, typename Visitor>
void CvLeague::Project::Serialize(ProjectT& project, Visitor& visitor)
{
	visitor(project.eType);
	visitor(project.vProductionList);
	visitor(project.bComplete);
	visitor(project.bProgressWarningSent);
}

FDataStream& operator>>(FDataStream& stream, CvLeague::Project& project)
{
	CvStreamLoadVisitor serialVisitor(stream);
	CvLeague::Project::Serialize(project, serialVisitor);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvLeague::Project& project)
{
	CvStreamSaveVisitor serialVisitor(stream);
	CvLeague::Project::Serialize(project, serialVisitor);
	return stream;
}

template<typename League, typename Visitor>
void CvLeague::Serialize(League& league, Visitor& visitor)
{
	visitor(league.m_eID);
	visitor(league.m_bUnitedNations);
	visitor(league.m_bInSession);
	visitor(league.m_iTurnsUntilSession);
	visitor(league.m_iNumResolutionsEverEnacted);
	visitor(league.m_vEnactProposals);
	visitor(league.m_vRepealProposals);
	visitor(league.m_vActiveResolutions);
	visitor(league.m_vMembers);
	visitor(league.m_eHost);
	visitor(league.m_vProjects);
	visitor(league.m_iConsecutiveHostedSessions);
	visitor(league.m_eAssignedName);
	visitor(league.m_szCustomName);
	visitor(league.m_eLastSpecialSession);
	visitor(league.m_eCurrentSpecialSession);
	visitor(league.m_vEnactProposalsOnHold);
	visitor(league.m_vRepealProposalsOnHold);

	visitor(league.m_vLastTurnEnactProposals);
	visitor(league.m_vLastTurnRepealProposals);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvLeague& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvLeague::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvLeague& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvLeague::Serialize(readFrom, serialVisitor);
	return saveTo;
}

// ================================================================================
//			CvGameLeagues
// ================================================================================
CvGameLeagues::CvGameLeagues(void)
{
	m_vActiveLeagues.clear();
	m_iNumLeaguesEverFounded = 0;
	m_eDiplomaticVictor = NO_PLAYER;
	m_iGeneratedIDCount = 0;
	m_eLastEraTrigger = NO_ERA;
}

CvGameLeagues::~CvGameLeagues(void)
{
}

void CvGameLeagues::Init()
{
}

void CvGameLeagues::DoTurn()
{
	if (!GC.getGame().isOption(GAMEOPTION_NO_LEAGUES))
	{
		GC.getGame().DoUpdateDiploVictory();

		// Not yet founded - is it time to start?
		if (GetNumActiveLeagues() == 0)
		{
			// Has any living major civ met every other living major civ?
			for (int iCiv = 0; iCiv < MAX_MAJOR_CIVS; iCiv++)
			{
				PlayerTypes eCiv = (PlayerTypes) iCiv;
				if (GET_PLAYER(eCiv).isAlive())
				{
					// Has the unlock from tech?
					if (GET_TEAM(GET_PLAYER(eCiv).getTeam()).HasTechForWorldCongress())
					{
						// Met every other civ?
						bool bMetEveryone = true;
						for (int iOtherCiv = 0; iOtherCiv < MAX_MAJOR_CIVS; iOtherCiv++)
						{
							PlayerTypes eOtherCiv = (PlayerTypes) iOtherCiv;
							if (eCiv != eOtherCiv && GET_PLAYER(eOtherCiv).isAlive())
							{
								if (!GET_TEAM(GET_PLAYER(eCiv).getTeam()).isHasMet(GET_PLAYER(eOtherCiv).getTeam()))
								{
									bMetEveryone = false;
									break;
								}
							}
						}

						if (bMetEveryone && GC.getGame().countMajorCivsAlive() > 1)
						{
							FoundLeague(eCiv);
							break;
						}
					}
				}
			}
		}
		// Already founded - do we want to trigger a special session, or just a normal turn?
		else
		{
			EraTypes eGameEra = LeagueHelpers::GetGameEraForTrigger();
			LeagueSpecialSessionTypes eSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
			if (eGameEra > GetLastEraTrigger())
			{
				for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
				{
					CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo((LeagueSpecialSessionTypes)i);
					if (pInfo != NULL)
					{
						EraTypes eSessionTrigger = pInfo->GetEraTrigger();
						if (eSessionTrigger <= eGameEra && eSessionTrigger > GetLastEraTrigger())
						{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
							//UN needs a building to be founded.
							if(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pInfo->IsUnitedNations())
							{
								BuildingTypes eBuilding = LeagueHelpers::GetBuildingForTrigger(pInfo->GetBuildingTrigger());
								if(eBuilding != NO_BUILDING)
								{
									eSpecialSession = (LeagueSpecialSessionTypes)i;
								}
							}
							else
#endif
								eSpecialSession = (LeagueSpecialSessionTypes)i;
						}
					}
				}
			}

			CvLeague* pLeague = GetActiveLeague();
			CvAssert(pLeague != NULL);
			if (pLeague != NULL)
			{
				// We want to trigger a special session
				if (eSpecialSession != NO_LEAGUE_SPECIAL_SESSION)
				{
					// And we can
					if (pLeague->CanStartSpecialSession(eSpecialSession))
					{
						pLeague->DoTurn(eSpecialSession);

						// Flag this era as the last era we did a special session
						CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eSpecialSession);
						CvAssert(pInfo != NULL);
						if (pInfo != NULL)
						{
							SetLastEraTrigger(pInfo->GetEraTrigger());
						}

						LogSpecialSession(eSpecialSession);
					}
					// Have to wait
					else
					{
						pLeague->DoTurn();
					}
				}
				// Just a normal turn
				else
				{
					pLeague->DoTurn();
				}
			}
		}
	}
	else
	{
		VictoryTypes eDiploVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true);
		if (eDiploVictory != NO_VICTORY)
		{
			CvAssertMsg(!GC.getGame().isVictoryValid(eDiploVictory), "Diplomacy victory is valid, but leagues are disabled.  Please send Anton your save file and version.");
		}
	}
}

void CvGameLeagues::DoPlayerTurn(CvPlayer& kPlayer)
{
	if (!GC.getGame().isOption(GAMEOPTION_NO_LEAGUES))
	{
		for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
		{
			if (it->IsMember(kPlayer.GetID()))
			{
				// Call for Proposals
				if (it->CanPropose(kPlayer.GetID()))
				{
					if (kPlayer.isHuman())
					{
						CvNotifications* pNotifications = kPlayer.GetNotifications();
						if (pNotifications)
						{
							CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_LEAGUE_PROPOSALS_NEEDED");

							Localization::String strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_PROPOSALS_NEEDED_TT");
							strTemp << it->GetName();
							CvString strInfo = strTemp.toUTF8();

							pNotifications->Add(NOTIFICATION_LEAGUE_CALL_FOR_PROPOSALS, strInfo, strSummary, -1, -1, it->GetID());
						}				
					}
					else
					{
#if defined(MOD_EVENTS_RESOLUTIONS)
						bool bAllUsed = false;
						
						if (MOD_EVENTS_RESOLUTIONS) {
							if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_ResolutionProposing, kPlayer.GetID(), it->GetID()) == GAMEEVENTRETURN_TRUE) {
								bAllUsed = true;
							}
						}
						
						if (!bAllUsed)
#endif
						kPlayer.GetLeagueAI()->DoProposals(&(*it));
					}
				}
				// Call for Votes and other Session actions
				else if (it->IsInSession())
				{
					// Honor vote commitments made in deals before spending any other votes
					kPlayer.GetLeagueAI()->DoVoteCommitments(&(*it));

					// Allocate free votes
					if (it->CanVote(kPlayer.GetID()))
					{
						if (kPlayer.IsVassalOfSomeone())
						{
							for (EnactProposalList::iterator it2 = it->m_vEnactProposals.begin(); it2 != it->m_vEnactProposals.end(); ++it2)
							{
								if (it2->GetEffects()->bDiplomaticVictory)
								{
									kPlayer.GetLeagueAI()->DoAbstainAllVotes(&(*it));
									return;
								}
							}
						}

						if (it->IsAnythingProposed())
						{
							if (kPlayer.isHuman())
							{
								CvNotifications* pNotifications = kPlayer.GetNotifications();
								if (pNotifications)
								{
									CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_LEAGUE_VOTES_NEEDED");

									Localization::String strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_LEAGUE_VOTES_NEEDED_TT");
									strTemp << it->GetName();
									CvString strInfo = strTemp.toUTF8();

									pNotifications->Add(NOTIFICATION_LEAGUE_CALL_FOR_VOTES, strInfo, strSummary, -1, -1, it->GetID());
								}
							}
							else
							{
#if defined(MOD_EVENTS_RESOLUTIONS)
								bool bAllUsed = false;
								
								if (MOD_EVENTS_RESOLUTIONS && it->GetRemainingVotesForMember(kPlayer.GetID()) > 0) {
									if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_ResolutionVoting, kPlayer.GetID(), it->GetID()) == GAMEEVENTRETURN_TRUE) {
										bAllUsed = true;
									}
								}
						
								if (!bAllUsed)
#endif
								
								kPlayer.GetLeagueAI()->DoVotes(&(*it));
							}
						}
						else
						{
							kPlayer.GetLeagueAI()->DoAbstainAllVotes(&(*it));
						}
					}
				}
			}
		}
	}
}

void CvGameLeagues::FoundLeague(PlayerTypes eFounder)
{
	if (!GC.getGame().isOption(GAMEOPTION_NO_LEAGUES))
	{
		CvAssertMsg(GetNumActiveLeagues() == 0, "Trying to found a second league when one is already active. Please send Anton your save file and version.");
		if (GetNumActiveLeagues() == 0)
		{
			CvLeague league((LeagueTypes)m_iNumLeaguesEverFounded++);
			
			// Add all living players as members
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				PlayerTypes ePlayer = (PlayerTypes) i;
				if (GET_PLAYER(ePlayer).isAlive())
				{
					league.AddMember(ePlayer);
				}
			}
			
			// Optional: Set initial host
			if (eFounder != NO_PLAYER)
			{
				league.SetHostMember(eFounder);
			}

			// Find which game era trigger this league begins at (must match with a special session)
			EraTypes eEarliestEraTrigger = NO_ERA;
			EraTypes eLatestEraTrigger = NO_ERA;
			for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
			{
				CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo((LeagueSpecialSessionTypes)i);
				if (pInfo != NULL)
				{
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
					if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
						//UN needs a building to be founded.
						BuildingTypes eBuilding = LeagueHelpers::GetBuildingForTrigger(pInfo->GetBuildingTrigger());
						if(pInfo->IsUnitedNations() && eBuilding == NO_BUILDING)
						{
							continue;
						}
					}
#endif
					if (eEarliestEraTrigger == NO_ERA || (int)pInfo->GetEraTrigger() < (int)eEarliestEraTrigger)
					{
						eEarliestEraTrigger = pInfo->GetEraTrigger();
					}
					if ((int)pInfo->GetEraTrigger() > (int)eLatestEraTrigger)
					{
						eLatestEraTrigger = pInfo->GetEraTrigger();
					}
				}
			}
			// In case the game era is actually before or after the triggers in the database
			EraTypes eGoverningEraTrigger = (EraTypes) MAX((int)LeagueHelpers::GetGameEraForTrigger(), (int)eEarliestEraTrigger);
			eGoverningEraTrigger = (EraTypes) MIN((int)eGoverningEraTrigger, (int)eLatestEraTrigger);

			// Find which special session info this league begins at
			LeagueSpecialSessionTypes eGoverningSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
			bool bBeginAsUnitedNations = false;
			for (int i = 0; i < GC.getNumLeagueSpecialSessionInfos(); i++)
			{
				LeagueSpecialSessionTypes e = (LeagueSpecialSessionTypes)i;
				CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(e);
				if (pInfo != NULL)
				{
					if (pInfo->GetEraTrigger() == eGoverningEraTrigger)
					{
						eGoverningSpecialSession = e;
						bBeginAsUnitedNations = pInfo->IsUnitedNations();
						break;
					}
				}
			}
			CvAssertMsg(eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION, "Trying to found a league but could not determine a correct governing special session. Do the World Congress prerequisite tech and the special session era triggers in the database match? Please send Anton your save file and version.");
			CvAssertMsg(eGoverningEraTrigger != NO_ERA, "Trying to found a league but could not determine a correct governing era trigger. Do the World Congress prerequisite tech and the special session era triggers in the database match? Please send Anton your save file and version.");
			league.Init(eGoverningSpecialSession);
			SetLastEraTrigger(eGoverningEraTrigger);

			CvGame& kGame = GC.getGame();
			CvPlayer& kActivePlayer = GET_PLAYER(kGame.getActivePlayer());

			if(!kGame.isReallyNetworkMultiPlayer() && !kActivePlayer.isObserver())
			{
				// Show splash screen
				CvPopupInfo kPopup(BUTTONPOPUP_LEAGUE_SPLASH, league.GetID(), league.GetHostMember(), eGoverningSpecialSession, 0, /*bJustFounded*/ true);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}

			DLLUI->AddMessage(0, GC.getGame().getActivePlayer(), false, GC.getEVENT_MESSAGE_TIME(), 
													GetLocalizedText("TXT_KEY_LEAGUE_SPECIAL_SESSION_START_WORLD_CONGRESS").GetCString());

			// Begins as United Nations?
			if (bBeginAsUnitedNations)
			{
				league.SetUnitedNations(true);
			}

			m_vActiveLeagues.push_back(league);

			LogLeagueFounded(eFounder);
		}
	}
}

void CvGameLeagues::DoPlayerAliveStatusChanged(PlayerTypes ePlayer)
{
	bool bAlive = GET_PLAYER(ePlayer).isAlive();
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (!bAlive && it->IsMember(ePlayer))
		{
			it->RemoveMember(ePlayer);
		}
		else if (bAlive)
		{
			it->AddMember(ePlayer);
		}
	}

	GC.getGame().DoUpdateDiploVictory();
}

void CvGameLeagues::DoUnitedNationsBuilt(PlayerTypes /*eBuilder*/)
{
}

int CvGameLeagues::GetNumActiveLeagues()
{
	return m_vActiveLeagues.size();
}

int CvGameLeagues::GetNumLeaguesEverFounded() const
{
	return m_iNumLeaguesEverFounded;
}

EraTypes CvGameLeagues::GetLastEraTrigger() const
{
	return m_eLastEraTrigger;
}

void CvGameLeagues::SetLastEraTrigger(EraTypes eEraTrigger)
{
	m_eLastEraTrigger = eEraTrigger;
}

CvLeague* CvGameLeagues::GetLeague(LeagueTypes eLeague)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->GetID() == eLeague)
		{
			return &(*it);
		}
	}
	return NULL;
}

CvLeague* CvGameLeagues::GetActiveLeague()
{
	if (m_vActiveLeagues.empty())
		return NULL;
	else
		return &m_vActiveLeagues.front();
}

int CvGameLeagues::GenerateResolutionUniqueID()
{
	return ++m_iGeneratedIDCount;
}

bool CvGameLeagues::CanContributeToLeagueProject(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->CanMemberContribute(ePlayer, eLeagueProject))
		{
			return true;
		}
	}
	return false;
}

void CvGameLeagues::DoLeagueProjectContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject, int iValue)
{
	int iMatches = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
#if !defined(MOD_BALANCE_CORE)
		if (it->CanMemberContribute(ePlayer, eLeagueProject))
#endif
		{
			iMatches++;
			it->ChangeMemberContribution(ePlayer, eLeagueProject, iValue);
		}
	}
	CvAssertMsg(iMatches == 1, "Unexpected case when contributing to a League Project. Please send Anton your save file and version.");
}

PlayerTypes CvGameLeagues::GetDiplomaticVictor() const
{
	return m_eDiplomaticVictor;
}

void CvGameLeagues::SetDiplomaticVictor(PlayerTypes ePlayer)
{
	// There can be only one
	CvAssertMsg(m_eDiplomaticVictor == NO_PLAYER, "Player voted Diplomatic Victor when there already was one. Please send Anton your save file and version.");
	if (m_eDiplomaticVictor == NO_PLAYER)
	{
		m_eDiplomaticVictor = ePlayer;
		GC.getGame().testVictory();
	}
}

bool CvGameLeagues::IsTradeEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(eTrader))
		{
			if (it->IsTradeEmbargoed(eTrader, eRecipient))
			{
				return true;
			}
		}
	}
	return false;
}

bool CvGameLeagues::IsLuxuryHappinessBanned(PlayerTypes ePlayer, ResourceTypes eLuxury)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(ePlayer))
		{
			if (it->IsLuxuryHappinessBanned(eLuxury))
			{
				return true;
			}
		}
	}
	return false;
}

int CvGameLeagues::GetResearchMod(PlayerTypes ePlayer, TechTypes eTech)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(ePlayer))
		{
			int iLeagueMod = it->GetResearchMod(eTech);
			if (iLeagueMod != 0)
			{
				iValue += iLeagueMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetFeatureYieldChange(PlayerTypes ePlayer, FeatureTypes eFeature, YieldTypes eYield)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(ePlayer))
		{
			int iLeagueMod = it->GetFeatureYieldChange(eFeature, eYield);
			if (iLeagueMod != 0)
			{
				iValue += iLeagueMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetWorldWonderYieldChange(PlayerTypes ePlayer, YieldTypes eYield)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(ePlayer))
		{
			int iLeagueMod = it->GetWorldWonderYieldChange(eYield);
			if (iLeagueMod != 0)
			{
				iValue += iLeagueMod;
			}
		}
	}
	return iValue;
}

bool CvGameLeagues::IsNoTrainingNuclearWeapons(PlayerTypes ePlayer)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(ePlayer))
		{
			if (it->IsNoTrainingNuclearWeapons())
			{
				return true;
			}
		}
	}
	return false;
}

int CvGameLeagues::GetCityTourismModifier(PlayerTypes ePlayer, const CvCity* pCity)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iLeagueMod = it->GetCityTourismModifier(pCity);
			if (iLeagueMod != 0)
			{
				iValue += iLeagueMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetReligionSpreadStrengthModifier(PlayerTypes ePlayer, ReligionTypes eReligion)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iLeagueMod = it->GetReligionSpreadStrengthModifier(eReligion);
			if (iLeagueMod != 0)
			{
				iValue += iLeagueMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetPressureForIdeology(PlayerTypes ePlayer, PolicyBranchTypes eIdeology)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iLeagueMod = it->GetPressureForIdeology(eIdeology);
			if (iLeagueMod != 0)
			{
				iValue += iLeagueMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetArtsyGreatPersonRateModifier(PlayerTypes ePlayer)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iMod = it->GetArtsyGreatPersonRateModifier();
			if (iMod != 0)
			{
				iValue += iMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetScienceyGreatPersonRateModifier(PlayerTypes ePlayer)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iMod = it->GetScienceyGreatPersonRateModifier();
			if (iMod != 0)
			{
				iValue += iMod;
			}
		}
	}
	return iValue;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
int CvGameLeagues::GetSpaceShipProductionMod(PlayerTypes ePlayer)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iMod = it->GetSpaceShipProductionMod();
			if (iMod != 0)
			{
				iValue += iMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetSpaceShipPurchaseMod(PlayerTypes ePlayer)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iMod = it->GetSpaceShipPurchaseMod();
			if (iMod != 0)
			{
				iValue += iMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::IsWorldWar(PlayerTypes ePlayer)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(ePlayer))
		{
			int iMod = it->GetWorldWar();
			if (iMod != 0)
			{
				iValue += iMod;
			}
		}
	}
	return iValue;
}

int CvGameLeagues::GetUnitMaintenanceMod(PlayerTypes ePlayer)
{
	int iValue = 0;
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			int iMod = it->GetUnitMaintenanceMod();
			if (iMod != 0)
			{
				iValue += iMod;
			}
		}
	}
	return iValue;
}

bool CvGameLeagues::IsCityStateEmbargo(PlayerTypes ePlayer)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); ++it)
	{
		if (it->IsMember(ePlayer))
		{
			if (it->IsCityStateEmbargo())
			{
				return true;
			}
		}
	}

	return false;
}

bool CvGameLeagues::IsIdeologyEmbargoed(PlayerTypes eTrader, PlayerTypes eRecipient)
{
	for (LeagueList::iterator it = m_vActiveLeagues.begin(); it != m_vActiveLeagues.end(); it++)
	{
		if (it->IsMember(eTrader))
		{
			if (it->IsIdeologyEmbargoed(eTrader, eRecipient))
			{
				return true;
			}
		}
	}
	return false;
}
#endif

CvString CvGameLeagues::GetLogFileName() const
{
	CvString strLogName;
	strLogName = "WorldCongressLog.csv";
	return strLogName;
}

void CvGameLeagues::LogLeagueMessage(CvString& kMessage)
{
	if(GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog;

		pLog = LOGFILEMGR.GetLog(GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %d", GC.getGame().getElapsedGameTurns(), GC.getGame().getGameTurnYear());
		strOutBuf = strBaseString + kMessage;
		pLog->Msg(strOutBuf);
	}
}

void CvGameLeagues::LogLeagueFounded(PlayerTypes eFounder)
{
	CvString sMessage = "";

	sMessage += ",Congress";
	sMessage += ",- - -";
	sMessage += ",Congress Founded";
	if (eFounder != NO_PLAYER)
	{
		CvPlayer* pFounder = &GET_PLAYER(eFounder);
		if (pFounder != NULL)
		{
			sMessage += ",";
			sMessage += pFounder->getCivilizationShortDescription();
		}
	}

	LogLeagueMessage(sMessage);
}

void CvGameLeagues::LogSpecialSession(LeagueSpecialSessionTypes eSpecialSession)
{
	CvString sMessage = "";

	sMessage += ",Congress";
	sMessage += ",- - -";
	sMessage += ",Special Session";
	CvAssert(eSpecialSession != NO_LEAGUE_SPECIAL_SESSION);
	if (eSpecialSession != NO_LEAGUE_SPECIAL_SESSION)
	{
		CvLeagueSpecialSessionEntry* pSpecialSessionInfo = GC.getLeagueSpecialSessionInfo(eSpecialSession);
		CvAssert(pSpecialSessionInfo);
		if (pSpecialSessionInfo)
		{
			CvEraInfo* pEraInfo = GC.getEraInfo(pSpecialSessionInfo->GetEraTrigger());
			if (pEraInfo)
			{
				sMessage += ",";
				sMessage += pEraInfo->GetDescription();
			}
		}
	}

	LogLeagueMessage(sMessage);
}

template<typename GameLeagues, typename Visitor>
void CvGameLeagues::Serialize(GameLeagues& gameLeagues, Visitor& visitor)
{
	visitor(gameLeagues.m_iGeneratedIDCount);
	visitor(gameLeagues.m_vActiveLeagues);
	visitor(gameLeagues.m_iNumLeaguesEverFounded);
	visitor(gameLeagues.m_eDiplomaticVictor);
	visitor(gameLeagues.m_eLastEraTrigger);
}

// Serialization Read
FDataStream& operator>>(FDataStream& loadFrom, CvGameLeagues& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGameLeagues::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// Serialization Write
FDataStream& operator<<(FDataStream& saveTo, const CvGameLeagues& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGameLeagues::Serialize(readFrom, serialVisitor);
	return saveTo;
}

// ================================================================================
//			CvLeagueAI
// ================================================================================
CvLeagueAI::CvLeagueAI(void)
{
	m_pPlayer = NULL;
}

CvLeagueAI::~CvLeagueAI(void)
{
	Uninit();
}

// Initialize class data
void CvLeagueAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	Reset();
}

// Cleanup
void CvLeagueAI::Uninit()
{

}

void CvLeagueAI::Reset()
{
	m_vVoteCommitmentList.clear();
}

template<typename VoteCommitmentT, typename Visitor>
void CvLeagueAI::VoteCommitment::Serialize(VoteCommitmentT& voteCommitment, Visitor& visitor)
{
	visitor(voteCommitment.eToPlayer);
	visitor(voteCommitment.iResolutionID);
	visitor(voteCommitment.iVoteChoice);
	visitor(voteCommitment.iNumVotes);
	visitor(voteCommitment.bEnact);
}

FDataStream& operator>>(FDataStream& stream, CvLeagueAI::VoteCommitment& voteCommitment)
{
	CvStreamLoadVisitor serialVisitor(stream);
	CvLeagueAI::VoteCommitment::Serialize(voteCommitment, serialVisitor);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvLeagueAI::VoteCommitment& voteCommitment)
{
	CvStreamSaveVisitor serialVisitor(stream);
	CvLeagueAI::VoteCommitment::Serialize(voteCommitment, serialVisitor);
	return stream;
}

template<typename LeagueAI, typename Visitor>
void CvLeagueAI::Serialize(LeagueAI& leagueAI, Visitor& visitor)
{
	visitor(leagueAI.m_vVoteCommitmentList);
}

void CvLeagueAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

void CvLeagueAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvLeagueAI& leagueAI)
{
	leagueAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvLeagueAI& leagueAI)
{
	leagueAI.Write(stream);
	return stream;
}

void CvLeagueAI::DoTurn()
{
	if (m_pPlayer->isHuman())
	{
		return;
	}

	// Evaluate current situation

	// Queue requests for Diplo system
}

void CvLeagueAI::DoVotes(CvLeague* pLeague)
{
	//AI_PERF_FORMAT("AI-perf.csv", ("CvLeagueAI::DoVotes, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription()) );

	int iAttempts = 0;
	while (pLeague->CanVote(GetPlayer()->GetID()))
	{
		AllocateVotes(pLeague);

		iAttempts++;
		if (iAttempts > 100)
		{
			CvAssertMsg(false, "AI unable to finish doing votes. Please send Anton your save file and version.");
			DoAbstainAllVotes(pLeague);
			break;
		}
	}
}

void CvLeagueAI::DoAbstainAllVotes(CvLeague* pLeague)
{
	pLeague->DoVoteAbstain(GetPlayer()->GetID(), pLeague->GetRemainingVotesForMember(GetPlayer()->GetID()));
}

void CvLeagueAI::DoProposals(CvLeague* pLeague)
{
	int iAttempts = 0;
	while (pLeague->CanPropose(GetPlayer()->GetID()))
	{
		AllocateProposals(pLeague);
		iAttempts++;
		if (iAttempts > 100)
		{
			CvAssertMsg(false, "AI unable to finish doing proposals. Please send Anton your save file and version.");
			break;
		}
	}
}

CvLeagueAI::VoteCommitmentList CvLeagueAI::GetDesiredVoteCommitments(PlayerTypes eFromPlayer, bool bFlippedLogic)
{
	VoteCommitmentList vDesired;

	if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague)
		{
			// For human players, make assumptions
			if (GetPlayer()->isHuman())
			{
				EnactProposalList vEnactProposals = pLeague->GetEnactProposals();
				for (EnactProposalList::iterator it = vEnactProposals.begin(); it != vEnactProposals.end(); ++it)
				{
					int iDesiredChoice = LeagueHelpers::CHOICE_NONE;

					// Normal Proposals
					if (it->GetVoterDecision()->GetType() == RESOLUTION_DECISION_YES_OR_NO)
					{
						if (it->GetProposalPlayer() == GetPlayer()->GetID())
						{
							iDesiredChoice = LeagueHelpers::CHOICE_YES;
						}
						if (it->GetEffects()->bEmbargoPlayer && it->GetProposerDecision()->GetDecision() == GetPlayer()->GetID())
						{
							iDesiredChoice = LeagueHelpers::CHOICE_NO;
						}
					}
					// Proposals voting on a player
					else if (it->GetVoterDecision()->GetType() == RESOLUTION_DECISION_ANY_MEMBER ||
							it->GetVoterDecision()->GetType() == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
							it->GetVoterDecision()->GetType() == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
					{
						if (it->GetEffects()->bChangeLeagueHost)
						{
							iDesiredChoice = GetPlayer()->GetID();
						}
						if (it->GetEffects()->bDiplomaticVictory)
						{
							iDesiredChoice = GetPlayer()->GetID();
						}
					}
					else
					{
						CvAssertMsg(false, "Unexpected case when evaluating a possible Delegate trade with a human player. Please send Anton your save file and version.");
					}

					if (iDesiredChoice != LeagueHelpers::CHOICE_NONE)
					{
						VoteCommitment temp;
						temp.iResolutionID = it->GetID();
						int iVotes = 0;
						if (bFlippedLogic)
						{
							iVotes =pLeague->GetPotentialVotesForMember(eFromPlayer, GetPlayer()->GetID());
						}
						else
						{
							iVotes = pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eFromPlayer);
						}	
						temp.iNumVotes = iVotes;
						temp.iVoteChoice = iDesiredChoice;
						temp.bEnact = true;
						vDesired.push_back(temp);
					}
				}

				RepealProposalList vRepealProposals = pLeague->GetRepealProposals();
				for (RepealProposalList::iterator it = vRepealProposals.begin(); it != vRepealProposals.end(); ++it)
				{
					int iDesiredChoice = LeagueHelpers::CHOICE_NONE;

					// Normal Proposals
					if (it->GetRepealDecision()->GetType() == RESOLUTION_DECISION_REPEAL)
					{
						if (it->GetProposalPlayer() == GetPlayer()->GetID())
						{
							iDesiredChoice = LeagueHelpers::CHOICE_YES;
						}
						if (it->GetEffects()->bEmbargoPlayer && it->GetProposerDecision()->GetDecision() == GetPlayer()->GetID())
						{
							iDesiredChoice = LeagueHelpers::CHOICE_YES;
						}
					}
					else
					{
						CvAssertMsg(false, "Unexpected case when evaluating a possible Delegate trade with a human player. Please send Anton your save file and version.");
					}

					if (iDesiredChoice != LeagueHelpers::CHOICE_NONE)
					{
						VoteCommitment temp;
						temp.iResolutionID = it->GetID();
						int iVotes = 0;
						if (bFlippedLogic)
						{
							iVotes = pLeague->GetPotentialVotesForMember(eFromPlayer, GetPlayer()->GetID());
						}
						else
						{
							iVotes = pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eFromPlayer);
						}
						temp.iNumVotes = iVotes;
						temp.iVoteChoice = iDesiredChoice;
						temp.bEnact = false;
						vDesired.push_back(temp);
					}
				}
			}
			// For AI players, use calculated value
			else
			{
				EnactProposalList vEnactProposals = pLeague->GetEnactProposals();
				for (EnactProposalList::iterator it = vEnactProposals.begin(); it != vEnactProposals.end(); ++it)
				{
					std::vector<int> vChoices = pLeague->GetChoicesForDecision(it->GetVoterDecision()->GetType(), eFromPlayer);
					int iDesiredChoice = LeagueHelpers::CHOICE_NONE;
					DesireLevels eHighestDesire = DESIRE_NEUTRAL;
					for (uint i = 0; i < vChoices.size(); i++)
					{
						int iChoice = vChoices[i];
						int iVotes = 0;
						if (bFlippedLogic)
						{
							iVotes = pLeague->GetPotentialVotesForMember(eFromPlayer, GetPlayer()->GetID());
						}
						else
						{
							iVotes = pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eFromPlayer);
						}
						DesireLevels eDesire = EvaluateVoteForTrade(it->GetID(), iChoice, iVotes, /*bRepeal*/false);

						if (eDesire > eHighestDesire)
						{
							iDesiredChoice = iChoice;
							eHighestDesire = eDesire;
						}
					}

					if (iDesiredChoice != LeagueHelpers::CHOICE_NONE)
					{
						VoteCommitment temp;
						temp.iResolutionID = it->GetID();
						int iVotes = 0;
						if (bFlippedLogic)
						{
							iVotes = pLeague->GetPotentialVotesForMember(eFromPlayer, GetPlayer()->GetID());
						}
						else
						{
							iVotes = pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eFromPlayer);
						}
						temp.iNumVotes = iVotes;
						temp.iVoteChoice = iDesiredChoice;
						temp.bEnact = true;
						vDesired.push_back(temp);
					}
				}

				RepealProposalList vRepealProposals = pLeague->GetRepealProposals();
				for (RepealProposalList::iterator it = vRepealProposals.begin(); it != vRepealProposals.end(); ++it)
				{
					std::vector<int> vChoices = pLeague->GetChoicesForDecision(it->GetRepealDecision()->GetType(), eFromPlayer);
					int iDesiredChoice = LeagueHelpers::CHOICE_NONE;
					DesireLevels eHighestDesire = DESIRE_NEUTRAL;
					for (uint i = 0; i < vChoices.size(); i++)
					{
						int iChoice = vChoices[i];
						int iVotes = 0;
						if (bFlippedLogic)
						{
							iVotes = pLeague->GetPotentialVotesForMember(eFromPlayer, GetPlayer()->GetID());
						}
						else
						{
							iVotes = pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eFromPlayer);
						}
						DesireLevels eDesire = EvaluateVoteForTrade(it->GetID(), iChoice, iVotes, /*bRepeal*/true);

						if (eDesire > eHighestDesire)
						{
							iDesiredChoice = iChoice;
							eHighestDesire = eDesire;
						}
					}

					if (iDesiredChoice != LeagueHelpers::CHOICE_NONE)
					{
						VoteCommitment temp;
						temp.iResolutionID = it->GetID();

						int iVotes = 0;
						if (bFlippedLogic)
						{
							iVotes = pLeague->GetPotentialVotesForMember(eFromPlayer, GetPlayer()->GetID());
						}
						else
						{
							iVotes = pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eFromPlayer);
						}
						temp.iNumVotes = iVotes;
						temp.iVoteChoice = iDesiredChoice;
						temp.bEnact = false;
						vDesired.push_back(temp);
					}
				}
			}
		}
	}

	return vDesired;
}

bool CvLeagueAI::HasVoteCommitment() const
{
	return (!m_vVoteCommitmentList.empty());
}

// Have we committed votes to a player on a particular proposal?
int CvLeagueAI::GetVoteCommitment(PlayerTypes eToPlayer, int iResolutionID, int iVoteChoice, bool bEnact)
{
	int iNumVotes = 0;
	for (VoteCommitmentList::iterator it = m_vVoteCommitmentList.begin(); it != m_vVoteCommitmentList.end(); ++it)
	{
		if (it->eToPlayer == eToPlayer && it->iResolutionID == iResolutionID && it->iVoteChoice == iVoteChoice && it->bEnact == bEnact)
		{
			iNumVotes += it->iNumVotes;
		}
	}
	return iNumVotes;
}

// Can someone secure our vote on a certain matter through a deal?
bool CvLeagueAI::CanCommitVote(PlayerTypes eToPlayer, CvString* sTooltipSink)
{
	CvAssertMsg(eToPlayer >= 0, "eToPlayer is expected to be non-negative (invalid Index). Please send Anton your save file and version.");
	CvAssertMsg(eToPlayer < MAX_MAJOR_CIVS, "eToPlayer is expected to be within maximum bounds (invalid Index). Please send Anton your save file and version.");
	if (eToPlayer < 0 || eToPlayer >= MAX_MAJOR_CIVS) return false;

	bool bCanCommit = true;
	
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague == NULL)
	{
		bCanCommit = false;
		if (sTooltipSink != NULL)
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_NO_LEAGUE").toUTF8();
	}
	else
	{
		// League is in session
		if (pLeague->IsInSession())
		{
			bCanCommit = false;
			if (sTooltipSink != NULL)
			{
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_IN_SESSION").toUTF8();
			}
		}

		// They don't have a diplomat in our capital
		if (!DEBUG_LEAGUES)
		{
			if (!GetPlayer()->GetEspionage()->IsOtherDiplomatVisitingMe(eToPlayer))
			{
				bCanCommit = false;
				if (sTooltipSink != NULL)
				{
					if (GC.getGame().getActivePlayer() == GetPlayer()->GetID())
					{
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_NO_SPY_ACTIVE_PLAYER").toUTF8();
					}
					else
					{
						(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_NO_SPY").toUTF8();
					}
				}
			}
		}

		// Not a member
		if (!pLeague->IsMember(GetPlayer()->GetID()))
		{
			bCanCommit = false;
			if (sTooltipSink != NULL)
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_NOT_MEMBER").toUTF8();
		}

		// Not allowed to vote
		if (!pLeague->CanEverVote(GetPlayer()->GetID()))
		{
			bCanCommit = false;
		}

		// No proposals made yet
		if (!pLeague->IsAnythingProposed())
		{
			bCanCommit = false;
			if (sTooltipSink != NULL)
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_NO_PROPOSALS").toUTF8();
		}

		// We already committed our votes
		if (HasVoteCommitment())
		{
			bCanCommit = false;
			if (sTooltipSink != NULL)
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_ALREADY_COMMITTED").toUTF8();
		}

		// The league is in session, and we already used our votes
		if (pLeague->IsInSession() && pLeague->GetRemainingVotesForMember(GetPlayer()->GetID()) < pLeague->GetPotentialVotesForMember(GetPlayer()->GetID(), eToPlayer))
		{
			bCanCommit = false;
			if (sTooltipSink != NULL)
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_DIPLO_VOTE_TRADE_ALREADY_USED_VOTES").toUTF8();
		}
	}

	return bCanCommit;
}

// Someone secured our vote on a certain matter through a deal
void CvLeagueAI::AddVoteCommitment(PlayerTypes eToPlayer, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal)
{
	VoteCommitment sCommitment(eToPlayer, iResolutionID, iVoteChoice, iNumVotes, !bRepeal);
	m_vVoteCommitmentList.push_back(sCommitment);
}

// Something caused all commitments to be cancelled (ie. war)
void CvLeagueAI::CancelVoteCommitmentsToPlayer(PlayerTypes eToPlayer)
{
	for (VoteCommitmentList::iterator it = m_vVoteCommitmentList.begin(); it != m_vVoteCommitmentList.end(); ++it)
	{
		if (it->eToPlayer == eToPlayer)
		{
			m_vVoteCommitmentList.erase(it);
			it--;
		}
	}
}

// Honor our vote commitments
void CvLeagueAI::DoVoteCommitments(CvLeague* pLeague)
{
	for (VoteCommitmentList::iterator it = m_vVoteCommitmentList.begin(); it != m_vVoteCommitmentList.end(); ++it)
	{
		CvAssertMsg(pLeague->CanVote(GetPlayer()->GetID()), "Trying to honor vote commitments but not able to vote. Please send Anton your save file and version.");
		CvAssertMsg(pLeague->GetRemainingVotesForMember(GetPlayer()->GetID()) >= it->iNumVotes, "Trying to honor vote commitments but not enough votes. Please send Anton your save file and version.");
		bool bProcessed = false;

		int iSetVotes = pLeague->CalculateStartingVotesForMember(GetPlayer()->GetID());
		if(it->iNumVotes > iSetVotes)
		{
			if (pLeague->CanVote(GetPlayer()->GetID()))
			{
				// Proposal is up, time to honor the commitment
				if (pLeague->IsProposed(it->iResolutionID, !it->bEnact, /*bCheckOnHold*/ false))
				{
					if (it->bEnact)
					{
						pLeague->DoVoteEnact(it->iResolutionID, GetPlayer()->GetID(), iSetVotes, it->iVoteChoice);
						bProcessed = true;
					}
					else
					{
						pLeague->DoVoteRepeal(it->iResolutionID, GetPlayer()->GetID(), iSetVotes, it->iVoteChoice);
						bProcessed = true;
					}
				}
				// Proposal is on hold (ex. special session), so keep the commitment for once it comes back up
				else if (pLeague->IsProposed(it->iResolutionID, !it->bEnact, /*bCheckOnHold*/ true))
				{	
				}
				// Proposal is no longer around (ex. invalidated by a player being killed)
				else
				{
					bProcessed = true;
				}
			}
		}
		else if (pLeague->CanVote(GetPlayer()->GetID()))
		{
			// Proposal is up, time to honor the commitment
			if (pLeague->IsProposed(it->iResolutionID, !it->bEnact, /*bCheckOnHold*/ false))
			{
				if (it->bEnact)
				{
					pLeague->DoVoteEnact(it->iResolutionID, GetPlayer()->GetID(), it->iNumVotes, it->iVoteChoice);
					bProcessed = true;
				}
				else
				{
					pLeague->DoVoteRepeal(it->iResolutionID, GetPlayer()->GetID(), it->iNumVotes, it->iVoteChoice);
					bProcessed = true;
				}
			}
			// Proposal is on hold (ex. special session), so keep the commitment for once it comes back up
			else if (pLeague->IsProposed(it->iResolutionID, !it->bEnact, /*bCheckOnHold*/ true))
			{	
			}
			// Proposal is no longer around (ex. invalidated by a player being killed)
			else
			{
				bProcessed = true;
			}
		}

		// Cleanup
		if (bProcessed)
		{
			m_vVoteCommitmentList.erase(it);
			it--;
		}
	}
}

// How much do we like this vote commitment (either from us to someone else, or from someone else to us)?
CvLeagueAI::DesireLevels CvLeagueAI::EvaluateVoteForTrade(int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal)
{
	DEBUG_VARIABLE(iNumVotes);
	DesireLevels eValue = DESIRE_NEVER;
	
	if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		CvAssert(pLeague != NULL);
		if (pLeague != NULL)
		{
			CvAssert(iNumVotes == pLeague->GetCoreVotesForMember(GetPlayer()->GetID()));
			if (pLeague->IsProposed(iResolutionID, bRepeal))
			{
				// Vote to repeal
				if (bRepeal)
				{
					RepealProposalList vProposals = pLeague->GetRepealProposals();
					for (RepealProposalList::iterator it = vProposals.begin(); it != vProposals.end(); ++it)
					{
						if (it->GetID() == iResolutionID)
						{
							eValue = EvaluateDesire(ScoreVoteChoice(&(*it), iVoteChoice, /*bConsiderGlobal*/ true));
							break;
						}
					}
				}
				// Vote to enact
				else
				{
					EnactProposalList vProposals = pLeague->GetEnactProposals();
					for (EnactProposalList::iterator it = vProposals.begin(); it != vProposals.end(); ++it)
					{
						if (it->GetID() == iResolutionID)
						{
							eValue = EvaluateDesire(ScoreVoteChoice(&(*it), iVoteChoice, /*bConsiderGlobal*/ true));
							break;
						}
					}
				}
			}
		}
	}

	return eValue;
}

// How much do we like an enact proposal, so that we can give a hint to the player making proposals?
CvLeagueAI::DesireLevels CvLeagueAI::EvaluateProposalForProposer(CvLeague* pLeague, PlayerTypes /*eProposer*/, ResolutionTypes eResolution, int iProposerChoice)
{
	CvAssert(pLeague);
	if (!pLeague)
	{
		return DESIRE_NEUTRAL;
	}
	DesireLevels eDesire = EvaluateDesire(ScoreProposal(pLeague, eResolution, iProposerChoice));

	return eDesire;
}

// How much do we like a repeal proposal, so that we can give a hint to the player making proposals?
CvLeagueAI::DesireLevels CvLeagueAI::EvaluateProposalForProposer(CvLeague* pLeague, PlayerTypes /*eProposer*/, int iTargetResolutionID)
{
	CvAssert(pLeague);
	if (!pLeague)
	{
		return DESIRE_NEUTRAL;
	}

	DesireLevels eDesire = DESIRE_NEUTRAL;
	bool bFound = false;
	ActiveResolutionList vActiveResolutions = pLeague->GetActiveResolutions();
	for (ActiveResolutionList::iterator it = vActiveResolutions.begin(); it != vActiveResolutions.end(); ++it)
	{
		if (it->GetID() == iTargetResolutionID)
		{
			eDesire = EvaluateDesire(ScoreProposal(pLeague, &(*it)));
			bFound = true;
			break;
		}
	}
	CvAssert(bFound);

	return eDesire;
}

// Calculate how much we think our interests are aligned with ePlayer, based on ideology, liberation, past voting patterns, etc.
CvLeagueAI::AlignmentLevels CvLeagueAI::EvaluateAlignment(PlayerTypes ePlayer, bool bIgnoreWar)
{
	int iAlignment = 0;
	CvDiplomacyAI* pDiplo = GetPlayer()->GetDiplomacyAI();

	if (ePlayer == NO_PLAYER||!GET_PLAYER(ePlayer).isAlive()||GET_PLAYER(ePlayer).isMinorCiv())
	{
		return ALIGNMENT_NEUTRAL;
	}
	if (GetPlayer()->GetID() == ePlayer)
	{
		return ALIGNMENT_SELF;
	}
	if (GetPlayer()->getTeam() == GET_PLAYER(ePlayer).getTeam())
	{
		if (GET_TEAM(GetPlayer()->getTeam()).getLeaderID() == ePlayer)
		{
			return ALIGNMENT_LEADER;
		}
		else
		{
			return ALIGNMENT_TEAMMATE;
		}
	}
	if (GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		if (!bIgnoreWar && pDiplo->IsWantsToConquer(ePlayer))
		{
			return ALIGNMENT_WAR;
		}
		else
		{
			if (pDiplo->IsPhonyWar(ePlayer))
			{
				iAlignment -= 1;
			}
			else
			{
				iAlignment -= 2;
			}
		}
	}


	bool bUntrustworthy = pDiplo->IsUntrustworthy(ePlayer);

	// Ideology
	PolicyBranchTypes eOurIdeology = GetPlayer()->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	if (eOurIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE)
	{
		int iIdeologyMod = 0;

		if (eOurIdeology == eTheirIdeology)
		{
			iIdeologyMod += 2;
		}
		else if (!pDiplo->IsIgnoreIdeologyDifferences(ePlayer))
		{
			iIdeologyMod -= 2;
		}

		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague != NULL)
		{
			// Is this the World Ideology?
			if ((pLeague->GetPressureForIdeology(eOurIdeology) > 0) || (pLeague->GetPressureForIdeology(eTheirIdeology) > 0))
			{
				iIdeologyMod *= 2;
			}
		}

		// Are either of us a vassal?
		if (GetPlayer()->IsVassalOfSomeone())
		{
			iIdeologyMod /= 2;
		}
		if (GET_PLAYER(ePlayer).IsVassalOfSomeone())
		{
			iIdeologyMod /= 2;
		}

		iAlignment += iIdeologyMod;
	}

	// Religion
	ReligionTypes eOurReligion = GetPlayer()->GetReligions()->GetCurrentReligion(false) != NO_RELIGION ? GetPlayer()->GetReligions()->GetCurrentReligion(false) : GetPlayer()->GetReligions()->GetReligionInMostCities();
	ReligionTypes eTheirReligion = (GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion(false) != NO_RELIGION && GC.getGame().GetGameReligions()->HasAnyDomesticCityFollowing(GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion(false), ePlayer)) ? GET_PLAYER(ePlayer).GetReligions()->GetCurrentReligion(false) : GET_PLAYER(ePlayer).GetReligions()->GetReligionInMostCities();
	if (eOurReligion != NO_RELIGION && eTheirReligion != NO_RELIGION)
	{
		int iReligionMod = 0;

		if (eOurReligion == eTheirReligion && !pDiplo->IsHolyCityCapturedBy(ePlayer))
		{
			iReligionMod += 1;
		}
		else if (!pDiplo->IsIgnoreReligionDifferences(ePlayer))
		{
			iReligionMod -= 1;
		}

		// Is this the World Religion?
		if ((GC.getGame().GetGameLeagues()->GetReligionSpreadStrengthModifier(ePlayer, eTheirReligion) > 0) || (GC.getGame().GetGameLeagues()->GetReligionSpreadStrengthModifier(GetPlayer()->GetID(), eOurReligion) > 0))
		{
			iReligionMod *= 2;
		}

		iAlignment += iReligionMod;
	}

	// Did they help us become host?
	if (pDiplo->GetSupportedMyHostingScore(ePlayer) != 0)
	{
		if (pDiplo->GetSupportedMyHostingScore(ePlayer) < -40)
		{
			iAlignment += 2;
		}
		else
		{
			iAlignment += 1;
		}
	}

	// Do we like their proposals or votes?
	if (pDiplo->GetLikedTheirProposalScore(ePlayer) < 0)
	{
		if (pDiplo->GetLikedTheirProposalScore(ePlayer) > -20)
		{
			iAlignment += 1;
		}
		else
		{
			iAlignment += 2;
		}
	}
	if (pDiplo->GetDislikedTheirProposalScore(ePlayer) > 0)
	{
		if (pDiplo->GetDislikedTheirProposalScore(ePlayer) < 20)
		{
			iAlignment -= 1;
		}
		else
		{
			iAlignment -= 2;
		}
	}

	// Have they supported our proposals?
	int iSupportedOurProposalScore = pDiplo->GetSupportedOurProposalScore(ePlayer);
	if (iSupportedOurProposalScore > 0)
	{
		if (pDiplo->IsFoiledOurProposalAndThenSupportedUs(ePlayer))
		{
			iAlignment -= 1;
		}
		else
		{
			iAlignment -= 2;
		}
	}
	else if (iSupportedOurProposalScore < 0)
	{
		if (pDiplo->IsSupportedOurProposalAndThenFoiledUs(ePlayer))
		{
			iAlignment += 1;
		}
		else
		{
			iAlignment += 2;
		}
	}

	// DoF or Denounce
	if (pDiplo->IsDenouncedPlayer(ePlayer) || pDiplo->IsDenouncedByPlayer(ePlayer))
	{
		iAlignment -= 2;
	}
	else if ((pDiplo->IsDoFAccepted(ePlayer) || pDiplo->IsWantsDoFWithPlayer(ePlayer)) && !pDiplo->IsWantsToEndDoFWithPlayer(ePlayer))
	{
		switch (pDiplo->GetDoFType(ePlayer))
		{
		case DOF_TYPE_FRIENDS:
			iAlignment += 1;
			break;
		case DOF_TYPE_ALLIES:
			iAlignment += 2;
			break;
		case DOF_TYPE_BATTLE_BROTHERS:
			iAlignment += 3;
			break;
		}
	}

	// Defensive Pact?
	if (pDiplo->IsHasDefensivePact(ePlayer) && !pDiplo->IsWantsToEndDefensivePactWithPlayer(ePlayer))
	{
		iAlignment += 2;
	}

	// Current vassal?
	if (pDiplo->IsVassal(ePlayer))
	{
		VassalTreatmentTypes eTreatmentLevel = pDiplo->GetVassalTreatmentLevel(ePlayer);

		if (pDiplo->IsVoluntaryVassalage(ePlayer))
		{
			switch (eTreatmentLevel)
			{
			case VASSAL_TREATMENT_CONTENT:
				iAlignment += 4;
				break;
			case VASSAL_TREATMENT_DISAGREE:
				iAlignment += 2;
				break;
			case VASSAL_TREATMENT_MISTREATED:
				iAlignment -= 0;
				break;
			case VASSAL_TREATMENT_UNHAPPY:
				iAlignment -=  2;
				break;
			case VASSAL_TREATMENT_ENSLAVED:
				iAlignment -=  4;
				break;
			}
		}
		else
		{
			switch (eTreatmentLevel)
			{
			case VASSAL_TREATMENT_CONTENT:
				iAlignment += 2;
				break;
			case VASSAL_TREATMENT_DISAGREE:
				iAlignment -= 1;
				break;
			case VASSAL_TREATMENT_MISTREATED:
				iAlignment -=  2;
				break;
			case VASSAL_TREATMENT_UNHAPPY:
				iAlignment -=  3;
				break;
			case VASSAL_TREATMENT_ENSLAVED:
				iAlignment -=  5;
				break;
			}
		}
	}
	else
	{
		// Previous vassalage?
		if (pDiplo->IsMasterLiberatedMeFromVassalage(ePlayer))
		{
			iAlignment += 2;
		}
		else if (pDiplo->IsHappyAboutPlayerVassalagePeacefullyRevoked(ePlayer))
		{
			iAlignment += 1;
		}
		else if (pDiplo->IsAngryAboutPlayerVassalageForcefullyRevoked(ePlayer))
		{
			iAlignment -= 1;
		}
	}

	// Competing for City-States?
	if (pDiplo->GetMinorCivDisputeLevel(ePlayer) != NO_DISPUTE_LEVEL)
	{
		iAlignment -= pDiplo->GetMinorCivDisputeLevel(ePlayer);
	}


	switch (pDiplo->GetCivApproach(ePlayer))
	{
	case CIV_APPROACH_WAR:
	case CIV_APPROACH_HOSTILE:
		iAlignment -= 2;
		break;
	case CIV_APPROACH_DECEPTIVE:
	case CIV_APPROACH_GUARDED:
		iAlignment -= 1;
		break;
	case CIV_APPROACH_AFRAID:
	case CIV_APPROACH_NEUTRAL:
		iAlignment += 0;
		break;
	case CIV_APPROACH_FRIENDLY:
		iAlignment += 1;
		break;
	}

	if (pDiplo->GetWarmongerThreat(ePlayer) > 0)
	{
		iAlignment -= pDiplo->GetWarmongerThreat(ePlayer);
	}
	if (pDiplo->GetCoopWarScore(ePlayer) > 0)
	{
		iAlignment += pDiplo->GetCoopWarScore(ePlayer);
	}

	// Backstabber?
	if (bUntrustworthy)
	{
		iAlignment -= 4;

	}
	// Liberator?
	if (pDiplo->IsPlayerLiberatedCapital(ePlayer) || pDiplo->IsPlayerLiberatedHolyCity(ePlayer) || pDiplo->WasResurrectedBy(ePlayer))
	{
		if (!pDiplo->IsCompetingForVictory() && !bUntrustworthy && !GetPlayer()->IsAtWarWith(ePlayer))
		{
			iAlignment += 4;
		}

		else
		{
			iAlignment += 2;
		}
	}
	if (!GetPlayer()->IsAtWarWith(ePlayer) && pDiplo->GetNumCitiesLiberatedBy(ePlayer) > 0)
	{
		iAlignment += pDiplo->GetNumCitiesLiberatedBy(ePlayer);
		if (pDiplo->IsPlayerReturnedCapital(ePlayer) || pDiplo->IsPlayerReturnedHolyCity(ePlayer))
		{
			iAlignment += 1;
		}
	}

	// Captured some of our cities before?
	if (pDiplo->GetNumCitiesCapturedBy(ePlayer) > 0)
	{
		iAlignment -= pDiplo->GetNumCitiesCapturedBy(ePlayer);
	}

	int iDisputeLevel = max((int)pDiplo->GetVictoryDisputeLevel(ePlayer), (int)pDiplo->GetVictoryBlockLevel(ePlayer));
	if (iDisputeLevel > 0)
	{
		iAlignment -= iDisputeLevel;
	}

	AlignmentLevels eAlignment = ALIGNMENT_NEUTRAL;
	if (iAlignment < -8)
	{
		eAlignment = ALIGNMENT_ENEMY;
	}
	else if (iAlignment < -5)
	{
		eAlignment = ALIGNMENT_HATRED;
	}
	else if (iAlignment < -2)
	{
		eAlignment = ALIGNMENT_RIVAL;
	}
	else if (iAlignment <= 2)
	{
		eAlignment = ALIGNMENT_NEUTRAL;
	}
	else if (iAlignment <= 5)
	{
		eAlignment = ALIGNMENT_FRIEND;
	}
	else if (iAlignment <= 8)
	{
		eAlignment = ALIGNMENT_CONFIDANT;
	}
	else if (pDiplo->WasResurrectedBy(ePlayer))
	{
		eAlignment = ALIGNMENT_LIBERATOR;
	}
	else
	{
		eAlignment = ALIGNMENT_ALLY;
	}
	return eAlignment;
}

CvLeagueAI::KnowledgeLevels CvLeagueAI::GetKnowledgeGivenToOtherPlayer(PlayerTypes eToPlayer, CvString* sTooltipSink)
{
	// Teammates or Debug Mode
	bool bOverride = GetPlayer()->getTeam() == GET_PLAYER(eToPlayer).getTeam() || GC.getGame().IsDiploDebugModeEnabled() || DEBUG_LEAGUES;

	// Shared Ideology
	PolicyBranchTypes eMyIdeology = GetPlayer()->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = GET_PLAYER(eToPlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	bool bShareIdeology = ((eMyIdeology == eTheirIdeology) && (eMyIdeology != NO_POLICY_BRANCH_TYPE) && (eTheirIdeology != NO_POLICY_BRANCH_TYPE));

	// Espionage
	bool bSpyVisitingUs = GetPlayer()->GetEspionage()->IsOtherDiplomatVisitingMe(eToPlayer);

	// Find the knowledge level
	KnowledgeLevels eKnowledge = KNOWLEDGE_NONE;
	if (bShareIdeology && bSpyVisitingUs)
	{
		eKnowledge = KNOWLEDGE_INTIMATE;
	}
	else if (bShareIdeology || bSpyVisitingUs)
	{
		eKnowledge = KNOWLEDGE_PARTIAL;
	}

	// Exceptions
	if (bOverride)
	{
		eKnowledge = KNOWLEDGE_INTIMATE;
	}
	if (GetPlayer()->isHuman())
	{
		// Human player intentions are never known
		eKnowledge = KNOWLEDGE_NONE;
	}

	// Tooltip
	if (sTooltipSink != NULL)
	{
		switch (eKnowledge)
		{
		case KNOWLEDGE_INTIMATE:
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_INTIMATE").toUTF8();
			break;
		case KNOWLEDGE_PARTIAL:
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_PARTIAL").toUTF8();
			break;
		case KNOWLEDGE_NONE:
		default:
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_NONE").toUTF8();
			break;
		}

		if (!bOverride && !GetPlayer()->isHuman())
		{
			if (bShareIdeology)
			{
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_IDEOLOGY").toUTF8();
			}
			else
			{
				(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_NO_IDEOLOGY").toUTF8();
			}
		}

		if (bSpyVisitingUs)
		{
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_SPY").toUTF8();
		}
		else
		{
			(*sTooltipSink) += Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_INTRIGUE_NO_SPY").toUTF8();
		}
	}

	return eKnowledge;
}

int CvLeagueAI::EvaluateVoteForOtherPlayerKnowledge(CvLeague* pLeague, PlayerTypes eToPlayer, CvEnactProposal* pProposal, CvString* sTooltipSink)
{
	CvAssert(pLeague);
	CvAssert(pProposal);
	if (!pLeague || !pProposal)
	{
		return LeagueHelpers::CHOICE_NONE;
	}

	// How much are we telling them?
	int iRevealedChoice = LeagueHelpers::CHOICE_NONE;
	KnowledgeLevels eKnowledge = GetKnowledgeGivenToOtherPlayer(eToPlayer);
	switch (eKnowledge)
	{
	case KNOWLEDGE_NONE:
		{
			if (sTooltipSink != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_NONE");
				sTemp << pProposal->GetProposalName();
				(*sTooltipSink) += sTemp.toUTF8();
			}
			break;
		}
	case KNOWLEDGE_PARTIAL:
		{	
		// What is our preferred choice on this proposal?
		int iTopChoice = LeagueHelpers::CHOICE_NONE;
		int iTopChoiceScore = MIN_INT;
		std::vector<int> vChoices = pLeague->GetChoicesForDecision(pProposal->GetVoterDecision()->GetType(), GetPlayer()->GetID());
		for (uint i = 0; i < vChoices.size(); i++)
		{
			int iChoice = vChoices[i];
			int iChoiceScore = ScoreVoteChoice(pProposal, iChoice, /*bConsiderGlobal*/ true);
			if (iChoiceScore > iTopChoiceScore)
			{
				iTopChoice = iChoice;
				iTopChoiceScore = iChoiceScore;
			}
		}
		CvAssert(iTopChoice != LeagueHelpers::CHOICE_NONE);
			iRevealedChoice = iTopChoice;
			if (sTooltipSink != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_PARTIAL");
				sTemp << pProposal->GetProposalName();
				sTemp << LeagueHelpers::GetTextForChoice(pProposal->GetVoterDecision()->GetType(), iTopChoice);
				(*sTooltipSink) += sTemp.toUTF8();
			}
			break;
		}
	case KNOWLEDGE_INTIMATE:
		{
		// What is our preferred choice on this proposal?
		int iTopChoice = LeagueHelpers::CHOICE_NONE;
		int iTopChoiceScore = MIN_INT;
		std::vector<int> vChoices = pLeague->GetChoicesForDecision(pProposal->GetVoterDecision()->GetType(), GetPlayer()->GetID());
		for (uint i = 0; i < vChoices.size(); i++)
		{
			int iChoice = vChoices[i];
			int iChoiceScore = ScoreVoteChoice(pProposal, iChoice, /*bConsiderGlobal*/ true);
			if (iChoiceScore > iTopChoiceScore)
			{
				iTopChoice = iChoice;
				iTopChoiceScore = iChoiceScore;
			}
		}
		CvAssert(iTopChoice != LeagueHelpers::CHOICE_NONE);
			iRevealedChoice = iTopChoice;
			if (sTooltipSink != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_INTIMATE");
				sTemp << pProposal->GetProposalName();
				sTemp << LeagueHelpers::GetTextForChoice(pProposal->GetVoterDecision()->GetType(), iTopChoice);
				sTemp << GetTextForDesire(EvaluateDesire(iTopChoiceScore));
				(*sTooltipSink) += sTemp.toUTF8();
			}
			break;
		}
	}

	return iRevealedChoice;
}

int CvLeagueAI::EvaluateVoteForOtherPlayerKnowledge(CvLeague* pLeague, PlayerTypes eToPlayer, CvRepealProposal* pProposal, CvString* sTooltipSink)
{
	CvAssert(pLeague);
	CvAssert(pProposal);
	if (!pLeague || !pProposal)
	{
		return LeagueHelpers::CHOICE_NONE;
	}


	// How much are we telling them?
	int iRevealedChoice = LeagueHelpers::CHOICE_NONE;
	KnowledgeLevels eKnowledge = GetKnowledgeGivenToOtherPlayer(eToPlayer);
	switch (eKnowledge)
	{
	case KNOWLEDGE_NONE:
		{
			if (sTooltipSink != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_NONE");
				sTemp << pProposal->GetProposalName();
				(*sTooltipSink) += sTemp.toUTF8();
			}
			break;
		}
	case KNOWLEDGE_PARTIAL:
		{	
		// What is our preferred choice on this proposal?
		int iTopChoice = LeagueHelpers::CHOICE_NONE;
		int iTopChoiceScore = 0;
		std::vector<int> vChoices = pLeague->GetChoicesForDecision(pProposal->GetRepealDecision()->GetType(), GetPlayer()->GetID());
		for (uint i = 0; i < vChoices.size(); i++)
		{
			int iChoice = vChoices[i];
			int iChoiceScore = ScoreVoteChoice(pProposal, iChoice,/*bConsiderGlobal*/ true);
			if (iChoiceScore > iTopChoiceScore)
			{
				iTopChoice = iChoice;
				iTopChoiceScore = iChoiceScore;
			}
		}

			iRevealedChoice = iTopChoice;
			if (sTooltipSink != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_PARTIAL");
				sTemp << pProposal->GetProposalName();
				sTemp << LeagueHelpers::GetTextForChoice(pProposal->GetRepealDecision()->GetType(), iTopChoice);
				(*sTooltipSink) += sTemp.toUTF8();
			}
			break;
		}
	case KNOWLEDGE_INTIMATE:
		{	// What is our preferred choice on this proposal?
		int iTopChoice = LeagueHelpers::CHOICE_NONE;
		int iTopChoiceScore = 0;
		std::vector<int> vChoices = pLeague->GetChoicesForDecision(pProposal->GetRepealDecision()->GetType(), GetPlayer()->GetID());
		for (uint i = 0; i < vChoices.size(); i++)
		{
			int iChoice = vChoices[i];
			int iChoiceScore = ScoreVoteChoice(pProposal, iChoice,/*bConsiderGlobal*/ true);
			if (iChoiceScore > iTopChoiceScore)
			{
				iTopChoice = iChoice;
				iTopChoiceScore = iChoiceScore;
			}
		}

			iRevealedChoice = iTopChoice;
			if (sTooltipSink != NULL)
			{
				Localization::String sTemp = Localization::Lookup("TXT_KEY_LEAGUE_OVERVIEW_MEMBER_OPINIONS_VOTE_INTIMATE");
				sTemp << pProposal->GetProposalName();
				sTemp << LeagueHelpers::GetTextForChoice(pProposal->GetRepealDecision()->GetType(), iTopChoice);
				sTemp << GetTextForDesire(EvaluateDesire(iTopChoiceScore));
				(*sTooltipSink) += sTemp.toUTF8();
			}
			break;
		}
	}

	return iRevealedChoice;
}

CvLeagueAI::DiplomatUsefulnessLevels CvLeagueAI::GetDiplomatUsefulnessAtCiv(PlayerTypes ePlayer)
{
	DiplomatUsefulnessLevels eUsefulness = DIPLOMAT_USEFULNESS_NONE;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague == NULL)
		return eUsefulness;

	int iScore = 0;
	if (GetExtraVotesPerDiplomat() > 0)
	{
		iScore += 1;
	}
	if (EvaluateAlignment(ePlayer) > ALIGNMENT_NEUTRAL)
	{
		iScore += 1;
	}
#if defined(MOD_BALANCE_CORE)
	int iLeague = pLeague->GetPotentialVotesForMember(m_pPlayer->GetID(), ePlayer);
	if(iLeague > pLeague->GetCoreVotesForMember(ePlayer))
	{
		iScore += 1;
	}
	if (iScore >= 3)
	{
		eUsefulness = DIPLOMAT_USEFULNESS_HIGH;
	}
	else
#endif

	if (iScore >= 2)
	{
		eUsefulness = DIPLOMAT_USEFULNESS_MEDIUM;
	}
	else if (iScore >= 1)
	{
		eUsefulness = DIPLOMAT_USEFULNESS_LOW;
	}
	
	return eUsefulness;
}

// Wrapper
int CvLeagueAI::GetExtraVotesPerDiplomat()
{
	return GetPlayer()->GetExtraVotesPerDiplomat();
}

int CvLeagueAI::GetExtraVotesPerCityStateAlly()
{
	int iVotes = 0;

	if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague)
		{
			LeagueSpecialSessionTypes eGoverningSpecialSession = NO_LEAGUE_SPECIAL_SESSION;
			if (pLeague->GetCurrentSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
			{
				eGoverningSpecialSession = pLeague->GetCurrentSpecialSession();
			}
			else if (pLeague->GetLastSpecialSession() != NO_LEAGUE_SPECIAL_SESSION)
			{
				eGoverningSpecialSession = pLeague->GetLastSpecialSession();
			}

			CvAssert(eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION);
			if (eGoverningSpecialSession != NO_LEAGUE_SPECIAL_SESSION)
			{
				CvLeagueSpecialSessionEntry* pInfo = GC.getLeagueSpecialSessionInfo(eGoverningSpecialSession);
				CvAssert(pInfo != NULL);
				if (pInfo != NULL) 
				{
					iVotes += pInfo->GetCityStateDelegates();
				}
			}
		}
	}

	return iVotes;
}

CvString CvLeagueAI::GetCommitVoteDetails(PlayerTypes eToPlayer)
{
	CvString s = "";

	CanCommitVote(eToPlayer, &s);

	return s;
}

CvPlayer* CvLeagueAI::GetPlayer()
{
	return m_pPlayer;
}

CvLeagueAI::VoteCommitment::VoteCommitment()
{
}

CvLeagueAI::VoteCommitment::VoteCommitment(PlayerTypes player, int resolutionID, int voteChoice, int numVotes, bool enact)
{
	eToPlayer = player;
	iResolutionID = resolutionID;
	iVoteChoice = voteChoice;
	iNumVotes = numVotes;
	bEnact = enact;
}

CvLeagueAI::VoteCommitment::~VoteCommitment()
{
}

CvLeagueAI::VoteConsideration::VoteConsideration()
{
}

CvLeagueAI::VoteConsideration::VoteConsideration(bool enact, int id, int choice, int allocated)
{
	bEnact = enact;
	iID = id;
	iChoice = choice;
	iNumAllocated = allocated;
}

CvLeagueAI::VoteConsideration::~VoteConsideration()
{
}

CvLeagueAI::ProposalConsideration::ProposalConsideration()
{
}

CvLeagueAI::ProposalConsideration::ProposalConsideration(bool enact, int index, int choice)
{
	bEnact = enact;
	iIndex = index;
	iChoice = choice;
}

CvLeagueAI::ProposalConsideration::~ProposalConsideration()
{
}

CvString CvLeagueAI::GetTextForDesire(DesireLevels eDesire)
{
	CvString s = "";

	switch (eDesire)
	{
	case DESIRE_NEVER:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_NEVER").toUTF8();
		break;
	case DESIRE_STRONG_DISLIKE:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_STRONG_DISLIKE").toUTF8();
		break;
	case DESIRE_DISLIKE:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_DISLIKE").toUTF8();
		break;
	case DESIRE_WEAK_DISLIKE:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_WEAK_DISLIKE").toUTF8();
		break;
	case DESIRE_NEUTRAL:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_NEUTRAL").toUTF8();
		break;
	case DESIRE_WEAK_LIKE:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_WEAK_LIKE").toUTF8();
		break;
	case DESIRE_LIKE:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_LIKE").toUTF8();
		break;
	case DESIRE_STRONG_LIKE:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_STRONG_LIKE").toUTF8();
		break;
	case DESIRE_ALWAYS:
		s = Localization::Lookup("TXT_KEY_LEAGUE_DESIRE_ALWAYS").toUTF8();
		break;
	}

	return s;
}

CvLeagueAI::DesireLevels CvLeagueAI::EvaluateDesire(int iRawScore)
{
	DesireLevels eDesire = DESIRE_NEVER;

	if (iRawScore < -2500)
	{
		eDesire = DESIRE_NEVER;
	}
	else if (iRawScore < -1000)
	{
		eDesire = DESIRE_STRONG_DISLIKE;
	}
	else if (iRawScore < -250)
	{
		eDesire = DESIRE_DISLIKE;
	}
	else if (iRawScore < -50)
	{
		eDesire = DESIRE_WEAK_DISLIKE;
	}
	else if (iRawScore <= 50)
	{
		eDesire = DESIRE_NEUTRAL;
	}
	else if (iRawScore <= 250)
	{
		eDesire = DESIRE_WEAK_LIKE;
	}
	else if (iRawScore <= 1000)
	{
		eDesire = DESIRE_LIKE;
	}
	else if (iRawScore <= 2500)
	{
		eDesire = DESIRE_STRONG_LIKE;
	}
	else
	{
		eDesire = DESIRE_ALWAYS;
	}

	return eDesire;
}

// Find the most preferred use of our votes in the session of pLeague, and allocate them with some randomness
void CvLeagueAI::AllocateVotes(CvLeague* pLeague)
{
	CvAssert(pLeague != NULL);
	if (!(pLeague != NULL)) 
		return;

	int iVotes = pLeague->GetRemainingVotesForMember(GetPlayer()->GetID());
	int iVotesAllOthersCombined = 0;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;
		if (ePlayer == NO_PLAYER || !pLeague->IsMember(ePlayer))
			continue;

		if (GetPlayer()->GetID() == ePlayer)
			continue;

		iVotesAllOthersCombined += pLeague->GetRemainingVotesForMember(ePlayer);
	}

	//how confident are we
	iVotesAllOthersCombined -= GetPlayer()->GetDiplomacyAI()->GetBoldness();
	
	if (iVotesAllOthersCombined <= pLeague->GetNumMembers())
		iVotesAllOthersCombined = pLeague->GetNumMembers();

	VoteConsiderationList vConsiderations;

	EnactProposalList vEnactProposals = pLeague->GetEnactProposals();
	for (EnactProposalList::iterator it = vEnactProposals.begin(); it != vEnactProposals.end(); ++it)
	{
		FindBestVoteChoices(&(*it), vConsiderations);
	}
	RepealProposalList vRepealProposals = pLeague->GetRepealProposals();
	for (RepealProposalList::iterator it = vRepealProposals.begin(); it != vRepealProposals.end(); ++it)
	{
		FindBestVoteChoices(&(*it), vConsiderations);
	}

	if (vConsiderations.size() > 0)
	{
		vConsiderations.SortItems();

		// Even if we don't like anything, make sure we have something to choose from
		if (vConsiderations.GetTotalWeight() <= 0)
		{
			for (int i = 0; i < vConsiderations.size(); i++)
			{
				vConsiderations.SetWeight(i, 1000);
			}
		}

		CvWeightedVector<VoteConsideration> vVotesAllocated;
		for (int i = 0; i < iVotes; i++)
		{
			vConsiderations.SortItems();
			VoteConsideration chosen = vConsiderations.GetElement(0);
			if (chosen.bEnact)
			{
				pLeague->DoVoteEnact(chosen.iID, GetPlayer()->GetID(), 1, chosen.iChoice);
			}
			else
			{
				pLeague->DoVoteRepeal(chosen.iID, GetPlayer()->GetID(), 1, chosen.iChoice);
			}

			//count how often we voted for one thing already
			//and reduce the weight for what we chose by 15%, so we introduce more variety into our options.
			for (int i = 0; i < vConsiderations.size(); i++)
			{
				VoteConsideration vc = vConsiderations.GetElement(i);
				if (vc.iID == chosen.iID)
				{
					chosen.iNumAllocated++;
					vConsiderations.SetElement(i, chosen);

					int iWeight = vConsiderations.GetWeight(i);
					iWeight *= 85;
					iWeight /= 100;
					vConsiderations.SetWeight(i, iWeight);
					break;
				}
			}

			if (chosen.iNumAllocated >= iVotesAllOthersCombined)
			{
				// If we have already alocated more than we should need to pass it, don't allocate more votes here.
				for (int j = 0; j < vConsiderations.size(); j++)
				{
					if (vConsiderations.GetWeight(j) > 0)
					{
						if (vConsiderations.GetElement(j).bEnact == chosen.bEnact && vConsiderations.GetElement(j).iID == chosen.iID)
						{
							vConsiderations.SetWeight(j, 0);
						}
					}
				}
			}

			// Zero out weight of any other choices that were considered for this proposal, since we can only allocate to one choice
			for (int j = 0; j < vConsiderations.size(); j++)
			{
				if (vConsiderations.GetWeight(j) > 0)
				{
					if (vConsiderations.GetElement(j).bEnact == chosen.bEnact && vConsiderations.GetElement(j).iID == chosen.iID)
					{
						if (vConsiderations.GetElement(j).iChoice != chosen.iChoice)
						{
							vConsiderations.SetWeight(j, 0);
						}
					}
				}
			}

			// For logging - track how many delegates we allocate towards each considered option
			bool bFirst = true;
			for (int j = 0; j < vVotesAllocated.size(); j++)
			{
				if (vVotesAllocated.GetElement(j).bEnact == chosen.bEnact &&
					vVotesAllocated.GetElement(j).iID == chosen.iID &&
					vVotesAllocated.GetElement(j).iChoice == chosen.iChoice)
				{
					bFirst = false;
					vVotesAllocated.SetWeight(j, vVotesAllocated.GetWeight(j) + 1);
				}
			}
			if (bFirst)
			{
				vVotesAllocated.push_back(chosen, 1);
			}
		}

		if (vVotesAllocated.size() <= 0)
			return;

		// Logging
		vVotesAllocated.SortItems();
		for (int i = 0; i < vVotesAllocated.size(); i++)
		{
			if (vVotesAllocated.GetElement(i).bEnact)
			{
				CvEnactProposal* pProposal = pLeague->GetEnactProposal(vVotesAllocated.GetElement(i).iID);
				CvAssert(pProposal != NULL);
				if (pProposal != NULL)
				{
					LogVoteChoiceCommitted(pProposal, vVotesAllocated.GetElement(i).iChoice, vVotesAllocated.GetWeight(i));
				}
			}
			else
			{
				CvRepealProposal* pProposal = pLeague->GetRepealProposal(vVotesAllocated.GetElement(i).iID);
				CvAssert(pProposal != NULL);
				if (pProposal != NULL)
				{
					LogVoteChoiceCommitted(pProposal, vVotesAllocated.GetElement(i).iChoice, vVotesAllocated.GetWeight(i));
				}
			}
		}
	}
}

// Find the best choices for a particular proposal, and add the data directly to the reference list parameter
void CvLeagueAI::FindBestVoteChoices(CvEnactProposal* pProposal, VoteConsiderationList& considerations)
{
	VoteConsiderationList vScoredChoices;
	int iMaxChoicesToConsider = 1;

	CvAssert(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0);
	if (!(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)) return;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvAssert(pLeague != NULL);
	if (!(pLeague != NULL)) return;

	switch (pProposal->GetVoterDecision()->GetType())
	{
	case RESOLUTION_DECISION_YES_OR_NO:
		{
			iMaxChoicesToConsider = 1;
			break;
		}
	case RESOLUTION_DECISION_ANY_MEMBER:
	case RESOLUTION_DECISION_MAJOR_CIV_MEMBER:
	case RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER:
		{
			iMaxChoicesToConsider = 1;
			break;
		}
	default:
		{
			CvAssertMsg(false, "Unexpected decision type when evaluating choices for AI on a proposed resolution.");
			break;
		}
	}

	std::vector<int> vChoices = pLeague->GetChoicesForDecision(pProposal->GetVoterDecision()->GetType(), GetPlayer()->GetID());
	for (uint i = 0; i < vChoices.size(); i++)
	{
		VoteConsideration consideration(/*bEnact*/ true, pProposal->GetID(), vChoices[i], 0);
		int iScore = ScoreVoteChoice(pProposal, vChoices[i], /*bConsiderGlobal*/ true);

		iScore = MAX(iScore, 0); // No negative weights
		vScoredChoices.push_back(consideration, iScore);
	}

	if (vScoredChoices.size() > 0)
	{
		vScoredChoices.SortItems();
		for (int i = 0; i < vScoredChoices.size() && i < iMaxChoicesToConsider; i++)
		{
			considerations.push_back(vScoredChoices.GetElement(i), vScoredChoices.GetWeight(i));
			LogVoteChoiceConsidered(pProposal, vScoredChoices.GetElement(i).iChoice, vScoredChoices.GetWeight(i));
		}
	}
}

// Find the best choices for a particular proposal, and add the data directly to the reference list parameter
void CvLeagueAI::FindBestVoteChoices(CvRepealProposal* pProposal, VoteConsiderationList& considerations)
{
	VoteConsiderationList vScoredChoices;
	int iMaxChoicesToConsider = 1;

	CvAssert(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0);
	if (!(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)) return;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvAssert(pLeague != NULL);
	if (!(pLeague != NULL)) return;

	switch (pProposal->GetRepealDecision()->GetType())
	{
	case RESOLUTION_DECISION_REPEAL:
		{
			iMaxChoicesToConsider = 1;
			break;
		}
	default:
		{
			CvAssertMsg(false, "Unexpected decision type when evaluating choices for AI on a proposed resolution.");
			break;
		}
	}

	std::vector<int> vChoices = pLeague->GetChoicesForDecision(pProposal->GetRepealDecision()->GetType(), GetPlayer()->GetID());
	for (uint i = 0; i < vChoices.size(); i++)
	{
		VoteConsideration consideration(/*bEnact*/ false, pProposal->GetID(), vChoices[i], 0);
		int iScore = ScoreVoteChoice(pProposal, vChoices[i],/*bConsiderGlobal*/ true);

		iScore = MAX(iScore, 0); // No negative weights
		vScoredChoices.push_back(consideration, iScore);
	}

	if (vScoredChoices.size() > 0)
	{
		vScoredChoices.SortItems();
		for (int i = 0; i < vScoredChoices.size() && i < iMaxChoicesToConsider; i++)
		{
			considerations.push_back(vScoredChoices.GetElement(i), vScoredChoices.GetWeight(i));
			LogVoteChoiceConsidered(pProposal, vScoredChoices.GetElement(i).iChoice, vScoredChoices.GetWeight(i));
		}
	}
}

// Score a particular choice on a particular proposal
int CvLeagueAI::ScoreVoteChoice(CvEnactProposal* pProposal, int iChoice, bool bConsiderGlobal)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return 0;
	CvAssert(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0);
	if (!(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)) return 0;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvAssert(pLeague != NULL);
	if (!(pLeague != NULL)) return 0;

	int iScore = 0;

	switch (pProposal->GetVoterDecision()->GetType())
	{
	case RESOLUTION_DECISION_YES_OR_NO:
		{
			iScore = ScoreVoteChoiceYesNo(pProposal, iChoice, /*bEnact*/ true, bConsiderGlobal);
			break;
		}
	case RESOLUTION_DECISION_ANY_MEMBER:
	case RESOLUTION_DECISION_MAJOR_CIV_MEMBER:
	case RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER:
		{
			iScore = ScoreVoteChoicePlayer(pProposal, iChoice, /*bEnact*/ true);
			break;
		}
	default:
		{
			CvAssertMsg(false, "Unexpected decision type when evaluating choices for AI on a proposed resolution.");
			break;
		}
	}
	
	return iScore;
}

// Score a particular choice on a particular proposal
int CvLeagueAI::ScoreVoteChoice(CvRepealProposal* pProposal, int iChoice, bool bConsiderGlobal)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return 0;
	CvAssert(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0);
	if (!(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)) return 0;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvAssert(pLeague != NULL);
	if (!(pLeague != NULL)) return 0;

	int iScore = 0;

	switch (pProposal->GetRepealDecision()->GetType())
	{
	case RESOLUTION_DECISION_REPEAL:
		{
			iScore = ScoreVoteChoiceYesNo(pProposal, iChoice, /*bEnact*/ false, bConsiderGlobal);
			break;
		}
	default:
		{
			CvAssertMsg(false, "Unexpected decision type when evaluating choices for AI on a proposed resolution.");
			break;
		}
	}

	return iScore;
}

// Score a particular choice on a particular proposal which is a decision between Yes and No
int CvLeagueAI::ScoreVoteChoiceYesNo(CvProposal* pProposal, int iChoice, bool bEnact, bool bConsiderGlobal, bool bForSelf)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return 0;

	// How much do we like this choice for this proposal?  Positive is like, negative is dislike.
	// Evaluate as if we are voting Yes to Enact the proposal.  Post-processing below to fit actual situation.
	int iScore = 0;

	// == Proposer Choice ==
	ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
	PlayerTypes eTargetPlayer = NO_PLAYER;
	if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
		eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
		eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
	{
		eTargetPlayer = (PlayerTypes) pProposal->GetProposerDecision()->GetDecision();
	}
	ResourceTypes eTargetLuxury = NO_RESOURCE;
	if (eProposerDecision == RESOLUTION_DECISION_ANY_LUXURY_RESOURCE)
	{
		CvResourceInfo* pInfo = GC.getResourceInfo((ResourceTypes) pProposal->GetProposerDecision()->GetDecision());
		if (pInfo && pInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
		{
			eTargetLuxury = (ResourceTypes) pProposal->GetProposerDecision()->GetDecision();
		}
	}
	ReligionTypes eTargetReligion = NO_RELIGION;
	if (eProposerDecision == RESOLUTION_DECISION_RELIGION)
	{
		eTargetReligion = (ReligionTypes) pProposal->GetProposerDecision()->GetDecision();
	}
	PolicyBranchTypes eTargetIdeology = NO_POLICY_BRANCH_TYPE;
	if (eProposerDecision == RESOLUTION_DECISION_IDEOLOGY)
	{
		eTargetIdeology = (PolicyBranchTypes) pProposal->GetProposerDecision()->GetDecision();
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	PlayerTypes eTargetCityState = NO_PLAYER;
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eProposerDecision == RESOLUTION_DECISION_CITY_CSD)
	{
		eTargetCityState = (PlayerTypes) pProposal->GetProposerDecision()->GetDecision();
	}
#endif
	PlayerTypes eProposer = pProposal->GetProposalPlayer();
	bool bScienceVictoryEnabled = GC.getGame().isVictoryValid((VictoryTypes)GC.getInfoTypeForString("VICTORY_SPACE_RACE", true));
	bool bDiploVictoryEnabled = GC.getGame().isVictoryValid((VictoryTypes)GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true));
	bool bCultureVictoryEnabled = GC.getGame().isVictoryValid((VictoryTypes)GC.getInfoTypeForString("VICTORY_CULTURE", true));
	int iExtra = 0;

	// == Gameplay Effects ==
	// International Projects
	if (pProposal->GetEffects()->eLeagueProjectEnabled != NO_LEAGUE_PROJECT)
	{
		LeagueProjectTypes eWorldsFair = (LeagueProjectTypes) GC.getInfoTypeForString("LEAGUE_PROJECT_WORLD_FAIR", true);
		LeagueProjectTypes eInternationalGames = (LeagueProjectTypes) GC.getInfoTypeForString("LEAGUE_PROJECT_WORLD_GAMES", true);
		LeagueProjectTypes eInternationalSpaceStation = (LeagueProjectTypes) GC.getInfoTypeForString("LEAGUE_PROJECT_INTERNATIONAL_SPACE_STATION", true);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		LeagueProjectTypes eTreasureFleet = (LeagueProjectTypes) GC.getInfoTypeForString("LEAGUE_PROJECT_TREASURE_FLEET", true);
		LeagueProjectTypes eWargames = (LeagueProjectTypes) GC.getInfoTypeForString("LEAGUE_PROJECT_WARGAMES", true);
		LeagueProjectTypes eUN = (LeagueProjectTypes)GC.getInfoTypeForString("LEAGUE_PROJECT_UNITED_NATIONS", true);
#endif
		iExtra = 0;
		// Production might
		int iMight = 0;
		int iOurProductionMight = GetPlayer()->GetProductionMight();
		int iHigherProductionCivs = 0;
		int iAliveCivs = 0;
		int iHighestProduction = 0;
		int iPercentofWinning = 0;
		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes e = (PlayerTypes) i;
			if (GET_PLAYER(e).isAlive() && !GET_PLAYER(e).isMinorCiv())
			{
				iAliveCivs++;
				iMight = GET_PLAYER(e).GetProductionMight();
				if (GetPlayer()->GetID() != e)
				{
					if (iMight > iOurProductionMight)
					{
						iHigherProductionCivs++;
					}
				}
				if (iMight > iHighestProduction)
				{
					iHighestProduction = iMight;
				}
				
			}
		}
		int iProductionMightPercent = (iOurProductionMight * 100) / max(1,iHighestProduction);
		bool bCanGold = iProductionMightPercent >= 80;
		if (bCanGold)
		{
			iPercentofWinning = 100 / (max(iHigherProductionCivs + 1, 1));
		}
		bool bCanSilver = iProductionMightPercent >= 40;
		bool bCanBronze = iProductionMightPercent >= 20;

		// Project specific benefits
		if (eWorldsFair == pProposal->GetEffects()->eLeagueProjectEnabled)
		{
			if (bCanGold)
			{
				// factories
				iExtra += (iPercentofWinning * 40 * GetPlayer()->getNumCities()) / 100;
				// golden age
				iExtra += 3 * iPercentofWinning;
			}
			if (bCanSilver)
			{
				int iTurnsForPolicy = (GetPlayer()->getNextPolicyCost()) / (max(1, GetPlayer()->GetTotalJONSCulturePerTurn()));
				iExtra += (40 * GC.getGame().getGameSpeedInfo().getCulturePercent())/(max(1, iTurnsForPolicy));
			}
			if (bCanBronze)
			{
				iExtra += 500;
			}
		}
		else if (eInternationalGames == pProposal->GetEffects()->eLeagueProjectEnabled)
		{
			int iLowestPercent = GC.getCULTURE_LEVEL_INFLUENTIAL();   // Don't want to target civs if already influential

			for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
				if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
				{
					int iInfluenceOn = GetPlayer()->GetCulture()->GetInfluenceOn((PlayerTypes)iLoopPlayer);
					int iLifetimeCulture = kPlayer.GetJONSCultureEverGenerated();
					int iPercent = 0;

					if (iInfluenceOn > 0)
					{
						iPercent = (iInfluenceOn * 100) / max(1, iLifetimeCulture);
					}
					if (iPercent < 0)
					{
						iPercent = 0;
					}
					if (iPercent < iLowestPercent)
					{
						iLowestPercent = iPercent;
					}
				}
			}
			if (bCanGold)
			{
				if (bCultureVictoryEnabled)
				{
					iExtra += iPercentofWinning * min(iLowestPercent, 50);
				}
				else
				{
					iExtra += 2 * iPercentofWinning;
				}
			}
			if(bCanSilver)
			{
				iExtra += 200;
			}
			if (bCanBronze)
			{
				if (iLowestPercent > 25)
				{
					if (bCultureVictoryEnabled)
					{
						iExtra += 120 * min(iLowestPercent - 25, 50);
					}
				}
			}
		}
		else if (eInternationalSpaceStation == pProposal->GetEffects()->eLeagueProjectEnabled)
		{
			int iTeams = 0;
			int iTechs;
			int iTotalTechs = 0;
			int iOurTechs = 0;
			for (int iTeamLoop = 0; iTeamLoop < MAX_MAJOR_CIVS; iTeamLoop++)	// Looping over all MAJOR teams
			{
				TeamTypes eTeam = (TeamTypes)iTeamLoop;

				if (GET_TEAM(eTeam).isAlive())
				{
					iTeams++;
					iTechs = GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown();
					if (iTechs > 0)
					{
						iTotalTechs += iTechs;
						if (eTeam == GetPlayer()->getTeam())
						{
							iOurTechs = iTechs;
						}
					}
				}
			}
			// Gives a percent we are above or below global mean techs
			int iTechPercent = ((iOurTechs * iTeams * 100) / max(1,iTotalTechs)) - 100;

			if (bCanGold)
			{
				iExtra += (iPercentofWinning * (10 + iTechPercent)) / 10;
			}
			if (bCanSilver)
			{
				iExtra += 15 * (10 + iTechPercent);
			}
			if (bCanBronze)
			{
				iExtra += 150;
			}
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		else if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eTreasureFleet == pProposal->GetEffects()->eLeagueProjectEnabled)
		{
			if (bCanGold)
			{
				int iOurSeaMight = GetPlayer()->GetMilitaryMight();
				int iHighestSeaMight = 0;
				for (int i = 0; i < MAX_MAJOR_CIVS; i++)
				{
					PlayerTypes e = (PlayerTypes)i;
					if (GET_PLAYER(e).isAlive() && !GET_PLAYER(e).isMinorCiv())
					{
						iAliveCivs++;
						int iSeaMight = GET_PLAYER(e).GetMilitaryMight();
						if (iSeaMight > iHighestSeaMight)
						{
							iHighestSeaMight = iSeaMight;
						}
					}
				}
				int iSeaMightPercent = (iOurSeaMight * 100) / max(1,iHighestSeaMight);
				iExtra += (iPercentofWinning * iSeaMightPercent) / 8;
				iExtra += iPercentofWinning * 3;
			}
			if (bCanSilver)
			{
				iExtra += 200;
			}
			if (bCanBronze)
			{
				iExtra += 100;
			}
		}
		else if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eWargames == pProposal->GetEffects()->eLeagueProjectEnabled)
		{
			if (bCanGold)
			{
				int iImprovements = 0;
				ImprovementTypes eLandmark = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK");
				ImprovementTypes eFort = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT");
				ImprovementTypes eCitadel = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITADEL");
				if (eLandmark != NO_IMPROVEMENT)
				{
					iImprovements += GetPlayer()->getImprovementCount(eLandmark);
				}
				if (eFort != NO_IMPROVEMENT)
				{
					iImprovements += GetPlayer()->getImprovementCount(eFort);
				}
				if (eCitadel != NO_IMPROVEMENT)
				{
					iImprovements += GetPlayer()->getImprovementCount(eCitadel);
				}
				iExtra += (5 * iPercentofWinning * iImprovements) / 100;
			}
			if (bCanSilver)
			{
				iExtra += 100;
				int iOurMilitaryMight = GetPlayer()->GetMilitaryMight();
				int iHighestMilitaryMight = 0;
				for (int i = 0; i < MAX_MAJOR_CIVS; i++)
				{
					PlayerTypes e = (PlayerTypes)i;
					if (GET_PLAYER(e).isAlive() && !GET_PLAYER(e).isMinorCiv())
					{
						iAliveCivs++;
						int iMilitaryMight = GET_PLAYER(e).GetMilitaryMight();
						if (iMilitaryMight > iHighestMilitaryMight)
						{
							iHighestMilitaryMight = iMilitaryMight;
						}
						if (e != GetPlayer()->GetID())
						{
							if (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(e) > TARGET_VALUE_AVERAGE)
							{
								iExtra += (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(e) - TARGET_VALUE_AVERAGE) * 100;
							}
						}
					}
				}
				int iMilitaryMightPercent = (iOurMilitaryMight * 100) / max(1,iHighestMilitaryMight);
				iExtra += iMilitaryMightPercent * 10;
			}
			if (bCanBronze)
			{
				iExtra += 150;
			}
		}
		else if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && eUN == pProposal->GetEffects()->eLeagueProjectEnabled)
		{
			int iVotes = GC.getGame().GetGameLeagues()->GetActiveLeague()->CalculateStartingVotesForMember(GetPlayer()->GetID());
			int iNeededVotes = GC.getGame().GetVotesNeededForDiploVictory();
			int iVoteRatio = 0;
			if (bDiploVictoryEnabled)
			{
				if (iNeededVotes > 0)
				{
					iVoteRatio = (iVotes * 100) / max(1,iNeededVotes);
					if (iVoteRatio >= 100)
					{
						iExtra += 10000;
					}
					else if (iVoteRatio >= 75)
					{
						iExtra += 5000;
					}
					else if (iVoteRatio >= 50)
					{
						iExtra += 2000;
					}
					else if (iVoteRatio >= 25)
					{
						iExtra += 750;
					}
				}
			}
			if (bCanGold)
			{
				iExtra += 3 * iPercentofWinning;
				iExtra += (iVoteRatio * iPercentofWinning) / 30;
			}
			if (bCanSilver)
			{
				iExtra += 200;
				iExtra += iVoteRatio * 2;
			}
			if (bCanBronze)
			{
				iExtra += 100;
			}
		}
#endif
		iScore += iExtra;
	}
	// Embargo City-States
	if (pProposal->GetEffects()->bEmbargoCityStates)
	{
		iExtra = 0;
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();

		// Trade connections
		int iCSDestinations = 0;
		int iCSPartners = 0;
		int iCivDestinations = 0;
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			PlayerTypes e = (PlayerTypes) i;
			if (e != GetPlayer()->GetID() && GET_PLAYER(e).isAlive())
			{
				if (GET_PLAYER(e).isMinorCiv())
				{
					iCSDestinations++;
					if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(GetPlayer()->GetID(), e))
					{
						iCSPartners++;
					}
				}
				else if (pLeague != NULL && !pLeague->IsTradeEmbargoed(GetPlayer()->GetID(), e))
				{
					iCivDestinations += GET_PLAYER(e).getNumCities();
				}
			}
		}

		// Would we lose active CS trade routes?
		if (iCSPartners > 0)
		{
			iExtra -= iCSPartners * 100;
		}

		// Do we have bonus influence from trade routes with CS?

		if (GetPlayer()->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE) > 0)
		{
			iExtra -= 8 * GetPlayer()->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE);
		}


		// Can we trade with any major civs?

			// Based on estimates, would we still have enough valid trade routes if this passed?	
		int iPossibleRoutesAfter = iCivDestinations + GetPlayer()->getNumCities();
		int iTradeRouteDefecit = ((int)GetPlayer()->GetTrade()->GetNumTradeRoutesPossible()) - iPossibleRoutesAfter;
		if (iTradeRouteDefecit > 0)
		{
			iExtra -= min(100 * iTradeRouteDefecit, 1000);
		}

		iScore += iExtra;
	}
	// Embargo
	if (pProposal->GetEffects()->bEmbargoPlayer)
	{
		CvAssertMsg(eTargetPlayer != NO_PLAYER, "Evaluating an embargo on NO_PLAYER. Please send Anton your save file and version.");
		iExtra = 0;
		int iTradeDealValue = 0;
		// Major Civ relations
		if (eTargetPlayer == GetPlayer()->GetID())
		{
			iExtra -= 500;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (eLoopPlayer != GetPlayer()->GetID() && GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv())
				{
					if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(GetPlayer()->GetID(), eLoopPlayer, true))
					{
						iTradeDealValue += GC.getGame().GetGameTrade()->CountNumPlayerConnectionsToPlayer(GetPlayer()->GetID(), eLoopPlayer, true) * 80;
					}
					iTradeDealValue += min(400, GC.getGame().GetGameDeals().GetDealValueWithPlayer(GetPlayer()->GetID(), eLoopPlayer, false) / 5);

					if (GetPlayer()->GetDiplomacyAI()->GetWarmongerThreat(eLoopPlayer) > THREAT_MAJOR)
					{
						iExtra -= (GetPlayer()->GetDiplomacyAI()->GetWarmongerThreat(eLoopPlayer) - THREAT_MAJOR) * 50;
					}
					if (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eLoopPlayer) > CIV_OPINION_NEUTRAL)
					{
						iExtra -= (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eLoopPlayer) - CIV_OPINION_NEUTRAL) * 50;
					}
				}
			}
			iExtra -= iTradeDealValue;
		}
		else if (GET_PLAYER(eTargetPlayer).isMajorCiv())
		{
			if (bForSelf)
			{
				if (GetPlayer()->GetDiplomacyAI()->GetCivApproach(eTargetPlayer) < CIV_APPROACH_GUARDED && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eTargetPlayer) != NO_CIV_APPROACH)
				{
					if (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(eTargetPlayer) > TARGET_VALUE_AVERAGE)
					{
						iExtra += (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(eTargetPlayer) - TARGET_VALUE_AVERAGE) * 80;
					}
				}

				if (GetPlayer()->IsAtWarWith(eTargetPlayer))
				{
					iExtra += 150;
				}

				if (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eTargetPlayer) > CIV_OPINION_ENEMY)
				{
					iExtra -= (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eTargetPlayer) - CIV_OPINION_ENEMY) * 80;
				}

				if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(GetPlayer()->GetID(), eTargetPlayer, true))
				{
					iTradeDealValue += GC.getGame().GetGameTrade()->CountNumPlayerConnectionsToPlayer(GetPlayer()->GetID(), eTargetPlayer, true) * 70;
				}
				iTradeDealValue += min(400,GC.getGame().GetGameDeals().GetDealValueWithPlayer(GetPlayer()->GetID(), eTargetPlayer, false) / 5);

				iExtra -= iTradeDealValue;
			}
		}

		iScore += iExtra;
	}
	// Ban Luxury
	if (pProposal->GetEffects()->bNoResourceHappiness)
	{
		CvAssertMsg(eTargetLuxury != NO_RESOURCE, "Evaluating banning Happiness for NO_RESOURCE. Please send Anton your save file and version.");
		iExtra = 0;

		// What other major civs have this resource?

		// Do we have this resource?
		if (GetPlayer()->getNumResourceTotal(eTargetLuxury) > 0)
		{
			iExtra -= 50;
			if (GetPlayer()->IsEmpireUnhappy())
			{
				iExtra -= -150;
			}
		}
		int iNumOwned = GetPlayer()->getResourceInOwnedPlots(eTargetLuxury);
		if (iNumOwned > 0)
		{
			iExtra -= 50 * iNumOwned;
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
			if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES && GetPlayer()->HasGlobalMonopoly(eTargetLuxury))
			{
				iExtra -= -300;
			}
#endif
		}

		iScore += iExtra;
	}
	// Standing Army Tax
	if (pProposal->GetEffects()->iUnitMaintenanceGoldPercent != 0)
	{
		iExtra = 0;
		int iFactor = pProposal->GetEffects()->iUnitMaintenanceGoldPercent;

		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		{
			// This is the Global Peace Accords resolution - doubles warmonger penalties
			if (iFactor > 0)
			{
				CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();
				int iWarmongerThreat = 0;
				int iWarStates = 0;
				int iWarTargets = 0;
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

					if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv() && GET_PLAYER(eLoopPlayer).getNumCities() > 0 && GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER(eLoopPlayer).getTeam()))
					{
						if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetWarmongerThreat(GetPlayer()->GetID()) != NO_THREAT_VALUE)
						{
							iWarmongerThreat += GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetWarmongerThreat(GetPlayer()->GetID());
						}
						// At war
						if (GetPlayer()->IsAtWarWith(eLoopPlayer))
						{
							if (pDiploAI->GetWarState(eLoopPlayer) > WAR_STATE_STALEMATE)
							{
								iWarStates += pDiploAI->GetWarState(eLoopPlayer) - WAR_STATE_STALEMATE;
							}
						}
						else
						{
							if (pDiploAI->GetPlayerTargetValue(eLoopPlayer) > TARGET_VALUE_AVERAGE)
							{
								iWarTargets += pDiploAI->GetPlayerTargetValue(eLoopPlayer) - TARGET_VALUE_AVERAGE;
							}
						}
					}
				}
				iExtra -= iWarmongerThreat * 50;
				iExtra -= iWarStates * 100;
				iExtra -= iWarTargets * 50;
			}
		}

		// What is the ratio of our current maintenance costs to our gross GPT?
		int iUnitMaintenance = GetPlayer()->GetTreasury()->GetExpensePerTurnUnitMaintenance();
		int iGPT = GetPlayer()->GetTreasury()->CalculateGrossGold();
		int iPercent = (iUnitMaintenance * 100) / max (1,iGPT);

		iExtra -= (max(0, iPercent - 10) * iFactor) / 2;

		iScore += iExtra;
	}
	// Casus Belli
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pProposal->GetEffects()->iIsWorldWar)
	{
		iExtra = 0;
		CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();
		
		int iWarmongerThreat = 0;
		int iWarStates = 0;
		int iWarTargets = 0;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

			if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv() && GET_PLAYER(eLoopPlayer).getNumCities() > 0 && GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetWarmongerThreat(GetPlayer()->GetID()) != NO_THREAT_VALUE)
				{
					iWarmongerThreat += GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetWarmongerThreat(GetPlayer()->GetID());
				}
				// At war
				if (GetPlayer()->IsAtWarWith(eLoopPlayer))
				{
					if (pDiploAI->GetWarState(eLoopPlayer) > WAR_STATE_STALEMATE)
					{
						iWarStates += pDiploAI->GetWarState(eLoopPlayer) - WAR_STATE_STALEMATE;
					}
				}
				else
				{
					if (pDiploAI->GetPlayerTargetValue(eLoopPlayer) > TARGET_VALUE_AVERAGE)
					{
						iWarTargets += pDiploAI->GetPlayerTargetValue(eLoopPlayer) - TARGET_VALUE_AVERAGE;
					}
				}
			}
		}
		iExtra += iWarmongerThreat * 50;
		iExtra += iWarStates * 100;
		iExtra += iWarTargets * 50;

		iScore += iExtra;
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && pProposal->GetEffects()->iVassalMaintenanceGoldPercent != 0)
	{
		iExtra = 0;
		int iFactor = pProposal->GetEffects()->iVassalMaintenanceGoldPercent;
		
		// REALLY hate it if we have vassals
		if(GET_TEAM(GetPlayer()->getTeam()).GetNumVassals() > 0)
		{
			iExtra -= 5 * GET_TEAM(GetPlayer()->getTeam()).GetNumVassals() * iFactor;
		}

		iScore += iExtra;
	}
	//End Vassalage
	if (MOD_DIPLOMACY_CIV4_FEATURES && pProposal->GetEffects()->bEndAllCurrentVassals)
	{
		iExtra = 0;
		//How does this affect us?
		TeamTypes eTeam = GetPlayer()->getTeam();
		if(eTeam != NO_TEAM)
		{

			if (GET_TEAM(eTeam).GetNumVassals() > 0 && !GetPlayer()->IsVassalsNoRebel())
			{
				//We have vassals? Eek!
				iExtra = (-1000 * GET_TEAM(eTeam).GetNumVassals());

			}
			else if (GetPlayer()->IsVassalOfSomeone())
			{

				TeamTypes eMasterTeam = GET_TEAM(GetPlayer()->getTeam()).GetMaster();
				vector<PlayerTypes> vMasterTeam = GET_TEAM(eMasterTeam).getPlayers();

				for (size_t i=0; i<vMasterTeam.size(); i++)
				{
					PlayerTypes eMaster = (PlayerTypes) vMasterTeam[i];

					if (GET_PLAYER(eMaster).isAlive() && !GET_PLAYER(eMaster).IsVassalsNoRebel())
					{

						VassalTreatmentTypes eVassalTreatmentLevel = GetPlayer()->GetDiplomacyAI()->GetVassalTreatmentLevel(eMaster);

						if (GetPlayer()->GetDiplomacyAI()->WasResurrectedBy(eMaster))
						{
							iExtra -= 500;
						}

						if (GetPlayer()->GetDiplomacyAI()->IsVoluntaryVassalage(eMaster))
						{
							switch (eVassalTreatmentLevel)
							{
							case VASSAL_TREATMENT_CONTENT:
								iExtra -= 500;
								break;
							case VASSAL_TREATMENT_DISAGREE:
								iExtra -= 250;
								break;
							case VASSAL_TREATMENT_MISTREATED:
								iExtra += 250;
								break;
							case VASSAL_TREATMENT_UNHAPPY:
								iExtra += 500;
								break;
							case VASSAL_TREATMENT_ENSLAVED:
								iExtra += 1000;
								break;
							}
						}
						else
						{
							switch (eVassalTreatmentLevel)
							{
							case VASSAL_TREATMENT_CONTENT:
								iExtra -= 500;
								break;
							case VASSAL_TREATMENT_DISAGREE:
								iExtra -= 250;
								break;
							case VASSAL_TREATMENT_MISTREATED:
								iExtra += 500;
								break;
							case VASSAL_TREATMENT_UNHAPPY:
								iExtra += 1000;
								break;
							case VASSAL_TREATMENT_ENSLAVED:
								iExtra += 2000;
								break;
							}
						}
					}
				}
			}

				//Let's see how we feel about other vassals out there...

			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv() && GET_PLAYER(eLoopPlayer).IsVassalOfSomeone())
				{
					CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eLoopPlayer);
					if (eApproach <= CIV_APPROACH_DECEPTIVE && eApproach != NO_CIV_APPROACH)
					{
						if (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(eLoopPlayer) > TARGET_VALUE_AVERAGE)
						{
							iExtra += (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(eLoopPlayer) - TARGET_VALUE_AVERAGE) * 100;
						}
					}

				}
			}

		}

		iScore += iExtra;
	}
#endif
	// Scholars in Residence
	if (pProposal->GetEffects()->iMemberDiscoveredTechMod != 0)
	{
		iExtra = 0;
		int iMostTechsKnown = 0;
		for (int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eLoopTeam = (TeamTypes)iTeamLoop;
			if (!GET_TEAM(eLoopTeam).isMinorCiv())
			{
				int iTechsKnown = GET_TEAM(eLoopTeam).GetTeamTechs()->GetNumTechsKnown();
				if (iTechsKnown > iMostTechsKnown)
				{
					iMostTechsKnown = iTechsKnown;
				}
			}
		}
		int iTechsBehind = iMostTechsKnown - GET_TEAM(GET_PLAYER(GetPlayer()->GetID()).getTeam()).GetTeamTechs()->GetNumTechsKnown() - 1;
		iExtra += 120 * iTechsBehind;

		iScore += iExtra;
	}
	// Cultural Heritage Sites
	if (pProposal->GetEffects()->iCulturePerWonder != 0)
	{
		iExtra = 0;
		int iNumWonders = GetPlayer()->GetNumWonders();
		iExtra += 40 * iNumWonders;

		int iNumGreatWorks = GetPlayer()->GetCulture()->GetNumGreatWorks();
		iExtra += 20 * iNumGreatWorks;

		iScore += iExtra;
	}
	// Natural Heritage Sites
	if (pProposal->GetEffects()->iCulturePerNaturalWonder != 0)
	{
		iExtra = 0;
		int iNumNaturalWonders = GetPlayer()->GetNumNaturalWondersInOwnedPlots();

		iExtra += 200 * iNumNaturalWonders;

		iScore += iExtra;
	}
	// Nuclear Non-Proliferation
	if (pProposal->GetEffects()->bNoTrainingNuclearWeapons)
	{
		iExtra = 0;
		int iDislikedNeighbours = 0;
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eMajor = (PlayerTypes)iMajorLoop;
			if (GET_PLAYER(eMajor).isAlive() && GET_PLAYER(eMajor).isMajorCiv())
			{
				if (GetPlayer()->GetDiplomacyAI()->GetLandDisputeLevel(eMajor) >= DISPUTE_LEVEL_STRONG)
				{
					if (GetPlayer()->GetDiplomacyAI()->GetCivApproach(eMajor) < CIV_APPROACH_DECEPTIVE && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eMajor) != NO_CIV_APPROACH)
					{
						iDislikedNeighbours += CIV_APPROACH_DECEPTIVE - GetPlayer()->GetDiplomacyAI()->GetCivApproach(eMajor);
					}
				}

			}

		}
		// Protect against a modder setting this to zero
		int iNukeFlavor = 8;
		for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
		{
			if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_NUKE")
			{
				iNukeFlavor = range(GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop), 0, 20);
				break;
			}
		}
		iExtra += (8 - iNukeFlavor) * iDislikedNeighbours * 20;

		// Speaking of Gandhi, he's very nuke happy!
		if (GC.getGame().IsNuclearGandhiEnabled() && GetPlayer()->GetPlayerTraits()->IsPopulationBoostReligion())
		{
			iExtra -= 1000;
		}

		iScore += iExtra;
	}
	// World Religion
	if (pProposal->GetEffects()->iVotesForFollowingReligion != 0 ||
		pProposal->GetEffects()->iHolyCityTourism != 0 ||
		pProposal->GetEffects()->iReligionSpreadStrengthMod != 0)
	{
		CvAssertMsg(eTargetReligion != NO_RELIGION, "Evaluating World Religion for NO_RELIGION. Please send Anton your save file and version.");
		iExtra = 0;

		ReligionTypes eFoundedReligion = GetPlayer()->GetReligions()->GetReligionCreatedByPlayer();
		if (eFoundedReligion != NO_RELIGION)		// if this player founded a religion
		{
			bool bFoundedTargetReligion = eFoundedReligion == eTargetReligion;

			// extra votes
			int iPossibleExtraVotes = 0;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).isMajorCiv())
				{
					if (GET_PLAYER(eLoopPlayer).GetReligions()->HasReligionInMostCities(eFoundedReligion))
					{
						++iPossibleExtraVotes;
					}
				}
			}
			if (bFoundedTargetReligion)
			{
				iExtra += 100 * iPossibleExtraVotes;
			}
			else
			{
				if (bForSelf)
				{
					iExtra -= 100 * iPossibleExtraVotes;		// negative because other world religion blocks our own
				}
			}
			if (!bDiploVictoryEnabled)
			{
				iExtra /= 2;
			}
			// Holy City Tourism
			const CvReligion* pkTargetReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, GetPlayer()->GetID());
			int iHolyCityTourism = min(20000,pkTargetReligion->GetHolyCity()->GetBaseTourism());
			if (!bCultureVictoryEnabled)
			{
				iHolyCityTourism /= 10;
			}
			if (bFoundedTargetReligion)
			{
				iExtra +=  iHolyCityTourism / 10;
			}
			else
			{
				if (bForSelf)
				{
					iExtra -= iHolyCityTourism / 10;
				}
			}

			// Religion Spread
			if (bFoundedTargetReligion)
			{
				iExtra += 150;
			}
			else
			{
				if (bForSelf)
				{
					iExtra -= 150;
				}
			}
		}
		else
		{
			bool bMajorityReligion = GetPlayer()->GetReligions()->HasReligionInMostCities(eTargetReligion);
			if (bMajorityReligion)
			{
				iExtra += 100;
			}
			else
			{
				if (bForSelf)
				{
					iExtra -= 50;
				}
			}
		}

		iScore += iExtra;
	}
	// World Ideology
	if (pProposal->GetEffects()->iVotesForFollowingIdeology != 0 ||
		pProposal->GetEffects()->iOtherIdeologyRebellionMod != 0)
	{
		CvAssertMsg(eTargetIdeology != NO_POLICY_BRANCH_TYPE, "Evaluating World Ideology for NO_POLICY_BRANCH_TYPE. Please send Anton your save file and version.");
		iExtra = 0;
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague)
		{
			if (bDiploVictoryEnabled)
			{
				int iVotes = pLeague->CalculateStartingVotesForMember(GetPlayer()->GetID());
				int iVotesNeeded = GC.getGame().GetVotesNeededForDiploVictory();
				int iPercent = iVotes * 100;
				iPercent /= max(1, iVotesNeeded);
				if (iPercent >= 100)
				{
					iExtra += 10000;
				}
				if (iPercent >= 80)
				{
					iExtra += 5000;
				}
				else if (iPercent >= 60)
				{
					iExtra += 2500;
				}
				else if (iPercent >= 40)
				{
					iExtra += 600;
				}
			}
		}

		PolicyBranchTypes eOurIdeology = GetPlayer()->GetPlayerPolicies()->GetLateGamePolicyTree();
		if (eOurIdeology != NO_POLICY_BRANCH_TYPE)
		{
			int iInfluenceDifference = 0;
			int iIdeologyPressure = 0;
			int iPotentialExtraVotes = 1;
			// Look at each civ
			for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
				if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
				{
					PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
					if (eOtherCivIdeology != NO_POLICY_BRANCH_TYPE)
					{
						if (eOtherCivIdeology == eOurIdeology || kPlayer.GetDiplomacyAI()->IsVassal(GetPlayer()->GetID()))
						{
							iPotentialExtraVotes += 1;
						}
						else
						{
							iInfluenceDifference = m_pPlayer->GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer) - kPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID());
							if (iInfluenceDifference > 0)
							{
								iIdeologyPressure += iInfluenceDifference;
							}
						}
					}
				}
			}
			if (!bDiploVictoryEnabled)
			{
				iPotentialExtraVotes += 1;
				iPotentialExtraVotes /= 2;
			}
			if (eOurIdeology == eTargetIdeology)
			{
				iExtra += iPotentialExtraVotes * 100 + iIdeologyPressure * 50;
			}
			else
			{
				if (bForSelf)
				{
					iExtra -= iPotentialExtraVotes * 100 + iIdeologyPressure * 50;
				}
			}
		}

		iScore += iExtra;
	}
	// Arts Funding or Sciences Funding
	if (pProposal->GetEffects()->iArtsyGreatPersonRateMod != 0 ||
		pProposal->GetEffects()->iScienceyGreatPersonRateMod != 0)
	{
		iExtra = 0;
		int iArtsMod = pProposal->GetEffects()->iArtsyGreatPersonRateMod;
		int iScienceMod = pProposal->GetEffects()->iScienceyGreatPersonRateMod;

		// Do we have a sciencey Great Person unique unit? (ie. Merchant of Venice)
		bool bScienceyUniqueUnit = false;
		UnitClassTypes eScienceyUnitClass1 = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCIENTIST", true);
		UnitClassTypes eScienceyUnitClass2 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ENGINEER", true);
		UnitClassTypes eScienceyUnitClass3 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MERCHANT", true);

		CvUnitClassInfo* pScienceyUnitClassInfo1 = GC.getUnitClassInfo(eScienceyUnitClass1);
		CvUnitClassInfo* pScienceyUnitClassInfo2 = GC.getUnitClassInfo(eScienceyUnitClass2);
		CvUnitClassInfo* pScienceyUnitClassInfo3 = GC.getUnitClassInfo(eScienceyUnitClass3);

		UnitTypes eScienceyUnit1 = (UnitTypes) GetPlayer()->getCivilizationInfo().getCivilizationUnits(eScienceyUnitClass1);
		UnitTypes eDefault1 = (UnitTypes) pScienceyUnitClassInfo1->getDefaultUnitIndex();
		UnitTypes eScienceyUnit2 = (UnitTypes)GetPlayer()->getCivilizationInfo().getCivilizationUnits(eScienceyUnitClass2);
		UnitTypes eDefault2 = (UnitTypes)pScienceyUnitClassInfo2->getDefaultUnitIndex();
		UnitTypes eScienceyUnit3 = (UnitTypes)GetPlayer()->getCivilizationInfo().getCivilizationUnits(eScienceyUnitClass3);
		UnitTypes eDefault3 = (UnitTypes)pScienceyUnitClassInfo3->getDefaultUnitIndex();

		if (eScienceyUnit1 != eDefault1 || eScienceyUnit2 != eDefault2 || eScienceyUnit3 != eDefault3)
		{
			bScienceyUniqueUnit = true;
			iExtra += iScienceMod * 10;
		}
		int iNumGPImprovements = GetPlayer()->getGreatPersonImprovementCount();
		iExtra += (iNumGPImprovements * iScienceMod) / 2;


		// Do we have a artsy Great Person unique unit?
		bool bArtsyUniqueUnit = false;
		UnitClassTypes eArtsyUnitClass1 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_WRITER", true);
		UnitClassTypes eArtsyUnitClass2 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ARTIST", true);
		UnitClassTypes eArtsyUnitClass3 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MUSICIAN", true);

		CvUnitClassInfo* pArtsyUnitClassInfo1 = GC.getUnitClassInfo(eArtsyUnitClass1);
		CvUnitClassInfo* pArtsyUnitClassInfo2 = GC.getUnitClassInfo(eArtsyUnitClass2);
		CvUnitClassInfo* pArtsyUnitClassInfo3 = GC.getUnitClassInfo(eArtsyUnitClass3);

		UnitTypes eArtsyUnit1 = (UnitTypes)GetPlayer()->getCivilizationInfo().getCivilizationUnits(eArtsyUnitClass1);
		eDefault1 = (UnitTypes)pArtsyUnitClassInfo1->getDefaultUnitIndex();
		UnitTypes eArtsyUnit2 = (UnitTypes)GetPlayer()->getCivilizationInfo().getCivilizationUnits(eArtsyUnitClass2);
		eDefault2 = (UnitTypes)pArtsyUnitClassInfo2->getDefaultUnitIndex();
		UnitTypes eArtsyUnit3 = (UnitTypes)GetPlayer()->getCivilizationInfo().getCivilizationUnits(eArtsyUnitClass3);
		eDefault3 = (UnitTypes)pArtsyUnitClassInfo3->getDefaultUnitIndex();

		if (eArtsyUnit1 != eDefault1 || eArtsyUnit2 != eDefault2 || eArtsyUnit3 != eDefault3)
		{
			bArtsyUniqueUnit = true;
			iExtra += iArtsMod * 10;
		}

		int iNumGreatWorks = GetPlayer()->GetCulture()->GetNumGreatWorks();
		iExtra += (iNumGreatWorks * iArtsMod) / 2;
		
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
		{
			if (iArtsMod > 0)
			{
#if defined(MOD_BALANCE_CORE)

				if (GetPlayer()->AidRankGeneric(1) != NO_PLAYER)
				{
					iExtra += 15 * max(0, GetPlayer()->ScoreDifferencePercent(1) - 40); 
				}
				// can't have arts and science funding
				if (GetPlayer()->AidRankGeneric(2) != NO_PLAYER)
				{
					iExtra -= 5 * max(0, GetPlayer()->ScoreDifferencePercent(2) - 40);
				}
#else
			if (GetPlayer()->AidRank() != NO_PLAYER)
			{
				iExtra += 50;
				iExtra += GetPlayer()->ScoreDifference();
			}
#endif
			}
		}
#endif

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
		{
			if (iScienceMod > 0)
			{
#if defined(MOD_BALANCE_CORE)

				if (GetPlayer()->AidRankGeneric(2) != NO_PLAYER)
				{ 
					iExtra += 15 * max (0, GetPlayer()->ScoreDifferencePercent(2) - 40); 
				}
				// can't have arts and science funding
				if (GetPlayer()->AidRankGeneric(1) != NO_PLAYER)
				{
					iExtra -= 5 * max(0, GetPlayer()->ScoreDifferencePercent(1) - 40);
				}
#else
				if (GetPlayer()->AidRank() != NO_PLAYER)
				{
					iExtra += 50;
					iExtra += GetPlayer()->ScoreDifference();
				}
#endif
			}
		}
#endif

		iScore += iExtra;
	}

	// Historical Landmarks
	if (pProposal->GetEffects()->iGreatPersonTileImprovementCulture != 0 ||
		pProposal->GetEffects()->iLandmarkCulture != 0)
	{
		iExtra = 0;
		int iNumGPImprovements = GetPlayer()->getGreatPersonImprovementCount();
		int iNumLandmarks = 0;
		ImprovementTypes eLandmark = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK");
		if (eLandmark != NO_IMPROVEMENT)
		{
			iNumLandmarks += GetPlayer()->getImprovementCount(eLandmark);
		}
		
		iExtra += 30 * iNumGPImprovements + 40 * iNumLandmarks;

		iScore += iExtra;
	}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	//Open Door
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pProposal->GetEffects()->bOpenDoor)
	{
		iExtra = 0;
		if (eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isMinorCiv())
		{

			//Proposer...
			PlayerTypes ePlayer = GetPlayer()->GetID();
			int iInfluence = GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer);
			// can we get friend bonus from this?
			if (iInfluence <= GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetFriendsThreshold(ePlayer))
			{
				iExtra += range(GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetFriendsThreshold(ePlayer) - iInfluence, 0, 100);
			}
			// How much do we want to ally the city state?
			CivApproachTypes eApproach;
			eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eTargetCityState);
			int iAllyDesire = 0;
			if (eApproach != NO_CIV_APPROACH)
			{
				iAllyDesire = (eApproach - CIV_APPROACH_GUARDED) * 100;
			}
			PlayerProximityTypes eProximity;
			eProximity = GET_PLAYER(eTargetCityState).GetProximityToPlayer(ePlayer);
			int iProximity = 0;
			switch (eProximity)
			{
			case PLAYER_PROXIMITY_CLOSE:
				iProximity = 50;
				break;
			case PLAYER_PROXIMITY_NEIGHBORS:
				iProximity = 150;
				break;
			}
			// Do they have a resource we lack?
			int iResourceDesire = 75 * GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetNumResourcesMajorLacks(ePlayer);
			if (iResourceDesire < 0)
			{
				iResourceDesire = 0;
			}
			if (iAllyDesire > 0)
			{
				iAllyDesire = iAllyDesire + iProximity + iResourceDesire;
				PlayerTypes eAlliedPlayer = NO_PLAYER;
				eAlliedPlayer = GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetAlly();
				if (eAlliedPlayer != NO_PLAYER && eAlliedPlayer != ePlayer)
				{
					int iAllyInfluence = GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlliedPlayer);
					iAllyDesire *= (iInfluence * 100) / max(1,iAllyInfluence);
					iAllyDesire /= 100;
				}
			}
			if (iAllyDesire < 0)
			{
				int iTargetValue = 0;
				if (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(eTargetCityState) > TARGET_VALUE_AVERAGE)
				{
					iTargetValue = 100 * (GetPlayer()->GetDiplomacyAI()->GetPlayerTargetValue(eTargetCityState) - TARGET_VALUE_AVERAGE);
				}
				iAllyDesire = iAllyDesire - iTargetValue;
			}
			
			iExtra -= iAllyDesire;
		}

		iScore += iExtra;
	}

	// Decolonization - City-States
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pProposal->GetEffects()->bDecolonization)
	{
		iExtra = 0;
		PlayerTypes ePlayer = GetPlayer()->GetID();
		if (eTargetPlayer != NO_PLAYER)
		{
			if (ePlayer == eTargetPlayer)
			{
				int iAllies = 0;
				int iNearAllies = 0;
				for (int iMinor = MAX_MAJOR_CIVS; iMinor < MAX_CIV_PLAYERS; iMinor++)
				{
					PlayerTypes eMinor = (PlayerTypes)iMinor;
					if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
					{
						PlayerTypes eAlliedPlayer = NO_PLAYER;
						eAlliedPlayer = GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly();
						if (eAlliedPlayer == ePlayer)
						{
							if (GET_PLAYER(eMinor).GetMinorCivAI()->GetPermanentAlly() != ePlayer)
							{
								iAllies++;
							}
						}
						else if (eAlliedPlayer != NO_PLAYER && GET_PLAYER(eMinor).GetMinorCivAI()->GetPermanentAlly() != eAlliedPlayer)
						{
							if (GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer) * 100 >= 75 * GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlliedPlayer))
							{
								iNearAllies++;
							}
						}
					}
				}
				iExtra -= 100 * iAllies + 50 * iNearAllies;
			}
			else
			{
				int iAllies = 0;
				int iNearAllies = 0;
				for (int iMinor = MAX_MAJOR_CIVS; iMinor < MAX_CIV_PLAYERS; iMinor++)
				{
					PlayerTypes eMinor = (PlayerTypes)iMinor;
					if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
					{
						PlayerTypes eAlliedPlayer = NO_PLAYER;
						eAlliedPlayer = GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly();
						if (eAlliedPlayer == ePlayer)
						{
							if (GET_PLAYER(eMinor).GetMinorCivAI()->GetPermanentAlly() != ePlayer)
							{
								if (GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eTargetPlayer) * 100 >= 75 * GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer))
								{
									iAllies++;
								}
							}
						}
						else if (eAlliedPlayer == eTargetPlayer && GET_PLAYER(eMinor).GetMinorCivAI()->GetPermanentAlly() != eAlliedPlayer)
						{
							if (GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer) * 100 >= 75 * GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eAlliedPlayer))
							{
								iNearAllies++;
							}
						}
					}
				}
				iExtra += 50 * iAllies + 100 * iNearAllies;
			}

		}
		if (bDiploVictoryEnabled)
		{
			iExtra *= 2;
		}

		iScore += iExtra;
	}
	// Spaceship Control Committee
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pProposal->GetEffects()->iLimitSpaceshipProduction)
	{
		iExtra = 0;
		// If science Victory is disabled - don't care
		if (bScienceVictoryEnabled)
		{
			int iTeams = 0;
			int iTechs;
			int iTotalTechs = 0;
			int iOurTechs = 0;
			for (int iTeamLoop = 0; iTeamLoop < MAX_MAJOR_CIVS; iTeamLoop++)	// Looping over all MAJOR teams
			{
				TeamTypes eTeam = (TeamTypes)iTeamLoop;

				if (GET_TEAM(eTeam).isAlive())
				{
					iTeams++;
					iTechs = GET_TEAM(eTeam).GetTeamTechs()->GetNumTechsKnown();
					if (iTechs > 0)
					{
						iTotalTechs += iTechs;
						if (eTeam == GetPlayer()->getTeam())
						{
							iOurTechs = iTechs;
						}
					}
				}
			}
			// Gives a percent we are above or below global mean techs
			int iTechPercent = ((iOurTechs * iTeams * 100) / max(1,iTotalTechs)) - 100;
			if (iTechPercent > 0)
			{
				iExtra -= iTechPercent * 400;
			}
		}


		iScore += iExtra;
	}

	//COLD WAR
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pProposal->GetEffects()->bEmbargoIdeology)
	{
		iExtra = 0;
		PlayerTypes eLoopPlayer;
		PolicyBranchTypes ePlayerIdeology = GET_PLAYER(GetPlayer()->GetID()).GetPlayerPolicies()->GetLateGamePolicyTree();
		
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if(!GET_PLAYER(eLoopPlayer).isMinorCiv() && !GET_PLAYER(eLoopPlayer).isBarbarian() && GET_PLAYER(eLoopPlayer).isAlive() && eLoopPlayer != GetPlayer()->GetID())
			{
				PolicyBranchTypes eOtherIdeology = GET_PLAYER(eLoopPlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
				//How many connections do we have with people of other ideologies?
				if((eOtherIdeology != ePlayerIdeology) && !GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsVassal(GetPlayer()->GetID()))
				{
					// Trade connections
					if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(GetPlayer()->GetID(), eLoopPlayer))
					{
						iExtra -= 100 * GC.getGame().GetGameTrade()->CountNumPlayerConnectionsToPlayer(GetPlayer()->GetID(), eLoopPlayer);
					}

					// any people we like with different ideologies?
					CivOpinionTypes eOpinion = GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eLoopPlayer);
					if (eOpinion > CIV_OPINION_NEUTRAL)
					{
						iExtra -= 100 * (eOpinion - CIV_OPINION_NEUTRAL);
					}
				}
			}
		}
		for (int iMinor = MAX_MAJOR_CIVS; iMinor < MAX_CIV_PLAYERS; iMinor++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinor;
			if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
			{
				// do we have trade routes with non-alled CS?
				if(!GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(GetPlayer()->GetID()))
				{
					if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(GetPlayer()->GetID(), eMinor))
					{
						iExtra -= 100 * GC.getGame().GetGameTrade()->CountNumPlayerConnectionsToPlayer(GetPlayer()->GetID(), eMinor);
					}
					else
					{
						iExtra -= 25;
					}
				}
				// no influence decay
				else
				{
					iExtra += 50;
				}
			}
		}

		iScore += iExtra;
	}
#endif
#if defined(MOD_BALANCE_CORE)
	//Reducing Tourism
	if(pProposal->GetEffects()->iChangeTourism != 0)
	{
		iExtra = 0;
		// If culture Victory is disabled - don't care
		if (bCultureVictoryEnabled)
		{
			int iTourismChange = pProposal->GetEffects()->iChangeTourism;
			int iLowestPercent = GC.getCULTURE_LEVEL_INFLUENTIAL();   // Don't want to target civs if already influential

			for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
				if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
				{
					int iInfluenceOn = GetPlayer()->GetCulture()->GetInfluenceOn((PlayerTypes)iLoopPlayer);
					int iLifetimeCulture = kPlayer.GetJONSCultureEverGenerated();
					int iPercent = 0;

					if (iInfluenceOn > 0)
					{
						iPercent = (iInfluenceOn * 100) / max(1, iLifetimeCulture);
					}
					if (iPercent < 0)
					{
						iPercent = 0;
					}
					if (iPercent < iLowestPercent)
					{
						iLowestPercent = iPercent;
					}
				}
			}
			if (iLowestPercent > 0)
			{
				iExtra += 2 * (iTourismChange * iLowestPercent);
			}
		}
		iScore += iExtra;
	}
#endif
	if (!bEnact)
	{
		iScore *= -1; // Flip the score when the proposal is to repeal these effects
	}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	// Sphere of Influence - City-States
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && pProposal->GetEffects()->bSphereOfInfluence)
	{
		iExtra = 0;
		if (eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isMinorCiv())
		{
			PlayerTypes ePlayer = GetPlayer()->GetID();
			// How much do we want to ally the city state?
			CivApproachTypes eApproach;
			eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eTargetCityState);
			int iAllyDesire = 0;
			if (eApproach > CIV_APPROACH_GUARDED)
			{
				iAllyDesire = (eApproach - CIV_APPROACH_GUARDED) * 100;
			}
			if (iAllyDesire > 0)
			{
				PlayerProximityTypes eProximity;
				eProximity = GET_PLAYER(eTargetCityState).GetProximityToPlayer(ePlayer);
				switch (eProximity)
				{
				case PLAYER_PROXIMITY_CLOSE:
					iAllyDesire += 100;
					break;
				case PLAYER_PROXIMITY_NEIGHBORS:
					iAllyDesire += 250;
					break;
				}
			}
			// Do they have a resource we lack?
			int iResourcesWeLack = GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetNumResourcesMajorLacks(ePlayer);
			if (iResourcesWeLack > 0)
			{
				iAllyDesire += 100 * iResourcesWeLack;
			}
			int iInfluence = GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer);
			iInfluence = iInfluence / 2;
			if (iInfluence > iAllyDesire)
			{
				iInfluence = iAllyDesire;
			}
			if (bEnact)
			{
				if (ePlayer == eProposer)
				{
					iExtra += iAllyDesire;
					iExtra -= iInfluence;
				}
				else if (ePlayer == GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetAlly())
				{
					iExtra -= iAllyDesire;
					iExtra -= iInfluence;
				}
				else
				{
					if (bForSelf)
					{
						iExtra -= iAllyDesire;
						iExtra -= iInfluence;
					}
				}

			}
			else if (!bEnact)
			{
				if (GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetPermanentAlly() != NO_PLAYER)
				{
					if (GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetPermanentAlly() == ePlayer)
					{
						iExtra -= iAllyDesire;
						iExtra += iInfluence;
					}
					else
					{
						if (bForSelf)
						{
							iExtra += iAllyDesire;
							iExtra += iInfluence;
						}
					}

				}
			}
		}

		iScore += iExtra;
	}
#endif
	if (bConsiderGlobal)
	{
		// == Alignment with Proposer ==
		if (eProposer != NO_PLAYER)
		{
			AlignmentLevels eAlignment = EvaluateAlignment(eProposer, false);
			switch (eAlignment)
			{
			case ALIGNMENT_SELF:
			case ALIGNMENT_LIBERATOR:
			case ALIGNMENT_TEAMMATE:
			case ALIGNMENT_LEADER:
				iScore += 150;
				break;
			case ALIGNMENT_ALLY:
				iScore += 120;
				break;
			case ALIGNMENT_CONFIDANT:
				iScore += 80;
				break;
			case ALIGNMENT_FRIEND:
				iScore += 40;
				break;
			case ALIGNMENT_RIVAL:
				iScore += -40;
				break;
			case ALIGNMENT_HATRED:
				iScore += -80;
				break;
			case ALIGNMENT_ENEMY:
			case ALIGNMENT_WAR:
				iScore += -120;
				break;
			default:
				break;
			}
		}


	}


	// == Post-Processing ==
	if (iChoice == LeagueHelpers::CHOICE_NO)
	{
		iScore *= -1; // Flip the score when we are considering NO for these effects
	}


#if defined(MOD_BALANCE_CORE)
	//How much will friends/enemies care about this?
	if (bConsiderGlobal == true)
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		CvDiplomacyAI* pDiplo = GetPlayer()->GetDiplomacyAI();
		if (pLeague)
		{
			int iDiploScore = 0;
			int iDisputeLevel = 0;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (pLeague->CanEverVote(eLoopPlayer) && GET_PLAYER(eLoopPlayer).GetID() != GetPlayer()->GetID())
				{
					iDiploScore = 0;
					if (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eLoopPlayer) != NO_CIV_OPINION)
					{
						iDiploScore += (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eLoopPlayer) - CIV_OPINION_NEUTRAL) * 8;
					}
					if (GetPlayer()->GetDiplomacyAI()->GetMostValuableFriend() == eLoopPlayer)
					{
						iDiploScore += 8;
					}
					if (GetPlayer()->GetDiplomacyAI()->GetBiggestCompetitor() == eLoopPlayer)
					{
						iDiploScore -= 8;
					}
					if (GetPlayer()->GetDiplomacyAI()->GetPrimeLeagueCompetitor() == eLoopPlayer)
					{
						iDiploScore -= 8;
					}
					iDisputeLevel = max((int)pDiplo->GetVictoryDisputeLevel(eLoopPlayer), (int)pDiplo->GetVictoryBlockLevel(eLoopPlayer));
					if (iDisputeLevel > 0)
					{
						iDiploScore -= iDisputeLevel * 8;
					}
					AlignmentLevels eAlignment = EvaluateAlignment(eLoopPlayer, false);
					switch (eAlignment)
					{
					case ALIGNMENT_SELF:
					case ALIGNMENT_TEAMMATE:
					case ALIGNMENT_LIBERATOR:
					case ALIGNMENT_LEADER:
						iDiploScore = 100;
						break;
					case ALIGNMENT_ALLY:
						iDiploScore += 24;
						break;
					case ALIGNMENT_CONFIDANT:
						iDiploScore += 16;
						break;
					case ALIGNMENT_FRIEND:
						iDiploScore += 8;
						break;
					case ALIGNMENT_RIVAL:
						iDiploScore += -8;
						break;
					case ALIGNMENT_HATRED:
						iDiploScore += -16;
						break;
					case ALIGNMENT_ENEMY:
					case ALIGNMENT_WAR:
						iDiploScore += -24;
						break;
					default:
						break;
					}

					iDiploScore *= GET_PLAYER(eLoopPlayer).GetLeagueAI()->ScoreVoteChoiceYesNo(pProposal, iChoice, bEnact, /*bConsiderGlobal*/ false, /*bForSelf*/ false);
					iDiploScore /= 100;
					iScore += iDiploScore;
				}
			}
		}
	}
#endif

	return iScore;
}

// Score a particular choice on a particular proposal which is a decision between players (ex. Choose Host, World Leader)
int CvLeagueAI::ScoreVoteChoicePlayer(CvProposal* pProposal, int iChoice, bool bEnact)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return 0;
	CvAssertMsg(bEnact, "Unexpected case when evaluating vote choices for AI. Please send Anton your save file and version.");
	if (!bEnact) return 0;
	PlayerTypes eChoicePlayer = (PlayerTypes) iChoice;
	CvAssert(eChoicePlayer != NO_PLAYER);
	if (!(eChoicePlayer != NO_PLAYER)) return 0;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvAssert(pLeague != NULL);
	if (!(pLeague != NULL)) return 0;

	PlayerTypes ePlayer = GetPlayer()->GetID();
	// How much do we like this choice for this proposal?  Positive is like, negative is dislike
	int iScore = 0;
	int iOurVotes = pLeague->CalculateStartingVotesForMember(ePlayer);
	int iTheirVotes = pLeague->CalculateStartingVotesForMember(eChoicePlayer);
	int iNeededVotes = max(1,GC.getGame().GetVotesNeededForDiploVictory());
	int iTheirPercent = (iTheirVotes * 100) / iNeededVotes;
	int iOurPercent = (iOurVotes * 100) / iNeededVotes;
	AlignmentLevels eAlignment = EvaluateAlignment(eChoicePlayer, false);
	bool bIsLiked = GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eChoicePlayer) > CIV_OPINION_NEUTRAL;
	// == Diplomatic Victory ==
	if (pProposal->GetEffects()->bDiplomaticVictory)
	{
		switch (eAlignment)
		{
		case ALIGNMENT_LEADER:
			iScore += 100000;
			break;
		case ALIGNMENT_SELF:
			iScore += /*1000*/ GC.getAI_WORLD_LEADER_BASE_WEIGHT_SELF() + 30 * iOurPercent;
			break;
		case ALIGNMENT_TEAMMATE:
		case ALIGNMENT_LIBERATOR:
			iScore += /*1000*/ GC.getAI_WORLD_LEADER_BASE_WEIGHT_SELF() + 30 * iTheirPercent;
			break;
		case ALIGNMENT_ALLY:
			if (bIsLiked)
			{
				iScore += 25 * iTheirPercent;
			}
			break;
		case ALIGNMENT_CONFIDANT:
			if (bIsLiked)
			{
				iScore += 15 * iTheirPercent;
			}
			break;
		case ALIGNMENT_FRIEND:
			if (bIsLiked)
			{
				iScore += 10 * iTheirPercent;
			}
			break;
		case ALIGNMENT_RIVAL:
			iScore += -25 * iTheirPercent;
			break;
		case ALIGNMENT_HATRED:
			iScore += -100 * iTheirPercent;
			break;
		case ALIGNMENT_ENEMY:
		case ALIGNMENT_WAR:
			iScore += -250 * iTheirPercent;
			break;
		default:
			break;
		}
	}

	// == Choose Host ==
	// Mostly same as evaluating Diplomatic Victory, but with higher value to pick others you like
	if (pProposal->GetEffects()->bChangeLeagueHost)
	{
		switch (eAlignment)
		{
		case ALIGNMENT_LEADER:
			iScore += 100000;
			break;
		case ALIGNMENT_SELF:
			iScore += 200 * (iOurVotes + 1);
			break;
		case ALIGNMENT_TEAMMATE:
		case ALIGNMENT_LIBERATOR:
			iScore += 200 * (iTheirVotes + 1);
			break;
		case ALIGNMENT_ALLY:
			if (bIsLiked)
			{
				iScore += 150 * (iTheirVotes + 1);
			}
			break;
		case ALIGNMENT_CONFIDANT:
			if (bIsLiked)
			{
				iScore += 100 * (iTheirVotes + 1);
			}
			break;
		case ALIGNMENT_FRIEND:
			if (bIsLiked)
			{
				iScore += 50 * (iTheirVotes + 1);
			}
			break;
		case ALIGNMENT_RIVAL:
			iScore += -50 * (iTheirVotes + 1);
			break;
		case ALIGNMENT_HATRED:
			iScore += -150 * (iTheirVotes + 1);
			break;
		case ALIGNMENT_ENEMY:
		case ALIGNMENT_WAR:
			iScore += -300 * (iTheirVotes + 1);
			break;
		default:
			break;
		}
	}

	return iScore;
}

void CvLeagueAI::AllocateProposals(CvLeague* pLeague)
{
	ProposalConsiderationList vConsiderations;

	std::vector<ResolutionTypes> vInactive = pLeague->GetInactiveResolutions();
	ActiveResolutionList vActive = pLeague->GetActiveResolutions();

	if (!vActive.empty())
	{
		for (uint iResolutionIndex = 0; iResolutionIndex < vActive.size(); iResolutionIndex++)
		{
			int iID = vActive[iResolutionIndex].GetID();
			if (pLeague->CanProposeRepeal(iID, GetPlayer()->GetID()))
			{
				ProposalConsideration consideration(/*bEnact*/ false, iResolutionIndex, LeagueHelpers::CHOICE_NONE);
				int iScore = ScoreProposal(pLeague, &vActive[iResolutionIndex], GetPlayer()->GetID(), true);

				for (EnactProposalList::iterator it = pLeague->m_vLastTurnEnactProposals.begin(); it != pLeague->m_vLastTurnEnactProposals.end(); it++)
				{
					if (it->GetID() == vActive[iResolutionIndex].GetID())
						iScore /= 2;
				}

				for (RepealProposalList::iterator it = pLeague->m_vLastTurnRepealProposals.begin(); it != pLeague->m_vLastTurnRepealProposals.end(); it++)
				{
					if (it->GetID() == vActive[iResolutionIndex].GetID())
						iScore /= 2;
				}

				vConsiderations.push_back(consideration, iScore);
			}
		}
	}

	if (!vInactive.empty())
	{
		for (uint iResolutionIndex = 0; iResolutionIndex < vInactive.size(); iResolutionIndex++)
		{
			ResolutionTypes eResolution = vInactive[iResolutionIndex];
			CvResolutionEntry* pkInfo = GC.getResolutionInfo(eResolution);
			if (pkInfo)
			{
				std::vector<int> vAllChoices = pLeague->GetChoicesForDecision(pkInfo->GetProposerDecision(), GetPlayer()->GetID());
				if (vAllChoices.empty())
				{
					if (pLeague->CanProposeEnact(eResolution, GetPlayer()->GetID(), LeagueHelpers::CHOICE_NONE))
					{
						ProposalConsideration consideration(/*bEnact*/ true, iResolutionIndex, LeagueHelpers::CHOICE_NONE);
						int iScore = ScoreProposal(pLeague, eResolution, LeagueHelpers::CHOICE_NONE, GetPlayer()->GetID(), true);

						for (EnactProposalList::iterator it = pLeague->m_vLastTurnEnactProposals.begin(); it != pLeague->m_vLastTurnEnactProposals.end(); it++)
						{
							if (it->GetID() == pkInfo->GetID())
								iScore /= 2;
						}

						for (RepealProposalList::iterator it = pLeague->m_vLastTurnRepealProposals.begin(); it != pLeague->m_vLastTurnRepealProposals.end(); it++)
						{
							if (it->GetID() == pkInfo->GetID())
								iScore /= 2;
						}

						vConsiderations.push_back(consideration, iScore);
					}
				}
				else
				{
					for (uint i = 0; i < vAllChoices.size(); i++)
					{
						int iChoice = vAllChoices[i];
						if (pLeague->CanProposeEnact(eResolution, GetPlayer()->GetID(), iChoice))
						{
							ProposalConsideration consideration(/*bEnact*/ true, iResolutionIndex, iChoice);
							int iScore = ScoreProposal(pLeague, eResolution, iChoice, GetPlayer()->GetID(), true);

							for (EnactProposalList::iterator it = pLeague->m_vLastTurnEnactProposals.begin(); it != pLeague->m_vLastTurnEnactProposals.end(); it++)
							{
								if (it->GetID() == pkInfo->GetID())
									iScore /= 2;
							}

							for (RepealProposalList::iterator it = pLeague->m_vLastTurnRepealProposals.begin(); it != pLeague->m_vLastTurnRepealProposals.end(); it++)
							{
								if (it->GetID() == pkInfo->GetID())
									iScore /= 2;
							}

							vConsiderations.push_back(consideration, iScore);
						}
					}
				}
			}
		}
	}

	// Choose by weight from the top N
	CvAssertMsg(vConsiderations.size() > 0, "No proposals available for the AI to make. Please send Anton your save file and version.");

	if (vConsiderations.size() > 0)
	{
		vConsiderations.SortItems();

		for (int i = 0; i < vConsiderations.size(); i++)
		{
			ProposalConsideration proposal = vConsiderations.GetElement(i);
			LogProposalConsidered(&proposal, proposal.iChoice, vConsiderations.GetWeight(i), false);
		}

		// Even if we don't like anything, make sure we have something to choose from
		bool bNothingGood = true;
		for (int i = 0; i < vConsiderations.size(); i++)
		{
			if (vConsiderations.GetWeight(i) > 0)
			{
				bNothingGood = false;
				break;
			}
		}
		if (bNothingGood)
		{
			for (int i = 0; i < vConsiderations.size(); i++)
			{
				vConsiderations.SetWeight(i, 1000);
			}
		}
		RandomNumberDelegate fn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		ProposalConsideration proposal = vConsiderations.GetElement(0);
		if (proposal.bEnact)
		{
			ResolutionTypes eResolution = vInactive[proposal.iIndex];
			pLeague->DoProposeEnact(eResolution, GetPlayer()->GetID(), proposal.iChoice);
		}
		else
		{
			int iID = vActive[proposal.iIndex].GetID();
			pLeague->DoProposeRepeal(iID, GetPlayer()->GetID());
		}
	}
}

int CvLeagueAI::ScoreProposal(CvLeague* pLeague, ResolutionTypes eResolution, int iChoice, PlayerTypes eProposer, bool bConsiderGlobal)
{
	CvEnactProposal fakeProposal(/*iID*/-1, eResolution, pLeague->GetID(), eProposer, iChoice);

	// How much do we like our YES vote on this proposal?
	int iYesScore = 0;
	bool bFoundYes = false;
	ResolutionDecisionTypes eVoteDecision = fakeProposal.GetVoterDecision()->GetType();
	if (eVoteDecision == RESOLUTION_DECISION_YES_OR_NO)
	{
		std::vector<int> vVoteChoices = pLeague->GetChoicesForDecision(fakeProposal.GetVoterDecision()->GetType(), GetPlayer()->GetID());
		CvAssert(!vVoteChoices.empty());
		for (uint i = 0; i < vVoteChoices.size(); i++)
		{
			if (vVoteChoices[i] == LeagueHelpers::CHOICE_YES)
			{
				iYesScore = ScoreVoteChoice(&fakeProposal, vVoteChoices[i], bConsiderGlobal);
				bFoundYes = true;
				break;
			}
		}
	}
	CvAssert(bFoundYes);

	return iYesScore;
}

int CvLeagueAI::ScoreProposal(CvLeague* pLeague, CvActiveResolution* pResolution, PlayerTypes eProposer, bool bConsiderGlobal)
{
	CvRepealProposal fakeProposal(pResolution, eProposer);

	// How much do we like our YES vote on this proposal?
	int iYesScore = 0;
	bool bFoundYes = false;
	ResolutionDecisionTypes eVoteDecision = fakeProposal.GetRepealDecision()->GetType();
	if (eVoteDecision == RESOLUTION_DECISION_REPEAL)
	{
		std::vector<int> vVoteChoices = pLeague->GetChoicesForDecision(fakeProposal.GetRepealDecision()->GetType(), GetPlayer()->GetID());
		CvAssert(!vVoteChoices.empty());
		for (uint i = 0; i < vVoteChoices.size(); i++)
		{
			if (vVoteChoices[i] == LeagueHelpers::CHOICE_YES)
			{
				iYesScore = ScoreVoteChoice(&fakeProposal, vVoteChoices[i], bConsiderGlobal);
				bFoundYes = true;
				break;
			}
		}
	}
	CvAssert(bFoundYes);

	return iYesScore;
}

void CvLeagueAI::LogProposalConsidered(ProposalConsideration* pProposal, int iChoice, int iScore, bool bPre)
{
	std::vector<ResolutionTypes> vInactive = GC.getGame().GetGameLeagues()->GetActiveLeague()->GetInactiveResolutions();
	ActiveResolutionList vActive = GC.getGame().GetGameLeagues()->GetActiveLeague()->GetActiveResolutions();
	if (vInactive.empty())
		return;

	if (pProposal == NULL)
		return;

	ResolutionTypes eResolution = pProposal->bEnact ? vInactive[pProposal->iIndex] : vActive[pProposal->iIndex].GetType();
	if (GC.getResolutionInfo(eResolution) == NULL)
		return;

	CvString sMessage = "";
	sMessage += ",";
	sMessage += GetPlayer()->getCivilizationShortDescription();
	sMessage += ",- - -";
	sMessage += ",Evaluating for Proposal";
	if (pProposal->bEnact)
		sMessage += ", ENACT";
	else
		sMessage += ", REPEAL";
	if (!bPre)
		sMessage += ", POST SORT";

	sMessage += ",";
	CvAssert(pProposal != NULL);
	if (pProposal != NULL)
	{
		sMessage += GC.getResolutionInfo(eResolution)->GetDescription();
	}

	sMessage += ",";
	CvAssert(iChoice != LeagueHelpers::CHOICE_NONE);
	if (iChoice != LeagueHelpers::CHOICE_NONE)
	{
		sMessage += LeagueHelpers::GetTextForChoice(GC.getResolutionInfo(eResolution)->GetProposerDecision(), iChoice);
	}

	sMessage += ",";
	CvString sTemp;
	sTemp.Format("%d", iScore);
	sMessage += sTemp;

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}

void CvLeagueAI::LogVoteChoiceConsidered(CvEnactProposal* pProposal, int iChoice, int iScore)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return;
	CvString sMessage = "";

	sMessage += ",";
	sMessage += GetPlayer()->getCivilizationShortDescription();
	sMessage += ",- - -";
	sMessage += ",Evaluating for Delegates";

	sMessage += ",";
	CvAssert(pProposal != NULL);
	if (pProposal != NULL)
	{
		sMessage += pProposal->GetProposalName(/*bForLogging*/ true);
	}

	sMessage += ",";
	CvAssert(iChoice != LeagueHelpers::CHOICE_NONE);
	if (iChoice != LeagueHelpers::CHOICE_NONE)
	{
		sMessage += LeagueHelpers::GetTextForChoice(pProposal->GetVoterDecision()->GetType(), iChoice);
	}

	sMessage += ",";
	CvString sTemp;
	sTemp.Format("%d", iScore);
	sMessage += sTemp;

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}

void CvLeagueAI::LogVoteChoiceConsidered(CvRepealProposal* pProposal, int iChoice, int iScore)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return;
	CvString sMessage = "";

	sMessage += ",";
	sMessage += GetPlayer()->getCivilizationShortDescription();
	sMessage += ",- - -";
	sMessage += ",Evaluating for Delegates";

	sMessage += ",";
	CvAssert(pProposal != NULL);
	if (pProposal != NULL)
	{
		sMessage += pProposal->GetProposalName(/*bForLogging*/ true);
	}

	sMessage += ",";
	CvAssert(iChoice != LeagueHelpers::CHOICE_NONE);
	if (iChoice != LeagueHelpers::CHOICE_NONE)
	{
		sMessage += LeagueHelpers::GetTextForChoice(pProposal->GetRepealDecision()->GetType(), iChoice);
	}

	sMessage += ",";
	CvString sTemp;
	sTemp.Format("%d", iScore);
	sMessage += sTemp;

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}

void CvLeagueAI::LogVoteChoiceCommitted(CvEnactProposal* pProposal, int iChoice, int iVotes)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return;
	CvString sMessage = "";

	sMessage += ",";
	sMessage += GetPlayer()->getCivilizationShortDescription();
	sMessage += ",- - -";
	sMessage += ",Committing Delegates";

	sMessage += ",";
	CvAssert(pProposal != NULL);
	if (pProposal != NULL)
	{
		sMessage += pProposal->GetProposalName(/*bForLogging*/ true);
	}

	sMessage += ",";
	CvAssert(iChoice != LeagueHelpers::CHOICE_NONE);
	if (iChoice != LeagueHelpers::CHOICE_NONE)
	{
		sMessage += LeagueHelpers::GetTextForChoice(pProposal->GetVoterDecision()->GetType(), iChoice);
	}

	sMessage += ",";
	CvString sTemp;
	sTemp.Format("%d", iVotes);
	sMessage += sTemp;

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}

void CvLeagueAI::LogVoteChoiceCommitted(CvRepealProposal* pProposal, int iChoice, int iVotes)
{
	CvAssert(pProposal != NULL);
	if (!(pProposal != NULL)) return;
	CvString sMessage = "";

	sMessage += ",";
	sMessage += GetPlayer()->getCivilizationShortDescription();
	sMessage += ",- - -";
	sMessage += ",Committing Delegates";

	sMessage += ",";
	CvAssert(pProposal != NULL);
	if (pProposal != NULL)
	{
		sMessage += pProposal->GetProposalName(/*bForLogging*/ true);
	}

	sMessage += ",";
	CvAssert(iChoice != LeagueHelpers::CHOICE_NONE);
	if (iChoice != LeagueHelpers::CHOICE_NONE)
	{
		sMessage += LeagueHelpers::GetTextForChoice(pProposal->GetRepealDecision()->GetType(), iChoice);
	}

	sMessage += ",";
	CvString sTemp;
	sTemp.Format("%d", iVotes);
	sMessage += sTemp;

	GC.getGame().GetGameLeagues()->LogLeagueMessage(sMessage);
}


// ================================================================================
//			CvLeagueSpecialSessionEntry
// ================================================================================
CvLeagueSpecialSessionEntry::CvLeagueSpecialSessionEntry(void)
{
	m_eEraTrigger						= NO_ERA;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	m_eBuildingTrigger					= NO_BUILDING;
	m_eResolutionTrigger				= NO_RESOLUTION;
#endif
	m_eImmediateProposal				= NO_RESOLUTION;
	m_eRecurringProposal				= NO_RESOLUTION;
	m_iTurnsBetweenSessions				= 0;
	m_iCivDelegates						= 0;
	m_iHostDelegates					= 0;
	m_iCityStateDelegates				= 0;
	m_bUnitedNations					= false;
}

CvLeagueSpecialSessionEntry::~CvLeagueSpecialSessionEntry(void)
{
}

bool CvLeagueSpecialSessionEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
	{
		return false;
	}

	m_eEraTrigger						= (EraTypes) GC.getInfoTypeForString(kResults.GetText("EraTrigger"), true);
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
		m_eBuildingTrigger					= (BuildingTypes) GC.getInfoTypeForString(kResults.GetText("BuildingTrigger"), true);
		m_eResolutionTrigger				= (ResolutionTypes) GC.getInfoTypeForString(kResults.GetText("TriggerResolution"), true); 
	}
#endif
	m_eImmediateProposal				= (ResolutionTypes) GC.getInfoTypeForString(kResults.GetText("ImmediateProposal"), true);
	m_eRecurringProposal				= (ResolutionTypes) GC.getInfoTypeForString(kResults.GetText("RecurringProposal"), true);
	m_iTurnsBetweenSessions				= kResults.GetInt("TurnsBetweenSessions");
	m_iCivDelegates						= kResults.GetInt("CivDelegates");
	m_iHostDelegates					= kResults.GetInt("HostDelegates");
	m_iCityStateDelegates				= kResults.GetInt("CityStateDelegates");
	m_bUnitedNations					= kResults.GetBool("UnitedNations");

	return true;
}

EraTypes CvLeagueSpecialSessionEntry::GetEraTrigger() const
{
	return m_eEraTrigger;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
BuildingTypes CvLeagueSpecialSessionEntry::GetBuildingTrigger() const
{
	return m_eBuildingTrigger;
}
ResolutionTypes CvLeagueSpecialSessionEntry::GetResolutionTrigger() const
{
	return m_eResolutionTrigger;
}
#endif

ResolutionTypes CvLeagueSpecialSessionEntry::GetImmediateProposal() const
{
	return m_eImmediateProposal;
}

ResolutionTypes CvLeagueSpecialSessionEntry::GetRecurringProposal() const
{
	return m_eRecurringProposal;
}

int CvLeagueSpecialSessionEntry::GetTurnsBetweenSessions() const
{
	return m_iTurnsBetweenSessions;
}

int CvLeagueSpecialSessionEntry::GetCivDelegates() const
{
	return m_iCivDelegates;
}

int CvLeagueSpecialSessionEntry::GetHostDelegates() const
{
	return m_iHostDelegates;
}

int CvLeagueSpecialSessionEntry::GetCityStateDelegates() const
{
	return m_iCityStateDelegates;
}

bool CvLeagueSpecialSessionEntry::IsUnitedNations() const
{
	return m_bUnitedNations;
}


// ================================================================================
//			CvLeagueSpecialSessionXMLEntries
// ================================================================================
CvLeagueSpecialSessionXMLEntries::CvLeagueSpecialSessionXMLEntries(void)
{
}

CvLeagueSpecialSessionXMLEntries::~CvLeagueSpecialSessionXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of league project entries
std::vector<CvLeagueSpecialSessionEntry*>& CvLeagueSpecialSessionXMLEntries::GetLeagueSpecialSessionEntries()
{
	return m_paLeagueSpecialSessionEntries;
}

// Returns number of defined league projects
int CvLeagueSpecialSessionXMLEntries::GetNumLeagueSpecialSessions()
{
	return m_paLeagueSpecialSessionEntries.size();
}

// Clears league project entries
void CvLeagueSpecialSessionXMLEntries::DeleteArray()
{
	for (std::vector<CvLeagueSpecialSessionEntry*>::iterator it = m_paLeagueSpecialSessionEntries.begin(); it != m_paLeagueSpecialSessionEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paLeagueSpecialSessionEntries.clear();
}

// Returns a specific league project entry
CvLeagueSpecialSessionEntry* CvLeagueSpecialSessionXMLEntries::GetEntry(int index)
{
	return m_paLeagueSpecialSessionEntries[index];
}


// ================================================================================
//			CvLeagueNameEntry
// ================================================================================
CvLeagueNameEntry::CvLeagueNameEntry(void)
: m_iID(-1)
{
}

CvLeagueNameEntry::~CvLeagueNameEntry(void)
{
}

bool CvLeagueNameEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility&)
{
	m_iID				= kResults.GetInt("ID");
	m_strType			= kResults.GetText("Type");
	m_strNameKey = kResults.GetText("NameKey");
	
	return true;
}


// ================================================================================
//			CvLeagueNameXMLEntries
// ================================================================================
CvLeagueNameXMLEntries::CvLeagueNameXMLEntries(void)
{
}

CvLeagueNameXMLEntries::~CvLeagueNameXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of league name entries
std::vector<CvLeagueNameEntry*>& CvLeagueNameXMLEntries::GetLeagueNameEntries()
{
	return m_paLeagueNameEntries;
}

// Returns number of defined league names
int CvLeagueNameXMLEntries::GetNumLeagueNames()
{
	return m_paLeagueNameEntries.size();
}

// Clears league name entries
void CvLeagueNameXMLEntries::DeleteArray()
{
	for (std::vector<CvLeagueNameEntry*>::iterator it = m_paLeagueNameEntries.begin(); it != m_paLeagueNameEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paLeagueNameEntries.clear();
}

// Returns a specific league name entry
CvLeagueNameEntry* CvLeagueNameXMLEntries::GetEntry(int index)
{
	return m_paLeagueNameEntries[index];
}


// ================================================================================
//			CvLeagueProjectRewardEntry
// ================================================================================
CvLeagueProjectRewardEntry::CvLeagueProjectRewardEntry(void)
{
	m_eBuilding							= NO_BUILDING;
	m_iHappiness						= 0;
	m_iFreeSocialPolicies				= 0;
	m_iCultureBonusTurns				= 0;
	m_iTourismBonusTurns				= 0;
	m_iGoldenAgePoints					= 0;
	m_iCityStateInfluenceBoost			= 0;
	m_iBaseBeakersTurnsToCount			= 0;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	m_iGetAttackBonusTurns				= 0;
	m_iGetBaseFreeUnits					= 0;
	m_iGetNumFreeGreatPeople			= 0;
#endif
	m_eFreeUnitClass					= NO_UNITCLASS;
}

CvLeagueProjectRewardEntry::~CvLeagueProjectRewardEntry(void)
{
}

bool CvLeagueProjectRewardEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
	{
		return false;
	}

	m_eBuilding							= (BuildingTypes) GC.getInfoTypeForString(kResults.GetText("Building"), true);
	m_iHappiness						= kResults.GetInt("Happiness");
	m_iFreeSocialPolicies				= kResults.GetInt("FreeSocialPolicies");
	m_iCultureBonusTurns				= kResults.GetInt("CultureBonusTurns");
	m_iTourismBonusTurns				= kResults.GetInt("TourismBonusTurns");
	m_iGoldenAgePoints					= kResults.GetInt("GoldenAgePoints");
	m_iCityStateInfluenceBoost			= kResults.GetInt("CityStateInfluenceBoost");
	m_iBaseBeakersTurnsToCount			= kResults.GetInt("BaseBeakersTurnsToCount");
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
		m_iGetAttackBonusTurns				= kResults.GetInt("AttackBonusTurns");
		m_iGetBaseFreeUnits					= kResults.GetInt("BaseFreeUnits");
		m_iGetNumFreeGreatPeople			= kResults.GetInt("GetNumFreeGreatPeople");
	}
#endif
	m_eFreeUnitClass					= (UnitClassTypes) GC.getInfoTypeForString(kResults.GetText("FreeUnitClass"), true);

	return true;
}

BuildingTypes CvLeagueProjectRewardEntry::GetBuilding() const
{
	return m_eBuilding;
}

int CvLeagueProjectRewardEntry::GetHappiness() const
{
	return m_iHappiness;
}

int CvLeagueProjectRewardEntry::GetFreeSocialPolicies() const
{
	return m_iFreeSocialPolicies;
}

int CvLeagueProjectRewardEntry::GetCultureBonusTurns() const
{
	return m_iCultureBonusTurns;
}

int CvLeagueProjectRewardEntry::GetTourismBonusTurns() const
{
	return m_iTourismBonusTurns;
}

int CvLeagueProjectRewardEntry::GetGoldenAgePoints() const
{
	return m_iGoldenAgePoints;
}

int CvLeagueProjectRewardEntry::GetCityStateInfluenceBoost() const
{
	return m_iCityStateInfluenceBoost;
}

int CvLeagueProjectRewardEntry::GetBaseBeakersTurnsToCount() const
{
	return m_iBaseBeakersTurnsToCount;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
int CvLeagueProjectRewardEntry::GetAttackBonusTurns() const
{
	return m_iGetAttackBonusTurns;
}

int CvLeagueProjectRewardEntry::GetBaseFreeUnits() const
{
	return m_iGetBaseFreeUnits;
}

int CvLeagueProjectRewardEntry::GetNumFreeGreatPeople() const
{
	return m_iGetNumFreeGreatPeople;
}
#endif

UnitClassTypes CvLeagueProjectRewardEntry::GetFreeUnitClass() const
{
	return m_eFreeUnitClass;
}


// ================================================================================
//			CvLeagueProjectRewardXMLEntries
// ================================================================================
CvLeagueProjectRewardXMLEntries::CvLeagueProjectRewardXMLEntries(void)
{
}

CvLeagueProjectRewardXMLEntries::~CvLeagueProjectRewardXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of league project entries
std::vector<CvLeagueProjectRewardEntry*>& CvLeagueProjectRewardXMLEntries::GetLeagueProjectRewardEntries()
{
	return m_paLeagueProjectRewardEntries;
}

// Returns number of defined league projects
int CvLeagueProjectRewardXMLEntries::GetNumLeagueProjectRewards()
{
	return m_paLeagueProjectRewardEntries.size();
}

// Clears league project entries
void CvLeagueProjectRewardXMLEntries::DeleteArray()
{
	for (std::vector<CvLeagueProjectRewardEntry*>::iterator it = m_paLeagueProjectRewardEntries.begin(); it != m_paLeagueProjectRewardEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paLeagueProjectRewardEntries.clear();
}

// Returns a specific league project entry
CvLeagueProjectRewardEntry* CvLeagueProjectRewardXMLEntries::GetEntry(int index)
{
	return m_paLeagueProjectRewardEntries[index];
}


// ================================================================================
//			CvLeagueProjectEntry
// ================================================================================
CvLeagueProjectEntry::CvLeagueProjectEntry(void)
{
	m_eProcess							= NO_PROCESS;
	m_iCostPerPlayer					= 0;
	m_eRewardTier1						= NO_LEAGUE_PROJECT_REWARD;
	m_eRewardTier2						= NO_LEAGUE_PROJECT_REWARD;
	m_eRewardTier3						= NO_LEAGUE_PROJECT_REWARD;
}

CvLeagueProjectEntry::~CvLeagueProjectEntry(void)
{
}

bool CvLeagueProjectEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
	{
		return false;
	}

	m_eProcess							= (ProcessTypes) GC.getInfoTypeForString(kResults.GetText("Process"), true);
	m_iCostPerPlayer					= kResults.GetInt("CostPerPlayer");
	m_eRewardTier1						= (LeagueProjectRewardTypes) GC.getInfoTypeForString(kResults.GetText("RewardTier1"), true);
	m_eRewardTier2						= (LeagueProjectRewardTypes) GC.getInfoTypeForString(kResults.GetText("RewardTier2"), true);
	m_eRewardTier3						= (LeagueProjectRewardTypes) GC.getInfoTypeForString(kResults.GetText("RewardTier3"), true);

	return true;
}

ProcessTypes CvLeagueProjectEntry::GetProcess() const
{
	return m_eProcess;
}

int CvLeagueProjectEntry::GetCostPerPlayer() const
{
	return m_iCostPerPlayer;
}

LeagueProjectRewardTypes CvLeagueProjectEntry::GetRewardTier1() const
{
	return m_eRewardTier1;
}

LeagueProjectRewardTypes CvLeagueProjectEntry::GetRewardTier2() const
{
	return m_eRewardTier2;
}

LeagueProjectRewardTypes CvLeagueProjectEntry::GetRewardTier3() const
{
	return m_eRewardTier3;
}


// ================================================================================
//			CvLeagueProjectXMLEntries
// ================================================================================
CvLeagueProjectXMLEntries::CvLeagueProjectXMLEntries(void)
{
}

CvLeagueProjectXMLEntries::~CvLeagueProjectXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of league project entries
std::vector<CvLeagueProjectEntry*>& CvLeagueProjectXMLEntries::GetLeagueProjectEntries()
{
	return m_paLeagueProjectEntries;
}

// Returns number of defined league projects
int CvLeagueProjectXMLEntries::GetNumLeagueProjects()
{
	return m_paLeagueProjectEntries.size();
}

// Clears league project entries
void CvLeagueProjectXMLEntries::DeleteArray()
{
	for (std::vector<CvLeagueProjectEntry*>::iterator it = m_paLeagueProjectEntries.begin(); it != m_paLeagueProjectEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paLeagueProjectEntries.clear();
}

// Returns a specific league project entry
CvLeagueProjectEntry* CvLeagueProjectXMLEntries::GetEntry(int index)
{
	return m_paLeagueProjectEntries[index];
}


// ================================================================================
//			CvResolutionEntry
// ================================================================================
CvResolutionEntry::CvResolutionEntry(void)
{
	m_eVoterDecision					= RESOLUTION_DECISION_NONE;
	m_eProposerDecision					= RESOLUTION_DECISION_NONE;
	m_eTechPrereqAnyMember				= NO_TECH;
	m_bAutomaticProposal				= false;
	m_bUniqueType						= false;
	m_bNoProposalByPlayer				= false;
	m_iQuorumPercent					= 0;
	m_iLeadersVoteBonusOnFail			= 0;
	m_bDiplomaticVictory				= false;
	m_bChangeLeagueHost					= false;
	m_iOneTimeGold						= 0;
	m_iOneTimeGoldPercent				= 0;
	m_bRaiseCityStateInfluenceToNeutral	= false;
	m_eLeagueProjectEnabled				= NO_LEAGUE_PROJECT;
	m_iGoldPerTurn						= 0;
	m_iResourceQuantity					= 0;
	m_bEmbargoCityStates				= false;
	m_bEmbargoPlayer					= false;
	m_bNoResourceHappiness				= false;
	m_iUnitMaintenanceGoldPercent		= 0;
	m_iMemberDiscoveredTechMod			= 0;
	m_iCulturePerWonder					= 0;
	m_iCulturePerNaturalWonder			= 0;
	m_bNoTrainingNuclearWeapons			= false;
	m_iVotesForFollowingReligion		= 0;
	m_iHolyCityTourism					= 0;
	m_iReligionSpreadStrengthMod		= 0;
	m_iVotesForFollowingIdeology		= 0;
	m_iOtherIdeologyRebellionMod		= 0;
	m_iArtsyGreatPersonRateMod			= 0;
	m_iScienceyGreatPersonRateMod		= 0;
	m_iGreatPersonTileImprovementCulture= 0;
	m_iLandmarkCulture					= 0;
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	m_bOpenDoor	= false;
	m_bSphereOfInfluence = false;
	m_bDecolonization = false;
	m_iSpaceshipProductionMod			= 0;
	m_iSpaceshipPurchaseMod				= 0;
	m_iIsWorldWar						= 0;
	m_bEmbargoIdeology					= false;
#endif
#if defined(MOD_BALANCE_CORE)
	m_iTourismMod						= 0;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_iVassalMaintenanceGoldPercent		= 0;
	m_bEndAllCurrentVassals				= false;
#endif
}

CvResolutionEntry::~CvResolutionEntry(void)
{
}

bool CvResolutionEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
	{
		return false;
	}

	m_eVoterDecision					= (ResolutionDecisionTypes) GC.getInfoTypeForString(kResults.GetText("VoterDecision"), true);
	m_eProposerDecision					= (ResolutionDecisionTypes) GC.getInfoTypeForString(kResults.GetText("ProposerDecision"), true);
	m_eTechPrereqAnyMember				= (TechTypes) GC.getInfoTypeForString(kResults.GetText("TechPrereqAnyMember"), true);
	m_bAutomaticProposal				= kResults.GetBool("AutomaticProposal");
	m_bUniqueType						= kResults.GetBool("UniqueType");
	m_bNoProposalByPlayer				= kResults.GetBool("NoProposalByPlayer");
	m_iQuorumPercent					= kResults.GetInt("QuorumPercent");
	m_iLeadersVoteBonusOnFail			= kResults.GetInt("LeadersVoteBonusOnFail");
	m_bDiplomaticVictory				= kResults.GetBool("DiplomaticVictory");
	m_bChangeLeagueHost					= kResults.GetBool("ChangeLeagueHost");
	m_iOneTimeGold						= kResults.GetInt("OneTimeGold");
	m_iOneTimeGoldPercent				= kResults.GetInt("OneTimeGoldPercent");
	m_bRaiseCityStateInfluenceToNeutral	= kResults.GetBool("RaiseCityStateInfluenceToNeutral");
	m_eLeagueProjectEnabled				= (LeagueProjectTypes) GC.getInfoTypeForString(kResults.GetText("LeagueProjectEnabled"), true);
	m_iGoldPerTurn						= kResults.GetInt("GoldPerTurn");
	m_iResourceQuantity					= kResults.GetInt("ResourceQuantity");
	m_bEmbargoCityStates				= kResults.GetBool("EmbargoCityStates");
	m_bEmbargoPlayer					= kResults.GetBool("EmbargoPlayer");
	m_bNoResourceHappiness				= kResults.GetBool("NoResourceHappiness");
	m_iUnitMaintenanceGoldPercent		= kResults.GetInt("UnitMaintenanceGoldPercent");
	m_iMemberDiscoveredTechMod			= kResults.GetInt("MemberDiscoveredTechMod");
	m_iCulturePerWonder					= kResults.GetInt("CulturePerWonder");
	m_iCulturePerNaturalWonder			= kResults.GetInt("CulturePerNaturalWonder");
	m_bNoTrainingNuclearWeapons			= kResults.GetBool("NoTrainingNuclearWeapons");
	m_iVotesForFollowingReligion		= kResults.GetInt("VotesForFollowingReligion");
	m_iHolyCityTourism					= kResults.GetInt("HolyCityTourism");
	m_iReligionSpreadStrengthMod		= kResults.GetInt("ReligionSpreadStrengthMod");
	m_iVotesForFollowingIdeology		= kResults.GetInt("VotesForFollowingIdeology");
	m_iOtherIdeologyRebellionMod		= kResults.GetInt("OtherIdeologyRebellionMod");
	m_iArtsyGreatPersonRateMod			= kResults.GetInt("ArtsyGreatPersonRateMod");
	m_iScienceyGreatPersonRateMod		= kResults.GetInt("ScienceyGreatPersonRateMod");
	m_iGreatPersonTileImprovementCulture= kResults.GetInt("GreatPersonTileImprovementCulture");
	m_iLandmarkCulture					= kResults.GetInt("LandmarkCulture");
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) {
		m_bOpenDoor							= kResults.GetBool("OpenDoor");
		m_bSphereOfInfluence				= kResults.GetBool("SphereOfInfluence");
		m_bDecolonization					= kResults.GetBool("Decolonization");
		m_iSpaceshipProductionMod			= kResults.GetInt("SpaceshipProductionMod");
		m_iSpaceshipPurchaseMod				= kResults.GetInt("SpaceshipPurchaseMod");
		m_iIsWorldWar						= kResults.GetInt("IsWorldWar");
		m_bEmbargoIdeology					= kResults.GetBool("EmbargoIdeology");
	}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		m_iVassalMaintenanceGoldPercent		= kResults.GetInt("VassalMaintenanceGoldPercent");
		m_bEndAllCurrentVassals				= kResults.GetInt("EndAllCurrentVassals");
	}
#endif
#if defined(MOD_BALANCE_CORE)
	m_iTourismMod					= kResults.GetInt("TourismMod");
#endif

	return true;
}

ResolutionDecisionTypes CvResolutionEntry::GetVoterDecision() const
{
	return m_eVoterDecision;
}

ResolutionDecisionTypes CvResolutionEntry::GetProposerDecision() const
{
	return m_eProposerDecision;
}

TechTypes CvResolutionEntry::GetTechPrereqAnyMember() const
{
	return m_eTechPrereqAnyMember;
}

bool CvResolutionEntry::IsAutomaticProposal() const
{
	return m_bAutomaticProposal;
}

bool CvResolutionEntry::IsUniqueType() const
{
	return m_bUniqueType;
}

bool CvResolutionEntry::IsNoProposalByPlayer() const
{
	return m_bNoProposalByPlayer;
}

int CvResolutionEntry::GetQuorumPercent() const
{
	return m_iQuorumPercent;
}

int CvResolutionEntry::GetLeadersVoteBonusOnFail() const
{
	return m_iLeadersVoteBonusOnFail;
}

bool CvResolutionEntry::IsDiplomaticVictory() const
{
	return m_bDiplomaticVictory;
}

bool CvResolutionEntry::IsChangeLeagueHost() const
{
	return m_bChangeLeagueHost;
}

int CvResolutionEntry::GetOneTimeGold() const
{
	return m_iOneTimeGold;
}

int CvResolutionEntry::GetOneTimeGoldPercent() const
{
	return m_iOneTimeGoldPercent;
}

bool CvResolutionEntry::IsRaiseCityStateInfluenceToNeutral() const
{
	return m_bRaiseCityStateInfluenceToNeutral;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
bool CvResolutionEntry::IsOpenDoor() const
{
	return MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && m_bOpenDoor;
}

bool CvResolutionEntry::IsSphereOfInfluence() const
{
	return MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && m_bSphereOfInfluence;
}
bool CvResolutionEntry::IsDecolonization() const
{
	return MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS && m_bDecolonization;
}
#endif

LeagueProjectTypes CvResolutionEntry::GetLeagueProjectEnabled() const
{
	return m_eLeagueProjectEnabled;
}

int CvResolutionEntry::GetGoldPerTurn() const
{
	return m_iGoldPerTurn;
}

int CvResolutionEntry::GetResourceQuantity() const
{
	return m_iResourceQuantity;
}

bool CvResolutionEntry::IsEmbargoCityStates() const
{
	return m_bEmbargoCityStates;
}

bool CvResolutionEntry::IsEmbargoPlayer() const
{
	return m_bEmbargoPlayer;
}

bool CvResolutionEntry::IsNoResourceHappiness() const
{
	return m_bNoResourceHappiness;
}

int CvResolutionEntry::GetUnitMaintenanceGoldPercent() const
{
	return m_iUnitMaintenanceGoldPercent;
}

int CvResolutionEntry::GetMemberDiscoveredTechMod() const
{
	return m_iMemberDiscoveredTechMod;
}

int CvResolutionEntry::GetCulturePerWonder() const
{
	return m_iCulturePerWonder;
}

int CvResolutionEntry::GetCulturePerNaturalWonder() const
{
	return m_iCulturePerNaturalWonder;
}

bool CvResolutionEntry::IsNoTrainingNuclearWeapons() const
{
	return m_bNoTrainingNuclearWeapons;
}

int CvResolutionEntry::GetVotesForFollowingReligion() const
{
	return m_iVotesForFollowingReligion;
}

int CvResolutionEntry::GetHolyCityTourism() const
{
	return m_iHolyCityTourism;
}

int CvResolutionEntry::GetReligionSpreadStrengthMod() const
{
	return m_iReligionSpreadStrengthMod;
}

int CvResolutionEntry::GetVotesForFollowingIdeology() const
{
	return m_iVotesForFollowingIdeology;
}

int CvResolutionEntry::GetOtherIdeologyRebellionMod() const
{
	return m_iOtherIdeologyRebellionMod;
}

int CvResolutionEntry::GetArtsyGreatPersonRateMod() const
{
	return m_iArtsyGreatPersonRateMod;
}

int CvResolutionEntry::GetScienceyGreatPersonRateMod() const
{
	return m_iScienceyGreatPersonRateMod;
}

int CvResolutionEntry::GetGreatPersonTileImprovementCulture() const
{
	return m_iGreatPersonTileImprovementCulture;
}

int CvResolutionEntry::GetLandmarkCulture() const
{
	return m_iLandmarkCulture;
}

#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
int CvResolutionEntry::GetSpaceShipProductionMod() const
{
	return m_iSpaceshipProductionMod;
}
int CvResolutionEntry::GetSpaceShipPurchaseMod() const
{
	return m_iSpaceshipPurchaseMod;
}
int CvResolutionEntry::GetWorldWar() const
{
	return m_iIsWorldWar;
}
bool CvResolutionEntry::IsEmbargoIdeology() const
{
	return m_bEmbargoIdeology;
}
#endif
#if defined(MOD_BALANCE_CORE)
int CvResolutionEntry::GetTourismMod() const
{
	return m_iTourismMod;
}
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
int CvResolutionEntry::GetVassalMaintenanceGoldPercent() const
{
	return m_iVassalMaintenanceGoldPercent;
}
bool CvResolutionEntry::IsEndAllCurrentVassals() const
{
	return m_bEndAllCurrentVassals;
}
#endif


// ================================================================================
//			CvResolutionXMLEntries
// ================================================================================
CvResolutionXMLEntries::CvResolutionXMLEntries(void)
{
}

CvResolutionXMLEntries::~CvResolutionXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of resolution entries
std::vector<CvResolutionEntry*>& CvResolutionXMLEntries::GetResolutionEntries()
{
	return m_paResolutionEntries;
}

// Returns number of defined resolutions
int CvResolutionXMLEntries::GetNumResolutions()
{
	return m_paResolutionEntries.size();
}

// Clears resolution entries
void CvResolutionXMLEntries::DeleteArray()
{
	for (std::vector<CvResolutionEntry*>::iterator it = m_paResolutionEntries.begin(); it != m_paResolutionEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paResolutionEntries.clear();
}

// Returns a specific resolution entry
CvResolutionEntry* CvResolutionXMLEntries::GetEntry(int index)
{
	return m_paResolutionEntries[index];
}

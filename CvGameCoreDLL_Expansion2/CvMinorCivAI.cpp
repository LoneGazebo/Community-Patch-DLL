/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvMinorCivAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvEnumSerialization.h"
#include "CvNotifications.h"
#include "CvDiplomacyAI.h"
#include "CvDllInterfaces.h"
#include "CvDllPlot.h"
#include "cvStopWatch.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvBarbarians.h"

// must be included after all other headers
#include "LintFree.h"

//======================================================================================================
//					CvMinorCivQuest
//======================================================================================================

/// Default Constructor
CvMinorCivQuest::CvMinorCivQuest()
{
	m_eMinor = NO_PLAYER;
	m_eAssignedPlayer = NO_PLAYER;
	m_eType = NO_MINOR_CIV_QUEST_TYPE;
	m_iStartTurn = NO_TURN; /* -1 */
	m_iData1 = NO_QUEST_DATA; /* -1 */
	m_iData2 = NO_QUEST_DATA; /* -1 */
	m_iData3 = NO_QUEST_DATA; /* -1 */
	m_iInfluence = 0;
	m_iDisabledInfluence = 0;
	m_iGold = 0;
	m_iScience = 0;
	m_iCulture = 0;
	m_iFaith = 0;
	m_iGoldenAgePoints = 0;
	m_iFood = 0;
	m_iProduction = 0;
	m_iTourism = 0;
	m_iHappiness = 0;
	m_iGP = 0;
	m_iGPGlobal = 0;
	m_iGeneralPoints = 0;
	m_iAdmiralPoints = 0;
	m_iExperience = 0;
	m_bPartialQuest = false;
	m_bHandled = false;
}

/// Constructor
CvMinorCivQuest::CvMinorCivQuest(PlayerTypes eMinor, PlayerTypes eAssignedPlayer, MinorCivQuestTypes eType)
{
	m_eMinor = eMinor;
	m_eAssignedPlayer = eAssignedPlayer;
	m_eType = eType;
	m_iStartTurn = NO_TURN; /* -1 */
	m_iData1 = NO_QUEST_DATA; /* -1 */
	m_iData2 = NO_QUEST_DATA; /* -1 */
	m_iData3 = NO_QUEST_DATA; /* -1 */
	m_iInfluence = 0;
	m_iDisabledInfluence = 0;
	m_iGold = 0;
	m_iScience = 0;
	m_iCulture = 0;
	m_iFaith = 0;
	m_iGoldenAgePoints = 0;
	m_iFood = 0;
	m_iProduction = 0;
	m_iTourism = 0;
	m_iHappiness = 0;
	m_iGP = 0;
	m_iGPGlobal = 0;
	m_iGeneralPoints = 0;
	m_iAdmiralPoints = 0;
	m_iExperience = 0;
	m_bPartialQuest = false;
	m_bHandled = false;
}

CvMinorCivQuest::~CvMinorCivQuest()
{
}

/// The minor that gave this quest
PlayerTypes CvMinorCivQuest::GetMinor() const
{
	return m_eMinor;
}

/// The player the quest was given to
PlayerTypes CvMinorCivQuest::GetPlayerAssignedTo() const
{
	return m_eAssignedPlayer;
}

/// Type of quest
MinorCivQuestTypes CvMinorCivQuest::GetType() const
{
	return m_eType;
}

/// Turn that quest began
int CvMinorCivQuest::GetStartTurn() const
{
	return m_iStartTurn;
}

/// Turn that quest ends, based on start turn. Some quests have no specified end turn.
int CvMinorCivQuest::GetEndTurn() const
{
	CvSmallAwardInfo* pkSmallAwardInfo = GC.getSmallAwardInfo((SmallAwardTypes)m_eType);
	if (pkSmallAwardInfo)
	{
		int iDuration = pkSmallAwardInfo->GetDuration();
		if (iDuration > 0) // > 0 if the quest is time-sensitive
		{
			// Horde/Rebellion don't scale with game speed
			if (m_eType == MINOR_CIV_QUEST_HORDE || m_eType == MINOR_CIV_QUEST_REBELLION)
			{
				return m_iStartTurn + iDuration;
			}

			// Modify for game speed
			iDuration *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
			iDuration /= 100;

			return m_iStartTurn + iDuration;
		}
	}

	// Other quests are not time-sensitive
	return NO_TURN;
}

/// How many turns left until quest is over?
int CvMinorCivQuest::GetTurnsRemaining(int iCurrentTurn) const
{
	int iStartTurn = GetStartTurn();
	int iEndTurn = GetEndTurn();

	if (iStartTurn == NO_TURN)
		return NO_TURN;

	if (iEndTurn == NO_TURN)
		return NO_TURN;

	return iEndTurn - iCurrentTurn;
}

/// Quest-specific data 1
int CvMinorCivQuest::GetPrimaryData() const
{
	return m_iData1;
}

/// Quest-specific data 2
int CvMinorCivQuest::GetSecondaryData() const
{
	return m_iData2;
}

/// Quest-specific data 3
int CvMinorCivQuest::GetTertiaryData() const
{
	return m_iData3;
}

/// Rewards for quest completion
int CvMinorCivQuest::GetInfluence() const
{
	return m_iInfluence;
}
int CvMinorCivQuest::GetDisabledInfluence() const
{
	return m_iDisabledInfluence;
}
int CvMinorCivQuest::GetGold() const
{
	return m_iGold;
}
int CvMinorCivQuest::GetScience() const
{
	return m_iScience;
}
int CvMinorCivQuest::GetCulture() const
{
	return m_iCulture;
}
int CvMinorCivQuest::GetFaith() const
{
	return m_iFaith;
}
int CvMinorCivQuest::GetGoldenAgePoints() const
{
	return m_iGoldenAgePoints;
}
int CvMinorCivQuest::GetFood() const
{
	return m_iFood;
}
int CvMinorCivQuest::GetProduction() const
{
	return m_iProduction;
}
int CvMinorCivQuest::GetTourism() const
{
	return m_iTourism;
}
int CvMinorCivQuest::GetHappiness() const
{
	return m_iHappiness;
}
int CvMinorCivQuest::GetGP() const
{
	return m_iGP;
}
int CvMinorCivQuest::GetGPGlobal() const
{
	return m_iGPGlobal;
}
int CvMinorCivQuest::GetGeneralPoints() const
{
	return m_iGeneralPoints;
}
int CvMinorCivQuest::GetAdmiralPoints() const
{
	return m_iAdmiralPoints;
}
int CvMinorCivQuest::GetExperience() const
{
	return m_iExperience;
}

void CvMinorCivQuest::SetInfluence(int iValue)
{
	m_iInfluence = iValue;
}
void CvMinorCivQuest::SetDisabledInfluence(int iValue)
{
	m_iDisabledInfluence = iValue;
}
void CvMinorCivQuest::SetGold(int iValue)
{
	m_iGold = iValue;
}
void CvMinorCivQuest::SetScience(int iValue)
{
	m_iScience = iValue;
}
void CvMinorCivQuest::SetCulture(int iValue)
{
	m_iCulture = iValue;
}
void CvMinorCivQuest::SetFaith(int iValue)
{
	m_iFaith = iValue;
}
void CvMinorCivQuest::SetGoldenAgePoints(int iValue)
{
	m_iGoldenAgePoints = iValue;
}
void CvMinorCivQuest::SetFood(int iValue)
{
	m_iFood = iValue;
}
void CvMinorCivQuest::SetProduction(int iValue)
{
	m_iProduction = iValue;
}
void CvMinorCivQuest::SetTourism(int iValue)
{
	m_iTourism = iValue;
}
void CvMinorCivQuest::SetHappiness(int iValue)
{
	m_iHappiness = iValue;
}
void CvMinorCivQuest::SetGP(int iValue)
{
	m_iGP = iValue;
}
void CvMinorCivQuest::SetGPGlobal(int iValue)
{
	m_iGPGlobal = iValue;
}
void CvMinorCivQuest::SetGeneralPoints(int iValue)
{
	m_iGeneralPoints = iValue;
}
void CvMinorCivQuest::SetAdmiralPoints(int iValue)
{
	m_iAdmiralPoints = iValue;
}
void CvMinorCivQuest::SetExperience(int iValue)
{
	m_iExperience = iValue;
}

// Was this quest partially completed?
bool CvMinorCivQuest::IsPartialQuest() const
{
	return m_bPartialQuest;
}
void CvMinorCivQuest::SetPartialQuest(bool bValue)
{
	m_bPartialQuest = bValue;
}

/// Calculate (or recalculate) the rewards this player will receive for completing this City-State quest
void CvMinorCivQuest::CalculateRewards(PlayerTypes ePlayer, bool bRecalc)
{
	if (ePlayer == NO_PLAYER || m_eMinor == NO_PLAYER || !GET_PLAYER(ePlayer).isMajorCiv())
		return;

	CvSmallAwardInfo* pkSmallAwardInfo = GC.getSmallAwardInfo((SmallAwardTypes)m_eType);
	if (!pkSmallAwardInfo)
		return;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (kPlayer.getCapitalCity() == NULL)
		return;

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	MinorCivPersonalityTypes ePersonality = pMinor->GetMinorCivAI()->GetPersonality();
	MinorCivTraitTypes eTrait = pMinor->GetMinorCivAI()->GetTrait();


	// Era scaling (VP only)
	int iEraScaler = 100;
	int iEra = kPlayer.GetCurrentEra();
	if (MOD_BALANCE_CORE_MINORS && iEra > 1)
	{
		iEraScaler += (100 * (iEra-1)) / 2;
	}

	// Random contribution (VP only)
	int iRandomContribution = 0;
	if (MOD_BALANCE_CORE_MINORS)
	{
		if (ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL)
		{
			iRandomContribution += GC.getGame().getSmallFakeRandNum(pkSmallAwardInfo->GetRandom(), kPlayer.GetPseudoRandomSeed() + m_eType) * 2;
			iRandomContribution -= GC.getGame().getSmallFakeRandNum(pkSmallAwardInfo->GetRandom(), pMinor->GetPseudoRandomSeed() + m_eType) * 2;
		}
		else
		{
			iRandomContribution += GC.getGame().getSmallFakeRandNum(pkSmallAwardInfo->GetRandom(), kPlayer.GetPseudoRandomSeed()+m_eType ) * 2;
		}
	}

	// Now determine the rewards!
	int iBaseModifier = pMinor->GetMinorCivAI()->GetQuestRewardModifier(ePlayer);

	// Influence (no gamespeed scaling!)
	if (pkSmallAwardInfo->GetInfluence() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetInfluence() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		if (bRecalc)
		{
			if (pMinor->GetMinorCivAI()->IsQuestInfluenceDisabled(ePlayer))
			{
				if (iBonus > GetDisabledInfluence())
					SetDisabledInfluence(iBonus);
			}
			else if (iBonus > GetInfluence())
			{
				SetInfluence(iBonus);
			}
		}
		else if (pMinor->GetMinorCivAI()->IsQuestInfluenceDisabled(ePlayer))
		{
			SetDisabledInfluence(iBonus);
		}
		else
			SetInfluence(iBonus);
	}

	// Gold
	if (pkSmallAwardInfo->GetGold() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetGold() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GOLD_MERCANTILE);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GOLD_MARITIME);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetGold())
			{
				SetGold(iBonus);
			}
		}
		else
			SetGold(iBonus);
	}

	// Science
	if (pkSmallAwardInfo->GetScience() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetScience() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iBonus *= /*100 in CP, 60 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_SCIENCE_RELIGIOUS);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetScience())
			{
				SetScience(iBonus);
			}
		}
		else
			SetScience(iBonus);
	}

	// Culture
	if (pkSmallAwardInfo->GetCulture() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetCulture() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_CULTURE_CULTURED);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL) //boring
		{
			iBonus *= /*100 in CP, 80 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_CULTURE_NEUTRAL);
			iBonus /= 100;
		}
		else if (ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL) //exciting
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_CULTURE_IRRATIONAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetCulture())
			{
				SetCulture(iBonus);
			}
		}
		else
			SetCulture(iBonus);
	}

	// Faith
	if (pkSmallAwardInfo->GetFaith() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetFaith() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_FAITH_RELIGIOUS);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)
		{
			iBonus *= /*100 in CP, 60 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_FAITH_NEUTRAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetFaith())
			{
				SetFaith(iBonus);
			}
		}
		else
			SetFaith(iBonus);
	}

	// Golden Age Points
	if (pkSmallAwardInfo->GetGAP() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetGAP() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GAP_CULTURED);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GAP_RELIGIOUS);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL)		
		{
			iBonus *= /*100 in CP, 60 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GAP_IRRATIONAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetGoldenAgePoints())
			{
				SetGoldenAgePoints(iBonus);
			}
		}
		else
			SetGoldenAgePoints(iBonus);
	}

	// Food (in Capital)
	if (pkSmallAwardInfo->GetFood() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetFood() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_FOOD_MARITIME);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iBonus *= /*100 in CP, 50 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_FOOD_MERCANTILE);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)
		{
			iBonus *= /*100 in CP, 80 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_FOOD_NEUTRAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetFood())
			{
				SetFood(iBonus);
			}
		}
		else
			SetFood(iBonus);
	}

	// Production (in Capital)
	if (pkSmallAwardInfo->GetProduction() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetProduction() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_PRODUCTION_MILITARISTIC);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetProduction())
			{
				SetProduction(iBonus);
			}
		}
		else
			SetProduction(iBonus);
	}

	// Tourism
	if (pkSmallAwardInfo->GetTourism() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetTourism() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iBonus *= /*100 in CP, 50 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_TOURISM_MILITARISTIC);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_TOURISM_CULTURED);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_TOURISM_NEUTRAL);
			iBonus /= 100;
		}
		else if (ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL)
		{
			iBonus *= /*100 in CP, 75 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_TOURISM_IRRATIONAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetTourism())
			{
				SetTourism(iBonus);
			}
		}
		else
			SetTourism(iBonus);	
	}

	// Happiness (no gamespeed scaling!)
	if (pkSmallAwardInfo->GetHappiness() > 0)
	{
		int iBonus = pkSmallAwardInfo->GetHappiness();
		if (MOD_BALANCE_CORE_MINORS)
			iBonus += iEra / 2;

		if (bRecalc)
		{
			if (iBonus > GetHappiness())
			{
				SetHappiness(iBonus);
			}
		}
		else
			SetHappiness(iBonus);
	}

	// Great Person points (in Capital; no gamespeed scaling!)
	if (pkSmallAwardInfo->GetGPPoints() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetGPPoints() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 75 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GPP_CULTURED);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)		
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GPP_NEUTRAL);
			iBonus /= 100;
		}
		else if (ePersonality == MINOR_CIV_PERSONALITY_IRRATIONAL)	
		{
			iBonus *= /*100 in CP, 75 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GPP_IRRATIONAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetGP())
			{
				SetGP(iBonus);
			}
		}
		else
			SetGP(iBonus);
	}

	// Great Person points (in all Cities; no gamespeed scaling!)
	if (pkSmallAwardInfo->GetGPPointsGlobal() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetGPPointsGlobal() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_RELIGIOUS);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 75 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_CULTURED);
			iBonus /= 100;
		}

		if (ePersonality == MINOR_CIV_PERSONALITY_NEUTRAL)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GPP_GLOBAL_NEUTRAL);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetGPGlobal())
			{
				SetGPGlobal(iBonus);
			}
		}
		else
			SetGPGlobal(iBonus);
	}

	// Great General points
	if (pkSmallAwardInfo->GetGeneralPoints() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetGeneralPoints() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_MILITARISTIC);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 75 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_GENERAL_POINTS_CULTURED);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetGeneralPoints())
			{
				SetGeneralPoints(iBonus);
			}
		}
		else
			SetGeneralPoints(iBonus);
	}

	// Great Admiral points
	if (pkSmallAwardInfo->GetAdmiralPoints() > 0)
	{
		int iBaseBonus = (pkSmallAwardInfo->GetAdmiralPoints() * iEraScaler)/100 + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		iBonus *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iBonus /= 100;

		if (eTrait == MINOR_CIV_TRAIT_MARITIME)
		{
			iBonus *= /*100 in CP, 150 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_MARITIME);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_CULTURED)
		{
			iBonus *= /*100 in CP, 75 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_ADMIRAL_POINTS_CULTURED);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetAdmiralPoints())
			{
				SetAdmiralPoints(iBonus);
			}
		}
		else
			SetAdmiralPoints(iBonus);
	}

	// XP to all units (no gamespeed scaling!)
	if (pkSmallAwardInfo->GetExperience() > 0)
	{
		int iBaseBonus = pkSmallAwardInfo->GetExperience() + iRandomContribution;
		int iBonus = (iBaseBonus * iBaseModifier)/100;

		if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iBonus *= /*100 in CP, 125 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_EXPERIENCE_MILITARISTIC);
			iBonus /= 100;
		}
		else if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iBonus *= /*100 in CP, 50 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_EXPERIENCE_RELIGIOUS);
			iBonus /= 100;
		}

		if (bRecalc)
		{
			if (iBonus > GetExperience())
			{
				SetExperience(iBonus);
			}
		}
		else
			SetExperience(iBonus);
	}

	// Log the quest rewards
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;
		CvString strTemp;
		CvString playerName = pMinor->getCivilizationShortDescription();

		// Open the log file
		if (GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		CvString otherPlayerName = pMinor->getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;
		CvString strOutBuf = strBaseString;

		// Quest Info
		strTemp = "SENT OUT QUEST - ";
		strOutBuf += ", " + strTemp;
		strOutBuf += GetRewardString(ePlayer, false);

		pLog->Msg(strOutBuf);
	}
}

/// This quest was completed, distribute rewards to the player who completed it
void CvMinorCivQuest::DoRewards(PlayerTypes ePlayer, bool bHeavyTribute)
{
	if (ePlayer == NO_PLAYER || !GET_PLAYER(ePlayer).isMajorCiv())
		return;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (kPlayer.getCapitalCity() == NULL)
		return;

	if (IsPartialQuest() || bHeavyTribute)
	{
		SetInfluence(GetInfluence() / 2);
		SetGold(GetGold() / 2);
		SetScience(GetScience() / 2);
		SetCulture(GetCulture() / 2);
		SetFaith(GetFaith() / 2);
		SetGoldenAgePoints(GetGoldenAgePoints() / 2);
		SetFood(GetFood() / 2);
		SetProduction(GetProduction() / 2);
		SetTourism(GetTourism() / 2);
		SetHappiness(GetHappiness() / 2);
		SetGP(GetGP() / 2);
		SetGPGlobal(GetGPGlobal() / 2);
		SetGeneralPoints(GetGeneralPoints() / 2);
		SetAdmiralPoints(GetAdmiralPoints() / 2);
		SetExperience(GetExperience() / 2);
	}

	if (GetInfluence() > 0 && !bHeavyTribute)
	{
		GET_PLAYER(m_eMinor).GetMinorCivAI()->ChangeFriendshipWithMajor(m_eAssignedPlayer, GetInfluence(), /*bFromQuest*/ true);

		if (MOD_BALANCE_CORE_JFD)
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetInfluence(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_JFD_LOYALTY, NULL, NO_TERRAIN, this);
	}
	if (GetGold() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetGold(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_GOLD, NULL, NO_TERRAIN, this);
	}
	if (GetScience() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetScience(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_SCIENCE, NULL, NO_TERRAIN, this);
	}
	if (GetCulture() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetCulture(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_CULTURE, NULL, NO_TERRAIN, this);
	}
	if (GetFaith() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetFaith(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_FAITH, NULL, NO_TERRAIN, this);
	}
	if (GetGoldenAgePoints() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetGoldenAgePoints(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_GOLDEN_AGE_POINTS, NULL, NO_TERRAIN, this);
	}
	if (GetFood() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetFood(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_FOOD, NULL, NO_TERRAIN, this);
	}
	if (GetProduction() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetProduction(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_PRODUCTION, NULL, NO_TERRAIN, this);
	}
	if (GetTourism() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetTourism(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_TOURISM, NULL, NO_TERRAIN, this);
	}
	if (GetHappiness() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetHappiness(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_CULTURE_LOCAL, NULL, NO_TERRAIN, this);
	}
	if (GetGP() > 0)
	{
		for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
		{
			const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
			CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
			if (pkSpecialistInfo)
			{
				// Does this Specialist spawn a GP?
				if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
				{
					kPlayer.getCapitalCity()->GetCityCitizens()->ChangeSpecialistGreatPersonProgressTimes100(eSpecialist, GetGP() * 100, true);
				}
			}
		}

		if (MOD_BALANCE_CORE_JFD)
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetGP(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_JFD_HEALTH, NULL, NO_TERRAIN, this);
	}
	if (GetGPGlobal() > 0)
	{
		// Loop through all Cities
		int iLoop = 0;
		for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
				CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
				if (pkSpecialistInfo)
				{
					// Does this Specialist spawn a GP?
					if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
					{
						pLoopCity->GetCityCitizens()->ChangeSpecialistGreatPersonProgressTimes100(eSpecialist, GetGPGlobal() * 100, true);
					}
				}
			}
		}

		if (MOD_BALANCE_CORE_JFD)
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetGPGlobal(), false, NO_PLAYER, NULL, false, NULL, false, true, bHeavyTribute, YIELD_JFD_HEALTH, NULL, NO_TERRAIN, this);
	}
	if (GetGeneralPoints() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetGeneralPoints(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_GREAT_GENERAL_POINTS, NULL, NO_TERRAIN, this);
	}
	if (GetAdmiralPoints() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetAdmiralPoints(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_GREAT_ADMIRAL_POINTS, NULL, NO_TERRAIN, this);
	}
	if (GetExperience() > 0)
	{
		int iLoop = 0;
		for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
		{
			if (pLoopUnit && !pLoopUnit->IsCivilianUnit())
			{
				pLoopUnit->changeExperienceTimes100(GetExperience() * 100);
			}
		}

		if (MOD_BALANCE_CORE_JFD)
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_MINOR_QUEST_REWARD, false, NO_GREATPERSON, NO_BUILDING, GetExperience(), false, NO_PLAYER, NULL, false, kPlayer.getCapitalCity(), false, true, bHeavyTribute, YIELD_JFD_SOVEREIGNTY, NULL, NO_TERRAIN, this);
	}
}

void CvMinorCivAI::RecalculateRewards(PlayerTypes ePlayer)
{
	if (!GetPlayer()->isAlive())
		return;

	if (m_QuestsGiven.empty() || m_QuestsGiven.size() <= (size_t)ePlayer)
		return;

	for (QuestListForPlayer::iterator itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
		itr_quest->CalculateRewards(ePlayer, true);
}

void CvMinorCivQuest::DisableInfluence(PlayerTypes ePlayer)
{
	if (ePlayer == NO_PLAYER || m_eMinor == NO_PLAYER || !GET_PLAYER(ePlayer).isMajorCiv())
		return;

	SetDisabledInfluence(GetInfluence());
	SetInfluence(0);
}

void CvMinorCivQuest::EnableInfluence(PlayerTypes ePlayer)
{
	if (ePlayer == NO_PLAYER || m_eMinor == NO_PLAYER || !GET_PLAYER(ePlayer).isMajorCiv())
		return;

	SetInfluence(GetDisabledInfluence());
	SetDisabledInfluence(0);
}

CvString CvMinorCivQuest::GetRewardString(PlayerTypes ePlayer, bool bFinish) const
{
	CvString szTooltip = "";
	CvString szTooltipHeader = "";
	if (ePlayer == NO_PLAYER || !GET_PLAYER(ePlayer).isMajorCiv())
		return szTooltip;

	szTooltipHeader = bFinish ? GetLocalizedText("TXT_KEY_CS_QUEST_BONUS_HEADER_FINISH") : GetLocalizedText("TXT_KEY_CS_QUEST_BONUS_HEADER_START");

	if (GetInfluence() > 0)
	{
		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_INFLUENCE", GetInfluence());
	}
	if (GetGold() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_GOLD", GetGold());
	}
	if (GetScience() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_SCIENCE", GetScience());
	}
	if (GetCulture() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_CULTURE", GetCulture());
	}
	if (GetFaith() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_FAITH", GetFaith());
	}
	if (GetGoldenAgePoints() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_GAP", GetGoldenAgePoints());
	}
	if (GetFood() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_FOOD", GetFood());
	}
	if (GetProduction() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_PRODUCTION", GetProduction());
	}
	if (GetTourism() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_TOURISM", GetTourism());
	}
	if (GetHappiness() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_HAPPINESS", GetHappiness());
	}
	if (GetGP() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_GPP", GetGP());
	}
	if (GetGPGlobal() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_GPP_GLOBAL", GetGPGlobal());
	}
	if (GetGeneralPoints() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_GENERAL", GetGeneralPoints());
	}
	if (GetAdmiralPoints() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_ADMIRAL", GetAdmiralPoints());
	}
	if (GetExperience() > 0)
	{
		if (szTooltip != "")
			szTooltip += "[NEWLINE]";

		szTooltip += GetLocalizedText("TXT_KEY_CS_QUEST_EXPERIENCE", GetExperience());
	}

	if (szTooltip != "")
		szTooltip = szTooltipHeader + szTooltip;

	return szTooltip;
}

// CONTEST FUNCTIONS

// Assumes that only one contest of a particular Type is active from this minor at any given time.
// Otherwise, the results across different spans of contest will be calculated together here.
int CvMinorCivQuest::GetContestValueForPlayer(PlayerTypes ePlayer) const
{
	int iValue = -1;

	CvPlayer* pMinor = &GET_PLAYER(GetMinor());
	if (!pMinor)
		return iValue;

	// Player must actually be in the contest to have a score!
	MinorCivQuestTypes eType = GetType();
	if (!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType))
		return iValue;

	switch (eType)
	{
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	{
		int iStartCulture = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndCulture = GET_PLAYER(ePlayer).GetJONSCultureEverGenerated();
		iValue = iEndCulture - iStartCulture;
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		if (MOD_BALANCE_VP)
		{
			ReligionTypes eReligion = (ReligionTypes) pMinor->GetMinorCivAI()->GetQuestData2(ePlayer, eType);
			if (eReligion == GET_PLAYER(ePlayer).GetReligions()->GetStateReligion(false))
			{
				int iStartFollowers = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
				int iEndFollowers = GC.getGame().GetGameReligions()->GetNumFollowers(eReligion);
				iValue = iEndFollowers - iStartFollowers;
			}
			else
				return iValue;
		}
		else
		{
			int iStartFaith = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
			int iEndFaith = GET_PLAYER(ePlayer).GetFaithEverGenerated();
			iValue = iEndFaith - iStartFaith;
		}
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	{
		int iStartTechs = pMinor->GetMinorCivAI()->GetQuestData1(ePlayer, eType);
		int iEndTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		iValue = iEndTechs - iStartTechs;
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		iValue = GET_PLAYER(ePlayer).GetCulture()->GetTourism() / 100;
		break;
	}
	default:
		break; // Not a contest quest.
	}

	return iValue;
}

int CvMinorCivQuest::GetContestValueForLeader()
{
	int iHighestValue = -1;
	MinorCivQuestTypes eType = GetType();

	if (eType != MINOR_CIV_QUEST_CONTEST_CULTURE && eType != MINOR_CIV_QUEST_CONTEST_FAITH && eType != MINOR_CIV_QUEST_CONTEST_TOURISM && eType != MINOR_CIV_QUEST_CONTEST_TECHS)
		return iHighestValue;

	// What is the largest value a participant has for this contest?
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		int iPlayerValue = GetContestValueForPlayer(ePlayer);
		if (iPlayerValue > iHighestValue)
			iHighestValue = iPlayerValue;
	}

	return iHighestValue;
}

// Returns a list, since ties are allowed
CivsList CvMinorCivQuest::GetContestLeaders()
{
	MinorCivQuestTypes eType = GetType();
	CivsList veTiedForLead; // Recursive to-do: remove nonstandard vector

	if (eType != MINOR_CIV_QUEST_CONTEST_CULTURE && eType != MINOR_CIV_QUEST_CONTEST_FAITH && eType != MINOR_CIV_QUEST_CONTEST_TOURISM && eType != MINOR_CIV_QUEST_CONTEST_TECHS)
		return veTiedForLead;

	int iHighestValue = GetContestValueForLeader();

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
		int iPlayerValue = GetContestValueForPlayer(ePlayer);
		if (iPlayerValue == iHighestValue)
			veTiedForLead.push_back(ePlayer);
	}

	if (veTiedForLead.empty())
		veTiedForLead.push_back(NO_PLAYER);

	return veTiedForLead;
}

// Is this player currently the contest leader?  (if no ePlayer is passed in, checks the player this instance of the quest is assigned to)
bool CvMinorCivQuest::IsContestLeader(PlayerTypes ePlayer)
{
	CvPlayer* pMinor = &GET_PLAYER(GetMinor());
	if (!pMinor)
		return false;

	if (ePlayer == NO_PLAYER)
		ePlayer = GetPlayerAssignedTo();

	if (ePlayer == NO_PLAYER)
		return false;

	MinorCivQuestTypes eType = GetType();
	if (eType != MINOR_CIV_QUEST_CONTEST_CULTURE && eType != MINOR_CIV_QUEST_CONTEST_FAITH && eType != MINOR_CIV_QUEST_CONTEST_TOURISM && eType != MINOR_CIV_QUEST_CONTEST_TECHS)
		return false;

	// Player must actually be in the contest!
	if (!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eType))
		return false;

	CivsList veTiedForLead = GetContestLeaders();
	for (uint ui = 0; ui < veTiedForLead.size(); ui++)
	{
		if (ePlayer == veTiedForLead[ui])
			return true;
	}

	return false;
}

// END CONTEST FUNCTIONS

// Checks if the conditions for completing the quest are met.  Does not actually complete the quest.
bool CvMinorCivQuest::IsComplete()
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);
	if (!pMinor || !pMinor->isMinorCiv() || (pMinor->getCapitalCity() && !pMinor->getCapitalCity()->plot()) || !pAssignedPlayer || !pAssignedPlayer->isMajorCiv() || (pAssignedPlayer->getCapitalCity() && !pAssignedPlayer->getCapitalCity()->plot()))
		UNREACHABLE();

	// Has either player died?
	if (!pMinor->isAlive() || !pMinor->getCapitalCity() || !pAssignedPlayer->isAlive() || !pAssignedPlayer->getCapitalCity())
		return false;

	// Can't complete quests while at war!
	if (pAssignedPlayer->IsAtWarWith(m_eMinor))
		return false;

	switch (m_eType)
	{
	case NO_MINOR_CIV_QUEST_TYPE:
		UNREACHABLE();
	case MINOR_CIV_QUEST_ROUTE:
	{
		// Road connection established?
		return pAssignedPlayer->IsCapitalConnectedToPlayer(m_eMinor);
	}
	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);

		// Camp removed & cleared by this player?
		return pPlot->getImprovementType() != GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT) && pPlot->GetPlayerThatClearedBarbCampHere() == m_eAssignedPlayer;
	}
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
	{
		ResourceTypes eResource = (ResourceTypes)m_iData1;

		// Player now has the Resource?
		return pAssignedPlayer->getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
	{
		BuildingTypes eWonder = (BuildingTypes)m_iData1;

		// Player built the Wonder?
		return pAssignedPlayer->countNumBuildings(eWonder) > 0;
	}
	case MINOR_CIV_QUEST_GREAT_PERSON:
	{
		UnitTypes eUnit = (UnitTypes)m_iData1;

		// Player has the Great Person?
		int iLoop = 0;
		for (CvUnit* pLoopUnit = pAssignedPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pAssignedPlayer->nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitType() == eUnit)
				return true;
		}
		return false;
	}
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		PlayerTypes eTargetCityState = (PlayerTypes)m_iData1;
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);
		CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());

		// Player killed the City-State?
		if (pTargetCityStateTeam->GetKilledByTeam() == pAssignedPlayer->getTeam())
			return true;

		// Allied to both? Partial reward.
		if (MOD_BALANCE_VP && pTargetCityState->GetMinorCivAI()->IsAllies(m_eAssignedPlayer) && pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer))
		{
			SetPartialQuest(true);
			return true;
		}

		return false;
	}
	case MINOR_CIV_QUEST_FIND_PLAYER:
	{
		PlayerTypes ePlayerToFind = (PlayerTypes)m_iData1;

		// Player found the target player?
		return GET_TEAM(pAssignedPlayer->getTeam()).IsHasFoundPlayersTerritory(ePlayerToFind);
	}
	case MINOR_CIV_QUEST_FIND_CITY:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		return pPlot->isCity() && pPlot->isRevealed(pAssignedPlayer->getTeam());
	}
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
	{
		int iNumWondersFoundBefore = m_iData1;

		// Player found a new NW?
		return GET_TEAM(pAssignedPlayer->getTeam()).GetNumNaturalWondersDiscovered() > iNumWondersFoundBefore;
	}
	case MINOR_CIV_QUEST_GIVE_GOLD:
	{
		int iGoldGiftedBefore = m_iData2;

		// Player gave gold since the quest began?
		return pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer) > iGoldGiftedBefore;
	}
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
	{
		// Player pledged to protect us?
		return pMinor->GetMinorCivAI()->IsProtectedByMajor(m_eAssignedPlayer);
	}
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		// Player won the contest?
		return GetEndTurn() <= GC.getGame().getGameTurn() && IsContestLeader(m_eAssignedPlayer);
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		// VP: Player must have the same religion as when the quest was assigned
		if (MOD_BALANCE_VP && pAssignedPlayer->GetReligions()->GetStateReligion(false) != (ReligionTypes)m_iData2)
			return false;

		// Player won the contest?
		return GetEndTurn() <= GC.getGame().getGameTurn() && IsContestLeader(m_eAssignedPlayer);
	}
	case MINOR_CIV_QUEST_INVEST:
	case MINOR_CIV_QUEST_INFLUENCE:
	{
		// This quest is automatically "completed" when it ends
		return GetEndTurn() <= GC.getGame().getGameTurn();
	}
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	{
		PlayerTypes eTargetMinor = (PlayerTypes)m_iData1;
		int iPreviousBullyTurnWhenQuestBegan = m_iData2;

		// Bullied the target since the quest began?
		return GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(m_eAssignedPlayer) > iPreviousBullyTurnWhenQuestBegan;
	}
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
	{
		PlayerTypes eTargetMajor = (PlayerTypes)m_iData1;

		// Denounced the target?
		return pAssignedPlayer->GetDiplomacyAI()->IsDenouncedPlayer(eTargetMajor);
	}
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
	{
		ReligionTypes eReligion = (ReligionTypes)m_iData1;

		// Does the CS's capital have the right majority religion?
		return pMinor->getCapitalCity()->GetCityReligions()->GetReligiousMajority() == eReligion;
	}
	case MINOR_CIV_QUEST_TRADE_ROUTE:
	{
		// Trade route active?
		return GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_eMinor, m_eAssignedPlayer);
	}
	case MINOR_CIV_QUEST_WAR:
	{
		PlayerTypes eTargetMajor = (PlayerTypes)m_iData1;

		// At war with the target?
		return pAssignedPlayer->IsAtWarWith(eTargetMajor);
	}
	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
	{
		// Sent us our requested unit?
		return pMinor->GetMinorCivAI()->GetHasSentUnitForQuest(m_eAssignedPlayer);
	}
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
	{
		PlayerTypes eTargetMinor = (PlayerTypes)m_iData1;

		// Met the target City-State?
		return GET_TEAM(pAssignedPlayer->getTeam()).isHasMet(GET_PLAYER(eTargetMinor).getTeam());
	}
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);

		// Antiquity site removed & cleared by this player?
		return pPlot->getResourceType() != GD_INT_GET(ARTIFACT_RESOURCE) && pPlot->GetPlayerThatClearedDigHere() == m_eAssignedPlayer;
	}
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		// Player team circumnavigated the world?
		return GC.getGame().GetTeamThatCircumnavigated() == GET_PLAYER(m_eAssignedPlayer).getTeam();
	}
	case MINOR_CIV_QUEST_LIBERATION:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		PlayerTypes eTargetPlayer = (PlayerTypes)m_iData3;

		// Did this player liberate the City-State?
		return pPlot->isCity() && pPlot->getOwner() == eTargetPlayer && pPlot->getPlotCity()->isEverLiberated(m_eAssignedPlayer);
	}
	case MINOR_CIV_QUEST_HORDE:
	{
		// Are all the Barbarians dead?
		return GetEndTurn() <= GC.getGame().getGameTurn() && pMinor->GetMinorCivAI()->GetNumBarbariansInBorders(true) == 0;
	}
	case MINOR_CIV_QUEST_REBELLION:
	{
		// Are all the Barbarians eliminated?
		if (GetEndTurn() <= GC.getGame().getGameTurn() && pMinor->GetMinorCivAI()->GetNumBarbariansInBorders(false) == 0)
			return true;

		// Has the City-State's ally changed?
		PlayerTypes eOriginalAlly = (PlayerTypes)m_iData1;
		if (pMinor->GetMinorCivAI()->GetAlly() != eOriginalAlly)
			return true;

		return false;
	}
	case MINOR_CIV_QUEST_EXPLORE_AREA:
	{
		// Explored the area?
		return pMinor->GetMinorCivAI()->GetExplorePercent(m_eAssignedPlayer, MINOR_CIV_QUEST_EXPLORE_AREA) >= 100;
	}
	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
	{
		BuildingTypes eBuilding = (BuildingTypes)m_iData1;
		int iNumToConstruct = m_iData2;

		int iNumConstructed = 0;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(m_eAssignedPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eAssignedPlayer).nextCity(&iCityLoop))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				iNumConstructed++;
		}

		// Built X of this building?
		return iNumConstructed >= iNumToConstruct;
	}
	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
	{
		PlayerTypes eTargetMajor = (PlayerTypes)m_iData1;
		int iNumThefts = m_iData2;

		// Stole from the target X times?
		return pAssignedPlayer->GetEspionage()->GetNumSpyActionsDone(eTargetMajor) >= iNumThefts;
	}
	case MINOR_CIV_QUEST_COUP:
	{
		PlayerTypes eTargetMinor = (PlayerTypes)m_iData1;

		// Successful coup against the target City-State?
		return GET_PLAYER(eTargetMinor).GetMinorCivAI()->IsCoupAttempted(m_eAssignedPlayer) && GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetAlly() == m_eAssignedPlayer;
	}
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
	{
		int iX = m_iData1, iY = m_iData2;
		CvPlot* pPlot = GC.getMap().plot(iX, iY);

		// Conquered or destroyed this city? NOTE: If the player liberated the city, it should still have the "previous owner" flag set
		return (pPlot->isCity() && (pPlot->getPlotCity()->getOwner() == m_eAssignedPlayer || pPlot->getPlotCity()->getPreviousOwner() == m_eAssignedPlayer)) || 
			(!pPlot->isCity() && pPlot->GetPlayerThatDestroyedCityHere() == m_eAssignedPlayer);
	}
	}

	return false;
}

/// Is this quest now revoked (because the player bullied or attacked us)?
bool CvMinorCivQuest::IsRevoked(bool bWar)
{
	// Horde/Rebellion are never revoked
	if (m_eType == MINOR_CIV_QUEST_HORDE || m_eType == MINOR_CIV_QUEST_REBELLION)
		return false;

	// All other quests are revoked by war
	if (bWar)
		return true;

	// Global quests are not revoked by bullying
	if (GET_PLAYER(m_eMinor).GetMinorCivAI()->IsGlobalQuest(m_eType))
		return false;

	// Bullied us recently? No personal quests for you!
	if (GET_PLAYER(m_eMinor).GetMinorCivAI()->IsRecentlyBulliedByMajor(m_eAssignedPlayer))
		return true;

	return false;
}

/// Is this quest now expired (i.e. time limit is up or condition is no longer valid)?
bool CvMinorCivQuest::IsExpired()
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);
	if (!pMinor || !pMinor->isMinorCiv() || (pMinor->getCapitalCity() && !pMinor->getCapitalCity()->plot()) || !pAssignedPlayer || !pAssignedPlayer->isMajorCiv() || (pAssignedPlayer->getCapitalCity() && !pAssignedPlayer->getCapitalCity()->plot()))
		UNREACHABLE();

	// Has either player died?
	if (!pMinor->isAlive() || !pMinor->getCapitalCity() || !pAssignedPlayer->isAlive() || !pAssignedPlayer->getCapitalCity())
		return true;

	// If this quest type has an end turn, have we passed it?
	if (GetEndTurn() != NO_TURN && GC.getGame().getGameTurn() > GetEndTurn())
		return true;

	CvTeam* pAssignedTeam = &GET_TEAM(pAssignedPlayer->getTeam());

	switch (m_eType)
	{
	case MINOR_CIV_QUEST_ROUTE:
	{
		// City-State's capital cannot be coastal
		if (pMinor->getCapitalCity()->isCoastal())
			return true;

		// Must have a city close to the City-State on the same Landmass
		const int iMaxRouteDistance = /*7 in CP, 11 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_MAXIMUM_DISTANCE);
		CvPlot* pMinorPlot = pMinor->getCapitalCity()->plot();

		int iLoop = 0;
		bool bInRange = false;
		for (CvCity* pLoopCity = GET_PLAYER(m_eAssignedPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eAssignedPlayer).nextCity(&iLoop))
		{
			if (pLoopCity->plot()->getLandmass() != pMinorPlot->getLandmass())
				continue;

			int iDistance = plotDistance(pMinorPlot->getX(), pMinorPlot->getY(), pLoopCity->getX(), pLoopCity->getY());
			if (iDistance <= iMaxRouteDistance)
			{
				bInRange = true;
				break;
			}
		}

		if (!bInRange)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		// Barbarian camp was cleared, and not by us
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		if (!pPlot->HasBarbarianCamp() && pPlot->GetPlayerThatClearedBarbCampHere() != m_eAssignedPlayer)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
	{
		ResourceTypes eResource = (ResourceTypes)m_iData1;
		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);

		// Resource must exist on the map
		if (GC.getMap().getNumResources(eResource) <= 0)
			return true;

		// No banned resources
		if (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague && pLeague->IsLuxuryHappinessBanned(eResource))
				return true;
		}

		// The player's capital city's landmass must contain this resource, or the player must be able to cross the ocean
		if (!GET_PLAYER(m_eAssignedPlayer).CanCrossOcean())
		{
			CvLandmass* pLandmass = GC.getMap().getLandmassById(pAssignedPlayer->getCapitalCity()->plot()->getLandmass());
			if (pLandmass->getNumResources(eResource) == 0)
				return true;
		}

		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

			// Someone built the Wonder, and it wasn't us
			if (m_eAssignedPlayer != eLoopPlayer && GET_PLAYER(eLoopPlayer).countNumBuildings(eWonder) > 0)
				return true;
		}

		// Check if the player is no longer able to construct the Wonder anywhere
		std::vector<int> allBuildingCount = GET_PLAYER(m_eAssignedPlayer).GetTotalBuildingCount();
		bool bNoValidCity = true;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(m_eAssignedPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eAssignedPlayer).nextCity(&iCityLoop))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eWonder) == 0 && !pLoopCity->isBuildingInQueue(eWonder) && !pLoopCity->canConstruct(eWonder,allBuildingCount))
				continue;

			bNoValidCity = false;
			break;
		}
		if (bNoValidCity)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_GREAT_PERSON:
	{
		// No longer able to obtain this Great Person?
		UnitTypes eUnit = (UnitTypes)m_iData1;
		if (!pAssignedPlayer->canTrainUnit(eUnit, false, false, true))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		CvPlayer* pTargetCityState = &GET_PLAYER(eTargetCityState);
		CvTeam* pTargetCityStateTeam = &GET_TEAM(pTargetCityState->getTeam());

		// Someone killed the City State, and it wasn't us
		if (!pTargetCityState->isAlive() && pTargetCityStateTeam->GetKilledByTeam() != GET_PLAYER(m_eAssignedPlayer).getTeam())
			return true;

		if (MOD_BALANCE_VP)
		{
			// Someone else allied to both? We lose!
			if (pTargetCityState->GetMinorCivAI()->GetAlly() != NO_PLAYER && pTargetCityState->GetMinorCivAI()->GetAlly() != m_eAssignedPlayer &&
				pTargetCityState->GetMinorCivAI()->GetAlly() == GET_PLAYER(m_eMinor).GetMinorCivAI()->GetAlly())
			{
				return true;
			}

			if (pTargetCityState->isAlive() && GC.getGame().getGameTurn() >= GetEndTurn() && !IsComplete())
			{
				//Remove the quest from the other player.
				if (pTargetCityState->GetMinorCivAI()->IsActiveQuestForPlayer(m_eAssignedPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE))
					pTargetCityState->GetMinorCivAI()->DeleteQuest(m_eAssignedPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE);

				return true;
			}
		}

		break;
	}
	case MINOR_CIV_QUEST_FIND_PLAYER:
	{
		// Someone killed the player
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		if (!GET_PLAYER(eTargetPlayer).isAlive())
			return true;

		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY:
	{
		// No city here anymore?
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		if (!pPlot->isCity())
			return true;

		break;
	}
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
	{
		int iNumFoundBefore = GetPrimaryData();
		int iNumInWorld = GC.getMap().GetNumNaturalWonders();

		// Sanity check in case the number of Natural Wonders changed for some reason
		if (iNumFoundBefore >= iNumInWorld)
			return true;

		if (GET_TEAM(GET_PLAYER(m_eAssignedPlayer).getTeam()).GetNumNaturalWondersDiscovered() >= iNumInWorld)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		// Contest completed, and not the winner
		if (GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;

		break;
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		// Religion changed?
		if (MOD_BALANCE_VP && pAssignedPlayer->GetReligions()->GetStateReligion(false) != (ReligionTypes)m_iData2)
			return true;

		// Contest completed, and not the winner
		if (GC.getGame().getGameTurn() == GetEndTurn() && !IsComplete())
			return true;

		break;
	}
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	{
		// Someone killed the player
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		if (!GET_PLAYER(eTargetPlayer).isAlive())
			return true;

		if (GET_PLAYER(eTargetPlayer).GetMinorCivAI()->GetAlly() == m_eAssignedPlayer)
			return true;

		if (GET_PLAYER(eTargetPlayer).GetMinorCivAI()->IsProtectedByMajor(m_eAssignedPlayer))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
	{
		// Someone killed the player
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		if (!GET_PLAYER(eTargetPlayer).isAlive())
			return true;

		// Are both players now humans?
		if (pAssignedPlayer->isHuman() && GET_PLAYER(eTargetPlayer).isHuman())
			return true;

		// We're now Allies with the Major
		if (pMinor->GetMinorCivAI()->GetAlly() == eTargetPlayer)
			return true;

		// Is this now a bad target (backstabbing)?
		if (!pMinor->GetMinorCivAI()->IsAcceptableQuestEnemy(MINOR_CIV_QUEST_DENOUNCE_MAJOR, m_eAssignedPlayer, eTargetPlayer))
			return true;

		// Cannot denounce due to game options
		if (GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE) && GET_PLAYER(m_eAssignedPlayer).IsAtWarWith(eTargetPlayer))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
	{
		// Player's Owned Religion now differs from the original religion
		ReligionTypes eReligion = (ReligionTypes)m_iData1;
		if (pAssignedPlayer->GetReligions()->GetOwnedReligion() != eReligion)
			return true;

		// Player no longer has any cities following the religion
		if (GC.getGame().GetGameReligions()->GetNumDomesticCitiesFollowing(eReligion, m_eAssignedPlayer) == 0)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_TRADE_ROUTE:
	{
		// City-State Sanctions resolution in effect?
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague != NULL && pLeague->IsTradeEmbargoed(m_eAssignedPlayer, m_eMinor))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_WAR:
	{
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();

		// We're now Allies with the Major
		PlayerTypes eAlly = pMinor->GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eTargetPlayer).getTeam())
			return true;

		// Someone killed the Major
		if (!GET_PLAYER(eTargetPlayer).isAlive())
			return true;

		// We can't go to war with the Major
		if (!GET_PLAYER(m_eAssignedPlayer).IsAtWarWith(eTargetPlayer) && !GET_TEAM(pAssignedPlayer->getTeam()).canDeclareWar(GET_PLAYER(eTargetPlayer).getTeam(), m_eAssignedPlayer))
			return true;

		// Is this now a bad target (backstabbing)?
		if (!pMinor->GetMinorCivAI()->IsAcceptableQuestEnemy(MINOR_CIV_QUEST_DENOUNCE_MAJOR, m_eAssignedPlayer, eTargetPlayer))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();

		// Check if the player is no longer able to construct the Wonder anywhere
		std::vector<int> allBuildingCount = GET_PLAYER(m_eAssignedPlayer).GetTotalBuildingCount();
		bool bNoValidCity = true;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(m_eAssignedPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eAssignedPlayer).nextCity(&iCityLoop))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eWonder) == 0 && !pLoopCity->isBuildingInQueue(eWonder) && !pLoopCity->canConstruct(eWonder,allBuildingCount))
				continue;

			bNoValidCity = false;
			break;
		}
		if (bNoValidCity)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
	{
		UnitTypes eUnitType = (UnitTypes)GetPrimaryData();

		// Cancel ship requests if City-State's capital is no longer coastal
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
		if (pkUnitInfo->GetDomainType() == DOMAIN_SEA && !pMinor->getCapitalCity()->isCoastal(10))
			return true;

		// Can we train the unit type this unit upgrades to?
		bool bCanUpgrade = false;
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
			if (pkUnitClassInfo && pkUnitInfo->GetUpgradeUnitClass(iI))
			{
				UnitTypes eUpgradeUnit = GET_PLAYER(m_eAssignedPlayer).GetSpecificUnitType(eUnitClass);
				if (GET_PLAYER(m_eAssignedPlayer).canTrainUnit(eUpgradeUnit, false, false, false, false))
				{
					bCanUpgrade = true;
					break;
				}
			}
		}

		// Are any versions of the original unit still around?
		bool bAlreadyHasUnit = false;
		int iLoop = 0;
		for (CvUnit* pLoopUnit = GET_PLAYER(m_eAssignedPlayer).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(m_eAssignedPlayer).nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitType() == eUnitType)
			{
				bAlreadyHasUnit = true;
				break;
			}
		}

		if (!bAlreadyHasUnit)
		{
			// Can upgrade it and don't already have it? Invalidated!
			if (bCanUpgrade)
				return true;

			// Make sure at least one of the player's cities can train this unit
			bool bNoValidCity = true;
			int iCityLoop = 0;
			for (CvCity* pLoopCity = GET_PLAYER(m_eAssignedPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eAssignedPlayer).nextCity(&iCityLoop))
			{
				if (!pLoopCity->canTrain(eUnitType, false, false, false, false))
					continue;

				bNoValidCity = false;
				break;
			}
			if (bNoValidCity)
				return true;
		}

		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
	{
		// Someone killed the player, and we haven't met them
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		if (!GET_PLAYER(eTargetPlayer).isAlive() && !pAssignedTeam->isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		// Someone dug it, and it wasn't us
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		if (!pPlot->HasDig() && pPlot->GetPlayerThatClearedDigHere() != m_eAssignedPlayer)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		TeamTypes eCircumnavigator = GC.getGame().GetTeamThatCircumnavigated();
		if (eCircumnavigator != NO_TEAM && eCircumnavigator != pAssignedPlayer->getTeam())
			return true;

		break;
	}
	case MINOR_CIV_QUEST_LIBERATION:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		PlayerTypes eTargetPlayer = (PlayerTypes)m_iData3;

		// No city here anymore!
		if (!pPlot->isCity())
			return true;

		CvCity* pTargetCity = pPlot->getPlotCity();
		PlayerTypes eCityOwner = pTargetCity->getOwner();

		// Someone other than us liberated them, or the city revolted.
		if (eCityOwner == eTargetPlayer && !pTargetCity->isEverLiberated(m_eAssignedPlayer))
			return true;

		// We conquered the city. Oops.
		if (GET_PLAYER(eCityOwner).getTeam() == pAssignedPlayer->getTeam())
			return true;

		// The quest giver conquered the city.
		if (GET_PLAYER(eCityOwner).getTeam() == pMinor->getTeam())
			return true;

		// The quest giver's ally conquered the city.
		PlayerTypes eAlly = pMinor->GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eCityOwner).getTeam())
			return true;

		// We can't liberate this city for some reason.
		if (GET_PLAYER(m_eAssignedPlayer).GetPlayerToLiberate(pTargetCity) != eTargetPlayer)
			return true;

		// We can't go to war with the owner of this city.
		if (!GET_PLAYER(m_eAssignedPlayer).IsAtWarWith(eCityOwner) && !GET_TEAM(pAssignedPlayer->getTeam()).canDeclareWar(GET_PLAYER(eCityOwner).getTeam(), m_eAssignedPlayer))
			return true;

		// Is the city owner now an unacceptable target (backstabbing)?
		if (GET_PLAYER(eCityOwner).isMajorCiv())
		{
			if (!pMinor->GetMinorCivAI()->IsAcceptableQuestEnemy(MINOR_CIV_QUEST_LIBERATION, m_eAssignedPlayer, eCityOwner))
				return true;
		}
		else if (GET_PLAYER(eCityOwner).isMinorCiv())
		{
			PlayerTypes eCityOwnerAlly = GET_PLAYER(eCityOwner).GetMinorCivAI()->GetAlly();
			if (eCityOwnerAlly != NO_PLAYER && GET_PLAYER(eCityOwnerAlly).getTeam() == pAssignedPlayer->getTeam())
				return true;

			vector<PlayerTypes> vPlayerTeam = GET_TEAM(pAssignedPlayer->getTeam()).getPlayers();
			for (size_t i=0; i<vPlayerTeam.size(); i++)
			{
				if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
					return true;

				if (GET_PLAYER(eCityOwner).GetMinorCivAI()->IsProtectedByMajor(vPlayerTeam[i]))
					return true;
			}
		}

		break;
	}
	case MINOR_CIV_QUEST_HORDE:
	{
		// The Horde is still in the City-State's threat-radius - oh no!
		if (GC.getGame().getGameTurn() >= GetEndTurn() && pMinor->GetMinorCivAI()->GetNumBarbariansInBorders(true) > 0)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_REBELLION:
	{
		// Are there still rebels milling about? You lose!
		if (GC.getGame().getGameTurn() >= GetEndTurn() && pMinor->GetMinorCivAI()->GetNumBarbariansInBorders(true) > 0)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
	{
		// Fewer cities than # of buildings required?
		if (pAssignedPlayer->getNumCities() < m_iData2)
			return true;

		BuildingTypes eBuilding = (BuildingTypes)m_iData1;

		// Check if the player is no longer able to construct the Building in enough cities
		std::vector<int> allBuildingCount = GET_PLAYER(m_eAssignedPlayer).GetTotalBuildingCount();
		int iNumValidCities = 0;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(m_eAssignedPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_eAssignedPlayer).nextCity(&iCityLoop))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				iNumValidCities++;
				continue;
			}

			if (!pLoopCity->isBuildingInQueue(eBuilding) && !pLoopCity->canConstruct(eBuilding,allBuildingCount))
				continue;

			iNumValidCities++;
		}
		if (iNumValidCities < m_iData2)
			return true;

		break;
	}
	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
	{
		// Someone killed the player
		PlayerTypes eTargetPlayer = (PlayerTypes) GetPrimaryData();
		if (!GET_PLAYER(eTargetPlayer).isAlive())
			return true;

		// Player is now a bad target?
		if (!pMinor->GetMinorCivAI()->IsAcceptableQuestEnemy(MINOR_CIV_QUEST_SPY_ON_MAJOR, m_eAssignedPlayer, eTargetPlayer))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_COUP:
	{
		PlayerTypes eTargetMinor = (PlayerTypes)m_iData1;
		CvPlayer* pTargetMinor = &GET_PLAYER(eTargetMinor);

		// City-State dead?
		if (!pTargetMinor->isAlive())
			return true;

		// City-State has no ally?
		if (pTargetMinor->GetMinorCivAI()->GetAlly() == NO_PLAYER)
			return true;

		// Sphere of Influence in place?
		if (pTargetMinor->GetMinorCivAI()->GetPermanentAlly() != NO_PLAYER)
			return true;
		
		if (pTargetMinor->GetMinorCivAI()->IsCoupAttempted(m_eAssignedPlayer))
		{
			// Failed coup attempt?
			if (pTargetMinor->GetMinorCivAI()->GetAlly() != m_eAssignedPlayer)
				return true;
		}

		// Ally is now a bad target?
		if (!pMinor->GetMinorCivAI()->IsAcceptableQuestEnemy(MINOR_CIV_QUEST_COUP, m_eAssignedPlayer, pTargetMinor->GetMinorCivAI()->GetAlly()))
			return true;

		break;
	}
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		PlayerTypes eCityOwner = pPlot->getOwner();
		if (!pPlot->isCity())
		{
			// No city here anymore, and we didn't destroy it?
			if (pPlot->GetPlayerThatDestroyedCityHere() != m_eAssignedPlayer)
				return true;
		}
		else if (eCityOwner != m_eAssignedPlayer)
		{
			// The quest giver conquered the city.
			if (GET_PLAYER(eCityOwner).getTeam() == pMinor->getTeam())
				return true;

			// The quest giver's ally conquered the city.
			PlayerTypes eAlly = pMinor->GetMinorCivAI()->GetAlly();
			if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eCityOwner).getTeam())
				return true;

			// We can't go to war with the owner of this city.
			if (!GET_PLAYER(m_eAssignedPlayer).IsAtWarWith(eCityOwner) && !GET_TEAM(pAssignedPlayer->getTeam()).canDeclareWar(GET_PLAYER(eCityOwner).getTeam(), m_eAssignedPlayer))
				return true;

			// Is the city owner now an unacceptable target (backstabbing)?
			if (GET_PLAYER(eCityOwner).isMajorCiv())
			{
				if (!pMinor->GetMinorCivAI()->IsAcceptableQuestEnemy(MINOR_CIV_QUEST_ACQUIRE_CITY, m_eAssignedPlayer, eCityOwner))
					return true;
			}
			else if (GET_PLAYER(eCityOwner).isMinorCiv())
			{
				PlayerTypes eCityOwnerAlly = GET_PLAYER(eCityOwner).GetMinorCivAI()->GetAlly();
				if (eCityOwnerAlly != NO_PLAYER && GET_PLAYER(eCityOwnerAlly).getTeam() == pAssignedPlayer->getTeam())
					return true;

				vector<PlayerTypes> vPlayerTeam = GET_TEAM(pAssignedPlayer->getTeam()).getPlayers();
				for (size_t i=0; i<vPlayerTeam.size(); i++)
				{
					if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
						return true;

					if (GET_PLAYER(eCityOwner).GetMinorCivAI()->IsProtectedByMajor(vPlayerTeam[i]))
						return true;
				}
			}
		}

		break;
	}
	default:
		break; // No special expiry conditions.
	}

	return false;
}

bool CvMinorCivQuest::IsObsolete(bool bWar)
{
	return (IsRevoked(bWar) || IsExpired());
}

// The end of this quest has been handled, no effects should happen, and it is marked to be deleted
bool CvMinorCivQuest::IsHandled() const
{
	return m_bHandled;
}

void CvMinorCivQuest::SetHandled(bool bValue)
{
	m_bHandled = bValue;
}

// Initializes data to track quest progress and sends notification to player.
// NOTE: Some types initialize data using randomness here. So two otherwise equivalent quests may be initialized with different data.
// NOTE: Should only be called once.
void CvMinorCivQuest::DoStartQuest(int iStartTurn, PlayerTypes pCallingPlayer)
{
	m_iStartTurn = iStartTurn;
	int iDuration = GetEndTurn() != -1 ? GetEndTurn() - iStartTurn : 0;

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);

	Localization::String strMessage, strSummary;
	int iNotificationX = -1, iNotificationY = -1;

	// Calculate rewards based on quest type
	CalculateRewards(m_eAssignedPlayer);

	switch (m_eType)
	{
	case MINOR_CIV_QUEST_ROUTE:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_ROUTE");
		break;
	}
	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		CvPlot* pPlot = pMinor->GetMinorCivAI()->GetBestNearbyCampToKill();
		m_iData1 = pPlot->getX();
		m_iData2 = pPlot->getY();

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();
		break;
	}
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
	{
		ResourceTypes eResource = pMinor->GetMinorCivAI()->GetNearbyResourceForQuest(m_eAssignedPlayer);
		m_iData1 = eResource;

		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONNECT_RESOURCE");
		strSummary << strResourceName;
		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	{
		BuildingTypes eWonder = pMinor->GetMinorCivAI()->GetBestWorldWonderForQuest(m_eAssignedPlayer, iDuration);
		m_iData1 = eWonder;

		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
		break;
	}
	case MINOR_CIV_QUEST_GREAT_PERSON:
	{
		UnitTypes eUnit = pMinor->GetMinorCivAI()->GetBestGreatPersonForQuest(m_eAssignedPlayer);
		m_iData1 = eUnit;

		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GREAT_PERSON");
		strSummary << strUnitName;
		break;
	}
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		if (MOD_BALANCE_VP)
		{
			PlayerTypes eTargetCityState = NO_PLAYER;
			if (pCallingPlayer == NO_PLAYER)
				eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer, true);
			else
				eTargetCityState = pCallingPlayer;

			m_iData1 = eTargetCityState;
			int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			if (pCallingPlayer == NO_PLAYER)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE");
				strMessage << strTargetNameKey;
				strMessage << iTurnsRemaining;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
				strSummary << strTargetNameKey;
			}
			else
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE_ATTACKED");
				strMessage << strTargetNameKey;
				strMessage << iTurnsRemaining;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
				strSummary << strTargetNameKey;
			}

			//He wants to kill me, eh? Well I want to kill him too!
			if (pCallingPlayer == NO_PLAYER && eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isAlive())
			{
				if (!GET_TEAM(pMinor->getTeam()).isAtWar(GET_PLAYER(eTargetCityState).getTeam()))
				{
					//DOW!
					GET_TEAM(pMinor->getTeam()).setAtWar(GET_PLAYER(eTargetCityState).getTeam(), true, false);
					GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).setAtWar(pMinor->getTeam(), true, false);
					pMinor->GetMinorCivAI()->SetPermanentWar(GET_PLAYER(eTargetCityState).getTeam(), true);
					GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetPermanentWar(pMinor->getTeam(), true);

					Localization::String locString;

					// Message everyone about what happened
					if (GC.getGame().isFinalInitialized() && !pMinor->isBarbarian() && !GET_PLAYER(eTargetCityState).isBarbarian())
					{
						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							PlayerTypes ePlayer = (PlayerTypes) iI;

							if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).GetNotifications())
							{
								// Players that are on neither team, but know both parties
								if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(pMinor->getTeam()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eTargetCityState).getTeam()))
								{
									locString = Localization::Lookup("TXT_KEY_MISC_SOMEONE_DECLARED_WAR");
									locString << GET_TEAM(pMinor->getTeam()).getName().GetCString() << GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).getName().GetCString();
									GET_PLAYER(ePlayer).GetNotifications()->Add(NOTIFICATION_WAR, locString.toUTF8(), locString.toUTF8(), -1, -1, GET_TEAM(pMinor->getTeam()).getLeaderID(), eTargetCityState);
								}
							}
						}
					}
				}
				if (!GET_PLAYER(eTargetCityState).GetMinorCivAI()->IsActiveQuestForPlayer(m_eAssignedPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE))
				{
					// Since we are adding each instance of the quest separately, global quests should not rely on random choice of data
					GET_PLAYER(eTargetCityState).GetMinorCivAI()->AddQuestForPlayer(m_eAssignedPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE, GC.getGame().getGameTurn(), pMinor->GetID());
				}
				// Give out the quest to all players.
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eSecondaryPlayer = (PlayerTypes) iPlayerLoop;
					if (eSecondaryPlayer != NO_PLAYER && eSecondaryPlayer != m_eAssignedPlayer && GET_PLAYER(eSecondaryPlayer).isAlive())
					{
						if (GET_TEAM(GET_PLAYER(eSecondaryPlayer).getTeam()).isHasMet(pMinor->getTeam()))
						{
							if (!GET_TEAM(GET_PLAYER(eSecondaryPlayer).getTeam()).isHasMet(GET_PLAYER(eTargetCityState).getTeam()))
							{
								GET_TEAM(GET_PLAYER(eSecondaryPlayer).getTeam()).meet(GET_PLAYER(eTargetCityState).getTeam(), true);
							}
							if (!GET_PLAYER(eTargetCityState).GetMinorCivAI()->IsActiveQuestForPlayer(eSecondaryPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE))
							{
								// Since we are adding each instance of the quest separately, global quests should not rely on random choice of data
								GET_PLAYER(eTargetCityState).GetMinorCivAI()->AddQuestForPlayer(eSecondaryPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE, GC.getGame().getGameTurn(), pMinor->GetID());
							}
							if (!pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(eSecondaryPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE))
							{
								// Since we are adding each instance of the quest separately, global quests should not rely on random choice of data
								pMinor->GetMinorCivAI()->AddQuestForPlayer(eSecondaryPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE, GC.getGame().getGameTurn(), eTargetCityState);
							}
						}
					}
				}
			}
		}
		else
		{
			PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer, true);
			m_iData1 = eTargetCityState;

			const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}
		break;
	}
	case MINOR_CIV_QUEST_FIND_PLAYER:
	{
		PlayerTypes ePlayerToFind = pMinor->GetMinorCivAI()->GetBestPlayerToFind(m_eAssignedPlayer);
		m_iData1 = ePlayerToFind;

		const char* strCivKey = GET_PLAYER(ePlayerToFind).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_PLAYER");
		strSummary << strCivKey;
		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY:
	{
		CvCity* pCityToFind = pMinor->GetMinorCivAI()->GetBestCityToFind(m_eAssignedPlayer);
		m_iData1 = pCityToFind->getX();
		m_iData2 = pCityToFind->getY();

		const char* strTargetNameKey = pCityToFind->getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_CITY");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_CITY");
		strSummary << strTargetNameKey;
		break;
	}
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
	{
		m_iData1 = GET_TEAM(pAssignedPlayer->getTeam()).GetNumNaturalWondersDiscovered();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_NATURAL_WONDER");
		break;
	}
	case MINOR_CIV_QUEST_GIVE_GOLD:
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();
		m_iData1 = eMostRecentBully;
		m_iData2 = pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer);

		const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GIVE_GOLD");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GIVE_GOLD");
		break;
	}
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();
		m_iData1 = eMostRecentBully;

		const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_PLEDGE_TO_PROTECT");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_PLEDGE_TO_PROTECT");
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	{
		m_iData1 = pAssignedPlayer->GetJONSCultureEverGenerated();

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_CULTURE");
		strMessage << iTurnsRemaining;
		strMessage << iDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_CULTURE");
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		if (MOD_BALANCE_VP)
		{
			ReligionTypes eReligion = pAssignedPlayer->GetReligions()->GetStateReligion(false);
			m_iData1 = GC.getGame().GetGameReligions()->GetNumFollowers(eReligion);
			m_iData2 = eReligion;
		}
		else
		{
			m_iData1 = pAssignedPlayer->GetFaithEverGenerated();
		}

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_FAITH");
		strMessage << iTurnsRemaining;
		strMessage << iDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_FAITH");
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	{
		m_iData1 = GET_TEAM(pAssignedPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONTEST_TECHS");
		strMessage << iTurnsRemaining;
		strMessage << iDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_TECHS");
		break;
	}
	case MINOR_CIV_QUEST_INVEST:
	{
		m_iData1 = pMinor->GetMinorCivAI()->GetNumGoldGifted(m_eAssignedPlayer);

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iBoostPercentage = 50;//antonjs: todo: XML

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_INVEST");
		strMessage << iTurnsRemaining;
		strMessage << iBoostPercentage;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_INVEST");
		break;
	}
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	{
		PlayerTypes eTargetMinor = pMinor->GetMinorCivAI()->GetBestCityStateTarget(m_eAssignedPlayer, false);
		m_iData1 = eTargetMinor;
		m_iData2 = GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(m_eAssignedPlayer);

		const char* strCivKey = GET_PLAYER(eTargetMinor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_BULLY_CITY_STATE");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_BULLY_CITY_STATE");
		strSummary << strCivKey;
		break;
	}
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();
		m_iData1 = eMostRecentBully;

		const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_DENOUNCE_MAJOR");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_DENOUNCE_MAJOR");
		strSummary << strCivKey;
		break;
	}
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
	{
		ReligionTypes eReligion = GET_PLAYER(m_eAssignedPlayer).GetReligions()->GetOwnedReligion();
		m_iData1 = eReligion;

		const CvReligion* pkReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		CvString strReligion = pkReligion->GetName(); // Not a key, already localized (may be custom name)

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_SPREAD_RELIGION");
		strMessage << strReligion;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_SPREAD_RELIGION");
		strSummary << strReligion;
		break;
	}
	case MINOR_CIV_QUEST_TRADE_ROUTE:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_TRADE_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_TRADE_ROUTE");
		break;
	}
	case MINOR_CIV_QUEST_WAR:
	{
		PlayerTypes eMostRecentBully = pMinor->GetMinorCivAI()->GetMostRecentBullyForQuest();
		m_iData1 = eMostRecentBully;

		const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_START_WAR");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_START_WAR");
		strSummary << strCivKey;

		//Let's issue an attack request.
		if (!pAssignedPlayer->isHuman() && GET_TEAM(pAssignedPlayer->getTeam()).canDeclareWar(GET_PLAYER(eMostRecentBully).getTeam(), pAssignedPlayer->GetID()))
			pAssignedPlayer->GetMilitaryAI()->RequestCityAttack(eMostRecentBully,2);

		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
	{
		BuildingTypes eNationalWonder = pMinor->GetMinorCivAI()->GetBestNationalWonderForQuest(m_eAssignedPlayer, iDuration);
		m_iData1 = eNationalWonder;

		const char* strBuildingName = GC.getBuildingInfo(eNationalWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CONSTRUCT_NATIONAL_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONSTRUCT_NATIONAL_WONDER");
		strSummary << strBuildingName;
		break;
	}
	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
	{
		UnitTypes eUnitType = pMinor->GetMinorCivAI()->GetBestUnitGiftFromPlayer(m_eAssignedPlayer);
		m_iData1 = eUnitType;
		m_iData2 = pMinor->GetMinorCivAI()->GetExperienceForUnitGiftQuest(m_eAssignedPlayer, eUnitType);

		const char* strUnitName = GC.getUnitInfo(eUnitType)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_GIFT_SPECIFIC_UNIT");
		strMessage << strUnitName;
		strMessage << m_iData2;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_GIFT_SPECIFIC_UNIT");
		strSummary << strUnitName;
		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
	{
		PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateMeetTarget(m_eAssignedPlayer);
		m_iData1 = eTargetCityState;

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_FIND_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_FIND_CITY_STATE");
		strSummary << strTargetNameKey;

		//Let's issue a recon request.
		EconomicAIStrategyTypes eNavalRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		if (!pAssignedPlayer->isHuman() && !pAssignedPlayer->GetEconomicAI()->IsUsingStrategy(eNavalRecon))
			pAssignedPlayer->GetEconomicAI()->SetUsingStrategy(eNavalRecon, true);
		break;
	}
	case MINOR_CIV_QUEST_INFLUENCE:
	{
		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		int iBoostPercentage = /*20*/ GD_INT_GET(INFLUENCE_MINOR_QUEST_BOOST);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_INFLUENCE");
		strMessage << iTurnsRemaining;
		strMessage << iBoostPercentage;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_INFLUENCE");
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		m_iData1 = pAssignedPlayer->GetCulture()->GetTourism() / 100;

		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_TOURISM");
		strMessage << iTurnsRemaining;
		strMessage << iDuration;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_TOURISM");
		break;
	}
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		CvPlot* pPlot = pMinor->GetMinorCivAI()->GetBestNearbyDig();
		m_iData1 = pPlot->getX();
		m_iData2 = pPlot->getY();

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->SetResourceForceReveal(pAssignedPlayer->getTeam(), true);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ARCHAEOLOGY");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();
		break;
	}
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_CIRCUMNAVIGATION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CIRCUMNAVIGATION");

		//Let's issue a recon request.
		EconomicAIStrategyTypes eNavalRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		if (!pAssignedPlayer->isHuman() && !pAssignedPlayer->GetEconomicAI()->IsUsingStrategy(eNavalRecon))
			pAssignedPlayer->GetEconomicAI()->SetUsingStrategy(eNavalRecon, true);

		break;
	}
	case MINOR_CIV_QUEST_LIBERATION:
	{
		PlayerTypes eTargetCityState = pMinor->GetMinorCivAI()->GetBestCityStateLiberate(m_eAssignedPlayer);
		m_iData1 = GET_PLAYER(eTargetCityState).GetOriginalCapitalX();
		m_iData2 = GET_PLAYER(eTargetCityState).GetOriginalCapitalY();
		m_iData3 = eTargetCityState;

		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		const char* strTargetNameKey = pPlot->getPlotCity()->getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_LIBERATION");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_LIBERATION");
		strSummary << strTargetNameKey;

		//Let's issue an attack request.
		PlayerTypes eCityOwner = pPlot->getOwner();
		if (!pAssignedPlayer->isHuman())
			pAssignedPlayer->GetMilitaryAI()->RequestCityAttack(eCityOwner,2);

		break;
	}
	case MINOR_CIV_QUEST_HORDE:
	{
		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_HORDE");
		strMessage << iTurnsRemaining;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_HORDE");
		
		if (!pMinor->GetMinorCivAI()->IsHordeActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(/*20*/ GD_INT_GET(MINOR_QUEST_REBELLION_TIMER));
			pMinor->GetMinorCivAI()->SetHordeActive(true);
		}

		//Tell the AI to get over there!
		if (!pAssignedPlayer->isHuman() && pAssignedPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(false) <= 0)
		{
			CvCity* pMinorCap = pMinor->getCapitalCity();
			if (pMinorCap && pAssignedPlayer->getCapitalCity() && pMinorCap->HasSharedAreaWith(pAssignedPlayer->getCapitalCity(),true,false))
			{
				PlayerProximityTypes eProximity = GET_PLAYER(pMinor->GetID()).GetProximityToPlayer(pAssignedPlayer->GetID());
				if (eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					pAssignedPlayer->addAIOperation(AI_OPERATION_CITY_DEFENSE, 1, pMinor->GetID(), pMinorCap);
			}
		}
		break;
	}
	case MINOR_CIV_QUEST_REBELLION:
	{
		int iTurnsRemaining = GetEndTurn() - GC.getGame().getGameTurn();
		PlayerTypes eMajor = pMinor->GetMinorCivAI()->GetAlly();
		if (eMajor != NO_PLAYER)
		{
			m_iData1 = eMajor;
			PolicyBranchTypes ePreferredIdeology = GET_PLAYER(eMajor).GetCulture()->GetPublicOpinionPreferredIdeology();
			const char* strCivKey = GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();

			if (ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_FREEDOM))
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_FREEDOM");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION");
				strSummary << strCivKey;
			}
			else if (ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_ORDER))
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_ORDER");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION");
				strSummary << strCivKey;
			}
			else if (ePreferredIdeology == GD_INT_GET(POLICY_BRANCH_AUTOCRACY))
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_AUTOCRACY");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION");
				strSummary << strCivKey;
			}
			else
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_GENERAL_BLANK");
				strMessage << iTurnsRemaining;
				strMessage << strCivKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION_BLANK");
				strSummary << strCivKey;
			}
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_REBELLION_GENERAL_BLANK_RANDOM");
			strMessage << iTurnsRemaining;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_REBELLION_BLANK_RANDOM");
		}

		if (!pMinor->GetMinorCivAI()->IsRebellionActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(/*20*/ GD_INT_GET(MINOR_QUEST_REBELLION_TIMER));
			pMinor->GetMinorCivAI()->SetRebellionActive(true);
		}

		//Tell the AI ally to get over there!
		if (eMajor != NO_PLAYER && !GET_PLAYER(eMajor).isHuman() && GET_PLAYER(eMajor).GetMilitaryAI()->GetNumberCivsAtWarWith(false) <= 0)
		{
			if (pMinor->GetMinorCivAI()->GetAlly() == eMajor)
			{
				CvCity* pMinorCap = pMinor->getCapitalCity();
				CvCity* pClosestCity = pAssignedPlayer->GetClosestCityByPathLength(pMinorCap->plot());
				if (pClosestCity)
					pAssignedPlayer->addAIOperation(AI_OPERATION_CITY_DEFENSE, 1, pMinor->GetID(), pMinor->getCapitalCity(), pClosestCity);
			}
		}
		break;
	}
	case MINOR_CIV_QUEST_EXPLORE_AREA:
	{
		CvPlot* pPlot = pMinor->GetMinorCivAI()->GetTargetPlot(m_eAssignedPlayer);
		m_iData1 = pPlot->getX();
		m_iData2 = pPlot->getY();

		//Set the plot's area ID on the minor civ so that no one else goes for it.
		pMinor->GetMinorCivAI()->SetTargetedAreaID(m_eAssignedPlayer, pPlot->getArea());

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_EXPLORE_AREA");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_EXPLORE_AREA");

		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();
		break;
	}
	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
	{
		BuildingTypes eBuilding = pMinor->GetMinorCivAI()->GetBestBuildingForQuest(m_eAssignedPlayer, iDuration);

		int iCities = pAssignedPlayer->getNumCities() - pAssignedPlayer->GetNumPuppetCities();
		int iActionAmount = GC.getGame().getSmallFakeRandNum(4, pAssignedPlayer->GetPseudoRandomSeed());
		if (iActionAmount == 1)
		{
			iActionAmount = iCities;
		}
		else if (iActionAmount == 2 || pAssignedPlayer->getNumCities() == 4)
		{
			iActionAmount = iCities / 2;
		}
		else
		{
			int iSomewhereInBetween = iCities - (iCities / 2) - 1;
			iActionAmount = (iCities / 2) + GC.getGame().getSmallFakeRandNum(iSomewhereInBetween, pMinor->GetPseudoRandomSeed());
		}

		if (iActionAmount > iCities)
			iActionAmount = iCities;

		m_iData1 = eBuilding;
		m_iData2 = iActionAmount;

		const char* strBuildingName = GC.getBuildingInfo(eBuilding)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_BUILD_X_BUILDINGS");
		strMessage << strBuildingName;
		strMessage << iActionAmount;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_BUILD_X_BUILDINGS");
		break;
	}
	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
	{
		CvCity* pCity = pMinor->GetMinorCivAI()->GetBestSpyTarget(m_eAssignedPlayer, false);
		int iActionAmount = GC.getGame().getSmallFakeRandNum(3, *pCity->plot());
		if (iActionAmount <= 0)
			iActionAmount = 1;

		m_iData1 = pCity->getOwner();
		m_iData2 = iActionAmount + pAssignedPlayer->GetEspionage()->GetNumSpyActionsDone(pCity->getOwner());

		const char* strTargetNameKey = GET_PLAYER(pCity->getOwner()).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_SPY_ON_MAJOR");
		strMessage << strTargetNameKey;
		strMessage << (m_iData2 - pAssignedPlayer->GetEspionage()->GetNumSpyActionsDone(pCity->getOwner()));
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_SPY_ON_MAJOR");
		break;
	}
	case MINOR_CIV_QUEST_COUP:
	{
		CvCity* pCity = pMinor->GetMinorCivAI()->GetBestSpyTarget(m_eAssignedPlayer, true);
		m_iData1 = pCity->getOwner();

		const char* strTargetNameKey = pCity->getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COUP");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COUP");

		iNotificationX = pCity->getX();
		iNotificationY = pCity->getY();
		break;
	}
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
	{
		CvCity* pCity = pMinor->GetMinorCivAI()->GetBestCityForQuest(m_eAssignedPlayer);
		m_iData1 = pCity->plot()->getX();
		m_iData2 = pCity->plot()->getY();
		m_iData3 = pCity->getOwner();

		pMinor->GetMinorCivAI()->SetTargetedCityX(m_eAssignedPlayer, pCity->getX());
		pMinor->GetMinorCivAI()->SetTargetedCityY(m_eAssignedPlayer, pCity->getY());

		const char* strTargetNameKey = pCity->getNameKey();

		if (pAssignedPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ACQUIRE_CITY_OCC");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_ACQUIRE_CITY_OCC");
			strSummary << strTargetNameKey;
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ACQUIRE_CITY");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_ACQUIRE_CITY");
			strSummary << strTargetNameKey;
		}

		iNotificationX = pCity->plot()->getX();
		iNotificationY = pCity->plot()->getY();
		break;
	}
	default:
		return;
	}

	strMessage << pMinor->getNameKey();
	strSummary << pMinor->getNameKey();

	CvString sMessage = strMessage.toUTF8();
	CvString sSummary = strSummary.toUTF8();

	//Additional Bonuses from Quests here.
	CvString sExtra = GetRewardString(m_eAssignedPlayer, false);
	if (sExtra != "")
		sMessage = sMessage + "[NEWLINE]" + sExtra;

	pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer, iNotificationX, iNotificationY);
}

// Begins the quest, but with client-provided data rather than initializing data internally.
// Should only be called when starting a quest for a player that has an equivalent quest which already exists for a different player (global quests).
void CvMinorCivQuest::DoStartQuestUsingExistingData(CvMinorCivQuest* pExistingQuest)
{
	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);
	CvPlayer* pAssignedPlayer = &GET_PLAYER(m_eAssignedPlayer);
	if (pMinor == NULL || pAssignedPlayer == NULL || pExistingQuest == NULL) return;

	Localization::String strMessage;
	Localization::String strSummary;
	int iNotificationX = -1;
	int iNotificationY = -1;

	// Kill a Camp - This quest needs to have the data for the same camp as the existing quest
	if (m_eType == MINOR_CIV_QUEST_KILL_CAMP)
	{
		m_iStartTurn = pExistingQuest->GetStartTurn();

		int iCampX = pExistingQuest->GetPrimaryData();
		int iCampY = pExistingQuest->GetSecondaryData();

		CvPlot* pPlot = GC.getMap().plot(iCampX, iCampY);

		m_iData1 = iCampX;
		m_iData2 = iCampY;

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->setRevealedImprovementType(pAssignedPlayer->getTeam(), pPlot->getImprovementType());

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CAMP");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();

		CvString sMessage = strMessage.toUTF8();
		CvString sSummary = strSummary.toUTF8();

		CalculateRewards(m_eAssignedPlayer);

		//Additional Bonuses from Quests here.
		CvString sExtra = GetRewardString(m_eAssignedPlayer, true);
		if (sExtra != "")
			sMessage = sMessage + "[NEWLINE]" + sExtra;

		pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer, iNotificationX, iNotificationY);
	}
	else if (m_eType == MINOR_CIV_QUEST_ARCHAEOLOGY)
	{
		m_iStartTurn = pExistingQuest->GetStartTurn();

		int iDigX = pExistingQuest->GetPrimaryData();
		int iDigY = pExistingQuest->GetSecondaryData();

		CvPlot* pPlot = GC.getMap().plot(iDigX, iDigY);

		m_iData1 = iDigX;
		m_iData2 = iDigY;

		pPlot->setRevealed(pAssignedPlayer->getTeam(), true);
		pPlot->SetResourceForceReveal(pAssignedPlayer->getTeam(), pPlot->getResourceType()!=NO_RESOURCE);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ARCHAEOLOGY");
		iNotificationX = pPlot->getX();
		iNotificationY = pPlot->getY();

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();

		CvString sMessage = strMessage.toUTF8();
		CvString sSummary = strSummary.toUTF8();

		CalculateRewards(m_eAssignedPlayer);

		//Additional Bonuses from Quests here.
		CvString sExtra = GetRewardString(m_eAssignedPlayer, true);
		if (sExtra != "")
			sMessage = sMessage + "[NEWLINE]" + sExtra;

		pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer, iNotificationX, iNotificationY);
	}
	else if (m_eType == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		PlayerTypes eTargetCityState = (PlayerTypes)pExistingQuest->GetPrimaryData();
		m_iData1 = eTargetCityState;

		if (!GET_TEAM(GET_PLAYER(m_eAssignedPlayer).getTeam()).isHasMet(GET_PLAYER(eTargetCityState).getTeam()))
			GET_TEAM(GET_PLAYER(m_eAssignedPlayer).getTeam()).meet(GET_PLAYER(eTargetCityState).getTeam(), true);

		m_iStartTurn = pExistingQuest->GetStartTurn();
		int iTurnsRemaining = pExistingQuest->GetEndTurn() - GC.getGame().getGameTurn();

		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE");
		strMessage << strTargetNameKey;
		strMessage << iTurnsRemaining;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE");
		strSummary << strTargetNameKey;

		strMessage << pMinor->getNameKey();
		strSummary << pMinor->getNameKey();

		CvString sMessage = strMessage.toUTF8();
		CvString sSummary = strSummary.toUTF8();

		CalculateRewards(m_eAssignedPlayer);

		//Additional Bonuses from Quests here.
		CvString sExtra = GetRewardString(m_eAssignedPlayer, true);
		if (sExtra != "")
			sMessage = sMessage + "[NEWLINE]" + sExtra;

		pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer, iNotificationX, iNotificationY);

		//He wants to kill me, eh? Well I want to kill him too!
		if (eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isAlive())
		{
			if (!GET_PLAYER(eTargetCityState).GetMinorCivAI()->IsActiveQuestForPlayer(m_eAssignedPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE))
				GET_PLAYER(eTargetCityState).GetMinorCivAI()->AddQuestForPlayer(m_eAssignedPlayer, MINOR_CIV_QUEST_KILL_CITY_STATE, pExistingQuest->GetStartTurn(), pMinor->GetID());
		}
	}
	// Other global quests (ie. contests) - Quest data is initialized as normal except for the start turn, which was in the past
	else if (pMinor->GetMinorCivAI()->IsGlobalQuest(pExistingQuest->GetType()))
	{
		DoStartQuest(pExistingQuest->GetStartTurn());
	}
	// Personal quests - Should not be started from an existing quest's data!!
	else
	{
		UNREACHABLE();
	}
}

// Awards influence and sends notification to player.
// Should only be called once.
bool CvMinorCivQuest::DoFinishQuest()
{
	if (!IsComplete())
		return false;

	if (IsHandled())
		return false;

	SetHandled(true); // We are handling the end of the quest, and this should only happen once

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);

	bool bWasFriends = pMinor->GetMinorCivAI()->IsFriends(m_eAssignedPlayer);
	bool bWasAllies = pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer);
	PlayerTypes eOldAlly = pMinor->GetMinorCivAI()->GetAlly();

	DoRewards(m_eAssignedPlayer);
	
	bool bNowFriends = pMinor->GetMinorCivAI()->IsFriends(m_eAssignedPlayer);
	bool bNowAllies = pMinor->GetMinorCivAI()->IsAllies(m_eAssignedPlayer);
	PlayerTypes eNewAlly = pMinor->GetMinorCivAI()->GetAlly();

	Localization::String strMessage;
	Localization::String strSummary;
	CivsList veNamesToShow;

	switch (m_eType)
	{
	case MINOR_CIV_QUEST_ROUTE:
	{
		// Route exists!
		pMinor->GetMinorCivAI()->SetRouteConnectionEstablished(m_eAssignedPlayer, true);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ROUTE_CONNECTION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ROUTE_CONNECTION");
		break;
	}
	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CAMP");
		break;
	}
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
	{
		ResourceTypes eResource = (ResourceTypes) GetPrimaryData();
		const char* strResourceName = GC.getResourceInfo(eResource)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE");
		strMessage << strResourceName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONNECT_RESOURCE");
		strSummary << strResourceName;
		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
		const char* strBuildingName = GC.getBuildingInfo(eWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
		break;
	}
	case MINOR_CIV_QUEST_GREAT_PERSON:
	{
		UnitTypes eUnit = (UnitTypes) GetPrimaryData();
		const char* strUnitName = GC.getUnitInfo(eUnit)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GREAT_PERSON");
		strSummary << strUnitName;
		break;
	}
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		if (MOD_BALANCE_VP)
		{
			// Peace!
			if (eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isAlive() && pMinor->isAlive() && GET_PLAYER(pMinor->GetID()).GetMinorCivAI()->GetAlly() == m_eAssignedPlayer && GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetAlly() == m_eAssignedPlayer)
			{
				GET_TEAM(pMinor->getTeam()).makePeace(GET_PLAYER(eTargetCityState).getTeam(), true, false, pMinor->GetID());
				GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).makePeace(pMinor->getTeam(), true, false, eTargetCityState);

				pMinor->GetMinorCivAI()->SetPermanentWar(GET_PLAYER(eTargetCityState).getTeam(), false);
				GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetPermanentWar(pMinor->getTeam(), false);

				const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CS_PEACE_MADE");
				strMessage << strTargetNameKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CS_WAR");
				strSummary << strTargetNameKey;
			}
			// Destruction...
			else
			{
				GET_TEAM(pMinor->getTeam()).makePeace(GET_PLAYER(eTargetCityState).getTeam(), true, false, pMinor->GetID());
				GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).makePeace(pMinor->getTeam(), true, false, eTargetCityState);

				pMinor->GetMinorCivAI()->SetPermanentWar(GET_PLAYER(eTargetCityState).getTeam(), false);
				GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetPermanentWar(pMinor->getTeam(), false);

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE");
				strMessage << strTargetNameKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE");
				strSummary << strTargetNameKey;
			}
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}
		break;
	}
	case MINOR_CIV_QUEST_FIND_PLAYER:
	{
		PlayerTypes ePlayerFound = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(ePlayerFound).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_PLAYER");
		strSummary << strCivKey;
		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		CvCity* pCityFound = pPlot->getPlotCity();
		const char* strTargetNameKey = pCityFound->getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_CITY");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_CITY");
		strSummary << strTargetNameKey;
		break;
	}
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_NATURAL_WONDER");
		break;
	}
	case MINOR_CIV_QUEST_GIVE_GOLD:
	{
		PlayerTypes eMostRecentBully = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIVE_GOLD");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GIVE_GOLD");
		break;
	}
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
	{
		PlayerTypes eMostRecentBully = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(eMostRecentBully).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_PLEDGE_TO_PROTECT");
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_CULTURE");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_FAITH");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONTEST_TECHS");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_INVEST:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_INVEST");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_INVEST");
		break;
	}
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	{
		PlayerTypes eTargetMinor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(eTargetMinor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_BULLY_CITY_STATE");
		strSummary << strCivKey;
		break;
	}
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
	{
		PlayerTypes eTargetMajor = (PlayerTypes) GetPrimaryData();
		const char* strCivKey = GET_PLAYER(eTargetMajor).getCivilizationShortDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR");
		strMessage << strCivKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_DENOUNCE_MAJOR");
		strSummary << strCivKey;
		break;
	}
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
	{
		ReligionTypes eReligion = (ReligionTypes) GetPrimaryData();
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);
		CvString strReligionKey = pReligion->GetName();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION");
		strMessage << strReligionKey.c_str();
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_SPREAD_RELIGION");
		strSummary << strReligionKey.c_str();
		break;
	}
	case MINOR_CIV_QUEST_TRADE_ROUTE:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_TRADE_ROUTE");
		break;
	}
	case MINOR_CIV_QUEST_WAR:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_WAR");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_WAR");
		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
	{
		BuildingTypes eNationalWonder = (BuildingTypes) GetPrimaryData();
		const char* strBuildingName = GC.getBuildingInfo(eNationalWonder)->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_NATIONAL_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CONSTRUCT_NATIONAL_WONDER");
		strSummary << strBuildingName;
		break;
	}
	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
	{
		UnitTypes eUnitType = (UnitTypes)GetPrimaryData();
		const char* strUnitName = GC.getUnitInfo(eUnitType)->GetDescriptionKey();

		pMinor->GetMinorCivAI()->SetHasSentUnitForQuest(m_eAssignedPlayer, false);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIFT_SPECIFIC_UNIT");
		strMessage << strUnitName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_GIFT_SPECIFIC_UNIT");
		strSummary << strUnitName;
		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_CITY_STATE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_FIND_CITY_STATE");
		strSummary << strTargetNameKey;
		break;
	}
	case MINOR_CIV_QUEST_INFLUENCE:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_INFLUENCE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_INFLUENCE");
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TOURISM");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_TOURISM");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_ARCHAEOLOGY");
		break;
	}
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CIRCUMNAVIGATION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_CIRCUMNAVIGATION");
		break;
	}
	case MINOR_CIV_QUEST_LIBERATION:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		const char* strTargetNameKey = pPlot->getPlotCity()->getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_LIBERATION");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_LIBERATION");
		strSummary << strTargetNameKey;
		break;
	}
	case MINOR_CIV_QUEST_HORDE:
	{
		if (pMinor->GetMinorCivAI()->IsHordeActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
			pMinor->GetMinorCivAI()->SetHordeActive(false);
			pMinor->GetMinorCivAI()->SetCooldownSpawn(30);
		}

		// Update Military AI
		CvAIOperation* pOp = GET_PLAYER(m_eAssignedPlayer).getFirstAIOperationOfType(AI_OPERATION_CITY_DEFENSE, pMinor->GetID());
		if (pOp)
			pOp->SetToAbort(AI_ABORT_NO_TARGET);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_HORDE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_HORDE");
		break;
	}
	case MINOR_CIV_QUEST_REBELLION:
	{
		if (pMinor->GetMinorCivAI()->IsRebellionActive())
		{
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
			pMinor->GetMinorCivAI()->SetRebellionActive(false);
			pMinor->GetMinorCivAI()->SetCooldownSpawn(30);
		}
	
		//Update Military AI
		CvAIOperation* pOp = GET_PLAYER(m_eAssignedPlayer).getFirstAIOperationOfType(AI_OPERATION_CITY_DEFENSE, pMinor->GetID());
		if (pOp)
			pOp->SetToAbort(AI_ABORT_NO_TARGET);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COMPLETE_REBELLION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_REBELLION");
		break;
	}
	case MINOR_CIV_QUEST_EXPLORE_AREA:
	{
		pMinor->GetMinorCivAI()->SetTargetedAreaID(m_eAssignedPlayer, -1);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_EXPLORE_AREA_COMPLETE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_EXPLORE_AREA_COMPLETE");
		break;
	}
	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_BUILD_X_BUILDINGS_COMPLETE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_BUILD_X_BUILDINGS_COMPLETE");
		break;
	}
	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
	{
		PlayerTypes eTargetMajor = (PlayerTypes)GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetMajor).getNameKey();
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_SPY_ON_MAJOR_COMPLETE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_SPY_ON_MAJOR_COMPLETE");
		break;
	}
	case MINOR_CIV_QUEST_COUP:
	{
		PlayerTypes eTargetCityState = (PlayerTypes)GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		pMinor->GetMinorCivAI()->SetCoupAttempted(m_eAssignedPlayer, false);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_COUP_COMPLETE");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_COUP_COMPLETE");
		break;
	}
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
	{
		if (GET_PLAYER(m_eAssignedPlayer).isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ACQUIRE_CITY_OCC_COMPLETE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_ACQUIRE_CITY_OCC_COMPLETE");
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ACQUIRE_CITY_COMPLETE");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_ACQUIRE_CITY_COMPLETE");
		}
		break;
	}
	default:
		break;
	}

	// Update the UI with the changed data, in case it is open
	if (m_eAssignedPlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	strMessage << pMinor->getNameKey();
	strSummary << pMinor->getNameKey();

	CvString sMessage = strMessage.toUTF8();
	CvString sSummary = strSummary.toUTF8();

	// This quest involved multiple minors, so grab their names for the notification
	if (veNamesToShow.size() > 0)
		sMessage += pMinor->GetMinorCivAI()->GetNamesListAsString(veNamesToShow);

	// This quest reward changed our status, so grab that info for the notification
	if ((!bWasFriends && bNowFriends) || (!bWasAllies && bNowAllies))
	{
		pair<CvString, CvString> statusChangeStrings = pMinor->GetMinorCivAI()->GetStatusChangeNotificationStrings(m_eAssignedPlayer, /*bAdd*/ true, bNowFriends, bNowAllies, eOldAlly, eNewAlly);
		sMessage = sMessage + "[NEWLINE][NEWLINE]" + statusChangeStrings.first;
	}

	//Additional Bonuses from Quests here.
	CvString sExtra = GetRewardString(m_eAssignedPlayer, true);
	if (sExtra != "")
		sMessage = sMessage + "[NEWLINE]" + sExtra;

	pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer);
	return true;
}

// Do any cleanup and notifications for when a quest is cancelled (ex. becomes obsolete)
bool CvMinorCivQuest::DoCancelQuest()
{
	if (IsHandled())
		return false;

	SetHandled(true); // We are handling the end of the quest, and this should only happen once

	CvPlayer* pMinor = &GET_PLAYER(m_eMinor);

	// Why is the quest cancelled?  Will affect which notification message we send
	bool bRevoked = IsRevoked();
	bool bExpired = IsExpired();

	Localization::String strMessage;
	Localization::String strSummary;
	CivsList veNamesToShow;

	// If quest was revoked due to bullying, notification is handled elsewhere (to allow condensing)
	if (bRevoked)
		return true;

	// This should not happen.
	if (!bExpired)
		return true;

	// If quest expired "naturally", send a notification particular to the expiration conditions of the quest
	switch (m_eType)
	{
	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		strMessage = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP");
		strSummary = Localization::Lookup("TXT_KEY_NTFN_QUEST_ENDED_KILL_CAMP_S");
		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	{
		BuildingTypes eWonder = (BuildingTypes) GetPrimaryData();
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eWonder);
		const char* strBuildingName = pkBuildingInfo->GetDescriptionKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONSTRUCT_WONDER");
		strMessage << strBuildingName;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONSTRUCT_WONDER");
		strSummary << strBuildingName;
		break;
	}
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		PlayerTypes eTargetCityState = (PlayerTypes) GetPrimaryData();
		const char* strTargetNameKey = GET_PLAYER(eTargetCityState).getNameKey();

		if (MOD_BALANCE_VP)
		{
			// Peace!
			if (eTargetCityState != NO_PLAYER && GET_PLAYER(eTargetCityState).isAlive() && pMinor->isAlive() && GET_PLAYER(pMinor->GetID()).GetMinorCivAI()->GetAlly() == m_eAssignedPlayer && GET_PLAYER(eTargetCityState).GetMinorCivAI()->GetAlly() == m_eAssignedPlayer)
			{
				GET_TEAM(pMinor->getTeam()).makePeace(GET_PLAYER(eTargetCityState).getTeam(), true, false, pMinor->GetID());
				GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).makePeace(pMinor->getTeam(), true, false, eTargetCityState);

				pMinor->GetMinorCivAI()->SetPermanentWar(GET_PLAYER(eTargetCityState).getTeam(), false);
				GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetPermanentWar(pMinor->getTeam(), false);

				const char* strAllyNameKey = GET_PLAYER(m_eAssignedPlayer).getNameKey();
				
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CS_OTHER_PEACE_MADE");
				strMessage << strTargetNameKey;
				strMessage << strAllyNameKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CS_WAR");
				strSummary << strTargetNameKey;
			}
			else
			{
				GET_TEAM(pMinor->getTeam()).makePeace(GET_PLAYER(eTargetCityState).getTeam(), true, false, pMinor->GetID());
				GET_TEAM(GET_PLAYER(eTargetCityState).getTeam()).makePeace(pMinor->getTeam(), true, false, eTargetCityState);

				pMinor->GetMinorCivAI()->SetPermanentWar(GET_PLAYER(eTargetCityState).getTeam(), false);
				GET_PLAYER(eTargetCityState).GetMinorCivAI()->SetPermanentWar(pMinor->getTeam(), false);

				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CS_WAR");
				strMessage << strTargetNameKey;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CS_WAR");
				strSummary << strTargetNameKey;
			}
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_KILL_CITY_STATE");
			strMessage << strTargetNameKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_KILL_CITY_STATE");
			strSummary << strTargetNameKey;
		}
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_CULTURE");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_FAITH");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_TECHS");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TOURISM");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CONTEST_TOURISM");
		veNamesToShow = GetContestLeaders();
		break;
	}
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_ARCHAEOLOGY");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_ARCHAEOLOGY");
		break;
	}
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_CIRCUMNAVIGATION");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_CIRCUMNAVIGATION");
		break;
	}
	case MINOR_CIV_QUEST_LIBERATION:
	{
		CvPlot* pPlot = GC.getMap().plot(m_iData1, m_iData2);
		const char* strTargetNameKey = pPlot->getPlotCity()->getNameKey();

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_LIBERATION");
		strMessage << strTargetNameKey;
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_LIBERATION");
		strSummary << strTargetNameKey;
		break;
	}
	case MINOR_CIV_QUEST_HORDE:
	{
		if (pMinor->GetMinorCivAI()->IsHordeActive())
		{
			pMinor->GetMinorCivAI()->SetHordeActive(false);
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
			pMinor->GetMinorCivAI()->SetCooldownSpawn(50);

			//cannot do this directly as it likely will kill the player, then we have a problem
			pMinor->GetMinorCivAI()->SetReadyForTakeOver();
		}

		// Update Military AI
		CvAIOperation* pOp = GET_PLAYER(m_eAssignedPlayer).getFirstAIOperationOfType(AI_OPERATION_CITY_DEFENSE, pMinor->GetID());
		if (pOp)
			pOp->SetToAbort(AI_ABORT_NO_TARGET);

		// City Sacked! Don't ignore city-state quests!			
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_HORDE");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_HORDE");
		break;
	}
	case MINOR_CIV_QUEST_REBELLION:
	{
		if (pMinor->GetMinorCivAI()->IsRebellionActive())
		{
			pMinor->GetMinorCivAI()->SetRebellionActive(false);
			pMinor->GetMinorCivAI()->SetRebellion(true);
			pMinor->GetMinorCivAI()->SetTurnsSinceRebellion(0);
			pMinor->GetMinorCivAI()->SetCooldownSpawn(50);
		}

		// Update Military AI
		CvAIOperation* pOp = GET_PLAYER(m_eAssignedPlayer).getFirstAIOperationOfType(AI_OPERATION_CITY_DEFENSE, pMinor->GetID());
		if (pOp)
			pOp->SetToAbort(AI_ABORT_NO_TARGET);

		// City Defects! Don't ignore city-state quests!
		PlayerTypes eAlly = (PlayerTypes) GetPrimaryData();
		if (eAlly != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eAlly).getCivilizationShortDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REBELLION");
			strMessage << strCivKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REBELLION");
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REBELLION_BLANK");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REBELLION");
		}
		break;
	}
	case MINOR_CIV_QUEST_EXPLORE_AREA:
	{
		pMinor->GetMinorCivAI()->SetTargetedAreaID(m_eAssignedPlayer, -1);

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_PLOT_DISCOVERY_FAILED");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_PLOT_DISCOVERY_FAILED_S");
		break;
	}
	case MINOR_CIV_QUEST_COUP:
	{
		pMinor->GetMinorCivAI()->SetCoupAttempted(m_eAssignedPlayer, false);

		PlayerTypes eTarget = (PlayerTypes)GetPrimaryData();
		if (eTarget != NO_PLAYER)
		{
			const char* strCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();

			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_COUP_FAILED");
			strMessage << strCivKey;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COUP_FAILED_S");
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_COUP_FAILED_B");
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_COUP_FAILED_B_S");
		}
		break;
	}
	default:
	{
		// General "Quest Expired" catch statement
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_OTHER");
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_OTHER");
		break;
	}
	}

	strMessage << pMinor->getNameKey();
	strSummary << pMinor->getNameKey();

	CvString sMessage = strMessage.toUTF8();
	CvString sSummary = strSummary.toUTF8();

	// This quest involved multiple winners, so grab their names for the notification
	if (veNamesToShow.size() > 0)
		sMessage += pMinor->GetMinorCivAI()->GetNamesListAsString(veNamesToShow);

	pMinor->GetMinorCivAI()->AddQuestNotification(sMessage, sSummary, m_eAssignedPlayer);
	return true;
}

///
template<typename MinorCivQuest, typename Visitor>
void CvMinorCivQuest::Serialize(MinorCivQuest& minorCivQuest, Visitor& visitor)
{
	visitor(minorCivQuest.m_eMinor);
	visitor(minorCivQuest.m_eAssignedPlayer);
	visitor(minorCivQuest.m_eType);
	visitor(minorCivQuest.m_iStartTurn);
	visitor(minorCivQuest.m_iData1);
	visitor(minorCivQuest.m_iData2);
	visitor(minorCivQuest.m_iData3);
	visitor(minorCivQuest.m_iInfluence);
	visitor(minorCivQuest.m_iDisabledInfluence);
	visitor(minorCivQuest.m_iGold);
	visitor(minorCivQuest.m_iScience);
	visitor(minorCivQuest.m_iCulture);
	visitor(minorCivQuest.m_iFaith);
	visitor(minorCivQuest.m_iGoldenAgePoints);
	visitor(minorCivQuest.m_iFood);
	visitor(minorCivQuest.m_iProduction);
	visitor(minorCivQuest.m_iTourism);
	visitor(minorCivQuest.m_iHappiness);
	visitor(minorCivQuest.m_iGP);
	visitor(minorCivQuest.m_iGPGlobal);
	visitor(minorCivQuest.m_iGeneralPoints);
	visitor(minorCivQuest.m_iAdmiralPoints);
	visitor(minorCivQuest.m_iExperience);
	visitor(minorCivQuest.m_bPartialQuest);
	visitor(minorCivQuest.m_bHandled);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvMinorCivQuest& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvMinorCivQuest::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvMinorCivQuest& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvMinorCivQuest::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//======================================================================================================
//					CvMinorCivIncomingUnitGift
//======================================================================================================

///
CvMinorCivIncomingUnitGift::CvMinorCivIncomingUnitGift()
	: m_iArrivalCountdown(-1)
	, m_eUnitType(NO_UNIT)
	, m_iFromX(0)
	, m_iFromY(0)
	, m_eOriginalOwner(NO_PLAYER)
	, m_eGiftedByPlayer(NO_PLAYER)
	, m_iGameTurnCreated(0)
	, m_bPromotedFromGoody(false)
	, m_iExperienceTimes100(0)
	, m_iLevel(0)
	, m_iOriginCity(-1)
	, m_eLeaderUnitType(NO_UNIT)
	, m_iNumGoodyHutsPopped(0)
{
}

///
CvMinorCivIncomingUnitGift::CvMinorCivIncomingUnitGift(const CvUnit& srcUnit, int iArriveInTurns, PlayerTypes eFromPlayer)
{
	init(srcUnit, iArriveInTurns, eFromPlayer);
}

///
void CvMinorCivIncomingUnitGift::init(const CvUnit& srcUnit, int iArriveInTurns, PlayerTypes eFromPlayer)
{
	reset();

	setArrivalCountdown(iArriveInTurns);
	setUnitType(srcUnit.getUnitType());
	setFromXY(srcUnit.getX(), srcUnit.getY());
	setOriginalOwner(srcUnit.GetOriginalOwner());
	setGiftedByPlayer(eFromPlayer);
	setGameTurnCreated(srcUnit.getGameTurnCreated());
	for (int i = 0; i < GC.getNumPromotionInfos(); ++i)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(i);
		if (srcUnit.isHasPromotion(ePromotion) && CvUnit::IsRetainablePromotion(ePromotion))
		{
			setHasPromotion(ePromotion, true);
			setPromotionDuration(ePromotion, srcUnit.getPromotionDuration(ePromotion));
			setTurnPromotionGained(ePromotion, srcUnit.getTurnPromotionGained(ePromotion));
		}
	}
	setHasBeenPromotedFromGoody(srcUnit.IsHasBeenPromotedFromGoody());
	setExperienceTimes100(srcUnit.getExperienceTimes100());
	setLevel(srcUnit.getLevel());
	const CvCity* pOriginCity = srcUnit.getOriginCity();
	setOriginCity(pOriginCity ? pOriginCity->GetID() : -1);
	setLeaderUnitType(srcUnit.getLeaderUnitType());
	setNumGoodyHutsPopped(srcUnit.GetNumGoodyHutsPopped());
	setName(srcUnit.getNameNoDesc());
}

///
int CvMinorCivIncomingUnitGift::getArrivalCountdown() const
{
	return m_iArrivalCountdown;
}

///
UnitTypes CvMinorCivIncomingUnitGift::getUnitType() const
{
	return m_eUnitType;
}

///
CvPlot* CvMinorCivIncomingUnitGift::getFromPlot() const
{
	return GC.getMap().plotCheckInvalid(m_iFromX, m_iFromY);
}

///
PlayerTypes CvMinorCivIncomingUnitGift::getOriginalOwner() const
{
	return m_eOriginalOwner;
}

///
PlayerTypes CvMinorCivIncomingUnitGift::getGiftedByPlayer() const
{
	return m_eGiftedByPlayer;
}

///
int CvMinorCivIncomingUnitGift::getGameTurnCreated() const
{
	return m_iGameTurnCreated;
}

///
bool CvMinorCivIncomingUnitGift::isHasPromotion(PromotionTypes ePromotion) const
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	return m_HasPromotions.GetBit(static_cast<uint>(ePromotion));
}

///
int CvMinorCivIncomingUnitGift::getPromotionDuration(PromotionTypes ePromotion) const
{
	const std::map<PromotionTypes, int>& m_map = m_PromotionDuration;
	std::map<PromotionTypes, int>::const_iterator it = m_map.find(ePromotion);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

///
int CvMinorCivIncomingUnitGift::getTurnPromotionGained(PromotionTypes ePromotion) const
{
	const std::map<PromotionTypes, int>& m_map = m_TurnPromotionGained;
	std::map<PromotionTypes, int>::const_iterator it = m_map.find(ePromotion);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

///
bool CvMinorCivIncomingUnitGift::isHasBeenPromotedFromGoody() const
{
	return m_bPromotedFromGoody;
}

///
int CvMinorCivIncomingUnitGift::getExperienceTimes100() const
{
	return m_iExperienceTimes100;
}

///
int CvMinorCivIncomingUnitGift::getLevel() const
{
	return m_iLevel;
}

///
int CvMinorCivIncomingUnitGift::getOriginCity() const
{
	return m_iOriginCity;
}

///
UnitTypes CvMinorCivIncomingUnitGift::getLeaderUnitType() const
{
	return m_eLeaderUnitType;
}

///
int CvMinorCivIncomingUnitGift::getNumGoodyHutsPopped() const
{
	return m_iNumGoodyHutsPopped;
}

///
const CvString& CvMinorCivIncomingUnitGift::getName() const
{
	return m_strName;
}

///
void CvMinorCivIncomingUnitGift::setArrivalCountdown(int iNewCountdown)
{
	m_iArrivalCountdown = iNewCountdown;
}

///
void CvMinorCivIncomingUnitGift::changeArrivalCountdown(int iChangeCountdown)
{
	CvAssert(m_iArrivalCountdown != -1);
	m_iArrivalCountdown = std::max(0, m_iArrivalCountdown + iChangeCountdown);
}

///
void CvMinorCivIncomingUnitGift::setUnitType(UnitTypes eNewUnitType)
{
	m_eUnitType = eNewUnitType;
}

///
void CvMinorCivIncomingUnitGift::setFromXY(int iFromX, int iFromY)
{
	m_iFromX = iFromX;
	m_iFromY = iFromY;
}

///
void CvMinorCivIncomingUnitGift::setOriginalOwner(PlayerTypes eNewOriginalOwner)
{
	m_eOriginalOwner = eNewOriginalOwner;
}

///
void CvMinorCivIncomingUnitGift::setGiftedByPlayer(PlayerTypes ePlayer)
{
	m_eGiftedByPlayer = ePlayer;
}

///
void CvMinorCivIncomingUnitGift::setGameTurnCreated(int iNewValue)
{
	m_iGameTurnCreated = iNewValue;
}

///
void CvMinorCivIncomingUnitGift::setHasPromotion(PromotionTypes ePromotion, bool bNewValue)
{
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if (ePromotion >= 0 && ePromotion < GC.getNumPromotionInfos())
	{
		m_HasPromotions.SetBit(static_cast<uint>(ePromotion), bNewValue);
	}
}

///
void CvMinorCivIncomingUnitGift::setPromotionDuration(PromotionTypes ePromotion, int iNewValue)
{
	std::map<PromotionTypes, int>& m_map = m_PromotionDuration;
	if (iNewValue > 0)
		m_map[ePromotion] = iNewValue;
	else
		m_map.erase(ePromotion);
}

///
void CvMinorCivIncomingUnitGift::setTurnPromotionGained(PromotionTypes ePromotion, int iNewValue)
{
	std::map<PromotionTypes, int>& m_map = m_TurnPromotionGained;
	if (iNewValue > 0)
		m_map[ePromotion] = iNewValue;
	else
		m_map.erase(ePromotion);
}

///
void CvMinorCivIncomingUnitGift::setHasBeenPromotedFromGoody(bool bPromotedFromGoody)
{
	m_bPromotedFromGoody = bPromotedFromGoody;
}

///
void CvMinorCivIncomingUnitGift::setExperienceTimes100(int iNewValueTimes100)
{
	m_iExperienceTimes100 = iNewValueTimes100;
}

///
void CvMinorCivIncomingUnitGift::setLevel(int iNewLevel)
{
	m_iLevel = iNewLevel;
}

///
void CvMinorCivIncomingUnitGift::setOriginCity(int iNewOriginCity)
{
	m_iOriginCity = iNewOriginCity;
}

///
void CvMinorCivIncomingUnitGift::setLeaderUnitType(UnitTypes eNewLeaderUnitType)
{
	m_eLeaderUnitType = eNewLeaderUnitType;
}

///
void CvMinorCivIncomingUnitGift::setNumGoodyHutsPopped(int iNewNumGoodyHutsPopped)
{
	m_iNumGoodyHutsPopped = iNewNumGoodyHutsPopped;
}

///
void CvMinorCivIncomingUnitGift::setName(const CvString& newName)
{
	m_strName = newName;
}

///
bool CvMinorCivIncomingUnitGift::hasIncomingUnit() const
{
	return m_iArrivalCountdown != -1;
}

///
void CvMinorCivIncomingUnitGift::applyToUnit(PlayerTypes eFromPlayer, CvUnit& destUnit) const
{
	destUnit.SetOriginalOwner(getOriginalOwner());
	destUnit.SetGiftedByPlayer(getGiftedByPlayer());
	destUnit.setGameTurnCreated(getGameTurnCreated());
	for (int i = 0; i < GC.getNumPromotionInfos(); ++i)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(i);
		if (isHasPromotion(ePromotion))
		{
			CvAssert(CvUnit::IsRetainablePromotion(ePromotion));
			destUnit.setHasPromotion(ePromotion, true);
			destUnit.SetPromotionDuration(ePromotion, getPromotionDuration(ePromotion));
			destUnit.SetTurnPromotionGained(ePromotion, getTurnPromotionGained(ePromotion));
		}
	}
	destUnit.SetBeenPromotedFromGoody(isHasBeenPromotedFromGoody());
	destUnit.setExperienceTimes100(CvUnit::CalcExperienceTimes100ForConvert(eFromPlayer, destUnit.getOwner(), getExperienceTimes100()));
	destUnit.setLevel(getLevel());
	destUnit.setOriginCity(getOriginCity());
	destUnit.setLeaderUnitType(getLeaderUnitType());
	destUnit.SetNumGoodyHutsPopped(getNumGoodyHutsPopped());
	destUnit.setName(getName());
}

///
void CvMinorCivIncomingUnitGift::reset()
{
	*this = CvMinorCivIncomingUnitGift();
}

///
template<typename MinorCivIncomingUnitGift, typename Visitor>
void CvMinorCivIncomingUnitGift::Serialize(MinorCivIncomingUnitGift& minorCivIncomingUnitGift, Visitor& visitor)
{
	visitor(minorCivIncomingUnitGift.m_iArrivalCountdown);

	// All fields are only serialized if the countdown is not -1.
	// This is safe because the associated data is only meaningful when the countdown is not -1.
	if (minorCivIncomingUnitGift.m_iArrivalCountdown != -1)
	{
		visitor(minorCivIncomingUnitGift.m_eUnitType);
		visitor(minorCivIncomingUnitGift.m_iFromX);
		visitor(minorCivIncomingUnitGift.m_iFromY);
		visitor(minorCivIncomingUnitGift.m_eOriginalOwner);
		visitor(minorCivIncomingUnitGift.m_eGiftedByPlayer);
		visitor(minorCivIncomingUnitGift.m_iGameTurnCreated);
		visitor(minorCivIncomingUnitGift.m_HasPromotions);
		visitor(minorCivIncomingUnitGift.m_PromotionDuration);
		visitor(minorCivIncomingUnitGift.m_TurnPromotionGained);
		visitor(minorCivIncomingUnitGift.m_bPromotedFromGoody);
		visitor(minorCivIncomingUnitGift.m_iExperienceTimes100);
		visitor(minorCivIncomingUnitGift.m_iLevel);
		visitor(minorCivIncomingUnitGift.m_iOriginCity);
		visitor(minorCivIncomingUnitGift.m_eLeaderUnitType);
		visitor(minorCivIncomingUnitGift.m_iNumGoodyHutsPopped);
		visitor(minorCivIncomingUnitGift.m_strName);
	}
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvMinorCivIncomingUnitGift& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvMinorCivIncomingUnitGift::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvMinorCivIncomingUnitGift& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvMinorCivIncomingUnitGift::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//======================================================================================================
//					CvMinorCivAI
//======================================================================================================
CvMinorCivAI::CvMinorCivAI()
{
}
//------------------------------------------------------------------------------
CvMinorCivAI::~CvMinorCivAI(void)
{
	Uninit();
}

/// Initialize
void CvMinorCivAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
#if defined(MOD_GLOBAL_MAX_MAJOR_CIVS)
	m_minorCivType = GetMinorCivType();
#else
	m_minorCivType = CvPreGame::minorCivType(m_pPlayer->GetID());
#endif

	Reset();
}

/// Deallocate memory created in initialize
void CvMinorCivAI::Uninit()
{
}

/// Reset AIStrategy status array to all false
void CvMinorCivAI::Reset()
{
	m_ePersonality = NO_MINOR_CIV_PERSONALITY_TYPE;
	m_eStatus = NO_MINOR_CIV_STATUS_TYPE;
	m_eUniqueUnit = NO_UNIT;
	m_eBullyUnit = NO_UNITCLASS;

	m_iTurnsSinceThreatenedByBarbarians = -1;
	m_iGlobalQuestCountdown = -1;

	m_eAlly = NO_PLAYER;
	m_iTurnAllied = -1;
	m_eMajorBoughtOutBy = NO_PLAYER;
	m_iNumThreateningBarbarians = 0;
	m_bAllowMajorsToIntrude = false;

	m_bDisableNotifications = false;
	m_iBullyPlotsBuilt = 0;
	m_bullyRelevantPlots.clear();

	m_bIsRebellion = false;
	m_iTurnsSinceRebellion = 0;
	m_bIsRebellionActive = false;
	m_bIsHordeActive = false;
	m_iCooldownSpawn = 0;
	m_ePermanentAlly = NO_PLAYER;
	m_bNoAlly = false;
	m_iCoup = 0;
	m_iTakeoverTurn = 0;
	m_iTurnLiberated = 0;

	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_abRouteConnectionEstablished[iI] = false;
		m_aiFriendshipWithMajorTimes100[iI] = 0;
		m_aiQuestCountdown[iI] = -1;
		m_aiUnitSpawnCounter[iI] = -1;
		m_aiNumUnitsGifted[iI] = 0;
		m_aiNumGoldGifted[iI] = 0;
		m_aiTurnLastBullied[iI] = -1;
		m_aiTurnLastPledged[iI] = -1;
		m_aiTurnLastBrokePledge[iI] = -1;
		m_abUnitSpawningDisabled[iI] = false;
		m_abQuestInfluenceDisabled[iI] = false;
		m_abEverFriends[iI] = false;
		m_abFriends[iI] = false;
		m_abPledgeToProtect[iI] = false;
		m_abIsMarried[iI] = false;
		m_abSiphoned[iI] = false;
		m_abCoupAttempted[iI] = false;
		m_abSentUnitForQuest[iI] = false;
		m_aiAssignedPlotAreaID[iI] = -1;
		m_aiTargetedCityX[iI] = -1;
		m_aiTargetedCityY[iI] = -1;
		m_aiTurnsSincePtPWarning[iI] = -1;
		m_IncomingUnitGifts[iI].reset();
		m_aiRiggingCoupChanceIncrease[iI] = 0;
		m_aiRestingPointChange[iI] = 0;
	}

	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		m_aiTurnLastAttacked[iI] = -1;
		m_abIgnoreJerk[iI] = false;
		m_abPermanentWar[iI] = false;
		m_abWaryOfTeam[iI] = false;
	}

	ResetQuestList();

	if(GetPlayer()->isMinorCiv())
	{
		CvPlot* pLoopPlot = NULL;
		TeamTypes eTeam = GetPlayer()->getTeam();
		int iNumPlotsInEntireWorld = GC.getMap().numPlots();
		for(int iLoopPlot = 0; iLoopPlot < iNumPlotsInEntireWorld; iLoopPlot++)
		{
			pLoopPlot = GC.getMap().plotByIndexUnchecked(iLoopPlot);
			if(pLoopPlot)
				pLoopPlot->setRevealed(eTeam, true);
		}
	}

}

void CvMinorCivAI::ResetQuestList()
{
	//antonjs: todo: this is called when PlayerTypes is -1, why? does it cause inefficiency or unused memory?

	for(uint iPlayerEntry = 0; iPlayerEntry < m_QuestsGiven.size(); iPlayerEntry++)
	{
		m_QuestsGiven[iPlayerEntry].clear();
	}

	m_QuestsGiven.clear();

	for(int iNewEntry = 0; iNewEntry < MAX_MAJOR_CIVS; iNewEntry++)
	{
		QuestListForPlayer vQuestList;
		m_QuestsGiven.push_back(vQuestList);
	}
}

///
template<typename MinorCivAI, typename Visitor>
void CvMinorCivAI::Serialize(MinorCivAI& minorCivAI, Visitor& visitor)
{
	visitor(minorCivAI.m_ePersonality);
	visitor(minorCivAI.m_eStatus);
	visitor(minorCivAI.m_eUniqueUnit);
	visitor(minorCivAI.m_eBullyUnit);

	visitor(minorCivAI.m_iTurnsSinceThreatenedByBarbarians);

	visitor(minorCivAI.m_iGlobalQuestCountdown);

	visitor(minorCivAI.m_eAlly);

	visitor(minorCivAI.m_iTurnAllied);

	visitor(minorCivAI.m_eMajorBoughtOutBy);
	visitor(minorCivAI.m_iNumThreateningBarbarians);
	visitor(minorCivAI.m_bAllowMajorsToIntrude);

	visitor(minorCivAI.m_abRouteConnectionEstablished);

	visitor(minorCivAI.m_aiFriendshipWithMajorTimes100);

	visitor(minorCivAI.m_aiQuestCountdown);
	visitor(minorCivAI.m_aiUnitSpawnCounter);

	visitor(minorCivAI.m_aiNumUnitsGifted);

	visitor(minorCivAI.m_aiNumGoldGifted);

	visitor(minorCivAI.m_aiTurnLastBullied);

	visitor(minorCivAI.m_aiTurnLastPledged);
	visitor(minorCivAI.m_aiTurnLastBrokePledge);

	visitor(minorCivAI.m_abUnitSpawningDisabled);
	visitor(minorCivAI.m_abQuestInfluenceDisabled);
	visitor(minorCivAI.m_abEverFriends);
	visitor(minorCivAI.m_abFriends);

	visitor(minorCivAI.m_abPledgeToProtect);

	visitor(minorCivAI.m_abPermanentWar);

	visitor(minorCivAI.m_abWaryOfTeam);
	visitor(minorCivAI.m_aiRiggingCoupChanceIncrease);
	visitor(minorCivAI.m_aiRestingPointChange);

	visitor(minorCivAI.m_bIsRebellion);
	visitor(minorCivAI.m_iTurnsSinceRebellion);
	visitor(minorCivAI.m_bIsRebellionActive);
	visitor(minorCivAI.m_bIsHordeActive);
	visitor(minorCivAI.m_iCooldownSpawn);
	visitor(minorCivAI.m_aiTurnLastAttacked);
	visitor(minorCivAI.m_abIgnoreJerk);
	visitor(minorCivAI.m_abIsMarried);
	visitor(minorCivAI.m_ePermanentAlly);
	visitor(minorCivAI.m_bNoAlly);
	visitor(minorCivAI.m_iCoup);
	visitor(minorCivAI.m_abSiphoned);
	visitor(minorCivAI.m_abCoupAttempted);
	visitor(minorCivAI.m_iTurnLiberated);
	visitor(minorCivAI.m_aiAssignedPlotAreaID);
	visitor(minorCivAI.m_aiTargetedCityX);
	visitor(minorCivAI.m_aiTargetedCityY);
	visitor(minorCivAI.m_aiTurnsSincePtPWarning);

	visitor(minorCivAI.m_IncomingUnitGifts);

	visitor(minorCivAI.m_QuestsGiven);
	visitor(minorCivAI.m_bDisableNotifications);
}

/// Serialization read
void CvMinorCivAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvMinorCivAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvMinorCivAI& minorCivAI)
{
	minorCivAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvMinorCivAI& minorCivAI)
{
	minorCivAI.Write(stream);
	return stream;
}

/// Pick the minor civ's personality and any special traits (ie. unique unit for Militaristic)
void CvMinorCivAI::DoPickInitialItems()
{
	// Personality
	DoPickPersonality();

	// Unique unit
	DoPickUniqueUnit();
}

/// Returns the Player object this MinorCivAI is associated with
CvPlayer* CvMinorCivAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns the MinorCivType this Minor is playing as (e.g. Scotland, Switzerland, etc.)
MinorCivTypes CvMinorCivAI::GetMinorCivType() const
{
	//	return m_minorCivType;
	return CvPreGame::minorCivType(m_pPlayer->GetID());
}

/// What is the personality of this Minor
MinorCivPersonalityTypes CvMinorCivAI::GetPersonality() const
{
	return m_ePersonality;
}
#if defined(MOD_BALANCE_CORE)
UnitClassTypes CvMinorCivAI::GetBullyUnit() const
{
	return m_eBullyUnit;
}
void CvMinorCivAI::SetBullyUnit(UnitClassTypes eUnitClassType)
{
	if(eUnitClassType == NO_UNITCLASS)
	{
		CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GetMinorCivType());
		if(pkMinorCivInfo)
		{
			if((UnitClassTypes)pkMinorCivInfo->GetBullyUnit() != NO_UNITCLASS)
			{
				m_eBullyUnit = (UnitClassTypes)pkMinorCivInfo->GetBullyUnit();
			}
			else
			{
				m_eBullyUnit = (UnitClassTypes)GetPlayer()->GetSpecificUnitType("UNITCLASS_WORKER");
			}
		}
	}
	else
	{
		 m_eBullyUnit = eUnitClassType;
	}
}
#endif
/// Set a Personality for this minor
void CvMinorCivAI::SetPersonality(MinorCivPersonalityTypes ePersonality)
{
	m_ePersonality = ePersonality;
}

/// Picks a random Personality for this minor
void CvMinorCivAI::DoPickPersonality()
{

	FlavorTypes eFlavorCityDefense = NO_FLAVOR;
	FlavorTypes eFlavorDefense = NO_FLAVOR;
	FlavorTypes eFlavorOffense = NO_FLAVOR;
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_CITY_DEFENSE")
		{
			eFlavorCityDefense = (FlavorTypes)iFlavorLoop;
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_DEFENSE")
		{
			eFlavorDefense = (FlavorTypes)iFlavorLoop;
		}
		if(GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			eFlavorOffense = (FlavorTypes)iFlavorLoop;
		}
	}

	CvAssert(eFlavorCityDefense != NO_FLAVOR);
	CvAssert(eFlavorDefense != NO_FLAVOR);
	CvAssert(eFlavorOffense != NO_FLAVOR);

	CvFlavorManager* pFlavorManager = m_pPlayer->GetFlavorManager();
	CvEnumMap<FlavorTypes, int>& pFlavors = pFlavorManager->GetAllPersonalityFlavors();

	MinorCivPersonalityTypes eRandPersonality = (MinorCivPersonalityTypes)GC.getGame().getSmallFakeRandNum(NUM_MINOR_CIV_PERSONALITY_TYPES-1, m_pPlayer->GetID());
	if (eRandPersonality == NO_MINOR_CIV_PERSONALITY_TYPE)
		eRandPersonality = MINOR_CIV_PERSONALITY_FRIENDLY;

	SetPersonality(eRandPersonality);
#if defined(MOD_BALANCE_CORE)
	SetBullyUnit();
#endif

	// Random seed to ensure the fake RNG doesn't return the same value repeatedly
	int iSeed = 0;

	switch (eRandPersonality)
	{
	case MINOR_CIV_PERSONALITY_FRIENDLY:
	case MINOR_CIV_PERSONALITY_HOSTILE:
		pFlavors[eFlavorCityDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorCityDefense], 2, 0, 10, iSeed);
		pFlavors[eFlavorDefense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorDefense], 2, 0, 10, iSeed);
		pFlavors[eFlavorOffense] = pFlavorManager->GetAdjustedValue(pFlavors[eFlavorOffense], 2, 0, 10, iSeed);
		pFlavorManager->ResetToBasePersonality();
		break;
	default:
		break; // TODO: Why do other personalities not have their flavors modified?
	}
}

/// What is this civ's trait?
MinorCivTraitTypes CvMinorCivAI::GetTrait() const
{
	CvMinorCivInfo* pkMinorCivInfo = GC.getMinorCivInfo(GetMinorCivType());
	if(pkMinorCivInfo)
	{
		return (MinorCivTraitTypes) pkMinorCivInfo->GetMinorCivTrait();
	}

	return NO_MINOR_CIV_TRAIT_TYPE;
}

/// Does this civ have a unique unit? (only for Militaristic)
bool CvMinorCivAI::IsHasUniqueUnit() const
{
	return (m_eUniqueUnit != NO_UNIT);
}

/// What is this civ's unique unit? (only for Militaristic)
UnitTypes CvMinorCivAI::GetUniqueUnit() const
{
	return m_eUniqueUnit;
}

/// Override picked unique unit (only for Militaristic)
void CvMinorCivAI::SetUniqueUnit(UnitTypes eUnit)
{
	CvAssertMsg(GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC, "Setting a unique unit for a non-Militaristic City-State.  It will never be spawned without additional scripting.");
	CvUnitEntry* pInfo = GC.getUnitInfo(eUnit);
	CvAssertMsg(pInfo, "Setting a unique unit for a City-State that the game core could not identify. Please send Anton your save file and version.");
	if (pInfo)
	{
		m_eUniqueUnit = eUnit;
	}
}

/// Picks a unique unit for Militaristic city-states
void CvMinorCivAI::DoPickUniqueUnit()
{
	m_eUniqueUnit = NO_UNIT;
	if (GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		int iCoastal = 0;
		int iPlayers = 0;
		bool bCoastal = false;
		if (GetPlayer()->getStartingPlot() != NULL)
		{
			if (GetPlayer()->getStartingPlot()->isCoastalLand(/*10*/ GD_INT_GET(MIN_WATER_SIZE_FOR_OCEAN)))
			{
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
					if (GET_PLAYER(ePlayer).isAlive())
					{
						if (GET_PLAYER(ePlayer).getStartingPlot() != NULL && GET_PLAYER(ePlayer).getStartingPlot()->isCoastalLand(/*10*/ GD_INT_GET(MIN_WATER_SIZE_FOR_OCEAN)))
						{
							iCoastal++;
						}
						iPlayers++;
					}
				}
				//If the majority of major civ players start on the coast, we should give out boats too.
				bCoastal = (iCoastal > iPlayers / 2);
			}

			m_eUniqueUnit = GC.getGame().GetRandomUniqueUnitType(/*bIncludeCivsInGame*/ false, /*bIncludeStartEraUnits*/ true, /*bIncludeOldEras*/ false, /*bIncludeRanged*/ true, 
																	bCoastal, GetPlayer()->getStartingPlot()->getX(), GetPlayer()->getStartingPlot()->getY());
		}
	}
}


int CvMinorCivAI::GetQuestRewardModifier(PlayerTypes ePlayer)
{
	if (!GET_PLAYER(ePlayer).isMajorCiv() || !GET_PLAYER(ePlayer).isAlive() || !GetPlayer()->isAlive())
		return 0;

	// Calculate this player's modifier for the base value
	int iBaseModifier = 100;
	iBaseModifier += GET_PLAYER(ePlayer).GetIncreasedQuestInfluence();
	iBaseModifier += IsProtectedByMajor(ePlayer) ? /*0 in CP, 15 in VP*/ GD_INT_GET(BALANCE_INFLUENCE_BOOST_PROTECTION_MINOR) : 0;

	MinorCivPersonalityTypes ePersonality = GetPersonality();
	if (ePersonality == MINOR_CIV_PERSONALITY_FRIENDLY)
		iBaseModifier += /*0 in CP, 25 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_FRIENDLY);
	else if (ePersonality == MINOR_CIV_PERSONALITY_HOSTILE)
		iBaseModifier += /*0 in CP, -25 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_REWARD_HOSTILE);

	if (IsSameReligionAsMajor(ePlayer))
	{
		int iFriendshipMod = GET_PLAYER(ePlayer).GetReligions()->GetCityStateInfluenceModifier(ePlayer);
		iBaseModifier *= 100 + iFriendshipMod;
		iBaseModifier /= 100;
	}

	return max(iBaseModifier, 0);
}


// ******************************
// Main functions
// ******************************


/// Processed every turn
void CvMinorCivAI::DoTurn()
{
	if(GetPlayer()->isMinorCiv())
	{
		DoTurnStatus();

		doIncomingUnitGifts();

		DoElection();
		DoFriendship();

		DoTestThreatenedAnnouncement();
		DoTestProxyWarAnnouncement();

		DoTurnQuests();

		DoUnitSpawnTurn();

		DoIntrusion();

		if (MOD_BALANCE_VP) 
		{
			//Test to keep quests from firing over and over if ended.
			if(GetCooldownSpawn() > 0)
			{
				ChangeCooldownSpawn(-1);
			}
			if(IsRebellion())
			{
				DoDefection();
			}
		}
		if(GetPermanentAlly() != NO_PLAYER)
		{
			if(GetPermanentAlly() != GetAlly())
			{
				SetAlly(GetPermanentAlly());
			}
		}
		if(IsNoAlly() && GetAlly() != NO_PLAYER)
		{
			SetAlly(NO_PLAYER);
		}

		if(GetCoupCooldown() > 0)
		{
			ChangeCoupCooldown(-1);
			if(GetCoupCooldown() == 0)
			{
				if(GetPlayer()->getCapitalCity() != NULL)
				{
					PlayerTypes ePlayer;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						ePlayer = (PlayerTypes) iPlayerLoop;

						if(GET_PLAYER(ePlayer).isAlive())
						{
							if(!IsHasMetPlayer(ePlayer))
							{
								continue;
							}
							CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
							if(pNotifications)
							{
								Localization::String strSummary;
								Localization::String strNotification;
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_NOW_POSSIBLE_S");
								strSummary << GetPlayer()->getCapitalCity()->getNameKey();
								strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_NOW_POSSIBLE");
								strNotification << GetPlayer()->getCapitalCity()->getNameKey();
								pNotifications->Add(NOTIFICATION_SPY_STAGE_COUP_SUCCESS, strNotification.toUTF8(), strSummary.toUTF8(), GetPlayer()->getCapitalCity()->getX(), GetPlayer()->getCapitalCity()->getY(), -1);
							}
						}
					}
				}
			}
		}
		//Let's see if we can make peace
		DoTestEndSkirmishes(NO_PLAYER);

		if (MOD_BALANCE_CORE_MINORS) 
		{
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if (GET_PLAYER(eLoopPlayer).isMajorCiv() && GET_PLAYER(eLoopPlayer).isAlive())
				{
					TeamTypes eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

					if (IsFriends(eLoopPlayer) && GetJerkTurnsRemaining(eLoopTeam) > 0)
					{
						SetIgnoreJerk(eLoopTeam, true);
					}

					TestChangeProtectionFromMajor(eLoopPlayer);
				}
			}
		}
	}
}


/// Minor is now dead or alive (haha, get it?)
void CvMinorCivAI::DoChangeAliveStatus(bool bAlive)
{
	if (!bAlive)
	{
		// Final check for quests
		DoTestActiveQuests(/*bTestComplete*/ true, /*bTestObsolete*/ true);
		DoQuestsCleanup();

		std::vector<int> vNewInfluence;
		for (int i = 0; i < MAX_MAJOR_CIVS; ++i)
		{
			PlayerTypes e = (PlayerTypes)i;

			// Cancel quests and PtPs
			DoChangeProtectionFromMajor(e, false);
			ResetRiggingCoupChanceIncrease(e);

			// Return all incoming unit gifts.
			returnIncomingUnitGift(e);

			// Calculate new influence levels (don't set here, since that could create a false temporary ally)
			int iOldInfluence = GetBaseFriendshipWithMajor(e);
			int iNewInfluence = iOldInfluence;
			if (IsFriendshipAboveAlliesThreshold(e, iOldInfluence))
			{
				iNewInfluence = /*45*/ GD_INT_GET(FRIENDSHIP_ALLIES_ON_DEATH);
			}
			else if (IsFriendshipAboveFriendsThreshold(e, iOldInfluence))
			{
				iNewInfluence = /*15*/ GD_INT_GET(FRIENDSHIP_FRIENDS_ON_DEATH);
			}
			else if (iOldInfluence > /*0*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_NEUTRAL))
			{
				iNewInfluence = /*0*/ GD_INT_GET(FRIENDSHIP_NEUTRAL_ON_DEATH);
			}
			vNewInfluence.push_back(iNewInfluence);
		}
		if (MOD_BALANCE_CORE_MINORS) 
		{
			SetTurnsSinceRebellion(0);
		}

		// Set new influence values
		SetDisableNotifications(true);
		for (unsigned int i = 0; i < vNewInfluence.size(); ++i)
		{
			PlayerTypes e = (PlayerTypes)i;
			// special workaround to allow status changes despite minor already being dead
			DoFriendshipChangeEffects(e, GetEffectiveFriendshipWithMajor(e), vNewInfluence.at(i), /*bFromQuest*/ false, /*bIgnoreMinorDeath*/ true);
			SetFriendshipWithMajor(e, vNewInfluence.at(i));
		}
		SetDisableNotifications(false);
	}

	// Apply or Remove any active bonuses
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		bool bFriends = false;
		bool bAllies = false;
		if (IsFriendshipAboveFriendsThreshold(ePlayer, GetEffectiveFriendshipWithMajor(ePlayer)))
		{
			bFriends = true;
		}
		if(GetAlly() == ePlayer)
		{
			CvAssertMsg(bAlive, "A Minor about to die still has an Ally, when it should have none.");
			bAllies = true;
		}
		if(bFriends || bAllies)
		{
			DoSetBonus(ePlayer, bAlive, bFriends, bAllies);
		}
	}

	// Death - Reset the cached ally and barbarian threat counter
	if (!bAlive)
	{
		bool bHasAlly = GetAlly() != NO_PLAYER;
		CvAssertMsg(!bHasAlly, "A Minor about to die still has an Ally, when it should have none.");
		if(bHasAlly)
		{
			SetAlly(NO_PLAYER);
		}

		SetTurnsSinceThreatenedAnnouncement(-1);
	}
}


/// First contact
void CvMinorCivAI::DoFirstContactWithMajor(TeamTypes eTeam, bool bSuppressMessages)
{
	// This guy's a warmonger or at war with our ally, so we DoW him
	if (IsPeaceBlocked(eTeam))
	{
		GET_TEAM(GetPlayer()->getTeam()).declareWar(eTeam, true, GetPlayer()->GetID());
	}
	// Normal diplo
	else
	{
		// Ideally we want the actual meeting player, but we'll have to settle for the lead player of the team
		// most of the time they will be one and the same
		PlayerTypes eMeetingPlayer = GET_TEAM(eTeam).getLeaderID();
		if (!GET_TEAM(eTeam).isMinorCiv() && !GET_TEAM(eTeam).isBarbarian() && eMeetingPlayer != NO_PLAYER && GET_PLAYER(eMeetingPlayer).isAlive())
		{
			MinorCivTraitTypes eTrait = GetTrait();
			MinorCivPersonalityTypes eRealPersonality = GetPersonality();
			MinorCivPersonalityTypes eFakePersonality = eRealPersonality;

			int iFriendshipBoost = 0;
			int iCultureGift = 0;
			int iFoodGift = 0;
			int iUnitGift = 0;

			int iGift = 0;
			char const* szTxtKeySuffix = "UNKNOWN";

			int iGoldGift = 0;
			int iFaithGift = 0;
			bool bFirstMajorCiv = false;

			// If this guy has been mean then no Gold gifts
			if (!GET_TEAM(eTeam).IsMinorCivAggressor())
			{
				// Gift depends on the type of city state met
				if (MOD_GLOBAL_CS_GIFTS) 
				{
					bFirstMajorCiv = (GET_TEAM(GetPlayer()->getTeam()).getHasMetCivCount(true) == 0);
					iFriendshipBoost = /*5*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_BONUS_FRIENDSHIP); // All CS types give an initial friendship boost

					if (eTrait == MINOR_CIV_TRAIT_CULTURED) {
						iCultureGift = /*5*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_BONUS_CULTURE);
						szTxtKeySuffix = "CULTURE";
					} else if (eTrait == MINOR_CIV_TRAIT_RELIGIOUS) {
						iFaithGift = /*5*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_BONUS_FAITH);
						szTxtKeySuffix = "FAITH";
					} else if (eTrait == MINOR_CIV_TRAIT_MERCANTILE) {
						iGoldGift = /*25*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_BONUS_GOLD);
						szTxtKeySuffix = "GOLD";
					} else if (eTrait == MINOR_CIV_TRAIT_MARITIME) {
						iFoodGift = /*10*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_BONUS_FOOD);
						szTxtKeySuffix = "FOOD";
					} else if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC) {
						iUnitGift = /*10*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_BONUS_UNIT);
						szTxtKeySuffix = "UNIT";
					}
				}

				// Hasn't met anyone yet?
				if(GET_TEAM(GetPlayer()->getTeam()).getHasMetCivCount(true) == 0)
				{
					// If we're not using the new gift system, just stick with the default gold and faith gifts
					if (!MOD_GLOBAL_CS_GIFTS) 
					{
						iGoldGift = /*30*/ GD_INT_GET(MINOR_CIV_CONTACT_GOLD_FIRST);
						if (GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
							iFaithGift = 8; //antonjs: todo: XML
					}
					
					bFirstMajorCiv = true;
				}
				else
				{
					if (MOD_GLOBAL_CS_GIFTS) 
					{
						// Reduce gifts if we're not the first team to meet the CS
						int iBonusMultiplier = /*1*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_MULTIPLIER);
						int iBonusDivisor = /*2*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_SUBSEQUENT_TEAM_DIVISOR);

						iFriendshipBoost = (iFriendshipBoost * iBonusMultiplier) / max(1,iBonusDivisor);
						iCultureGift = iCultureGift / max(1,iBonusDivisor);
						iFaithGift = iFaithGift / max(1,iBonusDivisor);

						iGoldGift = 0;
						iFoodGift = 0;
						iUnitGift = 0;
					} 
					else 
					{
						iGoldGift = /*15 in CP, 20 in VP*/ GD_INT_GET(MINOR_CIV_CONTACT_GOLD_OTHER);
						if (GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
							iFaithGift = 4; //antonjs: todo: XML
					}
				}

				if (MOD_GLOBAL_CS_GIFTS) 
				{
					if (eRealPersonality == MINOR_CIV_PERSONALITY_IRRATIONAL) 
					{
						// Assumes MINOR_CIV_PERSONALITY_IRRATIONAL is the last entry in the enum
						eFakePersonality = (MinorCivPersonalityTypes)GC.getGame().getSmallFakeRandNum(NUM_MINOR_CIV_PERSONALITY_TYPES - 1, m_pPlayer->GetPseudoRandomSeed());
					}
					
		 			// Personality modifiers - friendly = x1.5, hostile = x0.5
					if (eFakePersonality == MINOR_CIV_PERSONALITY_FRIENDLY) 
					{
						int iBonusMultiplier = /*3*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_MULTIPLIER);
						int iBonusDivisor = max(1, /*2*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_FRIENDLY_BONUS_DIVISOR));

						iFriendshipBoost = iFriendshipBoost * iBonusMultiplier / iBonusDivisor;
						iCultureGift = iCultureGift * iBonusMultiplier / iBonusDivisor;
						iFaithGift = iFaithGift * iBonusMultiplier / iBonusDivisor;
						iGoldGift = iGoldGift * iBonusMultiplier / iBonusDivisor;
						iFoodGift = iFoodGift * iBonusMultiplier / iBonusDivisor;

						int iUnitMultiplier = /*2*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_MULTIPLIER);
						int iUnitDivisor = max(1, /*1*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_FRIENDLY_UNIT_DIVISOR));

						iUnitGift = iUnitGift * iUnitMultiplier / iUnitDivisor;
					} 
					else if (eFakePersonality == MINOR_CIV_PERSONALITY_HOSTILE) 
					{
						int iBonusMultiplier = /*1*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_MULTIPLIER);
						int iBonusDivisor = max(1, /*2*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_HOSTILE_BONUS_DIVISOR));

						iFriendshipBoost = iFriendshipBoost * iBonusMultiplier / iBonusDivisor;
						iCultureGift = iCultureGift * iBonusMultiplier / iBonusDivisor;
						iFaithGift = iFaithGift * iBonusMultiplier / iBonusDivisor;
						iGoldGift = iGoldGift * iBonusMultiplier / iBonusDivisor;
						iFoodGift = iFoodGift * iBonusMultiplier / iBonusDivisor;

						int iUnitMultiplier = /*0*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_MULTIPLIER);
						int iUnitDivisor = max(1, /*1*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_HOSTILE_UNIT_DIVISOR));

						iUnitGift = iUnitGift * iUnitMultiplier / iUnitDivisor;
					}
				}
			}

//			if (MOD_GLOBAL_CS_GIFTS)
//				CUSTOMLOG("CS Gift: Id %i, Trait %i, Personality %i: %sFriendship=%i, Gold=%i, Culture=%i, Faith=%i, Food=%i, Unit=%i", GetPlayer()->GetID(), eTrait, eRealPersonality, (bFirstMajorCiv ? "First " : ""), iFriendshipBoost, iGoldGift, iCultureGift, iFaithGift, iFoodGift, iUnitGift);

			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

				if (GET_PLAYER(ePlayer).getTeam() == eTeam)
				{
					if (MOD_GLOBAL_CS_GIFTS) 
					{
						// Give the friendship boost to this team member
						ChangeFriendshipWithMajor(ePlayer, iFriendshipBoost, /*bFromQuest*/ false);
						iGoldGift *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
						iGoldGift /= 100;

						iCultureGift *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
						iCultureGift /= 100;

						iFaithGift *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
						iFaithGift /= 100;

						iFoodGift *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
						iFoodGift /= 100;

						// Give the gifts to this team member
						GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(iGoldGift);
						GET_PLAYER(ePlayer).changeInstantYieldValue(YIELD_GOLD, iGoldGift);
						GET_PLAYER(ePlayer).changeJONSCulture(iCultureGift);
						GET_PLAYER(ePlayer).changeInstantYieldValue(YIELD_CULTURE, iCultureGift);
						GET_PLAYER(ePlayer).ChangeFaith(iFaithGift);
						GET_PLAYER(ePlayer).changeInstantYieldValue(YIELD_FAITH, iFaithGift);

						iGift = iGoldGift + iCultureGift + iFaithGift;

						// Food and unit gifts only go to the player who actually met the CS
						if (eMeetingPlayer == ePlayer) {
							int iBonusMultiplier = /*2*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_PLAYER_MULTIPLIER);
							int iBonusDivisor = max(1, /*1*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_PLAYER_DIVISOR));

							// Give the friendship boost again to this team member (ie the meeting player gets twice the friendship boost)
							int iExtraFriendship = (iFriendshipBoost * iBonusMultiplier / iBonusDivisor) - iFriendshipBoost;
							ChangeFriendshipWithMajor(ePlayer, iExtraFriendship, /*bFromQuest*/ false);
							iFriendshipBoost = iFriendshipBoost + iExtraFriendship;  // Need this adjusting for the popup dialog

							CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
							if (eTrait == MINOR_CIV_TRAIT_MILITARISTIC) {
								if (iUnitGift > 0) {
									if (GC.getGame().getSmallFakeRandNum(100, pPlayer->GetPseudoRandomSeed()) < iUnitGift) {
										CvUnit* pUnit = DoSpawnUnit(ePlayer, true, true);
										if (pUnit != NULL) {
											pUnit->changeExperienceTimes100(100 * (pPlayer->GetCurrentEra() * /*5*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_XP_PER_ERA) + GC.getGame().getSmallFakeRandNum(/*5*/ GD_INT_GET(MINOR_CIV_FIRST_CONTACT_XP_RANDOM), pPlayer->getTotalPopulation())));
											iGift = pUnit->getUnitType();
										}
									}
								}
							} else if (eTrait == MINOR_CIV_TRAIT_MARITIME && pPlayer->getCapitalCity() != NULL) {
								iGift = iGift + iFoodGift;

								if (iFoodGift > 0) {
									CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
									CvPlot* pPlot = (pMinorCapital == NULL) ? GetPlayer()->getStartingPlot() : pMinorCapital->plot();

									CvCity* pBestCity = NULL;

									if (pPlayer->GetCurrentEra() < (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true) && pPlayer->getCapitalCity()->plot()->getArea() == pPlot->getArea()) {
										// Pre-Medieval and on the same landmass, just add the food to the capital
										pBestCity = pPlayer->getCapitalCity();
									} else {
										// Ripped from CvPlayer::receiveGoody()
										int iDistance = 0;
										int iBestCityDistance = -1;

										CvCity* pLoopCity = NULL;
										int iLoop = 0;
										// Find the closest City to us
										for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop)) {
											iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

											if (iBestCityDistance == -1 || iDistance < iBestCityDistance) {
												iBestCityDistance = iDistance;
												pBestCity = pLoopCity;
											}
										}
									}

									if (pBestCity != NULL) {
										pBestCity->changeFood(iFoodGift);
										GET_PLAYER(ePlayer).changeInstantYieldValue(YIELD_FOOD, iFoodGift);
									}
								}
							}
						}
					}
					else 
					{
						if (iGoldGift != 0)
						{
							// Gold gift
							GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(iGoldGift);
							GET_PLAYER(ePlayer).changeInstantYieldValue(YIELD_GOLD, iGoldGift);
						}
						// Faith gift
						if (iFaithGift > 0){

							GET_PLAYER(ePlayer).ChangeFaith(iFaithGift);
							GET_PLAYER(ePlayer).changeInstantYieldValue(YIELD_FAITH, iFaithGift);
						}
					}

					// Need to seed quest counter?
					if(GC.getGame().getElapsedGameTurns() > GetFirstPossibleTurnForPersonalQuests())
					{
						DoTestSeedQuestCountdownForPlayer(ePlayer);
					}

					// See if Threatening Barbarians event is active
					if(GetTurnsSinceThreatenedAnnouncement() >= 0 && GetTurnsSinceThreatenedAnnouncement() < 10)
					{
						DoTestThreatenedAnnouncementForPlayer(ePlayer);
					}

					// See if Proxy War event is active
					DoTestProxyWarAnnouncementOnFirstContact(ePlayer);

					// See if there are any quests you can join now
					DoTestQuestsOnFirstContact(ePlayer);

					// Greeting for active human player
					if(ePlayer == GC.getGame().getActivePlayer() && !bSuppressMessages)
					{
						if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Should this be !GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)
						{
							/* Current assignments of members of popupInfo
							 *   Data1 is the player id
							 *   Data2 is the gold gift value
							 *   Data3 is the faith gift value
							 *   Option1 is first met
							 *   Option2 is nil
							 *   Text is nil
							 *
							 * Updated assignments of members of popupInfo
							 *   Data1 is the player id (unchanged)
							 *   Data2 is the gift "value" (Gold/Culture/Faith/Food amount, UnitId)
							 *   Data3 is the friendship boost
							 *   Option1 is first met (unchanged)
							 *   Option2 is nil (unchanged)
							 *   Text is suffix for the TXT_KEY_ to format with
							 */
							if (MOD_GLOBAL_CS_GIFTS) 
							{
								CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_STATE_GREETING, GetPlayer()->GetID(), iGift, iFriendshipBoost, 0, bFirstMajorCiv);
								strcpy_s(kPopupInfo.szText, szTxtKeySuffix);
								GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
							} 
							else 
							{
								CvPopupInfo kPopupInfo(BUTTONPOPUP_CITY_STATE_GREETING, GetPlayer()->GetID(), iGoldGift, iFaithGift, 0, bFirstMajorCiv);
								GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
							}

							// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
							CancelActivePlayerEndTurn();
						}

						// update the mouseover text for the city-state's city banners
						int iLoop = 0;
						for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
						{
							if(pLoopCity->plot()->isRevealed(eTeam))
							{
								CvInterfacePtr<ICvCity1> pDllLoopCity = GC.WrapCityPointer(pLoopCity);
								GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pDllLoopCity.get(), CITY_UPDATE_TYPE_BANNER);
							}
						}
					}

					if (MOD_GLOBAL_CS_GIFTS && MOD_EVENTS_MINORS_GIFTS)
					{
						// Send an event with the details
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorGift, GetPlayer()->GetID(), ePlayer, iGift, iFriendshipBoost, 0, bFirstMajorCiv, false, szTxtKeySuffix);
					}

					if (GET_PLAYER(ePlayer).GetPlayerTraits()->GetInfluenceMeetCS() != 0)
					{
						SetFriendshipWithMajor(ePlayer, GET_PLAYER(ePlayer).GetPlayerTraits()->GetInfluenceMeetCS(), /*bFromQuest*/ false);
					}
				}
			}
		}
	}
}

/// Are we at war with a minor and not allied with anyone?
void CvMinorCivAI::DoTestEndWarsVSMinors(PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	if(eOldAlly == NO_PLAYER)
		return;

	if(!GetPlayer()->isAlive())
		return;

	PlayerTypes eOtherMinor;
	int iOtherMinorLoop = 0;
	PlayerTypes eOtherAlly;
	bool bForcedWar = false;

	TeamTypes eLoopTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;

		// Another Minor
		if(!GET_TEAM(eLoopTeam).isMinorCiv())
			continue;

		// They not alive!
		if(!GET_TEAM(eLoopTeam).isAlive())
			continue;

		// At war with him
		if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
			continue;

		if(eOldAlly != NO_PLAYER)
		{
			// Old ally wasn't at war
			if(!GET_TEAM(GET_PLAYER(eOldAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		if(eNewAlly != NO_PLAYER)
		{
			// New ally IS at war
			if(GET_TEAM(GET_PLAYER(eNewAlly).getTeam()).isAtWar(eLoopTeam))
				continue;
		}

		// Make sure this guy isn't allied with someone at war with us
		bForcedWar = false;

		for(iOtherMinorLoop = 0; iOtherMinorLoop < MAX_CIV_TEAMS; iOtherMinorLoop++)
		{
			eOtherMinor = (PlayerTypes) iOtherMinorLoop;

			// Other minor is on this team
			if(GET_PLAYER(eOtherMinor).getTeam() == eLoopTeam)
			{
				eOtherAlly = GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetAlly();
				if(eOtherAlly != NO_PLAYER)
				{
					// This guy's ally at war with us?
					if(GET_TEAM(GET_PLAYER(eOtherAlly).getTeam()).isAtWar(GetPlayer()->getTeam()))
					{
						bForcedWar = true;
						break;
					}
				}
			}

		}

		if(bForcedWar)
			continue;

		if(IsPermanentWar(eLoopTeam))
			continue;

		GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam, true, false, GetPlayer()->GetID());
	}
}

/// Are we at war with a minor also allied to our new BFF?
void CvMinorCivAI::DoTestEndSkirmishes(PlayerTypes eNewAlly)
{
	if(eNewAlly == NO_PLAYER)
	{
		eNewAlly = GetPlayer()->GetMinorCivAI()->GetAlly();
	}
	if(eNewAlly != NO_PLAYER)
	{
		if(!GetPlayer()->isAlive())
			return;

		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

			// Another Minor
			if(!GET_TEAM(eLoopTeam).isMinorCiv())
				continue;

			// They're not alive!
			if(!GET_TEAM(eLoopTeam).isAlive())
				continue;

			// At war with them
			if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
				continue;

			if(IsPermanentWar(eLoopTeam))
				continue;

			// New ally IS at war (how???)
			if(GET_TEAM(GET_PLAYER(eNewAlly).getTeam()).isAtWar(eLoopTeam))
				continue;

			for(int iOtherMinorLoop = MAX_MAJOR_CIVS; iOtherMinorLoop < MAX_CIV_PLAYERS; iOtherMinorLoop++)
			{
				PlayerTypes eOtherMinor = (PlayerTypes) iOtherMinorLoop;
				CvPlayer& kOtherMinor = GET_PLAYER(eOtherMinor);

				// Other minor is on this team
				if(kOtherMinor.isAlive() && kOtherMinor.getTeam() == eLoopTeam)
				{
					if(kOtherMinor.GetMinorCivAI()->GetAlly() == eNewAlly)
					{
						// We are at war with our new ally's ally!
						CUSTOMLOG("CS %i is at war with CS %i but they share the same ally %i - making peace", GetPlayer()->GetID(), iOtherMinorLoop, ((int) eNewAlly));
						GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam, true, false, GetPlayer()->GetID());
					}
				}
			}
		}
	}
	//No ally? End all wars!
	else
	{
		if(!GetPlayer()->isAlive())
			return;

		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

			// Another Minor
			if(!GET_TEAM(eLoopTeam).isMinorCiv())
				continue;

			// They're not alive!
			if(!GET_TEAM(eLoopTeam).isAlive())
				continue;

			// At war with them
			if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(eLoopTeam))
				continue;

			for(int iOtherMinorLoop = MAX_MAJOR_CIVS; iOtherMinorLoop < MAX_CIV_PLAYERS; iOtherMinorLoop++)
			{
				PlayerTypes eOtherMinor = (PlayerTypes) iOtherMinorLoop;
				CvPlayer& kOtherMinor = GET_PLAYER(eOtherMinor);

				// Other minor is on this team
				if(kOtherMinor.isAlive() && kOtherMinor.getTeam() == eLoopTeam)
				{
					//Not if permanent war!
					if(!kOtherMinor.isMinorCiv())
						continue;

					if(kOtherMinor.GetMinorCivAI()->IsPermanentWar(GetPlayer()->getTeam()))
						continue;
					
					//No ally, or not at war with their ally? Peace!
					if(kOtherMinor.GetMinorCivAI()->GetAlly() == NO_PLAYER || !GET_TEAM(GET_PLAYER(kOtherMinor.GetMinorCivAI()->GetAlly()).getTeam()).isAtWar(GetPlayer()->getTeam()))
					{
						// We are at war with our new ally's ally!
						CUSTOMLOG("CS %i is at war with CS %i but neither has an ally or a warring ally %i - making peace", GetPlayer()->GetID(), iOtherMinorLoop, ((int) eNewAlly));
						GET_TEAM(GetPlayer()->getTeam()).makePeace(eLoopTeam, true, false, GetPlayer()->GetID());
					}
				}
			}
		}
	}
}

/// Update what our status is
void CvMinorCivAI::DoTurnStatus()
{
	int iWeight = 0;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
		CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
		CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());

		PlayerProximityTypes eProximity = pPlayer->GetProximityToPlayer(GetPlayer()->GetID());

		// Check how close the player is
		switch(eProximity)
		{
			// DISTANT: Elevated if we're at war
		case PLAYER_PROXIMITY_DISTANT:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 2;
			}
			if(pTeam->IsMinorCivWarmonger())
			{
				iWeight += 4;
			}
			if(IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 6;
			}
			break;

			// FAR: Elevated if they're an aggressor OR we're at war (note the ELSE IF)
		case PLAYER_PROXIMITY_FAR:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 4;
			}
			if(pTeam->IsMinorCivWarmonger())
			{
				iWeight += 6;
			}
			if(IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 8;
			}
			// CLOSE: Elevated if they're an aggressor, critical if we're at war
		case PLAYER_PROXIMITY_CLOSE:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 10;
			}
			if(pTeam->IsMinorCivWarmonger())
			{
				iWeight += 20;
			}
			if(IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 15;
			}
			break;

			// NEIGHBORS: Pretty much anything makes the situation critical
		case PLAYER_PROXIMITY_NEIGHBORS:
			if(pTeam->IsMinorCivAggressor())
			{
				iWeight += 20;
			}
			if(pTeam->IsMinorCivWarmonger())
			{
				iWeight += 20;
			}
			if(IsAtWarWithPlayersTeam(ePlayer))
			{
				iWeight += 20;
			}
			break;
		default:
			break;
		}
	}

	if(GetNumThreateningBarbarians() > 0)
	{
		iWeight += GetNumThreateningBarbarians() * 10;
	}
	if(m_pPlayer->getNumMilitaryUnits() <= 6)
	{
		iWeight += 10;
	}
	if(m_pPlayer->getNumMilitaryUnits() <= 3)
	{
		iWeight += 20;
	}

	// Do the final math
	if(iWeight >= 20)
	{
		m_eStatus = MINOR_CIV_STATUS_CRITICAL;
	}
	else if(iWeight >= 10)
	{
		m_eStatus = MINOR_CIV_STATUS_ELEVATED;
	}
	else
	{
		m_eStatus = MINOR_CIV_STATUS_NORMAL;
	}
}

/// What is our status
MinorCivStatusTypes CvMinorCivAI::GetStatus() const
{
	return m_eStatus;
}

/// We have a new city! Add any special starting resources we get.
void CvMinorCivAI::DoAddStartingResources(CvPlot* pCityPlot)
{
	CvAssertMsg(pCityPlot != NULL, "City's plot should not be NULL. Please send Anton your save file and version.");
	if (pCityPlot == NULL) return;

	MinorCivTraitTypes eTrait = GetTrait();

	// Mercantile
	if (eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		bool bAddUniqueLuxury = true;
		
		// Only for our first city
		if (GetPlayer()->getNumCities() > 1)
		{
			bAddUniqueLuxury = false;
		}
	
		// Only if there isn't already a unique luxury on this plot
		ResourceTypes ePlotResource = pCityPlot->getResourceType();
		if (ePlotResource != NO_RESOURCE)
		{
			CvResourceInfo* pkPlotResourceInfo = GC.getResourceInfo(ePlotResource);
			if (pkPlotResourceInfo && pkPlotResourceInfo->isOnlyMinorCivs())
			{
				bAddUniqueLuxury = false;
			}
		}

		// Pick and add a unique luxury
		if (bAddUniqueLuxury)
		{
			vector<ResourceTypes> veUniqueLuxuries;
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				const ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResourceLoop);
				if(pkResourceInfo)
				{
					if(pkResourceInfo->isOnlyMinorCivs())
					{
						veUniqueLuxuries.push_back(eResourceLoop);
					}
				}
			}

			pCityPlot->setResourceType(NO_RESOURCE, 0, true);
			if (veUniqueLuxuries.size() > 0)
			{
				int iRoll = GC.getGame().getSmallFakeRandNum(veUniqueLuxuries.size(), *pCityPlot); // range = [0, size - 1]
				int iQuantity = /*1*/ GD_INT_GET(MINOR_CIV_MERCANTILE_RESOURCES_QUANTITY);
				ResourceTypes eSpecialLuxury = veUniqueLuxuries[iRoll];

				pCityPlot->setResourceType(eSpecialLuxury, iQuantity, true);
			}
		}
	}
}

/// Notifications
void CvMinorCivAI::AddNotification(const CvString& sString, const CvString& sSummaryString, PlayerTypes ePlayer, int iX, int iY)
{
	if(iX == -1 && iY == -1)
	{
		CvCity* capCity = GetPlayer()->getCapitalCity();

		if(capCity != NULL)
		{
			iX = capCity->getX();
			iY = capCity->getY();
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		pNotifications->Add(NOTIFICATION_MINOR, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}

/// Quest Notifications
void CvMinorCivAI::AddQuestNotification(CvString sString, const CvString& sSummaryString, PlayerTypes ePlayer, int iX, int iY, bool bNewQuest)
{
	CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
	if(pNotifications)
	{
		sString += "[NEWLINE][NEWLINE]";
		sString += Localization::Lookup("TXT_KEY_MINOR_QUEST_BLOCKING_TT").toUTF8();

		if (bNewQuest)
			pNotifications->Add(NOTIFICATION_MINOR_QUEST, sString, sSummaryString, iX, iY, GetPlayer()->GetID(), 1);
		else
			pNotifications->Add(NOTIFICATION_MINOR_QUEST, sString, sSummaryString, iX, iY, GetPlayer()->GetID());
	}
}

// ******************************
// Threatened by Barbarians event
// ******************************

/// Are there threatening barbarians that ePlayer can get credit for clearing out?
/// NOTE: This event can be active even if we haven't sent ePlayer a notification (ex. is only one barb, ePlayer is far away)
bool CvMinorCivAI::IsThreateningBarbariansEventActiveForPlayer(PlayerTypes ePlayer)
{
	if (GetPlayer()->isAlive())
	{
		if (IsHasMetPlayer(ePlayer) && !IsAtWarWithPlayersTeam(ePlayer))
		{
			if (GetNumThreateningBarbarians() > 0)
			{
				return true;
			}
		}
	}
	
	return false;
}

/// Barbs in or near our borders?
int CvMinorCivAI::GetNumThreateningBarbarians()
{
	int iCount = 0;

	int iLoop = 0;
	for(CvUnit* pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
		if(pLoopUnit->plot()->isAdjacentTeam(GetPlayer()->getTeam()))
			iCount++;

	return iCount;
}

int CvMinorCivAI::GetNumThreateningMajors()
{
	int iCount = 0;

	for (int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;
		if (ePlayer == NO_PLAYER)
			continue;

		if (ePlayer == GetPlayer()->GetID())
			continue;

		CvPlayer& kPlayer = GET_PLAYER(ePlayer);

		if (!kPlayer.isAlive())
			continue;

		if (!kPlayer.IsAtWarWith(GetPlayer()->GetID()))
			continue;

		int iLoop = 0;
		for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
			if (pLoopUnit->plot()->isAdjacentTeam(GetPlayer()->getTeam()))
				iCount++;
	}
	

	return iCount;
}

/// Barbs in our borders?
int CvMinorCivAI::GetNumBarbariansInBorders(bool bOnlyAdjacentToCity)
{
	if (GetPlayer()->getCapitalCity() == NULL)
		return 0;

	int iCount = 0;

	int iLoop = 0;
	for (CvUnit* pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
	{
		if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->plot()->getOwner() == m_pPlayer->GetID())
			if (!bOnlyAdjacentToCity || pLoopUnit->plot()->IsAdjacentCity())
				iCount++;
	}

	return iCount;
}

/// Barbarians threatening this Minor?
void CvMinorCivAI::DoTestThreatenedAnnouncement()
{
	// Increment counter - this is only used when sending notifications to players
	if(GetTurnsSinceThreatenedAnnouncement() >= 0)
	{
		ChangeTurnsSinceThreatenedAnnouncement(1);

		// Long enough to have expired?
		if(GetTurnsSinceThreatenedAnnouncement() >= 30)
			SetTurnsSinceThreatenedAnnouncement(-1);
	}

	// Not already threatened?
	if(GetTurnsSinceThreatenedAnnouncement() == -1)
	{
		if(GetNumThreateningBarbarians() >= /*2*/ GD_INT_GET(MINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD))
		{
			// Wasn't under attack before, but is now!
			SetTurnsSinceThreatenedAnnouncement(0);

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				DoTestThreatenedAnnouncementForPlayer((PlayerTypes) iPlayerLoop);
			}
		}
	}
}

/// How long has this Minor been under attack from Barbs?
int CvMinorCivAI::GetTurnsSinceThreatenedAnnouncement() const
{
	return m_iTurnsSinceThreatenedByBarbarians;
}

/// How long has this Minor been under attack from Barbs?
void CvMinorCivAI::SetTurnsSinceThreatenedAnnouncement(int iValue)
{
	if(GetTurnsSinceThreatenedAnnouncement() != iValue)
		m_iTurnsSinceThreatenedByBarbarians = iValue;
}

/// How long has this Minor been under attack from Barbs?
void CvMinorCivAI::ChangeTurnsSinceThreatenedAnnouncement(int iChange)
{
	SetTurnsSinceThreatenedAnnouncement(GetTurnsSinceThreatenedAnnouncement() + iChange);
}

/// Barbarians are invading...ask ePlayer for help!
void CvMinorCivAI::DoTestThreatenedAnnouncementForPlayer(PlayerTypes ePlayer)
{
	if (GetPlayer()->isAlive())
	{
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if (pCapital)
		{
			int iX = pCapital->getX();
			int iY = pCapital->getY();
			if (IsThreateningBarbariansEventActiveForPlayer(ePlayer))
			{
				// Is ePlayer is close to us?
				if (IsPlayerCloseEnoughForThreatenedAnnouncement(ePlayer))
				{
					// Are there enough barbs to warrant sending a message about it?
					if (GetNumThreateningBarbarians() >= /*2*/ GD_INT_GET(MINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD))
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARBS_QUEST");
						strMessage << GetPlayer()->getNameKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_BARBS_QUEST");
						strSummary << GetPlayer()->getNameKey();
						AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer, iX, iY);
					}
				}
			}
		}
	}
}

/// Is player close enough for the game to send a notification about the barbarian threat?
bool CvMinorCivAI::IsPlayerCloseEnoughForThreatenedAnnouncement(PlayerTypes eMajor)
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorCapital = GET_PLAYER(eMajor).getCapitalCity();

	if (!pCapital || !pMajorCapital)
		return false;

	// Has Minor met this player yet?
	if (!IsHasMetPlayer(eMajor))
		return false;

	if (pCapital->HasSharedAreaWith(pMajorCapital,true,false))
		return true;

	int iDistance = plotDistance(pCapital->getX(), pCapital->getY(), pMajorCapital->getX(), pMajorCapital->getY());
	return iDistance <= /*50*/ GD_INT_GET(MAX_DISTANCE_MINORS_BARB_QUEST);
}

/// Player killed a threatening barb, so reward him!
/// NOTE: Player can get credit even if we haven't sent him a notification (ex. is only one barb, player is far away)
void CvMinorCivAI::DoThreateningBarbKilled(PlayerTypes eKillingPlayer, int iX, int iY)
{
	CvAssertMsg(eKillingPlayer >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eKillingPlayer < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");

	if (IsThreateningBarbariansEventActiveForPlayer(eKillingPlayer))
	{
		ChangeFriendshipWithMajor(eKillingPlayer, /*12 in CP, 15 in VP*/ GD_INT_GET(FRIENDSHIP_PER_BARB_KILLED));

		Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED");
		strMessage << GetPlayer()->getNameKey();
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SM_MINOR_BARB_KILLED");
		strSummary << GetPlayer()->getNameKey();

		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eKillingPlayer, iX, iY);
	}
}


// ******************************
// ***** Proxy War event *****
// ******************************

/// Time to send out a "Help us with Units" notification?
void CvMinorCivAI::DoTestProxyWarAnnouncement()
{
	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());
		if (!pNotifyTeam->isHasMet(GetPlayer()->getTeam()))
		{
			continue;
		}

		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes eOtherMajor = (PlayerTypes) i;
			if (IsProxyWarActiveForMajor(eNotifyPlayer, eOtherMajor))
			{
				CvPlayer* pOtherMajor = &GET_PLAYER(eOtherMajor);
				CvAssertMsg(pOtherMajor, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
				if (pOtherMajor)
				{
					TeamTypes eEnemyTeam = pOtherMajor->getTeam();
					PlayerTypes eEnemyTeamLeader = GET_TEAM(eEnemyTeam).getLeaderID();
					CvPlayer* pEnemyTeamLeader = &GET_PLAYER(eEnemyTeamLeader);
					CvAssert(pEnemyTeamLeader);
					if (pEnemyTeamLeader && GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsAtWar(eEnemyTeam) == 1)
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
						strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyTeamLeader->getCivilizationShortDescriptionKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
						strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

						AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eNotifyPlayer);

						break;
					}
				}
			}
		}
	}
}

void CvMinorCivAI::DoTestProxyWarAnnouncementOnFirstContact(PlayerTypes eMajor)
{
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		PlayerTypes eOtherMajor = (PlayerTypes) i;
		if (IsProxyWarActiveForMajor(eMajor, eOtherMajor))
		{
			CvPlayer* pOtherMajor = &GET_PLAYER(eOtherMajor);
			CvAssertMsg(pOtherMajor, "Error sending out proxy war notification from a city-state. Please send Anton your save file and version.");
			if (pOtherMajor)
			{
				TeamTypes eEnemyTeam = pOtherMajor->getTeam();
				PlayerTypes eEnemyTeamLeader = GET_TEAM(eEnemyTeam).getLeaderID();
				CvPlayer* pEnemyTeamLeader = &GET_PLAYER(eEnemyTeamLeader);
				CvAssert(pEnemyTeamLeader);
				if (pEnemyTeamLeader)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_WAR_UNIT_HELP");
					strMessage << GetPlayer()->getCivilizationShortDescriptionKey() << pEnemyTeamLeader->getCivilizationShortDescriptionKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_WAR_UNIT_HELP");
					strSummary << GetPlayer()->getCivilizationShortDescriptionKey();

					AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);

					break;
				}
			}
		}
	}
}

bool CvMinorCivAI::IsProxyWarActiveForMajor(PlayerTypes eMajor, PlayerTypes eOtherMajor)
{
	CvAssert(eMajor >= 0 && eMajor < MAX_MAJOR_CIVS);
	CvAssert(eOtherMajor >= 0 && eOtherMajor < MAX_MAJOR_CIVS);
	if (eMajor != eOtherMajor && eMajor >= 0 && eMajor < MAX_MAJOR_CIVS && eOtherMajor >= 0 && eOtherMajor < MAX_MAJOR_CIVS)
	{
		TeamTypes eOtherTeam = GET_PLAYER(eOtherMajor).getTeam();
		CvAssert(eOtherTeam != NO_TEAM);
		if (eOtherTeam != NO_TEAM)
		{
			// eMajor is at peace with us and eOtherMajor
			if (GET_PLAYER(eMajor).isAlive() && !IsAtWarWithPlayersTeam(eMajor) && !GET_TEAM(eOtherTeam).isAtWar(GET_PLAYER(eMajor).getTeam()))
			{
				// eOtherMajor is at war with us
				if (GET_PLAYER(eOtherMajor).isAlive() && IsAtWarWithPlayersTeam(eOtherMajor) && !IsPeaceBlocked(eOtherTeam))
				{
					// Do some additional checks to safeguard against weird scenario cases (ex. major and minor on same team, major is dead)
					PlayerTypes eOtherTeamLeader = GET_TEAM(eOtherTeam).getLeaderID();
					CvPlayer* pOtherTeamLeader = &GET_PLAYER(eOtherTeamLeader);
					if (pOtherTeamLeader && !pOtherTeamLeader->isMinorCiv() && pOtherTeamLeader->isAlive())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvMinorCivAI::IsProxyWarActiveForMajor(PlayerTypes eMajor)
{
	CvAssert(eMajor >= 0 && eMajor < MAX_MAJOR_CIVS);
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if (IsProxyWarActiveForMajor(eMajor, (PlayerTypes)i))
		{
			return true;
		}
	}
	return false;
}


// ******************************
// ***** Quests *****
// ******************************

/// Update turn for Quests
void CvMinorCivAI::DoTurnQuests()
{
	int iFirstTurnForGlobalQuests = GetFirstPossibleTurnForGlobalQuests();
	int iFirstTurnForPersonalQuests = GetFirstPossibleTurnForPersonalQuests();

	// ********************
	// Check Current Quests
	// ********************
	DoTestActiveQuests(/*bTestComplete*/ true, /*bTestObsolete*/ true);
	DoQuestsCleanup();

	// Increment turns since last quest
	if (GetGlobalQuestCountdown() > 0)
		ChangeGlobalQuestCountdown(-1);

	vector<PlayerTypes> ValidMajors;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (!GET_PLAYER(ePlayer).isAlive() || !GET_PLAYER(ePlayer).isMajorCiv() || !GET_PLAYER(ePlayer).getCapitalCity())
			continue;

		if (!IsHasMetPlayer(ePlayer))
			continue;

		// Increment turns since last quest
		if (GetQuestCountdownForPlayer(ePlayer) > 0)
			ChangeQuestCountdownForPlayer(ePlayer, -1);

		if (IsAtWarWithPlayersTeam(ePlayer))
			continue;

		ValidMajors.push_back(ePlayer);
	}

	// Stop here if we don't have a capital.
	if (!m_pPlayer->getCapitalCity())
		return;

	// ********************
	// Give Global Quests
	// ********************

	// Unlock turn - seed the counter
	if (GC.getGame().getElapsedGameTurns() == iFirstTurnForGlobalQuests)
	{
		DoTestSeedGlobalQuestCountdown();
	}

	if (GC.getGame().getElapsedGameTurns() > iFirstTurnForGlobalQuests || ENABLE_QUESTS_AT_START)
	{
		DoTestStartGlobalQuest();
	}

	// ********************
	// Give Personal Quests
	// ********************

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (!GET_PLAYER(ePlayer).isAlive() || !GET_PLAYER(ePlayer).isMajorCiv() || !GET_PLAYER(ePlayer).getCapitalCity())
			continue;

		if (IsAtWarWithPlayersTeam(ePlayer))
			continue;

		if (!IsHasMetPlayer(ePlayer))
			continue;

		// Increment turns since last quest
		if (GetQuestCountdownForPlayer(ePlayer) > 0)
		{
			ChangeQuestCountdownForPlayer(ePlayer, -1);
		}

		// Unlock turn - seed the counter
		if (GC.getGame().getElapsedGameTurns() == iFirstTurnForPersonalQuests)
		{
			DoTestSeedQuestCountdownForPlayer(ePlayer);
		}
		// Can't start a quest too early
		else if (GC.getGame().getElapsedGameTurns() > iFirstTurnForPersonalQuests || ENABLE_QUESTS_AT_START)
		{
			DoTestStartPersonalQuest(ePlayer);
		}
	}
}

/// What is the first possible turn of the game we can fire off a Quest for a player?
int CvMinorCivAI::GetFirstPossibleTurnForPersonalQuests() const
{
	int firstTurn = /*30 in CP, 4 in VP*/ GD_INT_GET(MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN);
	firstTurn *= sqrti(GC.getGame().getGameSpeedInfo().getGreatPeoplePercent());   		// used sqrti because turn 90 (Community Patch Only) seemed too long to wait on marathon
	firstTurn /= 10;
	return firstTurn;
}

/// What is the first possible turn of the game we can give out global Quests, that are for multiple players?
int CvMinorCivAI::GetFirstPossibleTurnForGlobalQuests() const
{
	int firstTurn = /*30 in CP, 4 in VP*/ GD_INT_GET(MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN);
	firstTurn *= sqrti(GC.getGame().getGameSpeedInfo().getGreatPeoplePercent());   		// used sqrti because turn 90 (Community Patch Only) seemed too long to wait on marathon
	firstTurn /= 10;
	return firstTurn;
}

/// What is the maximum number of player-specific quests a player can have at one time?
int CvMinorCivAI::GetMaxActivePersonalQuestsForPlayer() const
{
	return 2; //antonjs: todo: constant/XML, link with size of typedef vectors in header file
}

/// What is the maximum number of global (multiple player) quests this minor can have active at one time?
int CvMinorCivAI::GetMaxActiveGlobalQuests() const
{
	return 1; //antonjs: todo: constant/XML, link with size of typedef vectors in header file
}

/// See if it is time to start a global (multiple player) quest
void CvMinorCivAI::DoTestStartGlobalQuest()
{
	if (GetGlobalQuestCountdown() > 0)
		return;

	if (GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		return;

	// Pick a valid quest
	vector<MinorCivQuestTypes> veValidQuests;
	for (int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		MinorCivQuestTypes eQuest = (MinorCivQuestTypes) iQuestLoop;
		if (IsEnabledQuest(eQuest) && IsGlobalQuest(eQuest))
		{
			// Are there enough players for this quest to be given out?
			int iNumValidPlayers = 0;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
				if (IsValidQuestForPlayer(ePlayer, eQuest))
					iNumValidPlayers++;
			}
			if (iNumValidPlayers < GetMinPlayersNeededForQuest(eQuest))
				continue;

			// What is the bias for this minor favoring this particular quest? Queue up multiple copies (default is 10)
			int iCount = GetNumQuestCopies(eQuest);
			for (int iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
			{
				veValidQuests.push_back(eQuest);
			}
		}
	}

	// No valid quests
	if (veValidQuests.size() == 0)
		return;

	// There are valid quests, so pick one at random
	int iRandSeed = GetNumActiveGlobalQuests() + m_pPlayer->GetPseudoRandomSeed() + GC.getGame().GetCultureMedian() + GC.getGame().GetScienceMedian();
	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidQuests.size(), iRandSeed);
	MinorCivQuestTypes eQuest = veValidQuests[iRandIndex];

	// Give out the quest
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
		if (IsValidQuestForPlayer(ePlayer, eQuest))
		{
			// Since we are adding each instance of the quest separately, global quests should not rely on random choice of data
			AddQuestForPlayer(ePlayer, eQuest, GC.getGame().getGameTurn());
		}
	}

	// Check if we need to seed the countdown timer to allow for another quest
	DoTestSeedGlobalQuestCountdown();
}

//Check if the player already has the same quest from another minor
bool IsUniqueQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	for (int i = MAX_MAJOR_CIVS; i < MAX_CIV_PLAYERS; i++)
		if (GET_PLAYER((PlayerTypes)i).GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, eQuest))
			return false;

	return true;
}

/// See if it is time to start a personal quest for this player
void CvMinorCivAI::DoTestStartPersonalQuest(PlayerTypes ePlayer)
{
	if (IsAtWarWithPlayersTeam(ePlayer) || IsRecentlyBulliedByMajor(ePlayer))
		return;

	// Can't start a new quest too quickly after an old one has ended
	if (GetQuestCountdownForPlayer(ePlayer) > 0)
		return;

	// Can't start a new quest if we already have several active ones
	if (GetNumActivePersonalQuestsForPlayer(ePlayer) >= GetMaxActivePersonalQuestsForPlayer())
		return;

	vector<MinorCivQuestTypes> veValidQuests;
	for (int iQuestLoop = 0; iQuestLoop < NUM_MINOR_CIV_QUEST_TYPES; iQuestLoop++)
	{
		MinorCivQuestTypes eQuest = (MinorCivQuestTypes) iQuestLoop;

		if (IsPersonalQuest(eQuest) && IsValidQuestForPlayer(ePlayer, eQuest) && (!MOD_BALANCE_VP || IsUniqueQuestForPlayer(ePlayer,eQuest)))
		{
			// What is the bias for this player wanting this particular quest? Queue up multiple copies (default is 10)
			int iCount = GetNumQuestCopies(eQuest);

			for (int iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
			{
				veValidQuests.push_back(eQuest);
			}
		}
	}

	// No valid Quests
	if (veValidQuests.size() == 0)
		return;

	int iRandSeed = ePlayer + GetNumActiveQuestsForAllPlayers() + m_pPlayer->GetTreasury()->GetLifetimeGrossGold();
	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidQuests.size(), iRandSeed);
	MinorCivQuestTypes eQuest = veValidQuests[iRandIndex];
	AddQuestForPlayer(ePlayer, eQuest, GC.getGame().getGameTurn());

	// Check if we need to seed the countdown timer to allow for another quest
	DoTestSeedQuestCountdownForPlayer(ePlayer);
}

/// Begin a quest, initializing it with data
void CvMinorCivAI::AddQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iStartTurn, PlayerTypes pCallingPlayer)
{
	CvMinorCivQuest newQuest(GetPlayer()->GetID(), ePlayer, eType);
	newQuest.DoStartQuest(iStartTurn, pCallingPlayer);
	m_QuestsGiven[ePlayer].push_back(newQuest);

	// Log quest received
	GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestReceived(GetPlayer()->GetID(), GetEffectiveFriendshipWithMajorTimes100(ePlayer), GetEffectiveFriendshipWithMajorTimes100(ePlayer), eType);
}

/// Begin a quest that is already underway, copying over quest data
void CvMinorCivAI::AddQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest)
{
	CvMinorCivQuest newQuest(GetPlayer()->GetID(), ePlayer, pQuest->GetType());
	newQuest.DoStartQuestUsingExistingData(pQuest);
	m_QuestsGiven[ePlayer].push_back(newQuest);

	// Log quest received
	GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestReceived(GetPlayer()->GetID(), GetEffectiveFriendshipWithMajorTimes100(ePlayer), GetEffectiveFriendshipWithMajorTimes100(ePlayer), pQuest->GetType());
}


/// Called on first contact with major.  Have major join global quests that are already in progress (ex. contests).
/// Works on the assumption that this minor can only give out one of each type of global quest at a time.
/// Works on the assumption that any quests eMajor would be given in this way are valid for eMajor without having to check.
void CvMinorCivAI::DoTestQuestsOnFirstContact(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	std::set<MinorCivQuestTypes> setGlobalQuests;
	
	for (uint iPlayerLoop = 0; iPlayerLoop < m_QuestsGiven.size(); iPlayerLoop++)
	{
		for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[iPlayerLoop].size(); iQuestLoop++)
		{
			CvMinorCivQuest quest = m_QuestsGiven[iPlayerLoop][iQuestLoop];
			CvMinorCivQuest* pQuest = &quest;
			if (!pQuest)
				continue;

			// Haven't addressed this quest type yet?
			MinorCivQuestTypes eQuestType = pQuest->GetType();
			if (IsGlobalQuest(eQuestType) && setGlobalQuests.find(eQuestType) == setGlobalQuests.end())
			{
				// Can we give a copy of this quest to this player?
				if (IsValidQuestCopyForPlayer(eMajor, pQuest))
				{
					AddQuestCopyForPlayer(eMajor, pQuest);
					setGlobalQuests.insert(eQuestType);
				}
			}
		}
	}
}

// Check all active quests for all players, processing and deleting ones that are complete or obsolete.
void CvMinorCivAI::DoTestActiveQuests(bool bTestComplete, bool bTestObsolete)
{
	if (bTestComplete)
		DoCompletedQuests();
	
	if (bTestObsolete)
		DoObsoleteQuests();
}

// Check all active quests for ePlayer, processing and deleting ones that are complete or obsolete.
// If no quest type is specified, will check all quest types.
void CvMinorCivAI::DoTestActiveQuestsForPlayer(PlayerTypes ePlayer, bool bTestComplete, bool bTestObsolete, MinorCivQuestTypes eQuest)
{
	if (bTestComplete)
		DoCompletedQuestsForPlayer(ePlayer, eQuest);
	
	if (bTestObsolete)
		DoObsoleteQuestsForPlayer(ePlayer, eQuest);
}

// Check for quests that have been completed, process them beginning with the player with highest resulting influence.
void CvMinorCivAI::DoCompletedQuests()
{
	WeightedCivsList vePlayers = CalculateFriendshipFromQuests();

	// Resolve quests starting with player that will end up with most influence, to avoid multiple ally swaps and awkward war declarations
	for (int iMajorIndex = 0; iMajorIndex < vePlayers.size(); iMajorIndex++)
	{
		PlayerTypes eMajor = vePlayers.GetElement(iMajorIndex);
		DoCompletedQuestsForPlayer(eMajor);
	}
}

// Return a sorted weighted vector of player influence values accounting for quest rewards they are about to receive.
WeightedCivsList CvMinorCivAI::CalculateFriendshipFromQuests()
{
	WeightedCivsList vePlayerInfluences;
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		int iInfTimes100 = GetEffectiveFriendshipWithMajorTimes100(eMajorLoop);
		if (IsHasMetPlayer(eMajorLoop) && !IsAtWarWithPlayersTeam(eMajorLoop))
		{
			QuestListForPlayer::iterator itr_quest;
			for (itr_quest = m_QuestsGiven[eMajorLoop].begin(); itr_quest != m_QuestsGiven[eMajorLoop].end(); itr_quest++)
			{
				if (itr_quest->IsComplete())
				{
					iInfTimes100 += itr_quest->GetInfluence() * 100;
				}
			}
		}
		int iWeight = iInfTimes100 + abs(/*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR) * 100); // Weights cannot be negative, so shift all values up by minimum influence amount
		vePlayerInfluences.push_back(eMajorLoop, iWeight);
	}

	vePlayerInfluences.StableSortItems();
	return vePlayerInfluences;
}

// Process completed quests that are active, and seed countdowns if needed.
// If no quest type is specified, will check all quest types.
void CvMinorCivAI::DoCompletedQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bCheckAllQuests = true;
	if (eSpecifyQuestType > NO_MINOR_CIV_QUEST_TYPE && eSpecifyQuestType < NUM_MINOR_CIV_QUEST_TYPES)
		bCheckAllQuests = false;

	//do not use an iterator here!
	//finishing one quest may generate new ones, potentially invalidating the iterator!
	for (size_t i = 0; i < m_QuestsGiven[ePlayer].size(); i++)
	{
		CvMinorCivQuest& quest = m_QuestsGiven[ePlayer][i];
		if (bCheckAllQuests || quest.GetType() == eSpecifyQuestType)
		{
			if (quest.IsComplete())
			{
				int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				bool bCompleted = quest.DoFinishQuest();
				int iNewFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				
				if (bCompleted)
					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestFinished(GetPlayer()->GetID(), iOldFriendshipTimes100, iNewFriendshipTimes100, quest.GetType());
			}
		}
	}
}

// Check for quests that are obsolete and process them for each player in no particular order.
void CvMinorCivAI::DoObsoleteQuests()
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		DoObsoleteQuestsForPlayer(eMajorLoop);
	}
}

// Process obsolete quests that are active, and seed countdowns if needed.
// If no quest type is specified, will check all quest types.
void CvMinorCivAI::DoObsoleteQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType, bool bWar, bool bHeavyTribute) 
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bCheckAllQuests = true;
	if (eSpecifyQuestType > NO_MINOR_CIV_QUEST_TYPE && eSpecifyQuestType < NUM_MINOR_CIV_QUEST_TYPES)
		bCheckAllQuests = false;

	bool bQuestRevoked = false;

	QuestListForPlayer::iterator itr_quest;
	for (itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if (bCheckAllQuests || itr_quest->GetType() == eSpecifyQuestType)
		{
			// If a specific quest type was given, cancel it. Otherwise, check if the quest is now obsolete. Declaring war or demanding heavy tribute makes all quests obsolete except for the barbarian horde quest
			if (!bCheckAllQuests || itr_quest->IsObsolete(bWar || bHeavyTribute))
			{
				int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				bool bCancelled = itr_quest->DoCancelQuest();
				int iNewFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(ePlayer);
				
				if (bCancelled)
				{
					if (itr_quest->IsRevoked(bWar || bHeavyTribute))
					{
						bQuestRevoked = true;
						// special rule for heavy tribute: if the quest to conquer another CS or to ally both was given, cancel the quest also for the other CS
						if (itr_quest->GetType() == MINOR_CIV_QUEST_KILL_CITY_STATE)
						{
							GET_PLAYER((PlayerTypes)itr_quest->GetPrimaryData()).GetMinorCivAI()->DoObsoleteQuestsForPlayer(ePlayer, MINOR_CIV_QUEST_KILL_CITY_STATE, false, false);
						}
					}

					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogMinorCivQuestCancelled(GetPlayer()->GetID(), iOldFriendshipTimes100, iNewFriendshipTimes100, itr_quest->GetType());
				}
			}
		}
	}

	// If quest(s) were revoked because of bullying or war, send out a notification
	if (bQuestRevoked && GetPlayer()->isAlive()) 
	{
		if (bWar)
		{
			Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REVOKED_WAR");
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REVOKED_WAR");
			strMessage << GetPlayer()->getNameKey();
			strSummary << GetPlayer()->getNameKey();
			AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
		}
		else
		{
			Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_QUEST_ENDED_REVOKED");
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_QUEST_ENDED_REVOKED");
			strMessage << GetPlayer()->getNameKey();
			strSummary << GetPlayer()->getNameKey();
			AddQuestNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
		}
	}
}

void CvMinorCivAI::DoQuestsCleanup()
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		DoQuestsCleanupForPlayer(eMajorLoop);
	}
}

// Deletes active completed and/or obsolete quests from memory, and resets the countdown timer if needed.
void CvMinorCivAI::DoQuestsCleanupForPlayer(PlayerTypes ePlayer)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	bool bPersonalQuestDone = false;
	bool bGlobalQuestDone = false;

	QuestListForPlayer::iterator itr_quest;
	for (itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
	{
		if (itr_quest->IsHandled())
		{
			MinorCivQuestTypes eQuestType = itr_quest->GetType();
			if (IsPersonalQuest(eQuestType))
				bPersonalQuestDone = true;
			if (IsGlobalQuest(eQuestType))
				bGlobalQuestDone = true;

			m_QuestsGiven[ePlayer].erase(itr_quest);
			itr_quest--;
		}
	}

	// Check if we need to seed the countdown timers
	if (bPersonalQuestDone)
		DoTestSeedQuestCountdownForPlayer(ePlayer);
	if (bGlobalQuestDone)
		DoTestSeedGlobalQuestCountdown();
}

bool CvMinorCivAI::IsTargetQuest(MinorCivQuestTypes eQuest)
{
	switch (eQuest)
	{
	case MINOR_CIV_QUEST_KILL_CAMP:
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	case MINOR_CIV_QUEST_LIBERATION:
	case MINOR_CIV_QUEST_EXPLORE_AREA:
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
		return true;
	default:
		return false;
	}
}

bool CvMinorCivAI::PlayerHasTarget(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;
	if (eQuest < NO_MINOR_CIV_QUEST_TYPE || eQuest >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	if (IsTargetQuest(eQuest))
	{
		for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
		{
			if (IsTargetQuest(m_QuestsGiven[ePlayer][iQuestLoop].GetType()))
			{
				return true;
			}
		}
	}

	return false;
}

// Is this quest enabled at all?
bool CvMinorCivAI::IsEnabledQuest(MinorCivQuestTypes eQuest)
{
	switch (eQuest)
	{
	case MINOR_CIV_QUEST_ROUTE:
		return GD_INT_GET(QUEST_DISABLED_ROUTE) < 1;
	case MINOR_CIV_QUEST_KILL_CAMP:
		return GD_INT_GET(QUEST_DISABLED_KILL_CAMP) < 1;
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
		return GD_INT_GET(QUEST_DISABLED_CONNECT_RESOURCE) < 1;
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
		return GD_INT_GET(QUEST_DISABLED_CONSTRUCT_WONDER) < 1;
	case MINOR_CIV_QUEST_GREAT_PERSON:
		return GD_INT_GET(QUEST_DISABLED_GREAT_PERSON) < 1;
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
		return !GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE) && (!MOD_BALANCE_VP || !GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE)) && GD_INT_GET(QUEST_DISABLED_KILL_CITY_STATE) < 1;
	case MINOR_CIV_QUEST_FIND_PLAYER:
		return GD_INT_GET(QUEST_DISABLED_FIND_PLAYER) < 1;
	case MINOR_CIV_QUEST_FIND_CITY:
		return GD_INT_GET(QUEST_DISABLED_FIND_CITY) < 1;
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
		return GD_INT_GET(QUEST_DISABLED_NATURAL_WONDER) < 1;
	case MINOR_CIV_QUEST_GIVE_GOLD:
		return GD_INT_GET(QUEST_DISABLED_GIVE_GOLD) < 1;
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
		return GD_INT_GET(QUEST_DISABLED_PLEDGE_TO_PROTECT) < 1;
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
		return !GC.getGame().isOption(GAMEOPTION_NO_POLICIES) && GD_INT_GET(QUEST_DISABLED_CONTEST_CULTURE) < 1;
	case MINOR_CIV_QUEST_CONTEST_FAITH:
		return !GC.getGame().isOption(GAMEOPTION_NO_RELIGION) && GD_INT_GET(QUEST_DISABLED_CONTEST_FAITH) < 1;
	case MINOR_CIV_QUEST_CONTEST_TECHS:
		return !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE) && GD_INT_GET(QUEST_DISABLED_CONTEST_TECHS) < 1;
	case MINOR_CIV_QUEST_INVEST:
		return GD_INT_GET(QUEST_DISABLED_INVEST) < 1;
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
		return GD_INT_GET(QUEST_DISABLED_BULLY_CITY_STATE) < 1;
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
		return GD_INT_GET(QUEST_DISABLED_DENOUNCE_MAJOR) < 1;
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
		return !GC.getGame().isOption(GAMEOPTION_NO_RELIGION) && GD_INT_GET(QUEST_DISABLED_SPREAD_RELIGION) < 1;
	case MINOR_CIV_QUEST_TRADE_ROUTE:
		return GD_INT_GET(QUEST_DISABLED_TRADE_ROUTE) < 1;
	case MINOR_CIV_QUEST_WAR:
		return !GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE) && !GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE) && GD_INT_GET(QUEST_DISABLED_WAR) < 1;
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
		return GD_INT_GET(QUEST_DISABLED_CONSTRUCT_NATIONAL_WONDER) < 1;
	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
		return GD_INT_GET(QUEST_DISABLED_GIFT_SPECIFIC_UNIT) < 1;
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
		return GD_INT_GET(QUEST_DISABLED_FIND_CITY_STATE) < 1;
	case MINOR_CIV_QUEST_INFLUENCE:
		return GD_INT_GET(QUEST_DISABLED_INFLUENCE) < 1;
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
		return GD_INT_GET(QUEST_DISABLED_CONTEST_TOURISM) < 1;
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
		return GD_INT_GET(QUEST_DISABLED_ARCHAEOLOGY) < 1;
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
		return GD_INT_GET(QUEST_DISABLED_CIRCUMNAVIGATION) < 1;
	case MINOR_CIV_QUEST_LIBERATION:
		return GD_INT_GET(QUEST_DISABLED_LIBERATION) < 1;
	case MINOR_CIV_QUEST_HORDE:
		return !GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS) && GD_INT_GET(QUEST_DISABLED_HORDE) < 1;
	case MINOR_CIV_QUEST_REBELLION:
		return !GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS) && GD_INT_GET(QUEST_DISABLED_REBELLION) < 1;
	case MINOR_CIV_QUEST_EXPLORE_AREA:
		return GD_INT_GET(QUEST_DISABLED_EXPLORE_AREA) < 1;
	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
		return GD_INT_GET(QUEST_DISABLED_BUILD_X_BUILDINGS) < 1;
	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
		return !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && (MOD_BALANCE_VP || !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE)) && GD_INT_GET(QUEST_DISABLED_SPY_ON_MAJOR) < 1;
	case MINOR_CIV_QUEST_COUP:
		return !GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) && GD_INT_GET(QUEST_DISABLED_COUP) < 1;
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
		return !GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE) && GD_INT_GET(QUEST_DISABLED_ACQUIRE_CITY) < 1;
	default:
		return false;
	}
}

/// Is eQuest valid for this minor to give to ePlayer?
bool CvMinorCivAI::IsValidQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	// Somebody's dead, that's no good
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if (!GET_PLAYER(ePlayer).isAlive() || !pMajorCapital || !pMajorCapital->plot() || !GetPlayer()->isAlive() || !pMinorCapital || !pMinorCapital->plot())
		return false;

	// The quest type must be enabled
	if (!IsEnabledQuest(eQuest))
		return false;

	// No quests are valid if we don't know you yet
	if (!IsHasMetPlayer(ePlayer))
		return false;

	bool bSpecialGlobal = eQuest == MINOR_CIV_QUEST_HORDE || eQuest == MINOR_CIV_QUEST_REBELLION;
	if (!bSpecialGlobal)
	{
		// No quests are valid if we are at war
		if (IsAtWarWithPlayersTeam(ePlayer))
			return false;

		if (IsRecentlyBulliedByMajor(ePlayer))
		{
			// No personal quests are valid if they recently bullied us
			if (IsPersonalQuest(eQuest))
				return false;

			// In Community Patch only, the invest quest isn't valid either
			if (!MOD_BALANCE_VP && eQuest == MINOR_CIV_QUEST_INVEST)
				return false;
		}
	}

	if (MOD_BALANCE_VP) 
	{
		// No quests are valid if they recently declared war on us
		if (!bSpecialGlobal && GetJerkTurnsRemaining(GET_PLAYER(ePlayer).getTeam()) > 0)
		{
			return false;
		}
		// Only one map-specific quest per minor for now. Keeps the tap icons clear.
		// Global quests ignore the limit, but still block new personal quests.
		if (IsPersonalQuest(eQuest) && PlayerHasTarget(ePlayer, eQuest))
		{
			return false;
		}
	}

	// This player cannot already have an active quest of the same type
	if (IsActiveQuestForPlayer(ePlayer, eQuest))
		return false;

	int iQuestDuration = 0;
	CvSmallAwardInfo* pkSmallAwardInfo = GC.getSmallAwardInfo((SmallAwardTypes)eQuest);
	if (pkSmallAwardInfo)
		iQuestDuration = pkSmallAwardInfo->GetDuration();

	// BUILD A ROUTE
	switch (eQuest)
	{
	case MINOR_CIV_QUEST_ROUTE:
	{
		// Must have been friends at some point
		if (!IsEverFriends(ePlayer))
			return false;

		// Can't have completed this Quest already
		if (IsRouteConnectionEstablished(ePlayer))
			return false;

		// Cannot already have a route
		if (GET_PLAYER(ePlayer).IsCapitalConnectedToPlayer(GetPlayer()->GetID()))
			return false;

		// City-State's capital cannot be coastal
		// Otherwise it's too easy and the logic becomes too complex with lighthouses
		if (pMinorCapital->isCoastal())
			return false;

		// Must have a city close to the City-State on the same Landmass
		const int iMaxRouteDistance = /*7 in CP, 11 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_MAXIMUM_DISTANCE);
		CvPlot* pMinorPlot = pMinorCapital->plot();

		int iLoop = 0;
		bool bInRange = false;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			if (pLoopCity->plot()->getLandmass() != pMinorPlot->getLandmass())
				continue;

			int iDistance = plotDistance(pMinorPlot->getX(), pMinorPlot->getY(), pLoopCity->getX(), pLoopCity->getY());
			if (iDistance <= iMaxRouteDistance)
			{
				bInRange = true;
				break;
			}
		}

		if (!bInRange)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		// Any nearby camps?
		if (GetBestNearbyCampToKill() == NULL)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
	{
		// Any nearby resources?
		if (GetNearbyResourceForQuest(ePlayer) == NO_RESOURCE)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	{
		if (GetBestWorldWonderForQuest(ePlayer, iQuestDuration) == NO_BUILDING)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_GREAT_PERSON:
	{
		if (GetBestGreatPersonForQuest(ePlayer) == NO_UNIT)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		if (MOD_BALANCE_VP)
		{
			if (GetAlly() != NO_PLAYER || IsNoAlly())
				return false;

			// Only one CS war at a time globally.
			for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
				if (GET_PLAYER(eMinor).isAlive() && GetPlayer()->IsAtWarWith(eMinor))
					return false;
			}
		}
		else if (GetPersonality() == MINOR_CIV_PERSONALITY_FRIENDLY)
			return false;

		if (GetBestCityStateTarget(ePlayer, true) == NO_PLAYER)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_FIND_PLAYER:
	{
		if (GetBestPlayerToFind(ePlayer) == NO_PLAYER)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY:
	{
		if (GetBestCityToFind(ePlayer) == NULL)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
	{
		if (!IsGoodTimeForNaturalWonderQuest(ePlayer))
			return false;

		break;
	}
	case MINOR_CIV_QUEST_GIVE_GOLD:
	{
		// Is there a recent bully that isn't this player?
		if (!IsRecentlyBulliedByAnyMajor() || GetMostRecentBullyForQuest() == ePlayer)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
	{
		// Is there a recent bully that isn't this player?
		if (!IsRecentlyBulliedByAnyMajor() || GetMostRecentBullyForQuest() == ePlayer)
			return false;

		// Only if we aren't already protected by this major, and the major is allowed to start
		if (!CanMajorStartProtection(ePlayer))
			return false;

		break;
	}
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	case MINOR_CIV_QUEST_CONTEST_TECHS:
	{
		// Don't create this quest until a player has entered the Medieval Era
		EraTypes eCurrentEra = GET_PLAYER(ePlayer).GetCurrentEra();
		EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
		if (eCurrentEra < eMedieval)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		// Don't create this quest until a player has entered the Medieval Era
		EraTypes eCurrentEra = GET_PLAYER(ePlayer).GetCurrentEra();
		EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
		if (eCurrentEra < eMedieval)
			return false;

		if (MOD_BALANCE_VP)
		{
			// At least 2 religions must have been founded
			if (GC.getGame().GetGameReligions()->GetNumReligionsFounded(true) <= 1)
				return false;

			// Only players with a state religion may participate
			if (GET_PLAYER(ePlayer).GetReligions()->GetStateReligion(false) == NO_RELIGION)
				return false;
		}

		break;
	}
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	{
		if (GetBestCityStateTarget(ePlayer, false) == NO_PLAYER)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
	{
		// Is there a recent bully?
		if (!IsRecentlyBulliedByAnyMajor())
			return false;

		PlayerTypes eMostRecentBully = GetMostRecentBullyForQuest();
		if (eMostRecentBully == NO_PLAYER)
			return false;

		// This player must not be the ally
		if (GetAlly() == eMostRecentBully)
			return false;

		// Humans are unable to denounce each other
		if (GET_PLAYER(ePlayer).isHuman() && GET_PLAYER(eMostRecentBully).isHuman())
			return false;

		// This player must not have already denounced the most recent bully
		if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eMostRecentBully))
			return false;

		// Cannot denounce due to game options
		if (GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE) && GET_PLAYER(ePlayer).IsAtWarWith(eMostRecentBully))
			return false;

		// Is this a bad target? (Same team, haven't met, backstabbing?)
		if (!IsAcceptableQuestEnemy(MINOR_CIV_QUEST_DENOUNCE_MAJOR, ePlayer, eMostRecentBully))
			return false;

		break;
	}
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
	{
		// Player must have founded (or conquered) a religion
		ReligionTypes eOwnedReligion = GET_PLAYER(ePlayer).GetReligions()->GetOwnedReligion();
		if (eOwnedReligion == NO_RELIGION)
			return false;

		// Minor must not already share player's religion
		if (IsSameReligionAsMajor(ePlayer))
			return false;

		// Must have at least one of their cities following their owned religion
		if (GC.getGame().GetGameReligions()->GetNumDomesticCitiesFollowing(eOwnedReligion, ePlayer) == 0)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_TRADE_ROUTE:
	{
		// City-State Sanctions resolution in effect?
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague != NULL && pLeague->IsTradeEmbargoed(ePlayer, GetPlayer()->GetID()))
			return false;

		// This player must not already be connected
		if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(ePlayer, GetPlayer()->GetID()))
			return false;

		// This player must be able to build a trade route either by land or sea
		if (!GC.getGame().GetGameTrade()->CanCreateTradeRoute(ePlayer,GetPlayer()->GetID(), DOMAIN_LAND) &&
			!GC.getGame().GetGameTrade()->CanCreateTradeRoute(ePlayer,GetPlayer()->GetID(), DOMAIN_SEA))
			return false;

		break;
	}
	case MINOR_CIV_QUEST_WAR:
	{
		// Is there a recent bully?
		if (!IsRecentlyBulliedByAnyMajor())
			return false;

		PlayerTypes eMostRecentBully = GetMostRecentBullyForQuest();
		if (eMostRecentBully == NO_PLAYER)
			return false;

		// This player must not be the ally
		if (GetAlly() != NO_PLAYER && GET_PLAYER(eMostRecentBully).getTeam() == GET_PLAYER(GetAlly()).getTeam())
			return false;

		// This player must not be at war with the most recent bully
		if (GET_PLAYER(ePlayer).IsAtWarWith(eMostRecentBully))
			return false;

		// This player must be able to declare war on the most recent bully
		if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(GET_PLAYER(eMostRecentBully).getTeam(), ePlayer))
			return false;

		// Is this a bad target? (Same team, haven't met, backstabbing?)
		if (!IsAcceptableQuestEnemy(MINOR_CIV_QUEST_WAR, ePlayer, eMostRecentBully))
			return false;

		break;
	}
	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
	{
		if (GetBestNationalWonderForQuest(ePlayer, iQuestDuration) == NO_BUILDING)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
	{
		UnitTypes eUnit = GetBestUnitGiftFromPlayer(ePlayer);
		if (eUnit == NO_UNIT)
			return false;

		if (GetExperienceForUnitGiftQuest(ePlayer, eUnit) == 0)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_FIND_CITY_STATE:
	{
		// Don't create this quest until a player has entered the Renaissance Era
		EraTypes eCurrentEra = GET_PLAYER(ePlayer).GetCurrentEra();	
		EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
		if (eCurrentEra < eRenaissance)
			return false;

		if (GetBestCityStateMeetTarget(ePlayer) == NO_PLAYER)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_INFLUENCE:
	{
		// Don't create this quest until a player has entered the Medieval Era
		EraTypes eCurrentEra = GET_PLAYER(ePlayer).GetCurrentEra();
		EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
		if (eCurrentEra < eMedieval)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		// Don't create this quest until a player has entered the Renaissance Era
		EraTypes eCurrentEra = GET_PLAYER(ePlayer).GetCurrentEra();	
		EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
		if (eCurrentEra < eRenaissance)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		// Any nearby dig sites?
		CvPlot* pDigPlot = GetBestNearbyDig();
		if (pDigPlot == NULL)
			return false;

		// Can the player send an archaeologist?
		for (int i = 0; i < GC.getNumBuildInfos(); i++)
		{
			BuildTypes eBuild = (BuildTypes)i;
			CvBuildInfo* pBuildInfo = GC.getBuildInfo(eBuild);
			if (pBuildInfo == NULL || eBuild == NO_BUILD)
				continue;
			// Does the build action create an archaeology prompt?
			ImprovementTypes eImprovement = (ImprovementTypes)pBuildInfo->getImprovement();
			if (eImprovement == NO_IMPROVEMENT)
				continue;
			CvImprovementEntry& pImprovementEntry = *GC.getImprovementInfo(eImprovement);
			ResourceTypes eResource = pDigPlot->getResourceType(GET_PLAYER(ePlayer).getTeam());
			if (!(pImprovementEntry.IsPromptWhenComplete() && pImprovementEntry.IsImprovementResourceMakesValid(eResource)))
				continue;
			// Does the player have access to the build action?
			if (!GET_PLAYER(ePlayer).canBuild(pDigPlot, eBuild, false /*bTestEra*/, false /*bTestVisible*/, false /*bTestGold*/, false /*bTestPlotOwner*/))
				continue;

			return true;
		}
		return false;
	}
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		if (!GC.getGame().circumnavigationAvailable())
			return false;

		//Give this quest out once the player can embark across oceans (Traits not included).
		if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canEmbarkAllWaterPassage())
			return false;

		break;
	}
	case MINOR_CIV_QUEST_LIBERATION:
	{
		if (GetBestCityStateLiberate(ePlayer) == NO_PLAYER)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_HORDE:
	case MINOR_CIV_QUEST_REBELLION:
	{
		if (GC.getGame().getGameTurn() < GC.getGame().GetBarbarianReleaseTurn())
			return false;

		if (eQuest == MINOR_CIV_QUEST_HORDE)
		{
			if (SpawnHorde() == NO_PLAYER && !IsHordeActive())
				return false;
		}
		else
		{
			if (SpawnRebels() == NO_PLAYER && !IsRebellionActive())
				return false;
		}

		break;
	}
	case MINOR_CIV_QUEST_EXPLORE_AREA:
	{
		if (GetTargetPlot(ePlayer) == NULL)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
	{
		// Don't give this quest to Venice
		if (GET_PLAYER(ePlayer).GetPlayerTraits()->IsNoAnnexing())
			return false;

		if (GetBestBuildingForQuest(ePlayer, iQuestDuration) == NO_BUILDING)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
	case MINOR_CIV_QUEST_COUP:
	{
		// Don't create this quest until a player has entered the Renaissance
		EraTypes eCurrentEra = GET_PLAYER(ePlayer).GetCurrentEra();
		EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
		if (eCurrentEra < eRenaissance)
			return false;

		if (GetBestSpyTarget(ePlayer, eQuest == MINOR_CIV_QUEST_COUP) == NULL)
			return false;

		break;
	}
	case MINOR_CIV_QUEST_ACQUIRE_CITY:
	{
		if (GetBestCityForQuest(ePlayer) == NULL)
			return false;

		break;
	}
	default:
		break; // No special condition requirements.
	}

	return true;
}

/// Can we give a copy of pQuest to ePlayer (i.e. late join to global quest)?
bool CvMinorCivAI::IsValidQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest)
{
	// Somebody's dead, that's no good
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	CvCity* pMajorCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if (!GET_PLAYER(ePlayer).isAlive() || !pMajorCapital || !pMajorCapital->plot() || !GetPlayer()->isAlive() || !pMinorCapital || !pMinorCapital->plot())
		return false;

	// The quest type must be enabled
	MinorCivQuestTypes eQuest = pQuest->GetType();
	if (!IsEnabledQuest(eQuest))
		return false;

	// No quests are valid if we don't know you yet
	if (!IsHasMetPlayer(ePlayer))
		return false;

	// This player cannot already have an active quest of the same type
	if (IsActiveQuestForPlayer(ePlayer, eQuest))
		return false;

	// No quests are valid if we are at war
	bool bSpecialGlobal = eQuest == MINOR_CIV_QUEST_HORDE || eQuest == MINOR_CIV_QUEST_REBELLION;
	if (!bSpecialGlobal && IsAtWarWithPlayersTeam(ePlayer))
		return false;

	if (MOD_BALANCE_VP) 
	{
		// No quests are valid if they recently declared war on us
		if (!bSpecialGlobal && GetJerkTurnsRemaining(GET_PLAYER(ePlayer).getTeam()) > 0)
		{
			return false;
		}
	}
	else if (eQuest == MINOR_CIV_QUEST_INVEST && IsRecentlyBulliedByMajor(ePlayer))
		return false;

	// Test if the quest is complete or expired
	if (pQuest->IsComplete() || pQuest->IsExpired())
		return false;

	return true;
}


bool CvMinorCivAI::IsGlobalQuest(MinorCivQuestTypes eQuest) const
{
	switch (eQuest)
	{
	case MINOR_CIV_QUEST_KILL_CAMP:
		return true;
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
		return MOD_BALANCE_VP; // Kill another City-State differs between CP and VP
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
		return true;
	case MINOR_CIV_QUEST_CONTEST_TOURISM:
		return true;
	case MINOR_CIV_QUEST_CONTEST_FAITH:
		return true;
	case MINOR_CIV_QUEST_CONTEST_TECHS:
		return true;
	case MINOR_CIV_QUEST_INVEST:
		return true;
	case MINOR_CIV_QUEST_INFLUENCE:
		return true;
	case MINOR_CIV_QUEST_ARCHAEOLOGY:
		return true;
	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
		return true;
	case MINOR_CIV_QUEST_HORDE:
		return true;
	case MINOR_CIV_QUEST_REBELLION:
		return true;
	default:
		return false;
	}
}

bool CvMinorCivAI::IsPersonalQuest(MinorCivQuestTypes eQuest) const
{
	return !IsGlobalQuest(eQuest);
}

int CvMinorCivAI::GetMinPlayersNeededForQuest(MinorCivQuestTypes eQuest) const
{
	int iPlayersNeeded = 1;

	if(eQuest == MINOR_CIV_QUEST_CONTEST_CULTURE)
	{
		iPlayersNeeded = 3; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_FAITH)
	{
		iPlayersNeeded = 3; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TECHS)
	{
		iPlayersNeeded = 3; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_INVEST)
	{
		iPlayersNeeded = 2; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_KILL_CITY_STATE)
	{
		iPlayersNeeded = 2; //antonjs: todo: XML
	}

	else if(eQuest == MINOR_CIV_QUEST_INFLUENCE)
	{
		iPlayersNeeded = 2;
	}

	else if(eQuest == MINOR_CIV_QUEST_CONTEST_TOURISM)
	{
		iPlayersNeeded = 3;
	}

	int iMajorsEverAlive = GC.getGame().countMajorCivsEverAlive();
	iPlayersNeeded = min(iPlayersNeeded, iMajorsEverAlive);

	return iPlayersNeeded;
}

/// How more or less likely is this minor to offer a particular quest type based on its trait and personality?
/// Default is 10 copies of each quest. Minimum 1 copy of each valid quest.
int CvMinorCivAI::GetNumQuestCopies(MinorCivQuestTypes eQuest) const
{
	int iNumCopies = 0;
	MinorCivPersonalityTypes ePersonality = GetPersonality();
	MinorCivTraitTypes eTrait = GetTrait();

	// ******************
	// Personal Quests
	// ******************

	switch (eQuest)
	{
	case MINOR_CIV_QUEST_ROUTE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*2*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*-8*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ROUTE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*20*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONNECT_RESOURCE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*20*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_WONDER_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_GREAT_PERSON:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*20*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GREAT_PERSON_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_FIND_PLAYER:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*20*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_PLAYER_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_FIND_CITY:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*20*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*-7*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_NATURAL_WONDER_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_GIVE_GOLD:
	{
		iNumCopies = /*30*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC: // We're tough, we don't need your charity >:(
			iNumCopies += /*-10*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE: // Money, that's what I need.
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIVE_GOLD_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
	{
		iNumCopies = /*30*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED: // Military? We don't have one of those!
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC: // We're tough, we can take care of ourselves.
			iNumCopies += /*-10*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_PLEDGE_TO_PROTECT_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*-7*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_BULLY_CITY_STATE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
	{
		iNumCopies = /*15*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_DENOUNCE_MAJOR_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_SPREAD_RELIGION:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*20*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPREAD_RELIGION_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_TRADE_ROUTE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_TRADE_ROUTE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_WAR:
	{
		iNumCopies = /*15*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED: // Help!!
			iNumCopies += /*40*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC: // How dare they bully us? We'll make them pay!
			iNumCopies += /*70*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE: // Vengeance will be ours!
			iNumCopies += /*50*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_WAR_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER:
	{
		iNumCopies = /*7*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*23*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONSTRUCT_NATIONAL_WONDER_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT:
	{
		iNumCopies = /*7*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*13*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_FIND_CITY_STATE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*1*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*2*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_STATE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_LIBERATION:
	{
		iNumCopies = /*5*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC: // War beckons - will you answer?
			iNumCopies += /*55*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*45*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY: // Our friends were in that city!
			iNumCopies += /*35*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_LIBERATION_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_EXPLORE_AREA:
	{
		iNumCopies = /*5*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*22*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*15*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_EXPLORE_AREA_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_BUILD_X_BUILDINGS:
	{
		iNumCopies = /*15*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_BUILD_X_BUILDINGS_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_SPY_ON_MAJOR:
	{
		iNumCopies = /*7*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*8*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*-2*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*28*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_SPY_ON_MAJOR_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_COUP:
	{
		iNumCopies = /*15*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*-8*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*8*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*30*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_COUP_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_ACQUIRE_CITY:
	{
		iNumCopies = /*2*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*48*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*18*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*-35*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*25*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ACQUIRE_CITY_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	// ******************
	// Global Quests
	// ******************

	case MINOR_CIV_QUEST_KILL_CAMP:
	{
		iNumCopies = /*30*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*30*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	// NOTE: This is a personal quest in Community Patch only
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0 in CP, -5 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*10 in CP, 5 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0 in CP, -5 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0 in CP, -5 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY: // In Community Patch only, friendly CS will not give this quest at all
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*-6 in CP, -5 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*10 in CP, 5 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0 in CP, -2 in VP*/ GD_INT_GET(MINOR_CIV_QUEST_KILL_CITY_STATE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONTEST_CULTURE:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_CULTURE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONTEST_FAITH:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_FAITH_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONTEST_TECHS:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*-5*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TECHS_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_INVEST:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE: // Money, that's what I need.
			iNumCopies += /*5*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INVEST_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_INFLUENCE:
	{
		iNumCopies = /*7*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE: // Leave us alone!
			iNumCopies += /*-2*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_INFLUENCE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CONTEST_TOURISM:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*3*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CONTEST_TOURISM_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_ARCHAEOLOGY:
	{
		iNumCopies = /*7*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC: // Recover the spoils of ancient war!
			iNumCopies += /*18*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS: // Religious relics, you say?
			iNumCopies += /*23*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_CIRCUMNAVIGATION:
	{
		iNumCopies = /*10*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME: // We are the masters of the sea!
			iNumCopies += /*15*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_CIRCUMNAVIGATION_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_HORDE:
	{
		iNumCopies = /*30*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_HORDE_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	case MINOR_CIV_QUEST_REBELLION:
	{
		iNumCopies = /*70*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_BASE);

		switch (eTrait)
		{
		case MINOR_CIV_TRAIT_CULTURED:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_CULTURED);
			break;
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_MILITARISTIC);
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_MARITIME);
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_MERCANTILE);
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_RELIGIOUS);
			break;
		default:
			UNREACHABLE();
		}

		switch (ePersonality)
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_FRIENDLY);
			break;
		case MINOR_CIV_PERSONALITY_NEUTRAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_NEUTRAL);
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_HOSTILE);
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iNumCopies += /*0*/ GD_INT_GET(MINOR_CIV_QUEST_REBELLION_COPIES_IRRATIONAL);
			break;
		default:
			UNREACHABLE();
		}

		break;
	}

	default:
		break;
	}

	return max(iNumCopies, 1);
}

/// ASSUMPTION: There is no more than one active quest of any given MinorCivQuestTypes
int CvMinorCivAI::GetNumActiveGlobalQuests() const
{
	//antonjs: todo: reevaluate this for efficiency and for use of std::set

	std::set<MinorCivQuestTypes> setGlobalQuests;

	for(uint iPlayerLoop = 0; iPlayerLoop < m_QuestsGiven.size(); iPlayerLoop++)
	{
		for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[iPlayerLoop].size(); iQuestLoop++)
		{
			if(IsGlobalQuest(m_QuestsGiven[iPlayerLoop][iQuestLoop].GetType()))
			{
				setGlobalQuests.insert(m_QuestsGiven[iPlayerLoop][iQuestLoop].GetType());
			}
		}
	}

	return setGlobalQuests.size();
}

int CvMinorCivAI::GetNumActiveQuestsForAllPlayers() const
{
	int iCount = 0;
	for (QuestListForAllPlayers::const_iterator it = m_QuestsGiven.begin(); it != m_QuestsGiven.end(); ++it)
		iCount += it->size();
	return iCount;
}

int CvMinorCivAI::GetNumActiveQuestsForPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	return m_QuestsGiven[ePlayer].size();
}

int CvMinorCivAI::GetNumActivePersonalQuestsForPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	int iCount = 0;
	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(IsPersonalQuest(m_QuestsGiven[ePlayer][iQuestLoop].GetType()))
		{
			iCount++;
		}
	}

	return iCount;
}

bool CvMinorCivAI::IsActiveQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return true;
		}
	}

	return false;
}

/// Get rid of all active quests
void CvMinorCivAI::EndAllActiveQuestsForPlayer(PlayerTypes ePlayer, bool bWar)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	//antonjs: todo: instead, call for cancel quest (with flag for no notif)
	DoObsoleteQuestsForPlayer(ePlayer, NO_MINOR_CIV_QUEST_TYPE, bWar);
}
#if defined(MOD_BALANCE_CORE)
void CvMinorCivAI::DeleteQuest(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			m_QuestsGiven[ePlayer][iQuestLoop].SetHandled(true);
		}
	}
}
#endif

int CvMinorCivAI::GetNumDisplayedQuestsForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	int iValue = 0;
	
	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(!m_QuestsGiven[ePlayer][iQuestLoop].IsHandled())
		{
			iValue++;
		}
	}

	return iValue;
}

bool CvMinorCivAI::IsDisplayedQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	CvAssertMsg(eType >= NO_MINOR_CIV_QUEST_TYPE, "eType is expected to be non-negative (invalid Index)");
	CvAssertMsg(eType < NUM_MINOR_CIV_QUEST_TYPES, "eType is expected to be within maximum bounds (invalid Index)");
	if(eType < NO_MINOR_CIV_QUEST_TYPE || eType >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			if(!m_QuestsGiven[ePlayer][iQuestLoop].IsHandled())
			{
				return true;
			}
		}
	}

	return false;
}

void CvMinorCivAI::DoTestSeedGlobalQuestCountdown(bool bForceSeed)
{
	if(!bForceSeed)
	{
		// Only seed if there is a potential spot for a global quest
		if(GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		{
			return;
		}

		// Only seed if the countdown is not already active
		if(GetGlobalQuestCountdown() > 0)
		{
			return;
		}
	}

	int iNumTurns = 0;

	// Quests are now available for the first time?
	if (GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForGlobalQuests())
	{
		iNumTurns += GC.getGame().getSmallFakeRandNum(/*20 in CP, 0 in VP*/ GD_INT_GET(MINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND), m_pPlayer->GetPseudoRandomSeed() * 2);
	}
	else
	{
		iNumTurns += /*40 in CP, 25 in VP*/ GD_INT_GET(MINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN);

		int iRand = /*25 in CP, 20 in VP*/ GD_INT_GET(MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN);
		if (GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iRand *= /*200*/ GD_INT_GET(MINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER);
			iRand /= 100;
		}
		iNumTurns += GC.getGame().getSmallFakeRandNum(iRand, m_pPlayer->GetPseudoRandomSeed() * 5);
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetGlobalQuestCountdown(iNumTurns);
}

int CvMinorCivAI::GetGlobalQuestCountdown() const
{
	return m_iGlobalQuestCountdown;
}

void CvMinorCivAI::SetGlobalQuestCountdown(int iValue)
{
	m_iGlobalQuestCountdown = iValue;
}

void CvMinorCivAI::ChangeGlobalQuestCountdown(int iChange)
{
	SetGlobalQuestCountdown(GetGlobalQuestCountdown() + iChange);
}

/// How long before we trigger a new quest?
void CvMinorCivAI::DoTestSeedQuestCountdownForPlayer(PlayerTypes ePlayer, bool bForceSeed)
{
	if(!bForceSeed)
	{
		// Only seed if there is a potential spot for a personal quest for this player
		if(GetNumActivePersonalQuestsForPlayer(ePlayer) >= GetMaxActivePersonalQuestsForPlayer())
		{
			return;
		}

		// Only seed if the countdown for this player is not already active
		if(GetQuestCountdownForPlayer(ePlayer) > 0)
		{
			return;
		}
	}

	int iNumTurns = 0;

	// Quests are now available for the first time?
	if (GC.getGame().getElapsedGameTurns() == GetFirstPossibleTurnForPersonalQuests())
	{
		iNumTurns += GC.getGame().getSmallFakeRandNum(/*20 in CP, 0 in VP*/ GD_INT_GET(MINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND), m_pPlayer->GetPseudoRandomSeed());
	}
	else
	{
		iNumTurns += /*20 in CP, 10 in VP*/ GD_INT_GET(MINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN);

		int iRand = /*25 in CP, 20 in VP*/ GD_INT_GET(MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN);
		if (GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
		{
			iRand *= /*200*/ GD_INT_GET(MINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER);
			iRand /= 100;
		}
		iNumTurns += GC.getGame().getSmallFakeRandNum(iRand, m_pPlayer->GetPseudoRandomSeed() * 4);
	}

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	SetQuestCountdownForPlayer(ePlayer, iNumTurns);
}

/// How many turns since the last Quest ended?
int CvMinorCivAI::GetQuestCountdownForPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as set during Reset()

	return m_aiQuestCountdown[ePlayer];
}

/// Sets How many turns since the last Quest ended
void CvMinorCivAI::SetQuestCountdownForPlayer(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiQuestCountdown[ePlayer] = iValue;
}

/// Changes many turns since the last Quest ended
void CvMinorCivAI::ChangeQuestCountdownForPlayer(PlayerTypes ePlayer, int iChange)
{
	SetQuestCountdownForPlayer(ePlayer, GetQuestCountdownForPlayer(ePlayer) + iChange);
}

/// For debugging and testing only. Adds eQuest if able, ignoring quest countdown timers.
bool CvMinorCivAI::AddQuestIfAble(PlayerTypes eMajor, MinorCivQuestTypes eQuest)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	if (eQuest < NO_MINOR_CIV_QUEST_TYPE || eQuest >= NUM_MINOR_CIV_QUEST_TYPES) return false;

	if (!IsValidQuestForPlayer(eMajor, eQuest))
		return false;

	if (IsPersonalQuest(eQuest))
	{
		if (GetNumActivePersonalQuestsForPlayer(eMajor) >= GetMaxActivePersonalQuestsForPlayer())
		{
			return false;
		}

		AddQuestForPlayer(eMajor, eQuest, GC.getGame().getGameTurn());
		return true;
	}
	else if (IsGlobalQuest(eQuest))
	{
		if (GetNumActiveGlobalQuests() >= GetMaxActiveGlobalQuests())
		{
			return false;
		}

		int iNumValidPlayers = 0;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			if (IsValidQuestForPlayer(ePlayerLoop, eQuest))
				iNumValidPlayers++;
		}
		if (iNumValidPlayers < GetMinPlayersNeededForQuest(eQuest))
			return false;

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayerLoop = (PlayerTypes) iPlayerLoop;
			if (IsValidQuestForPlayer(ePlayerLoop, eQuest))
				AddQuestForPlayer(ePlayerLoop, eQuest, GC.getGame().getGameTurn());
		}
		return true;
	}

	return false;
}

/// What is Data member 1
int CvMinorCivAI::GetQuestData1(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetPrimaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}

/// What is Data member 2
int CvMinorCivAI::GetQuestData2(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetSecondaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}

/// What is Data member 3
int CvMinorCivAI::GetQuestData3(PlayerTypes ePlayer, MinorCivQuestTypes eType) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_QUEST_DATA;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetTertiaryData();
		}
	}

	return CvMinorCivQuest::NO_QUEST_DATA;
}

int CvMinorCivAI::GetQuestTurnsRemaining(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iGameTurn) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return CvMinorCivQuest::NO_TURN;

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetTurnsRemaining(iGameTurn);
		}
	}

	return CvMinorCivQuest::NO_TURN;
}

CvString CvMinorCivAI::GetRewardString(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";

	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetRewardString(ePlayer, false);
		}
	}

	return "";
}
CvString CvMinorCivAI::GetTargetCityString(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			PlayerTypes eTargetPlayer = (PlayerTypes)m_QuestsGiven[ePlayer][iQuestLoop].m_iData3;
			if (eTargetPlayer != NO_PLAYER)
			{
				int iX = m_QuestsGiven[ePlayer][iQuestLoop].m_iData1;
				int iY = m_QuestsGiven[ePlayer][iQuestLoop].m_iData2;
				CvPlot* pPlot = GC.getMap().plot(iX, iY);
				if(pPlot != NULL && pPlot->isCity())
				{
					CvCity* pCity = pPlot->getPlotCity();
					if(pCity != NULL)
					{
						return pCity->getNameKey();
					}
				}
			}
		}
	}

	return "";
}

bool CvMinorCivAI::IsContestLeader(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].IsContestLeader(ePlayer);
		}
	}

	return false;
}

int CvMinorCivAI::GetContestValueForLeader(MinorCivQuestTypes eType)
{
	PlayerTypes eParticipant;
	for (int iMajor = 0; iMajor < MAX_MAJOR_CIVS; iMajor++)
	{
		eParticipant = (PlayerTypes) iMajor;
		for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[eParticipant].size(); iQuestLoop++)
		{
			if (m_QuestsGiven[eParticipant][iQuestLoop].GetType() == eType)
			{
				return m_QuestsGiven[eParticipant][iQuestLoop].GetContestValueForLeader();
			}
		}
	}

	return -1;
}

int CvMinorCivAI::GetContestValueForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	for (uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if (m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eType)
		{
			return m_QuestsGiven[ePlayer][iQuestLoop].GetContestValueForPlayer(ePlayer);
		}
	}

	return -1;
}

/// Has a Route been established?
bool CvMinorCivAI::IsRouteConnectionEstablished(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	return m_abRouteConnectionEstablished[eMajor];
}

/// Sets that a Route was established
void CvMinorCivAI::SetRouteConnectionEstablished(PlayerTypes eMajor, bool bValue)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(m_abRouteConnectionEstablished[eMajor] != bValue)
	{
		m_abRouteConnectionEstablished[eMajor] = bValue;
	}
}

/// Any Camps near us?
/// NOTE: This should pick a camp deterministically, given the current implementation of distributing global quests
CvPlot* CvMinorCivAI::GetBestNearbyCampToKill()
{
	CvMap& theMap = GC.getMap();
	CvWeightedVector<int> viPlotIndexes;
	int iNumWorldPlots = theMap.numPlots();
	int iRange = /*12*/ max(GD_INT_GET(MINOR_CIV_QUEST_KILL_CAMP_RANGE), 2);
	ImprovementTypes eCamp = (ImprovementTypes)GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT);
	int iCapitalX = GetPlayer()->getCapitalCity()->getX(), iCapitalY = GetPlayer()->getCapitalCity()->getY();

	for (int iI = 0; iI < iNumWorldPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);
		ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

		// Does it have a Barbarian Encampment?
		if (eImprovement != eCamp)
			continue;

		// Is it within range?
		int iDistance = plotDistance(iCapitalX, iCapitalY, pLoopPlot->getX(), pLoopPlot->getY());
		if (iDistance > iRange)
			continue;

		// Closer camps have higher weight
		int iWeight = 1 + iRange - iDistance;
		viPlotIndexes.push_back(pLoopPlot->GetPlotIndex(), iWeight);
	}

	// Didn't find any nearby
	if (viPlotIndexes.size() == 0)
		return NULL;

	// Choose the best plot
	viPlotIndexes.StableSortItems();
	return GC.getMap().plotByIndex(viPlotIndexes.GetElement(0));
}

/// Any dig sites near us?
/// NOTE: This should pick a dig deterministically, given the current implementation of distributing global quests
CvPlot* CvMinorCivAI::GetBestNearbyDig()
{
	CvMap& theMap = GC.getMap();
	CvWeightedVector<int> viPlotIndexes;
	int iNumWorldPlots = theMap.numPlots();
	int iRange = /*12*/ max(GD_INT_GET(MINOR_CIV_QUEST_ARCHAEOLOGY_RANGE), 2);
	ResourceTypes eAntiquitySite = (ResourceTypes)GD_INT_GET(ARTIFACT_RESOURCE);
	int iCapitalX = GetPlayer()->getCapitalCity()->getX(), iCapitalY = GetPlayer()->getCapitalCity()->getY();

	for (int iI = 0; iI < iNumWorldPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);
		ResourceTypes eResource = pLoopPlot->getResourceType();

		// Does it have an Antiquity Site?
		if (eResource != eAntiquitySite)
			continue;

		// Is it within range?
		int iDistance = plotDistance(iCapitalX, iCapitalY, pLoopPlot->getX(), pLoopPlot->getY());
		if (iDistance > iRange)
			continue;

		// Closer sites have higher weight
		int iWeight = 1 + iRange - iDistance;
		viPlotIndexes.push_back(pLoopPlot->GetPlotIndex(), iWeight);
	}

	// Didn't find any nearby
	if (viPlotIndexes.size() == 0)
		return NULL;

	// Choose the best plot
	viPlotIndexes.StableSortItems();
	return GC.getMap().plotByIndex(viPlotIndexes.GetElement(0));
}

/// Horde Spawning Time!
/// NOTE: This should tell us if it is time to spawn a horde - it is based on the wealth and size of CSs - stronger = better target!
PlayerTypes CvMinorCivAI::SpawnHorde()
{
	if(!IsValidRebellion())
	{
		return NO_PLAYER;
	}
	PlayerTypes pActiveMinor = GetPlayer()->GetID();
	EraTypes eAssumeEra = NO_ERA;
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)

		eCurrentEra = GET_TEAM(GET_PLAYER(pActiveMinor).getTeam()).GetCurrentEra();
		
		EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
		EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	//This is just for Classical/Medieval/Renaissance Eras.
	if(eCurrentEra < eClassical)
	{
		return NO_PLAYER;
	}
	else if(eCurrentEra > eRenaissance)
	{
		return NO_PLAYER;
	}

	//No hordes if isolated on islands (prevents barbarian overload).
	bool bIsAlone = true;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && GET_PLAYER(eLoopPlayer).getCapitalCity() != NULL)
		{
			if(GET_PLAYER(eLoopPlayer).getCapitalCity()->HasSharedAreaWith(GET_PLAYER(pActiveMinor).getCapitalCity(),true,false))
			{
				bIsAlone = false;
				break;
			}
		}
	}
	if(bIsAlone)
	{
		return NO_PLAYER;
	}

	int iTarget = 0;

	//Top 25%
	int iTopTier = (GC.getGame().GetNumMinorCivsAlive() / 4);
	if(iTopTier <= 0)
	{
		iTopTier = 1;
	}

	CvWeightedVector<PlayerTypes> veMinorRankings;
	
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		if(eMinorLoop != NO_PLAYER)
		{
			CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);
			CvCity* pCity = pMinorLoop->getCapitalCity();

			//Let's see if our CS is juicy and vulnerable.
			if(pMinorLoop->isAlive() && pMinorLoop->isMinorCiv() && pCity)
			{
				CvCityCitizens* pCitizens = pCity->GetCityCitizens();

				int iPlots = 0;
				int iWater = 0;
				int iImpassable = 0;

				// How easy to access is this minor? We'll ignore island/mountainous CSs for this quest, to help the AI.
				for(int iPlotLoop = 1; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
				{
					CvPlot* pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

					if(pPlot)
					{
						if(pPlot->isWater())
						{
							iWater++;
						}
						if(!pPlot->isValidMovePlot(BARBARIAN_PLAYER))
						{
							iImpassable++;
						}
					}
					iPlots++;
				}
				//50% Water? Abort. Probably an island.
				if(iWater >= (iPlots / 2))
				{
					continue;
				}

				//50% Mountains? Abort. Probably Minas Tirith.
				if(iImpassable >= (iPlots / 2))
				{
					continue;
				}

				//Baseline is population.
				iTarget = pCity->getPopulation();

				// Gold increases proclivity.
				iTarget += (pMinorLoop->GetTreasury()->GetGold() / 10);
				iTarget += pMinorLoop->GetTreasury()->GetImprovementGoldMaintenance();
				iTarget += pMinorLoop->GetTreasury()->CalculateBaseNetGold();
				iTarget += pMinorLoop->GetTrade()->GetNumDifferentTradingPartners();

				//Less military units = higher score.
				iTarget -= pMinorLoop->getNumMilitaryUnits();

				if(iTarget > 0)
				{
					veMinorRankings.push_back(eMinorLoop, iTarget);
				}
			}
		}
	}

	veMinorRankings.StableSortItems();
	if(veMinorRankings.size() != 0)
	{
		for(int iRanking = 0; iRanking < veMinorRankings.size(); iRanking++)
		{
			if(veMinorRankings.GetElement(iRanking) == pActiveMinor)
			{
				//Are we in the top 25% of CSs? If so, the quest can fire.
				if(iRanking <= iTopTier)
				{
					return pActiveMinor;
				}
			}
		}
	}
	
	return NO_PLAYER;
}

/// Rebel Spawning Time!
/// NOTE: This should tell us if it is time to spawn rebels
PlayerTypes CvMinorCivAI::SpawnRebels()
{
	if(!IsValidRebellion())
	{
		return NO_PLAYER;
	}

	PlayerTypes pActiveMinor = GetPlayer()->GetID();
	PlayerTypes ePlayer = GetPlayer()->GetMinorCivAI()->GetAlly();

	if(ePlayer == NO_PLAYER)
	{
		return NO_PLAYER;
	}
	if(GetPermanentAlly() == ePlayer)
	{
		return NO_PLAYER;
	}

	int iRebelBuildUp = 0;

	PublicOpinionTypes eOpinionInMyCiv = m_pPlayer->GetCulture()->GetPublicOpinionType();
	PlayerProximityTypes eProximity;
	eProximity = GET_PLAYER(pActiveMinor).GetProximityToPlayer(ePlayer);

	//Base value is influence with CS / 100 (i.e. 80 -> 8).
	int iRebelBoilPoint = GetPlayer()->GetMinorCivAI()->GetEffectiveFriendshipWithMajor(ePlayer);
	iRebelBoilPoint /= 10;
	
	//Hard cap at 250 influence (25)
	if(iRebelBoilPoint > 25)
	{
		iRebelBoilPoint = 25;
	}
	
	if(ePlayer != NO_PLAYER && GET_PLAYER(ePlayer).isAlive() && !GET_PLAYER(ePlayer).isMinorCiv())
	{
		if(GET_PLAYER(ePlayer).IsEmpireUnhappy())
		{
			iRebelBuildUp += 2;	
		}
		if(GET_PLAYER(ePlayer).IsEmpireVeryUnhappy())
		{
			iRebelBoilPoint /= 2;
		}
		if(GET_PLAYER(ePlayer).GetCulture()->GetPublicOpinionUnhappiness() > 0)
		{
			iRebelBuildUp += 2;
		}
		if (eOpinionInMyCiv >= PUBLIC_OPINION_CIVIL_RESISTANCE)
		{
			iRebelBoilPoint /= 2;
		}
		//How close are we? Further away = higher chance of rebellion.
		if(eProximity >= PLAYER_PROXIMITY_CLOSE)
		{
			iRebelBoilPoint += ((iRebelBoilPoint * 125) / 100);
		}
		else if(eProximity < PLAYER_PROXIMITY_CLOSE)
		{
			iRebelBoilPoint += ((iRebelBoilPoint * 100) / 125);
		}
		TeamTypes eLoopTeam;
		int iWar = 0;
		//CSs don't like war.
		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;

			if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(eLoopTeam))
			{
				iWar++;
			}
		}
		if(iWar > 0)
		{
			iRebelBuildUp += iWar;
		}

		iRebelBuildUp += GC.getGame().getSmallFakeRandNum(GC.getGame().getCurrentEra(), m_pPlayer->GetPseudoRandomSeed());

		if(iRebelBuildUp >= iRebelBoilPoint)
		{
			return pActiveMinor;
		}
	}

	return NO_PLAYER;
}

bool CvMinorCivAI::IsRebellion() const
{
	return m_bIsRebellion;
}

void CvMinorCivAI::SetRebellion(bool bValue)
{
	m_bIsRebellion = bValue;
}

void CvMinorCivAI::ChangeTurnsSinceRebellion(int iChange)
{
	SetTurnsSinceRebellion(GetTurnsSinceRebellion() + iChange);
}

int CvMinorCivAI::GetTurnsSinceRebellion() const
{
	return m_iTurnsSinceRebellion;
}

void CvMinorCivAI::SetTurnsSinceRebellion(int iValue)
{
	m_iTurnsSinceRebellion = iValue;
}

void CvMinorCivAI::DoRebellion()
{
	// In hundreds
	int iNumRebels = GetPlayer()->getNumMilitaryUnits() * /*60*/ GD_INT_GET(MINOR_QUEST_REBELLION_BARBS_PER_CS_UNIT); //Based on number of military units of CS.
	int iExtraRoll = GC.getGame().getCurrentEra(); //Increase possible rebel spawns as game continues.
	iNumRebels += iExtraRoll * /*0*/ GD_INT_GET(MINOR_QUEST_REBELLION_BARBS_PER_ERA_BASE);
	iNumRebels += GC.getGame().getSmallFakeRandNum(iExtraRoll,m_pPlayer->GetMilitaryMight()) * /*200*/ GD_INT_GET(MINOR_QUEST_REBELLION_BARBS_PER_ERA_RAND);
	iNumRebels /= 100;

	if (iNumRebels < /*2*/ GD_INT_GET(MINOR_QUEST_REBELLION_BARBS_MIN))
		iNumRebels = GD_INT_GET(MINOR_QUEST_REBELLION_BARBS_MIN);

	CvCity* pCapital = GetPlayer()->getCapitalCity();
	if (pCapital && pCapital->plot())
		CvBarbarians::SpawnBarbarianUnits(pCapital->plot(), iNumRebels, BARB_SPAWN_HORDE_QUEST);
}

bool CvMinorCivAI::IsValidRebellion()
{
	//Test to keep quests from firing over and over if ended.
	if(GetCooldownSpawn() > 0 )
	{
		return false;
	}

	int iActiveRebellions = 0;

	//Call every minor, otherwise no minor is aware of what other minors have given out.

	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;

		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);

		if (pMinorLoop && pMinorLoop->isAlive() && pMinorLoop->isMinorCiv() && (pMinorLoop != GetPlayer()))
		{
			if(pMinorLoop->GetMinorCivAI()->IsHordeActive())
			{
				iActiveRebellions++;
			}
			if(pMinorLoop->GetMinorCivAI()->IsRebellionActive())
			{
				iActiveRebellions++;
			}
		}
	}
	if(iActiveRebellions > 0)
	{
		int iActivePlayers = GC.getGame().countMajorCivsAlive();
		//Let's make this more granular.
		iActiveRebellions  *= 100;
		iActivePlayers *= 10;

		int iMaxQuests = (iActiveRebellions / iActivePlayers);

		//If there are more quests active than 20% of all civs, the quest should abort, as that's too many for the AI to handle.
		if(iMaxQuests >= 2)
		{
			return false;
		}
	}
	return true;
}

bool CvMinorCivAI::IsRebellionActive() const
{
	return m_bIsRebellionActive;
}

void CvMinorCivAI::SetRebellionActive(bool bValue)
{
	if(m_bIsRebellionActive != bValue)
	{
		m_bIsRebellionActive = bValue;
	}
}

bool CvMinorCivAI::IsHordeActive() const
{
	return m_bIsHordeActive;
}

void CvMinorCivAI::SetHordeActive(bool bValue)
{
	if(m_bIsHordeActive != bValue)
	{
		m_bIsHordeActive = bValue;
	}
}

//Cooldown
void CvMinorCivAI::ChangeCooldownSpawn(int iChange)
{
	SetCooldownSpawn(GetCooldownSpawn() + iChange);
}

int CvMinorCivAI::GetCooldownSpawn() const
{
	return m_iCooldownSpawn;
}

void CvMinorCivAI::SetCooldownSpawn(int iValue)
{
	if(GetCooldownSpawn() != iValue)
		m_iCooldownSpawn = iValue;
}

/// Is this an acceptable target for an aggressive City-State quest?
bool CvMinorCivAI::IsAcceptableQuestEnemy(MinorCivQuestTypes eQuest, PlayerTypes ePlayer, PlayerTypes eEnemyPlayer)
{
	TeamTypes ePlayerTeam = GET_PLAYER(ePlayer).getTeam();
	if (ePlayerTeam == GET_PLAYER(eEnemyPlayer).getTeam())
		return false;
	if (GET_PLAYER(ePlayer).IsAtWarWith(eEnemyPlayer))
		return true;
	if (!GET_TEAM(ePlayerTeam).isHasMet(GET_PLAYER(eEnemyPlayer).getTeam()))
		return false;

	vector<PlayerTypes> vPlayerTeam = GET_TEAM(ePlayerTeam).getPlayers();

	if (eQuest == MINOR_CIV_QUEST_WAR || eQuest == MINOR_CIV_QUEST_LIBERATION || eQuest == MINOR_CIV_QUEST_ACQUIRE_CITY) // Quests which require going to war
	{
		vector<PlayerTypes> vDefensiveWarAllies = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetDefensiveWarAllies(eEnemyPlayer, /*bIncludeMinors*/ false, /*bReverseMode*/ true, /*bNewWarsOnly*/ true);
		vDefensiveWarAllies.push_back(eEnemyPlayer);
		for (size_t i=0; i<vPlayerTeam.size(); i++)
		{
			if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
				continue;

			for (size_t j=0; j<vDefensiveWarAllies.size(); j++)
			{
				if (!GET_PLAYER(vDefensiveWarAllies[j]).isAlive() || !GET_PLAYER(vDefensiveWarAllies[j]).isMajorCiv())
					continue;

				CvDiplomacyAI* pDiplo = GET_PLAYER(vPlayerTeam[i]).GetDiplomacyAI();
				CvDiplomacyAI* pOtherDiplo = GET_PLAYER(vDefensiveWarAllies[j]).GetDiplomacyAI();
				if (pDiplo->IsDoFAccepted(vDefensiveWarAllies[j]) || pDiplo->IsHasDefensivePact(vDefensiveWarAllies[j]) ||
					pDiplo->IsVassal(vDefensiveWarAllies[j]) || pDiplo->IsMaster(vDefensiveWarAllies[j]) ||
					pDiplo->WasResurrectedBy(vDefensiveWarAllies[j]) || pOtherDiplo->WasResurrectedBy(vPlayerTeam[i]) ||
					(pOtherDiplo->IsDoFBroken(vPlayerTeam[i]) && pOtherDiplo->GetTurnsSinceDoFBroken(vPlayerTeam[i]) < /*10*/ GD_INT_GET(DOF_BROKEN_BACKSTAB_TIMER)) ||
					pOtherDiplo->IsPlayerMadeMilitaryPromise(vPlayerTeam[i]) || pDiplo->GetGlobalCoopWarWithState(vDefensiveWarAllies[j]) >= COOP_WAR_STATE_PREPARING)
				{
					return false;
				}
			}
		}
	}
	else // Quests which don't require going to war
	{
		TeamTypes eEnemyTeam = GET_PLAYER(eEnemyPlayer).getTeam();
		if (GET_TEAM(ePlayerTeam).IsHasDefensivePact(eEnemyTeam) || GET_TEAM(ePlayerTeam).GetMaster() == eEnemyTeam || GET_TEAM(eEnemyTeam).GetMaster() == ePlayerTeam)
			return false;

		vector<PlayerTypes> vEnemyTeam = GET_TEAM(eEnemyTeam).getPlayers();
		for (size_t i=0; i<vPlayerTeam.size(); i++)
		{
			if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
				continue;

			for (size_t j=0; j<vEnemyTeam.size(); j++)
			{
				if (!GET_PLAYER(vEnemyTeam[j]).isAlive() || !GET_PLAYER(vEnemyTeam[j]).isMajorCiv())
					continue;

				CvDiplomacyAI* pDiplo = GET_PLAYER(vPlayerTeam[i]).GetDiplomacyAI();
				CvDiplomacyAI* pOtherDiplo = GET_PLAYER(vEnemyTeam[j]).GetDiplomacyAI();
				if (pDiplo->IsDoFAccepted(vEnemyTeam[j]) || pDiplo->WasResurrectedBy(vEnemyTeam[j]) || pOtherDiplo->WasResurrectedBy(vPlayerTeam[i]) ||
					(pOtherDiplo->IsDoFBroken(vPlayerTeam[i]) && pOtherDiplo->GetTurnsSinceDoFBroken(vPlayerTeam[i]) < /*10*/ GD_INT_GET(DOF_BROKEN_BACKSTAB_TIMER)) ||
					pDiplo->GetGlobalCoopWarWithState(vEnemyTeam[j]) >= COOP_WAR_STATE_PREPARING)
				{
					return false;
				}
			}
		}
	}

	return true;
}

/// Find a Resource that a Minor would want a major to connect
ResourceTypes CvMinorCivAI::GetNearbyResourceForQuest(PlayerTypes ePlayer)
{
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	bool bCanCrossOcean = GET_PLAYER(ePlayer).CanCrossOcean();
	CvPlot* pCapitalPlot = GET_PLAYER(ePlayer).getCapitalCity()->plot();
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvLandmass* pLandmass = GC.getMap().getLandmassById(pCapitalPlot->getLandmass());
	vector<ResourceTypes> veValidResources;

	// Loop through all Resources and see if they're useful
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes) iResourceLoop;
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);

		// Must not be a plain ol' bonus resource
		if (!pkResourceInfo || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			continue;

		// Player must be able to see and use this resource
		if (!GET_TEAM(eTeam).IsResourceRevealed(eResource) || !GET_TEAM(eTeam).IsResourceCityTradeable(eResource))
			continue;

		// Resource must exist on the map
		if (GC.getMap().getNumResources(eResource) <= 0)
			continue;

		// Player can't already have this resource
		if (GET_PLAYER(ePlayer).getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0)
			continue;

		// City-State can't already have access to this resource
		if (GetPlayer()->getNumResourceTotal(eResource, /*bIncludeImport*/ true) > 0
			|| GetPlayer()->getResourceInOwnedPlots(eResource) > 0)
		{
			continue;
		}

		// No banned resources
		if (pLeague && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY && pLeague->IsLuxuryHappinessBanned(eResource))
			continue;

		// The player's capital city's landmass must contain this resource, or the player must be able to cross the ocean
		if (bCanCrossOcean || pLandmass->getNumResources(eResource) == 0)
			continue;

		veValidResources.push_back(eResource);
	}

	// Didn't find any valid resources
	if (veValidResources.size() == 0)
		return NO_RESOURCE;
	
	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidResources.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidResources.size());
	return veValidResources[iRandIndex];
}

/// Best wonder for a Quest given to ePlayer?
BuildingTypes CvMinorCivAI::GetBestWorldWonderForQuest(PlayerTypes ePlayer, int iDuration)
{
	std::vector<int> allBuildingCount = GET_PLAYER(ePlayer).GetTotalBuildingCount();
	vector<BuildingTypes> veValidBuildings;
	int iCompletionThreshold = /*25*/ max(GD_INT_GET(MINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD), 0);
	int iCompletionMaxTurns = /*30*/ max(GD_INT_GET(MINOR_CIV_QUEST_WONDER_COMPLETION_MAX_TURNS), 0);
	if (iDuration > 0 && max(iDuration - 2, 1) < iCompletionMaxTurns)
		iCompletionMaxTurns = max(iDuration - 2, 1);

	// Loop through all Buildings and see if they're useful
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (pkBuildingInfo == NULL || pkBuildingInfo->IsDummy())
			continue;

		// Must be a World Wonder
		if (!isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
			continue;

		// Must not be a Corporation building
		if (pkBuildingInfo->GetBuildingClassInfo().getCorporationType() != NO_CORPORATION)
			continue;

		// Must not be mutually exclusive
		if (pkBuildingInfo->GetMutuallyExclusiveGroup() != -1)
			continue;

		bool bBad = false;
		const CvCivilizationInfo& thisCivInfo = *GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType());
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingClassTypes eLockedBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetLockedBuildingClasses(iI);

			if (eLockedBuildingClass != NO_BUILDINGCLASS)
			{
				BuildingTypes eLockedBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(eLockedBuildingClass));

				if (eLockedBuilding != NO_BUILDING)
				{
					bBad = true;
					break;
				}
			}
			if (pkBuildingInfo->IsBuildingClassNeededNowhere(iI))
			{
				BuildingTypes ePrereqBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(iI));

				if (ePrereqBuilding != NO_BUILDING)
				{
					bBad = true;
					break;
				}
			}
		}
		if (bBad)
			continue;

		// Someone CAN be building this wonder right now, but they can't be more than a certain % of the way done
		bool bFoundWonderTooFarAlong = false;
		int iHighestPercentCompleted = 0;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			int iCityLoop = 0;
			for (CvCity* pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					bFoundWonderTooFarAlong = true;
					break;
				}

				int iWonderProgress = pLoopCity->GetCityBuildings()->GetBuildingProduction(eBuilding);
				int iPercentCompleted = iWonderProgress * 100 / pLoopCity->getProductionNeeded(eBuilding);
				if (iCompletionThreshold > 0 && iPercentCompleted >= iCompletionThreshold)
				{
					bFoundWonderTooFarAlong = true;
					break;
				}
				if (iPercentCompleted > iHighestPercentCompleted)
					iHighestPercentCompleted = iPercentCompleted;
			}
			if (bFoundWonderTooFarAlong)
				break;
		}
		if (bFoundWonderTooFarAlong)
			continue;

		// At least one of the player's cities must be able to construct the Wonder in a reasonable amount of time
		// If the Wonder has already been partially completed, reduce the turn threshold accordingly
		int iMaxTurns = iCompletionMaxTurns;
		if (iMaxTurns > 0)
		{
			if (iHighestPercentCompleted > 0)
			{
				iMaxTurns *= 100 - iHighestPercentCompleted;
				iMaxTurns /= 100;
				if (iMaxTurns == 0)
					iMaxTurns = 1;
			}

			// Scale maximum turn amount with game speed
			iMaxTurns *= GC.getGame().getGameSpeedInfo().getConstructPercent();
			iMaxTurns /= 100;
			if (iMaxTurns == 0)
				iMaxTurns = 1;
		}

		bool bNoValidCity = true;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			// Don't look at cities in resistance, being razed, about to fall, or that can't construct the Wonder
			if (pLoopCity->IsResistance() || pLoopCity->IsRazing() || pLoopCity->isInDangerOfFalling(true))
				continue;

			if (!pLoopCity->isBuildingInQueue(eBuilding) && !pLoopCity->canConstruct(eBuilding,allBuildingCount))
				continue;

			// How many turns will it take to produce the Wonder? Is it a reasonable delay?
			if (iMaxTurns > 0 && pLoopCity->getProductionTurnsLeft(eBuilding, 0) > iMaxTurns)
				continue;

			bNoValidCity = false;
			break;
		}
		if (bNoValidCity)
			continue;

		veValidBuildings.push_back(eBuilding);
	}

	// Didn't find any valid Wonders
	if (veValidBuildings.size() == 0)
		return NO_BUILDING;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidBuildings.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidBuildings.size());
	return veValidBuildings[iRandIndex];
}

/// Best national wonder for a Quest given to ePlayer?
BuildingTypes CvMinorCivAI::GetBestNationalWonderForQuest(PlayerTypes ePlayer, int iDuration)
{
	std::vector<int> allBuildingCount = GET_PLAYER(ePlayer).GetTotalBuildingCount();
	vector<BuildingTypes> veValidBuildings;
	int iCompletionMaxTurns = iDuration > 0 ? max(iDuration - 2, 1) : 0;

	// Loop through all Buildings and see if they're useful
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (pkBuildingInfo == NULL || pkBuildingInfo->IsDummy())
			continue;

		// Must be a National Wonder
		if (!isNationalWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
			continue;

		if (GC.getBuildingClassInfo(pkBuildingInfo->GetBuildingClassType())->getMaxPlayerInstances() > 1)
			continue;

		if (GC.getBuildingClassInfo(pkBuildingInfo->GetBuildingClassType())->getMaxTeamInstances() > 1)
			continue;

		// Must not be a Corporation building
		if (pkBuildingInfo->GetBuildingClassInfo().getCorporationType() != NO_CORPORATION)
			continue;

		// Must not be mutually exclusive
		if (pkBuildingInfo->GetMutuallyExclusiveGroup() != -1)
			continue;

		bool bBad = false;
		const CvCivilizationInfo& thisCivInfo = *GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType());
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingClassTypes eLockedBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetLockedBuildingClasses(iI);

			if (eLockedBuildingClass != NO_BUILDINGCLASS)
			{
				BuildingTypes eLockedBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(eLockedBuildingClass));

				if (eLockedBuilding != NO_BUILDING)
				{
					bBad = true;
					break;
				}
			}
			if (pkBuildingInfo->IsBuildingClassNeededNowhere(iI))
			{
				BuildingTypes ePrereqBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(iI));

				if (ePrereqBuilding != NO_BUILDING)
				{
					bBad = true;
					break;
				}
			}
		}
		if (bBad)
			continue;

		bool bNoValidCity = true;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				break;

			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			// Don't look at cities in resistance, being razed, about to fall, or that can't construct the Wonder
			if (pLoopCity->IsResistance() || pLoopCity->IsRazing() || pLoopCity->isInDangerOfFalling(true))
				continue;

			if (!pLoopCity->isBuildingInQueue(eBuilding) && !pLoopCity->canConstruct(eBuilding,allBuildingCount))
				continue;

			// How many turns will it take to produce the Wonder?
			if (iCompletionMaxTurns > 0 && pLoopCity->getProductionTurnsLeft(eBuilding, 0) > iCompletionMaxTurns)
				continue;

			bNoValidCity = false;
			break;
		}
		if (bNoValidCity)
			continue;

		veValidBuildings.push_back(eBuilding);
	}

	// Didn't find any valid National Wonders
	if (veValidBuildings.size() == 0)
		return NO_BUILDING;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidBuildings.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidBuildings.size());
	return veValidBuildings[iRandIndex];
}

/// Anyone that this City State would want ePlayer to liberate?
PlayerTypes CvMinorCivAI::GetBestCityStateLiberate(PlayerTypes ePlayer)
{
	vector<PlayerTypes> veValidTargets;

	for (int iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;

		if (GET_PLAYER(eTarget).isAlive() || !GET_PLAYER(eTarget).isMinorCiv())
			continue;

		if (GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		int iX = GET_PLAYER(eTarget).GetOriginalCapitalX();
		int iY = GET_PLAYER(eTarget).GetOriginalCapitalY();
		CvPlot* pPlot = GC.getMap().plot(iX, iY);
		if (pPlot == NULL || !pPlot->isCity())
			continue;

		// Plot not revealed?
		if (!pPlot->isRevealed(GET_PLAYER(ePlayer).getTeam()))
			continue;

		// Already liberated this city before?
		if (pPlot->getPlotCity()->isEverLiberated(ePlayer))
			continue;

		// Can't liberate this city?
		if (GET_PLAYER(ePlayer).GetPlayerToLiberate(pPlot->getPlotCity()) != eTarget)
			continue;

		PlayerTypes eCityOwner = GET_PLAYER(pPlot->getOwner()).GetID();
		TeamTypes eCityOwnerTeam = GET_PLAYER(eCityOwner).getTeam();

		if (eCityOwnerTeam == GET_PLAYER(eTarget).getTeam())
			continue;

		if (eCityOwnerTeam == m_pPlayer->getTeam())
			continue;

		if (eCityOwnerTeam == GET_PLAYER(ePlayer).getTeam())
			continue;

		if (GetAlly() != NO_PLAYER && eCityOwnerTeam == GET_PLAYER(GetAlly()).getTeam())
			continue;

		if (GET_PLAYER(ePlayer).GetProximityToPlayer(eTarget) == PLAYER_PROXIMITY_DISTANT)
			continue;

		TeamTypes eConqueredTeam = GET_PLAYER(eTarget).getTeam();
		if (eConqueredTeam == NO_TEAM)
			continue;

		TeamTypes eConquerorTeam = GET_TEAM(eConqueredTeam).GetKilledByTeam();
		if (eConquerorTeam == GET_PLAYER(ePlayer).getTeam())
			continue;

		if (!GET_PLAYER(ePlayer).IsAtWarWith(eCityOwner) && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(eCityOwnerTeam, ePlayer))
			continue;

		if (GET_PLAYER(eCityOwner).isMajorCiv())
		{
			if (!IsAcceptableQuestEnemy(MINOR_CIV_QUEST_LIBERATION, ePlayer, eCityOwner))
				continue;
		}
		else if (GET_PLAYER(eCityOwner).isMinorCiv())
		{
			PlayerTypes eCityOwnerAlly = GET_PLAYER(eCityOwner).GetMinorCivAI()->GetAlly();
			if (eCityOwnerAlly != NO_PLAYER && GET_PLAYER(eCityOwnerAlly).getTeam() == GET_PLAYER(ePlayer).getTeam())
				continue;

			vector<PlayerTypes> vPlayerTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getPlayers();
			for (size_t i=0; i<vPlayerTeam.size(); i++)
			{
				if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
					continue;

				if (GET_PLAYER(eCityOwner).GetMinorCivAI()->IsProtectedByMajor(vPlayerTeam[i]))
					continue;
			}
		}

		veValidTargets.push_back(eTarget);
	}

	// Didn't find any valid Target players
	if (veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidTargets.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidTargets.size());
	return veValidTargets[iRandIndex];
}

/// Find a Great Person that a Minor would want a major to spawn
UnitTypes CvMinorCivAI::GetBestGreatPersonForQuest(PlayerTypes ePlayer)
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");
	vector<UnitTypes> veValidUnits;

	// Loop through all Units and see if they're useful
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo == NULL)
			continue;

		// Can't be able to train it
		if (pkUnitInfo->GetProductionCost() != -1)
			continue;

		// Must be a Great Person (defined in SpecialUnitType in Unit XML)
		if (pkUnitInfo->GetSpecialUnitType() != eSpecialUnitGreatPerson)
			continue;

		// Must be a Great Person that can be spawned in the current game
		if (GC.getGame().isOption(GAMEOPTION_NO_RELIGION) && pkUnitInfo->IsFoundReligion())
			continue;

		if (GC.getGame().isOption(GAMEOPTION_NO_SCIENCE) && pkUnitInfo->GetBaseBeakersTurnsToCount() > 0)
			continue;

		// Must be a Great Person for this player's civ
		if (!GET_PLAYER(ePlayer).canTrainUnit(eUnit, /*bContinue*/ false, /*bTestVisible*/ false, /*bIgnoreCost*/ true))
			continue;

		if (pkUnitInfo->GetDefaultUnitAIType() == UNITAI_ADMIRAL)
		{
			if (GET_PLAYER(ePlayer).getNavalCombatExperienceTimes100() <= 0)
				continue;
		}
		else if (pkUnitInfo->GetDefaultUnitAIType() == UNITAI_GENERAL)
		{
			if (GET_PLAYER(ePlayer).getCombatExperienceTimes100() <= 0)
				continue;
		}
		else
		{
			// Will only ask for GPs they can produce.
			bool bGood = false;
			for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
			{
				const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
				CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);

				// Does this Specialist spawn a GP?
				if (pkSpecialistInfo && pkSpecialistInfo->getGreatPeopleUnitClass() == pkUnitInfo->GetUnitClassType())
				{
					if (GET_PLAYER(ePlayer).getCapitalCity()->GetCityCitizens()->GetSpecialistGreatPersonProgress(eSpecialist) > 0)
					{
						bGood = true;
						break;
					}
				}
			}
			if (!bGood)
				continue;
		}

		// Player can't already have this Unit
		bool bAlreadyHasUnit = false;
		int iLoop = 0;
		for (CvUnit* pLoopUnit = GET_PLAYER(ePlayer).firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = GET_PLAYER(ePlayer).nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitType() == eUnit)
			{
				bAlreadyHasUnit = true;
				break;
			}
		}
		if (bAlreadyHasUnit)
			continue;

		veValidUnits.push_back(eUnit);
	}

	// Didn't find any valid Units
	if (veValidUnits.size() == 0)
		return NO_UNIT;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidUnits.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidUnits.size());
	return veValidUnits[iRandIndex];
}

/// Anyone that this City State would want to kill / bully?
PlayerTypes CvMinorCivAI::GetBestCityStateTarget(PlayerTypes ePlayer, bool bKillQuest)
{
	CvWeightedVector<PlayerTypes> veValidTargets;
	vector<PlayerTypes> veTargetChoices;
	vector<PlayerTypes> vPlayerTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getPlayers();

	for (int iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;

		if (!GET_PLAYER(eTarget).isAlive() || !GET_PLAYER(eTarget).isMinorCiv() || !GET_PLAYER(eTarget).getCapitalCity() || !GET_PLAYER(eTarget).getCapitalCity()->plot())
			continue;

		if (GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		PlayerProximityTypes eProximity = GetPlayer()->GetProximityToPlayer(eTarget);
		if (MOD_BALANCE_VP && bKillQuest)
		{
			if (eProximity != PLAYER_PROXIMITY_NEIGHBORS)
				continue;

			// Don't pick CS that are allied to anyone - only unallied & ally-able CS for both target ends.
			if (GET_PLAYER(eTarget).GetMinorCivAI()->GetAlly() != NO_PLAYER || GET_PLAYER(eTarget).GetMinorCivAI()->IsNoAlly())
				continue;

			// Only one CS war at a time globally.
			for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				PlayerTypes eOtherMinor = (PlayerTypes) iMinorLoop;
				if (eOtherMinor != eTarget && GET_PLAYER(eTarget).IsAtWarWith(eOtherMinor))
					continue;
			}
		}
		else
		{
			if (eProximity < PLAYER_PROXIMITY_CLOSE)
				continue;

			if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eTarget).getTeam()))
				continue;

			// Don't give a quest to attack a player's own ally / PTP
			PlayerTypes eTargetAlly = GET_PLAYER(eTarget).GetMinorCivAI()->GetAlly();
			if (bKillQuest)
			{
				if (eTargetAlly != NO_PLAYER && GET_PLAYER(eTargetAlly).getTeam() == GET_PLAYER(ePlayer).getTeam())
					continue;

				if (!GET_PLAYER(ePlayer).IsAtWarWith(eTarget) && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(GET_PLAYER(eTarget).getTeam(), ePlayer))
					continue;

				for (size_t i=0; i<vPlayerTeam.size(); i++)
				{
					if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
						continue;

					if (GET_PLAYER(eTarget).GetMinorCivAI()->IsProtectedByMajor(vPlayerTeam[i]))
						continue;
				}
			}
			else
			{
				if (eTargetAlly == ePlayer)
					continue;

				if (GET_PLAYER(eTarget).GetMinorCivAI()->IsProtectedByMajor(ePlayer))
					continue;
			}
		}

		int iWeight = 0;
		switch (GET_PLAYER(eTarget).GetMinorCivAI()->GetTrait())
		{
		case MINOR_CIV_TRAIT_MILITARISTIC:
			iWeight = 14;
			break;
		case MINOR_CIV_TRAIT_CULTURED:
			iWeight = 8;
			break;
		case MINOR_CIV_TRAIT_MERCANTILE:
			iWeight = 6;
			break;
		case MINOR_CIV_TRAIT_MARITIME:
			iWeight = 4;
			break;
		case MINOR_CIV_TRAIT_RELIGIOUS:
		{
			ReligionTypes eMyReligion = GetPlayer()->getCapitalCity()->GetCityReligions()->GetReligiousMajority();
			ReligionTypes eTheirReligion = GET_PLAYER(eTarget).getCapitalCity()->GetCityReligions()->GetReligiousMajority();
			if (eMyReligion > RELIGION_PANTHEON && eTheirReligion > RELIGION_PANTHEON)
			{
				if (eMyReligion != eTheirReligion)
					iWeight = 12;
				else
					iWeight = 2;
			}
			else if (eTheirReligion > RELIGION_PANTHEON)
				iWeight = 10;
			else
				iWeight = 4;

			break;
		}
		default:
			UNREACHABLE();
		}

		switch (GET_PLAYER(eTarget).GetMinorCivAI()->GetPersonality())
		{
		case MINOR_CIV_PERSONALITY_FRIENDLY:
			iWeight /= 2;
			break;
		case MINOR_CIV_PERSONALITY_HOSTILE:
			iWeight *= 2;
			break;
		case MINOR_CIV_PERSONALITY_IRRATIONAL:
			iWeight *= 3;
			iWeight /= 2;
			break;
		default:
			break;
		}

		iWeight *= GET_PLAYER(eTarget).getNumCities();

		if (eProximity == PLAYER_PROXIMITY_NEIGHBORS)
		{
			if (!MOD_BALANCE_VP || !bKillQuest)
				iWeight *= 2;

			veValidTargets.push_back(eTarget, iWeight);
		}
		else if (eProximity == PLAYER_PROXIMITY_CLOSE)
		{
			veValidTargets.push_back(eTarget, iWeight);
		}
	}

	// Didn't find any valid Target players
	if (veValidTargets.size() == 0)
		return NO_PLAYER;

	// Add WEIGHT copies of each target into the second vector from which the pseudorandom choice will be made
	for (int iTargetLoop = 0; iTargetLoop < veValidTargets.size(); iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) veValidTargets.GetElement(iTargetLoop);
		int iCount = veValidTargets.GetWeight(iTargetLoop);
		for (int iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
		{
			veTargetChoices.push_back(eTarget);
		}
	}

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veTargetChoices.size(), m_pPlayer->GetPseudoRandomSeed());
	return veTargetChoices[iRandIndex];
}

CvCity* CvMinorCivAI::GetBestCityForQuest(PlayerTypes ePlayer)
{
	int iMinDistance = INT_MAX;
	CvCity* pBestCity = NULL;
	vector<PlayerTypes> vPlayerTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getPlayers();

	for (int iTargetLoop = 0; iTargetLoop < MAX_MAJOR_CIVS; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;

		if (!GET_PLAYER(eTarget).isAlive())
			continue;

		// Don't commission war against ourselves!
		if (GET_PLAYER(eTarget).getTeam() == GetPlayer()->getTeam())
			continue;

		// Or our ally!
		if (GetAlly() != NO_PLAYER && GET_PLAYER(GetAlly()).getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		// Can't go to war with this player.
		if (!GET_PLAYER(ePlayer).IsAtWarWith(eTarget) && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(GET_PLAYER(eTarget).getTeam(), ePlayer))
			continue;

		// Is the city owner an unacceptable target (same team, haven't met, backstabbing)?
		if (GET_PLAYER(eTarget).isMajorCiv())
		{
			if (!IsAcceptableQuestEnemy(MINOR_CIV_QUEST_ACQUIRE_CITY, ePlayer, eTarget))
				continue;
		}
		else if (GET_PLAYER(eTarget).isMinorCiv())
		{
			PlayerTypes eTargetAlly = GET_PLAYER(eTarget).GetMinorCivAI()->GetAlly();
			if (eTargetAlly != NO_PLAYER && GET_PLAYER(eTargetAlly).getTeam() == GET_PLAYER(ePlayer).getTeam())
				continue;

			for (size_t i=0; i<vPlayerTeam.size(); i++)
			{
				if (!GET_PLAYER(vPlayerTeam[i]).isAlive() || !GET_PLAYER(vPlayerTeam[i]).isMajorCiv())
					continue;

				if (GET_PLAYER(eTarget).GetMinorCivAI()->IsProtectedByMajor(vPlayerTeam[i]))
					continue;
			}
		}

		if (GET_PLAYER(ePlayer).GetProximityToPlayer(eTarget) < PLAYER_PROXIMITY_CLOSE)
			continue;

		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(eTarget).firstCity(&iCityLoop, true); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTarget).nextCity(&iCityLoop, true))
		{
			// Not the capital
			if (pLoopCity->isCapital())
				continue;

			// Must be revealed
			if (!pLoopCity->plot()->isRevealed(GET_PLAYER(ePlayer).getTeam()))
				continue;

			// Must not be previously owned by ePlayer
			if (pLoopCity->GetNumTimesOwned(ePlayer) > 0)
				continue;

			// Check for other minors that are currently targeting this city
			bool bBad = false;
			for (int iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes) iTargetLoop;

				if (!GET_PLAYER(eMinor).isAlive())
					continue;

				if (GetPlayer()->getTeam() == GET_PLAYER(eMinor).getTeam())
					continue;

				if (!GET_PLAYER(eMinor).isMinorCiv())
					continue;

				if (GET_PLAYER(eMinor).GetMinorCivAI()->GetTargetedCityX(ePlayer) == pLoopCity->getX() && GET_PLAYER(eMinor).GetMinorCivAI()->GetTargetedCityY(ePlayer) == pLoopCity->getY())
				{
					bBad = true;
					break;
				}
			}
			if (bBad)
				continue;

			int iDistance = GET_PLAYER(ePlayer).GetCityDistancePathLength(pLoopCity->plot()) + m_pPlayer->GetCityDistancePathLength(pLoopCity->plot());
			if (iDistance < iMinDistance)
			{
				iMinDistance = iDistance;
				pBestCity = pLoopCity;
			}
		}
	}

	return pBestCity;
}

BuildingTypes CvMinorCivAI::GetBestBuildingForQuest(PlayerTypes ePlayer, int iDuration)
{
	// Have nowhere to build.
	if (GET_PLAYER(ePlayer).getNumCities() <= 3 || GET_PLAYER(ePlayer).GetPlayerTraits()->IsNoAnnexing())
		return NO_BUILDING;

	std::vector<int> allBuildingCount = GET_PLAYER(ePlayer).GetTotalBuildingCount();
	vector<BuildingTypes> veValidBuildings;
	int iCompletionMaxTurns = iDuration > 0 ? max(iDuration - 2, 1) : 0;

	// Loop through all Buildings and see if they're useful
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (pkBuildingInfo == NULL || pkBuildingInfo->IsDummy())
			continue;

		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(pkBuildingInfo->GetBuildingClassType());
		if (pkBuildingClassInfo == NULL)
			continue;

		// Must not be a Wonder
		if (isLimitedWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
			continue;

		// Must not be a Corporation building
		if (pkBuildingInfo->GetBuildingClassInfo().getCorporationType() != NO_CORPORATION)
			continue;

		// Must not be mutually exclusive
		if (pkBuildingInfo->GetMutuallyExclusiveGroup() != -1)
			continue;

		bool bBad = false;
		const CvCivilizationInfo& thisCivInfo = *GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType());
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingClassTypes eLockedBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetLockedBuildingClasses(iI);

			if (eLockedBuildingClass != NO_BUILDINGCLASS)
			{
				BuildingTypes eLockedBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(eLockedBuildingClass));

				if (eLockedBuilding != NO_BUILDING)
				{
					bBad = true;
					break;
				}
			}
			if (pkBuildingInfo->IsBuildingClassNeededNowhere(iI))
			{
				BuildingTypes ePrereqBuilding = (BuildingTypes)(thisCivInfo.getCivilizationBuildings(iI));

				if (ePrereqBuilding != NO_BUILDING)
				{
					bBad = true;
					break;
				}
			}
		}
		if (bBad)
			continue;

		// Must be able to build it in all non-puppet cities.
		int iNumCitiesAlreadyBuilt = 0;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop, true); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop, true))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				iNumCitiesAlreadyBuilt++;

				// Haven't built this building in more than one city already.
				if (iNumCitiesAlreadyBuilt > 1)
				{
					bBad = true;
					break;
				}

				continue;
			}
			if (!pLoopCity->isBuildingInQueue(eBuilding) && !pLoopCity->canConstruct(eBuilding,allBuildingCount))
			{
				bBad = true;
				break;
			}
			// How many turns will it take to produce the building?
			if (iCompletionMaxTurns > 0 && pLoopCity->getProductionTurnsLeft(eBuilding, 0) > iCompletionMaxTurns)
			{
				bBad = true;
				break;
			}
		}
		if (bBad)
			continue;

		veValidBuildings.push_back(eBuilding);
	}

	// Didn't find any valid buildings
	if (veValidBuildings.size() == 0)
		return NO_BUILDING;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidBuildings.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidBuildings.size());
	return veValidBuildings[iRandIndex];
}

UnitTypes CvMinorCivAI::GetBestUnitGiftFromPlayer(PlayerTypes ePlayer)
{
	CvWeightedVector<UnitTypes> veValidUnits;
	vector<UnitTypes> veUnitChoices;
	bool bAllowNaval = GetPlayer()->getCapitalCity()->isCoastal(10) && GET_PLAYER(ePlayer).GetNumEffectiveCoastalCities() > 1;

	// Loop through all Unit Classes
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eLoopUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if (pkUnitInfo == NULL)
			continue;

		UnitAITypes eUnitAI = pkUnitInfo->GetDefaultUnitAIType();
		const UnitClassTypes eUnitClass = (UnitClassTypes)pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if (pkUnitClassInfo == NULL || GetPlayer()->GetSpecificUnitType(eUnitClass) == NO_UNIT)
			continue;

		// No planes, no naval units unless allowed, and no siege units
		DomainTypes eDomain = pkUnitInfo->GetDomainType();
		if (eDomain == DOMAIN_AIR)
			continue;
		if (eDomain == DOMAIN_SEA && !bAllowNaval)
			continue;
		if (eDomain == DOMAIN_LAND && eUnitAI == UNITAI_CITY_BOMBARD)
			continue;

		// No units that can found cities
		if (pkUnitInfo->IsFound() || pkUnitInfo->IsFoundAbroad() || pkUnitInfo->GetNumColonyFound() > 0)
			continue;

		// No non-combat units
		if (pkUnitInfo->GetCombat() <= 0)
			continue;

		// No recon units
		if (eUnitAI == UNITAI_EXPLORE || eUnitAI == UNITAI_EXPLORE_SEA)
			continue;

		bool bValid = true;
		for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
		{
			const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if (pkPromotionInfo && pkUnitInfo->GetFreePromotions(iLoop))
			{
				if (pkPromotionInfo->IsOnlyDefensive() || pkPromotionInfo->IsHoveringUnit())
				{
					bValid = false;
					break;
				}
			}
		}
		if (!bValid)
			continue;

		// Can we train the unit type this unit upgrades to?
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
			if (pkUnitClassInfo && pkUnitInfo->GetUpgradeUnitClass(iI))
			{
				UnitTypes eUpgradeUnit = GET_PLAYER(ePlayer).GetSpecificUnitType(eUnitClass);
				if (GET_PLAYER(ePlayer).canTrainUnit(eUpgradeUnit, false, false, false, false))
				{
					bValid = false;
					break;
				}
			}
		}
		if (!bValid)
			continue;

		// Make sure at least one of the player's cities can train this unit
		bValid = false;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			// Don't look at cities in resistance, being razed, about to fall, or that can't train the unit
			if (pLoopCity->IsResistance() || pLoopCity->IsRazing() || pLoopCity->isInDangerOfFalling(true))
				continue;

			if (!pLoopCity->canTrain(eLoopUnit, false, false, false, false))
				continue;

			bValid = true;
			break;
		}
		if (!bValid)
			continue;

		int iValue = 11;

		// If this is a UU, add extra value so that the unique unit is more likely to get picked
		if (eLoopUnit != pkUnitClassInfo->getDefaultUnitIndex())
			iValue += 10;

		// Reduce value if the unit already has similar characteristics to the City-State's existing units
		int iLoop = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			// Don't count civilians or exploration units
			if (pLoopUnit->IsCivilianUnit() || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				continue;

			if (pLoopUnit->getDomainType() == pkUnitInfo->GetDomainType() && bAllowNaval)
			{
				iValue -= 2;
			}
			if (pLoopUnit->IsCanAttackRanged() == (pkUnitInfo->GetRange() > 0 && pkUnitInfo->GetRangedCombat() > 0))
			{
				iValue -= 2;
			}
			if (pLoopUnit->getUnitInfo().GetMoves() > 2 == pkUnitInfo->GetMoves() > 2)
			{
				iValue -= 2;
			}
		}

		veValidUnits.push_back(eLoopUnit, max(iValue, 1));
	}

	// Didn't find any valid units
	if (veValidUnits.size() == 0)
		return NO_UNIT;

	// Add WEIGHT copies of each unit into the second vector from which the pseudorandom choice will be made
	for (int iUnitLoop = 0; iUnitLoop < veValidUnits.size(); iUnitLoop++)
	{
		UnitTypes eUnit = (UnitTypes) veValidUnits.GetElement(iUnitLoop);
		int iCount = veValidUnits.GetWeight(iUnitLoop);
		for (int iCountLoop = 0; iCountLoop < iCount; iCountLoop++)
		{
			veUnitChoices.push_back(eUnit);
		}
	}

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veUnitChoices.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veUnitChoices.size());
	return veUnitChoices[iRandIndex];
}

int CvMinorCivAI::GetExperienceForUnitGiftQuest(PlayerTypes ePlayer, UnitTypes eUnitType)
{
	int iExperience = 0;
	int iLoop = 0;
	for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if (!pLoopCity->IsPuppet() && !pLoopCity->IsRazing())
		{
			iExperience = max(iExperience, pLoopCity->getProductionExperience(eUnitType));
		}
	}
	return iExperience;
}

bool CvMinorCivAI::IsUnitValidGiftForCityStateQuest(PlayerTypes ePlayer, CvUnit* pUnit)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS)
		return false;

	if (pUnit == NULL)
		return false;

	if (IsActiveQuestForPlayer(ePlayer, MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT))
	{
		if ((UnitTypes)GetQuestData1(ePlayer, MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT) == pUnit->getUnitType() && GetQuestData2(ePlayer, MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT) * 100 <= pUnit->getExperienceTimes100())
		{
			return true;
		}
	}
	return false;
}

bool CvMinorCivAI::GetHasSentUnitForQuest(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	return m_abSentUnitForQuest[ePlayer];
}
void CvMinorCivAI::SetHasSentUnitForQuest(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if (GetHasSentUnitForQuest(ePlayer) != bValue)
	{
		m_abSentUnitForQuest[ePlayer] = bValue;
	}
}
void CvMinorCivAI::SetCoupAttempted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(IsCoupAttempted(ePlayer) != bValue)
	{
		m_abCoupAttempted[ePlayer] = bValue;
	}
}
bool CvMinorCivAI::IsCoupAttempted(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= REALLY_MAX_PLAYERS) return false;  // as defined in Reset()
	return m_abCoupAttempted[ePlayer];
}
void CvMinorCivAI::SetTargetedAreaID(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(iValue != m_aiAssignedPlotAreaID[ePlayer])
	{
		m_aiAssignedPlotAreaID[ePlayer] = iValue;
	}
}
int CvMinorCivAI::GetTargetedAreaID(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= REALLY_MAX_PLAYERS) return -1;  // as defined in Reset()
	return m_aiAssignedPlotAreaID[ePlayer];
}
void CvMinorCivAI::SetTargetedCityX(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(iValue != m_aiTargetedCityX[ePlayer])
	{
		m_aiTargetedCityX[ePlayer] = iValue;
	}
}
int CvMinorCivAI::GetTargetedCityX(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= REALLY_MAX_PLAYERS) return -1;  // as defined in Reset()
	return m_aiTargetedCityX[ePlayer];
}
void CvMinorCivAI::SetTargetedCityY(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(iValue != m_aiTargetedCityY[ePlayer])
	{
		m_aiTargetedCityY[ePlayer] = iValue;
	}
}
int CvMinorCivAI::GetTargetedCityY(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= REALLY_MAX_PLAYERS) return -1;  // as defined in Reset()
	return m_aiTargetedCityY[ePlayer];
}
void CvMinorCivAI::SetNumTurnsSincePtPWarning(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(iValue != m_aiTurnsSincePtPWarning[ePlayer])
	{
		m_aiTurnsSincePtPWarning[ePlayer] = iValue;
	}
}
int CvMinorCivAI::GetNumTurnsSincePtPWarning(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= REALLY_MAX_PLAYERS) return -1;  // as defined in Reset()
	return m_aiTurnsSincePtPWarning[ePlayer];
}
void CvMinorCivAI::ChangeNumTurnsSincePtPWarning(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "eForPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "eForPlayer is expected to be within maximum bounds (invalid Index)");
	
	if(iValue != 0)
	{
		m_aiTurnsSincePtPWarning[ePlayer] += iValue;
	}
}

CvCity* CvMinorCivAI::GetBestSpyTarget(PlayerTypes ePlayer, bool bMinor)
{
	if (GET_PLAYER(ePlayer).GetEspionage() == NULL || GET_PLAYER(ePlayer).GetEspionage()->GetNumSpies() <= 0)
		return NULL;

	vector<PlayerTypes> veValidTargets;
	uint iLoopStart = bMinor ? MAX_MAJOR_CIVS : 0;
	uint iLoopEnd = bMinor ? MAX_CIV_PLAYERS : MAX_MAJOR_CIVS;

	for (uint iTargetLoop = iLoopStart; iTargetLoop < iLoopEnd; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;

		if (!GET_PLAYER(eTarget).isAlive())
			continue;

		if (GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if (GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		if (bMinor)
		{
			if (!GET_PLAYER(eTarget).isMinorCiv())
				continue;

			if (GET_PLAYER(eTarget).GetMinorCivAI()->GetPermanentAlly() != NO_PLAYER)
				continue;

			if (GET_PLAYER(eTarget).GetMinorCivAI()->IsCoupAttempted(ePlayer))
				continue;

			PlayerTypes eAlly = GET_PLAYER(eTarget).GetMinorCivAI()->GetAlly();
			if (eAlly == NO_PLAYER || !IsAcceptableQuestEnemy(MINOR_CIV_QUEST_COUP, ePlayer, eAlly))
				continue;
		}
		else
		{
			if (GET_PLAYER(eTarget).isMinorCiv())
				continue;

			if (!IsAcceptableQuestEnemy(MINOR_CIV_QUEST_SPY_ON_MAJOR, ePlayer, eTarget))
				continue;
		}

		veValidTargets.push_back(eTarget);
	}

	// Didn't find any valid Target players
	if (veValidTargets.size() == 0)
		return NULL;

	int iBestValue = 0;
	CvCity* pBestCity = NULL;
	for (uint iTargetLoop = 0; iTargetLoop < veValidTargets.size(); iTargetLoop++)
	{
		PlayerTypes eTarget = veValidTargets[iTargetLoop];
	
		int iCityLoop;
		for (CvCity* pLoopCity = GET_PLAYER(eTarget).firstCity(&iCityLoop, true); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTarget).nextCity(&iCityLoop, true))
		{
			if (!GET_PLAYER(ePlayer).GetEspionage()->CanEverMoveSpyTo(pLoopCity))
				continue;

			if (!pLoopCity->plot()->isRevealed(GET_PLAYER(ePlayer).getTeam()))
				continue;

			if (GET_PLAYER(ePlayer).GetEspionage()->GetSpyIndexInCity(pLoopCity) != -1)
				continue;

			int iValue = pLoopCity->getPopulation();
			iValue += pLoopCity->getBaseYieldRate(YIELD_GOLD);
			iValue += pLoopCity->getBaseYieldRate(YIELD_SCIENCE);
			iValue *= max(1, pLoopCity->GetEspionageRanking() / 100);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestCity = pLoopCity;
			}
		}
	}

	return pBestCity;
}

int CvMinorCivAI::GetExplorePercent(PlayerTypes ePlayer, MinorCivQuestTypes eQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	int iPercent = 0;
	for(uint iQuestLoop = 0; iQuestLoop < m_QuestsGiven[ePlayer].size(); iQuestLoop++)
	{
		if(m_QuestsGiven[ePlayer][iQuestLoop].GetType() == eQuest)
		{
			int iX = m_QuestsGiven[ePlayer][iQuestLoop].m_iData1;
			int iY = m_QuestsGiven[ePlayer][iQuestLoop].m_iData2;
			CvPlot* pPlot = GC.getMap().plot(iX, iY);
			if(pPlot)
			{
				CvArea* pArea = pPlot->area();
				if(pArea)
				{
					if(pArea->getNumUnrevealedTiles(eTeam) <= 0)
						return 100;
					
					return MIN(100, (100 * pArea->getNumRevealedTiles(eTeam)) / pArea->getNumUnrevealedTiles(eTeam));
				}
			}
		}
	}
	return iPercent;
}

/// Find best (relatively) close unrevealed plot for this civ to discover.
CvPlot* CvMinorCivAI::GetTargetPlot(PlayerTypes ePlayer)
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	if (GET_TEAM(eTeam).isMapTrading())
		return NULL;

	int iWorldWidth = GC.getMap().getGridWidth() / 5;

	CvArea* pBestArea = NULL;
	CvPlot* pBestPlot = NULL;
	int iBestAreaValue = 0;
	int iBestPlotValue = 0;

	int iLoop = 0;
	for (CvArea* pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if (pLoopArea->isWater())
			continue;

		// Can't do super small islands!
		if (pLoopArea->getNumTiles() <= 3)
			continue;

		if (pLoopArea->getNumUnrevealedTiles(eTeam) <= 0)
			continue;

		// Not too close to starting area
		if (GET_PLAYER(ePlayer).getCapitalCity()->HasAccessToArea(pLoopArea->GetID()))
			continue;

		// Check for other minors that are currently targeting this area
		bool bBad = false;
		for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorLoop;

			if (eMinor == GetPlayer()->GetID() || !GET_PLAYER(eMinor).isAlive() || !GET_PLAYER(eMinor).isMinorCiv())
				continue;

			if (GET_PLAYER(eMinor).GetMinorCivAI()->GetTargetedAreaID(ePlayer) == pLoopArea->GetID())
			{
				bBad = true;
				break;
			}
		}
		if (bBad)
			continue;

		// >= 66% revealed? Next!
		int iPercent = ((100 * pLoopArea->getNumRevealedTiles(eTeam)) / pLoopArea->getNumUnrevealedTiles(eTeam));
		if (iPercent >= 66)
			continue;

		int iValue = pLoopArea->getNumTiles();
		if (iValue > iBestAreaValue)
		{
			bool bAreaHasValidPlot = false;
			for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
				if (pPlot && pPlot->getArea() == pLoopArea->GetID())
				{
					// We want a distant, unrevealed, unowned, non-coastal plot. Doesn't have to be fancy.
					if (pPlot->isRevealed(eTeam))
						continue;

					if (pPlot->getOwner() != NO_PLAYER)
						continue;

					if (pPlot->isCoastalLand())
						continue;

					// Do not pick a Natural Wonder because the quest reveals the tile, which automatically fulfills the "Find Natural Wonder" quest without any effort
					if (pPlot->IsNaturalWonder())
						continue;

					int iDistance = plotDistance(GET_PLAYER(ePlayer).getCapitalCity()->getX(), GET_PLAYER(ePlayer).getCapitalCity()->getY(), pPlot->getX(), pPlot->getY());
					if (iDistance < iWorldWidth)
						continue;

					if (!bAreaHasValidPlot)
					{
						iBestAreaValue = iValue;
						pBestArea = pLoopArea;
						iBestPlotValue = 0;
						pBestPlot = NULL;
						bAreaHasValidPlot = true;
					}

					iValue = 2;

					// However, plots adjacent to a Natural Wonder are more rewarding...give a big bonus
					// BUT, we want to avoid picking a plot adjacent to an already-revealed plot
					bool bAdjacentToRevealed = false;
					bool bAdjacentToNW = false;
					CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
					for (int iI=0; iI<NUM_DIRECTION_TYPES; iI++)
					{
						CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
						if (pAdjacentPlot != NULL)
						{
							if (pAdjacentPlot->isRevealed(eTeam))
							{
								bAdjacentToRevealed = true;
								iValue = 1;
								break;
							}
							if (pAdjacentPlot->IsNaturalWonder())
								bAdjacentToNW = true;
						}
					}
					if (!bAdjacentToRevealed)
					{
						if (bAdjacentToNW)
							iValue += 50;

						// Favor plots with resources
						if (pPlot->getResourceType(eTeam) != NO_RESOURCE)
							iValue += 30;

						// Favor "more interesting" plots
						if (pPlot->getFeatureType() != NO_FEATURE)
							iValue += 10;

						if (pPlot->isMountain())
							iValue += 15;
						else if (pPlot->isHills())
							iValue += 15;
						else if (pPlot->isLake())
							iValue += 10;
						else if (pPlot->isRiver())
							iValue += 5;
					}

					if (iValue > iBestPlotValue)
					{
						iBestPlotValue = iValue;
						pBestPlot = pPlot;
					}
				}
			}
		}
	}

	return pBestPlot;
}

// Returns the PlayerTypes enum of the most recent valid bully, NO_PLAYER if there isn't one
PlayerTypes CvMinorCivAI::GetMostRecentBullyForQuest() const
{
	PlayerTypes eBully = NO_PLAYER;
	int iTurn = -1;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		// Bully must still be alive
		if (!GET_PLAYER((PlayerTypes)iPlayerLoop).isAlive())
			continue;

		if (m_aiTurnLastBullied[iPlayerLoop] > iTurn)
		{
			iTurn = m_aiTurnLastBullied[iPlayerLoop];
			eBully = (PlayerTypes)iPlayerLoop;
		}
	}

	return eBully;
}

/// Has this minor asked any other player to go after eMinor?
bool CvMinorCivAI::IsWantsMinorDead(PlayerTypes eMinor)
{
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajor = (PlayerTypes) iMajorLoop;

		// Major must be alive
		if (!GET_PLAYER(eMajor).isAlive())
			continue;

		// Has the quest?
		if (!IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_KILL_CITY_STATE))
			continue;

		// Right minor?
		if (GetQuestData1(eMajor, MINOR_CIV_QUEST_KILL_CITY_STATE) != eMinor)
			continue;

		return true;
	}

	return false;
}

/// Any good players to ask ePlayer to find?
PlayerTypes CvMinorCivAI::GetBestPlayerToFind(PlayerTypes ePlayer)
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam* pTeam = &GET_TEAM(eTeam);
	vector<PlayerTypes> veValidTargets;

	// First, loop through majors and see if the player has met them
	for (int iTargetMajorLoop = 0; iTargetMajorLoop < MAX_MAJOR_CIVS; iTargetMajorLoop++)
	{
		PlayerTypes eTargetMajor = (PlayerTypes) iTargetMajorLoop;
		TeamTypes eTargetTeam = GET_PLAYER(eTargetMajor).getTeam();

		if (eTargetTeam == eTeam || !GET_PLAYER(eTargetMajor).isAlive() || !GET_PLAYER(eTargetMajor).isMajorCiv())
			continue;

		// Player must already know the target
		if (!pTeam->isHasMet(eTargetTeam))
			continue;

		// Player can't have seen this guy's territory before
		if (pTeam->IsHasFoundPlayersTerritory(eTargetMajor))
			continue;

		veValidTargets.push_back(eTargetMajor);
	}

	// Didn't find any valid players
	if (veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidTargets.size(), GET_PLAYER(ePlayer).getNumUnits() + GET_PLAYER(ePlayer).GetTreasury()->GetLifetimeGrossGold());
	return veValidTargets[iRandIndex];
}

/// Any good cities to ask ePlayer to find?
CvCity* CvMinorCivAI::GetBestCityToFind(PlayerTypes ePlayer)
{
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	if (GET_TEAM(eTeam).isMapTrading())
		return NULL;

	bool bCanCrossOcean = GET_PLAYER(ePlayer).CanCrossOcean();
	CvWeightedVector<CvCity*> CitiesSortedByDistance;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (!GET_PLAYER(eLoopPlayer).isAlive())
			continue;

		TeamTypes eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();
		if (eLoopTeam == eTeam)
			continue;

		// Must have met the owner of the city
		if (!GET_TEAM(eTeam).isHasMet(eLoopTeam))
			continue;

		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
		{
			// No major civ capitals, they're too easy to find (Accept Embassy)
			if (GET_PLAYER(eLoopPlayer).isMajorCiv() && pLoopCity->isCapital())
				continue;

			// No cities that are currently being destroyed
			if (pLoopCity->IsRazing())
				continue;

			// Must not be revealed yet
			CvPlot* pPlot = pLoopCity->plot();
			if (pPlot == NULL || pPlot->isRevealed(eTeam))
				continue;

			// We want an accessible city that's a decent distance away from this player's closest city
			CvLandmass* pLandmass = GC.getMap().getLandmassById(pPlot->getLandmass());
			if (bCanCrossOcean || pLandmass->getCitiesPerPlayer(ePlayer) > 0)
			{
				CvCity* pClosestCity = GET_PLAYER(ePlayer).GetClosestCityByPlots(pPlot);
				if (!pClosestCity)
					continue;

				int iDistance = plotDistance(pClosestCity->getX(), pClosestCity->getY(), pPlot->getX(), pPlot->getY());
				if (iDistance >= /*16*/ GD_INT_GET(MINOR_CIV_QUEST_FIND_CITY_MIN_DISTANCE))
				{
					int iScore = 1000 - iDistance;
					CitiesSortedByDistance.push_back(pLoopCity, iScore);
				}
			}
		}
	}

	// Didn't find any possibilities
	if (CitiesSortedByDistance.size() == 0)
		return NULL;

	// Select the closest city
	CitiesSortedByDistance.StableSortItems();
	return CitiesSortedByDistance.GetElement(0);
}

/// Natural Wonder available to find that's not TOO easy to find?
bool CvMinorCivAI::IsGoodTimeForNaturalWonderQuest(PlayerTypes ePlayer)
{
	CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());

	// No starting plot?
	if (pPlayer->getStartingPlot() == NULL)
		return false;

	// Player's already found them all
	if (pTeam->GetNumNaturalWondersDiscovered() >= GC.getMap().GetNumNaturalWonders())
		return false;

	// Player hasn't yet found all the NWs in his area
	int iNumNaturalWondersInStartingArea = GC.getMap().getAreaById(pPlayer->getStartingPlot()->getArea())->GetNumNaturalWonders();
	if (pPlayer->GetNumNaturalWondersDiscoveredInArea() < iNumNaturalWondersInStartingArea)
		return false;

	return true;
}

/// Any distant City States that this City State would want ePlayer to meet?
PlayerTypes CvMinorCivAI::GetBestCityStateMeetTarget(PlayerTypes ePlayer)
{
	CvTeam* pTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());
	vector<PlayerTypes> veValidTargets;

	// First, loop through the Minors in the game to what the closest proximity is to any of the players
	for (int iTargetLoop = MAX_MAJOR_CIVS; iTargetLoop < MAX_CIV_PLAYERS; iTargetLoop++)
	{
		PlayerTypes eTarget = (PlayerTypes) iTargetLoop;

		if (!GET_PLAYER(eTarget).isAlive())
			continue;

		if (GetPlayer()->getTeam() == GET_PLAYER(eTarget).getTeam())
			continue;

		// Player can't have seen this guy's territory or met them before
		if (pTeam->isHasMet(GET_PLAYER(eTarget).getTeam()))
			continue;

		if (pTeam->IsHasFoundPlayersTerritory(eTarget))
			continue;

		if (GetPlayer()->GetProximityToPlayer(eTarget) == PLAYER_PROXIMITY_NEIGHBORS)
			continue;

		veValidTargets.push_back(eTarget);
	}

	// Didn't find any valid Target players
	if (veValidTargets.size() == 0)
		return NO_PLAYER;

	int iRandIndex = GC.getGame().getSmallFakeRandNum(veValidTargets.size(), m_pPlayer->GetPseudoRandomSeed() + GET_PLAYER(ePlayer).GetPseudoRandomSeed() + veValidTargets.size());
	return veValidTargets[iRandIndex];
}


// ******************************
// ***** Friendship *****
// ******************************



/// Per-turn friendship stuff
void CvMinorCivAI::DoFriendship()
{
	Localization::String strMessage;
	Localization::String strSummary;
	const char* strMinorsNameKey = GetPlayer()->getNameKey();

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (!GET_PLAYER(ePlayer).isAlive() || !IsHasMetPlayer(ePlayer))
			continue;

		// Look at the base friendship (not counting war status etc.) and change it
		int iOldFriendship = GetBaseFriendshipWithMajor(ePlayer);
		int iChangeThisTurn = GetFriendshipChangePerTurnTimes100(ePlayer);
		int iFriendshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
		int iNewFriendship = iOldFriendship + (iChangeThisTurn / 100);

		if (iOldFriendship >= iFriendshipAnchor && iNewFriendship < iFriendshipAnchor)
		{
			// If we are at or above anchor, don't let the decay dip us below it
			SetFriendshipWithMajor(ePlayer, iFriendshipAnchor);
		}
		else if (iChangeThisTurn != 0)
		{
			ChangeFriendshipWithMajorTimes100(ePlayer, iChangeThisTurn);
		}
		else
		{
			// Friendship amount doesn't change, but ally state could have (ex. current ally decays below our level)
			DoFriendshipChangeEffects(ePlayer, iOldFriendship, iNewFriendship);
		}

		// Notification for status changes
		if (GetPlayer()->isAlive() && GetPermanentAlly() != ePlayer)
		{
			const int iTurnsWarning = 2;
			const int iAlliesThreshold = GetAlliesThreshold(ePlayer) * 100;
			const int iFriendsThreshold = GetFriendsThreshold(ePlayer) * 100;
			int iEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer);

			if (IsAllies(ePlayer))
			{
				if (iEffectiveFriendship + (iTurnsWarning * iChangeThisTurn) < iAlliesThreshold &&
					iEffectiveFriendship + ((iTurnsWarning-1) * iChangeThisTurn) >= iAlliesThreshold)
				{
					strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_ALLIES");
					strMessage << strMinorsNameKey;
					strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
					strSummary << strMinorsNameKey;

					AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
				}

				if (MOD_API_ACHIEVEMENTS && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman())
					gDLL->UnlockAchievement(ACHIEVEMENT_CITYSTATE_ALLY);
			}
			else if (IsFriends(ePlayer))
			{
				if (iEffectiveFriendship + (iTurnsWarning * iChangeThisTurn) < iFriendsThreshold &&
					iEffectiveFriendship + ((iTurnsWarning-1) * iChangeThisTurn) >= iFriendsThreshold)
				{
					strMessage = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_NOT_FRIENDS");
					strMessage << strMinorsNameKey;
					strSummary = Localization::Lookup("TXT_KEY_NTFN_CITY_STATE_ALMOST_SM");
					strSummary << strMinorsNameKey;

					AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), ePlayer);
				}
			}
		}
	}
}

/// How much does friendship drop per turn with ePlayer?
/// Note that this does not pay attention to whether ePlayer is at war
/// with this minor, in which case there is no friendship change per turn.
int CvMinorCivAI::GetFriendshipChangePerTurnTimes100(PlayerTypes ePlayer)
{
	if (ePlayer == NO_PLAYER || !GET_PLAYER(ePlayer).isMajorCiv() || GetPlayer()->getCapitalCity() == NULL)
		return 0;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iCurrentInfluence = GetBaseFriendshipWithMajorTimes100(ePlayer);
	int iRestingPoint = GetFriendshipAnchorWithMajor(ePlayer) * 100;
	int iUnitGiftInfluence = kPlayer.GetPlayerTraits()->GetMinorInfluencePerGiftedUnit();
	int iShift = 0;

	// Influence bonus from special sources (policies, etc.)
	if (GET_TEAM(kPlayer.getTeam()).isHasMet(GetPlayer()->getTeam()))
	{
		int iTradeRouteBonus = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PROTECTED_MINOR_INFLUENCE);
		if (MOD_BALANCE_CORE_MINORS && iTradeRouteBonus != 0)
		{
			if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(ePlayer, GetPlayer()->GetID()))
			{
				int iNumRoutes = min(kPlayer.GetTrade()->GetNumberOfCityStateTradeRoutes(), 5);
				iShift = iTradeRouteBonus * iNumRoutes;
			}
		}

		if (CanMajorBullyGold(ePlayer))
		{
			iShift += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_AFRAID_INFLUENCE);
			iShift += kPlayer.GetPlayerTraits()->GetAfraidMinorPerTurnInfluence();
		}

		// Bonus Influence from unit gifts
		if (iUnitGiftInfluence != 0)
		{
			int iLoop = 0;
			for (CvUnit* pLoopUnit = GetPlayer()->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iLoop))
			{
				if (!pLoopUnit->IsCombatUnit() || pLoopUnit->isDelayedDeath())
					continue;

				if (pLoopUnit->GetGiftedByPlayer() != ePlayer)
					continue;

				iShift += iUnitGiftInfluence * 100;
			}
		}
	}

	// Nothing to change!
	if (iCurrentInfluence == iRestingPoint)
		return iShift;

	int iChangeThisTurn = 0;

	// Below resting point? Influence goes up!
	if (iCurrentInfluence < iRestingPoint)
	{
		iChangeThisTurn += /*100*/ GD_INT_GET(MINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN);
		int iReligionBonus = IsSameReligionAsMajor(ePlayer) ? /*50*/ GD_INT_GET(MINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION) : 0;

		if (iChangeThisTurn > 0)
		{
			iChangeThisTurn *= (100 + iReligionBonus + kPlayer.GetPlayerTraits()->GetCityStateFriendshipModifier());
			iChangeThisTurn /= 100;
		}
		else
			iChangeThisTurn = 0;
	}
	// Above resting point? Influence goes down!
	else if (iCurrentInfluence > iRestingPoint)
	{
		if (GET_TEAM(kPlayer.getTeam()).IsMinorCivAggressor())
			iChangeThisTurn += /*-200*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR);
		else if (GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
			iChangeThisTurn += /*-150*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE);
		else
			iChangeThisTurn += /*-100*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_PER_TURN);

		if (MOD_BALANCE_VP)
		{
			if (iCurrentInfluence - iRestingPoint >= 10000)
			{
				//Influence decay increases the higher your influence over your resting point. 100 over resting point equals -1, 200 equals -2.82, 300 equals -5.2, 400 equals -8, 500 equals -11.18, and so on.
				double dInfluenceAboveRestingPoint = ((double)iCurrentInfluence - (double)iRestingPoint) / 10000;
				double dExponent = /*1.5*/ (double)GD_FLOAT_GET(MINOR_INFLUENCE_SCALING_DECAY_EXPONENT);
				double dExtraDecay = pow(dInfluenceAboveRestingPoint, dExponent) * -100;
				iChangeThisTurn += (int)dExtraDecay;
			}
			else
			{
				// If below 100 over resting point, but still over, then extra decay = (Influence over resting point)% of -1. So, 5 over resting point = -0.05 decay
				int iExtraDecay = (iCurrentInfluence - iRestingPoint) / -100;
				iChangeThisTurn += iExtraDecay;
			}
		}

		// Multiply decay rate towards protectors if capital has taken damage
		if (GetPlayer()->getCapitalCity()->getDamage() > 0 && IsProtectedByMajor(ePlayer))
		{
			iChangeThisTurn *= /*100 in CP, 300 in VP*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_PER_TURN_DAMAGED_CAPITAL_MULTIPLIER);
			iChangeThisTurn /= 100;
		}

		// Reductions to decay?
		if (iChangeThisTurn < 0)
		{
			int iDecayMod = 100 + GET_PLAYER(ePlayer).GetMinorFriendshipDecayMod();
			iDecayMod += IsSameReligionAsMajor(ePlayer) ? /*-25*/ (GD_INT_GET(MINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION) / -2) : 0;
			iDecayMod += kPlayer.GetPlayerTraits()->GetCityStateFriendshipModifier() / -2;

			if (iDecayMod < 0)
				iDecayMod = 0;

			iChangeThisTurn *= iDecayMod;
			iChangeThisTurn /= 100;
		}
		else
			iChangeThisTurn = 0;
	}

	iChangeThisTurn += iShift;

	if (iChangeThisTurn < 0)
	{
		// No City-State decay while at war?
		if (IsAllies(ePlayer) && GET_PLAYER(ePlayer).IsNoCSDecayAtWar() && GET_PLAYER(ePlayer).IsAtWar())
		{
			iChangeThisTurn = 0;
		}
		// Cold War fun?
		else if (IsAllies(ePlayer))
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
					if (eLoopPlayer != NO_PLAYER)
					{
						if (GC.getGame().GetGameLeagues()->IsIdeologyEmbargoed(ePlayer, eLoopPlayer))
						{
							iChangeThisTurn = 0;
						}
					}
				}
			}
		}
	}

	// Mod everything by game speed
	iChangeThisTurn *= 100;							// this could produce int over 32000
	iChangeThisTurn /= GC.getGame().getGameSpeedInfo().getTrainPercent();

	// Decay can't bring a player below their resting point, and recovery (except through special bonuses) can't bring a player above their resting point
	int iRtnValue = iChangeThisTurn;
	int iDelta = iRestingPoint - iCurrentInfluence;

	if (iCurrentInfluence > iRestingPoint && iChangeThisTurn < 0)
	{
		iRtnValue = max(iChangeThisTurn, iDelta);
	}
	else if (iCurrentInfluence < iRestingPoint && iChangeThisTurn > 0)
	{
		if (iShift > 0)
		{
			int iChangeThisTurnWithoutShift = iChangeThisTurn - iShift;
			if (iChangeThisTurnWithoutShift > 0)
			{
				iRtnValue = min(iChangeThisTurnWithoutShift, iDelta) + iShift;
			}
		}
		else
			iRtnValue = min(iChangeThisTurn, iDelta);
	}

	return iRtnValue;
}

// What is the level of Friendship between this Minor and the requested Major Civ?
// Takes things like war status into account
// NOTE: Not const because of need to check war status
int CvMinorCivAI::GetEffectiveFriendshipWithMajorTimes100(PlayerTypes ePlayer, bool bIgnoreWar)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()

	// Are we at war?
	if (!bIgnoreWar && IsAtWarWithPlayersTeam(ePlayer))
		return (100 * /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR));

	return GetBaseFriendshipWithMajorTimes100(ePlayer);
}

// What is the raw, stored level of Friendship between this Minor and the requested Major Civ?
int CvMinorCivAI::GetBaseFriendshipWithMajorTimes100(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0; // as defined during Reset()

	return m_aiFriendshipWithMajorTimes100[ePlayer];
}

/// Sets the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::SetFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iNum, bool bFromQuest, bool bFromCoup, bool bFromWar)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	int iOldEffectiveFriendship = GetEffectiveFriendshipWithMajorTimes100(ePlayer, bFromWar);

	m_aiFriendshipWithMajorTimes100[ePlayer] = iNum;

	int iMinimumFriendship = /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR);
	if(GetBaseFriendshipWithMajor(ePlayer) < iMinimumFriendship)
		m_aiFriendshipWithMajorTimes100[ePlayer] = iMinimumFriendship * 100;

	int iNewEffectiveFriendship = bFromCoup ? m_aiFriendshipWithMajorTimes100[ePlayer] : GetEffectiveFriendshipWithMajorTimes100(ePlayer, bFromWar);

	// Has the friendship in effect changed?
	if(iOldEffectiveFriendship != iNewEffectiveFriendship)
	{
		DoFriendshipChangeEffects(ePlayer, iOldEffectiveFriendship/100, iNewEffectiveFriendship/100, bFromQuest);
	}

	// Update City banners and game info if this is the active player
	if(ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}

/// Changes the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::ChangeFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		// If this friendship was earned from a Quest, then we might apply a modifier to it
		if(bFromQuest && iChange > 0)
		{
			if(GET_PLAYER(ePlayer).getMinorQuestFriendshipMod() != 0)
			{
				iChange *= (100 + GET_PLAYER(ePlayer).getMinorQuestFriendshipMod());
				iChange /= 100;
			}
		}

		SetFriendshipWithMajorTimes100(ePlayer, GetBaseFriendshipWithMajorTimes100(ePlayer) + iChange, bFromQuest);
	}
}

// What is the level of Friendship between this Minor and the requested Major Civ?
// Takes war status into account
// NOTE: Not const because of need to check war status
int CvMinorCivAI::GetEffectiveFriendshipWithMajor(PlayerTypes ePlayer)
{
	return GetEffectiveFriendshipWithMajorTimes100(ePlayer) / 100;
}

// What is the raw, stored level of Friendship between this Minor and the requested Major Civ?
int CvMinorCivAI::GetBaseFriendshipWithMajor(PlayerTypes ePlayer) const
{
	return GetBaseFriendshipWithMajorTimes100(ePlayer) / 100;
}

/// Sets the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::SetFriendshipWithMajor(PlayerTypes ePlayer, int iNum, bool bFromQuest, bool bFromWar)
{
	SetFriendshipWithMajorTimes100(ePlayer, iNum * 100, bFromQuest, false, bFromWar);
}

/// Changes the base level of Friendship between this Minor and the specified Major Civ
void CvMinorCivAI::ChangeFriendshipWithMajor(PlayerTypes ePlayer, int iChange, bool bFromQuest)
{
	ChangeFriendshipWithMajorTimes100(ePlayer, iChange * 100, bFromQuest);
}

/// What is the resting point of Influence this major has?  Affected by religion, social policies, Wary Of, etc.
int CvMinorCivAI::GetFriendshipAnchorWithMajor(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return 0;

	CvPlayer* pMajor = &GET_PLAYER(eMajor);
	CvAssertMsg(pMajor, "MINOR CIV AI: pMajor not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMajor) return 0;

	int iAnchor = /*0*/ GD_INT_GET(MINOR_FRIENDSHIP_ANCHOR_DEFAULT) + GetRestingPointChange(eMajor);

	// Pledge to Protect
	if (IsProtectedByMajor(eMajor))
	{
		iAnchor += /*5*/ GD_INT_GET(MINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED);
	}

	// Wary Of?
	if (IsWaryOfTeam(pMajor->getTeam()))
	{
		iAnchor += /*-20*/ GD_INT_GET(MINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF);
	}
	if (MOD_BALANCE_CORE_MINORS) 
	{
		if (GetJerkTurnsRemaining(pMajor->getTeam()) > 0)
		{
			iAnchor += /*-20*/ GD_INT_GET(MINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF);
		}
	}

	// Diplomatic Marriage? (VP)
	if (!GetPlayer()->IsAtWarWith(pMajor->GetID()) && pMajor->GetPlayerTraits()->IsDiplomaticMarriage() && IsMarried(eMajor))
	{
		int iEra = pMajor->GetCurrentEra();
		if (iEra <= 0)
		{
			iEra = 1;
		}
		iAnchor += /*75*/ GD_INT_GET(BALANCE_MARRIAGE_RESTING_POINT_INCREASE) * iEra;
	}
	// United Front? (VP)
	if (IsAllies(eMajor) && pMajor->IsAtWar())
	{
		iAnchor += pMajor->GetMinimumAllyInfluenceIncreaseAtWar();
	}

	// Social Policies
	iAnchor += pMajor->GetMinorFriendshipAnchorMod();

	// Religion
	CvPlayerReligions* pMajorReligions = pMajor->GetReligions();
	CvAssertMsg(pMajorReligions, "MINOR CIV AI: pMajorReligions not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMajorReligions) return iAnchor;
	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if (!pMinorCapital) return iAnchor; // Happens when city was just captured, after buyout, etc., so just return the anchor value we have
	CvCityReligions* pMinorCapitalReligions = pMinorCapital->GetCityReligions();
	CvAssertMsg(pMinorCapitalReligions, "MINOR CIV AI: pMinorCapitalReligions not expected to be NULL.  Please send Anton your save file and version.");
	if (!pMinorCapitalReligions) return iAnchor;
	iAnchor += pMajorReligions->GetCityStateMinimumInfluence(pMinorCapitalReligions->GetReligiousMajority(), eMajor);

	return iAnchor;
}

/// Resets the base level of Friendship to zero
void CvMinorCivAI::ResetFriendshipWithMajor(PlayerTypes ePlayer)
{
	// If ePlayer isn't a major civ then there is no influence value to reset, so just return
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return; // as defined during Reset()

	int iOldFriendship = GetEffectiveFriendshipWithMajor(ePlayer);
	const int iResetFriendship = 0;
	if(GetPlayer()->isAlive())
	{
		SetFriendshipWithMajor(ePlayer, iResetFriendship);
	}
	else
	{
		// special workaround to allow status changes despite minor already being dead
		DoFriendshipChangeEffects(ePlayer, iOldFriendship, iResetFriendship, /*bFromQuest*/ false, /*bIgnoreMinorDeath*/ true);
		SetFriendshipWithMajor(ePlayer, iResetFriendship);
	}

}

/// Update Best Relations Resource Bonus
void CvMinorCivAI::DoUpdateAlliesResourceBonus(PlayerTypes eNewAlly, PlayerTypes eOldAlly)
{
	if (eNewAlly == eOldAlly)
		return; //nothing to do

	PlayerTypes TechTestPlayer = NO_PLAYER;
	if (eNewAlly != NO_PLAYER)
		TechTestPlayer = eNewAlly;
	else if (eOldAlly != NO_PLAYER)
		TechTestPlayer = eOldAlly;

	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;
		const CvPlayer * pPlayer = &GET_PLAYER(TechTestPlayer);
		if (TechTestPlayer != NO_PLAYER && pPlayer && !pPlayer->IsResourceRevealed(eResource))
			continue;

		ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();

		if(eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
		{
			bool bNeedsUpdate = true;
			if (eOldAlly == eNewAlly)
			{
				int iResourceQuantityNew = GetPlayer()->getNumResourceTotal(eResource);
				if (iResourceQuantityNew == 0)
					bNeedsUpdate = false;
			}
			
			if (bNeedsUpdate)
			{
				// Someone is losing the bonus :(
				if (eOldAlly != NO_PLAYER)
				{
					int iResourceQuantity = GetPlayer()->getResourceExport(eResource);

					if (iResourceQuantity > 0)
					{
						GET_PLAYER(eOldAlly).changeResourceFromMinors(eResource, -iResourceQuantity);
						GetPlayer()->changeResourceExport(eResource, -iResourceQuantity);
					}
				}

				// Someone new is getting the bonus :D
				if (eNewAlly != NO_PLAYER)
				{
					int iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);

					if (iResourceQuantity > 0)
					{
						GET_PLAYER(eNewAlly).changeResourceFromMinors(eResource, iResourceQuantity);
						GetPlayer()->changeResourceExport(eResource, iResourceQuantity);
					}
				}
			}
		}
	}
}

/// The most Friendship (effective, not base) any player has with this Minor
int CvMinorCivAI::GetMostFriendshipWithAnyMajor(PlayerTypes& eBestPlayer)
{
	int iMostFriendship = 0;
	PlayerTypes eMajor;

	int iFriendship = 0;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eMajor = (PlayerTypes) iMajorLoop;

		if(IsHasMetPlayer(eMajor))
		{
			iFriendship = GetEffectiveFriendshipWithMajor(eMajor);

			if(iFriendship > iMostFriendship)
			{
				eBestPlayer = eMajor;
				iMostFriendship = iFriendship;
			}
		}
	}

	return iMostFriendship;
}

/// Who has the best relations with us right now?
PlayerTypes CvMinorCivAI::GetAlly() const
{
	if (IsNoAlly())
	{
		return NO_PLAYER;
	}
	else if (GetPermanentAlly() != NO_PLAYER)
	{
		return GetPermanentAlly();
	}

	return m_eAlly;
}

/// Sets who has the best relations with us right now
void CvMinorCivAI::SetAlly(PlayerTypes eNewAlly)
{
	CvAssertMsg(eNewAlly >= NO_PLAYER, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eNewAlly < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvMap& theMap = GC.getMap();
	int iNumPlots = GC.getMap().numPlots();

	PlayerTypes eOldAlly = GetAlly();
#if defined(MOD_BALANCE_CORE)
	if(IsNoAlly())
	{
		m_eAlly = NO_PLAYER;
		return;
	}
	if(GetPermanentAlly() != NO_PLAYER && eNewAlly != GetPermanentAlly())
	{
		m_eAlly = GetPermanentAlly();
		GET_PLAYER(GetPermanentAlly()).RefreshCSAlliesFriends();
		GET_PLAYER(GetPermanentAlly()).UpdateHappinessFromMinorCivs();
		return;
	}
#endif
	int iPlotVisRange = /*1*/ GD_INT_GET(PLOT_VISIBILITY_RANGE);

	if(eOldAlly != NO_PLAYER)
	{
		for(int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight(GET_PLAYER(eOldAlly).getTeam(), iPlotVisRange, false, NO_INVISIBLE, NO_DIRECTION);
			}
		}

		//teleport our units if necessary
		GC.getMap().verifyUnitValidPlot();

		if(eOldAlly == GC.getGame().getActivePlayer())
		{
			theMap.updateDeferredFog();
		}
	}

	m_eAlly = eNewAlly;
	m_iTurnAllied = GC.getGame().getGameTurn();

	// Seed the GP counter?
	if(eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);

		// share the visibility with my ally (and his team-mates)
		for(int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pPlot = theMap.plotByIndexUnchecked(iI);
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				pPlot->changeAdjacentSight(kNewAlly.getTeam(), iPlotVisRange, true, NO_INVISIBLE, NO_DIRECTION);
			}
		}

		for(int iPolicyLoop = 0; iPolicyLoop < GC.getNumPolicyInfos(); iPolicyLoop++)
		{
			const PolicyTypes eLoopPolicy = static_cast<PolicyTypes>(iPolicyLoop);
			CvPolicyEntry* pkPolicyInfo = GC.getPolicyInfo(eLoopPolicy);
			if(pkPolicyInfo)
			{
				if(kNewAlly.GetPlayerPolicies()->HasPolicy(eLoopPolicy) && !kNewAlly.GetPlayerPolicies()->IsPolicyBlocked(eLoopPolicy))
				{
					// This is the policy we want!
					if(pkPolicyInfo->IsMinorGreatPeopleAllies())
					{
						if(kNewAlly.GetGreatPeopleSpawnCounter() <= 0)
							kNewAlly.DoSeedGreatPeopleSpawnCounter();
						else
							kNewAlly.DoApplyNewAllyGPBonus();
					}
				}
			}
		}

		//Achievement Test
		if (MOD_API_ACHIEVEMENTS)
			kNewAlly.GetPlayerAchievements().AlliedWithCityState(GetPlayer()->GetID());
	}

	// Alter who gets this guy's resources
	DoUpdateAlliesResourceBonus(eNewAlly, eOldAlly);
	if (eNewAlly != NO_PLAYER)
	{
		GET_PLAYER(eNewAlly).RefreshCSAlliesFriends();
		GET_PLAYER(eNewAlly).UpdateHappinessFromMinorCivs();
	}
	if (eOldAlly != NO_PLAYER)
	{
		GET_PLAYER(eOldAlly).RefreshCSAlliesFriends();
		GET_PLAYER(eOldAlly).UpdateHappinessFromMinorCivs();
	}

	// Declare war on Ally's enemies
	if(eNewAlly != NO_PLAYER)
	{
		CvPlayerAI& kNewAlly = GET_PLAYER(eNewAlly);
		CvTeam& kNewAllyTeam = GET_TEAM(kNewAlly.getTeam());
		CvTeam& kOurTeam = GET_TEAM(GetPlayer()->getTeam());

		TeamTypes eLoopTeam;
		for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
		{
			eLoopTeam = (TeamTypes) iTeamLoop;

			if(!GET_TEAM(eLoopTeam).isAlive())
				continue;

			if(kNewAllyTeam.isAtWar(eLoopTeam))
			{
				kOurTeam.declareWar(eLoopTeam, true, GetPlayer()->GetID());
			}
		}
	}

	DoTestEndWarsVSMinors(eOldAlly, eNewAlly);
	DoTestEndSkirmishes(eNewAlly);

	//If we get a yield bonus in all cities because of CS alliance, this is a good place to change it.
	if (MOD_BALANCE_CORE && eNewAlly != NO_PLAYER)
	{
		int iEra = GET_PLAYER(eNewAlly).GetCurrentEra();
		if(iEra <= 0)
		{
			iEra = 1;
		}
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			YieldTypes eYield = (YieldTypes) iI;
			if(GET_PLAYER(eNewAlly).GetPlayerTraits()->GetYieldFromCSAlly(eYield) > 0)
			{
				CvCity* pCapital = GET_PLAYER(eNewAlly).getCapitalCity();
				if(pCapital != NULL)
				{
					pCapital->ChangeBaseYieldRateFromCSAlliance(eYield, GET_PLAYER(eNewAlly).GetPlayerTraits()->GetYieldFromCSAlly(eYield) * iEra);
				}
			}
		}
	}
	//If we lose a yield bonus in all cities because of CS alliance, this is a good place to change it.
	if (MOD_BALANCE_CORE && (eOldAlly != NO_PLAYER) && (eOldAlly != eNewAlly))
	{
		int iEra = GET_PLAYER(eOldAlly).GetCurrentEra();
		if(iEra <= 0)
		{
			iEra = 1;
		}
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			YieldTypes eYield = (YieldTypes) iI;
			if(GET_PLAYER(eOldAlly).GetPlayerTraits()->GetYieldFromCSAlly(eYield) > 0)
			{
				CvCity* pCapital = GET_PLAYER(eOldAlly).getCapitalCity();
				if(pCapital != NULL)
				{
					pCapital->ChangeBaseYieldRateFromCSAlliance(eYield, (GET_PLAYER(eOldAlly).GetPlayerTraits()->GetYieldFromCSAlly(eYield) * -1 * iEra));
				}
			}
		}
	}

	CvCity* pCity = m_pPlayer->getCapitalCity();
	if (pCity)
		pCity->updateStrengthValue();

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eOldAlly);
		args->Push(eNewAlly);

		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "SetAlly", args.get(), bResult);
	}

	// Test for Domination Victory
	if (eNewAlly != NO_PLAYER)
		GC.getGame().DoTestConquestVictory();
}

/// How many turns has the alliance been active?
int CvMinorCivAI::GetAlliedTurns() const
{
	int iRtnValue = -1;

	if (m_eAlly != NO_PLAYER)
	{
		iRtnValue = GC.getGame().getGameTurn() - m_iTurnAllied;
	}

	return iRtnValue;
}

/// Is ePlayer Allies with this minor?
bool CvMinorCivAI::IsAllies(PlayerTypes ePlayer) const
{
	return (m_eAlly == ePlayer || (GetPermanentAlly() == ePlayer && !IsNoAlly()));
}

/// Is ePlayer Friends with this minor?
bool CvMinorCivAI::IsFriends(PlayerTypes ePlayer)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;
	return m_abFriends[ePlayer];
}

/// Has ePlayer ever been Friends with this minor?
bool CvMinorCivAI::IsEverFriends(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return m_abEverFriends[ePlayer];
}

/// Has ePlayer ever been Friends with this minor?
void CvMinorCivAI::SetEverFriends(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abEverFriends[ePlayer] = bValue;
}

/// Is ePlayer Friends with this minor?
void CvMinorCivAI::SetFriends(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abFriends[ePlayer] = bValue;
}


/// Are we about to lose our status? (used in Diplo AI)
bool CvMinorCivAI::IsCloseToNotBeingAllies(PlayerTypes ePlayer)
{
	if (GetPermanentAlly() == ePlayer)
		return false;

	int iBuffer = GetEffectiveFriendshipWithMajor(ePlayer) - GetAlliesThreshold(ePlayer);

	if(iBuffer >= 0 && iBuffer < /*8*/ GD_INT_GET(MINOR_FRIENDSHIP_CLOSE_AMOUNT))
		return true;

	return false;
}

/// Are we about to lose our status? (used in Diplo AI)
bool CvMinorCivAI::IsCloseToNotBeingFriends(PlayerTypes ePlayer)
{
	int iBuffer = GetEffectiveFriendshipWithMajor(ePlayer) - GetFriendsThreshold(ePlayer);

	if(iBuffer >= 0 && iBuffer < /*8*/ GD_INT_GET(MINOR_FRIENDSHIP_CLOSE_AMOUNT))
		return true;

	return false;
}

/// What level of Friendship does a player have with this Minor?
int CvMinorCivAI::GetFriendshipLevelWithMajor(PlayerTypes ePlayer)
{
	if(IsAllies(ePlayer))
	{
		return 2;
	}
	else if(IsFriends(ePlayer))
	{
		return 1;
	}

	return 0;
}


/// Friendship needed for next level of bonuses
int CvMinorCivAI::GetFriendshipNeededForNextLevel(PlayerTypes ePlayer)
{
	if (!IsFriends(ePlayer))
	{
		return GetFriendsThreshold(ePlayer);
	}
	else if (!IsAllies(ePlayer))
	{
		return GetAlliesThreshold(ePlayer);
	}

	return 0;
}

/// What happens when Friendship changes?
void CvMinorCivAI::DoFriendshipChangeEffects(PlayerTypes ePlayer, int iOldFriendship, int iNewFriendship, bool bFromQuest, bool bIgnoreMinorDeath)
{
	// Can't give out bonuses if we're dead!
	if(!bIgnoreMinorDeath && !GetPlayer()->isAlive())
		return;

	Localization::String strMessage;
	Localization::String strSummary;

	PlayerTypes eOldAlly = GetAlly();

	bool bAdd = false;
	bool bFriendsChanged = false;
	bool bAlliesChanged = false;
	bool bWasFriends = IsFriends(ePlayer);
	bool bNowFriends = IsFriendshipAboveFriendsThreshold(ePlayer, iNewFriendship);
	bool bNowAllies = IsFriendshipAboveAlliesThreshold(ePlayer, iNewFriendship);

	// If we are Friends now, mark that we've been Friends at least once this game
	if(bNowFriends)
		SetEverFriends(ePlayer, true);

	// Add Friends Bonus
	if (!bWasFriends && bNowFriends)
	{
		bAdd = true;
		bFriendsChanged = true;
		SetFriends(ePlayer, true);
	}
	// Remove Friends bonus
	else if (bWasFriends && !bNowFriends)
	{
		bAdd = false;
		bFriendsChanged = true;
		SetFriends(ePlayer, false);

		//todo: should we wake up all units of ePlayer in our territory?
	}

	// Resolve Allies status with sphere of influence or open door
	bool bHasOtherPermanentAlly = false;
	if (IsNoAlly() || (GetPermanentAlly() != NO_PLAYER && GetPermanentAlly() != ePlayer))
	{
		bNowAllies = false;
		bHasOtherPermanentAlly = true;
	}

	// No old ally and now allies, OR our friendship is now higher than a previous ally
	if((eOldAlly == NO_PLAYER && bNowAllies)
	        || ((eOldAlly != NO_PLAYER && GetEffectiveFriendshipWithMajor(ePlayer) > GetEffectiveFriendshipWithMajor(eOldAlly)) && !bHasOtherPermanentAlly))
	{
		bAdd = true;
		bAlliesChanged = true;
	}
	// Remove Allies bonus
	else if (eOldAlly == ePlayer && (!bNowAllies || bHasOtherPermanentAlly))
	{
		bAdd = false;
		bAlliesChanged = true;
	}

	if (MOD_EVENTS_MINORS)
	{
		if (bFriendsChanged)
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorFriendsChanged, m_pPlayer->GetID(), ePlayer, bAdd, iOldFriendship, iNewFriendship);
		if (bAlliesChanged)
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorAlliesChanged, m_pPlayer->GetID(), ePlayer, bAdd, iOldFriendship, iNewFriendship);
	}

	// Make changes to bonuses here. Only send notifications if this change is not related to quests (otherwise it is rolled into quest notification)
	if(bFriendsChanged || bAlliesChanged)
		DoSetBonus(ePlayer, bAdd, bFriendsChanged, bAlliesChanged, /*bSuppressNotifications*/ bFromQuest);

	// Now actually changed Allied status, since we needed the old player in effect to create the notifications in the function above us
	if(bAlliesChanged)
	{
		if(bAdd)
			SetAlly(ePlayer);
		else
			SetAlly(NO_PLAYER);	// We KNOW no one else can be higher, so set the Ally to NO_PLAYER
	}
	if (ePlayer != NO_PLAYER)
	{
		GET_PLAYER(ePlayer).RefreshCSAlliesFriends();
	}
}


/// Is the player above the "Friends" threshold?
bool CvMinorCivAI::IsFriendshipAboveFriendsThreshold(PlayerTypes ePlayer, int iFriendship) const
{
	int iFriendshipThresholdFriends = GetFriendsThreshold(ePlayer);

	if (m_pPlayer->GetMinorCivAI()->IsAtWarWithPlayersTeam(ePlayer))
		return false;

	if(iFriendship >= iFriendshipThresholdFriends)
	{
		return true;
	}

	if (GetPermanentAlly() == ePlayer)
		return true;

	return false;
}


/// What is the friends threshold?
int CvMinorCivAI::GetFriendsThreshold(PlayerTypes ePlayer) const
{
	int iThreshold = /*30*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_FRIENDS);

	if (MOD_CITY_STATE_SCALE)
	{
		iThreshold *= 100 + max(0, m_pPlayer->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iThreshold /= 100;

		EraTypes eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();
		EraTypes eMedieval = (EraTypes)GC.getInfoTypeForString("ERA_MEDIEVAL", true);
		EraTypes eIndustrial = (EraTypes)GC.getInfoTypeForString("ERA_INDUSTRIAL", true);

		// Industrial era or Later
		if (eCurrentEra >= eIndustrial)
		{
			iThreshold *= /*10*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_MOD_INDUSTRIAL);
			iThreshold /= 100;
		}
		// Medieval era or later
		else if (eCurrentEra >= eMedieval)
		{
			iThreshold *= /*6*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_MOD_MEDIEVAL);
			iThreshold /= 100;
		}
	}

	return iThreshold;
}

/// Is the player above the threshold to get the Allies bonus?
bool CvMinorCivAI::IsFriendshipAboveAlliesThreshold(PlayerTypes ePlayer, int iFriendship) const
{
	int iFriendshipThresholdAllies = GetAlliesThreshold(ePlayer);

	if (iFriendship >= iFriendshipThresholdAllies)
	{
		return true;
	}

	return false;
}

/// What is the allies threshold?
int CvMinorCivAI::GetAlliesThreshold(PlayerTypes ePlayer) const
{
	int iThreshold = /*60*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_ALLIES);

	if (MOD_CITY_STATE_SCALE)
	{
		iThreshold *= 100 + max(0, m_pPlayer->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iThreshold /= 100;

		EraTypes eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();
		EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
		EraTypes eIndustrial = (EraTypes)GC.getInfoTypeForString("ERA_INDUSTRIAL", true);

		// Industrial era or Later
		if (eCurrentEra >= eIndustrial)
		{
			iThreshold *= /*10*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_MOD_INDUSTRIAL);
			iThreshold /= 100;
		}
		// Medieval era or later
		else if (eCurrentEra >= eMedieval)
		{
			iThreshold *= /*6*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_MOD_MEDIEVAL);
			iThreshold /= 100;
		}
	}

	return iThreshold;
}

/// Sets a major to get a Bonus (or not) - set both bFriends and bAllies to be true if you're adding/removing both states at once
void CvMinorCivAI::DoSetBonus(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, bool bSuppressNotifications, bool bPassedBySomeone, PlayerTypes eNewAlly)
{
	MinorCivTraitTypes eTrait = GetTrait();

	// Cultured
	if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
	}
	// Militaristic
	else if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		if(bAdd)
		{
			// Seed Counter if it hasn't been done yet in this game. We don't have to undo this at any point because the counter is not processed if we are no longer Friends
			if(GetUnitSpawnCounter(ePlayer) == -1)
				DoSeedUnitSpawnCounter(ePlayer, /*bBias*/ true);
		}
	}
	// Maritime
	else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if(bFriends)	// Friends bonus
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if(bAllies)		// Allies bonus
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if(!bAdd)		// Flip amount of we're taking bonuses away
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}

		GET_PLAYER(ePlayer).ChangeCapitalYieldChangeTimes100(YIELD_FOOD, iCapitalFoodTimes100);
		GET_PLAYER(ePlayer).ChangeCityYieldChangeTimes100(YIELD_FOOD, iOtherCitiesFoodTimes100);
	}
	// Mercantile
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		GET_PLAYER(ePlayer).CalculateNetHappiness();
	}
	// Religious
	if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
	}

	if(ePlayer == GC.getGame().getActivePlayer())
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	CvString strDetailedInfo = GetStatusChangeDetails(ePlayer, bAdd, bFriends, bAllies);

	PlayerTypes eOldAlly = GetAlly();
	TeamTypes eOldAllyTeam = eOldAlly != NO_PLAYER ? GET_PLAYER(eOldAlly).getTeam() : NO_TEAM;

	// Should we remove the Ally bonus from another player?
	if(bAdd && bAllies)
	{
		if(eOldAlly != NO_PLAYER && ePlayer != eOldAlly)
		{
			DoSetBonus(eOldAlly, /*bAdd*/ false, /*bFriends*/ false, /*bAllies*/ true, /*bSuppressNotifications*/ false, /*bPassedBySomeone*/ true, ePlayer);
		}
	}

	// *******************************************
	// NOTIFICATIONS FOR THIS PLAYER
	// *******************************************
	// We're not displaying notifications at all
	if (IsDisableNotifications())
	{
		return;
	}

	if (!bSuppressNotifications)
	{
		pair<CvString, CvString> notifStrings = GetStatusChangeNotificationStrings(ePlayer, bAdd, bFriends, bAllies, eOldAlly, (bAdd && bAllies) ? ePlayer : eNewAlly);
		if (notifStrings.first != "")
		{
			AddNotification(notifStrings.first, notifStrings.second, ePlayer);
		}
	}

	// *******************************************
	// NOTIFICATIONS FOR OTHER PLAYERS IN THE GAME
	// *******************************************
	Localization::String strMessageOthers;
	Localization::String strSummaryOthers;

	// We need to do this because this function is recursive, and if we're UNDOING someone else, we don't yet know who the new guy is because it hasn't been set yet
	if (bPassedBySomeone)
	{
		ePlayer = eNewAlly;

		// Notify diplo AI (competition penalty)
		if (eOldAlly != NO_PLAYER && eNewAlly != NO_PLAYER && eOldAlly != eNewAlly)
			GET_PLAYER(eOldAlly).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(eNewAlly, 1);
	}

	for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
		PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyLoop;
		CvPlayerAI& kCurNotifyPlayer = GET_PLAYER(eNotifyPlayer);
		CvTeam* pNotifyTeam = &GET_TEAM(kCurNotifyPlayer.getTeam());
		TeamTypes eNewAllyTeam = GET_PLAYER(ePlayer).getTeam();
		const char* strNewBestPlayersNameKey = NULL;

		// Notify player has met the new Ally
		if(pNotifyTeam->isHasMet(eNewAllyTeam))
			strNewBestPlayersNameKey = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
		// Notify player has NOT met the new Ally
		else
			strNewBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";

		const char* strOldBestPlayersNameKey = "";

		// Someone got passed up
		if (eOldAlly != NO_PLAYER)
		{
			// Notify player has met the old Ally
			if (pNotifyTeam->isHasMet(eOldAllyTeam))
			{
				strOldBestPlayersNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();
			}
			// Notify player has NOT met the old Ally
			else
			{
				strOldBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
			}
		}

		const char* strMinorsNameKey = GetPlayer()->getNameKey();
		TeamTypes eMinorTeam = GetPlayer()->getTeam();

		// Adding/Increasing bonus
		if(bAdd)
		{
			// Jumped up to Allies (either from Neutral or from Friends, or passing another player)
			if(bAllies)
			{
				if(ePlayer != eNotifyPlayer)
				{
					// Has the notify player met this minor
					if(pNotifyTeam->isHasMet(eMinorTeam))
					{
						// Someone got passed up
						if(eOldAlly != NO_PLAYER && eOldAlly != ePlayer)
						{
							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_NEW_BEST_RELATIONS_ALL");
							strMessageOthers << strNewBestPlayersNameKey << strOldBestPlayersNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_BEST_RELATIONS_ALL");
							strSummaryOthers << strMinorsNameKey;
						}
						// No one previously had the bonus
						else
						{
							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_NOW_BEST_RELATIONS_ALL");
							strMessageOthers << strNewBestPlayersNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_NOW_ALLIES_ALL");
							strSummaryOthers << strMinorsNameKey << strNewBestPlayersNameKey;
						}

						// If we're being passed by someone, then don't display this message... we'll roll it into a later one
						if(eOldAlly != eNotifyPlayer)
							AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eNotifyPlayer);
					}
				}
			}
		}
		// Removing/Reducing bonus
		else
		{
			// Dropped from Allies
			if(bAllies)
			{
				if(ePlayer != eNotifyPlayer && eOldAlly != GetPermanentAlly())
				{
					if(pNotifyTeam->isHasMet(eMinorTeam))
					{
						// Only show this message for normal friendship decay
						if(!bPassedBySomeone)
						{
							const char* strOldAllyNameKey = NULL;

							// Notify player has met the old Ally
							if(pNotifyTeam->isHasMet(eOldAllyTeam))
								strOldAllyNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();
							// Notify player has NOT met the old Ally
							else
								strOldAllyNameKey = "TXT_KEY_UNMET_PLAYER";

							strMessageOthers = Localization::Lookup("TXT_KEY_NTFN_MINOR_BEST_RELATIONS_LOST_ALL");
							strMessageOthers << strOldAllyNameKey << strMinorsNameKey;
							strSummaryOthers = Localization::Lookup("TXT_KEY_NTFN_SMMRY_MINOR_BEST_RELATIONS_LOST_ALL");
							strSummaryOthers << strMinorsNameKey << strOldAllyNameKey;

							AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eNotifyPlayer);
						}
					}
				}
			}
		}
	}
}


void CvMinorCivAI::DoUpdateNumThreateningBarbarians()
{
	m_iNumThreateningBarbarians = GetNumThreateningBarbarians() + GetNumThreateningMajors();
	if (m_iNumThreateningBarbarians > 0)
		m_bAllowMajorsToIntrude = true;
}

/// Major Civs intruding in our lands?
void CvMinorCivAI::DoIntrusion()
{
	int iNumThreateningBarbariansLastTurn = m_iNumThreateningBarbarians;
	DoUpdateNumThreateningBarbarians();

	//allow major players in our territory for one turn after the barbarians have been vanquished
	if (iNumThreateningBarbariansLastTurn == 0 && m_iNumThreateningBarbarians == 0)
		m_bAllowMajorsToIntrude = false;
	

	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajor = (PlayerTypes) iMajorLoop;

		if (GetPlayer()->GetMinorCivAI()->IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_HORDE) || GetPlayer()->GetMinorCivAI()->IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_REBELLION))
		{
			m_bAllowMajorsToIntrude = true;
			break;
		}
	}

	vector<PlayerTypes> vIntruders;

	// Look at how many Units each Major Civ has in the Minor's Territory
	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		// Plot owned by this Minor?
		if(pLoopPlot->getOwner() == GetPlayer()->GetID())
		{
			const IDInfo* pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				const CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				// Does this unit not cause anger?
				if(pLoopUnit && pLoopUnit->IsAngerFreeUnit())
					continue;

				// Does this Unit belong to a Major?
				if(pLoopUnit && pLoopUnit->getOwner() < MAX_MAJOR_CIVS)
				{
					// If player has been granted Open Borders or has a friendship with minors bonus, then the Minor doesn't care about intrusion
					if(!IsPlayerHasOpenBorders(pLoopUnit->getOwner()))
					{
						// If the player is at war with the Minor then don't bother
						if(!IsAtWarWithPlayersTeam(pLoopUnit->getOwner()))
						{
							// Ignore if the player trait allows us to intrude without angering
							if(!GET_PLAYER(pLoopUnit->getOwner()).GetPlayerTraits()->IsAngerFreeIntrusionOfCityStates())
							{
								ChangeFriendshipWithMajor(pLoopUnit->getOwner(), /*-6 in CP, -5 in VP*/ GD_INT_GET(FRIENDSHIP_PER_UNIT_INTRUDING));

								// only modify if the unit isn't automated nor having a pending order
								if(!pLoopUnit->IsAutomated() && pLoopUnit->GetLengthMissionQueue() == 0)
								{
									vIntruders.push_back(pLoopUnit->getOwner());
								}
							}
						}
					}
				}
			}
		}
	}

	// Now send out Notifications (if necessary)
	Localization::String strMessage;
	Localization::String strSummary;

	for (size_t i=0; i<vIntruders.size(); i++)
	{
		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_INTRUSION");
		strMessage << GetPlayer()->getNameKey();
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_INTRUSION");
		strSummary << GetPlayer()->getNameKey();

		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), vIntruders[i]);
	}
}

//barbarians may take over the city once it's their turn
void CvMinorCivAI::SetReadyForTakeOver()
{
	m_iTakeoverTurn = GC.getGame().getGameTurn();
}

bool CvMinorCivAI::IsReadyForTakeOver() const
{
	return m_iTakeoverTurn == GC.getGame().getGameTurn();
}

//Do Defection
void CvMinorCivAI::DoDefection()
{
	//Quest Over - Player loses all influence, rival ideology followers gain alliance.
	PlayerTypes eOldAlly = GetPlayer()->GetMinorCivAI()->GetAlly();
	PolicyBranchTypes ePreferredIdeology = NO_POLICY_BRANCH_TYPE;
	if(eOldAlly != NO_PLAYER)
	{
		ePreferredIdeology = GET_PLAYER(eOldAlly).GetCulture()->GetPublicOpinionPreferredIdeology();
		GetPlayer()->GetMinorCivAI()->SetFriendshipWithMajor(eOldAlly, /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR), true);
		GetPlayer()->GetMinorCivAI()->EndAllActiveQuestsForPlayer(eOldAlly);
	}

	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;

		if((GET_PLAYER(eMajorLoop).GetPlayerPolicies()->GetLateGamePolicyTree() == ePreferredIdeology) && (ePreferredIdeology != NO_POLICY_BRANCH_TYPE))
		{
			GetPlayer()->GetMinorCivAI()->ChangeFriendshipWithMajor(eMajorLoop, (GetPlayer()->GetMinorCivAI()->GetAlliesThreshold(eMajorLoop) + 5), true);
		}
	}
	CvCity* pCity = GetPlayer()->getCapitalCity();
	CvCityCitizens* pCitizens = pCity->GetCityCitizens();
	//Let's kill off any barbs remaining.

	for(int iPlotLoop = 1; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
	{
		CvPlot* pPlot = pCitizens->GetCityPlotFromIndex(iPlotLoop);

		if(pPlot && pPlot->getNumUnits() > 0)
		{
			// Get the current list of units because we will possibly be moving them out of the plot's list
			IDInfoVector currentUnits;
			if (pPlot->getUnits(&currentUnits) > 0)
			{
				for(IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
				{
					CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

					if(pLoopUnit && pLoopUnit->isBarbarian())
					{
						pLoopUnit->kill(false);
					}
				}
			}
		}
	}

	//Reset Rebellion, otherwise they just keep spawning forever.
	SetRebellion(false);

	//Resistance, for flavor.
	GetPlayer()->getCapitalCity()->ChangeResistanceTurns(5);

}

/// Is a player allowed to be inside someone else's borders?
bool CvMinorCivAI::IsPlayerHasOpenBorders(PlayerTypes ePlayer)
{
	// At war?
	if (IsAtWarWithPlayersTeam(ePlayer))
		return false;

	// Special trait?
	if (IsPlayerHasOpenBordersAutomatically(ePlayer))
		return true;

	if (m_bAllowMajorsToIntrude)
		return true;

	return IsFriends(ePlayer);
}

/// Is a player allowed to be inside someone else's borders automatically?
bool CvMinorCivAI::IsPlayerHasOpenBordersAutomatically(PlayerTypes ePlayer)
{
	// Special trait?
	if (GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateFriendshipModifier() > 0)
		return true;

	return false;
}


/// Major liberates a Minor by recapturing its City!
void CvMinorCivAI::DoLiberationByMajor(PlayerTypes eLiberator, TeamTypes eConquerorTeam)
{
	// Clear the "bought out by" indicator
	SetMajorBoughtOutBy(NO_PLAYER);

	// Ignore jerk status for the liberator's team
	SetIgnoreJerk(GET_PLAYER(eLiberator).getTeam(), true);

	//set this to a value > 0 so that it takes one turn until other players may not enter our territory
	//prevents immediate teleport of AI units in the neighborhood
	m_iNumThreateningBarbarians = 1;

	Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION");
	strMessage << GetPlayer()->getNameKey() << GET_PLAYER(eLiberator).getNameKey();
	Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();

	int iHighestOtherMajorInfluence = /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR);

	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iI;

		if (ePlayer == eLiberator)
			continue;

		int iInfluence = GetBaseFriendshipWithMajor(ePlayer);

		if (iInfluence > iHighestOtherMajorInfluence)
			iHighestOtherMajorInfluence = iInfluence;

		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isMajorCiv() && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetPlayer()->getTeam()))
		{
			// Influence for other players - Were you the one that conquered us before?
			if (GET_PLAYER(ePlayer).getTeam() == eConquerorTeam)
			{
				SetFriendshipWithMajor(ePlayer, /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR));
			}
			// If we were conquered by Barbarians, reset Influence
			else if (MOD_BALANCE_VP && GET_PLAYER(BARBARIAN_PLAYER).getTeam() == eConquerorTeam)
			{
				SetFriendshipWithMajor(ePlayer, 0);
			}

			// Notification for other players
			CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
			if (pNotifications)
			{
				pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strMessage.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
		}
	}

	// Influence for liberator
	int iNewInfluence = 0;

	// If Open Doors / Sphere of Influence is in effect, raise Influence just above Friends threshold
	if (IsNoAlly() || GetPermanentAlly() != NO_PLAYER)
	{
		iNewInfluence = GetFriendsThreshold(eLiberator) + 10;
	}
	// Otherwise, raise to ally status
	else
	{
		iNewInfluence = max(iHighestOtherMajorInfluence + /*105*/ GD_INT_GET(MINOR_LIBERATION_FRIENDSHIP), GetBaseFriendshipWithMajor(eLiberator) + /*105*/ GD_INT_GET(MINOR_LIBERATION_FRIENDSHIP));
		iNewInfluence = max(GetAlliesThreshold(eLiberator) + 10, iNewInfluence); // Must be at least enough to make us allies

		if (MOD_BALANCE_CORE_MINORS)
		{
			int iEra = GET_PLAYER(eLiberator).GetCurrentEra();
			if (iEra <= 0)
				iEra = 1;

			iNewInfluence *= iEra;
		}
	}

	// Were we liberated from a barbarian? Less influence for you!
	if (MOD_BALANCE_VP && GET_PLAYER(BARBARIAN_PLAYER).getTeam() == eConquerorTeam)
		iNewInfluence /= 2;

	SetFriendshipWithMajor(eLiberator, iNewInfluence);
	SetTurnLiberated(GC.getGame().getGameTurn());

	// Notification for liberator
	strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LIBERATION_YOU");
	strMessage << GetPlayer()->getNameKey();
	strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_LIBERATION");
	strSummary << GetPlayer()->getNameKey();
	AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eLiberator);
}

void CvMinorCivAI::SetTurnLiberated(int iValue)
{
	if(iValue != GetTurnLiberated())
	{
		m_iTurnLiberated = iValue;
	}
}
int CvMinorCivAI::GetTurnLiberated() const
{
	return m_iTurnLiberated;
}

void CvMinorCivAI::TestChangeProtectionFromMajor(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if (!MOD_BALANCE_CORE_MINOR_PTP_MINIMUM_VALUE)
		return;

	if (!IsProtectedByMajor(eMajor))
		return;

	Localization::String strMessage, strSummary;

	if (CanMajorProtect(eMajor, false))
	{
		if (GetNumTurnsSincePtPWarning(eMajor) > 0)
		{
			SetNumTurnsSincePtPWarning(eMajor, 0);
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_TIMER_STOPPED");
			strMessage << GetPlayer()->getNameKey();
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_TIMER_STOPPED_SHORT");
			strSummary << GetPlayer()->getNameKey();
			AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);
		}
		return;
	}

	// If they've fallen below 0 Influence, end protection immediately.
	if (GetEffectiveFriendshipWithMajor(eMajor) < /*0*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT))
	{
		DoChangeProtectionFromMajor(eMajor, false, true);
		SetNumTurnsSincePtPWarning(eMajor, 0);

		CvCity* pCity = m_pPlayer->getCapitalCity();
		if (pCity != NULL)
		{
			pCity->updateStrengthValue();
		}

		strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_CANCELLED_INFLUENCE");
		strMessage << GetPlayer()->getNameKey();
		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_CANCELLED_SHORT");
		strSummary << GetPlayer()->getNameKey();
		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);

		return;
	}

	int iMaxWarningTurns = 12;
	iMaxWarningTurns *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iMaxWarningTurns /= 100;

	bool bBadMilitary = false, bDistance = true;
	int iMajorStrength = 1; // to avoid division by zero issues
	std::vector<int> viMilitaryStrengths;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isMajorCiv())
		{
			int iStrength = GET_PLAYER(ePlayer).GetMilitaryMight();
			viMilitaryStrengths.push_back(iStrength);

			if (ePlayer == eMajor && iStrength > 0)
				iMajorStrength = iStrength;
		}
	}

	// This should not happen.
	if (viMilitaryStrengths.empty())
		return;

	size_t MedianElement = viMilitaryStrengths.size() / 2; // this returns the median, except if the median is an average of two values, in which case it returns the lowest of the two
	std::nth_element(viMilitaryStrengths.begin(), viMilitaryStrengths.begin() + MedianElement, viMilitaryStrengths.end());
	if (iMajorStrength < viMilitaryStrengths[MedianElement])
	{
		bBadMilitary = true;
		bDistance = CanMajorProtect(eMajor, true);
	}

	if (GetNumTurnsSincePtPWarning(eMajor) > iMaxWarningTurns)
	{
		DoChangeProtectionFromMajor(eMajor, false, true);
		SetNumTurnsSincePtPWarning(eMajor, 0);

		CvCity* pCity = m_pPlayer->getCapitalCity();
		if (pCity != NULL)
		{
			pCity->updateStrengthValue();
		}

		if (bBadMilitary)
		{
			strMessage = bDistance ? Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_CANCELLED_MILITARY_AND_DISTANCE") : Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_CANCELLED_MILITARY");
			strMessage << GetPlayer()->getNameKey();
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_CANCELLED_DISTANCE");
			strMessage << GetPlayer()->getNameKey();
		}

		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_CANCELLED_SHORT");
		strSummary << GetPlayer()->getNameKey();
		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);
	}
	else if (GetNumTurnsSincePtPWarning(eMajor) <= 0)
	{
		if (bBadMilitary)
		{
			int iPercentIncrease = (viMilitaryStrengths[MedianElement] - iMajorStrength) * 100 / iMajorStrength;
			strMessage = bDistance ? Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_MILITARY_AND_DISTANCE") : Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_MILITARY");
			strMessage << GetPlayer()->getNameKey();
			strMessage << (iMaxWarningTurns - GetNumTurnsSincePtPWarning(eMajor));
			strMessage << iPercentIncrease;		
		}
		else
		{
			strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_DISTANCE");
			strMessage << GetPlayer()->getNameKey();
			strMessage << (iMaxWarningTurns - GetNumTurnsSincePtPWarning(eMajor));
		}

		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_SHORT");
		AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);
		SetNumTurnsSincePtPWarning(eMajor, 1);
	}
	else if (GetNumTurnsSincePtPWarning(eMajor) > 0)
	{
		if (GetNumTurnsSincePtPWarning(eMajor) % 4 == 0)
		{
			if (bBadMilitary)
			{
				int iPercentIncrease = (viMilitaryStrengths[MedianElement] - iMajorStrength) * 100 / iMajorStrength;
				strMessage = bDistance ? Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_TIMER_MILITARY_AND_DISTANCE") : Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_TIMER_MILITARY");
				strMessage << GetPlayer()->getNameKey(); 
				strMessage << (iMaxWarningTurns - GetNumTurnsSincePtPWarning(eMajor));
				strMessage << iPercentIncrease;
			}
			else
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_TIMER_DISTANCE");
				strMessage << GetPlayer()->getNameKey();
				strMessage << (iMaxWarningTurns - GetNumTurnsSincePtPWarning(eMajor));
			}

			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_PTP_WARNING_TIMER_SHORT");
			AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor);
		}

		ChangeNumTurnsSincePtPWarning(eMajor, 1);
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

CvString CvMinorCivAI::GetPledgeProtectionInvalidReason(PlayerTypes eMajor)
{
	CvString sFactors = "";

	Localization::String sMandatory = Localization::Lookup("TXT_KEY_POP_CSTATE_PTP_ALL_CORRECT");
	sFactors += sMandatory.toUTF8();

	// If at war, may not protect
	if (GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(GetPlayer()->getTeam()))
	{
		Localization::String sWar = Localization::Lookup("TXT_KEY_POP_CSTATE_PTP_AT_WAR");
		sFactors += sWar.toUTF8();
	}

	// Must have positive INF
	if (GetEffectiveFriendshipWithMajor(eMajor) < /*0*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT))
	{ 
		Localization::String sInf = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_INFLUENCE_TT");
		sInf << /*0*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT);
		sFactors += sInf.toUTF8();
	}

	// Must not be too soon after a previous pledge was broken
	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeBrokenTurn = GetTurnLastPledgeBrokenByMajor(eMajor);
	const int iGracePeriod = 20; //antonjs: todo: xml

	if (iLastPledgeBrokenTurn >= 0 && iLastPledgeBrokenTurn + iGracePeriod > iCurrentTurn)
	{
		Localization::String sPledgeBroken = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_RECENT_BROKEN_TT");
		int iTurn = ((iLastPledgeBrokenTurn + iGracePeriod) - iCurrentTurn);
		sPledgeBroken << iTurn;

		sFactors += sPledgeBroken.toUTF8();
	}

	int iMajorStrength = 1; // to avoid division by zero issues
	std::vector<int> viMilitaryStrengths;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isMajorCiv())
		{
			int iStrength = GET_PLAYER(ePlayer).GetMilitaryMight();
			viMilitaryStrengths.push_back(iStrength);

			if (ePlayer == eMajor && iStrength > 0)
				iMajorStrength = iStrength;
		}
	}

	if (!viMilitaryStrengths.empty()) // This should not happen.
	{
		size_t MedianElement = viMilitaryStrengths.size() / 2; // this returns the median, except if the median is an average of two values, in which case it returns the lowest of the two
		std::nth_element(viMilitaryStrengths.begin(), viMilitaryStrengths.begin() + MedianElement, viMilitaryStrengths.end());
		if (iMajorStrength < viMilitaryStrengths[MedianElement])
		{
			int iPercentIncrease = (viMilitaryStrengths[MedianElement] - iMajorStrength) * 100 / iMajorStrength;
			Localization::String sMilitaryPower = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_NEED_MORE_MILITARY_TT");
			sMilitaryPower << iPercentIncrease;
			sFactors += sMilitaryPower.toUTF8();
		}
	}

	Localization::String sOptions = Localization::Lookup("TXT_KEY_POP_CSTATE_PTP_ANY_CORRECT");
	sFactors += sOptions.toUTF8();

	Localization::String sNeedAllies = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_NEED_ALLIES_TT");
	Localization::String sNotClose = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_NOT_CLOSE_ENOUGH_TT");
	Localization::String sNeedTrade = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_NO_TRADE_TT");
	

	sFactors += sNeedAllies.toUTF8();
	if (IsAllies(eMajor))
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_ACHIEVED");
		sFactors += strPositiveFactor.toUTF8();
	}
	sFactors += sNeedTrade.toUTF8();
	if (GET_PLAYER(eMajor).GetTrade()->IsConnectedToPlayer(GetPlayer()->GetID()))
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_ACHIEVED");
		sFactors += strPositiveFactor.toUTF8();
	}
	sFactors += sNotClose.toUTF8();

	// This player must be able to build a trade route either by land or sea
	if (GET_PLAYER(eMajor).GetTrade()->GetNumTradeRoutesPossible() > 0)
	{
		if (GC.getGame().GetGameTrade()->CanCreateTradeRoute(eMajor, GetPlayer()->GetID(), DOMAIN_LAND) || GC.getGame().GetGameTrade()->CanCreateTradeRoute(eMajor, GetPlayer()->GetID(), DOMAIN_SEA))
		{
			Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_PLEDGE_ACHIEVED");
			sFactors += strPositiveFactor.toUTF8();
		}
	}
	return sFactors;
}

void CvMinorCivAI::DoChangeProtectionFromMajor(PlayerTypes eMajor, bool bProtect, bool bPledgeNowBroken)
{
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	if(bProtect == IsProtectedByMajor(eMajor)) return;

	if (bProtect)
	{
		if (!CanMajorProtect(eMajor, false))
			return;

		SetTurnLastPledgedProtectionByMajor(eMajor, GC.getGame().getGameTurn());

		if (MOD_EVENTS_MINORS_INTERACTION) 
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerProtected, eMajor, GetPlayer()->GetID());
		}
	}
	else
	{
		if (bPledgeNowBroken)
		{
			SetTurnLastPledgeBrokenByMajor(eMajor, GC.getGame().getGameTurn());

			int iEra = GET_PLAYER(eMajor).GetCurrentEra();
			if (iEra <= 0 || !MOD_BALANCE_CORE_MINORS)
				iEra = 1;

			ChangeFriendshipWithMajorTimes100(eMajor, iEra * /*-2000*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT));
		}

		if (MOD_EVENTS_MINORS_INTERACTION) 
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerRevoked, eMajor, GetPlayer()->GetID(), bPledgeNowBroken);
		}
	}

	m_abPledgeToProtect[eMajor] = bProtect;

	// In case we had a Pledge to Protect quest active, complete it now
	DoTestActiveQuestsForPlayer(eMajor, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_PLEDGE_TO_PROTECT);

	CvCity* pCity = m_pPlayer->getCapitalCity();
	if (pCity)
		pCity->updateStrengthValue();

	RecalculateRewards(eMajor);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}

bool CvMinorCivAI::CanMajorProtect(PlayerTypes eMajor, bool bIgnoreMilitaryRequirement)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	// If at war, may not protect
	if (GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(GetPlayer()->getTeam()))
		return false;

	// Must have positive INF
	if (GetEffectiveFriendshipWithMajor(eMajor) < /*0*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT))
		return false;

	// Must not be too soon after a previous pledge was broken
	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeBrokenTurn = GetTurnLastPledgeBrokenByMajor(eMajor);
	const int iGracePeriod = 20; //antonjs: todo: xml

	if (iLastPledgeBrokenTurn >= 0 && iLastPledgeBrokenTurn + iGracePeriod <= iCurrentTurn)
	{
		if ((iLastPledgeBrokenTurn + iGracePeriod) <= iCurrentTurn)
			SetTurnLastPledgeBrokenByMajor(eMajor, -1);
		else
			return false;
	}

	if (MOD_EVENTS_MINORS_INTERACTION) 
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanProtect, eMajor, GetPlayer()->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}

	if (MOD_BALANCE_CORE_MINOR_PTP_MINIMUM_VALUE)
	{
		if (!bIgnoreMilitaryRequirement)
		{
			int iMajorStrength = 1;
			std::vector<int> viMilitaryStrengths;

			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

				if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isMajorCiv())
				{
					int iStrength = GET_PLAYER(ePlayer).GetMilitaryMight();
					viMilitaryStrengths.push_back(iStrength);

					if (ePlayer == eMajor && iStrength > 0)
						iMajorStrength = iStrength;
				}
			}

			// This should not happen.
			if (viMilitaryStrengths.empty())
				return false;

			size_t MedianElement = viMilitaryStrengths.size() / 2; // this returns the median, except if the median is an average of two values, in which case it returns the lowest of the two
			std::nth_element(viMilitaryStrengths.begin(), viMilitaryStrengths.begin() + MedianElement, viMilitaryStrengths.end());
			if (iMajorStrength < viMilitaryStrengths[MedianElement])
				return false;
		}

		// Do they meet one of the three conditions?
		bool bValid = false;

		if (IsAllies(eMajor))
		{
			bValid = true;
		}
		else if (GET_PLAYER(eMajor).GetTrade()->IsConnectedToPlayer(GetPlayer()->GetID()))
		{
			bValid = true;
		}
		// This player must be able to build a trade route either by land or sea
		else if (GET_PLAYER(eMajor).GetTrade()->GetNumTradeRoutesPossible() > 0)
		{
			if (GC.getGame().GetGameTrade()->CanCreateTradeRoute(eMajor, GetPlayer()->GetID(), DOMAIN_LAND) || 
				GC.getGame().GetGameTrade()->CanCreateTradeRoute(eMajor, GetPlayer()->GetID(), DOMAIN_SEA))
			{
				bValid = true;
			}
		}

		if (!bValid)
			return false;
	}

	return true;
}

bool CvMinorCivAI::CanMajorStartProtection(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	return (CanMajorProtect(eMajor, false) && !IsProtectedByMajor(eMajor));
}

bool CvMinorCivAI::CanMajorWithdrawProtection(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	if (!IsProtectedByMajor(eMajor))
		return false;

	// Pledge is locked in for a certain time
	int iCurrentTurn = GC.getGame().getGameTurn();
	int iLastPledgeTurn = GetTurnLastPledgedProtectionByMajor(eMajor);
	int iGracePeriod = /*10 in CP, 50 in VP*/ GD_INT_GET(BALANCE_MINOR_PROTECTION_MINIMUM_DURATION);

	if (iLastPledgeTurn >= 0 && iLastPledgeTurn + iGracePeriod > iCurrentTurn)
		return false;

	if (MOD_EVENTS_MINORS_INTERACTION) 
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanRevoke, eMajor, GetPlayer()->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}			
	
	return true;
}

bool CvMinorCivAI::IsProtectedByMajor(PlayerTypes eMajor) const
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	if (!GET_PLAYER(eMajor).isAlive())
		return false;

	return m_abPledgeToProtect[eMajor];
}

bool CvMinorCivAI::IsProtectedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsProtectedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

int CvMinorCivAI::GetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastPledged[eMajor];
}

void CvMinorCivAI::SetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor, int iTurn)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastPledged[eMajor] = iTurn;
}

int CvMinorCivAI::GetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor) const
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastBrokePledge[eMajor];
}

void CvMinorCivAI::SetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor, int iTurn)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastBrokePledge[eMajor] = iTurn;
}



// ******************************
// ***** Friendship - with Benefits *****
// ******************************



/// Someone changed eras - does this affect their bonuses?
bool CvMinorCivAI::DoMajorCivEraChange(PlayerTypes ePlayer, EraTypes eNewEra)
{
	bool bSomethingChanged = false;

	MinorCivTraitTypes eTrait = GetTrait();

	// MARITIME
	if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldFood = 0, iNewFood = 0;

			// Capital
			iOldFood = GetFriendsCapitalFoodBonus(ePlayer);
			iNewFood = GetFriendsCapitalFoodBonus(ePlayer, eNewEra);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChangeTimes100(YIELD_FOOD, iNewFood - iOldFood);
			}

			// Other Cities
			iOldFood = GetFriendsOtherCityFoodBonus(ePlayer);
			iNewFood = GetFriendsOtherCityFoodBonus(ePlayer, eNewEra);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChangeTimes100(YIELD_FOOD, iNewFood - iOldFood);
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldFood = 0, iNewFood = 0;

			// Capital
			iOldFood = GetAlliesCapitalFoodBonus(ePlayer);
			iNewFood = GetAlliesCapitalFoodBonus(ePlayer);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCapitalYieldChangeTimes100(YIELD_FOOD, iNewFood - iOldFood);
			}

			// Other Cities
			iOldFood = GetAlliesOtherCityFoodBonus(ePlayer);
			iNewFood = GetAlliesOtherCityFoodBonus(ePlayer);

			if(iOldFood != iNewFood)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).ChangeCityYieldChangeTimes100(YIELD_FOOD, iNewFood - iOldFood);
			}
		}
	}

	// CULTURED
	else if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldCulture = GetCultureFlatFriendshipBonus(ePlayer) + GetCulturePerBuildingFriendshipBonus(ePlayer);
			int iNewCulture = GetCultureFlatFriendshipBonus(ePlayer, eNewEra) + GetCulturePerBuildingFriendshipBonus(ePlayer, eNewEra);

			if(iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldCulture = GetCultureFlatAlliesBonus(ePlayer) + GetCulturePerBuildingAlliesBonus(ePlayer);
			int iNewCulture = GetCultureFlatAlliesBonus(ePlayer, eNewEra) + GetCulturePerBuildingAlliesBonus(ePlayer, eNewEra);

			if(iOldCulture != iNewCulture)
			{
				bSomethingChanged = true;
			}
		}
	}

	// MERCANTILE
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldHappiness = 0, iNewHappiness = 0;

			iOldHappiness = GetHappinessFlatFriendshipBonus(ePlayer) + GetHappinessPerLuxuryFriendshipBonus(ePlayer);
			iNewHappiness = GetHappinessFlatFriendshipBonus(ePlayer, eNewEra) + GetHappinessPerLuxuryFriendshipBonus(ePlayer, eNewEra);

			if(iOldHappiness != iNewHappiness)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).CalculateNetHappiness();
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldHappiness = 0, iNewHappiness = 0;

			iOldHappiness = GetHappinessFlatAlliesBonus(ePlayer) + GetHappinessPerLuxuryAlliesBonus(ePlayer);
			iNewHappiness = GetHappinessFlatAlliesBonus(ePlayer, eNewEra) + GetHappinessPerLuxuryAlliesBonus(ePlayer, eNewEra);

			if(iOldHappiness != iNewHappiness)
			{
				bSomethingChanged = true;
				GET_PLAYER(ePlayer).CalculateNetHappiness();
			}
		}
	}

	// RELIGIOUS
	else if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
		// Friends
		if(IsFriends(ePlayer))
		{
			int iOldFaith = GetFaithFlatFriendshipBonus(ePlayer);
			int iNewFaith = GetFaithFlatFriendshipBonus(ePlayer, eNewEra);

			if(iOldFaith != iNewFaith)
			{
				bSomethingChanged = true;
			}
		}

		// Allies
		if(IsAllies(ePlayer))
		{
			int iOldFaith = GetFaithFlatAlliesBonus(ePlayer);
			int iNewFaith = GetFaithFlatAlliesBonus(ePlayer, eNewEra);

			if(iOldFaith != iNewFaith)
			{
				bSomethingChanged = true;
			}
		}
	}

	return bSomethingChanged;
}

// Science bonus when friends with a minor
int CvMinorCivAI::GetScienceFriendshipBonus()
{
	int iResult = GetScienceFriendshipBonusTimes100();
	iResult /= 100;

	return iResult;
}

int CvMinorCivAI::GetScienceFriendshipBonusTimes100()
{
	int iResult = GET_PLAYER(m_pPlayer->GetID()).GetScienceTimes100() * /*25*/ GD_INT_GET(MINOR_CIV_SCIENCE_BONUS_MULTIPLIER);
	iResult /= 100;

	return iResult;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentScienceFriendshipBonusTimes100(PlayerTypes ePlayer)
{
	if(GET_PLAYER(ePlayer).IsGetsScienceFromPlayer(GetPlayer()->GetID()))
		return GetScienceFriendshipBonusTimes100();

	return 0;
}

// Flat culture bonus when Friends with a minor
int CvMinorCivAI::GetCultureFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: here it was assumed that this CS is cultured type, which is nice for me prototyping, but later maybe add in a check for this
	int iCultureBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
	{
		iCultureBonus += /*13 in CP, 10 in VP*/ GD_INT_GET(FRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
	{
		iCultureBonus += /*6 in CP, 4 in VP*/ GD_INT_GET(FRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL);
	}

	// Pre-Medieval
	else
	{
		iCultureBonus += /*3 in CP, 1 in VP*/ GD_INT_GET(FRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT);
	}

	return iCultureBonus;
}

// Flat culture bonus when Allies with a minor
int CvMinorCivAI::GetCultureFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: here it was assumed that this CS is cultured type, which is nice for me prototyping, but later maybe add in a check for this
	int iCultureBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if (eCurrentEra >= eIndustrial)
	{
		iCultureBonus += /*13 in CP, 12 in VP*/ GD_INT_GET(ALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Medieval era or later
	else if (eCurrentEra >= eMedieval)
	{
		iCultureBonus += /*6*/ GD_INT_GET(ALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL);
	}

	// Pre-Medieval
	else
	{
		iCultureBonus += /*3 in CP, 2 in VP*/ GD_INT_GET(ALLIES_CULTURE_BONUS_AMOUNT_ANCIENT);
	}

	return iCultureBonus;
}

/// Flat-rate culture bonus
int CvMinorCivAI::GetCurrentCultureFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Don't give a bonus to a minor civ player
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only give a bonus if we are Cultural trait
	if(GetTrait() != MINOR_CIV_TRAIT_CULTURED)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetCultureFlatAlliesBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetCultureFlatFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

#if defined(MOD_BALANCE_CORE)
	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iAmount *= (iModifier + 100);
			iAmount /= 100;
		}
	}
#endif

	if (MOD_CITY_STATE_SCALE)
	{
		iAmount *= 100 + max( 1, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iAmount /= 100;
	}

	return iAmount;
}

//antonjs: This feature was prototyped, but later removed. Rewrite this function to add the bonus back in.
/// Cumulative per building culture bonus from friendship with a minor
int CvMinorCivAI::GetCulturePerBuildingFriendshipBonus(PlayerTypes /*ePlayer*/, EraTypes /*eAssumeEra*/)
{
	return 0;
}

//antonjs: This feature was prototyped, but later removed. Rewrite this function to add the bonus back in.
/// Cumulative per building culture bonus from being allies with a minor
int CvMinorCivAI::GetCulturePerBuildingAlliesBonus(PlayerTypes /*ePlayer*/, EraTypes /*eAssumeEra*/)
{
	return 0;
}

//antonjs: This feature was prototyped, but later removed. It will return 0 (no bonus).
int CvMinorCivAI::GetCurrentCulturePerBuildingBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Don't give a bonus to a minor civ player
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only give a bonus if we are Cultural trait
	if(GetTrait() != MINOR_CIV_TRAIT_CULTURED)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetCulturePerBuildingAlliesBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetCulturePerBuildingFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	return iAmount;
}

int CvMinorCivAI::GetCurrentCultureBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	int iAmount = 0;

	iAmount += GetCurrentCultureFlatBonus(ePlayer);
	iAmount += GetCurrentCulturePerBuildingBonus(ePlayer); //antonjs: This feature was prototyped, but later removed. Its value is 0 (no bonus).

	iAmount *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iAmount /= 100;

	return iAmount;
}

/// Flat happiness bonus from friendship with a minor
int CvMinorCivAI::GetHappinessFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return /*3*/ GD_INT_GET(FRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL);

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return /*3*/ GD_INT_GET(FRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL);

	// Pre-Medieval
	else
		return /*2*/ GD_INT_GET(FRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT);
}

/// Flat happiness bonus from being allies with a minor
int CvMinorCivAI::GetHappinessFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return /*0*/ GD_INT_GET(ALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL);

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return /*0*/ GD_INT_GET(ALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL);

	// Pre-Medieval
	else
		return /*0*/ GD_INT_GET(ALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT);
}

/// Flat happiness bonus currently in effect
int CvMinorCivAI::GetCurrentHappinessFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Mercantile trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetHappinessFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetHappinessFlatFriendshipBonus(ePlayer);

	if (MOD_CITY_STATE_SCALE)
	{
		iAmount *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iAmount /= 100;
	}

	iAmount *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iAmount /= 100;

	return iAmount;
}

/// Cumulative per luxury happiness bonus from friendship with a minor
int CvMinorCivAI::GetHappinessPerLuxuryFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: optimize
	int iNumLuxuries = 0;
	ResourceTypes eResource;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumLuxuries++;
			}
		}
	}

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return (iNumLuxuries * /*0*/ GD_INT_GET(FRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL));

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return (iNumLuxuries * /*0*/ GD_INT_GET(FRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL));

	// Pre-Medieval
	else
		return (iNumLuxuries * /*0*/ GD_INT_GET(FRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT));
}

/// Cumulative per luxury happiness bonus from being allies with a minor
int CvMinorCivAI::GetHappinessPerLuxuryAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	//antonjs: consider: optimize
	int iNumLuxuries = 0;
	ResourceTypes eResource;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) > 0)
		{
			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo != NULL && pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				iNumLuxuries++;
			}
		}
	}

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);

	// Industrial era or Later
	if(eCurrentEra >= eIndustrial)
		return (iNumLuxuries * /*0*/ GD_INT_GET(ALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL));

	// Medieval era or later
	else if(eCurrentEra >= eMedieval)
		return (iNumLuxuries * /*0*/ GD_INT_GET(ALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL));

	// Pre-Medieval
	else
		return (iNumLuxuries * /*0*/ GD_INT_GET(ALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT));
}

/// Per luxury happiness bonus currently in effect
int CvMinorCivAI::GetCurrentHappinessPerLuxuryBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Mercantile trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetHappinessPerLuxuryAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetHappinessPerLuxuryFriendshipBonus(ePlayer);
	return iAmount;
}

/// Total happiness bonus from this minor civ for this player
int CvMinorCivAI::GetCurrentHappinessBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentHappinessFlatBonus(ePlayer);
	iValue += GetCurrentHappinessPerLuxuryBonus(ePlayer);

	return iValue;
}

int CvMinorCivAI::GetFaithFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iFaithBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iFaithBonus += /*8 in CP, 12 in VP*/ GD_INT_GET(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iFaithBonus += /*4 in CP, 9 in VP*/ GD_INT_GET(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE);
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iFaithBonus += /*4 in CP, 7 in VP*/ GD_INT_GET(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL);
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iFaithBonus += /*2*/ GD_INT_GET(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL);
	}

	// Ancient era
	else
	{
		iFaithBonus += /*2 in CP, 1 in VP*/ GD_INT_GET(FRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT);
	}

	return iFaithBonus;
}

int CvMinorCivAI::GetFaithFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iFaithBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iFaithBonus += /*8 in CP, 12 in VP*/ GD_INT_GET(ALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iFaithBonus += /*4 in CP, 9 in VP*/ GD_INT_GET(ALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE);
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iFaithBonus += /*4 in CP, 7 in VP*/ GD_INT_GET(ALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL);
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iFaithBonus += /*2 in CP, 3 in VP*/ GD_INT_GET(ALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL);
	}

	// Ancient era
	else
	{
		iFaithBonus += /*2*/ GD_INT_GET(ALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT);
	}

	return iFaithBonus;
}

int CvMinorCivAI::GetCurrentFaithFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Religious trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_RELIGIOUS)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetFaithFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetFaithFlatFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iAmount *= (iModifier + 100);
			iAmount /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iAmount *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iAmount /= 100;
	}

	iAmount *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iAmount /= 100;

	return iAmount;
}

/// Total faith bonus from this minor civ for this player
int CvMinorCivAI::GetCurrentFaithBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentFaithFlatBonus(ePlayer);

	return iValue;
}

#if defined(MOD_BALANCE_CORE)
int CvMinorCivAI::GetGoldFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iGoldBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iGoldBonus += /*4*/ GD_INT_GET(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iGoldBonus += /*3*/ GD_INT_GET(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE);
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iGoldBonus += /*2*/ GD_INT_GET(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL);
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iGoldBonus += /*1*/ GD_INT_GET(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL);
	}

	// Ancient era
	else
	{
		iGoldBonus += /*1*/ GD_INT_GET(FRIENDS_GOLD_FLAT_BONUS_AMOUNT_ANCIENT);
	}

	return iGoldBonus;
}

int CvMinorCivAI::GetGoldFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iGoldBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iGoldBonus += /*6*/ GD_INT_GET(ALLIES_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iGoldBonus += /*5*/ GD_INT_GET(ALLIES_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE);
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iGoldBonus += /*4*/ GD_INT_GET(ALLIES_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL);
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iGoldBonus += /*3*/ GD_INT_GET(ALLIES_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL);
	}

	// Ancient era
	else
	{
		iGoldBonus += /*2*/ GD_INT_GET(ALLIES_GOLD_FLAT_BONUS_AMOUNT_ANCIENT);
	}

	return iGoldBonus;
}

int CvMinorCivAI::GetCurrentGoldFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Mercantile trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_MERCANTILE)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetGoldFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetGoldFlatFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iAmount *= (iModifier + 100);
			iAmount /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iAmount *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iAmount /= 100;
	}

	iAmount *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iAmount /= 100;

	return iAmount;
}

/// Total Gold bonus from this minor civ for this player
int CvMinorCivAI::GetCurrentGoldBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentGoldFlatBonus(ePlayer);

	return iValue;
}

int CvMinorCivAI::GetScienceFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iScienceBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iScienceBonus += /*10*/ GD_INT_GET(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iScienceBonus += /*6*/ GD_INT_GET(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE);
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iScienceBonus += /*4*/ GD_INT_GET(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL);
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iScienceBonus += /*2*/ GD_INT_GET(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL);
	}

	// Ancient era
	else
	{
		iScienceBonus += /*1*/ GD_INT_GET(FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT);
	}

	return iScienceBonus;
}

int CvMinorCivAI::GetScienceFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra) const
{
	int iScienceBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eIndustrial = (EraTypes) GC.getInfoTypeForString("ERA_INDUSTRIAL", true);
	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);
	EraTypes eMedieval = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	EraTypes eClassical = (EraTypes) GC.getInfoTypeForString("ERA_CLASSICAL", true);

	// Industrial era or later
	if(eCurrentEra >= eIndustrial)
	{
		iScienceBonus += /*10*/ GD_INT_GET(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL);
	}

	// Renaissance era
	else if(eCurrentEra >= eRenaissance)
	{
		iScienceBonus += /*8*/ GD_INT_GET(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE);
	}

	// Medieval era
	else if(eCurrentEra >= eMedieval)
	{
		iScienceBonus += /*6*/ GD_INT_GET(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL);
	}

	// Classical era
	else if(eCurrentEra >= eClassical)
	{
		iScienceBonus += /*4*/ GD_INT_GET(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL);
	}

	// Ancient era
	else
	{
		iScienceBonus += /*3*/ GD_INT_GET(ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT);
	}

	return iScienceBonus;
}

int CvMinorCivAI::GetCurrentScienceFlatBonus(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_PLAYERS) return 0;

	// Minor civ players do not get a bonus
	if(ePlayer >= MAX_MAJOR_CIVS)
		return 0;

	// Only for Religious trait minors
	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iAmount = 0;
	if(IsAllies(ePlayer))
		iAmount += GetScienceFlatAlliesBonus(ePlayer);
	if(IsFriends(ePlayer))
		iAmount += GetScienceFlatFriendshipBonus(ePlayer);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iAmount *= (iModifier + 100);
		iAmount /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iAmount *= (iModifier + 100);
			iAmount /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iAmount *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iAmount /= 100;
	}

	iAmount *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iAmount /= 100;

	return iAmount;
}

/// Total faith bonus from this minor civ for this player
int CvMinorCivAI::GetCurrentScienceBonus(PlayerTypes ePlayer)
{
	int iValue = 0;

	iValue += GetCurrentScienceFlatBonus(ePlayer);

	return iValue;
}
#endif
// Food bonus when Friends with a minor - additive with general city bonus
int CvMinorCivAI::GetFriendsCapitalFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

	// Medieval era or sooner
	if(eCurrentEra < eRenaissance)
		iBonus = /*200 in CP, 300 in VP*/ GD_INT_GET(FRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE);

	// Renaissance era or later
	else
		iBonus = /*200 in CP, 600 in VP*/ GD_INT_GET(FRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iBonus *= (iModifier + 100);
			iBonus /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iBonus *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iBonus /= 100;
	}

	iBonus *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iBonus /= 100;

	return iBonus;
}

// Food bonus when Friends with a minor
int CvMinorCivAI::GetFriendsOtherCityFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra)
{
	int iBonus = 0;

	EraTypes eCurrentEra = eAssumeEra;
	if(eCurrentEra == NO_ERA)
		eCurrentEra = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetCurrentEra();

	EraTypes eRenaissance = (EraTypes) GC.getInfoTypeForString("ERA_RENAISSANCE", true);

	// Medieval era or sooner
	if(eCurrentEra < eRenaissance)
		iBonus = /*0 in CP, 50 in VP*/ GD_INT_GET(FRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE);

	// Renaissance era or later
	else
		iBonus = /*0 in CP, 100 in VP*/ GD_INT_GET(FRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iBonus *= (iModifier + 100);
			iBonus /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iBonus *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iBonus /= 100;
	}

	iBonus *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iBonus /= 100;

	return iBonus;
}

// Food bonus when Allies with a minor - additive with general city bonus
int CvMinorCivAI::GetAlliesCapitalFoodBonus(PlayerTypes ePlayer)
{
	int iBonus = /*0 in CP, 200 in VP*/ GD_INT_GET(ALLIES_CAPITAL_FOOD_BONUS_AMOUNT);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iBonus *= (iModifier + 100);
			iBonus /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iBonus *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iBonus /= 100;
	}

	iBonus *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iBonus /= 100;

	return iBonus;
}

// Food bonus when Allies with a minor
int CvMinorCivAI::GetAlliesOtherCityFoodBonus(PlayerTypes ePlayer)
{
	int iBonus = /*100 in CP, 200 in VP*/ GD_INT_GET(ALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT);

	// Modify the bonus if called for by our trait
	int iModifier = GET_PLAYER(ePlayer).GetPlayerTraits()->GetCityStateBonusModifier();
	if(iModifier > 0)
	{
		iBonus *= (iModifier + 100);
		iBonus /= 100;
	}

	if (IsSameReligionAsMajor(ePlayer))
	{
		iModifier = GET_PLAYER(ePlayer).GetReligions()->GetCityStateYieldModifier(ePlayer);
		if (iModifier > 0)
		{
			iBonus *= (iModifier + 100);
			iBonus /= 100;
		}
	}

	if (MOD_CITY_STATE_SCALE)
	{
		iBonus *= 100 + max(0, GetPlayer()->getNumCities() - 1) * /*0*/ GD_INT_GET(CITY_STATE_SCALE_PER_CITY_MOD);
		iBonus /= 100;
	}

	iBonus *= (100 + GET_PLAYER(ePlayer).GetCSYieldBonusModifier());
	iBonus /= 100;

	return iBonus;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentCapitalFoodBonus(PlayerTypes ePlayer)
{
	// This guy isn't Maritime
	if(GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
	{
		iAmount += GetAlliesCapitalFoodBonus(ePlayer);
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);
	}

	if(IsFriends(ePlayer))
	{
		iAmount += GetFriendsCapitalFoodBonus(ePlayer);
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);
	}

	return iAmount;
}

/// How much are we getting RIGHT NOW (usually 0)
int CvMinorCivAI::GetCurrentOtherCityFoodBonus(PlayerTypes ePlayer)
{
	// This guy isn't Maritime
	if(GetTrait() != MINOR_CIV_TRAIT_MARITIME)
		return 0;

	int iAmount = 0;

	if(IsAllies(ePlayer))
		iAmount += GetAlliesOtherCityFoodBonus(ePlayer);

	if(IsFriends(ePlayer))
		iAmount += GetFriendsOtherCityFoodBonus(ePlayer);

	return iAmount;
}

// Figures out how long before we spawn a free unit for ePlayer
void CvMinorCivAI::DoSeedUnitSpawnCounter(PlayerTypes ePlayer, bool bBias)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTurns = GetSpawnBaseTurns(ePlayer);

	// Add some randomness
	int iRand = /*3*/ GD_INT_GET(FRIENDS_RAND_TURNS_UNIT_SPAWN);
	iNumTurns += GC.getGame().getSmallFakeRandNum(iRand, m_pPlayer->GetPseudoRandomSeed());

	// If we're biasing the result then decrease the number of turns
	if (bBias)
	{
		iNumTurns *= /*50*/ GD_INT_GET(UNIT_SPAWN_BIAS_MULTIPLIER);
		iNumTurns /= 100;
	}

	SetUnitSpawnCounter(ePlayer, std::max(1, iNumTurns));
}

// How long before we spawn a free unit for ePlayer?
int CvMinorCivAI::GetUnitSpawnCounter(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as defined during Reset()

	return m_aiUnitSpawnCounter[ePlayer];
}

// Sets how long before we spawn a free unit for ePlayer
void CvMinorCivAI::SetUnitSpawnCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiUnitSpawnCounter[ePlayer] = iValue;
}

// Changes how long before we spawn a free unit for ePlayer
void CvMinorCivAI::ChangeUnitSpawnCounter(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetUnitSpawnCounter(ePlayer, GetUnitSpawnCounter(ePlayer) + iChange);
}

/// Allowed to spawn Units for ePlayer?
bool CvMinorCivAI::IsUnitSpawningAllowed(PlayerTypes ePlayer)
{
	// Must have met ePlayer
	if(!IsHasMetPlayer(ePlayer))
		return false;

	// Must be Militaristic
	if(GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return false;

	// Can't be at war!
	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;

	// Must be Friends
	if(!IsFriends(ePlayer))
		return false;

	// We must be alive
	if(!GetPlayer()->isAlive())
		return false;

	// They must be alive
	if(!GET_PLAYER(ePlayer).isAlive())
		return false;

	// They must allow unit spawning
	if (IsUnitSpawningDisabled(ePlayer))
		return false;

	return true;
}

/// Has the player chosen to disable Unit spawning?
bool CvMinorCivAI::IsUnitSpawningDisabled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return m_abUnitSpawningDisabled[ePlayer];
}

/// Set the player chosen to disable Unit spawning
void CvMinorCivAI::SetUnitSpawningDisabled(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_abUnitSpawningDisabled[ePlayer] = bValue;
}

/// Create a unit
CvUnit* CvMinorCivAI::DoSpawnUnit(PlayerTypes eMajor, bool bLocal, bool bExplore, bool bCityStateAnnexed)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return NULL;

	if (bCityStateAnnexed && !GET_PLAYER(eMajor).GetPlayerTraits()->IsAnnexedCityStatesGiveYields())
		return NULL;

	if (bExplore && !MOD_GLOBAL_CS_GIFTS)
		return NULL;

	// Unit spawning is not allowed (manually disabled, or major is over supply limit)
	bool bCanSupply = GET_PLAYER(eMajor).GetNumUnitsToSupply() < GET_PLAYER(eMajor).GetNumUnitsSupplied(); // this works when we're at the limit
	if (!bCanSupply || (IsUnitSpawningDisabled(eMajor) && !bCityStateAnnexed))
		return NULL;

	// Minor has no capital
	CvCity* pMinorCapital = NULL;
	if (!bCityStateAnnexed)
	{
		pMinorCapital = GetPlayer()->getCapitalCity();
	}
	else
	{
		CvPlot* pStartingPlot = GC.getMap().plotCheckInvalid(GetPlayer()->GetOriginalCapitalX(), GetPlayer()->GetOriginalCapitalY());
		if(pStartingPlot)
			pMinorCapital = pStartingPlot->getPlotCity();
	}

	if (!pMinorCapital)
		return NULL;

	if (pMinorCapital->plot() == NULL)
		return NULL;

	// Major has no capital
	CvCity* pMajorCapital = GET_PLAYER(eMajor).getCapitalCity();
	if (!pMajorCapital)
		return NULL;

	if (pMajorCapital->plot() == NULL)
		return NULL;

	bool bBoatsAllowed = MOD_GLOBAL_CS_GIFT_SHIPS && pMinorCapital->isCoastal(/*10*/ GD_INT_GET(MIN_WATER_SIZE_FOR_OCEAN));

	// What's their closest city? If they have at least one coastal city, allow spawning naval units
	CvCity* pClosestCity = NULL;
	CvCity* pClosestCoastalCity = NULL;
	if (bCityStateAnnexed)
	{
		pClosestCity = pMinorCapital;
		pClosestCoastalCity = pMinorCapital;
	}
	else
	{
		pClosestCoastalCity = bBoatsAllowed ? OperationalAIHelpers::GetClosestFriendlyCoastalCity(eMajor, pMinorCapital->plot(), /*10*/ GD_INT_GET(MIN_WATER_SIZE_FOR_OCEAN)) : NULL;

		int iLowestDistance = MAX_INT;
		int iLowestCoastalDistance = MAX_INT;
		// If we are coastal, but we can't find a connection, then keep track of *any* city that is connected to an ocean (otherwise, don't bother checking)
		if (!bBoatsAllowed || pClosestCoastalCity != NULL)
			iLowestCoastalDistance = MIN_INT;

		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(eMajor).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eMajor).nextCity(&iCityLoop))
		{
			if (pLoopCity->plot() == NULL)
				continue;

			int iDistance = plotDistance(*pMinorCapital->plot(), *pLoopCity->plot());
			if (iDistance < iLowestDistance)
			{
				iLowestDistance = iDistance;
				pClosestCity = pLoopCity;

				if (iDistance < iLowestCoastalDistance && pLoopCity->isCoastal(/*10*/ GD_INT_GET(MIN_WATER_SIZE_FOR_OCEAN)))
				{
					iLowestCoastalDistance = iDistance;
					pClosestCoastalCity = pLoopCity;
				}
			}
		}
	}

	if (!pClosestCity)
		pClosestCity = pMajorCapital;

	if (!pClosestCoastalCity)
		bBoatsAllowed = false;

	// Which Unit to choose?
	UnitTypes eUnit = NO_UNIT;

	// If they're our ally, should we give our unique unit?
	if (GetAlly() == eMajor || bCityStateAnnexed)
	{
		UnitTypes eUniqueUnit = GetUniqueUnit();
		if (eUniqueUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUniqueUnit);
			if (pkUnitInfo)
			{
				// If naval, major must have a coastal city
				if (bBoatsAllowed || pkUnitInfo->GetDomainType() != DOMAIN_SEA)
				{
					// If scout, must be a scout
					if (!bExplore || pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE || pkUnitInfo->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
					{
						// Ally must have unit's prereq tech
						TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
						if (ePrereqTech == NO_TECH || GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
						{
							// Ally must NOT have unit's obsolete tech
							TechTypes eObsoleteTech = (TechTypes) pkUnitInfo->GetObsoleteTech();
							if (eObsoleteTech == NO_TECH || !GET_TEAM(GET_PLAYER(eMajor).getTeam()).GetTeamTechs()->HasTech(eObsoleteTech))
							{
								bool bFailedResourceCheck = false;

								// Ally must meet this unit's strategic resource requirements
								for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
								{
									const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
									CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
									if (pkResourceInfo)
									{
										int iResourceRequirement = pkUnitInfo->GetResourceQuantityRequirement(eResource);

										if (iResourceRequirement > 0 && iResourceRequirement > GET_PLAYER(eMajor).getNumResourceAvailable(eResource, true))
										{
											bFailedResourceCheck = true;
											break;
										}
									}
								}

								if (!bFailedResourceCheck)
									eUnit = eUniqueUnit;
							}
						}
					}
				}
			}
		}
	}

	if (eUnit == NO_UNIT)
	{
		if (bExplore) // Free exploration unit (First contact bonus)
			eUnit = GC.getGame().GetCsGiftSpawnUnitType(eMajor, bBoatsAllowed);
		else
			eUnit = GC.getGame().GetCompetitiveSpawnUnitType(eMajor, /*bIncludeUUs*/ false, /*bIncludeRanged*/ true, bBoatsAllowed);
	}

	if (eUnit == NO_UNIT)
		return NULL;

	// Where to put the Unit?
	CvPlot* pUnitPlot = NULL;
	CvCity* pXPCity = NULL;

	// Local units spawn in the minor's capital
	if (bLocal)
	{
		pXPCity = pMinorCapital;
		pUnitPlot = pMinorCapital->GetPlotForNewUnit(eUnit);
		if (!pUnitPlot)
			pUnitPlot = pMinorCapital->plot();
	}
	else
	{
		// Is this a naval unit? Spawn it in their closest coastal city
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo && pkUnitInfo->GetDomainType() == DOMAIN_SEA)
		{
			pXPCity = MOD_GLOBAL_CS_GIFTS_LOCAL_XP ? pClosestCoastalCity : pMinorCapital;
			pUnitPlot = pClosestCoastalCity->GetPlotForNewUnit(eUnit);
			if (!pUnitPlot)
				pUnitPlot = pClosestCoastalCity->plot();
		}
		else
		{
			pXPCity = MOD_GLOBAL_CS_GIFTS_LOCAL_XP ? pClosestCity : pMinorCapital;
			pUnitPlot = pClosestCity->GetPlotForNewUnit(eUnit);
			if (!pUnitPlot)
				pUnitPlot = pClosestCity->plot();
		}
	}

	// Now actually spawn the Unit
	CvUnit* pNewUnit = GET_PLAYER(eMajor).initUnit(eUnit, pUnitPlot->getX(), pUnitPlot->getY());
	if (!pNewUnit)
		return NULL;

	if (pNewUnit->canMoveInto(*pNewUnit->plot(),CvUnit::MOVEFLAG_DESTINATION) || pNewUnit->jumpToNearestValidPlotWithinRange(3))
	{
		// Bonus experience for CS Units (Siam UA)
		if (GET_PLAYER(eMajor).GetPlayerTraits()->GetCityStateBonusModifier() > 0)
			pNewUnit->changeExperienceTimes100(1000);

		if (MOD_BALANCE_VP)
			pXPCity->addProductionExperience(pNewUnit);

		if (!bCityStateAnnexed)
		{
			// Reseed counter
			DoSeedUnitSpawnCounter(eMajor);

			// Notify the player
			if (GET_PLAYER(eMajor).GetNotifications())
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_STATE_UNIT_SPAWN");
				strMessage << GetPlayer()->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_STATE_UNIT_SPAWN");
				strSummary << GetPlayer()->getNameKey();

				AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor, pNewUnit->getX(), pNewUnit->getY());
			}
		}
		else
		{
			if (GET_PLAYER(eMajor).GetNotifications())
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_ANNEXED_CITY_STATE_UNIT_SPAWN");
				strMessage << GetPlayer()->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_ANNEXED_CITY_STATE_UNIT_SPAWN");
				strSummary << GetPlayer()->getNameKey();

				AddNotification(strMessage.toUTF8(), strSummary.toUTF8(), eMajor, pNewUnit->getX(), pNewUnit->getY());
			}
		}
	}
	else
	{
		pNewUnit->kill(false); // Could not find a spot!
		return NULL;
	}

	return pNewUnit;
}

/// Time to spawn a Unit?
void CvMinorCivAI::DoUnitSpawnTurn()
{
	// Loop through all players and see if we should give them a Unit
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajor = (PlayerTypes) iMajorLoop;

		if (IsUnitSpawningAllowed(eMajor))
		{
			// Tick down
			if (GetUnitSpawnCounter(eMajor) > 0)
				ChangeUnitSpawnCounter(eMajor, -1);

			// Time to spawn!
			if (GetUnitSpawnCounter(eMajor) == 0)
			{
				CvUnit* pSpawnUnit = DoSpawnUnit(eMajor);

#if defined(MOD_EVENTS_MINORS_GIFTS)
				// Send an event with the details
				if (MOD_EVENTS_MINORS_GIFTS && pSpawnUnit != NULL)
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_MinorGiftUnit, GetPlayer()->GetID(), eMajor, pSpawnUnit->getUnitType());
#endif
			}
		}
	}
}

/// What is the base number of turns between unit spawns, before randomness is applied?
int CvMinorCivAI::GetSpawnBaseTurns(PlayerTypes ePlayer, bool bCityStateAnnexed)
{
	// This guy isn't militaristic
	if (GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	// Rome Exception: Annexed Militaristic City-States continue to give units at an allied level
	if (bCityStateAnnexed)
	{
		int iNumTurns = 100 * (/*19*/ GD_INT_GET(FRIENDS_BASE_TURNS_UNIT_SPAWN) + /*-3*/ GD_INT_GET(ALLIES_EXTRA_TURNS_UNIT_SPAWN));
		// Modify for Game Speed
		iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
		iNumTurns /= 100;
		return iNumTurns / 100;
	}

	// Not friends
	if (!IsFriends(ePlayer))
		return 0;

	int iNumTurns = 100 * /*19*/ GD_INT_GET(FRIENDS_BASE_TURNS_UNIT_SPAWN);

	// If relations are at allied level then reduce spawn counter
	if (IsAllies(ePlayer))
		iNumTurns += 100 * /*-3*/ GD_INT_GET(ALLIES_EXTRA_TURNS_UNIT_SPAWN);

	// Modify for Game Speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iNumTurns /= 100;

	// Modify for policies
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iPolicyMod = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_UNIT_FREQUENCY_MODIFIER);
	if (iPolicyMod > 0)
	{
		if (GET_TEAM(kPlayer.getTeam()).HasCommonEnemy(m_pPlayer->getTeam()))
		{
			iNumTurns *= 100;
			iNumTurns /= (100 + iPolicyMod);
		}
	}

	return iNumTurns / 100;
}

/// What is the average number of turns between unit spawns?
int CvMinorCivAI::GetCurrentSpawnEstimate(PlayerTypes ePlayer)
{
	// Not friends
	if (!IsFriends(ePlayer))
		return 0;

	// This guy isn't militaristic
	if (GetTrait() != MINOR_CIV_TRAIT_MILITARISTIC)
		return 0;

	int iNumTurns = GetSpawnBaseTurns(ePlayer) * 100;

	int iRand = 100 * /*3*/ GD_INT_GET(FRIENDS_RAND_TURNS_UNIT_SPAWN);
	iNumTurns += (iRand / 2);

	return iNumTurns / 100;
}

/// Has the player chosen to disable Influence rewards from quests?
bool CvMinorCivAI::IsQuestInfluenceDisabled(PlayerTypes ePlayer) const
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;
	return m_abQuestInfluenceDisabled[ePlayer];
}

void CvMinorCivAI::SetQuestInfluenceDisabled(PlayerTypes ePlayer, bool bValue)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	if (bValue != m_abQuestInfluenceDisabled[ePlayer])
	{
		m_abQuestInfluenceDisabled[ePlayer] = bValue;
		if (bValue)
			DisableQuestInfluence(ePlayer);
		else
			EnableQuestInfluence(ePlayer);
	}
}

void CvMinorCivAI::DisableQuestInfluence(PlayerTypes ePlayer)
{
	if (!GetPlayer()->isAlive())
		return;

	if (m_QuestsGiven.empty() || m_QuestsGiven.size() <= (size_t)ePlayer)
		return;

	for (QuestListForPlayer::iterator itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
		itr_quest->DisableInfluence(ePlayer);
}

void CvMinorCivAI::EnableQuestInfluence(PlayerTypes ePlayer)
{
	if (!GetPlayer()->isAlive())
		return;

	if (m_QuestsGiven.empty() || m_QuestsGiven.size() <= (size_t)ePlayer)
		return;

	for (QuestListForPlayer::iterator itr_quest = m_QuestsGiven[ePlayer].begin(); itr_quest != m_QuestsGiven[ePlayer].end(); itr_quest++)
		itr_quest->EnableInfluence(ePlayer);
}


/// Has this minor been married by us?
bool CvMinorCivAI::IsMarried(PlayerTypes eMajor) const
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	return m_abIsMarried[eMajor];
}

void CvMinorCivAI::SetMajorMarried(PlayerTypes eMajor, bool bValue)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;
	m_abIsMarried[eMajor] = bValue;
}

/// What player has bought out this minor?
PlayerTypes CvMinorCivAI::GetMajorBoughtOutBy() const
{
	return m_eMajorBoughtOutBy;
}

void CvMinorCivAI::SetMajorBoughtOutBy(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;
	m_eMajorBoughtOutBy = eMajor;
}

/// Has this minor been bought out by someone?
bool CvMinorCivAI::IsBoughtOut() const
{
	return GetMajorBoughtOutBy() != NO_PLAYER;
}


/// Can this minor be bought out by this major? (Austria VP UA)
bool CvMinorCivAI::CanMajorDiploMarriage(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	// Can't be already married
	if (IsMarried(eMajor))
		return false;

	// We must both be alive and they must have a capital
	if (!GET_PLAYER(eMajor).isAlive() || !GetPlayer()->isAlive() || GET_PLAYER(eMajor).getCapitalCity() == NULL)
		return false;
	
	// They must have the ability to marry City-States
	if (!GET_PLAYER(eMajor).GetPlayerTraits()->IsDiplomaticMarriage())
		return false;

	// Can't be at war
	if (GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eMajor).getTeam()))
		return false;

	// Must be allies
	if (!IsAllies(eMajor))
		return false;

	// ...for a while
	if (GetAlliedTurns() < /*5 in CP, 10 in VP*/ GD_INT_GET(MINOR_CIV_BUYOUT_TURNS))
		return false;

	// They must have enough Gold
	if (GET_PLAYER(eMajor).GetTreasury()->GetGold() < GetMarriageCost(eMajor))
		return false;

	return true;
}

/// Can this minor be bought out by this major? (Austria BNW UA)
bool CvMinorCivAI::CanMajorBuyout(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;

	// We must both be alive and they must have a capital
	if (!GET_PLAYER(eMajor).isAlive() || !GetPlayer()->isAlive() || GET_PLAYER(eMajor).getCapitalCity() == NULL)
		return false;
	
	// They must have the ability to purchase City-States
	if (!GET_PLAYER(eMajor).IsAbleToAnnexCityStates())
		return false;

	// Can't be at war
	if (GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eMajor).getTeam()))
		return false;

	// Not allowed in OCC games
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(eMajor).isHuman())
		return false;

	// Must be allies
	if (!IsAllies(eMajor))
		return false;

	// ...for a while
	if (GetAlliedTurns() < /*5 in CP, 10 in VP*/ GD_INT_GET(MINOR_CIV_BUYOUT_TURNS))
		return false;

	// They must have enough Gold
	if (GET_PLAYER(eMajor).GetTreasury()->GetGold() < GetBuyoutCost(eMajor))
		return false;

	if (MOD_EVENTS_MINORS_INTERACTION) 
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanBuyOut, eMajor, GetPlayer()->GetID()) == GAMEEVENTRETURN_FALSE)
			return false;
	}
	
	return true;
}


/// What is the cost for eMajor to marry this City-State? (Austria VP UA)
int CvMinorCivAI::GetMarriageCost(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	int iGold = /*200*/ GD_INT_GET(BALANCE_MARRIAGE_COST_BASE);

	// Increase based on the number of marriages we've already got.
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		if (GET_PLAYER(eMinorLoop).isMinorCiv() && GET_PLAYER(eMinorLoop).GetMinorCivAI()->IsMarried(eMajor))
		{
			iGold += /*200*/ GD_INT_GET(BALANCE_MARRIAGE_COST_INCREASE_PER_PREVIOUS_MARRIAGE);
		}
	}

	// Game Speed Mod
	iGold *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iGold /= 100;

	// Rounding
	int iVisibleDivisor = /*5*/ max(GD_INT_GET(MINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR), 1);
	iGold /= iVisibleDivisor;
	iGold *= iVisibleDivisor;

	return iGold;
}

/// What is the cost for eMajor to buy out this City-State? (Austria BNW UA)
int CvMinorCivAI::GetBuyoutCost(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;

	int iGold = /*500*/ GD_INT_GET(MINOR_CIV_BUYOUT_COST);

	// Game Speed Mod
	iGold *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iGold /= 100;

	// Add in the scrap cost of all this minor's units
	int iUnitLoop = 0;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		iGold += pLoopUnit->GetScrapGold();
	}

	// Rounding
	int iVisibleDivisor = /*5*/ max(GD_INT_GET(MINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR), 1);
	iGold /= iVisibleDivisor;
	iGold *= iVisibleDivisor;

	return iGold;
}


/// Do a City-State buyout (handles both of Austria's VP marriage and BNW buyout abilities)
void CvMinorCivAI::DoBuyout(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return;

	// Marriage overrides buyout!
	bool bMarriage = GET_PLAYER(eMajor).GetPlayerTraits()->IsDiplomaticMarriage();

	if (bMarriage && !CanMajorDiploMarriage(eMajor))
		return;

	if (!bMarriage && !CanMajorBuyout(eMajor))
		return;

	// Locate our current capital (for the notification)
	int iLoop = 0;
	int iCapitalX = -1;
	int iCapitalY = -1;
	int iNumUnits = 0;
	for (CvCity* pLoopCity = GetPlayer()->firstCity(&iLoop, true); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop, true))
	{
		if (pLoopCity->isCapital())
		{
			iCapitalX = pLoopCity->getX();
			iCapitalY = pLoopCity->getY();
			break;
		}
	}

	// Pay the cost
	const int iBuyoutCost = bMarriage ? GetMarriageCost(eMajor) : GetBuyoutCost(eMajor);
	GET_PLAYER(eMajor).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString( CivsList(1,GetPlayer()->GetID()) ), iBuyoutCost,6);
	GET_PLAYER(eMajor).GetTreasury()->ChangeGold(-iBuyoutCost);
	GET_PLAYER(eMajor).GetDiplomacyAI()->LogMinorCivBuyout(GetPlayer()->GetID(), iBuyoutCost, /*bSaving*/ false);

	if (bMarriage)
		SetMajorMarried(eMajor, true);
	else
	{
		SetMajorBoughtOutBy(eMajor);
		iNumUnits = TransferUnitsAndCitiesToMajor(eMajor);
	}

	// Send out notifications to everyone
	TeamTypes eBuyerTeam = GET_PLAYER(eMajor).getTeam();
	int iCoinToss = GC.getGame().getSmallFakeRandNum(1, m_pPlayer->GetPseudoRandomSeed()); // Is it a boy or a girl?

	Localization::String strMessage;
	Localization::String strSummary;
	if (bMarriage)
	{
		strMessage = iCoinToss == 0 ? Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_MARRIAGE_TT_1") : Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_MARRIAGE_TT_2");
		strMessage << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
		strMessage << GetPlayer()->getCivilizationShortDescriptionKey();

		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_MARRIAGE");
		strSummary << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
		strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
	}
	else
	{
		strMessage = iCoinToss == 0 ? Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT_TT_1") : Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT_TT_2");
		strMessage << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
		strMessage << GetPlayer()->getCivilizationShortDescriptionKey();

		strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BUYOUT");
		strSummary << GET_PLAYER(eMajor).getCivilizationShortDescriptionKey();
		strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
	}

	for (int iMajorLoop = 0; iMajorLoop < MAX_PLAYERS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;

		CvNotifications* pNotifications = GET_PLAYER(eMajorLoop).GetNotifications();
		if (!pNotifications)
			continue;

		if (GET_PLAYER(eMajorLoop).isObserver())
		{
			pNotifications->Add(NOTIFICATION_MINOR_BUYOUT, strMessage.toUTF8(), strSummary.toUTF8(), iCapitalX, iCapitalY, GetPlayer()->GetID());
			continue;
		}

		if (!GET_PLAYER(eMajorLoop).isAlive() || !GET_PLAYER(eMajorLoop).isMajorCiv())
			continue;

		TeamTypes eLoopTeam = GET_PLAYER(eMajorLoop).getTeam();

		if (IsHasMetPlayer(eMajorLoop) && (eLoopTeam == eBuyerTeam || GET_TEAM(eLoopTeam).isHasMet(eBuyerTeam)))
		{
			pNotifications->Add(NOTIFICATION_MINOR_BUYOUT, strMessage.toUTF8(), strSummary.toUTF8(), iCapitalX, iCapitalY, GetPlayer()->GetID());
		}
	}

	if (!bMarriage)
	{
		if (MOD_EVENTS_MINORS_INTERACTION) 
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBoughtOut, eMajor, GetPlayer()->GetID());
		}

		if (MOD_API_ACHIEVEMENTS)
		{
			CvPlayerAI& kMajorPlayer = GET_PLAYER(eMajor);
			kMajorPlayer.GetPlayerAchievements().BoughtCityState(iNumUnits);

			//Nigerian Prince Achievement
			MinorCivTypes eBornu =(MinorCivTypes) GC.getInfoTypeForString("MINOR_CIV_BORNU", /*bHideAssert*/ true);
			MinorCivTypes  eSokoto =(MinorCivTypes) GC.getInfoTypeForString("MINOR_CIV_SOKOTO", /*bHideAssert*/ true);
			bool bUsingXP2Scenario2 = gDLL->IsModActivated(CIV5_XP2_SCENARIO2_MODID);

			if (kMajorPlayer.isHuman() && bUsingXP2Scenario2 && (GetPlayer()->GetMinorCivAI()->GetMinorCivType() == eBornu || GetPlayer()->GetMinorCivAI()->GetMinorCivType() == eSokoto ))
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_54);
		}
	}
}

/// Transfers all of this City-State's units and cities to eMajor. Called by an Austrian buyout (see above) as well as by the Merchant of Venice's buyout mission and the Rome forced annex.
int CvMinorCivAI::TransferUnitsAndCitiesToMajor(PlayerTypes eMajor, bool bForced)
{
	// Transfer all units
	int iLoop = 0;
	int iNumUnits = 0;
	for (CvUnit* pLoopUnit = GetPlayer()->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iLoop))
	{
		GET_PLAYER(eMajor).DoDistanceGift(GetPlayer()->GetID(), pLoopUnit);
		iNumUnits++;
	}

	// Transfer all cities (turn off notifications during the transfer)
	SetDisableNotifications(true);
	vector<CvCity*> vpCitiesToAcquire;
	for (CvCity* pLoopCity = GetPlayer()->firstCity(&iLoop, true); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop, true))
	{
		vpCitiesToAcquire.push_back(pLoopCity);
	}

	for (uint iI = 0; iI < vpCitiesToAcquire.size(); iI++)
	{
		CvCity* pNewCity = GET_PLAYER(eMajor).acquireCity(vpCitiesToAcquire[iI], bForced, true);

		// Reduce the resistance to 0 turns because we bought it fairly
		if (pNewCity)
			pNewCity->ChangeResistanceTurns(-pNewCity->GetResistanceTurns());
	}

	SetDisableNotifications(false);
	GC.getGame().DoUpdateDiploVictory();
	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);

	return iNumUnits;
}

// ******************************
// ***** Bullying *****
// ******************************

const ReachablePlots & CvMinorCivAI::GetBullyRelevantPlots()
{
	if (m_iBullyPlotsBuilt != GC.getGame().getGameTurn())
	{
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if (pCapital)
		{
			SPathFinderUserData data(GetPlayer()->GetID(), PT_ARMY_MIXED, -1, MINOR_POWER_COMPARISON_RADIUS);
			data.iFlags = CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE;
			m_bullyRelevantPlots = GC.GetStepFinder().GetPlotsInReach(GetPlayer()->getCapitalCity()->plot(), data);
			m_iBullyPlotsBuilt = GC.getGame().getGameTurn();

			//make sure we include all adjacent plots even if they are impassable for us ... but they might be passable for the bully (inca!)
			for (int i = RING0_PLOTS; i < RING1_PLOTS; i++)
			{
				CvPlot* pPlot = iterateRingPlots(pCapital->plot(), i);
				if (pPlot && m_bullyRelevantPlots.find(pPlot->GetPlotIndex()) == m_bullyRelevantPlots.end())
					m_bullyRelevantPlots.insertWithIndex( SMovePlot(pPlot->GetPlotIndex(),1,0,500) );
			}
		}
		else
			m_bullyRelevantPlots.clear();
	}

	return m_bullyRelevantPlots;
}

int CvMinorCivAI::GetBullyGoldAmount(PlayerTypes eBullyPlayer, bool bIgnoreScaling, bool bForUnit)
{
	int iGold = /*50*/ GD_INT_GET(MINOR_BULLY_GOLD);
	int iGoldGrowthFactor = 400;

	// Add gold, more if later in game
	float fGameProgressFactor = ((float) GC.getGame().getElapsedGameTurns() / (float) GC.getGame().getEstimateEndTurn());
	CvAssertMsg(fGameProgressFactor >= 0.0f, "fGameProgressFactor is not expected to be negative! Please send Anton your save file and version.");
	if(fGameProgressFactor > 1.0f)
		fGameProgressFactor = 1.0f;

	iGold += (int)(fGameProgressFactor * iGoldGrowthFactor);

	// VP changes to BullyGoldAmount
	if (MOD_BALANCE_VP)
	{
		iGold *= 3;
		int iEra = GET_PLAYER(eBullyPlayer).GetCurrentEra();
		if (iEra <= 0)
		{
			iEra = 1;
		}
		iGold *= iEra;
	}

	// Game Speed Mod
	iGold *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent(); //antonjs: consider: separate XML
	iGold /= 100;

	iGold *= (100 + GET_PLAYER(eBullyPlayer).GetPlayerTraits()->GetBullyValueModifier());
	iGold /= 100;

	if (!bIgnoreScaling)
	{
		int iFactor = CalculateBullyScore(eBullyPlayer, bForUnit);
		iGold *= iFactor;
		iGold /= 100;
	}
	if (iGold <= 0)
		iGold = 0;

	return iGold;
}

// Calculates a basic score for whether the major can bully this minor based on many factors.
// Negative score if bully attempt is a failure, positive if success. In Community Patch only, zero is also a success.
// May be modified after return, if the task is easier or harder (ex. bully a worker vs. bully gold)
int CvMinorCivAI::CalculateBullyScore(PlayerTypes eBullyPlayer, bool bHeavyTribute, CvString* sTooltipSink)
{
	const int iFailScore = -100;
	if (eBullyPlayer < 0 || eBullyPlayer >= MAX_MAJOR_CIVS)
		return iFailScore;

	// Can't bully the dead
	if (!GetPlayer()->isAlive() || !GET_PLAYER(eBullyPlayer).isAlive())
		return iFailScore;

	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if (pMinorCapital == NULL || GET_PLAYER(eBullyPlayer).getCapitalCity() == NULL)
		return iFailScore;

	CvString sFactors = "";

	if (GC.getGame().getGameTurn() == GetTurnLastBulliedByMajor(eBullyPlayer))
	{
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iFailScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY";
			sFactors += strNegativeFactor.toUTF8();
			(*sTooltipSink) += sFactors;
		}
		return iFailScore;
	}

	int iScore = 0;

	// **************************
	// Overall Military Power
	//
	// +0 ~ +75; in VP, typically much lower
	// **************************

	int iGlobalMilitaryScore = 0;

	if (MOD_BALANCE_VP)
	{
		int iTotalMilitaryMight = 0;
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eMajorLoop = (PlayerTypes)iMajorLoop;
			if (GET_PLAYER(eMajorLoop).isAlive() && GET_PLAYER(eMajorLoop).getNumCities() > 0)
				iTotalMilitaryMight += GET_PLAYER(eMajorLoop).GetMilitaryMight();
		}

		int iMilitaryMightPercent = 100 * GET_PLAYER(eBullyPlayer).GetMilitaryMight() / max(1, iTotalMilitaryMight);
		iGlobalMilitaryScore = iMilitaryMightPercent * 50 / 100;
	}
	else
	{
		CvWeightedVector<PlayerTypes> viMilitaryRankings;
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eMajorLoop = (PlayerTypes)iMajorLoop;
			if (GET_PLAYER(eMajorLoop).isAlive() && GET_PLAYER(eMajorLoop).getNumCities() > 0)
				viMilitaryRankings.push_back(eMajorLoop, GET_PLAYER(eMajorLoop).GetMilitaryMight());
		}
		viMilitaryRankings.StableSortItems();
		for (int iRanking = 0; iRanking < viMilitaryRankings.size(); iRanking++)
		{
			if (viMilitaryRankings.GetElement(iRanking) == eBullyPlayer)
			{
				float fRankRatio = (float)(viMilitaryRankings.size() - iRanking) / (float)(viMilitaryRankings.size());
				iGlobalMilitaryScore = (int)(fRankRatio * 75); // A score between 75*(1 / num majors alive) and 75, with the highest rank major getting 75
				break;
			}
		}
	}

	iGlobalMilitaryScore *= 100 + GET_PLAYER(eBullyPlayer).GetPlayerTraits()->GetBullyMilitaryStrengthModifier();
	iGlobalMilitaryScore /= 100;
	iScore += iGlobalMilitaryScore;

	if (sTooltipSink)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iGlobalMilitaryScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_GLOBAL_MILITARY";
		sFactors += strPositiveFactor.toUTF8();
	}

	// **************************
	// Military near City-State
	// **************************

	pair<int, int> localPower = TacticalAIHelpers::EstimateLocalUnitPower(GetBullyRelevantPlots(), GetPlayer()->getTeam(), GET_PLAYER(eBullyPlayer).getTeam(), false);
	//don't forget the city itself
	int iOurPower = max(localPower.first + pMinorCapital->GetPower(), 1);
	int iBullyPower = localPower.second;
	int iLocalPowerRatio = 0;
	int iLocalMilitaryScore = 0;

	if (MOD_BALANCE_VP)
	{
		//linear if the bully is weaker, sqrt if the bully is stronger
		if (iBullyPower > iOurPower)
			iLocalPowerRatio = sqrti(10000 * iBullyPower / iOurPower); //percent
		else
			iLocalPowerRatio = 100 * iBullyPower / iOurPower;

		iLocalMilitaryScore = iLocalPowerRatio;
	}
	else
	{
		iLocalPowerRatio = 100 * iBullyPower / iOurPower;
		if (iLocalPowerRatio >= 300)
			iLocalMilitaryScore = 125;
		else if (iLocalPowerRatio >= 200)
			iLocalMilitaryScore = 100;
		else if (iLocalPowerRatio >= 150)
			iLocalMilitaryScore = 75;
		else if (iLocalPowerRatio >= 100)
			iLocalMilitaryScore = 50;
		else if (iLocalPowerRatio >= 50)
			iLocalMilitaryScore = 25;
	}

	iLocalMilitaryScore *= 100 + GET_PLAYER(eBullyPlayer).GetPlayerTraits()->GetBullyMilitaryStrengthModifier();
	iLocalMilitaryScore /= 100;
	iScore += iLocalMilitaryScore;

	if (sTooltipSink)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iLocalMilitaryScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_MILITARY_PRESENCE";
		sFactors += strPositiveFactor.toUTF8();
	}

	// **************************
	// Social Policies
	//
	// Modifier to positive scores
	// **************************

	int iPoliciesScore = iScore * GET_PLAYER(eBullyPlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MINOR_BULLY_SCORE_MODIFIER) / 100;
	if (sTooltipSink && iPoliciesScore != 0)
	{
		Localization::String strPositiveFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_POSITIVE");
		strPositiveFactor << iPoliciesScore;
		strPositiveFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_GUNBOAT_DIPLOMACY";
		sFactors += strPositiveFactor.toUTF8();
	}
	iScore += iPoliciesScore;

	// **************************
	// CP Only: Base Reluctance
	// **************************

	if (!MOD_BALANCE_VP)
	{
		iScore -= 110;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << -110;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BASE_RELUCTANCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// CP Only: Very low Influence
	// **************************

	if (!MOD_BALANCE_VP && GetEffectiveFriendshipWithMajor(eBullyPlayer) < /*-30*/ GD_INT_GET(FRIENDSHIP_THRESHOLD_CAN_BULLY))
	{
		iScore -= 300;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << -300;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_INFLUENCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// Recently liberated
	// **************************

	if (MOD_BALANCE_VP && GetTurnLiberated() != 0)
	{
		int iDuration = GC.getGame().getGameTurn() - GetTurnLiberated();
		if (iDuration >= 0)
		{
			int iLimit = 50;
			if (iDuration <= iLimit)
			{
				iScore -= 100;
				if (sTooltipSink)
				{
					Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
					strNegativeFactor << -100;
					strNegativeFactor << "TXT_KEY_POP_CSTATE_RECENTLY_LIBERATED";
					sFactors += strNegativeFactor.toUTF8();
				}
			}
		}
	}


	// **************************
	// CP only: Heavy Tribute
	// **************************

	if (!MOD_BALANCE_VP && bHeavyTribute)
	{
		int iHeavyTributeScore = -30;
		iScore += iHeavyTributeScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iHeavyTributeScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_UNIT_RELUCTANCE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// CP only: Low population (Heavy Tribute only)
	// **************************

	if (!MOD_BALANCE_VP && bHeavyTribute && pMinorCapital->getPopulation() < 5)
	{
		int iLowPopScore = -300;
		iScore += iLowPopScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iLowPopScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_POPULATION";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// Gave you tribute recently
	// **************************

	int iLastBullyTurn = GetTurnLastBulliedByMajor(eBullyPlayer);

	if (iLastBullyTurn >= 0)
	{
		int iBulliedVeryRecentlyScore = 0;
		int iBulliedRecentlyScore = 0;
		if (MOD_BALANCE_VP)
		{
			int iCooldown = 30;
			iCooldown *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iCooldown /= 100;

			if (iLastBullyTurn + iCooldown >= GC.getGame().getGameTurn())
				iBulliedRecentlyScore = (iLastBullyTurn + iCooldown - GC.getGame().getGameTurn()) * -10;
		}
		else
		{
			int iCooldown = 10;
			iCooldown *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iCooldown /= 100;

			if (iLastBullyTurn + iCooldown >= GC.getGame().getGameTurn())
			{
				iBulliedVeryRecentlyScore = -300;
			}
			else
			{
				iCooldown *= 2;
				if (iLastBullyTurn + iCooldown >= GC.getGame().getGameTurn())
					iBulliedRecentlyScore = -40;
			}
		}

		if (iBulliedVeryRecentlyScore != 0)
		{
			iScore += iBulliedVeryRecentlyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iBulliedVeryRecentlyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
		else if (iBulliedRecentlyScore != 0)
		{
			iScore += iBulliedRecentlyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iBulliedRecentlyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
	}

	// **************************
	// Passive Support from other majors
	// **************************

	if (!GET_PLAYER(eBullyPlayer).GetPlayerTraits()->IgnoreBullyPenalties())
	{
		// **************************
		// Has Ally
		// **************************

		if (GetAlly() != NO_PLAYER && GET_PLAYER(GetAlly()).getTeam() != GET_PLAYER(eBullyPlayer).getTeam())
		{
			int iAllyScore = MOD_BALANCE_VP ? -15 : -10;
			iScore += iAllyScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iAllyScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_ALLIES";
				sFactors += strNegativeFactor.toUTF8();
			}
		}

		// **************************
		// Has Pledge(s) of Protection
		// **************************

		int iProtectionScore = 0;
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eMajorLoop = (PlayerTypes)iMajorLoop;
			if (GET_PLAYER(eMajorLoop).getTeam() == GET_PLAYER(eBullyPlayer).getTeam())
				continue;

			if (IsProtectedByMajor(eMajorLoop))
			{
				iProtectionScore -= MOD_BALANCE_VP ? 15 : 20;
			}
		}

		if (iProtectionScore != 0)
		{
			if (!MOD_BALANCE_VP && iProtectionScore < -20)
				iProtectionScore = -20;

			iScore += iProtectionScore;
			if (sTooltipSink)
			{
				Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
				strNegativeFactor << iProtectionScore;
				strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_PLEDGES_TO_PROTECT";
				sFactors += strNegativeFactor.toUTF8();
			}
		}
	}

	// **************************
	// Hostile Personality
	// **************************

	if (GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
	{
		int iHostileScore = -10;
		iScore += iHostileScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iHostileScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_HOSTILE";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	// **************************
	// Militaristic
	// **************************

	if (GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		int iMilitaristicScore = -10;
		iScore += iMilitaristicScore;
		if (sTooltipSink)
		{
			Localization::String strNegativeFactor = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_FACTOR_NEGATIVE");
			strNegativeFactor << iMilitaristicScore;
			strNegativeFactor << "TXT_KEY_POP_CSTATE_BULLY_FACTOR_MILITARISTIC";
			sFactors += strNegativeFactor.toUTF8();
		}
	}

	if (sTooltipSink != NULL)
	{
		(*sTooltipSink) += sFactors;
	}
	
	if (iScore > 100)
		iScore = 100;

	return iScore;
}

bool CvMinorCivAI::CanMajorBullyGold(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;

	int iScore = CalculateBullyScore(ePlayer, /*bForUnit*/ false);

	return CanMajorBullyGold(ePlayer, iScore);
}

// In case client wants to specify a metric beforehand (i.e. they calculated it on their end, for logging purposes etc.)
bool CvMinorCivAI::CanMajorBullyGold(PlayerTypes ePlayer, int iSpecifiedBullyMetric)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

#if defined(MOD_BALANCE_CORE)
	if(IsAtWarWithPlayersTeam(ePlayer))
		return false;
#endif

#if defined(MOD_EVENTS_MINORS_INTERACTION)
	if (MOD_EVENTS_MINORS_INTERACTION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanBullyGold, ePlayer, GetPlayer()->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif				
	if (MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
		return (iSpecifiedBullyMetric > 0);
	else
		return (iSpecifiedBullyMetric >= 0);
}

CvString CvMinorCivAI::GetMajorBullyGoldDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";
	
	CvString sFactors = "";
	int iScore = CalculateBullyScore(ePlayer, /*bForUnit*/ false, &sFactors);
	bool bCanBully = CanMajorBullyGold(ePlayer, iScore);

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
	}
	if (MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
	{
		if (iScore < 0)
			iScore *= -1;
		sFear << iScore;
	}
	else
		sFear << iScore;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_GOLD_TT");
	sResult << sFear.toUTF8() << sFactors;

	return sResult.toUTF8();
}

bool CvMinorCivAI::CanMajorBullyUnit(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	if(MOD_BALANCE_CORE && IsAtWarWithPlayersTeam(ePlayer))
		return false;

	int iScore = CalculateBullyScore(ePlayer, /*bForUnit*/ true);
	return CanMajorBullyUnit(ePlayer, iScore);
}

// In case client wants to specify a metric beforehand (i.e. they calculated it on their end, for logging purposes etc.)
bool CvMinorCivAI::CanMajorBullyUnit(PlayerTypes ePlayer, int iSpecifiedBullyMetric)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Can't bully us if we're dead!
	if(!GetPlayer()->isAlive())
		return false;

	if(MOD_BALANCE_CORE && IsAtWarWithPlayersTeam(ePlayer))
		return false;

	if (MOD_EVENTS_MINORS_INTERACTION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanBullyUnit, ePlayer, GetPlayer()->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}

	return iSpecifiedBullyMetric >= /*0 in CP, 100 in VP*/ GD_INT_GET(MINOR_CIV_HEAVY_TRIBUTE_THRESHOLD);
}

CvString CvMinorCivAI::GetMajorBullyUnitDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";

	CvString sFactors = "";
	int iScore = CalculateBullyScore(ePlayer, /*bForUnit*/ true, &sFactors);
	bool bCanBully = CanMajorBullyUnit(ePlayer, iScore);
#if defined(MOD_BALANCE_CORE)
	UnitClassTypes eUnitClassType = GetBullyUnit();
	if(eUnitClassType == NO_UNITCLASS)
	{
		return "";
	}
	UnitTypes eUnitType = NO_UNIT;
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClassType);
	if(pkUnitClassInfo != NULL)
	{
		eUnitType = (GET_PLAYER(ePlayer).GetSpecificUnitType(eUnitClassType));
	}
	if(eUnitType == NO_UNIT)
	{
		return "";
	}
#else
	UnitTypes eUnitType = (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER"); //antonjs: todo: XML/function
#endif
	CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnitType);
	CvAssert(pUnitInfo);
	if (!pUnitInfo)
		return "";

	int iBullyThreshold = /*0 in CP, 100 in VP*/ GD_INT_GET(MINOR_CIV_HEAVY_TRIBUTE_THRESHOLD);

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = (iBullyThreshold > 0) ? Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT_VARIABLE_THRESHOLD") : Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
		iScore = iScore - iBullyThreshold;
	}

	if (MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
	{
		if (iScore < 0)
			iScore *= -1;
	}
	sFear << iScore;
	if (iBullyThreshold > 0)
		sFear << iBullyThreshold;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_UNIT_TT");
	if (MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
	{
		sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_VARIABLE_CBP");
	}
	sResult << sFear.toUTF8() << sFactors << pUnitInfo->GetDescriptionKey();

	return sResult.toUTF8();
}

CvString CvMinorCivAI::GetMajorBullyAnnexDetails(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return "";

	CvString sFactors = "";
	int iScore = CalculateBullyScore(ePlayer, /*bForUnit*/ true, &sFactors);
	bool bCanBully = CanMajorBullyUnit(ePlayer, iScore);
	UnitClassTypes eUnitClassType = GetBullyUnit();
	if (eUnitClassType == NO_UNITCLASS)
	{
		return "";
	}
	UnitTypes eUnitType = NO_UNIT;
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClassType);
	if (pkUnitClassInfo != NULL)
	{
		eUnitType = (GET_PLAYER(ePlayer).GetSpecificUnitType(eUnitClassType));
	}
	if (eUnitType == NO_UNIT)
	{
		return "";
	}
	CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnitType);
	CvAssert(pUnitInfo);
	if (!pUnitInfo)
		return "";

	Localization::String sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_AFRAID");
	if (!bCanBully)
	{
		sFear = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_RESILIENT");
	}
	if (MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
	{
		if (iScore < 0)
			iScore *= -1;
		sFear << iScore;
	}
	else
		sFear << iScore;

	Localization::String sResult = Localization::Lookup("TXT_KEY_POP_CSTATE_BULLY_UNIT_TT_ANNEX");
	sResult << sFear.toUTF8() << sFactors << pUnitInfo->GetDescriptionKey();

	return sResult.toUTF8();
}

void CvMinorCivAI::DoMajorBullyGold(PlayerTypes eBully, int iGold)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
	if (pMinorCapital == NULL)
	{
		CvAssertMsg(false, "Trying to take tribute but the minor has no capital. Please send Anton your save file and version.");
		return;
	}
	CvCity* pBullyCapital = GET_PLAYER(eBully).getCapitalCity();
	if (pBullyCapital == NULL)
	{
		CvAssertMsg(false, "Trying to take tribute without having a capital. Please send Anton your save file and version.");
		return;
	}

	int iBullyMetric = CalculateBullyScore(eBully, /*bForUnit*/ false);
	bool bSuccess = CanMajorBullyGold(eBully, iBullyMetric);
	int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(eBully);
	if (bSuccess)
	{
		CvAssertMsg(iGold >= 0, "iGold is expected to be non-negative. Please send Anton your save file and version.");

		if (GC.getGame().getActivePlayer() == eBully)
		{
			int iBullyGold = 0;
			gDLL->GetSteamStat(ESTEAMSTAT_BULLIEDGOLD, &iBullyGold);

			iBullyGold += iGold;

			gDLL->SetSteamStat(ESTEAMSTAT_BULLIEDGOLD, iBullyGold);
		}

		if (GET_PLAYER(eBully).GetPlayerTraits()->GetBullyYieldMultiplierAnnex() != 0)
		{
			iGold *= (100 + GET_PLAYER(eBully).GetPlayerTraits()->GetBullyYieldMultiplierAnnex());
			iGold /= 100;
		}

		GET_PLAYER(eBully).doInstantYield(INSTANT_YIELD_TYPE_BULLY, true, NO_GREATPERSON, NO_BUILDING, iGold, false, NO_PLAYER, NULL, false, pBullyCapital, false, true, false, YIELD_GOLD, NULL, NO_TERRAIN, NULL, pMinorCapital);

		if (GC.getGame().getActivePlayer() != NO_PLAYER)
		{
			if (GET_TEAM(GET_PLAYER(GC.getGame().getActivePlayer()).getTeam()).isHasMet(GetPlayer()->getTeam()))
			{
				CvString bullyText = GetLocalizedText("TXT_KEY_MISC_BULLIED_POPUP");
				char text[256] = { 0 };
				sprintf_s(text, "[COLOR_RED]%s -%d[ENDCOLOR][ICON_GOLD]", bullyText.c_str(), iGold);
				SHOW_PLOT_POPUP(pMinorCapital->plot(), GC.getGame().getActivePlayer(), text);
			}
		}
		
		DoBulliedByMajorReaction(eBully, /*-1500 in CP, -3000 in VP*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS));

		if (MOD_EVENTS_MINORS_INTERACTION) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBullied, eBully, GetPlayer()->GetID(), iGold, -1, -1, -1, YIELD_GOLD);
		}

		// additional instant yields from policies etc.
		GET_PLAYER(eBully).doInstantYield(INSTANT_YIELD_TYPE_BULLY, true, NO_GREATPERSON, NO_BUILDING, iGold, false, NO_PLAYER, NULL, false, pBullyCapital, false, true, false, NO_YIELD, NULL, NO_TERRAIN, NULL, pMinorCapital);

		if (GET_PLAYER(eBully).GetBullyGlobalCSReduction() > 0)
		{
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;

				if (ePlayer == eBully)
				{
					int iInfluence = GetEffectiveFriendshipWithMajor(eBully);
					iInfluence *= GET_PLAYER(eBully).GetBullyGlobalCSReduction();
					iInfluence /= 100;

					if (iInfluence <= 0)
						continue;

					ChangeFriendshipWithMajor(eBully, iInfluence);

					if (GET_PLAYER(eBully).isHuman())
					{
						const char* strMinorsNameKey = GetPlayer()->getNameKey();
						//const char* strBullyName = GET_PLAYER(eBully).getNameKey();
						Localization::String strMessageOthers;
						Localization::String strSummaryOthers;

						// Notify player has met the bully
						strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS_YOU");
						strMessageOthers << strMinorsNameKey;
						strMessageOthers << iInfluence;
						strMessageOthers << GET_PLAYER(eBully).GetBullyGlobalCSReduction();
						strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS_YOU_S");
						strSummaryOthers << strMinorsNameKey;
						AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eBully);
					}
				}

				else if (GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eBully).getTeam())
					continue;

				else if (GET_PLAYER(ePlayer).isAlive())
				{
					int iInfluence = GetEffectiveFriendshipWithMajor(ePlayer);
					iInfluence *= GET_PLAYER(eBully).GetBullyGlobalCSReduction();
					iInfluence /= 100;

					if (iInfluence <= 0)
						continue;

					ChangeFriendshipWithMajor(ePlayer, -iInfluence);

					GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(eBully, 1);

					if (GET_PLAYER(ePlayer).isHuman())
					{
						const char* strMinorsNameKey = GetPlayer()->getNameKey();
						const char* strBullyName = GET_PLAYER(eBully).getNameKey();
						Localization::String strMessageOthers;
						Localization::String strSummaryOthers;

						// Notify player has met the bully
						strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS");
						strMessageOthers << strMinorsNameKey;
						strMessageOthers << strBullyName;
						strMessageOthers << iInfluence;
						strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS_S");
						strSummaryOthers << strMinorsNameKey;
						AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), ePlayer);
					}
				}
			}
		}
	}

	// Logging
	GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyGold(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), iGold, bSuccess, iBullyMetric);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

void CvMinorCivAI::DoMajorBullyAnnex(PlayerTypes eBully)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if (eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	int iBullyMetric = CalculateBullyScore(eBully, /*bForUnit*/ false);
	bool bSuccess = CanMajorBullyUnit(eBully, iBullyMetric);
	if (bSuccess)
	{
		if (GetPlayer()->getCapitalCity() != NULL)
		{
			PlayerTypes ePlayer;
			const char* strMinorsNameKey = GetPlayer()->getNameKey();
			const char* strBullyName = NULL;
			Localization::String strMessageOthers;
			Localization::String strSummaryOthers;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				ePlayer = (PlayerTypes)iPlayerLoop;
				if (GET_PLAYER(ePlayer).GetID() == eBully)
				{
					// Notify player has met the bully
					strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_ANNEXED_CS");
					strMessageOthers << strMinorsNameKey;
					strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_ANNEXED_CS_SUMMARY");
					strSummaryOthers << strMinorsNameKey;
					AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), ePlayer);
				}
				else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetPlayer()->getTeam()))
				{
					if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eBully).getTeam()))
					{
						// Notify player has met the bully
						strBullyName = GET_PLAYER(eBully).getCivilizationShortDescriptionKey();
						strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN");
						strMessageOthers << strBullyName << strMinorsNameKey;
						strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY");
						strSummaryOthers << strMinorsNameKey;
						AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), ePlayer);
					}
					else
					{
						// Notify player has not met the bully
						strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN");
						strMessageOthers << strMinorsNameKey;
						strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN_SUMMARY");
						strSummaryOthers << strMinorsNameKey;
						AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), ePlayer);
					}
				}
				else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eBully).getTeam()))
				{
					// Notify player has met the bully
					strBullyName = GET_PLAYER(eBully).getCivilizationShortDescriptionKey();
					strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN");
					strMessageOthers << strBullyName;
					strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY");
					strSummaryOthers << strBullyName;
					AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), ePlayer);
				}
			}

			if (GC.getLogging() && GC.getAILogging())
			{
				// Logging
				CvString strLogString;
				strLogString.Format("%s annexed by %s through bullying. X: %d, Y: %d", GetPlayer()->getName(), GET_PLAYER(eBully).getName(), GetPlayer()->getCapitalCity()->getX(), GetPlayer()->getCapitalCity()->getY());
				GET_PLAYER(eBully).GetHomelandAI()->LogHomelandMessage(strLogString);
			}

			GC.getGame().SetLastTurnCSAnnexed(GC.getGame().getGameTurn());
			TransferUnitsAndCitiesToMajor(eBully, true);
		}
	}
}
void CvMinorCivAI::DoMajorBullyUnit(PlayerTypes eBully, UnitTypes eUnitType)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if(eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	int iBullyMetric = CalculateBullyScore(eBully, /*bForUnit*/ true);
	bool bSuccess = CanMajorBullyUnit(eBully, iBullyMetric);
	int iOldFriendshipTimes100 = GetEffectiveFriendshipWithMajorTimes100(eBully);

	if(bSuccess)
	{
#if defined(MOD_BALANCE_CORE)
		int iEra = GET_PLAYER(eBully).GetCurrentEra();
		if(iEra <= 0)
		{
			iEra = 1;
		}
		if (!MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
		{
			CvCity* pCapital = GET_PLAYER(eBully).getCapitalCity();
			if (pCapital != NULL)
			{
				GET_PLAYER(eBully).doInstantYield(INSTANT_YIELD_TYPE_BULLY, true, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pCapital);
			}
		}
#endif
#if defined(MOD_BALANCE_CORE_AFRAID_ANNEX)
		if (GET_PLAYER(eBully).GetBullyGlobalCSReduction() > 0)
		{
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;

				if (ePlayer == eBully)
				{
					int iInfluence = GetEffectiveFriendshipWithMajor(eBully);
					iInfluence *= GET_PLAYER(eBully).GetBullyGlobalCSReduction();
					iInfluence /= 100;

					if (iInfluence <= 0)
						continue;

					ChangeFriendshipWithMajor(eBully, iInfluence);

					if (GET_PLAYER(eBully).isHuman())
					{
						const char* strMinorsNameKey = GetPlayer()->getNameKey();
						Localization::String strMessageOthers;
						Localization::String strSummaryOthers;

						// Notify player has met the bully
						strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS_YOU");
						strMessageOthers << strMinorsNameKey;
						strMessageOthers << iInfluence;
						strMessageOthers << GET_PLAYER(eBully).GetBullyGlobalCSReduction();
						strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS_YOU_S");
						strSummaryOthers << strMinorsNameKey;
						AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), eBully);
					}
				}

				else if (GET_PLAYER(ePlayer).getTeam() == GET_PLAYER(eBully).getTeam())
					continue;

				else if (GET_PLAYER(ePlayer).isAlive())
				{
					int iInfluence = GetEffectiveFriendshipWithMajor(ePlayer);
					iInfluence *= GET_PLAYER(eBully).GetBullyGlobalCSReduction();
					iInfluence /= 100;

					if (iInfluence <= 0)
						continue;

					ChangeFriendshipWithMajor(ePlayer, -iInfluence);

					GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(eBully, 1);

					if (GET_PLAYER(ePlayer).isHuman())
					{
						const char* strMinorsNameKey = GetPlayer()->getNameKey();
						const char* strBullyName = GET_PLAYER(eBully).getNameKey();
						Localization::String strMessageOthers;
						Localization::String strSummaryOthers;

						// Notify player has met the bully
						strMessageOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS");
						strMessageOthers << strMinorsNameKey;
						strMessageOthers << strBullyName;
						strMessageOthers << iInfluence;
						strSummaryOthers = Localization::Lookup("TXT_KEY_BALANCE_AUTOCRACY_BULLY_INFLUENCE_REDUCTION_CS_S");
						strSummaryOthers << strMinorsNameKey;
						strSummaryOthers << strBullyName;
						AddNotification(strMessageOthers.toUTF8(), strSummaryOthers.toUTF8(), ePlayer);
					}
				}
			}
		}

#endif
#if defined(MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
		// Minor must have Capital
		if(MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
		{
			CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
			if(pMinorCapital == NULL)
			{
				CvAssertMsg(false, "Trying to spawn a Unit for a major civ but the minor has no capital. Please send Anton your save file and version.");
				return;
			}
			CvCity* pBullyCapital = GET_PLAYER(eBully).getCapitalCity();
			int iGold = 0;
			float fDelay = 0.0f;
			CvString bullyText;
			if (pBullyCapital != NULL)
			{
				// Heavy tribute gives gold, equal to normal tribute, for all types of city-states.
				iGold = GetBullyGoldAmount(eBully, false, /*bForUnit*/ true);
				if (iGold > 0)
				{
					if (GET_PLAYER(eBully).GetPlayerTraits()->GetBullyYieldMultiplierAnnex() != 0)
					{
						iGold *= (100 + GET_PLAYER(eBully).GetPlayerTraits()->GetBullyYieldMultiplierAnnex());
						iGold /= 100;
					}

					GET_PLAYER(eBully).doInstantYield(INSTANT_YIELD_TYPE_BULLY, true, NO_GREATPERSON, NO_BUILDING, iGold, false, NO_PLAYER, NULL, false, pBullyCapital, false, true, false, YIELD_GOLD, NULL, NO_TERRAIN, NULL, pMinorCapital);

					// additional yields from policies etc.
					GET_PLAYER(eBully).doInstantYield(INSTANT_YIELD_TYPE_BULLY, true, NO_GREATPERSON, NO_BUILDING, iGold, false, NO_PLAYER, NULL, false, pBullyCapital, false, true, false, NO_YIELD, NULL, NO_TERRAIN, NULL, pMinorCapital);


					if (GC.getGame().getActivePlayer() != NO_PLAYER)
					{
						if (GET_TEAM(GET_PLAYER(GC.getGame().getActivePlayer()).getTeam()).isHasMet(GetPlayer()->getTeam()))
						{
							char text[256] = { 0 };
							fDelay += 1.5f;
							bullyText = GetLocalizedText("TXT_KEY_MISC_BULLIED_POPUP");
							sprintf_s(text, "[COLOR_RED]%s -%d[ENDCOLOR][ICON_GOLD]", bullyText.c_str(), iGold);
							SHOW_PLOT_POPUP(pMinorCapital->plot(), GC.getGame().getActivePlayer(), text);
						}
					}
				}
				GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyHeavy(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), YIELD_GOLD, iGold, bSuccess, iBullyMetric);
				if (GC.getLogging() && GC.getAILogging())
				{
					// Logging
					CvString strLogString;
					strLogString.Format("%s robbed of gold by %s through bullying. X: %d, Y: %d", GetPlayer()->getName(), GET_PLAYER(eBully).getName(), GetPlayer()->getCapitalCity()->getX(), GetPlayer()->getCapitalCity()->getY());
					GET_PLAYER(eBully).GetHomelandAI()->LogHomelandMessage(strLogString);
				}

				// half of the rewards of all quests from the city-state are given to the player, and the quests are canceled
				QuestListForPlayer::iterator itr_quest;
				for (itr_quest = m_QuestsGiven[eBully].begin(); itr_quest != m_QuestsGiven[eBully].end(); itr_quest++)
				{
					if (itr_quest->IsObsolete(true)) // is this quest canceled by demanding heavy tribute?
					{
						itr_quest->DoRewards(eBully, true);
					}
				}
				DoObsoleteQuestsForPlayer(eBully, NO_MINOR_CIV_QUEST_TYPE, false, true);

				DoBulliedByMajorReaction(eBully, /*-6000*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS));

				if (MOD_EVENTS_MINORS_INTERACTION) {
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBullied, eBully, GetPlayer()->GetID(), iGold, -1, -1, -1, YIELD_SCIENCE);
				}

			}
			else
			{
				if(eUnitType == NO_UNIT)
				{
					CvAssertMsg(false, "eUnitType is not expected to be NO_UNIT. Please send Anton your save file and version.");
					return;
				}
				// Minor must have Capital
				CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
				if(pMinorCapital == NULL)
				{
					CvAssertMsg(false, "Trying to spawn a Unit for a major civ but the minor has no capital. Please send Anton your save file and version.");
					return;
				}
				int iX = pMinorCapital->getX();
				int iY = pMinorCapital->getY();

				CvUnit* pNewUnit = GET_PLAYER(eBully).initUnit(eUnitType, iX, iY);
				if (pNewUnit->jumpToNearestValidPlot())
				{
					pNewUnit->finishMoves(); // The given unit cannot move this turn

					if(GetPlayer()->getCapitalCity())
						GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);

					DoBulliedByMajorReaction(eBully, /*-5000 in CP, 6000 in VP*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS));
#if defined(MOD_EVENTS_MINORS_INTERACTION)
					if (MOD_EVENTS_MINORS_INTERACTION) {
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBullied, eBully, GetPlayer()->GetID(), -1, eUnitType, pNewUnit->getX(), pNewUnit->getY(), -1);
					}
#endif
				}
				else
					pNewUnit->kill(false);	// Could not find a spot for the unit!
			}
		}
		else
		{
#endif
		if(eUnitType == NO_UNIT)
		{
			CvAssertMsg(false, "eUnitType is not expected to be NO_UNIT. Please send Anton your save file and version.");
			return;
		}
		// Minor must have Capital
		CvCity* pMinorCapital = GetPlayer()->getCapitalCity();
		if(pMinorCapital == NULL)
		{
			CvAssertMsg(false, "Trying to spawn a Unit for a major civ but the minor has no capital. Please send Anton your save file and version.");
			return;
		}
		int iX = pMinorCapital->getX();
		int iY = pMinorCapital->getY();

		CvUnit* pNewUnit = GET_PLAYER(eBully).initUnit(eUnitType, iX, iY);
		if (pNewUnit->jumpToNearestValidPlot())
		{
			pNewUnit->finishMoves(); // The given unit cannot move this turn

			if(GetPlayer()->getCapitalCity())
				GetPlayer()->getCapitalCity()->addProductionExperience(pNewUnit);

			DoBulliedByMajorReaction(eBully, /*-5000 in CP, 6000 in VP*/ GD_INT_GET(MINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS));
			
#if defined(MOD_EVENTS_MINORS_INTERACTION)
			if (MOD_EVENTS_MINORS_INTERACTION) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBullied, eBully, GetPlayer()->GetID(), -1, eUnitType, pNewUnit->getX(), pNewUnit->getY(), -1);
			}
#endif
		}
		else
			pNewUnit->kill(false);	// Could not find a spot for the unit!
#if defined(MOD_BALANCE_CORE)
		}
#endif
	}

	// Logging
	if (!MOD_BALANCE_CORE_MINOR_VARIABLE_BULLYING)
		GET_PLAYER(eBully).GetDiplomacyAI()->LogMinorCivBullyUnit(GetPlayer()->GetID(), iOldFriendshipTimes100, GetEffectiveFriendshipWithMajorTimes100(eBully), eUnitType, bSuccess, iBullyMetric);

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

// We were just bullied, how do we react?
void CvMinorCivAI::DoBulliedByMajorReaction(PlayerTypes eBully, int iInfluenceChangeTimes100)
{
	CvAssertMsg(eBully >= 0, "eBully is expected to be non-negative (invalid Index)");
	CvAssertMsg(eBully < MAX_MAJOR_CIVS, "eBully is expected to be within maximum bounds (invalid Index)");
	if (eBully < 0 || eBully >= MAX_MAJOR_CIVS) return;

	CvPlayer* pBully = &GET_PLAYER(eBully);
	CvAssertMsg(pBully, "pBully not expected to be NULL. Please send Anton your save file and version.");
	if (!pBully) return;

	if (GET_PLAYER(eBully).GetBullyGlobalCSReduction() == 0)
	{
		ChangeFriendshipWithMajorTimes100(eBully, iInfluenceChangeTimes100);
	}

	// In case we have quests that bullying makes obsolete, check now
	if (GET_PLAYER(eBully).GetBullyGlobalCSReduction() == 0)
	{
		DoTestActiveQuests(/*bTestComplete*/ false, /*bTestObsolete*/ true);
	}
#if defined(MOD_BALANCE_CORE_MINORS)
	if (GET_PLAYER(eBully).GetBullyGlobalCSReduction() == 0)
	{
		DoChangeProtectionFromMajor(eBully, false, true);
	}
#endif

	// Inform alive majors who have met the bully
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajorLoop = (PlayerTypes) iMajorLoop;
		CvPlayer* pMajorLoop = &GET_PLAYER(eMajorLoop);
		if (!pMajorLoop) continue;

		if (pMajorLoop->isAlive() && eMajorLoop != eBully && GET_TEAM(pMajorLoop->getTeam()).isHasMet(GetPlayer()->getTeam()) && GET_TEAM(pMajorLoop->getTeam()).isHasMet(pBully->getTeam()))
		{
			pMajorLoop->GetDiplomacyAI()->DoPlayerBulliedSomeone(eBully, GetPlayer()->GetID());

			CvNotifications* pNotifications = pMajorLoop->GetNotifications();
			if (pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BULLIED_BY_SOMEONE_S");
				strSummary << GetPlayer()->getNameKey();
				strSummary << pBully->getNameKey();
				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_BULLIED_BY_SOMEONE");
				strNotification << GetPlayer()->getNameKey();
				strNotification << pBully->getNameKey();
				pNotifications->Add(NOTIFICATION_MINOR, strNotification.toUTF8(), strSummary.toUTF8(), GetPlayer()->getCapitalCity()->getX(), GetPlayer()->getCapitalCity()->getY(), -1);
			}
		}
	}

	// Inform other alive minors, in case they had a quest that this fulfills
	for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinorLoop = (PlayerTypes) iMinorLoop;
		if (eMinorLoop == GetPlayer()->GetID()) continue;

		CvPlayer* pMinorLoop = &GET_PLAYER(eMinorLoop);
		if (!pMinorLoop) continue;

		if (pMinorLoop->isAlive() && pMinorLoop->GetMinorCivAI()->IsHasMetPlayer(eBully))
		{
			pMinorLoop->GetMinorCivAI()->DoTestActiveQuestsForPlayer(eBully, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_BULLY_CITY_STATE);
		}
	}

	SetTurnLastBulliedByMajor(eBully, GC.getGame().getGameTurn());
}

bool CvMinorCivAI::IsEverBulliedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsEverBulliedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

bool CvMinorCivAI::IsEverBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return (m_aiTurnLastBullied[ePlayer] >= 0); // -1 means never bullied
}

bool CvMinorCivAI::IsRecentlyBulliedByAnyMajor() const
{
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		if(IsRecentlyBulliedByMajor((PlayerTypes)iPlayerLoop))
			return true;
	return false;
}

// Checks whether this minor has been bullied by a major recently
/* "Recently" is defined to be an interval of turns during which the
* major and the minor consider the bully action to be a dominant part
* of their diplomatic relationship.  For example, a major will likely not
* give a gift of gold to a recently bullied minor.  A minor will likely not
* ask for assistance from a major that recently bullied them.
*
* This is not intended to influence the success of a bully attempt, but rather
* other situations that arise from a bully attempt.
*/
bool CvMinorCivAI::IsRecentlyBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	const int iRecentlyBulliedTurnInterval = 20; //antonjs: todo: constant/XML
	return (m_aiTurnLastBullied[ePlayer] >= 0 && m_aiTurnLastBullied[ePlayer] >= (GC.getGame().getGameTurn() - iRecentlyBulliedTurnInterval)); // -1 means never bullied
}

int CvMinorCivAI::GetTurnLastBulliedByMajor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_aiTurnLastBullied[ePlayer];
}

void CvMinorCivAI::SetTurnLastBulliedByMajor(PlayerTypes ePlayer, int iTurn)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiTurnLastBullied[ePlayer] = iTurn;
}

// ****************
// *** Election ***
// ****************
void CvMinorCivAI::DoElection()
{
	// if it is not an election turn, don't process!
	if(GC.getGame().GetTurnsUntilMinorCivElection() != 0)
	{
		return;
	}

	CvWeightedVector<PlayerTypes> wvVotes;
	Firaxis::Array<CvEspionageSpy*, MAX_MAJOR_CIVS> apSpy;
	CvCity* pCapital = GetPlayer()->getCapitalCity();
	if(!pCapital)
	{
		return;
	}

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eEspionagePlayer = (PlayerTypes)ui;
		CvPlayerEspionage* pPlayerEspionage = GET_PLAYER(eEspionagePlayer).GetEspionage();
		int iVotes = 0;
		int iLoop = 0;
		apSpy[ui] = NULL;

		if (!GET_PLAYER(eEspionagePlayer).isAlive())
		{
			continue;
		}

		for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
		{
			CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
			int iSpyID = pCityEspionage->m_aiSpyAssignment[eEspionagePlayer];
			// if no spies are assigned here, continue
			if(iSpyID == -1)
			{
				continue;
			}

			// on election day, evaluate spy to be reassigned
			pPlayerEspionage->m_aSpyList[iSpyID].m_bEvaluateReassignment = true;

			// if the spy assigned here is not rigging the election yet, continue
			if(pPlayerEspionage->m_aSpyList[iSpyID].m_eSpyState != SPY_STATE_RIG_ELECTION)
			{
				continue;
			}

			apSpy[ui] = &(pPlayerEspionage->m_aSpyList[iSpyID]);

			iVotes += (pCityEspionage->m_aiAmount[eEspionagePlayer] * (100 + (-1 * GET_PLAYER(eEspionagePlayer).GetPlayerTraits()->GetEspionageModifier()) + GET_PLAYER(eEspionagePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER))) / 100;

			// now that votes are counted, remove the progress from the spy
			pCityEspionage->ResetProgress(eEspionagePlayer);

			int iRate = pPlayerEspionage->CalcPerTurn(SPY_STATE_RIG_ELECTION, pCity, iSpyID);
			int iGoal = pPlayerEspionage->CalcRequired(SPY_STATE_RIG_ELECTION, pCity, iSpyID);
			pCityEspionage->SetActivity(eEspionagePlayer, 0, iRate, iGoal);
			pCityEspionage->SetLastProgress(eEspionagePlayer, iRate);
		}

		if(iVotes > 0)
		{
			wvVotes.push_back(eEspionagePlayer, iVotes);
		}
	}

	if(wvVotes.size() > 0)
	{
		wvVotes.StableSortItems();
		RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		PlayerTypes eElectionWinner = wvVotes.ChooseByWeight(&fcn, "Choosing CS election winner by weight");

		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes ePlayer = (PlayerTypes)ui;

			if(ePlayer == eElectionWinner)
			{
				CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
				if(pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS_S");
					strSummary << pCapital->getNameKey();
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS");
					strNotification << GET_PLAYER(ePlayer).GetEspionage()->GetSpyRankName(apSpy[ui]->m_eRank);
					strNotification << apSpy[ui]->GetSpyName(&GET_PLAYER(ePlayer));
					strNotification << pCapital->getNameKey();
					pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_SUCCESS, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
				}

				int iValue = /*20 in CP, 15 in VP*/ GD_INT_GET(ESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION);

				int iEra = GET_PLAYER(ePlayer).GetCurrentEra();
				if (iEra <= 0)
				{
					iEra = 1;
				}
				if (MOD_BALANCE_CORE_SPIES_ADVANCED)
				{
					iValue *= (iEra + iEra);
				}
				ChangeFriendshipWithMajor(ePlayer, iValue, false);

				// find the spy who has rigged the election
				int iLoop = 0;
				int iSpyID = -1;
				for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
				{
					CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
					iSpyID = pCityEspionage->m_aiSpyAssignment[ePlayer];
					if (iSpyID != -1)
					{
						break;
					}
				}
				CvAssertMsg(iSpyID==-1, "Couldn't find a spy in any of the cities of the Minor Civ")

				if (GET_PLAYER(ePlayer).GetEspionage()->CanStageCoup(iSpyID, true))
				{
					ChangeRiggingCoupChanceIncrease(ePlayer, GD_INT_GET(ESPIONAGE_COUP_CHANCE_INCREASE_FOR_RIGGED_ELECTION_BASE) + apSpy[ui]->GetSpyRank(ePlayer) * GD_INT_GET(ESPIONAGE_COUP_CHANCE_INCREASE_FOR_RIGGED_ELECTION_PER_SPY_LEVEL));
				}

				//Achievements!
				if (MOD_API_ACHIEVEMENTS && ePlayer == GC.getGame().getActivePlayer())
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_14);

				if (MOD_EVENTS_ESPIONAGE) 
				{
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_ElectionResultSuccess, (int)ePlayer, iSpyID, iValue, pCapital->getX(), pCapital->getY());
				}

				GET_PLAYER(ePlayer).doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);
				GET_PLAYER(ePlayer).GetEspionage()->LevelUpSpy(iSpyID, /*20 in CP, 15 in VP*/ GD_INT_GET(ESPIONAGE_DIPLOMAT_SPY_EXPERIENCE));
			}
			else
			{
				int iFriendship = GetEffectiveFriendshipWithMajor(ePlayer);
				int iRelationshipAnchor = GetFriendshipAnchorWithMajor(ePlayer);
				bool bFriends = IsFriends(ePlayer);
				bool bMet = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(m_pPlayer->getTeam());

				// if they have a spy in the city
				if(apSpy[ui] != NULL)
				{
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE_S");
						strSummary << pCapital->getNameKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE");
						strNotification << GET_PLAYER(ePlayer).GetEspionage()->GetSpyRankName(apSpy[ui]->m_eRank);
						strNotification << apSpy[ui]->GetSpyName(&GET_PLAYER(ePlayer));
						strNotification << pCapital->getNameKey();
						strNotification << GET_PLAYER(eElectionWinner).getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_FAILURE, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
					}
				}
				else if (bMet && (bFriends || iFriendship > iRelationshipAnchor))
				{
					// no spy in the city, so just give them an alert that scenanigans are going on
					CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT_S");
						strSummary << pCapital->getNameKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT");
						strNotification << pCapital->getNameKey();
						pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_ALERT, strNotification.toUTF8(), strSummary.toUTF8(), pCapital->getX(), pCapital->getY(), -1);
					}
				}

				int iDiminishAmount = 0;
				if (GetEffectiveFriendshipWithMajorTimes100(ePlayer) > 0)
				{
					iDiminishAmount = min(/*500*/ GD_INT_GET(ESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION) * 100, GetEffectiveFriendshipWithMajorTimes100(ePlayer));
					if (MOD_BALANCE_CORE_SPIES_ADVANCED)
					{
						iDiminishAmount *= GC.getGame().getCurrentEra();
					}
					ChangeFriendshipWithMajorTimes100(ePlayer, -iDiminishAmount, false);
					
					GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(eElectionWinner, 1);
					//GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eElectionWinner, 1);
				}
#if defined(MOD_EVENTS_ESPIONAGE)
				int iLoop = 0;
				int iSpyID = -1;
				for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
				{
					CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
					iSpyID = pCityEspionage->m_aiSpyAssignment[ePlayer];
					if (iSpyID != -1)
					{
						break;
					}
				}
				CvAssertMsg(iSpyID == -1, "Couldn't find a spy in any of the cities of the Minor Civ")

				if (MOD_EVENTS_ESPIONAGE) {
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_ElectionResultFailure, (int)ePlayer, iSpyID, iDiminishAmount, pCapital->getX(), pCapital->getY());
				}
#endif
			}
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
}


// ******************************
// ***** General Minor Civ Stuff *****
// ******************************


// How many units has ePlayer gifted this minor? (used to prevent unit spam for influence exploits)
int CvMinorCivAI::GetNumUnitsGifted(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // as defined during Reset()

	return m_aiNumUnitsGifted[ePlayer];
}

// How many units has ePlayer gifted this minor? (used to prevent unit spam for influence exploits)
void CvMinorCivAI::SetNumUnitsGifted(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_aiNumUnitsGifted[ePlayer] = iValue;
}

// How many units has ePlayer gifted this minor? (used to prevent unit spam for influence exploits)
void CvMinorCivAI::ChangeNumUnitsGifted(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	SetNumUnitsGifted(ePlayer, GetNumUnitsGifted(ePlayer) + iChange);
}

void CvMinorCivAI::DoUnitGiftFromMajor(PlayerTypes eFromPlayer, CvUnit*& pGiftUnit, bool bDistanceGift)
{
	CvAssertMsg(eFromPlayer >= 0, "eFromPlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFromPlayer < MAX_MAJOR_CIVS, "eFromPlayer is expected to be within maximum bounds (invalid Index)");
	if (eFromPlayer < 0 || eFromPlayer >= MAX_MAJOR_CIVS) return;

	CvAssertMsg(pGiftUnit != NULL, "pGiftUnit is NULL");
	if (pGiftUnit == NULL) return;

	ChangeNumUnitsGifted(eFromPlayer, 1);

	// Influence
	int iInfluence = GetFriendshipFromUnitGift(eFromPlayer, pGiftUnit->IsGreatPerson(), bDistanceGift);
	ChangeFriendshipWithMajor(eFromPlayer, iInfluence);

	if (IsUnitValidGiftForCityStateQuest(eFromPlayer, pGiftUnit))
	{
		SetHasSentUnitForQuest(eFromPlayer, true);
		DoTestActiveQuestsForPlayer(eFromPlayer, true, false, MINOR_CIV_QUEST_GIFT_SPECIFIC_UNIT);
	}

#if defined(MOD_EVENTS_MINORS_INTERACTION)
	if (MOD_EVENTS_MINORS_INTERACTION) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerGifted, eFromPlayer, GetPlayer()->GetID(), -1, pGiftUnit->getUnitType(), -1, -1);
	}
#endif

	// We can't keep Great Person units
	if (pGiftUnit->IsGreatPerson())
	{
		pGiftUnit->kill(false);
		pGiftUnit = NULL; // Important to NULL the unit here so that the caller knows we've done away with it.
	}
}

int CvMinorCivAI::GetFriendshipFromUnitGift(PlayerTypes eFromPlayer, bool bGreatPerson, bool /*bDistanceGift*/)
{
	if (eFromPlayer < 0 || eFromPlayer >= MAX_MAJOR_CIVS) return 0;

	int iInfluence = 0;
	CvPlayer& kFromPlayer = GET_PLAYER(eFromPlayer);

	// Influence
	if (bGreatPerson)
	{
		int iGPInfluence = kFromPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence();
		if (iGPInfluence > 0)
		{
			iInfluence += iGPInfluence;
		}
	}
	else
	{
		iInfluence += /*5 in CP, 15 in VP*/ GD_INT_GET(FRIENDSHIP_PER_UNIT_GIFTED);

		// War state
		if (IsProxyWarActiveForMajor(eFromPlayer))
		{
			iInfluence *= 2;
		}
		
		// Policies
		int iMilitaryInfluence = kFromPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_MILITARY_UNIT_GIFT_INFLUENCE);
		if (iMilitaryInfluence > 0)
		{
			iInfluence += iMilitaryInfluence;
		}

		iInfluence -= min((/*7*/ GD_INT_GET(FRIENDSHIP_PER_UNIT_GIFTED) / 2), (GetPlayer()->getNumMilitaryUnits()));
	}

	return iInfluence;
}

// How much gold has ePlayer gifted this minor?
int CvMinorCivAI::GetNumGoldGifted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return 0;  // as defined in Reset()
	return m_aiNumGoldGifted[ePlayer];
}

void CvMinorCivAI::SetNumGoldGifted(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;
	m_aiNumGoldGifted[ePlayer] = iValue;
}

void CvMinorCivAI::ChangeNumGoldGifted(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;
	SetNumGoldGifted(ePlayer, GetNumGoldGifted(ePlayer) + iChange);
}


/// Major Civ gifted some Gold to this Minor
void CvMinorCivAI::DoGoldGiftFromMajor(PlayerTypes ePlayer, int iGold)
{
	if(GET_PLAYER(ePlayer).GetTreasury()->GetGold() >= iGold)
	{
		int iFriendshipChange = GetFriendshipFromGoldGift(ePlayer, iGold);
		if(iFriendshipChange > 0)
			GET_PLAYER(ePlayer).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString( CivsList(1,GetPlayer()->GetID()) ), iGold,4);

		GET_PLAYER(ePlayer).GetTreasury()->ChangeGold(-iGold);
		
		ChangeNumGoldGifted(ePlayer, iGold);
		
		ChangeFriendshipWithMajor(ePlayer, iFriendshipChange);

		// In case we had a Gold Gift quest active, complete it now
		DoTestActiveQuestsForPlayer(ePlayer, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_GIVE_GOLD);
		
#if defined(MOD_EVENTS_MINORS_INTERACTION)
		if (MOD_EVENTS_MINORS_INTERACTION) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerGifted, ePlayer, GetPlayer()->GetID(), iGold, -1, -1, -1);
		}
#endif
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// How many friendship points gained from a gift of Gold
int CvMinorCivAI::GetFriendshipFromGoldGift(PlayerTypes eMajor, int iGold)
{
	// The more Gold you spend the more Friendship you get!
	iGold = (int) pow((double) iGold, (double) /*1.01f*/ GD_FLOAT_GET(GOLD_GIFT_FRIENDSHIP_EXPONENT));
	// The higher this divisor the less Friendship is gained
	int iFriendship = int(iGold / /*9.8f in CP, 18f in CSD*/ GD_FLOAT_GET(GOLD_GIFT_FRIENDSHIP_DIVISOR));

	// Game progress factor based on how far into the game we are
	double fGameProgressFactor = float(GC.getGame().getElapsedGameTurns()) / GC.getGame().getEstimateEndTurn();
	fGameProgressFactor = min(fGameProgressFactor, 1.0); // Don't count above 1.0, otherwise it will end up negative!
	
	// Tweak factor slightly, otherwise Gold will do literally NOTHING once we reach the end of the game!
	fGameProgressFactor *= /*2*/ GD_INT_GET(MINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER);
	fGameProgressFactor /= /*3*/ GD_INT_GET(MINOR_CIV_GOLD_GIFT_GAME_DIVISOR);
	fGameProgressFactor = 1 - fGameProgressFactor;

	iFriendship = (int)(iFriendship * fGameProgressFactor);


	// Mod (Policies, etc.)
	int iFriendshipMod = GET_PLAYER(eMajor).getMinorGoldFriendshipMod();

	if (IsSameReligionAsMajor(eMajor))
		iFriendshipMod += GET_PLAYER(eMajor).GetReligions()->GetCityStateInfluenceModifier(eMajor);

	if(iFriendshipMod != 0)
	{
		iFriendship *= (100 + iFriendshipMod);
		iFriendship /= 100;
	}

	// Game Speed Mod
	iFriendship *= GC.getGame().getGameSpeedInfo().getGoldGiftMod();
	iFriendship /= 100;

	//antonjs: todo: refine
	if(IsActiveQuestForPlayer(eMajor, MINOR_CIV_QUEST_INVEST))
	{
		int iBoostPercentage = 50; //antonjs: todo: xml
		iFriendship *= 100 + iBoostPercentage;
		iFriendship /= 100;
	}

	// Friendship gained should always be positive
	int iMinimumReward = max(/*5 in CP, 0 in CSD*/ GD_INT_GET(MINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD), 0);
	iFriendship = max(iFriendship, iMinimumReward);

	// Round the number so it's pretty
	int iVisibleDivisor = /*5*/ max(GD_INT_GET(MINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR), 1);
	iFriendship /= iVisibleDivisor;
	iFriendship *= iVisibleDivisor;

	return iFriendship;
}

// Can this major gift us a tile improvement?
bool CvMinorCivAI::CanMajorGiftTileImprovement(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if(pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return false;
	}

	// Must have enough gold
	const int iCost = GetGiftTileImprovementCost(eMajor);
	if(pPlayer->GetTreasury()->GetGold() < iCost)
		return false;

	// Must own an improveable plot
	bool bHasValidPlot = false;
	const PlotIndexContainer& aiPlots = GetPlayer()->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
		{
			continue;
		}
		if (IsLackingGiftableTileImprovementAtPlot(eMajor, pPlot->getX(), pPlot->getY()))
		{
			bHasValidPlot = true;
			break;
		}
	}

	if (!bHasValidPlot)
		return false;

#if defined(MOD_EVENTS_MINORS_INTERACTION)
	if (MOD_EVENTS_MINORS_INTERACTION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanGiftImprovement, eMajor, GetPlayer()->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif				
	
	return true;
}

CvPlot* CvMinorCivAI::GetMajorGiftTileImprovement(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return NULL;
	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if (pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return NULL;
	}

	// Must have enough gold
	const int iCost = GetGiftTileImprovementCost(eMajor);
	if (pPlayer->GetTreasury()->GetGold() < iCost)
		return NULL;

	int iBestScore = 0;
	CvPlot* pBestPlot = NULL;
	// Must own an improveable plot
	bool bHasValidPlot = false;
	const PlotIndexContainer& aiPlots = GetPlayer()->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
		{
			continue;
		}
		if (IsLackingGiftableTileImprovementAtPlot(eMajor, pPlot->getX(), pPlot->getY()))
		{
			if (pPlot->getResourceType(GET_PLAYER(eMajor).getTeam()) != NO_RESOURCE)
			{
				int iScore = 0;
				ResourceUsageTypes eUsage = GC.getResourceInfo(pPlot->getResourceType(GET_PLAYER(eMajor).getTeam()))->getResourceUsage();
				if (eUsage == RESOURCEUSAGE_STRATEGIC)
				{
					iScore = (pPlot->getNumResource() * 100);
				}
				else if (eUsage == RESOURCEUSAGE_LUXURY)
				{
					iScore = 150;
					// New? Ooh.
					if (GET_PLAYER(eMajor).getNumResourceTotal(pPlot->getResourceType(GET_PLAYER(eMajor).getTeam()), /*bIncludeImport*/ true) == 0)
						iScore += 100;
				}
				if (iScore > iBestScore)
				{
					iBestScore = iScore;
					pBestPlot = pPlot;
				}
			}
		}
	}

	if (!bHasValidPlot)
		return pBestPlot;

	return pBestPlot;
}

// Does this plot lack a resource tile improvement that eMajor has access to?
bool CvMinorCivAI::IsLackingGiftableTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	CvAssertMsg(eMajor >= 0, "eMajor is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "eMajor is expected to be within maximum bounds (invalid Index)");
	if(eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(pPlot == NULL)
	{
		CvAssertMsg(false, "pPlot not expected to be NULL, invalid coordinates. Please send Anton your save file and version.");
		return false;
	}

	// Only allowed to improve plots that the minor actually owns
	if(pPlot->getOwner() != GetPlayer()->GetID())
		return false;

	// Only allowed to improve Luxury and Strategic resources
	ResourceTypes eResource = pPlot->getResourceType(GET_PLAYER(eMajor).getTeam());
	if (eResource == NO_RESOURCE)
		return false;

	CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	if (!pkResourceInfo)
		return false;

	ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();
	if (eUsage != RESOURCEUSAGE_STRATEGIC && eUsage != RESOURCEUSAGE_LUXURY)
		return false;

	ImprovementTypes eImprovement = (ImprovementTypes)pPlot->getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (pImprovementInfo != NULL)
		{
			// Existing improvement that connects the resource? If it's pillaged, we can repair it. Otherwise, we can't improve this tile.
			// todo: remove yucky hardcoding of GPTIs connecting resources
			if (pImprovementInfo->IsConnectsResource(eResource) || pImprovementInfo->IsCreatedByGreatPerson())
				return pPlot->IsImprovementPillaged();
			else if (pImprovementInfo->IsPermanent())
				return false;
		}
	}

	eImprovement = pPlot->getImprovementTypeNeededToImproveResource(eMajor, /*bTestOwner*/ false, true);

	// There must be a valid improvement for the player to build
	if (eImprovement == NO_IMPROVEMENT)
		return false;

	return true;
}

// Convenience wrapper function
bool CvMinorCivAI::CanMajorGiftTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return false;
	return (CanMajorGiftTileImprovement(eMajor) && IsLackingGiftableTileImprovementAtPlot(eMajor, iPlotX, iPlotY));
}

int CvMinorCivAI::GetGiftTileImprovementCost(PlayerTypes eMajor)
{
	if (eMajor < 0 || eMajor >= MAX_MAJOR_CIVS) return -1;
	return /*200*/ GD_INT_GET(MINOR_CIV_TILE_IMPROVEMENT_GIFT_COST);
}

void CvMinorCivAI::DoTileImprovementGiftFromMajor(PlayerTypes eMajor, int iPlotX, int iPlotY)
{
	if(!CanMajorGiftTileImprovementAtPlot(eMajor, iPlotX, iPlotY))
	{
		return;
	}
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	if(pPlot == NULL)
	{
		CvAssertMsg(false, "pPlot not expected to be NULL, invalid coordinates. Please send Anton your save file and version.");
		return;
	}
	CvPlayer* pPlayer = &GET_PLAYER(eMajor);
	if(pPlayer == NULL)
	{
		CvAssertMsg(false, "pPlayer not expected to be NULL. Please send Anton your save file and version.");
		return;
	}

	ResourceTypes eResource = pPlot->getResourceType(GET_PLAYER(eMajor).getTeam());
	ImprovementTypes eImprovement = NO_IMPROVEMENT;
	ImprovementTypes eCurrentImprovement = (ImprovementTypes)pPlot->getImprovementType();
	CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(eCurrentImprovement);
	if (eCurrentImprovement != NO_IMPROVEMENT && pImprovementInfo != NULL && (pImprovementInfo->IsConnectsResource(eResource) || pImprovementInfo->IsCreatedByGreatPerson()))
	{
		// If we got here, IsLackingGiftableTileImprovementAtPlot() has verified that there is an existing improvement which connects the resource on the tile, and it's pillaged.
		// So unpillage it instead of building something new. Calling setImprovementType() will unpillage it.
		eImprovement = eCurrentImprovement;
	}
	else
	{
		eImprovement = pPlot->getImprovementTypeNeededToImproveResource(eMajor, /*bTestOwner*/ false, true);
		if (eImprovement == NO_IMPROVEMENT)
			return;
	}

	pPlot->setImprovementType(eImprovement, eMajor);

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
		{
			CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes)iI);
			if (!pkBuildInfo)
				continue;

			ImprovementTypes eLoopImprovement = ((ImprovementTypes)(pkBuildInfo->getImprovement()));

			if (eImprovement == eLoopImprovement)
			{
				if (pkBuildInfo->isFeatureRemove(pPlot->getFeatureType()))
				{
					pPlot->setFeatureType(NO_FEATURE);
					break;
				}
			}
		}
	}

	for (int iI = 0; iI < pPlot->getNumUnits(); iI++)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(iI);
		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->getBuildType() != NO_BUILD)
			{
				pLoopUnit->SetAutomateType(NO_AUTOMATE);
				pLoopUnit->ClearMissionQueue();
				pLoopUnit->SetActivityType(ACTIVITY_AWAKE);
			}
		}
	}

	// VFX
	CvInterfacePtr<ICvPlot1> pDllPlot(new CvDllPlot(pPlot));
	gDLL->GameplayDoFX(pDllPlot.get());

	const int iCost = GetGiftTileImprovementCost(eMajor);
	GET_PLAYER(eMajor).GetTreasury()->LogExpenditure(GetPlayer()->GetMinorCivAI()->GetNamesListAsString( CivsList(1,GetPlayer()->GetID()) ), iCost,5);
	pPlayer->GetTreasury()->ChangeGold(-iCost);

	if (MOD_EVENTS_MINORS_INTERACTION)
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerGifted, eMajor, GetPlayer()->GetID(), -1, -1, iPlotX, iPlotY);
}

/// Now at war with eTeam
void CvMinorCivAI::DoNowAtWarWithTeam(TeamTypes eTeam)
{
	int iWarFriendship = /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR);

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			// Friendship change
			int iOldFriendship = GetBaseFriendshipWithMajor(ePlayer);
			DoFriendshipChangeEffects(ePlayer, iOldFriendship, iWarFriendship);

			// Revoke PtP is there was one
			if (IsProtectedByMajor(ePlayer))
			{
#if defined(MOD_BALANCE_CORE)
				DoChangeProtectionFromMajor(ePlayer, false, true);
#else
				DoChangeProtectionFromMajor(ePlayer, false);
#endif
			}

			// Revoke quests if there were any
			if (GetNumActiveQuestsForPlayer(ePlayer) > 0)
			{
				EndAllActiveQuestsForPlayer(ePlayer, true);
			}

			// Nullify Quests - Deprecated?
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->SetPeaceQuestCompletedByMajor((PlayerTypes) iMajorCivLoop, true);
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->SetWarQuestCompletedByMajor((PlayerTypes) iMajorCivLoop, true);

			//// Is this player declaring war also already a Bully?  If so, he's gonna regret it
			//GET_PLAYER((PlayerTypes) iMinorCivLoop).GetMinorCivAI()->DoBullyDeclareWar((PlayerTypes) iMajorCivLoop);
		}
		else if (!IsAtWarWithPlayersTeam(ePlayer) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(eTeam))
		{
			// If ePlayer is also at war with eTeam, we might shorten the unit spawn timer
			if (IsFriends(ePlayer) && GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
			{
				int iBaseSpawnTurns = GetSpawnBaseTurns(ePlayer); // May be significantly less now with common enemy due to social policy
				iBaseSpawnTurns = MAX(iBaseSpawnTurns, 1);
				if (iBaseSpawnTurns < GetUnitSpawnCounter(ePlayer))
				{
					SetUnitSpawnCounter(ePlayer, iBaseSpawnTurns);
				}
			}
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// Now at peace with eTeam
void CvMinorCivAI::DoNowPeaceWithTeam(TeamTypes eTeam)
{
	int iWarFriendship = /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR);

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;

		if (GET_PLAYER(ePlayer).getTeam() == eTeam)
		{
			// Friendship change
			int iNewFriendship = GetBaseFriendshipWithMajor(ePlayer);
			DoFriendshipChangeEffects(ePlayer, iWarFriendship, iNewFriendship);
		}
	}

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// Will this AI allow peace with ePlayer?
bool CvMinorCivAI::IsPeaceBlocked(TeamTypes eTeam) const
{
	// Permanent war?
	if (GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) || IsPermanentWar(eTeam))
		return true;

	// Allies with someone at war with this guy?
	for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajor = (PlayerTypes) iMajorLoop;

		// Major must be alive
		if (!GET_PLAYER(eMajor).isAlive())
			continue;

		// Must be allies
		if (!IsAllies(eMajor))
			continue;

		// Ally must be at war with this team
		if (!GET_TEAM(GET_PLAYER(eMajor).getTeam()).isAtWar(eTeam))
			continue;

		return true;
	}

	return false;
}

/// eTeam declared war on us
void CvMinorCivAI::DoTeamDeclaredWarOnMe(TeamTypes eEnemyTeam)
{
	CvTeam* pEnemyTeam = &GET_TEAM(eEnemyTeam);
	CivsList veMinorsNowWary;
	int iRand = 0;
	bool bOthersDontUpdateWariness = false;

	// Since eEnemyTeam was the aggressor, drop the base influence to the minimum
	for(int iEnemyMajorLoop = 0; iEnemyMajorLoop < MAX_MAJOR_CIVS; iEnemyMajorLoop++)
	{
		PlayerTypes eEnemyMajorLoop = (PlayerTypes) iEnemyMajorLoop;
		if(!GET_PLAYER(eEnemyMajorLoop).isAlive())
			continue;
		if(GET_PLAYER(eEnemyMajorLoop).getTeam() != eEnemyTeam)
			continue;

		GET_PLAYER(eEnemyMajorLoop).SetTurnLastAttackedMinorCiv(GC.getGame().getGameTurn());

		// Player has already attacked this minor recently
		int iTurn = GetTurnLastAttacked(eEnemyTeam);
		int iTurnDifference = GC.getGame().getGameTurn() - iTurn;
		if (iTurn > -1 && iTurnDifference < 50)
		{
			bOthersDontUpdateWariness = true;
		}

		//antonjs: consider: forcibly revoke PtP here instead, and have negative INF / broken PtP fallout
		
		SetFriendshipWithMajor(eEnemyMajorLoop, /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR), false, true);
#if defined(MOD_BALANCE_CORE)
		DoChangeProtectionFromMajor(eEnemyMajorLoop, false, true);
		if(GetNumActiveQuestsForPlayer(eEnemyMajorLoop) > 0)
		{
			EndAllActiveQuestsForPlayer(eEnemyMajorLoop, true);
		}
#endif
	}

	//antonjs: todo: xml, rename xml to indicate it is for WaryOf, not Permanent War
	// Minor Civ Warmonger
	if (pEnemyTeam->IsMinorCivWarmonger() || pEnemyTeam->IsMinorCivAggressor())
	{
		if(!IsWaryOfTeam(eEnemyTeam))
		{
			SetWaryOfTeam(eEnemyTeam, true);
			veMinorsNowWary.push_back(GetPlayer()->GetID());
		}

		if (pEnemyTeam->IsMinorCivWarmonger())
		{
			if (ENABLE_PERMANENT_WAR)
				SetPermanentWar(eEnemyTeam, true);
		}
		else
		{
			iRand = GC.getGame().getSmallFakeRandNum(100, m_pPlayer->GetPseudoRandomSeed());

			if (iRand < /*50 in CP, 40 in VP*/ GD_INT_GET(PERMANENT_WAR_AGGRESSOR_CHANCE))
			{
				if (ENABLE_PERMANENT_WAR)
					SetPermanentWar(eEnemyTeam, true);
			}
		}
	}

	// See if other minors will declare war
	if(pEnemyTeam->IsMinorCivAggressor() && !bOthersDontUpdateWariness)
	{
		int iChance = 0;

		PlayerProximityTypes eProximity;

		int iAttackingMajorPlayer = 0;
		PlayerTypes eAttackingMajorPlayer;
		bool bAttackerIsAlly = false;

		CvPlayer* pOtherMinorCiv = NULL;
		PlayerTypes eOtherMinorCiv;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eOtherMinorCiv = (PlayerTypes) iMinorCivLoop;
			pOtherMinorCiv = &GET_PLAYER((eOtherMinorCiv));

			iChance = 0;

			// Must be alive
			if(!pOtherMinorCiv->isAlive())
				continue;

			// Must be a different minor
			if(eOtherMinorCiv == GetPlayer()->GetID())
				continue;

			// Ignore minors that want THIS minor dead
			if(pOtherMinorCiv->GetMinorCivAI()->IsWantsMinorDead(GetPlayer()->GetID()))
				continue;

			bAttackerIsAlly = false;

			// Ignore minors that are allied to the attacker
			for(iAttackingMajorPlayer = 0; iAttackingMajorPlayer < MAX_MAJOR_CIVS; iAttackingMajorPlayer++)
			{
				eAttackingMajorPlayer = (PlayerTypes) iAttackingMajorPlayer;

				// Not on this team
				if(GET_PLAYER(eAttackingMajorPlayer).getTeam() != eEnemyTeam)
					continue;

				// Not alive
				if(!GET_PLAYER(eAttackingMajorPlayer).isAlive())
					continue;

				if(pOtherMinorCiv->GetMinorCivAI()->GetAlly() == eAttackingMajorPlayer)
				{
					bAttackerIsAlly = true;
					break;
				}
			}

			if(bAttackerIsAlly)
				continue;

			// Closer to the minor the more likely war is
			eProximity = pOtherMinorCiv->GetProximityToPlayer(GetPlayer()->GetID());

			// Warmonger
			if(pEnemyTeam->IsMinorCivWarmonger())
			{
				if (eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance += /*20*/ GD_INT_GET(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT);
				else if(eProximity == PLAYER_PROXIMITY_FAR)
					iChance += /*30*/ GD_INT_GET(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR);
				else if(eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance += /*50*/ GD_INT_GET(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE);
				else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance += /*60*/ GD_INT_GET(PERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT);
			}
			// Aggressor
			else
			{
				if(eProximity == PLAYER_PROXIMITY_DISTANT)
					iChance += /*10*/ GD_INT_GET(PERMANENT_WAR_OTHER_CHANCE_DISTANT);
				else if(eProximity == PLAYER_PROXIMITY_FAR)
					iChance += /*15*/ GD_INT_GET(PERMANENT_WAR_OTHER_CHANCE_FAR);
				else if(eProximity == PLAYER_PROXIMITY_CLOSE)
					iChance += /*20*/ GD_INT_GET(PERMANENT_WAR_OTHER_CHANCE_CLOSE);
				else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
					iChance += /*25*/ GD_INT_GET(PERMANENT_WAR_OTHER_CHANCE_NEIGHBORS);
			}

			// If the minor is already at war, then there's a chance of it turning into permanent war
			if(GET_TEAM(pOtherMinorCiv->getTeam()).isAtWar(eEnemyTeam))
				iChance += /*20*/ GD_INT_GET(PERMANENT_WAR_OTHER_AT_WAR);

			iRand = GC.getGame().getSmallFakeRandNum(100, m_pPlayer->GetPseudoRandomSeed() + iMinorCivLoop);
			if(iRand < iChance)
			{
				if(!pOtherMinorCiv->GetMinorCivAI()->IsWaryOfTeam(eEnemyTeam))
				{
					pOtherMinorCiv->GetMinorCivAI()->SetWaryOfTeam(eEnemyTeam, true);
					veMinorsNowWary.push_back(eOtherMinorCiv);
				}
				if(ENABLE_PERMANENT_WAR)
					pOtherMinorCiv->GetMinorCivAI()->SetPermanentWar(eEnemyTeam, true);
			}
		}
	}

	if(veMinorsNowWary.size() > 0)
	{
		int iMinimumFriendshipMod = 20; //antonjs: todo: xml
		Localization::String strTemp;

		strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_MINORS_NOW_WARY");
		CvString strSummary = strTemp.toUTF8();
		strTemp = Localization::Lookup("TXT_KEY_NOTIFICATION_MINORS_NOW_WARY_TT");
		strTemp << iMinimumFriendshipMod;
		CvString strMessage = strTemp.toUTF8();

		for(int iEnemyMajorLoop = 0; iEnemyMajorLoop < MAX_MAJOR_CIVS; iEnemyMajorLoop++)
		{
			PlayerTypes eEnemyMajorLoop = (PlayerTypes) iEnemyMajorLoop;
			if(!GET_PLAYER(eEnemyMajorLoop).isAlive())
				continue;
			if(GET_PLAYER(eEnemyMajorLoop).getTeam() != eEnemyTeam)
				continue;

			strMessage = strMessage + GetNamesListAsString(veMinorsNowWary);
			AddNotification(strMessage, strSummary, eEnemyMajorLoop);
		}
	}
#if defined(MOD_BALANCE_CORE_MINORS)
	if (MOD_BALANCE_CORE_MINORS) 
	{
		SetTurnLastAttacked(eEnemyTeam, GC.getGame().getGameTurn());
		SetIgnoreJerk(eEnemyTeam, false);
	}
#endif

	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
}

/// Permanent War with eTeam?
bool CvMinorCivAI::IsPermanentWar(TeamTypes eTeam) const
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return false;
	return m_abPermanentWar[eTeam];
}

/// Permanent War with eTeam?
void CvMinorCivAI::SetPermanentWar(TeamTypes eTeam, bool bValue)
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return;

	if (ENABLE_PERMANENT_WAR)
		m_abPermanentWar[eTeam] = bValue;

	else if (GET_TEAM(eTeam).isMinorCiv() && GET_TEAM(GetPlayer()->getTeam()).isMinorCiv())
		m_abPermanentWar[eTeam] = bValue;
}

// Wary of eTeam?
bool CvMinorCivAI::IsWaryOfTeam(TeamTypes eTeam) const
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return false;
	return m_abWaryOfTeam[eTeam];
}

// Wary of eTeam?
void CvMinorCivAI::SetWaryOfTeam(TeamTypes eTeam, bool bValue)
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return;
	m_abWaryOfTeam[eTeam] = bValue;
}

#if defined(MOD_BALANCE_CORE_MINORS)
//JERK COOLDOWN RATE
int CvMinorCivAI::GetTurnLastAttacked(TeamTypes eTeam) const
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return -1;
	return m_aiTurnLastAttacked[eTeam];
}

void CvMinorCivAI::SetTurnLastAttacked(TeamTypes eTeam, int iTurn)
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return;
	m_aiTurnLastAttacked[eTeam] = max(iTurn, -1);
}

int CvMinorCivAI::GetJerkTurnsRemaining(TeamTypes eTeam) const
{
	if (IsIgnoreJerk(eTeam))
		return 0;

	int iTurn = GetTurnLastAttacked(eTeam);
	if (iTurn < 0)
		return 0;

	int iJerkTurns = /*50*/ GD_INT_GET(BALANCE_CS_WAR_COOLDOWN_RATE);
	iJerkTurns *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iJerkTurns /= 100;

	if (iJerkTurns <= 0)
		return 0;

	int iTurnDifference = GC.getGame().getGameTurn() - iTurn;
	if (iTurnDifference >= iJerkTurns)
		return 0;

	return iJerkTurns - iTurnDifference;
}

bool CvMinorCivAI::IsIgnoreJerk(TeamTypes eTeam) const
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return false;
	return m_abIgnoreJerk[eTeam];
}

void CvMinorCivAI::SetIgnoreJerk(TeamTypes eTeam, bool bValue)
{
	if (eTeam < 0 || eTeam >= MAX_CIV_TEAMS) return;
	m_abIgnoreJerk[eTeam] = bValue;
}

PlayerTypes CvMinorCivAI::GetPermanentAlly() const
{
	return m_ePermanentAlly;
}
void CvMinorCivAI::SetPermanentAlly(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= NO_PLAYER, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (ePlayer != NO_PLAYER)
	{
		if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GetPlayer()->getTeam()))
		{
			GET_TEAM(GET_PLAYER(ePlayer).getTeam()).meet(GetPlayer()->getTeam(), false);
		}
	}

	m_ePermanentAlly = ePlayer;
}

bool CvMinorCivAI::IsNoAlly() const
{
	return m_bNoAlly;
}
void CvMinorCivAI::SetNoAlly(bool bValue)
{
	m_bNoAlly = bValue;
}
void CvMinorCivAI::ChangeCoupCooldown(int iChange)
{
	SetCoupCooldown(GetCoupCooldown() + iChange);
}
int CvMinorCivAI::GetCoupCooldown() const
{
	return m_iCoup;
}
void CvMinorCivAI::SetCoupCooldown(int iValue)
{
	if(GetCoupCooldown() != iValue)
	{
		m_iCoup = iValue;
	}
}
bool CvMinorCivAI::IsSiphoned(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < REALLY_MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(ePlayer < 0 || ePlayer >= REALLY_MAX_PLAYERS)
	{
		return false;  // as defined in Reset()
	}
	return m_abSiphoned[ePlayer];
}
void CvMinorCivAI::SetSiphoned(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < REALLY_MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	if(IsSiphoned(ePlayer) != bValue)
	{
		m_abSiphoned[ePlayer] = bValue;
	}
}
#endif

int CvMinorCivAI::GetRiggingCoupChanceIncrease(PlayerTypes ePlayer) const
{
	CvAssert(ePlayer >= 0);
	CvAssert(ePlayer < MAX_MAJOR_CIVS);
	return m_aiRiggingCoupChanceIncrease[ePlayer];
}

void CvMinorCivAI::ChangeRiggingCoupChanceIncrease(PlayerTypes ePlayer, int iChange)
{
	CvAssert(ePlayer >= 0);
	CvAssert(ePlayer < MAX_MAJOR_CIVS);
	m_aiRiggingCoupChanceIncrease[ePlayer] += iChange;
}

void CvMinorCivAI::ResetRiggingCoupChanceIncrease(PlayerTypes ePlayer)
{
	CvAssert(ePlayer >= 0);
	CvAssert(ePlayer < MAX_MAJOR_CIVS);
	m_aiRiggingCoupChanceIncrease[ePlayer] = 0;
}


int CvMinorCivAI::GetRestingPointChange(PlayerTypes ePlayer) const
{
	CvAssert(ePlayer >= 0);
	CvAssert(ePlayer < MAX_MAJOR_CIVS);
	return m_aiRestingPointChange[ePlayer];
}

void CvMinorCivAI::ChangeRestingPointChange(PlayerTypes ePlayer, int iChange)
{
	CvAssert(ePlayer >= 0);
	CvAssert(ePlayer < MAX_MAJOR_CIVS);
	m_aiRestingPointChange[ePlayer] += iChange;
}


const CvMinorCivIncomingUnitGift& CvMinorCivAI::getIncomingUnitGift(PlayerTypes eMajor) const
{
	CvAssert(eMajor >= 0);
	CvAssert(eMajor < MAX_MAJOR_CIVS);
	return m_IncomingUnitGifts[eMajor];
}

CvMinorCivIncomingUnitGift& CvMinorCivAI::getIncomingUnitGift(PlayerTypes eMajor)
{
	CvAssert(eMajor >= 0);
	CvAssert(eMajor < MAX_MAJOR_CIVS);
	return m_IncomingUnitGifts[eMajor];
}

void CvMinorCivAI::doIncomingUnitGifts()
{
	for (int iLoop = 0; iLoop < MAX_MAJOR_CIVS; iLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iLoop;
		CvMinorCivIncomingUnitGift& unitGift = getIncomingUnitGift(eLoopPlayer);

		if (unitGift.hasIncomingUnit())
		{
			unitGift.changeArrivalCountdown(-1);

			// Time to spawn a new unit
			if (unitGift.getArrivalCountdown() == 0)
			{
				// Must have capital to actually spawn unit
				CvCity* pCapital = GetPlayer()->getCapitalCity();
				if (pCapital)
				{
					CvUnit* pNewUnit = GetPlayer()->initUnit(unitGift.getUnitType(), pCapital->getX(), pCapital->getY());
					CvAssert(pNewUnit);
					if (pNewUnit)
					{
						unitGift.applyToUnit(eLoopPlayer, *pNewUnit);

						// Gift from a major to a city-state
						if (!GET_PLAYER(eLoopPlayer).isMinorCiv())
						{
							// Note that pNewUnit may be NULL'd here if DoUnitGiftFromMajor chooses to kill it.
							DoUnitGiftFromMajor(eLoopPlayer, pNewUnit, /*bDistanceGift*/ true);
						}

						// Must check if pNewUnit is valid because DoUnitGiftFromMajor may have killed it.
						if (pNewUnit)
						{
							if (pNewUnit->getDomainType() != DOMAIN_AIR)
							{
								if (!pNewUnit->jumpToNearestValidPlot())
								{
									pNewUnit->kill(false);
									pNewUnit = NULL;
								}
							}
						}
					}
				}

				// Reset stuff
				unitGift.reset();
			}
		}
	}
}

void CvMinorCivAI::returnIncomingUnitGift(PlayerTypes eMajor)
{
	CvMinorCivIncomingUnitGift& unitGift = getIncomingUnitGift(eMajor);
	if (unitGift.hasIncomingUnit())
	{
		const UnitTypes eUnitType = unitGift.getUnitType();
		CvAssert(eUnitType != NO_UNIT);
		const CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
		CvAssert(pkUnitInfo);

		// Select a default city to return to.
		// We will return to this city if no better city is found.
		const CvCity* pMajorDefaultCity = NULL;
		if (unitGift.getOriginCity() != -1)
		{
			pMajorDefaultCity = GET_PLAYER(eMajor).getCity(unitGift.getOriginCity());
		}

		if (pMajorDefaultCity == NULL)
		{
			pMajorDefaultCity = GET_PLAYER(eMajor).getCapitalCity();
		}

		if (pMajorDefaultCity)
		{
			const CvPlot* pReturnToPlot = pMajorDefaultCity->plot();
			const CvPlot* pFromPlot = unitGift.getFromPlot();
			if (pFromPlot == NULL)
			{
				// Try to guess where this unit is returning from.
				if (GetPlayer()->getCapitalCity() != NULL)
				{
					pFromPlot = GetPlayer()->getCapitalCity()->plot();
				}
				else
				{
					pFromPlot = GC.getMap().plotCheckInvalid(GetPlayer()->GetOriginalCapitalX(), GetPlayer()->GetOriginalCapitalY());
				}
			}

			// If we know where the unit is returning from, pick the city closest as the return to plot.
			if (pFromPlot)
			{
				CvCity* pClosestCity = NULL;
				if (pkUnitInfo->GetDomainType() == DOMAIN_SEA)
				{
					pClosestCity = OperationalAIHelpers::GetClosestFriendlyCoastalCity(eMajor, pFromPlot, 1);
				}
				else
				{
					pClosestCity = GET_PLAYER(eMajor).GetClosestCityByPlots(pFromPlot);
				}

				if (pClosestCity)
				{
					pReturnToPlot = pClosestCity->plot();
				}
			}

			CvUnit* pNewUnit = GET_PLAYER(eMajor).initUnit(eUnitType, pReturnToPlot->getX(), pReturnToPlot->getY());
			if (pNewUnit)
			{
				unitGift.applyToUnit(eMajor, *pNewUnit);
				if (pNewUnit->getDomainType() != DOMAIN_AIR && !pNewUnit->jumpToNearestValidPlot())
				{
					pNewUnit->kill(false);
				}
				else
				{
					CvNotifications* pNotify = GET_PLAYER(eMajor).GetNotifications();
					if (pNotify)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CS_GIFT_RETURNED_SUMMARY");
						strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_CS_GIFT_RETURNED");
						strNotification << GetPlayer()->getNameKey();
						strNotification << pNewUnit->getNameKey();
						pNotify->Add(NOTIFICATION_GENERIC, strNotification.toUTF8(), strSummary.toUTF8(), pNewUnit->getX(), pNewUnit->getY(), -1);
					}
				}
			}
		}

		unitGift.reset();
	}
}

// ******************************
// ***** Misc Helper Functions *****
// ******************************


/// Has met another Player?
bool CvMinorCivAI::IsHasMetPlayer(PlayerTypes ePlayer)
{
	return GET_TEAM(GetPlayer()->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam());
}

/// At war with a Player?
bool CvMinorCivAI::IsAtWarWithPlayersTeam(PlayerTypes ePlayer)
{
	return GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GetPlayer()->getTeam());
}

/// How many resources does this minor own that eMajor doesn't?
int CvMinorCivAI::GetNumResourcesMajorLacks(PlayerTypes eMajor)
{
	CvAssertMsg(eMajor >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(eMajor < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	int iNumTheyLack = 0;

	// Loop through all resources to see what this minor has
	ResourceTypes eResource;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		// Must not be a Bonus resource
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
			continue;

		// We must have it
		if(GetPlayer()->getNumResourceTotal(eResource, /*bIncludeImport*/ false) == 0)
			continue;

		// They must not have it
		if(GET_PLAYER(eMajor).getNumResourceTotal(eResource, /*bIncludeImport*/ false) > 0)
			continue;

		iNumTheyLack++;
	}

	return iNumTheyLack;
}

/// Helper function which returns a "Good" Tech that a Player doesn't have but CAN currently research
TechTypes CvMinorCivAI::GetGoodTechPlayerDoesntHave(PlayerTypes ePlayer, int iRoughTechValue) const
{
	CvAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	CvWeightedVector<int> TechVector;
	int iValue = 0, iProgress = 0;


	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTechLoop);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(pkTechInfo == NULL)
			continue;

		// Player doesn't already have Tech
		if(!kTeam.GetTeamTechs()->HasTech(eTech))
		{
			// Player can research this Tech
			if(kPlayer.GetPlayerTechs()->CanResearch(eTech))
			{
				iValue = pkTechInfo->GetResearchCost();

				// Reduce value of a Tech if it's already in progress
				iProgress = kTeam.GetTeamTechs()->GetResearchProgress(eTech);

				if(iProgress > 0)
				{
					iValue -= iProgress;
				}

				// Random factor so that the same thing isn't always picked
				iValue += GC.getGame().getSmallFakeRandNum(iValue / 4, m_pPlayer->GetTreasury()->GetLifetimeGrossGold() + iTechLoop);

				TechVector.push_back(iTechLoop, iValue);
			}
		}
	}

	// If there's only one option return it... this will help prevent divide by zero stuff later
	if(TechVector.size() == 1)
	{
		return (TechTypes) TechVector.GetElement(0);
	}
	else if(TechVector.size() == 0)
	{
		return NO_TECH;
	}

	TechVector.StableSortItems();

	// Our rough estimate is that 20 is a good ceiling for the max Tech value
	if(iRoughTechValue > 20)
	{
		iRoughTechValue = 20;
	}

	int iIndex = (TechVector.size() - 1) * iRoughTechValue / 20;

	return (TechTypes) TechVector.GetElement(iIndex);
}

/// Checks to see if the majority religion of the city-state is the religion that this major has founded
bool CvMinorCivAI::IsSameReligionAsMajor(PlayerTypes eMajor)
{
	CvCity* pCapital = GetPlayer()->getCapitalCity();
	if (pCapital)
	{
		ReligionTypes eMinorReligion = pCapital->GetCityReligions()->GetReligiousMajority();
		ReligionTypes eMajorReligion = GET_PLAYER(eMajor).GetReligions()->GetOwnedReligion();

		if (eMajorReligion != NO_RELIGION && eMajorReligion == eMinorReligion)
			return true;
	}

	return false;
}

CvString CvMinorCivAI::GetStatusChangeDetails(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies)
{
	Localization::String strDetailedInfo;

	MinorCivTraitTypes eTrait = GetTrait();

	if(eTrait == MINOR_CIV_TRAIT_CULTURED)
	{
		int iCultureBonusAmount = 0;
		if (bFriends)
		{
			iCultureBonusAmount += GetCultureFlatFriendshipBonus(ePlayer) + GetCulturePerBuildingFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iCultureBonusAmount += GetCultureFlatAlliesBonus(ePlayer) + GetCulturePerBuildingAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iCultureBonusAmount = -iCultureBonusAmount;
		}

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_CULTURE");
			strDetailedInfo << iCultureBonusAmount;
		}
	}
	else if(eTrait == MINOR_CIV_TRAIT_MILITARISTIC)
	{
#if defined(MOD_BALANCE_CORE)
		int iScienceBonusAmount = 0;
		if (bFriends)
		{
			iScienceBonusAmount += GetScienceFlatFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iScienceBonusAmount += GetScienceFlatAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iScienceBonusAmount = -iScienceBonusAmount;
		}
#endif
		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC");
#if defined(MOD_BALANCE_CORE)
		if(iScienceBonusAmount != 0)
		{
			strDetailedInfo << iScienceBonusAmount;
		}
#endif
		else if(bFriends && bAdd)		// Now Friends
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC");
#if defined(MOD_BALANCE_CORE)
			if(iScienceBonusAmount != 0)
			{
				strDetailedInfo << iScienceBonusAmount;
			}
#endif
		else if(bFriends && !bAdd)		// No longer Friends (includes drop from Allies down to nothing) - this should be before the Allies check!
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC");
#if defined(MOD_BALANCE_CORE)
			if(iScienceBonusAmount != 0)
			{
				strDetailedInfo << iScienceBonusAmount;
			}
#endif
		else if(bAllies && !bAdd)		// No longer Allies
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC");
#if defined(MOD_BALANCE_CORE)
			if(iScienceBonusAmount != 0)
			{
				strDetailedInfo << iScienceBonusAmount;
			}
#endif
	}
	else if(eTrait == MINOR_CIV_TRAIT_MARITIME)
	{
		int iCapitalFoodTimes100 = 0;
		int iOtherCitiesFoodTimes100 = 0;

		if(bFriends)	// Friends bonus
		{
			iCapitalFoodTimes100 += GetFriendsCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetFriendsOtherCityFoodBonus(ePlayer);
		}
		if(bAllies)		// Allies bonus
		{
			iCapitalFoodTimes100 += GetAlliesCapitalFoodBonus(ePlayer);
			iOtherCitiesFoodTimes100 += GetAlliesOtherCityFoodBonus(ePlayer);
		}

		if(!bAdd)		// Flip amount of we're taking bonuses away
		{
			iCapitalFoodTimes100 = -iCapitalFoodTimes100;
			iOtherCitiesFoodTimes100 = -iOtherCitiesFoodTimes100;
		}

		// Now that we've changed the gameplay, add together the two so the DISPLAY looks right
		iCapitalFoodTimes100 += iOtherCitiesFoodTimes100;
		float fCapitalFood = float(iCapitalFoodTimes100) / 100;
		float fOtherCitiesFood = float(iOtherCitiesFoodTimes100) / 100;
		//iCapitalFood += iOtherCitiesFood;

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MARITIME");
			strDetailedInfo << fCapitalFood << fOtherCitiesFood;
		}
	}
	else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
	{
		int iHappinessBonus = 0;
#if defined(MOD_BALANCE_CORE)
		int iGoldBonusAmount = 0;
		if (bFriends)
		{
			iGoldBonusAmount += GetGoldFlatFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iGoldBonusAmount += GetGoldFlatAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iGoldBonusAmount = -iGoldBonusAmount;
		}
#endif
		if(bFriends)	// Friends bonus
		{
			iHappinessBonus += GetHappinessFlatFriendshipBonus(ePlayer) + GetHappinessPerLuxuryFriendshipBonus(ePlayer);
		}
		if(bAllies)		// Allies bonus
		{
			iHappinessBonus += GetHappinessFlatAlliesBonus(ePlayer) + GetHappinessPerLuxuryAlliesBonus(ePlayer);
		}
		if(!bAdd)		// Flip amount of we're taking bonuses away
		{
			iHappinessBonus = -iHappinessBonus;
		}

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
#if defined(MOD_BALANCE_CORE)
			if(iGoldBonusAmount != 0)
			{
				strDetailedInfo << iGoldBonusAmount;
			}
#endif
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
#if defined(MOD_BALANCE_CORE)
			if(iGoldBonusAmount != 0)
			{
				strDetailedInfo << iGoldBonusAmount;
			}
#endif
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_MERCANTILE");
			strDetailedInfo << iHappinessBonus;
#if defined(MOD_BALANCE_CORE)
			if(iGoldBonusAmount != 0)
			{
				strDetailedInfo << iGoldBonusAmount;
			}
#endif
		}
	}

	// Religious
	if(eTrait == MINOR_CIV_TRAIT_RELIGIOUS)
	{
		int iFaithBonusAmount = 0;
		if (bFriends)
		{
			iFaithBonusAmount += GetFaithFlatFriendshipBonus(ePlayer);
		}
		if (bAllies)
		{
			iFaithBonusAmount += GetFaithFlatAlliesBonus(ePlayer);
		}
		if (!bAdd)
		{
			iFaithBonusAmount = -iFaithBonusAmount;
		}

		if(bAllies && bAdd)		// Now Allies (includes jump from nothing through Friends to Allies)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
		else if(bFriends && bAdd)		// Now Friends
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
		else if(!bAdd)		// Bonus diminished (or removed)
		{
			strDetailedInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_LOST_RELIGIOUS");
			strDetailedInfo << iFaithBonusAmount;
		}
	}

	return strDetailedInfo.toUTF8();
}

pair<CvString, CvString> CvMinorCivAI::GetStatusChangeNotificationStrings(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, PlayerTypes eOldAlly, PlayerTypes eNewAlly)
{
	Localization::String strMessage = "";
	Localization::String strSummary = "";

	if (ePlayer != GetPermanentAlly())
	{
		CvTeam* pTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());
		CvAssertMsg(pTeam, "pTeam not expected to be NULL. Please send Anton your save file and version.");

		const char* strMinorsNameKey = GetPlayer()->getNameKey();

		// Adding/Increasing bonus
		if (bAdd)
		{
			// Jumped up to Allies (either from Neutral or from Friends, or passing another player)
			if (bAllies)
			{
				// BASE ALLIES MESSAGE

				// No previous Ally (or it was us)
				if (eOldAlly == NO_PLAYER || eOldAlly == ePlayer)
				{
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_BASE");
				}
				// We're passing someone
				else
				{
					CvAssertMsg(eOldAlly != NO_PLAYER, "eOldAlly not expected to be NO_PLAYER here. Please send Anton your save file and version.");
					const char* strOldBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
					TeamTypes eOldAllyTeam = GET_PLAYER(eOldAlly).getTeam();
					if (pTeam->isHasMet(eOldAllyTeam))
						strOldBestPlayersNameKey = GET_PLAYER(eOldAlly).getCivilizationShortDescriptionKey();

					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_BASE_PASSED");
					strMessage << strOldBestPlayersNameKey;
				}

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS");

				// Build Resource info
				int iNumResourceTypes = 0;
				vector<ResourceTypes> veResources;
				ResourceTypes eResource;
				ResourceUsageTypes eUsage;
				int iResourceQuantity = 0;
				for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
				{
					eResource = (ResourceTypes)iResourceLoop;
					iResourceQuantity = GetPlayer()->getNumResourceTotal(eResource);

					if (iResourceQuantity > 0)
					{
						const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
						if (pkResourceInfo != NULL)
						{
							eUsage = pkResourceInfo->getResourceUsage();

							if (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
							{
								veResources.push_back(eResource);
								iNumResourceTypes++;
							}
						}
					}
				}
				// APPEND RESOURCE INFO
				Localization::String strResourceDetails;
				if (iNumResourceTypes == 0)
				{
					strResourceDetails = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_NONE");
					strResourceDetails << strMinorsNameKey;
				}
				else
				{
					CvString strResourceNames = GC.getResourceInfo(veResources[0])->GetDescription();
					int i = 1;
					while (i < iNumResourceTypes)
					{
						strResourceNames += ", ";
						strResourceNames += GC.getResourceInfo(veResources[i++])->GetDescription();
					}

					strResourceDetails = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_GAINED_BEST_RELATIONS_BONUS_SOME");
					strResourceDetails << strResourceNames.c_str();
				}
				strMessage << strResourceDetails.toUTF8();
			}
			// Went from Neutral to Friends
			else if (bFriends)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_BASE");
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDS_STATUS");
			}
		}
		// Removing/Reducing bonus
		else
		{
			// Dropped from Allies
			if (bAllies)
			{
				// Normal friendship decay
				if (eNewAlly == NO_PLAYER)
				{
					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ALLIES_LOST");
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS_LOST");
				}
				// Someone passed us up
				else
				{
					CvAssertMsg(eNewAlly != NO_PLAYER, "eNewAlly not expected to be NO_PLAYER here. Please send Anton your save file and version.");
					CvAssertMsg(eNewAlly != ePlayer, "eNewAlly not expected to be same as ePlayer here. Please send Anton your save file and version.");
					const char* strNewBestPlayersNameKey = "TXT_KEY_UNMET_PLAYER";
					TeamTypes eNewAllyTeam = GET_PLAYER(eNewAlly).getTeam();
					if (pTeam->isHasMet(eNewAllyTeam))
						strNewBestPlayersNameKey = GET_PLAYER(eNewAlly).getCivilizationShortDescriptionKey();

					strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_ALLIES_PASSED");
					strMessage << strNewBestPlayersNameKey;
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_ALLIES_STATUS_PASSED");
					strSummary << strNewBestPlayersNameKey;
				}
			}
			// Dropped down to Neutral from Friends (case of Allies down to Neutral not handled well... let's hope it doesn't happen often!)
			else if (bFriends)
			{
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_MINOR_FRIENDS_LOST_BASE");
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_MINOR_FRIENDS_STATUS_LOST");
			}
		}

		CvString strDetailedInfo = GetStatusChangeDetails(ePlayer, bAdd, bFriends, bAllies);
		strMessage << GetPlayer()->getNameKey() << strDetailedInfo;
		strSummary << GetPlayer()->getNameKey();
	}

	pair<CvString, CvString> notifStrings = pair<CvString, CvString>(strMessage.toUTF8(), strSummary.toUTF8());
	return notifStrings;
}

// Return a string with the list of players' names, each on its own line (for notifications)
CvString CvMinorCivAI::GetNamesListAsString(CivsList veNames)
{
	CvString s = "";
	if (veNames.size() > 0)
	{
		for (uint ui = 0; ui < veNames.size(); ui++)
		{
			PlayerTypes ePlayerLoop = veNames[ui];
			if (ePlayerLoop != NO_PLAYER)
			{
				CvPlayer* pPlayerLoop = &GET_PLAYER(ePlayerLoop);
				if (pPlayerLoop)
				{
					Localization::String sName = Localization::Lookup(pPlayerLoop->getCivilizationShortDescriptionKey());
					s = s + "[NEWLINE]" + sName.toUTF8();
				}
			}
			else
			{
				s += "[NEWLINE](None)";
			}
		}
	}
	return s;
}

/// Have notifications been (temporarily) disabled?
bool CvMinorCivAI::IsDisableNotifications() const
{
	return m_bDisableNotifications;
}

/// Disable notifications for minor civs for now
void CvMinorCivAI::SetDisableNotifications(bool bDisableNotifications)
{
	if (m_bDisableNotifications != bDisableNotifications)
	{
		m_bDisableNotifications = bDisableNotifications;
	}
}

//======================================================================================================
//					CvMinorCivInfo
//======================================================================================================
CvMinorCivInfo::CvMinorCivInfo() :
	m_iDefaultPlayerColor(NO_PLAYERCOLOR),
	m_iArtStyleType(NO_ARTSTYLE),
	m_iMinorCivTrait(NO_MINOR_CIV_TRAIT_TYPE),
#if defined(MOD_BALANCE_CORE)
	m_iBullyUnit(NO_UNITCLASS),
#endif
	m_piFlavorValue(NULL)
{
}
//------------------------------------------------------------------------------
CvMinorCivInfo::~CvMinorCivInfo()
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getArtStylePrefix() const
{
	return m_strArtStylePrefix.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getArtStyleSuffix() const
{
	return m_strArtStyleSuffix.c_str();
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::getNumCityNames() const
{
	return m_vCityNames.size();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getShortDescription() const
{
	return m_wstrShortDescription.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getShortDescriptionKey() const
{
	return m_strShortDescriptionKey.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getShortDescriptionKeyWide() const
{
	return m_wstrShortDescriptionKey.c_str();
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setShortDescriptionKey(const char* szVal)
{
	m_strShortDescriptionKey = szVal;
	m_wstrShortDescriptionKey = szVal;
	m_wstrShortDescription = GetLocalizedText(m_strShortDescriptionKey.c_str());
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getAdjective()	const
{
	return m_wstrAdjective.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getAdjectiveKey() const
{
	return m_strAdjectiveKey.c_str();
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getAdjectiveKeyWide() const
{
	return m_wstrAdjectiveKey.c_str();
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setAdjectiveKey(const char* szVal)
{
	m_strAdjectiveKey = szVal;
	m_wstrAdjectiveKey = szVal;
	m_wstrAdjective = GetLocalizedText(m_strAdjectiveKey.c_str());
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getFlagTexture() const
{
	return NULL;
}
//------------------------------------------------------------------------------
const char* CvMinorCivInfo::getArtDefineTag() const
{
	return m_strArtDefineTag;
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setArtDefineTag(const char* szVal)
{
	m_strArtDefineTag = szVal;
}
//------------------------------------------------------------------------------
int CvMinorCivInfo::GetMinorCivTrait() const
{
	return m_iMinorCivTrait;
}
#if defined(MOD_BALANCE_CORE)
int CvMinorCivInfo::GetBullyUnit() const
{
	return m_iBullyUnit;
}
#endif
//------------------------------------------------------------------------------
int CvMinorCivInfo::getFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}
//------------------------------------------------------------------------------
const std::string& CvMinorCivInfo::getCityNames(int i) const
{
	return m_vCityNames[i];
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setArtStylePrefix(const char* szVal)
{
	m_strArtStylePrefix = szVal;
}
//------------------------------------------------------------------------------
void CvMinorCivInfo::setArtStyleSuffix(const char* szVal)
{
	m_strArtStyleSuffix = szVal;
}
//------------------------------------------------------------------------------
bool CvMinorCivInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	const char* szTextVal = NULL;

	szTextVal = kResults.GetText("ShortDescription");
	setShortDescriptionKey(szTextVal);

	szTextVal = kResults.GetText("Adjective");
	setAdjectiveKey(szTextVal);

	szTextVal = kResults.GetText("ArtDefineTag");
	setArtDefineTag(szTextVal);

	//References
	szTextVal = kResults.GetText("DefaultPlayerColor");
	m_iDefaultPlayerColor = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ArtStyleType");
	m_iArtStyleType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ArtStylePrefix");
	setArtStylePrefix(szTextVal);

	szTextVal = kResults.GetText("ArtStyleSuffix");
	setArtStyleSuffix(szTextVal);

	szTextVal = kResults.GetText("MinorCivTrait");
	m_iMinorCivTrait = GC.getInfoTypeForString(szTextVal, true);

#if defined(MOD_BALANCE_CORE)
	szTextVal = kResults.GetText("BullyUnitClass");
	m_iBullyUnit = GC.getInfoTypeForString(szTextVal, true);
#endif

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "MinorCivilization_Flavors", "MinorCivType", szType, -1);

	//City Names
	{
		m_vCityNames.clear();

		std::string strKey = "MinorCiv - CityName";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select CityName from MinorCivilization_CityNames where MinorCivType = ?");
		}

		pResults->Bind(1, szType, -1, false);

		while(pResults->Step())
		{
			m_vCityNames.push_back(pResults->GetText(0));
		}

		pResults->Reset();
	}

	return true;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivStatusTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivStatusTypes& writeTo)
{
	int v = 0;
	loadFrom >> v;
	writeTo = static_cast<MinorCivStatusTypes>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivPersonalityTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivPersonalityTypes& writeTo)
{
	int v = 0;
	loadFrom >> v;
	writeTo = static_cast<MinorCivPersonalityTypes>(v);
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MinorCivQuestTypes& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MinorCivQuestTypes& writeTo)
{
	int v = 0;
	loadFrom >> v;
	writeTo = static_cast<MinorCivQuestTypes>(v);
	return loadFrom;
}

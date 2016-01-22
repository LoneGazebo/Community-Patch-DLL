/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvProcessProductionAI.h"
#include "CvInfosSerializationHelper.h"
#if defined(MOD_BALANCE_CORE)
#include "CvMilitaryAI.h"
#include "CvEconomicAI.h"
#include "CvVotingClasses.h"
#include "CvEconomicAI.h"
#include "CvWonderProductionAI.h"
#endif

// include this after all other headers!
#include "LintFree.h"

/// Constructor
CvProcessProductionAI::CvProcessProductionAI(CvCity* pCity):
	m_pCity(pCity)
{
}

/// Destructor
CvProcessProductionAI::~CvProcessProductionAI(void)
{
}

/// Clear out AI local variables
void CvProcessProductionAI::Reset()
{
	m_ProcessAIWeights.clear();

	// Loop through reading each one and add an entry with 0 weight to our vector
	for (int i = 0; i < GC.getNumProcessInfos(); i++)
	{
		m_ProcessAIWeights.push_back(i, 0);
	}
}

/// Serialization read
void CvProcessProductionAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	int iWeight;

	// Reset vector
	m_ProcessAIWeights.clear();
	m_ProcessAIWeights.resize(GC.getNumProcessInfos());
	for(int i = 0; i < GC.getNumProcessInfos(); ++i)
		m_ProcessAIWeights.SetWeight(i, 0);

	// Loop through reading each one and adding it to our vector
	int iNumProcess;
	kStream >> iNumProcess;
	for(int i = 0; i < iNumProcess; i++)
	{
		int iType = CvInfosSerializationHelper::ReadHashed(kStream);
		kStream >> iWeight;
		if (iType >= 0 && iType < m_ProcessAIWeights.size())
			m_ProcessAIWeights.SetWeight(iType, iWeight);
	}
}

/// Serialization write
void CvProcessProductionAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	// Loop through writing each entry
	kStream << GC.getNumProcessInfos();
	for(int i = 0; i < GC.getNumProcessInfos(); i++)
	{
		CvInfosSerializationHelper::WriteHashed(kStream, GC.getProcessInfo((ProcessTypes)i));
		kStream << m_ProcessAIWeights.GetWeight(i);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvProcessProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	int iProcess;
	CvProcessInfo* entry(NULL);

	// Loop through all projects
	for(iProcess = 0; iProcess < GC.getNumProcessInfos(); iProcess++)
	{
		entry = GC.getProcessInfo((ProcessTypes)iProcess);
		if (entry)
		{
			// Set its weight by looking at project's weight for this flavor and using iWeight multiplier passed in
			m_ProcessAIWeights.IncreaseWeight(iProcess, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}

/// Retrieve sum of weights on one item
int CvProcessProductionAI::GetWeight(ProcessTypes eProject)
{
	return m_ProcessAIWeights.GetWeight(eProject);
}
#if defined(MOD_BALANCE_CORE)
int CvProcessProductionAI::CheckProcessBuildSanity(ProcessTypes eProcess, int iTempWeight)
{
	CvProcessInfo* pProcess = GC.getProcessInfo(eProcess);
	if(!pProcess)
	{
		return 0;
	}

	if(iTempWeight == 0)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if(kPlayer.isMinorCiv())
	{
		return 0;
	}
	if(!kPlayer.GetPlayerTraits()->IsNoAnnexing() && m_pCity->IsPuppet())
	{
		return 0;
	}

	MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	// scale based on flavor and world size
	if(eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
	{
		return 0;
	}
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	EconomicAIStrategyTypes eStrategyCultureGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CULTURE");
	AICityStrategyTypes eNeedFood = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD");
	AICityStrategyTypes eNeedFoodNaval = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_GROWTH");
	EconomicAIStrategyTypes eGrowCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GROW_LIKE_CRAZY");
	AICityStrategyTypes eScienceCap = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_KEY_SCIENCE_CITY");
	AICityStrategyTypes eGoodGP = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_GOOD_GP_CITY");

	//Help Venice use processes more rationally.
	if(kPlayer.GetPlayerTraits()->IsNoAnnexing() && m_pCity->IsPuppet())
	{
		CvProcessInfo* pProcess = GC.getProcessInfo((ProcessTypes)eProcess);
		for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = (YieldTypes)iYield;
			if(pProcess->getProductionToYieldModifier(eYield) > 0)
			{
				iTempWeight = m_pCity->GetCityStrategyAI()->GetProcessProductionAI()->GetWeight(eProcess);
				iTempWeight /= 10;
			}
		}
	}

	//Yield value.
	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if(eYield == NO_YIELD)
			continue;

		if(m_pCity->GetCityStrategyAI()->GetDeficientYield() == eYield)
		{
			iTempWeight *= 2;
		}
		if(pProcess->getProductionToYieldModifier(eYield) > 0)
		{
			switch(eYield)
			{
				case YIELD_GOLD:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromGold() > 0)
						{
							iTempWeight *= (m_pCity->getUnhappinessFromGold() * 2);
						}
					}
					if(eStrategyLosingMoney != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
					{
						iTempWeight *= 5;
					}
				}
				break;
				case YIELD_CULTURE:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromCulture() > 0)
						{
							iTempWeight *= (m_pCity->getUnhappinessFromCulture() * 2);
						}
					}
					if(eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
					{
						iTempWeight *= 2;
					}
					if(eGoodGP != NO_AICITYSTRATEGY &&  m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eGoodGP))
					{
						iTempWeight *= 2;
					}
				}
				break;
				case YIELD_SCIENCE:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromScience() > 0)
						{
							iTempWeight *= (m_pCity->getUnhappinessFromScience() * 2);
						}
					}
					if(eScienceCap != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eScienceCap))
					{
						iTempWeight *= 3;
					}
				}
				break;
				case YIELD_FOOD:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromStarving() > 0)
						{
							iTempWeight *= (m_pCity->getUnhappinessFromStarving() * 2);
						}
					}
					if(eGrowCrazy != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eGrowCrazy))
					{
						iTempWeight *= 2;
					}
					if(eNeedFood != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedFood))
					{
						iTempWeight *= 2;
					}
					if(eNeedFoodNaval != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedFoodNaval))
					{
						iTempWeight *= 2;
					}
				}
				break;
			}
		}
	}
	for(int iI = 0; iI < GC.getNumLeagueProjectInfos(); iI++)
	{
		LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) iI;
		CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eLeagueProject);
		if (pInfo && pInfo->GetProcess() == eProcess)
		{
			if (GC.getGame().GetGameLeagues()->CanContributeToLeagueProject(m_pCity->getOwner(), eLeagueProject))
			{
				FStaticVector<LeagueProjectRewardTypes, 4, true, c_eCiv5GameplayDLL> veRewards;
				veRewards.push_back(pInfo->GetRewardTier3());
				veRewards.push_back(pInfo->GetRewardTier2());
				veRewards.push_back(pInfo->GetRewardTier1());
			
				for (uint i = 0; i < veRewards.size(); i++)
				{
					CvLeagueProjectRewardEntry* pRewardInfo = GC.getLeagueProjectRewardInfo(veRewards[i]);
					CvAssert(pRewardInfo);
					if (!pRewardInfo) continue;

					// Free Building in Capital
					if (pRewardInfo->GetBuilding() != NO_BUILDING)
					{
						CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(pRewardInfo->GetBuilding());
						if(pBuildingInfo && kPlayer.GetWonderProductionAI()->IsWonder(*pBuildingInfo))
						{
							iTempWeight += kPlayer.GetWonderProductionAI()->GetWeight(pRewardInfo->GetBuilding());
						}
						else
						{
							iTempWeight += m_pCity->GetCityStrategyAI()->GetBuildingProductionAI()->GetWeight(pRewardInfo->GetBuilding());
						}
					}

					// Happiness
					if (pRewardInfo->GetHappiness() != 0)
					{
						iTempWeight += pRewardInfo->GetHappiness() * (20 - kPlayer.GetHappiness());
					}

					// Free Social Policy
					if (pRewardInfo->GetFreeSocialPolicies() > 0)
					{
						iTempWeight *= kPlayer.GetPlayerPolicies()->GetNumPoliciesOwned();
					}

					EconomicAIStrategyTypes eStrategyCultureGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CULTURE");
					// Temporary Culture Modifier
					if (pRewardInfo->GetCultureBonusTurns() > 0)
					{			
						if(eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
						{
							iTempWeight *= 10;
						}
					}

					// Temporary Tourism Modifier
					if (pRewardInfo->GetTourismBonusTurns() > 0)
					{
						if(eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
						{
							iTempWeight *= 10;
						}
					}

					// Golden Age Points
					if (pRewardInfo->GetGoldenAgePoints() > 0)
					{
						if(kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier() > 0)
						{
							iTempWeight *= kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier();
						}
						else
						{
							iTempWeight *= kPlayer.getGoldenAgeModifier();
						}
						if(kPlayer.isGoldenAge())
						{
							iTempWeight /= 5;
						}
					}

					// City-State Influence Boost
					//antonjs: todo: ordering, to prevent ally / no longer ally notif spam
					EconomicAIStrategyTypes eStrategyUNGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_DIPLOMACY");
					if (pRewardInfo->GetCityStateInfluenceBoost() > 0)
					{		
						if(eStrategyUNGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyUNGS))
						{
							iTempWeight *= 10;
						}
					}
					EconomicAIStrategyTypes eStrategySpaceShip = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP");
					// Beaker boost based on previous turns
					if (pRewardInfo->GetBaseBeakersTurnsToCount() > 0)
					{
						if(eStrategySpaceShip != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategySpaceShip))
						{
							iTempWeight *= 10;
						}
					}
					EconomicAIStrategyTypes eStrategyConquest = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CONQUEST");
					
					// Free unit class
					if (pRewardInfo->GetFreeUnitClass() != NO_UNITCLASS)
					{
						UnitTypes eUnit = (UnitTypes) kPlayer.getCivilizationInfo().getCivilizationUnits(pRewardInfo->GetFreeUnitClass());
						if (eUnit != NO_UNIT)
						{
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
							if(pkUnitInfo)
							{
								if(pkUnitInfo->GetCombat() > 0)
								{
									if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
									{
										iTempWeight *= 10;
									}
								}
							}
						}
					}
		
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
					if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
					{
						//CSD Project Rewards
						if (pRewardInfo->GetAttackBonusTurns() > 0)
						{
							if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
							{
								iTempWeight *= 15;
							}
						}
						if (pRewardInfo->GetBaseFreeUnits() > 0)
						{
							if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
							{
								iTempWeight *= 15;
							}
						}
						// Temporary Culture Modifier
						if (pRewardInfo->GetNumFreeGreatPeople() > 0)
						{
							iTempWeight *= 25;
						}
					}
#endif
				}
			}
		}
	}
	if(kPlayer.IsAtWarAnyMajor())
	{
		iTempWeight /= 5;
	}
	return iTempWeight;
}
#endif

/// Log all potential builds
void CvProcessProductionAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;
		CvString strLogName;

		CvAssert(m_pCity);
		if(!m_pCity) return;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);
		CvAssert(pLog);
		if(!pLog) return;

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvProcessInfo* pProcessInfo = GC.getProcessInfo((ProcessTypes)m_Buildables.GetElement(iI));
			strDesc = (pProcessInfo != NULL)? pProcessInfo->GetDescription() : "Unknown";
			strTemp.Format("Process, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}


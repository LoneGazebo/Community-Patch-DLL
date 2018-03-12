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
int CvProcessProductionAI::CheckProcessBuildSanity(ProcessTypes eProcess, int iTempWeight, int iNumBuildables, int iGPT)
{
	CvProcessInfo* pProcess = GC.getProcessInfo(eProcess);
	if(!pProcess)
	{
		return 0;
	}

	if(iTempWeight == 0)
		return 0;

	if(iNumBuildables > 0)
	{
		if(iTempWeight > 400)
		{
			iTempWeight = 400;
		}
	}
	else
	{
		if(iTempWeight > 1000)
		{
			iTempWeight = 1000;
		}
	}

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if (kPlayer.isMinorCiv())
		return iTempWeight/2; // buildings POST process is not applied for Minors, so they often fall below 400 treshold! also, process is not weighted bu turns as it is considered 1 turn always

	int iModifier = 0;

	//////
	//WAR
	///////
	if (pProcess->getDefenseValue() == 0)
	{
		//Fewer processes while at war.
		if (!m_pCity->IsPuppet())
		{
			int iNumWar = kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false);
			if (iNumWar > 0)
			{
				iModifier -= (iNumWar * 15);
				if (kPlayer.getNumCities() > 1 && m_pCity->GetThreatRank() != -1)
				{
					//More cities = more threat.
					int iThreat = (kPlayer.getNumCities() - m_pCity->GetThreatRank()) * 25;
					if (iThreat > 0)
					{
						iModifier -= iThreat;
					}
				}
				if (m_pCity->IsBastion())
				{
					iModifier -= 100;
				}
				if (m_pCity->isCoastal() && m_pCity->IsBlockaded(true))
				{
					iModifier -= 100;
				}
				if (m_pCity->IsBlockadedWaterAndLand())
				{
					iModifier -= 100;
				}
			}
		}
		//Tiny army? Eek!
		if (kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 2))
		{
			iModifier -= 100;
		}

		MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
		// scale based on flavor and world size
		if (eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
		{
			iModifier -= 50;
		}
	}
	else
	{
		//Unless it is defense.
		if (!m_pCity->IsPuppet())
		{
			int iNumWar = kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false);
			if (iNumWar > 0)
			{
				if (m_pCity->isUnderSiege())
				{
					iModifier += 500;
				}
				else if (m_pCity->getDamage() > 0)
				{
					iModifier += m_pCity->getDamage() * 50;
				}
				else if (m_pCity->isInDangerOfFalling())
				{
					iModifier += 1000;
				}
				else if (m_pCity->isCoastal() && m_pCity->IsBlockaded(true))
				{
					iModifier += 150;
				}
				else if (m_pCity->IsBlockadedWaterAndLand())
				{
					iModifier += 150;
				}
				//None of these things?
				else
					return 0;
			}
			else
				return 0;
		}
		//Tiny army? Eek!
		if (kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 2))
		{
			iModifier -= 1000;
		}

		MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
		// scale based on flavor and world size
		if (eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
		{
			iModifier -= 1000;
		}

		if (m_pCity->getUnhappinessFromDefense() > 0)
		{
			iModifier += (m_pCity->getUnhappinessFromDefense() * 5);
		}

		iModifier *= (pProcess->getDefenseValue() + 100);
		iModifier /= 100;

		if (kPlayer.isMinorCiv())
			iModifier /= 5;
	}
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	EconomicAIStrategyTypes eStrategyCultureGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CULTURE");
	AICityStrategyTypes eNeedFood = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD");
	AICityStrategyTypes eNeedFoodNaval = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_GROWTH");
	EconomicAIStrategyTypes eGrowCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GROW_LIKE_CRAZY");
	AICityStrategyTypes eScienceCap = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_KEY_SCIENCE_CITY");

	//Yield value.
	
	//Base value of production.
	iModifier += (m_pCity->getYieldRate(YIELD_PRODUCTION, false) / 5);
	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if(eYield == NO_YIELD)
			continue;

		if(pProcess->getProductionToYieldModifier(eYield) > 0)
		{
			if (m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == eYield)
			{
				iModifier += 150;
			}
			switch(eYield)
			{
				case YIELD_GOLD:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromGold() > 0)
						{
							iModifier += (m_pCity->getUnhappinessFromGold() * 3);
						}
					}
					if(eStrategyLosingMoney != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
					{
						iModifier += 30;
					}
					if (kPlayer.GetTreasury()->GetGold() <= 0 && kPlayer.GetTreasury()->AverageIncome(5) <= 0)
						iModifier += 500;
				}
				break;
				case YIELD_CULTURE:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromCulture() > 0)
						{
							iModifier += (m_pCity->getUnhappinessFromCulture() * 3);
						}
					}
					if(eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
					{
						iModifier += 30;
					}
				}
				break;
				case YIELD_SCIENCE:
				{
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromScience() > 0)
						{
							iModifier += (m_pCity->getUnhappinessFromScience() * 3);
						}
					}
					if(eScienceCap != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eScienceCap))
					{
						iModifier += 30;
					}
				}
				break;
				case YIELD_FOOD:
				{
					if(m_pCity->GetCityCitizens()->IsForcedAvoidGrowth())
						return 0;

					int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
					if (iExcessFoodTimes100 < 0)
					{
						iModifier += 30;
					}
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						if(m_pCity->getUnhappinessFromStarving() > 0)
						{
							iModifier += (m_pCity->getUnhappinessFromStarving() * 10);
						}
					}
					if(eGrowCrazy != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eGrowCrazy))
					{
						iModifier += 20;
					}
					if(eNeedFood != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedFood))
					{
						iModifier += 20;
					}
					if(eNeedFoodNaval != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedFoodNaval))
					{
						iModifier += 20;
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
			if (m_pCity->getProductionProcess() == eProcess)
			{
				iModifier += 10000;
			}
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
						if(pBuildingInfo)
						{
							int iValue = 2000;
							if(kPlayer.getCapitalCity() != NULL)
							{
								iValue = kPlayer.getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(pRewardInfo->GetBuilding(), iValue, 5, 5, 1);
								iModifier += iValue;
							}
							else
							{
								iModifier += m_pCity->GetCityStrategyAI()->GetBuildingProductionAI()->GetWeight(pRewardInfo->GetBuilding());
							}
						}
					}

					// Happiness
					if (pRewardInfo->GetHappiness() != 0)
					{
						iModifier += pRewardInfo->GetHappiness() * (100 - kPlayer.GetHappiness());
					}

					// Free Social Policy
					if (pRewardInfo->GetFreeSocialPolicies() > 0)
					{
						iModifier += (kPlayer.GetPlayerPolicies()->GetNumPoliciesOwned() * 50);
					}

					EconomicAIStrategyTypes eStrategyCultureGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CULTURE");
					// Temporary Culture Modifier
					if (pRewardInfo->GetCultureBonusTurns() > 0)
					{			
						if(eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
						{
							iModifier += 1500;
						}
					}

					// Temporary Tourism Modifier
					if (pRewardInfo->GetTourismBonusTurns() > 0)
					{
						if(eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
						{
							iModifier += 1500;
						}
					}

					// Golden Age Points
					if (pRewardInfo->GetGoldenAgePoints() > 0)
					{
						if(kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier() > 0)
						{
							iModifier += (pRewardInfo->GetGoldenAgePoints() + kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier()) * 25;
						}
						else
						{
							iModifier += (pRewardInfo->GetGoldenAgePoints() + kPlayer.getGoldenAgeModifier()) * 25;
						}
					}

					// City-State Influence Boost
					//antonjs: todo: ordering, to prevent ally / no longer ally notif spam
					EconomicAIStrategyTypes eStrategyUNGS = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_DIPLOMACY");
					if (pRewardInfo->GetCityStateInfluenceBoost() > 0)
					{		
						if(eStrategyUNGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyUNGS))
						{
							iModifier += 1000;
						}
						else
						{
							iModifier += 500;
						}
					}
					EconomicAIStrategyTypes eStrategySpaceShip = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_SPACESHIP");
					// Beaker boost based on previous turns
					if (pRewardInfo->GetBaseBeakersTurnsToCount() > 0)
					{
						if(eStrategySpaceShip != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategySpaceShip))
						{
							iModifier += 1000;
						}
						else
						{ 
							iModifier += 500;
						}
					}
					
					// Free unit class
					if (pRewardInfo->GetFreeUnitClass() != NO_UNITCLASS)
					{
						UnitTypes eUnit = (UnitTypes) kPlayer.getCivilizationInfo().getCivilizationUnits(pRewardInfo->GetFreeUnitClass());
						if (eUnit != NO_UNIT)
						{
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
							if(pkUnitInfo)
							{
								int iValue = 1500;
								if(kPlayer.getCapitalCity() != NULL)
								{
									iValue = kPlayer.getCapitalCity()->GetCityStrategyAI()->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, NULL, iValue, 1);
								}
								iModifier += iValue;
							}
						}
					}
					EconomicAIStrategyTypes eStrategyConquest = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CONQUEST");
		
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
					if (MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS) 
					{
						//CSD Project Rewards
						if (pRewardInfo->GetAttackBonusTurns() > 0)
						{
							if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
							{
								iModifier += 300;
							}
							else
							{
								iModifier += 150;
							}
						}
						if (pRewardInfo->GetBaseFreeUnits() > 0)
						{
							if(eStrategyConquest != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyConquest))
							{
								iModifier += 300;
							}
							else
							{
								iModifier += 150;
							}
						}
						// Temporary Culture Modifier
						if (pRewardInfo->GetNumFreeGreatPeople() > 0)
						{
							iModifier += 500;
						}
					}
#endif
				}
			}
		}
	}
	if(iGPT <= 0)
	{
		iModifier += (iGPT *= -1);
	}

	if(m_pCity->IsPuppet())
	{
		iTempWeight *= (75 + iModifier);
		iTempWeight /= 100;
	}
	else
	{
		iTempWeight *= (100 + iModifier);
		iTempWeight /= 100;
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


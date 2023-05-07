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

///
template<typename ProcessProductionAI, typename Visitor>
void CvProcessProductionAI::Serialize(ProcessProductionAI& processProductionAI, Visitor& visitor)
{
	visitor(processProductionAI.m_ProcessAIWeights);
}

/// Serialization read
void CvProcessProductionAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvProcessProductionAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& loadFrom, CvProcessProductionAI& writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
FDataStream& operator<<(FDataStream& saveTo, const CvProcessProductionAI& readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvProcessProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	int iProcess = 0;
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
		return SR_IMPOSSIBLE;

	if(iTempWeight < 1)
		return SR_IMPOSSIBLE;

	//this seems to work well to bring the raw flavor weight into a sensible range [0 ... 200]
	iTempWeight = sqrti(10 * iTempWeight);

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());
	vector<PlayerTypes> v;

	//barbarians can only farm
	if (kPlayer.isBarbarian() && pProcess->getProductionToYieldModifier(YIELD_FOOD) == 0)
		return SR_IMPOSSIBLE;

	if (kPlayer.isMinorCiv())
		return iTempWeight/2; // buildings POST process is not applied for Minors, so they often fall below 400 threshold! also, process is not weighted bu turns as it is considered 1 turn always

	int iModifier = 0;

	//Tiny army? Eek, better build units
	if (kPlayer.getNumMilitaryUnits() <= (kPlayer.getNumCities() * 2) || (!m_pCity->HasGarrison() && m_pCity->isBorderCity(v)))
	{
		iModifier -= 100;
	}

	//////
	//WAR
	///////
	if (pProcess->getDefenseValue() == 0)
	{
		//Fewer processes while at war.
		if (!CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
		{
			int iNumWar = kPlayer.GetMilitaryAI()->GetNumberCivsAtWarWith(false);
			if (iNumWar > 0)
			{
				iModifier -= (iNumWar * 5);
				iModifier -= m_pCity->getThreatValue();

				if (m_pCity->isBorderCity(v))
				{
					iModifier -= 25;
				}
				if (m_pCity->isUnderSiege())
				{
					iModifier -= 25;
				}
			}
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
		if (!CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
		{
			//don't need this if no damage
			if (m_pCity->getDamage() == 0)
				return SR_USELESS;

			if (m_pCity->isInDangerOfFalling())
			{
				iModifier += 1000;
				iModifier += m_pCity->getDamage();
			}
			else if (m_pCity->isUnderSiege())
			{
				iModifier += 500;
				iModifier += m_pCity->getDamage();
			}
		}

		MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
		// scale based on flavor and world size
		if (eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
		{
			iModifier -= 100;
		}

		if (m_pCity->GetDistress(false) > 0)
		{
			iModifier += (m_pCity->GetDistress(false) * 5);
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
	AICityStrategyTypes eScienceCap = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_KEY_SCIENCE_CITY");

	//Yield value.
	
	//Base value of production.
	int iProduction = m_pCity->getYieldRate(YIELD_PRODUCTION, false);
	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if(eYield == NO_YIELD)
			continue;

		if(pProcess->getProductionToYieldModifier(eYield) > 0)
		{
			if (m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == eYield)
				iModifier += 150;

			int iConvertedYield = (iProduction * m_pCity->GetYieldFromProcessModifier(eYield)) / 100;

			iModifier += iConvertedYield;

			switch(eYield)
			{
				case NO_YIELD:
				UNREACHABLE();
				case YIELD_GOLD:
				{
					if (MOD_BALANCE_VP)
					{
						int iPoverty = m_pCity->GetPoverty(false);
						if (iPoverty > 0)
						{
							iModifier += iPoverty * 3;
						}
					}
					if (eStrategyLosingMoney != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney))
					{
						iModifier += 50;
					}
				}
				break;
				case YIELD_CULTURE:
				{
					if (MOD_BALANCE_VP)
					{
						int iBoredom = m_pCity->GetBoredom(false);
						if (iBoredom > 0)
						{
							iModifier += iBoredom * 3;
						}
					}
					if (eStrategyCultureGS != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyCultureGS))
					{
						iModifier += 30;
					}
				}
				break;
				case YIELD_SCIENCE:
				{
					if (MOD_BALANCE_VP)
					{
						int iIlliteracy = m_pCity->GetIlliteracy(false);
						if (iIlliteracy > 0)
						{
							iModifier += iIlliteracy * 3;
						}
					}
					if (eScienceCap != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eScienceCap))
					{
						iModifier += 30;
					}
				}
				break;
				case YIELD_FOOD:
				{
					if (m_pCity->GetCityCitizens()->IsForcedAvoidGrowth())
						return 0;

					int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
					if (iExcessFoodTimes100 < 0)
					{
						iModifier += 30;
					}
					if (MOD_BALANCE_VP)
					{
						int iFamine = m_pCity->GetUnhappinessFromFamine();
						if (iFamine > 0)
						{
							iModifier += iFamine * 10;
						}
					}
					if (eNeedFood != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedFood))
					{
						iModifier += 20;
					}
					if (eNeedFoodNaval != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedFoodNaval))
					{
						iModifier += 20;
					}
				}
				break;
				default:
				// No default processes exist for other yield types.
				// Not unreachable though because a mod could add more.
				// 
				// TODO: AI should probably still have some behavior for
				// other processes even if they aren't available by default
				break; 
			}
		}
	}

	bool bIsProject = false;
	for(int iI = 0; iI < GC.getNumLeagueProjectInfos(); iI++)
	{
		LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) iI;
		CvLeagueProjectEntry* pInfo = GC.getLeagueProjectInfo(eLeagueProject);
		if (pInfo && pInfo->GetProcess() == eProcess)
		{
			bIsProject = true;
			if (m_pCity->getProductionProcess() == eProcess)
			{
				iModifier += 10000;
			}
			if (GC.getGame().GetGameLeagues()->CanContributeToLeagueProject(m_pCity->getOwner(), eLeagueProject))
			{
				vector<LeagueProjectRewardTypes> veRewards;
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
								iValue = kPlayer.getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(pRewardInfo->GetBuilding(), iValue);
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
						iModifier += (pRewardInfo->GetGoldenAgePoints() + kPlayer.getGoldenAgeModifier(false)) * 25;
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
						UnitTypes eUnit = kPlayer.GetSpecificUnitType(pRewardInfo->GetFreeUnitClass());
						if (eUnit != NO_UNIT)
						{
							CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
							if(pkUnitInfo)
							{
								int iValue = 1500;
								if(kPlayer.getCapitalCity() != NULL)
								{
									iValue = kPlayer.getCapitalCity()->GetCityStrategyAI()->GetUnitProductionAI()->CheckUnitBuildSanity(eUnit, false, iValue);
								}
								iModifier += iValue;
							}
						}
					}
					EconomicAIStrategyTypes eStrategyConquest = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_GS_CONQUEST");

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
			}
		}
	}

	if (bIsProject)
	{
		iTempWeight +=iModifier;
	}
	else
	{
		iTempWeight *= (100 + iModifier);
		iTempWeight /= 100;
	}

	return max(1,iTempWeight);
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
		FILogFile* pLog = NULL;
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


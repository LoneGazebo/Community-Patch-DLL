/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvWonderProductionAI.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvCitySpecializationAI.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvInfosSerializationHelper.h"
// include this after all other headers
#include "LintFree.h"


/// Constructor
CvWonderProductionAI::CvWonderProductionAI(CvPlayer* pPlayer, CvBuildingXMLEntries* pBuildings):
	m_pPlayer(pPlayer),
	m_pBuildings(pBuildings)
{
}

/// Destructor
CvWonderProductionAI::~CvWonderProductionAI(void)
{
}

/// Initialize
void CvWonderProductionAI::Init(CvBuildingXMLEntries* pBuildings, CvPlayer* pPlayer, bool bIsCity)
{
	// Init base class
	CvFlavorRecipient::Init();

	// Store off the pointer to the buildings for this game
	m_pBuildings = pBuildings;
	m_pPlayer = pPlayer;
	m_bIsCity = bIsCity;

	Reset();
}

/// Clear out AI local variables
void CvWonderProductionAI::Reset()
{
	CvAssertMsg(m_pBuildings != NULL, "Wonder Production AI init failure: building entries are NULL");

	// Reset vector
	m_WonderAIWeights.clear();

	// Loop through reading each one and adding it to our vector
	if(m_pBuildings)
	{
		for(int i = 0; i < m_pBuildings->GetNumBuildings(); i++)
		{
			m_WonderAIWeights.push_back(i, 0);
		}
	}
}

template<typename WonderProductionAI, typename Visitor>
void CvWonderProductionAI::Serialize(WonderProductionAI& wonderProductionAI, Visitor& visitor)
{
	visitor(wonderProductionAI.m_WonderAIWeights);
}

/// Serialization read
void CvWonderProductionAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvWonderProductionAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvWonderProductionAI& wonderProductionAI)
{
	wonderProductionAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvWonderProductionAI& wonderProductionAI)
{
	wonderProductionAI.Write(stream);
	return stream;
}

/// Respond to a new set of flavor values
void CvWonderProductionAI::FlavorUpdate()
{
	// Broadcast to our sub AI objects
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes)iFlavor);
		AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
	}
}

/// Establish weights for one flavor; can be called multiple times to layer strategies
void CvWonderProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{
	if (iWeight==0)
		return;

	// Loop through all buildings (even though we're only go to do anything on wonders)
	for(int iBldg = 0; iBldg < m_pBuildings->GetNumBuildings(); iBldg++)
	{
		CvBuildingEntry* entry = m_pBuildings->GetEntry(iBldg);
		if(entry)
		{
			CvBuildingEntry& kBuilding = *entry;
			if(IsWonder(kBuilding))
			{
				// Set its weight by looking at wonder's weight for this flavor and using iWeight multiplier passed in
				m_WonderAIWeights.IncreaseWeight(iBldg, kBuilding.GetFlavorValue(eFlavor) * iWeight);
			}
		}
	}
}

/// Retrieve sum of weights on one item
int CvWonderProductionAI::GetWeight(BuildingTypes eBldg)
{
	return m_WonderAIWeights.GetWeight(eBldg);
}

/// Recommend highest-weighted wonder, also return total weight of all buildable wonders
BuildingTypes CvWonderProductionAI::ChooseWonder(bool /* bAdjustForOtherPlayers */, int& iWonderWeight)
{
	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);

	// Reset list of all the possible wonders
	m_Buildables.clear();

	// Guess which city will be producing this (doesn't matter that much since weights are all relative)
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		int iEstimatedProductionPerTurn = pLoopCity->getCurrentProductionDifference(true, false);
		if (iEstimatedProductionPerTurn < 1)
			iEstimatedProductionPerTurn = 1;

		// Loop through adding the available wonders
		for (int iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBldgLoop);
			CvBuildingEntry* pkBuildingInfo = m_pBuildings->GetEntry(eBuilding);
			if (!pkBuildingInfo)
				continue;

			if (!pLoopCity->IsBestForWonder(pkBuildingInfo->GetBuildingClassType()))
				continue;

			int iTurnsRequired = std::max(1, pkBuildingInfo->GetProductionCost() / iEstimatedProductionPerTurn);

			// if we are forced to restart a wonder, give one that has been started already a huge bump
			bool bAlreadyStarted = pLoopCity->GetCityBuildings()->GetBuildingProduction(eBuilding) > 0;
			int iTempWeight = bAlreadyStarted ? m_WonderAIWeights.GetWeight(iBldgLoop) * 25 : m_WonderAIWeights.GetWeight(iBldgLoop);

			int iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(iTempWeight, iTurnsRequired);
			if (iWeight <= 0)
				continue;

			iWeight = pLoopCity->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuilding, iWeight, false, false, true);
			if (iWeight > 0)
				m_Buildables.push_back(iBldgLoop, iWeight);
		}
	}

	// Sort items and grab the first one
	if (m_Buildables.size() > 0)
	{
		m_Buildables.StableSortItems();
		LogPossibleWonders();

		if (m_Buildables.GetTotalWeight() > 0)
		{
			iWonderWeight = m_Buildables.GetTotalWeight();
			BuildingTypes eSelection = (BuildingTypes)m_Buildables.ChooseAbovePercentThreshold(GC.getGame().getHandicapInfo().getCityProductionChoiceCutoffThreshold(), &fcn, "Choosing wonder from Top Choices");
			return eSelection;
		}
		// Nothing with any weight
		else
		{
			return NO_BUILDING;
		}
	}

	// Unless we didn't find any
	return NO_BUILDING;
}

/// Log all potential builds
void CvWonderProductionAI::LogPossibleWonders()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ
		CvString playerName = m_pPlayer->getCivilizationShortDescription();

		// Get the leading info for this line
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvString strOutBuf = strBaseString;

			CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(m_Buildables.GetElement(iI));
			if(pEntry != NULL)
			{
				CvString strDesc = pEntry->GetDescription();
				CvString strTemp;
				strTemp.Format("Possible Wonder, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;

			}

			m_pPlayer->GetCitySpecializationAI()->LogMsg(strOutBuf);
		}
	}
}

/// Stub - Probably don't need to log flavors to city specialization log -- is in enough places already
void CvWonderProductionAI::LogFlavors(FlavorTypes)
{
}

/// Check to make sure this is one of the buildings we consider to be a wonder
bool CvWonderProductionAI::IsWonder(const CvBuildingEntry& kBuilding) 
{
	const CvBuildingClassInfo& kBuildingClass = kBuilding.GetBuildingClassInfo();

	if(::isWorldWonderClass(kBuildingClass) ||
	        ::isTeamWonderClass(kBuildingClass) ||
	        ::isNationalWonderClass(kBuildingClass))
	{
		return true;
	}
	return false;
}

/// Check wonders excluding national wonders you can only have one of.
bool CvWonderProductionAI::IsWonderNotNationalUnique(const CvBuildingEntry& kBuilding) 
{
	const CvBuildingClassInfo& kBuildingClass = kBuilding.GetBuildingClassInfo();

	bool isNationalUnique = kBuildingClass.getMaxPlayerInstances() == 1;

	if((::isWorldWonderClass(kBuildingClass) || ::isTeamWonderClass(kBuildingClass) || ::isNationalWonderClass(kBuildingClass)) && !isNationalUnique)
	{
		return true;
	}
	return false;
}

// PRIVATE METHODS

/// Check to make sure some city can build this wonder
bool CvWonderProductionAI::HaveCityToBuild(BuildingTypes eBuilding) const
{
	std::vector<int> allBuildingCount = m_pPlayer->GetTotalBuildingCount();

	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->canConstruct(eBuilding,allBuildingCount))
		{
			return true;
		}
	}
	return false;
}
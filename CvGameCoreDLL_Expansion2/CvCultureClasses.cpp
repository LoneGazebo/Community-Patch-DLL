/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvInternalGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvGameTextMgr.h"
#include "CvEconomicAI.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvTypes.h"
#include "CvMilitaryAI.h"

#include <utility>

#include "LintFree.h"

//=====================================
// CvGreatWork
//=====================================
/// Default Constructor
CvGreatWork::CvGreatWork()
	: m_eType(NO_GREAT_WORK)
	, m_eClassType(NO_GREAT_WORK_CLASS)
	, m_iTurnFounded(-1)
	, m_eEra(NO_ERA)
	, m_ePlayer(NO_PLAYER)
{
}

/// Constructor
CvGreatWork::CvGreatWork(const CvString& szGreatPersonName, GreatWorkType eType, GreatWorkClass eClassType, int iTurn, EraTypes eEra, PlayerTypes ePlayer)
	: m_szGreatPersonName(szGreatPersonName)
	, m_eType(eType)
	, m_eClassType(eClassType)
	, m_iTurnFounded(iTurn)
	, m_eEra(eEra)
	, m_ePlayer(ePlayer)
{
	m_iTurnFounded = GC.getGame().getGameTurn();
}

template<typename GreatWork, typename Visitor>
void CvGreatWork::Serialize(GreatWork& greatWork, Visitor& visitor)
{
	visitor(greatWork.m_szGreatPersonName);
	visitor(greatWork.m_eType);
	visitor(greatWork.m_eClassType);
	visitor(greatWork.m_iTurnFounded);
	visitor(greatWork.m_eEra);
	visitor(greatWork.m_ePlayer);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGreatWork& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGreatWork::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGreatWork& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGreatWork::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvGameCulture
//=====================================
/// Constructor
CvGameCulture::CvGameCulture(void)
{
	m_CurrentGreatWorks.clear();

	m_bReportedSomeoneInfluential = false;
}

/// Destructor
CvGameCulture::~CvGameCulture(void)
{
}

/// Run the turn culture computations for all the players
void CvGameCulture::DoTurn()
{
	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)uiPlayer;
		CvPlayer &kPlayer = GET_PLAYER(ePlayer);
		if (kPlayer.isAlive())
		{
			kPlayer.GetCulture()->DoTurn();
		}
	}
}

// Factory method to create a Great Work, returns index of the new work
int CvGameCulture::CreateGreatWork(GreatWorkType eType, GreatWorkClass eClass, PlayerTypes ePlayer, EraTypes eEra, const CvString& szCreator)
{
	CvGreatWork newGreatWork;
	newGreatWork.m_eType = eType;
	newGreatWork.m_eClassType = eClass;
	newGreatWork.m_eEra = eEra;
	if (ePlayer == NO_PLAYER)
		ePlayer = BARBARIAN_PLAYER;
	newGreatWork.m_ePlayer = ePlayer;
	newGreatWork.m_iTurnFounded = GC.getGame().getGameTurn();
	newGreatWork.m_szGreatPersonName = szCreator;

	int iNumGreatWorks = m_CurrentGreatWorks.size();

	m_CurrentGreatWorks.push_back(newGreatWork);

	return iNumGreatWorks;
}

GreatWorkType CvGameCulture::GetGreatWorkType(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork* pWork = &m_CurrentGreatWorks[iIndex];
	return pWork->m_eType;
}

GreatWorkClass CvGameCulture::GetGreatWorkClass(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork* pWork = &m_CurrentGreatWorks[iIndex];
	return pWork->m_eClassType;
}

/// Returns UI tooltip for this Great Work
CvString CvGameCulture::GetGreatWorkTooltip(int iIndex, PlayerTypes eOwner) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szTooltip = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	ASSERT_DEBUG(pWork->m_eClassType != NO_GREAT_WORK_CLASS, "Invalid Great Work Class");

	CvString strYearString;
	CvGameTextMgr::setDateStr(strYearString,
		pWork->m_iTurnFounded,
		false /*bSave*/,
		GC.getGame().getCalendar(),
		GC.getGame().getStartYear(),
		GC.getGame().getGameSpeedType());

	Localization::String strGreatWorkName = Localization::Lookup(CultureHelpers::GetGreatWorkName(pWork->m_eType));

	szTooltip = strGreatWorkName.toUTF8();
	szTooltip += "[NEWLINE]";

	if (strlen(pWork->m_szGreatPersonName) > 0)
	{
		szTooltip += pWork->m_szGreatPersonName;
		szTooltip += "[NEWLINE]";
	}
	szTooltip += GET_PLAYER(pWork->m_ePlayer).getCivilizationShortDescription();
	szTooltip += "[NEWLINE]";
	szTooltip += GC.getEraInfo(pWork->m_eEra)->GetDescription();
	szTooltip += " (";
	szTooltip += strYearString;
	szTooltip += ")";
	szTooltip += "[NEWLINE]";
	CvString cultureString;

	cultureString = "";
	char const* sPrefix = "";
	int iBaseValueTimes100 = 0;
	int iAddedValue = 0;

	CvCity* pCity = GetGreatWorkCity(iIndex);

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++) {
		YieldTypes eYield = (YieldTypes) iYield;
		
		if (eYield == YIELD_CULTURE)
		{
			iBaseValueTimes100 = 100 * /*2 in CP, 3 in VP*/ GD_INT_GET(BASE_CULTURE_PER_GREAT_WORK);
		}
		else if (eYield == YIELD_TOURISM)
		{
			iBaseValueTimes100 = 100 * /*2*/ GD_INT_GET(BASE_TOURISM_PER_GREAT_WORK);
			int iMod = GET_PLAYER(eOwner).GetGreatWorksTourismModifierGlobal();
			if (pCity)
			{
				iMod += pCity->GetCityBuildings()->GetGreatWorksTourismModifier();
			}
			iBaseValueTimes100 *= 100 + iMod;
			iBaseValueTimes100 /= 100;
		}
		else
		{
			iBaseValueTimes100 = 0;
		}

		iAddedValue = GET_PLAYER(eOwner).GetGreatWorkYieldChange(eYield);
		iAddedValue += GET_PLAYER(eOwner).GetPlayerTraits()->GetGreatWorkYieldChanges(eYield);
		if (pCity)
		{
			iAddedValue += pCity->GetGreatWorkYieldChange(eYield);
		}

		GreatWorkClass eWritingClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE");
		GreatWorkClass eArtClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART");
		GreatWorkClass eArtifactsClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");
		GreatWorkClass eMusicClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_MUSIC");
		GreatWorkClass eFilmClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_FILM", true);
		GreatWorkClass eRelicClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_RELIC", true);
		if(pWork->m_eClassType == eWritingClass)
		{
			iAddedValue += GET_PLAYER(eOwner).GetPlayerTraits()->GetLitYieldChanges(eYield);
			iAddedValue += GET_PLAYER(eOwner).getLitYieldBonus(eYield);
		}
		if(pWork->m_eClassType == eArtClass)
		{
			iAddedValue += GET_PLAYER(eOwner).GetPlayerTraits()->GetArtYieldChanges(eYield);
			iAddedValue += GET_PLAYER(eOwner).getArtYieldBonus(eYield);
		}
		if(pWork->m_eClassType == eArtifactsClass)
		{
			iAddedValue += GET_PLAYER(eOwner).GetPlayerTraits()->GetArtifactYieldChanges(eYield);
			iAddedValue += GET_PLAYER(eOwner).getArtifactYieldBonus(eYield);
		}
		if(pWork->m_eClassType == eMusicClass)
		{
			iAddedValue += GET_PLAYER(eOwner).GetPlayerTraits()->GetMusicYieldChanges(eYield);
			iAddedValue += GET_PLAYER(eOwner).getMusicYieldBonus(eYield);
		}
		if (pWork->m_eClassType == eFilmClass)
		{
			iAddedValue += GET_PLAYER(eOwner).getFilmYieldBonus(eYield);
		}
		if (pWork->m_eClassType == eRelicClass)
		{
			iAddedValue += GET_PLAYER(eOwner).getRelicYieldBonus(eYield);
		}

		if (pCity) {
			ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
			BeliefTypes eSecondaryPantheon = NO_BELIEF;
			if(eMajority >= RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
				if(pReligion)
				{
					iAddedValue += pReligion->m_Beliefs.GetGreatWorkYieldChange(pCity->getPopulation(), eYield, pCity->getOwner(), pCity);
					eSecondaryPantheon = pCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
					if (eSecondaryPantheon != NO_BELIEF)
					{
						iAddedValue += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGreatWorkYieldChange(eYield);
					}
				}
			}

			// mod for civs keeping their pantheon belief forever
			if (MOD_RELIGION_PERMANENT_PANTHEON)
			{
				if (GC.getGame().GetGameReligions()->HasCreatedPantheon(eOwner))
				{
					const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, eOwner);
					BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(eOwner);
					if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
					{
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, eOwner);
						if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, eOwner))) // check that the our religion does not have our belief, to prevent double counting
						{
							iAddedValue += GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetGreatWorkYieldChange(eYield);
						}
					}
				}
			}
		}

		int iTotalValueTimes100 = iBaseValueTimes100 + 100 * iAddedValue;
		if (iTotalValueTimes100 != 0) {
			if(iTotalValueTimes100 % 100 == 0)
			{
				cultureString.Format("%s%s+%d %s", cultureString.c_str(), sPrefix, iTotalValueTimes100 / 100, GC.getYieldInfo(eYield)->getIconString());
			}
			else
			{
				float fTotalValue = (float)iTotalValueTimes100 / 100;
				if (iTotalValueTimes100 % 10 == 0)
				{
					cultureString.Format("%s%s+%.1f %s", cultureString.c_str(), sPrefix, fTotalValue, GC.getYieldInfo(eYield)->getIconString());
				}
				else
				{
					cultureString.Format("%s%s+%.2f %s", cultureString.c_str(), sPrefix, fTotalValue, GC.getYieldInfo(eYield)->getIconString());
				}
			}
			sPrefix = ", ";
		}
	}

	szTooltip += cultureString;

	return szTooltip;
}

/// Returns name of this Great Work
CvString CvGameCulture::GetGreatWorkName(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork* pWork = &m_CurrentGreatWorks[iIndex];

	return CultureHelpers::GetGreatWorkName(pWork->m_eType);
}

/// Returns artist of this Great Work
CvString CvGameCulture::GetGreatWorkArtist(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szArtist = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	szArtist = pWork->m_szGreatPersonName;

	return szArtist;
}

/// Returns era of this Great Work
CvString CvGameCulture::GetGreatWorkEra(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szEra = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];

	CvString strYearString;
	CvGameTextMgr::setDateStr(strYearString,
		pWork->m_iTurnFounded,
		false /*bSave*/,
		GC.getGame().getCalendar(),
		GC.getGame().getStartYear(),
		GC.getGame().getGameSpeedType());

	szEra += GC.getEraInfo(pWork->m_eEra)->GetDescription();
	szEra += " (";
	szEra += strYearString;
	szEra += ")";

	return szEra;
}

CvString CvGameCulture::GetGreatWorkEraAbbreviation(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szEra = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	szEra = GC.getEraInfo(pWork->m_eEra)->getAbbreviation();

	return szEra;	
}

CvString CvGameCulture::GetGreatWorkEraShort(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szEra = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	szEra = GC.getEraInfo(pWork->m_eEra)->getShortDesc();

	return szEra;	
}

PlayerTypes CvGameCulture::GetGreatWorkCreator (int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	return pWork->m_ePlayer;
}

PlayerTypes CvGameCulture::GetGreatWorkController(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	
	// for each player
	//   for each building
	//     for each slot
	//       check to see if it holds this work

	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)uiPlayer;

		int iCityLoop = 0;
		for (CvCity* pCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
				BuildingTypes eBuilding = pCity->GetBuildingTypeFromClass(eBuildingClass);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;

				if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						if (iGreatWorkIndex == iIndex)
						{
							return ePlayer;
						}
					}
				}
			}
		}
	}

	return NO_PLAYER;
}

bool CvGameCulture::IsGreatWorkCreated(GreatWorkType eType) const
{
	GreatWorkList::const_iterator it;
	for(it = m_CurrentGreatWorks.begin(); it != m_CurrentGreatWorks.end(); ++it)
	{
		if ((*it).m_eType == eType)
		{
			return true;
		}
	}
	
	return false;
}

CvCity* CvGameCulture::GetGreatWorkCity(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");
	
	// for each player
	//   for each building
	//     for each slot
	//       check to see if it holds this work

	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)uiPlayer;

		int iCityLoop = 0;
		for (CvCity* pCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
				const BuildingTypes eBuilding = pCity->GetBuildingTypeFromClass(eBuildingClass);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;

				if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						if (iGreatWorkIndex == iIndex)
						{
							return pCity;
						}
					}
				}
			}
		}
	}
	
	return NULL;
}

int CvGameCulture::GetGreatWorkCurrentThemingBonus(int iIndex) const
{
	ASSERT_DEBUG(iIndex < GetNumGreatWorks(), "Bad Great Work index");

	// for each player
	//   for each building
	//     for each slot
	//       check to see if it holds this work

	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)uiPlayer;

		int iCityLoop = 0;
		for (CvCity* pCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
				const BuildingTypes eBuilding = pCity->GetBuildingTypeFromClass(eBuildingClass);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;

				if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						if (iGreatWorkIndex == iIndex)
						{
							return pCity->GetCityCulture()->GetThemingBonus(eBuildingClass);
						}
					}
				}
			}
		}
	}

	return 0;
}

/// Swap the great works!
bool CvGameCulture::SwapGreatWorks (PlayerTypes ePlayer1, int iWork1, PlayerTypes ePlayer2, int iWork2)
{
	if (ePlayer1 == NO_PLAYER || ePlayer2 == NO_PLAYER)
	{
		return false;
	}

	if (iWork1 == -1 || iWork2 == -1)
	{
		return false;
	}
	
	if (!GET_PLAYER(ePlayer1).isAlive() || !GET_PLAYER(ePlayer2).isAlive())
	{
		return false;
	}

	if (GET_PLAYER(ePlayer1).IsAtWarWith(ePlayer2))
		return false;


	CvPlayerCulture* pCulture1 = GET_PLAYER(ePlayer1).GetCulture();
	CvPlayerCulture* pCulture2 = GET_PLAYER(ePlayer2).GetCulture();

	GreatWorkClass eWritingClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE");
	GreatWorkClass eArtClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART");
	GreatWorkClass eArtifactsClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");
	GreatWorkClass eMusicClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_MUSIC");

	GreatWorkClass eClass1 = GetGreatWorkClass(iWork1);
	GreatWorkClass eClass2 = GetGreatWorkClass(iWork2);

	if (eClass1 != eClass2)
	{
		return false;
	}
	
	// We can't trade things that aren't in our swappable slots
	bool bFoundSwappable = false;
	if (eClass1 == eWritingClass)
	{
		if (pCulture1->GetSwappableWritingIndex() == iWork1)
		{
			pCulture1->SetSwappableWritingIndex(-1);
			bFoundSwappable = true;
		}
	}
	else if (eClass1 == eArtClass)
	{
		if (pCulture1->GetSwappableArtIndex() == iWork1)
		{
			pCulture1->SetSwappableArtIndex(-1);
			bFoundSwappable = true;
		}
	}
	else if (eClass1 == eArtifactsClass)
	{
		if (pCulture1->GetSwappableArtifactIndex() == iWork1)
		{
			pCulture1->SetSwappableArtifactIndex(-1);
			bFoundSwappable = true;
		}
	}
	else if (eClass1 == eMusicClass)
	{
		if (pCulture1->GetSwappableMusicIndex() == iWork1)
		{
			pCulture1->SetSwappableMusicIndex(-1);
			bFoundSwappable = true;
		}
	}

	if (!bFoundSwappable)
	{
		return false;
	}

	bFoundSwappable = false;
	if (eClass2 == eWritingClass)
	{
		if (pCulture2->GetSwappableWritingIndex() == iWork2)
		{
			pCulture2->SetSwappableWritingIndex(-1);
			bFoundSwappable = true;
		}
	}
	else if (eClass2 == eArtClass)
	{
		if (pCulture2->GetSwappableArtIndex() == iWork2)
		{
			pCulture2->SetSwappableArtIndex(-1);
			bFoundSwappable = true;
		}
	}
	else if (eClass2 == eArtifactsClass)
	{
		if (pCulture2->GetSwappableArtifactIndex() == iWork2)
		{
			pCulture2->SetSwappableArtifactIndex(-1);
			bFoundSwappable = true;
		}
	}
	else if (eClass2 == eMusicClass)
	{
		if (pCulture2->GetSwappableMusicIndex() == iWork2)
		{
			pCulture2->SetSwappableMusicIndex(-1);
			bFoundSwappable = true;
		}
	}

	if (!bFoundSwappable)
	{
		return false;
	}

	CvCity* pCity1 = NULL;
	BuildingClassTypes eBuildingClass1 = NO_BUILDINGCLASS;
	int iSwapIndex1 = -1;

	CvCity* pCity2 = NULL;
	BuildingClassTypes eBuildingClass2 = NO_BUILDINGCLASS;
	int iSwapIndex2 = -1;

	for (uint ui = 0; ui < 2; ui++)
	{
		PlayerTypes eTempPlayer = NO_PLAYER;
		switch (ui)
		{
		case 0:
			eTempPlayer = ePlayer1;
			break;
		case 1:
			eTempPlayer = ePlayer2;
			break;
		}

		int iCityLoop = 0;
		for (CvCity* pCity = GET_PLAYER(eTempPlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(eTempPlayer).nextCity(&iCityLoop))
		{
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
				const BuildingTypes eBuilding = pCity->GetBuildingTypeFromClass(eBuildingClass);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;

				if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						if (ui == 0)
						{
							if (iGreatWorkIndex == iWork1)
							{
								pCity1 = pCity;
								eBuildingClass1 = eBuildingClass;
								iSwapIndex1 = iI;
								break;
							}
						}
						else if (ui == 1)
						{
							if (iGreatWorkIndex == iWork2)
							{
								pCity2 = pCity;
								eBuildingClass2 = eBuildingClass;
								iSwapIndex2 = iI;
								break;
							}
						}
					}
				}
			}
		}
	}

	if (pCity1 == NULL || pCity2 == NULL)
	{
		return false;
	}

	if (eBuildingClass1 == NO_BUILDINGCLASS || eBuildingClass2 == NO_BUILDINGCLASS)
	{
		return false;
	}

	if (iSwapIndex1 == -1 || iSwapIndex2 == -1)
	{
		return false;
	}

	// remove existing great works
	pCity1->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass1, iSwapIndex1, NO_GREAT_WORK);
	pCity2->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass2, iSwapIndex2, NO_GREAT_WORK);

	// add in new works
	pCity1->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass1, iSwapIndex1, iWork2);
	pCity2->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass2, iSwapIndex2, iWork1);
	
	GC.GetEngineUserInterface()->setDirty(GreatWorksScreen_DIRTY_BIT, true);

	pCity1->UpdateAllNonPlotYields(true);
	if (pCity1 != pCity2)
	{
		pCity2->UpdateAllNonPlotYields(true);
	}

	return true;
}

void CvGameCulture::MoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, int iCity2, int iBuildingClass2, int iWorkIndex2)
{
	if(ePlayer == NO_PLAYER)
		return;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (!kPlayer.isAlive())
		return;

	CvCity* pCity1 = kPlayer.getCity(iCity1);
	CvCity* pCity2 = kPlayer.getCity(iCity2);
	if(!pCity1 || !pCity2)
		return;

	int workType1 = pCity1->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClass1, iWorkIndex1);
	int workType2 = pCity2->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClass2, iWorkIndex2);
	pCity1->GetCityBuildings()->SetBuildingGreatWork((BuildingClassTypes)iBuildingClass1, iWorkIndex1, workType2);
	pCity2->GetCityBuildings()->SetBuildingGreatWork((BuildingClassTypes)iBuildingClass2, iWorkIndex2, workType1);

#if defined(MOD_BALANCE_CORE)
	pCity1->UpdateAllNonPlotYields(true);
	if (pCity1 != pCity2)
	{
		pCity2->UpdateAllNonPlotYields(true);
	}
#endif
}

/// How many civs do we need to be influential over to win?
int CvGameCulture::GetNumCivsInfluentialForWin() const
{
	// How many players are alive?
	int iAliveMajors = 0;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iPlayerLoop);
		if (kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			iAliveMajors++;
		}
	}
	return iAliveMajors - 1;  // Don't have to be influential over yourself
}

// SERIALIZATION

FDataStream& operator<<(FDataStream& saveTo, const PublicOpinionTypes& readFrom)
{
	return saveTo << static_cast<int>(readFrom);
}
FDataStream& operator>>(FDataStream& loadFrom, PublicOpinionTypes& writeTo)
{
	int v = 0;
	loadFrom >> v;
	writeTo = static_cast<PublicOpinionTypes>(v);
	return loadFrom;
}

template<typename GameCulture, typename Visitor>
void CvGameCulture::Serialize(GameCulture& gameCulture, Visitor& visitor)
{
	visitor(gameCulture.m_CurrentGreatWorks);
	visitor(gameCulture.m_bReportedSomeoneInfluential);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameCulture& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGameCulture::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameCulture& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGameCulture::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvGreatWorkInMyEmpire
//=====================================
/// Default Constructor
CvGreatWorkInMyEmpire::CvGreatWorkInMyEmpire()
: m_iGreatWorkIndex(-1)
, m_iCityID(-1)
, m_eBuilding(NO_BUILDING)
, m_iSlot(-1)
{
}

/// Constructor
CvGreatWorkInMyEmpire::	CvGreatWorkInMyEmpire(int iIndex, int iCityID, BuildingTypes eBuilding, int iSlot, PlayerTypes ePlayer, EraTypes eEra)
: m_iGreatWorkIndex(iIndex)
, m_iCityID(iCityID)
, m_eBuilding(eBuilding)
, m_iSlot(iSlot)
, m_ePlayer(ePlayer)
, m_eEra(eEra)
{
}

//=====================================
// CvGreatWorkBuildingInMyEmpire
//=====================================
/// Default Constructor
CvGreatWorkBuildingInMyEmpire::CvGreatWorkBuildingInMyEmpire()
: m_iCityID(-1)
, m_eBuilding(NO_BUILDING)
{
	m_iGPThemingBonus = 0;
	m_bThemed = false;
	m_bEndangered = false;
	m_bPuppet = false;
    m_eYieldType = NO_YIELD;
}

//=====================================
// CvPlayerCulture
//=====================================
/// Constructor
CvPlayerCulture::CvPlayerCulture(void):
m_pPlayer(NULL)
{
}

/// Destructor
CvPlayerCulture::~CvPlayerCulture(void)
{
}

/// Initialize class data
void CvPlayerCulture::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	ResetDigCompletePlots();

	m_iLastTurnLifetimeCulture = 0;
	m_iLastTurnCPT = 0;
	m_iOpinionUnhappiness = 0;
	m_iLastUpdate = 0;
	m_iLastThemUpdate = 0;
	m_iBoredomCache = 0;
	m_iBoredomCacheTurn = 0;

	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_aiCulturalInfluence[iI] = 0;
		m_aiLastTurnCulturalInfluence[iI] = 0;
		m_aiLastTurnCulturalIPT[iI] = 0;
	}

	m_bReportedTwoCivsAway = false;
	m_bReportedOneCivAway = false;

	m_iSwappableWritingIndex  = -1;
	m_iSwappableArtIndex      = -1;
	m_iSwappableArtifactIndex = -1;
	m_iSwappableMusicIndex    = -1;

	m_iTurnIdeologySwitch		= -1;
	m_iTurnIdeologyAdopted		= -1;
}

// GREAT WORKS

/// Is there a Great Work slot of the appropriate type somewhere?
bool CvPlayerCulture::HasAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot)
{
	int iLoop = 0;
	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pCity->GetCityBuildings()->HasAvailableGreatWorkSlot(eGreatWorkSlot))
		{
			return true;
		}
	}
	return false;
}

/// How many open Great Work slots do we have of a certain type?
int CvPlayerCulture::GetNumAvailableGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot) const
{
	int iLoop = 0;
	int iCount = 0;

	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		iCount += pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eGreatWorkSlot);
	}
	return iCount;
}

/// Return the city (and building/slot) of the city that can provide the closest Great Work slot)
CvCity* CvPlayerCulture::GetClosestAvailableGreatWorkSlot(int iX, int iY, GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes& eBuildingClass, int& iSlot) const
{
	int iLoop = 0;
	int iBestDistance = MAX_INT;
	CvCity *pBestCity = NULL;
	BuildingClassTypes eBuildingClassReturned = NO_BUILDINGCLASS; // Passed by reference below
	int iSlotReturned = -1; // Passed by reference below

	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		int iDistance = plotDistance(iX, iY, pCity->getX(), pCity->getY());

		if (iDistance < iBestDistance)
		{
			if (pCity->GetCityBuildings()->GetNextAvailableGreatWorkSlot(eGreatWorkSlot, eBuildingClassReturned, iSlotReturned))
			{
				iBestDistance = iDistance;
				pBestCity = pCity;
				eBuildingClass = eBuildingClassReturned;
				iSlot = iSlotReturned;
			}
		}
	}

	return pBestCity;
}

/// How many Great Works are in the entire empure?
int CvPlayerCulture::GetNumGreatWorks() const
{
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	int iRtnValue = 0;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iRtnValue += pLoopCity->GetCityCulture()->GetNumGreatWorks();
	}

	return iRtnValue;
}

/// How many Great Work slots are in the entire empire?
int CvPlayerCulture::GetNumGreatWorkSlots() const
{
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	int iRtnValue = 0;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iRtnValue += pLoopCity->GetCityCulture()->GetNumGreatWorkSlots();
	}

	return iRtnValue;
}

/// How many Great Work slots of a particular type?
int CvPlayerCulture::GetNumGreatWorkSlots(GreatWorkSlotType eSlotType) const
{
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	int iRtnValue = 0;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iRtnValue += pLoopCity->GetCityCulture()->GetNumAvailableGreatWorkSlots(eSlotType);
	}

	return iRtnValue;
}

/// Does this player have this great work in one of their buildings
bool CvPlayerCulture::ControlsGreatWork(int iIndex)
{
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
			const BuildingTypes eBuilding = pLoopCity->GetBuildingTypeFromClass(eBuildingClass);
			if (eBuilding == NO_BUILDING)
				continue;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (!pkBuildingInfo)
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
				for (int iI = 0; iI < iNumSlots; iI++)
				{
					int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
					if (iGreatWorkIndex == iIndex)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CvPlayerCulture::GetGreatWorkLocation(int iSearchIndex, int &iReturnCityID, BuildingTypes &eReturnBuilding, int &iReturnSlot)
{
	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
			const BuildingTypes eBuilding = pLoopCity->GetBuildingTypeFromClass(eBuildingClass);
			if (eBuilding == NO_BUILDING)
				continue;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (!pkBuildingInfo)
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
				for (int iI = 0; iI < iNumSlots; iI++)
				{
					int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
					if (iGreatWorkIndex == iSearchIndex)
					{
						iReturnCityID = pLoopCity->GetID();
						eReturnBuilding = eBuilding;
						iReturnSlot = iI;
						return true;
					}
				}
			}
		}
	}

	return false;
}
#if defined(MOD_BALANCE_CORE)
void CvPlayerCulture::DoSwapGreatWorksHuman(bool bSwap)
{
	GreatWorkClass eWritingClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE");
	GreatWorkClass eArtClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART");
	GreatWorkClass eArtifactsClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");
	GreatWorkClass eMusicClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_MUSIC");

	vector<CvGreatWorkInMyEmpire> aGreatWorksWriting;
	vector<CvGreatWorkInMyEmpire> aGreatWorksArt;
	vector<CvGreatWorkInMyEmpire> aGreatWorksArtifacts;
	vector<CvGreatWorkInMyEmpire> aGreatWorksMusic;
	vector<CvGreatWorkInMyEmpire> aNull;

	vector<CvGreatWorkBuildingInMyEmpire> aGreatWorkBuildingsWriting;
	vector<CvGreatWorkBuildingInMyEmpire> aGreatWorkBuildingsArt;
	vector<CvGreatWorkBuildingInMyEmpire> aGreatWorkBuildingsMusic;

	int iLoop = 0;

	// CUSTOMLOG("Processing Great Works by city -> building -> slot into art(ifact)/writing/music silos");
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
			const BuildingTypes eBuilding = pLoopCity->GetBuildingTypeFromClass(eBuildingClass);
			if (eBuilding == NO_BUILDING)
				continue;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (!pkBuildingInfo)
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				//do not put great works in cities which might be lost soon
				if (!pLoopCity->isInDangerOfFalling(true))
				{
					CvGreatWorkBuildingInMyEmpire building;
					building.m_eBuilding = eBuilding;
					building.m_iCityID = pLoopCity->GetID();
					if (pLoopCity->getGPRateModifierPerLocalTheme() > 0)
					{
						building.m_iGPThemingBonus = pLoopCity->getGPRateModifierPerLocalTheme() * pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
						for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							building.m_iGPThemingBonus += pLoopCity->GetYieldChangesPerLocalTheme((YieldTypes)iI);
						}
					}

					if (pLoopCity->isCapital())
						building.m_bEndangered = false;
					else
						building.m_bEndangered = (pLoopCity->getDamage() > 0);

					GreatWorkSlotType eSlotType = pkBuildingInfo->GetGreatWorkSlotType();
					if (eSlotType == CvTypes::getGREAT_WORK_SLOT_LITERATURE())
					{
						aGreatWorkBuildingsWriting.push_back(building);
					}
					else if (eSlotType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
					{
						aGreatWorkBuildingsArt.push_back(building);
					}
					else if (eSlotType == CvTypes::getGREAT_WORK_SLOT_MUSIC())
					{
						aGreatWorkBuildingsMusic.push_back(building);
					}
				}

				//but do consider great works in all cities to be moved!
				int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
				for (int iI = 0; iI < iNumSlots; iI++)
				{
					int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
					if (iGreatWorkIndex != -1)
					{
						CvGreatWorkInMyEmpire work;
						work.m_eBuilding = eBuilding;
						work.m_iCityID = pLoopCity->GetID();
						work.m_iGreatWorkIndex = iGreatWorkIndex;
						work.m_iSlot = iI;
						work.m_ePlayer = GC.getGame().GetGameCulture()->GetGreatWorkCreator(iGreatWorkIndex);
						work.m_eEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex].m_eEra;

						GreatWorkType eGreatWorkType = GC.getGame().GetGameCulture()->GetGreatWorkType(iGreatWorkIndex);
						GreatWorkClass eGWClass = CultureHelpers::GetGreatWorkClass(eGreatWorkType);
						if (eGWClass == eWritingClass)
						{
							aGreatWorksWriting.push_back(work);
						}
						else if (eGWClass == eArtClass)
						{
							aGreatWorksArt.push_back(work);
						}
						else if (eGWClass == eArtifactsClass)
						{
							aGreatWorksArtifacts.push_back(work);
						}
						else if (eGWClass == eMusicClass)
						{
							aGreatWorksMusic.push_back(work);
						}
					}
				}
			}
		}
	}
	MoveWorks(CvTypes::getGREAT_WORK_SLOT_LITERATURE(), aGreatWorkBuildingsWriting, aGreatWorksWriting, aNull, YIELD_CULTURE, bSwap);
	MoveWorks(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT(), aGreatWorkBuildingsArt, aGreatWorksArt, aGreatWorksArtifacts, YIELD_CULTURE, bSwap);
	MoveWorks(CvTypes::getGREAT_WORK_SLOT_MUSIC(), aGreatWorkBuildingsMusic, aGreatWorksMusic, aNull, YIELD_CULTURE, bSwap);
}
#endif
/// AI routine to decide what Great Work swapping should take place (including placing Great Works up for swap from another player)
void CvPlayerCulture::DoSwapGreatWorks(YieldTypes eFocusYield)
{
	// CUSTOMLOG("DoSwapGreatWorks focus is %i", ((int) eFocusYield));
	GreatWorkClass eWritingClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE");
	GreatWorkClass eArtClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART");
	GreatWorkClass eArtifactsClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");
	GreatWorkClass eMusicClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_MUSIC");

	vector<CvGreatWorkInMyEmpire> aGreatWorksWriting;
	vector<CvGreatWorkInMyEmpire> aGreatWorksArt;
	vector<CvGreatWorkInMyEmpire> aGreatWorksArtifacts;
	vector<CvGreatWorkInMyEmpire> aGreatWorksMusic;
	vector<CvGreatWorkInMyEmpire> aNull;

	vector<CvGreatWorkBuildingInMyEmpire> aGreatWorkBuildingsWriting;
	vector<CvGreatWorkBuildingInMyEmpire> aGreatWorkBuildingsArt;
	vector<CvGreatWorkBuildingInMyEmpire> aGreatWorkBuildingsMusic;

	int iLoop = 0;

	// CUSTOMLOG("Processing Great Works by city -> building -> slot into art(ifact)/writing/music silos");
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
			const BuildingTypes eBuilding = pLoopCity->GetBuildingTypeFromClass(eBuildingClass);
			if (eBuilding == NO_BUILDING)
				continue;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (!pkBuildingInfo)
				continue;

			if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				//do not put great works in cities which might be lost soon
				if (!pLoopCity->isInDangerOfFalling(true))
				{
					CvGreatWorkBuildingInMyEmpire building;
					building.m_eBuilding = eBuilding;
					building.m_iCityID = pLoopCity->GetID();
					if (pLoopCity->getGPRateModifierPerLocalTheme() > 0)
					{
						building.m_iGPThemingBonus = pLoopCity->getGPRateModifierPerLocalTheme() * pLoopCity->GetCityCitizens()->GetTotalSpecialistCount();
						for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
						{
							building.m_iGPThemingBonus += pLoopCity->GetYieldChangesPerLocalTheme((YieldTypes)iI);
						}
					}

					if (pLoopCity->isCapital())
						building.m_bEndangered = false;
					else
						building.m_bEndangered = (pLoopCity->getDamage() > (pLoopCity->GetMaxHitPoints() / 2)) || (pLoopCity->IsRazing() && pLoopCity->getPopulation() < 3);

					building.m_bPuppet = pLoopCity->IsPuppet();
					building.m_eYieldType = GC.GetGameBuildings()->GetEntry(eBuilding)->GetGreatWorkYieldType();

					GreatWorkSlotType eSlotType = pkBuildingInfo->GetGreatWorkSlotType();
					if (eSlotType == CvTypes::getGREAT_WORK_SLOT_LITERATURE())
					{
						aGreatWorkBuildingsWriting.push_back(building);
					}
					else if (eSlotType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
					{
						aGreatWorkBuildingsArt.push_back(building);
					}
					else if (eSlotType == CvTypes::getGREAT_WORK_SLOT_MUSIC())
					{
						aGreatWorkBuildingsMusic.push_back(building);
					}
				}

				//but do consider great works in all cities to be moved
				int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
				for (int iI = 0; iI < iNumSlots; iI++)
				{
					int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
					if (iGreatWorkIndex != -1)
					{
						CvGreatWorkInMyEmpire work;
						work.m_eBuilding = eBuilding;
						work.m_iCityID = pLoopCity->GetID();
						work.m_iGreatWorkIndex = iGreatWorkIndex;
						work.m_iSlot = iI;
						work.m_ePlayer = GC.getGame().GetGameCulture()->GetGreatWorkCreator(iGreatWorkIndex);
						work.m_eEra = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex].m_eEra;

						GreatWorkType eGreatWorkType = GC.getGame().GetGameCulture()->GetGreatWorkType(iGreatWorkIndex);
						GreatWorkClass eGWClass = CultureHelpers::GetGreatWorkClass(eGreatWorkType);
						if (eGWClass == eWritingClass)
						{
							aGreatWorksWriting.push_back(work);
						}
						else if (eGWClass == eArtClass)
						{
							aGreatWorksArt.push_back(work);
						}
						else if (eGWClass == eArtifactsClass)
						{
							aGreatWorksArtifacts.push_back(work);
						}
						else if (eGWClass == eMusicClass)
						{
							aGreatWorksMusic.push_back(work);
						}
					}
				}
			}
		}
	}

	MoveWorks(CvTypes::getGREAT_WORK_SLOT_LITERATURE(), aGreatWorkBuildingsWriting, aGreatWorksWriting, aNull, eFocusYield, true);
	MoveWorks(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT(), aGreatWorkBuildingsArt, aGreatWorksArt, aGreatWorksArtifacts, eFocusYield, true);
	MoveWorks(CvTypes::getGREAT_WORK_SLOT_MUSIC(), aGreatWorkBuildingsMusic, aGreatWorksMusic, aNull, eFocusYield, true);
}

/// Sorts building by AI priority which determines the order they should be evaluated when applying theming bonuses
static bool SortThemingBonus(const CvGreatWorkBuildingInMyEmpire& kEntry1, const CvGreatWorkBuildingInMyEmpire& kEntry2)
{
	CvBuildingEntry *pEntry1 = GC.GetGameBuildings()->GetEntry(kEntry1.m_eBuilding);
	CvBuildingEntry *pEntry2 = GC.GetGameBuildings()->GetEntry(kEntry2.m_eBuilding);

	if (pEntry1 && pEntry2)
	{
		CvThemingBonusInfo *pBonus1 = pEntry1->GetThemingBonusInfo(0);
		CvThemingBonusInfo *pBonus2 = pEntry2->GetThemingBonusInfo(0);

		if (pBonus1 && pBonus2)
		{
			int iBonus1 = 50 * pBonus1->GetAIPriority() + kEntry1.m_iGPThemingBonus;
			int iBonus2 = 50 * pBonus2->GetAIPriority() + kEntry2.m_iGPThemingBonus;
			return (iBonus1 > iBonus2);
		}
		else if (pBonus1)
		{
			return true;
		}
	}

	return false;
}

/// Overall routine that orchestrates all the maneuvering of Great Works between buildings and players for one AI turn
void CvPlayerCulture::MoveWorks(GreatWorkSlotType eType, vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, YieldTypes eFocusYield, bool bSwap)
{
	// CUSTOMLOG("Move Works for slot type %i", ((int) eType));
	std::stable_sort (buildings.begin(), buildings.end(), SortThemingBonus);

	//   The order is
	//   - theme homeland and puppet buildings
	//   - theme endangered buildings
	//   - work out the swaps
	//   - fill single homeland buildings with a focused yield
	//   - fill single puppet buildings with a focused yield
	//   - fill single homeland buildings with any yield
	//   - fill single puppet buildings with any yield
	//   - fill single homeland buildings with no yield
	//   - fill single puppet buildings with no yield
	//   - fill single endangered buildings with a focused yield
	//   - fill single endangered buildings with any yield
	//   - fill single endangered buildings with no yield

	bool bUpdate = false;

	// First building that are not endangered and not puppets
	// CUSTOMLOG("  ... theming safe buildings");
	vector<CvGreatWorkBuildingInMyEmpire>::iterator itBuilding;
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); ++itBuilding)
	{
		if (!itBuilding->m_bEndangered && !itBuilding->m_bPuppet)
		{
			itBuilding->m_bThemed = false;
			if (ThemeBuilding(itBuilding, works1, works2, false /*bConsiderOtherPlayers*/))
			{
				itBuilding->m_bThemed = true;
				bUpdate = true;
			}
		}
	}

	// Next building that are not endangered and are puppets
	// CUSTOMLOG("  ... theming safe buildings");
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); ++itBuilding)
	{
		if (!itBuilding->m_bEndangered && itBuilding->m_bPuppet)
		{
			itBuilding->m_bThemed = false;
			if (ThemeBuilding(itBuilding, works1, works2, false /*bConsiderOtherPlayers*/))
			{
				itBuilding->m_bThemed = true;
				bUpdate = true;
			}
		}
	}

	//Not a human? Let's get swappin!
	if ((works1.size() > 0 || works2.size() > 0) && !m_pPlayer->isHuman())
		bSwap = true;

	// Set the first work left that we haven't themed as something we'd be willing to trade
	// CUSTOMLOG("Setting available swaps");

	// For Writing
	if (eType == CvTypes::getGREAT_WORK_SLOT_LITERATURE())
	{
		if(bSwap)
		{
			if (works1.size() > 0)
			{
				// CUSTOMLOG("  ... for writing to %i", works1[0].m_iGreatWorkIndex);
				SetSwappableWritingIndex(works1[0].m_iGreatWorkIndex);
			}
			else if (works2.size() > 0)
			{
				// CUSTOMLOG("  ... for writing to %i", works2[0].m_iGreatWorkIndex);
				SetSwappableWritingIndex(works2[0].m_iGreatWorkIndex);
			}
			else
			{
				SetSwappableWritingIndex(-1);
			}
		}
		else
		{
			SetSwappableWritingIndex(-1);
		}
	}
	// For Art and Artifacts
	else
	{
		if (eType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
		{
			if(bSwap)
			{
				if (works1.size() > 0)
				{
					// CUSTOMLOG("  ... for art(ifact) to %i", works1[0].m_iGreatWorkIndex);
					SetSwappableArtIndex(works1[0].m_iGreatWorkIndex);
				}
				else
				{
					SetSwappableArtIndex(-1);
				}
			}
			else
			{
				SetSwappableArtIndex(-1);
			}
		}

		if (eType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT() && GetSwappableArtIndex() == -1)
		{
			if (bSwap)
			{
				if (works2.size() > 0)
				{
					// CUSTOMLOG("  ... for art(ifact) to %i", works2[0].m_iGreatWorkIndex);
					SetSwappableArtifactIndex(works2[0].m_iGreatWorkIndex);
				}
				else if (works1.size() > 0)
				{
					// CUSTOMLOG("  ... for art(ifact) to %i", works1[0].m_iGreatWorkIndex);
					SetSwappableArtIndex(works1[0].m_iGreatWorkIndex);
				}
				else
				{
					SetSwappableArtifactIndex(-1);
				}
			}
			else
			{
				SetSwappableArtifactIndex(-1);
			}
		}
	}

	if (bSwap)
	{
		// One more pass through those that are not endangered to see if swapping with another player would help (as long as this isn't Music)
		if (eType != CvTypes::getGREAT_WORK_SLOT_MUSIC())
		{
			// CUSTOMLOG("  ... checking safe buildings for swaps");
			for (itBuilding = buildings.begin(); itBuilding != buildings.end(); ++itBuilding)
			{
				if (!itBuilding->m_bEndangered && !itBuilding->m_bThemed)
				{
					if (ThemeBuilding(itBuilding, works1, works2, true /*bConsiderOtherPlayers*/))
					{
						itBuilding->m_bThemed = true;
						bUpdate = true;
					}
				}
			}
		}
	}

	// Then endangered ones
	// CUSTOMLOG("  ... theming endangered buildings");
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); ++itBuilding)
	{
		if (itBuilding->m_bEndangered)
		{
			itBuilding->m_bThemed = false;
			if (ThemeBuilding(itBuilding, works1, works2, false /*bConsiderOtherPlayers*/))
			{
				itBuilding->m_bThemed = true;
				bUpdate = true;
			}
		}
	}

	bool bSecondUpdate = MoveSingleWorks(buildings, works1, works2, eFocusYield, true);

	if(bSecondUpdate || bUpdate)
	{
		std::vector<CvCity*> CityList;
		for (itBuilding = buildings.begin(); itBuilding != buildings.end(); ++itBuilding)
		{
			CvCity* pCity = m_pPlayer->getCity(itBuilding->m_iCityID);
			if (pCity != NULL)
			{
				bool bAlreadyChecked = false;
				if (CityList.size() > 0)
				{
					for (uint ui = 0; ui < CityList.size(); ui++)
					{
						if (pCity == CityList[ui])
						{
							bAlreadyChecked = true;
							break;
						}
					}
				}

				if (bAlreadyChecked)
					continue;

				pCity->UpdateAllNonPlotYields(false);
				CityList.push_back(pCity);
			}
		}
	}
}

/// Uses the available Great Works to fill a building with those works that provide the best Theming Bonus
bool CvPlayerCulture::ThemeBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator buildingIt, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool bConsiderOtherPlayers)
{
	CvGameCulture *pkGameCulture = GC.getGame().GetGameCulture();

	vector<CvGreatWorkInMyEmpire> worksToConsider;
	vector<CvGreatWorkInMyEmpire>::const_iterator it;
	vector<CvGreatWorkInMyEmpire>::const_iterator it2;
	vector<CvGreatWorkInMyEmpire>::iterator it3;
	vector<CvGreatWorkInMyEmpire> tempWorks;

	vector<int> aWorksChosen;
	vector<PlayerTypes> aPlayersSeen;
	vector<EraTypes> aErasSeen;

	CvBuildingEntry *pkEntry = GC.getBuildingInfo(buildingIt->m_eBuilding);
	if (!pkEntry || pkEntry->GetGreatWorkCount() < 2)
	{
		return false;
	}

	CvWeightedVector<int> bestThemes;

	for (int iI = 0; iI < pkEntry->GetNumThemingBonuses(); iI++)
	{
		if (pkEntry->GetThemingBonusInfo(iI) == NULL)
			continue;

		bestThemes.push_back(iI, pkEntry->GetThemingBonusInfo(iI)->GetBonus());
	}

	bestThemes.StableSortItems();

	// Try each of the theming bonuses for this building
	for (int iI = 0; iI < bestThemes.size(); iI++)
	{
		bool bThemedProperly = false;
		CvThemingBonusInfo *pkBonusInfo = pkEntry->GetThemingBonusInfo(bestThemes.GetElement(iI));
		if (pkBonusInfo->IsMustBeArt())
		{
			worksToConsider = works1;
		}
		else if (pkBonusInfo->IsMustBeArtifact())
		{
			worksToConsider = works2;
		}
		// Dedicated routine to handle the equal art/artifact case
		else if (pkBonusInfo->IsMustBeEqualArtArtifact())
		{
			if (ThemeEqualArtArtifact(*buildingIt, bestThemes.GetElement(iI), pkEntry->GetGreatWorkCount(), works1, works2, bConsiderOtherPlayers, bestThemes.GetElement(iI)))
			{
				return true;
			}
		}
		else
		{
			worksToConsider = works1;
			for (it = works2.begin(); it != works2.end(); ++it)
			{
				worksToConsider.push_back(*it);
			}
		}

		// If not enough works, try other theming bonuses
		int iCountSlots = pkEntry->GetGreatWorkCount();
		if (worksToConsider.size() < (unsigned int)iCountSlots)
		{
			continue;
		}

		// Try each of the works as the starter
		for (it = worksToConsider.begin(); it != worksToConsider.end(); ++it)
		{
			// First, make sure this "starter" is valid
			if (pkBonusInfo->IsRequiresOwner() && it->m_ePlayer != m_pPlayer->GetID())
			{
				continue;
			}
			if (pkBonusInfo->IsRequiresAnyButOwner() && it->m_ePlayer == m_pPlayer->GetID())
			{
				continue;
			}

			aWorksChosen.clear();
			aPlayersSeen.clear();
			aErasSeen.clear();

			aWorksChosen.push_back(it->m_iGreatWorkIndex);
			aPlayersSeen.push_back(it->m_ePlayer);
			aErasSeen.push_back(it->m_eEra);

			// Loop through the rest looking for works that will match up
			it2 = it;
			for (++it2; it2 != worksToConsider.end() && aWorksChosen.size() < (unsigned int)iCountSlots; ++it2)
			{
				if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, it2->m_eEra, aErasSeen, it2->m_ePlayer, aPlayersSeen, m_pPlayer->GetID()))
				{
					aWorksChosen.push_back(it2->m_iGreatWorkIndex);
					aPlayersSeen.push_back(it2->m_ePlayer);
					aErasSeen.push_back(it2->m_eEra);
				}
			}

			// Did we theme it properly?
			bThemedProperly = false;
			if (aWorksChosen.size() == iCountSlots && CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == bestThemes.GetElement(iI))
			{
				bThemedProperly = true;
			}

			if (!bThemedProperly && bConsiderOtherPlayers && (pkEntry->GetGreatWorkSlotType() != CvTypes::getGREAT_WORK_SLOT_MUSIC()))
			{
				//Let's look at every applicable GW in the world.
				vector<int> aForeignWorksToConsider;
				CvWeightedVector<int> aGreatWorkPairs;
				vector<int> aWorksToDiscard;

				GreatWorkSlotType eSlotType = pkEntry->GetGreatWorkSlotType();

				aGreatWorkPairs.clear();
				aForeignWorksToConsider.clear();
				aWorksToDiscard.clear();
				for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
					if (kPlayer.isAlive() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid((PlayerTypes)iLoopPlayer) && !m_pPlayer->IsAtWarWith((PlayerTypes)iLoopPlayer))
					{
						int iToBeAcquiredWorkIndex = NO_GREAT_WORK;

						if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
						{
							if (pkBonusInfo->IsMustBeArt())
							{
								iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtIndex();
							}
							else if (pkBonusInfo->IsMustBeArtifact())
							{
								iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtifactIndex();
							}
							else
							{
								iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtIndex();
								if (iToBeAcquiredWorkIndex == -1)
									iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtifactIndex();
							}
						}
						else
						{
							iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableWritingIndex();
						}

						// Does this work fit?
						if (iToBeAcquiredWorkIndex != -1)
						{
							//Push back into a global consideration pool of all foreign works on offer.
							aForeignWorksToConsider.push_back(iToBeAcquiredWorkIndex);
						}
					}
				}
				//Okay, we have a list of all GWs available right now now. Let's look at the list and figure out what we can do with it.
				if(aForeignWorksToConsider.size() > 0)
				{
					for (uint ui = 0; ui < aForeignWorksToConsider.size(); ui++)
					{
						//We found enough pairs? Cool.
						//Shouldn't go above, but it might.
						if (aGreatWorkPairs.size() >= iCountSlots)
						{
							break;
						}

						aWorksToDiscard.clear();
						//Add chosen works (if any) to a discard pool so we know exactly what we have to offer, and what we don't have to offer.
						if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
						{
							if (pkBonusInfo->IsMustBeArt())
							{
								if (GetSwappableArtIndex() != -1)
									aWorksToDiscard.push_back(GetSwappableArtIndex());
							}
							else if (pkBonusInfo->IsMustBeArtifact())
							{
								if (GetSwappableArtifactIndex() != -1)
									aWorksToDiscard.push_back(GetSwappableArtifactIndex());
							}
							else
							{
								if (GetSwappableArtIndex() != -1)
									aWorksToDiscard.push_back(GetSwappableArtIndex());

								if (GetSwappableArtifactIndex() != -1)
									aWorksToDiscard.push_back(GetSwappableArtifactIndex());
							}
						}
						else
						{
							if (GetSwappableWritingIndex() != -1)
								aWorksToDiscard.push_back(GetSwappableWritingIndex());
						}

						if (aWorksToDiscard.size() <= 0)
							break;

						bool bInvalid = false;
						for (uint l = 0; l < aWorksToDiscard.size(); l++)
						{
							//work considering is our work to discard? whoops.
							if (it->m_iGreatWorkIndex == aWorksToDiscard[l])
							{
								bInvalid = true;
								break;
							}
						}

						if (bInvalid)
							continue;

						int iToBeAcquiredWorkIndex = NO_GREAT_WORK;
						int iToBeDiscardedWorkIndex = NO_GREAT_WORK;

						iToBeAcquiredWorkIndex = aForeignWorksToConsider[ui];

						if (iToBeAcquiredWorkIndex == NO_GREAT_WORK)
						{
							continue;
						}

						PlayerTypes eForeignOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeAcquiredWorkIndex);

						//Bwa?
						if (eForeignOwner == m_pPlayer->GetID() || eForeignOwner == NO_PLAYER)
						{
							continue;
						}

						//don't trade back for our own GWs...
						if (GC.getGame().GetGameCulture()->GetGreatWorkCreator(iToBeAcquiredWorkIndex) == m_pPlayer->GetID())
							continue;

						for (uint ui = 0; ui < aWorksToDiscard.size(); ui++)
						{
							GreatWorkClass eClass = GC.getGame().GetGameCulture()->GetGreatWorkClass(aWorksToDiscard[ui]);

							if (eSlotType == CvTypes::getGREAT_WORK_SLOT_LITERATURE())
							{
								if (eClass != (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE"))
									continue;
							}
							else if (eSlotType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
							{
								if (eClass != (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART") && eClass != (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT"))
									continue;
							}

							iToBeDiscardedWorkIndex = aWorksToDiscard[ui];
							break;
						}

						if (iToBeDiscardedWorkIndex == -1)
							continue;

						PlayerTypes eOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeDiscardedWorkIndex);
						//Bwa?
						if (eOwner != m_pPlayer->GetID() || eOwner == NO_PLAYER)
						{
							continue;
						}

						//We have a swappable Great Work and a target Great Work? Cool.
						if (iToBeAcquiredWorkIndex != -1 && iToBeDiscardedWorkIndex != -1)
						{
							//Do we match? We should.
							GreatWorkClass eClass1 = GC.getGame().GetGameCulture()->GetGreatWorkClass(iToBeAcquiredWorkIndex);
							GreatWorkClass eClass2 = GC.getGame().GetGameCulture()->GetGreatWorkClass(iToBeDiscardedWorkIndex);

							if (eClass1 != eClass2)
							{
								bool bArtOk = false;

								//exception! art/artifacts can be swapped
								if ((eClass1 == 1 && eClass2 == 2) || (eClass1 == 2 && eClass2 == 1))
									bArtOk = true;

								if (!bArtOk)
									continue;
							}
							EraTypes eEra = pkGameCulture->m_CurrentGreatWorks[iToBeAcquiredWorkIndex].m_eEra;
							PlayerTypes ePlayer = pkGameCulture->GetGreatWorkCreator(iToBeAcquiredWorkIndex);

							//We need to know if it matches local GWs, but we also need to know if it matches the foreign ones we've already seen... 
							if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, eEra, aErasSeen, ePlayer, aPlayersSeen, m_pPlayer->GetID()))
							{
								//If it does match, add to actual list of things to grab and discard (paired up).
								aGreatWorkPairs.push_back(iToBeAcquiredWorkIndex, iToBeDiscardedWorkIndex);
								//And add our discarded work to our list of things to discard so we don't assume it still exists.
								if (eClass2 == (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE"))
									SetSwappableWritingIndex(-1);
								else if (eClass2 == (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART"))
									SetSwappableArtIndex(-1);
								else if (eClass2 == (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT"))
								SetSwappableArtifactIndex(-1);
							}
						}
					}
				}
				//We've got at least one work swap pair? Awesome.
				if(aGreatWorkPairs.size() > 0)
				{
					for (int ui = 0; ui < aGreatWorkPairs.size(); ui++)
					{
						//Grab our data for swap and discard item.
						int iToBeAcquiredWorkIndex = aGreatWorkPairs.GetElement(ui);
						int iToBeDiscardedWorkIndex = aGreatWorkPairs.GetWeight(ui);

						//Can't move forward if either are invalid.
						if(iToBeAcquiredWorkIndex == NO_GREAT_WORK || iToBeDiscardedWorkIndex == NO_GREAT_WORK)
						{
							continue;
						}
						PlayerTypes eForeignOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeAcquiredWorkIndex);
						//Bwa?
						if(eForeignOwner == m_pPlayer->GetID() || eForeignOwner == NO_PLAYER)
						{
							continue;
						}
						PlayerTypes eOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeDiscardedWorkIndex);
						//Bwa?
						if(eOwner != m_pPlayer->GetID() || eOwner == NO_PLAYER)
						{
							continue;
						}
						//Push all the valid foreign works into our list.
						aWorksChosen.push_back(iToBeAcquiredWorkIndex);
						aPlayersSeen.push_back(it->m_ePlayer);
						aErasSeen.push_back(it->m_eEra);
					}
					//We've got the theme we want? Or perhaps we're building a theme?
					if (CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == bestThemes.GetElement(iI))
					{
						int iLoop = 0;
						//Now loop back through for the actual swapping.
						for (int ui = 0; ui < aGreatWorkPairs.size(); ui++)
						{
							//Grab our data for swap and discard item.
							int iToBeAcquiredWorkIndex = aGreatWorkPairs.GetElement(ui);
							int iToBeDiscardedWorkIndex = aGreatWorkPairs.GetWeight(ui);

							//Can't move forward if either are invalid.
							if(iToBeAcquiredWorkIndex == NO_GREAT_WORK || iToBeDiscardedWorkIndex == NO_GREAT_WORK)
							{
								continue;
							}
							PlayerTypes eForeignOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeAcquiredWorkIndex);
							//Bwa?
							if(eForeignOwner == m_pPlayer->GetID() || eForeignOwner == NO_PLAYER)
							{
								continue;
							}
							PlayerTypes eOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeDiscardedWorkIndex);
							//Bwa?
							if(eOwner != m_pPlayer->GetID() || eOwner == NO_PLAYER)
							{
								continue;
							}

							//Let's make sure our item is in our swap slot.
							if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
							{
								if (pkBonusInfo->IsMustBeArt())
								{
									if(iToBeDiscardedWorkIndex != m_pPlayer->GetCulture()->GetSwappableArtIndex())
									{
										m_pPlayer->GetCulture()->SetSwappableArtIndex(iToBeDiscardedWorkIndex);
									}
								}
								else if (pkBonusInfo->IsMustBeArtifact())
								{
									if(iToBeDiscardedWorkIndex != m_pPlayer->GetCulture()->GetSwappableArtifactIndex())
									{
										m_pPlayer->GetCulture()->SetSwappableArtifactIndex(iToBeDiscardedWorkIndex);
									}
								}
							}
							else
							{
								if(iToBeDiscardedWorkIndex != m_pPlayer->GetCulture()->GetSwappableWritingIndex())
								{
									m_pPlayer->GetCulture()->SetSwappableWritingIndex(iToBeDiscardedWorkIndex);
								}
							}
							if(pkGameCulture->SwapGreatWorks(m_pPlayer->GetID(), iToBeDiscardedWorkIndex, eForeignOwner, iToBeAcquiredWorkIndex))
							{

								LogSwapMultipleWorks(eOwner, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);

								// Update works list for the swapped items.
								tempWorks.clear();
								if (!pkBonusInfo->IsMustBeArtifact())
								{
									for (it3 = works1.begin(); it3 != works1.end(); it3++)
									{
										if (it3->m_iGreatWorkIndex == iToBeDiscardedWorkIndex)
										{
											it3->m_iGreatWorkIndex = iToBeAcquiredWorkIndex;
											GetGreatWorkLocation(it3->m_iGreatWorkIndex, it3->m_iCityID, it3->m_eBuilding, it3->m_iSlot);
										}
										tempWorks.push_back(*it3);
									}
									works1 = tempWorks;
								}
								else
								{
									for (it3 = works2.begin(); it3 != works2.end(); it3++)
									{
										if (it3->m_iGreatWorkIndex == iToBeDiscardedWorkIndex)
										{
											it3->m_iGreatWorkIndex = iToBeAcquiredWorkIndex;
											GetGreatWorkLocation(it3->m_iGreatWorkIndex, it3->m_iCityID, it3->m_eBuilding, it3->m_iSlot);
										}
										tempWorks.push_back(*it3);
									}
									works2 = tempWorks;
								}

								CultureHelpers::SendArtSwapNotification(pkEntry->GetGreatWorkSlotType(), pkBonusInfo->IsMustBeArt(), m_pPlayer->GetID(), eForeignOwner, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);
								// Themed it through acquisition
								iLoop++;
							}
						}
						if(iLoop > 0)
						{
							bThemedProperly = true;
						}
					}
				}
			}
			if (bThemedProperly)
			{
				int iSlot = 0;
				for (int jJ = 0; jJ < (int)aWorksChosen.size(); jJ++)
				{
					for (int kK = 0; kK < (int)worksToConsider.size(); kK++)
					{
						if (worksToConsider[kK].m_iGreatWorkIndex == aWorksChosen[jJ])
						{
							if (MoveWorkIntoSlot(worksToConsider[kK], buildingIt->m_iCityID, buildingIt->m_eBuilding, iSlot))
							{
								iSlot++;
								break;
							}
						}
					}
				}

				// Remove these works from those to consider later
				tempWorks.clear();
				for (it3 = works1.begin(); it3 != works1.end(); it3++)
				{
					// Copy it over if not chosen, updating its location
					if (find(aWorksChosen.begin(), aWorksChosen.end(), it3->m_iGreatWorkIndex) == aWorksChosen.end())
					{
						GetGreatWorkLocation(it3->m_iGreatWorkIndex, it3->m_iCityID, it3->m_eBuilding, it3->m_iSlot);
						tempWorks.push_back(*it3);
					}
				}
				works1 = tempWorks;

				tempWorks.clear();
				for (it3 = works2.begin(); it3 != works2.end(); it3++)
				{
					// Copy it over if not chosen, updating its location
					if (find(aWorksChosen.begin(), aWorksChosen.end(), it3->m_iGreatWorkIndex) == aWorksChosen.end())
					{
						GetGreatWorkLocation(it3->m_iGreatWorkIndex, it3->m_iCityID, it3->m_eBuilding, it3->m_iSlot);
						tempWorks.push_back(*it3);
					}
				}
				works2 = tempWorks;

				// All done
				if (CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == bestThemes.GetElement(iI))
				{
					LogThemedBuilding(buildingIt->m_iCityID, buildingIt->m_eBuilding, pkBonusInfo->GetBonus());
					return true;
				}
			}
		}
	}
	return false;
}

/// Specialized version of ThemeBuilding() that handles those buildings that are split between Art and Artifact
bool CvPlayerCulture::ThemeEqualArtArtifact(CvGreatWorkBuildingInMyEmpire kBldg, int iThemingBonusIndex, int iNumSlots, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool bConsiderOtherPlayers, int iThemeID)
{
	CvGameCulture *pkGameCulture = GC.getGame().GetGameCulture();

	vector<CvGreatWorkInMyEmpire>::const_iterator it;
	vector<CvGreatWorkInMyEmpire>::iterator it5;
	vector<CvGreatWorkInMyEmpire> tempWorks;

	vector<int> aArtifactsChosen;
	vector<PlayerTypes> aArtifactsPlayersSeen;
	vector<EraTypes> aArtifactsErasSeen;

	int iWorksInHalf = iNumSlots / 2;
	if (iWorksInHalf % 2 != 0 || (int)works1.size() < iWorksInHalf || (int)works2.size() < iWorksInHalf)
	{
		return false;
	}

	CvBuildingEntry *pkEntry = GC.getBuildingInfo(kBldg.m_eBuilding);
	if (!pkEntry || pkEntry->GetGreatWorkCount() < 2)
	{
		return false;
	}

	CvThemingBonusInfo *pkBonusInfo = pkEntry->GetThemingBonusInfo(iThemingBonusIndex);

	int iCountSlots = pkEntry->GetGreatWorkCount();

	// Try each of the Artifacts as the starter
	for (it = works2.begin(); it != works2.end(); ++it)
	{
		// First, make sure this "starter" is valid
		if (pkBonusInfo->IsRequiresOwner() && it->m_ePlayer != m_pPlayer->GetID())
		{
			continue;
		}
		if (pkBonusInfo->IsRequiresAnyButOwner() && it->m_ePlayer == m_pPlayer->GetID())
		{
			continue;
		}

		aArtifactsChosen.clear();
		aArtifactsPlayersSeen.clear();
		aArtifactsErasSeen.clear();

		aArtifactsChosen.push_back(it->m_iGreatWorkIndex);
		aArtifactsPlayersSeen.push_back(it->m_ePlayer);
		aArtifactsErasSeen.push_back(it->m_eEra);

		// Loop through the rest looking for works that will match up
		vector<CvGreatWorkInMyEmpire>::const_iterator it2 = it;
		for (++it2; it2 != works2.end() && aArtifactsChosen.size() < (unsigned int)iWorksInHalf; ++it2)
		{
			if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, it2->m_eEra, aArtifactsErasSeen, it2->m_ePlayer, aArtifactsPlayersSeen, m_pPlayer->GetID()))
			{
				aArtifactsChosen.push_back(it2->m_iGreatWorkIndex);
				aArtifactsPlayersSeen.push_back(it2->m_ePlayer);
				aArtifactsErasSeen.push_back(it2->m_eEra);
			}
		}
		// Do we have the right amount of art?
		if (aArtifactsChosen.size() == iWorksInHalf)
		{
			vector<int> aWorksChosen;
			vector<PlayerTypes> aPlayersSeen;
			vector<EraTypes> aErasSeen;

			// Now see if we can get the right number of art works to work as well
			vector<CvGreatWorkInMyEmpire>::iterator it3;
			for (it3 = works1.begin(); it3 != works1.end() && aWorksChosen.size() < (unsigned int)iNumSlots; it3++)
			{
				// First, make sure this "starter" is valid
				if (pkBonusInfo->IsRequiresOwner() && it3->m_ePlayer != m_pPlayer->GetID())
				{
					continue;
				}
				if (pkBonusInfo->IsRequiresAnyButOwner() && it3->m_ePlayer == m_pPlayer->GetID())
				{
					continue;
				}
				if (pkBonusInfo->IsSameEra() && it3->m_eEra != aArtifactsErasSeen[0])
				{
					continue;
				}
				if (pkBonusInfo->IsUniqueEras() && find(aArtifactsErasSeen.begin(), aArtifactsErasSeen.end(), it3->m_eEra) != aArtifactsErasSeen.end())
				{
					continue;
				}

				aWorksChosen = aArtifactsChosen;
				aPlayersSeen = aArtifactsPlayersSeen;
				aErasSeen = aArtifactsErasSeen;

				aWorksChosen.push_back(it3->m_iGreatWorkIndex);
				aPlayersSeen.push_back(it3->m_ePlayer);
				aErasSeen.push_back(it3->m_eEra);

				// Loop through the rest looking for works that will match up
				vector<CvGreatWorkInMyEmpire>::const_iterator it4 = it3;
				for (it4++; it4 != works1.end() && aWorksChosen.size() < (unsigned int)iNumSlots; it4++)
				{
					if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, it4->m_eEra, aErasSeen, it4->m_ePlayer, aPlayersSeen, m_pPlayer->GetID()))
					{
						aWorksChosen.push_back(it4->m_iGreatWorkIndex);
						aPlayersSeen.push_back(it4->m_ePlayer);
						aErasSeen.push_back(it4->m_eEra);
					}
				}

				// Did we theme it properly?
				bool bThemedProperly = false;
				if (aWorksChosen.size() == iNumSlots && CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == iThemingBonusIndex)
				{
					bThemedProperly = true;
				}

				if (!bThemedProperly && bConsiderOtherPlayers && (pkEntry->GetGreatWorkSlotType() != CvTypes::getGREAT_WORK_SLOT_MUSIC()))
				{
					//Let's look at every applicable GW in the world.
					vector<int> aForeignWorksToConsider;
					CvWeightedVector<int> aGreatWorkPairs;
					vector<int> aWorksToDiscard;

					GreatWorkSlotType eSlotType = pkEntry->GetGreatWorkSlotType();

					aGreatWorkPairs.clear();
					aForeignWorksToConsider.clear();
					aWorksToDiscard.clear();
					for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
						if (kPlayer.isAlive() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid((PlayerTypes)iLoopPlayer) && !m_pPlayer->IsAtWarWith((PlayerTypes)iLoopPlayer))
						{
							int iToBeAcquiredWorkIndex = NO_GREAT_WORK;

							if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
							{
								if (pkBonusInfo->IsMustBeArt())
								{
									iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtIndex();
								}
								else if (pkBonusInfo->IsMustBeArtifact())
								{
									iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtifactIndex();
								}
								else
								{
									iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtIndex();
									if (iToBeAcquiredWorkIndex == -1)
										iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtifactIndex();
								}
							}
							else
							{
								iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableWritingIndex();
							}

							// Does this work fit?
							if (iToBeAcquiredWorkIndex != -1)
							{
								//Push back into a global consideration pool of all foreign works on offer.
								aForeignWorksToConsider.push_back(iToBeAcquiredWorkIndex);
							}
						}
					}
					//Okay, we have a list of all GWs available right now now. Let's look at the list and figure out what we can do with it.
					if (aForeignWorksToConsider.size() > 0)
					{
						for (uint ui = 0; ui < aForeignWorksToConsider.size(); ui++)
						{
							//We found enough pairs? Cool.
							//Shouldn't go above, but it might.
							if (aGreatWorkPairs.size() >= iCountSlots)
							{
								break;
							}

							aWorksToDiscard.clear();
							//Add chosen works (if any) to a discard pool so we know exactly what we have to offer, and what we don't have to offer.
							if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
							{
								if (pkBonusInfo->IsMustBeArt())
								{
									if (GetSwappableArtIndex() != -1)
										aWorksToDiscard.push_back(GetSwappableArtIndex());
								}
								else if (pkBonusInfo->IsMustBeArtifact())
								{
									if (GetSwappableArtifactIndex() != -1)
										aWorksToDiscard.push_back(GetSwappableArtifactIndex());
								}
								else
								{
									if (GetSwappableArtIndex() != -1)
										aWorksToDiscard.push_back(GetSwappableArtIndex());

									if (GetSwappableArtifactIndex() != -1)
										aWorksToDiscard.push_back(GetSwappableArtifactIndex());
								}
							}
							else
							{
								if (GetSwappableWritingIndex() != -1)
									aWorksToDiscard.push_back(GetSwappableWritingIndex());
							}

							if (aWorksToDiscard.size() <= 0)
								break;

							bool bInvalid = false;
							for (uint l = 0; l < aWorksToDiscard.size(); l++)
							{
								//work considering is our work to discard? whoops.
								if (it->m_iGreatWorkIndex == aWorksToDiscard[l])
								{
									bInvalid = true;
									break;
								}
							}

							if (bInvalid)
								continue;

							int iToBeAcquiredWorkIndex = NO_GREAT_WORK;
							int iToBeDiscardedWorkIndex = NO_GREAT_WORK;

							iToBeAcquiredWorkIndex = aForeignWorksToConsider[ui];

							if (iToBeAcquiredWorkIndex == NO_GREAT_WORK)
							{
								continue;
							}

							PlayerTypes eForeignOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeAcquiredWorkIndex);

							//Bwa?
							if (eForeignOwner == m_pPlayer->GetID() || eForeignOwner == NO_PLAYER)
							{
								continue;
							}

							//don't trade back for our own GWs...
							if (GC.getGame().GetGameCulture()->GetGreatWorkCreator(iToBeAcquiredWorkIndex) == m_pPlayer->GetID())
								continue;

							for (uint ui = 0; ui < aWorksToDiscard.size(); ui++)
							{
								GreatWorkClass eClass = GC.getGame().GetGameCulture()->GetGreatWorkClass(aWorksToDiscard[ui]);

								if (eSlotType == CvTypes::getGREAT_WORK_SLOT_LITERATURE())
								{
									if (eClass != (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE"))
										continue;
								}
								else if (eSlotType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
								{
									if (eClass != (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART") && eClass != (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT"))
										continue;
								}

								iToBeDiscardedWorkIndex = aWorksToDiscard[ui];
								break;
							}

							if (iToBeDiscardedWorkIndex == -1)
								continue;

							PlayerTypes eOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeDiscardedWorkIndex);
							//Bwa?
							if (eOwner != m_pPlayer->GetID() || eOwner == NO_PLAYER)
							{
								continue;
							}

							//We have a swappable Great Work and a target Great Work? Cool.
							if (iToBeAcquiredWorkIndex != -1 && iToBeDiscardedWorkIndex != -1)
							{
								//Do we match? We should.
								GreatWorkClass eClass1 = GC.getGame().GetGameCulture()->GetGreatWorkClass(iToBeAcquiredWorkIndex);
								GreatWorkClass eClass2 = GC.getGame().GetGameCulture()->GetGreatWorkClass(iToBeDiscardedWorkIndex);

								if (eClass1 != eClass2)
								{
									bool bArtOk = false;

									//exception! art/artifacts can be swapped
									if ((eClass1 == 1 && eClass2 == 2) || (eClass1 == 2 && eClass2 == 1))
										bArtOk = true;

									if (!bArtOk)
										continue;
								}
								EraTypes eEra = pkGameCulture->m_CurrentGreatWorks[iToBeAcquiredWorkIndex].m_eEra;
								PlayerTypes ePlayer = pkGameCulture->GetGreatWorkCreator(iToBeAcquiredWorkIndex);

								//We need to know if it matches local GWs, but we also need to know if it matches the foreign ones we've already seen... 
								if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, eEra, aErasSeen, ePlayer, aPlayersSeen, m_pPlayer->GetID()))
								{
									//If it does match, add to actual list of things to grab and discard (paired up).
									aGreatWorkPairs.push_back(iToBeAcquiredWorkIndex, iToBeDiscardedWorkIndex);
									//And add our discarded work to our list of things to discard so we don't assume it still exists.
									if (eClass2 == (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE"))
										SetSwappableWritingIndex(-1);
									else if (eClass2 == (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART"))
										SetSwappableArtIndex(-1);
									else if (eClass2 == (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT"))
										SetSwappableArtifactIndex(-1);
								}
							}
						}
					}
					//We've got at least one work swap pair? Awesome.
					if (aGreatWorkPairs.size() > 0)
					{
						for (int ui = 0; ui < aGreatWorkPairs.size(); ui++)
						{
							//Grab our data for swap and discard item.
							int iToBeAcquiredWorkIndex = aGreatWorkPairs.GetElement(ui);
							int iToBeDiscardedWorkIndex = aGreatWorkPairs.GetWeight(ui);

							//Can't move forward if either are invalid.
							if (iToBeAcquiredWorkIndex == NO_GREAT_WORK || iToBeDiscardedWorkIndex == NO_GREAT_WORK)
							{
								continue;
							}
							PlayerTypes eForeignOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeAcquiredWorkIndex);
							//Bwa?
							if (eForeignOwner == m_pPlayer->GetID() || eForeignOwner == NO_PLAYER)
							{
								continue;
							}
							PlayerTypes eOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeDiscardedWorkIndex);
							//Bwa?
							if (eOwner != m_pPlayer->GetID() || eOwner == NO_PLAYER)
							{
								continue;
							}
							//Push all the valid foreign works into our list.
							aWorksChosen.push_back(iToBeAcquiredWorkIndex);
						}
						//We've got the theme we want? Or perhaps we're building a theme?
						if (CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == iThemeID)
						{
							int iLoop = 0;
							//Now loop back through for the actual swapping.
							for (int ui = 0; ui < aGreatWorkPairs.size(); ui++)
							{
								//Grab our data for swap and discard item.
								int iToBeAcquiredWorkIndex = aGreatWorkPairs.GetElement(ui);
								int iToBeDiscardedWorkIndex = aGreatWorkPairs.GetWeight(ui);

								//Can't move forward if either are invalid.
								if (iToBeAcquiredWorkIndex == NO_GREAT_WORK || iToBeDiscardedWorkIndex == NO_GREAT_WORK)
								{
									continue;
								}
								PlayerTypes eForeignOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeAcquiredWorkIndex);
								//Bwa?
								if (eForeignOwner == m_pPlayer->GetID() || eForeignOwner == NO_PLAYER)
								{
									continue;
								}
								PlayerTypes eOwner = GC.getGame().GetGameCulture()->GetGreatWorkController(iToBeDiscardedWorkIndex);
								//Bwa?
								if (eOwner != m_pPlayer->GetID() || eOwner == NO_PLAYER)
								{
									continue;
								}

								//Let's make sure our item is in our swap slot.
								if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
								{
									if (pkBonusInfo->IsMustBeArt())
									{
										if (iToBeDiscardedWorkIndex != m_pPlayer->GetCulture()->GetSwappableArtIndex())
										{
											m_pPlayer->GetCulture()->SetSwappableArtIndex(iToBeDiscardedWorkIndex);
										}
									}
									else if (pkBonusInfo->IsMustBeArtifact())
									{
										if (iToBeDiscardedWorkIndex != m_pPlayer->GetCulture()->GetSwappableArtifactIndex())
										{
											m_pPlayer->GetCulture()->SetSwappableArtifactIndex(iToBeDiscardedWorkIndex);
										}
									}
								}
								else
								{
									if (iToBeDiscardedWorkIndex != m_pPlayer->GetCulture()->GetSwappableWritingIndex())
									{
										m_pPlayer->GetCulture()->SetSwappableWritingIndex(iToBeDiscardedWorkIndex);
									}
								}
								if (pkGameCulture->SwapGreatWorks(m_pPlayer->GetID(), iToBeDiscardedWorkIndex, eForeignOwner, iToBeAcquiredWorkIndex))
								{

									LogSwapMultipleWorks(eOwner, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);

									// Update works list for the swapped items.
									tempWorks.clear();
									if (!pkBonusInfo->IsMustBeArtifact())
									{
										for (it3 = works1.begin(); it3 != works1.end(); it3++)
										{
											if (it3->m_iGreatWorkIndex == iToBeDiscardedWorkIndex)
											{
												it3->m_iGreatWorkIndex = iToBeAcquiredWorkIndex;
												GetGreatWorkLocation(it3->m_iGreatWorkIndex, it3->m_iCityID, it3->m_eBuilding, it3->m_iSlot);
											}
											tempWorks.push_back(*it3);
										}
										works1 = tempWorks;
									}
									else
									{
										for (it3 = works2.begin(); it3 != works2.end(); it3++)
										{
											if (it3->m_iGreatWorkIndex == iToBeDiscardedWorkIndex)
											{
												it3->m_iGreatWorkIndex = iToBeAcquiredWorkIndex;
												GetGreatWorkLocation(it3->m_iGreatWorkIndex, it3->m_iCityID, it3->m_eBuilding, it3->m_iSlot);
											}
											tempWorks.push_back(*it3);
										}
										works2 = tempWorks;
									}

									CultureHelpers::SendArtSwapNotification(pkEntry->GetGreatWorkSlotType(), pkBonusInfo->IsMustBeArt(), m_pPlayer->GetID(), eForeignOwner, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);
									// Themed it through acquisition
									iLoop++;
								}
							}
							if (iLoop > 0)
							{
								bThemedProperly = true;
							}
						}
					}
				}
				if (bThemedProperly)
				{
					int iSlot = 0;
					for (int jJ = 0; jJ < (int)aWorksChosen.size(); jJ++)
					{
						for (int kK = 0; kK < (int)works1.size(); kK++)
						{
							if (works1[kK].m_iGreatWorkIndex == aWorksChosen[jJ])
							{
								if (MoveWorkIntoSlot(works1[kK], kBldg.m_iCityID, kBldg.m_eBuilding, iSlot))
								{
									iSlot++;
									break;
								}
							}
						}
						for (int kK = 0; kK < (int)works2.size(); kK++)
						{
							if (works2[kK].m_iGreatWorkIndex == aWorksChosen[jJ])
							{
								if (MoveWorkIntoSlot(works2[kK], kBldg.m_iCityID, kBldg.m_eBuilding, iSlot))
								{
									iSlot++;
									break;
								}
							}
						}
					}

					// Remove these works from those to consider later
					tempWorks.clear();
					for (it5 = works1.begin(); it5 != works1.end(); it5++)
					{
						// Copy it over if not chosen, updating its location
						if (find(aWorksChosen.begin(), aWorksChosen.end(), it5->m_iGreatWorkIndex) == aWorksChosen.end())
						{
							GetGreatWorkLocation(it5->m_iGreatWorkIndex, it5->m_iCityID, it5->m_eBuilding, it5->m_iSlot);
							tempWorks.push_back(*it5);
						}
					}
					works1 = tempWorks;

					tempWorks.clear();
					for (it5 = works2.begin(); it5 != works2.end(); it5++)
					{
						// Copy it over if not chosen, updating its location
						if (find(aWorksChosen.begin(), aWorksChosen.end(), it5->m_iGreatWorkIndex) == aWorksChosen.end())
						{
							GetGreatWorkLocation(it5->m_iGreatWorkIndex, it5->m_iCityID, it5->m_eBuilding, it5->m_iSlot);
							tempWorks.push_back(*it5);
						}
					}
					works2 = tempWorks;

					// All done
					if (CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == iThemingBonusIndex)
					{
						LogThemedBuilding(kBldg.m_iCityID, kBldg.m_eBuilding, pkBonusInfo->GetBonus());
						return true;
					}
				}
			}
		}
	}
	return false;
}

#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
bool CvPlayerCulture::MoveSingleWorks(vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, YieldTypes eFocusYield, bool bPuppet)
{
	// CUSTOMLOG("Move Single Works");
	vector<CvGreatWorkBuildingInMyEmpire>::iterator itBuilding;

	/*
	 * The order is
	 *  - fill single homeland buildings with a focused yield
	 *  - fill single puppet buildings with a focused yield
	 *  - fill single homeland buildings with any yield
	 *  - fill single puppet buildings with any yield
	 *  - fill single homeland buildings with no yield
	 *  - fill single puppet buildings with no yield
	 *  - fill single endangered buildings with a focused yield
	 *  - fill single endangered buildings with any yield
	 *  - fill single endangered buildings with no yield
	*/

	vector<CvGreatWorkBuildingInMyEmpire> homelandBuildingsFocus;
	vector<CvGreatWorkBuildingInMyEmpire> homelandBuildingsAny;
	vector<CvGreatWorkBuildingInMyEmpire> homelandBuildingsNone;
	vector<CvGreatWorkBuildingInMyEmpire> puppetBuildingsFocus;
	vector<CvGreatWorkBuildingInMyEmpire> puppetBuildingsAny;
	vector<CvGreatWorkBuildingInMyEmpire> puppetBuildingsNone;
	vector<CvGreatWorkBuildingInMyEmpire> endangeredBuildingsFocus;
	vector<CvGreatWorkBuildingInMyEmpire> endangeredBuildingsAny;
	vector<CvGreatWorkBuildingInMyEmpire> endangeredBuildingsNone;

	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); ++itBuilding)
	{
		if (!itBuilding->m_bThemed) {
			if (!itBuilding->m_bEndangered) {
				if (itBuilding->m_bPuppet && bPuppet) {
					if (itBuilding->m_eYieldType == eFocusYield) {
						puppetBuildingsFocus.push_back(*itBuilding);
					}
					else if (itBuilding->m_eYieldType != NO_YIELD) {
						puppetBuildingsAny.push_back(*itBuilding);
					}
					else {
						puppetBuildingsNone.push_back(*itBuilding);
					}
					
				} else {
					if (itBuilding->m_eYieldType == eFocusYield) {
						homelandBuildingsFocus.push_back(*itBuilding);
					}
					else if (itBuilding->m_eYieldType != NO_YIELD) {
						homelandBuildingsAny.push_back(*itBuilding);
					}
					else {
						homelandBuildingsNone.push_back(*itBuilding);
					}
				}
			} else {
				if (itBuilding->m_eYieldType == eFocusYield) {
					endangeredBuildingsFocus.push_back(*itBuilding);
				} else if (itBuilding->m_eYieldType != NO_YIELD) {
					endangeredBuildingsAny.push_back(*itBuilding);
				} else {
					endangeredBuildingsNone.push_back(*itBuilding);
				}
			}
		}
	}
	bool bUpdate = false;

	for (itBuilding = homelandBuildingsFocus.begin(); itBuilding != homelandBuildingsFocus.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = homelandBuildingsAny.begin(); itBuilding != homelandBuildingsAny.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = homelandBuildingsNone.begin(); itBuilding != homelandBuildingsNone.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = puppetBuildingsFocus.begin(); itBuilding != puppetBuildingsFocus.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = puppetBuildingsAny.begin(); itBuilding != puppetBuildingsAny.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = puppetBuildingsNone.begin(); itBuilding != puppetBuildingsNone.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = endangeredBuildingsFocus.begin(); itBuilding != endangeredBuildingsFocus.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = endangeredBuildingsAny.begin(); itBuilding != endangeredBuildingsAny.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	for (itBuilding = endangeredBuildingsNone.begin(); itBuilding != endangeredBuildingsNone.end(); ++itBuilding) {
		bUpdate = FillBuilding(itBuilding, works1, works2);
	}
	return bUpdate;
}
#endif

/// Simple version of ThemeBuilding() which just fills in a building with ANY Great Works, not ones that provide a theming bonus
bool CvPlayerCulture::FillBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator buildingIt, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2)
{
	// CUSTOMLOG("Fill building %i in city %i", ((int) (buildingIt->m_eBuilding)), buildingIt->m_iCityID);
	CvBuildingEntry *pkEntry = GC.getBuildingInfo(buildingIt->m_eBuilding);
	if (!pkEntry)
	{
		return false;
	}

	int iCountSlots = pkEntry->GetGreatWorkCount();
	if (iCountSlots < 1)
	{
		return false;

	}

	bool bUpdate = false;

	vector<CvGreatWorkInMyEmpire> worksToConsider;
	vector<int> aWorksChosen;

	worksToConsider = works1;
	vector<CvGreatWorkInMyEmpire>::const_iterator it;
	for (it = works2.begin(); it != works2.end(); ++it)
	{
		worksToConsider.push_back(*it);
	}

	it = worksToConsider.begin();
	for (int iI = 0; iI < iCountSlots && it != worksToConsider.end(); iI++, ++it)
	{
		aWorksChosen.push_back(worksToConsider[iI].m_iGreatWorkIndex);
		// CUSTOMLOG("  filling slot %i with Great Work %i", iI, worksToConsider[iI].m_iGreatWorkIndex);
		bUpdate = MoveWorkIntoSlot(worksToConsider[iI], buildingIt->m_iCityID, buildingIt->m_eBuilding, iI);
	}

	// Remove these works from those to consider later
	if (aWorksChosen.size() > 0)
	{
		vector<CvGreatWorkInMyEmpire>::iterator it2;
		vector<CvGreatWorkInMyEmpire> tempWorks;

		tempWorks.clear();
		for (it2 = works1.begin(); it2 != works1.end(); ++it2)
		{
			// Copy it over if not chosen, updating its location
			if (find(aWorksChosen.begin(), aWorksChosen.end(), it2->m_iGreatWorkIndex) == aWorksChosen.end())
			{
				GetGreatWorkLocation(it2->m_iGreatWorkIndex, it2->m_iCityID, it2->m_eBuilding, it2->m_iSlot);
				tempWorks.push_back(*it2);
			}
		}
		works1 = tempWorks;

		tempWorks.clear();
		for (it2 = works2.begin(); it2 != works2.end(); ++it2)
		{
			// Copy it over if not chosen, updating its location
			if (find(aWorksChosen.begin(), aWorksChosen.end(), it2->m_iGreatWorkIndex) == aWorksChosen.end())
			{
				GetGreatWorkLocation(it2->m_iGreatWorkIndex, it2->m_iCityID, it2->m_eBuilding, it2->m_iSlot);
				tempWorks.push_back(*it2);
			}
		}
		works2 = tempWorks;
	}

	return bUpdate;
}

/// Lower-level routine to perform the swap between two Great Works within your own empire
bool CvPlayerCulture::MoveWorkIntoSlot (CvGreatWorkInMyEmpire kWork, int iCityID, BuildingTypes eBuilding, int iSlot)
{
	CvBuildingEntry *pkToEntry = GC.getBuildingInfo(eBuilding);

	int iFromCityID = 0;
	BuildingTypes eFromBuildingType;
	int iFromSlot = 0;
	GetGreatWorkLocation(kWork.m_iGreatWorkIndex, iFromCityID, eFromBuildingType, iFromSlot);

	CvBuildingEntry *pkFromEntry = GC.getBuildingInfo(eFromBuildingType);
	if (pkToEntry && pkFromEntry)
	{
		CvCity *pToCity = m_pPlayer->getCity(iCityID);
	
		if(pToCity)
		{
			BuildingClassTypes eToBuildingClass = pkToEntry->GetBuildingClassType();
			if(eToBuildingClass != NO_BUILDINGCLASS)
			{
				CvCity *pFromCity = m_pPlayer->getCity(iFromCityID);
				BuildingClassTypes eFromBuildingClass = pkFromEntry->GetBuildingClassType();
				if(pFromCity && eFromBuildingClass != NO_BUILDINGCLASS)
				{
					gDLL->sendMoveGreatWorks(
						m_pPlayer->GetID(),
						pToCity->GetID(),
						eToBuildingClass,
						iSlot,
						pFromCity->GetID(),
						eFromBuildingClass,
						iFromSlot
					);
					return true;
				}
			}
		}
	}

	return false;
}

int CvPlayerCulture::GetSwappableWritingIndex() const
{
	return m_iSwappableWritingIndex;
}

int CvPlayerCulture::GetSwappableArtIndex() const
{
	return m_iSwappableArtIndex;
}

int CvPlayerCulture::GetSwappableArtifactIndex() const
{
	return m_iSwappableArtifactIndex;
}

int CvPlayerCulture::GetSwappableMusicIndex() const
{
	return m_iSwappableMusicIndex;
}

void CvPlayerCulture::SetSwappableWritingIndex (int iIndex)
{
	m_iSwappableWritingIndex = iIndex;
}

void CvPlayerCulture::SetSwappableArtIndex (int iIndex)
{
	m_iSwappableArtIndex = iIndex;
}

void CvPlayerCulture::SetSwappableArtifactIndex (int iIndex)
{
	m_iSwappableArtifactIndex = iIndex;
}

void CvPlayerCulture::SetSwappableMusicIndex (int iIndex)
{
	m_iSwappableMusicIndex = iIndex;
}


// ARCHAEOLOGY

/// Add to the list of plots where we have archaeologists waiting for orders
void CvPlayerCulture::AddDigCompletePlot(CvPlot* pPlot)
{
	ASSERT_DEBUG(pPlot != NULL);
	m_aDigCompletePlots.push_back(pPlot);
}

/// Remove a plot from the list of plots where we have archaeologists waiting for orders
void CvPlayerCulture::RemoveDigCompletePlot(CvPlot* pPlot)
{
	ASSERT_DEBUG(pPlot != NULL);
	vector<CvPlot*>::iterator it = std::find(m_aDigCompletePlots.begin(), m_aDigCompletePlots.end(), pPlot);
	if (it != m_aDigCompletePlots.end())
	{
		m_aDigCompletePlots.erase(it);
	}
}

/// Reset the list of plots where we have an archaeologist waiting for orders
void CvPlayerCulture::ResetDigCompletePlots()
{
	m_aDigCompletePlots.clear();
}

/// Find the next plot where we have an archaeologist waiting for orders
CvPlot* CvPlayerCulture::GetNextDigCompletePlot() const
{
	CvPlot* pRtnValue = NULL;

	if (m_aDigCompletePlots.size() > 0)
	{
		pRtnValue = m_aDigCompletePlots[0];
	}

	return pRtnValue;
}

/// Find the next archaeologist waiting for orders
CvUnit *CvPlayerCulture::GetNextDigCompleteArchaeologist(CvPlot **ppPlot) const
{
	CvUnit *pRtnValue = NULL;
	CvPlot *pPlot = GetNextDigCompletePlot();
	*ppPlot = pPlot;
	int iClosestDistance = MAX_INT;

	if (pPlot)
	{
		int iLoop = 0;
		CvUnit *pLoopUnit = NULL;
		for (pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_ARCHAEOLOGIST)
			{
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pPlot->getX(), pPlot->getY());
				if (iDistance < iClosestDistance)
				{
					pRtnValue = pLoopUnit;
					iClosestDistance = iDistance;
				}
				if (iDistance == 0) break;
			}
		}
	}

	return pRtnValue;
}

/// Is there a dig that completed at this plot?
bool CvPlayerCulture::HasDigCompleteHere(CvPlot* pPlot) const
{
	ASSERT_DEBUG(pPlot != NULL);
	return std::find(m_aDigCompletePlots.begin(), m_aDigCompletePlots.end(), pPlot) != m_aDigCompletePlots.end();
}

/// How much culture can we receive from cashing in a written artifact?
int CvPlayerCulture::GetWrittenArtifactCulture() const
{
	// Culture boost based on 8 previous turns; same as GREAT_WRITER; move to XML?
	int iValue = m_pPlayer->getYieldPerTurnHistory(YIELD_CULTURE, 8 /*iPreviousTurnsToCount */);
	// Modify based on game speed
	iValue *= GC.getGame().getGameSpeedInfo().getCulturePercent();
	iValue /= 100;

	return iValue;
}

/// AI routine - choose what to do with a completed dig site
ArchaeologyChoiceType CvPlayerCulture::GetArchaeologyChoice(CvPlot *pPlot)
{
	ArchaeologyChoiceType eRtnValue = ARCHAEOLOGY_DO_NOTHING;

	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();

	if (pPlot->HasWrittenArtifact())
	{
		eRtnValue = ARCHAEOLOGY_ARTIFACT_WRITING;
	}
	
	// No slots? Definitely go for Landmark or Cultural Renaissance
	if (!HasAvailableGreatWorkSlot(eArtArtifactSlot))
	{
		if (pPlot->HasWrittenArtifact())
		{
			eRtnValue = ARCHAEOLOGY_CULTURE_BOOST;
		}
		else
		{
			eRtnValue = ARCHAEOLOGY_LANDMARK;
		}
	}

	// Outside territory? Go for artifact ...
	else if (pPlot->getOwner() != m_pPlayer->GetID())
	{
		eRtnValue = ARCHAEOLOGY_ARTIFACT_PLAYER1;

		// ... unless this is a city state we want to influence to help with diplo victory
		if (pPlot->getOwner() != NO_PLAYER)
		{
			if (GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			{
				if (m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() && m_pPlayer->GetDiplomacyAI()->GetCivApproach(pPlot->getOwner()) > CIV_APPROACH_HOSTILE)
				{
					eRtnValue = ARCHAEOLOGY_LANDMARK;
				}
			}
			else
			{
				if (m_pPlayer->IsEmpireUnhappy() || m_pPlayer->GetDiplomacyAI()->GetCivOpinion(pPlot->getOwner()) >= CIV_OPINION_FRIEND)
					eRtnValue = ARCHAEOLOGY_LANDMARK;
			}
		}
	}

	// Not a tile a city can work?  Go for artifact
	else if (pPlot->getOwningCity() == NULL)
	{
		eRtnValue = ARCHAEOLOGY_ARTIFACT_PLAYER1;
	}

	// Otherwise go for Landmark if from Medieval Era or earlier, or if have enough other Archaeologists and Antiquity sites to fill all slots
	else if (pPlot->GetArchaeologicalRecord().m_eEra <= 2)
	{
		eRtnValue = ARCHAEOLOGY_LANDMARK;
	}

	else
	{
		int iNumArchaeologists = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true) - 1 /* For this one that just completed work */;
		int iNumSites = m_pPlayer->GetEconomicAI()->GetVisibleAntiquitySites() - 1 /* For this one then just was completed */;
		int iNumGreatWorkSlots = m_pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		int iLimitingFactor = min(iNumArchaeologists, iNumSites);

		if (iNumGreatWorkSlots <= iLimitingFactor)
		{
			if (pPlot->HasWrittenArtifact())
			{
				eRtnValue = ARCHAEOLOGY_CULTURE_BOOST;
			}
			else
			{
				eRtnValue = ARCHAEOLOGY_LANDMARK;
			}
		}
		else
		{
			eRtnValue = ARCHAEOLOGY_ARTIFACT_PLAYER1;
		}
	}

	// If artifact is chosen, would player 2's be better?
	if (eRtnValue == ARCHAEOLOGY_ARTIFACT_PLAYER1)
	{
		// For now have AI player try to collect their own artifacts
		if (pPlot->GetArchaeologicalRecord().m_ePlayer2 == m_pPlayer->GetID())
		{
			eRtnValue = ARCHAEOLOGY_ARTIFACT_PLAYER2;
		}
	}

	return eRtnValue;
}

/// Make things happen at an archaeology dig
void CvPlayerCulture::DoArchaeologyChoice (ArchaeologyChoiceType eChoice)
{
	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	BuildingClassTypes eBuildingToHouse = NO_BUILDINGCLASS;
	int iSlot = -1;
	CvCity *pHousingCity = NULL;
	CvPlot *pPlot = NULL;
	CvUnit *pUnit = GetNextDigCompleteArchaeologist(&pPlot);
	
	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	GreatWorkSlotType eWritingSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();
	GreatWorkType eGreatArtifact = CultureHelpers::GetArtifact(pPlot);
	GreatWorkClass eClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");

	switch (eChoice)
	{
	case NO_ARCHAEOLOGY_CHOICE:
	case ARCHAEOLOGY_DO_NOTHING:
		break;
	case ARCHAEOLOGY_LANDMARK:
	{
		ImprovementTypes eLandmarkImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK");
		if (eLandmarkImprovement != NO_IMPROVEMENT)
		{
			pPlot->setImprovementType(eLandmarkImprovement, m_pPlayer->GetID());
			pPlot->SetPlayerThatClearedDigHere(m_pPlayer->GetID());

			if (pUnit)
				pUnit->kill(true);

			if (pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != m_pPlayer->GetID())
			{
				CvPlayer &kOwner = GET_PLAYER(pPlot->getOwner());

				// City-state owned territory?
				if (kOwner.isMinorCiv())
				{
					pPlot->SetLandmarkCreditMinor(kOwner.GetID()); // applies resting Influence bonus

					int iExtraInfluence = /*50 in CP, 0 in VP*/ GD_INT_GET(LANDMARK_MINOR_FRIENDSHIP_CHANGE);
					if (MOD_BALANCE_VP)
					{
						int iEra = m_pPlayer->GetCurrentEra();
						if (iEra <= 0)
							iEra = 1;

						iExtraInfluence *= iEra;

						if (GD_INT_GET(MINOR_LANDMARK_RESTING_INFLUENCE) > 0)
						{
							// Influence snaps to new resting Influence if lower, then the extra Influence is added.
							int iNewRestingInfluenceTimes100 = kOwner.GetMinorCivAI()->GetFriendshipAnchorWithMajor(m_pPlayer->GetID()) * 100;
							if (iNewRestingInfluenceTimes100 <= kOwner.GetMinorCivAI()->GetBaseFriendshipWithMajorTimes100(m_pPlayer->GetID()))
								kOwner.GetMinorCivAI()->ChangeFriendshipWithMajor(m_pPlayer->GetID(), iExtraInfluence);
							else
								kOwner.GetMinorCivAI()->SetFriendshipWithMajorTimes100(m_pPlayer->GetID(), iExtraInfluence * 100 + iNewRestingInfluenceTimes100);
						}
						else
							kOwner.GetMinorCivAI()->ChangeFriendshipWithMajor(m_pPlayer->GetID(), iExtraInfluence);
					}
					else
						kOwner.GetMinorCivAI()->ChangeFriendshipWithMajor(m_pPlayer->GetID(), iExtraInfluence);
				}
				// Major civ owned territory?
				else if (kOwner.isMajorCiv())
				{
					kOwner.GetDiplomacyAI()->ChangeNumLandmarksBuiltForMe(m_pPlayer->GetID(), 1);
					kOwner.GetDiplomacyAI()->SetWaitingForDigChoice(false);
				}
			}
		}
	}
	break;
	case ARCHAEOLOGY_ARTIFACT_PLAYER1:
	{
		if (pPlot->getOwner() != NO_PLAYER && GET_PLAYER(pPlot->getOwner()).isMajorCiv())
		{
			if (pUnit && pPlot->getTeam() != pUnit->getTeam() && GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->IsWaitingForDigChoice())
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 1);
			}
			else
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->SetWaitingForDigChoice(false);
			}
		}

		pHousingCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pPlot->getX(), pPlot->getY(), eArtArtifactSlot, eBuildingToHouse, iSlot);
		int iGWindex = pCulture->CreateGreatWork(eGreatArtifact, eClass, pPlot->GetArchaeologicalRecord().m_ePlayer1, pPlot->GetArchaeologicalRecord().m_eEra, "");
#if defined(MOD_BALANCE_CORE)
		if (m_pPlayer->GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				CvCity* pLoopCity = NULL;
				int iCityLoop = 0;

				// Loop through owner's cities.
				for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
				{
					if (pLoopCity != NULL)
					{
						pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
					}
				}
				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA_GREAT_WORK");
					strText << iWLTKD << /*25*/ GD_INT_GET(WLTKD_GROWTH_MULTIPLIER);
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA_GREAT_WORK");
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), -1);
				}
			}
		}
#endif
		pHousingCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingToHouse, iSlot, iGWindex);
		if (pUnit)
			pPlot->setImprovementType(NO_IMPROVEMENT);

		pPlot->SetPlayerThatClearedDigHere(m_pPlayer->GetID());

		pHousingCity->UpdateAllNonPlotYields(true);
		if (pUnit)
			pUnit->kill(true);
	}
	break;
	case ARCHAEOLOGY_ARTIFACT_PLAYER2:
	{
		if (GET_PLAYER(pPlot->getOwner()).isMajorCiv())
		{
			if (pUnit && pPlot->getTeam() != pUnit->getTeam() && GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->IsWaitingForDigChoice())
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 1);
			}
			else
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->SetWaitingForDigChoice(false);
			}
		}
		pHousingCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pPlot->getX(), pPlot->getY(), eArtArtifactSlot, eBuildingToHouse, iSlot);
		int iGWindex = pCulture->CreateGreatWork(eGreatArtifact, eClass, pPlot->GetArchaeologicalRecord().m_ePlayer2, pPlot->GetArchaeologicalRecord().m_eEra, "");
		pHousingCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingToHouse, iSlot, iGWindex);
		if (pUnit)
			pPlot->setImprovementType(NO_IMPROVEMENT);

		pPlot->SetPlayerThatClearedDigHere(m_pPlayer->GetID());

#if defined(MOD_BALANCE_CORE)
		if (m_pPlayer->GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				CvCity* pLoopCity = NULL;
				int iCityLoop = 0;

				// Loop through owner's cities.
				for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
				{
					if (pLoopCity != NULL)
					{
						pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
					}
				}
				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA_GREAT_WORK");
					strText << iWLTKD << /*25*/ GD_INT_GET(WLTKD_GROWTH_MULTIPLIER);
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA_GREAT_WORK");
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), -1);
				}
			}
		}
#endif
		pHousingCity->UpdateAllNonPlotYields(true);

		if (pUnit)
			pUnit->kill(true);
	}
	break;

	case ARCHAEOLOGY_ARTIFACT_WRITING:
	{
		if (GET_PLAYER(pPlot->getOwner()).isMajorCiv())
		{
			if (pUnit && pPlot->getTeam() != pUnit->getTeam() && GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->IsWaitingForDigChoice())
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 1);
			}
			else
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->SetWaitingForDigChoice(false);
			}
		}
		pHousingCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pPlot->getX(), pPlot->getY(), eWritingSlot, eBuildingToHouse, iSlot);
		int iGWindex = pCulture->CreateGreatWork(eGreatArtifact, (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE"), pPlot->GetArchaeologicalRecord().m_ePlayer1, pPlot->GetArchaeologicalRecord().m_eEra, "");
		pHousingCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingToHouse, iSlot, iGWindex);
		if (pUnit)
			pPlot->setImprovementType(NO_IMPROVEMENT);

		pPlot->SetPlayerThatClearedDigHere(m_pPlayer->GetID());

		if (m_pPlayer->GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				CvCity* pLoopCity = NULL;
				int iCityLoop = 0;

				// Loop through owner's cities.
				for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
				{
					if (pLoopCity != NULL)
					{
						pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
					}
				}
				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA_GREAT_WORK");
					strText << iWLTKD;
					strText << /*25*/ GD_INT_GET(WLTKD_GROWTH_MULTIPLIER);
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA_GREAT_WORK");
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), -1);
				}
			}
		}

		pHousingCity->UpdateAllNonPlotYields(true);

		if (pUnit)
			pUnit->kill(true);
	}
	break;

	case ARCHAEOLOGY_CULTURE_BOOST:
	{
		if (GET_PLAYER(pPlot->getOwner()).isMajorCiv())
		{
			if (pUnit && pPlot->getTeam() != pUnit->getTeam() && GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->IsWaitingForDigChoice())
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 1);
			}
			else
			{
				GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->SetWaitingForDigChoice(false);
			}
		}

		// Culture boost based on 8 previous turns; same as GREAT_WRITER; move to XML?
		int iValue = m_pPlayer->getYieldPerTurnHistory(YIELD_CULTURE, 8 /*iPreviousTurnsToCount */);

		// Modify based on game speed
		iValue *= GC.getGame().getGameSpeedInfo().getCulturePercent();
		iValue /= 100;

		m_pPlayer->changeJONSCulture(iValue);
		if (pPlot->getEffectiveOwningCity() != NULL && pPlot->getOwner() == m_pPlayer->GetID())
			pPlot->getEffectiveOwningCity()->ChangeJONSCultureStored(iValue);

		if (pUnit)
			pPlot->setImprovementType(NO_IMPROVEMENT);

		pPlot->SetPlayerThatClearedDigHere(m_pPlayer->GetID());

		if (pUnit)
			pUnit->kill(true);
	}
	break;
	}

	m_pPlayer->SetNumArchaeologyChoices(m_pPlayer->GetNumArchaeologyChoices() - 1);
	m_pPlayer->GetCulture()->RemoveDigCompletePlot(pPlot);

	if (m_pPlayer->GetArchaeologicalDigTourism() > 0)
	{
		int iTourism = m_pPlayer->GetHistoricEventTourism(HISTORIC_EVENT_DIG);
		m_pPlayer->ChangeNumHistoricEvents(HISTORIC_EVENT_DIG, 1);
		m_pPlayer->GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
		if(iTourism > 0)
		{
			if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
			{
				char text[256] = {0};
				sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourism);
				SHOW_PLOT_POPUP(pPlot, m_pPlayer->GetID(), text);

				CvNotifications* pNotification = m_pPlayer->GetNotifications();
				if(pNotification)
				{
					CvString strMessage;
					CvString strSummary;
					strMessage = GetLocalizedText("TXT_KEY_TOURISM_EVENT_ARCHAEOLOGY", iTourism);
					strSummary = GetLocalizedText("TXT_KEY_TOURISM_EVENT_SUMMARY");
					pNotification->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strMessage, strSummary, pPlot->getX(), pPlot->getY(), m_pPlayer->GetID());
				}
			}
		}
	}
	else
	{
		m_pPlayer->DoDifficultyBonus(HISTORIC_EVENT_DIG);
	}
	pPlot->setResourceType(NO_RESOURCE, 0);
}

// CULTURAL INFLUENCE

/// Update cultural influence numbers for this turn
void CvPlayerCulture::DoTurn()
{
	int iInfluentialCivsForWin = GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin();

	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		// Move over last turn's numbers
		m_aiLastTurnCulturalInfluence[iLoopPlayer] = m_aiCulturalInfluence[iLoopPlayer];

		CvPlayer &kOtherPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		CvTeam &kOtherTeam = GET_TEAM(kOtherPlayer.getTeam());
		if (iLoopPlayer != m_pPlayer->GetID() && kOtherPlayer.isAlive() && !kOtherPlayer.isMinorCiv() && kOtherTeam.isHasMet(m_pPlayer->getTeam()))
		{
			m_aiCulturalInfluence[iLoopPlayer] += GetInfluencePerTurn((PlayerTypes)iLoopPlayer);
		}

		//and store off our delta
		m_aiLastTurnCulturalIPT[iLoopPlayer] = m_aiCulturalInfluence[iLoopPlayer] - m_aiLastTurnCulturalInfluence[iLoopPlayer];
	}

	SetBoredomCache(m_pPlayer->GetUnhappinessFromBoredom());
	
	DoPublicOpinion();

	CvString strSummary;
	CvString strInfo;

	int iThisTurnInfluentialCivs = GetNumCivsInfluentialOn();

	VictoryTypes eVictory = (VictoryTypes) GC.getInfoTypeForString("VICTORY_CULTURAL", true);
	const bool bCultureVictoryValid = (eVictory == NO_VICTORY || GC.getGame().isVictoryValid(eVictory));

	if (iThisTurnInfluentialCivs > 0 && !GC.getGame().GetGameCulture()->GetReportedSomeoneInfluential())
	{
		if(bCultureVictoryValid)
		{//This civilization is the first civ to be influential over another civs.  Notify the masses!
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL");
			CvString							targFirstInfluentialInfo = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL_ACTIVE_PLAYER_TT");
			Localization::String	someoneFirstInfluentialInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL_TT");
			CvString							unmetFirstInfluentialInfo = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_UNMET_INFLUENTIAL_TT");

			for(int iNotifyPlayerID = 0; iNotifyPlayerID < MAX_MAJOR_CIVS; ++iNotifyPlayerID){
				PlayerTypes eNotifyPlayer = (PlayerTypes) iNotifyPlayerID;
				CvPlayerAI& kNotifyPlayer = GET_PLAYER(eNotifyPlayer);

				if (m_pPlayer->GetID() == eNotifyPlayer){	
					strInfo = targFirstInfluentialInfo;
				}
				else
				{
					CvTeam& kCurTeam = GET_TEAM(kNotifyPlayer.getTeam());
					if(kCurTeam.isHasMet(m_pPlayer->getTeam())){
						Localization::String strTemp = someoneFirstInfluentialInfo;
						strTemp << m_pPlayer->getCivilizationShortDescriptionKey();
						strInfo = strTemp.toUTF8();
					}
					else
					{
						strInfo = unmetFirstInfluentialInfo;
					}					
				}
				kNotifyPlayer.GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strInfo, strSummary, -1, -1, m_pPlayer->GetID());
			}
		}

		GC.getGame().GetGameCulture()->SetReportedSomeoneInfluential(true);
	}

	if (!m_bReportedTwoCivsAway && iThisTurnInfluentialCivs > 0 && iThisTurnInfluentialCivs == iInfluentialCivsForWin - 2 && GC.getGame().GetNumMajorCivsEver() >= 4)
	{
		if(bCultureVictoryValid)
		{//This civilization is the first civ to be two civilizations away from getting a cultural victory.  Notify the masses!
			CvString							targCloseTwoSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER");
			Localization::String	targCloseTwoInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER_TT");
			CvString							someoneCloseTwoSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO");
			Localization::String	someoneCloseTwoInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_TT");
			CvString							unmetCloseTwoInfo = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_UNMET_TT");
			for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
				PlayerTypes eCurPlayer = (PlayerTypes) iNotifyLoop;
				CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);

				if (eCurPlayer == m_pPlayer->GetID()){	
					strSummary = targCloseTwoSummary;
					Localization::String strTemp = targCloseTwoInfo;
					strTemp << m_pPlayer->getCivilizationShortDescriptionKey();
					strInfo = strTemp.toUTF8();
					kCurPlayer.GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER, strInfo, strSummary, -1, -1, m_pPlayer->GetID());
				}
				else{
					strSummary = someoneCloseTwoSummary;
					CvTeam& kCurTeam = GET_TEAM(kCurPlayer.getTeam());
					if(kCurTeam.isHasMet(m_pPlayer->getTeam())){
						Localization::String strTemp = someoneCloseTwoInfo;
						strTemp << m_pPlayer->getCivilizationShortDescriptionKey();
						strInfo = strTemp.toUTF8();
					}
					else{
						strInfo = unmetCloseTwoInfo;
					}

					kCurPlayer.GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO, strInfo, strSummary, -1, -1, m_pPlayer->GetID());
				}
			}
		}

		m_bReportedTwoCivsAway = true;
	}

	if (!m_bReportedOneCivAway && iThisTurnInfluentialCivs == iInfluentialCivsForWin - 1 && GC.getGame().GetNumMajorCivsEver() >= 3)
	{
		if(bCultureVictoryValid)
		{//This civilization is the first civ to be one civilizations away from getting a cultural victory.  Notify the masses!

		  // but don't notify if there are only two players left in the game!
		  if (GC.getGame().GetNumMajorCivsAlive() > 2) 
		  {
			CvString							targCloseOneSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER");
			Localization::String	targCloseOneInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER_TT");
			CvString							someoneCloseOneSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE");
			Localization::String	someoneCloseOneInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_TT");
			CvString							unmetCloseOneInfo = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_UNMET_TT");
			for(int iNotifyLoop = 0; iNotifyLoop < MAX_MAJOR_CIVS; ++iNotifyLoop){
				PlayerTypes eCurPlayer = (PlayerTypes) iNotifyLoop;
				CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);

				if (eCurPlayer == m_pPlayer->GetID()){	
					strSummary = targCloseOneSummary;
					Localization::String strTemp = targCloseOneInfo;
					strTemp << m_pPlayer->getCivilizationShortDescriptionKey();
					strInfo = strTemp.toUTF8();
					kCurPlayer.GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER, strInfo, strSummary, -1, -1, m_pPlayer->GetID());
				}
				else
				{
					strSummary = someoneCloseOneSummary;
					CvTeam& kCurTeam = GET_TEAM(kCurPlayer.getTeam());
					if(kCurTeam.isHasMet(m_pPlayer->getTeam())){
						Localization::String strTemp = someoneCloseOneInfo;
						strTemp << m_pPlayer->getCivilizationShortDescriptionKey();
						strInfo = strTemp.toUTF8();
					}
					else{
						strInfo = unmetCloseOneInfo;
					}
					kCurPlayer.GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE, strInfo, strSummary, -1, -1, m_pPlayer->GetID());
				}
			}
		  }
		}

		m_bReportedOneCivAway = true;
	}

	//Tourism Update
	strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_SUMMARY");
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvString strOurInfluenceInfo = "";
		CvString strOurInfluenceInfoBad = "";
		PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;
		if(ePlayer == NO_PLAYER)
		{
			continue;
		}
		if(ePlayer == m_pPlayer->GetID())
		{
			continue;
		}
		CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);
		CvTeam &kOtherTeam = GET_TEAM(kOtherPlayer.getTeam());
		if(!kOtherTeam.isHasMet(m_pPlayer->getTeam()))
		{
			continue;
		}
		if(kOtherTeam.GetID() == m_pPlayer->getTeam())
		{
			continue;
		}
		if(kOtherPlayer.isMinorCiv())
		{
			continue;
		}
		if(!kOtherPlayer.isAlive())
		{
			continue;
		}
		
		int iInfluenceOn = GetInfluenceOn(ePlayer);
		int iLifetimeCulture = kOtherPlayer.GetJONSCultureEverGenerated();
		int iPercent = 0;

		if (iLifetimeCulture > 0)
		{
			iPercent = iInfluenceOn * 100 / iLifetimeCulture;
		}

		int iInfluenceOnLastTurn = GetLastTurnInfluenceOn(ePlayer);
		int iLifetimeCultureLastTurn = kOtherPlayer.GetCulture()->GetLastTurnLifetimeCulture();
		int iPercentLastTurn = 0;

		if (iLifetimeCultureLastTurn > 0)
		{
			iPercentLastTurn = iInfluenceOnLastTurn * 100 / iLifetimeCultureLastTurn;
		}
		//Are we influential this turn, but weren't last turn? Notification!
		if (iPercent >= /*200*/ GD_INT_GET(CULTURE_LEVEL_DOMINANT) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_DOMINANT))
		{
			strOurInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_5", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent >= /*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL))
		{
			strOurInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_4", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent >= /*60 in CP, 50 in VP*/ GD_INT_GET(CULTURE_LEVEL_POPULAR) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_POPULAR))
		{
			strOurInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_3", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent >= /*30 in CP, 25 in VP*/ GD_INT_GET(CULTURE_LEVEL_FAMILIAR) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_FAMILIAR))
		{
			strOurInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_2", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent >= /*10 in CP, 5 in VP*/ GD_INT_GET(CULTURE_LEVEL_EXOTIC) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_EXOTIC))
		{
			strOurInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_1", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		if(!strOurInfluenceInfo.empty())
		{
			SetLastUpdate(GC.getGame().getGameTurn());
			m_pPlayer->GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strOurInfluenceInfo, strSummary, -1, -1, m_pPlayer->GetID());
		}

		//Were we influential last turn, but now we aren't? Notification!
		if (iPercent < /*200*/ GD_INT_GET(CULTURE_LEVEL_DOMINANT) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_DOMINANT))
		{
			strOurInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_5_BAD", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent < /*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL))
		{
			strOurInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_4_BAD", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent < /*60 in CP, 50 in VP*/ GD_INT_GET(CULTURE_LEVEL_POPULAR) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_POPULAR))
		{
			strOurInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_3_BAD", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent < /*30 in CP, 25 in VP*/ GD_INT_GET(CULTURE_LEVEL_FAMILIAR) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_FAMILIAR))
		{
			strOurInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_2_BAD", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		else if (iPercent < /*10 in CP, 5 in VP*/ GD_INT_GET(CULTURE_LEVEL_EXOTIC) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_EXOTIC))
		{
			strOurInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_US_1_BAD", kOtherPlayer.getCivilizationShortDescriptionKey());
		}
		if(!strOurInfluenceInfoBad.empty())
		{
			SetLastUpdate(GC.getGame().getGameTurn());
			m_pPlayer->GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO, strOurInfluenceInfoBad, strSummary, -1, -1, m_pPlayer->GetID());
		}
	}
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvString strTheirInfluenceInfo = "";
		CvString strTheirInfluenceInfoBad = "";
		PlayerTypes ePlayer = (PlayerTypes)iLoopPlayer;
		if(ePlayer == NO_PLAYER)
		{
			continue;
		}
		if(ePlayer == m_pPlayer->GetID())
		{
			continue;
		}
		CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);
		CvTeam &kOtherTeam = GET_TEAM(kOtherPlayer.getTeam());
		if(!kOtherTeam.isHasMet(m_pPlayer->getTeam()))
		{
			continue;
		}
		if(kOtherTeam.GetID() == m_pPlayer->getTeam())
		{
			continue;
		}
		if(kOtherPlayer.isMinorCiv())
		{
			continue;
		}
		if(!kOtherPlayer.isAlive())
		{
			continue;
		}
		
		int iInfluenceOn = kOtherPlayer.GetCulture()->GetInfluenceOn(m_pPlayer->GetID());
		int iLifetimeCulture = m_pPlayer->GetJONSCultureEverGenerated();
		int iPercent = 0;

		if (iLifetimeCulture > 0)
		{
			iPercent = iInfluenceOn * 100 / iLifetimeCulture;
		}

		int iInfluenceOnLastTurn = kOtherPlayer.GetCulture()->GetLastTurnInfluenceOn(m_pPlayer->GetID());
		int iLifetimeCultureLastTurn = m_pPlayer->GetCulture()->GetLastTurnLifetimeCulture();
		int iPercentLastTurn = 0;

		if (iLifetimeCultureLastTurn > 0)
		{
			iPercentLastTurn = iInfluenceOnLastTurn * 100 / iLifetimeCultureLastTurn;
		}
		//Are they influential this turn, but weren't last turn? Notification!
		if (iPercent >= /*200*/ GD_INT_GET(CULTURE_LEVEL_DOMINANT) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_DOMINANT))
		{
			strTheirInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_5", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent >= /*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL))
		{
			strTheirInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_4", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent >= /*60 in CP, 50 in VP*/ GD_INT_GET(CULTURE_LEVEL_POPULAR) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_POPULAR))
		{
			strTheirInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_3", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent >= /*30 in CP, 25 in VP*/ GD_INT_GET(CULTURE_LEVEL_FAMILIAR) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_FAMILIAR))
		{
			strTheirInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_2", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent >= /*10 in CP, 5 in VP*/ GD_INT_GET(CULTURE_LEVEL_EXOTIC) && iPercentLastTurn < GD_INT_GET(CULTURE_LEVEL_EXOTIC))
		{
			strTheirInfluenceInfo +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_1", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		if(!strTheirInfluenceInfo.empty())
		{
			SetLastThemUpdate(GC.getGame().getGameTurn());
			m_pPlayer->GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO, strTheirInfluenceInfo, strSummary, -1, -1, m_pPlayer->GetID());
		}
		//Were they influential last turn, but now they aren't? Notification!
		if (iPercent < /*200*/ GD_INT_GET(CULTURE_LEVEL_DOMINANT) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_DOMINANT))
		{
			strTheirInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_5_BAD", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent < /*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL))
		{
			strTheirInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_4_BAD", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent < /*60 in CP, 50 in VP*/ GD_INT_GET(CULTURE_LEVEL_POPULAR) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_POPULAR))
		{
			strTheirInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_3_BAD", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent < /*30 in CP, 25 in VP*/ GD_INT_GET(CULTURE_LEVEL_FAMILIAR) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_FAMILIAR))
		{
			strTheirInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_2_BAD", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		else if (iPercent < /*10 in CP, 5 in VP*/ GD_INT_GET(CULTURE_LEVEL_EXOTIC) && iPercentLastTurn >= GD_INT_GET(CULTURE_LEVEL_EXOTIC))
		{
			strTheirInfluenceInfoBad +=  GetLocalizedText("TXT_KEY_INFLUENCE_THEM_1_BAD", kOtherPlayer.getCivilizationAdjectiveKey());
		}
		if(!strTheirInfluenceInfoBad.empty())
		{
			SetLastThemUpdate(GC.getGame().getGameTurn());
			m_pPlayer->GetNotifications()->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strTheirInfluenceInfoBad, strSummary, -1, -1, m_pPlayer->GetID());
		}
	}

	if (MOD_API_ACHIEVEMENTS && m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		// check for having city-state artifacts
		std::vector<int> aiCityStateArtifact;
		int iLoop = 0;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
				const BuildingTypes eBuilding = pLoopCity->GetBuildingTypeFromClass(eBuildingClass);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;

				if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						if (iGreatWorkIndex != NO_GREAT_WORK)
						{
							bool bDupe = false;
							PlayerTypes eCreatingPlayer = GC.getGame().GetGameCulture()->GetGreatWorkCreator(iGreatWorkIndex);
							if (GET_PLAYER(eCreatingPlayer).isMinorCiv())
							{
								for (uint ui = 0; ui < aiCityStateArtifact.size(); ui++)
								{
									if (aiCityStateArtifact[ui] == eCreatingPlayer)
									{
										bDupe = true;
										break;
									}
								}

								if (!bDupe)
								{
									aiCityStateArtifact.push_back(eCreatingPlayer);
									if (aiCityStateArtifact.size() >= 10)
									{
										gDLL->UnlockAchievement(ACHIEVEMENT_XP2_35);
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		// check for having broadcast towers filled
		PolicyBranchTypes eCurrentBranchType = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();

		if (eCurrentBranchType == (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_FREEDOM))
		{
			int iNumWorksInBroadcastTowers = 0;
			for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(GC.getInfoTypeForString("BUILDINGCLASS_BROADCAST_TOWER"));
				const BuildingTypes eBuilding = pLoopCity->GetBuildingTypeFromClass(eBuildingClass);
				if (eBuilding == NO_BUILDING)
					continue;

				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if (!pkBuildingInfo)
					continue;

				if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
					bool bAnySlotEmpty = false;
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						if (iGreatWorkIndex == NO_GREAT_WORK)
						{
							bAnySlotEmpty = true;
						}
					}

					if (!bAnySlotEmpty)
					{
						iNumWorksInBroadcastTowers++;
					}
				}
			}
			if (iNumWorksInBroadcastTowers >= 10)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_38);
			}
		}
	}	

	LogCultureData();
}

int CvPlayerCulture::GetLastUpdate() const
{
	return m_iLastUpdate;
}

void CvPlayerCulture::SetLastUpdate(int iValue)
{
	m_iLastUpdate = iValue;
}


int CvPlayerCulture::GetLastThemUpdate() const
{
	return m_iLastThemUpdate;
}


void CvPlayerCulture::SetLastThemUpdate(int iValue)
{
	m_iLastThemUpdate = iValue;
}

void CvPlayerCulture::SetBoredomCache(int iValue)
{
	if (GC.getGame().getGameTurn() != m_iBoredomCacheTurn)
	{
		m_iBoredomCacheTurn = GC.getGame().getGameTurn();
		m_iBoredomCache = iValue;
	}
}
int CvPlayerCulture::GetBoredomCache() const
{
	return m_iBoredomCache;
}

/// What was our total culture generated throughout the game last turn?
int CvPlayerCulture::GetLastTurnLifetimeCulture() const
{
	return m_iLastTurnLifetimeCulture;
}

/// Set our total culture generated throughout the game  - last turn's number
void CvPlayerCulture::SetLastTurnLifetimeCulture(int iValue)
{
	m_iLastTurnLifetimeCulture = iValue;
}

/// What was our total culture generated throughout the game last turn?
int CvPlayerCulture::GetLastTurnCPT() const
{
	return m_iLastTurnCPT;
}

/// Set our total culture generated throughout the game  - last turn's number
void CvPlayerCulture::SetLastTurnCPT(int iValue)
{
	m_iLastTurnCPT = iValue;
}

/// What is our cultural influence now?
int CvPlayerCulture::GetInfluenceOn(PlayerTypes ePlayer) const
{
	ASSERT_DEBUG(ePlayer >= 0, "Invalid player index");
	ASSERT_DEBUG(ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS)
		return 0;

	return m_aiCulturalInfluence[iIndex];
}

// What is our cultural influence now?
void CvPlayerCulture::ChangeInfluenceOn(PlayerTypes ePlayer, int iValue)
{
	ASSERT_DEBUG(ePlayer >= 0, "Invalid player index");
	ASSERT_DEBUG(ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS)
		return;

	m_aiCulturalInfluence[iIndex] = m_aiCulturalInfluence[iIndex] + iValue;

	//store off this data
	m_pPlayer->changeInstantTourismPerPlayerValue(ePlayer, iValue);
}

int CvPlayerCulture::ChangeInfluenceOn(PlayerTypes eOtherPlayer, int iBaseInfluence, bool bApplyModifiers /* = false */, bool bModifyForGameSpeed /* = true */)
{
	int iInfluence = iBaseInfluence;
	
	if (bModifyForGameSpeed)
	{
		iInfluence = iInfluence * GC.getGame().getGameSpeedInfo().getCulturePercent() / 100;
	}

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if (bApplyModifiers && pCapital)
	{
		int iModifier = pCapital->GetCityCulture()->GetTourismMultiplier(eOtherPlayer, false, false, false, false, false);
		iInfluence = iInfluence * (100 + iModifier) / 100;
	}

	if (iInfluence != 0)
	{
		ChangeInfluenceOn(eOtherPlayer, iInfluence);
	}

	return iInfluence;
 }

/// What was our cultural influence last turn?
int CvPlayerCulture::GetLastTurnInfluenceOn(PlayerTypes ePlayer) const
{
	ASSERT_DEBUG(ePlayer >= 0, "Invalid player index");
	ASSERT_DEBUG(ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS) return 0;
	return m_aiLastTurnCulturalInfluence[iIndex];
}

/// What was our cultural influence last turn?
int CvPlayerCulture::GetLastTurnInfluenceIPT(PlayerTypes ePlayer) const
{
	ASSERT_DEBUG(ePlayer >= 0, "Invalid player index");
	ASSERT_DEBUG(ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS) return 0;
	return m_aiLastTurnCulturalIPT[iIndex];
}

/// Influence being applied each turn
int CvPlayerCulture::GetInfluencePerTurn(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	int iModifier = 0;

	CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);
	CvTeam &kOtherTeam = GET_TEAM(kOtherPlayer.getTeam());

	if ((int)ePlayer != m_pPlayer->GetID() && kOtherPlayer.isAlive() && !kOtherPlayer.isMinorCiv() && kOtherTeam.isHasMet(m_pPlayer->getTeam()))
	{
		// check to see if the other player has the Great Firewall
		bool bTargetHasGreatFirewall = m_pPlayer->IsNullifyInfluenceModifier();

		int iLoopCity = 0;
		CvCity *pLoopCity = NULL;

		// only check for firewall if the internet influence spread modifier is > 0
		int iTechSpreadModifier = m_pPlayer->GetInfluenceSpreadModifier();

		int iInfluenceToAdd = 0;
		// Loop through each of our cities
		for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			// Design has changed so modifier is always player-to-player so only need to get it once and can apply it at the end
			if (pLoopCity->isCapital())
			{
				iModifier = pLoopCity->GetCityCulture()->GetTourismMultiplier(kOtherPlayer.GetID(), false, false, false, false, false);
			}

			iInfluenceToAdd += pLoopCity->GetBaseTourism();

			// if we have the internet online, check to see if the opponent has the firewall
			// if they have the firewall, deduct the internet bonus from them
			if (iTechSpreadModifier != 0 && bTargetHasGreatFirewall)
			{
				int iInfluenceWithoutModifier = pLoopCity->GetBaseTourismBeforeModifiers();
				int iInfluenceWithTechModifier = iInfluenceWithoutModifier * iTechSpreadModifier;
				iInfluenceToAdd -= (iInfluenceWithTechModifier / 100);
			}
		}

		iRtnValue += iInfluenceToAdd;

		int iExtraInfluenceToAdd = 0;

		// Tourism from religion
		iExtraInfluenceToAdd += m_pPlayer->GetYieldPerTurnFromReligion(YIELD_TOURISM) * 100;

		// Trait bonus which adds Tourism for trade partners? 
		iExtraInfluenceToAdd += m_pPlayer->GetYieldPerTurnFromTraits(YIELD_TOURISM) * 100;
		
		if (iExtraInfluenceToAdd != 0)
		{
			// if we have the internet online, check to see if the opponent has the firewall
			// if they have the firewall, deduct the internet bonus from them
			if (iTechSpreadModifier != 0 && bTargetHasGreatFirewall)
			{
				iExtraInfluenceToAdd -= ((iExtraInfluenceToAdd * iTechSpreadModifier) / 100);
			}
		}

		iRtnValue += m_pPlayer->GetYieldPerTurnFromEspionageEvents(YIELD_TOURISM, true) * 100;
		iRtnValue -= m_pPlayer->GetYieldPerTurnFromEspionageEvents(YIELD_TOURISM, false) * 100;
		iRtnValue = max(0, iRtnValue);

		iRtnValue += iExtraInfluenceToAdd;

		iRtnValue = iRtnValue * (100 + iModifier) / 100;
	}

	return iRtnValue / 100;

}

/// Current influence level on this player
InfluenceLevelTypes CvPlayerCulture::GetInfluenceLevel(PlayerTypes ePlayer) const
{
	InfluenceLevelTypes eRtnValue;

	CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);
	CvTeam &kOtherTeam = GET_TEAM(kOtherPlayer.getTeam());

	if (ePlayer == m_pPlayer->GetID())
		return NO_INFLUENCE_LEVEL;

	if (!kOtherTeam.isHasMet(m_pPlayer->getTeam()))
	{
		eRtnValue = NO_INFLUENCE_LEVEL;
	}
	else
	{
		int iInfluenceOn = GetInfluenceOn(ePlayer);
		int iLifetimeCulture = kOtherPlayer.GetJONSCultureEverGenerated();
		int iPercent = 0;

		if (iLifetimeCulture > 0)
		{
			iPercent = iInfluenceOn * 100 / iLifetimeCulture;
		}

		eRtnValue = INFLUENCE_LEVEL_UNKNOWN;

		if (iPercent >= /*200*/ GD_INT_GET(CULTURE_LEVEL_DOMINANT))
		{
			eRtnValue = INFLUENCE_LEVEL_DOMINANT;
		}
		else if (iPercent >= /*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL))
		{
			eRtnValue = INFLUENCE_LEVEL_INFLUENTIAL;
		}
		else if (iPercent >= /*60 in CP, 50 in VP*/ GD_INT_GET(CULTURE_LEVEL_POPULAR))
		{
			eRtnValue = INFLUENCE_LEVEL_POPULAR;
		}
		else if (iPercent >= /*30 in CP, 25 in VP*/ GD_INT_GET(CULTURE_LEVEL_FAMILIAR))
		{
			eRtnValue = INFLUENCE_LEVEL_FAMILIAR;
		}
		else if (iPercent >= /*10 in CP, 5 in VP*/ GD_INT_GET(CULTURE_LEVEL_EXOTIC))
		{
			eRtnValue = INFLUENCE_LEVEL_EXOTIC;
		}
	}

	return eRtnValue;
}

/// Current influence trend on this player
InfluenceLevelTrend CvPlayerCulture::GetInfluenceTrend(PlayerTypes ePlayer) const
{
	//looking up average yields is expensive, so we cache the last result
	map<PlayerTypes, pair<int, InfluenceLevelTrend>>::const_iterator it = m_influenceTrendCache.find(ePlayer);
	if (it != m_influenceTrendCache.end() && it->second.first == GC.getGame().getTurnSlice())
		return it->second.second;

	InfluenceLevelTrend eRtnValue = INFLUENCE_TREND_STATIC;
	CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);

	float iTheirCultureThisTurn = (float)m_pPlayer->GetCulture()->GetOtherPlayerCulturePerTurnIncludingInstant(ePlayer) + (float)kOtherPlayer.GetJONSCultureEverGenerated();
	float iTheirCultureLastTurn = (float)kOtherPlayer.GetCulture()->GetLastTurnLifetimeCulture() + kOtherPlayer.GetCulture()->GetLastTurnCPT();

	float iOurTourismThisTurn = (float)GetInfluenceOn(ePlayer) + (float)m_pPlayer->GetCulture()->GetTourismPerTurnIncludingInstant(ePlayer);
	float iOurTourismLastTurn = (float)GetLastTurnInfluenceOn(ePlayer) + (float)m_pPlayer->GetCulture()->GetLastTurnInfluenceIPT(ePlayer);

	if (iTheirCultureLastTurn <= 0)
		iTheirCultureLastTurn = 1;
	if (iTheirCultureThisTurn <= 0)
		iTheirCultureThisTurn = 1;

	float iLastTurnPercent = (iOurTourismLastTurn * 100) / iTheirCultureLastTurn;
	float iThisTurnPercent = (iOurTourismThisTurn * 100) / iTheirCultureThisTurn;

	float iDiff = fabs(iLastTurnPercent - iThisTurnPercent);
	if (iDiff <= .1f)
		return eRtnValue;

	else if (iLastTurnPercent < iThisTurnPercent)
	{
		eRtnValue = INFLUENCE_TREND_RISING;
	}
	else if (iLastTurnPercent > iThisTurnPercent)
	{
		eRtnValue = INFLUENCE_TREND_FALLING;
	}

	//update the cache
	m_influenceTrendCache[ePlayer] = make_pair(GC.getGame().getTurnSlice(), eRtnValue);
	return eRtnValue;
}

int CvPlayerCulture::GetOtherPlayerCulturePerTurnIncludingInstant(PlayerTypes eOtherPlayer)
{
	int iBase = GET_PLAYER(eOtherPlayer).GetTotalJONSCulturePerTurn();
	int iEndTurn = GC.getGame().getGameTurn();
	int iStartTurn = GC.getGame().getGameTurn() - INSTANT_YIELD_HISTORY_LENGTH;

	return iBase + GET_PLAYER(eOtherPlayer).getInstantYieldAvg(YIELD_CULTURE, iStartTurn, iEndTurn);
}


int CvPlayerCulture::GetTourismPerTurnIncludingInstant(PlayerTypes ePlayer, bool bJustInstant)
{
	int iBase = 0;
	if (!bJustInstant)
		iBase = GetInfluencePerTurn(ePlayer);

	int iEndTurn = GC.getGame().getGameTurn();
	int iStartTurn = GC.getGame().getGameTurn() - INSTANT_YIELD_HISTORY_LENGTH;

	int iAvgGlobal = m_pPlayer->getInstantYieldAvg(YIELD_TOURISM, iStartTurn, iEndTurn);
	int iAvgIndividual = m_pPlayer->getInstantTourismPerPlayerAvg(ePlayer, iStartTurn, iEndTurn);

	return iBase + iAvgGlobal + iAvgIndividual;
}

/// If influence is rising, how many turns until we get to Influential? (999 if not rising fast enough to make it there eventually)
int CvPlayerCulture::GetTurnsToInfluential(PlayerTypes ePlayer)
{
	CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);
	int iRtnValue = 999;

	if (GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_INFLUENTIAL)
	{
		return 0;
	}
	else if (GetInfluenceTrend(ePlayer) == INFLUENCE_TREND_RISING)
	{
		int iInfluence = GetInfluenceOn(ePlayer);
		int iInflPerTurn = GetTourismPerTurnIncludingInstant(ePlayer);
		if (iInflPerTurn == 0)
			return 999;

		int iCulture = kOtherPlayer.GetJONSCultureEverGenerated();
		int iCultPerTurn = GetOtherPlayerCulturePerTurnIncludingInstant(ePlayer);

		int iNumerator = (/*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL) * iCulture / 100) - iInfluence;
		int iDivisor = iInflPerTurn - (/*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL) * iCultPerTurn / 100);

		iRtnValue = iNumerator / max(1, iDivisor);

		// Round up
		if (iNumerator % max(1, iDivisor) != 0)
		{
			iRtnValue++;
		}

		if (iRtnValue <= 0 || iRtnValue >= 999)
			return 999;
	}

	return iRtnValue;
}

/// How many civs do we have influence culture over?
int CvPlayerCulture::GetNumCivsInfluentialOn() const
{
	int iRtnValue = 0;
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
		{
			if (GetInfluenceLevel(kPlayer.GetID()) >= INFLUENCE_LEVEL_INFLUENTIAL)
			{
				iRtnValue++;
			}
		}
	}
	return iRtnValue;
}

/// How many other civs are in the game?
int CvPlayerCulture::GetNumCivsToBeInfluentialOn() const
{
	int iRtnValue = 0;

	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

/// Which civ is the one we need to do the most work on to get to Influential culture?
PlayerTypes CvPlayerCulture::GetCivLowestInfluence(bool bCheckOpenBorders) const
{
	PlayerTypes eRtnValue = NO_PLAYER;
	int iLowestPercent = /*100*/ GD_INT_GET(CULTURE_LEVEL_INFLUENTIAL);   // Don't want to target civs if already influential

	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kTeam.isAtWar(m_pPlayer->getTeam()))
		{
			if (!bCheckOpenBorders || (MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && GET_TEAM(m_pPlayer->getTeam()).IsAllowsOpenBordersToTeam(kTeam.GetID()))
			|| (!MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && kTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam())))
			{
				int iInfluenceOn = GetInfluenceOn((PlayerTypes)iLoopPlayer);
				int iLifetimeCulture = kPlayer.GetJONSCultureEverGenerated();
				int iPercent = 0;

				if (iLifetimeCulture > 0)
				{
					iPercent = iInfluenceOn * 100 / iLifetimeCulture;
				}

				if (iPercent < iLowestPercent)
				{
					iLowestPercent = iPercent;
					eRtnValue = (PlayerTypes)iLoopPlayer;
				}
			}
		}
	}

	return eRtnValue;
}

// NON-CULTURE TOURISM BONUSES
/// Get extra gold from trade routes based on current influence level
int CvPlayerCulture::GetInfluenceTradeRouteGoldBonus(PlayerTypes ePlayer) const
{
	if (!MOD_BALANCE_CORE_POLICIES)
		return 0;

	int iRtnValue = 0;

	if (ePlayer > NO_PLAYER && ePlayer < MAX_MAJOR_CIVS)
	{
		InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
		switch (eLevel)
		{
		case NO_INFLUENCE_LEVEL:
		case INFLUENCE_LEVEL_UNKNOWN:
			break;
		case INFLUENCE_LEVEL_EXOTIC:
			iRtnValue = /*200*/ GD_INT_GET(BALANCE_GOLD_INFLUENCE_LEVEL_EXOTIC);
			break;
		case INFLUENCE_LEVEL_FAMILIAR:
			iRtnValue = /*400*/ GD_INT_GET(BALANCE_GOLD_INFLUENCE_LEVEL_FAMILIAR);
			break;
		case INFLUENCE_LEVEL_POPULAR:
			iRtnValue = /*600*/ GD_INT_GET(BALANCE_GOLD_INFLUENCE_LEVEL_POPULAR);
			break;
		case INFLUENCE_LEVEL_INFLUENTIAL:
			iRtnValue = /*800*/ GD_INT_GET(BALANCE_GOLD_INFLUENCE_LEVEL_INFLUENTIAL);
			break;
		case INFLUENCE_LEVEL_DOMINANT:
			iRtnValue = /*1000*/ GD_INT_GET(BALANCE_GOLD_INFLUENCE_LEVEL_DOMINANT);
			break;
		}
	}

	return iRtnValue;
}

/// Get extra growth from trade routes based on current influence level
int CvPlayerCulture::GetInfluenceTradeRouteGrowthBonus(PlayerTypes ePlayer) const
{
	if (!MOD_BALANCE_CORE_POLICIES)
		return 0;

	int iRtnValue = 0;

	if (ePlayer < MAX_MAJOR_CIVS)
	{
		InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
		switch (eLevel)
		{
		case NO_INFLUENCE_LEVEL:
		case INFLUENCE_LEVEL_UNKNOWN:
			break; // No impact.
		case INFLUENCE_LEVEL_EXOTIC:
			iRtnValue = /*5*/ GD_INT_GET(BALANCE_GROWTH_INFLUENCE_LEVEL_EXOTIC);
			break;
		case INFLUENCE_LEVEL_FAMILIAR:
			iRtnValue = /*10*/ GD_INT_GET(BALANCE_GROWTH_INFLUENCE_LEVEL_FAMILIAR);
			break;
		case INFLUENCE_LEVEL_POPULAR:
			iRtnValue = /*15*/ GD_INT_GET(BALANCE_GROWTH_INFLUENCE_LEVEL_POPULAR);
			break;
		case INFLUENCE_LEVEL_INFLUENTIAL:
			iRtnValue = /*20*/ GD_INT_GET(BALANCE_GROWTH_INFLUENCE_LEVEL_INFLUENTIAL);
			break;
		case INFLUENCE_LEVEL_DOMINANT:
			iRtnValue = /*25*/ GD_INT_GET(BALANCE_GROWTH_INFLUENCE_LEVEL_DOMINANT);
			break;
		}
	}

	return iRtnValue;
}

/// Get extra science from trade routes based on current influence level
int CvPlayerCulture::GetInfluenceTradeRouteScienceBonus(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;
	if (ePlayer < MAX_MAJOR_CIVS)
	{
		InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
		switch (eLevel)
		{
		case NO_INFLUENCE_LEVEL:
		case INFLUENCE_LEVEL_UNKNOWN:
			break; // No impact.
		case INFLUENCE_LEVEL_EXOTIC:
			iRtnValue = /*0 in CP, 2 in VP*/ GD_INT_GET(BALANCE_SCIENCE_INFLUENCE_LEVEL_EXOTIC);
			break;
		case INFLUENCE_LEVEL_FAMILIAR:
			iRtnValue = /*1 in CP, 4 in VP*/ GD_INT_GET(BALANCE_SCIENCE_INFLUENCE_LEVEL_FAMILIAR);
			break;
		case INFLUENCE_LEVEL_POPULAR:
			iRtnValue = /*2 in CP, 6 in VP*/ GD_INT_GET(BALANCE_SCIENCE_INFLUENCE_LEVEL_POPULAR);
			break;
		case INFLUENCE_LEVEL_INFLUENTIAL:
			iRtnValue = /*3 in CP, 8 in VP*/ GD_INT_GET(BALANCE_SCIENCE_INFLUENCE_LEVEL_INFLUENTIAL);
			break;
		case INFLUENCE_LEVEL_DOMINANT:
			iRtnValue = /*4 in CP, 10 in VP*/ GD_INT_GET(BALANCE_SCIENCE_INFLUENCE_LEVEL_DOMINANT);
			break;
		}
	}

	return iRtnValue;
}

/// Get reduction in conquest times and population loss based on current influence level
int CvPlayerCulture::GetInfluenceCityConquestReduction(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;

	if (ePlayer < MAX_MAJOR_CIVS)
	{
		InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
		switch (eLevel)
		{
		case NO_INFLUENCE_LEVEL:
		case INFLUENCE_LEVEL_UNKNOWN:
			break; // No impact.
		case INFLUENCE_LEVEL_EXOTIC:
			iRtnValue = 10;
			break;
		case INFLUENCE_LEVEL_FAMILIAR:
			iRtnValue = 25;
			break;
		case INFLUENCE_LEVEL_POPULAR:
			iRtnValue = 50;
			break;
		case INFLUENCE_LEVEL_INFLUENTIAL:
			iRtnValue = 75;
			break;
		case INFLUENCE_LEVEL_DOMINANT:
			iRtnValue = 100;
			break;
		}
	}
	
	return range(iRtnValue + /*0 in CP, 5 in VP*/ GD_INT_GET(BALANCE_CONQUEST_REDUCTION_BOOST), 0, 100);
}

/// Get spy time to establish surveillance based on current influence level
int CvPlayerCulture::GetInfluenceSurveillanceTime(PlayerTypes ePlayer) const
{
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	if (kPlayer.isMajorCiv())
	{
		if (GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_FAMILIAR)
			return 1;
	}
	else if (kPlayer.isMinorCiv())
	{
		PlayerTypes eAlly = kPlayer.GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER)
		{
			if (GetInfluenceLevel(eAlly) >= INFLUENCE_LEVEL_FAMILIAR)
				return 1;
		}
	}

	return 3;
}

/// Get extra spy rank in city state allies based on current influence level
int CvPlayerCulture::GetInfluenceCityStateSpyRankBonus(PlayerTypes eCityStatePlayer) const
{
	int iRtnValue = 0;

	// Have a major power ally?
	CvPlayer &kCityState = GET_PLAYER(eCityStatePlayer);
	if (kCityState.isMinorCiv())
	{
		PlayerTypes eAlly = kCityState.GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER)
		{
			InfluenceLevelTypes eLevel = GetInfluenceLevel(eAlly);
			switch (eLevel)
			{
			case NO_INFLUENCE_LEVEL:
			case INFLUENCE_LEVEL_UNKNOWN:
			case INFLUENCE_LEVEL_EXOTIC:
			case INFLUENCE_LEVEL_FAMILIAR:
				break; // No impact.
			case INFLUENCE_LEVEL_POPULAR:
				iRtnValue = 1;
				break;
			case INFLUENCE_LEVEL_INFLUENTIAL:
				iRtnValue = 2;
				break;
			case INFLUENCE_LEVEL_DOMINANT:
				iRtnValue = 3;
				break;
			}
		}
	}

	return iRtnValue;
}

/// Get extra spy rank in major civ cities based on current influence level
int CvPlayerCulture::GetInfluenceMajorCivSpyRankBonus(PlayerTypes ePlayer) const
{
	if (!MOD_BALANCE_VP)
		return 0;

	int iRtnValue = 0;
	InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
	switch (eLevel)
	{
	case NO_INFLUENCE_LEVEL:
	case INFLUENCE_LEVEL_UNKNOWN:
	case INFLUENCE_LEVEL_EXOTIC:
	case INFLUENCE_LEVEL_FAMILIAR:
	case INFLUENCE_LEVEL_POPULAR:
		break; // No impact.
	case INFLUENCE_LEVEL_INFLUENTIAL:
		iRtnValue = 1;
		break;
	case INFLUENCE_LEVEL_DOMINANT:
		iRtnValue = 2;
		break;
	}

	return iRtnValue;
}

/// Get spy rank tooltip associated with bonus from cultural influence
CvString CvPlayerCulture::GetInfluenceSpyRankTooltip(const CvString& szName, const CvString& szRank, PlayerTypes ePlayer)
{
	CvString szRtnValue = GetLocalizedText("TXT_KEY_EO_SPY_RANK_TT", szName, szRank);
	
	if (ePlayer != NO_PLAYER)
	{
		CvPlayer& kOtherPlayer = GET_PLAYER(ePlayer);

		int iRankBonus = 0;
		if (kOtherPlayer.isMinorCiv())
		{
			iRankBonus = GetInfluenceCityStateSpyRankBonus(ePlayer);

			if (iRankBonus > 0)
			{
				szRtnValue += GetLocalizedText("TXT_KEY_SPY_BONUS_CITY_STATE", iRankBonus);
			}
		}
		else
		{
			iRankBonus = GetInfluenceMajorCivSpyRankBonus(ePlayer);

			if (iRankBonus > 0)
			{
				szRtnValue += GetLocalizedText("TXT_KEY_SPY_BONUS_MAJOR_CIV", iRankBonus);
			}
		}
	}

	return szRtnValue;
}

/// What is my total tourism per turn (before modifiers)
int CvPlayerCulture::GetTourism()
{
	int iRtnValue = 0;
	int iStartEra = /*ANCIENT*/ GD_INT_GET(TOURISM_START_ERA);
	int iStartTech = /*AGRICULTURE*/ GD_INT_GET(TOURISM_START_TECH);
	
	if ((iStartTech == -1 || m_pPlayer->HasTech((TechTypes) iStartTech)) && (iStartEra == -1 || m_pPlayer->GetCurrentEra() >= iStartEra))
	{
		int iLoop = 0;
		for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
		{
			iRtnValue += pCity->GetBaseTourism();
		}

		// Tourism from religion
		iRtnValue += m_pPlayer->GetYieldPerTurnFromReligion(YIELD_TOURISM) * 100;

		// Trait bonus which adds Tourism for trade partners? 
		iRtnValue += m_pPlayer->GetYieldPerTurnFromTraits(YIELD_TOURISM) * 100;

		// Tourism from Espionage Events
		iRtnValue += m_pPlayer->GetYieldPerTurnFromEspionageEvents(YIELD_TOURISM, true) * 100;
		iRtnValue -= m_pPlayer->GetYieldPerTurnFromEspionageEvents(YIELD_TOURISM, false) * 100;
	}

	return max(0, iRtnValue);
}

/// At the player level, what is the modifier for tourism between these players?
int CvPlayerCulture::GetTourismModifierWith(PlayerTypes ePlayer) const
{
	int iMultiplier = 0;
	if (m_pPlayer->getCapitalCity())
	{
		iMultiplier = m_pPlayer->getCapitalCity()->GetCityCulture()->GetTourismMultiplier(ePlayer, false, false, false, false, false);
	}
	return iMultiplier;
}

/// Tooltip for GetTourismModifierWith()
CvString CvPlayerCulture::GetTourismModifierWithTooltip(PlayerTypes eTargetPlayer) const
{
	CvString szRtnValue = "";
	CvTeam &kMyTeam = GET_TEAM(m_pPlayer->getTeam());
	CvPlayer &kTargetPlayer = GET_PLAYER(eTargetPlayer);
	CvTeam &kTargetTeam = GET_TEAM(kTargetPlayer.getTeam());
	PolicyBranchTypes eMyIdeology = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = kTargetPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();


	int iTourismWithTargetPlayer = m_pPlayer->GetCulture()->GetTourismPerTurnIncludingInstant(eTargetPlayer, true);
	if (iTourismWithTargetPlayer != 0)
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_INSTANT_TT_VALUE", iTourismWithTargetPlayer) + "[ENDCOLOR]";
	}

	szRtnValue += "[NEWLINE]------------------------[NEWLINE]";

	// POSITIVE MODIFIERS

	// This player is on the same Team as us
	if (m_pPlayer->getTeam() == GET_PLAYER(eTargetPlayer).getTeam())
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_SAME_TEAM", 200) + "[ENDCOLOR]";
	}

	// Trade route between one of this player's cities and ours
	if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), eTargetPlayer))
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_TRADE_ROUTE", GetTourismModifierTradeRoute()) + "[ENDCOLOR]";
	}

	// Open Borders with this player
	if ((MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && kMyTeam.IsAllowsOpenBordersToTeam(kTargetTeam.GetID())) ||
		(!MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && kTargetTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam())))
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_OPEN_BORDERS", GetTourismModifierOpenBorders()) + "[ENDCOLOR]";
	}

	// Shared Religion with this player
	int iSharedReligionModifier = GetTourismModifierSharedReligion(eTargetPlayer);
	if (iSharedReligionModifier > 0)
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_RELIGION_NOTE", iSharedReligionModifier) + "[ENDCOLOR]";
	}

	// Diplomat in this player's Capital City
	if (m_pPlayer->GetEspionage()->IsMyDiplomatVisitingThem(eTargetPlayer) && 
		(MOD_BALANCE_CORE || (!MOD_BALANCE_CORE && eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)))
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA", /*25 in CP, 20 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIPLOMAT)) + "[ENDCOLOR]";
	}
	
	if (MOD_BALANCE_VP)
	{
		// This player has more boredom (unhappiness due to culture deficit) than us
		int iBoredom = kTargetPlayer.GetCulture()->GetBoredomCache();
		int iDelta = iBoredom - GetBoredomCache();
		if (iDelta > 0)
		{	
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_BOREDOM", iDelta) + "[ENDCOLOR]";
		}
	}

	// Expended a Great Musician in one of this player's cities
	if (m_pPlayer->getTourismBonusTurnsPlayer(eTargetPlayer) > 0)
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_TOUR_BONUS", m_pPlayer->getTourismBonusTurnsPlayer(eTargetPlayer)) + "[ENDCOLOR]";
	}
	// Expended a Great Musician in another player's cities
	else if (m_pPlayer->GetTourismBonusTurns() > 0)
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_TOUR_BONUS_HALF", m_pPlayer->GetTourismBonusTurns()) + "[ENDCOLOR]";
	}

	// This player is our Vassal
	if (kTargetTeam.GetMaster() == m_pPlayer->getTeam())
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_VASSAL", GetTourismModifierVassal()) + "[ENDCOLOR]";
	}
	
	// Positive Congress Proposal is Active
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague != NULL && pLeague->GetTourismMod() > 0)
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_LEAGUE", pLeague->GetTourismMod()) + "[ENDCOLOR]";
	}

	// This player shares our ideology (activated with a policy)
	int iSharedIdeologyMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY);
	if (iSharedIdeologyMod != 0)
	{
		if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology == eTheirIdeology)
		{
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_SHARED_IDEOLOGY", iSharedIdeologyMod) + "[ENDCOLOR]";
		}
	}

	// This player is at war against the same opponent as us (activated with a policy)
	int iCommonFoeMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_COMMON_FOE);
	if (iCommonFoeMod != 0)
	{
		int iTotal = 0;
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (eLoopPlayer != eTargetPlayer && eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				// Are they at war with me too?
				if (kMyTeam.isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) && kTargetTeam.isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					iTotal += iCommonFoeMod;
					break;
				}
			}
		}
		if (iTotal != 0)
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_COMMON_FOE", iTotal) + "[ENDCOLOR]";
	}

	// This player is less happy than us (activated with a policy)
	int iLessHappyMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_LESS_HAPPY);
	if (iLessHappyMod != 0)
	{
		if (m_pPlayer->GetExcessHappiness() > kTargetPlayer.GetExcessHappiness())
		{
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_LESS_HAPPY", iLessHappyMod) + "[ENDCOLOR]";
		}
	}

	// Have a positive war-score against any player (activated with a policy)
	if (m_pPlayer->GetPositiveWarScoreTourismMod() != 0)
	{
		int iWarScore = m_pPlayer->GetDiplomacyAI()->GetHighestWarscore();

		if (iWarScore > 0)
		{
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_WARSCORE", iWarScore) + "[ENDCOLOR]";
		}
	}

	// Corporation franchises in this player's cities
	int iFranchiseBonus = m_pPlayer->GetCulture()->GetFranchiseModifier(eTargetPlayer);
	if (iFranchiseBonus != 0)
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_FRANCHISES", iFranchiseBonus) + "[ENDCOLOR]";
	}

	// Carnival is active (UA)
	if (m_pPlayer->isGoldenAge() && m_pPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier())
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_CARNIVAL", m_pPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier()) + "[ENDCOLOR]";
	}

	// NEUTRAL MODIFIERS
	if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), eTargetPlayer))
	{
		szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_TRADE_ROUTE", 0) + "[ENDCOLOR]";
	}
	if ((MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && !kMyTeam.IsAllowsOpenBordersToTeam(kTargetTeam.GetID())) ||
		(!MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && !kTargetTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam())))
	{
		szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_OPEN_BORDERS", 0) + "[ENDCOLOR]";
	}
	if (iSharedReligionModifier <= 0)
	{
		szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_RELIGION_NOTE", 0) + "[ENDCOLOR]";
	}
	if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
	{
		if (!m_pPlayer->GetEspionage()->IsMyDiplomatVisitingThem(eTargetPlayer))
		{
			szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA", 0) + "[ENDCOLOR]";
		}
	}

	// NEGATIVE MODIFIERS
	// This player has a different Ideology than us
	if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
	{
		szRtnValue += "[COLOR_NEGATIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_DIFFERENT_IDEOLOGIES", /*-34 in CP, -10 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIFFERENT_IDEOLOGIES)) + "[ENDCOLOR]";
	}
	
	// City difference - do we have more than them?
	int iNumCities = m_pPlayer->GetNumEffectiveCities() - kTargetPlayer.GetNumEffectiveCities();
	if (iNumCities > 0)
	{
		// Mod for City Count
		int iMod = /*0 in CP, 5 in VP*/ GC.getMap().getWorldInfo().GetNumCitiesTourismCostMod();
		iMod -= m_pPlayer->GetTourismCostXCitiesMod();

		iMod *= iNumCities;

		if (iMod != 0)
			szRtnValue += "[COLOR_NEGATIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_CAPITAL_PENALTY", iMod, iNumCities) + "[ENDCOLOR]";
	}

	// Negative Congress Proposal is active
	if(pLeague != NULL && pLeague->GetTourismMod() < 0)
	{
		szRtnValue += "[COLOR_NEGATIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_LEAGUE_NEGATIVE", pLeague->GetTourismMod()) + "[ENDCOLOR]";
	}

	// This player has closed borders with us (UA)
	if (kTargetPlayer.isMajorCiv() && kTargetPlayer.GetPlayerTraits()->IsNoOpenTrade())
	{
		if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(eTargetPlayer, m_pPlayer->GetID(), true))
		{
			szRtnValue += "[COLOR_NEGATIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_JAPAN_UA", /*-34 in CP, -10 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIFFERENT_IDEOLOGIES)) + "[ENDCOLOR]";
		}
	}

	return szRtnValue;
}

/// Tourism modifier (base plus policy boost) - shared religion
int CvPlayerCulture::GetTourismModifierSharedReligion(PlayerTypes eTargetPlayer) const
{
	ReligionTypes eMyReligion = m_pPlayer->GetReligions()->GetStateReligion();
	if (eMyReligion <= RELIGION_PANTHEON || eTargetPlayer == NO_PLAYER)
		return 0;
	
	int iModifier = /*25 in CP, 1 in VP*/ GD_INT_GET(TOURISM_MODIFIER_SHARED_RELIGION);
	int iModifierMax = /*0 in CP, 50 in VP*/ GD_INT_GET(TOURISM_MODIFIER_SHARED_RELIGION_MAX);
	int iMultiplier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SHARED_RELIGION_TOURISM_MODIFIER) + m_pPlayer->GetPlayerTraits()->GetSharedReligionTourismModifier();
	
	switch (/*0 in CP, 2 in VP*/ GD_INT_GET(TOURISM_MODIFIER_SHARED_RELIGION_TYPE))
	{
		case 0:
			// Flat value if both players have the same religion
			return GET_PLAYER(eTargetPlayer).GetReligions()->GetStateReligion() == eMyReligion ? iModifier + iMultiplier : 0;
		case 1:
			// Scales with number of cities following religion
			iModifier *= GC.getGame().GetGameReligions()->GetNumDomesticCitiesFollowing(eMyReligion, eTargetPlayer);
			break;
		case 2:
			// Scales with percentage of followers of religion in population
			int iFollowers = GC.getGame().GetGameReligions()->GetNumFollowers(eMyReligion, eTargetPlayer);
			int iPopulation = GET_PLAYER(eTargetPlayer).getTotalPopulation();
			iModifier = iModifier * 100 * iFollowers / max(iPopulation, 1);
			break;
	}
	
	if (iModifierMax > 0)
		iModifier = min(iModifierMax, iModifier);
	
	return iModifier * (100 + iMultiplier) / 100;
}

/// Tourism modifier (base plus policy boost) - trade route
int CvPlayerCulture::GetTourismModifierTradeRoute() const
{
	return /*25 in CP, 10 in VP*/ GD_INT_GET(TOURISM_MODIFIER_TRADE_ROUTE) + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_ROUTE_TOURISM_MODIFIER);
}

/// Tourism modifier (base plus policy boost) - open borders
int CvPlayerCulture::GetTourismModifierOpenBorders() const
{
	return /*25 in CP, 10 in VP*/ GD_INT_GET(TOURISM_MODIFIER_OPEN_BORDERS) + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_OPEN_BORDERS_TOURISM_MODIFIER);
}

/// Tourism modifier - corporation franchise
int CvPlayerCulture::GetFranchiseModifier(PlayerTypes ePlayer, bool bJustCheckOne) const
{
	return m_pPlayer->GetCorporations()->GetFranchiseTourismMod(ePlayer, bJustCheckOne);
}

/// Tourism modifier - vassal
int CvPlayerCulture::GetTourismModifierVassal() const
{
	return /*33*/ GD_INT_GET(VASSAL_TOURISM_MODIFIER);
}

/// Is the populace satisfied?
PublicOpinionTypes CvPlayerCulture::GetPublicOpinionType() const
{
	return m_eOpinion;
}

/// Which ideology would the populace prefer?
PolicyBranchTypes CvPlayerCulture::GetPublicOpinionPreferredIdeology() const
{
	return m_ePreferredIdeology;
}

/// Tooltip breaking down public opinion
CvString CvPlayerCulture::GetPublicOpinionTooltip() const
{
	return m_strOpinionTooltip;
}

/// Unhappiness generated from public opinion
int CvPlayerCulture::GetPublicOpinionUnhappiness() const
{
	return m_iOpinionUnhappiness;
}

/// Tooltip breaking down public opinion unhappiness
CvString CvPlayerCulture::GetPublicOpinionUnhappinessTooltip() const
{
	return m_strOpinionUnhappinessTooltip;
}

/// Which civ is putting the largest cultural pressure on us?
PlayerTypes CvPlayerCulture::GetPublicOpinionBiggestInfluence() const
{
	return m_eOpinionBiggestInfluence;
}

/// What turn did we switch ideologies
int CvPlayerCulture::GetTurnIdeologySwitch() const
{
	return m_iTurnIdeologySwitch;
}
void CvPlayerCulture::SetTurnIdeologySwitch(int iValue)
{
	m_iTurnIdeologySwitch = iValue;
}

/// What turn did we switch ideologies
int CvPlayerCulture::GetTurnIdeologyAdopted() const
{
	return m_iTurnIdeologyAdopted;
}
void CvPlayerCulture::SetTurnIdeologyAdopted(int iValue)
{
	m_iTurnIdeologyAdopted = iValue;
}

/// Add tourism with all known civs
void CvPlayerCulture::AddTourismAllKnownCivs(int iTourism)
{
#if defined(MOD_BALANCE_CORE)
	if(m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
	{
		return;
	}
#endif
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if (MOD_BALANCE_CORE) {
                ChangeInfluenceOn(eLoopPlayer, iTourism, true, true);
            } else {
                ChangeInfluenceOn(eLoopPlayer, iTourism);
            }
		}
	}
}

/// Add tourism with all known civs
void CvPlayerCulture::AddTourismAllKnownCivsWithModifiers(int iTourism)
{
#if defined(MOD_BALANCE_CORE)
	if (m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
	{
		return;
	}
#endif
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;

		if (eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			ChangeInfluenceOn(eLoopPlayer, iTourism, true, true);
		}
	}
}

/// Add tourism with all known civs
void CvPlayerCulture::AddTourismAllKnownCivsOtherCivWithModifiers(PlayerTypes eOtherPlayer, int iTourism)
{
#if defined(MOD_BALANCE_CORE)
	if (m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
		return;
	if (GET_PLAYER(eOtherPlayer).isMinorCiv() || GET_PLAYER(eOtherPlayer).isBarbarian())
		return;

#endif
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;

		if (eLoopPlayer == m_pPlayer->GetID() || eLoopPlayer == eOtherPlayer)
			continue;

		if (GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			ChangeInfluenceOn(eLoopPlayer, iTourism, true, true);
		}
	}
}

// PRIVATE METHODS
/// Once per turn calculation of public opinion data
void CvPlayerCulture::DoPublicOpinion()
{
	PolicyBranchTypes eOurIdeology = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();

	// If we don't have an ideology, public opinion doesn't matter
	if (eOurIdeology == NO_POLICY_BRANCH_TYPE)
		return;

	//this function will set these next 6 vars
	m_eOpinion = NO_PUBLIC_OPINION;
	m_ePreferredIdeology = NO_POLICY_BRANCH_TYPE;
	m_iOpinionUnhappiness = 0;
	m_strOpinionTooltip = "";
	m_strOpinionUnhappinessTooltip = "";
	m_eOpinionBiggestInfluence = NO_PLAYER;


	std::vector<int> aIdeologyPressure;
	aIdeologyPressure.resize(GC.getNumPolicyBranchInfos(), 0);

	std::vector<CvString> vIdeologyPressureStrings;
	vIdeologyPressureStrings.resize(GC.getNumPolicyBranchInfos(), "");

	CvString strWorldIdeologyPressureString = "";

	Localization::String locOverview = Localization::Lookup("TXT_KEY_CO_OPINION_TT_OVERVIEW");
	m_strOpinionTooltip += locOverview.toUTF8();

	for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
	{
		PolicyBranchTypes eIdeologyBranch = (PolicyBranchTypes)iPolicyBranch;
		if (eIdeologyBranch != NO_POLICY_BRANCH_TYPE)
		{
			CvPolicyBranchEntry* kIdeology = GC.getPolicyBranchInfo(eIdeologyBranch);

			if (kIdeology->IsPurchaseByLevel())
			{

				CvString sIdeologyIcon = kIdeology->GetIconString();
				if (sIdeologyIcon == NULL || sIdeologyIcon.IsEmpty())
				{
					//back up: font icon will be [ICON_IDEOLOGY_%kIdeology->GetDescription()%]
					sIdeologyIcon = "[ICON_IDEOLOGY_";
					for (uint x = 0; x < strlen(kIdeology->GetDescription()); x++)
						sIdeologyIcon += toupper(kIdeology->GetDescription()[x]);

					sIdeologyIcon += "]";
				}
				if (MOD_ISKA_HERITAGE && eIdeologyBranch == (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_HERITAGE))
				{
					sIdeologyIcon = "[ICON_RELIGION]";
				}

				// Look at World Congress
				aIdeologyPressure[eIdeologyBranch] += GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eIdeologyBranch);
				if (aIdeologyPressure[eIdeologyBranch] > 0)
				{

					CvString sTemp = "[NEWLINE]For [COLOR_POSITIVE_TEXT]"; //used to be TXT_KEY_CO_OPINION_TT_FOR_ORDER, TXT_KEY_CO_OPINION_TT_FOR_FREEDOM, etc.
					sTemp += kIdeology->GetDescription();
					sTemp += "[ENDCOLOR]:[NEWLINE]";

					for (int i = 0; i < aIdeologyPressure[eIdeologyBranch]; i++)
					{
						sTemp += sIdeologyIcon;
					}

					strWorldIdeologyPressureString += ((Localization::String)sTemp).toUTF8();
				}
			}
		}
	}

	// Look at each civ
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
			if (eOtherCivIdeology != NO_POLICY_BRANCH_TYPE)
			{
				int iCulturalDominanceOverUs = 0;
				int iTheirInfluenceLevel = kPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID());
				int iOurInfluenceLevel = m_pPlayer->GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);

				if (MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
				{
					//Only for popular+ civs.
					if (iTheirInfluenceLevel > (int)INFLUENCE_LEVEL_FAMILIAR)
					{
						//INFLUENCE_TREND_RISING = 1, INFLUENCE_TREND_STATIC = 0, INFLUENCE_TREND_FALLING = -1

						iTheirInfluenceLevel -= (int)(kPlayer.GetCulture()->GetInfluenceTrend(m_pPlayer->GetID())) - 1;
						iOurInfluenceLevel -= (int)(m_pPlayer->GetCulture()->GetInfluenceTrend((PlayerTypes)iLoopPlayer)) - 1;
					}
				}

				iCulturalDominanceOverUs = max(0, iTheirInfluenceLevel) - max(0, iOurInfluenceLevel);

				if (iCulturalDominanceOverUs > 0)
				{
					CvString sTheirIdeologyIcon = GC.getPolicyBranchInfo(eOtherCivIdeology)->GetIconString();
					if (sTheirIdeologyIcon == NULL || sTheirIdeologyIcon.IsEmpty())
					{
						//back up: font icon will be [ICON_IDEOLOGY_%kIdeology->GetDescription()%]
						sTheirIdeologyIcon = "[ICON_IDEOLOGY_";
						for (uint x = 0; x < strlen(GC.getPolicyBranchInfo(eOtherCivIdeology)->GetDescription()); x++)
							sTheirIdeologyIcon += toupper(GC.getPolicyBranchInfo(eOtherCivIdeology)->GetDescription()[x]);

						sTheirIdeologyIcon += "]";
					}
					if (MOD_ISKA_HERITAGE && eOtherCivIdeology == (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_HERITAGE))
					{
						sTheirIdeologyIcon = "[ICON_RELIGION]";
					}

					aIdeologyPressure[eOtherCivIdeology] += iCulturalDominanceOverUs;
					if (vIdeologyPressureStrings[eOtherCivIdeology].size() > 0)
					{
						vIdeologyPressureStrings[eOtherCivIdeology] += ", ";
					}
					vIdeologyPressureStrings[eOtherCivIdeology] += kPlayer.getCivilizationShortDescription();
					vIdeologyPressureStrings[eOtherCivIdeology] += ": ";
					for (int iI = 0; iI < iCulturalDominanceOverUs; iI++)
					{
						vIdeologyPressureStrings[eOtherCivIdeology] += sTheirIdeologyIcon;
					}
				}
			}
		}
	}

	if (MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
	{
		CvString sIdeologyIcon = GC.getPolicyBranchInfo(eOurIdeology)->GetIconString();
		if (sIdeologyIcon == NULL || sIdeologyIcon.IsEmpty())
		{
			//back up: font icon will be [ICON_IDEOLOGY_%kIdeology->GetDescription()%]
			sIdeologyIcon = "[ICON_IDEOLOGY_";
			for (uint x = 0; x < strlen(GC.getPolicyBranchInfo(eOurIdeology)->GetDescription()); x++)
				sIdeologyIcon += toupper(GC.getPolicyBranchInfo(eOurIdeology)->GetDescription()[x]);

			sIdeologyIcon += "]";
		}
		if (MOD_ISKA_HERITAGE && eOurIdeology == (PolicyBranchTypes)GD_INT_GET(POLICY_BRANCH_HERITAGE))
		{
			sIdeologyIcon = "[ICON_RELIGION]";
		}

		//Get a bonus for fewer followers.
		int iNumCivsNotFollowingOurIdeology = GetNumCivsFollowingAnyIdeology() - GetNumCivsFollowingIdeology(eOurIdeology) + 1;
		//Divide by number of ideologies.
		int iNumIdeologies = 0;
		for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
		{
			if (GC.getPolicyBranchInfo((PolicyBranchTypes)iPolicyBranch)->IsPurchaseByLevel())
				iNumIdeologies++;
		}
		iNumCivsNotFollowingOurIdeology /= iNumIdeologies;
		aIdeologyPressure[eOurIdeology] += iNumCivsNotFollowingOurIdeology;

		if (iNumCivsNotFollowingOurIdeology > 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_IDEOLOGY_INFLUENCE_CIVS_WITH_OTHER_IDEOLOGY");
			m_strOpinionTooltip += locText.toUTF8();
			m_strOpinionTooltip += " ";
			for (int iI = 0; iI < iNumCivsNotFollowingOurIdeology; iI++)
			{
				m_strOpinionTooltip += sIdeologyIcon;
			}
			m_strOpinionTooltip += "[NEWLINE][NEWLINE]";
		}

		//first ideology?
		int iTurnsSinceIdeology = 0;
		if (GetTurnIdeologySwitch() == -1)
		{
			iTurnsSinceIdeology = GC.getGame().getGameTurn() - GetTurnIdeologyAdopted();
		}
		else
		{
			iTurnsSinceIdeology = GC.getGame().getGameTurn() - GetTurnIdeologySwitch();
		}
		//free 'pressure' for our ideology if we just adopted.
		int iTurnLag = 30;
		iTurnLag *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iTurnLag /= 100;
		iTurnsSinceIdeology = (int)ceil((iTurnLag - iTurnsSinceIdeology) / 10.0);
		if (iTurnsSinceIdeology > 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_IDEOLOGY_INFLUENCE_RECENTLY_ADOPTED");
			m_strOpinionTooltip += locText.toUTF8();
			m_strOpinionTooltip += " ";
			aIdeologyPressure[eOurIdeology] += iTurnsSinceIdeology;
			for (int iI = 0; iI < iTurnsSinceIdeology; iI++)
			{
				m_strOpinionTooltip += sIdeologyIcon;
			}
			m_strOpinionTooltip += "[NEWLINE][NEWLINE]";
		}
	}

	// Now compute satisfaction with this branch compared to the other ones
	int iDissatisfaction = 0;

	int iTotal = 0;

	//used if not content
	int iMax = -999;
	PolicyBranchTypes ePrefIdeology = NO_POLICY_BRANCH_TYPE;

	for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
	{
		PolicyBranchTypes eIdeologyBranch = (PolicyBranchTypes)iPolicyBranch;
		if (eIdeologyBranch != NO_POLICY_BRANCH_TYPE)
		{
			CvPolicyBranchEntry* kIdeology = GC.getPolicyBranchInfo(eIdeologyBranch);

			if (kIdeology->IsPurchaseByLevel())
			{
				iTotal += aIdeologyPressure[eIdeologyBranch];
				if (eOurIdeology != eIdeologyBranch && aIdeologyPressure[eIdeologyBranch] > iMax)
				{
					iMax = aIdeologyPressure[eIdeologyBranch];
					ePrefIdeology = eIdeologyBranch;
				}
			}
		}
	}

	if (aIdeologyPressure[eOurIdeology] >= (iTotal - aIdeologyPressure[eOurIdeology]))
	{
		m_eOpinion = PUBLIC_OPINION_CONTENT;
	}
	else
	{
		m_ePreferredIdeology = ePrefIdeology;
		iDissatisfaction = iTotal - (aIdeologyPressure[eOurIdeology] * 2);
	}

	if (m_eOpinion != PUBLIC_OPINION_CONTENT)
	{
		if ((MOD_BALANCE_CORE_POLICIES && iDissatisfaction < 4) || (!MOD_BALANCE_CORE_POLICIES && iDissatisfaction < 3))
		{
			m_eOpinion = PUBLIC_OPINION_DISSIDENTS;
		}
		else if ((MOD_BALANCE_CORE_POLICIES && iDissatisfaction < 7) || (!MOD_BALANCE_CORE_POLICIES && iDissatisfaction < 5))
		{
			m_eOpinion = PUBLIC_OPINION_CIVIL_RESISTANCE;
		}
		else
		{
			m_eOpinion = PUBLIC_OPINION_REVOLUTIONARY_WAVE;
		}

		m_iOpinionUnhappiness = ComputePublicOpinionUnhappiness(iDissatisfaction);

		// Find civ exerting greatest pressure
		int iGreatestDominance = -1;
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
		{
			CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
			if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
			{
				PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
				if (eOtherCivIdeology == m_ePreferredIdeology)
				{
					int iCulturalDominanceOverUs = kPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID()) - m_pPlayer->GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);
					if (iCulturalDominanceOverUs > 0)
					{
						if (iCulturalDominanceOverUs > iGreatestDominance)
						{
							iGreatestDominance = iCulturalDominanceOverUs;
							m_eOpinionBiggestInfluence = (PlayerTypes)iLoopPlayer;
						}
					}
				}
			}
		}
	}

	// Build tooltip

	if (!strWorldIdeologyPressureString.empty())
	{
		Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_INFLUENCED_WORLD_IDEOLOGY");
		m_strOpinionTooltip += locText.toUTF8();
		m_strOpinionTooltip += strWorldIdeologyPressureString;
		m_strOpinionTooltip += "[NEWLINE][NEWLINE]";
	}

	bool bHasIdeologyInfluence = false;
	for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
	{
		PolicyBranchTypes eIdeologyBranch = (PolicyBranchTypes)iPolicyBranch;
		if (eIdeologyBranch != NO_POLICY_BRANCH_TYPE)
		{
			CvPolicyBranchEntry* kIdeology = GC.getPolicyBranchInfo(eIdeologyBranch);

			if (kIdeology->IsPurchaseByLevel())
			{
				if (vIdeologyPressureStrings[eIdeologyBranch].size() > 0)
				{
					CvString sTemp = "[NEWLINE]For [COLOR_POSITIVE_TEXT]"; //used to be TXT_KEY_CO_OPINION_TT_FOR_ORDER, TXT_KEY_CO_OPINION_TT_FOR_FREEDOM, etc.
					sTemp += kIdeology->GetDescription();
					sTemp += "[ENDCOLOR]:[NEWLINE]";
					sTemp += vIdeologyPressureStrings[eIdeologyBranch];

					vIdeologyPressureStrings[eIdeologyBranch] = ((Localization::String)sTemp).toUTF8();

					bHasIdeologyInfluence = true;
				}
			}
		}
	}

	if (!bHasIdeologyInfluence)
	{
		Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_NOT_INFLUENCED");
		m_strOpinionTooltip += locText.toUTF8();
	}
	else
	{
		Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_INFLUENCED_BY");
		m_strOpinionTooltip += locText.toUTF8();
		for (std::vector<CvString>::iterator it = vIdeologyPressureStrings.begin(); it != vIdeologyPressureStrings.end(); ++it)
		{
			CvString s = *it;
			if (!s.empty())
				m_strOpinionTooltip += s;
		}
	}

	if (m_ePreferredIdeology != NO_POLICY_BRANCH_TYPE)
	{
		Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_PREFERRED_IDEOLOGY");
		locText << GC.getPolicyBranchInfo(m_ePreferredIdeology)->GetDescription();
		m_strOpinionTooltip += locText.toUTF8();
	}
	if (m_iOpinionUnhappiness > 0)
	{
		Localization::String locText;
		locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_UNHAPPINESS_LINE1");
		locText << m_iOpinionUnhappiness;
		m_strOpinionUnhappinessTooltip += locText.toUTF8();

		if (!MOD_BALANCE_CORE_POLICIES)
		{
			locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_UNHAPPINESS_LINE2");
			m_strOpinionUnhappinessTooltip += locText.toUTF8();

			int iPerCityUnhappy = 0;
			int iUnhappyPerXPop = 0;
			if (iDissatisfaction < 3)
			{
				iPerCityUnhappy = 1;
				iUnhappyPerXPop = 10;
			}
			else if (iDissatisfaction < 5)
			{
				iPerCityUnhappy = 2;
				iUnhappyPerXPop = 5;
			}
			else
			{
				iPerCityUnhappy = 4;
				iUnhappyPerXPop = 3;
			}

			locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_UNHAPPINESS_LINE3");
			locText << iPerCityUnhappy;
			m_strOpinionUnhappinessTooltip += locText.toUTF8();

			locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_UNHAPPINESS_LINE4");
			locText << iUnhappyPerXPop;
			m_strOpinionUnhappinessTooltip += locText.toUTF8();
		}
	}
}
/// What would the unhappiness be if we chose this Ideology?
int CvPlayerCulture::ComputeHypotheticalPublicOpinionUnhappiness(PolicyBranchTypes eBranch)
{
	int iDissatisfaction = 0;
	std::vector<int> aIdeologyPressure;
	aIdeologyPressure.resize(GC.getNumPolicyBranchInfos(), 0);

	// Start with World Congress
	for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
	{
		PolicyBranchTypes eIdeologyBranch = (PolicyBranchTypes)iPolicyBranch;
		if (eIdeologyBranch != NO_POLICY_BRANCH_TYPE)
		{
			aIdeologyPressure[eIdeologyBranch] = GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eIdeologyBranch);
		}
	}

	// Look at each civ
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
		{
			PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
			if (eOtherCivIdeology != NO_POLICY_BRANCH_TYPE)
			{
				int iTheirInfluenceLevel = kPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID());
				int iOurInfluenceLevel = m_pPlayer->GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);

				if (MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
				{
					//Only for popular+ civs.
					if (iTheirInfluenceLevel <= (int)INFLUENCE_LEVEL_FAMILIAR)
						continue;

					//INFLUENCE_TREND_RISING = 1, INFLUENCE_TREND_STATIC = 0, INFLUENCE_TREND_FALLING = -1
					iTheirInfluenceLevel -= (int)(kPlayer.GetCulture()->GetInfluenceTrend(m_pPlayer->GetID())) - 1;
					iOurInfluenceLevel -= (int)(m_pPlayer->GetCulture()->GetInfluenceTrend((PlayerTypes)iLoopPlayer)) - 1;
				}

				int iCulturalDominanceOverUs = max(0, iTheirInfluenceLevel) - max(0, iOurInfluenceLevel);
				if (iCulturalDominanceOverUs > 0)
					aIdeologyPressure[eOtherCivIdeology] += iCulturalDominanceOverUs;
			}
		}
	}

	if (MOD_BALANCE_CORE_VICTORY_GAME_CHANGES)
	{
		//Get a bonus for fewer followers.
		int iNumCivsNotFollowingNewIdeology = GetNumCivsFollowingAnyIdeology() - GetNumCivsFollowingIdeology(eBranch) + 1;
		//Divide by number of ideologies.
		int iNumIdeologies = 0;
		for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
		{
			if (GC.getPolicyBranchInfo((PolicyBranchTypes)iPolicyBranch)->IsPurchaseByLevel())
				iNumIdeologies++;
		}
		iNumCivsNotFollowingNewIdeology /= iNumIdeologies;
		aIdeologyPressure[eBranch] += iNumCivsNotFollowingNewIdeology;
	}

	int iTotal = 0;
	int iMax = -999;
	PolicyBranchTypes ePrefIdeology = NO_POLICY_BRANCH_TYPE;

	for (int iPolicyBranch = 0; iPolicyBranch < GC.getNumPolicyBranchInfos(); iPolicyBranch++)
	{
		PolicyBranchTypes eIdeologyBranch = (PolicyBranchTypes)iPolicyBranch;
		if (eIdeologyBranch != NO_POLICY_BRANCH_TYPE)
		{
			CvPolicyBranchEntry* kIdeology = GC.getPolicyBranchInfo(eIdeologyBranch);

			if (kIdeology->IsPurchaseByLevel())
			{
				iTotal += aIdeologyPressure[eIdeologyBranch];
				if (eBranch != eIdeologyBranch && aIdeologyPressure[eIdeologyBranch] > iMax)
				{
					iMax = aIdeologyPressure[eIdeologyBranch];
					ePrefIdeology = eIdeologyBranch;
				}
			}
		}
	}

	if (aIdeologyPressure[eBranch] < (iTotal - aIdeologyPressure[eBranch]))
	{
		m_ePreferredIdeology = ePrefIdeology;
		iDissatisfaction = iTotal - (aIdeologyPressure[eBranch] * 2);
	}

	if (iDissatisfaction == 0)
	{
		return 0;
	}
	else
	{
		return ComputePublicOpinionUnhappiness(iDissatisfaction);
	}
}

bool CvPlayerCulture::WantsDiplomatDoingPropaganda(PlayerTypes eTargetPlayer) const
{
	if (eTargetPlayer == m_pPlayer->GetID())
	{
		return false;
	}

	if (!GET_PLAYER(eTargetPlayer).isAlive())
	{
		return false;
	}

	if (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).isAtWar(m_pPlayer->getTeam()))
	{
		return false;
	}

	InfluenceLevelTypes eInfluenceLevel = GetInfluenceLevel(eTargetPlayer);
	InfluenceLevelTrend eInfluenceTrend = GetInfluenceTrend(eTargetPlayer);
	return eInfluenceLevel > INFLUENCE_LEVEL_FAMILIAR && eInfluenceTrend >= INFLUENCE_TREND_STATIC;
}

int CvPlayerCulture::GetNumCivsFollowingIdeology(PolicyBranchTypes ePolicyBranch)
{
	int iFollowers = 0;
	// determine which civs have run out of techs to steal
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)ui;

		if (GET_PLAYER(eOtherPlayer).GetPlayerPolicies()->GetLateGamePolicyTree() == ePolicyBranch)
			iFollowers++;
	}
	return iFollowers;
}

int CvPlayerCulture::GetNumCivsFollowingAnyIdeology()
{
	int iFollowers = 0;
	// determine which civs have run out of techs to steal
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)ui;

		if (GET_PLAYER(eOtherPlayer).GetPlayerPolicies()->GetLateGamePolicyTree() != NO_POLICY_BRANCH_TYPE)
			iFollowers++;
	}
	return iFollowers;
}

/// How many diplomats could I possibly want now?
int CvPlayerCulture::GetMaxPropagandaDiplomatsWanted() const
{
	int iRtnValue = 0;

	// determine which civs have run out of techs to steal
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)ui;

		if (m_pPlayer->GetID() == eOtherPlayer)
		{
			continue;
		}
		
		if (WantsDiplomatDoingPropaganda(eOtherPlayer))
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

/// Summary of all culture/tourism coming from theming bonuses
int CvPlayerCulture::GetTotalThemingBonuses() const
{
	int iRtnValue = 0;

	CvCity *pCity = NULL;
	int iLoop = 0;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		iRtnValue += pCity->GetCityBuildings()->GetCurrentThemingBonuses(YIELD_CULTURE);
	}

	return iRtnValue;
}

// PRIVATE METHODS

/// Compute effects of dissatisfaction
int CvPlayerCulture::ComputePublicOpinionUnhappiness(int iDissatisfaction)
{
	if (iDissatisfaction == 0)
		return 0;

	if (!MOD_BALANCE_CORE_POLICIES)
	{
		int iPerCityUnhappy = 0;
		int iUnhappyPerXPop = 0;

		if (iDissatisfaction < 3)
		{
			iPerCityUnhappy = 1;
			iUnhappyPerXPop = 10;
		}
		else if (iDissatisfaction < 5)
		{
			iPerCityUnhappy = 2;
			iUnhappyPerXPop = 5;
		}
		else
		{
			iPerCityUnhappy = 4;
			iUnhappyPerXPop = 3;
		}

		return max(m_pPlayer->getNumCities() * iPerCityUnhappy, m_pPlayer->getTotalPopulation() / iUnhappyPerXPop);
	}

	float fPerCityUnhappy = 1.0f;
	float fUnhappyPerXPop = 10.0f;
	
	//important!
	float fPerCityUnhappySlope = 0.2f;
	float fUnhappyPerXPopSlope = -1.0f;

	fPerCityUnhappy += iDissatisfaction*fPerCityUnhappySlope;
	fUnhappyPerXPop += iDissatisfaction*fUnhappyPerXPopSlope;

	//sanitization
	fPerCityUnhappy = min(10.f,max(1.f,fPerCityUnhappy));
	fUnhappyPerXPop = min(100.f,max(1.f,fUnhappyPerXPop));

	return (int) max(m_pPlayer->getNumCities() * fPerCityUnhappy, m_pPlayer->getTotalPopulation() / fUnhappyPerXPop);
}

// LOGGING FUNCTIONS

/// Build log filename
void CvPlayerCulture::LogCultureData()
{
	CvTeam &pTeam = GET_TEAM(m_pPlayer->getTeam());

	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	static bool bFirstRun = true;
	bool bBuildHeader = false;
	CvString strHeader;
	if(bFirstRun)
	{
		bFirstRun = false;
		bBuildHeader = true;
	}

	CvString strLog;
	FILogFile* pLog = NULL;
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

	CvString str;
	CvCity *pCity = NULL;
	int iSpecialists = 0;

	// civ name
	AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);

	// turn
	AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());

	// # cities
	AppendToLog(strHeader, strLog, "# Cities", m_pPlayer->getNumCities());

	// Guilds
	TechTypes eTechDrama = (TechTypes)GC.getInfoTypeForString("TECH_DRAMA", true);
	TechTypes eTechGuilds = (TechTypes)GC.getInfoTypeForString("TECH_GUILDS", true);
	TechTypes eTechAcoustics = (TechTypes)GC.getInfoTypeForString("TECH_ACOUSTICS", true);
	BuildingClassTypes eWritersGuildClass = (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_WRITERS_GUILD", true);
	BuildingClassTypes eArtistsGuildClass = (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_ARTISTS_GUILD", true);
	BuildingClassTypes eMusiciansGuildClass = (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_MUSICIANS_GUILD", true);
	BuildingTypes eWritersGuild = (BuildingTypes)GC.getInfoTypeForString("BUILDING_WRITERS_GUILD", true);
	BuildingTypes eArtistsGuild = (BuildingTypes)GC.getInfoTypeForString("BUILDING_ARTISTS_GUILD", true);
	BuildingTypes eMusiciansGuild = (BuildingTypes)GC.getInfoTypeForString("BUILDING_MUSICIANS_GUILD", true);

	if (eWritersGuildClass != NO_BUILDINGCLASS && eWritersGuild != NO_BUILDING)
	{
		AppendToLog(strHeader, strLog, "Drama", pTeam.GetTeamTechs()->HasTech(eTechDrama) ? "yes" : "no");
		pCity = m_pPlayer->GetFirstCityWithBuildingClass(eWritersGuildClass);
		AppendToLog(strHeader, strLog, "Writers' Guild", (pCity != NULL ? pCity->getName() : ""));
		iSpecialists = (pCity != NULL) ? pCity->GetCityCitizens()->GetNumSpecialistsInBuilding(eWritersGuild) : 0;
		AppendToLog(strHeader, strLog, "Spclsts", iSpecialists);
		AppendToLog(strHeader, strLog, "Slots", GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_LITERATURE()));
	}

	if (eArtistsGuildClass != NO_BUILDINGCLASS && eArtistsGuild != NO_BUILDING)
	{
		AppendToLog(strHeader, strLog, "Guilds", pTeam.GetTeamTechs()->HasTech(eTechGuilds) ? "yes" : "no");
		pCity = m_pPlayer->GetFirstCityWithBuildingClass(eArtistsGuildClass);
		AppendToLog(strHeader, strLog, "Artists' Guild", (pCity != NULL ? pCity->getName() : ""));
		iSpecialists = (pCity != NULL) ? pCity->GetCityCitizens()->GetNumSpecialistsInBuilding(eArtistsGuild) : 0;
		AppendToLog(strHeader, strLog, "Spclsts", iSpecialists);
		AppendToLog(strHeader, strLog, "Slots", GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT()));
	}

	if (eMusiciansGuildClass != NO_BUILDINGCLASS && eMusiciansGuild != NO_BUILDING)
	{
		AppendToLog(strHeader, strLog, "Acoustics", pTeam.GetTeamTechs()->HasTech(eTechAcoustics) ? "yes" : "no");
		pCity = m_pPlayer->GetFirstCityWithBuildingClass(eMusiciansGuildClass);
		AppendToLog(strHeader, strLog, "Musicians' Guild", (pCity != NULL ? pCity->getName() : ""));
		iSpecialists = (pCity != NULL) ? pCity->GetCityCitizens()->GetNumSpecialistsInBuilding(eMusiciansGuild) : 0;
		AppendToLog(strHeader, strLog, "Spclsts", iSpecialists);
		AppendToLog(strHeader, strLog, "Slots", GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_MUSIC()));
	}

	AppendToLog(strHeader, strLog, "Great Works", GetNumGreatWorks());
	AppendToLog(strHeader, strLog, "Tourism", GetTourism() / 100);
	AppendToLog(strHeader, strLog, "Theming Bonuses", GetTotalThemingBonuses());

	if(bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);

	CvCity *pLoopCity = NULL;
	int iLoop = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->GetCityCulture()->GetNumGreatWorks() > 0)
		{
			pLoopCity->GetCityCulture()->LogGreatWorks(pLog);
		}
	}
}

/// Utility function - AppendToLog
void CvPlayerCulture::AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, const CvString& strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void CvPlayerCulture::AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void CvPlayerCulture::AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, float fValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}

void CvPlayerCulture::LogThemedBuilding(int iCityID, BuildingTypes eBuilding, int iBonus)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	CvString strLog;
	FILogFile* pLog = NULL;
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

	CvString strLine;
	CvString strTurn;
	CvString strBonus;
	CvBuildingEntry *pkBldg = GC.getBuildingInfo(eBuilding);
	if (pkBldg)
	{
		strTurn.Format (", %d,", GC.getGame().getGameTurn());
		strBonus.Format (", Bonus: %d", iBonus);
		strLine = strPlayerName + strTurn + "Themed Building in, " + m_pPlayer->getCity(iCityID)->getName() + ", " + pkBldg->GetDescription() + strBonus;

		pLog->Msg(strLine);
	}
}
#if defined(MOD_BALANCE_CORE)
void CvPlayerCulture::LogSwapMultipleWorks(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	CvString strLog;
	FILogFile* pLog = NULL;
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

	CvString strLine;
	strLine.Format ("Acquiring Multiple non-Artifact Works from Players: %d, Discarded: %d, Acquired: %d", (int)eOtherPlayer, iWorkDiscarded, iWorkAcquired);
	CvString strTurn;
	strTurn.Format (", %d,", GC.getGame().getGameTurn());
	strLine = strPlayerName + strTurn + strLine;

	pLog->Msg(strLine);
}
void CvPlayerCulture::LogSwapMultipleArtifacts(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	CvString strLog;
	FILogFile* pLog = NULL;
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

	CvString strLine;
	strLine.Format ("Acquiring Multiple Artifacts/Art from Players: %d, Discarded: %d, Acquired: %d", (int)eOtherPlayer, iWorkDiscarded, iWorkAcquired);
	CvString strTurn;
	strTurn.Format (", %d,", GC.getGame().getGameTurn());
	strLine = strPlayerName + strTurn + strLine;

	pLog->Msg(strLine);
}
#endif
void CvPlayerCulture::LogSwapWorks(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	CvString strLog;
	FILogFile* pLog = NULL;
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

	CvString strLine;
	strLine.Format ("Acquired Work from Player: %d, Discarded: %d, Acquired: %d", (int)eOtherPlayer, iWorkDiscarded, iWorkAcquired);
	CvString strTurn;
	strTurn.Format (", %d,", GC.getGame().getGameTurn());
	strLine = strPlayerName + strTurn + strLine;

	pLog->Msg(strLine);
}

/// Build log filename
CvString CvPlayerCulture::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CultureAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "CultureAILog.csv";
	}

	return strLogName;
}

// SERIALIZATION

template<typename PlayerCulture, typename Visitor>
void CvPlayerCulture::Serialize(PlayerCulture& playerCulture, Visitor& visitor)
{
	visitor(playerCulture.m_aDigCompletePlots);
	visitor(playerCulture.m_iLastTurnLifetimeCulture);
	visitor(playerCulture.m_iLastTurnCPT);

	visitor(playerCulture.m_aiCulturalInfluence);
	visitor(playerCulture.m_aiLastTurnCulturalInfluence);
	visitor(playerCulture.m_aiLastTurnCulturalIPT);

	visitor(playerCulture.m_bReportedTwoCivsAway);
	visitor(playerCulture.m_bReportedOneCivAway);

	visitor(playerCulture.m_eOpinion);
	visitor(playerCulture.m_ePreferredIdeology);
	visitor(playerCulture.m_iOpinionUnhappiness);
	visitor(playerCulture.m_iLastUpdate);
	visitor(playerCulture.m_iLastThemUpdate);
	visitor(playerCulture.m_strOpinionTooltip);
	visitor(playerCulture.m_strOpinionUnhappinessTooltip);
	visitor(playerCulture.m_eOpinionBiggestInfluence);
	visitor(playerCulture.m_iTurnIdeologyAdopted);
	visitor(playerCulture.m_iTurnIdeologySwitch);

	visitor(playerCulture.m_iSwappableWritingIndex);
	visitor(playerCulture.m_iSwappableArtIndex);
	visitor(playerCulture.m_iSwappableArtifactIndex);
	visitor(playerCulture.m_iSwappableMusicIndex);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvPlayerCulture& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvPlayerCulture::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvPlayerCulture& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvPlayerCulture::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvCityCulture
//=====================================
/// Constructor
CvCityCulture::CvCityCulture(void):
m_pCity(NULL)
{
}

/// Destructor
CvCityCulture::~CvCityCulture(void)
{
}

/// Initialize class data
void CvCityCulture::Init(CvCity* pCity)
{
	m_pCity = pCity;
}

/// How many Great Works are in the city?
int CvCityCulture::GetNumGreatWorks(bool bIgnoreYield) const
{
	return m_pCity->GetCityBuildings()->GetNumGreatWorks(bIgnoreYield);
}

/// How many Great Works slots are available in the city? (counting both open and filled and counting all types)
int CvCityCulture::GetNumGreatWorkSlots() const
{
	return (m_pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots() + GetNumGreatWorks());
}

/// How many OPEN Great Works slots of a particular type are available in the city?
int CvCityCulture::GetNumAvailableGreatWorkSlots(GreatWorkSlotType eSlotType) const
{
	return (m_pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eSlotType));
}

/// How many OPEN Great Works slots of a particular type are available in the city?
int CvCityCulture::GetNumFilledGreatWorkSlots(GreatWorkSlotType eSlotType) const
{
	return (m_pCity->GetCityBuildings()->GetNumFilledGreatWorkSlots(eSlotType));
}

/// Clear out Great Works in a city (used by scenarios)
void CvCityCulture::ClearGreatWorks()
{
	for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
		BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass);
		if (eBuilding == NO_BUILDING)
			continue;

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (!pkBuildingInfo)
			continue;

		if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
			for (int iI = 0; iI < iNumSlots; iI++)
			{
				m_pCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, iI, -1);
			}
		}
	}
}

/// Which type of Great Work slot can we add with the cheapest available culture building?
GreatWorkSlotType CvCityCulture::GetSlotTypeFirstAvailableCultureBuilding() const
{
	int iCheapest = MAX_INT;
	GreatWorkSlotType eRtnValue = NO_GREAT_WORK_SLOT;
	std::vector<int> allBuildingCount = GET_PLAYER(m_pCity->getOwner()).GetTotalBuildingCount();

	for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iBuildingClassLoop);
		if (m_pCity->HasBuildingClass(eBuildingClass))
			continue;

		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if (!pkBuildingClassInfo)
			continue;

		BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass);
		if (eBuilding == NO_BUILDING)
			continue;

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (!pkBuildingInfo)
			continue;

		if (!isWorldWonderClass(*pkBuildingClassInfo))
		{
			int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
			if (iNumSlots > 0)
			{
				if (m_pCity->canConstruct(eBuilding, allBuildingCount))
				{
					int iCost = pkBuildingInfo->GetProductionCost();
					if (iCost < iCheapest)
					{
						iCheapest = iCost;
						eRtnValue = pkBuildingInfo->GetGreatWorkSlotType();
					}
				}
			}
		}
	}
	return eRtnValue;
}

#if defined(MOD_BALANCE_CORE)
void CvCityCulture::CalculateBaseTourismBeforeModifiers()
{
	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());
	// If we're in Resistance, then no Tourism!
	if(m_pCity->IsResistance() || m_pCity->IsRazing() || m_pCity->GetNoTourismTurns() > 0)
	{
		m_pCity->SetBaseTourismBeforeModifiers(0);
		return;
	}

	// Ignore those Great Works in storage (ie not generating a yield)
	int iBase = GetNumGreatWorks(false) * (/*2 in CP, 3 in VP*/ GD_INT_GET(BASE_TOURISM_PER_GREAT_WORK) + kPlayer.GetGreatWorkYieldChange(YIELD_TOURISM));
	int iBonus = ((m_pCity->GetCityBuildings()->GetGreatWorksTourismModifier() + kPlayer.GetGreatWorksTourismModifierGlobal()) * iBase / 100);
	iBase += iBonus;

	iBase += m_pCity->GetCityBuildings()->GetCurrentThemingBonuses(YIELD_CULTURE);

	// Add in all the tourism from yields
	iBase += m_pCity->getYieldRate(YIELD_TOURISM, false);

	if(kPlayer.isGoldenAge())
	{
		if(m_pCity->isCapital())
		{
			int iPercent = kPlayer.GetPlayerTraits()->GetTourismGABonus();
			if(iPercent != 0)
			{
				int iCulture = m_pCity->getJONSCulturePerTurn();
				iBase += ((iCulture * iPercent) / 100);
			}
		}
	}

	int iPercent = m_pCity->GetCityBuildings()->GetLandmarksTourismPercent() + kPlayer.GetLandmarksTourismPercentGlobal();
	if (iPercent != 0)
	{
		int iFromWonders = GetCultureFromWonders();
		int iFromNaturalWonders = GetCultureFromNaturalWonders();
		int iFromImprovements = m_pCity->GetBaseYieldRateFromTerrain(YIELD_CULTURE);

		int iFromCity = iFromWonders + iFromNaturalWonders + iFromImprovements;

		iBase += iFromCity * iPercent / 100;
	}

	ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
	if(pReligion)
	{
		int iFaithBuildingTourism = pReligion->m_Beliefs.GetFaithBuildingTourism(m_pCity->getOwner(), m_pCity);
		if (iFaithBuildingTourism != 0)
		{
			iBase += m_pCity->GetCityBuildings()->GetNumBuildingsFromFaith() * iFaithBuildingTourism;
		}

		// If the option to check for all buildings in a class is enabled, we loop through all buildings in the city
		const std::vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
		for(size_t iI = 0; iI < allBuildings.size(); iI++)
		{
			int iCount = m_pCity->GetCityBuildings()->GetNumBuilding(allBuildings[iI]);
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo(allBuildings[iI]);
			if (pkBuilding)
			{
				if (iCount > 0)
				{
					iBase += pReligion->m_Beliefs.GetBuildingClassTourism(pkBuilding->GetBuildingClassType(), m_pCity->getOwner(), m_pCity) * iCount;
				}
			}
		}
	}

	m_pCity->SetBaseTourismBeforeModifiers(max(0, iBase));
}

void CvCityCulture::CalculateBaseTourism()
{
	int iBase = m_pCity->GetBaseTourismBeforeModifiers() * 100;
	if(iBase <= 0)
	{
		m_pCity->SetBaseTourism(0);
		return;
	}
	int iModifier = 0;

	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());
	int iTechSpreadModifier = kPlayer.GetInfluenceSpreadModifier();
	if (iTechSpreadModifier != 0)
	{
		iModifier += iTechSpreadModifier;
	}

	int iLeagueCityModifier = GC.getGame().GetGameLeagues()->GetCityTourismModifier(m_pCity->getOwner(), m_pCity);
	if (iLeagueCityModifier != 0)
	{
		iModifier += iLeagueCityModifier;
	}

	if (kPlayer.isGoldenAge())
	{
		iModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeTourismModifier();
	}
	
	const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
	for(size_t iI = 0; iI < allBuildings.size(); iI++)
	{
		CvBuildingEntry *pkBuilding = GC.getBuildingInfo(allBuildings[iI]);
		if (pkBuilding)
		{
			int iBuildingMod = kPlayer.GetPlayerPolicies()->GetBuildingClassTourismModifier(pkBuilding->GetBuildingClassType());
			iModifier += iBuildingMod * m_pCity->GetCityBuildings()->GetNumBuilding(allBuildings[iI]);
		}
	}

	// City level yield modifiers (eg from buildings, policies, etc)
	int iCityBaseTourismYieldRateMod = m_pCity->getBaseYieldRateModifier(YIELD_TOURISM) - 100;
	if (iCityBaseTourismYieldRateMod != 0)
	{
		iModifier += iCityBaseTourismYieldRateMod;
	}

	if (iModifier != 0)
	{
		iBase *= (100 + iModifier);
		iBase /= 100;
	}
	m_pCity->SetBaseTourism(max(0, iBase));
}
#endif

/// What is the tourism modifier for one player
int CvCityCulture::GetTourismMultiplier(PlayerTypes eTargetPlayer, bool bIgnoreReligion, bool bIgnoreOpenBorders, bool bIgnoreTrade, bool bIgnorePolicies, bool bIgnoreIdeologies) const
{
	int iMultiplier = 0;
	if(eTargetPlayer == NO_PLAYER || m_pCity->getOwner() == NO_PLAYER)
		return 0;
	
	CvPlayer &kTargetPlayer = GET_PLAYER(eTargetPlayer);
	CvTeam &kTargetTeam = GET_TEAM(kTargetPlayer.getTeam());
	
	CvPlayer &kCityPlayer = GET_PLAYER(m_pCity->getOwner());
	CvTeam &kCityTeam = GET_TEAM(kCityPlayer.getTeam());

	if(kCityPlayer.isMinorCiv())
		return 0;

	PolicyBranchTypes eMyIdeology = kCityPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = kTargetPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

	if (!bIgnoreReligion)
	{
		// they share (at least some of) this player's religion
		iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierSharedReligion(eTargetPlayer);
	}

	if (!bIgnoreOpenBorders)
	{
		// Open borders with this player
		if ((MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && kCityTeam.IsAllowsOpenBordersToTeam(kTargetTeam.GetID())) ||
			(!MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && kTargetTeam.IsAllowsOpenBordersToTeam(m_pCity->getTeam())))
		{
			iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierOpenBorders();
		}
	}

	if (!bIgnoreTrade)
	{
		// Trade route to one of this player's cities from here
		if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pCity->getOwner(), eTargetPlayer))
		{
			iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierTradeRoute();
		}
		if (kCityPlayer.isMajorCiv() && kCityPlayer.GetPlayerTraits()->IsNoOpenTrade())
		{
			if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(eTargetPlayer, m_pCity->getOwner(), true))
			{
				iMultiplier += /*-34 in CP, -10 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIFFERENT_IDEOLOGIES);
			}
		}
	}

	if (!bIgnoreIdeologies)
	{
		if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
		{
			iMultiplier += /*-34 in CP, -10 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIFFERENT_IDEOLOGIES);

			if (!MOD_BALANCE_CORE && kCityPlayer.GetEspionage()->IsMyDiplomatVisitingThem(eTargetPlayer))
			{
				iMultiplier += /*25 in CP, 20 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIPLOMAT);
			}
		}
	}
	if (!bIgnorePolicies)
	{
		int iLessHappyMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_LESS_HAPPY);
		if (iLessHappyMod != 0)
		{
			if (kCityPlayer.GetExcessHappiness() > kTargetPlayer.GetExcessHappiness())
			{
				iMultiplier += iLessHappyMod;
			}
		}
		int iCommonFoeMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_COMMON_FOE);
		if (iCommonFoeMod != 0)
		{
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != eTargetPlayer && eLoopPlayer != m_pCity->getOwner() && kCityPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
				{
					// Are they at war with me too?
					if (kCityTeam.isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) && kTargetTeam.isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
					{
						iMultiplier += iCommonFoeMod;
						break;
					}
				}
			}
		}
		int iSharedIdeologyMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY);
		if (iSharedIdeologyMod != 0)
		{
			if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology == eTheirIdeology)
			{
				iMultiplier += iSharedIdeologyMod;
			}
		}
	}

	if(MOD_BALANCE_CORE)
	{
		if (kCityPlayer.GetEspionage() && kCityPlayer.GetEspionage()->IsMyDiplomatVisitingThem(eTargetPlayer))
		{
			iMultiplier += /*25 in CP, 20 in VP*/ GD_INT_GET(TOURISM_MODIFIER_DIPLOMAT);
		}
	}
	if (MOD_BALANCE_VP)
	{
		int iBoredom = kTargetPlayer.GetCulture()->GetBoredomCache();
		int iDelta = iBoredom - kCityPlayer.GetCulture()->GetBoredomCache();
		if (iDelta > 0)
		{
			iMultiplier += iDelta;
		}
	}

	//Corporations
	iMultiplier += kCityPlayer.GetCulture()->GetFranchiseModifier(eTargetPlayer);

	if (kCityPlayer.getTourismBonusTurnsPlayer(eTargetPlayer) > 0)
	{
		iMultiplier += /*200 in CP, 100 in VP*/ GD_INT_GET(TEMPORARY_TOURISM_BOOST_MOD) * 2;
	}
	else if (kCityPlayer.GetTourismBonusTurns() > 0)
	{
		iMultiplier += /*100 in CP, 50 in VP*/ GD_INT_GET(TEMPORARY_TOURISM_BOOST_MOD);
	}


	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague != NULL)
	{
		if (pLeague->GetTourismMod() != 0)
		{
			iMultiplier += (pLeague->GetTourismMod());
		}
	}
	if (kCityPlayer.GetPositiveWarScoreTourismMod() != 0)
	{
		int iWarScore = kCityPlayer.GetDiplomacyAI()->GetHighestWarscore();
		if (iWarScore > 0)
		{
				iMultiplier += iWarScore;
		}		
	}
	// My vassal
	if (kTargetTeam.GetMaster() == kCityPlayer.getTeam())
	{
		iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierVassal();
	}

	// City difference - do we have more than them?
	int iNumCities = kCityPlayer.GetNumEffectiveCities() - kTargetPlayer.GetNumEffectiveCities();
	if (iNumCities > 0)
	{
		// Mod for City Count
		int iMod = /*0 in CP, 5 in VP*/ GC.getMap().getWorldInfo().GetNumCitiesTourismCostMod();
		iMod -= kCityPlayer.GetTourismCostXCitiesMod();

		iMod *= iNumCities;

		iMultiplier -= iMod;
	}
	// LATER add top science city and research agreement with this player???

	return max(/*-100*/ max(GD_INT_GET(MINIMUM_TOURISM_MODIFIER), -100), iMultiplier);
}

/// What is the tooltip describing the tourism output?
CvString CvCityCulture::GetTourismTooltip()
{
	CvString szRtnValue = "";
	CvString szTemp;
	CvPlayer &kCityPlayer = GET_PLAYER(m_pCity->getOwner());
	// Great Works
	// Ignore those Great Works in storage, ie not generating a yield
	int iGWTourism = GetNumGreatWorks(false) * (/*2 in CP, 3 in VP*/ GD_INT_GET(BASE_TOURISM_PER_GREAT_WORK) + kCityPlayer.GetGreatWorkYieldChange(YIELD_TOURISM)) + m_pCity->GetGreatWorkYieldChange(YIELD_TOURISM);
	iGWTourism += ((m_pCity->GetCityBuildings()->GetGreatWorksTourismModifier() + kCityPlayer.GetGreatWorksTourismModifierGlobal()) * iGWTourism / 100);
	szRtnValue = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_GREAT_WORKS", iGWTourism, m_pCity->GetCityCulture()->GetNumGreatWorks());

	int iThemingBonuses = m_pCity->GetCityBuildings()->GetCurrentThemingBonuses(YIELD_CULTURE);
	if (iThemingBonuses != 0)
	{
		szRtnValue += "[NEWLINE][NEWLINE]";
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_THEMING_BONUSES", iThemingBonuses);
	}

	int iTraitBonuses = m_pCity->GetYieldPerTurnFromTraits(YIELD_TOURISM);
	if (iTraitBonuses != 0)
	{
		szRtnValue += "[NEWLINE][NEWLINE]";
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_TRAIT_BONUSES", iTraitBonuses);
	}

	// Landmarks, Wonders, Natural Wonders, Improvements
	int iTileTourism = 0;
	int iPercent = m_pCity->GetCityBuildings()->GetLandmarksTourismPercent() + kCityPlayer.GetLandmarksTourismPercentGlobal();
	if (iPercent != 0)
	{
		int iFromWonders = GetCultureFromWonders();
		int iFromNaturalWonders = GetCultureFromNaturalWonders();
		int iFromImprovements = m_pCity->GetBaseYieldRateFromTerrain(YIELD_CULTURE);
		iTileTourism = ((iFromWonders + iFromNaturalWonders + iFromImprovements) * iPercent / 100);
		if (!szRtnValue.empty())
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_TILES", iTileTourism, iPercent);
	}
#if defined(MOD_BALANCE_CORE)
	if(m_pCity->isCapital() && kCityPlayer.isGoldenAge())
	{
		// Landmarks, Wonders, Natural Wonders, Improvements
		int iGATourism = 0;
		int iPercent = kCityPlayer.GetPlayerTraits()->GetTourismGABonus();
		if (iPercent != 0)
		{
			int iCulture = m_pCity->getJONSCulturePerTurn();
			iGATourism = ((iCulture * iPercent) / 100);
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_GA_BONUS", iGATourism, iPercent);
		}
	}
#endif
	// Beliefs
	int iSacredSitesTourism = 0;
	int iReligiousArtTourism = 0;
	ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
	if(pReligion)
	{
		int iFaithBuildingTourism = pReligion->m_Beliefs.GetFaithBuildingTourism(m_pCity->getOwner(), m_pCity);
		if (iFaithBuildingTourism != 0)
		{
			iSacredSitesTourism = m_pCity->GetCityBuildings()->GetNumBuildingsFromFaith() * iFaithBuildingTourism;
		}
		if (iSacredSitesTourism > 0)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_FAITH_BUILDINGS", iSacredSitesTourism);
		}

		const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
		for(size_t iI = 0; iI < allBuildings.size(); iI++)
		{
			BuildingTypes eBuilding = allBuildings[iI];
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
			if (pkBuilding)
			{
				iReligiousArtTourism += pReligion->m_Beliefs.GetBuildingClassTourism(pkBuilding->GetBuildingClassType(), 
					m_pCity->getOwner(), m_pCity) * m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding);
			}
		}
		if(iReligiousArtTourism != 0)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_RELIGIOUS_ART", iReligiousArtTourism);
		}
	}

#if defined(MOD_BALANCE_CORE)
	//Buildings with Tourism
	int iTourismFromWW = kCityPlayer.GetYieldChangeWorldWonder(YIELD_TOURISM);
	if(iTourismFromWW != 0)
	{
		iTourismFromWW *= m_pCity->getNumWorldWonders();
		if(iTourismFromWW != 0)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_POLICY_AESTHETICS", iTourismFromWW);
		}
	}
	int iRemainder = (m_pCity->getYieldRate(YIELD_TOURISM, false) - iTraitBonuses - iTourismFromWW - m_pCity->GetBaseYieldRateFromGreatWorks(YIELD_TOURISM));
	if(iRemainder != 0)
	{
		if (!szRtnValue.empty())
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_GENERAL", iRemainder);
	}
#endif

	const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
	for(size_t iI = 0; iI < allBuildings.size(); iI++)
	{
		CvBuildingEntry *pkBuilding = GC.getBuildingInfo(allBuildings[iI]);
		if (pkBuilding)
		{
			int iBuildingMod = kCityPlayer.GetPlayerPolicies()->GetBuildingClassTourismModifier(pkBuilding->GetBuildingClassType());
			if (iBuildingMod != 0)
			{
				iBuildingMod *= m_pCity->GetCityBuildings()->GetNumBuilding(allBuildings[iI]);

				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE][NEWLINE]";
				}
				szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_BUILDING_BONUS", iBuildingMod, pkBuilding->GetDescription());
				szRtnValue += szTemp;
			}
		}
	}

	// City level yield modifiers (eg from buildings, policies, etc)
	bool bHasCityModTooltip = false;
	int iTempMod = 0;
	iTempMod = m_pCity->getYieldRateModifier(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD", iTempMod);
	}

	CvPlot* pCityPlot = m_pCity->plot();
	for (int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
	{
		iTempMod = pCityPlot->getUnitByIndex(iUnitLoop)->GetYieldModifier(YIELD_TOURISM);
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_UNITPROMOTION", iTempMod);
		}
	}

	iTempMod = m_pCity->getResourceYieldRateModifier(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_RESOURCES", iTempMod);
	}

	iTempMod = m_pCity->getHappinessModifier(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_HAPPINESS", iTempMod);
	}

	CvArea* pArea =m_pCity->plot()->area();
	if (pArea != NULL)
	{
		iTempMod = pArea->getYieldRateModifier(m_pCity->getOwner(), YIELD_TOURISM);
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_AREA", iTempMod);
		}
	}

	iTempMod = kCityPlayer.getYieldRateModifier(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_PLAYER", iTempMod);
	}

	if (m_pCity->isCapital())
	{
		iTempMod = kCityPlayer.getCapitalYieldRateModifier(YIELD_TOURISM);
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_CAPITAL", iTempMod);
		}
	}

#if defined(MOD_BALANCE_CORE)
	iTempMod = (m_pCity->GetTradeRouteCityMod(YIELD_TOURISM));
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_CORPORATION", iTempMod);
	}

	iTempMod = m_pCity->GetYieldModifierFromHappiness(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_YIELD_MODIFIER_HAPPINESS", iTempMod);
	}

	iTempMod = m_pCity->GetYieldModifierFromHealth(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_YIELD_MODIFIER_HEALTH", iTempMod);
	}

	iTempMod = m_pCity->GetYieldModifierFromDevelopment(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_YIELD_MODIFIER_CRIME", iTempMod);
	}

	iTempMod = m_pCity->GetYieldModifierFromDevelopment(YIELD_TOURISM);
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_YIELD_MODIFIER_DEVELOPMENT", iTempMod);
	}

	iTempMod = min(20, (kCityPlayer.getYieldModifierFromGreatWorks(YIELD_TOURISM) * m_pCity->GetCityBuildings()->GetNumGreatWorks()));
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_GREAT_WORKS", iTempMod);
	}
	iTempMod = min(30, (kCityPlayer.getYieldModifierFromActiveSpies(YIELD_TOURISM) * kCityPlayer.GetSpyPoints(true) / 100));
	if (iTempMod != 0)
	{
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_SPIES", iTempMod);
	}
#endif

	// Golden Age Yield Modifier
	if (kCityPlayer.isGoldenAge())
	{
		CvYieldInfo* pYield = GC.getYieldInfo(YIELD_TOURISM);
		if (pYield)
		{
			iTempMod = pYield->getGoldenAgeYieldMod();
			if (iTempMod != 0)
			{
				if (!bHasCityModTooltip)
				{
					if (!szRtnValue.empty())
					{
						szRtnValue += "[NEWLINE]";
					}
					bHasCityModTooltip = true;
				}
				szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_GOLDEN_AGE", iTempMod);
			}
		}
#if defined(MOD_BALANCE_CORE)
		iTempMod = m_pCity->GetGoldenAgeYieldMod(YIELD_TOURISM);
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_GOLDEN_AGE_BUILDINGS", iTempMod);
		}

		iTempMod = kCityPlayer.getGoldenAgeYieldMod(YIELD_TOURISM);
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_GOLDEN_AGE_POLICIES", iTempMod);
		}
#endif
	}

	// Religion Yield Rate Modifier
	if (pReligion)
	{
		int max = 0;
		int iReligionYieldMaxFollowersPercent = pReligion->m_Beliefs.GetMaxYieldModifierPerFollowerPercent(max, YIELD_TOURISM, m_pCity->getOwner(), kCityPlayer.getCity(m_pCity->GetID()));
		if (iReligionYieldMaxFollowersPercent > 0)
		{
			int iVal = m_pCity->GetCityReligions()->GetNumFollowers(eMajority) * iReligionYieldMaxFollowersPercent;
			iVal /= 100;

			if (iVal <= 0)
				iVal = 1;

			iTempMod = min(max, iReligionYieldMaxFollowersPercent);
			if (iTempMod != 0)
			{
				if (!bHasCityModTooltip)
				{
					if (!szRtnValue.empty())
					{
						szRtnValue += "[NEWLINE]";
					}
					bHasCityModTooltip = true;
				}
				szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_BELIEF", iTempMod);
			}
		}
		else
		{
			int iReligionYieldMaxFollowers = pReligion->m_Beliefs.GetMaxYieldModifierPerFollower(YIELD_TOURISM, m_pCity->getOwner(), kCityPlayer.getCity(m_pCity->GetID()));
			if (iReligionYieldMaxFollowers > 0)
			{
				int iFollowers = m_pCity->GetCityReligions()->GetNumFollowers(eMajority);
				iTempMod = min(iFollowers, iReligionYieldMaxFollowers);
				if (iTempMod != 0)
				{
					if (!bHasCityModTooltip)
					{
						if (!szRtnValue.empty())
						{
							szRtnValue += "[NEWLINE]";
						}
						bHasCityModTooltip = true;
					}
					szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_BELIEF", iTempMod);
				}
			}
		}
	}

	if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		// Do we get increased yields from a resource monopoly?
		int iTempMod = kCityPlayer.getCityYieldModFromMonopoly(YIELD_TOURISM);
		if (iTempMod != 0)
		{
			iTempMod += kCityPlayer.GetMonopolyModPercent();
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_BELIEF", iTempMod);
		}
	}

	ReligionTypes eStateReligion = kCityPlayer.GetReligions()->GetStateReligion();
	if (MOD_BALANCE_CORE_BELIEFS && eStateReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eStateReligion, m_pCity->getOwner());
		if (pReligion)
		{
			int iGoldenAge = pReligion->m_Beliefs.GetYieldBonusGoldenAge(YIELD_TOURISM, m_pCity->getOwner(), kCityPlayer.getCity(m_pCity->GetID()), true);
			if (iGoldenAge > 0)
			{
				if (kCityPlayer.getGoldenAgeTurns() > 0)
				{
					iTempMod = iGoldenAge;
					if (iTempMod != 0)
					{
						if (!bHasCityModTooltip)
						{
							if (!szRtnValue.empty())
							{
								szRtnValue += "[NEWLINE]";
							}
							bHasCityModTooltip = true;
						}
						szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_YIELD_GOLDEN_AGE_RELIGION", iTempMod);
					}
				}
			}
			int iWLTKD = pReligion->m_Beliefs.GetYieldFromWLTKD(YIELD_TOURISM, m_pCity->getOwner(), kCityPlayer.getCity(m_pCity->GetID()));
			if (iWLTKD != 0)
			{
				if (m_pCity->GetWeLoveTheKingDayCounter() > 0)
				{
					iTempMod = iWLTKD;
					if (iTempMod != 0)
					{
						if (!bHasCityModTooltip)
						{
							if (!szRtnValue.empty())
							{
								szRtnValue += "[NEWLINE]";
							}
							bHasCityModTooltip = true;
						}
						szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_WLTKD_RELIGION", iTempMod);
					}
				}
			}
		}
	}

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague)
	{
		iTempMod = pLeague->GetCityTourismModifier(m_pCity);
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_TOURISM_MOD_HOLY_CITY", iTempMod);
		}
	}

	if (m_pCity->GetWeLoveTheKingDayCounter() > 0)
	{
		iTempMod = (m_pCity->GetYieldFromWLTKD(YIELD_TOURISM) + kCityPlayer.GetYieldFromWLTKD(YIELD_TOURISM));
		if (iTempMod != 0)
		{
			if (!bHasCityModTooltip)
			{
				if (!szRtnValue.empty())
				{
					szRtnValue += "[NEWLINE]";
				}
				bHasCityModTooltip = true;
			}
			szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_WLTKD", iTempMod);
		}
	}

	if (m_pCity->IsPuppet())
	{
		iTempMod = kCityPlayer.GetPuppetYieldPenaltyMod() + kCityPlayer.GetPlayerTraits()->GetPuppetPenaltyReduction() + /*0 in CP, -80 in VP*/ GD_INT_GET(PUPPET_TOURISM_MODIFIER);
		if (kCityPlayer.GetPlayerTraits()->GetPuppetPenaltyReduction() != 0 && iTempMod > 0)
			iTempMod = 0;
		if (!bHasCityModTooltip)
		{
			if (!szRtnValue.empty())
			{
				szRtnValue += "[NEWLINE]";
			}
			bHasCityModTooltip = true;
		}
		szRtnValue += GetLocalizedText("TXT_KEY_PRODMOD_PUPPET", iTempMod);
	}

	return szRtnValue;
}

/// What is the tooltip describing the tourism output?
CvString CvCityCulture::GetFilledSlotsTooltip()
{
	CvString szRtnValue = "";
	const int iGWWriting = m_pCity->GetCityBuildings()->GetNumGreatWorks(CvTypes::getGREAT_WORK_SLOT_LITERATURE());
	const int iGWArt = m_pCity->GetCityBuildings()->GetNumGreatWorks(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT());
	const int iGWMusic = m_pCity->GetCityBuildings()->GetNumGreatWorks(CvTypes::getGREAT_WORK_SLOT_MUSIC());
	szRtnValue = GetLocalizedText("TXT_KEY_CO_GREAT_WORK_TT", iGWWriting, iGWArt, iGWMusic);

	return szRtnValue;
}

/// What is the tooltip describing the tourism output?
CvString CvCityCulture::GetTotalSlotsTooltip()
{
	CvString szRtnValue = "";
	CvString szTemp1;
	CvString szTemp2;
	CvString szTemp3;
	
	GreatWorkSlotType eLiteratureSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();
	int iFilledWriting = m_pCity->GetCityBuildings()->GetNumGreatWorks(eLiteratureSlot);
	int iGWWriting = iFilledWriting + m_pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eLiteratureSlot);

	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	int iFilledArt = m_pCity->GetCityBuildings()->GetNumGreatWorks(eArtArtifactSlot);
	int iGWArt = iFilledArt + m_pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);

	GreatWorkSlotType eMusicSlot = CvTypes::getGREAT_WORK_SLOT_MUSIC();
	int iFilledMusic = m_pCity->GetCityBuildings()->GetNumGreatWorks(eMusicSlot);
	int iGWMusic = iFilledMusic + m_pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eMusicSlot);

	szTemp1 = GetLocalizedText("TXT_KEY_CO_GREAT_WORK_SLOTS_TT_ENTRY", iFilledWriting, iGWWriting);
	szTemp2 = GetLocalizedText("TXT_KEY_CO_GREAT_WORK_SLOTS_TT_ENTRY", iFilledArt, iGWArt);
	szTemp3 = GetLocalizedText("TXT_KEY_CO_GREAT_WORK_SLOTS_TT_ENTRY", iFilledMusic, iGWMusic);
	szRtnValue = GetLocalizedText("TXT_KEY_CO_GREAT_WORK_SLOTS_TT", szTemp1, szTemp2, szTemp3);

	return szRtnValue;
}

// Does this building ever give a theming bonus
bool CvCityCulture::IsThemingBonusPossible(BuildingClassTypes eBuildingClass) const
{
	const BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass, true);
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	return pkBuildingInfo && pkBuildingInfo->GetThemingBonusInfo(0);
}

int CvCityCulture::GetThemingBonus(BuildingClassTypes eBuildingClass) const
{
	CvPlayer& kPlayer = GET_PLAYER(m_pCity->getOwner());
	int iRtnValue = 0;

	if (IsThemingBonusPossible(eBuildingClass))
	{
		int iIndex = GetThemingBonusIndex(eBuildingClass);
		if (iIndex >= 0)
		{
			BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass, true);
			if (eBuilding == NO_BUILDING)
				return iRtnValue;

			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (!pkBuildingInfo)
				return iRtnValue;

			int iBonus = pkBuildingInfo->GetThemingBonusInfo(iIndex)->GetBonus();
			int iModifier = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_THEMING_BONUS);
			iRtnValue = iBonus * (100 + iModifier) / 100;
			if (m_pCity->isCapital())
			{
				iModifier = kPlayer.GetPlayerTraits()->GetCapitalThemingBonusModifier();
				if (iModifier != 0)
				{
					iRtnValue = iRtnValue * (100 + iModifier) / 100;

					if (MOD_API_ACHIEVEMENTS && kPlayer.isHuman() && !GC.getGame().isGameMultiPlayer() && iRtnValue >= 16)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_40);
					}
				}
			}
		}
	}

	return iRtnValue;
}

CvString CvCityCulture::GetThemingTooltip(BuildingClassTypes eBuildingClass) const
{
	CvString szRtnValue = "";
	CvString szTextKey = "";

	if (!IsThemingBonusPossible(eBuildingClass))
		return szRtnValue;

	BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass, true);
	if (eBuilding == NO_BUILDING)
		return szRtnValue;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if (!pkBuildingInfo)
		return szRtnValue;

	int iIndex = GetThemingBonusIndex(eBuildingClass);
	if (iIndex != -1)
	{
		CvThemingBonusInfo* pkThemingInfo = pkBuildingInfo->GetThemingBonusInfo(iIndex);
		ASSERT_DEBUG(pkThemingInfo, "Couldn't find theming bonus info");

		CvString szBonusString = "";
		szBonusString.Format("+%d [ICON_TOURISM]/[ICON_CULTURE]: ", GetThemingBonus(eBuildingClass));

		CvString szThemeDescription = "";
		int iGreatWork = m_pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, 0);
		if (pkThemingInfo && iGreatWork != -1)
		{
			CvGreatWork work = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWork];
			CvString szEraString = GC.getEraInfo(work.m_eEra)->getShortDesc();
			CvString szCivAdj = pkThemingInfo->IsRequiresAnyButOwner() ? "TXT_KEY_CO_WORLD_MUSEUM" : GET_PLAYER(work.m_ePlayer).getCivilizationAdjectiveKey();
			szThemeDescription = GetLocalizedText(pkThemingInfo->GetDescription(), szEraString, szCivAdj);
		}

		szRtnValue = szBonusString + szThemeDescription;
		szRtnValue += "[NEWLINE]";
		szTextKey = "TXT_KEY_CO_TOURISM_THEME_BONUS";
	}
	else
	{
		szRtnValue = GetLocalizedText("TXT_KEY_CO_NO_THEME");
		szRtnValue += "[NEWLINE][NEWLINE]";
		szRtnValue += GetLocalizedText(pkBuildingInfo->GetThemingBonusHelp());
		szRtnValue += "[NEWLINE]";
		szTextKey = "TXT_KEY_CO_TOURISM_THEME_BONUS_NEEDED";
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++) 
	{
		YieldTypes eYield = static_cast<YieldTypes>(iYield);
		CvYieldInfo* pkYieldInfo = GC.getYieldInfo(eYield);
		int iYieldBonus = pkBuildingInfo->GetThemingYieldBonus(iYield);
		if (iYieldBonus != 0)
		{
			szRtnValue += "[NEWLINE]";
			szRtnValue += GetLocalizedText(szTextKey.c_str(), pkBuildingInfo->GetThemingYieldBonus(iYield), pkYieldInfo->getIconString(), pkYieldInfo->GetDescription());
		}
	}

	return szRtnValue;
}

// PRIVATE FUNCTIONS

/// City's current culture from wonders
int CvCityCulture::GetCultureFromWonders() const
{
	int iRtnValue = 0;

	const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
	for(size_t iI = 0; iI < allBuildings.size(); iI++)
	{
		CvBuildingEntry *pkBuilding = GC.getBuildingInfo(allBuildings[iI]);
		if (pkBuilding)
		{
			if (isWorldWonderClass(pkBuilding->GetBuildingClassInfo()))
			{
				iRtnValue += pkBuilding->GetYieldChange(YIELD_CULTURE);
				iRtnValue += GC.getGame().GetGameLeagues()->GetWorldWonderYieldChange(m_pCity->getOwner(), YIELD_CULTURE);
			}
		}
	}

	return iRtnValue;
}

/// City's current culture from natural wonders
int CvCityCulture::GetCultureFromNaturalWonders() const
{
	int iRtnValue = 0;

	// Look at all workable Plots
	const std::vector<int>& vWorkedPlots =  m_pCity->GetCityCitizens()->GetWorkedPlots();
	for (size_t ui=0; ui<vWorkedPlots.size(); ui++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(vWorkedPlots[ui]);
		if (pLoopPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pLoopPlot->getFeatureType())->IsNaturalWonder(true))
		{
			iRtnValue += pLoopPlot->getYield(YIELD_CULTURE);
		}
	}
	return iRtnValue;
}

/// Log out data on Great Works in this city
void CvCityCulture::LogGreatWorks(FILogFile* pLog)
{
	CvString strMsg;
	strMsg = m_pCity->getName() + ", ";
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingClassTypes eBldgClass = (BuildingClassTypes)iI;
		
		for (int jJ = 0; jJ < 4; jJ++)
		{
			int iIndex = m_pCity->GetCityBuildings()->GetBuildingGreatWork(eBldgClass, jJ);
			if (iIndex != -1)
			{
				CvString strTemp;
				strTemp.Format("%d, ", iIndex);
				strMsg += strTemp;
			}
		}
	}
	pLog->Msg(strMsg);
}

/// Which of the theming bonuses for this building is active
int CvCityCulture::GetThemingBonusIndex(BuildingClassTypes eBuildingClass) const
{  
	BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass);
	if (eBuilding != NO_BUILDING && m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		return m_pCity->GetCityBuildings()->GetThemingBonusIndex(eBuilding);

	return -1;
}

/// Which of the theming bonuses for this building is active
void CvCityCulture::UpdateThemingBonusIndex(BuildingClassTypes eBuildingClass)
{
	vector<int> aGreatWorkIndices;
	BuildingTypes eBuilding = m_pCity->GetBuildingTypeFromClass(eBuildingClass);
	if (eBuilding == NO_BUILDING)
		return;

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if (!pkBuildingInfo)
		return;

	m_pCity->GetCityBuildings()->SetThemingBonusIndex(eBuilding, -1);

	if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) <= 0)
		return;

	int iNumSlots = pkBuildingInfo->GetGreatWorkCount();
	if (m_pCity->GetCityBuildings()->GetNumGreatWorksInBuilding(eBuildingClass) < iNumSlots)
		return;

	// Store info on the attributes of all our Great Works
	for (int iI = 0; iI < iNumSlots; iI++)
	{
		int iGreatWork = m_pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
		aGreatWorkIndices.push_back(iGreatWork);
	}

	int iValue = CultureHelpers::GetThemingBonusIndex(m_pCity->getOwner(), pkBuildingInfo, aGreatWorkIndices);
	m_pCity->GetCityBuildings()->SetThemingBonusIndex(eBuilding, iValue);
}

// HELPER FUNCTIONS

/// Build a name for this artifact
GreatWorkType CultureHelpers::GetArtifact(CvPlot *pPlot)
{
	CvArchaeologyData archData = pPlot->GetArchaeologicalRecord();

	//somehow got a bad artifact pull? Quickly generate one.
	if (archData.m_eArtifactType == NO_GREAT_WORK_ARTIFACT_CLASS)
	{
		pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_ANCIENT_RUIN(), GC.getGame().getActivePlayer(), NO_PLAYER);
		archData = pPlot->GetArchaeologicalRecord();
	}

	// Writing?  If so we already know which one it is
	if (archData.m_eArtifactType == CvTypes::getARTIFACT_WRITING())
	{
		return archData.m_eWork;
	}

	// Otherwise normal retrieval of Great Work
	GreatWorkType eGreatWork = NO_GREAT_WORK;

	//Developer Note:
	//This could probably be shrunk down into a single SQL query but for now I'll leave it as 2.
	//The idea here is that we grab all possible items for a specific era and then pick 1 at random.
	//If there are no items for that era, we use NULL and grab those items.
	//Since era-agnostic and era-specific items are mutually exclusive, we must first check for the the 
	//existence of era-specific items.
	const char* szSql = "SELECT gw.ID FROM GreatWorks as gw "
						"left outer join Eras on gw.EraType == Eras.Type "
						"inner join GreatWorkArtifactClasses on gw.ArtifactClassType == GreatWorkArtifactClasses.Type "
						"where gw.GreatWorkClassType = 'GREAT_WORK_ARTIFACT' and Eras.ID is ? and GreatWorkArtifactClasses.ID == ? "
						"order by Random() limit 1;";

	const char* szExistsSQL =	"select 1 from GreatWorks as gw "
								"inner join Eras on gw.EraType == Eras.Type "
								"inner join GreatWorkArtifactClasses on gw.ArtifactClassType == GreatWorkArtifactClasses.Type "
								"where gw.GreatWorkClassType = 'GREAT_WORK_ARTIFACT' and Eras.ID = ? and GreatWorkArtifactClasses.ID == ? Limit 1;";

	Database::Connection* db = GC.GetGameDatabase();

	bool bHasEraSpecificItems = false;

	if(archData.m_eEra != NO_ERA)
	{
		Database::Results kEraItemsExistQ;
		if(db->Execute(kEraItemsExistQ, szExistsSQL))
		{
			kEraItemsExistQ.Bind(1, archData.m_eEra);
			kEraItemsExistQ.Bind(2, archData.m_eArtifactType);

			if(kEraItemsExistQ.Step())
			{
				bHasEraSpecificItems = true;	
			}
		}
	}

	Database::Results kQuery;
	if(db->Execute(kQuery, szSql))
	{
		if(bHasEraSpecificItems)
			kQuery.Bind(1, archData.m_eEra);
		else
			kQuery.BindNULL(1);

		kQuery.Bind(2, archData.m_eArtifactType);

		if(kQuery.Step())
		{
			eGreatWork = static_cast<GreatWorkType>(kQuery.GetInt(0));
		}
	}

	return eGreatWork;
}

GreatWorkClass CultureHelpers::GetGreatWorkClass(GreatWorkType eGreatWork)
{
	GreatWorkClass eClass = NO_GREAT_WORK_CLASS;

	Database::Connection* db = GC.GetGameDatabase();
	if(db != NULL)
	{
		Database::Results kQuery;
		const char* szSQL = "SELECT GreatWorkClasses.ID from GreatWorkClasses inner join GreatWorks on GreatWorkClassType = GreatWorkClasses.Type where GreatWorks.ID = ?";
		if(db->Execute(kQuery, szSQL))
		{
			kQuery.Bind(1, eGreatWork);

			if(kQuery.Step())
			{
				eClass = static_cast<GreatWorkClass>(kQuery.GetInt(0));
			}
		}
	}

	return eClass;
}

CvString CultureHelpers::GetGreatWorkName(GreatWorkType eGreatWork)
{
	CvString strGreatWorkName;

	Database::Connection* db = GC.GetGameDatabase();
	if(db != NULL)
	{
		Database::Results kQuery;
		if(db->Execute(kQuery, "SELECT Description FROM GreatWorks WHERE ID = ?"))
		{
			kQuery.Bind(1, eGreatWork);

			if(kQuery.Step())
			{
				strGreatWorkName = kQuery.GetText(0);
			}
		}
	}

	return strGreatWorkName;
}

CvString CultureHelpers::GetGreatWorkAudio(GreatWorkType eGreatWorkType)
{
	CvString strGreatWorkAudio;

	Database::Connection* db = GC.GetGameDatabase();
	if(db != NULL)
	{
		Database::Results kQuery;
		if(db->Execute(kQuery, "SELECT Audio FROM GreatWorks WHERE ID = ?"))
		{
			kQuery.Bind(1, eGreatWorkType);

			if(kQuery.Step())
			{
				strGreatWorkAudio = kQuery.GetText(0);
			}
		}
	}

	return strGreatWorkAudio;
}

GreatWorkSlotType CultureHelpers::GetGreatWorkSlot(GreatWorkType eType)
{
	GreatWorkSlotType eSlot = NO_GREAT_WORK_SLOT;
	Database::Connection* db = GC.GetGameDatabase();
	if(db != NULL)
	{
		const char* szSQL = "SELECT GreatWorkSlots.ID from GreatWorks "
							"inner join GreatWorkClasses  on GreatWorks.GreatWorkClassType == GreatWorkClasses .Type "
							"inner join GreatWorkSlots on GreatWorkClasses.SlotType == GreatWorkSlots.Type where GreatWorks .ID = ? LIMIT 1";

		Database::Results kResults;
		if(db->Execute(kResults, szSQL))
		{
			kResults.Bind(1, eType);

			if(kResults.Step())
			{
				eSlot = static_cast<GreatWorkSlotType>(kResults.GetInt(0));
			}
		}
	}

	return eSlot;
}

int CultureHelpers::GetThemingBonusIndex(PlayerTypes eOwner, CvBuildingEntry *pkEntry, vector<int> &aGreatWorkIndices)
{
	int iCountArt = 0;
	int iCountArtifact = 0;
	set<EraTypes> aErasSeen;
	set<PlayerTypes> aPlayersSeen;

	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	GreatWorkClass eArtifactClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");
	GreatWorkClass eArtClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ART");

	if (pkEntry)
	{
		int iNumSlots = pkEntry->GetGreatWorkCount();
		if (aGreatWorkIndices.size() != iNumSlots)
		{
			return -1;  // No theming bonus if some slots still empty or too many entries
		}

		EraTypes eFirstEra = (EraTypes) GC.getNumEraInfos();
		EraTypes eLastEra = NO_ERA;

		// Store info on the attributes of all our Great Works
		for (int iI = 0; iI < iNumSlots; iI++)
		{
			int iGreatWork = aGreatWorkIndices[iI];
			CvGreatWork work = pCulture->m_CurrentGreatWorks[iGreatWork];

			// Check Great Work class
			if (work.m_eClassType == eArtifactClass)
			{
				iCountArtifact++;
			}
			else if (work.m_eClassType == eArtClass)
			{
				iCountArt++;
			}

			// Store era and player
			aErasSeen.insert(work.m_eEra);
			aPlayersSeen.insert(work.m_ePlayer);

			if (work.m_eEra < eFirstEra) 
			{
				eFirstEra = work.m_eEra;
			}
			if (work.m_eEra > eLastEra) 
			{
				eLastEra = work.m_eEra;
			}
		}

		// Now see if we match a theme bonus
		int iNumThemes = pkEntry->GetNumThemingBonuses();
		for (int jJ = 0; jJ < iNumThemes; jJ++)
		{
			bool bValid = true;

			// Can we rule this out based on type?
			CvThemingBonusInfo *bonusInfo = pkEntry->GetThemingBonusInfo(jJ);
			if (bValid && bonusInfo->IsMustBeArt() && iCountArtifact > 0)
			{
				bValid = false;
			}
			if (bValid && bonusInfo->IsMustBeArtifact() && iCountArt > 0)
			{
				bValid = false;
			}
			if (bValid && bonusInfo->IsMustBeEqualArtArtifact() && iCountArt != iCountArtifact)
			{
				bValid = false;
			}

			// Can we rule this out based on era?
			if (bValid && bonusInfo->IsSameEra() && aErasSeen.size()>1)
			{
				bValid = false;
			}

			if (bValid && bonusInfo->IsUniqueEras() && aErasSeen.size()<(size_t)iNumSlots)
			{
				bValid = false;
			}

			if (bValid && bonusInfo->IsConsecutiveEras())
			{
				if ((eLastEra - eFirstEra + 1) != iNumSlots)
				{
					bValid = false;
				}
			}

			// Can we rule this out based on player?
			if (bValid && bonusInfo->IsRequiresOwner() && (aPlayersSeen.size()>1 || *(aPlayersSeen.begin())!=eOwner) )
			{
				bValid = false;
			}

			if (bValid && bonusInfo->IsRequiresSamePlayer() && aPlayersSeen.size()>1)
			{
				bValid = false;
			}

			if (bValid && bonusInfo->IsRequiresUniquePlayers() && aPlayersSeen.size()<(size_t)iNumSlots)
			{
				bValid = false;
			}
			if (bValid && bonusInfo->IsRequiresAnyButOwner() && aPlayersSeen.find(eOwner)!=aPlayersSeen.end() )
			{
				bValid = false;
			}

			// Haven't ruled it out?  Then this is it (ASSUMES THEMING BONUSES FOR A BUILDING ARE IN SORTED ORDER IN DB!)
			if (bValid)
			{
				return jJ;
			}
		}
	}
	return -1;
}
#if defined(MOD_BALANCE_CORE)
bool CultureHelpers::IsValidForForeignThemingBonus(CvThemingBonusInfo *pBonusInfo, EraTypes eEra, vector<EraTypes> &aForeignErasSeen, vector<EraTypes> &aErasSeen, PlayerTypes ePlayer, vector<PlayerTypes> &aForeignPlayersSeen, vector<PlayerTypes> &aPlayersSeen, PlayerTypes eOwner)
{
	bool bValid = true;

	// Can we rule this out based on era?
	if (bValid && pBonusInfo->IsSameEra())
	{
		//Are local or foreign eras matched? Good.
		if ((eEra != aErasSeen[0]) && (eEra != aForeignErasSeen[0]))
		{
			bValid = false;
		}
	}
	if (bValid && pBonusInfo->IsUniqueEras())
	{
		//Are local or foreign eras unmatched? Good.
		if ((std::find(aErasSeen.begin(), aErasSeen.end(), eEra) != aErasSeen.end()) && (std::find(aForeignErasSeen.begin(), aForeignErasSeen.end(), eEra) != aForeignErasSeen.end()))
		{
			bValid = false;
		}
	}

	// Can we rule this out based on player?
	if (bValid && pBonusInfo->IsRequiresOwner())
	{
		if (ePlayer != eOwner)
		{
			bValid = false;
		}
	}
	if (bValid && pBonusInfo->IsRequiresAnyButOwner())
	{
		if (ePlayer == eOwner)
		{
			bValid = false;
		}
	}
	if (bValid && pBonusInfo->IsRequiresSamePlayer())
	{
		//Are local or foreign players matched? Good.
		if ((ePlayer != aPlayersSeen[0]) && (ePlayer != aForeignPlayersSeen[0]))
		{
			bValid = false;
		}

	}
	if (bValid && pBonusInfo->IsRequiresUniquePlayers())
	{
		//Are local or foreign players unmatched? Good.
		if ((std::find(aPlayersSeen.begin(), aPlayersSeen.end(), ePlayer) != aPlayersSeen.end()) && (std::find(aForeignPlayersSeen.begin(), aForeignPlayersSeen.end(), ePlayer) != aForeignPlayersSeen.end()))
		{
			bValid = false;
		}
	}

	return bValid;
}
#endif
bool CultureHelpers::IsValidForThemingBonus(CvThemingBonusInfo *pBonusInfo, EraTypes eEra, vector<EraTypes> &aErasSeen, PlayerTypes ePlayer, vector<PlayerTypes> &aPlayersSeen, PlayerTypes eOwner)
{
	bool bValid = true;

	// Can we rule this out based on era?
	if (bValid && pBonusInfo->IsSameEra())
	{
		if (eEra != aErasSeen[0])
		{
			bValid = false;
		}
	}
	if (bValid && pBonusInfo->IsUniqueEras())
	{
		if (std::find(aErasSeen.begin(), aErasSeen.end(), eEra) != aErasSeen.end())
		{
			bValid = false;
		}
	}

	// Can we rule this out based on player?
	if (bValid && pBonusInfo->IsRequiresOwner())
	{
		if (ePlayer != eOwner)
		{
			bValid = false;
		}
	}
	if (bValid && pBonusInfo->IsRequiresAnyButOwner())
	{
		if (ePlayer == eOwner)
		{
			bValid = false;
		}
	}
	if (bValid && pBonusInfo->IsRequiresSamePlayer())
	{
		if (ePlayer != aPlayersSeen[0])
		{
			bValid = false;
		}

	}
	if (bValid && pBonusInfo->IsRequiresUniquePlayers())
	{
		if (std::find(aPlayersSeen.begin(), aPlayersSeen.end(), ePlayer) != aPlayersSeen.end())
		{
			bValid = false;
		}
	}

	return bValid;
}

int CultureHelpers::FindWorkNotChosen(vector<CvGreatWorkInMyEmpire> &aWorks, vector<int> &aWorksChosen)
{
	for (unsigned int iI = 0; iI < aWorks.size(); iI++)
	{
		if (find (aWorksChosen.begin(), aWorksChosen.end(), aWorks[iI].m_iGreatWorkIndex) == aWorksChosen.end())
		{
			return aWorks[iI].m_iGreatWorkIndex;
		}
	}
	return NO_GREAT_WORK;
}

void CultureHelpers::SendArtSwapNotification(GreatWorkSlotType eType, bool bArt, PlayerTypes eOriginator, PlayerTypes eReceipient, int iWorkFromOriginator, int iWorkFromRecipient)
{
	CvGameCulture *pkGameCulture = GC.getGame().GetGameCulture();
	CvPlayer &kOriginator = GET_PLAYER(eOriginator);

	CvString strBuffer;
	CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_WORK_SWAP");
	if (eType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
	{
		if (bArt)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_WORK_ART_SWAP", kOriginator.getLeaderInfo().GetDescription(),
				pkGameCulture->GetGreatWorkName(iWorkFromRecipient), pkGameCulture->GetGreatWorkName(iWorkFromOriginator));
		}
		else
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ARTIFACT_SWAP", kOriginator.getLeaderInfo().GetDescription(),
				pkGameCulture->GetGreatWorkName(iWorkFromRecipient), pkGameCulture->GetGreatWorkName(iWorkFromOriginator));
		}
	}
	else
	{
		strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_WORK_WRITING_SWAP", kOriginator.getLeaderInfo().GetDescription(),
			pkGameCulture->GetGreatWorkName(iWorkFromRecipient), pkGameCulture->GetGreatWorkName(iWorkFromOriginator));
	}
	GET_PLAYER(eReceipient).GetNotifications()->Add(NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, strBuffer, strSummary, -1, -1, iWorkFromOriginator, kOriginator.GetID());
}

const CvString CultureHelpers::GetInfluenceText(InfluenceLevelTypes eLevel, int iTourism)
{
	CvString strInfluenceText;
	switch (eLevel)
	{
	case NO_INFLUENCE_LEVEL:
	case INFLUENCE_LEVEL_UNKNOWN:
		strInfluenceText = GetLocalizedText("TXT_KEY_CO_UNKNOWN");
		break;
	case INFLUENCE_LEVEL_EXOTIC:
		strInfluenceText = GetLocalizedText("TXT_KEY_CO_EXOTIC");
		break;
	case INFLUENCE_LEVEL_FAMILIAR:
		strInfluenceText = GetLocalizedText("TXT_KEY_CO_FAMILIAR");
		break;
	case INFLUENCE_LEVEL_POPULAR:
		strInfluenceText = GetLocalizedText("TXT_KEY_CO_POPULAR");
		break;
	case INFLUENCE_LEVEL_INFLUENTIAL:
		strInfluenceText = GetLocalizedText("TXT_KEY_CO_INFLUENTIAL");
		break;
	case INFLUENCE_LEVEL_DOMINANT:
		strInfluenceText = GetLocalizedText("TXT_KEY_CO_DOMINANT");
		break;
	}
	CvString strReturn;
	strReturn.Format("[COLOR_WHITE]+%d [ICON_TOURISM][ENDCOLOR]   %s", iTourism, strInfluenceText.c_str());
	return strReturn;
}

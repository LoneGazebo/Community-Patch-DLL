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
CvGreatWork::CvGreatWork(CvString szGreatPersonName, GreatWorkType eType, GreatWorkClass eClassType, int iTurn, EraTypes eEra, PlayerTypes ePlayer)
	: m_szGreatPersonName(szGreatPersonName)
	, m_eType(eType)
	, m_eClassType(eClassType)
	, m_iTurnFounded(iTurn)
	, m_eEra(eEra)
	, m_ePlayer(ePlayer)
{
	m_iTurnFounded = GC.getGame().getGameTurn();
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGreatWork& writeTo)
{
	int iTemp;

	uint uiVersion;
	loadFrom >> uiVersion;

	if(uiVersion == 1)
	{
		CvString oldGreatWorkName;
		loadFrom >> oldGreatWorkName;
	}

	loadFrom >> writeTo.m_szGreatPersonName;

	loadFrom >> iTemp;
	writeTo.m_eType = (GreatWorkType)iTemp;

	if (uiVersion >= 3)
	{
		loadFrom >> iTemp;
		writeTo.m_eClassType = (GreatWorkClass)iTemp;
	}
	else
	{
		writeTo.m_eClassType = CultureHelpers::GetGreatWorkClass(writeTo.m_eType);
	}

	loadFrom >> writeTo.m_iTurnFounded;
	loadFrom >> writeTo.m_eEra;
	loadFrom >> writeTo.m_ePlayer;

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGreatWork& readFrom)
{
	uint uiVersion = 3;
	saveTo << uiVersion;

	saveTo << readFrom.m_szGreatPersonName;

	saveTo << readFrom.m_eType;
	saveTo << readFrom.m_eClassType;
	
	saveTo << readFrom.m_iTurnFounded;
	saveTo << readFrom.m_eEra;
	saveTo << readFrom.m_ePlayer;

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
int CvGameCulture::CreateGreatWork(GreatWorkType eType, GreatWorkClass eClass, PlayerTypes ePlayer, EraTypes eEra, CvString szCreator)
{
	CvGreatWork newGreatWork;
	newGreatWork.m_eType = eType;
	newGreatWork.m_eClassType = eClass;
	newGreatWork.m_eEra = eEra;
	newGreatWork.m_ePlayer = ePlayer;
	newGreatWork.m_iTurnFounded = GC.getGame().getGameTurn();
	newGreatWork.m_szGreatPersonName = szCreator;

	int iNumGreatWorks = m_CurrentGreatWorks.size();

	m_CurrentGreatWorks.push_back(newGreatWork);

	return iNumGreatWorks;
}

GreatWorkType CvGameCulture::GetGreatWorkType(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork* pWork = &m_CurrentGreatWorks[iIndex];
	return pWork->m_eType;
}

GreatWorkClass CvGameCulture::GetGreatWorkClass(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork* pWork = &m_CurrentGreatWorks[iIndex];
	return pWork->m_eClassType;
}

/// Returns UI tooltip for this Great Work
CvString CvGameCulture::GetGreatWorkTooltip(int iIndex, PlayerTypes eOwner) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szTooltip = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];

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
	int iCulturePerWork = GC.getBASE_CULTURE_PER_GREAT_WORK();
	iCulturePerWork += GET_PLAYER(eOwner).GetGreatWorkYieldChange(YIELD_CULTURE);
	int iTourismPerWork = GC.getBASE_TOURISM_PER_GREAT_WORK();

	cultureString.Format ("+%d [ICON_CULTURE], +%d [ICON_TOURISM]", iCulturePerWork, iTourismPerWork);
	szTooltip += cultureString;

	return szTooltip;
}

/// Returns name of this Great Work
CvString CvGameCulture::GetGreatWorkName(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork* pWork = &m_CurrentGreatWorks[iIndex];

	return CultureHelpers::GetGreatWorkName(pWork->m_eType);
}

/// Returns artist of this Great Work
CvString CvGameCulture::GetGreatWorkArtist(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szArtist = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	szArtist = pWork->m_szGreatPersonName;

	return szArtist;
}

/// Returns era of this Great Work
CvString CvGameCulture::GetGreatWorkEra(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
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
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szEra = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	szEra = GC.getEraInfo(pWork->m_eEra)->getAbbreviation();

	return szEra;	
}

CvString CvGameCulture::GetGreatWorkEraShort(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	CvString szEra = "";

	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	szEra = GC.getEraInfo(pWork->m_eEra)->getShortDesc();

	return szEra;	
}

PlayerTypes CvGameCulture::GetGreatWorkCreator (int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	const CvGreatWork *pWork = &m_CurrentGreatWorks[iIndex];
	return pWork->m_ePlayer;
}

PlayerTypes CvGameCulture::GetGreatWorkController(int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");
	
	// for each player
	//   for each building
	//     for each slot
	//       check to see if it holds this work

	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)uiPlayer;

		int iCityLoop;
		CvCity* pCity = NULL;
		for (pCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(ePlayer).getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							int iNumSlots = pkBuilding->GetGreatWorkCount();
							for (int iI = 0; iI < iNumSlots; iI++)
							{
								int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
								if (iGreatWorkIndex == iIndex)
								{
									return ePlayer;
								}
							}
						}
					}
				}
			}
		}
	}

	return NO_PLAYER;
}

int CvGameCulture::GetGreatWorkCurrentThemingBonus (int iIndex) const
{
	CvAssertMsg (iIndex < GetNumGreatWorks(), "Bad Great Work index");

	// for each player
	//   for each building
	//     for each slot
	//       check to see if it holds this work

	for (uint uiPlayer = 0; uiPlayer < MAX_MAJOR_CIVS; uiPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)uiPlayer;

		int iCityLoop;
		CvCity* pCity = NULL;
		for (pCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
		{
			for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				BuildingClassTypes eBuildingClass = (BuildingClassTypes)iBuildingClassLoop;
				CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(ePlayer).getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							int iNumSlots = pkBuilding->GetGreatWorkCount();
							for (int iI = 0; iI < iNumSlots; iI++)
							{
								int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
								if (iGreatWorkIndex == iIndex)
								{
									return pCity->GetCityCulture()->GetThemingBonus(eBuildingClass);
								}
							}
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

	int iCityLoop;
	CvCity* pCity = NULL;
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

		for (pCity = GET_PLAYER(eTempPlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(eTempPlayer).nextCity(&iCityLoop))
		{
			for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(eTempPlayer).getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							int iNumSlots = pkBuilding->GetGreatWorkCount();
							for (int iI = 0; iI < iNumSlots; iI++)
							{
								int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
								if (ui == 0)
								{
									if (iGreatWorkIndex == iWork1)
									{
										pCity1 = pCity;
										eBuildingClass1 = (BuildingClassTypes)iBuildingClassLoop;
										iSwapIndex1 = iI;
										break;
									}
								}
								else if (ui == 1)
								{
									if (iGreatWorkIndex == iWork2)
									{
										pCity2 = pCity;
										eBuildingClass2 = (BuildingClassTypes)iBuildingClassLoop;
										iSwapIndex2 = iI;
										break;
									}
								}
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

	return true;
}

void CvGameCulture::MoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, 
																												int iCity2, int iBuildingClass2, int iWorkIndex2)
{
	if(ePlayer == NO_PLAYER){
		return;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (!kPlayer.isAlive()){
		return;
	}

	CvCity* pCity1 = kPlayer.getCity(iCity1);
	CvCity* pCity2 = kPlayer.getCity(iCity2);
	if(!pCity1 || !pCity2){
		return;
	}

	int workType1 = pCity1->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClass1, iWorkIndex1);
	int workType2 = pCity2->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClass2, iWorkIndex2);
	pCity1->GetCityBuildings()->SetBuildingGreatWork((BuildingClassTypes)iBuildingClass1, iWorkIndex1, workType2);
	pCity2->GetCityBuildings()->SetBuildingGreatWork((BuildingClassTypes)iBuildingClass2, iWorkIndex2, workType1);
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

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameCulture& writeTo)
{
	uint uiVersion;

	loadFrom >> uiVersion;

	int iEntriesToRead;
	CvGreatWork tempItem;

	writeTo.m_CurrentGreatWorks.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_CurrentGreatWorks.push_back(tempItem);
	}

	if (uiVersion >= 2)
	{
		bool bTempBool;
		loadFrom >> bTempBool;
		writeTo.SetReportedSomeoneInfluential(bTempBool);
	}
	else
	{
		writeTo.SetReportedSomeoneInfluential(false);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameCulture& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;

	GreatWorkList::const_iterator it;
	saveTo << readFrom.m_CurrentGreatWorks.size();
	for(it = readFrom.m_CurrentGreatWorks.begin(); it != readFrom.m_CurrentGreatWorks.end(); it++)
	{
		saveTo << *it;
	}

	saveTo << readFrom.GetReportedSomeoneInfluential();

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
	m_bThemed = false;
	m_bEndangered = false;
}

/// Constructor
CvGreatWorkBuildingInMyEmpire::	CvGreatWorkBuildingInMyEmpire(int iCityID, BuildingTypes eBuilding)
: m_iCityID(iCityID)
, m_eBuilding(eBuilding)
{
	m_bThemed = false;
	m_bEndangered = false;
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
	m_iOpinionUnhappiness = 0;

	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_aiCulturalInfluence[iI] = 0;
		m_aiLastTurnCulturalInfluence[iI] = 0;
	}

	m_bReportedTwoCivsAway = false;
	m_bReportedOneCivAway = false;

	m_iSwappableWritingIndex  = -1;
	m_iSwappableArtIndex      = -1;
	m_iSwappableArtifactIndex = -1;
	m_iSwappableMusicIndex    = -1;

	m_iTurnIdeologySwitch = -1;
}

// GREAT WORKS

/// Is there a Great Work slot of the appropriate type somewhere?
bool CvPlayerCulture::HasAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot)
{
	int iLoop;
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
	int iLoop;
	int iCount = 0;

	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		iCount += pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eGreatWorkSlot);
	}
	return iCount;
}

/// Return the city (and building/slot) of the city that can provide the closest Great Work slot)
CvCity *CvPlayerCulture::GetClosestAvailableGreatWorkSlot(int iX, int iY, GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, int *iSlot) const
{
	int iLoop;
	int iBestDistance = MAX_INT;
	CvCity *pBestCity = NULL;
	BuildingClassTypes eBuildingClassReturned = NO_BUILDINGCLASS; // Passed by reference below
	int iSlotReturned = -1; // Passed by reference below

	for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		int iDistance = plotDistance (iX, iY, pCity->getX(), pCity->getY());

		if (iDistance < iBestDistance)
		{
			if (pCity->GetCityBuildings()->GetNextAvailableGreatWorkSlot(eGreatWorkSlot, &eBuildingClassReturned, &iSlotReturned))
			{
				iBestDistance = iDistance;
				pBestCity = pCity;
				*eBuildingClass = eBuildingClassReturned;
				*iSlot = iSlotReturned;
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
bool CvPlayerCulture::ControlsGreatWork (int iIndex)
{
	CvCity* pLoopCity = NULL;
	int iLoop = 0;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			CvCivilizationInfo& playerCivilizationInfo = m_pPlayer->getCivilizationInfo();
			BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
			if (eBuilding != NO_BUILDING)
			{
				CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
				if (pkBuilding)
				{
					if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						int iNumSlots = pkBuilding->GetGreatWorkCount();
						for (int iI = 0; iI < iNumSlots; iI++)
						{
							int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
							if (iGreatWorkIndex == iIndex)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;	
}

bool CvPlayerCulture::GetGreatWorkLocation(int iSearchIndex, int &iReturnCityID, BuildingTypes &eReturnBuilding, int &iReturnSlot)
{
	CvCity* pLoopCity = NULL;
	int iLoop = 0;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			CvCivilizationInfo& playerCivilizationInfo = m_pPlayer->getCivilizationInfo();
			BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
			if (eBuilding != NO_BUILDING)
			{
				CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
				if (pkBuilding)
				{
					if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						int iNumSlots = pkBuilding->GetGreatWorkCount();
						for (int iI = 0; iI < iNumSlots; iI++)
						{
							int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
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
		}
	}

	return false;	
}

/// AI routine to decide what Great Work swapping should take place (including placing Great Works up for swap from another player)
void CvPlayerCulture::DoSwapGreatWorks()
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

	CvCity* pLoopCity = NULL;
	int iLoop = 0;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
		{
			CvCivilizationInfo& playerCivilizationInfo = m_pPlayer->getCivilizationInfo();
			BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
			if (eBuilding != NO_BUILDING)
			{
				CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
				if (pkBuilding)
				{
					if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						CvGreatWorkBuildingInMyEmpire building;
						building.m_eBuilding = eBuilding;
						building.m_iCityID = pLoopCity->GetID();
						building.m_bEndangered = (pLoopCity->getDamage() > 0);

						GreatWorkSlotType eSlotType = pkBuilding->GetGreatWorkSlotType();
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

						int iNumSlots = pkBuilding->GetGreatWorkCount();
						for (int iI = 0; iI < iNumSlots; iI++)
						{
							int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
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
		}
	}

	MoveWorks (CvTypes::getGREAT_WORK_SLOT_LITERATURE(), aGreatWorkBuildingsWriting, aGreatWorksWriting, aNull);
	MoveWorks (CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT(), aGreatWorkBuildingsArt, aGreatWorksArt, aGreatWorksArtifacts);
	MoveWorks (CvTypes::getGREAT_WORK_SLOT_MUSIC(), aGreatWorkBuildingsMusic, aGreatWorksMusic, aNull);
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
			return (pBonus1->GetAIPriority() > pBonus2->GetAIPriority());
		}
		else if (pBonus1)
		{
			return true;
		}
	}

	return false;
}

/// Overall routine that orchestrates all the maneuvering of Great Works between buildings and players for one AI turn
void CvPlayerCulture::MoveWorks (GreatWorkSlotType eType, vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2)
{
	std::sort (buildings.begin(), buildings.end(), SortThemingBonus);

	// First building that are not endangered
	vector<CvGreatWorkBuildingInMyEmpire>::iterator itBuilding;
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); itBuilding++)
	{
		if (!itBuilding->m_bEndangered)
		{
			itBuilding->m_bThemed = false;
			if (ThemeBuilding(itBuilding, works1, works2, false /*bConsiderOtherPlayers*/))
			{
				itBuilding->m_bThemed = true;
			}
		}
	}

	// Then endangered ones
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); itBuilding++)
	{
		if (itBuilding->m_bEndangered)
		{
			itBuilding->m_bThemed = false;
			if (ThemeBuilding(itBuilding, works1, works2, false /*bConsiderOtherPlayers*/))
			{
				itBuilding->m_bThemed = true;
			}
		}
	}

	// One more pass through those that are not endangered to see if swapping with another player would help (as long as this isn't Music)
	if (eType != CvTypes::getGREAT_WORK_SLOT_MUSIC())
	{
		for (itBuilding = buildings.begin(); itBuilding != buildings.end(); itBuilding++)
		{
			if (!itBuilding->m_bEndangered && !itBuilding->m_bThemed)
			{
				if (ThemeBuilding(itBuilding, works1, works2, true /*bConsiderOtherPlayers*/))
				{
					itBuilding->m_bThemed = true;
				}
			}
		}
	}

	// Set the first work left that we haven't themed as something we'd be willing to trade
	//    for Writing
	if (eType == CvTypes::getGREAT_WORK_SLOT_LITERATURE())
	{
		if (works1.size() > 0)
		{
			SetSwappableWritingIndex(works1[0].m_iGreatWorkIndex);
		}
		else
		{
			SetSwappableWritingIndex(-1);
		}
	}

	//    for Art and Artifacts
	else
	{
		if (eType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
		{
			if (works1.size() > 0)
			{
				SetSwappableArtIndex(works1[0].m_iGreatWorkIndex);
			}
			else
			{
				SetSwappableArtIndex(-1);
			}
		}
		if (eType == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
		{
			if (works2.size() > 0)
			{
				SetSwappableArtifactIndex(works2[0].m_iGreatWorkIndex);
			}
			else
			{
				SetSwappableArtifactIndex(-1);
			}
		}
	}

	// Fill unthemed buildings, first those that aren't endangered
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); itBuilding++)
	{
		if (!itBuilding->m_bEndangered && !itBuilding->m_bThemed)
		{
			FillBuilding(itBuilding, works1, works2);
		}
	}
	for (itBuilding = buildings.begin(); itBuilding != buildings.end(); itBuilding++)
	{
		if (itBuilding->m_bEndangered && !itBuilding->m_bThemed)
		{
			FillBuilding(itBuilding, works1, works2);
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

	// Try each of the theming bonuses for this building
	for (int iI = 0; iI < pkEntry->GetNumThemingBonuses(); iI++)
	{
		CvThemingBonusInfo *pkBonusInfo = pkEntry->GetThemingBonusInfo(iI);
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
			if (ThemeEqualArtArtifact(*buildingIt, iI, pkEntry->GetGreatWorkCount(), works1, works2, bConsiderOtherPlayers))
			{
				return true;
			}
		}
		else
		{
			worksToConsider = works1;
			for (it = works2.begin(); it != works2.end(); it++)
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
		for (it = worksToConsider.begin(); it != worksToConsider.end(); it++)
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
			for (it2++; it2 != worksToConsider.end() && aWorksChosen.size() < (unsigned int)iCountSlots; it2++)
			{
				if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, it2->m_eEra, aErasSeen, it2->m_ePlayer, aPlayersSeen, m_pPlayer->GetID()))
				{
					aWorksChosen.push_back(it2->m_iGreatWorkIndex);
					aPlayersSeen.push_back(it2->m_ePlayer);
					aErasSeen.push_back(it2->m_eEra);
				}
			}

			// Did we theme it properly?
			bool bThemedProperly = false;
			if (aWorksChosen.size() == iCountSlots && CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == iI)
			{
				bThemedProperly = true;
			}

			// If we are one work short, let's look to other players
			else if (bConsiderOtherPlayers && aWorksChosen.size() == (iCountSlots - 1) && (pkEntry->GetGreatWorkSlotType() != CvTypes::getGREAT_WORK_SLOT_MUSIC()))
			{
				for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
					if (kPlayer.isAlive() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid((PlayerTypes)iLoopPlayer))
					{
						int iToBeAcquiredWorkIndex = NO_GREAT_WORK;
						int iToBeDiscardedWorkIndex = NO_GREAT_WORK;

						if (pkEntry->GetGreatWorkSlotType() == CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT())
						{
							if (pkBonusInfo->IsMustBeArt())
							{
								iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtIndex();
								iToBeDiscardedWorkIndex = CultureHelpers::FindWorkNotChosen(works1, aWorksChosen);
							}
							else if (pkBonusInfo->IsMustBeArtifact())
							{
								iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtifactIndex();
								iToBeDiscardedWorkIndex = CultureHelpers::FindWorkNotChosen(works2, aWorksChosen);
							}
						}
						else
						{
							iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableWritingIndex();
							iToBeDiscardedWorkIndex = CultureHelpers::FindWorkNotChosen(works1, aWorksChosen);
						}

						// Does this work fit?
						if (iToBeAcquiredWorkIndex != NO_GREAT_WORK && iToBeDiscardedWorkIndex != NO_GREAT_WORK)
						{
							EraTypes eEra = pkGameCulture->m_CurrentGreatWorks[iToBeAcquiredWorkIndex].m_eEra;
							PlayerTypes ePlayer = pkGameCulture->GetGreatWorkCreator(iToBeAcquiredWorkIndex);
							if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, eEra, aErasSeen, ePlayer, aPlayersSeen, m_pPlayer->GetID()))
							{
								aWorksChosen.push_back(iToBeAcquiredWorkIndex);
								if (CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == iI)
								{
									bool bSwapSuccess = pkGameCulture->SwapGreatWorks(m_pPlayer->GetID(), iToBeDiscardedWorkIndex, ePlayer, iToBeAcquiredWorkIndex);
									if (bSwapSuccess)
									{
										bThemedProperly = true;

										LogSwapWorks(ePlayer, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);

										// Update works list
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

										CultureHelpers::SendArtSwapNotification(pkEntry->GetGreatWorkSlotType(), pkBonusInfo->IsMustBeArt(), m_pPlayer->GetID(), ePlayer, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);
										break;  // Themed it through acquisition
									}
								}
							}
						}
					}
				}
			}

			if (bThemedProperly)
			{
				for (int jJ = 0; jJ < (int)aWorksChosen.size(); jJ++)
				{
					for (int kK = 0; kK < (int)worksToConsider.size(); kK++)
					{
						if (worksToConsider[kK].m_iGreatWorkIndex == aWorksChosen[jJ])
						{
							MoveWorkIntoSlot(worksToConsider[kK], buildingIt->m_iCityID, buildingIt->m_eBuilding, jJ);
							break;
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
				LogThemedBuilding(buildingIt->m_iCityID, buildingIt->m_eBuilding, pkBonusInfo->GetBonus());
				return true;
			}
		}
	}
	return false;
}

/// Specialized version of ThemeBuilding() that handles those buildings that are split between Art and Artifact
bool CvPlayerCulture::ThemeEqualArtArtifact(CvGreatWorkBuildingInMyEmpire kBldg, int iThemingBonusIndex, int iNumSlots, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool /*bConsiderOtherPlayers*/)
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

	// Try each of the Artifacts as the starter
	for (it = works2.begin(); it != works2.end(); it++)
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
		for (it2++; it2 != works2.end() && aArtifactsChosen.size() < (unsigned int)iWorksInHalf; it2++)
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
			vector<CvGreatWorkInMyEmpire>::const_iterator it3;
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

				// If we are one work short, let's look to other players for a last piece of art
				else if (aWorksChosen.size() == (iNumSlots - 1))
				{
					for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
						if (kPlayer.isAlive() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid((PlayerTypes)iLoopPlayer))
						{
							int iToBeAcquiredWorkIndex = kPlayer.GetCulture()->GetSwappableArtIndex();
							int iToBeDiscardedWorkIndex = CultureHelpers::FindWorkNotChosen(works1, aWorksChosen);

							// Does this work fit?
							if (iToBeAcquiredWorkIndex != NO_GREAT_WORK && iToBeDiscardedWorkIndex != NO_GREAT_WORK)
							{
								EraTypes eEra = pkGameCulture->m_CurrentGreatWorks[iToBeAcquiredWorkIndex].m_eEra;
								PlayerTypes ePlayer = pkGameCulture->GetGreatWorkCreator(iToBeAcquiredWorkIndex);
								if (CultureHelpers::IsValidForThemingBonus(pkBonusInfo, eEra, aErasSeen, ePlayer, aPlayersSeen, m_pPlayer->GetID()))
								{
									aWorksChosen.push_back(iToBeAcquiredWorkIndex);
									if (CultureHelpers::GetThemingBonusIndex(m_pPlayer->GetID(), pkEntry, aWorksChosen) == iThemingBonusIndex)
									{
										bool bSwapSuccessful = pkGameCulture->SwapGreatWorks(m_pPlayer->GetID(), iToBeDiscardedWorkIndex, ePlayer, iToBeAcquiredWorkIndex);
										if (bSwapSuccessful)
										{
											bThemedProperly = true;

											// Update works list
											tempWorks.clear();
											for (it5 = works1.begin(); it5 != works1.end(); it5++)
											{
												if (it5->m_iGreatWorkIndex == iToBeDiscardedWorkIndex)
												{
													it5->m_iGreatWorkIndex = iToBeAcquiredWorkIndex;
													GetGreatWorkLocation(it5->m_iGreatWorkIndex, it5->m_iCityID, it5->m_eBuilding, it5->m_iSlot);
												}
												tempWorks.push_back(*it5);
											}
											works1 = tempWorks;

											CultureHelpers::SendArtSwapNotification(pkEntry->GetGreatWorkSlotType(), pkBonusInfo->IsMustBeArt(), m_pPlayer->GetID(), ePlayer, iToBeDiscardedWorkIndex, iToBeAcquiredWorkIndex);
											break;  // Themed it through acquisition
										}
									}
								}
							}
						}
					}
				}

				if (bThemedProperly)
				{
					for (int jJ = 0; jJ < (int)aWorksChosen.size(); jJ++)
					{
						for (int kK = 0; kK < (int)works1.size(); kK++)
						{
							if (works1[kK].m_iGreatWorkIndex == aWorksChosen[jJ])
							{
								MoveWorkIntoSlot(works1[kK], kBldg.m_iCityID, kBldg.m_eBuilding, jJ);
								break;
							}
						}
						for (int kK = 0; kK < (int)works2.size(); kK++)
						{
							if (works2[kK].m_iGreatWorkIndex == aWorksChosen[jJ])
							{
								MoveWorkIntoSlot(works2[kK], kBldg.m_iCityID, kBldg.m_eBuilding, jJ);
								break;
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
					LogThemedBuilding(kBldg.m_iCityID, kBldg.m_eBuilding, pkBonusInfo->GetBonus());
					return true;
				}
			}
		}
	}
	return false;
}

/// Simple version of ThemeBuilding() which just fills in a building with ANY Great Works, not ones that provide a theming bonus
bool CvPlayerCulture::FillBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator buildingIt, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2)
{
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

	vector<CvGreatWorkInMyEmpire> worksToConsider;
	vector<int> aWorksChosen;

	worksToConsider = works1;
	vector<CvGreatWorkInMyEmpire>::const_iterator it;
	for (it = works2.begin(); it != works2.end(); it++)
	{
		worksToConsider.push_back(*it);
	}

	it = worksToConsider.begin();
	for (int iI = 0; iI < iCountSlots && it != worksToConsider.end(); iI++, it++)
	{
		aWorksChosen.push_back(worksToConsider[iI].m_iGreatWorkIndex);
		MoveWorkIntoSlot(worksToConsider[iI], buildingIt->m_iCityID, buildingIt->m_eBuilding, iI);
	}

	// Remove these works from those to consider later
	if (aWorksChosen.size() > 0)
	{
		vector<CvGreatWorkInMyEmpire>::iterator it2;
		vector<CvGreatWorkInMyEmpire> tempWorks;

		tempWorks.clear();
		for (it2 = works1.begin(); it2 != works1.end(); it2++)
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
		for (it2 = works2.begin(); it2 != works2.end(); it2++)
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
	return true;
}

/// Lower-level routine to perform the swap between two Great Works within your own empire
void CvPlayerCulture::MoveWorkIntoSlot (CvGreatWorkInMyEmpire kWork, int iCityID, BuildingTypes eBuilding, int iSlot)
{
	CvBuildingEntry *pkToEntry = GC.getBuildingInfo(eBuilding);

	int iFromCityID;
	BuildingTypes eFromBuildingType;
	int iFromSlot;
	GetGreatWorkLocation(kWork.m_iGreatWorkIndex, iFromCityID, eFromBuildingType, iFromSlot);

	CvBuildingEntry *pkFromEntry = GC.getBuildingInfo(eFromBuildingType);
	if (pkToEntry && pkFromEntry)
	{
		CvCity *pToCity = m_pPlayer->getCity(iCityID);
		BuildingClassTypes eToBuildingClass = (BuildingClassTypes)pkToEntry->GetBuildingClassType();

		int iFromWork = pToCity->GetCityBuildings()->GetBuildingGreatWork(eToBuildingClass, iSlot);
		CvCity *pFromCity = m_pPlayer->getCity(iFromCityID);
		BuildingClassTypes eFromBuildingClass = (BuildingClassTypes)pkFromEntry->GetBuildingClassType();

		pToCity->GetCityBuildings()->SetBuildingGreatWork(eToBuildingClass, iSlot, kWork.m_iGreatWorkIndex);
		pFromCity->GetCityBuildings()->SetBuildingGreatWork(eFromBuildingClass, iFromSlot, iFromWork);
	}
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
void CvPlayerCulture::AddDigCompletePlot(CvPlot *pPlot)
{
	m_aDigCompletePlots.push_back(pPlot);
}

/// Remove a plot from the list of plots where we have archaeologists waiting for orders
void CvPlayerCulture::RemoveDigCompletePlot(CvPlot *pPlot)
{
	vector<CvPlot *>::const_iterator it;

	for (it = m_aDigCompletePlots.begin(); it != m_aDigCompletePlots.end(); it++)
	{
		if (*it == pPlot)
		{
			m_aDigCompletePlots.erase(it);
			break;
		}
	}
}

/// Reset the list of plots where we have an archaeologist waiting for orders
void CvPlayerCulture::ResetDigCompletePlots()
{
	m_aDigCompletePlots.clear();
}

/// Find the next plot where we have an archaeologist waiting for orders
CvPlot *CvPlayerCulture::GetNextDigCompletePlot() const
{
	CvPlot *pRtnValue = NULL;

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
		int iLoop;
		CvUnit *pLoopUnit;
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
bool CvPlayerCulture::HasDigCompleteHere(CvPlot *pPlot) const
{
	vector<CvPlot *>::const_iterator it;

	for (it = m_aDigCompletePlots.begin(); it != m_aDigCompletePlots.end(); it++)
	{
		if (*it == pPlot)
		{
			return true;
		}
	}

	return false;
}

/// How much culture can we receive from cashing in a written artifact?
int CvPlayerCulture::GetWrittenArtifactCulture() const
{
	// Culture boost based on 8 previous turns; same as GREAT_WRITER; move to XML?
	int iValue = m_pPlayer->GetCultureYieldFromPreviousTurns(GC.getGame().getGameTurn(), 8 /*iPreviousTurnsToCount */);

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
		if (pPlot->getOwner() != NO_PLAYER && GET_PLAYER(pPlot->getOwner()).isMinorCiv())
		{
			if (m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
			{
				if (m_pPlayer->GetDiplomacyAI()->GetMinorCivApproach(pPlot->getOwner()) != MINOR_CIV_APPROACH_CONQUEST)
				{
					eRtnValue = ARCHAEOLOGY_LANDMARK;
				}
			}
		}
	}

	// Not a tile a city can work?  Go for artifact
	else if (pPlot->getWorkingCity() == NULL)
	{
		eRtnValue = ARCHAEOLOGY_ARTIFACT_PLAYER1;
	}

	// Otherwise go for Landmark if from Ancient Era, or if have enough other Archaeologists and Antiquity sites to fill all slots
	else if (pPlot->GetArchaeologicalRecord().m_eEra == 0)
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

	// If chose an artifact, would player 2's be better?
	if (eRtnValue == ARCHAEOLOGY_ARTIFACT_PLAYER1)
	{
		if (pPlot->GetArchaeologicalRecord().m_ePlayer2 == m_pPlayer->GetID())
		{
			// For now have AI player try to collect their own artifacts
			eRtnValue = ARCHAEOLOGY_ARTIFACT_PLAYER2;
		}
	}

	return eRtnValue;
}

/// Make things happen at an archaeology dig
void CvPlayerCulture::DoArchaeologyChoice (ArchaeologyChoiceType eChoice)
{
	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	BuildingClassTypes eBuildingToHouse;
	int iSlot;
	CvCity *pHousingCity;
	CvPlot *pPlot;
	CvUnit *pUnit = GetNextDigCompleteArchaeologist(&pPlot);
	if (pUnit)
	{
		GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
		GreatWorkSlotType eWritingSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();
		GreatWorkType eGreatArtifact = CultureHelpers::GetArtifact(pPlot);
		GreatWorkClass eClass = (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_ARTIFACT");

		switch (eChoice)
		{
		case ARCHAEOLOGY_DO_NOTHING:
			break;
		case ARCHAEOLOGY_LANDMARK:
			{
				ImprovementTypes eLandmarkImprovement = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_LANDMARK");
				if (eLandmarkImprovement != NO_IMPROVEMENT)
				{
					pPlot->setImprovementType(eLandmarkImprovement, m_pPlayer->GetID());
					pUnit->kill(true);

					if (pPlot->getOwner() != NO_PLAYER)
					{
						CvPlayer &kOwner = GET_PLAYER(pPlot->getOwner());

						// City-state owned territory?
						if (kOwner.isMinorCiv())
						{
							int iFriendship = GC.getLANDMARK_MINOR_FRIENDSHIP_CHANGE();
							kOwner.GetMinorCivAI()->ChangeFriendshipWithMajor(m_pPlayer->GetID(), iFriendship);
						}

						// AI major civ owned territory?
						else if (!kOwner.isHuman())
						{
							kOwner.GetDiplomacyAI()->ChangeNumLandmarksBuiltForMe(m_pPlayer->GetID(), 1);
						}
					}
				}
			}
			break;
		case ARCHAEOLOGY_ARTIFACT_PLAYER1:
			{
				if (pPlot->getOwner() != pUnit->getOwner() && pPlot->getOwner() != NO_PLAYER)
				{
					GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 10);
				}
				pHousingCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pUnit->getX(),pUnit->getY(), eArtArtifactSlot, &eBuildingToHouse, &iSlot);
				int iGWindex = 	pCulture->CreateGreatWork(eGreatArtifact, eClass, pPlot->GetArchaeologicalRecord().m_ePlayer1, pPlot->GetArchaeologicalRecord().m_eEra, "");
				pHousingCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingToHouse, iSlot, iGWindex);
				pPlot->setImprovementType(NO_IMPROVEMENT);
				pUnit->kill(true);
			}
			break;
		case ARCHAEOLOGY_ARTIFACT_PLAYER2:
			{
				if (pPlot->getOwner() != pUnit->getOwner() && pPlot->getOwner() != NO_PLAYER)
				{
					GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 10);
				}
				pHousingCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pUnit->getX(),pUnit->getY(), eArtArtifactSlot, &eBuildingToHouse, &iSlot);
				int iGWindex = 	pCulture->CreateGreatWork(eGreatArtifact, eClass, pPlot->GetArchaeologicalRecord().m_ePlayer2, pPlot->GetArchaeologicalRecord().m_eEra, "");
				pHousingCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingToHouse, iSlot, iGWindex);
				pPlot->setImprovementType(NO_IMPROVEMENT);
				pUnit->kill(true);
			}
			break;

		case ARCHAEOLOGY_ARTIFACT_WRITING:
			{
				if (pPlot->getOwner() != pUnit->getOwner() && pPlot->getOwner() != NO_PLAYER)
				{
					GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 10);
				}
				pHousingCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pUnit->getX(),pUnit->getY(), eWritingSlot, &eBuildingToHouse, &iSlot);
				int iGWindex = 	pCulture->CreateGreatWork(eGreatArtifact, (GreatWorkClass)GC.getInfoTypeForString("GREAT_WORK_LITERATURE"), pPlot->GetArchaeologicalRecord().m_ePlayer1, pPlot->GetArchaeologicalRecord().m_eEra, "");
				pHousingCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingToHouse, iSlot, iGWindex);
				pPlot->setImprovementType(NO_IMPROVEMENT);
				pUnit->kill(true);
			}
			break;

		case ARCHAEOLOGY_CULTURE_BOOST:
			{
				if (pPlot->getOwner() != pUnit->getOwner() && pPlot->getOwner() != NO_PLAYER)
				{
					GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeNegativeArchaeologyPoints(pUnit->getOwner(), 10);
				}

				// Culture boost based on 8 previous turns; same as GREAT_WRITER; move to XML?
				int iValue = m_pPlayer->GetCultureYieldFromPreviousTurns(GC.getGame().getGameTurn(), 8 /*iPreviousTurnsToCount */);

				// Modify based on game speed
				iValue *= GC.getGame().getGameSpeedInfo().getCulturePercent();
				iValue /= 100;

				m_pPlayer->changeJONSCulture(iValue);

				pPlot->setImprovementType(NO_IMPROVEMENT);
				pUnit->kill(true);
			}
			break;
		}

		m_pPlayer->SetNumArchaeologyChoices(m_pPlayer->GetNumArchaeologyChoices() - 1);
		m_pPlayer->GetCulture()->RemoveDigCompletePlot(pPlot);
	}
}

// CULTURAL INFLUENCE

/// Update cultural influence numbers for this turn
void CvPlayerCulture::DoTurn()
{
	int iInfluentialCivsForWin = GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin();
	int iLastTurnInfluentialCivs = GetNumCivsInfluentialOn();

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
	}
	
	DoPublicOpinion();

	CvString strSummary;
	CvString strInfo;
	CvNotifications* pTargetNotifications = m_pPlayer->GetNotifications();
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

	if (iThisTurnInfluentialCivs < iLastTurnInfluentialCivs && bCultureVictoryValid)
	{
		Localization::String strTemp;
		strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_NO_LONGER_INFLUENTIAL");
		strInfo = GetLocalizedText("TXT_KEY_NOTIFICATION_CULTURE_VICTORY_NO_LONGER_INFLUENTIAL_TT");
		pTargetNotifications->Add(NOTIFICATION_CULTURE_VICTORY_NO_LONGER_INFLUENTIAL, strInfo, strSummary, -1, -1, m_pPlayer->GetID());
	}

	if (!m_bReportedTwoCivsAway && iThisTurnInfluentialCivs > 0 && iThisTurnInfluentialCivs == iInfluentialCivsForWin - 2 && GC.getGame().countMajorCivsEverAlive() >= 4)
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

	if (!m_bReportedOneCivAway && iThisTurnInfluentialCivs == iInfluentialCivsForWin - 1 && GC.getGame().countMajorCivsEverAlive() >= 3)
	{
		if(bCultureVictoryValid)
		{//This civilization is the first civ to be one civilizations away from getting a cultural victory.  Notify the masses!
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

		m_bReportedOneCivAway = true;
	}

	if (m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		// check for having city-state artifacts
		std::vector<int> aiCityStateArtifact;
		int iLoop;
		CvCity* pLoopCity = NULL;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				CvCivilizationInfo& playerCivilizationInfo = m_pPlayer->getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							int iNumSlots = pkBuilding->GetGreatWorkCount();
							for (int iI = 0; iI < iNumSlots; iI++)
							{
								int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
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
			}
		}

		// check for having broadcast towers filled
		PolicyBranchTypes eCurrentBranchType = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();

		if (eCurrentBranchType == (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM())
		{
			int iNumWorksInBroadcastTowers = 0;
			for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
			{
				for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
				{
					CvCivilizationInfo& playerCivilizationInfo = m_pPlayer->getCivilizationInfo();
					BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
					if (eBuilding != NO_BUILDING)
					{
						CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
						if (pkBuilding && strcmp(pkBuilding->GetType(), "BUILDING_BROADCAST_TOWER") == 0)
						{
							if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
							{
								int iNumSlots = pkBuilding->GetGreatWorkCount();
								bool bAnySlotEmpty = false;
								for (int iI = 0; iI < iNumSlots; iI++)
								{
									int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
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

/// What is our cultural influence now?
int CvPlayerCulture::GetInfluenceOn(PlayerTypes ePlayer) const
{
	CvAssertMsg (ePlayer >= 0, "Invalid player index");
	CvAssertMsg (ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS) return 0;
	return m_aiCulturalInfluence[iIndex];
}

// What is our cultural influence now?
void CvPlayerCulture::ChangeInfluenceOn(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg (ePlayer >= 0, "Invalid player index");
	CvAssertMsg (ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS) return;
	m_aiCulturalInfluence[iIndex] = m_aiCulturalInfluence[iIndex] + iValue;
}

/// What was our cultural influence last turn?
int CvPlayerCulture::GetLastTurnInfluenceOn(PlayerTypes ePlayer) const
{
	CvAssertMsg (ePlayer >= 0, "Invalid player index");
	CvAssertMsg (ePlayer < MAX_MAJOR_CIVS, "Invalid player index");

	int iIndex = (int)ePlayer;
	if (iIndex < 0 || iIndex >= MAX_MAJOR_CIVS) return 0;
	return m_aiLastTurnCulturalInfluence[iIndex];
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
		bool bTargetHasGreatFirewall = false;

		int iLoopCity;
		CvCity *pLoopCity;

		// only check for firewall if the internet influence spread modifier is > 0
		int iTechSpreadModifier = m_pPlayer->GetInfluenceSpreadModifier();
		if (iTechSpreadModifier > 0) 
		{
			for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoopCity))
			{
				// Buildings
				for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
				{
					BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

					CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
					if(!pkBuildingClassInfo)
					{
						continue;
					}

					CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(pLoopCity->getOwner()).getCivilizationInfo();
					BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);
					if(eBuilding != NO_BUILDING)
					{

						CvBuildingEntry* pBuildingEntry = GC.GetGameBuildings()->GetEntry(eBuilding);
						if (!pBuildingEntry || !pBuildingEntry->NullifyInfluenceModifier())
						{
							continue;
						}

						if(pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							bTargetHasGreatFirewall = true;				
						}
					}
				}
			}
		}

		// Loop through each of our cities
		for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			// Design has changed so modifier is always player-to-player so only need to get it once and can apply it at the end
			if (pLoopCity->isCapital())
			{
				iModifier = pLoopCity->GetCityCulture()->GetTourismMultiplier(kOtherPlayer.GetID(), false, false, false, false, false);
			}

			int iInfluenceToAdd = pLoopCity->GetCityCulture()->GetBaseTourism();

			// if we have the internet online, check to see if the opponent has the firewall
			// if they have the firewall, deduct the internet bonus from them
			if (iTechSpreadModifier > 0 && bTargetHasGreatFirewall)
			{
				int iInfluenceWithoutModifier = pLoopCity->GetCityCulture()->GetBaseTourismBeforeModifiers();
				int iInfluenceWithTechModifier = iInfluenceWithoutModifier * iTechSpreadModifier;
				iInfluenceToAdd -= (iInfluenceWithTechModifier / 100);
			}
			
			iRtnValue += iInfluenceToAdd;
		}
		iRtnValue = iRtnValue * (100 + iModifier) / 100;
	}

	return iRtnValue;

}

/// Current influence level on this player
InfluenceLevelTypes CvPlayerCulture::GetInfluenceLevel(PlayerTypes ePlayer) const
{
	InfluenceLevelTypes eRtnValue;

	CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);
	CvTeam &kOtherTeam = GET_TEAM(kOtherPlayer.getTeam());

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

		if (iPercent >= GC.getCULTURE_LEVEL_DOMINANT())
		{
			eRtnValue = INFLUENCE_LEVEL_DOMINANT;
		}
		else if (iPercent >= GC.getCULTURE_LEVEL_INFLUENTIAL())
		{
			eRtnValue = INFLUENCE_LEVEL_INFLUENTIAL;
		}
		else if (iPercent >= GC.getCULTURE_LEVEL_POPULAR())
		{
			eRtnValue = INFLUENCE_LEVEL_POPULAR;
		}
		else if (iPercent >= GC.getCULTURE_LEVEL_FAMILIAR())
		{
			eRtnValue = INFLUENCE_LEVEL_FAMILIAR;
		}
		else if (iPercent >= GC.getCULTURE_LEVEL_EXOTIC())
		{
			eRtnValue = INFLUENCE_LEVEL_EXOTIC;
		}
	}

	return eRtnValue;
}

/// Current influence trend on this player
InfluenceLevelTrend CvPlayerCulture::GetInfluenceTrend(PlayerTypes ePlayer) const
{
	InfluenceLevelTrend eRtnValue = INFLUENCE_TREND_STATIC;

	CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);

	// PctTurn1 = InfluenceT1 / LifetimeCultureT1
	// PctTurn2 = InfluenceT2 / LifetimeCultureT2
	
	// So if looking at is PctT2 > PctT1, can see if  (InfluenceT2 * LifetimeCultureT1) > (InfluenceT1 * LifetimeCultureT2)
	int iLHS = GetInfluenceOn(ePlayer) * kOtherPlayer.GetCulture()->GetLastTurnLifetimeCulture();
	int iRHS = GetLastTurnInfluenceOn(ePlayer) * kOtherPlayer.GetJONSCultureEverGenerated();

	if (kOtherPlayer.GetCulture()->GetLastTurnLifetimeCulture() > 0 && kOtherPlayer.GetJONSCultureEverGenerated() > 0)
	{
		if (iLHS > iRHS)
		{
			eRtnValue = INFLUENCE_TREND_RISING;
		}
		else if (iLHS < iRHS)
		{
			eRtnValue = INFLUENCE_TREND_FALLING;
		}
	}
		
	return eRtnValue;
}

/// If influence is rising, how many turns until we get to Influential? (999 if not rising fast enough to make it there eventually)
int CvPlayerCulture::GetTurnsToInfluential(PlayerTypes ePlayer) const
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
		int iInflPerTurn = GetInfluencePerTurn(ePlayer);
		int iCulture = kOtherPlayer.GetJONSCultureEverGenerated();
		int iCultPerTurn = kOtherPlayer.GetTotalJONSCulturePerTurn();

		int iNumerator = (GC.getCULTURE_LEVEL_INFLUENTIAL() * iCulture / 100) -  iInfluence;
		int iDivisor = iInflPerTurn - (GC.getCULTURE_LEVEL_INFLUENTIAL() * iCultPerTurn / 100);

		if (iDivisor > 0)
		{
			iRtnValue = iNumerator / iDivisor;

			// Round up
			if (iNumerator % iDivisor != 0)
			{
				iRtnValue++;
			}
		}
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
	int iLowestPercent = GC.getCULTURE_LEVEL_INFLUENTIAL();   // Don't want to target civs if already influential

	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv() && !kTeam.isAtWar(m_pPlayer->getTeam()))
		{
			if (!bCheckOpenBorders || kTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
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

/// Get extra science from trade routes based on current influence level
int CvPlayerCulture::GetInfluenceTradeRouteScienceBonus(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;

	if (ePlayer < MAX_MAJOR_CIVS)
	{
		InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
		switch (eLevel)
		{
		case INFLUENCE_LEVEL_FAMILIAR:
			iRtnValue = 1;
			break;
		case INFLUENCE_LEVEL_POPULAR:
			iRtnValue = 2;
			break;
		case INFLUENCE_LEVEL_INFLUENTIAL:
			iRtnValue = 3;
			break;
		case INFLUENCE_LEVEL_DOMINANT:
			iRtnValue = 4;
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
	
	return iRtnValue;
}

/// Get spy time to establish surveillance based on current influence level
int CvPlayerCulture::GetInfluenceSurveillanceTime(PlayerTypes ePlayer) const
{
	int iRtnValue = 3;

	if (ePlayer < MAX_MAJOR_CIVS)
	{
		InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);

		if (eLevel >= INFLUENCE_LEVEL_FAMILIAR)
		{
			iRtnValue = 1;
		}
	}
	else
	{
		// Have a major power ally?
		CvPlayer &kCityState = GET_PLAYER(ePlayer);
		if (kCityState.isMinorCiv())
		{
			PlayerTypes eAlly = kCityState.GetMinorCivAI()->GetAlly();
			if (eAlly != NO_PLAYER)
			{
				InfluenceLevelTypes eLevel = GetInfluenceLevel(eAlly);
				if (eLevel >= INFLUENCE_LEVEL_FAMILIAR)
				{
					iRtnValue = 1;
				}
			}
		}
	}

	return iRtnValue;
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
			case INFLUENCE_LEVEL_POPULAR:
				iRtnValue = 1;
				break;
			case INFLUENCE_LEVEL_INFLUENTIAL:
				iRtnValue = 1;
				break;
			case INFLUENCE_LEVEL_DOMINANT:
				iRtnValue = 2;
				break;
			}
		}
	}

	return iRtnValue;
}

/// Get extra spy rank in major civ cities based on current influence level
int CvPlayerCulture::GetInfluenceMajorCivSpyRankBonus(PlayerTypes ePlayer) const
{
	int iRtnValue = 0;

	InfluenceLevelTypes eLevel = GetInfluenceLevel(ePlayer);
	switch (eLevel)
	{
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
CvString CvPlayerCulture::GetInfluenceSpyRankTooltip(CvString szName, CvString szRank, PlayerTypes ePlayer)
{
	CvString szRtnValue = GetLocalizedText("TXT_KEY_EO_SPY_RANK_TT", szName, szRank);

	if (ePlayer != NO_PLAYER)
	{
		CvPlayer &kOtherPlayer = GET_PLAYER(ePlayer);

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

	CvCity *pCity;
	int iLoop;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		iRtnValue += pCity->GetCityCulture()->GetBaseTourism();
	}

	return iRtnValue;
}

/// At the player level, what is the modifier for tourism between these players?
int CvPlayerCulture::GetTourismModifierWith(PlayerTypes ePlayer) const
{
	int iMultiplier = 0;
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
	PolicyBranchTypes eMyIdeology = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

	// Open borders with this player
	if (kTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
	{
		iMultiplier += GetTourismModifierOpenBorders();
	}

	// Trade route to one of this player's cities from here
	if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), ePlayer))
	{
		iMultiplier += GetTourismModifierTradeRoute();
	}

	if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
	{
		iMultiplier += GC.getTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES();

		if (m_pPlayer->GetEspionage()->IsMyDiplomatVisitingThem(ePlayer))
		{
			iMultiplier += GC.getTOURISM_MODIFIER_DIPLOMAT();
		}
	}

	int iLessHappyMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_LESS_HAPPY);
	if (iLessHappyMod > 0)
	{
		if (m_pPlayer->GetExcessHappiness() > kPlayer.GetExcessHappiness())
		{
			iMultiplier += iLessHappyMod;
		}
	}
	int iCommonFoeMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_COMMON_FOE);
	if (iCommonFoeMod > 0)
	{
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (eLoopPlayer != ePlayer && eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				// Are they at war with me too?
				if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					iMultiplier += iCommonFoeMod;
				}
			}
		}
	}
	int iSharedIdeologyMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY);
	if (iSharedIdeologyMod > 0)
	{
		if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology == eTheirIdeology)
		{
			iMultiplier += iSharedIdeologyMod;
		}
	}

	if (m_pPlayer->isGoldenAge() && m_pPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier())
	{
		iMultiplier += m_pPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier();
	}

	ReligionTypes ePlayerReligion = m_pPlayer->GetReligions()->GetReligionInMostCities();
	if (ePlayerReligion != NO_RELIGION && kPlayer.GetReligions()->HasReligionInMostCities(ePlayerReligion))
	{
		iMultiplier += GetTourismModifierSharedReligion();
	}

	return iMultiplier;
}

/// Tooltip for GetTourismModifierWith()
CvString CvPlayerCulture::GetTourismModifierWithTooltip(PlayerTypes ePlayer) const
{
	CvString szRtnValue = "";
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
	PolicyBranchTypes eMyIdeology = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

	// POSITIVE MODIFIERS

	// Open borders with this player
	if (kTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_OPEN_BORDERS", GetTourismModifierOpenBorders()) + "[ENDCOLOR]";
	}

	// Trade route to one of this player's cities from here
	if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), ePlayer))
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_TRADE_ROUTE", GetTourismModifierTradeRoute()) + "[ENDCOLOR]";
	}

	ReligionTypes ePlayerReligion = m_pPlayer->GetReligions()->GetReligionInMostCities();
	if (ePlayerReligion != NO_RELIGION && kPlayer.GetReligions()->HasReligionInMostCities(ePlayerReligion))
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_RELIGION_NOTE", GetTourismModifierSharedReligion()) + "[ENDCOLOR]";
	}

	if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
	{
		if (m_pPlayer->GetEspionage()->IsMyDiplomatVisitingThem(ePlayer))
		{
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA", GC.getTOURISM_MODIFIER_DIPLOMAT()) + "[ENDCOLOR]";
		}
	}

	int iSharedIdeologyMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY);
	if (iSharedIdeologyMod > 0)
	{
		if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology == eTheirIdeology)
		{
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_SHARED_IDEOLOGY", iSharedIdeologyMod) + "[ENDCOLOR]";
		}
	}

	int iCommonFoeMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_COMMON_FOE);
	if (iCommonFoeMod > 0)
	{
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if (eLoopPlayer != ePlayer && eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				// Are they at war with me too?
				if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_COMMON_FOE", iCommonFoeMod) + "[ENDCOLOR]";
				}
			}
		}
	}

	int iLessHappyMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_LESS_HAPPY);
	if (iLessHappyMod > 0)
	{
		if (m_pPlayer->GetExcessHappiness() > kPlayer.GetExcessHappiness())
		{
			szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_LESS_HAPPY", iLessHappyMod) + "[ENDCOLOR]";
		}
	}

	if (m_pPlayer->isGoldenAge() && m_pPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier())
	{
		szRtnValue += "[COLOR_POSITIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_CARNIVAL", m_pPlayer->GetPlayerTraits()->GetGoldenAgeTourismModifier()) + "[ENDCOLOR]";
	}

	// NEUTRAL MODIFIERS
	if (!kTeam.IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
	{
		szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_OPEN_BORDERS", 0) + "[ENDCOLOR]";		
	}
	if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pPlayer->GetID(), ePlayer))
	{
		szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_TRADE_ROUTE", 0) + "[ENDCOLOR]";		
	}
	if (ePlayerReligion == NO_RELIGION || !kPlayer.GetReligions()->HasReligionInMostCities(ePlayerReligion))
	{
		szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_RELIGION_NOTE", 0) + "[ENDCOLOR]";
	}
	if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
	{
		if (!m_pPlayer->GetEspionage()->IsMyDiplomatVisitingThem(ePlayer))
		{
			szRtnValue += "[COLOR_GREY]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA", 0) + "[ENDCOLOR]";
		}
	}

	// NEGATIVE MODIFIERS
	if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
	{
		szRtnValue += "[COLOR_NEGATIVE_TEXT]" + GetLocalizedText("TXT_KEY_CO_PLAYER_TOURISM_DIFFERENT_IDEOLOGIES", GC.getTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES()) + "[ENDCOLOR]";
	}

	return szRtnValue;
}

/// Tourism modifier (base plus policy boost) - shared religion
int CvPlayerCulture::GetTourismModifierSharedReligion() const
{
	return GC.getTOURISM_MODIFIER_SHARED_RELIGION() + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_SHARED_RELIGION_TOURISM_MODIFIER);
}

/// Tourism modifier (base plus policy boost) - trade route
int CvPlayerCulture::GetTourismModifierTradeRoute() const
{
	return GC.getTOURISM_MODIFIER_SHARED_RELIGION() + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_ROUTE_TOURISM_MODIFIER);
}

/// Tourism modifier (base plus policy boost) - open borders
int CvPlayerCulture::GetTourismModifierOpenBorders() const
{
	return GC.getTOURISM_MODIFIER_SHARED_RELIGION() + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_OPEN_BORDERS_TOURISM_MODIFIER);
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

/// How strong will a concert tour be right now?
int CvPlayerCulture::GetTourismBlastStrength(int iMultiplier)
{
	int iStrength = iMultiplier * GetTourism();
	
	// Scale by game speed
	iStrength *= GC.getGame().getGameSpeedInfo().getCulturePercent();
	iStrength /= 100;

	return max(iStrength, GC.getMINIUMUM_TOURISM_BLAST_STRENGTH());
}

/// Add tourism with all known civs
void CvPlayerCulture::AddTourismAllKnownCivs(int iTourism)
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if (eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			ChangeInfluenceOn(eLoopPlayer, iTourism);
		}
	}
}

// PRIVATE METHODS

/// Once per turn calculation of public opinion data
void CvPlayerCulture::DoPublicOpinion()
{
	m_eOpinion = NO_PUBLIC_OPINION;
	m_ePreferredIdeology = NO_POLICY_BRANCH_TYPE;
	m_iOpinionUnhappiness = 0;
	m_strOpinionTooltip = "";
	m_strOpinionUnhappinessTooltip = "";
	m_eOpinionBiggestInfluence = NO_PLAYER;

	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();

	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return;
	}

	Localization::String locOverview = Localization::Lookup("TXT_KEY_CO_OPINION_TT_OVERVIEW");
	m_strOpinionTooltip += locOverview.toUTF8();

	PolicyBranchTypes eCurrentIdeology = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();

	// We have an ideology, so public opinion matters
	if (eCurrentIdeology != NO_POLICY_BRANCH_TYPE)
	{
		int iPressureForFreedom = 0;
		int iPressureForAutocracy = 0;
		int iPressureForOrder = 0;
		CvString strWorldIdeologyPressureString = "";
		CvString strFreedomPressureString = "";
		CvString strAutocracyPressureString = "";
		CvString strOrderPressureString = "";

		// Look at World Congress
		iPressureForFreedom += GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eFreedomBranch);
		if (iPressureForFreedom > 0)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_CO_OPINION_TT_FOR_FREEDOM");
			CvString sIcons = "";
			for (int i = 0; i < iPressureForFreedom; i++)
			{
				sIcons += "[ICON_IDEOLOGY_FREEDOM]";
			}
			sTemp << sIcons;
			strWorldIdeologyPressureString += sTemp.toUTF8();
		}
		iPressureForAutocracy += GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eAutocracyBranch);
		if (iPressureForAutocracy > 0)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_CO_OPINION_TT_FOR_AUTOCRACY");
			CvString sIcons = "";
			for (int i = 0; i < iPressureForAutocracy; i++)
			{
				sIcons += "[ICON_IDEOLOGY_AUTOCRACY]";
			}
			sTemp << sIcons;
			strWorldIdeologyPressureString += sTemp.toUTF8();
		}
		iPressureForOrder += GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eOrderBranch);
		if (iPressureForOrder > 0)
		{
			Localization::String sTemp = Localization::Lookup("TXT_KEY_CO_OPINION_TT_FOR_ORDER");
			CvString sIcons = "";
			for (int i = 0; i < iPressureForOrder; i++)
			{
				sIcons += "[ICON_IDEOLOGY_ORDER]";
			}
			sTemp << sIcons;
			strWorldIdeologyPressureString += sTemp.toUTF8();
		}

		// Look at each civ
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
		{
			CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
			if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
			{
				PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
				if (eOtherCivIdeology != NO_POLICY_BRANCH_TYPE)
				{
					int iCulturalDominanceOverUs = kPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID()) - m_pPlayer->GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);
					if (iCulturalDominanceOverUs > 0)
					{
						if (eOtherCivIdeology == eFreedomBranch)
						{
							iPressureForFreedom += iCulturalDominanceOverUs;
							if (strFreedomPressureString.size() > 0)
							{
								strFreedomPressureString += ", ";
							}
							strFreedomPressureString += kPlayer.getCivilizationShortDescription();
							for (int iI = 0; iI < iCulturalDominanceOverUs; iI++)
							{
								strFreedomPressureString += "[ICON_IDEOLOGY_FREEDOM]";
							}
						}
						else if (eOtherCivIdeology == eAutocracyBranch)
						{
							iPressureForAutocracy += iCulturalDominanceOverUs;
							if (strAutocracyPressureString.size() > 0)
							{
								strAutocracyPressureString += ", ";
							}
							strAutocracyPressureString += kPlayer.getCivilizationShortDescription();
							for (int iI = 0; iI < iCulturalDominanceOverUs; iI++)
							{
								strAutocracyPressureString += "[ICON_IDEOLOGY_AUTOCRACY]";
							}
						}
						else
						{
							iPressureForOrder += iCulturalDominanceOverUs;
							if (strOrderPressureString.size() > 0)
							{
								strOrderPressureString += ", ";
							}
							strOrderPressureString += kPlayer.getCivilizationShortDescription();
							for (int iI = 0; iI < iCulturalDominanceOverUs; iI++)
							{
								strOrderPressureString += "[ICON_IDEOLOGY_ORDER]";
							}
						}
					}
				}
			}
		}

		// Now compute satisfaction with this branch compared to two other ones
		int iDissatisfaction = 0;
		if (eCurrentIdeology == eFreedomBranch)
		{
			if (iPressureForFreedom >= (iPressureForAutocracy + iPressureForOrder))
			{
				m_eOpinion = PUBLIC_OPINION_CONTENT;
			}
			else
			{
				if (iPressureForAutocracy > iPressureForOrder)
				{
					m_ePreferredIdeology = eAutocracyBranch;
				}
				else if (iPressureForOrder >= iPressureForAutocracy)
				{
					m_ePreferredIdeology = eOrderBranch;
				}
				iDissatisfaction = (iPressureForAutocracy + iPressureForOrder) - iPressureForFreedom;
			}
		}
		else if (eCurrentIdeology == eAutocracyBranch)
		{
			if (iPressureForAutocracy >= (iPressureForFreedom + iPressureForOrder))
			{
				m_eOpinion = PUBLIC_OPINION_CONTENT;
			}
			else
			{
				if (iPressureForFreedom >= iPressureForOrder)
				{
					m_ePreferredIdeology = eFreedomBranch;
				}
				else if (iPressureForOrder > iPressureForFreedom)
				{
					m_ePreferredIdeology = eOrderBranch;
				}
				iDissatisfaction = (iPressureForFreedom + iPressureForOrder) - iPressureForAutocracy;
			}
		}
		else
		{
			if (iPressureForOrder >= (iPressureForFreedom + iPressureForAutocracy))
			{
				m_eOpinion = PUBLIC_OPINION_CONTENT;
			}
			else
			{
				if (iPressureForFreedom > iPressureForAutocracy)
				{
					m_ePreferredIdeology = eFreedomBranch;
				}
				else if (iPressureForAutocracy >= iPressureForFreedom)
				{
					m_ePreferredIdeology = eAutocracyBranch;
				}
				iDissatisfaction = (iPressureForFreedom + iPressureForAutocracy) - iPressureForOrder;
			}
		}

		// Compute effects of dissatisfaction
		int iPerCityUnhappy = 1;
		int iUnhappyPerXPop = 10;
		if (m_eOpinion != PUBLIC_OPINION_CONTENT)
		{
			if (iDissatisfaction < 3)
			{
				m_eOpinion = PUBLIC_OPINION_DISSIDENTS;
			}
			else if (iDissatisfaction < 5)
			{
				m_eOpinion = PUBLIC_OPINION_CIVIL_RESISTANCE;
			}
			else
			{
				m_eOpinion = PUBLIC_OPINION_REVOLUTIONARY_WAVE;
			}

			m_iOpinionUnhappiness = ComputePublicOpinionUnhappiness(iDissatisfaction, iPerCityUnhappy, iUnhappyPerXPop);

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
		if (strFreedomPressureString.size() > 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_FOR_FREEDOM");
			locText << strFreedomPressureString;
			strFreedomPressureString = locText.toUTF8();
		}
		if (strAutocracyPressureString.size() > 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_FOR_AUTOCRACY");
			locText << strAutocracyPressureString;
			strAutocracyPressureString = locText.toUTF8();
		}
		if (strOrderPressureString.size() > 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_FOR_ORDER");
			locText << strOrderPressureString;
			strOrderPressureString = locText.toUTF8();
		}

		if (strWorldIdeologyPressureString.size() != 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_INFLUENCED_WORLD_IDEOLOGY");
			m_strOpinionTooltip += locText.toUTF8();
			m_strOpinionTooltip += strWorldIdeologyPressureString;
			m_strOpinionTooltip += "[NEWLINE][NEWLINE]";
		}

		if ((strFreedomPressureString.size() + strAutocracyPressureString.size() + strOrderPressureString.size()) == 0)
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_NOT_INFLUENCED");
			m_strOpinionTooltip += locText.toUTF8();
		}
		else
		{
			Localization::String locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_INFLUENCED_BY");
			m_strOpinionTooltip += locText.toUTF8();
			m_strOpinionTooltip += strFreedomPressureString + strAutocracyPressureString + strOrderPressureString;
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

			locText = Localization::Lookup("TXT_KEY_CO_OPINION_TT_UNHAPPINESS_LINE2");
			m_strOpinionUnhappinessTooltip += locText.toUTF8();

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

	PolicyBranchTypes eFreedomBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eAutocracyBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_AUTOCRACY();
	PolicyBranchTypes eOrderBranch = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();

	if (eFreedomBranch == NO_POLICY_BRANCH_TYPE || eAutocracyBranch == NO_POLICY_BRANCH_TYPE || eOrderBranch == NO_POLICY_BRANCH_TYPE)
	{
		return 0;
	}

	// Start with World Congress
	int iPressureForFreedom = GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eFreedomBranch);
	int iPressureForAutocracy = GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eAutocracyBranch);
	int iPressureForOrder = GC.getGame().GetGameLeagues()->GetPressureForIdeology(m_pPlayer->GetID(), eOrderBranch);

	// Look at each civ
	for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
		if (iLoopPlayer != m_pPlayer->GetID() && kPlayer.isAlive() && !kPlayer.isMinorCiv())
		{
			PolicyBranchTypes eOtherCivIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
			if (eOtherCivIdeology != NO_POLICY_BRANCH_TYPE)
			{
				int iCulturalDominanceOverUs = kPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID()) - m_pPlayer->GetCulture()->GetInfluenceLevel((PlayerTypes)iLoopPlayer);
				if (iCulturalDominanceOverUs > 0)
				{
					if (eOtherCivIdeology == eFreedomBranch)
					{
						iPressureForFreedom += iCulturalDominanceOverUs;
					}
					else if (eOtherCivIdeology == eAutocracyBranch)
					{
						iPressureForAutocracy += iCulturalDominanceOverUs;
					}
					else
					{
						iPressureForOrder += iCulturalDominanceOverUs;
					}
				}
			}
		}
	}

	if (eBranch == eFreedomBranch)
	{
		if (iPressureForFreedom < (iPressureForAutocracy + iPressureForOrder))
		{
			if (iPressureForAutocracy > iPressureForOrder)
			{
				m_ePreferredIdeology = eAutocracyBranch;
			}
			else if (iPressureForOrder >= iPressureForAutocracy)
			{
				m_ePreferredIdeology = eOrderBranch;
			}
			iDissatisfaction = (iPressureForAutocracy + iPressureForOrder) - iPressureForFreedom;
		}
	}
	else if (eBranch == eAutocracyBranch)
	{
		if (iPressureForAutocracy < (iPressureForFreedom + iPressureForOrder))
		{
			if (iPressureForFreedom >= iPressureForOrder)
			{
				m_ePreferredIdeology = eFreedomBranch;
			}
			else if (iPressureForOrder > iPressureForFreedom)
			{
				m_ePreferredIdeology = eOrderBranch;
			}
			iDissatisfaction = (iPressureForFreedom + iPressureForOrder) - iPressureForAutocracy;
		}
	}
	else
	{
		if (iPressureForOrder < (iPressureForFreedom + iPressureForAutocracy))
		{
			if (iPressureForFreedom > iPressureForAutocracy)
			{
				m_ePreferredIdeology = eFreedomBranch;
			}
			else if (iPressureForAutocracy >= iPressureForFreedom)
			{
				m_ePreferredIdeology = eAutocracyBranch;
			}
			iDissatisfaction = (iPressureForFreedom + iPressureForAutocracy) - iPressureForOrder;
		}
	}

	int iPerCityUnhappy = 1;
	int iUnhappyPerXPop = 10;

	if (iDissatisfaction == 0)
	{
		return 0;
	}
	else
	{
		return ComputePublicOpinionUnhappiness(iDissatisfaction, iPerCityUnhappy, iUnhappyPerXPop);
	}
}

bool CvPlayerCulture::WantsDiplomatDoingPropaganda(PlayerTypes eTargetPlayer) const
{
	// only return the top two
	int iFirstValue = NO_INFLUENCE_LEVEL;
	int iSecondValue = NO_INFLUENCE_LEVEL;
	PlayerTypes eFirstPlayer = NO_PLAYER;
	PlayerTypes eSecondPlayer = NO_PLAYER;

	// only do this if everybody is exotic
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if (ePlayer == m_pPlayer->GetID())
		{
			continue;
		}

		if (!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		int iInfluenceLevel = GetInfluenceLevel(ePlayer);
		if (iInfluenceLevel < INFLUENCE_LEVEL_EXOTIC)
		{
			return false;
		}
		else if (iInfluenceLevel > iFirstValue && iInfluenceLevel > iSecondValue)
		{
			iSecondValue = iFirstValue;
			eSecondPlayer = eFirstPlayer;
			iFirstValue = iInfluenceLevel;
			eFirstPlayer = ePlayer;
		}
		else if (iInfluenceLevel > iSecondValue)
		{
			iSecondValue = iInfluenceLevel;
			eSecondPlayer = ePlayer;
		}
	}

	return (eFirstPlayer == eTargetPlayer || eSecondPlayer == eTargetPlayer);
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

	CvCity *pCity;
	int iLoop;
	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		iRtnValue += pCity->GetCityBuildings()->GetThemingBonuses();;
	}

	return iRtnValue;
}

// PRIVATE METHODS

/// Compute effects of dissatisfaction
int CvPlayerCulture::ComputePublicOpinionUnhappiness(int iDissatisfaction, int &iPerCityUnhappy, int &iUnhappyPerXPop)
{
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
	FILogFile* pLog;
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

	CvString str;
	CvCity *pCity;
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
	AppendToLog(strHeader, strLog, "Tourism", GetTourism());
	AppendToLog(strHeader, strLog, "Theming Bonuses", GetTotalThemingBonuses());

	if(bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);

	CvCity *pLoopCity;
	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->GetCityCulture()->GetNumGreatWorks() > 0)
		{
			pLoopCity->GetCityCulture()->LogGreatWorks(pLog);
		}
	}
}

/// Utility function - AppendToLog
void CvPlayerCulture::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void CvPlayerCulture::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void CvPlayerCulture::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue)
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
	FILogFile* pLog;
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

void CvPlayerCulture::LogSwapWorks(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	CvString strLog;
	FILogFile* pLog;
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

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvPlayerCulture& writeTo)
{
	uint uiVersion;

	loadFrom >> uiVersion;

	int iEntriesToRead;
	int iTempX;
	int iTempY;
	CvPlot *pPlot;

	writeTo.m_aDigCompletePlots.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> iTempX;
		loadFrom >> iTempY;
		pPlot = GC.getMap().plot(iTempX, iTempY);
		writeTo.m_aDigCompletePlots.push_back(pPlot);
	}

	loadFrom >> writeTo.m_iLastTurnLifetimeCulture;
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> writeTo.m_aiCulturalInfluence[iI];
		loadFrom >> writeTo.m_aiLastTurnCulturalInfluence[iI];
	}

	if (uiVersion >= 2)
	{
		loadFrom >> writeTo.m_bReportedTwoCivsAway;
		loadFrom >> writeTo.m_bReportedOneCivAway;
	}
	else
	{
		writeTo.m_bReportedTwoCivsAway = false;
		writeTo.m_bReportedOneCivAway = false;
	}

	if (uiVersion >= 3)
	{
		int iTemp;
		loadFrom >> iTemp;
		writeTo.m_eOpinion = (PublicOpinionTypes)iTemp;
		loadFrom >> writeTo.m_ePreferredIdeology;
		loadFrom >> writeTo.m_iOpinionUnhappiness;
		loadFrom >> writeTo.m_strOpinionTooltip;
	}
	else
	{
		writeTo.m_eOpinion = NO_PUBLIC_OPINION;
		writeTo.m_ePreferredIdeology = NO_POLICY_BRANCH_TYPE;
		writeTo.m_iOpinionUnhappiness = 0;
		writeTo.m_strOpinionTooltip = "";
	}

	if (uiVersion >= 5)
	{
		loadFrom >> writeTo.m_strOpinionUnhappinessTooltip;
	}
	else
	{
		writeTo.m_strOpinionUnhappinessTooltip = "";
	}

	if (uiVersion >= 6)
	{
		loadFrom >> writeTo.m_eOpinionBiggestInfluence;
		loadFrom >> writeTo.m_iTurnIdeologySwitch;
	}
	else
	{
		writeTo.m_eOpinionBiggestInfluence = NO_PLAYER;
		writeTo.m_iTurnIdeologySwitch = -1;

	}
	if (uiVersion >= 4)
	{
		loadFrom >> writeTo.m_iSwappableWritingIndex;
		loadFrom >> writeTo.m_iSwappableArtIndex;
		loadFrom >> writeTo.m_iSwappableArtifactIndex;
		loadFrom >> writeTo.m_iSwappableMusicIndex;
	}
	else
	{
		writeTo.m_iSwappableWritingIndex = -1;
		writeTo.m_iSwappableArtIndex = -1;
		writeTo.m_iSwappableArtifactIndex = -1;
		writeTo.m_iSwappableMusicIndex = -1;
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvPlayerCulture& readFrom)
{
	uint uiVersion = 6;
	saveTo << uiVersion;

	vector<CvPlot *>::const_iterator it;
	saveTo << readFrom.m_aDigCompletePlots.size();
	for(it = readFrom.m_aDigCompletePlots.begin(); it != readFrom.m_aDigCompletePlots.end(); it++)
	{
		CvPlot *pPlot = *it;
		saveTo << pPlot->getX();
		saveTo << pPlot->getY();
	}

	saveTo << readFrom.m_iLastTurnLifetimeCulture;
	saveTo << MAX_MAJOR_CIVS;
	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		saveTo << readFrom.m_aiCulturalInfluence[iI];
		saveTo << readFrom.m_aiLastTurnCulturalInfluence[iI];
	}

	saveTo << readFrom.m_bReportedTwoCivsAway;
	saveTo << readFrom.m_bReportedOneCivAway;

	saveTo << readFrom.m_eOpinion;
	saveTo << readFrom.m_ePreferredIdeology;
	saveTo << readFrom.m_iOpinionUnhappiness;
	saveTo << readFrom.m_strOpinionTooltip;
	saveTo << readFrom.m_strOpinionUnhappinessTooltip;
	saveTo << readFrom.m_eOpinionBiggestInfluence;
	saveTo << readFrom.m_iTurnIdeologySwitch;

	saveTo << readFrom.m_iSwappableWritingIndex;
	saveTo << readFrom.m_iSwappableArtIndex;
	saveTo << readFrom.m_iSwappableArtifactIndex;
	saveTo << readFrom.m_iSwappableMusicIndex;

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
int CvCityCulture::GetNumGreatWorks() const
{
	return m_pCity->GetCityBuildings()->GetNumGreatWorks();
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

/// Clear out Great Works in a city (used by scenarios)
void CvCityCulture::ClearGreatWorks()
{
	CvPlayer &kCityPlayer = GET_PLAYER(m_pCity->getOwner());
	for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		CvCivilizationInfo& playerCivilizationInfo = kCityPlayer.getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
		if (eBuilding != NO_BUILDING)
		{
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
			if (pkBuilding)
			{
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					int iNumSlots = pkBuilding->GetGreatWorkCount();
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						m_pCity->GetCityBuildings()->SetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI, -1);
					}
				}
			}
		}
	}
}

/// Which type of Great Work slot can we add with the cheapest available culture building?
GreatWorkSlotType CvCityCulture::GetSlotTypeFirstAvailableCultureBuilding() const
{
	int iCheapest = MAX_INT;
	GreatWorkSlotType eRtnValue = NO_GREAT_WORK_SLOT;
	CvPlayer &kCityPlayer = GET_PLAYER(m_pCity->getOwner());

	for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		CvCivilizationInfo& playerCivilizationInfo = kCityPlayer.getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
		if (eBuilding != NO_BUILDING)
		{
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
			CvBuildingClassInfo *pkBuildingClass = GC.getBuildingClassInfo((BuildingClassTypes)iBuildingClassLoop);
			if (pkBuilding && pkBuildingClass && !isWorldWonderClass(*pkBuildingClass))
			{
				int iNumSlots = pkBuilding->GetGreatWorkCount();
				if (iNumSlots > 0 && m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) == 0)
				{
					if (m_pCity->canConstruct(eBuilding))
					{
						int iCost = pkBuilding->GetProductionCost();
						if (iCost < iCheapest)
						{
							iCheapest = iCost;
							eRtnValue = pkBuilding->GetGreatWorkSlotType();
						}
					}
				}
			}
		}
	}
	return eRtnValue;
}

/// Compute raw tourism from this city
int CvCityCulture::GetBaseTourismBeforeModifiers()
{
	// If we're in Resistance, then no Tourism!
	if(m_pCity->IsResistance() || m_pCity->IsRazing())
	{
		return 0;
	}

	int iBase = GetNumGreatWorks() * GC.getBASE_TOURISM_PER_GREAT_WORK();
	int iBonus = (m_pCity->GetCityBuildings()->GetGreatWorksTourismModifier() * iBase / 100);
	iBase += iBonus;

	iBase += m_pCity->GetCityBuildings()->GetThemingBonuses();

	int iPercent = m_pCity->GetCityBuildings()->GetLandmarksTourismPercent();
	if (iPercent > 0)
	{
		int iFromWonders = GetCultureFromWonders();
		int iFromNaturalWonders = GetCultureFromNaturalWonders();
		int iFromImprovements = GetCultureFromImprovements();
		iBase += ((iFromWonders + iFromNaturalWonders + iFromImprovements) * iPercent / 100);
	}

	ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
	if(pReligion)
	{
		int iFaithBuildingTourism = pReligion->m_Beliefs.GetFaithBuildingTourism();
		if (iFaithBuildingTourism > 0)
		{
			iBase += m_pCity->GetCityBuildings()->GetNumBuildingsFromFaith() * iFaithBuildingTourism;
		}

		// Buildings
		for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(m_pCity->getOwner()).getCivilizationInfo();
			BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

			if(eBuilding != NO_BUILDING)
			{
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					iBase += pReligion->m_Beliefs.GetBuildingClassTourism(eBuildingClass);
				}
			}
		}
	}

	// Tech enhanced Tourism
	for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(!pkBuildingClassInfo)
		{
			continue;
		}

		CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(m_pCity->getOwner()).getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

		if(eBuilding != NO_BUILDING)
		{
			CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
			if(pkEntry && m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				int iTourism = pkEntry->GetTechEnhancedTourism();
				if (iTourism > 0 && GET_TEAM(m_pCity->getTeam()).GetTeamTechs()->HasTech((TechTypes)pkEntry->GetEnhancedYieldTech()))
				{
					iBase += iTourism;
				}
			}
		}
	}

	return iBase;
}

/// What is the tourism output ignoring player-specific modifiers?
int CvCityCulture::GetBaseTourism()
{
	int iBase = GetBaseTourismBeforeModifiers();

	int iModifier = 0;

	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());
	int iTechSpreadModifier = kPlayer.GetInfluenceSpreadModifier();
	if (iTechSpreadModifier > 0)
	{
		iModifier += iTechSpreadModifier;
	}

	if (kPlayer.GetTourismBonusTurns() > 0)
	{
		iModifier += GC.getTEMPORARY_TOURISM_BOOST_MOD();
	}

	int iLeagueCityModifier = GC.getGame().GetGameLeagues()->GetCityTourismModifier(m_pCity->getOwner(), m_pCity);
	if (iLeagueCityModifier > 0)
	{
		iModifier += iLeagueCityModifier;
	}

	if (kPlayer.isGoldenAge())
	{
		iModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeTourismModifier();
	}
	

	int iBuildingMod = 0;
	for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		CvCivilizationInfo& playerCivilizationInfo = kPlayer.getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
		if (eBuilding != NO_BUILDING)
		{
			CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
			if (pkEntry)
			{
				iBuildingMod = kPlayer.GetPlayerPolicies()->GetBuildingClassTourismModifier((BuildingClassTypes)iBuildingClassLoop);
				if (iBuildingMod != 0 && m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					iModifier += iBuildingMod;
				}
			}
		}
	}

	if (iModifier > 0)
	{
		iBase = iBase * (100 + iModifier) / 100;
	}

	return iBase;
}

/// What is the tourism modifier for one player
int CvCityCulture::GetTourismMultiplier(PlayerTypes ePlayer, bool bIgnoreReligion, bool bIgnoreOpenBorders, bool bIgnoreTrade, bool bIgnorePolicies, bool bIgnoreIdeologies) const
{
	int iMultiplier = 0;
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
	CvPlayer &kCityPlayer = GET_PLAYER(m_pCity->getOwner());
	PolicyBranchTypes eMyIdeology = kCityPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

	if (!bIgnoreReligion)
	{
		// City shares religion with this player
		ReligionTypes ePlayerReligion = kCityPlayer.GetReligions()->GetReligionInMostCities();
		if (ePlayerReligion != NO_RELIGION && kPlayer.GetReligions()->HasReligionInMostCities(ePlayerReligion))
		{
			iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierSharedReligion();
		}
	}

	if (!bIgnoreOpenBorders)
	{
		// Open borders with this player
		if (kTeam.IsAllowsOpenBordersToTeam(kCityPlayer.getTeam()))
		{
			iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierOpenBorders();
		}
	}

	if (!bIgnoreTrade)
	{
		// Trade route to one of this player's cities from here
		if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pCity->getOwner(), ePlayer))
		{
			iMultiplier += kCityPlayer.GetCulture()->GetTourismModifierTradeRoute();
		}
	}

	if (!bIgnoreIdeologies)
	{
		if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
		{
			iMultiplier += GC.getTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES();

			if (kCityPlayer.GetEspionage()->IsMyDiplomatVisitingThem(ePlayer))
			{
				iMultiplier += GC.getTOURISM_MODIFIER_DIPLOMAT();
			}
		}
	}

	if (!bIgnorePolicies)
	{
		int iLessHappyMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_LESS_HAPPY);
		if (iLessHappyMod > 0)
		{
			if (kCityPlayer.GetExcessHappiness() > kPlayer.GetExcessHappiness())
			{
				iMultiplier += iLessHappyMod;
			}
		}
		int iCommonFoeMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_COMMON_FOE);
		if (iCommonFoeMod > 0)
		{
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != ePlayer && eLoopPlayer != m_pCity->getOwner() && kCityPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
				{
					// Are they at war with me too?
					if (GET_TEAM(kCityPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
					{
						iMultiplier += iCommonFoeMod;
					}
				}
			}
		}
		int iSharedIdeologyMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY);
		if (iSharedIdeologyMod > 0)
		{
			if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology == eTheirIdeology)
			{
				iMultiplier += iSharedIdeologyMod;
			}
		}
	}

	// LATER add top science city and research agreement with this player???

	return iMultiplier;
}

/// What is the tooltip describing the tourism output?
CvString CvCityCulture::GetTourismTooltip()
{
	CvString szRtnValue = "";
	CvString szTemp;
 	CvString sharedReligionCivs = "";
	CvString openBordersCivs = "";
	CvString tradeRouteCivs = "";
	CvString lessHappyCivs = "";
	CvString commonFoeCivs = "";
	CvString sharedIdeologyCivs = "";
	CvString differentIdeologyCivs = "";
	TeamTypes eTeam = m_pCity->getTeam();
	CvPlayer &kCityPlayer = GET_PLAYER(m_pCity->getOwner());
	PolicyBranchTypes eMyIdeology = kCityPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
	ReligionTypes ePlayerReligion = kCityPlayer.GetReligions()->GetReligionInMostCities();

	// Great Works
	int iGWTourism = GetNumGreatWorks() * GC.getBASE_TOURISM_PER_GREAT_WORK();
	iGWTourism += (m_pCity->GetCityBuildings()->GetGreatWorksTourismModifier() * iGWTourism / 100);
	szRtnValue = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_GREAT_WORKS", iGWTourism, m_pCity->GetCityCulture()->GetNumGreatWorks());

	int iThemingBonuses = m_pCity->GetCityBuildings()->GetThemingBonuses();
	if (iThemingBonuses > 0)
	{
		szRtnValue += "[NEWLINE][NEWLINE]";
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_THEMING_BONUSES", iThemingBonuses);
	}

	// Landmarks, Wonders, Natural Wonders, Improvements
	int iTileTourism = 0;
	int iPercent = m_pCity->GetCityBuildings()->GetLandmarksTourismPercent();
	if (iPercent > 0)
	{
		int iFromWonders = GetCultureFromWonders();
		int iFromNaturalWonders = GetCultureFromNaturalWonders();
		int iFromImprovements = GetCultureFromImprovements();
		iTileTourism = ((iFromWonders + iFromNaturalWonders + iFromImprovements) * iPercent / 100);
		if (szRtnValue.length() > 0)
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_TILES", iTileTourism, iPercent);
	}

	// Beliefs
	int iSacredSitesTourism = 0;
	int iReligiousArtTourism = 0;
	ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
	const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
	if(pReligion)
	{
		int iFaithBuildingTourism = pReligion->m_Beliefs.GetFaithBuildingTourism();
		if (iFaithBuildingTourism > 0)
		{
			iSacredSitesTourism = m_pCity->GetCityBuildings()->GetNumBuildingsFromFaith() * iFaithBuildingTourism;
		}
		if (szRtnValue.length() > 0)
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_FAITH_BUILDINGS", iSacredSitesTourism);

		for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

			CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
			if(!pkBuildingClassInfo)
			{
				continue;
			}

			CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(m_pCity->getOwner()).getCivilizationInfo();
			BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

			if(eBuilding != NO_BUILDING)
			{
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					iReligiousArtTourism += pReligion->m_Beliefs.GetBuildingClassTourism(eBuildingClass);
				}
			}
		}
		if (szRtnValue.length() > 0)
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_RELIGIOUS_ART", iReligiousArtTourism);
	}

	// Tech enhanced Tourism
	for(int jJ = 0; jJ < GC.getNumBuildingClassInfos(); jJ++)
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)jJ;

		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if(!pkBuildingClassInfo)
		{
			continue;
		}

		CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(m_pCity->getOwner()).getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

		if(eBuilding != NO_BUILDING)
		{
			if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				int iTechEnhancedTourism = GC.getBuildingInfo(eBuilding)->GetTechEnhancedTourism();
				if (iTechEnhancedTourism > 0 && GET_TEAM(m_pCity->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildingInfo(eBuilding)->GetEnhancedYieldTech()))
				{
					if (szRtnValue.length() > 0)
					{
						szRtnValue += "[NEWLINE][NEWLINE]";
					}
					szRtnValue += GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_TECH_ENHANCED", iTechEnhancedTourism);
				}
			}
		}
	}

	int iBuildingMod = 0;
	for(int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		CvCivilizationInfo& playerCivilizationInfo = kCityPlayer.getCivilizationInfo();
		BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
		if (eBuilding != NO_BUILDING)
		{
			CvBuildingEntry *pkEntry = GC.getBuildingInfo(eBuilding);
			if (pkEntry)
			{
				iBuildingMod = kCityPlayer.GetPlayerPolicies()->GetBuildingClassTourismModifier((BuildingClassTypes)iBuildingClassLoop);
				if (iBuildingMod != 0 && m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					if (szRtnValue.length() > 0)
					{
						szRtnValue += "[NEWLINE][NEWLINE]";
					}
					szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_BUILDING_BONUS", iBuildingMod, pkEntry->GetDescription());
					szRtnValue += szTemp;
				}
			}
		}
	}

	// Get policy bonuses
	int iLessHappyMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_LESS_HAPPY);
	int iCommonFoeMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_COMMON_FOE);
	int iSharedIdeologyMod = kCityPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TOURISM_MOD_SHARED_IDEOLOGY);

	// If generating any, itemize which players we have bonuses with
	if (iGWTourism > 0 || iTileTourism > 0)
	{
		for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
		{
			CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iLoopPlayer);
			PolicyBranchTypes eTheirIdeology = kPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();
			if (kPlayer.isAlive() && !kPlayer.isMinorCiv() && iLoopPlayer != m_pCity->getOwner() && GET_TEAM(kCityPlayer.getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iLoopPlayer).getTeam()))
			{
				// City shares religion with this player
				if (kPlayer.GetReligions()->HasReligionInMostCities(ePlayerReligion))
				{
					if (sharedReligionCivs.length() > 0)
					{
						sharedReligionCivs += ", ";
					}
					sharedReligionCivs += kPlayer.getCivilizationShortDescription();
				}

				// Open borders with this player
				CvTeam &kTeam = GET_TEAM(kPlayer.getTeam());
				if (kTeam.IsAllowsOpenBordersToTeam(eTeam))
				{
					if (openBordersCivs.length() > 0)
					{
						openBordersCivs += ", ";
					}
					openBordersCivs += kPlayer.getCivilizationShortDescription();
				}

				// Trade route with this player
				if (GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(m_pCity->getOwner(), (PlayerTypes)iLoopPlayer))
				{
					if (tradeRouteCivs.length() > 0)
					{
						tradeRouteCivs += ", ";
					}
					tradeRouteCivs += kPlayer.getCivilizationShortDescription();
				}

				// POLICY BONUSES
				if (iLessHappyMod > 0)
				{
					if (kCityPlayer.GetExcessHappiness() > kPlayer.GetExcessHappiness())
					{
						if (lessHappyCivs.length() > 0)
						{
							lessHappyCivs += ", ";
						}
						lessHappyCivs += kPlayer.getCivilizationShortDescription();
					}
				}
				if (iCommonFoeMod > 0)
				{
					PlayerTypes eLoopPlayer;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						if(eLoopPlayer !=(PlayerTypes) iLoopPlayer && eLoopPlayer != m_pCity->getOwner() && kCityPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
						{
							// Are they at war with me too?
							if (GET_TEAM(kCityPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()) && GET_TEAM(kPlayer.getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
							{
								if (commonFoeCivs.length() > 0)
								{
									commonFoeCivs += ", ";
								}
								commonFoeCivs += kPlayer.getCivilizationShortDescription();
							}
						}
					}
				}

				// Shared ideology bonus (comes from a policy)
				if (iSharedIdeologyMod > 0)
				{
					if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology == eTheirIdeology)
					{
						if (sharedIdeologyCivs.length() > 0)
						{
							sharedIdeologyCivs += ", ";
						}
						sharedIdeologyCivs += kPlayer.getCivilizationShortDescription();
					}
				}

				// Different ideology penalty (applies all the time)
				if (eMyIdeology != NO_POLICY_BRANCH_TYPE && eTheirIdeology != NO_POLICY_BRANCH_TYPE && eMyIdeology != eTheirIdeology)
				{
					if (differentIdeologyCivs.length() > 0)
					{
						differentIdeologyCivs += ", ";
					}
					differentIdeologyCivs += kPlayer.getCivilizationShortDescription();
				}
			}
		}

		// Build the strings
		if (sharedReligionCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_RELIGION_BONUS", kCityPlayer.GetCulture()->GetTourismModifierSharedReligion());
			szRtnValue += szTemp + sharedReligionCivs;
		}
		if (openBordersCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_OPEN_BORDERS_BONUS", kCityPlayer.GetCulture()->GetTourismModifierOpenBorders());
			szRtnValue += szTemp + openBordersCivs;
		}
		if (tradeRouteCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_TRADE_ROUTE_BONUS", kCityPlayer.GetCulture()->GetTourismModifierTradeRoute());
			szRtnValue += szTemp + tradeRouteCivs;
		}
		if (lessHappyCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_LESS_HAPPY_BONUS", iLessHappyMod);
			szRtnValue += szTemp + lessHappyCivs;
		}
		if (commonFoeCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_COMMON_FOE_BONUS", iCommonFoeMod);
			szRtnValue += szTemp + commonFoeCivs;
		}
		if (sharedIdeologyCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_SHARED_IDEOLOGY_BONUS", iSharedIdeologyMod);
			szRtnValue += szTemp + sharedIdeologyCivs;
		}
		if (differentIdeologyCivs.length() > 0)
		{
			if (szRtnValue.length() > 0)
			{
				szRtnValue += "[NEWLINE][NEWLINE]";
			}
			szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_DIFFERENT_IDEOLOGY_PENALTY", GC.getTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES());
			szRtnValue += szTemp + differentIdeologyCivs;
		}
	}

	int iTechSpreadModifier = kCityPlayer.GetInfluenceSpreadModifier();
	if (iTechSpreadModifier > 0)
	{
		if (szRtnValue.length() > 0)
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_TECH_BONUS", iTechSpreadModifier);
		szRtnValue += szTemp;
	}
	int iLeagueCityModifier = GC.getGame().GetGameLeagues()->GetCityTourismModifier(m_pCity->getOwner(), m_pCity);
	if (iLeagueCityModifier > 0)
	{
		if (szRtnValue.length() > 0)
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_LEAGUES_BONUS", iLeagueCityModifier);
		szRtnValue += szTemp;
	}

	if (kCityPlayer.isGoldenAge() && kCityPlayer.GetPlayerTraits()->GetGoldenAgeTourismModifier())
	{
		if (szRtnValue.length() > 0)
		{
			szRtnValue += "[NEWLINE][NEWLINE]";
		}
		szTemp = GetLocalizedText("TXT_KEY_CO_CITY_TOURISM_CARNIVAL_BONUS", kCityPlayer.GetPlayerTraits()->GetGoldenAgeTourismModifier());
		szRtnValue += szTemp;
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
	CvString szTemp1, szTemp2, szTemp3;
	
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
	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());
	BuildingTypes eBuilding = (BuildingTypes)kPlayer.getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
	CvBuildingEntry *pkBuilding = GC.GetGameBuildings()->GetEntry(eBuilding);
	if (pkBuilding)
	{
		if (pkBuilding->GetThemingBonusInfo(0) != NULL)
		{
			return true;
		}
	}

	return false;
}

int CvCityCulture::GetThemingBonus(BuildingClassTypes eBuildingClass) const
{
	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());
	int iRtnValue = 0;

	if (IsThemingBonusPossible(eBuildingClass))
	{
		int iIndex = GetThemingBonusIndex(eBuildingClass);
		if (iIndex >= 0)
		{
			BuildingTypes eBuilding = (BuildingTypes)kPlayer.getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
			CvBuildingEntry *pkBuilding = GC.GetGameBuildings()->GetEntry(eBuilding);
			if (pkBuilding)
			{
				int iBonus = pkBuilding->GetThemingBonusInfo(iIndex)->GetBonus();
				int iModifier = kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_THEMING_BONUS);
				iRtnValue = iBonus * (100 + iModifier) / 100;
				if (m_pCity->isCapital())
				{
					iModifier = kPlayer.GetPlayerTraits()->GetCapitalThemingBonusModifier();
					if (iModifier > 0)
					{
						iRtnValue = iRtnValue * (100 + iModifier) / 100;

						if (kPlayer.isHuman() && !GC.getGame().isGameMultiPlayer() && iRtnValue >= 16)
						{
							gDLL->UnlockAchievement(ACHIEVEMENT_XP2_40);
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}

CvString CvCityCulture::GetThemingTooltip(BuildingClassTypes eBuildingClass) const
{
	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());
	CvString szBonusString = "";
	CvString szThemeDescription = "";
	CvString szRtnValue = "";

	if (IsThemingBonusPossible(eBuildingClass))
	{
		BuildingTypes eBuilding = (BuildingTypes)kPlayer.getCivilizationInfo().getCivilizationBuildings(eBuildingClass);
		CvBuildingEntry *pkBuilding = GC.GetGameBuildings()->GetEntry(eBuilding);
		if (pkBuilding)
		{
			int iIndex = GetThemingBonusIndex(eBuildingClass);
			if (iIndex >= 0)
			{
				szBonusString.Format("+%d: ", GetThemingBonus(eBuildingClass));

				if (pkBuilding->GetBuildingClassType() == (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_LOUVRE") ||
					pkBuilding->GetBuildingClassType() == (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_HERMITAGE"))
				{
					szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription());
				}
				else
				{
					int iGreatWork = m_pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, 0);
					CvGreatWork work = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWork];
					CvString szEraString = GC.getEraInfo(work.m_eEra)->getShortDesc();
					CvString szCivAdj = GET_PLAYER(m_pCity->getOwner()).getCivilizationAdjectiveKey();
					switch (iIndex)
					{
					case 0:
					case 2:
						szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription(), szEraString, szCivAdj);
						break;
					case 1:
					case 3:
						szCivAdj = "TXT_KEY_CO_WORLD_MUSEUM";
						szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription(), szEraString, szCivAdj);
						break;
					case 4:
					case 5:
					case 7:
						szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription(), szEraString);
						break;
					case 6:
						szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription(), szCivAdj, szEraString);
						break;
					case 8:
					case 10:
						szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription(), szCivAdj);
						break;
					case 9:
					case 11:
						szCivAdj = "TXT_KEY_CO_WORLD_MUSEUM";
						szThemeDescription = GetLocalizedText(pkBuilding->GetThemingBonusInfo(iIndex)->GetDescription(), szCivAdj);
						break;
					}
				}
				szRtnValue = szBonusString + szThemeDescription;
			}
			else
			{
				szRtnValue = GetLocalizedText("TXT_KEY_CO_NO_THEME");
				szRtnValue += "[NEWLINE][NEWLINE]";
				szRtnValue += GetLocalizedText(pkBuilding->GetThemingBonusHelp());
			}
		}
	}

	return szRtnValue;
}

// PRIVATE FUNCTIONS

/// City's current culture from wonders
int CvCityCulture::GetCultureFromWonders() const
{
	int iRtnValue = 0;
	CvPlayer &kPlayer = GET_PLAYER(m_pCity->getOwner());

	CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(kPlayer.getCivilizationType());
	if (pkCivInfo)
	{
		int iNumBuildingClassInfos = GC.getNumBuildingClassInfos();

		for(int iI = 0; iI < iNumBuildingClassInfos; iI++)
		{
			BuildingTypes eWonderBuilding = ((BuildingTypes)(pkCivInfo->getCivilizationBuildings(iI)));
			if (eWonderBuilding != NO_BUILDING)
			{
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eWonderBuilding) > 0)
				{
					CvBuildingEntry *pkBuildingInfo = GC.getBuildingInfo(eWonderBuilding);
					if (pkBuildingInfo)
					{
						if (isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
						{
							iRtnValue += pkBuildingInfo->GetYieldChange(YIELD_CULTURE);
							iRtnValue += GC.getGame().GetGameLeagues()->GetWorldWonderYieldChange(m_pCity->getOwner(), YIELD_CULTURE);
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}

/// City's current culture from natural wonders
int CvCityCulture::GetCultureFromNaturalWonders() const
{
	int iRtnValue = 0;
	CvPlot* pLoopPlot;

	// Look at all workable Plots
	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		if(iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = m_pCity->GetCityCitizens()->GetCityPlotFromIndex(iPlotLoop);

			if(pLoopPlot != NULL)
			{
				// Is this a Plot this City controls?
				if(pLoopPlot->getWorkingCity() != NULL && pLoopPlot->getWorkingCity()->GetID() == m_pCity->GetID())
				{
					// Working the Plot?
					if (m_pCity->GetCityCitizens()->IsWorkingPlot(pLoopPlot))
					{
						if(pLoopPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pLoopPlot->getFeatureType())->IsNaturalWonder())
						{
							iRtnValue += pLoopPlot->getYield(YIELD_CULTURE);
						}
					}
				}
			}
		}
	}
	return iRtnValue;
}

/// City's current culture from improvements
int CvCityCulture::GetCultureFromImprovements() const
{
	int iRtnValue = 0;
	CvPlot* pLoopPlot;

	// Look at all workable Plots
	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		if(iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = m_pCity->GetCityCitizens()->GetCityPlotFromIndex(iPlotLoop);

			if(pLoopPlot != NULL)
			{
				// Is this a Plot this City controls?
				if(pLoopPlot->getWorkingCity() != NULL && pLoopPlot->getWorkingCity()->GetID() == m_pCity->GetID())
				{
					// Working the Plot?
					if (m_pCity->GetCityCitizens()->IsWorkingPlot(pLoopPlot))
					{
						ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
						if (eImprovement != NO_IMPROVEMENT)
						{
							iRtnValue += pLoopPlot->calculateYield(YIELD_CULTURE);

							CvImprovementEntry* pImprovement = GC.getImprovementInfo(eImprovement);
							if(pImprovement && pImprovement->GetYieldChange(YIELD_CULTURE) > 0)
							{
								int iAdjacentCulture = pImprovement->GetCultureAdjacentSameType();
								if(iAdjacentCulture > 0)
								{
									iRtnValue += pLoopPlot->ComputeCultureFromAdjacentImprovement(*pImprovement, eImprovement);
								}
							}
						}
					}
				}
			}
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
	vector<int> aGreatWorkIndices;
	CvCivilizationInfo *pkCivInfo = GC.getCivilizationInfo(m_pCity->getCivilizationType());
	if (pkCivInfo)
	{
		BuildingTypes eBuilding = (BuildingTypes)pkCivInfo->getCivilizationBuildings(eBuildingClass);
		if(NO_BUILDING != eBuilding)
		{
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
				if (pkBuilding)
				{
					int iNumSlots = pkBuilding->GetGreatWorkCount();
					if (m_pCity->GetCityBuildings()->GetNumGreatWorksInBuilding(eBuildingClass) < iNumSlots)
					{
						return -1;  // No theming bonus if some slots still empty
					}

					// Store info on the attributes of all our Great Works
					for (int iI = 0; iI < iNumSlots; iI++)
					{
						int iGreatWork = m_pCity->GetCityBuildings()->GetBuildingGreatWork(eBuildingClass, iI);
						aGreatWorkIndices.push_back(iGreatWork);
					}

					return CultureHelpers::GetThemingBonusIndex(m_pCity->getOwner(), pkBuilding, aGreatWorkIndices);
				}
			}
		}
	}
	return -1;
}

// HELPER FUNCTIONS

/// Build a name for this artifact
GreatWorkType CultureHelpers::GetArtifact(CvPlot *pPlot)
{
	CvArchaeologyData archData = pPlot->GetArchaeologicalRecord();

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
		if(bHasEraSpecificItems == true)
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
	vector<EraTypes> aErasSeen;
	vector<PlayerTypes> aPlayersSeen;

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
			aErasSeen.push_back(work.m_eEra);
			aPlayersSeen.push_back(work.m_ePlayer);
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
			if (bValid && bonusInfo->IsSameEra())
			{
				int eFirstEra = aErasSeen[0];
				for (unsigned int kK = 1; kK < aErasSeen.size(); kK++)
				{
					if (aErasSeen[kK] != eFirstEra)
					{
						bValid = false;
					}
				}
			}
			if (bValid && bonusInfo->IsUniqueEras())
			{
				vector<EraTypes> aProcessedEras;
				aProcessedEras.push_back(aErasSeen[0]);
				for (unsigned int kK = 1; kK < aErasSeen.size(); kK++)
				{
					EraTypes eEra = aErasSeen[kK];
					if (std::find(aProcessedEras.begin(), aProcessedEras.end(), eEra) != aProcessedEras.end())
					{
						bValid = false;
					}
					else
					{
						aProcessedEras.push_back(eEra);
					}
				}
			}

			// Can we rule this out based on player?
			if (bValid && bonusInfo->IsRequiresOwner())
			{
				for (unsigned int kK = 0; kK < aPlayersSeen.size(); kK++)
				{
					if (aPlayersSeen[kK] != eOwner)
					{
						bValid = false;
					}
				}
			}
			if (bValid && bonusInfo->IsRequiresSamePlayer())
			{
				int eFirstPlayer = aPlayersSeen[0];
				for (unsigned int kK = 1; kK < aPlayersSeen.size(); kK++)
				{
					if (aPlayersSeen[kK] != eFirstPlayer)
					{
						bValid = false;
					}
				}
			}
			if (bValid && bonusInfo->IsRequiresUniquePlayers())
			{
				vector<PlayerTypes> aProcessedPlayers;
				aProcessedPlayers.push_back(aPlayersSeen[0]);
				for (unsigned int kK = 1; kK < aPlayersSeen.size(); kK++)
				{
					PlayerTypes ePlayer = aPlayersSeen[kK];
					if (std::find(aProcessedPlayers.begin(), aProcessedPlayers.end(), ePlayer) != aProcessedPlayers.end())
					{
						bValid = false;
					}
					else
					{
						aProcessedPlayers.push_back(ePlayer);
					}
				}
			}
			if (bValid && bonusInfo->IsRequiresAnyButOwner())
			{
				for (unsigned int kK = 0; kK < aPlayersSeen.size(); kK++)
				{
					if (aPlayersSeen[kK] == eOwner)
					{
						bValid = false;
					}
				}
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
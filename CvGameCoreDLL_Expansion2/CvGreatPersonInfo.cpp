#include "CvGameCoreDLLPCH.h"
#include "CvGreatPersonInfo.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_API_UNIFIED_YIELDS)

CvGreatPersonInfo::CvGreatPersonInfo() :
	m_iUnitClass(NO_UNITCLASS),
	m_iSpecialist(NO_SPECIALIST)
{
}

CvGreatPersonInfo::~CvGreatPersonInfo()
{
}

int CvGreatPersonInfo::GetUnitClassType() const
{
	return m_iUnitClass;
}

int CvGreatPersonInfo::GetSpecialistType() const
{
	return m_iSpecialist;
}

const char* CvGreatPersonInfo::GetIconString() const
{
	return m_strIconString;
}

bool CvGreatPersonInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	const char* szTextVal = NULL;

	szTextVal = kResults.GetText("Class");
	m_iUnitClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Specialist");
	m_iSpecialist = GC.getInfoTypeForString(szTextVal, true);

	m_strIconString = kResults.GetText("IconString");

	return true;
}

GreatPersonTypes GetGreatPersonFromUnitClass(UnitClassTypes eUnitClass)
{
	for (int iI = 0; iI < GC.getNumGreatPersonInfos(); ++iI) {
		GreatPersonTypes eGreatPerson = (GreatPersonTypes) iI;

		if (GC.getGreatPersonInfo(eGreatPerson)->GetUnitClassType() == eUnitClass) {
			return eGreatPerson;
		}
	}

	return NO_GREATPERSON;
}

GreatPersonTypes GetGreatPersonFromSpecialist(SpecialistTypes eSpecialist)
{
	for (int iI = 0; iI < GC.getNumGreatPersonInfos(); ++iI) {
		GreatPersonTypes eGreatPerson = (GreatPersonTypes) iI;

		if (GC.getGreatPersonInfo(eGreatPerson)->GetSpecialistType() == eSpecialist) {
			return eGreatPerson;
		}
	}

	return NO_GREATPERSON;
}

#endif

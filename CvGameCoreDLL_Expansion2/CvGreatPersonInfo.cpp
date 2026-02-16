#include "CvGameCoreDLLPCH.h"
#include "CvGreatPersonInfo.h"

// must be included after all other headers
#include "LintFree.h"

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
	std::map<UnitClassTypes, GreatPersonTypes>::const_iterator it = GC.m_unitClassToGreatPersonCache.find(eUnitClass);
	if (it != GC.m_unitClassToGreatPersonCache.end())
	{
		return it->second;
	}

	return NO_GREATPERSON;
}

GreatPersonTypes GetGreatPersonFromSpecialist(SpecialistTypes eSpecialist)
{
	std::map<SpecialistTypes, GreatPersonTypes>::const_iterator it = GC.m_specialistToGreatPersonCache.find(eSpecialist);
	if (it != GC.m_specialistToGreatPersonCache.end())
	{
		return it->second;
	}

	return NO_GREATPERSON;
}
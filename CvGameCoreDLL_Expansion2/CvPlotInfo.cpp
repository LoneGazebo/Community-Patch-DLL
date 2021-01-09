#include "CvGameCoreDLLPCH.h"
#include "CvPlotInfo.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_API_PLOT_YIELDS)

CvPlotInfo::CvPlotInfo() :
	m_bWater(false),
	m_bImpassable(false),
#if defined(MOD_PLOTS_EXTENSIONS)
	m_pppiAdjacentFeatureYieldChange(),
#endif
	m_piYields(NULL)
{
}

CvPlotInfo::~CvPlotInfo()
{
#if defined(MOD_PLOTS_EXTENSIONS)
	m_pppiAdjacentFeatureYieldChange.clear();
#endif
	SAFE_DELETE_ARRAY(m_piYields);
}

bool CvPlotInfo::isWater() const
{
	return m_bWater;
}

bool CvPlotInfo::isImpassable() const
{
	return m_bImpassable;
}

int CvPlotInfo::getYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYields ? m_piYields[i] : -1;
}

#if defined(MOD_PLOTS_EXTENSIONS)
//------------------------------------------------------------------------------
int CvPlotInfo::GetAdjacentFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield, bool bNaturalWonderPlot) const
{
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(eFeature > -1, "Index out of bounds");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(eYield > -1, "Index out of bounds");

	std::map<FeatureTypes, std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>>::const_iterator itFeature = m_pppiAdjacentFeatureYieldChange.find(eFeature);
	int iYieldChange = 0;
	if (itFeature != m_pppiAdjacentFeatureYieldChange.end())
	{
		if (bNaturalWonderPlot)
		{
			std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>::const_iterator itBool = itFeature->second.find(false);
			if (itBool != itFeature->second.end())
			{
				std::map<YieldTypes, int>::const_iterator itYield = itBool->second.find(eYield);
				if (itYield != itBool->second.end())
				{
					iYieldChange +=itYield->second;
				}
			}
		}
		else
		{
			std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>::const_iterator itBool;
			for(itBool = itFeature->second.begin(); itBool != itFeature->second.end(); ++itBool)
			{
				std::map<YieldTypes, int>::const_iterator itYield = itBool->second.find(eYield);
				if (itYield != itBool->second.end())
				{
					iYieldChange += itYield->second;
				}
			}
		}
	}

	return iYieldChange;
}
//------------------------------------------------------------------------------
/// Quick check if a plot gains yields from any adjacent feature
bool CvPlotInfo::IsAdjacentFeatureYieldChange(bool bNaturalWonderPlot) const
{
	if (bNaturalWonderPlot)
	{
		std::map<FeatureTypes, std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>>::const_iterator itFeature;
		for (itFeature = m_pppiAdjacentFeatureYieldChange.begin(); itFeature != m_pppiAdjacentFeatureYieldChange.end(); ++itFeature)
		{
			std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>::const_iterator itBool = itFeature->second.find(false);
			if (itBool != itFeature->second.end())
			{
				return true;
			}
		}
		return false;
	}
	return !m_pppiAdjacentFeatureYieldChange.empty();
}
//------------------------------------------------------------------------------
/// Check if a plot gains yields from a particular feature
bool CvPlotInfo::IsAdjacentFeatureYieldChange(FeatureTypes eFeature, bool bNaturalWonderPlot) const
{
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(eFeature > -1, "Index out of bounds");

	std::map<FeatureTypes, std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>>::const_iterator itFeature = m_pppiAdjacentFeatureYieldChange.find(eFeature);
	if (itFeature != m_pppiAdjacentFeatureYieldChange.end())
	{
		if (bNaturalWonderPlot)
		{
			std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>::const_iterator itBool = itFeature->second.find(false);
			if (itBool != itFeature->second.end())
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}
#endif

bool CvPlotInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_bWater = kResults.GetBool("Water");
	m_bImpassable = kResults.GetBool("Impassable");

	//Arrays
	const char* szPlotType = GetType();
	kUtility.SetYields(m_piYields, "Plot_Yields", "PlotType", szPlotType);

#if defined(MOD_PLOTS_EXTENSIONS)
	//Plot_AdjacentFeatureYieldChanges
	{
		std::string strKey("Plot_AdjacentFeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield, IgnoreNaturalWonderPlots from Plot_AdjacentFeatureYieldChanges inner join Features on FeatureType = Features.Type inner join Yields on YieldType = Yields.Type where PlotType = ?");
		}

		pResults->Bind(1, szPlotType);

		while (pResults->Step())
		{
			const FeatureTypes eFeature = (FeatureTypes)pResults->GetInt(0);
			const YieldTypes eYield = (YieldTypes)pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);
			const IgnoreNaturalWonders bIgnoreNaturalWonderPlots = pResults->GetBool(3);

			if (iYieldChange != 0)
			{
				m_pppiAdjacentFeatureYieldChange[eFeature][bIgnoreNaturalWonderPlots][eYield] += iYieldChange;
			}
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<FeatureTypes, std::map<IgnoreNaturalWonders, std::map<YieldTypes, int>>>(m_pppiAdjacentFeatureYieldChange).swap(m_pppiAdjacentFeatureYieldChange);
	}
#endif

	return true;
}

#endif
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


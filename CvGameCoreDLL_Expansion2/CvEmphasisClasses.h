/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_EMPHASIS_CLASSES_H
#define CIV5_EMPHASIS_CLASSES_H

#include "CvEnumMap.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvEmphasisEntry
//!  \brief		A single entry in the emphasis XML file
//
//!  Key Attributes:
//!  - Used to be called CvEmphasizeInfo
//!  - Populated from XML\GameInfo\CIV5EmphasizeInfos.xml
//!  - Array of these contained in CvEmphasisXMLEntries class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasisEntry: public CvBaseInfo
{
public:
	CvEmphasisEntry(void);
	virtual ~CvEmphasisEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	bool IsAvoidGrowth() const;
	bool IsGreatPeople() const;

	// Arrays
	int GetYieldChange(int i) const;

protected:

	bool m_bAvoidGrowth;
	bool m_bGreatPeople;

	// Arrays
	int* m_piYieldModifiers;

private:
	CvEmphasisEntry(const CvEmphasisEntry&);
	CvEmphasisEntry& operator=(const CvEmphasisEntry&);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvEmphasisXMLEntries
//!  \brief		Game-wide information about city emphasis
//
//! Key Attributes:
//! - Plan is it will be contained in CvGameRules object within CvGame class
//! - Populated from XML\GameInfo\CIV5EmphasizeInfos.xml
//! - Contains an array of CvEmphasisEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvCityEmphasis class (which stores the emphasis state for a single city)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasisXMLEntries
{
public:
	CvEmphasisXMLEntries(void);
	~CvEmphasisXMLEntries(void);

	// Accessor functions
	std::vector<CvEmphasisEntry*>& GetEmphasisEntries();
	int GetNumEmphases();
	_Ret_maybenull_ CvEmphasisEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvEmphasisEntry*> m_paEmphasisEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCityEmphases
//!  \brief		Information about the emphases selected for a single city
//
//!  Key Attributes:
//!  - One instance for each city
//!  - Accessed by any class that needs to check emphasis selections
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityEmphases
{
public:
	CvCityEmphases(void);
	~CvCityEmphases(void);
	void Init(CvCity* pCity);
	void Uninit();
	void Reset();

	// Accessor functions
	int GetEmphasizeAvoidGrowthCount() const;
	bool IsEmphasizeAvoidGrowth();
	int GetEmphasizeYieldCount(YieldTypes eIndex);
	bool IsEmphasizeYield(YieldTypes eIndex);
	bool IsEmphasize(EmphasizeTypes eIndex);
	void SetEmphasize(EmphasizeTypes eIndex, bool bNewValue);

	// Serialization
	template<typename CityEmphases, typename Visitor>
	static void Serialize(CityEmphases& cityEmphases, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

private:
	int m_iEmphasizeAvoidGrowthCount;
	int m_iEmphasizeGreatPeopleCount;
	CvEnumMap<YieldTypes, int> m_aiEmphasizeYieldCount;
	CvEnumMap<EmphasizeTypes, bool> m_pbEmphasize;

	CvCity* m_pCity;

	friend FDataStream& operator>>(FDataStream&, CvCityEmphases&);
	friend FDataStream& operator<<(FDataStream&, const CvCityEmphases&);
};

FDataStream& operator>>(FDataStream&, CvCityEmphases&);
FDataStream& operator<<(FDataStream&, const CvCityEmphases&);

#endif //CIV5_EMPHASIS_CLASSES_H
//---------------------------------------------------------------------------------------
//
//  *****************   CIV 5 World Builder Map   ********************
//
//  FILE:    CvWorldBuilderMapTypeDesc.h
//
//  AUTHOR:  Eric Jordan  --  4/5/2010
//
//  PURPOSE: Describes what the type IDs used in a world build save represent.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#pragma once
#ifndef CvWorldBuilderMapTypeDesc_h
#define CvWorldBuilderMapTypeDesc_h

class CvWorldBuilderMapTypeDesc
{
public:
	CvWorldBuilderMapTypeDesc();

	// This special string type ensures that all strings used in
	// CvWorldBuilderMapTypeDesc come from the same string palette.
	// This makes for fast string compares and better memory usage.
	class String
	{
	public:
		String() : m_sz(NULL) {}
		String(const String &sRhs) : m_sz(sRhs.m_sz) {}
		String(_In_opt_z_ const char *sz);

		_Ret_opt_z_ operator const char*() const { return m_sz; }
		_Ret_opt_z_ const char *operator =(_In_opt_z_ const char *sz);

		// This pointer compare is possible because the strings come from the same palette
		const bool operator ==(const String &sRhs) const { return (m_sz == sRhs.m_sz); }
		const bool operator ==(_In_z_ const char *szRhs) const { return (strcmp(m_sz, szRhs) == 0); }

	private:
		const char *m_sz;
	};

	class TypeMap
	{
		typedef std::vector<String> TypeArray;
		TypeArray asTypes;

	public:
		TypeMap(uint uiReserve = 0) { if (uiReserve > 0) asTypes.reserve(uiReserve); };

		bool FindType(const char *szType, uint &uiTypeOut) const;
		const size_t GetSerializedSize() const;

		uint Size() const { return asTypes.size(); };
		void Clear() { asTypes.clear(); };
		const String& Get(uint uiIndex) const;
		uint Add(const char* szType);
		uint Add(const String &szType);

		// Obviously, the calling code is responsible for ensuring the buffer
		// is large enough.  GetSerializedSize() should be sufficient for this.
		void Serialize(void *pvBuffer) const;

		void Deserialize(void *pvBuffer, size_t stSize);
	};

	typedef TypeMap TerrainTypeMap;
	TerrainTypeMap m_kTerrainTypes;

	typedef TypeMap FeatureTypeMap;
	FeatureTypeMap m_kFeatures;

	typedef TypeMap NaturalWonderTypeMap;
	NaturalWonderTypeMap m_kNaturalWonders;

	typedef TypeMap ResourceTypeMap;
	ResourceTypeMap m_kResources;

	typedef TypeMap ImprovementTypeMap;
	ImprovementTypeMap m_kImprovements;

	typedef TypeMap UnitTypeMap;
	UnitTypeMap m_kUnits;

	typedef TypeMap BuildingTypeMap;
	BuildingTypeMap m_kBuildingTypes;

	typedef TypeMap TechTypeMap;
	TechTypeMap m_kTechs;

	typedef TypeMap PolicyTypeMap;
	PolicyTypeMap m_kPolicies;

	typedef TypeMap UnitPromotionTypeMap;
	UnitPromotionTypeMap m_kUnitPromotions;

	typedef std::vector<uint> LookupTable;
	static bool BuildLookupTable(const TypeMap &kFrom, const TypeMap &kTo, LookupTable& auiLookupTable)
	{
		// Initialize the lookup table
		uint uiMaxSize = MAX(kFrom.Size(), kTo.Size());
		auiLookupTable.resize(uiMaxSize);
		for( uint i = 0; i < uiMaxSize; ++i )
			auiLookupTable[i] = uint(-1);

		bool bDifferencesFound = kFrom.Size() != kTo.Size();
		for( uint uiFrom = 0; uiFrom < kFrom.Size(); ++uiFrom )
		{
			// I know this looks like a string copy but it's just a string pointer copy
			const String sFrom = kFrom.Get(uiFrom);

			// Note: NULL and empty strings are considered to represent invalid types and are never allowed to have a match.
			const char *szFrom = sFrom;
			if( szFrom && *szFrom )
			{
				for( uint uiTo = 0; uiTo < kTo.Size(); ++uiTo )
				{
					// I know this looks like a string compare but since they're tabled
					// strings it's just a pointer comparison which is pretty fast.
					if( sFrom == kTo.Get(uiTo) )
					{
						auiLookupTable[uiFrom] = uiTo;
						if( uiFrom != uiTo ) bDifferencesFound = true;
						break;
					}
				}
			}
			else
			{
				// If from was NULL or empty but to is not then a difference has been found.
				const char *szTo = kTo.Get(uiFrom);
				if( szTo && *szTo )
					bDifferencesFound = true;
			}
		}

		return bDifferencesFound;
	}
};

#endif // CvWorldBuilderMapTypeDesc_h

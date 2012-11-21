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

	template<uint TSize>
	class TypeMap
	{
	public:
		TypeMap() : uiSize(0) {}

		static const uint MaxTypes = TSize;
		uint uiSize;
		Firaxis::Array<String, TSize> asTypes;

		bool FindType(const char *szType, uint &uiTypeOut)
		{
			// Get the string from the string table
			CvWorldBuilderMapTypeDesc::String sType(szType);
			for( uint i = 0; i < uiSize; ++i )
			{
				// I know this looks like a string compare but since they're tabled
				// strings it's just a pointer comparison which is pretty fast.
				if( asTypes[i] == sType )
				{
					uiTypeOut = i;
					return true;
				}
			}

			return false;
		}

		const size_t GetSerializedSize() const
		{
			const uint uiTypeCount = (TSize < uiSize)? TSize : uiSize;

			size_t stSerializedSize = 0;
			for( uint i = 0; i < uiTypeCount; ++i )
				stSerializedSize += FString::SafeStrlen(asTypes[i]) + 1;

			return stSerializedSize;
		}

		// Obviously, the calling code is responsible for ensuring the buffer
		// is large enough.  GetSerializedSize() should be sufficient for this.
		void Serialize(void *pvBuffer) const
		{
			const uint uiTypeCount = (TSize < uiSize)? TSize : uiSize;

			char *szWriteTo = (char*)pvBuffer;
			for( uint i = 0; i < uiTypeCount; ++i )
			{
				const char *szReadFrom = asTypes[i];
				if( szReadFrom )
				{
					while( *szReadFrom != '\0' )
					{
						*szWriteTo = *szReadFrom;
						++szReadFrom;
						++szWriteTo;
					}
				}

				*szWriteTo = '\0';
				++szWriteTo;
			}
		}

		void Deserialize(void *pvBuffer, size_t stSize)
		{
			uiSize = 0;
			FAssert(pvBuffer != NULL && stSize > 0);
			if( pvBuffer != NULL && stSize > 0 )
			{
				// Always ensure NULL termination
				((char*)pvBuffer)[stSize - 1] = '\0';

				const char *szReadFrom = (const char*)pvBuffer;
				while( szReadFrom < ((char*)pvBuffer + stSize) && uiSize < TSize )
				{
					asTypes[uiSize] = szReadFrom;
					szReadFrom += FString::SafeStrlen(szReadFrom) + 1;
					++uiSize;
				}
			}
		}
	};

	typedef TypeMap<32> TerrainTypeMap;
	TerrainTypeMap m_kTerrainTypes;

	typedef TypeMap<128> FeatureTypeMap;
	FeatureTypeMap m_kFeatures;

	typedef TypeMap<128> NaturalWonderTypeMap;
	NaturalWonderTypeMap m_kNaturalWonders;

	typedef TypeMap<64> ResourceTypeMap;
	ResourceTypeMap m_kResources;

	typedef TypeMap<64> ImprovementTypeMap;
	ImprovementTypeMap m_kImprovements;

	typedef TypeMap<256> UnitTypeMap;
	UnitTypeMap m_kUnits;

	typedef TypeMap<256> BuildingTypeMap;
	BuildingTypeMap m_kBuildingTypes;

	typedef TypeMap<256> TechTypeMap;
	TechTypeMap m_kTechs;

	typedef TypeMap<256> PolicyTypeMap;
	PolicyTypeMap m_kPolicies;

	typedef TypeMap<256> UnitPromotionTypeMap;
	UnitPromotionTypeMap m_kUnitPromotions;

	template<class TTypeMap>
	static bool BuildLookupTable(const TTypeMap &kFrom, const TTypeMap &kTo, uint auiLookupTable[TTypeMap::MaxTypes])
	{
		// Initialize the lookup table
		for( uint i = 0; i < TTypeMap::MaxTypes; ++i )
			auiLookupTable[i] = uint(-1);

		bool bDifferencesFound = kFrom.uiSize != kTo.uiSize;
		for( uint uiFrom = 0; uiFrom < kFrom.uiSize; ++uiFrom )
		{
			// I know this looks like a string copy but it's just a string pointer copy
			const String sFrom = kFrom.asTypes[uiFrom];

			// Note: NULL and empty strings are considered to represent invalid types and are never allowed to have a match.
			const char *szFrom = sFrom;
			if( szFrom && *szFrom )
			{
				for( uint uiTo = 0; uiTo < kTo.uiSize; ++uiTo )
				{
					// I know this looks like a string compare but since they're tabled
					// strings it's just a pointer comparison which is pretty fast.
					if( sFrom == kTo.asTypes[uiTo] )
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
				const char *szTo = kTo.asTypes[uiFrom];
				if( szTo && *szTo )
					bDifferencesFound = true;
			}
		}

		return bDifferencesFound;
	}
};

#endif // CvWorldBuilderMapTypeDesc_h

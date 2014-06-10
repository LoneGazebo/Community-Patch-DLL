/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#ifndef CVINFOSSERIALIZATIONHELPER_H
#define CVINFOSSERIALIZATIONHELPER_H

#pragma once

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper Functions to serialize arrays that use CvInfos as member data or as implicit IDs by
// the index in the array.
// The index / explicit type will be mapped to either a string version of the type or its unique hash
// This allows the info data set to change size.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
namespace CvInfosSerializationHelper
{
/// Read an array of T values and assume that the index in the array is the V0 index
template<typename TData>
void ReadV0DataArray(FDataStream& kStream, TData* paArray, int iArraySize, const char** ppszV0Tags, uint uiV0TagCount)
{
	for(uint iI = 0; iI < uiV0TagCount; iI++)
	{
		TData tValue;
		kStream >> tValue;
		int iType = GC.getInfoTypeForString(ppszV0Tags[iI]);
		if(iType != -1)
		{
			if(iType < iArraySize)
				paArray[iType] = tValue;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", ppszV0Tags[iI]);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}
}

/// Read an array of T values and assume that the index in the array is the V0 index
template<typename TData>
void ReadV0DataArray(FDataStream& kStream, std::vector<TData>& aiArray, uint uiMaxSize, const char** ppszV0Tags, uint uiV0TagCount)
{
	if(aiArray.size() < uiMaxSize)
		aiArray.resize(uiMaxSize);

	uint uiSize;
	kStream >> uiSize;	// Get the number written

	for(uint iI = 0; iI < uiV0TagCount && iI < uiSize; iI++)
	{
		TData tValue;
		kStream >> tValue;
		int iType = GC.getInfoTypeForString(ppszV0Tags[iI]);
		if(iType != -1)
		{
			aiArray[iType] = tValue;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", ppszV0Tags[iI]);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}
}

/// Read a 2 dimensional array of T values and assume that the primary index in the array is the V0 type index
template<typename TData>
void ReadV0DataArray(FDataStream& kStream, TData** ppaArray, int iSubArraySize, int iArraySize, const char** ppszV0Tags, uint uiV0TagCount)
{
	for(uint iI = 0; iI < uiV0TagCount; iI++)
	{
		TData tValue;
		int iType = GC.getInfoTypeForString(ppszV0Tags[iI]);
		if(iType != -1)
		{
			if(iType < iArraySize)
			{
				TData* paiArray = ppaArray[iType];
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				{
					kStream >> tValue;
					paiArray[iJ] = tValue;
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Type mapping outside the supplied array bounds: %s", ppszV0Tags[iI]);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);

				// Burn through the data so we can continue on a type outside the bounds
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
					kStream >> tValue;
			}
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", ppszV0Tags[iI]);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);

			for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				kStream >> tValue;
		}
	}
}

/// Read an array of T types and assume the source value is a V0 type (index)
template<typename TType>
void ReadV0TypeArray(FDataStream& kStream, std::vector<TType>& aiArray, uint uiMaxSize, const char** ppszV0Tags, uint uiV0TagCount)
{
	if(aiArray.size() < uiMaxSize)
		aiArray.resize(uiMaxSize);

	uint uiSize;
	kStream >> uiSize;	// Get the number written

	for(uint iI = 0; iI < uiSize; iI++)
	{
		TType eType;
		kStream >> eType;

		if(eType == (TType)-1)
			aiArray[iI] = (TType)-1;
		else
		{
			if((uint) eType < uiV0TagCount)
			{
				int iMappedType = GC.getInfoTypeForString(ppszV0Tags[eType]);
				if(iMappedType != -1)
				{
					aiArray[iI] = (TType)iMappedType;
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Type not found: %d", eType);
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
			else
				aiArray[iI] = (TType)-1;
		}
	}
}

/// Read an array of T types and assume the source value is a V0 type (index)
/// Assumes there is no count written
template<typename TType>
void ReadV0TypeArray(FDataStream& kStream, TType* paArray, uint uiSize, const char** ppszV0Tags, uint uiV0TagCount)
{
	for(uint iI = 0; iI < uiSize; iI++)
	{
		TType eType;
		kStream >> eType;

		if(eType == (TType)-1)
			paArray[iI] = (TType)-1;
		else
		{
			if((uint) eType < uiV0TagCount)
			{
				int iMappedType = GC.getInfoTypeForString(ppszV0Tags[eType]);
				if(iMappedType != -1)
				{
					paArray[iI] = (TType)iMappedType;
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Type not found: %d", eType);
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
			else
				paArray[iI] = (TType)-1;
		}
	}
}

// Convert a V0 type (index) to the type index for the currently loaded data
template<typename TType>
TType ConvertV0(TType eType, const char** ppszV0Tags, uint uiV0TagCount)
{
	if(eType != (TType)-1 && (uint)eType < uiV0TagCount)
	{
		int iMappedType = GC.getInfoTypeForString(ppszV0Tags[(int)eType]);
		if(iMappedType != -1)
		{
			return (TType)iMappedType;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %d", (int)eType);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}

	return (TType)-1;
}

/// Helper function to read in an array of T data that has a string'ized type ID before each entry
template<typename TData>
void ReadDataArray(FDataStream& kStream, TData* paArray, int iArraySize)
{
	uint uiNumEntries;
	FStringFixedBuffer(sTemp, 256);
	int iType;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = Read(kStream, &bValid);
		if(iType != -1)
		{
			TData tValue;
			kStream >> tValue;
			if((int)iType < iArraySize)
				paArray[iType] = tValue;
		}
		else if(!bValid)
		{
			TData tValue;
			kStream >> tValue;
		}
	}
}

/// Helper function to read in an array of T data that has a string'ized type ID before each entry
template<typename TData>
void ReadHashedDataArray(FDataStream& kStream, TData* paArray, int iArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = ReadHashed(kStream, &bValid);
		if(iType != -1)
		{
			TData tValue;
			kStream >> tValue;
			if((int)iType < iArraySize)
				paArray[iType] = tValue;
		}
		else if(!bValid)
		{
			TData tValue;
			kStream >> tValue;
		}
	}
}

/// Helper function to read in an two dimensional array of T data that has a string'ized type ID before each primary array entry
template<typename TData>
void ReadDataArray(FDataStream& kStream, TData** ppaArray, int iSubArraySize, int iArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = Read(kStream, &bValid);
		TData tValue;
		if(iType != -1)
		{
			if((int)iType < iArraySize)
			{
				int* paArray = ppaArray[iType];
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				{
					kStream >> tValue;
					paArray[iJ] = tValue;
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Type mapping outside the supplied array bounds: %d", iType);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				bValid = false;
			}

		}

		if(!bValid)
		{
			for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				kStream >> tValue;
		}
	}
}

/// Helper function to read in an two dimensional array of T data that has a string'ized type ID before each primary array entry
template<typename TData>
void ReadHashedDataArray(FDataStream& kStream, TData** ppaArray, int iSubArraySize, int iArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = ReadHashed(kStream, &bValid);
		TData tValue;
		if(iType != -1)
		{
			if((int)iType < iArraySize)
			{
				int* paArray = ppaArray[iType];
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				{
					kStream >> tValue;
					paArray[iJ] = tValue;
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Type mapping outside the supplied array bounds: %d", iType);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				bValid = false;
			}
		}

		if(!bValid)
		{
			for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				kStream >> tValue;
		}
	}
}

/// Helper function to read in an std::vector of T data that has a string'ized type ID before each entry
template<typename TData>
void ReadDataArray(FDataStream& kStream, std::vector<TData>& aiArray)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	if(aiArray.size() < uiNumEntries)
		aiArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = Read(kStream, &bValid);
		if(iType != -1)
		{
			if (iType >= (int)aiArray.size())
				aiArray.resize(iType+1);

			kStream >> aiArray[iType];
		}
		else if(!bValid)
		{
			TData tDummy;
			kStream >> tDummy;
		}
	}
}

/// Helper function to read in an std::vector of T data that has a string'ized type ID before each entry
template<typename TData>
void ReadHashedDataArray(FDataStream& kStream, std::vector<TData>& aiArray)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	if(aiArray.size() < uiNumEntries)
		aiArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = ReadHashed(kStream, &bValid);
		if(iType != -1)
		{
			if (iType >= (int)aiArray.size())
				aiArray.resize(iType+1);

			kStream >> aiArray[iType];
		}
		else if(!bValid)
		{
			TData tDummy;
			kStream >> tDummy;
		}
	}
}

/// Helper function to read in an array of hashed type entries and convert them to runtime type entries (indices)
template<typename TType>
void ReadTypeArray(FDataStream& kStream, std::vector<TType>& aArray)
{
	uint uiNumEntries;
	int iType;

	kStream >> uiNumEntries;

	if(aArray.size() < uiNumEntries)
		aArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		aArray[iI] = (TType)Read(kStream);
	}
}

/// Helper function to read in an array of hashed type entries and convert them to runtime type entries (indices)
template<typename TType>
void ReadHashedTypeArray(FDataStream& kStream, std::vector<TType>& aArray)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	if(aArray.size() < uiNumEntries)
		aArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		aArray[iI] = (TType)ReadHashed(kStream);
	}
}

/// Helper function to read in an array of hashed type entries and convert them to runtime type entries (indices)
template<typename TType>
void ReadHashedTypeArray(FDataStream& kStream, TType* paArray, uint uiArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		TType eType = (TType)ReadHashed(kStream);
		if(iI < uiArraySize)
			paArray[iI] = eType;
	}
}

/// Helper function to read in an array of hashed type entries and convert them to runtime type entries (indices)
/// This differs from the above only in that it uses the input array size, rather than reading the size
template<typename TType>
void ReadHashedTypeArray(FDataStream& kStream, uint uiInputArraySize, TType* paArray, uint uiOutputArraySize)
{
	for(uint iI = 0; iI < uiInputArraySize; iI++)
	{
		TType eType = (TType)ReadHashed(kStream);
		if(iI < uiOutputArraySize)
			paArray[iI] = eType;
	}
}

/// Helper function to read in an array of data entries and remap them to new locations
/// Note: This assumes the array in the stream does NOT have a lead count value.
template<typename TType>
bool ReadAndRemapDataArray(FDataStream& kStream, int iSrcCount, TType* pvDest, int iDestCount, const int* piaRemap)
{
	TType* pvBuffer = (TType*)_malloca(iSrcCount * sizeof(TType));
	kStream.ReadIt(iSrcCount * sizeof(TType), (void*)pvBuffer);

	bool bRemapSuccess = true;
	for(int i = 0; i < iSrcCount; ++i)
	{
		int iDestIndex = piaRemap[i];	// The new index
		if(iDestIndex >= 0 && iDestIndex < iDestCount)
		{
			pvDest[iDestIndex] = pvBuffer[i];
		}
		else
			bRemapSuccess = false;
	}
	_freea(pvBuffer);
	return bRemapSuccess;
}

/// Helper function to read in an array of hashed type entries and convert them to runtime type entries (indices)
template<typename TType>
void ReadTypeArrayDBLookup(FDataStream& kStream, std::vector<TType>& aArray, const char* szTable)
{
	uint uiNumEntries;
	kStream >> uiNumEntries;

	if(aArray.size() < uiNumEntries)
		aArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		aArray[iI] = (TType)ReadDBLookup(kStream, szTable);
	}
}



/// Helper function to read a single type ID as a string and convert it to an ID
int Read(FDataStream& kStream, bool* bValid = NULL);

/// Helper function to read a single resource type ID as a hash and convert it to an ID
int ReadHashed(FDataStream& kStream, bool* bValid = NULL);

/// Helper function to read a single type ID as a string and convert it to an ID using the database (not InfoTables)
int ReadDBLookup(FDataStream& kStream, const char* szTable, bool* bValid = NULL);


//////////////////////////////////////////////////////////////////////////
/// Writers
//////////////////////////////////////////////////////////////////////////

/// Helper function to write out an array of TData data, assuming the array index is the types runtime ID index of TType
template<typename TType, typename TData>
void WriteDataArray(FDataStream& kStream, TData* paArray, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(Write(kStream, eType))
		{
			kStream << paArray[iI];
		}
	}
}

/// Helper function to write out an array of TData data, assuming the array index is the types runtime ID index of TType
template<typename TType, typename TData>
void WriteHashedDataArray(FDataStream& kStream, TData* paArray, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(WriteHashed(kStream, eType))
		{
			kStream << paArray[iI];
		}
	}
}

/// Helper function to write out an array of data TData, assuming the array index is the types runtime ID index of TType
template<typename TType, typename TData>
void WriteDataArray(FDataStream& kStream, const std::vector<TData>& aArray)
{
	kStream << aArray.size();

	for(uint iI = 0; iI < aArray.size(); iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(Write(kStream, eType))
		{
			kStream << aArray[iI];
		}
	}
}

/// Helper function to write out an array of data TData, assuming the array index is the types runtime ID index of TType
template<typename TType, typename TData>
void WriteHashedDataArray(FDataStream& kStream, const std::vector<TData>& aArray)
{
	kStream << aArray.size();

	for(uint iI = 0; iI < aArray.size(); iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(WriteHashed(kStream, eType))
		{
			kStream << aArray[iI];
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of resource types
template<typename TType, typename TData>
void WriteDataArray(FDataStream& kStream, TData** ppaArray, uint uiSubArraySize, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);

		if(Write(kStream, eType))
		{
			TData* paArray = ppaArray[iI];
			for(uint iJ = 0; iJ < uiSubArraySize; ++iJ)
			{
				kStream << paArray[iJ];
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of resource types
template<typename TType, typename TData>
void WriteHashedDataArray(FDataStream& kStream, TData** ppaArray, uint uiSubArraySize, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);

		if(WriteHashed(kStream, eType))
		{
			TData* paArray = ppaArray[iI];
			for(uint iJ = 0; iJ < uiSubArraySize; ++iJ)
			{
				kStream << paArray[iJ];
			}
		}
	}
}

/// Helper function to write out an array of data of TType as string resource IDs
template<typename TType>
void WriteTypeArray(FDataStream& kStream, const std::vector<TType>& aiArray)
{
	kStream << aiArray.size();

	for(uint iI = 0; iI < aiArray.size(); iI++)
	{
		Write(kStream, aiArray[iI]);
	}
}

/// Helper function to write out an array of data of TType as hashed resource IDs
template<typename TType>
void WriteHashedTypeArray(FDataStream& kStream, const std::vector<TType>& aiArray)
{
	kStream << aiArray.size();

	for(uint iI = 0; iI < aiArray.size(); iI++)
	{
		WriteHashed(kStream, aiArray[iI]);
	}
}

/// Helper function to write out an array of data of TType as hashed resource IDs
template<typename TType>
void WriteHashedTypeArray(FDataStream& kStream, TType* paArray, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		WriteHashed(kStream, paArray[iI]);
	}
}

#define CVINFO_V0_TAG_COUNT(x)	(sizeof(x) / sizeof(const char*))
#define CVINFO_V0_TAGS(x)	&x[0], sizeof(x) / sizeof(const char*)

//////////////////////////////////////////////////////////////////////////
// Start Type specific functions.

// These functions convert a runtime type ID (index) to a unique type
// string or hash

// The V0 tag table for ResourceTypes
extern const char* ms_V0ResourceTags[27];
/// Helper function to write out a single resource type ID as a string
bool Write(FDataStream& kStream, const ResourceTypes eType);
/// Helper function to write out a single resource type ID as a hash
bool WriteHashed(FDataStream& kStream, const ResourceTypes eType);

extern const char* ms_V0PolicyTags[60];
/// Helper function to write out a single policy type ID as a string
bool Write(FDataStream& kStream, const PolicyTypes eType);
/// Helper function to write out a single policy type ID as a hash
bool WriteHashed(FDataStream& kStream, const PolicyTypes eType);

extern const char* ms_V0PolicyBranchTags[10];
/// Helper function to write out a single policy branch type ID as a string
bool Write(FDataStream& kStream, const PolicyBranchTypes eType);
/// Helper function to write out a single policy branch type ID as a hash
bool WriteHashed(FDataStream& kStream, const PolicyBranchTypes eType);

/// Helper function to write out a single tech type ID as a string
bool Write(FDataStream& kStream, const TechTypes eType);

/// Helper function to write out a single tech type ID as a hash
bool WriteHashed(FDataStream& kStream, const TechTypes eType);

/// Helper function to write out a single belief type ID as a string
bool Write(FDataStream& kStream, const BeliefTypes eType);

/// Helper function to write out a single belief type ID as a hash
bool WriteHashed(FDataStream& kStream, const BeliefTypes eType);

/// Helper function to write out a single religion type ID as a string
bool Write(FDataStream& kStream, const ReligionTypes eType);

/// Helper function to write out a single religion type ID as a hash
bool WriteHashed(FDataStream& kStream, const ReligionTypes eType);

/// Helper function to write out a single PlayerColor type ID as a string
bool Write(FDataStream& kStream, const PlayerColorTypes eType);

/// Helper function to write out a single PlayerColor type ID as a hash
bool WriteHashed(FDataStream& kStream, const PlayerColorTypes eType);

/// Helper function to write out a single MinorCivType type ID as a string
bool Write(FDataStream& kStream, const MinorCivTypes eType);

/// Helper function to write out a single MinorCivType type ID as a hash
bool WriteHashed(FDataStream& kStream, const MinorCivTypes eType);

/// End Type specific functions
//////////////////////////////////////////////////////////////////////////

/// Helper function to write out an info type ID as string
bool Write(FDataStream& kStream, const CvBaseInfo* pkInfo);
/// Helper function to write out an info type ID as a hash
bool WriteHashed(FDataStream& kStream, const CvBaseInfo* pkInfo);
}

#endif // CVINFOSSERIALIZATIONHELPER_H
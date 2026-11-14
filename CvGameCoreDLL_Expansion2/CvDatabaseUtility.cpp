/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDatabaseUtility.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(_WIN32)
namespace
{
// The EXE registers every *.Civ5Pkg below its Assets folder (cvContentPackageManager::FindPackages
// searches it recursively) without checking its version. A registered package's folder joins the
// file catalog, so an XML file there with the name of a base game database file replaces that file
// at every start, whatever the Mods menu says; the last package listing a game DLL also supplies
// the DLL used without mods. That is how a modpack (Modpack Maker: VP_MODPACK\Override) brings its
// database, so an old Vox Populi or modpack left there sets up its own version of the tables this
// DLL reads (#13451), and verifying the game files does not remove it. A Firaxis package that is
// on disk but not owned by the current Steam account (installed by another account on the same PC)
// is rejected by the ownership test, yet its folder still broke loading saves in #13247.
const char* const s_aszFiraxisPackageIDs[] =
{
	"293C1EE3-1176-44f6-AC1F-59663826DE74", // DLC_01
	"B685D5DE-7CCA-4e75-81B4-2F60754E6330", // DLC_02
	"ECF7C605-BA11-4CAC-8D80-D71306AAC471", // DLC_03
	"B3030D39-C0D8-4bc7-91B1-7AD1CAF585AB", // DLC_04
	"112C22B2-5308-42b6-B734-171CCAB3037B", // DLC_05
	"BBB0D085-A0B1-4475-B007-3E549CF3ADC3", // DLC_06
	"EA67AED5-5859-4875-BF3A-360FE9E55D1B", // DLC_07
	"7459BA32-5764-44ae-8E95-01AD0E0EFD48", // DLC_Deluxe
	"3F49DF54-68B6-44d1-A930-A168628FAA59", // DLC_SP_Maps
	"46EAEFFC-7B1D-443d-BFC8-F825DFEFB094", // DLC_SP_Maps_2
	"4255F5F7-D3AB-4e55-ACEE-4670082040ED", // DLC_SP_Maps_3
	"0E3751A1-F840-4e1b-9706-519BF484E59D", // Expansion
	"6DA07636-4123-4018-B643-6575B4EC336B", // Expansion2
	"8871E748-29A4-4910-8C57-8C99E32D0167", // Shared
	"CBE91AF6-0683-4326-BC77-58BB489E509F", // Tablet
};

struct CvContentPackage
{
	std::string strFile; // relative to the game folder
	std::string strName;
	bool bFiraxis;
	bool bAccepted; // registered and owned, as the EXE decides (IsDLCValid)
	bool bChangesGameplay; // lists a game DLL or holds database files
	std::string strGameplayFile; // first database file found, relative to the package folder

	// worth naming when the database does not match this DLL
	bool IsSuspect() const { return bFiraxis ? !bAccepted : bChangesGameplay; }
};

std::string WideToUtf8(const std::wstring& strWide)
{
	const int iBytes = WideCharToMultiByte(CP_UTF8, 0, strWide.c_str(), -1, NULL, 0, NULL, NULL);
	if (iBytes <= 1)
		return std::string();
	std::vector<char> vBuffer(iBytes);
	WideCharToMultiByte(CP_UTF8, 0, strWide.c_str(), -1, &vBuffer[0], iBytes, NULL, NULL);
	return std::string(&vBuffer[0]);
}

// Text of the first <szTag ...>text< at or after uiFrom (package files are small and flat).
std::string GetTagText(const std::string& strXml, const char* szTag, size_t uiFrom)
{
	const size_t uiTag = strXml.find(std::string("<") + szTag, uiFrom);
	if (uiTag == std::string::npos)
		return std::string();
	const size_t uiStart = strXml.find('>', uiTag);
	if (uiStart == std::string::npos)
		return std::string();
	const size_t uiEnd = strXml.find('<', uiStart + 1);
	if (uiEnd == std::string::npos)
		return std::string();
	return strXml.substr(uiStart + 1, uiEnd - uiStart - 1);
}

bool ReadPackageFile(const std::wstring& strPath, std::string& strXml)
{
	HANDLE hFile = CreateFileW(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	bool bRead = false;
	const DWORD dwSize = GetFileSize(hFile, NULL);
	if (dwSize != INVALID_FILE_SIZE && dwSize > 0 && dwSize < 1024 * 1024)
	{
		strXml.resize(dwSize);
		DWORD dwRead = 0;
		bRead = ReadFile(hFile, &strXml[0], dwSize, &dwRead, NULL) && dwRead == dwSize;
	}
	CloseHandle(hFile);
	return bRead;
}

bool IsFiraxisPackage(const std::string& strGUID)
{
	const size_t uiFirst = strGUID.find_first_not_of(" \t\r\n{");
	const size_t uiLast = strGUID.find_last_not_of(" \t\r\n}");
	if (uiFirst == std::string::npos || uiLast < uiFirst)
		return false;
	const std::string strID = strGUID.substr(uiFirst, uiLast - uiFirst + 1);
	for (size_t i = 0; i < _countof(s_aszFiraxisPackageIDs); i++)
	{
		if (_stricmp(strID.c_str(), s_aszFiraxisPackageIDs[i]) == 0)
			return true;
	}
	return false;
}

// First XML file below strDir that is database data (<GameData> near its start, the test the
// Modpack Maker uses too), relative to strDir; empty if there is none.
std::wstring FindGameDataFile(const std::wstring& strDir, const std::wstring& strRelative, int iDepth)
{
	WIN32_FIND_DATAW kFindData;
	HANDLE hFind = FindFirstFileW((strDir + L"\\*").c_str(), &kFindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return std::wstring();
	std::wstring strFound;
	do
	{
		const std::wstring strEntry = kFindData.cFileName;
		if (strEntry == L"." || strEntry == L"..")
			continue;
		const std::wstring strEntryRelative = strRelative.empty() ? strEntry : strRelative + L"\\" + strEntry;
		if (kFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (iDepth < 16)
				strFound = FindGameDataFile(strDir + L"\\" + strEntry, strEntryRelative, iDepth + 1);
		}
		else if (strEntry.size() > 4 && _wcsicmp(strEntry.c_str() + strEntry.size() - 4, L".xml") == 0)
		{
			HANDLE hFile = CreateFileW((strDir + L"\\" + strEntry).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				char szHead[4096];
				DWORD dwRead = 0;
				if (ReadFile(hFile, szHead, sizeof(szHead) - 1, &dwRead, NULL))
				{
					szHead[dwRead] = '\0';
					if (strstr(szHead, "<GameData") != NULL)
						strFound = strEntryRelative;
				}
				CloseHandle(hFile);
			}
		}
	}
	while (strFound.empty() && FindNextFileW(hFind, &kFindData));
	FindClose(hFind);
	return strFound;
}

void FindContentPackages(const std::wstring& strDir, size_t uiGameDirLength, int iDepth, std::vector<CvContentPackage>& vPackages)
{
	WIN32_FIND_DATAW kFindData;
	HANDLE hFind = FindFirstFileW((strDir + L"\\*").c_str(), &kFindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do
	{
		const std::wstring strEntry = kFindData.cFileName;
		if (strEntry == L"." || strEntry == L"..")
			continue;
		const std::wstring strPath = strDir + L"\\" + strEntry;
		if (kFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// bounded so that a junction loop cannot hang the game
			if (iDepth < 16)
				FindContentPackages(strPath, uiGameDirLength, iDepth + 1, vPackages);
		}
		else if (strEntry.size() > 8 && _wcsicmp(strEntry.c_str() + strEntry.size() - 8, L".civ5pkg") == 0)
		{
			std::string strXml;
			if (!ReadPackageFile(strPath, strXml))
				continue;
			const std::string strGUID = GetTagText(strXml, "GUID", 0);
			GUID kGUID;
			CvContentPackage kPackage;
			kPackage.bFiraxis = IsFiraxisPackage(strGUID);
			kPackage.bAccepted = gDLL && ExtractGUID(strGUID.c_str(), kGUID) && gDLL->IsDLCValid(kGUID);
			kPackage.strFile = WideToUtf8(strPath.substr(uiGameDirLength));
			const size_t uiName = strXml.find("<Name");
			if (uiName != std::string::npos)
				kPackage.strName = GetTagText(strXml, "Value", uiName);
			// Firaxis folders are not searched: theirs is the reference data
			if (!kPackage.bFiraxis)
				kPackage.strGameplayFile = WideToUtf8(FindGameDataFile(strDir, std::wstring(), 0));
			kPackage.bChangesGameplay = !kPackage.strGameplayFile.empty() || strXml.find("<GameCore") != std::string::npos;
			vPackages.push_back(kPackage);
		}
	}
	while (FindNextFileW(hFind, &kFindData));
	FindClose(hFind);
}

// Scanned once per DLL load: the packages cannot change while the game runs.
const std::vector<CvContentPackage>& GetContentPackages()
{
	static std::vector<CvContentPackage> s_vPackages;
	static bool s_bScanned = false;
	if (!s_bScanned)
	{
		s_bScanned = true;
		wchar_t wszExePath[MAX_PATH];
		const DWORD dwLength = GetModuleFileNameW(NULL, wszExePath, MAX_PATH);
		const std::wstring strExePath(wszExePath, dwLength < MAX_PATH ? dwLength : 0);
		const size_t uiSlash = strExePath.find_last_of(L"\\/");
		if (uiSlash != std::wstring::npos)
			FindContentPackages(strExePath.substr(0, uiSlash) + L"\\Assets", uiSlash + 1, 0, s_vPackages);
	}
	return s_vPackages;
}

std::string DescribeSuspectContentPackages()
{
	std::string strList;
	const std::vector<CvContentPackage>& vPackages = GetContentPackages();
	for (std::vector<CvContentPackage>::const_iterator it = vPackages.begin(); it != vPackages.end(); ++it)
	{
		if (!it->IsSuspect())
			continue;
		if (!strList.empty())
			strList += ", ";
		strList += it->strFile;
		if (!it->strName.empty())
			strList += " (" + it->strName + ")";
		if (!it->strGameplayFile.empty())
			strList += " with " + it->strGameplayFile;
	}
	return strList;
}
}

void CvDatabaseUtility::LogContentPackages()
{
	const std::vector<CvContentPackage>& vPackages = GetContentPackages();
	CUSTOMLOG("Content packages below Assets: %u", static_cast<unsigned int>(vPackages.size()));
	for (std::vector<CvContentPackage>::const_iterator it = vPackages.begin(); it != vPackages.end(); ++it)
	{
		if (it->bFiraxis && it->bAccepted)
			continue;
		// CUSTOMLOG is a block, so the branches need braces
		if (it->bFiraxis)
		{
			CUSTOMLOG("  %s - %s: Firaxis, on disk but not accepted by the game (not owned by this account?)", it->strFile.c_str(), it->strName.c_str());
		}
		else if (!it->strGameplayFile.empty())
		{
			CUSTOMLOG("  %s - %s: not made by Firaxis, replaces game database files (%s)", it->strFile.c_str(), it->strName.c_str(), it->strGameplayFile.c_str());
		}
		else
		{
			CUSTOMLOG("  %s - %s: not made by Firaxis, %s", it->strFile.c_str(), it->strName.c_str(), it->bChangesGameplay ? "lists a game DLL" : "no database files");
		}
	}
}
#else
// The scan uses the Win32 file API; elsewhere no package is named.
namespace
{
std::string DescribeSuspectContentPackages()
{
	return std::string();
}
}

void CvDatabaseUtility::LogContentPackages()
{
}
#endif

CvDatabaseUtility::CvDatabaseUtility()
{

}
//------------------------------------------------------------------------------
CvDatabaseUtility::~CvDatabaseUtility()
{
	//Remove any cached statements
	ClearResults();
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::ClearResults()
{
	for(ResultsMap::const_iterator it = m_storedResults.begin(); it != m_storedResults.end(); ++it)
	{
		Database::Results* pResults = it->second;
		delete pResults;
	}

	m_storedResults.clear();
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::ClearResults(const std::string& strKey)
{
	Database::Results* pResults = GetResults(strKey);
	if(pResults)
	{
		delete pResults;
		m_storedResults.erase(strKey);
	}
}
//------------------------------------------------------------------------------
Database::Results* CvDatabaseUtility::GetResults(const std::string& strKey)
{
	ResultsMap::const_iterator it = m_storedResults.find(strKey);
	if(it != m_storedResults.end())
	{
		it->second->Reset();
		return it->second;
	}

	return NULL;
}
//------------------------------------------------------------------------------
Database::Results* CvDatabaseUtility::PrepareResults(const std::string& strKey, const char* szStmt)
{
	Database::Results* pResults = new Database::Results();

	if(DB.Execute(*pResults, szStmt))
	{
		Database::Results* pOldResults = GetResults(strKey);
		if(pOldResults)
			delete pOldResults;

		m_storedResults[strKey] = pResults;
		return pResults;
	}

	delete pResults;

	// A failed prepare here means the gameplay database is missing tables or
	// columns (corrupted install, broken mod, or a package below Assets that
	// sets up an older version of the tables). Most callers dereference the
	// result unchecked, so without this we crash with an unexplained
	// NULL-deref in Bind/Step (#13313). Fail with a diagnostic naming the
	// query instead. Verifying the game files fixes a corrupted install but
	// leaves extra packages in place, so name those first; the query text goes
	// last because the dialog truncates long messages.
	const std::string strPackages = DescribeSuspectContentPackages();
	if (!strPackages.empty())
	{
		PRECONDITION(false, "Failed to prepare database query '%s': %s -- these packages in the game's Assets folder can replace the game data whatever the Mods menu says: %s. Remove an old Vox Populi, a modpack for another version, or a DLC folder this Steam account does not own, then delete the cache folder in My Games\\Sid Meier's Civilization 5. Query: %s", strKey.c_str(), GetErrorMessage(), strPackages.c_str(), szStmt);
	}
	PRECONDITION(false, "Failed to prepare database query '%s' [%s]: %s -- the game database is corrupted or a mod is broken; try verifying game files", strKey.c_str(), szStmt, GetErrorMessage());

	return NULL;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::Initialize2DArray(int**& ppArray, const char* szTable1Name, const char* szTable2Name, int iDefault /* = 0 */)
{
	const int iCount1 = MaxRows(szTable1Name);
	const int iCount2 = MaxRows(szTable2Name);

	if(iCount1 <= 0 || iCount2 <= 0)
	{
		ppArray = NULL;
		ASSERT(false, "Cannot initialize array to 0 size.");
		return false;
	}

	unsigned int iNumBytes = iCount1 * sizeof(int*) + iCount1 * iCount2 * sizeof(int);
	unsigned char* pData = FNEW(unsigned char[iNumBytes], c_eCiv5GameplayDLL, 0);
	ppArray = (int**)pData;
	ppArray[0] = (int*)(pData + iCount1 * sizeof(int*));
	for(int i = 0; i < iCount2; ++i)
	{
		ppArray[0][i] = iDefault;
	}
	for(int i = 1; i < iCount1; i++)
	{
		ppArray[i] = ppArray[i-1] + iCount2;
		for(int j = 0; j < iCount2; ++j)
		{
			ppArray[i][j] = iDefault;
		}
	}

	return true;
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::Initialize2DArray(int**& ppArray, const size_t iCount1, const size_t iCount2, int iDefault /*= 0*/)
{
	if(iCount1 <= 0 || iCount2 <= 0)
	{
		ASSERT(false, "Cannot initialize array to 0 size.");
		return;
	}

	unsigned int iNumBytes = iCount1 * sizeof(int*) + iCount1 * iCount2 * sizeof(int);
	unsigned char* pData = FNEW(unsigned char[iNumBytes], c_eCiv5GameplayDLL, 0);
	ppArray = (int**)pData;
	ppArray[0] = (int*)(pData + iCount1 * sizeof(int*));
	for(unsigned int i = 0; i < iCount2; ++i)
	{
		ppArray[0][i] = iDefault;
	}
	for(unsigned int i = 1; i < iCount1; i++)
	{
		ppArray[i] = ppArray[i-1] + iCount2;
		for(unsigned int j = 0; j < iCount2; ++j)
		{
			ppArray[i][j] = iDefault;
		}
	}
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::SafeDelete2DArray(int**& pArray)
{
	if(pArray != NULL && ReleaseShared2DArray(pArray))
	{
		pArray = NULL;
		return;
	}
	delete[] pArray;
	pArray = NULL;
}
//------------------------------------------------------------------------------
// Shared all-zero 2D tables (Share2DArrayIfZero): one per table shape, reference counted, on read-only pages.
// Loading the database runs on one thread, as do the frees (CvGlobals::deleteInfoArrays).
namespace
{
struct Shared2DArray
{
	size_t iCount1;
	size_t iCount2;
	size_t iRefs;
	int** ppArray;
};
const size_t MAX_SHARED_2D_ARRAYS = 64;
Shared2DArray s_aShared2DArrays[MAX_SHARED_2D_ARRAYS];
size_t s_iNumShared2DArrays = 0;

size_t Size2DArray(size_t iCount1, size_t iCount2)
{
	return iCount1 * sizeof(int*) + iCount1 * iCount2 * sizeof(int);
}

int** CreateShared2DArray(size_t iCount1, size_t iCount2)
{
	// Same layout as Initialize2DArray. VirtualAlloc returns zeroed pages; only the row pointers are written.
	const size_t iNumBytes = Size2DArray(iCount1, iCount2);
	unsigned char* pData = (unsigned char*)VirtualAlloc(NULL, iNumBytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(pData == NULL)
		return NULL;
	int** ppArray = (int**)pData;
	int* pRows = (int*)(pData + iCount1 * sizeof(int*));
	for(size_t i = 0; i < iCount1; ++i)
		ppArray[i] = pRows + i * iCount2;
	DWORD dwOldProtect = 0;
	if(!VirtualProtect(pData, iNumBytes, PAGE_READONLY, &dwOldProtect))
	{
		VirtualFree(pData, 0, MEM_RELEASE);
		return NULL;
	}
	return ppArray;
}
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::IsShared2DArray(const int* const* ppArray)
{
	for(size_t i = 0; i < s_iNumShared2DArrays; ++i)
	{
		if(s_aShared2DArrays[i].ppArray == ppArray)
			return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::ReleaseShared2DArray(const int* const* ppArray)
{
	for(size_t i = 0; i < s_iNumShared2DArrays; ++i)
	{
		Shared2DArray& kShared = s_aShared2DArrays[i];
		if(kShared.ppArray != ppArray)
			continue;

		ASSERT(kShared.iRefs > 0, "Shared 2D array released more often than shared.");
#if defined(VPDEBUG)
		const int* pRows = kShared.ppArray[0];
		for(size_t j = 0; j < kShared.iCount1 * kShared.iCount2; ++j)
		{
			ASSERT(pRows[j] == 0, "Shared 2D array was written.");
		}
#endif
		if(--kShared.iRefs == 0)
		{
			VirtualFree(kShared.ppArray, 0, MEM_RELEASE);
			s_aShared2DArrays[i] = s_aShared2DArrays[--s_iNumShared2DArrays];
		}
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
void CvDatabaseUtility::Share2DArrayIfZero(int**& ppArray)
{
	if(ppArray == NULL || IsShared2DArray(ppArray))
		return;

	// Read the shape back from the Initialize2DArray layout: iCount1 row pointers, then iCount1 rows of iCount2 ints,
	// row i at ppArray[0] + i * iCount2. Anything else is left alone.
	const unsigned char* pBlock = (const unsigned char*)ppArray;
	const unsigned char* pFirstRow = (const unsigned char*)ppArray[0];
	if(pFirstRow <= pBlock || (size_t)(pFirstRow - pBlock) % sizeof(int*) != 0)
		return;
	const size_t iCount1 = (size_t)(pFirstRow - pBlock) / sizeof(int*);
	if(iCount1 < 2)
		return; // the row length cannot be read back from one row; such tables are tiny anyway
	if(ppArray[1] <= ppArray[0])
		return;
	const size_t iCount2 = (size_t)(ppArray[1] - ppArray[0]);
	for(size_t i = 2; i < iCount1; ++i)
	{
		if(ppArray[i] != ppArray[0] + i * iCount2)
			return;
	}

	const int* pRows = ppArray[0];
	for(size_t i = 0; i < iCount1 * iCount2; ++i)
	{
		if(pRows[i] != 0)
			return;
	}

	Shared2DArray* pShared = NULL;
	for(size_t i = 0; i < s_iNumShared2DArrays; ++i)
	{
		if(s_aShared2DArrays[i].iCount1 == iCount1 && s_aShared2DArrays[i].iCount2 == iCount2)
		{
			pShared = &s_aShared2DArrays[i];
			break;
		}
	}
	if(pShared == NULL)
	{
		if(s_iNumShared2DArrays == MAX_SHARED_2D_ARRAYS)
			return; // keep the table's own block
		int** ppNew = CreateShared2DArray(iCount1, iCount2);
		if(ppNew == NULL)
			return; // keep the table's own block
		pShared = &s_aShared2DArrays[s_iNumShared2DArrays++];
		pShared->iCount1 = iCount1;
		pShared->iCount2 = iCount2;
		pShared->iRefs = 0;
		pShared->ppArray = ppNew;
	}

	delete[] ppArray;
	ppArray = pShared->ppArray;
	pShared->iRefs++;
}

//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateArrayByExistence(bool*& pArray, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue, int iMinArraySize)
{
	int iSize = MaxRows(szTypeTableName);
	InitializeArray(pArray, (iSize < iMinArraySize) ? iMinArraySize : iSize, false);

	std::string strKey = "_PABE_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);

	Database::Results* pResults = GetResults(strKey);
	if(pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);

		pResults = PrepareResults(strKey, szSQL);
		if(pResults == NULL)
			return false;
	}

	if(!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}

	while(pResults->Step())
	{
		const int idx = pResults->GetInt(0);
		pArray[idx] = true;
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateArrayByExistence(int*& pArray, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue)
{
	InitializeArray(pArray, MaxRows(szTypeTableName), -1);

	std::string strKey = "_PABE_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);

	Database::Results* pResults = GetResults(strKey);
	if(pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);
		pResults = PrepareResults(strKey, szSQL);
		if(pResults == NULL)
			return false;
	}

	if(!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}

	int idx = 0;
	while(pResults->Step())
	{
		pArray[idx++] = pResults->GetInt(0);
	}

	pResults->Reset();

	return true;
}

//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateVector(std::vector<int>& pVector, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue)
{
	pVector.clear();

	std::string strKey = "_PV_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);

	Database::Results* pResults = GetResults(strKey);
	if (pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);
		pResults = PrepareResults(strKey, szSQL);
		if (pResults == NULL)
			return false;
	}

	if (!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}

	while (pResults->Step())
	{
		pVector.push_back(pResults->GetInt(0));
	}

	pResults->Reset();

	return true;
}

bool CvDatabaseUtility::PopulateVectorByValue(vector<int>& pVector, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue, const char* szValueColumn, int iVectorSize, int iDefaultValue, const char* szAdditionalCondition)
{
	int iSize = (iVectorSize > 0) ? iVectorSize : MaxRows(szTypeTableName);
	pVector.resize(iSize, iDefaultValue);

	string strKey = "_PVBV_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);
	strKey.append(szValueColumn);
	strKey.append(szAdditionalCondition);

	Database::Results* pResults = GetResults(strKey);
	if (!pResults)
	{
		string strAddedClause = "";
		if (szAdditionalCondition && szAdditionalCondition[0] != '\0')
		{
			strAddedClause.append(" and ");
			strAddedClause.append(szAdditionalCondition);
		}
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID, %s from %s inner join %s on %s = %s.Type where %s = ?%s",
			szTypeTableName, szValueColumn, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn, strAddedClause.c_str());
		pResults = PrepareResults(strKey, szSQL);
		if (!pResults)
			return false;
	}

	if (!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}

	while (pResults->Step())
	{
		const int idx = pResults->GetInt(0);
		const int value = pResults->GetInt(1);
		pVector[idx] = value;
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateArrayByValue(int*& pArray, const char* szTypeTableName, const char* szDataTableName, const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue, const char* szValueColumn, int iDefaultValue /* = 0 */, int iMinArraySize /* = 0 */, const char* szAdditionalCondition /* = "" */)
{
	int iSize = MaxRows(szTypeTableName);
	InitializeArray(pArray, (iSize<iMinArraySize)?iMinArraySize:iSize, iDefaultValue);

	std::string strKey = "_PABV_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);
	strKey.append(szValueColumn);
	strKey.append(szAdditionalCondition);

	Database::Results* pResults = GetResults(strKey);
	if(pResults == NULL)
	{
		std::string strAddedClause = "";
		if (szAdditionalCondition && szAdditionalCondition[0] != '\0')
		{
			strAddedClause.append(" and ");
			strAddedClause.append(szAdditionalCondition);
		}
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID, %s from %s inner join %s on %s = %s.Type where %s = ?%s", szTypeTableName, szValueColumn, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn, strAddedClause.c_str());
		pResults = PrepareResults(strKey, szSQL);
		if(pResults == NULL)
			return false;
	}

	if(!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}
	while(pResults->Step())
	{
		const int idx = pResults->GetInt(0);
		const int value = pResults->GetInt(1);
		pArray[idx] = value;
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::PopulateSetByExistence(set<int>& siData, const char* szTypeTableName, const char* szDataTableName,
	const char* szTypeColumn, const char* szFilterColumn, const char* szFilterValue)
{
	siData.clear();

	string strKey = "_PSBE_";
	strKey.append(szTypeTableName);
	strKey.append(szDataTableName);
	strKey.append(szFilterColumn);

	Database::Results* pResults = GetResults(strKey);
	if (!pResults)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select %s.ID from %s inner join %s on %s = %s.Type where %s = ?", szTypeTableName, szDataTableName, szTypeTableName, szTypeColumn, szTypeTableName, szFilterColumn);
		pResults = PrepareResults(strKey, szSQL);
		if (!pResults)
			return false;
	}

	if (!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}

	while (pResults->Step())
	{
		siData.insert(pResults->GetInt(0));
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::SetFlavors(int*& pFlavorsArray,
                                   const char* szTableName,
                                   const char* szFilterColumn,
                                   const char* szFilterValue,
                                   int iDefaultValue)
{
	return PopulateArrayByValue(pFlavorsArray, "Flavors", szTableName,
	                            "FlavorType", szFilterColumn, szFilterValue, "Flavor", iDefaultValue);
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::SetYields(int*& pYieldsArray,
                                  const char* szTableName,
                                  const char* szFilterColumn,
                                  const char* szFilterValue,
	                              const char* szAdditionalCondition)
{
	return PopulateArrayByValue(pYieldsArray, "Yields", szTableName,
	                            "YieldType", szFilterColumn, szFilterValue, "Yield", 0, 0, szAdditionalCondition);
}
//------------------------------------------------------------------------------
bool CvDatabaseUtility::SetFractionYields(fraction*& pYieldsArray,
	const char* szTableName,
	const char* szFilterColumn,
	const char* szFilterValue,
	const char* szDenominatorColumn)
{
	InitializeArray(pYieldsArray, "Yields");

	std::string strKey = "_SFY_";
	strKey.append(szTableName);
	strKey.append(szFilterColumn);
	strKey.append(szFilterValue);
	strKey.append(szDenominatorColumn);

	Database::Results* pResults = GetResults(strKey);
	if (pResults == NULL)
	{
		char szSQL[512];
		sprintf_s(szSQL, "select Yields.ID, Yield, %s from %s inner join Yields on Yields.Type = YieldType where %s = ?", szDenominatorColumn, szTableName, szFilterColumn);
		pResults = PrepareResults(strKey, szSQL);
		
		if (pResults == NULL)
			return false;
	}

	if (!pResults->Bind(1, szFilterValue, false))
	{
		ASSERT(false, GetErrorMessage());
		return false;
	}

	while (pResults->Step())
	{
		const int YieldID = pResults->GetInt(0);
		const int yield = pResults->GetInt(1);
		const int iNumRequired = pResults->GetInt(2);
		PRECONDITION(iNumRequired != 0, "%s mustn't be 0 in table %s", szDenominatorColumn, szTableName);

		pYieldsArray[YieldID] = pYieldsArray[YieldID] + fraction(yield, iNumRequired);
	}

	pResults->Reset();

	return true;
}
//------------------------------------------------------------------------------
int CvDatabaseUtility::MaxRows(const char* szTableName)
{
	char szSQL[256] = {0};
	sprintf_s(szSQL, "SELECT max(rowid) from %s", szTableName);
	Database::Results kResults;
	int maxValue = 0;
	if(DB.Execute(kResults, szSQL))
	{
		if(kResults.Step())
		{
			//Since some rowid's start at 0 in Civ, let's increase this # by 1.
			maxValue = kResults.GetInt(0) + 1;
		}
	}

	return maxValue;
}
//------------------------------------------------------------------------------
const char* CvDatabaseUtility::GetErrorMessage() const
{
	return DB.ErrorMessage();
}
//------------------------------------------------------------------------------
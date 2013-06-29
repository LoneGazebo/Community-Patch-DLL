
#pragma once

//------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------
#define DLLCALL __stdcall

/*
	Some guidelines...

	Design Guidelines:
	* Must derive from ICvUnknown either directly or indirectly.
	* Must have a GUID defined in the global scope.
	* Must contain an inline function GetInterfaceId() that returns the GUID.
	* Must not change after being defined.
	* All functions must be declared with DLLCALL as their calling convention.
	* Functions that return interface instances must be of type ICvUnknown* or version 1 of the interface type.

	Naming Conventions:
	* Interfaces must be prefixed with ICv
	* Interfaces other than ICvUnknown must be post-fixed with a version number starting at 1.
	* Globally scoped GUIDs that relate to specific interfaces must be named guid<InterfaceName>.

	Usage Guidelines
	* Interface instances must be explicitly deleted in order to be correctly released by the DLL.
	* It's safe and encouraged to use interface instances inside of auto_ptr and shared_ptr types.

	Implementation Guidelines:
	* QueryInterface must support all interfaces that the implementation derives from (including ICvUnknown).
	* New instances may be returned as a result of a QueryInterface call.
	* NULL must be returned if QueryInterface does not support the interface.
	* A single implementation may implement multiple interfaces.
	* It's the implementer's responsibility that interface instances are cleaned up properly via Destroy().
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Please note that the interfaces in the file should be considered FROZEN.
	These interfaces have been made public and any mods will be depending on these not changing.
*/
//////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// GUIDs
//------------------------------------------------------------------------------

// {D89BA82F-9FA3-4696-B3F4-52BDB101CFB2}
static const GUID guidICvUnknown =
{0xd89ba82f, 0x9fa3, 0x4696, 0xb3, 0xf4, 0x52, 0xbd, 0xb1, 0x1, 0xcf, 0xb2};

// {579A12E9-3C70-4276-8460-C941A5F9585F}
static const GUID guidICvEnumerator = 
{ 0x579a12e9, 0x3c70, 0x4276, { 0x84, 0x60, 0xc9, 0x41, 0xa5, 0xf9, 0x58, 0x5f } };

// {AE87F7BD-8510-444f-9D54-983D776485A6}
static const GUID guidICvDLLDatabaseUtility1 = 
{ 0xae87f7bd, 0x8510, 0x444f, { 0x9d, 0x54, 0x98, 0x3d, 0x77, 0x64, 0x85, 0xa6 } };

// {C004434C-878E-4bcd-BAB2-4CEE341D711B}
static const GUID guidICvScriptSystemUtility1 = 
{ 0xc004434c, 0x878e, 0x4bcd, { 0xba, 0xb2, 0x4c, 0xee, 0x34, 0x1d, 0x71, 0x1b } };

// {A309FA38-CF60-4239-A162-8586C0D1C7D3}
static const GUID guidICvGameContext1 =
{ 0xa309fa38, 0xcf60, 0x4239, 0xa1, 0x62, 0x85, 0x86, 0xc0, 0xd1, 0xc7, 0xd3};

// {E75BA944-05DB-4D6C-96A6-A07B71CDBE77}
static const GUID guidICvCity1 =
{ 0xe75ba944, 0x5db, 0x4d6c, { 0x96, 0xa6, 0xa0, 0x7b, 0x71, 0xcd, 0xbe, 0x77 } };

// {DA81A0DC-30B3-4773-8786-35D470E0EF64}
static const GUID guidICvCombatInfo1 = 
{ 0xda81a0dc, 0x30b3, 0x4773, { 0x87, 0x86, 0x35, 0xd4, 0x70, 0xe0, 0xef, 0x64 } };

// {61CA9E15-59FA-4980-AD88-27A71AC425A5}
static const GUID guidICvDeal1 = 
{ 0x61ca9e15, 0x59fa, 0x4980, { 0xad, 0x88, 0x27, 0xa7, 0x1a, 0xc4, 0x25, 0xa5 } };

// {33320CC0-47FD-4b67-A024-FFF3AA4F66C1}
static const GUID guidICvDealAI1 = 
{ 0x33320cc0, 0x47fd, 0x4b67, { 0xa0, 0x24, 0xff, 0xf3, 0xaa, 0x4f, 0x66, 0xc1 } };

// {5215A1C1-5649-46a7-A0D2-5819536A62F1}
static const GUID guidICvDiplomacyAI1 = 
{ 0x5215a1c1, 0x5649, 0x46a7, { 0xa0, 0xd2, 0x58, 0x19, 0x53, 0x6a, 0x62, 0xf1 } };

// {DC39D22C-12AE-4395-8A20-EEF145A77DD0}
static const GUID guidICvGame1 = 
{ 0xdc39d22c, 0x12ae, 0x4395, { 0x8a, 0x20, 0xee, 0xf1, 0x45, 0xa7, 0x7d, 0xd0 } };

// {637E8AEC-5B31-49d5-9814-76DCE305A29B}
static const GUID guidICvGameAsynch1 = 
{ 0x637e8aec, 0x5b31, 0x49d5, { 0x98, 0x14, 0x76, 0xdc, 0xe3, 0x5, 0xa2, 0x9b } };

// {8B71D2B1-3673-41a2-B663-C67C02E16F57}
static const GUID guidICvMap1 = 
{ 0x8b71d2b1, 0x3673, 0x41a2, { 0xb6, 0x63, 0xc6, 0x7c, 0x2, 0xe1, 0x6f, 0x57 } };

// {D97F1045-472F-48e7-8C61-5993DECC2677}
static const GUID guidICvMissionData1 = 
{ 0xd97f1045, 0x472f, 0x48e7, { 0x8c, 0x61, 0x59, 0x93, 0xde, 0xcc, 0x26, 0x77 } };

// {55B80DA7-D175-4ea7-B555-B98845DDDC8E}
static const GUID guidICvNetMessageHandler1 = 
{ 0x55b80da7, 0xd175, 0x4ea7, { 0xb5, 0x55, 0xb9, 0x88, 0x45, 0xdd, 0xdc, 0x8e } };

// {A8D76617-571D-40a5-B82C-FF0C51497C13}
static const GUID guidICvNetMessageHandler2 = 
{ 0xa8d76617, 0x571d, 0x40a5, { 0xb8, 0x2c, 0xff, 0xc, 0x51, 0x49, 0x7c, 0x13 } };

// {113A2A5B-907F-4a99-9756-9B76425D161D}
static const GUID guidICvNetworkSyncronization1 = 
{ 0x113a2a5b, 0x907f, 0x4a99, { 0x97, 0x56, 0x9b, 0x76, 0x42, 0x5d, 0x16, 0x1d } };

// {AE4C09EB-A092-45EB-9CA7-F4A5D2F88602}
static const GUID guidICvPlayer1 =
{ 0xae4c09eb, 0xa092, 0x45eb, { 0x9c, 0xa7, 0xf4, 0xa5, 0xd2, 0xf8, 0x86, 0x2 } };

// {611993FC-2A77-44ee-82AF-28AE38380803}
static const GUID guidICvPlot1 =
{ 0x611993fc, 0x2a77, 0x44ee, { 0x82, 0xaf, 0x28, 0xae, 0x38, 0x38, 0x8, 0x3 } };

// {C826212A-5D8B-46ea-B326-CB57C75B9578}
static const GUID guidICvPreGame1 = 
{ 0xc826212a, 0x5d8b, 0x46ea, { 0xb3, 0x26, 0xcb, 0x57, 0xc7, 0x5b, 0x95, 0x78 } };

// {2331CBF1-AD80-4c4a-A614-F377444529D9}
static const GUID guidICvRandom1 = 
{ 0x2331cbf1, 0xad80, 0x4c4a, { 0xa6, 0x14, 0xf3, 0x77, 0x44, 0x45, 0x29, 0xd9 } };

// {AC702E9C-8EF0-45de-9951-DDFE8BCC52CD}
static const GUID guidICvTeam1 = 
{ 0xac702e9c, 0x8ef0, 0x45de, { 0x99, 0x51, 0xdd, 0xfe, 0x8b, 0xcc, 0x52, 0xcd } };

// {9F157E04-4B2D-4797-9AA5-1325DE221607}
static const GUID guidICvUnit1 =
{0x9f157e04, 0x4b2d, 0x4797, 0x9a, 0xa5, 0x13, 0x25, 0xde, 0x22, 0x16, 0x7};

// {78F0497A-7A44-49a0-8D14-58AAB8C128B8}
static const GUID guidICvUnitInfo1 = 
{ 0x78f0497a, 0x7a44, 0x49a0, { 0x8d, 0x14, 0x58, 0xaa, 0xb8, 0xc1, 0x28, 0xb8 } };

// {F2DFB50A-AEDA-47cd-BF02-177A67944A74}
static const GUID guidICvBuildingInfo1 = 
{ 0xf2dfb50a, 0xaeda, 0x47cd, { 0xbf, 0x2, 0x17, 0x7a, 0x67, 0x94, 0x4a, 0x74 } };

// {944BCC2F-0BC6-4345-BCBD-82383470F32E}
static const GUID guidICvImprovementInfo1 = 
{ 0x944bcc2f, 0xbc6, 0x4345, { 0xbc, 0xbd, 0x82, 0x38, 0x34, 0x70, 0xf3, 0x2e } };

// {E0B39559-B03D-4338-BBE3-24C08DADD54E}
static const GUID guidICvTechInfo1 = 
{ 0xe0b39559, 0xb03d, 0x4338, { 0xbb, 0xe3, 0x24, 0xc0, 0x8d, 0xad, 0xd5, 0x4e } };

// {D347C353-2BA5-4da7-881F-46A5F860B74A}
static const GUID guidICvBuildInfo1 = 
{ 0xd347c353, 0x2ba5, 0x4da7, { 0x88, 0x1f, 0x46, 0xa5, 0xf8, 0x60, 0xb7, 0x4a } };

// {49C59819-A545-4c34-9328-CA3ACED932B8}
static const GUID guidICvCivilizationInfo1 = 
{ 0x49c59819, 0xa545, 0x4c34, { 0x93, 0x28, 0xca, 0x3a, 0xce, 0xd9, 0x32, 0xb8 } };

// {22CCAA13-6102-44e0-B1F6-94EE76A23BC1}
static const GUID guidICvColorInfo1 = 
{ 0x22ccaa13, 0x6102, 0x44e0, { 0xb1, 0xf6, 0x94, 0xee, 0x76, 0xa2, 0x3b, 0xc1 } };

// {1A87B286-5001-4661-9A89-8126AF132702}
static const GUID guidICvDlcPackageInfo1 = 
{ 0x1a87b286, 0x5001, 0x4661, { 0x9a, 0x89, 0x81, 0x26, 0xaf, 0x13, 0x27, 0x2 } };

// {FDA260CE-A5D3-4191-835F-1B99B360291D}
static const GUID guidICvEraInfo1 = 
{ 0xfda260ce, 0xa5d3, 0x4191, { 0x83, 0x5f, 0x1b, 0x99, 0xb3, 0x60, 0x29, 0x1d } };

// {CCA94937-D8F5-4755-BC1D-4DAE0DD71008}
static const GUID guidICvFeatureInfo1 = 
{ 0xcca94937, 0xd8f5, 0x4755, { 0xbc, 0x1d, 0x4d, 0xae, 0xd, 0xd7, 0x10, 0x8 } };

// {4E021163-B771-4cfb-98AA-4F7AD9BCA37E}
static const GUID guidICvGameDeals1 = 
{ 0x4e021163, 0xb771, 0x4cfb, { 0x98, 0xaa, 0x4f, 0x7a, 0xd9, 0xbc, 0xa3, 0x7e } };

// {BAAC0587-5B40-4611-A75A-190E3CB86F79}
static const GUID guidICvGameOptionInfo1 = 
{ 0xbaac0587, 0x5b40, 0x4611, { 0xa7, 0x5a, 0x19, 0xe, 0x3c, 0xb8, 0x6f, 0x79 } };

// {F1319991-D469-4202-9E95-C3AE01AE5646}
static const GUID guidICvGameSpeedInfo1 = 
{ 0xf1319991, 0xd469, 0x4202, { 0x9e, 0x95, 0xc3, 0xae, 0x1, 0xae, 0x56, 0x46 } };

// {7597D310-0517-4e27-80B3-CEA862385AC1}
static const GUID guidICvHandicapInfo1 = 
{ 0x7597d310, 0x517, 0x4e27, { 0x80, 0xb3, 0xce, 0xa8, 0x62, 0x38, 0x5a, 0xc1 } };

// {154AEB0B-7D94-4077-B711-352C3B1EDB88}
static const GUID guidICvInterfaceModeInfo1 = 
{ 0x154aeb0b, 0x7d94, 0x4077, { 0xb7, 0x11, 0x35, 0x2c, 0x3b, 0x1e, 0xdb, 0x88 } };

// {0E264A94-8802-4062-A0AD-0364D9DD29A3}
static const GUID guidICvLeaderHeadInfo1 = 
{ 0xe264a94, 0x8802, 0x4062, { 0xa0, 0xad, 0x3, 0x64, 0xd9, 0xdd, 0x29, 0xa3 } };

// {0DDF7A39-4801-47ce-8D9C-538ED9C4A479}
static const GUID guidICvMinorCivInfo1 = 
{ 0xddf7a39, 0x4801, 0x47ce, { 0x8d, 0x9c, 0x53, 0x8e, 0xd9, 0xc4, 0xa4, 0x79 } };

// {F3328352-CCFA-4d41-853F-D9E374D06013}
static const GUID guidICvMissionInfo1 = 
{ 0xf3328352, 0xccfa, 0x4d41, { 0x85, 0x3f, 0xd9, 0xe3, 0x74, 0xd0, 0x60, 0x13 } };

// {7917C933-2EDC-49af-B7F3-C065CBB5EB2C}
static const GUID guidICvNetInitInfo1 = 
{ 0x7917c933, 0x2edc, 0x49af, { 0xb7, 0xf3, 0xc0, 0x65, 0xcb, 0xb5, 0xeb, 0x2c } };

// {47189636-A5D4-4d0b-AA79-F970656F14EB}
static const GUID guidICvNetLoadGameInfo1 = 
{ 0x47189636, 0xa5d4, 0x4d0b, { 0xaa, 0x79, 0xf9, 0x70, 0x65, 0x6f, 0x14, 0xeb } };

// {1ED0AE82-0B7A-431e-AD52-E0EE05F8A56A}
static const GUID guidICvPlayerColorInfo1 = 
{ 0x1ed0ae82, 0xb7a, 0x431e, { 0xad, 0x52, 0xe0, 0xee, 0x5, 0xf8, 0xa5, 0x6a } };

// {8C315274-E9A7-4210-9B2E-0AF13C43C0B6}
static const GUID guidICvPlayerOptionInfo1 = 
{ 0x8c315274, 0xe9a7, 0x4210, { 0x9b, 0x2e, 0xa, 0xf1, 0x3c, 0x43, 0xc0, 0xb6 } };

// {79C1DABA-9D1D-421e-83CE-83F581582619}
static const GUID guidICvPolicyInfo1 = 
{ 0x79c1daba, 0x9d1d, 0x421e, { 0x83, 0xce, 0x83, 0xf5, 0x81, 0x58, 0x26, 0x19 } };

// {B1AC8C62-11CF-4ac9-8633-B387EBF9E0DA}
static const GUID guidICvPromotionInfo1 = 
{ 0xb1ac8c62, 0x11cf, 0x4ac9, { 0x86, 0x33, 0xb3, 0x87, 0xeb, 0xf9, 0xe0, 0xda } };

// {20415753-638D-4a6d-922F-8A9D11F6008E}
static const GUID guidICvResourceInfo1 = 
{ 0x20415753, 0x638d, 0x4a6d, { 0x92, 0x2f, 0x8a, 0x9d, 0x11, 0xf6, 0x0, 0x8e } };

// {0EDE50AB-37D4-4272-BDAF-0B25E2A05FAD}
static const GUID guidICvTerrainInfo1 = 
{ 0xede50ab, 0x37d4, 0x4272, { 0xbd, 0xaf, 0xb, 0x25, 0xe2, 0xa0, 0x5f, 0xad } };

// {53507549-1614-428e-899F-25FFAC803938}
static const GUID guidICvWorldInfo1 = 
{ 0x53507549, 0x1614, 0x428e, { 0x89, 0x9f, 0x25, 0xff, 0xac, 0x80, 0x39, 0x38 } };

// {AFEFDF63-DB01-412a-B370-162DBB452F0C}
static const GUID guidICvVictoryInfo1 = 
{ 0xafefdf63, 0xdb01, 0x412a, { 0xb3, 0x70, 0x16, 0x2d, 0xbb, 0x45, 0x2f, 0xc } };

// {297098FA-8CD6-4828-A885-0E1D58AFFDD4}
static const GUID guidICvUnitCombatClassInfo1 = 
{ 0x297098fa, 0x8cd6, 0x4828, { 0xa8, 0x85, 0xe, 0x1d, 0x58, 0xaf, 0xfd, 0xd4 } };

// {39474804-14F7-44e3-B551-A2498DCAD11C}
static const GUID guidICvWorldBuilderMapLoader1 = 
{ 0x39474804, 0x14f7, 0x44e3, { 0xb5, 0x51, 0xa2, 0x49, 0x8d, 0xca, 0xd1, 0x1c } };

// {D13F7E42-8644-4e60-8483-3A3EB8F0E8C1}
static const GUID guidICvPathFinderUpdate1 = 
{ 0xd13f7e42, 0x8644, 0x4e60, { 0x84, 0x83, 0x3a, 0x3e, 0xb8, 0xf0, 0xe8, 0xc1 } };


//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
struct lua_State;

class FAutoArchive;
class FIGameIniParser;

class ICvBuildingInfo1;
class ICvCity1;
class ICvCombatInfo1;
class ICvDLLDatabaseUtility1;
class ICvDeal1;
class ICvDlcPackageInfo1;
class ICvEngineUtility1;
class ICvGame1;
class ICvGameAsynch1;
class ICvImprovementInfo1;
class ICvMap1;
class ICvNetMessageHandler1;
class ICvNetworkSyncronization1;
class ICvNetInitInfo1;
class ICvNetLoadGameInfo1;
class ICvPlayer1;
class ICvPlot1;
class ICvPreGame1;
class ICvRandom1;
class ICvScriptSystemUtility1;
class ICvTeam1;
class ICvTechInfo1;
class ICvUnit1;
class ICvUnitInfo1;
class ICvWorldBuilderMapLoader1;
class ICvPathFinderUpdate1;

//------------------------------------------------------------------------------
// Base Interfaces
//------------------------------------------------------------------------------
class ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId(){ return guidICvUnknown; }

	void DLLCALL operator delete(void* p)
	{
		if (p)
		{
			ICvUnknown* inst = (ICvUnknown*)(p);
			inst->Destroy();
		}
	}

	virtual void* DLLCALL QueryInterface(GUID guidInterface) = 0;

 	template<typename T>
 	T* DLLCALL QueryInterface()
 	{
 		return static_cast<T*>(QueryInterface(T::GetInterfaceId()));
 	}

protected:
	virtual void DLLCALL Destroy() = 0;
};

class ICvEnumerator : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId(){ return guidICvEnumerator; }

	virtual bool DLLCALL MoveNext() = 0;
	virtual void DLLCALL Reset() = 0;

	virtual ICvUnknown* DLLCALL GetCurrent() = 0;
};

class ICvBuildInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvBuildInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual int DLLCALL GetImprovement() = 0;
	virtual int DLLCALL GetRoute() = 0;
};

class ICvCivilizationInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvCivilizationInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual bool DLLCALL IsAIPlayable() = 0;
	virtual bool DLLCALL IsPlayable() = 0;
	virtual const char* DLLCALL GetShortDescription() = 0;
	virtual int DLLCALL GetDefaultPlayerColor() = 0;
	virtual int DLLCALL GetArtStyleType() = 0;
	virtual int DLLCALL GetNumCityNames() = 0;
	virtual const char* DLLCALL GetArtStylePrefix() = 0;
	virtual const char* DLLCALL GetArtStyleSuffix() = 0;
	virtual const char* DLLCALL GetDawnOfManAudio() = 0;
	virtual int DLLCALL GetCivilizationBuildings(int i) = 0;
	virtual bool DLLCALL IsLeaders(int i) = 0;
	virtual const char* DLLCALL GetCityNames(int i) = 0;
	virtual const char* DLLCALL GetSoundtrackKey() = 0;
};

class ICvColorInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvColorInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const CvColorA& DLLCALL GetColor() = 0;
};

class ICvDlcPackageInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvDlcPackageInfo1; }

	virtual GUID DLLCALL GetPackageID() = 0;
};

class ICvEraInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvEraInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual int DLLCALL GetNumEraVOs() = 0;
	virtual const char* DLLCALL GetEraVO(int iIndex) = 0;
	virtual const char* DLLCALL GetArtPrefix() = 0;
};

class ICvFeatureInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvFeatureInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual bool DLLCALL IsNoCoast() = 0;
	virtual bool DLLCALL IsNoRiver() = 0;
	virtual bool DLLCALL IsNoAdjacent() = 0;
	virtual bool DLLCALL IsRequiresFlatlands() = 0;
	virtual bool DLLCALL IsRequiresRiver() = 0;
	virtual bool DLLCALL IsNaturalWonder() = 0;
	virtual const char* DLLCALL GetArtDefineTag() = 0;
	virtual int DLLCALL GetWorldSoundscapeScriptId() = 0;
	virtual const char* DLLCALL GetEffectTypeTag() = 0;
	virtual bool DLLCALL IsTerrain(int i) = 0;
};

class ICvGameDeals1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGameDeals1; }

	virtual void DLLCALL AddProposedDeal(ICvDeal1* pDeal) = 0;
	virtual bool DLLCALL FinalizeDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted) = 0;
	
	virtual ICvDeal1* DLLCALL GetTempDeal() = 0;
	virtual void DLLCALL SetTempDeal(ICvDeal1* pDeal) = 0;

	virtual PlayerTypes DLLCALL HasMadeProposal(PlayerTypes eFromPlayer) = 0;
	virtual bool DLLCALL ProposedDealExists(PlayerTypes eFromPlayer, PlayerTypes eToPlayer) = 0;
	virtual ICvDeal1* DLLCALL GetProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer) = 0; 

	virtual ICvDeal1* DLLCALL GetCurrentDeal(PlayerTypes ePlayer, unsigned int index) = 0; 
	virtual ICvDeal1* DLLCALL GetHistoricDeal(PlayerTypes ePlayer, unsigned int indx) = 0; 
	virtual unsigned int DLLCALL GetNumCurrentDeals(PlayerTypes ePlayer) = 0; 
	virtual unsigned int DLLCALL GetNumHistoricDeals(PlayerTypes ePlayer) = 0; 

	virtual unsigned int DLLCALL CreateDeal() = 0;
	virtual ICvDeal1* DLLCALL GetDeal(unsigned int index) = 0;
	virtual void DLLCALL DestroyDeal(unsigned int index) = 0;
};

class ICvGameOptionInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGameOptionInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual bool DLLCALL GetDefault() = 0;
};

class ICvGameSpeedInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGameSpeedInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual int DLLCALL GetBarbPercent() = 0;
};

class ICvHandicapInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvHandicapInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual int DLLCALL GetBarbSpawnMod() = 0;
};

class ICvInterfaceModeInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvInterfaceModeInfo1; }

	virtual int DLLCALL GetMissionType() = 0;
};

class ICvLeaderHeadInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvLeaderHeadInfo1; }

	virtual const char* DLLCALL GetDescription() = 0;
	virtual const char* DLLCALL GetArtDefineTag() = 0;
};

class ICvMinorCivInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvMinorCivInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual int DLLCALL GetDefaultPlayerColor() const = 0;
	virtual int DLLCALL GetArtStyleType() const = 0;
	virtual int DLLCALL GetNumCityNames() const = 0;
	virtual const char* DLLCALL GetArtStylePrefix() const = 0;				
	virtual const char* DLLCALL GetArtStyleSuffix() const = 0;
	virtual const char* DLLCALL GetCityNames(int i) const = 0;
};

class ICvMissionInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvMissionInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
};

class ICvNetInitInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvNetInitInfo1; }

	virtual const char* DLLCALL GetDebugString() = 0;
	virtual bool DLLCALL Read(FDataStream& kStream) = 0;
	virtual bool DLLCALL Write(FDataStream& kStream) = 0;
	virtual bool DLLCALL Commit() = 0;
};

class ICvNetLoadGameInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvNetLoadGameInfo1; }

	virtual bool DLLCALL Read(FDataStream& kStream) = 0;
	virtual bool DLLCALL Write(FDataStream& kStream) = 0;
	virtual bool DLLCALL Commit() = 0;
};

class ICvPlayerColorInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPlayerColorInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual ColorTypes DLLCALL GetColorTypePrimary() = 0;
	virtual ColorTypes DLLCALL GetColorTypeSecondary() = 0;
};

class ICvPlayerOptionInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPlayerOptionInfo1; }

	virtual bool DLLCALL GetDefault() = 0;
};

class ICvPolicyInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPolicyInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
};

class ICvPromotionInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPromotionInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
};

class ICvResourceInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvResourceInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual int DLLCALL GetResourceClassType() = 0;
	virtual ResourceUsageTypes DLLCALL GetResourceUsage() = 0;
	virtual const char* DLLCALL GetIconString() = 0;
	virtual const char* DLLCALL GetArtDefineTag() = 0;
	virtual const char* DLLCALL GetArtDefineTagHeavy() = 0;
	virtual const char* DLLCALL GetAltArtDefineTag() = 0;
	virtual const char* DLLCALL GetAltArtDefineTagHeavy() = 0;
	virtual bool DLLCALL IsTerrain(int i) = 0;
	virtual bool DLLCALL IsFeature(int i) = 0;
	virtual bool DLLCALL IsFeatureTerrain(int i) = 0;
};

class ICvTerrainInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvTerrainInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
	virtual bool DLLCALL IsWater() = 0;
	virtual const char* DLLCALL GetArtDefineTag() = 0;
	virtual int DLLCALL GetWorldSoundscapeScriptId() = 0;
	virtual const char* DLLCALL GetEffectTypeTag() = 0;
};

class ICvUnitCombatClassInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvUnitCombatClassInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
};

class ICvVictoryInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvVictoryInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescription() = 0;
};

class ICvWorldInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvWorldInfo1; }

	virtual const char* DLLCALL GetType() = 0;
	virtual const char* DLLCALL GetDescriptionKey() = 0;
	virtual int DLLCALL GetDefaultPlayers() = 0;
	virtual int DLLCALL GetDefaultMinorCivs() = 0;
};

class ICvGameContext1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGameContext1; }
	
	virtual GUID DLLCALL GetDLLGUID() = 0;
	virtual const char * DLLCALL GetDLLVersion() = 0;

	virtual ICvNetworkSyncronization1* DLLCALL GetNetworkSyncronizer() = 0;
	virtual ICvNetMessageHandler1* DLLCALL GetNetMessageHandler() = 0;
	virtual ICvWorldBuilderMapLoader1* DLLCALL GetWorldBuilderMapLoader() = 0;
	virtual ICvPreGame1* DLLCALL GetPreGame() = 0;
	
	virtual ICvGame1* DLLCALL GetGame() = 0;
	virtual ICvGameAsynch1* DLLCALL GetGameAsynch() = 0;
	virtual ICvMap1* DLLCALL GetMap() = 0;
	virtual ICvTeam1* DLLCALL GetTeam(TeamTypes eTeam) = 0;

	// Infos Accessors
	virtual int DLLCALL GetInfoTypeForString(const char* szType, bool hideAssert = false) const = 0;
	virtual int DLLCALL GetInfoTypeForHash(uint uiHash, bool hideAssert = false) const = 0;
	
	virtual CivilizationTypes DLLCALL GetCivilizationInfoIndex(const char* pszType) = 0;

	virtual int DLLCALL GetNumPlayableCivilizationInfos() = 0;
	virtual int DLLCALL GetNumAIPlayableCivilizationInfos() = 0;
	virtual int DLLCALL GetNumPlayableMinorCivs() = 0;

	virtual int DLLCALL GetNumBuildInfos() = 0;
	virtual int DLLCALL GetNumBuildingInfos() = 0;
	virtual int DLLCALL GetNumCivilizationInfos() = 0;
	virtual int DLLCALL GetNumClimateInfos() = 0;
	virtual int DLLCALL GetNumColorInfos() = 0;
	virtual int DLLCALL GetNumEraInfos() = 0;
	virtual int DLLCALL GetNumFeatureInfos() = 0;
	virtual int DLLCALL GetNumGameOptionInfos() = 0;
	virtual int DLLCALL GetNumGameSpeedInfos() = 0;
	virtual int DLLCALL GetNumHandicapInfos() = 0;
	virtual int DLLCALL GetNumImprovementInfos() = 0;
	virtual int DLLCALL GetNumLeaderHeadInfos() = 0;
	virtual int DLLCALL GetNumMinorCivInfos() = 0;
	virtual int DLLCALL GetNumPlayerColorInfos() = 0;
	virtual int DLLCALL GetNumPolicyInfos() = 0;
	virtual int DLLCALL GetNumPromotionInfos() = 0;
	virtual int DLLCALL GetNumResourceInfos() = 0;
	virtual int DLLCALL GetNumSeaLevelInfos() = 0;
	virtual int DLLCALL GetNumTechInfos() = 0;
	virtual int DLLCALL GetNumTerrainInfos() = 0;
	virtual int DLLCALL GetNumUnitCombatClassInfos() = 0;
	virtual int DLLCALL GetNumUnitInfos() = 0;
	virtual int DLLCALL GetNumVictoryInfos() = 0;
	virtual int DLLCALL GetNumWorldInfos() = 0;

	virtual ICvBuildInfo1* DLLCALL GetBuildInfo(BuildTypes eBuildNum) = 0;
	virtual ICvBuildingInfo1* DLLCALL GetBuildingInfo(BuildingTypes eBuilding) = 0;
	virtual ICvCivilizationInfo1* DLLCALL GetCivilizationInfo(CivilizationTypes eCivilizationNum) = 0;
	virtual ICvColorInfo1* DLLCALL GetColorInfo(ColorTypes e) = 0;
	virtual ICvEraInfo1* DLLCALL GetEraInfo(EraTypes eEraNum) = 0;
	virtual ICvFeatureInfo1* DLLCALL GetFeatureInfo(FeatureTypes eFeatureNum) = 0;
	virtual ICvGameOptionInfo1* DLLCALL GetGameOptionInfo(GameOptionTypes eGameOptionNum) = 0;
	virtual ICvGameSpeedInfo1* DLLCALL GetGameSpeedInfo(GameSpeedTypes eGameSpeedNum) = 0;
	virtual ICvHandicapInfo1* DLLCALL GetHandicapInfo(HandicapTypes eHandicapNum) = 0;
	virtual ICvInterfaceModeInfo1* DLLCALL GetInterfaceModeInfo(InterfaceModeTypes e) = 0;
	virtual ICvImprovementInfo1* DLLCALL GetImprovementInfo(ImprovementTypes eImprovement) = 0;
	virtual ICvLeaderHeadInfo1* DLLCALL GetLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum) = 0;
	virtual ICvMinorCivInfo1* DLLCALL GetMinorCivInfo(MinorCivTypes eMinorCiv) = 0;
	virtual ICvMissionInfo1* DLLCALL GetMissionInfo(MissionTypes eMission) = 0;
	virtual ICvPlayerColorInfo1* DLLCALL GetPlayerColorInfo(PlayerColorTypes e) = 0;
	virtual ICvPlayerOptionInfo1* DLLCALL GetPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum) = 0;
	virtual ICvPolicyInfo1* DLLCALL GetPolicyInfo(PolicyTypes ePolicy) = 0;
	virtual ICvPromotionInfo1* DLLCALL GetPromotionInfo(PromotionTypes ePromotion) = 0;
	virtual ICvResourceInfo1* DLLCALL GetResourceInfo(ResourceTypes eResourceNum) = 0;
	virtual ICvTechInfo1* DLLCALL GetTechInfo(TechTypes eTech) = 0;
	virtual ICvTerrainInfo1* DLLCALL GetTerrainInfo(TerrainTypes eTerrainNum) = 0;
	virtual ICvUnitInfo1* DLLCALL GetUnitInfo(UnitTypes eUnit) = 0;
	virtual ICvUnitCombatClassInfo1* DLLCALL GetUnitCombatClassInfo(UnitCombatTypes eUnitCombat) = 0;
	virtual ICvVictoryInfo1* DLLCALL GetVictoryInfo(VictoryTypes eVictoryType) = 0;

	// Defines
	// ***** EXPOSED *****
	// use very sparingly - this is costly
	virtual bool DLLCALL GetDefineSTRING(char* szBuffer, size_t lenBuffer, const char* szName, bool bReportErrors = true) = 0;

	virtual int DLLCALL GetMOVE_DENOMINATOR() const = 0;
	virtual int DLLCALL GetMAX_CITY_HIT_POINTS() const = 0;
	virtual float DLLCALL GetCITY_ZOOM_OFFSET() const = 0;
	virtual float DLLCALL GetCITY_ZOOM_LEVEL_1() const = 0;
	virtual float DLLCALL GetCITY_ZOOM_LEVEL_2() const = 0;
	virtual float DLLCALL GetCITY_ZOOM_LEVEL_3() const = 0;
	virtual int DLLCALL GetRUINS_IMPROVEMENT() const = 0;
	virtual int DLLCALL GetSHALLOW_WATER_TERRAIN() const = 0;
	virtual int DLLCALL GetDEFICIT_UNIT_DISBANDING_THRESHOLD() const = 0;
	virtual int DLLCALL GetLAST_UNIT_ART_ERA() const = 0;
	virtual int DLLCALL GetLAST_EMBARK_ART_ERA() const = 0;
	virtual int DLLCALL GetHEAVY_RESOURCE_THRESHOLD() const = 0;
	virtual int DLLCALL GetSTANDARD_HANDICAP() const = 0;
	virtual int DLLCALL GetSTANDARD_GAMESPEED() const = 0;
	virtual int DLLCALL GetLAST_BRIDGE_ART_ERA() const = 0;
	virtual int DLLCALL GetBARBARIAN_CIVILIZATION() const = 0;
	virtual int DLLCALL GetMINOR_CIVILIZATION() const = 0;
	virtual int DLLCALL GetBARBARIAN_HANDICAP() const = 0;
	virtual int DLLCALL GetBARBARIAN_LEADER() const = 0;
	virtual int DLLCALL GetMINOR_CIV_HANDICAP() const = 0;
	virtual int DLLCALL GetWALLS_BUILDINGCLASS() const = 0;
	virtual int DLLCALL GetAI_HANDICAP() const = 0;
	virtual int DLLCALL GetNUM_CITY_PLOTS() const = 0;

	virtual const char** DLLCALL GetHexDebugLayerNames() = 0;
	virtual float DLLCALL GetHexDebugLayerScale(const char* szLayerName) = 0;
	virtual bool DLLCALL GetHexDebugLayerString(ICvPlot1* pPlot, const char* szLayerName, PlayerTypes ePlayer, char* szBuffer, unsigned int uiBufferLength) = 0;

	virtual void DLLCALL Init() = 0;
	virtual void DLLCALL Uninit() = 0;
	virtual ICvScriptSystemUtility1* DLLCALL GetScriptSystemUtility() = 0;
	virtual const char* DLLCALL GetNotificationType(int NotificationID) const = 0;

	virtual bool DLLCALL GetLogging() = 0;
	virtual void DLLCALL SetLogging(bool bEnable) = 0;
	virtual int DLLCALL GetRandLogging() = 0;
	virtual void DLLCALL SetRandLogging(int iRandLoggingFlags) = 0;
	virtual bool DLLCALL GetAILogging() = 0;
	virtual void DLLCALL SetAILogging(bool bEnable) = 0;
	virtual bool DLLCALL GetAIPerfLogging() = 0;
	virtual void DLLCALL SetAIPerfLogging(bool bEnable) = 0;
	virtual bool DLLCALL GetBuilderAILogging() = 0;
	virtual void DLLCALL SetBuilderAILogging(bool bEnable) = 0;
	virtual bool DLLCALL GetPlayerAndCityAILogSplit() = 0;
	virtual void DLLCALL SetPlayerAndCityAILogSplit(bool bEnable) = 0;
	virtual bool DLLCALL GetTutorialLogging() = 0;
	virtual void DLLCALL SetTutorialLogging(bool bEnable) = 0;
	virtual bool DLLCALL GetTutorialDebugging() = 0;
	virtual void DLLCALL SetTutorialDebugging(bool bEnable) = 0;
	virtual bool DLLCALL GetAllowRClickMovementWhileScrolling() = 0;
	virtual void DLLCALL SetAllowRClickMovementWhileScrolling(bool bAllow) = 0;
	virtual bool DLLCALL GetPostTurnAutosaves() = 0;
	virtual void DLLCALL SetPostTurnAutosaves(bool bEnable) = 0;
	
	virtual ICvDLLDatabaseUtility1* DLLCALL GetDatabaseLoadUtility() = 0;
	virtual int* DLLCALL GetPlotDirectionX() = 0;
	virtual int* DLLCALL GetPlotDirectionY() = 0;
	virtual int* DLLCALL GetCityPlotX() = 0; 
	virtual int* DLLCALL GetCityPlotY() = 0;
	virtual void DLLCALL SetGameDatabase(Database::Connection* pGameDatabase) = 0;
	virtual bool DLLCALL SetDLLIFace(ICvEngineUtility1* pDll) = 0;
	virtual bool DLLCALL IsGraphicsInitialized() const = 0;
	virtual void DLLCALL SetGraphicsInitialized(bool bVal) = 0;
	virtual void DLLCALL SetOutOfSyncDebuggingEnabled(bool isEnabled) = 0;
	virtual bool DLLCALL RandomNumberGeneratorSyncCheck(PlayerTypes ePlayer, ICvRandom1* pRandom, bool bIsHost) = 0;

	virtual unsigned int DLLCALL CreateRandomNumberGenerator() = 0;
	virtual ICvRandom1* DLLCALL GetRandomNumberGenerator(unsigned int index) = 0;
	virtual void DLLCALL DestroyRandomNumberGenerator(unsigned int index) = 0;

	virtual unsigned int DLLCALL CreateNetInitInfo() = 0;
	virtual ICvNetInitInfo1* DLLCALL GetNetInitInfo(unsigned int index) = 0;
	virtual void DLLCALL DestroyNetInitInfo(unsigned int index) = 0;

	virtual unsigned int DLLCALL CreateNetLoadGameInfo() = 0;
	virtual ICvNetLoadGameInfo1* DLLCALL GetNetLoadGameInfo(unsigned int index) = 0;
	virtual void DLLCALL DestroyNetLoadGameInfo(unsigned int index) = 0;

	virtual void DLLCALL TEMPOnHexUnitChanged(ICvUnit1* pUnit) = 0;
	virtual void DLLCALL TEMPOnHexUnitChangedAttack(ICvUnit1* pUnit) = 0;
	virtual ICvEnumerator* DLLCALL TEMPCalculatePathFinderUpdates(ICvUnit1* pHeadSelectedUnit, int iMouseMapX, int iMouseMapY) = 0;
	virtual void DLLCALL ResetPathFinder() = 0;
};

//------------------------------------------------------------------------------
// Script System Utility Interfaces
//------------------------------------------------------------------------------
class ICvScriptSystemUtility1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvScriptSystemUtility1; }

	//! Allows the DLL to register additional global functions and data to Lua.
	virtual void DLLCALL RegisterScriptLibraries(lua_State* L) = 0;

	virtual void DLLCALL PushCvCityInstance(lua_State* L, ICvCity1* pkCity) = 0;
	virtual ICvCity1* DLLCALL GetCvCityInstance(lua_State* L, int index, bool bErrorOnFail = true) = 0;

	virtual void DLLCALL PushCvDealInstance(lua_State* L, ICvDeal1* pkDeal) = 0;
	virtual ICvDeal1* DLLCALL GetCvDealInstance(lua_State* L, int index, bool bErrorOnFail = true) = 0;

	virtual void DLLCALL PushCvPlotInstance(lua_State* L, ICvPlot1* pkPlot) = 0;
	virtual ICvPlot1* DLLCALL GetCvPlotInstance(lua_State* L, int index, bool bErrorOnFail = true) = 0;

	virtual void DLLCALL PushCvUnitInstance(lua_State* L, ICvUnit1* pkUnit) = 0;
	virtual ICvUnit1* DLLCALL GetCvUnitInstance(lua_State* L, int index, bool bErrorOnFail = true) = 0;

	virtual void DLLCALL PushReplayFromStream(lua_State* L, FDataStream& stream) = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Building Info interfaces
//------------------------------------------------------------------------------
class ICvBuildingInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvCombatInfo1; }

	virtual const char* DLLCALL GetType() const = 0;
	virtual const char* DLLCALL GetText() const = 0;
	virtual int DLLCALL GetPreferredDisplayPosition() const = 0;
	virtual bool DLLCALL IsBorderObstacle() const = 0;
	virtual bool DLLCALL IsPlayerBorderObstacle() const = 0;
	virtual const char* DLLCALL GetArtDefineTag() const = 0;			
	virtual const bool DLLCALL GetArtInfoCulturalVariation() const = 0;
	virtual const bool DLLCALL GetArtInfoEraVariation() const = 0;
	virtual const bool DLLCALL GetArtInfoRandomVariation() const = 0;
	virtual const char* DLLCALL GetWonderSplashAudio() const = 0;
};

//------------------------------------------------------------------------------
// City Interfaces
//------------------------------------------------------------------------------
class ICvCity1 : public ICvUnknown
{
public:

	static GUID DLLCALL GetInterfaceId() { return guidICvCity1; }

	virtual int DLLCALL GetID() const = 0;
	virtual PlayerTypes DLLCALL GetOwner() const = 0;

	virtual int DLLCALL GetMaxHitPoints() const = 0;

	virtual void DLLCALL GetPosition(int& iX, int& iY) const = 0;

	virtual ICvPlot1* DLLCALL Plot() const = 0;

	virtual int DLLCALL GetPopulation() const = 0;

	virtual const char* DLLCALL GetName() const = 0;

	virtual bool DLLCALL CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost = false) = 0;
	virtual CitySizeTypes DLLCALL GetCitySizeType() const = 0;
	virtual const char* DLLCALL GetCityBombardEffectTag() const = 0;
	
	virtual TeamTypes DLLCALL GetTeam() const = 0;

	virtual bool DLLCALL IsPuppet() const = 0;
	
	virtual int DLLCALL GetX() const = 0;
	virtual int DLLCALL GetY() const = 0;
	
	virtual int DLLCALL GetStrengthValue(bool bForRangeStrike = false) const = 0;	
	virtual int DLLCALL FoodDifference(bool bBottom = true) const = 0;	
	virtual int DLLCALL GetFoodTurnsLeft() const = 0;				
	virtual int DLLCALL GetYieldRate(int eIndex) const = 0;		
	virtual int DLLCALL GetJONSCulturePerTurn() const = 0;
	virtual IDInfo DLLCALL GetIDInfo() const = 0;
	
	virtual bool DLLCALL IsWorkingPlot(ICvPlot1* pPlot) const = 0;
	virtual bool DLLCALL CanWork(ICvPlot1* pPlot) const = 0;
	virtual ICvPlot1* DLLCALL GetCityPlotFromIndex(int iIndex) const = 0;
	virtual FAutoArchive& DLLCALL GetSyncArchive() = 0;
};

//------------------------------------------------------------------------------
// Combat Info interfaces
//------------------------------------------------------------------------------
class ICvCombatInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvCombatInfo1; }

	virtual ICvUnit1* DLLCALL GetUnit(BattleUnitTypes unitType) const = 0;
	virtual ICvCity1* DLLCALL GetCity(BattleUnitTypes unitType) const = 0;
	virtual ICvPlot1* DLLCALL GetPlot() const = 0;

	virtual bool DLLCALL GetAttackerAdvances() const = 0;
	virtual bool DLLCALL GetVisualizeCombat() const = 0;
	virtual bool DLLCALL GetDefenderRetaliates() const  = 0;
	virtual int	 DLLCALL GetNuclearDamageLevel() const  = 0;
	virtual bool DLLCALL GetAttackIsRanged() const  = 0;
	virtual bool DLLCALL GetAttackIsBombingMission() const  = 0;
	virtual bool DLLCALL GetAttackIsAirSweep() const  = 0;
	virtual bool DLLCALL GetDefenderCaptured() const  = 0;

	virtual int DLLCALL GetDamageInflicted(BattleUnitTypes unitType) const = 0;
	virtual int DLLCALL GetFinalDamage(BattleUnitTypes unitType) const = 0;

	virtual const CvCombatMemberEntry* DLLCALL GetDamageMember(int iIndex) const = 0;
	virtual int DLLCALL GetDamageMemberCount() const = 0;
	virtual int DLLCALL GetMaxDamageMemberCount() const = 0;

	virtual void* DLLCALL TEMPGetRawCombatInfo() const = 0;
};

//------------------------------------------------------------------------------
// Deal Interfaces
//------------------------------------------------------------------------------
class ICvDeal1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvDeal1; }

	virtual PlayerTypes DLLCALL GetOtherPlayer(PlayerTypes eFromPlayer) = 0;
	virtual PlayerTypes DLLCALL GetToPlayer() = 0;
	virtual PlayerTypes DLLCALL GetFromPlayer() = 0;
	virtual unsigned int DLLCALL GetStartTurn() = 0;
	virtual unsigned int DLLCALL GetDuration() = 0;
	virtual unsigned int DLLCALL GetEndTurn() = 0;

	virtual void DLLCALL CopyFrom(ICvDeal1* pOtherDeal) = 0;
	virtual void DLLCALL Read(FDataStream& kStream) = 0;
	virtual void DLLCALL Write(FDataStream& kStream) = 0;

};

//------------------------------------------------------------------------------
// Deal AI Interfaces
//------------------------------------------------------------------------------
class ICvDealAI1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvDealAI1; }

	virtual ICvPlayer1* DLLCALL GetPlayer() = 0;
	virtual int DLLCALL DoHumanOfferDealToThisAI(ICvDeal1* pDeal) = 0;
	virtual void DLLCALL DoAcceptedDeal(PlayerTypes eFromPlayer, ICvDeal1* pDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering) = 0;
	virtual int DLLCALL DoHumanDemand(ICvDeal1* pDeal) = 0;
	virtual void DLLCALL DoAcceptedDemand(PlayerTypes eFromPlayer, ICvDeal1* pDeal) = 0;
	virtual bool DLLCALL DoEqualizeDealWithHuman(ICvDeal1* pDeal, PlayerTypes eOtherPlayer, bool bDontChangeMyExistingItems, bool bDontChangeTheirExistingItems, bool &bDealGoodToBeginWith, bool &bCantMatchDeal) = 0;
};

//------------------------------------------------------------------------------
// Diplomacy AI Interfaces
//------------------------------------------------------------------------------
class ICvDiplomacyAI1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvDiplomacyAI1; }

	virtual void DLLCALL DoBeginDiploWithHuman() = 0;
	virtual const char* DLLCALL GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer = NO_PLAYER, const char* szOptionalKey1 = "") = 0;
	virtual void DLLCALL TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1) = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Game Interfaces
//------------------------------------------------------------------------------
class ICvGame1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGame1; }

	virtual ICvPlayer1* DLLCALL GetPlayer(const PlayerTypes ePlayer) = 0;
	virtual PlayerTypes DLLCALL GetActivePlayer() = 0;
	virtual TeamTypes DLLCALL GetActiveTeam() = 0;
	virtual int DLLCALL GetGameTurn() const = 0;

	virtual void DLLCALL ChangeNumGameTurnActive(int iChange, const char* why) = 0;
	virtual int DLLCALL CountHumanPlayersAlive() const = 0;	
	virtual int DLLCALL CountNumHumanGameTurnActive() = 0;
	virtual bool DLLCALL CyclePlotUnits(ICvPlot1* pPlot, bool bForward = true, bool bAuto = false, int iCount = -1) = 0;		
	virtual void DLLCALL CycleUnits(bool bClear, bool bForward = true, bool bWorkers = false) = 0;							
	virtual void DLLCALL DoGameStarted() = 0;
	virtual void DLLCALL EndTurnTimerReset() = 0;
	virtual void DLLCALL EndTurnTimerSemaphoreDecrement() = 0;
	virtual void DLLCALL EndTurnTimerSemaphoreIncrement() = 0;
	virtual int DLLCALL GetAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual int DLLCALL IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual ImprovementTypes DLLCALL GetBarbarianCampImprovementType() = 0;
	virtual int DLLCALL GetElapsedGameTurns() const = 0;																		
	virtual bool DLLCALL GetFOW() = 0;
	virtual ICvGameDeals1* DLLCALL GetGameDeals() = 0;
	virtual GameSpeedTypes DLLCALL GetGameSpeedType() const = 0;
	virtual GameStateTypes DLLCALL GetGameState() = 0;
	virtual HandicapTypes DLLCALL GetHandicapType() const = 0;
	virtual ICvRandom1* DLLCALL GetRandomNumberGenerator() = 0;
	virtual int DLLCALL GetJonRandNum(int iNum, const char* pszLog) = 0;		
	virtual int DLLCALL GetMaxTurns() const = 0;	
	virtual int DLLCALL GetNumGameTurnActive() = 0;																		
	virtual PlayerTypes DLLCALL GetPausePlayer() = 0;																			
	virtual bool DLLCALL GetPbemTurnSent() const = 0;
	virtual ICvUnit1* DLLCALL GetPlotUnit(ICvPlot1* pPlot, int iIndex) = 0;
	virtual unsigned int DLLCALL GetVariableCitySizeFromPopulation(unsigned int nPopulation) = 0;
	virtual VictoryTypes DLLCALL GetVictory() const = 0;																	
	virtual int DLLCALL GetVotesNeededForDiploVictory() const = 0;
	virtual TeamTypes DLLCALL GetWinner() const = 0;	
	virtual int DLLCALL GetWinningTurn() const = 0;
	virtual void DLLCALL HandleAction(int iAction) = 0;
	virtual bool DLLCALL HasTurnTimerExpired() = 0;
	virtual void DLLCALL Init(HandicapTypes eHandicap) = 0;
	virtual bool DLLCALL Init2() = 0;
	virtual void DLLCALL InitScoreCalculation() = 0;
	virtual void DLLCALL InitTacticalAnalysisMap(int iNumPlots) = 0;
	virtual bool DLLCALL IsCityScreenBlocked() = 0;
	virtual bool DLLCALL CanOpenCityScreen(PlayerTypes eOpener, ICvCity1* pCity) = 0;
	virtual bool DLLCALL IsDebugMode() const = 0;
	virtual bool DLLCALL IsFinalInitialized() const = 0;																		
	virtual bool DLLCALL IsGameMultiPlayer() const = 0;																			
	virtual bool DLLCALL IsHotSeat() const = 0;		
	virtual bool IsMPOption(MultiplayerOptionTypes eIndex) const = 0;												
	virtual bool DLLCALL IsNetworkMultiPlayer() const = 0;																	
	virtual bool IsOption(GameOptionTypes eIndex) const = 0;																
	virtual bool DLLCALL IsPaused() = 0;																									
	virtual bool DLLCALL IsPbem() const = 0;																								
	virtual bool DLLCALL IsTeamGame() const = 0;																						
	virtual void DLLCALL LogGameState(bool bLogHeaders = false) = 0;
	virtual void DLLCALL ResetTurnTimer() = 0;
	virtual void DLLCALL SelectAll(ICvPlot1* pPlot) = 0;
	virtual void DLLCALL SelectGroup(ICvUnit1* pUnit, bool bShift, bool bCtrl, bool bAlt) = 0;
	virtual bool DLLCALL SelectionListIgnoreBuildingDefense() = 0;
	virtual void DLLCALL SelectionListMove(ICvPlot1* pPlot, bool bShift) = 0;
	virtual void DLLCALL SelectSettler(void) = 0;
	virtual void DLLCALL SelectUnit(ICvUnit1* pUnit, bool bClear, bool bToggle = false, bool bSound = false) = 0;
	virtual void DLLCALL SendPlayerOptions(bool bForce = false) = 0;
	virtual void DLLCALL SetDebugMode(bool bDebugMode) = 0;
	virtual void DLLCALL SetFinalInitialized(bool bNewValue) = 0;
	virtual void DLLCALL SetInitialTime(unsigned int uiNewValue) = 0;
	virtual void SetMPOption(MultiplayerOptionTypes eIndex, bool bEnabled) = 0;
	virtual void DLLCALL SetPausePlayer(PlayerTypes eNewValue) = 0;
	virtual void DLLCALL SetTimeStr(_Inout_z_cap_c_(256) char* szString, int iGameTurn, bool bSave) = 0;
	virtual bool DLLCALL TunerEverConnected() const = 0;
	virtual void DLLCALL Uninit() = 0;
	virtual void DLLCALL UnitIsMoving() = 0;
	virtual void DLLCALL Update() = 0;
	virtual void DLLCALL UpdateSelectionList() = 0;
	virtual void DLLCALL UpdateTestEndTurn() = 0;
	virtual void DLLCALL Read(FDataStream& kStream) = 0;
	virtual void DLLCALL Write(FDataStream& kStream) const = 0;
	virtual void DLLCALL ReadSupportingClassData(FDataStream& kStream) = 0;
	virtual void DLLCALL WriteSupportingClassData(FDataStream& kStream) const = 0;
	virtual void DLLCALL WriteReplay(FDataStream& kStream) const = 0;

	virtual bool DLLCALL CanMoveUnitTo(ICvUnit1* pUnit, ICvPlot1* pPlot) const = 0;
};

/// Similar to the CvGame interface, but will not lock access.
/// The methods are safe to call while the GameCore is processing.
class ICvGameAsynch1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGameAsynch1; }

	virtual PlayerTypes DLLCALL GetActivePlayer() = 0;
	virtual TeamTypes	DLLCALL GetActiveTeam() = 0;
	virtual int			DLLCALL GetGameTurn() const = 0;
	virtual GameSpeedTypes DLLCALL GetGameSpeedType() const = 0;
	virtual GameStateTypes DLLCALL GetGameState() = 0;
	virtual HandicapTypes DLLCALL GetHandicapType() const = 0;
	virtual int			DLLCALL IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual bool		DLLCALL IsDebugMode() const = 0;
	virtual bool		DLLCALL IsFinalInitialized() const = 0;																		
	virtual bool		DLLCALL IsGameMultiPlayer() const = 0;																			
	virtual bool		DLLCALL IsHotSeat() const = 0;																							
	virtual bool		DLLCALL IsMPOption(MultiplayerOptionTypes eIndex) const = 0;												
	virtual bool		DLLCALL IsNetworkMultiPlayer() const = 0;																	
	virtual bool		DLLCALL IsOption(GameOptionTypes eIndex) const = 0;																
	virtual bool		DLLCALL IsPaused() = 0;																									
	virtual bool		DLLCALL IsPbem() const = 0;																								
	virtual bool		DLLCALL IsTeamGame() const = 0;										
	virtual bool		DLLCALL TunerEverConnected() const = 0;
};

//------------------------------------------------------------------------------
// Improvement Info interfaces
//------------------------------------------------------------------------------
class ICvImprovementInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvImprovementInfo1; }

	virtual const char* DLLCALL GetType() const = 0;
	virtual const char* DLLCALL GetText() const = 0;
	virtual bool DLLCALL IsWater() const = 0;
	virtual bool DLLCALL IsDestroyedWhenPillaged() const = 0;
	virtual bool DLLCALL IsGoody() const = 0;
	virtual const char* DLLCALL GetArtDefineTag() const = 0;
	virtual ImprovementUsageTypes DLLCALL GetImprovementUsage() const = 0;
	virtual int DLLCALL GetWorldSoundscapeScriptId() const = 0;
	virtual bool DLLCALL GetTerrainMakesValid(int i) const = 0;
	virtual bool DLLCALL IsImprovementResourceMakesValid(int i) const = 0;
};


//------------------------------------------------------------------------------
// Map Interfaces
//------------------------------------------------------------------------------
class ICvMap1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvMap1; }

	virtual void DLLCALL Init(CvMapInitData* pInitData = NULL) = 0;
	virtual void DLLCALL Uninit() = 0;
	virtual void DLLCALL UpdateSymbolVisibility() = 0;
	virtual void DLLCALL UpdateLayout(bool bDebug) = 0;
	virtual void DLLCALL UpdateDeferredFog() = 0;
	virtual ICvCity1* DLLCALL FindCity(	int iX, 
								int iY, 
								PlayerTypes eOwner = NO_PLAYER, 
								TeamTypes eTeam = NO_TEAM, 
								bool bSameArea = true, 
								bool bCoastalOnly = false, 
								TeamTypes eTeamAtWarWith = NO_TEAM, 
								DirectionTypes eDirection = NO_DIRECTION, 
								ICvCity1* pSkipCity = NULL) = 0;

	virtual int DLLCALL NumPlots() const = 0;
	virtual int DLLCALL PlotNum(int iX, int iY) const = 0;
	virtual int DLLCALL GetGridWidth() const = 0;
	virtual int DLLCALL GetGridHeight() const = 0;
	virtual bool DLLCALL IsWrapX() const = 0;
	virtual bool DLLCALL IsWrapY() const = 0;
	virtual ICvPlot1* DLLCALL GetPlotByIndex(int iIndex) const = 0;
	virtual ICvPlot1* DLLCALL GetPlot(int iX, int iY) const = 0;
	virtual void DLLCALL RecalculateLandmasses() = 0;

	virtual void DLLCALL Read(FDataStream& kStream) = 0;
	virtual void DLLCALL Write(FDataStream& kStream) const = 0;
	virtual int DLLCALL Validate() = 0;
};


//------------------------------------------------------------------------------
// Mission Data Interfaces
//------------------------------------------------------------------------------
class ICvMissionData1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvMissionData1; }

	virtual int DLLCALL GetData1() const = 0;
	virtual int DLLCALL GetData2() const = 0;
	virtual int DLLCALL GetFlags() const = 0;
	virtual int DLLCALL GetPushTurn() const = 0;
	virtual MissionTypes DLLCALL GetMissionType() const = 0;
};

//------------------------------------------------------------------------------
// Network Message Handling interfaces
//------------------------------------------------------------------------------
class ICvNetMessageHandler1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvNetMessageHandler1; }

	virtual void DLLCALL ResponseAdvancedStartAction(PlayerTypes ePlayer, AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd) = 0;
	virtual void DLLCALL ResponseAutoMission(PlayerTypes ePlayer, int iUnitID) = 0;
	virtual void DLLCALL ResponseBarbarianRansom(PlayerTypes ePlayer, int iOptionChosen, int iUnitID) = 0;
	virtual void DLLCALL ResponseChangeWar(PlayerTypes ePlayer, TeamTypes eRivalTeam, bool bWar) = 0;
	virtual void DLLCALL ResponseCityBuyPlot(PlayerTypes ePlayer, int iCityID, int iX, int iY) = 0;
	virtual void DLLCALL ResponseCityDoTask(PlayerTypes ePlayer, int iCityID, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual void DLLCALL ResponseCityPopOrder(PlayerTypes ePlayer, int iCityID, int iNum) = 0;
	virtual void DLLCALL ResponseCityPurchase(PlayerTypes ePlayer, int iCityID, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType) = 0;
	virtual void DLLCALL ResponseCityPushOrder(PlayerTypes ePlayer, int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl) = 0;
	virtual void DLLCALL ResponseCitySwapOrder(PlayerTypes ePlayer, int iCityID, int iNum) = 0;
	virtual void DLLCALL ResponseChooseElection(PlayerTypes ePlayer, int iSelection, int iVoteId) = 0;
	virtual void DLLCALL ResponseDestroyUnit(PlayerTypes ePlayer, int iUnitID) = 0;
	virtual void DLLCALL ResponseDiplomacyFromUI(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2) = 0;
	virtual void DLLCALL ResponseDiploVote(PlayerTypes ePlayer, PlayerTypes eVotePlayer) = 0;
	virtual void DLLCALL ResponseDoCommand(PlayerTypes ePlayer, int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt) = 0;
	virtual void DLLCALL ResponseExtendedGame(PlayerTypes ePlayer) = 0;
	virtual void DLLCALL ResponseGiftUnit(PlayerTypes ePlayer, PlayerTypes eMinor, int iUnitID) = 0;
	virtual void DLLCALL ResponseGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit) = 0;
	virtual void DLLCALL ResponseLaunchSpaceship(PlayerTypes ePlayer, VictoryTypes eVictory) = 0;
	virtual void DLLCALL ResponseLiberatePlayer(PlayerTypes ePlayer, PlayerTypes eLiberatedPlayer, int iCityID) = 0;
	virtual void DLLCALL ResponseMinorCivGiftGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold) = 0;
	virtual void DLLCALL ResponseMinorNoUnitSpawning(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue) = 0;
	virtual void DLLCALL ResponsePlayerDealFinalized(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, PlayerTypes eActBy, bool bAccepted) = 0;
	virtual void DLLCALL ResponsePlayerOption(PlayerTypes ePlayer, PlayerOptionTypes eOption, bool bValue) = 0;
	virtual void DLLCALL ResponsePushMission(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift) = 0;
	virtual void DLLCALL ResponseRenameCity(PlayerTypes ePlayer, int iCityID, _In_z_ const char* szName) = 0;
	virtual void DLLCALL ResponseRenameUnit(PlayerTypes ePlayer, int iUnitID, _In_z_ const char* szName) = 0;
	virtual void DLLCALL ResponseResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, bool bShift) = 0;
	virtual void DLLCALL ResponseReturnCivilian(PlayerTypes ePlayer, PlayerTypes eToPlayer, int iUnitID, bool bReturn) = 0;
	virtual void DLLCALL ResponseSellBuilding(PlayerTypes ePlayer, int iCityID, BuildingTypes eBuilding) = 0;
	virtual void DLLCALL ResponseSetCityAIFocus(PlayerTypes ePlayer, int iCityID, CityAIFocusTypes eFocus) = 0;
	virtual void DLLCALL ResponseSetCityAvoidGrowth(PlayerTypes ePlayer, int iCityID, bool bAvoidGrowth) = 0;
	virtual void DLLCALL ResponseSwapUnits(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift) = 0;
	virtual void DLLCALL ResponseUpdateCityCitizens(PlayerTypes ePlayer, int iCityID) = 0;
	virtual void DLLCALL ResponseUpdatePolicies(PlayerTypes ePlayer, bool bNOTPolicyBranch, int iPolicyID, bool bValue) = 0;
};

class ICvNetMessageHandler2 : public ICvNetMessageHandler1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvNetMessageHandler2; }

	virtual void DLLCALL ResponseCityPurchase(PlayerTypes ePlayer, int iCityID, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, int ePurchaseYield) = 0;
	virtual void DLLCALL ResponseFoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief) = 0;
	virtual void DLLCALL ResponseFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, int iCityX, int iCityY) = 0;
	virtual void DLLCALL ResponseEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, int iCityX, int iCityY) = 0;
	virtual void DLLCALL ResponseMayaBonusChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit) = 0;
	virtual void DLLCALL ResponseFaithGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit) = 0;
	virtual void DLLCALL ResponseMoveSpy(PlayerTypes ePlayer, int iSpyIndex, int iTargetPlayer, int iTargetCity, bool bAsDiplomat) = 0;
	virtual void DLLCALL ResponseStageCoup(PlayerTypes eSpyPlayer, int iSpyIndex) = 0;
	virtual void DLLCALL ResponseFaithPurchase(PlayerTypes ePlayer, FaithPurchaseTypes eFaithPurchaseType, int iFaithPurchaseIndex) = 0;
	virtual void DLLCALL ResponseMinorCivBullyGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold) = 0;
	virtual void DLLCALL ResponseMinorCivBullyUnit(PlayerTypes ePlayer, PlayerTypes eMinor, UnitTypes eUnitType) = 0;
	virtual void DLLCALL ResponseMinorCivGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY) = 0;
	virtual void DLLCALL ResponseMinorCivBuyout(PlayerTypes eMajor, PlayerTypes eMinor) = 0;
	virtual void DLLCALL ResponsePledgeMinorProtection(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue, bool bPledgeNowBroken) = 0;
	virtual void DLLCALL ResponseResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, PlayerTypes ePlayerToStealFrom, bool bShift) = 0;
};

//------------------------------------------------------------------------------
// Network sync interfaces
//------------------------------------------------------------------------------
class ICvNetworkSyncronization1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvNetworkSyncronization1; }

	virtual void DLLCALL ClearCityDeltas() = 0;
	virtual void DLLCALL ClearPlayerDeltas() = 0;
	virtual void DLLCALL ClearPlotDeltas() = 0;
	virtual void DLLCALL ClearRandomDeltas() = 0;
	virtual void DLLCALL ClearUnitDeltas() = 0;
	
	virtual void DLLCALL SyncCities() = 0;
	virtual void DLLCALL SyncPlayers() = 0;
	virtual void DLLCALL SyncPlots() = 0;
	virtual void DLLCALL SyncUnits() = 0;
};

class ICvPathFinderUpdate1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPathFinderUpdate1; }

	virtual int DLLCALL GetX() const = 0;
	virtual int DLLCALL GetY() const = 0;
	virtual int DLLCALL GetTurnNumber() const = 0;
};
//------------------------------------------------------------------------------
// Player Interfaces
//------------------------------------------------------------------------------
class ICvPlayer1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPlayer1; }

	virtual void DLLCALL Init(PlayerTypes eID) = 0;
	virtual void DLLCALL SetupGraphical() = 0;
	virtual void DLLCALL LaterInit () = 0; // what a lousy name. This is called after the map and other aspects have been created.
	virtual void DLLCALL Uninit() = 0;
	virtual bool DLLCALL IsHuman() const = 0;
	virtual bool DLLCALL IsBarbarian() const = 0;
	virtual const char* DLLCALL GetName() const = 0;
	virtual const char* const DLLCALL GetNickName() const = 0;
	virtual ArtStyleTypes DLLCALL GetArtStyleType() const = 0;
	virtual ICvUnit1* DLLCALL GetFirstReadyUnit() = 0;
	virtual bool DLLCALL HasBusyUnit() const = 0;
	virtual bool DLLCALL HasBusyUnitOrCity() const = 0;
	virtual int DLLCALL GetScore(bool bFinal = false, bool bVictory = false) const = 0;
	virtual ICvPlot1* DLLCALL GetStartingPlot() const = 0;
	virtual ICvCity1* DLLCALL GetCapitalCity() = 0;
	virtual bool DLLCALL IsHasLostCapital() const = 0;
	virtual void DLLCALL SetStartTime(uint uiStartTime) = 0;
	virtual bool DLLCALL IsMinorCiv() const = 0;
	virtual bool DLLCALL IsAlive() const = 0;
	virtual bool DLLCALL IsEverAlive() const = 0;
	virtual bool DLLCALL IsTurnActive() const = 0;
	virtual void DLLCALL SetTurnActive(bool bNewValue, bool bDoTurn = true) = 0;
	virtual PlayerTypes DLLCALL GetID() const = 0;
	virtual HandicapTypes DLLCALL GetHandicapType() const = 0;
	virtual CivilizationTypes DLLCALL GetCivilizationType() const = 0;
	virtual LeaderHeadTypes DLLCALL GetLeaderType() const = 0;
	virtual const char* DLLCALL GetLeaderTypeKey() const = 0;
	virtual EraTypes DLLCALL GetCurrentEra() const = 0;
	virtual TeamTypes DLLCALL GetTeam() const = 0;
	virtual PlayerColorTypes DLLCALL GetPlayerColor() const = 0;
	virtual bool DLLCALL IsOption(PlayerOptionTypes eIndex) const = 0;
	virtual ICvCity1* DLLCALL FirstCity(int* pIterIdx, bool bRev = false) = 0;
	virtual ICvCity1* DLLCALL NextCity(int* pIterIdx, bool bRev = false) = 0;
	virtual int DLLCALL GetNumCities() const = 0;
	virtual ICvCity1* DLLCALL GetCity(int iID) = 0;
	virtual ICvUnit1* DLLCALL FirstUnit(int* pIterIdx, bool bRev = false) = 0;
	virtual ICvUnit1* DLLCALL NextUnit(int* pIterIdx, bool bRev = false) = 0;
	virtual ICvUnit1* DLLCALL GetUnit(int iID) = 0;
	virtual int DLLCALL GetPlotDanger(ICvPlot1* pPlot) const = 0;
	virtual int DLLCALL GetCityDistanceHighwaterMark() const = 0;
	virtual void DLLCALL Disconnected() = 0;
	virtual void DLLCALL Reconnected() = 0;
	virtual void DLLCALL SetBusyUnitUpdatesRemaining(int iUpdateCount) = 0;
	virtual bool DLLCALL HasUnitsThatNeedAIUpdate() const = 0;
	
	virtual int DLLCALL GetNumPolicyBranchesFinished() const = 0;

	virtual void DLLCALL Read(FDataStream& kStream) = 0;
	virtual void DLLCALL Write(FDataStream& kStream) const = 0;

	virtual int DLLCALL GetGold() const = 0;
	virtual int DLLCALL CalculateBaseNetGold() = 0;
	virtual const char* DLLCALL GetEmbarkedGraphicOverride() = 0;

	virtual FAutoArchive& DLLCALL GetSyncArchive() = 0;

	virtual MinorCivTypes DLLCALL GetMinorCivType() const = 0;
	virtual ICvDiplomacyAI1* DLLCALL GetDiplomacyAI() = 0;
	virtual ICvDealAI1* DLLCALL GetDealAI() = 0;

	//Diplomacy
	virtual bool DLLCALL AddDiplomacyRequest(PlayerTypes ePlayerID, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1) = 0;
	virtual void DLLCALL ActiveDiplomacyRequestComplete() = 0;

	//Notifications
	virtual int DLLCALL AddNotification(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData = -1) = 0;
	virtual void DLLCALL ActivateNotification(int iLookupIndex) = 0;
	virtual void DLLCALL DismissNotification(int iLookupIndex, bool bUserInvoked) = 0;
	virtual bool DLLCALL MayUserDismissNotification(int iLookupIndex) = 0;
	virtual void DLLCALL RebroadcastNotifications(void) = 0;

	//! Get the current tech the player is researching and how many turns are left
	virtual bool DLLCALL GetCurrentResearchTech(TechTypes* pkTech, int *pkTurnsLeft) const = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Plot Interfaces
//------------------------------------------------------------------------------
class ICvPlot1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPlot1; }

	virtual TeamTypes DLLCALL GetTeam() const = 0;
	virtual FogOfWarModeTypes DLLCALL GetActiveFogOfWarMode() const = 0;
	virtual void DLLCALL UpdateCenterUnit() = 0;
	virtual bool DLLCALL IsAdjacent(ICvPlot1* pPlot) const = 0;
	virtual bool DLLCALL IsRiver() const = 0;
	virtual ICvPlot1* DLLCALL GetNeighboringPlot(DirectionTypes eDirection) const = 0;
	virtual int DLLCALL GetBuildTime(BuildTypes eBuild, PlayerTypes ePlayer) const = 0;
	virtual bool DLLCALL IsVisible(TeamTypes eTeam, bool bDebug) const = 0;
	virtual bool DLLCALL IsCity() const = 0;
	virtual bool DLLCALL IsEnemyCity(ICvUnit1* pUnit) const = 0;
	virtual bool DLLCALL IsFighting() const = 0;
	virtual bool DLLCALL IsTradeRoute (PlayerTypes ePlayer = NO_PLAYER) const = 0;
	virtual bool DLLCALL IsImpassable() const = 0;
	virtual void DLLCALL GetPosition(int& iX, int& iY) const = 0;
	virtual bool DLLCALL IsNEOfRiver() const = 0;
	virtual bool DLLCALL IsWOfRiver() const = 0;
	virtual bool DLLCALL IsNWOfRiver() const = 0;
	virtual FlowDirectionTypes DLLCALL GetRiverEFlowDirection() const = 0;
	virtual FlowDirectionTypes DLLCALL GetRiverSEFlowDirection() const = 0;
	virtual FlowDirectionTypes DLLCALL GetRiverSWFlowDirection() const = 0;
	virtual PlayerTypes DLLCALL GetOwner() const = 0;
	virtual PlotTypes DLLCALL GetPlotType() const = 0;
	virtual bool DLLCALL IsWater() const = 0;
	virtual bool DLLCALL IsHills() const = 0;
	virtual bool DLLCALL IsOpenGround() const = 0;
	virtual bool DLLCALL IsMountain() const = 0;
	virtual TerrainTypes DLLCALL GetTerrainType() const = 0;
	virtual FeatureTypes DLLCALL GetFeatureType() const = 0;
	virtual ResourceTypes DLLCALL GetResourceType(TeamTypes eTeam = NO_TEAM) const = 0;
	virtual int DLLCALL GetNumResource() const = 0;
	virtual ImprovementTypes DLLCALL GetImprovementType() const = 0;
	virtual bool DLLCALL IsImprovementPillaged() const = 0;
	virtual GenericWorldAnchorTypes DLLCALL GetWorldAnchor() const = 0;
	virtual int DLLCALL GetWorldAnchorData() const = 0;
	virtual RouteTypes DLLCALL GetRouteType() const = 0;
	virtual bool DLLCALL IsRoutePillaged() const = 0;
	virtual ICvCity1* DLLCALL GetPlotCity() const = 0;
	virtual ICvCity1* DLLCALL GetWorkingCity() const = 0;
	virtual bool DLLCALL IsRevealed(TeamTypes eTeam, bool bDebug) const = 0;
	virtual ImprovementTypes DLLCALL GetRevealedImprovementType(TeamTypes eTeam, bool bDebug) const = 0;
	virtual int DLLCALL GetBuildProgress(BuildTypes eBuild) const = 0;
	virtual bool DLLCALL GetAnyBuildProgress() const = 0;
	virtual void DLLCALL UpdateLayout(bool bDebug) = 0;
	virtual ICvUnit1* DLLCALL GetCenterUnit() = 0;
	virtual int DLLCALL GetNumUnits() const = 0;
	virtual ICvUnit1* DLLCALL GetUnitByIndex(int iIndex) const = 0;
	virtual void DLLCALL AddUnit(ICvUnit1* pUnit, bool bUpdate = true) = 0;
	virtual void DLLCALL RemoveUnit(ICvUnit1* pUnit, bool bUpdate = true) = 0;
	virtual const IDInfo* DLLCALL NextUnitNode(const IDInfo* pNode) const = 0;
	virtual IDInfo* DLLCALL NextUnitNode(IDInfo* pNode) = 0;
	virtual IDInfo* DLLCALL HeadUnitNode() = 0;
	virtual int DLLCALL GetPlotIndex() const = 0;
	virtual char DLLCALL GetContinentType() const = 0;

	virtual FAutoArchive& DLLCALL GetSyncArchive() = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Pregame interfaces
//------------------------------------------------------------------------------
class ICvPreGame1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPreGame1; }

	//TODO: This needs some MAJOR refactoring before it's production ready.
	// * Replace pascal casing w/ camel case
	// * Drop the STL containers for something DLL safe.
	// * Drop any of these that we aren't actually using.
	virtual PlayerTypes DLLCALL activePlayer() = 0;
	virtual int DLLCALL advancedStartPoints() = 0;
	virtual bool DLLCALL autorun() = 0; 
	virtual float DLLCALL autorunTurnDelay() = 0; 
	virtual int DLLCALL autorunTurnLimit() = 0; 
	virtual CalendarTypes DLLCALL calendar() = 0;
	virtual bool DLLCALL canReadyLocalPlayer() = 0;
	virtual CvString DLLCALL civilizationAdjective(PlayerTypes p) = 0;
	virtual const CvString DLLCALL civilizationAdjectiveKey(PlayerTypes p) = 0;
	virtual CvString DLLCALL civilizationDescription(PlayerTypes p) = 0;
	virtual const CvString DLLCALL civilizationDescriptionKey(PlayerTypes p) = 0;
	virtual CivilizationTypes DLLCALL civilization(PlayerTypes p) = 0;
	virtual const CvString DLLCALL civilizationPassword(PlayerTypes p) = 0;
	virtual CvString DLLCALL civilizationShortDescription(PlayerTypes p) = 0;
	virtual const CvString DLLCALL civilizationShortDescriptionKey(PlayerTypes p) = 0;
	virtual const CvString DLLCALL civilizationKey(PlayerTypes p) = 0;
	virtual bool DLLCALL civilizationKeyAvailable(PlayerTypes p) = 0;
	virtual const GUID DLLCALL  civilizationKeyPackageID(PlayerTypes p) = 0;
	virtual void DLLCALL clearDLCAllowed() = 0;
	virtual ClimateTypes DLLCALL climate() = 0;
	virtual void DLLCALL closeAllSlots() = 0;
	virtual void DLLCALL closeInactiveSlots() = 0;
	virtual const CvString DLLCALL emailAddress(PlayerTypes p) = 0;
	virtual const CvString DLLCALL emailAddress() = 0; 
	virtual float DLLCALL endTurnTimerLength() = 0; 
	virtual EraTypes DLLCALL era() = 0;
	virtual PlayerTypes DLLCALL findPlayerByNickname(const char * const name) = 0;
	virtual GameMode DLLCALL gameMode() = 0;
	virtual const CvString DLLCALL  gameName() = 0;
	virtual bool DLLCALL gameStarted() = 0;
	virtual uint DLLCALL GetGameOptionCount() = 0;
	virtual bool DLLCALL GetGameOption(uint uiIndex, char* szOptionNameBuffer, uint nOptionNameBufferSize, int& iValue) = 0;
	virtual bool DLLCALL GetGameOption(const char* szOptionName, int& iValue) = 0;
	virtual bool DLLCALL GetGameOption(GameOptionTypes eOption, int& iValue) = 0;
	virtual uint DLLCALL GetMapOptionCount() = 0;
	virtual bool DLLCALL GetMapOption(uint uiIndex, char* szOptionNameBuffer, uint nOptionNameBuffersize, int& iValue) = 0;
	virtual bool DLLCALL GetMapOption(const char* szOptionName, int& iValue) = 0;
	virtual bool DLLCALL GetPersistSettings() = 0;
	virtual GameSpeedTypes DLLCALL gameSpeed() = 0;
	virtual int DLLCALL gameTurn() = 0;
	virtual GameTypes DLLCALL gameType() = 0;
	virtual GameStartTypes DLLCALL gameStartType() = 0;
	virtual HandicapTypes DLLCALL handicap(PlayerTypes p) = 0;
	virtual bool DLLCALL isDLCAllowed(const GUID& kDLCID) = 0;
	virtual bool DLLCALL isDLCAvailable(PlayerTypes p, const GUID& kDLCID) = 0;
	virtual bool DLLCALL isEarthMap() = 0;
	virtual bool DLLCALL isHotSeat() = 0;
	virtual bool DLLCALL isHotSeatGame() = 0;
	virtual bool DLLCALL isHuman(PlayerTypes p) = 0;
	virtual bool DLLCALL isInternetGame() = 0;
	virtual bool DLLCALL isMinorCiv(PlayerTypes p) = 0;
	virtual bool DLLCALL isNetworkMultiplayerGame() = 0;
	virtual bool DLLCALL isPitBoss() = 0;
	virtual bool DLLCALL isPlayable(PlayerTypes p) = 0;
	virtual bool DLLCALL isPlayByEmail() = 0;
	virtual bool DLLCALL isReady(PlayerTypes p) = 0;
	virtual bool DLLCALL isVictory(VictoryTypes v) = 0;
	virtual bool DLLCALL isWBMapScript() = 0;
	virtual bool DLLCALL isWhiteFlag(PlayerTypes p) = 0;
	virtual LeaderHeadTypes DLLCALL leaderHead(PlayerTypes p) = 0;
	virtual CvString DLLCALL leaderName(PlayerTypes p) = 0;
	virtual const CvString DLLCALL leaderNameKey(PlayerTypes p) = 0;
	virtual const CvString DLLCALL leaderKey(PlayerTypes p) = 0;
	virtual bool DLLCALL leaderKeyAvailable(PlayerTypes p) = 0;
	virtual const GUID DLLCALL leaderKeyPackageID(PlayerTypes p) = 0;
	virtual const CvString DLLCALL loadFileName() = 0; 
	virtual void DLLCALL loadFromIni(FIGameIniParser& iniParser) = 0;
	virtual bool DLLCALL mapNoPlayers() = 0;
	virtual unsigned int DLLCALL mapRandomSeed() = 0;
	virtual bool DLLCALL loadWBScenario() = 0;
	virtual bool DLLCALL overrideScenarioHandicap() = 0;
	virtual const CvString DLLCALL mapScriptName() = 0;
	virtual int DLLCALL maxCityElimination() = 0;
	virtual int DLLCALL maxTurns() = 0;
	virtual MinorCivTypes DLLCALL minorCivType(PlayerTypes p) = 0;
	virtual bool DLLCALL multiplayerAIEnabled() = 0;
	virtual int DLLCALL netID(PlayerTypes p) = 0;
	virtual const CvString DLLCALL nickname(PlayerTypes p) = 0; 
	virtual const CvString DLLCALL nicknameDisplayed(PlayerTypes p) = 0;
	virtual int DLLCALL numMinorCivs() = 0;
	virtual PlayerColorTypes DLLCALL playerColor(PlayerTypes p) = 0;
	virtual bool DLLCALL privateGame() = 0;
	virtual bool DLLCALL quickCombat() = 0;
	virtual bool DLLCALL quickMovement() = 0;
	virtual bool DLLCALL quickstart() = 0; 
	virtual bool DLLCALL randomWorldSize() = 0; 
	virtual bool DLLCALL randomMapScript() = 0; 
	virtual void DLLCALL read(FDataStream& loadFrom, bool bReadVersion) = 0;
	virtual bool DLLCALL readPlayerSlotInfo(FDataStream& loadFrom, bool bReadVersion) = 0;
	virtual void DLLCALL resetGame() = 0;
	virtual void DLLCALL ResetGameOptions() = 0;
	virtual void DLLCALL ResetMapOptions() = 0;
	virtual void DLLCALL resetPlayer(PlayerTypes p) = 0;
	virtual void DLLCALL resetPlayers() = 0;
	virtual void DLLCALL resetSlots() = 0;
	virtual void DLLCALL restoreSlots() = 0;
	virtual void DLLCALL saveSlots() = 0;
	virtual SeaLevelTypes DLLCALL seaLevel() = 0;
	virtual void DLLCALL setActivePlayer(PlayerTypes p) = 0;
	virtual void DLLCALL setAdminPassword(const CvString & p) = 0;
	virtual void DLLCALL setAdvancedStartPoints(int a) = 0;
	virtual void DLLCALL setAlias(const CvString & a) = 0; 
	virtual void DLLCALL setAutorun(bool isAutoStart) = 0; 
	virtual void DLLCALL setAutorunTurnDelay(float turnDelay) = 0; 
	virtual void DLLCALL setAutorunTurnLimit(int turnLimit) = 0; 
	virtual void DLLCALL setBandwidth(BandwidthType b) = 0; 
	virtual void DLLCALL setBandwidth(const CvString & b) = 0; 
	virtual void DLLCALL setCalendar(CalendarTypes c) = 0;
	virtual void DLLCALL setCalendar(const CvString & c) = 0;
	virtual void DLLCALL setCivilization(PlayerTypes p, CivilizationTypes c) = 0;
	virtual void DLLCALL setCivilizationAdjective(PlayerTypes p, const CvString & a) = 0;
	virtual void DLLCALL setCivilizationDescription(PlayerTypes p, const CvString & d) = 0;
	virtual void DLLCALL setCivilizationPassword(PlayerTypes p, const CvString & pwd) = 0;
	virtual void DLLCALL setCivilizationShortDescription(PlayerTypes p, const CvString & d) = 0;
	virtual void DLLCALL setCivilizationKey(PlayerTypes p, const CvString & d) = 0;
	virtual void DLLCALL setCivilizationKeyPackageID(PlayerTypes p, const GUID & kKey) = 0;
	virtual void DLLCALL setClimate(ClimateTypes c) = 0;
	virtual void DLLCALL setClimate(const CvString & c) = 0;
	virtual void DLLCALL setCustomWorldSize(int iWidth, int iHeight, int iPlayers = 0, int iMinorCivs = 0) = 0;
	virtual void DLLCALL setDLCAllowed(const GUID& kDLCID, bool bState) = 0;
	virtual void DLLCALL setEarthMap(bool bIsEarthMap) = 0;
	virtual void DLLCALL setEmailAddress(PlayerTypes p, const CvString & a) = 0;
	virtual void DLLCALL setEmailAddress(const CvString & a) = 0; 
	virtual void DLLCALL setEndTurnTimerLength(float f) = 0; 
	virtual void DLLCALL setEra(EraTypes e) = 0;
	virtual void DLLCALL setEra(const CvString & e) = 0;
	virtual void DLLCALL setGameMode(GameMode g) = 0;
	virtual void DLLCALL setGameName(const CvString & g) = 0;
	virtual void DLLCALL setGameSpeed(GameSpeedTypes g) = 0;
	virtual void DLLCALL setGameSpeed(const CvString & g) = 0;
	virtual void DLLCALL setGameStarted(bool) = 0;
	virtual void DLLCALL setGameTurn(int turn) = 0;
	virtual bool DLLCALL SetGameOption(const char* szOptionName, int iValue) = 0;
	virtual bool DLLCALL SetGameOption(GameOptionTypes eOption, int iValue) = 0;
	virtual void DLLCALL setGameType(GameTypes g) = 0;
	virtual void DLLCALL setGameType(GameTypes g, GameStartTypes eStartType) = 0;
	virtual void DLLCALL setGameType(const CvString & g) = 0;
	virtual void DLLCALL setGameStartType(GameStartTypes g) = 0;
	virtual void DLLCALL setGameUpdateTime(int updateTime) = 0; 
	virtual void DLLCALL setHandicap(PlayerTypes p, HandicapTypes h) = 0;
	virtual void DLLCALL setInternetGame(bool isInternetGame) = 0;
	virtual void DLLCALL setLeaderHead(PlayerTypes p, LeaderHeadTypes l) = 0;
	virtual void DLLCALL setLeaderName(PlayerTypes p, const CvString& n) = 0;
	virtual void DLLCALL setLeaderKey(PlayerTypes p, const CvString& n) = 0;
	virtual void DLLCALL setLeaderKeyPackageID(PlayerTypes p, const GUID & n) = 0;
	virtual void DLLCALL setLoadFileName(const CvString & fileName) = 0;
	virtual void DLLCALL setMapNoPlayers(bool p) = 0;
	virtual void DLLCALL setMapRandomSeed(unsigned int newSeed) = 0;
	virtual void DLLCALL setLoadWBScenario(bool b) = 0;
	virtual void DLLCALL setOverrideScenarioHandicap(bool b) = 0;
	virtual void DLLCALL setMapScriptName(const CvString & s) = 0;
	virtual bool DLLCALL SetMapOption(const char* szOptionName, int iValue) = 0;
	virtual void DLLCALL setMaxCityElimination(int m) = 0;
	virtual void DLLCALL setMaxTurns(int maxTurns) = 0;
	virtual void DLLCALL setMinorCivType(PlayerTypes p, MinorCivTypes m) = 0;
	virtual void DLLCALL setMinorCiv(PlayerTypes p, bool isMinor) = 0;
	virtual void DLLCALL setMultiplayerAIEnabled(bool isEnabled) = 0;
	virtual void DLLCALL setNetID(PlayerTypes p, int id) = 0;
	virtual void DLLCALL setNickname(PlayerTypes p, const CvString& n) = 0; 
	virtual void DLLCALL setNumMinorCivs(int n) = 0;
	virtual void DLLCALL setPersistSettings(bool bPersist) = 0;
	virtual void DLLCALL setPlayable(PlayerTypes p, bool playable) = 0;
	virtual void DLLCALL setPlayerColor(PlayerTypes p, PlayerColorTypes c) = 0;
	virtual void DLLCALL setPrivateGame(bool isPrivateGame) = 0;
	virtual void DLLCALL setQuickCombat(bool isQuickCombat) = 0; 
	virtual void DLLCALL setQuickMovement(bool isQuickCombat) = 0; 
	virtual void DLLCALL setQuickHandicap(HandicapTypes h) = 0; 
	virtual void DLLCALL setQuickHandicap(const CvString & h) = 0; 
	virtual void DLLCALL setQuickstart(bool isQuickStart) = 0; 
	virtual void DLLCALL setRandomWorldSize(bool isRandomWorldSize) = 0; 
	virtual void DLLCALL setRandomMapScript(bool isRandomWorldScript) = 0; 
	virtual void DLLCALL setReady(PlayerTypes p, bool bIsReady) = 0;
	virtual void DLLCALL setSeaLevel(SeaLevelTypes s) = 0;
	virtual void DLLCALL setSeaLevel(const CvString & s) = 0;
	virtual void DLLCALL setSlotClaim(PlayerTypes p, SlotClaim c) = 0;
	virtual void DLLCALL setSlotStatus(PlayerTypes eID, SlotStatus eSlotStatus) = 0;
	virtual void DLLCALL setSyncRandomSeed(unsigned int newSeed) = 0;
	virtual void DLLCALL setTeamType(PlayerTypes p, TeamTypes t) = 0;
	virtual void DLLCALL setTransferredMap(bool transferred) = 0; 
	virtual void DLLCALL setTurnTimer(TurnTimerTypes t) = 0;
	virtual void DLLCALL setTurnTimer(const CvString & t) = 0;
	virtual void DLLCALL SetCityScreenBlocked(bool bCityScreenBlocked) = 0;
	virtual void DLLCALL setVersionString(const std::string & v) = 0;
	virtual void DLLCALL setVictory(VictoryTypes v, bool isValid) = 0;
	virtual void DLLCALL setVictories(const std::vector<bool> & v) = 0;
	virtual void DLLCALL setWhiteFlag(PlayerTypes p, bool flag) = 0;
	virtual void DLLCALL setWorldSize(WorldSizeTypes w, bool bResetSlots=true) = 0;
	virtual void DLLCALL setWorldSize(const CvString& w) = 0;
	virtual SlotClaim DLLCALL slotClaim(PlayerTypes p) = 0;
	virtual SlotStatus DLLCALL slotStatus(PlayerTypes eID) = 0;
	virtual unsigned int DLLCALL syncRandomSeed() = 0;
	virtual bool DLLCALL transferredMap() = 0;
	virtual TeamTypes DLLCALL teamType(PlayerTypes p) = 0;
	virtual TurnTimerTypes DLLCALL turnTimer() = 0;
	virtual const std::string DLLCALL versionString() = 0;
	virtual WorldSizeTypes DLLCALL worldSize() = 0;
	virtual ICvWorldInfo1* DLLCALL GetWorldInfo() = 0;
	virtual void DLLCALL write(FDataStream& saveTo) = 0;
	virtual int DLLCALL getActiveSlotCount() = 0;
	virtual int DLLCALL readActiveSlotCountFromSaveGame(FDataStream& loadFrom, bool bReadVersion) = 0;
	virtual StorageLocation DLLCALL LoadFileStorage() = 0;
	virtual void DLLCALL SetLoadFileName(const char* szFileName, StorageLocation eStorage) = 0;

	virtual ICvEnumerator* DLLCALL GetDLCAllowed() = 0;
	virtual ICvEnumerator* DLLCALL GetDLCAvailable(PlayerTypes p) = 0;
	virtual void DLLCALL SetDLCAvailable(PlayerTypes p, ICvEnumerator* pList) = 0;
};

//------------------------------------------------------------------------------
// Random interfaces
//------------------------------------------------------------------------------
class ICvRandom1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvRandom1; }

	virtual void DLLCALL Init(unsigned long ulSeed) = 0;
	virtual void DLLCALL Reset(unsigned long ulSeed = 0) = 0;

	virtual void DLLCALL CopyFrom(ICvRandom1* pOther) = 0;

	virtual unsigned short DLLCALL Get(unsigned short usNum, const char* pszLog = NULL) = 0;  //  Returns value from 0 to num-1 inclusive.
	virtual float DLLCALL GetFloat() = 0;

	virtual unsigned long DLLCALL GetSeed() const = 0;
	
	virtual void DLLCALL Read(FDataStream& Stream) = 0;
	virtual void DLLCALL Write(FDataStream& Stream) const = 0;
};

//------------------------------------------------------------------------------
// Team interfaces
//------------------------------------------------------------------------------
class ICvTeam1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvTeam1; }

	virtual bool DLLCALL CanEmbarkAllWaterPassage() const = 0;
	virtual int DLLCALL GetAtWarCount(bool bIgnoreMinors) const = 0;
	virtual EraTypes DLLCALL GetCurrentEra() const = 0;
	virtual PlayerTypes DLLCALL GetLeaderID() const = 0;
	virtual int DLLCALL GetProjectCount(ProjectTypes eIndex) const = 0;
	virtual int DLLCALL GetTotalSecuredVotes() const = 0;
	virtual void DLLCALL Init(TeamTypes eID) = 0;
	virtual bool DLLCALL IsAlive() const = 0;
	virtual bool DLLCALL IsAtWar(TeamTypes eIndex) const = 0;
	virtual bool DLLCALL IsBarbarian() const = 0;
	virtual bool DLLCALL IsBridgeBuilding() const = 0;
	virtual bool DLLCALL IsHasMet(TeamTypes eIndex) const = 0;
	virtual bool DLLCALL IsHomeOfUnitedNations() const = 0;
	virtual void DLLCALL Uninit() = 0;
	virtual void DLLCALL Read(FDataStream& kStream) = 0;
	virtual void DLLCALL Write(FDataStream& kStream) const = 0;

	// Techs
	//! Get the number of Techs the player has fully researched
	virtual int DLLCALL GetTechCount() const = 0;
	//! Get all the techs the player has researched
	virtual int DLLCALL GetTechs(TechTypes* pkTechArray, uint uiArraySize) const = 0;
};

//------------------------------------------------------------------------------
// Tech Info interfaces
//------------------------------------------------------------------------------
class ICvTechInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvTechInfo1; }

	virtual const char* DLLCALL GetDescription() const = 0;
	virtual const char* DLLCALL GetType() const = 0;
	virtual const char* DLLCALL GetText() const = 0;
	virtual int DLLCALL GetEra() const = 0;						
	virtual const char* DLLCALL GetSound() const = 0;			
	virtual const char* DLLCALL GetSoundMP() const = 0;
};

//------------------------------------------------------------------------------
// Unit Interfaces
//------------------------------------------------------------------------------
class ICvUnit1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvUnit1; }

	virtual int DLLCALL GetID() const = 0;

	virtual PlayerTypes DLLCALL GetOwner() const = 0;

	virtual int DLLCALL GetDamage() const = 0;
	virtual int DLLCALL GetMoves() const = 0;
	virtual bool DLLCALL IsSelected() const = 0;
	virtual int DLLCALL GetMaxHitPoints() const = 0;

	virtual CivilizationTypes DLLCALL GetCivilizationType() const = 0;
	virtual TeamTypes DLLCALL GetTeam() const = 0;
	virtual DomainTypes DLLCALL GetDomainType() const = 0;

	virtual bool DLLCALL IsCombatUnit() const = 0;
	virtual bool DLLCALL IsBarbarian() const = 0;
	virtual bool DLLCALL IsHoveringUnit() const = 0;
	virtual bool DLLCALL IsInvisible(TeamTypes eTeam, bool bDebug) const = 0;

	virtual bool DLLCALL CanMoveImpassable() const = 0;
	virtual DirectionTypes DLLCALL GetFacingDirection(bool checkLineOfSightProperty) const = 0;

	virtual ICvPlot1* DLLCALL GetPlot() const = 0;

	virtual ICvUnitInfo1* DLLCALL GetUnitInfo() const = 0;

	virtual bool DLLCALL IsEmbarked() const = 0;
	virtual bool DLLCALL IsGarrisoned() const = 0;

	virtual ICvMissionData1* DLLCALL GetHeadMissionData() const = 0;

	virtual UnitCombatTypes DLLCALL GetUnitCombatType() = 0;

	virtual int DLLCALL GetX() const = 0;
	virtual int DLLCALL GetY() const = 0;
	virtual void DLLCALL GetPosition(int& iX, int& iY) const = 0;

	virtual bool DLLCALL CanSwapWithUnitHere(_In_ ICvPlot1* pPlot) const = 0;
	virtual bool DLLCALL CanEmbarkOnto(_In_ ICvPlot1* pOriginPlot, _In_ ICvPlot1* pTargetPlot, bool bOverrideEmbarkedCheck = false) const = 0;
	virtual bool DLLCALL CanDisembarkOnto(_In_ ICvPlot1* pOriginPlot, _In_ ICvPlot1* pTargetPlot, bool bOverrideEmbarkedCheck = false) const = 0;

	virtual bool DLLCALL IsFirstTimeSelected () const = 0;
	virtual UnitTypes DLLCALL GetUnitType() const = 0;
	virtual bool DLLCALL IsDelayedDeathExported() const = 0;
	virtual bool DLLCALL IsBusy() const = 0;

	virtual void DLLCALL SetupGraphical() = 0;

	virtual CvString DLLCALL GetName() const = 0;

	virtual void DLLCALL DoCommand(CommandTypes eCommand, int iData1, int iData2) = 0;

	virtual bool DLLCALL CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility = false) = 0;
	virtual IDInfo DLLCALL GetIDInfo() const = 0;
	virtual int DLLCALL MovesLeft() const = 0;
	virtual bool DLLCALL CanMoveInto(ICvPlot1* pPlot, byte bMoveFlags = 0) const = 0;
	virtual TeamTypes DLLCALL GetDeclareWarMove(ICvPlot1* pPlot) const = 0;
	virtual bool DLLCALL ReadyToSelect() const = 0;

	virtual bool DLLCALL CanWork() const = 0;
	virtual bool DLLCALL CanFound() const = 0;

	virtual FAutoArchive& DLLCALL GetSyncArchive() = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Unit Info Interfaces
//------------------------------------------------------------------------------
class ICvUnitInfo1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvUnitInfo1; }

	virtual int DLLCALL GetCombat() const = 0;		
	virtual int DLLCALL GetDomainType() const = 0;
	virtual const char* DLLCALL GetType() const = 0;
	virtual const char* DLLCALL GetText() const = 0;
	virtual UnitMoveRate DLLCALL GetMoveRate(int numHexes) const = 0;
	virtual const char* DLLCALL GetUnitArtInfoTag() const = 0;
	virtual bool DLLCALL GetUnitArtInfoCulturalVariation() const = 0;
	virtual bool DLLCALL GetUnitArtInfoEraVariation() const = 0;
	virtual int DLLCALL GetUnitFlagIconOffset() const = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// WorldBuilder Map Loader Interfaces
//------------------------------------------------------------------------------
class ICvWorldBuilderMapLoader1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvWorldBuilderMapLoader1; }

	virtual const CvWorldBuilderMapLoaderMapInfo& DLLCALL GetCurrentMapInfo() = 0;
	virtual bool DLLCALL Preload( _In_z_ const wchar_t* wszFilename, bool bScenario) = 0;
	virtual void DLLCALL SetupGameOptions() = 0;
	virtual void DLLCALL SetupPlayers() = 0;
	virtual void DLLCALL SetInitialItems(bool bFirstCall) = 0;
	virtual bool DLLCALL InitMap() = 0;
	virtual bool DLLCALL Save( _In_z_ const wchar_t* wszFilename, const char *szMapName = NULL) = 0;
	virtual int DLLCALL LoadModData(lua_State *L) = 0;
	virtual void DLLCALL ValidateTerrain() = 0;
	virtual void DLLCALL ValidateCoast() = 0;
	virtual void DLLCALL ClearResources() = 0;
	virtual void DLLCALL ClearGoodies() = 0;
	virtual void DLLCALL ResetPlayerSlots() = 0;
	virtual void DLLCALL MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot) = 0;
	virtual unsigned int DLLCALL PreviewPlayableCivCount( _In_z_ const wchar_t* wszFilename) = 0;
	virtual int DLLCALL GetMapPreview(lua_State *L) = 0;
	virtual int DLLCALL GetMapPlayers(lua_State *L) = 0;
	virtual int DLLCALL AddRandomItems(lua_State *L) = 0;
	virtual int DLLCALL ScatterResources(lua_State *L) = 0;
	virtual int DLLCALL ScatterGoodies(lua_State *L) = 0;

	virtual PlayerTypes DLLCALL GetMapPlayerSlot(uint uiPlayer) = 0;
	virtual int DLLCALL GetMapPlayerCount() = 0;


	virtual void DLLCALL GenerateRandomMap(const char* szMapScript) = 0;
};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Templates
//------------------------------------------------------------------------------
template<typename T>
class CvEnumerator
{
public:
	CvEnumerator(ICvEnumerator* pEnumerator)
	: m_Enumerator(pEnumerator)
	{}
	
	bool MoveNext()
	{
		return (m_Enumerator.get() != NULL)? m_Enumerator->MoveNext() : false;
	}

	void Reset()
	{
		if(m_Enumerator.get())
			m_Enumerator->Reset();
	}

	T* GetCurrent()
	{
		if(m_Enumerator.get() != NULL)
		{
			auto_ptr<ICvUnknown> pValue(m_Enumerator->GetCurrent());
			return (pValue.get() != NULL)? pValue->QueryInterface<T>() : NULL;
		}
		else
		{
			return NULL;
		}	
	}

private:
	std::auto_ptr<ICvEnumerator> m_Enumerator;
};

// Include any newer interfaces

#include "CvDllInterfaces2.h"
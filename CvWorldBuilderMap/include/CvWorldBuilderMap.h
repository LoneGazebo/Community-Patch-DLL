//---------------------------------------------------------------------------------------
//
//  *****************   CIV 5 World Builder Map   ********************
//
//  FILE:    CvWorldBuilderMap.h
//
//  AUTHOR:  Eric Jordan  --  7/20/2009
//
//  PURPOSE:
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#pragma once
#ifndef CvWorldBuilderMap_h
#define CvWorldBuilderMap_h

#include "FRelationshipBitset.h"
#include "CvWorldBuilderMapElementAllocator.h"
#include "CvWorldBuilderMapModData.h"
#include "CvWorldBuilderMapTypeGroup.h"
#include "FireWorks/FUncopyable.h"

class FIFile;
class CvWorldBuilderMapTypeDesc;

class CvWorldBuilderMap : FUncopyable
{
public:
	static const uint FileVersion = 12;
	static const uint MaxMapSize = 32768;
	static const uint MaxPlayers = 32;
	static const uint MaxCityStates = 64;
	static const uint MaxTeams = MaxPlayers + MaxCityStates;
	static const uint BarbarianPlayer = MaxPlayers + MaxCityStates;

	CvWorldBuilderMap();
	~CvWorldBuilderMap();

	void Resize(uint uiWidth, uint uiHeight);

#ifndef CV_WORLD_BUILDER_MAP_IN_GAME
	MEMBER_EVENT2(MapResized, uint, uint); // uiWidth, uiHeight
#endif

	// Load / Save
	bool Save( _In_z_ const wchar_t* wszFilename, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario = true) const;
	bool Load( _In_z_ const wchar_t* wszFilename, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario = true);

	inline uint GetWidth() const { return m_kMapDesc.uiWidth; }
	inline uint GetHeight() const { return m_kMapDesc.uiHeight; }
	inline uint GetPlotCount() const { return m_kMapDesc.uiWidth * m_kMapDesc.uiHeight; }

	class PlotMapData
	{
	public:
		PlotMapData();

		__forceinline byte GetTerrainType() const { return m_byTerrainType; }
		__forceinline void SetTerrainType(byte byType) { m_byTerrainType = byType; }

		__forceinline byte GetContinentType() const { return m_byContinentType; }
		__forceinline void SetContinentType(byte byType) { m_byContinentType = byType; }

		static const byte InvalidResource = byte(-1);
		__forceinline byte GetResourceType() const { return m_byResourceType; }
		__forceinline void SetResourceType(byte byType) { m_byResourceType = byType; }

		__forceinline byte GetResourceAmount() const { return m_byResourceAmount; }
		__forceinline void SetResourceAmount(byte byAmount) { m_byResourceAmount = byAmount; }

		static const byte InvalidFeature = byte(-1);
		__forceinline byte GetFeatureType() const { return m_byFeatureType; }
		__forceinline void SetFeatureType(byte byType) { m_byFeatureType = byType; }

		static const byte InvalidNaturalWonder = byte(-1);
		__forceinline byte GetNaturalWonderType() const { return m_byNaturalWonderType; }
		__forceinline void SetNaturalWonderType(byte byType) { m_byNaturalWonderType = byType; }

		enum PlotHeight
		{
			FLAT_TERRAIN = 0,
			HILLS = 1,
			MOUNTAINS = 2,
		};

		__forceinline PlotHeight GetPlotHeight() const { return (PlotHeight)m_byPlotHeight; }
		__forceinline void SetPlotHeight(PlotHeight eHeight) { m_byPlotHeight = (byte)eHeight; }

		enum PlotFlags
		{
			// Rivers
			W_OF_RIVER    = 0x01,
			NW_OF_RIVER   = 0x02,
			NE_OF_RIVER   = 0x04,
			RIVER_FLOW_S  = 0x08,
			RIVER_FLOW_NE = 0x10,
			RIVER_FLOW_NW = 0x20,

			// Starting Positions
			START_POS_MAJOR = 0x40,
			START_POS_MINOR = 0x80,
		};

		__forceinline bool GetFlag(PlotFlags eFlag) const { return (m_byFlags & eFlag) != 0; }
		__forceinline void SetFlag(PlotFlags eFlag) { m_byFlags |= eFlag; }
		__forceinline void ClearFlag(PlotFlags eFlag) { m_byFlags &= ~eFlag; } 
		__forceinline void SetFlag(PlotFlags eFlag, bool bValue)
		{
			if( bValue ) m_byFlags |= eFlag;
			else m_byFlags &= ~eFlag;
		}

		__forceinline byte GetFlags() const { return m_byFlags; }
		__forceinline void SetFlags(byte byFlags) { m_byFlags = byFlags; }

	private:
		// Note: member data forms qword
		byte m_byTerrainType;
		byte m_byResourceType;
		byte m_byFeatureType;
		byte m_byFlags;
		byte m_byPlotHeight;
		byte m_byContinentType;
		byte m_byNaturalWonderType;
		byte m_byResourceAmount;
	};

	// Backwards Compatibility Only.  Not used for anything but loading old maps.
	struct OldUnit
	{
		static const byte InvalidUnitType = byte(-1);
		typedef CvWorldBuilderMapElementAllocator<OldUnit, 16> Map;
		typedef Map::Handle Handle;

		OldUnit();

		enum DirectionTypes
		{
			DIRECTION_RANDOM,
			DIRECTION_NORTHEAST,
			DIRECTION_EAST,
			DIRECTION_SOUTHEAST,
			DIRECTION_SOUTHWEST,
			DIRECTION_WEST,
			DIRECTION_NORTHWEST,
		};

		Handle m_hStackedUnit;
		word m_wExperience;
		float m_fHealth;
		byte m_byUnitType;
		byte m_byOwner;
		byte m_byDirection;
		bool m_bFortified : 1;
		bool m_bEmbarked : 1;
	};

	// Just a fixed buffer for a unit name.  I want to package these together in a serializable
	// block allocator which is why they aren't variable-length heap allocations.
	struct UnitName
	{
		typedef CvWorldBuilderMapElementAllocator<UnitName, 4> Map;
		typedef Map::Handle Handle;

		static const uint MaxLength = 64;
		char m_szText[MaxLength];

		UnitName() { m_szText[0] = 0; }
	};

	struct UnitV2
	{
		static const byte InvalidUnitType = byte(-1);
		typedef CvWorldBuilderMapElementAllocator<UnitV2, 16> Map;
		typedef Map::Handle Handle;

		static const uint MaxPromotionTypes = 256;
		typedef std::bitset<MaxPromotionTypes> PromotionBitset;

		static const uint MaxHealth = 100000;

		UnitV2();

		enum DirectionTypes
		{
			DIRECTION_RANDOM,
			DIRECTION_NORTHEAST,
			DIRECTION_EAST,
			DIRECTION_SOUTHEAST,
			DIRECTION_SOUTHWEST,
			DIRECTION_WEST,
			DIRECTION_NORTHWEST,
		};

		enum Flags
		{
			UNIT_FORTIFIED  = 0x01,
			UNIT_EMBARKED   = 0x02,
			UNIT_GARRISONED = 0x04
		};

		Handle m_hStackedUnit;
		UnitName::Handle m_hCustomName;
		uint m_uiExperience;
		uint m_uiHealth;
		byte m_byUnitType;
		byte m_byOwner;
		byte m_byDirection;
		byte m_byFlags;

		PromotionBitset m_kPromotions;
	};

	struct Unit
	{
		static const uint InvalidUnitType = uint(-1);
		typedef CvWorldBuilderMapElementAllocator<Unit, 16> Map;
		typedef Map::Handle Handle;

		static const uint MaxPromotionTypes = 512;
		typedef std::bitset<MaxPromotionTypes> PromotionBitset;

		static const uint MaxHealth = 100000;

		Unit();
		explicit Unit(const OldUnit &kOldUnit); // For conversion
		explicit Unit(const UnitV2 &kOldUnit); // For conversion

		enum DirectionTypes
		{
			DIRECTION_RANDOM,
			DIRECTION_NORTHEAST,
			DIRECTION_EAST,
			DIRECTION_SOUTHEAST,
			DIRECTION_SOUTHWEST,
			DIRECTION_WEST,
			DIRECTION_NORTHWEST,
		};

		enum Flags
		{
			UNIT_FORTIFIED  = 0x01,
			UNIT_EMBARKED   = 0x02,
			UNIT_GARRISONED = 0x04
		};

		typedef uint UnitType;
		Handle m_hStackedUnit;
		UnitName::Handle m_hCustomName;
		uint m_uiExperience;
		uint m_uiHealth;
		UnitType m_byUnitType;
		byte m_byOwner;
		byte m_byDirection;
		byte m_byFlags;

		PromotionBitset m_kPromotions;

		// Remember floating point error if you choose to use these
		void SetHealthAsFloat(const float fHealth) { m_uiHealth = (uint)(fHealth * MaxHealth); }
		const float GetHealthAsFloat() const { return (float)m_uiHealth / (float)MaxHealth; }

		__forceinline bool GetFlag(Flags eFlag) const { return (m_byFlags & eFlag) != 0; }
		__forceinline void SetFlag(Flags eFlag) { m_byFlags |= eFlag; }
		__forceinline void ClearFlag(Flags eFlag) { m_byFlags &= ~eFlag; } 
		__forceinline void SetFlag(Flags eFlag, bool bValue)
		{
			if( bValue ) m_byFlags |= eFlag;
			else m_byFlags &= ~eFlag;
		}
	};

	// Backwards Compatibility Only.  Not used for anything but loading old maps.
	struct OldCity
	{
		typedef CvWorldBuilderMapElementAllocator<OldCity, 8> Map;
		typedef Map::Handle Handle;

		static const uint MaxBuildingTypes = 256;
		typedef std::bitset<MaxBuildingTypes> BuildingBitset;

		OldCity();

		char m_szName[56];
		uint m_uiPopulation;
		uint m_uiCulture;
		byte m_byOwner;
		bool m_bLocalizedName;
		BuildingBitset m_kBuildings;
	};

	struct CityV2
	{
		typedef CvWorldBuilderMapElementAllocator<CityV2, 8> Map;
		typedef Map::Handle Handle;

		static const uint MaxBuildingTypes = 256;
		typedef std::bitset<MaxBuildingTypes> BuildingBitset;

		static const uint MaxNameLength = 64;

		static const uint MaxHealth = 100000;

		enum Flags
		{
			CITY_LOCALIZED_NAME = 0x01,
			CITY_PUPPET_STATE   = 0x02,
			CITY_OCCUPIED       = 0x04
		};

		CityV2();

		char m_szName[MaxNameLength];
		byte m_byOwner;
		byte m_byFlags;
		word m_wPopulation;
		uint m_uiHealth;
		BuildingBitset m_kBuildings;
	};

	struct City
	{
		typedef CvWorldBuilderMapElementAllocator<City, 8> Map;
		typedef Map::Handle Handle;

		static const uint MaxBuildingTypes = 512;
		typedef std::bitset<MaxBuildingTypes> BuildingBitset;

		static const uint MaxNameLength = 64;

		static const uint MaxHealth = 100000;

		enum Flags
		{
			CITY_LOCALIZED_NAME = 0x01,
			CITY_PUPPET_STATE   = 0x02,
			CITY_OCCUPIED       = 0x04
		};

		City();
		explicit City(const OldCity &kOldCity); // For conversion
		explicit City(const CityV2 &kOldCity); // For conversion

		char m_szName[MaxNameLength];
		byte m_byOwner;
		byte m_byFlags;
		word m_wPopulation;
		uint m_uiHealth;
		BuildingBitset m_kBuildings;

		// Remember floating point error if you choose to use these
		void SetHealthAsFloat(const float fHealth) { m_uiHealth = (uint)(fHealth * MaxHealth); }
		const float GetHealthAsFloat() const { return (float)m_uiHealth / (float)MaxHealth; }

		__forceinline bool GetFlag(Flags eFlag) const { return (m_byFlags & eFlag) != 0; }
		__forceinline void SetFlag(Flags eFlag) { m_byFlags |= eFlag; }
		__forceinline void ClearFlag(Flags eFlag) { m_byFlags &= ~eFlag; } 
		__forceinline void SetFlag(Flags eFlag, bool bValue)
		{
			if( bValue ) m_byFlags |= eFlag;
			else m_byFlags &= ~eFlag;
		}
	};

	struct PlotScenarioData
	{
		static const byte InvalidCulture = byte(-1);
		static const byte InvalidImprovement = byte(-1);
		static const byte InvalidRoute = byte(-1);

		PlotScenarioData();

		// Note: member data forms qword
		City::Handle m_hCity;
		Unit::Handle m_hUnitStack;
		byte m_byCulture;
		byte m_byImprovement;
		byte m_byRoute;
		byte m_byRouteOwner;
	};

	struct Team
	{
		Team();

		static const uint MaxTechs = 256;
		typedef std::bitset<MaxTechs> TechBitset;

		char m_szName[32];

		TechBitset m_kTechs;
	};

	// Backwards Compatibility Only.  Not used for anything but loading old maps.
	struct OldPlayer
	{
		static const uint MaxPolicies = 256;
		static const byte InvalidTeam = byte(-1);
		typedef std::bitset<MaxPolicies> PolicyBitset;

		OldPlayer();

		PolicyBitset m_kPolicies;

		char m_szCivType[64];
		char m_szTeamColor[64];
		char m_szEra[64];
		char m_szHandicap[60];

		uint m_uiCulture;
		uint m_uiGold;
		uint m_uiStartX, m_uiStartY;

		byte m_byTeam;
		bool m_bPlayable;
	};

	struct Player
	{
		static const uint MaxPolicies = 256;
		static const byte InvalidTeam = byte(-1);
		typedef std::bitset<MaxPolicies> PolicyBitset;

		Player();
		explicit Player(const OldPlayer &kOldPlayer); // Conversion

		PolicyBitset m_kPolicies;

		char m_szLeaderName[64];
		char m_szCivName[64];
		char m_szCivType[64];
		char m_szTeamColor[64];
		char m_szEra[64];
		char m_szHandicap[64];

		uint m_uiCulture;
		uint m_uiGold;
		uint m_uiStartX, m_uiStartY;

		byte m_byTeam;
		bool m_bPlayable;
	};

	void SetAllPlotData(const PlotMapData &kPlotData, const PlotScenarioData &kScenarioData);
	void ClearScenarioData(bool bPreserveImprovements = false);

	inline const uint GetPlotIndex(uint uiX, uint uiY) const
	{
		FAssertMsg(uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight, "Invalid Position");
		return uiX + uiY * m_kMapDesc.uiWidth;
	}

	inline const PlotMapData &GetPlotData(uint uiX, uint uiY) const
	{
		if( uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight )
		{
			return m_akMapData[uiX + uiY * m_kMapDesc.uiWidth];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotMapData;
		}
	}

	inline PlotMapData &GetPlotData(uint uiX, uint uiY)
	{
		if( uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight )
		{
			return m_akMapData[uiX + uiY * m_kMapDesc.uiWidth];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotMapData;
		}
	}

	inline const PlotMapData &GetPlotData(uint uiIndex) const
	{
		if( uiIndex < m_kMapDesc.uiWidth * m_kMapDesc.uiHeight && uiIndex < MaxMapSize )
		{
			return m_akMapData[uiIndex];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotMapData;
		}
	}

	inline PlotMapData &GetPlotData(uint uiIndex)
	{
		if( uiIndex < m_kMapDesc.uiWidth * m_kMapDesc.uiHeight && uiIndex < MaxMapSize )
		{
			return m_akMapData[uiIndex];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotMapData;
		}
	}

	inline const PlotScenarioData &GetPlotScenarioData(uint uiX, uint uiY) const
	{
		if( uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight )
		{
			return m_akScenarioData[uiX + uiY * m_kMapDesc.uiWidth];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotScenarioData;
		}
	}

	inline PlotScenarioData &GetPlotScenarioData(uint uiX, uint uiY)
	{
		if( uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight )
		{
			return m_akScenarioData[uiX + uiY * m_kMapDesc.uiWidth];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotScenarioData;
		}
	}

	inline const PlotScenarioData &GetPlotScenarioData(uint uiIndex) const
	{
		if( uiIndex < m_kMapDesc.uiWidth * m_kMapDesc.uiHeight && uiIndex < MaxMapSize )
		{
			return m_akScenarioData[uiIndex];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotScenarioData;
		}
	}

	inline PlotScenarioData &GetPlotScenarioData(uint uiIndex)
	{
		if( uiIndex < m_kMapDesc.uiWidth * m_kMapDesc.uiHeight && uiIndex < MaxMapSize )
		{
			return m_akScenarioData[uiIndex];
		}
		else
		{
			FAssertMsg(false, "Invalid Position");
			return sm_kErrorPlotScenarioData;
		}
	}

	inline const byte GetPlayerCount() const { return m_kGameDesc.byPlayerCount; }
	inline void SetPlayerCount(byte byCount)
	{
		FAssertMsg(byCount < MaxPlayers, "Too many players");
		if( byCount < MaxPlayers )
			m_kGameDesc.byPlayerCount = byCount;
	}

	inline const byte GetCityStateCount() const { return m_kGameDesc.byCityStateCount; }
	inline void SetCityStateCount(byte byCount)
	{
		FAssertMsg(byCount < MaxCityStates, "Too many city states");
		if( byCount < MaxCityStates )
			m_kGameDesc.byCityStateCount = byCount;
	}

	inline const byte GetTeamCount() const { return m_kGameDesc.byTeamCount; }
	inline void SetTeamCount(byte byCount)
	{
		FAssertMsg(byCount < MaxTeams, "Too many teams");
		if( byCount < MaxTeams )
			m_kGameDesc.byTeamCount = byCount;
	}

	inline Player &GetPlayer(byte byPlayer)
	{
		if( byPlayer < m_kGameDesc.byPlayerCount && byPlayer < MaxPlayers )
		{
			return m_akPlayers[byPlayer];
		}
		else
		{
			FAssertMsg(false, "Invalid Player");
			return sm_kErrorPlayer;
		}
	}

	inline const Player &GetPlayer(byte byPlayer) const
	{
		if( byPlayer < m_kGameDesc.byPlayerCount && byPlayer < MaxPlayers )
		{
			return m_akPlayers[byPlayer];
		}
		else
		{
			FAssertMsg(false, "Invalid Player");
			return sm_kErrorPlayer;
		}
	}

	inline Player &GetCityState(byte byPlayer)
	{
		if( byPlayer < m_kGameDesc.byCityStateCount && byPlayer < MaxCityStates )
		{
			return m_akCityStates[byPlayer];
		}
		else
		{
			FAssertMsg(false, "Invalid City State");
			return sm_kErrorPlayer;
		}
	}

	inline const Player &GetCityState(byte byPlayer) const
	{
		if( byPlayer < m_kGameDesc.byCityStateCount && byPlayer < MaxCityStates )
		{
			return m_akCityStates[byPlayer];
		}
		else
		{
			FAssertMsg(false, "Invalid City State");
			return sm_kErrorPlayer;
		}
	}

	inline Team &GetTeam(byte byTeam)
	{
		if( byTeam < m_kGameDesc.byTeamCount && byTeam < MaxTeams )
		{
			return m_akTeams[byTeam];
		}
		else
		{
			FAssertMsg(false, "Invalid Team");
			return sm_kErrorTeam;
		}
	}

	inline const Team &GetTeam(byte byTeam) const
	{
		if( byTeam < m_kGameDesc.byTeamCount && byTeam < MaxTeams )
		{
			return m_akTeams[byTeam];
		}
		else
		{
			FAssertMsg(false, "Invalid Team");
			return sm_kErrorTeam;
		}
	}

	Unit::Handle AddUnit(uint uiX, uint uiY);
	bool RemoveUnit(uint uiX, uint uiY, Unit::Handle &hUnit);

	inline const bool GetVisibility(uint uiX, uint uiY, uint uiTeam) const
	{
		FAssertMsg(uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight, "Invalid Position");
		FAssertMsg(uiTeam < m_kGameDesc.byTeamCount, "Invalid Team");
		const uint uiMapSize = m_kMapDesc.uiWidth * m_kMapDesc.uiHeight;
		return m_kVisibility.Get(uiX + uiY * m_kMapDesc.uiWidth + uiTeam * uiMapSize);
	}

	inline void SetVisibility(uint uiX, uint uiY, uint uiTeam, bool bVisible)
	{
		FAssertMsg(uiX < m_kMapDesc.uiWidth && uiY < m_kMapDesc.uiHeight, "Invalid Position");
		FAssertMsg(uiTeam < m_kGameDesc.byTeamCount, "Invalid Team");
		const uint uiMapSize = m_kMapDesc.uiWidth * m_kMapDesc.uiHeight;
		m_kVisibility.Set(uiX + uiY * m_kMapDesc.uiWidth + uiTeam * uiMapSize, bVisible);
	}

	void SetDefaultGameSpeed( _In_z_ const char *szSpeedType);
	inline const char *GetDefaultGameSpeed() const { return m_kGameDesc.szDefaultSpeed; }

	void SetWorldType( _In_z_ const char* szWorldType);
	inline const char* GetWorldType() const {return m_szWorldType;}

	inline void SetStartYear(int iYear) { m_kGameDesc.iStartYear = iYear; }
	inline int GetStartYear() const { return m_kGameDesc.iStartYear; }

	inline void SetMaxTurns(uint uiMaxTurns) { m_kGameDesc.uiMaxTurns = uiMaxTurns; }
	inline uint GetMaxTurns() const { return m_kGameDesc.uiMaxTurns; }

	void SwapPlayers(byte byPlayer1, byte byPlayer2);
	void SwapCityStates(byte byCityState1, byte byCityState2);

	FRelationshipBitset<MaxTeams, true>  m_kTeamsInContact;
	FRelationshipBitset<MaxTeams, false> m_kTeamsAtWar;
	FRelationshipBitset<MaxTeams, true>  m_kTeamsAtPermanentWarOrPeace;
	FRelationshipBitset<MaxTeams, true>  m_kTeamsSharingOpenBorders;
	FRelationshipBitset<MaxTeams, true>  m_kTeamsSharingDefensivePacts;

	Unit::Map m_kUnits;
	UnitName::Map m_kUnitNames;
	City::Map m_kCities;

	CvWorldBuilderMapModData m_kModData;

	FStringA m_sMapName;
	FStringA m_sMapDescription;

	CvWorldBuilderMapTypeGroup m_kVictoryTypes;
	CvWorldBuilderMapTypeGroup m_kGameOptions;

	int m_aiCityStateInfluence[MaxPlayers][MaxCityStates];

	static uint PreviewPlayableCivCount( _In_z_ const wchar_t* wszFilename);

	enum MapFlags
	{
		FLAG_WORLD_WRAP       = 0x01,
		FLAG_RANDOM_RECOURCES = 0x02,
		FLAG_RANDOM_GOODIES   = 0x04,
		FLAG_OLD_GOODIE_RULES = 0x08, // Backwards compatibility thing.  Don't set normally.
		FLAG_TRANSFERABLE     = 0x10, // Determines if a map can be transfered over the internets
	};

	__forceinline bool GetFlag(MapFlags eFlag) const { return (m_uiFlags & eFlag) != 0; }
	__forceinline void SetFlag(MapFlags eFlag) { m_uiFlags |= eFlag; }
	__forceinline void ClearFlag(MapFlags eFlag) { m_uiFlags &= ~eFlag; } 
	__forceinline void SetFlag(MapFlags eFlag, bool bValue)
	{
		if( bValue ) m_uiFlags |= eFlag;
		else m_uiFlags &= ~eFlag;
	}

	uint m_uiFlags;

private:
	// Map loading for all versions of world builder maps
	bool Load0(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load1(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load2(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load3(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load4(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load5(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load6(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load7(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load8(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load9(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load10(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario);
	bool Load11Plus(FIFile &kFile, const CvWorldBuilderMapTypeDesc &kTypeDesc, bool bScenario, byte byFileVersion);


	struct MapDescription
	{
		MapDescription();

		dword uiWidth;
		dword uiHeight;
	};

	struct GameDescription
	{
		GameDescription();

		char szDefaultSpeed[64];

		uint uiStartingTurn;
		uint uiMaxTurns;
		uint uiTargetScore;
		int iStartYear;

		byte byPlayerCount;
		byte byCityStateCount;
		byte byTeamCount;
	};

	MapDescription m_kMapDesc;
	GameDescription m_kGameDesc;
	PlotMapData m_akMapData[MaxMapSize];
	PlotScenarioData m_akScenarioData[MaxMapSize];
	Player m_akPlayers[MaxPlayers];
	Player m_akCityStates[MaxCityStates];
	Team m_akTeams[MaxTeams];

	//This should really go in either the MapDescription or GameDescription
	//but cannot for the following reasons:
	//GameDescription: Only used during scenarios
	//MapDescription: Read prior to any version handling
	char m_szWorldType[64];

	template<uint TBits>
	class SerializableBitset
	{
	public:
		SerializableBitset()
		{
			ZeroMemory(m_Mem, sizeof(m_Mem));
		}

		void Reset()
		{
			ZeroMemory(m_Mem, sizeof(m_Mem));
		}

		const bool Get(uint uiBitIndex) const
		{
			FAssertMsg(uiBitIndex < TBits, "Out of range");
			const uint uiByte = uiBitIndex / 8;
			const uint uiBit = uiBitIndex % 8;
			const byte byMask = (1 << uiBit);

			if( uiByte < ByteCount )
				return (m_Mem[uiByte] & byMask) != 0;
			else
				return false;
		}

		void Set(uint uiBitIndex, bool bVal)
		{
			FAssertMsg(uiBitIndex < TBits, "Out of range");
			const uint uiByte = uiBitIndex / 8;
			const uint uiBit = uiBitIndex % 8;
			const byte byMask = (1 << uiBit);

			if( uiByte < ByteCount )
			{
				if( bVal ) m_Mem[uiByte] |= byMask;
				else m_Mem[uiByte] &= ~byMask;
			}
		}

		static uint GetSerializedSize(uint uiBitCount)
		{
			return uiBitCount / 8 + ((uiBitCount % 8)? 1 : 0);
		}

		void Serialize(void *pvBuffer, uint uiBitCount) const
		{
			FAssert(uiBitCount < TBits);
			memcpy(pvBuffer, m_Mem, GetSerializedSize(uiBitCount));
		}

		void Deserialize(const void *pvBuffer, uint uiBitCount)
		{
			FAssert(uiBitCount < TBits);
			memcpy(m_Mem, pvBuffer, GetSerializedSize(uiBitCount));
		}

	private:
		static const uint ByteCount = TBits / 8 + ((TBits % 8)? 1 : 0);
		byte m_Mem[ByteCount];
	};

	SerializableBitset<MaxMapSize * MaxTeams> m_kVisibility;

	// Return references to these when an out-of-range error occurs
	static PlotMapData sm_kErrorPlotMapData;
	static PlotScenarioData sm_kErrorPlotScenarioData;
	static Player sm_kErrorPlayer;
	static Team sm_kErrorTeam;
};

#endif // CvWorldBuilderMap_h

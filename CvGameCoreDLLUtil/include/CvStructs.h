#pragma once 

struct CvColorA 
{
	CvColorA(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
	CvColorA() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)  {}
	float r, g, b, a;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCombatMemberEntry
//!  \brief		A single member in combat.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvCombatMemberEntry {
	PlayerTypes		m_ePlayer;
	int				m_iID;
	uint16			m_bfFlags;

	enum INFO_FLAGS
	{
		INFO_FLAG_IS_CITY	= (1 << 0)
	};
public:

	enum MEMBER_TYPE
	{
		MEMBER_UNIT,
		MEMBER_CITY
	};

	int16			m_iDamage;		// Amount of damage for this combat
	int16			m_iFinalDamage;	// Amount of unit damage in total (damage this combat + previous damage)
	int16			m_iMaxHitPoints;
	int16			m_iX;			// Plot X
	int16			m_iY;			// Plot X
	UnitCombatTypes	m_eCombatType;
	EraTypes		m_eEra;			// Era type for the unit/city

	uint32			m_uiWeaponType;		// Used for cities
	float			m_fWeaponOption;	// Used for cities

	PlayerTypes GetPlayer() const { return m_ePlayer; }
	void		SetPlayer(PlayerTypes ePlayer) { m_ePlayer = ePlayer; }

	bool		IsValid() const { return m_ePlayer != NO_PLAYER && m_iID != -1; }
	int			GetID() const { return m_iID; }
	void		SetID(int iID, MEMBER_TYPE eType) 
	{ 
		m_iID = iID; 
		if (eType == MEMBER_UNIT)
			m_bfFlags &= ~(uint)INFO_FLAG_IS_CITY; 
		else
			m_bfFlags |= (uint)INFO_FLAG_IS_CITY; 
	}

	bool		IsUnit() const { return IsValid() && (m_bfFlags & (uint)INFO_FLAG_IS_CITY) == 0; }
	int			GetUnitID() const { if (IsUnit()) return m_iID; else return -1; }
	void		SetUnitID(int iID) { m_iID = iID; m_bfFlags &= ~(uint)INFO_FLAG_IS_CITY; }

	bool		IsCity() const { return IsValid() && (m_bfFlags & (uint)INFO_FLAG_IS_CITY) != 0; }
	int			GetCityID() const { if (IsCity()) return m_iID; else return -1; }
	void		SetCityID(int iID) { m_iID = iID; m_bfFlags |= (uint)INFO_FLAG_IS_CITY; }

	bool		IsType(MEMBER_TYPE eType) const 
	{ 
		if (IsValid())
		{
			if ((m_bfFlags & (uint)INFO_FLAG_IS_CITY) != 0)
				return eType == MEMBER_CITY;
			else
				return eType == MEMBER_UNIT;
		}

		return false;
	}
	int			GetDamage() const { return (int)m_iDamage; }
	void		SetDamage(int iDamage) { m_iDamage = (int16)iDamage; }

	int			GetFinalDamage() const { return (int)m_iFinalDamage; }
	void		SetFinalDamage(int iFinalDamage) { m_iFinalDamage = (int16)iFinalDamage; }

	int			GetMaxHitPoints() const { return (int)m_iMaxHitPoints; }
	void		SetMaxHitPoints(int iMaxHitPoints) { m_iMaxHitPoints = (int16)iMaxHitPoints; }

	UnitCombatTypes GetCombatType() const { return m_eCombatType; }
	void		SetCombatType(UnitCombatTypes eCombatType) { m_eCombatType = eCombatType; }

	EraTypes	GetEra() const { return m_eEra; }
	void		SetEra(EraTypes eEra) { m_eEra = eEra; }

	int			GetX() const { return m_iX; }
	int			GetY() const { return m_iY; }
	void		SetPlot(int32 iX, int32 iY) { m_iX = (int16)iX; m_iY = (int16)iY; }

	uint32		GetWeaponType() const { return m_uiWeaponType; }
	void		SetWeaponType(uint32 uiWeaponType) { m_uiWeaponType = uiWeaponType; }

	float		GetWeaponOption() const { return m_fWeaponOption; }
	void		SetWeaponOption(float fWeaponOption) { m_fWeaponOption = fWeaponOption; }

	CvCombatMemberEntry() : m_ePlayer(NO_PLAYER), m_iID(-1), m_bfFlags(0), m_iDamage(0), m_iFinalDamage(0), m_iMaxHitPoints(0), m_eCombatType(NO_UNITCOMBAT), m_eEra(NO_ERA), m_iX(0), m_iY(0), m_uiWeaponType(0), m_fWeaponOption(0.f) {}
};

#define POPUP_PARAM_INT_ARRAY(x)	sizeof(x)/sizeof(int), &x[0]
#define POPUP_PARAM_BOOL_ARRAY(x)	sizeof(x)/sizeof(bool), &x[0]

//------------------------------------------------------------------------------
struct CvPopupInfo
{
public:
	CvPopupInfo(ButtonPopupTypes buttonPopupType = BUTTONPOPUP_TEXT, int data1 = -1, int data2 = -1, int data3 = -1, int flags = 0, bool option1 = false, bool option2 = false)
		: iData1(data1)
		, iData2(data2)
		, iData3(data3)
		, iFlags(flags)
		, bOption1(option1)
		, bOption2(option2)
		, eButtonPopupType(buttonPopupType)
	{
		//Nothing
		szText[0] = 0;
	}
	
	int iData1;
	int iData2;
	int iData3;
	int iFlags;
	bool bOption1;
	bool bOption2;
	ButtonPopupTypes eButtonPopupType;
	char szText[512];
};

struct CvMapInitData
{
	int m_iGridW;
	int m_iGridH;
	int m_iTopLatitude;
	int m_iBottomLatitude;

	bool m_bWrapX;
	bool m_bWrapY;

	CvMapInitData(int iGridW = 0, int iGridH = 0, int iTopLatitude = 90, int iBottomLatitude = -90, bool bWrapX = true, bool bWrapY = false) :
	m_iGridH(iGridH),m_iGridW(iGridW),m_iTopLatitude(iTopLatitude),m_iBottomLatitude(iBottomLatitude),m_bWrapY(bWrapY),m_bWrapX(bWrapX)
	{ }
};

//------------------------------------------------------------------------------
struct CvWorldBuilderMapLoaderMapInfo
{
	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned int uiPlayers;
	unsigned int uiCityStates;
	unsigned int uiMajorCivStartingPositions;
	unsigned int uiMinorCivStartingPositions;
	bool bWorldWrap;
	bool bRandomResources;

	CvWorldBuilderMapLoaderMapInfo() 
		: uiWidth(0)
		, uiHeight(0)
		, uiPlayers(0)
		, uiCityStates(0)
		, uiMajorCivStartingPositions(0)
		, uiMinorCivStartingPositions(0)
		, bWorldWrap(false)
		, bRandomResources(false)
	{

	}
};
//------------------------------------------------------------------------------
struct IDInfo
{
	IDInfo(PlayerTypes eOwner=NO_PLAYER, int iID=FFreeList::INVALID_INDEX) : eOwner(eOwner), iID(iID) {}
	PlayerTypes eOwner;
	int iID;

	bool operator== (const IDInfo& info) const
	{
		return (eOwner == info.eOwner && iID == info.iID);
	}

	bool operator !=(const IDInfo& other) const
	{
		return (eOwner != other.eOwner || iID != other.iID);
	}

	void reset()
	{
		eOwner = NO_PLAYER;
		iID = FFreeList::INVALID_INDEX;
	}

	bool isInvalid() const
	{
		return eOwner == NO_PLAYER && iID == FFreeList::INVALID_INDEX;
	}
};
//------------------------------------------------------------------------------
struct UnitMoveRate 
{
	UnitMoveRate():m_fTotalMoveRate(1.0f),m_fEaseIn(0.0f),m_fEaseOut(0.0f),m_fIndividualOffset(0.0f), m_fRowOffset(0.0f), m_fCurveRoll(0.0f), m_iPathSubdivision(1)
	{
	}

	float m_fTotalMoveRate;
	float m_fEaseIn;
	float m_fEaseOut;
	float m_fIndividualOffset;
	float m_fRowOffset;
	float m_fCurveRoll;
	int   m_iPathSubdivision;
};


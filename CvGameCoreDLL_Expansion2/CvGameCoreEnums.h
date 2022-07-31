/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once

enum CLOSED_ENUM YieldTypes
{
    NO_YIELD = -1,

    YIELD_FOOD,
    YIELD_PRODUCTION,
    YIELD_GOLD,
    YIELD_SCIENCE,
    YIELD_CULTURE,
    YIELD_FAITH,
    YIELD_TOURISM,
    YIELD_GOLDEN_AGE_POINTS,
#if defined(MOD_BALANCE_CORE_YIELDS)
	YIELD_GREAT_GENERAL_POINTS,
	YIELD_GREAT_ADMIRAL_POINTS,
	YIELD_POPULATION,
	YIELD_CULTURE_LOCAL,
#endif
#if defined(MOD_BALANCE_CORE_JFD)
	YIELD_JFD_HEALTH,
	YIELD_JFD_DISEASE,
	YIELD_JFD_CRIME,
	YIELD_JFD_LOYALTY,
	YIELD_JFD_SOVEREIGNTY,
#endif

    NUM_YIELD_TYPES ENUM_META_VALUE
};

// Carries the value of an enum represented by type T.
// Useful to communicate what a value is meant to represent while still
// being able to specify exactly what kind of storage it occupies.
// 
// !! Please only use when you actually want to specify the storage type
// !! Using this for temporaries such as stack values is not recommended
template<typename Enum, typename T>
class CvEnum
{
public:
	inline CvEnum() {}
	inline CvEnum(Enum src)
		: m_value(static_cast<T>(src))
	{}
	inline CvEnum(const CvEnum<Enum, T>& src)
		: m_value(src.m_value)
	{}

	inline const T& raw() const
	{
		return m_value;
	}
	inline T& raw()
	{
		return m_value;
	}
	inline operator Enum() const
	{
		return static_cast<Enum>(m_value);
	}

	inline CvEnum<Enum, T>& operator=(Enum rhs)
	{
		m_value = static_cast<T>(rhs);
		return *this;
	}
	inline CvEnum<Enum, T>& operator=(const CvEnum<Enum, T>& rhs)
	{
		m_value = rhs.m_value;
		return *this;
	}

	inline bool operator==(Enum rhs) const
	{
		return static_cast<Enum>(m_value) == rhs;
	}
	inline bool operator==(const CvEnum<Enum, T>& rhs) const
	{
		return m_value == rhs.m_value;
	}
	inline bool operator!=(Enum rhs) const
	{
		return static_cast<Enum>(m_value) != rhs;
	}
	inline bool operator!=(const CvEnum<Enum, T>& rhs) const
	{
		return m_value != rhs.m_value;
	}
	inline bool operator<=(Enum rhs) const
	{
		return static_cast<Enum>(m_value) <= rhs;
	}
	inline bool operator<=(const CvEnum<Enum, T>& rhs) const
	{
		return m_value <= rhs.m_value;
	}
	inline bool operator>=(Enum rhs) const
	{
		return static_cast<Enum>(m_value) >= rhs;
	}
	inline bool operator>=(const CvEnum<Enum, T>& rhs) const
	{
		return m_value >= rhs.m_value;
	}
	inline bool operator<(Enum rhs) const
	{
		return static_cast<Enum>(m_value) < rhs;
	}
	inline bool operator<(const CvEnum<Enum, T>& rhs) const
	{
		return m_value < rhs.m_value;
	}
	inline bool operator>(Enum rhs) const
	{
		return static_cast<Enum>(m_value) > rhs;
	}
	inline bool operator>(const CvEnum<Enum, T>& rhs) const
	{
		return m_value > rhs.m_value;
	}

private:
	T m_value;
};

// Popups specific to this DLL

// Hashed values.  Use FStringHashGen to create!
#define BUTTONPOPUP_LEAGUE_OVERVIEW						((ButtonPopupTypes)0x41D0F622)
#define BUTTONPOPUP_DECLAREWAR_PLUNDER_TRADE_ROUTE		((ButtonPopupTypes)0xFCB501AF)
#define BUTTONPOPUP_CHOOSE_ARCHAEOLOGY					((ButtonPopupTypes)0x0752FFBE)
#define BUTTONPOPUP_CHOOSE_IDEOLOGY						((ButtonPopupTypes)0xF604A676)
#define BUTTONPOPUP_CHOOSE_INTERNATIONAL_TRADE_ROUTE	((ButtonPopupTypes)0x920D58CC)
#define BUTTONPOPUP_CULTURE_OVERVIEW					((ButtonPopupTypes)0xE81563A9)
#define BUTTONPOPUP_CHOOSE_TRADE_UNIT_NEW_HOME			((ButtonPopupTypes)0x10BD8E0F)
#define BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER	((ButtonPopupTypes)0x3D7CCE59)
#define BUTTONPOPUP_TRADE_ROUTE_OVERVIEW				((ButtonPopupTypes)0x7D23885D)
#define BUTTONPOPUP_LEAGUE_PROJECT_COMPLETED			((ButtonPopupTypes)0xCAA4FBA2)
#define BUTTONPOPUP_CHOOSE_GOODY_HUT_REWARD				((ButtonPopupTypes)0xFF412A6F)
#define BUTTONPOPUP_LEAGUE_SPLASH						((ButtonPopupTypes)0xA53EBF16)
#define BUTTONPOPUP_CHOOSE_ADMIRAL_PORT					((ButtonPopupTypes)0x4D1B317C)

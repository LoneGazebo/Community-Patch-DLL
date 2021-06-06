#pragma once

#ifndef CVINFOMAP_H
#define CVINFOMAP_H

// Like a std::map but specifically where an cv enum type is the key.
// Meant to replace various locations where arrays are being used
// like maps in order to better communicate intent.
// Automatic specialization selection is provided by CvEnumMap<> and
// should provide no more overhead than normally necessary.

#include "FFireTypes.h"
#include "FDefNew.h"

#include "CvEnumsUtil.h"

#include <algorithm>
#include <iterator>

template<typename Enum, typename T, bool Fixed = CvEnumsUtil::Traits<Enum>::IsFixed>
class CvEnumMap
{
public:
	inline CvEnumMap()
		: m_values(NULL)
	{}
	inline ~CvEnumMap()
	{
		uninit();
	}

	typedef Enum KeyType;
	typedef T ValueType;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

	inline void init()
	{
		FAssertMsg(m_values == NULL, "Dynamic CvEnumMap<>::init() called without first calling CvEnumMap<>::uninit() - Memory will leak");
		m_values = FNEW(T[size()], c_eCiv5GameplayDLL, 0);
	}
	inline void init(const T& fill)
	{
		init();
		assign(fill);
	}
	inline void uninit()
	{
		SAFE_DELETE_ARRAY(m_values);
	}

	inline void assign(const T& fill)
	{
		checkValidAccess();
		for (Iterator it = begin(); it != end(); ++it)
		{
			*it = fill;
		}
	}

	inline static std::size_t size()
	{
		return CvEnumsUtil::Count<Enum>();
	}
	inline const T* data() const
	{
		checkValidAccess();
		return m_values;
	}
	inline T* data()
	{
		checkValidAccess();
		return m_values;
	}

	inline const T& operator[](std::size_t idx) const
	{
		checkValidAccess();
		FAssertMsg(idx < size(), "Attempting to access out of range index in CvEnumMap<>");
		return m_values[idx];
	}
	inline T& operator[](std::size_t idx)
	{
		return const_cast<T&>(const_cast<const CvEnumMap<Enum, T>&>(*this).operator[](idx));
	}

	inline ConstIterator begin() const
	{
		checkValidAccess();
		return m_values;
	}
	inline Iterator begin()
	{
		checkValidAccess();
		return m_values;
	}
	inline ConstIterator end() const
	{
		checkValidAccess();
		return m_values + size();
	}
	inline Iterator end()
	{
		checkValidAccess();
		return m_values + size();
	}
	inline ConstIterator cbegin() const
	{
		checkValidAccess();
		return m_values;
	}
	inline ConstIterator cend() const
	{
		checkValidAccess();
		return m_values + size();
	}

	inline ConstReverseIterator rbegin() const
	{
		checkValidAccess();
		return ConstReverseIterator(m_values + size());
	}
	inline ReverseIterator rbegin()
	{
		checkValidAccess();
		return ReverseIterator(m_values + size());
	}
	inline ConstReverseIterator rend() const
	{
		checkValidAccess();
		return ConstReverseIterator(m_values);
	}
	inline ReverseIterator rend()
	{
		checkValidAccess();
		return ReverseIterator(m_values);
	}
	inline ConstIterator crbegin() const
	{
		checkValidAccess();
		return ConstReverseIterator(m_values + size());
	}
	inline ConstIterator crend() const
	{
		checkValidAccess();
		return ConstReverseIterator(m_values);
	}

	inline operator bool() const
	{
		return m_values != NULL;
	}
	inline bool operator==(const CvEnumMap<Enum, T, Fixed>& rhs) const
	{
		checkValidAccess();
		return std::equal(begin(), end(), rhs.begin());
	}
	inline bool operator!=(const CvEnumMap<Enum, T, Fixed>& rhs) const
	{
		return !(*this == rhs);
	}

private:
	CvEnumMap(const CvEnumMap<Enum, T, Fixed>&);
	CvEnumMap<Enum, T, Fixed>& operator=(const CvEnumMap<Enum, T, Fixed>&);

	void checkValidAccess() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access dynamic CvEnumMap<> without first calling CvEnumMap<>::init()");
	}

	T* m_values;
};

template<typename Enum, typename T>
class CvEnumMap<Enum, T, true>
{
public:
	typedef Enum EnumType;
	typedef T ValueType;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

	enum { SizeConstant = CvEnumsUtil::Traits<Enum>::CountConstant };

	inline CvEnumMap()
	{}

	inline void init()
	{
	}
	inline void init(const T& fill)
	{
		assign(fill);
	}
	inline void uninit()
	{
	}

	inline void assign(const T& fill)
	{
		for (Iterator it = begin(); it != end(); ++it)
		{
			*it = fill;
		}
	}

	inline static std::size_t size()
	{
		return SizeConstant;
	}
	inline const T* data() const
	{
		return m_values;
	}
	inline T* data()
	{
		return m_values;
	}

	inline const T& operator[](std::size_t idx) const
	{
		FAssertMsg(idx < size(), "Attempting to access out of range index in CvEnumMap<>");
		return m_values[idx];
	}
	inline T& operator[](std::size_t idx)
	{
		return const_cast<T&>(const_cast<const CvEnumMap<Enum, T>&>(*this).operator[](idx));
	}

	inline ConstIterator begin() const
	{
		return m_values;
	}
	inline Iterator begin()
	{
		return m_values;
	}
	inline ConstIterator end() const
	{
		return m_values + size();
	}
	inline Iterator end()
	{
		return m_values + size();
	}
	inline ConstIterator cbegin() const
	{
		return m_values;
	}
	inline ConstIterator cend() const
	{
		return m_values + size();
	}

	inline ConstReverseIterator rbegin() const
	{
		return ConstReverseIterator(m_values + size());
	}
	inline ReverseIterator rbegin()
	{
		return ReverseIterator(m_values + size());
	}
	inline ConstReverseIterator rend() const
	{
		return ConstReverseIterator(m_values);
	}
	inline ReverseIterator rend()
	{
		return ReverseIterator(m_values);
	}
	inline ConstIterator crbegin() const
	{
		return ConstReverseIterator(m_values + size());
	}
	inline ConstIterator crend() const
	{
		return ConstReverseIterator(m_values);
	}

	inline bool operator==(const CvEnumMap<Enum, T, true>& rhs) const
	{
		return std::equal(begin(), end(), rhs.begin());
	}
	inline bool operator!=(const CvEnumMap<Enum, T, true>& rhs) const
	{
		return !(*this == rhs);
	}

private:
	CvEnumMap(const CvEnumMap<Enum, T, true>&);
	CvEnumMap<Enum, T, true>& operator=(const CvEnumMap<Enum, T, true>&);

	T m_values[SizeConstant];
};

#endif

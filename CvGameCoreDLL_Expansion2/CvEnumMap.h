#pragma once

#ifndef CVINFOMAP_H
#define CVINFOMAP_H

// Like a std::map but specifically where an cv enum type is the key.
// Meant to replace various locations where arrays are being used
// like maps in order to better communicate intent.
// Automatic specialization selection is provided by CvEnumMap<> and
// should provide no more overhead than normally necessary.

#include "CvAssert.h"
#include "CvAlignedStorage.h"
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
	{
		CvAssert(sizeof(StorageType) == sizeof(T));
		CvAssert(__alignof(StorageType) == __alignof(T));
	}
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

	void init()
	{
		CvAssertMsg(m_values == NULL, "Dynamic CvEnumMap<>::init() called without first calling CvEnumMap<>::uninit() - Memory will leak");
		m_values = FNEW(StorageType[size()], c_eCiv5GameplayDLL, 0);
#ifdef VPDEBUG
		m_initSize = size();
#endif
		for (Iterator it = begin(); it != end(); ++it)
		{
			new (&(*it)) T;
		}
	}
	void init(const T& fill)
	{
		CvAssertMsg(m_values == NULL, "Dynamic CvEnumMap<>::init() called without first calling CvEnumMap<>::uninit() - Memory will leak");
		m_values = FNEW(StorageType[size()], c_eCiv5GameplayDLL, 0);
#ifdef VPDEBUG
		m_initSize = size();
#endif
		for (Iterator it = begin(); it != end(); ++it)
		{
			new (&(*it)) T(fill);
		}
	}
	void uninit()
	{
		if (m_values != NULL)
		{
			for (ReverseIterator it = rbegin(); it != rend(); ++it)
			{
				(*it).~T();
			}
			SAFE_DELETE_ARRAY(m_values);
		}
	}

	void assign(const T& fill)
	{
		checkValidAccess();
		std::fill_n(begin(), size(), fill);
	}

	inline static std::size_t size()
	{
		return CvEnumsUtil::Count<Enum>();
	}
	inline const T* data() const
	{
		checkValidAccess();
		return reinterpret_cast<const T*>(m_values);
	}
	inline T* data()
	{
		checkValidAccess();
		return reinterpret_cast<T*>(m_values);
	}

	inline const T& operator[](std::size_t idx) const
	{
		checkValidAccess();
		CvAssertMsg(idx < size(), "Attempting to access out of range index in CvEnumMap<>");
		return data()[idx];
	}
	inline T& operator[](std::size_t idx)
	{
		return const_cast<T&>(const_cast<const CvEnumMap<Enum, T>&>(*this).operator[](idx));
	}

	inline ConstIterator begin() const
	{
		checkValidAccess();
		return data();
	}
	inline Iterator begin()
	{
		checkValidAccess();
		return data();
	}
	inline ConstIterator end() const
	{
		checkValidAccess();
		return data() + size();
	}
	inline Iterator end()
	{
		checkValidAccess();
		return data() + size();
	}
	inline ConstIterator cbegin() const
	{
		checkValidAccess();
		return data();
	}
	inline ConstIterator cend() const
	{
		checkValidAccess();
		return data() + size();
	}

	inline ConstReverseIterator rbegin() const
	{
		checkValidAccess();
		return ConstReverseIterator(data() + size());
	}
	inline ReverseIterator rbegin()
	{
		checkValidAccess();
		return ReverseIterator(data() + size());
	}
	inline ConstReverseIterator rend() const
	{
		checkValidAccess();
		return ConstReverseIterator(data());
	}
	inline ReverseIterator rend()
	{
		checkValidAccess();
		return ReverseIterator(data());
	}
	inline ConstIterator crbegin() const
	{
		checkValidAccess();
		return ConstReverseIterator(data() + size());
	}
	inline ConstIterator crend() const
	{
		checkValidAccess();
		return ConstReverseIterator(data());
	}

	inline bool valid() const
	{
		return m_values != NULL;
	}
	inline bool operator==(const CvEnumMap<Enum, T, Fixed>& rhs) const
	{
		return std::equal(begin(), end(), rhs.begin());
	}
	inline bool operator!=(const CvEnumMap<Enum, T, Fixed>& rhs) const
	{
		return !(*this == rhs);
	}

private:
	CvEnumMap(const CvEnumMap<Enum, T, Fixed>&);
	CvEnumMap<Enum, T, Fixed>& operator=(const CvEnumMap<Enum, T, Fixed>&);

	inline void checkValidAccess() const
	{
		CvAssertMsg(m_values != NULL, "Attempting to access dynamic CvEnumMap<> without first calling CvEnumMap<>::init()");
		CvAssertMsg(m_values == NULL || m_initSize == size(), "Attempting to access dynamic CvEnumMap<> whose size has changed since CvEnumMap<>::init() was called");
	}

	typedef typename CvAlignedStorage<T>::Type StorageType;
	StorageType* m_values;
#ifdef VPDEBUG
	std::size_t m_initSize;
#endif
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
		: m_initialized(false)
	{
		CvAssert(sizeof(StorageType) == sizeof(T));
		CvAssert(__alignof(StorageType) == __alignof(T));
	}

	inline ~CvEnumMap()
	{
		uninit();
	}

	void init()
	{
		CvAssertMsg(m_initialized == false, "Fixed CvEnumMap<>::init() called without first calling CvEnumMap<>::uninit() - Elements will not be destroyed");
		m_initialized = true;
		for (Iterator it = begin(); it != end(); ++it)
		{
			new (&(*it)) T;
		}
	}
	void init(const T& fill)
	{
		CvAssertMsg(m_initialized == false, "Fixed CvEnumMap<>::init() called without first calling CvEnumMap<>::uninit() - Elements will not be destroyed");
		m_initialized = true;
		for (Iterator it = begin(); it != end(); ++it)
		{
			new (&(*it)) T(fill);
		}
	}
	void uninit()
	{
		if (m_initialized)
		{
			for (ReverseIterator it = rbegin(); it != rend(); ++it)
			{
				(*it).~T();
			}
			m_initialized = false;
		}
	}

	void assign(const T& fill)
	{
		checkValidAccess();
		std::fill_n(begin(), std::size_t(SizeConstant), fill);
	}

	inline static std::size_t size()
	{
		return SizeConstant;
	}
	inline const T* data() const
	{
		checkValidAccess();
		return reinterpret_cast<const T*>(m_values);
	}
	inline T* data()
	{
		checkValidAccess();
		return reinterpret_cast<T*>(m_values);
	}

	inline const T& operator[](std::size_t idx) const
	{
		checkValidAccess();
		CvAssertMsg(idx < size(), "Attempting to access out of range index in CvEnumMap<>");
		return data()[idx];
	}
	inline T& operator[](std::size_t idx)
	{
		return const_cast<T&>(const_cast<const CvEnumMap<Enum, T>&>(*this).operator[](idx));
	}

	inline ConstIterator begin() const
	{
		checkValidAccess();
		return data();
	}
	inline Iterator begin()
	{
		checkValidAccess();
		return data();
	}
	inline ConstIterator end() const
	{
		checkValidAccess();
		return data() + size();
	}
	inline Iterator end()
	{
		checkValidAccess();
		return data() + size();
	}
	inline ConstIterator cbegin() const
	{
		checkValidAccess();
		return data();
	}
	inline ConstIterator cend() const
	{
		checkValidAccess();
		return data() + size();
	}

	inline ConstReverseIterator rbegin() const
	{
		checkValidAccess();
		return ConstReverseIterator(data() + size());
	}
	inline ReverseIterator rbegin()
	{
		checkValidAccess();
		return ReverseIterator(data() + size());
	}
	inline ConstReverseIterator rend() const
	{
		checkValidAccess();
		return ConstReverseIterator(data());
	}
	inline ReverseIterator rend()
	{
		checkValidAccess();
		return ReverseIterator(data());
	}
	inline ConstIterator crbegin() const
	{
		checkValidAccess();
		return ConstReverseIterator(data() + size());
	}
	inline ConstIterator crend() const
	{
		checkValidAccess();
		return ConstReverseIterator(data());
	}

	inline bool valid() const
	{
		return m_initialized;
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

	inline void checkValidAccess() const
	{
		CvAssertMsg(m_initialized == true, "Attempting to access fixed CvEnumMap<> without first calling CvEnumMap<>::init()");
	}

	typedef typename CvAlignedStorage<T>::Type StorageType;
	StorageType m_values[SizeConstant];
	bool m_initialized;
};

#endif

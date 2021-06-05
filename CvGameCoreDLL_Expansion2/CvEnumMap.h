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
#include "CvMeta.h"

#include <iterator>

template<typename Enum, typename T>
class CvEnumMapDynamic
{
public:
	inline CvEnumMapDynamic()
		: m_values(NULL)
	{}

	typedef Enum KeyType;
	typedef T ValueType;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

	inline void init()
	{
		FAssertMsg(m_values == NULL, "CvEnumMapDynamic<>::init() called without first calling CvEnumMapDynamic<>::uninit() - Memory will leak");
		m_values = FNEW(T[size()], c_eCiv5GameplayDLL, 0);
	}
	inline void init(const T& fill)
	{
		init();
		reset(fill);
	}
	inline void uninit()
	{
		SAFE_DELETE_ARRAY(m_values);
	}

	inline void reset(const T& fill)
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
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
		return m_values;
	}
	inline T* data()
	{
		return m_values;
	}

	inline const T& operator[](std::size_t idx) const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		FAssertMsg(idx >= size(), "Attempting to access out of range index in CvEnumMapDynamic<>");
		return m_values[idx];
	}
	inline T& operator[](std::size_t idx)
	{
		return const_cast<T&>(const_cast<const CvEnumMapDynamic<Enum, T>&>(*this).operator[](idx));
	}

	inline ConstIterator begin() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return m_values;
	}
	inline Iterator begin()
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return m_values;
	}
	inline ConstIterator end() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return m_values + size();
	}
	inline Iterator end()
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return m_values + size();
	}
	inline ConstIterator cbegin() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return m_values;
	}
	inline ConstIterator cend() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return m_values + size();
	}

	inline ConstReverseIterator rbegin() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return ConstReverseIterator(m_values + size());
	}
	inline ReverseIterator rbegin()
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return ReverseIterator(m_values + size());
	}
	inline ConstReverseIterator rend() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return ConstReverseIterator(m_values);
	}
	inline ReverseIterator rend()
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return ReverseIterator(m_values);
	}
	inline ConstIterator crbegin() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return ConstReverseIterator(m_values + size());
	}
	inline ConstIterator crend() const
	{
		FAssertMsg(m_values != NULL, "Attempting to access CvEnumMapDynamic<> without first calling CvEnumMapDynamic<>::init()");
		return ConstReverseIterator(m_values);
	}

	inline operator bool() const
	{
		return m_values != NULL;
	}

private:
	T* m_values;
};

template<typename Enum, typename T>
class CvEnumMapFixed
{
public:
	typedef Enum EnumType;
	typedef T ValueType;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

	enum { SizeConstant = CvEnumsUtil::Traits<Enum>::CountConstant };

	inline void init()
	{
	}
	inline void init(const T& fill)
	{
		init();
		reset(fill);
	}
	inline void uninit()
	{
	}

	inline void reset(const T& fill)
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
		FAssertMsg(idx >= size(), "Attempting to access out of range index in CvEnumMapFixed<>");
		return m_values[idx];
	}
	inline T& operator[](std::size_t idx)
	{
		return const_cast<T&>(const_cast<const CvEnumMapFixed<Enum, T>&>(*this).operator[](idx));
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

private:
	T m_values[SizeConstant];
};

template<typename Enum, typename T>
class CvEnumMap
	: public CvMeta::Conditional<CvEnumsUtil::Traits<Enum>::IsFixed, CvEnumMapFixed<Enum, T>, CvEnumMapDynamic<Enum, T>>::Type
{
public:
	typedef typename CvMeta::Conditional<CvEnumsUtil::Traits<Enum>::IsFixed, CvEnumMapFixed<Enum, T>, CvEnumMapDynamic<Enum, T>>::Type ConcreteType;
};

#endif

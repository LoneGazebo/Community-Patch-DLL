#pragma once

#ifndef CVSPAN_H
#define CVSPAN_H

#include "CvAssert.h"
#include "CvMeta.h"

#include <cstddef>
#include <iterator>

// Interface similar to C++20's std::span
// Similar to Fraxis' ArrayWrapper but more "correct"
// Exists mostly to make serialization easier but feel free to use anywhere

enum DynamicExtentType
{
#ifdef __clang__
	DYNAMIC_EXTENT = INT_MAX,
#else
	DYNAMIC_EXTENT = std::size_t(-1),
#endif // __clang__
};

template<typename T, std::size_t TExtent = DYNAMIC_EXTENT>
class CvSpan
{
public:
	typedef T ElementType;
	typedef typename CvMeta::RemoveCV<T>::Type ValueType;

	typedef std::size_t SizeType;
	typedef std::ptrdiff_t DifferenceType;

	typedef T* Pointer;
	typedef const T* ConstPointer;

	typedef T& Reference;
	typedef const T& ConstReference;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

	enum { Extent = TExtent };

	template<typename It>
	inline explicit CvSpan(It first, typename CvMeta::EnableIf<CvMeta::IsSame<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::Value, SizeType>::Type count)
		: m_begin(&(*first))
	{
		CvAssert(m_begin != NULL && count == TExtent);
	}
	template<typename It>
	inline explicit CvSpan(It first, typename CvMeta::EnableIf<CvMeta::IsSame<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::Value, It>::Type last)
		: m_begin(&(*first))
	{
		CvAssert(last - first == TExtent);
	}
	inline CvSpan(ElementType(&arr)[TExtent])
		: m_begin(arr)
	{
	}
	template<typename U, std::size_t N>
	inline CvSpan(const CvSpan<U, N>& src, typename CvMeta::EnableIf<CvMeta::IsSame<typename CvSpan<U, N>::ValueType, ValueType>::Value && (N == TExtent || N == DYNAMIC_EXTENT), int>::Type = 0)
		: m_begin(&(*src.begin()))
	{
		CvAssert(src.size() == TExtent);
	}

	inline Iterator begin() const
	{
		return m_begin;
	}
	inline Iterator end() const
	{
		return m_begin + TExtent;
	}

	inline ReverseIterator rbegin() const
	{
		return ReverseIterator(m_begin + TExtent);
	}
	inline ReverseIterator rend() const
	{
		return ReverseIterator(m_begin);
	}

	inline Reference front() const
	{
		return *m_begin;
	}
	inline Reference back() const
	{
		return *(m_begin + (TExtent - 1));
	}
	inline Reference operator[](SizeType idx) const
	{
		CvAssert(idx < TExtent);
		return m_begin[idx];
	}
	inline Pointer data() const
	{
		return m_begin;
	}

	inline SizeType size() const
	{
		return TExtent;
	}
	inline bool empty() const
	{
		return false;
	}

private:
	T* m_begin;
};

template<typename T>
class CvSpan<T, DYNAMIC_EXTENT>
{
public:
	typedef T ElementType;
	typedef typename CvMeta::RemoveCV<T>::Type ValueType;

	typedef std::size_t SizeType;
	typedef std::ptrdiff_t DifferenceType;

	typedef T* Pointer;
	typedef const T* ConstPointer;

	typedef T& Reference;
	typedef const T& ConstReference;

	typedef T* Iterator;
	typedef std::reverse_iterator<Iterator> ReverseIterator;

	enum { Extent = DYNAMIC_EXTENT };

	inline CvSpan()
		: m_begin(NULL)
		, m_end(NULL)
	{
	}
	template<typename It>
	inline CvSpan(It first, typename CvMeta::EnableIf<CvMeta::IsSame<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::Value, SizeType>::Type count)
		: m_begin(&(*first))
		, m_end(m_begin + count)
	{
		CvAssert(count == 0 || m_begin != NULL);
	}
	template<typename It>
	inline CvSpan(It first, typename CvMeta::EnableIf<CvMeta::IsSame<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::Value, It>::Type last)
		: m_begin(&(*first))
		, m_end(&(*last))
	{
		CvAssert(m_begin <= m_end);
	}
	template<std::size_t N>
	inline CvSpan(ElementType(&arr)[N])
		: m_begin(arr)
		, m_end(arr + N)
	{
	}
	template<typename U, std::size_t N>
	inline CvSpan(const CvSpan<U, N>& src, typename CvMeta::EnableIf<CvMeta::IsSame<typename CvSpan<U, N>::ValueType, ValueType>::Value, int>::Type = 0)
		: m_begin(&(*src.begin()))
		, m_end(&(*src.end()))
	{
	}

	inline Iterator begin() const
	{
		return m_begin;
	}
	inline Iterator end() const
	{
		return m_end;
	}

	inline ReverseIterator rbegin() const
	{
		return ReverseIterator(m_end);
	}
	inline ReverseIterator rend() const
	{
		return ReverseIterator(m_begin);
	}

	inline Reference front() const
	{
		CvAssert(!empty());
		return *m_begin;
	}
	inline Reference back() const
	{
		CvAssert(!empty());
		return *(m_end - 1);
	}
	inline Reference operator[](SizeType idx) const
	{
		CvAssert(idx < size());
		return m_begin[idx];
	}
	inline Pointer data() const
	{
		return m_begin;
	}

	inline SizeType size() const
	{
		return m_end - m_begin;
	}
	inline bool empty() const
	{
		return m_begin != m_end;
	}

private:
	T* m_begin;
	T* m_end;
};

template<typename T>
class CvSpan<T, 0>
{
public:
	typedef T ElementType;
	typedef typename CvMeta::RemoveCV<T>::Type ValueType;

	typedef std::size_t SizeType;
	typedef std::ptrdiff_t DifferenceType;

	typedef T* Pointer;
	typedef const T* ConstPointer;

	typedef T& Reference;
	typedef const T& ConstReference;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

	enum { Extent = 0 };

	inline CvSpan()
	{
	}
	template<typename It>
	inline explicit CvSpan(It first, typename CvMeta::EnableIf<CvMeta::IsSame<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::Value, SizeType>::Type count)
	{
		CvAssert(count == 0);
	}
	template<typename It>
	inline explicit CvSpan(It first, typename CvMeta::EnableIf<CvMeta::IsSame<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::Value, It>::Type last)
	{
		CvAssert(m_end - m_begin == 0);
	}
	template<typename U, std::size_t N>
	inline CvSpan(const CvSpan<U, N>& src, typename CvMeta::EnableIf<CvMeta::IsSame<typename CvSpan<U, N>::ValueType, ValueType>::Value && (N == 0 || N == DYNAMIC_EXTENT), int>::Type = 0)
	{
		CvAssert(src.size() == 0);
	}

	inline Iterator begin() const
	{
		return NULL;
	}
	inline Iterator end() const
	{
		return NULL;
	}

	inline ReverseIterator rbegin() const
	{
		return ReverseIterator(NULL);
	}
	inline ReverseIterator rend() const
	{
		return ReverseIterator(NULL);
	}

	inline Reference front() const
	{
		CvAssertMsg(false, "front() of empty extent is undefined behavior");
		return *static_cast<Pointer>(NULL);
	}
	inline Reference back() const
	{
		CvAssertMsg(false, "back() of empty extent is undefined behavior");
		return *static_cast<Pointer>(NULL);
	}
	inline Reference operator[](SizeType idx) const
	{
		CvAssertMsg(false, "operator[] of empty extent is undefined behavior");
		return *static_cast<Pointer>(NULL);
	}
	inline Pointer data() const
	{
		return NULL;
	}

	inline SizeType size() const
	{
		return 0;
	}
	inline bool empty() const
	{
		return true;
	}
};

// Helpers to make spans without having to specify type info
// Const versions are useful for passing to serialize visitors
template<typename It>
inline CvSpan<typename std::iterator_traits<It>::value_type, DYNAMIC_EXTENT> MakeSpan(It first, std::size_t count)
{
	return CvSpan<typename std::iterator_traits<It>::value_type, DYNAMIC_EXTENT>(first, count);
}
template<typename It>
inline const CvSpan<typename std::iterator_traits<It>::value_type, DYNAMIC_EXTENT> MakeConstSpan(It first, std::size_t count)
{
	return MakeSpan(first, count);
}

template<typename It>
inline CvSpan<typename std::iterator_traits<It>::value_type, DYNAMIC_EXTENT> MakeSpan(It first, It last)
{
	return CvSpan<typename std::iterator_traits<It>::value_type, DYNAMIC_EXTENT>(first, last);
}
template<typename It>
inline const CvSpan<typename std::iterator_traits<It>::value_type, DYNAMIC_EXTENT> MakeConstSpan(It first, It last)
{
	return MakeSpan(first, last);
}

template<typename T, std::size_t N>
inline CvSpan<T, N> MakeSpan(T(&arr)[N])
{
	return CvSpan<T, N>(arr);
}
template<typename T, std::size_t N>
inline const CvSpan<T, N> MakeConstSpan(T(&arr)[N])
{
	return MakeSpan(arr);
}

template<typename T, std::size_t N>
inline CvSpan<T, N> MakeSpan(const CvSpan<T, N>& src)
{
	return CvSpan<T, N>(src);
}
template<typename T, std::size_t N>
inline const CvSpan<T, N> MakeConstSpan(const CvSpan<T, N>& src)
{
	return MakeSpan(src);
}

#endif

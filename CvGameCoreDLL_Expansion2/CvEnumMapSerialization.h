#pragma once

#ifndef CVINFOMAPSERIALIZATION_H
#define CVINFOMAPSERIALIZATION_H

#include "CvEnumMap.h"
#include "CvSpanSerialization.h"

template<typename Enum, typename T>
inline FDataStream& operator<<(FDataStream& saveTo, const CvEnumMapDynamic<Enum, T>& readFrom)
{
	const CvSpan<const T> span(readFrom.begin(), readFrom.end());
	return saveTo << span;
}
template<typename Enum, typename T>
inline FDataStream& operator>>(FDataStream& loadFrom, CvEnumMapDynamic<Enum, T>& writeTo)
{
	const CvSpan<T> span(writeTo.begin(), writeTo.end());
	return loadFrom >> span;
}

template<typename Enum, typename T>
inline FDataStream& operator<<(FDataStream& saveTo, const CvEnumMapFixed<Enum, T>& readFrom)
{
	const CvSpan<const T, CvEnumMapFixed<Enum, T>::SizeConstant> span(readFrom.begin(), readFrom.end());
	return saveTo << span;
}
template<typename Enum, typename T>
inline FDataStream& operator>>(FDataStream& loadFrom, CvEnumMapFixed<Enum, T>& writeTo)
{
	const CvSpan<T, CvEnumMapFixed<Enum, T>::SizeConstant> span(writeTo.begin(), writeTo.end());
	return loadFrom >> span;
}

template<typename Enum, typename T>
inline FDataStream& operator<<(FDataStream& saveTo, const CvEnumMap<Enum, T>& readFrom)
{
	return saveTo << static_cast<const typename CvEnumMap<Enum, T>::ConcreteType&>(readFrom);
}
template<typename Enum, typename T>
inline FDataStream& operator>>(FDataStream& loadFrom, CvEnumMap<Enum, T>& writeTo)
{
	return loadFrom >> static_cast<typename CvEnumMap<Enum, T>::ConcreteType&>(writeTo);
}

#endif
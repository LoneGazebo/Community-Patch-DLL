#pragma once

#ifndef CVINFOMAPSERIALIZATION_H
#define CVINFOMAPSERIALIZATION_H

#include "CvEnumMap.h"
#include "CvSpanSerialization.h"

template<typename Enum, typename T, bool Fixed>
inline FDataStream& operator<<(FDataStream& saveTo, const CvEnumMap<Enum, T, Fixed>& readFrom)
{
	const CvSpan<const T> span(readFrom.begin(), readFrom.end());
	return saveTo << span;
}
template<typename Enum, typename T, bool Fixed>
inline FDataStream& operator>>(FDataStream& loadFrom, CvEnumMap<Enum, T, Fixed>& writeTo)
{
	const CvSpan<T> span(writeTo.begin(), writeTo.end());
	return loadFrom >> span;
}

template<typename Enum, typename T>
inline FDataStream& operator<<(FDataStream& saveTo, const CvEnumMap<Enum, T, true>& readFrom)
{
	const CvSpan<const T, CvEnumMap<Enum, T, true>::SizeConstant> span(readFrom.begin(), readFrom.end());
	return saveTo << span;
}
template<typename Enum, typename T>
inline FDataStream& operator>>(FDataStream& loadFrom, CvEnumMap<Enum, T, true>& writeTo)
{
	const CvSpan<T, CvEnumMap<Enum, T, true>::SizeConstant> span(writeTo.begin(), writeTo.end());
	return loadFrom >> span;
}

#endif
#pragma once

#ifndef CVSPANSERIALIZATION_H
#define CVSPANSERIALIZATION_H

#include "CvSpan.h"
#include "FDataStream.h"

template<typename T, std::size_t N>
FDataStream& operator<<(FDataStream& saveTo, const CvSpan<T, N>& readFrom)
{
	for (typename CvSpan<T, N>::Iterator it = readFrom.begin(); it != readFrom.end(); ++it)
	{
		saveTo << *it;
	}
	return saveTo;
}
template<typename T, std::size_t N>
FDataStream& operator>>(FDataStream& loadFrom, const CvSpan<T, N>& writeTo)
{
	for (typename CvSpan<T, N>::Iterator it = writeTo.begin(); it != writeTo.end(); ++it)
	{
		loadFrom >> *it;
	}
	return loadFrom;
}

template<typename T>
FDataStream& operator<<(FDataStream& saveTo, const CvSpan<T, DYNAMIC_EXTENT>& readFrom)
{
	for (typename CvSpan<T, DYNAMIC_EXTENT>::Iterator it = readFrom.begin(); it != readFrom.end(); ++it)
	{
		saveTo << *it;
	}
	return saveTo;
}
template<typename T>
FDataStream& operator>>(FDataStream& loadFrom, const CvSpan<T, DYNAMIC_EXTENT>& writeTo)
{
	for (typename CvSpan<T, DYNAMIC_EXTENT>::Iterator it = writeTo.begin(); it != writeTo.end(); ++it)
	{
		loadFrom >> *it;
	}
	return loadFrom;
}

#endif
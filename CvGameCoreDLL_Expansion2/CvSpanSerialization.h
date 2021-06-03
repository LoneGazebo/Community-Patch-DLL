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
	// Dynamic span assumes the data we're saving may change in size so we save it
	const std::size_t size = readFrom.size();
	saveTo << size;
	for (typename CvSpan<T, DYNAMIC_EXTENT>::Iterator it = readFrom.begin(); it != readFrom.end(); ++it)
	{
		saveTo << *it;
	}
	return saveTo;
}
template<typename T>
FDataStream& operator>>(FDataStream& loadFrom, const CvSpan<T, DYNAMIC_EXTENT>& writeTo)
{
	// Dynamic span cannot be resized, but we should verify that the size is correct
	std::size_t size;
	loadFrom >> size;
	FAssertMsg(size == writeTo.size(), "Mismatched span size");
	if (size != writeTo.size()) // The save data is incompatible and we cannot safely continue
		std::exit(0); // FIXME - should probably log this error...
	for (typename CvSpan<T, DYNAMIC_EXTENT>::Iterator it = writeTo.begin(); it != writeTo.end(); ++it)
	{
		loadFrom >> *it;
	}
	return loadFrom;
}

#endif
#pragma once

#ifndef CVALIGNEDSTORAGE_H
#define CVALIGNEDSTORAGE_H

#include <cstddef>

template<typename T, std::size_t A = __alignof(T)>
struct CvAlignedStorage;

template<typename T>
struct CvAlignedStorage<T, 1>
{
	struct Type
	{
		char data[sizeof(T)];
	};
};

template<typename T>
struct CvAlignedStorage<T, 2>
{
	union Type
	{
		char data[sizeof(T)];
		short _;
	};
};

template<typename T>
struct CvAlignedStorage<T, 4>
{
	union Type
	{
		char data[sizeof(T)];
		int _;
	};
};

template<typename T>
struct CvAlignedStorage<T, 8>
{
	union Type
	{
		char data[sizeof(T)];
		long long _;
	};
};

#endif // CVALIGNEDSTORAGE_H

//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FStlContainerSerialization.h
//
//  AUTHOR:		Justin Randall
//
//  PURPOSE:	Support STL containers with any type serializable with FDataStream.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef _INCLUDED_FStlContainerSerialization_H
#define _INCLUDED_FStlContainerSerialization_H

//---------------------------------------------------------------------
// Specific container specializations help the compiler associate
// the right code for the type serializing with an FDataStream. 
//
// Many containers are represented identically in an FDataStream, so
// use more generic methods for moving data to/from the FDataStream.
// For example, lists, vectors and deques are all sequence containers
// and so all use the same load/save code, but specify explicit 
// specializations to match the template with the container calling
// the shift operator. 
//
// maps and sets are associative containers.
// Though hash_maps aren't implemented in this file, they can use the 
// same associative container code here and simply provide the 
// specialized template wrapper.
//---------------------------------------------------------------------

#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <unordered_set>


template<typename FirstType, typename SecondType>
FDataStream & operator<<(FDataStream & saveTo, const std::pair<FirstType, SecondType> & readFrom)
{
	saveTo << readFrom.first;
	saveTo << readFrom.second;
	return saveTo;
}

template<typename FirstType, typename SecondType>
FDataStream & operator>>(FDataStream & loadFrom, std::pair<FirstType, SecondType> & writeTo)
{
	loadFrom >> writeTo.first;
	loadFrom >> writeTo.second;
	return loadFrom;
}

// STL sequence containers are all serialized the same way.
// deques, vectors and lists are identical in a stream, so
// the Serialize[To/From]SequenceContainer functors
// handle the specifics of serialization.
template<typename ElementType, typename ContainerType>
struct SerializeFromSequenceContainer
{
	SerializeFromSequenceContainer(FDataStream & saveTo, const ContainerType & container) :
	m_saveTo(saveTo)
	, m_container(container)
	{
		m_saveTo << container.size();
	}

	void operator() (ElementType & i)
	{
		m_saveTo << i;
	}

	FDataStream & m_saveTo;
	const ContainerType & m_container;
};
template<typename ElementType, typename ContainerType>
void SerializeToSequenceContainer(FDataStream & loadFrom, ContainerType & container)
{
	container.clear();
	ContainerType::size_type count = 0;
	loadFrom >> count;
	ContainerType::size_type i = 0;
	for(i = 0; i < count; ++i)
	{
		ElementType v;
		loadFrom >> v;
		container.push_back(v);
	}
}

/*
template<typename ElementType, typename ContainerType>
struct SerializeToSequenceContainer
{
	SerializeToSequenceContainer(FDataStream & loadFrom, ContainerType & container) :
	m_loadFrom(loadFrom)
	, m_container(container)
	{
		container.clear();
		size_t count = 0;
		m_loadFrom >> count;
		container.resize(count);
	}

	void operator() (ElementType & i)
	{
		m_loadFrom >> i;
	}

	FDataStream & m_loadFrom;
	ContainerType & m_container;
};
*/
template<typename ElementType, typename ContainerType>
struct SerializeFromAssociativeContainer
{
	SerializeFromAssociativeContainer(FDataStream & saveTo, ContainerType & container) :
	m_saveTo(saveTo)
	, m_container(container)
	{
		size_t count = container.size();
		m_saveTo << count;
	}

	void operator()(const ElementType & i)
	{
		m_saveTo << i;
	}

	FDataStream & m_saveTo;
	ContainerType & m_container;
};

template<typename ElementType, typename ContainerType>
void SerializeToAssociativeContainer(FDataStream & loadFrom, ContainerType & container)
{
	container.clear();
	size_t count = 0;
	loadFrom >> count;
	size_t i = 0;
	for(i = 0; i < count; ++i)
	{
		ElementType e;
		loadFrom >> e;
		container.insert(e);
	}
}

template<typename KeyType, typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const std::map<KeyType, ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<std::pair<KeyType, ElementType>, const std::map<KeyType, ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename KeyType, typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const stdext::hash_map<KeyType, ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<std::pair<KeyType, ElementType>, const stdext::hash_map<KeyType, ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename KeyType>
FDataStream & operator <<(FDataStream & saveTo, const std::tr1::unordered_set<KeyType>& readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<KeyType, const std::tr1::unordered_set<KeyType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const std::set<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromAssociativeContainer<ElementType, const std::set<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType> 
FDataStream & operator<<(FDataStream & saveTo, const std::deque<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromSequenceContainer<const ElementType, const std::deque<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType> 
FDataStream & operator<<(FDataStream & saveTo, const std::list<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromSequenceContainer<const ElementType, const std::list<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType> 
FDataStream & operator<<(FDataStream & saveTo, const std::vector<ElementType> & readFrom)
{
	std::for_each(readFrom.begin(), readFrom.end(), SerializeFromSequenceContainer<const ElementType, const std::vector<ElementType> >(saveTo, readFrom));
	return saveTo;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::deque<ElementType> & writeTo)
{
	// The functor needs to be instantiated to properly resize the container based 
	// on how many elements the stream says it should have before passing it along
	// to std::for_each
	SerializeToSequenceContainer<ElementType, std::deque<ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::list<ElementType> & writeTo)
{
	// The functor needs to be instantiated to properly resize the container based 
	// on how many elements the stream says it should have before passing it along
	// to std::for_each
	SerializeToSequenceContainer<ElementType, std::list<ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::vector<ElementType> & writeTo)
{
	// The functor needs to be instantiated to properly resize the container based 
	// on how many elements the stream says it should have before passing it along
	// to std::for_each
	SerializeToSequenceContainer<ElementType, std::vector<ElementType> >(loadFrom, writeTo);

	//std::for_each(writeTo.begin(), writeTo.end(), func);
	return loadFrom;
}

template<typename KeyType, typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::map<KeyType, ElementType> & writeTo)
{
	SerializeToAssociativeContainer<std::pair<KeyType, ElementType>, std::map<KeyType, ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename KeyType, typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, stdext::hash_map<KeyType, ElementType> & writeTo)
{
	SerializeToAssociativeContainer<std::pair<KeyType, ElementType>, stdext::hash_map<KeyType, ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename KeyType>
FDataStream & operator>>(FDataStream & loadFrom, std::tr1::unordered_set<KeyType> & writeTo)
{
	SerializeToAssociativeContainer<KeyType, std::tr1::unordered_set<KeyType> >(loadFrom, writeTo);
	return loadFrom;
}

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, std::set<ElementType> & writeTo)
{
	SerializeToAssociativeContainer<ElementType, std::set<ElementType> >(loadFrom, writeTo);
	return loadFrom;
}

FDataStream & operator<<(FDataStream & saveTo, const std::string & readFrom);
FDataStream & operator>>(FDataStream & loadFrom, std::string & writeTo);

#endif//_INCLUDED_FStlContainerSerialization_H

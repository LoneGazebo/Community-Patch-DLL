//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FDataStream.h
//
//  AUTHOR:		Mustafa Thamer	--  11/25/2003
//
//  PURPOSE:	Basic stream class.
//				Easily read and write data to those kinds of streams using the same baseclass interface.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2006 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FDATASTREAM_H
#define		FDATASTREAM_H
#pragma		once

#include "FFastVector.h"
#include "FArray.h"
#include "FEndian.h"

class YouMustImplementASerializeOperatorForThisTypeError;
class FStringA;
class FStringW;

//
// Stream base class
//
class FDataStream
{
public:
	// Types:
	//------
	//typedef void (*CallbackFxnType)();	// register an optional CB fxn.  It will be called every time a read/write occurs

	// Constructors:
	//-------------
	FDataStream(void);

	// Destructors:
	//------------
	virtual	~FDataStream(void);

	// Operators:
	//----------
	template <typename T>
	FDataStream& operator<<(const T& kData);
	template <typename T>
	FDataStream& operator>>(T& kData);
	// Abstract:
	//---------
	virtual unsigned int ReadIt(unsigned int byteCount, void* buffer) = 0;
	virtual unsigned int WriteIt(unsigned int byteCount, const void* buffer) = 0;
	virtual void Skip(unsigned int deltaByteCount) = 0;
	virtual void Rewind() = 0;

	// Virtual:
	//--------
	virtual bool AtEnd() const = 0;
	virtual void FastFwd() = 0;
	virtual unsigned int GetBytesUsed() const;
	virtual unsigned int GetPosition() const;
	virtual void SetPosition(unsigned int position);
	virtual void Truncate();
	virtual void Flush();
	virtual unsigned int GetEOF() const;
	virtual unsigned int GetSizeLeft() const;
	virtual void CopyToMem(void* mem);
	virtual void RaiseException( std::exception& kException ) const;

	// Methods:
	//--------

	// Endianness
	void SetDesiredEndianNess(FEndian::EEndianness e) { m_eEndianness = e; }
	FEndian::EEndianness GetDesiredEndianNess() const { return m_eEndianness; }

	// Deprecate these
	unsigned int WriteString(const char* szName);
	unsigned int WriteString(const wchar* szName);
	char* ReadString();			// allocates and returns memory , be sure to delete it, or there will be a leak
	wchar* ReadWideString();		// allocates and returns memory, be sure to delete it or there will be a leak
	unsigned int ReadString(char* szName); // reads characters from stream, can't check szName for size, may overrun memory
	unsigned int ReadString(wchar* szName); // reads characters from stream, can't check szName for size, may overrun memory

protected:
	// only access through shift operators << and >>
	// Writing
	unsigned int WriteString(const FStringA& szName);
	unsigned int WriteString(int count, const FStringA values[]);
	unsigned int WriteString(const FStringW& szName);
	unsigned int WriteString(int count, const FStringW values[]);
	unsigned int WriteString(const std::string& szName);
	unsigned int WriteString(int count, const std::string values[]);
	unsigned int WriteString(const std::wstring& szName);
	unsigned int WriteString(int count, const std::wstring values[]);
	void Write(const YouMustImplementASerializeOperatorForThisTypeError & szName);
	void Write(const FStringA& szName);
	void Write(int count, const FStringA values[]);
	void Write(const FStringW& szName);
	void Write(int count, const FStringW values[]);
	void Write(const std::string& szName);
	void Write(int count, const std::string values[]);
	void Write(const std::wstring& szName);
	void Write(int count, const std::wstring values[]);

	/*template <typename T>
	void Write(const T& value);*/
	template <typename T>
	void Write(int count, const T values[]);

	void Write(const GUID &); // JAR : todo  -- GUID isn't portable and should probably be handled elsewhere
	void Write(const bool &);
	void Write(const unsigned char &);
	void Write(const char &);
	void Write(const INT32 &);
	void Write(const UINT32 &);
	void Write(const UINT64 &);
	void Write(const INT64 &);
#ifndef		_NDS
	// On DS, UINT32 is a define for u32 which is a define for "unsigned long"
	void Write(const unsigned long &);
#endif	//	_NDS
	void Write(const INT16 &);
	void Write(const UINT16 &);
	void Write(const WCHAR &);
	void Write(const float &);
	void Write(const double &);

	void Write(int, const INT32 values[]);
	void Write(int, const UINT32 values[]);
	void Write(int, const INT64 values[]);
	void Write(int, const UINT64 values[]);
#ifndef		_NDS
	// On DS, UINT32 is a define for u32 which is a define for "unsigned long"
	void Write(int, const unsigned long values[]);
#endif	//	_NDS
	void Write(int, const INT16 values[]);
	void Write(int, const UINT16 values[]);
	void Write(int, const WCHAR values[]);
	void Write(int, const float values[]);
	void Write(int, const double values[]);
	void Write(int, const GUID values[]);

		// Reading
	unsigned int ReadString(FStringA& szName);
	unsigned int ReadString(int count, FStringA values[]);
	unsigned int ReadString(FStringW& szName);
	unsigned int ReadString(int count, FStringW values[]);
	unsigned int ReadString(std::string& szName);
	unsigned int ReadString(int count, std::string values[]);
	unsigned int ReadString(std::wstring& szName);
	unsigned int ReadString(int count, std::wstring values[]);
	
	void Read(YouMustImplementASerializeOperatorForThisTypeError & szName);
	void Read(FStringA& szName);
	void Read(int count, FStringA values[]);
	void Read(FStringW& szName);
	void Read(int count, FStringW values[]);
	void Read(std::string& szName);
	void Read(int count, std::string values[]);
	void Read(std::wstring& szName);
	void Read(int count, std::wstring values[]);
	/*template <class T>
	void Read(T& value); */
	
	template <class T>
	void Read(int count, T values[]);
	
	void Read(GUID &);
	void Read(bool &);
	void Read(unsigned char &);
	void Read(char &);
	// specializations for plain old data types
	void Read(INT32 &);
	void Read(UINT32 &);
	void Read(UINT64 &);
	void Read(INT64 &);
#ifndef		_NDS
	// On DS, UINT32 is a define for u32 which is a define for "unsigned long"
	void Read(unsigned long &);
#endif	//	_NDS
	void Read(INT16 &);
	void Read(UINT16 &);
	void Read(WCHAR &);
	void Read(float &);
	void Read(double &);

	void Read(int, GUID values[]); 
	void Read(int, INT32 values[]);
	void Read(int, UINT32 values[]);
	void Read(int, INT64 values[]);
	void Read(int, UINT64 values[]);
#ifndef		_NDS
	// On DS, UINT32 is a define for u32 which is a define for "unsigned long"
	void Read(int, unsigned long values[]);
#endif	//	_NDS
	void Read(int, INT16 values[]);
	void Read(int, UINT16 values[]);
	void Read(int, WCHAR values[]);
	void Read(int, float values[]);
	void Read(int, double values[]);



protected:
	// Members:
	//--------
	bool m_bEnableCB;
	FEndian::EEndianness m_eEndianness;		// the desired endianness of data

	unsigned int m_uiBytesRead;
	unsigned int m_uiPosition;	
};

FDataStream & operator<<(FDataStream & saveTo, FDataStream & readFrom);
FDataStream & operator>>(FDataStream & loadFrom, FDataStream & writeTo);

template<typename ValueType, size_t count>
FDataStream & operator<<(FDataStream & saveTo, const ValueType (&readFrom)[count])
{
	size_t i = 0;
	for(i = 0; i < count; ++i)
	{
		saveTo << readFrom[i];
	}
	return saveTo;
}

template<typename ValueType, size_t count>
FDataStream & operator>>(FDataStream & loadFrom, ValueType (&writeTo)[count])
{
	size_t i = 0;
	for(i = 0; i < count; ++i)
	{
		loadFrom >> writeTo[i];
	}
	return loadFrom;
}

template < class T >
inline FDataStream& FDataStream::operator <<( const T& kData )
{
	// If the compiler produces an error here complaining about 
	// YouMustImplementASerializeOperatorForThisTypeError then
	// it could not find a specialization for the type supplied
	// in scope. If a specialization exists, be sure to include
	// the header (FStlContainerSerialization.h, for example), 
	// if not, implement :
	// FDataStream & operator<<(FDataStream &, YourCustomType &)
	Write( kData );
	return ( *this );
}

template < class T >
inline FDataStream& FDataStream::operator >>( T& kData )
{
	// If the compiler produces an error here complaining about 
	// YouMustImplementASerializeOperatorForThisTypeError then
	// it could not find a specialization for the type supplied
	// in scope. If a specialization exists, be sure to include
	// the header (FStlContainerSerialization.h, for example), 
	// if not, implement :
	// FDataStream & operator>>(FDataStream &, YourCustomType &)
	Read( kData );
	return ( *this );
}

/* JAR  - disabling this catch-all. It takes any non-specialized
          type, gets sizeof(T), then dumps it through a void *
		  producing a raw memory dump of the type in the stream.
		  Disabling this uncovered dozens of probably unknown bugs 
		  in serialization.

                                    ________
                              , -‘”          ``~ ,
                          , -”                    “- ,
                        ,/                           ”:,
                     ,?                                 \,
                    /                                    ,}
                  /                               ,:`^`  }
                /                              ,:”       /
               ?    __                        :`        /
              /__ (   “~-,_                 ,:`        /
             /(_  ”~,_    “~,_             ,:`       _/
            {  _$;_  ”=,_    “-,_     ,-~-,}, ~”;/  }
             ((   *~_  ”=- _    “;,, /`  /”          /
     ,,,___ \`~,     “~ ,           `     }         /
              (  `=-,,   `                 (  ;_,,-”
              / `~,      `-                 \   /\
               \`~ *-,                       |, / \,__
  ,,_          } >- _\                      |         `=~-,
       `=~-,_\_      `\,                     \
                  `=~-,, \,                    \
                            `:,,                `\        __
                                `=-,            ,%`>--==``
                                   _\    _,-%       `\
                                 ,<`  _|_,-&``      `\

             P I C A R D    F A C E P A L M
Because expressing how dumb that was in words just doesn't work

template < class T >
inline void FDataStream::Write( const T& value )
{
	WriteIt( sizeof ( T ), &value );
}
template < class T >
inline void FDataStream::Read( T& value )
{
ReadIt( sizeof ( T ), &value );
}
*/
template < class T >
inline void FDataStream::Write( int count, const T values[] )
{
	if (sizeof(T) == 1)
		WriteIt( count, values );
	else
	{
		int i;
		for(i=0;i<count;i++)
		{
			//Write( values[i] );
			*this << values[i];
		}
	}
}

template < class T >
inline void FDataStream::Read( int count, T values[] )
{
	if (sizeof(T) == 1)
		ReadIt( count, values );
	else
	{
		int i;
		for(i=0;i<count;i++)
		{
			// JAR  - comment this out, allow data stream to 
			// call user-supplied serialization, and not
			// restrict the class to the hard-coded types
			// available here
		//	Read( values[i] );
			*this >> values[i];
		}
	}
}

/*
template<typename T>
FDataStream & operator<<(FDataStream & saveTo, const T & readFrom)
{
	saveTo.Write(readFrom);
	return saveTo;
}

template<typename T>
FDataStream & operator>>(FDataStream & loadFrom, T & writeTo)
{
	loadFrom.Read(writeTo);
	return loadFrom;
}
*/

//-----------------------------------------------------------------------
// Support a bunch of legacy code that passes raw pointer data, arrays with magic number counts
// and dynamically allocated arrays. These should probably be encapsulated in containers
// for safety and manageability (std::vector, for example)
//-----------------------------------------------------------------------
template<typename ValueType>
class ArrayWrapper
{
public:
	ArrayWrapper(int count, ValueType * values)
		: m_values(values), m_count(count) {}

	ValueType * getArray()
	{
		return m_values;
	}
	const ValueType * getArray() const
	{
		return m_values;
	}

	int getCount() const
	{
		return m_count;
	}

	// Read an array, assuming there is a count value at the current position
	// in the stream.  If the count is less that the array size, the remaining entries
	// in the array will be filled with the supplied value.  If the count is greater than
	// the array, the remaining items in the stream will be skipped
	void readCounted(FDataStream & readFrom, ValueType tFill = (ValueType)0)
	{
		int i = 0;
		int iStreamCount;
		readFrom >> iStreamCount;
		int iArrayCount = getCount();
		int readCount = (iStreamCount < iArrayCount)?iStreamCount:iArrayCount;
		ValueType * values = getArray();
		for(i = 0; i < readCount; ++i)
		{
			readFrom >> values[i];
		}
		if (iStreamCount < iArrayCount)
		{
			for(; i < iArrayCount; ++i)
			{
				values[i] = tFill;
			}
		}
		else
		if (iStreamCount > iArrayCount)
		{
			for(; i < iStreamCount; ++i)
			{
				readFrom >> tFill;
			}		
		}
		
	}

	// Writes out the array, with a count value first.
	void writeCounted(FDataStream & writeTo)
	{
		int i = 0;
		int count = getCount();
		writeTo << count;
		const ValueType * values = getArray();
		for(i = 0; i < count; ++i)
		{
			writeTo << values[i];
		}
	}

private:
	// disabled, this wrapper does not belong in containers 
	// or allow copy construction
	ArrayWrapper();
	ArrayWrapper(const ArrayWrapper &);
	ArrayWrapper & operator=(const ArrayWrapper &);

	ValueType * m_values;
	int         m_count;
};

//Const version for writing (stores only a const pointer)
template< typename ValueType >
class ArrayWrapperConst
{
public:
	ArrayWrapperConst(int count, const ValueType * values)
		: m_values(values), m_count(count) {}

	const ValueType* getArray() const
	{
		return m_values;
	}
	int getCount() const
	{
		return m_count;
	}

	// Writes out the array, with a count value first.
	void writeCounted(FDataStream & writeTo)
	{
		int i = 0;
		int count = getCount();
		writeTo << count;
		const ValueType * values = getArray();
		for(i = 0; i < count; ++i)
		{
			writeTo << values[i];
		}
	}

private:

	// disabled, this wrapper does not belong in containers 
	// or allow copy construction
	ArrayWrapperConst();
	ArrayWrapperConst(const ArrayWrapperConst &);
	ArrayWrapperConst & operator=(const ArrayWrapperConst &);

	const ValueType * m_values;
	int         m_count;
};

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// Stream operators for array wrapper classes
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

template<typename ValueType>
FDataStream & operator<<(FDataStream & saveTo, const ArrayWrapper<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	const ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		saveTo << values[i];
	}
	return saveTo;
}

template<typename ValueType>
FDataStream & operator>>(FDataStream & loadFrom, ArrayWrapper<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		loadFrom >> values[i];
	}
	return loadFrom;
}

template<typename ValueType>
FDataStream & operator<<(FDataStream & saveTo, const ArrayWrapperConst<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	const ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		saveTo << values[i];
	}
	return saveTo;
}

template<typename ValueType>
FDataStream & operator>>(FDataStream & loadFrom, ArrayWrapperConst<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		loadFrom >> values[i];
	}
	return loadFrom;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Serialization for Firaxis::Array
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
template< class T, unsigned int ARRAY_SIZE >
FDataStream & operator >> ( FDataStream & kStream, Firaxis::Array<T, ARRAY_SIZE> & kArray )
{
	for( unsigned int i = 0; i < kArray.size(); ++i )
	{
		kStream >> kArray[i];
	}
	return kStream;
}

template< class T, unsigned int ARRAY_SIZE >
FDataStream & operator << (FDataStream & kStream, const Firaxis::Array< T, ARRAY_SIZE > & kArray )
{
	for( unsigned int i = 0; i < kArray.size(); ++i )
	{
		kStream << kArray[i];
	}
	return kStream;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Serialization for FFastVector and FStaticVector
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator >> ( FDataStream & kStream, FFastVector< T, bPODType, AllocPool, nSubID > & kVec )
{
	unsigned int iSize;
	kStream >> iSize;

	if(iSize > kVec.size())
	{
		unsigned int count = iSize - kVec.size();
		kVec.push_back_copy(T(), count);
	}
	
	unsigned int i = 0;
	for( i = 0; i < iSize; ++i )
	{
		kStream >> kVec[i];
	}
	return kStream;
}

template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator << ( FDataStream & kStream, const FFastVector< T, bPODType, AllocPool, nSubID > & kVec )
{
	kStream << kVec.size();
	for( unsigned int i = 0; i < kVec.size(); ++i )
	{
		kStream << kVec[i];
	}
	return kStream;
}

template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator >> ( FDataStream & kStream, FStaticVector< T, L, bPODType, AllocPool, nSubID > & kVec )
{
	unsigned int iSize;
	kStream >> iSize;
	kVec.resize( iSize );

	for( unsigned int i = 0; i < iSize; ++i )
	{
		kStream >> kVec[i];
	}
	return kStream;
}

template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator << ( FDataStream & kStream, const FStaticVector< T, L, bPODType, AllocPool, nSubID > & kVec )
{
	kStream << (unsigned int)kVec.size();

	for( unsigned int i = 0; i < kVec.size(); ++i )
	{
		kStream << kVec[i];
	}
	return kStream;
}

#endif	//FDATASTREAM_H


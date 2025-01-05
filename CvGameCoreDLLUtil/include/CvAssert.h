//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvAssert.h
//!  \brief     Public header of the CvAssert suite.
//!
//!		This file includes assertion macros used by Civilization.
//!
//!  Key Macros:
//!  - CvAssert(expr, ...)			- assertions w/ printf style messages
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef _CVASSERT_H
#define _CVASSERT_H
#pragma once
#include <set>

bool CvAssertDlg(const char* expr, const char* szFile, unsigned int uiLine, bool& bIgnoreAlways, const char* msg);

// Set breakpoint creation macros
#if defined (_WIN32)
#define CVASSERT_BREAKPOINT __debugbreak()
#else
#define CVASSERT_BREAKPOINT assert(0)
#endif

// Only compile in CvAssertMsg's in debug configuration, or in release configuration (with a more user-friendly text) if VPRELEASE_ERRORMSG is defined
// Asserts can be disabled by defining DISABLE_CVASSERT in the project settings
#if !defined(CVASSERT_ENABLE) && !defined(DISABLE_CVASSERT)
#if !defined(FINAL_RELEASE) || defined(VPDEBUG) || defined(VPRELEASE_ERRORMSG)
#define		CVASSERT_ENABLE
#endif	// FINAL_RELEASE
#endif	// DISABLE_CVASSERT

#ifdef CVASSERT_ENABLE 

// shows a message dialog if expr is false and CVASSERT_ENABLE is defined.
// Unlike PRECONDITION, a failed assertion does not cause the game to crash
#define ASSERT(expr, ...)																\
{																							\
	static bool bIgnoreAlways = false;														\
	if( !bIgnoreAlways && !(expr) )								                			\
	{																						\
		CvString str;																		\
		CvString::format(str, __VA_ARGS__);													\
		if(CvAssertDlg(#expr, __FILE__, __LINE__, bIgnoreAlways, str.c_str()))				\
			{ CVASSERT_BREAKPOINT; }														\
	}																						\
}

// An assert that only happens in the when CVASSERT_ENABLE is true AND it is a debug build
#ifdef _DEBUG
#define ASSERT_DEBUG( expr, ...) ASSERT( expr, __VA_ARGS__)
#else
#define ASSERT_DEBUG(expr, ...)
#endif

#if 0 // disabling Object Validation
#include "FSTLBlockAllocator.h"

template<typename ObjectType, unsigned int pool_type>
struct ObjectValidator
{
	std::set<const ObjectType *, std::less<const ObjectType *>, FSTLBlockAllocator<ObjectType,1024,pool_type> > m_deletedObjects;
	void objectAllocated(const ObjectType * obj)
	{
		m_deletedObjects.erase(obj);
	}
	void objectDestroyed(const ObjectType * obj)
	{
		m_deletedObjects.insert(obj);
	}
	bool isObjectValid(const ObjectType * obj)
	{
		bool result = (obj != 0);
		if(result) 
			result = (m_deletedObjects.find(obj) == m_deletedObjects.end());
		CvAssertMsg(result, "Object is invalid in this context");
		return result;
	}
};

#define OBJECT_VALIDATE_DEFINITION(ObjectType) ObjectValidator<ObjectType, c_eDebugSafetyNet> s_objectValidator;

#define OBJECT_ALLOCATED s_objectValidator.objectAllocated(this);;
#define OBJECT_DESTROYED s_objectValidator.objectDestroyed(this);
#define VALIDATE_OBJECT() s_objectValidator.isObjectValid(this);
#else
#define OBJECT_VALIDATE_DEFINITION(ObjectType)
#define OBJECT_ALLOCATED
#define OBJECT_DESTROYED
#define VALIDATE_OBJECT() PRECONDITION(this, "Null pointer exception")
#endif

#else	//CVASSERT_ENABLE == FALSE

#define ASSERT(expr, ...)
#define ASSERT_DEBUG(expr, ...)

#define OBJECT_VALIDATE_DEFINITION(ObjectType)
#define OBJECT_ALLOCATED
#define OBJECT_DESTROYED
#define VALIDATE_OBJECT()

#endif


#endif // _CVASSERT_H
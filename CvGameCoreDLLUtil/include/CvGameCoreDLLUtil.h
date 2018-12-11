//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvGameCoreDLLUtil.h
//!  \brief     Public header of the CvGameCoreDLLUtil library.
//!
//!		This library includes classes utilized by CvGameCoreDLL but not fully
//!		published in the source code.
//!
//!  Key Classes:
//!	 - CvAssert*	- Civilization Assert Macros
//!	 - CvDataStream - Generic data stream
//!  - CvString		- Generic string class
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef CvGameCoreDLLUtil_h
#define CvGameCoreDLLUtil_h
#pragma once

#include <limits>

using namespace std;

// JAR : Initialize your variables, don't desync an MP game and make me cry
#pragma warning ( 1 : 4701) 

// JAR : and if you ignore the warning, I make you cry
#pragma warning ( error : 4701 )

#include "CvDLLUtilDefines.h"
#include "CvEnums.h"
#include "CvString.h"
#include "CvStructs.h"
#include "CvAssert.h"

struct CompareFirst
{
  CompareFirst(int val) : val_(val) {}
  bool operator()(const std::pair<int,int>& elem) const {
    return val_ == elem.first;
  }
  private:
    int val_;
};

struct CompareSecond
{
  CompareSecond(int val) : val_(val) {}
  bool operator()(const std::pair<int,int>& elem) const {
    return val_ == elem.second;
  }
  private:
    int val_;
};

#endif	// CvGameCoreDLLUtil_h

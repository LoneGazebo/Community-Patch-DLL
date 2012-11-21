//-----------------------------------------------------------------------------
//
//  FRemark.h
//
//      Run-time configurable debug comment facility.
//
//      David McKibbin  --  1/4/2002
//		Dominic Cerquetti -- 9/11/07
//
//-----------------------------------------------------------------------------
//  Copyright (c) 1997-2007 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef		FREMARK_H
#define		FREMARK_H
#pragma		once

#include "FAssert.h"

// Only compile in Remarks's if REMARK_ENABLE is defined.  By default, however, let's key off of
// _DEBUG.  Sometimes, however, it's useful to enable asserts in release builds, and you can do that
// simply by changing the following lines to define REMARK_ENABLE or using project settings to override
#if !defined(FINAL_RELEASE) && !defined(PROFILE)
	#define		REMARK_ENABLE
#endif	

void   dprintf( const char* format, ... );    // old standby: printf() to OutputDebugString
void   dxPrint( const char* format, ... );    // new version: printf() to dxPrinter
void   dxPrintNL( const char* format, ... );  // new version: printf() to dxPrinter (with auto-NL)
typedef void(*dxPRINTPROC)( char* txt, int level ); // function to receive dxPrint output
extern  void(*dxPrinter)( char* txt, int level ); // direct access to our printer: dxPrinter( "raw text\n" )
dxPRINTPROC dxPrintHook( dxPRINTPROC newPrinter=0 );    // returns old printer

#define MAIN "MAIN"


//-----------------------------------------------------------------------------
//  REMARK_GROUP macro  --  auto insert group name into Remark() call
//
//      Creates a private (static) Remark() function for each module.
//      This function provides an early-out if remarks are turned OFF,
//      or the input level is non-zero and all remarks are set to level_0.
//      It then calls _Remark() with the group name added.
//
//      For normal usage, the local Remark() function created needs to
//      be "static" since each group has its own private copy.  Our
//      default group (MAIN) created in Remark.cpp can be accessed
//      for general usage using RemarkTo().
//
//      For some reason, "AddRemarkGroup( name );" below does NOT string
//      pool its string with the string from "*group = name;"  Maybe due
//      to the AddRemarkGroup being prior to WinMain and hence invisible
//      and magic.  So change AddRemarkGroup's arg to "group".
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  Remark() usage:
//
//      The Remark facility allows the output of debug information to be
//  selectively enabled or disabled at run-time.  It does this by assigning
//  each remark to both a group and a level.  The reporting level for each
//  group (or all groups) can be set at run-time via the command line, or
//  by function calls.  Remark()'s output can be revectored to allow tools
//  to capture the stream for printing to a window, etc.
//
//  bool Remark( int level, char *format, ... )
//  ------------------------------------------
//      Each Remark() selects a reporting or importance level as follows:
//
//          0 = super   Remark(1, "Heap %s grew past expected size", myHeap )
//          ----------
//          1 = high		Remark(2, "Opening file %s...", myFile )
//          2 = medium		Remark(3, "Closing file %s...", myFile )
//          3 = low			Remark(4, "Reading %d bytes from file %s...", mySize, myFile )
//          4 = debugging	Remark(4, "Incremented loop variable %i", i )
//
//      Each of the levels (1-2-3-4) allow deeper and deeper probing into a
//  subsystem during debugging.  In the above example, starting at level_1
//  would show only file opens.  Level_2 would add the corresponding file
//  closings and level_3 would add all file accesses.  Level_0 is always
//  enabled and is used for violations that are not quite of the Verify()
//  caliber but are still very serious.  Examples are things that are out
//  of expected range, but do not corrupt memory or program execution.
//
//  N.B. - Remark() adds a trailing return (\n) to each line.
//  
//   
//  REMARK_GROUP( "Sound" )
//  ----------------------
//      Each Remark() is automatically assigned to a reporting group via
//  the REMARK_GROUP macro.  This allows groups to be assigned individual
//  reporting levels and also causes the group name to be prefixed to all
//  its remarks.  Each compilation unit will be assigned to one and only
//  one remark group.  However, several compilation units can be assigned
//  to the same remark group either explicitly or by including the macro
//  in their StdAfx.h file.  Both methods require that string pooling be
//  enabled (aka Eliminate duplicate strings).  This option (/Gf) is auto-
//  matically enabled by /ZI:edit&continue or /O1 or /O2.  Since we are
//  not using edit&continue (since it adds 64 bytes to every stack frame)
//  we need to add /Gf to all Debug build settings.  The symptoms of not
//  doing this will be multiple copies of the group name in the group list.
//  Remark groups are distinguished by the address of their text string.
//  mgCore defines remark group "MAIN" which is the default group for all
//  remarks w/o an associated REMARK_GROUP macro.
//
//      The REMARK_GROUP macro generates a local Remark() function with
//  an early-out feature that immediately returns if all remark levels
//  are off.  Otherwise it adds the group name and calls _Remark().  For
//  efficiency reasons, if all remarks are off (/R0), then even level_0
//  remarks are off.  If any remark group is enabled, then all level_0
//  remarks are enabled.  By default, remark group MAIN is set to level_1.
//  This will thus enable all level 0 remarks for all groups.
//
//      Normally, all Remarks in a compilation unit are assigned to only
//  one remark group.  However, using the helper function directly you
//  assign a remark to any remark group as follows:
//
//      _Remark( "Video", 2, "Playing video %s", movieTitle );
//  
//      
//  Querying Your Remark Level
//  --------------------------
//      Remark() returns a bool indicating if the requested level is active
//  and hence whether output occurred.  Remark() is declared with the format
//  string defaulted to zero.  This allows Remark() to be used to query the
//  status of a level and create block remarks with the group name only on
//  the first line as in:
//
//      if (Remark(2, "Camera position" ))  --or-- Remark(2)
//      {
//          dxPrint( "    x-pos: %6f \n", camera.x );
//          dxPrint( "    y-pos: %6f \n", camera.y );
//          dxPrint( "    z-pos: %6f \n", camera.z );
//      }
//
//
//  Setting Remark Levels (via command line)
//  ----------------------------------------
//      To enable groups at run-time using command line switches:
//
//           /R1:Sound      enable "Sound" group at level 1
//           /R2:X-Box      enable "X-Box" group at level 2
//           /R3            enable   ALL  groups at level 3 (everything)
//           /R0:Video      disable "Video" group
//
//      You can use as many as required.  Also, the string comparison is
//  not case sensitive and has an implicit wildcard at the end, for example
//  "sound" would match groups: "SOUND", "SoundBox", "SoundRender", etc.
//
//      Further, switches are processed in order, so to enable everything at
//  level 1, but disable "Music" group use: /R1 /R0:Music.  Or to temporarily
//  disable everything, but retain all command switches for later, just add
//  "/R0" to the end of the command line.
//
//  
//  Setting Remark Levels (via function calls)
//  ------------------------------------------
//      Remark reporting levels can be set at run-time using function calls:
//
//      SetRemarkLevels( "/R1 /R0:Music" ); // "Music" off, all else level_1
//      SetRemarkLevels( "/R0 /R1:Video" ); // "Video" level_1, all else off
//      SetRemarkLevels( "/R2:mgTerrain" ); // "mgTerrain" level_2, all else no change
//
//      Another function call interface exists primarily for tools and remote use:
//
//      char** GetRemarkGroupList();
//      uint   GetRemarkLevel( int group );
//      void   SetRemarkGroupCutoffLevel( int group, int level );
//      void   SetRemarkLevelAll( int level );
//      char** GetRemarkGroupList() - returns the current list of remark groups.
//
//      You can walk the group list either by count or by end-sentinel:
//
//          for (int i=0; i<g_RemarkGroups; i++) { ... }
//
//                  -- or --
//
//          char **myList = GetRemarkGroupList();
//          while (*myList) { ..., myList++; }
//
//      Then the zero-based index into the GroupList can be used to get/set
//      the reporting level for that group.
//
//
//  Setting Remark Levels (real-time)
//  ---------------------------------
//      While debugging, you can change the Remark level by examining the
//  g_RemarkGroup[] array to find the index of the group (currently [0..31])
//  and then setting its bit-pair in "g_RemarkLevel" as follows:
//
//          g_RemarkLevel |= (level << 2*group#);
//
//
//  dxPrint routines
//  ----------------
//      We have two functions to output text which can be revectored: one
//  formatted the other un-formatted.  All Remark() output goes through
//  "dxPrinter()".  The default output device is "OutputDebugString".
//
//      dxPrint( char *format, ... );   // formatted
//      dxPrinter( char *text );        // un-formatted (faster)
//
//      dxPrintHook( newPrinter );      // revector output to "newPrinter"
//
//
//-----------------------------------------------------------------------------
#ifdef REMARK_ENABLE
	//-----------------------------------------------------------------------------
	//  Global Variables  (expose all for debugging)
	//-----------------------------------------------------------------------------
	extern uint   g_RemarkEnable;				// global Remark enable


	//-----------------------------------------------------------------------------
	//  Function Prototypes
	//-----------------------------------------------------------------------------
	inline void SetRemarkSystemEnabled(bool bEnabled) {g_RemarkEnable=bEnabled;}

	uint   GetRemarkGroupCutoffLevel( int group );
	void   SetRemarkGroupCutoffLevel( int group, int level );
	void   SetRemarkLevelAll( int level );
	void   SetRemarkLevels( const char * pOption);
	void   SetRemarkLogger( const char* cmdLine );
	const char * GetRemarkLogFile();
	void   SetRemarkLogFile( const char* fileName );
	void   SaveRemarkLogFile( const char* savedFileName );
	int GetRemarkGroup(const char *groupName);
	bool _Remark( char* prefix, int level, const char* format, va_list argptr );
	bool _Remark( char* prefix, int level, const wchar_t* format, va_list argptr );


	const char* AddRemarkGroup( const char* group );    // internal only -- this is NOT threadsafe!
	#define VA_REMARK( group, level, format )	\
		bool bResult;	\
		va_list vaArgList;	\
		va_start( vaArgList, format );	\
		bResult = _Remark( group, level, format, vaArgList );	\
		va_end( vaArgList );	\
		return ( bResult );

	#define RemarkAssert(expr, format) \
		FAssertMsg(expr, format); \
		RemarkIf(!(expr), 0, format);

	#define RemarkAssert1(expr, format, p0) \
		FAssertMsg1(expr, format, p0); \
		RemarkIf(!(expr), 0, format, p0);

	#define RemarkAssert2(expr, format, p0, p1) \
		FAssertMsg2(expr, format, p0, p1); \
		RemarkIf(!(expr), 0, format, p0, p1);

	#define RemarkAssert3(expr, format, p0, p1, p2) \
		FAssertMsg3(expr, format, p0, p1, p2); \
		RemarkIf(!(expr), 0, format, p0, p1, p2);

	#define REMARK_GROUP( name )							\
		static char* group =  name;							\
		static const char* trick = AddRemarkGroup( group );	\
		static bool Remark( int level, const wchar_t* format, ...) \
		{													\
		if (!g_RemarkEnable)  return false;				\
		VA_REMARK( group, level, format );	\
		}	\
		static bool Remark( int level, const char *format=0, ... )	\
		{													\
			if (!g_RemarkEnable)  return false;				\
			VA_REMARK( group, level, format );	\
		}	\
		static bool RemarkIf( bool bExpr, int level, const char* format=0, ... )	\
		{													\
			if (!g_RemarkEnable)  return false;				\
			if (bExpr)	\
			{	\
				VA_REMARK( group, level, format );	\
			}	\
			return ( false );	\
		} 
	//---------------------------------------------------------------------------------------
	//	RemarkTo()  --  remark TO specified group
	//---------------------------------------------------------------------------------------
	inline bool RemarkTo( char* group, int level, const char *format=0, ... )
	{
		VA_REMARK( group, level, format );
	}

	inline bool RemarkTo( char* group, int level, const wchar_t *format=0, ... )
	{
		VA_REMARK( group, level, format );
	}


	//---------------------------------------------------------------------------------------
	//	RemarkX()  --  force the remark.  For debugging, just add the 'X' to force always
	//---------------------------------------------------------------------------------------
	inline bool RemarkX( int level, const char *format=0, ... )
	{
		VA_REMARK( MAIN, 0, format );
	}
#else
	#define VA_REMARK( group, level, format )
	#define RemarkAssert(expr, format) 
	#define RemarkAssert1(expr, format, p0) 
	#define RemarkAssert2(expr, format, p0, p1) 
	#define RemarkAssert3(expr, format, p0, p1, p2)
	#define REMARK_GROUP( name )\
		inline bool Remark( int /*level*/, const wchar_t* /*format=0*/, ... ){return false;}\
		inline bool Remark( int /*level*/, const char */*format=0*/, ... ){return false;}\
		inline bool RemarkIf( bool /*bExpr*/, int /*level*/, const char* /*format=0*/, ... ){return false;}

	inline void   SetRemarkGroupCutoffLevel( int group, int level ){}
	inline void   SetRemarkLevelAll( int level ){}
	inline void   SetRemarkLevels( const char * pOption){}
	inline void   SetRemarkLogger( const char* cmdLine ){}
	inline void   SetRemarkLogFile( const char* fileName ){}
	inline void   SaveRemarkLogFile( const char* savedFileName ){}
	inline void   SetRemarkSystemEnabled(bool bEnabled){}
	
	inline uint GetRemarkGroupCutoffLevel( int group ){return 0;}
	inline int GetRemarkGroup(const char *groupName){return -1;}

	inline bool RemarkTo( char* group, int level, const char *format=0, ... ){return false;}
	inline bool RemarkX( int level, const char *format=0, ... ){return false;}
#endif //REMARK_ENABLE



#endif	//FREMARK_H

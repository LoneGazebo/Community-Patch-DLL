//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FTimer.h
//
//  AUTHOR:  David McKibbin  --  8/9/2002
//
//  PURPOSE: Various & Sundry real-time counters and timers.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#ifndef		FTIMER_H
#define		FTIMER_H
#pragma		once

// Add the standard low resolution Windows time functions to the PS3
#ifdef		_PS3
DWORD GetTickCount();
inline DWORD timeGetTime()
{
	return GetTickCount();
}
#endif	//	_PS3

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:  FTimer
//
//  DESC:   High accuracy timer - returns FLOAT seconds w/sub-microsecond accuracy
//
//  NOTES:	Start() - reset the zero mark
//			Stop()  - record/return the elapsed time
//			Peek()  - return the running time since Start() (even after Stop())
//			float   - recall the elapsed time from Stop()
//
//			static FTimer::Read() - returns DOUBLE seconds w/sub-microsecond accuracy
//								    zero mark is 1st use by program
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct FTimer
{
	FTimer()		 { Start(); }

	float Start()
	{
#ifdef		_PS3
        SYS_TIMEBASE_GET(m_qStart);
#else	//	_PS3
        QueryPerformanceCounter( (LARGE_INTEGER*)&m_qStart );
#endif	//	_PS3
        return (m_fTimer = 0);
	}

	float StartWithOffset(float offset)
	{//positive offset offsets the start time into the past.
		Start();
		Peek(); //peek to make sure that m_fFreq is initialized.
	
		__int64 offset64 = (__int64)(offset / m_fFreq);
		m_qStart -= offset64;
		return (m_fTimer = 0);	
	}

	float Peek() const	 { return  ReadF(  m_qStart ); }
	float Stop()	 { return (m_fTimer = Peek()); }
	operator float() { return  m_fTimer; }

  __int64 m_qStart;		// Hi-Rez count @ Start()
    float m_fTimer;		// elapsed time @ Stop()

    //--
    //  Static interface
    //    
	static double Read( __int64 offset=0 );    // raw read counter (sec)
	static float ReadF( __int64 offset=0 ) { return (float)Read( offset ); }

    static   void Test();		// test Win32 timers

	static __int64 m_qMark;		// time_zero reference: 1st counter value
	static __int64 m_qRate;		// QueryPerformanceFrequency()
	static  double m_fFreq;		// QueryPerformanceFrequency() reciprocal
};

// The following is very x86 specific.
#if			!defined( _XENON ) && !defined( _PS3 ) && !defined(_WIN64)
//---------------------------------------------------------------------------------------
//  tmSTART / tmSTOP
//---------------------------------------------------------------------------------------
#define tmSTART1  uint TICKS; tmSTART
#define tmSTART  __asm rdtsc  __asm mov TICKS,eax
#ifdef		_XBOX
#define tmSTOP   __asm rdtsc  __asm sub eax,TICKS  __asm sub eax,32  __asm mov TICKS,eax
#else	//	_XBOX
#define tmSTOP   __asm rdtsc  __asm sub eax,TICKS  __asm sub eax,80  __asm mov TICKS,eax
#endif	//	_XBOX


__forceinline __int64 GetTicks() { __asm rdtsc }


//---------------------------------------------------------------------------------------
//  P5time  --  return TICKS since last call
//---------------------------------------------------------------------------------------
inline __declspec(naked) uint P5time()
{
	static	int tick=0;

	__asm  mov   ecx,tick
    __asm  rdtsc
	__asm  mov   tick,eax
	__asm  sub   eax,ecx
	__asm  ret
}
#endif	//	!defined( _XENON ) && !defined( _PS3 )

#endif	//	FTIMER_H

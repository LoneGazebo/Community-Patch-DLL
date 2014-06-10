/***
* banned.h - list of Microsoft Security Development Lifecycle banned APIs
*
* Purpose:
*       This include file contains a list of banned API which should not be used in new code and
*       removed from legacy code over time
* History
* 01-Jan-2006 - mikehow - Initial Version
* 22-Apr-2008 - mikehow	- Updated to SDL 4.1, commented out recommendations and added memcpy
*
***/

#ifndef _INC_BANNED
#	define _INC_BANNED
#endif

#ifdef _MSC_VER
// Some of these functions are Windows specific
#	pragma once
#	pragma deprecated (strcpy, strcpyA, strcpyW, wcscpy, _tcscpy, _mbscpy, StrCpy, StrCpyA, StrCpyW, lstrcpy, lstrcpyA, lstrcpyW, _tccpy, _mbccpy)
#	pragma deprecated (strcat, strcatA, strcatW, wcscat, _tcscat, _mbscat, StrCat, StrCatA, StrCatW, lstrcat, lstrcatA, lstrcatW, StrCatBuff, StrCatBuffA, StrCatBuffW, StrCatChainW, _tccat, _mbccat)
#	pragma deprecated (wnsprintf, wnsprintfA, wnsprintfW, sprintfW, sprintfA, wsprintf, wsprintfW, wsprintfA, sprintf, swprintf, _stprintf, _snwprintf, _snprintf, _sntprintf)
#	pragma deprecated (wvsprintf, wvsprintfA, wvsprintfW, vsprintf, _vstprintf, vswprintf)
#	pragma deprecated (_vsnprintf, _vsnwprintf, _vsntprintf, wvnsprintf, wvnsprintfA, wvnsprintfW)
#	pragma deprecated (strncpy, wcsncpy, _tcsncpy, _mbsncpy, _mbsnbcpy, StrCpyN, StrCpyNA, StrCpyNW, StrNCpy, strcpynA, StrNCpyA, StrNCpyW, lstrcpyn, lstrcpynA, lstrcpynW)
#	pragma deprecated (strncat, wcsncat, _tcsncat, _mbsncat, _mbsnbcat, StrCatN, StrCatNA, StrCatNW, StrNCat, StrNCatA, StrNCatW, lstrncat, lstrcatnA, lstrcatnW, lstrcatn)
#	pragma deprecated (strtok, _tcstok, wcstok, _mbstok)
#	pragma deprecated (makepath, _tmakepath,  _makepath, _wmakepath)
#	pragma deprecated (_splitpath, _tsplitpath, _wsplitpath)
#	pragma deprecated (scanf, wscanf, _tscanf, sscanf, swscanf, _stscanf, snscanf, snwscanf, _sntscanf)
//#	pragma deprecated (_itoa, _itow, _i64toa, _i64tow, _ui64toa, _ui64tot, _ui64tow, _ultoa, _ultot, _ultow)
#	pragma deprecated (gets, _getts, _gettws)
#	pragma deprecated (IsBadWritePtr, IsBadHugeWritePtr, IsBadReadPtr, IsBadHugeReadPtr, IsBadCodePtr, IsBadStringPtr)
#	pragma deprecated (CharToOem, CharToOemA, CharToOemW, OemToChar, OemToCharA, OemToCharW, CharToOemBuffA, CharToOemBuffW)
//#	pragma deprecated (alloca, _alloca)
#	pragma deprecated (strlen, wcslen, _mbslen, _mbstrlen, StrLen, lstrlen)
#	pragma deprecated (memcpy, RtlCopyMemory, CopyMemory)
#else
#ifdef __GNUC__
// Some of these functions are Windows specific, so you may want to add *nix specific banned function calls
#	pragma GCC poison strcpy strcpyA strcpyW wcscpy _tcscpy _mbscpy StrCpy StrCpyA StrCpyW lstrcpy lstrcpyA lstrcpyW _tccpy _mbccpy
#	pragma GCC poison strcat strcatA strcatW wcscat _tcscat _mbscat StrCat StrCatA StrCatW lstrcat lstrcatA lstrcatW StrCatBuff StrCatBuffA StrCatBuffW StrCatChainW _tccat _mbccat
#	pragma GCC poison wnsprintf wnsprintfA wnsprintfW sprintfW sprintfA wsprintf wsprintfW wsprintfA sprintf swprintf _stprintf _snwprintf _snprintf _sntprintf
#	pragma GCC poison wvsprintf wvsprintfA wvsprintfW vsprintf _vstprintf vswprintf
#	pragma GCC poison _vsnprintf _vsnwprintf _vsntprintf wvnsprintf wvnsprintfA wvnsprintfW
#	pragma GCC poison strncpy wcsncpy _tcsncpy _mbsncpy _mbsnbcpy StrCpyN StrCpyNA StrCpyNW StrNCpy strcpynA StrNCpyA StrNCpyW lstrcpyn lstrcpynA lstrcpynW
#	pragma GCC poison strncat wcsncat _tcsncat _mbsncat _mbsnbcat StrCatN StrCatNA StrCatNW StrNCat StrNCatA StrNCatW lstrncat lstrcatnA lstrcatnW lstrcatn
#	pragma GCC poison strtok _tcstok wcstok _mbstok
#	pragma GCC poison makepath _tmakepath  _makepath _wmakepath
#	pragma GCC poison _splitpath _tsplitpath _wsplitpath
#	pragma GCC poison scanf wscanf _tscanf sscanf swscanf _stscanf snscanf snwscanf _sntscanf
//#	pragma GCC poison _itoa _itow _i64toa _i64tow _ui64toa _ui64tot _ui64tow _ultoa _ultot _ultow
#	pragma GCC poison gets _getts _gettws
#	pragma GCC poison IsBadWritePtr IsBadHugeWritePtr IsBadReadPtr IsBadHugeReadPtr IsBadCodePtr IsBadStringPtr
#	pragma GCC poison CharToOem CharToOemA CharToOemW OemToChar OemToCharA OemToCharW CharToOemBuffA CharToOemBuffW
//#	pragma GCC poison alloca _alloca
#	pragma GCC poison strlen wcslen _mbslen _mbstrlen StrLen lstrlen
#	pragma GCC poison memcpy RtlCopyMemory CopyMemory
#endif

#endif  /* _INC_BANNED */

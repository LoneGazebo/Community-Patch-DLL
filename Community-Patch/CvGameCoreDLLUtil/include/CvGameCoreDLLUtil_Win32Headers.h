//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvGameCoreDLLUtil.h
//!  \brief     Public header for Civilization's use of the Win32 API.
//!
//!		Basic Win32 headers w/ necessary preprocessor defines.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once 
#ifndef CVGAMECOREDLLUTIL_WIN32HEADERS_H
#define CVGAMECOREDLLUTIL_Win32HEADERS_H

//Defines to reduce the amount included by the standard Windows.h
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS    
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES    
#define NOWINSTYLES      
#define NOSYSMETRICS     
#define NOMENUS          
#define NOICONS          
#define NOKEYSTATES      
#define NOSYSCOMMANDS    
#define NORASTEROPS      
#define NOSHOWWINDOW     
#define OEMRESOURCE      
#define NOATOM           
#define NOCLIPBOARD      
#define NOCOLOR          
#define NOCTLMGR         
#define NODRAWTEXT       
#define NOGDI            
#define NOKERNEL         
#define NOUSER           
#define NOMB             
#define NOMEMMGR         
#define NOMETAFILE       
#define NOMINMAX         
#define NOMSG            
#define NOOPENFILE       
#define NOSCROLL         
#define NOSERVICE        
#define NOSOUND          
#define NOTEXTMETRIC     
#define NOWH             
#define NOWINOFFSETS     
#define NOCOMM           
#define NOKANJI          
#define NOHELP           
#define NOPROFILER       
#define NODEFERWINDOWPOS 
#define NOMCX   

//Now to include Windows.h
#include <Windows.h>

#endif //CVGAMECOREDLLUTIL_Win32HEADERS_H
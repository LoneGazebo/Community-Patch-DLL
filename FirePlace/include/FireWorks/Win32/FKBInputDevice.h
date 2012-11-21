//	$Revision: #2 $		$Author: mbreitkreutz $ 	$DateTime: 2005/06/13 13:35:55 $
//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FKBInputDevice.h
//
//  AUTHOR:  Mustafa Thamer  --  09/10/2003
//
//  PURPOSE: Contains abstract class for sampling keyboard input 
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FKBINPUTDEVICE_H
#define		FKBINPUTDEVICE_H
#pragma		once

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:      FKBInputDevice
//
//  PURPOSE:    Abstract base class for sampling keyboard input.
//
//+++++++++++++++++++++++++

#include "..\FInputDevice.h"

class FKBInputDevice : public FInputDevice
{
public:
	enum Modifiers		// bit flags
	{
		CTRL	= 0x1,
		ALT		= 0x2,
		SHIFT	= 0x4
	};

	// these input events are added to the input map (if appropriate) whenever the input device is sampled.
	// currently only uses keyboard input events.
	enum InputType		
	{
		KB_NONE = INPUT_NONE  ,
		KB_ESCAPE     ,
		KB_0          ,
		KB_1          ,
		KB_2          ,
		KB_3          ,
		KB_4          ,
		KB_5          ,
		KB_6          ,
		KB_7          ,
		KB_8          ,
		KB_9          ,
		KB_MINUS      ,	    /* - on main keyboard */		
		KB_A          ,
		KB_B         ,
		KB_C          ,
		KB_D          ,
		KB_E          ,
		KB_F          ,
		KB_G          ,
		KB_H          ,
		KB_I          ,
		KB_J          ,
		KB_K          ,
		KB_L          ,
		KB_M          ,
		KB_N          ,
		KB_O          ,
		KB_P          ,
		KB_Q          ,
		KB_R          ,
		KB_S          ,
		KB_T          ,
		KB_U          ,
		KB_V          ,
		KB_W          ,
		KB_X          ,
		KB_Y          ,
		KB_Z          ,
		KB_EQUALS     ,
		KB_BACKSPACE  ,
		KB_TAB        ,
		KB_LBRACKET   ,
		KB_RBRACKET   ,
		KB_RETURN     ,		/* Enter on main keyboard */
		KB_LCONTROL   ,
		KB_SEMICOLON  ,
		KB_APOSTROPHE ,
		KB_GRAVE      ,		/* accent grave */
		KB_LSHIFT     ,
		KB_BACKSLASH  ,
		KB_COMMA      ,
		KB_PERIOD     ,
		KB_SLASH      ,
		KB_RSHIFT     ,
		KB_NUMPADSTAR   ,
		KB_LALT      ,	
		KB_SPACE      ,
		KB_CAPSLOCK    ,
		KB_F1         ,
		KB_F2         ,
		KB_F3         ,
		KB_F4         ,
		KB_F5         ,
		KB_F6         ,
		KB_F7         ,
		KB_F8         ,
		KB_F9         ,
		KB_F10        ,
		KB_F11        ,
		KB_F12        ,
		KB_NUMLOCK    ,
		KB_SCROLL     ,
		KB_NUMPAD0    ,
		KB_NUMPAD1    ,
		KB_NUMPAD2    ,
		KB_NUMPAD3    ,
		KB_NUMPAD4    ,
		KB_NUMPAD5    ,
		KB_NUMPAD6    ,
		KB_NUMPAD7    ,
		KB_NUMPAD8    ,
		KB_NUMPAD9    ,
		KB_NUMPADMINUS   ,
		KB_NUMPADPLUS        ,
		KB_NUMPADPERIOD    , 
		KB_NUMPADEQUALS,
		KB_AT,
		KB_UNDERLINE,
		KB_COLON,
		KB_NUMPADENTER,
		KB_RCONTROL   ,
		KB_VOLUMEDOWN ,
		KB_VOLUMEUP   ,
		KB_NUMPADCOMMA,
		KB_NUMPADSLASH     ,
		KB_SYSRQ      ,
		KB_RALT      ,
		KB_PAUSE      ,
		KB_HOME       ,
		KB_UP         ,
		KB_PGUP      ,
		KB_LEFT       ,
		KB_RIGHT      ,
		KB_END        ,
		KB_DOWN       ,
		KB_PGDN       ,
		KB_INSERT     ,
		KB_DELETE		,
		NUM_INPUT_TYPE
		};

	FKBInputDevice() {}
	~FKBInputDevice() {}

	// converters
	static unsigned long GetVKCode(uint);
	static unsigned int GetAsciiCode(uint, bool bShift);
	static InputType GetInputType(unsigned long uiVkCode);

	// immediate mode accessors
	bool IsKeyDown(InputType key) const; 
	bool IsKeyUp(InputType key) const; 
	bool IsAltKeyDown() const { return IsKeyDown(KB_LALT) || IsKeyDown(KB_RALT);	}
	bool IsShiftKeyDown() const { return IsKeyDown(KB_LSHIFT) || IsKeyDown(KB_RSHIFT);	}
	bool IsControlKeyDown() const { return IsKeyDown(KB_LCONTROL) || IsKeyDown(KB_RCONTROL);	}
	bool IsScrollLockDown() const { return IsKeyDown(KB_SCROLL); }
	bool IsNumLockDown() const { return IsKeyDown(KB_NUMLOCK); }
	bool IsCapsLockDown() const { return IsKeyDown(KB_CAPSLOCK); }
};

#endif	//FKBINPUTDEVICE

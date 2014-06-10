//	$Revision: #4 $		$Author: mbreitkreutz $ 	$DateTime: 2005/06/13 13:35:55 $
//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FInputDevice.h
//
//  AUTHOR:  Mustafa Thamer  --  09/10/2003
//		Revision 2 - 6/6/06, Mustafa Thamer
//
//  CLASS:      FInputDevice
//
//  PURPOSE:    Samples input device and sets mapping to indicate what input events
//		happened since the last update call.
//
//		Also provides immediate (stateless) mode functionality for queries like	isKeyDown.
//
//		HotKey mappings can be set which will map input1 to input2.  When input1 happens, input2 
//			will be reported.
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003-2006 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FINPUTDEVICE_H
#define		FINPUTDEVICE_H
#pragma		once

#include "FSTLBlockAllocatorMT.h"

class FInputDevice
{
public:

	enum
	{
		// general input type
		INPUT_NONE		= 0,

		// general state values
		UNINIT		= -1,
		UP,
		DOWN,
		DBL_CLICK
	};


	// holds the value of an input event
	struct InputValue
	{
		int m_iState;		// used for key state, UNINIT/UP/DOWN/DBL_CLICK
		POINT m_point;		// 2D int coords, used for mouse/controller analog
		InputValue(int iState=UNINIT, int px=-1, int py=-1) : m_iState(iState) {m_point.x=px; m_point.y=py;  }
		bool operator==(const InputValue& in) const		{ return in.m_iState==m_iState && in.m_point.x==m_point.x && in.m_point.y==m_point.y; }
	};

	// holds the input type and data
	struct InputEvent
	{
		uint m_uiTimeStamp;
		uint m_uiModifiers;		// CTRL/ALT/SHIFT/ControllerNum (see FKBInputDevice.h), hit bit is reserved to indicate key repeat
		InputValue m_val;		// state and location
		uint m_type;			// input type

		InputEvent() : m_uiTimeStamp(0), m_uiModifiers(0), m_type(INPUT_NONE) {}
		InputEvent(uint uiType, InputValue iv, uint uiTime, uint uiMod=0) : 
			m_uiTimeStamp(uiTime), m_uiModifiers(uiMod), m_val(iv), m_type(uiType)  {}
		bool operator==(const InputEvent& in) const		// ignores time
		{ return (in.m_val==m_val && in.m_type==m_type && in.m_uiModifiers==m_uiModifiers); }
		
		int GetControllerNum() const { return (m_uiModifiers & 0xffff); }

		// check hi bit to indicate that the event was triggered as a key repeat
		bool IsKeyRepeat() const { return (m_uiModifiers & 0x8000000) != 0; }
		void SetKeyRepeat(bool bVal) { if (bVal) m_uiModifiers |= 0x8000000; else m_uiModifiers &= ~0x8000000; }
	};
	typedef std::vector<InputEvent, FSTL_Tagged_Allocator<InputEvent, 16,c_eMPoolTypeContainer> > InputEventsList;	

	FInputDevice();
	virtual ~FInputDevice();

	bool Init(unsigned long flags);		// flags are usually 0, but check with specific devices to be sure
	bool UnInit();									
	virtual bool Update(uint uiCurTime) = 0;			// if enabled, sample input and fill in InputEventList
	
	void SetEnabled(bool bEnabled);		// clears events when disabled
	bool GetEnabled() const { return m_bEnabled; }
	unsigned long GetFlags() const { return m_ulFlags; }		

	// access the map of buffered input events
	const InputEventsList& GetInputEvents() const { return m_InputEvents;	}	// return list of input events

	// immediate mode (polling) accessors
	virtual bool GetInputImmediate(InputValue *pResultValue, uint uiInputType, int iControllerNum=0) const = 0;

	// hot key mappings
	void AddHotKeyMapping(uint inputFromType, uint inputToType);	// Will overwrite any existing mapping for that input.
	bool RemoveHotKeyMapping(uint inputFromType);
	void ClearHotKeyMappings();
	uint TranslateHotKey(uint inputFromType) const;					// Call to use hot key mappings, return inputType
protected:
	typedef std::map<uint /*InputType*/, uint /*InputType*/,
		std::less<uint>, FSTL_Tagged_Allocator<std::pair<uint, uint>, 16,c_eMPoolTypeContainer> > HotKeyMap;	// map of inputs to inputs (internal use only)

	virtual bool IInit() = 0;			// device specific init
	virtual bool IUnInit() = 0;			// device specific uninit

	HotKeyMap m_HotKeyMap;
	InputEventsList m_InputEvents;
	bool m_bInitted;
	bool m_bEnabled;
	uint m_ulFlags;
};

#endif	//FINPUTDEVICE_H

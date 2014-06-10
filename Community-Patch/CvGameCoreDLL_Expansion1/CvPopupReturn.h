/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CVPOPUPRETURN_h
#define CVPOPUPRETURN_h

//	Class:		PopupReturn
//	Purpose:	Return structure for popups
//	Author:		Patrick Dawson
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2002 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

class FDataStream;
#define CvPopup_SetAtGrow(kArray, iIdx, kValue)\
	if((int)kArray.size() <= iIdx) kArray.resize(iIdx+1);\
	kArray[iIdx] = kValue;

class PopupReturn
{

public:

	// default ctor
	PopupReturn() {}

	//	Copy constructor
	PopupReturn(const PopupReturn& popupReturn);

	// Assignment operator
	PopupReturn& operator=(const PopupReturn& source);

	//	Set the selected Radio Button
	void setSelectedRadioButton(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiSelectedRadioButton, iGroup, iValue);
		//m_aiSelectedRadioButton, iGroup, iValue );
	}

	//	Will get the selected radio button
	int getSelectedRadioButton(int iGroup = 0) const
	{
		return m_aiSelectedRadioButton[iGroup];
	}

	//	Will get the radio button size
	int getRadioButtonSize(void) const
	{
		return m_aiSelectedRadioButton.size();
	}

	//	Will set the bitfield for checkboxes
	void setCheckboxBitfield(int iValue, int iGroup = 0)
	{
		//m_aiBitField, iGroup, iValue );
		CvPopup_SetAtGrow(m_aiBitField, iGroup, iValue);
	}

	//	Will get the selected bitfield
	int getCheckboxBitfield(int iGroup = 0) const
	{
		return m_aiBitField[iGroup];
	}

	//	Will get the checkbox size
	int getCheckboxSize(void) const
	{
		return m_aiBitField.size();
	}

	//	Will set the editbox string
	void setEditBoxString(CvString strValue, int iGroup = 0)
	{
		//m_aszEditBoxString, iGroup, szValue.c_str() );
		CvPopup_SetAtGrow(m_aszEditBoxString, iGroup, strValue);
	}

	//	Will get the selected editbox string
	const char* getEditBoxString(int iGroup = 0) const
	{
//		return ((uint)iGroup<m_aszEditBoxString.size()) ? m_aszEditBoxString[iGroup] : NULL;
		if((uint)iGroup<m_aszEditBoxString.size())
			return m_aszEditBoxString[iGroup];
		return NULL;
	}

	//	Will get the editbox size
	int getEditboxSize(void) const
	{
		return m_aszEditBoxString.size();
	}

	//	Sets the spinner widget value
	void setSpinnerWidgetValue(const int iValue, int iGroup = 0)
	{
		//		m_aiSpinnerWidgetValues, iGroup, iValue );
		CvPopup_SetAtGrow(m_aiSpinnerWidgetValues, iGroup, iValue);
	}

	//	Will get the selected spinner widget value
	int getSpinnerWidgetValue(int iGroup = 0) const
	{
		return ((uint)iGroup<m_aiSpinBoxID.size()) ? m_aiSpinBoxID[iGroup] : -1;
	}

	//	Will get the spinner widget size
	int getSpinnerWidsize(void) const
	{
		return m_aiSpinnerWidgetValues.size();
	}

	//	Will set the selected pulldown value
	void setSelectedPulldownValue(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiPulldownID, iGroup, iValue);
	}

	//	Will get the selected pulldown value
	int getSelectedPullDownValue(int iGroup = 0) const
	{
		return m_aiPulldownID[iGroup];
	}

	//	Will get the pulldown size
	int getPulldownSize(void) const
	{
		return m_aiPulldownID.size();
	}

	//	Will set the selected listbox value
	void setSelectedListBoxValue(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiListBoxID, iGroup, iValue);
	}

	//	Will get the selected listbox value
	int getSelectedListBoxValue(int iGroup = 0) const
	{
		return ((uint)iGroup<m_aiListBoxID.size()) ? m_aiListBoxID[iGroup] : -1;
	}

	//	Will get the listbox size
	int getListBoxSize(void) const
	{
		return m_aiListBoxID.size();
	}

	//	Will set the current spinbox value
	void setCurrentSpinBoxValue(int iValue, int iIndex = 0)
	{
		CvPopup_SetAtGrow(m_aiSpinBoxID, iIndex, iValue);
	}

	//	Will get the current spinbox value
	int getCurrentSpinBoxValue(int iIndex = 0) const
	{
		return ((uint)iIndex<m_aiSpinBoxID.size()) ? m_aiSpinBoxID[iIndex] : -1;
	}

	//	Will get the spinbox size
	int getSpinBoxSize(void) const
	{
		return m_aiSpinBoxID.size();
	}

	//	Will set the button clicked
	void setButtonClicked(int iValue, int iGroup = 0)
	{
		CvPopup_SetAtGrow(m_aiButtonID, iGroup, iValue);
	}

	//	Will get the button ID
	int GetButtonClicked(int iGroup = 0) const
	{
		return m_aiButtonID[iGroup];
	}

	//	Will get the button size
	int GetButtonSize(void) const
	{
		return m_aiButtonID.size();
	}

	// read and write object to a stream
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

private:

	std::vector<int> m_aiSelectedRadioButton;		//	Selected Radio Button
	std::vector<int> m_aiBitField;					//	BitField
	std::vector<CvString> m_aszEditBoxString;			//	EditBox String
	std::vector<int> m_aiSpinnerWidgetValues;		//	Spinner Widget Values
	std::vector<int> m_aiPulldownID;					//	Pulldown ID
	std::vector<int> m_aiListBoxID;					//	ListBox ID
	std::vector<int> m_aiButtonID;					//	The button clicked on
	std::vector<int> m_aiSpinBoxID;					//	SpinBox ID
};

#undef CvPopup_SetAtGrow

#endif	// CVPOPUPRETURN_h

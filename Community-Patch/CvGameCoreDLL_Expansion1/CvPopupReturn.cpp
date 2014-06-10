/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvPopupReturn.h"

// Include this after all other headers.
#include "LintFree.h"

#define CvPopup_SetAtGrow(kArray, iIdx, kValue)\
	if((int)kArray.size() <= iIdx) kArray.resize(iIdx+1);\
	kArray[iIdx] = kValue;

PopupReturn::PopupReturn(const PopupReturn& popupReturn)
{
	int iI;

	for(iI = 0; iI < popupReturn.getRadioButtonSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiSelectedRadioButton, iI, popupReturn.getSelectedRadioButton(iI));
	}

	for(iI = 0; iI < popupReturn.getCheckboxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiBitField, iI, popupReturn.getCheckboxBitfield(iI));
	}

	for(iI = 0; iI < popupReturn.getEditboxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aszEditBoxString, iI, popupReturn.getEditBoxString(iI));
	}

	for(iI = 0; iI < popupReturn.getSpinnerWidsize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiSpinnerWidgetValues, iI, popupReturn.getSpinnerWidgetValue(iI));
	}

	for(iI = 0; iI < popupReturn.getPulldownSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiPulldownID, iI, popupReturn.getSelectedPullDownValue(iI));
	}

	for(iI = 0; iI < popupReturn.getListBoxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiListBoxID, iI, popupReturn.getSelectedListBoxValue(iI));
	}

	for(iI = 0; iI < popupReturn.getSpinBoxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiSpinBoxID, iI, popupReturn.getSpinnerWidgetValue(iI));
	}

	for(iI = 0; iI < popupReturn.GetButtonSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiButtonID, iI, popupReturn.GetButtonClicked(iI));
	}
}

// Assignment operator
PopupReturn& PopupReturn::operator=(const PopupReturn& source)
{
	int iI;

	for(iI = 0; iI < source.getRadioButtonSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiSelectedRadioButton, iI, source.getSelectedRadioButton(iI));
	}

	for(iI = 0; iI < source.getCheckboxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiBitField, iI, source.getCheckboxBitfield(iI));
	}

	for(iI = 0; iI < source.getEditboxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aszEditBoxString, iI, source.getEditBoxString(iI));
	}

	for(iI = 0; iI < source.getSpinnerWidsize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiSpinnerWidgetValues, iI, source.getSpinnerWidgetValue(iI));
	}

	for(iI = 0; iI < source.getPulldownSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiPulldownID, iI, source.getSelectedPullDownValue(iI));
	}

	for(iI = 0; iI < source.getListBoxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiListBoxID, iI, source.getSelectedListBoxValue(iI));
	}

	for(iI = 0; iI < source.getSpinBoxSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiSpinBoxID, iI, source.getSpinnerWidgetValue(iI));
	}

	for(iI = 0; iI < source.GetButtonSize(); iI++)
	{
		CvPopup_SetAtGrow(m_aiButtonID, iI, source.GetButtonClicked(iI));
	}

	return (*this);
}

//
// read object from a stream
//
void PopupReturn::read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	int iSize;
	int iValue;
	int i;

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiSelectedRadioButton, i, iValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiBitField, i, iValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		CvString strValue;
		kStream >>  strValue ;
		CvPopup_SetAtGrow(m_aszEditBoxString, i, strValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiSpinnerWidgetValues, i, iValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiPulldownID, i, iValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiListBoxID, i, iValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiSpinBoxID, i, iValue);
	}

	kStream >>  iSize ;
	for(i = 0; i < iSize; i++)
	{
		kStream >>  iValue ;
		CvPopup_SetAtGrow(m_aiButtonID, i, iValue);
	}
}

//
// write object to a stream
//
void PopupReturn::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	unsigned int iI;
	//char szString[1024];

	kStream <<  m_aiSelectedRadioButton.size() ;
	for(iI = 0; iI < m_aiSelectedRadioButton.size(); iI++)
	{
		kStream <<  m_aiSelectedRadioButton[iI] ;
	}

	kStream <<  m_aiBitField.size() ;
	for(iI = 0; iI < m_aiBitField.size(); iI++)
	{
		kStream <<  m_aiBitField[iI] ;
	}

	kStream <<  m_aszEditBoxString.size() ;
	for(iI = 0; iI < m_aszEditBoxString.size(); iI++)
	{
		kStream <<  m_aszEditBoxString[iI] ;
	}

	kStream <<  m_aiSpinnerWidgetValues.size() ;
	for(iI = 0; iI < m_aiSpinnerWidgetValues.size(); iI++)
	{
		kStream <<  m_aiSpinnerWidgetValues[iI] ;
	}

	kStream <<  m_aiPulldownID.size() ;
	for(iI = 0; iI < m_aiPulldownID.size(); iI++)
	{
		kStream <<  m_aiPulldownID[iI] ;
	}

	kStream <<  m_aiListBoxID.size() ;
	for(iI = 0; iI < m_aiListBoxID.size(); iI++)
	{
		kStream <<  m_aiListBoxID[iI] ;
	}

	kStream <<  m_aiSpinBoxID.size() ;
	for(iI = 0; iI < m_aiSpinBoxID.size(); iI++)
	{
		kStream <<  m_aiSpinBoxID[iI] ;
	}

	kStream <<  m_aiButtonID.size() ;
	for(iI = 0; iI < m_aiButtonID.size(); iI++)
	{
		kStream <<  m_aiButtonID[iI] ;
	}
}


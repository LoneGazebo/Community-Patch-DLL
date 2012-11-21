/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

//  FILE:    CvAdvisorCounsel.h
//
//  PURPOSE: Public definition of CvGameTextMgr.

#pragma once
#ifndef CIV5_ADVISOR_COUNSEL_H
#define CIV5_ADVISOR_COUNSEL_H

class CvAdvisorCounsel
{
public:
	CvAdvisorCounsel();
	~CvAdvisorCounsel();

	void Init();
	void Uninit();
	void Reset();

	void BuildCounselList (PlayerTypes ePlayer);
	bool SetCounselEntry (uint uiIndex, AdvisorTypes eAdvisor, const char* strAdvisorCounsel, int iValue);
	void SortCounselList (void);

	struct Counsel
	{
		AdvisorTypes m_eAdvisor;
		char m_strTxtKey[512];
		int m_iValue;
	};

	FFastVector<Counsel, true, c_eCiv5GameplayDLL, 0> m_aCounsel;
};

#endif	//CIV5_ADVISOR_COUNSEL_H

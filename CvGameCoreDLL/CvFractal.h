/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// fractal.h

#ifndef CIV5_FRACTAL_H
#define CIV5_FRACTAL_H

#define FRACTAL_MAX_DIMS 255

class CvRandom;
class CvFractal
{

public:

	enum FracVals
	{
		FRAC_WRAP_X					= (0x0001),
		FRAC_WRAP_Y					= (0x0002),
		FRAC_PERCENT				= (0x0004),  //  Returns raw data as a percent.
		FRAC_POLAR					= (0x0008),  //  Sets polar regions to zero.
		FRAC_CENTER_RIFT			= (0x0010),  //  Draws rift in center of world, too.
		FRAC_INVERT_HEIGHTS			= (0x0020),  //  Draws inverts the heights
		DEFAULT_FRAC_X_EXP			= 7,
		DEFAULT_FRAC_Y_EXP			= 6
	};

	//  Smoothness varies with 2^grain.
	//  Totally random is grain = min (iFracXExp, iFracYExp).
	//  As smooth as possible is grain = 0;

	// iFracXExp should be 8 or less
	// iFracYExp should be one less than iFracXExp for Civ3 worlds

	void fracInit(int iNewXs, int iNewYs, int iGrain, CvRandom& random, int iFlags, CvFractal* pRifts=NULL, int iFracXExp=DEFAULT_FRAC_X_EXP, int iFracYExp=DEFAULT_FRAC_Y_EXP);
	void fracInitHinted(int iNewXs, int iNewYs, int iGrain, CvRandom& random, byte* pbyHints, int iHintsLength, int iFlags, CvFractal* pRifts, int iFracXExp=DEFAULT_FRAC_X_EXP, int iFracYExp=DEFAULT_FRAC_Y_EXP);

	int getHeight(int x, int y);
	int getHeightFromPercent(int iPercent);

	void reset();
	CvFractal();
	virtual ~CvFractal();
	void uninit();

	// this can optionally be used after a fractal has been created to blend a ridge system into it
	// this could be used to make mountain ranges that look something like would be caused by plate tectonics (even though it doesn't do any of that)

	// This method generates a Voronoi diagram http://en.wikipedia.org/wiki/Voronoi_diagram to approximate the ridges that would be formed by plate tectonics.
	// These ridges can be perturbed by the flags passed in.  The Voronoi diagram is then blended with an existing fractal.
	// The various settings in the fractal (for example, grain) can be used to modify the look.  The resulting fractal can be used in any of the
	// ways that the fractal class could.
	//
	// void CvFractal::ridgeBuilder(CvRandom& random, int iNumVoronoiSeeds, int iRidgeFlags, int iBlendRidge, int iBlendFract)
	//
	//	random is the random number generator used to build the Fractal.
	//
	//	iNumVoronoiSeeds is how many sections/plates are used to build the fractal.
	//
	//	iRidgeFlags is used to set the flags that determine the modified distance for the Voronoi ridges.  Currently they are all or nothing, but I am using an int so that I can potentially add finer control later.  These vaguely approximate continental drift and general randomness.
	//
	//	iBlendRidge is the amount of the new Voronoi ridges to blend into the original fractal for the resultant fractal.
	//
	//	iBlendFract is the amount of the original fractal to blend with the new ridges for the resultant fractal.
	//
	//	For example, if iBlendRidge is 1 and iBlendFract is 2 this means blend 2 parts original fractal with one part ridge.  This will add a bit of ridge to the fractal.  If iBlendFract is zero, this will be just the ridges.
	//
	// IMPORTANT: (iBlendRidge + iBlendFract) should be greater than zero (although either can be zero).
	//
	void ridgeBuilder(CvRandom& random, int iNumVoronoiSeeds, int iRidgeFlags, int iBlendRidge, int iBlendFract);

protected:

	int m_iXs;
	int m_iYs;
	int m_iFlags;

	int m_iFracX;
	int m_iFracY;
	int m_iFracXExp;
	int m_iFracYExp;

	int m_iXInc;
	int m_iYInc;

	int m_aaiFrac[FRACTAL_MAX_DIMS+1][FRACTAL_MAX_DIMS+1]; //[FRAC_X + 1][FRAC_Y + 1];
	void fracInitInternal(int iNewXs, int iNewYs, int iGrain, CvRandom& random, byte* pbyHints, int iHintsLength, int iFlags, CvFractal* pRifts, int iFracXExp, int iFracYExp);
	void tectonicAction(CvFractal* pRifts);
	int yieldX(int iBadX);

};

#endif

/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvFractal.h"
#include "CvRandom.h"
#include "CvGameCoreUtils.h"
#include "CvGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"


#define FLOAT_PRECISION		(1000)


// Public Functions...

CvFractal::CvFractal()
{
	m_iFracXExp = -1;
	m_iFracYExp = -1;
	m_iXs = -1;
	m_iYs = -1;
	m_iFlags = 0;
	m_iFracX = -1;
	m_iFracY = -1;
}

CvFractal::~CvFractal()
{
	uninit();
}

void CvFractal::uninit()
{
}

void CvFractal::reset()
{
	uninit();
	m_iFracXExp = -1;
	m_iFracYExp = -1;
	m_iXs = -1;
	m_iYs = -1;
	m_iFlags = 0;
	m_iFracX = -1;
	m_iFracY = -1;
}

void CvFractal::fracInit(int iNewXs, int iNewYs, int iGrain, CvRandom& random, int iFlags, CvFractal* pRifts, int iFracXExp/*=7*/, int iFracYExp/*=6*/)
{
	fracInitInternal(iNewXs, iNewYs, iGrain, random, NULL, -1, iFlags, pRifts, iFracXExp, iFracYExp);
}

// pbyHints should be a 1d array of bytes representing a 2d array
//	with width = 2^(iFracXExp - minExp + iGrain) + (GC.getMap().isWrapX() ? 0 : 1)
//	and height = 2^(iFracYExp - minExp + iGrain) + (GC.getMap().isWrapY() ? 0 : 1)
// where minExp = std::min(iFracXExp, iFracYExp)
// Note above that an extra value is required in a dimension in which the map does not wrap.

void CvFractal::fracInitHinted(int iNewXs, int iNewYs, int iGrain, CvRandom& random, byte* pbyHints, int iHintsLength, int iFlags, CvFractal* pRifts, int iFracXExp/*=7*/, int iFracYExp/*=6*/)
{
	int iFlagsNonPolar = iFlags & (~FRAC_POLAR);
	fracInitInternal(iNewXs, iNewYs, iGrain, random, pbyHints, iHintsLength, iFlagsNonPolar, pRifts, iFracXExp, iFracYExp);
}

void CvFractal::fracInitInternal(int iNewXs, int iNewYs, int iGrain, CvRandom& random, byte* pbyHints, int iHintsLength, int iFlags, CvFractal* pRifts, int iFracXExp, int iFracYExp)
{
	FAssert(iNewXs < FRACTAL_MAX_DIMS);
	FAssert(iNewYs < FRACTAL_MAX_DIMS);

	int iSmooth;
	int iScreen;  // This screens out already marked spots in m_aaiFrac[][];
	int iPass;
	int iSum;
	int iX, iY;
	int iI;

	reset();

	if(iFracXExp < 0)
	{
		iFracXExp = DEFAULT_FRAC_X_EXP;
	}
	if(iFracYExp < 0)
	{
		iFracYExp = DEFAULT_FRAC_Y_EXP;
	}

	m_iFracXExp = iFracXExp;
	m_iFracYExp = iFracYExp;
	m_iFracX = 1 << iFracXExp;
	m_iFracY = 1 << iFracYExp;

	// Init m_aaiFrac to all zeroes:
	for(iX = 0; iX < m_iFracX + 1; iX++)
	{
		for(iY = 0; iY < m_iFracY + 1; iY++)
		{
			m_aaiFrac[iX][iY] = 0;
		}
	}

	m_iXs = iNewXs;
	m_iYs = iNewYs;
	m_iFlags = iFlags;
	m_iXInc = ((m_iFracX * FLOAT_PRECISION) / m_iXs);
	m_iYInc = ((m_iFracY * FLOAT_PRECISION) / m_iYs);

	int iMinExp = std::min(m_iFracXExp, m_iFracYExp);
	iSmooth = range(iMinExp - iGrain, 0, iMinExp);

	int iHintsWidth = (1 << (m_iFracXExp - iSmooth)) + ((m_iFlags & FRAC_WRAP_X) ? 0 : 1);
	int iHintsHeight = (1 << (m_iFracYExp - iSmooth)) + ((m_iFlags & FRAC_WRAP_Y) ? 0 : 1);
	if(pbyHints != NULL)
	{
		CvAssertMsg(iHintsLength == iHintsWidth*iHintsHeight, "pbyHints is the wrong size!")
	}

	for(iPass = iSmooth; iPass >= 0; iPass--)
	{
		iScreen = 0;

		for(iI = 0; iI <= iPass; iI++)
		{
			iScreen |= (1 << iI);
		}

		if(m_iFlags & FRAC_WRAP_Y)
		{
			for(iX = 0; iX < m_iFracX + 1; iX++)
			{
				m_aaiFrac[iX][m_iFracY] = m_aaiFrac[iX][0];
			}
		}
		else if(m_iFlags & FRAC_POLAR)
		{
			for(iX = 0; iX < m_iFracX + 1; iX++)
			{
				m_aaiFrac[iX][   0    ] = 0;
				m_aaiFrac[iX][m_iFracY] = 0;
			}
		}

		if(m_iFlags & FRAC_WRAP_X)
		{
			for(iY = 0; iY < m_iFracY + 1; iY++)
			{
				m_aaiFrac[m_iFracX][iY] = m_aaiFrac[0][iY];
			}
		}
		else if(m_iFlags & FRAC_POLAR)
		{
			for(iY = 0; iY < m_iFracY + 1; iY++)
			{
				m_aaiFrac[   0    ][iY] = 0;
				m_aaiFrac[m_iFracX][iY] = 0;
			}
		}

		if(m_iFlags & FRAC_CENTER_RIFT)
		{
			if(m_iFlags & FRAC_WRAP_Y)
			{
				for(iX = 0; iX < m_iFracX + 1; iX++)
				{
					for(iY = 0; iY < (m_iFracY / 6); iY++)
					{
						m_aaiFrac[iX][        iY         ] /= (abs((m_iFracY / 12) - iY) + 1);
						m_aaiFrac[iX][(m_iFracY / 2) + iY] /= (abs((m_iFracY / 12) - iY) + 1);
					}
				}
			}

			if(m_iFlags & FRAC_WRAP_X)
			{
				for(iY = 0; iY < m_iFracY + 1; iY++)
				{
					for(iX = 0; iX < (m_iFracX / 6); iX++)
					{
						m_aaiFrac[        iX         ][iY] /= (abs((m_iFracX / 12) - iX) + 1);
						m_aaiFrac[(m_iFracX / 2) + iX][iY] /= (abs((m_iFracX / 12) - iX) + 1);
					}
				}
			}
		}

		for(iX = 0; iX < (m_iFracX >> iPass) + ((m_iFlags & FRAC_WRAP_X) ? 0 : 1); iX++)
		{
			for(iY = 0; iY < (m_iFracY >> iPass) + ((m_iFlags & FRAC_WRAP_Y) ? 0 : 1); iY++)
			{
				if((iPass == iSmooth)) // If this is the first, pass, set the initial random spots
				{
					if(pbyHints == NULL)
					{
						m_aaiFrac[iX << iPass][iY << iPass] = random.get(256, "Fractal Gen");
					}
					else
					{
						int iXX = iX % iHintsWidth;  // wrap
						int iYY = iY % iHintsHeight; // wrap
						int iHintsI = iYY*iHintsWidth + iXX;

						DEBUG_VARIABLE(iHintsLength);
						CvAssertMsg(iHintsI < iHintsLength, "iHintsI out of range");
						m_aaiFrac[iX << iPass][iY << iPass] = pbyHints[iHintsI];
					}
				}
				else  // Interpolate
				{
					iSum = 0;
					if((iX << iPass) & iScreen)
					{
						if((iY << iPass) & iScreen)   // (center)
						{
							iSum += m_aaiFrac[(iX-1) << iPass][(iY-1) << iPass];
							iSum += m_aaiFrac[(iX+1) << iPass][(iY-1) << iPass];
							iSum += m_aaiFrac[(iX-1) << iPass][(iY+1) << iPass];
							iSum += m_aaiFrac[(iX+1) << iPass][(iY+1) << iPass];
							iSum >>= 2;
							iSum += random.get(1 << (8 - iSmooth + iPass), "Fractal Gen 2");
							iSum -= 1 << (7 - iSmooth + iPass);
							iSum = range(iSum, 0, 255);
							m_aaiFrac[iX << iPass][iY << iPass] = iSum;
						}
						else  // (horizontal)
						{
							iSum += m_aaiFrac[(iX-1) << iPass][iY << iPass];
							iSum += m_aaiFrac[(iX+1) << iPass][iY << iPass];
							iSum >>= 1;
							iSum += random.get(1 << (8 - iSmooth + iPass), "Fractal Gen 3");
							iSum -= 1 << (7 - iSmooth + iPass);
							iSum = range(iSum, 0, 255);
							m_aaiFrac[iX << iPass][iY << iPass] = iSum;
						}
					}
					else
					{
						if((iY << iPass) & iScreen)   // (vertical)
						{
							iSum += m_aaiFrac[iX << iPass][(iY-1) << iPass];
							iSum += m_aaiFrac[iX << iPass][(iY+1) << iPass];
							iSum >>= 1;
							iSum += random.get(1 << (8 - iSmooth + iPass), "Fractal Gen 4");
							iSum -= 1 << (7 - iSmooth + iPass);
							iSum = range(iSum, 0, 255);
							m_aaiFrac[iX << iPass][iY << iPass] = (BYTE) iSum;
						}
						else
						{
							continue;  // (corner) This was already set in an earlier iPass.
						}
					}
				}
			}
		}
	}

	if(pRifts)
	{
		tectonicAction(pRifts);  //  Assumes FRAC_WRAP_X is on.
	}

	if(m_iFlags & FRAC_INVERT_HEIGHTS)
	{
		for(iX = 0; iX < m_iFracX; iX++)
		{
			for(iY = 0; iY < m_iFracY; iY++)
			{
				m_aaiFrac[iX][iY] = (255 - m_aaiFrac[iX][iY]);
			}
		}
	}
}


int CvFractal::getHeight(int iX, int iY)
{
	int iErrX = 0;
	int iErrY = 0;
	int iSum = 0;
	int iHeight = 0;
	int iLowX = 0;
	int iLowY = 0;

	CvAssertMsg(0 <= iX && iX < m_iXs, "iX out of range");
	if(!(0 <= iX && iX < m_iXs)) return 0;

	CvAssertMsg(0 <= iY && iY < m_iYs, "iY out of range");
	if(!(0 <= iY && iY < m_iYs)) return 0;

	iLowX = ((m_iXInc * iX) / FLOAT_PRECISION);
	if(iLowX > m_iFracX - 1)
	{
		iLowX = m_iFracX - 1;	// clamp so that iLowX+1 doesn't overrun array
	}

	iLowY = ((m_iYInc * iY) / FLOAT_PRECISION);
	if(iLowY > m_iFracY - 1)
	{
		iLowY = m_iFracY - 1;	// clamp so that iLowY+1 doesn't overrun array
	}
	iErrX = ((m_iXInc * iX) - (iLowX * FLOAT_PRECISION));
	iErrY = ((m_iYInc * iY) - (iLowY * FLOAT_PRECISION));

	if(iLowX < 0 || iLowX > FRACTAL_MAX_DIMS) return 0; // array is defined as FRACTAL_MAX_DIMS+1
	if(iLowY < 0 || iLowY > FRACTAL_MAX_DIMS) return 0; // array is defined as FRACTAL_MAX_DIMS+1

	iSum = 0;
	iSum += ((FLOAT_PRECISION - iErrX) * (FLOAT_PRECISION - iErrY) * m_aaiFrac[iLowX    ][iLowY    ]);
	iSum += ((iErrX) * (FLOAT_PRECISION - iErrY) * m_aaiFrac[iLowX + 1][iLowY    ]);
	iSum += ((FLOAT_PRECISION - iErrX) * (iErrY) * m_aaiFrac[iLowX    ][iLowY + 1]);
	iSum += ((iErrX) * (iErrY) * m_aaiFrac[iLowX + 1][iLowY + 1]);

	iSum /= (FLOAT_PRECISION * FLOAT_PRECISION);

	iHeight = range(iSum, 0, 255);

	if(m_iFlags & FRAC_PERCENT)
	{
		return ((iHeight * 100) >> 8);
	}
	else
	{
		return iHeight;
	}
}


int CvFractal::getHeightFromPercent(int iPercent)
{
	int iEstimate;
	int iLowerBound;
	int iUpperBound;
	int iSum;
	int iX, iY;

	iLowerBound = 0;
	iUpperBound = 255;

	iPercent = range(iPercent, 0, 100);
	iEstimate = 255 * iPercent / 100;

	while(iEstimate != iLowerBound)
	{
		iSum = 0;

		for(iX = 0; iX < m_iFracX; iX++)
		{
			for(iY = 0; iY < m_iFracY; iY++)
			{
				if(m_aaiFrac[iX][iY] < iEstimate)
				{
					iSum++;
				}
			}
		}
		if(((100 * iSum) / (m_iFracX * m_iFracY)) > iPercent)
		{
			iUpperBound = iEstimate;
			iEstimate = (iUpperBound + iLowerBound) / 2;
		}
		else
		{
			iLowerBound = iEstimate;
			iEstimate = (iUpperBound + iLowerBound) / 2;
		}
	}

	return iEstimate;
}

// Protected Functions...

void CvFractal::tectonicAction(CvFractal* pRifts)  //  Assumes FRAC_WRAP_X is on.
{
	//int iRift1x = (m_iFracX / 4);
	const int iRift2x = ((m_iFracX / 4) * 3);
	const int iWidth = 16;

	for(int iY = 0; iY < m_iFracY + 1; iY++)
	{
		for(int iX = 0; iX < iWidth; iX++)
		{
			//  Rift along edge of map.
			int iDeep = 0;
			int iRx = yieldX(((((pRifts->m_aaiFrac[iRift2x][iY] - 128) * m_iFracX) / 128) / 8) + iX);
			int iLx = yieldX(((((pRifts->m_aaiFrac[iRift2x][iY] - 128) * m_iFracX) / 128) / 8) - iX);
			m_aaiFrac[iRx][iY] = (((m_aaiFrac[iRx][iY] * iX) + iDeep * (iWidth - iX)) / iWidth);
			m_aaiFrac[iLx][iY] = (((m_aaiFrac[iLx][iY] * iX) + iDeep * (iWidth - iX)) / iWidth);
		}
	}

	for(int iY = 0; iY < m_iFracY + 1; iY++)
	{
		m_aaiFrac[m_iFracX][iY] = m_aaiFrac[0][iY];
	}
}


int CvFractal::yieldX(int iBadX)  //  Assumes FRAC_WRAP_X is on.
{
	if(iBadX < 0)
	{
		return (iBadX + m_iFracX);
	}

	if(iBadX >= m_iFracX)
	{
		return (iBadX - m_iFracX);
	}

	return iBadX;
}


struct VoronoiSeed
{
	int m_iHexspaceX;
	int m_iHexspaceY;
	int m_iWeakness;
	int m_eBiasDirection;
	int m_iDirectionalBiasStrength;
};


void CvFractal::ridgeBuilder(CvRandom& random, int iNumVoronoiSeeds, int iRidgeFlags, int iBlendRidge, int iBlendFract)
{
	// this will use a modified Voronoi system to give the appearance of mountain ranges

	iNumVoronoiSeeds = std::max(iNumVoronoiSeeds,3); // make sure that we have at least 3

	// randomly place the seed points of each region ??? do we want a way for an advanced script to pass this in ???
	FStaticVector<VoronoiSeed,256, true, c_eCiv5GameplayDLL, 0> vVoronoiSeeds;
	vVoronoiSeeds.reserve(iNumVoronoiSeeds);

	for(int iThisVoronoiSeedIndex = 0; iThisVoronoiSeedIndex < iNumVoronoiSeeds; iThisVoronoiSeedIndex++)
	{
		VoronoiSeed thisVoronoiSeed;
		thisVoronoiSeed.m_iHexspaceY = random.get(m_iFracY, "Ridge Gen 1");
		thisVoronoiSeed.m_iHexspaceX =  xToHexspaceX(random.get(m_iFracX, "Ridge Gen 2"), thisVoronoiSeed.m_iHexspaceY);
		thisVoronoiSeed.m_iWeakness = std::max(0,random.get(7, "Ridge Gen 3")-3); // ??? do we want to parameterize this???
		thisVoronoiSeed.m_eBiasDirection = random.get(NUM_DIRECTION_TYPES, "Ridge Gen 4");
		thisVoronoiSeed.m_iDirectionalBiasStrength = std::max(0,random.get(8, "Ridge Gen 5") - 4); // ??? do we want to parameterize this???

		// check to see if we are too close to an existing seed
		bool bNeedToRecheck;
		do
		{
			bNeedToRecheck = false;
			for(int iThatVoronoiSeedIndex = 0; iThatVoronoiSeedIndex < iThisVoronoiSeedIndex; iThatVoronoiSeedIndex++)
			{
				int iDistanceBetweenVoronoiSeeds = hexDistance(thisVoronoiSeed.m_iHexspaceX-vVoronoiSeeds[iThatVoronoiSeedIndex].m_iHexspaceX,thisVoronoiSeed.m_iHexspaceY-vVoronoiSeeds[iThatVoronoiSeedIndex].m_iHexspaceY);

				if(iDistanceBetweenVoronoiSeeds < 7)  // ??? parameterize ???
				{
					thisVoronoiSeed.m_iHexspaceX = random.get(m_iFracX, "Ridge Gen 6");
					thisVoronoiSeed.m_iHexspaceY = random.get(m_iFracY, "Ridge Gen 7");
					bNeedToRecheck = true;
					break;
				}
			}

		}
		while(bNeedToRecheck);

		vVoronoiSeeds.push_back(thisVoronoiSeed);
	}

	//std::vector<int> vDistances;
	//vDistances.reserve(iNumVoronoiSeeds);

	for(int iX = 0; iX < m_iFracX; iX++)
	{
		for(int iY = 0; iY < m_iFracY; iY++)
		{
			// get the hexspace coordinate for this position
			int iThisHexX = xToHexspaceX(iX,iY);
			int iThisHexY = iY; // not really needed except for clarity

			// find the distance to each of the seeds (with modifiers for strength of the seed, directional bias, and random factors)
			//vDistances.clear();
			int iClosestSeed = INT_MAX;
			int iNextClosestSeed = INT_MAX;
			for(int iThisVoronoiSeedIndex= 0; iThisVoronoiSeedIndex < iNumVoronoiSeeds; iThisVoronoiSeedIndex++)
			{
				int iModifiedHexspaceDistance = hexDistance(iThisHexX-vVoronoiSeeds[iThisVoronoiSeedIndex].m_iHexspaceX,iThisHexY-vVoronoiSeeds[iThisVoronoiSeedIndex].m_iHexspaceY);
				if(iRidgeFlags)  // we may decide to add more control later
				{
					iModifiedHexspaceDistance += vVoronoiSeeds[iThisVoronoiSeedIndex].m_iWeakness;
					iModifiedHexspaceDistance += random.get(3, "Ridge Gen 8");
					DirectionTypes eRelativeDirection = estimateDirection(vVoronoiSeeds[iThisVoronoiSeedIndex].m_iHexspaceX-iThisHexX,vVoronoiSeeds[iThisVoronoiSeedIndex].m_iHexspaceY-iThisHexY);
					if(eRelativeDirection == vVoronoiSeeds[iThisVoronoiSeedIndex].m_eBiasDirection)
					{
						iModifiedHexspaceDistance -= vVoronoiSeeds[iThisVoronoiSeedIndex].m_iDirectionalBiasStrength;
					}
					else if(eRelativeDirection == (vVoronoiSeeds[iThisVoronoiSeedIndex].m_eBiasDirection + 3) % NUM_DIRECTION_TYPES)
					{
						iModifiedHexspaceDistance += vVoronoiSeeds[iThisVoronoiSeedIndex].m_iDirectionalBiasStrength;
					}
					iModifiedHexspaceDistance = std::max(1,iModifiedHexspaceDistance);
				}
				//vDistances.push_back(iModifiedHexspaceDistance);
				if(iModifiedHexspaceDistance < iClosestSeed)
				{
					iNextClosestSeed = iClosestSeed;
					iClosestSeed = iModifiedHexspaceDistance;
				}
				else if(iModifiedHexspaceDistance < iNextClosestSeed)
				{
					iNextClosestSeed = iModifiedHexspaceDistance;
				}
			}

			// use the modified distance between the two closest seeds to determine the ridge height
			// ??? are there any other fudge factors I want to add in here???
			//std::sort(vDistances.begin(),vDistances.end());

			int iRidgeHeight = (255 * iClosestSeed) / iNextClosestSeed;

			// blend the new ridge height with the previous fractal height
			m_aaiFrac[iX][iY] = (iRidgeHeight * iBlendRidge + m_aaiFrac[iX][iY] * iBlendFract) / std::max(iBlendRidge + iBlendFract, 1);
		}
	}
}

/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvGame.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGlobals.h"
#include "CvGameQueries.h"

//	---------------------------------------------------------------------------
UnitClassTypes CvGameQueries::GetLeastAdvancedUnitClassNobodyHas(bool bUseRandom /* = false */)
{
	CvWeightedVector<int> UnitClassesVector;
	int iWeight = 0;

	bool bValid = false;

	for (int i = 0; i < GC.getNumUnitClassInfos(); i++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(i);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			const UnitTypes eDefaultUnit = (UnitTypes) pkUnitClassInfo->getDefaultUnitIndex();

			// If the UnitClass doesn't have a Default Unit available to everyone, skip it
			if (eDefaultUnit == NO_UNIT)
			{
				continue;
			}

			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eDefaultUnit);
			if(pkUnitInfo == NULL)
			{
				continue;
			}

			// Only look at Land Units
			if (pkUnitInfo->GetDomainType() != DOMAIN_LAND)
			{
				continue;
			}
			// Only look at Military Units
			if (pkUnitInfo->GetCombat() <= 0)
			{
				continue;
			}

			const TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();

			// Unit has to require a Tech, otherwise it's too easy!
			if (ePrereqTech == NO_TECH)
			{
				continue;
			}

			CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereqTech);
			if(pkTechInfo == NULL)
			{
				continue;
			}

			bValid = true;
			bool bAllPlayersHaveTech = true;

			for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				if (GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
				{
					// If a player already has one of this Unit, throw it out - we want something more advanced
					if (GET_PLAYER((PlayerTypes) iMajorLoop).getUnitClassCount(eUnitClass) > 0)
					{
						bValid = false;
						break;
					}

					// Even if no one has this Unit lying around, if all players already have the Tech which unlocks it, throw it out - too easy
					if (bAllPlayersHaveTech && !GET_TEAM((TeamTypes) GET_PLAYER((PlayerTypes) iMajorLoop).getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
					{
						bAllPlayersHaveTech = false;
					}
				}
			}
			if (!bValid || bAllPlayersHaveTech)
			{
				continue;
			}

			iWeight = pkUnitInfo->GetPower();

			// Add a Tech factor, since we want something in the near future, not too far off
			iWeight += (pkTechInfo->GetResearchCost() / 8);

			// Add a random bit so that the same Unit isn't ALWAYS picked
			if (bUseRandom)
			{
				iWeight += GC.getGame().randRangeInclusive(0, iWeight / 10, CvSeeder::fromRaw(0x19bc549e).mix(i));
			}

			UnitClassesVector.push_back(i, iWeight);
		}
	}

	UnitClassesVector.StableSortItems();

	//int iValue;

	bValid = true;

	for (int i = UnitClassesVector.size() - 1; i >= 0; i--)
	{
		const UnitClassTypes eUnitClass = (UnitClassTypes) UnitClassesVector.GetElement(i);

		// First Unit we find in this sorted Vector is our guy
		if (bValid)
		{
			return eUnitClass;
		}
	}

	return NO_UNITCLASS;
}

//	--------------------------------------------------------------------------------
// For each of n teams, let the closeness score for that team be the average distance of an edge between two players on that team.
// This function calculates the closeness score for each team and returns the sum of those n scores.
// The lower the result, the better "clumped" the players' starting locations are.
//
// Note: for the purposes of this function, player i will be assumed to start in the location of player aiStartingLocs[i]
int CvGameQueries::GetTeamClosenessScore(int** aaiDistances, int* aiStartingLocs)
{
	int iScore = 0;

	for(int iTeam = 0; iTeam < MAX_CIV_TEAMS; iTeam++)
	{
		if(GET_TEAM((TeamTypes)iTeam).isAlive())
		{
			int iTeamTotalDist = 0;
			int iNumEdges = 0;
			for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
			{
				if(GET_PLAYER((PlayerTypes)iPlayer).isAlive())
				{
					if(GET_PLAYER((PlayerTypes)iPlayer).getTeam() == (TeamTypes)iTeam)
					{
						for(int iOtherPlayer = 0; iOtherPlayer < iPlayer; iOtherPlayer++)
						{
							if(GET_PLAYER((PlayerTypes)iOtherPlayer).getTeam() == (TeamTypes)iTeam)
							{
								// Add the edge between these two players that are on the same team
								iNumEdges++;
								int iPlayerStart = aiStartingLocs[iPlayer];
								int iOtherPlayerStart = aiStartingLocs[iOtherPlayer];

								if(iPlayerStart < iOtherPlayerStart)  // Make sure that iPlayerStart > iOtherPlayerStart
								{
									int iTemp = iPlayerStart;
									iPlayerStart = iOtherPlayerStart;
									iOtherPlayerStart = iTemp;
								}
								else if(iPlayerStart == iOtherPlayerStart)
								{
									CvAssertMsg(false, "Two players are (hypothetically) assigned to the same starting location!");
								}
								iTeamTotalDist += aaiDistances[iPlayerStart][iOtherPlayerStart];
							}
						}
					}
				}
			}

			int iTeamScore = 0;
			if(iNumEdges == 0)
			{
				iTeamScore = 0;
			}
			else
			{
				iTeamScore = iTeamTotalDist/iNumEdges; // the avg distance between team edges is the team score
			}

			iScore += iTeamScore;
		}
	}
	return iScore;
}
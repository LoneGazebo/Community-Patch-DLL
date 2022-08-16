/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_TECHAI_H
#define CIV5_TECHAI_H

// Multiplier when reweighting based on cost.  Want something roughly equal to sqrt(cost of most expensive policy)
//   Doesn't need to be exact, just used to make sure weights don't fall too low (since using integer math)
#define TECH_COST_WEIGHTING_MULTIPLIER 100

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvTechAI
//!  \brief		Handles tech research decisions for one player
//
//!  Key Attributes:
//!  - Object created by CvPlayerTechs (so one per player)
//!  - Design similar to one used for policies (that is described in Doc\Tech\GameCoreDLL\PolicyAI.doc)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTechAI
{
public:
	CvTechAI(CvPlayerTechs* currentTechs);
	~CvTechAI(void);

	// Initialization
	void Reset();

	// Serialization routines
	template<typename TechAI, typename Visitor>
	static void Serialize(TechAI& techAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Establish weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent);

	// Choose a player's next tech to research
	TechTypes ChooseNextTech(CvPlayer *pPlayer, bool bFreeTech = false);
	TechTypes RecommendNextTech(CvPlayer *pPlayer, TechTypes eIgnoreTech = NO_TECH);

	int GetWeight(TechTypes eTech);

	float GetTechRatio();

	void LogResearchCompleted(TechTypes eTech);

private:
	// Internal methods
	void PropagateWeights(int iTech, int iWeight, int iPropagationPercent, int iPropagationLevel);

	// Recompute weights taking into account tech cost
	void ReweightByCost(CvPlayer *pPlayer, bool bWantsExpensive = false);

	// Logging functions
	void LogPossibleResearch();
	void LogResearchChoice(TechTypes eTech);
	CvString GetLogFileName(CvString& playerName) const;

	// Private data
	CvPlayerTechs* m_pCurrentTechs;
	CvWeightedVector<int> m_TechAIWeights;
	CvWeightedVector<int> m_ResearchableTechs;
};

FDataStream& operator<<(FDataStream&, const CvTechAI&);
FDataStream& operator>>(FDataStream&, CvTechAI&);

#endif //CIV5_TECHAI_H

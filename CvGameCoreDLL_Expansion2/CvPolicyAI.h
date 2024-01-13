/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_POLICYAI_H
#define CIV5_POLICYAI_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvPolicyAI
//!  \brief		Handles policy AI decisions for one player
//
//!  Key Attributes:
//!  - Object created by CvPlayerPolicies (so one per player)
//!  - Design described in Doc\Tech\GameCoreDLL\PolicyAI.doc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPolicyAI
{
public:
	CvPolicyAI(CvPlayerPolicies* currentPolicies);
	~CvPolicyAI(void);

	// Initialization
	void Reset();

	// Serialization routines
	template<typename PolicyAI, typename Visitor>
	static void Serialize(PolicyAI& policyAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Establish weights for one flavor
	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent);

	// Choose a player's next policy purchase
	int ChooseNextPolicy(CvPlayer* pPlayer);
	bool CanContinuePolicyBranch(PolicyBranchTypes ePolicyBranch);

	// Ideology
	void DoChooseIdeology(CvPlayer* pPlayer);
	void DoConsiderIdeologySwitch(CvPlayer* pPlayer);
	int GetBranchBuildingHappiness(CvPlayer* pPlayer, PolicyBranchTypes eBranch);
	int GetNumHappinessPolicies(CvPlayer* pPlayer, PolicyBranchTypes eBranch);

	int WeighBranch(CvPlayer* pPlayer, PolicyBranchTypes eBranch);
	int WeighPolicy(CvPlayer* pPlayer, PolicyTypes ePolicy);

private:
	// Internal methods
	void WeightPrereqs(const vector<int>& vTempWeights, int iPropagationPercent);
	void PropagateWeights(int iPolicy, int iWeight, int iPropagationPercent, int iPropagationLevel);
	Firaxis::Array<int, NUM_YIELD_TYPES> WeightPolicyAttributes(CvPlayer* pPlayer, PolicyTypes ePolicy);
	bool IsBranchEffectiveInGame(PolicyBranchTypes eBranch);

	// Logging functions
	void LogPolicyAttributeYields(CvPlayer* pPlayer, PolicyTypes ePolicy, Firaxis::Array<int, NUM_YIELD_TYPES> yields);
	void LogPossiblePolicies();
	void LogPolicyChoice(PolicyTypes ePolicy);
	void LogBranchChoice(PolicyBranchTypes eBranch);
	void LogIdeologyChoice(CvString &decisionState, int iWeightFreedom, int iWeightAutocracy, int iWeightOrder);
	CvString GetLogFileName(CvString& playerName) const;

	// Private data
	CvPlayerPolicies* m_pCurrentPolicies;
	CvWeightedVector<int> m_PolicyAIWeights;

	// First NUM_POLICY_BRANCH_TYPE entries are branches; policies start after that
	CvWeightedVector<int> m_AdoptablePolicies;

	// Locally cached GlobalAIDefines
	int m_iPolicyWeightPropagationLevels;
	int m_iPolicyWeightPercentDropNewBranch;
};
FDataStream& operator<<(FDataStream&, const CvPolicyAI&);
FDataStream& operator>>(FDataStream&, CvPolicyAI&);

#endif //CIV5_POLICYAI_H
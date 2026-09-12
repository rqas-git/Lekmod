






#pragma once

#ifndef CIV5_POLICYAI_H
#define CIV5_POLICYAI_H




#define SAFE_ESTIMATE_NUM_POLICIES 64









class CvPolicyAI
{
public:
	CvPolicyAI(CvPlayerPolicies* currentPolicies);
	~CvPolicyAI(void);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent);


#ifdef AUI_WARNING_FIXES
	uint ChooseNextPolicy(CvPlayer* pPlayer);
#else
	int ChooseNextPolicy(CvPlayer* pPlayer);
#endif


	void DoChooseIdeology(CvPlayer* pPlayer);
	void DoConsiderIdeologySwitch(CvPlayer* pPlayer);
	int GetBranchBuildingHappiness(CvPlayer* pPlayer, PolicyBranchTypes eBranch);
	int GetNumHappinessPolicies(CvPlayer* pPlayer, PolicyBranchTypes eBranch);

private:

#ifdef AUI_WARNING_FIXES
	void WeightPrereqs(FFastVector<int, true> paiTempWeights, int iPropagationPercent);
#else
	void WeightPrereqs(int* paiTempWeightst, int iPropagationPercent);
#endif
	void PropagateWeights(int iPolicy, int iWeight, int iPropagationPercent, int iPropagationLevel);
	int WeighBranch(PolicyBranchTypes eBranch);
	bool IsBranchEffectiveInGame(PolicyBranchTypes eBranch);


	void LogPossiblePolicies();
	void LogPolicyChoice(PolicyTypes ePolicy);
	void LogBranchChoice(PolicyBranchTypes eBranch);
	void LogIdeologyChoice(CvString &decisionState, int iWeightFreedom, int iWeightAutocracy, int iWeightOrder);
	CvString GetLogFileName(CvString& playerName) const;


	CvPlayerPolicies* m_pCurrentPolicies;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_POLICIES, true> m_PolicyAIWeights;


#ifdef AUI_WARNING_FIXES
	CvWeightedVector<uint, SAFE_ESTIMATE_NUM_POLICIES, true> m_AdoptablePolicies;
#else
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_POLICIES, true> m_AdoptablePolicies;
#endif


	int m_iPolicyWeightPropagationLevels;
	int m_iPolicyWeightPercentDropNewBranch;
};

#endif

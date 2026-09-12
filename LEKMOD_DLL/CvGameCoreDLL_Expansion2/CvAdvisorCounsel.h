









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

	void BuildCounselList(PlayerTypes ePlayer);
	bool SetCounselEntry(uint uiIndex, AdvisorTypes eAdvisor, const char* strAdvisorCounsel, int iValue);
	void SortCounselList(void);

	struct Counsel
	{
		AdvisorTypes m_eAdvisor;
		char m_strTxtKey[512];
		int m_iValue;
	};

	FFastVector<Counsel, true, c_eCiv5GameplayDLL, 0> m_aCounsel;
};

#endif

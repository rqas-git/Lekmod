











#pragma once
#ifndef CVSTOPWATCH_H
#define CVSTOPWATCH_H

class cvStopWatch
{
public:
	cvStopWatch(const char* Name, const char* LogFile = NULL, uint logFlags = 0, bool bDisable = false, bool bShowNesting = false);
	~cvStopWatch();

	static void InitPerfTest();

	void StartPerfTest();
	void EndPerfTest();
	void SetText(const char* szName) { m_szName = szName; }

	double GetDeltaInSeconds() const;

protected:
	void PerfLog(const char* szName, double dtSeconds);

private:
	static bool ms_bPerfInit;
	static LARGE_INTEGER ms_ticksPerSecond;

	const char* m_szName;
	const char* m_szLogFile;
	double m_dtseconds;
	uint m_logFlags;
	int m_nesting;
	bool m_bStarted;
	bool m_bDisable;
	bool m_bShowNesting;
	LARGE_INTEGER m_oldTimerVal;
};

#if !defined(FINAL_RELEASE)
#define CVSTOPWATCH(x)	cvStopwatch(x)
#define CVSTOPWATCH_STR(x)	std::ostringstream stopwatchstr; stopwatchstr << x; cvStopwatch(stopwatchstr.str())
#else
#define CVSTOPWATCH(x)	((void)0)
#define CVSTOPWATCH_STR(x)	((void)0)
#endif

#endif

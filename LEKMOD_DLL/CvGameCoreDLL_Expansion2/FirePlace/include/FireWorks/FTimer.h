













#ifndef		FTIMER_H
#define		FTIMER_H
#pragma		once


#ifdef		_PS3
DWORD GetTickCount();
inline DWORD timeGetTime()
{
	return GetTickCount();
}
#endif
















struct FTimer
{
	FTimer()		 { Start(); }

	float Start()
	{
#ifdef		_PS3
        SYS_TIMEBASE_GET(m_qStart);
#else
        QueryPerformanceCounter( (LARGE_INTEGER*)&m_qStart );
#endif
        return (m_fTimer = 0);
	}

	float StartWithOffset(float offset)
	{
		Start();
		Peek();
	
		__int64 offset64 = (__int64)(offset / m_fFreq);
		m_qStart -= offset64;
		return (m_fTimer = 0);	
	}

	float Peek() const	 { return  ReadF(  m_qStart ); }
	float Stop()	 { return (m_fTimer = Peek()); }
	operator float() { return  m_fTimer; }

  __int64 m_qStart;
    float m_fTimer;




	static double Read( __int64 offset=0 );
	static float ReadF( __int64 offset=0 ) { return (float)Read( offset ); }

    static   void Test();

	static __int64 m_qMark;
	static __int64 m_qRate;
	static  double m_fFreq;
};


#if			!defined( _XENON ) && !defined( _PS3 ) && !defined(_WIN64)



#define tmSTART1  uint TICKS; tmSTART
#define tmSTART  __asm rdtsc  __asm mov TICKS,eax
#ifdef		_XBOX
#define tmSTOP   __asm rdtsc  __asm sub eax,TICKS  __asm sub eax,32  __asm mov TICKS,eax
#else
#define tmSTOP   __asm rdtsc  __asm sub eax,TICKS  __asm sub eax,80  __asm mov TICKS,eax
#endif


__forceinline __int64 GetTicks() { __asm rdtsc }





inline __declspec(naked) uint P5time()
{
	static	int tick=0;

	__asm  mov   ecx,tick
    __asm  rdtsc
	__asm  mov   tick,eax
	__asm  sub   eax,ecx
	__asm  ret
}
#endif

#endif















#ifndef		FCRITICALSECTION_H
#define		FCRITICALSECTION_H
#pragma		once








class FCriticalSection
{
public:


	FCriticalSection( void );



	~FCriticalSection( void );



	bool Try( void );

	void Enter( void );
	void Leave( void );

	int GetLockCount() const;
	int GetRecursionCount() const;

private:


#ifdef		WIN32
		CRITICAL_SECTION m_kCriticalSection;
#else
#ifdef		_PS3
		sys_lwmutex_t m_kCriticalSection;
#endif
#endif

};








class FScopedCriticalSection
{
public:


	FScopedCriticalSection( FCriticalSection& kCriticalSection );



	~FScopedCriticalSection( void );


private:


	FCriticalSection& m_kCriticalSection;
};

#include	"FCriticalSection.inl"

#endif

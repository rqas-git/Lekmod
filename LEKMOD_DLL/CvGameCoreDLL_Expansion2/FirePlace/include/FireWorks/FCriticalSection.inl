






inline FScopedCriticalSection::FScopedCriticalSection( FCriticalSection& kCriticalSection ): m_kCriticalSection( kCriticalSection )
{
	m_kCriticalSection.Enter();
}




inline FScopedCriticalSection::~FScopedCriticalSection( void )
{
	m_kCriticalSection.Leave();
}

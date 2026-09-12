











#ifndef FAtomic_h
#define FAtomic_h


#if defined(_WINPC) 
	#ifndef _WIN64

	LONGLONG _FAtomicIncr64(LONGLONG volatile* pLLIncr );
	LONGLONG _FAtomicDecr64(LONGLONG volatile* pLLDecr );
	#else
	#define _FAtomicIncr64(x) InterlockedIncrement64(x)
	#define _FAtomicDecr64(x) InterlockedDecrement64(x)
	#endif
#endif

#if defined(_WINPC) 

	#define FAtomicIncr(x)			InterlockedIncrement(&x)
	#define FAtomicIncr64(x)		_FAtomicIncr64(&x)

	#define FAtomicDecr(x)			InterlockedDecrement(&x)
	#define FAtomicDecr64(x)		_FAtomicDecr64(&x)

	#define FAtomicAdd(x, a)		InterlockedExchangeAdd(&x, a)

	#define FAtomicAnd(x, a)		InterlockedAnd(&x, a)

	#define FAtomicOr(x, a)			InterlockedOr(&x, a)

	#define FAtomicCAX(x, a, b)		InterlockedCompareExchange(&x, a, b)
    #define FAtomicCAXPtr(x, a, b)  InterlockedCompareExchangePointer((void**)&x, a, b)

#elif defined(_XENON)

	#define FAtomicIncr(x)			InterlockedIncrement(&x)
	#define FAtomicIncr64(x)		InterlockedIncrement64(&x)

	#define FAtomicDecr(x)			InterlockedDecrement(&x)
	#define FAtomicDecr64(x)		InterlockedDecrement64(&x)

	#define FAtomicAdd(x, a)		InterlockedExchangeAdd(&x, a)

	#define FAtomicAnd(x, a)		InterlockedAnd(&x, a)

	#define FAtomicOr(x, a)			InterlockedOr(&x, a)

	#define FAtomicCAX(x, a, b)		InterlockedCompareExchange(&x, a, b)
    #define FAtomicCAXPtr(x, a, b)  InterlockedCompareExchangePointer((void**)&x, a, b)
#elif defined(_PS3)


	#define FAtomicIncr(x)			cellAtomicIncr32(&x)
	#define FAtomicIncr64(x)		cellAtomicIncr64((std::uint64_t*)&x)

	#define FAtomicDecr(x)			cellAtomicDecr32(&x)
	#define FAtomicDecr64(x)		cellAtomicDecr64((std::uint64_t*)&x)

	#define FAtomicAdd(x, a)		cellAtomicAdd32(&x, a)

	#define FAtomicAnd(x, a)		cellAtomicAnd32(&x, a)

	#define FAtomicOr(x, a)			cellAtomicOr32(&x, a)

	#define FAtomicCAX(x, a, b)		cellAtomicCompareAndSwap32(&x, b, a)
    #define FAtomicCAXPtr(x, a, b)  cellAtomicCompareAndSwap32(&x, b, a)
#endif






#ifndef FXS_IS_DLL
class ReadWriteLock
{
public:

    ReadWriteLock() : m_uiLock(0){};

    enum { eWriteShift = 16 };


    class WriteLock_Infinite
    {
    public:

        WriteLock_Infinite(volatile LONG& uiLockRef) : m_uiLock(uiLockRef)
        {
            while(1)
            {
                volatile uint uiLock = uiLockRef;
                if( (uiLock == 0) && (FAtomicCAX(uiLockRef, (uiLock+(1<<eWriteShift)), uiLock) == uiLock) )
                    return;
            }
        };
        ~WriteLock_Infinite() {m_uiLock=0;}

        volatile LONG& m_uiLock;
    };


    class WriteLock_Single
    {
    public:

        WriteLock_Single(volatile LONG& uiLockRef) : m_uiLock(uiLockRef)
        {
            FAtomicCAX(uiLockRef, (1<<eWriteShift), 0);
        };

        BOOL IsLocked() const { return m_uiLock&(1<<eWriteShift);}

        ~WriteLock_Single() {m_uiLock=0;}

        volatile LONG& m_uiLock;
    };


    class ReadLock_Infinite
    {
    public:
        ReadLock_Infinite(volatile LONG& uiLockRef) : m_uiLock(uiLockRef)
        {
            while(1)
            {
                volatile uint uiLock = uiLockRef;
                if( (uiLock<1<<eWriteShift) && (FAtomicCAX(uiLockRef, (uiLock+1), uiLock) == uiLock) )
                    return;
            }
        };

        ~ReadLock_Infinite() {FAtomicDecr(m_uiLock);}

        volatile LONG& m_uiLock;
    };




    typedef WriteLock_Infinite WRITE_LOCK_INFINITE;
    typedef WriteLock_Single   WRITE_LOCK_SINGLE;
    typedef ReadLock_Infinite  READ_LOCK_INFINITE;


    volatile LONG& Acquire() const { return m_uiLock; }

    __declspec( align(16)) volatile mutable LONG m_uiLock;
};




class AtomicLock
{
public:

    AtomicLock() : m_uiLock(0){};


    class ScopedLock_Infinite
    {
    public:
        ScopedLock_Infinite(volatile LONG& uiLockRef) : m_uiLock(uiLockRef)
        {
            while(FAtomicCAX(uiLockRef, 1, 0) != 0){}
        };

        ~ScopedLock_Infinite() {m_uiLock = 0;}

        volatile LONG& m_uiLock;
    };




    typedef volatile ScopedLock_Infinite SCOPED_LOCK_INFINITE;
    typedef volatile LONG LOCK_MEMBER_TYPE;


    volatile LONG& Acquire() const { return m_uiLock; }

    __declspec( align(16)) volatile mutable LONG m_uiLock;
};
#endif

#endif

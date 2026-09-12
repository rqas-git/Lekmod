











#ifndef FPrefetch_h
#define FPrefetch_h

#ifdef _XENON
#define FPREFETCH( ptr ) __dcbt( 0, ptr );
#define FPREFETCH_2x( ptr ) __dcbt( 0, ptr ); __dcbt( 128, ptr );
#define PREFETCH_SIZE 128

static void PrefetchRegion(const char* pHead, const uint uiSize)
{
    const uint uiLoop = (uiSize+(PREFETCH_SIZE-1))/PREFETCH_SIZE;
    for(uint iCurr = 0; iCurr<uiLoop;++iCurr)
    {
        FPREFETCH(pHead+(iCurr*PREFETCH_SIZE));
    }
}

#elif defined(_PS3)
#define FPREFETCH( ptr ) __dcbt(ptr );
#define FPREFETCH_2x( ptr )  __dcbt( ptr ); __dcbt( ((byte*)ptr) + 128);
#define PREFETCH_SIZE 128

static void PrefetchRegion(const char* pHead, const uint uiSize)
{
    const uint uiLoop = (uiSize+(PREFETCH_SIZE-1))/PREFETCH_SIZE;
    for(uint iCurr = 0; iCurr<uiLoop;++iCurr)
    {
        FPREFETCH(pHead+(iCurr*PREFETCH_SIZE));
    }
}

#elif defined(_WINPC)












#include <xmmintrin.h>
#define FPREFETCH( ptr ) _mm_prefetch((const char*)ptr,  _MM_HINT_T0 );
#define FPREFETCH_2x( ptr ) FPREFETCH(ptr); FPREFETCH(ptr+64                                                   );
#define PREFETCH_SIZE 64

static void PrefetchRegion(const char* pHead, const uint uiSize)
{
    const uint uiLoop = (uiSize+(PREFETCH_SIZE-1))/PREFETCH_SIZE;
    for(uint iCurr = 0; iCurr<uiLoop;++iCurr)
    {
        FPREFETCH(pHead+(iCurr*PREFETCH_SIZE));
    }
}

#endif

#endif

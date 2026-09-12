





#ifndef __FDEF_NEW_H__
#define __FDEF_NEW_H__


#if !defined(USE_MFC)

#ifdef FXS_MEMORY_TRACKER
	#define NI_MEMORY_DEBUGGER
#endif





	#ifdef _PS3

		#define _FREE_BLOCK   0
		#define _NORMAL_BLOCK 1
		#define _CRT_BLOCK    2
		#define _IGNORE_BLOCK 3
		#define _CLIENT_BLOCK 4
		#define _MAX_BLOCKS   5

	#endif





#if defined(FXS_MEMORY_TRACKER) || defined(FXS_MEMORY_MANAGER)

	#define FNEW( type, mpool, tag ) new(_NORMAL_BLOCK, __FILE__, __LINE__, mpool, tag) type
	#define FMALLOC( size, mpool, tag ) FireMalloc( size, __FILE__, __LINE__, mpool, tag )
	#define FMALLOC_EXP( size, file, line, pool, tag ) FireMalloc( size, file, line, pool, tag )
	#define FREALLOC( ptr, size, mpool, tag ) FireRealloc(ptr, size, __FILE__, __LINE__, mpool, tag)
	#define FREALLOC_EXP( ptr, size, file, line, pool, tag ) FireRealloc( ptr, size, file, line, pool, tag)
	#define FMALLOCALIGNED( size, align, mpool, tag ) FireMallocAligned(size, align, __FILE__, __LINE__, mpool, tag)
	#define FMALLOCALIGNED_EXP( size, align, file, line, pool, tag ) FireMallocAligned( size, align, file, line, pool, tag )
	#define FFREE( ptr ) FireFree(ptr)
	#define FFREEALIGNED( ptr ) FireFreeAligned(ptr)
	#define FMEMSIZE( ptr ) FireMemSize(ptr)
#else
	#define	FNEW( type, mpool, tag ) new type
	#define FMALLOC( size, mpool, tag ) malloc( size )
	#define FMALLOC_EXP( size, file, line, mpool, tag ) malloc( size )
	#define FREALLOC( ptr, size, mpool, tag ) realloc(ptr, size)
	#define FREALLOC_EXP( ptr, size, file, line, pool, tag ) realloc(ptr, size)
	#define FMALLOCALIGNED( size, align, mpool, tag ) FireMallocAlignedNoTracking(size, align, __FILE__, __LINE__)
	#define FMALLOCALIGNED_EXP( size, align, file, line, mpool, tag ) FireMallocAlignedNoTracking(size, align, file, line )
	#define FFREE( ptr ) free(ptr)
	#define FFREEALIGNED( ptr ) FireFreeAlignedNoTracking(ptr)
	#define FMEMSIZE( ptr ) _msize(ptr)
#endif



#endif

#endif

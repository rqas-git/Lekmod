








#if !defined(USE_MFC)
#if defined(FXS_MEMORY_MANAGER_ENABLED) || defined(MS_MEMORY_TRACKER)





	#ifdef malloc
	#undef malloc
	#endif

	#ifdef calloc
	#undef calloc
	#endif

	#ifdef realloc
	#undef realloc
	#endif

	#ifdef memalign
	#undef memalign
	#endif

	#ifdef _aligned_malloc
	#undef _aligned_malloc
	#endif

	#ifdef free
	#undef free
	#endif

	#ifdef _aligned_free
	#undef _aligned_free
	#endif

	#ifdef _msize
	#undef _msize
	#endif





	#ifdef new
	#undef new
	#endif

	#ifdef delete
	#undef delete
	#endif

#endif
#endif

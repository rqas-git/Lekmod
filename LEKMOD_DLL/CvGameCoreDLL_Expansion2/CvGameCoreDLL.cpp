






#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "ICvDLLUserInterface.h"
#ifndef LEKMOD_MACOS
#include "Win32/FDebugHelper.h"
#endif
#include "CvDllContext.h"
#ifdef LEKMOD_MACOS
#include "abi_checks.hpp"
#endif


#include "LintFree.h"


#if defined(LEKMOD_MACOS)
extern "C"
__attribute__((visibility("default")))
ICvGameContext1* DllGetGameContext()
#else
extern "C" ICvGameContext1* DllGetGameContext()
#endif
{
	return CvDllGameContext::GetSingleton();
}

#if defined(LEKMOD_MACOS)
__attribute__((constructor)) static void LekmodInitialize()
{
    fprintf(stderr, "[Lekmod macOS] Initializing native v35.3 game core\n");
    timeBeginPeriod(1);
    CvDllGameContext::InitializeSingleton();
    fprintf(stderr, "[Lekmod macOS] Game context initialized\n");
}
__attribute__((destructor)) static void LekmodShutdown()
{
    timeEndPeriod(1);
    CvDllGameContext::DestroySingleton();
    GC.setDLLIFace(NULL);
}
#else
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{

		OutputDebugString("DLL_PROCESS_ATTACH\n");
		FDebugHelper::GetInstance().LoadSymbols((HMODULE)hModule);

#ifdef CVASSERT_ENABLE
		MMRESULT iTimeSet = timeBeginPeriod(1);
		DEBUG_VARIABLE(iTimeSet);
		CvAssertMsg(iTimeSet==TIMERR_NOERROR, "failed setting timer resolution to 1 ms");
#else
		timeBeginPeriod(1);
#endif
		CvDllGameContext::InitializeSingleton();
	}
	break;
	case DLL_THREAD_ATTACH:
		OutputDebugString("DLL_THREAD_ATTACH\n");
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString("DLL_THREAD_DETACH\n");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString("DLL_PROCESS_DETACH\n");
		timeEndPeriod(1);
		CvDllGameContext::DestroySingleton();
		GC.setDLLIFace(NULL);
		break;
	}

	return TRUE;
}
#endif

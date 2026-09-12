#ifndef FDEBUGHELPER_H
#define FDEBUGHELPER_H

#include <dbghelp.h>




#define USE_RTL_CAPTURE_STACK


#ifdef USE_RTL_CAPTURE_STACK

typedef USHORT (WINAPI *CaptureStackBackTraceFuncType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
#endif






#ifdef _M_IX86
	#define FILL_CONTEXT( context ) \
		{ \
		__asm{ mov context.Esp, esp } \
		__asm{ mov context.Ebp, ebp } \
		__asm{ xor eax, eax } \
		__asm{ call jumpl } \
	jumpl: \
		__asm{ mov eax, [esp] } \
		__asm{ mov context.Eip, eax } \
		__asm{ pop eax } \
		}
#elif defined(_M_X64)

	#define FILL_CONTEXT( context ) RtlCaptureContext( &context )
#else
	#define FILL_CONTEXT( context ) 
#endif











class FCallStack;
class FMiniDumper;

class FDebugHelper
{
public:
	static FDebugHelper& GetInstance();
	static FDebugHelper* GetCurrentInstance() { return pInstance; };


	bool LoadSymbols();

	bool LoadSymbols(HMODULE hModule);
	void UnloadSymbols(HMODULE hModule);

	bool GetSymbolName( DWORD64 dwAddress, TCHAR* name, unsigned int maxNameLength );
	bool GetSymbolName( const CONTEXT& context, TCHAR* name, unsigned int maxNameLength );

	bool GetSymbolFileLine( DWORD64 dwAddress, TCHAR* name, DWORD* pLineNumber, unsigned int maxNameLength );
	bool GetSymbolFileLine( const CONTEXT& context, TCHAR* name, DWORD* pLineNumber, unsigned int maxNameLength );

	bool GetModuleName( DWORD64 dwAddress, TCHAR* name, unsigned int maxNameLength );







	bool GetCallStack(FCallStack *pCallStack, int iStrip = 0, int iDepth = 0);

	template< class Function > void WalkStack( const CONTEXT& context, Function Func );


	static void DestroyDebugHelper();

	typedef void (*ExceptCallback)(TCHAR* szExceptionText);
	void SetExceptionTextCB(ExceptCallback pCB) { m_pExceptionTextCB = pCB; }
	const ExceptCallback GetExceptionTextCB() const { return m_pExceptionTextCB; } 


#if !defined( FINAL_RELEASE )
	static FMiniDumper*	GetMiniDumper();
#endif


	static void WaitForDebuggerAttachment(bool bShowWindow = true);

private:

	FDebugHelper();
	~FDebugHelper();
	FDebugHelper( const FDebugHelper& );
	const FDebugHelper& operator=( const FDebugHelper& );

	static FDebugHelper* pInstance;

	HANDLE hCurrentProcess;
	ExceptCallback m_pExceptionTextCB;

#ifdef USE_RTL_CAPTURE_STACK
	CaptureStackBackTraceFuncType m_pkCaptureStackBackTrace;
#endif

};

#include "FDebugHelper.inl"

#endif

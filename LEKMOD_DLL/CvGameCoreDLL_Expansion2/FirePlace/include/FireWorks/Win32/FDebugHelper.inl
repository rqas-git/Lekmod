template< class Function > void FDebugHelper::WalkStack( const CONTEXT& context, Function Func )
{
	DWORD dwMachineType = 0;

	STACKFRAME64 sf;
	memset( &sf, 0, sizeof(sf) );

#ifdef _M_IX86


	sf.AddrPC.Offset       = context.Eip;
	sf.AddrPC.Mode         = AddrModeFlat;
	sf.AddrStack.Offset    = context.Esp;
	sf.AddrStack.Mode      = AddrModeFlat;
	sf.AddrFrame.Offset    = context.Ebp;
	sf.AddrFrame.Mode      = AddrModeFlat;

	dwMachineType = IMAGE_FILE_MACHINE_I386;
#elif defined(_WIN64)
	sf.AddrPC.Offset       = context.Rip;
	sf.AddrPC.Mode         = AddrModeFlat;
	sf.AddrStack.Offset    = context.Rsp;
	sf.AddrStack.Mode      = AddrModeFlat;
	sf.AddrFrame.Offset    = context.Rbp;
	sf.AddrFrame.Mode      = AddrModeFlat;

	dwMachineType = IMAGE_FILE_MACHINE_AMD64;
#endif

	while( StackWalk64( dwMachineType,
		hCurrentProcess,
		GetCurrentThread(),
		&sf,
		0,
		0,
		SymFunctionTableAccess64,
		SymGetModuleBase64,
		0) )
	{
		if( !Func( sf ) )
			return;
	}
}














#ifndef		FREMARK_H
#define		FREMARK_H
#pragma		once

#include "FAssert.h"




#if !defined(FINAL_RELEASE) && !defined(PROFILE)
	#define		REMARK_ENABLE
#endif	

void   dprintf( const char* format, ... );
void   dxPrint( const char* format, ... );
void   dxPrintNL( const char* format, ... );
typedef void(*dxPRINTPROC)( char* txt, int level );
extern  void(*dxPrinter)( char* txt, int level );
dxPRINTPROC dxPrintHook( dxPRINTPROC newPrinter=0 );

#define MAIN "MAIN"












































































































































































#ifdef REMARK_ENABLE



	extern uint   g_RemarkEnable;





	inline void SetRemarkSystemEnabled(bool bEnabled) {g_RemarkEnable=bEnabled;}

	uint   GetRemarkGroupCutoffLevel( int group );
	void   SetRemarkGroupCutoffLevel( int group, int level );
	void   SetRemarkLevelAll( int level );
	void   SetRemarkLevels( const char * pOption);
	void   SetRemarkLogger( const char* cmdLine );
	const char * GetRemarkLogFile();
	void   SetRemarkLogFile( const char* fileName );
	void   SaveRemarkLogFile( const char* savedFileName );
	int GetRemarkGroup(const char *groupName);
	bool _Remark( char* prefix, int level, const char* format, va_list argptr );
	bool _Remark( char* prefix, int level, const wchar_t* format, va_list argptr );


	const char* AddRemarkGroup( const char* group );
	#define VA_REMARK( group, level, format )	\
		bool bResult;	\
		va_list vaArgList;	\
		va_start( vaArgList, format );	\
		bResult = _Remark( group, level, format, vaArgList );	\
		va_end( vaArgList );	\
		return ( bResult );

	#define RemarkAssert(expr, format) \
		FAssertMsg(expr, format); \
		RemarkIf(!(expr), 0, format);

	#define RemarkAssert1(expr, format, p0) \
		FAssertMsg1(expr, format, p0); \
		RemarkIf(!(expr), 0, format, p0);

	#define RemarkAssert2(expr, format, p0, p1) \
		FAssertMsg2(expr, format, p0, p1); \
		RemarkIf(!(expr), 0, format, p0, p1);

	#define RemarkAssert3(expr, format, p0, p1, p2) \
		FAssertMsg3(expr, format, p0, p1, p2); \
		RemarkIf(!(expr), 0, format, p0, p1, p2);

	#define REMARK_GROUP( name )							\
		static char* group =  name;							\
		static const char* trick = AddRemarkGroup( group );	\
		static bool Remark( int level, const wchar_t* format, ...) \
		{													\
		if (!g_RemarkEnable)  return false;				\
		VA_REMARK( group, level, format );	\
		}	\
		static bool Remark( int level, const char *format=0, ... )	\
		{													\
			if (!g_RemarkEnable)  return false;				\
			VA_REMARK( group, level, format );	\
		}	\
		static bool RemarkIf( bool bExpr, int level, const char* format=0, ... )	\
		{													\
			if (!g_RemarkEnable)  return false;				\
			if (bExpr)	\
			{	\
				VA_REMARK( group, level, format );	\
			}	\
			return ( false );	\
		} 



	inline bool RemarkTo( char* group, int level, const char *format=0, ... )
	{
		VA_REMARK( group, level, format );
	}

	inline bool RemarkTo( char* group, int level, const wchar_t *format=0, ... )
	{
		VA_REMARK( group, level, format );
	}





	inline bool RemarkX( int level, const char *format=0, ... )
	{
		VA_REMARK( MAIN, 0, format );
	}
#else
	#define VA_REMARK( group, level, format )
	#define RemarkAssert(expr, format) 
	#define RemarkAssert1(expr, format, p0) 
	#define RemarkAssert2(expr, format, p0, p1) 
	#define RemarkAssert3(expr, format, p0, p1, p2)
	#define REMARK_GROUP( name )\
		inline bool Remark( int          , const wchar_t*             , ... ){return false;}\
		inline bool Remark( int          , const char *            , ... ){return false;}\
		inline bool RemarkIf( bool          , int          , const char*             , ... ){return false;}

#ifdef AUI_WARNING_FIXES
	inline void   SetRemarkGroupCutoffLevel(int, int){}
	inline void   SetRemarkLevelAll(int){}
	inline void   SetRemarkLevels(const char*){}
	inline void   SetRemarkLogger(const char*){}
	inline void   SetRemarkLogFile(const char*){}
	inline void   SaveRemarkLogFile(const char*){}
	inline void   SetRemarkSystemEnabled(bool){}

	inline uint GetRemarkGroupCutoffLevel(int){return 0;}
	inline int GetRemarkGroup(const char*){return -1;}

	inline bool RemarkTo(char*, int, const char*, ... ){return false;}
	inline bool RemarkX(int, const char*, ... ){return false;}
#else
	inline void   SetRemarkGroupCutoffLevel( int group, int level ){}
	inline void   SetRemarkLevelAll( int level ){}
	inline void   SetRemarkLevels( const char * pOption){}
	inline void   SetRemarkLogger( const char* cmdLine ){}
	inline void   SetRemarkLogFile( const char* fileName ){}
	inline void   SaveRemarkLogFile( const char* savedFileName ){}
	inline void   SetRemarkSystemEnabled(bool bEnabled){}
	
	inline uint GetRemarkGroupCutoffLevel( int group ){return 0;}
	inline int GetRemarkGroup(const char *groupName){return -1;}

	inline bool RemarkTo( char* group, int level, const char *format=0, ... ){return false;}
	inline bool RemarkX( int level, const char *format=0, ... ){return false;}
#endif
#endif



#endif

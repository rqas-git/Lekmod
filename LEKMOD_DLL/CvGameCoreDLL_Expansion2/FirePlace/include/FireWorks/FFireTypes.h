













#ifndef		FFIRETYPES_H
#define		FFIRETYPES_H
#pragma		once

typedef unsigned char		byte;
typedef unsigned short		word;
typedef unsigned int		uint;
#ifdef LEKMOD_MACOS

typedef unsigned int dword;
#else
typedef unsigned long		dword;
#endif
#if		defined(_PS3)


typedef int64_t				__int64;
#elif	defined(_NDS)
typedef u64					qword;
typedef s64					__int64;
#else
typedef unsigned __int64	qword;
#endif
typedef wchar_t         	wchar;

#ifdef		_PS3

typedef char CHAR;
typedef wchar_t WCHAR;

typedef unsigned char UCHAR;

typedef short SHORT;
typedef long  LONG;

typedef unsigned short USHORT;
typedef unsigned long  ULONG;

typedef unsigned short u_short;
typedef unsigned long  u_long;

typedef byte  BYTE;
typedef word  WORD;
typedef uint  UINT;
typedef dword DWORD;

typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef ULONG_PTR SIZE_T;

typedef int8_t    INT8,   *PINT8;
typedef int16_t   INT16,  *PINT16;
typedef int32_t   INT32,  *PINT32;
typedef int64_t   INT64,  *PINT64;
typedef uint8_t   UINT8,  *PUINT8;
typedef uint16_t  UINT16, *PUINT16;
typedef uint32_t  UINT32, *PUINT32;
typedef uint64_t  UINT64, *PUINT64;

typedef int BOOL;

#ifndef		FALSE
#define		FALSE	0
#endif

#ifndef		TRUE
#define		TRUE	1
#endif

typedef struct _POINT
{
    long x;
    long y;
}   POINT, *PPOINT, *LPPOINT;

typedef struct tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
}   RECT, *PRECT, *LPRECT;




typedef int HWND;


typedef UINT * WPARAM;
typedef LONG * LPARAM;
typedef LONG * LRESULT;


#endif

#ifdef		_NDS
typedef char CHAR;
typedef wchar_t WCHAR;

typedef unsigned char UCHAR;

typedef short SHORT;
typedef long  LONG;

typedef unsigned short USHORT;
typedef unsigned long  ULONG;

typedef unsigned short u_short;
typedef unsigned long  u_long;

typedef byte  BYTE;
typedef word  WORD;
typedef uint  UINT;
typedef dword DWORD;

typedef s8    INT8,   *PINT8;
typedef s16   INT16,  *PINT16;
typedef s32   INT32,  *PINT32;
typedef s64   INT64,  *PINT64;
typedef u8   UINT8,  *PUINT8;
typedef u16  UINT16, *PUINT16;
typedef u32  UINT32, *PUINT32;
typedef u64  UINT64, *PUINT64;

typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef struct _POINT
{
    long x;
    long y;
}   POINT, *PPOINT, *LPPOINT;

typedef struct tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
}   RECT, *PRECT, *LPRECT;


#define LOWORD(l) ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l) ((WORD)((DWORD)(l) >> 16))




	typedef int HWND;


	typedef UINT * WPARAM;
	typedef LONG * LPARAM;
	typedef LONG * LRESULT;



#endif

typedef UINT64	uint64;
typedef UINT32	uint32;
typedef UINT16	uint16;
typedef UINT8	uint8;
typedef INT32	int32;
typedef INT64	int64;
typedef INT16	int16;
typedef INT8	int8;
 

#ifdef		_PS3
typedef pthread_t	threadid_t;
#else
typedef DWORD		threadid_t;
#endif

typedef unsigned short port_t;

#ifdef		_PS3
typedef unsigned int socklen_t;
#else
typedef int socklen_t;
#endif



#include <functional>
template<class _Ty>
struct ptr_less : std::binary_function<_Ty, _Ty, bool>
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{
		return (*_X < *_Y);
	}
};

























#define  QNAN	0x7FC00000
#define  SNAN	0x7F800001


#define  BIT(n)		(1<<(n))

#define  BIT0		BIT( 0)
#define  BIT1		BIT( 1)
#define  BIT2		BIT( 2)
#define  BIT3		BIT( 3)
#define  BIT4		BIT( 4)
#define  BIT5		BIT( 5)
#define  BIT6		BIT( 6)
#define  BIT7		BIT( 7)
#define  BIT8		BIT( 8)
#define  BIT9		BIT( 9)
#define  BIT10		BIT(10)
#define  BIT11		BIT(11)
#define  BIT12		BIT(12)
#define  BIT13		BIT(13)
#define  BIT14		BIT(14)
#define  BIT15		BIT(15)
#define  BIT16		BIT(16)
#define  BIT17		BIT(17)
#define  BIT18		BIT(18)
#define  BIT19		BIT(19)
#define  BIT20		BIT(20)
#define  BIT21		BIT(21)
#define  BIT22		BIT(22)
#define  BIT23		BIT(23)
#define  BIT24		BIT(24)
#define  BIT25		BIT(25)
#define  BIT26		BIT(26)
#define  BIT27		BIT(27)
#define  BIT28		BIT(28)
#define  BIT29		BIT(29)
#define  BIT30		BIT(30)
#define  BIT31		BIT(31)


#define MAX_CHAR                            (0x7f)
#define MIN_CHAR                            (0x80)
#define MAX_SHORT                           (0x7fff)
#define MIN_SHORT                           (0x8000)
#define MAX_INT                             (0x7fffffff)
#define MIN_INT                             (0x80000000)
#define MAX_UNSIGNED_CHAR                   (0xff)
#define MIN_UNSIGNED_CHAR                   (0x00)
#define MAX_UNSIGNED_SHORT                  (0xffff)
#define MIN_UNSIGNED_SHORT                  (0x0000)
#define MAX_UNSIGNED_INT                    (0xffffffff)
#define MIN_UNSIGNED_INT                    (0x00000000)

#ifndef FPAD_N
#define FPAD_N(size, N) (((size) + ((N)-1)) & (~((N)-1)))
#endif

#ifndef FPAD_8
#define FPAD_8(size) FPAD_N(size, 8)
#endif

#ifndef FPAD_16
#define FPAD_16(size) FPAD_N(size, 16)
#endif



template<class T> __forceinline void SAFE_DELETE( T *& pkInstance )
{
	delete pkInstance;
	pkInstance = NULL;
};

template<class T> __forceinline void SAFE_DELETE_ARRAY( T *& pkInstanceArray )
{
	delete[] pkInstanceArray;
	pkInstanceArray = NULL;
};

template<class T> __forceinline void SAFE_RELEASE( T *& pkRefObject )
{
	if (pkRefObject) pkRefObject->Release();
	pkRefObject = NULL;
};

#define SAFE_DELETE_INTERNAL(p)			{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY_INTERNAL(p)	{ if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE_INTERNAL(p)		{ if(p) { (p)->Release(); (p)=NULL; } }

#define SAFE_STRCPY(dest,src,len)	{ strncpy(dest,src,len); dest[len-1]=NULL; }


template<typename T> __forceinline void SAFE_FREE( T ** pT )
{
	if ( *pT != NULL )
	{
		free( *pT );
		*pT = NULL;
	}
}


struct FSafeDelete
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p );
	}
};

struct FSafeDeleteFirst
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p.first );
	}
};

struct FSafeDeleteSecond
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p.second );
	}
};

struct FSafeDeletePair
{
	template < typename T >
	void operator()( T p )
	{
		SAFE_DELETE( p.first );
		SAFE_DELETE( p.second );
	}
};

template < typename Container, typename Functor > __forceinline
void SAFE_DELETE_ALL( Container c, Functor f )
{
	std::for_each( c.begin(), c.end(), f );
}
template < typename Container > __forceinline
void SAFE_DELETE_ALL( Container c )
{
	SAFE_DELETE_ALL( c, FSafeDelete() );
}


const uint KILOBYTE	= 1024;
const uint MEGABYTE	= KILOBYTE * 1024;
const uint GIGABYTE	= MEGABYTE * 1024;
#define		CONVERT_BT_TO_KB(bt)	((bt+KILOBYTE-1)/KILOBYTE)
#define		CONVERT_BT_TO_MB(bt)	((bt+MEGABYTE-1)/MEGABYTE)
#define		CONVERT_BT_TO_GB(bt)	((bt+GIGABYTE-1)/GIGABYTE)
#define		CONVERT_KB_TO_MB(kb)	((kb+KILOBYTE-1)/KILOBYTE)
#define		CONVERT_KB_TO_GB(kb)	((kb+MEGABYTE-1)/MEGABYTE)
#define		CONVERT_MB_TO_GB(mb)	((mb+MEGABYTE-1)/MEGABYTE)
#define		CONVERT_KB_TO_BT(kb)	(kb*KILOBYTE)
#define		CONVERT_MB_TO_BT(mb)	(mb*MEGABYTE)
#define		CONVERT_GB_TO_BT(gb)	(gb*GIGABYTE)
#define		CONVERT_MB_TO_KB(mb)	(mb*KILOBYTE)
#define		CONVERT_GB_TO_KB(gb)	(gb*MEGABYTE)
#define		CONVERT_GB_TO_MB(gb)	(gb*KILOBYTE)




typedef dword FOURCC;
#define		FCC(ch4)	((((DWORD)(ch4) & 0x000000FF) << 24) |  \
            	      	(((DWORD)(ch4) & 0x0000FF00) <<  8) |   \
		                (((DWORD)(ch4) & 0x00FF0000) >>  8) |   \
		                (((DWORD)(ch4) & 0xFF000000) >> 24))


#ifndef		WIN32
struct GUID
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	byte           Data4[ 8 ];

	GUID( void )
	{
		Data1 = 0;
		Data2 = 0;
		Data3 = 0;
		memset( Data4, 0, sizeof ( Data4 ) );
	}
};

inline bool operator==( const GUID & lhs, const GUID & rhs )
{
	if ( lhs.Data1 == rhs.Data1 &&
		 lhs.Data2 == rhs.Data2 &&
		 lhs.Data3 == rhs.Data3 &&
		 memcmp(lhs.Data4, rhs.Data4, 8) == 0
	   )
	{
		return true;
	}

	return false;
}
#endif
typedef GUID GameDescription;

#endif

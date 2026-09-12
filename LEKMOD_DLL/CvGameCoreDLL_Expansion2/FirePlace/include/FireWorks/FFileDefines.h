













#ifndef		FFILEDEFINES_H
#define		FFILEDEFINES_H
#pragma		once


#undef		SUPPORT_ASYNC_IO

#if			defined(_WINPC)
	#define	SUPPORT_ASYNC_IO
#elif		defined(_XENON)
	#define	ENABLE_PAKFILE_BUFFERING
	#define	SUPPORT_ASYNC_IO
#elif		defined(_XBOX)
	#define	ENABLE_PAKFILE_BUFFERING
	#define	SUPPORT_ASYNC_IO
#elif		defined(_PS3)
	#define	ENABLE_PAKFILE_BUFFERING
	#define	SUPPORT_ASYNC_IO
#endif




#ifdef		SUPPORT_ASYNC_IO
#define		FFILE_FLAG_ASYNCHRONOUS		0x01
#else
#define		FFILE_FLAG_ASYNCHRONOUS		0
#endif
#ifdef		WIN32
#define 	FFILE_FLAG_RANDOM_ACCESS	0x02
#endif
#define		FFILE_FLAG_HIDE_ERRORS		0x04
#define		FFILE_FLAG_BIG_ENDIAN		0x10
#define		FFILE_FLAG_UNICODE			0x20
#ifdef		_XENON
#define		FFILE_FLAG_NO_CACHE			0x40
#endif


#ifdef		_XBOX
#define		FFILE_FLAG_DEFAULT			FFILE_FLAG_ASYNCHRONOUS
#endif
#ifndef		FFILE_FLAG_DEFAULT
#define		FFILE_FLAG_DEFAULT			0
#endif





#ifdef		_WINPC
const uint FFILE_DEFAULT_BUFFER_SIZE = 0;
#elif		defined(_XBOX)





const uint FFILE_DEFAULT_BUFFER_SIZE = 32 * KILOBYTE;
#elif		defined(_XENON)

const uint FFILE_DEFAULT_BUFFER_SIZE = 128 * KILOBYTE;
#elif		defined(_PS3)

const uint FFILE_DEFAULT_BUFFER_SIZE = 64 * KILOBYTE;
#elif		defined(_NDS)

const uint FFILE_DEFAULT_BUFFER_SIZE = 0;
#else

const uint FFILE_DEFAULT_BUFFER_SIZE = 32 * KILOBYTE;
#endif





enum FileErr
{
	FILE_OK,
	FILE_NOTFOUND,
	FILE_ALREADYEXISTS,
	FILE_BADMODE,
	FILE_BADSEEK,
	FILE_MAPERROR,
	FILE_VIEWERROR,
	FILE_BADNAME,
	FILE_BADSIG,
	FILE_DISKFULL,
	FILE_CORRUPT,
};

#endif

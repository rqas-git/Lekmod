#ifndef _FIFILE_H_
#define _FIFILE_H_
#pragma once

#include "FFileDefines.h"

#ifdef WIN32




class FFileTimestamp
{
public:

	enum
	{
		LESS = -1,
		EQUAL = 0,
		GREATER = 1
	};

	inline FFileTimestamp() {};
	inline FFileTimestamp( const FILETIME& t ) : m_time(t) {};

	inline bool operator<( const FFileTimestamp& a ) const  { return Compare( a ) == LESS;    };
	inline bool operator<=( const FFileTimestamp& a ) const { return Compare( a ) != GREATER; };
	inline bool operator>( const FFileTimestamp& a ) const  { return Compare( a ) == GREATER; };
	inline bool operator>=( const FFileTimestamp& a ) const { return Compare( a ) != LESS;    };
	inline bool operator==( const FFileTimestamp& a ) const { return Compare( a ) == EQUAL;   };
	inline bool operator!=( const FFileTimestamp& a ) const { return Compare( a ) != EQUAL;   };
	inline long Compare( const FFileTimestamp& a ) const { return CompareFileTime( &m_time, &a.m_time ); };

private:
	FILETIME m_time;
};

#else

typedef unsigned int FFileTimestamp;

#endif

class FFileErrorHandler;







class FIFile
{
public:
	enum SeekMode { begin=0, current=1, end=2 };
	enum OpenMode { modeRead=1, modeWrite=2, modeEdit=3 };

	virtual ~FIFile() = 0 { };

	virtual FileErr Open( const  char*  szFileName, OpenMode eOpenMode = modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE ) = 0;
	virtual FileErr Open( const wchar* wszFileName, OpenMode eOpenMode = modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE ) = 0;
	virtual FileErr Close( void ) = 0;

	virtual const wchar_t* GetFileName() = 0;

	virtual byte* GetViewCaret( void ) const = 0;
	virtual byte* GetViewEnd( void ) const = 0;
	virtual byte* GetViewPtr( void ) const = 0;

	virtual uint GetLength( void ) const = 0;
	virtual void SetLength( uint size ) = 0;

	virtual bool IsOpen( void ) const = 0;
	virtual bool IsMapped( void ) const = 0;

	virtual uint Seek( long lOffset, SeekMode eOrigin ) = 0;

	virtual uint SeekToBegin() = 0;
	virtual uint SeekToEnd() = 0;
	virtual uint GetPosition() = 0;
	virtual void Rewind( void ) = 0;

	virtual bool IsEOF( void ) = 0;

	virtual uint Read(        void* pData, uint size ) = 0;
	virtual uint Write( const void* pData, uint size ) = 0;




	template <class T> uint Read(        T& kData )
	{
		return ( Read( &kData, sizeof( T ) ) );
	}

	template <class T> uint Write( const T& kData )
	{
		return ( Write( &kData, sizeof( T ) ) );
	}

	virtual	uint Flush( void ) = 0;

	class Allocator
	{
	public:

		virtual byte* Allocate(const uint uiNumBytes) = 0;
		virtual void  Delete(byte* pBuffer)           = 0;
	};

protected:

	FIFile() { };


	FIFile( const FIFile& obj );


	FIFile& operator=( const FIFile& obj );
};





class FFileInstance
{
	FIFile* m_pkFile;
public:
	~FFileInstance();
	FFileInstance( const  char*  szFileName = NULL, FIFile::OpenMode eOpenMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE );
	FFileInstance( const wchar* wszFileName       , FIFile::OpenMode eOpenMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE );


	FIFile& operator*() { return *m_pkFile; }
	FIFile* operator->() { return m_pkFile; }
};


class FIFileSystem
{
public:

	virtual FIFile*		Create( const  char*  szFileName = NULL, FIFile::OpenMode eOpenMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE ) = 0;
	virtual FIFile*		Create( const wchar* wszFileName       , FIFile::OpenMode eOpenMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE ) = 0;
	

	virtual FileErr		Delete( const  char*  szFileName ) = 0;
	virtual FileErr		Delete( const wchar* wszFileName ) = 0;


	virtual FileErr		ReadAll( const  char*  szFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags = FFILE_FLAG_DEFAULT) = 0;
	virtual FileErr		ReadAll( const  wchar*  wszFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags = FFILE_FLAG_DEFAULT) = 0;



	virtual FileErr		ReadAll( const  char*  szFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags, FIFile::Allocator& alloc) = 0;
	virtual FileErr		ReadAll( const  wchar*  wszFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags, FIFile::Allocator& alloc) = 0;


	virtual bool		Exist( const char* pszFileName ) = 0;
	virtual bool		Exist( const wchar* pwszFileName ) = 0;


	virtual uint		GetLength( const  char*  szFileName ) = 0;
	virtual uint		GetLength( const wchar* wszFileName ) = 0;


	virtual FFileTimestamp GetLastWriteTime( const char* pFileName ) = 0;



	virtual bool		IsFileCatalogEnabled() = 0;


	virtual	FFileErrorHandler* GetErrorHandler() = 0;

	static FIFileSystem& GetInstance();
	static void			SetInstance(FIFileSystem* pkInstance);

protected:
	static FIFileSystem* ms_pInstance;
};

#if defined(FXS_IS_DLL)
#define FFILESYSTEM FIFileSystem::GetInstance()
#else



#include "FFileSystem.h"
#define FFILESYSTEM FFileSystem::GetInstance()
#endif

#endif

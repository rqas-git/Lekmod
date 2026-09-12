














#ifndef		FMEMORYSTREAM_H
#define		FMEMORYSTREAM_H
#pragma		once

#include "FDataStream.h"




class FMemoryStream : public FDataStream 
{
public:











	FMemoryStream(int size, const void* data, uint	uMemoryPool, uint uMemoryTag, bool bRealloc, bool bReallocMin);
	FMemoryStream(uint	uMemoryPool, uint uMemoryTag, bool bRealloc, bool bReallocMin);
	FMemoryStream(int size, const void* data, bool bRealloc, bool bReallocMin);
	FMemoryStream(int size, const void* data, bool bRealloc);
	FMemoryStream(int size, const void* data);
	FMemoryStream(bool bRealloc, bool bReallocMin);
	FMemoryStream(bool bRealloc);
	FMemoryStream();
	FMemoryStream(const FMemoryStream &);



	virtual ~FMemoryStream( void );



	virtual unsigned int ReadIt(unsigned int byteCount, void * buffer);
	virtual unsigned int WriteIt(unsigned int byteCount, const void* buffer);
	virtual void Skip(unsigned int deltaByteCount);
	virtual void Rewind();



	virtual void Init(int size, const void* data, bool bRealloc=false);
	virtual void Uninit();
	virtual bool AtEnd() const;
	virtual void FastFwd();
	virtual void Truncate();
	virtual unsigned int GetEOF() const;
	virtual void CopyToMem(void* mem);



	void  Grow( uint uiMinAdditionalSizeRequired = 0 );
	char* GetBuf();
	const char* GetBuf() const;
	void Init(int size, const void* data, bool bRealloc, bool bReallocMin);

	FMemoryStream & operator=(const FMemoryStream &);

protected:


	char*	m_pStart;
	char*	m_pData;
	char*	m_pStop;
	char*	m_pEndOfBuffer;
	bool    m_bRealloc;
	bool	m_bReallocMin;

	uint	m_uMemoryPool, m_uMemoryTag;
};





class FMemoryStreamRLE : public FMemoryStream 
{
public:


	FMemoryStreamRLE( int size, const void* data, bool bRealloc=false ) : FMemoryStream( size, data, bRealloc, false ) { Reset(); }
	FMemoryStreamRLE( bool bRealloc=true ) : FMemoryStream( bRealloc, false ) { Reset(); }



	virtual uint ReadIt ( uint byteCount,       void* buffer );
	virtual uint WriteIt( uint byteCount, const void* buffer );

	virtual void Skip( uint deltaByteCount ) { char ch; while ((int)deltaByteCount-- > 0) ReadIt( 1, &ch ); }
	virtual void Rewind() { Reset(); FMemoryStream::Rewind(); }

protected:
#ifdef AUI_WARNING_FIXES
	void Reset() { Run = 0; Last = MAX_UNSIGNED_INT; }
#else
	void Reset() { Run = 0; Last = EOF; }
#endif



	uint  Last;
	uint  Run;
};

#endif

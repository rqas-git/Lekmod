








































































#ifndef		FCRC_H
#define		FCRC_H
#pragma		once

#define CRC_INIT	((dword) ~0)

enum
{
	CRC12 = 0x080D,
	CRC14 = 0x202B,
	CRC16 = 0x8005,
	CCITT = 0x1021,
	CRC32 = 0xEDB88320,
};








class FCRC
{
  public:

	FCRC( dword polynomial = CRC32 );

	~FCRC();


	dword Calc		( const void *pBuf, int nLen, dword crc = CRC_INIT ) const;
	dword CalcBits	( const void *pBuf, int nLen, dword crc = CRC_INIT ) const;
	dword CalcFile	( const char *pFileName ) const ;
	dword CalcFile  ( const wchar_t* pFileName) const;








  private:


	dword m_table[ 256 ];
	dword m_polynomial;
	dword m_magic;

	void InitTable( dword polynomial );


	FCRC( const FCRC& obj );


	FCRC & operator=( const FCRC& obj );
};

extern FCRC g_CRC32;

#endif

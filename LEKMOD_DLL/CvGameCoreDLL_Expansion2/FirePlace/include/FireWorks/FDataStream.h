














#ifndef		FDATASTREAM_H
#define		FDATASTREAM_H
#pragma		once

#include "FFastVector.h"
#include "FArray.h"
#include "FEndian.h"

class YouMustImplementASerializeOperatorForThisTypeError;
class FStringA;
class FStringW;




class FDataStream
{
public:






	FDataStream(void);



	virtual	~FDataStream(void);



	template <typename T>
	FDataStream& operator<<(const T& kData);
	template <typename T>
	FDataStream& operator>>(T& kData);


	virtual unsigned int ReadIt(unsigned int byteCount, void* buffer) = 0;
	virtual unsigned int WriteIt(unsigned int byteCount, const void* buffer) = 0;
	virtual void Skip(unsigned int deltaByteCount) = 0;
	virtual void Rewind() = 0;



	virtual bool AtEnd() const = 0;
	virtual void FastFwd() = 0;
	virtual unsigned int GetBytesUsed() const;
	virtual unsigned int GetPosition() const;
	virtual void SetPosition(unsigned int position);
	virtual void Truncate();
	virtual void Flush();
	virtual unsigned int GetEOF() const;
	virtual unsigned int GetSizeLeft() const;
	virtual void CopyToMem(void* mem);
	virtual void RaiseException( std::exception& kException ) const;





	void SetDesiredEndianNess(FEndian::EEndianness e) { m_eEndianness = e; }
	FEndian::EEndianness GetDesiredEndianNess() const { return m_eEndianness; }


	unsigned int WriteString(const char* szName);
	unsigned int WriteString(const wchar* szName);
	char* ReadString();
	wchar* ReadWideString();
	unsigned int ReadString(char* szName);
	unsigned int ReadString(wchar* szName);

protected:


	unsigned int WriteString(const FStringA& szName);
	unsigned int WriteString(int count, const FStringA values[]);
	unsigned int WriteString(const FStringW& szName);
	unsigned int WriteString(int count, const FStringW values[]);
	unsigned int WriteString(const std::string& szName);
	unsigned int WriteString(int count, const std::string values[]);
	unsigned int WriteString(const std::wstring& szName);
	unsigned int WriteString(int count, const std::wstring values[]);
	void Write(const YouMustImplementASerializeOperatorForThisTypeError & szName);
	void Write(const FStringA& szName);
	void Write(int count, const FStringA values[]);
	void Write(const FStringW& szName);
	void Write(int count, const FStringW values[]);
	void Write(const std::string& szName);
	void Write(int count, const std::string values[]);
	void Write(const std::wstring& szName);
	void Write(int count, const std::wstring values[]);



	template <typename T>
	void Write(int count, const T values[]);

	void Write(const GUID &);
	void Write(const bool &);
	void Write(const unsigned char &);
	void Write(const char &);
	void Write(const INT32 &);
	void Write(const UINT32 &);
	void Write(const UINT64 &);
	void Write(const INT64 &);
#ifndef		_NDS

	void Write(const unsigned long &);
#endif
	void Write(const INT16 &);
	void Write(const UINT16 &);
	void Write(const WCHAR &);
	void Write(const float &);
	void Write(const double &);

	void Write(int, const INT32 values[]);
	void Write(int, const UINT32 values[]);
	void Write(int, const INT64 values[]);
	void Write(int, const UINT64 values[]);
#ifndef		_NDS

	void Write(int, const unsigned long values[]);
#endif
	void Write(int, const INT16 values[]);
	void Write(int, const UINT16 values[]);
	void Write(int, const WCHAR values[]);
	void Write(int, const float values[]);
	void Write(int, const double values[]);
	void Write(int, const GUID values[]);


	unsigned int ReadString(FStringA& szName);
	unsigned int ReadString(int count, FStringA values[]);
	unsigned int ReadString(FStringW& szName);
	unsigned int ReadString(int count, FStringW values[]);
	unsigned int ReadString(std::string& szName);
	unsigned int ReadString(int count, std::string values[]);
	unsigned int ReadString(std::wstring& szName);
	unsigned int ReadString(int count, std::wstring values[]);
	
	void Read(YouMustImplementASerializeOperatorForThisTypeError & szName);
	void Read(FStringA& szName);
	void Read(int count, FStringA values[]);
	void Read(FStringW& szName);
	void Read(int count, FStringW values[]);
	void Read(std::string& szName);
	void Read(int count, std::string values[]);
	void Read(std::wstring& szName);
	void Read(int count, std::wstring values[]);


	
	template <class T>
	void Read(int count, T values[]);
	
	void Read(GUID &);
	void Read(bool &);
	void Read(unsigned char &);
	void Read(char &);

	void Read(INT32 &);
	void Read(UINT32 &);
	void Read(UINT64 &);
	void Read(INT64 &);
#ifndef		_NDS

	void Read(unsigned long &);
#endif
	void Read(INT16 &);
	void Read(UINT16 &);
	void Read(WCHAR &);
	void Read(float &);
	void Read(double &);

	void Read(int, GUID values[]); 
	void Read(int, INT32 values[]);
	void Read(int, UINT32 values[]);
	void Read(int, INT64 values[]);
	void Read(int, UINT64 values[]);
#ifndef		_NDS

	void Read(int, unsigned long values[]);
#endif
	void Read(int, INT16 values[]);
	void Read(int, UINT16 values[]);
	void Read(int, WCHAR values[]);
	void Read(int, float values[]);
	void Read(int, double values[]);



protected:


	bool m_bEnableCB;
	FEndian::EEndianness m_eEndianness;

	unsigned int m_uiBytesRead;
	unsigned int m_uiPosition;	
};

FDataStream & operator<<(FDataStream & saveTo, FDataStream & readFrom);
FDataStream & operator>>(FDataStream & loadFrom, FDataStream & writeTo);

template<typename ValueType, size_t count>
FDataStream & operator<<(FDataStream & saveTo, const ValueType (&readFrom)[count])
{
	size_t i = 0;
	for(i = 0; i < count; ++i)
	{
		saveTo << readFrom[i];
	}
	return saveTo;
}

template<typename ValueType, size_t count>
FDataStream & operator>>(FDataStream & loadFrom, ValueType (&writeTo)[count])
{
	size_t i = 0;
	for(i = 0; i < count; ++i)
	{
		loadFrom >> writeTo[i];
	}
	return loadFrom;
}

template < class T >
inline FDataStream& FDataStream::operator <<( const T& kData )
{







	Write( kData );
	return ( *this );
}

template < class T >
inline FDataStream& FDataStream::operator >>( T& kData )
{







	Read( kData );
	return ( *this );
}














































template < class T >
inline void FDataStream::Write( int count, const T values[] )
{
	if (sizeof(T) == 1)
		WriteIt( count, values );
	else
	{
		int i;
		for(i=0;i<count;i++)
		{

			*this << values[i];
		}
	}
}

template < class T >
inline void FDataStream::Read( int count, T values[] )
{
	if (sizeof(T) == 1)
		ReadIt( count, values );
	else
	{
		int i;
		for(i=0;i<count;i++)
		{





			*this >> values[i];
		}
	}
}






















template<typename ValueType>
class ArrayWrapper
{
public:
	ArrayWrapper(int count, ValueType * values)
		: m_values(values), m_count(count) {}

	ValueType * getArray()
	{
		return m_values;
	}
	const ValueType * getArray() const
	{
		return m_values;
	}

	int getCount() const
	{
		return m_count;
	}





	void readCounted(FDataStream & readFrom, ValueType tFill = (ValueType)0)
	{
		int i = 0;
		int iStreamCount;
		readFrom >> iStreamCount;
		int iArrayCount = getCount();
		int readCount = (iStreamCount < iArrayCount)?iStreamCount:iArrayCount;
		ValueType * values = getArray();
		for(i = 0; i < readCount; ++i)
		{
			readFrom >> values[i];
		}
		if (iStreamCount < iArrayCount)
		{
			for(; i < iArrayCount; ++i)
			{
				values[i] = tFill;
			}
		}
		else
		if (iStreamCount > iArrayCount)
		{
			for(; i < iStreamCount; ++i)
			{
				readFrom >> tFill;
			}		
		}
		
	}


	void writeCounted(FDataStream & writeTo)
	{
		int i = 0;
		int count = getCount();
		writeTo << count;
		const ValueType * values = getArray();
		for(i = 0; i < count; ++i)
		{
			writeTo << values[i];
		}
	}

private:


	ArrayWrapper();
	ArrayWrapper(const ArrayWrapper &);
	ArrayWrapper & operator=(const ArrayWrapper &);

	ValueType * m_values;
	int         m_count;
};


template< typename ValueType >
class ArrayWrapperConst
{
public:
	ArrayWrapperConst(int count, const ValueType * values)
		: m_values(values), m_count(count) {}

	const ValueType* getArray() const
	{
		return m_values;
	}
	int getCount() const
	{
		return m_count;
	}


	void writeCounted(FDataStream & writeTo)
	{
		int i = 0;
		int count = getCount();
		writeTo << count;
		const ValueType * values = getArray();
		for(i = 0; i < count; ++i)
		{
			writeTo << values[i];
		}
	}

private:



	ArrayWrapperConst();
	ArrayWrapperConst(const ArrayWrapperConst &);
	ArrayWrapperConst & operator=(const ArrayWrapperConst &);

	const ValueType * m_values;
	int         m_count;
};







template<typename ValueType>
FDataStream & operator<<(FDataStream & saveTo, const ArrayWrapper<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	const ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		saveTo << values[i];
	}
	return saveTo;
}

template<typename ValueType>
FDataStream & operator>>(FDataStream & loadFrom, ArrayWrapper<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		loadFrom >> values[i];
	}
	return loadFrom;
}

#if defined(LEKMOD_MACOS)

template<typename ValueType>
FDataStream & operator>>(FDataStream & loadFrom, ArrayWrapper<ValueType> && v)
{
    return operator>>(loadFrom, v);
}

#endif
template<typename ValueType>
FDataStream & operator<<(FDataStream & saveTo, const ArrayWrapperConst<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	const ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		saveTo << values[i];
	}
	return saveTo;
}

template<typename ValueType>
FDataStream & operator>>(FDataStream & loadFrom, ArrayWrapperConst<ValueType> & v)
{
	int i = 0;
	int count = v.getCount();
	ValueType * values = v.getArray();
	for(i = 0; i < count; ++i)
	{
		loadFrom >> values[i];
	}
	return loadFrom;
}







template< class T, unsigned int ARRAY_SIZE >
FDataStream & operator >> ( FDataStream & kStream, Firaxis::Array<T, ARRAY_SIZE> & kArray )
{
	for( unsigned int i = 0; i < kArray.size(); ++i )
	{
		kStream >> kArray[i];
	}
	return kStream;
}

template< class T, unsigned int ARRAY_SIZE >
FDataStream & operator << (FDataStream & kStream, const Firaxis::Array< T, ARRAY_SIZE > & kArray )
{
	for( unsigned int i = 0; i < kArray.size(); ++i )
	{
		kStream << kArray[i];
	}
	return kStream;
}







template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator >> ( FDataStream & kStream, FFastVector< T, bPODType, AllocPool, nSubID > & kVec )
{
	unsigned int iSize;
	kStream >> iSize;

	if(iSize > kVec.size())
	{
		unsigned int count = iSize - kVec.size();
		kVec.push_back_copy(T(), count);
	}
	
	unsigned int i = 0;
	for( i = 0; i < iSize; ++i )
	{
		kStream >> kVec[i];
	}
	return kStream;
}

template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator << ( FDataStream & kStream, const FFastVector< T, bPODType, AllocPool, nSubID > & kVec )
{
	kStream << kVec.size();
	for( unsigned int i = 0; i < kVec.size(); ++i )
	{
		kStream << kVec[i];
	}
	return kStream;
}

template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator >> ( FDataStream & kStream, FStaticVector< T, L, bPODType, AllocPool, nSubID > & kVec )
{
	unsigned int iSize;
	kStream >> iSize;
	kVec.resize( iSize );

	for( unsigned int i = 0; i < iSize; ++i )
	{
		kStream >> kVec[i];
	}
	return kStream;
}

template< class T, unsigned int L, bool bPODType, unsigned int AllocPool, unsigned int nSubID >
FDataStream & operator << ( FDataStream & kStream, const FStaticVector< T, L, bPODType, AllocPool, nSubID > & kVec )
{
	kStream << (unsigned int)kVec.size();

	for( unsigned int i = 0; i < kVec.size(); ++i )
	{
		kStream << kVec[i];
	}
	return kStream;
}

#endif

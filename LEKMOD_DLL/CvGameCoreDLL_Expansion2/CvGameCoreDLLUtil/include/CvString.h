#pragma once

#ifndef CvString_h
#define CvString_h

#include <string>
#include <cstdarg>

#include "CvAssert.h"
class FDataStream;









class CvString : public std::string
{
public:
	CvString() {}
	CvString(int iLen) { reserve(iLen); }
	CvString(const char* s) : std::string(s ? s : "") {CvAssertMsg(s != NULL, "Passing NULL to std::string; possible heap corruption!");}
	CvString(const std::string& s): std::string(s) {}

	~CvString() {}


	operator const char*() const 	{ return c_str(); }												


	char& operator[](int i) { return std::string::operator[](i);	}
	char& operator[](std::string::size_type i) { return std::string::operator[](i);	}
	const char operator[](int i) const { return std::string::operator[](i);	}
	CvString& operator=( const char* s) { if (s) assign(s); else clear();	return *this; }	
	CvString& operator=( const std::string& s) { assign(s.c_str());	return *this; }	


#if defined(LEKMOD_MACOS)
	bool IsEmpty() const { return (empty() || (*this)[0] == '\0');}
#else
	bool IsEmpty() const { return (empty() || this[0] == '\0');}
#endif
	const char* GetCString() const 	{ return c_str(); }
	int CompareNoCase( const char* lpsz ) const { return _stricmp(lpsz, c_str()); }
	int CompareNoCase( const char* lpsz, int iLength ) const { return _strnicmp(lpsz, c_str(), iLength);  }
	void Format( const char* lpszFormat, ... );
	int GetLength() const { return (int) size(); }
	int Replace( char chOld, char chNew );


	static bool formatv(std::string& out, const char * fmt, va_list args);
	static bool format(std::string & out, const char * fmt, ...);
	static CvString format(const char * fmt, ...);
	static std::string formatv(const char * fmt, va_list args);
};

class CvStringBuffer
{
public:
	CvStringBuffer()
	{
		m_pBuffer = NULL;
		m_iLength = 0;
		m_iCapacity = 0;
	}

	~CvStringBuffer()
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	void append(char character)
	{
		int newLength = m_iLength + 1;
		ensureCapacity(newLength + 1);
		m_pBuffer[m_iLength] = character;
		m_pBuffer[m_iLength + 1] = 0;
		m_iLength = newLength;
	}

	void append(const char* szCharacters)
	{
		if(szCharacters == NULL)
			return;

		int inputLength = (int) strlen(szCharacters);
		int newLength = m_iLength + inputLength;
		ensureCapacity(newLength + 1);


		memcpy(m_pBuffer + m_iLength, szCharacters, sizeof(char) * (inputLength + 1));
		m_iLength = newLength;
	}

	void append(const CvString &szString)
	{
		append(szString.GetCString());
	}

	void append(const CvStringBuffer &szStringBuffer)
	{
		append(szStringBuffer.m_pBuffer);
	}

	void assign(const CvString &szString)
	{
		assign(szString.c_str());
	}

	void assign(const char* szCharacters)
	{
		clear();
		append(szCharacters);
	}

	void clear()
	{
		if(m_pBuffer != NULL)
		{
			m_iLength = 0;
			m_pBuffer[0] = 0;
		}
	}

	bool isEmpty() const
	{
		if(m_iLength == 0)
			return true;
		else
			return false;
	}

	const char *getCString()
	{
		ensureCapacity(1);
		return m_pBuffer;
	}

private:
	void ensureCapacity(int newCapacity)
	{
		if(newCapacity > m_iCapacity)
		{
			m_iCapacity = 2 * newCapacity;
			char *newBuffer = new char[m_iCapacity];


			if(m_pBuffer != NULL)
			{
				memcpy(newBuffer, m_pBuffer, sizeof(char) * (m_iLength + 1));

				delete [] m_pBuffer;
			}
			else
			{
				newBuffer[0] = 0;
			}

			m_pBuffer = newBuffer;
		}
	}

	char *m_pBuffer;
	int m_iLength;
	int m_iCapacity;
};






inline int CvString::Replace( char chOld, char chNew )
{
	int i, iCnt = 0;
	for(i=0;i<(int)size();i++)
	{
		if ((*this)[i] == chOld)
		{
			replace(i, 1, std::string(1, chNew) );
			iCnt++;
		}
	}
	return iCnt;
}




inline bool CvString::formatv(std::string & out, const char * fmt, va_list args)
{
	char buf[2048];
	char * pbuf = buf;
	int len = 0;
	int attempts = 0;
	bool success = false;
	const int kMaxAttempts = 40;

	do
	{
		int maxlen = 2047+2048*attempts;
		len = _vsnprintf(pbuf,maxlen,fmt,args);
		attempts++;
		success = (len>=0 && len<=maxlen);
		if (!success)
		{
			if (pbuf!=buf)
				delete [] pbuf;
			pbuf = new char[2048+2048*attempts];
		}
	}
	while (!success && attempts<kMaxAttempts);

	if ( attempts==kMaxAttempts )
	{

	}

	if (success)
		out = pbuf;
	else
		out = "";

	if (pbuf!=buf)
		delete [] pbuf;

	return success;
}




inline std::string CvString::formatv(const char * fmt, va_list args)
{
	std::string result;
	formatv( result, fmt, args );
	return result;
}



inline CvString CvString::format(const char * fmt, ...)
{
	std::string result;
	va_list args;
	va_start(args,fmt);
	formatv(result,fmt,args);
	va_end(args);
	return CvString(result);
}




inline bool CvString::format(std::string & out, const char * fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	bool r = formatv(out,fmt,args);
	va_end(args);
	return r;
}





inline void CvString::Format( const char* lpszFormat, ... )
{
	std::string result;
	va_list args;
	va_start(args,lpszFormat);
	formatv(result,lpszFormat,args);
	va_end(args);
	*this = result;
}

#endif

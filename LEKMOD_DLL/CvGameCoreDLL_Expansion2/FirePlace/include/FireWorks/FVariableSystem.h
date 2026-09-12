











#ifndef		FVARIABLESYSTEM_H
#define		FVARIABLESYSTEM_H
#pragma		once

#include "FString.h"


enum eVariableType
{
	FVARTYPE_BOOL,
	FVARTYPE_CHAR,
	FVARTYPE_UCHAR,
	FVARTYPE_SHORT,
	FVARTYPE_USHORT,
	FVARTYPE_INT,
	FVARTYPE_UINT,
	FVARTYPE_FLOAT,
	FVARTYPE_DOUBLE,
	FVARTYPE_STRING,
	FVARTYPE_WSTRING,
	FVARTYPE_COUNT
};
class FDataStream;
FDataStream & operator<<(FDataStream &, const eVariableType &);
FDataStream & operator>>(FDataStream &, eVariableType &);

class FVariable;
class FVariableDebug;


#ifdef		FINAL_RELEASE
#define FVARTYPE FVariable
#else
#define FVARTYPE FVariableDebug
#endif

typedef STDEXT::hash_map< FString, FVARTYPE*, FStringHashTraits > FVariableHash;









class FVariableCallback
{
	public:
        virtual ~FVariableCallback() {};
#ifdef AUI_WARNING_FIXES
		virtual void VariableAttached(const FVARTYPE *               ) { };
		virtual void VariableHasChanged(const FVARTYPE *               ) { };
		virtual void VariableRemoved(const FVARTYPE *               ) { };
#else
		virtual void VariableAttached(const FVARTYPE * pkVariable) { };
		virtual void VariableHasChanged(const FVARTYPE * pkVariable) { };
		virtual void VariableRemoved(const FVARTYPE * pkVariable) { };
#endif
		virtual bool FinalValueOnly() const { return false; }
		virtual bool ApplyInitialState() const { return true; }
};











class FDataStream;
class FVariable
{
	public:
		FVariable() : m_dValue(0) {}
		FVariable(const FVariable& src) { CopyFrom(src); }
		virtual ~FVariable();

		const FVariable& operator=( const FVariable& varSrc ) { CopyFrom(varSrc); return *this; }
		void CopyFrom(const FVariable& varSrc);
		void Read(FDataStream &);
		void Write(FDataStream &) const;

		union
		{
			bool		m_bValue;
			char		m_cValue;
			byte		m_ucValue;
			short		m_wValue;
			word		m_uwValue;
			int			m_iValue;
			uint		m_uiValue;
			float		m_fValue;
			double		m_dValue;
			char *		m_szValue;
			wchar_t *	m_wszValue;
		};

		eVariableType	m_eType;

#ifdef AUI_WARNING_FIXES
		virtual void SetCategory(const char *               ) { };
		virtual void SetEditType(const char *               ) { };
		virtual void SetCallback(FVariableCallback *               ) { };
#else
		virtual void SetCategory( const char * szCategory) { };
		virtual void SetEditType( const char * szEditType) { };
		virtual void SetCallback( FVariableCallback * pkCallback ) { };
#endif

		virtual const char * GetCategory() const
		{
			return "Default";
		}

		static const char * GetEditType( eVariableType eType )
		{
			switch( eType )
			{
			case FVARTYPE_BOOL: return "Check"; break;
			case FVARTYPE_CHAR: return "Char"; break;
			case FVARTYPE_UCHAR: return "Char"; break;
			case FVARTYPE_SHORT: return "Int"; break;
			case FVARTYPE_USHORT: return "UInt"; break;
			case FVARTYPE_INT: return "Int"; break;
			case FVARTYPE_UINT: return "UInt"; break;
			case FVARTYPE_FLOAT: return "Float"; break;
			case FVARTYPE_DOUBLE: return "Float"; break;
			default: case FVARTYPE_STRING: return "String"; break;
			case FVARTYPE_WSTRING: return "String"; break;
			}
		}

		static eVariableType GetEditType( const char * szType )
		{
			if ( !_stricmp(szType, "Check" ) ) return FVARTYPE_BOOL; 
			if ( !_stricmp(szType, "Char")) return FVARTYPE_CHAR; 
			if ( !_stricmp(szType, "Char")) return FVARTYPE_UCHAR; 
			if ( !_stricmp(szType, "Int")) return FVARTYPE_SHORT; 
			if ( !_stricmp(szType, "UInt")) return FVARTYPE_USHORT; 
			if ( !_stricmp(szType, "Int")) return FVARTYPE_INT; 
			if ( !_stricmp(szType, "UInt")) return FVARTYPE_UINT; 
			if ( !_stricmp(szType, "Float"))return FVARTYPE_FLOAT; 
			if ( !_stricmp(szType, "Float")) return FVARTYPE_DOUBLE; 
			if ( !_stricmp(szType, "String")) return FVARTYPE_STRING; 
			if ( !_stricmp(szType, "String")) return FVARTYPE_WSTRING; 
			return FVARTYPE_INT;
		}

		virtual const char * GetEditType() const
		{
			return GetEditType(m_eType);
		}

		virtual FVariableCallback * GetCallback() const { return NULL; }

#ifdef AUI_WARNING_FIXES
		virtual void SetVariableRange(const char *          , const char *          ) {};
#else
		virtual void SetVariableRange( const char * szMin, const char * szMax ) {};
#endif
		virtual const char * GetVariableMinimum() const { return NULL; };
		virtual const char * GetVariableMaximum() const { return NULL; };
};






class FVariableDebug : public FVariable
{
	public:
		FVariableDebug() :  FVariable(),  m_szCategory(NULL),  m_szEditType(NULL),  m_pkVariableCallback(NULL),
			m_szMinimum(NULL), m_szMaximum(NULL) {}
		FVariableDebug(const FVariableDebug& src) : FVariable(src) 
		{  
			m_szCategory = src.m_szCategory;
			m_szEditType = src.m_szEditType;
			m_szMinimum = src.m_szMinimum;
			m_szMaximum = src.m_szMaximum;
			m_pkVariableCallback = src.m_pkVariableCallback;
		}
		virtual ~FVariableDebug() {}

		virtual const char * GetCategory() const { return m_szCategory ? m_szCategory : FVariable::GetCategory(); }
		virtual void SetCategory(const char * szCategory) { m_szCategory = szCategory; }
		virtual const char * GetEditType() const { return m_szEditType ? m_szEditType : FVariable::GetEditType(); }
		virtual void SetEditType(const char * szEditType) { m_szEditType = szEditType; }
		virtual FVariableCallback * GetCallback() const { return m_pkVariableCallback; }
		virtual void SetCallback( FVariableCallback * pkCallback ) { m_pkVariableCallback = pkCallback; }
		virtual void SetVariableRange( const char * szMin, const char * szMax )
		{
			m_szMinimum = szMin;
			m_szMaximum = szMax;
		};
		virtual const char * GetVariableMinimum() const { return m_szMinimum; };
		virtual const char * GetVariableMaximum() const { return m_szMaximum; };


	private:
		const char *		m_szCategory;
		const char *		m_szEditType;
        FVariableCallback * m_pkVariableCallback;



		const char *		m_szMinimum;			
		const char *		m_szMaximum;
};










class FVariableSystem
{
	public:


		FVariableSystem( );
		virtual ~FVariableSystem( );

		void UnInit();


		uint GetSize() const;


		bool GetValue( const char * szVariable, bool & bValue ) const;
		bool GetValue( const char * szVariable, char & cValue ) const;
		bool GetValue( const char * szVariable, byte & ucValue ) const;
		bool GetValue( const char * szVariable, short & wValue ) const;
		bool GetValue( const char * szVariable, word & uwValue ) const;
		bool GetValue( const char * szVariable, int & iValue ) const;
		bool GetValue( const char * szVariable, uint & uiValue ) const;
		bool GetValue( const char * szVariable, float & fValue ) const;
		bool GetValue( const char * szVariable, double & dValue ) const;
		bool GetValue( const char * szVariable, const char * & pszValue ) const;
		bool GetValue( const char * szVariable, const wchar * & pszValue ) const;
		const FVARTYPE * GetVariable( const char * szVariable ) const;
		FVARTYPE * GetVariable( const char * szVariable );


		void SetValue( const char * szVariable, bool bValue );
		void SetValue( const char * szVariable, char cValue );
		void SetValue( const char * szVariable, byte ucValue );
		void SetValue( const char * szVariable, short wValue );
		void SetValue( const char * szVariable, word uwValue );
		void SetValue( const char * szVariable, int iValue );
		void SetValue( const char * szVariable, uint uiValue );
		void SetValue( const char * szVariable, float fValue );
		void SetValue( const char * szVariable, double dValue );
		void SetValue( const char * szVariable, const char * szValue );
		void SetValue( const char * szVariable, const wchar * wszValue );
		bool SetValueFromType( const char * szVariable, const char * szType, const char * szValue );


		bool SetCategoryEditTypeCallback( const char * szVariable, 
			const char * szCategory, const char * szEditType, FVariableCallback * pkCallback );
		bool SetVariableRange( const char * szVariable, const char * szMin, const char * szMax );


		bool RemValue( const char * szVariable );


		std::string GetFirstVariableName( );
		std::string GetNextVariableName( );

		void Read(FDataStream &);
		void Write(FDataStream &) const;

protected:


		FVARTYPE * PreSetValue(const char * szVariable);

		FVariableHash				m_mapVariableMap;
		FVariableHash::iterator		m_iVariableIterator;


};

FDataStream & operator<<(FDataStream &, const FVariable &);
FDataStream & operator>>(FDataStream &, FVariable &);


#if _MSC_VER <= 1400

#endif

#endif

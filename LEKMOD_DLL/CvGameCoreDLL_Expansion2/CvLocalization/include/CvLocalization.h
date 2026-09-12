

#pragma once 
#ifndef _CVLOCALIZATION_H
#define _CVLOCALIZATION_H







#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifndef CvLocalizationAPI
#define CvLocalizationAPI __declspec(dllimport)
#endif

#include "ILocalizationLogger.h"

namespace Localization
{


	enum Gender{
		GENDER_NEUTER = 0,
		GENDER_MASCULINE,
		GENDER_FEMININE,
	};


	class String;
	class StringDictionary;

	class IPluralityRule
	{
	public:

		virtual unsigned int GetMaxPluralForms() const = 0;
		virtual unsigned int GetPluralForm(int nElements) const = 0;
	};


	class CvLocalizationAPI StringParameter
	{
	public:
		StringParameter();
		StringParameter(const StringParameter& other);

		int ArgumentIndex;
		size_t uiTextKeyStart;
		size_t lenTextKey;

		size_t uiParameterStart;
		size_t uiParameterEnd;

		size_t uiTransformTagStart;
		size_t uiTransformTagEnd;

		std::vector<StringParameter> m_injected_parameters;
	};


	class CvLocalizationAPI ParameterArgument
	{
	public:
		enum Type{
			ARGTYPE_INT,
			ARGTYPE_INT64,
			ARGTYPE_FLOAT,
			ARGTYPE_DOUBLE,
			ARGTYPE_STRING,
		};

		ParameterArgument(const ParameterArgument& other);
		ParameterArgument(const String& strText);
		ParameterArgument(const int val);
		ParameterArgument(const __int64 val);
		ParameterArgument(const float val);
		ParameterArgument(const double val);

		union {
			int		i;
			__int64	i64;
			float	f;
			double	d;
		} m_value;

		std::tr1::shared_ptr<String> m_str;

		Type m_type;
	};

	class ParameterTransforms
	{
	public:
		ParameterTransforms(const char* szLocaleCode);
		virtual ~ParameterTransforms();


		CvLocalizationAPI bool FormatCurrency(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatNumber(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatPercent(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatRoman(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatSpellout(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;


		CvLocalizationAPI bool FormatUppercase(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatLowercase(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;;
		CvLocalizationAPI bool FormatTitlecase(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatTextkey(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;


		CvLocalizationAPI bool ChoiceFormatGender(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool ChoiceFormatPlurality(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool ChoiceFormatText(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
	};





	class Language
	{
	public:
		Language(const char* szLanguageType, Localization::IPluralityRule* pkPluralityRule);
		Language(const Language& copy);
		virtual ~Language();


		CvLocalizationAPI const char* GetType() const;


		CvLocalizationAPI unsigned int GetPluralForm(int nElements) const;
		CvLocalizationAPI Localization::IPluralityRule* GetPluralityRule() const;
		CvLocalizationAPI void SetPluralityRule(Localization::IPluralityRule* pkPluralityRule);

		CvLocalizationAPI bool Compare(const char* szString1, size_t lenString1, const char* szString2, size_t lenString2, int& result) const;


		CvLocalizationAPI const Localization::ParameterTransforms& GetParameterTransforms() const;
	};


	CvLocalizationAPI bool Initialize(const wchar_t* wszIcuDatFile, Database::Connection& db, const char* szLanguagesTableName, const char* szLanguageType, bool bDisableFallbackLanguageSupport);
	CvLocalizationAPI bool Shutdown();


	CvLocalizationAPI bool SetCurrentLanguage(const char* szType);
	CvLocalizationAPI const Localization::Language& GetCurrentLanguage();



	CvLocalizationAPI const Localization::StringDictionary& GetStringDictionary();
	CvLocalizationAPI Localization::String Lookup(const char* textKey);

	CvLocalizationAPI bool ShutdownDictionary();
	CvLocalizationAPI bool InitializeDictionary(Database::Connection& db, const char* szLanguagesTableName, const char* szLanguageType, bool bDisableFallbackLanguageSupport);



	CvLocalizationAPI void SetLogger(ILocalizationLogger* pkLogger);
	CvLocalizationAPI ILocalizationLogger* GetLogger();
	CvLocalizationAPI void LogMessage(const char* szMessage);
	CvLocalizationAPI void LogWarning(const char* szWarning);
	CvLocalizationAPI void LogError(const char* szError);


	CvLocalizationAPI Localization::String ConvertUTCDatetoLocalTZDate(SYSTEMTIME& stUniversalTime, _In_opt_z_ const char* szDTFormat);
	CvLocalizationAPI Localization::String ConvertUTCTimetoLocalTZTime(SYSTEMTIME& stUniversalTime, _In_opt_z_ const char* szTimeFormat);
	CvLocalizationAPI Localization::String ConvertUTCDateTimetoLocalTZDateTime(SYSTEMTIME& stUniversalTime, _In_opt_z_ const char* szDateFormat, _In_opt_z_ const char* szTimeFormat);








	class CvLocalizationAPI SharedString
	{
	public:
		SharedString();
		SharedString(_In_bytecount_(length) const char* szString, size_t length);
		SharedString(const SharedString& copy);

		SharedString& operator = (const SharedString& other);


		const char* GetBuffer(size_t& length) const;


		const char* GetString(size_t& length) const;


		size_t GetBufferLength() const;


		size_t GetOffset() const;


		size_t GetLength() const;


		bool SetOffsetAndLength(size_t offset, size_t length);

	private:

		std::tr1::shared_ptr<char> m_Buffer;


		size_t m_BufferLength;


		size_t m_offset;


		size_t m_length;
	};



	class SingleFormString
	{
	public:
		SingleFormString();
		SingleFormString(const SharedString& baseString, size_t plurality = 0, Gender gender = GENDER_NEUTER);
		virtual ~SingleFormString();

		SingleFormString& operator = (const SingleFormString& other);

		const SharedString& GetBaseString() const;

		const SharedString& GetComposedString() const;

		void SetPlurality();
		size_t GetPlurality() const;

		void SetGender(Gender gender);
		Gender GetGender() const;

		bool HasTrait(const char* szTrait, size_t lenTrait) const;

		bool Parse();
		bool Compose(const std::vector<ParameterArgument>& arguments);
	private:
		SharedString m_BaseString;
		SharedString m_ComposedString;

		size_t m_Plurality;
		int m_Gender;
		std::vector<std::string> m_Traits;
		std::vector<StringParameter> m_Parameters;

		bool m_Composed;
		bool m_Parsed;
	};




			















			


	class String
	{
	public:
		CvLocalizationAPI String();
		CvLocalizationAPI String(const char* szString);
		CvLocalizationAPI String(const char* szString, unsigned int uiLen);
		CvLocalizationAPI String(const String& other);

		CvLocalizationAPI virtual ~String();




		CvLocalizationAPI bool IsEmpty() const;



		CvLocalizationAPI String& operator = (const String& other);
		CvLocalizationAPI String& operator = (_In_z_ const char* szBaseString);






		CvLocalizationAPI const char* toUTF8();
		CvLocalizationAPI const char* toUTF8(size_t& bytes, int nForm = 1);




		CvLocalizationAPI const char* GetBaseString(size_t& bytes) const;






		CvLocalizationAPI Localization::Language* GetLanguage() const;



		CvLocalizationAPI bool SetLanguage(Localization::Language* Language);



	

		CvLocalizationAPI size_t GetArgumentCount() const;


		CvLocalizationAPI bool PushArgument(_In_z_ const char* szText);
		CvLocalizationAPI bool PushArgument(_In_bytecount_(length) const char* szText, size_t length);
		CvLocalizationAPI bool PushArgument(const int val);
#ifdef LEKMOD_MACOS

        CvLocalizationAPI bool PushArgument(long val);
        bool PushArgument(__int64 val) { return PushArgument(static_cast<long>(val)); }
#else
		CvLocalizationAPI bool PushArgument(const __int64 val);
#endif
		CvLocalizationAPI bool PushArgument(const float val);
		CvLocalizationAPI bool PushArgument(const double val);
		CvLocalizationAPI bool PushArgument(const String& strText);
		bool PushArgument(const ParameterArgument& argument);

		template<size_t N>
		bool PushArgument(const char(&val)[N]);



		CvLocalizationAPI size_t GetNumForms() const;


		CvLocalizationAPI size_t GetFormByPlurality(int iPlurality, int nOffset = 0) const;


		CvLocalizationAPI size_t GetFormByGender(_In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;


		CvLocalizationAPI size_t GetFormByPluralityAndGender(int iPlurality, _In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;

		template<size_t N>
		size_t GetFormByGender(const char(&szGender)[N], int nOffset = 0) const;

		template<size_t N>
		size_t GetFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset = 0) const;


		CvLocalizationAPI bool HasForm(int nForm) const;


		CvLocalizationAPI bool HasFormByPlurality(int iPlurality, int nOffset = 0) const;


		CvLocalizationAPI bool HasFormByGender(_In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;


		CvLocalizationAPI bool HasFormByPluralityAndGender(int iPlurality, _In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;

		template<size_t N>
		bool HasFormByGender(const char(&szGender)[N], int nOffset = 0) const;

		template<size_t N>
		bool HasFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset = 0) const;



		CvLocalizationAPI bool SetGender(_In_bytecount_(lenGender) const char* szGender, size_t lenGender);


		CvLocalizationAPI bool SetGender(size_t nForm, _In_bytecount_(lenGender) const char* szGender, size_t lenGender);


		CvLocalizationAPI bool SetGender(Gender gender);


		CvLocalizationAPI bool SetGender(size_t nForm, Gender gender);


		CvLocalizationAPI Gender GetGender(size_t nForm = 1) const;

		template<size_t N>
		bool SetGender(const char(&szGender)[N]);




		CvLocalizationAPI bool SetPlurality(_In_bytecount_(lenPlurality) const char* szPlurality, size_t lenPlurality);


		CvLocalizationAPI bool SetPlurality(size_t iPlurality);


		CvLocalizationAPI bool SetPlurality(size_t nForm, size_t iPlurality);


		CvLocalizationAPI size_t GetPlurality(size_t nForm = 1) const;

		template<size_t N>
		bool SetPlurality(const char(&szPlurality)[N]);




		CvLocalizationAPI bool Compose(size_t nForm = 1);


		CvLocalizationAPI bool Parse(size_t nForm = 1);


		CvLocalizationAPI static String Empty;
		
	private:
		SharedString m_BaseString;
		SharedString m_ComposedNullTerminated;

		std::vector<ParameterArgument>	m_arguments;
		std::vector<SingleFormString> m_Forms;

		Localization::Language* m_pkLanguage;
	};


	class StringDictionary
	{
	public:

		class Language
		{
		public:


			CvLocalizationAPI const char* GetType() const;
			CvLocalizationAPI const char* GetDisplayName() const;
			CvLocalizationAPI int GetPluralityRule() const;
		};

		CvLocalizationAPI String Lookup(const char* szTextKey) const;
		CvLocalizationAPI String Lookup(const char* szTextKey, unsigned int lenTextKey) const;

		CvLocalizationAPI bool TryLookup(const char* szTextKey, unsigned int lenTextKey, String& output) const;

		CvLocalizationAPI String LookupLanguage(const char* szTextKey, unsigned int uiLanguageIndex) const;
		CvLocalizationAPI String LookupLanguage(const char* szTextKey, unsigned int lenTextKey, unsigned int uiLanguageIndex) const;

		CvLocalizationAPI bool TryLookupLanguage(const char* szTextKey, unsigned int lenTextKey, unsigned int uiLanguageIndex, String& output) const;

		CvLocalizationAPI bool HasKey(const char* szTextKey, unsigned int lenTextKey) const;
		CvLocalizationAPI bool HasKey(const char* szTextKey, unsigned int lenTextKey, unsigned int uiLanguageIndex) const;

		CvLocalizationAPI unsigned int GetCurrentLanguageIndex() const;

		CvLocalizationAPI bool SetCurrentLanguage(unsigned int idx);
		CvLocalizationAPI bool SetCurrentLanguage(const char* szLanguageType);

		CvLocalizationAPI size_t GetLanguageCount() const;
		CvLocalizationAPI const char* GetLanguageType(unsigned int idx) const;
		CvLocalizationAPI const char* GetLanguageDisplayName(unsigned int idx) const;
		CvLocalizationAPI const Language& GetLanguage(unsigned int idx) const;

		CvLocalizationAPI void NotifyDatabaseUpdated() const;
	};
}




std::ostream& operator << (std::ostream& out, Localization::String& s);

template<typename T>
Localization::String& operator << (Localization::String& s, const T& val)
{
	s.PushArgument(val);
	return s;
}

template<size_t N>
Localization::String& operator << (Localization::String& s, const char(&val)[N])
{
	s.PushArgument(val, N - 1);
	return s;
}





template<size_t N>
inline size_t Localization::String::GetFormByGender(const char(&szGender)[N], int nOffset) const
{
	return GetFormByGender(szGender, N - 1, nOffset);
}

template<size_t N>
inline size_t Localization::String::GetFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset) const
{
	return GetFormByPluralityAndGender(iPlurality, szGender, N - 1, nOffset);
}

template<size_t N>
bool Localization::String::HasFormByGender(const char(&szGender)[N], int nOffset) const
{
	return HasFormByGender(szGender, N - 1, nOffset);
}

template<size_t N>
bool Localization::String::HasFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset) const
{
	return HasFormByPluralityAndGender(iPlurality, szGender, N - 1, nOffset);
}

template<size_t N>
inline bool Localization::String::SetGender(const char(&szGender)[N])
{
	return SetGender(szGender, N - 1);
}

template<size_t N>
inline bool Localization::String::SetPlurality(const char(&szPlurality)[N])
{
	return SetPlurality(szPlurality, N - 1);
}

template<size_t N>
inline bool Localization::String::PushArgument(const char(&val)[N])
{
	return PushArgument(val, N - 1);
}




#endif

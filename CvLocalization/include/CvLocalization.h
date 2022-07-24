// $Header
//------------------------------------------------------------------------------
#pragma once 
#ifndef _CVLOCALIZATION_H
#define _CVLOCALIZATION_H

/*
	This header represents a public amalgamation of the CvLocalization library.
	It is the combination of public interfaces from many files simplified into
	a single file.
*/

#include <iostream>		//! Needed for ostream helper 
#include <memory>		//! Needed for std::tr1::shared_ptr
#include <string>		//! Needed for std::string
#include <vector>		//! Needed for std::vector

#ifndef CvLocalizationAPI
#define CvLocalizationAPI __declspec(dllimport)
#endif

#include "ILocalizationLogger.h"

namespace Localization
{

	//Public Enumerations
	enum Gender{
		GENDER_NEUTER = 0,
		GENDER_MASCULINE,
		GENDER_FEMININE,
	};

	//Forward Declarations
	class String;
	class StringDictionary;

	class IPluralityRule
	{
	public:

		virtual unsigned int GetMaxPluralForms() const = 0;
		virtual unsigned int GetPluralForm(int nElements) const = 0;
	};

	//TODO: Document Localization::String::Parameter.
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

	//TODO: Document Localization::String::Argument.
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

		//Numeric transforms
		CvLocalizationAPI bool FormatCurrency(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatNumber(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatPercent(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatRoman(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatSpellout(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;

		//String transforms
		CvLocalizationAPI bool FormatUppercase(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatLowercase(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;;
		CvLocalizationAPI bool FormatTitlecase(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool FormatTextkey(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;

		//Choice transforms
		CvLocalizationAPI bool ChoiceFormatGender(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool ChoiceFormatPlurality(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
		CvLocalizationAPI bool ChoiceFormatText(const Localization::ParameterArgument& arg, const std::string& style_args, std::string& outStr) const;
	};

	/*
		Represents a single supported language.
		This class will also contain any implementation details specific to a language.
	*/
	class Language
	{
	public:
		Language(const char* szLanguageType, Localization::IPluralityRule* pkPluralityRule);
		Language(const Language& copy);
		virtual ~Language();

		// General information
		CvLocalizationAPI const char* GetType() const;

		// Plurality
		CvLocalizationAPI unsigned int GetPluralForm(int nElements) const;
		CvLocalizationAPI Localization::IPluralityRule* GetPluralityRule() const;
		CvLocalizationAPI void SetPluralityRule(Localization::IPluralityRule* pkPluralityRule);

		CvLocalizationAPI bool Compare(const char* szString1, size_t lenString1, const char* szString2, size_t lenString2, int& result) const;

		// Language specific transformations
		CvLocalizationAPI const Localization::ParameterTransforms& GetParameterTransforms() const;
	};

	//Initialization/Shutdown
	CvLocalizationAPI bool Initialize(const wchar_t* wszIcuDatFile, Database::Connection& db, const char* szLanguagesTableName, const char* szLanguageType, bool bDisableFallbackLanguageSupport);
	CvLocalizationAPI bool Shutdown();

	//Configuration
	CvLocalizationAPI bool SetCurrentLanguage(const char* szType);
	CvLocalizationAPI const Localization::Language& GetCurrentLanguage();
	//CvLocalizationAPI const Localization::Language& GetLanguage(const char* szType);

	// Dictionary Lookup
	CvLocalizationAPI const Localization::StringDictionary& GetStringDictionary();
	CvLocalizationAPI Localization::String Lookup(const char* textKey);
	//CvLocalizationAPI Localization::String Lookup(const char* textKey, Localization::Language& language);
	CvLocalizationAPI bool ShutdownDictionary();
	CvLocalizationAPI bool InitializeDictionary(Database::Connection& db, const char* szLanguagesTableName, const char* szLanguageType, bool bDisableFallbackLanguageSupport);


	//Logging
	CvLocalizationAPI void SetLogger(ILocalizationLogger* pkLogger);
	CvLocalizationAPI ILocalizationLogger* GetLogger();
	CvLocalizationAPI void LogMessage(const char* szMessage);
	CvLocalizationAPI void LogWarning(const char* szWarning);
	CvLocalizationAPI void LogError(const char* szError);

	//Helper Methods
	CvLocalizationAPI Localization::String ConvertUTCDatetoLocalTZDate(SYSTEMTIME& stUniversalTime, _In_opt_z_ const char* szDTFormat);
	CvLocalizationAPI Localization::String ConvertUTCTimetoLocalTZTime(SYSTEMTIME& stUniversalTime, _In_opt_z_ const char* szTimeFormat);
	CvLocalizationAPI Localization::String ConvertUTCDateTimetoLocalTZDateTime(SYSTEMTIME& stUniversalTime, _In_opt_z_ const char* szDateFormat, _In_opt_z_ const char* szTimeFormat);


	/*
		This class represents a shared immutable raw string.
		Multiple instances of this class access the same buffer
		via a shared pointer.  Each instance also stores the original
		size of the buffer, and substring info.
	*/
	class CvLocalizationAPI SharedString
	{
	public:
		SharedString();
		SharedString(_In_bytecount_(length) const char* szString, size_t length);
		SharedString(const SharedString& copy);

		SharedString& operator = (const SharedString& other);

		//! Obtains the original buffer with a given length.
		const char* GetBuffer(size_t& length) const;

		//! Obtains the buffer of the substring w/ length.
		const char* GetString(size_t& length) const;

		//! Obtains the length of the string buffer.
		size_t GetBufferLength() const;

		//! Obtains the offset of the substring.
		size_t GetOffset() const;

		//! Obtains the length of the substring.
		size_t GetLength() const;

		//! Sets the offset and length for a buffer.
		bool SetOffsetAndLength(size_t offset, size_t length);

	private:
		//Shared pointer to a raw string buffer.
		std::tr1::shared_ptr<char> m_Buffer;

		//Length of string buffer, used for checking per instance offsets and lengths.
		size_t m_BufferLength;

		//! Per instance offset.
		size_t m_offset;

		//! Per instance length.
		size_t m_length;
	};

	//This class is publicly accessed.
	//It's only here so that the compiler can generate a proper size for String.
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

	/* Localization::String (NOT THREAD-SAFE) 
		This class represents a fully localized string composed from a single base string with multiple arguments.
		It has the following features:
			
			*lazy composition of strings (only composes the final string once and when requested)
			 internal string pool for most data.

			*locale-aware text transforms (upper case, lower case, title case)
			*locale-aware numeric transforms (integer, currency, percentage, roman numeral, spell-out)
			*locale-aware plurality selection
			*conditional transforms with optional support for plurality and gender

		A unique syntax is used to handle composition and conditional transforms. For more information about this 
		syntax please view CvLocalization*.doc in the project's base folder.


		This class has the following dependencies:
			*A singleton instance of Localization::System for obtaining the default language as well as locale-specific
			 data.
			
			*A singleton instance of Localization::StringDictionary for internal text-key lookups.
	*/
	class String
	{
	public:
		CvLocalizationAPI String();
		CvLocalizationAPI String(const char* szString);
		CvLocalizationAPI String(const char* szString, unsigned int uiLen);
		CvLocalizationAPI String(const String& other);

		CvLocalizationAPI virtual ~String();


		//FLAGS
		//! Determine if the string is empty.
		CvLocalizationAPI bool IsEmpty() const;


		//OPERATORS
		CvLocalizationAPI String& operator = (const String& other);
		CvLocalizationAPI String& operator = (_In_z_ const char* szBaseString);


		//ACESSORS
		//! Returns a UTF-8 formatted character array of the composed string.
		//! @param bytes [out] the size of the returned array, in bytes.
		//! @return A UTF-8 formatted character array representing the composed string.
		CvLocalizationAPI const char* toUTF8();
		CvLocalizationAPI const char* toUTF8(size_t& bytes, int nForm = 1);

		//! Returns a UTF-8 formatted character array of the base string.
		//! @param bytes [out] the size of the returned array, in bytes.
		//! @return A UTF-8 formatted character array representing the base string.
		CvLocalizationAPI const char* GetBaseString(size_t& bytes) const;


		// LANGUAGE METHODS
		//! Gets the currently assigned language.
		//! By default strings are not explicitly assigned a language and
		//! instead just use the currently assigned language in the system.
		CvLocalizationAPI Localization::Language* GetLanguage() const;

		//! Assigns a specific language to a string.  Everything built from this string
		//! will thus use this language (including string lookup).
		CvLocalizationAPI bool SetLanguage(Localization::Language* Language);


		// ARGUMENT METHODS
	
		//! Obtain the number of currently pushed arguments.
		CvLocalizationAPI size_t GetArgumentCount() const;

		//! Inserts an argument.
		CvLocalizationAPI bool PushArgument(_In_z_ const char* szText);
		CvLocalizationAPI bool PushArgument(_In_bytecount_(length) const char* szText, size_t length);
		CvLocalizationAPI bool PushArgument(const int val);
		CvLocalizationAPI bool PushArgument(const __int64 val);
		CvLocalizationAPI bool PushArgument(const float val);
		CvLocalizationAPI bool PushArgument(const double val);
		CvLocalizationAPI bool PushArgument(const String& strText);
		bool PushArgument(const ParameterArgument& argument);

		template<size_t N>
		bool PushArgument(const char(&val)[N]);

		// FORMS
		//! Obtains the number of forms in a string.
		CvLocalizationAPI size_t GetNumForms() const;

		//! Obtains the Nth form which matches the specified plurality.
		CvLocalizationAPI size_t GetFormByPlurality(int iPlurality, int nOffset = 0) const;

		//! Obtains the Nth form which matches a specific gender or noun trait.
		CvLocalizationAPI size_t GetFormByGender(_In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;

		//! Obtains the Nth form which matches a specific plurality AND gender.
		CvLocalizationAPI size_t GetFormByPluralityAndGender(int iPlurality, _In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;

		template<size_t N>
		size_t GetFormByGender(const char(&szGender)[N], int nOffset = 0) const;

		template<size_t N>
		size_t GetFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset = 0) const;

		//! Determine if the string contains a specific 1-based index form.
		CvLocalizationAPI bool HasForm(int nForm) const;

		//! Determine if the string contains an Nth form which matches the specified plurality.
		CvLocalizationAPI bool HasFormByPlurality(int iPlurality, int nOffset = 0) const;

		//! Determine if the string contains an Nth form which matches a specified gender
		CvLocalizationAPI bool HasFormByGender(_In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;

		//! Determine if the string contains an Nth form which matches a specified plurality and gender.
		CvLocalizationAPI bool HasFormByPluralityAndGender(int iPlurality, _In_bytecount_(lenGender) const char* szGender, size_t lenGender, int nOffset = 0) const;

		template<size_t N>
		bool HasFormByGender(const char(&szGender)[N], int nOffset = 0) const;

		template<size_t N>
		bool HasFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset = 0) const;

		// NOUN TRAITS
		//! Assigns the gender and noun traits to one or many forms of the string.
		CvLocalizationAPI bool SetGender(_In_bytecount_(lenGender) const char* szGender, size_t lenGender);

		//! Assigns a string-based gender to a specific form of the string.
		CvLocalizationAPI bool SetGender(size_t nForm, _In_bytecount_(lenGender) const char* szGender, size_t lenGender);

		//! Assigns a single gender value to all forms of the string.
		CvLocalizationAPI bool SetGender(Gender gender);

		//! Assigns a gender to a specific form.
		CvLocalizationAPI bool SetGender(size_t nForm, Gender gender);

		//! Obtains the gender of a specific form. Returns NULL if invalid params are used.
		CvLocalizationAPI Gender GetGender(size_t nForm = 1) const;

		template<size_t N>
		bool SetGender(const char(&szGender)[N]);

		// PLURALITY
		//! Assigns the plurality to one or many forms of the string.
		//! NOTE: Plurality verification is not performed until string composition.
		CvLocalizationAPI bool SetPlurality(_In_bytecount_(lenPlurality) const char* szPlurality, size_t lenPlurality);

		//! Sets all forms of the string to a specific plurality.
		CvLocalizationAPI bool SetPlurality(size_t iPlurality);

		//! Sets the plural form of a specific string form.
		CvLocalizationAPI bool SetPlurality(size_t nForm, size_t iPlurality);

		//! Obtains the plurality of a specific form.  Returns -1 if invalid params are used.
		CvLocalizationAPI size_t GetPlurality(size_t nForm = 1) const;

		template<size_t N>
		bool SetPlurality(const char(&szPlurality)[N]);

		// BUILDING METHODS
		//! Composes the final string for the specified form.
		//! @return true on success, false on failure.
		CvLocalizationAPI bool Compose(size_t nForm = 1);

		//! Parses the string for parameters and other information.
		CvLocalizationAPI bool Parse(size_t nForm = 1);

		// STATIC CONSTANTS
		CvLocalizationAPI static String Empty;
		
	private:
		SharedString m_BaseString;						//! Used for storing the base string.
		SharedString m_ComposedNullTerminated;			//! Used to store a null terminated composed version of the first form.

		std::vector<ParameterArgument>	m_arguments;	//! Used to store metadata for passed arguments.
		std::vector<SingleFormString> m_Forms;			//! Used to store individual form data.

		Localization::Language* m_pkLanguage;			//! Used to store the explicit language of the string.
	};


	class StringDictionary
	{
	public:

		class Language
		{
		public:

			// Accessors
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



// << operator helpers.
std::ostream& operator << (std::ostream& out, Localization::String& s);
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// inline members
//------------------------------------------------------------------------------
template<size_t N>
inline size_t Localization::String::GetFormByGender(const char(&szGender)[N], int nOffset) const
{
	return GetFormByGender(szGender, N - 1, nOffset);
}
//------------------------------------------------------------------------------
template<size_t N>
inline size_t Localization::String::GetFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset) const
{
	return GetFormByPluralityAndGender(iPlurality, szGender, N - 1, nOffset);
}
//------------------------------------------------------------------------------
template<size_t N>
bool Localization::String::HasFormByGender(const char(&szGender)[N], int nOffset) const
{
	return HasFormByGender(szGender, N - 1, nOffset);
}
//------------------------------------------------------------------------------
template<size_t N>
bool Localization::String::HasFormByPluralityAndGender(int iPlurality, const char(&szGender)[N], int nOffset) const
{
	return HasFormByPluralityAndGender(iPlurality, szGender, N - 1, nOffset);
}
//------------------------------------------------------------------------------
template<size_t N>
inline bool Localization::String::SetGender(const char(&szGender)[N])
{
	return SetGender(szGender, N - 1);
}
//------------------------------------------------------------------------------
template<size_t N>
inline bool Localization::String::SetPlurality(const char(&szPlurality)[N])
{
	return SetPlurality(szPlurality, N - 1);
}
//------------------------------------------------------------------------------
template<size_t N>
inline bool Localization::String::PushArgument(const char(&val)[N])
{
	return PushArgument(val, N - 1);
}
//------------------------------------------------------------------------------



#endif //_CVLOCALIZATION_H
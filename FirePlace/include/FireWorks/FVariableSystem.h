//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//!  \file		FVariableSystem.h
//!  \author	Bart Muzzin - 11/22/2004
//!	 \brief		Implementation of a runtime modifiable set of variables (header).
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FVARIABLESYSTEM_H
#define		FVARIABLESYSTEM_H
#pragma		once

#include "FString.h"

//! Represents the different types of data an FVariable can represent.
enum eVariableType
{
	FVARTYPE_BOOL,		//!< Boolean value.
	FVARTYPE_CHAR,		//!< One byte integer (signed).
	FVARTYPE_UCHAR,		//!< One byte integer (unsigned).
	FVARTYPE_SHORT,		//!< Two byte integer (signed).
	FVARTYPE_USHORT,	//!< Two byte integer (unsigned).
	FVARTYPE_INT,		//!< Four byte integer (signed).
	FVARTYPE_UINT,		//!< Four byte integer (unsigned).
	FVARTYPE_FLOAT,		//!< Four byte floating point number.
	FVARTYPE_DOUBLE,	//!< Eight byte floating point number.
	FVARTYPE_STRING,	//!< String data (uses FString).
	FVARTYPE_WSTRING,	//!< String data (uses FStringW).
	FVARTYPE_COUNT
};
class FDataStream;
FDataStream & operator<<(FDataStream &, const eVariableType &);
FDataStream & operator>>(FDataStream &, eVariableType &);

class FVariable;
class FVariableDebug;

// Determine which type of variable we will use - final_release uses FVariable, all else use FVariableDebug
#ifdef		FINAL_RELEASE
#define FVARTYPE FVariable
#else	//	FINAL_RELEASE
#define FVARTYPE FVariableDebug
#endif	//	FINAL_RELEASE

typedef STDEXT::hash_map< FString, FVARTYPE*, FStringHashTraits > FVariableHash;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CLASS:	FVariableCallback
//
//! \brief Used with FVariableSystem and FVariable as a callback when an FVariable is affected.
//!
//!	Variables that require a callback to perform some function can derive from this class
//! and override the desired methods.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FVariableCallback
{
	public:
        virtual ~FVariableCallback() {};
		virtual void VariableAttached(const FVARTYPE * pkVariable) { };
		virtual void VariableHasChanged(const FVARTYPE * pkVariable) { };
		virtual void VariableRemoved(const FVARTYPE * pkVariable) { };
		virtual bool FinalValueOnly() const { return false; }				// Does this callback want slider values as the slider is moving or only the final value (when the button is unpressed)?
		virtual bool ApplyInitialState() const { return true; }				// Should this callback apply it's initial state when the var is first registered?
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CLASS:	FVariable
//
//! \brief Used with FVariableSystem to create a set of run-time variables.
//!
//! Note that there are no constructors or methods for this class, and all data is public.
//! This is done intentionally to reduce overhead, and this class should rarely be accessed
//! outside of FVariableSystem code. There is a destructor however, because the data contained
//! inside may need to be freed, such as in the case of string data.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
			bool		m_bValue;		//!< Boolean data
			char		m_cValue;		//!< One byte integer (signed) data.
			byte		m_ucValue;		//!< One byte integer (unsigned) data.
			short		m_wValue;		//!< Two byte integer (signed) data.
			word		m_uwValue;		//!< Two byte integer (unsigned) data.
			int			m_iValue;		//!< Four byte integer (signed) data.
			uint		m_uiValue;		//!< Four byte integer (unsigned) data.
			float		m_fValue;		//!< Four byte floating point data.
			double		m_dValue;		//!< Eight byte floating point data.
			char *		m_szValue;		//!< String data.
			wchar_t *	m_wszValue;		//!< Wide string data.
		};

		eVariableType	m_eType;		//!< The type of data contained in this variable.

		virtual void SetCategory( const char * szCategory) { };
		virtual void SetEditType( const char * szEditType) { };
		virtual void SetCallback( FVariableCallback * pkCallback ) { };

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
			return FVARTYPE_INT; // ?
		}

		virtual const char * GetEditType() const
		{
			return GetEditType(m_eType);
		}

		virtual FVariableCallback * GetCallback() const { return NULL; }

		virtual void SetVariableRange( const char * szMin, const char * szMax ) {};
		virtual const char * GetVariableMinimum() const { return NULL; };
		virtual const char * GetVariableMaximum() const { return NULL; };
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CLASS:	FVariableDebug
//
//! \brief Used with FVariableSystem. Like FVariable, but with some extra customizable information.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

		// Ranges are stored as strings - they aren't used internally, only as hints to
		// debugging applications (ie. FireTune tools). If they are NULL, they aren't used
		const char *		m_szMinimum;			
		const char *		m_szMaximum;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CLASS:	FVariableSystem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! \brief Creates a system in which variables can be added/removed/queried/modified at runtime.
//!
//! This should be used when the application is managing variable data obtained from/exposed to an external source.
//! For example, if variables are read from an XML file, and the variable names are not known beforehand, this system
//! can manage them.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FVariableSystem
{
	public:

		// Constructor/Destructor
		FVariableSystem( );
		virtual ~FVariableSystem( );

		void UnInit();

		// Number of variables in the system
		uint GetSize() const;

		// Variable accessors
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

		// Variable additions/modifiers. If a variable does not exist, it will be added.
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

		// Category, Edit type and callback overridding
		bool SetCategoryEditTypeCallback( const char * szVariable, 
			const char * szCategory, const char * szEditType, FVariableCallback * pkCallback );
		bool SetVariableRange( const char * szVariable, const char * szMin, const char * szMax );

		// Variable removal
		bool RemValue( const char * szVariable );

		// Iteration
		std::string GetFirstVariableName( );
		std::string GetNextVariableName( );

		void Read(FDataStream &);
		void Write(FDataStream &) const;

protected:

		// Helper function for setting values
		FVARTYPE * PreSetValue(const char * szVariable);

		FVariableHash				m_mapVariableMap;		//!< Hash map of variable types
		FVariableHash::iterator		m_iVariableIterator;	//!< Current iterator used with GetFirst/NextVariableName


};

FDataStream & operator<<(FDataStream &, const FVariable &);
FDataStream & operator>>(FDataStream &, FVariable &);

// Phasing out using .inl files. VC90 projects will now use a .cpp instead.
#if _MSC_VER <= 1400
//#include "FVariableSystem.inl"
#endif

#endif	//	FVARIABLESYSTEM_H

//---------------------------------------------------------------------------------------
//
//  *****************   CIV 5 World Builder Map   ********************
//
//  FILE:    CvWorldBuilderMapModData.h
//
//  AUTHOR:  Eric Jordan  --  5/13/2010
//
//  PURPOSE:
//		This little monstrosity was created to support the Civ 5 World Builder map format.
//		It's designed to allow modders to inject custom data formats into their map files.
//
//      If you are a modder looking at this then run!  You can access this functionality
//      from the world builder UI and script.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#pragma once
#ifndef CvWorldBuilderMapModData_h
#define CvWorldBuilderMapModData_h

#include "FireWorks\FUncopyable.h"

class CvWorldBuilderMapModData;
class ModType;

class ModDataStringBuffer : FUncopyable
{
	friend CvWorldBuilderMapModData;

public:
	uint AddString(const char *sz);
	_Ret_z_ const char *GetString(uint uiStringID) const;

private:
	ModDataStringBuffer();
	~ModDataStringBuffer();

	void Clear();

	size_t GetSerializedSize() const;
	void Serialize(void *pv) const;
	void Deserialize(const void *pv);

	uint m_uiSize, m_uiReservedSize;
	char *m_szBuffer;
};

class ModEnum
{
	friend ModType;

public:
	ModEnum();
	ModEnum(const ModEnum &kRhs);
	~ModEnum() {}

	void AddMember(const char *szMember);

	uint GetMemberCount() const;
	_Ret_opt_z_ const char *GetMember(uint uiMember) const;

private:
	ModEnum(ModDataStringBuffer &kStringBuffer);

	size_t GetSerializedSize() const;
	void Serialize(void *pv) const;
	void Deserialize(const void *pv);

	ModDataStringBuffer *m_pkStringBuffer;
	FStaticVector<uint, 8, true> m_kMembers;
};

class ModDataEntry
{
	friend CvWorldBuilderMapModData;

public:
	void SetField(uint uiField, bool bValue);
	void SetField(uint uiField, int iValue);
	void SetField(uint uiField, float fValue);
	void SetField(uint uiField, const char *szValue);
	void SetFieldToDefault(uint uiField);

	bool GetFieldAsBool(uint uiField) const;
	int GetFieldAsInt(uint uiField) const;
	float GetFieldAsFloat(uint uiField) const;
	_Ret_z_ const char *GetFieldAsString(uint uiField) const;
	bool IsFieldDefaultValue(uint uiField) const;

private:
	ModDataEntry(ModType &kModType, dword *aFields);

	// There is no definition for this as this class is not supposed to use assignment
	const ModDataEntry &operator=(const ModDataEntry &kRhs);

	ModType &m_kModType;
	dword *m_aFields;
};

class ModType
{
	friend CvWorldBuilderMapModData;
	friend ModDataEntry;

public:
	enum FieldType
	{
		TYPE_INVALID = -1,
		TYPE_BOOL = 0,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_ENUM // Must be last
	};

	void AddField(const char *szName, bool bDefaultValue);
	void AddField(const char *szName, float fDefaultValue);
	void AddField(const char *szName, const char *szDefaultValue);
	ModEnum &AddEnumField(const char *szName, int iDefaultValue);

	uint GetFieldCount() const;

	_Ret_opt_z_ const char *GetFieldName(uint uiField) const;
	FieldType GetFieldType(uint uiField) const;

	bool GetFieldDefaultAsBool(uint uiField) const;
	int GetFieldDefaultAsInt(uint uiField) const;
	float GetFieldDefaultAsFloat(uint uiField) const;
	const char *GetFieldDefaultAsString(uint uiField) const;

	_Ret_opt_ const ModEnum *GetFieldEnumType(uint uiField) const;

private:
	struct Field
	{
		dword dwFieldType;
		dword dwFieldName; // String ID
		dword dwDefault;
	};

	ModType(ModDataStringBuffer &kStringBuffer);
	ModType(const ModType &kRhs);
	~ModType() {}

	// There is no definition for this as this class is not supposed to use assignment
	const ModType &operator=(const ModType &kRhs);

	void Clear();

	size_t GetSerializedSize() const;
	void Serialize(void *pv) const;
	void Deserialize(const void *pv);

	ModDataStringBuffer &m_kStringBuffer;
	FStaticVector<Field, 8, true> m_kFields;
	FStaticVector<ModEnum, 8> m_kEnums;
};

class CvWorldBuilderMapModData : FUncopyable
{
	friend ModType;
	friend ModEnum;

public:
	CvWorldBuilderMapModData();

	void InitDataEntries(uint uiPlotCount);
	void ResetTypes();

	ModType m_kMapModData;
	ModType m_kPlotModData;

	// Yep, this returns by value.  Don't worry editing it still
	// edits the real mod data.  The ModDataEntry is really just
	// a reference to a ModType and a memory location to store fields.
	ModDataEntry GetMapDataEntry();
	ModDataEntry GetPlotDataEntry(uint uiPlot);

	size_t GetSerializedSize() const;
	void Serialize(void *pv) const;
	void Deserialize(const void *pv);

private:
	FFastVector<dword, true> m_kData;
	ModDataStringBuffer m_kStringBuffer;
};

#endif // CvWorldBuilderMapModData_h

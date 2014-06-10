//---------------------------------------------------------------------------------------
//
//  *****************   CIV 5 World Builder Map   ********************
//
//  FILE:    CvWorldBuilderMapTypeGroup.h
//
//  AUTHOR:  Eric Jordan  --  5/24/2010
//
//  PURPOSE:
//		This little monstrosity was created to support the Civ 5 World Builder map format.
//		It's designed to store a group of related type strings.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#pragma once
#ifndef CvWorldBuilderMapTypeGroup_h
#define CvWorldBuilderMapTypeGroup_h

class CvWorldBuilderMapTypeGroup
{
public:
	CvWorldBuilderMapTypeGroup();
	CvWorldBuilderMapTypeGroup(const CvWorldBuilderMapTypeGroup &kRhs);
	~CvWorldBuilderMapTypeGroup();

	const CvWorldBuilderMapTypeGroup &operator=(const CvWorldBuilderMapTypeGroup &kRhs);

	bool Contains(_In_z_ const char *szType) const;

	void Add(_In_z_ const char *szType);
	void Remove(_In_z_ const char *szType);
	void Clear();

	uint GetSerializedSize() const { return m_uiBufferSize; }
	void Serialize(void *p) const;
	void Deserialize(const void *p, uint uiSize);

private:
	char *m_szBuffer;
	uint m_uiBufferSize;
};

#endif // CvWorldBuilderMapTypeGroup_h

//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FUncopyable.h
//
//  AUTHOR:  Eric Jordan  --  10/13/2008
//
//  PURPOSE: Inheriting from FUncopyable will protect a class from being copied.
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2008 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef _FUNCOPYABLE_H_
#define _FUNCOPYABLE_H_
#pragma once

class FUncopyableBase
{
public:
	FUncopyableBase(){}

private:
	// Copy Constructor purposely left undefined
	FUncopyableBase(const FUncopyableBase& kRhs);
	const FUncopyableBase &operator=(const FUncopyableBase& kRhs);
};

class FUncopyable : private FUncopyableBase
{
	/*******************************************************\
	Dear Coder,

		If you got a compiler error here it means that you
	attempted to copy an object whose class inherits from
	FUncopyable.  This means that the class was never intended
	to be copied.

		One common way to end up with unwanted copying of
	objects is inside of a collection such as a vector.
	If the copy operation is coming from a collection then
	try storing the objects by pointer rather than by value.
	This way the pointers will be copied instead of the
	objects themselves.

		Sincerely,
		Eric Jordan
	\*******************************************************/
};

#endif // _FUNCOPYABLE_H_

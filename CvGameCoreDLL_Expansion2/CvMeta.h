#pragma once

#ifndef CVMETA_H
#define CVMETA_H

// Helpers for template-meta-programming

namespace CvMeta
{
	// std::remove_cv
	template<typename T> struct RemoveCV
	{
		typedef T Type;
	};
	template<typename T> struct RemoveCV<const T>
	{
		typedef T Type;
	};
	template<typename T> struct RemoveCV<const volatile T>
	{
		typedef T Type;
	};

	// std::enable_if
	template<bool B, typename T = void> struct EnableIf
	{
	};
	template<typename T> struct EnableIf<true, T>
	{
		typedef T Type;
	};

	// std::is_same
	template<typename T, typename U> struct IsSame
	{
		enum { Value = false };
	};
	template<typename T> struct IsSame<T, T>
	{
		enum { Value = true };
	};

	// std::conditional
	template<bool B, typename T, typename F> struct Conditional
	{
		typedef T Type;
	};
	template<typename T, typename F> struct Conditional<false, T, F>
	{
		typedef F Type;
	};
}

#endif

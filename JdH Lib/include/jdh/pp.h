#pragma once

#include <boost/preprocessor.hpp>

#define __MSVC_VARIADIC_SIZE_HELPER(...)	unused, __VA_ARGS__
#define MSVC_VARIADIC_SIZE(...)    BOOST_PP_DEC( BOOST_PP_VARIADIC_SIZE( __MSVC_VARIADIC_SIZE_HELPER(__VA_ARGS__) ) )

#define MSVC_OVERLOAD(prefix, ...)    BOOST_PP_CAT(prefix, MSVC_VARIADIC_SIZE(__VA_ARGS__))

#define MSVC_VARIADIC_SWITCH(prefix, ...)	BOOST_PP_IF(MSVC_VARIADIC_SIZE(__VA_ARGS__), prefix ## N, prefix ## 0)

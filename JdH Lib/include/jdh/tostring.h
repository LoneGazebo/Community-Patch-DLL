#pragma once

#include <string>

#include "pp.h"


namespace jdh
{
	PCSTR szprintf(PCSTR fmt, ...);

	namespace impl
	{
		std::string& GetTempString();
	}


	inline PCSTR tostring(_In_ PCSTR pStr)
	{
		return pStr;
	}
	inline void append(std::string& buffer, char c)
	{
		buffer += c;
	}
	inline void append(std::string& buffer, _In_ PCSTR pStr)
	{
		buffer += pStr;
	}
	inline void appendAsArg(std::string& buffer, PCSTR pStr)
	{
		if (pStr)
		{
			buffer += '"';
			buffer += pStr;
			buffer += '"';
		}
		else
		{
			buffer += "(null)";
		}
	}


#define TOSTRING_DECL(param_type, param_name)	\
	inline void append(std::string& buffer, param_type param_name); \
	_Ret_z_ inline PCSTR tostring(param_type param_name) { \
		std::string& buffer = impl::GetTempString(); \
		append(buffer, param_name); \
		return buffer.c_str(); \
				} \
	inline void append(std::string& buffer, param_type param_name)

#define TOSTRING_DECL_ARG_WRAPPED(param_type, param_name)	\
	inline void append(std::string& buffer, param_type param_name); \
	_Ret_z_ inline PCSTR tostring(param_type param_name) { \
		std::string& buffer = impl::GetTempString(); \
		append(buffer, param_name); \
		return buffer.c_str(); \
		} \
	inline void appendAsArg(std::string& buffer, param_type param_name) { \
		buffer += '"'; \
		append(buffer, param_name); \
		buffer += '"'; \
		} \
	inline void append(std::string& buffer, param_type param_name)

#define TOSTRING_DECL_TEMPLATE(template_def, param_type, param_name)	\
	template<BOOST_PP_REM template_def> inline void append(std::string& buffer, BOOST_PP_REM param_type param_name); \
	template<BOOST_PP_REM template_def> _Ret_z_ inline PCSTR tostring(BOOST_PP_REM param_type param_name) { \
		std::string& buffer = impl::GetTempString(); \
		append(buffer, param_name); \
		return buffer.c_str(); \
		} \
	template<BOOST_PP_REM template_def> inline void append(std::string& buffer, BOOST_PP_REM param_type param_name)


	TOSTRING_DECL_ARG_WRAPPED(std::string const &, str) {
		buffer += str;
	}
	TOSTRING_DECL(void const* const, ptr) {
		buffer += szprintf("%p", ptr);
	}
	TOSTRING_DECL(double, d) {
		buffer += szprintf("%f", d);
	}
	TOSTRING_DECL(bool, b) {
		buffer += (b ? "true" : "false");
	}
	TOSTRING_DECL(int, i) {
		buffer += szprintf("%i", i);
	}
	TOSTRING_DECL(unsigned int, u) {
		buffer += szprintf("%u", u);
	}
	TOSTRING_DECL(DWORD, u) {
		buffer += szprintf("0x%X", u);
	}
	TOSTRING_DECL(unsigned __int64, u64) {
		buffer += szprintf("%llu", u64);
	}
	TOSTRING_DECL(GUID const &, guid) {
		buffer += szprintf("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	TOSTRING_DECL_TEMPLATE((class T), (const std::list<T>&), aList)
	{
		buffer += '{';
		std::list<T>::const_iterator iter = aList.begin();
		if (iter != aList.end())
		{
			appendAsArg(buffer, *iter);
			for (++iter; iter != aList.end(); ++iter)
			{
				buffer += ',';
				appendAsArg(buffer, *iter);
			}
		}
		buffer += '}';
	}
	TOSTRING_DECL_TEMPLATE((class T), (const std::vector<T>&), aVec)
	{
		buffer += '[';
		std::vector<T>::const_iterator iter = aVec.begin();
		if (iter != aVec.end())
		{
			appendAsArg(buffer, *iter);
			for (++iter; iter != aVec.end(); ++iter)
			{
				buffer += ',';
				appendAsArg(buffer, *iter);
			}
		}
		buffer += ']';
	}
	TOSTRING_DECL_TEMPLATE((), (const std::vector<bool>&), ab) {
		buffer += '[';
		for (std::vector<bool>::const_iterator iter = ab.begin(); iter != ab.end(); ++iter)
		{
			buffer += (*iter ? 't' : 'f');
		}
		buffer += ']';
	}
	template<class T, size_t N>
	struct ArrayTypes
	{
		typedef T Arr[N];
		typedef T const CArr[N];
		typedef typename Arr& Ref;
		typedef typename CArr& CRef;
	};
	TOSTRING_DECL_TEMPLATE((size_t N), (typename ArrayTypes<bool, N>::Ref), ab) {
		buffer += '[';
		for (size_t i = 0; i < N; ++i)
		{
			buffer += (ab[i] ? 't' : 'f');
		}
		buffer += ']';
	}



	template<class T>
	void appendAsArg(std::string& buffer, const T& a) { append(buffer, a); }

#define DEFINE_APPEND(z, N, _) \
	template<BOOST_PP_ENUM_PARAMS_Z(z, N, class T)> \
	inline void append(std::string& buffer, BOOST_PP_ENUM_BINARY_PARAMS_Z(z, N, const T, &a)) { \
		BOOST_PP_IF(N, append(buffer, BOOST_PP_ENUM_PARAMS_Z(z, BOOST_PP_DEC(N), a)), ); \
		append(buffer, BOOST_PP_CAT(a, BOOST_PP_DEC(N))); \
		} \
	template<BOOST_PP_ENUM_PARAMS_Z(z, N, class T)> \
	inline PCSTR tostring(BOOST_PP_ENUM_BINARY_PARAMS_Z(z, N, const T, &a)) { \
		std::string& buffer = impl::GetTempString(); \
		append(buffer, BOOST_PP_ENUM_PARAMS_Z(z, N, a)); \
		return buffer.c_str(); \
		} \
	template<BOOST_PP_ENUM_PARAMS_Z(z, N, class T)> \
	inline void appendAsArg(std::string& buffer, BOOST_PP_ENUM_BINARY_PARAMS_Z(z, N, const T, &a)) { \
		appendAsArg(buffer, BOOST_PP_ENUM_PARAMS_Z(z, BOOST_PP_DEC(N), a)); \
		buffer += ", "; \
		appendAsArg(buffer, BOOST_PP_CAT(a, BOOST_PP_DEC(N))); \
		} \
	template<BOOST_PP_ENUM_PARAMS_Z(z, N, class T)> \
	inline PCSTR tostringAsArg(BOOST_PP_ENUM_BINARY_PARAMS_Z(z, N, const T, &a)) { \
		std::string& buffer = impl::GetTempString(); \
		appendAsArg(buffer, BOOST_PP_ENUM_PARAMS_Z(z, N, a)); \
		return buffer.c_str(); \
		}

	BOOST_PP_REPEAT_FROM_TO(2, 10, DEFINE_APPEND, nil);

#undef DEFINE_APPEND
}

namespace jdh
{
#if 0
	TOSTRING_DECL(const ICvEnumerator*, pList)
	{

	}
#endif
}

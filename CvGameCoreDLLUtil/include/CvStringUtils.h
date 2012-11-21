//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvstringUtils.h
//!  \brief     Public header common string functions
//!
//!		This header includes utility functions for changing string encoding.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

namespace CvStringUtils
{
	std::wstring FromACPToUTF16(const std::string& strCPString);
	std::wstring FromUTF8ToUTF16(const std::string& strUTF8String);

	std::string FromUTF16ToACP(const std::wstring& strUTF16String);
	std::string FromUTF16ToUTF8(const std::wstring& strUTF16String);
}

// $Header
//------------------------------------------------------------------------------
#pragma once 

/*! 
	The following are highly optimized methods for decoding UTF-8 encoded bytes in an array.
*/
namespace Localization{
	namespace UTF8{

	//! Determine if a UTF-8 formatted character is a single byte.
	template<typename C>
	bool IsSingleByte(C c)
	{
		return ((c & 0x80) ==0);
	}

	template<typename C>
	bool IsTrailingByte(C c)
	{
        //        11000000    10------
		return ((c & 0xC0) == 0x80);
	}

	static unsigned char CountTrailBytes(unsigned char leadByte)
	{
		//Data table used to quickly count trailing bytes
		static const unsigned char utf8_countTrailBytes[256]= {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3,
			3, 3, 3,    /* illegal in Unicode */
			4, 4, 4, 4, /* illegal in Unicode */
			5, 5,       /* illegal in Unicode */
			0, 0        /* illegal bytes 0xfe and 0xff */
		};

		return utf8_countTrailBytes[leadByte];
	}

	//! Returns the next UTF-8 character c in sequence s at index i.
	//! NOTE: This is unsafe because it does not perform any buffer overrun checking.
	template<typename S, typename C>
	unsigned char NextUnsafe(const S* s, unsigned int i, C& c)
	{
		c = (unsigned char) s[i++];
		if((unsigned char)(c - 0xc0) < 0x35)
		{
			const unsigned char __count = CountTrailBytes(c);

			c &= (1 << (6 - __count)) - 1;	//mask lead byte

			switch(__count)
			{
			/* each following branch falls through to the next one */
			case 3:
					c = (c << 6)| (s[i++] & 0x3f);
			case 2:
					c = (c << 6)| (s[i++] & 0x3f);
			case 1:
					c = (c << 6)|( s[i++] & 0x3f);
				/* no other branches to optimize switch() */
				break;
			}

			return __count;
		}

		return 0;
	}

}}	//namespace Localization::UTF8



#pragma once 




namespace Localization{
	namespace UTF8{


	template<typename C>
	bool IsSingleByte(C c)
	{
		return ((c & 0x80) ==0);
	}

	template<typename C>
	bool IsTrailingByte(C c)
	{

		return ((c & 0xC0) == 0x80);
	}

	static unsigned char CountTrailBytes(unsigned char leadByte)
	{

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
			3, 3, 3,
			4, 4, 4, 4,
			5, 5,
			0, 0
		};

		return utf8_countTrailBytes[leadByte];
	}



	template<typename S, typename C>
	unsigned char NextUnsafe(const S* s, unsigned int i, C& c)
	{
		c = (unsigned char) s[i++];
		if((unsigned char)(c - 0xc0) < 0x35)
		{
			const unsigned char __count = CountTrailBytes(c);

			c &= (1 << (6 - __count)) - 1;

			switch(__count)
			{

			case 3:
					c = (c << 6)| (s[i++] & 0x3f);
			case 2:
					c = (c << 6)| (s[i++] & 0x3f);
			case 1:
					c = (c << 6)|( s[i++] & 0x3f);

				break;
			}

			return __count;
		}

		return 0;
	}

}}

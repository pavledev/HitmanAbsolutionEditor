#pragma once

class Color
{
public:
	Color() = default;
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	explicit Color(unsigned int color);

	union
	{
		struct
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
		};

		unsigned int bits;
	};
};

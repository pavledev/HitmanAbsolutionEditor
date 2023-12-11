#include "Math/Color.h"

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : b(b), g(g), r(r), a(a)
{
}

Color::Color(unsigned int color)
{
	bits = color;
}

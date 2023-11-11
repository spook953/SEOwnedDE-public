#pragma once

#include <string>
#include <sstream>

struct Color_t
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	std::string toHexStr();
	std::wstring toHexStrW();
};

namespace ColorUtils
{
	inline float ToFloat(unsigned char x)
	{
		return static_cast<float>(x) / 255.0f;
	}

	inline unsigned long ToDWORD(Color_t x)
	{
		return static_cast<unsigned long>(((x.r & 0xFF) << 24) | ((x.g & 0xFF) << 16) | ((x.b & 0xFF) << 8) | (x.a & 0xFF));
	}

	Color_t HSLToRGB(float h, float s, float l);
	Color_t Mult(Color_t clr, float mult);
}
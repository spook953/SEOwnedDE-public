#include "Color.h"

#include <format>

Color_t ColorUtils::HSLToRGB(float h, float s, float l)
{
	float q = 0.0f;

	if (l < 0.5f)
		q = l * (s + 1.0f);

	else q = l + s - (l * s);

	float p = 2.0f * l - q;

	float rgb[3] = {};

	rgb[0] = h + (1.0f / 3.0f);
	rgb[1] = h;
	rgb[2] = h - (1.0f / 3.0f);

	for (int i = 0; i < 3; ++i)
	{
		if (rgb[i] < 0.0f)
			rgb[i] += 1.0f;

		if (rgb[i] > 1.0f)
			rgb[i] -= 1.0f;

		if (rgb[i] < (1.0f / 6.0f))
			rgb[i] = p + ((q - p) * 6.0f * rgb[i]);

		else if (rgb[i] < 0.5f)
			rgb[i] = q;

		else if (rgb[i] < (2.0f / 3.0f))
			rgb[i] = p + ((q - p) * 6.0f * ((2.0f / 3.0f) - rgb[i]));

		else
			rgb[i] = p;
	}

	return
	{
		static_cast<unsigned char>(rgb[0] * 255.0f),
		static_cast<unsigned char>(rgb[1] * 255.0f),
		static_cast<unsigned char>(rgb[2] * 255.0f),
		static_cast<unsigned char>(255)
	};
}

Color_t ColorUtils::Mult(Color_t clr, float mult)
{
	float r = static_cast<float>(clr.r) * mult;
	float g = static_cast<float>(clr.g) * mult;
	float b = static_cast<float>(clr.b) * mult;

	return { static_cast<unsigned char>(r),  static_cast<unsigned char>(g),  static_cast<unsigned char>(b), clr.a };
}

std::string Color_t::toHexStr()
{
	std::string rs{ r < 16 ? std::format("0{:X}", r) : std::format("{:X}", r) };
	std::string rg{ g < 16 ? std::format("0{:X}", g) : std::format("{:X}", g) };
	std::string rb{ b < 16 ? std::format("0{:X}", b) : std::format("{:X}", b) };
	std::string ra{ a < 16 ? std::format("0{:X}", a) : std::format("{:X}", a) };

	return rs + rg + rb + ra;
}

std::wstring Color_t::toHexStrW()
{
	std::wstring rs{ r < 16 ? std::format(L"0{:X}", r) : std::format(L"{:X}", r) };
	std::wstring rg{ g < 16 ? std::format(L"0{:X}", g) : std::format(L"{:X}", g) };
	std::wstring rb{ b < 16 ? std::format(L"0{:X}", b) : std::format(L"{:X}", b) };
	std::wstring ra{ a < 16 ? std::format(L"0{:X}", a) : std::format(L"{:X}", a) };

	return rs + rg + rb + ra;
}
#pragma once

namespace character
{
	template<typename type>
	constexpr bool isUpper(const type character)
	{
		return (character >= static_cast<const type>(65) && character <= static_cast<const type>(90));
	}

	template<typename type>
	constexpr type toLower(const type character)
	{
		if (isUpper(character))
		{
			return character + static_cast<const type>(32);
		}

		return character;
	}

	template<typename type>
	constexpr bool isTerminator(const type character)
	{
		return (character == static_cast<const type>(0));
	}

	template<typename type>
	constexpr bool isQuestion(const type character)
	{
		return (character == static_cast<const type>(63));
	}

	template<typename type>
	constexpr size_t getLength(const type *const data)
	{
		size_t length{ 0 };

		while (true)
		{
			if (isTerminator(data[length]))
			{
				break;
			}

			length++;
		}

		return length;
	}
}

namespace hash
{
	using hash_t = unsigned int;

	constexpr auto hash_prime = 0x1000193u;
	constexpr auto hash_basis = 0x811C9DC5u;

	template<typename type>
	constexpr hash_t hash_compute(hash_t hash_basis, const type *data, size_t size, bool ignore_case)
	{
		if (size == 0)
		{
			return hash_basis;
		}

		const auto element = static_cast<hash_t>(ignore_case ? character::toLower(data[0]) : data[0]);

		return hash_compute((hash_basis ^ element) * hash_prime, data + 1, size - 1, ignore_case);
	}

	template<typename type>
	constexpr hash_t fnv1a32_hash(const type *const data, size_t size, bool ignore_case)
	{
		return hash_compute(hash_basis, data, size, ignore_case);
	}

	constexpr hash_t fnv1a32_hash(const char *const data, bool ignore_case)
	{
		return fnv1a32_hash(data, character::getLength(data), ignore_case);
	}

	constexpr hash_t fnv1a32_hash(const wchar_t *const data, bool ignore_case)
	{
		return fnv1a32_hash(data, character::getLength(data), ignore_case);
	}
}

#define HASH_CT(data)\
[&]() \
{ \
	constexpr auto hash = hash::fnv1a32_hash(data, true);\
	return hash;\
}()

#define HASH_RT(data) hash::fnv1a32_hash(data, true)
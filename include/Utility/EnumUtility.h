#pragma once

#include <type_traits>

class EnumUtility
{
public:
	template<typename Enum>
	static constexpr bool HasFlag(Enum flag, Enum value)
	{
		using T = std::underlying_type_t<Enum>;

		return static_cast<T>(flag) & static_cast<T>(value);
	}
};

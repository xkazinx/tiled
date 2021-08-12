#pragma once
#include "App.h"

class App;
class Random;

#define DEBUG_MODE

namespace global
{
	constexpr char kAppName[] = "Tileship";

	extern App* g_app;
	extern unique_ptr<Random> g_rand;

	namespace app
	{
		extern void Initialize();
		extern void Deinitialize();
	}

	class Guard {
	public:
		template<typename T>
		Guard(T func)
			: _reset(func) {

		}
		~Guard() {
			_reset();
		}

		std::function<void()> _reset;
	};

	template<typename T, typename = enable_if_t<
		is_integral_v<T> || is_enum_v<T>
		>> constexpr auto ncast(T value)
	{
		if constexpr (sizeof(T) != 1)
		{
			if constexpr (is_enum_v<T>)
			{
				return static_cast<std::underlying_type_t<T>>(value);
			}
			else
			{
				return value;
			}
		}
		else
		{
			if constexpr (is_enum_v<T>)
			{
				using utype = std::underlying_type_t<T>;
				if constexpr (is_unsigned_v<utype>)
				{
					return static_cast<u16>(value);
				}
				else if constexpr (is_signed_v<utype>)
				{
					return static_cast<i16>(value);
				}
			}
			else
			{
				if constexpr (is_signed_v<T>)
				{
					return static_cast<i16>(value);
				}
				else if constexpr (is_unsigned_v<T> || is_same_v<bool, T>)
				{
					return static_cast<u16>(value);
				}
			}
		}
	}
}

namespace font {
	enum class ID : u8 {
		Arial,
		Max,
	};

	using FontNames = const vector<string>;
	extern FontNames kNames;
}
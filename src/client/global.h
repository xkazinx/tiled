#pragma once
#include "App.h"

class App;
class Random;
class SceneMgr;
class Socket;

#define DEBUG_MODE

namespace Global
{
	constexpr char kAppName[] = "Tileship";

	extern App* g_app;
	extern unique_ptr<Random> g_rand;
	extern unique_ptr<SceneMgr> g_scene;
	extern unique_ptr<Socket> g_sock;

	extern void Initialize();
	extern void Deinitialize();

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

	namespace scene
	{
		enum ID : u8
		{
			Login,
			Main,
			Lobby,
			Game,
			Result,
		};
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

using SceneID = Global::scene::ID;
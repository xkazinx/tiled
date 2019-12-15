#pragma once
#include "App.h"

class App;
class SceneMgr;
class Camera;
class TextMgr;
class Random;
class Background;
class Button;
class Player;
class Object;

#define DEBUG_MODE

namespace global
{
	constexpr char kAppName[] = "Yip";

	extern unique_ptr<App> g_app;
	extern unique_ptr<SceneMgr> g_scene;
	extern unique_ptr<Camera> g_camera;
	extern unique_ptr<TextMgr> g_text;
	extern unique_ptr<Random> g_rand;

	extern RenderWindow & GetWindow();

	namespace app
	{
		template<typename Class, typename Ptr>
		static auto & Create(Ptr && ptr) {
			ptr = mk<Class>();
			return ptr;
		}

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

	extern unique_ptr<Guard> WindowGuard(bool start = false);;
	extern Vector2f DeltaVelocity(const Vector2f & velocity, const sf::Time & delta);
}

namespace scene {
	enum class ID : u8 {
		Main,
		Game,
		Max,
	};


}

namespace font {
	enum class ID : u8 {
		Arial,
		Max,
	};

	using FontNames = const vector<string>;
	extern FontNames kNames;
}

template<class T>
class TargetStatesBase {
public:
	sf::RenderTarget & target;
	T states;

	auto & operator ()() {
		return target;
	}
};

using TargetStates = TargetStatesBase<sf::RenderStates>;
using TargetStatesRef = TargetStatesBase<sf::RenderStates&>;

enum class ObjectID : u8 {

	None,
	Player,
	Projectile,
	Enemy,
	Star,
	Background,
	Max,
};

namespace resolution 
{
	extern sf::Vector2f GetCenter(const Vector2f & base);
}
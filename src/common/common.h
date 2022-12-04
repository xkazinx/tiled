#pragma once

#define KAZIN_WINDOWS
#define KAZIN_NET
#define DEBUG_PACKET
#include <kazin/kazin.h>

using namespace kazin::global;
using Socket = kazin::Socket;
using InPacket = kazin::InPacket;
using Auth = kazin::Auth;

namespace common
{
constexpr u16 kPort = 321;
constexpr const char * kHost = "localhost";

template <typename T>
constexpr underlying_type_t<T> to_underlying(T value) noexcept
{
	return underlying_type_t<T>(value);
}

using RenderPointValue = i16;
using TilePointValue = u16;
using WorldPointValue = u16;

#ifdef CLIENT
using RenderPoint = sf::Vector2<RenderPointValue>;
using TilePoint = sf::Vector2<TilePointValue>;
using WorldPoint = sf::Vector2<WorldPointValue>;
using MovePoint = sf::Vector2f;
#endif

namespace tile
{
	enum class Type : u8
	{
		Movable,
		Blocked, // Can't move
		Water, 
		Max
	};

	static constexpr i16
		kSize = 32,
		kHalfSize = kSize / 2;
}

namespace player
{
	using Idx = u16;
	constexpr Idx
		kNullIdx = 0,
		kMax = 512;

	constexpr u8
		kUsernameSize = 32,
		kPasswordSize = 32;

	class Base
	{
	public:

	};
}

using PlayerIdx = player::Idx;



enum class FormatType : u8
{
	None,
	bool_Switch,	  // On, Off
	bool_State,		  // true, false
	bool_Status,	  // Enabled, Disabled
	bool_Result,	  // Success, Failure
	bool_Affirmation, // Yes, No
};

template<typename T>
inline string Format(T v, FormatType type = FormatType::None)
{
	if constexpr (is_enum_v<T>)
	{
		return format("({}:{})", to_underlying(v), enum_name(v));
	}
	else if constexpr (is_same_v<bool, T>)
	{
		using enum FormatType;
		switch (type)
		{
		case bool_Switch:		return format("{}", v ? "On" : "Off");
		case bool_State:		return format("{}", v ? "True" : "False");
		case bool_Status:		return format("{}", v ? "Enabled" : "Disabled");
		case bool_Result:		return format("{}", v ? "Success" : "Failure");
		case bool_Affirmation:	return format("{}", v ? "Yes" : "No");
		default:				return format("{}", v);
		}
	}
	else if constexpr (is_same_v<ENetPeer*, T>)
	{
		return format("({}:{}:PlayerIdx({}))", kazin::Socket::FetchIP(v), v->address.port, 
			v->data ? *reinterpret_cast<PlayerIdx*>(v->data) : player::kNullIdx);
	}
	else if constexpr (is_same_v<kazin::Auth, T>)
	{
		return format("({}:{})", v._user.c_str(), v._pass.c_str());
	}
	else
	{
		return to_string(v);
	}
}
}

using namespace common;
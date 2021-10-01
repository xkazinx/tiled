#pragma once

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;
using std::to_string;
using namespace std::string_literals;

#include <type_traits>
using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;

using u64 = std::uint64_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;

using std::size_t;
using szt = size_t;

#include <memory>
using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

#include <vector>
using std::vector;

#include <array>
using std::array;

#include <map>
using std::map;

#include <thread>
namespace this_thread = std::this_thread;

#include <mutex>
using std::mutex;
using unique_mutex = std::unique_lock<mutex>;

#include <chrono>
namespace chrono = std::chrono;

#include <format>
using std::format;

#include <functional>
using std::function;

#include <cassert>

using std::copy;
using std::copy_n;
using std::move;
using std::forward;

using std::enable_if_t;
using std::is_integral_v;
using std::is_enum_v;
using std::is_unsigned_v;
using std::is_signed_v;
using std::is_same_v;
using std::underlying_type_t;
using std::is_base_of_v;
using std::is_pointer_v;

#include <magic_enum.h>
using namespace magic_enum;

#include <console/log.h>

#define DEBUG_PACKET

#include <netops.h>
#include <random.h>
#include <packet.h>
#include <socket.h>
#include <broadcaster.h>
#include <auth.h>

namespace common
{
constexpr u16 kPort = 321;
constexpr const char * kHost = "localhost";

template <typename T>
constexpr underlying_type_t<T> to_underlying(T value) noexcept
{
	return underlying_type_t<T>(value);
}

namespace tile
{
	enum class Type : u8
	{
		Grass,
		Water,
		Rock,
		Max
	};

	using LineSize = u8;
	static constexpr float
		kSize = 32.f,
		kHalfSize = kSize / 2.f;

#ifdef CLIENT
	using ColorArray = const array<sf::Color, (szt)Type::Max>;
	extern ColorArray kColors;
#endif

}

namespace player
{
	using Idx = u16;
	constexpr Idx
		kNullIdx = 0,
		kMax = 512;

	constexpr u8
		kNameSize = 32,
		kPasswordSize = 32;
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
		return format("({}:{}:PlayerIdx({}))", Socket::FetchIP(v), v->address.port, 
			v->data ? *reinterpret_cast<PlayerIdx*>(v->data) : player::kNullIdx);
	}
	else if constexpr (is_same_v<Auth, T>)
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
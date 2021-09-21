#pragma once

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;

#include <type_traits>
using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;

using u64 = std::uint64_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;

using szt = std::size_t;

#include <memory>
using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

#include <vector>
using std::vector;

#include <array>
using std::array;

#include <thread>
namespace this_thread = std::this_thread;

#include <chrono>
namespace chrono = std::chrono;

using std::enable_if_t;
using std::is_integral_v;
using std::is_enum_v;
using std::is_unsigned_v;
using std::is_signed_v;
using std::is_same_v;
using std::underlying_type_t;
using std::is_base_of_v;
using std::is_pointer_v;
using std::forward;

#include <functional>

#include <cassert>

// common filter
#include <netops.h>
#include <random.h>
#include <packet.h>
#include <socket.h>
#include <broadcaster.h>

#define DEBUG_PACKET

namespace common
{
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
	static constexpr float kSize = 32.f;
	static constexpr float kHalfSize = kSize / 2.f;

#ifdef CLIENT
	using ColorArray = const array<sf::Color, (szt)Type::Max>;
	extern ColorArray kColors;
#endif

}

namespace player
{
	using Idx = u16;
	constexpr Idx kNullIdx = 0;
	constexpr Idx kMax = 512;
}

using PlayerIdx = player::Idx;
}

using namespace common;
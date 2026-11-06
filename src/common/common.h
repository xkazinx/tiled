#pragma once
// What the client and the server share: kazilib (std names, startup flags, the
// log, magic_enum, enet and its packet layer, the broadcaster base), the points,
// tiles, directions, maps, units and players' common parts, and Format.
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

// winsock2 before anything includes <windows.h> (initflags.h does), or windows.h
// brings the old winsock.h and enet's winsock2.h redefines it all. Linux (the
// server) and the browser (the client, Emscripten) have neither.
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#endif

#include <cstddef>
#include <cstdint>
using u64 = std::uint64_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;
using i8 = std::int8_t;
using szt = std::size_t;

// kazilib: std names without std::, startup flags, the log, enums, enet and packets
#include <stdnames.h>
#include <initflags.h>
#include <under.h>
#include <log.h>
#include <magic_enum.h>
#include <net/packet.h>
#include <net/socket.h>
#include <net/broadcaster.h>
#include <time/thread_time.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <format>
#include <map>
#include <random>
#include <thread>
#include <utility>

#include "point.h"

using std::format;
using std::is_enum_v;
using std::is_signed_v;
using std::is_unsigned_v;
using std::is_integral_v;
using lib::OPkt;
using lib::OPktPtr;
using lib::IPkt;
using lib::PktSz;
using lib::Socket;
using lib::SocketHandler;
using magic_enum::enum_name;
using magic_enum::enum_names;
using magic_enum::enum_values;
using magic_enum::enum_contains;
namespace this_thread = std::this_thread;

#include "netops.h"

namespace common
{
// The game server: ENet on UDP 7031 (native clients), browsers through its WebSocket
// gateway on 7030 (net/ws_gateway.h). Launch flags ip= / port= / ws= or init.json.
constexpr u16 kPort = 7031;
constexpr u16 kWsPort = 7030;
constexpr const char * kHost = "127.0.0.1";
constexpr const char * kAppName = "Tiled";

inline string ServerIp() { return lib::init::Value("ip", kHost); }
inline u16 ServerPort() { return (u16)lib::init::Number("port", kPort); }

// The per-packet traces (the old PrintDebug): launch flag `debug`
inline bool DebugOn()
{
	static const bool on = lib::init::Flag("debug", false);
	return on;
}
#define DebugLog(...) do { if (::common::DebugOn()) Info(__VA_ARGS__); } while (0)

// The case of a broadcaster's switch: Op::Name runs NetOp_Name()
#define OP(name) case Op::name: NetOp_##name(); break

inline long long ProcessId()
{
#ifdef _WIN32
	return (long long)::_getpid();
#else
	return (long long)::getpid();
#endif
}

inline std::mt19937 & Rng()
{
	static std::mt19937 rng{ std::random_device{}() };
	return rng;
}

// True with probability p (0..1)
inline bool Probability(const double p)
{
	return std::bernoulli_distribution(p)(Rng());
}

// A login: the account's name and password (c->s Login / CreateAccount)
struct Auth : lib::NetItem
{
	string _user, _pass;

	Auth() = default;
	Auth(string user, string pass) : _user(std::move(user)), _pass(std::move(pass)) {}
	explicit Auth(IPkt * const in) { in->Read(_user, _pass); }

	_serialize(_user, _pass)

	bool IsValid() const;
};

using RenderPointValue = i16;
using TilePointValue = i16;
using WorldPointValue = i16;

using RenderPoint = Point<RenderPointValue>;
using TilePoint = Point<TilePointValue>;
using WorldPoint = Point<WorldPointValue>;
using MovePoint = Point<float>;

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

using TileType = tile::Type;

struct Direction
{
	enum ID : u8
	{
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest,
		None = u8(-1),
		Max = NorthWest + 1,
		Min = North,
	};

	Direction(const ID & id) : _id(id) {}
	Direction() : Direction(ID::None) {}

	bool operator==(const ID & id) const
	{
		return _id == id;
	}

	bool operator==(const Direction & dir) const
	{
		return operator==(dir._id);
	}

	bool operator!=(const ID & id) const
	{
		return !(operator==(id));
	}

	ID _id;

	static Direction GetNext(const TilePoint & src, const TilePoint & dst)
	{
		auto point = src - dst;

		if (point.x == 0 && point.y == 0)
			return ID::None;

		if (point.x == 0)
		{
			if (point.y > 0)
				return ID::North;

			if (point.y < 0)
				return ID::South;
		}

		if (point.y == 0)
		{
			if (point.x > 0)
				return ID::West;

			if (point.x < 0)
				return ID::East;
		}

		if (point.x > 0 && point.y > 0)
			return ID::NorthWest;

		if (point.x < 0 && point.y > 0)
			return ID::NorthEast;

		if (point.x > 0 && point.y < 0)
			return ID::SouthWest;

		if (point.x < 0 && point.y < 0)
			return ID::SouthEast;

		return ID::None;
	}

	auto SetNext(const TilePoint & src, const TilePoint & dst)
	{
		return (*this = GetNext(src, dst));
	}

	static Point<i8> GetDelta(ID id)
	{
		static const Point<i8> list[] =
		{
			{  0, -1 },
			{  1, -1 },
			{  1,  0 },
			{  1,  1 },
			{  0,  1 },
			{ -1,  1 },
			{ -1,  0 },
			{ -1, -1 }
		};

		// None (and anything past NorthWest) moves nowhere
		return id < ID::Max ? list[id] : Point<i8>{};
	}

	auto GetDelta() const
	{
		return GetDelta(_id);
	}

	template <typename U>
	void Move(U & pos) const
	{
		pos += GetDelta().template As<typename U::Value>();
	}

	static Direction GetContrary(ID id)
	{
		switch (id)
		{
		case ID::North:		return ID::South;
		case ID::NorthEast:	return ID::SouthWest;
		case ID::East:		return ID::West;
		case ID::SouthEast:	return ID::NorthWest;
		case ID::South:		return ID::North;
		case ID::SouthWest:	return ID::NorthEast;
		case ID::West:		return ID::East;
		case ID::NorthWest:	return ID::SouthEast;
		default:			break;
		}

		return ID::None;
	}

	auto GetContrary() const
	{
		return GetContrary(_id);
	}

	bool IsValid() const
	{
		return _id < ID::Max;
	}
};

namespace map
{
	static constexpr RenderPoint
		Resolution =
	{
		1024, 768
	},
		VisibleTiles =
	{
		Resolution / tile::kSize
	}, VisibleTilesRange =
	{
		VisibleTiles / (RenderPointValue)2
	};

	enum ID : u8
	{
		Battlefield,

		None = u8(-1)
	};

	template <typename Tile>
	struct Base
	{
		ID _id = ID::None;

		TilePoint _size;

		vector<unique_ptr<Tile>> _tiles;

		Tile * GetTile(TilePoint pt)
		{
			return _tiles[pt.y + pt.x * _size.y].get();
		}

		bool IsValidPoint(TilePoint pt) const
		{
			return pt.x > 0 && pt.y > 0 && pt.x < _size.x && pt.y < _size.y;
		}

		bool IsMovable([[maybe_unused]] TilePoint pt) const
		{
			if (pt.x < VisibleTilesRange.x || pt.y < VisibleTilesRange.y ||
				pt.x + VisibleTilesRange.x >= _size.x || pt.y + VisibleTilesRange.y >= _size.y)
			{
				return false;
			}

			// #todo read from tile
			return true;
		}
	};
}

using MapID = map::ID;

namespace unit
{
	using Idx = u16;
	constexpr Idx
		kNullIdx = 0;

	enum Action : u8
	{
		Stop,
		Move,
		None = u8(-1),
	};

	struct Base : lib::NetItem
	{
		using Idx = unit::Idx;

		string _name;
		TilePoint _pt;
		Idx _idx = kNullIdx;
		Action _action = Action::None;
		Direction _dir = Direction::South;

		_serialize(_name, _pt, _idx, _action, _dir)

		bool IsOnSight(const unit::Base * base) const
		{
			// #todo check if it's >= or >
			return
				base->_pt.x >= _pt.x - map::VisibleTilesRange.x &&
				base->_pt.x <= _pt.x + map::VisibleTilesRange.x &&
				base->_pt.y >= _pt.y - map::VisibleTilesRange.y &&
				base->_pt.y <= _pt.y + map::VisibleTilesRange.y;
		}

		bool Is(const Base * base) const
		{
			return _idx == base->_idx;
		}

#ifdef CLIENT
		bool IsSelf() const;
#endif
	};
}

namespace player
{
	using Idx = unit::Idx;
	constexpr Idx
		kNullIdx = unit::kNullIdx,
		kMax = 512;

	constexpr u8
		kUsernameSize = 32,
		kPasswordSize = 32;
}

namespace npc
{
	using Idx = unit::Idx;
	constexpr Idx
		kNullIdx = unit::kNullIdx;
}

using PlayerIdx = player::Idx;

inline bool Auth::IsValid() const
{
	if (_user.empty() || _user.size() > player::kUsernameSize || _pass.empty() || _pass.size() > player::kPasswordSize)
		return false;

	return std::all_of(_user.begin(), _user.end(), [](const char c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-';
	});
}

enum class FormatType : u8
{
	None,
	bool_Switch,	  // On, Off
	bool_State,		  // true, false
	bool_Status,	  // Enabled, Disabled
	bool_Result,	  // Success, Failure
	bool_Affirmation, // Yes, No
};

template <typename T>
inline string Format(T v, FormatType type = FormatType::None)
{
	if constexpr (is_enum_v<T>)
	{
		return format("({}:{})", (long long)std::to_underlying(v), enum_name(v));
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
	else if constexpr (is_same_v<ENetPeer *, T>)
	{
		char ip[64] = {};
		enet_address_get_host_ip(&v->address, ip, sizeof(ip));
		return format("({}:{}:PlayerIdx({}))", ip, v->address.port,
			v->data ? *reinterpret_cast<PlayerIdx *>(v->data) : player::kNullIdx);
	}
	else if constexpr (is_same_v<Auth, T>)
	{
		// The password never printed
		return format("({}:{} bytes)", v._user, v._pass.size());
	}
	else if constexpr (
		is_same_v<TilePoint, T> ||
		is_same_v<WorldPoint, T> ||
		is_same_v<MovePoint, T> ||
		is_same_v<RenderPoint, T>)
	{
		return format("({}:{})", v.x, v.y);
	}
	else
	{
		return to_string(v);
	}
}
}

using namespace common;

#pragma once

#define KAZIN_WINDOWS
#define KAZIN_NET
#define DEBUG_PACKET
#include <kazin/kazin.h>

using namespace kazin::global;
using Socket = kazin::Socket;
using InPacket = kazin::InPacket;
using OutPktRef = kazin::OutPktRef;

using Auth = kazin::Auth;
using namespace kazin;

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
using TilePointValue = i16;
using WorldPointValue = i16;

/*
#ifdef CLIENT
using RenderPoint = sf::Vector2<RenderPointValue>;
using TilePoint = sf::Vector2<TilePointValue>;
using WorldPoint = sf::Vector2<WorldPointValue>;
using MovePoint = sf::Vector2f;
#else
using RenderPoint =	Point<RenderPointValue>;
using TilePoint =		Point<TilePointValue>;
using WorldPoint =	Point<WorldPointValue>;
using MovePoint =		Point<float>;
#endif
*/
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

class Direction
{
public:
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

	auto operator==(const ID & id)
	{
		return _id == id;
	}

	auto operator==(const Direction& dir)
	{
		return operator==(dir._id);
	}

	auto operator!=(const ID & id)
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

	Point<i8> GetDelta(ID id)
	{
		static const vector<Point<i8>> list =
		{
			{  0, -1	},
			{  1, -1	},
			{  1,  0	},
			{  1,  1	},
			{  0,  1	},
			{ -1,  1	},
			{ -1,  0	},
			{ -1, -1 }
		};

		return list[id];
	}

	auto GetDelta()
	{
		return GetDelta(_id);
	}

	template<typename U>
	void Move(U & pos)
	{
		pos += GetDelta().As<decltype(pos.x)>();
	}

	static Direction GetContrary(ID id)
	{
		switch (id)
		{
		case ID::North:		return ID::South;
		case ID::NorthEast:	return ID::SouthWest;
		case ID::East:			return ID::West;
		case ID::SouthEast:	return ID::NorthWest;
		case ID::South:		return ID::North;
		case ID::SouthWest:	return ID::NorthEast;
		case ID::West:			return ID::East;
		case ID::NorthWest:	return ID::SouthEast;
		}

		return ID::None;
	}

	auto GetContrary() const 
	{
		return GetContrary(_id);
	}

	auto IsValid() const
	{
		return enum_contains(_id);
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
		VisibleTiles / 2
	};

	enum ID : u8
	{
		Battlefield,

		None = u8(-1)
	};

	template<typename Tile>
	class Base
	{
	public:
		ID _id = ID::None;

		TilePoint _size;

		vector<unique_ptr<Tile>> _tiles;

		Tile* GetTile(TilePoint pt)
		{
			return _tiles[pt.y + pt.x * _size.y].get();
		}

		bool IsValidPoint(TilePoint pt) const
		{
			return pt.x > 0 && pt.y > 0 && pt.x < _size.x&& pt.y < _size.y;
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

	enum Action
	{
		Stop,
		Move,
		None = u8(-1),
	};

	class Base : public packet::Serializable
	{
	public:
		using Idx = unit::Idx;

		string _name;
		TilePoint _pt;
		Idx _idx = kNullIdx;
		Action _action = Action::None;
		Direction _dir = Direction::South;

#define sparams _name, _pt, _idx, _action, _dir
		void Write([[maybe_unused]] packet::Out& out) const override
		{
#ifndef CLIENT
			out.Write(sparams);
#endif
		}

		void Read([[maybe_unused]] packet::In* in) override
		{
#ifdef CLIENT
			in->Read(sparams);
#endif
		}

		packet::Size GetPacketSize() const override
		{
			return packet::Base::Sizer::Get(sparams);
		}
#undef sparams

		bool IsOnSight(unit::Base * base)
		{
			// #todo check if it's >= or >
			return 
				_pt.x - map::VisibleTilesRange.x >= base->_pt.x && 
				_pt.x + map::VisibleTilesRange.x <= base->_pt.x && 
				_pt.y - map::VisibleTilesRange.y >= base->_pt.y &&
				_pt.y + map::VisibleTilesRange.y <= base->_pt.y;
		}

		bool Is(Base * base) const
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

	class Base
	{
	public:

	};
}

namespace npc
{
	using Idx = unit::Idx;
	constexpr Idx
		kNullIdx = unit::kNullIdx;


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
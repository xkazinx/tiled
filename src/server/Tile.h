#pragma once

namespace Server
{
struct Unit;

struct Tile
{
	using Type = TileType;
	Tile(Type type);
	Type _type;

	bool IsMovable() const
	{
		return _type == TileType::Movable;
	}

	Unit * _unit = nullptr;
};
}

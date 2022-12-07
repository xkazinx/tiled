#pragma once

namespace Server
{
class Unit;

class Tile
{
public:
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
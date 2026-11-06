#pragma once
#include "Unit.h"

namespace Client
{
struct Player : Unit
{
	Player(const string & name) : Unit() {
		_name = name;
	}

	Direction GetNextDir(TilePoint src, TilePoint dst);
	TilePoint _next_pt = { 0, 0 };
};
}

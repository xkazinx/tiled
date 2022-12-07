#pragma once
#include "Unit.h"

namespace Client
{
class Player : public Unit
{
public:
	Player(const string & name) : Unit() {
		_name = name;
	}
	Direction GetNextDir(TilePoint src, TilePoint dst);
	TilePoint _next_pt;
	Direction _stop_dir;
};
}
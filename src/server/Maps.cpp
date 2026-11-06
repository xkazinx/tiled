#include "stdafx.h"
#include "Maps.h"
#include "Map.h"
#include "Tile.h"

namespace Server
{
	Maps::Maps()
	{
		for (auto id : enum_values<MapID>())
		{
			// map::ID::None names no map
			if (id != MapID::None)
				_tpl.push_back(make_unique<Map>(id));
		}
	}

	Maps::~Maps()
	{
	}

	void Maps::Tick()
	{
		for (auto & map : _list)
		{
			map->Tick();
		}
	}
}

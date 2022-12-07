#include "stdafx.h"
#include "MapMgr.h"
#include "Map.h"
#include "Tile.h"

namespace Server
{
	MapMgr::MapMgr()
	{
		for(auto id : enum_values<MapID>())
			_tpl.push_back(make_unique<Map>(id));
	}

	MapMgr::~MapMgr()
	{
	}
}
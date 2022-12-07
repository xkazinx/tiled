#pragma once

namespace Server
{
class Map;

class MapMgr
{
public:
	MapMgr();
	~MapMgr();

	vector<unique_ptr<Map>> _list, _tpl;
};
}

using namespace Server;
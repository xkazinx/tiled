#pragma once

namespace Server
{
class Map;

class Maps
{
public:
	Maps();
	~Maps();

	void Tick();
	
	vector<unique_ptr<Map>> _list, _tpl;
};
}

using namespace Server;
#pragma once

namespace Server
{
struct Map;

struct Maps
{
	Maps();
	~Maps();

	void Tick();

	vector<unique_ptr<Map>> _list, _tpl;
};
}

using namespace Server;

#pragma once

namespace Server
{
struct Map;

struct Unit : unit::Base
{
	Map * _map = nullptr;
	bool _is_dead = false;
};
}

#pragma once

namespace Server
{
class Map;

class Unit : public unit::Base
{
public:
	Map * _map = nullptr;
	bool _is_dead = false;
};
}
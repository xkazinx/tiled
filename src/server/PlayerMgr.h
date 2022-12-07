#pragma once

namespace Server
{
class Player;

class PlayerMgr
{
public:
	array<unique_ptr<Player>, player::kMax> _array;
	vector<Player*> _list;

	PlayerMgr();
	~PlayerMgr();
};
}
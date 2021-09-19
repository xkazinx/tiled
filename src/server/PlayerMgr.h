#pragma once

class Player;

class PlayerMgr
{
public:
	std::array<unique_ptr<Player>, 20> _array;
	std::vector<Player*> _list;

	void Add(unique_ptr<Player>&& player);

	PlayerMgr();
	~PlayerMgr();

	int Tick();
	void HandleSocket();
};


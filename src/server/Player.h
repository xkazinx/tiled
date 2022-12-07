#pragma once
#include "Unit.h"

namespace Server
{
class Map;
class Player : public Unit
{
public:
	Player(PlayerIdx idx, ENetPeer* peer);
	~Player();
	
	ENetPeer* _peer;
	string _ip;

	bool _in_game = false;
	void OnEnterGame(const Auth & auth);
};
}
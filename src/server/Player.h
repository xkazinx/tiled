#pragma once
#include "Unit.h"

namespace Server
{
struct Map;
struct Player : Unit
{
	Player(PlayerIdx idx, ENetPeer * peer);
	~Player();

	ENetPeer * _peer;
	string _ip;

	bool _in_game = false;
	void OnEnterGame(const Auth & auth);
};
}

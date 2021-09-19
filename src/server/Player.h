#pragma once

class Player
{
public:
	Player(PlayerIdx idx, ENetPeer* peer);
	~Player();

	int Tick();
	void HandleSocket();

	PlayerIdx _idx;
	ENetPeer* _peer;
};


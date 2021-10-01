#pragma once

class Player
{
public:
	Player(PlayerIdx idx, ENetPeer* peer);
	~Player();
	
	PlayerIdx _idx;
	ENetPeer* _peer;
	string _ip;
};


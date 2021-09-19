#include "stdafx.h"
#include "Player.h"

Player::Player(PlayerIdx idx, ENetPeer* peer) :
	_idx(idx), _peer(peer)
{

}

Player::~Player()
{

}
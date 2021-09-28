#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"

void PlayerMgr::Add(unique_ptr<Player>&& player)
{
	assert(!_array[1]);
	_array[/*player->_peer_id*/1] = move(player);
}

PlayerMgr::PlayerMgr()
{

}

PlayerMgr::~PlayerMgr()
{
	
}

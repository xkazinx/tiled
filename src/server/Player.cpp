#include "stdafx.h"
#include "Player.h"
#include "MapMgr.h"
#include "Map.h"
#include "Tile.h"
#include "PlayerMgr.h"
#include "App.h"

namespace Server
{
Player::Player(PlayerIdx idx, ENetPeer * peer) :
	_peer(peer)
{
	// #todo Unit constructor
	_idx = idx;
}

Player::~Player()
{
	if(_map)
	{
		std::erase(_map->_players, this);
	}
}

void Player::OnEnterGame(const Auth & auth)
{
	_name = auth._user.c_str();
	_in_game = true; // #todo if already in game, can't log in nor create account

	Map * map = nullptr;
	if(g_maps->_list.empty())
	{
		g_maps->_list.push_back(make_unique<Map>(g_maps->_tpl[0].get()));
		map = g_maps->_list.back().get();
	}
	else
	{
		map = g_maps->_list[0].get();
	}

	if(!map)
	{
		PrintError("Player::OnEnterGame::!map");
		return;
	}
	
	_map = map;
	map->_players.push_back(this);

	_pt = { 50 + (TilePointValue)g_players->_list.size(), 50 };

	map->SetUnit(this);

	{
		// #todo create rooms | a map = a game
		auto pkt = g_app->DynamicPacket(MainOp::EnterGame, sizeof(u32) + (map->_tiles.size() * sizeof(TileType)), _idx, _pt);
		map->Write_EnterGame(pkt);

		Global::Send(this, pkt);
	}

	// Spawn for nearby
	{
	}
}
}
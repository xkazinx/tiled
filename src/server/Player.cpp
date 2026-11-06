#include "stdafx.h"
#include "Player.h"
#include "Maps.h"
#include "Map.h"
#include "Tile.h"
#include "Units.h"
#include "App.h"

namespace Server
{
Player::Player(PlayerIdx idx, ENetPeer * peer) :
	_peer(peer)
{
	// #todo Unit constructor
	_idx = idx;
	DebugLog("Player constructed with index {}", _idx);
}

Player::~Player()
{
	if (_map)
	{
		std::erase(_map->_players, this);
	}
}

void Player::OnEnterGame(const Auth & auth)
{
	_name = auth._user;
	_in_game = true; // #todo if already in game, can't log in nor create account

	Map * map = nullptr;
	if (g_maps->_list.empty())
	{
		g_maps->_list.push_back(make_unique<Map>(g_maps->_tpl[0].get()));
		map = g_maps->_list.back().get();
	}
	else
	{
		map = g_maps->_list[0].get();
	}

	if (!map)
	{
		Err("Player::OnEnterGame::!map");
		return;
	}

	_map = map;
	map->_players.push_back(this);
	_pt = { 80, 80 };

	_pt.x += (decltype(_pt.x))_map->_players.size();

	if (!_map->IsValidPoint(_pt))
	{
		// #todo Make it so that it searches for a new spot
	}

	map->SetUnit(this);

	{
		vector<Player *> on_sight;
		for (auto player : map->_players)
		{
			if (player != this && IsOnSight(player))
				on_sight.push_back(player);
			else if (player != this)
				DebugLog("Player not on sight for spawn: {} | {}", Format(_pt), Format(player->_pt));
		}

		PktSz units_size = 0;
		for (auto player : on_sight)
			units_size += static_cast<unit::Base *>(player)->GetPacketSize();

		// #todo create rooms | a map = a game
		auto pkt = g_app->DynamicPacket(MainOp::EnterGame,
			(PktSz)(sizeof(TilePoint) +
			(map->_tiles.size() * sizeof(TileType)) +
			sizeof(u16) + units_size),
			_idx, _pt);

		map->Write_EnterGame(pkt);

		pkt->Write((u16)on_sight.size());
		for (auto player : on_sight)
		{
			static_cast<Unit *>(player)->Write(*pkt);
			DebugLog("Wrote player on sight for spawn: {}", player->_name);
		}

		Global::Send(this, pkt);
	}

	{
		auto pkt = g_units->StaticPacket(UnitOp::Spawn, (unit::Base &)*this);
		_map->SendNearby(pkt, this);
	}
}
}

#include "stdafx.h"
#include "Global.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "MapMgr.h"

namespace Server::Global
{
	App* g_app;
	unique_ptr<kazin::Socket> g_sock;
	unique_ptr<PlayerMgr> g_players;
	unique_ptr<MapMgr> g_maps;
	void Initialize() {
		// unordered
		g_players = make_unique<PlayerMgr>();
		g_maps = make_unique<MapMgr>();
		// ordered
	}

	void Deinitialize() {

	}

	void Send(const Player* player, kazin::OutPktRef & out, enet_uint8 channel)
	{
		g_sock->Send(player->_peer, out, channel);
	}
}

using namespace Server;
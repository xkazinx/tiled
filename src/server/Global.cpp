#include "stdafx.h"
#include "Global.h"
#include "PlayerMgr.h"
#include "Player.h"

namespace Global
{
	App* g_app;
	unique_ptr<kazin::Socket> g_sock;
	unique_ptr<PlayerMgr> g_player_mgr;

	void Initialize() {
		// unordered
		g_player_mgr = make_unique<PlayerMgr>();

		// ordered
	}

	void Deinitialize() {

	}

	void Send(const Player* player, kazin::OutPktRef & out, enet_uint8 channel)
	{
		g_sock->Send(player->_peer, out, channel);
	}
}
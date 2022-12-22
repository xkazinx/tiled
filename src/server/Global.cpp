#include "stdafx.h"
#include "Global.h"
#include "Units.h"
#include "Player.h"
#include "Maps.h"

namespace Server::Global
{
	App* g_app;
	unique_ptr<kazin::Socket> g_sock;
	unique_ptr<Units> g_units;
	unique_ptr<Maps> g_maps;

	void Initialize() {
		// unordered
		g_units = make_unique<Units>();
		g_maps = make_unique<Maps>();
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
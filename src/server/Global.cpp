#include "stdafx.h"
#include "Global.h"
#include <Socket.h>
#include "PlayerMgr.h"

namespace Global
{
	App* g_app;
	unique_ptr<Socket> g_sock;
	unique_ptr<PlayerMgr> g_player_mgr;

	void Initialize() {
		// unordered
		g_sock = make_unique<Socket>(Socket::Type::Server, 256);
		g_player_mgr = make_unique<PlayerMgr>();

		// ordered
	}

	void Deinitialize() {

	}
}
#pragma once

class App;
class Socket;
class PlayerMgr;

namespace Global
{
	constexpr char kAppName[] = "Tileship Server";

	extern App* g_app;
	extern unique_ptr<Socket> g_sock;
	extern unique_ptr<PlayerMgr> g_player_mgr;

	extern void Initialize();
	extern void Deinitialize();
}
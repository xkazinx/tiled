#pragma once

class App;
class Socket;
class PlayerMgr;
class Player;

namespace Global
{
	constexpr char kAppName[] = "Tileship Server";

	extern App* g_app;
	extern unique_ptr<Socket> g_sock;
	extern unique_ptr<PlayerMgr> g_player_mgr;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(const Player* player, OutPktRef&& out, enet_uint8 channel = 0);
}
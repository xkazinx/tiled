#pragma once

namespace Server
{
class App;
class PlayerMgr;
class Player;
class MapMgr;

namespace Global
{
	constexpr char kAppName[] = "Tiled Server";

	extern App * g_app;
	extern unique_ptr<kazin::Socket> g_sock;
	extern unique_ptr<PlayerMgr> g_players;
	extern unique_ptr<MapMgr> g_maps;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(const Player* player, kazin::OutPktRef & out, enet_uint8 channel = 0);
}
}
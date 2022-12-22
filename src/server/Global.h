#pragma once

namespace Server
{
class App;
class Units;
class Player;
class Maps;

namespace Global
{
	constexpr char kAppName[] = "Tiled Server";

	extern App * g_app;
	extern unique_ptr<kazin::Socket> g_sock;
	extern unique_ptr<Units> g_units;
	extern unique_ptr<Maps> g_maps;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(const Player* player, kazin::OutPktRef & out, enet_uint8 channel = 0);
}
}
#pragma once

class App;
class Socket;

namespace Global
{
	constexpr char kAppName[] = "Tileship Server";

	extern App* g_app;
	extern unique_ptr<Socket> g_sock;

	extern void Initialize();
	extern void Deinitialize();
}
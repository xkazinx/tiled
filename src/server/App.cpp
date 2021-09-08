#include "stdafx.h"
#include "App.h"
#include <Socket.h>

App::App()
{
	g_app = this;
	Global::Initialize();
}

App::~App()
{
	Global::Deinitialize();
}


int App::Tick() 
{
	while (true)
	{
		HandleSocket();

		this_thread::sleep_for(chrono::milliseconds(1));
	}
	return 0;
}

void App::HandleSocket()
{
	ENetEvent ev;
	while (g_sock->Service(ev))
	{
		switch (ev.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			cout << "CONECTADO" << endl;
			//OnConnect();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			//OnPacket(ev.packet);
			enet_packet_destroy(ev.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			cout << "DESCONECTADO";
			//OnDisconnect();
			break;
		}
	}
}
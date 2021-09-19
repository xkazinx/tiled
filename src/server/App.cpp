#include "stdafx.h"
#include "App.h"
#include <Socket.h>
#include "Player.h"
#include "PlayerMgr.h"

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

void App::OnConnect(ENetEvent& ev)
{
	auto p = OnAuth(ev);
	if (!p)
	{
		cout << "Error OnConnect::!p" << endl;
		return;
	}

	NetOp_Login();
}

void App::OnPacket(ENetEvent& ev)
{
	auto p = OnAuth(ev);
	if (!p)
	{
		cout << "OnPacket::!p" << endl;
		return;
	}

	NetOp op = NetOp::Login;
	switch(op)
	{
	case NetOp::Login:	NetOp_Login(); break;
	default:
		cout << "Unknown packet received " << (int)op << endl;
		break;
	}
}

void App::OnDisconnect(ENetEvent& ev)
{
	//g_player_mgr->_list.push_back(p.get());
	// g_player_mgr->_array[*player_idx].get();
}

Player* App::OnAuth(const ENetEvent& ev) const
{
	Player* player = nullptr;
	auto& player_idx = (PlayerIdx*&)(ev.peer->data);
	if (!player_idx)
	{
		for (PlayerIdx i = 1; i < player::kMax; i++)
		{
			auto& p = g_player_mgr->_array[i];
			if (!p)
			{
				p = make_unique<Player>(i, ev.peer);
				player_idx = new PlayerIdx(i);
				player = p.get();
				g_player_mgr->_list.push_back(p.get());
				cout << "Player connected: " << i;
				break;
			}
		}
	}
	else
	{
		player = g_player_mgr->_array[*player_idx].get();
	}

	return player;
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
			OnConnect(ev);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			OnPacket(ev);
			enet_packet_destroy(ev.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			cout << "DESCONECTADO";
			OnDisconnect(ev);
			break;
		}
	}
}

void App::NetOp_Login()
{
	auto sql = Sql::Create();
	cout << "Sql connection: " << (bool)(sql.get() != nullptr) << endl;
	if (!sql)
		return;

	//sql->Query("INSER ({},{})", username, password);
	string username = "user";
	string password = "pass";
	//auto query = "SELECT * FROM players WHERE players.username = " + username + " AND players.password = " + hash + " LIMIT 1";
	auto query = "INSERT INTO `players` (username, password) VALUES(\'" + username + "\',\'" + password + "\');";
	mysql_real_query(sql->Get(), query.data(), query.size());
	// auto player = GetPacketPlayer();
	// auto username = GetPacket()->Pop<string>();
	// auto password = GetPacket()->Pop<string>();

	//auto hash = password; // TODO Hash

	
	// Initializar base de datos
	// Armar query
	// Comparar y leer datos
	// Enviar confirmacion al cliente y (si es necesario) a otros clientes
}
#include "stdafx.h"
#include "App.h"
#include <Socket.h>
#include "Player.h"
#include "PlayerMgr.h"

App::App()
	: Broadcaster<MainOp>(NetOp::Main)
{
	g_app = this;
	Global::Initialize();
}

App::~App()
{
	Global::Deinitialize();
}

void App::NetOperationProcess()
{
	cout << "NetOperationProcess Op: " << (int)GetOperation() << endl;
	switch(GetOperation())
	{
	case MainOp::Login:	NetOp_Login(); break;
	default:
		break;
	}
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

void App::OnConnect([[maybe_unused]] ENetEvent& ev)
{
}

void App::OnPacket(ENetEvent& ev)
{
	auto* p = OnAuth(ev);
	if (!p)
	{
		cout << "OnPacket::!p" << endl;
		return;
	}
	
	InPkt in(ev.packet);
	const InOperation opt(in, p, ev.peer);
	switch (const auto op = in.ReadAs<NetOp>();
		op)
	{
	case NetOp::Main:	this->NetOperation(opt);	break;

	default: // #todo
		cout << "Error: Unknown NetOp" << (int)op << endl;
		break;
	}
}

void App::OnDisconnect([[maybe_unused]] ENetEvent& ev)
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
			OnConnect(ev);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			OnPacket(ev);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			OnDisconnect(ev);
			break;
		}
	}
}

void App::NetOp_Login()
{
	auto sql = Sql::Create();
	if (!sql)
		return;

	//sql->Query("INSER ({},{})", username, password);
	string username = "user";
	string password = "pass";

	GetPacket()->Read(username, password);
	cout << "User: " << username << " | Password: " << password << endl;

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
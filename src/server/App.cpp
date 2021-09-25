#include "stdafx.h"
#include "App.h"
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

	if (auto& player_idx = reinterpret_cast<PlayerIdx*&>(ev.peer->data); 
		!player_idx)
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
				cout << "Player authored: " << i << endl;
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
	
	packet::SerializableString username(2);
	packet::SerializableString password;

	auto algo = GetPacket()->ReadAs<int>();
	GetPacket()->Read(username, password);

	cout << "algo " << algo;
	cout << "User: " << username << " | Password: " << password << endl;

	/*sql->Insert("players",
		vector<string>{"username", "password" },
		vector<vector<string>>{{ { username }, { password } }}
	);*/

	auto res = sql->Select("players", { "username", "password" });
	while (auto it = res->Pop())
	{
		cout << it->Get<string>("username") << endl;
		cout << it->Get<string>("password") << endl;
		cout << endl;
		/*auto u = it->Get<string>("username");
		auto p = it->Get<string>("password");
		cout << 
			"username: " << u << " " <<
			"password: " << p << endl;*/
	}

	// auto player = GetPacketPlayer();
	// auto username = GetPacket()->Pop<string>();
	// auto password = GetPacket()->Pop<string>();

	//auto hash = password; // TODO Hash

	// Initializar base de datos
	// Armar query
	// Comparar y leer datos
	// Enviar confirmacion al cliente y (si es necesario) a otros clientes
}
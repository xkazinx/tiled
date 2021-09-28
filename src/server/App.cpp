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
	cout << "NetOperationProcess Op: " << to_underlying(GetOperation()) << endl;
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
		//Log::Err("Unknown NetOp: " << to_underlying(op) << endl;
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
	const auto sql = Sql::Create();
	if (!sql)
		return;
	
	packet::SerializableString username(2);
	packet::SerializableString password;
	
	GetPacket()->Read(username, password);

	Log::Out("Data from packet... Username: {} - Password: {}", username, password);

	/*sql->Insert("players",
		vector<string>{"username", "password" },
		vector<vector<string>>{{ { username }, { password } }}
	);*/

	if (const auto res = sql->Select("players",
		{ "username", "password" }))
	{
		while (const auto it = res->Pop())
		{
			Log::Out("From database... Username: {} - Password: {}",
				it->Get<string>("username"),
				it->Get<string>("password"));
		}
	}
}
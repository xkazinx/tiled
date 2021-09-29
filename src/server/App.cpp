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
	Print("NetOperationProcess: MainOp{}", Format(GetOperation()));

	switch(GetOperation())
	{
	case MainOp::Login:	NetOp_Login(); break;
	default:
		Log::Error("Unknown MainOp{}", Format(GetOperation()));
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
		Log::Error("App::OnPacket::!p");
		return;
	}
	
	InPkt in(ev.packet);
	const InOperation opt(in, p, ev.peer);
	switch (const auto op = in.ReadAs<NetOp>();
		op)
	{
	case NetOp::Main:	this->NetOperation(opt);	break;

	default: // #todo
		Log::Error("Unknown NetOp{}", Format(op));
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
				Print("OnAuth: PlayerIdx {}", i);
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
	
	NetString username(player::kNameSize);
	NetString password(player::kPasswordSize);
	
	GetPacket()->Read(username, password);

	Print("Data from packet... Username: {} Password: {}", username, password);

	/*sql->Insert("players",
		vector<string>{"username", "password" },
		vector<vector<string>>{{ { username }, { password } }}
	);*/

	if (const auto res = sql->Select("players",
		{ "username", "password" }))
	{
		while (const auto it = res->Pop())
		{
			Print("From database... Username: {} - Password: {}",
				it->Get<string>("username"),
				it->Get<string>("password"));
		}
	}
}
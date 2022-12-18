#include "stdafx.h"
#include "App.h"
#include "Player.h"
#include "PlayerMgr.h"

namespace Server
{
App::App()
	: kazin::app::Base("Tiled Server"), Broadcaster<Op>(NetOp::Main)
{
	// #todo #param or whatever name it has for client and server
	g_app = this;
	Global::Initialize();

	using namespace std;
	g_sock = make_unique<Socket>(Socket::Type::Server, 
		bind(&App::OnConnect,	 this,	placeholders::_1), 
		bind(&App::OnPacket,		 this,	placeholders::_1),
		bind(&App::OnDisconnect, this,	placeholders::_1),
		256);
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
		OP(Login);
		OP(CreateAccount);
		OP(EnterGame);

	default:
		PrintError("Unknown MainOp{}", Format(GetOperation()));
		break;
	}
}

int App::Tick() 
{
	while (true)
	{
		time::Tick();

		g_sock->Tick();

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
		PrintError("App::OnPacket::!p");
		return;
	}
	
	using enum NetOp;
	InPacket in(ev.packet);
	const InOperation opt(in, p, ev.peer);

	switch (const auto op = in.ReadAs<NetOp>();
		op)
	{
	case Main:	this->NetOperation(opt);	break;

	default: // #todo
		PrintError("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::OnDisconnect([[maybe_unused]] ENetEvent & ev)
{
	//g_player_mgr->_list.push_back(p.get());
	//g_player_mgr->_array[*player_idx].get();
}

Player* App::OnAuth(const ENetEvent & ev) const
{
	Player* player = nullptr;

	if (auto& player_idx = reinterpret_cast<PlayerIdx*&>(ev.peer->data); 
		!player_idx)
	{
		for (PlayerIdx i = 1; i < player::kMax; i++)
		{
			auto& p = g_players->_array[i];
			if (!p)
			{
				p = make_unique<Player>(i, ev.peer);
				player_idx = new PlayerIdx(i);
				player = p.get();
				g_players->_list.push_back(p.get());
				Print("OnAuth: PlayerIdx {}", i);
				break;
			}
		}
	}
	else
	{
		player = g_players->_array[*player_idx].get();
	}

	return player;
}

void App::NetOp_Login()
{
	const Auth auth(GetPacket());
	if (!auth.IsValid())
	{
		PrintError("NetOp_Login: Invalid Auth{}", Format(auth));
		return;
	}
	
	Print("NetOp_Login: Incoming Auth{}", Format(auth));

	const auto sql = Sql::Create();
	if (!sql)
		return;

	if (const auto res = sql->Select("players",
		{ "username", "password" }, 
		format(" WHERE `username` = '{}' LIMIT 1", auth._user)))
	{
		if (const auto it = res->Pop())
		{
			if (auth._pass == it->Get<string>("password"))
			{
				Print("Login success for Auth{}", Format(auth));
				Send(GetPlayer(), MainOp::Login, true);
				GetPlayer()->OnEnterGame(auth);
				return;
			}
		}
	}
	
	Send(GetPlayer(), MainOp::Login, false);
	Print("Login failure for Auth{}", Format(auth));
}

void App::NetOp_CreateAccount()
{
	const auto sql = Sql::Create();
	if (!sql)
		return;

	const Auth auth(GetPacket());
	Print("Received auth: {} {}", auth._user, auth._pass);
	if (!auth.IsValid())
	{
		PrintError("NetOp_CreateAccount: Invalid Auth{}", Format(auth));
		return;
	}

	const auto res = sql->Select("players",
		{ "uid" }, format(" WHERE username = '{}' LIMIT 1", auth._user));
	
	Send(GetPlayer(), MainOp::CreateAccount, res->IsEmpty());

	if (res->IsEmpty())
	{
		sql->Insert("players",
			vector<string>{"username", "password" },
			vector<vector<string>>{ { { auth._user }, { auth._pass } }}
		);

		Print("Create account success: username({})", auth._user);
	}
	else
	{	
		Print("Create account failure: username({}) already in use", auth._user);
	}
}

void App::NetOp_EnterGame()
{
	Print("NetOp_EnterGame!!!");


}
}
#include "stdafx.h"
#include "App.h"
#include "Player.h"
#include "Units.h"
#include "Map.h"
#include "Maps.h"
#include <os/exe_path.h>
#ifdef _WIN32
#include <os/console_window.h>
#endif
#include <filesystem>

namespace Server
{
App::App()
	: Broadcaster<Op>(NetOp::Main)
{
	g_app = this;
	Global::Initialize();

#ifdef _WIN32
	lib::console_window::SetTitle(format("Tiled Server - {}", ProcessId()));
#endif

	g_sock = make_unique<SocketHandler>(Socket::Type::Host, ServerIp(), ServerPort(), player::kMax, 1);
	if (!g_sock->Get()->_host)
	{
		Err("Could not listen on port {}", ServerPort());
		std::exit(1);
	}
	Out("Listening on port {}", ServerPort());

	if (const u16 ws_port = (u16)lib::init::Number("ws", kWsPort))
	{
		std::error_code ec;
		const auto web_dir = lib::exe::Dir() / "web";
		const string web = lib::init::Value("web", std::filesystem::is_directory(web_dir, ec) ? web_dir.string() : string());
		if (_ws.Start({ .bind = lib::init::Value("ws_bind", "127.0.0.1"), .port = ws_port,
			.enet_host = "127.0.0.1", .enet_port = ServerPort(), .channels = 1, .max_clients = player::kMax, .web_root = web }))
			Out("WebSocket gateway on port {}{}", ws_port, web.empty() ? "" : " (serving " + web + ")");
		else
			Err("WebSocket gateway: could not listen on port {}", ws_port);
	}

	// The database answers before the first player comes
	if (!Sql::Create())
		Err("MySQL: not reachable (init.json db_host / db_port / db_user / db_pass / db_name): logins fail until it is");
}

App::~App()
{
	_ws.Stop();
	Global::Deinitialize();
}

void App::NetOperationProcess()
{
	DebugLog("NetOperationProcess: MainOp{}", Format(_op));

	switch (_op)
	{
		OP(Login);
		OP(CreateAccount);
		OP(EnterGame);

	default:
		Err("Unknown MainOp{}", Format(_op));
		break;
	}
}

int App::Tick()
{
	while (true)
	{
		lib::thread_time::Update();

		// The socket's own Service (not the handler's: that clears a peer's data on
		// its disconnect, where OnDisconnect finds the player's index)
		ENetEvent ev;
		while (g_sock->Get()->Service(&ev) > 0)
		{
			switch ((Socket::Event)ev.type)
			{
			case Socket::Event::Connect:	OnConnect(ev);		break;
			case Socket::Event::Packet:		OnPacket(ev);		break;
			case Socket::Event::Disconnect:	OnDisconnect(ev);	break;
			default: break;
			}
		}

		g_maps->Tick();

		this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}

void App::OnConnect(ENetEvent & ev)
{
	Socket::ApplyPeerTimeout(ev.peer);
	Out("OnConnected() {}", Format(ev.peer));
}

void App::OnPacket(ENetEvent & ev)
{
	IPkt in(ev.packet); // destroys the enet packet when read

	auto * p = OnAuth(ev);
	if (!p)
	{
		Err("App::OnPacket::!p");
		return;
	}

	using enum NetOp;
	switch (const auto op = in.ReadAs<NetOp>(); op)
	{
	case Main:	NetOperation(&in, p, ev.peer);	break;
	case Game:	if (p->_map) p->_map->NetOperation(&in, p, ev.peer);	break;
	case Unit:	if (p->_map) g_units->NetOperation(&in, p, ev.peer);	break;

	default: // #todo
		Err("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::OnDisconnect(ENetEvent & ev)
{
	auto *& player_idx = reinterpret_cast<PlayerIdx *&>(ev.peer->data);
	Out("OnDisconnect() {}", Format(ev.peer));
	if (!player_idx)
		return;

	const auto idx = *player_idx;
	delete player_idx;
	player_idx = nullptr;

	// The player leaves the map (the others told) and frees its slot
	auto & p = g_units->_array[idx];
	if (!p)
		return;

	if (p->_map)
	{
		p->_map->UnsetUnit(p.get());
		auto pkt = g_units->StaticPacket(UnitOp::Remove, idx);
		p->_map->SendNearby(pkt, p.get());
	}

	std::erase(g_units->_list, p.get());
	p.reset();
}

Player * App::OnAuth(const ENetEvent & ev) const
{
	Player * player = nullptr;

	if (auto & player_idx = reinterpret_cast<PlayerIdx *&>(ev.peer->data);
		!player_idx)
	{
		for (PlayerIdx i = 1; i < player::kMax; i++)
		{
			auto & p = g_units->_array[i];
			if (!p)
			{
				p = make_unique<Player>(i, ev.peer);
				player_idx = new PlayerIdx(i);
				player = p.get();
				g_units->_list.push_back(p.get());
				Out("OnAuth: PlayerIdx {}", i);
				break;
			}
		}
	}
	else
	{
		player = g_units->_array[*player_idx].get();
	}

	return player;
}

void App::NetOp_Login()
{
	const Auth auth(_in);
	if (!auth.IsValid())
	{
		Err("NetOp_Login: Invalid Auth{}", Format(auth));
		Send(_player, MainOp::Login, false);
		return;
	}

	if (_player->_in_game)
	{
		Err("NetOp_Login: Auth{} already in game", Format(auth));
		return;
	}

	Out("NetOp_Login: Incoming Auth{}", Format(auth));

	if (const auto sql = Sql::Create())
	{
		if (sql->CheckLogin(auth._user, auth._pass))
		{
			Out("Login success for Auth{}", Format(auth));
			Send(_player, MainOp::Login, true);
			_player->OnEnterGame(auth);
			return;
		}
	}

	Send(_player, MainOp::Login, false);
	Out("Login failure for Auth{}", Format(auth));
}

void App::NetOp_CreateAccount()
{
	const Auth auth(_in);
	if (!auth.IsValid())
	{
		Err("NetOp_CreateAccount: Invalid Auth{}", Format(auth));
		Send(_player, MainOp::CreateAccount, false);
		return;
	}

	const auto sql = Sql::Create();
	if (!sql)
	{
		Send(_player, MainOp::CreateAccount, false);
		return;
	}

	const bool created = sql->CreateAccount(auth._user, auth._pass);
	Send(_player, MainOp::CreateAccount, created);

	if (created)
		Out("Create account success: username({})", auth._user);
	else
		Out("Create account failure: username({}) already in use", auth._user);
}

void App::NetOp_EnterGame()
{
	DebugLog("NetOp_EnterGame!!!");
}
}

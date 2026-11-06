#include "stdafx.h"
#include "App.h"
#include "Scenes.h"
#include "LoginScene.h"
#include "Player.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "Units.h"
#include <imgui/imgui-SFML.h>
#ifdef _WIN32
#include <os/console_window.h>
#endif

namespace Client
{
App::App() :
	Broadcaster<Op>(NetOp::Main)
{
	g_app = this;

	_res_x = (u16)lib::init::Number("resx", 1024);
	_res_y = (u16)lib::init::Number("resy", 768);

	const string title = format("Tiled Client - {}", ProcessId());
#ifdef __EMSCRIPTEN__
	// The browser's canvas: WebGL2 gives what SFML asks for
	_window.create(VideoMode({ 1024, 768 }), title, sf::Style::Default);
#else
	_window.create(
		VideoMode({ 1024, 768 }),
		title,
		sf::Style::Default,
		sf::State::Windowed,
		ContextSettings{ 0, 0, 0, 3, 2 }
	);
	lib::console_window::SetTitle(title);
#endif

	if (!_window.setActive(true))
		Err("App: the window's OpenGL context could not be made current");
	_window.setVerticalSyncEnabled(true);

	ImGui::SFML::Init(_window);
	// The login window opens where the code puts it (no imgui.ini)
	ImGui::GetIO().IniFilename = nullptr;

	Global::Initialize();

	//ActiveAndAdjustWindow();

#ifdef __EMSCRIPTEN__
	// A browser reaches the server through its WebSocket gateway (kazilib
	// net/ws_gateway.h, enet/websocket.cpp): ?ws=wss://host/path/ws, else the
	// page's own folder + "ws"
	if (const string ws = lib::init::Value("ws"); !ws.empty())
		enet_ws_set_url(ws.c_str());
#endif

	g_sock = make_unique<SocketHandler>(Socket::Type::Client, ServerIp(), ServerPort(), 1, 1);
	g_sock->Connect();
	Out("Server: {}:{}", ServerIp(), ServerPort());
}

App::~App()
{
	if (g_sock)
		g_sock->Disconnect();
	Global::Deinitialize();
	ImGui::SFML::Shutdown();
}

void App::NetOperationProcess()
{
	DebugLog("NetOperationProcess MainOp{}", Format(_op));

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

void App::InitializeWindow()
{
}

int App::Tick()
{
	sf::Clock delta_clock;

	while (_window.isOpen())
	{
		if (!Frame(delta_clock))
			break;

#ifndef __EMSCRIPTEN__
		// A browser: one frame per pass; display() waits for the browser's next
		// animation frame (the page's refresh), so nothing to wait for there
		this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
	}

	return 0;
}

bool App::Frame(sf::Clock & delta_clock)
{
	lib::thread_time::Update();

	// Events
	{
		if (!HandleEvents())
			return false;
	}

	// Network
	{
		ENetEvent ev;
		for (auto e = g_sock->Service(&ev); e != Socket::Event::None; e = g_sock->Service(&ev))
		{
			switch (e)
			{
			case Socket::Event::Connect:	OnConnect(ev);		break;
			case Socket::Event::Packet:		OnPacket(ev);		break;
			case Socket::Event::Disconnect:	OnDisconnect(ev);	break;
			default: break;
			}
		}
	}

	// Update
	{
		g_scene->Tick();
		ImGui::SFML::Update(_window, delta_clock.restart());
	}

	// Render
	{
		_window.clear(sf::Color::Black);
		{
			g_scene->Render();
			ImGui::SFML::Render(_window);
		}
		_window.display();
	}

	return true;
}

bool App::HandleEvents()
{
	while (const std::optional ev = _window.pollEvent())
	{
		ImGui::SFML::ProcessEvent(*ev);

		if (ev->is<sf::Event::Closed>())
		{
			_window.close();
			return false;
		}

		if (ev->is<sf::Event::Resized>())
		{
			//auto _ = WindowGuard();
			//AdjustViewport(ev.size.width, ev.size.height);
		}
		else
		{
			g_scene->HandleEvent(&*ev);
		}
	}

	return true;
}

void App::OnConnect([[maybe_unused]] ENetEvent & ev)
{
	Out("Connected to server");
	auto * login = g_scene->Get<LoginScene>();
	if (!login)
		return;

	login->_status = "Connected";

	// Launch flags for a run without typing (tests, a link): `create` makes the
	// account first, `autologin` logs in (both with user= / pass=)
	const Auth auth(login->_user, login->_pass);
	if (lib::init::Flag("create", false))
		Send(MainOp::CreateAccount, auth);
	else if (lib::init::Flag("autologin", false))
		Send(MainOp::Login, auth);
}

void App::OnDisconnect([[maybe_unused]] ENetEvent & ev)
{
	Out("Disconnected from server");
	if (auto * login = g_scene->Get<LoginScene>())
		login->_status = "Disconnected from the server";
}

void App::OnPacket(ENetEvent & ev)
{
	DebugLog("OnPacket()");

	IPkt in(ev.packet); // destroys the enet packet when read
	using enum NetOp;
	switch (const auto op = in.ReadAs<NetOp>(); op)
	{
	case Main:	this->NetOperation(&in);		break;
	case Unit:	if (g_map) g_units->NetOperation(&in);	break;
	default:
		Err("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::NetOp_Login()
{
	auto * login = g_scene->Get<LoginScene>();
	if (!login)
		return;

	if (!_in->ReadAs<bool>())
	{
		login->_status = "Login failed: wrong username or password";
		Out("Login failed");
		return;
	}

	Out("Login accepted");
	g_player = make_unique<Player>(login->_user);
	g_scene->Set(SceneID::Game);
}

void App::NetOp_CreateAccount()
{
	auto * login = g_scene->Get<LoginScene>();
	const bool created = _in->ReadAs<bool>();
	Out("Create account: {}", created ? "created" : "refused");
	if (!login)
		return;

	login->_status = created ? "Account created: you can log in" : "Account not created: the name is taken or invalid";

	if (lib::init::Flag("create", false) && lib::init::Flag("autologin", false))
		Send(MainOp::Login, Auth(login->_user, login->_pass));
}

void App::NetOp_EnterGame()
{
	if (!g_player)
		return;

	_in->Read(g_player->_idx, g_player->_pt);

	g_map = make_unique<Map>(_in);

	DebugLog("NetOp_EnterGame()");
	// #todo Temporarily reading map data until passing it into files or something

	g_map->SetUnit(g_player.get());

	auto total_units = _in->ReadAs<u16>();
	DebugLog("TotalUnits: {}", total_units);
	while (total_units-- > 0)
	{
		unit::Base base;
		_in->Read(base);
		DebugLog("Base {} received", base._name);
		g_map->SetUnit(&base);
	}

	Out("Entered the game as {} at {}", g_player->_name, Format(g_player->_pt));

	// Launch flag walk=<dx>,<dy> (tests): walk that far from the start, as a click there
	if (const string walk = lib::init::Value("walk"); !walk.empty())
	{
		int dx = 0, dy = 0;
		if (std::sscanf(walk.c_str(), "%d,%d", &dx, &dy) == 2)
			g_player->_next_pt = g_player->_pt + TilePoint(dx, dy);
	}
}
}

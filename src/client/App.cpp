#include "stdafx.h"
#include "App.h"
#include "Scenes.h"
#include "LoginScene.h"
#include "Player.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "Units.h"
#include <kazilib/imgui/imgui-SFML.h>

namespace Client
{
App::App() :
	Broadcaster<Op>(NetOp::Main)
{
	g_app = this;
	Global::Initialize();

	_res_x = decltype(_res_x)(stoi(_args.Get("-resx", "1024")));
	_res_y = decltype(_res_y)(stoi(_args.Get("-resy", "768")));

	_window.create(
		VideoMode(1024, 768),
		common::kAppName,
		sf::Style::Default,
		ContextSettings{ 0, 0, 0, 3, 2 }
	);

	{
		string title = format("Tiled Client - {}", ::_getpid()).c_str();
		SetConsoleTitle(title.c_str());
		_window.setTitle(title);
	}

	_window.setActive(true);
	_window.setVerticalSyncEnabled(true);

	ImGui::SFML::Init(_window);

	//ActiveAndAdjustWindow();

	using namespace std;
	(g_sock = make_unique<Socket>(Socket::Type::Client,
		bind(&App::OnConnect,	 this, placeholders::_1),
		bind(&App::OnPacket,		 this, placeholders::_1),
		bind(&App::OnDisconnect, this, placeholders::_1))
	)->Connect(kHost, kPort);
}

App::~App()
{
	Global::Deinitialize();
	ImGui::SFML::Shutdown();
}

void App::NetOperationProcess()
{
	PrintDebug("NetOperationProcess MainOp{}", Format(GetOperation()));
	
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

void App::InitializeWindow()
{
}

int App::Tick() 
{
	sf::RenderStates states;
	sf::Clock delta_clock;

	while (_window.isOpen())
	{
		time::Tick();

		// Events
		{	
			if (!HandleEvents())
				break;
		}

		// Network
		{	
			g_sock->Tick();
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
				g_scene->Render(_window, states);
				ImGui::SFML::Render(_window);
			}
			_window.display();
		}

		this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}

bool App::HandleEvents()
{
	sf::Event ev;

	while (_window.pollEvent(ev))
	{
		ImGui::SFML::ProcessEvent(ev);

		if (ev.type == sf::Event::Closed)
		{
			_window.close();
			return false;
		}
		
		if (ev.type == sf::Event::Resized)
		{
			//auto _ = WindowGuard();
			//AdjustViewport(ev.size.width, ev.size.height);
		}
		else 
		{
			g_scene->HandleEvent(ev);
		}
	}

	return true;
}

void App::OnConnect([[maybe_unused]] ENetEvent & ev)
{
	PrintDebug("OnConnect()");
	if(g_game)
	{
		//g_app->Send(MainOp::EnterGame);
	}
}

void App::OnDisconnect([[maybe_unused]] ENetEvent & ev)
{
	PrintDebug("OnDisconnect()");
}

void App::OnPacket([[maybe_unused]] ENetEvent & ev)
{
	PrintDebug("OnPacket()");

	kazin::InPacket in(ev.packet);
	using enum NetOp;
	switch (const auto op = in.ReadAs<NetOp>();
		op)
	{
	case Main:	this->NetOperation(in);		break;
	case Unit:	g_units->NetOperation(in);	break;
	default:
		PrintError("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::NetOp_Login()
{
	g_player = make_unique<Player>(g_scene->Get<LoginScene>()->_auth._user);
	g_scene->Set(SceneID::Game);
}

void App::NetOp_CreateAccount()
{

}

void App::NetOp_EnterGame()
{
	TilePoint pt;
	GetPacket()->Read(g_player->_idx, g_player->_pt);

	g_map = make_unique<Map>(GetPacket());

	PrintDebug("NetOp_EnterGame()");
	PrintError("----------------------------------");
	// #todo Temporarily reading map data until passing it into files or something
	//GetPacket()->

	g_map->SetUnit(g_player.get());

	auto total_units = GetPacket()->ReadAs<u16>();
	PrintDebug("TotalUnits: {}", total_units);
	while(total_units-- > 0)
	{
		unit::Base base;
		GetPacket()->Read(base);
		PrintError("Base {} received", base._name);
		g_map->SetUnit(&base);
	}
}
}
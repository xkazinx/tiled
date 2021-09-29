#include "stdafx.h"
#include "App.h"
#include "SceneMgr.h"

App::App()
	: Broadcaster<MainOp>(NetOp::Main)
{
	g_app = this;
	Global::Initialize();

	InitializeWindow();

	ImGui::SFML::Init(_window);

	g_sock->Connect("localhost", 321);
}

App::~App()
{
	ImGui::SFML::Shutdown(); 
	Global::Deinitialize();
}

void App::NetOperationProcess()
{
	Log::Debug("NetOperationProcess MainOp{}", Format(GetOperation()));

	switch(GetOperation())
	{
	case MainOp::Login:
		Log::Debug("Login response: {}", GetPacket()->ReadAs<bool>());
		break;

	default:
		Log::Error("Unknown MainOp{}", Format(GetOperation()));
		break;
	}
}

void App::InitializeWindow()
{
	// OpenGL settings
	ContextSettings settings;
	{
		settings.depthBits = 24;
		settings.stencilBits = 8;
		settings.antialiasingLevel = 0;
		settings.majorVersion = 3;
		settings.minorVersion = 2;
	}

	_window.create(VideoMode(g_res_x, g_res_y), kAppName, sf::Style::Default, settings);

	// https://www.sfml-dev.org/tutorials/2.5/window-opengl.php
	_window.setActive(true);
	_window.setVerticalSyncEnabled(true);

	AdjustViewport(_window.getSize().x, _window.getSize().y);
}

void App::AdjustViewport(const GLsizei & x, const GLsizei & y)
{
	glViewport(0, 0, x, y);
}

int App::Tick() 
{
	sf::RenderStates states;
	sf::Clock delta_clock;

	while (_window.isOpen())
	{
		// Events
		{	
			if (!HandleEvents())
				break;
		}

		// Network
		{	
			HandleSocket();
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

		this_thread::sleep_for(chrono::milliseconds(1));
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
			AdjustViewport(ev.size.width, ev.size.height);
		}
		else 
		{
			g_scene->HandleEvent(ev);
		}
	}

	return true;
}

void App::OnConnect([[maybe_unused]] ENetEvent& ev)
{
	Log::Debug("OnConnect()");
}

void App::OnDisconnect([[maybe_unused]] ENetEvent& ev)
{
	Log::Debug("OnDisconnect()");
}

void App::OnPacket([[maybe_unused]] ENetEvent& ev)
{
	Log::Debug("OnPacket()");

	InPkt in(ev.packet);
	switch (const auto op = in.ReadAs<NetOp>();
		op)
	{
	case NetOp::Main:	this->NetOperation(in);	break;

	default: // #todo
		Log::Error("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::HandleSocket()
{
	ENetEvent ev;
	while(g_sock->Service(ev))
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
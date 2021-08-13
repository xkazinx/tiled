#include "stdafx.h"
#include "App.h"
#include "SceneMgr.h"

App::App()
{
	g_app = this;
	Global::Initialize();

	g_sock->Connect("127.0.0.1", 1234);

	InitializeWindow();
}

App::~App()
{
	Global::Deinitialize();
}

void App::InitializeWindow()
{
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
	// Configure the viewport (the same size as the window)
	glViewport(0, 0, x, y);
}

int App::Tick() 
{
	_window.clear(sf::Color::Black);

	sf::RenderStates states;

	while (_window.isOpen())
	{
		if (!HandleEvents())
			break;
		
		HandleSocket();
		
		// Update
		g_scene->Tick();
		// Render
		g_scene->Render(_window, states);

		_window.display();

		this_thread::sleep_for(chrono::milliseconds(1));
	}

	return 0;
}

bool App::HandleEvents()
{
	sf::Event ev;
	while (_window.pollEvent(ev))
	{
		if (ev.type == sf::Event::Closed)
		{
			//_window.close();
			return false;
		}

		// Adjust the viewport when the window is resized
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

void App::HandleSocket()
{
	ENetEvent ev;
	while(g_sock->Service(ev) != ENET_EVENT_TYPE_NONE)
	{
		switch (ev.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				ev.peer->address.host,
				ev.peer->address.port);

			/* Store any relevant client information here. */
			ev.peer->data = (void*)"Client information";
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				ev.packet->dataLength,
				ev.packet->data,
				(const char*)ev.peer->data,
				ev.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(ev.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n", (const char*)ev.peer->data);
			/* Reset the peer's client information. */
			ev.peer->data = nullptr;
			break;
		}
	}
}
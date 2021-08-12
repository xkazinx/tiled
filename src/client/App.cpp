#include "stdafx.h"
#include "App.h"

App::App()
{
	g_app = this;
	global::app::Initialize();
	InitializeWindow();
}

App::~App()
{
	global::app::Deinitialize();
}

void App::InitializeWindow()
{
	/*ContextSettings settings;
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

	AdjustViewport(_window.getSize().x, _window.getSize().y);*/
}

void App::AdjustViewport(const GLsizei & x, const GLsizei & y)
{
	// Configure the viewport (the same size as the window)
	glViewport(0, 0, x, y);
}

int App::Tick() 
{
	glClearColor(0.f, 0.f, 0.f, 1.f);

	while (_window.isOpen())
	{
		if (!HandleEvents())
			break;

		// Update
		// Render

		this_thread::sleep_for(chrono::milliseconds(1));
	}

	return 0;
}

bool App::HandleEvents()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			//_window.close();
			return false;
		}

		// Adjust the viewport when the window is resized
		if (event.type == sf::Event::Resized)
		{
			//auto _ = WindowGuard();
			//_AdjustViewport(event.size.width, event.size.height);
		}
		else 
		{
			//g_scene->HandleEvent(event);
		}
	}

	return true;
}
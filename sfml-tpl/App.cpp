#include "stdafx.h"
#include "App.h"

#include "Ticker.h"
#include "Camera.h"
#include "SceneMgr.h"

App::App()
{
	global::app::Initialize();

	_CreateWindow();
}

App::~App()
{
	global::app::Deinitialize();
}

void App::_CreateWindow()
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

	// Initialise GLEW for the extended functions.
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "glewInit != GLEW_OK";
		return;
	}

	_AdjustViewport(_window.getSize().x, _window.getSize().y);
}

void App::_AdjustViewport(const GLsizei & x, const GLsizei & y)
{
	// Configure the viewport (the same size as the window)
	glViewport(0, 0, x, y);
}

App * App::Tick() 
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	
	Ticker ticker;
	while (_window.isOpen())
	{
		if (!HandleEvents())
			break;

		ticker.Tick([&]()
		{
			sf::Time elapsed = _delta_clock.restart();
			g_scene->Update(elapsed);
		}, [&]()
		{
			_window.setView(*g_camera);
			_window.draw(*g_camera);
			_window.setView(_window.getDefaultView());
		});

		this_thread::sleep_for(chrono::milliseconds(1));
	}

	return this;
}

void App::Render() 
{ 
	auto gl_states = [&]() 
	{
		_window.pushGLStates();
		return mk<Guard>([&]() {
			_window.popGLStates();
		});
	};
}

int App::Destroy() 
{
	g_app.reset();
	return 0;
}

bool App::HandleEvents()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			_window.close();
			return false;
		}

		// Adjust the viewport when the window is resized
		if (event.type == sf::Event::Resized)
		{
			auto _ = WindowGuard();
			_AdjustViewport(event.size.width, event.size.height);
		}
		else {
			g_scene->HandleEvent(event);
		}
	}

	return true;
}
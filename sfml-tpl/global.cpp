#include "stdafx.h"
#include "global.h"
#include "SceneMgr.h"
#include "Camera.h"
#include "Random.h"
#include "TextMgr.h"

namespace global
{
	unique_ptr<App> g_app;
	unique_ptr<SceneMgr> g_scene;
	unique_ptr<Camera> g_camera;
	unique_ptr<TextMgr> g_text;
	unique_ptr<Random> g_rand;

	RenderWindow & GetWindow() {
		return g_app->_window;
	}

	unique_ptr<global::Guard> WindowGuard(bool start)
	{
		// Make the window the active window for OpenGL calls... says docs

		g_app->_window.setActive(start);
		return mk<Guard>([&]() {
			g_app->_window.setActive(!start);
		});
	}

	Vector2f DeltaVelocity(const Vector2f & velocity, const sf::Time & delta)
	{
		return { velocity.x * delta.asMilliseconds(), velocity.y * delta.asMilliseconds() };
	}

	namespace app
	{
		void Initialize() {
			// unordered

			g_rand = mk<Random>();
			g_text = mk<TextMgr>();

			// ordered
			g_camera = mk<Camera>();
			g_scene = mk<SceneMgr>();
		}

		void Deinitialize() {

		}
	}
}

namespace font {
	FontNames kNames = {
		"arial.ttf",
	};
}

namespace resolution {
	sf::Vector2f resolution::GetCenter(const Vector2f & base)
	{
		return { g_res_x / 2.f - base.x / 2.f, g_res_y / 2.f - base.y / 2.f };
	}
}

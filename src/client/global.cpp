#include "stdafx.h"
#include "Global.h"
#include "SceneMgr.h"
#include "Random.h"

namespace Global
{
	App* g_app;
	unique_ptr<Random> g_rand;
	unique_ptr<SceneMgr> g_scene;
	unique_ptr<Socket> g_sock;

	void Initialize() {
		// unordered
		g_rand = make_unique<Random>();
		g_scene = make_unique<SceneMgr>();
		g_sock = make_unique<Socket>(Socket::Type::Client);

		// ordered
	}

	void Deinitialize() {

	}
}


namespace font {
	FontNames kNames = {
		"arial.ttf",
	};
}
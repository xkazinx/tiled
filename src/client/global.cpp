#include "stdafx.h"
#include "Global.h"
#include "SceneMgr.h"
#include "GameScene.h"

namespace Global
{
	App * g_app;
	unique_ptr<SceneMgr> g_scene;
	unique_ptr<kazin::Socket>	g_sock;
	Map * g_map;
	GameScene * g_game = nullptr;

	void Initialize()
	{
		// unordered
		g_scene	= make_unique<SceneMgr>();

		// ordered
	}

	void Deinitialize()
	{

	}

	void Send(const kazin::OutPktRef out, enet_uint8 channel)
{
		g_sock->Send(out, channel);
	}
}

namespace font
{
	FontNames kNames = 
	{
		"arial.ttf",
	};
}
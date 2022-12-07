#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"
#include "Player.h"

namespace Client
{
GameScene::GameScene() 
	: _game_view({ 0.f, 0.f, (float)g_app->_res_x, (float)g_app->_res_y })
{
	g_game = this;

	PrintDebug("Initialized GameScene with player name {}", g_player->_name);
}

GameScene::~GameScene()
{
	g_game = nullptr;
}

void GameScene::Tick()
{

}

void GameScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	auto & default_view = g_app->_window.getDefaultView();

	tgt.setView(_game_view);

	{
		if(g_map)
			g_map->Render(tgt, states);

		if(g_map)
			g_map->RenderObjects(tgt, states);
	}

	tgt.setView(default_view);
}

bool GameScene::HandleEvents([[maybe_unused]] const sf::Event & ev)
{
	if(ev.type == sf::Event::KeyPressed)
	{
		switch(ev.key.code)
		{
			using enum sf::Keyboard::Key;
		case Subtract:
			PrintDebug("SUBSTRACT!");
			_game_view.zoom(1.1f);
			break;

		case Add:
			_game_view.zoom(0.9f);
			break;

		default:
			return false;
		}
	}

	return true;
}
}
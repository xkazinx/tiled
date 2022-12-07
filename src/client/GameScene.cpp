#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"
#include "Player.h"

namespace Client
{
GameScene::GameScene()
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
	if(g_map)
		g_map->Render(tgt, states);

	if(g_map)
		g_map->RenderObjects(tgt, states);
}

void GameScene::HandleEvents([[maybe_unused]] const sf::Event& ev)
{
	
}
}
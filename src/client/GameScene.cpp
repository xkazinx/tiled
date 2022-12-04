#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"

GameScene::GameScene()
{
	g_game = this;
	_map = make_unique<Map>();
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
	_map->Render(tgt, states);
}

void GameScene::HandleEvents([[maybe_unused]] const sf::Event& ev)
{
	
}

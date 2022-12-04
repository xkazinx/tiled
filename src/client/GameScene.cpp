#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"

GameScene::GameScene()
{
	_map = make_unique<Map>();
}

GameScene::~GameScene()
{

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

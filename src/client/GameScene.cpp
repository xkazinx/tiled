#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"

GameScene::GameScene()
{
	{
		//const string data = "10";
		const string data = "3012012012";
		vector<unique_ptr<Tile>> tiles;
		tile::LineSize size = (u8)(data[0] - '0');

		for(szt i = 1; i < data.size(); ++i)
		{
			tiles.emplace_back(make_unique<Tile>((tile::Type)(data[i] - '0'),
				sf::Vector2i{ ((i - 1) % size), (int)floor((i - 1) / size) }));
		}

		_map = make_unique<Map>(tiles, size);
	}
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

void GameScene::HandleEvents(const sf::Event& ev)
{
	
}

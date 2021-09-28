#include "stdafx.h"
#include "Map.h"
#include "Tile.h"

Map::Map(vector<unique_ptr<Tile>>& tiles, tile::LineSize line_size)
	: _line_size(line_size), _tiles(move(tiles))
{
	g_map = this;
}

Map::~Map()
{

}

void Map::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	for(const auto& tile : _tiles)
	{
		tile->Render(tgt, states);
	}
}

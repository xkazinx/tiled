#include "stdafx.h"
#include "Tile.h"
#include "Map.h"

Tile::Tile(Type type, const sf::Vector2i& pos)
	: _type(type), _pos(pos)
{

}

Tile::~Tile()
{

}

void Tile::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{

}

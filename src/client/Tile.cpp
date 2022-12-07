#include "stdafx.h"
#include "Tile.h"
#include "Map.h"

namespace Client
{
Tile::Tile(Type type, const sf::Vector2i& pos)
	: _type(type), _pos(pos)
{

}

Tile::Tile(InPacket* pkt)
{
	pkt->Read(_type);
}

Tile::~Tile()
{

}

void Tile::Render(
	[[maybe_unused]] sf::RenderTarget& tgt, 
	[[maybe_unused]] const sf::RenderStates& states) const
{

}
}
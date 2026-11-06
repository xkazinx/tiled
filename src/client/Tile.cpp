#include "stdafx.h"
#include "Tile.h"
#include "Map.h"

namespace Client
{
Tile::Tile(Type type, TilePoint pt)
	: _type(type), _pt(pt)
{

}

Tile::Tile(IPkt * pkt, TilePoint pt) : _pt(pt)
{
	pkt->Read(_type);
}

Tile::~Tile()
{

}

void Tile::Render(
	[[maybe_unused]] sf::RenderTarget & tgt,
	[[maybe_unused]] const sf::RenderStates & states) const
{

}
}

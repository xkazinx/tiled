#pragma once

namespace Client
{
class Tile final
{
	using Type = tile::Type;

public:
	Tile(InPacket * pkt, TilePoint pt);
	Tile(Type type, TilePoint pt);
	~Tile();

	Type _type;
	unit::Base * _unit = nullptr;

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;
	
	TilePoint _pt;
};
}
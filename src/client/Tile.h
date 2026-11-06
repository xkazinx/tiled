#pragma once

namespace Client
{
struct Tile final
{
	using Type = tile::Type;

	Tile(IPkt * pkt, TilePoint pt);
	Tile(Type type, TilePoint pt);
	~Tile();

	Type _type;
	unit::Base * _unit = nullptr;

	void Render(sf::RenderTarget & tgt, const sf::RenderStates & states) const;

	TilePoint _pt;
};
}

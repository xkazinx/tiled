#pragma once

namespace Client
{
class Tile final
{
	using Type = tile::Type;

public:
	Tile(InPacket * pkt);
	Tile(Type type, const sf::Vector2i& pos);
	~Tile();

	Type _type;
	unit::Base * _unit = nullptr;

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;
	
	sf::Vector2i _pos;
};
}
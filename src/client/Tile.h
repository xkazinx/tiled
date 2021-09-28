#pragma once

class Tile final
{
	using Type = tile::Type;

public:
	Tile(Type type, const sf::Vector2i& pos);
	~Tile();

	Type _type;

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;
	
	sf::Vector2i _pos;
	sf::VertexBuffer _vertex_buffer;
	sf::VertexArray _vertex_array;
};
#include "stdafx.h"
#include "Tile.h"
#include "Map.h"

Tile::Tile(Type type, const sf::Vector2i& pos)
	: _type(type), _pos(pos)
{
	_vertex_buffer.create(4);
	_vertex_buffer.setPrimitiveType(sf::PrimitiveType::Quads);
	
	constexpr auto half = tile::kHalfSize;
	constexpr auto origin = 300.f;

	_vertex_array.append(sf::Vertex{ {origin + ((pos.x - pos.y) * half),			origin + ((pos.x + pos.y) * half) }, tile::kColors[(szt)type] });
	_vertex_array.append(sf::Vertex{ {origin + ((pos.x - pos.y) * half) + half,	origin + ((pos.x + pos.y) * half) + half }, tile::kColors[(szt)type] });
	_vertex_array.append(sf::Vertex{ {origin + ((pos.x - pos.y) * half),			origin + ((pos.x + pos.y) * half) + tile::kSize }, tile::kColors[(szt)type] });
	_vertex_array.append(sf::Vertex{ {origin + ((pos.x - pos.y) * half) - half,	origin + ((pos.x + pos.y) * half) + half }, tile::kColors[(szt)type] });

	_vertex_buffer.update(&_vertex_array[0]);
}

Tile::~Tile()
{

}

void Tile::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	tgt.draw(_vertex_buffer, states);
}

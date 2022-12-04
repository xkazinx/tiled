#pragma once

class Tile;

class Map
{
public:
	Map();
	~Map();

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states);

	vector<unique_ptr<Tile>> _tiles;
	
	mutable sf::RenderTexture _grid_texture;
	mutable vector<sf::Vertex> _grid_vertex;
	mutable sf::VertexBuffer _grid;

	sf::Vector2<i16> _visible_tiles, _visible_tiles_range, _visible_tile_offset,
		_world_offset, _world_offset_adjust, _world_offset_movement, _half_tile_offset;

	bool _grid_visible = true;

	void UpdateCamera();

	RenderPoint GetRenderPoint(const TilePoint& tile_pt, const TilePoint& offset);
};
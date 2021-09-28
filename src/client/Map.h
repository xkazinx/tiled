#pragma once

class Tile;

class Map
{
public:
	Map(vector<unique_ptr<Tile>>& tiles, tile::LineSize line_size);
	~Map();

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;

	vector<unique_ptr<Tile>> _tiles;
	tile::LineSize _line_size;
};
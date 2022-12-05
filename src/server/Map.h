#pragma once

class Tile;

class Map
{
public:
	vector<unique_ptr<Tile>> _tiles;
};
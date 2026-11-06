#pragma once

namespace Client
{
struct Tile;
struct Unit;

struct Map : map::Base<Tile>
{
	Map(IPkt * pkt);

	Map();
	~Map();

	void Render(sf::RenderTarget & tgt, const sf::RenderStates & states);

	mutable vector<sf::Vertex> _grid_vertex;
	mutable sf::VertexBuffer _grid;

	Point<i16> _visible_tiles, _visible_tiles_range, _visible_tile_offset,
		_world_offset, _world_offset_adjust, _world_offset_movement,
		_half_tile_offset, _bounds_offset;

	bool _grid_visible = true;

	void UpdateCamera();

	RenderPoint GetRenderPoint(const TilePoint & tile_pt, const TilePoint & offset);

	void SetUnit(unit::Base * unit);
	void UnsetUnit(unit::Base * unit);
	// A player left (UnitOp::Remove): off its tile and the list
	void RemoveUnit(unit::Idx idx);
	void Tick();
	void RenderObjects(sf::RenderTarget & tgt, const sf::RenderStates & states);

	bool IsInSight(WorldPoint wpt);
	vector<unique_ptr<Unit>> _units_storage;
	vector<Unit *> _units;

	WorldPoint GetWorldPoint(TilePoint pt)
	{
		return pt * tile::kSize;
	}
};
}

#include "stdafx.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"
#include "Player.h"
#include "Units.h"

namespace Client
{
Map::Map()
{
	{
		auto res = g_app->_window.getSize();
		Print("Map::Map Resolution: (g_app->_window.getSize()) = {}x{}", res.x, res.y);
	}

	{
		auto tile_cnt = szt(((g_app->_res_x + 4 * tile::kSize) / tile::kSize) 
			* ((g_app->_res_y + 4 * tile::kSize) / tile::kSize)
			+ (ceil(g_app->_res_x / tile::kSize) + ceil(g_app->_res_y / tile::kSize) + 1));
	
		_grid.create(tile_cnt * 8);
		_grid.setPrimitiveType(sf::PrimitiveType::Lines);
		_grid.setUsage(sf::VertexBuffer::Dynamic);
		_grid_vertex.resize(tile_cnt * 8);
	}

	{
		_visible_tiles = map::VisibleTiles;
		_visible_tiles_range = map::VisibleTilesRange;

		if((_visible_tiles.x % 2) == 0)
			_half_tile_offset.x = -tile::kHalfSize;
	
		if((_visible_tiles.y % 2) == 0)
			_half_tile_offset.y = -tile::kHalfSize;
	}

	UpdateCamera();
}

Map::Map(InPacket * pkt) : Map()
{
	pkt->Read(_size);
	PrintError("Map::Map: Reading from packet, map of size {}", Format(_size));

	_tiles.reserve(_size.x * _size.y);

	TilePoint pt;
	for (pt.y = 0; pt.y < _size.y; ++pt.y)
	{
		for (pt.x = 0; pt.x < _size.x; ++pt.x)
		{
			_tiles.push_back(make_unique<Tile>(pkt, pt));	
		}
	}
}

void Map::UpdateCamera()
{
	TilePoint player_pt = g_player->_pt;

	_visible_tile_offset.x = player_pt.x - _visible_tiles_range.x;
	_visible_tile_offset.y = player_pt.y - _visible_tiles_range.y;

	/*
	if(_visible_tile_offset.x < 0)
		_visible_tile_offset.x = 0;

	if(_visible_tile_offset.y < 0)
		_visible_tile_offset.y = 0;
	*/

	_world_offset.x = _visible_tile_offset.x * tile::kSize;
	_world_offset.y = _visible_tile_offset.y * tile::kSize;

	_world_offset_adjust.x = _half_tile_offset.x + _world_offset.x;
	_world_offset_adjust.y = _half_tile_offset.y + _world_offset.y;

	{
		auto rpt = GetRenderPoint(player_pt, { 0, 0 });
		rpt.x -= tile::kHalfSize - 1;
		rpt.y -= tile::kHalfSize - 1;

		rpt.x += (player_pt.x * tile::kSize) - rpt.x;
		rpt.y += (player_pt.y * tile::kSize) - rpt.y;

		MovePoint mvpt = g_player->_mvpt;
		_world_offset_movement = { 0, 0 };
		_world_offset_movement.x -= RenderPointValue(mvpt.x - rpt.x);
		_world_offset_movement.y -= RenderPointValue(mvpt.y - rpt.y);

		if (g_player->_is_moving)
		{
			_world_offset_movement.x -= (RenderPointValue)round(g_player->_mvpt_decrease.x);
			_world_offset_movement.y -= (RenderPointValue)round(g_player->_mvpt_decrease.y);
		}
	}
}

Map::~Map()
{

}

void Map::Render(sf::RenderTarget& tgt, const sf::RenderStates& states)
{
	szt i;

	// for East and South
	constexpr auto extra_tile_sz = tile::kSize + tile::kHalfSize;

	/*
	Print("---");
	Print("_visible_tile_offset = {} {}", _visible_tile_offset.x, _visible_tile_offset.y);
	Print("g_app->_res = {} {}", g_app->_res_x, g_app->_res_y);
	Print("_world_offset = {} {}", _world_offset.x, _world_offset.y);
	Print("_world_offset_adjust = {} {}", _world_offset_adjust.x, _world_offset_adjust.y);
	Print("_world_offset_movement = {} {}", _world_offset_movement.x, _world_offset_movement.y);
	*/

	/*
	RenderPoint render_offset = {0, 0};
	if(_visible_tile_offset.x == 0)
		render_offset.x = abs(g_player->_pt.x - _visible_tiles_range.x);

	if (_visible_tile_offset.y == 0)
		render_offset.y = abs(g_player->_pt.y - _visible_tiles_range.y);
	*/
	RenderPoint rpt;
	TilePoint tpt;
	for(i = 0,
		tpt.y = _visible_tile_offset.y,
		rpt.y = /*-tile::kSize +*/ (_world_offset_adjust.y - _world_offset.y) + _world_offset_movement.y;
		rpt.y <= g_app->_res_y + extra_tile_sz;
		rpt.y += tile::kSize, ++tpt.y)
	{
		for(tpt.x = _visible_tile_offset.x,
			rpt.x = /*-tile::kSize +*/ (_world_offset_adjust.x - _world_offset.x) + _world_offset_movement.x;
			 rpt.x <= g_app->_res_x + extra_tile_sz;
			 rpt.x += tile::kSize, ++tpt.x, ++i)
		{
			if(_grid_visible)
			{
				auto v = &_grid_vertex[i * 8];
				auto px = float(rpt.x);
				auto py = float(rpt.y);
				//if (v[0].position != Vector2f(px, py))
				{
					v[0].position = Vector2f(px, py);
					v[1].position = Vector2f(px + tile::kSize, py);
					v[2].position = Vector2f(px + tile::kSize, py);
					v[3].position = Vector2f(px + tile::kSize, py + tile::kSize);
					v[4].position = Vector2f(px + tile::kSize, py + tile::kSize);
					v[5].position = Vector2f(px, py + tile::kSize);
					v[6].position = Vector2f(px, py + tile::kSize);
					v[7].position = Vector2f(px, py);

					auto color = sf::Color(255, 255, 255, 255 / 6);
					
					if(!IsValidPoint(tpt))
						continue;
					
					auto tile = GetTile(tpt);
					if (!tile)
						continue;

					if (tile->_unit)
						color = sf::Color::Green;

					if(!IsMovable(tpt))
						color = sf::Color::Red;
					if (v[0].color != color)
					{
						v[0].color = color;
						v[1].color = color;
						v[2].color = color;
						v[3].color = color;
						v[4].color = color;
						v[5].color = color;
						v[6].color = color;
						v[7].color = color;
					}

					font::Fixed(rpt, format("{}", Format(tpt)), sf::Color::White);
				}
			}
		}
	}

	_grid.update(_grid_vertex.data());
	tgt.draw(_grid, 0, _grid.getVertexCount(), states);
}

RenderPoint Map::GetRenderPoint(const TilePoint& tile_pt, const TilePoint& offset)
{
	return {
		((tile_pt.x - offset.x) * tile::kSize) + _half_tile_offset.x,
		((tile_pt.y - offset.y) * tile::kSize) + _half_tile_offset.y
	};
}

void Map::SetUnit(Unit * base)
{
	switch (base->_action)
	{
	case unit::Action::Move:
	{
		//auto last_last_pt = base._last_pt;

		base->_last_pt = base->_pt;
		
		/*Log::Info("XMap::SetUnit: base._last_pt({} {}) changed to ({} {})",
			last_last_pt.x, last_last_pt.y, base._last_pt.x, base._last_pt.y);
			*/
		base->_dir.Move(base->_pt);
		break;
	}

	case unit::Action::Stop:
		base->_last_pt = base->_pt;

		if(g_player)
			g_player->_next_pt = base->_pt;

		base->_wpt = GetWorldPoint(base->_pt);
		base->_mvpt = base->_wpt.As<float>();
		base->_mvpt_decrease = { 0, 0 };
		base->_is_moving = false;
		break;

	default:

		break;
	}

	// #todo
	if (!IsValidPoint(base->_pt))
	{
		PrintError("Map::SetUnit::!IsValidPoint {}", Format(base->_pt));
		return;
	}

	// What happens if the unit is being set as non-Stop and was never registered?
	// 
	//auto tile = GetTile(base->_pt - g_map->_visible_tile_offset.As<TilePointValue>());
	auto tile = GetTile(base->_pt);
	if (!tile)
	{
		PrintError("Map::SetUnit::if(!tile) {}", Format(base->_pt));
		return;
	}

	PrintDebug("SetUnit tile pos: {}", Format(base->_pt));

	// #todo check that the correct tile is being set to unit and rendered
	static auto cnt = 0;
	PrintError("tile->_unit = base CNT({})", ++cnt);

	if (auto it = find_if(_units, [&](auto * u)
	{
		return u->_idx == base->_idx;
	}); it != _units.end())
	{
		auto * found_unit = *it;

		

		// Updates Unit
		if (g_player->_idx == found_unit->_idx)
		{
			{
				auto new_pt = found_unit->_pt;
				found_unit->_pt = found_unit->_last_pt;
				UnsetUnit(found_unit);
				found_unit->_pt = new_pt;
			}

			(unit::Base&)(*g_player.get()) = *base;

			if (found_unit->_action != unit::Action::Stop)
				found_unit->_wpt = GetWorldPoint(found_unit->_pt);
		}
		else
		{
			UnsetUnit(found_unit);
			(unit::Base&)(found_unit) = *base;
		}

		tile->_unit = found_unit;

		if (found_unit->_last_pt != found_unit->_pt)
		{
			found_unit->_is_moving = true;
		}
		else
		{
			UpdateCamera();
		}
	}
	else
	{
		Unit * new_unit;

		if (g_player->_idx == base->_idx)
		{
			g_player->_pt = base->_pt;
			new_unit = g_player.get();
		}
		else
		{
			//_units_storage.push_back(make_unique<unit::Base>(unit));
			new_unit = _units_storage.back().get();
		}

		_units.push_back(new_unit);

		if (g_player->_idx == base->_idx)
		{
			g_player->_next_pt = new_unit->_last_pt = new_unit->_pt;

			new_unit->_wpt = GetWorldPoint(new_unit->_pt);

			new_unit->_mvpt = new_unit->_wpt.As<float>();

			UpdateCamera();
		}

		Print("Added new unit({}) at ({}) wpt({})",
			new_unit->_name.c_str(), Format(new_unit->_pt), Format(new_unit->_wpt)
		);

		tile->_unit = new_unit;
	}
}

void Map::UnsetUnit(Unit * unit)
{
	// #todo
	if (!IsValidPoint(unit->_pt))
	{
		PrintError("Map::SetUnit::!IsValidPoint {}", Format(unit->_pt));
		return;
	}

	auto tile = GetTile(unit->_pt);
	if (!tile)
		return;

	if(!tile->_unit)
	{
		PrintError("Map::UnsetUnit::!tile->_unit not present to unset ({} {})",
			unit->_name.c_str(), Format(unit->_pt)
		);

		return;
	}

	if (tile->_unit != unit)
	{
		PrintError("Map::UnsetUnit: Unit is present ({} {}) but doesn't match owner ({} {})",
			tile->_unit->_name.c_str(), Format(tile->_unit->_pt),
			unit->_name.c_str(), Format(unit->_pt)
		);

		return;
	}

	tile->_unit = nullptr;
}

void Map::Tick()
{
	g_units->Tick();
}

void Map::RenderObjects([[maybe_unused]] sf::RenderTarget & tgt, [[maybe_unused]] const sf::RenderStates & states)
{
	for(auto unit : _units)
	{
		if(!IsInSight(unit->_wpt))
			continue;

		unit->Render(tgt, states);
	}
}

bool Map::IsInSight([[maybe_unused]] WorldPoint wpt)
{
	return true;
}

}
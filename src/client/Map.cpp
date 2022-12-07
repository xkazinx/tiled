#include "stdafx.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"
#include "Player.h"

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
		_visible_tiles = { 
			(g_app->_res_x / tile::kSize),
			(g_app->_res_y / tile::kSize) 
		};

		_visible_tiles_range.x = _visible_tiles.x / 2;
		_visible_tiles_range.y = _visible_tiles.y / 2;

		if((_visible_tiles.x % 2) == 0)
			_half_tile_offset.x = -tile::kHalfSize;
	
		if((_visible_tiles.y % 2) == 0)
			_half_tile_offset.y = -tile::kHalfSize;
	}

	UpdateCamera();
}

Map::Map(InPacket * pkt) : Map()
{
	auto total = pkt->ReadAs<u32>();
	_tiles.reserve(total);

	while(total-- > 0)
	{
		_tiles.push_back(make_unique<Tile>(pkt));
	}
}

void Map::UpdateCamera()
{
	TilePoint player_pt = g_player->_pt;

	_visible_tile_offset.x = player_pt.x - _visible_tiles_range.x;
	_visible_tile_offset.y = player_pt.y - _visible_tiles_range.y;

	_world_offset.x = _visible_tile_offset.x * tile::kSize;
	_world_offset.y = _visible_tile_offset.y * tile::kSize;

	_world_offset_adjust.x = _half_tile_offset.x + _world_offset.x;
	_world_offset_adjust.y = _half_tile_offset.y + _world_offset.y;

	{
		auto rpt = GetRenderPoint(player_pt, { 0, 0 });
		rpt.x -= tile::kHalfSize - 1;
		rpt.y -= tile::kHalfSize - 1;

		rpt.x += (player_pt.x * tile::kSize) - rpt.x;
		rpt.y += (player_pt.x * tile::kSize) - rpt.x;

		MovePoint mvpt = { (float)rpt.x, (float)rpt.y}; // #todo g_player->_mvpt
		_world_offset_movement = { 0, 0 };
		_world_offset_movement.x -= RenderPointValue(mvpt.x - rpt.x);
		_world_offset_movement.y -= RenderPointValue(mvpt.y - rpt.y);

		auto player_is_moving = false; // #todo g_player->_is_moving
		if (player_is_moving)
		{
			MovePoint mvpt_decrease = {0, 0}; // todo g_player->_mvpt_decrease
			_world_offset_movement.x -= (RenderPointValue)round(mvpt_decrease.x);
			_world_offset_movement.y -= (RenderPointValue)round(mvpt_decrease.y);
		}
	}
}

Map::~Map()
{

}

void Map::Render(sf::RenderTarget& tgt, const sf::RenderStates& states)
{
	szt i;
	RenderPoint pt;

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

	sf::Vector2<i16> index;
	for(i = 0,
		index.y = _visible_tile_offset.y,
		pt.y = -tile::kSize + (_world_offset_adjust.y - _world_offset.y) + _world_offset_movement.y;
		pt.y <= g_app->_res_y + extra_tile_sz;
		pt.y += tile::kSize, ++index.y)
	{
		for(index.x = _visible_tile_offset.x,
			pt.x = -tile::kSize + (_world_offset_adjust.x - _world_offset.x) + _world_offset_movement.x;
			 pt.x <= g_app->_res_x + extra_tile_sz;
			 pt.x += tile::kSize, ++index.x, ++i)
		{
			if(_grid_visible)
			{
				auto v = &_grid_vertex[i * 8];
				auto px = float(pt.x);
				auto py = float(pt.y);
				if (v[0].position != sf::Vector2f(px, (float)pt.y))
				{
					v[0].position = sf::Vector2f(px, py);
					v[1].position = sf::Vector2f(px + tile::kSize, py);
					v[2].position = sf::Vector2f(px + tile::kSize, py);
					v[3].position = sf::Vector2f(px + tile::kSize, py + tile::kSize);
					v[4].position = sf::Vector2f(px + tile::kSize, py + tile::kSize);
					v[5].position = sf::Vector2f(px, py + tile::kSize);
					v[6].position = sf::Vector2f(px, py + tile::kSize);
					v[7].position = sf::Vector2f(px, py);

					auto color = sf::Color(255, 255, 255, 255 / 6);
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
	// #todo
	if(!IsValidPoint(base->_pt))
	{
		PrintError("Map::SetUnit::!IsValidPoint {}", Format(base->_pt));
		return;
	}

	auto tile = GetTile(base->_pt - g_map->_visible_tile_offset.As<TilePointValue>());
	if(!tile)
	{
		PrintError("Map::SetUnit::if(!tile) {}", Format(base->_pt));
		return;
	}

	tile->_unit = base;

	if (auto it = find_if(_units, [&](auto * u)
	{
		return u->_idx == base->_idx;
	}); it != _units.end())
	{
		auto * found_unit = *it;

		// Updates Unit
		if (g_player->_idx == found_unit->_idx)
		{
			(unit::Base&)(*g_player.get()) = *base;

			if (found_unit->_action != unit::Action::Stop)
				found_unit->_wpt = GetWorldPoint(found_unit->_pt);
		}
		else
		{
			(unit::Base&)(found_unit) = *base;
		}

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
			new_unit->_last_pt = new_unit->_pt;

			new_unit->_wpt = GetWorldPoint(new_unit->_pt);

			new_unit->_mvpt = new_unit->_wpt.As<float>();

			UpdateCamera();
		}

		Print("Added new unit({}) at ({}) wpt({})",
			new_unit->_name.c_str(), Format(new_unit->_pt), Format(new_unit->_wpt)
		);
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
			unit->_name, Format(unit->_pt)
		);

		return;
	}

	if (tile->_unit != unit)
	{
		PrintError("Map::UnsetUnit: Unit is present ({} {}) but doesn't match owner ({} {})",
			tile->_unit->_name, Format(tile->_unit->_pt),
			unit->_name, Format(unit->_pt)
		);

		return;
	}

	tile->_unit = nullptr;
}

void Map::Tick()
{
	if(auto * unit = g_player.get())
	{
		if (!unit->_is_moving && unit->_pt != unit->_next_pt)
		{
			PrintDebug("Map::Tick(): Point: {} | NextPoint: {}",
				Format(unit->_pt), Format(unit->_next_pt)
			);

			if (g_player->_stop_dir != Direction::None)
			{
				g_player->_dir = g_player->_stop_dir;
				g_player->_stop_dir = Direction::None;

				g_player->_action = unit::Action::Stop;

				SetUnit(g_player.get());
			}
			else
			{
				/*Direction dir = _g->GetNextMoveDir(
					unit->GetPoint(), unit->_next_pt,
					_g->_illusion_mov && !_g->IsAction(Action::DamageMove)
				);

				if (!dir.IsValid())
					return;

				unit->_dir = dir;
				unit->_action = unit::Action::Move;

				SetUnit(g_player.get());*/
			}
		}
	}
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
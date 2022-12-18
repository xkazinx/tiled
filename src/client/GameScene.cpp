#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"
#include "Player.h"

namespace Client
{
GameScene::GameScene() 
	: _game_view({ 0.f, 0.f, (float)g_app->_res_x, (float)g_app->_res_y })
{
	g_game = this;

	PrintDebug("Initialized GameScene with player name {}", g_player->_name);
}

GameScene::~GameScene()
{
	g_game = nullptr;
}

void GameScene::Tick()
{
	TilePoint tile_dst = GetMouseTile();

	if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		PrintDebug("if(sf::Mouse::isButtonPressed(sf::Mouse::Left))");
		if (g_player->_pt == tile_dst)
			return;

		if (g_player->_is_moving)
			return;

		auto dir = g_player->GetNextDir(
			g_player->_pt, tile_dst
		);

		if (dir == Direction::None)
		{
			PrintError("Game::HandleClicks: return: next direction is invalid with pos' {} against {}",
				Format(g_player->_pt), Format(tile_dst));
			return;
		}

		PrintDebug("Left Click {} {}", tile_dst.x, tile_dst.y);

		g_player->_next_pt = tile_dst;

		g_player->_dir = dir;

		g_player->_action = unit::Action::Move;

		g_map->SetUnit(g_player.get());

		//SendMotion(GetAction(), dir);
		//_p->UpdateUnit(GetAction());
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		auto dir = Direction::GetNext(g_player->_pt, tile_dst);

		if (dir == Direction::None)
			return;

		if (g_player->_is_moving)
		{
			g_player->_stop_dir = dir;
			return;
		}

		g_player->_dir = dir;

		g_player->_action = unit::Action::Stop;

		g_map->SetUnit(g_player.get());
	}

	g_map->Tick();
}

void GameScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states)
{
	auto & default_view = g_app->_window.getDefaultView();

	tgt.setView(_game_view);

	{
		if(g_map)
			g_map->Render(tgt, states);

		if(g_map)
			g_map->RenderObjects(tgt, states);
	}

	tgt.setView(default_view);

	{
		auto mp = GetMouseTile();
		auto mpos = sf::Mouse::getPosition(g_app->_window);
		auto pos = g_app->_window.mapPixelToCoords(mpos);
		font::Fixed({
			(RenderPointValue)pos.x,
			(RenderPointValue)pos.y + 15
			},
			
			format("mouse_pos {}\n"
				"g_player->_pt {}\n"
				"g_player->_last_pt {}\n"
				"g_player->_next_pt {}\n"
				"_visible_tiles {}\n"
				"_visible_tiles_range {}\n"
				"_visible_tile_offset {}\n"
				"_world_offset {}\n"
				"_world_offset_adjust {}\n"
				"_world_offset_movement {}\n"
				"_half_tile_offset {}\n",
				Format(mp),
				Format(g_player->_pt),
				Format(g_player->_last_pt),
				Format(g_player->_next_pt),
				Format(g_map->_visible_tiles), 
				Format(g_map->_visible_tiles_range), 
				Format(g_map->_visible_tile_offset),
				Format(g_map->_world_offset), 
				Format(g_map->_world_offset_adjust), 
				Format(g_map->_world_offset_movement), 
				Format(g_map->_half_tile_offset)),
			
			sf::Color::White);
	}
}

bool GameScene::HandleEvents([[maybe_unused]] const sf::Event & ev)
{
	if(ev.type == sf::Event::KeyPressed)
	{
		switch(ev.key.code)
		{
			using enum sf::Keyboard::Key;
		case Subtract:
			PrintDebug("SUBSTRACT!");
			_game_view.zoom(1.1f);
			break;

		case Add:
			_game_view.zoom(0.9f);
			break;

		default:
			return false;
		}
	}

	return true;
}

TilePoint GameScene::GetMouseTile()
{
	auto mp = g_app->_window.mapPixelToCoords(sf::Mouse::getPosition(g_app->_window));

	TilePoint tile = {
		TilePointValue(mp.x / tile::kSize),
		TilePointValue(mp.y / tile::kSize)
	};

	if (g_map->_half_tile_offset.x != 0 &&
		(mp.x - tile.x * tile::kSize) > tile::kHalfSize)
	{
		++tile.x;
	}

	if (g_map->_half_tile_offset.y != 0 &&
		(mp.y - tile.y * tile::kSize) > tile::kHalfSize)
	{
		++tile.y;
	}

	return tile + g_map->_visible_tile_offset.As<TilePointValue>();
}
}
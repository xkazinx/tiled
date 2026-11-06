#pragma once

namespace Client
{
struct Unit : unit::Base
{
	Unit();
	Unit(const unit::Base * base);

	void Render(sf::RenderTarget & tgt, const sf::RenderStates & states);

	WorldPoint _wpt;
	TilePoint _last_pt;
	MovePoint _mvpt;

	bool _is_moving = false;

	Direction _stop_dir = Direction::None;

	// #todo maybe move to Player?
	MovePoint _mvpt_decrease = { 0.f, 0.f };
};
}

#pragma once

namespace Client
{
class Unit : public unit::Base
{
public:
	Unit();
	Unit(const unit::Base * base);

	void Render(sf::RenderTarget & tgt, const sf::RenderStates & states);

	WorldPoint _wpt;
	TilePoint _last_pt;
	MovePoint _mvpt;

	bool _is_moving = false;

	// #todo maybe move to Player?
	MovePoint _mvpt_decrease = {0.f, 0.f};
};
}
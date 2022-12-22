#include "stdafx.h"
#include "Unit.h"
#include "Map.h"

namespace Client
{

Unit::Unit()
{

}

void Unit::Render([[maybe_unused]] sf::RenderTarget & tgt, [[maybe_unused]] const sf::RenderStates & states)
{
	constexpr auto sz = 16.f;
	static sf::RectangleShape shape;
	shape.setFillColor(sf::Color::Red);
	shape.setSize({ sz, sz });

	RenderPoint extra{ 0, 0 };
	if (_is_moving && _last_pt != _pt && _action == unit::Action::Move)
	{
		Direction dir = _dir.GetNext(_last_pt, _pt);
		auto delta = dir.GetDelta();
		extra = delta.As<decltype(extra.x)>() * tile::kSize;

		auto dst_wpt = _wpt - (dir.GetContrary().GetDelta().As<decltype(_wpt.x)>() * tile::kHalfSize);

		const auto velocity = 64.0f;

		//auto delta = Clock.delta();
		auto delta_clock = time::DeltaFloat();

		auto new_mvpt = _mvpt.Project(
			dst_wpt.As<float>(), velocity * delta_clock
		);

		auto mvpt_diff = new_mvpt - _mvpt;
		_mvpt_decrease += mvpt_diff / 2;

		TilePoint offset = { 0, 0 };
		auto rpt = g_map->GetRenderPoint(_pt, offset);
		rpt -= delta.As<decltype(rpt.x)>() * (tile::kHalfSize - 1);
		rpt += ((_pt.As<decltype(rpt.x)>() * tile::kSize) - rpt).As<decltype(rpt.x)>();
		rpt -= _mvpt_decrease.As<decltype(rpt.x)>();

		shape.setSize({ 2.f, 2.f });

		{
			auto dst = RenderPoint(
				(RenderPointValue)round(rpt.x),
				(RenderPointValue)round(rpt.y)
			);

			dst -= g_map->_world_offset + g_map->_half_tile_offset.Abs();
			dst += extra;
			shape.setPosition({ (float)dst.x, (float)dst.y });
		}

		tgt.draw(shape);

		shape.setSize({ sz, sz });

		_mvpt = new_mvpt;

		if (delta.x > 0)
		{
			if (new_mvpt.x >= rpt.x)
				_mvpt.x = rpt.x;
		}
		else if (delta.x < 0)
		{
			if (new_mvpt.x <= rpt.x)
				_mvpt.x = rpt.x;
		}

		if (delta.y > 0)
		{
			if (new_mvpt.y >= rpt.y)
				_mvpt.y = rpt.y;
		}
		else if (delta.y < 0)
		{
			if (new_mvpt.y <= rpt.y)
				_mvpt.y = rpt.y;
		}

		if (_mvpt == rpt.As<float>())
		{
			PrintDebug("Object reached next tile destination");

			_last_pt			= _pt;
			_is_moving		= false;
			_wpt				= _wpt;
			_mvpt				= _wpt.As<float>();
			_mvpt_decrease = { 0, 0 };

			extra = { 0, 0 };
		}
	}

	if ((Unit*)g_player.get() == this)
	{
		g_map->UpdateCamera();
	}
	auto dst = RenderPoint(
		(RenderPointValue)round(_mvpt.x - (_mvpt_decrease.x * 2)),
		(RenderPointValue)round(_mvpt.y - (_mvpt_decrease.y * 2))
	);

	auto pre_dst = dst;
	dst -= g_map->_world_offset + g_map->_half_tile_offset.Abs();
	dst += extra;

	shape.setPosition(
		dst.x
		+ (tile::kHalfSize) - (sz / 2.f)
		,
		dst.y
		+ (tile::kHalfSize) - (sz / 2.f)
	);

	/*
	Print("RENDERING! {} | g_map->_world_offset: {} | g_map->_half_tile_offset: {} | PlayerPos: {}", 
		Format(dst), Format(g_map->_world_offset), Format(g_map->_half_tile_offset), Format(_pt));
		*/
	tgt.draw(shape);
}
}

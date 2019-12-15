#include "stdafx.h"
#include "Background.h"
#include "Random.h"
#include "Camera.h"

void Background::Render(TargetStatesRef && ts) const
{
	ts().draw(_stars, ts.states);
	ts().draw(_transition_stars, ts.states);
}

Background::Background(const size_t & stars_count, const Vector2f & size)
	: _stars(stars_count, size),
	_transition_stars(200, Vector2f(g_res_x, size.y))
{
	_transition_stars.setPosition({ size.x, 0.f });
}

Background::~Background()
{
}

void Background::Update(const sf::Time & delta)
{
	if (g_camera->GetLeft() >= _stars.getPosition().x + _stars._size.x)
		_stars.move({ _stars._size.x + _transition_stars._size.x, 0.f });

	if(g_camera->GetRight() >= _transition_stars.getPosition().x + 2 * _transition_stars._size.x)
		_transition_stars.move({ _stars._size.x + _transition_stars._size.x, 0.f });
}

Stars::Stars(const size_t & count, const Vector2f & size) : _size(size)
{
	_list.resize(count);

	ForEach([&](auto & star) {
		star.color = sf::Color::White;

		star.position = Vector2f{
			g_rand->Generate<float>(0.f, _size.x),
			g_rand->Generate<float>(0.f, _size.y)
		};
	});
}

void Stars::Update(const sf::Time & delta)
{
}

void Stars::Render(TargetStatesRef && ts) const
{
	ts().draw(&_list[0], _list.getVertexCount(), sf::PrimitiveType::Points, ts.states);
}

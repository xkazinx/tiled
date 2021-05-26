#include "stdafx.h"
#include "PDot.h"
#include "Camera.h"
#include <math.h>

PDot::PDot(const Vector2f& pos, const Vector2f& end_pos) :
	Object(pos, ObjectID::PDot),
	_initial_pos(pos), _end_pos(end_pos)
{
	_shape.setRadius(3.f);
	_shape.setFillColor(sf::Color::Red);
}

void PDot::Render(TargetStatesRef&& ts) const
{
	ts().draw(_shape, ts.states);
}

void PDot::Update(const sf::Time& delta)
{
	auto x_vel = .50f;
	auto y_vel = .50f;
	float x_diff, y_diff;
	if (_initial_pos.x > _end_pos.x)
	{
		x_vel *= -1.f;
		x_diff = _initial_pos.x - _end_pos.x;
	}
	else
	{
		x_diff = _end_pos.x - _initial_pos.x;
	}
	x_diff = _initial_pos.x - _end_pos.x;
	if (_initial_pos.y > _end_pos.y)
	{
		y_vel *= -1.f;
		y_diff = _initial_pos.y - _end_pos.y;
	}
	else
	{
		y_diff = _end_pos.y - _initial_pos.y;
	}
	y_diff = _initial_pos.y - _end_pos.y;
	/*
	auto angle = atan2(x_diff, y_diff);

	auto magnitude = 1.0f;
	float velX = cos(angle) * magnitude;
	float velY = sin(angle) * magnitude;*/

	/*auto angle = atan2(x_diff, y_diff);
	auto magnitude = 1.0f;
	float velX = cos(angle) * magnitude;
	float velY = sin(angle) * magnitude;
	move(velX * delta.asMilliseconds(), velY * delta.asMilliseconds());*/
}

Object::Collider PDot::GetCollider() const
{
	auto & pos = getPosition();
	auto rect = _shape.getGlobalBounds();
	return sf::FloatRect(rect.left + pos.x, rect.top + pos.y, rect.width, rect.height);
}

void PDot::HandleCollition(const Object * object) {
	if (object->Is(ObjectID::Player))
	{
		Expire();
	}
}

bool PDot::_HasExpired() const
{
	return getPosition().x <= g_camera->GetLeft() || getPosition().x >= g_camera->GetRight();
}
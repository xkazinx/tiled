#include "stdafx.h"
#include "PNormal.h"

PNormal::PNormal(const Vector2f& pos) :
	Object(pos, ObjectID::PNormal), 
	_initial_pos(pos)
{
	
	_shape.setSize({ 15.f, 5.f });
	_shape.setFillColor(sf::Color::Green);
}

void PNormal::Render(TargetStatesRef&& ts) const
{
	ts().draw(_shape, ts.states);
}

void PNormal::Update(const sf::Time& delta)
{
	auto velocity = .50f;
	move(0.05f + velocity * delta.asMilliseconds(), 0.f);
}

Object::Collider PNormal::GetCollider() const
{
	auto & pos = getPosition();
	auto rect = _shape.getGlobalBounds();
	return sf::FloatRect(rect.left + pos.x, rect.top + pos.y, rect.width, rect.height);
}

void PNormal::HandleCollition(const Object * object) {
	if (object->Is(ObjectID::Enemy))
	{
		Expire();
	}
}

bool PNormal::_HasExpired() const
{
	return getPosition().x - _initial_pos.x >= g_res_x;// 400.f;
}
#include "stdafx.h"
#include "Projectile.h"
#include "Enemy.h"

Projectile::Projectile(const Vector2f & pos) 
	: Object(pos, ObjectID::Projectile), _shape({ 15.f, 5.f }), _initial_pos(pos)
{
	_shape.setFillColor(sf::Color::Green);
}

Projectile::~Projectile()
{

}

void Projectile::Render(TargetStatesRef && ts) const
{
	ts().draw(_shape, ts.states);
}

void Projectile::Update(const sf::Time & delta)
{
	auto velocity = .15f;
	move(0.05f + velocity * delta.asMilliseconds(), 0.f);
}

bool Projectile::_HasExpired() const
{
	return getPosition().x - _initial_pos.x > 400.f;
}

void Projectile::HandleCollition(const Object * object) {
	if (object->Is(ObjectID::Enemy)) 
	{
		Expire();
	}
}
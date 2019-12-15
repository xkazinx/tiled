#include "stdafx.h"
#include "Enemy.h"
#include "Camera.h"

Enemy::Enemy(const Vector2f & pos) 
	: Object(pos, ObjectID::Enemy), _shape({ 30.f, 30.f })
{
	_shape.setFillColor(sf::Color::Yellow);
	_shape.setOrigin(_shape.getSize().x / 2.f, _shape.getSize().y / 2.f);
}

Enemy::~Enemy()
{

}

void Enemy::Update(const sf::Time & delta)
{
	auto velocity = 0.05f;
}

Object::Collider Enemy::GetCollider() const
{
	auto & pos = getPosition();
	auto rect = _shape.getGlobalBounds();
	return sf::FloatRect(rect.left + pos.x, rect.top + pos.y, rect.width, rect.height);
}

bool Enemy::_HasExpired() const {
	return getPosition().x < g_camera->GetLeft() + 400.f;
}

void Enemy::HandleCollition(const Object * object)
{
	if (object->Is(ObjectID::Projectile)) {
		Expire();
	}
}
